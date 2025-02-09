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
using namespace ICollector::P;         using namespace IEvtCore::P;
using namespace ILog::P;               using namespace IStd::P;
using namespace ISysUtil::P;           using namespace IThread::P;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* -- Available engine commands -------------------------------------------- */
enum EvtMainCmd : size_t               // Engine thread event commands
{ /* -- Main events -------------------------------------------------------- */
  EMC_NONE,                            // 00: No event occured
  /* -- Quit events -------------------------------------------------------- */
  EMC_QUIT,                            // 01: Main loop should quit
  EMC_QUIT_THREAD,                     // 02: Thread loop should quit
  EMC_QUIT_RESTART,                    // 03: Cleanly quit and restart app
  EMC_QUIT_RESTART_NP,                 // 04: Same as above but without args
  /* -- Lua events --------------------------------------------------------- */
  EMC_LUA_ASK_EXIT,                    // 05: To LUA asking to clean up & exit
  EMC_LUA_CONFIRM_EXIT,                // 06: From LUA to confirm the exit
  EMC_LUA_END,                         // 07: End LUA execution
  EMC_LUA_PAUSE,                       // 08: Pause LUA execution
  EMC_LUA_REDRAW,                      // 09: Ask LUA guest to redraw fbo's
  EMC_LUA_REINIT,                      // 10: Reinit LUA execution
  EMC_LUA_RESUME,                      // 11: Resume LUA execution
  /* -- Window events ------------------------------------------------------ */
  EMC_WIN_CLOSE,                       // 12: Window wants to be closed
  EMC_WIN_FOCUS,                       // 13: Window focus was changed
  EMC_WIN_ICONIFY,                     // 14: Window was minimised or restored
  EMC_WIN_MOVED,                       // 15: Window was moved
  EMC_WIN_REFRESH,                     // 16: Window content needs refresh
  EMC_WIN_RESIZED,                     // 17: Window was resized
  EMC_WIN_SCALE,                       // 18: Window content scale changed
  /* -- OpenGL events ------------------------------------------------------ */
  EMC_VID_FB_REINIT,                   // 19: Reset frame buffer
  EMC_VID_MATRIX_REINIT,               // 20: Reset matrix
  /* -- Audio events ------------------------------------------------------- */
  EMC_AUD_REINIT,                      // 21: Re-initialise audio
  /* -- Input events ------------------------------------------------------- */
  EMC_INP_DRAG_DROP,                   // 22: Files dragged and dropped
  EMC_INP_JOY_STATE,                   // 23: Joystick status changed
  EMC_INP_MOUSE_FOCUS,                 // 24: Mouse moved (in|out)side window
  EMC_INP_PASTE,                       // 25: Paste into input from clipboard
  /* -- Unique async events ------------------------------------------------ */
  EMC_CB_EVENT,                        // 26: Clipboard event occured
  EMC_CUR_EVENT,                       // 27: Cursor event occured
  EMC_STR_EVENT,                       // 28: Stream event occured
  EMC_VID_EVENT,                       // 29: Video event occured
  /* -- Console events------------------------------------------------------ */
  EMC_CON_UPDATE,                      // 30: Force syscon display update
  EMC_CON_KEYPRESS,                    // 31: Console key pressed (NoLog here)
  /* -- Input events ------------------------------------------------------- */
  EMC_INP_CHAR,                        // 32: Filtered key pressed
  EMC_INP_KEYPRESS,                    // 33: Unfiltered key pressed
  EMC_INP_MOUSE_CLICK,                 // 34: Mouse button clicked
  EMC_INP_MOUSE_MOVE,                  // 35: Mouse cursor moved
  EMC_INP_MOUSE_SCROLL,                // 36: Mouse wheel scrolled
  /* -- Loading async events ----------------------------------------------- */
  EMC_MP_ARCHIVE,                      // 37: Archive async event occured
  EMC_MP_ASSET,                        // 38: Asset async event occured
  EMC_MP_FONT,                         // 39: Char async event occured
  EMC_MP_IMAGE,                        // 40: Image async event occured
  EMC_MP_JSON,                         // 41: Json async event occured
  EMC_MP_PCM,                          // 42: Pcm async event occured
  EMC_MP_PROCESS,                      // 43: Process async event occured
  EMC_MP_SOCKET,                       // 44: Socket async event occured
  EMC_MP_STREAM,                       // 45: Stream async event occured
  EMC_MP_VIDEO,                        // 46: Video async event occured
  /* ----------------------------------------------------------------------- */
  EMC_MAX,                             // 47: Below are just codes
  /* ----------------------------------------------------------------------- */
  EMC_LUA_ERROR,                       // 48: Error in LUA exec (not an event)
  /* ----------------------------------------------------------------------- */
#if defined(ALPHA)                     // Compiling debug version?
  EMC_NOLOG = EMC_MAX                  // Log all events
#else                                  // Compiling on beta or release?
  EMC_NOLOG = EMC_CON_KEYPRESS         // Suppress log from this event forwards
#endif                                 // Build check
};/* -- Remember to update the id strings at EvtMain constructor ----------- */
static class EvtMain final :           // Event list for render thread
  /* -- Dependencies ------------------------------------------------------- */
  private IHelper,                     // Initialisation helper
  public EvtCore                       // Events common class
   <EvtMainCmd,                        // The enum list of events supported
    EMC_MAX,                           // Maximum events allowed
    EMC_NONE,                          // Event id for NONE
    EMC_NOLOG>,                        // Event id for NOLOG
  public Thread                        // Engine thread
{ /* -- Private --------------------------------------------------- */ private:
  EvtMainCmd       emcPending;         // Event fired before exit requested
  EvtMainCmd       emcExit;            // Thread exit code
  unsigned int     uiConfirm;          // Exit confirmation progress
  /* -- Events list -------------------------------------------------------- */
  const RegVec     rvEvents;           // Events list to register
  /* -- Check events (called from Main) ------------------------------------ */
  bool ProcessResult(const EvtMainCmd emcResult)
  { // Which event?
    switch(emcResult)
    { // Thread should quit. Tell main loop to loop again
      case EMC_QUIT_THREAD: emcExit = EMC_QUIT_THREAD;
        return false;
      // Thread should quit and main thread should restart completely
      case EMC_QUIT_RESTART: ConfirmExit(EMC_QUIT_RESTART); break;
      // Same as above but without command line parameters
      case EMC_QUIT_RESTART_NP: ConfirmExit(EMC_QUIT_RESTART_NP); break;
      // Lua executing is ending
      case EMC_LUA_END: ConfirmExit(EMC_LUA_END); break;
      // Lua executing is re-initialising
      case EMC_LUA_REINIT: ConfirmExit(EMC_LUA_REINIT); break;
      // Lua confirmed exit is allowed now so return the code we recorded
      case EMC_LUA_CONFIRM_EXIT: if(!uiConfirm) break;
        // Restore original exit code, reset exit and confirmation codes
        emcExit = emcPending;
        emcPending = EMC_NONE;
        uiConfirm = 0;
        // Break main loop
        return false;
      // Thread and main thread should quit so tell thread to break.
      case EMC_QUIT: ConfirmExit(EMC_QUIT); break;
      // Other event, thread shouldn't break
      default: break;
    } // Thread shouldn't break
    return true;
  }
  /* -- Get exit reason code --------------------------------------- */ public:
  EvtMainCmd GetExitReason(void) const { return emcExit; }
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
  bool ExitRequested(void) const { return !!uiConfirm; }
  /* -- Incase of error we need to update the exit code -------------------- */
  void UpdateConfirmExit(void)
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
  bool HandleSafe(void)
  { // Main thread requested break? Why bother managing events?
    if(ThreadShouldExit()) return false;
    // Handle event and return true if nothing special happened
    if(ProcessResult(ManageSafe())) return true;
    // Thread should terminate
    ThreadSetExit();
    // Thread should break
    return false;
  }
  /* -- Handle events from serialised loop --------------------------------- */
  bool HandleUnsafe(void)
  { // Main thread requested break? Why bother managing events?
    if(ThreadShouldExit()) return false;
    // Handle event and return true if nothing special happened
    if(ProcessResult(ManageUnsafe())) return true;
    // Thread should terminate
    ThreadSetExit();
    // Thread should break
    return false;
  }
  /* -- Confirm to the engine that Lua is aborting execution --------------- */
  void ConfirmExit(void) { Add(EMC_LUA_CONFIRM_EXIT); }
  /* -- Add event to quit the engine --------------------------------------- */
  void RequestQuit(void) { Add(EMC_QUIT); }
  /* -- Add event to quit thread and restart window manager ---------------- */
  void RequestQuitThread(void) { Add(EMC_QUIT_THREAD); }
  /* -- Initialise base events (called from Main) -------------------------- */
  void Init(void)
  { // Class initialised
    IHInitialise();
    // Registrer base events
    RegisterEx(rvEvents);
  }
  /* -- DeInitialise base events (called from Main) ------------------------ */
  void DeInit(void)
  { // Ignore if class not initialised
    if(IHNotDeInitialise()) return;
    // Registrer base events
    UnregisterEx(rvEvents);
  }
  /* -- Destructor --------------------------------------------------------- */
  DTORHELPER(~EvtMain)
  /* -- Constructor -------------------------------------------------------- */
  EvtMain(void) :
    /* -- Initialisers ----------------------------------------------------- */
    IHelper{ __FUNCTION__ },           // Initialise helper
    EvtCore{ "EventMain", ISList{{     // Set name of this object
      /* ------------------------------------------------------------------- */
#define EMC(x) STR(EMC_ ## x)          // Helper to define event id strings
      /* ------------------------------------------------------------------- */
      EMC(NONE),
      /* ------------------------------------------------------------------- */
      EMC(QUIT),            EMC(QUIT_THREAD),      EMC(QUIT_RESTART),
      EMC(QUIT_RESTART_NP),
      /* ------------------------------------------------------------------- */
      EMC(LUA_ASK_EXIT),    EMC(LUA_CONFIRM_EXIT), EMC(LUA_END),
      EMC(LUA_PAUSE),       EMC(LUA_REDRAW),       EMC(LUA_REINIT),
      EMC(LUA_RESUME),
      /* ------------------------------------------------------------------- */
      EMC(WIN_FOCUS),       EMC(WIN_ICONIFY),      EMC(WIN_MOVED),
      EMC(WIN_REFRESH),     EMC(WIN_RESIZED),      EMC(WIN_SCALE),
      EMC(WIN_CLOSE),
      /* ------------------------------------------------------------------- */
      EMC(VID_FB_REINIT),   EMC(VID_MATRIX_REINIT),
      /* ------------------------------------------------------------------- */
      EMC(AUD_REINIT),
      /* ------------------------------------------------------------------- */
      EMC(INP_DRAG_DROP),   EMC(INP_JOY_STATE),    EMC(INP_MOUSE_FOCUS),
      EMC(INP_PASTE),
      /* ------------------------------------------------------------------- */
      EMC(CB_EVENT),        EMC(CUR_EVENT),        EMC(STR_EVENT),
      EMC(VID_EVENT),
      /* ------------------------------------------------------------------- */
      EMC(CON_UPDATE),      EMC(CON_KEYPRESS),
      /* ------------------------------------------------------------------- */
      EMC(INP_CHAR),        EMC(INP_KEYPRESS),     EMC(INP_MOUSE_CLICK),
      EMC(INP_MOUSE_MOVE),  EMC(INP_MOUSE_SCROLL),
      /* ------------------------------------------------------------------- */
      EMC(MP_ARCHIVE),      EMC(MP_ASSET),         EMC(MP_FONT),
      EMC(MP_IMAGE),        EMC(MP_JSON),          EMC(MP_PCM),
      EMC(MP_PROCESS),      EMC(MP_SOCKET),        EMC(MP_STREAM),
      EMC(MP_VIDEO),
      /* ------------------------------------------------------------------- */
#undef EMC                             // Done with this macro
      /* ------------------------------------------------------------------- */
    }}},
    Thread{ "engine", STP_ENGINE },    // Set up high perf engine thread
    emcPending(EMC_NONE),              // Not exiting yet
    emcExit(EMC_NONE),                 // Not exited yet
    uiConfirm(0),                      // Exit not confirmed yet
    /* --------------------------------------------------------------------- */
    rvEvents{                          // Initialise custom handled events
      { EMC_QUIT,             nullptr }, { EMC_QUIT_THREAD,      nullptr },
      { EMC_QUIT_RESTART,     nullptr }, { EMC_QUIT_RESTART_NP,  nullptr },
      { EMC_LUA_END,          nullptr }, { EMC_LUA_REINIT,       nullptr },
      { EMC_LUA_CONFIRM_EXIT, nullptr }
    }
    /* --------------------------------------------------------------------- */
    { }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(EvtMain)             // Suppress default functions for safety
  /* -- End ---------------------------------------------------------------- */
} *cEvtMain = nullptr;                 // Pointer to static class
/* ------------------------------------------------------------------------- */
typedef EvtMain::Args   EvtMainArgs;   // Shortcut to EvtMain::Args class
typedef EvtMain::Event  EvtMainEvent;  // Shortcut to EvtMain::Cell class
typedef EvtMain::RegVec EvtMainRegVec; // Shortcut to EvtMain::RegVec class
/* ------------------------------------------------------------------------- */
};                                     // End of public module namespace
/* ------------------------------------------------------------------------- */
};                                     // End of private module namespace
/* == EoF =========================================================== EoF == */
