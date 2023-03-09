/* == LLIMAGE.HPP ========================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## Defines the 'Image' namespace and methods for the guest to use in   ## **
** ## Lua. This file is invoked by 'lualib.hpp'.                          ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
#pragma once                           // Only one incursion allowed
/* ========================================================================= **
** ######################################################################### **
** ========================================================================= */
// % Image
/* ------------------------------------------------------------------------- */
// ! This allows the programmer to manipulate images asynchronously and send
// ! the objects to the OpenGL if needed.
// !
// ! The list and order of supported formats can is subject to change without
// ! notice. Type 'imgfmts' in the console to see the most up to date list of
// ! formats supported.
/* ========================================================================= */
namespace LLImage {                    // Image namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IAsset::P;             using namespace IImage::P;
using namespace IImageDef::P;          using namespace IStd::P;
using namespace ITexDef::P;            using namespace Common;
/* ========================================================================= **
** ######################################################################### **
** ## Image common helper classes                                         ## **
** ######################################################################### **
** -- Create Image class argument ------------------------------------------ */
struct AcImage : public ArClass<Image> {
  explicit AcImage(lua_State*const lS) :
    ArClass{*LuaUtilClassCreate<Image>(lS, *cImages)}{} };
/* -- Read Image flags argument -------------------------------------------- */
struct AgImageFlags : public AgFlags<ImageFlagsConst> {
  explicit AgImageFlags(lua_State*const lS, const int iArg) :
    AgFlags{ lS, iArg, IL_MASK }{} };
/* -- Read a dimension ----------------------------------------------------- */
struct AgDimension : public AgUIntLGE {
  explicit AgDimension(lua_State*const lS, const int iArg) :
    AgUIntLGE{ lS, iArg, 1, numeric_limits<uint16_t>::max() }{} };
/* ========================================================================= **
** ######################################################################### **
** ## Image:* member functions                                            ## **
** ######################################################################### **
** ========================================================================= */
// $ Image:Depth
// < Height:integer=The bit-depth of the image in bits
// ? Returns the bit-depth of the image
/* ------------------------------------------------------------------------- */
LLFUNC(Depth, 1, LuaUtilPushVar(lS, AgImage{lS, 1}().GetBitsPerPixel()))
/* ========================================================================= */
// $ Image:Destroy
// ? Destroys the image and frees all the memory associated with it. The
// ? object will no longer be useable after this call and an error will be
// ? generated if accessed.
/* ------------------------------------------------------------------------- */
LLFUNC(Destroy, 0, LuaUtilClassDestroy<Image>(lS, 1, *cImages))
/* ========================================================================= */
// $ Image:Height
// < Height:integer=The height of the image in pixels
// ? Returns the height if the image.
/* ------------------------------------------------------------------------- */
LLFUNC(Height, 1, LuaUtilPushVar(lS, AgImage{lS, 1}().DimGetHeight()))
/* ========================================================================= */
// $ Image:Id
// < Id:integer=The id number of the Image object.
// ? Returns the unique id of the Image object.
/* ------------------------------------------------------------------------- */
LLFUNC(Id, 1, LuaUtilPushVar(lS, AgImage{lS, 1}().CtrGet()))
/* ========================================================================= */
// $ Image:Name
// < Name:string=Name of the image.
// ? If this font was loaded by a filename or it was set with a custom id.
// ? This function returns that name which was assigned to it.
/* ------------------------------------------------------------------------- */
LLFUNC(Name, 1, LuaUtilPushVar(lS, AgImage{lS, 1}().IdentGet()))
/* ========================================================================= */
// $ Image:Save
// < File:string=The filename to save to.
// ? Saves the file to disk.
/* ------------------------------------------------------------------------- */
LLFUNC(Save, 0,
  const AgImage aImage{lS, 1};
  const AgFilename aFilename{lS, 2};
  aImage().SaveFile(aFilename, 0, IFMT_PNG))
/* ========================================================================= */
// $ Image:Width
// < Width:integer=The width of the image in pixels
// ? Returns the width if the image.
/* ------------------------------------------------------------------------- */
LLFUNC(Width, 1, LuaUtilPushVar(lS, AgImage{lS, 1}().DimGetWidth()))
/* ========================================================================= **
** ######################################################################### **
** ## Image:* member functions structure                                  ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
LLRSMFBEGIN                            // Image:* member functions begin
  LLRSFUNC(Destroy), LLRSFUNC(Depth), LLRSFUNC(Height),  LLRSFUNC(Id),
  LLRSFUNC(Name),    LLRSFUNC(Save),  LLRSFUNC(Width),
LLRSEND                                // Image:* member functions end
/* ========================================================================= */
// $ Image.Asset
// > Name:String=The user-defined id of the image
// > Data:Asset=The data of the image to load
// > Flags:Integer=Load flags
// < Handle:Image=The image object
// ? Loads a image on the main thread from the specified asset object. The
// ? Asset object specified is consumed upon load. Returns an image object.
/* ------------------------------------------------------------------------- */
LLFUNC(Asset, 1,
  const AgNeString aIdentifier{lS, 1};
  const AgAsset aAsset{lS, 2};
  const AgImageFlags aFlags{lS, 3};
  AcImage{lS}().InitArray(aIdentifier, aAsset, aFlags))
/* ========================================================================= */
// $ Image.AssetAsync
// > Name:string=A user-defined id of the image
// > Data:array=The image data array
// > Flags:Integer=Load flags
// > ErrorFunc:function=The function to call when there is an error
// > ProgressFunc:function=The function to call when there is progress
// > SuccessFunc:function=The function to call when the image is laoded
// ? Loads a image off the main thread from the specified array object. The
// ? callback functions send an argument to the image object that was created.
/* ------------------------------------------------------------------------- */
LLFUNC(AssetAsync, 0,
  LuaUtilCheckParams(lS, 6);
  const AgNeString aIdentifier{lS, 1};
  const AgAsset aAsset{lS, 2};
  const AgImageFlags aFlags{lS, 3};
  LuaUtilCheckFunc(lS, 4, 5, 6);
  AcImage{lS}().InitAsyncArray(lS, aIdentifier, aAsset, aFlags))
/* ========================================================================= */
// $ Image.Blank
// > Name:string=The name of the texture
// > Width:integer=The width of the texture
// > Height:integer=The height of the texture
// > Filter:integer=The texture filtering mode
// < Handle:Texture=A handle to the texture(s) created.
// ? Creates an empty blank texture for manipulation.
/* ------------------------------------------------------------------------- */
LLFUNC(Blank, 1,
  const AgNeString aIdentifier{lS, 1};
  const AgDimension aWidth{lS, 2},
                    aHeight{lS, 3};
  const AgBoolean aAlpha{lS, 4},
                  aClear{lS, 5};
  AcImage{lS}().InitBlank(aIdentifier, aWidth, aHeight, aAlpha, aClear))
/* ========================================================================= */
// $ Image.Colour
// > Colour:integer=The colour of the pixel (Syntax: 0xAABBGGRR)
// < Handle:Image=The image object
// ? Loads a 1x1 pixel for use as a solid.
/* ------------------------------------------------------------------------- */
LLFUNC(Colour, 1, const AgUInt32 aColour{lS, 1};
  AcImage{lS}().InitColour(aColour))
/* ========================================================================= */
// $ Image.File
// > Filename:string=The filename of the image to load
// > Flags:Integer=Load flags
// < Handle:Image=The image object
// ? Loads a image on the main thread from the specified file on disk. Returns
// ? the image object.
/* ------------------------------------------------------------------------- */
LLFUNC(File, 1,
  const AgFilename aFilename{lS, 1};
  const AgImageFlags aFlags{lS, 2};
  AcImage{lS}().InitFile(aFilename, aFlags))
/* ========================================================================= */
// $ Image.FileAsync
// > Filename:string=The filename of the image to load
// > Flags:Integer=Load flags
// > ErrorFunc:function=The function to call when there is an error
// > ProgressFunc:function=The function to call when there is progress
// > SuccessFunc:function=The function to call when the image is laoded
// ? Loads a image off the main thread. The callback functions send an
// ? argument to the image object that was created.
/* ------------------------------------------------------------------------- */
LLFUNC(FileAsync, 0,
  LuaUtilCheckParams(lS, 5);
  const AgFilename aFilename{lS, 1};
  const AgImageFlags aFlags{lS, 2};
  LuaUtilCheckFunc(lS, 3, 4, 5);
  AcImage{lS}().InitAsyncFile(lS, aFilename, aFlags))
/* ========================================================================= */
// $ Image.Raw
// > Data:Asset=The data of the image to load which is consumed.
// > Width:integer=The width of the image being sent (1-65535)
// > Height:integer=The height of the image being sent (1-65535)
// > Bits:integer=Bits per pixel of the image being sent (1,8,16,24 or 32)
// < Handle:Image=The image object
// ? Loads a image on the main thread from the specified array object.
/* ------------------------------------------------------------------------- */
LLFUNC(Raw, 1,
  const AgNeString aIdentifier{lS, 1};
  const AgAsset aAsset{lS, 2};
  const AgDimension aWidth{lS, 3},
                    aHeight{lS, 4};
  const AgIntegerLG<BitDepth> aBits{lS, 5, BD_BINARY, BD_RGBA};
  AcImage{lS}().InitRaw(aIdentifier, aAsset, aWidth, aHeight, aBits))
/* ========================================================================= */
// $ Image.WaitAsync
// ? Halts main-thread execution until all async image events have completed
/* ------------------------------------------------------------------------- */
LLFUNC(WaitAsync, 0, cImages->WaitAsync())
/* ========================================================================= **
** ######################################################################### **
** ## Image.* namespace functions structure                               ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
LLRSBEGIN                              // Image.* namespace functions begin
  LLRSFUNC(AssetAsync), LLRSFUNC(Asset),     LLRSFUNC(Blank),
  LLRSFUNC(Colour),     LLRSFUNC(File),      LLRSFUNC(FileAsync),
  LLRSFUNC(Raw),        LLRSFUNC(WaitAsync),
LLRSEND                                // Image.* namespace functions end
/* ========================================================================= **
** ######################################################################### **
** ## Image.* namespace constants                                         ## **
** ######################################################################### **
** ========================================================================= */
// @ Image.Flags
// < Codes:table=The table of key/value pairs of available flags
// ? Returns the image plugins available. Returned as key/value pairs. The
// ? value is a unique identifier to the flag.
/* ------------------------------------------------------------------------- */
LLRSKTBEGIN(Flags)                     // Beginning of image flags codes
  LLRSKTITEM(IL_,NONE),    LLRSKTITEM(IL_,TOGPU),    LLRSKTITEM(IL_,TO24BPP),
  LLRSKTITEM(IL_,TO32BPP), LLRSKTITEM(IL_,TOBGR),    LLRSKTITEM(IL_,TORGB),
  LLRSKTITEM(IL_,REVERSE), LLRSKTITEM(IL_,TOBINARY), LLRSKTITEM(IL_,ATLAS),
  LLRSKTITEM(IL_,FCE_DDS), LLRSKTITEM(IL_,FCE_GIF),  LLRSKTITEM(IL_,FCE_JPG),
  LLRSKTITEM(IL_,FCE_PNG),
LLRSKTEND                              // End of image flags codes
/* ------------------------------------------------------------------------- */
// @ Image.Formats
// < Codes:table=The table of key/value pairs of available texture formats
// ? Returns the texture formats supported.
/* ------------------------------------------------------------------------- */
LLRSKTBEGIN(Formats)                   // Beginning of image formats codes
  LLRSKTITEM(TT_,BGR),       LLRSKTITEM(TT_,BGRA), LLRSKTITEM(TT_,DXT1),
  LLRSKTITEM(TT_,DXT3),      LLRSKTITEM(TT_,DXT5), LLRSKTITEM(TT_,GRAY),
  LLRSKTITEM(TT_,GRAYALPHA), LLRSKTITEM(TT_,RGB),  LLRSKTITEM(TT_,RGBA),
LLRSKTEND                              // End of image formats codes
/* ========================================================================= **
** ######################################################################### **
** ## Image.* namespace constants structure                               ## **
** ######################################################################### **
** ========================================================================= */
LLRSCONSTBEGIN                         // Image.* namespace consts begin
  LLRSCONST(Flags), LLRSCONST(Formats),
LLRSCONSTEND                           // Image.* namespace consts end
/* ========================================================================= */
}                                      // End of Image namespace
/* == EoF =========================================================== EoF == */
