/* == LOCKABLE.HPP ========================================================= **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## This class stores a simple 'locked' boolean and functions.          ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace ILockable {                  // Start of private module namespace
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* ------------------------------------------------------------------------- */
class Lockable                         // Lua lock class
{ /* -- Private variables -------------------------------------------------- */
  bool             bLocked;            // Class is locked from being dealloced
  /* -- Set locked status -------------------------------------------------- */
  void LockSet(const bool bState) { bLocked = bState; }
  /* -- Return true if lock is (not) set --------------------------- */ public:
  bool LockIsSet() const { return bLocked; }
  bool LockIsNotSet() const { return !LockIsSet(); }
  /* -- Set/clear locked status -------------------------------------------- */
  void LockSet() { LockSet(true); }
  void LockClear() { LockSet(false); }
  /* -- Swap lock status with another class -------------------------------- */
  void LockSwap(Lockable &lOther) { swap(bLocked, lOther.bLocked); }
  /* -- (Default) constructor ----------------------------------- */ protected:
  explicit Lockable(                   // Initialise with lock (def: false)
    /* -- Parameters ------------------------------------------------------- */
    const bool bState=false            // The user requested lock state
    ): /* -- Initialisers -------------------------------------------------- */
    bLocked(bState)                    // Set the initial lock state
    /* -- No code ---------------------------------------------------------- */
    {}
};/* -- End ---------------------------------------------------------------- */
};                                     // End of private module namespace
/* ------------------------------------------------------------------------- */
};                                     // End of public module namespace
/* == EoF =========================================================== EoF == */
