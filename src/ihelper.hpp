/* == IHELPER.HPP ========================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## This class holds the name of a class and the time it has been       ## **
** ## initialised and de-initialised. It also stops classes being         ## **
** ## initialised twice due to potential programming errors.              ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IHelper {                    // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IClock::P;             using namespace IError::P;
using namespace IIdent::P;             using namespace IString::P;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* ------------------------------------------------------------------------- */
class InitHelper :                     // The Init Helper class
  /* -- Base classes ------------------------------------------------------- */
  public IdentConst                    // Holds the identifier
{ /* -- Private variables -------------------------------------------------- */
  ClkTimePoint     ctInitialised,      // Time class was initialised
                   ctDeinitialised;    // Time class was deinitialised
  /* ----------------------------------------------------------------------- */
  void IHSetInitialised(void) { ctInitialised = cmHiRes.GetTime(); }
  void IHSetDeInitialised(void) { ctDeinitialised = cmHiRes.GetTime(); }
  /* --------------------------------------------------------------- */ public:
  bool IHIsInitialised(void) const { return ctInitialised > ctDeinitialised; }
  bool IHIsNotInitialised(void) const { return !IHIsInitialised(); }
  /* ----------------------------------------------------------------------- */
  void IHInitialise(void)
  { // Raise exception if object already initialised
    if(IHIsInitialised())
      XC("Object already initialised!",
         "Identifier", IdentGet(),
         "Age", StrShortFromDuration(cmHiRes.
                  TimePointToClampedDouble(ctInitialised)));
    // Class now initialised
    IHSetInitialised();
  }
  /* ----------------------------------------------------------------------- */
  bool IHDeInitialise(void)
  { // Return if class already initialised
    if(IHIsNotInitialised()) return false;
    // Class now de-initialised
    IHSetDeInitialised();
    // Success
    return true;
  }
  /* ----------------------------------------------------------------------- */
  bool IHNotDeInitialise(void) { return !IHDeInitialise(); }
  /* -- Constructors -------------------------------------------- */ protected:
  explicit InitHelper(const string_view &strvName) :
    /* -- Initialisers ----------------------------------------------------- */
    IdentConst{strvName},              // Initialise name
    ctInitialised{seconds{0}},         // Clear initialised time
    ctDeinitialised{cmHiRes.GetTime()} // Set deinitialised time
    /* -- No code ---------------------------------------------------------- */
    { }
};/* ----------------------------------------------------------------------- */
};                                     // End of private module namespace
/* ------------------------------------------------------------------------- */
};                                     // End of public module namespace
/* == EoF =========================================================== EoF == */
