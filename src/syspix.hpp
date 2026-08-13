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
using namespace IError::P;             using namespace IFStream::P;
using namespace IMemory::P;            using namespace IStd::P;
using namespace Lib::OS;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* ------------------------------------------------------------------------- */
class SysCorePosix                     // Paired with SysProcess
{ /* -- Private variables -------------------------------------------------- */
  FStream          fsDevRandom;        // Handle to dev/random (rng)
  bool             bWindowInitialised; // Is window initialised?
  const size_t     stPageSize;         // Memory page size
  const pid_t      piProcessId;        // Process id
  const pthread_t  vpThreadId;         // Thread id (handle in this case)
  /* -- Return page size ---------------------------------------- */ protected:
  size_t GetPageSize() const { return stPageSize; }
  /* -- Send signal -------------------------------------------------------- */
  static int SendSignal(const unsigned uPid, const int iSignal)
    { return kill(static_cast<pid_t>(uPid), iSignal); }
  /* -- Terminate a process ---------------------------------------- */ public:
  static bool TerminatePid(const unsigned uPid)
    { return !SendSignal(uPid, SIGTERM); }
  /* -- Check if specified process id is running --------------------------- */
  static bool IsPidRunning(const unsigned uPid)
    { return !SendSignal(uPid, 0); }
  /* -- Return process id -------------------------------------------------- */
  template<typename IntType = decltype(piProcessId)>
    requires StdIsIntegral<IntType>
  IntType GetPid() const { return static_cast<IntType>(piProcessId); }
  /* -- Return thread id --------------------------------------------------- */
  template<typename IntType = decltype(vpThreadId)>
    requires StdIsIntegral<IntType>
  IntType GetTid() const
    { return static_cast<IntType>(StdBruteCast<const size_t>(vpThreadId)); }
  /* -- Return data from /dev/urandom -------------------------------------- */
  Memory GetEntropy()
    { return fsDevRandom.FStreamReadBlockSafe(GetPageSize()); }
  /* -- GLFW handles the icons on this ------------------------------------- */
  static void UpdateIcons() {}
  /* -- Free a loaded shared library --------------------------------------- */
  static bool LibFree(void*const vpModule)
    { return vpModule && !dlclose(vpModule); }
  /* -- Get function address inside a loaded shared library ---------------- */
  template<typename FuncType>
    requires StdIsPointer<FuncType>
  static FuncType LibGetAddr(void*const vpModule, const char *cpName)
    { return vpModule != nullptr ?
        reinterpret_cast<FuncType>(dlsym(vpModule, cpName)) : nullptr; }
  /* -- Load an external shared library ------------------------------------ */
  static void *LibLoad(const char*const cpName)
    { return dlopen(cpName, RTLD_LAZY | RTLD_LOCAL); }
  /* -- Return if running as root ------------------------------------------ */
  static bool DetectElevation() { return getuid() == 0; }
  /* -- Return last socket or system error --------------------------------- */
  static int LastSocketOrSysError() { return StdGetError(); }
  /* -- Return window handle (n/a) ----------------------------------------- */
  static void *GetWindowHandle() { return nullptr; }
  /* -- A window was created ----------------------------------------------- */
  void WindowInitialised(const GlFW::GLFWwindow*const gwWindow)
    { bWindowInitialised = gwWindow != nullptr; }
  /* -- Window was destroyed, nullify handles ------------------------------ */
  void SetWindowDestroyed() { bWindowInitialised = false; }
  /* -- Default constructor ------------------------------------------------ */
  SysCorePosix() :
    /* -- Initialisers ----------------------------------------------------- */
    fsDevRandom{ "/dev/random",        // Open dev random garbage stream
                 FM_R_B },             // - Read/Binary mode
    bWindowInitialised(false),         // Window initialised later
    stPageSize(static_cast<size_t>(    // Get memory  page size
      sysconf(_SC_PAGESIZE))),         // It's 4KB normally (16KB on Apple)
    piProcessId(getpid()),             // Get native process id
    vpThreadId(pthread_self())         // Get native thread id
    /* -- Check that /dev/random opened ------------------------------------ */
    { if(fsDevRandom.FStreamClosed())
        XCL("Failed to open random device stream!",
          "Stream", fsDevRandom.NameGet()); }
};/* ----------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
