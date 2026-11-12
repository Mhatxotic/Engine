/* == UTFCP.HPP ============================================================ **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## Defines a class which helps decode raw string bytes into a UTF-8    ## **
** ## codepoint character.                                                ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IUtfCodepoint {              // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IStd::P;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* ------------------------------------------------------------------------- */
using UtfCodepoint = unsigned;         // Type of a UTF-8 character
/* -- Decode UTF character ------------------------------------------------- */
class UtfDecodeChar final
{ /* -- Private typedefs --------------------------------------------------- */
  enum UtfStateId : unsigned
  { /* --------------------------------------------------------------------- */
    USI_NONE                     =  0, // There are no more UTF-8 control flags
    USI_SINGLE                   =  1, // There is one byte after the first
    USI_DOUBLE                   =  2, // There are two bytes after the first
    USI_TRIPLE                   =  3, // There are three bytes after the first
    USI_ERROR                    = 12  // Malformed codepoint detected
  };/* -- Private variables ------------------------------------------------ */
  UtfStateId       usiState;           // The current state
  UtfCodepoint     utcChar;            // The UTF8 character being built
  /* -- Set a new codepoint and state -------------------------------------- */
  void UtfStateSet(const UtfStateId usiNState) { usiState = usiNState; }
  void UtfStateSetError() { UtfStateSet(USI_ERROR); }
  void UtfSetCodepoint(const UtfCodepoint utcNChar) { utcChar = utcNChar; }
  void UtfSetCodepointAndState(const UtfCodepoint utcNChar,
    const UtfStateId usiNState)
  { UtfSetCodepoint(utcNChar); UtfStateSet(usiNState); }
  /* -- Return the state ------------------------------------------- */ public:
  UtfStateId UtfGetState() const { return usiState; }
  /* -- Return if state is set --------------------------------------------- */
  bool UtfIsStateSet() const { return UtfGetState() != USI_NONE; }
  bool UtfIsStateNotSet() const { return !UtfIsStateSet(); }
  bool UtfIsErrorStateSet() const { return UtfGetState() == USI_ERROR; }
  bool UtfIsErrorStateNotSet() const { return !UtfIsErrorStateSet(); }
  /* -- Get the current character ------------------------------------------ */
  UtfCodepoint UtfGetCodepoint() const { return utcChar; }
  /* -- Decode the specified character ------------------------------------- */
  void UtfDecode(const unsigned char ucByte)
  { // No state?
    if(UtfIsStateNotSet()) [[likely]]
    { // 1-byte sequence?
      if((ucByte & 0x80) == 0) utcChar = ucByte;
      // 2-byte sequence?
      else if((ucByte & 0xE0) == 0xC0)
        UtfSetCodepointAndState(ucByte & 0x1F, USI_SINGLE);
      // 3-byte sequence?
      else if((ucByte & 0xF0) == 0xE0)
        UtfSetCodepointAndState(ucByte & 0x0F, USI_DOUBLE);
      // 4-byte sequence?
      else if((ucByte & 0xF8) == 0xF0)
        UtfSetCodepointAndState(ucByte & 0x07, USI_TRIPLE);
      // Invalid byte? Set error state
      else UtfStateSetError();
      // Done
      return;
    } // Continuation flag? Transfer bits to codepoint and decrement bytes left
    if((ucByte & 0xC0) == 0x80)
      UtfSetCodepointAndState((UtfGetCodepoint() << 6) | (ucByte & 0x3F),
        static_cast<UtfStateId>(
          static_cast<StdUnderlyingType<UtfStateId>>(usiState) - 1));
    // Invalid continuation byte so set error state
    else UtfStateSetError();
  }
  /* -- Default constructor ------------------------------------------------ */
  UtfDecodeChar() : usiState(USI_NONE), utcChar(0) {}
};/* ----------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
