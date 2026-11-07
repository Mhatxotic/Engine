/* == TIME.HPP ============================================================= **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## Time parsing and formatting related functions.                      ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace ITime {                      // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace ICommon::P;            using namespace IStd::P;
using namespace IStdLib::P;            using namespace IString::P;
/* -- Format time alias ---------------------------------------------------- */
template<typename PtrType>
  requires (!StdIsPointer<PtrType>)
static auto TimeFormat(const StdTMStruct*const stdData,
  const PtrType*const ptFormat)
{ return ::std::put_time(stdData, ptFormat); }
/* -- Parse time alias ----------------------------------------------------- */
template<typename PtrType>
  requires (!StdIsPointer<PtrType>)
static auto TimeParse(StdTMStruct*const stdData, const PtrType*const ptFormat)
  { return ::std::get_time(stdData, ptFormat); }
/* -- Public functions ----------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* -- Some helpful globals so not to repeat anything ----------------------- */
static const char caTimeFormat[] = "%a %b %d %H:%M:%S %Y %z";
/* -- Convert special formatted string to unix timestamp ------------------- */
template<typename StrType>
  requires StdIsString<StrType>
static StdTimeT TimeParseStr2(const StrType &strFormat)
{ // Time structure
  StdTMStruct tmsData;
  // Prepare buffer
  StdStringBuf sbBuffer;
  sbBuffer.sputn(strFormat.data(), static_cast<ssize_t>(strFormat.size()));
  StdIStream isS{ &sbBuffer };
  // Scan timestamp into time structure (Don't care about day name). We'll
  // store the timezone in tm_isdst and we'll optimise this by storing the
  // month string in the actual month integer var (4 or 8 bytes so safe).
  // Fmt: %3s %3s %02d %02d:%02d:%02d %05d %04d
  // Test example to just quickly copy and paste in the engine...
  // lexec 'Console.Write(Util.ParseTime2("Mon Mar 14 00:00:00 -0800 2017"));'
  isS >> TimeParse(&tmsData, "%a %b %d %T");
  if(isS.fail()) return 0;
  isS >> tmsData.tm_wday;
  isS >> TimeParse(&tmsData, "%Y");
  if(isS.fail()) return 0;
  // No daylight savings
  tmsData.tm_isdst = 0;
  // Return timestamp and adjust for specified timezone if neccesary
  return StdMkTime(&tmsData) + (!tmsData.tm_wday ? 0 : ((tmsData.tm_wday < 0 ?
    ((tmsData.tm_wday % 100) * 60) : -((tmsData.tm_wday % 100) * 60)) +
    ((tmsData.tm_wday / 100) * 3600)));
}
/* -- Convert ISO 8601 string to unix timestamp ---------------------------- */
template<typename StrType>
  requires StdIsString<StrType>
static StdTimeT TimeParseStr(const StrType &strFormat,
  const char*const cpFormat = "%Y-%m-%dT%TZ")
{ // Time structure
  StdTMStruct tmsData;
  // Prepare buffer
  StdStringBuf sbBuffer;
  sbBuffer.sputn(strFormat.data(), static_cast<ssize_t>(strFormat.size()));
  StdIStream isS{ &sbBuffer };
  // Scan timestamp into time structure
  isS >> TimeParse(&tmsData, cpFormat);
  if(isS.fail()) return 0;
  // Fill in other useless junk in the struct
  tmsData.tm_isdst = 0;
  // Return timestamp
  return StdMkTime(&tmsData);
}
/* -- Duration functors ---------------------------------------------------- */
template<typename StrType = StdStringView>
  requires StdIsString<StrType>
class Duration
{ /* -- Private variables -------------------------------------------------- */
  const StrType    strYear,  strYears,  // Year (singular) Years (plural)
                   strMonth, strMonths, // Month (singular) Months (plural)
                   strDay,   strDays,   // Day (singular) Days (plural)
                   strHour,  strHours,  // Hour (singular) Hours (plural)
                   strMin,   strMins,   // Minute (singular) Minutes (plural)
                   strSec,   strSecs,   // Second (singular) Seconds (plural)
                   strMSec,  strMSecs,  // Millisecond and milliseconds
                   strUSec,  strUSecs,  // Microsecond and microseconds
                   strNSec,  strNSecs,  // Nanosecond and nanoseconds
                   strPSec,  strPSecs;  // Picosecond and picoseconds
  /* -- Add a space if specified string stream is not empty ---------------- */
  static void Format(StdOStringStream &osS, const auto aValue,
    const StrType &strSingular, const StrType &strPlural)
  { // Add a space if we already wrote something
    osS << (osS.tellp() ? cCommon->CommonSpace() : cCommon->CommonBlank())
        // Add the value
        << aValue
        // Then add singular or plural depending on if it is a 1 or not
        << StrPluraliseRef(aValue, strSingular, strPlural);
  }
  /* -- Helper function for Parse(integer) --------------------------------- */
  static void Cell(StdOStringStream &osS, const int iValue,
    unsigned &uCompMax, const StrType &strSingular, const StrType &strPlural)
  { // Format integral
    Format(osS, iValue, strSingular, strPlural);
    // Reduce allowed number of components
    --uCompMax;
  }
  /* -- Helper function for Parse(double) ---------------------------------- */
  static void Cell(StdOStringStream &osS, const double dDuration,
    const double dPrecision, const StrType &strSingular,
    const StrType &strPlural)
  { // Format into string stream
    Format(osS, static_cast<StdTimeT>(floor(dDuration * dPrecision)),
      strSingular, strPlural);
  }
  /* -- Convert time to long duration ------------------------------ */ public:
  StdString Parse(const StdTimeT strDuration, unsigned uCompMax = StdMaxUInt)
    const
  { // Time buffer
    StdTMStruct tD;
    // Lets convert the duration as a time then str will be properly formated
    // in terms of leap years, proper days in a month etc.
    StdGMTime(&tD, &strDuration);
    // Output string
    StdOStringStream &osS = cCommon->o.StreamReset();
    // If failed? Manually do str
    if(tD.tm_year == -1)
    { // Clear years and months since we can't realiably calculate that.
      tD.tm_year = tD.tm_mon = 0;
      // Set days, hours, minutes and seconds
      tD.tm_mday = static_cast<int>(strDuration / 86400);
      tD.tm_hour = static_cast<int>(strDuration / 3600 % 24);
      tD.tm_min = static_cast<int>(strDuration / 60 % 60);
      tD.tm_sec = static_cast<int>(strDuration % 60);
    } // Succeeded, subtract 70 as str returns as years past 1900.
    else tD.tm_year -= 70;
    // Add years?
    if(tD.tm_year && uCompMax > 0)
      Cell(osS, tD.tm_year, uCompMax, strYear, strYears);
    // Add months?
    if(tD.tm_mon && uCompMax > 0)
      Cell(osS, tD.tm_mon, uCompMax, strMonth, strMonths);
    // Add days? (removing the added 1)
    if(--tD.tm_mday && uCompMax > 0)
      Cell(osS, tD.tm_mday, uCompMax, strDay, strDays);
    // Add hours?
    if(tD.tm_hour && uCompMax > 0)
      Cell(osS, tD.tm_hour, uCompMax, strHour, strHours);
    // Add Minutes?
    if(tD.tm_min && uCompMax > 0)
      Cell(osS, tD.tm_min, uCompMax, strMin, strMins);
    // Check seconds
    if((tD.tm_sec || !strDuration) && uCompMax > 0)
      Cell(osS, tD.tm_sec, uCompMax, strSec, strSecs);
    // Return string
    return osS.str();
  }
  /* -- Convert double time to long duration ------------------------------ */
  StdString Parse(const double dDuration, unsigned uCompMax = StdMaxUInt) const
  { // Output string
    StdOStringStream &osS = cCommon->o.StreamReset();
    // Precision not important?
    if(dDuration >= 60.0)
      return Parse(static_cast<StdTimeT>(dDuration), uCompMax);
    // Add milliseconds?
    if(dDuration >= 0.001)
      Cell(osS, dDuration, 1000.0, strMSec, strMSecs);
    // Add microseconds?
    else if(dDuration >= 0.000001)
      Cell(osS, dDuration, 1000000.0, strUSec, strUSecs);
    // Add nanoseconds?
    else if(dDuration >= 0.000000001)
      Cell(osS, dDuration, 1000000000.0, strNSec, strNSecs);
    // Add picoseconds?
    else if(dDuration >= 0.000000000001)
      Cell(osS, dDuration, 1000000000000.0, strPSec, strPSecs);
    // Return string
    return osS.str();
  }
  /* --------------------------------------------------------------- */ public:
  Duration(const StrType &strNYear, const StrType &strNYears,
    const StrType &strNMonth, const StrType &strNMonths,
    const StrType &strNDay, const StrType &strNDays, const StrType &strNHour,
    const StrType &strNHours, const StrType &strNMin, const StrType &strNMins,
    const StrType &strNSec, const StrType &strNSecs, const StrType &strNMSec,
    const StrType &strNMSecs, const StrType &strNUSec,
    const StrType &strNUSecs, const StrType &strNNSec,
    const StrType &strNNSecs, const StrType &strNPSec,
    const StrType &strNPSecs) :
      strYear{ strNYear }, strYears{ strNYears }, strMonth{ strNMonth },
      strMonths{ strNMonths }, strDay{ strNDay }, strDays{ strNDays },
      strHour{ strNHour }, strHours{ strNHours }, strMin{ strNMin },
      strMins{ strNMins }, strSec{ strNSec }, strSecs{ strNSecs },
      strMSec{ strNMSec }, strMSecs{ strNMSecs }, strUSec{ strNUSec },
      strUSecs{ strNUSecs }, strNSec{ strNNSec }, strNSecs{ strNNSecs },
      strPSec{ strNPSec }, strPSecs{ strNPSecs }
  /* ----------------------------------------------------------------------- */
  {}
  /* ----------------------------------------------------------------------- */
  Duration(const StrType &strNYear, const StrType &strNMonth,
    const StrType &strNDay, const StrType &strNHour, const StrType &strNMin,
    const StrType &strNSec, const StrType &strNMSec, const StrType &strNUSec,
    const StrType &strNNSec, const StrType &strNPSec) :
      Duration{ strNYear, strNYear, strNMonth, strNMonth, strNDay, strNDay,
        strNHour, strNHour, strNMin, strNMin, strNSec, strNSec, strNMSec,
        strNMSec, strNUSec, strNUSec, strNNSec, strNNSec, strNPSec, strNPSec }
  /* ----------------------------------------------------------------------- */
  {}
};/* ----------------------------------------------------------------------- */
const Duration<> durLong{ " year", " years", " month", " months", " day",
  " days", " hour", " hours", " minute", " minutes", " second", " seconds",
  " millisecond", " milliseconds", " microsecond", " microseconds",
  " nanosecond", " nanoseconds ", " picosecond", " picoseconds" },
/* ------------------------------------------------------------------------- */
durShort{ "y", "mo", "d", "h", "m", "s", "ms", "µs", "ns", "ps" };
/* -- Do convert the specified structure to string ------------------------= */
static StdString TimeTMToStr(const StdTMStruct &tmsData,
  const char*const cpFormat)
{ return StrAppend(TimeFormat(&tmsData, cpFormat)); }
/* -- Convert specified timestamp to string -------------------------------- */
static StdString TimeLocalTTtoStr(const StdTimeT ttTimestamp,
  const char*const cpFormat = caTimeFormat)
{ // Convert it to local time in a structure, do the parse and return string
  StdTMStruct tmsData;
  StdLocalTime(&tmsData, &ttTimestamp);
  return TimeTMToStr(tmsData, cpFormat);
}
/* -- Convert specified timestamp to string (UTC) -------------------------- */
static StdString TimeUTCTTtoStr(const StdTimeT ttTimestamp,
  const char*const cpFormat = caTimeFormat)
{ // Convert it to local time, do the parse and return the string
  StdTMStruct tmsData;
  StdGMTime(&tmsData, &ttTimestamp);
  return TimeTMToStr(tmsData, cpFormat);
}
/* -- Convert time to short duration --------------------------------------- */
static StdString TimeToShortDuration(const double dDuration,
  const int iPrecision = 6)
{ // Output string
  StdOStringStream &osS = cCommon->o.StreamReset();
  // Get duration ceiled and if negative?
  double dInt, dFrac = modf(dDuration, &dInt);
  if(dInt < 0)
  { // Set negative symbol and negate the duration
    osS << '-';
    dInt = -dInt;
    dFrac = -dFrac;
  } // Set floating point precision with zero fill
  osS << StdIOSFixed << StdIOSSetFill('0') << StdIOSSetPrecision(0);
  // Have days?
  if(dInt >= 86400)
    osS <<                           floor(dInt / 86400)     << ':'
        << StdIOSSetWidth(2) << fmod(floor(dInt / 3600), 24) << ':'
        << StdIOSSetWidth(2) << fmod(floor(dInt / 60),   60) << ':'
        << StdIOSSetWidth(2);
  // No days, but hours?
  else if(dInt >= 3600)
    osS <<                      fmod(floor(dInt / 3600), 24) << ':'
        << StdIOSSetWidth(2) << fmod(floor(dInt / 60),   60) << ':'
        << StdIOSSetWidth(2);
  // No hours, but minutes?
  else if(dInt >= 60)
    osS << fmod(floor(dInt/60), 60) << ':' << StdIOSSetWidth(2);
  // No minutes so no zero padding
  else osS << StdIOSSetWidth(0);
  // On the seconds part, we have a problem where having a precision
  // of zero is causing stringstream to round so we'll just convert it to an
  // int instead to fix it.
  osS << fmod(dInt, 60);
  if(iPrecision > 0)
    osS << '.' << StdIOSSetWidth(iPrecision) <<
      static_cast<unsigned>(fabs(dFrac) * pow(10.0, iPrecision));
  // Return string
  return osS.str();
}
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
