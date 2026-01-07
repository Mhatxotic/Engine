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
using namespace IError::P;             using namespace IIdent::P;
using namespace ILog::P;               using namespace ILuaIdent::P;
using namespace ILuaLib::P;            using namespace IMutex::P;
using namespace IStd::P;               using namespace IString::P;
using namespace ISysUtil::P;           using ::std::thread;
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
  TS_OK,                               // [+4] Thread will terminate cleanly
};/* == Thread collector class with global thread id counter =============== */
CTOR_BEGIN(Threads, Thread, CLHelperSafe,
  /* ----------------------------------------------------------------------- */
  AtomicSizeT      astRunning;         // Number of threads running
  const IdMap<ThreadStatus> imCodes;   // Thread status codes
)/* ------------------------------------------------------------------------ */
typedef ThreadStatus (CbThFuncT)(Thread&); // Thread callback function
typedef function<CbThFuncT> CbThFunc;  // Wrapped inside a function class
/* ------------------------------------------------------------------------- */
class ThreadBase                       // Thread variables class
{ /* -- Private variables --------------------------------------- */ protected:
  typedef atomic<ThreadStatus> AtomicThreadStatus;
  AtomicThreadStatus atsCode;          // Callback exit code
  void            *vpParam;            // User parameter
  CbThFunc         ctfFunc;            // Thread callback function
  AtomicBool       abShouldExit;       // Thread should exit
  AtomicClkDuration acdStart,          // Thread start time
                   scdEnd;             // Thread end time
  const SysThread  stPerf;             // Thread is high performance?
  /* -- Constructor -------------------------------------------------------- */
  ThreadBase(const SysThread stNPerf,  // Thread is high performance?
             void*const vpNParam,      // Thread user parameter
             const CbThFunc &ctfNFunc) : // Thread callback function
    /* -- Initialisers ----------------------------------------------------- */
    atsCode{TS_STANDBY},               // Set exit code to standby
    vpParam(vpNParam),                 // Set user thread parameter
    ctfFunc{ ctfNFunc },               // Set thread callback function
    abShouldExit(false),               // Should never exit at first
    stPerf(stNPerf)                    // Set thread high performance
    /* -- No code ---------------------------------------------------------- */
    {}
};/* ----------------------------------------------------------------------- */
CTOR_MEM_BEGIN_CSLAVE(Threads, Thread, ICHelperUnsafe),
  /* -- Base classes ------------------------------------------------------- */
  public ThreadBase,                   // Thread variables class
  private thread                       // The C++11 thread
{ /* -- Put in place a new thread ------------------------------------------ */
  template<typename ...VarArgs>void ThreadNew(VarArgs &&...vaArgs)
  { // Start the thread
    thread tNewThread{ StdForward<VarArgs>(vaArgs)... };
    // Set to this thread
    this->swap(tNewThread);
  }
  /* -- Thread handler function -------------------------------------------- */
  void ThreadHandler() try
  { // Wait for main thread swap function to complete for the minute chance
    // that this thread could execute faster than the main thread and access
    // data from an uninitialised 'std::thread' class before 'swap()' finishes.
    while(atsCode != TS_INIT) StdSuspend();
    // Incrememt thread running count
    ++cParent->astRunning;
    // Thread starting up in log
    cLog->LogDebugExSafe("Thread $<$> started.", CtrGet(), IdentGet());
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
    --cParent->astRunning;
    // Log if thread didn't signal to exit
    cLog->LogDebugExSafe("Thread $<$> finished in $ with $.",
      CtrGet(), IdentGet(),
      StrShortFromDuration(ClockTimePointRangeToClampedDouble(
        ThreadGetEndTime(), ThreadGetStartTime())), ThreadGetExitCodeString());
    // Reset exit code to waiting for acknowledgement by engine thread
    ThreadSetExitCode(TS_FINISHED);
  } // exception occurred in thread so handle it
  catch(const exception &eReason)
  { // Reduce thread count
    --cParent->astRunning;
    // Set shutdown time
    scdEnd = cmHiRes.GetEpochTime();
    // Log if thread didn't signal to exit
    cLog->LogErrorExSafe("Thread $<$> finished in $ due to exception: $!",
      CtrGet(), IdentGet(),
      StrShortFromDuration(ClockTimePointRangeToClampedDouble(
        ThreadGetEndTime(), ThreadGetStartTime())), eReason);
    // Return error and set thread to exit
    ThreadSetExitCode(TS_EXCEPTION);
  }
  /* ----------------------------------------------------------------------- */
  static void ThreadMain(void*const vpPtr)
  { // Get pointer to thread class and if valid?
    if(Thread*const tPtr = reinterpret_cast<Thread*>(vpPtr))
    { // Set thread name and priority in system
      if(!SysInitThread(tPtr->IdentGetData(), tPtr->stPerf))
        cLog->LogWarningExSafe("Thread '$' update priority to $ failed: $!",
          tPtr->IdentGet(), tPtr->ThreadGetPerf(), StrFromErrNo());
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
  /* --------------------------------------------------------------- */ public:
  template<typename ReturnType>ReturnType ThreadGetParam() const
    { return reinterpret_cast<ReturnType>(vpParam); }
  template<typename ReturnType>void ThreadSetParam(const ReturnType rtParam)
    { vpParam = reinterpret_cast<void*>(rtParam); }
  /* ----------------------------------------------------------------------- */
  const ClkTimePoint ThreadGetStartTime() const
    { return ClkTimePoint{ acdStart }; }
  const ClkTimePoint ThreadGetEndTime() const
    { return ClkTimePoint{ scdEnd }; }
  /* ----------------------------------------------------------------------- */
  ThreadStatus ThreadGetExitCode() const { return atsCode; }
  void ThreadSetExitCode(const ThreadStatus tsNew) { atsCode = tsNew; }
  const string_view ThreadGetExitCodeString() const
    { return cParent->imCodes.Get(ThreadGetExitCode()); }
  /* ----------------------------------------------------------------------- */
  const CbThFunc &ThreadGetCallback() const { return ctfFunc; }
  bool ThreadHaveCallback() const { return !!ThreadGetCallback(); }
  /* ----------------------------------------------------------------------- */
  bool ThreadIsParamSet() const { return !!vpParam; }
  /* ----------------------------------------------------------------------- */
  void ThreadCancelExit() { ThreadSetExitFlag(false); }
  /* ----------------------------------------------------------------------- */
  void ThreadSetExit()
  { // Ignore if already exited or already signalled to exit
    if(ThreadShouldExit()) return;
    // Set exit
    ThreadDoSetExit();
    // Log signal to exit if sent from the thread
    if(ThreadIsCurrent())
      cLog->LogDebugExSafe("Thread $<$> signalling to exit.",
        CtrGet(), IdentGet());
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
        "Thread '$' tried to join from the same thread!", IdentGet());
    // Proceed with termination
    ThreadStopNoCheck();
  }
  /* ----------------------------------------------------------------------- */
  void ThreadStop()
  { // Return if thread is not running
    if(ThreadIsNotJoinable()) return;
    // If is this thread then this is a bad idea
    if(ThreadIsCurrent())
      XC("Tried to join from the same thread!", "Identifier", IdentGet());
    // Proceed with termination
    ThreadStopNoCheck();
  }
  /* ----------------------------------------------------------------------- */
  bool ThreadIsCurrent() const { return StdThreadId() == get_id(); }
  bool ThreadIsNotCurrent() const { return !ThreadIsCurrent(); }
  /* ----------------------------------------------------------------------- */
  unsigned int ThreadGetPerf() const
    { return static_cast<unsigned int>(stPerf); }
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
  void ThreadInit(const CbThFunc &cbfC)
    { ctfFunc = cbfC; this->CollectorRegister(); }
  /* -- Initialise with name and callback ---------------------------------- */
  void ThreadInit(const string &strN, const CbThFunc &cbfC)
    { IdentSet(strN); ThreadInit(cbfC); }
  /* -- Initialise with name, callback, parameter and start execute -------- */
  void ThreadInit(const string &strN, const CbThFunc &cbfC, void*const vpPtr)
    { ThreadInit(strN, cbfC); ThreadStart(vpPtr); }
  /* -- Initialise with callback, parameter and start execute -------------- */
  void ThreadInit(const CbThFunc &cbfC, void*const vpPtr)
    { ThreadInit(cbfC); ThreadStart(vpPtr); }
  /* -- Full initialise and execute constructor ---------------------------- */
  Thread(const string &strN,           // Requested Thread name
         const SysThread sP,           // Thread needs high performance?
         const CbThFunc &cbfC,         // Requested callback function
         void*const vpPtr) :           // User parameter to store
    /* -- Initialisers ----------------------------------------------------- */
    Ident{ strN },                     // Initialise requested thread name
    ICHelperThread{ cThreads, this },  // Automatic (de)registration
    IdentCSlave{ cParent->CtrNext() }, // Initialise identification number
    ThreadBase{ sP, vpPtr, cbfC },     // Set perf, parameter and callback
    thread{ ThreadMain, this }         // Start the thread straight away
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- Standby constructor (set everything except user parameter) --------- */
  Thread(const string &strN,           // Requested Thread name
         const SysThread sP,           // Thread needs high performance?
         const CbThFunc &cbfC) :       // Requested callback function
    /* -- Initialisers ----------------------------------------------------- */
    Ident{ strN },                     // Set requested identifier
    ICHelperThread{ cThreads, this },  // Automatic (de)registration
    IdentCSlave{ cParent->CtrNext() }, // Initialise identification number
    ThreadBase{ sP, nullptr, cbfC }    // Just set callback function
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- Standby constructor ------------------------------------------------ */
  Thread(const string &strN,           // Requested Thread name
         const SysThread sP) :         // Thread needs high performance?
    /* -- Initialisers ----------------------------------------------------- */
    Ident{ strN },                     // Set requested identifer
    ICHelperThread{ cThreads },        // No automatic registration
    IdentCSlave{ cParent->CtrNext() }, // Initialise identification number
    ThreadBase{ sP, nullptr, nullptr } // Initialise nothing else
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- Standby constructor ------------------------------------------------ */
  explicit Thread(const SysThread sP) :// Thread needs high performance?
    /* -- Initialisers ----------------------------------------------------- */
    ICHelperThread{ cThreads },        // No automatic registration
    IdentCSlave{ cParent->CtrNext() }, // Initialise identification number
    ThreadBase{ sP, nullptr, nullptr } // Initialise only thread priority
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
        CtrGet(), IdentGet());
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
/* -- Thread sync helper --------------------------------------------------- */
template<class Callbacks>class ThreadSyncHelper : private Callbacks
{ /* -------------------------------------------------------------- */ private:
  Thread            &tOwner;           // Reference to thread owner
  AtomicBool         abUnlock;         // Synchronisation should occur?
  bool               bAToB,            // Release lock for thread A?
                     bBToA;            // Release lock for thread B?
  MutexLock          mAToB,            // Thread A to Thread B mutex
                     mBToA;            // Thread B to Thread A mutex
  condition_variable cvAToB,           // Thread A to Thread B notification
                     cvBToA;           // Thread B to Thread A notification
  /* ----------------------------------------------------------------------- */
  void SendNotification(MutexLock &mM, condition_variable &cvCV, bool &bUL)
  { // Acquire unique lock
    mM.MutexCall([&bUL, &cvCV](){
      // Modify the unlock boolean
      bUL = true;
      // Send notification
      cvCV.notify_one();
    });
  }
  /* ----------------------------------------------------------------------- */
  void WaitForThreadNotification(MutexLock &mM, condition_variable &cvCV,
    bool &bUL)
  { // Setup lock for condition variable
    mM.MutexUniqueCall([this, &cvCV, &bUL](UniqueLock &ulLock){
      // Wait for Thread A to notify us to continue.
      cvCV.wait(ulLock,
        [this, &bUL]{ return bUL || tOwner.ThreadShouldExit(); });
      // Reset locked boolean
      bUL = false;
    });
  }
  /* ----------------------------------------------------------------------- */
  void SendNotificationToThreadA()
    { SendNotification(mBToA, cvBToA, bBToA); }
  void WaitForThreadANotification()
    { WaitForThreadNotification(mAToB, cvAToB, bAToB); }
  void SendNotificationToThreadB()
    { SendNotification(mAToB, cvAToB, bAToB); }
  void WaitForThreadBNotification()
    { WaitForThreadNotification(mBToA, cvBToA, bBToA); }
  /* --------------------------------------------------------------- */ public:
  void CheckStateInThreadA()
  { // Ignore if other thread doesn't need action
    if(!abUnlock) return;
    // Unlock progressing
    abUnlock = false;
    // Perform starting callback from 'Callbacks' class
    this->StartThreadACallback();
    // Send notification to Thread B to say we're done initial processing
    SendNotificationToThreadB();
    // Wait for thread B to finish processing
    WaitForThreadBNotification();
    // Perform finishing callback from 'Callbacks' class
    this->FinishThreadACallback();
    // Tell thread B we've finished with the procedure
    SendNotificationToThreadB();
  }
  /* -- End notification to Thread A from Thread B ------------------------- */
  void FinishNotifyThreadA()
  { // Ignore if render thread isn't running
    if(tOwner.ThreadIsNotJoinable()) return;
    // Call final Thread B callback
    this->FinishThreadBCallback();
    // Send notification to Thread A
    SendNotificationToThreadA();
    // Wait for Thread A to send notification back to us in Thread B
    WaitForThreadANotification();
  }
  /* -- Begin notification to Thread A from Thread B ----------------------- */
  void StartNotifyThreadA()
  { // Ignore if render thread isn't running
    if(tOwner.ThreadIsNotJoinable()) return;
    // Set state so Thread A thread knows to proceed
    abUnlock = true;
    // Wait for Thread A to send notification back to us in Thread B
    WaitForThreadANotification();
    // Now processing Thread B start procedure
    this->StartThreadBCallback();
  }
  /* -- Constructor -------------------------------------------------------- */
  explicit ThreadSyncHelper(Thread &tO) : // Thread being used
    /* -- Initialisers ----------------------------------------------------- */
    tOwner(tO),                        // Set thread owner
    abUnlock(false),                   // Initially unlocked
    bAToB(false),                      // Not sending msg from Thread A to B
    bBToA(false)                       // Not sending msg from Thread B to A
    /* -- No code ---------------------------------------------------------- */
    {}
};
/* ------------------------------------------------------------------------- */
static size_t ThreadGetRunning() { return cThreads->astRunning; }
/* ------------------------------------------------------------------------- */
}                                      // End of public namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private namespace
/* == EoF =========================================================== EoF == */
