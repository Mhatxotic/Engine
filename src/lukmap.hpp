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
template<
  class KeyType = unsigned,            // The user specified type of the key
  class ValueType = StdStringView,     // The user specified type of the value
  class PairType =                     // The pair type to hold key/value pairs
    StdPair<const KeyType,             // The pair key type
            const ValueType>,          // The pair value type
  class MapType =                      // The map type to hold key/value pairs
    StdMap<const KeyType,              // The key type
           const ValueType>,           // The value type
  class IteratorType =                 // The iter type to hold key/value pairs
    MapType::const_iterator>           // The iterator type
struct LookupMap :                     // Members initially public
  /* -- Dependencies ------------------------------------------------------- */
  private NameConst,                   // Alternative if id is unknown
  private MapType                      // Map of key->value pairs
{ /* -- Macros ------------------------------------------------------------- */
#define IDMAPSTR(e) { e, #e }          // Helper macro
  /* -- Constructor with alternative string -------------------------------- */
  explicit LookupMap(
    /* -- Parameters ------------------------------------------------------- */
    const MapType &mtList,             // Source map
    const StdStringView &strvIdent =   // Unknown item string...
      cCommon->CommonBlank()) :        // ...default blank string
    /* -- Initialisers ----------------------------------------------------- */
    NameConst{ strvIdent },            // Unknown item string
    MapType{ mtList }                  // Items map
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- Test all items as flags and return a list of strings set ----------- */
  StrViewVector Test(const KeyType ktValue) const
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
  const ValueType &Get(const KeyType ktId) const
  { // Find code and return custom error if not found else return string
    const IteratorType ptName{ this->find(ktId) };
    return ptName != this->cend() ? ptName->second : NameGet();
  }
};/* ----------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
