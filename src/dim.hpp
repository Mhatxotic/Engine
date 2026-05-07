/* == DIM.HPP ============================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## Sets up some basic dimension and coordinate classes to use          ## **
** ## throughout the engine.                                              ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IDim {                       // Start of module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IIntPair::P;           using namespace Lib::OS::GlFW::Types;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* ------------------------------------------------------------------------- */
template<typename IntType,             // Integer type to use
  class Base = IntPair<IntType>>       // Base class type to use
requires StdIsArithmatic<IntType>      // Must be be integer or float
struct Dimensions :                    // Members initially public
  /* -- Base classes ------------------------------------------------------- */
  private Base                         // Base double value class
{ /* -- Test --------------------------------------------------------------- */
  bool DimIsWidthSet() const { return this->IPIsOneSet(); }
  bool DimIsNotWidthSet() const { return this->IPIsNotOneSet(); }
  bool DimIsHeightSet() const { return this->IPIsTwoSet(); }
  bool DimIsNotHeightSet()const { return this->IPIsNotTwoSet(); }
  bool DimIsSet() const { return this->IPIsSet(); }
  bool DimIsNotSet() const { return this->IPIsNotSet(); }
  bool DimIsEqual(const Dimensions &dRef) const
    { return this->IPIsEqual(dRef); }
  bool DimIsNotEqual(const Dimensions &dRef) const
    { return this->IPIsNotEqual(dRef); }
  bool DimIsEqual(const IntType itW, const IntType itH) const
    { return this->IPIsEqual(itW, itH); }
  bool DimIsNotEqual(const IntType itW, const IntType itH) const
    { return this->IPIsNotEqual(itW, itH); }
  /* -- Get ---------------------------------------------------------------- */
  template<typename RIntType = IntType>
    requires StdIsArithmatic<RIntType>
  RIntType DimGetWidth() const
    { return this->template IPGetOne<RIntType>(); }
  template<typename RIntType = IntType>
    requires StdIsArithmatic<RIntType>
  RIntType DimGetHeight() const
    { return this->template IPGetTwo<RIntType>(); }
  template<typename RIntType, class RBase = Dimensions<RIntType>>
    requires StdIsArithmatic<RIntType>
  RBase DimGet() const { return this->template IPGet<RIntType,RBase>(); }
  template<typename RIntType = IntType>
    requires StdIsArithmatic<RIntType>
  RIntType DimDefGet() const
    { return this->template IPDefGet<RIntType>(); }
  /* -- Get reference ------------------------------------------------------ */
  IntType &DimGetWidthRef() { return this->IPGetOneRef(); }
  IntType &DimGetHeightRef() { return this->IPGetTwoRef(); }
  /* -- Set ---------------------------------------------------------------- */
  void DimSetWidth(const IntType itW) { this->IPSetOne(itW); }
  void DimSetHeight(const IntType itH) { this->IPSetTwo(itH); }
  void DimSet(const IntType itW, const IntType itH) { this->IPSet(itW, itH); }
  void DimSet(const IntType itV = Base::itD0) { this->IPSet(itV); }
  void DimSet(const Dimensions &dRef) { this->IPSet(dRef); }
  void DimSwap(Dimensions &dRef) { this->IPSwap(dRef); }
  /* -- Increment ---------------------------------------------------------- */
  void DimIncWidth(const IntType itW = Base::itD1) { this->IPIncOne(itW); }
  void DimIncHeight(const IntType itH = Base::itD1) { this->IPIncTwo(itH); }
  /* -- Decrement ---------------------------------------------------------- */
  void DimDecWidth(const IntType itW = Base::itD1) { this->IPDecOne(itW); }
  void DimDecHeight(const IntType itH = Base::itD1) { this->IPDecTwo(itH); }
  void DimDec(const IntType itV = Base::itD1) { this->IPDec(itV); }
  /* -- Initialisation of one value constructor ---------------------------- */
  explicit Dimensions(const IntType itBoth) :
    /* -- Initialisers ----------------------------------------------------- */
    Base{ itBoth }                     // Initialise specified values
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- Initialisation of both values constructor -------------------------- */
  Dimensions(const IntType itWidth, const IntType itHeight) :
    /* -- Initialisers ----------------------------------------------------- */
    Base{ itWidth, itHeight }          // Initialise specified values
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- Default constructor that does not need to do anything -------------- */
  Dimensions() = default;
};/* ----------------------------------------------------------------------- */
using DimGLFloat = Dimensions<GLfloat>;  // Dimension of GLfloats
using DimGLSizei = Dimensions<GLsizei>;  // Dimension of GLsizeis
using DimGLUInt  = Dimensions<GLuint>;   // Dimension of GLuints
using DimDouble  = Dimensions<double>;   // Dimension of doubles
using DimFloat   = Dimensions<float>;    // Dimension of floats
using DimInt     = Dimensions<int>;      // Dimension of ints
using DimUInt    = Dimensions<unsigned>; // Dimension of unsigned ints
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of module namespace
/* == EoF =========================================================== EoF == */
