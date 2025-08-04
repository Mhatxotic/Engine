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
using namespace IConGraph::P;          using namespace IConsole::P;
using namespace ICVar::P;              using namespace ICVarDef::P;
using namespace ICVarLib::P;           using namespace IDim::P;
using namespace IEvtMain::P;           using namespace IEvtWin::P;
using namespace IFboCore::P;           using namespace IFlags;
using namespace IGlFW::P;              using namespace IGlFWUtil::P;
using namespace IHelper::P;            using namespace IJoystick::P;
using namespace ILog::P;               using namespace ILuaFunc::P;
using namespace IStd::P;               using namespace IString::P;
using namespace ISysUtil::P;           using namespace IUtf::P;
using namespace IUtil::P;              using namespace Lib::OS::GlFW::Types;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* == Input flags ========================================================== */
BUILD_FLAGS(Input,
  /* ----------------------------------------------------------------------- */
  // No flags                          Mouse cursor is enabled?
  IF_NONE                   {Flag(0)}, IF_CURSOR                 {Flag(1)},
  // Full-screen toggler enabled?      Mouse cursor has focus?
  IF_FSTOGGLER              {Flag(2)}, IF_MOUSEFOCUS             {Flag(3)},
  // Send events at startup?           Do joystick polling?
  IF_INITEVENTS             {Flag(4)}, IF_POLLJOYSTICKS          {Flag(5)},
  // Ignore input on focus loss?       Clamp mouse cursor? (MacOS only)
  IF_RESTORE                {Flag(6)}, IF_CLAMPMOUSE             {Flag(7)}
);/* == Input class ======================================================== */
class Input;                           // Class prototype
static Input *cInput = nullptr;        // Pointer to global class
class Input :                          // Handles keyboard, mouse & controllers
  /* -- Base classes ------------------------------------------------------- */
  private InitHelper,                  // Initialsation helper
  public InputFlags,                   // Input configuration settings
  private EvtMainRegAuto,              // Events list to register
  public DimInt,                       // Window dimensions
  public Joystick                      // Joystick class
{ /* -- Console ------------------------------------------------------------ */
  int              iConKey1, iConKey2; // Primary and secondary console keys
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
    const unsigned int uiKey = emeEvent.eaArgs[1].UInt();
    // If console is enabled, send it to console instead
    if(cConsole->IsVisible()) return cConsole->OnCharPress(uiKey);
    // Else send the key to lua callbacks
    lfOnChar.LuaFuncDispatch(uiKey);
  }
  /* -- Mouse moved -------------------------------------------------------- */
  void OnMouseMove(const EvtMainEvent &emeEvent)
  { // Get reference to actual arguments vector
    const EvtMainArgs &emaArgs = emeEvent.eaArgs;
    // Do not process this event if input disabled on lack of mouse focus
#if defined(MACOS)
    // Get mouse position. We might need to clamp it
    double dX = emaArgs[1].Double(), dY = emaArgs[2].Double();
    // If mouse clamp enabled?
    if(FlagIsSet(IF_CLAMPMOUSE))
    { // Return if mouse out of focus
      if(FlagIsClear(IF_MOUSEFOCUS)) return;
      // Clamp mouse co-ordinates if out of the window?
      if(dX < 0.0) dX = 0.0;
      else if(dX >= DimGetWidth()) dX = DimGetWidth() - 1;
      if(dY < 0.0) dY = 0.0;
      else if(dY >= DimGetHeight()) dY = DimGetHeight() - 1;
    }
#else
    // Get mouse position
    const double dX = emaArgs[1].Double(), dY = emaArgs[2].Double();
#endif
    // Recalculate cursor position based on framebuffer size and send the new
    // co-ordinates to the lua callback handler
    lfOnMouseMove.LuaFuncDispatch(
      UtilScaleValue(dX, DimGetWidth(),
        cFboCore->fboMain.ffcStage.GetCoLeft(),
        cFboCore->fboMain.GetCoRight()),
      UtilScaleValue(dY, DimGetHeight(),
        cFboCore->fboMain.ffcStage.GetCoTop(),
        cFboCore->fboMain.GetCoBottom()));
  }
  /* -- Mouse went inside the window --------------------------------------- */
  void OnMouseFocus(const EvtMainEvent &emeEvent)
  { // Get and check state
    const int iState = emeEvent.eaArgs[1].Int();
    switch(iState)
    { // Mouse is in the window? Set mouse in window flag
      case GLFW_TRUE:
        // Return if mouse is already focused
        if(FlagIsSet(IF_MOUSEFOCUS)) return;
        // Set mouse in window
        FlagSet(IF_MOUSEFOCUS);
        // Done, send lua event
        break;
      // Mouse is not in the window? Clear mouse in window flag
      case GLFW_FALSE:
        // Return if mouse is not focused already?
        if(FlagIsClear(IF_MOUSEFOCUS)) return;
        // Set mouse is out of window
        FlagClear(IF_MOUSEFOCUS);
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
    const EvtMainArgs &emaArgs = emeEvent.eaArgs;
    // Get movements
    const double dX = emaArgs[1].Double(), dY = emaArgs[2].Double();
    // If console is enabled and ctrl not pressed? Send it to console instead
    if(cConsole->IsVisible()) return cConGraphics->OnMouseWheel(dX, dY);
    // Set event to lua callbacks
    lfOnMouseScroll.LuaFuncDispatch(dX, dY);
  }
  /* -- Mouse button clicked ----------------------------------------------- */
  void OnMouseClick(const EvtMainEvent &emeEvent)
  { // Get reference to actual arguments vector
    const EvtMainArgs &emaArgs = emeEvent.eaArgs;
    // Set event to lua callbacks
    lfOnMouseClick.LuaFuncDispatch(emaArgs[1].Int(), emaArgs[2].Int(),
      emaArgs[3].Int());
  }
  /* -- Unfiltered key pressed --------------------------------------------- */
  void OnKeyPress(const EvtMainEvent &emeEvent)
  { // Get reference to actual arguments vector
    const EvtMainArgs &emaArgs = emeEvent.eaArgs;
    // Get key code, state and modifier state
    const int iKey = emaArgs[1].Int(), iState = emaArgs[3].Int(),
      iMod = emaArgs[4].Int();
    // If...
    if(iMod == GLFW_MOD_ALT &&         // ALT key pressed/released/repeated?
       iKey == GLFW_KEY_ENTER &&       // ENTER key pressed/released/repeated?
       FlagIsSet(IF_FSTOGGLER))        // Full-screen toggler key enabled?
    { // Return if key has not been released
      if(iState != GLFW_RELEASE) return;
      // Get inverted full-screen setting
      const bool bFullScreen = !cCVars->GetInternal<bool>(VID_FS);
      // Set full screen setting depending on current state
      cCVars->SetInternal<bool>(VID_FS, bFullScreen);
      // Send command to toggle full-screen
      cEvtWin->AddUnblock(EWC_WIN_TOGGLE_FS, bFullScreen);
      // Don't send key to guest
      return;
    } // Return if console handled this key
    else if(cConGraphics->IsKeyHandled(iKey, iState, iMod)) return;
    // Compare state
    switch(iState)
    { // Key initially pressed down?
      case GLFW_PRESS:
        // If modifier key or console key not pressed and setting console to
        // enabled worked? Ignore first key as registering OnCharPress will
        // trigger this keystroke and print it out in the console.
        if(!iMod && (iKey == iConKey1 || iKey == iConKey2) &&
           cConGraphics->SetVisible(true))
          return cConsole->FlagSet(CF_IGNOREKEY);
        // Key not handled so send event to guest
        break;
      // Key released?
      case GLFW_RELEASE:
        // Ignore the ESCAPE generated from hiding the console
        if(cConsole->FlagIsSet(CF_IGNOREESC) &&
           !iMod && iKey == GLFW_KEY_ESCAPE)
          return cConsole->FlagClear(CF_IGNOREESC);
        // Key not handled so send event to guest
        break;
      // Key still being pressed? Key not handled so send event to guest
      case GLFW_REPEAT: break;
      // Anything else just ignore
      default:
        // Log the bad mouse focus state and return
        cLog->LogWarningExSafe("Input ignored bad key press state $<$$>!",
          iState, hex, iState);
        // Don't dispatch an event
        return;
    } // Send lua event with key, state, mod and scan code
    lfOnKey.LuaFuncDispatch(iKey, iState, iMod, emaArgs[2].Int());
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
    UtfDecoder utfString{ cGlFW->WinGetClipboard() };
    // For each character, ddd the character to queue if valid
    while(const unsigned int uiChar = utfString.Next())
      if(uiChar >= 32) cConsole->OnCharPress(uiChar);
  }
  /* -- Commit cursor visibility ------------------------------------------- */
  void CommitCursor(void)
    { cEvtWin->AddUnblock(EWC_WIN_CURSETVIS, FlagIsSet(IF_CURSOR)); }
  /* -- Set visibility of mouse cursor ------------------------------------- */
  void SetCursor(const bool bState)
  { // Set member var incase window needs to reinit so we can restore the
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
  /* -- Update window size from actual glfw window ------------------------- */
  void UpdateWindowSize(void) { DimSet(cGlFW->WinGetSize()); }
  /* -- Request input state ------------------------------------------------ */
  void RequestMousePosition(void) const
    { cEvtWin->AddUnblock(EWC_WIN_CURPOSGET); }
  /* -- Forcefully move the cursor ----------------------------------------- */
  void SetCursorPos(const GLfloat fX, const GLfloat fY)
  { // Expand the stage co-ordinates to actual desktop window co-ordinates
    const GLfloat
      fAdjX = (fX + -cFboCore->fboMain.ffcStage.GetCoLeft()) /
        cFboCore->fboMain.GetCoRight() * DimGetWidth(),
      fAdjY = (fY + -cFboCore->fboMain.ffcStage.GetCoTop()) /
        cFboCore->fboMain.GetCoBottom() * DimGetHeight(),
      // Clamp the new position to the window bounds.
      fNewX = UtilClamp(fAdjX, 0.0f, DimGetWidth() - 1.0f),
      fNewY = UtilClamp(fAdjY, 0.0f, DimGetHeight() - 1.0f);
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
    SetLockKeyModEnabled(cCVars->GetInternal<bool>(INP_LOCKKEYMODS));
    SetRawMouseEnabled(cCVars->GetInternal<bool>(INP_RAWMOUSE));
    SetStickyKeyEnabled(cCVars->GetInternal<bool>(INP_STICKYKEY));
    SetStickyMouseEnabled(cCVars->GetInternal<bool>(INP_STICKYMOUSE));
    // Set/Restore cursor state
    SetCursor(FlagIsSet(IF_CURSOR));
    // Init joystick system
    JoyInit();
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
    // De-init joystick system
    JoyDeInit();
    // Log progress
    cLog->LogDebugSafe("Input interface deinitialised.");
  }
  /* -- Destructor ---------------------------------------------- */ protected:
  DTORHELPER(~Input, DeInit())
  /* -- Constructor -------------------------------------------------------- */
  Input(void) :
    /* -- Initialisers ----------------------------------------------------- */
    InitHelper{ __FUNCTION__ },        // Init initialisation helper class
    InputFlags{ IF_NONE },             // No flags set initially
    /* -- Init events for event manager ------------------------------------ */
    EvtMainRegAuto{ cEvtMain, {        // Events list to register
      { EMC_INP_CHAR,         bind(&Input::OnFilteredKey, this, _1) },
      { EMC_INP_PASTE,        bind(&Input::OnWindowPaste, this, _1) },
      { EMC_INP_MOUSE_MOVE,   bind(&Input::OnMouseMove,   this, _1) },
      { EMC_INP_MOUSE_CLICK,  bind(&Input::OnMouseClick,  this, _1) },
      { EMC_INP_MOUSE_FOCUS,  bind(&Input::OnMouseFocus,  this, _1) },
      { EMC_INP_MOUSE_SCROLL, bind(&Input::OnMouseWheel,  this, _1) },
      { EMC_INP_KEYPRESS,     bind(&Input::OnKeyPress,    this, _1) },
      { EMC_INP_DRAG_DROP,    bind(&Input::OnDragDrop,    this, _1) },
      { EMC_INP_JOY_STATE,    bind(&Joystick::OnJoyState, this, _1) },
    } },
    /* -- More initialisers ------------------------------------------------ */
    iConKey1(GLFW_KEY_UNKNOWN),        // Init primary console key
    iConKey2(iConKey1),                // Init secondary console key
    lfOnMouseClick{ "OnMouseClick" },  // Init mouse click lua event
    lfOnMouseMove{ "OnMouseMove" },    // Init mouse movement lua event
    lfOnMouseScroll{ "OnMouseScroll" },// Init mouse wheel lua event
    lfOnMouseFocus{ "OnMouseFocus" },  // Init mouse focus lua event
    lfOnKey{ "OnUnfilteredKey" },      // Init unfiltered keypress lua event
    lfOnChar{ "OnFilteredKey" },       // Init filtered keypress lua event
    lfOnDragDrop{ "OnDragDrop" }       // Init drag & drop lua event
    /* -- Set global pointer to static class ------------------------------- */
    { cInput = this; }
  /* -- CVar callback to toggle raw mouse -------------------------- */ public:
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
  // -- CVar callback to toggle lock key mod bits -------------------------- */
  CVarReturn SetLockKeyModEnabled(const bool bState)
  { // Send request to set lock key mods state if enabled
    if(IHIsInitialised()) cEvtWin->AddUnblock(EWC_WIN_SETLKMODS, bState);
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
  /* -- Set clamp mouse cursor --------------------------------------------- */
  CVarReturn SetClampMouse(const bool bState)
    { FlagSetOrClear(IF_CLAMPMOUSE, bState); return ACCEPT; }
};/* ----------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
