/* == CHRONO.HPP =========================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## This class defines a chronometer that records the time and then     ## **
** ## can return the duration from that time point.                       ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IChrono {                    // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IClock::P;             using namespace IUtil::P;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* ------------------------------------------------------------------------- */
class Chrono
{ /* -- Private variables -------------------------------------------------- */
  ClkTimePoint     ctpStart;           // Don't make this a base class
  /* -- Subtract specified time from the stored time and return as - */ public:
  double CCDeltaRangeToDouble(const ClkTimePoint &ctpEnd) const
    { return ClockDurationToDouble(ctpEnd - ctpStart); }
  /* -- Same as above but clamps to zero so there is no negative time ------ */
  double CCDeltaToClampedDouble(const ClkTimePoint &ctpEnd) const
    { return UtilMaximum(CCDeltaRangeToDouble(ctpEnd), 0.0); }
  /* -- Return uptime as milliseconds in a 64-bit uint --------------------- */
  uint64_t CCDeltaMS() const
    { return static_cast<uint64_t>(
        cmHiRes.GetDurationCount<milliseconds>(ctpStart)); }
  /* -- Return uptime as nanoseconds in a 64-bit uint ---------------------- */
  uint64_t CCDeltaNS() const
    { return static_cast<uint64_t>(
        cmHiRes.GetDurationCount<nanoseconds>(ctpStart)); }
  /* -- Return uptime as microseconds in a 64-bit uint --------------------- */
  uint64_t CCDeltaUS() const
    { return static_cast<uint64_t>(
        cmHiRes.GetDurationCount<microseconds>(ctpStart)); }
  /* -- Return uptime as a double ------------------------------------------ */
  double CCDeltaToDouble() const
    { return cmHiRes.TimePointToDouble(ctpStart); }
  /* -- Reset the start time ----------------------------------------------- */
  void CCReset() { ctpStart = cmHiRes.GetTime(); }
  /* -- Constructor. Just initialise current time -------------------------- */
  Chrono() :
    /* -- Initialisers ----------------------------------------------------- */
    ctpStart{ cmHiRes.GetTime() }      // Set start time
    /* -- No code ---------------------------------------------------------- */
    {}
};/* ----------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
