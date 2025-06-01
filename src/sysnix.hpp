/* == SYSNIX.HPP =========================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## This is a Linux specific module that allows the engine to talk      ## **
** ## to, and manipulate operating system procedures and funtions.        ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* -- Classes -------------------------------------------------------------- */
#include "pixbase.hpp"                 // Base system class
#include "pixcon.hpp"                  // Console terminal window class
#include "pixmod.hpp"                  // Module information class
#include "pixmap.hpp"                  // File mapping class
#include "pixpip.hpp"                  // Process output piping class
/* == System intialisation helper ========================================== **
** ######################################################################### **
** ## Because we want to try and statically init const data as much as    ## **
** ## possible, we need this class to derive by the System class so we    ## **
** ## can make sure these functions are initialised first. Also making    ## **
** ## this private prevents us from accessing these functions because     ## **
** ## again - they are only for initialisation.                           ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
class SysProcess :                     // Need this before of System init order
  /* -- Dependency classes ------------------------------------------------- */
  private Ident                        // Mutex identifier
{ /* -- Streams ------------------------------------------------- */ protected:
  FStream          fsDevRandom,        // Handle to dev/random (rng)
                   fsProcStat,         // Handle to proc/stat (cpu)
                   fsProcStatM;        // Handle to proc/statm (memory)
  /* -- Processor ---------------------------------------------------------- */
  clock_t          ctUser,             // Last user cpu time
                   ctLow,              // Last low cpu time
                   ctSystem,           // Last system cpu time
                   ctIdle,             // Last clock idle time
                   ctProc,             // Last process cpu time
                   ctProcUser,         // Last process user cpu time
                   ctProcSys;          // Last process system cpu time
  /* -- Process ------------------------------------------------------------ */
  const size_t     stPageSize;         // Memory page size
  /* -------------------------------------------------------------- */ private:
  const pid_t      ullProcessId;       // Process id
  const pthread_t  vpThreadId;         // Thread id
  /* -- Return process and thread id ---------------------------- */ protected:
  template<typename IntType=decltype(ullProcessId)>IntType GetPid(void) const
    { return static_cast<IntType>(ullProcessId); }
  template<typename IntType=decltype(vpThreadId)>IntType GetTid(void) const
    { return static_cast<IntType>(vpThreadId); }
  /* -- Initialise global mutex ------------------------------------ */ public:
  bool InitGlobalMutex(const string_view &strvTitle)
  { // Initialise mutex ident
    IdentSet(strvTitle);
    // Create the semaphore and if the creation failed?
    if(shm_open(strvTitle.data(), O_CREAT | O_EXCL, 0) == -1)
    { // If it was because it already exists?
      if(StdIsError(EEXIST) || StdIsError(EACCES))
      { // Execution may not continue
        cLog->LogWarningSafe(
         "System detected another instance of this application running.");
        return false;
      } // Report error
      cLog->LogWarningExSafe("System failed to setup global mutex: $!",
        SysError());
    } // Execution may continue
    return true;
  }
  /* -- Destructor --------------------------------------------------------- */
  ~SysProcess(void)
  { // Unlink the mutex and show warning in log if failed
    if(IdentIsSet() && shm_unlink(IdentGetData()))
      cLog->LogWarningExSafe("SysMutex could not delete old mutex '$': $",
        IdentGet(), SysError());
  }
  /* -- Constructor -------------------------------------------------------- */
  SysProcess(void) :
    /* -- Initialisers ----------------------------------------------------- */
    fsDevRandom{ "/dev/random",        // Open dev random garbage stream
                 FM_R_B },             // - Read/Binary mode
    fsProcStat{ "/proc/stat",          // Open proc cpu stats stream
                FM_R_B },              // - Read/Binary mode
    fsProcStatM{ "/proc/self/statm",   // Open proc memory stats stream
                 FM_R_B },             // - Read/Binary mode
    ctUser(0),                         // Init user cpu time
    ctLow(0),                          // Init low cpu time
    ctSystem(0),                       // Init system cpu time
    ctIdle(0),                         // Init idle cpu time
    ctProc(0),                         // Init process cpu time
    ctProcUser(0),                     // Init user process cpu time
    ctProcSys(0),                      // Init system process cpu time
    stPageSize(sysconf(_SC_PAGESIZE)), // Get memory page size
    ullProcessId(getpid()),            // Get native process id
    vpThreadId(pthread_self())         // Get native thread id
    /* -- No code ---------------------------------------------------------- */
    { }
};/* == Class ============================================================== */
class SysCore :
  /* -- Base classes ------------------------------------------------------- */
  public SysProcess,                   // System process object
  public SysCon,                       // Defined in 'pixcon.hpp"
  public SysCommon                     // Common system object
{ /* -- Variables ---------------------------------------------------------- */
  bool             bWindowInitialised; // Is window initialised?
  const bool       bIsWayland;         // Is using wayland
  /* --------------------------------------------------------------- */ public:
  void UpdateMemoryUsageData(void)
  { // If the stat file is opened
    if(fsProcStatM.FStreamIsReadyRead())
    { // Read string from stat
      string strStat{ fsProcStatM.FStreamReadStringChunked() };
      if(!strStat.empty())
      { // Find line feed
        const size_t stLF = strStat.find('\n');
        if(stLF != StdNPos)
        { // Truncate the end of string. We only care about the top line.
          strStat.resize(stLF);
          // Grab tokens and if we have enough?
          const TokenListNC tStats{ strStat, cCommon->CommonSpace(), 8 };
          if(tStats.size() >= 3)
          { // We're only interested in the first value
            memData.stMProcUse = StrToNum<size_t>(tStats[1]) * stPageSize;
            // Check for new process peak
            if(memData.stMProcUse > memData.stMProcPeak)
              memData.stMProcPeak = memData.stMProcUse;
          } // Token count mismatch so reset memory value
          else memData.stMProcUse = 0;
        }
      } // Failed to read file
      else memData.stMProcUse = 0;
      // Go back to start for next read
      fsProcStatM.FStreamRewind();
    } // Stat file is closed
    else memData.stMProcUse = 0;
    // Grab system memory information and if successful?
    struct sysinfo siData;
    if(!sysinfo(&siData))
      memData.qMTotal = siData.totalram,
      memData.qMFree = siData.freeram + siData.bufferram,
      memData.qMUsed = memData.qMTotal - memData.qMFree,
      memData.stMFree = UtilMinimum(memData.qMFree, 0xFFFFFFFF),
      memData.dMLoad = UtilMakePercentage(memData.qMUsed, memData.qMTotal);
    // Failed
    else memData.qMTotal = memData.qMFree = memData.qMUsed = 0,
         memData.stMFree = 0,
         memData.dMLoad = 0;
  }
  /* -- Return operating system uptime (cmHiRes.GetTimeS() doesn't work!) -- */
  StdTimeT GetUptime(void)
  { // Get uptime
    struct timespec tsData;
    clock_gettime(CLOCK_MONOTONIC, &tsData);
    // Return as time_t (future ref, also has t.tv_nsec)
    return static_cast<time_t>(tsData.tv_sec);
  }
  /* -- Terminate a process ------------------------------------------------ */
  bool TerminatePid(const unsigned int uiPid) const
    { return !kill(uiPid, SIGTERM); }
  /* -- Check if specified process id is running --------------------------- */
  bool IsPidRunning(const unsigned int uiPid) const
    { return !kill(uiPid, 0); }
  /* -- GLFW handles the icons on this ------------------------------------- */
  void UpdateIcons(void) { }
  /* ----------------------------------------------------------------------- */
  static bool LibFree(void*const vpModule)
    { return vpModule && !dlclose(vpModule); }
  /* ----------------------------------------------------------------------- */
  template<typename T>static T LibGetAddr(void*const vpModule,
    const char *cpName) { return vpModule ?
      reinterpret_cast<T>(dlsym(vpModule, cpName)) : nullptr; }
  /* ----------------------------------------------------------------------- */
  static void *LibLoad(const char*const cpName)
    { return dlopen(cpName, RTLD_LAZY | RTLD_LOCAL); }
  /* ----------------------------------------------------------------------- */
  const string LibGetName(void*const vpModule, const char *cpAltName) const
  { // Return nothing if no module
    if(!vpModule) return {};
    // Get information about the shared object
    struct Lib::OS::link_map *lmData;
    if(dlinfo(vpModule, RTLD_DI_LINKMAP, &lmData) || !lmData)
      XCL("Failed to read info about shared object!", "File", cpAltName);
    // Get full pathname of file
    return StdMove(PathSplit{ lmData->l_name, true }.strFull);
  }
  /* ----------------------------------------------------------------------- */
  void UpdateCPUUsageData(void)
  { // If the stat file is opened
    if(fsProcStatM.FStreamIsReadyRead())
    { // Read string from stat and if succeeded?
      string strStat{ fsProcStat.FStreamReadStringChunked() };
      if(!strStat.empty())
      { // Find line feed
        const size_t stLF = strStat.find('\n');
        if(stLF != StdNPos)
        { // Truncate the end of string. We only care about the top line.
          strStat.resize(stLF);
          // First item must be cpu and second should be empty. We created the
          // string so this tokeniser class is allowed to modify it for
          // increased performance of processing it.
          const TokenListNC tStats{ strStat, cCommon->CommonSpace(), 6 };
          if(tStats.size() >= 5)
          { // Get idle time
            const clock_t cUserNow = StrToNum<clock_t>(tStats[2]),
                          cLowNow = StrToNum<clock_t>(tStats[3]),
                          cSystemNow = StrToNum<clock_t>(tStats[4]),
                          cIdleNow = StrToNum<clock_t>(tStats[5]);
            // Check for valid time
            if(cUserNow < ctUser || cLowNow < ctLow ||
               cSystemNow < ctSystem || cIdleNow < ctIdle)
                 cpuUData.dSystem = -1;
            // Valid time
            else
            { // Grab cpu usage jiffies
              clock_t cTotal = ((cUserNow - ctUser) + (cLowNow - ctLow) +
                (cSystemNow - ctSystem));
              // Set the percent to this
              cpuUData.dSystem = cTotal;
              // Add the idle jiffies to the total jiffies
              cTotal += (cIdleNow - ctIdle);
              // Now we can calculate the true percent
              cpuUData.dSystem =
                UtilMakePercentage(cpuUData.dSystem, cTotal);
            } // Update new times;
            ctUser = cUserNow, ctLow = cLowNow, ctSystem = cSystemNow,
              ctIdle = cIdleNow;
          } // No header
          else cpuUData.dSystem = -2;
        } // No line feed
        else cpuUData.dSystem = -3;
      } // Read failed
      else cpuUData.dSystem = -4;
      // Go back to start for next read
      fsProcStat.FStreamRewind();
    } // Stat file is not opened
    else cpuUData.dSystem = -5;
    // Get cpu times
    struct tms tmsData;
    const clock_t cProcNow = times(&tmsData);
    // If times are not valid? Show percent as error
    if(cProcNow <= ctProc || tmsData.tms_stime < ctProcSys ||
                             tmsData.tms_utime < ctProcUser)
      cpuUData.dProcess = -1;
    // Times are valid
    else
    { // Caclulate total time
      cpuUData.dProcess = (tmsData.tms_stime - ctProcSys) +
                           (tmsData.tms_utime - ctProcUser);
      // Divide by total cpu time
      cpuUData.dProcess /= (cProcNow - ctProc);
      cpuUData.dProcess /= StdThreadMax();
      cpuUData.dProcess *= 100;
      // Update times
      ctProc = cProcNow, ctProcSys = tmsData.tms_stime,
        ctProcUser = tmsData.tms_utime;
    }
  }
  /* -- Seek to position in specified handle ------------------------------- */
  template<typename IntType>
    static IntType SeekFile(int iFp, const IntType itP)
      { return static_cast<IntType>
          (lseek64(iFp, static_cast<off64_t>(itP), SEEK_SET)); }
  /* -- Get executable size from header (N/A on Linux) --------------------- */
  static size_t GetExeSize(const string &strFile)
  { // Machine byte order magic
    constexpr const unsigned int uiELFDataNative =
#if defined(LITTLEENDIAN)         // Intel, ARM, etc.
      ELFDATA2LSB;
#elif defined(BIGENDIAN)          // PPC, etc.
      ELFDATA2MSB;
#endif
    // Open exe file and return on error
    if(FStream fExe{ strFile, FM_R_B })
    { // Read in the header
      Elf64_Ehdr ehData;
      if(const size_t stRead = fExe.FStreamReadSafe(&ehData, sizeof(ehData)))
      { // We read enough bytes?
        if(stRead == sizeof(ehData))
        { // Rewind back to start
          if(!fExe.FStreamRewind())
            XCL("Failed to rewind executable file!", "File", strFile);
          // Get ELF data order type and if correct byte order?
          const unsigned int uiType = ehData.e_ident[EI_DATA];
          if(uiType == ELFDATA2LSB || uiType == ELFDATA2MSB)
          { // Is a 32-bit executable?
            const unsigned int uiClass = ehData.e_ident[EI_CLASS];
            if(uiClass == ELFCLASS32)
            { // Read in 32-bit header
              Elf32_Ehdr ehData32;
              if(const size_t stRead2 =
                fExe.FStreamReadSafe(&ehData32, sizeof(ehData32)))
              { // We read enough bytes?
                if(stRead2 == sizeof(ehData32))
                { // Reverse bytes if not native
                  if(uiType != uiELFDataNative)
                    ehData.e_shoff = SWAP_U32(ehData32.e_shoff),
                    ehData.e_shentsize = SWAP_U16(ehData32.e_shentsize),
                    ehData.e_shnum = SWAP_U16(ehData32.e_shnum);
                  else ehData.e_shoff = ehData32.e_shoff,
                       ehData.e_shentsize = ehData32.e_shentsize,
                       ehData.e_shnum = ehData32.e_shnum;
                } // Failed to read enough bytes for ELF32 header
                else XC("Failed to read enough bytes for ELF32 header!",
                        "Requested", sizeof(ehData32), "Actual", stRead,
                        "File", strFile);
              } // Failed to read enough bytes for ELF32 header
              else XCL("Failed to read ELF32 header!",
                       "Requested", sizeof(ehData32), "File", strFile);
            } // Or is a 64-bit executable?
            else if(uiClass == ELFCLASS64)
            { // Read in 64-bit header
              Elf64_Ehdr ehData64;
              if(const size_t stRead2 =
                fExe.FStreamReadSafe(&ehData64, sizeof(ehData64)))
              { // We read enough bytes?
                if(stRead2 == sizeof(ehData64))
                { // Reverse bytes if not native
                  if(uiType != uiELFDataNative)
                    ehData.e_shoff = SWAP_U64(ehData64.e_shoff),
                    ehData.e_shentsize = SWAP_U16(ehData64.e_shentsize),
                    ehData.e_shnum = SWAP_U16(ehData64.e_shnum);
                  else ehData.e_shoff = ehData64.e_shoff,
                       ehData.e_shentsize = ehData64.e_shentsize,
                       ehData.e_shnum = ehData64.e_shnum;
                } // Failed to read enough bytes for ELF64 header
                else XC("Failed to read enough bytes for ELF64 header!",
                        "Requested", sizeof(ehData64), "Actual", stRead,
                        "File", strFile);
              } // Failed to read enough bytes for 64-bit header
              else XCL("Failed to read ELF64 header!",
                       "Requested", sizeof(ehData64), "File", strFile);
            } // Unknown executable type
            else XC("Invalid ELF header architecture!",
                  "Requested", ELFCLASS32, "OrRequested", ELFCLASS64,
                  "Actual", uiClass, "File", strFile);
            // Now we can return result
            return ehData.e_shoff + (ehData.e_shentsize * ehData.e_shnum);
          } // Failed to detect executable type
          else XC("Invalid ELF executable type!",
                  "Requested", ELFDATA2LSB, "OrRequested", ELFDATA2MSB,
                  "Actual", uiType, "File", strFile);
        } // Failed to read enough bytes
        else XC("Failed to read enough bytes for ELF header!",
                "Requested", sizeof(ehData), "Actual", stRead,
                "File", strFile);
      } // Failed to read elf ident
      else XCL("Failed to read ELF header!",
               "Requested", sizeof(ehData), "File", strFile);
    } // Failed to open executable file to read
    else XCL("Failed to open executable!", "File", strFile);
  }
  /* -- Get executable file name ------------------------------------------- */
  const string GetExeName(void)
  { // Storage for executable name
    string strName; strName.resize(PATH_MAX);
    strName.resize(readlink("/proc/self/exe",
      const_cast<char*>(strName.data()), strName.size()));
    return strName;
  }
  /* -- Enum modules ------------------------------------------------------- */
  SysModMap EnumModules(void)
  { // Make verison string
    string strVersion{ StrAppend(numeric_limits<void*>::digits,
      "-bit version") };
    // Mod list
    SysModMap smmMap;
    smmMap.emplace(make_pair(0UL, SysModule{ GetExeName(), VER_MAJOR,
      VER_MINOR, VER_BUILD, VER_REV, VER_AUTHOR, VER_NAME, StdMove(strVersion),
      VER_STR }));
    // Module list which includes the executable module
    return smmMap;
  }
  /* ----------------------------------------------------------------------- */
  OSData GetOperatingSystememData(void)
  { // Get operating system name
    struct utsname utsnData;
    if(uname(&utsnData)) XCS("Failed to read operating system information!");
    // Tokenize version numbers
    const Token tVersion{ utsnData.release, cCommon->CommonPeriod() };
    // Get LANGUAGE code and set default if not found
    string strCode{ cCmdLine->CmdLineGetEnv("LANGUAGE") };
    if(strCode.size() != 5)
    { // Get LANG code and set default if not found
      strCode = cCmdLine->CmdLineGetEnv("LANG");
      if(strCode.size() < 5) strCode = "en_GB.UTF8";
    } // Set global locale and show error if failed
    if(!setlocale(LC_ALL, strCode.c_str()))
      XCL("Failed to initialise default locale!", "Locale", strCode);
    // Replace underscore with dash to be consistent with Windows
    if(strCode[2] == '_') strCode[2] = '-';
    // Find a period (e.g. "en_GB.UTF8") and remove suffix it if found
    const size_t stPeriod = strCode.find('.');
    if(stPeriod != StdNPos) strCode = strCode.substr(0, stPeriod);
    // Return operating system info
    return { utsnData.sysname, cCommon->CommonBlank(),
      tVersion.empty()    ? 0 : StrToNum<unsigned int>(tVersion[0]),
      tVersion.size() < 2 ? 0 : StrToNum<unsigned int>(tVersion[1]),
      tVersion.size() < 3 ? 0 : StrToNum<unsigned int>(tVersion[2]),
      sizeof(void*)<<3, StdMove(strCode), DetectElevation(), false };
  }
  /* ----------------------------------------------------------------------- */
  ExeData GetExecutableData(void) { return { 0, 0, false, false }; }
  /* ----------------------------------------------------------------------- */
  CPUData GetProcessorData(void)
  {  // Open cpu information file
    if(FStream fsCpuInfo{ "/proc/cpuinfo", FM_R_B })
    { // Read file and if we got data?
      const string strFile{ fsCpuInfo.FStreamReadStringChunked() };
      if(!strFile.empty())
      { // Parse the variables and if we got some?
        ParserConst<> pcParser{ strFile, cCommon->CommonLf(), ':' };
        if(!pcParser.empty())
        { // Move stirngs from loaded variables
          string strCpuId{ StdMove(pcParser.ParserGet("model name")) },
                 strSpeed{ StdMove(pcParser.ParserGet("cpu MHz")) },
                 strVendor{ StdMove(pcParser.ParserGet("vendor_id")) },
                 strFamily{ StdMove(pcParser.ParserGet("cpu family")) },
                 strModel{ StdMove(pcParser.ParserGet("model")) },
                 strStepping{ StdMove(pcParser.ParserGet("stepping")) };
          // Remove excessive whitespaces from strings
          StrCompactRef(strCpuId);
          StrCompactRef(strSpeed);
          StrCompactRef(strVendor);
          StrCompactRef(strFamily);
          StrCompactRef(strModel);
          StrCompactRef(strStepping);
          // Fail-safe any empty strings
          if(strSpeed.empty()) strSpeed = cCommon->CommonZero();
          if(strVendor.empty()) strVendor = cCommon->CommonUnspec();
          if(strCpuId.empty()) strCpuId = strVendor;
          if(strFamily.empty()) strFamily = cCommon->CommonZero();
          if(strModel.empty()) strModel = cCommon->CommonZero();
          if(strStepping.empty()) strStepping = cCommon->CommonZero();
          // Make processor id so it is consistent with the other platforms
          // Return strings
          return { StdThreadMax(),
                   StrToNum<unsigned int>(strSpeed),
                   StrToNum<unsigned int>(strFamily),
                   StrToNum<unsigned int>(strModel),
                   StrToNum<unsigned int>(strStepping),
                   StdMove(strCpuId) };
        } // Failed to parse cpu variables
        else cLog->LogWarningSafe("Could not parse cpu information file!");
      } // Failed to read cpu info failed
      else cLog->LogWarningExSafe("Could not read cpu information file: $!",
        StrFromErrNo());
    } // Failed to open cpu info file
    else cLog->LogWarningExSafe("Could not open cpu information file: $!",
      StrFromErrNo());
    // Return default data we could not read
    return { StdThreadMax(), 0, 0, 0, 0, cCommon->CommonUnspec() };
  }
  /* ----------------------------------------------------------------------- */
  bool DebuggerRunning(void) const { return false; }
  /* -- Get process affinity masks ----------------------------------------- */
  uint64_t GetAffinity(const bool bS)
  { // Get the process affinity mask
    cpu_set_t cstMask;
    if(sched_getaffinity(GetPid(), sizeof(cstMask), &cstMask))
      XCS("Failed to acquire process affinity!");
    // Return value
    uint64_t qwAffinity = 0;
    // Fill in the mask
    for(size_t stIndex = 0,
               stMaximum = UtilMinimum(64, CPU_COUNT(&cstMask));
               stIndex < stMaximum;
             ++stIndex)
      qwAffinity |= CPU_ISSET(stIndex, &cstMask) << stIndex;
    // Return affinity mask
    return qwAffinity;
  }
  /* ----------------------------------------------------------------------- */
  int GetPriority(void)
  { // Get priority value and throw if failed
    const int iNice = getpriority(PRIO_PROCESS, GetPid());
    if(iNice == -1) XCS("Failed to acquire process priority!");
    // Return priority
    return iNice;
   }
  /* -- Return if running as root ------------------------------------------ */
  bool DetectElevation(void) { return getuid() == 0; }
  /* -- Return data from /dev/urandom -------------------------------------- */
  Memory GetEntropy(void)
    { return fsDevRandom.FStreamReadBlockSafe(stPageSize); }
  /* ----------------------------------------------------------------------- */
  void *GetWindowHandle(void) const { return nullptr; }
  /* -- A window was created ----------------------------------------------- */
  void WindowInitialised(GlFW::GLFWwindow*const gwWindow)
    { bWindowInitialised = !!gwWindow; }
  /* -- Window was destroyed, nullify handles ------------------------------ */
  void SetWindowDestroyed(void) { bWindowInitialised = false; }
  /* -- Help with debugging ------------------------------------------------ */
  const char *HeapCheck(void) const { return "Not implemented!"; }
  /* ----------------------------------------------------------------------- */
  int LastSocketOrSysError(void) const { return StdGetError(); }
  /* -- Returns if using wayland ------------------------------------------- */
  bool IsWayland(void) const { return bIsWayland; }
  /* -- Build user roaming directory ---------------------------- */ protected:
  const string BuildRoamingDir(void) const
    { return cCmdLine->CmdLineMakeEnvPath("HOME", "/.local"); }
  /* -- Constructor -------------------------------------------------------- */
  SysCore(void) :
    /* -- Initialisers ----------------------------------------------------- */
    SysCon{ EnumModules(), 0 },
    SysCommon{ GetExecutableData(),
               GetOperatingSystememData(),
               GetProcessorData() },
    bWindowInitialised(false),
    bIsWayland(!cCmdLine->CmdLineGetEnv("WAYLAND_DISPLAY").empty())
    /* -- No code ---------------------------------------------------------- */
    { }
};/* ----------------------------------------------------------------------- */
/* == EoF =========================================================== EoF == */
