/* == LLCORE.hPP =========================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## Defines the 'Core' namespace and methods for the guest to use in    ## **
** ## Lua. This file is invoked by 'lualib.hpp'.                          ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
#pragma once                           // Only one incursion allowed
/* ========================================================================= **
** ######################################################################### **
** ========================================================================= */
// % Core
/* ------------------------------------------------------------------------- */
// ! The core class allows manipulation of and retrieve information from the
// ! game engine itself.
/* ========================================================================= */
namespace LLCore {                     // Core namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace Common;                using namespace IClock::P;
using namespace ICredit::P;            using namespace ICmdLine::P;
using namespace IConDef::P;            using namespace IConsole::P;
using namespace ICore::P;              using namespace IDisplay::P;
using namespace IEvtMain::P;           using namespace ILog::P;
using namespace ILua::P;               using namespace IStd::P;
using namespace IString::P;            using namespace ISystem::P;
using namespace ITimer::P;             using namespace IUtil::P;
/* ========================================================================= **
** ######################################################################### **
** ## Core common helper classes                                          ## **
** ######################################################################### **
** -- Get process pid argument --------------------------------------------- */
struct AgPid : public AgIntegerL<unsigned int> {
  explicit AgPid(lua_State*const lS, const int iArg) :
    AgIntegerL{lS, iArg, 1}{} };
/* -- Read a credit id ----------------------------------------------------- */
struct AgCreditEnum : public AgIntegerLGE<CreditEnums> {
  explicit AgCreditEnum(lua_State*const lS, const int iArg) :
    AgIntegerLGE{lS, iArg, CL_FIRST, CL_MAX}{} };
/* ========================================================================= **
** ######################################################################### **
** ## Core.* namespace functions                                          ## **
** ######################################################################### **
** ========================================================================= */
// $ Core.Catchup
// ? Resets the high resolution timer and resets the accumulator. Please be
// ? advised that the video system relies on this timer so videos will not
// ? play properly if this is constantly used. Only use when doing loading
// ? screens.
/* ------------------------------------------------------------------------- */
LLFUNC(Catchup, 0, cTimer->TimerCatchup())
/* ========================================================================= */
// $ Core.CPU
// < CPUid:string=The CPUID string.
// < Count:integer=Number of threads available to the engine.
// < Speed:integer=Processor frequency in Hz.
// < Family:integer=Processor family.
// < Model:integer=Processor model.
// < Stepping:integer=Processor stepping.
// ? Returns information about the installed Central Processing Unit.
/* ------------------------------------------------------------------------- */
LLFUNC(CPU, 6, LuaUtilPushVar(lS, cSystem->CPUName(), cSystem->CPUCount(),
  cSystem->CPUSpeed(), cSystem->CPUFamily(), cSystem->CPUModel(),
  cSystem->CPUStepping()))
/* ========================================================================= */
// $ Core.CPUFPS
// < FPS:number=Frames per second.
// ? Get CPU loops processed in the last second. Should be the same as GPU for
// ? most people but at times may be different, sometimes much higher.
/* ------------------------------------------------------------------------- */
LLFUNC(CPUFPS, 1, LuaUtilPushVar(lS, cTimer->TimerGetFPS()))
/* ========================================================================= */
// $ Core.CPUProcUsage
// < Percent:number=Percentage process.
// ? Returns the engine CPU load. It is hard-coded to only update once a
// ? a second so constant calls won't stress the kernel.
/* ------------------------------------------------------------------------- */
LLFUNC(CPUProcUsage, 1, LuaUtilPushVar(lS, cSystem->UpdateAndGetCPUUsage()))
/* ========================================================================= */
// $ Core.CPUSysUsage
// < Percent:number=Percentage system.
// ? Returns the system CPU load. It is hard-coded to only update once a
// ? a second so constant calls won't stress the kernel.
/* ------------------------------------------------------------------------- */
LLFUNC(CPUSysUsage, 1,
  LuaUtilPushVar(lS, cSystem->UpdateAndGetCPUUsageSystem()))
/* ========================================================================= */
// $ Core.CPUUsage
// < Percent:number=Percentage process.
// < Percent:number=Percentage system.
// ? Returns the engine and system CPU load. Use this instead of the other
// ? two cpu usage functions if you need both values. This omits a second
// ? check to update cpu usage and very slightly more optimal than calling
// ? both cpu usage functions which are hard-coded to only update once a
// ? a second so constant calls won't stress the kernel.
/* ------------------------------------------------------------------------- */
LLFUNC(CPUUsage, 2, LuaUtilPushVar(lS,
  cSystem->UpdateAndGetCPUUsage(), cSystem->CPUUsageSystem()))
/* ========================================================================= */
// $ Core.Delay
// < Time:number=Delay time in seconds.
// ? Returns the current thread delay time in seconds without having to read
// ? the 'vid_delay' variable. This would be useful if you are actually using
// ? the delay and you want to offset a time point by the thread delay.
/* ------------------------------------------------------------------------- */
LLFUNC(Delay, 1,
  LuaUtilPushVar(lS, static_cast<lua_Number>(cTimer->TimerGetDelay()) / 1000))
/* ========================================================================= */
// $ Core.Done
// ? Confirms that you want the engine to exit. This is so you can perform
// ? clean up actions in your own time by setting Core.SetEnd(), then calling
// ? this function to confirm you're done.
/* ------------------------------------------------------------------------- */
LLFUNC(Done, 0, cEvtMain->ConfirmExit())
/* ========================================================================= */
// $ Core.End
// ? Ends LUA execution and enables the console.
/* ------------------------------------------------------------------------- */
LLFUNC(End, 0, cEvtMain->Add(EMC_LUA_END))
/* ========================================================================= */
// $ Core.Engine
// < Title:string=Title of engine (normally Mhatxotic Engine).
// < Vendor:string=Author of engine (normally Mhatxotic Design).
// < Major:integer=Major version number of engine.
// < Minor:integer=Minor version number of engine.
// < Build:integer=Build version number of engine.
// < Revision:integer=Revision version number of engine.
// < Bits:integer=Bits version number of engine (32 or 64).
// < Type:string=Text representation of built type (Release,Alpha,Beta).
// < Target:string=Text string of the type of executable file.
// < Compiled:string=The timestamp of the executable compilation time.
// < Compiler:string=The name of the compiler that built the executable.
// < CompVersion:string=The version of the compiler that built the executable.
// ? Returns version information about the engine.
/* ------------------------------------------------------------------------- */
LLFUNC(Engine, 12, LuaUtilPushVar(lS, cSystem->ENGName(),
  cSystem->ENGAuthor(), cSystem->ENGMajor(), cSystem->ENGMinor(),
  cSystem->ENGBuild(), cSystem->ENGRevision(), cSystem->ENGBits(),
  cSystem->ENGBuildType(), cSystem->ENGTarget(), cSystem->ENGCompiled(),
  cSystem->ENGCompiler(), cSystem->ENGCompVer()))
/* ========================================================================= */
// $ Core.Env
// > Value:string=The name of the variable to query.
// < Value:string=The value of the specified variable.
// ? Queries the specified environment variable. Returns a blank string if
// ? empty. All environment variables are converted to upper-case at startup.
// ? Type 'env' in the console to see the current environment.
/* ------------------------------------------------------------------------- */
LLFUNC(Env, 1, LuaUtilPushVar(lS, cCmdLine->GetEnv(AgString{lS,1})))
/* ========================================================================= */
// $ Core.Events
// < Events:integer=Number of events in the engine events system.
// ? Returns the number of events in the engine event system. Helps with
// ? synchronising Video or Stream class events.
/* ------------------------------------------------------------------------- */
LLFUNC(Events, 1, LuaUtilPushVar(lS, cEvtMain->SizeSafe()))
/* ========================================================================= */
// $ Core.IsOSLinux
// < Boolean:boolean=True if using Linux, false if not.
// ? Returns true if executable was built for Linux, false if not.
/* ------------------------------------------------------------------------- */
#if defined(LINUX)
LLFUNC(IsOSLinux, 1, LuaUtilPushVar(lS, true))
#else
LLFUNC(IsOSLinux, 1, LuaUtilPushVar(lS, false))
#endif
/* ========================================================================= */
// $ Core.IsOSMac
// < Boolean:boolean=True if using MacOS, false if not.
// ? Returns true if executable was built for MacOS, false if not.
/* ------------------------------------------------------------------------- */
#if defined(MACOS)
LLFUNC(IsOSMac, 1, LuaUtilPushVar(lS, true))
#else
LLFUNC(IsOSMac, 1, LuaUtilPushVar(lS, false))
#endif
/* ========================================================================= */
// $ Core.IsOSWindows
// < Boolean:boolean=True if using Windows, false if not.
// ? Returns true if executable was built for Windows, false if not.
/* ------------------------------------------------------------------------- */
#if defined(WINDOWS)
LLFUNC(IsOSWindows, 1, LuaUtilPushVar(lS, true))
#else
LLFUNC(IsOSWindows, 1, LuaUtilPushVar(lS, false))
#endif
/* ========================================================================= */
// $ Core.Locale
// < LocaleID:number=Locale id.
// ? Returns the system locale id.
/* ------------------------------------------------------------------------- */
LLFUNC(Locale, 1, LuaUtilPushVar(lS, cSystem->OSLocale()))
/* ========================================================================= */
// $ Core.KillPid
// > Pid:integer=The pid of the executable to kill.
// < Result:boolean=The process is killed?
// ? Kills the specified process id. This only works on pids that were
// ? originally spawned by the engine. Specifying pid zero will cause an
// ? exception.
/* ------------------------------------------------------------------------- */
LLFUNC(KillPid, 1, const AgPid aPid{lS, 1};
  LuaUtilPushVar(lS, cSystem->TerminatePid(aPid)))
/* ========================================================================= */
// $ Core.Log
// > Text:string=The line of text to write to the log.
// ? Writes the specified line of text to the engine log with highest level.
/* ------------------------------------------------------------------------- */
LLFUNC(Log, 0,
  cLog->LogExSafe(LH_CRITICAL, "(Lua) $", AgCStringChar{lS, 1}()))
/* ========================================================================= */
// $ Core.LogEx
// > Text:string=The line of text to write to the log.
// > Level:integer=The log severity level.
// ? Writes the specified line of text to the engine log. Note that if the
// ? current log level cvar setting is below this then the function does not
// ? log anything.
/* ------------------------------------------------------------------------- */
LLFUNC(LogEx, 0,
  const AgCStringChar aString{lS, 1};
  const AgIntegerLGE<LHLevel> aLevel{lS, 2, LH_CRITICAL, LH_MAX};
  cLog->LogExSafe(aLevel, "(Lua) $", aString()))
/* ========================================================================= */
// $ Core.LUAMicroTime
// < Time:integer=The time in microseconds.
// ? Returns the total time in the LUA sandbox in microseconds.
/* ------------------------------------------------------------------------- */
LLFUNC(LUAMicroTime, 1, LuaUtilPushVar(lS, cLua->CCDeltaUS()))
/* ========================================================================= */
// $ Core.LUAMilliTime
// < Time:integer=The execution time in milliseconds.
// ? Returns the total time in the LUA sandbox in milliseconds.
/* ------------------------------------------------------------------------- */
LLFUNC(LUAMilliTime, 1, LuaUtilPushVar(lS, cLua->CCDeltaMS()))
/* ========================================================================= */
// $ Core.LUANanoTime
// < Time:integer=The time in nanoseconds.
// ? Returns the total time in the LUA sandbox in nanoseconds.
/* ------------------------------------------------------------------------- */
LLFUNC(LUANanoTime, 1, LuaUtilPushVar(lS, cLua->CCDeltaNS()))
/* ========================================================================= */
// $ Core.LUATime
// < Timestamp:number=The execution time as a number.
// ? Returns the total time in the LUA sandbox in seconds.
/* ------------------------------------------------------------------------- */
LLFUNC(LUATime, 1, LuaUtilPushVar(lS, cLua->CCDeltaToDouble()))
/* ========================================================================= */
// $ Core.LUAUsage
// < Bytes:integer=Bytes of memory.
// ? Returns the amount of memory in use by Lua
/* ------------------------------------------------------------------------- */
LLFUNC(LUAUsage, 1, LuaUtilPushVar(lS, LuaUtilGetUsage(lS)))
/* ========================================================================= */
// $ Core.Library
// > Id:integer=The index of the license.
// < Name:string=Name of the credit.
// < Version:string=Version of the api.
// < Id:integer=License index.
// < Website:string=Website of the api.
// < Copyright:string=Copyright of the api.
// < Author:string=Author of the api.
// ? Shows the full credits information of the specified api index.
/* ------------------------------------------------------------------------- */
LLFUNC(Library, 4,
  const AgCreditEnum aCredit{lS, 1};
  const CreditLib &clItem = cCredits->CreditGetItem(aCredit);
  LuaUtilPushVar(lS, clItem.GetName(),     clItem.GetVersion(),
            clItem.IsCopyright(), clItem.GetAuthor()))
/* ========================================================================= */
// $ Core.License
// > Id:integer=The index of the license.
// < Name:string=Name of the license.
// < Text:string=Full text file of the license.
// ? Shows the full license information of the specified index.
/* ------------------------------------------------------------------------- */
LLFUNC(License, 1,
  LuaUtilPushVar(lS, cCredits->CreditGetItemText(AgCreditEnum{lS, 1})))
/* ========================================================================= */
// $ Core.OS
// < Name:string=Operating system type (Windows,Linux,MacOS).
// < Major:integer=Major version number of operating system.
// < Minor:integer=Minor version number of operating system.
// < Build:integer=Build version number of operating system.
// < Platform:integer=Platform version number of operating system (Windows).
// < SPack:integer=Service pack number of operating system (Windows).
// < Suite:integer=Suite number of operating system (Windows).
// < ProdType:integer=Product number of operating system (Windows).
// < Bits:integer=Bits version of operating system.
// < Extra:string=Extra operating system information (e.g. Wine/Kernel).
// ? Returns version information about the operating system.
/* ------------------------------------------------------------------------- */
LLFUNC(OS, 6, LuaUtilPushVar(lS, cSystem->OSName(), cSystem->OSMajor(),
  cSystem->OSMinor(), cSystem->OSBuild(), cSystem->OSBits(),
  cSystem->OSNameEx()))
/* ========================================================================= */
// $ Core.OSMicroTime
// < Time:integer=The time in microseconds.
// ? Returns the time elapsed in microseconds since the OS started. Precision
// ? will be lost over time.
/* ------------------------------------------------------------------------- */
LLFUNC(OSMicroTime, 1, LuaUtilPushVar(lS, cmSys.GetTimeUS()))
/* ========================================================================= */
// $ Core.OSMilliTime
// < Time:integer=The time in milliseconds.
// ? Returns the time elapsed in milliseconds since the OS started. Precision
// ? will be lost over time.
/* ------------------------------------------------------------------------- */
LLFUNC(OSMilliTime, 1, LuaUtilPushVar(lS, cmSys.GetTimeMS()))
/* ========================================================================= */
// $ Core.OSNanoTime
// < Time:integer=The time in nanoseconds.
// ? Returns the time elapsed in nanoseconds since the OS started. Precision
// ? will be lost over time.
/* ------------------------------------------------------------------------- */
LLFUNC(OSNanoTime, 1, LuaUtilPushVar(lS, cmSys.GetTimeNS()))
/* ========================================================================= */
// $ Core.OSNumTime
// < Timestamp:number=The UNIX timestamp as a number.
// ? Returns a unix timestamp of the current time in seconds as a number.
/* ------------------------------------------------------------------------- */
LLFUNC(OSNumTime, 1, LuaUtilPushVar(lS, cmSys.GetTimeS<lua_Number>()))
/* ========================================================================= */
// $ Core.OnTick
// > Func:function=The main tick function to change to.
// ? On initialisation of IfLua:: The function address of 'MainTick' is.
// ? stored in the LUA registry for quick successive execution. Thus to change
// ? this function, you need to run this command with the function you want to
// ? change to.
/* ------------------------------------------------------------------------- */
LLFUNC(OnTick, 0, cLua->SetLuaRef(lS, cLua->lrMainTick))
/* ========================================================================= */
// $ Core.OnEnd
// > Func:function=The main end function to change to.
// ? The function address to execute when the engine has been asked to.
// ? terminate. The function _MUST_ call Core.Done() when that function has
// ? finished tidying up or the engine will soft-lock. Calling this when the
// ? engine is already terminating will do nothing so use Core.SetMain()
// ? instead if you want to change to a new main tick function.
/* ------------------------------------------------------------------------- */
LLFUNC(OnEnd, 0, cLua->SetLuaRef(lS, cLua->lrMainEnd))
/* ========================================================================= */
// $ Core.OSTime
// < Timestamp:integer=The UNIX timestamp.
// ? Returns a unix timestamp of the current time in seconds.
/* ------------------------------------------------------------------------- */
LLFUNC(OSTime, 1, LuaUtilPushVar(lS, cmSys.GetTimeS()))
/* ========================================================================= */
// $ Core.Pause
// ? Pauses LUA execution. Obviously, you can't resume and must do it manually!
/* ------------------------------------------------------------------------- */
LLFUNC(Pause, 0, cEvtMain->Add(EMC_LUA_PAUSE))
/* ========================================================================= */
// $ Core.PidRunning
// > Id:integer=The pid number to check
// < Result:boolean=The pid is valid and running?
// ? Asks the operating system to check if the specified pid exists and if the
// ? specified executable matches, true is returned, else false. Specifying
// ? pid zero will cause an exception.
/* ------------------------------------------------------------------------- */
LLFUNC(PidRunning, 1, const AgPid aPid{lS, 1};
  LuaUtilPushVar(lS, cSystem->IsPidRunning(aPid)))
/* ========================================================================= */
// $ Core.Quit
// ? Terminates the engine process cleanly.
/* ------------------------------------------------------------------------- */
LLFUNC(Quit, 0, cEvtMain->RequestQuit())
/* ========================================================================= */
// $ Core.RAM
// < Load:number=% load of total physical memory.
// < Total:integer=Total physical memory installed in system.
// < Free:integer=Available physical memory to engine.
// < Used:integer=Physical memory in use by the system and other apps.
// < ProcUse:integer=The total memory in use by the engine (including Virtual).
// < ProcPeak:integer=The peak memory usage of the engine.
// ? Returns information about physical memory in the computer.
/* ------------------------------------------------------------------------- */
LLFUNC(RAM, 6, cSystem->UpdateMemoryUsageData();
  LuaUtilPushVar(lS, cSystem->RAMLoad(), cSystem->RAMTotal(),
    cSystem->RAMFree(), cSystem->RAMUsed(), cSystem->RAMProcUse(),
    cSystem->RAMProcPeak()))
/* ========================================================================= */
// $ Core.Reset
// < Result:boolean = Was the event sent successfully?
// ? Ends LUA execution, clears the context, and restarts LUA execution. It
// ? will return 'false' if Lua is already re-initialising.
/* ------------------------------------------------------------------------- */
LLFUNC(Reset, 1, LuaUtilPushVar(lS, cLua->ReInit()))
/* ========================================================================= */
// $ Core.Restart
// ? Restarts the engine process cleanly.
/* ------------------------------------------------------------------------- */
LLFUNC(Restart, 0, cEvtMain->Add(EMC_QUIT_RESTART))
/* ========================================================================= */
// $ Core.RestartFresh
// ? Restarts the engine process cleanly without command-line arguments.
/* ------------------------------------------------------------------------- */
LLFUNC(RestartNP, 0, cEvtMain->Add(EMC_QUIT_RESTART_NP))
/* ========================================================================= */
// $ Core.RestoreDelay
// ? Restores the frame thread suspend value set via cvars
/* ------------------------------------------------------------------------- */
LLFUNC(RestoreDelay, 0, cTimer->TimerRestoreDelay())
/* ========================================================================= */
// $ Core.ScrollDown
// ? Scrolls the console up one line.
/* ------------------------------------------------------------------------- */
LLFUNC(ScrollDown, 0, cConsole->MoveLogDown())
/* ========================================================================= */
// $ Core.ScrollUp
// ? Scrolls the console up one line.
/* ------------------------------------------------------------------------- */
LLFUNC(ScrollUp, 0, cConsole->MoveLogUp())
/* ========================================================================= */
// $ Core.SetDelay
// > Millisecs:integer=Milliseconds to delay by each tick
// ? This is the same as updating the cvar 'app_delay' apart from that the cvar
// ? is not updated and not saved.
/* ------------------------------------------------------------------------- */
LLFUNC(SetDelay, 0, const AgUIntLG aMilliseconds{lS, 1, 0, 1000};
  cTimer->TimerUpdateDelay(aMilliseconds))
/* ========================================================================= */
// $ Core.SetIcon
// > Filename:string=The filenames of the large icon to set.
// ? Asks the operating system to set the specified icons of the programmers
// ? choosing. Separate each filename with a colon ':'. On Windows, anything
// ? but the first and the last icon are dropped so make sure you list the
// ? first filename as the large icon and the last filename as the small icon.
/* ------------------------------------------------------------------------- */
LLFUNC(SetIcon, 0, cDisplay->SetIconFromLua(AgString{lS, 1}))
/* ========================================================================= */
// $ Core.Stack
// < Stack:string=The current stack trace.
// > Text:string=The message to prefix.
// ? Returns the current stack as a string formatted by the engine and not
// ? Lua. This is needed for example when you use xpcall() with an error
// ? handler. Note that pcall() error messages do not include the stack.
/* ------------------------------------------------------------------------- */
LLFUNC(Stack, 1, const AgCStringChar aString{lS, 1};
  LuaUtilPushVar(lS, StrAppend(aString, LuaUtilStack(lS))))
/* ========================================================================= */
// $ Core.StatusLeft
// > String:string=Console status text.
// ? In terminal mode, this function will set the text to appear when no text
// ? is input into the input bar. Useful for customised stats. It will update
// ? every second.
/* ------------------------------------------------------------------------- */
LLFUNC(StatusLeft, 0, cConsole->SetStatusLeft(AgString{lS, 1}))
/* ========================================================================= */
// $ Core.StatusRight
// > String:string=Console status text.
// ? In terminal mode, this function will set the text to appear when no text
// ? is input into the input bar. Useful for customised stats. It will update
// ? every second.
/* ------------------------------------------------------------------------- */
LLFUNC(StatusRight, 0, cConsole->SetStatusRight(AgString{lS, 1}))
/* ========================================================================= */
// $ Core.Suspend
// > Millisecs:integer=Time in seconds.
// ? Delays the engine thread for this amount of time.
/* ------------------------------------------------------------------------- */
LLFUNC(Suspend, 0, const AgUIntLG aMilliseconds{lS, 1, 0, 1000};
  cTimer->TimerSuspend(aMilliseconds))
/* ========================================================================= */
// $ Core.Ticks
// < Ticks:integer=Number of ticks.
// ? Returns the total number of frames rendered since the engine started.
/* ------------------------------------------------------------------------- */
LLFUNC(Ticks, 1, LuaUtilPushVar(lS, cTimer->TimerGetTicks()))
/* ========================================================================= */
// $ Core.Time
// < Time:number=The time in seconds.
// ? Returns the time elapsed in seconds since the engine started. This is a
// ? very high resolution timer. Use it for time-criticial timings. This
// ? counter can be reset with Core.Update() as it will lose precision over
// ? time.
/* ------------------------------------------------------------------------- */
LLFUNC(Time, 1, LuaUtilPushVar(lS, cmHiRes.GetTimeDouble()))
/* ========================================================================= */
// $ Core.Uptime
// < Ticks:number=Uptime in seconds
// ? Returns the total number of seconds elapsed since the start of the engine
// ? this call maybe a little more expensive than Glfw.Time() but you have no
// ? choice to use this if you are using terminal mode. This uses std::chrono
// ? to retrieve this value.
/* ------------------------------------------------------------------------- */
LLFUNC(Uptime, 1, LuaUtilPushVar(lS, cLog->CCDeltaToDouble()))
/* ========================================================================= */
// $ Core.UpMicroTime
// < Time:integer=The engine uptime in microseconds.
// ? Returns the total time the engine has been running in microseconds.
/* ------------------------------------------------------------------------- */
LLFUNC(UpMicroTime, 1, LuaUtilPushVar(lS, cLog->CCDeltaUS()))
/* ========================================================================= */
// $ Core.UpMilliTime
// < Time:integer=The engine uptime in milliseconds.
// ? Returns the total time in the engine has been running in milliseconds.
/* ------------------------------------------------------------------------- */
LLFUNC(UpMilliTime, 1, LuaUtilPushVar(lS, cLog->CCDeltaMS()))
/* ========================================================================= */
// $ Core.UpNanoTime
// < Time:integer=The engine time in nanoseconds.
// ? Returns the total time in the engine has been running in nanoseconds.
/* ------------------------------------------------------------------------- */
LLFUNC(UpNanoTime, 1, LuaUtilPushVar(lS, cLog->CCDeltaNS()))
/* ========================================================================= */
// $ Core.WaitAsync
// ? Delays main thread execution until ALL asynchronous threads have completed
// ? Please note that the engine will be unresponsive during this time and
// ? therefore this call should only really be used in emergencies. Sockets
// ? are NOT synchronied. Use Sockets.CloseAll() to do that.
/* ------------------------------------------------------------------------- */
LLFUNC(WaitAsync, 0, cCore->CoreWaitAllAsync())
/* ========================================================================= */
// $ Core.Write
// > String:string=Text to write to console.
// ? Writes the specified line of text directly to the console with no regard
// ? to colour of text.
/* ------------------------------------------------------------------------- */
LLFUNC(Write, 0, cConsole->AddLine(COLOUR_CYAN, AgString{lS, 1}))
/* ========================================================================= */
// $ Core.WriteEx
// > Message:string=Text to write to console.
// > Colour:integer=The optional colour to use.
// ? Writes the specified line of text directly to the console with the
// ? specified text colour.
/* ------------------------------------------------------------------------- */
LLFUNC(WriteEx, 0,
  const AgString aMessage{lS, 1};
  const AgIntegerLGE<Colour> aColour{lS, 2, COLOUR_BLACK, COLOUR_MAX};
  cConsole->AddLine(aColour, aMessage))
/* ========================================================================= **
** ######################################################################### **
** ## Core.* namespace functions structure                                ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
LLRSBEGIN                              // Core.* namespace functions begin
  LLRSFUNC(Catchup),      LLRSFUNC(CPU),          LLRSFUNC(CPUFPS),
  LLRSFUNC(CPUProcUsage), LLRSFUNC(CPUSysUsage),  LLRSFUNC(CPUUsage),
  LLRSFUNC(Delay),        LLRSFUNC(Done),         LLRSFUNC(End),
  LLRSFUNC(Engine),       LLRSFUNC(Env),          LLRSFUNC(Events),
  LLRSFUNC(IsOSLinux),    LLRSFUNC(IsOSMac),      LLRSFUNC(IsOSWindows),
  LLRSFUNC(KillPid),      LLRSFUNC(Library),      LLRSFUNC(License),
  LLRSFUNC(Locale),       LLRSFUNC(Log),
  LLRSFUNC(LogEx),        LLRSFUNC(LUAMicroTime), LLRSFUNC(LUAMilliTime),
  LLRSFUNC(LUANanoTime),  LLRSFUNC(LUATime),      LLRSFUNC(LUAUsage),
  LLRSFUNC(OnEnd),        LLRSFUNC(OnTick),       LLRSFUNC(OS),
  LLRSFUNC(OSMicroTime),  LLRSFUNC(OSMilliTime),  LLRSFUNC(OSNanoTime),
  LLRSFUNC(OSNumTime),    LLRSFUNC(OSTime),       LLRSFUNC(Pause),
  LLRSFUNC(PidRunning),   LLRSFUNC(Quit),         LLRSFUNC(RAM),
  LLRSFUNC(Reset),        LLRSFUNC(Restart),      LLRSFUNC(RestartNP),
  LLRSFUNC(RestoreDelay), LLRSFUNC(ScrollDown),   LLRSFUNC(ScrollUp),
  LLRSFUNC(SetDelay),     LLRSFUNC(SetIcon),      LLRSFUNC(Stack),
  LLRSFUNC(StatusLeft),   LLRSFUNC(StatusRight),  LLRSFUNC(Suspend),
  LLRSFUNC(Ticks),        LLRSFUNC(Time),         LLRSFUNC(UpMicroTime),
  LLRSFUNC(UpMilliTime),  LLRSFUNC(UpNanoTime),   LLRSFUNC(Uptime),
  LLRSFUNC(WaitAsync),    LLRSFUNC(Write),        LLRSFUNC(WriteEx),
LLRSEND                                // Core.* namespace functions end
/* ========================================================================= **
** ######################################################################### **
** ## Core.* namespace constants                                          ## **
** ######################################################################### **
** ========================================================================= */
// @ Core.Colours
// < Data:table=A table of const string/int key pairs
// ? Returns all the colour palette of console colours used with ConWrite.
/* ------------------------------------------------------------------------- */
LLRSKTBEGIN(Colours)                   // Beginning of console colours
  LLRSKTITEM(COLOUR_,BLACK),           LLRSKTITEM(COLOUR_,BLUE),
  LLRSKTITEM(COLOUR_,GREEN),           LLRSKTITEM(COLOUR_,CYAN),
  LLRSKTITEM(COLOUR_,RED),             LLRSKTITEM(COLOUR_,GRAY),
  LLRSKTITEM(COLOUR_,MAGENTA),         LLRSKTITEM(COLOUR_,BROWN),
  LLRSKTITEM(COLOUR_,LGRAY),           LLRSKTITEM(COLOUR_,LBLUE),
  LLRSKTITEM(COLOUR_,LGREEN),          LLRSKTITEM(COLOUR_,LCYAN),
  LLRSKTITEM(COLOUR_,LRED),            LLRSKTITEM(COLOUR_,LMAGENTA),
  LLRSKTITEM(COLOUR_,YELLOW),          LLRSKTITEM(COLOUR_,WHITE),
  LLRSKTITEM(COLOUR_,MAX),
LLRSKTEND                              // End of console colours
/* ========================================================================= */
// @ Core.Libraries
// < Ids:table=The table of all library ids
// ? A table containing the ids of all the libraries supported.
/* ------------------------------------------------------------------------- */
LLRSKTBEGIN(Libraries)                 // Beginning of supported library ids
  LLRSKTITEM(CL_,MSE),  LLRSKTITEM(CL_,FT),   LLRSKTITEM(CL_,GLFW),
  LLRSKTITEM(CL_,JPEG), LLRSKTITEM(CL_,GIF),  LLRSKTITEM(CL_,PNG),
  LLRSKTITEM(CL_,LUA),  LLRSKTITEM(CL_,LZMA), LLRSKTITEM(CL_,MP3),
#if !defined(WINDOWS)                  // Not using Windows?
  LLRSKTITEM(CL_,NCURSES),             // Id for NCurses credit data
#endif                                 // Not using windows
  LLRSKTITEM(CL_,OGG),  LLRSKTITEM(CL_,AL),  LLRSKTITEM(CL_,SSL),
  LLRSKTITEM(CL_,JSON), LLRSKTITEM(CL_,SQL), LLRSKTITEM(CL_,THEO),
  LLRSKTITEM(CL_,ZLIB), LLRSKTITEM(CL_,MAX),
LLRSKTEND                              // End of supported library ids
/* ========================================================================= */
// @ Core.LogLevels
// < Data:table=The entire list of possible log levels.
// ? Returns a table of key/value pairs that identify possible log levels.
/* ------------------------------------------------------------------------- */
LLRSKTBEGIN(LogLevels)                 // Beginning of log levels
  LLRSKTITEM(LH_,CRITICAL),            LLRSKTITEM(LH_,ERROR),
  LLRSKTITEM(LH_,WARNING),             LLRSKTITEM(LH_,INFO),
  LLRSKTITEM(LH_,DEBUG),               LLRSKTITEM(LH_,MAX),
LLRSKTEND                              // End of log levels
/* ========================================================================= **
** ######################################################################### **
** ## Core.* namespace constants structure                                ## **
** ######################################################################### **
** ========================================================================= */
LLRSCONSTBEGIN                         // Core.* namespace consts begin
  LLRSCONST(Colours), LLRSCONST(Libraries), LLRSCONST(LogLevels),
LLRSCONSTEND                           // Core.* namespace consts end
/* ========================================================================= */
}                                      // End of Core namespace
/* == EoF =========================================================== EoF == */
