/* == SYSCORE.HPP ========================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## This is the header to the system class which allows the engine to   ## **
** ## interface with the operating system. Since all operating systems    ## **
** ## are different, we need to have seperate modules for them because    ## **
** ## they will be huge amounts of different code to work properly!       ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* -- Includes ------------------------------------------------------------- */
#if defined(WINDOWS)                   // Using windows?
# include "winmod.hpp"                 // Module information class
# include "winreg.hpp"                 // Registry class
# include "winmap.hpp"                 // File mapping class
# include "winpipe.hpp"                // Process output piping class
# include "winbase.hpp"                // Base system class
# include "wincon.hpp"                 // Console terminal window class
# include "syswin.hpp"                 // Include windows system core
#else                                  // Linux or MacOS (POSIX)?
# include "pixredir.hpp"               // Output to logger redirector
# include "pixbase.hpp"                // Base system class
# include "pixcon.hpp"                 // Console terminal window class
# include "pixmod.hpp"                 // Module information class
# include "pixmap.hpp"                 // File mapping class
# include "pixpipe.hpp"                // Process output piping class
# include "pixmutex.hpp"               // Mutex class
# include "syspix.hpp"                 // Common POSIX core
# if defined(MACOS)                    // Using mac?
#  include "sysmac.hpp"                // Include MacOS system core
# elif defined(LINUX)                  // Using linux?
#  include "sysnix.hpp"                // Common POSIX core
# endif                                // MacOS or Linux?
#endif                                 // Done checking OS
/* ------------------------------------------------------------------------- */
namespace ISystem {                    // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IClock::P;             using namespace ICmdLine::P;
using namespace ICommon::P;            using namespace ICVarDef::P;
using namespace IDir::P;               using namespace IError::P;
using namespace IInterval::P;          using namespace ILog::P;
using namespace ILookupArray::P;       using namespace IMemory::P;
using namespace IPSplit::P;            using namespace IStd::P;
using namespace IStdLib::P;            using namespace IString::P;
using namespace ISysCore::P;           using namespace ISysUtil::P;
using namespace ITime::P;              using namespace IUtil::P;
/* ------------------------------------------------------------------------- */
using ::std::terminate_handler;        using ::std::set_terminate;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* ------------------------------------------------------------------------- **
** ######################################################################### **
** ## The actual system class which we build using all the other classes  ## **
** ## we already defined above.                                           ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
class System;                          // Class prototype
static System *cSystem = nullptr;      // Address of global class
class System :                         // The main system class
  /* -- Base classes ------------------------------------------------------- */
  public SysCore                       // Defined in 'sys*.hpp' headers
{ /* -- Private typedefs --------------------------------------------------- */
  using ModeList = LookupArray<16>;    // List of possible combinations
  /* ----------------------------------------------------------------------- */
  const ModeList   mList;              // Modes list
  CoreFlags        cfMode;             // Requested core subsystem flags
  Interval         ivCpu;              // For getting cpu usage
  const size_t     stProcessId,        // Readable process id
                   stThreadId;         // Readable thread id
  StdStringView    strvTitle,          // Guest title
                   strvShortTitle,     // Guest short title
                   strvVersion,        // Guest version
                   strvAuthor,         // Guest author
                   strvCopyright,      // Guest copyright
                   strvDescription,    // Guest description
                   strvWebsite;        // Guest website
  /* ----------------------------------------------------------------------- */
  terminate_handler thHandler;         // Old C++ termination handler
  /* ----------------------------------------------------------------------- */
  const StdString     strRoamingDir;      // Roaming directory
  /* -- Callback for set_terminate() defined later ------------------------- */
  static void SysOnTerminateHandler[[noreturn]]();
  /* -- Return readable process and thread id ---------------------- */ public:
  size_t SysGetReadablePid() const { return stProcessId; }
  size_t SysGetReadableTid() const { return stThreadId; }
  /* -- Update CPU usage information --------------------------------------- */
  void SysUpdateCPUUsage()
    { if(ivCpu.CIIsTrigger()) UpdateCPUUsageData(); }
  /* -- Update and return process CPU usage -------------------------------- */
  double SysUpdateAndGetCPUUsage()
    { SysUpdateCPUUsage(); return CPUUsage(); }
  /* -- Update and return system CPU usage --------------------------------- */
  double SysUpdateAndGetCPUUsageSystem()
    { SysUpdateCPUUsage(); return CPUUsageSystem(); }
  /* -- Show message box with window handle (thiscall) --------------------- */
  unsigned SysMsgEx(const StdString &strReason,
    const StdString &strMessage, unsigned uFlags = MB_ICONSTOP) const
  { return SysMessage(GetWindowHandle(),
      StrAppend(ENGName(), ": ", strReason), strMessage, uFlags); }
  /* -- Get descriptor strings --------------------------------------------- */
  const StdStringView &SysGetGuestTitle() const { return strvTitle; }
  const StdStringView &SysGetGuestShortTitle() const { return strvShortTitle; }
  const StdStringView &SysGetGuestVersion() const { return strvVersion; }
  const StdStringView &SysGetGuestAuthor() const { return strvAuthor; }
  const StdStringView &SysGetGuestCopyright() const { return strvCopyright; }
  const StdStringView &SysGetGuestDescription() const
    { return strvDescription; }
  const StdStringView &SysGetGuestWebsite() const { return strvWebsite; }
  /* ----------------------------------------------------------------------- */
  const CoreFlagsConst SysGetCoreFlags() const { return cfMode; }
  bool SysIsCoreFlagsHave(const CoreFlagsConst cfFlags) const
    { return !cfFlags || SysGetCoreFlags().FlagIsSet(cfFlags); }
  bool SysIsCoreFlagsNotHave(const CoreFlagsConst cfFlags) const
    { return !SysIsCoreFlagsHave(cfFlags); }
  bool SysIsGraphicalMode() const
    { return SysGetCoreFlags().FlagIsSet(CFL_VIDEO); }
  bool SysIsNotGraphicalMode() const { return !SysIsGraphicalMode(); }
  bool SysIsTextMode() const
    { return SysGetCoreFlags().FlagIsSet(CFL_TEXT); }
  bool SysIsNotTextMode() const { return !SysIsTextMode(); }
  bool SysIsAudioMode() const
    { return SysGetCoreFlags().FlagIsSet(CFL_AUDIO); }
  bool SysIsNotAudioMode() const { return !SysIsAudioMode(); }
  bool SysIsTimerMode() const
    { return SysGetCoreFlags().FlagIsSet(CFL_TIMER); }
  bool SysIsNotTimerMode() const { return !SysIsTimerMode(); }
  /* -- Return users roaming directory ------------------------------------- */
  const StdString &SysGetRoamingDir() const { return strRoamingDir; }
  /* ----------------------------------------------------------------------- */
  const StdStringView
    &SysGetCoreFlagsString(const CoreFlagsConst cfFlags) const
  { return mList.Get(cfFlags); }
  /* ----------------------------------------------------------------------- */
  const StdStringView &SysGetCoreFlagsString() const
    { return SysGetCoreFlagsString(SysGetCoreFlags()); }
  /* -- Default error handler ---------------------------------------------- */
  static void SysCriticalHandler[[noreturn]](const char*const cpMessage)
  { // Show message box with error
    cLog->LogNLCErrorExSafe("Critical error: $!", cpMessage);
    // Abort and crash
    abort();
  }
  /* -- Restore old unexpected and termination handlers --------- */ protected:
  DTORHELPER(~System, set_terminate(thHandler))
  /* -- Default constructor ------------------------------------------------ */
  System() :
    /* -- Initialisers ----------------------------------------------------- */
    mList{{                            // Initialise mode strings list
      "nothing",                       // [00<      0>] (nothing)
      "text",                          // [01<      1>] (text)
      "audio",                         // [02<      2>] (audio)
      "text+audio",                    // [03<    1|2>] (text+audio)
      "video",                         // [04<      4>] (video)
      "text+video",                    // [05<    1|4>] (video+text)
      "audio+video",                   // [06<    2|4>] (video+audio)
      "text+audio+video",              // [07<  1|2|4>] (text+audio+video)
      "timer",                         // [08<      8>] (timer)
      "text+timer",                    // [09<    1|8>] (text+timer)
      "audio+timer",                   // [10<    2|8>] (audio+timer)
      "text+audio+timer",              // [11<  1|2|8>] (text+audio+timer)
      "video+timer",                   // [12<    4|8>] (video+timer)
      "text+video+timer",              // [13<  1|4|8>] (video+text+timer)
      "audio+video+timer",             // [14<  2|4|8>] (video+audio+timer)
      "text+audio+video+timer",        // [15<1|2|4|8>] (text+audio+video+timr)
    }},                                // Mode strings list initialised
    cfMode{ CFL_MASK },                // Guimode initially set by cvars
    ivCpu{ cd1S },                     // Cpu refresh time is one seconds
    stProcessId(GetPid<size_t>()),     // Init readable proceess id
    stThreadId(GetTid<size_t>()),      // Init readable thread id
    thHandler(set_terminate(           // Store current termination handler
      SysOnTerminateHandler)),         // " Use our termination handler
    strRoamingDir{                     // Set user roaming directory
      PSplitBackToForwardSlashes(      // Convert backward slashes to forward
        BuildRoamingDir()) }           // Get roaming directory from system
  /* -- Code --------------------------------------------------------------- */
  { // Set address of global class
    cSystem = this;
    // Update cpu and memory usage data
    SysUpdateCPUUsage();
    UpdateMemoryUsageData();
    // Log information about the environment
    cLog->LogNLCInfoExSafe("$ v$.$.$.$ ($) for $.\n"
       "+ Executable is $.\n"
       "+ Created at $ with $ v$.\n"
#if defined(WINDOWS)
       "+ Checksum $ with $<0x$$$> expecting $<0x$$$>.\n"
#endif
       "+ Working directory is $.\n"
       "+ Persistent directory is $.\n"
       "+ Process and thread id are $<0x$$$> and $<0x$$$>.\n"
#if !defined(MACOS)
       "+ Priority and affinity are $<0x$$$> and $<0x$$$>[$<0x$$$>].\n"
#endif
       "+ Processor is $<$x$MHz;FMS:$,$,$>.\n"
       "+ Memory has $ with $ free and $ initial.\n"
       "+ System is $ v$.$.$ ($-bit) in $$.\n"
       "+ Uptime is $.\n"
       "+ Clock is $.\n"
       "+ UTC clock is $.\n"
       "+ Admin is $ and bundled is $.",
      ENGName(), ENGMajor(), ENGMinor(), ENGBuild(), ENGRevision(),
        ENGBuildType(), ENGTarget(),
      ENGFull(), ENGCompiled(), ENGCompiler(), ENGCompVer(),
#if defined(WINDOWS)
      EXEModified() ? "failed" : "verified",
        exeData.ulHeaderSum, StdIOSHex, exeData.ulHeaderSum, StdIOSDec,
        exeData.ulCheckSum, StdIOSHex, exeData.ulCheckSum, StdIOSDec,
#endif
      cCmdLine->CmdLineGetStartupCWD(),
      SysGetRoamingDir(),
      SysGetReadablePid(), StdIOSHex, SysGetReadablePid(), StdIOSDec,
        SysGetReadableTid(), StdIOSHex, SysGetReadableTid(), StdIOSDec,
#if !defined(MACOS)
      GetPriority(), StdIOSHex, GetPriority(), StdIOSDec,
        GetAffinity(false), StdIOSHex, GetAffinity(false), StdIOSDec,
        GetAffinity(true), StdIOSHex, GetAffinity(true), StdIOSDec,
#endif
      CPUName(), CPUCount(), CPUSpeed(), CPUFamily(), CPUModel(),
        CPUStepping(),
      StrToBytes(RAMTotal()), StrToBytes(RAMFree()), StrToBytes(RAMProcUse()),
      OSName(), OSMajor(), OSMinor(), OSBuild(), OSBits(), OSLocale(),
        IsOSNameExSet() ? StrAppend(" via ", OSNameEx()) :
                          cCommon->CommonBlank(),
      durLong.Parse(GetUptime()),
      cmSys.FormatTime(), cmSys.FormatTimeUTC(),
      StrFromBoolTF(OSIsAdmin()), StrFromBoolTF(EXEBundled()));
  }
  /* -- CVar callbacks to update guest descriptor strings ---------- */ public:
  CVarReturn SysSetGuestTitle(const StdString&, const StdString &strV)
    { strvTitle = strV; return ACCEPT; }
  CVarReturn SysSetGuestShortTitle(const StdString&, const StdString &strV)
    { strvShortTitle = strV; return ACCEPT; }
  CVarReturn SysSetGuestVersion(const StdString&, const StdString &strV)
    { strvVersion = strV; return ACCEPT; }
  CVarReturn SysSetGuestAuthor(const StdString&, const StdString &strV)
    { strvAuthor = strV; return ACCEPT; }
  CVarReturn SysSetGuestCopyright(const StdString&, const StdString &strV)
    { strvCopyright = strV; return ACCEPT; }
  CVarReturn SysSetGuestDescription(const StdString&, const StdString &strV)
    { strvDescription = strV; return ACCEPT; }
  CVarReturn SysSetGuestWebsite(const StdString&, const StdString &strV)
    { strvWebsite = strV; return ACCEPT; }
  /* -- Update minimum RAM ------------------------------------------------- */
  CVarReturn SysSetMinRAM(const uint64_t ullMinValue)
  { // If we're to check for minimum memory free
    if(const size_t stMemory = UtilIntOrMax<size_t>(ullMinValue))
    { // Update memory usage data
      UpdateMemoryUsageData();
      // Capture allocation error
      try
      { // Try to allocate the memory. Take away current process memory usage.
        // We'll do a underflow check because utilities like valgrind can mess
        // with this.
        Memory{}.MemInitSafe(RAMProcUse() > stMemory ?
          stMemory : stMemory - RAMProcUse());
      } // Allocation failed?
      catch(const StdException &eReason)
      { // Throw memory error
        XC("There is not enough system memory available. Close any "
          "running applications consuming it and try running again!",
          "Error",   eReason,    "Available", RAMFree(),
          "Total",   RAMTotal(), "Required",  stMemory,
          "Percent", UtilMakePercentage(RAMFree(), RAMTotal()),
          "Needed",  stMemory - RAMFree());
      }
    } // Success
    return ACCEPT;
  }
  /* -- Set/Get GUI mode status -------------------------------------------- */
  CVarReturn SysSetCoreFlags(const CoreFlagsType cftFlags)
  { // Failed if bad value
    if(cftFlags != CFL_BASIC && (cftFlags & ~CFL_MASK)) return DENY;
    // Set new value
    cfMode.FlagReset(cftFlags);
    // Accepted
    return ACCEPT;
  }
  /* -- Set throw error on executable checksum mismatch -------------------- */
  CVarReturn SysCheckChecksumModified(const bool bEnabled)
  { // Ignore if we don't care that executabe checksum was modified
    if(!bEnabled || !EXEModified()) return ACCEPT;
    // Throw error
    XC("This software has been modified! Please re-acquire and/or "
      "re-install a fresh version of this software and try again!",
      "Path",     ENGFull(),
      "Expected", EXECheckSum(),
      "Actual",   EXEHeaderSum());
  }
  /* -- Set throw error if debugger is attached ---------------------------- */
  CVarReturn SysCheckDebuggerDetected(const bool bEnabled)
  { // Ignore if we don't care that a debugger is attached to the process
    if(!bEnabled || !DebuggerRunning()) return ACCEPT;
    // Throw error
    XC("There is a debugger attached to this software. Please "
      "close the offending debugger that is hooking onto this application "
      "and try running the application again");
  }
  /* -- Set working directory ---------------------------------------------- */
  CVarReturn SysSetWorkDir(const StdString &strP, StdString &strV)
  { // Set current directory to the startup directory as we want to honour the
    // users choice of relative directory.
    cCmdLine->CmdLineSetStartupCWD();
    // If targeting Apple systems?
#if defined(MACOS)
    // Working directory
    StdString strWorkDir;
    // No directory specified?
    if(strP.empty())
    { // Build app bundle directory suffix and if we're calling from it from
      // the application bundle? Use the MacOS/../Resources directory instead.
      if(EXEBundled())
        strWorkDir = StdMove(PathSplit{
          StrAppend(ENGLoc(), "../Resources"), true }.strFull);
      // Use executable working directory
      else strWorkDir = ENGLoc();
    } // Directory specified so use that and build full path for it
    else strWorkDir = StdMove(PathSplit{ strP, true }.strFull);
#else
    // Build directory
    StdString strWorkDir{ strP.empty() ? ENGLoc() :
      StdMove(PathSplit{ strP, true }.strFull) };
#endif
    // Set the directory and if failed? Throw the error
    if(!DirSetCWD(strWorkDir))
      XCL("Failed to set working directory!", "Directory", strWorkDir);
    // We are changing the value ourselves...
    strV = StdMove(strWorkDir);
    // ...so make sure the cvar system knows
    return ACCEPT_HANDLED;
  }
  /* -- Set throw error on elevated priviliedges --------------------------- */
  CVarReturn SysCheckAdminModified(const unsigned uMode)
  { // Valid modes allowed
    enum Mode { AM_OK,                 // [0] Running as admin is okay?
                AM_NOTOK,              // [1] Running as admin is not okay?
                AM_NOTOKIFMODERNOS };  // [2] As above but not if OS is modern?
    // Check mode
    switch(static_cast<Mode>(uMode))
    { // Return if OS uses admin as default for accounts else fall through
      case AM_NOTOKIFMODERNOS: if(OSIsAdminDefault()) return ACCEPT;
                               [[fallthrough]];
      // Break to error if running as admin else fall through to accept
      case AM_NOTOK: if(OSIsAdmin()) break;
                     [[fallthrough]];
      // Don't care if running as admin.
      case AM_OK: return ACCEPT;
      // Unknown parameter
      default: return DENY;
    }// Throw error
    XC("You are running this software with elevated privileges which can be "
      "dangerous and thus has been blocked on request by the guest. Please "
      "restart this software with reduced privileges.");
  }
};/* ----------------------------------------------------------------------- */
/* -- Callback for set_terminate() ----------------------------------------- */
void System::SysOnTerminateHandler()
{ // Show message box to user
  cSystem->SysMsgEx("Abnormal program termination!",
    "An unexpected error has occurred and the engine "
    "must now terminate! We apologise for the inconvenience!");
  // Terminate now without destructors
  _exit(-1);
}
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* -- Pre-defined SysBase callbacks that require access to cSystem global -- */
ENGINE_SYSBASE_CALLBACKS();            // Parse requested SysBase callbacks
#undef ENGINE_SYSBASE_CALLBACKS        // Done with this
/* -- Pre-defined SysCon callbacks that require access to cSystem global --- */
ENGINE_SYSCON_CALLBACKS();             // Parse requested SysCon callbacks
#undef ENGINE_SYSCON_CALLBACKS         // Done with this
/* == EoF =========================================================== EoF == */
