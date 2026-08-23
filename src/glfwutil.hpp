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
/* -- Dependencies --------------------------------------------------------- */
using namespace IError::P;             using namespace IEvtMain::P;
using namespace IGlFWBase::P;          using namespace ILog::P;
using namespace ILookupMap::P;         using namespace Lib::OS::GlFW;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* ------------------------------------------------------------------------- */
class GlFWUtil                         // Members initially private
{ /* -- Private typedefs --------------------------------------------------- */
  using HintList = LookupMap<const int>; // List of glfw hints
  const HintList   hsStrings;            // Hint strings
  /* -- Convert window hint id to string --------------------------- */ public:
  const StdStringView &GlFWGetHintAttribStr(const int iTarget) const
    { return hsStrings.Get(iTarget); }
  /* -- Set window hint ---------------------------------------------------- */
  void GlFWSetHint(const int iVar, const int iVal)
  { // Set window hint directly
    GlFWWindowHint(iVar, iVal);
    // Log the change
    cLog->LogDebugExSafe("GlFW set hint $<0x$$> to $$<0x$$>.",
      GlFWGetHintAttribStr(iVar), StdIOSHex, iVar, StdIOSDec, iVal, StdIOSHex,
      iVal);
  }
  /* -- Set window hint core functions ------------------------------------- */
  void GlFWSetHintBoolean(const int iVar, const bool bVal)
    { GlFWSetHint(iVar, GlFWBaseBooleanToGBoolean(bVal)); }
  void GlFWSetHintEnabled(const int iVar)
    { GlFWSetHintBoolean(iVar, true); }
  void GlFWSetHintDisabled(const int iVar)
    { GlFWSetHintBoolean(iVar, false); }
  /* -- Set window hint string --------------------------------------------- */
  void GlFWSetHintString(const int iHint, const char*const cpValue)
  { // Set window hint directly
    GlFWWindowHintString(iHint, cpValue);
    // Log the change
    cLog->LogDebugExSafe("GlFW set hint $<0x$$> to '$'.",
      GlFWGetHintAttribStr(iHint), StdIOSHex, iHint, cpValue);
  }
  /* -- OS specific routines ----------------------------------------------- */
#if defined(MACOS)                     // Targeting MacOS?
  /* -- Set frame name in MacOS -------------------------------------------- */
  void GlFWSetCocoaFrameName(const char*const cpName)
    { GlFWSetHintString(GLFW_COCOA_FRAME_NAME, cpName); }
  /* ----------------------------------------------------------------------- */
#elif defined(LINUX)                   // Targeting Linux?
  /* -- Set class name in X11 ---------------------------------------------- */
  void GlFWSetX11ClassName(const char*const cpName)
    { GlFWSetHintString(GLFW_X11_CLASS_NAME, cpName); }
  /* -- Set instance name in X11 ------------------------------------------- */
  void GlFWSetX11InstanceName(const char*const cpName)
    { GlFWSetHintString(GLFW_X11_INSTANCE_NAME, cpName); }
  /* ----------------------------------------------------------------------- */
#endif                                 // End of target checks
  /* -- Set window frame names --------------------------------------------- */
  void GlFWSetFrameName(const char*const cpName)
  { // Set custom frame names based on operating system
#if defined(MACOS)
    GlFWSetCocoaFrameName(cpName);
#elif defined(LINUX)
    GlFWSetX11ClassName(cpName);
    GlFWSetX11InstanceName(cpName);
#else
    static_cast<void>(cpName);
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
  SET(PositionX, POSITION_X)           // Set X position of window
  SET(PositionY, POSITION_Y)           // Set Y position of window
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
  void GlFWSet ## nc ## Enabled() { GlFWSetHintEnabled(GLFW_ ## nu); } \
  void GlFWSet ## nc ## Disabled() { GlFWSetHintDisabled(GLFW_ ## nu); }
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
  SET(MousePassthrough, MOUSE_PASSTHROUGH) // Set mouse passthrough
  SET(NoErrors, CONTEXT_NO_ERROR)      // Set context no errors
  SET(Resizable, RESIZABLE)            // Set window resizable state
  SET(RetinaMode, SCALE_FRAMEBUFFER)   // Set frame buffer scale
  SET(ScaleMonitor, SCALE_TO_MONITOR)  // Set scale window to monitor
  SET(SRGBCapable, SRGB_CAPABLE)       // Set SRGB colour space capable
  SET(Stereo, CENTER_CURSOR)           // Set window cursor centre state
  SET(Transparency, TRANSPARENT_FRAMEBUFFER) // Set transparent framebuffer
  SET(Visibility, VISIBLE)             // Set window visibility state
  /* ----------------------------------------------------------------------- */
#undef SET                             // Done with this macro
  /* -- Default constructor ------------------------------------------------ */
  GlFWUtil() :
    /* -- Initialisers ----------------------------------------------------- */
    hsStrings{{                        // Initialise hint strings
      IDMAPSTR(GLFW_ACCUM_ALPHA_BITS), IDMAPSTR(GLFW_ACCUM_BLUE_BITS),
      IDMAPSTR(GLFW_ACCUM_GREEN_BITS), IDMAPSTR(GLFW_ACCUM_RED_BITS),
      IDMAPSTR(GLFW_ALPHA_BITS), IDMAPSTR(GLFW_AUTO_ICONIFY),
      IDMAPSTR(GLFW_AUX_BUFFERS), IDMAPSTR(GLFW_BLUE_BITS),
      IDMAPSTR(GLFW_CENTER_CURSOR), IDMAPSTR(GLFW_CLIENT_API),
      IDMAPSTR(GLFW_COCOA_FRAME_NAME), IDMAPSTR(GLFW_COCOA_GRAPHICS_SWITCHING),
      IDMAPSTR(GLFW_CONTEXT_CREATION_API), IDMAPSTR(GLFW_CONTEXT_NO_ERROR),
      IDMAPSTR(GLFW_CONTEXT_RELEASE_BEHAVIOR), IDMAPSTR(GLFW_CONTEXT_REVISION),
      IDMAPSTR(GLFW_CONTEXT_ROBUSTNESS), IDMAPSTR(GLFW_CONTEXT_VERSION_MAJOR),
      IDMAPSTR(GLFW_CONTEXT_VERSION_MINOR), IDMAPSTR(GLFW_DECORATED),
      IDMAPSTR(GLFW_DEPTH_BITS), IDMAPSTR(GLFW_DOUBLEBUFFER),
      IDMAPSTR(GLFW_FLOATING), IDMAPSTR(GLFW_FOCUS_ON_SHOW),
      IDMAPSTR(GLFW_FOCUSED), IDMAPSTR(GLFW_GREEN_BITS),
      IDMAPSTR(GLFW_HOVERED), IDMAPSTR(GLFW_ICONIFIED),
      IDMAPSTR(GLFW_MAXIMIZED), IDMAPSTR(GLFW_MOUSE_PASSTHROUGH),
      IDMAPSTR(GLFW_OPENGL_DEBUG_CONTEXT),
      IDMAPSTR(GLFW_OPENGL_FORWARD_COMPAT), IDMAPSTR(GLFW_OPENGL_PROFILE),
      IDMAPSTR(GLFW_POSITION_X), IDMAPSTR(GLFW_POSITION_Y),
      IDMAPSTR(GLFW_RED_BITS), IDMAPSTR(GLFW_REFRESH_RATE),
      IDMAPSTR(GLFW_RESIZABLE), IDMAPSTR(GLFW_SAMPLES),
      IDMAPSTR(GLFW_SCALE_FRAMEBUFFER), IDMAPSTR(GLFW_SCALE_TO_MONITOR),
      IDMAPSTR(GLFW_SRGB_CAPABLE), IDMAPSTR(GLFW_STENCIL_BITS),
      IDMAPSTR(GLFW_STEREO), IDMAPSTR(GLFW_TRANSPARENT_FRAMEBUFFER),
      IDMAPSTR(GLFW_VISIBLE), IDMAPSTR(GLFW_X11_CLASS_NAME),
      IDMAPSTR(GLFW_X11_INSTANCE_NAME), IDMAPSTR(GLFW_CONNECTED),
      IDMAPSTR(GLFW_DISCONNECTED),
    }, "GLFW_UNKNOWN" }                // End of initialisation of hint strings
  /* -- No code ------------------------------------------------------------ */
  {}
};/* ----------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
