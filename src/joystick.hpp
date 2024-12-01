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
using namespace IGlFWUtil::P;          using namespace IIdent::P;
using namespace ILog::P;               using namespace ILuaFunc::P;
using namespace IStd::P;               using namespace IString::P;
using namespace Lib::OS::GlFW;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* == Input flags ========================================================== */
BUILD_FLAGS(Input,
  /* ----------------------------------------------------------------------- */
  // No flags                          Mouse cursor is enabled?
  IF_NONE                   {Flag[0]}, IF_CURSOR                 {Flag[1]},
  // Full-screen toggler enabled?      Mouse cursor has focus?
  IF_FSTOGGLER              {Flag[2]}, IF_MOUSEFOCUS             {Flag[3]},
  // Send events at startup            Do joystick polling?
  IF_INITEVENTS             {Flag[4]}, IF_POLLJOYSTICKS          {Flag[5]}
);/* -- Axis class --------------------------------------------------------- */
class JoyAxisInfo
{ /* -------------------------------------------------------------- */ private:
  const int        iId;                // Axis unique identifier
  float            fDeadZoneR;         // Reverse deadzone threshold
  float            fDeadZoneF;         // Forward deadzone threshold
  float            fUnbuffered;        // Current unbuffered axis press state
  int              iBuffered;          // Current buffered axis press state
  /* -- Get axis identifier ---------------------------------------- */ public:
  int AxisGetId(void) const { return iId; }
  /* -- Get reverse deadzone value ----------------------------------------- */
  float AxisGetReverseDeadZone(void) const { return fDeadZoneR; }
  /* -- Get forward deadzone value ----------------------------------------- */
  float AxisGetForwardDeadZone(void) const { return fDeadZoneF; }
  /* -- Set reverse deadzone value ----------------------------------------- */
  void AxisSetReverseDeadZone(const float fDZ) { fDeadZoneR = -fDZ; }
  /* -- Set forward deadzone value ----------------------------------------- */
  void AxisSetForwardDeadZone(const float fDZ) { fDeadZoneF = fDZ; }
  /* -- Get unbuffered state ----------------------------------------------- */
  float AxisGetUnbufferedState(void) const { return fUnbuffered; }
  /* -- Get buffered state ------------------------------------------------- */
  int AxisGetBufferedState(void) const { return iBuffered; }
  /* -- Clear button state ------------------------------------------------- */
  void AxisClearState(void) { fUnbuffered = 0.0f; iBuffered = GLFW_RELEASE; }
  /* -- Set axis state ----------------------------------------------------- */
  void AxisSetState(const float*const fpData)
  { // Get the axis reading and if it's moving negatively?
    fUnbuffered = fpData[AxisGetId()];
    if(fUnbuffered < fDeadZoneR)
    { // Released or already in reverse position?
      if(iBuffered <= GLFW_RELEASE)
      { // Set to -GLFW_REPEAT if currently -GLFW_PRESSED or
        //        -GLFW_PRESSED if currently GLFW_RELEASE
        if(iBuffered > -GLFW_REPEAT) --iBuffered;
        // State changed
        return;
      }
    } // Moving positively?
    else if(fUnbuffered > fDeadZoneF)
    { // Released or already in forward position?
      if(iBuffered >= GLFW_RELEASE)
      { // Set to GLFW_REPEAT if currentl GLFW_PRESSED or
        //        GLFW_PRESSED if currently GLFW_RELEASE
        if(iBuffered < GLFW_REPEAT) ++iBuffered;
        // State changed
        return;
      }
    } // Nothing pressed so reset buffered counter
    iBuffered = GLFW_RELEASE;
  }
  /* -- Constructor with unique id ----------------------------------------- */
  explicit JoyAxisInfo(const int iNId) :
    /* -- Initialisers ----------------------------------------------------- */
    iId(iNId),                         // Set unique id
    fDeadZoneR(-0.25f),                // Set default reverse deadzone
    fDeadZoneF(0.25f),                 // Set default forward deadzone
    fUnbuffered(0.0f),                 // Set default unbuffered state
    iBuffered(GLFW_RELEASE)            // Set default state
    /* -- No code ---------------------------------------------------------- */
    { }
};/* -- Axis data list type ------------------------------------------------ */
template<class Container = array<JoyAxisInfo, 8>,
         class Iterator = typename Container::const_iterator>
  class JoyAxisList : private Container
{ /* -------------------------------------------------------------- */ private:
  int              iAxes;              // Button count as int
  /* -- Return axes count clamped ------------------------------------------ */
  int JoyAxisListSizeCountClamped(void) const { return iAxes & 0x7f; }
  /* -- Return button class ------------------------------------------------ */
  JoyAxisInfo &JoyAxisListGetMod(const size_t stId) { return (*this)[stId]; }
  /* -- Return axes array size ------------------------------------- */ public:
  size_t JoyAxisListSize(void) const { return this->size(); }
  /* -- Return axes count -------------------------------------------------- */
  size_t JoyAxisListCount(void) const { return static_cast<size_t>(iAxes); }
  /* -- Return button class ------------------------------------------------ */
  const JoyAxisInfo &JoyAxisListGet(const size_t stId) const
    { return (*this)[stId]; }
  /* -- Clear button data -------------------------------------------------- */
  void JoyAxisListClear(void)
    { StdForEach(par_unseq, this->begin(),
        this->begin() + JoyAxisListSizeCountClamped(),
          [](JoyAxisInfo& jaiRef) { jaiRef.AxisClearState(); }); }
  /* -- Refresh button data ------------------------------------------------ */
  void JoyAxisListRefresh(const int iJId)
  { // Get joystick axes and if found? Refresh axes data for each one
    if(const float *fpData = GlFWGetJoystickAxes(iJId, iAxes))
      StdForEach(seq, this->begin(),
        this->begin() + JoyAxisListSizeCountClamped(),
      [fpData](JoyAxisInfo &jaiData){ jaiData.AxisSetState(fpData); });
    else iAxes = 0;
  }
  /* -- Get axis state ----------------------------------------------------- */
  void JoyAxisListSetForwardDeadZone(const size_t stAxisId, const float fDZ)
    { JoyAxisListGetMod(stAxisId).AxisSetForwardDeadZone(fDZ); }
  void JoyAxisListSetReverseDeadZone(const size_t stAxisId, const float fDZ)
    { JoyAxisListGetMod(stAxisId).AxisSetReverseDeadZone(fDZ); }
  void JoyAxisListSetDeadZones(const size_t stAxisId,
    const float fFDZ, const float fRDZ)
  { // Get axis data and set the new values
    JoyAxisInfo &jaiItem = JoyAxisListGetMod(stAxisId);
    jaiItem.AxisSetForwardDeadZone(fFDZ);
    jaiItem.AxisSetReverseDeadZone(fRDZ);
  }
  /* -- Set default positive deadzone -------------------------------------- */
  void JoyAxisListSetReverseDeadZone(const float fDZ)
    { StdForEach(par_unseq, this->begin(),
        this->end() + JoyAxisListSizeCountClamped(),
          [fDZ](JoyAxisInfo &jaiItem)
            { jaiItem.AxisSetReverseDeadZone(fDZ); }); }
  /* -- Set default positive deadzone -------------------------------------- */
  void JoyAxisListSetForwardDeadZone(const float fDZ)
    { StdForEach(par_unseq, this->begin(),
        this->end() + JoyAxisListSizeCountClamped(),
          [fDZ](JoyAxisInfo &jaiItem)
            { jaiItem.AxisSetForwardDeadZone(fDZ); }); }
  /* -- Iterators ---------------------------------------------------------- */
  Iterator JoyAxisListBegin(void) const
    { return this->cbegin(); }
  Iterator JoyAxisListEnd(void) const
    { return this->cbegin() + JoyAxisListSizeCountClamped(); }
  /* -- Default constructor ------------------------------------- */ protected:
  JoyAxisList(void) :
    /* -- Initialise joystick buttons -------------------------------------- */
#define JAIX(x) JoyAxisInfo{ x }
#define JAI(x) JAIX(GLFW_GAMEPAD_AXIS_ ## x)
    /* --------------------------------------------------------------------- */
    Container{{                        // Initialise joystick axes ids
      /* ------------------------------------------------------------------- */
      JAI(LEFT_X),       JAI(LEFT_Y),        JAI(RIGHT_X),   JAI(RIGHT_Y),
      JAI(LEFT_TRIGGER), JAI(RIGHT_TRIGGER), JAIX(6),        JAIX(7)
      /* ------------------------------------------------------------------- */
    }},                                // End of joystick axes ids init
    /* --------------------------------------------------------------------- */
#undef JAI                             // Done with this macro
#undef JAIX                            // Done with this macro
    /* --------------------------------------------------------------------- */
    iAxes(0)                           // Initialise axes count
    /* -- No code ---------------------------------------------------------- */
    { }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(JoyAxisList)         // Suppress default functions for safety
};/* ----------------------------------------------------------------------- */
/* -- Button class --------------------------------------------------------- */
class JoyButtonInfo
{ /* -- Private variables ----------------------------------------- */ private:
  const int        iId;                // Button unique identifier
  unsigned int     uiState;            // Current buffered butn press state
  /* -- Get button id ---------------------------------------------- */ public:
  int ButtonGetId(void) const { return iId; }
  /* -- Get buffered state (GLFW_RELEASE, GLFW_PRESS or GLFW_REPEAT) ------- */
  unsigned int ButtonGetState(void) const { return uiState; }
  /* -- Clear button state ------------------------------------------------- */
  void ButtonClearState(void) { uiState = GLFW_RELEASE; }
  /* -- Set button state --------------------------------------------------- */
  void ButtonSetState(const unsigned char*const ucpState)
  { // Save unbuffered state and compare it
    switch(const unsigned int uiUnbuffered =
      static_cast<unsigned int>(ucpState[ButtonGetId()]))
    { // Button is released?
      case GLFW_RELEASE:
        // Reset button state if not released (0 = released)
        if(uiState > GLFW_RELEASE) uiState = GLFW_RELEASE;
        // Done
        return;
      // Button is pressed? Increase button state (1=pressed and 2=held)
      case GLFW_PRESS:
        // Set GLFW_PRESSED if GLFW_RELEASED or
        //     GLFW_REPEAT if GLFW_PRESSED.
        if(uiState < GLFW_REPEAT) ++uiState;
        // Done
        return;
      // Shouldn't get here
      default:
        // Reset to released
        uiState = GLFW_RELEASE;
        // Done
        return;
    }
  }
  /* -- Constructor with unique id ----------------------------------------- */
  explicit JoyButtonInfo(const int iNId) :
    /* -- Initialisers ----------------------------------------------------- */
    iId(iNId),                         // Set unique id
    uiState(GLFW_RELEASE)              // Set default buffered state
    /* -- No code ---------------------------------------------------------- */
    { }
};/* -- Button data list --------------------------------------------------- */
template<class Container = array<JoyButtonInfo, 128>,
         class Iterator = typename Container::const_iterator>
  class JoyButtonList : private Container
{ /* ----------------------------------------------------------------------- */
  int              iButtons;           // Button count as int
  /* -- Return button array size ----------------------------------- */ public:
  int JoyButtonListSizeCountClamped(void) const { return iButtons & 0x7f; }
  /* -- Return button array size ------------------------------------------- */
  size_t JoyButtonListSize(void) const { return this->size(); }
  /* -- Return button count ------------------------------------------------ */
  size_t JoyButtonListCount(void) const
    { return static_cast<size_t>(iButtons); }
  /* -- Return button class ------------------------------------------------ */
  const JoyButtonInfo &JoyButtonListGet(const size_t stId) const
    { return (*this)[stId]; }
  /* -- Clear button data -------------------------------------------------- */
  void JoyButtonListClear(void)
    { StdForEach(par_unseq, this->begin(),
        this->begin() + JoyButtonListSizeCountClamped(),
          [](JoyButtonInfo& jbiRef) { jbiRef.ButtonClearState(); }); }
  /* -- Refresh button data ------------------------------------------------ */
  void JoyButtonListRefresh(const int iJId)
  { // Get joystick buttons and if found? Refresh button data for each one
    if(const unsigned char*const ucpData =
       GlFWGetJoystickButtons(iJId, iButtons))
      StdForEach(seq, this->begin(),
        this->begin() + JoyButtonListSizeCountClamped(),
      [ucpData](JoyButtonInfo &jbiData){ jbiData.ButtonSetState(ucpData); });
    else iButtons = 0;
  }
  /* -- Iterators ---------------------------------------------------------- */
  Iterator JoyButtonListBegin(void) const
    { return this->cbegin(); }
  Iterator JoyButtonListEnd(void) const
    { return this->cbegin() + JoyButtonListSizeCountClamped(); }
  /* -- Default constructor ------------------------------------- */ protected:
  JoyButtonList(void) :
    /* -- Initialise joystick buttons -------------------------------------- */
#define JBLX(x) JoyButtonInfo{ x }
#define JBL(n) JBLX(GLFW_GAMEPAD_BUTTON_ ## n)
    /* --------------------------------------------------------------------- */
    Container{{                        // Initialise joystick buttons ids
      /* ------------------------------------------------------------------- */
      JBL(A),           JBL(B),            JBL(X),           JBL(Y),
      JBL(LEFT_BUMPER), JBL(RIGHT_BUMPER), JBL(BACK),        JBL(START),
      JBL(GUIDE),       JBL(LEFT_THUMB),   JBL(RIGHT_THUMB), JBL(DPAD_UP),
      JBL(DPAD_RIGHT),  JBL(DPAD_DOWN),    JBL(DPAD_LEFT),
      JBLX( 15),JBLX( 16),JBLX( 17),JBLX( 18),JBLX( 19),JBLX( 20),JBLX( 21),
      JBLX( 22),JBLX( 23),JBLX( 24),JBLX( 25),JBLX( 26),JBLX( 27),JBLX( 28),
      JBLX( 29),JBLX( 30),JBLX( 31),JBLX( 32),JBLX( 33),JBLX( 34),JBLX( 35),
      JBLX( 36),JBLX( 37),JBLX( 38),JBLX( 39),JBLX( 40),JBLX( 41),JBLX( 42),
      JBLX( 43),JBLX( 44),JBLX( 45),JBLX( 46),JBLX( 47),JBLX( 48),JBLX( 49),
      JBLX( 50),JBLX( 51),JBLX( 52),JBLX( 53),JBLX( 54),JBLX( 55),JBLX( 56),
      JBLX( 57),JBLX( 58),JBLX( 59),JBLX( 60),JBLX( 61),JBLX( 62),JBLX( 63),
      JBLX( 64),JBLX( 65),JBLX( 66),JBLX( 67),JBLX( 68),JBLX( 69),JBLX( 70),
      JBLX( 71),JBLX( 72),JBLX( 73),JBLX( 74),JBLX( 75),JBLX( 76),JBLX( 77),
      JBLX( 78),JBLX( 79),JBLX( 80),JBLX( 81),JBLX( 82),JBLX( 83),JBLX( 84),
      JBLX( 85),JBLX( 86),JBLX( 87),JBLX( 88),JBLX( 89),JBLX( 90),JBLX( 91),
      JBLX( 92),JBLX( 93),JBLX( 94),JBLX( 95),JBLX( 96),JBLX( 97),JBLX( 98),
      JBLX( 99),JBLX(100),JBLX(101),JBLX(102),JBLX(103),JBLX(104),JBLX(105),
      JBLX(106),JBLX(107),JBLX(108),JBLX(109),JBLX(110),JBLX(111),JBLX(112),
      JBLX(113),JBLX(114),JBLX(115),JBLX(116),JBLX(117),JBLX(118),JBLX(119),
      JBLX(120),JBLX(121),JBLX(122),JBLX(123),JBLX(124),JBLX(125),JBLX(126),
      JBLX(127)
    /* ------------------------------------------------------------------- */
    }},                                // End of joystick button ids init
    /* ------------------------------------------------------------------- */
#undef JBL                             // Done with this macro
#undef JBX                             // Done with this macro
    /* --------------------------------------------------------------------- */
    iButtons(0)                        // Initialise button count
    /* -- No code ---------------------------------------------------------- */
    { }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(JoyButtonList)       // Suppress default functions for safety
};/* ----------------------------------------------------------------------- */
/* -- Joystick type typedef ------------------------------------------------ */
BUILD_FLAGS(Joy,
  /* ----------------------------------------------------------------------- */
  // No flags                          Joystick is connnected?
  JF_NONE                   {Flag[0]}, JF_CONNECTED              {Flag[1]},
  // Joystick is actually a gamepad
  JF_GAMEPAD                {Flag[2]}
);/* -- Joystick class ----------------------------------------------------- */
class JoyInfo :
  /* -- Base classes ------------------------------------------------------- */
  public JoyFlags,                     // Joystick flags
  public Ident,                        // Joystick identifier
  public JoyAxisList<>,                // Joystick axes list
  public JoyButtonList<>               // Joystick button list
{ /* ----------------------------------------------------------------------- */
  const int      iId;                  // Unique identifier of this
  string         strName,              // Name of controller
                 strGuid;              // Guid of controller
  /* -- Return joystick id ----------------------------------------- */ public:
  int JoyGetId(void) const { return iId; }
  /* -- Return type of joystick -------------------------------------------- */
  const string_view &JoyGetGamepadOrJoystickString(void) const
  { // Return if it is a gamepad or a joystick
    static const string_view svGamepad{ "gamepad" },
                             svJoystick{ "joystick" };
    return FlagIsSet(JF_GAMEPAD) ? svGamepad : svJoystick;
  }
  /* -- Refresh data ------------------------------------------------------- */
  void JoyRefreshData(void)
    { JoyAxisListRefresh(JoyGetId()); JoyButtonListRefresh(JoyGetId()); }
  /* -- Joystick is connected/disconnected? -------------------------------- */
  bool JoyIsConnected(void) const { return FlagIsSet(JF_CONNECTED); }
  bool JoyIsDisconnected(void) const { return !JoyIsConnected(); }
  /* -- Clear button state if connected ------------------------------------ */
  void JoyClearButtonStateIfConnected(void)
    { if(JoyIsConnected()) JoyButtonListClear(); }
  /* -- Clear buttons and axis state --------------------------------------- */
  void JoyClearState(void) { JoyAxisListClear(); JoyButtonListClear(); }
  /* -- Refresh data if connected ------------------------------------------ */
  void JoyRefreshDataIfConnected(void)
    { if(JoyIsConnected()) JoyRefreshData(); }
  /* -- Return gamepad name ------------------------------------------------ */
  const string &JoyGamePadName(void) const { return strName; }
  /* -- Return guid name --------------------------------------------------- */
  const string &JoyGUID(void) const { return strGuid; }
  /* -- Return if joystick is a gamepad ------------------------------------ */
  bool JoyIsGamepad(void) const { return FlagIsSet(JF_GAMEPAD); }
  /* -- Get/Set gamepad status --------------------------------------------- */
  void JoyConnect(void)
  { // Now connected
    FlagSet(JF_CONNECTED);
    // Set gamepad status
    FlagSetOrClear(JF_GAMEPAD, GlFWJoystickIsGamepad(JoyGetId()));
    // Get joystick name and if it's not null?
    if(const char*const cpName = GlFWGetJoystickName(JoyGetId()))
    { // If monitor name is blank return blank name
      if(*cpName) IdentSet(cpName);
      // Return blank name
      else IdentSet(cCommon->Unspec());
    } // Return null name
    else IdentSet(cCommon->Null());
    // Refresh joystick data
    JoyRefreshData();
    // We gained this joystick
    cLog->LogInfoExSafe("Joystick detected $ '$' (I:$;B:$;A:$).",
      JoyGetGamepadOrJoystickString(), IdentGet(), JoyGetId(),
      JoyButtonListCount(), JoyAxisListCount());
    // Report unique identifier
    if(const char*const cpIdent = GlFWGetJoystickGUID(JoyGetId()))
    { // Save guid for future reference and log the identifier
      strGuid = cpIdent;
      cLog->LogDebugExSafe("- Identifier: $.", strGuid);
    } // Clear guid
    else strGuid = cCommon->Null();
    // Report name if gamepad
    if(JoyIsGamepad())
      if(const char*const cpName = GlFWGetGamepadName(JoyGetId()))
      { // Save name for future reference and log the identifier
        strName = cpName;
        cLog->LogDebugExSafe("- Gamepad Name: $.", strName);
      } // Clear name
      else strName = cCommon->Null();
    else strName.clear();
  }
  /* -- Remove connected flag ---------------------------------------------- */
  void JoyDoDisconnect(void) { FlagClear(JF_CONNECTED); }
  /* -- Reset data --------------------------------------------------------- */
  void JoyDisconnect(void)
  { // Ignore if already disconnected
    if(JoyIsDisconnected()) return;
    // No longer connected
    JoyDoDisconnect();
    // We lost the specified joystick
    cLog->LogInfoExSafe("Joystick disconnected $ '$' (I:$).",
      JoyGetGamepadOrJoystickString(), IdentGet(), JoyGetId());
  }
  /* -- Detect joystick ---------------------------------------------------- */
  bool JoyIsPresent(void) { return GlFWJoystickPresent(JoyGetId()); }
  /* -- Constructor -------------------------------------------------------- */
  explicit JoyInfo(const int iNId) :
    /* -- Initialisers ----------------------------------------------------- */
    JoyFlags{ JF_NONE },               // Set no flags
    /* -- Other initialisers ----------------------------------------------- */
    iId(iNId)                          // Set unique joystick id
    /* -- No code ---------------------------------------------------------- */
    { }
};/* -- Joystick state typedefs -------------------------------------------- */
typedef array<JoyInfo, GLFW_JOYSTICK_LAST+1> JoyList; // Actual joystick data
typedef JoyList::const_iterator JoyListIt; // Iterator for vector of joys
/* ------------------------------------------------------------------------- */
class Joystick :
  /* -- Base classes ------------------------------------------------------- */
  public JoyList                       // Joystick list data classes
{ /* -- Variables ---------------------------------------------------------- */
  bool             bPoll;              // Poll joysticks?
  LuaFunc          lfOnJoyState;       // Joystick lua event
  size_t           stConnected;        // Joysticks connected
  /* -- Event handler for 'glfwSetJoystickCallback' ------------------------ */
  static void OnGamePad(int iJId, int iEvent)
    { cEvtMain->Add(EMC_INP_JOY_STATE, iJId, iEvent); }
  /* -- Enable or disable joystick polling --------------------------------- */
  void JoyEnablePoll(void) { bPoll = true; }
  void JoyDisablePoll(void) { bPoll = false; }
  /* -- Dispatch connected event to lua ------------------------------------ */
  void JoyDispatchLuaEvent(const size_t stJoystickId, const bool bConnected)
    { lfOnJoyState.LuaFuncDispatch(static_cast<lua_Integer>(stJoystickId),
        bConnected); }
  /* -- Joystick state changed ------------------------------------- */ public:
  void OnJoyState(const EvtMainEvent &emeEvent)
  { // Get reference to actual arguments vector
    const EvtMainArgs &emaArgs = emeEvent.aArgs;
    // Get joystick id as int
    const int iJId = emaArgs[0].i, iEvent = emaArgs[1].i;
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
  void JoyPoll(void)
    { if(bPoll)
        for(JoyInfo &jiRef : *this) jiRef.JoyRefreshDataIfConnected(); }
  /* -- Return event ------------------------------------------------------- */
  LuaFunc &JoyGetLuaEvent(void) { return lfOnJoyState; }
  /* -- Return writable joystick data -------------------------------------- */
  JoyInfo &JoyGet(const size_t stJId) { return (*this)[stJId]; }
  /* -- Return read-only joystick data ------------------------------------- */
  const JoyInfo &JoyGetConst(const size_t stJId) const
    { return (*this)[stJId]; }
  /* -- Get read-only acess to joystick list ------------------------------- */
  const JoyList &JoyListGetConst(void) const { return *this; }
  /* -- Get connected count ----------------------------------------------- */
  size_t JoyGetConnected(void) const { return stConnected; }
  /* -- Return joysticks count --------------------------------------------- */
  size_t JoyGetCount(void) const { return JoyListGetConst().size(); }
  /* -- Return if joystick is connected ------------------------------------ */
  bool JoyIsConnected(const size_t stId)
    { return JoyGetConst(stId).JoyIsConnected(); }
  /* -- Clear buttons and axis state --------------------------------------- */
  void JoyClearStates(void)
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
  void JoyDetect(void)
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
  void JoyReset(void)
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
  void JoyInit(void) const { glfwSetJoystickCallback(OnGamePad); }
  void JoyDeInit(void) const { glfwSetJoystickCallback(nullptr); }
  /* -- Constructor --------------------------------------------- */ protected:
  Joystick(void) :
    /* -- Init joystick ids ------------------------------------------------ */
    JoyList{{ JoyInfo{ GLFW_JOYSTICK_1  }, JoyInfo{ GLFW_JOYSTICK_2 },
              JoyInfo{ GLFW_JOYSTICK_3  }, JoyInfo{ GLFW_JOYSTICK_4 },
              JoyInfo{ GLFW_JOYSTICK_5  }, JoyInfo{ GLFW_JOYSTICK_6 },
              JoyInfo{ GLFW_JOYSTICK_7  }, JoyInfo{ GLFW_JOYSTICK_8 },
              JoyInfo{ GLFW_JOYSTICK_9  }, JoyInfo{ GLFW_JOYSTICK_10 },
              JoyInfo{ GLFW_JOYSTICK_11 }, JoyInfo{ GLFW_JOYSTICK_12 },
              JoyInfo{ GLFW_JOYSTICK_13 }, JoyInfo{ GLFW_JOYSTICK_14 },
              JoyInfo{ GLFW_JOYSTICK_15 }, JoyInfo{ GLFW_JOYSTICK_16 } }},
    /* -- More initialisers ------------------------------------------------ */
    bPoll(false),                      // Init disabled polling
    lfOnJoyState{ "OnJoyState" },      // Init joy state lua event
    stConnected(0)                     // Init joystick count to zero
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Destructor --------------------------------------------------------- */
  ~Joystick(void) { JoyDeInit(); }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(Joystick)            // Suppress default functions for safety
  /* -- Handle a deadzone change ----------------------------------- */ public:
  CVarReturn SetDefaultJoyDZ(const float fDZ,
    const function<void(JoyInfo&)> &fcbCallBack)
  { // Return if invalid deadzone else set it and return success
    if(fDZ > 1) return DENY;
    StdForEach(par_unseq, this->begin(), this->end(), fcbCallBack);
    return ACCEPT;
  }
  /* -- Set default negative deadzone -------------------------------------- */
  CVarReturn SetDefaultJoyRevDZ(const float fNewDeadZone)
    { return SetDefaultJoyDZ(fNewDeadZone, [fNewDeadZone](JoyInfo &jiRef)
        { jiRef.JoyAxisListSetReverseDeadZone(fNewDeadZone); }); }
  /* -- Set default positive deadzone -------------------------------------- */
  CVarReturn SetDefaultJoyFwdDZ(const float fNewDeadZone)
    { return SetDefaultJoyDZ(fNewDeadZone, [fNewDeadZone](JoyInfo &jiRef)
        { jiRef.JoyAxisListSetForwardDeadZone(fNewDeadZone); }); }
};/* ----------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
