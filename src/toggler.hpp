/* == TOGGLER.HPP ========================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## Helps to turn a variable off and on                                 ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IToggler {                   // Start of private module namespace
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* ------------------------------------------------------------------------- */
template<typename IntegerType=unsigned int>class TogglerMaster
{ /* -- Private variables -------------------------------------------------- */
  IntegerType      itMaster;           // Reference to protected variable
  /* --------------------------------------------------------------- */ public:
  void TogglerSetDisabled(void) { --itMaster; }
  void TogglerSetEnabled(void) { ++itMaster; }
  bool TogglerIsEnabled(void) const { return itMaster > 0; }
  bool TogglerIsDisabled(void) const { return !itMaster; }
  /* -- Constructor --------------------------------------------- */ protected:
  TogglerMaster(void) : itMaster(0) { }
  /* -- Destructor --------------------------------------------------------- */
  ~TogglerMaster(void) { }
  /* ----------------------------------------------------------------------- */
};                                     // End of class
/* ------------------------------------------------------------------------- */
template<typename IntegerType=unsigned int,
         class TmcType=TogglerMaster<IntegerType>>
class TogglerSlave
{ /* -- Private variables -------------------------------------------------- */
  TmcType         &tmcMaster;          // Reference to protected variable
  /* -- Constructor that takes a master reference ------------------ */ public:
  explicit TogglerSlave(TmcType &tmcRef) : tmcMaster(tmcRef)
    { tmcMaster.TogglerSetEnabled(); }
  /* -- Constructor that takes a master pointer (converts to reference) ---- */
  explicit TogglerSlave(TmcType*const tmcPtr) : TogglerSlave(*tmcPtr) { }
  /* -- Destructor --------------------------------------------------------- */
  ~TogglerSlave(void) { tmcMaster.TogglerSetDisabled(); }
  /* ----------------------------------------------------------------------- */
};                                     // End of class
/* ------------------------------------------------------------------------- */
};                                     // End of private module namespace
/* ------------------------------------------------------------------------- */
};                                     // End of public module namespace
/* == EoF =========================================================== EoF == */
