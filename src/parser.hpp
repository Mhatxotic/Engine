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
  void ParserDoInit(const string &strSep, const string &strLineSep,
    const char cDelimiter)
  { // Ignore if any of the variables are empty
    if(strSep.empty() || strLineSep.empty()) return;
    // Location of next separator
    size_t stStart = 0;
    // Until eof, push each item split into list
    for(size_t stLoc;
              (stLoc = strSep.find(strLineSep, stStart)) != string::npos;
               stStart = stLoc + strLineSep.length())
      ParserPushLine(strSep, stStart, stLoc, cDelimiter);
    // Push remainder of string if available
    ParserPushLine(strSep, stStart, strSep.length(), cDelimiter);
  }
  /* --------------------------------------------------------------- */ public:
  void ParserPushPair(const string &strKey, const string &strValue)
    { this->insert({ strKey, strValue }); }
  void ParserPushPair(const string &strKey, string &&strValue)
    { this->insert({ strKey, StdMove(strValue) }); }
  void ParserPushPair(const string &strKey, const string_view &strValue)
    { this->insert({ strKey, { strValue.data(), strValue.size() } }); }
  void ParserPushPair(string &&strKey, const string &strValue)
    { this->insert({ StdMove(strKey), strValue }); }
  void ParserPushPair(string &&strKey, string &&strValue)
    { this->insert({ StdMove(strKey), StdMove(strValue) }); }
  /* -- Insert new key if we don't have it --------------------------------- */
  void ParserPushIfNotExist(const string &strKey, const string &strValue)
    { if(this->find(strKey) == this->end())
        ParserPushPair(strKey, strValue); }
  void ParserPushIfNotExist(const string &strKey, const string_view &strvValue)
    { if(this->find(strKey) == this->end())
        ParserPushPair(strKey, strvValue); }
  void ParserPushIfNotExist(string &&strKey, const string &strValue)
    { if(this->find(strKey) == this->end())
        ParserPushPair(StdMove(strKey), strValue); }
  void ParserPushIfNotExist(const string &strKey, string &&strValue)
    { if(this->find(strKey) == this->end())
        ParserPushPair(strKey, StdMove(strValue)); }
  void ParserPushIfNotExist(string &&strKey, string &&strValue)
    { if(this->find(strKey) == this->end())
        ParserPushPair(StdMove(strKey), StdMove(strValue)); }
  /* -- Direct conditional access ---------------------------------------- */
  operator bool(void) const { return !this->empty(); }
  /* -- Value access by key name ------------------------------------------- */
  const string &ParserGet(const string &strKey) const
  { // Find key and return empty string or value
    const ParserMapTypeConstIt pmtciIt{ this->find(strKey) };
    if(pmtciIt != this->end()) return pmtciIt->second;
    XC("No such key in table!", "Key", strKey, "Count", this->size());
  }
  /* -- Value access by key name ------------------------------------------- */
  const string ParserGetAndRemove(const string &strKey)
  { // Find key and throw error if not found
    const ParserMapTypeConstIt pmtciIt{ this->find(strKey) };
    if(pmtciIt == this->end())
      XC("No such key in table!", "Key", strKey, "Count", this->size());
    // Move the string into a temp var, erase it and return the string
    const string strOut{ StdMove(pmtciIt->second) };
    this->erase(pmtciIt->second);
    return strOut;
  }
  /* -- Converts the variables to a string --------------------------------- */
  const string ParserImplodeEx(const string &strSep,
    const string &strSuf) const
  { // String to return
    ostringstream osS;
    // For each key/value pair, implode it into a string
    for(const ParserMapTypePair &pmtpPair : *this)
      osS << pmtpPair.first << strSep << pmtpPair.second << strSuf;
    // Return what we created
    return osS.str();
  }
  /* ----------------------------------------------------------------------- */
  void ParserPushLine(const string &strSep, const size_t stSegStart,
    const size_t stSegEnd, const char cDelimiter)
  { // Look for separator and if found?
    const size_t stSepLoc =
      StrFindCharForwards(strSep, stSegStart, stSegEnd, cDelimiter);
    if(stSepLoc != string::npos)
    { // Find start of keyname and if found?
      const size_t stKeyStart =
        StrFindCharNotForwards(strSep, stSegStart, stSepLoc);
      if(stKeyStart != string::npos)
      { // Find end of keyname and if found?
        const size_t stKeyEnd =
          StrFindCharNotBackwards(strSep, stSepLoc-1, stSegStart);
        if(stKeyEnd != string::npos)
        { // Find start of value name and if found?
          const size_t stValStart =
            StrFindCharNotForwards(strSep, stSepLoc+1, stSegEnd);
          if(stValStart != string::npos)
          { // Find end of value name and if found? We can grab key/value
            const size_t stValEnd =
              StrFindCharNotBackwards(strSep, stSegEnd-1, stValStart);
            if(stValEnd != string::npos)
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
  void ParserReInit(const string &strSep, const string &strLineSep,
    const char cDelimiter)
      { this->clear(); ParserDoInit(strSep, strLineSep, cDelimiter); }
  /* -- Initialise or add entries from a string ---------------------------- */
  ParserBase(const string &strSep, const string &strLineSep,
    const char cDelimiter)
      { ParserDoInit(strSep, strLineSep, cDelimiter); }
  /* -- Move constructor --------------------------------------------------- */
  ParserBase(ParserBase &&pbOther) :   // Other Parser class to move from
    /* -- Initialisers ----------------------------------------------------- */
    ParserMapType{ StdMove(pbOther) }  // Initialise moving vars
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Constructor -------------------------------------------------------- */
  ParserBase(void) { }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(ParserBase)          // Suppress default functions for safety
}; /* -- A Parser class where the values can be modified ------------------- */
template<class ParserBaseType = ParserBase<StrNCStrMap>>struct Parser :
  /* -- Base classes ------------------------------------------------------- */
  public ParserBaseType                // Base non-const type
{ /* ----------------------------------------------------------------------- */
  void ParserPushOrUpdatePair(const string &strKey, const string &strValue)
  { // Find key and if it exists, just update the value else insert a new one
    const StrNCStrMapIt sncsmiIt{ this->find(strKey) };
    if(sncsmiIt != this->end()) sncsmiIt->second = strValue;
    else this->ParserPushPair(strKey, strValue);
  }
  /* -- Try to move key but copy value ------------------------------------- */
  void ParserPushOrUpdatePair(string &&strKey, const string &strValue)
  { const StrNCStrMapIt sncsmiIt{ this->find(strKey) };
    if(sncsmiIt != this->end()) sncsmiIt->second = strValue;
    else this->ParserPushPair(StdMove(strKey), strValue);
  }
  /* -- Try to move value but copy key ------------------------------------- */
  void ParserPushOrUpdatePair(const string &strKey, string &&strValue)
  { const StrNCStrMapIt sncsmiIt{ this->find(strKey) };
    if(sncsmiIt != this->end()) sncsmiIt->second = StdMove(strValue);
    else this->ParserPushPair(strKey, StdMove(strValue));
  }
  /* -- Try to move key and value ------------------------------------------ */
  void ParserPushOrUpdatePair(string &&strKey, string &&strValue)
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
  const string Extract(const string &strKey)
  { // Find key and return empty string if not found
    const StrNCStrMapIt sncsmiIt{ this->find(strKey) };
    if(sncsmiIt == this->end()) return {};
    // Take ownership of the string (faster than copy)
    const string strOut{ StdMove(sncsmiIt->second) };
    // Erase keypair
    this->erase(sncsmiIt);
    // Return the value
    return strOut;
  } /* -- Constructor ------------------------------------------------------ */
  Parser(void) { }
  /* -- MOVE assignment constructor ---------------------------------------- */
  Parser(Parser &&pOther) :            // Other Parser class to move from
    /* -- Initialisers ----------------------------------------------------- */
    ParserBaseType{ StdMove(pOther) }  // Initialise moving vars
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Constructor -------------------------------------------------------- */
  Parser(const string &strSep,         // String to explode
         const string &strLineSep,     // ...Record (line) separator
         const char cDelimiter) :      // ...Key/value separator
    /* -- Initialisers ----------------------------------------------------- */
    ParserBaseType{ strSep,            // Initialise string to explode
                    strLineSep,        // Initialise record (line) separator
                    cDelimiter }       // Initialise key/value separator
    /* -- No code ---------------------------------------------------------- */
    { }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(Parser)              // Suppress default functions for safety
};/* -- A Parser class thats values cannot be modified at all -------------- */
template<class ParserBaseType = const ParserBase<const StrStrMap>>
  struct ParserConst :
  /* -- Base classes ------------------------------------------------------- */
  public ParserBaseType                // The base map type
{ /* -- Constructor -------------------------------------------------------- */
  ParserConst(void) { }
  /* -- MOVE assignment constructor ---------------------------------------- */
  ParserConst(ParserConst &&pcOther) : // Other vars
    /* -- Initialisers ----------------------------------------------------- */
    ParserBaseType{ StdMove(pcOther) } // Move it over
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Constructor -------------------------------------------------------- */
  ParserConst(const string &strSep,    // String to explode
              const string &strLineSep,// ...Record (line) separator
              const char cDelimiter) : // ...Key/value separator
    /* -- Initialisers ----------------------------------------------------- */
    ParserBaseType{ strSep,            // Initialise string to explode
                    strLineSep,        // Initialise record (line) separator
                    cDelimiter }       // Initialise key/value separator
    /* -- No code ---------------------------------------------------------- */
    { }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(ParserConst)         // Suppress default functions for safety
};/* ----------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
