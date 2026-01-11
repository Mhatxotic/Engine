/* == TIMER.HPP ============================================================ **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## This class is the engine timer manager. We use 'noexcept'           ## **
** ## extensively here as nothing in 'std::chrono::*' will ever throw     ## **
** ## exceptions.                                                         ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace ITimer {                     // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IClock::P;             using namespace ICVarDef::P;
using namespace IStd::P;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* ------------------------------------------------------------------------- */
class Timer;                           // Class prototype
static Timer *cTimer = nullptr;        // Address of global class
class Timer                            // Members initially private
{ /* -- Variables ---------------------------------------------------------- */
  ClkTimePoint     ctpFrame,           // Frame start time
                   ctpStart,           // Start of frame time
                   ctpSecond,          // One second interval
                   ctpTimeOut,         // Time script times out
                   ctpEnd;             // End of frame time
  ClkDuration      cdAcc,              // Accumulator duration
                   cdFrame,            // Frame time
                   cdLimit,            // Frame limit
                   cdDelay,            // Delay duration
                   cdDelayPst,         // Persistent delay duration
                   cdTick,             // Tick duration
                   cdTimeOut;          // Frame timeout duration
  uint64_t         uqTriggers,         // Number of frame timeout checks
                   uqTicks;            // Number of ticks processed this sec
  double           dFPS;               // Frames per second
  /* -- Update fps value --------------------------------------------------- */
  void TimerSetFPS(const double dTime) { dFPS = TimerPerSec(dTime); }
  /* -- Update delay as double ----------- ------------------------- */ public:
  void TimerUpdateDelay(const unsigned int uiNewDelay)
    { cdDelay = milliseconds{ uiNewDelay }; }
  /* -- Requested by rendered to wait because nothing was drawn ------------ */
  void TimerForceWait() { cdDelay = cd1MS; }
  /* -- Forces a delay internally if delay is disabled --------------------- */
  void TimerSetDelayIfZero() { if(cdDelay != cd0) TimerUpdateDelay(1); }
  /* -- Restore saved persistent delay timer ------------------------------- */
  void TimerRestoreDelay() { cdDelay = cdDelayPst; }
  /* -- Get start time ----------------------------------------------------- */
  const ClkTimePoint TimerGetStartTime() const { return ctpStart; }
  /* -- Do time calculations ----------------------------------------------- */
  void TimerCalculateTime()
  { // Set end time
    ctpEnd = cmHiRes.GetTime();
    // Get duration we had to wait since end of last game loop
    cdTick = ctpEnd - ctpStart;
    // Set new start time
    ctpStart = ctpEnd;
    // Set new LUA timeout time
    ctpTimeOut = ctpEnd + cdTimeOut;
  }
  /* -- Calculate time elapsed since c++ ----------------------------------- */
  void TimerUpdateBot()
  { // Sleep if theres a delay else just yield
    StdSuspend(cdDelay);
    // Set new fps
    TimerSetFPS(ClockDurationToDouble(cdTick));
    // Calculate current time
    TimerCalculateTime();
    // Increment ticks
    ++uqTicks;
  }
  /* -- Should not execute a game tick? ------------------------------------ */
  bool TimerShouldNotTick()
  { // Calculate frame time
    TimerCalculateTime();
    // Increase accumulator by frame time
    cdAcc += cdTick;
    // Frame limit not reached?
    if(cdAcc < cdLimit) [[likely]]
    { // Wait for specified delay if we can or yield
      if(cdAcc + cdDelay < cdLimit) [[likely]] StdSuspend(cdDelay);
      // Suspend engine thread for the requested delay
      return true;
    } // Set new fps
    TimerSetFPS(ClockDurationToDouble(ctpEnd - ctpFrame));
    // Set total time the frame took
    cdFrame = cdAcc;
    // Set new frame start time
    ctpFrame = ctpEnd;
    // Reduce accumulator
    cdAcc -= cdLimit;
    // Reset delay if it was changed (by idle GPU routine)
    if(cdDelayPst != cdDelay) [[unlikely]] cdDelay = cdDelayPst;
    // Increase number of ticks rendered
    ++uqTicks;
    // Tick loop should render a frame
    return false;
  }
  /* -- Reset counters and reinitialise start and end time ----------------- */
  void TimerCatchup()
  { // Reset accumulator and duration
    cdAcc = cdFrame = cdTick = cd0;
    // Update new tick start and end time and start frame time
    ctpStart = ctpEnd = ctpFrame = cmHiRes.GetTime();
  }
  /* -- Return if script timer timed out ----------------------------------- */
  bool TimerIsTimedOut()
    { ++uqTriggers; return cmHiRes.GetTime() >= ctpTimeOut; }
  /* -- Return how many times the script trigger was checked --------------- */
  uint64_t TimerGetTriggers() const { return uqTriggers; }
  /* -- Return the current script timeout ---------------------------------- */
  double TimerGetTimeOut() const { return ClockDurationToDouble(cdTimeOut); }
  /* -- Return the current engine target tick time ------------------------- */
  double TimerGetLimit() const { return ClockDurationToDouble(cdLimit); }
  /* -- Return the current accumulated frame time -------------------------- */
  double TimerGetAccumulator() const { return ClockDurationToDouble(cdFrame); }
  /* -- Return the frames per second based on the last frame --------------- */
  double TimerGetFPS() const { return dFPS; }
  /* -- Calculate times per second based on an interval -------------------- */
  static constexpr double TimerPerSec(const double dVal) { return 1.0 / dVal; }
  /* -- Return the target frames per second -------------------------------- */
  double TimerGetFPSLimit() const { return TimerPerSec(TimerGetLimit()); }
  /* -- Get the number of engine ticks processed --------------------------- */
  uint64_t TimerGetTicks() const { return uqTicks; }
  /* -- Reset tick count --------------------------------------------------- */
  void TimerResetTicks() { uqTicks = 0; }
  /* -- Get the current suspend time --------------------------------------- */
  double TimerGetDelay() const { return ClockDurationToDouble(cdDelay); }
  /* -- Get the current tick start time ------------------------------------ */
  double TimerGetStart() const
    { return ClockDurationToDouble(ctpStart.time_since_epoch()); }
  /* -- Reset the delay and timer ------------------------------------------ */
  void TimerReset(const bool bIdle)
  { // Restore delay timer since we let Lua modify it
    TimerRestoreDelay();
    // Force a suspend when leaving sandbox if suspend is disabled
    if(bIdle) TimerSetDelayIfZero();
    // Reset timer
    TimerCatchup();
  }
  /* -- Default constructor ------------------------------------- */ protected:
  Timer() :
    /* --------------------------------------------------------------------- */
    uqTriggers(0),                     // Init number of frame timeout checks
    uqTicks(0),                        // Init no. of ticks processed this sec
    dFPS(0.0)                          // Init frames per second
    /* -- Set global pointer to static class ------------------------------- */
    { cTimer = this; }
  /* -- Set time out ----------------------------------------------- */ public:
  CVarReturn TimerSetTimeOut(const unsigned int uiTimeOut)
    { return CVarSimpleSetIntNL(cdTimeOut, seconds{ uiTimeOut }, cd1S); }
  /* -- Set global target fps ---------------------------------------------- */
  CVarReturn TimerTickRateModified(const uint64_t uqInterval)
  { // Return if not valid
    if(CVarSimpleSetIntNLGE(cdLimit, nanoseconds{ uqInterval },
         nanoseconds{ 2000000 }, nanoseconds{ 1000000000 }) == DENY)
      return DENY;
    // Set expected FPS
    TimerSetFPS(TimerGetLimit());
    // Success
    return ACCEPT;
  }
  /* -- TimerSetDelay ------------------------------------------------------ */
  CVarReturn TimerSetDelay(const unsigned int uiNewDelay)
  { // Ignore if set over one second, any sort of app would not be usable with
    // over one second delay, so might as well cap it
    if(uiNewDelay > 1000) return DENY;
    // Set new delay
    TimerUpdateDelay(uiNewDelay);
    // Update persistent delay
    cdDelayPst = cdDelay;
    // Success
    return ACCEPT;
  }
};/* ----------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
