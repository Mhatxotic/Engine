/* == COORD.HPP ============================================================ **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## Sets up co-ordinate structure with a set of co-ordinates.           ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace ICoord {                     // Start of module namespace
/* ------------------------------------------------------------------------- */
using namespace IIntPair::P;           using namespace Lib::OS::GlFW::Types;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* ------------------------------------------------------------------------- */
template<typename IntType,             // Integer type to use
  class Base = IntPair<IntType>>       // Base int pair class type to use
requires is_arithmetic_v<IntType>      // Must be be integer or float
struct Coord :                         // Members initially public
  /* -- Base classes ------------------------------------------------------- */
  private Base                         // Base double value class
{ /* -- Get ---------------------------------------------------------------- */
  template<typename RInt=IntType>RInt CoordGetX() const
    { return this->template IPGetOne<RInt>(); }
  template<typename RInt=IntType>RInt CoordGetY() const
    { return this->template IPGetTwo<RInt>(); }
  template<typename RInt=IntType>RInt CoordDefGet() const
    { return this->template IPDefGet<RInt>(); }
  /* -- Get reference ------------------------------------------------------ */
  IntType &CoordGetXRef() { return this->IPGetOneRef(); }
  IntType &CoordGetYRef() { return this->IPGetTwoRef(); }
  /* -- Set ---------------------------------------------------------------- */
  void CoordSetX(const IntType itV) { this->IPSetOne(itV); }
  void CoordSetY(const IntType itV) { this->IPSetTwo(itV); }
  void CoordSet(const IntType itX, const IntType itY) { this->IPSet(itX,itY); }
  void CoordSet(const IntType itV = Base::itD0) { this->IPSet(itV); }
  void CoordSet(const Coord &cRef) { this->IPSet(cRef); }
  void CoordSwap(Coord &cRef) { this->IPSwap(cRef); }
  /* -- Increment ---------------------------------------------------------- */
  void CoordIncX(const IntType itV = Base::itD1) { this->IPIncOne(itV); }
  void CoordIncY(const IntType itV = Base::itD1) { this->IPIncTwo(itV); }
  void CoordInc(const IntType itX = Base::itD1, const IntType itY = Base::itD1)
    { this->IPIncOne(itX); this->IPIncTwo(itY);}
  void CoordInc(const Coord &cOther) { this->IPInc(cOther); }
  /* -- Decrement ---------------------------------------------------------- */
  void CoordDecX(const IntType itV = Base::itD1) { this->IPDecOne(itV); }
  void CoordDecY(const IntType itV = Base::itD1) { this->IPDecTwo(itV); }
  /* -- Test --------------------------------------------------------------- */
  bool CoordIsXSet() const { return this->IPIsOneSet(); }
  bool CoordIsNotXSet() const { return this->IPIsNotOneSet(); }
  bool CoordIsYSet() const { return this->IPIsTwoSet(); }
  bool CoordIsNotYSet() const { return this->IPIsNotTwoSet(); }
  bool CoordIsSet() const { return this->IPIsSet(); }
  bool CoordIsNotSet() const { return this->IPIsNotSet(); }
  bool CoordIsEqual(const Coord &cOther) const
    { return this->IPIsEqual(cOther); }
  bool CoordIsNotEqual(const Coord &cOther) const
    { return this->IPIsNotEqual(cOther); }
  /* -- Test operator ------------------------------------------------------ */
  operator bool() const { return CoordIsSet(); }
  /* -- Initialisation of one value constructor ---------------------------- */
  explicit Coord(const IntType itBoth) :
    /* -- Initialisers ----------------------------------------------------- */
    Base{ itBoth }                     // Initialise specified values
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- Initialisation of both values constructor -------------------------- */
  Coord(const IntType itX, const IntType itY) :
    /* -- Initialisers ----------------------------------------------------- */
    Base{ itX, itY }                   // Initialise specified values
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- Default constructor that does not need to do anything -------------- */
  Coord() = default;
};/* ----------------------------------------------------------------------- */
typedef Coord<int> CoordInt;           // Cordinates typedef
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of module namespace
/* == EoF =========================================================== EoF == */
