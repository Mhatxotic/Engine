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
using namespace IStd::P;               using namespace IUtil::P;
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
  uint64_t         ullTriggers,        // Number of frame timeout checks
                   ullTicks;           // Number of ticks processed this sec
  double           dFps, dFpsSmoothed; // Caluclated FPS value + smoothed
  /* -- Update fps value --------------------------------------------------- */
  void TimerSetFPS(const double dTime)
    { dFps = UtilSmooth(UtilPerSec(dTime), dFpsSmoothed); }
  /* -- Do time calculations ----------------------------------------------- */
  void TimerCalculateTime()
  { // Set end time
    ctpEnd = cmHiRes.GetTime();
    // Get duration we had to wait since end of last game loop
    cdTick = ctpEnd - ctpStart;
    // Set new start time
    ctpStart = ctpEnd;
  }
  /* -- Requested by rendered to wait because nothing was drawn ---- */ public:
  void TimerForceWait() { cdDelay = cd1MS; }
  /* -- Update delay as double --------------------------------------------- */
  void TimerUpdateDelay(const int iNDelay)
    { cdDelay = milliseconds{ iNDelay }; }
  /* -- Forces a delay internally if delay is disabled --------------------- */
  void TimerSetDelayIfZero() { if(cdDelay != cd0) TimerUpdateDelay(1); }
  /* -- Restore saved persistent delay timer ------------------------------- */
  void TimerRestoreDelay() { cdDelay = cdDelayPst; }
  /* -- Get start time ----------------------------------------------------- */
  const ClkTimePoint TimerGetStartTime() const { return ctpFrame; }
  /* -- Recalculate LUA timeout -------------------------------------------- */
  void TimerRecalculateLuaTimeOut() { ctpTimeOut = ctpEnd + cdTimeOut; }
  /* -- Do suspend with currently requested delay -------------------------- */
  void TimerDoSuspend() { StdSuspend(cdDelay); }
  /* -- Calculate time elapsed since c++ ----------------------------------- */
  void TimerUpdateBot()
  { // Sleep if theres a delay else just yield
    TimerDoSuspend();
    // Calculate current time
    TimerCalculateTime();
    // Set new fps if a delay was registered
    if(cdTick != cd0) TimerSetFPS(ClockDurationToDouble(cdTick));
    // Set new LUA timeout time
    TimerRecalculateLuaTimeOut();
    // Update new frame start time
    ctpFrame = ctpStart;
    // Increment ticks
    ++ullTicks;
  }
  /* -- Do calculation and return if we're still under the frame time? ----- */
  bool TimerDoCalcAndShouldTick()
  { // Calculate frame time
    TimerCalculateTime();
    // Increase accumulator by frame time
    cdAcc += cdTick;
    // Frame limit not reached?
    return cdAcc < cdLimit;
  }
  /* -- Should execute a game tick? ---------------------------------------- */
  bool TimerShouldTick()
  { // Frame limit not reached?
    if(TimerDoCalcAndShouldTick()) [[likely]]
    { // If we can suspend or yield to save CPU cycles?
      if(cdDelay != cd0 && cdAcc + cdDelay < cdLimit) [[likely]]
      { // Wait for specified delay
        TimerDoSuspend();
        // Frame limit still not reached?
        if(TimerDoCalcAndShouldTick()) [[unlikely]] return false;
      } // Suspend engine thread for the requested delay
      else return false;
    } // Set total time the frame took
    cdFrame = cdAcc;
    // Set new fps
    TimerSetFPS(ClockDurationToDouble(ctpEnd - TimerGetStartTime()));
    // Set new frame start time
    ctpFrame = ctpStart;
    // Set new LUA timeout time
    TimerRecalculateLuaTimeOut();
    // Reduce accumulator
    cdAcc -= cdLimit;
    // Reset delay if it was changed (by idle GPU routine)
    if(cdDelayPst != cdDelay) [[unlikely]] cdDelay = cdDelayPst;
    // Increase number of ticks rendered
    ++ullTicks;
    // Tick loop should render a frame
    return true;
  }
  /* -- Should not execute a game tick? ------------------------------------ */
  bool TimerShouldNotTick() { return !TimerShouldTick(); }
  /* -- Reset counters and reinitialise start and end time ----------------- */
  void TimerCatchup()
  { // Reset accumulator to zero
    cdAcc = cd0;
    // Reset tick and frame duration to limit
    cdFrame = cdTick = cdLimit;
    // Update new tick start and end time and start frame time
    ctpStart = ctpEnd = cmHiRes.GetTime();
  }
  /* -- Return if script timer has not timed out yet ----------------------- */
  bool TimerIsNotTimedOut()
    { ++ullTriggers; return cmHiRes.GetTime() < ctpTimeOut; }
  /* -- Return how many times the script trigger was checked --------------- */
  uint64_t TimerGetTriggers() const { return ullTriggers; }
  /* -- Return the current script timeout ---------------------------------- */
  double TimerGetTimeOut() const { return ClockDurationToDouble(cdTimeOut); }
  /* -- Return the current engine target tick time ------------------------- */
  double TimerGetLimit() const { return ClockDurationToDouble(cdLimit); }
  /* -- Return the current accumulated frame time -------------------------- */
  double TimerGetAccumulator() const { return ClockDurationToDouble(cdFrame); }
  /* -- Return the frames per second based on the last frame --------------- */
  double TimerGetFPS() const { return dFpsSmoothed; }
  /* -- Return the target frames per second -------------------------------- */
  double TimerGetFPSLimit() const { return UtilPerSec(TimerGetLimit()); }
  /* -- Get the number of engine ticks processed --------------------------- */
  uint64_t TimerGetTicks() const { return ullTicks; }
  /* -- Reset tick count --------------------------------------------------- */
  void TimerResetTicks() { ullTicks = 0; }
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
    ullTriggers(0),                    // Init number of frame timeout checks
    ullTicks(0),                       // Init no. of ticks processed this sec
    dFps(0.0), dFpsSmoothed(0.0)       // Init frames per second and smoothed
    /* -- Set global pointer to static class ------------------------------- */
    { cTimer = this; }
  /* -- Set time out ----------------------------------------------- */ public:
  CVarReturn TimerSetTimeOut(const unsigned int uiTimeOut)
    { return CVarSimpleSetIntNL(cdTimeOut, seconds{ uiTimeOut }, cd1S); }
  /* -- Set global target fps ---------------------------------------------- */
  CVarReturn TimerTickRateModified(const uint64_t ullInterval)
  { // Return if not valid
    if(CVarSimpleSetIntNLGE(cdLimit, nanoseconds{ ullInterval },
         nanoseconds{ 2000000 }, nanoseconds{ 1000000000 }) == DENY)
      return DENY;
    // Initialise expected FPS
    dFps = dFpsSmoothed = UtilPerSec(TimerGetLimit());
    // Success
    return ACCEPT;
  }
  /* -- TimerSetDelay ------------------------------------------------------ */
  CVarReturn TimerSetDelay(const unsigned int uiNDelay)
  { // Ignore if set over one second, any sort of app would not be usable with
    // over one second delay, so might as well cap it.
    if(uiNDelay > 1000) return DENY;
    // Set new delay
    TimerUpdateDelay(uiNDelay);
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
