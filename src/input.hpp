/* == INPUT.HPP ============================================================ **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## This module handles the keyboard and controller input.              ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IInput {                     // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace ICollector::P;         using namespace IConsole::P;
using namespace ICVar::P;              using namespace ICVarDef::P;
using namespace ICVarLib::P;           using namespace IDim;
using namespace IEvtMain::P;           using namespace IEvtWin::P;
using namespace IFboCore::P;           using namespace IFlags;
using namespace IGlFW::P;              using namespace IGlFWUtil::P;
using namespace IJoystick::P;          using namespace ILog::P;
using namespace ILuaFunc::P;           using namespace IStd::P;
using namespace IString::P;            using namespace ISysUtil::P;
using namespace IUtil::P;              using namespace Lib::OS::GlFW;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* == Input flags ========================================================== */
BUILD_FLAGS(Input,
  /* ----------------------------------------------------------------------- */
  // No flags                          Mouse cursor is enabled?
  IF_NONE                   {Flag[0]}, IF_CURSOR                 {Flag[1]},
  // Full-screen toggler enabled?      Mouse cursor has focus?
  IF_FSTOGGLER              {Flag[2]}, IF_MOUSEFOCUS             {Flag[3]},
  // Send events at startup?           Do joystick polling?
  IF_INITEVENTS             {Flag[4]}, IF_POLLJOYSTICKS          {Flag[5]},
  // Ignore input on focus loss?       Input events were modified?
  IF_NOINPUTONFOCUSLOSS     {Flag[6]}, IF_RESTORE                {Flag[7]},
  // Clamp mouse cursor? (MacOS only)
  IF_CLAMPMOUSE             {Flag[8]}
);/* == Input class ======================================================== */
static class Input final :             // Handles keyboard, mouse & controllers
  /* -- Base classes ------------------------------------------------------- */
  private IHelper,                     // Initialsation helper
  public InputFlags,                   // Input configuration settings
  private EvtMainRegVec,               // Events list to register
  private DimCoords<int, double>,      // Window width and cursor position
  public Joystick                      // Joystick class
{ /* -- Console ------------------------------------------------------------ */
  int              iConKey1, iConKey2; // Primary and secondary console keys
  /* -- Alternative mouse movement functions ------------------------------- */
#if defined(MACOS)                     // Compiling MacOS version?
  EvtMain::CbEcFunc fnMouseFocus,      // OnMouseFocus event
                    fnMoveUnfocused,   // OnMouseMove event on unfocused window
                    fnMoveClamped;     // OnMouseMove event for clamping mouse
#else                                  // Compiling Windows or linux version?
  EvtMain::CbEcFunc fnMouseFocus;      // OnMouseFocus event
#endif                                 // #if defined(MACOS)
  /* -- Events ----------------------------------------------------- */ public:
  LuaFunc          lfOnMouseClick,     // Mouse button clicked
                   lfOnMouseMove,      // Mouse cursor moved
                   lfOnMouseScroll,    // Mouse scroll wheel moved
                   lfOnMouseFocus,     // Mouse focus changed from window
                   lfOnKey,            // Unfiltered key pressed
                   lfOnChar,           // Filtered key pressed
                   lfOnDragDrop;       // Drag and dropped files
  /* -- Filtered key pressed ----------------------------------------------- */
  void OnFilteredKey(const EvtMainEvent &emeEvent)
  { // Get key pressed
    const unsigned int uiKey = emeEvent.aArgs[1].ui;
    // If console is enabled, send it to console instead
    if(cConsole->IsVisible()) return cConsole->OnCharPress(uiKey);
    // Else send the key to lua callbacks
    lfOnChar.LuaFuncDispatch(uiKey);
  }
  /* -- Clamp mouse to window for guest (MacOS only) ----------------------- */
#if defined(MACOS)                     // To mimic Windows cursor functionality
  bool ClampToWindow(void)
  { // Return if disabled or mouse is in bounds
    if(FlagIsClear(IF_CLAMPMOUSE) ||
       (CoordGetX() >= 0.0 && CoordGetY() >= 0.0 &&
        CoordGetX() < GetWindowWidth() && CoordGetY() < GetWindowHeight()))
      return false;
    // Dispatch event to guest with mouse cursor clamped to window edge to
    // mimic Windows and Linux functionality.
    lfOnMouseMove.LuaFuncDispatch(
      UtilClamp(UtilScaleValue(CoordGetX(), GetWindowWidth(),
        cFboCore->fboMain.ffcStage.GetCoLeft(),
        cFboCore->fboMain.GetCoRight()),
        cFboCore->fboMain.ffcStage.GetCoLeft(),
        cFboCore->fboMain.ffcStage.GetCoRight() - 1.0),
      UtilClamp(UtilScaleValue(CoordGetY(), GetWindowHeight(),
        cFboCore->fboMain.ffcStage.GetCoTop(),
        cFboCore->fboMain.GetCoBottom()),
        cFboCore->fboMain.ffcStage.GetCoTop(),
        cFboCore->fboMain.ffcStage.GetCoBottom() - 1.0));
    // We clamped the cursor
    return true;
  }
  /* -- Mouse moved while window unfocused --------------------------------- */
  void OnMouseMoveUnfocused(const EvtMainEvent &emeEvent)
  { // Get reference to actual arguments vector
    const EvtMainArgs &emaArgs = emeEvent.aArgs;
    // Store cursor position for use in other events
    CoordSet(emaArgs[1].d, emaArgs[2].d);
    // Tell guest we clamped position if mouse is out of bounds
    ClampToWindow();
    // Disable further mouse input
    cEvtMain->NullOp(EMC_INP_MOUSE_MOVE);
  }
  /* -- Mouse moved but keeping it clamped --------------------------------- */
  void OnMouseMoveClamped(const EvtMainEvent &emeEvent)
  { // Get reference to actual arguments vector
    const EvtMainArgs &emaArgs = emeEvent.aArgs;
    // Store cursor position for use in other events
    CoordSet(emaArgs[1].d, emaArgs[2].d);
    // Keep the mouse clamped to the window and return if we did clamp it
    if(ClampToWindow()) return;
    // Or process normal mouse movement
    lfOnMouseMove.LuaFuncDispatch(
      UtilScaleValue(CoordGetX(), GetWindowWidth(),
        cFboCore->fboMain.ffcStage.GetCoLeft(),
        cFboCore->fboMain.GetCoRight()),
      UtilScaleValue(CoordGetY(), GetWindowHeight(),
        cFboCore->fboMain.ffcStage.GetCoTop(),
        cFboCore->fboMain.GetCoBottom()));
  }
#endif                                 // End of MacOS specific code
  /* -- Mouse went inside the window --------------------------------------- */
  void OnMouseFocus(const EvtMainEvent &emeEvent)
  { // Get and check state
    const int iState = emeEvent.aArgs[1].i;
    switch(iState)
    { // Mouse is in the window? Set mouse in window flag
      case GLFW_TRUE:
        // If mouse is not focused?
        if(FlagIsClear(IF_MOUSEFOCUS))
        { // If we're to restore events?
          if(FlagIsSet(IF_RESTORE))
          { // Clear the flag and re-enable input events on MacOS
            FlagClear(IF_RESTORE);
            EnableInputEvents();
          } // Set mouse is focused
          FlagSet(IF_MOUSEFOCUS);
        } // No event needs to be sent
        else return;
        // Done
        break;
      // Mouse is not in the window? Clear mouse in window flag
      case GLFW_FALSE:
        // If mouse is focused?
        if(FlagIsSet(IF_MOUSEFOCUS))
        { // Setting is enable to ignore input?
          if(FlagIsSet(IF_NOINPUTONFOCUSLOSS))
          { // Disable all input events (except this one)
            cEvtMain->NullOpEx(*this);
            cEvtMain->Register(EMC_INP_MOUSE_FOCUS, fnMouseFocus);
            // If using MacOS
#if defined(MACOS)
            // Set function to clamp any further mouse events and block more
            cEvtMain->Register(EMC_INP_MOUSE_MOVE, fnMoveUnfocused);
            // Tell guest we clamped position if mouse is out of bounds
            ClampToWindow();
#endif
            // This indicates when focus is true to restore the events
            FlagSet(IF_RESTORE);
          } // If using MacOS?
#if defined(MACOS)
          else
          { // Set function to clamp mouse cursor but still send event to Lua
            cEvtMain->Register(EMC_INP_MOUSE_MOVE, fnMoveClamped);
            // Tell guest we clamped position if mouse is out of bounds
            ClampToWindow();
          }
#endif
          // Set mouse is out of window
          FlagClear(IF_MOUSEFOCUS);
        } // No event needs to be sent
        else return;
        // Done
        break;
      // Unknown state?
      default:
        // Log the bad mouse focus state and return
        cLog->LogWarningExSafe("Input ignored bad mouse focus state $<$$>!",
          iState, hex, iState);
        // Don't dispatch an event
        return;
    } // Dispatch event to lua scripts
    lfOnMouseFocus.LuaFuncDispatch(iState);
  }
  /* -- Mouse wheel scroll ------------------------------------------------- */
  void OnMouseWheel(const EvtMainEvent &emeEvent)
  { // Get reference to actual arguments vector
    const EvtMainArgs &emaArgs = emeEvent.aArgs;
    // Get movements
    const double dX = emaArgs[1].d, dY = emaArgs[2].d;
    // If console is enabled and ctrl not pressed? Send it to console instead
    if(cConsole->IsVisible()) return cConGraphics->OnMouseWheel(dX, dY);
    // Set event to lua callbacks
    lfOnMouseScroll.LuaFuncDispatch(dX, dY);
  }
  /* -- Mouse button clicked ----------------------------------------------- */
  void OnMouseClick(const EvtMainEvent &emeEvent)
  { // Get reference to actual arguments vector
    const EvtMainArgs &emaArgs = emeEvent.aArgs;
    // Set event to lua callbacks
    lfOnMouseClick.LuaFuncDispatch(emaArgs[1].i, emaArgs[2].i, emaArgs[3].i);
  }
  /* -- Mouse moved -------------------------------------------------------- */
  void OnMouseMove(const EvtMainEvent &emeEvent)
  { // Get reference to actual arguments vector
    const EvtMainArgs &emaArgs = emeEvent.aArgs;
    // Store cursor position for use in other events
    CoordSet(emaArgs[1].d, emaArgs[2].d);
    // Recalculate cursor position based on framebuffer size and send the
    // new co-ordinates to the lua callback handler
    lfOnMouseMove.LuaFuncDispatch(UtilScaleValue(CoordGetX(), GetWindowWidth(),
      cFboCore->fboMain.ffcStage.GetCoLeft(), cFboCore->fboMain.GetCoRight()),
                      UtilScaleValue(CoordGetY(), GetWindowHeight(),
      cFboCore->fboMain.ffcStage.GetCoTop(), cFboCore->fboMain.GetCoBottom()));
  }
  /* -- Unfiltered key pressed --------------------------------------------- */
  void OnKeyPress(const EvtMainEvent &emeEvent)
  { // Get reference to actual arguments vector
    const EvtMainArgs &emaArgs = emeEvent.aArgs;
    // Get key code, state and modifier state
    const int iKey = emaArgs[1].i, iState = emaArgs[3].i, iMod = emaArgs[4].i;
    // Compare press state
    switch(iState)
    { // Key initially pressed down?
      case GLFW_PRESS:
        // If the console didn't handle this key?
        if(cConGraphics->IsKeyNotHandled(iKey, iState, iMod))
        { // If modifier key or console key not pressed then don't handle it
          if(iMod || (iKey != iConKey1 && iKey != iConKey2)) break;
          // Set console enabled and if enabled? Ignore first key as registering
          // OnCharPress will trigger this keystroke and print it out in the
          // console.
          if(cConGraphics->SetVisible(true)) cConsole->FlagSet(CF_IGNOREKEY);
        } // We handled this key so do not dispatch it to scripts
        return;
      // Key released?
      case GLFW_RELEASE:
        // Return if alt+enter not pressed or input toggler key is disabled
        if(iKey == GLFW_KEY_ENTER && iMod == GLFW_MOD_ALT &&
           FlagIsSet(IF_FSTOGGLER))
        { // Get inverted full-screen setting
          const bool bFullScreen = !cCVars->GetInternal<bool>(VID_FS);
          // Set full screen setting depending on current state
          cCVars->SetInternal<bool>(VID_FS, bFullScreen);
          // Send command to toggle full-screen
          cEvtWin->AddUnblock(EWC_WIN_TOGGLE_FS, bFullScreen);
        } // If console cannot handle this key?
        else if(cConGraphics->IsKeyNotHandled(iKey, iState, iMod))
        { // Ignore the ESCAPE generated from hiding the console
          if(cConsole->FlagIsSet(CF_IGNOREESC) &&
             !iMod && iKey == GLFW_KEY_ESCAPE)
            cConsole->FlagClear(CF_IGNOREESC);
          // No key was handled
          else break;
        } // We handled this key so do not dispatch it to scripts
        return;
      // Key still being pressed?
      case GLFW_REPEAT:
        // Break if console cannot handle this key
        if(cConGraphics->IsKeyNotHandled(iKey, iState, iMod)) break;
        // Console handled the key so return;
        return;
      // Anything else just ignore
      default:
        // Log the bad mouse focus state and return
        cLog->LogWarningExSafe("Input ignored bad key press state $<$$>!",
          iState, hex, iState);
        // Don't dispatch an event
        return;
    } // Send lua event with key, state, mod and scan code
    lfOnKey.LuaFuncDispatch(iKey, iState, iMod, emaArgs[2].i);
  }
  /* -- Files dragged and dropped on window--------------------------------- */
  void OnDragDrop(const EvtMainEvent&)
  { // Get files and return if empty
    StrVector &vFiles = cGlFW->WinGetFiles();
    if(vFiles.empty()) return;
    // Send off the event to lua callbacks
    lfOnDragDrop.LuaFuncDispatch(vFiles);
    // Clear the list
    vFiles.clear();
    vFiles.shrink_to_fit();
  }
  /* -- Window past event--------------------------------------------------- */
  void OnWindowPaste(const EvtMainEvent&)
  { // Get text in clipboard
    IUtf::UtfDecoder utfString{ cGlFW->WinGetClipboard() };
    // For each character, ddd the character to queue if valid
    while(const unsigned int uiChar = utfString.Next())
      if(uiChar >= 32) cConsole->OnCharPress(uiChar);
  }
  /* -- Commit cursor visibility now ------------------------------- */ public:
  void CommitCursorNow(void) { cGlFW->WinSetCursor(FlagIsSet(IF_CURSOR)); }
  /* -- Commit cursor visibility ------------------------------------------- */
  void CommitCursor(void)
    { cEvtWin->AddUnblock(EWC_WIN_CURSETVIS, FlagIsSet(IF_CURSOR)); }
  /* -- Set visibility of mouse cursor ------------------------------------- */
  void SetCursor(const bool bState)
  { // Set member var incase window needs to re-init so we can restore the
    // cursor state
    FlagSetOrClear(IF_CURSOR, bState);
    // Request to set cursor visibility
    CommitCursor();
  }
  /* -- Reset environment -------------------------------------------------- */
  void ResetEnvironment(void)
  { // Reset cursor visibility
    SetCursor(true);
    // Reset joystick environment
    JoyReset();
  }
  /* -- Get window size ---------------------------------------------------- */
  int GetWindowWidth(void) const { return DimGetWidth(); }
  int GetWindowHeight(void) const { return DimGetHeight(); }
  /* -- Update window size from actual glfw window ------------------------- */
  void UpdateWindowSize(void) { cGlFW->WinGetSize(DimGetWidthRef(),
                                                  DimGetHeightRef()); }
  /* -- Update window size (from display) ---------------------------------- */
  void SetWindowSize(const int iX, const int iY) { DimSet(iX, iY); }
  /* -- Request input state ------------------------------------------------ */
  void RequestMousePosition(void) const
    { cEvtWin->AddUnblock(EWC_WIN_CURPOSGET); }
  /* -- Forcefully move the cursor ----------------------------------------- */
  void SetCursorPos(const GLfloat fX, const GLfloat fY)
  { // Expand the stage co-ordinates to actual desktop window co-ordinates
    const GLfloat
      fAdjX = (fX + -cFboCore->fboMain.ffcStage.GetCoLeft()) /
        cFboCore->fboMain.GetCoRight() * GetWindowWidth(),
      fAdjY = (fY + -cFboCore->fboMain.ffcStage.GetCoTop()) /
        cFboCore->fboMain.GetCoBottom() * GetWindowHeight(),
      // Clamp the new position to the window bounds.
      fNewX = UtilClamp(fAdjX, 0.0f, GetWindowWidth() - 1.0f),
      fNewY = UtilClamp(fAdjY, 0.0f, GetWindowHeight() - 1.0f);
    // Dispatch the request to set the cursor
    cEvtWin->AddUnblock(EWC_WIN_CURPOSSET,
      static_cast<double>(fNewX), static_cast<double>(fNewY));
    // Dispatch an event to the mouse moved since GlFW won't send an update
    lfOnMouseMove.LuaFuncDispatch(static_cast<double>(fX),
      static_cast<double>(fY));
  }
  /* -- Forcefully move the cursor to the centre --------------------------- */
  void SetCursorCentre(void)
    { SetCursorPos(cFboCore->GetMatrixWidth() / 2.0f,
                   cFboCore->GetMatrixHeight() / 2.0f); }
  /* -- Disable input events ----------------------------------------------- */
  void DisableInputEvents(void) { cEvtMain->UnregisterEx(*this); }
  /* -- Enable input events ------------------------------------------------ */
  void EnableInputEvents(void) { cEvtMain->RegisterEx(*this); }
  /* -- Init --------------------------------------------------------------- */
  void Init(void)
  { // if window not available? This should never happen but we will put
    // this here just incase. The subsequent operations are pointless without
    // a valid GLFW window. Log that initialisation is being skipped and return
    // if window is not available.
    if(!cGlFW->WinIsAvailable())
      return cLog->LogWarningSafe(
       "Input initialisation skipped with no window available!");
    // Class initialised
    IHInitialise();
    // Log progress
    cLog->LogDebugSafe("Input interface is initialising...");
    // Init input settings
    SetRawMouseEnabled(cCVars->GetInternal<bool>(INP_RAWMOUSE));
    SetStickyKeyEnabled(cCVars->GetInternal<bool>(INP_STICKYKEY));
    SetStickyMouseEnabled(cCVars->GetInternal<bool>(INP_STICKYMOUSE));
    // Set/Restore cursor state
    SetCursor(FlagIsSet(IF_CURSOR));
    // Init joystick system
    JoyInit();
    // Init input engine events
    EnableInputEvents();
    // Log progress
    cLog->LogDebugExSafe("Input interface initialised (R:$;J:$).",
      StrFromBoolTF(GlFWIsRawMouseMotionSupported()), JoyGetCount());
  }
  /* -- DeInit ------------------------------------------------------------- */
  void DeInit(void)
  { // Ignore if class not initialised
    if(IHNotDeInitialise()) return;
    // Log progress
    cLog->LogDebugSafe("Input interface deinitialising...");
    // Deinit engine events in the order they were registered
    DisableInputEvents();
    // De-init joystick system
    JoyDeInit();
    // Log progress
    cLog->LogDebugSafe("Input interface deinitialised.");
  }
  /* -- Constructor -------------------------------------------------------- */
  Input(void) :
    /* -- Initialisers ----------------------------------------------------- */
    IHelper{ __FUNCTION__ },           // Init initialisation helper class
    InputFlags{ IF_NONE },             // No flags set initially
    /* -- Init events for event manager ------------------------------------ */
    EvtMainRegVec{                     // Events list to register
      { EMC_INP_CHAR,         bind(&Input::OnFilteredKey, this, _1) },
      { EMC_INP_PASTE,        bind(&Input::OnWindowPaste, this, _1) },
      { EMC_INP_MOUSE_MOVE,   bind(&Input::OnMouseMove,   this, _1) },
      { EMC_INP_MOUSE_CLICK,  bind(&Input::OnMouseClick,  this, _1) },
      { EMC_INP_MOUSE_FOCUS,  bind(&Input::OnMouseFocus,  this, _1) },
      { EMC_INP_MOUSE_SCROLL, bind(&Input::OnMouseWheel,  this, _1) },
      { EMC_INP_KEYPRESS,     bind(&Input::OnKeyPress,    this, _1) },
      { EMC_INP_DRAG_DROP,    bind(&Input::OnDragDrop,    this, _1) },
      { EMC_INP_JOY_STATE,    bind(&Joystick::OnJoyState, this, _1) },
    },
    /* -- More initialisers ------------------------------------------------ */
    iConKey1(GLFW_KEY_UNKNOWN),        // Init primary console key
    iConKey2(iConKey1),                // Init secondary console key
    fnMouseFocus{ bind(&Input::OnMouseFocus, this, _1) },
#if defined(MACOS)                     // Is compiling MacOS version?
    fnMoveUnfocused{ bind(&Input::OnMouseMoveUnfocused, this, _1) },
    fnMoveClamped{ bind(&Input::OnMouseMoveClamped, this, _1) },
#endif                                 // #if defined(MACOS)
    lfOnMouseClick{ "OnMouseClick" },  // Init mouse click lua event
    lfOnMouseMove{ "OnMouseMove" },    // Init mouse movement lua event
    lfOnMouseScroll{ "OnMouseScroll" },// Init mouse wheel lua event
    lfOnMouseFocus{ "OnMouseFocus" },  // Init mouse focus lua event
    lfOnKey{ "OnUnfilteredKey" },      // Init unfiltered keypress lua event
    lfOnChar{ "OnFilteredKey" },       // Init filtered keypress lua event
    lfOnDragDrop{ "OnDragDrop" }       // Init drag & drop lua event
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Destructor --------------------------------------------------------- */
  DTORHELPER(~Input, DeInit())
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(Input)               // Suppress default functions for safety
  // -- CVar callback to toggle raw mouse ---------------------------------- */
  CVarReturn SetRawMouseEnabled(const bool bState)
  { // Send request to set raw mouse motion state if enabled
    if(IHIsInitialised()) cEvtWin->AddUnblock(EWC_WIN_SETRAWMOUSE, bState);
    // CVar allowed to be set
    return ACCEPT;
  }
  // -- CVar callback to toggle sticky keys -------------------------------- */
  CVarReturn SetStickyKeyEnabled(const bool bState)
  { // Send request to set sticky keys state if enabled
    if(IHIsInitialised()) cEvtWin->AddUnblock(EWC_WIN_SETSTKKEYS, bState);
    // CVar allowed to be set
    return ACCEPT;
  }
  // -- CVar callback to toggle sticky mouse ------------------------------- */
  CVarReturn SetStickyMouseEnabled(const bool bState)
  { // Send request to set sticky mouse if enabled
    if(IHIsInitialised()) cEvtWin->AddUnblock(EWC_WIN_SETSTKMOUSE, bState);
    // CVar allowed to be set
    return ACCEPT;
  }
  /* -- Set first console key ---------------------------------------------- */
  CVarReturn SetConsoleKey1(const int iK)
    { return CVarSimpleSetIntNG(iConKey1, iK, GLFW_KEY_LAST); }
  /* -- Set send joystick events at startup -------------------------------- */
  CVarReturn SetSendEventsEnabled(const bool bState)
    { FlagSetOrClear(IF_INITEVENTS, bState); return ACCEPT; }
  /* -- Set second console key --------------------------------------------- */
  CVarReturn SetConsoleKey2(const int iK)
    { return CVarSimpleSetIntNG(iConKey2, iK, GLFW_KEY_LAST); }
  /* -- Set full screen toggler -------------------------------------------- */
  CVarReturn SetFSTogglerEnabled(const bool bState)
    { FlagSetOrClear(IF_FSTOGGLER, bState); return ACCEPT; }
  /* -- Set no input on focus loss ----------------------------------------- */
  CVarReturn SetNoInputOnFocusLoss(const bool bState)
    { FlagSetOrClear(IF_NOINPUTONFOCUSLOSS, bState); return ACCEPT; }
  /* -- Set clamp mouse cursor --------------------------------------------- */
  CVarReturn SetClampMouse(const bool bState)
    { FlagSetOrClear(IF_CLAMPMOUSE, bState); return ACCEPT; }
  /* ----------------------------------------------------------------------- */
} *cInput = nullptr;                   // Global input class
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
