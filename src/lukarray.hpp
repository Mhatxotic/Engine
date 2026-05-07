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
/* ------------------------------------------=============================== */
template<
  size_t stMaximum,                    // Maximum number of items
  size_t stMinimum=0,                  // Minimum allowed value
  class List =                         // List array type alias
    StdArray<const StdStringView,      // Use const type string
      stMaximum>>                      // Maximum number of strings in array
struct LookupArray :                   // Members initially public
  /* -- Dependents --------------------------------------------------------- */
  private NameConst,                   // Alternative if id is unknown
  private List                         // Array of strings
{ /* -- Constructor with alternative string -------------------------------- */
  public: LookupArray(const List &lNI, const StdStringView &strNU) :
    /* -- Initialisers ----------------------------------------------------- */
    NameConst{ StdMove(strNU) },       // Unknown item string
    List{ StdMove(lNI) }               // Items
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- Constructor with blank alternative string -------------------------- */
  explicit LookupArray(const List &lNI) :
    /* -- Initialisers ----------------------------------------------------- */
    LookupArray{ lNI, cCommon->CommonBlank() }
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- Get name from id --------------------------------------------------- */
  const StdStringView &Get(const auto aId) const
  { // Allow any input integer type, we don't need to convert if the same
    const size_t stId = static_cast<size_t>(aId);
    return stId >= stMinimum && stId < stMaximum ?
      (*this)[static_cast<size_t>(aId)] : NameGet();
  }
};/* ----------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
