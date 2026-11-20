/* == DIMCOORD.HPP ========================================================= **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## Sets up a class containing a coord and a dimension class.           ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IDimCoord {                  // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace ICoord::P;             using namespace IDim::P;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* ------------------------------------------------------------------------- */
template<typename IntType>             // Dimensions integer type
  requires StdIsArithmatic<IntType>    // Dimension type must be int/float
struct DimCoords :                     // Members initially public
  /* -- Base classes ------------------------------------------------------- */
  public Coord<IntType>,               // Co-ordinates class
  public Dimensions<IntType>           // Dimensions class
{ /* -- Public typedefs ---------------------------------------------------- */
  using CClass = Coord<IntType>;       // Shortcut to coord class
  using DClass = Dimensions<IntType>;  // Shortcut to dimensions class
  /* -- Set co-ordinates and dimensions ------------------------------------ */
  void DimCoSet(const IntType itX, const IntType itY,
                const IntType itW, const IntType itH)
    { this->CoordSet(itX, itY); this->DimSet(itW, itH); }
  /* -- Constructor to fill all parameters --------------------------------- */
  DimCoords(const CClass &cOther, const DClass &dOther) :
    /* -- Initialisers ----------------------------------------------------- */
    CClass{ cOther },                  // Initialise co-ordinates
    DClass{ dOther }                   // Initialise dimensions
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- Constructor to fill all parameters --------------------------------- */
  DimCoords(const IntType itX, const IntType itY,
            const IntType itW, const IntType itH) :
    /* -- Initialisers ----------------------------------------------------- */
    CClass{ itX, itY },                // Initialise co-ordinates
    DClass{ itW, itH }                 // Initialise dimensions
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- Default constructor that zero initialises everything --------------- */
  DimCoords() = default;
};/* ----------------------------------------------------------------------- */
using DimCoInt   = DimCoords<int>;     // Dimension int coords typedef
using DimCoSizeT = DimCoords<size_t>;  // Dimension size_t coords typedef
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
