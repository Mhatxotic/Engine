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
using namespace IFlags::P;             using namespace ILog::P;
using namespace ILookupMap::P;         using namespace ILuaIdent::P;
using namespace ILuaLib::P;            using namespace IMutex::P;
using namespace IName::P;              using namespace ISerial::P;
using namespace IStd::P;               using namespace IString::P;
using namespace ISysUtil::P;
/* -- Typedefs ------------------------------------------------------------- */
using StdThread = ::std::thread;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public namespace
/* ------------------------------------------------------------------------- */
BUILD_SECURE_FLAGS(Thread,
  /* -- Current status ----------------------------------------------------- */
  TS_STANDBY                {Flag(0)}, // [0x00] Thread is not doing anything?
  TS_INIT                   {Flag(1)}, // [0x01] Thread starting up?
  TS_RUNNING                {Flag(2)}, // [0x02] Thread is running still?
  TS_ABORT                  {Flag(3)}, // [0x04] Thread should exit?
  TS_RETRY                  {Flag(4)}, // [0x08] Thread will start again?
  TS_FINISHED               {Flag(5)}, // [0x10] Thread waiting for join?
  TS_EXCEPTION              {Flag(6)}, // [0x20] Thread exception occured?
  /* -- Thread callbacks must only return one of these --------------------- */
  TS_ERROR                  {Flag(7)}, // [0x40] Thread terminated uncleanly?
  TS_OK                     {Flag(8)}  // [0x80] Thread terminated cleanly?
);/* == Thread collector class with global thread id counter =============== */
CTOR_BEGIN(Threads, Thread, CLHelperSafe,
  /* ----------------------------------------------------------------------- */
  AtomicSizeT      astRunning;               // Number of threads running
  const LookupMap<ThreadFlagsType> imCodes;  // Thread status codes
)/* ------------------------------------------------------------------------ */
using CbThFuncT = ThreadFlagsType(Thread&);  // Thread callback function
using CbThFunc = function<CbThFuncT> ;       // Wrapped inside a function class
/* ------------------------------------------------------------------------- */
class ThreadBase :                     // Thread variables class
  /* -- Base classes ------------------------------------------------------- */
  public ThreadFlags                   // Thread flags
{ /* -- Private variables --------------------------------------- */ protected:
  void            *vpParam;            // User parameter
  CbThFunc         ctfFunc;            // Thread callback function
  AtomicClkDuration acdStart,          // Thread start time
                   acdEnd;             // Thread end time
  const SysThread  sytPerf;            // Thread is high performance?
  /* -- Constructor -------------------------------------------------------- */
  explicit ThreadBase(
    /* -- Arguments -------------------------------------------------------- */
    const SysThread sytNPerf,          // Performance setting
    void*const     vpNParam = nullptr, // User parameter
    const CbThFunc &ctfNFunc = nullptr // Main callback function
    ):/* -- Initialisers --------------------------------------------------- */
    ThreadFlags{ TS_STANDBY },         // Set exit code to standby
    vpParam(vpNParam),                 // Set user thread parameter
    ctfFunc{ ctfNFunc },               // Set thread callback function
    sytPerf(sytNPerf)                  // Set thread high performance
    /* -- No code ---------------------------------------------------------- */
    {}
};/* ----------------------------------------------------------------------- */
CTOR_MEM_BEGIN_CSLAVE(Threads, Thread, ICHelperSafe),
  /* -- Base classes ------------------------------------------------------- */
  public ThreadBase,                   // Thread variables class
  private StdThread                    // The C++11 thread
{ /* -- Put in place a new thread ------------------------------------------ */
  template<typename ...VarArgs>void ThreadNew(VarArgs &&...vaArgs)
  { // Start the thread and set it to this thread
    StdThread tNewThread{ StdForward<VarArgs>(vaArgs)... };
    this->swap(tNewThread);
  }
  /* -- Set that the thread is exiting ------------------------------------- */
  void ThreadSetExiting(const ThreadFlagsConst tfcCode)
  { // Add the exiting flag
    FlagSetAndClear(tfcCode | TS_FINISHED, TS_RUNNING);
    // Set shutdown time
    acdEnd = cmHiRes.ClockGetEpochTime();
    // Reduce thread count
    --ctParent.astRunning;
  }
  /* -- Thread handler function -------------------------------------------- */
  void ThreadHandler() try
  { // Wait for main thread swap function to complete for the minute chance
    // that this thread could execute faster than the main thread and access
    // data from an uninitialised 'std::thread' class before 'swap()' finishes.
    while(ThreadIsNotInitialising()) StdSuspend();
    // Incrememt thread running count
    ++ctParent.astRunning;
    // Thread starting up in log
    cLog->LogDebugExSafe("Thread $<$> started.", Serial(), NameGet());
    // Set the start time and initialise the end time
    acdStart = cmHiRes.ClockGetEpochTime();
    acdEnd = cd0;
    // Loop forever...
    while(ThreadShouldNotExit())
    { // Set exit code as a reference that execution is proceeding
      FlagReset(TS_RUNNING);
      // Run thread and capture and check the result
      switch(const ThreadFlagsConst tfcResult{ ThreadGetCallback()(*this) })
      { // Clean exit of thread? Fallthrough to break the thread
        case TS_OK: [[fallthrough]];
        // Thread encounted an error?
        case TS_ERROR:
          // Set finished
          ThreadSetExiting(tfcResult);
          // Log the finish
          cLog->LogDebugExSafe("Thread $<$> finished in $ with $<0x$$>.",
            Serial(), NameGet(),
            durShort.Parse(ClockTimePointRangeToClampedDouble(
              ThreadGetEndTime(), ThreadGetStartTime())),
            ctParent.imCodes.Get(tfcResult), StdIOSHex, FlagGet());
          // Finished
          return;
        // Thread wants to retry? Then just retry!
        case TS_RETRY: continue;
        // Anything else just throw this error
        default: XC("Internal error: Invalid thread exit code!",
                   "Return", tfcResult.FlagGet(), "Flags", FlagGet());
      } // Execution never gets here
    } // Set finished
    ThreadSetExiting(TS_OK);
    // Log that the thread has terminated normally
    cLog->LogDebugExSafe("Thread $<$> aborted in $ with 0x$$.",
      Serial(), NameGet(), durShort.Parse(ClockTimePointRangeToClampedDouble(
        ThreadGetEndTime(), ThreadGetStartTime())), StdIOSHex, FlagGet());
  } // exception occurred in thread so handle it
  catch(const StdException &eReason)
  { // Begin shutting down
    ThreadSetExiting(TS_EXCEPTION);
    // Log if thread didn't signal to exit
    cLog->LogErrorExSafe("Thread $<$> finished in $ with 0x$$ "
                         "due to unhandled exception: $!",
      Serial(), NameGet(), durShort.Parse(ClockTimePointRangeToClampedDouble(
        ThreadGetEndTime(), ThreadGetStartTime())),
      StdIOSHex, FlagGet(), eReason);
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
  void ThreadSetExitFlag(const bool bState)
    { FlagSetOrClear(TS_ABORT, bState); }
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
  ClkTimePoint ThreadGetEndTime() const { return ClkTimePoint{ acdEnd }; }
  /* ----------------------------------------------------------------------- */
  const CbThFunc &ThreadGetCallback() const { return ctfFunc; }
  bool ThreadHaveCallback() const { return ThreadGetCallback() != nullptr; }
  /* ----------------------------------------------------------------------- */
  void ThreadCancelExit() { ThreadSetExitFlag(false); }
  /* ----------------------------------------------------------------------- */
  void ThreadSetExit()
  { // Ignore if already exited or already signalled to exit
    if(FlagIsAnyOfSet(TS_ABORT | TS_FINISHED)) return;
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
    if(ThreadIsJoinable()) ThreadJoin();
  }
  /* ----------------------------------------------------------------------- */
  void ThreadStopNoCheck()
  { // Thread is running? Inform thread loops that it should exit now
    ThreadSetExit();
    // Wait for thread to complete
    ThreadWait();
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
  /* -- Access individual thread flags ------------------------------------- */
  bool ThreadIsStandBy() const { return FlagGet() == TS_STANDBY; }
  bool ThreadIsNotStandBy() const { return !ThreadIsStandBy(); }
  bool ThreadIsInitialising() const { return FlagIsSet(TS_INIT); }
  bool ThreadIsNotInitialising() const { return !ThreadIsInitialising(); }
  bool ThreadIsExited() const { return FlagIsSet(TS_FINISHED); }
  bool ThreadIsNotExited() const { return !ThreadIsExited(); }
  bool ThreadIsRunning() const { return FlagIsSet(TS_RUNNING); }
  bool ThreadIsExitRetry() const { return FlagIsSet(TS_RETRY); }
  bool ThreadIsExitOk() const { return FlagIsSet(TS_OK); }
  bool ThreadIsExitError() const { return FlagIsSet(TS_ERROR); }
  bool ThreadIsExitExc() const { return FlagIsSet(TS_EXCEPTION); }
  bool ThreadShouldExit() const { return FlagIsSet(TS_ABORT); }
  bool ThreadShouldNotExit() const { return !ThreadShouldExit(); }
  /* ----------------------------------------------------------------------- */
  bool ThreadIsCurrent() const { return StdThreadId() == get_id(); }
  bool ThreadIsNotCurrent() const { return !ThreadIsCurrent(); }
  /* ----------------------------------------------------------------------- */
  unsigned ThreadGetPerf() const { return static_cast<unsigned>(sytPerf); }
  /* ----------------------------------------------------------------------- */
  bool ThreadIsJoinable() const { return joinable(); }
  bool ThreadIsNotJoinable() const { return !ThreadIsJoinable(); }
  /* ----------------------------------------------------------------------- */
  void ThreadJoin() { return join(); }
  /* ----------------------------------------------------------------------- */
  void ThreadStart(void*const vpPtr = nullptr)
  { // Set new thread parameters
    ThreadSetParam(vpPtr);
    // Reset the thread flags
    FlagReset(TS_STANDBY);
    // Start the thread and move that thread class to this thread
    ThreadNew(ThreadMain, this);
    // Register thread in collector
    this->CollectorRegister();
    // The thread is suspended until it sees this flag
    FlagReset(TS_INIT);
  }
  /* -- Stop and deinitialise the thread ---------------------------------- */
  void ThreadDeInit()
  { // Return if thread is not running
    if(ThreadIsNotJoinable()) return;
    // If is this thread then this is a bad idea
    if(ThreadIsCurrent())
      XC("Tried to join from the same thread!", "Name", NameGet());
    // Proceed with stop procuder if not exited
    if(ThreadIsNotExited()) ThreadStopNoCheck();
    // Else check to make sure it's not joined anyway
    else ThreadJoin();
  }
  /* -- Initialise with callback only and register ------------------------- */
  void ThreadInit(const CbThFunc &cbfCb) { ctfFunc = cbfCb; }
  /* -- Initialise with name and callback ---------------------------------- */
  template<class StrType>
    requires StdIsStrOrCStr<StrType>
  void ThreadInit(StrType &&strIdent, const CbThFunc &cbfCb)
    { NameSet(StdForward<StrType>(strIdent)); ThreadInit(cbfCb); }
  /* -- Initialise with name only and start (async.hpp) -------------------- */
  template<class StrType>
    requires StdIsStrOrCStr<StrType>
  void ThreadInit(StrType &&strIdent, void*const vpPtr)
    { NameSet(StdForward<StrType>(strIdent)); ThreadStart(vpPtr); }
  /* -- Initialise with name, callback, parameter and start execute -------- */
  template<class StrType>
    requires StdIsStrOrCStr<StrType>
  void ThreadInit(StrType &&strIdent, const CbThFunc &cbfCb, void*const vpPtr)
    { ThreadInit(StdForward<StrType>(strIdent), cbfCb); ThreadStart(vpPtr); }
  /* -- Initialise with callback, parameter and start execute (video.hpp) -- */
  void ThreadInit(const CbThFunc &cbfCb, void*const vpPtr)
    { ThreadInit(cbfCb); ThreadStart(vpPtr); }
  /* -- Full initialise and execute constructor ---------------------------- */
  template<class StrType>
    requires StdIsStrOrCStr<StrType>
  Thread(StrType &&strId,              // Requested Thread name
         const SysThread sytPf,        // Thread needs high performance?
         const CbThFunc &cbfCb,        // Requested callback function
         void*const vpPtr) :           // User parameter to store
    /* -- Initialisers ----------------------------------------------------- */
    Name{ StdForward<StrType>(strId) },// Initialise requested thread name
    ICHelperThread{ cThreads, this },  // Automatic (de)registration
    SerialSlave{ ctParent.Serial() },  // Initialise identification number
    ThreadBase{ sytPf, vpPtr, cbfCb }, // Set perf, parameter and callback
    StdThread{ ThreadMain, this }      // Start the thread straight away
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- Init name, priority and function in standby ------------------------ */
  template<class StrType>
    requires StdIsStrOrCStr<StrType>
  Thread(StrType &&strId,              // Requested Thread name
         const SysThread sytPf,        // Thread needs high performance?
         const CbThFunc &cbfCb) :      // Requested callback function
    /* -- Initialisers ----------------------------------------------------- */
    Name{ StdForward<StrType>(strId) },// Initialise requested thread name
    ICHelperThread{ cThreads },        // Automatic (de)registration
    SerialSlave{ ctParent.Serial() },  // Initialise identification number
    ThreadBase{ sytPf, nullptr, cbfCb }// Just set callback function
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- Init name and priority in standby ---------------------------------- */
  template<class StrType>
    requires StdIsStrOrCStr<StrType>
  Thread(StrType &&strId,              // Requested Thread name
         const SysThread sytPf) :      // Thread needs high performance?
    /* -- Initialisers ----------------------------------------------------- */
    Name{ StdForward<StrType>(strId) },// Initialise requested thread name
    ICHelperThread{ cThreads },        // No automatic registration
    SerialSlave{ ctParent.Serial() },  // Initialise identification number
    ThreadBase{ sytPf }                // Initialise nothing else
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- Init only priority in standby -------------------------------------- */
  explicit Thread(const SysThread sytPf) : // Requested callback function
    /* -- Initialisers ----------------------------------------------------- */
    ICHelperThread{ cThreads },        // No automatic registration
    SerialSlave{ ctParent.Serial() },  // Initialise identification number
    ThreadBase{ sytPf }                // Just set callback function
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- Init priority and function in standby ------------------------------ */
  Thread(const SysThread sytPf,        // Thread needs high performance?
         const CbThFunc &cbfCb) :      // Requested callback function
    /* -- Initialisers ----------------------------------------------------- */
    ICHelperThread{ cThreads },        // No automatic registration
    SerialSlave{ ctParent.Serial() },  // Initialise identification number
    ThreadBase{ sytPf, nullptr, cbfCb }// Just set callback function
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
  /* -- These only cover the exit codes ------------------------------------ */
  IDMAPSTR(TS_ERROR), IDMAPSTR(TS_EXCEPTION), IDMAPSTR(TS_OK),
  IDMAPSTR(TS_RETRY),
  /* ----------------------------------------------------------------------- */
}, "TS_UNKNOWN" });                    // End of 'Threads' initialisation
/* ------------------------------------------------------------------------- */
static size_t ThreadGetRunning() { return cThreads->astRunning; }
/* ------------------------------------------------------------------------- */
}                                      // End of public namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private namespace
/* == EoF =========================================================== EoF == */
