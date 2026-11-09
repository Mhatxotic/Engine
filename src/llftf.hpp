/* == LLFTF.HPP ============================================================ **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## Defines the 'Ftf' namespace and methods for the guest to use in     ## **
** ## Lua. This file is invoked by 'lualib.hpp'.                          ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ========================================================================= **
** ######################################################################### **
** ========================================================================= */
// % Ftf
/* ------------------------------------------------------------------------- */
// ! This allows the programmer to maniuplate supported freetype fonts forms
// ! asynchronously and send the objects to a font rendering object.
/* ========================================================================= */
namespace LLFtf {                      // Ftf namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IAsset::P;             using namespace IFtf::P;
using namespace IStd::P;               using namespace Common;
/* ========================================================================= **
** ######################################################################### **
** ## Ftf common helper classes                                           ## **
** ######################################################################### **
** -- Create Ftf class argument -------------------------------------------- */
struct AcFtf : public ArClass<Ftf> {
  explicit AcFtf(lua_State*const lS) :
    ArClass{LuaUtilClassCreateRef<Ftf>(lS, cFtfs)}{} };
/* -- Get Ftf object ------------------------------------------------------- */
struct AgFtf : public ArClass<Ftf> {
  explicit AgFtf(lua_State*const lS, const int iArg) :
    ArClass{LuaUtilGetClassRef<Ftf>(lS, iArg, cFtfs)}{} };
/* -- Read a font dimension argument --------------------------------------- */
struct AgDimension : public AgGLfloatLG
  { explicit AgDimension(lua_State*const lS, const int iArg) :
      AgGLfloatLG{lS, iArg, 1.0f, 4096.0f}{} };
/* -- Read a DPI dimension argument ---------------------------------------- */
struct AgDpiDimension : public AgUIntLG
  { explicit AgDpiDimension(lua_State*const lS, const int iArg) :
      AgUIntLG{lS, iArg, 1, 1024}{} };
/* -- Read an outline argument --------------------------------------------- */
struct AgOutline : public AgGLfloatLG
  { explicit AgOutline(lua_State*const lS, const int iArg) :
      AgGLfloatLG{lS, iArg, 0.0f, 1024.0f}{} };
/* ========================================================================= **
** ######################################################################### **
** ## Ftf:* methods                                                       ## **
** ######################################################################### **
** ========================================================================= */
// $ Ftf:Destroy
// ? Destroys the freetype font and frees all the memory associated with it.
// ? The object will no longer be useable after this call and an error will be
// ? generated if accessed.
/* ------------------------------------------------------------------------- */
LLFUNC(Destroy, 0, LuaUtilClassDestroyChecked<Ftf>(lS, cFtfs))
/* ========================================================================= */
// $ Ftf:GetFamily
// < Name:string=The internal name of the freetype font
// ? Returns the internal name of the actual freetype font. (e.g. Arial)
/* ------------------------------------------------------------------------- */
LLFUNC(GetFamily, 1, LuaUtilPushVar(lS, AgFtf{lS, 1}().FtfGetFamily()))
/* ========================================================================= */
// $ Ftf:GetId
// < Id:integer=The id number of the Ftf object.
// ? Returns the unique id of the Ftf object.
/* ------------------------------------------------------------------------- */
LLFUNC(GetId, 1, LuaUtilPushVar(lS, AgFtf{lS, 1}().Serial()))
/* ========================================================================= */
// $ Ftf:GetGlyphs
// < Count:integer=Number of glyphs available.
// ? Returns the internal number of glyphs supported by this freetype font.
/* ------------------------------------------------------------------------- */
LLFUNC(GetGlyphs, 1, LuaUtilPushVar(lS, AgFtf{lS, 1}().FtfGetGlyphCount()))
/* ========================================================================= */
// $ Ftf:GetName
// < Name:string=The name of the freetype font
// ? Returns the name of the specified object when it was created.
/* ------------------------------------------------------------------------- */
LLFUNC(GetName, 1, LuaUtilPushVar(lS, AgFtf{lS, 1}().NameGet()))
/* ========================================================================= */
// $ Ftf:GetStyle
// < Name:string=The internal style name of the freetype font
// ? Returns the internal name of the actual freetype font. (e.g. Bold)
/* ------------------------------------------------------------------------- */
LLFUNC(GetStyle, 1, LuaUtilPushVar(lS, AgFtf{lS, 1}().FtfGetStyle()))
/* ========================================================================= */
// $ Ftf:IsDestroyed
// < Destroyed:boolean=If the Ftf class is destroyed
// ? Returns if the Ftf class is destroyed.
/* ------------------------------------------------------------------------- */
LLFUNC(IsDestroyed, 1, LuaUtilPushVar(lS, LuaUtilIsClassDestroyed(lS, cFtfs)))
/* ========================================================================= **
** ######################################################################### **
** ## Ftf:* methods structure                                             ## **
** ######################################################################### **
** ========================================================================= */
LLRSMFBEGIN                            // Ftf:* methods begin
  LLRSFUNC(Destroy),     LLRSFUNC(GetFamily), LLRSFUNC(GetGlyphs),
  LLRSFUNC(GetId),       LLRSFUNC(GetName),   LLRSFUNC(GetStyle),
  LLRSFUNC(IsDestroyed),
LLRSEND                                // Ftf:* methods end
/* ========================================================================= **
** ######################################################################### **
** ## Ftf.* non-member functions                                          ## **
** ######################################################################### **
** ========================================================================= */
// $ Ftf.Asset
// > Id:String=The identifier string for the Freetype object.
// > Data:Asset=The raw font data supported by FreeType.
// > Width:number=The width of the font in pixels.
// > Height:number=The height of the font in pixels.
// > DPIWidth:integer=The DPI width of the font.
// > DPIHeight:integer=The DPI height of the font.
// > Outline:number=The pixels to add for a border.
// < Handle:Ftf=The Ftf object
// ? Loads a font into the freetype system and returns a handle you can use
// ? with the Font.Create() function.
/* ------------------------------------------------------------------------- */
LLFUNC(Asset, 1,
  const AgNeString aName{lS, 1};
  const AgAsset aAsset{lS, 2};
  const AgDimension aWidth{lS, 3},
                    aHeight{lS, 4};
  const AgDpiDimension aDpiWidth{lS, 5},
                       aDpiHeight{lS, 6};
  const AgOutline aOutline{lS, 7};
  AcFtf{lS}().FtfInitArray(aName(), aAsset, aWidth, aHeight, aDpiWidth,
    aDpiHeight, aOutline))
/* ========================================================================= */
// $ Ftf.AssetAsync
// > Id:String=The identifier string for the Freetype object.
// > Data:Asset=The raw font data supported by FreeType.
// > Width:number=The width of the font in pixels.
// > Height:number=The height of the font in pixels.
// > DPIWidth:integer=The DPI width of the font.
// > DPIHeight:integer=The DPI height of the font.
// > Outline:number=The pixels to add for a border.
// > ErrorFunc:function=The function to call when there is an error
// > ProgressFunc:function=The function to call when there is progress
// > SuccessFunc:function=The function to call when the audio file is laoded
// ? Asyncronously loads a freetype font into memory for use with the
// ? Font.Create() function. The original Asset class is destroyed in the
// ? process.
/* ------------------------------------------------------------------------- */
LLFUNC(AssetAsync, 0,
  LuaUtilCheckParams(lS, 10);
  const AgNeString aName{lS, 1};
  const AgAsset aAsset{lS, 2};
  const AgDimension aWidth{lS, 3},
                    aHeight{lS, 4};
  const AgDpiDimension aDpiWidth{lS, 5},
                       aDpiHeight{lS, 6};
  const AgOutline aOutline{lS, 7};
  LuaUtilCheckFunc(lS, 8, 9, 10);
  AcFtf{lS}().FtfInitAsyncArray(lS, aName(), aAsset, aWidth, aHeight,
    aDpiWidth, aDpiHeight, aOutline))
/* ========================================================================= */
// $ Ftf.File
// > Filename:string=The filename of the ftf file to load
// > Width:number=The width of the font in pixels.
// > Height:number=The height of the font in pixels.
// > DPIWidth:integer=The DPI width of the font.
// > DPIHeight:integer=The DPI height of the font.
// > Outline:number=The pixels to add for a border.
// < Handle:Ftf=The Ftf object
// ? Loads a freetype compatible font from the specified file on disk for use
// ? with the Font.Create() function.
/* ------------------------------------------------------------------------- */
LLFUNC(File, 1,
  const AgFilename aFilename{lS, 1};
  const AgDimension aWidth{lS, 2},
                    aHeight{lS, 3};
  const AgDpiDimension aDpiWidth{lS, 4},
                       aDpiHeight{lS, 5};
  const AgOutline aOutline{lS, 6};
  AcFtf{lS}().FtfInitFile(aFilename, aWidth, aHeight, aDpiWidth, aDpiHeight,
    aOutline))
/* ========================================================================= */
// $ Ftf.FileAsync
// > Filename:string=The filename of the freetype font to load
// > Width:number=The width of the font in pixels.
// > Height:number=The height of the font in pixels.
// > DPIWidth:integer=The DPI width of the font.
// > DPIHeight:integer=The DPI height of the font.
// > Outline:number=The pixels to add for a border.
// > ErrorFunc:function=The function to call when there is an error
// > ProgressFunc:function=The function to call when there is progress
// > SuccessFunc:function=The function to call when the file is laoded
// ? Loads a freetype compatible font from the specified file on disk
// ? asynchronously for use with the Font.Create() function.
/* ------------------------------------------------------------------------- */
LLFUNC(FileAsync, 0,
  LuaUtilCheckParams(lS, 9);
  const AgFilename aFilename{lS, 1};
  const AgDimension aWidth{lS, 2},
                    aHeight{lS, 3};
  const AgDpiDimension aDpiWidth{lS, 4},
                       aDpiHeight{lS, 5};
  const AgOutline aOutline{lS, 6};
  LuaUtilCheckFunc(lS, 7, 8, 9);
  AcFtf{lS}().FtfInitAsyncFile(lS, aFilename, aWidth, aHeight, aDpiWidth,
    aDpiHeight, aOutline))
/* ========================================================================= */
// $ Ftf.GetClasses
// < Count:integer=Total number of ftfs created.
// ? Returns the total number of ftf classes currently active.
/* ------------------------------------------------------------------------- */
LLFUNC(GetClasses, 1, LuaUtilPushVar(lS, cFtfs->CollectorCount()))
/* ========================================================================= */
// $ Ftf.WaitAsync
// ? Halts main-thread execution until all async Ftf events have completed
/* ------------------------------------------------------------------------- */
LLFUNC(WaitAsync, 0, cFtfs->WaitAsync())
/* ========================================================================= **
** ######################################################################### **
** ## Ftf.* non-member functions structure                                ## **
** ######################################################################### **
** ========================================================================= */
LLRSBEGIN                              // Ftf.* non-member functions begin
  LLRSFUNC(Asset),     LLRSFUNC(AssetAsync), LLRSFUNC(File),
  LLRSFUNC(FileAsync), LLRSFUNC(GetClasses), LLRSFUNC(WaitAsync),
LLRSEND                                // Ftf.* non-member functions end
/* ========================================================================= */
}                                      // End of Ftf namespace
/* == EoF =========================================================== EoF == */
