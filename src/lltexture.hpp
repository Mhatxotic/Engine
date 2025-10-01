/* == LLTEXTURE.HPP ======================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## Defines the 'Texture' namespace and methods for the guest to use in ## **
** ## Lua. This file is invoked by 'lualib.hpp'.                          ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ========================================================================= **
** ######################################################################### **
** ========================================================================= */
// % Texture
/* ------------------------------------------------------------------------- */
// ! The texture class allows the programmer to draw textures on the screen
// ! which also includes font management.
/* ========================================================================= */
namespace LLTexture {                  // Texture namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IConGraph::P;          using namespace IConsole::P;
using namespace IFbo::P;               using namespace IImage::P;
using namespace IOgl::P;               using namespace ITexture::P;
using namespace Common;
/* ========================================================================= **
** ######################################################################### **
** ## Texture common helper classes                                       ## **
** ######################################################################### **
** -- Create Texture class argument ---------------------------------------- */
struct AcTexture : public ArClass<Texture> {
  explicit AcTexture(lua_State*const lS) :
    ArClass{*LuaUtilClassCreate<Texture>(lS, *cTextures)}{} };
/* -- Read a Tile id ------------------------------------------------------- */
struct AgTileId : public AgIntegerLGE<size_t> {
  explicit AgTileId(lua_State*const lS, const int iArg, const Texture &tCref) :
    AgIntegerLGE{lS, iArg, 0, tCref.GetTileCount()}{} };
/* -- Other types ---------------------------------------------------------- */
typedef AgInteger<GLint> AgGLint;
/* -- Read vector of the specified integer type ---------------------------- */
template<class VectorValueType, class VectorType = vector<VectorValueType>>
  struct AgIntegerVector : public VectorType
{ const VectorType &operator()(void) const { return *this; }
  operator const VectorType&(void) const { return operator()(); }
  explicit AgIntegerVector(lua_State*const lS, const int iArg) :
    VectorType{ LuaUtilToIntVector<VectorType>(lS, iArg)}{} };
/* ========================================================================= **
** ######################################################################### **
** ## Texture:* member functions                                          ## **
** ######################################################################### **
** ========================================================================= */
// $ Texture:Blit
// ? Blits texture 0 with the stored preset texcoord (SetTC*), vertex (SetV*)
// ? and colour (SetC*). Using the Blit* functions will overwrite this stored
// ? preset data.
/* ------------------------------------------------------------------------- */
LLFUNC(Blit, 0, AgTexture{lS, 1}().Blit(0))
/* ========================================================================= */
// $ Texture:BlitILT
// > TexIndex:integer=The texture index to use.
// > Left:number=The starting column to draw from.
// > Top:number=The starting row to draw from.
// ? Blits tile 'TileIndex' of texture 0 at the specified screen co-ordinates
// ? with the internal tile width and height calculated when created.
/* ------------------------------------------------------------------------- */
LLFUNC(BlitILT, 0,
  const AgTexture aTexture{lS, 1};
  const AgTextureId aTextureId{lS, 2, aTexture};
  const AgGLfloat aLeft{lS, 3},
                  aTop{lS, 4};
  aTexture().BlitLT(aTextureId, 0, aLeft, aTop))
/* ========================================================================= */
// $ Texture:BlitILTA
// > TexIndex:integer=The texture index to use.
// > Left:number=The starting column to draw from.
// > Top:number=The starting row to draw from.
// > Angle:number=The angle in which to rotate from the centre point (-1 to 1).
// ? Blits tile 'TileIndex' of texture 0 at the specified screen co-ordinates
// ? with the internal tile width and height calculated when created and with
// ? the specified angle.
/* ------------------------------------------------------------------------- */
LLFUNC(BlitILTA, 0,
  const AgTexture aTexture{lS, 1};
  const AgTextureId aTextureId{lS, 2, aTexture};
  const AgGLfloat aLeft{lS, 3},
                  aTop{lS, 4};
  const AgAngle aAngle{lS, 5};
  aTexture().BlitLTA(aTextureId, 0, aLeft, aTop, aAngle))
/* ========================================================================= */
// $ Texture:BlitILTRB
// > TexIndex:integer=The texture index to use.
// > Left:number=The starting column to draw from.
// > Top:number=The starting row to draw from.
// > Right:number=The ending column pixel to draw to.
// > Bottom:number=The ending row pixel to draw to.
// ? Blits tile 0 of texture 'TexIndex' at the specified bounds.
/* ------------------------------------------------------------------------- */
LLFUNC(BlitILTRB, 0,
  const AgTexture aTexture{lS, 1};
  const AgTextureId aTextureId{lS, 2, aTexture};
  const AgGLfloat aLeft{lS, 3},
                  aTop{lS, 4},
                  aRight{lS, 5},
                  aBottom{lS, 6};
  aTexture().BlitLTRB(aTextureId, 0, aLeft, aTop, aRight, aBottom))
/* ========================================================================= */
// $ Texture:BlitILTWH
// > TexIndex:integer=The texture index to use.
// > Left:number=The starting column to draw from.
// > Top:number=The starting row to draw from.
// > Width:number=The width of the quad to draw.
// > Height:number=The height of the quad to draw
// ? Blits tile 0 of texture 'TexIndex' at the specified screen co-ordinates
// ? and with the specified dimensions.
/* ------------------------------------------------------------------------- */
LLFUNC(BlitILTWH, 0,
  const AgTexture aTexture{lS, 1};
  const AgTextureId aTextureId{lS, 2, aTexture};
  const AgGLfloat aLeft{lS, 3},
                  aTop{lS, 4},
                  aWidth{lS, 5},
                  aHeight{lS, 6};
  aTexture().BlitLTWH(aTextureId, 0, aLeft, aTop, aWidth, aHeight))
/* ========================================================================= */
// $ Texture:BlitILTWHA
// > TexIndex:integer=The texture index to use.
// > Left:number=The starting column to draw from.
// > Top:number=The starting row to draw from.
// > Width:number=The width of the quad to draw.
// > Height:number=The height of the quad to draw
// > Angle:number=The angle in which to rotate from the centre point (-1 to 1).
// ? Blits tile 0 of texture 'TexIndex' at the specified screen co-ordinates
// ? and with the specified dimensions at the specified angle.
/* ------------------------------------------------------------------------- */
LLFUNC(BlitILTWHA, 0,
  const AgTexture aTexture{lS, 1};
  const AgTextureId aTextureId{lS, 2, aTexture};
  const AgGLfloat aLeft{lS, 3},
                  aTop{lS, 4},
                  aWidth{lS, 5},
                  aHeight{lS, 6};
  const AgAngle aAngle{lS, 7};
  aTexture().BlitLTWHA(aTextureId, 0, aLeft, aTop, aWidth, aHeight, aAngle))
/* ========================================================================= */
// $ Texture:BlitISLT
// > TexIndex:integer=The texture index to use.
// > TileIndex:integer=The tile index of the texture to blit.
// > Left:number=The starting column to draw from.
// > Top:number=The starting row to draw from.
// ? Blits tile 'TileIndex' of texture 'TexIndex' at the specified screen
// ? co-ordinates with the internal tile width and height calculated when
// ? created.
/* ------------------------------------------------------------------------- */
LLFUNC(BlitISLT, 0,
  const AgTexture aTexture{lS, 1};
  const AgTextureId aTextureId{lS, 2, aTexture};
  const AgTileId aTileId{lS, 3, aTexture};
  const AgGLfloat aLeft{lS, 4},
                  aTop{lS, 5};
  aTexture().BlitLT(aTextureId, aTileId, aLeft, aTop))
/* ========================================================================= */
// $ Texture:BlitISLTA
// > TexIndex:integer=The texture index to use.
// > TileIndex:integer=The tile index of the texture to blit.
// > Left:number=The starting column to draw from.
// > Top:number=The starting row to draw from.
// > Angle:number=The angle in which to rotate from the centre point (-1 to 1).
// ? Blits tile 'TileIndex' of texture 'TexIndex' at the specified screen
// ? co-ordinates with the internal tile width and height calculated when
// ? created, and with the specified angle.
/* ------------------------------------------------------------------------- */
LLFUNC(BlitISLTA, 0,
  const AgTexture aTexture{lS, 1};
  const AgTextureId aTextureId{lS, 2, aTexture};
  const AgTileId aTileId{lS, 3, aTexture};
  const AgGLfloat aLeft{lS, 4},
                  aTop{lS, 5};
  const AgAngle aAngle{lS, 6};
  aTexture().BlitLTA(aTextureId, aTileId, aLeft, aTop, aAngle))
/* ========================================================================= */
// $ Texture:BlitISLTRB
// > TexIndex:integer=The texture index to use.
// > TileIndex:integer=The tile index to blit.
// > Left:number=The starting column to draw from.
// > Top:number=The starting row to draw from.
// > Right:number=The ending column pixel to draw to.
// > Bottom:number=The ending row pixel to draw to.
// ? Blits tile 'TileIndex' of texture 'TexIndex' at the specified screen
// ? bounds.
/* ------------------------------------------------------------------------- */
LLFUNC(BlitISLTRB, 0,
  const AgTexture aTexture{lS, 1};
  const AgTextureId aTextureId{lS, 2, aTexture};
  const AgTileId aTileId{lS, 3, aTexture};
  const AgGLfloat aLeft{lS, 4},
                  aTop{lS, 5},
                  aRight{lS, 6},
                  aBottom{lS, 7};
  aTexture().BlitLTRB(aTextureId, aTileId, aLeft, aTop, aRight, aBottom))
/* ========================================================================= */
// $ Texture:BlitISLTWH
// > TexIndex:integer=The texture index to use.
// > TileIndex:integer=The tile index to blit.
// > Left:number=The starting column to draw from.
// > Top:number=The starting row to draw from.
// > Width:number=The width of the quad to draw.
// > Height:number=The height of the quad to draw
// ? Blits tile 'TileIndex' of texture 'TexIndex' at the specified screen
// ? co-ordinates and with the specified dimensions.
/* ------------------------------------------------------------------------- */
LLFUNC(BlitISLTWH, 0,
  const AgTexture aTexture{lS, 1};
  const AgTextureId aTextureId{lS, 2, aTexture};
  const AgTileId aTileId{lS, 3, aTexture};
  const AgGLfloat aLeft{lS, 4},
                  aTop{lS, 5},
                  aWidth{lS, 6},
                  aHeight{lS, 7};
  aTexture().BlitLTWH(aTextureId, aTileId, aLeft, aTop, aWidth, aHeight))
/* ========================================================================= */
// $ Texture:BlitISLTWHA
// > TexIndex:integer=The texture index to use.
// > TileIndex:integer=The tile index to blit.
// > Left:number=The starting column to draw from.
// > Top:number=The starting row to draw from.
// > Width:number=The width of the quad to draw.
// > Height:number=The height of the quad to draw
// > Angle:number=The angle in which to rotate from the centre point (-1 to 1).
// ? Blits tile 'TileIndex' of texture 'TexIndex' at the specified screen
// ? co-ordinates and with the specified dimensions.
/* ------------------------------------------------------------------------- */
LLFUNC(BlitISLTWHA, 0,
  const AgTexture aTexture{lS, 1};
  const AgTextureId aTextureId{lS, 2, aTexture};
  const AgTileId aTileId{lS, 3, aTexture};
  const AgGLfloat aLeft{lS, 4},
                  aTop{lS, 5},
                  aWidth{lS, 6},
                  aHeight{lS, 7};
  const AgAngle aAngle{lS, 8};
  aTexture().BlitLTWHA(aTextureId, aTileId, aLeft, aTop, aWidth, aHeight,
    aAngle))
/* ========================================================================= */
// $ Texture:BlitLT
// > Left:number=The starting column to draw from.
// > Top:number=The starting row to draw from.
// ? Blits tile 0 of texture 0 at the specified screen co-ordinates with the
// ? internal tile width and height calculated when created.
/* ------------------------------------------------------------------------- */
LLFUNC(BlitLT, 0,
  const AgTexture aTexture{lS, 1};
  const AgGLfloat aLeft{lS, 2},
                  aTop{lS, 3};
  aTexture().BlitLT(0, 0, aLeft, aTop))
/* ========================================================================= */
// $ Texture:BlitLTA
// > Left:number=The starting column to draw from.
// > Top:number=The starting row to draw from.
// > Angle:number=The angle in which to rotate from the centre point (-1 to 1).
// ? Blits tile 0 of texture 0 at the specified screen co-ordinates with the
// ? internal tile width and height calculated when created and with the
// ? specified angle.
/* ------------------------------------------------------------------------- */
LLFUNC(BlitLTA, 0,
  const AgTexture aTexture{lS, 1};
  const AgGLfloat aLeft{lS, 2},
                  aTop{lS, 3};
  const AgAngle aAngle{lS, 4};
  aTexture().BlitLTA(0, 0, aLeft, aTop, aAngle))
/* ========================================================================= */
// $ Texture:BlitLTRB
// > Left:number=The starting column to draw from.
// > Top:number=The starting row to draw from.
// > Right:number=The ending column pixel to draw to.
// > Bottom:number=The ending row pixel to draw to.
// ? Blits tile 0 of texture 0 at the specified bounds.
/* ------------------------------------------------------------------------- */
LLFUNC(BlitLTRB, 0,
  const AgTexture aTexture{lS, 1};
  const AgGLfloat aLeft{lS, 2},
                  aTop{lS, 3},
                  aRight{lS, 4},
                  aBottom{lS, 5};
  aTexture().BlitLTRB(0, 0, aLeft, aTop, aRight, aBottom))
/* ========================================================================= */
// $ Texture:BlitLTWH
// > Left:number=The starting column to draw from.
// > Top:number=The starting row to draw from.
// > Width:number=The width of the quad to draw.
// > Height:number=The height of the quad to draw
// ? Blits tile 0 of texture 0 at the specified screen co-ordinates and with
// ? the specified dimensions.
/* ------------------------------------------------------------------------- */
LLFUNC(BlitLTWH, 0,
  const AgTexture aTexture{lS, 1};
  const AgGLfloat aLeft{lS, 2},
                  aTop{lS, 3},
                  aWidth{lS, 4},
                  aHeight{lS, 5};
  aTexture().BlitLTWH(0, 0, aLeft, aTop, aWidth, aHeight))
/* ========================================================================= */
// $ Texture:BlitLTWHA
// > Left:number=The starting column to draw from.
// > Top:number=The starting row to draw from.
// > Width:number=The width of the quad to draw.
// > Height:number=The height of the quad to draw
// > Angle:number=The angle in which to rotate from the centre point (-1 to 1).
// ? Blits tile 0 of texture 0 at the specified screen co-ordinates and with
// ? the specified dimensions at the specified angle from the centre of the
// ? quad.
/* ------------------------------------------------------------------------- */
LLFUNC(BlitLTWHA, 0,
  const AgTexture aTexture{lS, 1};
  const AgGLfloat aLeft{lS, 2},
                  aTop{lS, 3},
                  aWidth{lS, 4},
                  aHeight{lS, 5};
  const AgAngle aAngle{lS, 6};
  aTexture().BlitLTWHA(0, 0, aLeft, aTop, aWidth, aHeight, aAngle))
/* ========================================================================= */
// $ Texture:BlitM
// > Columns:integer=The number of horizonal textures to blit.
// > Left:number=The left position of the blit.
// > Top:number=The top position of the blit.
// > Right:number=The right position of the blit.
// > Bottom:number=The bottom position of the blit.
// ? Blits multiple textures as one giant texture which is a workaround for
// ? texture size limits on OpenGL.
/* ------------------------------------------------------------------------- */
LLFUNC(BlitM, 0,
  const AgTexture aTexture{lS, 1};
  const AgGLuint aColumns{lS, 2};
  const AgGLfloat aLeft{lS, 3},
                  aTop{lS, 4},
                  aRight{lS, 5},
                  aBottom{lS, 6};
  aTexture().BlitMulti(aColumns, aLeft, aTop, aRight,
    aBottom))
/* ========================================================================= */
// $ Texture:BlitSLT
// > TileIndex:integer=The tile index of the texture to blit.
// > Left:number=The starting column to draw from.
// > Top:number=The starting row to draw from.
// ? Blits tile 'TileIndex' of texture 0 at the specified screen co-ordinates
// ? with the internal tile width and height calculated when created.
/* ------------------------------------------------------------------------- */
LLFUNC(BlitSLT, 0,
  const AgTexture aTexture{lS, 1};
  const AgTileId aTileId{lS, 2, aTexture};
  const AgGLfloat aLeft{lS, 3},
                  aTop{lS, 4};
  aTexture().BlitLT(0, aTileId, aLeft, aTop))
/* ========================================================================= */
// $ Texture:BlitSLTA
// > TileIndex:integer=The tile index of the texture to blit.
// > Left:number=The starting column to draw from.
// > Top:number=The starting row to draw from.
// > Angle:number=The angle in which to rotate from the centre point (-1 to 1).
// ? Blits tile 'TileIndex' of texture 0 at the specified screen co-ordinates
// ? with the internal tile width and height calculated when created and with
// ? the specified angle.
/* ------------------------------------------------------------------------- */
LLFUNC(BlitSLTA, 0,
  const AgTexture aTexture{lS, 1};
  const AgTileId aTileId{lS, 2, aTexture};
  const AgGLfloat aLeft{lS, 3},
                  aTop{lS, 4};
  const AgAngle aAngle{lS, 5};
  aTexture().BlitLTA(0, aTileId, aLeft, aTop, aAngle))
/* ========================================================================= */
// $ Texture:BlitSLTRB
// > TileIndex:integer=The tile index of the texture to blit.
// > Left:number=The starting column to draw from.
// > Top:number=The starting row to draw from.
// > Right:number=The ending column pixel to draw to.
// > Bottom:number=The ending row pixel to draw to.
// ? Blits tile 'TileIndex' of texture 0 at the specified bounds.
/* ------------------------------------------------------------------------- */
LLFUNC(BlitSLTRB, 0,
  const AgTexture aTexture{lS, 1};
  const AgTileId aTileId{lS, 2, aTexture};
  const AgGLfloat aLeft{lS, 3},
                  aTop{lS, 4},
                  aRight{lS, 5},
                  aBottom{lS, 6};
  aTexture().BlitLTRB(0, aTileId, aLeft, aTop, aRight, aBottom))
/* ========================================================================= */
// $ Texture:BlitSLTWH
// > TileIndex:integer=The tile index of the texture to blit.
// > Left:number=The starting column to draw from.
// > Top:number=The starting row to draw from.
// > Width:number=The width of the quad to draw.
// > Height:number=The height of the quad to draw
// ? Blits tile 'TileIndex' of texture 0 at the specified screen co-ordinates
// ? and with the specified dimensions.
/* ------------------------------------------------------------------------- */
LLFUNC(BlitSLTWH, 0,
  const AgTexture aTexture{lS, 1};
  const AgTileId aTileId{lS, 2, aTexture};
  const AgGLfloat aLeft{lS, 3},
                  aTop{lS, 4},
                  aWidth{lS, 5},
                  aHeight{lS, 6};
  aTexture().BlitLTWH(0, aTileId, aLeft, aTop, aWidth, aHeight))
/* ========================================================================= */
// $ Texture:BlitSLTWHA
// > TileIndex:integer=The tile index of the texture to blit.
// > Left:number=The starting column to draw from.
// > Top:number=The starting row to draw from.
// > Width:number=The width of the quad to draw.
// > Height:number=The height of the quad to draw
// > Angle:number=The angle in which to rotate from the centre point (-1 to 1).
// ? Blits tile 'TileIndex' of texture 0 at the specified screen co-ordinates
// ? and with the specified dimensions and angle.
/* ------------------------------------------------------------------------- */
LLFUNC(BlitSLTWHA, 0,
  const AgTexture aTexture{lS, 1};
  const AgTileId aTileId{lS, 2, aTexture};
  const AgGLfloat aLeft{lS, 3},
                  aTop{lS, 4},
                  aWidth{lS, 5},
                  aHeight{lS, 6};
  const AgAngle aAngle{lS, 7};
  aTexture().BlitLTWHA(0, aTileId, aLeft, aTop, aWidth, aHeight, aAngle))
/* ========================================================================= */
// $ Texture:Destroy
// ? Destroys the texture and frees all the memory associated with it. The
// ? OpenGL handles and VRAM allocated by it are freed after the main FBO has
// ? has been rendered. The object will no longer be useable after this call
// ? and an error will be generated if accessed.
/* ------------------------------------------------------------------------- */
LLFUNC(Destroy, 0, LuaUtilClassDestroy<Texture>(lS, cTextures))
/* ========================================================================= */
// $ Texture:Destroyed
// < Destroyed:boolean=If the Texture class is destroyed
// ? Returns if the Texture class is destroyed.
/* ------------------------------------------------------------------------- */
LLFUNC(Destroyed, 1,
  LuaUtilPushVar(lS, LuaUtilIsClassDestroyed(lS, cTextures)))
/* ========================================================================= */
// $ Texture:Download
// > TexId:Integer=The sub-id of the texture to download
// < Handle:Image=The image class
// ? Dumps the texture from VRAM to the specified array. The format of which
// ? is how the image was originally uploaded
/* ------------------------------------------------------------------------- */
LLFUNC(Download, 1,
  const AgTexture aTexture{lS, 1};
  const AgTextureId aTextureId{lS, 2, aTexture};
  AcTexture{lS}().SwapImage({ aTexture().Download(aTextureId) }))
/* ========================================================================= */
// $ Texture:Dump
// ? Dumps the texture to the specified file in the specified format.
/* ------------------------------------------------------------------------- */
LLFUNC(Dump, 0,
  const AgTexture aTexture{lS, 1};
  const AgTextureId aTextureId{lS, 2, aTexture};
  const AgFilename aFilename{lS, 3};
  aTexture().Dump(aTextureId, aFilename))
/* ========================================================================= */
// $ Texture:GetFlags
// < Flags:integer=Texture flags value
// ? Returns the current texture flags.
/* ------------------------------------------------------------------------- */
LLFUNC(GetFlags, 1, LuaUtilPushVar(lS, AgTexture{lS, 1}().FlagGet()))
/* ========================================================================= */
// $ Texture:GetHeight
// < Height:integer=The handle of the new mask created.
// ? Returns height of texture.
/* ------------------------------------------------------------------------- */
LLFUNC(GetHeight, 1, LuaUtilPushVar(lS, AgTexture{lS, 1}().DimGetHeight()))
/* ========================================================================= */
// $ Texture:GetId
// < Id:integer=The id number of the Texture object.
// ? Returns the unique id of the Texture object.
/* ------------------------------------------------------------------------- */
LLFUNC(GetId, 1, LuaUtilPushVar(lS, AgTexture{lS, 1}().CtrGet()))
/* ========================================================================= */
// $ Texture:GetName
// < Name:string=Name of the texture.
// ? If this texture was loaded by a filename or it was set with a custom id.
// ? This function returns that name which was assigned to it.
/* ------------------------------------------------------------------------- */
LLFUNC(GetName, 1, LuaUtilPushVar(lS, AgTexture{lS, 1}().IdentGet()))
/* ========================================================================= */
// $ Texture:GetSubCount
// < Count:integer=The number of sub-textures in this texture
// ? Returns number of sub-textures found in the loaded image file.
/* ------------------------------------------------------------------------- */
LLFUNC(GetSubCount, 1, LuaUtilPushVar(lS, AgTexture{lS, 1}().GetSubCount()))
/* ========================================================================= */
// $ Texture:GetWidth
// < Width:integer=The handle of the new mask created.
// ? Returns width of texture.
/* ------------------------------------------------------------------------- */
LLFUNC(GetWidth, 1, LuaUtilPushVar(lS, AgTexture{lS, 1}().DimGetWidth()))
/* ========================================================================= */
// $ Texture:PopColour
// ? Restores the currently saved colour by PushColour.
/* ------------------------------------------------------------------------- */
LLFUNC(PopColour, 0, AgTexture{lS, 1}().FboItemPopQuadColour())
/* ========================================================================= */
// $ Texture:PushColour
// ? Saves the currently set colour by SetCRGBA. Use PopColour to restore it.
/* ------------------------------------------------------------------------- */
LLFUNC(PushColour, 0, AgTexture{lS, 1}().FboItemPushQuadColour())
/* ========================================================================= */
// $ Texture:SetCA
// > Alpha:number=The transparency of the texture (0-1).
// ? Sets the colour transparency of the texture. The change affects all
// ? subsequent calls to all blitting functions.
/* ------------------------------------------------------------------------- */
LLFUNC(SetCA, 0,
  const AgTexture aTexture{lS, 1};
  const AgGLfloat aColour{lS, 2};
  aTexture().FboItemSetQuadAlpha(aColour))
/* ========================================================================= */
// $ Texture:SetCB
// > Blue:number=The colour intensity of the texture's blue component (0-1).
// ? Sets the colour intensity of the texture for the blue component. The
// ? change affects all subsequent calls to all blitting functions.
/* ------------------------------------------------------------------------- */
LLFUNC(SetCB, 0,
  const AgTexture aTexture{lS, 1};
  const AgGLfloat aColour{lS, 2};
  aTexture().FboItemSetQuadBlue(aColour))
/* ========================================================================= */
// $ Texture:SetCG
// > Green:number=The colour intensity of the texture's green component (0-1).
// ? Sets the colour intensity of the texture for the green component. The
// ? change affects all subsequent calls to all blitting functions.
/* ------------------------------------------------------------------------- */
LLFUNC(SetCG, 0,
  const AgTexture aTexture{lS, 1};
  const AgGLfloat aColour{lS, 2};
  aTexture().FboItemSetQuadGreen(aColour))
/* ========================================================================= */
// $ Texture:SetCR
// > Red:number=The colour intensity of the texture's red component (0-1).
// ? Sets the colour intensity of the texture for the red component. The change
// ? affects all subsequent calls to all blitting functions.
/* ------------------------------------------------------------------------- */
LLFUNC(SetCR, 0,
  const AgTexture aTexture{lS, 1};
  const AgGLfloat aColour{lS, 2};
  aTexture().FboItemSetQuadRed(aColour))
/* ========================================================================= */
// $ Texture:SetCRGB
// > Red:number=The colour intensity of the texture's red component (0-1).
// > Green:number=The colour intensity of the texture's green component (0-1).
// > Blue:number=The colour intensity of the texture's blue omponent (0-1).
// ? Sets the colour intensity of the texture for each component. The change
// ? affects all subsequent calls to all blitting functions.
/* ------------------------------------------------------------------------- */
LLFUNC(SetCRGB, 0,
  const AgTexture aTexture{lS, 1};
  const AgGLfloat aRed{lS, 2},
                  aGreen{lS, 3},
                  aBlue{lS, 4};
  aTexture().FboItemSetQuadRGB(aRed, aGreen, aBlue))
/* ========================================================================= */
// $ Texture:SetCRGBA
// > Red:number=The colour intensity of the texture's red component (0-1).
// > Green:number=The colour intensity of the texture's green component (0-1).
// > Blue:number=The colour intensity of the texture's blue omponent (0-1).
// > Alpha:number=The transparency of the texture (0-1).
// ? Sets the colour intensity of the texture for each component. The change
// ? affects all subsequent calls to all blitting functions.
/* ------------------------------------------------------------------------- */
LLFUNC(SetCRGBA, 0,
  const AgTexture aTexture{lS, 1};
  const AgGLfloat aRed{lS, 2},
                  aGreen{lS, 3},
                  aBlue{lS, 4},
                  aAlpha{lS, 5};
  aTexture().FboItemSetQuadRGBA(aRed, aGreen, aBlue, aAlpha))
/* ========================================================================= */
// $ Texture:SetCRGBAI
// > Colour:integer=The entire colour to set as an integer (0xAARRGGBB)
// ? Sets the colour intensity of the texture for each component using a 32
// ? bit integer.
/* ------------------------------------------------------------------------- */
LLFUNC(SetCRGBAI, 0,
  const AgTexture aTexture{lS, 1};
  const AgUInt32 aColour{lS, 2};
  aTexture().FboItemSetQuadRGBAInt(aColour))
/* ========================================================================= */
// $ Texture:SetCX
// > TriIndex:integer=Triangle index. Triangle #1 (zero) or triangle #2 (one).
// > V1Red:number=Red component of vertex #1 of the specified triangle.
// > V1Green:number=Green component of vertex #1 of the specified triangle.
// > V1Blue:number=Blue component of vertex #1 of the specified triangle.
// > V1Alpha:number=Alpha component of vertex #1 of the specified triangle.
// > V2Red:number=Red component of vertex #2 of the specified triangle.
// > V2Green:number=Green component of vertex #2 of the specified triangle.
// > V2Blue:number=Blue component of vertex #2 of the specified triangle.
// > V2Alpha:number=Alpha component of vertex #2 of the specified triangle.
// > V3Red:number=Red component of vertex #3 of the specified triangle.
// > V3Green:number=Green component of vertex #3 of the specified triangle.
// > V3Blue:number=Blue component of vertex #3 of the specified triangle.
// > V3Alpha:number=Alpha component of vertex #3 of the specified triangle.
// ? Presets the specified colour intensities on each vertex for the
// ? Texture:BlitP* function.
/* ------------------------------------------------------------------------- */
LLFUNC(SetCX, 0,
  const AgTexture aTexture{lS, 1};
  const AgTriangleId aTriangleId{lS, 2};
  const FboBase::TriColData tcdNew{
    AgGLfloat{lS,  3}, AgGLfloat{lS,  4}, AgGLfloat{lS,  5}, AgGLfloat{lS,  6},
    AgGLfloat{lS,  7}, AgGLfloat{lS,  8}, AgGLfloat{lS,  9}, AgGLfloat{lS, 10},
    AgGLfloat{lS, 11}, AgGLfloat{lS, 12}, AgGLfloat{lS, 13}, AgGLfloat{lS, 14}
  };
  aTexture().FboItemSetColourEx(aTriangleId, tcdNew))
/* ========================================================================= */
// $ Texture:SetTCLTRB
// > Left:number=The left co-ordinate.
// > Top:number=The top co-ordinate.
// > Right:number=The right co-ordinate.
// > Bottom:number=The bottom co-ordinate.
// ? Preset the specified texture bounds for the Texture:BlitP* function.
/* ------------------------------------------------------------------------- */
LLFUNC(SetTCLTRB, 0,
  const AgTexture aTexture{lS, 1};
  const AgGLfloat aLeft{lS, 2},
                  aTop{lS, 3},
                  aRight{lS, 4},
                  aBottom{lS, 5};
  aTexture().FboItemSetTexCoord(aLeft, aTop, aRight, aBottom))
/* ========================================================================= */
// $ Texture:SetTCLTWH
// > Left:number=The left co-ordinate.
// > Top:number=The top co-ordinate.
// > Width:number=The width of the tile.
// > Height:number=The height of the tile.
// ? Preset the specified texture co-ordinates and dimensions for the
// ? Texture:BlitP* function.
/* ------------------------------------------------------------------------- */
LLFUNC(SetTCLTWH, 0,
  const AgTexture aTexture{lS, 1};
  const AgGLfloat aLeft{lS, 2},
                  aTop{lS, 3},
                  aWidth{lS, 4},
                  aHeight{lS, 5};
  aTexture().FboItemSetTexCoordWH(aLeft, aTop, aWidth, aHeight))
/* ========================================================================= */
// $ Texture:SetTCX
// > TriIndex:integer=Triangle index. Triangle #1 (zero) or triangle #2 (one).
// > TC1Left:number=GLfloat for X coord of vertex #1 of the specified triangle.
// > TC1Top:number=GLfloat for Y coord of vertex #2 of the specified triangle.
// > TC2Left:number=GLfloat for X coord of vertex #3 of the specified triangle.
// > TC2Top:number=GLfloat for Y coord of vertex #1 of the specified triangle.
// > TC3Left:number=GLfloat for X coord of vertex #2 of the specified triangle.
// > TC3Top:number=GLfloat for Y coord of vertex #3 of the specified triangle.
// ? Presets the specified texture co-ordinates on the specified triangle for
// ? the Texture:BlitP* function.
/* ------------------------------------------------------------------------- */
LLFUNC(SetTCX, 0,
  const AgTexture aTexture{lS, 1};
  const AgTriangleId aTriangleId{lS, 2};
  const FboBase::TriCoordData tcdNew{
    AgGLfloat{lS, 3}, AgGLfloat{lS, 4}, AgGLfloat{lS, 5},
    AgGLfloat{lS, 6}, AgGLfloat{lS, 7}, AgGLfloat{lS, 8}
  };
  aTexture().FboItemSetTexCoordEx(aTriangleId, tcdNew))
/* ========================================================================= */
// $ Texture:SetVLTRB
// > Left:number=The starting column to draw from.
// > Top:number=The starting row to draw from.
// > Right:number=The ending column pixel to draw to.
// > Bottom:number=The ending row pixel to draw to.
// ? Presets these screen bounds for the next use of Texture:BlitP*.
/* ------------------------------------------------------------------------- */
LLFUNC(SetVLTRB, 0,
  const AgTexture aTexture{lS, 1};
  const AgGLfloat aLeft{lS, 2},
                  aTop{lS, 3},
                  aRight{lS, 4},
                  aBottom{lS, 5};
  aTexture().FboItemSetVertex(aLeft, aTop, aRight, aBottom))
/* ========================================================================= */
// $ Texture:SetVLTWH
// > Left:number=The starting column to draw from.
// > Top:number=The starting row to draw from.
// > Width:number=The width of the quad to draw.
// > Height:number=The height of the quad to draw
// ? Presets the specified screen co-ordinates and dimensions for the next use
// ? of Texture:BlitP*.
/* ------------------------------------------------------------------------- */
LLFUNC(SetVLTWH, 0,
  const AgTexture aTexture{lS, 1};
  const AgGLfloat aLeft{lS, 2},
                  aTop{lS, 3},
                  aWidth{lS, 4},
                  aHeight{lS, 5};
  aTexture().FboItemSetVertexWH(aLeft, aTop, aWidth, aHeight))
/* ========================================================================= */
// $ Texture:SetVLTWHA
// > Left:number=The starting column to draw from.
// > Top:number=The starting row to draw from.
// > Width:number=The width of the quad to draw.
// > Height:number=The height of the quad to draw
// > Angle:number=The angle in which to rotate from the centre point (-1 to 1).
// ? Presets the specified screen co-ordinates and dimensions with angle for
// ? the next use of Texture:BlitP*.
/* ------------------------------------------------------------------------- */
LLFUNC(SetVLTWHA, 0,
  const AgTexture aTexture{lS, 1};
  const AgGLfloat aLeft{lS, 2},
                  aTop{lS, 3},
                  aWidth{lS, 4},
                  aHeight{lS, 5};
  const AgAngle aAngle{lS, 6};
  aTexture().FboItemSetVertexWH(aLeft, aTop, aWidth, aHeight, aAngle))
/* ========================================================================= */
// $ Texture:SetVX
// > TriIndex:integer=Triangle index. Triangle #1 (zero) or triangle #2 (one).
// > V1Left:number=GLfloat for X coord of vertex #1 of the specified triangle.
// > V1Top:number=GLfloat for Y coord of vertex #1 of the specified triangle.
// > V2Left:number=GLfloat for X coord of vertex #2 of the specified triangle.
// > V2Top:number=GLfloat for Y coord of vertex #2 of the specified triangle.
// > V3Left:number=GLfloat for X coord of vertex #3 of the specified triangle.
// > V3Top:number=GLfloat for Y coord of vertex #3 of the specified triangle.
// ? Presets the specified co-ordinates on each vertex for the specified
// ? triangle for the Texture:BlitP* function.
/* ------------------------------------------------------------------------- */
LLFUNC(SetVX, 0,
  const AgTexture aTexture{lS, 1};
  const AgTriangleId aTriangleId{lS, 2};
  const FboBase::TriPosData tpdNew{
    AgGLfloat{lS, 3}, AgGLfloat{lS, 4}, AgGLfloat{lS, 5},
    AgGLfloat{lS, 6}, AgGLfloat{lS, 7}, AgGLfloat{lS, 8}
  };
  aTexture().FboItemSetVertexEx(aTriangleId, tpdNew))
/* ========================================================================= */
// $ Texture:TileA
// > Left:integer=The left pixel position on the texture..
// > Top:integer=The top pixel position on the texture..
// > Right:integer=The right pixel position on the texture.
// > Bottom:integer=The bottom pixel position on the texture.
// < Id:integer=The id number for the cached texcoords..
// ? Caches the specified texture coordinates which you can use as a parameter
// ? to 'tileid' on blitting functions.
/* ------------------------------------------------------------------------- */
LLFUNC(TileA, 1,
  const AgTexture aTexture{lS, 1};
  const AgGLfloat aLeft{lS, 2},
                  aTop{lS, 3},
                  aRight{lS, 4},
                  aBottom{lS, 5};
  aTexture().AddTileDOR(0, aLeft, aTop, aRight, aBottom);
  LuaUtilPushVar(lS, aTexture().GetTileCount() - 1))
/* ========================================================================= */
// $ Texture:TileAD
// > Left:integer=The left pixel position on the texture.
// > Top:integer=The top pixel position on the texture.
// > Width:integer=The width of the texture.
// > Height:integer=The height of the texture.
// < Id:integer=The id number for the cached texcoords.
// ? Caches the specified texture coordinates which you can use as a parameter
// ? to 'tileid' on blitting functions.
/* ------------------------------------------------------------------------- */
LLFUNC(TileAD, 1,
  const AgTexture aTexture{lS, 1};
  const AgGLfloat aLeft{lS, 2},
                  aTop{lS, 3},
                  aWidth{lS, 4},
                  aHeight{lS, 5};
  aTexture().AddTileDORWH(0, aLeft, aTop, aWidth, aHeight);
  LuaUtilPushVar(lS, aTexture().GetTileCount() - 1))
/* ========================================================================= */
// $ Texture:TileAS
// > Id:integer=The id of the sub-texture to add to
// > Left:integer=The left pixel position on the texture.
// > Top:integer=The top pixel position on the texture.
// > Right:integer=The right pixel position on the texture
// > Bottom:integer=The bottom pixel position on the texture
// < Id:integer=The id number for the cached texcoords.
// ? Caches the specified texture coordinates which you can use as a parameter
// ? to 'tileid' on blitting functions of the specified sub-texture.
/* ------------------------------------------------------------------------- */
LLFUNC(TileAS, 1,
  const AgTexture aTexture{lS, 1};
  const AgTextureId aTextureId{lS, 2, aTexture};
  const AgGLfloat aLeft{lS, 3},
                  aTop{lS, 4},
                  aRight{lS, 5},
                  aBottom{lS, 6};
  aTexture().AddTileDOR(aTextureId, aLeft, aTop, aRight, aBottom);
  LuaUtilPushVar(lS, aTexture().GetTileCount(aTextureId) - 1))
/* ========================================================================= */
// $ Texture:TileASD
// > Id:integer=The id of the sub-texture to add to.
// > Left:integer=The left pixel position on the texture..
// > Top:integer=The top pixel position on the texture..
// > Width:integer=The width of the texture..
// > Height:integer=The height of the texture..
// < Id:integer=The id number for the cached texcoords..
// ? Caches the specified texture coordinates which you can use as a parameter
// ? to 'tileid' on blitting functions of the specified sub-texture.
/* ------------------------------------------------------------------------- */
LLFUNC(TileASD, 1,
  const AgTexture aTexture{lS, 1};
  const AgTextureId aTextureId{lS, 2, aTexture};
  const AgGLfloat aLeft{lS, 3},
                  aTop{lS, 4},
                  aWidth{lS, 5},
                  aHeight{lS, 6};
  aTexture().AddTileDORWH(aTextureId, aLeft, aTop, aWidth, aHeight);
  LuaUtilPushVar(lS, aTexture().GetTileCount(aTextureId) - 1))
/* ========================================================================= */
// $ Texture:TileGSTC
// > Id:integer=The id of the sub-texture to get the count for.
// < Count:integer=The handle of the texture created.
// ? Returns number of tiles found in the loaded texture file with the
// ? specified sub-texture id.
/* ------------------------------------------------------------------------- */
LLFUNC(TileGSTC, 1,
  const AgTexture aTexture{lS, 1};
  const AgTileId aTileId{lS, 2, aTexture};
  LuaUtilPushVar(lS, aTexture().GetTileCount(aTileId)))
/* ========================================================================= */
// $ Texture:TileGTC
// < Count:integer=The handle of the texture created.
// ? Returns number of tiles found in the loaded texture file.
/* ------------------------------------------------------------------------- */
LLFUNC(TileGTC, 1, LuaUtilPushVar(lS, AgTexture{lS, 1}().GetTileCount()))
/* ========================================================================= */
// $ Texture:TileS
// > Id:integer=The id of the tile to change
// > Left:integer=The left pixel position on the texture.
// > Top:integer=The top pixel position on the texture.
// > Right:integer=The right pixel position on the texture
// > Bottom:integer=The bottom pixel position on the texture
// ? Changes the specified texture coordinates which you can use as a parameter
// ? to 'tileid' on blitting functions.
/* ------------------------------------------------------------------------- */
LLFUNC(TileS, 0,
  const AgTexture aTexture{lS, 1};
  const AgTileId aTileId{lS, 2, aTexture};
  const AgGLfloat aLeft{lS, 3},
                  aTop{lS, 4},
                  aRight{lS, 5},
                  aBottom{lS, 6};
  aTexture().SetTileDOR(0, aTileId, aLeft, aTop, aRight, aBottom))
/* ========================================================================= */
// $ Texture:TileSD
// > Id:integer=The id of the tile to change
// > Left:integer=The left pixel position on the texture.
// > Top:integer=The top pixel position on the texture.
// > Width:integer=The width of the texture.
// > Height:integer=The height of the texture.
// ? Changes the specified texture coordinates which you can use as a parameter
// ? to 'tileid' on blitting functions.
/* ------------------------------------------------------------------------- */
LLFUNC(TileSD, 0,
  const AgTexture aTexture{lS, 1};
  const AgTileId aTileId{lS, 2, aTexture};
  const AgGLfloat aLeft{lS, 3},
                  aTop{lS, 4},
                  aWidth{lS, 5},
                  aHeight{lS, 6};
  aTexture().SetTileDORWH(0, aTileId, aLeft, aTop, aWidth, aHeight))
/* ========================================================================= */
// $ Texture:TileSS
// > TexId:integer=The id of the sub texture to change.
// > TileId:integer=The id of the tile id to change.
// > Left:integer=The left pixel position on the texture.
// > Top:integer=The top pixel position on the texture.
// > Right:integer=The right pixel position on the texture
// > Bottom:integer=The bottom pixel position on the texture
// ? Changes the specified texture coordinates which you can use as a parameter
// ? to 'tileid' on blitting functions.
/* ------------------------------------------------------------------------- */
LLFUNC(TileSS, 0,
  const AgTexture aTexture{lS, 1};
  const AgTextureId aTextureId{lS, 2, aTexture};
  const AgTileId aTileId{lS, 3, aTexture};
  const AgGLfloat aLeft{lS, 4},
                  aTop{lS, 5},
                  aRight{lS, 6},
                  aBottom{lS, 7};
  aTexture().SetTileDOR(aTextureId, aTileId, aLeft, aTop, aRight, aBottom))
/* ========================================================================= */
// $ Texture:TileSSD
// > TexId:integer=The id of the sub texture to change.
// > TileId:integer=The id of the tile id to change.
// > Left:integer=The left pixel position on the texture.
// > Top:integer=The top pixel position on the texture.
// > Width:integer=The width of the texture.
// > Height:integer=The height of the texture.
// ? Changes the specified texture coordinates which you can use as a parameter
// ? to 'tileid' on blitting functions.
/* ------------------------------------------------------------------------- */
LLFUNC(TileSSD, 0,
  const AgTexture aTexture{lS, 1};
  const AgTextureId aTextureId{lS, 2, aTexture};
  const AgTileId aTileId{lS, 3, aTexture};
  const AgGLfloat aLeft{lS, 4},
                  aTop{lS, 5},
                  aWidth{lS, 6},
                  aHeight{lS, 7};
  aTexture().SetTileDORWH(aTextureId, aTileId, aLeft, aTop, aWidth, aHeight))
/* ========================================================================= */
// $ Texture:TileSSTC
// > Id:integer=The id of the sub-texture to set the size of.
// > Count:integer=The new size of the tile list.
// ? Modifies the size of the tile list. Useful if you want to manage
// ? your tile texcoord list dynamically. If the new size is below the current
// ? size then existing entries will be deleted, otherwise if the new size
// ? is larger, then new blank entries are created and are set to zero. You
// ? will need to use SetTileData to change them.
/* ------------------------------------------------------------------------- */
LLFUNC(TileSSTC, 0,
  const AgTexture aTexture{lS, 1};
  const AgTextureId aTextureId{lS, 2, aTexture};
  const AgSizeTLG aCount{lS, 3, 0, UINT_MAX};
  aTexture().SetTileCount(aCount, aTextureId))
/* ========================================================================= */
// $ Texture:TileSTC
// > Count:integer=The new size of the tile list.
// ? Modifies the size of the tile list. Useful if you want to manage
// ? your tile texcoord list dynamically. If the new size is below the current
// ? size then existing entries will be deleted, otherwise if the new size
// ? is larger, then new blank entries are created and are set to zero. You
// ? will need to use SetTileData to change them.
/* ------------------------------------------------------------------------- */
LLFUNC(TileSTC, 0,
  const AgTexture aTexture{lS, 1};
  const AgSizeTLG aCount{lS, 2, 0, UINT_MAX};
  aTexture().SetTileCount(aCount))
/* ========================================================================= */
// $ Texture:Upload
// > Data:Image=The pixel data to update the texture with
// ? Updates the whole texture in VRAM with the texture specified in the array.
// ? The array data must be the same width, the same height and the same pixel
// ? type of when the texture was first originally updated, thus the size of
// ? the memory must match the actual texture memory. The source and
// ? destination colour format must be the same, or use UploadEx
/* ------------------------------------------------------------------------- */
LLFUNC(Upload, 0, AgTexture{lS, 1}().Update(AgImage{lS,2}()))
/* ========================================================================= */
// $ Texture:UploadEx
// > TexId:Integer=The sub-id of the texture to download
// > Data:Image=The image to upload to the texture
// > OffsetX:Integer=The X position on the destination texture where to place
// > OffsetY:Integer=The Y position on the destination texture where to place
// ? Updates a partial area of the supplied texture in VRAM with the texture
// ? data specified in the array. Only sources and destinations of 24 and 32
// ? bits per pixel are supported.
/* ------------------------------------------------------------------------- */
LLFUNC(UploadEx, 0,
  const AgTexture aTexture{lS, 1};
  const AgTextureId aTextureId{lS, 2, aTexture};
  const AgImage aImage{lS, 3};
  const AgGLint aX{lS, 4}, aY{lS, 5};
  aTexture().UpdateEx(aTextureId, aImage, aX, aY))
/* ========================================================================= **
** ######################################################################### **
** ## Texture:* member functions structure                                ## **
** ######################################################################### **
** ========================================================================= */
LLRSMFBEGIN                            // Texture:* member functions begin
  LLRSFUNC(Blit),        LLRSFUNC(BlitILT),     LLRSFUNC(BlitILTA),
  LLRSFUNC(BlitILTRB),   LLRSFUNC(BlitILTWH),   LLRSFUNC(BlitILTWHA),
  LLRSFUNC(BlitISLT),    LLRSFUNC(BlitISLTA),   LLRSFUNC(BlitISLTRB),
  LLRSFUNC(BlitISLTWH),  LLRSFUNC(BlitISLTWHA), LLRSFUNC(BlitLT),
  LLRSFUNC(BlitLTA),     LLRSFUNC(BlitLTRB),    LLRSFUNC(BlitLTWH),
  LLRSFUNC(BlitLTWHA),   LLRSFUNC(BlitM),       LLRSFUNC(BlitSLT),
  LLRSFUNC(BlitSLTA),    LLRSFUNC(BlitSLTRB),   LLRSFUNC(BlitSLTWH),
  LLRSFUNC(BlitSLTWHA),  LLRSFUNC(Destroy),     LLRSFUNC(Destroyed),
  LLRSFUNC(Download),    LLRSFUNC(Dump),        LLRSFUNC(GetFlags),
  LLRSFUNC(GetHeight),   LLRSFUNC(GetId),       LLRSFUNC(GetName),
  LLRSFUNC(GetSubCount), LLRSFUNC(GetWidth),    LLRSFUNC(PopColour),
  LLRSFUNC(PushColour),  LLRSFUNC(SetCA),       LLRSFUNC(SetCB),
  LLRSFUNC(SetCG),       LLRSFUNC(SetCR),       LLRSFUNC(SetCRGB),
  LLRSFUNC(SetCRGBA),    LLRSFUNC(SetCRGBAI),   LLRSFUNC(SetCX),
  LLRSFUNC(SetTCLTRB),   LLRSFUNC(SetTCLTWH),   LLRSFUNC(SetTCX),
  LLRSFUNC(SetVLTRB),    LLRSFUNC(SetVLTWH),    LLRSFUNC(SetVLTWHA),
  LLRSFUNC(SetVX),       LLRSFUNC(TileA),       LLRSFUNC(TileAD),
  LLRSFUNC(TileAS),      LLRSFUNC(TileASD),     LLRSFUNC(TileGSTC),
  LLRSFUNC(TileGTC),     LLRSFUNC(TileS),       LLRSFUNC(TileSD),
  LLRSFUNC(TileSS),      LLRSFUNC(TileSSD),     LLRSFUNC(TileSSTC),
  LLRSFUNC(TileSTC),     LLRSFUNC(Upload),      LLRSFUNC(UploadEx),
LLRSEND                                // Texture:* member functions end
/* ========================================================================= **
** ######################################################################### **
** ## Texture.* namespace functions                                       ## **
** ######################################################################### **
** ========================================================================= */
// $ Texture.Console
// < Handle:Texture=Texture handle to console texture
// ? Returns the handle to the console texture. Useful if you want to reuse the
// ? texture in your application. Careful not to mess around with it's
// ? properties!
/* ------------------------------------------------------------------------- */
LLFUNC(Console, 1,
  LuaUtilClassCreatePtr<Texture>(lS, *cTextures, cConGraphics->GetTexture()))
/* ========================================================================= */
// $ Texture.Create
// > Source:image=The image class to load from.
// > Filter:integer=The default filter to use
// < Handle:Texture=A handle to the texture(s) created.
// ? Creates the specified texture from the specified image class.
/* ------------------------------------------------------------------------- */
LLFUNC(Create, 1,
  const AgImage aImage{lS, 1};
  const AgFilterId aFilterId{lS, 2};
  AcTexture{lS}().InitTextureImage(aImage, 0, 0, 0, 0, aFilterId))
/* ========================================================================= */
// $ Texture.Count
// < Count:integer=Total number of textures created.
// ? Returns the total number of texture classes currently active.
/* ------------------------------------------------------------------------- */
LLFUNC(Count, 1, LuaUtilPushVar(lS, cTextures->CollectorCount()))
/* ========================================================================= */
// $ Texture.Manifest
// > Source:image=The image class to load from.
// > Manifest:Jsonr=The Json object containing the texture metadata.
// < Handle:Texture=A handle to the texture(s) created.
// ? Creates the specified texture from the specified image class with the
// ? specified json metadata.
/* ------------------------------------------------------------------------- */
LLFUNC(Manifest, 1,
  const AgImage aImage{lS, 1};
  const AgJson aJson{lS, 2};
  AcTexture{lS}().InitTextureImageManifest(aImage, aJson))
/* ========================================================================= */
// $ Texture.MaxSize
// < Size:integer=Maximum supported texture size.
// ? Returns the maximum texture size supported by the rendering system.
/* ------------------------------------------------------------------------- */
LLFUNC(MaxSize, 1, LuaUtilPushVar(lS, cOgl->MaxTexSize()))
/* ========================================================================= */
// $ Texture.CreateTS
// > Source:image=The image class to load from.
// > TWidth:integer=The width of each tile in the image.
// > THeight:integer=The height of each tile in the image.
// > PWidth:integer=The width padding between each tile in the image.
// > PHeight:integer=The height padding between each tile in the image.
// > Filter:integer=The filtering setting to use on texture
// < Handle:Texture=A handle to the texture(s) created.
// ? Loads a texture as a tileset from the specified image class.
/* ------------------------------------------------------------------------- */
LLFUNC(CreateTS, 1,
  const AgImage aImage{lS, 1};
  const AgUIntLG aTW{lS, 2, 0, UINT_MAX}, aTH{lS, 3, 0, UINT_MAX},
                 aPW{lS, 4, 0, UINT_MAX}, aPH{lS, 5, 0, UINT_MAX};
  const AgFilterId aFilterId{lS, 6};
  AcTexture{lS}().InitTextureImage(aImage, aTW, aTH, aPW, aPH, aFilterId))
/* ========================================================================= */
// $ Texture.ImageUT
// > Source:image=The image class to load from.
// > Filter:integer=The filtering setting to use on texture
// > Tiles:table=The tileset to use (X, Y, Width, Height, [...]).
// < Handle:Texture=A handle to the texture(s) created.
// ? Loads a texture and applies the specified tile set.
/* ------------------------------------------------------------------------- */
LLFUNC(ImageUT, 1,
  const AgImage aImage{lS, 1};
  const AgFilterId aFilterId{lS, 2};
  const AgIntegerVector<GLuint> aTiles{lS, 3};
  AcTexture{lS}().InitTextureImageTiles(aImage, aFilterId, aTiles))
/* ========================================================================= **
** ######################################################################### **
** ## Texture.* namespace functions structure                             ## **
** ######################################################################### **
** ========================================================================= */
LLRSBEGIN                              // Texture.* namespace functions begin
  LLRSFUNC(Create),  LLRSFUNC(Count),    LLRSFUNC(ImageUT), LLRSFUNC(Manifest),
  LLRSFUNC(MaxSize), LLRSFUNC(CreateTS), LLRSFUNC(Console),
LLRSEND                                // Texture.* namespace functions end
/* ========================================================================= */
}                                      // End of Texture namespace
/* == EoF =========================================================== EoF == */
