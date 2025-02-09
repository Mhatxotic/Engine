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
using namespace ICollector::P;         using namespace IConsole::P;
using namespace ICVar::P;              using namespace ICVarDef::P;
using namespace ICVarLib::P;           using namespace IDim;
using namespace IDir::P;               using namespace IEvtMain::P;
using namespace IEvtWin::P;            using namespace IFboCore::P;
using namespace IFlags;                using namespace IFont::P;
using namespace IGlFW::P;              using namespace IGlFWCursor::P;
using namespace IGlFWMonitor::P;       using namespace IGlFWUtil::P;
using namespace IIdent::P;             using namespace IImage::P;
using namespace IImageDef::P;          using namespace IInput::P;
using namespace ILog::P;               using namespace ILuaFunc::P;
using namespace IStd::P;               using namespace IString::P;
using namespace ISystem::P;            using namespace ISysUtil::P;
using namespace ITexture::P;           using namespace IToken::P;
using namespace IUtf;                  using namespace IUtil::P;
using namespace Lib::OS::GlFW;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* ------------------------------------------------------------------------- */
BUILD_FLAGS(Display,
  /* -- Active flags ------------------------------------------------------- */
  // No display flags                  Window is focused?
  DF_NONE                   {Flag[0]}, DF_FOCUSED                {Flag[1]},
  // Exclusive mode full-screen?       Full-screen locked?
  DF_EXCLUSIVE              {Flag[2]}, DF_NATIVEFS               {Flag[3]},
  // Window is actually in fullscreen?
  DF_INFULLSCREEN           {Flag[4]},
  /* -- End-user configuration flags --------------------------------------- */
  // Use forward compatible context?   Use double-buffering?
  DF_FORWARD               {Flag[47]}, DF_DOUBLEBUFFER          {Flag[48]},
  // Automatic minimise?               Focus on show?
  DF_AUTOICONIFY           {Flag[49]}, DF_AUTOFOCUS             {Flag[50]},
  // Window is resizable?              Always on top?
  DF_SIZABLE               {Flag[51]}, DF_FLOATING              {Flag[52]},
  // Window has a border?              Minimize on lose focus?
  DF_BORDER                {Flag[53]}, DF_MINFOCUS              {Flag[54]},
  // HiDPI is enabled?                 SRGB namespace is enabled?
  DF_HIDPI                 {Flag[55]}, DF_SRGB                  {Flag[56]},
  // Graphics switching enabled?       Full-screen mode set?
  DF_GASWITCH              {Flag[57]}, DF_FULLSCREEN            {Flag[58]},
  // Window is closable?               Stereo mode enabled?
  DF_CLOSEABLE             {Flag[59]}, DF_STEREO                {Flag[60]},
  // OpenGL debug context?             Window transparency enabled?
  DF_DEBUG                 {Flag[61]}, DF_TRANSPARENT           {Flag[62]},
  // No opengl errors?                 Window maximised at start?
  DF_NOERRORS              {Flag[63]}, DF_MAXIMISED             {Flag[64]}
);
/* == Display class ======================================================== */
static class Display final :
  /* -- Base classes ------------------------------------------------------- */
  private IHelper,                     // Initialisation helper
  public  DisplayFlags,                // Display settings
  private EvtMain::RegVec,             // Main events list to register
  private EvtWin::RegVec,              // Window events list to register
  private DimCoInt                     // Requested window position and size
{ /* -- Variables ---------------------------------------------------------- */
  GlFWMonitors     mlData;             // Monitor list data
  const GlFWMonitor *moSelected;       // Monitor selected
  const GlFWRes   *rSelected;          // Monitor resolution selected
  size_t           stMRequested,       // Monitor id request
                   stVRequested;       // Video mode requested
  GLfloat          fGamma,             // Monitor gamma setting
                   fMatrixWidth,       // Saved matrix width
                   fMatrixHeight;      // Saved matrix height
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
                   iWinPosX, iWinPosY, // Window position
                   iAuxBuffers,        // Auxilliary buffers to use
                   iSamples;           // FSAA setting to use
  float            fWinScaleWidth,     // Window scale width
                   fWinScaleHeight;    // Window scale height
  string           strClipboard;       // Clipboard string to copy or grab
  /* -- Icons -------------------------------------------------------------- */
  typedef vector<GLFWimage> GlFWIconList; // Glfw image list
  GlFWIconList     gilIcons;           // Contiguous memory for glfw image data
  typedef vector<Image> IconList;      // Actual icon list
  IconList         ilIcons;            // Image icon data
  /* -- Macros ----------------------------------------------------- */ public:
  LuaFunc          lrFocused,          // Window focused lua event
                   lrClipboard;        // Clipboard function callback
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
  void CheckWindowMoved(const int iNewX, const int iNewY)
  { // If position not changed? Report event and return
    if(iWinPosX == iNewX && iWinPosY == iNewY)
      return cLog->LogDebugExSafe("Display received window position of $x$.",
        iNewX, iNewY);
    // Report change
    cLog->LogInfoExSafe("Display changed window position from $x$ to $x$.",
      iWinPosX, iWinPosY, iNewX, iNewY);
    // Update position
    iWinPosX = iNewX;
    iWinPosY = iNewY;
  }
  /* -- Window moved request ----------------------------------------------- */
  void OnMoved(const EvtMainEvent &emeEvent)
  { // Get reference to actual arguments vector
    const EvtMainArgs &emaArgs = emeEvent.aArgs;
    // Check to see if the window moved
    CheckWindowMoved(emaArgs[1].i, emaArgs[2].i);
  }
  /* -- Window set icon request -------------------------------------------- */
  void OnReqSetIcons(const EvtWinEvent&) { UpdateIcons(); }
  /* -- Window set set cursor visibility ----------------------------------- */
  void OnReqSetCurVisib(const EvtWinEvent &eweEvent)
  { // Get requested state
    const bool bState = eweEvent.aArgs.front().b;
    // Set the new input if we can and log status
    cGlFW->WinSetCursor(bState);
    cLog->LogDebugExSafe("Input updated cursor visibility status to $.",
      StrFromBoolTF(bState));
  }
  /* -- Window set raw mouse request --------------------------------------- */
  void OnReqSetRawMouse(const EvtWinEvent &eweEvent)
  { // If raw mouse support is supported?
    if(cGlFW->IsNotRawMouseMotionSupported()) return;
    // Set the new input if we can and log status
    cGlFW->WinSetRawMouseMotion(eweEvent.aArgs.front().b);
    cLog->LogDebugExSafe("Input updated raw mouse status to $.",
      StrFromBoolTF(cGlFW->WinGetRawMouseMotion()));
  }
  /* -- On request window attenti on event --------------------------------- */
  void OnReqAttention(const EvtWinEvent&) { cGlFW->WinRequestAttention(); }
  /* -- On request window focus event -------------------------------------- */
  void OnReqFocus(const EvtWinEvent&) { cGlFW->WinFocus(); }
  /* -- On request window maximise event ----------------------------------- */
  void OnReqMaximise(const EvtWinEvent&) { cGlFW->WinMaximise(); }
  /* -- On request window minimise event ----------------------------------- */
  void OnReqMinimise(const EvtWinEvent&) { cGlFW->WinMinimise(); }
  /* -- On request window restore event ------------------------------------ */
  void OnReqRestore(const EvtWinEvent&) { cGlFW->WinRestore(); }
  /* -- Window set sticky keys request ------------------------------------- */
  void OnReqStickyKeys(const EvtWinEvent &eweEvent)
  { // Set the new input if we can and log status
    cGlFW->WinSetStickyKeys(eweEvent.aArgs.front().b);
    cLog->LogDebugExSafe("Input updated sticky keys status to $.",
      StrFromBoolTF(cGlFW->WinGetStickyKeys()));
  }
  /* -- Window set sticky mouse request ------------------------------------ */
  void OnReqStickyMouse(const EvtWinEvent &eweEvent)
  { // Set the new input if we can and log status
    cGlFW->WinSetStickyMouseButtons(eweEvent.aArgs.front().b);
    cLog->LogDebugExSafe("Input updated sticky mouse status to $.",
      StrFromBoolTF(cGlFW->WinGetStickyMouseButtons()));
  }
  /* -- Window was asked to be hidden -------------------------------------- */
  void OnReqHide(const EvtWinEvent&) { cGlFW->WinHide(); }
  /* -- Resend mouse position ---------------------------------------------- */
  void OnReqGetCursorPos(const EvtWinEvent&)
    { cGlFW->WinSendMousePosition(); }
  /* -- Set mouse position ------------------------------------------------- */
  void OnReqSetCursorPos(const EvtWinEvent &eweEvent)
    { cGlFW->WinSetCursorPos(eweEvent.aArgs.front().d,
                             eweEvent.aArgs.back().d); }
  /* -- Window cursor request ---------------------------------------------- */
  void OnReqSetCursor(const EvtWinEvent &eweEvent)
    { cGlFW->SetCursor(static_cast<GlFWCursorType>
        (eweEvent.aArgs.front().z)); }
  /* -- Window reset cursor request ---------------------------------------- */
  void OnReqResetCursor(const EvtWinEvent&)
    { cGlFW->WinSetCursorGraphic(); }
  /* -- Window scale change request ---------------------------------------- */
  void OnScale(const EvtMainEvent &emeEvent)
  { // Get reference to actual arguments vector
    const EvtMainArgs &emaArgs = emeEvent.aArgs;
    // Get new values
    const float fNewWidth = emaArgs[1].f,
                fNewHeight = emaArgs[2].f;
    // If scale not changed? Report event and return
    if(UtilIsFloatEqual(fNewWidth, fWinScaleWidth) &&
       UtilIsFloatEqual(fNewHeight, fWinScaleHeight))
      return cLog->LogDebugExSafe("Display received window scale of $x$.",
        fNewWidth, fNewHeight);
    // Report change
    cLog->LogInfoExSafe("Display changed window scale from $x$ to $x$.",
      fWinScaleWidth, fWinScaleHeight, fNewWidth, fNewHeight);
    // Set new value
    fWinScaleWidth = fNewWidth;
    fWinScaleHeight = fNewHeight;
  }
  /* -- Window limits change request --------------------------------------- */
  void OnReqSetLimits(const EvtWinEvent &eweEvent)
  { // Get reference to actual arguments vector
    const EvtWinArgs &ewaArgs = eweEvent.aArgs;
    // Get the new limits
    const int iMinW = ewaArgs[0].i, iMinH = ewaArgs[1].i,
              iMaxW = ewaArgs[2].i, iMaxH = ewaArgs[3].i;
    // Set the new limits
    cGlFW->WinSetLimits(iMinW, iMinH, iMaxW, iMaxH);
  }
  /* -- Window focused ----------------------------------------------------- */
  void OnFocus(const EvtMainEvent &emeEvent)
  { // Get state and check it
    const int iState = emeEvent.aArgs[1].i;
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
  void OnRefresh(const EvtMainEvent&)
  { // Report that the window was resized
    cLog->LogDebugSafe("Display redrawing window contents.");
    // Set to force redraw the next frame
    cFboCore->SetDraw();
  }
  /* == Check if window resized ============================================ */
  void CheckWindowResized(const int iWidth, const int iHeight) const
  { // If position not changed? Report event and return
    if(cInput->GetWindowWidth() == iWidth &&
       cInput->GetWindowHeight() == iHeight)
      return cLog->LogDebugExSafe("Display received window size of $x$.",
        iWidth, iHeight);
    // Report change
    cLog->LogInfoExSafe("Display changed window size from $x$ to $x$.",
      cInput->GetWindowWidth(), cInput->GetWindowHeight(), iWidth, iHeight);
    // Update position
    cInput->SetWindowSize(iWidth, iHeight);
  }
  /* -- On window resized callback ----------------------------------------- */
  void OnResized(const EvtMainEvent &emeEvent)
  { // Get reference to actual arguments vector
    const EvtMainArgs &emaArgs = emeEvent.aArgs;
    // Check if the window resized
    CheckWindowResized(emaArgs[1].i, emaArgs[2].i);
  }
  /* -- On window closed callback ------------------------------------------ */
  void OnClose(const EvtMainEvent&)
  { // If window is not closable then ignore the event
    if(FlagIsClear(DF_CLOSEABLE)) return;
    // Send quit event
    cEvtMain->RequestQuit();
  }
  /* -- Window iconified --------------------------------------------------- */
  void OnIconify(const EvtMainEvent &emeEvent)
  { // Get state and check it
    switch(const int iState = emeEvent.aArgs[1].i)
    { // Minimized? Log that we minimised and return
      case GLFW_TRUE:
        return cLog->LogDebugSafe("Display window state minimised.");
      // Restored? Redraw console at least and log event
      case GLFW_FALSE:
        cFboCore->SetDraw();
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
  void EnumerateMonitorsAndVideoModes(void)
  { // Log progress
    cLog->LogDebugSafe("Display now enumerating available displays...");
    // Refresh monitors data
    mlData.Refresh();
    // Get primary monitor information, throw exception if invalid
    for(const GlFWMonitor &mData : mlData)
    { // Write information about the monitor
      cLog->LogDebugExSafe(
        "- Monitor $: $.\n"            "- Mode count: $.\n"
        "- Position: $x$.\n"           "- Dimensions: $$$x$\" ($x$mm).\n"
        "- Size: $\" ($mm).",
        mData.Index(), mData.Name(), mData.size(), mData.CoordGetX(),
        mData.CoordGetY(), fixed, setprecision(1), mData.WidthInch(),
        mData.HeightInch(), mData.DimGetWidth(), mData.DimGetHeight(),
        mData.DiagonalInch(), mData.Diagonal());
      // Log resolution data
      for(const GlFWRes &rData : mData)
        cLog->LogDebugExSafe("-- Mode $: $x$x$<RGB$$$> @ $Hz$.",
          rData.Index(), rData.Width(), rData.Height(), rData.Depth(),
          rData.Red(), rData.Green(), rData.Blue(), rData.Refresh(),
          &rData == mData.Primary() ? " (Active)" : cCommon->Blank());
    } // Custom monitor selected (-2) and valid monitor? Set it
    static constexpr size_t stM2 = StdMaxSizeT - 1;
    moSelected = (stMRequested < stM2 && stMRequested < mlData.size()) ?
      &mlData[stMRequested] : mlData.Primary();
    // Get selected screen resolution
    rSelected = (stVRequested < stM2 && stVRequested < (*moSelected).size()) ?
      &(*moSelected)[stVRequested] : moSelected->Primary();
    // Set exclusive full-screen mode flag if value is not -2
    FlagSetOrClear(DF_EXCLUSIVE, stVRequested != stM2);
    // Get primary monitor and mode
    const GlFWMonitor &moPrimary = *mlData.Primary();
    const GlFWRes &rPrimary = *moPrimary.Primary();
    // Write to log the monitor we are using
    cLog->LogInfoExSafe(
      "Display finished enumerating $ displays...\n"
      "- Primary monitor $: $ @$x$ ($$$\"x$\"=$\").\n"
      "- Primary mode $: $x$x$bpp(R$G$B$) @$hz.\n"
      "- Selected monitor $: $ @$x$ ($$$\"x$\"=$\").\n"
      "- Selected mode $: $x$x$bpp(R$G$B$) @$hz.",
      mlData.size(),
      moPrimary.Index(), moPrimary.Name(), moPrimary.CoordGetX(),
        moPrimary.CoordGetY(), fixed, setprecision(1),
        moPrimary.WidthInch(), moPrimary.HeightInch(),
        moPrimary.DiagonalInch(),
      rPrimary.Index(), rPrimary.Width(), rPrimary.Height(),
        rPrimary.Depth(), rPrimary.Red(), rPrimary.Green(),
        rPrimary.Blue(), rPrimary.Refresh(),
      moSelected->Index(), moSelected->Name(), moSelected->CoordGetX(),
        moSelected->CoordGetY(), fixed, setprecision(1),
        moSelected->WidthInch(), moSelected->HeightInch(),
        moSelected->DiagonalInch(),
      rSelected->Index(), rSelected->Width(), rSelected->Height(),
        rSelected->Depth(), rSelected->Red(), rSelected->Green(),
        rSelected->Blue(), rSelected->Refresh());
  }
  /* -- Monitor changed ---------------------------------------------------- */
  static void OnMonitorStatic(GLFWmonitor*const, const int);
  void OnMonitor(GLFWmonitor*const mAffected, const int iAction)
  { // We're in the main window thread here so we cannot modify any of the
    // monitor state as the engine thread could read from it at any time. All
    // we can do for now is ask the engine thread to restart.
    switch(iAction)
    { // Device was connected?
      case GLFW_CONNECTED:
        // Ignore if null context
        if(!mAffected)
          cLog->LogWarningSafe(
            "Display connected monitor event with null context!");
        // We shouldn't already have this monitor in our structured list
        else if(const GlFWMonitor*const moAffected = mlData.Find(mAffected))
          cLog->LogWarningExSafe(
            "Display already connected monitor '$'...", moAffected->Name());
        // Anything else?
        else
        { // Try to get monitor name and if found? Log the name.
          const char*const cpName = GlFWGetMonitorName(mAffected);
          if(UtfIsCStringValid(cpName))
            cLog->LogInfoExSafe(
              "Display detected new monitor '$', re-initialising...",
                cpName);
          // Could not get name of monitor
          else cLog->LogInfoSafe(
            "Display detected a new monitor, re-initialising...");
          // We need to re-initialise the opengl context
          cEvtMain->RequestQuitThread();
        } // Done
        break;
      // Device was disconnected?
      case GLFW_DISCONNECTED:
        // Ignore if null context
        if(!mAffected)
          cLog->LogWarningSafe(
            "Display disconnected monitor event with null context!");
        // We should have this monitor in our structured list
        else if(const GlFWMonitor*const moAffected = mlData.Find(mAffected))
        { // If this was our monitor?
          if(moAffected == moSelected)
          { // We recognise it so we can savely disconnect it
            cLog->LogInfoExSafe(
              "Display disconnected monitor '$', re-initialising...",
              moAffected->Name());
            // The selected device is no longer valid so make sure it is
            // cleared so DeInit() doesn't try to restore gamma and crash the
            // whole engine with an exception.
            moSelected = nullptr;
            // We need to re-initialise the opengl context
            cEvtMain->RequestQuitThread();
          } // This is not our monitor?
          else
          { // Try to get monitor name and if found? Log the name.
            const char*const cpName = GlFWGetMonitorName(mAffected);
            if(UtfIsCStringValid(cpName))
              cLog->LogInfoExSafe(
                "Display disconnected monitor '$', re-initialising...",
                  cpName);
            // Monitor name not available
            else cLog->LogInfoSafe(
              "Display disconnected a monitor, re-initialising...");
            // We need to re-initialise the opengl context
            cEvtMain->RequestQuitThread();
          }
        } // We don't have it so ignore it
        else cLog->LogWarningExSafe(
          "Display already disconnected monitor '$'...",
          GlFWGetMonitorName(mAffected));
        // Break to return
        break;
      // Unknown state?
      default:
        // Ignore if null context
        if(!mAffected)
          cLog->LogWarningExSafe(
            "Display sent bad state of 0x$$ with null context!",
            hex, iAction);
        // Not a null context?
        else
        { // Find this monitor context, log the failure and ignore event
          const GlFWMonitor*const moAffected = mlData.Find(mAffected);
          cLog->LogWarningExSafe(
            "Display sent bad state of 0x$$ for monitor '$'!",
            hex, iAction,
            moAffected ? moAffected->Name() : GlFWGetMonitorName(mAffected));
        } // Break to return
        break;
    }
  }
  /* == Matrix reset requested ============================================= */
  void OnMatrixReset(const EvtMainEvent&) { ReInitMatrix(); }
  /* == Framebuffer reset requested ======================================== */
  void OnFBReset(const EvtMainEvent &emeEvent)
  { // Get reference to actual arguments vector
    const EvtMainArgs &emaArgs = emeEvent.aArgs;
    // Get new frame buffer size
    const int iWidth = emaArgs[1].i, iHeight = emaArgs[2].i;
    // On Mac?
#if defined(MACOS)
    // Get addition position and window size data
    const int iWinX     = emaArgs[3].i, iWinY      = emaArgs[4].i,
              iWinWidth = emaArgs[5].i, iWinHeight = emaArgs[6].i;
    // Log new viewport
    cLog->LogDebugExSafe(
      "Display received new frame buffer size of $x$ (P:$x$;W:$x$).",
      iWidth, iHeight, iWinX, iWinY, iWinWidth, iWinHeight);
    // What is the window type?
    // Frame buffer is covering the entire screen?
    if(!iWinX && !iWinY && rSelected->IsDim(iWinWidth, iWinHeight))
    { // We don't recognise the full-screen?
      if(FlagIsClear(DF_INFULLSCREEN))
      { // Now a native borderless full-screen window
        fsType = FST_NATIVE;
        // In native full-screen mode also re-init the console fbo as well.
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
      // Not in native full-screen mode and re-init console fbo
      FlagClear(DF_INFULLSCREEN|DF_NATIVEFS);
      // Log non-standard full-screen switch
      cLog->LogDebugSafe("Display received external desktop switch!");
      // Update viewport jump from above if/condition scope
      UpdateViewport: cEvtMain->Add(EMC_VID_MATRIX_REINIT);
      // Check if window moved/resized as glfw wont send these
      CheckWindowMoved(iWinX, iWinY);
      CheckWindowResized(iWinWidth, iWinHeight);
    } // Clear native flag otherwise
    else if(FlagIsSet(DF_NATIVEFS)) FlagClear(DF_NATIVEFS);
    // Anything but Mac?
#else
    // Just log new viewport
    cLog->LogDebugExSafe("Display received new framebuffer size of $x$.",
      iWidth, iHeight);
#endif
    // Resize main viewport and if it changed, re-initialise the console fbo
    // and redraw the console
    if(cFboCore->AutoViewport(iWidth, iHeight)) cConGraphics->InitFBO();
    // Redraw the console if enabled
    else cConsole->SetRedrawIfEnabled();
  }
  /* -- Window size requested ---------------------------------------------- */
  void OnReqResize(const EvtWinEvent &eweEvent)
  { // Get reference to actual arguments vector
    const EvtWinArgs &ewaArgs = eweEvent.aArgs;
    // Send the new size to GLFW
    cGlFW->WinSetSize(ewaArgs[0].i, ewaArgs[1].i);
  }
  /* -- Window move requested ---------------------------------------------- */
  void OnReqMove(const EvtWinEvent &eweEvent)
  { // Get reference to actual arguments vector
    const EvtWinArgs &ewaArgs = eweEvent.aArgs;
    // Send the new position to GLFW
    cGlFW->WinMove(ewaArgs[0].i, ewaArgs[1].i);
  }
  /* -- Window centre request ---------------------------------------------- */
  void OnReqCentre(const EvtWinEvent&)
  { // If in full screen mode, don't resize or move anything
    if(FlagIsSet(DF_INFULLSCREEN)) return;
    // Get centre co-ordinates
    int iX, iY; GetCentreCoords(iX, iY,
      cInput->GetWindowWidth(), cInput->GetWindowHeight());
    // Move the window
    cGlFW->WinMove(iX, iY);
  }
  /* -- Window reset requested --------------------------------------------- */
  void OnReqReset(const EvtWinEvent&)
  { // If in full screen mode, don't resize or move anything
    if(FlagIsSet(DF_INFULLSCREEN)) return;
    // Restore window state
    cGlFW->WinRestore();
    // Translate user specified window size and set the size of hte window
    int iX, iY; TranslateUserSize(iX, iY);
    cGlFW->WinSetSize(iX, iY);
    // Get new window size (Input won't have received it yet).
    int iW, iH; cGlFW->WinGetSize(iW, iH);
    TranslateUserCoords(iX, iY, iW, iH);
    // Update window position
    cGlFW->WinMove(iX, iY);
  }
  /* -- Toggle full-screen event (Engine thread) --------------------------- */
  void OnReqToggleFS(const EvtWinEvent &eweEvent)
  { // Ignore further requests if already restarting or using native fullscreen
    if(FlagIsSet(DF_NATIVEFS)) return;
    // Disable input events to prevent the full-screen toggler being repeated
    cInput->DisableInputEvents();
    // Use requested setting instead
    SetFullScreen(eweEvent.aArgs.front().b);
    // Re-enable input events
    cInput->EnableInputEvents();
  }
  /* -- Apply gamma setting ------------------------------------------------ */
  void ApplyGamma(void)
  { // Set gamma
    GlFWSetGamma(moSelected->Context(), fGamma);
    // Report
    cLog->LogDebugExSafe("Display set gamma to $$.", fixed, fGamma);
  }
  /* -- Translate user specified window dimensions ------------------------- */
  void TranslateUserSize(int &iW, int &iH) const
  { // Get window size specified by user
    iW = DimGetWidth();
    iH = DimGetHeight();
    // If size is not valid? Report result and return
    if(iW > 0 && iH > 0)
      return cLog->LogDebugExSafe(
        "Display using user specified dimensions of $x$.", iW, iH);
    // Convert selected height to double as we need to use it twice
    const double dHeight = static_cast<double>(rSelected->Height());
    // Set the height to 80% of desktop height
    iH = static_cast<int>(ceil(dHeight * 0.8));
    // Now set the width based on desktop aspect ratio
    iW = static_cast<int>(ceil(static_cast<double>(iH) *
      (static_cast<double>(rSelected->Width()) / dHeight)));
    // Report result
    cLog->LogDebugExSafe("Display translated user size to $x$.", iW, iH);
  }
  /* -- Get centre co-ordinates -------------------------------------------- */
  void GetCentreCoords(int &iX, int &iY, const int iW, const int iH) const
  { // Bail if not in full screen
    if(FlagIsSet(DF_INFULLSCREEN)) { iX = iY = 0; return; }
    // Must have current desktop information
    if(!moSelected->Context())
    { // Clear co-ordinates
      iX = iY = 0;
      // Put message in log and return
      return cLog->LogWarningSafe(
        "Display class cannot centre the window without monitor data.");
    } // Get new co-ordinates
    iX = (rSelected->Width() / 2) - (iW / 2);
    iY = (rSelected->Height() / 2) - (iH / 2);
  }
  /* -- Translate co-ordinates --------------------------------------------- */
  void TranslateUserCoords(int &iX, int &iY, const int iW, const int iH) const
  { // Put user values into window co-ordinates
    iX = CoordGetX();
    iY = CoordGetY();
    // Centre window?
    if(iX==-2 || iY==-2) GetCentreCoords(iX, iY, iW, iH);
  }
  /* -- Re-initialise window ----------------------------------------------- */
  void ReInitWindow(const bool bState)
  { // Initial width and height of window
    int iWidth, iHeight;
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
        mUsing = moSelected->Context();
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
      iWinPosX = iWinPosY = 0;
      // Set initial window width and height
      iWidth = rSelected->Width();
      iHeight = rSelected->Height();
      // Instruct glfw to set full-screen window
      cGlFW->WinSetMonitor(mUsing, iWinPosX, iWinPosY,
        iWidth, iHeight, rSelected->Refresh());
      // Log that we switched to full-screen mode. Casting requested monitor
      // and video mode to int so it displays as -1 and not max uint64.
      cLog->LogInfoExSafe(
        "Display switch to $ full-screen $x$ (M:$>$;V:$>$;R:$).",
        cpType, iWidth, iHeight, static_cast<ssize_t>(stMRequested),
        moSelected->Index(), static_cast<ssize_t>(stVRequested),
        rSelected->Index(), rSelected->Refresh());
    } // Window mode selected
    else
    { // Not in full-screen mode or native mode
      FlagClear(DF_INFULLSCREEN|DF_NATIVEFS);
      // Trnslate user specified window size
      TranslateUserSize(iWidth, iHeight);
      TranslateUserCoords(iWinPosX, iWinPosY, iWidth, iHeight);
      // Is a desktop mode window (Could change via OnFBReset())
      fsType = FST_WINDOW;
      // We need to adjust to the position of the currently selected monitor so
      // it actually appears on that monitor.
      iWinPosX += moSelected->CoordGetX();
      iWinPosY += moSelected->CoordGetY();
      // Instruct glfw to change to window mode
      cGlFW->WinSetMonitor(nullptr, iWinPosX, iWinPosY, iWidth, iHeight, 0);
      // Window mode so update users window border
      cGlFW->WinSetDecoratedAttrib(FlagIsSet(DF_BORDER));
      cGlFW->WinSetResizableAttrib(FlagIsSet(DF_SIZABLE));
      // Log that we switched to window mode
      cLog->LogInfoExSafe("Display switched to desktop window $x$ at $x$.",
        iWidth, iHeight, iWinPosX, iWinPosY);
    } // Update window attributes
    cGlFW->WinSetFloatingAttrib(FlagIsSet(DF_FLOATING));
    cGlFW->WinSetAutoIconifyAttrib(FlagIsSet(DF_AUTOICONIFY));
    cGlFW->WinSetFocusOnShowAttrib(FlagIsSet(DF_AUTOFOCUS));
    // Store current window position
    cGlFW->WinGetPos(iWinPosX, iWinPosY);
    // Store initial window size. This needs to be done because on Linux, the
    // window size isn't sent so we need to store the value.
    cInput->SetWindowSize(iWidth, iHeight);
    // Get scale of window
    cGlFW->WinGetScale(fWinScaleWidth, fWinScaleHeight);
    // Need to fix a GLFW scaling bug with this :(
#if defined(MACOS)
    // If hidpi not enabled? Update the main fbo viewport size without scale
    if(FlagIsClear(DF_HIDPI))
      return cFboCore->DimSet(static_cast<GLsizei>(cInput->GetWindowWidth()),
                              static_cast<GLsizei>(cInput->GetWindowHeight()));
    // Update the main fbo viewport size with scale
    cFboCore->DimSet(static_cast<GLsizei>(cInput->GetWindowWidth()) *
                       static_cast<GLsizei>(fWinScaleWidth),
                     static_cast<GLsizei>(cInput->GetWindowHeight()) *
                       static_cast<GLsizei>(fWinScaleHeight));
    // Remove native flag since GLFW canno set or detect this directly.
    FlagClear(DF_NATIVEFS);
    // Windows and linux doesn't need the scale
#else
    // Update the main fbo viewport size without scale
    cFboCore->DimSet(static_cast<GLsizei>(cInput->GetWindowWidth()),
                     static_cast<GLsizei>(cInput->GetWindowHeight()));
#endif
    // Show the window
    cGlFW->WinShow();
    // Window has been focued if auto-focus is enabled
    if(FlagIsSet(DF_AUTOFOCUS)) FlagSet(DF_FOCUSED);
    // Tell input to reset mouse in window focus
    cInput->FlagSet(IF_MOUSEFOCUS);
    // Update cursor visibility as OS or glfw can mess it up
    cInput->CommitCursorNow();
    // If we're in Linux?
#if defined(LINUX)
    // Send a event to recalculate the matrix because it seems the fbo resize
    // event isn't being sent on the GLFW that came with Ubuntu.
    cEvtMain->Add(EMC_VID_MATRIX_REINIT);
#endif
  }
  /* -- Get monitors list ------------------------------------------ */ public:
  const GlFWMonitors &GetMonitors(void) const { return mlData; }
  /* -- Get monitors cound ------------------------------------------------- */
  size_t GetMonitorsCount(void) const { return mlData.size(); }
  /* -- Return selected monitor -------------------------------------------- */
  const GlFWMonitor *GetSelectedMonitor(void) const { return moSelected; }
  /* -- Return selected resolution ----------------------------------------- */
  const GlFWRes *GetSelectedRes(void) const { return rSelected; }
  /* -- Request from alternative thread to resize window ------------------- */
  void RequestResize(const int iW, const int iH)
    { cEvtWin->AddUnblock(EWC_WIN_RESIZE, iW, iH); }
  /* -- Request from alternative thread to move window --------------------- */
  void RequestMove(const int iX, const int iY)
    { cEvtWin->AddUnblock(EWC_WIN_MOVE, iX, iY); }
  /* -- Request the window set a new cursor graphic ------------------------ */
  void RequestSetCursor(const GlFWCursorType gctType)
    { cEvtWin->AddUnblock(EWC_WIN_CURSET, gctType); }
  /* -- Request the window reset the cursor graphic ------------------------ */
  void RequestResetCursor(void) { cEvtWin->AddUnblock(EWC_WIN_CURRESET); }
  /* -- Request from alternative thread to centre the window --------------- */
  void RequestCentre(void) { cEvtWin->AddUnblock(EWC_WIN_CENTRE); }
  /* -- Request from alternative thread to reposition the window ----------- */
  void RequestReposition(void) { cEvtWin->AddUnblock(EWC_WIN_RESET); }
  /* -- Request to close window -------------------------------------------- */
  void RequestClose(void) { cEvtWin->Add(EWC_WIN_HIDE); }
  /* -- Request to minimise window ----------------------------------------- */
  void RequestMinimise(void) { cEvtWin->Add(EWC_WIN_MINIMISE); }
  /* -- Request to maximise window ----------------------------------------- */
  void RequestMaximise(void) { cEvtWin->Add(EWC_WIN_MAXIMISE); }
  /* -- Request to restore window ------------------------------------------ */
  void RequestRestore(void) { cEvtWin->Add(EWC_WIN_RESTORE); }
  /* -- Request to focus window -------------------------------------------- */
  void RequestFocus(void) { cEvtWin->Add(EWC_WIN_FOCUS); }
  /* -- Request for window attention --------------------------------------- */
  void RequestAttention(void) { cEvtWin->Add(EWC_WIN_ATTENTION); }
  /* -- Request from alternative thread to fullscreen toggle without save -- */
  void RequestFSToggle(const bool bState)
    { cEvtWin->AddUnblock(EWC_WIN_TOGGLE_FS, bState); }
  /* -- Set full screen in Window thread ----------------------------------- */
  void SetFullScreen(const bool bState)
  {// Return if setting not different than actual
    if(FlagIsEqualToBool(DF_INFULLSCREEN, bState)) return;
    // If using Linux?
#if defined(LINUX)
    // Here appears to be yet another issue with GLFW on Linux. Changing back
    // to window mode from full-screen isn't working for some reason so I'm
    // just going to work around that by just quitting the thread and doing a
    // full re-initialisation until I can (ever?) figure out why this is
    // happening on Linux and not on MacOS or Windows.
    if(!bState) return cEvtMain->RequestQuitThread();
#endif
    // Update new fullscreen setting and re-initialise if successful
    ReInitWindow(bState);
    // Update viewport
    cEvtMain->Add(EMC_VID_MATRIX_REINIT);
  }
  /* -- Return current video mode refresh rate ----------------------------- */
  int GetRefreshRate(void) { return rSelected->Refresh(); }
  /* -- Get selected monitor id -------------------------------------------- */
  int GetMonitorId(void) const { return moSelected->Index(); }
  /* -- Get selected video mode id ----------------------------------------- */
  int GetVideoModeId(void) const { return rSelected->Index(); }
  /* -- Init info ---------------------------------------------------------- */
  const string &GetMonitorName(void) const { return moSelected->Name(); }
  /* -- Set default matrix ------------------------------------------------- */
  void SetDefaultMatrix(const bool bForce=true) const
  { // Set the default matrix from the configuration and if it was changed
    // also update the consoles FBO too.
    if(cFboCore->AutoMatrix(fMatrixWidth, fMatrixHeight, bForce))
      cConGraphics->InitFBO();
    // Else redraw the console if enabled
    else cConsole->SetRedrawIfEnabled();
  }
  /* -- Alter default matrix ----------------------------------------------- */
  bool AlterDefaultMatrix(const GLfloat fNewWidth, const GLfloat fNewHeight)
  { // If width changed?
    if(UtilIsFloatNotEqual(fNewWidth, fMatrixWidth))
    { // Update width and if height changed? Update the height
      fMatrixWidth = fNewWidth;
      if(UtilIsFloatNotEqual(fNewHeight, fMatrixHeight))
        fMatrixHeight = fNewHeight;
    } // If height changed? Update the height and fall through to re-init
    else if(UtilIsFloatNotEqual(fNewHeight, fMatrixHeight))
      fMatrixHeight = fNewHeight;
    // Not modified so don't change the fbo
    else return false;
    // Force re-initialise the matrix
    SetDefaultMatrix();
    // Send event to addons that the matrix changed
    cEvtMain->Add(EMC_VID_MATRIX_REINIT);
    // Success
    return true;
  }
  /* -- ReInit matrix ------------------------------------------------------ */
  void ReInitMatrix(void)
  { // Force-reinitialise matrix
    SetDefaultMatrix();
    // Inform lua scripts that they should redraw the framebuffer
    cEvtMain->Add(EMC_LUA_REDRAW);
  }
  /* -- Update window icon ------------------------------------------------- */
  void UpdateIcons(void)
  { // This functionality throws a GLFW api error on MacOS so just NullOp it
#if !defined(MACOS)
    // If using interactive mode?
    if(cSystem->IsGraphicalMode())
    { // Ignore if no icons
      if(gilIcons.empty()) return;
      // Capture exceptions and ask GLFW to set the icon
      try { cGlFW->WinSetIcon(UtilIntOrMax<int>(gilIcons.size()),
              gilIcons.data()); }
      // Exception occured? GLFW can throw GLFW_PLATFORM_ERROR on Wayland which
      // is absolutely retarded as is not consistent with other platforms such
      // as MacOS which will silently succeed
      catch(const exception &e)
      { // Just log the error that occured
        cLog->LogWarningExSafe(
          "Display could not load $ icon files due to GlFW exception: $.",
          gilIcons.size(), e.what());
        // Done
        return;
      } // Report that we updated the icons
      cLog->LogInfoExSafe("Display updated $ windows icons." ,
        gilIcons.size());
      // Show details?
      if(cLog->HasLevel(LH_DEBUG))
        for(Image &imC : ilIcons)
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
  bool SetIcon(const string &strNames)
  { // Seperate icon names and if we got an icon name
    if(Token tIcons{ strNames, ":", 3 })
    { // If using interactive mode?
      if(cSystem->IsGraphicalMode())
      { // Clear images and icons
        gilIcons.clear();
        ilIcons.clear();
        // Create contiguous memory for glfw icon descriptors and icon data
        gilIcons.reserve(tIcons.size());
        ilIcons.reserve(tIcons.size());
        // Build icons
        for(string &strName : tIcons)
        { // Check filename and load icon and force to RGB 32BPP.
          DirVerifyFileNameIsValid(strName);
          const Image &imC = ilIcons.emplace_back(
            Image{ StdMove(strName), IL_REVERSE|IL_TORGB|IL_TO32BPP });
          const ImageSlot &imsD = imC.GetSlotsConst().front();
          gilIcons.emplace_back(GLFWimage{ imsD.DimGetWidth<int>(),
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
  void SetIconFromLua(const string &strNames)
    { if(SetIcon(strNames)) return cEvtWin->Add(EWC_WIN_SETICON); }
  /* -- Get window full-screen type ---------------------------------------- */
  FSType GetFSType(void) const { return fsType; }
  const string_view &GetFSTypeString(const FSType fsT) const
    { return fstStrings.Get(fsT); }
  const string_view &GetFSTypeString(void) const
    { return GetFSTypeString(fsType); }
  /* -- Get window position ------------------------------------------------ */
  int GetWindowPosX(void) const { return iWinPosX; }
  int GetWindowPosY(void) const { return iWinPosY; }
  float GetWindowScaleWidth(void) const { return fWinScaleWidth; }
  float GetWindowScaleHeight(void) const { return fWinScaleHeight; }
  /* -- Init --------------------------------------------------------------- */
  void Init(void)
  { // Class initialised
    IHInitialise();
    // Log progress
    cLog->LogDebugSafe("Display class starting up...");
    // Enumerate monitors and video modes
    EnumerateMonitorsAndVideoModes();
    // Inform main fbo class of our transparency setting
    cFboCore->fboMain.FboSetTransparency(FlagIsSet(DF_TRANSPARENT));
    // Set context settings
    cGlFW->GlFWSetAlphaBits(iFBDepthA);
    cGlFW->GlFWSetAuxBuffers(iAuxBuffers);
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
    cGlFW->GlFWSetRefreshRate(GetRefreshRate());
    cGlFW->GlFWSetRelease(iRelease);
    cGlFW->GlFWSetRetinaMode(FlagIsSet(DF_HIDPI));
    cGlFW->GlFWSetRobustness(iRobustness);
    cGlFW->GlFWSetSRGBCapable(FlagIsSet(DF_SRGB));
    cGlFW->GlFWSetStencilBits(0); // No use (yet)
    cGlFW->GlFWSetStereo(FlagIsSet(DF_STEREO));
    cGlFW->GlFWSetTransparency(cFboCore->fboMain.FboIsTransparencyEnabled());
    // Set Apple operating system only settings
    // Get window name and use it for frame and instance name. It's assumed
    // that 'cpTitle' won't be freed while using it these two times.
    const char*const cpTitle = cCVars->GetCStrInternal(APP_TITLE);
    cGlFW->GlFWSetFrameName(cpTitle);
    // Initialise basic window. We will modify it after due to limitations in
    // this particular function. For example, this can't set the refresh rate.
    cSystem->WindowInitialised(cGlFW->WinInit(1, 1, cpTitle, nullptr));
    // Re-adjust the window
    ReInitWindow(FlagIsSet(DF_FULLSCREEN));
    // Set forced aspect ratio
    cGlFW->WinSetAspectRatio(cCVars->GetStrInternal(WIN_ASPECT));
    // Register monitor removal event. We can't use our events system for this
    // because once the event callback is over, the data for the monitor is
    // freed.
    glfwSetMonitorCallback(OnMonitorStatic);
    // Update icons if there are some loaded by the cvars callbacks
    UpdateIcons();
    // Set default gamma for selected monitor
    ApplyGamma();
    // Register main and window thread events
    cEvtMain->RegisterEx(*this);
    cEvtWin->RegisterEx(*this);
    // Log progress
    cLog->LogInfoSafe("Display class started successfully.");
  }
  /* -- DeInit ------------------------------------------------------------- */
  void DeInit(void)
  { // Ignore if class not initialised
    if(IHNotDeInitialise()) return;
    // Log progress
    cLog->LogDebugSafe("Display class deinitialising...");
    // Remove events we personally handle
    glfwSetMonitorCallback(nullptr);
    // Remove invalidated active flags
    FlagClear(DF_FOCUSED|DF_EXCLUSIVE|DF_INFULLSCREEN|DF_NATIVEFS);
    // Window type deinitialised
    fsType = FST_STANDBY;
    // Deinit window and engine events
    cEvtWin->UnregisterEx(*this);
    cEvtMain->UnregisterEx(*this);
    // Have window?
    if(cGlFW->WinIsAvailable())
    { // If we have monitor?
      if(moSelected)
      { // Restore gamma (this fails if theres no window).
        GlFWSetGamma(moSelected->Context(), 1.0);
        // Monitor no longer valid
        moSelected = nullptr;
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
      moSelected = nullptr;
    } // Clear selected video mode
    rSelected = nullptr;
    // Log progress
    cLog->LogInfoSafe("Display class deinitialised successfully.");
  }
  /* -- Constructor -------------------------------------------------------- */
  Display(void) :
    /* --------------------------------------------------------------------- */
    IHelper{ __FUNCTION__ },           // Send name to init helper
    DisplayFlags{ DF_NONE },           // No display flags set
    EvtMain::RegVec{                   // Register main events
      { EMC_VID_FB_REINIT,     bind(&Display::OnFBReset,     this, _1) },
      { EMC_VID_MATRIX_REINIT, bind(&Display::OnMatrixReset, this, _1) },
      { EMC_WIN_CLOSE,         bind(&Display::OnClose,       this, _1) },
      { EMC_WIN_FOCUS,         bind(&Display::OnFocus,       this, _1) },
      { EMC_WIN_ICONIFY,       bind(&Display::OnIconify,     this, _1) },
      { EMC_WIN_MOVED,         bind(&Display::OnMoved,       this, _1) },
      { EMC_WIN_REFRESH,       bind(&Display::OnRefresh,     this, _1) },
      { EMC_WIN_RESIZED,       bind(&Display::OnResized,     this, _1) },
      { EMC_WIN_SCALE,         bind(&Display::OnScale,       this, _1) },
    },
    EvtWin::RegVec{                    // Register window events
      { EWC_WIN_ATTENTION,   bind(&Display::OnReqAttention,    this, _1) },
      { EWC_WIN_CENTRE,      bind(&Display::OnReqCentre,       this, _1) },
      { EWC_WIN_CURPOSGET,   bind(&Display::OnReqGetCursorPos, this, _1) },
      { EWC_WIN_CURPOSSET,   bind(&Display::OnReqSetCursorPos, this, _1) },
      { EWC_WIN_CURRESET,    bind(&Display::OnReqResetCursor,  this, _1) },
      { EWC_WIN_CURSET,      bind(&Display::OnReqSetCursor,    this, _1) },
      { EWC_WIN_CURSETVIS,   bind(&Display::OnReqSetCurVisib,  this, _1) },
      { EWC_WIN_FOCUS,       bind(&Display::OnReqFocus,        this, _1) },
      { EWC_WIN_HIDE,        bind(&Display::OnReqHide,         this, _1) },
      { EWC_WIN_LIMITS,      bind(&Display::OnReqSetLimits,    this, _1) },
      { EWC_WIN_MAXIMISE,    bind(&Display::OnReqMaximise,     this, _1) },
      { EWC_WIN_MINIMISE,    bind(&Display::OnReqMinimise,     this, _1) },
      { EWC_WIN_MOVE,        bind(&Display::OnReqMove,         this, _1) },
      { EWC_WIN_RESET,       bind(&Display::OnReqReset,        this, _1) },
      { EWC_WIN_RESIZE,      bind(&Display::OnReqResize,       this, _1) },
      { EWC_WIN_RESTORE,     bind(&Display::OnReqRestore,      this, _1) },
      { EWC_WIN_SETICON,     bind(&Display::OnReqSetIcons,     this, _1) },
      { EWC_WIN_SETRAWMOUSE, bind(&Display::OnReqSetRawMouse,  this, _1) },
      { EWC_WIN_SETSTKKEYS,  bind(&Display::OnReqStickyKeys,   this, _1) },
      { EWC_WIN_SETSTKMOUSE, bind(&Display::OnReqStickyMouse,  this, _1) },
      { EWC_WIN_TOGGLE_FS,   bind(&Display::OnReqToggleFS,     this, _1) },
    },
    DimCoInt{ -1, -1, 0, 0 },          // Requested position and size
    moSelected(nullptr),               // No monitor selected
    rSelected(nullptr),                // No video mode selected
    stMRequested(StdMaxSizeT),         // No monitor requested
    stVRequested(StdMaxSizeT),         // No video mode id requested
    fGamma(0),                         // Gamma initialised by CVars
    fMatrixWidth(0.0f),                // Matrix width initialised by CVars
    fMatrixHeight(0.0f),               // Matrix height initialised by CVars
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
    iWinPosX(GLFW_DONT_CARE),          // No initial window X position
    iWinPosY(GLFW_DONT_CARE),          // No initial window Y position
    iAuxBuffers(GLFW_DONT_CARE),       // No auxiliary buffers specified
    iSamples(GLFW_DONT_CARE),          // No anti-aliasing samples specified
    fWinScaleWidth(1.0f),              // No initial scale width
    fWinScaleHeight(1.0f),             // No initial scale height
    lrFocused{ "OnFocused" },          // Set name for OnFocused lua event
    fsType(FST_STANDBY),               // Full-screen type
    fstStrings{{                       // Init full-screen type strings
      STR(FST_STANDBY),    STR(FST_WINDOW), STR(FST_EXCLUSIVE),
      STR(FST_BORDERLESS), STR(FST_NATIVE)
    }, "FST_UNKNOWN"}                  // End of full-screen type strings list
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Destructor --------------------------------------------------------- */
  DTORHELPER(~Display, DeInit())
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(Display)             // Suppress default functions for safety
  /* -- Helper macro for boolean based CVars based on OS ------------------- */
#define CBCVARFLAG(n, f) CVarReturn n(const bool bState) \
    { FlagSetOrClear(f, bState); return ACCEPT; }
#define CBCVARFORCEFLAG(n, f, v) CVarReturn n(const bool) \
    { FlagSetOrClear(f, v); return ACCEPT; }
#define CBCVARRANGE(t,n,d,l,g) CVarReturn n(const t tParam) \
    { return CVarSimpleSetIntNLG(d, tParam, l, g); }
  /* -- Create boolean based cvar callbacks -------------------------------- */
  CBCVARFLAG(AutoFocusChanged, DF_AUTOFOCUS)
  CBCVARFLAG(AutoIconifyChanged, DF_AUTOICONIFY)
  CBCVARFLAG(BorderChanged, DF_BORDER)
  CBCVARFLAG(CloseableChanged, DF_CLOSEABLE)
  CBCVARFLAG(DoubleBufferChanged, DF_DOUBLEBUFFER)
  CBCVARFLAG(FloatingChanged, DF_FLOATING)
  CBCVARFLAG(FullScreenStateChanged, DF_FULLSCREEN)
  CBCVARFLAG(MinFocusChanged, DF_MINFOCUS)
  CBCVARFLAG(SRGBColourSpaceChanged, DF_SRGB)
  CBCVARFLAG(SetMaximisedMode, DF_MAXIMISED)
  CBCVARFLAG(SetWindowTransparency, DF_TRANSPARENT)
  CBCVARFLAG(SizableChanged, DF_SIZABLE)
  CBCVARRANGE(int, AuxBuffersChanged, iAuxBuffers, GLFW_DONT_CARE, 16)
  CBCVARRANGE(int, CtxMajorChanged, iCtxMajor, GLFW_DONT_CARE, 4)
  CBCVARRANGE(int, CtxMinorChanged, iCtxMinor, GLFW_DONT_CARE, 6)
  CBCVARRANGE(int, FsaaChanged, iSamples, GLFW_DONT_CARE, 8)
  CBCVARRANGE(int, SetForcedBitDepthA, iFBDepthA, GLFW_DONT_CARE, 16)
  CBCVARRANGE(int, SetForcedBitDepthB, iFBDepthB, GLFW_DONT_CARE, 16)
  CBCVARRANGE(int, SetForcedBitDepthG, iFBDepthG, GLFW_DONT_CARE, 16)
  CBCVARRANGE(int, SetForcedBitDepthR, iFBDepthR, GLFW_DONT_CARE, 16)
  CBCVARRANGE(GLfloat, SetMatrixHeight, fMatrixHeight, 200.0f, 16384.0f)
  CBCVARRANGE(GLfloat, SetMatrixWidth, fMatrixWidth, 320.0f, 16384.0f)
  /* ----------------------------------------------------------------------- */
#if defined(MACOS)                     // Compiling on MacOS?
  /* ----------------------------------------------------------------------- */
  CBCVARFLAG(GraphicsSwitchingChanged, DF_GASWITCH)
  CBCVARFLAG(HiDPIChanged, DF_HIDPI)
  CBCVARFORCEFLAG(ForwardChanged, DF_FORWARD, true)
  CBCVARFORCEFLAG(SetGLDebugMode, DF_DEBUG, false)
  CBCVARFORCEFLAG(SetNoErrorsMode, DF_NOERRORS, false)
  CBCVARFORCEFLAG(SetStereoMode, DF_STEREO, false)
  /* ----------------------------------------------------------------------- */
#else                                  // Windows or Linux?
  /* ----------------------------------------------------------------------- */
  CBCVARFLAG(ForwardChanged, DF_FORWARD)
  CBCVARFLAG(SetGLDebugMode, DF_DEBUG)
  CBCVARFLAG(SetNoErrorsMode, DF_NOERRORS)
  CBCVARFLAG(SetStereoMode, DF_STEREO)
  CBCVARFORCEFLAG(GraphicsSwitchingChanged, DF_GASWITCH, false)
  CBCVARFORCEFLAG(HiDPIChanged, DF_HIDPI, false)
  /* ----------------------------------------------------------------------- */
#endif                                 // End of OS conditions check
  /* -- Done with these macros --------------------------------------------- */
#undef CBCVARRANGE
#undef CBCVARFORCEFLAG
#undef CBCVARFLAG
  /* -- Set window width and height ---------------------------------------- */
  CVarReturn HeightChanged(const int iHeight)
    { DimSetHeight(iHeight); return ACCEPT; }
  CVarReturn WidthChanged(const int iWidth)
    { DimSetWidth(iWidth); return ACCEPT; }
  /* -- Set robustness ----------------------------------------------------- */
  CVarReturn RobustnessChanged(const size_t stIndex)
  { // Not supported on MacOS
#if defined(MACOS)
    static_cast<void>(stIndex); iRobustness = GLFW_NO_ROBUSTNESS;
#else
    // Possible values
    static const array<const int,3> aValues{
      GLFW_NO_RESET_NOTIFICATION, GLFW_LOSE_CONTEXT_ON_RESET,
      GLFW_NO_ROBUSTNESS
    }; // Fail if invalid
    if(stIndex >= aValues.size()) return DENY;
    // Set the api
    iRobustness = aValues[stIndex];
#endif
    // Success
    return ACCEPT;
  }
  /* -- Set release behaviour ---------------------------------------------- */
  CVarReturn ReleaseChanged(const size_t stIndex)
  { // Not supported on MacOS
#if defined(MACOS)
    static_cast<void>(stIndex); iRelease = GLFW_RELEASE_BEHAVIOR_NONE;
#else
    // Possible values
    static const array<const int,3> aValues{
      GLFW_ANY_RELEASE_BEHAVIOR, GLFW_RELEASE_BEHAVIOR_FLUSH,
      GLFW_RELEASE_BEHAVIOR_NONE
    }; // Fail if invalid
    if(stIndex >= aValues.size()) return DENY;
    // Set the api
    iRelease = aValues[stIndex];
#endif
    // Success
    return ACCEPT;
  }
  /* -- Set api ------------------------------------------------------------ */
  CVarReturn ApiChanged(const size_t stIndex)
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
  CVarReturn ProfileChanged(const size_t stIndex)
  { // Only core profile supported on MacOS
#if defined(MACOS)
    static_cast<void>(stIndex); iProfile = GLFW_OPENGL_CORE_PROFILE;
#else
    // Possible values
    static const array<const int,3> aValues{
      GLFW_OPENGL_CORE_PROFILE, GLFW_OPENGL_COMPAT_PROFILE,
      GLFW_OPENGL_ANY_PROFILE
    }; // Fail if invalid
    if(stIndex >= aValues.size()) return DENY;
    // Set the api
    iProfile = aValues[stIndex];
#endif
    // Success
    return ACCEPT;
  }
  /* -- Set full-screen video mode cvar ------------------------------------ */
  CVarReturn FullScreenModeChanged(const int iVId)
  { // Return if invalid full-screen mode
    if(iVId < -2) return DENY;
    // Set video mode requested
    stVRequested = static_cast<size_t>(iVId);
    // Accepted
    return ACCEPT;
  }
  /* -- Set monitor number ------------------------------------------------- */
  CVarReturn MonitorChanged(const int iMId)
  { // Return if invalid full-screen mode
    if(iMId < -1) return DENY;
    // Set video mode requested
    stMRequested = static_cast<size_t>(iMId);
    // Accepted
    return ACCEPT;
  }
  /* -- Set window X position ---------------------------------------------- */
  CVarReturn SetXPosition(const int iNewX)
  { // Deny change request if an invalid value was sent
    CoordSetX(iNewX);
    // Apply window position if window is available
    if(cGlFW && cGlFW->WinIsAvailable()) RequestReposition();
    // Success
    return ACCEPT;
  }
  /* -- Set window Y position ---------------------------------------------- */
  CVarReturn SetYPosition(const int iNewY)
  { // Deny change request if an invalid value was sent
    CoordSetY(iNewY);
    // Apply window position if window is available
    if(cGlFW && cGlFW->WinIsAvailable()) RequestReposition();
    // Success
    return ACCEPT;
  }
  /* -- Set gamma ---------------------------------------------------------- */
  CVarReturn GammaChanged(const GLfloat fNewGamma)
  { // Deny change request if an invalid gamma value was sent
    if(!CVarToBoolReturn(CVarSimpleSetIntNLG(fGamma, fNewGamma, 0.25f, 4.00f)))
      return DENY;
    // Apply new gamma setting if window is available
    if(cGlFW && cGlFW->WinIsAvailable() && moSelected->Context()) ApplyGamma();
    // Success
    return ACCEPT;
  }
  /* -- Icon filenames changed (allow blank strings) ------ Core::SetIcon -- */
  CVarReturn SetIcon(const string &strF, string&)
    { return BoolToCVarReturn(strF.empty() || SetIcon(strF)); }
  /* ----------------------------------------------------------------------- */
} *cDisplay = nullptr;                 // Pointer to static class
/* -- Monitor changed static event ----------------------------------------- */
void Display::OnMonitorStatic(GLFWmonitor*const mA, const int iA)
  { cDisplay->OnMonitor(mA, iA); }
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
