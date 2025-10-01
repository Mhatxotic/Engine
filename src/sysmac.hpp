/* == SYSMAC.HPP =========================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## This is a MacOS specific module that allows the engine to talk      ## **
** ## to, and manipulate operating system procedures and funtions.        ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* -- Dependiencies -------------------------------------------------------- */
#include "pixbase.hpp"                 // Base system class
#include "pixcon.hpp"                  // Console emulation class
#include "pixmod.hpp"                  // Version information class
#include "pixmap.hpp"                  // FileMap memory mapping class
#include "pixpip.hpp"                  // Process pipe handling
/* ------------------------------------------------------------------------- */
#undef _XOPEN_SOURCE_EXTENDED          // Done with this macro
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
{ /* -- Private variables -------------------------------------------------- */
  const pid_t      piProcessId;        // Process id
  const pthread_t  vpThreadId;         // Thread id
  /* -- Protected variables ------------------------------------- */ protected:
  const uint64_t   qwPageSize;         // Memory page size
  const mach_port_t mptHost, mptTask;  // Current mach host and task id
  /* -- Shared memory ------------------------------------------------------ */
  pid_t           *pipProcessId;       // Process id (OS level memory)
  static constexpr size_t stPidSize = sizeof(piProcessId); // Size of a pid
  /* -- Return process and thread id --------------------------------------- */
  template<typename IntType=decltype(piProcessId)>IntType GetPid(void) const
    { return static_cast<IntType>(piProcessId); }
  template<typename IntType=decltype(vpThreadId)>IntType GetTid(void) const
    { return static_cast<IntType>(UtilBruteCast<const size_t>(vpThreadId)); }
  /* -- Initialise global mutex ------------------------------------ */ public:
  bool InitGlobalMutex(const string_view &strvTitle)
  { // Initialise mutex ident
    IdentSet(strvTitle);
    // Shared memory file descriptor helper
    class Shm
    { /* -- Private variables ---------------------------------------------- */
      const string_view &strvTitle; // Filename of shm object
      int               iFd,        // The file descriptor of the shm object
                        iMode;      // Requested mode for the shm object
      /* -- Return mode -------------------------------------------- */ public:
      int Mode(void) const { return iMode; }
      /* -- Get file descriptor -------------------------------------------- */
      int Get(void) const { return iFd; }
      /* -- Resize file descriptor content --------------------------------- */
      bool Truncate(const size_t stSize) const
        { return !ftruncate(Get(), static_cast<off_t>(stSize)); }
      /* -- Close the file descriptor -------------------------------------- */
      bool Close(void)
      { // Return success if already closed
        if(Get() < 0) return true;
        // Do the close and return failure if failed
        if(close(Get())) return false;
        // Reset the file descriptor
        iFd = -1;
        // Success
        return true;
      }
      /* -- Open the file descriptor --------------------------------------- */
      bool Open(const int iNMode)
      { // Close previous file descriptor
        Close();
        // Set new mode
        iMode = iNMode;
        // Open and assign the new descriptor
        iFd = shm_open(strvTitle.data(), iMode, 0600);
        // Return if the open succeeded
        return Get() >= 0;
      }
      /* -- Destructor that closes the file descriptor --------------------- */
      ~Shm(void) { if(Get() >= 0) close(Get()); }
      /* -- Constructor that initialises variables ------------------------- */
      explicit Shm(const string_view &strvNTitle) :
        /* -- Initialisers ------------------------------------------------- */
        strvTitle(strvNTitle),         // Set reference to title
        iFd(-1),                       // File descriptor uninitialised
        iMode(0)                       // Mode uninitialised
        /* -- No code ------------------------------------------------------ */
        { }
      /* -- Initialise a single object that automatically cleans up -------- */
    } shmShm{ strvTitle };
    // Create the semaphore and if an error occurs?
    if(!shmShm.Open(O_CREAT|O_EXCL|O_WRONLY))
    { // Report error if it isn't because the semaphore already exists
      if(StdIsNotError(EEXIST))
        XCL("Failed to setup shared memory object for exclusive writing!",
            "Name", strvTitle, "Mode", shmShm.Mode());
      // Try opening it again for reading with exclusivity
      if(!shmShm.Open(O_RDONLY|O_EXCL))
        XCL("Failed to setup shared memory object for exclusive reading!",
            "Name", strvTitle, "Mode", shmShm.Mode());
      // Initialise memory
      pipProcessId = StdMMap<pid_t>
        (nullptr, stPidSize, PROT_READ, MAP_SHARED, shmShm.Get(), 0);
      if(pipProcessId == MAP_FAILED)
        XCL("Failed to setup shared memory for reading!",
            "Name", strvTitle, "ObjectFD", shmShm.Get(), "Size", stPidSize);
      // Test if the process exists
      const pid_t pPId = *pipProcessId;
      if(getpgid(pPId) >= 0)
      { // Put in log that another instance of this application is running and
        // return to caller that execution must cease.
        cLog->LogWarningExSafe("System detected another instance of this "
          "application running at pid $.", pPId);
        // Disable deprecation warnings as we need to use this
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
        // Get process serial number from pid and show error if failed
        ProcessSerialNumber psnApp;
        if(const OSStatus ossErr = GetProcessForPID(pPId, &psnApp))
          cLog->LogErrorExSafe("System can't get serial for pid $ (code $)!",
            pPId, ossErr);
        // Try to activate the app with specified serial number
        else if(const OSStatus ossErr = SetFrontProcess(&psnApp))
          cLog->LogErrorExSafe(
            "System can't activate app with pid $ (code $)!", pPId, ossErr);
        // Success
        else cLog->LogInfoExSafe("System activated application at pid $.",
          pPId);
        // Restore deprecation warning
#pragma clang diagnostic pop
        // Done
        return false;
      } // Put in log that another instance of this application is running.
      cLog->LogWarningExSafe(
        "Previous pid of $ not valid so assuming no previous instance.", pPId);
      // Unmap previous shared memory
      if(munmap(pipProcessId, stPidSize))
        XCL("Failed to unmap shared memory from reading!", "Name", strvTitle);
      // Try reopening for exclusive writing again
      if(!shmShm.Open(O_WRONLY|O_EXCL))
        XCL("Failed to setup shared memory object for exclusive writing!",
            "Name", strvTitle, "Mode", shmShm.Mode());
    } // Make sure it is the correct size
    if(!shmShm.Truncate(stPidSize) && StdIsNotError(EINVAL))
      XCL("Failed to truncate shared memory object!",
          "Name", strvTitle, "ObjectFD", shmShm.Get(), "Size", stPidSize);
    // Initialise memory
    pipProcessId = StdMMap<pid_t>
      (nullptr, stPidSize, PROT_WRITE, MAP_SHARED, shmShm.Get(), 0);
    if(pipProcessId == MAP_FAILED)
      XCL("Failed to setup shared memory for writing!",
          "Name", strvTitle, "ObjectFD", shmShm.Get(), "Size", stPidSize);
    // Write the current PID to shared memory
    *pipProcessId = piProcessId;
    // Execution can continue
    return true;
  }
  /* -- Destructor --------------------------------------------------------- */
  ~SysProcess(void)
  { // Unmap the memory containing the pid and reset the pid memory address
    if(pipProcessId && munmap(pipProcessId, stPidSize))
      cLog->LogWarningExSafe("System failed to unmap shared memory of size "
        "$ bytes! $", stPidSize, SysError());
    // Unlink the shared memory object if data is set
    if(IdentIsSet() && shm_unlink(IdentGetData()))
      cLog->LogWarningExSafe("System failed to unlink shared memory object "
        "'$'! $", IdentGet(), SysError());
  }
  /* -- Constructor -------------------------------------------------------- */
  SysProcess(void) :
    /* -- Initialisers ----------------------------------------------------- */
    piProcessId(getpid()),             // Initialise process id number
    vpThreadId(pthread_self()),        // Initialise main thread id number
    qwPageSize(static_cast<uint64_t>   // Initialise memory page size
      (sysconf(_SC_PAGESIZE))),        // Usually 16k on M1 or 4k on Intel
    mptHost(mach_host_self()),         // Initialise host task
    mptTask(mach_task_self()),         // Initialise self task
    pipProcessId(nullptr)              // Process id memory not available
    /* -- No code ---------------------------------------------------------- */
    { }
};/* == Class ============================================================== */
class SysCore :
  /* -- Dependency classes ------------------------------------------------- */
  public SysProcess,                   // Process information class
  public SysCon,                       // Defined in 'pixcon.hpp'
  public SysCommon                     // Common functions class
{ /* -- Variables ---------------------------------------------------------- */
  bool             bWindowInitialised; // Is window initialised?
  /* ----------------------------------------------------------------------- */
  const string GetSysCTLInfoString(const char *cpS)
  { // Get the size and return error if failed
    size_t stSize = 0;
    if(sysctlbyname(cpS, nullptr, &stSize, nullptr, 0) < 0) return "#ERR1#";
    // Resize and fill string
    string strOut; strOut.resize(stSize - 1);
    if(sysctlbyname(cpS, UtfToNonConstCast<char*>(strOut.c_str()),
      &stSize, nullptr, 0) < 0)
        return cCommon->CommonBlank();
    // Return the string
    return strOut;
  }
  /* ----------------------------------------------------------------------- */
  template<typename T>const T GetSysCTLInfoNum(const char *cpS)
  { // Resize
    T tOut;
    // Size
    size_t stOut = sizeof(tOut);
    // Return the number
    return sysctlbyname(cpS, &tOut, &stOut, nullptr, 0) < 0 ? 0 : tOut;
  }
  /* ----------------------------------------------------------------------- */
  void InitMemorySize(void)
  { // Store real total memory and return if succesful
    memData.qMTotal = GetSysCTLInfoNum<uint64_t>("hw.memsize");
    if(memData.qMTotal) return;
    // Update memory information and set total from current data instead
    UpdateMemoryUsageData();
    memData.qMTotal = memData.qMFree + memData.qMUsed;
  }
  /* ----------------------------------------------------------------------- */
  void UpdateProcessCpuUsage(void)
  { // For cpu information directly from operating system
    struct task_thread_times_info ttiInfo;
    mach_msg_type_number_t mmtnCount = TASK_THREAD_TIMES_INFO_COUNT;
    // Get process info and return if we can't
    if(task_info(mptTask, TASK_THREAD_TIMES_INFO,
         reinterpret_cast<task_info_t>(&ttiInfo), &mmtnCount) != KERN_SUCCESS)
      return;
    // Do cpu time calculations
    const uint64_t
      uqUserTime = static_cast<uint64_t>(ttiInfo.user_time.seconds) *
        1000000ULL + static_cast<uint64_t>(ttiInfo.user_time.microseconds),
      uqSystemTime = static_cast<uint64_t>(ttiInfo.system_time.seconds) *
        1000000ULL + static_cast<uint64_t>(ttiInfo.system_time.microseconds),
      uqTotalTime = uqUserTime + uqSystemTime,
      uqTimeNow = cmHiRes.GetTimeUS();
    // Persistent last times
    static uint64_t uqTimeNowL = 0, uqTotalTimeL = 0;
    // Calculate cpu usage. Sometimes total time can be less.
    if(uqTotalTime >= uqTotalTimeL)
      cpuUData.dProcess = UtilMakePercentage(uqTotalTime - uqTotalTimeL,
        uqTimeNow - uqTimeNowL) / StdThreadMax();
    // Update times for next query
    uqTotalTimeL = uqTotalTime;
    uqTimeNowL = uqTimeNow;
  }
  /* ----------------------------------------------------------------------- */
  void UpdateSystemCpuUsage(void)
  { // For cpu information directly from operating system
    mach_msg_type_number_t mmtnType = HOST_CPU_LOAD_INFO_COUNT;
    host_cpu_load_info_data_t hclidData;
    // Get the usage and if succeeded?
    if(host_statistics(mptHost, HOST_CPU_LOAD_INFO,
         reinterpret_cast<host_info_t>(&hclidData), &mmtnType) != KERN_SUCCESS)
      return;
    // Calculate the total ticks
    uint64_t uqTotalTicks = 0;
    for(size_t stIndex = 0; stIndex < CPU_STATE_MAX; ++stIndex)
      uqTotalTicks += hclidData.cpu_ticks[stIndex];
    // Persistent last times
    static uint64_t uqTotalTicksL = 0, uqIdleTicksL = 0;
    // Do calculations
    const uint64_t
      uqIdleTicks = hclidData.cpu_ticks[CPU_STATE_IDLE],
      uqTotalTicksSinceLastTime = uqTotalTicks - uqTotalTicksL,
      uqIdleTicksSinceLastTime = uqIdleTicks - uqIdleTicksL;
    // Set previous values
    uqTotalTicksL = uqTotalTicks;
    uqIdleTicksL = uqIdleTicks;
    // Final calculation
    cpuUData.dSystem = (1.0 - ((uqTotalTicksSinceLastTime > 0) ?
      (static_cast<double>(uqIdleTicksSinceLastTime)) /
      uqTotalTicksSinceLastTime : 0)) * 100;
  }
  /* ----------------------------------------------------------------------- */
  void UpdateProcessMemoryUsage(void)
  { // For getting process info
    task_vm_info_data_t tvidData;
    mach_msg_type_number_t mmtnCount = TASK_VM_INFO_COUNT;
    // Get process memory usage
    if(task_info(mptTask, TASK_VM_INFO,
      reinterpret_cast<task_info_t>(&tvidData), &mmtnCount) == KERN_SUCCESS)
        memData.stMProcUse = static_cast<uint64_t>(tvidData.phys_footprint);
    // Set peak if breached
    if(memData.stMProcUse > memData.stMProcPeak)
      memData.stMProcPeak = memData.stMProcUse;
  }
  /* ----------------------------------------------------------------------- */
  void UpdateSystemMemoryUsage(void)
  { // More containers
		vm_statistics64 vmsData;
    mach_msg_type_number_t mmtnType = HOST_VM_INFO64_COUNT;
    // Get total physical memory and if succeeded?
    if(host_statistics64(mptHost, HOST_VM_INFO64,
         reinterpret_cast<host_info_t>(&vmsData), &mmtnType) != KERN_SUCCESS)
      return;
    // Succeeded getting info, now set the counters
    memData.qMUsed =
      (static_cast<uint64_t>(vmsData.active_count) +
       static_cast<uint64_t>(vmsData.wire_count)) * qwPageSize;
    memData.qMFree = memData.qMTotal - memData.qMUsed;
    // Calculate usage percentage
    memData.dMLoad = UtilMakePercentage(memData.qMUsed, memData.qMTotal);
  }
  /* --------------------------------------------------------------- */ public:
  void UpdateMemoryUsageData(void)
    { UpdateSystemMemoryUsage(); UpdateProcessMemoryUsage(); }
  /* ----------------------------------------------------------------------- */
  void UpdateCPUUsageData(void)
    { UpdateSystemCpuUsage(); UpdateProcessCpuUsage(); }
  /* -- Get uptime from clock class ---------------------------------------- */
  StdTimeT GetUptime(void) const { return cmHiRes.GetTimeS(); }
  /* -- Send signal -------------------------------------------------------- */
  static int SendSignal(const unsigned int uiPid, const int iSignal)
    { return kill(static_cast<pid_t>(uiPid), iSignal); }
  /* -- Terminate a process ------------------------------------------------ */
  bool TerminatePid(const unsigned int uiPid) const
    { return !SendSignal(uiPid, SIGTERM); }
  /* -- Check if specified process id is running --------------------------- */
  bool IsPidRunning(const unsigned int uiPid) const
    { return !SendSignal(uiPid, 0); }
  /* -- GLFW handles the icons on this ------------------------------------- */
  void UpdateIcons(void) { }
  /* ----------------------------------------------------------------------- */
  static bool LibFree(void*const vpModule)
    { return vpModule && !dlclose(vpModule); }
  /* ----------------------------------------------------------------------- */
  template<typename T>
    static T LibGetAddr(void*const vpModule, const char *cpName)
      { return vpModule ? reinterpret_cast<T>(dlsym(vpModule, cpName)) :
          nullptr; }
  /* ----------------------------------------------------------------------- */
  static void *LibLoad(const char*const cpName)
    { return dlopen(cpName, RTLD_LAZY | RTLD_LOCAL); }
  /* ----------------------------------------------------------------------- */
  const string LibGetName(void*const vpModule, const char *cpAltName) const
  { // Return nothing if no module
    if(!vpModule) return {};
    // Get information about the shared object
    Dl_info dlData;
    if(!dladdr(vpModule, &dlData))
      XCL("Failed to read info about shared object!", "File", cpAltName);
    // Get full pathname of file
    return StdMove(PathSplit{ dlData.dli_fname, true }.strFull);
  }
  /* -- Seek to position in specified handle ------------------------------- */
  template<typename IntType>
    static IntType SeekFile(int iFp, const IntType itP)
      { return static_cast<IntType>
          (lseek(iFp, static_cast<off_t>(itP), SEEK_SET)); }
  /* -- Read command and segment data as big-endian ------------------------ */
  template<typename HdrType, class Swap32Type, class Swap64Type>
    static size_t GetExeSize(const char*const cpType, FStream &fExe)
  { // Rewind executable to start of file
    fExe.FStreamRewind();
    // Load specified header
    HdrType mhData;
    if(const size_t stRead = fExe.FStreamReadSafe(&mhData, sizeof(mhData)))
    { // We read enough bytes?
      if(stRead == sizeof(mhData))
      { // Make sure byte order is correct
        mhData.ncmds = Swap32Type(mhData.ncmds).v;
        mhData.sizeofcmds = Swap32Type(mhData.sizeofcmds).v;
        // Highest executable position
        size_t stExeSize = 0;
        // Walk through each command
        for(unsigned int uiIndex = 0;
                         uiIndex < mhData.ncmds && fExe.FStreamIsNotEOF();
                       ++uiIndex)
        { // Save file position
          const int64_t qwCmdPos = fExe.FStreamTell();
          // Create memory to store segment data
          load_command lcData;
          if(const size_t stReadCmd =
            fExe.FStreamReadSafe(&lcData, sizeof(lcData)))
          { // We read enough bytes?
            if(stReadCmd == sizeof(lcData))
            { // Format command data
              lcData.cmd = Swap32Type(lcData.cmd).v;
              lcData.cmdsize = Swap32Type(lcData.cmdsize).v;
              // We are only interested in segments
              switch(lcData.cmd)
              { // 64-bit segment?
                case LC_SEGMENT_64:
                { // Move back to original offset
                  fExe.FStreamSeekSet(qwCmdPos);
                  // Read segment data
                  segment_command_64 scItem;
                  if(const size_t stReadSeg = fExe.FStreamReadSafe(&scItem,
                    sizeof(scItem)))
                  { // We read enough bytes?
                    if(stReadSeg == sizeof(scItem))
                    { // Format segment data
                      scItem.fileoff = Swap64Type(scItem.fileoff).v;
                      scItem.filesize = Swap64Type(scItem.filesize).v;
                      // Get highest point in exe
                      const size_t stEnd = scItem.fileoff + scItem.filesize;
                      if(stEnd > stExeSize) stExeSize = stEnd;
                    } // Failed to read enough bytes for segment
                    else XC("Failed to read enough bytes for MACH-O 64 "
                            "segment!",
                            "Requested", sizeof(scItem),
                            "Actual",    stReadCmd,
                            "Type",      cpType,
                            "File",      fExe.IdentGet());
                  } // Failed to read segment bytes
                  else XCL("Failed to read MACH-O 64 segment!",
                           "Requested", sizeof(scItem), "Type", cpType,
                           "File",      fExe.IdentGet());
                  // Done
                  break;
                } // 32-bit segment?
                case LC_SEGMENT:
                { // Move back to original offset
                  fExe.FStreamSeekSet(qwCmdPos);
                  // Read segment data
                  segment_command scItem;
                  if(const size_t stReadSeg = fExe.FStreamReadSafe(&scItem,
                    sizeof(scItem)))
                  { // We read enough bytes?
                    if(stReadSeg == sizeof(scItem))
                    { // Format needed segment data
                      scItem.fileoff = Swap32Type(scItem.fileoff).v;
                      scItem.filesize = Swap32Type(scItem.filesize).v;
                      // Get highest point in exe
                      const size_t stEnd = scItem.fileoff + scItem.filesize;
                      if(stEnd > stExeSize) stExeSize = stEnd;
                    } // Failed to read enough bytes for segment
                    else XC("Failed to read enough bytes for MACH-O 32 "
                            "segment!",
                            "Requested", sizeof(scItem),
                            "Actual",    stReadCmd,
                            "Type",      cpType,
                            "File",      fExe.IdentGet());
                  } // Failed to read segment bytes
                  else XCL("Failed to read MACH-O 32 segment!",
                           "Requested", sizeof(scItem), "Type", cpType,
                           "File",      fExe.IdentGet());
                  // Done
                  break;
                } // We're not supporting this command
                default: break;
              } // Move forward to next command position
              fExe.FStreamSeekSet(qwCmdPos + lcData.cmdsize);
            } // Failed to read enough bytes
            else XC("Failed to read enough bytes for MACH-O command!",
                    "Requested", sizeof(lcData), "Actual", stReadCmd,
                    "Type",      cpType,         "File",   fExe.IdentGet());
          } // Failed to read command bytes
          else XCL("Failed to read MACH-O command!",
                 "Requested", sizeof(lcData), "Type", cpType,
                 "File",      fExe.IdentGet());
        } // Return executable size
        return stExeSize;
      } // Failed to read enough bytes
      else XC("Failed to read enough bytes for MACH-O header!",
              "Requested", sizeof(mhData), "Actual", stRead,
              "Type",      cpType,         "File",   fExe.IdentGet());
    } // Failed to read elf ident
    else XCL("Failed to read MACH-O header!",
             "Requested", sizeof(mhData), "Type", cpType,
             "File",      fExe.IdentGet());
  }
  /* -- Read command and segment data as big-endian ------------------------ */
  template<typename ArchHdr, class SwapType>
    static size_t GetFatExeSize(const char*const cpType, FStream &fExe)
  { // Rewind executable to start of file
    fExe.FStreamRewind();
    // Load specified header
    fat_header fhData;
    if(const size_t stReadFat = fExe.FStreamReadSafe(&fhData, sizeof(fhData)))
    { // We read enough bytes?
      if(stReadFat == sizeof(fhData))
      { // Highest executable position
        size_t stExeSize = 0;
        // Walk through all the executables
        for(size_t stMax =
          static_cast<size_t>(SwapType(fhData.nfat_arch).v),
                   stIndex = 0;
                   stIndex < stMax; ++stIndex)
        { // Read archive header
          ArchHdr faData;
          if(const size_t stReadArch =
            fExe.FStreamReadSafe(&faData, sizeof(faData)))
          { // We read enough bytes?
            if(stReadArch == sizeof(faData))
            { // Format needed segment data
              faData.offset = SwapType(faData.offset).v;
              faData.size = SwapType(faData.size).v;
              // Get highest point in exe
              const size_t stEnd = faData.offset + faData.size;
              if(stEnd > stExeSize) stExeSize = stEnd;
            } // Failed to read enough bytes
            else XC("Failed to read enough bytes for FAT archive header!",
                    "Requested", sizeof(faData), "Actual", stReadArch,
                    "Type",      cpType,         "File",   fExe.IdentGet());
          } // Failed to read elf ident
          else XCL("Failed to read FAT archive header!",
                   "Requested", sizeof(faData), "Type", cpType,
                   "File",      fExe.IdentGet());
        } // Return executable size
        return stExeSize;
      } // Failed to read enough bytes
      else XC("Failed to read enough bytes for FAT header!",
              "Requested", sizeof(fhData), "Actual", stReadFat,
              "Type",      cpType,         "File",   fExe.IdentGet());
    } // Failed to read elf ident
    else XCL("Failed to read FAT header!",
             "Requested", sizeof(fhData), "Type", cpType,
             "File",      fExe.IdentGet());
  }
  /* -- Get executable size from header (N/A on MacOS) --------------------- */
  static size_t GetExeSize(const string &strFile)
  { // Open exe file and return on error
    if(FStream fExe{ strFile, FM_R_B })
    { // Possible MachO header magic values
      enum MachOMagic : uint32_t {
#if defined(LITTLEENDIAN)         // Intel and ARM?
        MACHO_LE32     = MH_MAGIC,     MACHO_LE64     = MH_MAGIC_64,
        MACHO_BE32     = MH_CIGAM,     MACHO_BE64     = MH_CIGAM_64,
        MACHO_FAT_LE32 = FAT_MAGIC,    MACHO_FAT_LE64 = FAT_MAGIC_64,
        MACHO_FAT_BE32 = FAT_CIGAM,    MACHO_FAT_BE64 = FAT_CIGAM_64,
#elif defined(BIGENDIAN)          // PPC?
        MACHO_LE32     = MH_CIGAM,     MACHO_LE64     = MH_CIGAM_64,
        MACHO_BE32     = MH_MAGIC,     MACHO_BE64     = MH_MAGIC_64,
        MACHO_FAT_LE32 = FAT_CIGAM,    MACHO_FAT_LE64 = FAT_CIGAM_64,
        MACHO_FAT_BE32 = FAT_MAGIC,    MACHO_FAT_BE64 = FAT_MAGIC_64,
#endif
      } uiMagic;
      // Load magic and compare it to possible recognised values...
      switch(const size_t stMagicBytes =
        fExe.FStreamReadSafe(&uiMagic, sizeof(uiMagic)))
      { // Read enough bytes? Compare magic value
        case sizeof(uiMagic): switch(uiMagic)
        { // Little-endian 32-bit format executable
          case MACHO_LE32: return GetExeSize<mach_header,
            UtilSwap32LEFunctor, UtilSwap64LEFunctor>("32LE", fExe);
          // Little-endian 64-bit format executable
          case MACHO_LE64: return GetExeSize<mach_header_64,
            UtilSwap32LEFunctor, UtilSwap64LEFunctor>("64LE", fExe);
          // Big-endian 32-bit format executable
          case MACHO_BE32: return GetExeSize<mach_header,
            UtilSwap32BEFunctor, UtilSwap64BEFunctor>("32BE", fExe);
          // Big-endian 64-bit format executable
          case MACHO_BE64: return GetExeSize<mach_header_64,
            UtilSwap32BEFunctor, UtilSwap64BEFunctor>("64BE", fExe);
          // Little-endian 32-bit format executable
          case MACHO_FAT_LE32: return GetFatExeSize<fat_arch,
            UtilSwap32LEFunctor>("FAT32LE", fExe);
          // Little-endian 64-bit format executable
          case MACHO_FAT_LE64: return GetFatExeSize<fat_arch_64,
            UtilSwap64LEFunctor>("FAT64LE", fExe);
          // Big-endian 32-bit format executable
          case MACHO_FAT_BE32: return GetFatExeSize<fat_arch,
            UtilSwap32BEFunctor>("FAT32BE", fExe);
          // Big-endian 64-bit format executable
          case MACHO_FAT_BE64: return GetFatExeSize<fat_arch_64,
            UtilSwap64BEFunctor>("FAT64BE", fExe);
          // Invalid magic
          default: XC("MACH-O magic is invalid!",
                      "Magic", uiMagic, "File", strFile);
        } // Read nothing? Throw error with reason
        case 0: XCL("Failed to read MACH-O magic!",
                    "Requested", sizeof(uiMagic), "File", fExe.IdentGet());
        // Failed to read enough bytes
        default: XC("Failed to read enough bytes for MACH-O executable magic!",
                    "Requested", sizeof(uiMagic), "Actual", stMagicBytes,
                    "File", strFile);
      }
    } // Failed to open mach executable
    XCL("Failed to open MACH-O executable!", "File", strFile);
  }
  /* -- Get executable file name ------------------------------------------- */
  const string GetExeName(void)
  { // Setup executable pathname
    string strExe; strExe.resize(PROC_PIDPATHINFO_MAXSIZE);
    // Get path to executable
    if(proc_pidpath(GetPid(),
      const_cast<char*>(strExe.c_str()), PROC_PIDPATHINFO_MAXSIZE) <= 0)
        XCS("Failed to get path to executable!",
            "Pid", GetPid(), "Buffer", strExe.capacity());
    // Set real size and trim the memory usage
    strExe.resize(strlen(strExe.c_str()));
    strExe.shrink_to_fit();
    // Return executable name
    return strExe;
  }
  /* -- Enum modules ------------------------------------------------------- */
  SysModMap EnumModules(void)
  { // Make verison string
    const string strVersion{ StrAppend(sizeof(void*)*8, "-bit version") };
    // Mod list
    SysModMap smmMap;
    smmMap.emplace(make_pair(0UL, SysModule{ GetExeName(), VER_MAJOR,
      VER_MINOR, VER_BUILD, VER_REV, VER_AUTHOR, VER_NAME, string(strVersion),
      VER_STR }));
    // Now walk through all the dylibs loaded. Skip the first entry which is
    // always the executable. We already added it!
    for(uint32_t ulIndex = 1, ulEnd = _dyld_image_count();
                 ulIndex < ulEnd; ++ulIndex)
    { // Version to use
      unsigned int uiMajor, uiMinor, uiBuild;
      // Get full path name and id to use. Do not 'const' as we will be moving
      // it into the returned structure. I don't believe this will ever be
      // nullptr but we will check just incase.
      const char*const cpFullPath = _dyld_get_image_name(ulIndex);
      if(UtfIsCStringNotValid(cpFullPath)) continue;
      string strFullPath{ cpFullPath };
      // Get filename. Again, this should never be null but just incase
      const char*const cpBaseName = basename(const_cast<char*>(cpFullPath));
      if(UtfIsCStringNotValid(cpBaseName)) continue;
      const string strBaseName{ cpBaseName };
      // Id name to lookup and add to to the returned structure
      string strPathName;
      // Find dot and ignore if not found? It's a frame work so the full name
      // will be the id.
      const size_t stDot = strBaseName.find_last_of('.');
      if(stDot == StdNPos) strPathName = StdMove(strBaseName);
      // Have extension? If it ends in 'dylib' and it starts with 'lib'? Grab
      // first part before dot and after the lib part
      else if(StrToLowCase(strBaseName.substr(stDot+1)) == "dylib" &&
              StrToLowCase(strBaseName.substr(0, 3)) == "lib")
        strPathName = strBaseName.substr(3, stDot-3);
      // Unknown extension
      else continue;
      // Now we can get the version and if we got it?
      unsigned int uiVer = static_cast<unsigned int>
        (NSVersionOfLinkTimeLibrary(strPathName.c_str()));
      // Try another function if failed
      if(uiVer == StdMaxUInt)
        uiVer = static_cast<unsigned int>
          (NSVersionOfRunTimeLibrary(strPathName.c_str()));
      // Worked this time?
      if(uiVer != StdMaxUInt)
      { // Fill in the bkanks
        uiMajor = (uiVer & 0xFFFF0000) >> 16;
        uiMinor = (uiVer & 0x0000FF00) >> 8;
        uiBuild =  uiVer & 0x000000FF;
      } // No version information
      else uiMajor = uiMinor = uiBuild = 0;
      // Add it to mods list
      smmMap.emplace(make_pair(static_cast<size_t>(ulIndex),
        SysModule{ StdMove(strFullPath), uiMajor, uiMinor, uiBuild,
          0, strPathName.c_str(), strPathName.c_str(),
          string(strVersion), string(StrFormat("$.$.$.0",
            uiMajor, uiMinor, uiBuild)) }));
    } // Module list which includes the executable module
    return smmMap;
  }
  /* ----------------------------------------------------------------------- */
  OSData GetOperatingSystemData(void)
  { // Get operating system name
    const Token tVersion{ GetSysCTLInfoString("kern.osproductversion"),
      cCommon->CommonPeriod() };
    unsigned int uiMajor =
        tVersion.empty() ? 0 : StrToNum<unsigned int>(tVersion[0]),
      uiMinor = tVersion.size() < 2 ? 0 : StrToNum<unsigned int>(tVersion[1]),
      uiBuild = tVersion.size() < 3 ? 0 : StrToNum<unsigned int>(tVersion[2]);
    // Set operating system version string
    ostringstream osS; osS << "MacOS ";
    // Version information table
    struct OSListItem
    { // Label to append if verified
      const char*const cpLabel;
      // Major, minor and service pack of OS which applies to this label
      const unsigned int uiHi, uiLo;
    };
    // List of MacOS versions and when they expire
    static const array<const OSListItem,22>osList{ {
      { "Tahoe",       26,  0 }, { "Sequoia",       15,  0 },
      { "Sonoma",      14,  0 }, { "Ventura",       13,  0 },
      { "Monterey",    12,  0 }, { "Big Sur",       11,  0 },
      { "Catalina",    10, 15 }, { "Mojave",        10, 14 },
      { "High Sierra", 10, 13 }, { "Sierra",        10, 12 },
      { "El Capitan",  10, 11 }, { "Yosemite",      10, 10 },
      { "Mavericks",   10,  9 }, { "Mountain Lion", 10,  8 },
      { "Lion",        10,  7 }, { "Snow Leopard",  10,  6 },
      { "Leopard",     10,  5 }, { "Tiger",         10,  4 },
      { "Panther",     10,  3 }, { "Jaguar",        10,  2 },
      { "Puma",        10,  1 }, { "Cheetah",       10,  0 },
    } };
    // Iterate through the versions and try to find a match for the
    // versions above. 'Unknown' is caught if none are found.
    for(const OSListItem &osItem : osList)
    { // Ignore if this version item doesn't match
      if(uiMajor < osItem.uiHi || uiMinor < osItem.uiLo) continue;
      // Set operating system version
      osS << osItem.cpLabel;
      // Skip adding version numbers
      goto SkipNumericalVersionNumber;
    } // Set unknown operating system label
    osS << "Unknown";
    // Label for when we found the a matching version
    SkipNumericalVersionNumber:
    // Get LANGUAGE code and set default if not 5 bytes long?
    string strCode{ cCmdLine->CmdLineGetEnv("LANGUAGE") } ;
    if(strCode.size() != 5)
    { // Get LANG code and set default if not found
      strCode = cCmdLine->CmdLineGetEnv("LANG");
      if(strCode.size() >= 5)
      { // Find a period (e.g. "en_GB.UTF8") and remove suffix it if found
        const size_t stPeriod = strCode.find('.');
        if(stPeriod != StdNPos) strCode.resize(stPeriod);
      } // Clear code
      else strCode.clear();
    } // Clear code
    else strCode.clear();
    // Couldn't find code? (true if running from bundle)
    if(strCode.empty())
    { // Resize buffer for storage
      strCode.resize(256);
      // Create autorelease storage for locale, ask OS for it and if success?
      typedef unique_ptr<const void,
        function<decltype(CFRelease)>> CFAutoRelPtr;
      if(const CFAutoRelPtr cfLocale{ reinterpret_cast<const void*>
        (CFLocaleCopyCurrent()), CFRelease })
      { // Get reference to string
        const CFStringRef csrRef =
          CFLocaleGetIdentifier(reinterpret_cast<const CFLocaleRef>
            (cfLocale.get()));
        // Copy the string into our STL string and if successful?
        if(CFStringGetCString(csrRef, const_cast<char*>(strCode.data()),
            static_cast<CFIndex>(strCode.capacity()), kCFStringEncodingUTF8))
        { // Get length and truncate the string to proper number of bytes
          const size_t stLength =
            static_cast<size_t>(CFStringGetLength(csrRef));
          strCode.resize(stLength);
          // It must be 5 bytes
          if(stLength < 5)
            XC("Region code too short!", "Code", strCode, "Length", stLength);
        } // Failed
        else XC("Could not get region code!");
      } // This should never happen but just incase?
      else XC("Could not detect region code!");
      // Update and set global locale
      cCommon->CommonSetLocale(strCode);
    } // Set global locale and show error if failed
    if(!setlocale(LC_ALL, strCode.c_str()))
      XCL("Failed to initialise default locale!", "Locale", strCode);
    // Replace underscore with dash to be consistent with Windows
    if(strCode[2] == '_') strCode[2] = '-';
    // Get operating system kernel name
    string strExtra;
    struct utsname utsnData;
    if(!uname(&utsnData))
      strExtra = StrAppend(utsnData.sysname, " v", utsnData.release);
    // Return operating system info
    return {
      osS.str(),                       // Version string
      StdMove(strExtra),               // Extra version string
      uiMajor,                         // Major OS version
      uiMinor,                         // Minor OS version
      uiBuild,                         // OS build version
      sizeof(void*)<<3,                // 32 or 64 OS arch
      StdMove(strCode),                // Get locale
      DetectElevation(),               // Elevated?
      false                            // Wine or Old OS?
    };
  }
  /* ----------------------------------------------------------------------- */
  ExeData GetExecutableData(void)
  { // Suffix to test against
    const string strMacSig{ ".app/Contents/MacOS/" };
    // Get engine directory
    const string &strLoc = ENGLoc();
    // Engine location length and Mac app signature length
    const size_t stEngLength = strLoc.length(),
                 stMacSigLength = strMacSig.length();
    // If executable directory matches this
    bool bIsBundled = stEngLength > stMacSigLength &&
      strLoc.substr(stEngLength - stMacSigLength) == strMacSig;
    // Return result
    return { 0, 0, false, bIsBundled };
  }
  /* ----------------------------------------------------------------------- */
  CPUData GetProcessorData(void)
  { // Using arm cpu?
#if defined(RISC)
    // Get family model and stepping (improvised for X-platform consistency)
    const unsigned int
      uiFamily = GetSysCTLInfoNum<unsigned int>("hw.cpusubfamily"),
      uiModel = GetSysCTLInfoNum<uint32_t>("hw.cpusubtype"),
      uiStepping = 0;
    // Get processor name
    string strProcessorName{ GetSysCTLInfoString("machdep.cpu.brand_string") },
      strVendorId{ "Apple" };
    // Remove unnecessary whitespaces
    StrCompactRef(strProcessorName);
    // Processor speeds common speeds (lowest vs highest speed).
    typedef array<const unsigned int, 2> UIntPair;
    const UIntPair uipM1{ { 2064, 3228 } }, // Apple M1
                   uipM2{ { 2420, 3480 } }, // Apple M2
                   uipM3{ { 2748, 4056 } }, // Apple M3
                   uipM4{ { 2896, 4464 } }, // Apple M4
                   uipM5{ { 2896, 4600 } }; // Apple M5 (Est.)
    // Processor table with speeds. This is because there is no API to get
    // the speed of Apple branded processors.
    typedef pair<const string, const UIntPair &> MacCpuListMapPair;
    typedef map<MacCpuListMapPair::first_type, MacCpuListMapPair::second_type>
      MacCpuListMap;
    typedef MacCpuListMap::const_iterator MacCpuListMapConstIt;
    const MacCpuListMap mclmData{
      { "Apple M1",       uipM1 }, { "Apple M1 Pro",   uipM1 },
      { "Apple M1 Max",   uipM1 }, { "Apple M1 Ultra", uipM1 },
      { "Apple M2",       uipM2 }, { "Apple M2 Pro",   uipM2 },
      { "Apple M2 Max",   uipM2 }, { "Apple M2 Ultra", uipM2 },
      { "Apple M3",       uipM3 }, { "Apple M3 Pro",   uipM3 },
      { "Apple M3 Max",   uipM3 }, { "Apple M3 Ultra", uipM3 },
      { "Apple M4",       uipM4 }, { "Apple M4 Pro",   uipM4 },
      { "Apple M4 Max",   uipM4 }, { "Apple M4 Ultra", uipM4 },
      { "Apple M5",       uipM5 }, { "Apple M5 Pro",   uipM5 },
      { "Apple M5 Max",   uipM5 }, { "Apple M5 Ultra", uipM5 }
    };
    // Find processor name to speed table and if we found it? Then copy the
    // value from the table as the actual speed.
    const MacCpuListMapConstIt mclmciIt{ mclmData.find(strProcessorName) };
    const unsigned int uiSpeed = mclmciIt == mclmData.cend() ?
      static_cast<unsigned int>
        (GetSysCTLInfoNum<uint64_t>("hw.tbfrequency")/10000) :
      mclmciIt->second[1];
    // Using INTEL processor?
#elif defined(CISC)
    // Get family model and stepping
    const unsigned int
      uiSpeed = GetSysCTLInfoNum<uint64_t>("hw.cpufrequency")/1000000,
      uiFamily = GetSysCTLInfoNum<uint32_t>("machdep.cpu.family"),
      uiModel = GetSysCTLInfoNum<uint32_t>("machdep.cpu.model"),
      uiStepping = GetSysCTLInfoNum<uint32_t>("machdep.cpu.stepping");
    // Get processor id and vendor
    string strProcessorName{ GetSysCTLInfoString("machdep.cpu.brand_string") },
      strVendorId{ GetSysCTLInfoString("machdep.cpu.vendor") };
    // Remove unnecessary whitespaces
    StrCompactRef(strVendorId);
    StrCompactRef(strProcessorName);
    // Fail-safe empty strings
    if(strVendorId.empty()) strVendorId = cCommon->CommonUnspec();
#endif
    // Check processor name is specified
    if(strProcessorName.empty()) strProcessorName = strVendorId;
    // Return default data we could not read
    return { StdThreadMax(), uiSpeed, uiFamily, uiModel, uiStepping,
      StdMove(strProcessorName) };
  }
  /* ----------------------------------------------------------------------- */
  bool DebuggerRunning(void) const
  { // Kernel process info
    struct kinfo_proc kipInfo;
    kipInfo.kp_proc.p_flag = 0;
    // Setup request parameters
    array<int,4> iaParams{ CTL_KERN, KERN_PROC, KERN_PROC_PID, GetPid() };
    // Check see if we're running a debugger
    size_t stSize = sizeof(kipInfo);
    sysctl(iaParams.data(), iaParams.size(), &kipInfo, &stSize, nullptr, 0);
    // Return status
    return (kipInfo.kp_proc.p_flag & P_TRACED) != 0;
  }
  /* -- Get process affinity masks ----------------------------------------- */
  uint64_t GetAffinity(const bool) const { return Flag(StdThreadMax()); }
  /* ----------------------------------------------------------------------- */
  int GetPriority(void) const
  { // Get priority value and throw if failed
    const int iNice = getpriority(PRIO_PROCESS, GetPid<unsigned int>());
    if(iNice == -1)
      XCS("Failed to acquire process priority!", "Pid", GetPid());
    // Return priority
    return iNice;
   }
  /* -- Return if running as root ----------------------------------------- */
  bool DetectElevation(void) { return getuid() == 0; }
  /* -- Return data from /dev/random -------------------------------------- */
  Memory GetEntropy(void) const
    { return FStream{ "/dev/random", FM_R_B }.FStreamReadBlockSafe(1024); }
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
  /* -- Build user roaming directory ---------------------------- */ protected:
  const string BuildRoamingDir(void) const
    { return cCmdLine->CmdLineMakeEnvPath("HOME",
        "/Library/Application Support"); }
  /* -- Default constructor ------------------------------------------------ */
  SysCore(void) :                      // No parameters
    /* -- Initialisers ----------------------------------------------------- */
    SysCon{ EnumModules(), 0 },        // Build system module dependencies
    SysCommon{ GetExecutableData(),    // Build data about the executable
               GetOperatingSystemData(), // Build data about the OS
               GetProcessorData() },   // Build data about the CPU
    bWindowInitialised(false)          // Window not initialised yet
    /* -- Initialise total memory size ------------------------------------- */
    { InitMemorySize(); }
};/* ----------------------------------------------------------------------- */
/* == EoF =========================================================== EoF == */
