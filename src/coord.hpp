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
template<typename Int = unsigned int,  // Integer type to use
         class Base = IntPair<Int>>    // Base int pair class type to use
requires is_arithmetic_v<Int>          // Must be be integer or float
struct Coordinates :                   // Members initially public
  /* -- Base classes ------------------------------------------------------- */
  private Base                         // Base double value class
{ /* -- Get ---------------------------------------------------------------- */
  template<typename RInt=Int>RInt CoordGetX() const noexcept
    { return this->template IPGetOne<RInt>(); }
  template<typename RInt=Int>RInt CoordGetY() const noexcept
    { return this->template IPGetTwo<RInt>(); }
  template<typename RInt=Int>RInt CoordDefGet() const noexcept
    { return this->template IPDefGet<RInt>(); }
  /* -- Get reference ------------------------------------------------------ */
  Int &CoordGetXRef() noexcept { return this->IPGetOneRef(); }
  Int &CoordGetYRef() noexcept { return this->IPGetTwoRef(); }
  /* -- Set ---------------------------------------------------------------- */
  void CoordSetX(const Int iV) noexcept { this->IPSetOne(iV); }
  void CoordSetY(const Int iV) noexcept { this->IPSetTwo(iV); }
  void CoordSet(const Int itX, const Int itY) noexcept
    { this->IPSet(itX,itY); }
  void CoordSet(const Int iV = Base::iD0) noexcept { this->IPSet(iV); }
  void CoordSet(const Coordinates &cO) noexcept { this->IPSet(cO); }
  void CoordSwap(Coordinates &cO) noexcept { this->IPSwap(cO); }
  /* -- Increment ---------------------------------------------------------- */
  void CoordIncX(const Int iV = Base::iD1) noexcept { this->IPIncOne(iV); }
  void CoordIncY(const Int iV = Base::iD1) noexcept { this->IPIncTwo(iV); }
  void CoordInc(const Int iX = Base::iD1, const Int iY = Base::iD1) noexcept
    { this->IPIncOne(iX); this->IPIncTwo(iY);}
  void CoordInc(const Coordinates &cOther) noexcept { this->IPInc(cOther); }
  /* -- Decrement ---------------------------------------------------------- */
  void CoordDecX(const Int iV = Base::iD1) noexcept { this->IPDecOne(iV); }
  void CoordDecY(const Int iV = Base::iD1) noexcept { this->IPDecTwo(iV); }
  /* -- Test --------------------------------------------------------------- */
  bool CoordIsXSet() const noexcept { return this->IPIsOneSet(); }
  bool CoordIsNotXSet() const noexcept { return this->IPIsNotOneSet(); }
  bool CoordIsYSet() const noexcept { return this->IPIsTwoSet(); }
  bool CoordIsNotYSet() const noexcept { return this->IPIsNotTwoSet(); }
  bool CoordIsSet() const noexcept { return this->IPIsSet(); }
  bool CoordIsNotSet() const noexcept { return this->IPIsNotSet(); }
  bool CoordIsEqual(const Coordinates &cOther) const noexcept
    { return this->IPIsEqual(cOther); }
  bool CoordIsNotEqual(const Coordinates &cOther) const noexcept
    { return this->IPIsNotEqual(cOther); }
  /* -- Test operator ------------------------------------------------------ */
  operator bool() const noexcept { return CoordIsSet(); }
  /* -- Initialisation of one value constructor ---------------------------- */
  explicit Coordinates(const Int itV) noexcept :
    /* -- Initialisers ----------------------------------------------------- */
    Base{ itV }                        // Initialise specified values
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- Initialisation of both values constructor -------------------------- */
  Coordinates(const Int itX, const Int itY) noexcept :
    /* -- Initialisers ----------------------------------------------------- */
    Base{ itX, itY }                   // Initialise specified values
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- Default constructor that does not need to do anything -------------- */
  Coordinates() = default;
};/* ----------------------------------------------------------------------- */
typedef Coordinates<int> CoordInt;     // Cordinates typedef
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of module namespace
/* == EoF =========================================================== EoF == */
