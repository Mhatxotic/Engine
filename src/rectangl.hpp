/* == RECTANGL.HPP ========================================================= **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## Sets up a rectangle class based on the co-ordinate struct.          ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IRectangle {                 // Start of module namespace
/* ------------------------------------------------------------------------- */
using namespace ICoord::P;             using namespace Lib::OS::GlFW::Types;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* ------------------------------------------------------------------------- */
template<typename Int,                 // The integer type to use
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
  Rectangle(void) = default;
};/* ----------------------------------------------------------------------- */
typedef Rectangle<GLfloat> RectFloat;  // Rectangle of GLfloats
typedef Rectangle<GLuint>  RectUint;   // Reload glyph size
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of module namespace
/* == EoF =========================================================== EoF == */
