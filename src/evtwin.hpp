/* == EVTWIN.HPP =========================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## This is the engine events class where the engine can queue messages ## **
** ## to be executed in the engine thread.                                ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IEvtWin {                    // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IEvtCore::P;           using namespace IGlFWUtil::P;
using namespace ILog::P;
using namespace IStd::P;               using namespace ISysUtil::P;
using namespace IThread::P;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* -- Available engine commands -------------------------------------------- */
enum EvtWinCmd : size_t                // Render thread event commands
{ /* -- Main events -------------------------------------------------------- */
  EWC_NONE,                            // 00: No event occured
  /* -- Window events ------------------------------------------------------ */
  EWC_WIN_ATTENTION,                   // 01: Request attention
  EWC_WIN_CENTRE,                      // 02: Put window in centre
  EWC_WIN_CURPOSGET,                   // 03: Get cursor position
  EWC_WIN_CURRESET,                    // 04: Reset cursor
  EWC_WIN_CURSET,                      // 05: Set cursor
  EWC_WIN_CURSETVIS,                   // 06: Set cursor visibility
  EWC_WIN_FOCUS,                       // 07: Focus window
  EWC_WIN_HIDE,                        // 08: Hide the window
  EWC_WIN_LIMITS,                      // 09: Window limits change
  EWC_WIN_MAXIMISE,                    // 10: Maximise window
  EWC_WIN_MINIMISE,                    // 11: Minimise window
  EWC_WIN_MOVE,                        // 12: Move window
  EWC_WIN_RESET,                       // 13: Reset window position and size
  EWC_WIN_RESIZE,                      // 14: Resize window
  EWC_WIN_RESTORE,                     // 15: Restore window
  EWC_WIN_SETICON,                     // 16: Set window icon
  EWC_WIN_SETRAWMOUSE,                 // 17: Set raw mouse motion
  EWC_WIN_SETSTKKEYS,                  // 18: Set sticky keys state
  EWC_WIN_SETSTKMOUSE,                 // 19: Set sticky mouse buttons state
  EWC_WIN_TOGGLE_FS,                   // 20: Toggle full-screen
  /* -- Clipboard events --------------------------------------------------- */
  EWC_CB_GET,                          // 21: Get clipboard (via Clip class)
  EWC_CB_SET,                          // 22: Set clipboard (via Clip class)
  EWC_CB_SETNR,                        // 23: " but no callback
  /* ----------------------------------------------------------------------- */
  EWC_WIN_CURPOSSET,                   // 24: Set cursor position
  /* ----------------------------------------------------------------------- */
  EWC_MAX,                             // 25: Maximum number of async events
  /* ----------------------------------------------------------------------- */
#if defined(ALPHA)                     // Compiling debug version?
  EWC_NOLOG = EWC_MAX                  // Log all events
#else                                  // Compiling on beta or release?
  EWC_NOLOG = EWC_WIN_CURPOSSET        // Suppress log from this event forwards
#endif                                 // Build check
};/* -- Remember to update the id strings at EvtWin constructor ------------ */
static class EvtWin final :            // Event list for window thread
  /* -- Dependencies ------------------------------------------------------- */
  public EvtCore                       // Events common class
   <EvtWinCmd,                         // The enum list of events supported
    EWC_MAX,                           // Maximum events allowed
    EWC_NONE,                          // Event id for NONE
    EWC_NOLOG>                         // Event id for NOLOG
{ /* -- Add with copy parameter semantics (starter) ---------------- */ public:
  template<typename ...VarArgs>
    void AddUnblock(const EvtWinCmd ewcCmd, const VarArgs &...vaArgs)
  { // Prepare parameters list and add a new event
    Add(ewcCmd, vaArgs...);
    // Unblock the window thread
    GlFWForceEventHack();
  }
  /* -- Destructor --------------------------------------------------------- */
  DTORHELPER(~EvtWin)
  /* -- Constructor -------------------------------------------------------- */
  EvtWin(void) :
    /* -- Initialisers ----------------------------------------------------- */
    EvtCore{ "EventWin", ISList{{      // Set name of this object
      /* ------------------------------------------------------------------- */
#define EWC(x) STR(EWC_ ## x)          // Helper to define event id strings
      /* ------------------------------------------------------------------- */
      EWC(NONE),
      /* ------------------------------------------------------------------- */
      EWC(WIN_ATTENTION),   EWC(WIN_CENTRE),      EWC(WIN_CURPOSGET),
      EWC(WIN_CURRESET),    EWC(WIN_CURSET),      EWC(WIN_CURSETVIS),
      EWC(WIN_FOCUS),       EWC(WIN_HIDE),        EWC(WIN_LIMITS),
      EWC(WIN_MAXIMISE),    EWC(WIN_MINIMISE),    EWC(WIN_MOVE),
      EWC(WIN_RESET),       EWC(WIN_RESIZE),      EWC(WIN_RESTORE),
      EWC(WIN_SETICON),     EWC(WIN_SETRAWMOUSE), EWC(WIN_SETSTKKEYS),
      EWC(WIN_SETSTKMOUSE), EWC(WIN_TOGGLE_FS),
      /* ------------------------------------------------------------------- */
      EWC(CB_GET),          EWC(CB_SET),          EWC(CB_SETNR),
      /* ------------------------------------------------------------------- */
      EWC(WIN_CURPOSSET)
      /* ------------------------------------------------------------------- */
#undef EWC                             // Done with this macro
      /* ------------------------------------------------------------------- */
    }}}
    /* -- No core ---------------------------------------------------------- */
    { }
  /* -- End ---------------------------------------------------------------- */
} *cEvtWin = nullptr;                  // Pointer to static class
/* ------------------------------------------------------------------------- */
typedef EvtWin::Args   EvtWinArgs;     // Shortcut to EvtWin::Args class
typedef EvtWin::Event  EvtWinEvent;    // Shortcut to EvtWin::Event class
typedef EvtWin::RegVec EvtWinRegVec;   // Shortcut to EvtWin::RegVec class
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
