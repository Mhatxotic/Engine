/* == CLOCK.HPP ============================================================ **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## Defines a class that makes it easier to interact with the high      ## **
** ## resolution and system clock.                                        ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IClock {                     // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IDuration::P;          using namespace IStd::P;
using namespace IStdLib::P;            using namespace ITime::P;
using namespace IUtil::P;              using namespace ::std::chrono;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* -- Dependencies --------------------------------------------------------- */
using ::std::chrono::duration;         using ::std::chrono::microseconds;
using ::std::chrono::milliseconds;     using ::std::chrono::nanoseconds;
using ::std::chrono::seconds;
/* ------------------------------------------------------------------------- */
using ClkTimePoint      = high_resolution_clock::time_point; // Holds a time
using ClkDuration       = high_resolution_clock::duration;   // Holds duration
using AtomicClkDuration = StdAtomic<ClkDuration>;            // Thread safe
/* -- Common duration values ----------------------------------------------- */
constexpr static const ClkDuration
  cd0{ nanoseconds{ 0 } },             cd1MS{ milliseconds{ 1 } },
  cd10MS{ milliseconds{ 10 } },        cd100MS{ milliseconds{ 100 } },
  cd1S{ seconds{ 1 } },                cd60S{ seconds{ 60 } };
/* -- Get count from a duration -------------------------------------------- */
template<typename DurationType>
  static auto ClockGetCount(const ClkDuration &cdDuration)
{ return duration_cast<DurationType>(cdDuration).count(); }
/* -- Convert duration to double ------------------------------------------- */
static double ClockDurationToDouble(const ClkDuration &cdDuration)
  { return ClockGetCount<duration<double>>(cdDuration); }
/* -- Subtract one timepoint from the other and return as double ----------- */
static double ClockTimePointRangeToDouble
  (const ClkTimePoint &ctpEnd, const ClkTimePoint &ctpStart)
{ return ClockDurationToDouble(ctpEnd - ctpStart); }
/* -- Subtract one timepoint from the other and return as clamped double --- */
static double ClockTimePointRangeToClampedDouble
  (const ClkTimePoint &ctpEnd, const ClkTimePoint &ctpStart)
{ return UtilMaximum(ClockTimePointRangeToDouble(ctpEnd, ctpStart), 0.0); }
/* -- Clock ---------------------------------------------------------------- */
template<class ClockType>struct Clock final
{ /* -- Get current time --------------------------------------------------- */
  static auto ClockGetTime() { return ClockType::now(); }
  /* -- Get time since epoch ----------------------------------------------- */
  static ClkDuration ClockGetEpochTime()
    { return ClockGetTime().time_since_epoch(); }
  /* -- Get current time since epoch casted and counted -------------------- */
  template<typename DurationType, typename RIntType>
    RIntType ClockGetTimeEx() const
  { return static_cast<RIntType>(
      ClockGetCount<DurationType>(ClockGetEpochTime())); }
  /* -- Return time as double ---------------------------------------------- */
  static double ClockGetTimeDouble()
    { return ClockDurationToDouble(ClockGetEpochTime()); }
  /* -- Return time since epoch count as integer --------------------------- */
  template<typename IntType = StdTimeT>
    requires StdIsArithmatic<IntType>
  IntType ClockGetTimeSeconds() const
    { return ClockGetTimeEx<duration<IntType>,IntType>(); }
  /* -- Return time in microseconds ---------------------------------------- */
  template<typename IntType = uint64_t>
    requires StdIsArithmatic<IntType>
  IntType ClockGetTimeMicroseconds() const
    { return ClockGetTimeEx<microseconds,IntType>(); }
  /* -- Return time in milliseconds ---------------------------------------- */
  template<typename IntType = uint64_t>
    requires StdIsArithmatic<IntType>
  IntType ClockGetTimeMilliseconds() const
    { return ClockGetTimeEx<milliseconds,IntType>(); }
  /* -- Return time in nanoseconds ----------------------------------------- */
  template<typename IntType = uint64_t>
    requires StdIsArithmatic<IntType>
  IntType ClockGetTimeNanoseconds() const
    { return ClockGetTimeEx<nanoseconds,IntType>(); }
  /* -- Get offset time ---------------------------------------------------- */
  static ClkDuration ClockGetDuration(const ClkTimePoint &ctpCurrent)
    { return ClockGetTime() - ctpCurrent; }
  /* -- Get timepoint count ------------------------------------------------ */
  template<typename IntType>
    auto ClockGetDurationCount(const ClkTimePoint &ctpCurrent) const
  { return ClockGetCount<IntType>(ClockGetDuration(ctpCurrent)); }
  /* -- Convert timepoint to double ---------------------------------------- */
  static double ClockTimePointToDouble(const ClkTimePoint &ctpTime)
    { return ClockDurationToDouble(ClockGetDuration(ctpTime)); }
  /* -- Convert clamped timepoint to double -------------------------------- */
  static double ClockTimePointToClampedDouble(const ClkTimePoint &ctpTime)
    { return UtilMaximum(ClockTimePointToDouble(ctpTime), 0.0); }
  /* -- Convert local time to string --------------------------------------- */
  StdString ClockFormatTime(const char*const cpFormat = caTimeFormat) const
    { return TimeLocalTTtoStr(ClockGetTimeSeconds(), cpFormat); }
  /* -- Convert universal time to string ----------------------------------- */
  StdString ClockFormatTimeUTC(const char*const cpFormat = caTimeFormat) const
    { return TimeUTCTTtoStr(ClockGetTimeSeconds(), cpFormat); }
  /* -- Convert time to short duration ------------------------------------- */
  static StdString ClockToDurationString(unsigned uPrecision = 6)
    { return TimeToShortDuration(ClockGetTimeDouble(), uPrecision); }
  /* -- Convert seconds to long duration relative to current time ---------- */
  StdString ClockToDurationRelative(const StdTimeT tDuration = 0,
    unsigned uCompMax = StdMaxUInt) const
  { return durLong.Parse(ClockGetTimeSeconds() - tDuration, uCompMax); }
  /* -- Convert time to long duration -------------------------------------- */
  StdString ClockToDurationLongStr(unsigned uCompMax = StdMaxUInt) const
    { return ClockToDurationRelative(0, uCompMax); }
  /* -- Unused constructor ------------------------------------------------- */
  Clock() = default;
}; /* -- Clocks we make use of --------------------------------------------- */
static const Clock<system_clock>          cmSys;
static const Clock<high_resolution_clock> cmHiRes;
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
