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
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* == Universally unique identifier helper ================================= */
struct Mutex :                         // Members initially public
  /* -- Base classes ------------------------------------------------------- */
  private mutex                        // The mutex object
{ /* -- Return mutex ------------------------------------------------------- */
  mutex &MutexGet() { return *this; }
  /* -- Safe call wrapper -------------------------------------------------- */
  template<typename Func>decltype(auto) MutexCall(const Func &&fCb)
  { // Lock access to the variables
    const LockGuard lgLock{ MutexGet() };
    // Call the requested lambda
    return ::std::invoke(::std::forward<const Func>(fCb));
  }
  /* -- Default constructor to initialise random uuid ---------------------- */
  Mutex() = default;
};/* ----------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
