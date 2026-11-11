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
