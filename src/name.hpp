/* == NAME.HPP ============================================================= **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## Simple class to hold a string name name.                            ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IName {                      // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IStd::P;               using namespace IString::P;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* -- Read only name class ------------------------------------------------- */
template<class StrType>
  class Name
{ /* -- Protected variables ------------------------------------------------ */
  StrType          sName;              // The name
  /* -- Name is set? ----------------------------------------------- */ public:
  bool NameIsNotSet() const { return NameGet().empty(); }
  bool NameIsSet() const { return !NameIsNotSet(); }
  /* -- Set name ----------------------------------------------------------- */
  void NameSet(auto &&aStr) { sName = StdMove(aStr); }
  /* -- Formatted set using StrFormat() ------------------------------------ */
  template<typename StrFormatType, typename ...VarArgs>
    void NameSetEx(StrFormatType &&strFormat, VarArgs &&...vaArgs)
  { NameSet(StrFormat(StdForward<StrFormatType>(strFormat),
                      StdForward<VarArgs>(vaArgs)...)); }
  /* -- Formatted set using StrAppend() ------------------------------------ */
  template<typename ...VarArgs>
    void NameSetA(VarArgs &&...vaArgs)
  { NameSet(StrAppend(StdForward<VarArgs>(vaArgs)...)); }
  /* -- Swap name ---------------------------------------------------------- */
  void NameSwap(Name &nOther) { sName.swap(nOther.sName); }
  /* -- Clear name --------------------------------------------------------- */
  void NameClear() { sName.clear(); }
  /* -- Get name ----------------------------------------------------------- */
  const StrType &NameGet() const { return sName; }
  /* -- Get name by address ------------------------------------------------ */
  const char *NameGetData() const { return NameGet().data(); }
  /* -- Move constructor from rvalue string -------------------------------- */
  explicit Name(StrType &&stNName) : sName{ StdMove(stNName) } {}
  /* -- Move constructor from rvalue name ---------------------------------- */
  explicit Name(Name &&nOther) : sName{ StdMove(nOther.NameGet()) } {}
  /* -- Copy constructor from another lvalue string ------------------------ */
  template<class AnyType>
    requires StdIsString<AnyType>
  explicit Name(const AnyType &atName) : sName{ atName } {}
  /* -- Copy constructor from string data and size ------------------------- */
  explicit Name(const char*const cpStr, const size_t stSize) :
    sName{ cpStr, stSize } {}
  /* -- Copy constructor from a literal c-string --------------------------- */
  template<size_t stN>
    requires (stN > 0)
  explicit Name(const char (&caName)[stN]) : Name{ caName, stN - 1 } {}
  /* -- Standby constructor ------------------------------------------------ */
  Name() = default;
};/* ----------------------------------------------------------------------- */
using NameStr   = Name<StdString>;           // Normal string storage
using NameConst = Name<const StdStringView>; // Const type of Name
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
