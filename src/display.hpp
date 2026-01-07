/* == DISPLAY.HPP ========================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## This module handles window creation and manipulation via the GLFW   ## **
** ## library.                                                            ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IDisplay {                   // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace ICommon::P;            using namespace IConGraph::P;
using namespace IConsole::P;           using namespace ICoord::P;
using namespace ICVar::P;              using namespace ICVarDef::P;
using namespace ICVarLib::P;           using namespace IDim::P;
using namespace IDimCoord::P;          using namespace IDir::P;
using namespace IEvtMain::P;           using namespace IEvtWin::P;
using namespace IFboCore::P;           using namespace IFlags;
using namespace IFont::P;              using namespace IGlFW::P;
using namespace IGlFWCursor::P;        using namespace IGlFWMonitor::P;
using namespace IGlFWUtil::P;          using namespace IHelper::P;
using namespace IIdent::P;             using namespace IImage::P;
using namespace IImageDef::P;          using namespace IInput::P;
using namespace ILog::P;               using namespace ILuaFunc::P;
using namespace IMutex::P;             using namespace IStd::P;
using namespace IString::P;            using namespace ISystem::P;
using namespace ISysUtil::P;           using namespace IToken::P;
using namespace IUtf::P;               using namespace IUtil::P;
using namespace Lib::OS::GlFW::Types;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* -- Public typedefs ------------------------------------------------------ */
BUILD_FLAGS(Display,                   // Display flags
  /* -- Active flags ------------------------------------------------------- */
  DF_NONE                   {Flag(0)}, // No display flags
  DF_FOCUSED                {Flag(1)}, // Window is focused?
  DF_EXCLUSIVE              {Flag(2)}, // Exclusive mode full-screen?
  DF_NATIVEFS               {Flag(3)}, // Full-screen locked?
  DF_INFULLSCREEN           {Flag(4)}, // Window is actually in fullscreen?
  DF_BADPOS                 {Flag(5)}, // Bad position was specified?
  DF_BADSIZE                {Flag(6)}, // Bad size was specified?
  /* -- End-user configuration flags --------------------------------------- */
  DF_FORWARD               {Flag(47)}, // Use forward compatible context?
  DF_DOUBLEBUFFER          {Flag(48)}, // Use double-buffering?
  DF_AUTOICONIFY           {Flag(49)}, // Automatic minimise?
  DF_AUTOFOCUS             {Flag(50)}, // Focus on show?
  DF_SIZABLE               {Flag(51)}, // Window is resizable?
  DF_FLOATING              {Flag(52)}, // Always on top?
  DF_BORDER                {Flag(53)}, // Window has a border?
  DF_MINFOCUS              {Flag(54)}, // Minimize on lose focus?
  DF_SRGB                  {Flag(56)}, // SRGB namespace is enabled?
  DF_GASWITCH              {Flag(57)}, // Graphics switching enabled?
  DF_FULLSCREEN            {Flag(58)}, // Full-screen mode set?
  DF_CLOSEABLE             {Flag(59)}, // Window is closable?
  DF_STEREO                {Flag(60)}, // Stereo mode enabled?
  DF_DEBUG                 {Flag(61)}, // OpenGL debug context?
  DF_TRANSPARENT           {Flag(62)}, // Window transparency enabled?
  DF_NOERRORS              {Flag(63)}, // No opengl errors?
  DF_MAXIMISED             {Flag(64)}  // Window maximised at start?
);/* ----------------------------------------------------------------------- */
/* -- HIDPI setting enums -------------------------------------------------- */
enum HiDPISetting                      // Possible values for 'vid_hidpi'
{ /* ----------------------------------------------------------------------- */
  HD_DISABLED,                         // [0] Do not enable HiDPI
  HD_ENABLED,                          // [1] Enable HiDPI
  HD_ENHANCED                          // [2] Enable HiDPI keep main FBO DPI
};/* ----------------------------------------------------------------------- */
/* -- Display class -------------------------------------------------------- */
class Display;                         // Class prototype
static Display *cDisplay = nullptr;    // Pointer to global class
class Display :                        // Actual class body
  /* -- Base classes ------------------------------------------------------- */
  private InitHelper,                  // Initialisation helper
  public  DisplayFlags,                // Display settings
  private DimCoInt,                    // Requested window position and size
  public  GlFWMonitors,                // Monitor list data
  private MutexLock,                   // Mutex for sychronising engine thread
  private condition_variable,          // CV for sychronising engine thread
  private EvtMainRegAuto,              // Main events list to register
  private EvtWinRegAuto                // Window events list to register
{ /* -- Monitors and resolutions ------------------------------------------- */
  const GlFWMonitor *gfwmActive;       // Monitor selected
  const GlFWRes     *gfwrActive;       // Monitor resolution selected
  size_t           stMRequested,       // Monitor id request
                   stVRequested;       // Video mode requested
  AtomicBool       abUnsuspend;        // Request to un-suspend window thread
  /* -- Configuration ------------------------------------------------------ */
  DimGLFloat       dfMatrix,           // Currently selected frame-buffer dims
                   dfMatrixReq,        // Requested frame-buffer dimensions
                   dfWinScale,         // Active window scale dimensions
                   dfLastScale;        // DisplayInit window scale dimensions
  GLfloat          fGamma;             // Monitor gamma setting
  CoordInt         ciPosition;         // Window position
  HiDPISetting     hdpiSetting;        // High DPI handling setting
  int              iApi,               // Selected API from GLFW
                   iProfile,           // Selected profile for the context
                   iCtxMajor,          // Selected context major version
                   iCtxMinor,          // Selected context minor version
                   iRobustness,        // Selected context robustness
                   iRelease,           // Selected context release behaviour
                   iFBDepthR,          // Selected red bit depth mode
                   iFBDepthG,          // Selected green bit depth mode
                   iFBDepthB,          // Selected blue bit depth mode
                   iFBDepthA,          // Selected alpha bit depth mode
                   iSamples;           // FSAA setting to use
  /* -- Icons -------------------------------------------------------------- */
  GlFWIconVector   gfwivIcons;         // Contiguous memory for glfw image data
  ImageVector      ivIcons;            // Image icon data
  /* -- Events ----------------------------------------------------- */ public:
  LuaFunc          lrFocused;          // Window focused lua event
  /* -- Public typedefs ---------------------------------------------------- */
  enum FSType {                        // Available window types
    /* --------------------------------------------------------------------- */
    FST_STANDBY,                       // [0] Full-screen type is not set
    FST_WINDOW,                        // [1] Window/desktop mode
    FST_EXCLUSIVE,                     // [2] Exclusive full-screen mode
    FST_BORDERLESS,                    // [3] Borderless full-screen mode
    FST_NATIVE,                        // [4] Native full-screen mode (MacOS)
    FST_MAX,                           // [5] Number of id's
    /* --------------------------------------------------------------------- */
  } fsType;                            // Current value
  /* -- Display class ------------------------------------------------------ */
  typedef IdList<FST_MAX> FSTStrings;  // List of FST_ id strings typedef.
  const FSTStrings    fstStrings;      // " container
  /* -- Check if window moved ------------------------------------- */ private:
  void DisplayCheckWindowMoved(const int iNewX, const int iNewY)
  { // If position not changed? Report event and return
    if(ciPosition.CoordGetX() == iNewX && ciPosition.CoordGetY() == iNewY)
      return cLog->LogDebugExSafe("Display received window position of $x$.",
        iNewX, iNewY);
    // Report change
    cLog->LogDebugExSafe("Display changed window position from $x$ to $x$.",
      ciPosition.CoordGetX(), ciPosition.CoordGetY(), iNewX, iNewY);
    // Update position
    ciPosition.CoordSet(iNewX, iNewY);
  }
  /* -- Window moved request ----------------------------------------------- */
  void DisplayOnMoved(const EvtMainEvent &emeEvent)
  { // Get reference to actual arguments vector
    const EvtMainArgs &emaArgs = emeEvent.eaArgs;
    // Check to see if the window moved
    DisplayCheckWindowMoved(emaArgs[1].Int(), emaArgs[2].Int());
  }
  /* -- Window set icon request -------------------------------------------- */
  void DisplayOnRqSetIcons(const EvtWinEvent&) { DisplayUpdateIcons(); }
  /* -- Window set set lock key mods state --------------------------------- */
  void DisplayOnRqSetLKMods(const EvtWinEvent &eweEvent)
  { // Set the new lock key mod state and log status
    cGlFW->WinSetLockKeyMods(eweEvent.eaArgs.front().Bool());
    cLog->LogDebugExSafe("Input updated lock key mod status to $.",
      StrFromBoolTF(cGlFW->WinGetLockKeyMods()));
  }
  /* -- Window set set cursor visibility ----------------------------------- */
  void DisplayOnRqSetCurVis(const EvtWinEvent &eweEvent)
  { // Get requested state
    const bool bState = eweEvent.eaArgs.front().Bool();
    // Set the new input if we can and log status
    cGlFW->WinSetCursor(bState);
    cLog->LogDebugExSafe("Input updated cursor visibility status to $.",
      StrFromBoolTF(bState));
  }
  /* -- Window set raw mouse request --------------------------------------- */
  void DisplayOnRqSetRawMouse(const EvtWinEvent &eweEvent)
  { // If raw mouse support is supported?
    if(cGlFW->GlFWIsNotRawMouseMotionSupported()) return;
    // Set the new input if we can and log status
    cGlFW->WinSetRawMouseMotion(eweEvent.eaArgs.front().Bool());
    cLog->LogDebugExSafe("Input updated raw mouse status to $.",
      StrFromBoolTF(cGlFW->WinGetRawMouseMotion()));
  }
  /* -- On request window attenti on event --------------------------------- */
  void DisplayOnRqAttention(const EvtWinEvent&)
    { cGlFW->WinRequestAttention(); }
  /* -- On request window focus event -------------------------------------- */
  void DisplayOnRqFocus(const EvtWinEvent&) { cGlFW->WinFocus(); }
  /* -- On request window maximise event ----------------------------------- */
  void DisplayOnRqMaximise(const EvtWinEvent&) { cGlFW->WinMaximise(); }
  /* -- On request window minimise event ----------------------------------- */
  void DisplayOnRqMinimise(const EvtWinEvent&) { cGlFW->WinMinimise(); }
  /* -- On request window restore event ------------------------------------ */
  void DisplayOnRqRestore(const EvtWinEvent&) { cGlFW->WinRestore(); }
  /* -- Window set sticky keys request ------------------------------------- */
  void DisplayOnRqStickyKeys(const EvtWinEvent &eweEvent)
  { // Set the new input if we can and log status
    cGlFW->WinSetStickyKeys(eweEvent.eaArgs.front().Bool());
    cLog->LogDebugExSafe("Input updated sticky keys status to $.",
      StrFromBoolTF(cGlFW->WinGetStickyKeys()));
  }
  /* -- Window set sticky mouse request ------------------------------------ */
  void DisplayOnRqStickyMouse(const EvtWinEvent &eweEvent)
  { // Set the new input if we can and log status
    cGlFW->WinSetStickyMouseButtons(eweEvent.eaArgs.front().Bool());
    cLog->LogDebugExSafe("Input updated sticky mouse status to $.",
      StrFromBoolTF(cGlFW->WinGetStickyMouseButtons()));
  }
  /* -- Window was asked to be hidden or shown ----------------------------- */
  void DisplayOnRqHide(const EvtWinEvent&) { cGlFW->WinHide(); }
  void DisplayOnRqShow(const EvtWinEvent&) { cGlFW->WinShow(); }
  /* -- Resend mouse position ---------------------------------------------- */
  void DisplayOnRqGetCurPos(const EvtWinEvent&)
    { cGlFW->WinSendMousePosition(); }
  /* -- Set mouse position ------------------------------------------------- */
  void DisplayOnRqSetCurPos(const EvtWinEvent &eweEvent)
    { cGlFW->WinSetCursorPos(eweEvent.eaArgs.front().Double(),
        eweEvent.eaArgs.back().Double()); }
  /* -- Window cursor request ---------------------------------------------- */
  void DisplayOnRqSetCurImg(const EvtWinEvent &eweEvent)
    { cGlFW->GlFWSetCursor(static_cast<GlFWCursorType>
        (eweEvent.eaArgs.front().SizeT())); }
  /* -- Window reset cursor request ---------------------------------------- */
  void DisplayOnRqResetCurImg(const EvtWinEvent&)
    { cGlFW->WinSetCursorGraphic(); }
  /* -- Window scale change request ---------------------------------------- */
  void DisplayOnScale(const EvtMainEvent &emeEvent)
  { // Get reference to actual arguments vector
    const EvtMainArgs &emaArgs = emeEvent.eaArgs;
    // Get new values
    const float fNewWidth = emaArgs[1].Float(),
                fNewHeight = emaArgs[2].Float();
    // If scale not changed? Report event and return
    if(StdIsFloatEqual(fNewWidth, dfWinScale.DimGetWidth()) &&
       StdIsFloatEqual(fNewHeight, dfWinScale.DimGetHeight()))
      return cLog->LogDebugExSafe("Display received window scale of $x$.",
        fNewWidth, fNewHeight);
    // Set scale to detect if we went from HiDPI to LoDPI
    if(StdIsFloatEqual(fNewWidth, 1.0f) &&
       StdIsFloatEqual(fNewHeight, 1.0f) &&
       fNewWidth < dfWinScale.DimGetWidth() &&
       fNewHeight < dfWinScale.DimGetHeight())
      dfLastScale.DimSet(dfWinScale);
    else dfLastScale.DimSet(0.0f, 0.0f);
    // Report change
    cLog->LogInfoExSafe("Display changed window scale from $x$ to $x$.",
      dfWinScale.DimGetWidth(), dfWinScale.DimGetHeight(),
      fNewWidth, fNewHeight);
    // Set new value
    dfWinScale.DimSet(fNewWidth, fNewHeight);
  }
  /* -- Window limits change request --------------------------------------- */
  void DisplayOnRqSetLimits(const EvtWinEvent &eweEvent)
  { // Get reference to actual arguments vector
    const EvtWinArgs &ewaArgs = eweEvent.eaArgs;
    // Get the new limits
    const int iMinW = ewaArgs[0].Int(), iMinH = ewaArgs[1].Int(),
              iMaxW = ewaArgs[2].Int(), iMaxH = ewaArgs[3].Int();
    // Set the new limits
    cGlFW->WinSetLimits(iMinW, iMinH, iMaxW, iMaxH);
  }
  /* -- Window focused ----------------------------------------------------- */
  void DisplayOnFocus(const EvtMainEvent &emeEvent)
  { // Get state and check it
    const int iState = emeEvent.eaArgs[1].Int();
    switch(iState)
    { // Focus restored?
      case GLFW_TRUE:
        // Return if we already recorded an focus event.
        if(FlagIsSet(DF_FOCUSED)) return;
        // Window is focused
        FlagSet(DF_FOCUSED);
        // Send message
        cLog->LogDebugSafe("Display window focus restored.");
        // Done
        break;
      // Focus lost?
      case GLFW_FALSE:
        // Return if we already recorded an unfocus event.
        if(FlagIsClear(DF_FOCUSED)) return;
        // Window is focused
        FlagClear(DF_FOCUSED);
        // Send message
        cLog->LogDebugSafe("Display window focus lost.");
        // Done
        break;
      // Unknown state?
      default:
        // Log the unknown state
        cLog->LogWarningExSafe("Display received unknown focus state $<0x$$>!",
          iState, hex, iState);
        // Done
        return;
    } // Dispatch event to lua
    lrFocused.LuaFuncDispatch(iState);
  }
  /* -- Window contents damaged and needs refreshing ----------------------- */
  void DisplayOnRefresh(const EvtMainEvent&)
  { // Compare draw setting
    if(cFboCore->FboCoreCannotDraw())
    { // Set to copy main FBO to the back buffer
      cFboCore->FboCoreClearDrawPartial();
      // Log that we got this event
      cLog->LogDebugSafe("Display refresh request redrawing window contents.");
    } // Now redrawing because it's already set.
    else cLog->LogDebugSafe("Display refresh request already requested.");
  }
  /* == Check if window resized ============================================ */
  void DisplayCheckWindowResized(const int iWidth, const int iHeight) const
  { // If position not changed? Report event and return
    if(cInput->DimGetWidth() == iWidth && cInput->DimGetHeight() == iHeight)
      return cLog->LogDebugExSafe("Display received window size of $x$.",
        iWidth, iHeight);
    // Report change
    cLog->LogDebugExSafe("Display changed window size from $x$ to $x$.",
      cInput->DimGetWidth(), cInput->DimGetHeight(), iWidth, iHeight);
    // Update position
    cInput->DimSet(iWidth, iHeight);
  }
  /* -- On window resized callback ----------------------------------------- */
  void DisplayOnResized(const EvtMainEvent &emeEvent)
  { // Get reference to actual arguments vector
    const EvtMainArgs &emaArgs = emeEvent.eaArgs;
    // Check if the window resized
    DisplayCheckWindowResized(emaArgs[1].Int(), emaArgs[2].Int());
  }
  /* -- On window closed callback ------------------------------------------ */
  void DisplayOnClose(const EvtMainEvent&)
  { // If window is not closable then ignore the event
    if(FlagIsClear(DF_CLOSEABLE)) return;
    // Send quit event
    cEvtMain->RequestQuit();
  }
  /* -- Window iconified --------------------------------------------------- */
  void DisplayOnIconify(const EvtMainEvent &emeEvent)
  { // Get state and check it
    switch(const int iState = emeEvent.eaArgs[1].Int())
    { // Minimized? Log that we minimised and return
      case GLFW_TRUE:
        return cLog->LogDebugSafe("Display window state minimised.");
      // Restored? Redraw console at least and log event
      case GLFW_FALSE:
        cFboCore->FboCoreSetDraw();
        cLog->LogDebugSafe("Display window state restored.");
        break;
      // Unknown state so log it
      default:
        cLog->LogWarningExSafe("Display received unknown iconify state $.",
          iState);
        break;
    }
  }
  /* -- Enumerate monitors ------------------------------------------------- */
  void DisplayEnumerateMonitorsAndVideoModes()
  { // Log initial progress
    cLog->LogDebugSafe("Display now enumerating available displays...");
    // Refresh monitors data
    MonitorsRefresh();
    // Enumerate through each monitor detected
    StdForEach(seq, MonitorsBegin(), MonitorsEnd(),
      [](const GlFWMonitor &gfwmMon)
    { // Write information about the monitor
      cLog->LogDebugExSafe(
        "- Monitor $: $.\n"            "- Mode count: $.\n"
        "- Position: $x$.\n"           "- Dimensions: $$$x$\" ($x$mm).\n"
        "- Size: $\" ($mm).",
        gfwmMon.Index(), gfwmMon.Name(), gfwmMon.Count(), gfwmMon.CoordGetX(),
        gfwmMon.CoordGetY(), fixed, setprecision(1), gfwmMon.WidthInch(),
        gfwmMon.HeightInch(), gfwmMon.DimGetWidth(), gfwmMon.DimGetHeight(),
        gfwmMon.DiagonalInch(), gfwmMon.Diagonal());
      // Enumerate each resolution of the monitor
      StdForEach(seq, gfwmMon.Begin(), gfwmMon.End(),
        [&gfwmMon](const GlFWRes &gfwrRes)
      { // Log resolution data
        cLog->LogDebugExSafe("-- Mode $: $x$x$<RGB$$$> @ $Hz$.",
          gfwrRes.Index(), gfwrRes.Width(), gfwrRes.Height(), gfwrRes.Depth(),
          gfwrRes.Red(), gfwrRes.Green(), gfwrRes.Blue(), gfwrRes.Refresh(),
          &gfwrRes == gfwmMon.PrimaryPtr() ?
            " (Active)" : cCommon->CommonBlank());
      });
    }); // Custom monitor selected (-2) and valid monitor? Set it
    static constexpr size_t stM2 = StdMaxSizeT - 1;
    gfwmActive = (stMRequested < stM2 && stMRequested < MonitorsCount()) ?
      MonitorsGetPtr(stMRequested) : MonitorsPrimaryPtr();
    // Get selected screen resolution
    gfwrActive = (stVRequested < stM2 && stVRequested < gfwmActive->Count()) ?
      gfwmActive->GetPtr(stVRequested) : gfwmActive->PrimaryPtr();
    // Set exclusive full-screen mode flag if value is not -2
    FlagSetOrClear(DF_EXCLUSIVE, stVRequested != stM2);
    // Get primary monitor and mode
    const GlFWMonitor &gfwmPrimaryRef = MonitorsPrimary();
    const GlFWRes &gfwrPrimaryRef = gfwmPrimaryRef.Primary();
    // Write to log the monitor we are using
    cLog->LogInfoExSafe("Display finished enumerating $ displays...\n"
                        "- Selected monitor $: $ @$x$ ($$$\"x$\"=$\").\n"
                        "- Selected mode $: $x$x$bpp(R$G$B$) @$hz.",
      MonitorsCount(),
      gfwmActive->Index(), gfwmActive->Name(), gfwmActive->CoordGetX(),
        gfwmActive->CoordGetY(), fixed, setprecision(1),
        gfwmActive->WidthInch(), gfwmActive->HeightInch(),
        gfwmActive->DiagonalInch(),
      gfwrActive->Index(), gfwrActive->Width(), gfwrActive->Height(),
        gfwrActive->Depth(), gfwrActive->Red(), gfwrActive->Green(),
        gfwrActive->Blue(), gfwrActive->Refresh());
    cLog->LogDebugExSafe("- Primary monitor $: $ @$x$ ($$$\"x$\"=$\").\n"
                         "- Primary mode $: $x$x$bpp(R$G$B$) @$hz.",
      gfwmPrimaryRef.Index(), gfwmPrimaryRef.Name(),
        gfwmPrimaryRef.CoordGetX(), gfwmPrimaryRef.CoordGetY(), fixed,
        setprecision(1), gfwmPrimaryRef.WidthInch(),
        gfwmPrimaryRef.HeightInch(), gfwmPrimaryRef.DiagonalInch(),
      gfwrPrimaryRef.Index(), gfwrPrimaryRef.Width(), gfwrPrimaryRef.Height(),
        gfwrPrimaryRef.Depth(), gfwrPrimaryRef.Red(), gfwrPrimaryRef.Green(),
        gfwrPrimaryRef.Blue(), gfwrPrimaryRef.Refresh());
  }
  /* -- Monitors refresh requested ----------------------------------------- */
  void DisplayOnRqMonitors(const EvtWinEvent &)
  { // Lock mutex and make other requests wait
    MutexUniqueCall([this](UniqueLock &ulLock){
      // Log that we're processing a monitor change event
      cLog->LogDebugSafe("Display got monitor event, suspending engine...");
      // Capture exceptions so we can resume a suspended engine thread
      try
      { // Tell engine thread to suspend
        cEvtMain->Add(EMC_SUSPEND, &abUnsuspend,
          static_cast<condition_variable*>(this));
        // Wait for engine thread to tell us it's suspended
        wait(ulLock, [this]{ return abUnsuspend == true; });
        // Reset our unsuspension variable
        abUnsuspend = false;
        // Log that we're processing a monitor change event
        cLog->LogDebugSafe("Display acknowledged engine suspension.");
        // Enumerate monitors and video modes
        DisplayEnumerateMonitorsAndVideoModes();
        // Engine thread can continue
        cEvtMain->Unsuspend();
        // Log that we're processing a monitor change event
        cLog->LogDebugSafe("Display finished processing monitor event.");
      } // Exception occured?
      catch(...)
      { // Reset our unsuspension variable
        abUnsuspend = false;
        // Engine thread can continue
        cEvtMain->Unsuspend();
        // Throw exception to LUA
        throw;
      }
    });
  }
  /* -- Monitor changed ---------------------------------------------------- */
  void DisplayOnRqMonitor(const EvtWinEvent &eweEvent)
  { // Get reference to actual arguments vector
    const EvtMainArgs &emaArgs = eweEvent.eaArgs;
    // Get connected monitor name. Will be NULL if GLFW_CONNECTED
    if(const char*const cpName = emaArgs[0].CStr())
    { // Log if name is set or not?
      if(*cpName)
        cLog->LogInfoExSafe("Display detected new monitor '$'.", cpName);
      else cLog->LogInfoSafe("Display detected new unnamed monitor.");
    } // Get disconnected monitor data. Will be NULL if GLFW_DISCONNECTED
    else if(const GlFWMonitor *gfwmPtr = emaArgs[1].Ptr<GlFWMonitor>())
    { // If this was our monitor?
      if(gfwmPtr == gfwmActive)
      { // We recognise it so we can savely disconnect it
        cLog->LogInfoExSafe(
          "Display disconnected monitor '$', reinitialising...",
           gfwmPtr->Name());
        // The selected device is no longer valid so make sure it is
        // cleared so DisplayDeInit() doesn't try to restore gamma and crash
        // the whole engine with an exception.
        gfwmActive = nullptr;
        // Request soft reinitialisation of window
        cEvtMain->RequestGLReInitWait();
        // Unsuspend the main thread and return
        return;
      } // Try to get monitor name and if found? Log the name.
      else cLog->LogInfoExSafe("Display disconnected monitor '$'.",
        gfwmActive->Name());
    } // Add event to refresh monitors
    cEvtWin->AddUnblock(EWC_WIN_MONITORS);
  }
  /* -- Add event to reinit matrix ----------------------------------------- */
  void DisplayRequestMatrixReInit() { cEvtMain->Add(EMC_VID_MATRIXREINIT); }
  /* -- Matrix reset requested --------------------------------------------- */
  void DisplayOnMatrixReset(const EvtMainEvent&)
  { // Force-reinitialise matrix
    DisplayCommitMatrix();
    // Inform lua scripts that they should redraw the framebuffer
    cEvtMain->Add(EMC_LUA_REDRAW);
  }
  /* -- Frame buffer was reset --------------------------------------------- */
  void DisplayOnFBReset(const EvtMainEvent &emeEvent)
  { // Get reference to actual arguments vector
    const EvtMainArgs &emaArgs = emeEvent.eaArgs;
    // Get new frame buffer size
    const int iWidth = emaArgs[1].Int(), iHeight = emaArgs[2].Int();
    // On Mac?
#if defined(MACOS)
    // Get addition position and window size data
    const int iWinX     = emaArgs[3].Int(), iWinY      = emaArgs[4].Int(),
              iWinWidth = emaArgs[5].Int(), iWinHeight = emaArgs[6].Int();
    // Log new viewport
    cLog->LogDebugExSafe(
      "Display received new frame buffer size of $x$ (P:$x$;W:$x$).",
      iWidth, iHeight, iWinX, iWinY, iWinWidth, iWinHeight);
    // What is the window type?
    // Frame buffer is covering the entire screen?
    if(!iWinX && !iWinY && gfwrActive->IsDim(iWinWidth, iWinHeight))
    { // We don't recognise the full-screen?
      if(FlagIsClear(DF_INFULLSCREEN))
      { // Now a native borderless full-screen window
        fsType = FST_NATIVE;
        // In native full-screen mode also reinit the console FBO as well.
        FlagSet(DF_INFULLSCREEN|DF_NATIVEFS);
        // Log non-standard full-screen switch
        cLog->LogDebugSafe("Display received external full-screen switch!");
        // Update viewport and check if window moved/resized as glfw wont
        goto UpdateViewport;
      }
    } // Full-screen mode flag is set?
    else if(FlagIsSet(DF_INFULLSCREEN))
    { // Now a normal window again
      fsType = FST_WINDOW;
      // Not in native full-screen mode and reinit console FBO
      FlagClear(DF_INFULLSCREEN|DF_NATIVEFS);
      // Log non-standard full-screen switch
      cLog->LogDebugSafe("Display received external desktop switch!");
      // Update viewport jump from above if/condition scope
      UpdateViewport: DisplayRequestMatrixReInit();
      // Check if window moved/resized as glfw wont send these
      DisplayCheckWindowMoved(iWinX, iWinY);
      DisplayCheckWindowResized(iWinWidth, iWinHeight);
    } // Clear native flag otherwise
    else if(FlagIsSet(DF_NATIVEFS)) FlagClear(DF_NATIVEFS);
    // Anything but Mac?
#else
    // Just log new viewport
    cLog->LogDebugExSafe("Display received new framebuffer size of $x$.",
      iWidth, iHeight);
#endif
    // Resize main viewport and if it changed, reinitialise the console FBO
    // and redraw the console
    if(cFboCore->FboCoreAutoViewport(iWidth, iHeight))
      cConGfx->ConGfxInitFBO();
    // Redraw the console if enabled
    else cConsole->SetRedrawIfEnabled();
  }
  /* -- Window size requested ---------------------------------------------- */
  void DisplayOnRqResize(const EvtWinEvent &eweEvent)
  { // Get reference to actual arguments vector and send the new size to GLFW
    const EvtWinArgs &ewaArgs = eweEvent.eaArgs;
    cGlFW->WinSetSize({ ewaArgs[0].Int(), ewaArgs[1].Int() });
  }
  /* -- Window move requested ---------------------------------------------- */
  void DisplayOnRqMove(const EvtWinEvent &eweEvent)
  { // Get reference to actual arguments vector and send new position to GLFW
    const EvtWinArgs &ewaArgs = eweEvent.eaArgs;
    cGlFW->WinSetPos({ ewaArgs[0].Int(), ewaArgs[1].Int() });
  }
  /* -- Window centre request ---------------------------------------------- */
  void DisplayOnRqCentre(const EvtWinEvent&)
    { cGlFW->WinSetPos(DisplayGetCentreCoords(*cInput)); }
  /* -- Window reset requested --------------------------------------------- */
  void DisplayOnRqReset(const EvtWinEvent&)
  { // If in full screen mode, don't resize or move anything
    if(FlagIsSet(DF_INFULLSCREEN)) return;
    // Restore window visibility state
    cGlFW->WinRestore();
    // Get optimal window dimensions based on desktop dimensions and Set the
    // new window size and then use the coord translation function to calculate
    // optimal co-ordinates and dimensions for window and update window pos.
    const DimInt diSize{ DisplayTranslateUserSize() };
    cGlFW->WinSetSize(diSize);
    cGlFW->WinSetPos(DisplayTranslateUserCoords(diSize));
  }
  /* -- Toggle full-screen event (Engine thread) --------------------------- */
  void DisplayOnRqToggleFS(const EvtWinEvent &eweEvent)
  { // Ignore further requests if already restarting or using native fullscreen
    if(FlagIsSet(DF_NATIVEFS)) return;
    // Use requested setting instead
    DisplaySetFullScreen(eweEvent.eaArgs.front().Bool());
  }
  /* -- Apply gamma setting ------------------------------------------------ */
  void DisplayApplyGamma()
  { // Set gamma
    GlFWSetGamma(gfwmActive->Context(), fGamma);
    // Report
    cLog->LogDebugExSafe("Display set gamma to $$.", fixed, fGamma);
  }
  /* -- Translate user specified window dimensions ------------------------- */
  DimInt DisplayTranslateUserSize() const
  { // Get window size specified by user and if optimal size requested?
    DimInt diOptimal{ *this };
    if(diOptimal.DimGetWidth() <= 0 && diOptimal.DimGetHeight() <= 0)
    { // Convert selected height to double as we need to use it twice
      const double dHeight = static_cast<double>(gfwrActive->Height());
      // Set the height to 80% of desktop height
      diOptimal.DimSetHeight(static_cast<int>(ceil(dHeight * 0.8)));
      // Now set the width based on desktop aspect ratio
      diOptimal.DimSetWidth(static_cast<int>(
        ceil(diOptimal.DimGetHeight<double>() *
          (static_cast<double>(gfwrActive->Width()) / dHeight))));
      // Report result
      cLog->LogDebugExSafe("Display calculated an optimal window size of $x$.",
        diOptimal.DimGetWidth(), diOptimal.DimGetHeight());
    } // User requested dimensions?
    else cLog->LogDebugExSafe(
      "Display using user specified dimensions of $x$.",
      diOptimal.DimGetWidth(), diOptimal.DimGetHeight());
    // Return new dimensions
    return diOptimal;
  }
  /* -- Get centre co-ordinates -------------------------------------------- */
  CoordInt DisplayGetCentreCoords(const DimInt &diSize) const
  { // If not in full-screen?
    if(FlagIsClear(DF_INFULLSCREEN))
    { // Return centre of screen if monitor selected
      if(gfwmActive->Context())
        return { (gfwrActive->Width() / 2) - (diSize.DimGetWidth() / 2),
                 (gfwrActive->Height() / 2) - (diSize.DimGetHeight() / 2) };
      // No monitor selected
      else cLog->LogWarningSafe(
        "Display class cannot centre the window without monitor data.");
    } // Return top-left of screen
    return {};
  }
  /* -- Translate co-ordinates --------------------------------------------- */
  CoordInt DisplayTranslateUserCoords(const DimInt &diSize) const
  { // If the user requested to centre the window then return the window
    // centered or return the user specified position
    return CoordGetX() == -2 || CoordGetY() == -2 ?
      DisplayGetCentreCoords(diSize) : static_cast<CoordInt>(*this);
  }
  /* -- After the Window is adjusted --------------------------------------- */
  template<bool bCheck>const DimCoords
    DisplayPostInitWindow(const CoordInt &ciNPosition, const DimInt &diNSize,
      DimInt &diSize)
  { // Return if we're not checking the new position
    if constexpr(!bCheck)
    { // We're not using this parameter
      static_cast<void>(diSize);
      // Return original co-ords and size
      return { ciNPosition, diNSize };
    } // Get newly selected window co-ordinates and dimensions
    const DimCoords dcNew{ ciNPosition, diNSize };
    // If position is different from requested?
    if(dcNew.CoordIsNotEqual(ciPosition))
    { // Set bad coordinates flag
      FlagSet(DF_BADPOS);
      // If size is different from requested?
      if(dcNew.DimIsNotEqual(diSize))
      { // Set bad dimensions flag
        FlagSet(DF_BADSIZE);
        // Log warning to say the requested size and coords were not honoured
        cLog->LogWarningExSafe(
          "Display set a $x$ window at $x$ instead of $x$ at $x$!",
          dcNew.DimGetWidth(), dcNew.DimGetHeight(), dcNew.CoordGetX(),
          dcNew.CoordGetY(), diSize.DimGetWidth(), diSize.DimGetHeight(),
          ciPosition.CoordGetX(), ciPosition.CoordGetY());
        // Store new size
        diSize.DimSet(dcNew);
      } // Size is not as requested?
      else
      { // Clear bad dimensions flag
        FlagClear(DF_BADSIZE);
        // Log warning to say the requested coordinates were not honoured
        cLog->LogWarningExSafe(
          "Display set a $x$ window at $x$ instead of $x$!",
          dcNew.DimGetWidth(), dcNew.DimGetHeight(), dcNew.CoordGetX(),
          dcNew.CoordGetY(), diSize.DimGetWidth(), diSize.DimGetHeight());
      } // Store new position
      ciPosition.CoordSet(dcNew);
    } // If position is same as requested?
    else
    { // Clear bad coordinates flag
      FlagClear(DF_BADPOS);
      // Size is different as requested?
      if(dcNew.DimIsNotEqual(diSize))
      { // Set bad dimensions flag
        FlagSet(DF_BADSIZE);
        // Log warning to say the requested size was not honoured
        cLog->LogWarningExSafe(
          "Display set a $x$ instead of a $x$ window at $x$!",
          dcNew.DimGetWidth(), dcNew.DimGetHeight(), diSize.DimGetWidth(),
          diSize.DimGetHeight(), dcNew.CoordGetX(), dcNew.CoordGetY());
        // Store new size
        diSize.DimSet(dcNew);
      } // Size is not as requested?
      else
      { // Clear bad dimensions flag
        FlagClear(DF_BADSIZE);
        // Log warning to say the requested cnates were not honoured
        cLog->LogInfoExSafe("Display set a $x$ window at $x$ successfully.",
          dcNew.DimGetWidth(), dcNew.DimGetHeight(), dcNew.CoordGetX(),
          dcNew.CoordGetY());
      }
    } // Return new co-ordinates
    return dcNew;
  }
  /* -- Reinitialise window ------------------------------------------------ */
  void DisplayReInitWindow(const bool bState)
  { // Update user requested values for window attributes
    cGlFW->WinSetFloatingAttrib(FlagIsSet(DF_FLOATING));
    cGlFW->WinSetAutoIconifyAttrib(FlagIsSet(DF_AUTOICONIFY));
    cGlFW->WinSetFocusOnShowAttrib(FlagIsSet(DF_AUTOFOCUS));
    // Initial width and height of window
    DimInt diSize;
    // Full-screen selected?
    if(bState)
    { // Actually in full screen mode window
      FlagSet(DF_INFULLSCREEN);
      // Chosen settings
      GLFWmonitor *mUsing;
      const char *cpType;
      // Exclusive full-screen window requested?
      if(FlagIsSet(DF_EXCLUSIVE))
      { // Set video mode and label
        mUsing = gfwmActive->Context();
        cpType = "exclusive";
        fsType = FST_EXCLUSIVE;
      } // Not exclusive full-screen mode?
      else
      { // Not using exclusive full-screen mode
        mUsing = nullptr;
        cpType = "borderless";
        fsType = FST_BORDERLESS;
        // Need to disable decoration and resizing
        cGlFW->WinSetDecoratedAttribDisabled();
        cGlFW->WinSetResizableAttribDisabled();
      } // Position is top-left in full-screen
      ciPosition.CoordSet();
      // Set initial position of window to top-left
#if !defined(LINUX)
      cGlFW->GlFWSetPositionX(0);
      cGlFW->GlFWSetPositionY(0);
#endif
      // Set initial window width and height
      diSize.DimSet(gfwrActive->Width(), gfwrActive->Height());
      // Log that we are switching to full-screen mode. Casting requested
      // monitor and video mode to int so it displays as -1 and not max uint64.
      cLog->LogInfoExSafe("Display setting a $x$ $ full-screen window...",
        diSize.DimGetWidth(), diSize.DimGetHeight(), cpType);
      // Instruct glfw to set full-screen window
      cGlFW->WinSetMonitor(mUsing, ciPosition, diSize, gfwrActive->Refresh());
    } // Window mode selected
    else
    { // Not in full-screen mode or native mode
      FlagClear(DF_INFULLSCREEN|DF_NATIVEFS);
      // Trnslate user specified window size
      diSize.DimSet(DisplayTranslateUserSize());
      ciPosition.CoordSet(DisplayTranslateUserCoords(diSize));
      // Set initial position of window
#if !defined(LINUX)
      cGlFW->GlFWSetPositionX(ciPosition.CoordGetX());
      cGlFW->GlFWSetPositionY(ciPosition.CoordGetY());
#endif
      // Is a desktop mode window (Could change via DisplayOnFBReset())
      fsType = FST_WINDOW;
      // We need to adjust to the position of the currently selected monitor so
      // it actually appears on that monitor.
      ciPosition.CoordInc(*gfwmActive);
      // Log that we switched to window mode
      cLog->LogInfoExSafe("Display setting a $x$ desktop window at $x$...",
        diSize.DimGetWidth(), diSize.DimGetHeight(), ciPosition.CoordGetX(),
        ciPosition.CoordGetY());
      // Window mode so update users window border
      cGlFW->WinSetDecoratedAttrib(FlagIsSet(DF_BORDER));
      cGlFW->WinSetResizableAttrib(FlagIsSet(DF_SIZABLE));
      // Instruct glfw to change to window mode
      cGlFW->WinSetMonitor(nullptr, ciPosition, diSize, 0);
    } // Calculate new co-ordinates and size
    const DimCoords dcNew{
    // If compiling on Linux?
#if defined(LINUX)
      // Getting position and size not available
      DisplayPostInitWindow<false>(ciPosition, diSize, diSize)
#else
      // Get new position and size normally
      DisplayPostInitWindow<true>(cGlFW->WinGetPos(),
        cGlFW->WinGetSize(), diSize)
#endif
    };
    // Store initial window size. This needs to be done because on Linux, the
    // window size isn't sent so we need to store the value.
    cInput->DimSet(diSize);
    // Remove native flag since GLFW cannot set or detect this directly.
    FlagClear(DF_NATIVEFS);
    // Need to fix a GLFW scaling bug with this :(
#if defined(MACOS)
    // Store scale of window
    dfWinScale.DimSet(cGlFW->WinGetScale());
    // If hidpi not enabled? Update the main FBO viewport size without scale
    switch(hdpiSetting)
    { // Disabled?
      case HD_DISABLED: cFboCore->DimSet(cInput->DimGet<GLsizei>()); break;
      // Enabled with downscale fix?
      case HD_ENHANCED:
        // If we went from >1x scale to 1x scale?
        if(StdIsFloatEqual(dfWinScale.DimGetWidth(), 1.0f) &&
           StdIsFloatEqual(dfWinScale.DimGetHeight(), 1.0f) &&
           dfWinScale.DimGetWidth() < dfLastScale.DimGetWidth() &&
           dfWinScale.DimGetHeight() < dfLastScale.DimGetWidth())
        { // Increase size by old scale size
          diSize.DimSet(static_cast<int>(diSize.DimGetWidth<GLfloat>() *
                          dfLastScale.DimGetWidth()),
                        static_cast<int>(diSize.DimGetHeight<GLfloat>() *
                          dfLastScale.DimGetHeight()));
          // Copy to the frame buffer upwards too since we're at 1x
          cFboCore->DimSet(diSize);
          // Set new window size to match the previous window size
          cGlFW->WinSetSize(diSize);
          // Write that we applied a fix
          cLog->LogDebugExSafe("Display fixed window size to $x$ due to "
            " DPI scale decrease from $x$ to $x$", diSize.DimGetWidth(),
            diSize.DimGetHeight(), dfLastScale.DimGetWidth(),
            dfLastScale.DimGetHeight(), dfWinScale.DimGetWidth(),
            dfWinScale.DimGetHeight());
          // Clear window scale
          dfWinScale.DimSet(0.0f, 0.0f);
          // Done
          break;
        } // Fall through
        [[fallthrough]];
      // Unknown setting (impossible)
      default: [[fallthrough]];
      // Enabled with no fixes
      case HD_ENABLED:
        // Just scale the frame buffer
        cFboCore->DimSet(static_cast<int>(cInput->DimGetWidth<GLfloat>() *
                           dfWinScale.DimGetWidth()),
                         static_cast<int>(cInput->DimGetHeight<GLfloat>() *
                           dfWinScale.DimGetHeight()));
        // Done
        break;
    }
    // Were any user specified parameters bad? GLFW has a bug where an invalid
    // window dimensions in MacOS can mess up the frame buffer.
    if(FlagIsAnyOfSet(DF_BADPOS|DF_BADSIZE))
    { // Move window position by a pixel to force GLFW to fix the frame buffer
      // and then move the window back to the original position.
      cGlFW->WinSetPos({ ciPosition.CoordGetX() + 1, ciPosition.CoordGetY() });
      DisplayRequestReposition();
    } // Windows and linux?
#else
    // Update the main FBO viewport size without scale
    cFboCore->DimSet(cInput->DimGet<GLsizei>());
#endif
    // Window has been focued if auto-focus is enabled
    if(FlagIsSet(DF_AUTOFOCUS)) FlagSet(DF_FOCUSED);
    // Check that cursor is in window
    double dX; double dY; cGlFW->WinGetCursorPos(dX, dY);
    cInput->FlagSetOrClear(IF_MOUSEFOCUS,
      dX >= 0.0 &&
      dY >= 0.0 &&
      dX < dcNew.DimGetWidth<double>() &&
      dY < dcNew.DimGetHeight<double>());
  }
  /* -- Return selected monitor ------------------------------------ */ public:
  const GlFWMonitor *DisplayGetSelectedMonitor() const { return gfwmActive; }
  /* -- Return selected resolution ----------------------------------------- */
  const GlFWRes *DisplayGetSelectedRes() const { return gfwrActive; }
  /* -- Request from alternative thread to resize window ------------------- */
  void DisplayRequestResize(const int iW, const int iH)
    { cEvtWin->AddUnblock(EWC_WIN_RESIZE, iW, iH); }
  /* -- Request from alternative thread to move window --------------------- */
  void DisplayRequestMove(const int iX, const int iY)
    { cEvtWin->AddUnblock(EWC_WIN_MOVE, iX, iY); }
  /* -- Request the window set a new cursor graphic ------------------------ */
  void DisplayRequestSetCursor(const GlFWCursorType gctType)
    { cEvtWin->AddUnblock(EWC_WIN_CURSET, gctType); }
  /* -- Request the window reset the cursor graphic ------------------------ */
  void DisplayRequestResetCursor() { cEvtWin->AddUnblock(EWC_WIN_CURRESET); }
  /* -- Request from alternative thread to centre the window --------------- */
  void DisplayRequestCentre() { cEvtWin->AddUnblock(EWC_WIN_CENTRE); }
  /* -- Request from alternative thread to reposition the window ----------- */
  void DisplayRequestReposition() { cEvtWin->AddUnblock(EWC_WIN_RESET); }
  /* -- Request to open window --------------------------------------------- */
  void DisplayRequestOpen() { cEvtWin->AddUnblock(EWC_WIN_SHOW); }
  /* -- Request to close window -------------------------------------------- */
  void DisplayRequestClose() { cEvtWin->AddUnblock(EWC_WIN_HIDE); }
  /* -- Request to minimise window ----------------------------------------- */
  void DisplayRequestMinimise() { cEvtWin->AddUnblock(EWC_WIN_MINIMISE); }
  /* -- Request to maximise window ----------------------------------------- */
  void DisplayRequestMaximise() { cEvtWin->AddUnblock(EWC_WIN_MAXIMISE); }
  /* -- Request to restore window ------------------------------------------ */
  void DisplayRequestRestore() { cEvtWin->AddUnblock(EWC_WIN_RESTORE); }
  /* -- Request to focus window -------------------------------------------- */
  void DisplayRequestFocus() { cEvtWin->AddUnblock(EWC_WIN_FOCUS); }
  /* -- Request for window attention --------------------------------------- */
  void DisplayRequestAttention() { cEvtWin->AddUnblock(EWC_WIN_ATTENTION); }
  /* -- Request from alternative thread to fullscreen toggle without save -- */
  void DisplayRequestFSToggle(const bool bState)
    { cEvtWin->AddUnblock(EWC_WIN_TOGGLEFS, bState); }
  /* -- Set full screen in Window thread ----------------------------------- */
  void DisplaySetFullScreen(const bool bState)
  {// Return if setting not different than actual
    if(FlagIsEqualToBool(DF_INFULLSCREEN, bState)) return;
    // If using Linux?
#if defined(LINUX)
    // Here appears to be yet another issue with GLFW on Linux. Changing back
    // to window mode from full-screen isn't working for some reason so I'm
    // just going to work around that by just quitting the thread and doing a
    // full reinitialisation until I can (ever?) figure out why this is
    // happening on Linux and not on MacOS or Windows.
    cEvtMain->RequestQuitThread();
    // Using Windows or MacOS?
#else
    // Update new fullscreen setting and reinitialise if successful
    DisplayReInitWindow(bState);
    // Update viewport
    DisplayRequestMatrixReInit();
#endif
  }
  /* -- Return current video mode refresh rate ----------------------------- */
  int DisplayGetRefreshRate() { return gfwrActive->Refresh(); }
  /* -- Get selected monitor id -------------------------------------------- */
  int DisplayGetMonitorId() const { return gfwmActive->Index(); }
  /* -- Get selected video mode id ----------------------------------------- */
  int DisplayGetVideoModeId() const { return gfwrActive->Index(); }
  /* -- Init info ---------------------------------------------------------- */
  const string &DisplayGetMonitorName() const { return gfwmActive->Name(); }
  /* -- Commit current matrix size ----------------------------------------- */
  void DisplayCommitMatrix(const bool bForce=true) const
  { // Set the default matrix from the configuration and if it was changed
    // also update the consoles FBO too.
    if(cFboCore->FboCoreAutoMatrix(
         dfMatrix.DimGetWidth(), dfMatrix.DimGetHeight(), bForce))
      cConGfx->ConGfxInitFBO();
    // Else redraw the console if enabled
    else cConsole->SetRedrawIfEnabled();
  }
  /* -- Restore default matrix --------------------------------------------- */
  void DisplayCommitDefaultMatrix()
  { // Restore default dimensions as set from the manifest
    dfMatrix.DimSet(dfMatrixReq);
    // Restore matrix but don't need to reinit if size didn't change
    DisplayCommitMatrix(false);
  }
  /* -- Alter default matrix ----------------------------------------------- */
  bool DisplayAlterDefaultMatrix(const GLfloat fNewWidth,
    const GLfloat fNewHeight)
  { // If width changed?
    if(StdIsFloatNotEqual(fNewWidth, dfMatrix.DimGetWidth()))
    { // Update width and if height changed? Update the height
      dfMatrix.DimSetWidth(fNewWidth);
      if(StdIsFloatNotEqual(fNewHeight, dfMatrix.DimGetHeight()))
        dfMatrix.DimSetHeight(fNewHeight);
    } // If height changed? Update the height and fall through to reinit
    else if(StdIsFloatNotEqual(fNewHeight, dfMatrix.DimGetHeight()))
      dfMatrix.DimSetHeight(fNewHeight);
    // Not modified so don't change the FBO
    else return false;
    // Force reinitialise the matrix
    DisplayCommitMatrix(false);
    // Send event to addons that the matrix changed
    DisplayRequestMatrixReInit();
    // Success
    return true;
  }
  /* -- Update window icon ------------------------------------------------- */
  void DisplayUpdateIcons()
  { // This functionality throws a GLFW api error on MacOS so just NullOp it
#if !defined(MACOS)
    // If using interactive mode?
    if(cSystem->SysIsGraphicalMode())
    { // Ignore if no icons
      if(gfwivIcons.empty()) return;
      // Capture exceptions and ask GLFW to set the icon
      try { cGlFW->WinSetIcon(UtilIntOrMax<int>(gfwivIcons.size()),
              gfwivIcons.data()); }
      // Exception occured? GLFW can throw GLFW_PLATFORM_ERROR on Wayland which
      // is absolutely retarded as is not consistent with other platforms such
      // as MacOS which will silently succeed
      catch(const exception &eReason)
      { // Just log the error that occured
        cLog->LogWarningExSafe(
          "Display could not load $ icon files due to GlFW exception: $.",
          gfwivIcons.size(), eReason);
        // Done
        return;
      } // Report that we updated the icons
      cLog->LogInfoExSafe("Display updated $ windows icons." ,
        gfwivIcons.size());
      // Show details?
      if(cLog->LogHasLevel(LH_DEBUG))
        for(Image &imC : ivIcons)
        { // Get first icon and log data
          const ImageSlot &imsD = imC.GetSlotsConst().front();
          cLog->LogNLCDebugExSafe("- $x$x$: $.", imsD.DimGetWidth(),
            imsD.DimGetHeight(), imC.GetBitsPerPixel(), imC.IdentGet());
        }
    } // Using console mode
    else cSystem->UpdateIcons();
#endif
  }
  /* -- Set window icons --------------------------------------------------- */
  bool DisplaySetIcon(const string &strNames)
  { // Seperate icon names and if we got an icon name
    if(Token tIcons{ strNames, ":", 3 })
    { // If using interactive mode?
      if(cSystem->SysIsGraphicalMode())
      { // Clear images and icons
        gfwivIcons.clear();
        ivIcons.clear();
        // Create contiguous memory for glfw icon descriptors and icon data
        gfwivIcons.reserve(tIcons.size());
        ivIcons.reserve(tIcons.size());
        // Build icons
        for(string &strName : tIcons)
        { // Check filename and load icon and force to RGB 32BPP.
          DirVerifyFileNameIsValid(strName);
          const Image &imC = ivIcons.emplace_back(
            Image{ StdMove(strName), IL_REVERSE|IL_TORGB|IL_TO32BPP });
          const ImageSlot &imsD = imC.GetSlotsConst().front();
          gfwivIcons.push_back({ imsD.DimGetWidth<int>(),
            imsD.DimGetHeight<int>(), imsD.MemPtr<unsigned char>() });
        }
      } // Not in interactive mode?
      else
      { // Only Win32 terminal windows can change the icon
#if defined(WINDOWS)
        // Have two icons at least?
        if(tIcons.size() >= 2)
        { // Get string and set small icon from the last icon specified
          string &strFile = tIcons.back();
          const Image imC{ StdMove(strFile), IL_REVERSE|IL_TOBGR };
          const ImageSlot &imsD = imC.GetSlotsConst().front();
          cSystem->SetSmallIcon(imC.IdentGet(), imsD.DimGetWidth(),
            imsD.DimGetHeight(), imC.GetBitsPerPixel(), imsD);
        } // Set large icon from the first icon specified
        string &strFile = tIcons.front();
        const Image imC{ StdMove(strFile), IL_REVERSE|IL_TOBGR };
        const ImageSlot &imsD = imC.GetSlotsConst().front();
        cSystem->SetLargeIcon(imC.IdentGet(), imsD.DimGetWidth(),
          imsD.DimGetHeight(), imC.GetBitsPerPixel(), imsD);
       // Not using windows?
#else
       // Log that we cannot set icons
       cLog->LogDebugSafe("Display cannot set icons on this mode and system.");
       // Windows check?
#endif
      } // Success
      return true;
    } // Failure
    return false;
  }
  /* -- Update icons and refresh icon if succeeded ------------------------- */
  void DisplaySetIconFromLua(const string &strNames)
    { if(DisplaySetIcon(strNames)) return cEvtWin->Add(EWC_WIN_SETICON); }
  /* -- Get window full-screen type ---------------------------------------- */
  FSType DisplayGetFSType() const { return fsType; }
  const string_view &DisplayGetFSTypeString(const FSType fsT) const
    { return fstStrings.Get(fsT); }
  const string_view &DisplayGetFSTypeString() const
    { return DisplayGetFSTypeString(fsType); }
  /* -- Get window position ------------------------------------------------ */
  int DisplayGetWindowPosX() const { return ciPosition.CoordGetX(); }
  int DisplayGetWindowPosY() const { return ciPosition.CoordGetY(); }
  float DisplayGetWindowScaleWidth() const
    { return dfWinScale.DimGetWidth(); }
  float DisplayGetWindowScaleHeight() const
    { return dfWinScale.DimGetHeight(); }
  /* -- ReInit ------------------------------------------------------------- */
  void DisplayReInit()
  { // Log progress
    cLog->LogDebugSafe("Display class reinitialising...");
    // Cancel window closure
    cGlFW->WinSetClose(false);
    // Enumerate monitors and video modes
    DisplayEnumerateMonitorsAndVideoModes();
    // Reinitialise the window
    DisplayReInitWindow(FlagIsSet(DF_INFULLSCREEN));
    // Log progress
    cLog->LogInfoSafe("Display class reinitialised successfully.");
  }
  /* -- Init --------------------------------------------------------------- */
  void DisplayInit()
  { // Class initialised
    IHInitialise();
    // Log progress
    cLog->LogDebugSafe("Display class starting up...");
    // Enumerate monitors and video modes
    DisplayEnumerateMonitorsAndVideoModes();
    // Inform main FBO class of our transparency setting
    cFboCore->FboCoreGetMain().FboSetTransparency(FlagIsSet(DF_TRANSPARENT));
    // Set context settings
    cGlFW->GlFWSetAlphaBits(iFBDepthA);
    cGlFW->GlFWSetAuxBuffers(0);
    cGlFW->GlFWSetBlueBits(iFBDepthB);
    cGlFW->GlFWSetClientAPI(iApi);
    cGlFW->GlFWSetCoreProfile(iProfile);
    cGlFW->GlFWSetCtxMajor(iCtxMajor);
    cGlFW->GlFWSetCtxMinor(iCtxMinor);
    cGlFW->GlFWSetDebug(FlagIsSet(DF_DEBUG));
    cGlFW->GlFWSetDepthBits(0);   // 2D framebuffer
    cGlFW->GlFWSetDoubleBuffer(FlagIsSet(DF_DOUBLEBUFFER));
    cGlFW->GlFWSetForwardCompat(FlagIsSet(DF_FORWARD));
    cGlFW->GlFWSetGPUSwitching(FlagIsSet(DF_GASWITCH));
    cGlFW->GlFWSetGreenBits(iFBDepthG);
    cGlFW->GlFWSetMaximised(FlagIsSet(DF_MAXIMISED));
    cGlFW->GlFWSetMultisamples(iSamples);
    cGlFW->GlFWSetNoErrors(FlagIsSet(DF_NOERRORS));
    cGlFW->GlFWSetRedBits(iFBDepthR);
    cGlFW->GlFWSetRefreshRate(DisplayGetRefreshRate());
    cGlFW->GlFWSetRelease(iRelease);
#if !defined(LINUX)
    cGlFW->GlFWSetRetinaMode(hdpiSetting != HD_DISABLED);
#endif
    cGlFW->GlFWSetRobustness(iRobustness);
    cGlFW->GlFWSetScaleMonitor(true);
    cGlFW->GlFWSetSRGBCapable(FlagIsSet(DF_SRGB));
    cGlFW->GlFWSetStencilBits(0); // Not used
    cGlFW->GlFWSetStereo(FlagIsSet(DF_STEREO));
    cGlFW->GlFWSetTransparency(cFboCore->FboCoreGetMain().
      FboIsTransparencyEnabled());
    // Set Apple operating system only settings
    // Get window name and use it for frame and instance name. It's assumed
    // that 'cpTitle' won't be freed while using it these two times.
    const char*const cpTitle = cCVars->GetCStrInternal(APP_TITLE);
    cGlFW->GlFWSetFrameName(cpTitle);
    // Initialise basic window. We will modify it after due to limitations in
    // this particular function. For example, this can't set the refresh rate.
    cSystem->WindowInitialised(cGlFW->WinInit(cpTitle, nullptr));
    // Clear any lingering window events which is very important because
    // events from the last window may contain invalidated pointers and as long
    // as they don't reach the 'cEvtWin->Manage()' function we're fine.
    cEvtWin->Flush();
    // Re-adjust the window
    DisplayReInitWindow(FlagIsSet(DF_FULLSCREEN));
    // Set forced aspect ratio
    cGlFW->WinSetAspectRatio(cCVars->GetStrInternal(WIN_ASPECT));
    // Update icons if there are some loaded by the cvars callbacks
    DisplayUpdateIcons();
    // Set default gamma for selected monitor
    DisplayApplyGamma();
    // Log progress
    cLog->LogInfoSafe("Display class started successfully.");
  }
  /* -- DeInit ------------------------------------------------------------- */
  void DisplayDeInit()
  { // Ignore if class not initialised
    if(IHNotDeInitialise()) return;
    // Log progress
    cLog->LogDebugSafe("Display class deinitialising...");
    // Remove invalidated active flags
    FlagClear(DF_FOCUSED|DF_EXCLUSIVE|DF_INFULLSCREEN|DF_NATIVEFS);
    // Window type deinitialised
    fsType = FST_STANDBY;
    // Have window?
    if(cGlFW->WinIsAvailable())
    { // If we have monitor?
      if(gfwmActive)
      { // Restore gamma (this fails if theres no window).
        GlFWSetGamma(gfwmActive->Context(), 1.0);
        // Monitor no longer valid
        gfwmActive = nullptr;
      } // Tell system we destroyed the window
      cSystem->SetWindowDestroyed();
      // Actually destroy window
      cGlFW->WinDeInit();
      // Log progress
      cLog->LogDebugSafe("Display window handle and context destroyed.");
    } // Don't have window
    else
    { // Skipped removal of window
      cLog->LogDebugSafe(
        "Display window handle and context destruction skipped.");
      // Can't restore gamma without window
      gfwmActive = nullptr;
    } // Clear selected video mode
    gfwrActive = nullptr;
    // Log progress
    cLog->LogInfoSafe("Display class deinitialised successfully.");
  }
  /* -- Default constructor ------------------------------------- */ protected:
  Display() :
    /* --------------------------------------------------------------------- */
    InitHelper{ __FUNCTION__ },        // Send name to init helper
    DisplayFlags{ DF_NONE },           // No display flags set
    DimCoInt{ -1, -1, 0, 0 },          // Requested position and size
    EvtMainRegAuto{ cEvtMain, {        // Register main events
      { EMC_VID_FBREINIT,    bind(&Display::DisplayOnFBReset,    this,_1) },
      { EMC_VID_MATRIXREINIT,bind(&Display::DisplayOnMatrixReset,this,_1) },
      { EMC_WIN_CLOSE,       bind(&Display::DisplayOnClose,      this,_1) },
      { EMC_WIN_FOCUS,       bind(&Display::DisplayOnFocus,      this,_1) },
      { EMC_WIN_ICONIFY,     bind(&Display::DisplayOnIconify,    this,_1) },
      { EMC_WIN_MOVED,       bind(&Display::DisplayOnMoved,      this,_1) },
      { EMC_WIN_REFRESH,     bind(&Display::DisplayOnRefresh,    this,_1) },
      { EMC_WIN_RESIZED,     bind(&Display::DisplayOnResized,    this,_1) },
      { EMC_WIN_SCALE,       bind(&Display::DisplayOnScale,      this,_1) },
    } },
    EvtWinRegAuto{ cEvtWin, {          // Register window request events
      { EWC_WIN_ATTENTION,  bind(&Display::DisplayOnRqAttention,  this,_1) },
      { EWC_WIN_CENTRE,     bind(&Display::DisplayOnRqCentre,     this,_1) },
      { EWC_WIN_CURPOSGET,  bind(&Display::DisplayOnRqGetCurPos,  this,_1) },
      { EWC_WIN_CURPOSSET,  bind(&Display::DisplayOnRqSetCurPos,  this,_1) },
      { EWC_WIN_CURRESET,   bind(&Display::DisplayOnRqResetCurImg,this,_1) },
      { EWC_WIN_CURSET,     bind(&Display::DisplayOnRqSetCurImg,  this,_1) },
      { EWC_WIN_CURSETVIS,  bind(&Display::DisplayOnRqSetCurVis,  this,_1) },
      { EWC_WIN_FOCUS,      bind(&Display::DisplayOnRqFocus,      this,_1) },
      { EWC_WIN_HIDE,       bind(&Display::DisplayOnRqHide,       this,_1) },
      { EWC_WIN_LIMITS,     bind(&Display::DisplayOnRqSetLimits,  this,_1) },
      { EWC_WIN_MAXIMISE,   bind(&Display::DisplayOnRqMaximise,   this,_1) },
      { EWC_WIN_MINIMISE,   bind(&Display::DisplayOnRqMinimise,   this,_1) },
      { EWC_WIN_MONITOR,    bind(&Display::DisplayOnRqMonitor,    this,_1) },
      { EWC_WIN_MONITORS,   bind(&Display::DisplayOnRqMonitors,   this,_1) },
      { EWC_WIN_MOVE,       bind(&Display::DisplayOnRqMove,       this,_1) },
      { EWC_WIN_RESET,      bind(&Display::DisplayOnRqReset,      this,_1) },
      { EWC_WIN_RESIZE,     bind(&Display::DisplayOnRqResize,     this,_1) },
      { EWC_WIN_RESTORE,    bind(&Display::DisplayOnRqRestore,    this,_1) },
      { EWC_WIN_SETICON,    bind(&Display::DisplayOnRqSetIcons,   this,_1) },
      { EWC_WIN_SETLKMODS,  bind(&Display::DisplayOnRqSetLKMods,  this,_1) },
      { EWC_WIN_SETRAWMOUSE,bind(&Display::DisplayOnRqSetRawMouse,this,_1) },
      { EWC_WIN_SETSTKKEYS, bind(&Display::DisplayOnRqStickyKeys, this,_1) },
      { EWC_WIN_SETSTKMOUSE,bind(&Display::DisplayOnRqStickyMouse,this,_1) },
      { EWC_WIN_SHOW,       bind(&Display::DisplayOnRqShow,       this,_1) },
      { EWC_WIN_TOGGLEFS,   bind(&Display::DisplayOnRqToggleFS,   this,_1) },
    } },
    gfwmActive(nullptr),               // No monitor selected
    gfwrActive(nullptr),               // No video mode selected
    stMRequested(StdMaxSizeT),         // No monitor requested
    stVRequested(StdMaxSizeT),         // No video mode id requested
    abUnsuspend{ false },              // Not unsuspending engine thread
    dfWinScale{ 1.0f, 1.0f },          // Window scale initialised later
    dfLastScale{ 0.0f, 0.0f },         // Window scale reduction
    fGamma(0),                         // Gamma initialised by CVars
    ciPosition{ GLFW_DONT_CARE },      // Window position
    hdpiSetting(HD_DISABLED),          // HiDPI setting initialised by CVars
    iApi(GLFW_DONT_CARE),              // Api type set by cvars
    iProfile(GLFW_DONT_CARE),          // Profile type set by cvars
    iCtxMajor(GLFW_DONT_CARE),         // Context major version set by cvars
    iCtxMinor(GLFW_DONT_CARE),         // Context major version set by cvars
    iRobustness(GLFW_DONT_CARE),       // Context robustness set by cvars
    iRelease(GLFW_DONT_CARE),          // Context release behaviour set by cvar
    iFBDepthR(GLFW_DONT_CARE),         // Bit depth not selected yet
    iFBDepthG(GLFW_DONT_CARE),         // Bit depth not selected yet
    iFBDepthB(GLFW_DONT_CARE),         // Bit depth not selected yet
    iFBDepthA(GLFW_DONT_CARE),         // Bit depth not selected yet
    iSamples(GLFW_DONT_CARE),          // No anti-aliasing samples specified
    lrFocused{ "OnFocused" },          // Set name for OnFocused lua event
    fsType(FST_STANDBY),               // Full-screen type
    fstStrings{{                       // DisplayInit full-screen type strings
      STR(FST_STANDBY),    STR(FST_WINDOW), STR(FST_EXCLUSIVE),
      STR(FST_BORDERLESS), STR(FST_NATIVE)
    }, "FST_UNKNOWN"}                  // End of full-screen type strings list
    /* -- Set global pointer to static class ------------------------------- */
    { cDisplay = this; }
  /* -- Destructor --------------------------------------------------------- */
  DTORHELPER(~Display, DisplayDeInit())
  /* -- Helper macro for boolean based CVars based on OS ----------- */ public:
#define CBCVARFLAG(n, f) CVarReturn n(const bool bState) \
    { FlagSetOrClear(f, bState); return ACCEPT; }
#define CBCVARFORCEFLAG(n, f, v) CVarReturn n(const bool) \
    { FlagSetOrClear(f, v); return ACCEPT; }
#define CBCVARRANGE(t,n,d,l,g) CVarReturn n(const t tParam) \
    { return CVarSimpleSetIntNLG(d, tParam, l, g); }
  /* -- Create boolean based cvar callbacks -------------------------------- */
  CBCVARFLAG(DisplayAutoFocusChanged, DF_AUTOFOCUS)
  CBCVARFLAG(DisplayAutoIconifyChanged, DF_AUTOICONIFY)
  CBCVARFLAG(DisplayBorderChanged, DF_BORDER)
  CBCVARFLAG(DisplayCloseableChanged, DF_CLOSEABLE)
  CBCVARFLAG(DisplayDoubleBufferChanged, DF_DOUBLEBUFFER)
  CBCVARFLAG(DisplayFloatingChanged, DF_FLOATING)
  CBCVARFLAG(DisplayFullScreenStateChanged, DF_FULLSCREEN)
  CBCVARFLAG(DisplayMinFocusChanged, DF_MINFOCUS)
  CBCVARFLAG(DisplaySRGBColourSpaceChanged, DF_SRGB)
  CBCVARFLAG(DisplaySetMaximisedMode, DF_MAXIMISED)
  CBCVARFLAG(DisplaySetWindowTransparency, DF_TRANSPARENT)
  CBCVARFLAG(DisplaySizableChanged, DF_SIZABLE)
  CBCVARRANGE(int, DisplayCtxMajorChanged, iCtxMajor, GLFW_DONT_CARE, 4)
  CBCVARRANGE(int, DisplayCtxMinorChanged, iCtxMinor, GLFW_DONT_CARE, 6)
  CBCVARRANGE(int, DisplayFsaaChanged, iSamples, GLFW_DONT_CARE, 8)
  CBCVARRANGE(int, DisplaySetForcedBitDepthA, iFBDepthA, GLFW_DONT_CARE, 16)
  CBCVARRANGE(int, DisplaySetForcedBitDepthB, iFBDepthB, GLFW_DONT_CARE, 16)
  CBCVARRANGE(int, DisplaySetForcedBitDepthG, iFBDepthG, GLFW_DONT_CARE, 16)
  CBCVARRANGE(int, DisplaySetForcedBitDepthR, iFBDepthR, GLFW_DONT_CARE, 16)
  CBCVARRANGE(GLfloat, DisplaySetMatrixHeight, dfMatrixReq.DimGetHeightRef(),
    200.0f, 16384.0f)
  CBCVARRANGE(GLfloat, DisplaySetMatrixWidth, dfMatrixReq.DimGetWidthRef(),
    320.0f, 16384.0f)
  CBCVARRANGE(HiDPISetting, DisplayHiDPIChanged, hdpiSetting,
    HD_DISABLED, HD_ENHANCED)
  /* ----------------------------------------------------------------------- */
#if defined(MACOS)                     // Compiling on MacOS?
  /* ----------------------------------------------------------------------- */
  CBCVARFORCEFLAG(DisplayForwardChanged, DF_FORWARD, true)
  CBCVARFLAG(DisplayGraphicsSwitchingChanged, DF_GASWITCH)
  CBCVARFORCEFLAG(DisplaySetGLDebugMode, DF_DEBUG, false)
  CBCVARFORCEFLAG(DisplaySetNoErrorsMode, DF_NOERRORS, false)
  CBCVARFORCEFLAG(DisplaySetStereoMode, DF_STEREO, false)
  /* ----------------------------------------------------------------------- */
#else                                  // Windows or Linux?
  /* ----------------------------------------------------------------------- */
  CBCVARFLAG(DisplayForwardChanged, DF_FORWARD)
  CBCVARFORCEFLAG(DisplayGraphicsSwitchingChanged, DF_GASWITCH, false)
  CBCVARFLAG(DisplaySetGLDebugMode, DF_DEBUG)
  CBCVARFLAG(DisplaySetNoErrorsMode, DF_NOERRORS)
  CBCVARFLAG(DisplaySetStereoMode, DF_STEREO)
  /* ----------------------------------------------------------------------- */
#endif                                 // End of OS conditions check
  /* -- Done with these macros --------------------------------------------- */
#undef CBCVARRANGE
#undef CBCVARFORCEFLAG
#undef CBCVARFLAG
  /* -- Set window width and height ---------------------------------------- */
  CVarReturn DisplayHeightChanged(const int iHeight)
    { DimSetHeight(iHeight); return ACCEPT; }
  CVarReturn DisplayWidthChanged(const int iWidth)
    { DimSetWidth(iWidth); return ACCEPT; }
  /* -- Set robustness ----------------------------------------------------- */
  CVarReturn DisplayRobustnessChanged(const size_t stIndex)
  { // Not supported on MacOS
#if defined(MACOS)
    static_cast<void>(stIndex); iRobustness = GLFW_NO_ROBUSTNESS;
#else
    // Possible values
    static const array<const int,3> aValues{ GLFW_NO_RESET_NOTIFICATION,
      GLFW_LOSE_CONTEXT_ON_RESET, GLFW_NO_ROBUSTNESS };
    // Fail if invalid
    if(stIndex >= aValues.size()) return DENY;
    // Set the api
    iRobustness = aValues[stIndex];
#endif
    // Success
    return ACCEPT;
  }
  /* -- Set release behaviour ---------------------------------------------- */
  CVarReturn DisplayReleaseChanged(const size_t stIndex)
  { // Not supported on MacOS
#if defined(MACOS)
    static_cast<void>(stIndex); iRelease = GLFW_RELEASE_BEHAVIOR_NONE;
#else
    // Possible values
    static const array<const int,3> aValues{ GLFW_ANY_RELEASE_BEHAVIOR,
      GLFW_RELEASE_BEHAVIOR_FLUSH, GLFW_RELEASE_BEHAVIOR_NONE };
    // Fail if invalid
    if(stIndex >= aValues.size()) return DENY;
    // Set the api
    iRelease = aValues[stIndex];
#endif
    // Success
    return ACCEPT;
  }
  /* -- Set api ------------------------------------------------------------ */
  CVarReturn DisplayApiChanged(const size_t stIndex)
  { // Possible values
    static const array<const int,3> aValues
      { GLFW_OPENGL_API, GLFW_OPENGL_ES_API, GLFW_NO_API };
    // Fail if invalid
    if(stIndex >= aValues.size()) return DENY;
    // Set the api
    iApi = aValues[stIndex];
    // Success
    return ACCEPT;
  }
  /* -- Set profile -------------------------------------------------------- */
  CVarReturn DisplayProfileChanged(const size_t stIndex)
  { // Only core profile supported on MacOS
#if defined(MACOS)
    static_cast<void>(stIndex); iProfile = GLFW_OPENGL_CORE_PROFILE;
#else
    // Possible values
    static const array<const int,3> aValues{ GLFW_OPENGL_CORE_PROFILE,
      GLFW_OPENGL_COMPAT_PROFILE, GLFW_OPENGL_ANY_PROFILE };
    // Fail if invalid
    if(stIndex >= aValues.size()) return DENY;
    // Set the api
    iProfile = aValues[stIndex];
#endif
    // Success
    return ACCEPT;
  }
  /* -- Set full-screen video mode cvar ------------------------------------ */
  CVarReturn DisplayFullScreenModeChanged(const int iVId)
  { // Return if invalid full-screen mode
    if(iVId < -2) return DENY;
    // Set video mode requested
    stVRequested = static_cast<size_t>(iVId);
    // Accepted
    return ACCEPT;
  }
  /* -- Set monitor number ------------------------------------------------- */
  CVarReturn DisplayMonitorChanged(const int iMId)
  { // Return if invalid full-screen mode
    if(iMId < -1) return DENY;
    // Set video mode requested
    stMRequested = static_cast<size_t>(iMId);
    // Accepted
    return ACCEPT;
  }
  /* -- Set window X position ---------------------------------------------- */
  CVarReturn DisplaySetXPosition(const int iNewX)
  { // Deny change request if an invalid value was sent
    CoordSetX(iNewX);
    // Apply window position if window is available
    if(cGlFW && cGlFW->WinIsAvailable()) DisplayRequestReposition();
    // Success
    return ACCEPT;
  }
  /* -- Set window Y position ---------------------------------------------- */
  CVarReturn DisplaySetYPosition(const int iNewY)
  { // Deny change request if an invalid value was sent
    CoordSetY(iNewY);
    // Apply window position if window is available
    if(cGlFW && cGlFW->WinIsAvailable()) DisplayRequestReposition();
    // Success
    return ACCEPT;
  }
  /* -- Set gamma ---------------------------------------------------------- */
  CVarReturn DisplayGammaChanged(const GLfloat fNewGamma)
  { // Deny change request if an invalid gamma value was sent
    if(!CVarToBoolReturn(CVarSimpleSetIntNLG(fGamma, fNewGamma, 0.25f, 4.00f)))
      return DENY;
    // Apply new gamma setting if window is available
    if(cGlFW && cGlFW->WinIsAvailable() && gfwmActive->Context())
      DisplayApplyGamma();
    // Success
    return ACCEPT;
  }
  /* -- Icon filenames changed (allow blank strings) ----------------------- */
  CVarReturn DisplaySetIcon(const string &strF, string&)
    { return BoolToCVarReturn(strF.empty() || DisplaySetIcon(strF)); }
};/* ----------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
