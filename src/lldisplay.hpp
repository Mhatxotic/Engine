/* == LLDISPLAY.HPP ======================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## Defines the 'Display' namespace and methods for the guest to use in ## **
** ## Lua. This file is invoked by 'lualib.hpp'.                          ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
#pragma once                           // Only one incursion allowed
/* ========================================================================= **
** ######################################################################### **
** ========================================================================= */
// % Display
/* ------------------------------------------------------------------------- */
// ! The window class contains functions for manipulating the Window.
/* ========================================================================= */
namespace LLDisplay {                  // Display namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IDisplay::P;           using namespace IEvtMain::P;
using namespace IFboCore::P;           using namespace IGlFW::P;
using namespace IGlFWCursor::P;        using namespace IGlFWMonitor::P;
using namespace IInput::P;             using namespace ILua::P;
using namespace IOgl::P;               using namespace ITimer::P;
using namespace Common;
/* ========================================================================= **
** ######################################################################### **
** ## Display common helper classes                                       ## **
** ######################################################################### **
** -- Get a monitor id ----------------------------------------------------- */
struct AgMonitorId : public AgSizeTLG {
  explicit AgMonitorId(lua_State*const lS, const int iArg) :
    AgSizeTLG{ lS, iArg, 0, cDisplay->GetMonitorsCount() }{} };
/* -- Get monitor data from id --------------------------------------------- */
struct AgMonitorData : public ArClass<const GlFWMonitor> {
  explicit AgMonitorData(lua_State*const lS, const int iArg) :
    ArClass{ cDisplay->GetMonitors()[AgMonitorId{ lS, iArg }] }{} };
/* -- Other types ---------------------------------------------------------- */
typedef AgIntegerLGE<int> AgIntLGE;
typedef AgIntegerLG<uint64_t> AgUInt64LG;
/* ========================================================================= **
** ######################################################################### **
** ## Display.* namespace functions                                       ## **
** ######################################################################### **
** ========================================================================= */
// $ Display.Attention
// ? This function requests user attention to the main window. On platforms
// ? where this is not supported, attention is requested to the application
// ? as a whole. Once the user has given attention, usually by focusing the
// ? window or application, the system will end the request automatically.
/* ------------------------------------------------------------------------- */
LLFUNC(Attention, 0, cDisplay->RequestAttention())
/* ========================================================================= */
// $ Display.Centre
// ? Moves the window to the centre of the screen.
/* ------------------------------------------------------------------------- */
LLFUNC(Centre, 0, cDisplay->RequestCentre())
/* ========================================================================= */
// $ Display.Decordated
// < State:boolean=The window is decorated?
// ? Returns if the window is decorated with a border.
/* ------------------------------------------------------------------------- */
LLFUNC(Decorated, 1, LuaUtilPushVar(lS, cGlFW->WinIsDecoratedAttribEnabled()))
/* ========================================================================= */
// $ Display.Floating
// < State:boolean=The window is floating?
// ? Returns if the window is floating an always showing on top.
/* ------------------------------------------------------------------------- */
LLFUNC(Floating, 1, LuaUtilPushVar(lS, cGlFW->WinIsFloatingAttribEnabled()))
/* ========================================================================= */
// $ Display.Focus
// ? Sets input focus to the window. This will actually restore the window if
// ? in full-screen mode.
/* ------------------------------------------------------------------------- */
LLFUNC(Focus, 0, cDisplay->RequestFocus())
/* ========================================================================= */
// $ Display.Focused
// < State:boolean=The window is focused?
// ? Returns if the window is focused.
/* ------------------------------------------------------------------------- */
LLFUNC(Focused, 1, LuaUtilPushVar(lS, cGlFW->WinIsFocusAttribEnabled()))
/* ========================================================================= */
// $ Display.FSType
// < Type:integer=Type of full-screen
// ? This function returns the type of full-screen currently engaged. Check
// ? Display.FSTypes table for a list of possible values.
/* ------------------------------------------------------------------------- */
LLFUNC(FSType, 1, LuaUtilPushVar(lS, cDisplay->GetFSType()))
/* ========================================================================= */
// $ Display.GetPos
// < X:integer=Current X position of the window.
// < Y:integer=Current Y position of the window.
// ? Gets the current position of the window on the desktop.
/* ------------------------------------------------------------------------- */
LLFUNC(GetPos, 2,
  LuaUtilPushVar(lS, cDisplay->GetWindowPosX(), cDisplay->GetWindowPosY()))
/* ========================================================================= */
// $ Display.GetSize
// < Width:integer=Width of the window.
// < Height:integer=Height of the window.
// ? Resets the current size of the window.
/* ------------------------------------------------------------------------- */
LLFUNC(GetSize, 2,
  LuaUtilPushVar(lS, cInput->GetWindowWidth(), cInput->GetWindowHeight()))
/* ========================================================================= */
// $ Display.GPU
// < Vendor:string=GL vendor string.
// < Version:string=GL version string.
// < Renderer:string=GL gpu string.
// ? Returns info about OpenGL.
/* ------------------------------------------------------------------------- */
LLFUNC(GPU, 3, LuaUtilPushVar(lS,
  cOgl->GetVendor(), cOgl->GetVersion(), cOgl->GetRenderer()))
/* ========================================================================= */
// $ Display.GPUFPS
// < FPS:number=Frames per second.
// ? Get GPU frames rendered in the second. Should be 60 for most people.
/* ------------------------------------------------------------------------- */
LLFUNC(GPUFPS, 1, LuaUtilPushVar(lS, cFboCore->dRTFPS))
/* ========================================================================= */
// $ Display.Hovered
// < State:boolean=Mouse is over the window?
// ? Indicates whether the cursor is currently directly over the content area
// ? of the window, with no other windows between.
/* ------------------------------------------------------------------------- */
LLFUNC(Hovered, 1,
  LuaUtilPushVar(lS, cGlFW->WinIsMouseHoveredAttribEnabled()))
/* ========================================================================= */
// $ Display.Iconified
// < State:boolean=The window is inconified?
// ? Returns if the window is iconified.
/* ------------------------------------------------------------------------- */
LLFUNC(Iconified, 1, LuaUtilPushVar(lS, cGlFW->WinIsIconifyAttribEnabled()))
/* ========================================================================= */
// $ Display.Iconify
// ? Minimises the window.
/* ------------------------------------------------------------------------- */
LLFUNC(Iconify, 0, cDisplay->RequestMinimise())
/* ========================================================================= */
// $ Display.Maximise
// ? Maximises the window.
/* ------------------------------------------------------------------------- */
LLFUNC(Maximise, 0, cDisplay->RequestMaximise())
/* ========================================================================= */
// $ Display.Maximised
// < State:boolean=The window is maximised?
// ? Returns if the window is maximised.
/* ------------------------------------------------------------------------- */
LLFUNC(Maximised, 1, LuaUtilPushVar(lS, cGlFW->WinIsMaximisedAttribEnabled()))
/* ========================================================================= */
// $ Display.Monitor
// < Id:number=Monitor index.
// ? Returns the currently selected monitor id.
/* ------------------------------------------------------------------------- */
LLFUNC(Monitor, 1, LuaUtilPushVar(lS, cDisplay->GetMonitorId()))
/* ========================================================================= */
// $ Display.Monitors
// < Id:number=Monitor count.
// ? Returns the total number of detected monitors on the system.
/* ------------------------------------------------------------------------- */
LLFUNC(Monitors, 1, LuaUtilPushVar(lS, cDisplay->GetMonitorsCount()))
/* ========================================================================= */
// $ Display.MonitorData
// > Id:integer=Id of monitor to query.
// < Name:string=Name of monitor.
// ? Returns the name of the specified monitor.
/* ------------------------------------------------------------------------- */
LLFUNC(MonitorData, 1, LuaUtilPushVar(lS, AgMonitorData{lS, 1}().Name()))
/* ========================================================================= */
// $ Display.OnFocused
// > Func:function=The event callback function
// ? When the window loses or gains focus, this event is called.
/* ------------------------------------------------------------------------- */
LLFUNC(OnFocused, 0, cLua->SetLuaRef(lS, cDisplay->lrFocused))
/* ========================================================================= */
// $ Display.Reset
// ? Resets the size and position of the window.
/* ------------------------------------------------------------------------- */
LLFUNC(Reset, 0, cDisplay->RequestReposition())
/* ========================================================================= */
// $ Display.ResetCursor
// ? Resets to the standard cursor graphic
/* ------------------------------------------------------------------------- */
LLFUNC(ResetCursor, 0, cDisplay->RequestResetCursor())
/* ========================================================================= */
// $ Display.Resizable
// < State:boolean=The window is visible?
// ? Indicates whether the window is resizable.
/* ------------------------------------------------------------------------- */
LLFUNC(Resizable, 1, LuaUtilPushVar(lS, cGlFW->WinIsResizableAttribEnabled()))
/* ========================================================================= */
// $ Display.Restore
// ? Restores the window from a minimized state. This may not work in
// ? full-screen mode. Check Display.Focus().
/* ------------------------------------------------------------------------- */
LLFUNC(Restore, 0, cDisplay->RequestRestore())
/* ========================================================================= */
// $ Display.SetCursor
// < Id:integer=The id of the new cursor
// ? This function sets a new standard cursor id. This operation is
// ? asynchronous and won't take effect until the next frame.
/* ------------------------------------------------------------------------- */
LLFUNC(SetCursor, 0, cDisplay->RequestSetCursor(
  AgIntegerLGE<GlFWCursorType>{lS, 1, CUR_ARROW, CUR_MAX}))
/* ========================================================================= */
// $ Display.SetFullScreen
// > State:boolean=true for fullscreen, false for window.
// ? Sets the full-screen window state and resets the video subsystem.
// ? This changes the cvar too.
/* ------------------------------------------------------------------------- */
LLFUNC(SetFullScreen, 0, cDisplay->RequestFSToggle(AgBoolean{lS, 1}))
/* ========================================================================= */
// $ Display.SetInterval
// > Ticks:integer=Number of ticks per second (min:1,max:200).
// ? This functions set the core timing frequency. The default target is to
// ? perform at least 60 game loop iterations per second. This is completely
// ? detatched from GPU rendering which will always render as fast as possible.
/* ------------------------------------------------------------------------- */
LLFUNC(SetInterval, 1, cTimer->TimerSetInterval(AgUInt64LG{lS, 1,
  cTimer->TimerGetMinInterval(), cTimer->TimerGetMaxInterval()}))
/* ========================================================================= */
// $ Display.SetMatrix
// > Width:number=Matrix width.
// > Height:number=Matrix height.
// ? Sets the 2D aspect size of the matrix. Recommended to use a 4:3
// ? aspect ratio to allow the automatic aspect ratio feature to work properly
// ? for widescreen window sizes.
/* ------------------------------------------------------------------------- */
LLFUNC(SetMatrix, 0,
  const AgNumberL<GLfloat> aWidth{lS, 1, 1.0f},
                           aHeight{lS, 2, 1.0f};
  cFboCore->AutoMatrix(aWidth, aHeight, false))
/* ========================================================================= */
// $ Display.SetPos
// > X:integer=New X position of the window.
// > Y:integer=New Y position of the window.
// ? Sets the new position of the window. This does not change the cvars which
// ? control the default window position.
/* ------------------------------------------------------------------------- */
LLFUNC(SetPos, 0,
  const AgInt aX{lS, 1},
              aY{lS, 2};
  cDisplay->RequestMove(aX, aY))
/* ========================================================================= */
// $ Display.SetSize
// > Width:integer=New width of the window.
// > Height:integer=New height of the window.
// ? Sets the new size of the window. This does not change the cvars which
// ? control the default window size.
/* ------------------------------------------------------------------------- */
LLFUNC(SetSize, 0,
  const AgIntLGE aWidth{lS, 1, 0, numeric_limits<int>::max()},
                 aHeight{lS, 2, 0, numeric_limits<int>::max()};
  cDisplay->RequestResize(aWidth, aHeight))
/* ========================================================================= */
// $ Display.Transparent
// < State:boolean=The window is using an alpha framebuffer?
// ? Returns if the window is transparent and using a alpha framebuffer.
/* ------------------------------------------------------------------------- */
LLFUNC(Transparent, 1, LuaUtilPushVar(lS,
  cGlFW->WinIsTransparencyAttribEnabled()))
/* ========================================================================= */
// $ Display.VidMode
// < Id:number=Mode id.
// ? Returns the currently selected video mode id.
/* ------------------------------------------------------------------------- */
LLFUNC(VidMode, 1, LuaUtilPushVar(lS, cDisplay->GetVideoModeId()))
/* ========================================================================= */
// $ Display.VidModeData
// > MonitorId:integer=Id of monitor to query.
// > ModeId:integer=Id of video mode to query.
// < Width:integer=Width of the video mode in pixels.
// < Height:integer=Height of the video mode in pixels.
// < Depth:integer=Depth of the video mode. (16 or 24).
// < Refresh:integer=Refresh rate of the video mode (normally 60).
// < Red:integer=Number of bits in the red component (normally 8).
// < Green:integer=Number of bits in the green component (normally 8).
// < Blue:integer=Number of bits in the blue component (normally 8).
// ? Returns information about the specified video mode.
/* ------------------------------------------------------------------------- */
LLFUNC(VidModeData, 7,
  const AgMonitorData aMonitorData{lS, 1};
  const AgSizeTLGE aVideoModeId{lS, 2, 0, aMonitorData().size()};
  const GlFWRes &rItem = aMonitorData()[aVideoModeId];
  LuaUtilPushVar(lS, rItem.Width(), rItem.Height(), rItem.Depth(),
    rItem.Refresh(), rItem.Red(), rItem.Green(), rItem.Blue()))
/* ========================================================================= */
// $ Display.VidModes
// < Count:number=Total video modes.
// ? Returns the total number of video modes supported by the GPU.
/* ------------------------------------------------------------------------- */
LLFUNC(VidModes, 1, LuaUtilPushVar(lS, AgMonitorData{lS, 1}().size()))
/* ========================================================================= */
// $ Display.Visible
// < State:boolean=The window is visible?
// ? Indicates whether the window is visible.
/* ------------------------------------------------------------------------- */
LLFUNC(Visible, 1, LuaUtilPushVar(lS, cGlFW->WinIsVisibilityAttribEnabled()))
/* ========================================================================= */
// $ Display.VRAM
// < Available:integer=Available video memory in bytes
// < Total:integer=Total video memory in bytes
// < Used:integer=Used video memory in bytes
// < Dedicated:boolean=Is video memory separate from main memory
// ? Returns info about the GPU's video memory.
/* ------------------------------------------------------------------------- */
LLFUNC(VRAM, 5, LuaUtilPushVar(lS, cOgl->GetVRAMFreePC(),
  cOgl->GetVRAMTotal(), cOgl->GetVRAMFree(), cOgl->GetVRAMUsed(),
  cOgl->FlagIsSet(GFL_SHARERAM)))
/* ========================================================================= */
// $ Display.VReset
// ? Resets the video subsystem.
/* ------------------------------------------------------------------------- */
LLFUNC(VReset, 0, cEvtMain->RequestQuitThread())
/* ========================================================================= **
** ######################################################################### **
** ## Display.* namespace functions structure                             ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
LLRSBEGIN                              // Display.* namespace func
  LLRSFUNC(Attention),   LLRSFUNC(Centre),        LLRSFUNC(Decorated),
  LLRSFUNC(Floating),    LLRSFUNC(Focus),         LLRSFUNC(Focused),
  LLRSFUNC(FSType),      LLRSFUNC(GetPos),        LLRSFUNC(GetSize),
  LLRSFUNC(GPU),         LLRSFUNC(GPUFPS),        LLRSFUNC(Hovered),
  LLRSFUNC(Iconified),   LLRSFUNC(Iconify),       LLRSFUNC(Maximise),
  LLRSFUNC(Maximised),   LLRSFUNC(Monitor),       LLRSFUNC(MonitorData),
  LLRSFUNC(Monitors),    LLRSFUNC(OnFocused),     LLRSFUNC(Reset),
  LLRSFUNC(ResetCursor), LLRSFUNC(Resizable),     LLRSFUNC(Restore),
  LLRSFUNC(SetCursor),   LLRSFUNC(SetFullScreen), LLRSFUNC(SetInterval),
  LLRSFUNC(SetMatrix),   LLRSFUNC(SetPos),        LLRSFUNC(SetSize),
  LLRSFUNC(Transparent), LLRSFUNC(VidMode),       LLRSFUNC(VidModeData),
  LLRSFUNC(VidModes),    LLRSFUNC(Visible),       LLRSFUNC(VRAM),
  LLRSFUNC(VReset),
LLRSEND                                // Display.* namespace functions end
/* ========================================================================= **
** ######################################################################### **
** ## Display.* namespace constants                                       ## **
** ######################################################################### **
** ========================================================================= */
// @ Display.Cursors
// < Data:table=The available standard cursor types
// ? Returns a table of key/value pairs that identify standard cursors
/* ------------------------------------------------------------------------- */
LLRSKTBEGIN(Cursors)                   // Beginning of cursor codes
  LLRSKTITEM(CUR_,ARROW),              LLRSKTITEM(CUR_,CROSSHAIR),
  LLRSKTITEM(CUR_,HAND),               LLRSKTITEM(CUR_,HRESIZE),
  LLRSKTITEM(CUR_,IBEAM),              LLRSKTITEM(CUR_,NOT_ALLOWED),
  LLRSKTITEM(CUR_,RESIZE_ALL),         LLRSKTITEM(CUR_,RESIZE_EW),
  LLRSKTITEM(CUR_,RESIZE_NESW),        LLRSKTITEM(CUR_,RESIZE_NS),
  LLRSKTITEM(CUR_,RESIZE_NWSE),        LLRSKTITEM(CUR_,VRESIZE),
LLRSKTEND                              // End of cursor codes
/* ========================================================================= */
// @ Display.FSTypes
// < Data:table=The available full-screen types
// ? Returns a table of key/value pairs that identify the states of the window.
// ? ID   Keyname     Meaning
// ? ===  =======     =========================================================
// ? [0]  WINDOW      Decorated and can be moved and sized around the desktop.
// ? [1]  EXCLUSIVE   Undecorated, covers the whole screen and set resolution.
// ? [2]  BORDERLESS  Undecorated, covers the whole screen only.
// ? [3]  NATIVE      (MacOS only) Same as BORDERLESS but disables toggler.
// ? ===  =======     =========================================================
/* ------------------------------------------------------------------------- */
LLRSKTBEGIN(FSTypes)                   // Beginning of full-screen types
  LLRSKTITEM(Display::FST_,WINDOW),    LLRSKTITEM(Display::FST_,EXCLUSIVE),
  LLRSKTITEM(Display::FST_,BORDERLESS),LLRSKTITEM(Display::FST_,NATIVE),
LLRSKTEND                              // End of mouse codes
/* ========================================================================= **
** ######################################################################### **
** ## Display.* namespace structure                                       ## **
** ######################################################################### **
** ========================================================================= */
LLRSCONSTBEGIN                         // Display.* namespace consts begin
  LLRSCONST(Cursors), LLRSCONST(FSTypes),
LLRSCONSTEND                           // Display.* namespace consts end
/* ========================================================================= */
}                                      // End of Display namespace
/* == EoF =========================================================== EoF == */
