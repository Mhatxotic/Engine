/* == UTF.HPP ============================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## This module defines a class that allows whole text buffers to be    ## **
** ## parsed for key/value pairs (i.e. k1=v1\nk2=v2) and places them      ## **
** ## into a c++ map class for quick access.                              ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* -- Wrapper for STL character functions (can't put in std.hpp) ----------- */
namespace IStd {                       // Start of module namespace
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* -- Returns if character is hexadecimal (0-9A-Fa-f) ---------------------- */
template<typename IntType>
  constexpr static bool StdIsXDigit(const IntType itChar)
    { return ::std::isxdigit(static_cast<int>(itChar)); }
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of module namespace
/* ------------------------------------------------------------------------- */
namespace IUtf {                       // Start of module namespace
/* ------------------------------------------------------------------------- */
using namespace IStd::P;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* ------------------------------------------------------------------------- */
typedef size_t     Codepoint;          // Type of a UTF-8 character
/* -- Remove const from a pointer ------------------------------------------ */
template<typename TypeTo, typename TypeFrom, typename TypeNonConst =
  remove_const_t<remove_pointer_t<TypeFrom>>*>
requires is_pointer_v<TypeFrom>
static TypeTo UtfToNonConstCast(TypeFrom tfV)
  { return reinterpret_cast<TypeTo>(const_cast<TypeNonConst>(tfV)); }
/* -- Check if C-string is nullptr or blank -------------------------------- */
template<typename PtrType>
  static bool UtfIsCStringValid(const PtrType*const ptpStr)
    { return ptpStr && *ptpStr; }
/* ------------------------------------------------------------------------- */
template<typename PtrType>
  static bool UtfIsCStringNotValid(const PtrType*const ptpStr)
    { return !UtfIsCStringValid<PtrType>(ptpStr); }
/* -- Structure for utf size and character code ---------------------------- */
struct UtfEncoderEx final { const size_t l;
  const union { const uint8_t u8[5]; const char c[5]; } u; };
/* -- Encode specified code and return string and size --------------------- */
static const UtfEncoderEx UtfEncodeEx(const Codepoint cCode)
{ // Macro to extract bits from a codepoint (shift, and, or)
#define X(s,a,o) static_cast<uint8_t>(((cCode >> s) & a) | o)
  // Normal ASCII character?
  if(cCode < 0x80)
    return { 1, {{ X(0,0x7F,0), 0, 0, 0, 0 }}};
  // ASCII/Unicode character between 128-2047?
  else if(cCode < 0x800)
    return { 2, {{ X(6,0x1F,0xC0), X(0,0x3F,0x80), 0, 0, 0 }}};
  // Unicode character between 2048-65535?
  else if(cCode < 0x10000)
    return { 3, {{ X(12,0x0F,0xE0), X(6,0x3F,0x80), X(0,0x3F,0x80), 0, 0 }}};
  // Unicode character between 65536-1114111?
  else if(cCode < 0x110000)
    return { 4, {{ X(18,0x07,0xF0), X(12,0x3F,0x80), X(6,0x3F,0x80),
                   X(0,0x3F,0x80), 0 }}};
  // Done with this macro
#undef X
  // This shouldn't happen, but just incase
  return { 0, {{ 0, 0, 0, 0, 0 }} };
}
/* -- Encode specified code and append it to the specified string ---------- */
static void UtfAppend(const Codepoint cCode, string &strDest)
{ // Encoded UTF8 and append to string
  const UtfEncoderEx ueeCode{ UtfEncodeEx(cCode) };
  strDest.append(ueeCode.u.c, ueeCode.l);
}
/* ------------------------------------------------------------------------- */
static const string UtfDecodeNum(uint32_t ulVal)
{ // Unset the un-needed upper 8-bits. This will act as the nullptr character.
  ulVal &= 0x00FFFFFF;
  // If we have any of the lower 24-bits set? Keep shifting the bits until the
  // bits are on the opposite end so we can cast it to a readable char pointer.
  // We're not changing endian, just reversing each of the eight bits.
  if(ulVal) for(ulVal = SWAP_U32(ulVal); !(ulVal & 0xFF); ulVal >>= 8);
  // Return the shifted value casted to a char pointer address.
  return { reinterpret_cast<const char*>(&ulVal) };
}
/* -- Decode UTF character ------------------------------------------------- */
static void UtfDecode(Codepoint &cState, Codepoint &cCode,
  const unsigned char ucByte)
{ // No state?
  if(cState == 0)
  { // 1-byte sequence?
    if((ucByte & 0x80) == 0) { cCode = ucByte; cState = 0; }
    // 2-byte sequence?
    else if((ucByte & 0xE0) == 0xC0) { cCode = ucByte & 0x1F; cState = 1; }
    // 3-byte sequence?
    else if((ucByte & 0xF0) == 0xE0) { cCode = ucByte & 0x0F; cState = 2; }
    // 4-byte sequence?
    else if((ucByte & 0xF8) == 0xF0) { cCode = ucByte & 0x07; cState = 3; }
    // Invalid byte? Set error state
    else cState = 12;
    // Done
    return;
  } // State set so continuation byte?
  if((ucByte & 0xC0) == 0x80)
    { cCode = (cCode << 6) | (ucByte & 0x3F); --cState; }
  // Invalid continuation byte? Set error state
  else cState = 12;
}
/* -- Pop UTF character from start of string-------------------------------- */
static bool UtfPopFront(string &strStr)
{ // String is not empty?
  if(!strStr.empty())
  { // Get start of buffer
    const char*const cpB = strStr.data(),
      // Get end of buffer
      *const cpE = cpB + strStr.size(),
      // Make moveable pointer
      *cpI = cpB;
    // State and code
    Codepoint cState = 0, cCode = 0;
    // Repeat...
    do { UtfDecode(cState, cCode, static_cast<unsigned char>(*cpI)); }
    // ...Until not end of string, not normal state or error state
    while(++cpI < cpE && cState != 0 && cState != 12);
    // Buffer still tp process and not error state? Erase characters
    if (cpI > cpB && cState != 12)
      { strStr.erase(0, static_cast<size_t>(cpI - cpB)); return true; }
  } // Failed
  return false;
}
/* -- Pop UTF character from end of string --------------------------------- */
static bool UtfPopBack(string &strStr)
{ // String is not empty?
  if(!strStr.empty())
  { // Get start of buffer and end of buffer
    const char*const cpS = strStr.data(), *cpI = cpS + strStr.size();
    // Skip valid UTF8 characters
    while(--cpI >= cpS && (*cpI & 0b10000000) && !(*cpI & 0b01000000));
    // If the pointer moved?
    if(cpI >= cpS)
    { // Remove the characters
      strStr.resize(static_cast<size_t>(cpI - cpS));
      // Success
      return true;
    } // Pointer did not move
  } // Return failure
  return false;
}
/* -- Move UTF character from back of one string to the front of another --- */
static bool UtfMoveBackToFront(string &strSrc, string &strDst)
{ // If string is not empty?
  if(!strSrc.empty())
  { // Get start of buffer and end of buffer
    const char*const cpS = strSrc.data(), *cpI = cpS + strSrc.size();
    // Skip valid UTF8 characters
    while(--cpI >= cpS && (*cpI & 0b10000000) && !(*cpI & 0b01000000));
    // If the pointer moved?
    if(cpI >= cpS)
    { // Amount to resize to
      const size_t stResize = static_cast<size_t>(cpI - cpS);
      // Add the characters we will remove to the beginning of the string
      strDst.insert(0, strSrc, stResize);
      // Remove the characters
      strSrc.resize(stResize);
      // Success
      return true;
    } // Pointer did not move
  } // Return failure
  return false;
}
/* -- Move UTF character from front of one string to the back of another --- */
static bool UtfMoveFrontToBack(string &strSrc, string &strDst)
{ // If the string is not empty?
  if(!strSrc.empty())
  { // Get start of buffer
    const char*const cpB = strSrc.data(),
      // Get end of buffer
      *const cpE = cpB + strSrc.size(),
      // Set start of string as enumerator
      *cpI = cpB;
    // Utf state and return code
    Codepoint cState = 0, cCode = 0;
    // Walk through the string until we get to a null terminator
    do UtfDecode(cState, cCode, static_cast<unsigned char>(*cpI));
      while(++cpI < cpE && cState);
    // Get size to remove
    const size_t stBytes = static_cast<size_t>(cpI - cpB);
    // Add the characters we will remove to the end of the string
    strDst.insert(strDst.size(), strSrc, 0, stBytes);
    // If we did not remove skipped characters?
    if(cpI >= cpB)
    { // Move character from source string
      strSrc.erase(0, stBytes);
      // Success
      return true;
    } // Removed skipped enumerators
  } // Return failure
  return false;
}
/* == Convert a unicode or ansi string to UTF8 ----------------------------- */
template<typename CharType>
  static const string UtfFromWide(const CharType *ctPtr)
{ // Empty string if nullptr or string empty
  if(UtfIsCStringNotValid<CharType>(ctPtr)) return {};
  // Output string
  string strOut;
  // For each character. Get character and convert to UTF8
  do
  { // Encode character
    const UtfEncoderEx ueeCode{ UtfEncodeEx(static_cast<Codepoint>(*ctPtr)) };
    // Append to string
    strOut.append(ueeCode.u.c, ueeCode.l);
    // Until end of string
  } while(*(++ctPtr));
  // Return string
  return strOut;
}
/* -- Template to reserve part of another object --------------------------- */
template<class AnyObject>struct Reserved : public AnyObject
  { explicit Reserved(const size_t stSize) { this->reserve(stSize); } };
/* -- UTF8 decoder helper class -------------------------------------------- */
class UtfDecoder final                 // UTF8 string decoder helper
{ /* ----------------------------------------------------------------------- */
  const unsigned char *ucpStr, *ucpPtr; // String and pointer to that string
  /* -- Test a custom condition on each character -------------------------- */
  template<class OpFunc>bool UtfIsType()
  { // For each character, test if it is a control character
    while(const Codepoint cCode = UtfNext()) if(OpFunc{}.T(cCode))
      return false;
    // Is a displayable character
    return true;
  }
  /* -- Iterator --------------------------------------------------- */ public:
  template<typename CharType=Codepoint>CharType UtfNext()
  { // Walk through the string until we get to a null terminator
    for(Codepoint cState = 0, cCode = 0; *ucpPtr > 0; ++ucpPtr)
    { // Decode the specified character
      UtfDecode(cState, cCode, *ucpPtr);
      // Ignore if we haven't got a valid UTF8 character yet.
      if(cState) continue;
      // Move position onwards
      ++ucpPtr;
      // Return the UTF8 character as requested type (limit to 0-2097151).
      // UTF8 can only address 1,112,064 (0x10F800) total characters
      return static_cast<CharType>(cCode & 0x1fffff);
    } // Invald string. Return null character
    return 0;
  }
  /* -- Check if is displayable character ---------------------------------- */
  bool UtfIsDisplayable()
  { // For each character, test if it is a control character
    struct Op{ Op() = default;
      static bool T(const Codepoint cCode) { return cCode < 0x20; }};
    return UtfIsType<Op>();
  }
  /* -- Check if is ASCII compatible --------------------------------------- */
  bool UtfIsASCII()
  { // For each character, test if it is valid ASCII
    struct Op{ Op() = default;
      static bool T(const Codepoint cCode) { return cCode > 0x7F; }};
    return UtfIsType<Op>();
  }
  /* -- Check if is extended ASCII compatible ------------------------------ */
  bool UtfIsExtASCII()
  { // For each character, test if it is valid extended ASCII
    struct Op{ Op() = default;
      static bool T(const Codepoint cCode) { return cCode > 0xFF; }};
    return UtfIsType<Op>();
  }
  /* -- Length ------------------------------------------------------------- */
  size_t UtfLength()
  { // Length of string
    size_t stLength = 0;
    // Walk through the string until we get to a null terminator
    while(UtfNext()) ++stLength;
    // Return length
    return stLength;
  }
  /* -- Skip number of utf8 characters ------------------------------------- */
  void UtfSkip(size_t stAmount) { while(stAmount-- && UtfNext()); }
  /* -- Skip characters and return last position (Char::* needs this) ------ */
  void UtfIgnore(const Codepoint cCode)
  { // Saved position
    const unsigned char *ucpPtrSaved;
    // While the chars are matched
    do { ucpPtrSaved = ucpPtr; } while(UtfNext() == cCode);
    // Return last position saved
    ucpPtr = ucpPtrSaved;
  }
  /* -- Skip a value ------------------------------------------------------- */
  template<typename IntType=Codepoint,size_t stMaximum=8>size_t UtfSkipValue()
  { // Ignore if at the end of the string or it is empty
    if(UtfFinished()) return false;
    // Save position and expected end position. The end position CAN be OOB
    // but is safe as all UTF strings are expected to be null terminated.
    const unsigned char*const ucpSaved = ucpPtr,
                       *const ucpEnd = ucpSaved + stMaximum;
    // Add characters as long as they are valid hexadecimal characters
    while(StdIsXDigit(*(ucpPtr++)) && ucpPtr < ucpEnd);
    // Return number of bytes read
    return static_cast<size_t>(ucpPtr - ucpSaved);
  }
  /* -- Scan a value ------------------------------------------------------- */
  template<typename IntType=Codepoint,size_t stMaximum=8>
    size_t UtfScanValue(IntType &itOut)
  { // Ignore if at the end of the string or it is empty
    if(UtfFinished()) return 0;
    // Capture up to eight characters
    Reserved<string> strMatched{ stMaximum };
    // Add characters as long as they are valid hexadecimal characters and the
    // matched string has not reached eight characters. Anything that could be
    // unicode character should auto break anyway so this should be safe.
    while(StdIsXDigit(*ucpPtr) && strMatched.length() < stMaximum)
      strMatched.push_back(static_cast<char>(*(ucpPtr++)));
    // Return failure if nothing added
    if(strMatched.empty()) return 0;
    // Put value into input string stream
    istringstream isS{ strMatched };
    // Push value into integer
    isS >> hex >> itOut;
    // Return bytes read
    return strMatched.length();
  }
  /* -- Slice string from pushed position to current position -------------- */
  const string UtfSlice(const unsigned char*const ucpPos) const
    { return { reinterpret_cast<const char*>(ucpPos),
        static_cast<size_t>(ucpPtr - ucpPos) }; }
  /* -- Return if string is valid ------------------------------------------ */
  bool UtfValid() const { return !!ucpStr; }
  /* -- Return if pointer is at the end ------------------------------------ */
  bool UtfFinished() const { return !*ucpPtr; }
  /* -- Reset pointer ------------------------------------------------------ */
  void UtfReset() { UtfSetCPtr(const_cast<unsigned char*>(ucpStr)); }
  /* -- Reset pointer with new strings ------------------------------------- */
  void UtfReset(const unsigned char*const ucpNew)
    { ucpPtr = ucpStr = ucpNew; }
  void UtfReset(const char*const cpNew)
    { ucpPtr = ucpStr = reinterpret_cast<const unsigned char*>(cpNew); }
  /* -- Return raw string -------------------------------------------------- */
  const unsigned char *UtfGetCString() const { return ucpStr; }
  /* -- Return raw string -------------------------------------------------- */
  const unsigned char *UtfGetCPtr() const { return ucpPtr; }
  /* -- Pop position ------------------------------------------------------- */
  void UtfSetCPtr(const unsigned char*const ucpPos) { ucpPtr = ucpPos; }
  /* -- Convert to wide string --------------------------------------------- */
  const wstring UtfWide()
  { // Output string
    wstring wstrOut;
    // Add character to string
    while(const Codepoint cCode = UtfNext())
      wstrOut += static_cast<wchar_t>(cCode);
    // Return string
    return wstrOut;
  }
  /* -- Constructor that initialises a pointer ----------------------------- */
  template<typename PtrType> requires is_pointer_v<PtrType>
    explicit UtfDecoder(PtrType ptSrc) :
    /* -- Initialisers ----------------------------------------------------- */
    ucpStr(reinterpret_cast<const unsigned char*>(
      const_cast<const PtrType>(ptSrc))),
    ucpPtr(ucpStr)                     // Copy for current position
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- Constructor that initialises any string object --------------------- */
  template<class StrType> requires is_class_v<StrType>
    explicit UtfDecoder(const StrType &stStr) :
    /* -- Initialisers ----------------------------------------------------- */
    ucpStr(reinterpret_cast<const unsigned char*>(stStr.data())),
    ucpPtr(ucpStr)                     // Copy for current position
    /* -- No code ---------------------------------------------------------- */
    {}
};/* -- Word wrap a utf string --------------------------------------------- */
static const StrVector UtfWordWrap(const string &strText, const size_t stWidth,
  const size_t stIndent)
{ // Return empty array if width is invalid.
  if(!stWidth || stWidth <= stIndent) return {};
  // If string is empty, return at least one item because this could be
  // a blank line on purpose.
  if(strText.empty()) return { strText };
  // The line list
  StrVector svLines;
  // Premade indent
  string strIndent;
  // Make string into utf string
  UtfDecoder udStr{ strText };
  // Save position
  const unsigned char*const ucpString = udStr.UtfGetCString();
  const unsigned char *ucpStart = udStr.UtfGetCPtr(),
                      *ucpSpace = ucpStart;
  // Helper function copy part of a string into the the word buffer
  const auto Snip = [ucpString, &svLines, &strIndent, &strText]
    (const unsigned char*const ucpS, const unsigned char*const ucpE) {
    svLines.emplace_back(strIndent + strText.substr(
      static_cast<size_t>(ucpS - ucpString),
      static_cast<size_t>(ucpE - ucpS)));
  }; // Current column
  size_t stColumn = 0;
  // Until we're out of valid UTF8 characters
  while(const Codepoint cCode = udStr.UtfNext())
  { // Character found
    ++stColumn;
    // Is it a space character? Mark the cut count and goto next character
    if(cCode == ' ') { ucpSpace = udStr.UtfGetCPtr(); continue; }
    // Is other character and we're not at the limit? Goto next character
    if(stColumn < stWidth) continue;
    // We already found where we can force a line break?
    // Add indentation if not first line then set current position to where
    // the last space was found
    if(ucpSpace != ucpStart)
    { // Copy up to the space we found traversing the string
      Snip(ucpStart, ucpSpace);
      // Reset position to where we found the last whitespace
      udStr.UtfSetCPtr(ucpSpace);
      // Update start of next line
      ucpStart = ucpSpace = udStr.UtfGetCPtr();
    } // No space found on this line.?
    else
    { // The wrap position is at the start
      ucpSpace = udStr.UtfGetCPtr();
      // Copy up to the last space we found
      Snip(ucpStart, ucpSpace);
      // Update start of next line
      ucpStart = ucpSpace;
    } // Set indentation for next line
    if(strIndent.empty()) strIndent.assign(stIndent, ' ');
    // Reset column to indent size
    stColumn = stIndent;
  } // If we are not at end of string? Add the remaining characters
  if(udStr.UtfGetCPtr() != ucpStart) Snip(ucpStart, udStr.UtfGetCPtr());
  // Return list
  return svLines;
}
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of module namespace
/* == EoF =========================================================== EoF == */
