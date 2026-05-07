/* == SYSPIX.HPP =========================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## Contains MacOS and Linux (POSIX) common shared functions.           ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace ISysPosix {                  // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IFStream::P;           using namespace IMemory::P;
using namespace IStd::P;               using namespace Lib::OS;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* ------------------------------------------------------------------------- */
class SysProcessPosix                  // Paired with SysProcess
{ /* -- Protected variables ------------------------------------- */ protected:
  FStream          fsDevRandom;        // Handle to dev/random (rng)
  /* -- Process and thread ------------------------------------------------- */
  const pid_t      piProcessId;        // Process id
  const pthread_t  vpThreadId;         // Thread id
  /* -- Memory ------------------------------------------------------------- */
  const size_t     stPageSize;         // Memory page size
  /* -- Default constructor ------------------------------------------------ */
  SysProcessPosix() :
    /* -- Initialisers ----------------------------------------------------- */
    fsDevRandom{ "/dev/random",        // Open dev random garbage stream
                 FM_R_B },             // - Read/Binary mode
    piProcessId(getpid()),             // Get native process id
    vpThreadId(pthread_self()),        // Get native thread id
    stPageSize(static_cast<size_t>(    // Get memory  page size
      sysconf(_SC_PAGESIZE)))          // It's 4KB normally (16KB on Apple)
    /* --------------------------------------------------------------------- */
    {}
};/* ----------------------------------------------------------------------- */
class SysCorePosix :                   // Paired with SysProcess
  /* -- Base classes ------------------------------------------------------- */
  public SysProcessPosix               // Common POSIX functions
{ /* -- Variables ---------------------------------------------------------- */
  bool             bWindowInitialised; // Is window initialised?
  /* -- Send signal --------------------------------------------- */ protected:
  static int SendSignal(const unsigned uPid, const int iSignal)
    { return kill(static_cast<pid_t>(uPid), iSignal); }
  /* -- Terminate a process ---------------------------------------- */ public:
  static bool TerminatePid(const unsigned uPid)
    { return !SendSignal(uPid, SIGTERM); }
  /* -- Check if specified process id is running --------------------------- */
  static bool IsPidRunning(const unsigned uPid)
    { return !SendSignal(uPid, 0); }
  /* -- Return process and thread id --------------------------------------- */
  template<typename IntType = decltype(piProcessId)>
    requires StdIsIntegral<IntType>
  IntType GetPid() const { return static_cast<IntType>(piProcessId); }
  template<typename IntType = decltype(vpThreadId)>
    requires StdIsIntegral<IntType>
  IntType GetTid() const
    { return static_cast<IntType>(StdBruteCast<const size_t>(vpThreadId)); }
  /* -- Return data from /dev/urandom -------------------------------------- */
  Memory GetEntropy()
    { return fsDevRandom.FStreamReadBlockSafe(stPageSize); }
  /* -- GLFW handles the icons on this ------------------------------------- */
  static void UpdateIcons() {}
  /* ----------------------------------------------------------------------- */
  static bool LibFree(void*const vpModule)
    { return vpModule && !dlclose(vpModule); }
  /* ----------------------------------------------------------------------- */
  template<typename FuncType>
    requires StdIsPointer<FuncType>
  static FuncType LibGetAddr(void*const vpModule, const char *cpName)
    { return vpModule != nullptr ?
        reinterpret_cast<FuncType>(dlsym(vpModule, cpName)) : nullptr; }
  /* ----------------------------------------------------------------------- */
  static void *LibLoad(const char*const cpName)
    { return dlopen(cpName, RTLD_LAZY | RTLD_LOCAL); }
  /* -- Return if running as root ------------------------------------------ */
  static bool DetectElevation() { return getuid() == 0; }
  /* ----------------------------------------------------------------------- */
  static int LastSocketOrSysError() { return StdGetError(); }
  /* -- Return window handle (n/a) ----------------------------------------- */
  static void *GetWindowHandle() { return nullptr; }
  /* -- A window was created ----------------------------------------------- */
  void WindowInitialised(const GlFW::GLFWwindow*const gwWindow)
    { bWindowInitialised = !!gwWindow; }
  /* -- Window was destroyed, nullify handles ------------------------------ */
  void SetWindowDestroyed() { bWindowInitialised = false; }
  /* ----------------------------------------------------------------------- */
  SysCorePosix() :
    /* -- Initialisers ----------------------------------------------------- */
    bWindowInitialised(false)
    /* --------------------------------------------------------------------- */
    {}
};/* ----------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
