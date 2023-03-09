/* == PIXPIP.HPP =========================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## This is a POSIX specific module that will assist in executing a new ## **
** ## process and capturing its output into a buffer. Since this means    ## **
** ## MacOS and Linux, we can support both systems very easily.           ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* == System pipe class ==================================================== */
class SysPipe :
  /* -- Base classes ------------------------------------------------------- */
  public SysPipeBase                   // Base SysPipe class
{ /* ----------------------------------------------------------------------- */
  unsigned int     uiPid;              // Readable process id
  pid_t            pPid;               // Internal process id
  /* -- Types -------------------------------------------------------------- */
  enum Handle : size_t { READ_HANDLE, WRITE_HANDLE, MAX_HANDLE };
  /* ----------------------------------------------------------------------- */
  typedef array<int,MAX_HANDLE> HandlesArray; // Handles array type
  /* ----------------------------------------------------------------------- */
  class Handles :
    /* --------------------------------------------------------------------- */
    private HandlesArray               // Handles (stdout/stdin)
  { /* -- Return handles array class --------------------------------------- */
    HandlesArray &GetHandles(void) { return *this; }
    /* -- Return reference to fd at specified location ------------- */ public:
    int &GetFd(const Handle hFd) { return GetHandles()[hFd]; }
    /* -- Return reference to read fd -------------------------------------- */
    int &GetReadFd(void) { return GetFd(READ_HANDLE); }
    /* -- Close fd and reset it -------------------------------------------- */
    void CloseFd(int &iFd) { if(iFd != -1) { close(iFd); iFd = -1; } }
    /* -- Close read fd and reset it --------------------------------------- */
    void CloseReadFd(void) { CloseFd(GetReadFd()); }
    /* -- Duplicate the read fd handle ------------------------------------- */
    void DupeReadFd(const int iFd) { dup2(GetReadFd(), iFd); }
    /* --------------------------------------------------------------------- */
    bool IsReadFd(const int iFd) { return GetReadFd() == iFd; }
    /* --------------------------------------------------------------------- */
    bool IsNotReadFd(const int iFd) { return !IsReadFd(iFd); }
    /* --------------------------------------------------------------------- */
    bool IsValidReadFd(void) { return IsNotReadFd(-1); }
    /* --------------------------------------------------------------------- */
    bool IsNotValidReadFd(void) { return !IsValidReadFd(); }
    /* --------------------------------------------------------------------- */
    int &GetWriteFd(void) { return GetFd(WRITE_HANDLE); }
    /* --------------------------------------------------------------------- */
    void CloseWriteFd(void) { CloseFd(GetWriteFd()); }
    /* --------------------------------------------------------------------- */
    void DupeWriteFd(const int iFd) { dup2(GetWriteFd(), iFd); }
    /* --------------------------------------------------------------------- */
    bool IsWriteFd(const int iFd) { return GetWriteFd() == iFd; }
    /* --------------------------------------------------------------------- */
    bool IsNotWriteFd(const int iFd) { return !IsWriteFd(iFd); }
    /* --------------------------------------------------------------------- */
    bool IsValidWriteFd(void) { return IsNotWriteFd(-1); }
    /* --------------------------------------------------------------------- */
    bool IsNotValidWriteFd(void) { return !IsValidWriteFd(); }
    /* --------------------------------------------------------------------- */
    void CloseHandles(void) { CloseReadFd(); CloseWriteFd(); }
    /* -- Return new IPC pipe handles -------------------------------------- */
    bool GenerateHandles(void) { return !pipe(data()); }
    /* -- Constructor ------------------------------------------------------ */
    Handles(void) :
      /* -- Initialisers --------------------------------------------------- */
      HandlesArray{ -1, -1 }           // Initially set to error status
      /* -- No code -------------------------------------------------------- */
      { }
    /* --------------------------------------------------------------------- */
    ~Handles(void) { CloseHandles(); }
  };/* --------------------------------------------------------------------- */
  Handles          haChildToParent;    // Child-to-parent handles
  Handles          haParentToChild;    // Parent-to-child handles
  /* -- Kill an wait for pid ----------------------------------------------- */
  int DeInit(void)
  { // Return result
    int iResult;
    // If we have a spawned process?
    if(pPid)
    { // Kill the pid and if failed? Write failure to log
      iResult = kill(pPid, SIGTERM);
      if(iResult)
        cLog->LogWarningExSafe("System failed to send SIGTERM to pid $: $!",
          pPid, StrFromErrNo());
      // Kill succeeded? Write success to log
      else cLog->LogDebugExSafe(
        "System sent SIGTERM to pid $ with result $<$$>.",
          pPid, SysPipeBaseGetStatus(), hex, SysPipeBaseGetStatus());
      // Wait for the pid to terminate
      do pPid = waitpid(pPid, &iResult, 0);
        while(pPid == -1 && StdIsError(EINTR));
      // Write error if there is one
      if(pPid == -1)
        cLog->LogWarningExSafe("System failed to wait for pid $: $!", pPid,
          StrFromErrNo());
      // Set return value
      else SysPipeBaseSetStatus(static_cast<int64_t>(WEXITSTATUS(iResult)));
    } // Return failure
    else iResult = -1;
    // Return failure
    return iResult;
  }
  /* -- Initialise arguments list ------------------------------------------ */
  void InitArgs(const string &strCmdLine, const Args &aList,
    const ValidType vtId)
  { // Get the program filename and check it
    const string &strApp = aList.front();
    switch(const ValidResult vrResult = DirValidName(strApp, vtId))
    { // Good result? Skip to next task
      case VR_OK: break;
      // Anything else and we need to throw an exception
      default: XC("Executable name is invalid!",
                  "Program", strApp, "Code", vrResult,
                  "Reason",  cDirBase->VNRtoStr(vrResult));
    } // Make sure the executable exists
    if(!DirIsFileExecutable(strApp))
      XCS("Executable not valid!", "Program", strApp);
    // This is the arguments list, it will be allocated manually. We need the
    // extra one pointer for nullptr which is the last argument.
    typedef vector<char*> CharVector;
    CharVector cvArgs{ aList.size() + 1 };
    // Enumerate through strings
    char**const cpaArgV = cvArgs.data(), **cpaArgVPtr = cpaArgV;
    for(const string &strArg : aList)
    { // Set address of argument string
      *cpaArgVPtr = const_cast<char*>(strArg.c_str());
      // Goto next argument to write to
      ++cpaArgVPtr;
    } // De-init existing process
    Finish();
    // Show command and arguments
    cLog->LogDebugExSafe("System opening pipe to '$' with $ args '$'.",
      strApp, aList.size(), strCmdLine);
    // Generate child-to-parent handles and throw error on failure
    if(!haChildToParent.GenerateHandles())
      XCS("Failed to create child-to-parent pipe handles for process!",
          "CmdLine", strCmdLine, "Program", strApp);
    // Generate parent-to-child handles and throw error on failure
    if(!haParentToChild.GenerateHandles())
      XCS("Failed to create parent-to-child pipe handles for process!",
          "CmdLine", strCmdLine, "Program", strApp);
    // Fork the process and get result and compare it
    switch(const pid_t pForkedPid = fork())
    { // Success? This is the child process routine. Remember that the child
      // process has it's own copy memory but the same system handles.
      case 0:
      { // C2P[WRITE_HANDLE] is where we write output and it is the new
        // standard output. C2P[READ_HANDLE] can be closed.
        haChildToParent.DupeWriteFd(STDOUT_FILENO);
        haChildToParent.DupeWriteFd(STDERR_FILENO);
        haChildToParent.CloseReadFd();
        // P2C[READ_HANDLE] is where we read input from and it is the new
        // standard input. P2C[WRITE_HANDLE] can be closed.
        haParentToChild.DupeReadFd(STDIN_FILENO);
        haParentToChild.CloseWriteFd();
        // Execute the program requested
        execve(*cpaArgV, cpaArgV, cCmdLine->GetCEnv());
        // Error so exit process NOW or see uncontrolled fireworks.
        _exit(127);
      } // Success? This is the parent process routine
      default:
      { // Read the output of the child from C2P[READ_HANDLE]
        // We don't need C2P[WRITE_HANDLE] so close it.
        haChildToParent.CloseWriteFd();
        // Write output to the child using P2C[WRITE_HANDLE]
        // We don't need P2C[READ_HANDLE] so close it.
        haParentToChild.CloseReadFd();
        // Set pid
        pPid = pForkedPid;
        uiPid = static_cast<unsigned int>(pForkedPid);
        // Store name of executable
        IdentSet(StdMove(strApp));
        // Report pid
        cLog->LogInfoExSafe("System forked '$' to pid $.", strApp, pPid);
        // Done
        break;
      } // Error? Bail out
      case -1: XCS("Failed to fork for piped process!",
                   "CmdLine", strCmdLine, "Program", strApp);
    }
  }
  /* -- DeInit pipe ------------------------------------------------ */ public:
  void Finish(void)
  { // Kill and wait for pid
    DeInit();
    // Pid now invalid
    pPid = 0;
    // Close handles
    haChildToParent.CloseHandles();
    haParentToChild.CloseHandles();
  }
  /* -- Constructor with init ---------------------------------------------- */
  void Init(const string &strCmdLine, const ValidType vtId=VT_UNTRUSTED)
    { if(const Args aList{ strCmdLine }) InitArgs(strCmdLine, aList, vtId); }
  /* -- Finished sending --------------------------------------------------- */
  void SendFinish(void)
  { // Ignore if write handle already closed
    if(haParentToChild.IsNotValidWriteFd()) return;
    // Close unused write handle
    haParentToChild.CloseWriteFd();
  }
  /* -- Read data ---------------------------------------------------------- */
  size_t Read(void*const vpDest, size_t stToRead)
  { // Send finish if stdin still open
    if(haParentToChild.IsValidWriteFd()) SendFinish();
    // Send finish if stdin still open
    if(haChildToParent.IsNotValidReadFd()) return 0;
    // Read some data and if we got some? Return how much we read
    switch(const size_t stRead = static_cast<size_t>
      (read(haChildToParent.GetReadFd(), vpDest, stToRead)))
    { // Success?
      default:
        // Report read
        cLog->LogDebugExSafe("System fork at pid $ read $ bytes.",
          pPid, stRead);
        // Return bytes
        return stRead;
      // Error? Report it!
      case StdMaxSizeT:
        XCS("Error reading from pipe!",
          "Executable", IdentGet(), "Bytes", stToRead);
    } // Don't get here
  }
  /* -- Read data into memory block ---------------------------------------- */
  Memory ReadBlock(const size_t stBuffer=4096)
  { // Buffer for process output
    Memory mBuffer{ stBuffer };
    // Read some data and resize the block to the amount read
    mBuffer.MemResize(Read(mBuffer.MemPtr<char>(), mBuffer.MemSize()));
    // Return memory block
    return mBuffer;
  }
  /* -- Send to stdin ------------------------------------------------------ */
  size_t Send(const void*const vpDest, size_t stToWrite)
  { // Write data to pipe if we have any
    if(haParentToChild.IsNotValidWriteFd()) return 0;
    // Read some data and if we got some? Return how much we read
    switch(const size_t stWritten = static_cast<size_t>
      (write(haParentToChild.GetWriteFd(), vpDest, stToWrite)))
    { // Success?
      default:
        // Report write
        cLog->LogDebugExSafe("System fork at pid $ wrote $ bytes.",
          pPid, stWritten);
        // Return bytes written
        return stWritten;
      // Error? Report it!
      case StdMaxSizeT:
        XCS("Error writing to pipe!",
          "Executable", IdentGet(), "Bytes", stToWrite);
    } // Don't get here
  }
  /* -- Return pid --------------------------------------------------------- */
  unsigned int GetPid(void) { return uiPid; }
  /* -- Constructor with init ---------------------------------------------- */
  SysPipe(void) :
    /* -- Initialisers ----------------------------------------------------- */
    uiPid(0),                          // No external pid
    pPid(0)                            // No internal pid
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Constructor with init ---------------------------------------------- */
  explicit SysPipe(const string &strF) :
    /* -- Initialisers ----------------------------------------------------- */
    SysPipe()                          // Use default constructor parameters
    /* -- Initialise pipe -------------------------------------------------- */
    { Init(strF); }
  /* -- Destructor to kill and wait for process to exit -------------------- */
  ~SysPipe(void) { DeInit(); }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(SysPipe)             // Suppress default functions for safety
};/* -- End ---------------------------------------------------------------- */
/* == EoF =========================================================== EoF == */
