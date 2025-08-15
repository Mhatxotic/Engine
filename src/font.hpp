/* == FONT.HPP ============================================================= **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## This module allows loading and drawing of beautifully rendered      ## **
** ## fonts using the FreeType library. There is a lot of code in this    ## **
** ## class so it is split apart into different font*.hpp files and will  ## **
** ## be included inline and the rest of the code in this file is related ## **
** ## to initialisation of the actual font and public metadata retrieval. ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IFont {                      // Start of private namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IAsset::P;             using namespace IAtlas::P;
using namespace ICollector::P;         using namespace IDim::P;
using namespace IError::P;             using namespace IFileMap::P;
using namespace IFreeType::P;          using namespace IFtf::P;
using namespace IImageDef::P;          using namespace IJson::P;
using namespace ILog::P;               using namespace ILuaIdent::P;
using namespace ILuaLib::P;            using namespace IMemory::P;
using namespace IOgl::P;               using namespace IRectangle::P;
using namespace IStd::P;               using namespace ISysUtil::P;
using namespace ITexDef::P;            using namespace ITexture::P;
using namespace IUtf::P;               using namespace IUtil::P;
using namespace Lib::FreeType;         using namespace Lib::OS::GlFW::Types;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public namespace
/* == Font collector class for collector data and custom variables ========= */
CTOR_BEGIN_NOBB(Fonts, Font, CLHelperUnsafe)
/* == Font Variables Class ================================================= */
// Only put vars used in the Font class in here. This is an optimisation so
// we do not have to initialise all these variables more than once as we have
// more than one constructor in the main Font class.
/* ------------------------------------------------------------------------- */
class FontBase :                       // Members initially private
  /* -- Base classes ------------------------------------------------------- */
  public Atlas                         // Atlas class
{ /* -- Protected typedefs -------------------------------------- */ protected:
  class Glyph :                        // Members initially private
    /* -- Dependencies ----------------------------------------------------- */
    public DimGLFloat,                 // Dimension of floats
    public RectFloat                   // Glyph bounding co-ordinates
  { /* --------------------------------------------------------------------- */
    bool           bLoaded;            // 0=ft unloaded or 1=ft loaded
    GLfloat        fAdvance;           // Width, height and advance of glyph
    /* -- Returns character advance -------------------------------- */ public:
    GLfloat GlyphGetAdvance(void) const { return fAdvance; }
    /* -- Sets character advance ------------------------------------------- */
    void GlyphSetAdvance(const GLfloat fNewAdv) { fAdvance = fNewAdv; }
    /* -- Returns if this glyph has been loaded ---------------------------- */
    bool GlyphIsLoaded(void) const { return bLoaded; }
    /* -- Set glyph as loaded ---------------------------------------------- */
    void GlyphSetLoaded(void) { bLoaded = true; }
    /* -- Constructor that initialises all members ------------------------- */
    Glyph(const GLfloat fWidth,        // Width of glyph in pixels for opengl
          const GLfloat fHeight,       // Height of glyph in pixels for opengl
          const bool    bNLoaded,      // Is the glyph loaded?
          const GLfloat fNAdvance,     // Specified glyph advance in pixels
          const GLfloat fX1,           // Top-left co-ordinate of glyph
          const GLfloat fY1,           // Bottom-left co-ordinate of glyph
          const GLfloat fX2,           // Top-right co-ordinate of glyph
          const GLfloat fY2) :         // Bottom-right co-ordinate of glyph
      /* -- Initialisers --------------------------------------------------- */
      DimGLFloat{ fWidth, fHeight },   // Initialise glpyh size
      RectFloat{ fX1, fY1, fX2, fY2 }, // Init adjustment co-ordinates
      bLoaded(bNLoaded),               // Init specified loaded value
      fAdvance(fNAdvance)              // Init specified advance value
      /* -- No code -------------------------------------------------------- */
      { }
    /* -- Default Constructor ---------------------------------------------- */
    Glyph(void) :                      // No arguments
      /* -- Initialisers --------------------------------------------------- */
      bLoaded(false),                  // Character not loaded yet
      fAdvance(0.0f)                   // Character advance value
      /* -- No code -------------------------------------------------------- */
      { }
  };/* -- Variables -------------------------------------------------------- */
  typedef vector<Glyph>         GlyphVector;   // Vector of Glyphs
  typedef GlyphVector::iterator GlyphVectorIt; // Iterator to GlyphVector
  /* --------------------------------------------------------------- */ public:
  GlyphVector      gvData;             // Glyph and outline data
  DimGLFloat       dfScale,            // Scaled font width and height
                   dfFont;             // Requested font size for OpenGL
  size_t           stMultiplier;       // 1 if no outline, 2 if outline
  GLfloat          fCharSpacing,       // Character spacing adjustment
                   fCharSpacingScale,  // Character spacing scaled size
                   fLineSpacing,       // Text line spacing
                   fLineSpacingHeight, // Height of line spacing plus height
                   fGSize,             // Scaled font texture glyph size
                   fScale,             // Font scale
                   fGPad, fGPadScaled; // Glyph position adjustment
  Texture         *tGlyphs;            // Texture for print calls only
  FboItem          fiOutline;          // Outline colour
  /* -- Freetype handles --------------------------------------------------- */
  Ftf              ftfData;            // FT font data
  FT_ULong         ulDefaultChar;      // Default fallback character
  /* -- Default constructor ------------------------------------------------ */
  explicit FontBase(const ImageFlagsConst ifcPurpose = IP_FONT) :
    /* --------------------------------------------------------------------- */
    Atlas{ ifcPurpose },               stMultiplier(1),
    fCharSpacing(0.0f),                fCharSpacingScale(0.0f),
    fLineSpacing(0.0f),                fLineSpacingHeight(0.0f),
    fGSize(0.0f),                      fScale(0.0f),
    fGPad(0.0f),                       fGPadScaled(0.0f),
    tGlyphs(nullptr),                  fiOutline{ 0xFF000000 },
    ulDefaultChar('?')
    /* --------------------------------------------------------------------- */
    { }
};/* ----------------------------------------------------------------------- */
/* == Font Class (which inherits an Atlas) ==========-====================== */
CTOR_MEM_BEGIN(Fonts, Font, ICHelperUnsafe, /* n/a */),
  /* -- Base classes ------------------------------------------------------- */
  public FontBase                      // Font variables class
{ /* -- Stroker functors for DoInitFTChar() -------------------------------- */
#include "fontglph.hpp"                // Include glyph loading members inline
#include "fontblit.hpp"                // Include glyph blitting members inline
  /* -- Set glyph size --------------------------------------------- */ public:
  void SetGlyphSize(const GLfloat fSize)
    { fGSize = fSize; UpdateGlyphPaddingTimesScale(); }
  /* -- Get character count ------------------------------------------------ */
  size_t GetCharCount(void) const { return gvData.size() / stMultiplier; }
  /* -- Get character scale ------------------------------------------------ */
  GLfloat GetCharScale(void) const { return fScale; }
  /* -- Get bin occupancy -------------------------------------------------- */
  double GetTexOccupancy(void) const { return ipData.Occupancy(); }
  /* -- Get below baseline height of specified character ------------------- */
  GLfloat GetBaselineBelow(const unsigned int uiChar)
  { // Now get character info and return data
    const Glyph &gRef = gvData[CheckGlyph(uiChar)];
    return (gRef.RectGetY2() - gRef.DimGetHeight()) * fScale;
  }
  /* -- Set line spacing of the font --------------------------------------- */
  void SetLineSpacing(const GLfloat fNewLineSpacing)
  { // Set line spacing
    fLineSpacing = fNewLineSpacing;
    // Update line spacing plus height
    UpdateHeightPlusLineSpacing();
  }
  /* -- Set new glyph Y padding and the new scaling value ------------------ */
  void SetGlyphPadding(const GLfloat fNewGPad)
  { // Set new glyph padding
    fGPad = fNewGPad;
    // Update glyph padding times scale
    UpdateGlyphPaddingTimesScale();
  }
  /* -- Set character spacing of the font ---------------------------------- */
  void SetCharSpacing(const GLfloat fNewCharSpacing)
  { // Update character spacing value
    fCharSpacing = fNewCharSpacing;
    // Update new character spacing times scale value
    UpdateCharSpacingTimesScale();
  }
  /* -- Set size of the font ----------------------------------------------- */
  void SetSize(const GLfloat fNScale)
  { // Set scale
    fScale = UtilClamp(fNScale, 0.0f, 1024.0f);
    // Update scaled dimensions
    dfScale.DimSet(GetTileWidthFloat() * fScale,
                   GetTileHeightFloat() * fScale);
    // Update glyph size
    SetGlyphSize(dfScale.DimGetHeight());
    // Update frequently used values
    UpdateCharSpacingTimesScale();
    UpdateHeightPlusLineSpacing();
  }
  /* -- Do initialise specified freetype character range ------------------- */
  void InitFTCharRange(const size_t stStart, const size_t stEnd)
  { // Ignore if not a freetype font.
    if(!ftfData.IsLoaded()) return;
    // Log pre-cache progress
    cLog->LogDebugExSafe("Font '$' pre-caching character range $ to $...",
      IdentGet(), stStart, stEnd);
    // Load the specified character range
    DoInitFTCharRangeApplyStroker<HandleGlyphFunc::FreeType>(stStart, stEnd);
    // Check if any textures need reloading
    AtlasCheckReloadTexture();
    // Log success
    cLog->LogDebugExSafe(
      "Font '$' finished pre-caching character range $ to $.",
      IdentGet(), stStart, stEnd);
  }
  /* -- Do initialise all freetype characters in specified string ---------- */
  void InitFTCharString(const GLubyte*const ucpPtr)
  { // Ignore if string not valid or font not loaded
    if(UtfIsCStringNotValid(ucpPtr) || !ftfData.IsLoaded()) return;
    // Do load string characters
    DoInitFTCharStringApplyStroker<HandleGlyphFunc::FreeType>(ucpPtr);
    // Check if any textures need reloading
    AtlasCheckReloadTexture();
  }
  /* -- Do initialise freetype font ---------------------------------------- */
  void InitFontFtf(Ftf &_ftfData, const GLuint uiISize,
    const GLuint _uiPadding, const OglFilterEnum ofeNFilter,
    const ImageFlagsConst &ifcFlags)
  { // Show that we're loading the file
    cLog->LogDebugExSafe("Font loading '$' (IS:$;P:$;F:$;FL:$$)...",
      _ftfData.IdentGet(), uiISize, _uiPadding, ofeNFilter, hex,
      ifcFlags.FlagGet());
    // If source and destination ftf class are not the same?
    if(&ftfData != &_ftfData)
    { // Assign freetype font data
      ftfData.SwapFtf(_ftfData);
      // The ftf passed in the arguments is usually still allocated by LUA
      // and will still be registered, so lets put a note in the image to show
      // that this function has nicked this font class.
      _ftfData.IdentSetEx("!FON!$!", ftfData.IdentGet());
    } // Initialise load flags
    FlagSet(ifcFlags);
    SetFontFreeType();
    // Outline characters will be stored one after the opaque characters
    // thus doubling the size of the glyph data/texture coord lists. The
    // multiplier is already 1 by default so this just makes it equal 2.
    if(ftfData.IsOutline()) stMultiplier = 2;
    // Initialise the atlas texture
    AtlasInit<ImageTypeGrayAlpha>(ftfData.IdentGet(),
      static_cast<GLuint>(ceil(ftfData.DimGetWidth())),
      static_cast<GLuint>(ceil(ftfData.DimGetHeight())),
      uiISize, _uiPadding, ofeNFilter);
    // Set requested font size for opengl
    dfFont.DimSet(ftfData);
    // Set default scaled font size and line spacing adjust
    SetSize(1.0f);
    fLineSpacingHeight = ftfData.DimGetHeight();
    // Update tile size as GLfloat for opengl
    dfTile.DimSet(GetTileWidth<GLfloat>(), GetTileHeight<GLfloat>());
    // Make enough space for initial tex coords set
    clTiles.resize(1);
    // Show that we've loaded the file
    cLog->LogInfoExSafe("Font '$' loaded FT font (IS:$;P:$;F:$;FL:$$)...",
      ftfData.IdentGet(), uiISize, uiPadding, ofeFilter, hex,
      ifcFlags.FlagGet());
  }
  /* -- Init a pre-rendered font directly ---------------------------------- */
  void InitFontImage(Image &imSrc, const GLuint uiTWidth,
    const GLuint uiTHeight, const GLuint uiPWidth, const GLuint uiPHeight,
    const OglFilterEnum ofeNFilter, const size_t stCharCount,
    const size_t stCharBegin, const size_t stCharDefault,
    const GLFloatVector &fvWidths)
  { // Show filename
    cLog->LogDebugExSafe("Font loading bitmap '$'...", imSrc.IdentGet());
    // Set Font's role as a Bitmap font.
    SetFontBitmap();
    // Set ending character
    const size_t stCharEnd = stCharBegin + stCharCount;
    // Set default character
    ulDefaultChar = static_cast<decltype(ulDefaultChar)>(stCharDefault);
    // Set filter
    ofeFilter = ofeNFilter;
    // Convert tile width and height to float
    const GLfloat fW = static_cast<GLfloat>(uiTWidth),
                  fH = static_cast<GLfloat>(uiTHeight);
    // If no values were specified?
    if(fvWidths.empty())
    { // Add the starting unused characters
      gvData.resize(stCharBegin);
      // Add all the characters to the default with automatically
      gvData.resize(stCharEnd, { fW, fH, true, fW, 0.0f, 0.0f, fW, fH });
    } // Error if we do not have the correct number of items
    else if(fvWidths.size() != stCharCount)
      XC("Unexpected number of widths specified!",
         "Identifier", imSrc.IdentGet(), "Required", stCharCount,
         "Actual",     fvWidths.size());
    // We have the correct number of items
    else
    { // Reserve memory for all the requested character space
      gvData.reserve(stCharEnd);
      // Add the starting unused characters
      gvData.resize(stCharBegin);
      // Set all the specified widths in the widths array
      transform(fvWidths.cbegin(), fvWidths.cend(), back_inserter(gvData),
        [fW, fH](const GLfloat fvWidth)->const Glyph{
          return { fW, fH, true, fvWidth, 0.0f, 0.0f, fW, fH };
        });
    }
    // Calculate beginning character minus one
    const size_t stCharBeginM1 = stCharBegin - 1;
    // Initialise the left uninitialised widths with the default character
    const Glyph &gRef = gvData[stCharDefault];
    GlyphVectorIt gviLeftEnd{ gvData.begin() };
    advance(gviLeftEnd, stCharBeginM1);
    StdFill(par_unseq, gvData.begin(), gviLeftEnd, gRef);
    // Initialise the right uninitialised widths with the default character
    GlyphVectorIt gviRightBegin{ gvData.begin() };
    advance(gviRightBegin, stCharEnd);
    StdFill(par_unseq, gviRightBegin, gvData.end(), gRef);
    // Initialise memory for texture tile co-ordinates
    clTiles.resize(1);
    CoordList &clFirst = clTiles.front();
    clFirst.reserve(stCharEnd);
    clFirst.resize(stCharBegin);
    // Init texture with custom parameters and generate tileset
    InitTextureImage(imSrc, uiTWidth, uiTHeight, uiPWidth, uiPHeight,
      ofeFilter);
    // Initialise the left uninitialised texcoords with the default character
    const CoordData &cdRef = clFirst[stCharDefault];
    CoordListIt cliItLeftEnd{ clFirst.begin() };
    advance(cliItLeftEnd, stCharBeginM1);
    StdFill(par_unseq, clFirst.begin(), cliItLeftEnd, cdRef);
    // Initialise the right uninitialised texcoords with the default character
    CoordListIt cliItRightBegin{ clFirst.begin() };
    advance(cliItRightBegin, stCharEnd);
    StdFill(par_unseq, cliItRightBegin, clFirst.end(), cdRef);
    // Initialise default font scale and line spacing to height
    SetSize(1.0f);
    fLineSpacingHeight = fH;
    // Show that we've loaded the file
    cLog->LogInfoExSafe("Font '$' loaded from bitmap (T:$x$;F:$).",
      IdentGet(), uiTWidth, uiTHeight, ofeFilter);
  }
  /* -- Init a pre-rendered font with manifest ----------------------------- */
  void InitFontImageManifest(Image &imSrc, const Json &jsDoc)
  { // Show filename
    cLog->LogDebugExSafe("Font loading bitmap '$' with manifest '$'...",
      imSrc.IdentGet(), jsDoc.IdentGet());
    // Set Font's role as a Bitmap font.
    SetFontBitmap();
    // Make sure correct version
    const unsigned int uiVersionRequired = 1;
    const unsigned int uiVersion = jsDoc.GetInteger("Version");
    if(uiVersion != uiVersionRequired)
      XC("Invalid version in manifest!",
         "Identifier", imSrc.IdentGet(),  "Manfiest", jsDoc.IdentGet(),
         "Required",   uiVersionRequired, "Actual",   uiVersion);
    // Number of characters supported cannot be zero!
    const size_t stCharCount = jsDoc.GetInteger("CharCount");
    if(stCharCount < 1)
      XC("Invalid character count in manifest!",
         "Identifier", imSrc.IdentGet(), "Manfiest", jsDoc.IdentGet(),
         "Value",      stCharCount);
    // Get beginning glyph id and calculate ending id
    const size_t stCharBegin = jsDoc.GetInteger("CharBegin"),
                 stCharEnd = stCharBegin + stCharCount;
    // Get and check default character
    // Check if it's a literal number as prefixed by a hash sign? Convert as a
    // literal number else convert as an a literal character then verify it is
    // in the specified range.
    ulDefaultChar = jsDoc.GetInteger("Default");
    if(ulDefaultChar < stCharBegin || ulDefaultChar >= stCharEnd)
      XC("Default character index out of range in manifest!",
         "Identifier", imSrc.IdentGet(), "Manfiest", jsDoc.IdentGet(),
         "Default",    ulDefaultChar,    "Minimum",  stCharBegin,
         "Maximum",    stCharEnd,        "Count",    stCharCount);
    // Get filter
    ofeFilter = static_cast<decltype(ofeFilter)>(jsDoc.GetInteger("Filter"));
    if(ofeFilter >= OF_MAX)
      XC("Invalid filter index specified in manifest!",
         "Identifier", imSrc.IdentGet(), "Manfiest", jsDoc.IdentGet(),
         "Filter",     ofeFilter);
    // Look for widths and throw if there are none then report them in log
    using Lib::RapidJson::Value;
    const Value &rjvWidths = jsDoc.GetValue("Widths");
    if(!rjvWidths.IsArray())
      XC("Widths array not valid!",
         "Identifier", imSrc.IdentGet(), "Manfiest", jsDoc.IdentGet());
    if(rjvWidths.Size() != stCharCount)
      XC("Unexpected number of widths specified in manifest!",
         "Identifier", imSrc.IdentGet(), "Manfiest", jsDoc.IdentGet(),
         "Expect",     stCharCount,      "Actual",   rjvWidths.Size());
    // Reserve memory for all the requested character space
    gvData.reserve(stCharEnd);
    // Add the starting unused characters
    gvData.resize(stCharBegin);
    // Read size of tile. Texture init will clamp this if needed.
    const GLuint uiTW = jsDoc.GetInteger("TileWidth"),
                 uiTH = jsDoc.GetInteger("TileHeight");
    if(uiTW > cOgl->MaxTexSize() || uiTH > cOgl->MaxTexSize())
      XC("Invalid tile dimensions in manifest!",
         "Identifier", imSrc.IdentGet(), "Manfiest", jsDoc.IdentGet(),
         "Width",      uiTW,             "Height",   uiTH,
         "Maximum",    cOgl->MaxTexSize());
    // Convert to float as we need a float version of this in the next loop
    const GLfloat fW = static_cast<GLfloat>(uiTW),
                  fH = static_cast<GLfloat>(uiTH);
    // Add the characters the manifest file cares about
    transform(rjvWidths.Begin(), rjvWidths.End(), back_inserter(gvData),
      [&imSrc, &jsDoc, fW, fH](const Value &rjvItem)->const Glyph{
        if(rjvItem.IsUint())
          return { fW, fH, true, static_cast<GLfloat>(rjvItem.GetUint()),
            0.0f, 0.0f, fW, fH };
        XC("Invalid element type from widths array in manifest!",
           "Identifier", imSrc.IdentGet(), "Manfiest", jsDoc.IdentGet());
      });
    // Calculate the beginning character minus one
    const size_t stCharBeginM1 = stCharBegin - 1;
    // Get the default character width data
    const Glyph &gRef = gvData[ulDefaultChar];
    // Initialise the left uninitialised glyph widths
    GlyphVectorIt gviLeftEnd{ gvData.begin() };
    advance(gviLeftEnd, stCharBeginM1);
    StdFill(par_unseq, gvData.begin(), gviLeftEnd, gRef);
    // Initialise the right uninitialised glyph widths
    GlyphVectorIt gviRightBegin{ gvData.begin() };
    advance(gviRightBegin, stCharEnd);
    StdFill(par_unseq, gviRightBegin, gvData.end(), gRef);
    // Initialise memory for texture tile co-ordinates
    clTiles.resize(1);
    CoordList &clFirst = clTiles.front();
    clFirst.reserve(stCharEnd);
    clFirst.resize(stCharBegin);
    // Get extra tile padding dimensions. Also clamped by texture class
    const unsigned int uiPX = jsDoc.GetInteger("TileSpacingWidth"),
                       uiPY = jsDoc.GetInteger("TileSpacingHeight");
    // Init texture with custom parameters and generate tileset
    InitTextureImage(imSrc, uiTW, uiTH, uiPX, uiPY, ofeFilter);
    // Get the texture coords for the default character
    const CoordData &cdRef = clFirst[ulDefaultChar];
    // Initialise the left uninitialised texcoords with the default character
    CoordListIt cliItLeftEnd{ clFirst.begin() };
    advance(cliItLeftEnd, stCharBeginM1);
    StdFill(par_unseq, clFirst.begin(), cliItLeftEnd, cdRef);
    // Initialise the right uninitialised texcoords with the default character
    CoordListIt cliItRightBegin{ clFirst.begin() };
    advance(cliItRightBegin, stCharEnd);
    StdFill(par_unseq, cliItRightBegin, clFirst.end(), cdRef);
    // Initialise default font scale, line and letter spacing
    SetSize(static_cast<GLfloat>(jsDoc.GetNumber("InitialScale")));
    SetLineSpacing(static_cast<GLfloat>(jsDoc.GetNumber("LineSpacing")));
    SetCharSpacing(static_cast<GLfloat>(jsDoc.GetNumber("LetterSpacing")));
    // Show that we've loaded the file
    cLog->LogInfoExSafe("Font '$' loaded from bitmap (T:$x$;F:$).",
      IdentGet(), uiTW, uiTH, ofeFilter);
  }
  /* -- Constructor (Initialisation then registration) --------------------- */
  Font(void) :                         // No parameters
    /* -- Initialisers ----------------------------------------------------- */
    ICHelperFont{ cFonts, this }       // Initially registered
    /* --------------------------------------------------------------------- */
    { }                                // Do nothing else
  /* -- Constructor (without registration) --------------------------------- */
  explicit Font(const ImageFlagsConst ifcPurpose) :
    /* -- Initialisers ----------------------------------------------------- */
    ICHelperFont{ cFonts },            // Initially unregistered
    FontBase{ ifcPurpose }             // Set purpose
    /* --------------------------------------------------------------------- */
    { }                                // Do nothing else
};/* ----------------------------------------------------------------------- */
CTOR_END_NOINITS(Fonts, Font, FONT)    // End of collector class
/* -- DeInit Font Textures ------------------------------------------------- */
static void FontDeInitTextures(void)
{ // Ignore if no fonts
  if(cFonts->empty()) return;
  // De-init all the font textures and log pre/post deinit
  cLog->LogDebugExSafe("Fonts de-initialising $ objects...", cFonts->size());
  for(Font*const fCptr : *cFonts) fCptr->DeInit();
  cLog->LogInfoExSafe("Fonts de-initialising $ objects.", cFonts->size());
}
/* -- Reload Font Textures ------------------------------------------------- */
static void FontReInitTextures(void)
{ // Ignore if no fonts
  if(cFonts->empty()) return;
  // Reinit all the font textures and log pre/post init
  cLog->LogDebugExSafe("Fonts reinitialising $ objects...", cFonts->size());
  for(Font*const fCptr : *cFonts) fCptr->ReloadTexture();
  cLog->LogInfoExSafe("Fonts reinitialising $ objects.", cFonts->size());
}
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
