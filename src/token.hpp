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
      case 1: emplace_back(strStr.c_str()); return;
      // Something else?
      default:
      { // Reserve memory for all the specified items that are expected
        reserve(stMax);
        // Location of separator
        size_t stStart = 0;
        // Until there are no more occurences of separator or maximum reached
        // Move the tokenised part into a new list item
        for(size_t stLoc, stMaxM1 = stMax-1;
          (stLoc = strStr.find(strSep, stStart)) != string::npos &&
            size() < stMaxM1;
          stStart += stLoc - stStart + strSep.length())
        { // Zero the character
          strStr[stLoc] = '\0';
          // Add it to the list
          emplace_back(&strStr[stStart]);
        } // Push remainder of string if there is a remainder
        if(stStart < strStr.length()) emplace_back(&strStr[stStart]);
        // Compact to fit all items
        shrink_to_fit();
        // Done
        break;
      }
    }
  }
  /* -- Direct conditional access ------------------------------------------ */
  operator bool(void) const { return !empty(); }
  /* -- MOVE assignment operator ------------------------------------------- */
  TokenListNC &operator=(TokenListNC &&tlOther)
    { swap(tlOther); return *this; }
  /* -- MOVE assignment constructor ---------------------------------------- */
  TokenListNC(TokenListNC &&tlOther) :
    /* -- Initialisers ----------------------------------------------------- */
    CStrVector{ StdMove(tlOther) }     // Move vector of C-Strings over
    /* -- No code ---------------------------------------------------------- */
    { }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(TokenListNC)         // Suppress default functions for safety
};/* ----------------------------------------------------------------------- */
/* -- Token class with line limit ------------------------------------------ */
struct TokenList :
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
      { // Location of separator
        size_t stStart = strStr.size() - 1;
        // Until there are no more occurences of separator
        for(size_t stLoc;
          stStart != string::npos &&
          (stLoc = strStr.find_last_of(strSep, stStart)) != string::npos;
          stStart = stLoc - 1)
        { // Get location plus length
          const size_t stLocPlusLength = stLoc + strSep.length();
          // Move the tokenised part into a new list item
          emplace_front(strStr.substr(stLocPlusLength,
            stStart - stLocPlusLength + 1));
          // If we're over the limit
          if(size() >= stMax) return;
        } // Push remainder of string if there is a remainder
        if(stStart != string::npos)
          emplace_front(strStr.substr(0, stStart + 1));
        // Done
        return;
      }
    }
  }
  /* -- Direct conditional access ------------------------------------------ */
  operator bool(void) const { return !empty(); }
  /* -- MOVE assignment operator ------------------------------------------- */
  TokenList& operator=(TokenList &&tlOther) { swap(tlOther); return *this; }
  /* -- MOVE assignment constructor ---------------------------------------- */
  TokenList(TokenList &&tlOther) :
    /* -- Initialisers ----------------------------------------------------- */
    StrList{ StdMove(tlOther) }            // Move list of strings over
    /* -- No code ---------------------------------------------------------- */
    { }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(TokenList)           // Suppress default functions for safety
}; /* ---------------------------------------------------------------------- */
/* == Token class ========================================================== */
struct Token :
  /* -- Base classes ------------------------------------------------------- */
  public StrVector                     // Vector of strings
{ /* -- Constructor with maximum token count ------------------------------- */
  Token(const string &strStr, const string &strSep, const size_t stMax)
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
      { // Reserve memory for specified number of items
        reserve(stMax);
        // Location of separator
        size_t stStart = 0;
        // Until there are no more occurences of separator or maximum reached
        // Move the tokenised part into a new list item
        for(size_t stLoc, stMaxM1 = stMax-1;
          (stLoc = strStr.find(strSep, stStart)) != string::npos &&
            size() < stMaxM1;
          stStart += stLoc - stStart + strSep.length())
            emplace_back(strStr.substr(stStart, stLoc - stStart));
        // Push remainder of string if there is a remainder
        if(stStart < strStr.length()) emplace_back(strStr.substr(stStart));
        // Compact to fit all items
        shrink_to_fit();
        // Done
        break;
      }
    }
  }
  /* -- Simple constructor with no restriction on token count -------------- */
  Token(const string &strStr, const string &strSep)
  { // Ignore if either string is empty
    if(strStr.empty() || strSep.empty()) return;
    // Location of separator
    size_t stStart = 0;
    // Until there are no more occurences of separator
    for(size_t stLoc;
      (stLoc = strStr.find(strSep, stStart)) != string::npos;
      stStart += stLoc - stStart + strSep.length())
        emplace_back(strStr.substr(stStart, stLoc - stStart));
    // Push remainder of string if there is a remainder
    if(stStart <= strStr.length()) emplace_back(strStr.substr(stStart));
  }
  /* -- Direct conditional access ------------------------------------------ */
  operator bool(void) const { return !empty(); }
  /* -- MOVE assignment operator ------------------------------------------- */
  Token& operator=(Token &&tlOther) { swap(tlOther); return *this; }
  /* -- MOVE assignment constructor ---------------------------------------- */
  Token(Token &&tlOther) :
    /* -- Initialisers ----------------------------------------------------- */
    StrVector{ StdMove(tlOther) }       // Move vector string over
    /* -- No code ---------------------------------------------------------- */
    { }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(Token)               // Suppress default functions for safety
};/* ----------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of module namespace
/* == EoF =========================================================== EoF == */
