/* == INTERVAL.HPP ========================================================= **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## Defines a clock interval class which makes it easier to manage      ## **
** ## timing intervals.                                                   ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IInterval {                  // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IClock::P;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* -- Interval helper ------------------------------------------------------ */
class Interval
{ /* -- Variables ---------------------------------------------------------- */
  ClkDuration      cdLimit;            // Time delay before trigger
  ClkTimePoint     ctpNext;            // Next trigger
  /* -- Returns if time + this duration not elapsed yet ------------ */ public:
  bool CIIsNotTriggeredIn(const ClkDuration &cdT) const
    { return cmHiRes.GetTime() + cdT < ctpNext; }
  /* -- Returns if timepoint not elapsed yet ------------------------------- */
  bool CIIsNotTriggered(const ClkTimePoint &ctpT) const
    { return ctpT < ctpNext; }
  /* -- Returns if current timepoint not elapsed yet ----------------------- */
  bool CIIsNotTriggered() const
    { return CIIsNotTriggered(cmHiRes.GetTime()); }
  /* -- Returns if current timepoint elapsed ------------------------------- */
  bool CIIsTriggered() const { return !CIIsNotTriggered(); }
  /* -- Add time to next limit --------------------------------------------- */
  void CIAccumulate() { ctpNext += cdLimit; }
  /* -- Time elapsed? ------------------------------------------------------ */
  bool CIIsTrigger()
  { // Get current high res time
    const ClkTimePoint ctpNow{ cmHiRes.GetTime() };
    // Return false if time hasn't elapsed yet
    if(CIIsNotTriggered(ctpNow)) return false;
    // Set next time strictly and not accumulate it
    ctpNext = ctpNow + cdLimit;
    // Success
    return true;
  }
  /* -- Time not elapsed? -------------------------------------------------- */
  bool CIIsNotTrigger() { return !CIIsTrigger(); }
  /* -- Reset trigger ------------------------------------------------------ */
  void CIReset() { ctpNext = cmHiRes.GetTime() + cdLimit; }
  /* -- Return time left --------------------------------------------------- */
  const ClkDuration CIDelta() const { return cmHiRes.GetTime() - ctpNext; }
  /* -- Sync now ----------------------------------------------------------- */
  void CISync() { ctpNext = cmHiRes.GetTime(); }
  /* -- Update limit and time now do a duration object --------------------- */
  void CISetLimit(const ClkDuration &duL) { cdLimit = duL; CISync(); }
  /* -- Update limit and time now to a double ------------------------------ */
  void CISetLimit(const double dL)
    { CISetLimit(duration_cast<ClkDuration>(duration<double>(dL))); }
  /* -- Constructor -------------------------------------------------------- */
  Interval() :
    /* -- Initialisers ----------------------------------------------------- */
    ctpNext{ cmHiRes.GetTime() }      // Will trigger next check
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- Constructor (set limit by lvalue) ---------------------------------- */
  explicit Interval(const ClkDuration &duL) :
    /* -- Initialisers ----------------------------------------------------- */
    cdLimit{ duL },                    // Copy limit from other class
    ctpNext{ cmHiRes.GetTime() }       // Will trigger next check
    /* -- No code ---------------------------------------------------------- */
    {}
}; /* -- End --------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
