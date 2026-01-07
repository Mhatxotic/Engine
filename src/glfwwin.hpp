/* == GLFWWIN.HPP ========================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## Manage Window objects using GLFW.                                   ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IGlFWWindow {                // Start of private module namespace
/* ------------------------------------------------------------------------- */
using namespace ICollector::P;         using namespace ICommon::P;
using namespace ICoord::P;             using namespace IDim::P;
using namespace IError::P;             using namespace IEvtMain::P;
using namespace IGlFWUtil::P;          using namespace ILog::P;
using namespace IStd::P;               using namespace IString::P;
using namespace IUtf::P;               using namespace Lib::OS::GlFW;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* ------------------------------------------------------------------------- */
class GlFWWindow :                     // GLFW window class
  /* -- Base classes ------------------------------------------------------- */
  public GlFWUtil                      // Include utilities
{ /* -- Private variables -------------------------------------------------- */
  GLFWwindow      *wClass;             // GLFW window context
  StrVector        svFiles;            // Drag and drop file list
  /* -- Return the window handle ------------------------------------------- */
  GLFWwindow *WinGetHandle() const { return wClass; }
  /* -- Set window data pointer -------------------------------------------- */
  template<typename AnyCast=void*const>void WinSetData(AnyCast acData) const
    { GlFWSetWindowUserPointer<AnyCast>(WinGetHandle(), acData); }
  /* -- Clear the window handle -------------------------------------------- */
  void WinClearHandle() { WinSetHandle(nullptr); }
  /* -- Assign a new window handle ----------------------------------------- */
  void WinSetHandle(GLFWwindow*const wNewClass) { wClass = wNewClass; }
  /* -- Check if window handle is set and copy it or return failure -------- */
  bool WinSetHandleResult(GLFWwindow*const wNewClass)
    { if(!wNewClass) return false; WinSetHandle(wNewClass); return true; }
  /* -- Prototypes --------------------------------------------------------- */
  void WinOnDrop(const GLFWwindow*const wC,
    unsigned int uiC, const char **const cpaF)
  { // Check if is our window, and check the pointer and return if empty or
    // invalid or a previous event has not been processed yet.
    if(wC != WinGetHandle() ||
       UtfIsCStringNotValid(cpaF) ||
       !WinGetFiles().empty())
      return;
    // Because we're in the main thread, we need to tell the engine thread
    // but the problem is that glfw will free 'cpaFiles' after this function
    // returns so we need to copy all the strings across unfortunately. Not
    // sure if I should mutex lock 'svFiles' as I am not sure if another
    // 'OnDrop' event will fire before this list gets to the Lua callback.
    // I highly doubt it, but if we get crashes, just bear that in mind!
    WinGetFiles().reserve(uiC);
    for(unsigned int uiI = 0; uiI < uiC; ++uiI)
      WinGetFiles().emplace_back(cpaF[uiI]);
    // Now dispatch the event
    cEvtMain->Add(EMC_INP_DRAGDROP);
  }
  /* -- Event handler for 'glfwSetCharCallback' ---------------------------- */
  static void WinOnInputChar(GLFWwindow*const wC, unsigned int uiChar)
    { cEvtMain->Add(EMC_INP_CHAR, reinterpret_cast<void*>(wC),
        static_cast<Codepoint>(uiChar)); }
  /* -- Event handler for 'glfwSetCursorEnterCallback' --------------------- */
  static void WinOnMouseFocus(GLFWwindow*const wC, int iOnStage)
    { cEvtMain->Add(EMC_INP_MOUSEFOCUS,
        reinterpret_cast<void*>(wC), iOnStage); }
  /* -- Event handler for 'glfwSetCursorPosCallback' ----------------------- */
  static void WinOnMouseMove(GLFWwindow*const wC, double dX, double dY)
    { cEvtMain->Add(EMC_INP_MOUSEMOVE, reinterpret_cast<void*>(wC), dX, dY); }
  /* -- Event handler for 'glfwSetDropCallback' ---------------------------- */
  static void WinOnDragDrop(GLFWwindow*const wC, int iC,
    const char**const cpaFiles)
  { GlFWGetWindowUserPointer<GlFWWindow*>(wC)->
      WinOnDrop(wC, static_cast<unsigned int>(iC), cpaFiles); }
  /* -- Event handler for 'glfwSetFramebufferSizeCallback' ----------------- */
  static void WinOnFrameBufferSize(GLFWwindow*const wC, int iW, int iH)
  { // On Mac?
#if defined(MACOS)
    // Note that there is literally no chance for this to be false but it's not
    // really time critical and only a programming error we make could cause it
    // so we shall check it anyway.
    // Get the window class pointer from the glfw window contact and if got it?
    if(const GlFWWindow*const wPtr = GlFWGetWindowUserPointer<GlFWWindow*>(wC))
    { // The user could use native full-screen and this is the only event we
      // get for it so we will send the window position and size to the event
      // callback can check if screen is actually full.
      const CoordInt ciPos{ wPtr->WinGetPos() };
      const DimInt diSize{ wPtr->WinGetSize() };
      cEvtMain->Add(EMC_VID_FBREINIT, reinterpret_cast<void*>(wC), iW, iH,
        ciPos.CoordGetX(), ciPos.CoordGetY(), diSize.DimGetWidth(),
        diSize.DimGetHeight());
    } // Log null window class pointer
    else cLog->LogWarningExSafe("GlFW got a resize frame buffer event for $x$ "
      "with a NULL window context pointer!", iW, iH);
    // On Windows or Linux?
#else
    // Just send new frame buffer dimensions
    cEvtMain->Add(EMC_VID_FBREINIT, reinterpret_cast<void*>(wC), iW, iH);
#endif
  }
  /* -- Event handler for 'glfwSetKeyCallback' ----------------------------- */
  static void WinOnKeyPress(GLFWwindow*const wC, int iKey, int iScanCode,
    int iAction, int iMods)
  { cEvtMain->Add(EMC_INP_KEYPRESS,
      reinterpret_cast<void*>(wC), iKey, iScanCode, iAction, iMods); }
  /* -- Event handler for 'glfwSetMouseButtonCallback' --------------------- */
  static void WinOnMousePress(GLFWwindow*const wC, int iButton, int iAction,
    int iMods)
  { cEvtMain->Add(EMC_INP_MOUSECLICK,
      reinterpret_cast<void*>(wC), iButton, iAction, iMods); }
  /* -- Event handler for 'glfwSetScrollCallback' -------------------------- */
  static void WinOnMouseScroll(GLFWwindow*const wC, double dX, double dY)
    { cEvtMain->Add(EMC_INP_MOUSESCROLL,
        reinterpret_cast<void*>(wC), dX, dY); }
  /* -- Event handler for 'glfwSetWindowCloseCallback' --------------------- */
  static void WinOnWindowClose(GLFWwindow*const wC)
  { // GLFW must be denied because we will close the window instead.
    glfwSetWindowShouldClose(wC, GL_FALSE);
    // Dispatch the event to handle this ourselve.
    cEvtMain->Add(EMC_WIN_CLOSE, reinterpret_cast<void*>(wC));
  }
  /* -- Event handler for 'glfwSetWindowContentScaleCallback' -------------- */
  static void WinOnWindowScale(GLFWwindow*const wC, const float fX,
    const float fY)
  { cEvtMain->Add(EMC_WIN_SCALE, reinterpret_cast<void*>(wC), fX, fY); }
  /* -- Event handler for 'glfwSetWindowFocusCallback' --------------------- */
  static void WinOnWindowFocus(GLFWwindow*const wC, int iFocused)
    { cEvtMain->Add(EMC_WIN_FOCUS, reinterpret_cast<void*>(wC), iFocused); }
  /* -- Event handler for 'glfwSetWindowIconifyCallback' ------------------- */
  static void WinOnWindowIconify(GLFWwindow*const wC, int iIconified)
    { cEvtMain->Add(EMC_WIN_ICONIFY,
        reinterpret_cast<void*>(wC), iIconified); }
  /* -- Event handler for 'glfwSetWindowPosCallback' ----------------------- */
  static void WinOnWindowMove(GLFWwindow*const wC, int iX, int iY)
    { cEvtMain->Add(EMC_WIN_MOVED, reinterpret_cast<void*>(wC), iX, iY); }
  /* -- Event handler for 'glfwSetWindowRefreshCallback' ------------------- */
  static void WinOnWindowRefresh(GLFWwindow*const wC)
    { cEvtMain->Add(EMC_WIN_REFRESH, reinterpret_cast<void*>(wC)); }
  /* -- Event handler for 'glfwSetWindowSizeCallback' ---------------------- */
  static void WinOnWindowResize(GLFWwindow*const wC, int iW, int iH)
    { cEvtMain->Add(EMC_WIN_RESIZED, reinterpret_cast<void*>(wC), iW, iH); }
  /* -- Register window events --------------------------------------------- */
  void WinRegisterEvents() const
  { // Register glfw event callbacks which will use our event system to
    // process these events.
    glfwSetCharCallback(WinGetHandle(), WinOnInputChar);
    glfwSetCursorEnterCallback(WinGetHandle(), WinOnMouseFocus);
    glfwSetCursorPosCallback(WinGetHandle(), WinOnMouseMove);
    glfwSetDropCallback(WinGetHandle(), WinOnDragDrop);
    glfwSetFramebufferSizeCallback(WinGetHandle(), WinOnFrameBufferSize);
    glfwSetKeyCallback(WinGetHandle(), WinOnKeyPress);
    glfwSetMouseButtonCallback(WinGetHandle(), WinOnMousePress);
    glfwSetScrollCallback(WinGetHandle(), WinOnMouseScroll);
    glfwSetWindowCloseCallback(WinGetHandle(), WinOnWindowClose);
    glfwSetWindowContentScaleCallback(WinGetHandle(), WinOnWindowScale);
    glfwSetWindowFocusCallback(WinGetHandle(), WinOnWindowFocus);
    glfwSetWindowIconifyCallback(WinGetHandle(), WinOnWindowIconify);
    glfwSetWindowPosCallback(WinGetHandle(), WinOnWindowMove);
    glfwSetWindowRefreshCallback(WinGetHandle(), WinOnWindowRefresh);
    glfwSetWindowSizeCallback(WinGetHandle(), WinOnWindowResize);
  }
  /* -- Unregister window events ------------------------------------------- */
  void WinUnregisterEvents() const
  { // Done if theres no window class
    if(WinIsNotAvailable()) return;
    // Remove other callbacks
    glfwSetWindowSizeCallback(WinGetHandle(), nullptr);
    glfwSetWindowRefreshCallback(WinGetHandle(), nullptr);
    glfwSetWindowPosCallback(WinGetHandle(), nullptr);
    glfwSetWindowIconifyCallback(WinGetHandle(), nullptr);
    glfwSetWindowFocusCallback(WinGetHandle(), nullptr);
    glfwSetWindowContentScaleCallback(WinGetHandle(), nullptr);
    glfwSetWindowCloseCallback(WinGetHandle(), nullptr);
    glfwSetScrollCallback(WinGetHandle(), nullptr);
    glfwSetMouseButtonCallback(WinGetHandle(), nullptr);
    glfwSetKeyCallback(WinGetHandle(), nullptr);
    glfwSetFramebufferSizeCallback(WinGetHandle(), nullptr);
    glfwSetDropCallback(WinGetHandle(), nullptr);
    glfwSetCursorPosCallback(WinGetHandle(), nullptr);
    glfwSetCursorEnterCallback(WinGetHandle(), nullptr);
    glfwSetCharCallback(WinGetHandle(), nullptr);
  }
  /* -- Get files -------------------------------------------------- */ public:
  StrVector &WinGetFiles() { return svFiles; }
  /* -- Is the window handle set? ------------------------------------------ */
  bool WinIsAvailable() const { return !!WinGetHandle(); }
  bool WinIsNotAvailable() const { return !WinIsAvailable(); }
  /* -- Set window icon ---------------------------------------------------- */
  void WinSetIcon(const int iCount, const GLFWimage*const giImages) const
    { glfwSetWindowIcon(WinGetHandle(), iCount, giImages); }
  /* -- Update monitor ----------------------------------------------------- */
  void WinSetMonitor(GLFWmonitor*const mM, const CoordInt &ciPosition,
    const DimInt &diSize, const int iR) const
      { glfwSetWindowMonitor(WinGetHandle(), mM, ciPosition.CoordGetX(),
          ciPosition.CoordGetY(), diSize.DimGetWidth(), diSize.DimGetHeight(),
          iR); }
  /* -- Tell GLFW if it should close the window ---------------------------- */
  void WinSetClose(const int iS) const
    { glfwSetWindowShouldClose(WinGetHandle(), iS); }
  /* -- Returns if the window should close --------------------------------- */
  bool WinShouldClose() const
    { return !WinIsAvailable() || glfwWindowShouldClose(WinGetHandle()); }
  bool WinShouldNotClose() const
    { return !WinShouldClose(); }
  /* -- Restore the window from being minimised ---------------------------- */
  void WinRestore() const { glfwRestoreWindow(WinGetHandle()); }
  /* -- Minimise/Iconify the window ---------------------------------------- */
  void WinMinimise() const { glfwIconifyWindow(WinGetHandle()); }
  /* -- Maximise the window ------------------------------------------------ */
  void WinMaximise() const { glfwMaximizeWindow(WinGetHandle()); }
  /* -- Set focus on the window -------------------------------------------- */
  void WinFocus() const { glfwFocusWindow(WinGetHandle()); }
  /* -- Show a window and register events ---------------------------------- */
  void WinShow() const
    { glfwShowWindow(WinGetHandle()); WinRegisterEvents(); }
  /* -- Hide a window and register events ---------------------------------- */
  void WinHide() const
    { WinUnregisterEvents(); glfwHideWindow(WinGetHandle()); }
  /* -- Send dummy mouse position ------------------------------------------ */
  void WinSendMousePosition() const
  { // Get current cursor position
    double dX; double dY; WinGetCursorPos(dX, dY);
    // Dispatch it to the engine thread
    WinOnMouseMove(WinGetHandle(), dX, dY);
  }
  /* -- Get window size ---------------------------------------------------- */
  DimInt WinGetSize() const
  { // Put window dimensions inside the dimensions class and return it
    DimInt diDimensions;
    glfwGetWindowSize(WinGetHandle(), &diDimensions.DimGetWidthRef(),
                                      &diDimensions.DimGetHeightRef());
    return diDimensions;
  }
  /* -- Set window size ---------------------------------------------------- */
  void WinSetSize(const DimInt diSize) const
    { glfwSetWindowSize(WinGetHandle(), diSize.DimGetWidth(),
                                        diSize.DimGetHeight()); }
  /* -- Get window DPI scale ----------------------------------------------- */
  DimFloat WinGetScale() const
  { // Put window scale inside the dimensions class and return it
    DimFloat dfScale;
    glfwGetWindowContentScale(WinGetHandle(), &dfScale.DimGetWidthRef(),
                                              &dfScale.DimGetHeightRef());
    return dfScale;
  }
  /* -- Get framebuffer size ----------------------------------------------- */
  void WinGetFBSize(int &iW, int &iH) const
    { glfwGetFramebufferSize(WinGetHandle(), &iW, &iH); }
  /* -- Set or clear cursor graphic ---------------------------------------- */
  void WinSetCursorGraphic(GLFWcursor*const gcContext = nullptr) const
    { GlFWSetCursor(WinGetHandle(), gcContext); }
  /* -- Toggle cursor visibility ------------------------------------------- */
  void WinSetCursor(const bool bS) const
    { glfwSetInputMode(WinGetHandle(), GLFW_CURSOR,
        bS ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_HIDDEN); }
  /* -- Get/set cursor position -------------------------------------------- */
  void WinGetCursorPos(double &dX, double &dY) const
    { glfwGetCursorPos(WinGetHandle(), &dX, &dY); }
  void WinSetCursorPos(const double dX, const double dY) const
    { glfwSetCursorPos(WinGetHandle(), dX, dY); }
  /* -- Set clipboard c-string --------------------------------------------- */
  void WinSetClipboard(const char*const cpT) const
    { glfwSetClipboardString(WinGetHandle(), cpT); }
  /* -- Set clipboard c++ string ------------------------------------------- */
  void WinSetClipboardString(const string &strT) const
    { WinSetClipboard(strT.data()); }
  /* -- Get clipboard c-string --------------------------------------------- */
  const char *WinGetClipboard() const
  { // Return clipboard string if available
    if(const char*const cpData =
      glfwGetClipboardString(WinGetHandle())) return cpData;
    // Return empty string
    return cCommon->CommonCBlank();
  }
  /* -- Get clipboard C++ string ------------------------------------------- */
  const string WinGetClipboardString() const
    { return WinGetClipboard(); }
  /* -- Get mouse/key button state ----------------------------------------- */
  int WinGetMouse(const int iB) const
    { return glfwGetMouseButton(WinGetHandle(), iB); }
  int WinGetKey(const int iK) const
    { return glfwGetKey(WinGetHandle(), iK); }
  /* -- Get window attributes ---------------------------------------------- */
  int WinGetAttrib(const int iA) const
    { return glfwGetWindowAttrib(WinGetHandle(), iA); }
  /* -- Get window hint boolean -------------------------------------------- */
  bool WinGetAttribBoolean(const int iVar) const
    { return GlFWGBooleanToBoolean(WinGetAttrib(iVar)); }
  /* -- Update window attributes ------------------------------------------- */
  void WinSetAttrib(const int iAttrib, const int iValue) const
  { // Set the window attribute
    glfwSetWindowAttrib(WinGetHandle(), iAttrib, iValue);
    // Log the attribute change
    cLog->LogDebugExSafe("GlFW set attrib $<0x$$> to $$<0x$$>.",
      GlFWGetHintAttribStr(iAttrib), hex, iAttrib, dec, iValue, hex, iValue);
  }
  /* -- Set window aspect ratio -------------------------------------------- */
  void WinSetAspectRatio(const int iNumeric, const int iDenominator) const
    { glfwSetWindowAspectRatio(WinGetHandle(), iNumeric, iDenominator); }
  void WinSetFreeAspectRatio() const
    { WinSetAspectRatio(GLFW_DONT_CARE, GLFW_DONT_CARE); }
  /* -- Set window aspect ratio with a float or double --------------------- */
  void WinSetAspectRatio(const string &strVal) const
  { // Seperate numeric and denominator
    const size_t stPos = strVal.find('.');
    if(stPos != StdNPos)
    { // Get numeric and denominator
      const int iNumeric = StrToNum<int>(strVal.substr(0, stPos)),
        iDenominator = StrToNum<int>(strVal.substr(stPos+1));
      // Free aspect ratio if either are invalid
      if(iNumeric > 0 && iDenominator > 0)
        return WinSetAspectRatio(iNumeric, iDenominator);
    } // Not valid so free the aspect ratio
    WinSetFreeAspectRatio();
  }
  /* -- Set window position ------------------------------------------------ */
  void WinSetPos(const CoordInt ciPosition) const
    { glfwSetWindowPos(WinGetHandle(), ciPosition.CoordGetX(),
                                       ciPosition.CoordGetY()); }
  /* -- Get window size ---------------------------------------------------- */
  CoordInt WinGetPos() const
  { // Put window position inside the coordinates class and return it
    CoordInt ciCoord;
    glfwGetWindowPos(WinGetHandle(), &ciCoord.CoordGetXRef(),
                                     &ciCoord.CoordGetYRef());
    return ciCoord;
  }
  /* -- Swap GL buffers ---------------------------------------------------- */
  void WinSwapGLBuffers() const { glfwSwapBuffers(WinGetHandle()); }
  /* -- Set window attribute core functions -------------------------------- */
  void WinSetAttribBoolean(const int iVar, const bool bVal) const
    { WinSetAttrib(iVar, GlFWBooleanToGBoolean(bVal)); }
  void WinSetAttribEnabled(const int iVar) const
    { WinSetAttribBoolean(iVar, true); }
  void WinSetAttribDisabled(const int iVar) const
    { WinSetAttribBoolean(iVar, false); }
  /* -- Create functions to access all attributes -------------------------- */
#define SET(nc,nu) \
  /* ---------------------------------------------------------------------- */\
  void WinSet ## nc ## Attrib[[maybe_unused]](const bool bState) const \
    { WinSetAttribBoolean(GLFW_ ## nu, bState); } \
  void WinSet ## nc ## AttribEnabled[[maybe_unused]]() const \
    { WinSetAttribEnabled(GLFW_ ## nu); } \
  void WinSet ## nc ## AttribDisabled[[maybe_unused]]() const \
    { WinSetAttribDisabled(GLFW_ ## nu); } \
  bool WinIs ## nc ## AttribEnabled[[maybe_unused]]() const \
    { return GlFWGBooleanToBoolean(WinGetAttrib(GLFW_ ## nu)); } \
  bool WinIs ## nc ## AttribDisabled[[maybe_unused]]() const \
    { return !WinIs ## nc ## AttribEnabled(); }
  /* ----------------------------------------------------------------------- */
  SET(AutoIconify, AUTO_ICONIFY)       // Set window auto-minimise state
  SET(CentreCursor, CENTER_CURSOR)     // Set window cursor centre state
  SET(Debug, OPENGL_DEBUG_CONTEXT)     // Set opengl debug mode
  SET(Decorated, DECORATED)            // Set window border state
  SET(DoubleBuffer, DOUBLEBUFFER)      // Set double buffering
  SET(Floating, FLOATING)              // Set window floating state
  SET(Focus, FOCUSED)                  // Set window focused state
  SET(FocusOnShow, FOCUS_ON_SHOW)      // Set focus on show window
  SET(ForwardCompat, OPENGL_FORWARD_COMPAT) // Set opengl fwd compatibility
  SET(Iconify, ICONIFIED)              // Set window minimised state
  SET(Maximised, MAXIMIZED)            // Set window maximised state
  SET(MouseHovered, HOVERED)           // Set mouse hovered over state
  SET(NoErrors, CONTEXT_NO_ERROR)      // Set context no errors
  SET(Resizable, RESIZABLE)            // Win is resizable?
  SET(SRGBCapable, SRGB_CAPABLE)       // Set SRGB colour space capable
  SET(Stereo, CENTER_CURSOR)           // Set window cursor centre state
  SET(Transparency, TRANSPARENT_FRAMEBUFFER) // Set transparent framebuffer
  SET(Visibility, VISIBLE)             // Set window visibility state
  /* ----------------------------------------------------------------------- */
#if defined(MACOS)                     // Using Apple compiler?
  /* ----------------------------------------------------------------------- */
  SET(GPUSwitching, COCOA_GRAPHICS_SWITCHING) // Set graphics switching?
  SET(RetinaMode,   COCOA_RETINA_FRAMEBUFFER) // Set retina framebuffer?
  /* ----------------------------------------------------------------------- */
#endif                                 // End of Apple check
  /* ----------------------------------------------------------------------- */
#undef SET                             // Done with this macro
  /* -- Check current context ---------------------------------------------- */
  bool WinIsCurrentContext() const
    { return GlFWContext() == WinGetHandle(); }
  /* -- Make current context ----------------------------------------------- */
  void WinSetContext() const { GlFWSetContext(WinGetHandle()); }
  /* -- Set window size limits --------------------------------------------- */
  void WinSetLimits(const int iMinW, const int iMinH,
    const int iMaxW, const int iMaxH) const
      { glfwSetWindowSizeLimits(WinGetHandle(),
          iMinW, iMinH, iMaxW, iMaxH); }
  /* -- Request window attention ------------------------------------------- */
  void WinRequestAttention() const
    { glfwRequestWindowAttention(WinGetHandle()); }
  /* -- Get or set input mode ---------------------------------------------- */
  int WinGetInputMode(const int iM) const
    { return glfwGetInputMode(WinGetHandle(), iM); }
  bool WinGetInputModeBoolean(const int iM) const
    { return GlFWGBooleanToBoolean(WinGetInputMode(iM)); }
  void WinSetInputMode(const int iM, const int iV) const
    { glfwSetInputMode(WinGetHandle(), iM, iV); }
  void WinSetInputModeBoolean(const int iM, const bool bS) const
    { WinSetInputMode(iM, GlFWBooleanToGBoolean(bS)); }
  /* -- Set/get raw mouse motion ------------------------------------------- */
  void WinSetRawMouseMotion(const bool bS) const
    { WinSetInputModeBoolean(GLFW_RAW_MOUSE_MOTION, bS); }
  bool WinGetRawMouseMotion() const
    { return WinGetInputModeBoolean(GLFW_RAW_MOUSE_MOTION); }
  /* -- Set/get lock key mods ---------------------------------------------- */
  void WinSetLockKeyMods(const bool bS) const
    { WinSetInputModeBoolean(GLFW_LOCK_KEY_MODS, bS); }
  bool WinGetLockKeyMods() const
    { return WinGetInputModeBoolean(GLFW_LOCK_KEY_MODS); }
  /* -- Set/get sticky mouse buttons --------------------------------------- */
  void WinSetStickyMouseButtons(const bool bS) const
    { WinSetInputModeBoolean(GLFW_STICKY_MOUSE_BUTTONS, bS); }
  bool WinGetStickyMouseButtons() const
    { return WinGetInputModeBoolean(GLFW_STICKY_MOUSE_BUTTONS); }
  /* -- Set/get sticky keys ------------------------------------------------ */
  void WinSetStickyKeys(const bool bS) const
    { WinSetInputModeBoolean(GLFW_STICKY_KEYS, bS); }
  bool WinGetStickyKeys() const
    { return WinGetInputModeBoolean(GLFW_STICKY_KEYS); }
  /* -- Get mouse buttons tatus--------------------------------------------- */
  int WinGetMouseButton(const int iB) const
    { return glfwGetMouseButton(this->WinGetHandle(), iB); }
  /* -- Destroy the GLFW window -------------------------------------------- */
  void WinDeInit()
  { // Done if theres no window class
    if(WinIsNotAvailable()) return;
    // Destroy the window
    glfwDestroyWindow(WinGetHandle());
    // Nullify the handle
    WinClearHandle();
    // Unregister remaining events
    WinUnregisterEvents();
  }
  /* -- Create the window -------------------------------------------------- */
  GLFWwindow *WinInit(const char*const cpT, GLFWmonitor*const mM)
  { // Bail if handle already set
    if(WinIsAvailable())
      XC("Window already created!",
        "Class", WinGetHandle(), "Title", cpT, "Monitor", mM);
    // Set handle and create window, throw exception if failed
    if(!WinSetHandleResult(glfwCreateWindow(1, 1, cpT, mM, nullptr)))
      XC("Failed to create window!", "Title", cpT, "Monitor", mM);
    // Set user pointer to this class
    WinSetData(this);
    // Return the class we created
    return WinGetHandle();
  }
  /* -- Constructor --------------------------------------------- */ protected:
  GlFWWindow() :                       // Default constructor (no arguments)
    /* -- Initialisers ----------------------------------------------------- */
    wClass{ nullptr }                  // Uninitialised window context
    /* -- No code ---------------------------------------------------------- */
    {}
};/* ----------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
