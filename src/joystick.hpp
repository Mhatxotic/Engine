/* == JOYSTICK.HPP ========================================================= **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## This module defines classes for setting and accessing joystick data ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IJoystick {                  // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace ICVarDef::P;           using namespace IEvtMain::P;
using namespace IEvtWin::P;            using namespace IFlags;
using namespace IGlFWUtil::P;          using namespace IJoyInfo::P;
using namespace ILog::P;               using namespace ILuaFunc::P;
using namespace IStd::P;               using namespace IUtil::P;
using namespace Lib::OS::GlFW::Types;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* -- Public typedefs ------------------------------------------------------ */
BUILD_FLAGS(Input,                     // Input flags
  /* ----------------------------------------------------------------------- */
  IF_NONE                   {Flag(0)}, // No flags
  IF_CURSOR                 {Flag(1)}, // Mouse cursor is enabled?
  IF_FSTOGGLER              {Flag(2)}, // Full-screen toggler enabled?
  IF_MOUSEFOCUS             {Flag(3)}, // Mouse cursor has focus?
  IF_INITEVENTS             {Flag(4)}, // Send events at startup?
  IF_POLLJOYSTICKS          {Flag(5)}  // Do joystick polling?
);/* ----------------------------------------------------------------------- */
/* ------------------------------------------------------------------------- */
class Joystick :
  /* -- Base classes ------------------------------------------------------- */
  public JoyList                       // Joystick list data classes
{ /* -- Variables ---------------------------------------------------------- */
  bool             bPoll;              // Poll joysticks?
  LuaFunc          lfOnJoyState;       // Joystick lua event
  size_t           stConnected;        // Joysticks connected
  /* -- Event handler for 'glfwSetJoystickCallback' ------------------------ */
  static void JoyOnGamePad(int iJId, int iEvent)
    { cEvtMain->Add(EMC_INP_JOYSTATE, iJId, iEvent); }
  /* -- Enable or disable joystick polling --------------------------------- */
  void JoyEnablePoll() { bPoll = true; }
  void JoyDisablePoll() { bPoll = false; }
  /* -- Dispatch connected event to lua ------------------------------------ */
  void JoyDispatchLuaEvent(const size_t stJoystickId, const bool bConnected)
    { lfOnJoyState.LuaFuncDispatch(static_cast<lua_Integer>(stJoystickId),
        bConnected); }
  /* -- Joystick state changed ------------------------------------- */ public:
  void JoyOnState(const EvtMainEvent &emeEvent)
  { // Get reference to actual arguments vector
    const EvtMainArgs &emaArgs = emeEvent.eaArgs;
    // Get joystick id as int
    const int iJId = emaArgs[0].Int(), iEvent = emaArgs[1].Int();
    // Show warning if joystick id is out of range
    if(static_cast<size_t>(iJId) >= JoyGetCount())
      return cLog->LogWarningExSafe(
        "Joystick event $ for id $ is out of range!", iEvent, iJId);
    // What happened to the joystick?
    switch(iEvent)
    { // Connected?
      case GLFW_CONNECTED:
        // Increase connected count and enable polling if the first
        if(++stConnected == 1) JoyEnablePoll();
        // Setup the joystick state and return
        return JoySetupDispatch(static_cast<size_t>(iJId));
      // Disconnected?
      case GLFW_DISCONNECTED:
        // Decrease connected count and disable polling if the last
        if(!--stConnected) JoyDisablePoll();
        // Clear the joystick state and return
        return JoyClearDispatch(static_cast<size_t>(iJId));
      // Invalid code?
      default:
        // Log the bad joystick state and return
        cLog->LogWarningExSafe("Joystick ignored bad state $$ for id $$!",
          hex, iEvent, dec, iJId);
        // No need to dispatch any events
        return;
    }
  }
  /* -- Poll joysticks if enabled ------------------------------------------ */
  void JoyPoll()
    { if(bPoll)
        for(JoyInfo &jiRef : *this) jiRef.JoyRefreshDataIfConnected(); }
  /* -- Return event ------------------------------------------------------- */
  LuaFunc &JoyGetLuaEvent() { return lfOnJoyState; }
  /* -- Return writable joystick data -------------------------------------- */
  JoyInfo &JoyGet(const size_t stJId) { return (*this)[stJId]; }
  /* -- Return read-only joystick data ------------------------------------- */
  const JoyInfo &JoyGetConst(const size_t stJId) const
    { return (*this)[stJId]; }
  /* -- Get read-only acess to joystick list ------------------------------- */
  const JoyList &JoyListGetConst() const { return *this; }
  /* -- Get connected count ----------------------------------------------- */
  size_t JoyGetConnected() const { return stConnected; }
  /* -- Return joysticks count --------------------------------------------- */
  size_t JoyGetCount() const { return JoyListGetConst().size(); }
  /* -- Return if joystick is connected ------------------------------------ */
  bool JoyIsConnected(const size_t stId)
    { return JoyGetConst(stId).JoyIsConnected(); }
  /* -- Clear buttons and axis state --------------------------------------- */
  void JoyClearStates()
    { StdForEach(par_unseq, this->begin(), this->end(),
        [](JoyInfo &jiRef) { jiRef.JoyClearButtonStateIfConnected(); }); }
  /* -- DeInitialise a joystick -------------------------------------------- */
  void JoyClearDispatch(const size_t stJoystickId)
  { // Get joystick data and ign ore if joystick wasn't originally connected
    JoyInfo &jiRef = JoyGet(stJoystickId);
    if(jiRef.JoyIsDisconnected()) return;
    // Send lua event to let guest know joystick was disconnected
    JoyDispatchLuaEvent(stJoystickId, false);
    // Clear joystick, axis and button data
    jiRef.JoyDisconnect();
    jiRef.JoyClearState();
  }
  /* -- Initialise a new joystick ------------------------------------------ */
  void JoySetupDispatch(const size_t stJoystickId)
  { // Get joystick data and ign ore if joystick wasn't originally connected
    JoyInfo &jiRef = JoyGet(stJoystickId);
    if(jiRef.JoyIsConnected()) return;
    // Begin detection and refresh data
    jiRef.JoyConnect();
    jiRef.JoyRefreshData();
    // Send lua event to let guest know joystick was connected and return
    JoyDispatchLuaEvent(stJoystickId, true);
  }
  /* -- Return a joystick is present? -------------------------------------- */
  void JoyDetect()
  { // Reset connected count
    stConnected = 0;
    // Enumerate joysticks and if joystick is present?
    for(JoyInfo &jiRef : *this)
      // If joystick is present?
      if(jiRef.JoyIsPresent())
      { // Send event that the joystick was connected
        JoySetupDispatch(static_cast<size_t>(jiRef.JoyGetId()));
        // Increase connected count
        ++stConnected;
      } // Send event that the joystick was disconnected
      else JoyClearDispatch(static_cast<size_t>(jiRef.JoyGetId()));
    // If we did not find joysticks?
    if(!JoyGetConnected())
    { // Disable polling to not waste CPU cycles
      JoyDisablePoll();
      // Log no controllers
      return cLog->LogDebugSafe("Joystick detected no controller devices.");
    } // Enable polling
    JoyEnablePoll();
    // Log result
    cLog->LogDebugExSafe(
      "Joystick enabling joystick polling as $ devices are detected.",
      JoyGetConnected());
  }
  /* -- Reset environment -------------------------------------------------- */
  void JoyReset()
  { // For each joystick
    for(JoyInfo &jiRef : *this)
    { // Clear the connected flag and clear the state
      jiRef.JoyDoDisconnect();
      jiRef.JoyClearState();
    } // No devices connected until the joystick event is set again.
    stConnected = 0;
    // Disable polling
    JoyDisablePoll();
  }
  /* -- Init/DeInit joystick callback -------------------------------------- */
  void JoyInit() const { GlFWSetJoystickCallback(JoyOnGamePad); }
  void JoyDeInit() const { GlFWSetJoystickCallback(nullptr); }
  /* -- Constructor --------------------------------------------- */ protected:
  Joystick() :
    /* -- Initialisers ----------------------------------------------------- */
    JoyList{ UtilMkFilledClassContainer<JoyList,int>() },
    bPoll(false),                      // Init disabled polling
    lfOnJoyState{ "OnJoyState" },      // Init joy state lua event
    stConnected(0)                     // Init joystick count to zero
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- Destructor --------------------------------------------------------- */
  ~Joystick() { JoyDeInit(); }
  /* -- Handle a deadzone change ----------------------------------- */ public:
  CVarReturn JoySetDefaultJoyDZ(const float fDZ,
    const function<void(JoyInfo&)> &fcbCallBack)
  { // Return if invalid deadzone else set it and return success
    if(fDZ > 1) return DENY;
    StdForEach(par_unseq, this->begin(), this->end(), fcbCallBack);
    return ACCEPT;
  }
  /* -- Set default negative deadzone -------------------------------------- */
  CVarReturn JoySetDefaultJoyRevDZ(const float fNewDeadZone)
    { return JoySetDefaultJoyDZ(fNewDeadZone, [fNewDeadZone](JoyInfo &jiRef)
        { jiRef.JoyAxisListSetReverseDeadZone(fNewDeadZone); }); }
  /* -- Set default positive deadzone -------------------------------------- */
  CVarReturn JoySetDefaultJoyFwdDZ(const float fNewDeadZone)
    { return JoySetDefaultJoyDZ(fNewDeadZone, [fNewDeadZone](JoyInfo &jiRef)
        { jiRef.JoyAxisListSetForwardDeadZone(fNewDeadZone); }); }
};/* ----------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
