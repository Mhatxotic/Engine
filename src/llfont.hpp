/* == LLFONT.HPP =========================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## Defines the 'Font' namespace and methods for the guest to use in    ## **
** ## Lua. This file is invoked by 'lualib.hpp'.                          ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ========================================================================= **
** ######################################################################### **
** ========================================================================= */
// % Font
/* ------------------------------------------------------------------------- */
// ! The font class allows dynamic creation from font files using the FREETYPE
// ! library.
/* ========================================================================= */
namespace LLFont {                     // Font namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IConGraph::P;          using namespace IConsole::P;
using namespace IFont::P;              using namespace IFtf::P;
using namespace IImage::P;             using namespace IImageDef::P;
using namespace IJson::P;              using namespace IOgl::P;
using namespace ITexture::P;           using namespace Common;
/* ========================================================================= **
** ######################################################################### **
** ## Font common helper classes                                          ## **
** ######################################################################### **
** -- Read Font class argument --------------------------------------------- */
struct AgFont : public ArClass<Font> {
  explicit AgFont(lua_State*const lS, const int iArg) :
    ArClass{*LuaUtilGetPtr<Font>(lS, iArg, *cFonts)}{} };
/* -- Create Font class argument ------------------------------------------- */
struct AcFont : public ArClass<Font> {
  explicit AcFont(lua_State*const lS) :
    ArClass{*LuaUtilClassCreate<Font>(lS, *cFonts)}{} };
/* -- Get a positive number value ------------------------------------------ */
struct AgPositive : public AgNumberL<GLfloat> {
  explicit AgPositive(lua_State*const lS, const int iArg) :
    AgNumberL{ lS, iArg, 0.0f }{} };
/* -- Read Font flags argument --------------------------------------------- */
struct AgFontFlags : public AgFlags<ImageFlagsConst> {
  explicit AgFontFlags(lua_State*const lS, const int iArg) :
    AgFlags{ lS, iArg, FF_MASK }{} };
/* -- Read vector of the specified number type ----------------------------- */
template<class VectorValueType, class VectorType = vector<VectorValueType>>
  struct AgNumberVector : public VectorType
{ const VectorType &operator()(void) const { return *this; }
  operator const VectorType&(void) const { return operator()(); }
  explicit AgNumberVector(lua_State*const lS, const int iArg) :
    VectorType{ LuaUtilToNumVector<VectorType>(lS, iArg)}{} };
/* -- Other types ---------------------------------------------------------- */
typedef AgCString<GLubyte> AgGLString;
typedef AgIntegerLG<GLuint> AgGLuintLG;
/* ========================================================================= **
** ######################################################################### **
** ## Font:* member functions                                             ## **
** ######################################################################### **
** ========================================================================= */
// $ Font:Destroy
// ? Destroys the font and frees all the memory associated with it. The OpenGL
// ? handles and VRAM allocated by it are freed after the main FBO has has been
// ? rendered. The object will no longer be useable after this call and an
// ? error will be generated if accessed.
/* ------------------------------------------------------------------------- */
LLFUNC(Destroy, 0, LuaUtilClassDestroy<Font>(lS, 1, *cFonts))
/* ========================================================================= */
// $ Font:Dump
// > TexId:integer=The texture id to dump.
// > File:stream=The filename of the stream to write to.
// ? Dumps the font tileset to the specified file.
/* ------------------------------------------------------------------------- */
LLFUNC(Dump, 0,
  const AgFont aFont{lS, 1};
  const AgTextureId aTextureId{lS, 2, aFont};
  const AgFilename aFilename{lS, 3};
  aFont().Dump(aTextureId, aFilename))
/* ========================================================================= */
// $ Font:GetHeight
// < Height:integer=The tile height of the font.
// ? Returns height of the font tile. If this font is a free-type font, this
// ? will just be the height of the white-space character only.
/* ------------------------------------------------------------------------- */
LLFUNC(GetHeight, 1,
  LuaUtilPushVar(lS, AgFont{lS, 1}().duiTile.DimGetHeight()))
/* ========================================================================= */
// $ Font:GetId
// < Id:integer=The id number of the Font object.
// ? Returns the unique id of the Font object.
/* ------------------------------------------------------------------------- */
LLFUNC(GetId, 1, LuaUtilPushVar(lS, AgFont{lS, 1}().CtrGet()))
/* ========================================================================= */
// $ Font:GetName
// < Name:string=Name of the font.
// ? If this font was loaded by a filename or it was set with a custom id.
// ? This function returns that name which was assigned to it.
/* ------------------------------------------------------------------------- */
LLFUNC(GetName, 1, LuaUtilPushVar(lS, AgFont{lS, 1}().IdentGet()))
/* ========================================================================= */
// $ Font:GetWidth
// < Width:integer=The tile width of the font.
// ? Returns width of the font tile. If this font is a free-type font, this
// ? will just be the width of the white-space character only.
/* ------------------------------------------------------------------------- */
LLFUNC(GetWidth, 1, LuaUtilPushVar(lS, AgFont{lS, 1}().duiTile.DimGetWidth()))
/* ========================================================================= */
// $ Font:LoadChars
// > Characters:string=A utf-8 string of characters you want to pre-cache.
// ? Caches all the characters in the specified utf-8 character range to the
// ? texture. Although new characters are dynamically loaded on demand, this
// ? function will act as a 'pre-cache' for characters you KNOW you are going
// ? to use.
/* ------------------------------------------------------------------------- */
LLFUNC(LoadChars, 0,
  const AgFont aFont{lS, 1};
  const AgGLString aString{lS, 2};
  aFont().InitFTCharString(aString))
/* ========================================================================= */
// $ Font:LoadRange
// > Start:integer=The starting UNICODE character index.
// > End:integer=The ending UNICODE character index.
// ? Caches the specified UNICODE character range to the texture. Although new
// ? characters are dynamically loaded on demand, this function will act as a
// ? 'pre-cache' for characters you KNOW you are going to use.
/* ------------------------------------------------------------------------- */
LLFUNC(LoadRange, 0,
  const AgFont aFont{lS, 1};
  const AgSizeT aStart{lS, 2},
                aEnd{lS, 3};
  aFont().InitFTCharRange(aStart, aEnd))
/* ========================================================================= */
// $ Font:Print
// > X:number=The X screen position of the string.
// > Y:number=The Y screen position of the string.
// > String:string=The string to print.
// ? Prints the specified string on screen with left alignment.
/* ------------------------------------------------------------------------- */
LLFUNC(Print, 0,
  const AgFont aFont{lS, 1};
  const AgGLfloat aX{lS, 2},
                  aY{lS, 3};
  const AgGLString aString{lS, 4};
  aFont().Print(aX, aY, aString))
/* ========================================================================= */
// $ Font:PrintC
// > X:number=The X screen position of the string.
// > Y:number=The Y screen position of the string.
// > Text:string=The string to print.
// ? Prints the specified string on screen with centre alignment.
/* ------------------------------------------------------------------------- */
LLFUNC(PrintC, 0,
  const AgFont aFont{lS, 1};
  const AgGLfloat aX{lS, 2},
                  aY{lS, 3};
  const AgGLString aString{lS, 4};
  aFont().PrintC(aX, aY, aString))
/* ========================================================================= */
// $ Font:PrintCT
// > X:number=The X position of where to start printing the string
// > Y:number=The Y position of the string.
// > Text:string=The string to print.
// > Glyphs:Texture=The texture handle to use for printing glyphs.
// ? Prints the specified string on screen with centre alignment and glyphs.
/* ------------------------------------------------------------------------- */
LLFUNC(PrintCT, 0,
  const AgFont aFont{lS, 1};
  const AgGLfloat aX{lS, 2},
                  aY{lS, 3};
  const AgGLString aString{lS, 4};
  const AgTexture aTexture{lS, 5};
  aFont().PrintCT(aX, aY, aString, aTexture))
/* ========================================================================= */
// $ Font:PrintM
// > X:number=The X position of where to draw the string.
// > Y:number=The Y position of where to draw the string.
// > Scroll:number=The amount to scroll leftwards by in pixels.
// > Width:number=The width of the string to print.
// > Text:string=The string to print
// ? Prints part of a string which helps one create a marquee effect.
/* ------------------------------------------------------------------------- */
LLFUNC(PrintM, 0,
  const AgFont aFont{lS, 1};
  const AgGLfloat aX{lS, 2},
                  aY{lS, 3};
  const AgGLfloat aScroll{lS, 4};
  const AgPositive aWidth{lS, 5};
  const AgGLString aString{lS, 6};
  aFont().PrintM(aX, aY, aScroll, aWidth, aString))
/* ========================================================================= */
// $ Font:PrintMT
// > X:number=The X position of where to draw the string.
// > Y:number=The Y position of where to draw the string.
// > Scroll:number=The amount to scroll leftwards by in pixels.
// > Width:number=The width of the string to print.
// > Text:string=The string to print
// > Glphs:Texture=The texture to use to printing glyph
// ? Prints part of a string which helps one create a marquee effect with
// ? glyphs support.
/* ------------------------------------------------------------------------- */
LLFUNC(PrintMT, 0,
  const AgFont aFont{lS, 1};
  const AgGLfloat aX{lS, 2},
                  aY{lS, 3};
  const AgGLfloat aScroll{lS, 4};
  const AgPositive aWidth{lS, 5};
  const AgGLString aString{lS, 6};
  const AgTexture aTexture{lS, 7};
  aFont().PrintMT(aX, aY, aScroll, aWidth, aString, aTexture))
/* ========================================================================= */
// $ Font:PrintS
// > Text:string=The string to simulate printing.
// < Width:Number=The width of the text in pixels.
// ? Simluates printing of the specified string on screen with left alignment
// ? and returns the width of the string printed
/* ------------------------------------------------------------------------- */
LLFUNC(PrintS, 1,
  const AgFont aFont{lS, 1};
  const AgGLString aString{lS, 2};
  LuaUtilPushVar(lS, aFont().PrintS(aString)))
/* ========================================================================= */
// $ Font:PrintTS
// > Text:string=The string to simulate printing.
// > Glyphs:Texture=The glyph textures to use
// < Width:Number=The width of the text in pixels.
// ? Simluates printing of the specified string on screen with left alignment
// ? and returns the width of the string printed
/* ------------------------------------------------------------------------- */
LLFUNC(PrintTS, 1,
  const AgFont aFont{lS, 1};
  const AgGLString aString{lS, 2};
  const AgTexture aTexture{lS, 3};
  LuaUtilPushVar(lS, aFont().PrintTS(aString, aTexture)))
/* ========================================================================= */
// $ Font:PrintW
// > X:number=The X position of where to start printing the string
// > Y:number=The Y screen position of the string.
// > Right:number=The right position of where to end printing the string
// > Indent:number=The width of the indentation on wrapping.
// > Text:string=The string to print.
// < Height:number=The height of the text printed on screen.
// ? Prints the specified string on screen with left alignment. While printing,
// ? the text will cleanly wrap if it cannot draw any word past the specified
// ? 'Right' position and position the cursor at the specified 'Indent'.
/* ------------------------------------------------------------------------- */
LLFUNC(PrintW, 1,
  const AgFont aFont{lS, 1};
  const AgGLfloat aX{lS, 2},
                  aY{lS, 3};
  const AgPositive aWidth{lS, 4},
                   aIndent{lS, 5};
  const AgGLString aString{lS, 6};
  LuaUtilPushVar(lS, aFont().PrintW(aX, aY, aWidth, aIndent, aString)))
/* ========================================================================= */
// $ Font:PrintR
// > X:number=The X screen position of the string.
// > Y:number=The Y screen position of the string.
// > Text:string=The string to print.
// ? Prints the specified string on screen with right alignment.
/* ------------------------------------------------------------------------- */
LLFUNC(PrintR, 0,
  const AgFont aFont{lS, 1};
  const AgGLfloat aX{lS, 2},
                  aY{lS, 3};
  const AgGLString aString{lS, 4};
  aFont().PrintR(aX, aY, aString))
/* ========================================================================= */
// $ Font:PrintRT
// > X:number=The X position of where to start printing the string
// > Y:number=The Y position of the string.
// > Text:string=The string to print.
// > Glyphs:Texture=The texture handle to use for printing glyphs.
// ? Same functionality as PrintR but with the option to print glyphs. You
// ? can do this by specifying \rthhhhhhhh anywhere in your text. Where 'h'
// ? is a hexadecimal number between 00000000 and ffffffff to match the tile
// ? id of the texture. You must zero pad the number as shown.
/* ------------------------------------------------------------------------- */
LLFUNC(PrintRT, 0,
  const AgFont aFont{lS, 1};
  const AgGLfloat aX{lS, 2},
                  aY{lS, 3};
  const AgGLString aString{lS, 4};
  const AgTexture aTexture{lS, 5};
  aFont().PrintRT(aX, aY, aString, aTexture))
/* ========================================================================= */
// $ Font:PrintT
// > X:number=The X position of where to start printing the string
// > Y:number=The Y position of the string.
// > Text:string=The string to print.
// > Glyphs:Texture=The texture handle to use for printing glyphs.
// ? Same functionality as Print but with the option to print glyphs. You
// ? can do this by specifying \rthhhhhhhh anywhere in your text. Where 'h'
// ? is a hexadecimal number between 00000000 and ffffffff to match the tile
// ? id of the texture. You must zero pad the number as shown.
/* ------------------------------------------------------------------------- */
LLFUNC(PrintT, 0,
  const AgFont aFont{lS, 1};
  const AgGLfloat aX{lS, 2},
                  aY{lS, 3};
  const AgGLString aString{lS, 4};
  const AgTexture aTexture{lS, 5};
  aFont().PrintT(aX, aY, aString, aTexture))
/* ========================================================================= */
// $ Font:PrintU
// > X:number=The X screen position of the string.
// > Y:number=The Y screen position of the string.
// > String:string=The string to print.
// < Height:number=The height of the text printed on screen.
// ? Prints the specified string on screen with bottom-left alignment.
// ? Equivalent of calling Print() with the result of PrintUS() subtracted from
// ? the Y position.
/* ------------------------------------------------------------------------- */
LLFUNC(PrintU, 0,
  const AgFont aFont{lS, 1};
  const AgGLfloat aX{lS, 2},
                  aY{lS, 3};
  const AgGLString aString{lS, 4};
  aFont().PrintU(aX, aY, aString))
/* ========================================================================= */
// $ Font:PrintUC
// > X:number=The X screen position of the string.
// > Y:number=The Y screen position of the string.
// > String:string=The string to print.
// < Height:number=The height of the text printed on screen.
// ? Prints the specified string on screen with vertical bottom-left vertical
// ? and horizontal centre alignment. Equivalent of calling PrintC() with the
// ? result of PrintUS() subtracted from Y position.
/* ------------------------------------------------------------------------- */
LLFUNC(PrintUC, 0,
  const AgFont aFont{lS, 1};
  const AgGLfloat aX{lS, 2},
                  aY{lS, 3};
  const AgGLString aString{lS, 4};
  aFont().PrintUC(aX, aY, aString))
/* ========================================================================= */
// $ Font:PrintUCT
// > X:number=The X screen position of the string.
// > Y:number=The Y screen position of the string.
// > String:string=The string to print.
// > Glyphs:Texture=The texture handle to use for printing glyphs.
// < Height:number=The height of the text printed on screen.
// ? Prints the specified string on screen with vertical bottom-left vertical
// ? and horizontal centre alignment. Equivalent of calling PrintCT() with the
// ? result of PrintUS() subtracted from Y position.
/* ------------------------------------------------------------------------- */
LLFUNC(PrintUCT, 0,
  const AgFont aFont{lS, 1};
  const AgGLfloat aX{lS, 2},
                  aY{lS, 3};
  const AgGLString aString{lS, 4};
  const AgTexture aTexture{lS, 5};
  aFont().PrintUCT(aX, aY, aString, aTexture))
/* ========================================================================= */
// $ Font:PrintUR
// > X:number=The X screen position of the string.
// > Y:number=The Y screen position of the string.
// > String:string=The string to print.
// < Height:number=The height of the text printed on screen.
// ? Prints the specified string on screen with vertical bottom-left vertical
// ? and horizontal right alignment. Equivalent of calling PrintR() with the
// ? result of PrintUS() subtracted from Y position.
/* ------------------------------------------------------------------------- */
LLFUNC(PrintUR, 0,
  const AgFont aFont{lS, 1};
  const AgGLfloat aX{lS, 2},
                  aY{lS, 3};
  const AgGLString aString{lS, 4};
  aFont().PrintUR(aX, aY, aString))
/* ========================================================================= */
// $ Font:PrintURT
// > X:number=The X screen position of the string.
// > Y:number=The Y screen position of the string.
// > String:string=The string to print.
// > Glyphs:Texture=The texture handle to use for printing glyphs.
// < Height:number=The height of the text printed on screen.
// ? Prints the specified string on screen with vertical bottom-left vertical
// ? and horizontal centre alignment. Equivalent of calling PrintCT() with the
// ? result of PrintUS() subtracted from Y position.
/* ------------------------------------------------------------------------- */
LLFUNC(PrintURT, 0,
  const AgFont aFont{lS, 1};
  const AgGLfloat aX{lS, 2},
                  aY{lS, 3};
  const AgGLString aString{lS, 4};
  const AgTexture aTexture{lS, 5};
  aFont().PrintURT(aX, aY, aString, aTexture))
/* ========================================================================= */
// $ Font:PrintUS
// > String:string=The string to print.
// < Height:number=The height of the text printed on screen.
// ? Simulates printing a string. Returns height.
/* ------------------------------------------------------------------------- */
LLFUNC(PrintUS, 1,
  const AgFont aFont{lS, 1};
  const AgGLString aString{lS, 2};
  LuaUtilPushVar(lS, aFont().PrintSU(aString)))
/* ========================================================================= */
// $ Font:PrintWS
// > WrapX:number=The X position to wrap the text at.
// > Indent:number=The width of the indentation on wrapping.
// > String:string=The string to print.
// < Height:number=The height of the text printed on screen.
// ? Simulates printing a string with word-wrap. Returns height.
/* ------------------------------------------------------------------------- */
LLFUNC(PrintWS, 1,
  const AgFont aFont{lS, 1};
  const AgPositive aWidth{lS, 2},
                   aIndent{lS, 3};
  const AgGLString aString{lS, 4};
  LuaUtilPushVar(lS, aFont().PrintWS(aWidth, aIndent, aString)))
/* ========================================================================= */
// $ Font:PrintWT
// > X:number=The X position of where to start printing the string
// > Y:number=The Y position of the string.
// > Right:number=The right position of where to wrap text to the next line.
// > Indent:number=The width of the indentation on wrapping.
// > Text:string=The string to print.
// > Glyphs:texture=The texture handle to use for printing glyphs.
// ? Same functionality as PrintW but with the option to print glyphs. You
// ? can do this by specifying \rthhhhhhhh anywhere in your text. Where 'h'
// ? is a hexadecimal number between 00000000 and ffffffff to match the tile
// ? id of the texture. You must zero pad the number as shown.
/* ------------------------------------------------------------------------- */
LLFUNC(PrintWT, 0,
  const AgFont aFont{lS, 1};
  const AgGLfloat aX{lS, 2},
                  aY{lS, 3};
  const AgPositive aWidth{lS, 4},
                   aIndent{lS, 5};
  const AgGLString aString{lS, 6};
  const AgTexture aTexture{lS, 7};
  aFont().PrintWT(aX, aY, aWidth, aIndent, aString, aTexture))
/* ========================================================================= */
// $ Font:PrintWTS
// > Width:number=The maximum width of a line before wrapping to the next.
// > Indent:number=The width of the indentation on wrapping.
// > Text:string=The string to print.
// > Glyphs:texture=The texture handle to use for printing glyphs.
// ? Same functionality as PrintW but with the option to print glyphs. You
// ? can do this by specifying \rthhhhhhhh anywhere in your text. Where 'h'
// ? is a hexadecimal number between 00000000 and ffffffff to match the tile
// ? id of the texture. You must zero pad the number as shown.
/* ------------------------------------------------------------------------- */
LLFUNC(PrintWTS, 1,
  const AgFont aFont{lS, 1};
  const AgPositive aWidth{lS, 2},
                   aIndent{lS, 3};
  const AgGLString aString{lS, 4};
  const AgTexture aTexture{lS, 5};
  LuaUtilPushVar(lS, aFont().PrintWTS(aWidth, aIndent, aString, aTexture)))
/* ========================================================================= */
// $ Font:PrintWU
// > X:number=The X screen position of the string.
// > Y:number=The Y screen position of the string.
// > Width:number=The X+Width to wrap at.
// > Indent:number=The width of the indentation on wrapping.
// > String:string=The string to print.
// < Height:number=The height of the text printed on screen.
// ? Prints the specified string on screen with left alignment. While printing,
// ? if X exceeds the specified Width, the text is wrapped with the specified
// ? indent size on the following line.
/* ------------------------------------------------------------------------- */
LLFUNC(PrintWU, 0,
  const AgFont aFont{lS, 1};
  const AgGLfloat aX{lS, 2},
                  aY{lS, 3};
  const AgPositive aWidth{lS, 4},
                   aIndent{lS, 5};
  const AgGLString aString{lS, 6};
  aFont().PrintWU(aX, aY, aWidth, aIndent, aString))
/* ========================================================================= */
// $ Font:PrintWUT
// > X:number=The X position of where to start printing the string
// > Y:number=The Y position of the string.
// > Right:number=The right position of where to wrap text to the next line.
// > Indent:number=The width of the indentation on wrapping.
// > Text:string=The string to print.
// > Glyphs:texture=The texture handle to use for printing glyphs.
// ? Same functionality as PrintWU but with the option to print glyphs. You
// ? can do this by specifying \rthhhhhhhh anywhere in your text. Where 'h'
// ? is a hexadecimal number between 00000000 and ffffffff to match the tile
// ? id of the texture. You must zero pad the number as shown.
/* ------------------------------------------------------------------------- */
LLFUNC(PrintWUT, 0,
  const AgFont aFont{lS, 1};
  const AgGLfloat aX{lS, 2},
                  aY{lS, 3};
  const AgPositive aWidth{lS, 4},
                   aIndent{lS, 5};
  const AgGLString aString{lS, 6};
  const AgTexture aTexture{lS, 7};
  aFont().PrintWUT(aX, aY, aWidth, aIndent, aString, aTexture))
/* ========================================================================= */
// $ Font:SetCA
// > Alpha:number=The transparency of the texture (0-1).
// ? Sets the colour transparency of the texture. The change affects all
// ? subsequent calls to all blitting functions.
/* ------------------------------------------------------------------------- */
LLFUNC(SetCA, 0,
  const AgFont aFont{lS, 1};
  const AgGLfloat aColour{lS, 2};
  aFont().FboItemSetQuadAlpha(aColour))
/* ========================================================================= */
// $ Font:SetCB
// > Blue:number=The colour intensity of the texture's blue component (0-1).
// ? Sets the colour intensity of the texture for the blue component. The
// ? change affects all subsequent calls to all blitting functions.
/* ------------------------------------------------------------------------- */
LLFUNC(SetCB, 0,
  const AgFont aFont{lS, 1};
  const AgGLfloat aColour{lS, 2};
  aFont().FboItemSetQuadBlue(aColour))
/* ========================================================================= */
// $ Font:SetCG
// > Green:number=The colour intensity of the texture's green component (0-1).
// ? Sets the colour intensity of the texture for the green component. The
// ? change affects all subsequent calls to all blitting functions.
/* ------------------------------------------------------------------------- */
LLFUNC(SetCG, 0,
  const AgFont aFont{lS, 1};
  const AgGLfloat aColour{lS, 2};
  aFont().FboItemSetQuadGreen(aColour))
/* ========================================================================= */
// $ Font:SetCOA
// > Alpha:number=The transparency of the outline texture (0-1).
// ? Sets the colour transparency of the outline texture. The change affects
// ? all subsequent calls to all blitting functions.
/* ------------------------------------------------------------------------- */
LLFUNC(SetCOA, 0,
  const AgFont aFont{lS, 1};
  const AgGLfloat aColour{lS, 2};
  aFont().fiOutline.FboItemSetQuadAlpha(aColour))
/* ========================================================================= */
// $ Font:SetCOB
// > Blue:number=The colour intensity of the blue outline component (0-1).
// ? Sets the colour intensity of the outline for the blue component. The
// ? change affects all subsequent calls to all blitting functions.
/* ------------------------------------------------------------------------- */
LLFUNC(SetCOB, 0,
  const AgFont aFont{lS, 1};
  const AgGLfloat aColour{lS, 2};
  aFont().fiOutline.FboItemSetQuadBlue(aColour))
/* ========================================================================= */
// $ Font:SetCOG
// > Green:number=The colour intensity of green outline component (0-1).
// ? Sets the colour intensity of the outline for the green component. The
// ? change affects all subsequent calls to all blitting functions.
/* ------------------------------------------------------------------------- */
LLFUNC(SetCOG, 0,
  const AgFont aFont{lS, 1};
  const AgGLfloat aColour{lS, 2};
  aFont().fiOutline.FboItemSetQuadGreen(aColour))
/* ========================================================================= */
// $ Font:SetCOR
// > Red:number=The colour intensity of red outline component (0-1).
// ? Sets the colour intensity of the outline for the red component. The change
// ? affects all subsequent calls to all blitting functions.
/* ------------------------------------------------------------------------- */
LLFUNC(SetCOR, 0,
  const AgFont aFont{lS, 1};
  const AgGLfloat aColour{lS, 2};
  aFont().fiOutline.FboItemSetQuadRed(aColour))
/* ========================================================================= */
// $ Font:SetCORGB
// > Red:number=The colour intensity of the outline red component (0-1).
// > Green:number=The colour intensity of the outline green component (0-1).
// > Blue:number=The colour intensity of the outline blue omponent (0-1).
// ? Sets the colour intensity of the outline for each component. The change
// ? affects all subsequent calls to all blitting functions.
/* ------------------------------------------------------------------------- */
LLFUNC(SetCORGB, 0,
  const AgFont aFont{lS, 1};
  const AgGLfloat aRed{lS, 2},
                  aGreen{lS, 3},
                  aBlue{lS, 4};
  aFont().fiOutline.FboItemSetQuadRGB(aRed, aGreen, aBlue))
/* ========================================================================= */
// $ Font:SetCORGBA
// > Red:number=The colour intensity of the outline red component (0-1).
// > Green:number=The colour intensity of the outline green component (0-1).
// > Blue:number=The colour intensity of the outline blue omponent (0-1).
// > Alpha:number=The transparency of the outline (0-1).
// ? Sets the colour intensity of the outline for each component. The change
// ? affects all subsequent calls to all blitting functions.
/* ------------------------------------------------------------------------- */
LLFUNC(SetCORGBA, 0,
  const AgFont aFont{lS, 1};
  const AgGLfloat aRed{lS, 2},
                  aGreen{lS, 3},
                  aBlue{lS, 4},
                  aAlpha{lS, 5};
  aFont().fiOutline.FboItemSetQuadRGBA(aRed, aGreen, aBlue, aAlpha))
/* ========================================================================= */
// $ Font:SetCORGBAI
// > Colour:integer=The entire colour to set as an integer (0xAARRGGBB)
// ? Sets the colour intensity of the font outline for each component using a
// ? 32 bit integer.
/* ------------------------------------------------------------------------- */
LLFUNC(SetCORGBAI, 0,
  const AgFont aFont{lS, 1};
  const AgUInt32 aColour{lS, 2};
  aFont().fiOutline.FboItemSetQuadRGBAInt(aColour))
/* ========================================================================= */
// $ Font:SetGlyphPadding
// > Padding:number=The top side Y padding to add to glyphs
// ? When using print functions with glyph texture addons, this controls the Y
// ? padding.
/* ------------------------------------------------------------------------- */
LLFUNC(SetGPad, 0,
  const AgFont aFont{lS, 1};
  const AgGLfloat aPadding{lS, 2};
  aFont().SetGlyphPadding(aPadding))
/* ========================================================================= */
// $ Font:SetGSize
// > Scale:number=The new scale of the font glyphs.
// ? Changes the scale of the font glyphs.
/* ------------------------------------------------------------------------- */
LLFUNC(SetGSize, 0,
  const AgFont aFont{lS, 1};
  const AgPositive aScale{lS, 2};
  aFont().SetGlyphSize(aScale))
/* ========================================================================= */
// $ Font:SetLSpacing
// > Adjust:number=The new line spacing adjustment
// ? Changes the line spacing adjustment
/* ------------------------------------------------------------------------- */
LLFUNC(SetLSpacing, 0,
  const AgFont aFont{lS, 1};
  const AgGLfloat aSpacing{lS, 2};
  aFont().SetLineSpacing(aSpacing))
/* ========================================================================= */
// $ Font:SetCR
// > Red:number=The colour intensity of the texture's red component (0-1).
// ? Sets the colour intensity of the texture for the red component. The change
// ? affects all subsequent calls to all blitting functions.
/* ------------------------------------------------------------------------- */
LLFUNC(SetCR, 0,
  const AgFont aFont{lS, 1};
  const AgGLfloat aColour{lS, 2};
  aFont().FboItemSetQuadRed(aColour))
/* ========================================================================= */
// $ Font:SetCRGB
// > Red:number=The colour intensity of the texture's red component (0-1).
// > Green:number=The colour intensity of the texture's green component (0-1).
// > Blue:number=The colour intensity of the texture's blue omponent (0-1).
// ? Sets the colour intensity of the texture for each component. The change
// ? affects all subsequent calls to all blitting functions.
/* ------------------------------------------------------------------------- */
LLFUNC(SetCRGB, 0,
  const AgFont aFont{lS, 1};
  const AgGLfloat aRed{lS, 2},
                  aGreen{lS, 3},
                  aBlue{lS, 4};
  aFont().FboItemSetQuadRGB(aRed, aGreen, aBlue))
/* ========================================================================= */
// $ Font:SetCRGBA
// > Red:number=The colour intensity of the texture's red component (0-1).
// > Green:number=The colour intensity of the texture's green component (0-1).
// > Blue:number=The colour intensity of the texture's blue omponent (0-1).
// > Alpha:number=The transparency of the texture (0-1).
// ? Sets the colour intensity of the texture for each component. The change
// ? affects all subsequent calls to all blitting functions.
/* ------------------------------------------------------------------------- */
LLFUNC(SetCRGBA, 0,
  const AgFont aFont{lS, 1};
  const AgGLfloat aRed{lS, 2},
                  aGreen{lS, 3},
                  aBlue{lS, 4},
                  aAlpha{lS, 5};
  aFont().FboItemSetQuadRGBA(aRed, aGreen, aBlue, aAlpha))
/* ========================================================================= */
// $ Font:SetCRGBAI
// > Colour:integer=The entire colour to set as an integer (0xAARRGGBB)
// ? Sets the colour intensity of the font face for each component using a 32
// ? bit integer.
/* ------------------------------------------------------------------------- */
LLFUNC(SetCRGBAI, 0,
  const AgFont aFont{lS, 1};
  const AgUInt32 aColour{lS, 2};
  aFont().FboItemSetQuadRGBAInt(aColour))
/* ========================================================================= */
// $ Font:SetSize
// > Scale:number=The new scale of the font.
// ? Changes the scale of the font.
/* ------------------------------------------------------------------------- */
LLFUNC(SetSize, 0,
  const AgFont aFont{lS, 1};
  const AgPositive aScale{lS, 2};
  aFont().SetSize(aScale))
/* ========================================================================= */
// $ Font:SetSpacing
// > Adjust:number=The new character spacing adjustment
// ? Changes the character spacing adjustment
/* ------------------------------------------------------------------------- */
LLFUNC(SetSpacing, 0,
  const AgFont aFont{lS, 1};
  const AgGLfloat aSpacing{lS, 2};
  aFont().SetCharSpacing(aSpacing))
/* ========================================================================= **
** ######################################################################### **
** ## Font:* member functions structure                                   ## **
** ######################################################################### **
** ========================================================================= */
LLRSMFBEGIN                            // Font:* member functions begin
  LLRSFUNC(Destroy),    LLRSFUNC(Dump),        LLRSFUNC(GetHeight),
  LLRSFUNC(GetId),      LLRSFUNC(GetName),     LLRSFUNC(GetWidth),
  LLRSFUNC(LoadChars),  LLRSFUNC(LoadRange),   LLRSFUNC(Print),
  LLRSFUNC(PrintC),     LLRSFUNC(PrintCT),     LLRSFUNC(PrintM),
  LLRSFUNC(PrintMT),    LLRSFUNC(PrintR),      LLRSFUNC(PrintRT),
  LLRSFUNC(PrintS),     LLRSFUNC(PrintT),      LLRSFUNC(PrintTS),
  LLRSFUNC(PrintU),     LLRSFUNC(PrintUC),     LLRSFUNC(PrintUCT),
  LLRSFUNC(PrintUR),    LLRSFUNC(PrintURT),    LLRSFUNC(PrintUS),
  LLRSFUNC(PrintW),     LLRSFUNC(PrintWS),     LLRSFUNC(PrintWT),
  LLRSFUNC(PrintWTS),   LLRSFUNC(PrintWU),     LLRSFUNC(PrintWUT),
  LLRSFUNC(SetCA),      LLRSFUNC(SetCB),       LLRSFUNC(SetCG),
  LLRSFUNC(SetCOA),     LLRSFUNC(SetCOB),      LLRSFUNC(SetCOG),
  LLRSFUNC(SetCOR),     LLRSFUNC(SetCORGB),    LLRSFUNC(SetCORGBA),
  LLRSFUNC(SetCORGBAI), LLRSFUNC(SetCR),       LLRSFUNC(SetCRGBAI),
  LLRSFUNC(SetCRGB),    LLRSFUNC(SetCRGBA),    LLRSFUNC(SetGPad),
  LLRSFUNC(SetGSize),   LLRSFUNC(SetLSpacing), LLRSFUNC(SetSize),
  LLRSFUNC(SetSpacing),
LLRSEND                                // Font:* member functions end
/* ========================================================================= **
** ######################################################################### **
** ## Font.* namespace functions                                          ## **
** ######################################################################### **
** ========================================================================= */
// $ Font.Console
// < Handle:Font=Font handle to console texture
// ? Returns the handle to the console font. Useful if you want to reuse the
// ? font in your application. Careful not to mess around with it's properties!
/* ------------------------------------------------------------------------- */
LLFUNC(Console, 1,
  LuaUtilClassCreatePtr<Font>(lS, *cFonts, cConGraphics->GetFont()))
/* ========================================================================= */
// $ Font.Count
// < Count:integer=Total number of fonts created.
// ? Returns the total number of font classes currently active.
/* ------------------------------------------------------------------------- */
LLFUNC(Count, 1, LuaUtilPushVar(lS, cFonts->CollectorCount()))
/* ========================================================================= */
// $ Font.Ftf
// > Font:Ftf=An ftf object of a loaded freetype font.
// > Size:integer=The initial canvas size of the texture.
// > Padding:integer=Amount of padding to use to prevent texture spilling.
// > Filter:integer=Font texture filtering mode to use.
// > Flags:integer=Font flags.
// < Handle:Font=A texture handle to the font.
// ? Creates a texture from the specified font using FreeType. Note that the
// ? ftf object invalidated externally as the new font object will assume
// ? private ownership of it.
/* ========================================================================= */
LLFUNC(Ftf, 1,
  const AgFtf aFtf{lS, 1};
  const AgGLuintLG aSize{lS, 2, 0, cOgl->MaxTexSize()},
                   aPadding{lS, 3, 0, 16};
  const AgFilterId aFilterId{lS, 4};
  const AgFontFlags aFlags{lS, 5};
  AcFont{lS}().InitFontFtf(aFtf, aSize, aPadding, aFilterId, aFlags))
/* ========================================================================= */
// $ Font.Image
// > Source:Image=The source image for use with the font.
// > TileWidth:integer=The maximum width of each font glyph.
// > TileHeight:integer=The maximum height of each font glyph.
// > PaddingWidth:integer=Pixels to ignore on the right of each glpyh.
// > PaddingHeight:integer=Pixels to ignore on below each glpyh.
// > FilterId:integer=The filter id to use.
// > CharCount:integer=Number of characters available in the font.
// > CharStart:integer=The starting UTF8 character id to begin with.
// > CharDefault:integer=The default character for unsupported characters.
// > Widths:table=Pixel widths of each glyph. Empty to set all to TileWidth.
// < Handle:Font=A handle to the newly created texture.
// ? Creates a texture as a font tileset.
/* ========================================================================= */
LLFUNC(Image, 1,
  const AgImage aImage{lS, 1};
  const AgGLuintLG aTileWidth{lS, 2, 0, cOgl->MaxTexSize()},
                   aTileHeight{lS, 3, 0, cOgl->MaxTexSize()},
                   aPaddingWidth{lS, 4, 0, cOgl->MaxTexSize()},
                   aPaddingHeight{lS, 5, 0, cOgl->MaxTexSize()};
  const AgFilterId aFilterId{lS, 6};
  const AgSizeTL aCharCount{lS, 7, 1},
                 aCharBegin{lS, 8, 0};
  const AgSizeTLGE aCharDef{lS, 9, aCharBegin(), aCharBegin() + aCharCount()};
  const AgNumberVector<GLfloat> aWidths{lS, 10};
  AcFont{lS}().InitFontImage(aImage, aTileWidth(), aTileHeight(),
    aPaddingWidth(), aPaddingHeight(), aFilterId(), aCharCount(), aCharBegin(),
    aCharDef(),
    aWidths))
/* ========================================================================= */
// $ Font.Manifest
// > Source:Image=The source image for use with the font.
// > Manifest:JSon=The manifest of information to describe the font.
// < Handle:Font=A handle to the newly created texture.
// ? Creates a texture as a font tileset using values from the specified
// ? manifest. These parameters in the root object are as follows...
// ?
// ? Version:UInt = The current version of the manifest (1).
// ? Default:UInt = The default UTF8 character to draw if char not available.
// ? CharCount:UInt = The number of UTF8 characters in the tileset.
// ? CharBegin:UInt = The UTF8 character to represent the first tile.
// ? InitialScale:Number = The starting scale of the chars (0.0=0%><1.0=100%).
// ? TileWidth:UInt = The width of the tile.
// ? TileHeight:UInt = The height of the tile.
// ? TileSpacingHeight:UInt = The vertical spacing between each tile.
// ? TileSpacingWidth:UInt = The horizontal spacing between each tile.
// ? Widths:Array = The widths of each character from value 'rangestart'
/* ========================================================================= */
LLFUNC(Manifest, 1,
  const AgImage aImage{lS, 1};
  const AgJson aJson{lS, 2};
  AcFont{lS}().InitFontImageManifest(aImage, aJson))
/* ========================================================================= **
** ######################################################################### **
** ## Font.* namespace functions structure                                ## **
** ######################################################################### **
** ========================================================================= */
LLRSBEGIN                              // Font.* namespace functions begin
  LLRSFUNC(Console), LLRSFUNC(Count), LLRSFUNC(Ftf), LLRSFUNC(Image),
  LLRSFUNC(Manifest),
LLRSEND                                // Font.* namespace functions end
/* ========================================================================= **
** ######################################################################### **
** ## Font.* namespace constants                                          ## **
** ######################################################################### **
** ========================================================================= */
// @ Font.Flags
// < Codes:table=The table of key/value pairs of available flags for use with
// ? the InitFTFont function.
/* ------------------------------------------------------------------------- */
LLRSKTBEGIN(Flags)                     // Beginning of ft font loading flags
  LLRSKTITEM(IL_,NONE),                LLRSKTITEM(FF_,USEGLYPHSIZE),
  LLRSKTITEM(FF_,FLOORADVANCE),        LLRSKTITEM(FF_,CEILADVANCE),
  LLRSKTITEM(FF_,ROUNDADVANCE),        LLRSKTITEM(FF_,STROKETYPE2),
LLRSKTEND                              // End of ft font loading flags
/* ========================================================================= **
** ######################################################################### **
** ## Font.* namespace constants structure                                ## **
** ######################################################################### **
** ========================================================================= */
LLRSCONSTBEGIN                         // Font.* namespace consts begin
  LLRSCONST(Flags),
LLRSCONSTEND                           // Font.* namespace consts end
/* ========================================================================= */
}                                      // End of Font namespace
/* == EoF =========================================================== EoF == */
