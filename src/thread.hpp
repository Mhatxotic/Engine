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
using namespace ILog::P;               using namespace ILuaLib::P;
using namespace IStd::P;               using namespace IString::P;
using namespace ISysUtil::P;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public namespace
/* == Thread collector class with global thread id counter ================= */
CTOR_BEGIN(Threads, Thread, CLHelperSafe,
  /* ----------------------------------------------------------------------- */
  SafeSizeT        stRunning;          // Number of threads running
)/* ------------------------------------------------------------------------ */
class ThreadBase                       // Thread variables class
{ /* -- Private typedefs ---------------------------------------- */ protected:
  typedef int (CBFuncT)(Thread&);      // Thread callback function
  /* -- Public typedefs -------------------------------------------- */ public:
  typedef function<CBFuncT> CBFunc;    // Wrapped inside a function class
  /* -- Private variables --------------------------------------- */ protected:
  SafeInt          siExitCode;         // Callback exit code
  void            *vpParam;            // User parameter
  CBFunc           cbfFunc;            // Thread callback function
  SafeBool         sbShouldExit,       // Thread should exit
                   sbRunning;          // Thread is running?
  SafeClkDuration  scdStart,           // Thread start time
                   scdEnd;             // Thread end time
  const SysThread  stPerf;             // Thread is high performance?
  /* -- Constructor -------------------------------------------------------- */
  ThreadBase(const SysThread stNPerf,  // Thread is high performance?
             void*const vpNParam,      // Thread user parameter
             const CBFunc &cbfNFunc) : // Thread callback function
    /* -- Initialisers ----------------------------------------------------- */
    siExitCode(0),                     // Set exit code to standby
    vpParam(vpNParam),                 // Set user thread parameter
    cbfFunc{ cbfNFunc },               // Set thread callback function
    sbShouldExit(false),               // Should never exit at first
    sbRunning(false),                  // Should never be running at first
    scdStart{ seconds{ 0 } },          // Never started time
    scdEnd{ seconds{ 0 } },            // Never finished time
    stPerf(stNPerf)                    // Set thread high performance
    /* --------------------------------------------------------------------- */
    { }                                // Do nothing else
};/* ----------------------------------------------------------------------- */
CTOR_MEM_BEGIN_CSLAVE(Threads, Thread, ICHelperUnsafe),
  /* -- Base classes ------------------------------------------------------- */
  public Ident,                        // Object name
  public ThreadBase,                   // Thread variables class
  private thread                       // The C++11 thread
{ /* -- Put in place a new thread ------------------------------------------ */
  template<typename ...VarArgs>void ThreadNew(const VarArgs &...vaArgs)
    { thread tNewThread{ vaArgs... }; this->swap(tNewThread); }
  /* -- Thread handler function -------------------------------------------- */
  void ThreadHandler(void) try
  { // Incrememt thread running count
    ++cParent->stRunning;
    // Thread longer running
    sbRunning = true;
    // Thread starting up in log
    cLog->LogDebugExSafe("Thread $<$> started.", CtrGet(), IdentGet());
    // Set the start time and initialise the end time
    scdStart = cmHiRes.GetEpochTime();
    scdEnd = seconds(0);
    // Loop forever until thread should exit
    while(ThreadShouldNotExit())
    { // Set exit code to -1 as a reference that execution is proceeding
      ThreadSetExitCode(-1);
      // Run thread and capture result
      ThreadSetExitCode(ThreadGetCallback()(*this));
      // If non-zero then break else start thread again
      if(ThreadGetExitCode()) break;
    } // Thread no longer running
    sbRunning = false;
    // Set shutdown time
    scdEnd = cmHiRes.GetEpochTime();
    // Log if thread didn't signal to exit
    cLog->LogDebugExSafe("Thread $<$> exited in $ with code $<$$>.",
      CtrGet(), IdentGet(),
      StrShortFromDuration(ClockTimePointRangeToClampedDouble(
        ThreadGetEndTime(), ThreadGetStartTime())),
      ThreadGetExitCode(), hex, ThreadGetExitCode());
    // Reduce thread count
    --cParent->stRunning;
  } // exception occured in thread so handle it
  catch(const exception &e)
  { // Thread no longer running
    sbRunning = false;
    // Return error and set thread to exit
    ThreadSetExitCode(-2);
    // Reduce thread count
    --cParent->stRunning;
    // Set shutdown time
    scdEnd = cmHiRes.GetEpochTime();
    // Log if thread didn't signal to exit
    cLog->LogErrorExSafe("Thread $<$> exited in $ due to exception: $!",
      CtrGet(), IdentGet(),
      StrShortFromDuration(ClockTimePointRangeToClampedDouble(
        ThreadGetEndTime(), ThreadGetStartTime())),
      e.what());
  }
  /* ----------------------------------------------------------------------- */
  static void ThreadMain(void*const vpPtr)
  { // Get pointer to thread class and if valid?
    if(Thread*const tPtr = reinterpret_cast<Thread*>(vpPtr))
    { // Set thread name and priority in system
      if(!SysInitThread(tPtr->IdentGetCStr(), tPtr->stPerf))
        cLog->LogWarningExSafe("Thread '$' update priority to $ failed: $!",
          tPtr->IdentGet(), tPtr->ThreadGetPerf(), StrFromErrNo());
      // Run the thread callback
      tPtr->ThreadHandler();
    } // Report the problem
    else cLog->LogErrorSafe(
      "Thread switch to thiscall failed with null class ptr!");
  }
  /* --------------------------------------------------------------- */ public:
  template<typename ReturnType>ReturnType ThreadGetParam(void) const
    { return reinterpret_cast<ReturnType>(vpParam); }
  template<typename ReturnType>void ThreadSetParam(const ReturnType rtParam)
    { vpParam = reinterpret_cast<void*>(rtParam); }
  /* ----------------------------------------------------------------------- */
  const ClkTimePoint ThreadGetStartTime(void) const
    { return ClkTimePoint{ scdStart }; }
  const ClkTimePoint ThreadGetEndTime(void) const
    { return ClkTimePoint{ scdEnd }; }
  /* ----------------------------------------------------------------------- */
  int ThreadGetExitCode(void) const { return siExitCode; }
  void ThreadSetExitCode(const int iNewCode) { siExitCode = iNewCode; }
  /* ----------------------------------------------------------------------- */
  const CBFunc &ThreadGetCallback(void) const { return cbfFunc; }
  bool ThreadHaveCallback(void) const { return !!ThreadGetCallback(); }
  /* ----------------------------------------------------------------------- */
  bool ThreadIsParamSet(void) const { return !!vpParam; }
  /* ----------------------------------------------------------------------- */
  void ThreadSetExit(void)
  { // Ignore if already exited or already signalled to exit
    if(ThreadShouldExit()) return;
    // Set exit
    sbShouldExit = true;
    // Log signalled to exit
    cLog->LogDebugExSafe("Thread $<$> $ to exit.", CtrGet(), IdentGet(),
      ThreadIsCurrent() ? "signalling" : "signalled");
  }
  /* ----------------------------------------------------------------------- */
  void ThreadCancelExit(void) { sbShouldExit = false; }
  /* ----------------------------------------------------------------------- */
  void ThreadWait(void)
  { // Wait for the thread to terminate. It is important to put this here
    // even though the thread explicitly said it's no longer executing
    // because join() HAS to be called in order for thread to be reused, and
    // join() will abort() if joinable() isn't checked as well.
    if(ThreadIsJoinable()) ThreadJoin();
    // No longer executing
    ThreadCancelExit();
  }
  /* ----------------------------------------------------------------------- */
  void ThreadStopNoCheck(void)
  { // Thread is running? Inform thread loops that it should exit now
    if(ThreadIsRunning()) ThreadSetExit();
    // Wait for thread to complete
    ThreadWait();
    // Set to standby with a new thread
    ThreadNew();
  }
  /* ----------------------------------------------------------------------- */
  void ThreadStopNoThrow(void)
  { // If is this thread then this is a bad idea
    if(ThreadIsCurrent())
      return cLog->LogWarningExSafe(
        "Thread '$' tried to join from the same thread!", IdentGet());
    // Proceed with termination
    ThreadStopNoCheck();
  }
  /* ----------------------------------------------------------------------- */
  void ThreadStop(void)
  { // If is this thread then this is a bad idea
    if(ThreadIsCurrent())
      XC("Tried to join from the same thread!", "Identifier", IdentGet());
    // Proceed with termination
    ThreadStopNoCheck();
  }
  /* ----------------------------------------------------------------------- */
  bool ThreadIsCurrent(void) const
    { return ::std::this_thread::get_id() == get_id(); }
  bool ThreadIsNotCurrent(void) const { return !ThreadIsCurrent(); }
  /* ----------------------------------------------------------------------- */
  unsigned int ThreadGetPerf(void) const
    { return static_cast<unsigned int>(stPerf); }
  /* ----------------------------------------------------------------------- */
  bool ThreadShouldExit(void) const { return sbShouldExit; }
  bool ThreadShouldNotExit(void) const { return !ThreadShouldExit(); }
  /* ----------------------------------------------------------------------- */
  bool ThreadIsRunning(void) const { return sbRunning; }
  bool ThreadIsNotRunning(void) const { return !ThreadIsRunning(); }
  /* ----------------------------------------------------------------------- */
  bool ThreadIsJoinable(void) const { return joinable(); }
  bool ThreadIsNotJoinable(void) const { return !ThreadIsJoinable(); }
  /* ----------------------------------------------------------------------- */
  void ThreadJoin(void) { return join(); }
  /* ----------------------------------------------------------------------- */
  bool ThreadIsExited(void) const { return ThreadGetExitCode() != -1; }
  bool ThreadIsNotExited(void) const { return !ThreadIsExited(); }
  /* ----------------------------------------------------------------------- */
  bool ThreadIsException(void) const { return ThreadGetExitCode() == -2; }
  /* ----------------------------------------------------------------------- */
  void ThreadStart(void*const vpPtr = nullptr)
  { // Bail if thread already started
    if(ThreadIsJoinable()) return;
    // Set new thread parameters
    ThreadCancelExit();
    ThreadSetParam(vpPtr);
    ThreadSetExitCode(0);
    // Start the thread and move that thread class to this thread
    ThreadNew(ThreadMain, this);
  }
  /* -- Stop and de-initialise the thread ---------------------------------- */
  void ThreadDeInit(void)
    { ThreadStop(); this->CollectorUnregister(); }
  /* -- Initialise with callback only and register ------------------------- */
  void ThreadInit(const CBFunc &cbfC)
    { cbfFunc = cbfC; this->CollectorRegister(); }
  /* -- Initialise with name and callback ---------------------------------- */
  void ThreadInit(const string &strN, const CBFunc &cbfC)
    { IdentSet(strN); ThreadInit(cbfC); }
  /* -- Initialise with name, callback, parameter and start execute -------- */
  void ThreadInit(const string &strN, const CBFunc &cbfC, void*const vpPtr)
    { ThreadInit(strN, cbfC); ThreadStart(vpPtr); }
  /* -- Initialise with callback, parameter and start execute -------------- */
  void ThreadInit(const CBFunc &cbfC, void*const vpPtr)
    { ThreadInit(cbfC); ThreadStart(vpPtr); }
  /* -- Destructor --------------------------------------------------------- */
  ~Thread(void)
  { // Done if not running
    if(ThreadIsNotJoinable()) return;
    // Not signalled to exit?
    if(ThreadIsRunning() && ThreadShouldNotExit())
    { // Set exit signal
      sbShouldExit = true;
      // Log signalled to exit in destructor
      cLog->LogWarningExSafe("Thread $<$> signalled to exit in destructor.",
        CtrGet(), IdentGet());
    } // Wait to synchronise
    ThreadJoin();
  }
  /* -- Full initialise and execute constructor ---------------------------- */
  Thread(const string &strN,           // Requested Thread name
         const SysThread sP,           // Thread needs high performance?
         const CBFunc &cbfC,           // Requested callback function
         void*const vpPtr) :           // User parameter to store
    /* -- Initialisers ----------------------------------------------------- */
    ICHelperThread{ cThreads, this },  // Automatic (de)registration
    IdentCSlave{ cParent->CtrNext() }, // Initialise identification number
    Ident{ strN },                     // Initialise requested thread name
    ThreadBase{ sP, vpPtr, cbfC },     // Set perf, parameter and callback
    thread{ ThreadMain, this }         // Start the thread straight away
    /* --------------------------------------------------------------------- */
    { }                                // Do nothing else
  /* -- Standby constructor (set everything except user parameter) --------- */
  Thread(const string &strN,           // Requested Thread name
         const SysThread sP,           // Thread needs high performance?
         const CBFunc &cbfC) :         // Requested callback function
    /* -- Initialisers ----------------------------------------------------- */
    ICHelperThread{ cThreads, this },  // Automatic (de)registration
    IdentCSlave{ cParent->CtrNext() }, // Initialise identification number
    Ident{ strN },                     // Set requested identifier
    ThreadBase{ sP, nullptr, cbfC }    // Just set callback function
    /* --------------------------------------------------------------------- */
    { }                                // Do nothing else
  /* -- Standby constructor (set only name) -------------------------------- */
  Thread(const string &strN,           // Requested Thread name
         const SysThread sP) :         // Thread needs high performance?
    /* -- Initialisers ----------------------------------------------------- */
    ICHelperThread{ cThreads },        // No automatic registration
    IdentCSlave{ cParent->CtrNext() }, // Initialise identification number
    Ident{ strN },                     // Set requested identifer
    ThreadBase{ sP, nullptr, nullptr } // Initialise nothing else
    /* --------------------------------------------------------------------- */
    { }                                // Do nothing else
  /* -- Standby constructor ------------------------------------------------ */
  explicit Thread(const SysThread sP) :// Thread needs high performance?
    /* -- Initialisers ----------------------------------------------------- */
    ICHelperThread{ cThreads },        // No automatic registration
    IdentCSlave{ cParent->CtrNext() }, // Initialise identification number
    ThreadBase{ sP, nullptr, nullptr } // Initialise only thread priority
    /* --------------------------------------------------------------------- */
    { }                                // Do nothing else
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(Thread)              // Suppress default functions for safety
};/* ======================================================================= */
CTOR_END(Threads, Thread, THREAD,,,, stRunning{0});
/* -- Thread sync helper --------------------------------------------------- */
template<class Callbacks>class ThreadSyncHelper : private Callbacks
{ /* -------------------------------------------------------------- */ private:
  Thread            &tOwner;           // Reference to thread owner
  SafeBool           bUnlock;          // Synchronisation should occur?
  bool               bAToB,            // Release lock for thread A?
                     bBToA;            // Release lock for thread B?
  mutex              mAToB,            // Thread A to Thread B mutex
                     mBToA;            // Thread B to Thread A mutex
  condition_variable cvAToB,           // Thread A to Thread B notification
                     cvBToA;           // Thread B to Thread A notification
  /* ----------------------------------------------------------------------- */
  void SendNotification(mutex &mM, condition_variable &cvCV, bool &bUL)
  { // Acquire unique lock
    const UniqueLock ulGuard{ mM };
    // Modify the unlock boolean
    bUL = true;
    // Send notification
    cvCV.notify_one();
  }
  /* ----------------------------------------------------------------------- */
  void WaitForThreadNotification(mutex &mM, condition_variable &cvCV,
    bool &bUL)
  { // Setup lock for condition variable
    UniqueLock uLock{ mM };
    // Wait for Thread A to notify us to continue.
    cvCV.wait(uLock, [this, &bUL]{ return bUL || tOwner.ThreadShouldExit(); });
    // Reset locked boolean
    bUL = false;
  }
  /* ----------------------------------------------------------------------- */
  void SendNotificationToThreadA(void)
    { SendNotification(mBToA, cvBToA, bBToA); }
  void WaitForThreadANotification(void)
    { WaitForThreadNotification(mAToB, cvAToB, bAToB); }
  void SendNotificationToThreadB(void)
    { SendNotification(mAToB, cvAToB, bAToB); }
  void WaitForThreadBNotification(void)
    { WaitForThreadNotification(mBToA, cvBToA, bBToA); }
  /* --------------------------------------------------------------- */ public:
  void CheckStateInThreadA(void)
  { // Ignore if other thread doesn't need action
    if(!bUnlock) return;
    // Unlock progressing
    bUnlock = false;
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
  void FinishNotifyThreadA(void)
  { // Ignore if render thread isn't running
    if(tOwner.ThreadIsNotRunning()) return;
    // Call final Thread B callback
    this->FinishThreadBCallback();
    // Send notification to Thread A
    SendNotificationToThreadA();
    // Wait for Thread A to send notification back to us in Thread B
    WaitForThreadANotification();
  }
  /* -- Begin notification to Thread A from Thread B ----------------------- */
  void StartNotifyThreadA(void)
  { // Ignore if render thread isn't running
    if(tOwner.ThreadIsNotRunning()) return;
    // Set state so Thread A thread knows to proceed
    bUnlock = true;
    // Wait for Thread A to send notification back to us in Thread B
    WaitForThreadANotification();
    // Now processing Thread B start procedure
    this->StartThreadBCallback();
  }
  /* -- Constructor -------------------------------------------------------- */
  explicit ThreadSyncHelper(Thread &tO) : // Thread being used
    /* -- Initialisers ----------------------------------------------------- */
    tOwner(tO),                        // Set thread owner
    bUnlock(false),                    // Initially unlocked
    bAToB(false),                      // Not sending msg from Thread A to B
    bBToA(false)                       // Not sending msg from Thread B to A
    /* --------------------------------------------------------------------- */
    { }                                // Do nothing else
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(ThreadSyncHelper)    // Suppress default functions for safety
};/* ----------------------------------------------------------------------- */
static size_t ThreadGetRunning(void) { return cThreads->stRunning; }
/* ------------------------------------------------------------------------- */
};                                     // End of public namespace
/* ------------------------------------------------------------------------- */
};                                     // End of private namespace
/* == EoF =========================================================== EoF == */
