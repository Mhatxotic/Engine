/* == DIMCOORD.HPP ========================================================= **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## Sets up a class containing a coord and a dimension class.           ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IDimCoord {                  // Start of module namespace
/* ------------------------------------------------------------------------- */
using namespace ICoord::P;             using namespace IDim::P;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* ------------------------------------------------------------------------- */
template<typename DInt = unsigned int, // Dimensions integer type
         typename CInt = DInt,         // Coords class integer type
         class DClass  = Dimensions<DInt>,  // Dimensions class to use
         class CClass  = Coordinates<CInt>> // Coords class to use
struct DimCoords :                     // Members initially public
  /* -- Base classes ------------------------------------------------------- */
  public CClass,                       // Co-ordinates class
  public DClass                        // Dimensions class
{ /* -- Set co-ordinates and dimensions ------------------------------------ */
  void DimCoSet(const CInt ciX, const CInt ciY, const DInt diW, const DInt diH)
    { this->CoordSet(ciX, ciY); this->DimSet(diW, diH); }
  /* -- Constructor to fill all parameters --------------------------------- */
  DimCoords(const CClass &ciOther,     // Specified co-ordinates
            const DClass &diOther) :   // Specified dimensions
    /* -- Initialisers ----------------------------------------------------- */
    CClass{ ciOther },                 // Initialise co-ordinates
    DClass{ diOther }                  // Initialise dimensions
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Constructor to fill all parameters --------------------------------- */
  DimCoords(const CInt ciX,            // Specified X co-ordinate
            const CInt ciY,            // Specified Y co-ordinate
            const DInt diW,            // Specified width dimension
            const DInt diH) :          // Specified height dimension
    /* -- Initialisers ----------------------------------------------------- */
    CClass{ ciX, ciY },                // Initialise co-ordinates
    DClass{ diW, diH }                 // Initialise dimensions
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Default constructor that zero initialises everything --------------- */
  DimCoords(void) = default;
};/* ----------------------------------------------------------------------- */
typedef DimCoords<int> DimCoInt;       // Dimension Cordinates typedef
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of module namespace
/* == EoF =========================================================== EoF == */
