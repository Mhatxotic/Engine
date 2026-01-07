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
/* -- Public typedefs ------------------------------------------------------ */
BUILD_FLAGS(Input,                     // Input flags
  /* ----------------------------------------------------------------------- */
  IF_NONE                   {Flag(0)}, // No flags?
  IF_CURSOR                 {Flag(1)}, // Mouse cursor is enabled?
  IF_FSTOGGLER              {Flag(2)}, // Full-screen toggler enabled?
  IF_MOUSEFOCUS             {Flag(3)}, // Mouse cursor has focus?
  IF_INITEVENTS             {Flag(4)}, // Send events at startup?
  IF_POLLJOYSTICKS          {Flag(5)}, // Do joystick polling?
  IF_RESTORE                {Flag(6)}, // Ignore input on focus loss?
  IF_CLAMPMOUSE             {Flag(7)}  // Clamp mouse cursor? (MacOS only)
);/* ----------------------------------------------------------------------- */
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
  void InputOnFilteredKey(const EvtMainEvent &emeEvent)
  { // Get key pressed (Codepoint defined in 'utf.hpp' is same as size_t)
    const Codepoint cKey = emeEvent.eaArgs[1].SizeT();
    // If console is enabled, send it to console instead
    if(cConsole->IsVisible()) return cConsole->OnCharPress(cKey);
    // Else send the key to lua callbacks
    lfOnChar.LuaFuncDispatch(cKey);
  }
  /* -- Mouse moved -------------------------------------------------------- */
  void InputOnMouseMove(const EvtMainEvent &emeEvent)
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
        cFboCore->FboCoreGetMainStage().CoordsGetLeft(),
        cFboCore->FboCoreGetMain().CoordsGetRight()),
      UtilScaleValue(dY, DimGetHeight(),
        cFboCore->FboCoreGetMainStage().CoordsGetTop(),
        cFboCore->FboCoreGetMain().CoordsGetBottom()));
  }
  /* -- Mouse went inside the window --------------------------------------- */
  void InputOnMouseFocus(const EvtMainEvent &emeEvent)
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
  void InputOnMouseWheel(const EvtMainEvent &emeEvent)
  { // Get reference to actual arguments vector
    const EvtMainArgs &emaArgs = emeEvent.eaArgs;
    // Get movements
    const double dX = emaArgs[1].Double(), dY = emaArgs[2].Double();
    // If console is enabled and ctrl not pressed? Send it to console instead
    if(cConsole->IsVisible()) return cConGfx->ConGfxOnMouseWheel(dX, dY);
    // Set event to lua callbacks
    lfOnMouseScroll.LuaFuncDispatch(dX, dY);
  }
  /* -- Mouse button clicked ----------------------------------------------- */
  void InputOnMouseClick(const EvtMainEvent &emeEvent)
  { // Get reference to actual arguments vector
    const EvtMainArgs &emaArgs = emeEvent.eaArgs;
    // Set event to lua callbacks
    lfOnMouseClick.LuaFuncDispatch(emaArgs[1].Int(), emaArgs[2].Int(),
      emaArgs[3].Int());
  }
  /* -- Unfiltered key pressed --------------------------------------------- */
  void InputOnKeyPress(const EvtMainEvent &emeEvent)
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
      const bool bFullScreen = !cCVars->CVarsGetInternal<bool>(VID_FS);
      // Set full screen setting depending on current state
      cCVars->SetInternal<bool>(VID_FS, bFullScreen);
      // Send command to toggle full-screen
      cEvtWin->AddUnblock(EWC_WIN_TOGGLEFS, bFullScreen);
      // Don't send key to guest
      return;
    } // Return if console handled this key
    else if(cConGfx->ConGfxIsKeyHandled(iKey, iState, iMod)) return;
    // Compare state
    switch(iState)
    { // Key initially pressed down?
      case GLFW_PRESS:
        // If modifier key or console key not pressed and setting console to
        // enabled worked? Ignore first key as registering OnCharPress will
        // trigger this keystroke and print it out in the console.
        if(!iMod && (iKey == iConKey1 || iKey == iConKey2) &&
           cConGfx->ConGfxSetVisible(true))
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
  void InputOnDragDrop(const EvtMainEvent&)
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
  void InputOnWindowPaste(const EvtMainEvent&)
  { // Get text in clipboard
    UtfDecoder udStr{ cGlFW->WinGetClipboard() };
    // For each character, ddd the character to queue if valid
    while(const Codepoint cChar = udStr.UtfNext())
      if(cChar >= 32) cConsole->OnCharPress(cChar);
  }
  /* -- Commit cursor visibility ------------------------------------------- */
  void InputCommitCursor()
    { cEvtWin->AddUnblock(EWC_WIN_CURSETVIS, FlagIsSet(IF_CURSOR)); }
  /* -- Set visibility of mouse cursor ------------------------------------- */
  void InputSetCursor(const bool bState)
  { // Set member var incase window needs to reinit so we can restore the
    // cursor state
    FlagSetOrClear(IF_CURSOR, bState);
    // Request to set cursor visibility
    InputCommitCursor();
  }
  /* -- Reset environment -------------------------------------------------- */
  void InputResetEnvironment()
  { // Reset cursor visibility
    InputSetCursor(true);
    // Reset joystick environment
    JoyReset();
  }
  /* -- Update window size from actual glfw window ------------------------- */
  void InputUpdateWindowSize() { DimSet(cGlFW->WinGetSize()); }
  /* -- Request input state ------------------------------------------------ */
  void InputRequestMousePosition() const
    { cEvtWin->AddUnblock(EWC_WIN_CURPOSGET); }
  /* -- Forcefully move the cursor ----------------------------------------- */
  void InputSetCursorPos(const GLfloat fX, const GLfloat fY)
  { // Expand the stage co-ordinates to actual desktop window co-ordinates
    const GLfloat
      fAdjX = (fX + -cFboCore->FboCoreGetMainStage().CoordsGetLeft()) /
        cFboCore->FboCoreGetMain().CoordsGetRight() * DimGetWidth(),
      fAdjY = (fY + -cFboCore->FboCoreGetMainStage().CoordsGetTop()) /
        cFboCore->FboCoreGetMain().CoordsGetBottom() * DimGetHeight(),
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
  void InputSetCursorCentre()
    { InputSetCursorPos(cFboCore->FboCoreGetMatrixWidth() / 2.0f,
                        cFboCore->FboCoreGetMatrixHeight() / 2.0f); }
  /* -- Init --------------------------------------------------------------- */
  void InputInit()
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
    InputSetLockKeyModEnabled(cCVars->CVarsGetInternal<bool>(INP_LOCKKEYMODS));
    InputSetRawMouseEnabled(cCVars->CVarsGetInternal<bool>(INP_RAWMOUSE));
    InputSetStickyKeyEnabled(cCVars->CVarsGetInternal<bool>(INP_STICKYKEY));
    InputSetStickyMouseEnabled(
      cCVars->CVarsGetInternal<bool>(INP_STICKYMOUSE));
    // Set/Restore cursor state
    InputSetCursor(FlagIsSet(IF_CURSOR));
    // Init joystick system
    JoyInit();
    // Log progress
    cLog->LogDebugExSafe("Input interface initialised (R:$;J:$).",
      StrFromBoolTF(GlFWIsRawMouseMotionSupported()), JoyGetCount());
  }
  /* -- DeInit ------------------------------------------------------------- */
  void InputDeInit()
  { // Ignore if class not initialised
    if(IHNotDeInitialise()) return;
    // Log progress
    cLog->LogDebugSafe("Input interface deinitialising...");
    // De-init joystick system
    JoyDeInit();
    // Log progress
    cLog->LogDebugSafe("Input interface deinitialised.");
  }
  /* -- Constructor --------------------------------------------- */ protected:
  Input() :
    /* -- Initialisers ----------------------------------------------------- */
    InitHelper{ __FUNCTION__ },        // Init initialisation helper class
    InputFlags{ IF_NONE },             // No flags set initially
    /* -- Init events for event manager ------------------------------------ */
    EvtMainRegAuto{ cEvtMain, {        // Events list to register
      { EMC_INP_CHAR,        bind(&Input::InputOnFilteredKey, this, _1) },
      { EMC_INP_PASTE,       bind(&Input::InputOnWindowPaste, this, _1) },
      { EMC_INP_MOUSEMOVE,   bind(&Input::InputOnMouseMove,   this, _1) },
      { EMC_INP_MOUSECLICK,  bind(&Input::InputOnMouseClick,  this, _1) },
      { EMC_INP_MOUSEFOCUS,  bind(&Input::InputOnMouseFocus,  this, _1) },
      { EMC_INP_MOUSESCROLL, bind(&Input::InputOnMouseWheel,  this, _1) },
      { EMC_INP_KEYPRESS,    bind(&Input::InputOnKeyPress,    this, _1) },
      { EMC_INP_DRAGDROP,    bind(&Input::InputOnDragDrop,    this, _1) },
      { EMC_INP_DRAGDROP,    bind(&Joystick::JoyOnState,      this, _1) },
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
  /* -- Destructor --------------------------------------------------------- */
  DTORHELPER(~Input, InputDeInit())
  /* -- CVar callback to toggle raw mouse -------------------------- */ public:
  CVarReturn InputSetRawMouseEnabled(const bool bState)
  { // Send request to set raw mouse motion state if enabled
    if(IHIsInitialised()) cEvtWin->AddUnblock(EWC_WIN_SETRAWMOUSE, bState);
    // CVar allowed to be set
    return ACCEPT;
  }
  // -- CVar callback to toggle sticky keys -------------------------------- */
  CVarReturn InputSetStickyKeyEnabled(const bool bState)
  { // Send request to set sticky keys state if enabled
    if(IHIsInitialised()) cEvtWin->AddUnblock(EWC_WIN_SETSTKKEYS, bState);
    // CVar allowed to be set
    return ACCEPT;
  }
  // -- CVar callback to toggle lock key mod bits -------------------------- */
  CVarReturn InputSetLockKeyModEnabled(const bool bState)
  { // Send request to set lock key mods state if enabled
    if(IHIsInitialised()) cEvtWin->AddUnblock(EWC_WIN_SETLKMODS, bState);
    // CVar allowed to be set
    return ACCEPT;
  }
  // -- CVar callback to toggle sticky mouse ------------------------------- */
  CVarReturn InputSetStickyMouseEnabled(const bool bState)
  { // Send request to set sticky mouse if enabled
    if(IHIsInitialised()) cEvtWin->AddUnblock(EWC_WIN_SETSTKMOUSE, bState);
    // CVar allowed to be set
    return ACCEPT;
  }
  /* -- Set first console key ---------------------------------------------- */
  CVarReturn InputSetConsoleKey1(const int iK)
    { return CVarSimpleSetIntNG(iConKey1, iK, GLFW_KEY_LAST); }
  /* -- Set send joystick events at startup -------------------------------- */
  CVarReturn InputSetSendEventsEnabled(const bool bState)
    { FlagSetOrClear(IF_INITEVENTS, bState); return ACCEPT; }
  /* -- Set second console key --------------------------------------------- */
  CVarReturn InputSetConsoleKey2(const int iK)
    { return CVarSimpleSetIntNG(iConKey2, iK, GLFW_KEY_LAST); }
  /* -- Set full screen toggler -------------------------------------------- */
  CVarReturn InputSetFSTogglerEnabled(const bool bState)
    { FlagSetOrClear(IF_FSTOGGLER, bState); return ACCEPT; }
  /* -- Set clamp mouse cursor --------------------------------------------- */
  CVarReturn InputSetClampMouse(const bool bState)
    { FlagSetOrClear(IF_CLAMPMOUSE, bState); return ACCEPT; }
};/* ----------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
