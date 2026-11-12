/* == UTFDEC.HPP =========================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## Handles decoding of integers into UTF-8 byte sequences              ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IUtfDecoder {                // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace ICommon::P;            using namespace IEndian::P;
using namespace IStd::P;               using namespace IUtfCodepoint::P;
using namespace IUtfEncoder::P;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* -- UTF8 decoder helper class -------------------------------------------- */
class UtfDecoder final :               // UTF8 string decoder helper
  /* ----------------------------------------------------------------------- */
  private StdStringView                // Address and size of string
{ /* ----------------------------------------------------------------------- */
  StringViewConstIt svciIt;            // Current position
  /* -- Test a custom condition on each character -------------------------- */
  template<class OpFunc>bool UtfIsType()
  { // For each character, test if it is a control character
    while(const UtfCodepoint utcChar = UtfNext())
      if(OpFunc{}.T(utcChar)) return false;
    // Is a displayable character
    return true;
  }
  /* -- Iterator --------------------------------------------------- */ public:
  template<typename CharType = UtfCodepoint>CharType UtfNext()
  { // Walk through the string until we get to a null terminator
    for(UtfDecodeChar udcCp; UtfGetPos() != cend(); ++svciIt)
    { // Decode the specified character and ignore if no valid character
      udcCp.UtfDecode(UtfGetByte<unsigned char>());
      if(udcCp.UtfIsStateSet()) continue;
      // Move position onwards
      ++svciIt;
      // Return the codepoint
      return static_cast<CharType>(udcCp.UtfGetCodepoint());
    } // Invald string. Return null character
    return 0;
  }
  /* -- Check if is displayable character ---------------------------------- */
  bool UtfIsDisplayable()
  { // For each character, test if it is a control character
    struct Op{ Op() = default;
      static bool T(const UtfCodepoint utcChar) { return utcChar < 0x20; }};
    return UtfIsType<Op>();
  }
  /* -- Check if is ASCII compatible --------------------------------------- */
  bool UtfIsASCII()
  { // For each character, test if it is valid ASCII
    struct Op{ Op() = default;
      static bool T(const UtfCodepoint utcChar) { return utcChar > 0x7F; }};
    return UtfIsType<Op>();
  }
  /* -- Check if is extended ASCII compatible ------------------------------ */
  bool UtfIsExtASCII()
  { // For each character, test if it is valid extended ASCII
    struct Op{ Op() = default;
      static bool T(const UtfCodepoint utcChar) { return utcChar > 0xFF; }};
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
  void UtfIgnore(const UtfCodepoint utcChar)
  { // Save position and keep recording the position until error or finish
    StringViewConstIt svciSavedIt{ UtfGetPos() };
    while(UtfNext() == utcChar) svciSavedIt = UtfGetPos();
    // Return last position saved
    UtfSetPos(svciSavedIt);
  }
  /* -- Scan a value ------------------------------------------------------- */
  template<typename IntType = UtfCodepoint, size_t stMaximum = 8>
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
    StdIStringStream &isS = cCommon->i.StreamReset(strMatched);
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
  void UtfReset(StdStringView &&ssvNew) { swap(ssvNew); UtfReset(); }
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
    while(const UtfCodepoint utcChar = UtfNext())
      wstrOut += static_cast<wchar_t>(utcChar);
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
static StrList UtfWordWrap(const StdStringView &ssvText, const size_t stWidth,
  const size_t stIndent)
{ // Return empty array if width is invalid.
  if(!stWidth || stWidth <= stIndent) return {};
  // If string is empty, return at least one item because this could be
  // a blank line on purpose.
  if(ssvText.empty()) return { StdString{ ssvText } };
  // Create the line list and indent string
  StrList slLines;
  StdString strIndent;
  // Prepare UTF-8 string parser
  UtfDecoder udStr{ ssvText };
  // Save starting and last whitespace found iterator
  StringViewConstIt svciStart{ udStr.UtfGetStartPos() },
                    svciSpace{ svciStart };
  // Helper function copy part of a string into the the word buffer
  const auto Snip = [&udStr, &slLines, &strIndent, &ssvText]
      (const StringViewConstIt svciS, const StringViewConstIt svciE) {
    slLines.emplace_back(strIndent + StdString{ ssvText.substr(
      static_cast<size_t>(svciS - udStr.UtfGetStartPos()),
      static_cast<size_t>(svciE - svciS)) });
  }; // Current column
  size_t stColumn = 0;
  // Until we're out of valid UTF8 characters
  while(const UtfCodepoint utcChar = udStr.UtfNext())
  { // Character found
    ++stColumn;
    // Is it a space character? Mark the cut count and goto next character
    if(utcChar == ' ') { svciSpace = udStr.UtfGetPos(); continue; }
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
    if(strIndent.empty() && stIndent) strIndent = StdString(stIndent, ' ');
    // Reset column to indent size
    stColumn = stIndent;
  } // If we are not at end of string? Add the remaining characters
  if(udStr.UtfGetPos() != svciStart) Snip(svciStart, udStr.UtfGetPos());
  // Return list
  return slLines;
}
/* -- Shorten a string with an ellipsis if too long ------------------------ */
template<typename StrType>
  requires StdIsClass<StrType>
static StdString UtfShorten(const StrType &strStr, const size_t stLimit,
  const StdStringView &ssvEllipsis = cCommon->CommonEllipsis())
{ // Return current string if empty
  if(strStr.empty()) return {};
  // Create and reserve output string
  StdString strOut; strOut.reserve(strStr.length());
  // If no limit specified then return an ellipsis
  if(!stLimit)
  { // Suffix the ellipsis character
    Ellipsis: strOut.append(ssvEllipsis);
    // Finish string memory as it could be more than we created
    strOut.shrink_to_fit();
    // Done. Return the string
    return strOut;
  } // Number of utf characters processed
  size_t stChars = 0;
  // Wrap string in a UTF decoder.
  UtfDecoder udStr{ strStr };
  // Enumerate through the entire string
  while(const UtfCodepoint utcChar = udStr.UtfNext())
  { // Reencode the character and add it to the string
    UtfAppend(utcChar, strOut);
    // Keep adding UTF characters until we're done
    if(++stChars < stLimit) continue;
    // Return if we're at the end of the string
    if(!udStr.UtfNext()) break;
    // Erase all
    const size_t stPos = strOut.find_last_not_of(' ');
    if(stPos != StdNPos) strOut.erase(stPos + 1, StdNPos);
    // Append the ellipsis
    goto Ellipsis;
  } // Return string
  return strOut;
}
/* -- Find the beginning byte sequence from the end of a string ------------ */
static StringConstIt UtfFindByteSeqFromEnd(const StdString &strStr)
{ // Return failure if string is empty
  if(strStr.empty()) [[unlikely]] return strStr.cend();
  // Get end of string
  StringConstIt sciIt{ strStr.cend() };
  // Move backward to find the start of the last UTF-8 code point and decrement
  // iterator until we find a byte that is not a continuation byte.
  while(--sciIt != strStr.cbegin() && (*sciIt & 0xC0) == 0x80);
  // Return the position that we found
  return sciIt;
}
/* -- Pop UTF character from end of string --------------------------------- */
static bool UtfPopBack(StdString &strStr)
{ // Find byte sequence from end and return if not found
  StringConstIt sciIt{ UtfFindByteSeqFromEnd(strStr) };
  if(sciIt == strStr.cend()) return false;
  // Erase from that iterator to end and return success
  strStr.erase(sciIt, strStr.cend());
  return true;
}
/* -- Move UTF character from back of one string to the front of another --- */
static bool UtfMoveBackToFront(StdString &strSrc, StdString &strDst)
{ // Find byte sequence from end and return if not found
  StringConstIt sciIt{ UtfFindByteSeqFromEnd(strSrc) };
  if(sciIt == strSrc.cend()) return false;
  // Insert the char sequence from 'it' to the end into the front of strDst
  strDst.insert(strDst.cbegin(), sciIt, strSrc.cend());
  // Erase the moved characters from the source string and return success
  strSrc.erase(sciIt, strSrc.cend());
  return true;
}
/* -- Find the second UTF8 character from the start of a string ------------ */
static StringConstIt UtfFindByteSeqFromStart(const StdString &strStr)
{ // String is not empty?
  if(strStr.empty()) [[unlikely]] return strStr.cbegin();
  // Starting iterator, state and code
  StringConstIt sciIt{ strStr.cbegin() };
  UtfDecodeChar udcCodepoint;
  // Repeat...
  do { udcCodepoint.UtfDecode(static_cast<unsigned char>(*sciIt)); }
  // ...Until...
  while(++sciIt != strStr.cend() &&            // ...end of string and
        udcCodepoint.UtfIsStateSet() &&        // ...continuation bytes set and
        udcCodepoint.UtfIsErrorStateNotSet()); // ...error not set
  // Return position or beginning if failed
  return udcCodepoint.UtfIsErrorStateNotSet() ? sciIt : strStr.cbegin();
}
/* -- Pop UTF character from start of string-------------------------------- */
static bool UtfPopFront(StdString &strStr)
{ // Starting iterator, state and code
  StringConstIt sciIt{ UtfFindByteSeqFromStart(strStr) };
  if(sciIt == strStr.cbegin()) return false;
  // Remove the raw bytes and return success
  strStr.erase(strStr.cbegin(), sciIt);
  return true;
}
/* -- Move UTF character from front of one string to the back of another --- */
static bool UtfMoveFrontToBack(StdString &strSrc, StdString &strDst)
{ // Starting iterator, state and code
  StringConstIt sciIt{ UtfFindByteSeqFromStart(strSrc) };
  if(sciIt == strSrc.cbegin()) return false;
  // Append the sequence from the front of strSrc to the back of strDst
  strDst.insert(strDst.cend(), strSrc.cbegin(), sciIt);
  // Erase the moved character sequence from the front of strSrc and return
  strSrc.erase(strSrc.cbegin(), sciIt);
  return true;
}
/* -- Convert a unicode or ansi string to UTF8 ----------------------------- */
template<typename StrType>
  requires StdIsClass<StrType>
static StdString UtfFromWide(const StrType &strStr)
{ // Enumerate string and convert all characters to UTF8
  StdString strOut;
  for(auto aChar : strStr) UtfAppend(static_cast<UtfCodepoint>(aChar), strOut);
  // Return string
  return strOut;
}
/* -- Convert an actual integer containing UTF8 byte sequence to char ------ */
template<typename IntType = unsigned>
  requires StdIsIntegral<IntType> && (sizeof(IntType) >= sizeof(uint32_t))
static StdString UtfDecodeNum(IntType itVal)
{ // Unset the un-needed upper 8-bits. This will act as the nullptr character.
  if constexpr(sizeof(IntType) > sizeof(uint32_t)) itVal &= 0xFFFFFFFF;
  // If we have any of the lower 24-bits set? Keep shifting the bits until the
  // bits are on the opposite end so we can cast it to a readable char pointer.
  // We're not changing endian, just reversing each of the eight bits.
  if(itVal) for(itVal = EndianSwap32(itVal); !(itVal & 0xFF); itVal >>= 8);
  // Return the shifted value casted to a char pointer address.
  return { reinterpret_cast<const char*>(&itVal) };
}
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
