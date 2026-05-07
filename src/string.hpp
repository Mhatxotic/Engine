/* == STRING.HPP =========================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## This is the module that defines miscellaneous String related        ## **
** ## utility functions. All functions will be prefixed with 'Str'.       ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IString {                    // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace ICommon::P;            using namespace IStd::P;
using namespace IUtf::P;
/* -- Process string format/append value into output string stream --------- */
template<typename AnyType>
  static void StrFormatValue(StdOStringStream &osS, AnyType &&atVal)
{ // If is an exception object? Push the string of it
  if constexpr(StdIsSame<StdDecay<AnyType>, StdException>)
    osS << atVal.what();
  // Let ostringstream handle the value
  else osS << atVal;
}
/* -- Append final parameter to output string stream ----------------------- */
static void StrAppendParam(StdOStringStream&) {}
/* -- Append a parameter to output string stream --------------------------- */
template<typename AnyType, typename ...VarArgs>
  static void StrAppendParam(StdOStringStream &osS, AnyType &&atVal,
    VarArgs &&...vaArgs)
{ // Push the specified value and process the next argument
  StrFormatValue(osS, StdForward<AnyType>(atVal));
  StrAppendParam(osS, StdForward<VarArgs>(vaArgs)...);
}
/* -- Format final parameter to output string stream ----------------------- */
static void StrFormatParam(StdOStringStream &osS, const StdStringView &strvPos,
  const size_t stPos) { osS << strvPos.substr(stPos); }
/* -- Check for next limiter ----------------------------------------------- */
static size_t StrFormatGetNextDelimiter(StdOStringStream &osS,
  const StdStringView &strvPos, size_t stPos)
{ // Find mark that will be replaced by this param and if no more tokens?
  const size_t stNewPos = strvPos.find('$', stPos);
  if(stNewPos == StdNPos)
  { // Return the rest of the string and return finished to the caller
    StrFormatParam(osS, strvPos, stPos);
    return StdNPos;
  } // How far did we find the new position
  switch(const size_t sitNum = static_cast<size_t>(stNewPos - stPos))
  { // One character? Just copy one character and move ahead two to skip over
    // the '$' we just processed.
    case 1: osS << strvPos[stPos];
            stPos += 2; break;
    // More than one character? Copy characters and stride over the '$' we
    // just processed. Better than storing single characters.
    default: osS << strvPos.substr(stPos, sitNum);
             stPos += sitNum + 1;
             break;
    // Did not move? This can happen at the start of the string. Just move
    // over the first '$'.
    case 0: ++stPos; break;
  } // Return new position
  return stPos;
}
/* -- Format any parameter to output string stream ------------------------- */
template<typename AnyType, typename ...VarArgs>
  static void StrFormatParam(StdOStringStream &osS,
    const StdStringView &strvPos, size_t stPos, AnyType &&atVal,
    VarArgs &&...vaArgs)
{ // Find mark that will be replaced by this param and return if finished
  const size_t stNewPos = StrFormatGetNextDelimiter(osS, strvPos, stPos);
  if(stNewPos == StdNPos) return;
  // Push the value we are supposed to replace the matched '$' with.
  StrFormatValue(osS, StdForward<AnyType>(atVal));
  // Process more parameters if we can.
  StrFormatParam(osS, strvPos, stNewPos, StdForward<VarArgs>(vaArgs)...);
}
/* -- Public functions ----------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* -- Append main function ------------------------------------------------- */
template<typename ...VarArgs>
  requires (sizeof...(VarArgs) > 0)
static StdString StrAppend(VarArgs &&...vaArgs)
{ // Create stream to write to, build it and return it
  StdOStringStream osS;
  StrAppendParam(osS, StdForward<VarArgs>(vaArgs)...);
  return osS.str();
}
/* -- Append with formatted numbers ---------------------------------------- */
template<typename ...VarArgs>
  requires (sizeof...(VarArgs) > 0)
static StdString StrAppendImbue(VarArgs &&...vaArgs)
{ // Stream to write to
  StdOStringStream osS;
  // Imbue current locale
  osS.imbue(cCommon->CommonLocale());
  // Build string
  StrAppendParam(osS, StdForward<VarArgs>(vaArgs)...);
  // Return appended string
  return osS.str();
}
/* -- Prepare message from a c-string pointer ------------------------------ */
template<typename StrType, typename ...VarArgs>
  requires (sizeof...(VarArgs) > 0)
static StdString StrFormat(StrType &&strFormat, VarArgs &&...vaArgs)
{ // Get type without reference and a decayed type
  using StrTypeNoRef = StdRemoveReference<StrType>;
  using StrTypeDecayed = StdDecay<StrType>;
  // If type is an C-style text array?
  if constexpr(StdIsArray<StrTypeNoRef>)
  { // Get the size of the text array and throw error if no size
    constexpr size_t stN = StdExtent<StrTypeNoRef>;
    static_assert(stN > 0);
    // Call itself again with the correct format
    return StrFormat(StdStringView{ strFormat, stN - 1 },
      StdForward<VarArgs>(vaArgs)...);
  } // If type is a null-terminated pointer to a C-String?
  else if constexpr(StdIsPointer<StrTypeDecayed>)
  { // We don't know the size so StdStringView will have to check for us
    return StrFormat(StdStringView{ strFormat },
      StdForward<VarArgs>(vaArgs)...);
  } // Correct type is a StdString?
  else if constexpr(StdIsString<StrTypeDecayed>)
  { // Create stringstream to write to, format the text and return it
    if(strFormat.empty()) return {};
    StdOStringStream osS;
    StrFormatParam(osS, StdForward<StrType>(strFormat), 0,
      StdForward<VarArgs>(vaArgs)...);
    return osS.str();
  } // Anything else is invalid
  else static_assert(sizeof(StrType) == 0, "Invalid format type!");
}
/* -- Format a number ------------------------------------------------------ */
template<typename IntType>
  requires StdIsArithmatic<IntType>
static StdString StrReadableFromNum(const IntType itVal, const int iPrec = 0)
  { return StrAppendImbue(StdIOSFixed, StdIOSSetPrecision(iPrec), itVal); }
/* -- Trim specified characters from end of string ------------------------- */
static StdString StrTrimSuffix(const StdStringView &strvStr, const char cChar)
{ // Return empty string if source string is empty or calculate ending
  // misoccurance of character then copy and return the string
  return StdString{ strvStr.empty() ?
    strvStr : strvStr.substr(0, strvStr.find_last_not_of(cChar) + 1) };
}
/* -- Trim specified characters from end of string ------------------------- */
template<class StrType>
  requires StdIsString<StrType>
static StrType &StrTrimSuffixRef[[maybe_unused]]
  (StrType &strStr, const char cChar)
{ // Locate the first non-occurrence of the char from end and erase them
  const size_t stPos = strStr.find_last_not_of(cChar);
  if(stPos != StdNPos) strStr.erase(stPos + 1);
  return strStr;
}
/* -- Trim specified characters from string -------------------------------- */
template<class StrType>
  requires StdIsString<StrType>
static StrType &StrTrimRef(StrType &strStr, const char cChar)
{ // Return empty string if source string is empty
  if(!strStr.empty())
  { // Calculate starting misoccurance of char. Return original if not found
    const size_t stBegin = strStr.find_first_not_of(cChar);
    if(stBegin != StdNPos) strStr.erase(0, stBegin);
    const size_t stEnd = strStr.find_last_not_of(cChar);
    if(stEnd != StdNPos) strStr.resize(stEnd + 1);
    strStr.shrink_to_fit();
  } // Return original string
  return strStr;
}
/* -- Trim specified characters from string -------------------------------- */
template<class StrType>
  requires StdIsString<StrType>
static StdDecay<StrType> StrTrim(StrType &&strStr, const char cChar)
{ // Return empty string if source string is empty
  if(!strStr.empty())
  { // Calculate starting misoccurance of char. Return original if not found
    const size_t stBegin = strStr.find_first_not_of(cChar);
    if(stBegin != StdNPos)
      // Calculate ending misoccurance of char then copy and return the string
      return strStr.substr(stBegin,
        strStr.find_last_not_of(cChar) - stBegin + 1);
  } // Return original string
  return strStr;
}
/* -- Convert integer to string with padding and precision ----------------- */
template<typename IntType>
  requires StdIsArithmatic<IntType> || StdIsEnum<IntType>
static StdString StrFromNum(const IntType itV, const int iW = 0,
  const int iPrecision = StdLimits<IntType>::digits10)
{ return StrAppend(StdIOSSetWidth(iW), StdIOSFixed,
    StdIOSSetPrecision(iPrecision), itV); }
/* -- Quickly convert numbered string to integer --------------------------- */
template<typename IntType>
  static IntType PtrToNum(const char*const cpStr, const ssize_t sstSize)
{ // Put value into input string stream
  StdStringBuf sbBuffer;
  sbBuffer.sputn(cpStr, sstSize);
  StdIStream isS{ &sbBuffer };
  // Is an enum? We have to treat it differently
  if constexpr(StdIsEnum<IntType>)
  { // Underlying value of the enum type to store into and convert it back
    StdUnderlyingType<IntType> utN;
    isS >> utN;
    return static_cast<IntType>(utN);
  } // Value is a normal integral? Treat it nromally
  else { IntType iitN; isS >> iitN; return iitN; }
}
/* -- Quickly convert numbered string to integer (thin template) ----------- */
template<typename IntType = int64_t, class StrType>
  requires (StdIsArithmatic<IntType> || StdIsEnum<IntType>) &&
            StdIsStrOrCStr<StrType>
static IntType StrToNum(StrType &&strStr)
  { return StdNormaliseString<StrType>(StdForward<StrType>(strStr),
      [](auto &aStr)->IntType{
        return PtrToNum<IntType>(aStr.data(),
          static_cast<ssize_t>(aStr.size())); }); }
/* -- Quickly convert hex string to integer -------------------------------- */
template<typename IntType>
  static IntType PtrHexToInt(const char*const cpStr, const ssize_t sstSize)
{ // Put value into input string stream
  StdStringBuf sbBuffer;
  sbBuffer.sputn(cpStr, sstSize);
  StdIStream isS{ &sbBuffer };
  // Push value into integer and return result
  IntType iitN; isS >> StdIOSHex >> iitN;
  return iitN;
}
/* -- Quickly convert hex string to integer (thin template) ---------------- */
template<typename IntType = int64_t, typename StrType>
  requires StdIsArithmatic<IntType> && StdIsString<StrType>
static IntType StrHexToInt(StrType &&strStr)
  { return StdNormaliseString<StrType>(StdForward<StrType>(strStr),
      [](auto &&aStr)->IntType{
        return PtrHexToInt<IntType>(aStr.data(),
          static_cast<ssize_t>(aStr.size())); }); }
/* -- Convert hex to string with zero padding (lowercase) ------------------ */
template<typename IntType>
  requires StdIsArithmatic<IntType> ||
           StdIsArithmatic<StdUnderlyingType<IntType>>
static StdString StrHexFromInt(const IntType itVal, const int iPrec = 0)
  { return StrAppend(StdIOSSetFill('0'), StdIOSHex, StdIOSSetWidth(iPrec),
      itVal); }
/* -- Convert hex to string with zero padding (uppercase) ------------------ */
template<typename IntType>
  requires StdIsArithmatic<IntType>
static StdString StrHexUFromInt(const IntType itVal, const int iPrec = 0)
  { return StrAppend(StdIOSSetFill('0'), StdIOSHex, StdIOSSetWidth(iPrec),
      StdIOSUpCase, itVal); }
/* -- Return if specified string has numbers ------------------------------- */
template<class StrType>
  requires StdIsString<StrType>
static bool StrIsAlpha(StrType &&strStr)
  { return StdAllOf(par_unseq, strStr.cbegin(), strStr.cend(),
      [](const char cValue) { return StdIsAlpha(cValue); }); }
/* -- Return if specified string has numbers ------------------------------- */
template<class StrType>
  requires StdIsString<StrType>
static bool StrIsAlphaNum(StrType &&strStr)
  { return StdAllOf(par_unseq, strStr.cbegin(), strStr.cend(),
      [](const char cValue)
        { return StdIsAlnum(static_cast<int>(cValue)); }); }
/* -- Return if specified string is a valid integer ------------------------ */
template<typename IntType>
  static bool PtrIsInt(const char*const cpStr, const ssize_t sstSize)
{ // Get string stream
  StdStringBuf sbBuffer;
  sbBuffer.sputn(cpStr, sstSize);
  StdIStream isS{ &sbBuffer };
  // Test value with input stream and return if failed
  IntType itV; isS >> StdIOSNoSkipWS >> itV;
  return isS.eof() && !isS.fail();
}
/* -- Return if specified string is a valid integer (thin template) -------- */
template<typename IntType = int64_t, class StrType>
  requires (StdIsArithmatic<IntType> || StdIsEnum<IntType>) &&
    StdIsStrOrCStr<StrType>
static bool StrIsInt(StrType &&strStr)
  { return StdNormaliseString<StrType>(StdForward<StrType>(strStr),
      [](auto &&aStr)->bool{
        return PtrIsInt<IntType>(aStr.data(),
          static_cast<ssize_t>(aStr.size())); }); }
/* -- Return if specified string is a valid float -------------------------- */
template<typename StrType>
  requires StdIsStrOrCStr<StrType>
static bool StrIsFloat(StrType &&strStr)
  { return StrIsInt<double>(StdForward<StrType>(strStr)); }
/* -- Return true if string is a value number to the power of 2 ------------ */
template<typename StrType>
  requires StdIsStrOrCStr<StrType>
static bool StrIsNumPOW2(StrType &&strStr)
  { return !strStr.empty() &&
     StdIntIsPOW2(StdAbsolute(StrToNum(StdForward<StrType>(strStr)))); }
/* -- Convert error number to string --------------------------------------- */
static StdString StrFromErrNo(const int iErrNo = errno)
{ // Buffer to store error message into
  StdResized<StdString> strErr{ 128 };
  // Windows?
#if defined(WINDOWS)
  // 'https://msdn.microsoft.com/en-us/library/51sah927.aspx' says:
  // "Your string message can be, at most, 94 characters long."
  if(strerror_s(const_cast<char*>(strErr.data()), strErr.capacity(), iErrNo))
    strErr.assign(StrAppend("Error ", iErrNo));
  // Targeting MacOS?
#elif defined(MACOS)
  // Grab the error result and if failed? Just put in the error number continue
  if(strerror_r(iErrNo, const_cast<char*>(strErr.data()), strErr.capacity()))
    strErr.assign(StrAppend("Error ", iErrNo));
  // Linux?
#elif defined(LINUX)
  // Grab the error result and if failed? Set a error and continue
  const char*const cpResult =
    strerror_r(iErrNo, const_cast<char*>(strErr.data()), strErr.capacity());
  if(!cpResult) strErr.assign(StrAppend("Error ", iErrNo));
  // We got a message but if was not put in our buffer just return as is
  else if(cpResult != strErr.data()) return cpResult;
#endif
  // Resize and compact the buffer
  strErr.resize(strlen(strErr.data()));
  // Have to do this because the string is still actually 94 bytes long
  return strErr;
}
/* -- Convert writable reference string to uppercase ----------------------- */
static StdString &StrToUpCaseRef(StdString &strStr)
{ // If string is not empty
  if(!strStr.empty())
    StdTransform(par_unseq, strStr.begin(), strStr.end(), strStr.begin(),
      [](unsigned char ucChar) { return StdToUpper(ucChar); });
  // Return output
  return strStr;
}
/* -- Convert string to upper case ----------------------------------------- */
template<typename StrType>
  requires StdIsString<StrType>
static StdString StrToUpCase[[maybe_unused]](StrType &&strSrc)
{ // String empty? Return a blank one
  if(strSrc.empty()) return {};
  // Create memory for destination string and copy the string over
  StdReserved<StdString> strDst{ strSrc.size() };
  StdTransformNXP(strSrc.begin(), strSrc.end(), StdBackInserter(strDst),
    [](unsigned char ucChar) { return StdToUpper(ucChar); });
  // Return result
  return strDst;
}
/* -- Convert writable referenced string to lowercase ---------------------- */
static StdString &StrToLowCaseRef(StdString &strStr)
{ // If string is not empty
  if(!strStr.empty())
    StdTransform(par_unseq, strStr.begin(), strStr.end(), strStr.begin(),
      [](unsigned char ucChar) { return StdToLower(ucChar); });
  // Return output
  return strStr;
}
/* -- Convert string to lower case ----------------------------------------- */
template<typename StrType>
  requires StdIsString<StrType>
static StdString StrToLowCase(StrType &&strSrc)
{ // String empty? Return a blank one
  if(strSrc.empty()) return {};
  // Prepare destination string and run a transform to lowercase each char
  StdReserved<StdString> strDst{ strSrc.size() };
  StdTransformNXP(strSrc.begin(), strSrc.end(), StdBackInserter(strDst),
    [](unsigned char ucChar) { return StdToLower(ucChar); });
  // Return result
  return strDst;
}
/* -- Basic multiple replace of text in string ----------------------------- */
template<class ListType = StrPairList, class StrType>
  requires StdIsString<StrType>
static StdString &StrReplaceExRef(StrType &&strDest, const ListType &ltList)
{ // Return original string if empty
  if(strDest.empty() || ltList.empty()) return strDest;
  // Current index to scan
  size_t stPos = 0;
  // Repeat...
  do
  { // Container type
    using ListTypeItem = typename ListType::value_type;
    // Enumerate each occurence to find...
    for(const ListTypeItem &ltiItem : ltList)
    { // Get string to find
      const StdTupleElement<0, ListTypeItem> &strWhat = ltiItem.first;
      // Last cut position and current character index
      if(strDest.size() < stPos + strWhat.size() ||
        strDest.compare(stPos, strWhat.size(), strWhat)) continue;
      // Get string to replace with and replace the occurence
      const StdTupleElement<1, ListTypeItem> &strWith = ltiItem.second;
      strDest.replace(stPos, strWhat.size(), strWith);
      // Go forward so we can search for the next occurence
      stPos += strWith.size();
      // Start again
      goto NextCharacter;
    } // Occurences not found
    ++stPos;
    // Occurence found
    NextCharacter:;
  } // ...until no more characters left
  while(stPos < strDest.size());
  // Return the string we build
  return strDest;
}
/* -- Basic multiple replace of text in string ----------------------------- */
template<class ListType = StrPairList, class StrType>
  requires StdIsStrOrCStr<StrType>
static StdString StrReplaceEx(StrType &&strSrc, const ListType &ltList)
  { StdString strDest{ strSrc }; return StrReplaceExRef(strDest, ltList); }
/* -- Basic replace of text in string -------------------------------------- */
static StdString &StrReplaceCharRef(StdString &strStr, const char cWhat,
  const char cWith)
{ // If not empty then replace every character we find
  if(!strStr.empty())
    for(size_t stPos = strStr.find(cWhat, 0); stPos != StdNPos;
      stPos = strStr.find(cWhat, stPos)) strStr[stPos++] = cWith;
  // Return string
  return strStr;
}
/* -- Basic replace of text in string -------------------------------------- */
template<class StrType>
  requires StdIsStrOrCStr<StrType>
static StdString StrReplaceChar(StrType &&strStr, const char cWhat,
  const char cWith)
{ StdString strDst{ strStr }; return StrReplaceCharRef(strDst, cWhat, cWith); }
/* ------------------------------------------------------------------------- */
template<class StrWhatType, class StrWithType>
  requires StdIsString<StrWhatType> && StdIsString<StrWithType>
static StdString &StrReplaceRef(StdString &strDest, StrWhatType &&strWhat,
  StrWithType &&strWith)
{ // Return original string if empty
  if(strDest.empty()) return strDest;
  // For each occurence of 'strWhat' with 'strWith'.
  for(size_t stPos  = strDest.find(strWhat, 0);
             stPos != StdNPos;
             stPos  = strDest.find(strWhat, stPos))
  { // Replace occurence
    strDest.replace(stPos, strWhat.size(), strWith);
    // Push position forward so we don't risk infinite loop
    stPos += strWith.size();
  } // Return string
  return strDest;
}
/* -- Basic replace of text in string -------------------------------------- */
template<class StrType, class StrWhatType, class StrWithType>
  requires StdIsStrOrCStr<StrType> &&
           StdIsStrOrCStr<StrWhatType> &&
           StdIsStrOrCStr<StrWithType>
static StdString StrReplace(StrType &&strStr, StrWhatType &&strWhat,
  StrWithType &&strWith)
{ StdString strOut{ strStr };
  return StrReplaceRef(strOut, StdForward<StrWhatType>(strWhat),
                               StdForward<StrWithType>(strWith)); }
/* -- Replace all occurences of whitespace with plus ----------------------- */
template<class StrType>
  requires StdIsStrOrCStr<StrType>
static StdString StrUrlEncodeSpaces(StrType &&strText)
  { return StrReplaceChar(StdForward<StrType>(strText), ' ', '+'); }
/* ------------------------------------------------------------------------- */
template<class StrTypeIn, class StrTypeAlt = StrTypeIn>
  requires StdIsStrOrCStr<StrTypeIn> && StdIsSame<StrTypeIn, StrTypeAlt>
static auto &StrIsBlank(StrTypeIn &&strIn, StrTypeAlt &&strAlt)
  { return strIn.empty() ? strAlt : strIn; }
/* -- Pluralise (returns a reference to an lvalue argument) ---------------- */
template<typename IntType, typename StrTypeSing, typename StrTypePlur>
  requires StdIsIntegral<IntType> &&
           StdIsStrOrCStr<StrTypeSing> &&
           StdIsStrOrCStr<StrTypePlur> &&
           StdIsSame<StrTypeSing, StrTypePlur>
StrTypeSing StrPluraliseRef(const IntType itC, StrTypeSing &&stsSingular,
  StrTypePlur &&stpPlural)
{ return itC != 1 ? stpPlural : stsSingular; }
/* -- Handles temporaries by returning by value avoiding dangling refs ----- */
template<typename IntType, typename StrTypeSing, typename StrTypePlur>
  requires StdIsIntegral<IntType> &&
           StdIsStrOrCStr<StrTypeSing> &&
           StdIsStrOrCStr<StrTypePlur>
constexpr auto StrPluraliseCopy(const IntType itC, StrTypeSing &&stsSingular,
  StrTypePlur &&stpPlural)
{ return itC != 1 ? StdForward<StrTypePlur>(stpPlural) :
                    StdForward<StrTypeSing>(stsSingular); }
/* -- Build non-imbued number with requested strings ----------------------- */
template<typename IntType, typename StrTypeSing, typename StrTypePlur>
  requires StdIsIntegral<IntType> &&
           StdIsStrOrCStr<StrTypeSing> &&
           StdIsStrOrCStr<StrTypePlur>
static StdString StrPluraliseNum(const IntType itCount,
  StrTypeSing &&stsSingular, StrTypePlur &&stpPlural)
{ return StrAppend(itCount, ' ',
    StrPluraliseCopy<IntType, StrTypeSing, StrTypePlur>
      (itCount, StdForward<StrTypeSing>(stsSingular),
                StdForward<StrTypePlur>(stpPlural))); }
/* -- Build imbued number with requested strings --------------------------- */
template<typename IntType, typename StrTypeSing, typename StrTypePlur>
  requires StdIsIntegral<IntType> &&
           StdIsStrOrCStr<StrTypeSing> &&
           StdIsStrOrCStr<StrTypePlur>
static StdString StrPluraliseNumEx(const IntType itCount,
  StrTypeSing &&stsSingular, StrTypePlur &&stpPlural)
{ return StrAppend(StrReadableFromNum(itCount), ' ',
    StrPluraliseRef<IntType, StrTypeSing, StrTypePlur>
      (itCount, StdForward<StrTypeSing>(stsSingular),
                StdForward<StrTypePlur>(stpPlural))); }
/* ------------------------------------------------------------------------- */
static const StdStringView &StrGetPositionSuffix(const uint64_t ullPosition)
{ // Pre-defined strings
  static const StdArray<const StdStringView, 4> aParts{{
    { "th" }, { "st" }, { "nd" }, { "rd" } }};
  // Value as base 100 not in teens? Compare value as base 10 instead
  const uint64_t ullVb100 = ullPosition % 100;
  if(ullVb100 <= 10 || ullVb100 >= 20)
  { // Get value as base 10 and return appropriate string from above array
    const uint64_t ullVb10 = ullPosition % 10;
    return aParts[ ullVb10 >= aParts.size() ? 0 : ullVb10 ];
  } // Everything else is "th"
  return aParts.front();
}
/* -- Get position of number as a string ----------------------------------- */
static StdString StrFromPosition(const uint64_t ullPosition)
  { return StrAppend(ullPosition, StrGetPositionSuffix(ullPosition)); }
/* -- Capitalise a string reference ---------------------------------------- */
static StdString &StrCapitaliseRef(StdString &strStr)
{ // Uppercase the first character of the string if we can and return it
  if(!strStr.empty()) strStr[0] = StdToUpper<char>(strStr.front());
  return strStr;
}
/* -- Capitalise a string -------------------------------------------------- */
template<class StrType>
  requires StdIsStrOrCStr<StrType>
static StdString StrCapitalise(StrType &&strStr)
  { StdString strNew{ strStr }; return StrCapitaliseRef(strNew); }
/* -- Evaluate a list of booleans and return a character value ------------- */
static StdString StrFromEvalTokens(const BoolCharPairVector &bcpvList)
  { return bcpvList.empty() ? cCommon->CommonBlank() :
      StdAccumulate(bcpvList.cbegin(), bcpvList.cend(), cCommon->CommonBlank(),
        [](const StdString &strOut, const BoolCharPair &bcpPair)
          { return bcpPair.first ? StrAppend(strOut,
            bcpPair.second) : strOut; }); }
/* -- Return true of false ------------------------------------------------- */
static const StdString &StrFromBoolTF(const bool bCondition)
  { return bCondition ? cCommon->CommonTrue() : cCommon->CommonFalse(); }
static const StdString &StrFromBoolYN(const bool bCondition)
  { return bCondition ? cCommon->CommonYes() : cCommon->CommonNo(); }
/* -- Count occurence of string -------------------------------------------- */
template<class StrType, class StrWhatType>
  static size_t StrCountOccurences(StrType &&strStr, StrWhatType &&strWhat)
{ // Normalise to StringView if not a string
  using StdNormString = StdNormalisedString<StrType>;
  StdNormString snsStr{ StdForward<StrType>(strStr) };
  using StdNormWhat = StdNormalisedString<StrWhatType>;
  StdNormWhat snsWhat{ StdForward<StrWhatType>(strWhat) };
  // Zero if string is empty
  if(snsStr.empty() || snsWhat.empty()) return 0;
  // Matching occurrences
  size_t stCount = 0;
  // Find occurrences
  for(size_t stIndex = snsStr.find(snsWhat);
             stIndex != StdNPos;
             stIndex = snsStr.find(snsWhat, stIndex + 1)) ++stCount;
  // Return occurrences
  return stCount;
}
/* -- Implode a stringdeque to a single string ----------------------------- */
template<class AnyArray, class StrSepType>
  requires StdIsStrOrCStr<StrSepType>
static StdString StrImplode(const AnyArray &aaArray,
  StrSepType &sstSep, const ssize_t &sstBegin = 0)
{ // Cast array size to ssize_t
  const ssize_t sstSize = static_cast<ssize_t>(aaArray.size());
  // Done if empty or begin position is invalid
  if(aaArray.empty() || sstBegin >= sstSize) return {};
  // Create output only string stream which stays cached (safe in c++11)
  StdOStringStream osS;
  // Get first iterator (penultimate from the end in the array)
  using AnyArrayConstIt = typename AnyArray::const_iterator;
  AnyArrayConstIt aaciStart{ StdNext(aaArray.cbegin(), sstBegin) };
  // How many items do we have? Have more than 1?
  if(sstSize - sstBegin != 1)
  { // Get ending iterator (penultimate from the beginning in the array)
    const AnyArrayConstIt aaciEnd{ StdPrev(aaArray.cend()) };
    // Build command string from vector
    while(aaciStart != aaciEnd) osS << *(aaciStart++) << sstSep;
    // Add final item
    osS << *aaciStart;
  } // Just access the one directly
  else osS << *aaciStart;
  // Done
  return osS.str();
}
/* ------------------------------------------------------------------------- */
template<typename IntType>
  requires StdIsIntegral<IntType> || StdIsFloat<IntType>
static StdString StrPrefixPosNeg(const IntType itVal, const int iPrecision)
  { return StrAppend(StdIOSShowPos, StdIOSFixed,
      StdIOSSetPrecision(iPrecision), itVal); }
/* ------------------------------------------------------------------------- */
template<typename IntType>
  requires StdIsIntegral<IntType> || StdIsFloat<IntType>
static StdString StrPrefixPosNegReadable(const IntType itVal,
  const int iPrecision)
{ return StrAppendImbue(StdIOSShowPos, StdIOSFixed,
    StdIOSSetPrecision(iPrecision), itVal); }
/* ------------------------------------------------------------------------- */
template<typename OutType, typename InType, class SuffixClass>
  requires StdIsFloat<OutType> && StdIsIntegral<InType> &&
    StdIsClass<SuffixClass>
static OutType StrToReadableSuffix(const InType itValue,
  const char**const cpSuffix, int &iPrecision, const SuffixClass &scLookup,
  const char*const cpDefault)
{ // Value to return
  OutType otReturn;
  // Discover the best measurement to show by testing each unit from the
  // lookup table to see if it is divisible and if it is not then try the next
  // one. The 'auto' is required because 'SuffixClass' is different array type.
  if(!StdAnyOf(scLookup.cbegin(), scLookup.cend(),
    [&otReturn, itValue, &cpSuffix](const auto &aItem)
  { // Calculate best measurement to show
    if(itValue < aItem.vValue) return false;
    // Set suffix that was sent and return success
    *cpSuffix = aItem.cpSuf;
    otReturn = static_cast<OutType>(itValue) /
      static_cast<OutType>(aItem.vValue);
    return true;
  }))
  { // Not found any matches so precision will now be zero
    iPrecision = 0;
    // Suffix is default suffix
    *cpSuffix = cpDefault;
    // Convert the input value to the output value
    otReturn = static_cast<OutType>(itValue);
  } // Return result
  return otReturn;
}
/* ------------------------------------------------------------------------- */
template<typename OutType, typename InType, class SuffixClass>
  static OutType StrToReadableSuffix(const InType itValue,
    const char**const cpSuffix, int &iPrecision, const SuffixClass &scLookup)
{ return StrToReadableSuffix<OutType, InType, SuffixClass>(itValue,
    cpSuffix, iPrecision, scLookup, cCommon->CommonCBlank()); }
/* ------------------------------------------------------------------------- */
template<typename IntType>
  static double StrToBytesHelper(const IntType itBytes,
    const char**const cpSuffix, int &iPrecision)
{ // A test to perform
  struct ByteValue { const IntType vValue; const char*const cpSuf; };
  // If input value is 64-bit?
  if constexpr(sizeof(IntType) == sizeof(uint64_t))
  { // Tests lookup table. This is all we can fit in a 64-bit integer
    static const StdArray<const ByteValue,6> bvLookup{ {
      { 0x1000000000000000ULL, "EB" }, { 0x0004000000000000ULL, "PB" },
      { 0x0000010000000000ULL, "TB" }, { 0x0000000040000000ULL, "GB" },
      { 0x0000000000100000ULL, "MB" }, { 0x0000000000000400ULL, "KB" }
    } };
    return StrToReadableSuffix<double>(itBytes,
      cpSuffix, iPrecision, bvLookup, "B");
  } // If input value is 32-bit?
  else if constexpr(sizeof(IntType) == sizeof(uint32_t))
  { // Tests lookup table. This is all we can fit in a 32-bit integer
    static const StdArray<const ByteValue,3> bvLookup{ {
      { 0x40000000UL, "GB" }, { 0x00100000UL, "MB" }, { 0x00000400UL, "KB" }
    } };
    return StrToReadableSuffix<double>(itBytes,
      cpSuffix, iPrecision, bvLookup, "B");
  } // If input value is 16-bit?
  else if constexpr(sizeof(IntType) == sizeof(uint16_t))
  { // Tests lookup table. This is all we can fit in a 16-bit integer
    static const StdArray<const ByteValue,1> bvLookup{ { { 0x0400, "KB" } } };
    return StrToReadableSuffix<double>(itBytes,
      cpSuffix, iPrecision, bvLookup, "B");
  } // Else needed on MSVC
  else
  { // Input value is not 64, 32 nor 16 bit? Use a empty table
    static const StdArray<const ByteValue,0> bvLookup{ {} };
    return StrToReadableSuffix<double>(itBytes,
      cpSuffix, iPrecision, bvLookup, "B");
  }
}
/* ------------------------------------------------------------------------- */
template<typename IntType>
  static StdString StrToBytes(const IntType itBytes, int iPrecision = 2)
{ // Process a human readable value for the specified number of bytes
  const char *cpSuffix = nullptr;
  const double dVal =
    StrToBytesHelper<IntType>(itBytes, &cpSuffix, iPrecision);
  // Move the stringstreams output string into the return value.
  return StrAppend(StdIOSFixed, StdIOSSetPrecision(iPrecision), dVal,
    cpSuffix);
}
/* ------------------------------------------------------------------------- */
template<typename IntType>
  static StdString StrToReadableBytes(const IntType itBytes,
    int iPrecision = 2)
{ // Process a human readable value for the specified number of bytes
  const char *cpSuffix = nullptr;
  const double dVal =
    StrToBytesHelper<IntType>(itBytes, &cpSuffix, iPrecision);
  // Move the stringstreams output string into the return value.
  return StrAppendImbue(StdIOSFixed, StdIOSSetPrecision(iPrecision), dVal,
    cpSuffix);
}
/* ------------------------------------------------------------------------- */
template<typename IntType>
  requires StdIsIntegral<IntType>
static double StrToReadableBitsHelper(const IntType itBits,
  const char**const cpSuffix, int &iPrecision)
{ // A test to perform
  struct BitValue { const IntType vValue; const char*const cpSuf; };
  // If input value is 64-bit?
  if constexpr(sizeof(IntType) == sizeof(uint64_t))
  { // Tests lookup table. This is all we can fit in a 64-bit integer.
    static const StdArray<const BitValue,6> bvLookup{ {
      { 1000000000000000000ULL, "Eb" }, { 1000000000000000ULL, "Pb" },
      {       1000000000000ULL, "Tb" }, {       1000000000ULL, "Gb" },
      {             1000000ULL, "Mb" }, {             1000ULL, "Kb" },
    } };
    // Return result
    return StrToReadableSuffix<double>(itBits,
      cpSuffix, iPrecision, bvLookup, "b");
  } // If input value is 32-bit?
  else if constexpr(sizeof(IntType) == sizeof(uint32_t))
  { // Tests lookup table. This is all we can fit in a 32-bit integer.
    static const StdArray<const BitValue,3> bvLookup{ {
      { 1000000000UL, "Gb" }, { 1000000UL, "Mb" }, { 1000UL, "Kb" },
    } };
    // Return result
    return StrToReadableSuffix<double>(itBits,
      cpSuffix, iPrecision, bvLookup, "b");
  } // If input value is 16-bit?
  else if constexpr(sizeof(IntType) == sizeof(uint16_t))
  { // Tests lookup table. This is all we can fit in a 16-bit integer.
    static const StdArray<const BitValue,6> bvLookup{ { { 1000, "Kb" } } };
    // Return result
    return StrToReadableSuffix<double>(itBits,
      cpSuffix, iPrecision, bvLookup, "b");
  } // Else needed on MSVC
  else
  { // Input value is not 64, 32 nor 16 bit? Use a empty table
    static const StdArray<const BitValue,0> bvLookup{ {} };
    // Show error
    return StrToReadableSuffix<double>(itBits,
      cpSuffix, iPrecision, bvLookup, "b");
  }
}
/* ------------------------------------------------------------------------- */
template<typename IntType>
  static StdString StrToBits(const IntType itBits, int iPrecision = 2)
{ // Process a human readable value for the specified number of bits
  const char *cpSuffix = nullptr;
  const double dVal =
    StrToReadableBitsHelper<IntType>(itBits, &cpSuffix, iPrecision);
  // Move the stringstreams output string into the return value.
  return StrAppend(StdIOSFixed, StdIOSSetPrecision(iPrecision), dVal,
    cpSuffix);
}
/* ------------------------------------------------------------------------- */
template<typename IntType>
  static StdString StrToReadableBits(const IntType itBits, int iPrecision)
{ // Process a human readable value for the specified number of bits
  const char *cpSuffix = nullptr;
  const double dVal =
    StrToReadableBitsHelper<IntType>(itBits, &cpSuffix, iPrecision);
  // Move the stringstreams output string into the return value.
  return StrAppendImbue(StdIOSFixed, StdIOSSetPrecision(iPrecision), dVal,
    cpSuffix);
}
/* ------------------------------------------------------------------------- */
template<typename IntType>
  static double StrToReadableHelper(const IntType itValue,
    const char**const cpSuffix, int &iPrecision)
{ // A test to perform
  struct Value { const IntType vValue; const char*const cpSuf; };
  // If input value is 64-bit?
  if constexpr(sizeof(IntType) == sizeof(uint64_t))
  { // Tests lookup table. This is all we can fit in a 64-bit integer.
    static const StdArray<const Value,4> vLookup{ {
      { 1000000000000ULL, "T" }, { 1000000000ULL, "B" },
      { 1000000ULL,       "M" }, { 1000ULL,       "K" }
    } };
    // Return result
    return StrToReadableSuffix<double>(itValue, cpSuffix, iPrecision, vLookup);
  } // If input value is 32-bit?
  else if constexpr(sizeof(IntType) == sizeof(uint32_t))
  { // Tests lookup table. This is all we can fit in a 64-bit integer.
    static const StdArray<const Value,3> vLookup{ {
      { 1000000000UL, "B" }, { 1000000UL, "M" }, { 1000UL, "K" }
    } };
    // Return result
    return StrToReadableSuffix<double>(itValue, cpSuffix, iPrecision, vLookup);
  } // If input value is 16-bit?
  else if constexpr(sizeof(IntType) == sizeof(uint16_t))
  { // Tests lookup table. This is all we can fit in a 64-bit integer.
    static const StdArray<const Value,1> vLookup{ { { 1000, "K" } } };
    // Return result
    return StrToReadableSuffix<double>(itValue, cpSuffix, iPrecision, vLookup);
  } // Else needed on MSVC
  else
  { // Input value is not 64, 32 nor 16 bit? Use a empty table
    static const StdArray<const Value,0> vLookup{ {} };
    // Show error
    return StrToReadableSuffix<double>(itValue, cpSuffix, iPrecision, vLookup);
  }
}
/* ------------------------------------------------------------------------- */
template<typename IntType, typename StrTypeSing, typename StrTypePlur>
  requires StdIsIntegral<IntType> &&
           StdIsStrOrCStr<StrTypeSing> &&
           StdIsStrOrCStr<StrTypePlur>
StdString StrToGroupedPluralise(const IntType itValue,
  StrTypeSing &&stsSingular, StrTypePlur &&stpPlural, int iPrecision = 2)
{ // Process a human readable value for the specified number of bits
  const char *cpSuffix = nullptr;
  const double dVal =
    StrToReadableHelper<IntType>(itValue, &cpSuffix, iPrecision);
  // Move the stringstreams output string into the return value.
  return StrAppend(StdIOSFixed, StdIOSSetPrecision(iPrecision), dVal, cpSuffix,
    StrPluraliseRef<IntType, StrTypeSing, StrTypePlur>
      (itValue, StdForward<StrTypeSing>(stsSingular),
                StdForward<StrTypePlur>(stpPlural)));
}
/* ------------------------------------------------------------------------- */
template<typename IntType>
  requires StdIsIntegral<IntType> || StdIsFloat<IntType>
static StdString StrToGrouped(const IntType itValue, int iPrecision = 2)
{ // Process a human readable value for the specified number of bits
  const char *cpSuffix = nullptr;
  const double dVal =
    StrToReadableHelper<IntType>(itValue, &cpSuffix, iPrecision);
  // Move the stringstreams output string into the return value.
  return StrAppend(StdIOSFixed, StdIOSSetPrecision(iPrecision), dVal,
    cpSuffix);
}
/* ------------------------------------------------------------------------- */
template<typename IntType>
  requires StdIsIntegral<IntType> || StdIsFloat<IntType>
static StdString StrToReadableGrouped(const IntType itValue, int iPrecision)
{ // Process a human readable value for the specified number of bits
  const char *cpSuffix = nullptr;
  const double dVal =
    StrToReadableHelper<IntType>(itValue, &cpSuffix, iPrecision);
  // Move the FORMATTED stringstreams output string into the return value.
  return StrAppendImbue(StdIOSFixed, StdIOSSetPrecision(iPrecision), dVal,
    cpSuffix);
}
/* ------------------------------------------------------------------------- */
template<class StrType>
  requires StdIsStrOrCStr<StrType>
static size_t StrFindCharForwards(StrType &&strStr, size_t stStart,
  const size_t stEnd, const char cpChar = ' ')
{ // Until we've reached the limit
  while(stStart < stEnd && stStart != StdNPos)
  { // Return position if we find the character
    if(strStr[stStart] == cpChar) return stStart;
    // Goto next index and try again
    ++stStart;
  } // Failed so return so
  return StdNPos;
}
/* ------------------------------------------------------------------------- */
template<class StrType>
  requires StdIsStrOrCStr<StrType>
static size_t StrFindCharBackwards[[maybe_unused]](StrType &&strStr,
  size_t stStart, const size_t stEnd, const char cpChar = ' ')
{ // Until we've reached the limit
  while(stStart >= stEnd && stStart != StdNPos)
  { // Return position if we find the character
    if(strStr[stStart] == cpChar) return stStart;
    // Goto next index and try again
    --stStart;
  } // Failed so return so
  return StdNPos;
}
/* ------------------------------------------------------------------------- */
template<class StrType>
  requires StdIsStrOrCStr<StrType>
static size_t StrFindCharNotForwards(StrType &strStr,
  size_t stStart, const size_t stEnd, const char cpChar = ' ')
{ // Until we've reached the limit
  while(stStart < stEnd && stStart != StdNPos)
  { // Return position if we find the character
    if(strStr[stStart] != cpChar) return stStart;
    // Goto next index and try again
    ++stStart;
  } // Failed so return so
  return StdNPos;
}
/* ------------------------------------------------------------------------- */
template<class StrType>
  requires StdIsStrOrCStr<StrType>
static size_t StrFindCharNotBackwards(StrType &&strStr,
  size_t stStart, const size_t stEnd, const char cpChar = ' ')
{ // Until we've reached the limit
  while(stStart >= stEnd && stStart != StdNPos)
  { // Return position if we find the character
    if(strStr[stStart] != cpChar) return stStart;
    // Goto next index and try again
    --stStart;
  } // Failed so return so
  return StdNPos;
}
/* -- Remove suffixing carriage return and line feed ----------------------- */
static StdString &StrChopRef(StdString &strStr)
{ // If string is not empty?
  if(!strStr.empty())
  { // Find the pos of the last char that is not a carriage return or line feed
    const size_t stEndPos = strStr.find_last_not_of(cCommon->CommonCrLf());
    // If all characters are removed, set the string to empty else erase part
    if(stEndPos == StdNPos) strStr.clear();
    else strStr.erase(stEndPos + 1);
  } // Return the modified string
  return strStr;
}
/* -- Remove suffixing carriage return and line feed ----------------------- */
template<class StrType>
  requires StdIsStrOrCStr<StrType>
static StdString StrChop(StrType &&strStr)
{ // Copy to a new string and remove the required characters
  StdString strOut{ strStr };
  return StrChopRef(strOut);
}
/* ------------------------------------------------------------------------- */
template<typename IntType>
  requires StdIsFloat<IntType> || StdIsIntegral<IntType>
static StdString StrFromRatio(const IntType itAntecedent,
  const IntType itConsequent)
{ // Return failure if parameters negative or zero
  if(itAntecedent <= 0 || itConsequent <= 0) return "0:0";
  // If we're a number, we need to convert it to an integer or gcd() fails
  if constexpr(StdIsFloat<IntType>)
    return StrFromRatio(static_cast<unsigned>(itAntecedent),
      static_cast<unsigned>(itConsequent));
  // Integral?
  else
  { // Calculate the greatest common divisor
    const IntType itGCD = ::std::gcd(itAntecedent, itConsequent),
      // Calculate the simplified ratio
      iitNum = itAntecedent / itGCD,
      itDen = itConsequent / itGCD;
    // Return the ratio as a string
    return StrAppend(iitNum, ':', itDen);
  }
}
/* -- Convert list to exploded string -------------------------------------- */
template<class ListType, class StrSepType, class StrLastType>
  requires StdIsStrOrCStr<StrSepType> && StdIsStrOrCStr<StrLastType>
static StdString StrExplodeEx(const ListType &lType, StrSepType &&sstSep,
  StrLastType &&sltLast)
{ // String to return
  StdOStringStream osS;
  // What is the size of this string
  switch(lType.size())
  { // Empty list? Just break to return empty string
    case 0: break;
    // Only one? Just return the string directly
    case 1: return StdString{ *lType.begin() };
    // Two? Return a simple appendage.
    case 2: osS << *lType.begin() << sltLast << *lType.crbegin(); break;
    // More than two? Write the first item first
    default: osS << *lType.begin();
             // Container type
             using ListTypeValue = typename ListType::value_type;
             // Write the rest but one prefixed with the separator
             StdForEach(seq,
               StdNext(lType.begin()), StdNext(lType.rbegin()).base(),
                 [&osS, &sstSep](const ListTypeValue &ltvStr)
                   { osS << sstSep << ltvStr; });
             // and now append the last separator and string from list
             osS << sltLast << *lType.rbegin();
             // Done
             break;
  } // Return the compacted string
  return osS.str();
}
/* -- Compact a string removing leading, trailing and duplicate spaces ----- */
static StdString &StrCompactRef(StdString &strStr)
{ // Return if string is empty
  if(strStr.empty()) return strStr;
  // Return string if no whitespace found
  const size_t stStart = strStr.find_first_not_of(' ');
  if (stStart == StdNPos) { strStr.clear(); return strStr; }
  // Trim trailing spaces
  const size_t stEnd = strStr.find_last_not_of(' ');
  strStr = strStr.substr(stStart, stEnd - stStart + 1);
  // Enumerate through spaces
  size_t stWriteIndex = 0;
  bool bInToken = false;
  for(size_t stReadIndex = 0; stReadIndex < strStr.size(); ++stReadIndex)
  { // Is character not a whitespace?
    if(StdIsNotSpace(strStr[stReadIndex]))
    { // Write space and increment position if in a space and not writing at
      // a different position.
      if(bInToken && stWriteIndex > 0) strStr[stWriteIndex++] = ' ';
      // Write the character
      strStr[stWriteIndex++] = strStr[stReadIndex];
      // No longer in a whitespace
      bInToken = false;
    } // Now in a whitespace block
    else bInToken = true;
  } // Resize the string to remove trailing spaces (if any)
  if(stWriteIndex > 0 && StdIsSpace(strStr[stWriteIndex - 1])) --stWriteIndex;
  // Truncate unused characters and the string
  strStr.resize(stWriteIndex);
  return strStr;
}
/* -- Compact a c-string removing duplicate spaces ------------------------- */
static StdString StrCompact(const char*cpStr)
{ // Ignore if empty
  if(UtfIsCStringNotValid(cpStr)) return {};
  // Convert to string, compact it and return it
  StdString strOut{ cpStr };
  StrCompactRef(strOut);
  return strOut;
}
/* -- Get return character format of text string --------------------------- */
template<class StrType>
  static StdStringView StrGetReturnFormat(StrType &&strStr)
{ // Normalise to StringView if not a string
  using StdNormString = StdNormalisedString<StrType>;
  StdNormString snsStr{ StdForward<StrType>(strStr) };
  using StdNormStringConstIt =
    typename StdDecay<StdNormString>::const_iterator;
   // String is not empty?
  if(!snsStr.empty())
    // Enumerate each character...
    for(StdNormStringConstIt snsciIt{ snsStr.cbegin() };
                             snsciIt != snsStr.cend();
                           ++snsciIt)
      // Test character
      switch(*snsciIt)
      { // Carriage-return found
        case '\r': return StdFind(par_unseq, snsciIt, snsStr.cend(), '\n') !=
          snsStr.cend() ? cCommon->CommonCrLf() : cCommon->CommonCr();
        // Line-feed found
        case '\n': return StdFind(par_unseq, snsciIt, snsStr.cend(), '\r') !=
          snsStr.cend() ? cCommon->CommonLfCr() : cCommon->CommonLf();
        // Anything else is ignored
        default: break;
      }
  // Nothing was found
  return {};
}
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
