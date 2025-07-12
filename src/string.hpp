/* == STRING.HPP =========================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## Miscaelennias string utility functions.                             ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IString {                    // Start of private module namespace
/* ------------------------------------------------------------------------- */
using namespace ICommon::P;            using namespace IStd::P;
using namespace IUtf::P;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* -- Some helpful globals so not to repeat anything ----------------------- */
static const char*const cpTimeFormat = "%a %b %d %H:%M:%S %Y %z";
/* -- Functions for StrAppend, StrAppendImbue and StrFormat ---------------- */
namespace H                            // Private functions
{ /* -- Process any value -------------------------------------------------- */
  template<typename AnyType>
    static void Value(ostringstream &osS, const AnyType &atVal)
  { // If is an exception object? Push the string of it
    if constexpr(is_same_v<AnyType, exception>) osS << atVal.what();
    // Let ostringstream handle the value
    else osS << atVal;
  }
  /* -- Append functions --------------------------------------------------- */
  namespace Append                     // Private functions
  { /* -- Append final parameter ------------------------------------------- */
    static void Param(ostringstream&) { }
    /* -- Append a parameter ----------------------------------------------- */
    template<typename AnyType, typename ...VarArgs>
      static void Param(ostringstream &osS, const AnyType &atVal,
        const VarArgs &...vatArgs)
    { // Push the specified value
      Value(osS, atVal);
      // Process next argument
      Param(osS, vatArgs...);
    }
    /* -- Append main function --------------------------------------------- */
    template<typename ...VarArgs>
      static const string StrAppend(const VarArgs &...vaVars)
    { // Theres no need to call this if theres no parameters
      static_assert(sizeof...(VarArgs) > 0, "Not enough parameters!");
      // Stream to write to
      ostringstream osS;
      // Build string
      Param(osS, vaVars...);
      // Return string
      return osS.str();
    }
    /* -- Append with formatted numbers ------------------------------------ */
    template<typename ...VarArgs>
      static const string StrAppendImbue(const VarArgs &...vaVars)
    { // Theres no need to call this if theres no parameters
      static_assert(sizeof...(VarArgs) > 0, "Not enough parameters!");
      // Stream to write to
      ostringstream osS;
      // Imbue current locale
      osS.imbue(cCommon->CommonLocale());
      // Build string
      Param(osS, vaVars...);
      // Return appended string
      return osS.str();
    }
  } /* -- Format functions ------------------------------------------------- */
  namespace Format                     // Private functions
  { /* -- Append final parameter (uses copy elision) ----------------------- */
    static void Param(ostringstream &osS, const char *cpPos)
      { if(*cpPos) osS << cpPos; }
    /* -- Process any value ------------------------------------------------ */
    template<typename AnyType, typename ...VarArgs>
      static void Param(ostringstream &osS, const char *cpPos,
        const AnyType &atVal, const VarArgs &...vaVars)
    { // Find the mark that will be replaced by this parameter and if we
      // find the character?
      if(const char*const cpNewPos = strchr(cpPos, '$'))
      { // How far did we find the new position
        switch(const size_t stNum = static_cast<size_t>(cpNewPos - cpPos))
        { // One character? Just copy one character and move ahead two to skip
          // over the '$' we just processed.
          case 1: osS << *cpPos; cpPos += 2; break;
          // More than one character? Copy characters and stride over the '$'
          // we just processed. Better than storing single characters.
          default: osS << string{ cpPos, stNum };
                   cpPos += stNum + 1;
                   break;
          // Did not move? This can happen at the start of the string. Just
          // move over the first '$'.
          case 0: ++cpPos; break;
        } // Push the value we are supposed to replace the matched '$' with.
        Value(osS, atVal);
        // Process more parameters if we can.
        Param(osS, cpPos, vaVars...);
      } // Return the rest of the string.
      else Param(osS, cpPos);
    }
    /* -- Prepare message from c-string format ----------------------------- */
    template<typename ...VarArgs>
      static const string StrFormat(const char*const cpFmt,
        const VarArgs &...vaVars)
    { // Theres no need to call this if theres no parameters
      static_assert(sizeof...(VarArgs) > 0, "Not enough parameters!");
      // Return if string empty of invalid
      if(UtfIsCStringNotValid(cpFmt)) return {};
      // Stream to write to
      ostringstream osS;
      // Format the text
      Param(osS, cpFmt, vaVars...);
      // Return formated text
      return osS.str();
    }
    /* -- Prepare message from string format ------------------------------- */
    template<typename ...VarArgs>
      static const string StrFormat[[maybe_unused]](const string &strS,
        const VarArgs &...vaVars)
    { // Return if string empty of invalid
      if(strS.empty()) return {};
      // Stream to write to
      ostringstream osS;
      // StrFormat the text
      Param(osS, strS.c_str(), vaVars...);
      // Return formated text
      return osS.str();
    }
  }
} /* ----------------------------------------------------------------------- */
using H::Append::StrAppend;            // Alias 'StrAppend' here
using H::Append::StrAppendImbue;       // Alias 'StrAppendImbue' here
using H::Format::StrFormat;            // Alias 'StrFormat' here
/* == Format a number ====================================================== */
template<typename IntType>
  static const string StrReadableFromNum(const IntType itVal,
    const int iPrec=0)
      { return StrAppendImbue(fixed, setprecision(iPrec), itVal); }
/* -- Trim specified characters from end of string ------------------------- */
static const string StrTrimSuffix(const string &strStr, const char cChar)
{ // Return empty string if source string is empty or calculate ending
  // misoccurance of character then copy and return the string
  return strStr.empty() ?
    strStr : strStr.substr(0, strStr.find_last_not_of(cChar) + 1);
}
/* -- Trim specified characters from string -------------------------------- */
static const string StrTrim(const string &strStr, const char cChar)
{ // Return empty string if source string is empty
  if(strStr.empty()) return strStr;
  // Calculate starting misoccurance of character. Return original if not found
  const size_t stBegin = strStr.find_first_not_of(cChar);
  if(stBegin == StdNPos) return strStr;
  // Calculate ending misoccurance of character then copy and return the string
  return strStr.substr(stBegin, strStr.find_last_not_of(cChar) - stBegin + 1);
}
/* -- Convert integer to string with padding and precision ----------------- */
template<typename IntType>static const string StrFromNum(const IntType itV,
  const int iW=0, const int iPrecision=numeric_limits<IntType>::digits10)
    { return StrAppend(setw(iW), fixed, setprecision(iPrecision), itV); }
/* -- Quickly convert numbered string to integer --------------------------- */
template<typename IntType=int64_t>
  static const IntType StrToNum(const string &strValue)
{ // Put value into input string stream
  istringstream isS{ strValue };
  // Push value into integer
  if constexpr(is_enum_v<IntType>)
  { // Underlying value of the enum type to store into
    underlying_type_t<IntType> utN;
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
  static const IntType StrHexToInt(const string &strValue)
{ // Value to store into
  IntType itN;
  // Put value into input string stream
  istringstream isS{ strValue };
  // Push value into integer
  isS >> hex >> itN;
  // Return result
  return itN;
}
/* -- Convert hex to string with zero padding ------------------------------ */
template<typename IntType>
  static const string StrHexFromInt(const IntType itVal, const int iPrec=0)
    { return StrAppend(setfill('0'), hex, setw(iPrec), itVal); }
template<typename IntType>
  static const string StrHexUFromInt(const IntType itVal, const int iPrec=0)
    { return StrAppend(setfill('0'), hex, setw(iPrec), uppercase, itVal); }
/* -- Return if specified string has numbers ------------------------------- */
static bool StrIsAlpha(const string &strValue)
  { return StdAllOf(par_unseq, strValue.cbegin(), strValue.cend(),
      [](const char cValue) { return StdIsAlpha(cValue); }); }
/* -- Return if specified string has numbers ------------------------------- */
static bool StrIsAlphaNum(const string &strValue)
  { return StdAllOf(par_unseq, strValue.cbegin(), strValue.cend(),
      [](const char cValue)
        { return StdIsAlnum(static_cast<int>(cValue)); }); }
/* -- Return if specified string is a valid integer ------------------------ */
template<typename IntType=int64_t>static bool StrIsInt(const string &strValue)
{ // Get string stream
  istringstream isS{ strValue };
  // Test with string stream
  IntType itV; isS >> noskipws >> itV;
  // Return if succeeded
  return isS.eof() && !isS.fail();
}
/* -- Return if specified string is a valid float -------------------------- */
static bool StrIsFloat(const string &strValue)
  { return StrIsInt<double>(strValue); }
/* -- Return true if string is a value number to the power of 2 ------------ */
static bool StrIsNumPOW2(const string &strValue)
  { return !strValue.empty() &&
      StdIntIsPOW2(StdAbsolute(StrToNum(strValue))); }
/* -- Convert error number to string --------------------------------------- */
static const string StrFromErrNo(const int iErrNo=errno)
{ // Buffer to store error message into
  string strErr; strErr.resize(128);
  // Windows?
#if defined(WINDOWS)
  // 'https://msdn.microsoft.com/en-us/library/51sah927.aspx' says:
  // "Your string message can be, at most, 94 characters long."
  if(strerror_s(const_cast<char*>(strErr.c_str()), strErr.capacity(), iErrNo))
    strErr.assign(StrAppend("Error ", iErrNo));
  // Targeting MacOS?
#elif defined(MACOS)
  // Grab the error result and if failed? Just put in the error number continue
  if(strerror_r(iErrNo, const_cast<char*>(strErr.c_str()), strErr.capacity()))
    strErr.assign(StrAppend("Error ", iErrNo));
  // Linux?
#elif defined(LINUX)
  // Grab the error result and if failed? Set a error and continue
  const char*const cpResult =
    strerror_r(iErrNo, const_cast<char*>(strErr.c_str()), strErr.capacity());
  if(!cpResult) strErr = StrAppend("Error ", iErrNo);
  // We got a message but if was not put in our buffer just return as is
  else if(cpResult != strErr.c_str()) return cpResult;
#endif
  // Resize and compact the buffer
  strErr.resize(strlen(strErr.c_str()));
  strErr.shrink_to_fit();
  // Have to do this because the string is still actually 94 bytes long
  return strErr;
}
/* -- Helper plugin for C runtime errno checking --------------------------- */
struct ErrorPluginStandard final
{ /* -- Exception class helper macro for C runtime errors ------------------ */
#define XCL(r,...) throw Error<ErrorPluginStandard>(r, ## __VA_ARGS__)
  /* -- Constructor to add C runtime error code ---------------------------- */
  explicit ErrorPluginStandard(ostringstream &osS)
    { osS << "\n+ Reason<" << StdGetError() << "> = \""
          << StrFromErrNo() << "\"."; }
};/* ----------------------------------------------------------------------- */
/* -- Convert special formatted string to unix timestamp ------------------- */
static StdTimeT StrParseTime2(const string &strS)
{ // Time structure
  StdTMStruct tData;
  // Scan timestamp into time structure (Don't care about day name). We'll
  // store the timezone in tm_isdst and we'll optimise this by storing the
  // month string in the actual month integer var (4 or 8 bytes so safe).
  // Fmt: %3s %3s %02d %02d:%02d:%02d %05d %04d
  // Test example to just quickly copy and paste in the engine...
  // lexec 'Console.Write(Util.ParseTime2("Mon Mar 14 00:00:00 -0800 2017"));'
  istringstream isS{ strS };
  isS >> get_time(&tData, "%a %b %d %T");
  if(isS.fail()) return 0;
  isS >> tData.tm_wday;
  isS >> get_time(&tData, "%Y");
  if(isS.fail()) return 0;
  // No daylight savings
  tData.tm_isdst = 0;
  // Return timestamp and adjust for specified timezone if neccesary
  return StdMkTime(&tData) + (!tData.tm_wday ? 0 : ((tData.tm_wday < 0 ?
    ((tData.tm_wday % 100) * 60) : -((tData.tm_wday % 100) * 60)) +
    ((tData.tm_wday / 100) * 3600)));
}
/* -- Convert ISO 8601 string to unix timestamp ---------------------------- */
static StdTimeT StrParseTime(const string &strS,
  const char*const cpF="%Y-%m-%dT%TZ")
{ // Time structure
  StdTMStruct tData;
  // Create static input stringstream (safe and fast in c++11)
  istringstream isS{ strS };
  // Scan timestamp into time structure
  isS >> get_time(&tData, cpF);
  if(isS.fail()) return 0;
  // Fill in other useless junk in the struct
  tData.tm_isdst = 0;
  // Return timestamp
  return StdMkTime(&tData);
}
/* -- Convert writable reference string to uppercase ----------------------- */
static string &StrToUpCaseRef(string &strStr)
{ // If string is not empty
  if(!strStr.empty())
    StdTransform(par_unseq, strStr.begin(), strStr.end(), strStr.begin(),
      [](unsigned char ucChar) { return StdToUpper(ucChar); });
  // Return output
  return strStr;
}
/* -- Convert writable referenced string to lowercase ---------------------- */
static string &StrToLowCaseRef(string &strStr)
{ // If string is not empty
  if(!strStr.empty())
    StdTransform(par_unseq, strStr.begin(), strStr.end(), strStr.begin(),
      [](unsigned char ucChar) { return StdToLower(ucChar); });
  // Return output
  return strStr;
}
/* -- Convert string to upper case ----------------------------------------- */
static const string StrToUpCase[[maybe_unused]](const string &strSrc)
{ // String empty? Return a blank one
  if(strSrc.empty()) return {};
  // Create memory for destination string and copy the string over
  string strDst; strDst.reserve(strSrc.size());
  transform(strSrc.begin(), strSrc.end(), back_inserter(strDst),
    [](unsigned char ucChar) { return StdToUpper(ucChar); });
  // Return result
  return strDst;
}
/* -- Convert string to lower case ----------------------------------------- */
static const string StrToLowCase[[maybe_unused]](const string &strSrc)
{ // String empty? Return a blank one
  if(strSrc.empty()) return {};
  // Prepare destination string and run a transform to lowercase each char
  string strDst; strDst.reserve(strSrc.size());
  transform(strSrc.begin(), strSrc.end(), back_inserter(strDst),
    [](unsigned char ucChar) { return StdToLower(ucChar); });
  // Return result
  return strDst;
}
/* -- Basic multiple replace of text in string ----------------------------- */
template<class ListType=StrPairList>
  static string &StrReplaceEx(string &strDest, const ListType &ltList)
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
      const string &strWhat = ltiItem.first;
      // Last cut position and current character index
      if(strncmp(strDest.c_str()+stPos, strWhat.data(), strWhat.length()))
        continue;
      // Get string to replace with
      const string &strWith = ltiItem.second;
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
static string &StrReplace(string &strStr, const char cWhat, const char cWith)
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
static string StrReplace(const string &strStr, const char cWhat,
  const char cWith)
    { string strDst{ strStr }; return StrReplace(strDst, cWhat, cWith); }
/* ------------------------------------------------------------------------- */
static string &StrReplace(string &strDest, const string &strWhat,
  const string &strWith)
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
static string StrReplace(const string &strIn, const string &strWhat,
  const string &strWith)
    { string strOut{ strIn }; return StrReplace(strOut, strWhat, strWith); }
/* -- Replace all occurences of whitespace with plus ----------------------- */
static const string StrUrlEncodeSpaces(const string &strText)
  { return StrReplace(strText, ' ', '+'); }
/* ------------------------------------------------------------------------- */
static const string &StrIsBlank(const string &strIn, const string &strAlt)
  { return strIn.empty() ? strAlt : strIn; }
/* ------------------------------------------------------------------------- */
static const string &StrIsBlank(const string &strIn)
  { return StrIsBlank(strIn, cCommon->CommonBlank()); }
/* ------------------------------------------------------------------------- */
template<typename IntType>
  static const char *StrCPluralise(const IntType itCount,
    const char*const cpSingular, const char*const cpPlural)
      { return itCount == 1 ? cpSingular : cpPlural; }
/* ------------------------------------------------------------------------- */
template<typename IntType>
  static const string StrCPluraliseNum(const IntType itCount,
    const char *cpSingular, const char *cpPlural)
      { return StrAppend(itCount, ' ',
          StrCPluralise<IntType>(itCount, cpSingular, cpPlural)); }
/* ------------------------------------------------------------------------- */
template<typename IntType>
  static const string StrCPluraliseNumEx(const IntType itCount,
    const char *cpSingular, const char *cpPlural)
      { return StrAppend(StrReadableFromNum(itCount), ' ',
          StrCPluralise<IntType>(itCount, cpSingular, cpPlural)); }
/* -- Convert time to long duration ---------------------------------------- */
static const string StrLongFromDuration(const StdTimeT tDuration,
  unsigned int uiCompMax = StdMaxUInt)
{ // Time buffer
  StdTMStruct tD;
  // Lets convert the duration as a time then it will be properly formated
  // in terms of leap years, proper days in a month etc.
  StdGMTime(&tD, &tDuration);
  // Output string
  ostringstream osS;
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
static const char *StrGetPositionSuffix(const uint64_t qPosition)
{ // Get value as base 100
  const uint64_t qVb100 = qPosition % 100;
  // Number not in teens? Compare value as base 10 instead
  if(qVb100 <= 10 || qVb100 >= 20) switch(qPosition % 10)
  { case 1: return "st"; case 2: return "nd"; case 3: return "rd";
    default: break;
  } // Everything else is 'th'
  return "th";
} /* -- Get position of number as a string --------------------------------- */
static const string StrFromPosition(const uint64_t qPosition)
  { return StrAppend(qPosition, StrGetPositionSuffix(qPosition)); }
/* -- Capitalise a string -------------------------------------------------- */
static const string StrCapitalise(const string &strStr)
{ // Capitalise first character if string not nullptr or empty
  if(strStr.empty()) return strStr;
  // Duplicate the string anad uppercase the first character
  string strNew{ strStr };
  strNew[0] = StdToUpper<char>(strStr.front());
  // Return provided string
  return strNew;
}
/* -- Evaluate a list of booleans and return a character value ------------- */
namespace StrFromEvalTokensPrivateData
{ // The private pair and vector types used in the function
  typedef pair<const bool, const char> BoolCharPair;
  typedef vector<BoolCharPair> BoolCharPairVector;
  // The actual function
  static const string StrFromEvalTokens(const BoolCharPairVector &bcpvList)
    { return bcpvList.empty() ? cCommon->CommonBlank() :
        accumulate(bcpvList.cbegin(), bcpvList.cend(), cCommon->CommonBlank(),
          [](const string &strOut, const BoolCharPair &bcpPair)
            { return bcpPair.first ? StrAppend(strOut,
              bcpPair.second) : strOut; }); }
} // Invoke the function in the IUtil namespace
using StrFromEvalTokensPrivateData::StrFromEvalTokens;
/* -- Convert time to short duration --------------------------------------- */
static const string StrShortFromDuration(const double dDuration,
  const int iPrecision=6)
{ // Output string
  ostringstream osS;
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
static const string &StrFromBoolTF(const bool bCondition)
  { return bCondition ? cCommon->CommonTrue() : cCommon->CommonFalse(); }
static const string &StrFromBoolYN(const bool bCondition)
  { return bCondition ? cCommon->CommonYes() : cCommon->CommonNo(); }
/* -- Count occurence of string -------------------------------------------- */
static size_t StrCountOccurences(const string &strStr, const string &strWhat)
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
  static const string StrImplode(const AnyArray &aArray,
    const ssize_t &sstBegin=0, const string &strSep=cCommon->CommonSpace())
{ // Cast array size to ssize_t
  const ssize_t sstSize = static_cast<ssize_t>(aArray.size());
  // Done if empty or begin position is invalid
  if(aArray.empty() || sstBegin >= sstSize) return {};
  // Create output only string stream which stays cached (safe in c++11)
  ostringstream osS;
  // How many items do we have? Have more than 1?
  if(sstSize - sstBegin != 1)
  { // Build command string from vector
    copy(next(aArray.cbegin(), sstBegin), prev(aArray.cend(), 1),
      ostream_iterator<CtrType>(osS, strSep.c_str()));
    // Add final item
    osS << *aArray.crbegin();
  } // Just access the one directly
  else osS << *next(aArray.cbegin(), sstBegin);
  // Done
  return osS.str();
}
/* -- Converts the key/value pairs to a stringvector ----------------------- */
static const string ImplodeMap(const StrNCStrMap &ssmSrc,
  const string &strLineSep=cCommon->CommonSpace(),
  const string &strKeyValSep=cCommon->CommonEquals(),
  const string &strValEncaps="\"")
{ // Done if empty
  if(ssmSrc.empty()) return {};
  // Make string vector to implode and reserve memory for items.
  // Insert each value in the map with the appropriate seperators.
  StrVector svRet; svRet.reserve(ssmSrc.size());
  transform(ssmSrc.cbegin(), ssmSrc.cend(), back_inserter(svRet),
    [&strKeyValSep, &strValEncaps](const StrNCStrMapPair &sncsmpPair)
      { return StrAppend(sncsmpPair.first, strKeyValSep,
          strValEncaps, sncsmpPair.second, strValEncaps); });
  // Return vector imploded into a string
  return StrImplode(svRet, 0, strLineSep);
}
/* ------------------------------------------------------------------------- */
template<typename AnyType>
  static const string StrPrefixPosNeg(const AnyType atVal,
    const int iPrecision)
      { return StrAppend(showpos, fixed, setprecision(iPrecision), atVal); }
/* ------------------------------------------------------------------------- */
template<typename AnyType>
  static const string StrPrefixPosNegReadable(const AnyType atVal,
    const int iPrecision)
      { return StrAppendImbue(showpos, fixed, setprecision(iPrecision),
          atVal); }
/* ------------------------------------------------------------------------- */
template<typename OutType, typename InType, class SuffixClass>
  static OutType StrToReadableSuffix(const InType itValue,
    const char**const cpSuffix, int &iPrecision, const SuffixClass &scLookup,
    const char*const cpDefault)
{ // Check types
  static_assert(is_floating_point_v<OutType>, "OutType not floating point!");
  static_assert(is_integral_v<InType>, "InType not integral!");
  static_assert(is_class_v<SuffixClass>, "Class invalid!");
  // Value to return
  OutType otReturn;
  // Discover the best measurement to show by testing each unit from the
  // lookup table to see if it is divisible and if it is not then try the next
  // one. The 'auto' is required because 'SuffixClass' is different array type.
  if(!any_of(scLookup.cbegin(), scLookup.cend(),
    [&otReturn, itValue, &cpSuffix](const auto &aItem)
  { // Calculate best measurement to show
    if(itValue < aItem.vValue) return false;
    // Set suffix that was sent and return success
    *cpSuffix = aItem.cpSuf;
    otReturn = static_cast<OutType>(itValue) / aItem.vValue;
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
    static const array<const ByteValue,6> bvLookup{ {
      { 0x1000000000000000, "EB" }, { 0x0004000000000000, "PB" },
      { 0x0000010000000000, "TB" }, { 0x0000000040000000, "GB" },
      { 0x0000000000100000, "MB" }, { 0x0000000000000400, "KB" }
    } };
    // Return result
    return StrToReadableSuffix<double>(itBytes,
      cpSuffix, iPrecision, bvLookup, "B");
  } // If input value is 32-bit?
  else if constexpr(sizeof(IntType) == sizeof(uint32_t))
  { // Tests lookup table. This is all we can fit in a 32-bit integer
    static const array<const ByteValue,3> bvLookup{ {
      { 0x40000000, "GB" }, { 0x00100000, "MB" }, { 0x00000400, "KB" }
    } };
    // Return result
    return StrToReadableSuffix<double>(itBytes,
      cpSuffix, iPrecision, bvLookup, "B");
  } // If input value is 16-bit?
  else if constexpr(sizeof(IntType) == sizeof(uint16_t))
  { // Tests lookup table. This is all we can fit in a 16-bit integer
    static const array<const ByteValue,1> bvLookup{ { { 0x0400, "KB" } } };
    // Return result
    return StrToReadableSuffix<double>(itBytes,
      cpSuffix, iPrecision, bvLookup, "B");
  } // Input value is not 64, 32 nor 16 bit? Use a empty table
  static const array<const ByteValue,0> bvLookup{ { } };
  // Show error
  return StrToReadableSuffix<double>(itBytes,
    cpSuffix, iPrecision, bvLookup, "B");
}
/* ------------------------------------------------------------------------- */
template<typename IntType>
  static const string StrToBytes(const IntType itBytes, int iPrecision=2)
{ // Process a human readable value for the specified number of bytes
  const char *cpSuffix = nullptr;
  const double dVal =
    StrToBytesHelper<IntType>(itBytes, &cpSuffix, iPrecision);
  // Move the stringstreams output string into the return value.
  return StrAppend(fixed, setprecision(iPrecision), dVal, cpSuffix);
}
/* ------------------------------------------------------------------------- */
template<typename IntType>
  static const string StrToReadableBytes(const IntType itBytes,
  int iPrecision=2)
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
    static const array<const BitValue,6> bvLookup{ {
      { 1000000000000000000, "Eb" }, { 1000000000000000, "Pb" },
      {       1000000000000, "Tb" }, {       1000000000, "Gb" },
      {             1000000, "Mb" }, {             1000, "Kb" },
    } };
    // Return result
    return StrToReadableSuffix<double>(itBits,
      cpSuffix, iPrecision, bvLookup, "b");
  } // If input value is 32-bit?
  else if constexpr(sizeof(IntType) == sizeof(uint32_t))
  { // Tests lookup table. This is all we can fit in a 32-bit integer.
    static const array<const BitValue,3> bvLookup{ {
      { 1000000000, "Gb" }, { 1000000, "Mb" }, { 1000, "Kb" },
    } };
    // Return result
    return StrToReadableSuffix<double>(itBits,
      cpSuffix, iPrecision, bvLookup, "b");
  } // If input value is 16-bit?
  else if constexpr(sizeof(IntType) == sizeof(uint16_t))
  { // Tests lookup table. This is all we can fit in a 16-bit integer.
    static const array<const BitValue,6> bvLookup{ { { 1000, "Kb" } } };
    // Return result
    return StrToReadableSuffix<double>(itBits,
      cpSuffix, iPrecision, bvLookup, "b");
  } // Input value is not 64, 32 nor 16 bit? Use a empty table
  static const array<const BitValue,0> bvLookup{ { } };
  // Show error
  return StrToReadableSuffix<double>(itBits,
    cpSuffix, iPrecision, bvLookup, "b");
}
/* ------------------------------------------------------------------------- */
template<typename IntType>
  static const string StrToBits(const IntType itBits, int iPrecision=2)
{ // Process a human readable value for the specified number of bits
  const char *cpSuffix = nullptr;
  const double dVal =
    StrToReadableBitsHelper<IntType>(itBits, &cpSuffix, iPrecision);
  // Move the stringstreams output string into the return value.
  return StrAppend(fixed, setprecision(iPrecision), dVal, cpSuffix);
}
/* ------------------------------------------------------------------------- */
template<typename IntType>
  static const string StrToReadableBits(const IntType itBits, int iPrecision)
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
    static const array<const Value,4> vLookup{ {
      { 1000000000000, "T" }, { 1000000000, "B" },
      { 1000000,       "M" }, { 1000,       "K" }
    } };
    // Return result
    return StrToReadableSuffix<double>(itValue, cpSuffix, iPrecision, vLookup);
  } // If input value is 32-bit?
  else if constexpr(sizeof(IntType) == sizeof(uint32_t))
  { // Tests lookup table. This is all we can fit in a 64-bit integer.
    static const array<const Value,3> vLookup{ {
      { 1000000000, "B" }, { 1000000, "M" }, { 1000, "K" }
    } };
    // Return result
    return StrToReadableSuffix<double>(itValue, cpSuffix, iPrecision, vLookup);
  } // If input value is 16-bit?
  else if constexpr(sizeof(IntType) == sizeof(uint16_t))
  { // Tests lookup table. This is all we can fit in a 64-bit integer.
    static const array<const Value,1> vLookup{ { { 1000, "K" } } };
    // Return result
    return StrToReadableSuffix<double>(itValue, cpSuffix, iPrecision, vLookup);
  } // Input value is not 64, 32 nor 16 bit? Use a empty table
  static const array<const Value,0> vLookup{ { } };
  // Show error
  return StrToReadableSuffix<double>(itValue, cpSuffix, iPrecision, vLookup);
}
/* ------------------------------------------------------------------------- */
template<typename IntType>
  static const string StrToGrouped(const IntType itValue, int iPrecision=2)
{ // Process a human readable value for the specified number of bits
  const char *cpSuffix = nullptr;
  const double dVal =
    StrToReadableHelper<IntType>(itValue, &cpSuffix, iPrecision);
  // Move the stringstreams output string into the return value.
  return StrAppend(fixed, setprecision(iPrecision), dVal, cpSuffix);
}
/* ------------------------------------------------------------------------- */
template<typename IntType>
  static const string StrToReadableGrouped(const IntType itValue,
    int iPrecision)
{ // Process a human readable value for the specified number of bits
  const char *cpSuffix = nullptr;
  const double dVal =
    StrToReadableHelper<IntType>(itValue, &cpSuffix, iPrecision);
  // Move the FORMATTED stringstreams output string into the return value.
  return StrAppendImbue(fixed, setprecision(iPrecision), dVal, cpSuffix);
}
/* ------------------------------------------------------------------------- */
static size_t StrFindCharForwards(const string &strS, size_t stStart,
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
static size_t StrFindCharBackwards[[maybe_unused]](const string &strS,
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
static size_t StrFindCharNotForwards[[maybe_unused]](const string &strS,
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
static size_t StrFindCharNotForwards(const string &strS, size_t stStart,
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
static size_t StrFindCharNotBackwards[[maybe_unused]](const string &strS,
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
static size_t StrFindCharNotBackwards(const string &strS, size_t stStart,
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
static const string StrFromTimeTM(const StdTMStruct &tmData,
  const char*const cpF) { return StrAppend(put_time(&tmData, cpF)); }
/* -- Convert specified timestamp to string -------------------------------- */
static const string StrFromTimeTT(const StdTimeT ttTimestamp,
  const char*const cpFormat = cpTimeFormat)
{ // Convert it to local time in a structure
  StdTMStruct tmData; StdLocalTime(&tmData, &ttTimestamp);
  // Do the parse and return the string
  return StrFromTimeTM(tmData, cpFormat);
}
/* -- Remove suffixing carriage return and line feed ----------------------- */
static string &StrChop(string &strStr)
{ // Find the pos of the last char that is not a carriage return or line feed
  const size_t stEndPos = strStr.find_last_not_of(cCommon->CommonCrLf());
  // If all characters are removed, set the string to empty else erase the part
  if(stEndPos == StdNPos) strStr.clear();
  else strStr.erase(stEndPos + 1);
  // Return the modified string
  return strStr;
}
/* -- Convert specified timestamp to string (UTC) -------------------------- */
static const string StrFromTimeTTUTC(const StdTimeT ttTimestamp,
  const char*const cpFormat = cpTimeFormat)
{ // Convert it to local time
  StdTMStruct tmData; StdGMTime(&tmData, &ttTimestamp);
  // Do the parse and return the string
  return StrFromTimeTM(tmData, cpFormat);
}
/* ------------------------------------------------------------------------- */
template<typename IntType>
  string StrFromRatio(const IntType itAntecedent, const IntType itConsequent)
{ // Return failure if parameters negative or zero
  if(itAntecedent <= 0 || itConsequent <= 0) return "N/A";
  // If we're a number, we need to convert it to an integer or gcd() fails
  if constexpr(is_floating_point_v<IntType>)
    return StrFromRatio(static_cast<unsigned int>(itAntecedent),
                        static_cast<unsigned int>(itConsequent));
  // Integral?
  else
  { // Calculate the greatest common divisor
    const IntType itGCD = gcd(itAntecedent, itConsequent),
    // Calculate the simplified ratio
                  itNum = itAntecedent / itGCD,
                  itDen = itConsequent / itGCD;
    // Return the ratio as a string
    return StrAppend(itNum, ':', itDen);
  }
}
/* -- Convert list to exploded string -------------------------------------- */
template<class ListType>
  string StrExplodeEx(const ListType &lType, const string &strSep,
    const string &strLast)
{ // String to return
  ostringstream ossOut;
  // What is the size of this string
  switch(lType.size())
  { // Empty list? Just break to return empty string
    case 0: break;
    // Only one? Just return the string directly
    case 1: return *lType.cbegin();
    // Two? Return a simple appendage.
    case 2: ossOut << *lType.cbegin() << strLast << *lType.crbegin(); break;
    // More than two? Write the first item first
    default: ossOut << *lType.cbegin();
             // Container type
             typedef typename ListType::value_type ListTypeValue;
             // Write the rest but one prefixed with the separator
             StdForEach(seq,
               next(lType.cbegin(), 1), next(lType.crbegin(), 1).base(),
                 [&ossOut, &strSep](const ListTypeValue &strStr)
                   { ossOut << strSep << strStr; });
             // and now append the last separator and string from list
             ossOut << strLast << *lType.crbegin();
             // Done
             break;
  } // Return the compacted string
  return ossOut.str();
}
/* -- Compact a string removing leading, trailing and duplicate spaces ----- */
static string &StrCompactRef(string &strStr)
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
static const string StrCompact(const char*cpStr)
{ // Ignore if empty
  if(UtfIsCStringNotValid(cpStr)) return {};
  // Convert to string, compact it and return it
  string strOut{ cpStr };
  StrCompactRef(strOut);
  return strOut;
}
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
