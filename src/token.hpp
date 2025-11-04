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
/* ------------------------------------------------------------------------- */
using namespace IStd::P;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* -- Simple constructor with no restriction on token count ---------------- */
template<class StrType, class StrSepType>
  static void Tokeniser(const StrType &strStr, const StrSepType &strSep,
    auto &&fFunc)
{ // Return if string or separator is empty
  if(strStr.empty() || strSep.empty()) return;
  // Get length of separator
  const size_t stSepLen = strSep.length();
  size_t stStart = 0;
  // Extract each word and emplace it into our vector of strings
  for(size_t stLoc; (stLoc = strStr.find(strSep, stStart)) != StdNPos;
        stStart = stLoc + stSepLen)
    fFunc({ strStr.data() + stStart, stLoc - stStart });
  // Theres one left? Make sure it's inserted
  if(stStart <= strStr.length())
    fFunc({ strStr.data() + stStart, strStr.size() - stStart });
};
/* -- Token class with permission to modify the original string ------------ */
struct TokenListNC :
  /* -- Base classes ------------------------------------------------------- */
  public CStrVector                    // Vector of C-Strings
{ /* -- Constructor with maximum token count ------------------------------- */
  TokenListNC(string &strStr, const string &strSep, const size_t stMax)
  { // Ignore if either string is empty
    if(strStr.empty() || strSep.empty()) return;
    // What is the maximum number of tokens allowed?
    switch(stMax)
    { // None? Return nothing
      case 0: return;
      // One? Return original string.
      case 1: emplace_back(strStr.data()); return;
      // Something else?
      default:
      { // Reserve memory for all the specified items that are expected
        reserve(stMax);
        // Get length of separator and maximum items minus one
        const size_t stSepLen = strSep.length(), stMaxM1 = stMax - 1;
        // Location of separator
        size_t stStart = 0;
        // Until there are no more occurences of separator or maximum reached
        // Move the tokenised part into a new list item
        for(size_t stLoc;
            (stLoc = strStr.find(strSep, stStart)) != StdNPos &&
              size() < stMaxM1;
            stStart += stLoc - stStart + stSepLen)
        { // Zero the character
          strStr[stLoc] = '\0';
          // Add it to the list
          emplace_back(&strStr[stStart]);
        } // Push remainder of string if there is a remainder
        if(stStart < strStr.length()) emplace_back(&strStr[stStart]);
        // Compact memory
        shrink_to_fit();
        // Done
        break;
      }
    }
  }
  /* -- Direct conditional access ------------------------------------------ */
  operator bool() const { return !empty(); }
  /* -- MOVE assignment constructor ---------------------------------------- */
  TokenListNC(TokenListNC &&tlOther) :
    /* -- Initialisers ----------------------------------------------------- */
    CStrVector{ StdMove(tlOther) }     // Move vector of C-Strings over
    /* -- No code ---------------------------------------------------------- */
    {}
};/* ----------------------------------------------------------------------- */
struct TokenList :                     // Token class with line limit
  /* -- Base classes ------------------------------------------------------- */
  public StrList                       // List of strings
{ /* -- Simple constructor with no restriction on line count --------------- */
  TokenList(const string &strStr, const string &strSep, const size_t stMax)
  { // Ignore if either string is empty
    if(strStr.empty() || strSep.empty()) return;
    // What is the maximum number of tokens allowed?
    switch(stMax)
    { // None? Return nothing
      case 0: return;
      // One? Return original string.
      case 1: emplace_back(strStr); return;
      // Something else?
      default:
      { // Get length of separator
        const size_t stSepLen = strSep.length();
        // Location of separator
        size_t stStart = strStr.size() - 1;
        // Until there are no more occurences of separator
        for(size_t stLoc;
             stStart != StdNPos &&
               (stLoc = strStr.find_last_of(strSep, stStart)) != StdNPos;
            stStart = stLoc - 1)
        { // Get location plus length
          const size_t stLocPlusLength = stLoc + stSepLen;
          // Move the tokenised part into a new list item
          emplace_front(strStr.substr(stLocPlusLength,
            stStart - stLocPlusLength + 1));
          // If we're over the limit
          if(size() >= stMax) return;
        } // Push remainder of string if there is a remainder
        if(stStart != StdNPos)
          emplace_front(strStr.substr(0, stStart + 1));
        // Done
        return;
      }
    }
  }
  /* -- Direct conditional access ------------------------------------------ */
  operator bool() const { return !empty(); }
  /* -- MOVE assignment constructor ---------------------------------------- */
  TokenList(TokenList &&tlOther) :
    /* -- Initialisers ----------------------------------------------------- */
    StrList{ StdMove(tlOther) }            // Move list of strings over
    /* -- No code ---------------------------------------------------------- */
    {}
}; /* ---------------------------------------------------------------------- */
struct Token :                         // Tokeniser class
  /* -- Base classes ------------------------------------------------------- */
  public StrVector                     // Vector of strings
{ /* -- Constructor with maximum token count ------------------------------- */
  Token(const string &strStr, const string &strSep, const size_t stMax)
  { // Return if string or separator is empty
    if(strStr.empty() || strSep.empty()) return;
    // What is the maximum number of tokens allowed?
    switch(stMax)
    { // None? Return nothing
      case 0: break;
      // One? Return original string.
      case 1: emplace_back(strStr); break;
      // Something else?
      default:
      { // Reserve memory for specified number of items
        reserve(stMax);
        // Get length of separator and maximum items minus one
        const size_t stSepLen = strSep.length(), stMaxM1 = stMax - 1;
        // Position in string
        size_t stStart = 0;
        // Enumerate the string through each separator and extract each string
        for(size_t stLoc;
            (stLoc = strStr.find(strSep, stStart)) != StdNPos &&
              size() < stMaxM1;
            stStart = stLoc + stSepLen)
          emplace_back(strStr.substr(stStart, stLoc - stStart));
        // More text left? Make sure to insert that
        if(stStart < strStr.length()) emplace_back(strStr.substr(stStart));
        // Compact memory
        shrink_to_fit();
        // Done
        break;
      }
    }
  }
  /* -- Simple constructor with no restriction on token count -------------- */
  Token(const string &strStr, const string &strSep)
  { // Run the generic tokeniser function to split apart the stirng
    Tokeniser(strStr, strSep, [this](string &&svStr)
      { emplace_back(StdMove(svStr)); });
  }
  /* -- Direct conditional access ------------------------------------------ */
  operator bool() const { return !empty(); }
  /* -- MOVE constructor --------------------------------------------------- */
  Token(Token &&tlOther) :
    /* -- Initialisers ----------------------------------------------------- */
    StrVector{ StdMove(tlOther) }      // Move vector of strings over
    /* -- No code ---------------------------------------------------------- */
    {}
};/* ----------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of module namespace
/* == EoF =========================================================== EoF == */
