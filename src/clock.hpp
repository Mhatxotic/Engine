/* == CLOCK.HPP ============================================================ **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## This class helps with processing of TimePoint variables into basic  ## **
** ## types. Also provides a simple class boolean wether the specified    ## **
** ## time has elapsed or not. Since the C++ time functions are so        ## **
** ## complecated, theres also a lot of code here to deal with that.      ## **
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
/* -- Get count from a duration -------------------------------------------- */
template<typename DurType>
  static auto ClockGetCount(const ClkDuration cdDuration)
    { return duration_cast<DurType>(cdDuration).count(); }
/* -- Convert duration to double ------------------------------------------- */
static double ClockDurationToDouble(const ClkDuration cdDuration)
  { return ClockGetCount<duration<double>>(cdDuration); }
/* -- Subtract one timepoint from the other and return as double ----------- */
static double ClockTimePointRangeToDouble
  (const ClkTimePoint &ctpEnd, const ClkTimePoint ctpStart)
    { return ClockDurationToDouble(ctpEnd - ctpStart); }
/* -- Subtract one timepoint from the other and return as clamped double --- */
static double ClockTimePointRangeToClampedDouble
  (const ClkTimePoint ctpEnd, const ClkTimePoint ctpStart)
    { return UtilMaximum(ClockTimePointRangeToDouble(ctpEnd, ctpStart), 0); }
/* -- Clock manager -------------------------------------------------------- */
template<class ClockType = CoreClock>struct ClockManager
{ /* -- Get current time --------------------------------------------------- */
  auto GetTime(void) const { return ClockType::now(); }
  /* -- Get time since epoch ----------------------------------------------- */
  const ClkDuration GetEpochTime(void) const
    { return GetTime().time_since_epoch(); }
  /* -- Get current time since epoch casted and counted -------------------- */
  template<typename Type,typename ReturnType>
    const ReturnType GetTimeEx(void) const
      { return static_cast<ReturnType>(ClockGetCount<Type>(GetEpochTime())); }
  /* -- Return time as double ---------------------------------------------- */
  double GetTimeDouble(void) const
    { return ClockDurationToDouble(GetEpochTime()); }
  /* -- Return time since epoch count as integer --------------------------- */
  template<typename Type=StdTimeT>const Type GetTimeS(void) const
    { return GetTimeEx<duration<Type>,Type>(); }
  /* -- Return time in microseconds ---------------------------------------- */
  template<typename Type=uint64_t>const Type GetTimeUS(void) const
    { return GetTimeEx<microseconds,Type>(); }
  /* -- Return time in milliseconds ---------------------------------------- */
  template<typename Type=uint64_t>const Type GetTimeMS(void) const
    { return GetTimeEx<milliseconds,Type>(); }
  /* -- Return time in nanoseconds ----------------------------------------- */
  template<typename Type=uint64_t>const Type GetTimeNS(void) const
    { return GetTimeEx<nanoseconds,Type>(); }
  /* -- Get offset time ---------------------------------------------------- */
  const ClkDuration GetDuration(const ClkTimePoint ctpCurrent) const
    { return GetTime() - ctpCurrent; }
  /* -- Get timepoint count ------------------------------------------------ */
  template<typename Type>
    auto GetDurationCount(const ClkTimePoint ctpCurrent) const
      { return ClockGetCount<Type>(GetDuration(ctpCurrent)); }
  /* -- Convert timepoint to double ---------------------------------------- */
  double TimePointToDouble(const ClkTimePoint ctpTime) const
    { return ClockDurationToDouble(GetDuration(ctpTime)); }
  /* -- Convert clamped timepoint to double -------------------------------- */
  double TimePointToClampedDouble(const ClkTimePoint ctpTime) const
    { return UtilMaximum(TimePointToDouble(ctpTime), 0); }
  /* -- Convert local time to string --------------------------------------- */
  const string FormatTime(const char*const cpFormat = cpTimeFormat)
    const { return StrFromTimeTT(GetTimeS(), cpFormat); }
  /* -- Convert universal time to string ----------------------------------- */
  const string FormatTimeUTC(const char*const cpFormat =
    cpTimeFormat) const { return StrFromTimeTTUTC(GetTimeS(), cpFormat); }
  /* -- Convert time to short duration ------------------------------------- */
  const string ToDurationString(unsigned int uiPrecision = 6) const
    { return StrShortFromDuration(GetTimeDouble(), uiPrecision); }
  /* -- Convert seconds to long duration relative to current time ---------- */
  const string ToDurationRel(const StdTimeT tDuration = 0,
    unsigned int uiCompMax = StdMaxUInt) const
      { return StrLongFromDuration(GetTimeS() - tDuration, uiCompMax); }
  /* -- Convert time to long duration -------------------------------------- */
  const string ToDurationLongString(unsigned int uiCompMax = StdMaxUInt) const
    { return ToDurationRel(0, uiCompMax); }
  /* -- Unused constructor ------------------------------------------------- */
  ClockManager(void) { }
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
  bool CIIsNotTriggered(const ClkDuration cdT) const
    { return this->GetTime() + cdT < ctpNext; }
  /* -- Returns if timepoint not elapsed yet ------------------------------- */
  bool CIIsNotTriggered(const ClkTimePoint ctpT) const
    { return ctpT < ctpNext; }
  /* -- Returns if current timepoint not elapsed yet ----------------------- */
  bool CIIsNotTriggered(void) const
    { return CIIsNotTriggered(this->GetTime()); }
  /* -- Returns if current timepoint elapsed ------------------------------- */
  bool CIIsTriggered(void) const { return !CIIsNotTriggered(); }
  /* -- Add time to next limit --------------------------------------------- */
  void CIAccumulate(void) { ctpNext += cdLimit; }
  /* -- Time elapsed? ------------------------------------------------------ */
  bool CITrigger(void)
  { // Return false if time hasn't elapsed yet
    if(CIIsNotTriggered()) return false;
    // Set next time
    CIAccumulate();
    // Success
    return true;
  }
  /* -- Time elapsed? ------------------------------------------------------ */
  bool CITriggerStrict(void)
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
  void CIReset(void) { ctpNext = this->GetTime() + cdLimit; }
  /* -- Return time left --------------------------------------------------- */
  const ClkDuration CIDelta(void) const { return this->GetTime() - ctpNext; }
  /* -- Sync now ----------------------------------------------------------- */
  void CISync(void) { ctpNext = this->GetTime(); }
  /* -- Update limit and time now do a duration object --------------------- */
  void CISetLimit(const ClkDuration duL) { cdLimit = duL; CISync(); }
  /* -- Update limit and time now to a double ------------------------------ */
  void CISetLimit(const double dL)
    { CISetLimit(duration_cast<ClkDuration>(duration<double>(dL))); }
  /* -- Constructor -------------------------------------------------------- */
  ClockInterval(void) :
    /* -- Initialisers ----------------------------------------------------- */
    cdLimit{ seconds{ 0 } },           // Set limit to instant
    ctpNext{ this->GetTime() }         // Will trigger next check
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Constructor (set limit by lvalue) ---------------------------------- */
  explicit ClockInterval(const ClkDuration duL) :
    /* -- Initialisers ----------------------------------------------------- */
    cdLimit{ duL },                    // Copy limit from other class
    ctpNext{ this->GetTime() }         // Will trigger next check
    /* -- No code ---------------------------------------------------------- */
    { }
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
  double CCDeltaRangeToDouble(const ClkTimePoint ctpEnd) const
    { return ClockDurationToDouble(ctpEnd - ctpStart); }
  /* -- Same as above but clamps to zero so there is no negative time ------ */
  double CCDeltaToClampedDouble(const ClkTimePoint ctpEnd) const
    { return UtilMaximum(CCDeltaRangeToDouble(ctpEnd), 0); }
  /* -- Return uptime as milliseconds in a 64-bit uint --------------------- */
  uint64_t CCDeltaMS(void) const
    { return static_cast<uint64_t>(
        this->template GetDurationCount<milliseconds>(ctpStart)); }
  /* -- Return uptime as nanoseconds in a 64-bit uint ---------------------- */
  uint64_t CCDeltaNS(void) const
    { return static_cast<uint64_t>(
        this->template GetDurationCount<nanoseconds>(ctpStart)); }
  /* -- Return uptime as microseconds in a 64-bit uint --------------------- */
  uint64_t CCDeltaUS(void) const
    { return static_cast<uint64_t>(
        this->template GetDurationCount<microseconds>(ctpStart)); }
  /* -- Return uptime as a double ------------------------------------------ */
  double CCDeltaToDouble(void) const
    { return this->TimePointToDouble(ctpStart); }
  /* -- Reset the start time ----------------------------------------------- */
  void CCReset(void) { ctpStart = this->GetTime(); }
  /* -- Constructor. Just initialise current time -------------------------- */
  ClockChrono(void) :        // No parameters
    /* -- Initialisers ----------------------------------------------------- */
    ctpStart{ this->GetTime() }        // Set start time
    /* -- No code ---------------------------------------------------------- */
    { }
};
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
