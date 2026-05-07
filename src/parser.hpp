/* == PARSER.HPP =========================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## This class will take a string and split it into tokens seperated by ## **
** ## the specified delimiter and split again into sorted key/value pairs ## **
** ## for fast access by key name.                                        ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IParser {                    // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IError::P;             using namespace IStd::P;
using namespace IString::P;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* -- Parser class --------------------------------------------------------- */
template<bool bTrim,
         class StrKeyType,
         class StrValType = StrKeyType,
         class ParserMapType = StdMap<StrKeyType, StrValType>>
class Parser :
  /* -- Base classes ------------------------------------------------------- */
  public ParserMapType                 // Derive by specified map type
{ /* -- Private variables -------------------------------------------------- */
  using ParserMapTypeConstIt = ParserMapType::const_iterator;
  using ParserMapTypePair = ParserMapType::value_type;
  /* ----------------------------------------------------------------------- */
  void ParserPushLine(auto &&aStr, const size_t stSegStart,
      const size_t stSegEnd, const char cDelimiter)
  { // Look for separator and if found?
    const size_t stSepLoc =
      StrFindCharForwards(aStr, stSegStart, stSegEnd, cDelimiter);
    if(stSepLoc != StdNPos)
    { // Find start of keyname and if found?
      const size_t stKeyStart =
        StrFindCharNotForwards(aStr, stSegStart, stSepLoc);
      if(stKeyStart != StdNPos)
      { // Find end of keyname and if found?
        const size_t stKeyEnd =
          StrFindCharNotBackwards(aStr, stSepLoc - 1, stSegStart);
        if(stKeyEnd != StdNPos)
        { // Find start of value name and if found?
          const size_t stValStart =
            StrFindCharNotForwards(aStr, stSepLoc + 1, stSegEnd);
          if(stValStart != StdNPos)
          { // Find end of value name and if found? We can grab key/value
            const size_t stValEnd =
              StrFindCharNotBackwards(aStr, stSegEnd - 1, stValStart);
            if(stValEnd != StdNPos)
              return ParserPushPair(
                aStr.substr(stKeyStart, stKeyEnd - stKeyStart + 1),
                aStr.substr(stValStart, stValEnd - stValStart + 1));
          } // Could not prune suffixed whitespaces on value.
        }  // Could not prune prefixed whitespaces on value.
      }  // Could not prune suffixed whitespaces on key.
    } // Could not prune prefixed whitespaces on key. Add full value for debug
    return ParserPushPair(StrAppend('\255', this->size()),
      aStr.substr(stSegStart, stSegEnd - stSegStart));
  }
  /* -- Initialise entries from a string ----------------------------------- */
  template<class StrSrcType, class StrLineSepType>
    void ParserDoInit(StrSrcType &&sstSrc, StrLineSepType &&slstLSep,
      const char cDelimiter)
  { // Force parameters to StringView if not of a string type
    using StrNormalisedSep = StdNormalisedString<StrSrcType>;
    StrNormalisedSep snsSrc{ StdForward<StrSrcType>(sstSrc) };
    using StrNormalisedLineSep = StdNormalisedString<StrLineSepType>;
    StrNormalisedLineSep snsLSep{ StdForward<StrLineSepType>(slstLSep) };
    // We have to reject if source text is string but key or value type is
    // string view as TEMPORARY variables created by substr()'s in
    // ParserPushLine will be downgraded to a string_view and then the memory
    // for that temporary is deallocated.
    static_assert(!StdIsSame<StrNormalisedSep,StdString> ||
      StdIsSame<StdDecay<StrKeyType>,StdStringView> ||
      StdIsSame<StdDecay<StrValType>,StdStringView>);
    // Ignore if any of the variables are empty
    if(snsSrc.empty() || snsLSep.empty()) return;
    // Location of next separator
    size_t stStart = 0;
    // Until eof, push each item split into list
    for(size_t stLoc;
              (stLoc = snsSrc.find(snsLSep, stStart)) != StdNPos;
               stStart = stLoc + snsLSep.size())
      ParserPushLine(StdForward<StrNormalisedSep>(snsSrc), stStart, stLoc,
        cDelimiter);
    // Push remainder of string if available
    ParserPushLine(StdForward<StrNormalisedSep>(snsSrc), stStart,
      snsSrc.size(), cDelimiter);
  }
  /* --------------------------------------------------------------- */ public:
  void ParserPushPair(auto &&aKey, auto &&aValue)
  { // If chop key names requested?
    if constexpr(bTrim)
    { // Remove any tab or space characters
      StdRemoveConst<StrKeyType> sktKey{ aKey };
      StrTrimSuffixRef(sktKey, '\t');
      StrTrimSuffixRef(sktKey, ' ');
      this->insert({ sktKey, StrValType{ aValue } });
    } // Add as-is (format known)
    else this->insert({ StrKeyType{ aKey }, StrValType{ aValue } });
  }
  /* -- Insert new key if we don't have it --------------------------------- */
  void ParserPushIfNotExist(auto &&aKey, auto &&aValue)
    { if(this->find(aKey) == this->end())
        ParserPushPair(StdForward<decltype(aKey)>(aKey),
                       StdForward<decltype(aValue)>(aValue)); }
  /* -- Value access by key name ------------------------------------------- */
  StrValType ParserGet(auto &&aKey) const
  { // Find key and return empty string or valuezz
    const ParserMapTypeConstIt pmtciIt{ this->find(aKey) };
    if(pmtciIt != this->end()) return pmtciIt->second;
    XC("No such key in table!", "Key", aKey, "Count", this->size());
  }
  /* -- Value access by key name ------------------------------------------- */
  StrValType ParserGetAndRemove(auto &&aKey)
  { // Find key and throw error if not found
    const ParserMapTypeConstIt pmtciIt{ this->find(aKey) };
    if(pmtciIt == this->end())
      XC("No such key in table!", "Key", aKey, "Count", this->size());
    // Move the string into a temp var, erase it and return the string
    const StrValType strOut{ StdMove(pmtciIt->second) };
    this->erase(pmtciIt->second);
    return strOut;
  }
  /* -- Converts the variables to a string --------------------------------- */
  StdString ParserImplodeEx(auto &&aSep, auto &&aSuf) const
  { // String to return
    StdOStringStream osS;
    // For each key/value pair, implode it into a string
    for(const ParserMapTypePair &pmtpPair : *this)
      osS << pmtpPair.first << aSep << pmtpPair.second << aSuf;
    // Return what we created
    return osS.str();
  }
  /* -- Initialise or add entries from a string ---------------------------- */
  void ParserReInit(auto &&aSep, auto &aLineSep, const char cDelimiter)
    { this->clear();
      ParserDoInit(StdForward<decltype(aSep)>(aSep),
        StdForward<decltype(aLineSep)>(aLineSep), cDelimiter); }
  /* ----------------------------------------------------------------------- */
  void ParserPushOrUpdatePair(auto &&aKey, auto &&aValue)
  { // Find key and if it exists, just update the value else insert a new one
    const StrNCStrMapIt sncsmiIt{ this->find(aKey) };
    if(sncsmiIt != this->end()) sncsmiIt->second = aValue;
    else this->ParserPushPair(aKey, aValue);
  }
  /* -- Add each value that was sent --------------------------------------- */
  void ParserPushOrUpdatePairs(const StrPairList &splValues)
    { for(const StrPair &spKeyValue : splValues)
        this->ParserPushOrUpdatePair(spKeyValue.first, spKeyValue.second); }
  /* -- Extracts and deletes the specified key pair ------------------------ */
  StrValType Extract(auto &&aKey)
  { // Find key and return empty string if not found
    const StrNCStrMapIt sncsmiIt{ this->find(aKey) };
    if(sncsmiIt == this->end()) return {};
    // Take ownership of the string (faster than copy)
    const StrValType svtOut{ StdMove(sncsmiIt->second) };
    this->erase(sncsmiIt);
    return svtOut;
  }
  /* -- Direct conditional access ------------------------------------------ */
  operator bool() const { return !this->empty(); }
  /* -- Initialise or add entries from a string ---------------------------- */
  Parser(auto &&aSep, auto &&aLineSep, const char cDelimiter)
    { ParserDoInit(StdForward<decltype(aSep)>(aSep),
                   StdForward<decltype(aLineSep)>(aLineSep), cDelimiter); }
  /* -- Move constructor --------------------------------------------------- */
  Parser(Parser &&pbOther) :   // Other Parser class to move from
    /* -- Initialisers ----------------------------------------------------- */
    ParserMapType{ StdMove(pbOther) }  // Initialise moving vars
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- Constructor -------------------------------------------------------- */
  Parser() = default;
};/* -- Completely read only key and value strings ------------------------- */
using ParserString        = Parser<false, const StdString>;
using ParserStringIt      = ParserString::iterator;
using ParserStringConstIt = ParserString::const_iterator;
/* -- Read only key but modifiable value strings --------------------------- */
using ParserStringVol        = Parser<false, const StdString, StdString>;
using ParserStringVolIt      = ParserStringVol::iterator;
using ParserStringVolConstIt = ParserStringVol::const_iterator;
/* -- Read only key but modifiable value strings --------------------------- */
using ParserStringVC        = Parser<true, const StdString, StdString>;
using ParserStringVCIt      = ParserStringVol::iterator;
using ParserStringVCConstIt = ParserStringVol::const_iterator;
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
