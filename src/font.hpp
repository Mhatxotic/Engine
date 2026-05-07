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
using namespace ICollector::P;         using namespace ICoords::P;
using namespace IDim::P;               using namespace IError::P;
using namespace IFileMap::P;           using namespace IFlags::P;
using namespace IFormat::P;            using namespace IFreeType::P;
using namespace IFtf::P;               using namespace IImageDef::P;
using namespace IJson::P;              using namespace ILog::P;
using namespace ILuaIdent::P;          using namespace ILuaLib::P;
using namespace IMemory::P;            using namespace IOgl::P;
using namespace IStd::P;               using namespace ISystem::P;
using namespace ISysUtil::P;           using namespace ITexDef::P;
using namespace ITexture::P;           using namespace IUtf::P;
using namespace IUtil::P;              using namespace Lib::FreeType;
using namespace Lib::OS::GlFW::Types;
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
    public CoordsGLFloat               // Glyph bounding co-ordinates
  { /* -- Private variables ------------------------------------------------ */
    bool           bLoaded;            // 0=ft unloaded or 1=ft loaded
    GLfloat        glfAdvance;         // Width, height and advance of glyph
    /* -- Returns character advance -------------------------------- */ public:
    GLfloat GlyphGetAdvance() const { return glfAdvance; }
    /* -- Sets character advance ------------------------------------------- */
    void GlyphSetAdvance(const GLfloat glfNAdvance)
      { glfAdvance = glfNAdvance; }
    /* -- Returns if this glyph has been loaded ---------------------------- */
    bool GlyphIsLoaded() const { return bLoaded; }
    /* -- Set glyph as loaded ---------------------------------------------- */
    void GlyphSetLoaded() { bLoaded = true; }
    /* -- Constructor that initialises all members ------------------------- */
    Glyph(const GLfloat glfWidth,      // Width of glyph in pixels for opengl
          const GLfloat glfHeight,     // Height of glyph in pixels for opengl
          const bool    bNLoaded,      // Is the glyph loaded?
          const GLfloat glfNAdvance,   // Specified glyph advance in pixels
          const GLfloat glfX1,         // Top-left co-ordinate of glyph
          const GLfloat glfY1,         // Bottom-left co-ordinate of glyph
          const GLfloat glfX2,         // Top-right co-ordinate of glyph
          const GLfloat glfY2) :       // Bottom-right co-ordinate of glyph
      /* -- Initialisers --------------------------------------------------- */
      DimGLFloat{ glfWidth,            // Init glpyh base size width
                  glfHeight },         // Init glpyh base size height
      CoordsGLFloat{ glfX1, glfY1,     // Init adjustment src co-ordinates
                     glfX2, glfY2 },   // Init adjustment dest co-ordinates
      bLoaded(bNLoaded),               // Init specified loaded value
      glfAdvance(glfNAdvance)          // Init specified advance value
      /* -- No code -------------------------------------------------------- */
      {}
    /* -- Default Constructor ---------------------------------------------- */
    Glyph() :                          // No arguments
      /* -- Initialisers --------------------------------------------------- */
      bLoaded(false),                  // Character not loaded yet
      glfAdvance(0.0f)                 // Character advance value
      /* -- No code -------------------------------------------------------- */
      {}
  };/* -- Variables -------------------------------------------------------- */
  using GlyphVector   = StdVector<Glyph>;      // Vector of Glyphs
  using GlyphVectorIt = GlyphVector::iterator; // Iterator to GlyphVector
  /* --------------------------------------------------------------- */ public:
  GlyphVector      gvData;             // Glyph and outline data
  DimGLFloat       gldfScale,          // Scaled font width and height
                   gldfFont;           // Requested font size for OpenGL
  size_t           stMultiplier;       // 1 if no outline, 2 if outline
  GLfloat          glfCSpacing,        // Character spacing adjustment
                   glfCSpacingScale,   // Character spacing scaled size
                   glfLSpacing,        // Text line spacing
                   glfLSpacingHeight,  // Height of line spacing plus height
                   glfGlyphSize,       // Scaled font texture glyph size
                   glfScale,           // Font scale
                   glfGPad,            // Glyph position adjustment
                   glfGPadScaled;      // Glyph position adjustment scaled
  Texture         *tGlyphs;            // Texture for print calls only
  FboItem          fiOutline;          // Outline colour
  /* -- Freetype handles --------------------------------------------------- */
  Ftf              ftfData;            // FT font data
  FT_ULong         fulDefaultChar;     // Default fallback character
  /* -- Default constructor ------------------------------------------------ */
  explicit FontBase(const ImageFlagsConst ifcPurpose = IP_FONT) :
    /* --------------------------------------------------------------------- */
    Atlas{ ifcPurpose },               stMultiplier(1),
    glfCSpacing(0.0f),                 glfCSpacingScale(0.0f),
    glfLSpacing(0.0f),                 glfLSpacingHeight(0.0f),
    glfGlyphSize(0.0f),                glfScale(0.0f),
    glfGPad(0.0f),                     glfGPadScaled(0.0f),
    tGlyphs(nullptr),                  fiOutline{ 0xFF000000 },
    fulDefaultChar('?')
    /* --------------------------------------------------------------------- */
    {}
};/* ----------------------------------------------------------------------- */
/* == Font Class (which inherits an Atlas) ==========-====================== */
CTOR_MEM_BEGIN(Fonts, Font, ICHelperUnsafe, /* n/a */),
  /* -- Base classes ------------------------------------------------------- */
  public FontBase                      // Font variables class
{ /* -- Stroker functors for DoInitFTChar() -------------------------------- */
#include "fontglph.hpp"                // Include glyph loading members inline
#include "fontblit.hpp"                // Include glyph blitting members inline
  /* -- Recalculate line spacing ------------------------------------------- */
  GLfloat GetLineSpacingHeight() const { return glfLSpacingHeight; }
  void UpdateHeightPlusLineSpacing()
    { glfLSpacingHeight = GetLineSpacing() + gldfScale.DimGetHeight(); }
  /* -- Update character spacing of the font ------------------------------- */
  GLfloat GetCharSpacingScale() const { return glfCSpacingScale; }
  void UpdateCharSpacingTimesScale()
    { glfCSpacingScale = GetCharSpacing() * glfScale; }
  /* -- Return text height plus line spacing ------------------------------- */
  void UpdateGlyphPaddingTimesScale() { glfGPadScaled = glfGPad * glfScale; }
  /* -- Set glyph size --------------------------------------------- */ public:
  void SetGlyphSize(const GLfloat glfSize)
    { glfGlyphSize = glfSize; UpdateGlyphPaddingTimesScale(); }
  /* -- Get character count ------------------------------------------------ */
  size_t GetCharCount() const { return gvData.size() / stMultiplier; }
  /* -- Get character scale ------------------------------------------------ */
  GLfloat GetCharScale() const { return glfScale; }
  /* -- Get bin occupancy -------------------------------------------------- */
  double GetTexOccupancy() const { return ipData.Occupancy(); }
  /* -- Get below baseline height of specified character ------------------- */
  GLfloat GetBaselineBelow(const Codepoint cChar)
  { // Now get character info and return data
    const Glyph &gRef = gvData[CheckGlyph(cChar)];
    return (gRef.CoordsGetY2() - gRef.DimGetHeight()) * glfScale;
  }
  /* -- Get/set line spacing ----------------------------------------------- */
  GLfloat GetLineSpacing() const { return glfLSpacing; }
  void SetLineSpacing(const GLfloat glfNLSpacing)
    { glfLSpacing = glfNLSpacing; UpdateHeightPlusLineSpacing(); }
  /* -- Set new glyph Y padding and the new scaling value ------------------ */
  void SetGlyphPadding(const GLfloat glfNGPad)
    { glfGPad = glfNGPad; UpdateGlyphPaddingTimesScale(); }
  /* -- Set character spacing of the font ---------------------------------- */
  GLfloat GetCharSpacing() const { return glfCSpacing; }
  void SetCharSpacing(const GLfloat glfNCSpacing)
    { glfCSpacing = glfNCSpacing; UpdateCharSpacingTimesScale(); }
  /* -- Set size of the font ----------------------------------------------- */
  void SetSize(const GLfloat glfNScale)
  { // Set scale
    glfScale = UtilClamp(glfNScale, 0.0f, 1024.0f);
    // Update scaled dimensions
    gldfScale.DimSet(GetTileWidthFloat() * glfScale,
      GetTileHeightFloat() * glfScale);
    // Update glyph size
    SetGlyphSize(gldfScale.DimGetHeight());
    // Update frequently used values
    UpdateCharSpacingTimesScale();
    UpdateHeightPlusLineSpacing();
  }
  /* -- Do initialise specified freetype character range ------------------- */
  void InitFTCharRange(const Codepoint cStart, const Codepoint cEnd)
  { // Ignore if not a freetype font.
    if(!ftfData.IsLoaded()) return;
    // Log pre-cache progress
    cLog->LogDebugExSafe("Font '$' pre-caching character range $ to $...",
      NameGet(), cStart, cEnd);
    // Load the specified character range
    DoInitFTCharRangeApplyStroker<HandleGlyphFunc::FreeType>(cStart, cEnd);
    // Check if any textures need reloading
    AtlasCheckReloadTexture();
    // Log success
    cLog->LogDebugExSafe(
      "Font '$' finished pre-caching character range $ to $.",
      NameGet(), cStart, cEnd);
  }
  /* -- Do initialise all freetype characters in specified string ---------- */
  void InitFTCharString(const StdStringView strvString)
  { // Ignore if string not valid or font not loaded
    if(strvString.empty() || !ftfData.IsLoaded()) return;
    // Do load string characters
    DoInitFTCharStringApplyStroker<HandleGlyphFunc::FreeType>(strvString);
    // Check if any textures need reloading
    AtlasCheckReloadTexture();
  }
  /* -- Do initialise freetype font ---------------------------------------- */
  void InitFontFtf(Ftf &ftfSrc, const GLuint gluISize,
    const GLuint gluNPadding, const OglFilterEnum ofeNFilter,
    const ImageFlagsConst &ifcFlags)
  { // Show that we're loading the file
    cLog->LogDebugExSafe("Font loading '$' (IS:$;P:$;F:$;FL:$$)...",
      ftfSrc.NameGet(), gluISize, gluNPadding, ofeNFilter, StdIOSHex,
      ifcFlags.FlagGet());
    // If source and destination ftf class are not the same?
    if(&ftfData != &ftfSrc)
    { // Take ownership of specified freetype identifier and handles
      NameSwap(ftfSrc);
      ftfData.SwapFtf(ftfSrc);
      ftfData.NameSet(NameGet());
    } // Initialise load flags
    FlagSet(ifcFlags);
    SetFontFreeType();
    // Outline characters will be stored one after the opaque characters
    // thus doubling the size of the glyph data/texture coord lists. The
    // multiplier is already 1 by default so this just makes it equal 2.
    if(ftfData.IsOutline()) stMultiplier = 2;
    // Initialise the atlas texture
    AtlasInit<ImageTypeGrayAlpha>(NameGet(),
      static_cast<GLuint>(ceil(ftfData.DimGetWidth())),
      static_cast<GLuint>(ceil(ftfData.DimGetHeight())),
      gluISize, gluNPadding, ofeNFilter);
    // Set requested font size for opengl
    gldfFont.DimSet(ftfData);
    // Set default scaled font size and line spacing adjust
    SetSize(1.0f);
    glfLSpacingHeight = ftfData.DimGetHeight();
    // Update tile size as GLfloat for opengl
    dglfTile.DimSet(GetTileWidth<GLfloat>(), GetTileHeight<GLfloat>());
    // Make enough space for initial tex coords set
    clTiles.resize(1);
    // Show that we've loaded the file
    cLog->LogInfoExSafe("Font '$' loaded FT font (IS:$;P:$;F:$;FL:$$)...",
      NameGet(), gluISize, uPadding, ofeFilter, StdIOSHex, ifcFlags.FlagGet());
  }
  /* -- Init a pre-rendered font directly ---------------------------------- */
  void InitFontImage(Image &imSrc, const GLuint gluTWidth,
    const GLuint gluTHeight, const GLuint gluPWidth, const GLuint gluPHeight,
    const OglFilterEnum ofeNFilter, const size_t stCharCount,
    const size_t stCharBegin, const size_t stCharDefault,
    const GLFloatVector &vglfWidths)
  { // Show filename
    cLog->LogDebugExSafe("Font loading bitmap '$'...", imSrc.NameGet());
    // Set Font's role as a Bitmap font.
    SetFontBitmap();
    // Set ending character
    const size_t stCharEnd = stCharBegin + stCharCount;
    // Set default character
    fulDefaultChar = static_cast<decltype(fulDefaultChar)>(stCharDefault);
    // Set filter
    ofeFilter = ofeNFilter;
    // Convert tile width and height to float
    const GLfloat fW = static_cast<GLfloat>(gluTWidth),
                  fH = static_cast<GLfloat>(gluTHeight);
    // If no values were specified?
    if(vglfWidths.empty())
    { // Add the starting unused characters
      gvData.resize(stCharBegin);
      // Add all the characters to the default with automatically
      gvData.resize(stCharEnd, { fW, fH, true, fW, 0.0f, 0.0f, fW, fH });
    } // Error if we do not have the correct number of items
    else if(vglfWidths.size() != stCharCount)
      XC("Unexpected number of widths specified!",
        "Name",   imSrc.NameGet(), "Required", stCharCount,
        "Actual", vglfWidths.size());
    // We have the correct number of items
    else
    { // Reserve memory for all the requested character space
      gvData.reserve(stCharEnd);
      // Add the starting unused characters
      gvData.resize(stCharBegin);
      // Set all the specified widths in the widths array
      StdTransformNXP(vglfWidths.cbegin(), vglfWidths.cend(),
        StdBackInserter(gvData), [fW, fH](const GLfloat fvWidth)->const Glyph{
          return { fW, fH, true, fvWidth, 0.0f, 0.0f, fW, fH };
        });
    }
    // Calculate beginning character minus one
    const size_t stCharBeginM1 = stCharBegin - 1;
    // Initialise the left uninitialised widths with the default character
    const Glyph &gRef = gvData[stCharDefault];
    GlyphVectorIt gviLeftEnd{ gvData.begin() };
    StdAdvance(gviLeftEnd, stCharBeginM1);
    StdFill(par_unseq, gvData.begin(), gviLeftEnd, gRef);
    // Initialise the right uninitialised widths with the default character
    GlyphVectorIt gviRightBegin{ gvData.begin() };
    StdAdvance(gviRightBegin, stCharEnd);
    StdFill(par_unseq, gviRightBegin, gvData.end(), gRef);
    // Initialise memory for texture tile co-ordinates
    clTiles.resize(1);
    CoordList &clFirst = clTiles.front();
    clFirst.reserve(stCharEnd);
    clFirst.resize(stCharBegin);
    // Init texture with custom parameters and generate tileset
    InitTextureImage(imSrc, gluTWidth, gluTHeight, gluPWidth, gluPHeight,
      ofeFilter);
    // Initialise the left uninitialised texcoords with the default character
    const CoordData &cdRef = clFirst[stCharDefault];
    CoordListIt cliItLeftEnd{ clFirst.begin() };
    StdAdvance(cliItLeftEnd, stCharBeginM1);
    StdFill(par_unseq, clFirst.begin(), cliItLeftEnd, cdRef);
    // Initialise the right uninitialised texcoords with the default character
    CoordListIt cliItRightBegin{ clFirst.begin() };
    StdAdvance(cliItRightBegin, stCharEnd);
    StdFill(par_unseq, cliItRightBegin, clFirst.end(), cdRef);
    // Initialise default font scale and line spacing to height
    SetSize(1.0f);
    glfLSpacingHeight = fH;
    // Show that we've loaded the file
    cLog->LogInfoExSafe("Font '$' loaded from bitmap (T:$x$;F:$).",
      NameGet(), gluTWidth, gluTHeight, ofeFilter);
  }
  /* -- Init a pre-rendered font with manifest ----------------------------- */
  void InitFontImageManifest(Image &imSrc, const Json &jsDoc)
  { // Show filename
    cLog->LogDebugExSafe("Font loading bitmap '$' with manifest '$'...",
      imSrc.NameGet(), jsDoc.NameGet());
    // Set Font's role as a Bitmap font.
    SetFontBitmap();
    // Make sure correct version
    const unsigned uVersionRequired = 1,
                   uVersion = jsDoc.GetInteger("Version");
    if(uVersion != uVersionRequired)
      XC("Invalid version in manifest!",
        "Name",     imSrc.NameGet(),  "Manfiest", jsDoc.NameGet(),
        "Required", uVersionRequired, "Actual",   uVersion);
    // Number of characters supported cannot be zero!
    const size_t stCharCount = jsDoc.GetInteger("CharCount");
    if(stCharCount < 1)
      XC("Invalid character count in manifest!",
        "Name",  imSrc.NameGet(), "Manfiest", jsDoc.NameGet(),
        "Value", stCharCount);
    // Get beginning glyph id and calculate ending id
    const size_t stCharBegin = jsDoc.GetInteger("CharBegin"),
                 stCharEnd = stCharBegin + stCharCount;
    // Get and check default character
    // Check if it's a literal number as prefixed by a hash sign? Convert as a
    // literal number else convert as an a literal character then verify it is
    // in the specified range.
    fulDefaultChar = jsDoc.GetInteger("Default");
    if(fulDefaultChar < stCharBegin || fulDefaultChar >= stCharEnd)
      XC("Default character index out of range in manifest!",
        "Name",    imSrc.NameGet(), "Manfiest", jsDoc.NameGet(),
        "Default", fulDefaultChar,  "Minimum",  stCharBegin,
        "Maximum", stCharEnd,       "Count",    stCharCount);
    // Get filter
    ofeFilter = static_cast<decltype(ofeFilter)>(jsDoc.GetInteger("Filter"));
    if(ofeFilter >= OF_MAX)
      XC("Invalid filter index specified in manifest!",
        "Name",   imSrc.NameGet(), "Manfiest", jsDoc.NameGet(),
        "Filter", ofeFilter);
    // Look for widths and throw if there are none then report them in log
    using Lib::RapidJson::Value;
    const Value &rjvWidths = jsDoc.GetValue("Widths");
    if(!rjvWidths.IsArray())
      XC("Widths array not valid!",
        "Name", imSrc.NameGet(), "Manfiest", jsDoc.NameGet());
    if(rjvWidths.Size() != stCharCount)
      XC("Unexpected number of widths specified in manifest!",
        "Name",   imSrc.NameGet(), "Manfiest", jsDoc.NameGet(),
        "Expect", stCharCount,     "Actual",   rjvWidths.Size());
    // Reserve memory for all the requested character space
    gvData.reserve(stCharEnd);
    // Add the starting unused characters
    gvData.resize(stCharBegin);
    // Read size of tile. Texture init will clamp this if needed.
    const GLuint uTW = jsDoc.GetInteger("TileWidth"),
                 uTH = jsDoc.GetInteger("TileHeight");
    if(uTW > cOgl->MaxTexSize() || uTH > cOgl->MaxTexSize())
      XC("Invalid tile dimensions in manifest!",
        "Name",    imSrc.NameGet(), "Manfiest", jsDoc.NameGet(),
        "Width",   uTW,             "Height",   uTH,
        "Maximum", cOgl->MaxTexSize());
    // Convert to float as we need a float version of this in the next loop
    const GLfloat fW = static_cast<GLfloat>(uTW),
                  fH = static_cast<GLfloat>(uTH);
    // Add the characters the manifest file cares about
    StdTransformNXP(rjvWidths.Begin(), rjvWidths.End(),
      StdBackInserter(gvData),
      [&imSrc, &jsDoc, fW, fH](const Value &rjvItem)->const Glyph{
        if(rjvItem.IsUint())
          return { fW, fH, true, static_cast<GLfloat>(rjvItem.GetUint()),
            0.0f, 0.0f, fW, fH };
        XC("Invalid element type from widths array in manifest!",
          "Name", imSrc.NameGet(), "Manfiest", jsDoc.NameGet());
      });
    // Calculate the beginning character minus one
    const size_t stCharBeginM1 = stCharBegin - 1;
    // Get the default character width data
    const Glyph &gRef = gvData[fulDefaultChar];
    // Initialise the left uninitialised glyph widths
    GlyphVectorIt gviLeftEnd{ gvData.begin() };
    StdAdvance(gviLeftEnd, stCharBeginM1);
    StdFill(par_unseq, gvData.begin(), gviLeftEnd, gRef);
    // Initialise the right uninitialised glyph widths
    GlyphVectorIt gviRightBegin{ gvData.begin() };
    StdAdvance(gviRightBegin, stCharEnd);
    StdFill(par_unseq, gviRightBegin, gvData.end(), gRef);
    // Initialise memory for texture tile co-ordinates
    clTiles.resize(1);
    CoordList &clFirst = clTiles.front();
    clFirst.reserve(stCharEnd);
    clFirst.resize(stCharBegin);
    // Get extra tile padding dimensions. Also clamped by texture class
    const unsigned uPX = jsDoc.GetInteger("TileSpacingWidth"),
                   uPY = jsDoc.GetInteger("TileSpacingHeight");
    // Init texture with custom parameters and generate tileset
    InitTextureImage(imSrc, uTW, uTH, uPX, uPY, ofeFilter);
    // Get the texture coords for the default character
    const CoordData &cdRef = clFirst[fulDefaultChar];
    // Initialise the left uninitialised texcoords with the default character
    CoordListIt cliItLeftEnd{ clFirst.begin() };
    StdAdvance(cliItLeftEnd, stCharBeginM1);
    StdFill(par_unseq, clFirst.begin(), cliItLeftEnd, cdRef);
    // Initialise the right uninitialised texcoords with the default character
    CoordListIt cliItRightBegin{ clFirst.begin() };
    StdAdvance(cliItRightBegin, stCharEnd);
    StdFill(par_unseq, cliItRightBegin, clFirst.end(), cdRef);
    // Initialise default font scale, line and letter spacing
    SetSize(static_cast<GLfloat>(jsDoc.GetNumber("InitialScale")));
    SetLineSpacing(static_cast<GLfloat>(jsDoc.GetNumber("LineSpacing")));
    SetCharSpacing(static_cast<GLfloat>(jsDoc.GetNumber("LetterSpacing")));
    // Show that we've loaded the file
    cLog->LogInfoExSafe("Font '$' loaded from bitmap (T:$x$;F:$).",
      NameGet(), uTW, uTH, ofeFilter);
  }
  /* -- Constructor (Initialisation then registration) --------------------- */
  Font() :
    /* -- Initialisers ----------------------------------------------------- */
    ICHelperFont{ cFonts, this }       // Initially registered
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- Constructor (without registration) --------------------------------- */
  explicit Font(const ImageFlagsConst ifcPurpose) :
    /* -- Initialisers ----------------------------------------------------- */
    ICHelperFont{ cFonts },            // Initially unregistered
    FontBase{ ifcPurpose }             // Set purpose
    /* -- No code ---------------------------------------------------------- */
    {}
};/* ----------------------------------------------------------------------- */
CTOR_END_NOINITS(Fonts, Font, FONT)    // End of collector class
/* -- DeInit Font Textures ------------------------------------------------- */
static void FontDeInitTextures()
{ // Ignore if no fonts
  if(cFonts->empty()) return;
  // De-init all the font textures and log pre/post deinit
  cLog->LogDebugExSafe("Fonts de-initialising $ objects...", cFonts->size());
  for(Font*const fCptr : *cFonts) fCptr->DeInit();
  cLog->LogInfoExSafe("Fonts de-initialising $ objects.", cFonts->size());
}
/* -- Reload Font Textures ------------------------------------------------- */
static void FontReInitTextures()
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
