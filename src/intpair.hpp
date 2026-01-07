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
template<typename IntType> requires is_arithmetic_v<IntType> class IntPair
{ /* -- Private variables --------------------------------------- */ protected:
  IntType              it1, it2;       // Two values of the specified type
  constexpr static const IntType itD0 = static_cast<IntType>(0),
                                 itD1 = static_cast<IntType>(1);
  /* -- Get ---------------------------------------------------------------- */
  template<typename RIntType=IntType>RIntType IPGetOne() const
    { return static_cast<RIntType>(it1); }
  template<typename RIntType=IntType>RIntType IPGetTwo() const
    { return static_cast<RIntType>(it2); }
  template<typename RIntType,class RBase=IntPair<RIntType>>RBase IPGet() const
    { return { IPGetOne<RIntType>(), IPGetTwo<RIntType>() }; }
  template<typename RIntType=IntType>static RIntType IPDefGet()
    { return static_cast<RIntType>(itD0); }
  /* -- Get reference ------------------------------------------------------ */
  IntType &IPGetOneRef() { return it1; }
  IntType &IPGetTwoRef() { return it2; }
  /* -- Set ---------------------------------------------------------------- */
  void IPSetOne(const IntType itV1) { it1 = itV1; }
  void IPSetTwo(const IntType itV2) { it2 = itV2; }
  void IPSet(const IntType itV1, const IntType itV2)
    { IPSetOne(itV1); IPSetTwo(itV2); }
  void IPSet(const IntType itV = itD0) { IPSet(itV, itV); }
  void IPSet(const IntPair &ipO) { IPSet(ipO.IPGetOne(), ipO.IPGetTwo()); }
  void IPSwap(IntPair &ipO) { swap(it1, ipO.it1); swap(it2, ipO.it2); }
  /* -- Increment ---------------------------------------------------------- */
  void IPIncOne(const IntType itV1 = itD1) { IPSetOne(IPGetOne() + itV1); }
  void IPIncTwo(const IntType itV2 = itD1) { IPSetTwo(IPGetTwo() + itV2); }
  void IPInc(const IntType itV = itD1) { IPIncOne(itV); IPIncTwo(itV); }
  void IPInc(const IntPair &ipRef)
    { IPIncOne(ipRef.IPGetOne()); IPIncTwo(ipRef.IPGetTwo()); }
  /* -- Decrement ---------------------------------------------------------- */
  void IPDecOne(const IntType itV1 = itD1) { IPSetOne(IPGetOne() - itV1); }
  void IPDecTwo(const IntType itV2 = itD1) { IPSetTwo(IPGetTwo() - itV2); }
  void IPDec(const IntType itV1 = itD1) { IPDecOne(itV1); IPDecTwo(itV1); }
  /* -- Test --------------------------------------------------------------- */
  bool IPIsOneSet() const { return IPGetOne() != IPDefGet(); }
  bool IPIsNotOneSet() const { return !IPIsOneSet(); }
  bool IPIsTwoSet() const { return IPGetTwo() != IPDefGet(); }
  bool IPIsNotTwoSet() const { return !IPIsTwoSet(); }
  bool IPIsSet() const { return IPIsOneSet() && IPIsTwoSet(); }
  bool IPIsNotSet() const { return !IPIsSet(); }
  /* ----------------------------------------------------------------------- */
  bool IPIsEqual(const IntPair &ipRef) const
    { return IPGetOne() == ipRef.IPGetOne() &&
             IPGetTwo() == ipRef.IPGetTwo(); }
  bool IPIsNotEqual(const IntPair &ipRef) const
    { return IPGetOne() != ipRef.IPGetOne() ||
             IPGetTwo() != ipRef.IPGetTwo(); }
  /* -- Copy assignment ---------------------------------------------------- */
  IntPair& operator=(const IntPair &ipRef) { IPSet(ipRef); return *this; }
  /* -- Initialisation on both values with one value constructor ----------- */
  explicit IntPair(const IntType itV) :
    /* -- Initialisers ----------------------------------------------------- */
    it1(itV),                          // Initialise first value
    it2(itV)                           // Initialise second value
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- Initialisation on both values constructor -------------------------- */
  IntPair(const IntType itV1, const IntType itV2) :
    /* -- Initialisers ----------------------------------------------------- */
    it1(itV1),                         // Initialise first value
    it2(itV2)                          // Initialise second value
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- Copy constructor --------------------------------------------------- */
  IntPair(const IntPair &ipO) :
    /* -- Initialisers ----------------------------------------------------- */
    IntPair{ ipO.it1, ipO.it2 }        // Call init ctor with values
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- Default constructor ------------------------------------------------ */
  IntPair() :
    /* -- Initialisers ----------------------------------------------------- */
    IntPair{ itD0, itD0 }              // Call init ctor with default values
    /* -- No code ---------------------------------------------------------- */
    {}
};/* ----------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of module namespace
/* == EoF =========================================================== EoF == */
