/* == UTFENC.HPP =========================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## Handles encoding of integers into utf-8 byte sequences              ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IUtfEncoder {                // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IStd::P;               using namespace IUtfCodepoint::P;
/* -- Private typedefs ----------------------------------------------------- */
using Array = StdArray<uint8_t, 5>;    // The array to store byte sequence
/* ------------------------------------------------------------------------- */
struct UtfResult
{ /* ----------------------------------------------------------------------- */
  const size_t     stBytes;            // Number of bytes in sequence
  const Array      aBytes;             // UTF-8 byte sequence
  /* -- Constructor -------------------------------------------------------- */
  constexpr explicit UtfResult(
    /* -- Arguments -------------------------------------------------------- */
    const size_t   stNBytes = 0,       // Number of bytes in sequence
    const uint8_t  ub1 = '\0',         // First byte
    const uint8_t  ub2 = '\0',         // Second byte
    const uint8_t  ub3 = '\0',         // Third byte
    const uint8_t  ub4 = '\0'          // Fourth byte
    ): /* -- Initialisers -------------------------------------------------- */
    stBytes{ stNBytes },               // Set number of bytes in sequence
    aBytes{ ub1,ub2,ub3,ub4, '\0' }    // Set byte sequence (last always null)
    /* -- No code ---------------------------------------------------------- */
    {}
};/* ----------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* ------------------------------------------------------------------------- */
class UtfEncoder final :
  /* -- Base classes ------------------------------------------------------- */
  private UtfResult                    // Containing the byte sequence
{ /* -- Initialise a byte -------------------------------------------------- */
  constexpr static uint8_t UtfInit(const UtfCodepoint utcChar,
    const unsigned uShift, const unsigned uAndFlags, const unsigned uOrFlags)
  { return static_cast<uint8_t>(static_cast<uint8_t>
      (((utcChar >> uShift) & uAndFlags) | uOrFlags)); }
  /* -- Get bytes -------------------------------------------------- */ public:
  const char *UtfBytes() const
    { return reinterpret_cast<const char*>(aBytes.data()); }
  /* -- Get length --------------------------------------------------------- */
  constexpr size_t UtfLength() const { return stBytes; }
  /* -- Append to a string ------------------------------------------------- */
  void UtfAppend(StdString &strDest) const
    { strDest.append(UtfBytes(), UtfLength()); }
  /* -- Append to a string and return size --------------------------------- */
  size_t UtfAppendRetSize(StdString &strDest) const
    { UtfAppend(strDest); return UtfLength(); }
  /* -- Insert into a string ----------------------------------------------- */
  void UtfAppend(StdString &strDest, const size_t stPos) const
    { strDest.insert(stPos, UtfBytes()); }
  /* -- Constructor to encode the codepoint efficiently. ------------------- */
  constexpr explicit UtfEncoder(const UtfCodepoint utcChar) :
    /* -- Initialisers ----------------------------------------------------- */
    UtfResult{ [utcChar]() constexpr->UtfResult
    { // Check if the codepoint represents a standard ASCII character
      if(utcChar < 0x000080)
        return UtfResult{ 1, UtfInit(utcChar,  0, 0x7F, 0x00) };
      // Check if the codepoint requires 2 bytes in UTF-8 encoding.
      if(utcChar < 0x000800)
        return UtfResult{ 2, UtfInit(utcChar,  6, 0x1F, 0xC0),
                             UtfInit(utcChar,  0, 0x3F, 0x80) };
      // Check if the codepoint requires 3 bytes in UTF-8 encoding.
      if(utcChar < 0x010000)
        return UtfResult{ 3, UtfInit(utcChar, 12, 0x0F, 0xE0),
                             UtfInit(utcChar,  6, 0x3F, 0x80),
                             UtfInit(utcChar,  0, 0x3F, 0x80) };
      // Check if the codepoint requires 4 bytes in UTF-8 encoding.
      if(utcChar < 0x110000)
        return UtfResult{ 4, UtfInit(utcChar, 18, 0x07, 0xF0),
                             UtfInit(utcChar, 12, 0x3F, 0x80),
                             UtfInit(utcChar,  6, 0x3F, 0x80),
                             UtfInit(utcChar,  0, 0x3F, 0x80) };
      // Fallback for invalid codepoints that exceed the valid Unicode range.
      return UtfResult{};
    }() }
    /* -- No code ---------------------------------------------------------- */
    {}
};/* ----------------------------------------------------------------------- */
/* -- Encode specified code and append it to the specified string ---------- */
static void UtfAppend(const UtfCodepoint utcChar, StdString &strDest)
  { UtfEncoder{ utcChar }.UtfAppend(strDest); }
/* -- Same as above function but returns the size of it -------------------- */
static size_t UtfAppendRetSize(const UtfCodepoint utcChar, StdString &strDest)
  { return UtfEncoder{ utcChar }.UtfAppendRetSize(strDest); }
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
