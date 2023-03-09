/* == ARGS.HPP ============================================================= **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## This module defines a arguments parsing class you can use to parse  ## **
** ## a plain string of command-line into a array indexed vector.         ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IArgs {                      // Start of module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IStd::P;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* ------------------------------------------------------------------------- */
struct Args :                          // Arguments list class
  /* -- Base classes ------------------------------------------------------- */
  public StrVector                     // A vector of strings
{ /* -- Constructor with string argument ----------------------------------- */
  explicit Args(const string &strArgs)
  { // Get beginning position of usable character and return if not found
    const size_t stFirst = strArgs.find_first_not_of(' ');
    if(stFirst == StdNPos) return;
    // Get ending position of usable character and extract trimmed string
    const size_t stLast = strArgs.find_last_not_of(' ');
    const string strTrimmed{ strArgs.substr(stFirst, stLast - stFirst + 1) };
    // Return if empty else get length of string
    if(strTrimmed.empty()) return;
    const size_t stLength = strTrimmed.size();
    // Boolean to say if we're in quotation marks or not
    bool bInQuotes = false;
    // Boolean to say if we're ignoring the ending quotation
    bool bWantQuote = false;
    // The current quote character being used
    char cQuoteChar = '\0';
    // Start of current argument
    size_t stStart = 0;
    // Enumerate string
    for(size_t stPos = 0; stPos < stLength; ++stPos)
    { // Get character from string and if its a space and not in quotes?
      const char cChar = strTrimmed[stPos];
      if(StdIsSpace(cChar) && !bInQuotes)
      { // If were not at the start of the argument? Extract/add the argument
        if(stStart != stPos)
          emplace_back(strTrimmed.substr(stStart, stPos - stStart));
        // Set next argument starting position
        stStart = stPos + 1;
      } // if it's a argument separator?
      else if(cChar == '\'' || cChar == '"')
      { // If in quotes and it's the ending quotation mark?
        if(bInQuotes && cChar == cQuoteChar)
        { // If we want the quotation? Not anymore
          if(bWantQuote) bWantQuote = false;
          // We don't want it?
          else
          { // Extract new argument
            emplace_back(strTrimmed.substr(stStart, stPos - stStart));
            // Set next argument starting position
            stStart = stPos + 1;
          } // No longer in quotes
          bInQuotes = false;
        } // Not in quotes and at start of argument?
        else if(!bInQuotes)
        { // We're now in quotes
          bInQuotes = true;
          // Set quote character used
          cQuoteChar = cChar;
          // Set next argument starting position if we're at the start
          if(stStart == stPos) stStart = stPos + 1;
          // Else ignore the ending quotation
          else bWantQuote = true;
        }
      }
    } // If there are remaining characters to add? Extract the string
    if(stStart < stLength) emplace_back(strTrimmed.substr(stStart));
  }
  /* -- Constructor that does nothing -------------------------------------- */
  Args(void) = default;
  /* -- Return if list is empty -------------------------------------------- */
  operator bool(void) const { return !empty(); }
};/* -- Build an array of arguments from a string -------------------------- */
static const Args ArgsBuildSafe(const string &strArgs)
  { return strArgs.empty() ? Args{} : Args{ strArgs }; }
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of module namespace
/* == EoF =========================================================== EoF == */
