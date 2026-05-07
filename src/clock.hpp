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
using namespace IStd::P;               using namespace IStdLib::P;
using namespace ITime::P;              using namespace IUtil::P;
/* ------------------------------------------------------------------------- */
using CoreClock = ::std::chrono::high_resolution_clock; // Default clock is HRC
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* -- Dependencies --------------------------------------------------------- */
using ::std::chrono::duration_cast;    using ::std::chrono::duration;
using ::std::chrono::microseconds;     using ::std::chrono::milliseconds;
using ::std::chrono::nanoseconds;      using ::std::chrono::seconds;
using ::std::chrono::system_clock;
/* -- Typedefs ------------------------------------------------------------- */
using ClkTimePoint      = CoreClock::time_point;  // Holds a time
using ClkDuration       = CoreClock::duration;    // Holds a duration
using AtomicClkDuration = StdAtomic<ClkDuration>; // Thread safe duration
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
  static auto GetTime() { return ClockType::now(); }
  /* -- Get time since epoch ----------------------------------------------- */
  static ClkDuration GetEpochTime() { return GetTime().time_since_epoch(); }
  /* -- Get current time since epoch casted and counted -------------------- */
  template<typename DurationType, typename RIntType>
    RIntType GetTimeEx() const
  { return static_cast<RIntType>(
      ClockGetCount<DurationType>(GetEpochTime())); }
  /* -- Return time as double ---------------------------------------------- */
  static double GetTimeDouble()
    { return ClockDurationToDouble(GetEpochTime()); }
  /* -- Return time since epoch count as integer --------------------------- */
  template<typename IntType = StdTimeT> requires StdIsArithmatic<IntType>
    IntType GetTimeS() const
  { return GetTimeEx<duration<IntType>,IntType>(); }
  /* -- Return time in microseconds ---------------------------------------- */
  template<typename IntType = uint64_t> requires StdIsArithmatic<IntType>
    IntType GetTimeUS() const
  { return GetTimeEx<microseconds,IntType>(); }
  /* -- Return time in milliseconds ---------------------------------------- */
  template<typename IntType = uint64_t> requires StdIsArithmatic<IntType>
    IntType GetTimeMS() const
  { return GetTimeEx<milliseconds,IntType>(); }
  /* -- Return time in nanoseconds ----------------------------------------- */
  template<typename IntType = uint64_t> requires StdIsArithmatic<IntType>
    IntType GetTimeNS() const
  { return GetTimeEx<nanoseconds,IntType>(); }
  /* -- Get offset time ---------------------------------------------------- */
  static ClkDuration GetDuration(const ClkTimePoint &ctpCurrent)
    { return GetTime() - ctpCurrent; }
  /* -- Get timepoint count ------------------------------------------------ */
  template<typename IntType>
    auto GetDurationCount(const ClkTimePoint &ctpCurrent) const
  { return ClockGetCount<IntType>(GetDuration(ctpCurrent)); }
  /* -- Convert timepoint to double ---------------------------------------- */
  static double TimePointToDouble(const ClkTimePoint &ctpTime)
    { return ClockDurationToDouble(GetDuration(ctpTime)); }
  /* -- Convert clamped timepoint to double -------------------------------- */
  static double TimePointToClampedDouble(const ClkTimePoint &ctpTime)
    { return UtilMaximum(TimePointToDouble(ctpTime), 0.0); }
  /* -- Convert local time to string --------------------------------------- */
  StdString FormatTime(const char*const cpFormat = cpTimeFormat) const
    { return TimeLocalTTtoStr(GetTimeS(), cpFormat); }
  /* -- Convert universal time to string ----------------------------------- */
  StdString FormatTimeUTC(const char*const cpFormat = cpTimeFormat) const
    { return TimeUTCTTtoStr(GetTimeS(), cpFormat); }
  /* -- Convert time to short duration ------------------------------------- */
  static StdString ToDurationString(unsigned uPrecision = 6)
    { return TimeToShortDuration(GetTimeDouble(), uPrecision); }
  /* -- Convert seconds to long duration relative to current time ---------- */
  StdString ToDurationRel(const StdTimeT tDuration = 0,
    unsigned uCompMax = StdMaxUInt) const
  { return durLong.Parse(GetTimeS() - tDuration, uCompMax); }
  /* -- Convert time to long duration -------------------------------------- */
  StdString ToDurationLongString(unsigned uCompMax = StdMaxUInt) const
    { return ToDurationRel(0, uCompMax); }
  /* -- Unused constructor ------------------------------------------------- */
  Clock() = default;
}; /* -- Clocks we make use of --------------------------------------------- */
static const Clock<system_clock> cmSys; // System time
static const Clock<CoreClock> cmHiRes; // Main high resolution timer
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
