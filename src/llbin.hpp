/* == LLBIN.HPP ============================================================ **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## Defines the 'Bin' namespace and methods for the guest to use in     ## **
** ## Lua. This file is invoked by 'lualib.hpp'.                          ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
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
    ArClass{*LuaUtilGetPtr<Bin>(lS, iArg, *cBins)}{} };
/* -- Create Bin class argument -------------------------------------------- */
struct AcBin : public ArClass<Bin> {
  explicit AcBin(lua_State*const lS) :
    ArClass{*LuaUtilClassCreate<Bin>(lS, *cBins)}{} };
/* -- Read dimensions arguments -------------------------------------------- */
struct AgDimension : public AgUIntLGE {
  explicit AgDimension(lua_State*const lS, const int iArg) :
    AgUIntLGE{lS, iArg, 1, numeric_limits<int>::max()}{} };
/* ========================================================================= **
** ######################################################################### **
** ## Bin:* member functions                                              ## **
** ######################################################################### **
** ========================================================================= */
// $ Bin:Destroy
// ? Destroys the bin and frees all the memory associated with it. The object
// ? will no longer be useable after this call and an error will be generated
// ? if accessed.
/* ------------------------------------------------------------------------- */
LLFUNC(Destroy, 0, LuaUtilClassDestroy<Bin>(lS, 1, *cBins))
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
// $ Bin:Height
// < Height:integer=The height of the bin in units
// ? Returns the height of the bin in units.
/* ------------------------------------------------------------------------- */
LLFUNC(Height, 1, LuaUtilPushVar(lS, AgBin{lS, 1}().DimGetHeight()))
/* ========================================================================= */
// $ Bin:Id
// < Id:integer=The id number of the Bin object.
// ? Returns the unique id of the Bin object.
/* ------------------------------------------------------------------------- */
LLFUNC(Id, 1, LuaUtilPushVar(lS, AgBin{lS, 1}().CtrGet()))
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
// $ Bin:Occupancy
// < Height:integer=The height of the bin in units
// ? Returns the height of the bin in units.
/* ------------------------------------------------------------------------- */
LLFUNC(Occupancy, 1, LuaUtilPushVar(lS, AgBin{lS, 1}().Occupancy()))
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
/* ========================================================================= */
// $ Bin:Width
// < Width:integer=The width of the bin in units
// ? Returns the width of the bin in units.
/* ------------------------------------------------------------------------- */
LLFUNC(Width, 1, LuaUtilPushVar(lS, AgBin{lS, 1}().DimGetWidth()))
/* ========================================================================= **
** ######################################################################### **
** ## Bin:* member functions structure                                    ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
LLRSMFBEGIN                            // Bin:* member functions begin
  LLRSFUNC(Destroy), LLRSFUNC(Id),        LLRSFUNC(Insert), LLRSFUNC(Enlarge),
  LLRSFUNC(Height),  LLRSFUNC(Occupancy), LLRSFUNC(Test),   LLRSFUNC(Width),
LLRSEND                                // Bin:* member functions end
/* ========================================================================= */
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
/* ========================================================================= **
** ######################################################################### **
** ## Bin:* namespace functions structure                                 ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
LLRSBEGIN                              // Bin.* namespace functions begin
  LLRSFUNC(Create),
LLRSEND                                // Bin.* namespace functions end
/* ========================================================================= */
}                                      // End of Bin namespace
/* == EoF =========================================================== EoF == */
