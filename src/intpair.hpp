/* == INTPAIR.HPP ========================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## Sets up a flexible structure with two integers                      ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IIntPair {                   // Start of module namespace
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* ------------------------------------------------------------------------- */
template<typename Int>class IntPair
{ /* -- Private variables --------------------------------------- */ protected:
  Int              i1, i2;             // Two values of the specified type
  constexpr static const Int iD0 = static_cast<Int>(0), // 0 of specified type
  /* Some needed values */   iD1 = static_cast<Int>(1); // 1 of specified type
  /* -- Get ---------------------------------------------------------------- */
  template<typename RInt=Int>RInt IPGetOne(void) const
    { return static_cast<RInt>(i1); }
  template<typename RInt=Int>RInt IPGetTwo(void) const
    { return static_cast<RInt>(i2); }
  template<typename RInt,class RBase=IntPair<RInt>>RBase IPGet(void) const
    { return { IPGetOne<RInt>(), IPGetTwo<RInt>() }; }
  template<typename RInt=Int>RInt IPDefGet(void) const
    { return static_cast<RInt>(iD0); }
  /* -- Get reference ------------------------------------------------------ */
  Int &IPGetOneRef(void) { return i1; }
  Int &IPGetTwoRef(void) { return i2; }
  /* -- Set ---------------------------------------------------------------- */
  void IPSetOne(const Int iV) { i1 = iV; }
  void IPSetTwo(const Int iV) { i2 = iV; }
  void IPSet(const Int iV1, const Int iV2) { IPSetOne(iV1); IPSetTwo(iV2); }
  void IPSet(const Int iV = iD0) { IPSet(iV, iV); }
  void IPSet(const IntPair &ipO) { IPSet(ipO.IPGetOne(), ipO.IPGetTwo()); }
  void IPSwap(IntPair &ipO) { swap(i1, ipO.i1); swap(i2, ipO.i2); }
  /* -- Increment ---------------------------------------------------------- */
  void IPIncOne(const Int iV = iD1) { IPSetOne(IPGetOne() + iV); }
  void IPIncTwo(const Int iV = iD1) { IPSetTwo(IPGetTwo() + iV); }
  void IPInc(const Int iV1 = iD1) { IPIncOne(iV1); IPIncTwo(iV1); }
  void IPInc(const IntPair &ipOther) { IPIncOne(ipOther.IPGetOne());
                                       IPIncTwo(ipOther.IPGetTwo()); }
  /* -- Decrement ---------------------------------------------------------- */
  void IPDecOne(const Int iV = iD1) { IPSetOne(IPGetOne() - iV); }
  void IPDecTwo(const Int iV = iD1) { IPSetTwo(IPGetTwo() - iV); }
  void IPDec(const Int iV1 = iD1) { IPDecOne(iV1); IPDecTwo(iV1); }
  /* -- Test --------------------------------------------------------------- */
  bool IPIsOneSet(void) const { return IPGetOne() != IPDefGet(); }
  bool IPIsNotOneSet(void) const { return !IPIsOneSet(); }
  bool IPIsTwoSet(void) const { return IPGetTwo() != IPDefGet(); }
  bool IPIsNotTwoSet(void) const { return !IPIsTwoSet(); }
  bool IPIsSet(void) const { return IPIsOneSet() && IPIsTwoSet(); }
  bool IPIsNotSet(void) const { return !IPIsSet(); }
  /* ----------------------------------------------------------------------- */
  bool IPIsEqual(const IntPair &ipOther) const
    { return IPGetOne() == ipOther.IPGetOne() &&
             IPGetTwo() == ipOther.IPGetTwo(); }
  bool IPIsNotEqual(const IntPair &ipOther) const
    { return IPGetOne() != ipOther.IPGetOne() ||
             IPGetTwo() != ipOther.IPGetTwo(); }
  /* -- Copy assignment ---------------------------------------------------- */
  IntPair& operator=(const IntPair &ipO) { IPSet(ipO); return *this; }
  /* -- Initialisation on both values with one value constructor ----------- */
  explicit IntPair(const Int iV) :     // Specified value
    /* -- Initialisers ----------------------------------------------------- */
    i1(iV),                            // Initialise first value
    i2(iV)                             // Initialise second value
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Initialisation on both values constructor -------------------------- */
  IntPair(const Int iV1,               // Specified first value
          const Int iV2) :             // Specified second value
    /* -- Initialisers ----------------------------------------------------- */
    i1(iV1),                           // Initialise first value
    i2(iV2)                            // Initialise second value
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Copy constructor --------------------------------------------------- */
  IntPair(const IntPair &ipO) :        // Other class
    /* -- Initialisers ----------------------------------------------------- */
    IntPair{ ipO.i1, ipO.i2 }          // Call init ctor with values
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Default constructor ------------------------------------------------ */
  IntPair(void) :                      // No parameters
    /* -- Initialisers ----------------------------------------------------- */
    IntPair{ iD0, iD0 }                // Call init ctor with default values
    /* -- No code ---------------------------------------------------------- */
    { }
};/* ----------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of module namespace
/* == EoF =========================================================== EoF == */
