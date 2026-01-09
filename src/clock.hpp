/* == CLOCK.HPP ============================================================ **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## This class helps with processing of TimePoint variables into basic  ## **
** ## types. Also provides a simple class boolean wether the specified    ## **
** ## time has elapsed or not. Since the C++ time functions are so        ## **
** ## complecated, theres also a lot of code here to deal with that. We   ## **
** ## use 'noexcept' extensively here as nothing in 'std::chrono::*' will ## **
** ## ever throw exceptions.                                              ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IClock {                     // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IStd::P;               using namespace IString::P;
using namespace IUtil::P;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* -- Dependencies --------------------------------------------------------- */
using ::std::chrono::duration_cast;    using ::std::chrono::duration;
using ::std::chrono::microseconds;     using ::std::chrono::milliseconds;
using ::std::chrono::nanoseconds;      using ::std::chrono::seconds;
using ::std::chrono::system_clock;
using CoreClock = ::std::chrono::high_resolution_clock; // Using HRC
/* -- Typedefs ------------------------------------------------------------- */
typedef CoreClock::time_point ClkTimePoint;    // Holds a time
typedef CoreClock::duration   ClkDuration;     // Holds a duration
typedef atomic<ClkDuration>   SafeClkDuration; // Thread safe duration
/* -- Common duration values ----------------------------------------------- */
static constexpr ClkDuration
  cd0{ nanoseconds{ 0 } },             cd1MS{ milliseconds{ 1 } },
  cd10MS{ milliseconds{ 10 } },        cd100MS{ milliseconds{ 100 } },
  cd1S{ seconds{ 1 } },                cd60S{ seconds{ 60 } };
/* -- Get count from a duration -------------------------------------------- */
template<typename DurType>
  static auto ClockGetCount(const ClkDuration &cdDuration) noexcept
    { return duration_cast<DurType>(cdDuration).count(); }
/* -- Convert duration to double ------------------------------------------- */
static double ClockDurationToDouble(const ClkDuration &cdDuration) noexcept
  { return ClockGetCount<duration<double>>(cdDuration); }
/* -- Subtract one timepoint from the other and return as double ----------- */
static double ClockTimePointRangeToDouble
  (const ClkTimePoint &ctpEnd, const ClkTimePoint &ctpStart) noexcept
    { return ClockDurationToDouble(ctpEnd - ctpStart); }
/* -- Subtract one timepoint from the other and return as clamped double --- */
static double ClockTimePointRangeToClampedDouble
  (const ClkTimePoint &ctpEnd, const ClkTimePoint &ctpStart) noexcept
    { return UtilMaximum(ClockTimePointRangeToDouble(ctpEnd, ctpStart), 0); }
/* -- Clock manager -------------------------------------------------------- */
template<class ClockType = CoreClock>struct ClockManager
{ /* -- Get current time --------------------------------------------------- */
  static auto GetTime() noexcept { return ClockType::now(); }
  /* -- Get time since epoch ----------------------------------------------- */
  static const ClkDuration GetEpochTime() noexcept
    { return GetTime().time_since_epoch(); }
  /* -- Get current time since epoch casted and counted -------------------- */
  template<typename Type,typename ReturnType>
    const ReturnType GetTimeEx() const noexcept
      { return static_cast<ReturnType>(ClockGetCount<Type>(GetEpochTime())); }
  /* -- Return time as double ---------------------------------------------- */
  static double GetTimeDouble() noexcept
    { return ClockDurationToDouble(GetEpochTime()); }
  /* -- Return time since epoch count as integer --------------------------- */
  template<typename Type=StdTimeT>const Type GetTimeS() const noexcept
    { return GetTimeEx<duration<Type>,Type>(); }
  /* -- Return time in microseconds ---------------------------------------- */
  template<typename Type=uint64_t>const Type GetTimeUS() const noexcept
    { return GetTimeEx<microseconds,Type>(); }
  /* -- Return time in milliseconds ---------------------------------------- */
  template<typename Type=uint64_t>const Type GetTimeMS() const noexcept
    { return GetTimeEx<milliseconds,Type>(); }
  /* -- Return time in nanoseconds ----------------------------------------- */
  template<typename Type=uint64_t>const Type GetTimeNS() const noexcept
    { return GetTimeEx<nanoseconds,Type>(); }
  /* -- Get offset time ---------------------------------------------------- */
  static const ClkDuration GetDuration(const ClkTimePoint &ctpCurrent) noexcept
    { return GetTime() - ctpCurrent; }
  /* -- Get timepoint count ------------------------------------------------ */
  template<typename Type>
    auto GetDurationCount(const ClkTimePoint &ctpCurrent) const noexcept
      { return ClockGetCount<Type>(GetDuration(ctpCurrent)); }
  /* -- Convert timepoint to double ---------------------------------------- */
  static double TimePointToDouble(const ClkTimePoint &ctpTime) noexcept
    { return ClockDurationToDouble(GetDuration(ctpTime)); }
  /* -- Convert clamped timepoint to double -------------------------------- */
  static double TimePointToClampedDouble(const ClkTimePoint &ctpTime) noexcept
    { return UtilMaximum(TimePointToDouble(ctpTime), 0); }
  /* -- Convert local time to string --------------------------------------- */
  const string FormatTime(const char*const cpFormat =
    cpTimeFormat) const noexcept
      { return StrFromTimeTT(GetTimeS(), cpFormat); }
  /* -- Convert universal time to string ----------------------------------- */
  const string FormatTimeUTC(const char*const cpFormat =
    cpTimeFormat) const noexcept
      { return StrFromTimeTTUTC(GetTimeS(), cpFormat); }
  /* -- Convert time to short duration ------------------------------------- */
  static const string ToDurationString(unsigned int uiPrecision = 6) noexcept
    { return StrShortFromDuration(GetTimeDouble(), uiPrecision); }
  /* -- Convert seconds to long duration relative to current time ---------- */
  const string ToDurationRel(const StdTimeT tDuration = 0,
    unsigned int uiCompMax = StdMaxUInt) const noexcept
      { return StrLongFromDuration(GetTimeS() - tDuration, uiCompMax); }
  /* -- Convert time to long duration -------------------------------------- */
  const string ToDurationLongString(unsigned int uiCompMax =
    StdMaxUInt) const noexcept
      { return ToDurationRel(0, uiCompMax); }
  /* -- Unused constructor ------------------------------------------------- */
  ClockManager() = default;
};/* -- Global functors / System time clock functor ------------------------ */
static const ClockManager<system_clock> cmSys;
/* -- High resolution clock functor ---------------------------------------- */
static const ClockManager<CoreClock> cmHiRes;
/* -- Interval helper ------------------------------------------------------ */
template<class CoreClockType = CoreClock,
         class ClockManagerType = ClockManager<CoreClockType>>
class ClockInterval :                  // Members initially private
  /* -- Base classes ------------------------------------------------------- */
  private ClockManagerType             // Type of clock to use
{ /* -- Variables ---------------------------------------------------------- */
  ClkDuration      cdLimit;            // Time delay before trigger
  ClkTimePoint     ctpNext;            // Next trigger
  /* -- Returns if time + this duration not elapsed yet ------------ */ public:
  bool CIIsNotTriggered(const ClkDuration &cdT) const noexcept
    { return this->GetTime() + cdT < ctpNext; }
  /* -- Returns if timepoint not elapsed yet ------------------------------- */
  bool CIIsNotTriggered(const ClkTimePoint &ctpT) const noexcept
    { return ctpT < ctpNext; }
  /* -- Returns if current timepoint not elapsed yet ----------------------- */
  bool CIIsNotTriggered() const noexcept
    { return CIIsNotTriggered(this->GetTime()); }
  /* -- Returns if current timepoint elapsed ------------------------------- */
  bool CIIsTriggered() const noexcept { return !CIIsNotTriggered(); }
  /* -- Add time to next limit --------------------------------------------- */
  void CIAccumulate() noexcept { ctpNext += cdLimit; }
  /* -- Time elapsed? ------------------------------------------------------ */
  bool CITrigger() noexcept
  { // Return false if time hasn't elapsed yet
    if(CIIsNotTriggered()) return false;
    // Set next time
    CIAccumulate();
    // Success
    return true;
  }
  /* -- Time elapsed? ------------------------------------------------------ */
  bool CITriggerStrict() noexcept
  { // Get current high res time
    const ClkTimePoint ctpNow{ this->GetTime() };
    // Return false if time hasn't elapsed yet
    if(CIIsNotTriggered(ctpNow)) return false;
    // Set next time strictly and not accumulate it
    ctpNext = ctpNow + cdLimit;
    // Success
    return true;
  }
  /* -- Reset trigger ------------------------------------------------------ */
  void CIReset() noexcept { ctpNext = this->GetTime() + cdLimit; }
  /* -- Return time left --------------------------------------------------- */
  const ClkDuration CIDelta() const noexcept
    { return this->GetTime() - ctpNext; }
  /* -- Sync now ----------------------------------------------------------- */
  void CISync() noexcept { ctpNext = this->GetTime(); }
  /* -- Update limit and time now do a duration object --------------------- */
  void CISetLimit(const ClkDuration &duL) noexcept { cdLimit = duL; CISync(); }
  /* -- Update limit and time now to a double ------------------------------ */
  void CISetLimit(const double dL) noexcept
    { CISetLimit(duration_cast<ClkDuration>(duration<double>(dL))); }
  /* -- Constructor -------------------------------------------------------- */
  ClockInterval() noexcept :
    /* -- Initialisers ----------------------------------------------------- */
    ctpNext{ this->GetTime() }         // Will trigger next check
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- Constructor (set limit by lvalue) ---------------------------------- */
  explicit ClockInterval(const ClkDuration &duL) noexcept :
    /* -- Initialisers ----------------------------------------------------- */
    cdLimit{ duL },                    // Copy limit from other class
    ctpNext{ this->GetTime() }         // Will trigger next check
    /* -- No code ---------------------------------------------------------- */
    {}
}; /* -- End --------------------------------------------------------------- */
/* == Chronometer ========================================================== */
template<class CoreClockType = CoreClock,
         class ClockManagerType = ClockManager<CoreClockType>>
class ClockChrono :                    // Members intially private
  /* -- Derived classes ---------------------------------------------------- */
  protected ClockManagerType           // Type of clock to use
{ /* -- Private variables -------------------------------------------------- */
  ClkTimePoint ctpStart;               // Don't make this a base class
  /* -- Subtract specified time from the stored time and return as - */ public:
  double CCDeltaRangeToDouble(const ClkTimePoint &ctpEnd) const noexcept
    { return ClockDurationToDouble(ctpEnd - ctpStart); }
  /* -- Same as above but clamps to zero so there is no negative time ------ */
  double CCDeltaToClampedDouble(const ClkTimePoint &ctpEnd) const noexcept
    { return UtilMaximum(CCDeltaRangeToDouble(ctpEnd), 0); }
  /* -- Return uptime as milliseconds in a 64-bit uint --------------------- */
  uint64_t CCDeltaMS() const noexcept
    { return static_cast<uint64_t>(
        this->template GetDurationCount<milliseconds>(ctpStart)); }
  /* -- Return uptime as nanoseconds in a 64-bit uint ---------------------- */
  uint64_t CCDeltaNS() const noexcept
    { return static_cast<uint64_t>(
        this->template GetDurationCount<nanoseconds>(ctpStart)); }
  /* -- Return uptime as microseconds in a 64-bit uint --------------------- */
  uint64_t CCDeltaUS() const noexcept
    { return static_cast<uint64_t>(
        this->template GetDurationCount<microseconds>(ctpStart)); }
  /* -- Return uptime as a double ------------------------------------------ */
  double CCDeltaToDouble() const noexcept
    { return this->TimePointToDouble(ctpStart); }
  /* -- Reset the start time ----------------------------------------------- */
  void CCReset() noexcept { ctpStart = this->GetTime(); }
  /* -- Constructor. Just initialise current time -------------------------- */
  ClockChrono() noexcept :
    /* -- Initialisers ----------------------------------------------------- */
    ctpStart{ this->GetTime() }        // Set start time
    /* -- No code ---------------------------------------------------------- */
    {}
};/* ----------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
