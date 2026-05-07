/* == TOKEN.HPP ============================================================ **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## This class will take a string and split it into tokens seperated by ## **
** ## the specified delimiter.                                            ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IToken {                     // Start of module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace ICommon::P;            using namespace IStd::P;
/* -- Picks the appropriate type for a string argument parameter ----------- */
template<class StrConType, typename StrArgType>
  using StdPickStrTypeForContainer = StdConditional<
    StdIsSame<StdDecay<StrConType>, StdString> &&
    StdIsSame<StdDecay<StrArgType>, StdString>,
    StdString,
    StdStringView>;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* -- Simple constructor with no restriction on token count ---------------- */
template<class StrConType, class StrType, class StrSepType>
  static void Tokeniser(StrType &&strStr, StrSepType &&sstSep, auto &&fFunc)
{ // Force parameters to StringView if not of a string type
  StdPickStrTypeForContainer<StrConType, StrType>
    snsStr{ StdForward<StrType>(strStr) };
  StdNormalisedString<StrSepType> snsSep{ StdForward<StrSepType>(sstSep) };
  // Return if string or separator is empty
  if(snsStr.empty() || snsSep.empty()) return;
  // Get length of separator
  const size_t stSepLen = snsSep.size();
  size_t stStart = 0;
  // Extract each word and emplace it into our vector of strings
  for(size_t stLoc; (stLoc = snsStr.find(snsSep, stStart)) != StdNPos;
        stStart = stLoc + stSepLen)
    fFunc(snsStr.substr(stStart, stLoc - stStart));
  // Theres one left? Make sure it's inserted
  if(stStart <= snsStr.size())
    fFunc(snsStr.substr(stStart, snsStr.size() - stStart));
};
/* ------------------------------------------------------------------------- */
template<class VecStrType,
         class StrVecType = StdVector<VecStrType>>
  requires StdIsString<VecStrType>
struct Token :                         // Tokeniser class
  /* -- Base classes ------------------------------------------------------- */
  public StrVecType                    // Container type for strings
{ /* -- Constructor with maximum token count ------------------------------- */
  template<class StrType, class StrSepType>
    Token(StrType &&strStr, StrSepType &&sstSep, const size_t stMax)
  { // Force parameters to StringView if not of a string type
    StdPickStrTypeForContainer<VecStrType, StrType>
      snsStr{ StdForward<StrType>(strStr) };
    StdNormalisedString<StrSepType>
      snsSep{ StdForward<StrSepType>(sstSep) };
    // Return if string or separator is empty
    if(snsStr.empty() || snsSep.empty()) return;
    // What is the maximum number of tokens allowed?
    switch(stMax)
    { // None? Return nothing
      case 0: break;
      // One? Return original string.
      case 1: this->emplace_back(VecStrType{ snsStr }); break;
      // Something else?
      default:
      { // Reserve memory for specified number of items if available
        constexpr const bool bHasReserve = StdHasReserve<StrVecType>;
        if constexpr(bHasReserve) this->reserve(stMax);
        // Get length of separator and maximum items minus one
        const size_t stSepLen = snsSep.size(), stMaxM1 = stMax - 1;
        // Position in string
        size_t stStart = 0;
        // Enumerate the string through each separator and extract each string
        for(size_t stLoc;
            (stLoc = snsStr.find(snsSep, stStart)) != StdNPos &&
              this->size() < stMaxM1;
            stStart = stLoc + stSepLen)
          this->emplace_back(VecStrType{
            snsStr.substr(stStart, stLoc - stStart) });
        // More text left? Make sure to insert that
        if(stStart < snsStr.size())
          this->emplace_back(VecStrType{ snsStr.substr(stStart) });
        // Compact memory if available and break to finish
        if constexpr(bHasReserve) this->shrink_to_fit();
        break;
      }
    }
  }
  /* -- Simple constructor with no restriction on token count -------------- */
  Token(auto &&aStr, auto &&aSep)
  { // Run the generic tokeniser function to split apart the stirng
    Tokeniser<VecStrType>(StdForward<decltype(aStr)>(aStr),
      StdForward<decltype(aSep)>(aSep), [this](const VecStrType &vstStr)
        { this->emplace_back(vstStr); });
  }
  /* -- Direct conditional access ------------------------------------------ */
  operator bool() const { return !this->empty(); }
  /* -- MOVE constructor --------------------------------------------------- */
  Token(Token &&tlOther) :
    /* -- Initialisers ----------------------------------------------------- */
    StrVector{ StdMove(tlOther) }      // Move vector of strings over
    /* -- No code ---------------------------------------------------------- */
    {}
};/* ----------------------------------------------------------------------- */
using TokenList           = Token<StdStringView, StdList<StdStringView>>;
using TokenStrView        = Token<StdStringView>;
using TokenStrViewConstIt = TokenStrView::const_iterator;
using TokenStr            = Token<StdString>;
using TokenStrConstIt     = TokenStr::const_iterator;
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of module namespace
/* == EoF =========================================================== EoF == */
