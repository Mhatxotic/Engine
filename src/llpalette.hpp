/* == LLPALETTE.HPP ======================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## Defines the 'Palette' namespace and methods for the guest to use in ## **
** ## Lua. This file is invoked by 'lualib.hpp'.                          ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
#pragma once                           // Only one incursion allowed
/* ========================================================================= **
** ######################################################################### **
** ========================================================================= */
// % Palette
/* ------------------------------------------------------------------------- */
// ! The palette class allows creating and modifying palettes and uploading
// ! them to the GPU for use with 8-bit paletted textures. Only one palette can
// ! be active at once and textures marked with LF_PALETTE are affected by the
// ! palette.
/* ========================================================================= */
namespace LLPalette {                  // Palette namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IFboDef::P;            using namespace IImage::P;
using namespace IPalette::P;           using namespace IStd::P;
using namespace ITexture::P;           using namespace Common;
/* ========================================================================= **
** ######################################################################### **
** ## Palette common helper classes                                       ## **
** ######################################################################### **
** -- Read Palette class argument ------------------------------------------ */
struct AgPalette : public ArClass<Palette> {
  explicit AgPalette(lua_State*const lS, const int iArg) :
    ArClass{*LuaUtilGetPtr<Palette>(lS, iArg, *cPalettes)}{} };
/* -- Create Palette class argument ---------------------------------------- */
struct AcPalette : public ArClass<Palette> {
  explicit AcPalette(lua_State*const lS) :
    ArClass{*LuaUtilClassCreate<Palette>(lS, *cPalettes)}{} };
/* -- Read a valid Palette position index ---------------------------------- */
struct AgPosition : public AgSizeTLGE {
  explicit AgPosition(lua_State*const lS, const int iArg) :
    AgSizeTLGE{lS, iArg, 0, cPalettes->palDefault.size()}{} };
/* -- Other types ---------------------------------------------------------- */
typedef AgIntegerLEG<ssize_t> AgSSizeTLEG;
/* ========================================================================= **
** ######################################################################### **
** ## Palette:* member functions                                          ## **
** ######################################################################### **
** ========================================================================= */
// $ Palette:Commit
// ? Use this function to commit the new palette to the shader.
/* ------------------------------------------------------------------------- */
LLFUNC(Commit, 0, AgPalette{lS, 1}().Commit())
/* ========================================================================= */
// $ Palette:Fill
// > Id:integer=Palette entry to modify from (0-255).
// > Count:integer=The number of indexes to fill
// < Red:number=The red intensity number (0.0-1.0).
// < Green:number=The green intensity number (0.0-1.0).
// < Blue:number=The blue intensity number (0.0-1.0).
// < Alpha:number=The alpha intensity number (0.0-1.0).
// ? Use this function to fill the palette with the specified values.
/* ------------------------------------------------------------------------- */
LLFUNC(Fill, 0,
  const AgPalette aPalette{lS, 1};
  const AgPosition aColourId{lS, 2};
  const AgSizeTLG aCount{lS, 3, 0, cPalettes->palDefault.size() - aColourId};
  const AgGLfloat aRed{lS, 4},
                  aGreen{lS, 5},
                  aBlue{lS, 6},
                  aAlpha{lS, 7};
  aPalette().Fill(aColourId, aCount, aRed, aGreen, aBlue, aAlpha))
/* ========================================================================= */
// $ Palette:Copy
// > Count:integer=The number of values to copy.
// > DstId:integer=Index offset to copy the values to.
// > Src:Palette=The source palette to copy.
// > SrcId:integer=Index offset to copy the values from.
// ? Use this function to copy palette data from another palette.
/* ------------------------------------------------------------------------- */
LLFUNC(Copy, 0,
  const AgPalette aPaletteDestination{lS, 1};
  const AgPosition aCount{lS, 2};
  const size_t stMaxEnd = cPalettes->palDefault.size() - aCount;
  const AgSizeTLG aDstIndex{lS, 3, 0, stMaxEnd};
  const AgPalette aPaletteSource{lS, 4};
  const AgSizeTLG aSrcIndex{lS, 5, 0, stMaxEnd};
  aPaletteDestination().Copy(aDstIndex, aPaletteSource, aSrcIndex, aCount))
/* ========================================================================= */
// $ Palette:Destroy
// ? Destroys the palette and frees the memory associated with it. This does
// ? not affect the active palette as this is stored on the GPU.
/* ------------------------------------------------------------------------- */
LLFUNC(Destroy, 0, LuaUtilClassDestroy<Palette>(lS, 1, *cPalettes))
/* ========================================================================= */
// $ Palette:GetA
// > Id:integer=Palette entry to modify (0-255).
// < Alpha:number=The alpha intensity number (0.0-1.0).
// ? Use this function to get the alpha palette entry.
/* ------------------------------------------------------------------------- */
LLFUNC(GetA, 1,
  const AgPalette aPalette{lS, 1};
  const AgPosition aColourId{lS, 2};
  LuaUtilPushVar(lS, aPalette().GetAlpha(aColourId)))
/* ========================================================================= */
// $ Palette:GetB
// > Id:integer=Palette entry to modify (0-255).
// < Red:number=The red intensity number (0.0-1.0).
// ? Use this function to get the blue palette entry.
/* ------------------------------------------------------------------------- */
LLFUNC(GetB, 1,
  const AgPalette aPalette{lS, 1};
  const AgPosition aColourId{lS, 2};
  LuaUtilPushVar(lS, aPalette().GetBlue(aColourId)))
/* ========================================================================= */
// $ Palette:GetG
// > Id:integer=Palette entry to modify (0-255).
// < Green:number=The green intensity number (0.0-1.0).
// ? Use this function to get the green palette entry.
/* ------------------------------------------------------------------------- */
LLFUNC(GetG, 1,
  const AgPalette aPalette{lS, 1};
  const AgPosition aColourId{lS, 2};
  LuaUtilPushVar(lS, aPalette().GetGreen(aColourId)))
/* ========================================================================= */
// $ Palette:GetAI
// > Id:integer=Palette entry to modify (0-255).
// < Red:integer=The alpha intensity integer (0-255).
// ? Use this function to get the alpha palette entry. This function is mainly
// ? for convenience only and has slightly more overhead than with :GetA() as
// ? the palette values have to be stored as GLfloats in the GPU and thus extra
// ? calculations are required as a result.
/* ------------------------------------------------------------------------- */
LLFUNC(GetAI, 1,
  const AgPalette aPalette{lS, 1};
  const AgPosition aColourId{lS, 2};
  LuaUtilPushVar(lS, aPalette().GetAlphaInt(aColourId)))
/* ========================================================================= */
// $ Palette:GetBI
// > Id:integer=Palette entry to modify (0-255).
// < Blue:integer=The blue intensity integer (0-255).
// ? Use this function to get the blue palette entry. This function is mainly
// ? for convenience only and has slightly more overhead than with :GetB() as
// ? the palette values have to be stored as GLfloats in the GPU and thus extra
// ? calculations are required as a result.
/* ------------------------------------------------------------------------- */
LLFUNC(GetBI, 1,
  const AgPalette aPalette{lS, 1};
  const AgPosition aColourId{lS, 2};
  LuaUtilPushVar(lS, aPalette().GetBlueInt(aColourId)));
/* ========================================================================= */
// $ Palette:GetGI
// > Id:integer=Palette entry to modify (0-255).
// < Green:integer=The green intensity integer (0-255).
// ? Use this function to get the green palette entry. This function is mainly
// ? for convenience only and has slightly more overhead than with :GetG() as
// ? the palette values have to be stored as GLfloats in the GPU and thus extra
// ? calculations are required as a result.
/* ------------------------------------------------------------------------- */
LLFUNC(GetGI, 1,
  const AgPalette aPalette{lS, 1};
  const AgPosition aColourId{lS, 2};
  LuaUtilPushVar(lS, aPalette().GetGreenInt(aColourId)))
/* ========================================================================= */
// $ Palette:GetRI
// > Id:integer=Palette entry to modify (0-255).
// < Red:integer=The red intensity integer (0-255).
// ? Use this function to get the red palette entry. This function is mainly
// ? for convenience only and has slightly more overhead than with :GetR() as
// ? the palette values have to be stored as GLfloats in the GPU and thus extra
// ? calculations are required as a result.
/* ------------------------------------------------------------------------- */
LLFUNC(GetRI, 1,
  const AgPalette aPalette{lS, 1};
  const AgPosition aColourId{lS, 2};
  LuaUtilPushVar(lS, aPalette().GetRedInt(aColourId)))
/* ========================================================================= */
// $ Palette:GetRGBAI
// > Id:integer=Palette entry to modify (0-255).
// < Red:integer=The red intensity integer (0-255).
// < Green:integer=The green intensity integer (0-255).
// < Blue:integer=The blue intensity integer (0-255).
// < Alpha:integer=The alpha intensity integer (0-255).
// ? Use this function to get the palette entry.
/* ------------------------------------------------------------------------- */
LLFUNC(GetRGBAI, 4,
  const AgPalette aPalette{lS, 1};
  const AgPosition aColourId{lS, 2};
  const auto &fbocData =
    aPalette().GetSlotConst(aColourId).Cast<lua_Integer>();
  LuaUtilPushVar(lS, fbocData[0], fbocData[1], fbocData[2], fbocData[3]))
/* ========================================================================= */
// $ Palette:GetR
// > Id:integer=Palette entry to modify (0-255).
// < Red:number=The red intensity number (0.0-1.0).
// ? Use this function to get the red palette entry.
/* ------------------------------------------------------------------------- */
LLFUNC(GetR, 1,
  const AgPalette aPalette{lS, 1};
  const AgPosition aColourId{lS, 2};
  LuaUtilPushVar(lS, aPalette().GetRed(aColourId)))
/* ========================================================================= */
// $ Palette:GetRGBA
// > Id:integer=Palette entry to modify (0-255).
// < Red:number=The red intensity number (0.0-1.0).
// < Green:number=The green intensity number (0.0-1.0).
// < Blue:number=The blue intensity number (0.0-1.0).
// < Alpha:number=The alpha intensity number (0.0-1.0).
// ? Use this function to get the palette entry.
/* ------------------------------------------------------------------------- */
LLFUNC(GetRGBA, 4,
  const AgPalette aPalette{lS, 1};
  const AgPosition aColourId{lS, 2};
  const FboColour &fbocData = aPalette().GetSlotConst(aColourId);
  LuaUtilPushVar(lS, fbocData.GetColourRed(), fbocData.GetColourGreen(),
            fbocData.GetColourBlue(), fbocData.GetColourAlpha()))
/* ========================================================================= */
// $ Palette:GetId
// < Id:integer=The id number of the Palette object.
// ? Returns the unique id of the Palette object.
/* ------------------------------------------------------------------------- */
LLFUNC(GetId, 1, LuaUtilPushVar(lS, AgPalette{lS, 1}().CtrGet()))
/* ========================================================================= */
// $ Palette:GetName
// < Name:string=The identifier of the palette.
// ? Returns the palette identifier
/* ------------------------------------------------------------------------- */
LLFUNC(GetName, 1, LuaUtilPushVar(lS, AgPalette{lS, 1}().IdentGet()))
/* ========================================================================= */
// $ Palette:SetA
// > Id:integer=Palette entry to modify (0-255).
// > Alpha:number=The alpha intensity number (0.0-1.0).
// ? Use this function to set the alpha palette entry.
/* ------------------------------------------------------------------------- */
LLFUNC(SetA, 0,
  const AgPalette aPalette{lS, 1};
  const AgPosition aColourId{lS, 2};
  const AgGLfloat aColour{lS, 3};
  aPalette().SetAlpha(aColourId, aColour))
/* ========================================================================= */
// $ Palette:SetB
// > Id:integer=Palette entry to modify (0-255).
// > Blue:number=The blue intensity number (0.0-1.0).
// ? Use this function to set the blue palette entry.
/* ------------------------------------------------------------------------- */
LLFUNC(SetB, 0,
  const AgPalette aPalette{lS, 1};
  const AgPosition aColourId{lS, 2};
  const AgGLfloat aColour{lS, 3};
  aPalette().SetBlue(aColourId, aColour))
/* ========================================================================= */
// $ Palette:SetG
// > Id:integer=Palette entry to modify (0-255).
// > Green:number=The green intensity number (0.0-1.0).
// ? Use this function to set the green palette entry.
/* ------------------------------------------------------------------------- */
LLFUNC(SetG, 0,
  const AgPalette aPalette{lS, 1};
  const AgPosition aColourId{lS, 2};
  const AgGLfloat aColour{lS, 3};
  aPalette().SetGreen(aColourId, aColour))
/* ========================================================================= */
// $ Palette:SetAI
// > Id:integer=Palette entry to modify (0-255).
// > Alpha:integer=The alpha intensity integer (0-255).
// ? Use this function to set the alpha palette entry.
/* ------------------------------------------------------------------------- */
LLFUNC(SetAI, 0,
  const AgPalette aPalette{lS, 1};
  const AgPosition aColourId{lS, 2};
  const AgUIntLG aColour{lS, 3, 0, 255};
  aPalette().SetAlphaInt(aColourId, aColour))
/* ========================================================================= */
// $ Palette:SetBI
// > Id:integer=Palette entry to modify (0-255).
// > Blue:integer=The blue intensity integer (0-255).
// ? Use this function to set the blue palette entry.
/* ------------------------------------------------------------------------- */
LLFUNC(SetBI, 0,
  const AgPalette aPalette{lS, 1};
  const AgPosition aColourId{lS, 2};
  const AgUIntLG aColour{lS, 3, 0, 255};
  aPalette().SetBlueInt(aColourId, aColour))
/* ========================================================================= */
// $ Palette:SetGI
// > Id:integer=Palette entry to modify (0-255).
// > Green:integer=The green intensity integer (0-255).
// ? Use this function to set the green palette entry.
/* ------------------------------------------------------------------------- */
LLFUNC(SetGI, 0,
  const AgPalette aPalette{lS, 1};
  const AgPosition aColourId{lS, 2};
  const AgUIntLG aColour{lS, 3, 0, 255};
  aPalette().SetGreenInt(aColourId, aColour))
/* ========================================================================= */
// $ Palette:SetRI
// > Id:integer=Palette entry to modify (0-255).
// > Red:integer=The red intensity integer (0-255).
// ? Use this function to set the red palette entry.
/* ------------------------------------------------------------------------- */
LLFUNC(SetRI, 0,
  const AgPalette aPalette{lS, 1};
  const AgPosition aColourId{lS, 2};
  const AgUIntLG aColour{lS, 3, 0, 255};
  aPalette().SetRedInt(aColourId, aColour))
/* ========================================================================= */
// $ Palette:SetRGBAI
// > Id:integer=Palette entry to modify (0-255).
// > Red:integer=The red intensity integer (0-255).
// > Green:integer=The green intensity integer (0-255).
// > Blue:integer=The blue intensity integer (0-255).
// > Alpha:integer=The alpha intensity integer (0-255).
// ? Use this function to set the palette entry using integers. This function
// ? is mainly for convenience only and has slightly more overhead than with
// ? :Set() as the palette values have to be stored as GLfloats in the GPU and
// ? thus extra calculations are required as a result.
/* ------------------------------------------------------------------------- */
LLFUNC(SetRGBAI, 0,
  const AgPalette aPalette{lS, 1};
  const AgPosition aColourId{lS, 2};
  const AgUIntLG aRed{lS, 3, 0, 255},
                 aGreen{lS, 4, 0, 255},
                 aBlue{lS, 5, 0, 255},
                 aAlpha{lS, 6, 0, 255};
  aPalette().SetRGBAInt(aColourId, aRed, aGreen, aBlue, aAlpha))
/* ========================================================================= */
// $ Palette:SetR
// > Id:integer=Palette entry to modify (0-255).
// > Red:number=The red intensity number (0.0-1.0).
// ? Use this function to set the red palette entry.
/* ------------------------------------------------------------------------- */
LLFUNC(SetR, 0,
  const AgPalette aPalette{lS, 1};
  const AgPosition aColourId{lS, 2};
  const AgGLfloat aColour{lS, 3};
  aPalette().SetRed(aColourId, aColour))
/* ========================================================================= */
// $ Palette:SetRGBA
// > Id:integer=Palette entry to modify (0-255).
// > Red:number=The red intensity number (0.0-1.0).
// > Green:number=The green intensity number (0.0-1.0).
// > Blue:number=The blue intensity number (0.0-1.0).
// > Alpha:number=The alpha intensity number (0.0-1.0).
// ? Use this function to set the palette entry.
/* ------------------------------------------------------------------------- */
LLFUNC(SetRGBA, 0,
  const AgPalette aPalette{lS, 1};
  const AgPosition aColourId{lS, 2};
  const AgGLfloat aRed{lS, 3},
                  aGreen{lS, 4},
                  aBlue{lS, 5},
                  aAlpha{lS, 6};
  aPalette().SetRGBA(aColourId, aRed, aGreen, aBlue, aAlpha))
/* ========================================================================= */
// $ Palette:Shift
// > Begin:integer=The starting palette index to shift up to.
// > End:integer=The ending palette index to shift up to.
// > Amount:integer=Amount to rotate by.
// ? Shifts all palette entries backwards or forwards this amount from the
// ? specified palette index and limited to the specified number of indexes.
/* ------------------------------------------------------------------------- */
LLFUNC(Shift, 0,
  const AgPalette aPalette{lS, 1};
  const AgSSizeTLG aBegin{lS, 2, 0, aPalette().Size()};
  const AgSSizeTLGE aEnd{lS, 3, 0, aPalette().Size() - aBegin};
  const AgSSizeTLEG aAmount{lS, 4, aPalette().SizeN(), aPalette().Size()};
  aPalette().Shift(aBegin, aEnd, aAmount))
/* ========================================================================= */
// $ Palette:ShiftB
// > Begin:integer=The starting palette index to shift up to.
// > End:integer=The ending palette index to shift up to.
// > Amount:integer=Amount to rotate backwards by.
// ? Shifts all palette entries backwards this amount from the specified
// ? palette index and limited to the specified number of indexes.
/* ------------------------------------------------------------------------- */
LLFUNC(ShiftB, 0,
  const AgPalette aPalette{lS, 1};
  const AgSSizeTLGE aBegin{lS, 2, 0, aPalette().Size()},
                    aEnd{lS, 3, aBegin, aPalette().Size()};
  const AgSSizeTLG aAmount{lS, 4, 0, aPalette().Size() - aBegin};
  aPalette().ShiftBck(aBegin, aEnd, aAmount))
/* ========================================================================= */
// $ Palette:ShiftF
// > Begin:integer=The starting palette index to shift up to.
// > End:integer=The ending palette index to shift up to.
// > Amount:integer=Amount to rotate forwards by.
// ? Shifts all palette entries forwards this amount from the specified
// ? palette index and limited to the specified number of indexes.
/* ------------------------------------------------------------------------- */
LLFUNC(ShiftF, 0,
  const AgPalette aPalette{lS, 1};
  const AgSSizeTLGE aBegin{lS, 2, 0, aPalette().Size()},
                    aEnd{lS, 3, aBegin, aPalette().Size()};
  const AgSSizeTLG aAmount{lS, 4, 0, aPalette().Size() - aBegin};
  aPalette().ShiftFwd(aBegin, aEnd, aAmount))
/* ========================================================================= **
** ######################################################################### **
** ## Palette:* member functions structure                                ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
LLRSMFBEGIN                            // Palette:* member functions begin
  LLRSFUNC(Commit),   LLRSFUNC(Copy),    LLRSFUNC(Destroy),  LLRSFUNC(Fill),
  LLRSFUNC(GetA),     LLRSFUNC(GetAI),   LLRSFUNC(GetB),     LLRSFUNC(GetBI),
  LLRSFUNC(GetG),     LLRSFUNC(GetGI),   LLRSFUNC(GetId),    LLRSFUNC(GetName),
  LLRSFUNC(GetR),     LLRSFUNC(GetRGBA), LLRSFUNC(GetRGBAI), LLRSFUNC(GetRI),
  LLRSFUNC(SetA),     LLRSFUNC(SetAI),   LLRSFUNC(SetB),     LLRSFUNC(SetBI),
  LLRSFUNC(SetG),     LLRSFUNC(SetGI),   LLRSFUNC(SetR),     LLRSFUNC(SetRGBA),
  LLRSFUNC(SetRGBAI), LLRSFUNC(SetRI),   LLRSFUNC(Shift),    LLRSFUNC(ShiftB),
  LLRSFUNC(ShiftF),
LLRSEND                                // Palette:* member functions end
/* ========================================================================= */
// $ Palette.Create
// > Identifier:string=Reference only user-defined identifier.
// < Handle:Palette=A handle to the newly created palette object.
// ? Creates a new empty palette with the specified name
/* ------------------------------------------------------------------------- */
LLFUNC(Create, 1,
  const AgNeString aIdentifier{lS, 1};
  AcPalette{lS}().Init(aIdentifier))
/* ========================================================================= */
// $ Palette.Default
// > Identifier:string=Reference only user-defined identifier.
// < Handle:Palette=A handle to the newly created palette object.
// ? Creates a new palette with the default VGA palette.
/* ------------------------------------------------------------------------- */
LLFUNC(Default, 1,
  const AgNeString aIdentifier{lS, 1};
  AcPalette{lS}().Init(aIdentifier, cPalettes->palDefault))
/* ========================================================================= */
// $ Palette.Image
// > Identifier:string=Reference only user-defined identifier.
// > Handle:Image=Handle to image to grab palette from.
// ? Creates a new palette from the specified image.
/* ------------------------------------------------------------------------- */
LLFUNC(Image, 1,
  const AgNeString aIdentifier{lS, 1};
  const AgImage aImage{lS,2};
  AcPalette{lS}().Init(aIdentifier, aImage))
/* ========================================================================= */
// $ Palette.Palette
// > Identifier:string=Reference only user-defined identifier.
// > Handle:Palette=A handle to the palette object to copy.
// < Handle:Palette=A handle to the newly created palette object.
// ? Creates a new palette from another essentially copying it.
/* ------------------------------------------------------------------------- */
LLFUNC(Palette, 1,
  const AgNeString aIdentifier{lS, 1};
  const AgPalette aSourcePalette{lS, 2};
  AcPalette{lS}().Init(aIdentifier, aSourcePalette))
/* ========================================================================= */
// $ Palette.Texture
// > Identifier:string=Reference only user-defined identifier.
// > Handle:Texture=Handle to texture to grab palette from.
// ? Creates a new palette from the specified texture.
/* ------------------------------------------------------------------------- */
LLFUNC(Texture, 1,
  const AgNeString aIdentifier{lS, 1};
  const AgTexture aTexture{lS, 2};
  AcPalette{lS}().Init(aIdentifier, aTexture))
/* ========================================================================= **
** ######################################################################### **
** ## Palette.* namespace functions structure                             ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
LLRSBEGIN                              // Palette.* namespace functions begin
  LLRSFUNC(Create),  LLRSFUNC(Default), LLRSFUNC(Image), LLRSFUNC(Palette),
  LLRSFUNC(Texture),
LLRSEND                                // Palette.* namespace functions end
/* ========================================================================= */
}                                      // End of Palette namespace
/* == EoF =========================================================== EoF == */
