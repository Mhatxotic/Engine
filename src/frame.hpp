/* == FRAME.HPP ============================================================ **
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
namespace IFrame {                     // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IClock::P;             using namespace ICVarDef::P;
using namespace IStd::P;               using namespace IUtil::P;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* ------------------------------------------------------------------------- */
class Frame;                           // Class prototype
static Frame *cFrame = nullptr;        // Address of global class
class Frame                            // Members initially private
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
  void FrameSetFPS(const double dTime)
    { dFps = UtilSmooth(UtilPerSec(dTime), dFpsSmoothed); }
  /* -- Do time calculations ----------------------------------------------- */
  void FrameCalculateTime()
  { // Set end time
    ctpEnd = cmHiRes.GetTime();
    // Get duration we had to wait since end of last game loop
    cdTick = ctpEnd - ctpStart;
    // Set new start time
    ctpStart = ctpEnd;
  }
  /* -- Requested by rendered to wait because nothing was drawn ---- */ public:
  void FrameForceWait() { cdDelay = cd1MS; }
  /* -- Update delay as double --------------------------------------------- */
  void FrameUpdateDelay(const unsigned uNDelay)
    { cdDelay = milliseconds{ uNDelay }; }
  /* -- Forces a delay internally if delay is disabled --------------------- */
  void FrameSetDelayIfZero() { if(cdDelay != cd0) FrameUpdateDelay(1); }
  /* -- Restore saved persistent delay timer ------------------------------- */
  void FrameRestoreDelay() { cdDelay = cdDelayPst; }
  /* -- Get start time ----------------------------------------------------- */
  const ClkTimePoint FrameGetStartTime() const { return ctpFrame; }
  /* -- Recalculate LUA timeout -------------------------------------------- */
  void FrameRecalculateLuaTimeOut() { ctpTimeOut = ctpEnd + cdTimeOut; }
  /* -- Do suspend with currently requested delay -------------------------- */
  void FrameDoSuspend() { StdSuspend(cdDelay); }
  /* -- Calculate time elapsed since c++ ----------------------------------- */
  void FrameUpdateBot()
  { // Sleep if theres a delay else just yield
    FrameDoSuspend();
    // Calculate current time
    FrameCalculateTime();
    // Set new fps if a delay was registered
    if(cdTick != cd0) FrameSetFPS(ClockDurationToDouble(cdTick));
    // Set new LUA timeout time
    FrameRecalculateLuaTimeOut();
    // Update new frame start time
    ctpFrame = ctpStart;
    // Increment ticks
    ++ullTicks;
  }
  /* -- Do calculation and return if we're still under the frame time? ----- */
  bool FrameDoCalcAndShouldNotTick()
  { // Calculate frame time
    FrameCalculateTime();
    // Increase accumulator by frame time
    cdAcc += cdTick;
    // Frame limit not reached?
    return cdAcc < cdLimit;
  }
  /* -- Should execute a game tick? ---------------------------------------- */
  bool FrameShouldTick()
  { // Frame limit not reached?
    if(FrameDoCalcAndShouldNotTick()) [[likely]]
    { // If we can suspend or yield to save CPU cycles?
      if(cdDelay != cd0 && cdAcc + cdDelay < cdLimit) [[likely]]
      { // Wait for specified delay
        FrameDoSuspend();
        // Frame limit still not reached?
        if(FrameDoCalcAndShouldNotTick()) [[unlikely]] return false;
      } // Suspend engine thread for the requested delay
      else return false;
    } // Set total time the frame took
    cdFrame = cdAcc;
    // Set new fps
    FrameSetFPS(ClockDurationToDouble(ctpEnd - FrameGetStartTime()));
    // Set new frame start time
    ctpFrame = ctpStart;
    // Set new LUA timeout time
    FrameRecalculateLuaTimeOut();
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
  bool FrameShouldNotTick() { return !FrameShouldTick(); }
  /* -- Reset counters and reinitialise start and end time ----------------- */
  void FrameCatchup()
  { // Reset accumulator to zero
    cdAcc = cd0;
    // Reset tick and frame duration to limit
    cdFrame = cdTick = cdLimit;
    // Update new tick start and end time and start frame time
    ctpStart = ctpEnd = cmHiRes.GetTime();
  }
  /* -- Return if script timer has not timed out yet ----------------------- */
  bool FrameIsNotTimedOut()
    { ++ullTriggers; return cmHiRes.GetTime() < ctpTimeOut; }
  /* -- Return how many times the script trigger was checked --------------- */
  uint64_t FrameGetTriggers() const { return ullTriggers; }
  /* -- Return the current script timeout ---------------------------------- */
  double FrameGetTimeOut() const { return ClockDurationToDouble(cdTimeOut); }
  /* -- Return the current engine target tick time ------------------------- */
  double FrameGetLimit() const { return ClockDurationToDouble(cdLimit); }
  /* -- Return the current accumulated frame time -------------------------- */
  double FrameGetAccumulator() const { return ClockDurationToDouble(cdFrame); }
  /* -- Return the frames per second based on the last frame --------------- */
  double FrameGetFPS() const { return dFpsSmoothed; }
  /* -- Return the target frames per second -------------------------------- */
  double FrameGetFPSLimit() const { return UtilPerSec(FrameGetLimit()); }
  /* -- Get the number of engine ticks processed --------------------------- */
  uint64_t FrameGetTicks() const { return ullTicks; }
  /* -- Reset tick count --------------------------------------------------- */
  void FrameResetTicks() { ullTicks = 0; }
  /* -- Get the current suspend time --------------------------------------- */
  double FrameGetDelay() const { return ClockDurationToDouble(cdDelay); }
  /* -- Get the current tick start time ------------------------------------ */
  double FrameGetStart() const
    { return ClockDurationToDouble(ctpStart.time_since_epoch()); }
  /* -- Reset the delay and timer ------------------------------------------ */
  void FrameReset(const bool bIdle)
  { // Restore delay timer since we let Lua modify it
    FrameRestoreDelay();
    // Force a suspend when leaving sandbox if suspend is disabled
    if(bIdle) FrameSetDelayIfZero();
    // Reset timer
    FrameCatchup();
  }
  /* -- Default constructor ------------------------------------- */ protected:
  Frame() :
    /* --------------------------------------------------------------------- */
    ullTriggers(0),                    // Init number of frame timeout checks
    ullTicks(0),                       // Init no. of ticks processed this sec
    dFps(0.0), dFpsSmoothed(0.0)       // Init frames per second and smoothed
    /* -- Set global pointer to static class ------------------------------- */
    { cFrame = this; }
  /* -- Set time out ----------------------------------------------- */ public:
  CVarReturn FrameSetTimeOut(const unsigned uTimeOut)
    { return CVarSimpleSetIntNL(cdTimeOut, seconds{ uTimeOut }, cd1S); }
  /* -- Set global target fps ---------------------------------------------- */
  CVarReturn FrameTickRateModified(const uint64_t ullInterval)
  { // Return if not valid
    if(CVarSimpleSetIntNLGE(cdLimit, nanoseconds{ ullInterval },
         nanoseconds{ 2000000 }, nanoseconds{ 1000000000 }) == DENY)
      return DENY;
    // Initialise expected FPS
    dFps = dFpsSmoothed = UtilPerSec(FrameGetLimit());
    // Success
    return ACCEPT;
  }
  /* -- FrameSetDelay ------------------------------------------------------ */
  CVarReturn FrameSetDelay(const unsigned uNDelay)
  { // Ignore if set over one second, any sort of app would not be usable with
    // over one second delay, so might as well cap it.
    if(uNDelay > 1000) return DENY;
    // Set new delay
    FrameUpdateDelay(uNDelay);
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
