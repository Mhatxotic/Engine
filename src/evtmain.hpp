/* == EVTMAIN.HPP ========================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## This is the engine events class where the engine can queue messages ## **
** ## to be executed in the engine thread.                                ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IEvtMain {                   // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IEvtCore::P;           using namespace IHelper::P;
using namespace IIdent::P;             using namespace ILog::P;
using namespace IMutex::P;             using namespace IStd::P;
using namespace ISysUtil::P;           using namespace IThread::P;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* -- Available engine commands -------------------------------------------- */
enum EvtMainCmd : size_t               // Engine thread event commands
{ /* -- Main events -------------------------------------------------------- */
  EMC_NONE,                            // 00: No event occured
  EMC_SUSPEND,                         // 01: Suspend and wait for signal
  /* -- Quit events -------------------------------------------------------- */
  EMC_QUIT,                            // 02: Main loop should quit
  EMC_QUIT_RESTART,                    // 03: Cleanly quit and restart app
  EMC_QUIT_RESTART_NP,                 // 04: Same as above but without args
  EMC_QUIT_THREAD,                     // 05: Thread loop should quit
  EMC_QUIT_VREINIT,                    // 06: As above but reinit open gl
  /* -- Lua events --------------------------------------------------------- */
  EMC_LUA_ASK_EXIT,                    // 07: To LUA asking to clean up & exit
  EMC_LUA_CONFIRM_EXIT,                // 08: From LUA to confirm the exit
  EMC_LUA_END,                         // 09: End LUA execution
  EMC_LUA_PAUSE,                       // 10: Pause LUA execution
  EMC_LUA_REDRAW,                      // 11: Ask LUA guest to redraw fbo's
  EMC_LUA_REINIT,                      // 12: Reinit LUA execution
  EMC_LUA_RESUME,                      // 13: Resume LUA execution
  /* -- Window events ------------------------------------------------------ */
  EMC_WIN_CLOSE,                       // 14: Window wants to be closed
  EMC_WIN_FOCUS,                       // 15: Window focus was changed
  EMC_WIN_ICONIFY,                     // 16: Window was minimised or restored
  EMC_WIN_MOVED,                       // 17: Window was moved
  EMC_WIN_REFRESH,                     // 18: Window content needs refresh
  EMC_WIN_RESIZED,                     // 19: Window was resized
  EMC_WIN_SCALE,                       // 20: Window content scale changed
  /* -- OpenGL events ------------------------------------------------------ */
  EMC_VID_FB_REINIT,                   // 21: Reset frame buffer
  EMC_VID_MATRIX_REINIT,               // 22: Reset matrix
  EMC_VID_REINIT,                      // 23: Reset open gl
  /* -- Audio events ------------------------------------------------------- */
  EMC_AUD_CD_UPDATED,                  // 24: Capture devices updated
  EMC_AUD_PD_UPDATED,                  // 25: Playback devices updated
  EMC_AUD_REINIT,                      // 26: Reinitialise audio
  /* -- Input events ------------------------------------------------------- */
  EMC_INP_DRAG_DROP,                   // 27: Files dragged and dropped
  EMC_INP_JOY_STATE,                   // 28: Joystick status changed
  EMC_INP_MOUSE_FOCUS,                 // 29: Mouse moved (in|out)side window
  EMC_INP_PASTE,                       // 30: Paste into input from clipboard
  /* -- Unique async events ------------------------------------------------ */
  EMC_CB_EVENT,                        // 31: Clipboard event occured
  EMC_CUR_EVENT,                       // 32: Cursor event occured
  EMC_STR_EVENT,                       // 33: Stream event occured
  EMC_VID_EVENT,                       // 34: Video event occured
  /* -- Console events------------------------------------------------------ */
  EMC_CON_UPDATE,                      // 35: Force syscon display update
  /* -- Input events ------------------------------------------------------- */
  EMC_INP_CHAR,                        // 36: Filtered key pressed
  EMC_INP_KEYPRESS,                    // 37: Unfiltered key pressed
  EMC_INP_MOUSE_CLICK,                 // 38: Mouse button clicked
  EMC_INP_MOUSE_MOVE,                  // 39: Mouse cursor moved
  EMC_INP_MOUSE_SCROLL,                // 40: Mouse wheel scrolled
  /* -- Loading async events ----------------------------------------------- */
  EMC_MP_ARCHIVE,                      // 41: Archive async event occured
  EMC_MP_ASSET,                        // 42: Asset async event occured
  EMC_MP_FONT,                         // 43: Char async event occured
  EMC_MP_IMAGE,                        // 44: Image async event occured
  EMC_MP_JSON,                         // 45: Json async event occured
  EMC_MP_PCM,                          // 46: Pcm async event occured
  EMC_MP_PROCESS,                      // 47: Process async event occured
  EMC_MP_SOCKET,                       // 48: Socket async event occured
  EMC_MP_STREAM,                       // 49: Stream async event occured
  EMC_MP_VIDEO,                        // 50: Video async event occured
  /* ----------------------------------------------------------------------- */
  EMC_MAX,                             // 51: Below are just codes
  /* ----------------------------------------------------------------------- */
  EMC_LUA_ERROR,                       // 52: Error in LUA exec (not an event)
  /* ----------------------------------------------------------------------- */
#if defined(ALPHA)                     // Compiling debug version?
  EMC_NOLOG = EMC_MAX                  // Log all events
#else                                  // Compiling on beta or release?
  EMC_NOLOG = EMC_INP_CHAR             // Suppress log from this event forwards
#endif                                 // Build check
};/* -- Remember to update the id strings at EvtMain constructor ----------- */
class EvtMain;                         // Prototype class
static EvtMain *cEvtMain = nullptr;    // Address of global class
class EvtMain :                        // Event list for render thread
  /* -- Dependencies ------------------------------------------------------- */
  private IdList<EMC_MAX>,             // Event strings
  public EvtCore                       // Events common class
   <EvtMainCmd,                        // The enum list of events supported
    EMC_MAX,                           // Maximum events allowed
    EMC_NONE,                          // Event id for NONE
    EMC_NOLOG>,                        // Event id for NOLOG
  public Thread,                       // Engine thread
  private condition_variable           // CV for suspending the thread
{ /* -- Private --------------------------------------------------- */ private:
  const RegAuto    raEvents;           // Events to register
  EvtMainCmd       emcPending,         // Event fired before exit requested
                   emcExit;            // Thread exit code
  unsigned int     uiConfirm;          // Exit confirmation progress
  bool             bUnsuspend;         // Waiting for unsuspend signal
  Mutex            mSuspend;           // Mutex for suspending engine thread
  /* -- A suspend event is requested? -------------------------------------- */
  void OnSuspend(const Event &eEvent)
  { // Wait for subsequent uses to complete
    mSuspend.MutexUniqueCall([this, &eEvent](UniqueLock &ulLock){
      // Log starting of suspension
      cLog->LogDebugSafe("EvtMain suspending engine thread...");
       // Get reference to argument stack
      const EvtArgs &eaArgs = eEvent.eaArgs;
      // Tell requesting thread that it may continue
      eaArgs[0].Ptr<SafeBool>()->store(true);
      eaArgs[1].Ptr<condition_variable>()->notify_one();
      // Wait for thread termination or calling thread to finish
      wait(ulLock, [this]{ return bUnsuspend; });
      // Reset suspension state and resume execution
      bUnsuspend = false;
      // Log finish of suspension
      cLog->LogDebugSafe("EvtMain suspension of engine thread complete.");
    });
  }
  /* -- Unsuspend the thread after a EVT_SUSPEND event ------------- */ public:
  void Unsuspend()
  { // Lock the unlock variable
    mSuspend.MutexCall([this](){
      // Return if already suspended
      if(bUnsuspend) return;
      // Unsuspend the condition variable wait
      bUnsuspend = true;
      // Unblock the condition variable wait
      notify_one();
      // Log finish of suspension
      cLog->LogDebugSafe("EvtMain signalling engine thread end suspension.");
    });
  }
  /* -- Get exit reason code ----------------------------------------------- */
  EvtMainCmd GetExitReason() const { return emcExit; }
  /* -- Set exit reason code ----------------------------------------------- */
  void SetExitReason(const EvtMainCmd emcReason)
  { // Ignore if this is already the code
    if(emcExit == emcReason) return;
    // Set the code
    emcExit = emcReason;
    // Log the change
    cLog->LogDebugExSafe("EvtMain set exit reason to $!", emcReason);
  }
  /* -- Check exit reason -------------------------------------------------- */
  bool IsExitReason(const EvtMainCmd emcReason) const
    { return emcExit == emcReason; }
  bool IsNotExitReason(const EvtMainCmd emcReason) const
    { return !IsExitReason(emcReason); }
  /* -- Incase of error we need to update the exit code -------------------- */
  bool ExitRequested() const { return !!uiConfirm; }
  /* -- Incase of error we need to update the exit code -------------------- */
  void UpdateConfirmExit()
  { // Ignore if not in a confirmation request
    if(!uiConfirm) return;
    // Reset confirmation
    uiConfirm = 0;
    // Set exit code and clear stored exit code
    emcExit = emcPending;
    emcPending = EMC_NONE;
  }
  /* -- Informs LUA that the user wants to quit ---------------------------- */
  void ConfirmExit(const EvtMainCmd emcWhat)
  { // A confirmation request is currently in progress?
    if(uiConfirm)
    { // Set the new exit code overriding the new one.
      emcPending = emcWhat;
      // Still waiting for confirmation
      return;
    } // Exit confirming
    uiConfirm = 1;
    // Ask lua to quit when it is ready. By default it will send confirm.
    Execute(EMC_LUA_ASK_EXIT, emcWhat);
    // Set the new exit code
    emcPending = emcWhat;
  }
  /* -- Handle events from parallel loop ----------------------------------- */
  bool HandleSafe()
  { // Main thread requested break? Why bother managing events?
    if(ThreadShouldExit()) return false;
    // Which event?
    switch(Manage())
    { // Thread should quit? Tell main loop to loop again
      case EMC_QUIT_THREAD: emcExit = EMC_QUIT_THREAD; break;
      // Thread should quit to reinit opengl? Tell main loop to loop again
      case EMC_QUIT_VREINIT: emcExit = EMC_QUIT_VREINIT; break;
      // Thread should quit and main thread should restart completely
      case EMC_QUIT_RESTART: ConfirmExit(EMC_QUIT_RESTART); goto t;
      // Same as above but without command line parameters
      case EMC_QUIT_RESTART_NP: ConfirmExit(EMC_QUIT_RESTART_NP); goto t;
      // Lua executing is ending
      case EMC_LUA_END: ConfirmExit(EMC_LUA_END); goto t;
      // Lua executing is reinitialising
      case EMC_LUA_REINIT: ConfirmExit(EMC_LUA_REINIT); goto t;
      // Lua confirmed exit is allowed now so return the code we recorded
      case EMC_LUA_CONFIRM_EXIT: if(!uiConfirm) goto t;
        // Restore original exit code, reset exit and confirmation codes
        emcExit = emcPending;
        emcPending = EMC_NONE;
        uiConfirm = 0;
        // Break main loop
        break;
      // Thread and main thread should quit so tell thread to break.
      case EMC_QUIT: ConfirmExit(EMC_QUIT); goto t;
      // Other event, thread shouldn't break
      default:t: return true;
    } // Thread should terminate
    ThreadSetExit();
    // Thread should break
    return false;
  }
  /* -- Confirm to the engine that Lua is aborting execution --------------- */
  void ConfirmExit() { Add(EMC_LUA_CONFIRM_EXIT); }
  /* -- Add event to quit the engine --------------------------------------- */
  void RequestQuit() { Add(EMC_QUIT); }
  /* -- Add event to quit thread and restart window manager ---------------- */
  void RequestQuitThread() { Add(EMC_QUIT_THREAD); }
  /* -- Add event to quit thread and wait for it to complete --------------- */
  void RequestQuitThreadWait() { Add(EMC_QUIT_THREAD); }
  /* -- Add event to quit thread and restart opengl ------------------------ */
  void RequestGLReInit() { Add(EMC_QUIT_VREINIT); }
  /* -- Add event to quit thread and restart opengl and wait --------------- */
  void RequestGLReInitWait() { RequestGLReInit(); ThreadJoin(); }
  /* -- Constructor --------------------------------------------- */ protected:
  EvtMain() :
    /* -- Initialisers ----------------------------------------------------- */
    IdList{{                           // Build event list
#define EMC(x) STR(EMC_ ## x)          // Helper to define event id strings
      EMC(NONE),            EMC(SUSPEND),          EMC(QUIT),
      EMC(QUIT_RESTART),    EMC(QUIT_RESTART_NP),  EMC(QUIT_THREAD),
      EMC(QUIT_VREINIT),    EMC(LUA_ASK_EXIT),     EMC(LUA_CONFIRM_EXIT),
      EMC(LUA_END),         EMC(LUA_PAUSE),        EMC(LUA_REDRAW),
      EMC(LUA_REINIT),      EMC(LUA_RESUME),       EMC(WIN_CLOSE),
      EMC(WIN_FOCUS),       EMC(WIN_ICONIFY),      EMC(WIN_MOVED),
      EMC(WIN_REFRESH),     EMC(WIN_RESIZED),      EMC(WIN_SCALE),
      EMC(VID_FB_REINIT),   EMC(VID_MATRIX_REINIT),EMC(VID_REINIT),
      EMC(AUD_CD_UPDATED),  EMC(AUD_PD_UPDATED),   EMC(AUD_REINIT),
      EMC(INP_DRAG_DROP),   EMC(INP_JOY_STATE),    EMC(INP_MOUSE_FOCUS),
      EMC(INP_PASTE),       EMC(CB_EVENT),         EMC(CUR_EVENT),
      EMC(STR_EVENT),       EMC(VID_EVENT),        EMC(CON_UPDATE),
      EMC(INP_CHAR),        EMC(INP_KEYPRESS),     EMC(INP_MOUSE_CLICK),
      EMC(INP_MOUSE_MOVE),  EMC(INP_MOUSE_SCROLL), EMC(MP_ARCHIVE),
      EMC(MP_ASSET),        EMC(MP_FONT),          EMC(MP_IMAGE),
      EMC(MP_JSON),         EMC(MP_PCM),           EMC(MP_PROCESS),
      EMC(MP_SOCKET),       EMC(MP_STREAM),        EMC(MP_VIDEO)
#undef EMC                             // Done with this macro
    }},
    EvtCore{ "EventMain", *this },     // Construct core
    Thread{ "engine", STP_ENGINE },    // Set up high perf engine thread
    raEvents{ this, {                  // Initialise custom handled events
      { EMC_SUSPEND,                 bind(&EvtMain::OnSuspend, this, _1) },
      { EMC_QUIT,             nullptr }, { EMC_QUIT_RESTART,     nullptr },
      { EMC_QUIT_RESTART_NP,  nullptr }, { EMC_QUIT_THREAD,      nullptr },
      { EMC_QUIT_VREINIT,     nullptr }, { EMC_LUA_END,          nullptr },
      { EMC_LUA_REINIT,       nullptr }, { EMC_LUA_CONFIRM_EXIT, nullptr }
    } },
    emcPending(EMC_NONE),              // Not exiting yet
    emcExit(EMC_NONE),                 // Not exited yet
    uiConfirm(0),                      // Exit not confirmed yet
    bUnsuspend(false)                  // Not suspended yet
    /* -- Set global pointer to static class and register events ----------- */
    { cEvtMain = this; }
};/* ----------------------------------------------------------------------- */
typedef EvtMain::EvtArgs EvtMainArgs;  // Event callback arguments
typedef EvtMain::Event   EvtMainEvent; // Event command
typedef EvtMain::RegAuto EvtMainRegAuto; // Event (de)registration
/* ------------------------------------------------------------------------- */
};                                     // End of public module namespace
/* ------------------------------------------------------------------------- */
};                                     // End of private module namespace
/* == EoF =========================================================== EoF == */
