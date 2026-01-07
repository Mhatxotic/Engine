/* == IDENT.HPP ============================================================ **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## Simple class to hold a string identifier.                           ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IIdent {                     // Start of private module namespace
/* ------------------------------------------------------------------------- */
using namespace ICommon::P;            using namespace IStd::P;
using namespace IString::P;            using namespace IUtf::P;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* -- Read only identifier class ------------------------------------------- */
template<class StringType>struct IdentBase
{ /* -- Identifier is set? ------------------------------------------------- */
  bool IdentIsNotSet() const { return IdentGet().empty(); }
  bool IdentIsSet() const { return !IdentIsNotSet(); }
  /* -- Get identifier ----------------------------------------------------- */
  const StringType &IdentGet() const { return strIdentifier; }
  /* -- Get identifier by address ------------------------------------------ */
  const char *IdentGetData() const { return IdentGet().data(); }
  /* -- Move constructor from another rvalue string ------------- */ protected:
  explicit IdentBase(StringType &&strId) : strIdentifier{ StdMove(strId) } {}
  /* -- Move constructor from rvalue identifier ---------------------------- */
  explicit IdentBase(IdentBase &&idOther) :
    strIdentifier{ StdMove(idOther.IdentGet()) } {}
  /* -- Copy constructor from another lvalue string ------------------------ */
  explicit IdentBase(const StringType &strId) : strIdentifier{ strId } {}
  /* -- Standby constructor ------------------------------------------------ */
  IdentBase() = default;
  /* -- Private variables -------------------------------------------------- */
  StringType       strIdentifier;      // The identifier
};/* -- Identifier class --------------------------------------------------- */
struct Ident :                         // Members initially public
  /* -- Base classes ------------------------------------------------------- */
  public IdentBase<string>             // The read-only class
{ /* -- Set identifier by rvalue ------------------------------------------- */
  void IdentSet(string &&strId) { strIdentifier = StdMove(strId); }
  /* -- Set identifier by lvalue ------------------------------------------- */
  void IdentSet(const char*const cpId) { strIdentifier = cpId; }
  /* -- Set identifier by lvalue ------------------------------------------- */
  void IdentSet(const string &strId) { strIdentifier = strId; }
  /* -- Set identifier by string view -------------------------------------- */
  void IdentSet(const string_view &strvId) { strIdentifier = strvId; }
  /* -- Set identifier by class -------------------------------------------- */
  void IdentSet(const IdentBase &ibO) { strIdentifier = ibO.IdentGet(); }
  /* -- Formatted set using StrFormat() ------------------------------------ */
  template<typename ...VarArgs>
    void IdentSetEx(const char*const cpFormat, VarArgs &&...vaArgs)
  { IdentSet(StrFormat(cpFormat, StdForward<VarArgs>(vaArgs)...)); }
  /* -- Formatted set using StrAppend() ------------------------------------ */
  template<typename ...VarArgs>void IdentSetA(VarArgs &&...vaArgs)
    { IdentSet(StrAppend(StdForward<VarArgs>(vaArgs)...)); }
  /* -- Clear identifier --------------------------------------------------- */
  void IdentClear() { strIdentifier.clear(); }
  /* -- Swap identifier ---------------------------------------------------- */
  void IdentSwap(Ident &idOther) { strIdentifier.swap(idOther.strIdentifier); }
  /* -- Move constructor from another rvalue string ------------------------ */
  explicit Ident(string &&strId) : IdentBase{ StdMove(strId) } {}
  /* -- Move constructor from rvalue identifier ---------------------------- */
  explicit Ident(Ident &&idO) : IdentBase{ StdMove(idO.IdentGet()) } {}
  /* -- Copy constructor from another lvalue string ------------------------ */
  explicit Ident(const string &strId) : IdentBase{ strId } {}
  /* -- Standby constructor ------------------------------------------------ */
  Ident() = default;
};/* ----------------------------------------------------------------------- */
typedef IdentBase<const string_view> IdentConst; // Const type of Ident
/* == Id to string list helper class ======================================= */
template<size_t stMaximum,             // Maximum number of items
         size_t stMinimum=0,           // Minimum allowed value
         class List =                  // List array type alias
           array<const string_view,    // Use const type string
             stMaximum>>               // Maximum number of strings in array
struct IdList :                        // Members initially public
  /* -- Dependents --------------------------------------------------------- */
  private IdentConst,                  // Alternative if id is unknown
  private List                         // Array of strings
{ /* -- Constructor with alternative string -------------------------------- */
  public: IdList(const List &lNI, const string_view &strNU) :
    /* -- Initialisers ----------------------------------------------------- */
    IdentConst{ StdMove(strNU) },      // Unknown item string
    List{ StdMove(lNI) }               // Items
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- Constructor with blank alternative string -------------------------- */
  explicit IdList(const List &lNI) :
    /* -- Initialisers ----------------------------------------------------- */
    IdList{ lNI, cCommon->CommonBlank() }
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- Get name from id --------------------------------------------------- */
  template<typename IntType=size_t>
    const string_view &Get(const IntType itId) const
  { // Allow any input integer type, we don't need to convert if the same
    const size_t stId = static_cast<size_t>(itId);
    return stId >= stMinimum && stId < stMaximum ? (*this)[stId] : IdentGet();
  }
};/* ----------------------------------------------------------------------- */
/* == Id to string list helper class ======================================= */
template<class KeyType = unsigned int, // The user specified type of the key
         class ValueType = string_view,// The user specified type of the value
         class PairType =              // The pair type to hold key/value pairs
           pair<const KeyType,         // The pair key type
                const ValueType>,      // The pair value type
         class MapType =               // The map type to hold key/value pairs
           map<const KeyType,          // The key type
               const ValueType>,       // The value type
         class IteratorType =          // The iter type to hold key/value pairs
           MapType::const_iterator>    // The iterator type
struct IdMap :                         // Members initially public
  /* -- Dependencies ------------------------------------------------------- */
  private IdentConst,                  // Alternative if id is unknown
  private MapType                      // Map of key->value pairs
{ /* -- Macros ------------------------------------------------------------- */
#define IDMAPSTR(e) { e, #e }          // Helper macro
  /* -- Constructor with alternative string -------------------------------- */
  explicit IdMap(
    /* -- Parameters ------------------------------------------------------- */
    const MapType &mtList,             // Source map
    const string_view &strvIdent =     // Unknown item string...
      cCommon->CommonBlank()) :        // ...default blank string
    /* -- Initialisers ----------------------------------------------------- */
    IdentConst{ strvIdent },           // Unknown item string
    MapType{ mtList }                  // Items map
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- Test all items as flags and return a list of strings set ----------- */
  const StrViewVector Test(const KeyType ktValue) const
  { // There will be at least this amount of strings in the list
    Reserved<StrViewVector> svvOut{ this->size() };
    // Enumerate through all the items and add the string if the bit is set
    for(const PairType &ptItem : *this)
      if(ktValue & ptItem.first) svvOut.push_back(ptItem.second);
    // We didn't add anything? Add the alternative
    if(svvOut.empty()) svvOut.push_back(IdentGet());
    // Compact the list to the actual number of items added
    svvOut.shrink_to_fit();
    // Return the list
    return svvOut;
  }
  /* -- Get string --------------------------------------------------------- */
  const ValueType &Get(const KeyType ktId) const
  { // Find code and return custom error if not found else return string
    const IteratorType ptName{ this->find(ktId) };
    return ptName != this->cend() ? ptName->second : IdentGet();
  }
};/* ----------------------------------------------------------------------- */
template<typename IntType = const uint64_t>struct IdentCSlave
{ /* -- Public functions --------------------------------------------------- */
  IntType CtrGet() const { return itCounter; }
  /* -- Protected variables ------------------------------------- */ protected:
  IntType          itCounter;          // The counter
  /* -- Constructor that initialises counter ------------------------------- */
  explicit IdentCSlave(const IntType itId) : itCounter(itId) {}
};/* ----------------------------------------------------------------------- */
template<typename IntType = uint64_t,      // Counter integer type (non-const)
  class SlaveClass = IdentCSlave<IntType>> // Slave class type
struct IdentCMaster : public SlaveClass    // Might as well reuse base class
{ /* -- Reset counter ------------------------------------------------------ */
  void CtrReset(const IntType itValue) { this->itCounter = itValue; }
  /* -- Return value before incrementing counter --------------------------- */
  IntType CtrNext() { return this->itCounter++; }
  /* -------------------------------------------------------------*/ protected:
  IdentCMaster() : SlaveClass(static_cast<IntType>(0)) {}
};/* ----------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
