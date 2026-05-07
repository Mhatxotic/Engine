/* == UTF.HPP ============================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## Our speedy UTF-8 character scanning and manipulation class.         ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IUtf {                       // Start of module namespace
/* ------------------------------------------------------------------------- */
using namespace ICommon::P;            using namespace IStd::P;
using namespace IEndian::P;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* ------------------------------------------------------------------------- */
using Codepoint = size_t;              // Type of a UTF-8 character
/* -- Check if C-string is nullptr or blank -------------------------------- */
template<typename CharType>
  requires (!StdIsPointer<CharType>)
static bool UtfIsCStringValid(const CharType*const ctStr)
  { return ctStr && *ctStr; }
/* ------------------------------------------------------------------------- */
template<typename CharType>
  requires (!StdIsPointer<CharType>)
static bool UtfIsCStringNotValid(const CharType*const ctStr)
  { return !UtfIsCStringValid<CharType>(ctStr); }
/* -- Structure for utf size and character code ---------------------------- */
struct UtfEncoderEx final { const size_t l;
  const union { const uint8_t u8[5]; const char c[5]; } u; };
/* -- Encode specified code and return string and size --------------------- */
static UtfEncoderEx UtfEncodeEx(const Codepoint coCode)
{ // Macro to extract bits from a codepoint (shift, and, or)
#define X(s,a,o) static_cast<uint8_t>(((coCode >> s) & a) | o)
  // Normal ASCII character?
  if(coCode < 0x80)
    return { 1, {{ X(0,0x7F,0), 0, 0, 0, 0 }}};
  // ASCII/Unicode character between 128-2047?
  else if(coCode < 0x800)
    return { 2, {{ X(6,0x1F,0xC0), X(0,0x3F,0x80), 0, 0, 0 }}};
  // Unicode character between 2048-65535?
  else if(coCode < 0x10000)
    return { 3, {{ X(12,0x0F,0xE0), X(6,0x3F,0x80), X(0,0x3F,0x80), 0, 0 }}};
  // Unicode character between 65536-1114111?
  else if(coCode < 0x110000)
    return { 4, {{ X(18,0x07,0xF0), X(12,0x3F,0x80), X(6,0x3F,0x80),
                   X(0,0x3F,0x80), 0 }}};
  // Done with this macro
#undef X
  // This shouldn't happen, but just incase
  return { 0, {{ 0, 0, 0, 0, 0 }} };
}
/* -- Encode specified code and append it to the specified string ---------- */
static void UtfAppend(const Codepoint coCode, StdString &strDest)
{ // Encoded UTF8 and append to string
  const UtfEncoderEx ueeCode{ UtfEncodeEx(coCode) };
  strDest.append(ueeCode.u.c, ueeCode.l);
}
/* ------------------------------------------------------------------------- */
template<typename IntType = unsigned>
  requires StdIsIntegral<IntType> && (sizeof(IntType) >= sizeof(uint32_t))
static StdString UtfDecodeNum(IntType itVal)
{ // Unset the un-needed upper 8-bits. This will act as the nullptr character.
  itVal &= 0xFFFFFF;
  // If we have any of the lower 24-bits set? Keep shifting the bits until the
  // bits are on the opposite end so we can cast it to a readable char pointer.
  // We're not changing endian, just reversing each of the eight bits.
  if(itVal) for(itVal = EndianSwap32(itVal); !(itVal & 0xFF); itVal >>= 8);
  // Return the shifted value casted to a char pointer address.
  return { reinterpret_cast<const char*>(&itVal) };
}
/* -- Decode UTF character ------------------------------------------------- */
static void UtfDecode(Codepoint &coState, Codepoint &coCode,
  const unsigned char ucByte)
{ // No state?
  if(coState == 0)
  { // 1-byte sequence?
    if((ucByte & 0x80) == 0) { coCode = ucByte; coState = 0; }
    // 2-byte sequence?
    else if((ucByte & 0xE0) == 0xC0) { coCode = ucByte & 0x1F; coState = 1; }
    // 3-byte sequence?
    else if((ucByte & 0xF0) == 0xE0) { coCode = ucByte & 0x0F; coState = 2; }
    // 4-byte sequence?
    else if((ucByte & 0xF8) == 0xF0) { coCode = ucByte & 0x07; coState = 3; }
    // Invalid byte? Set error state
    else coState = 12;
    // Done
    return;
  } // State set so continuation byte?
  if((ucByte & 0xC0) == 0x80)
    { coCode = (coCode << 6) | (ucByte & 0x3F); --coState; }
  // Invalid continuation byte? Set error state
  else coState = 12;
}
/* -- Pop UTF character from start of string-------------------------------- */
static bool UtfPopFront(StdString &strStr)
{ // String is not empty?
  if(strStr.empty()) [[unlikely]] return false;
  // Starting iterator, state and code
  StringConstIt sciIt{ strStr.cbegin() };
  Codepoint coState = 0, coCode = 0;
  // Repeat...
  do { UtfDecode(coState, coCode, static_cast<unsigned char>(*sciIt)); }
  // ...Until not end of string, not normal state or error state
  while(++sciIt != strStr.cend() && coState != 0 && coState != 12);
  // Return if we didn't process anything or UTF-8 code is in a error state
  if(coState == 12) [[unlikely]] return false;
  // Remove the raw bytes and return success
  strStr.erase(strStr.cbegin(), sciIt);
  return true;
}
/* -- Pop UTF character from end of string --------------------------------- */
static bool UtfPopBack(StdString &strStr)
{ // Return failure if string is empty
  if(strStr.empty()) [[unlikely]] return false;
  // Get beginning and end iterators
  StringIt siIt{ strStr.end() };
  // Move backward to find the start of the last UTF-8 code point and decrement
  // iterator until we find a byte that is not a continuation byte.
  // if (!(ucChar & 0x80) ||           // ASCII start byte (0xxxxxxx)
  //      (ucChar & 0x40)) break;      // leading byte (11xxxxxx)
  while((static_cast<unsigned char>(*(--siIt)) & 0xC0) == 0x80
    && siIt != strStr.cbegin());
  // Return if iterator didn't move
  if(siIt == strStr.cend()) [[unlikely]] return false;
  // Erase from that iterator to end and return success
  strStr.erase(siIt, strStr.cend());
  return true;
}
/* -- Move UTF character from back of one string to the front of another --- */
static bool UtfMoveBackToFront(StdString &strSrc, StdString &strDst)
{ // Failed if empty string
  if(strSrc.empty()) [[unlikely]] return false;
  // Start from the last character
  StringIt siIt{ strSrc.end() };
  // Move backward to find the start of the UTF-8 multibyte character. In UTF-8
  // continuation bytes match the pattern 10xxxxxx. We stop when we hit a lead
  // byte (0xxxxxxx or 11xxxxxx) or reach the beginning.
  while(--siIt != strSrc.begin() && (*siIt & 0b11000000) == 0b10000000);
  // Insert the char sequence from 'it' to the end into the front of strDst
  strDst.insert(strDst.begin(), siIt, strSrc.end());
  // Erase the moved characters from the source string and return success
  strSrc.erase(siIt, strSrc.end());
  return true;
}
/* -- Move UTF character from front of one string to the back of another --- */
static bool UtfMoveFrontToBack(StdString &strSrc, StdString &strDst)
{ // Failed if empty string
  if(strSrc.empty()) [[unlikely]] return false;
  // Starting iterator, state and code
  StringIt siIt{ strSrc.begin() };
  Codepoint coState = 0, coCode = 0;
  // Decode bytes from the front...
  do { UtfDecode(coState, coCode, static_cast<unsigned char>(*siIt)); }
  // ...Until a full UTF-8 codepoint is processed or we hit the end of string.
  while(++siIt != strSrc.cend() && coState != 0);
  // Append the sequence from the front of strSrc to the back of strDst
  strDst.insert(strDst.end(), strSrc.begin(), siIt);
  // Erase the moved character sequence from the front of strSrc and return
  strSrc.erase(strSrc.begin(), siIt);
  return true;
}
/* == Convert a unicode or ansi string to UTF8 ----------------------------- */
template<typename StrType>
  requires StdIsClass<StrType>
static StdString UtfFromWide(const StrType &strStr)
{ // Enumerate string and convert all characters to UTF8
  StdString strOut;
  for(auto aChar : strStr)
  { // Encode byte and append it to string
    const UtfEncoderEx ueeCode{ UtfEncodeEx(static_cast<Codepoint>(aChar)) };
    strOut.append(ueeCode.u.c, ueeCode.l);
  } // Return string
  return strOut;
}
/* -- UTF8 decoder helper class -------------------------------------------- */
class UtfDecoder final :               // UTF8 string decoder helper
  /* ----------------------------------------------------------------------- */
  private StdStringView                // Address and size of string
{ /* ----------------------------------------------------------------------- */
  StringViewConstIt svciIt;            // Current position
  /* -- Test a custom condition on each character -------------------------- */
  template<class OpFunc>bool UtfIsType()
  { // For each character, test if it is a control character
    while(const Codepoint coCode = UtfNext())
      if(OpFunc{}.T(coCode)) return false;
    // Is a displayable character
    return true;
  }
  /* -- Iterator --------------------------------------------------- */ public:
  template<typename CharType = Codepoint>CharType UtfNext()
  { // Walk through the string until we get to a null terminator
    for(Codepoint coState = 0, coCode = 0; UtfGetPos() != cend(); ++svciIt)
    { // Decode the specified character
      UtfDecode(coState, coCode, UtfGetByte<unsigned char>());
      // Ignore if we haven't got a valid UTF8 character yet.
      if(coState) continue;
      // Move position onwards
      ++svciIt;
      // Return the UTF8 character as requested type (limit to 0-2097151).
      // UTF8 can only address 1,112,064 (0x10F800) total characters
      return static_cast<CharType>(coCode & 0x1fffff);
    } // Invald string. Return null character
    return 0;
  }
  /* -- Check if is displayable character ---------------------------------- */
  bool UtfIsDisplayable()
  { // For each character, test if it is a control character
    struct Op{ Op() = default;
      static bool T(const Codepoint coCode) { return coCode < 0x20; }};
    return UtfIsType<Op>();
  }
  /* -- Check if is ASCII compatible --------------------------------------- */
  bool UtfIsASCII()
  { // For each character, test if it is valid ASCII
    struct Op{ Op() = default;
      static bool T(const Codepoint coCode) { return coCode > 0x7F; }};
    return UtfIsType<Op>();
  }
  /* -- Check if is extended ASCII compatible ------------------------------ */
  bool UtfIsExtASCII()
  { // For each character, test if it is valid extended ASCII
    struct Op{ Op() = default;
      static bool T(const Codepoint coCode) { return coCode > 0xFF; }};
    return UtfIsType<Op>();
  }
  /* -- Length ------------------------------------------------------------- */
  size_t UtfLength()
  { // Walk through the string until we get to a null terminator and return
    size_t stLength = 0;
    while(UtfNext()) ++stLength;
    return stLength;
  }
  /* -- Skip number of utf8 characters ------------------------------------- */
  void UtfSkip(size_t stAmount) { while(stAmount-- && UtfNext()); }
  /* -- Skip characters and return last position (Char::* needs this) ------ */
  void UtfIgnore(const Codepoint coCode)
  { // Save position and keep recording the position until error or finish
    StringViewConstIt svciSavedIt{ UtfGetPos() };
    while(UtfNext() == coCode) svciSavedIt = UtfGetPos();
    // Return last position saved
    UtfSetPos(svciSavedIt);
  }
  /* -- Scan a value ------------------------------------------------------- */
  template<typename IntType = Codepoint, size_t stMaximum = 8>
    size_t UtfScanValue(IntType &itOut)
  { // Ignore if at the end of the string or it is empty
    if(UtfFinished()) return 0;
    // Capture up to eight characters
    StdReserved<StdString> strMatched{ stMaximum };
    // Add characters as long as they are valid hexadecimal characters and the
    // matched string has not reached eight characters. Anything that could be
    // unicode character should auto break anyway so this should be safe.
    while(StdIsXDigit(UtfGetByte<int>()) && strMatched.size() < stMaximum)
      strMatched.push_back(static_cast<char>(*(svciIt++)));
    // Return failure if nothing added
    if(strMatched.empty()) return 0;
    // Put value into input string stream
    StdIStringStream isS{ strMatched };
    // Push value into integer
    isS >> StdIOSHex >> itOut;
    // Return bytes read
    return strMatched.size();
  }
  /* -- Slice string from pushed position to current position -------------- */
  StdStringView UtfSlice(const StringViewConstIt svciPosIt) const
    { return { &(*svciPosIt), StdDistance<size_t>(svciPosIt, UtfGetPos()) }; }
  /* -- Return if string is valid ------------------------------------------ */
  bool UtfValid() const { return !empty(); }
  /* -- Return if pointer is at the end ------------------------------------ */
  bool UtfFinished() const { return UtfGetPos() == cend(); }
  /* -- Reset pointer ------------------------------------------------------ */
  void UtfReset() { UtfSetPos(UtfGetStartPos()); }
  /* -- Reset pointer to specified position -------------------------------- */
  void UtfReset(StdStringView &&strvNew) { swap(strvNew); UtfReset(); }
  /* -- Return current string position ------------------------------------- */
  StringViewConstIt UtfGetPos() const { return svciIt; }
  /* -- Return current character ------------------------------------------- */
  template<typename CharType = char>
    requires StdIsIntegral<CharType>
  CharType UtfGetByte() const
    { return static_cast<CharType>(*UtfGetPos()); }
  /* -- Return start of string --------------------------------------------- */
  StringViewConstIt UtfGetStartPos() const { return cbegin(); }
  /* -- Pop position ------------------------------------------------------- */
  void UtfSetPos(const StringViewConstIt svciPos) { svciIt = svciPos; }
  /* -- Return current byte position --------------------------------------- */
  size_t UtfGetBytePos() const
    { return StdDistance<size_t>(UtfGetStartPos(), UtfGetPos()); }
  /* -- Convert to wide string --------------------------------------------- */
  StdWideString UtfWide()
  { // Convert all UTF-8 characters to wchar_t (don't use StdReserve<>)
    StdWideString wstrOut; wstrOut.reserve(size());
    while(const Codepoint coCode = UtfNext())
      wstrOut += static_cast<wchar_t>(coCode);
    wstrOut.shrink_to_fit();
    return wstrOut;
  }
  /* -- Return size of buffer (not characters!) ---------------------------- */
  size_t UtfSize() const { return this->size(); }
  /* -- Constructor that initialises a pointer ----------------------------- */
  template<typename StrType>
    requires StdIsPointer<StrType> || StdIsString<StrType>
  explicit UtfDecoder(const StrType &stSrc) :
    /* -- Initialisers ----------------------------------------------------- */
    StdStringView{ stSrc },
    svciIt{ UtfGetStartPos() }
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- Constructor that initialises a pointer and a size ------------------ */
  UtfDecoder(const char*const cpPtr, const size_t stSize) :
    /* -- Initialisers ----------------------------------------------------- */
    StdStringView{ cpPtr, stSize },
    svciIt{ UtfGetStartPos() }
    /* -- No code ---------------------------------------------------------- */
    {}
};/* -- Word wrap a utf string --------------------------------------------- */
static StrList UtfWordWrap(const StdStringView &strvText,
  const size_t stWidth, const size_t stIndent)
{ // Return empty array if width is invalid.
  if(!stWidth || stWidth <= stIndent) return {};
  // If string is empty, return at least one item because this could be
  // a blank line on purpose.
  if(strvText.empty()) return { StdString{ strvText } };
  // Create the line list and indent string
  StrList slLines;
  StdString strIndent;
  // Prepare UTF-8 string parser
  UtfDecoder udStr{ strvText };
  // Save starting and last whitespace found iterator
  StringViewConstIt svciStart{ udStr.UtfGetStartPos() },
                    svciSpace{ svciStart };
  // Helper function copy part of a string into the the word buffer
  const auto Snip = [&udStr, &slLines, &strIndent, &strvText]
      (const StringViewConstIt svciS, const StringViewConstIt svciE) {
    slLines.emplace_back(strIndent + StdString{ strvText.substr(
      static_cast<size_t>(svciS - udStr.UtfGetStartPos()),
      static_cast<size_t>(svciE - svciS)) });
  }; // Current column
  size_t stColumn = 0;
  // Until we're out of valid UTF8 characters
  while(const Codepoint coCode = udStr.UtfNext())
  { // Character found
    ++stColumn;
    // Is it a space character? Mark the cut count and goto next character
    if(coCode == ' ') { svciSpace = udStr.UtfGetPos(); continue; }
    // Is other character and we're not at the limit? Goto next character
    if(stColumn < stWidth) continue;
    // We already found where we can force a line break?
    // Add indentation if not first line then set current position to where
    // the last space was found
    if(svciSpace != svciStart)
    { // Copy up to the space we found traversing the string
      Snip(svciStart, svciSpace);
      // Reset position to where we found the last whitespace
      udStr.UtfSetPos(svciSpace);
      // Update start of next line
      svciStart = svciSpace = udStr.UtfGetPos();
    } // No space found on this line.?
    else
    { // The wrap position is at the start
      svciSpace = udStr.UtfGetPos();
      // Copy up to the last space we found
      Snip(svciStart, svciSpace);
      // Update start of next line
      svciStart = svciSpace;
    } // Set indentation for next line
    if(strIndent.empty()) strIndent = cCommon->CommonSpace();
    // Reset column to indent size
    stColumn = stIndent;
  } // If we are not at end of string? Add the remaining characters
  if(udStr.UtfGetPos() != svciStart) Snip(svciStart, udStr.UtfGetPos());
  // Return list
  return slLines;
}
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of module namespace
/* == EoF =========================================================== EoF == */
