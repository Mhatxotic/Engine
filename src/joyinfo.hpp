/* == JOYINFO.HPP ========================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## This module defines joystick info class for joystick access.        ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IJoyInfo {                   // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IFlags;                using namespace IGlFWUtil::P;
using namespace IIdent::P;             using namespace IJoyAxis::P;
using namespace IJoyButton::P;         using namespace ILog::P;
using namespace IString::P;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* -- Joystick type typedef ------------------------------------------------ */
BUILD_FLAGS(Joy,
  /* ----------------------------------------------------------------------- */
  // No flags                          Joystick is connnected?
  JF_NONE                   {Flag(0)}, JF_CONNECTED              {Flag(1)},
  // Joystick is actually a gamepad
  JF_GAMEPAD                {Flag(2)}
);
/* ------------------------------------------------------------------------- */
class JoyInfo :                        // Joystick class
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
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
