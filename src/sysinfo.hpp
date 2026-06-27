/* == SYSINFO.HPP ========================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## This is a class to store system information, engine and executable  ## **
** ## information.                                                        ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace ISysInfo {                   // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace ICommon::P;            using namespace IStd::P;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* == System common ======================================================== **
** ######################################################################### **
** ## The operating specific modules populate their data inside this      ## **
** ## class to maintain a common access for operating system information. ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
class SysInfo                          // Common system structs and funcs
{ /* -- Typedefs ------------------------------------------------ */ protected:
  const struct ExeData                 // Executable data
  { /* -- Public variables ------------------------------------------------- */
    const unsigned ulHeaderSum,        // Executable checksum in header
                   ulCheckSum;         // Executable actual checksum
    const bool     bExeIsModified,     // True if executable is modified
                   bExeIsBundled;      // True if executable is bundled
    /* -- Initialiser constructor ------------------------------------------ */
    ExeData(const unsigned ulNHeaderSum, const unsigned ulNCheckSum,
      const bool bNExeIsModified, const bool bNExeIsBundled) :
      /* -- Initialisers --------------------------------------------------- */
      ulHeaderSum(ulNHeaderSum),       ulCheckSum(ulNCheckSum),
      bExeIsModified(bNExeIsModified), bExeIsBundled(bNExeIsBundled)
      /* -- No code -------------------------------------------------------- */
      {}
  } /* --------------------------------------------------------------------- */
  exeData;                             // Physical executable data
  /* ----------------------------------------------------------------------- */
  const struct OSData                  // Operating system data
  { /* -- Public variables ------------------------------------------------- */
    const StdString strName,           // Os name (e.g. Windows)
                    strNameEx;         // Os host (e.g. Wine)
    const unsigned  uMajor,            // Os major version
                    uMinor,            // Os minor version
                    uBuild,            // Os build number
                    uBits;             // Os bit version
    const StdString strLocale;         // Os locale
    const bool      bIsAdmin,          // Os user has elevated privileges
                    bIsAdminDef;       // Os uses admin accounts by default
    /* -- Initialiser constructor ------------------------------------------ */
    OSData(StdString &&strNName, StdString &&strNNameEx,
      const unsigned uNMajor, const unsigned uNMinor, const unsigned uNBuild,
      const unsigned uNBits, StdString &&strNLocale, const bool bNIsAdmin,
      const bool bNIsAdminDef) :
      /* -- Initialisers --------------------------------------------------- */
      strName{StdMove(strNName)},      strNameEx{StdMove(strNNameEx)},
      uMajor(uNMajor),                 uMinor(uNMinor),
      uBuild(uNBuild),                 uBits(uNBits),
      strLocale{StdMove(strNLocale)},  bIsAdmin(bNIsAdmin),
      bIsAdminDef(bNIsAdminDef)
      /* -- No code -------------------------------------------------------- */
      {}
  } /* --------------------------------------------------------------------- */
  osData;                              // Operating system data
  /* ----------------------------------------------------------------------- */
  const struct CPUData                 // Processor data
  { /* -- Public variables ------------------------------------------------- */
    const size_t    stCpuCount;        // Cpu count
    const unsigned  ulSpeed,           // ~MHz
                    ulFamily,          // Family
                    ulModel,           // Model
                    ulStepping;        // Stepping
    const StdString sProcessorName;    // CPU id or vendor
    /* -- Initialiser constructor (detection success) ---------------------- */
    CPUData(const unsigned ulNSpeed, const unsigned ulNFamily,
      const unsigned ulNModel, const unsigned ulNStepping,
      StdString &&sNProcessorName) :
      /* -- Initialisers --------------------------------------------------- */
      stCpuCount(StdThreadMax()),      ulSpeed(ulNSpeed),
      ulFamily(ulNFamily),             ulModel(ulNModel),
      ulStepping(ulNStepping),         sProcessorName{StdMove(sNProcessorName)}
      /* -- No code -------------------------------------------------------- */
      {}
    /* -- Default constructor (detection failed) --------------------------- */
    CPUData() :
      /* -- Initialisers --------------------------------------------------- */
      stCpuCount(StdThreadMax()),      ulSpeed(0),
      ulFamily(0),                     ulModel(0),
      ulStepping(0),                   sProcessorName{cCommon->CommonUnspec()}
      /* -- No code -------------------------------------------------------- */
      {}
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
      ullMTotal(0),                    ullMFree(0),
      ullMUsed(0),                     dMLoad(0.0),
      stMFree(0),                      stMProcUse(0),
      stMProcPeak(0)
      /* -- No code -------------------------------------------------------- */
      {}
  } /* --------------------------------------------------------------------- */
  memData;                             // Memory data
  /* --------------------------------------------------------------- */ public:
  bool EXEModified() const { return exeData.bExeIsModified; }
  unsigned EXEHeaderSum() const { return exeData.ulHeaderSum; }
  unsigned EXECheckSum() const { return exeData.ulCheckSum; }
  bool EXEBundled() const { return exeData.bExeIsBundled; }
  /* ----------------------------------------------------------------------- */
  const StdString &OSName() const { return osData.strName; }
  const StdString &OSNameEx() const { return osData.strNameEx; }
  bool IsOSNameExSet() const { return !OSNameEx().empty(); }
  unsigned OSMajor() const { return osData.uMajor; }
  unsigned OSMinor() const { return osData.uMinor; }
  unsigned OSBuild() const { return osData.uBuild; }
  unsigned OSBits() const { return osData.uBits; }
  const StdString &OSLocale() const { return osData.strLocale; }
  bool OSIsAdmin() const { return osData.bIsAdmin; }
  bool OSIsAdminDefault() const { return osData.bIsAdminDef; }
  /* ----------------------------------------------------------------------- */
  size_t CPUCount() const { return cpuData.stCpuCount; }
  unsigned CPUSpeed() const { return cpuData.ulSpeed; }
  unsigned CPUFamily() const { return cpuData.ulFamily; }
  unsigned CPUModel() const { return cpuData.ulModel; }
  unsigned CPUStepping() const { return cpuData.ulStepping; }
  const StdString &CPUName() const { return cpuData.sProcessorName; }
  double CPUUsage() const { return cpuUData.dProcess; }
  double CPUUsageSystem() const { return cpuUData.dSystem; }
  /* ----------------------------------------------------------------------- */
  static constexpr const double dBytesPerMeg = 1048576.0;
  uint64_t RAMTotal() const { return memData.ullMTotal; }
  double RAMTotalMegs() const
    { return static_cast<double>(RAMTotal()) / dBytesPerMeg; }
  uint64_t RAMFree() const { return memData.ullMFree; }
  double RAMFreeMegs() const
    { return static_cast<double>(RAMFree()) / dBytesPerMeg; }
  uint64_t RAMUsed() const { return memData.ullMUsed; }
  size_t RAMFree32() const { return memData.stMFree; }
  double RAMFree32Megs() const
    { return static_cast<double>(RAMFree32()) / dBytesPerMeg; }
  double RAMLoad() const { return memData.dMLoad; }
  size_t RAMProcUse() const { return memData.stMProcUse; }
  double RAMProcUseMegs() const
    { return static_cast<double>(RAMProcUse()) / dBytesPerMeg; }
  size_t RAMProcPeak() const { return memData.stMProcPeak; }
  /* -- Constructor --------------------------------------------- */ protected:
  SysInfo(ExeData &&edExe, OSData &&osdOS, CPUData &&cpudCPU) :
    /* -- Initialisers ----------------------------------------------------- */
    exeData{ StdMove(edExe) },         // Move executable data
    osData{ StdMove(osdOS) },          // Move operating system data
    cpuData{ StdMove(cpudCPU) }        // Move processor data
    /* -- No code ---------------------------------------------------------- */
    {}
};/* ----------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
