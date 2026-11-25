/* == LUKMAP.HPP =========================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## Simple class to hold unique id numbers and their corresponding      ## **
** ## string value in a key/value table with a safe lookup.               ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace ILookupMap {                 // Start of private module namespace
/* ------------------------------------------------------------------------- */
using namespace ICommon::P;            using namespace IName::P;
using namespace IStd::P;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* -- Id to string list helper class --------------------------------------- */
template<class KeyType>                // The user specified type of the key
class LookupMap :                      // Members initially private
  /* -- Dependencies ------------------------------------------------------- */
  NameConst,                                 // Alternative if id is unknown
  StdMap<const KeyType, const StdStringView> // Map of key->val pairs
{ /* -- Private typedefs --------------------------------------------------- */
  using PairType     = StdPair<const KeyType, const StdStringView>;
  using ValueType    = PairType::second_type;
  using MapType      = StdMap<typename PairType::first_type, ValueType>;
  using IteratorType = MapType::const_iterator;
  /* -- Macros ------------------------------------------------------------- */
#define IDMAPSTR(e) { e, #e }          // Helper macro
  /* -- Constructor with alternative string ------------------------ */ public:
  explicit LookupMap(
    /* -- Parameters ------------------------------------------------------- */
    MapType &&mtList,                  // Source map
    StdStringView &&ssvIdent =         // Unknown item string...
      cCommon->CommonBlankStr()) :     // ...default blank string
    /* -- Initialisers ----------------------------------------------------- */
    NameConst{ StdForward<decltype(ssvIdent)>(ssvIdent) }, // Alternate id
    MapType{ StdForward<decltype(mtList)>(mtList) }        // Items map
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- Test all items as flags and return a list of strings set ----------- */
  StrViewVector LukMapTest(const KeyType ktValue) const
  { // There will be at least this amount of strings in the list
    StdReserved<StrViewVector> svvOut{ this->size() };
    // Enumerate through all the items and add the string if the bit is set
    for(const PairType &ptItem : *this)
      if(ktValue & ptItem.first) svvOut.push_back(ptItem.second);
    // We didn't add anything? Add the alternative
    if(svvOut.empty()) svvOut.push_back(NameGet());
    // Compact the list to the actual number of items added
    svvOut.shrink_to_fit();
    // Return the list
    return svvOut;
  }
  /* -- Get string --------------------------------------------------------- */
  const ValueType &LukMapGet(const KeyType ktId) const
  { // Find code and return custom error if not found else return string
    const IteratorType ptName{ this->find(ktId) };
    return ptName != this->cend() ? ptName->second : NameGet();
  }
};/* ----------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
