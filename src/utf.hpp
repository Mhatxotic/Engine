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
/* ------------------------------------------------------------------------- */
namespace IUtf {                       // Start of module namespace
/* -- Remove const from a pointer ------------------------------------------ */
template<typename TypeTo, typename TypeFrom, typename TypeNonConst =
  typename remove_const<typename remove_pointer<TypeFrom>::type>::type*>
static TypeTo UtfToNonConstCast(TypeFrom tfV)
{ // Check that type has a pointer
  static_assert(is_pointer_v<TypeFrom>, "Input type must have pointer!");
  // Do the const and reinterpret cast!
  return reinterpret_cast<TypeTo>(const_cast<TypeNonConst>(tfV));
}
/* -- Check if C-string is nullptr or blank -------------------------------- */
template<typename PtrType>
  static bool UtfIsCStringValid(const PtrType*const ptpStr)
    { return ptpStr && *ptpStr; }
template<typename PtrType>
  static bool UtfIsCStringNotValid(const PtrType*const ptpStr)
    { return !UtfIsCStringValid<PtrType>(ptpStr); }
/* -- Lookup table for decoder --------------------------------------------- **
** ######################################################################### **
** ## The first part of the table maps bytes to character classes to      ## **
** ## reduce the size of the transition table and create bitmasks.        ## **
** ######################################################################### */
static const array<const unsigned int,256> uiaDecodeMap{
   0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
   0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
   0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
   0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
   1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,  9,9,9,9,9,9,9,9, 9,9,9,9,9,9,9,9,
   7,7,7,7,7,7,7,7, 7,7,7,7,7,7,7,7,  7,7,7,7,7,7,7,7, 7,7,7,7,7,7,7,7,
   8,8,2,2,2,2,2,2, 2,2,2,2,2,2,2,2,  2,2,2,2,2,2,2,2, 2,2,2,2,2,2,2,2,
  10,3,3,3,3,3,3,3, 3,3,3,3,3,4,3,3, 11,6,6,6,5,8,8,8, 8,8,8,8,8,8,8,8
};
/* ######################################################################### **
** ## The second part is a transition table that maps a combination of a  ## **
** ## state of the automaton and a character class to a state.            ## **
** ######################################################################### */
static const array<const unsigned int,108> uiaDecodeTransition{
   0,12,24,36,60,96,84,12, 12,12,48,72,12,12,12,12, 12,12,12,12,12,12,12,12,
  12, 0,12,12,12,12,12, 0, 12, 0,12,12,12,24,12,12, 12,12,12,24,12,24,12,12,
  12,12,12,12,12,12,12,24, 12,12,12,12,12,24,12,12, 12,12,12,12,12,24,12,12,
  12,12,12,12,12,12,12,36, 12,36,12,12,12,36,12,12, 12,12,12,36,12,36,12,12,
  12,36,12,12,12,12,12,12, 12,12,12,12
};
/* -- Structure for utf size and character code ---------------------------- */
struct UtfEncoderEx { const size_t l;
  const union { const uint8_t u8[5]; const char c[5]; } u; };
/* -- Encode specified code and return string and size --------------------- */
static const UtfEncoderEx UtfEncodeEx(const unsigned int uiUtfCode)
{ // Normal ASCII character?
  if(uiUtfCode < 0x80) return
    { 1, {{ static_cast<uint8_t>(((uiUtfCode>>0)&0x7F)), 0, 0, 0, 0 }}};
  // ASCII/Unicode character between 128-2047?
  else if(uiUtfCode < 0x800) return
    { 2, {{ static_cast<uint8_t>(((uiUtfCode>>6)&0x1F)|0xC0),
            static_cast<uint8_t>(((uiUtfCode>>0)&0x3F)|0x80), 0, 0, 0 }}};
  // Unicode character between 2048-65535?
  else if(uiUtfCode < 0x10000) return
    { 3, {{ static_cast<uint8_t>(((uiUtfCode>>12)&0x0F)|0xE0),
            static_cast<uint8_t>(((uiUtfCode>> 6)&0x3F)|0x80),
            static_cast<uint8_t>(((uiUtfCode>> 0)&0x3F)|0x80), 0, 0 }}};
  // Unicode character between 65536-1114111?
  else if(uiUtfCode < 0x110000) return
    { 4, {{ static_cast<uint8_t>(((uiUtfCode>>18)&0x07)|0xF0),
            static_cast<uint8_t>(((uiUtfCode>>12)&0x3F)|0x80),
            static_cast<uint8_t>(((uiUtfCode>> 6)&0x3F)|0x80),
            static_cast<uint8_t>(((uiUtfCode>> 0)&0x3F)|0x80), 0 }}};
  // This shouldn't happen, but just incase
  return { 0, {{ 0, 0, 0, 0, 0 }} };
}
/* -- Encode specified code and append it to the specified string ---------- */
static void UtfAppend(const unsigned int uiChar, string &strDest)
{ // Encoded UTF8
  const UtfEncoderEx utfCode{ UtfEncodeEx(uiChar) };
  // Append to string
  strDest.append(utfCode.u.c, utfCode.l);
}
/* ------------------------------------------------------------------------- */
static void UtfDecode(unsigned int &uiState, unsigned int &uiCode,
  const unsigned char ucByte)
{ // Get type from lookup table
  const unsigned int uiType = uiaDecodeMap[ucByte];
  // Calculate the utf code
  uiCode = uiState ? (ucByte & 0x3fu) | (uiCode << 6) :
                     (0xff >> uiType) & ucByte;
  // Update the new state of the code
  uiState = uiaDecodeTransition[uiState + uiType];
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
/* -- Pop UTF character from start of string-------------------------------- */
static bool UtfPopFront(string &strStr)
{ // String is not empty?
  if(!strStr.empty())
  { // Get start and end of buffer in string
    const unsigned char*const cpB =
      UtfToNonConstCast<unsigned char*>(strStr.data()),
        *const cpE = cpB + strStr.size();
    // Set start of string as enumerator
    unsigned char *cpI = const_cast<unsigned char*>(cpB);
    // Utf state and return code
    unsigned int uiState = 0;
    // Walk through the string until we get to a null terminator
    do uiState = uiaDecodeTransition[uiState + uiaDecodeMap[*cpI]];
      while(++cpI < cpE && uiState);
    // If pointer moved?
    if(cpI >= cpB)
    { // Erase the specified characters
      strStr.erase(0, static_cast<size_t>(cpI - cpB));
      // Success
      return true;
    } // Pointer did not move
  } // Return failure
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
  { // Get start and end of buffer
    const unsigned char*const cpB =
      UtfToNonConstCast<unsigned char*>(strSrc.data()),
        *const cpE = cpB + strSrc.size();
    // Set start of string as enumerator
    unsigned char *cpI = const_cast<unsigned char*>(cpB);
    // Utf state and return code
    unsigned int uiState = 0, uiCode = 0;
    // Walk through the string until we get to a null terminator
    do UtfDecode(uiState, uiCode, *cpI);
      while(++cpI < cpE && uiState);
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
    const UtfEncoderEx
      utfCode{ UtfEncodeEx(static_cast<unsigned int>(*ctPtr)) };
    // Append to string
    strOut.append(utfCode.u.c, utfCode.l);
    // Until end of string
  } while(*(++ctPtr));
  // Return string
  return strOut;
}
/* -- UTF8 decoder helper class -------------------------------------------- */
class UtfDecoder                        // UTF8 string decoder helper
{ /* ----------------------------------------------------------------------- */
  const unsigned char *ucpStr, *ucpPtr; // String and pointer to that string
  /* -- Iterator --------------------------------------------------- */ public:
  template<typename CharType=unsigned int>CharType Next(void)
  { // Walk through the string until we get to a null terminator
    for(unsigned int uiState = 0, uiCode = 0; *ucpPtr > 0; ++ucpPtr)
    { // Decode the specified character
      UtfDecode(uiState, uiCode, *ucpPtr);
      // Ignore if we haven't got a valid UTF8 character yet.
      if(uiState) continue;
      // Move position onwards
      ++ucpPtr;
      // Return the UTF8 character as requested type (limit to 0-2097151).
      // UTF8 can only address 1,112,064 (0x10F800) total characters
      return static_cast<CharType>(uiCode & 0x1fffff);
    } // Invald string. Return null character
    return 0;
  }
  /* -- Check if is displayable character ---------------------------------- */
  bool IsDisplayable(void)
  { // For each character, test if it is a control character
    while(const unsigned int uiChar = Next())
      if(static_cast<wchar_t>(uiChar) < 0x20) return false;
    // Is a displayable character
    return true;
  }
  /* -- Check if is ASCII compatible --------------------------------------- */
  bool IsASCII(void)
  { // For each character, test if it is valid ASCII
    while(const unsigned int uiChar = Next())
      if(static_cast<wchar_t>(uiChar) > 0x7F) return false;
    // Is valid ASCII
    return true;
  }
  /* -- Check if is extended ASCII compatible ------------------------------ */
  bool IsExtASCII(void)
  { // For each character, test if it is valid extended ASCII
    while(const unsigned int uiChar = Next())
      if(static_cast<wchar_t>(uiChar) > 0xFF) return false;
    // Is valid extended ASCII
    return true;
  }
  /* -- Length ------------------------------------------------------------- */
  size_t Length(void)
  { // Length of string
    size_t stLength = 0;
    // Walk through the string until we get to a null terminator
    while(Next()) ++stLength;
    // Return length
    return stLength;
  }
  /* -- Skip number of utf8 characters ------------------------------------- */
  void Skip(size_t stAmount) { while(stAmount-- && Next()); }
  /* -- Skip number of C characters ---------------------------------------- */
  void SkipChars(size_t stAmount) { while(stAmount-- && *ucpPtr++); }
  /* -- Skip characters and return last position (Char::* needs this) ------ */
  void Ignore(const unsigned int uiChar)
  { // Saved position
    const unsigned char *ucpPtrSaved;
    // While the chars are matched
    do { ucpPtrSaved = ucpPtr; } while(Next() == uiChar);
    // Return last position saved
    ucpPtr = ucpPtrSaved;
  }
  /* -- Skip a value ------------------------------------------------------- */
  template<typename IntType=unsigned int,size_t stMaximum=8>
    size_t SkipValue(void)
  { // Ignore if at the end of the string or it is empty
    if(Finished()) return false;
    // Save position and expected end position. The end position CAN be OOB
    // but is safe as all UTF strings are expected to be null terminated.
    const unsigned char*const ucpSaved = ucpPtr,
                       *const ucpEnd = ucpSaved + stMaximum;
    // Add characters as long as they are valid hexadecimal characters
    while(isxdigit(*(ucpPtr++)) && ucpPtr < ucpEnd);
    // Return number of bytes read
    return static_cast<size_t>(ucpPtr - ucpSaved);
  }
  /* -- Scan a value ------------------------------------------------------- */
  template<typename IntType=unsigned int,size_t stMaximum=8>
    size_t ScanValue(IntType &itOut)
  { // Ignore if at the end of the string or it is empty
    if(Finished()) return 0;
    // Capture up to eight characters
    string strMatched; strMatched.reserve(stMaximum);
    // Add characters as long as they are valid hexadecimal characters and the
    // matched string has not reached eight characters. Anything that could be
    // unicode character should auto break anyway so this should be safe.
    while(isxdigit(*ucpPtr) && strMatched.length() < stMaximum)
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
  const string Slice(const unsigned char*const ucpPos) const
    { return { reinterpret_cast<const char*>(ucpPos),
        static_cast<size_t>(ucpPtr - ucpPos) }; }
  /* -- Return if string is valid ------------------------------------------ */
  bool Valid(void) const { return !!ucpStr; }
  /* -- Return if pointer is at the end ------------------------------------ */
  bool Finished(void) const { return !*ucpPtr; }
  /* -- Reset pointer ------------------------------------------------------ */
  void Reset(void) { SetCPtr(const_cast<unsigned char*>(ucpStr)); }
  /* -- Reset pointer with new strings ------------------------------------- */
  void Reset(const unsigned char*const ucpNew)
    { ucpPtr = ucpStr = ucpNew; }
  void Reset(const char*const cpNew)
    { ucpPtr = ucpStr = reinterpret_cast<const unsigned char*>(cpNew); }
  /* -- Return raw string -------------------------------------------------- */
  const unsigned char *GetCString(void) const { return ucpStr; }
  /* -- Return raw string -------------------------------------------------- */
  const unsigned char *GetCPtr(void) const { return ucpPtr; }
  /* -- Pop position ------------------------------------------------------- */
  void SetCPtr(const unsigned char*const ucpPos) { ucpPtr = ucpPos; }
  /* -- Convert to wide string --------------------------------------------- */
  const wstring Wide(void)
  { // Output string
    wstring wstrOut;
    // Add character to string
    while(const unsigned int uiChar = Next())
      wstrOut += static_cast<wchar_t>(uiChar);
    // Return string
    return wstrOut;
  }
  /* -- Constructors that copy the address of the allocated text ----------- */
  explicit UtfDecoder(const char*const ucpSrc) :
    /* -- Initialisers ----------------------------------------------------- */
    ucpStr(reinterpret_cast            // Set start of string
      <const unsigned char*>(ucpSrc)),
    ucpPtr(ucpStr)                     // Copy for current position
    /* -- No code ---------------------------------------------------------- */
    { }
  explicit UtfDecoder(const unsigned char*const ucpSrc) :
    /* -- Initialisers ----------------------------------------------------- */
    ucpStr(ucpSrc),                    // Set start of string
    ucpPtr(ucpStr)                     // Copy for current position
    /* -- No codes --------------------------------------------------------- */
    { }
  explicit UtfDecoder(const string_view &strvStr) :
    /* -- Initialisers ----------------------------------------------------- */
    ucpStr(reinterpret_cast            // Set start of string
      <const unsigned char*>
        (strvStr.data())),
    ucpPtr(ucpStr)                     // Copy for current position
    /* -- No code ---------------------------------------------------------- */
    { }
  explicit UtfDecoder(const string &strStr) :
    /* -- Initialisers ----------------------------------------------------- */
    ucpStr(reinterpret_cast            // Set start of string
      <const unsigned char*>
        (strStr.c_str())),
    ucpPtr(ucpStr)                     // Copy for current position
    /* -- No code ---------------------------------------------------------- */
    { }
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
  UtfDecoder utfString{ strText };
  // Save position
  const unsigned char*const ucpString = utfString.GetCString();
  const unsigned char *ucpStart = utfString.GetCPtr(),
                      *ucpSpace = ucpStart;
  // Current column
  size_t stColumn = 0;
  // Until we're out of valid UTF8 characters
  while(const unsigned int uiChar = utfString.Next())
  { // Character found
    ++stColumn;
    // Is it a space character? Mark the cut count and goto next character
    if(uiChar == ' ') { ucpSpace = utfString.GetCPtr(); continue; }
    // Is other character and we're not at the limit? Goto next character
    if(stColumn < stWidth) continue;
    // We already found where we can force a line break?
    // Add indentation if not first line then set current position to where
    // the last space was found
    if(ucpSpace != ucpStart)
    { // Copy up to the space we found traversing the string
      svLines.emplace_back(strIndent + strText.substr(
        static_cast<size_t>(ucpStart - ucpString),
        static_cast<size_t>(ucpSpace - ucpStart)));
        utfString.SetCPtr(ucpSpace);
      // Update start of next line
      ucpStart = ucpSpace = utfString.GetCPtr();
    } // No space found on this line.?
    else
    { // The wrap position is at the start
      ucpSpace = utfString.GetCPtr();
      // Copy up to the last space we found
      svLines.emplace_back(strIndent + strText.substr(
        static_cast<size_t>(ucpStart - ucpString),
        static_cast<size_t>(ucpSpace - ucpStart)));
      // Update start of next line
      ucpStart = ucpSpace;
    } // Set indentation for next line
    if(strIndent.empty()) strIndent = string(stIndent, ' ');
    // Reset column to indent size
    stColumn = stIndent;
  } // If we are not at end of string? Add the remaining characters
  if(utfString.GetCPtr() != ucpStart)
    svLines.emplace_back(strIndent + strText.substr(
      static_cast<size_t>(ucpStart - ucpString),
      static_cast<size_t>(utfString.GetCPtr() - ucpStart)));
  // Return list
  return svLines;
}
/* ------------------------------------------------------------------------- */
}                                      // End of module namespace
/* == EoF =========================================================== EoF == */
