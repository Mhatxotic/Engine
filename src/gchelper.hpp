/* == GCHELPER.HPP ========================================================= **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## This class sets the address of a global pointer at construction     ## **
** ## and nullifies it at destruction.                                    ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IGCHelper {                  // Start of private module namespace
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* -- Class template ------------------------------------------------------- */
template<class ParentType>             // The parent class of this class
  requires StdIsClass<ParentType>      // Requires any class to be supplied
/* -- Class body ----------------------------------------------------------- */
class GCHelper                         // Members initially private
{ /* -- Private variables -------------------------------------------------- */
  ParentType**const ptAddr;            // Address of pointer to global class
  /* -- Protected typedefs -------------------------------------- */ protected:
  using ClassType = ParentType;        // Expose parent class type
  using SelfType  = GCHelper;          // Expose this class type
  /* -- Destructor that clears the global class pointer -------------------- */
  ~GCHelper() { *ptAddr = nullptr; }
  /* -- Constructor that initialises the global class pointer -------------- */
  explicit GCHelper(ParentType*const ptPtr, ParentType**const ptNAddr) :
    /* -- Initialisers ----------------------------------------------------- */
    ptAddr{ ptNAddr }                  // Store address of class pointer
    /* -- Initialise address of global class pointer ----------------------- */
    { *ptAddr = ptPtr; }
};/* ----------------------------------------------------------------------- */
};                                     // End of private module namespace
/* ------------------------------------------------------------------------- */
};                                     // End of public module namespace
/* == EoF =========================================================== EoF == */
