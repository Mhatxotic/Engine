/* == REFCTR.HPP =========================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## Provides a reference counter for protecting objects from being      ## **
** ## destroyed abruptly.                                                 ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IRefCtr {                    // Start of private module namespace
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* ------------------------------------------------------------------------- */
template<typename IntType=unsigned int>
requires is_integral_v<IntType>
class RefCtrMaster
{ /* -- Private variables -------------------------------------------------- */
  IntType          itMaster;           // Reference to protected variable
  /* --------------------------------------------------------------- */ public:
  void RefCtrSetDisabled() { --itMaster; }
  void RefCtrSetEnabled() { ++itMaster; }
  bool RefCtrIsEnabled() const { return itMaster > 0; }
  bool RefCtrIsDisabled() const { return !itMaster; }
  /* -- Constructor --------------------------------------------- */ protected:
  RefCtrMaster() : itMaster(0) {}
  /* ----------------------------------------------------------------------- */
};                                     // End of class
/* ------------------------------------------------------------------------- */
template<typename IntType=unsigned int,
         class RmcType=RefCtrMaster<IntType>>
requires is_integral_v<IntType>
class RefCtrSlave
{ /* -- Private variables -------------------------------------------------- */
  RmcType         &rtMaster;           // Reference to protected variable
  /* -- Constructor that takes a master reference ------------------ */ public:
  explicit RefCtrSlave(RmcType &rtRef) : rtMaster(rtRef)
    { rtMaster.RefCtrSetEnabled(); }
  /* -- Constructor that takes a master pointer (converts to reference) ---- */
  explicit RefCtrSlave(RmcType*const rtPtr) : RefCtrSlave(*rtPtr) {}
  /* -- Destructor --------------------------------------------------------- */
  ~RefCtrSlave() { rtMaster.RefCtrSetDisabled(); }
  /* ----------------------------------------------------------------------- */
};                                     // End of class
/* ------------------------------------------------------------------------- */
};                                     // End of private module namespace
/* ------------------------------------------------------------------------- */
};                                     // End of public module namespace
/* == EoF =========================================================== EoF == */
