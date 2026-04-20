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
/* ------------------------------------------------------------------------- */
using namespace IError::P;             using namespace IStd::P;
using namespace IString::P;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* -- Parser class --------------------------------------------------------- */
template<class ParserMapType>class ParserBase :
  /* -- Base classes ------------------------------------------------------- */
  public ParserMapType                 // Derive by specified map type
{ /* -- Private variables -------------------------------------------------- */
  typedef ParserMapType::const_iterator ParserMapTypeConstIt;
  typedef ParserMapType::value_type     ParserMapTypePair;
  /* -- Initialise entries from a string ----------------------------------- */
  void ParserDoInit(const StdString &strSep, const StdString &strLineSep,
    const char cDelimiter)
  { // Ignore if any of the variables are empty
    if(strSep.empty() || strLineSep.empty()) return;
    // Location of next separator
    size_t stStart = 0;
    // Until eof, push each item split into list
    for(size_t stLoc;
              (stLoc = strSep.find(strLineSep, stStart)) != StdNPos;
               stStart = stLoc + strLineSep.length())
      ParserPushLine(strSep, stStart, stLoc, cDelimiter);
    // Push remainder of string if available
    ParserPushLine(strSep, stStart, strSep.length(), cDelimiter);
  }
  /* --------------------------------------------------------------- */ public:
  void ParserPushPair(const StdString &strKey, const StdString &strValue)
    { this->insert({ strKey, strValue }); }
  void ParserPushPair(const StdString &strKey, StdString &&strValue)
    { this->insert({ strKey, StdMove(strValue) }); }
  void ParserPushPair(const StdString &strKey, const StdStringView &strValue)
    { this->insert({ strKey, { strValue.data(), strValue.size() } }); }
  void ParserPushPair(StdString &&strKey, const StdString &strValue)
    { this->insert({ StdMove(strKey), strValue }); }
  void ParserPushPair(StdString &&strKey, StdString &&strValue)
    { this->insert({ StdMove(strKey), StdMove(strValue) }); }
  /* -- Insert new key if we don't have it --------------------------------- */
  void ParserPushIfNotExist(const StdString &strKey, const StdString &strValue)
    { if(this->find(strKey) == this->end())
        ParserPushPair(strKey, strValue); }
  void ParserPushIfNotExist(const StdString &strKey,
    const StdStringView &strvValue)
  { if(this->find(strKey) == this->end())
      ParserPushPair(strKey, strvValue); }
  void ParserPushIfNotExist(StdString &&strKey, const StdString &strValue)
    { if(this->find(strKey) == this->end())
        ParserPushPair(StdMove(strKey), strValue); }
  void ParserPushIfNotExist(const StdString &strKey, StdString &&strValue)
    { if(this->find(strKey) == this->end())
        ParserPushPair(strKey, StdMove(strValue)); }
  void ParserPushIfNotExist(StdString &&strKey, StdString &&strValue)
    { if(this->find(strKey) == this->end())
        ParserPushPair(StdMove(strKey), StdMove(strValue)); }
  /* -- Direct conditional access ---------------------------------------- */
  operator bool() const { return !this->empty(); }
  /* -- Value access by key name ------------------------------------------- */
  const StdString &ParserGet(const StdString &strKey) const
  { // Find key and return empty string or value
    const ParserMapTypeConstIt pmtciIt{ this->find(strKey) };
    if(pmtciIt != this->end()) return pmtciIt->second;
    XC("No such key in table!", "Key", strKey, "Count", this->size());
  }
  /* -- Value access by key name ------------------------------------------- */
  const StdString ParserGetAndRemove(const StdString &strKey)
  { // Find key and throw error if not found
    const ParserMapTypeConstIt pmtciIt{ this->find(strKey) };
    if(pmtciIt == this->end())
      XC("No such key in table!", "Key", strKey, "Count", this->size());
    // Move the string into a temp var, erase it and return the string
    const StdString strOut{ StdMove(pmtciIt->second) };
    this->erase(pmtciIt->second);
    return strOut;
  }
  /* -- Converts the variables to a string --------------------------------- */
  const StdString ParserImplodeEx(const StdString &strSep,
    const StdString &strSuf) const
  { // String to return
    ostringstream osS;
    // For each key/value pair, implode it into a string
    for(const ParserMapTypePair &pmtpPair : *this)
      osS << pmtpPair.first << strSep << pmtpPair.second << strSuf;
    // Return what we created
    return osS.str();
  }
  /* ----------------------------------------------------------------------- */
  void ParserPushLine(const StdString &strSep, const size_t stSegStart,
    const size_t stSegEnd, const char cDelimiter)
  { // Look for separator and if found?
    const size_t stSepLoc =
      StrFindCharForwards(strSep, stSegStart, stSegEnd, cDelimiter);
    if(stSepLoc != StdNPos)
    { // Find start of keyname and if found?
      const size_t stKeyStart =
        StrFindCharNotForwards(strSep, stSegStart, stSepLoc);
      if(stKeyStart != StdNPos)
      { // Find end of keyname and if found?
        const size_t stKeyEnd =
          StrFindCharNotBackwards(strSep, stSepLoc-1, stSegStart);
        if(stKeyEnd != StdNPos)
        { // Find start of value name and if found?
          const size_t stValStart =
            StrFindCharNotForwards(strSep, stSepLoc+1, stSegEnd);
          if(stValStart != StdNPos)
          { // Find end of value name and if found? We can grab key/value
            const size_t stValEnd =
              StrFindCharNotBackwards(strSep, stSegEnd-1, stValStart);
            if(stValEnd != StdNPos)
              return ParserPushPair(
                StdMove(strSep.substr(stKeyStart, stKeyEnd-stKeyStart+1)),
                StdMove(strSep.substr(stValStart, stValEnd-stValStart+1)));
          } // Could not prune suffixed whitespaces on value.
        }  // Could not prune prefixed whitespaces on value.
      }  // Could not prune suffixed whitespaces on key.
    } // Could not prune prefixed whitespaces on key. Add full value for debug
    return ParserPushPair(StrAppend('\255', this->size()),
      StdMove(strSep.substr(stSegStart, stSegEnd-stSegStart)));
  }
  /* -- Initialise or add entries from a string ---------------------------- */
  void ParserReInit(const StdString &strSep, const StdString &strLineSep,
    const char cDelimiter)
      { this->clear(); ParserDoInit(strSep, strLineSep, cDelimiter); }
  /* -- Initialise or add entries from a string ---------------------------- */
  ParserBase(const StdString &strSep, const StdString &strLineSep,
    const char cDelimiter)
      { ParserDoInit(strSep, strLineSep, cDelimiter); }
  /* -- Move constructor --------------------------------------------------- */
  ParserBase(ParserBase &&pbOther) :   // Other Parser class to move from
    /* -- Initialisers ----------------------------------------------------- */
    ParserMapType{ StdMove(pbOther) }  // Initialise moving vars
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- Constructor -------------------------------------------------------- */
  ParserBase() = default;
}; /* -- A Parser class where the values can be modified ------------------- */
template<class ParserBaseType = ParserBase<StrNCStrMap>>struct Parser :
  /* -- Base classes ------------------------------------------------------- */
  public ParserBaseType                // Base non-const type
{ /* ----------------------------------------------------------------------- */
  void ParserPushOrUpdatePair(const StdString &strKey,
    const StdString &strValue)
  { // Find key and if it exists, just update the value else insert a new one
    const StrNCStrMapIt sncsmiIt{ this->find(strKey) };
    if(sncsmiIt != this->end()) sncsmiIt->second = strValue;
    else this->ParserPushPair(strKey, strValue);
  }
  /* -- Try to move key but copy value ------------------------------------- */
  void ParserPushOrUpdatePair(StdString &&strKey, const StdString &strValue)
  { const StrNCStrMapIt sncsmiIt{ this->find(strKey) };
    if(sncsmiIt != this->end()) sncsmiIt->second = strValue;
    else this->ParserPushPair(StdMove(strKey), strValue);
  }
  /* -- Try to move value but copy key ------------------------------------- */
  void ParserPushOrUpdatePair(const StdString &strKey, StdString &&strValue)
  { const StrNCStrMapIt sncsmiIt{ this->find(strKey) };
    if(sncsmiIt != this->end()) sncsmiIt->second = StdMove(strValue);
    else this->ParserPushPair(strKey, StdMove(strValue));
  }
  /* -- Try to move key and value ------------------------------------------ */
  void ParserPushOrUpdatePair(StdString &&strKey, StdString &&strValue)
  { const StrNCStrMapIt sncsmiIt{ this->find(strKey) };
    if(sncsmiIt != this->end()) sncsmiIt->second = StdMove(strValue);
    else this->ParserPushPair(StdMove(strKey), StdMove(strValue));
  }
  /* -- Add each value that was sent --------------------------------------- */
  void ParserPushOrUpdatePairs(const StrPairList &splValues)
    { for(const StrPair &spKeyValue : splValues)
        this->ParserPushOrUpdatePair(StdMove(spKeyValue.first),
          StdMove(spKeyValue.second)); }
  /* -- Extracts and deletes the specified key pair ------------------------ */
  const StdString Extract(const StdString &strKey)
  { // Find key and return empty string if not found
    const StrNCStrMapIt sncsmiIt{ this->find(strKey) };
    if(sncsmiIt == this->end()) return {};
    // Take ownership of the string (faster than copy)
    const StdString strOut{ StdMove(sncsmiIt->second) };
    // Erase keypair
    this->erase(sncsmiIt);
    // Return the value
    return strOut;
  } /* -- Constructor ------------------------------------------------------ */
  Parser() = default;
  /* -- MOVE assignment constructor ---------------------------------------- */
  Parser(Parser &&pOther) :            // Other Parser class to move from
    /* -- Initialisers ----------------------------------------------------- */
    ParserBaseType{ StdMove(pOther) }  // Initialise moving vars
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- Constructor -------------------------------------------------------- */
  Parser(const StdString &strSep,      // String to explode
         const StdString &strLineSep,  // ...Record (line) separator
         const char cDelimiter) :      // ...Key/value separator
    /* -- Initialisers ----------------------------------------------------- */
    ParserBaseType{ strSep,            // Initialise string to explode
                    strLineSep,        // Initialise record (line) separator
                    cDelimiter }       // Initialise key/value separator
    /* -- No code ---------------------------------------------------------- */
    {}
};/* -- A Parser class thats values cannot be modified at all -------------- */
template<class ParserBaseType = ParserBase<StrStrMap>>struct ParserConst :
  /* -- Base classes ------------------------------------------------------- */
  public ParserBaseType                // The base map type
{ /* -- Constructor -------------------------------------------------------- */
  ParserConst() = default;
  /* -- MOVE assignment constructor ---------------------------------------- */
  ParserConst(ParserConst &&pcOther) : // Other vars
    /* -- Initialisers ----------------------------------------------------- */
    ParserBaseType{ StdMove(pcOther) } // Move it over
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- Constructor -------------------------------------------------------- */
  ParserConst(const StdString &strSep,     // String to explode
              const StdString &strLineSep, // ...Record (line) separator
              const char cDelimiter) :     // ...Key/value separator
    /* -- Initialisers ----------------------------------------------------- */
    ParserBaseType{ strSep,            // Initialise string to explode
                    strLineSep,        // Initialise record (line) separator
                    cDelimiter }       // Initialise key/value separator
    /* -- No code ---------------------------------------------------------- */
    {}
};/* ----------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
