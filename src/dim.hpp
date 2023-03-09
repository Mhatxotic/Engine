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
struct Dimensions :                    // Members initially public
  /* -- Base classes ------------------------------------------------------- */
  private Base                         // Base double value class
{ /* -- Get ---------------------------------------------------------------- */
  template<typename RInt=Int>RInt DimGetWidth(void) const
    { return this->template IPGetOne<RInt>(); }
  template<typename RInt=Int>RInt DimGetHeight(void) const
    { return this->template IPGetTwo<RInt>(); }
  template<typename RInt,class RBase=Dimensions<RInt>>RBase DimGet(void) const
    { return this->template IPGet<RInt,RBase>(); }
  template<typename RInt=Int>RInt DimDefGet(void) const
    { return this->template IPDefGet<RInt>(); }
  /* -- Get reference ------------------------------------------------------ */
  Int &DimGetWidthRef(void) { return this->IPGetOneRef(); }
  Int &DimGetHeightRef(void) { return this->IPGetTwoRef(); }
  /* -- Set ---------------------------------------------------------------- */
  void DimSetWidth(const Int iV) { this->IPSetOne(iV); }
  void DimSetHeight(const Int iV) { this->IPSetTwo(iV); }
  void DimSet(const Int iW, const Int iH) { this->IPSet(iW, iH); }
  void DimSet(const Int iV = Base::iD0) { this->IPSet(iV); }
  void DimSet(const Dimensions &dO) { this->IPSet(dO); }
  void DimSwap(Dimensions &dO) { this->IPSwap(dO); }
  /* -- Increment ---------------------------------------------------------- */
  void DimIncWidth(const Int iV = Base::iD1) { this->IPIncOne(iV); }
  void DimIncHeight(const Int iV = Base::iD1) { this->IPIncTwo(iV); }
  /* -- Decrement ---------------------------------------------------------- */
  void DimDecWidth(const Int iV = Base::iD1) { this->IPDecOne(iV); }
  void DimDecHeight(const Int iV = Base::iD1) { this->IPDecTwo(iV); }
  void DimDec(const Int iV = Base::iD1) { this->IPDec(iV); }
  /* -- Test --------------------------------------------------------------- */
  bool DimIsWidthSet(void) const { return this->IPIsOneSet(); }
  bool DimIsNotWidthSet(void) const { return this->IPIsNotOneSet(); }
  bool DimIsHeightSet(void) const { return this->IPIsTwoSet(); }
  bool DimIsNotHeightSet(void)const { return this->IPIsNotTwoSet(); }
  bool DimIsSet(void) const { return this->IPIsSet(); }
  bool DimIsNotSet(void) const { return this->IPIsNotSet(); }
  bool DimIsEqual(const Dimensions &dOther) const
    { return this->IPIsEqual(dOther); }
  bool DimIsNotEqual(const Dimensions &dOther) const
    { return this->IPIsNotEqual(dOther); }
  /* -- Initialisation of one value constructor ---------------------------- */
  explicit Dimensions(const Int itV) : // Specified value to initialise to
    /* -- Initialisers ----------------------------------------------------- */
    Base{ itV }                        // Initialise specified values
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Initialisation of both values constructor -------------------------- */
  Dimensions(const Int itW,            // Specified width to initialise to
             const Int itH) :          // Specified height to initialise to
    /* -- Initialisers ----------------------------------------------------- */
    Base{ itW, itH }                   // Initialise specified values
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Default constructor that does not need to do anything -------------- */
  Dimensions(void) = default;
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
