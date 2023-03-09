/* == LLMASK.HPP =========================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## Defines the 'Mask' namespace and methods for the guest to use in    ## **
** ## Lua. This file is invoked by 'lualib.hpp'.                          ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
#pragma once                           // Only one incursion allowed
/* ========================================================================= **
** ######################################################################### **
** ========================================================================= */
// % Mask
/* ------------------------------------------------------------------------- */
// ! The mask class is a pixel-perfect 1-bit bitmask processing API which
// ! allows the programmer to create destructable terrain and collision testing
// ! whether the test maybe between two sprites or actual terrain background.
// ! You can create bitmasks as big as the system will allow.
/* ========================================================================= */
namespace LLMask {                     // Mask namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IImage::P;             using namespace IMask::P;
using namespace Common;
/* ========================================================================= **
** ######################################################################### **
** ## Mask common helper classes                                          ## **
** ######################################################################### **
** -- Read Mask class argument --------------------------------------------- */
struct AgMask : public ArClass<Mask> {
  explicit AgMask(lua_State*const lS, const int iArg) :
    ArClass{*LuaUtilGetPtr<Mask>(lS, iArg, *cMasks)}{} };
/* -- Create Mask class argument ------------------------------------------- */
struct AcMask : public ArClass<Mask> {
  explicit AcMask(lua_State*const lS) :
    ArClass{*LuaUtilClassCreate<Mask>(lS, *cMasks)}{} };
/* -- Read mask tile position ---------------------------------------------- */
struct AgTileId : public AgSizeTLGE {
  explicit AgTileId(lua_State*const lS, const int iArg, const Mask &mCref) :
    AgSizeTLGE{ lS, iArg, 0, mCref.size() }{} };
/* ========================================================================= **
** ######################################################################### **
** ## Mask:* member functions                                             ## **
** ######################################################################### **
** ========================================================================= */
// $ Mask:Count
// < Count:integer=Number of sub-masks in the mask.
// ? Returns the number of sub-masks in the mask.
/* ------------------------------------------------------------------------- */
LLFUNC(Count, 1, LuaUtilPushVar(lS, AgMask{lS, 1}().size()))
/* ========================================================================= */
// $ Mask:Destroy
// ? destroys the mask and frees all the memory associated with it. The object
// ? will no longer be useable after this call and an error will be generated
// ? if accessed.
/* ------------------------------------------------------------------------- */
LLFUNC(Destroy, 0, LuaUtilClassDestroy<Mask>(lS, 1, *cMasks))
/* ========================================================================= */
// $ Mask:IsCollide
// > Dest:Mask=The destination mask to test upon
// > DestId:integer=The destination sub-mask id
// > DestX:integer=The X position to overlap the source mask
// > DestY:integer=The Y position to overlap the source mask
// < Result:boolean=True if the two bitmasks overlap. False if not
// ? Returns true if the second mask collides with the first mask at x0 y0.
/* ------------------------------------------------------------------------- */
LLFUNC(IsCollide, 1,
  const AgMask aSrcMask{lS, 1},
               aDestMask{lS, 2};
  const AgTileId aDestTileId{lS, 3, aDestMask};
  const AgInt aDestX{lS, 4},
              aDestY{lS, 5};
  LuaUtilPushVar(lS, aSrcMask().IsCollide(0, 0, 0, aDestMask, aDestTileId,
    aDestX, aDestY)))
/* ========================================================================= */
// $ Mask:IsCollideEx
// > SrcTileId:integer=The sub-mask id to use
// > SrcX:integer=The X position of the source mask
// > SrcY:integer=The Y position of the source mask
// > Dest:Mask=The destination mask to test upon
// > DestTileId:integer=The destination sub-mask id
// > DestX:integer=The X position to overlap the source mask
// > DestY:integer=The Y position to overlap the source mask
// < Result:boolean=True if the two bitmasks overlap. False if not
// ? Performs a bitmask comparison between two masks
/* ------------------------------------------------------------------------- */
LLFUNC(IsCollideEx, 1,
  const AgMask aSrcMask{lS, 1};
  const AgTileId aSrcTileId{lS, 2, aSrcMask};
  const AgInt aSrcX{lS, 3},
              aSrcY{lS, 4};
  const AgMask aDestMask{lS, 5};
  const AgTileId aDestTileId{lS, 6, aDestMask};
  const AgInt aDestX{lS, 7},
              aDestY{lS, 8};
  LuaUtilPushVar(lS, aSrcMask().IsCollide(aSrcTileId, aSrcX, aSrcY, aDestMask,
    aDestTileId, aDestX, aDestY)))
/* ========================================================================= */
// $ Mask:Clear
// > DestX:integer=Destination X position to start filling at.
// > DestY:integer=Destination Y position to start filling at.
// > DestWidth:integer=Horizontal pixel count to clear.
// > DestHeight:integer=Vertical pixel count to clear.
// ? Clears all the specified bits in the mask.
/* ------------------------------------------------------------------------- */
LLFUNC(Clear, 0,
  const AgMask aDestMask{lS, 1};
  const AgInt aDestX{lS, 2},
              aDestY{lS, 3},
              aDestWidth{lS, 4},
              aDestHeight{lS, 5};
  aDestMask().Clear(aDestX, aDestY, aDestWidth, aDestHeight))
/* ========================================================================= */
// $ Mask:Copy
// > SrcHandle:Mask=Source mask to merge from.
// > SrcTileId:integer=The id number of the source mask.
// > DestX:integer=Destination X position.
// > DestY:integer=Destination Y position.
// ? Copies the source mask id into destination mask id zero at the specified
// ? position.
/* ------------------------------------------------------------------------- */
LLFUNC(Copy, 0,
  const AgMask aDestMask{lS, 1},
               aSrcMask{lS, 2};
  const AgTileId aSrcTileId{lS, 3, aSrcMask};
  const AgInt aDestX{lS, 4},
              aDestY{lS, 5};
  aDestMask().Copy(0, aSrcMask, aSrcTileId, aDestX, aDestY))
/* ========================================================================= */
// $ Mask:Erase
// > DestTileId:integer=The id number of the destination mask.
// ? Fills the destination mask with zeros.
/* ------------------------------------------------------------------------- */
LLFUNC(Erase, 0,
  const AgMask aDestMask{lS, 1};
  const AgTileId aDestTileId{lS, 2, aDestMask};
  aDestMask().Erase(aDestTileId))
/* ========================================================================= */
// $ Mask:Fill
// > DestX:integer=Destination X position to start filling at.
// > DestY:integer=Destination Y position to start filling at.
// > DestWidth:integer=Horizontal pixel count to fill.
// > DestHeight:integer=Vertical pixel count to fill.
// ? Sets all the specified bits in the mask.
/* ------------------------------------------------------------------------- */
LLFUNC(Fill, 0,
  const AgMask aDestMask{lS, 1};
  const AgInt aDestX{lS, 2},
              aDestY{lS, 3},
              aDestWidth{lS, 4},
              aDestHeight{lS, 5};
  aDestMask().Fill(aDestX, aDestY, aDestWidth, aDestHeight))
/* ========================================================================= */
// $ Mask:Height
// < Height:integer=Height of each tile in sub-masks.
// ? Returns the height of each sub-mask in the mask.
/* ------------------------------------------------------------------------- */
LLFUNC(Height, 1, LuaUtilPushVar(lS, AgMask{lS, 1}().DimGetHeight()))
/* ========================================================================= */
// $ Mask:Id
// < Id:integer=The id number of the Mask object.
// ? Returns the unique id of the Mask object.
/* ------------------------------------------------------------------------- */
LLFUNC(Id, 1, LuaUtilPushVar(lS, AgMask{lS, 1}().CtrGet()))
/* ========================================================================= */
// $ Mask:Merge
// > SrcHandle:Mask=Source mask to merge from.
// > SrcTileId:integer=The id number of the source mask.
// > DestMaskX:integer=Destination X position.
// > DestMaskY:integer=Destination Y position.
// ? Merges the source mask id into destination mask id zero at the specified
// ? position.
/* ------------------------------------------------------------------------- */
LLFUNC(Merge, 0,
  const AgMask aDestMask{lS, 1},
               aSrcMask{lS, 2};
  const AgTileId aSrcTileId{lS, 3, aSrcMask};
  const AgInt aDestX{lS, 4},
              aDestY{lS, 5};
  aDestMask().Merge(aSrcMask, aSrcTileId, aDestX, aDestY))
/* ========================================================================= */
// $ Mask:Name
// < Name:string=Name of the mask.
// ? If this mask was loaded by a filename or it was set with a custom id.
// ? This function returns that name which was assigned to it.
/* ------------------------------------------------------------------------- */
LLFUNC(Name, 1, LuaUtilPushVar(lS, AgMask{lS, 1}().IdentGet()))
/* ========================================================================= */
// $ Mask:RayCast
// > SrcTileId:integer=The source mask id to test
// > SrcX:integer=The X position of the source mask
// > SrcY:integer=The Y position of the source mask
// > Dest:Mask=The destination mask handle to test upon
// > DestTileId:integer=The destination mask id to test upon
// > DestFX:integer=The starting X position of the destination mask
// > DestFY:integer=The starting Y position of the destination mask
// > DestTX:integer=The ending X position of the destination mask
// > DestTY:integer=The ending Y position of the destination mask
// < DoesCollide:boolean=True if the raycast collided with the other mask
// < CollideX:integer=If collision, the X co-ordinate of it
// < CollideY:integer=If collision, the Y co-ordinate of it
// ? Performs a raycast collision
/* ------------------------------------------------------------------------- */
LLFUNC(Raycast, 3,
  const AgMask aSrcMask{lS, 1};
  const AgTileId aSrcTileId{lS, 2, aSrcMask};
  const AgInt aSrcX{lS, 3},
              aSrcY{lS, 4};
  const AgMask aDestMask{lS, 5};
  const AgTileId aDestTileId{lS, 6, aDestMask};
  const AgInt aDestStartX{lS, 7},
              aDestStartY{lS, 8};
  int iX = LuaUtilGetInt<decltype(iX)>(lS, 9),
      iY = LuaUtilGetInt<decltype(iY)>(lS, 10);
  LuaUtilPushVar(lS, aSrcMask().Raycast(aSrcTileId, aSrcX, aSrcY, aDestMask,
    aDestTileId, aDestStartX, aDestStartY, iX, iY), iX, iY))
/* ========================================================================= */
// $ Mask:Save
// > Id:integer=The id number of the mask to dump.
// > Filename:string=The file to save to.
// ? Dumps the specified mask to disk for debugging purposes.
/* ------------------------------------------------------------------------- */
LLFUNC(Save, 0,
  const AgMask aMask{lS, 1};
  const AgTileId aTileId{lS, 2, aMask};
  const AgFilename aFilename{lS, 3};
  aMask().Dump(aTileId, aFilename))
/* ========================================================================= */
// $ Mask:Width
// < Width:integer=Width of each tile in sub-masks.
// ? Returns the width of each sub-mask in the mask.
/* ------------------------------------------------------------------------- */
LLFUNC(Width, 1, LuaUtilPushVar(lS, AgMask{lS, 1}().DimGetWidth()))
/* ========================================================================= **
** ######################################################################### **
** ## Mask:* member functions structure                                   ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
LLRSMFBEGIN                            // Mask:* member functions begin
  LLRSFUNC(Clear),       LLRSFUNC(Copy),  LLRSFUNC(Count),
  LLRSFUNC(Destroy),     LLRSFUNC(Erase), LLRSFUNC(Fill),
  LLRSFUNC(Height),      LLRSFUNC(Id),    LLRSFUNC(IsCollide),
  LLRSFUNC(IsCollideEx), LLRSFUNC(Merge), LLRSFUNC(Name),
  LLRSFUNC(Raycast),     LLRSFUNC(Save),  LLRSFUNC(Width),
LLRSEND                                // Mask:* member functions end
/* ========================================================================= */
// $ Mask.Create
// > Image:image=The monochrome image to open.
// > Count:integer=The number of tiles to grab from image.
// > Width:integer=The width of each tile.
// > Height:integer=The height of each tile.
// < Handle:Mask=The handle of the new mask created.
// ? Loads pre-defined masks from the specified raster image file.
/* ------------------------------------------------------------------------- */
LLFUNC(Create, 1,
  const AgImage aImage{lS, 1};
  const AgUInt aWidth{lS, 2},
               aHeight{lS, 3};
  AcMask{lS}().InitFromImage(aImage, aWidth, aHeight))
/* ========================================================================= */
// $ Mask.CreateOne
// > Identifier:string=The identifier of the mask.
// > Width:integer=The width of the new mask.
// > Height:integer=The height of the new mask.
// < Handle:Mask=The handle of the new mask created.
// ? Creates a new collision mask of the specified dimensions with all the bits
// ? set to ONE.
/* ------------------------------------------------------------------------- */
LLFUNC(CreateOne, 1,
  const AgNeString aIdentifier{lS, 1};
  const AgUInt aWidth{lS, 2},
               aHeight{lS, 3};
  AcMask{lS}().InitOne(aIdentifier, aWidth, aHeight))
/* ========================================================================= */
// $ Mask.CreateZero
// > Identifier:string=The identifier of the mask.
// > Width:integer=The width of the new mask.
// > Height:integer=The height of the new mask.
// < Handle:Mask=The handle of the new mask created.
// ? Creates a new collision mask of the specified dimensions with all the bits
// ? set to ZERO.
/* ------------------------------------------------------------------------- */
LLFUNC(CreateZero, 1,
  const AgNeString aIdentifier{lS, 1};
  const AgUInt aWidth{lS, 2},
               aHeight{lS, 3};
  AcMask{lS}().InitZero(aIdentifier, aWidth, aHeight))
/* ========================================================================= **
** ######################################################################### **
** ## Mask.* namespace functions structure                                ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
LLRSBEGIN                              // Mask.* namespace functions begin
  LLRSFUNC(Create), LLRSFUNC(CreateOne), LLRSFUNC(CreateZero),
LLRSEND                                // Mask.* namespace functions end
/* ========================================================================= */
}                                      // End of Mask namespace
/* == EoF =========================================================== EoF == */
