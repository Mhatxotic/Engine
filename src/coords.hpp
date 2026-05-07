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
/* ------------------------------------------------------------------------- */
template<typename IntType>             // Custom type
  requires StdIsArithmatic<IntType>    // Must compute
class Coords                           // Co-ordinates class
{ /* -- Private variables -------------------------------------------------- */
  using CoordType = Coord<IntType>;    // Co-ordinates container type
  CoordType        ctXY1, ctXY2;       // Top-left/right-bottom co-ordinates
  /* -- Get co-ordinate -------------------------------------------- */ public:
  template<typename RIntType = IntType>RIntType CoordsGetX1() const
    { return ctXY1.template CoordGetX<RIntType>(); }
  template<typename RIntType = IntType>RIntType CoordsGetY1() const
    { return ctXY1.template CoordGetY<RIntType>(); }
  template<typename RIntType = IntType>RIntType CoordsGetX2() const
    { return ctXY2.template CoordGetX<RIntType>(); }
  template<typename RIntType = IntType>RIntType CoordsGetY2() const
    { return ctXY2.template CoordGetY<RIntType>(); }
  /* -- Set co-ordinate by integral values --------------------------------- */
  void CoordsSetX1(const IntType itNX1) { ctXY1.CoordSetX(itNX1); }
  void CoordsSetY1(const IntType itNY1) { ctXY1.CoordSetY(itNY1); }
  void CoordsSetX2(const IntType itNX2) { ctXY2.CoordSetX(itNX2); }
  void CoordsSetY2(const IntType itNY2) { ctXY2.CoordSetY(itNY2); }
  /* -- Set co-ordinate by another struct ---------------------------------- */
  void CoordsSetX1(const Coords &cRef) { CoordsSetX1(cRef.CoordsGetX1()); }
  void CoordsSetY1(const Coords &cRef) { CoordsSetY1(cRef.CoordsGetY1()); }
  void CoordsSetX2(const Coords &cRef) { CoordsSetX2(cRef.CoordsGetX2()); }
  void CoordsSetY2(const Coords &cRef) { CoordsSetY2(cRef.CoordsGetY2()); }
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
  void CoordsSet(const IntType itNX1, const IntType itNY1,
    const IntType itNX2, const IntType itNY2)
  { CoordsSetX1(itNX1); CoordsSetY1(itNY1);
    CoordsSetX2(itNX2); CoordsSetY2(itNY2); }
  /* -- Set all co-ordinates by another struct ----------------------------- */
  void CoordsSet(const Coords &cRef)
    { CoordsSet(cRef.CoordsGetX1(), cRef.CoordsGetY1(),
                cRef.CoordsGetX2(), cRef.CoordsGetY2()); }
  /* -- Init constructor --------------------------------------------------- */
  Coords(const IntType itNX1,          // Left
         const IntType itNY1,          // Top
         const IntType itNX2,          // Right
         const IntType itNY2) :        // Bottom
    /* -- Initialisers ----------------------------------------------------- */
    ctXY1{ itNX1, itNY1 },             // X1 & Y1 co-ordinate
    ctXY2{ itNX2, itNY2 }              // X2 & Y2 co-ordinate
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- Default constructor ------------------------------------------------ */
  Coords() = default;
};/* ----------------------------------------------------------------------- */
using CoordsGLFloat = Coords<GLfloat>; // GLfloat co-ordinates
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
