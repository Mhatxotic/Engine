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
class Mutex                            // Members initially public
{ /* -- Base classes ------------------------------------------------------- */
  mutex            mMutex;             // The mutex object
  /* -- Return mutex ----------------------------------------------- */ public:
  mutex &MutexGet() { return mMutex; }
  /* -- Safe call wrapper -------------------------------------------------- */
  template<typename Func>decltype(auto) MutexCall(Func &&fCb)
  { // Lock access to the mutex and call the requested lambda
    const LockGuard lgLock{ MutexGet() };
    return invoke(StdForward<Func>(fCb));
  }
  /* -- Safe unique lock call wrapper -------------------------------------- */
  template<typename Func, typename...Args>
    decltype(auto) MutexUniqueCall(Func &&fCb, Args&&...aArgs)
  { // Lock access to the mutex with the specified type and call the lambda
    UniqueLock ulLock{ MutexGet(), StdForward<Args>(aArgs)... };
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
};/* ----------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
