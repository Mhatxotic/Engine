/* == LLINPUT.HPP ========================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## Defines the 'Input' namespace and methods for the guest to use in   ## **
** ## Lua. This file is invoked by 'lualib.hpp'.                          ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ========================================================================= **
** ######################################################################### **
** ========================================================================= */
// % Input
/* ------------------------------------------------------------------------- */
// ! The input class allows the programmer to query the state of the keyboard,
// ! mouse and game controllers.
/* ========================================================================= */
namespace LLInput {                    // Input namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IGlFW::P;              using namespace IGlFWUtil::P;
using namespace IInput::P;             using namespace IJoyInfo::P;
using namespace IJoystick::P;          using namespace ILua::P;
using namespace Common;
/* ========================================================================= **
** ######################################################################### **
** ## Input common helper classes                                         ## **
** ######################################################################### **
** -- Get joystick index argument ------------------------------------------ */
struct AgJoystickId : public AgSizeTLGE {
  explicit AgJoystickId(lua_State*const lS, const int iArg) :
    AgSizeTLGE{ lS, iArg, 0, cInput->JoyGetCount() }{} };
/* -- Get joystick data from joystick index -------------------------------- */
struct AgJoystickData : public ArClass<JoyInfo> {
  explicit AgJoystickData(lua_State*const lS, const int iArg) :
    ArClass{ cInput->JoyGet(AgJoystickId{lS, iArg}()) }{} };
/* -- Get axis data from joystick data ------------------------------------- */
struct AgJoystickAxis : public AgSizeTLGE {
  explicit AgJoystickAxis(lua_State*const lS, const int iArg,
    const JoyInfo &jiData) :
      AgSizeTLGE{ lS, iArg, 0, jiData.JoyAxisListCount() }{} };
/* -- Get DeadZone argument ------------------------------------------------ */
struct AgDeadZone : public AgNumberLGE<float> {
  explicit AgDeadZone(lua_State*const lS, const int iArg) :
    AgNumberLGE{ lS, iArg, 0.0f, 1.0f }{} };
/* ========================================================================= **
** ######################################################################### **
** ## Input.* namespace functions                                         ## **
** ######################################################################### **
** ========================================================================= */
// $ Input.SetJoyAxisForwardDeadZone
// > JId:integer=The joystick id.
// > AId:integer=The joystick axis id.
// > Threshold:number=The forward deadzone value
// ? Sets the forward deadzone of the specified axis for the specified joystick
/* ------------------------------------------------------------------------- */
LLFUNC(SetJoyAxisForwardDeadZone, 0,
  const AgJoystickData aData{lS, 1};
  const AgJoystickAxis aAxis{lS, 2, aData};
  const AgDeadZone aDeadZone{lS, 3};
  aData().JoyAxisListSetForwardDeadZone(aAxis, aDeadZone))
/* ========================================================================= */
// $ Input.SetJoyAxisReverseDeadZone
// > JId:integer=The joystick id.
// > AId:integer=The joystick axis id.
// > Threshold:number=The forward deadzone value (positive)
// ? Sets the reverse deadzone of the specified axis for the specified joystick
// ? This value automatically gets converted to negative value.
/* ------------------------------------------------------------------------- */
LLFUNC(SetJoyAxisReverseDeadZone, 0,
  const AgJoystickData aData{lS, 1};
  const AgJoystickAxis aAxis{lS, 2, aData};
  const AgDeadZone aDeadZone{lS, 3};
  aData().JoyAxisListSetReverseDeadZone(aAxis, aDeadZone))
/* ========================================================================= */
// $ Input.SetJoyAxisDeadZones
// > JId:integer=The joystick id.
// > AId:integer=The joystick axis id.
// > FThresh:number=The forward deadzone value (positive)
// > RThresh:number=The reverse deadzone value (positive)
// ? Sets the forward and reverse deadzone of the specified axis for the
// ? specified joystick. The reverse value automatically gets converted to
// ? negative value.
/* ------------------------------------------------------------------------- */
LLFUNC(SetJoyAxisDeadZones, 0,
  const AgJoystickData aData{lS, 1};
  const AgJoystickAxis aAxis{lS, 2, aData};
  const AgDeadZone aDeadZoneForward{lS, 3},
                   aDeadZoneReverse{lS, 4};
  aData().JoyAxisListSetDeadZones(aAxis, aDeadZoneForward, aDeadZoneReverse))
/* ========================================================================= */
// $ Input.SetCursorPos
// > X:integer=The X co-ordinate of the mouse cursor.
// > Y:integer=The Y co-ordinate of the mouse cursor.
// ? Sets the position of the mouse cursor relative to the top-left of the
// ? main FBO matrix, meaning X could be negative. The specified co-ordinates
// ? will also be sent to the 'Input.OnMouseMove' callback if set.
/* ------------------------------------------------------------------------- */
LLFUNC(SetCursorPos, 0,
  const AgGLfloat aX{lS, 1},
                  aY{lS, 2};
  cInput->SetCursorPos(aX, aY))
/* ========================================================================= */
// $ Input.ClearStates
// ? Clears the keyboard, mouse and joystick states which wipes the current
// ? state of all the control methods preventing input.
/* ------------------------------------------------------------------------- */
LLFUNC(ClearStates, 0, cInput->JoyClearStates())
/* ========================================================================= */
// $ Input.CursorCentre
// ? Sets the mouse cursor in the centre of the window.
/* ------------------------------------------------------------------------- */
LLFUNC(SetCursorCentre, 0, cInput->SetCursorCentre())
/* ========================================================================= */
// $ Input.SetCursor
// > State:boolean=True to show the cursor, false to hide it.
// ? Shows or hides the OS cursor so you can effectively design your own
// ? software based cursor.
/* ------------------------------------------------------------------------- */
LLFUNC(SetCursor, 0, cInput->SetCursor(AgBoolean{lS, 1}))
/* ========================================================================= */
// $ Input.JoyExists
// > Id:integer=The joystick id.
// < State:boolean=True if it exists, false if it does not.
// ? Tests if the specified joystick exists.
/* ------------------------------------------------------------------------- */
LLFUNC(JoyExists, 1,
  LuaUtilPushVar(lS, cInput->JoyIsConnected(AgJoystickId{lS, 1})))
/* ========================================================================= */
// $ Input.GetJoyName
// > Id:integer=The joystick id.
// < Name:string=The name of the joystick.
// ? Returns the name of the specified joystick as reported by the OS.
/* ------------------------------------------------------------------------- */
LLFUNC(GetJoyName, 1, LuaUtilPushVar(lS, AgJoystickData{lS, 1}().IdentGet()))
/* ========================================================================= */
// $ Input.GetNumJoyButtons
// > Id:integer=The joystick id.
// < Count:integer=Number of buttons.
// ? Returns the number of buttons this joystick supports.
/* ------------------------------------------------------------------------- */
LLFUNC(GetNumJoyButtons, 1,
   LuaUtilPushVar(lS, AgJoystickData{lS, 1}().JoyButtonListCount()))
/* ========================================================================= */
// $ Input.GetNumJoyAxises
// > Id:integer=The joystick id.
// < Count:integer=Number of axises.
// ? Returns the number of axises this joystick supports.
/* ------------------------------------------------------------------------- */
LLFUNC(GetNumJoyAxises, 1,
  LuaUtilPushVar(lS, AgJoystickData{lS, 1}().JoyAxisListCount()))
/* ========================================================================= */
// $ Input.GetJoyButton
// > Id:integer=The joystick id.
// > ButtonId:integer=The button id to test.
// < Result:integer=Number of ticks the button has been held down.
// ? Returns the state of the specified joystick button.
// ?
// ? Result can be:
// ? =0: The button is not held down.
// ? =1: The button was pressed.
// ? >1: The button is being held.
/* ------------------------------------------------------------------------- */
LLFUNC(GetJoyButton, 1,
  const AgJoystickData aData{lS, 1};
  const AgSizeTLGE aJoystickButton{lS, 2, 0, aData().JoyButtonListCount()};
  LuaUtilPushVar(lS,
    aData().JoyButtonListGet(aJoystickButton()).ButtonGetState()))
/* ========================================================================= */
// $ Input.GetJoyAxis
// > Id:integer=The joystick id.
// > AxisId:integer=The axis id to test.
// < Result:integer=Number of ticks the button has been held down.
// ? Returns the state of the specified joystick axis.
/* ------------------------------------------------------------------------- */
LLFUNC(GetJoyAxis, 1,
  const AgJoystickData aData{lS, 1};
  const AgJoystickAxis aAxis{lS, 2, aData};
  LuaUtilPushVar(lS, aData().JoyAxisListGet(aAxis).AxisGetBufferedState()))
/* ========================================================================= */
// $ Input.GetJoyAxisUB
// > Id:integer=The joystick id.
// > AxisId:integer=The axis id to test.
// < Result:number=The raw state of axis.
/* ------------------------------------------------------------------------- */
LLFUNC(GetJoyAxisUB, 1,
  const AgJoystickData aData{lS, 1};
  const AgJoystickAxis aAxis{lS, 2, aData};
  LuaUtilPushVar(lS, aData().JoyAxisListGet(aAxis).AxisGetUnbufferedState()))
/* ========================================================================= */
// $ Input.OnMouseFocus
// > Func:function=The callback function to use
// ? When the mouse is moved from the window to another application and back,
// ? this function will be called with a integer whether the mouse entered the
// ? window or not.
/* ------------------------------------------------------------------------- */
LLFUNC(OnMouseFocus, 0, cLua->SetLuaRef(lS, cInput->lfOnMouseFocus))
/* ========================================================================= */
// $ Input.OnMouseClick
// > Func:function=The callback function to use
// ? When the mouse is clicked, this function will be called with the
// ? mouse button clicked, the state and the key modifiers
/* ------------------------------------------------------------------------- */
LLFUNC(OnMouseClick, 0, cLua->SetLuaRef(lS, cInput->lfOnMouseClick))
/* ========================================================================= */
// $ Input.OnMouseMove
// > Func:function=The callback function to use
// ? Sets teh callback function for When the mouse is moved. It will also be
// ? called immediately after setting a function. The syntax for the callback
// ? is 'Callback(nX, nY)' where 'nX' is the left co-ordinate and 'nY' is the
// ? top co-ordinate. Setting the function to 'nil' unreferences the already
// ? referenced function.
/* ------------------------------------------------------------------------- */
LLFUNC(OnMouseMove, 0,
  if(cLua->SetLuaRef(lS, cInput->lfOnMouseMove))
    cInput->RequestMousePosition())
/* ========================================================================= */
// $ Input.OnDragDrop
// > Func:function=The callback function to use
// ? When files are dragged into the window, this function will be called with
// ? an array of files that were dragged into it.
/* ------------------------------------------------------------------------- */
LLFUNC(OnDragDrop, 0, cLua->SetLuaRef(lS, cInput->lfOnDragDrop))
/* ========================================================================= */
// $ Input.OnMouseScroll
// > Func:function=The callback function to use
// ? When a filtered key is pressed, this function will be called with the
// ? 2 axis coordinates on which direction the mouse was scrolled
/* ------------------------------------------------------------------------- */
LLFUNC(OnMouseScroll, 0, cLua->SetLuaRef(lS, cInput->lfOnMouseScroll))
/* ========================================================================= */
// $ Input.OnChar
// > Func:function=The callback function to use
// ? When a filtered key is pressed, this function will be called with the
// ? key that was pressed.
/* ------------------------------------------------------------------------- */
LLFUNC(OnChar, 0, cLua->SetLuaRef(lS, cInput->lfOnChar))
/* ========================================================================= */
// $ Input.OnJoyState
// > Func:function=The callback function to use
// ? Sets the callback event when a joystick event occurs and then polls the
// ? joysticks sending results to the callback. Specify 'nil' to clear the
// ? event. The callback syntax is 'Callback(iJoystick, bConnected)' Where
// ? 'iJoystick' is the unique joystick id given to the joystick and
// ?  'bConnected' is if the joystick was connected (true) or disconnected
// ? (false).
/* ------------------------------------------------------------------------- */
LLFUNC(OnJoyState, 0,
  if(cLua->SetLuaRef(lS, cInput->JoyGetLuaEvent())) cInput->JoyDetect())
/* ========================================================================= */
// $ Input.OnKey
// > Func:function=The callback function to use
// ? When an filtered key is pressed, this function will be called with the
// ? key that was pressed
/* ------------------------------------------------------------------------- */
LLFUNC(OnKey, 0, cLua->SetLuaRef(lS, cInput->lfOnKey))
/* ========================================================================= */
// $ Input.GetKeyName
// > Value:integer=The key id of the key
// < Name:integer=The scan id of the key
// ? Returns the name of the specified key
/* ------------------------------------------------------------------------- */
LLFUNC(GetKeyName, 1,
  const AgInt aKeyId{lS, 1},
              aScanId{lS, 2};
  LuaUtilPushVar(lS, GlFWGetKeyName(aKeyId, aScanId)))
/* ========================================================================= **
** ######################################################################### **
** ## Input.* namespace functions structure                               ## **
** ######################################################################### **
** ========================================================================= */
LLRSBEGIN                              // Input.* namespace functions begin
  LLRSFUNC(ClearStates),               LLRSFUNC(GetJoyAxis),
  LLRSFUNC(GetJoyAxisUB),              LLRSFUNC(GetJoyButton),
  LLRSFUNC(GetJoyName),                LLRSFUNC(GetKeyName),
  LLRSFUNC(GetNumJoyAxises),           LLRSFUNC(GetNumJoyButtons),
  LLRSFUNC(JoyExists),                 LLRSFUNC(OnChar),
  LLRSFUNC(OnDragDrop),                LLRSFUNC(OnJoyState),
  LLRSFUNC(OnKey),                     LLRSFUNC(OnMouseClick),
  LLRSFUNC(OnMouseFocus),              LLRSFUNC(OnMouseMove),
  LLRSFUNC(OnMouseScroll),             LLRSFUNC(SetCursor),
  LLRSFUNC(SetCursorCentre),           LLRSFUNC(SetCursorPos),
  LLRSFUNC(SetJoyAxisDeadZones),       LLRSFUNC(SetJoyAxisForwardDeadZone),
  LLRSFUNC(SetJoyAxisReverseDeadZone),
LLRSEND                                // Input.* namespace functions end
/* ========================================================================= **
** ######################################################################### **
** ## Input.* namespace constants                                         ## **
** ######################################################################### **
** ========================================================================= */
// @ Input.KeyCodes
// < Data:table=The entire list of key codes GLFW supports
// ? Returns a table of key/value pairs that identify a specific key with its
// ? corresponding GLFW code (for use with GetKeyButton[UB]).
/* ------------------------------------------------------------------------- */
LLRSKTBEGIN(KeyCodes)                  // Beginning of key codes
  LLRSKTITEM(GLFW_KEY_,UNKNOWN),       LLRSKTITEM(GLFW_KEY_,SPACE),
  LLRSKTITEM(GLFW_KEY_,APOSTROPHE),    LLRSKTITEM(GLFW_KEY_,COMMA),
  LLRSKTITEM(GLFW_KEY_,MINUS),         LLRSKTITEM(GLFW_KEY_,PERIOD),
  LLRSKTITEM(GLFW_KEY_,SLASH),         LLRSKTITEMEX2("N0",GLFW_KEY_0),
  LLRSKTITEMEX2("N1",GLFW_KEY_1),      LLRSKTITEMEX2("N2",GLFW_KEY_2),
  LLRSKTITEMEX2("N3",GLFW_KEY_3),      LLRSKTITEMEX2("N4",GLFW_KEY_4),
  LLRSKTITEMEX2("N5",GLFW_KEY_5),      LLRSKTITEMEX2("N6",GLFW_KEY_6),
  LLRSKTITEMEX2("N7",GLFW_KEY_7),      LLRSKTITEMEX2("N8",GLFW_KEY_8),
  LLRSKTITEMEX2("N9",GLFW_KEY_9),      LLRSKTITEM(GLFW_KEY_,SEMICOLON),
  LLRSKTITEM(GLFW_KEY_,EQUAL),         LLRSKTITEM(GLFW_KEY_,A),
  LLRSKTITEM(GLFW_KEY_,B),             LLRSKTITEM(GLFW_KEY_,C),
  LLRSKTITEM(GLFW_KEY_,D),             LLRSKTITEM(GLFW_KEY_,E),
  LLRSKTITEM(GLFW_KEY_,F),             LLRSKTITEM(GLFW_KEY_,G),
  LLRSKTITEM(GLFW_KEY_,H),             LLRSKTITEM(GLFW_KEY_,I),
  LLRSKTITEM(GLFW_KEY_,J),             LLRSKTITEM(GLFW_KEY_,K),
  LLRSKTITEM(GLFW_KEY_,L),             LLRSKTITEM(GLFW_KEY_,M),
  LLRSKTITEM(GLFW_KEY_,N),             LLRSKTITEM(GLFW_KEY_,O),
  LLRSKTITEM(GLFW_KEY_,P),             LLRSKTITEM(GLFW_KEY_,Q),
  LLRSKTITEM(GLFW_KEY_,R),             LLRSKTITEM(GLFW_KEY_,S),
  LLRSKTITEM(GLFW_KEY_,T),             LLRSKTITEM(GLFW_KEY_,U),
  LLRSKTITEM(GLFW_KEY_,V),             LLRSKTITEM(GLFW_KEY_,W),
  LLRSKTITEM(GLFW_KEY_,X),             LLRSKTITEM(GLFW_KEY_,Y),
  LLRSKTITEM(GLFW_KEY_,Z),             LLRSKTITEM(GLFW_KEY_,LEFT_BRACKET),
  LLRSKTITEM(GLFW_KEY_,BACKSLASH),     LLRSKTITEM(GLFW_KEY_,RIGHT_BRACKET),
  LLRSKTITEM(GLFW_KEY_,GRAVE_ACCENT),  LLRSKTITEM(GLFW_KEY_,WORLD_1),
  LLRSKTITEM(GLFW_KEY_,WORLD_2),       LLRSKTITEM(GLFW_KEY_,ESCAPE),
  LLRSKTITEM(GLFW_KEY_,ENTER),         LLRSKTITEM(GLFW_KEY_,TAB),
  LLRSKTITEM(GLFW_KEY_,BACKSPACE),     LLRSKTITEM(GLFW_KEY_,INSERT),
  LLRSKTITEM(GLFW_KEY_,DELETE),        LLRSKTITEM(GLFW_KEY_,RIGHT),
  LLRSKTITEM(GLFW_KEY_,LEFT),          LLRSKTITEM(GLFW_KEY_,DOWN),
  LLRSKTITEM(GLFW_KEY_,UP),            LLRSKTITEM(GLFW_KEY_,PAGE_UP),
  LLRSKTITEM(GLFW_KEY_,PAGE_DOWN),     LLRSKTITEM(GLFW_KEY_,HOME),
  LLRSKTITEM(GLFW_KEY_,END),           LLRSKTITEM(GLFW_KEY_,CAPS_LOCK),
  LLRSKTITEM(GLFW_KEY_,SCROLL_LOCK),   LLRSKTITEM(GLFW_KEY_,NUM_LOCK),
  LLRSKTITEM(GLFW_KEY_,PRINT_SCREEN),  LLRSKTITEM(GLFW_KEY_,PAUSE),
  LLRSKTITEM(GLFW_KEY_,F1),            LLRSKTITEM(GLFW_KEY_,F2),
  LLRSKTITEM(GLFW_KEY_,F3),            LLRSKTITEM(GLFW_KEY_,F4),
  LLRSKTITEM(GLFW_KEY_,F5),            LLRSKTITEM(GLFW_KEY_,F6),
  LLRSKTITEM(GLFW_KEY_,F7),            LLRSKTITEM(GLFW_KEY_,F8),
  LLRSKTITEM(GLFW_KEY_,F9),            LLRSKTITEM(GLFW_KEY_,F10),
  LLRSKTITEM(GLFW_KEY_,F11),           LLRSKTITEM(GLFW_KEY_,F12),
  LLRSKTITEM(GLFW_KEY_,F13),           LLRSKTITEM(GLFW_KEY_,F14),
  LLRSKTITEM(GLFW_KEY_,F15),           LLRSKTITEM(GLFW_KEY_,F16),
  LLRSKTITEM(GLFW_KEY_,F17),           LLRSKTITEM(GLFW_KEY_,F18),
  LLRSKTITEM(GLFW_KEY_,F19),           LLRSKTITEM(GLFW_KEY_,F20),
  LLRSKTITEM(GLFW_KEY_,F21),           LLRSKTITEM(GLFW_KEY_,F22),
  LLRSKTITEM(GLFW_KEY_,F23),           LLRSKTITEM(GLFW_KEY_,F24),
  LLRSKTITEM(GLFW_KEY_,F25),           LLRSKTITEM(GLFW_KEY_,KP_0),
  LLRSKTITEM(GLFW_KEY_,KP_1),          LLRSKTITEM(GLFW_KEY_,KP_2),
  LLRSKTITEM(GLFW_KEY_,KP_3),          LLRSKTITEM(GLFW_KEY_,KP_4),
  LLRSKTITEM(GLFW_KEY_,KP_5),          LLRSKTITEM(GLFW_KEY_,KP_6),
  LLRSKTITEM(GLFW_KEY_,KP_7),          LLRSKTITEM(GLFW_KEY_,KP_8),
  LLRSKTITEM(GLFW_KEY_,KP_9),          LLRSKTITEM(GLFW_KEY_,KP_DECIMAL),
  LLRSKTITEM(GLFW_KEY_,KP_DIVIDE),     LLRSKTITEM(GLFW_KEY_,KP_MULTIPLY),
  LLRSKTITEM(GLFW_KEY_,KP_SUBTRACT),   LLRSKTITEM(GLFW_KEY_,KP_ADD),
  LLRSKTITEM(GLFW_KEY_,KP_ENTER),      LLRSKTITEM(GLFW_KEY_,KP_EQUAL),
  LLRSKTITEM(GLFW_KEY_,LEFT_SHIFT),    LLRSKTITEM(GLFW_KEY_,LEFT_CONTROL),
  LLRSKTITEM(GLFW_KEY_,LEFT_ALT),      LLRSKTITEM(GLFW_KEY_,LEFT_SUPER),
  LLRSKTITEM(GLFW_KEY_,RIGHT_SHIFT),   LLRSKTITEM(GLFW_KEY_,RIGHT_CONTROL),
  LLRSKTITEM(GLFW_KEY_,RIGHT_ALT),     LLRSKTITEM(GLFW_KEY_,RIGHT_SUPER),
  LLRSKTITEM(GLFW_KEY_,MENU),
LLRSKTEND                              // End of key codes
/* ========================================================================= */
// @ Input.KeyMods
// < Data:table=The entire list of states GLFW supports
// ? Returns a table of key/value pairs that identify a the state in which
// ? the keyboard or mouse key was pressed.
/* ------------------------------------------------------------------------- */
LLRSKTBEGIN(KeyMods)                   // Beginning of key/mouse mod states
  LLRSKTITEM(GLFW_MOD_,SHIFT),         LLRSKTITEM(GLFW_MOD_,CONTROL),
  LLRSKTITEM(GLFW_MOD_,ALT),           LLRSKTITEM(GLFW_MOD_,SUPER),
  LLRSKTITEM(GLFW_MOD_,CAPS_LOCK),     LLRSKTITEM(GLFW_MOD_,NUM_LOCK),
LLRSKTEND                              // End of mouse codes
/* ========================================================================= */
// @ Input.MouseCodes
// < Data:table=The entire list of key codes GLFW supports
// ? Returns a table of key/value pairs that identify a specific mouse button
// ? with its corresponding GLFW code (for use with GetMouseButton[UB]).
/* ------------------------------------------------------------------------- */
LLRSKTBEGIN(MouseCodes)                // Beginning of mouse codes
  LLRSKTITEMEX2("ONE",   GLFW_MOUSE_BUTTON_1),
  LLRSKTITEMEX2("TWO",   GLFW_MOUSE_BUTTON_2),
  LLRSKTITEMEX2("THREE", GLFW_MOUSE_BUTTON_3),
  LLRSKTITEMEX2("FOUR",  GLFW_MOUSE_BUTTON_4),
  LLRSKTITEMEX2("FIVE",  GLFW_MOUSE_BUTTON_5),
  LLRSKTITEMEX2("SIX",   GLFW_MOUSE_BUTTON_6),
  LLRSKTITEMEX2("SEVEN", GLFW_MOUSE_BUTTON_7),
  LLRSKTITEMEX2("EIGHT", GLFW_MOUSE_BUTTON_8),
  LLRSKTITEM(GLFW_MOUSE_BUTTON_,LEFT),
  LLRSKTITEM(GLFW_MOUSE_BUTTON_,RIGHT),
  LLRSKTITEM(GLFW_MOUSE_BUTTON_,MIDDLE),
LLRSKTEND                              // End of mouse codes
/* ========================================================================= */
// @ Input.States
// < Data:table=The entire list of states GLFW supports
// ? Returns a table of key/value pairs that identify a the state in which
// ? the keyboard or mouse key was pressed.
/* ------------------------------------------------------------------------- */
LLRSKTBEGIN(States)                    // Beginning of keyboard/mouse states
  LLRSKTITEM(GLFW_,RELEASE),           LLRSKTITEM(GLFW_,PRESS),
  LLRSKTITEM(GLFW_,REPEAT),
LLRSKTEND                              // End of mouse codes
/* ========================================================================= **
** ######################################################################### **
** ## Input.* namespace constants structure                               ## **
** ######################################################################### **
** ========================================================================= */
LLRSCONSTBEGIN                         // Input.* namespace consts begin
  LLRSCONST(KeyCodes),                 LLRSCONST(KeyMods),
  LLRSCONST(MouseCodes),               LLRSCONST(States),
LLRSCONSTEND                           // Input.* namespace consts end
/* ========================================================================= */
}                                      // End of Input namespace
/* == EoF =========================================================== EoF == */
