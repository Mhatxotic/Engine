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
/* ------------------------------------------------------------------------- */
using namespace ICoord::P;             using namespace IIntPair::P;
using namespace Lib::OS::GlFW::Types;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* ------------------------------------------------------------------------- */
template<typename Int,                 // Integer type to use
         class Base = IntPair<Int>>    // Base class type to use
requires is_arithmetic_v<Int>          // Must be be integer or float
struct Dimensions :                    // Members initially public
  /* -- Base classes ------------------------------------------------------- */
  private Base                         // Base double value class
{ /* -- Get ---------------------------------------------------------------- */
  template<typename RInt=Int>RInt DimGetWidth() const noexcept
    { return this->template IPGetOne<RInt>(); }
  template<typename RInt=Int>RInt DimGetHeight() const noexcept
    { return this->template IPGetTwo<RInt>(); }
  template<typename RInt,class RBase=Dimensions<RInt>>
    RBase DimGet() const noexcept
      { return this->template IPGet<RInt,RBase>(); }
  template<typename RInt=Int>RInt DimDefGet() const noexcept
    { return this->template IPDefGet<RInt>(); }
  /* -- Get reference ------------------------------------------------------ */
  Int &DimGetWidthRef() noexcept { return this->IPGetOneRef(); }
  Int &DimGetHeightRef() noexcept { return this->IPGetTwoRef(); }
  /* -- Set ---------------------------------------------------------------- */
  void DimSetWidth(const Int iV) noexcept { this->IPSetOne(iV); }
  void DimSetHeight(const Int iV) noexcept { this->IPSetTwo(iV); }
  void DimSet(const Int iW, const Int iH) noexcept { this->IPSet(iW, iH); }
  void DimSet(const Int iV = Base::iD0) noexcept { this->IPSet(iV); }
  void DimSet(const Dimensions &dO) noexcept { this->IPSet(dO); }
  void DimSwap(Dimensions &dO) noexcept { this->IPSwap(dO); }
  /* -- Increment ---------------------------------------------------------- */
  void DimIncWidth(const Int iV = Base::iD1) noexcept { this->IPIncOne(iV); }
  void DimIncHeight(const Int iV = Base::iD1) noexcept { this->IPIncTwo(iV); }
  /* -- Decrement ---------------------------------------------------------- */
  void DimDecWidth(const Int iV = Base::iD1) noexcept { this->IPDecOne(iV); }
  void DimDecHeight(const Int iV = Base::iD1) noexcept { this->IPDecTwo(iV); }
  void DimDec(const Int iV = Base::iD1) noexcept { this->IPDec(iV); }
  /* -- Test --------------------------------------------------------------- */
  bool DimIsWidthSet() const noexcept { return this->IPIsOneSet(); }
  bool DimIsNotWidthSet() const noexcept { return this->IPIsNotOneSet(); }
  bool DimIsHeightSet() const noexcept { return this->IPIsTwoSet(); }
  bool DimIsNotHeightSet()const noexcept { return this->IPIsNotTwoSet(); }
  bool DimIsSet() const noexcept { return this->IPIsSet(); }
  bool DimIsNotSet() const noexcept { return this->IPIsNotSet(); }
  bool DimIsEqual(const Dimensions &dOther) const noexcept
    { return this->IPIsEqual(dOther); }
  bool DimIsNotEqual(const Dimensions &dOther) const noexcept
    { return this->IPIsNotEqual(dOther); }
  /* -- Initialisation of one value constructor ---------------------------- */
  explicit Dimensions(const Int itBoth) noexcept :
    /* -- Initialisers ----------------------------------------------------- */
    Base{ itBoth }                     // Initialise specified values
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- Initialisation of both values constructor -------------------------- */
  Dimensions(const Int itWidth, const Int itHeight) noexcept :
    /* -- Initialisers ----------------------------------------------------- */
    Base{ itWidth, itHeight }          // Initialise specified values
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- Default constructor that does not need to do anything -------------- */
  Dimensions() = default;
};/* ----------------------------------------------------------------------- */
typedef Dimensions<GLfloat>      DimGLFloat; // Dimension of GLfloats
typedef Dimensions<GLsizei>      DimGLSizei; // Dimension of GLsizeis
typedef Dimensions<GLuint>       DimGLUInt;  // Dimension of GLuints
typedef Dimensions<double>       DimDouble;  // Dimension of doubles
typedef Dimensions<float>        DimFloat;   // Dimension of floats
typedef Dimensions<int>          DimInt;     // Dimension of ints
typedef Dimensions<unsigned int> DimUInt;    // Dimension of unsigned ints
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of module namespace
/* == EoF =========================================================== EoF == */
