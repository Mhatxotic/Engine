/* == LLBIN.HPP ============================================================ **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## Defines the 'Bin' namespace and methods for the guest to use in     ## **
** ## Lua. This file is invoked by 'lualib.hpp'.                          ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ========================================================================= **
** ######################################################################### **
** ========================================================================= */
// % Bin
/* ------------------------------------------------------------------------- */
// ! This gives the guest to have a virtual bin of the specified size and
// ! rectangles of a given size are placed inside until filled up.
/* ========================================================================= */
namespace LLBin {                      // Bin namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IBin::P;               using namespace Common;
/* ========================================================================= **
** ######################################################################### **
** ## Bin common helper classes                                           ## **
** ######################################################################### **
** -- Read Bin class argument ---------------------------------------------- */
struct AgBin : public ArClass<Bin> {
  explicit AgBin(lua_State*const lS, const int iArg) :
    ArClass{LuaUtilGetClassRef<Bin>(lS, iArg, cBins)}{} };
/* -- Create Bin class argument -------------------------------------------- */
struct AcBin : public ArClass<Bin> {
  explicit AcBin(lua_State*const lS) :
    ArClass{LuaUtilClassCreateRef<Bin>(lS, cBins)}{} };
/* -- Read dimensions arguments -------------------------------------------- */
struct AgDimension : public AgUIntLGE {
  explicit AgDimension(lua_State*const lS, const int iArg) :
    AgUIntLGE{lS, iArg, 1, StdLimits<int>::max()}{} };
/* ========================================================================= **
** ######################################################################### **
** ## Bin:* methods                                                       ## **
** ######################################################################### **
** ========================================================================= */
// $ Bin:Destroy
// ? Destroys the bin and frees all the memory associated with it. The object
// ? will no longer be useable after this call and an error will be generated
// ? if accessed.
/* ------------------------------------------------------------------------- */
LLFUNC(Destroy, 0, LuaUtilClassDestroy<Bin>(lS, cBins))
/* ========================================================================= */
// $ Bin:Enlarge
// > Width:integer=The new width of the bin in units
// > Height:integer=The new height of the bin in units
// < Success:boolean=The enlargement was a success
// ? Makes the specified bin bigger for you to put more rects in. Note this is
// ? a dirty operation. None of the original bounds are modified, only new
// ? new empty rects are added, i.e. sub-bins to explain it simply.
/* ------------------------------------------------------------------------- */
LLFUNC(Enlarge, 1,
  const AgBin aBin{lS, 1};
  const AgDimension aWidth{lS, 2},
                    aHeight{lS, 3};
  LuaUtilPushVar(lS, aBin().Resize(aWidth, aHeight)))
/* ========================================================================= */
// $ Bin:GetHeight
// < Height:integer=The height of the bin in units
// ? Returns the height of the bin in units.
/* ------------------------------------------------------------------------- */
LLFUNC(GetHeight, 1, LuaUtilPushVar(lS, AgBin{lS, 1}().DimGetHeight()))
/* ========================================================================= */
// $ Bin:GetId
// < Id:integer=The id number of the Bin object.
// ? Returns the unique id of the Bin object.
/* ------------------------------------------------------------------------- */
LLFUNC(GetId, 1, LuaUtilPushVar(lS, AgBin{lS, 1}().Serial()))
/* ========================================================================= */
// $ Bin:GetOccupancy
// < Height:integer=The height of the bin in units
// ? Returns the height of the bin in units.
/* ------------------------------------------------------------------------- */
LLFUNC(GetOccupancy, 1, LuaUtilPushVar(lS, AgBin{lS, 1}().Occupancy()))
/* ========================================================================= */
// $ Bin:GetWidth
// < Width:integer=The width of the bin in units
// ? Returns the width of the bin in units.
/* ------------------------------------------------------------------------- */
LLFUNC(GetWidth, 1, LuaUtilPushVar(lS, AgBin{lS, 1}().DimGetWidth()))
/* ========================================================================= */
// $ Bin:Insert
// > Width:integer=The width of the item to insert in units
// > Height:integer=The height of the item to insert in units
// < X1:integer=The X1 position of the item inserted
// < Y1:integer=The Y1 position of the item inserted
// < X2:integer=The X2 position of the item inserted
// < Y2:integer=The Y2 position of the item inserted
// ? Inserts the specified rectangle in the bin. All values returned are zero
// ? on failure.
/* ------------------------------------------------------------------------- */
LLFUNC(Insert, 4,
  const AgBin aBin{lS, 1};
  const AgDimension aWidth{lS, 2},
                    aHeight{lS, 3};
  const Pack<int>::Rect &rAllotted = aBin().Insert(aWidth, aHeight);
  LuaUtilPushVar(lS, rAllotted.CoordGetX(), rAllotted.CoordGetY(),
    rAllotted.DimGetWidth(), rAllotted.DimGetHeight()))
/* ========================================================================= */
// $ Bin:IsDestroyed
// < Destroyed:boolean=If the Bin class is destroyed
// ? Returns if the Bin class is destroyed.
/* ------------------------------------------------------------------------- */
LLFUNC(IsDestroyed, 1, LuaUtilPushVar(lS, LuaUtilIsClassDestroyed(lS, cBins)))
/* ========================================================================= */
// $ Bin:Test
// > Width:integer=The width of the item to insert in units
// > Height:integer=The height of the item to insert in units
// < X1:integer=The X1 position of the item inserted
// < Y1:integer=The Y1 position of the item inserted
// < X2:integer=The X2 position of the item inserted
// < Y2:integer=The Y2 position of the item inserted
// ? Test inserts the specified rectangle in the bin. All values returned are
// ? zero on failure.
/* ------------------------------------------------------------------------- */
LLFUNC(Test, 4,
  const AgBin aBin{lS, 1};
  const AgDimension aWidth{lS, 2},
                    aHeight{lS, 3};
  const Pack<int>::Rect &rAllotted = aBin().Test(aWidth, aHeight);
  LuaUtilPushVar(lS, rAllotted.CoordGetX(), rAllotted.CoordGetY(),
    rAllotted.DimGetWidth(), rAllotted.DimGetHeight()))
/* ========================================================================= **
** ######################################################################### **
** ## Bin:* methods structure                                             ## **
** ######################################################################### **
** ========================================================================= */
LLRSMFBEGIN                            // Bin:* methods begin
  LLRSFUNC(Destroy),   LLRSFUNC(Enlarge),      LLRSFUNC(GetId),
  LLRSFUNC(GetHeight), LLRSFUNC(GetOccupancy), LLRSFUNC(GetWidth),
  LLRSFUNC(Insert),    LLRSFUNC(IsDestroyed),  LLRSFUNC(Test),
LLRSEND                                // Bin:* methods end
/* ========================================================================= **
** ######################################################################### **
** ## Bin.* non-member functions                                          ## **
** ######################################################################### **
** ========================================================================= */
// $ Bin.Create
// > Width:integer=The width of the bin in units
// > Height:integer=The height of the bin in units
// < Handle:Bin=Handle to the Bin class
// ? Creates an empty bin of the specified size.
/* ------------------------------------------------------------------------- */
LLFUNC(Create, 1,
  const AgDimension aWidth{lS, 1},
                    aHeight{lS, 2};
  AcBin{lS}().Init(aWidth, aHeight))
/* ========================================================================= */
// $ Bin.GetClasses
// < Count:integer=Total number of bins created.
// ? Returns the total number of bin classes currently active.
/* ------------------------------------------------------------------------- */
LLFUNC(GetClasses, 1, LuaUtilPushVar(lS, cBins->CollectorCount()))
/* ========================================================================= **
** ######################################################################### **
** ## Bin.* non-member functions structure                                ## **
** ######################################################################### **
** ========================================================================= */
LLRSBEGIN                              // Bin.* non-member functions begin
  LLRSFUNC(Create), LLRSFUNC(GetClasses),
LLRSEND                                // Bin.* non-member functions end
/* ========================================================================= */
}                                      // End of Bin namespace
/* == EoF =========================================================== EoF == */
