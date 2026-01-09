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
template<typename Int> requires is_arithmetic_v<Int> class IntPair
{ /* -- Private variables --------------------------------------- */ protected:
  Int              i1, i2;             // Two values of the specified type
  constexpr static const Int iD0 = static_cast<Int>(0), // 0 of specified type
  /* Some needed values */   iD1 = static_cast<Int>(1); // 1 of specified type
  /* -- Get ---------------------------------------------------------------- */
  template<typename RInt=Int>RInt IPGetOne() const noexcept
    { return static_cast<RInt>(i1); }
  template<typename RInt=Int>RInt IPGetTwo() const noexcept
    { return static_cast<RInt>(i2); }
  template<typename RInt,class RBase=IntPair<RInt>>RBase IPGet() const noexcept
    { return { IPGetOne<RInt>(), IPGetTwo<RInt>() }; }
  template<typename RInt=Int>static RInt IPDefGet() noexcept
    { return static_cast<RInt>(iD0); }
  /* -- Get reference ------------------------------------------------------ */
  Int &IPGetOneRef() noexcept { return i1; }
  Int &IPGetTwoRef() noexcept { return i2; }
  /* -- Set ---------------------------------------------------------------- */
  void IPSetOne(const Int iV) noexcept { i1 = iV; }
  void IPSetTwo(const Int iV) noexcept { i2 = iV; }
  void IPSet(const Int iV1, const Int iV2) noexcept
    { IPSetOne(iV1); IPSetTwo(iV2); }
  void IPSet(const Int iV = iD0) noexcept { IPSet(iV, iV); }
  void IPSet(const IntPair &ipO) noexcept
    { IPSet(ipO.IPGetOne(), ipO.IPGetTwo()); }
  void IPSwap(IntPair &ipO) noexcept { swap(i1, ipO.i1); swap(i2, ipO.i2); }
  /* -- Increment ---------------------------------------------------------- */
  void IPIncOne(const Int iV = iD1) noexcept { IPSetOne(IPGetOne() + iV); }
  void IPIncTwo(const Int iV = iD1) noexcept { IPSetTwo(IPGetTwo() + iV); }
  void IPInc(const Int iV1 = iD1) noexcept { IPIncOne(iV1); IPIncTwo(iV1); }
  void IPInc(const IntPair &ipOther) noexcept
    { IPIncOne(ipOther.IPGetOne()); IPIncTwo(ipOther.IPGetTwo()); }
  /* -- Decrement ---------------------------------------------------------- */
  void IPDecOne(const Int iV = iD1) noexcept { IPSetOne(IPGetOne() - iV); }
  void IPDecTwo(const Int iV = iD1) noexcept { IPSetTwo(IPGetTwo() - iV); }
  void IPDec(const Int iV1 = iD1) noexcept { IPDecOne(iV1); IPDecTwo(iV1); }
  /* -- Test --------------------------------------------------------------- */
  bool IPIsOneSet() const noexcept { return IPGetOne() != IPDefGet(); }
  bool IPIsNotOneSet() const noexcept { return !IPIsOneSet(); }
  bool IPIsTwoSet() const noexcept { return IPGetTwo() != IPDefGet(); }
  bool IPIsNotTwoSet() const noexcept { return !IPIsTwoSet(); }
  bool IPIsSet() const noexcept { return IPIsOneSet() && IPIsTwoSet(); }
  bool IPIsNotSet() const noexcept { return !IPIsSet(); }
  /* ----------------------------------------------------------------------- */
  bool IPIsEqual(const IntPair &ipOther) const noexcept
    { return IPGetOne() == ipOther.IPGetOne() &&
             IPGetTwo() == ipOther.IPGetTwo(); }
  bool IPIsNotEqual(const IntPair &ipOther) const noexcept
    { return IPGetOne() != ipOther.IPGetOne() ||
             IPGetTwo() != ipOther.IPGetTwo(); }
  /* -- Copy assignment ---------------------------------------------------- */
  IntPair& operator=(const IntPair &ipO) noexcept { IPSet(ipO); return *this; }
  /* -- Initialisation on both values with one value constructor ----------- */
  explicit IntPair(const Int iV) noexcept :
    /* -- Initialisers ----------------------------------------------------- */
    i1(iV),                            // Initialise first value
    i2(iV)                             // Initialise second value
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- Initialisation on both values constructor -------------------------- */
  IntPair(const Int iV1, const Int iV2) noexcept :
    /* -- Initialisers ----------------------------------------------------- */
    i1(iV1),                           // Initialise first value
    i2(iV2)                            // Initialise second value
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- Copy constructor --------------------------------------------------- */
  IntPair(const IntPair &ipO) noexcept :
    /* -- Initialisers ----------------------------------------------------- */
    IntPair{ ipO.i1, ipO.i2 }          // Call init ctor with values
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- Default constructor ------------------------------------------------ */
  IntPair() noexcept :
    /* -- Initialisers ----------------------------------------------------- */
    IntPair{ iD0, iD0 }                // Call init ctor with default values
    /* -- No code ---------------------------------------------------------- */
    {}
};/* ----------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of module namespace
/* == EoF =========================================================== EoF == */
