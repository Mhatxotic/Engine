/* == THREAD.HPP =========================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## This module handles the ability to manage and spawn multiple        ## **
** ## threads for parallel and asynchronous execution.                    ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IThread {                    // Start of private namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IClock::P;             using namespace ICollector::P;
using namespace IDuration::P;          using namespace IError::P;
using namespace ILog::P;               using namespace ILookupMap::P;
using namespace ILuaIdent::P;          using namespace ILuaLib::P;
using namespace IMutex::P;             using namespace IName::P;
using namespace ISerial::P;            using namespace IStd::P;
using namespace IString::P;            using namespace ISysUtil::P;
/* -- Typedefs ------------------------------------------------------------- */
using StdThread = ::std::thread;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public namespace
/* ------------------------------------------------------------------------- */
enum ThreadStatus : int                // Thread status codes
{ /* ----------------------------------------------------------------------- */
  TS_EXCEPTION = -4,                   // [-4] Thread exited with an exception
  TS_INIT,                             // [-3] Thread starting up
  TS_FINISHED,                         // [-2] Thread waiting for join
  TS_ERROR,                            // [-1] Thread will terminate cleanly
  TS_STANDBY,                          // [+0] Thread is in stand-by mode
  TS_RUNNING,                          // [+1] Thread is running still
  TS_RETRY,                            // [+2] Thread will start again
  TS_OK,                               // [+3] Thread will terminate cleanly
};/* == Thread collector class with global thread id counter =============== */
CTOR_BEGIN(Threads, Thread, CLHelperSafe,
  /* ----------------------------------------------------------------------- */
  AtomicSizeT      astRunning;           // Number of threads running
  const LookupMap<ThreadStatus> imCodes; // Thread status codes
)/* ------------------------------------------------------------------------ */
using CbThFuncT = ThreadStatus(Thread&); // Thread callback function
using CbThFunc = function<CbThFuncT> ;   // Wrapped inside a function class
/* ------------------------------------------------------------------------- */
class ThreadBase                       // Thread variables class
{ /* -- Private variables --------------------------------------- */ protected:
  using AtomicThreadStatus = StdAtomic<ThreadStatus> ;
  AtomicThreadStatus atsCode;          // Callback exit code
  void            *vpParam;            // User parameter
  CbThFunc         ctfFunc;            // Thread callback function
  AtomicBool       abShouldExit;       // Thread should exit
  AtomicClkDuration acdStart,          // Thread start time
                   scdEnd;             // Thread end time
  const SysThread  sytPerf;            // Thread is high performance?
  /* -- Constructor -------------------------------------------------------- */
  ThreadBase(const SysThread sytNPerf,             // Thread is high perf?
             void*const vpNParam      = nullptr,   // Thread user parameter
             const CbThFunc &ctfNFunc = nullptr) : // Thread callback function
    /* -- Initialisers ----------------------------------------------------- */
    atsCode{TS_STANDBY},               // Set exit code to standby
    vpParam(vpNParam),                 // Set user thread parameter
    ctfFunc{ ctfNFunc },               // Set thread callback function
    abShouldExit(false),               // Should never exit at first
    sytPerf(sytNPerf)                  // Set thread high performance
    /* -- No code ---------------------------------------------------------- */
    {}
};/* ----------------------------------------------------------------------- */
CTOR_MEM_BEGIN_CSLAVE(Threads, Thread, ICHelperUnsafe),
  /* -- Base classes ------------------------------------------------------- */
  public ThreadBase,                   // Thread variables class
  private StdThread                    // The C++11 thread
{ /* -- Put in place a new thread ------------------------------------------ */
  template<typename ...VarArgs>void ThreadNew(VarArgs &&...vaArgs)
  { // Start the thread and set it to this thread
    StdThread tNewThread{ StdForward<VarArgs>(vaArgs)... };
    this->swap(tNewThread);
  }
  /* -- Thread handler function -------------------------------------------- */
  void ThreadHandler() try
  { // Wait for main thread swap function to complete for the minute chance
    // that this thread could execute faster than the main thread and access
    // data from an uninitialised 'std::thread' class before 'swap()' finishes.
    while(atsCode != TS_INIT) StdSuspend();
    // Incrememt thread running count
    ++ctParent.astRunning;
    // Thread starting up in log
    cLog->LogDebugExSafe("Thread $<$> started.", Serial(), NameGet());
    // Set the start time and initialise the end time
    acdStart = cmHiRes.GetEpochTime();
    scdEnd = cd0;
    // Loop forever until thread should exit
    while(ThreadShouldNotExit())
    { // Set exit code to -1 as a reference that execution is proceeding
      ThreadSetExitCode(TS_RUNNING);
      // Run thread and capture result
      ThreadSetExitCode(ThreadGetCallback()(*this));
      // If non-zero then break else start thread again
      if(ThreadGetExitCode() != TS_RETRY) break;
    } // Set shutdown time
    scdEnd = cmHiRes.GetEpochTime();
    // Reduce thread count
    --ctParent.astRunning;
    // Log if thread didn't signal to exit
    cLog->LogDebugExSafe("Thread $<$> finished in $ with $<$>.",
      Serial(), NameGet(), durShort.Parse(ClockTimePointRangeToClampedDouble(
        ThreadGetEndTime(), ThreadGetStartTime())), ThreadGetExitCodeString(),
        ThreadGetExitCode());
    // Reset exit code to waiting for acknowledgement by engine thread
    ThreadSetExitCode(TS_FINISHED);
  } // exception occurred in thread so handle it
  catch(const StdException &eReason)
  { // Reduce thread count
    --ctParent.astRunning;
    // Set shutdown time
    scdEnd = cmHiRes.GetEpochTime();
    // Log if thread didn't signal to exit
    cLog->LogErrorExSafe("Thread $<$> finished in $ due to exception: $!",
      Serial(), NameGet(), durShort.Parse(ClockTimePointRangeToClampedDouble(
        ThreadGetEndTime(), ThreadGetStartTime())), eReason);
    // Return error and set thread to exit
    ThreadSetExitCode(TS_EXCEPTION);
  }
  /* ----------------------------------------------------------------------- */
  static void ThreadMain(void*const vpPtr)
  { // Get pointer to thread class and if valid?
    if(Thread*const tPtr = reinterpret_cast<Thread*>(vpPtr))
    { // Set thread name and priority in system
      if(!SysInitThread(tPtr->NameGetData(), tPtr->sytPerf))
        cLog->LogWarningExSafe("Thread '$' update priority to $ failed: $!",
          tPtr->NameGet(), tPtr->ThreadGetPerf(), StrFromErrNo());
      // Run the thread callback
      tPtr->ThreadHandler();
    } // Report the problem
    else cLog->LogErrorSafe(
      "Thread switch to thiscall failed with null class ptr!");
  }
  /* ----------------------------------------------------------------------- */
  void ThreadSetExitFlag(const bool bState) { abShouldExit = bState; }
  /* ----------------------------------------------------------------------- */
  void ThreadDoSetExit() { ThreadSetExitFlag(true); }
  /* -- Get stored user parameter ---------------------------------- */ public:
  template<typename PtrType = void>
    requires (!StdIsPointer<PtrType>)
  PtrType *ThreadGetParam() const
    { return reinterpret_cast<PtrType*>(vpParam); }
  /* -- Return if user parameter is set ------------------------------------ */
  bool ThreadIsParamSet() const { return ThreadGetParam() != nullptr; }
  /* -- Set stored user parameter ------------------------------------------ */
  template<typename PtrType>
    requires (!StdIsPointer<PtrType>)
  void ThreadSetParam(PtrType*const ptParam)
    { vpParam = reinterpret_cast<void*>(ptParam); }
  /* ----------------------------------------------------------------------- */
  ClkTimePoint ThreadGetStartTime() const { return ClkTimePoint{ acdStart }; }
  ClkTimePoint ThreadGetEndTime() const { return ClkTimePoint{ scdEnd }; }
  /* ----------------------------------------------------------------------- */
  ThreadStatus ThreadGetExitCode() const { return atsCode; }
  void ThreadSetExitCode(const ThreadStatus tsNew) { atsCode = tsNew; }
  StdStringView ThreadGetExitCodeString() const
    { return ctParent.imCodes.Get(ThreadGetExitCode()); }
  /* ----------------------------------------------------------------------- */
  const CbThFunc &ThreadGetCallback() const { return ctfFunc; }
  bool ThreadHaveCallback() const { return ThreadGetCallback() != nullptr; }
  /* ----------------------------------------------------------------------- */
  void ThreadCancelExit() { ThreadSetExitFlag(false); }
  /* ----------------------------------------------------------------------- */
  void ThreadSetExit()
  { // Ignore if already exited or already signalled to exit
    if(ThreadShouldExit()) return;
    // Set exit
    ThreadDoSetExit();
    // Log signal to exit if sent from the thread
    cLog->LogDebugExSafe("Thread $<$> exit signalled by $.",
      Serial(), NameGet(), ThreadIsCurrent() ? "itself" : "another");
  }
  /* ----------------------------------------------------------------------- */
  void ThreadWait()
  { // Wait for the thread to terminate. It is important to put this here
    // even though the thread explicitly said it's no longer executing
    // because join() HAS to be called in order for thread to be reused, and
    // join() will abort() if joinable() isn't checked as well.
    if(ThreadIsJoinable()) { ThreadJoin(); ThreadSetExitCode(TS_STANDBY); }
    // No longer executing
    ThreadCancelExit();
  }
  /* ----------------------------------------------------------------------- */
  void ThreadStopNoCheck()
  { // Thread is running? Inform thread loops that it should exit now
    ThreadSetExit();
    // Wait for thread to complete
    ThreadWait();
    // Set to standby with a new thread
    ThreadNew();
  }
  /* ----------------------------------------------------------------------- */
  void ThreadStopNoThrow()
  { // Return if thread is not running
    if(ThreadIsNotJoinable()) return;
    // If is this thread then this is a bad idea
    if(ThreadIsCurrent())
      return cLog->LogWarningExSafe(
        "Thread '$' tried to join from the same thread!", NameGet());
    // Proceed with termination
    ThreadStopNoCheck();
  }
  /* ----------------------------------------------------------------------- */
  void ThreadStop()
  { // Return if thread is not running
    if(ThreadIsNotJoinable()) return;
    // If is this thread then this is a bad idea
    if(ThreadIsCurrent())
      XC("Tried to join from the same thread!", "Name", NameGet());
    // Proceed with termination
    ThreadStopNoCheck();
  }
  /* ----------------------------------------------------------------------- */
  bool ThreadIsCurrent() const { return StdThreadId() == get_id(); }
  bool ThreadIsNotCurrent() const { return !ThreadIsCurrent(); }
  /* ----------------------------------------------------------------------- */
  unsigned ThreadGetPerf() const { return static_cast<unsigned>(sytPerf); }
  /* ----------------------------------------------------------------------- */
  bool ThreadShouldExit() const { return abShouldExit; }
  bool ThreadShouldNotExit() const { return !ThreadShouldExit(); }
  /* ----------------------------------------------------------------------- */
  bool ThreadIsJoinable() const { return joinable(); }
  bool ThreadIsNotJoinable() const { return !ThreadIsJoinable(); }
  /* ----------------------------------------------------------------------- */
  void ThreadJoin() { return join(); }
  /* ----------------------------------------------------------------------- */
  bool ThreadIsExited() const { return ThreadGetExitCode() != -1; }
  bool ThreadIsNotExited() const { return !ThreadIsExited(); }
  /* ----------------------------------------------------------------------- */
  bool ThreadIsException() const { return ThreadGetExitCode() == -2; }
  /* ----------------------------------------------------------------------- */
  void ThreadStart(void*const vpPtr = nullptr)
  { // Bail if thread already started
    if(ThreadIsJoinable()) return;
    // Set new thread parameters
    ThreadCancelExit();
    ThreadSetParam(vpPtr);
    ThreadSetExitCode(TS_INIT);
    // Start the thread and move that thread class to this thread
    ThreadNew(ThreadMain, this);
  }
  /* -- Stop and de-initialise the thread ---------------------------------- */
  void ThreadDeInit()
    { ThreadStop(); this->CollectorUnregister(); }
  /* -- Initialise with callback only and register ------------------------- */
  void ThreadInit(const CbThFunc &cbfCb)
    { ctfFunc = cbfCb; this->CollectorRegister(); }
  /* -- Initialise with name and callback ---------------------------------- */
  void ThreadInit(const StdStringView &ssvIdent, const CbThFunc &cbfCb)
    { NameSet(ssvIdent); ThreadInit(cbfCb); }
  /* -- Initialise with name, callback, parameter and start execute -------- */
  void ThreadInit(const StdStringView &ssvIdent, const CbThFunc &cbfCb,
    void*const vpPtr)
  { ThreadInit(ssvIdent, cbfCb); ThreadStart(vpPtr); }
  /* -- Initialise with callback, parameter and start execute -------------- */
  void ThreadInit(const CbThFunc &cbfCb, void*const vpPtr)
    { ThreadInit(cbfCb); ThreadStart(vpPtr); }
  /* -- Full initialise and execute constructor ---------------------------- */
  Thread(const StdStringView &ssvIdent,// Requested Thread name
         const SysThread sytPf,        // Thread needs high performance?
         const CbThFunc &cbfCb,        // Requested callback function
         void*const vpPtr) :           // User parameter to store
    /* -- Initialisers ----------------------------------------------------- */
    Name{ ssvIdent },                  // Initialise requested thread name
    ICHelperThread{ cThreads, this },  // Automatic (de)registration
    SerialSlave{ ctParent.Serial() },  // Initialise identification number
    ThreadBase{ sytPf, vpPtr, cbfCb }, // Set perf, parameter and callback
    StdThread{ ThreadMain, this }      // Start the thread straight away
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- Standby constructor (set everything except user parameter) --------- */
  Thread(const StdStringView &ssvIdent,// Requested Thread name
         const SysThread sytPf,        // Thread needs high performance?
         const CbThFunc &cbfCb) :      // Requested callback function
    /* -- Initialisers ----------------------------------------------------- */
    Name{ ssvIdent },                  // Set requested identifier
    ICHelperThread{ cThreads, this },  // Automatic (de)registration
    SerialSlave{ ctParent.Serial() },  // Initialise identification number
    ThreadBase{ sytPf, nullptr, cbfCb }// Just set callback function
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- Standby constructor ------------------------------------------------ */
  Thread(const StdStringView &ssvIdent,// Requested Thread name
         const SysThread sytPf) :      // Thread needs high performance?
    /* -- Initialisers ----------------------------------------------------- */
    Name{ ssvIdent },                  // Set requested identifer
    ICHelperThread{ cThreads },        // No automatic registration
    SerialSlave{ ctParent.Serial() },  // Initialise identification number
    ThreadBase{ sytPf }                // Initialise nothing else
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- Standby constructor ------------------------------------------------ */
  explicit Thread(const SysThread sytPf) :// Thread needs high performance?
    /* -- Initialisers ----------------------------------------------------- */
    ICHelperThread{ cThreads },        // No automatic registration
    SerialSlave{ ctParent.Serial() },  // Initialise identification number
    ThreadBase{ sytPf }                // Initialise only thread priority
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- Destructor --------------------------------------------------------- */
  DTORHELPER(~Thread,
    // Done if not running
    if(ThreadIsNotJoinable()) return;
    // Not signalled to exit?
    if(ThreadShouldNotExit())
    { // Set exit signal
      ThreadDoSetExit();
      // Log signalled to exit in destructor
      cLog->LogWarningExSafe("Thread $<$> signalled to exit in destructor.",
        Serial(), NameGet());
    } // Wait to synchronise
    ThreadJoin();
  )
};/* ======================================================================= */
CTOR_END(Threads, Thread, THREAD,,,, astRunning{0}, imCodes{{
  /* ----------------------------------------------------------------------- */
  IDMAPSTR(TS_EXCEPTION),              IDMAPSTR(TS_INIT),
  IDMAPSTR(TS_ERROR),                  IDMAPSTR(TS_STANDBY),
  IDMAPSTR(TS_RUNNING),                IDMAPSTR(TS_RETRY),
  IDMAPSTR(TS_FINISHED),               IDMAPSTR(TS_OK),
  /* ----------------------------------------------------------------------- */
}});                                   // End of 'Threads' initialisation
/* ------------------------------------------------------------------------- */
static size_t ThreadGetRunning() { return cThreads->astRunning; }
/* ------------------------------------------------------------------------- */
}                                      // End of public namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private namespace
/* == EoF =========================================================== EoF == */
