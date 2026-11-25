/* == LUKARRAY.HPP ========================================================= **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## Simple class to hold unique id numbers and their corresponding      ## **
** ## string value in an array table with a safe lookup system.           ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace ILookupArray {               // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace ICommon::P;            using namespace IName::P;
using namespace IStd::P;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* ------------------------------------------------------------------------- */
template<size_t stMaximum>             // Maximum number of items
class LookupArray :                    // Members initially private
  /* -- Dependents --------------------------------------------------------- */
  NameConst,                               // Alternative if id is unknown
  StdArray<const StdStringView, stMaximum> // Array of strings
{ /* -- Private typedefs --------------------------------------------------- */
  using Array = StdArray<const StdStringView, stMaximum> ;
  /* -- Constructor with alternative string ------------------------ */ public:
  public: explicit LookupArray(Array &&aInit,
    StdStringView &&strAlt = cCommon->CommonBlankStr()) :
    /* -- Initialisers ----------------------------------------------------- */
    NameConst{ StdForward<decltype(strAlt)>(strAlt) }, // Unknown item string
    Array{ StdForward<decltype(aInit)>(aInit) }        // Items
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- Get name from id --------------------------------------------------- */
  const StdStringView &LukArrayGet(const auto aId) const
  { // Allow any input integer type, we don't need to convert if the same
    const size_t stId = static_cast<size_t>(aId);
    return stId < stMaximum ? (*this)[static_cast<size_t>(aId)] : NameGet();
  }
};/* ----------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
