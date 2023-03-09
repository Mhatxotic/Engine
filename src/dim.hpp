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
template<typename Int = unsigned int>class IntPair
{ /* -- Private variables --------------------------------------- */ protected:
  Int              i1, i2;             // Two values of the specified type
  constexpr static const Int iD0 = static_cast<Int>(0), // 0 of specified type
  /* Some needed values */   iD1 = static_cast<Int>(1); // 1 of specified type
  /* -- Get ---------------------------------------------------------------- */
  template<typename RInt=Int>RInt IPGetOne(void) const
    { return static_cast<RInt>(i1); }
  template<typename RInt=Int>RInt IPGetTwo(void) const
    { return static_cast<RInt>(i2); }
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
  /* -- Copy assignment ---------------------------------------------------- */
  IntPair& operator=(const IntPair &ipO) { IPSet(ipO); return *this; }
  /* -- Initialisation constructor ----------------------------------------- */
  IntPair(const Int iV1,               // Specified first value
          const Int iV2) :             // Specified second value
    /* -- Initialisers ----------------------------------------------------- */
    i1(iV1),                           // Initialise first value
    i2(iV2)                            // Initialise first value
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
template<typename Int = unsigned int,  // Integer type to use
         class Base = IntPair<Int>>    // Base class type to use
struct Dimensions :                    // Members initially public
  /* -- Base classes ------------------------------------------------------- */
  private Base                         // Base double value class
{ /* -- Get ---------------------------------------------------------------- */
  template<typename RInt=Int>RInt DimGetWidth(void) const
    { return this->template IPGetOne<RInt>(); }
  template<typename RInt=Int>RInt DimGetHeight(void) const
    { return this->template IPGetTwo<RInt>(); }
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
  /* -- Initialisation constructor ----------------------------------------- */
  Dimensions(const Int itW,            // Specified width to initialise to
             const Int itH) :          // Specified height to initialise to
    /* -- Initialisers ----------------------------------------------------- */
    Base{ itW, itH }                   // Initialise specified values
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Default constructor that does not need to do anything -------------- */
  Dimensions(void)                     // No parameters
    /* -- No code ---------------------------------------------------------- */
    { }
};/* ----------------------------------------------------------------------- */
template<typename Int = unsigned int,  // Integer type to use
         class Base = IntPair<Int>>    // Base int pair class type to use
struct Coordinates :                   // Members initially public
  /* -- Base classes ------------------------------------------------------- */
  private Base                         // Base double value class
{ /* -- Get ---------------------------------------------------------------- */
  template<typename RInt=Int>RInt CoordGetX(void) const
    { return this->template IPGetOne<RInt>(); }
  template<typename RInt=Int>RInt CoordGetY(void) const
    { return this->template IPGetTwo<RInt>(); }
  template<typename RInt=Int>RInt CoordDefGet(void) const
    { return this->template IPDefGet<RInt>(); }
  /* -- Get reference ------------------------------------------------------ */
  Int &CoordGetXRef(void) { return this->IPGetOneRef(); }
  Int &CoordGetYRef(void) { return this->IPGetTwoRef(); }
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
  /* -- Decrement ---------------------------------------------------------- */
  void CoordDecX(const Int iV = Base::iD1) { this->IPDecOne(iV); }
  void CoordDecY(const Int iV = Base::iD1) { this->IPDecTwo(iV); }
  /* -- Test --------------------------------------------------------------- */
  bool CoordIsXSet(void) const { return this->IPIsOneSet(); }
  bool CoordIsNotXSet(void) const { return this->IPIsNotOneSet(); }
  bool CoordIsYSet(void) const { return this->IPIsTwoSet(); }
  bool CoordIsNotYSet(void) const { return this->IPIsNotTwoSet(); }
  bool CoordIsSet(void) const { return this->IPIsSet(); }
  bool CoordIsNotSet(void) const { return this->IPIsNotSet(); }
  /* -- Test operator ------------------------------------------------------ */
  operator bool(void) const { return CoordIsSet(); }
  /* -- Initialisation constructor ----------------------------------------- */
  Coordinates(const Int itX, const Int itY) :
    /* -- Initialisers ----------------------------------------------------- */
    Base{ itX, itY }                   // Initialise specified values
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Default constructor that does not need to do anything -------------- */
  Coordinates(void)          // No parameters
    /* -- No code ---------------------------------------------------------- */
    { }
};/* ----------------------------------------------------------------------- */
template<typename DInt = unsigned int, // Dimensions integer type
         typename CInt = DInt,         // Coords class integer type
         class DClass  = Dimensions<DInt>,  // Dimensions class to use
         class CClass  = Coordinates<CInt>> // Coords class to use
struct DimCoords :                     // Members initially public
  /* -- Base classes ------------------------------------------------------- */
  public CClass,                       // Co-ordinates class
  public DClass                        // Dimensions class
{ /* -- Set co-ordinates and dimensions ------------------------------------ */
  void DimCoSet(const CInt ciX, const CInt ciY, const DInt diW,
    const DInt diH)
  { this->CoordSet(ciX, ciY); this->DimSet(diW, diH); }
  /* -- Constructor to fill all parameters --------------------------------- */
  DimCoords(const CInt ciX, const CInt ciY, const DInt diW,
    const DInt diH) :
    /* -- Initialisers ----------------------------------------------------- */
    CClass{ ciX, ciY },                // Initialise co-ordinates
    DClass{ diW, diH }                 // Initialise dimensions
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Default constructor that zero initialises everything --------------- */
  DimCoords(void)            // No parameters
    /* -- No code ---------------------------------------------------------- */
    { }
};/* ----------------------------------------------------------------------- */
typedef DimCoords<int> DimCoInt;       // Dimension Cordinates typedef
/* ------------------------------------------------------------------------- */
template<typename Int = unsigned int,  // The integer type to use
         class Class = Coordinates<Int>> // Coordinates class to use
class Rectangle                        // Members initially private
{ /* -- Values ------------------------------------------------------------- */
  Class            co1, co2;           // Co-ordinates
  /* -- Set -------------------------------------------------------- */ public:
  void RectSetX1(const Int iV = Class::iD1) { co1.CoordSetX(iV); }
  void RectSetY1(const Int iV = Class::iD1) { co1.CoordSetY(iV); }
  void RectSetX2(const Int iV = Class::iD1) { co2.CoordSetX(iV); }
  void RectSetY2(const Int iV = Class::iD1) { co2.CoordSetY(iV); }
  /* -- Increment ---------------------------------------------------------- */
  void RectIncX1(const Int iV = Class::iD1) { co1.CoordIncX(iV); }
  void RectIncY1(const Int iV = Class::iD1) { co1.CoordIncY(iV); }
  void RectIncX2(const Int iV = Class::iD1) { co2.CoordIncX(iV); }
  void RectIncY2(const Int iV = Class::iD1) { co2.CoordIncY(iV); }
  /* -- Decrement ---------------------------------------------------------- */
  void RectDecX1(const Int iV = Class::iD1) { co1.CoordDecX(iV); }
  void RectDecY1(const Int iV = Class::iD1) { co1.CoordDecY(iV); }
  void RectDecX2(const Int iV = Class::iD1) { co2.CoordDecX(iV); }
  void RectDecY2(const Int iV = Class::iD1) { co2.CoordDecY(iV); }
  /* -- Set co-ordinates --------------------------------------------------- */
  void RectSetTopLeft(const Int itX, const Int itY) { co1.CoordSet(itX, itY); }
  void RectSetTopLeft(const Int iV) { RectSetTopLeft(iV, iV); }
  void RectSetBottomRight(const Int itX, const Int itY)
    { co2.CoordSet(itX, itY); }
  void RectSetBottomRight(const Int iV) { RectSetBottomRight(iV, iV); }
  /* -- Set more ----------------------------------------------------------- */
  void RectSet(const Int itX1, const Int itY1, const Int itX2, const Int itY2)
    { RectSetTopLeft(itX1, itY1); RectSetBottomRight(itX2, itY2); }
  /* -- Get ---------------------------------------------------------------- */
  template<typename RInt=Int>RInt RectGetX1(void) const
    { return co1.template CoordGetX<RInt>(); }
  template<typename RInt=Int>RInt RectGetY1(void) const
    { return co1.template CoordGetY<RInt>(); }
  template<typename RInt=Int>RInt RectGetX2(void) const
    { return co2.template CoordGetX<RInt>(); }
  template<typename RInt=Int>RInt RectGetY2(void) const
    { return co2.template CoordGetY<RInt>(); }
  /* -- Constructor to fill all parameters --------------------------------- */
  Rectangle(const Int itX1,            // First co-ordinate X position
            const Int itY1,            // First co-ordinate Y position
            const Int itX2,            // Second co-ordinate X position
            const Int itY2):           // Second co-ordinate Y position
    /* -- Initialisers ----------------------------------------------------- */
    co1{ itX1, itY1 },                 // Initialise first co-ordinates
    co2{ itX2, itY2 }                  // Initialise second co-ordinates
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Default constructor ------------------------------------------------ */
  Rectangle(void)                      // No parameters
    /* -- No code ---------------------------------------------------------- */
    { }
};/* ----------------------------------------------------------------------- */
}                                      // End of module namespace
/* == EoF =========================================================== EoF == */
