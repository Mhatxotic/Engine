/* == COORDS.HPP =========================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## Defines the 'Coords' class which has methods for dealing with       ## **
** ## co-ordinates and makes use of the 'Coord' class from 'coord.hpp'.   ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace ICoords {                    // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace ICoord::P;             using Lib::OS::GlFW::GLfloat;
using Lib::OS::GlFW::GLuint;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* == Fbo coords class ===================================================== */
template<typename IntType> requires is_arithmetic_v<IntType> class Coords
{ /* -- Private variables -------------------------------------------------- */
  typedef Coord<IntType> CoordType;    // Co-ordinates container type
  CoordType        ctXY1, ctXY2;       // Top-left/right-bottom co-ordinates
  /* -- Get co-ordinate -------------------------------------------- */ public:
  template<typename RIntType=IntType>RIntType CoordsGetLeft() const
    { return ctXY1.template CoordGetX<RIntType>(); }
  template<typename RIntType=IntType>RIntType CoordsGetTop() const
    { return ctXY1.template CoordGetY<RIntType>(); }
  template<typename RIntType=IntType>RIntType CoordsGetRight() const
    { return ctXY2.template CoordGetX<RIntType>(); }
  template<typename RIntType=IntType>RIntType CoordsGetBottom() const
    { return ctXY2.template CoordGetY<RIntType>(); }
  /* -- Set co-ordinate by integral values --------------------------------- */
  void CoordsSetLeft(const IntType itNLeft) { ctXY1.CoordSetX(itNLeft); }
  void CoordsSetTop(const IntType itNTop) { ctXY1.CoordSetY(itNTop); }
  void CoordsSetRight(const IntType itNRight) { ctXY2.CoordSetX(itNRight); }
  void CoordsSetBottom(const IntType itNBottom) { ctXY2.CoordSetY(itNBottom);}
  /* -- Set co-ordinate by another struct ---------------------------------- */
  void CoordsSetLeft(const Coords &cRef)
    { CoordsSetLeft(cRef.CoordsGetLeft()); }
  void CoordsSetTop(const Coords &cRef) { CoordsSetTop(cRef.CoordsGetTop()); }
  void CoordsSetRight(const Coords &cRef)
    { CoordsSetRight(cRef.CoordsGetRight()); }
  void CoordsSetBottom(const Coords &cRef)
    { CoordsSetBottom(cRef.CoordsGetBottom()); }
  /* -- Set two co-ordinates ----------------------------------------------- */
  void CoordsSetTopLeft(const IntType itX, const IntType itY)
    { ctXY1.CoordSet(itX, itY); }
  void CoordsSetTopLeft(const IntType itV)
    { CoordsSetTopLeft(itV, itV); }
  void CoordsSetBottomRight(const IntType itX, const IntType itY)
    { ctXY2.CoordSet(itX, itY); }
  void CoordsSetBottomRight(const IntType itV)
    { CoordsSetBottomRight(itV, itV); }
  /* -- Reset co-ordinates ------------------------------------------------- */
  void CoordsReset() { ctXY1.CoordSet(); ctXY2.CoordSet(); }
  /* -- Set all co-ordinates by integral values ---------------------------- */
  void CoordsSet(const IntType itNLeft, const IntType itNTop,
    const IntType itNRight, const IntType itNBottom)
  { CoordsSetLeft(itNLeft); CoordsSetTop(itNTop); CoordsSetRight(itNRight);
    CoordsSetBottom(itNBottom); }
  /* -- Set all co-ordinates by another struct ----------------------------- */
  void CoordsSet(const Coords &cRef)
    { CoordsSet(cRef.CoordsGetLeft(), cRef.CoordsGetTop(),
                cRef.CoordsGetRight(), cRef.CoordsGetBottom()); }
  /* -- Init constructor --------------------------------------------------- */
  Coords(const IntType itNLeft, const IntType itNTop, const IntType itNRight,
    const IntType itNBottom) :         // X1, Y1, X2, Y2
    /* -- Initialisers ----------------------------------------------------- */
    ctXY1{ itNLeft, itNTop },          // X1 & Y1 co-ordinate
    ctXY2{ itNRight, itNBottom }       // X2 & Y2 co-ordinate
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- Default constructor ------------------------------------------------ */
  Coords() = default;
};/* ----------------------------------------------------------------------- */
typedef Coords<GLfloat> CoordsFloat;   // GLfloat co-ordinates
typedef Coords<GLuint>  CoordsUint;    // GLuint co-ordinates
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
