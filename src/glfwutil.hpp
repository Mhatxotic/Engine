/* == GLFWUTIL.HPP ========================================================= **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## Some GLFW related utilities                                         ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IGlFWUtil {                  // Start of private module namespace
/* ------------------------------------------------------------------------- */
using namespace IError::P;             using namespace IEvtMain::P;
using namespace IIdent::P;             using namespace ILog::P;
using namespace Lib::OS::GlFW;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* -- Returns GLFW_TRUE or GLFW_FALSE depending on expression -------------- */
static int GlFWBooleanToGBoolean(const bool bC)
  { return bC ? GLFW_TRUE : GLFW_FALSE; }
/* -- Returns false if GLFW_FALSE or true if anything else ----------------- */
static bool GlFWGBooleanToBoolean(const int iR)
  { return iR != GLFW_FALSE; }
/* ------------------------------------------------------------------------- */
class GlFWUtil                         // Members initially private
{ /* -- Private typedefs --------------------------------------------------- */
  typedef IdMap<const int> HintList;   // List of glfw hints
  const HintList   hsStrings;          // Hint strings
  /* -- Convert window hint id to string --------------------------- */ public:
  const string_view &GlFWGetHintAttribStr(const int iTarget) const
    { return hsStrings.Get(iTarget); }
  /* -- Set window hint ---------------------------------------------------- */
  void GlFWSetHint(const int iVar, const int iVal)
  { // Set window hint directly
    glfwWindowHint(iVar, iVal);
    // Log the change
    cLog->LogDebugExSafe("GlFW set hint $<0x$$> to $$<0x$$>.",
      GlFWGetHintAttribStr(iVar), hex, iVar, dec, iVal, hex, iVal);
  }
  /* -- Set window hint core functions ------------------------------------- */
  void GlFWSetHintBoolean(const int iVar, const bool bVal)
    { GlFWSetHint(iVar, GlFWBooleanToGBoolean(bVal)); }
  void GlFWSetHintEnabled(const int iVar)
    { GlFWSetHintBoolean(iVar, true); }
  void GlFWSetHintDisabled(const int iVar)
    { GlFWSetHintBoolean(iVar, false); }
  /* -- Set window hint string --------------------------------------------- */
  void GlFWSetHintString(const int iHint, const char*const cpValue)
  { // Set window hint directly
    glfwWindowHintString(iHint, cpValue);
    // Log the change
    cLog->LogDebugExSafe("GlFW set hint $<0x$$> to '$'.",
      GlFWGetHintAttribStr(iHint), hex, iHint, cpValue);
  }
  /* -- OS specific routines ----------------------------------------------- */
#if defined(MACOS)                     // Targeting MacOS?
  /* -- Set frame name in MacOS -------------------------------------------- */
  void GlFWSetCocoaFrameName([[maybe_unused]] const char*const cpName)
    { GlFWSetHintString(GLFW_COCOA_FRAME_NAME, cpName); }
  /* ----------------------------------------------------------------------- */
#elif defined(LINUX)                   // Targeting Linux?
  /* -- Set class name in X11 ---------------------------------------------- */
  void GlFWSetX11ClassName([[maybe_unused]] const char*const cpName)
    { GlFWSetHintString(GLFW_X11_CLASS_NAME, cpName); }
  /* -- Set instance name in X11 ------------------------------------------- */
  void GlFWSetX11InstanceName([[maybe_unused]] const char*const cpName)
    { GlFWSetHintString(GLFW_X11_INSTANCE_NAME, cpName); }
  /* ----------------------------------------------------------------------- */
#endif                                 // End of target checks
  /* -- Set window frame names --------------------------------------------- */
  void GlFWSetFrameName([[maybe_unused]] const char*const cpName)
  { // Set custom frame names based on operating system
#if defined(MACOS)
    GlFWSetCocoaFrameName(cpName);
#elif defined(LINUX)
    GlFWSetX11ClassName(cpName);
    GlFWSetX11InstanceName(cpName);
#endif
  }
  /* -- Create functions to access all attributes -------------------------- */
#define SET(nc,nu) \
  /* ---------------------------------------------------------------------- */\
  void GlFWSet ## nc[[maybe_unused]](const int iNewMode) \
    { GlFWSetHint(GLFW_ ## nu, iNewMode); }
  /* ----------------------------------------------------------------------- */
  SET(RedBits, RED_BITS)               // Set depth of red component
  SET(GreenBits, GREEN_BITS)           // Set depth of green component
  SET(BlueBits, BLUE_BITS)             // Set depth of blue component
  SET(AlphaBits, ALPHA_BITS)           // Set depth of alpha component
  SET(DepthBits, DEPTH_BITS)           // Set depth of Z component
  SET(StencilBits, STENCIL_BITS)       // Set depth of stencil component
  SET(Multisamples, SAMPLES)           // Set anti-aliasing factor
  SET(AuxBuffers, AUX_BUFFERS)         // Set auxiliary buffer count
  SET(RefreshRate, REFRESH_RATE)       // Set desktop refresh rate
  SET(ClientAPI, CLIENT_API)           // Set client api to use
  SET(CtxMajor, CONTEXT_VERSION_MAJOR) // Set gl context major version
  SET(CtxMinor, CONTEXT_VERSION_MINOR) // Set gl context minor version
  SET(CoreProfile, OPENGL_PROFILE)     // Set gl profile to use
  SET(Robustness, CONTEXT_ROBUSTNESS)  // Set context robustness
  SET(Release, CONTEXT_RELEASE_BEHAVIOR) // Set context release behaviour
  /* ----------------------------------------------------------------------- */
#undef SET                             // Done with this macro
  /* -- Create functions to access all attributes -------------------------- */
#define SET(nc,nu) \
  /* ---------------------------------------------------------------------- */\
  void GlFWSet ## nc(const bool bState) \
    { GlFWSetHintBoolean(GLFW_ ## nu, bState); } \
  void GlFWSet ## nc ## Enabled(void) { GlFWSetHintEnabled(GLFW_ ## nu); } \
  void GlFWSet ## nc ## Disabled(void) { GlFWSetHintDisabled(GLFW_ ## nu); }
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
  SET(GPUSwitching, COCOA_GRAPHICS_SWITCHING) // Set graphics switching?
  SET(Iconify, ICONIFIED)              // Set window minimised state
  SET(Maximised, MAXIMIZED)            // Set window maximised state
  SET(MouseHovered, HOVERED)           // Set mouse hovered over state
  SET(NoErrors, CONTEXT_NO_ERROR)      // Set context no errors
  SET(Resizable, RESIZABLE)            // Set window resizable state
  SET(RetinaMode, COCOA_RETINA_FRAMEBUFFER) // Set retina framebuffer?
  SET(SRGBCapable, SRGB_CAPABLE)       // Set SRGB colour space capable
  SET(Stereo, CENTER_CURSOR)           // Set window cursor centre state
  SET(Transparency, TRANSPARENT_FRAMEBUFFER) // Set transparent framebuffer
  SET(Visibility, VISIBLE)             // Set window visibility state
  /* ----------------------------------------------------------------------- */
#undef SET                             // Done with this macro
  /* -- Default constructor ------------------------------------------------ */
  GlFWUtil(void) :
    /* -- Initialisers ----------------------------------------------------- */
    hsStrings{{                        // Initialise hint strings
      IDMAPSTR(GLFW_ACCUM_ALPHA_BITS), IDMAPSTR(GLFW_ACCUM_BLUE_BITS),
      IDMAPSTR(GLFW_ACCUM_GREEN_BITS), IDMAPSTR(GLFW_ACCUM_RED_BITS),
      IDMAPSTR(GLFW_ALPHA_BITS), IDMAPSTR(GLFW_AUTO_ICONIFY),
      IDMAPSTR(GLFW_AUX_BUFFERS), IDMAPSTR(GLFW_BLUE_BITS),
      IDMAPSTR(GLFW_CENTER_CURSOR), IDMAPSTR(GLFW_CLIENT_API),
      IDMAPSTR(GLFW_COCOA_FRAME_NAME), IDMAPSTR(GLFW_COCOA_GRAPHICS_SWITCHING),
      IDMAPSTR(GLFW_COCOA_RETINA_FRAMEBUFFER),
      IDMAPSTR(GLFW_CONTEXT_CREATION_API), IDMAPSTR(GLFW_CONTEXT_NO_ERROR),
      IDMAPSTR(GLFW_CONTEXT_RELEASE_BEHAVIOR), IDMAPSTR(GLFW_CONTEXT_REVISION),
      IDMAPSTR(GLFW_CONTEXT_ROBUSTNESS), IDMAPSTR(GLFW_CONTEXT_VERSION_MAJOR),
      IDMAPSTR(GLFW_CONTEXT_VERSION_MINOR), IDMAPSTR(GLFW_DECORATED),
      IDMAPSTR(GLFW_DEPTH_BITS), IDMAPSTR(GLFW_DOUBLEBUFFER),
      IDMAPSTR(GLFW_FLOATING), IDMAPSTR(GLFW_FOCUS_ON_SHOW),
      IDMAPSTR(GLFW_FOCUSED), IDMAPSTR(GLFW_GREEN_BITS),
      IDMAPSTR(GLFW_HOVERED), IDMAPSTR(GLFW_ICONIFIED),
      IDMAPSTR(GLFW_MAXIMIZED), IDMAPSTR(GLFW_OPENGL_DEBUG_CONTEXT),
      IDMAPSTR(GLFW_OPENGL_FORWARD_COMPAT), IDMAPSTR(GLFW_OPENGL_PROFILE),
      IDMAPSTR(GLFW_RED_BITS), IDMAPSTR(GLFW_REFRESH_RATE),
      IDMAPSTR(GLFW_RESIZABLE), IDMAPSTR(GLFW_SAMPLES),
      IDMAPSTR(GLFW_SCALE_TO_MONITOR), IDMAPSTR(GLFW_SRGB_CAPABLE),
      IDMAPSTR(GLFW_STENCIL_BITS), IDMAPSTR(GLFW_STEREO),
      IDMAPSTR(GLFW_TRANSPARENT_FRAMEBUFFER), IDMAPSTR(GLFW_VISIBLE),
      IDMAPSTR(GLFW_X11_CLASS_NAME), IDMAPSTR(GLFW_X11_INSTANCE_NAME),
      IDMAPSTR(GLFW_CONNECTED), IDMAPSTR(GLFW_DISCONNECTED),
    }, "GLFW_UNKNOWN" }                // End of initialisation of hint strings
  /* -- No code ------------------------------------------------------------ */
  { }
};/* ----------------------------------------------------------------------- */
/* -- Get window data pointer ---------------------------------------------- */
template<typename AnyCast=void*>
  static AnyCast GlFWGetWindowUserPointer(GLFWwindow*const wC)
    { return reinterpret_cast<AnyCast>(glfwGetWindowUserPointer(wC)); }
/* -- Set window data pointer ---------------------------------------------- */
template<typename AnyCast=void*const>
  static void GlFWSetWindowUserPointer(GLFWwindow*const wC, AnyCast acData)
    { glfwSetWindowUserPointer(wC, reinterpret_cast<void*>(acData)); }
/* ------------------------------------------------------------------------- */
static void GlFWForceEventHack(void) { glfwPostEmptyEvent(); }
/* -- Joystick axes -------------------------------------------------------- */
static const float *GlFWGetJoystickAxes(int iJ, int &iJAC)
  { return glfwGetJoystickAxes(iJ, &iJAC); }
/* -- Joystick buttons ----------------------------------------------------- */
static const unsigned char *GlFWGetJoystickButtons(int iJ, int &iJAB)
  { return glfwGetJoystickButtons(iJ, &iJAB); }
/* -- Joystick other ------------------------------------------------------- */
static const char *GlFWGetJoystickName(const int iJ)
  { return glfwGetJoystickName(iJ); }
/* -- Joystick is actually a game controller ------------------------------- */
static bool GlFWJoystickIsGamepad(const int iJ)
  { return glfwJoystickIsGamepad(iJ); }
/* -- Get gamepad name ----------------------------------------------------- */
static const char *GlFWGetGamepadName(const int iJ)
  { return glfwGetGamepadName(iJ); }
/* -- Get joystick unique identification number ---------------------------- */
static const char *GlFWGetJoystickGUID(const int iJ)
  { return glfwGetJoystickGUID(iJ); }
/* -- Return if joystick is present ---------------------------------------- */
static bool GlFWJoystickPresent(const int iJ)
  { return glfwJoystickPresent(iJ); }
/* -- Set joystick callback ------------------------------------------------ */
static GLFWjoystickfun GlFWSetJoystickCallback(GLFWjoystickfun gjfCb)
  { return glfwSetJoystickCallback(gjfCb); }
/* -- Set monitor change callback ------------------------------------------ */
static GLFWmonitorfun GlFWSetMonitorCallback(GLFWmonitorfun gmfCb)
  { return glfwSetMonitorCallback(gmfCb); }
/* -- Set swap interval ---------------------------------------------------- */
static void GlFWSetVSync(const int iI) { glfwSwapInterval(iI); }
/* -- Wait for window event ------------------------------------------------ */
static void GlFWWaitEvents(void) { glfwWaitEvents(); }
/* -- Release current context ---------------------------------------------- */
static void GlFWSetContext(GLFWwindow*const wWindow=nullptr)
  { glfwMakeContextCurrent(wWindow); }
/* -- Set gamma ------------------------------------------------------------ */
static void GlFWSetGamma(GLFWmonitor*const mDevice, const GLfloat fValue)
  { glfwSetGamma(mDevice, fValue); }
/* -- Get function address-------------------------------------------------- */
static bool GlFWProcExists(const char*const cpFunction)
  { return !!glfwGetProcAddress(cpFunction); }
/* -- Get internal name of key --------------------------------------------- */
static const char *GlFWGetKeyName(const int iK, const int iSC)
  { return glfwGetKeyName(iK, iSC); }
/* -- Return if raw mouse is supported? ------------------------------------ */
static bool GlFWIsRawMouseMotionSupported(void)
  { return GlFWGBooleanToBoolean(glfwRawMouseMotionSupported()); }
/* -- Return monitor name -------------------------------------------------- */
static const char *GlFWGetMonitorName(GLFWmonitor*const mDevice)
  { return glfwGetMonitorName(mDevice); }
/* -- Is the context not set ----------------------------------------------- */
static GLFWwindow *GlFWContext(void) { return glfwGetCurrentContext(); }
/* ------------------------------------------------------------------------- */
static void GlFWSetCursor(GLFWwindow*const gwCtx, GLFWcursor*const gcCtx)
  { glfwSetCursor(gwCtx, gcCtx); }
/* ------------------------------------------------------------------------- */
static GLFWglproc GlFWGetProcAddress(const char*const cpFunction)
{ // Get function and return it if we have it
  if(GLFWglproc fCB = glfwGetProcAddress(cpFunction)) return fCB;
  // No function so return error
  XC("The specified OpenGL ICD function could not be found from your video "
      "card manufacturers display driver. Try upgrading your graphics "
      "drivers to the latest version from your manufacturers or OEM "
      "website and make sure your operating system is up to date. If all "
      "else fails, you will need to upgrade your graphics hardware!",
      "Function", cpFunction);
}
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
