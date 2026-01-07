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
/* ------------------------------------------------------------------------- */
namespace ISystem {                    // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IArgs::P;              using namespace IClock::P;
using namespace ICmdLine::P;           using namespace ICommon::P;
using namespace IConDef::P;            using namespace ICoord::P;
using namespace ICVarDef::P;           using namespace IDim::P;
using namespace IDimCoord::P;          using namespace IDir::P;
using namespace IError::P;             using namespace IEvtMain::P;
using namespace IFlags;                using namespace IFStream::P;
using namespace IHelper::P;            using namespace IIdent::P;
using namespace ILog::P;               using namespace IMemory::P;
using namespace IMutex::P;             using namespace IParser::P;
using namespace IPSplit::P;            using namespace IStat::P;
using namespace IStd::P;               using namespace IString::P;
using namespace ISysUtil::P;           using namespace IToken::P;
using namespace IThread::P;            using namespace IUtf::P;
using namespace IUtil::P;              using namespace Lib::OS;
using ::std::terminate_handler;        using ::std::set_terminate;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* == System module data =================================================== **
** ######################################################################### **
** ## Information about a module.                                         ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
class SysModuleData :                  // Members initially private
  /* -- Base classes ------------------------------------------------------- */
  private PathSplit                    // Path parts to mod name
{ /* -- Variables ---------------------------------------------------------- */
  const unsigned int uiMajor, uiMinor, // Major and minor version of module
                  uiBuild, uiRevision; // Build and revision version of module
  const string     strVendor,          // Vendor of module
                   strDesc,            // Description of module
                   strComments,        // Comments of module
                   strVersion;         // Version as string
  /* --------------------------------------------------------------- */ public:
  const string &GetDrive() const { return strDrive; }
  const string &GetDir() const { return strDir; }
  const string &GetFile() const { return strFile; }
  const string &GetExt() const { return strExt; }
  const string &GetFileExt() const { return strFileExt; }
  const string &GetFull() const { return strFull; }
  const string &GetLoc() const { return strLoc; }
  unsigned int GetMajor() const { return uiMajor; }
  unsigned int GetMinor() const { return uiMinor; }
  unsigned int GetBuild() const { return uiBuild; }
  unsigned int GetRevision() const { return uiRevision; }
  const string &GetVendor() const { return strVendor; }
  const string &GetDesc() const { return strDesc; }
  const string &GetComments() const { return strComments; }
  const string &GetVersion() const { return strVersion; }
  /* -- Move constructor --------------------------------------------------- */
  SysModuleData(SysModuleData &&smdO) :
    /* -- Initialisers ----------------------------------------------------- */
    PathSplit{ StdMove(smdO) },                 // Copy filename
    uiMajor(smdO.GetMajor()),                   // Copy major version
    uiMinor(smdO.GetMinor()),                   // Copy minor version
    uiBuild(smdO.GetBuild()),                   // Copy build version
    uiRevision(smdO.GetRevision()),             // Copy revision version
    strVendor{ StdMove(smdO.GetVendor()) },     // Move vendor string
    strDesc{ StdMove(smdO.GetDesc()) },         // Move description string
    strComments{ StdMove(smdO.GetComments()) }, // Move comments string
    strVersion{ StdMove(smdO.GetVersion()) }    // Move version string
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- Initialise all members contructor ---------------------------------- */
  explicit SysModuleData(const string &strF, const unsigned int uiMa,
    const unsigned int uiMi, const unsigned int uiBu, const unsigned int uiRe,
    string &&strVen, string &&strDe, string &&strCo, string &&strVer) :
    /* -- Initialisers ----------------------------------------------------- */
    PathSplit{ strF },                 // Copy filename
    uiMajor(uiMa),                     // Copy major version
    uiMinor(uiMi),                     // Copy minor version
    uiBuild(uiBu),                     // Copy build version
    uiRevision(uiRe),                  // Copy revision version
    strVendor{ StdMove(strVen) },      // Move vendor string
    strDesc{ StdMove(strDe) },         // Move description string
    strComments{ StdMove(strCo) },     // Move comments string
    strVersion{ StdMove(strVer) }      // Move version string
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- Initialise filename only constructor ------------------------------- */
  explicit SysModuleData(const string &strF) :
    /* -- Initialisers ----------------------------------------------------- */
    PathSplit{ strF },                 // Initialise path parts
    uiMajor(0),                        // Major version not initialised yet
    uiMinor(0),                        // Minor version not initialised yet
    uiBuild(0),                        // Build version not initialised yet
    uiRevision(0)                      // Revision not initialised yet
    /* -- No code ---------------------------------------------------------- */
    {}
};/* ----------------------------------------------------------------------- */
/* == System modules ======================================================= **
** ######################################################################### **
** ## Storage for all the modules loaded to this executable.              ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
MAPPACK_BUILD(SysMod, const size_t,const SysModuleData)
/* ------------------------------------------------------------------------- */
struct SysModules :
  /* -- Base classes ------------------------------------------------------- */
  public SysModMap                     // System module list
{ /* -- Move constructor ---------------------------------------- */ protected:
  SysModules(SysModules &&smOther) :
    /* -- Initialisers ----------------------------------------------------- */
    SysModMap{ StdMove(smOther) }
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- Init from SysModMap ----------------------------------------------- */
  explicit SysModules(SysModMap &&smlOther) :
    /* -- Initialisers ----------------------------------------------------- */
    SysModMap{ StdMove(smlOther) }
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- Dump module list ------------------------------------------- */ public:
  CVarReturn DumpModuleList(const unsigned int uiShow)
  { // No modules? Return okay
    if(!uiShow || empty()) return ACCEPT;
    // Print how many modules we are enumerating
    cLog->LogNLCInfoExSafe("System enumerating $ modules...", size());
    // For each shared module, print the data for it to log
    for(const SysModMapPair &smmpPair : *this)
    { // Get mod data and pathsplit data
      const SysModuleData &smdData = smmpPair.second;
      // Log the module data
      cLog->LogNLCInfoExSafe("- $ <$> '$' by '$' from '$'.",
        smdData.GetFileExt(), smdData.GetVersion(),
        smdData.GetDesc().empty() ?
          cCommon->CommonUnspec() : smdData.GetDesc(),
        smdData.GetVendor().empty() ?
          cCommon->CommonUnspec() : smdData.GetVendor(),
        smdData.GetLoc());
    } // Done
    return ACCEPT;
  }
};/* ----------------------------------------------------------------------- */
/* == System version ======================================================= **
** ######################################################################### **
** ## Information about the engine executable and modules loaded.         ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
class SysVersion :
  /* -- Base classes ------------------------------------------------------- */
  public SysModules                    // System modules
{ /* ----------------------------------------------------------------------- */
  const SysModuleData &smdEng;         // Engine executable information
  /* ----------------------------------------------------------------------- */
  const string_view strvBuildType,     // Build type
                    strvCompVer,       // Compiler version
                    strvCompiled,      // Compilation date
                    strvCompiler,      // Compiler name
                    strvBuildTarget;   // Target architechture
  /* -- Access to engine version data ------------------------------ */ public:
  const string_view &ENGBuildType() const { return strvBuildType; }
  const string_view &ENGCompVer() const { return strvCompVer; }
  const string_view &ENGCompiled() const { return strvCompiled; }
  const string_view &ENGCompiler() const { return strvCompiler; }
  const string_view &ENGTarget() const { return strvBuildTarget; }
  const string &ENGAuthor() const { return smdEng.GetVendor(); }
  const string &ENGComments() const { return smdEng.GetComments(); }
  const string &ENGDir() const { return smdEng.GetDir(); }
  const string &ENGDrive() const { return smdEng.GetDrive(); }
  const string &ENGExt() const { return smdEng.GetExt(); }
  const string &ENGFile() const { return smdEng.GetFile(); }
  const string &ENGFileExt() const { return smdEng.GetFileExt(); }
  const string &ENGFull() const { return smdEng.GetFull(); }
  const string &ENGLoc() const { return smdEng.GetLoc(); }
  const string &ENGName() const { return smdEng.GetDesc(); }
  const string &ENGVersion() const { return smdEng.GetVersion(); }
  unsigned int ENGBits() const { return sizeof(void*)<<3; }
  unsigned int ENGBuild() const { return smdEng.GetBuild(); }
  unsigned int ENGMajor() const { return smdEng.GetMajor(); }
  unsigned int ENGMinor() const { return smdEng.GetMinor(); }
  unsigned int ENGRevision() const { return smdEng.GetRevision(); }
  /* -- Find executable module info and return reference to it -- */ protected:
  const SysModuleData &FindBaseModuleInfo(const size_t stId) const
  { // Find the module, we stored it as a zero, if not found?
    const SysModMap &smmMap = *this;
    const SysModMapConstIt smmciIt{ smmMap.find(stId) };
    if(smmciIt == smmMap.cend())
      XC("Failed to locate executable information!",
        "Length", smmMap.size(), "Instance", stId);
    // Return version data
    return smmciIt->second;
  }
  /* -- Init from SysModMap ----------------------------------------------- */
  SysVersion(SysModMap &&smlOther, const size_t stI) :
    /* -- Initialisers ----------------------------------------------------- */
    SysModules{ StdMove(smlOther) },            // Move system modules list
    smdEng{ FindBaseModuleInfo(stI) },          // Move engine executable info
    strvBuildType{ BUILD_TYPE_LABEL },          // Build type
    strvCompVer{ COMPILER_VERSION },            // Compiler version
    strvCompiled{ VER_DATE },                   // Compilation date
    strvCompiler{ COMPILER_NAME },              // Compiler name
    strvBuildTarget{ BUILD_TARGET }             // Target architechture
    /* -- No code ---------------------------------------------------------- */
    {}
};/* ----------------------------------------------------------------------- */
/* == System common ======================================================== **
** ######################################################################### **
** ## The operating specific modules populate their data inside this      ## **
** ## class to maintain a common access for operating system information. ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
class SysCommon                        // Common system structs and funcs
{ /* -- Typedefs ------------------------------------------------ */ protected:
  const struct ExeData                 // Executable data
  { /* --------------------------------------------------------------------- */
    const unsigned int ulHeaderSum,    // Executable checksum in header
                       ulCheckSum;     // Executable actual checksum
    const bool         bExeIsModified, // True if executable is modified
                       bExeIsBundled;  // True if executable is bundled
  } /* --------------------------------------------------------------------- */
  exeData;                             // Physical executable data
  /* ----------------------------------------------------------------------- */
  const struct OSData                  // Operating system data
  { /* --------------------------------------------------------------------- */
    const string       strName,        // Os name (e.g. Windows)
                       strNameEx;      // Os host (e.g. Wine)
    const unsigned int uiMajor,        // Os major version
                       uiMinor,        // Os minor version
                       uiBuild,        // Os build number
                       uiBits;         // Os bit version
    const string       strLocale;      // Os locale
    const bool         bIsAdmin,       // Os user has elevated privileges
                       bIsAdminDef;    // Os uses admin accounts by default
  } /* --------------------------------------------------------------------- */
  osData;                              // Operating system data
  /* ----------------------------------------------------------------------- */
  const struct CPUData                 // Processor data
  { /* --------------------------------------------------------------------- */
    const size_t       stCpuCount;     // Cpu count
    const unsigned int ulSpeed,        // ~MHz
                       ulFamily,       // Family
                       ulModel,        // Model
                       ulStepping;     // Stepping
    const string       sProcessorName; // CPU id or vendor
  } /* --------------------------------------------------------------------- */
  cpuData;                             // System processor data
  /* ----------------------------------------------------------------------- */
  struct CPUUseData                    // Processor usage data
  { /* --------------------------------------------------------------------- */
    double         dProcess,           // Process cpu usage
                   dSystem;            // System cpu usage
    /* -- Default constructor ---------------------------------------------- */
    CPUUseData() :
      /* -- Initialisers --------------------------------------------------- */
      dProcess(0.0),                   // Zero process cpu usage
      dSystem(0.0)                     // Zero system cpu usage
      /* -- No code -------------------------------------------------------- */
      {}
  } /* --------------------------------------------------------------------- */
  cpuUData;                            // Processor usage data
  /* ----------------------------------------------------------------------- */
  struct MemData                       // Processor data
  { /* --------------------------------------------------------------------- */
    uint64_t       ullMTotal,          // 64-bit memory total
                   ullMFree,           // 64-bit memory free
                   ullMUsed;           // 64-bit memory used
    double         dMLoad;             // Ram use in %
    size_t         stMFree,            // 32-bit memory free
                   stMProcUse,         // Process memory usage
                   stMProcPeak;        // Peak process memory usage
    /* -- Default constructor ---------------------------------------------- */
    MemData() :
      /* -- Initialisers --------------------------------------------------- */
      ullMTotal(0),                    // Zero total memory
      ullMFree(0),                     // Zero free memory
      ullMUsed(0),                     // Zero used memory
      dMLoad(0.0),                     // Zero memory usage percentage
      stMFree(0),                      // Zero 32-bit free memory
      stMProcUse(0),                   // Zero process usage
      stMProcPeak(0)                   // Zero process peak usage
      /* -- No code -------------------------------------------------------- */
      {}
  } /* --------------------------------------------------------------------- */
  memData;                             // Memory data
  /* --------------------------------------------------------------- */ public:
  bool EXEModified() const { return exeData.bExeIsModified; }
  unsigned int EXEHeaderSum() const { return exeData.ulHeaderSum; }
  unsigned int EXECheckSum() const { return exeData.ulCheckSum; }
  bool EXEBundled() const { return exeData.bExeIsBundled; }
  /* ----------------------------------------------------------------------- */
  const string &OSName() const { return osData.strName; }
  const string &OSNameEx() const { return osData.strNameEx; }
  bool IsOSNameExSet() const { return !OSNameEx().empty(); }
  unsigned int OSMajor() const { return osData.uiMajor; }
  unsigned int OSMinor() const { return osData.uiMinor; }
  unsigned int OSBuild() const { return osData.uiBuild; }
  unsigned int OSBits() const { return osData.uiBits; }
  const string &OSLocale() const { return osData.strLocale; }
  bool OSIsAdmin() const { return osData.bIsAdmin; }
  bool OSIsAdminDefault() const { return osData.bIsAdminDef; }
  /* ----------------------------------------------------------------------- */
  size_t CPUCount() const { return cpuData.stCpuCount; }
  unsigned int CPUSpeed() const { return cpuData.ulSpeed; }
  unsigned int CPUFamily() const { return cpuData.ulFamily; }
  unsigned int CPUModel() const { return cpuData.ulModel; }
  unsigned int CPUStepping() const { return cpuData.ulStepping; }
  const string &CPUName() const { return cpuData.sProcessorName; }
  double CPUUsage() const { return cpuUData.dProcess; }
  double CPUUsageSystem() const { return cpuUData.dSystem; }
  /* ----------------------------------------------------------------------- */
  uint64_t RAMTotal() const { return memData.ullMTotal; }
  double RAMTotalMegs() const
    { return static_cast<double>(RAMTotal()) / 1048576; }
  uint64_t RAMFree() const { return memData.ullMFree; }
  double RAMFreeMegs() const
    { return static_cast<double>(RAMFree()) / 1048576; }
  uint64_t RAMUsed() const { return memData.ullMUsed; }
  size_t RAMFree32() const { return memData.stMFree; }
  double RAMFree32Megs() const
    { return static_cast<double>(RAMFree32()) / 1048576; }
  double RAMLoad() const { return memData.dMLoad; }
  size_t RAMProcUse() const { return memData.stMProcUse; }
  double RAMProcUseMegs() const
    { return static_cast<double>(RAMProcUse()) / 1048576; }
  size_t RAMProcPeak() const { return memData.stMProcPeak; }
  /* -- Constructor --------------------------------------------- */ protected:
  SysCommon(ExeData &&edExe, OSData &&osdOS, CPUData &&cpudCPU) :
    /* -- Initialisers ----------------------------------------------------- */
    exeData{ StdMove(edExe) },         // Move executable data
    osData{ StdMove(osdOS) },          // Move operating system data
    cpuData{ StdMove(cpudCPU) }        // Move processor data
    /* -- No code ---------------------------------------------------------- */
    {}
};/* ----------------------------------------------------------------------- */
/* == SysPipeBase class ==================================================== **
** ######################################################################### **
** ## Base class for SysPipe                                              ## **
** ######################################################################### **
** == System pipe base class =============================================== */
class SysPipeBase :
  /* -- Base classes ------------------------------------------------------- */
  public Ident                         // Name of the pipe
{ /* ----------------------------------------------------------------------- */
  int64_t          llExitCode;         // Process exit code
  /* -- Set status code ----------------------------------------- */ protected:
  void SysPipeBaseSetStatus(const int64_t llNewCode)
    { llExitCode = llNewCode; }
  /* -- Return status code ----------------------------------------- */ public:
  int64_t SysPipeBaseGetStatus() const { return llExitCode; }
  /* -- Constructor with init ---------------------------------------------- */
  SysPipeBase() :
    /* -- Initialisers ----------------------------------------------------- */
    llExitCode(127)                    // Standard exit code
    /* -- No code ---------------------------------------------------------- */
    {}
};/* -- End ---------------------------------------------------------------- */
/* == SysConBase class ===================================================== **
** ######################################################################### **
** ## Base class for SysCon                                               ## **
** ######################################################################### **
** -- Typedefs ------------------------------------------------------------- */
BUILD_FLAGS(SysCon,                    // Console flags classes
  /* ----------------------------------------------------------------------- */
  SCO_NONE                  {Flag(0)}, // No settings?
  SCO_CURVISIBLE            {Flag(1)}, // Cursor is visible?
  SCO_CURINSERT             {Flag(2)}, // Cursor is in insert mode?
  SCO_EXIT                  {Flag(3)}  // Exit requested?
);/* ----------------------------------------------------------------------- */
class SysConBase :
  /* -- Base classes ------------------------------------------------------- */
  protected SysConFlags                // Flags settings
{ /* -- Typedefs --------------------------------------------------- */ public:
  enum KeyType { KT_NONE, KT_KEY, KT_CHAR, KT_RESET }; // GetKey return types
  /* -- For handling CTRL_CLOSE_EVENT --------------------------- */ protected:
  condition_variable cvExit;           // Exit condition variable
  /* -- Handle CTRL_CLOSE_EVENT ------------------------------------ */ public:
  bool SysConIsClosing() const { return FlagIsSet(SCO_EXIT); }
  bool SysConIsNotClosing() const { return !SysConIsClosing(); }
  void SysConCanCloseNow() { cvExit.notify_one(); }
  /* -- Constructor --------------------------------------------- */ protected:
  SysConBase() :
    /* -- Initialisers ----------------------------------------------------- */
    SysConFlags{ SCO_NONE }            // Current flags
    /* -- No code ---------------------------------------------------------- */
    {}
};/* ----------------------------------------------------------------------- */
/* == Includes ============================================================= **
** ######################################################################### **
** ## Here we include data for the specific operating system.             ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
#if defined(WINDOWS)                   // Using windows?
# include "syswin.hpp"                 // Include windows system core
#elif defined(MACOS)                   // Using mac?
# include "sysmac.hpp"                 // Include MacOS system core
#elif defined(LINUX)                   // Using linux?
# include "sysnix.hpp"                 // Include Linux system core
#endif                                 // Done checking OS
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
  typedef IdList<16> ModeList;         // List of possible combinations
  /* ----------------------------------------------------------------------- */
  const ModeList   mList;              // Modes list
  CoreFlags        cfMode;             // Requested core subsystem flags
  ClockInterval<>  ciCpu;              // For getting cpu usage
  const size_t     stProcessId,        // Readable process id
                   stThreadId;         // Readable thread id
  string_view      strvTitle,          // Guest title
                   strvShortTitle,     // Guest short title
                   strvVersion,        // Guest version
                   strvAuthor,         // Guest author
                   strvCopyright,      // Guest copyright
                   strvDescription,    // Guest description
                   strvWebsite;        // Guest website
  /* ----------------------------------------------------------------------- */
  terminate_handler thHandler;         // Old C++ termination handler
  /* ----------------------------------------------------------------------- */
  const string     strRoamingDir;      // Roaming directory
  /* -- Callback for set_terminate() defined later ------------------------- */
  static void SysOnTerminateHandler[[noreturn]]();
  /* -- Return readable process and thread id ---------------------- */ public:
  size_t SysGetReadablePid() const { return stProcessId; }
  size_t SysGetReadableTid() const { return stThreadId; }
  /* -- Update CPU usage information --------------------------------------- */
  void SysUpdateCPUUsage()
    { if(ciCpu.CITriggerStrict()) UpdateCPUUsageData(); }
  /* -- Update and return process CPU usage -------------------------------- */
  double SysUpdateAndGetCPUUsage()
    { SysUpdateCPUUsage(); return CPUUsage(); }
  /* -- Update and return system CPU usage --------------------------------- */
  double SysUpdateAndGetCPUUsageSystem()
    { SysUpdateCPUUsage(); return CPUUsageSystem(); }
  /* -- Show message box with window handle (thiscall) --------------------- */
  unsigned int SysMsgEx(const string &strReason, const string &strMessage,
    unsigned int uiFlags = MB_ICONSTOP) const
      { return SysMessage(GetWindowHandle(),
          StrAppend(ENGName(), ' ', strReason), strMessage, uiFlags); }
  /* -- Get descriptor strings --------------------------------------------- */
  const string_view &SysGetGuestTitle() const { return strvTitle; }
  const string_view &SysGetGuestShortTitle() const { return strvShortTitle; }
  const string_view &SysGetGuestVersion() const { return strvVersion; }
  const string_view &SysGetGuestAuthor() const { return strvAuthor; }
  const string_view &SysGetGuestCopyright() const { return strvCopyright; }
  const string_view &SysGetGuestDescription() const {return strvDescription; }
  const string_view &SysGetGuestWebsite() const { return strvWebsite; }
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
  const string &SysGetRoamingDir() const { return strRoamingDir; }
  /* ----------------------------------------------------------------------- */
  const string_view &SysGetCoreFlagsString(const CoreFlagsConst cfFlags) const
    { return mList.Get(cfFlags); }
  /* ----------------------------------------------------------------------- */
  const string_view &SysGetCoreFlagsString() const
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
    ciCpu{ cd1S },                     // Cpu refresh time is one seconds
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
        exeData.ulHeaderSum, hex, exeData.ulHeaderSum, dec,
        exeData.ulCheckSum, hex, exeData.ulCheckSum, dec,
#endif
      cCmdLine->CmdLineGetStartupCWD(),
      SysGetRoamingDir(),
      SysGetReadablePid(), hex, SysGetReadablePid(), dec,
        SysGetReadableTid(), hex, SysGetReadableTid(), dec,
#if !defined(MACOS)
      GetPriority(), hex, GetPriority(), dec,
        GetAffinity(false), hex, GetAffinity(false), dec,
        GetAffinity(true), hex, GetAffinity(true), dec,
#endif
      CPUName(), CPUCount(), CPUSpeed(), CPUFamily(), CPUModel(),
        CPUStepping(),
      StrToBytes(RAMTotal()), StrToBytes(RAMFree()), StrToBytes(RAMProcUse()),
      OSName(), OSMajor(), OSMinor(), OSBuild(), OSBits(), OSLocale(),
        IsOSNameExSet() ? StrAppend(" via ", OSNameEx()) :
                          cCommon->CommonBlank(),
      StrLongFromDuration(GetUptime()),
      cmSys.FormatTime(), cmSys.FormatTimeUTC(),
      StrFromBoolTF(OSIsAdmin()), StrFromBoolTF(EXEBundled()));
  }
  /* -- CVar callbacks to update guest descriptor strings ---------- */ public:
  CVarReturn SysSetGuestTitle(const string&, const string &strV)
    { strvTitle = strV; return ACCEPT; }
  CVarReturn SysSetGuestShortTitle(const string&, const string &strV)
    { strvShortTitle = strV; return ACCEPT; }
  CVarReturn SysSetGuestVersion(const string&, const string &strV)
    { strvVersion = strV; return ACCEPT; }
  CVarReturn SysSetGuestAuthor(const string&, const string &strV)
    { strvAuthor = strV; return ACCEPT; }
  CVarReturn SysSetGuestCopyright(const string&, const string &strV)
    { strvCopyright = strV; return ACCEPT; }
  CVarReturn SysSetGuestDescription(const string&, const string &strV)
    { strvDescription = strV; return ACCEPT; }
  CVarReturn SysSetGuestWebsite(const string&, const string &strV)
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
      catch(const exception &eReason)
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
  CVarReturn SysSetWorkDir(const string &strP, string &strV)
  { // Set current directory to the startup directory as we want to honour the
    // users choice of relative directory.
    cCmdLine->CmdLineSetStartupCWD();
    // If targeting Apple systems?
#if defined(MACOS)
    // Working directory
    string strWorkDir;
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
    string strWorkDir{ strP.empty() ? ENGLoc() :
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
  CVarReturn SysCheckAdminModified(const unsigned int uiMode)
  { // Valid modes allowed
    enum Mode { AM_OK,                 // [0] Running as admin is okay?
                AM_NOTOK,              // [1] Running as admin is not okay?
                AM_NOTOKIFMODERNOS };  // [2] As above but not if OS is modern?
    // Check mode
    switch(static_cast<Mode>(uiMode))
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
/* -- Pre-defined SysBase callbacks that require access to cSystem global -- */
ENGINE_SYSBASE_CALLBACKS();            // Parse requested SysBase callbacks
#undef ENGINE_SYSBASE_CALLBACKS        // Done with this
/* -- Pre-defined SysCon callbacks that require access to cSystem global --- */
ENGINE_SYSCON_CALLBACKS();             // Parse requested SysCon callbacks
#undef ENGINE_SYSCON_CALLBACKS         // Done with this
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
