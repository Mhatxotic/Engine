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
/* -- Private dependencies and functions ----------------------------------- */
using namespace ICommon::P;            using namespace IStd::P;
using namespace IUtf::P;
/* -- Format time alias ---------------------------------------------------- */
template<typename T>static auto StrPutTime(const StdTMStruct*const stdData,
  const T*const tFormat) { return ::std::put_time(stdData, tFormat); }
/* -- Parse time alias ----------------------------------------------------- */
template<typename T>static auto StrGetTime(StdTMStruct*const stdData,
  const T*const tFormat) { return ::std::get_time(stdData, tFormat); }
/* -- Process string format/append value into output string stream --------- */
template<typename AnyType>
  static void StrFormatValue(StdOStringStream &osS, const AnyType &atVal)
{ // If is an exception object? Push the string of it
  if constexpr(StdIsSame<AnyType, StdException>) osS << atVal.what();
  // Let ostringstream handle the value
  else osS << atVal;
}
/* -- Append final parameter to output string stream ----------------------- */
static void StrAppendParam(StdOStringStream&) {}
/* -- Append a parameter to output string stream --------------------------- */
template<typename AnyType, typename ...VarArgs>
  static void StrAppendParam(StdOStringStream &osS, const AnyType &atVal,
    VarArgs &&...vaArgs)
{ // Push the specified value and process the next argument
  StrFormatValue(osS, atVal);
  StrAppendParam(osS, StdForward<VarArgs>(vaArgs)...);
}
/* -- Format final parameter to output string stream ----------------------- */
static void StrFormatParam(StdOStringStream &osS, const char *cpPos)
  { if(*cpPos) osS << cpPos; }
/* -- Format any parameter to output string stream ------------------------- */
template<typename AnyType, typename ...VarArgs>
  static void StrFormatParam(StdOStringStream &osS, const char *cpPos,
    const AnyType &atVal, VarArgs &&...vaArgs)
{ // Find mark that will be replaced by this param and if we find the char?
  if(const char*const cpNewPos = strchr(cpPos, '$'))
  { // How far did we find the new position
    switch(const size_t stNum = static_cast<size_t>(cpNewPos - cpPos))
    { // One character? Just copy one character and move ahead two to skip over
      // the '$' we just processed.
      case 1: osS << *cpPos; cpPos += 2; break;
      // More than one character? Copy characters and stride over the '$' we
      // just processed. Better than storing single characters.
      default: osS << StdString{ cpPos, stNum };
               cpPos += stNum + 1;
               break;
      // Did not move? This can happen at the start of the string. Just move
      // over the first '$'.
      case 0: ++cpPos; break;
    } // Push the value we are supposed to replace the matched '$' with.
    StrFormatValue(osS, atVal);
    // Process more parameters if we can.
    StrFormatParam(osS, cpPos, StdForward<VarArgs>(vaArgs)...);
  } // Return the rest of the string.
  else StrFormatParam(osS, cpPos);
}
/* -- Converting type aliases ---------------------------------------------- */
template<typename T>using StdUnderlyingType = ::std::underlying_type_t<T>;
/* -- Types used for 'StrFromEvalTokens' function -------------------------- */
typedef StdPair<const bool, const char> BoolCharPair;
typedef StdVector<BoolCharPair> BoolCharPairVector;
 /* -- Public functions ---------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* -- Some helpful globals so not to repeat anything ----------------------- */
static const char*const cpTimeFormat = "%a %b %d %H:%M:%S %Y %z";
/* -- Append main function ------------------------------------------------- */
template<typename ...VarArgs> requires (sizeof...(VarArgs) > 0)
  static StdString StrAppend(VarArgs &&...vaArgs)
{ // Stream to write to
  StdOStringStream osS;
  // Build string
  StrAppendParam(osS, StdForward<VarArgs>(vaArgs)...);
  // Return string
  return osS.str();
}
/* -- Append with formatted numbers ---------------------------------------- */
template<typename ...VarArgs> requires (sizeof...(VarArgs) > 0)
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
/* -- Prepare message from c-string format --------------------------------- */
template<typename ...VarArgs> requires (sizeof...(VarArgs) > 0)
  static StdString StrFormat(const char*const cpFmt, VarArgs &&...vaArgs)
{ // Return if string empty of invalid
  if(UtfIsCStringNotValid(cpFmt)) return {};
  // Stream to write to
  StdOStringStream osS;
  // Format the text
  StrFormatParam(osS, cpFmt, StdForward<VarArgs>(vaArgs)...);
  // Return formated text
  return osS.str();
}
/* -- Prepare message from string format (used by build.cpp) --------------- */
template<typename ...VarArgs>
  static StdString StrFormat [[maybe_unused]](const StdString &strS,
    VarArgs &&...vaArgs)
{ // Return if string empty of invalid
  if(strS.empty()) return {};
  // Stream to write to
  StdOStringStream osS;
  // StrFormat the text
  StrFormatParam(osS, strS.data(), StdForward<VarArgs>(vaArgs)...);
  // Return formated text
  return osS.str();
}
/* -- Format a number ------------------------------------------------------ */
template<typename IntType>
  static StdString StrReadableFromNum(const IntType itVal, const int iPrec=0)
    { return StrAppendImbue(fixed, setprecision(iPrec), itVal); }
/* -- Trim specified characters from end of string ------------------------- */
static StdString StrTrimSuffix(const StdString &strStr, const char cChar)
{ // Return empty string if source string is empty or calculate ending
  // misoccurance of character then copy and return the string
  return strStr.empty() ?
    strStr : strStr.substr(0, strStr.find_last_not_of(cChar) + 1);
}
/* -- Trim specified characters from string -------------------------------- */
static StdString StrTrim(const StdString &strStr, const char cChar)
{ // Return empty string if source string is empty
  if(strStr.empty()) return strStr;
  // Calculate starting misoccurance of character. Return original if not found
  const size_t stBegin = strStr.find_first_not_of(cChar);
  if(stBegin == StdNPos) return strStr;
  // Calculate ending misoccurance of character then copy and return the string
  return strStr.substr(stBegin, strStr.find_last_not_of(cChar) - stBegin + 1);
}
/* -- Convert integer to string with padding and precision ----------------- */
template<typename IntType>
  static StdString StrFromNum(const IntType itV, const int iW=0,
    const int iPrecision=StdLimits<IntType>::digits10)
{ return StrAppend(setw(iW), fixed, setprecision(iPrecision), itV); }
/* -- Quickly convert numbered string to integer --------------------------- */
template<typename IntType=int64_t>
  static IntType StrToNum(const StdString &strValue)
{ // Put value into input string stream
  StdIStringStream isS{ strValue };
  // Push value into integer
  if constexpr(StdIsEnum<IntType>)
  { // Underlying value of the enum type to store into
    StdUnderlyingType<IntType> utN;
    // Store the value
    isS >> utN;
    // Return converting it back to the original type (no performance loss)
    return static_cast<IntType>(utN);
  } // Value is not enum type?
  else
  { // Value to store into
    IntType itN;
    // Store the value
    isS >> itN;
    // Return the value
    return itN;
  }
}
/* -- Quickly convert hex string to integer ------------------------------== */
template<typename IntType=int64_t>
  static IntType StrHexToInt(const StdString &strValue)
{ // Value to store into
  IntType itN;
  // Put value into input string stream
  StdIStringStream isS{ strValue };
  // Push value into integer
  isS >> hex >> itN;
  // Return result
  return itN;
}
/* -- Convert hex to string with zero padding ------------------------------ */
template<typename IntType>
  static StdString StrHexFromInt(const IntType itVal, const int iPrec=0)
    { return StrAppend(setfill('0'), hex, setw(iPrec), itVal); }
template<typename IntType>
  static StdString StrHexUFromInt(const IntType itVal, const int iPrec=0)
    { return StrAppend(setfill('0'), hex, setw(iPrec), uppercase, itVal); }
/* -- Return if specified string has numbers ------------------------------- */
static bool StrIsAlpha(const StdString &strValue)
  { return StdAllOf(par_unseq, strValue.cbegin(), strValue.cend(),
      [](const char cValue) { return StdIsAlpha(cValue); }); }
/* -- Return if specified string has numbers ------------------------------- */
static bool StrIsAlphaNum(const StdString &strValue)
  { return StdAllOf(par_unseq, strValue.cbegin(), strValue.cend(),
      [](const char cValue)
        { return StdIsAlnum(static_cast<int>(cValue)); }); }
/* -- Return if specified string is a valid integer ------------------------ */
template<typename IntType=int64_t>
  static bool StrIsInt(const StdString &strValue)
{ // Get string stream
  StdIStringStream isS{ strValue };
  // Test with string stream
  IntType itV; isS >> noskipws >> itV;
  // Return if succeeded
  return isS.eof() && !isS.fail();
}
/* -- Return if specified string is a valid float -------------------------- */
static bool StrIsFloat(const StdString &strValue)
  { return StrIsInt<double>(strValue); }
/* -- Return true if string is a value number to the power of 2 ------------ */
static bool StrIsNumPOW2(const StdString &strValue)
  { return !strValue.empty() &&
      StdIntIsPOW2(StdAbsolute(StrToNum(strValue))); }
/* -- Convert error number to string --------------------------------------- */
static StdString StrFromErrNo(const int iErrNo=errno)
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
  if(!cpResult) strErr = StrAppend("Error ", iErrNo);
  // We got a message but if was not put in our buffer just return as is
  else if(cpResult != strErr.data()) return cpResult;
#endif
  // Resize and compact the buffer
  strErr.resize(strlen(strErr.data()));
  // Have to do this because the string is still actually 94 bytes long
  return strErr;
}
/* -- Convert special formatted string to unix timestamp ------------------- */
static StdTimeT StrParseTime2(const StdString &strS)
{ // Time structure
  StdTMStruct tData;
  // Scan timestamp into time structure (Don't care about day name). We'll
  // store the timezone in tm_isdst and we'll optimise this by storing the
  // month string in the actual month integer var (4 or 8 bytes so safe).
  // Fmt: %3s %3s %02d %02d:%02d:%02d %05d %04d
  // Test example to just quickly copy and paste in the engine...
  // lexec 'Console.Write(Util.ParseTime2("Mon Mar 14 00:00:00 -0800 2017"));'
  StdIStringStream isS{ strS };
  isS >> StrGetTime(&tData, "%a %b %d %T");
  if(isS.fail()) return 0;
  isS >> tData.tm_wday;
  isS >> StrGetTime(&tData, "%Y");
  if(isS.fail()) return 0;
  // No daylight savings
  tData.tm_isdst = 0;
  // Return timestamp and adjust for specified timezone if neccesary
  return StdMkTime(&tData) + (!tData.tm_wday ? 0 : ((tData.tm_wday < 0 ?
    ((tData.tm_wday % 100) * 60) : -((tData.tm_wday % 100) * 60)) +
    ((tData.tm_wday / 100) * 3600)));
}
/* -- Convert ISO 8601 string to unix timestamp ---------------------------- */
static StdTimeT StrParseTime(const StdString &strS,
  const char*const cpF="%Y-%m-%dT%TZ")
{ // Time structure
  StdTMStruct tData;
  // Create static input stringstream (safe and fast in c++11)
  StdIStringStream isS{ strS };
  // Scan timestamp into time structure
  isS >> StrGetTime(&tData, cpF);
  if(isS.fail()) return 0;
  // Fill in other useless junk in the struct
  tData.tm_isdst = 0;
  // Return timestamp
  return StdMkTime(&tData);
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
/* -- Convert writable referenced string to lowercase ---------------------- */
static StdString &StrToLowCaseRef(StdString &strStr)
{ // If string is not empty
  if(!strStr.empty())
    StdTransform(par_unseq, strStr.begin(), strStr.end(), strStr.begin(),
      [](unsigned char ucChar) { return StdToLower(ucChar); });
  // Return output
  return strStr;
}
/* -- Convert string to upper case ----------------------------------------- */
static StdString StrToUpCase[[maybe_unused]](const StdString &strSrc)
{ // String empty? Return a blank one
  if(strSrc.empty()) return {};
  // Create memory for destination string and copy the string over
  StdReserved<StdString> strDst{ strSrc.size() };
  StdTransformNXP(strSrc.begin(), strSrc.end(), StdBackInserter(strDst),
    [](unsigned char ucChar) { return StdToUpper(ucChar); });
  // Return result
  return strDst;
}
/* -- Convert string to lower case ----------------------------------------- */
static StdString StrToLowCase[[maybe_unused]](const StdString &strSrc)
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
template<class ListType=StrPairList>
  static StdString &StrReplaceEx(StdString &strDest, const ListType &ltList)
{ // Return original string if empty
  if(strDest.empty() || ltList.empty()) return strDest;
  // Current index to scan
  size_t stPos = 0;
  // Repeat...
  do
  { // Container type
    typedef typename ListType::value_type ListTypeItem;
    // Enumerate each occurence to find...
    for(const ListTypeItem &ltiItem : ltList)
    { // Get string to find
      const StdString &strWhat = ltiItem.first;
      // Last cut position and current character index
      if(strncmp(strDest.data()+stPos, strWhat.data(), strWhat.length()))
        continue;
      // Get string to replace with
      const StdString &strWith = ltiItem.second;
      // Replace the occurence with the specified text
      strDest.replace(stPos, strWhat.length(), strWith);
      // Go forward so we can search for the next occurence
      stPos += strWith.length();
      // Start again
      goto NextCharacter;
    } // Occurences not found
    ++stPos;
    // Occurence found
    NextCharacter:;
  } // ...until no more characters left
  while(stPos < strDest.length());
  // Return the string we build
  return strDest;
}
/* -- Basic replace of text in string -------------------------------------- */
static StdString &StrReplace(StdString &strStr, const char cWhat,
  const char cWith)
{ // Return original string if empty
  if(strStr.empty()) return strStr;
  // For each occurence of 'strWhat' with 'strWith'.
  for(size_t stPos  = strStr.find(cWhat, 0);
             stPos != StdNPos;
             stPos  = strStr.find(cWhat, stPos)) strStr[stPos++] = cWith;
  // Return string
  return strStr;
}
/* -- Basic replace of text in string -------------------------------------- */
static StdString StrReplace(const StdString &strStr, const char cWhat,
  const char cWith)
    { StdString strDst{ strStr }; return StrReplace(strDst, cWhat, cWith); }
/* ------------------------------------------------------------------------- */
static StdString &StrReplace(StdString &strDest, const StdString &strWhat,
  const StdString &strWith)
{ // Return original string if empty
  if(strDest.empty()) return strDest;
  // For each occurence of 'strWhat' with 'strWith'.
  for(size_t stPos  = strDest.find(strWhat,0);
             stPos != StdNPos;
             stPos  = strDest.find(strWhat, stPos))
  { // Replace occurence
    strDest.replace(stPos, strWhat.length(), strWith);
    // Push position forward so we don't risk infinite loop
    stPos += strWith.length();
  } // Return string
  return strDest;
}
/* -- Basic replace of text in string -------------------------------------- */
static StdString StrReplace(const StdString &strIn, const StdString &strWhat,
  const StdString &strWith)
    { StdString strOut{ strIn }; return StrReplace(strOut, strWhat, strWith); }
/* -- Replace all occurences of whitespace with plus ----------------------- */
static StdString StrUrlEncodeSpaces(const StdString &strText)
  { return StrReplace(strText, ' ', '+'); }
/* ------------------------------------------------------------------------- */
template<class StrTypeIn, class StrTypeAlt=StrTypeIn>
  requires (StdIsPointer<StrTypeIn> || StdIsClass<StrTypeIn>) &&
    StdIsSame<StrTypeIn, StrTypeAlt>
static auto &StrIsBlank(const StrTypeIn &strIn, const StrTypeAlt &strAlt)
  { return strIn.empty() ? strAlt : strIn; }
/* ------------------------------------------------------------------------- */
template<class StrTypeIn>
  requires StdIsPointer<StrTypeIn> || StdIsClass<StrTypeIn>
static auto &StrIsBlank[[maybe_unused]](const StrTypeIn &strIn)
  { return StrIsBlank<StrTypeIn>(strIn, cCommon->CommonBlank()); }
/* ------------------------------------------------------------------------- */
template<typename IntType> requires StdIsInteger<IntType>
  static const char *StrCPluralise(const IntType itCount,
    const char*const cpSingular, const char*const cpPlural)
{ return itCount == 1 ? cpSingular : cpPlural; }
/* ------------------------------------------------------------------------- */
template<typename IntType>
  static StdString StrCPluraliseNum(const IntType itCount,
    const char *cpSingular, const char *cpPlural)
{ return StrAppend(itCount, ' ',
    StrCPluralise<IntType>(itCount, cpSingular, cpPlural)); }
/* ------------------------------------------------------------------------- */
template<typename IntType>
  static StdString StrCPluraliseNumEx(const IntType itCount,
    const char *cpSingular, const char *cpPlural)
{ return StrAppend(StrReadableFromNum(itCount), ' ',
    StrCPluralise<IntType>(itCount, cpSingular, cpPlural)); }
/* -- Convert time to long duration ---------------------------------------- */
static StdString StrLongFromDuration(const StdTimeT tDuration,
  unsigned int uiCompMax = StdMaxUInt)
{ // Time buffer
  StdTMStruct tD;
  // Lets convert the duration as a time then it will be properly formated
  // in terms of leap years, proper days in a month etc.
  StdGMTime(&tD, &tDuration);
  // Output string
  StdOStringStream osS;
  // If failed? Manually do it
  if(tD.tm_year == -1)
  { // Clear years and months since we can't realiably calculate that.
    tD.tm_year = tD.tm_mon = 0;
    // Set days, hours, minutes and seconds
    tD.tm_mday = static_cast<int>(tDuration / 86400);
    tD.tm_hour = static_cast<int>(tDuration / 3600 % 24);
    tD.tm_min = static_cast<int>(tDuration / 60 % 60);
    tD.tm_sec = static_cast<int>(tDuration % 60);
  } // Succeeded, subtract 70 as it returns as years past 1900.
  else tD.tm_year -= 70;
  // Add years?
  if(tD.tm_year && uiCompMax > 0)
  { // Do add years
    osS << StrCPluraliseNum(tD.tm_year, "year", "years");
    --uiCompMax;
  } // Add months?
  if(tD.tm_mon && uiCompMax > 0)
  { // Do add months
    osS << (osS.tellp() ? cCommon->CommonSpace() : cCommon->CommonBlank())
        << StrCPluraliseNum(tD.tm_mon, "month", "months");
    --uiCompMax;
  } // Add days? (removing the added 1)
  if(--tD.tm_mday && uiCompMax > 0)
  { // Do add days
    osS << (osS.tellp() ? cCommon->CommonSpace() : cCommon->CommonBlank())
        << StrCPluraliseNum(tD.tm_mday, "day", "days");
    --uiCompMax;
  } // Add hours?
  if(tD.tm_hour && uiCompMax > 0)
  { // Do add hours
    osS << (osS.tellp() ? cCommon->CommonSpace() : cCommon->CommonBlank())
        << StrCPluraliseNum(tD.tm_hour, "hour", "hours");
    --uiCompMax;
  } // Add Minutes?
  if(tD.tm_min && uiCompMax > 0)
  { // Do add minutes
    osS << (osS.tellp() ? cCommon->CommonSpace() : cCommon->CommonBlank())
        << StrCPluraliseNum(tD.tm_min, "min", "mins");
    --uiCompMax;
  } // Check seconds
  if((tD.tm_sec || !tDuration) && uiCompMax > 0)
    osS << (osS.tellp() ? cCommon->CommonSpace() : cCommon->CommonBlank())
        << StrCPluraliseNum(tD.tm_sec, "sec", "secs");
  // Return string
  return osS.str();
}
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
/* -- Capitalise a string -------------------------------------------------- */
static StdString StrCapitalise(const StdString &strStr)
{ // Capitalise first character if string not nullptr or empty
  if(strStr.empty()) return strStr;
  // Duplicate the string anad uppercase the first character
  StdString strNew{ strStr };
  strNew[0] = StdToUpper<char>(strStr.front());
  // Return provided string
  return strNew;
}
/* -- Evaluate a list of booleans and return a character value ------------- */
static StdString StrFromEvalTokens(const BoolCharPairVector &bcpvList)
  { return bcpvList.empty() ? cCommon->CommonBlank() :
      StdAccumulate(bcpvList.cbegin(), bcpvList.cend(), cCommon->CommonBlank(),
        [](const StdString &strOut, const BoolCharPair &bcpPair)
          { return bcpPair.first ? StrAppend(strOut,
            bcpPair.second) : strOut; }); }
/* -- Convert time to short duration --------------------------------------- */
static StdString StrShortFromDuration(const double dDuration,
  const int iPrecision=6)
{ // Output string
  StdOStringStream osS;
  // Get duration ceiled and if negative?
  double dInt, dFrac = modf(dDuration, &dInt);
  if(dInt < 0)
  { // Set negative symbol and negate the duration
    osS << '-';
    dInt = -dInt;
    dFrac = -dFrac;
  } // Set floating point precision with zero fill
  osS << fixed << setfill('0') << setprecision(0);
  // Have days?
  if(dInt >= 86400)
    osS <<                 floor(dInt/86400)     << ':'
        << setw(2) << fmod(floor(dInt/3600), 24) << ':'
        << setw(2) << fmod(floor(dInt/60),   60) << ':' << setw(2);
  // No days, but hours?
  else if(dInt >= 3600)
    osS <<            fmod(floor(dInt/3600), 24) << ':'
        << setw(2) << fmod(floor(dInt/60),   60) << ':' << setw(2);
  // No hours, but minutes?
  else if(dInt >= 60)
    osS << fmod(floor(dInt/60), 60) << ':' << setw(2);
  // No minutes so no zero padding
  else osS << setw(0);
  // On the seconds part, we have a problem where having a precision
  // of zero is causing stringstream to round so we'll just convert it to an
  // int instead to fix it.
  osS << fmod(dInt, 60);
  if(iPrecision > 0)
    osS << '.' << setw(iPrecision) <<
      static_cast<unsigned int>(fabs(dFrac) * pow(10.0, iPrecision));
  // Return string
  return osS.str();
}
/* -- Return true of false ------------------------------------------------- */
static const StdString &StrFromBoolTF(const bool bCondition)
  { return bCondition ? cCommon->CommonTrue() : cCommon->CommonFalse(); }
static const StdString &StrFromBoolYN(const bool bCondition)
  { return bCondition ? cCommon->CommonYes() : cCommon->CommonNo(); }
/* -- Count occurence of string -------------------------------------------- */
static size_t StrCountOccurences(const StdString &strStr,
  const StdString &strWhat)
{ // Zero if string is empty
  if(strStr.empty() || strWhat.empty()) return 0;
  // Matching occurences
  size_t stCount = 0;
  // Find occurences
  for(size_t stIndex = strStr.find(strWhat);
             stIndex != StdNPos;
             stIndex = strStr.find(strWhat, stIndex + 1)) ++stCount;
  // Return occurences
  return stCount;
}
/* -- Implode a stringdeque to a single string ----------------------------- */
template<class AnyArray, class CtrType = typename AnyArray::value_type>
  static StdString StrImplode(const AnyArray &aArray,
  const ssize_t &sstBegin=0, const StdString &strSep=cCommon->CommonSpace())
{ // Cast array size to ssize_t
  const ssize_t sstSize = static_cast<ssize_t>(aArray.size());
  // Done if empty or begin position is invalid
  if(aArray.empty() || sstBegin >= sstSize) return {};
  // Create output only string stream which stays cached (safe in c++11)
  StdOStringStream osS;
  // Get first iterator (penultimate from the end in the array)
  typedef typename AnyArray::const_iterator AnyArrayConstIt;
  AnyArrayConstIt aaciStart{ StdNext(aArray.cbegin(), sstBegin) };
  // How many items do we have? Have more than 1?
  if(sstSize - sstBegin != 1)
  { // Get ending iterator (penultimate from the beginning in the array)
    const AnyArrayConstIt aaciEnd{ StdPrev(aArray.cend()) };
    // Build command string from vector
    while(aaciStart != aaciEnd) osS << *(aaciStart++) << strSep;
    // Add final item
    osS << *aaciStart;
  } // Just access the one directly
  else osS << *aaciStart;
  // Done
  return osS.str();
}
/* -- Converts the key/value pairs to a stringvector ----------------------- */
static StdString ImplodeMap[[maybe_unused]](const StrNCStrMap &ssmSrc,
  const StdString &strLineSep=cCommon->CommonSpace(),
  const StdString &strKeyValSep=cCommon->CommonEquals(),
  const StdString &strValEncaps="\"")
{ // Done if empty
  if(ssmSrc.empty()) return {};
  // Make string vector to implode and reserve memory for items.
  // Insert each value in the map with the appropriate seperators.
  StdReserved<StrVector> svRet{ ssmSrc.size() };
  StdTransformNXP(ssmSrc.cbegin(), ssmSrc.cend(), StdBackInserter(svRet),
    [&strKeyValSep, &strValEncaps](const StrNCStrMapPair &sncsmpPair)
      { return StrAppend(sncsmpPair.first, strKeyValSep,
          strValEncaps, sncsmpPair.second, strValEncaps); });
  // Return vector imploded into a string
  return StrImplode(svRet, 0, strLineSep);
}
/* ------------------------------------------------------------------------- */
template<typename AnyType>
  static StdString StrPrefixPosNeg(const AnyType atVal, const int iPrecision)
{ return StrAppend(showpos, fixed, setprecision(iPrecision), atVal); }
/* ------------------------------------------------------------------------- */
template<typename AnyType>
  static StdString StrPrefixPosNegReadable(const AnyType atVal,
    const int iPrecision)
{ return StrAppendImbue(showpos, fixed, setprecision(iPrecision), atVal); }
/* ------------------------------------------------------------------------- */
template<typename OutType, typename InType, class SuffixClass>
  requires StdIsFloat<OutType> && StdIsInteger<InType> &&
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
    // Return result
    return StrToReadableSuffix<double>(itBytes,
      cpSuffix, iPrecision, bvLookup, "B");
  } // If input value is 32-bit?
  else if constexpr(sizeof(IntType) == sizeof(uint32_t))
  { // Tests lookup table. This is all we can fit in a 32-bit integer
    static const StdArray<const ByteValue,3> bvLookup{ {
      { 0x40000000UL, "GB" }, { 0x00100000UL, "MB" }, { 0x00000400UL, "KB" }
    } };
    // Return result
    return StrToReadableSuffix<double>(itBytes,
      cpSuffix, iPrecision, bvLookup, "B");
  } // If input value is 16-bit?
  else if constexpr(sizeof(IntType) == sizeof(uint16_t))
  { // Tests lookup table. This is all we can fit in a 16-bit integer
    static const StdArray<const ByteValue,1> bvLookup{ { { 0x0400, "KB" } } };
    // Return result
    return StrToReadableSuffix<double>(itBytes,
      cpSuffix, iPrecision, bvLookup, "B");
  } // Else needed on MSVC
  else
  { // Input value is not 64, 32 nor 16 bit? Use a empty table
    static const StdArray<const ByteValue,0> bvLookup{ {} };
    // Show error
    return StrToReadableSuffix<double>(itBytes,
      cpSuffix, iPrecision, bvLookup, "B");
  }
}
/* ------------------------------------------------------------------------- */
template<typename IntType>
  static StdString StrToBytes(const IntType itBytes, int iPrecision=2)
{ // Process a human readable value for the specified number of bytes
  const char *cpSuffix = nullptr;
  const double dVal =
    StrToBytesHelper<IntType>(itBytes, &cpSuffix, iPrecision);
  // Move the stringstreams output string into the return value.
  return StrAppend(fixed, setprecision(iPrecision), dVal, cpSuffix);
}
/* ------------------------------------------------------------------------- */
template<typename IntType>
  static StdString StrToReadableBytes(const IntType itBytes, int iPrecision=2)
{ // Process a human readable value for the specified number of bytes
  const char *cpSuffix = nullptr;
  const double dVal =
    StrToBytesHelper<IntType>(itBytes, &cpSuffix, iPrecision);
  // Move the stringstreams output string into the return value.
  return StrAppendImbue(fixed, setprecision(iPrecision), dVal, cpSuffix);
}
/* ------------------------------------------------------------------------- */
template<typename IntType>
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
  static StdString StrToBits(const IntType itBits, int iPrecision=2)
{ // Process a human readable value for the specified number of bits
  const char *cpSuffix = nullptr;
  const double dVal =
    StrToReadableBitsHelper<IntType>(itBits, &cpSuffix, iPrecision);
  // Move the stringstreams output string into the return value.
  return StrAppend(fixed, setprecision(iPrecision), dVal, cpSuffix);
}
/* ------------------------------------------------------------------------- */
template<typename IntType>
  static StdString StrToReadableBits(const IntType itBits, int iPrecision)
{ // Process a human readable value for the specified number of bits
  const char *cpSuffix = nullptr;
  const double dVal =
    StrToReadableBitsHelper<IntType>(itBits, &cpSuffix, iPrecision);
  // Move the stringstreams output string into the return value.
  return StrAppendImbue(fixed, setprecision(iPrecision), dVal, cpSuffix);
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
template<typename IntType>
  static StdString StrToGrouped(const IntType itValue, int iPrecision=2)
{ // Process a human readable value for the specified number of bits
  const char *cpSuffix = nullptr;
  const double dVal =
    StrToReadableHelper<IntType>(itValue, &cpSuffix, iPrecision);
  // Move the stringstreams output string into the return value.
  return StrAppend(fixed, setprecision(iPrecision), dVal, cpSuffix);
}
/* ------------------------------------------------------------------------- */
template<typename IntType>
  static StdString StrToReadableGrouped(const IntType itValue, int iPrecision)
{ // Process a human readable value for the specified number of bits
  const char *cpSuffix = nullptr;
  const double dVal =
    StrToReadableHelper<IntType>(itValue, &cpSuffix, iPrecision);
  // Move the FORMATTED stringstreams output string into the return value.
  return StrAppendImbue(fixed, setprecision(iPrecision), dVal, cpSuffix);
}
/* ------------------------------------------------------------------------- */
static size_t StrFindCharForwards(const StdString &strS, size_t stStart,
  const size_t stEnd, const char cpChar)
{ // Until we've reached the limit
  while(stStart < stEnd && stStart != StdNPos)
  { // Return position if we find the character
    if(strS[stStart] == cpChar) return stStart;
    // Goto next index and try again
    ++stStart;
  } // Failed so return so
  return StdNPos;
}
/* ------------------------------------------------------------------------- */
static size_t StrFindCharBackwards[[maybe_unused]](const StdString &strS,
  size_t stStart, const size_t stEnd, const char cpChar)
{ // Until we've reached the limit
  while(stStart >= stEnd && stStart != StdNPos)
  { // Return position if we find the character
    if(strS[stStart] == cpChar) return stStart;
    // Goto next index and try again
    --stStart;
  } // Failed so return so
  return StdNPos;
}
/* ------------------------------------------------------------------------- */
static size_t StrFindCharNotForwards[[maybe_unused]](const StdString &strS,
  size_t stStart, const size_t stEnd, const char cpChar)
{ // Until we've reached the limit
  while(stStart < stEnd && stStart != StdNPos)
  { // Return position if we find the character
    if(strS[stStart] != cpChar) return stStart;
    // Goto next index and try again
    ++stStart;
  } // Failed so return so
  return StdNPos;
}
/* ------------------------------------------------------------------------- */
static size_t StrFindCharNotForwards(const StdString &strS, size_t stStart,
  const size_t stEnd)
{ // Until we've reached the limit
  while(stStart < stEnd && stStart != StdNPos)
  { // Return position if we find a non-control character
    if(strS[stStart] > ' ') return stStart;
    // We could not match any character
    ++stStart;
  } // Failed so return so
  return StdNPos;
}
/* ------------------------------------------------------------------------- */
static size_t StrFindCharNotBackwards[[maybe_unused]](const StdString &strS,
  size_t stStart, const size_t stEnd, const char cpChar)
{ // Until we've reached the limit
  while(stStart >= stEnd && stStart != StdNPos)
  { // Return position if we find the character
    if(strS[stStart] != cpChar) return stStart;
    // Goto next index and try again
    --stStart;
  } // Failed so return so
  return StdNPos;
}
/* ------------------------------------------------------------------------- */
static size_t StrFindCharNotBackwards(const StdString &strS, size_t stStart,
  const size_t stEnd)
{ // Until we've reached the limit
  while(stStart >= stEnd && stStart != StdNPos)
  { // Return position if we find a non-control character
    if(strS[stStart] > ' ') return stStart;
    // We could not match any character
    --stStart;
  } // Failed so return so
  return StdNPos;
}
/* -- Do convert the specified structure to string ------------------------= */
static StdString StrFromTimeTM(const StdTMStruct &tmData, const char*const cpF)
  { return StrAppend(StrPutTime(&tmData, cpF)); }
/* -- Convert specified timestamp to string -------------------------------- */
static StdString StrFromTimeTT(const StdTimeT ttTimestamp,
  const char*const cpFormat = cpTimeFormat)
{ // Convert it to local time in a structure
  StdTMStruct tmData; StdLocalTime(&tmData, &ttTimestamp);
  // Do the parse and return the string
  return StrFromTimeTM(tmData, cpFormat);
}
/* -- Remove suffixing carriage return and line feed ----------------------- */
static StdString &StrChop(StdString &strStr)
{ // Find the pos of the last char that is not a carriage return or line feed
  const size_t stEndPos = strStr.find_last_not_of(cCommon->CommonCrLf());
  // If all characters are removed, set the string to empty else erase the part
  if(stEndPos == StdNPos) strStr.clear();
  else strStr.erase(stEndPos + 1);
  // Return the modified string
  return strStr;
}
/* -- Convert specified timestamp to string (UTC) -------------------------- */
static StdString StrFromTimeTTUTC(const StdTimeT ttTimestamp,
  const char*const cpFormat = cpTimeFormat)
{ // Convert it to local time
  StdTMStruct tmData; StdGMTime(&tmData, &ttTimestamp);
  // Do the parse and return the string
  return StrFromTimeTM(tmData, cpFormat);
}
/* ------------------------------------------------------------------------- */
template<typename IntType>
  StdString StrFromRatio(const IntType itAntecedent,
    const IntType itConsequent)
{ // Return failure if parameters negative or zero
  if(itAntecedent <= 0 || itConsequent <= 0) return "0:0";
  // If we're a number, we need to convert it to an integer or gcd() fails
  if constexpr(StdIsFloat<IntType>)
    return StrFromRatio(static_cast<unsigned int>(itAntecedent),
                        static_cast<unsigned int>(itConsequent));
  // Integral?
  else
  { // Calculate the greatest common divisor
    const IntType itGCD = ::std::gcd(itAntecedent, itConsequent),
    // Calculate the simplified ratio
                  itNum = itAntecedent / itGCD,
                  itDen = itConsequent / itGCD;
    // Return the ratio as a string
    return StrAppend(itNum, ':', itDen);
  }
}
/* -- Convert list to exploded string -------------------------------------- */
template<class ListType>
  static StdString StrExplodeEx(const ListType &lType, const StdString &strSep,
    const StdString &strLast)
{ // String to return
  StdOStringStream osS;
  // What is the size of this string
  switch(lType.size())
  { // Empty list? Just break to return empty string
    case 0: break;
    // Only one? Just return the string directly
    case 1: return *lType.cbegin();
    // Two? Return a simple appendage.
    case 2: osS << *lType.cbegin() << strLast << *lType.crbegin(); break;
    // More than two? Write the first item first
    default: osS << *lType.cbegin();
             // Container type
             typedef typename ListType::value_type ListTypeValue;
             // Write the rest but one prefixed with the separator
             StdForEach(seq,
               StdNext(lType.cbegin()), StdNext(lType.crbegin()).base(),
                 [&osS, &strSep](const ListTypeValue &strStr)
                   { osS << strSep << strStr; });
             // and now append the last separator and string from list
             osS << strLast << *lType.crbegin();
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
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
