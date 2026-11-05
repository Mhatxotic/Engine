/* == IMGSLOT.HPP ========================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## Defines a class that holds image pixel data and dimensions plus a   ## **
** ## list of these classes that are contained in the 'Image' class.      ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IImageSlot {                 // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IDim::P;               using namespace IMemory::P;
using namespace IStd::P;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* ------------------------------------------------------------------------- */
struct ImageSlot :                     // Members initially public
  /* -- Initialisers ------------------------------------------------------- */
  public Memory,                       // Memory data
  public DimUInt                       // Bitmap dimensions
{ /* -- Copy constructor --------------------------------------------------- */
  ImageSlot(const Memory &mData, const unsigned uW, const unsigned uH) :
    /* -- Initialisers ----------------------------------------------------- */
    Memory{ mData },                   // Copy memory in place
    DimUInt{ uW, uH }                  // Set dimensions
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- Move constructor --------------------------------------------------- */
  ImageSlot(Memory &&mData, const unsigned uW, const unsigned uH) :
    /* -- Initialisers ----------------------------------------------------- */
    Memory{ StdMove(mData) },          // Move memory in place
    DimUInt{ uW, uH }                  // Set dimensions
    /* -- No code ---------------------------------------------------------- */
    {}
};/* ----------------------------------------------------------------------- */
using SlotList = StdVector<ImageSlot>; // list of bitmaps
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
