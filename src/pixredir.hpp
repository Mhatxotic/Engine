/* == PIXREDIR.HPP ========================================================= **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## This is a POSIX specific module that handles redirecting a standard ## **
** ## output to the engine logger. It should not be used in debug mode.   ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace ISysRedirect {               // Start of private module namespace
/* ------------------------------------------------------------------------- */
using namespace IError::P;             using namespace ILog::P;
using namespace IMemory::P;            using namespace IStd::P;
using namespace ISysUtil::P;           using namespace IThread::P;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* ------------------------------------------------------------------------- */
class SysRedirect final :              // Rediector
  /* -- Base classes ------------------------------------------------------- */
  protected Thread,                    // Thread to monitor file descriptor
  public Memory                        // Storage for read text data
{ /* -- Private variables -------------------------------------------------- */
  constexpr static int iInvalid = -1;  // Handle is invalid value
  /* ----------------------------------------------------------------------- */
  const int      iRequested;           // Requested handle
  /* -- Handles for pipe() function ---------------------------------------- */
  using PipeHandles = StdArray<int, 2>; // Handles struct (for pipe())
  PipeHandles    phHandles;            // Pipe handles (for pipe())
  /* -- Order is important ------------------------------------------------- */
  int            iSaved,               // Saved (ahHandles[0])
                &iWrite,               // Write (ahHandles[1]/phHandles[1])
                &iRead;                // Read (ahHandles[2]/phHandles[0])
  /* -- All handles so we can close them all together ---------------------- */
  using AllHandles = StdArray<StdRefWrapper<int>, 3>; // StdArray<int&,3>
  AllHandles    ahHandles;             // All handles (iSaved, iWrite, iRead)
  /* -- Async off-main thread function ------------------------------------- */
  ThreadStatus ThreadMain(Thread&)
  { // Until thread should exit or end of file
    while(ThreadShouldNotExit())
    { // Read some data and if we got some? Return how much we read
      switch(const size_t stRead = static_cast<size_t>
        (read(iRead, MemPtr(), MemSize())))
      { // Success?
        default:
        { // Report read string to log
          cLog->LogWarningExSafe("$<$>: $",
            NameGet(), stRead, MemToStringViewSafe(stRead));
          // Fallthrough to break
          [[fallthrough]];
        } // Handle was closed? Terminate the thread
        case 0: break;
        // Error?
        case StdMaxSizeT:
        { // Ignore it if we're quitting
          if(ThreadShouldNotExit()) break;
          // Else throw error and terminate thread
          XCS("Error reading system output from pipe!",
            "Output", NameGet(), "Bytes", MemSize());
        }
      } // Don't get here
    } // Return success to thread manager
    return TS_OK;
  }
  /* ----------------------------------------------------------------------- */
  void CloseAndReset(int &iHandle) { close(iHandle); iHandle = iInvalid; }
  /* ----------------------------------------------------------------------- */
  void Reset()
  { // Thread is running? Signal the exit
    if(ThreadIsJoinable()) ThreadSetExit();
    // Restoring the original FD with dup2(iSaved, iRequested) will replace
    // the current iRequested fd (which points at the pipe's write end) with
    // the original saved fd. As a side-effect dup2 closes the old iRequested
    // (the pipe writer) which allows the reader to observe EOF.
    if(iSaved != iInvalid && dup2(iSaved, iRequested) == iInvalid)
      cLog->LogWarningExSafe(
        "System failed to restore dup2 oldf $ to $ for $! $",
        iSaved, iRequested, NameGet(), SysError());
    // Now close the read end to ensure the reader's read() either sees EOF
    // or gets EBADF.
    if(iRead != iInvalid) CloseAndReset(iRead);
    // Join the reader thread
    ThreadJoin();
  }
  /* ----------------------------------------------------------------------- */
  void CloseAll()
  { // Close all handles
    StdForEach(seq, ahHandles.begin(), ahHandles.end(), [this](int &iHandle)
      { if(iHandle != iInvalid) CloseAndReset(iHandle); });
  }
  /* -- Shut down stderr reader ------------------------------------ */ public:
  void ResetSafe() { Reset(); CloseAll(); }
  /* ----------------------------------------------------------------------- */
  SysRedirect(const int iHandle, const StdString &strNName) :
    /* --------------------------------------------------------------------- */
    Name{ strNName },                // Initialise identifier
    Thread{ STP_LOW },               // Initialise low priority thread
    Memory{ 4096 },                  // Initialise buffer
    iRequested(iHandle),             // Store requested handle
    phHandles{ iInvalid, iInvalid }, // Initialise pipe handles references
    iSaved(dup(iRequested)),         // Duplicate requested handle
    iWrite(phHandles[1]),            // Init reference to write pipe
    iRead(phHandles[0]),             // Init reference to read pipe
    ahHandles{ iSaved,iWrite,iRead } // Initialise all handles references
    /* --------------------------------------------------------------------- */
  { // Return if handle not copied
    if(iSaved == iInvalid)
    { // Write warning to log
      cLog->LogWarningExSafe("System failed to dup fd $ for $! $",
        iRequested, NameGet(), SysError());
      // Do not run the reader thread
      return;
    } // Create pipe handles and if failed?
    if(pipe(phHandles.data()))
    { // Need to close saved handle
      CloseAndReset(iSaved);
      // Write warning to log
      cLog->LogWarningExSafe("System pipe call failed for $! $",
        NameGet(), SysError());
      // Do not run the reader thread
      return;
    } // SysRedirect requested output handle to the pipe and if failed?
    if(dup2(iWrite, iHandle) == iInvalid)
    { // Close and reset all handles
      CloseAll();
      // Write warning to log
      cLog->LogWarningExSafe(
        "System failed to dup2 oldfd $ to fd $ for $! $",
        iWrite, iHandle, NameGet(), SysError());
      // Do not run the reader thread
      return;
    } // Close the original write-end returned by pipe(); the duplicate now
    // lives at iRequested and we must not hold the original iWrite,
    // otherwise the reader never sees EOF.
    CloseAndReset(iWrite);
    // Start the monitoring thread if it is open
    ThreadInit(bind(&SysRedirect::ThreadMain, this, _1), this);
  }
  /* ----------------------------------------------------------------------- */
  ~SysRedirect()
  { // Terminate the reader thread
    Reset();
    // Close all handles with no need to reset handle values
    StdForEach(seq, ahHandles.cbegin(), ahHandles.cend(),
      [](const int iHandle) { if(iHandle != iInvalid) close(iHandle); });
  }
};/* ----------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
