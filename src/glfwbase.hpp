/* == GLFWBASE.HPP ========================================================= **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## These are all wrapper functions we use to interface with GLFW       ## **
** ## itself. Keep all other 'helper' functions in 'glfwutil.hpp'. We do  ## **
** ## this incase the GLFW developers decide to change something and      ## **
** ## hopefully we just have to change this module. We also add fancy C++ ## **
** ## templates to make them easier to use. Note that with compiler       ## **
** ## optimisations enabled. This namespace and all the functions in it   ## **
** ## should be optimised out.                                            ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IGlFWBase {                  // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace Lib::OS::GlFW;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* -- Get monitor data pointer --------------------------------------------- */
template<typename AnyType = void*>
  requires StdIsPointer<AnyType>
static AnyType GlFWGetMonitorUserPointer(GLFWmonitor*const mC)
  { return reinterpret_cast<AnyType>(glfwGetMonitorUserPointer(mC)); }
/* -- Get window data pointer ---------------------------------------------- */
template<typename AnyType = void*>
  requires StdIsPointer<AnyType>
static AnyType GlFWGetWindowUserPointer(GLFWwindow*const wC)
  { return reinterpret_cast<AnyType>(glfwGetWindowUserPointer(wC)); }
/* -- Set window data pointer ---------------------------------------------- */
template<typename AnyType = void*const>
  requires StdIsPointer<AnyType>
static void GlFWSetWindowUserPointer(GLFWwindow*const wC, AnyType acData)
  { glfwSetWindowUserPointer(wC, reinterpret_cast<void*>(acData)); }
/* ------------------------------------------------------------------------- */
static void GlFWForceEventHack() { glfwPostEmptyEvent(); }
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
static void GlFWWaitEvents() { glfwWaitEvents(); }
/* -- Release current context ---------------------------------------------- */
static void GlFWSetContext(GLFWwindow*const wWindow=nullptr)
  { glfwMakeContextCurrent(wWindow); }
/* -- Set gamma ------------------------------------------------------------ */
static void GlFWSetGamma(GLFWmonitor*const mDevice, const GLfloat fValue)
  { glfwSetGamma(mDevice, fValue); }
/* -- Get function address-------------------------------------------------- */
static bool GlFWProcExists(const char*const cpFunction)
  { return glfwGetProcAddress(cpFunction) != nullptr; }
/* -- Get internal name of key --------------------------------------------- */
static const char *GlFWGetKeyName(const int iK, const int iSC)
  { return glfwGetKeyName(iK, iSC); }
/* -- Return monitor name -------------------------------------------------- */
static const char *GlFWGetMonitorName(GLFWmonitor*const mDevice)
  { return glfwGetMonitorName(mDevice); }
/* -- Is the context not set ----------------------------------------------- */
static GLFWwindow *GlFWContext() { return glfwGetCurrentContext(); }
/* ------------------------------------------------------------------------- */
static void GlFWSetCursor(GLFWwindow*const gwCtx, GLFWcursor*const gcCtx)
  { glfwSetCursor(gwCtx, gcCtx); }
/* ------------------------------------------------------------------------- */
static GLFWglproc GlFWGetProcAddress(const char*const cpFunction)
  { return glfwGetProcAddress(cpFunction); }
/* ------------------------------------------------------------------------- */
static void GlFWWindowHintString(const int iHint, const char*const cpValue)
  { glfwWindowHintString(iHint, cpValue); }
/* ------------------------------------------------------------------------- */
static void GlFWWindowHint(const int iHint, const int iValue)
  { glfwWindowHint(iHint, iValue); }
/* -- Returns true if code is GLFW_TRUE, false if not ---------------------- */
static bool GlFWGBooleanToBoolean(const int iCode)
  { return iCode == GLFW_TRUE; }
/* -- Return if raw mouse is supported? ------------------------------------ */
static bool GlFWBaseRawMouseMotionSupported()
  { return GlFWGBooleanToBoolean(glfwRawMouseMotionSupported()); }
/* -- Initialise glfw and return on success? ------------------------------- */
static bool GlFWBaseInit() { return GlFWGBooleanToBoolean(glfwInit()); }
/* -- Is OpenGL extension supported? --------------------------------------- */
static bool GlFWBaseExtensionSupported(const char*const cpName)
  { return GlFWGBooleanToBoolean(glfwExtensionSupported(cpName)); }
/* -- Set window attribute ------------------------------------------------- */
static bool GlFWBaseGetWindowAttrib(GLFWwindow*const glfwWindow,
  const int iAttribute)
{ return GlFWGBooleanToBoolean(glfwGetWindowAttrib(glfwWindow, iAttribute)); }
/* -- Create window -------------------------------------------------------- */
static GLFWwindow *GlFWBaseCreateWindow(const	int iWidth, const int iHeight,
  const char*const cpTitle,	GLFWmonitor*const glfwmMonitor = nullptr,
  GLFWwindow*const glfwwShare = nullptr)
{ return glfwCreateWindow(iWidth, iHeight, cpTitle,
    glfwmMonitor, glfwwShare); }
/* -- Destroy window ------------------------------------------------------- */
static void GlFWBaseDestroyWindow(GLFWwindow*const glfwwWindow)
  { return glfwDestroyWindow(glfwwWindow); }
/* -- Returns a GLFW return type based on specified boolean ---------------- */
static int GlFWBaseBooleanToGBoolean(const bool bCondition)
  { return bCondition ? GLFW_TRUE : GLFW_FALSE; }
/* -- Get mouse buttons tatus----------------------------------------------- */
static int GlFWGetMouseButton(GLFWwindow*const glfwwWindow, const int iButton)
  { return glfwGetMouseButton(glfwwWindow, iButton); }
/* -- Get keyboard input mode ---------------------------------------------- */
static bool GlFWGetInputMode(GLFWwindow*const glfwwWindow, const int iMode)
  { return GlFWGBooleanToBoolean(glfwGetInputMode(glfwwWindow, iMode)); }
/* -- Set keyboard input mode ---------------------------------------------- */
static void GlFWSetInputMode(GLFWwindow*const glfwwWindow, const int iMode,
  const bool bEnabled)
{ glfwSetInputMode(glfwwWindow, iMode, GlFWBaseBooleanToGBoolean(bEnabled)); }
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
