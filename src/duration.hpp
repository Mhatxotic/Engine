/* == DURATION.HPP ========================================================= **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## Provides the Duration class which parses a time duration            ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IDuration {                  // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace ICommon::P;
using namespace IStd::P;               using namespace IStdLib::P;
using namespace IString::P;
/* -- Public functions ----------------------------------------------------- */
namespace P {                          // Start of public module namespace
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
  /* -- Constructor that initialises singular and plural strings --- */ public:
  Duration(
    /* -- Arguments -------------------------------------------------------- */
    const StrType &strYearN,  const StrType &strYearsN,  // Year    / Years
    const StrType &strMonthN, const StrType &strMonthsN, // Month   / Months
    const StrType &strDayN,   const StrType &strDaysN,   // Day     / Days
    const StrType &strHourN,  const StrType &strHoursN,  // Hour    / Hours
    const StrType &strMinN,   const StrType &strMinsN,   // Minute  / Minutes
    const StrType &strSecN,   const StrType &strSecsN,   // Second  / Seconds
    const StrType &strMSecN,  const StrType &strMSecsN,  // Msecond / Mseconds
    const StrType &strUSecN,  const StrType &strUSecsN,  // USecond / USeconds
    const StrType &strNSecN,  const StrType &strNSecsN,  // NSecond / NSeconds
    const StrType &strPSecN,  const StrType &strPSecsN   // PSecond / PSeconds
    ):/* -- Initialisers --------------------------------------------------- */
    strYear{ strYearN },   strYears{ strYearsN },   // Set year(s) string
    strMonth{ strMonthN }, strMonths{ strMonthsN }, // Set month(s) string
    strDay{ strDayN },     strDays{ strDaysN },     // Set day(s) string
    strHour{ strHourN },   strHours{ strHoursN },   // Set hour(s) string
    strMin{ strMinN },     strMins{ strMinsN },     // Set minute(s) string
    strSec{ strSecN },     strSecs{ strSecsN },     // Set second(s) string
    strMSec{ strMSecN },   strMSecs{ strMSecsN },   // Set millisec(s) string
    strUSec{ strUSecN },   strUSecs{ strUSecsN },   // Set microsec(s) string
    strNSec{ strNSecN },   strNSecs{ strNSecsN },   // Set nanosecond(s) string
    strPSec{ strPSecN },   strPSecs{ strPSecsN }    // Set picosecond(s) string
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- Constructor that initialises singular strings ---------------------- */
  Duration(
    /* -- Arguments -------------------------------------------------------- */
    const StrType &strYearN,           // Year singular string
    const StrType &strMonthN,          // Month singular string
    const StrType &strDayN,            // Day singular string
    const StrType &strHourN,           // Hour singular string
    const StrType &strMinN,            // Minute singular string
    const StrType &strSecN,            // Second singular string
    const StrType &strMSecN,           // Millisecond singular string
    const StrType &strUSecN,           // Microsecond singular string
    const StrType &strNSecN,           // Nanosecond singular string
    const StrType &strPSecN            // Picosecond singular string
    ):/* -- Initialiser that calls the above constructor ------------------- */
    Duration{ strYearN, strYearN, strMonthN, strMonthN, strDayN, strDayN,
      strHourN, strHourN, strMinN, strMinN, strSecN, strSecN, strMSecN,
      strMSecN, strUSecN, strUSecN, strNSecN, strNSecN, strPSecN, strPSecN }
    /* -- No code ---------------------------------------------------------- */
    {}
};/* ----------------------------------------------------------------------- */
const Duration<> durLong{ " year", " years", " month", " months", " day",
  " days", " hour", " hours", " minute", " minutes", " second", " seconds",
  " millisecond", " milliseconds", " microsecond", " microseconds",
  " nanosecond", " nanoseconds ", " picosecond", " picoseconds" },
/* ------------------------------------------------------------------------- */
durShort{ "y", "mo", "d", "h", "m", "s", "ms", "µs", "ns", "ps" };
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
