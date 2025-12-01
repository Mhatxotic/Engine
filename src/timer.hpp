/* == TIMER.HPP ============================================================ **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## This class is the engine timer manager.                             ## **
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
{ /* -- Limits ------------------------------------------------------------- */
  static const uint64_t uqIntvMin =    2000000, // Minimum interval
                        uqIntvMax = 1000000000; // Maximum interval
  /* -- Variables ---------------------------------------------------------- */
  ClkTimePoint     ctpStart,           // Start of frame time
                   ctpSecond,          // One second interval
                   ctpTimeOut,         // Time script times out
                   ctpEnd;             // End of frame time
  ClkDuration      cdAcc,              // Accumulator duration
                   cdLimit,            // Frame limit
                   cdDelay,            // Delay duration
                   cdDelayPst,         // Persistent delay duration
                   cdFrame,            // Frame duration
                   cdSecond,           // One second timer
                   cdTimeOut;          // Frame timeout duration
  uint64_t         uqTriggers,         // Number of frame timeout checks
                   uqTicks,            // Number of ticks processed this sec
                   uqTicksLast;        // Number of ticks processed last sec
  unsigned int     uiFPS;              // Frames per second
  bool             bWait;              // Force wait?
  /* -- Set engine tick rate --------------------------------------- */ public:
  void TimerSetInterval(const uint64_t uqInterval)
    { cdLimit = nanoseconds{ uqInterval }; }
  /* -- Get minimum maximum interval value --------------------------------- */
  static uint64_t TimerGetMinInterval() { return uqIntvMin; }
  static uint64_t TimerGetMaxInterval() { return uqIntvMax; }
  /* -- Update delay as double --------------------------------------------- */
  void TimerUpdateDelay(const unsigned int uiNewDelay)
    { cdDelay = milliseconds{ uiNewDelay }; }
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
    cdFrame = ctpEnd - ctpStart;
    // Set new start time
    ctpStart = ctpEnd;
    // Set new timeout time
    ctpTimeOut = ctpEnd + cdTimeOut;
    // Add to one second timer
    cdSecond += cdFrame;
    // Return if one second has past
    if(cdSecond < cd1S) return;
    // Set new fps
    uiFPS = static_cast<unsigned int>(uqTicks - uqTicksLast);
    // Update last ticks
    uqTicksLast = uqTicks;
    // Reset second timer
    cdSecond = cd0;
  }
  /* -- Thread suspense by requested duration ------------------------------ */
  void TimerSuspendRequested() const { StdSuspend(cdDelay); }
  /* -- Force wait if delay is disabled (cFboCore->Render()) --------------- */
  void TimerForceWait() { if(cdDelay == cd0) bWait = true; }
  /* -- Calculate time elapsed since c++ ----------------------------------- */
  void TimerUpdateBot()
  { // Sleep if theres a delay
    TimerSuspendRequested();
    // Calculate current time using stl
    TimerCalculateTime();
    // Increment ticks
    ++uqTicks;
  }
  /* -- Should not execute a game tick? ------------------------------------ */
  bool TimerShouldNotTick()
  { // Calculate frame time
    TimerCalculateTime();
    // Increase accumulator by frame time
    cdAcc += cdFrame;
    // Frame limit not reached?
    if(cdAcc < cdLimit)
    { // Force if we're forced to wait
      if(bWait) StdSuspend();
      // Or wait a little bit if we can
      else TimerSuspendRequested();
      // Suspend engine thread for the requested delay
      return true;
    } // Reduce accumulator
    cdAcc -= cdLimit;
    // Increase number of ticks rendered
    ++uqTicks;
    // We are no longer forced to wait
    bWait = false;
    // Tick loop should render a frame
    return false;
  }
  /* -- Reset counters and reinitialise start and end time ----------------- */
  void TimerCatchup()
  { // Reset accumulator and duration
    cdAcc = cdFrame = cd0;
    // Update new start and end time
    ctpStart = ctpEnd = cmHiRes.GetTime();
  }
  /* -- Return if script timer timed out ----------------------------------- */
  bool TimerIsTimedOut()
    { ++uqTriggers; return cmHiRes.GetTime() >= ctpTimeOut; }
  /* -- Return how many times the script trigger was checked --------------- */
  uint64_t TimerGetTriggers() const { return uqTriggers; }
  /* -- Return the current script timeout ---------------------------------- */
  double TimerGetTimeOut() const
    { return ClockDurationToDouble(cdTimeOut); }
  /* -- Return the duration of the last frame ------------------------------ */
  double TimerGetDuration() const
    { return ClockDurationToDouble(cdFrame); }
  /* -- Return the current engine target tick time ------------------------- */
  double TimerGetLimit() const { return ClockDurationToDouble(cdLimit); }
  /* -- Return the current accumulated frame time -------------------------- */
  double TimerGetAccumulator() const
    { return ClockDurationToDouble(cdAcc); }
  /* -- Return the frames per second based on the last frame --------------- */
  unsigned int TimerGetFPS() const { return uiFPS; }
  /* -- Return the target frames per second -------------------------------- */
  double TimerGetFPSLimit() const { return 1.0 / TimerGetLimit(); }
  /* -- Get the number of engine ticks processed --------------------------- */
  uint64_t TimerGetTicks() const { return uqTicks; }
  /* -- Reset tick count --------------------------------------------------- */
  void TimerResetTicks() { uqTicks = uqTicksLast = 0; }
  /* -- Get the current suspend time --------------------------------------- */
  double TimerGetDelay() const
    { return ClockDurationToDouble(cdDelay); }
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
    uqTicksLast(0),                    // Init no. of ticks processed last time
    uiFPS(0),                          // Init frames per second
    bWait(false)                       // Init force wait
    /* -- Set global pointer to static class ------------------------------- */
    { cTimer = this; }
  /* -- Set time out ----------------------------------------------- */ public:
  CVarReturn TimerSetTimeOut(const unsigned int uiTimeOut)
    { return CVarSimpleSetIntNL(cdTimeOut,
        seconds{ uiTimeOut }, cd1S); }
  /* -- Set global target fps ---------------------------------------------- */
  CVarReturn TimerTickRateModified(const uint64_t uqInterval)
  { // Return if not valid
    if(CVarSimpleSetIntNLGE(cdLimit, nanoseconds{ uqInterval },
        nanoseconds{ TimerGetMinInterval() },
        nanoseconds{ TimerGetMaxInterval() }) == DENY) return DENY;
    // Set expected FPS
    uiFPS = static_cast<unsigned int>(1.0 / TimerGetLimit());
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
