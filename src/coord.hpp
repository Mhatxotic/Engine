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
struct Coordinates :                   // Members initially public
  /* -- Base classes ------------------------------------------------------- */
  private Base                         // Base double value class
{ /* -- Get ---------------------------------------------------------------- */
  template<typename RInt=Int>RInt CoordGetX() const
    { return this->template IPGetOne<RInt>(); }
  template<typename RInt=Int>RInt CoordGetY() const
    { return this->template IPGetTwo<RInt>(); }
  template<typename RInt=Int>RInt CoordDefGet() const
    { return this->template IPDefGet<RInt>(); }
  /* -- Get reference ------------------------------------------------------ */
  Int &CoordGetXRef() { return this->IPGetOneRef(); }
  Int &CoordGetYRef() { return this->IPGetTwoRef(); }
  /* -- Set ---------------------------------------------------------------- */
  void CoordSetX(const Int iV) { this->IPSetOne(iV); }
  void CoordSetY(const Int iV) { this->IPSetTwo(iV); }
  void CoordSet(const Int itX, const Int itY) { this->IPSet(itX,itY); }
  void CoordSet(const Int iV = Base::iD0) { this->IPSet(iV); }
  void CoordSet(const Coordinates &cO) { this->IPSet(cO); }
  void CoordSwap(Coordinates &cO) { this->IPSwap(cO); }
  /* -- Increment ---------------------------------------------------------- */
  void CoordIncX(const Int iV = Base::iD1) { this->IPIncOne(iV); }
  void CoordIncY(const Int iV = Base::iD1) { this->IPIncTwo(iV); }
  void CoordInc(const Int iX = Base::iD1, const Int iY = Base::iD1)
    { this->IPIncOne(iX); this->IPIncTwo(iY);}
  void CoordInc(const Coordinates &cOther) { this->IPInc(cOther); }
  /* -- Decrement ---------------------------------------------------------- */
  void CoordDecX(const Int iV = Base::iD1) { this->IPDecOne(iV); }
  void CoordDecY(const Int iV = Base::iD1) { this->IPDecTwo(iV); }
  /* -- Test --------------------------------------------------------------- */
  bool CoordIsXSet() const { return this->IPIsOneSet(); }
  bool CoordIsNotXSet() const { return this->IPIsNotOneSet(); }
  bool CoordIsYSet() const { return this->IPIsTwoSet(); }
  bool CoordIsNotYSet() const { return this->IPIsNotTwoSet(); }
  bool CoordIsSet() const { return this->IPIsSet(); }
  bool CoordIsNotSet() const { return this->IPIsNotSet(); }
  bool CoordIsEqual(const Coordinates &cOther) const
    { return this->IPIsEqual(cOther); }
  bool CoordIsNotEqual(const Coordinates &cOther) const
    { return this->IPIsNotEqual(cOther); }
  /* -- Test operator ------------------------------------------------------ */
  operator bool() const { return CoordIsSet(); }
  /* -- Initialisation of one value constructor ---------------------------- */
  explicit Coordinates(const Int itV) : // Specified value to initialise to
    /* -- Initialisers ----------------------------------------------------- */
    Base{ itV }                        // Initialise specified values
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- Initialisation of both values constructor -------------------------- */
  Coordinates(const Int itX,           // Specified X coord to initialise to
              const Int itY) :         // Specified Y coord to initialise to
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
