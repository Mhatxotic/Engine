/* == ATLAS.HPP ============================================================ **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## This module allows automatic and manual building of texture atlases ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IAtlas {                     // Start of private namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IBin::P;               using namespace ICollector::P;
using namespace IError::P;             using namespace IFboDef::P;
using namespace IImageDef::P;          using namespace ILog::P;
using namespace ILuaIdent::P;          using namespace ILuaLib::P;
using namespace IMemory::P;            using namespace IOgl::P;
using namespace IRectangle::P;         using namespace IStd::P;
using namespace ISysUtil::P;           using namespace ITexDef::P;
using namespace ITexture::P;           using namespace IUtil::P;
using namespace Lib::OS::GlFW::Types;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public namespace
/* == Atlas collector class for collector data and custom variables ======== */
CTOR_BEGIN_NOBB(Atlases, Atlas, CLHelperUnsafe)
/* == Atlas Variables Class ================================================ */
// Only put vars used in the Atlas class in here. This is an optimisation so
// we do not have to initialise all these variables more than once as we have
// more than one constructor in the main Atlas class.
/* ------------------------------------------------------------------------- */
class AtlasBase :                      // Members initially private
  /* -- Base classes ------------------------------------------------------- */
  public Texture                       // Texture class
{ /* -- Protected typedefs -------------------------------------- */ protected:
  typedef Pack<GLint> IntPack;         // Bin pack using GLint
  typedef IntPack::Rect IntPackRect;   // Bin pack rectangle
  /* -- Protected variables ------------------------------------------------ */
  OglFilterEnum    ofeFilter;          // Selected texture filter
  GLuint           uiPadding;          // Padding after each glyph
  IntPack          ipData;             // FT packed characters in image
  /* -- Reload texture parameters ------------------------------------------ */
  enum RTCmd { RT_NONE, RT_FULL, RT_PARTIAL } rtCmd; // Reload texture command
  RectUint         rRedraw;            // Reload cordinates and dimensions
  /* -- Default constructor ------------------------------------------------ */
  explicit AtlasBase(const ImageFlagsConst ifcPurpose = IP_ATLAS) :
    /* -- Initialisers ----------------------------------------------------- */
    Texture{ ifcPurpose },             // Initialise texture and register it
    ofeFilter(OF_N_N),                 // Initialise point texture filter
    uiPadding(0),                      // Initialise padding between tiles
    rtCmd(RT_NONE),                    // Initialise re-upload command
    rRedraw{                           // Initialise redraw bounds
      numeric_limits<GLuint>::max(),   // Highest possible
      numeric_limits<GLuint>::max(),   // Highest possible
      0, 0 }                           // Lowest possible
    /* -- No code ---------------------------------------------------------- */
    { }
};/* ----------------------------------------------------------------------- */
/* == Atlas Class (which inherits a Texture) =============================== */
CTOR_MEM_BEGIN(Atlases, Atlas, ICHelperUnsafe, /* n/a */),
  /* -- Base classes ------------------------------------------------------- */
  public AtlasBase                     // Atlas variables class
{  /* -- Convert co-ordinates to absolute position -------------- */ protected:
  static size_t CoordsToAbsolute(const size_t stPosX,
                                 const size_t stPosY,
                                 const size_t stWidth,
                                 const size_t stBytesPerColumn=1)
    { return ((stPosY * stWidth) + stPosX) * stBytesPerColumn; }
  /* -- Check if texture reload required ----------------------------------- */
  void AtlasCheckReloadTexture(void)
  { // Check reload command
    switch(rtCmd)
    { // No reload so just return
      case RT_NONE: return;
      // Full reload
      case RT_FULL:
      { // Reset reload command incase of error
        rtCmd = RT_NONE;
        // Full reload of texture
        ReloadTexture();
        // Log that we reuploaded the texture
        cLog->LogDebugExSafe("Atlas '$' full texture reload (S:$,$).",
          IdentGet(), DimGetWidth(), DimGetHeight());
        // Done
        return;
      } // Partial reload
      case RT_PARTIAL:
      { // Reset reload command incase of error
        rtCmd = RT_NONE;
        // Calculate position to read from in buffer
        const size_t stRTPos = CoordsToAbsolute(rRedraw.RectGetX1(),
          rRedraw.RectGetY1(), DimGetWidth(), 2);
        // Calculate position in buffer to read from
        const GLubyte*const ucpSrc =
          GetSlots().front().MemRead<GLubyte>(stRTPos, DimGetWidth());
        // Update partial texture
        UpdateEx(GetSubName(),
          rRedraw.RectGetX1<GLint>(), rRedraw.RectGetY1<GLint>(),
          static_cast<GLsizei>(rRedraw.RectGetX2() - rRedraw.RectGetX1()),
          static_cast<GLsizei>(rRedraw.RectGetY2() - rRedraw.RectGetY1()),
          GetPixelType(), ucpSrc, DimGetWidth<GLsizei>());
        // Log that we partially reuploaded the texture
        cLog->LogDebugExSafe("Atlas '$' partial re-upload (B:$,$,$,$;P:$).",
          IdentGet(), rRedraw.RectGetX1(), rRedraw.RectGetY1(),
          rRedraw.RectGetX2(), rRedraw.RectGetY2(), stRTPos);
        // Reset range parameters
        rRedraw.RectSetTopLeft(numeric_limits<GLuint>::max());
        rRedraw.RectSetBottomRight(0);
        // Done
        return;
      } // Unknown reload command
      default: XC("Internal error: Unknown reload command!",
                  "Command", rtCmd);
    }
  }
  /* -- Gray-Alpha (16-bpp) bitmap enlarging support ----------------------- */
  struct ImageTypeGrayAlpha
  { /* -- Statics ---------------------------------------------------------- */
    constexpr static BitDepth  biDepth   = BD_GRAYALPHA;
    constexpr static ByteDepth byDepth   = BY_GRAYALPHA;
    constexpr static uint64_t  uqInitVal = 0x00FF00FF00FF00FF;
    /* -- Image resize routine --------------------------------------------- */
    void Resize(const size_t stNewWidth,
                const size_t stNewHeight,
                const MemConst &mcRef,
                const size_t stOldWidth,
                const size_t stOldHeight,
                Memory &mDst)
    { // Size of a scan line from the old surface in bytes
      const size_t stSrcScanSize = stOldWidth * byDepth,
      // Size of Extra width to clear on right
        stDestExtra = (stNewWidth - stOldWidth) * byDepth;
      // Copy scan lines from the old image
      for(size_t stY = 0; stY < stOldHeight; ++stY)
      { // Calculate source position and get validated pointer to src memory
        const size_t stSrcPos = (stOldWidth * stY) * byDepth;
        const char*const cpSrc = mcRef.MemRead(stSrcPos, stSrcScanSize);
        // Calculate the destination position and copy old scan line to new
        const size_t stDestPos = (stNewWidth * stY) * byDepth;
        mDst.MemWrite(stDestPos, cpSrc, stSrcScanSize);
        // Calculate clear position and set white transparent pixels
        const size_t stClearPos = stDestPos + stSrcScanSize;
        mDst.MemFillEx(stClearPos, uqInitVal, stDestExtra);
      } // Calculate size of a scan line in the new surface
      const size_t stDestScanSize = stNewWidth * byDepth;
      // Clear extra pixels on the bottom
      for(size_t stY = stOldHeight; stY < stNewHeight; ++stY)
      { // Calculate source and destination position and copy the scan line
        const size_t stClearPos = (stNewWidth * stY) * byDepth;
        mDst.MemFillEx(stClearPos, uqInitVal, stDestScanSize);
      }
    }
    /* -- Glyph copy routine ----------------------------------------------- */
    void Copy(const size_t stSrcWidth,
              const size_t stSrcHeight,
              const MemConst &mcSrc,
              const size_t stDstX,
              const size_t stDstY,
              const size_t stDstWidth,
              Memory &mDst)
    { // For each pixel row of glyph image
      for(size_t stPixPosY = 0; stPixPosY < stSrcHeight; ++stPixPosY)
      { // Calculate Y position co-ordinate in buffer.
        const size_t stPosY = stDstY + stPixPosY;
        // For each pixel column of glyph image
        for(size_t stPixPosX = 0; stPixPosX < stSrcWidth; ++stPixPosX)
        { // Calculate X position co-ordinate in buffer.
          const size_t stPosX = stDstX + stPixPosX;
          // Set the character to write...
          // * Step 1: 0xNN   (8-bit colour pixel value) converts to...
          // * Step 2: 0x00NN (16-bit integer) shift eight bits left...
          // * Step 3: 0xNN00 (16-bit integer) set all first eight bits...
          // * Step 4: 0xNNFF (16-bit colour alpha pixel value).
          // This will obviously need to be revised if compiling on Big-End.
          const size_t stSrcPos =
            CoordsToAbsolute(stPixPosX, stPixPosY, stSrcWidth);
          const uint16_t usPixel = static_cast<uint16_t>(
            static_cast<int>(mcSrc.MemReadInt<uint8_t>(stSrcPos)) << 8 | 0xFF);
          // ...and the final offset position value
          const size_t stDstPos =
            CoordsToAbsolute(stPosX, stPosY, stDstWidth, sizeof(usPixel));
          // If the paint position is in the tile bounds?
          // Copy pixels from source to destination.
          mDst.MemWriteInt<uint16_t>(stDstPos, usPixel);
        }
      }
    }
    /* -- Initialse memory area of image ----------------------------------- */
    void Init(Memory &mPixels) { mPixels.MemFill(uqInitVal); }
    /* -- Constructor that does nothing ------------------------------------ */
    ImageTypeGrayAlpha(void) = default;
  };/* --------------------------------------------------------------------- */
  /* -- Render texture data to memory -------------------------------------- */
  template<class ImageType>
    void AtlasCopyBitmap(const IntPackRect &iprRef,
                         const size_t stSrcWidth,
                         const size_t stSrcHeight,
                         CoordData &cdRef,
                         const void*const vpSrc,
                         Memory &mDst)
  { // Get source and destination sizes and return if they're different
    const size_t stDstWidth = iprRef.DimGetWidth<size_t>(),
                 stDstHeight = iprRef.DimGetHeight<size_t>();
    if(stSrcWidth != stDstWidth || stSrcHeight != stDstHeight) return;
    // Get destination X and Y position as size_t
    const size_t stDstX = iprRef.CoordGetX<size_t>(),
                 stDstY = iprRef.CoordGetY<size_t>();
    // Put glyph data and size in a managed class
    const MemConst mcSrc{ stSrcWidth * stSrcHeight, vpSrc };
    // Run the requested copy procedure
    ImageType{}.Copy(stSrcWidth, stSrcHeight, mcSrc,
      stDstX, stDstY, DimGetWidth<size_t>(), mDst);
    // Calculate texture bounds
    const GLfloat
      fMinX = static_cast<GLfloat>(stDstX),
      fMinY = static_cast<GLfloat>(stDstY),
      fMaxX = fMinX + static_cast<GLfloat>(stSrcWidth),
      fMaxY = fMinY + static_cast<GLfloat>(stSrcHeight),
      fBW   = DimGetWidth<GLfloat>(),
      fBH   = DimGetHeight<GLfloat>();
    // Get reference to first and second triangles
    TriCoordData &tcT1 = cdRef[0], &tcT2 = cdRef[1];
    // Push opengl tile coords (keep .fW/.fH to zero which is already zero)
    tcT1[0] = tcT1[4] = tcT2[2] = fMinX / fBW; // Left
    tcT1[1] = tcT1[3] = tcT2[5] = fMinY / fBH; // Top
    tcT1[2] = tcT2[0] = tcT2[4] = fMaxX / fBW; // Right
    tcT1[5] = tcT2[1] = tcT2[3] = fMaxY / fBH; // Bottom
  }
  /* -- Upload tile to texture --------------------------------------------- */
  template<class ImageType>
    void AtlasAddBitmap(const size_t stSlot,
                        const GLuint uiTWidth,
                        const GLuint uiTHeight,
                        const void*const vpSrc)
  { // Calculate size plus padding and return if size not set
    const GLuint uiTPWidth = uiTWidth + uiPadding,
                 uiTPHeight = uiTHeight + uiPadding;
    // Get image slot and data we're writing to
    ImageSlot &isRef = GetSlots().front();
    // Get co-ordinates to tile on texture
    CoordData &cdRef = clTiles[0][stSlot];
    // Put this glyph in the bin packer and if succeeded
    IntPackRect iprNew{ ipData.Insert(uiTPWidth, uiTPHeight) };
    if(iprNew.DimGetHeight() > 0)
    { // The result rect will include padding so remove it
      iprNew.DimDec(static_cast<int>(uiPadding));
      // Copy the glyph to texture atlast
      AtlasCopyBitmap<ImageType>(iprNew, static_cast<size_t>(uiTWidth),
        static_cast<size_t>(uiTHeight), cdRef, vpSrc, isRef);
      // Full redraw not already specified?
      if(rtCmd != RT_FULL)
      { // Set partial redraw
        rtCmd = RT_PARTIAL;
        // Set lowest most left bound
        if(iprNew.CoordGetX<GLuint>() < rRedraw.RectGetX1())
          rRedraw.RectSetX1(iprNew.CoordGetX<GLuint>());
        // Set lowest most top bound
        if(iprNew.CoordGetY<GLuint>() < rRedraw.RectGetY1())
          rRedraw.RectSetY1(iprNew.CoordGetY<GLuint>());
        // Set highest most right bound
        const GLuint uiX2 = static_cast<GLuint>(iprNew.CoordGetX() +
          iprNew.DimGetWidth());
        if(uiX2 > rRedraw.RectGetX2()) rRedraw.RectSetX2(uiX2);
        // Set highest most bottom bound
        const GLuint uiY2 = static_cast<GLuint>(iprNew.CoordGetY() +
          iprNew.DimGetHeight());
        if(uiY2 > rRedraw.RectGetY2()) rRedraw.RectSetY2(uiY2);
      } // Done
      return;
    } // Failed to resize so get next biggest size from bounds
    const GLuint uiSize =
      TextureGetMaxSizeFromBounds(ipData.DimGetWidth<GLuint>(),
        ipData.DimGetHeight<GLuint>(), uiTPWidth, uiTPHeight, 2);
    const GLuint uiMaximum = cOgl->MaxTexSize();
    // Make sure the size is supported by graphics. !FIXME. Make a new
    // OpenGL surface in a sub-texture slot here but this may require a bit
    // of work.
    if(uiSize > uiMaximum)
      XC("Cannot grow texture any further due to GPU limitation!",
         "Identifier",    IdentGet(),   "Slot",           stSlot,
         "BinWidth",      ipData.DimGetWidth(),
         "BinHeight",     ipData.DimGetHeight(),
         "TileWidth",     uiTWidth,     "TileHeight",     uiTHeight,
         "TileWidth+Pad", uiTPWidth,    "TileHeight+Pad", uiTPHeight,
         "Requested",     uiSize,       "Maximum",       uiMaximum);
    // Double the size taking into account that the requested glyph size
    // must fit inside it as well and the video card maximum texture
    // space must support the glyph as well. Then try placing it in the
    // bin again and if it still failed then there is nothing more we can
    // do for now.
    ipData.Resize(uiSize, uiSize);
    iprNew = ipData.Insert(uiTPWidth, uiTPHeight);
    if(iprNew.DimGetHeight() <= 0)
      XC("No texture space left for tile!",
         "Identifier",    IdentGet(),   "Slot",       stSlot,
         "BinWidth",      ipData.DimGetWidth(),
         "BinHeight",     ipData.DimGetHeight(),
         "TileWidth",     uiTWidth,     "TileHeight",     uiTHeight,
         "TileWidth+Pad", uiTPWidth,    "TileHeight+Pad", uiTPHeight,
         "Occupancy",     ipData.Occupancy(),
         "Maximum",       cOgl->MaxTexSize());
    // THe result rect will include padding so remove it
    iprNew.DimDec(static_cast<int>(uiPadding));
    // Convert new surface size to size_t
    const size_t stBinWidth = ipData.DimGetWidth<size_t>(),
                 stBinHeight = ipData.DimGetHeight<size_t>();
    // Get image data
    ImageType itData;
    // We need to make a new image because we need to modify the data in
    // the old obsolete image.
    Memory mDst{ stBinWidth * stBinHeight * itData.byDepth};
    // Run the requested copy procedure
    itData.Resize(stBinWidth, stBinHeight, isRef, DimGetWidth(),
      DimGetHeight(), mDst);
    // This is the new image and the old one will be destroyed
    const size_t stOldAlloc = isRef.MemSize();
    isRef.MemSwap(mDst);
    mDst.MemDeInit();
    AdjustAlloc(stOldAlloc, isRef.MemSize());
    // Calculate how much the image increased. This should really be 2
    // every time but we'll just make a calculation like this just
    // incase.
    const unsigned int uiDivisor =
      ipData.DimGetWidth<unsigned int>() / DimGetWidth();
    // Update the image dimensions of the font texture
    DimSet(ipData.DimGetWidth<unsigned int>(),
           ipData.DimGetHeight<unsigned int>());
    // Update the dimensions in the image slot class
    isRef.DimSet(*this);
    // Now we need to walk through the char datas and reduce the values
    // by the enlargement factor. A very simple and effective solution.
    // Note that using transform is ~100% slower than this.
    StdForEach(par_unseq, clTiles[0].begin(), clTiles[0].end(),
      [uiDivisor](CoordData &tcI)
        { for(size_t stTriId = 0; stTriId < stTrisPerQuad; ++stTriId)
            for(size_t stFltId = 0; stFltId < stFloatsPerCoord; ++stFltId)
              tcI[stTriId][stFltId] /= uiDivisor; });
    // Copy the glyph to texture atlast
    AtlasCopyBitmap<ImageType>(iprNew,
      static_cast<size_t>(uiTWidth), static_cast<size_t>(uiTHeight),
      cdRef, vpSrc, isRef);
    // Re-upload the whole texture to VRAM.
    rtCmd = RT_FULL;
    // Say that we resized the image
    cLog->LogDebugExSafe("Atlas enlarged '$' by a factor of $ to $x$.",
      IdentGet(), uiDivisor, DimGetWidth(), DimGetHeight());
  }
  /* -- Initialise the atlas --------------------------------------- */ public:
  template<class ImageType>
    void AtlasInit(const string &strId,
                   const GLuint uiTWidth,
                   const GLuint uiTHeight,
                   const GLuint uiISize,
                   const GLuint uiTPadding,
                   const OglFilterEnum ofeTFilter)
  { // Make sure padding isn't negative. We use int because it is optimal for
    // use with the BinPack routines.
    if(UtilIntWillOverflow<int>(uiPadding))
      XC("Atlas padding size overflows signed integer range!",
         "Identifier", strId, "Requested", uiTPadding);
    // Initialise base size, padding and opengl filter
    uiPadding = uiTPadding;
    ofeFilter = ofeTFilter;
    // Set initial tile size. This is just for the Texture class.
    duiTile.DimSet(uiTWidth, uiTHeight);
    // Set initial size of image. The image size starts here and can
    // automatically grow by the power of 2 if more space is needed.
    DimSet(uiISize ? uiISize :
      TextureGetMaxSizeFromBounds(0, 0, uiTWidth, uiTHeight, 1));
    // Check if texture size is valid
    if(DimGetWidth() > cOgl->MaxTexSize() || uiTWidth > cOgl->MaxTexSize())
      XC("Atlas dimensions not supported by graphics processor!",
         "Identifier", strId,               "Requested",  uiISize,
         "Width",      DimGetWidth(),       "Height",     DimGetHeight(),
         "TileWidth",  uiTWidth,            "TileHeight", uiTHeight,
         "Maximum",    cOgl->MaxTexSize());
    // Estimate how many glyphs we're fitting in here to prevent unnecessary
    // alocations
    const size_t stGColumns = DimGetWidth() / uiTWidth,
                 stGRows = DimGetHeight() / uiTHeight,
                 stGTotal = UtilNearestPow2<size_t>(stGColumns * stGRows);
    // Init the virtual space where we place the bounds of each tile
    ipData.Init(DimGetWidth(), DimGetHeight(), stGTotal, stGTotal);
    // Get image processing data
    ImageType itData;
    // Allocate memory for bitmap surface pixels
    Memory mPixels{ DimGetWidth() * DimGetHeight() * itData.byDepth };
    // Clear it with uniformed transparent pixels
    itData.Init(mPixels);
     // Initialise the 'Image' class.
    InitRaw(strId, mPixels, DimGetWidth(), DimGetHeight(), itData.biDepth);
    // Initialise the 'Texture' class.
    InitTextureImage(*this,
      uiTWidth, uiTHeight, uiPadding, uiPadding, ofeFilter, false);
  }
  /* -- Constructor (Initialisation then registration) --------------------- */
  Atlas(void) :                        // No parameters
    /* -- Initialisers ----------------------------------------------------- */
    ICHelperAtlas{ cAtlases, this }    // Initially registered
    /* --------------------------------------------------------------------- */
    { }                                // Do nothing else
  /* -- Constructor (without registration) --------------------------------- */
  explicit Atlas(const ImageFlagsConst ifcPurpose) :
    /* -- Initialisers ----------------------------------------------------- */
    ICHelperAtlas{ cAtlases },         // Initially unregistered
    AtlasBase{ ifcPurpose }            // Initialise purpose of atlas
    /* --------------------------------------------------------------------- */
    { }                                // Do nothing else
};/* ----------------------------------------------------------------------- */
CTOR_END_NOINITS(Atlases, Atlas, ATLAS) // End of collector class
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
