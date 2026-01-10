/* == MUTEX.HPP ============================================================ **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## Defined a mutex helper class.                                       ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IMutex {                     // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IStd::P;               using ::std::invoke;
using ::std::lock_guard;               using ::std::mutex;
using ::std::scoped_lock;              using ::std::unique_lock;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* -- Dependencies and types ----------------------------------------------- */
using ::std::condition_variable;       // Condition variable alias
using ::std::try_to_lock;              // Try to lock and continue if failed
typedef lock_guard<mutex> LockGuard;   // Shortcut to a mutex lock guard
typedef unique_lock<mutex> UniqueLock; // Shortcut to a mutex unique lock
/* -- Dummy mutex lock ----------------------------------------------------- */
struct mutexnoop
{ /* -- No-op lock/unlock/try_lock ----------------------------------------- */
  static void lock() {}
  static void unlock() {}
  static bool try_lock() { return true; }
  /* -- No-op ctor --------------------------------------------------------- */
  mutexnoop(void) = default;
};/* ----------------------------------------------------------------------- */
/* -- Unique lock unlock/relock class -------------------------------------- */
class UniqueRelock
{ /* -- Private variables -------------------------------------------------- */
  UniqueLock      &ulLock;             // Reference to existing unique lock
  /* -- Constructor ------------------------------------------------ */ public:
  explicit UniqueRelock(UniqueLock &ulL) : ulLock(ulL) { ulLock.unlock(); }
  /* -- Destructor --------------------------------------------------------- */
  ~UniqueRelock() { ulLock.lock(); }
};/* ----------------------------------------------------------------------- */
/* -- Mutex wrapper class -------------------------------------------------- */
template<class MutexType>class Mutex   // Members initially private
{ /* -- Private variables -------------------------------------------------- */
  MutexType        mtMutex;            // The mutex object
  /* -- Return mutex ----------------------------------------------- */ public:
  MutexType &MutexGet() { return mtMutex; }
  /* -- Safe call wrapper -------------------------------------------------- */
  template<typename Func>decltype(auto) MutexCall(Func &&fCb)
  { // Lock access to the mutex and call the requested lambda
    const lock_guard<MutexType> lgLock{ MutexGet() };
    return invoke(StdForward<Func>(fCb));
  }
  /* -- Safe unique lock call wrapper -------------------------------------- */
  template<typename Func, typename...Args>
    decltype(auto) MutexUniqueCall(Func &&fCb, Args&&...aArgs)
  { // Lock access to the mutex with the specified type and call the lambda
    unique_lock<MutexType> ulLock{ MutexGet(), StdForward<Args>(aArgs)... };
    return invoke(StdForward<Func>(fCb), ulLock);
  }
  /* -- Safe scoped call wrapper ------------------------------------------- */
  template<typename Func, typename...Args>
    decltype(auto) MutexScopedCall(Func &&fCb, Args&&...aArgs)
  { // Lock access to the mutex and call the requested function
    const scoped_lock slLock{ MutexGet(), StdForward<Args>(aArgs)... };
    return invoke(StdForward<Func>(fCb));
  }
  /* -- Default constructor to initialise random uuid ---------------------- */
  Mutex() = default;
};/* -- Selects NoMutex on MacOS (for StdForEach) or Mutex (for reset) ----- */
using MutexLock = Mutex<mutex>;        // Alias for locking mutex
using MutexNoop = Mutex<mutexnoop>;    // Alias for no-op mutex
#if defined(MACOS)                     // Using MacOS?
using MutexAuto = MutexNoop;           // Use no-op mutex
#else                                  // Anything but MacOS?
using MutexAuto = MutexLock;           // Use locking mutex
#endif                                 // OS check
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
