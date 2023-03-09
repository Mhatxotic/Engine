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
/* ------------------------------------------------------------------------- */
struct Args :                          // Arguments list
  /* -- Dependencies ------------------------------------------------------- */
  public StrVector                     // List of arguments parsed
{ /* -- Direct access using class variable name which returns if !empty ---- */
  operator bool(void) const { return !empty(); }
  /* -- Constructor -------------------------------------------------------- */
  Args(void) { }
  /* -- Constructor with string argument ----------------------------------- */
  explicit Args(const string &strArgs)
  { // Common characters used when parsing
    static constexpr const char
      cSpace      = ' ',  // Argument seperator unless enclosed on quotes
      cApostrophy = '\'', // First possible encapsulation character
      cQuotation  = '"',  // Second possible encapsulation character
      cNull       = '\0'; // Null termination of a string
    // The current character position in the arguments string
    size_t stPos = 0;
    // Ignore the prefixed whitespace characters
    while(strArgs[stPos] == cSpace) if(++stPos >= strArgs.length()) return;
    // This is the length of the usable string
    size_t stLength = strArgs.length();
    // Ignore all suffixed whitespace characters
    for(; strArgs[stLength-1] == cSpace; --stLength);
    // This tells us if we're in a argument that started with a separator
    bool bQuote;
    // Set if we're starting an argument with a valid separator character
    switch(strArgs[stPos])
    { case cApostrophy: [[fallthrough]];
      case cQuotation: bQuote = true; break;
      default: bQuote = false; break;
    } // Set starting position because this current character is valid
    size_t stStart = stPos;
    // Until we reach end of string
    if(stPos < stLength)
    { // Prefix and suffix being used for whole strings with spaces
      char cSepChar = cNull;
      // Repeat...
      do
      { // Check current character
        switch(const char cChar = strArgs[stPos])
        { // Whitespace?
          case cSpace:
          { // Break if we already have a seperator character because
            // whitespaces are allowed until we find the end of it.
            if(cSepChar != cNull) break;
            // If we have an argument to save?
            if(stPos > stStart)
              emplace_back(strArgs.substr(stStart, stPos-stStart));
            // Skip spaces and return the list if we've run out of characters
            while(strArgs[stPos] == cSpace) if(++stPos >= stLength) return;
            // Reset starting and ending position
            stStart = stPos;
            // Set if we're in quotation marks
            switch(strArgs[stPos])
            { case cApostrophy: [[fallthrough]];
              case cQuotation: bQuote = true; break;
              default: bQuote = false; break;
            } // Test current character, don't move position ahead
            continue;
          } // Apostrophe or quotation mark?
          case cApostrophy : case cQuotation:
          { // Have a separator character?
            if(cSepChar != cNull)
            { // Its the seperator char? Reset seperator character
              if(cChar == cSepChar)
              { // Get position plus one and if at end of string?
                const size_t stPosP1 = stPos + 1;
                if(stPosP1 >= stLength)
                { // If we're in quotation marks
                  if(bQuote)
                  { // Move start forward to ignore the starting quote char
                    ++stStart;
                    // Add new entry
                    emplace_back(strArgs.substr(stStart, stPos-stStart));
                  } // Not in quotation marks? Add new entry
                  else emplace_back(strArgs.substr(stStart, stPosP1-stStart));
                  // Return the list
                  return;
                } // Is a space character?
                if(strArgs[stPosP1] == cSpace)
                { // If we're in quotation marks
                  if(bQuote)
                  { // Move start forward to ignore the starting quote char
                    ++stStart;
                    // Add new entry
                    emplace_back(strArgs.substr(stStart, stPos-stStart));
                  } // Not in quotation marks? Add new entry
                  else emplace_back(strArgs.substr(stStart, stPosP1-stStart));
                  // Skip spaces
                  while(strArgs[++stPos] == cSpace)
                    if(stPos >= stLength) return;
                  // Reset starting and ending position
                  stStart = stPos;
                  // Set if we're in quotation marks
                  switch(strArgs[stPos])
                  { case cApostrophy: [[fallthrough]];
                    case cQuotation: bQuote = true; break;
                    default: bQuote = false; break;
                  } // Reset separator character
                  cSepChar = cNull;
                  // Test current character, don't move position ahead
                  continue;
                } // Not at end of string and not a space character
              } // Not separator character
            } // Set new seperator character
            else cSepChar = cChar;
            // Done
            break;
          } // Everything else
          default: break;
        } // Move forward in string
        ++stPos;
      } // ...until at end of string
      while(stPos < stLength);
    } // If we have an argument to save then add the final argument to the list
    if(stPos > stStart) emplace_back(strArgs.substr(stStart, stPos-stStart));
  }
};
/* -- Build an array of arguments from a string ---------------------------- */
static const Args ArgsBuildSafe(const string &strArgs)
  { return strArgs.empty() ? Args{} : Args{ strArgs }; }
/* ------------------------------------------------------------------------- */
}                                      // End of module namespace
/* == EoF =========================================================== EoF == */
