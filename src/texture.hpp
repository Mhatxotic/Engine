/* == TEXTURE.HPP ========================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## This module handles the loading and uploading of textures into      ## **
** ## OpenGL. It will also handle some core drawing procedures inside the ## **
** ## collector class too.                                                ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace ITexture {                   // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace ICollector::P;         using namespace IDim::P;
using namespace IError::P;             using namespace IFbo::P;
using namespace IFboDef::P;            using namespace IFboItem::P;
using namespace IImage::P;             using namespace IImageDef::P;
using namespace IJson::P;              using namespace ILog::P;
using namespace ILuaIdent::P;          using namespace ILuaLib::P;
using namespace IMemory::P;            using namespace IOgl::P;
using namespace IShader::P;            using namespace IShaders::P;
using namespace IStd::P;               using namespace ISysUtil::P;
using namespace ITexDef::P;            using namespace IUtil::P;
using namespace Lib::OS::GlFW::Types;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* -- Texture collector class for collector data and custom variables ------ */
CTOR_BEGIN_NOBB(Textures, Texture, CLHelperUnsafe)
/* ------------------------------------------------------------------------- */
class TextureBase :                    // All members initially private
  /* -- Base classes ------------------------------------------------------- */
  public FboItem,                      // Fbo item class with drawing co-ords
  public Image                         // Image class with raw image pixel data
{ /* -- Protected variables ------------------------------------- */ protected:
  GLint            iTexMinFilter,      // GL texture minification setting
                   iTexMagFilter,      // GL texture magnification setting
                   iMipmaps;           // Sub-image's are mipmaps (count)
  OglFilterEnum    ofeTexFilter;       // Texture filter (for reference)
  /* -- Tile co-ordinates class ------------------------------------ */ public:
  struct CoordData :                   // All members are public
    /* -- Initialisers ----------------------------------------------------- */
    public DimGLFloat,                 // Dimensions of texture
    public QuadCoordData               // GL quad co-ords of two triangles
  { /* -- 2D tex-coord quad initialisation constructor --------------------- */
    CoordData(const GLfloat fWidth,    // Pixel width of tile   |---Y1---| ^
              const GLfloat fHeight,   // Pixel height of tile  |........| |
              const GLfloat fLeft,     // Normal X1 coord (0-1) X1......X2 H
              const GLfloat fTop,      // Normal Y1 coord (0-1) |........| |
              const GLfloat fRight,    // Normal X2 coord (0-1) |---Y2---| v
              const GLfloat fBottom) : // Normal Y2 coord (0-1) <----W--->
      /* -- Initialisers --------------------------------------------------- */
      DimGLFloat{ fWidth, fHeight },   // Initialise tile pixel dimensions
      QuadCoordData{ {                 // Initialise two GL triangle tex coords
        { fLeft,  fTop,                // [0][0-1] T1 @ XY1    XY1--XY2 XY3
          fRight, fTop,                // [0][2-3] T1 @ XY2     |.../   /|
          fLeft,  fBottom },           // [0][4-5] T1 @ XY3     |T1/   /.|
        { fRight, fBottom,             // [1][0-1] T2 @ XY1     |./   /T2|
          fLeft,  fBottom,             // [1][2-3] T2 @ XY2     |/   /...|
          fRight, fTop } } }           // [1][4-5] T2 @ XY3    XY3 XY2-XY1
      /* -- No code -------------------------------------------------------- */
      { }                              // Note that our shader handles Z co-ord
    /* -- Default constructor ---------------------------------------------- */
    CoordData(void) :                  // No parameters
      /* -- Initialisers --------------------------------------------------- */
      QuadCoordData{ {                 // Default init two GL tri tex coords
        { 0.0f, 0.0f, 0.0f,            // [0][0-2] Tri1\X1, Tri1\Y1, Tri1\X2
          0.0f, 0.0f, 0.0f },          // [0][3-5] Tri1\Y2, Tri1\X3, Tri1\Y3
        { 0.0f, 0.0f, 0.0f,            // [1][0-2] Tri2\X1, Tri2\Y1, Tri2\X2
          0.0f, 0.0f, 0.0f } } }       // [1][3-5] Tri2\Y2, Tri2\X3, Tri2\Y3
      /* -- No code -------------------------------------------------------- */
      { }                              // Note that our shader handles Z co-ord
  };/* --------------------------------------------------------------------- */
  typedef vector<CoordData>   CoordList;   // Tile coordinates data list
  typedef CoordList::iterator CoordListIt; // Iterator to a CoordList
  typedef vector<CoordList>   CoordsList;  // A list of tile coords per sub-tex
  /* ----------------------------------------------------------------------- */
  CoordsList       clTiles;            // Texture coordinates for tiles
  GLUIntVector     uivTexture;         // OpenGL texture handle list
  Shader          *shProgram;          // Default shader program to use
  DimGLUInt        duiTile;            // Texture tile width and height
  DimGLFloat       dfPad,              // Texture tile padding (GL)
                   dfImage,            // Texture image width and height (GL)
                   dfTile;             // Same as duiTile but as a GLfloat
  /* -- Constructor -------------------------------------------------------- */
  explicit TextureBase(const ImageFlagsConst ifcPurpose) :
    /* -- Initialisers ----------------------------------------------------- */
    Image{ ifcPurpose },               // Iniitalise purpose of texture
    iTexMinFilter(GL_NONE),            // No minification filter set yet
    iTexMagFilter(GL_NONE),            // No magnification filter set et
    iMipmaps(0),                       // No mipmaps yet
    ofeTexFilter(OF_N_N),              // No texture filter index set yet
    shProgram(nullptr)                 // No shader programme yet
    /* -- Code ------------------------------------------------------------- */
    { }                                // No code
};/* ----------------------------------------------------------------------- */
CTOR_MEM_BEGIN(Textures, Texture, ICHelperUnsafe, /* No IdentCSlave<> */),
  /* -- Base classes ------------------------------------------------------- */
  public TextureBase                   // Texture class variables
{ /* -- Functors for upload texture function ------------------------------- */
  struct TexCompFtor                   // Keep functors categorised
  { /* -- Load as raw uncompressed pixels ---------------------------------- */
    struct RAW                         // Uniform pixels (R,RG,RGB,RGBA, etc.)
    { /* -- Default constructor -------------------------------------------- */
      RAW(const Texture &tTexture, const size_t stSubTexId,
        const GLint iMipLevel, const TextureType ttNICFormat,
        const TextureType ttNXCFormat, const ImageSlot &isSlot)
      { // Upload uncompressed texture to video ram
        GL(cOgl->UploadTextureTT(iMipLevel, isSlot.DimGetWidth<GLsizei>(),
          isSlot.DimGetHeight<GLsizei>(), ttNICFormat, ttNXCFormat,
          isSlot.MemPtr()),
          "Could not upload uncompressed texture to video ram!",
          "Identifier", tTexture.IdentGet(),  "Index", stSubTexId,
          "TexId",      tTexture.GetSubName(stSubTexId),
          "Level",      iMipLevel,            "Width", isSlot.DimGetWidth(),
          "Height",     isSlot.DimGetHeight(),
          "XCFormat",   ImageGetPixelFormat(tTexture.GetPixelType()),
          "NICFormat",  ImageGetPixelFormat(ttNICFormat),
          "NXCFormat",  ImageGetPixelFormat(ttNXCFormat),
          "Size",       isSlot.MemSize(),     "Data",  isSlot.MemPtr<void>());
      }
    };
    /* -- Load as compressed dxt pixels ------------------------------------ */
    struct DXT                         // DXT1, DXT3 or DXT5
    { /* -- Default constructor -------------------------------------------- */
      DXT(const Texture &tTexture, const size_t stSubTexId,
        const GLint iMipLevel, const TextureType, const TextureType,
        const ImageSlot &isSlot)
      { // Upload pre-compressed texture to video ram
        GL(cOgl->UploadCompressedTextureTT(iMipLevel, tTexture.GetPixelType(),
          isSlot.DimGetWidth<GLsizei>(), isSlot.DimGetHeight<GLsizei>(),
          isSlot.MemSize<GLsizei>(), isSlot.MemPtr()),
          "Could not upload compressed texture to video ram!",
          "Identifier", tTexture.IdentGet(), "Index", stSubTexId,
          "TexId",      tTexture.GetSubName(stSubTexId),
          "Level",      iMipLevel,           "Width", isSlot.DimGetWidth(),
          "Height",     isSlot.DimGetHeight(),
          "XCFormat",   ImageGetPixelFormat(tTexture.GetPixelType()),
          "Size",       isSlot.MemSize(),    "Data",  isSlot.MemPtr<void>());
      }
    };
  };
  /* -- Do create textures ------------------------------------------------- */
  void CreateTextureHandles(const size_t stCount)
  { // Resize array to fit texture names
    uivTexture.resize(stCount);
    // Make OpenGL to generate texture names and throw error if failed
    GL(cOgl->CreateTextures(uivTexture), "Failed to generate textures!",
      "Identifier", IdentGet(), "Count", stCount);
  }
  /* -- Generate mipmaps if needed ----------------------------------------- */
  void ReGenerateMipmaps(void)
  { // Only need to generate mipmaps if we're actually using mipmapping
    switch(iTexMinFilter)
    { // Any of the mipmapping settings? Generate mipmaps
      case GL_LINEAR_MIPMAP_LINEAR:  case GL_LINEAR_MIPMAP_NEAREST:
      case GL_NEAREST_MIPMAP_LINEAR: case GL_NEAREST_MIPMAP_NEAREST:
        GL(cOgl->GenerateMipmaps(),
          "Failed to generate mipmaps!", "Identifier", IdentGet());
      // Nothing special
      default: break;
    }
  }
  /* -- Do load texture from image class ----------------------------------- */
  template<class TexCompFtor>
    void UploadTexture(const size_t stSlots, const ImageSlot &isSlot,
      const TextureType ttNICFormat, const TextureType ttNXCFormat)
  { // Reset previous marked for deletion flag
    FlagClear(TF_DELETE);
    // If no mipmaps in this bitmap?
    if(IsNotMipmaps())
    { // Image does not contain mipmaps?. No mipmaps
      iMipmaps = 0;
      // Create texture handles
      CreateTextureHandles(stSlots);
      // For each image slot
      for(size_t stSubTexId = 0; stSubTexId < stSlots; ++stSubTexId)
      { // Get next slot and verify that dimensions are different from slot 0
        const ImageSlot &isRef = GetSlots()[stSubTexId];
        if(DimGetWidth() != isRef.DimGetWidth() ||
           DimGetHeight() != isRef.DimGetHeight())
          XC("Alternating image sizes are not supported!",
             "Identifier", IdentGet(),     "LastWidth", DimGetWidth(),
             "LastHeight", DimGetHeight(), "ThisWidth", isRef.DimGetWidth(),
             "ThisHeight", isRef.DimGetHeight());
        // Get texture id and bind it
        const unsigned int uiTexId = GetSubName(stSubTexId);
        GL(cOgl->BindTexture(uiTexId), "Texture id failed to bind!",
          "Identifier", IdentGet(), "Index", stSubTexId,
          "Maximum",    stSlots,    "TexId", uiTexId);
        // Configure the texture
        ConfigureTexture(stSubTexId);
        // Load the image
        TexCompFtor(*this, stSubTexId, 0, ttNICFormat, ttNXCFormat, isRef);
        // Automatically generate mipmaps if requested. This has to be done
        // for each uploaded texture
        ReGenerateMipmaps();
      }
    } // Bitmap has mipmaps?
    else
    { // Record number of mipmaps we have
      iMipmaps = UtilIntOrMax<GLint>(stSlots);
      // Create only one texture handle as other slots are for mipmaps
      CreateTextureHandles(1);
      // Get texture id
      const unsigned int uiTexId = GetSubName();
      // Bind the texture id
      GL(cOgl->BindTexture(uiTexId), "Mipmapped texture failed to bind!",
        "Identifier", IdentGet(), "TexId", uiTexId);
      // Initialise mipmap counter
      GL(cOgl->SetTexParam(GL_TEXTURE_BASE_LEVEL, 0),
        "Failed to set max mipmap base level on texture!",
        "Identifier", IdentGet(), "TexId", uiTexId);
      GL(cOgl->SetTexParam(GL_TEXTURE_MAX_LEVEL, GetMipmaps()),
        "Failed to set max mipmap max level on texture!",
        "Identifier", IdentGet(), "TexId", uiTexId, "Count", GetMipmaps());
      // Configure the texture
      ConfigureTexture(0);
      // Upload first mipmap
      TexCompFtor(*this, 0, 0, ttNICFormat, ttNXCFormat, isSlot);
      // Done if there more slots to load
      if(stSlots <= 1) return;
      // Last mipmap size
      unsigned int uiLWidth = DimGetWidth(), uiLHeight = DimGetHeight();
      // Load all the other mipmaps
      for(size_t stSubTexId = 1; stSubTexId < stSlots; ++stSubTexId)
      { // Get next slot
        const ImageSlot &isRef = GetSlotsConst()[stSubTexId];
        // Make sure size is smaller than the last
        if(isRef.DimGetWidth() >= uiLWidth ||
           isRef.DimGetHeight() >= uiLHeight)
          XC("Specified mipmap is not smaller than the last!",
             "Identifier", IdentGet(), "LastWidth", uiLWidth,
             "LastHeight", uiLHeight,  "ThisWidth", isRef.DimGetWidth(),
             "ThisHeight", isRef.DimGetHeight());
        // Load the mipmap
        TexCompFtor(*this, 0, static_cast<GLint>(stSubTexId), ttNICFormat,
          ttNXCFormat, isRef);
        // Update last mipmap size
        uiLWidth = DimGetWidth();
        uiLHeight = DimGetHeight();
      }
    } // Log progress
    cLog->LogDebugExSafe("Texture '$'[S:$;F:$/$;M:$;D:$x$] uploaded.",
      IdentGet(), stSlots, ImageGetPixelFormat(ttNXCFormat),
      ImageGetPixelFormat(ttNICFormat), GetMipmaps(), isSlot.DimGetWidth(),
      isSlot.DimGetHeight());
  }
  /* -- Return a new texcoord tile converting 2D texture coords to 3D ------ */
  const CoordData NewTile(const GLfloat fLeft, const GLfloat fTop,
    const GLfloat fRight, const GLfloat fBottom, const GLfloat fWidth,
    const GLfloat fHeight)
      { return { fWidth, fHeight, fLeft / dfImage.DimGetWidth(),
          1.0f - fTop / dfImage.DimGetHeight(), fRight / dfImage.DimGetWidth(),
          1.0f - fBottom / dfImage.DimGetHeight() }; }
  /* -- Set the texture co-ordinates of a tile ----------------------------- */
  void SetTile(const size_t stSubTexId, const size_t stTileId,
    const GLfloat fLeft, const GLfloat fTop, const GLfloat fRight,
    const GLfloat fBottom, const GLfloat fWidth, const GLfloat fHeight)
      { clTiles[stSubTexId][stTileId] =
          NewTile(fLeft, fTop, fRight, fBottom, fWidth, fHeight); }
  /* -- Set the texture co-ordinates of a tile ----------------------------- */
  void SetTile(const size_t stSubTexId, const size_t stTileId,
    const GLfloat fLeft, const GLfloat fTop, const GLfloat fRight,
    const GLfloat fBottom)
      { SetTile(stSubTexId, stTileId, fLeft, fTop, fRight, fBottom,
          fRight - fLeft, fBottom - fTop); }
  /* -- Set the texture co-ordinates of a tile that has reversed scanlines - */
  void SetTileR(const size_t stSubTexId, const size_t stTileId,
    const GLfloat fLeft, const GLfloat fTop, const GLfloat fRight,
    const GLfloat fBottom)
      { const GLfloat fNTop = dfImage.DimGetHeight() - fTop,
                      fNBottom = dfImage.DimGetHeight() - fBottom;
        SetTile(stSubTexId, stTileId, fLeft, fNTop,
          fRight, fNBottom, fRight - fLeft, fNTop - fNBottom); }
  /* -- Add a tile with width and height ----------------------------------- */
  void SetTileWH(const size_t stSubTexId, const size_t stTileId,
    const GLfloat fLeft, const GLfloat fTop, const GLfloat fWidth,
    const GLfloat fHeight)
      { const GLfloat fNRight = fLeft + fWidth,
                      fNBottom = fTop + fHeight;
        SetTile(stSubTexId, stTileId, fLeft, fTop, fNRight, fNBottom,
          fNRight - fLeft, fNBottom - fTop); }
  /* -- Add a reversed tile with width and height setting ------------------ */
  void SetTileRWH(const size_t stSubTexId, const size_t stTileId,
    const GLfloat fLeft, const GLfloat fTop, const GLfloat fWidth,
    const GLfloat fHeight)
      { const GLfloat fNTop = dfImage.DimGetHeight()-fTop,
                      fNBottom = fNTop + fHeight,
                      fNRight = fLeft + fWidth;
        SetTile(stSubTexId, stTileId, fLeft, fTop, fNRight, fNBottom,
          fNRight - fLeft, fNTop - fNBottom); }
  /* -- Do add a tile with custom width and height setting ----------------- */
  void AddTile(const size_t stSubTexId, const GLfloat fLeft,
    const GLfloat fTop, const GLfloat fRight, const GLfloat fBottom,
    const GLfloat fWidth, const GLfloat fHeight)
      { clTiles[stSubTexId].emplace_back(NewTile(fLeft, fTop, fRight, fBottom,
          fWidth, fHeight)); }
  /* -- Add a tile --------------------------------------------------------- */
  void AddTile(const size_t stSubTexId, const GLfloat fLeft,
    const GLfloat fTop, const GLfloat fRight, const GLfloat fBottom)
      { AddTile(stSubTexId, fLeft, fTop, fRight, fBottom, fRight - fLeft,
          fBottom - fTop); }
  /* -- Add a reversed tile ------------------------------------------------ */
  void AddTileR(const size_t stSubTexId, const GLfloat fLeft,
    const GLfloat fTop, const GLfloat fRight, const GLfloat fBottom)
      { const GLfloat fNTop = dfImage.DimGetHeight() - fTop,
                      fNBottom = dfImage.DimGetHeight() - fBottom;
        AddTile(stSubTexId, fLeft, fNTop, fRight, fNBottom,
          fRight - fLeft, fNTop - fNBottom); }
  /* -- Add a tile with width and height ----------------------------------- */
  void AddTileWH(const size_t stSubTexId, const GLfloat fLeft,
    const GLfloat fTop, const GLfloat fWidth, const GLfloat fHeight)
      { const GLfloat fNRight = fLeft + fWidth,
                      fNBottom = fTop + fHeight;
        AddTile(stSubTexId, fLeft, fTop, fNRight, fNBottom, fNRight - fLeft,
          fNBottom - fTop); }
  /* -- Add a reversed tile with width and height -------------------------- */
  void AddTileRWH(const size_t stSubTexId, const GLfloat fLeft,
    const GLfloat fTop, const GLfloat fWidth, const GLfloat fHeight)
      { const GLfloat fNTop = dfImage.DimGetHeight() - fTop,
                      fNBottom = fNTop + fHeight, fNRight = fLeft+fWidth;
        AddTile(stSubTexId, fLeft, fTop, fNRight, fNBottom, fNRight - fLeft,
          fNTop - fNBottom); }
  /* -- Configure the specified texture id --------------------------------- */
  void ConfigureTexture(const size_t stSubTexId)
  { // Start configuring the texture.
    GL(cOgl->SetTexParam(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE),
      "Could not set texture wrapping S!",
      "Identifier", IdentGet(), "Index", stSubTexId);
    GL(cOgl->SetTexParam(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE),
      "Could not set texture wrapping T!",
      "Identifier", IdentGet(), "Index", stSubTexId);
    GL(cOgl->SetTexParam(GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE),
      "Could not set texture wrapping R!",
      "Identifier", IdentGet(), "Index", stSubTexId);
    // Set filtering based on developers setting
    GL(cOgl->SetTexParam(GL_TEXTURE_MIN_FILTER, iTexMinFilter),
      "Could not set texture minifaction filter!",
      "Identifier", IdentGet(), "Index", stSubTexId,
      "MinFilter",  iTexMinFilter);
    GL(cOgl->SetTexParam(GL_TEXTURE_MAG_FILTER, iTexMagFilter),
      "Could not set texture magnification filter!",
      "Identifier", IdentGet(), "Index", stSubTexId,
      "MagFilter",  iTexMagFilter);
  }
  /* -- Load texture from image class -------------------------------------- */
  void LoadFromImage(void)
  { // Get number of slots in this image and return if zero
    if(IsNoSlots()) XC("No data in image object!", "Identifier", IdentGet());
    // Get first slot
    const ImageSlot &isSlot = GetSlotsConst().front();
    // Copy image dimensions to texture dimensions
    DimSet(isSlot);
    // Set image dimensions as float for opengl
    dfImage.DimSet(DimGetWidth<GLfloat>(), DimGetHeight<GLfloat>());
    // Check width and height's are valid for the graphics device
    const unsigned int uiMaxSize = cOgl->MaxTexSize();
    if(isSlot.DimIsNotSet() || isSlot.DimGetWidth() > uiMaxSize ||
       isSlot.DimGetHeight() > uiMaxSize)
      XC("Image dimensions not supported by graphics hardware!",
         "Identifier", IdentGet(),            "Width",   isSlot.DimGetWidth(),
         "Height",     isSlot.DimGetHeight(), "Maximum", uiMaxSize);
    // Lookup internal pixel type chosen and check it
    const TextureType ttICFormat = ImageBYtoTexType(GetBytesPerPixel());
    if(ttICFormat == TT_NONE)
      XC("Unsupported texture colour type!",
         "Identifier",   IdentGet(), "BytesPerPixel", GetBytesPerPixel(),
         "BitsPerPixel", GetBitsPerPixel());
    // Get usable slots
    const size_t stSlots = GetSlotCount() - static_cast<size_t>(IsPalette());
    // Calculated internal and external format
    TextureType ttNXCFormat;
    // Because we're using shaders we can trick OpenGL into giving it a
    // non-standard colour encoded raster image and the shaders can decode
    // it on the fly. This variable will redefine the external colour format
    // if neccesary on anything other than a RGB or RGBA image.
    switch(ttICFormat)
    { // Texture is 8-bpp?
      case TT_GRAY:
        // Is palleted?
        if(IsPalette())
        { // Set palette shader
          shProgram = &cShaderCore->sh2D8Pal;
          // Force no filtering and no mipmapping or we get the wrong colours
          ofeTexFilter = OF_N_N;
          iTexMinFilter = iTexMagFilter = GL_NEAREST;
        } // No palette
        else shProgram = &cShaderCore->sh2D8;
        // Set requested external format
        ttNXCFormat = GetPixelType();
        // Break to upload raw pixels
        break;
      // Texture is 16-bpp?
      case TT_GRAYALPHA:
        // Set GL_LUMINANCE_ALPHA decoding shader
        shProgram = &cShaderCore->sh2D16;
        ttNXCFormat = GetPixelType();
        // Break to upload raw pixels
        break;
      // Texture is 24 or 32-bpp?
      case TT_RGB: case TT_RGBA:
        // Whats the external format?
        switch(GetPixelType())
        { // Compressed texture?
          case TT_DXT1: case TT_DXT3: case TT_DXT5:
            // Set compressed texture
            ttNXCFormat = GetPixelType();
            shProgram = &cShaderCore->sh2D;
            // Upload as compressed texture
            UploadTexture<TexCompFtor::DXT>
              (stSlots, isSlot, ttICFormat, ttNXCFormat);
            // Return because we uploaded compressed pixels
            return;
          // BGRA colour order type?
          case TT_BGRA:
            // Use BGR shader and redefine to RGBA.
            shProgram = &cShaderCore->sh2DBGR;
            ttNXCFormat = TT_RGBA;
            // Break to upload raw pixels
            break;
          // BGR colour order type?
          case TT_BGR:
            // Use BGR shader and redefine to RGB.
            shProgram = &cShaderCore->sh2DBGR;
            ttNXCFormat = TT_RGB;
            // Break to upload raw pixels
            break;
          // RGBA or RGB (Normal image).
          case TT_RGB: case TT_RGBA:
            // Use RGB shader. No format change.
            shProgram = &cShaderCore->sh2D;
            ttNXCFormat = GetPixelType();
            // Break to upload raw pixels
            break;
          // Unknown external format
          default: XC("External colour type not acceptable!",
            "Identifier", IdentGet(), "XCFormat", GetPixelType());
        } // Done
        break;
      // Unknown internal format
      default: XC("Internal colour type not acceptable!",
        "Identifier", IdentGet(), "ICFormat", ttICFormat,
        "XCFormat", GetPixelType());
    } // The pixel type is raw uniform pixels so upload them
    UploadTexture<TexCompFtor::RAW>(stSlots, isSlot, ttICFormat, ttNXCFormat);
  }
  /* -- Return tile dimensions as float ------------------------- */ protected:
  GLfloat GetTileWidthFloat(void) const { return dfTile.DimGetWidth(); }
  GLfloat GetTileHeightFloat(void) const { return dfTile.DimGetHeight(); }
  /* -- Return padding dimensions ---------------------------------- */ public:
  GLfloat GetPaddingWidth(void) const { return dfPad.DimGetWidth(); }
  GLfloat GetPaddingHeight(void) const { return dfPad.DimGetHeight(); }
  /* -- Return tile dimensions --------------------------------------------- */
  template<typename IntType=GLuint>IntType GetTileWidth(void) const
    { return duiTile.DimGetWidth<IntType>(); }
  template<typename IntType=GLuint>IntType GetTileHeight(void) const
    { return duiTile.DimGetHeight<IntType>(); }
  /* -- Return number of tiles --------------------------------------------- */
  size_t GetTileCount(const size_t stSubTexId=0) const
    { return clTiles[stSubTexId].size(); }
  /* -- Set tile count ----------------------------------------------------- */
  void SetTileCount(const size_t stCount, const size_t stSubTexId=0)
    { clTiles[stSubTexId].resize(stCount); }
  /* -- Return the number of mipmaps in the texture ------------------------ */
  GLint GetMipmaps(void) const { return iMipmaps; }
  /* -- Return the current texture filter index setting -------------------- */
  OglFilterEnum GetTexFilter(void) const { return ofeTexFilter; }
  /* -- Return the OpenGL texture name for the specified sub-textures ------ */
  GLuint GetSubName(const size_t stSubTexId=0) const
    { return uivTexture[stSubTexId]; }
  /* -- Return number of sub-textures -------------------------------------- */
  size_t GetSubCount(void) const { return uivTexture.size(); }
  /* -- Check if texture is initialised ------------------------------------ */
  bool IsNotInitialised(void) const { return uivTexture.empty(); }
  bool IsInitialised(void) const { return !IsNotInitialised(); }
  /* -- Set a tile based on reversal --------------------------------------- */
  void SetTileDOR(const size_t stSubTexId, const size_t stTileId,
    const GLfloat fLeft, const GLfloat fTop, const GLfloat fRight,
    const GLfloat fBottom)
      { if(IsReversed())
          SetTileR(stSubTexId, stTileId, fLeft, fTop, fRight, fBottom);
        else SetTile(stSubTexId, stTileId, fLeft, fTop, fRight, fBottom); }
  /* -- Set a tile based on reversal with width and height setting --------- */
  void SetTileDORWH(const size_t stSubTexId, const size_t stTileId,
    const GLfloat fLeft, const GLfloat fTop, const GLfloat fWidth,
    const GLfloat fHeight)
      { if(IsReversed())
          SetTileRWH(stSubTexId, stTileId, fLeft, fTop, fWidth, fHeight);
        else SetTileWH(stSubTexId, stTileId, fLeft, fTop, fWidth, fHeight); }
  /* -- Add a tile based on reversal setting ------------------------------- */
  void AddTileDOR(const size_t stSubTexId, const GLfloat fLeft,
    const GLfloat fTop, const GLfloat fRight, const GLfloat fBottom)
      { if(IsReversed()) AddTileR(stSubTexId, fLeft, fTop, fRight, fBottom);
        else AddTile(stSubTexId, fLeft, fTop, fRight, fBottom); }
  /* -- Add a tile with width and height based on reversal ----------------- */
  void AddTileDORWH(const size_t stSubTexId, const GLfloat fLeft,
    const GLfloat fTop, const GLfloat fWidth, const GLfloat fHeight)
      { if(IsReversed()) AddTileRWH(stSubTexId, fLeft, fTop, fWidth, fHeight);
        else AddTileWH(stSubTexId, fLeft, fTop, fWidth, fHeight); }
  /* -- Blit a triangle ---------------------------------------------------- */
  void BlitTri(const GLuint uiGLTexId, const TriCoordData &tcoTex,
    const TriPosData &tpdVert, const TriColData &tcdClr)
      { FboActive()->FboBlit(uiGLTexId,
          tpdVert, tcoTex, tcdClr, 0, shProgram); }
  /* -- Blit two triangles that form a square ------------------------------ */
  void BlitQuad(const GLuint uiGLTexId, const QuadCoordData &qcoVert,
    const QuadPosData &qpdTex, const QuadColData &qcdClr)
      { for(size_t stTriId = 0; stTriId < stTrisPerQuad; ++stTriId)
          BlitTri(uiGLTexId, qcoVert[stTriId], qpdTex[stTriId],
            qcdClr[stTriId]); }
  /* -- Blit with currently stored position, texture and colour ------------ */
  void Blit(const size_t stSubTexId)
    { BlitQuad(GetSubName(stSubTexId), FboItemGetTCData(), FboItemGetVData(),
        FboItemGetCData()); }
  /* -- Blit specified triangle with currently stored position ------------- */
  void BlitT(const size_t stTriId, const size_t stTexId, const size_t stTileId)
    { BlitTri(GetSubName(stTexId), clTiles[stTexId][stTileId][stTriId],
        FboItemGetVData(stTriId), FboItemGetCData(stTriId)); }
  /* -- Blit quad with position and stored size ---------------------------- */
  void BlitLT(const size_t stSubTexId, const size_t stTileId,
    const GLfloat fLeft, const GLfloat fTop)
      { const CoordData &cdTex = clTiles[stSubTexId][stTileId];
        BlitQuad(GetSubName(stSubTexId), cdTex,
          FboItemSetAndGetVertex(fLeft, fTop, fLeft + cdTex.DimGetWidth(),
            fTop + cdTex.DimGetHeight()), FboItemGetCData()); }
  /* -- Blit quad with custom colour (used by font) ------------------------ */
  void BlitLTRBC(const size_t stSubTexId, const size_t stTileId,
    const GLfloat fLeft, const GLfloat fTop, const GLfloat fRight,
    const GLfloat fBottom, const QuadColData &qcdClr)
      { BlitQuad(GetSubName(stSubTexId), clTiles[stSubTexId][stTileId],
          FboItemSetAndGetVertex(fLeft, fTop, fRight, fBottom), qcdClr); }
  /* -- Blit quad with bounds ---------------------------------------------- */
  void BlitLTRB(const size_t stSubTexId, const size_t stTileId,
    const GLfloat fLeft, const GLfloat fTop, const GLfloat fRight,
    const GLfloat fBottom)
      { BlitLTRBC(stSubTexId, stTileId, fLeft, fTop, fRight, fBottom,
          FboItemGetCData()); }
  /* -- Blit quad with coords and dimensions ------------------------------- */
  void BlitLTWH(const size_t stSubTexId, const size_t stTileId,
    const GLfloat fLeft, const GLfloat fTop, const GLfloat fWidth,
    const GLfloat fHeight)
      { BlitLTRB(stSubTexId, stTileId, fLeft, fTop,
          fLeft + fWidth, fTop + fHeight); }
  /* -- Blit quad with truncated texcoord width and colour ----------------- */
  void BlitLTRBSC(const size_t stSubTexId, const size_t stTileId,
    const GLfloat fLeft, const GLfloat fTop, const GLfloat fRight,
    const GLfloat fBottom, const GLfloat fLeftEdge, const GLfloat fRightEdge,
    const QuadColData &qcdClr)
      { BlitQuad(GetSubName(stSubTexId),
          FboItemSetAndGetCoord(clTiles[stSubTexId][stTileId],
            fLeftEdge, fRightEdge),
          FboItemSetAndGetVertex(fLeft, fTop, fRight, fBottom,
            fLeftEdge, fRightEdge),
          qcdClr); }
  /* -- Blit quad with truncated texcoord width ---------------------------- */
  void BlitLTRBS(const size_t stSubTexId, const size_t stTileId,
    const GLfloat fLeft, const GLfloat fTop, const GLfloat fRight,
    const GLfloat fBottom, const GLfloat fLeftEdge, const GLfloat fRightEdge)
      { BlitLTRBSC(stSubTexId, stTileId, fLeft, fTop, fRight, fBottom,
          fLeftEdge, fRightEdge, FboItemGetCData()); }
  /* -- Blit quad with position, stored size and with angle ---------------- */
  void BlitLTA(const size_t stSubTexId, const size_t stTileId,
    const GLfloat fLeft, const GLfloat fTop, const GLfloat fAngle)
      { const CoordData &cdTex = clTiles[stSubTexId][stTileId];
        BlitQuad(GetSubName(stSubTexId), cdTex,
          FboItemSetAndGetVertex(fLeft, fTop, fLeft + cdTex.DimGetWidth(),
            fTop + cdTex.DimGetHeight(), fAngle), FboItemGetCData()); }
  /* -- Blit quad with coords, dimensions and angle ------------------------ */
  void BlitLTWHA(const size_t stSubTexId, const size_t stTileId,
    const GLfloat fLeft, const GLfloat fTop, const GLfloat fWidth,
    const GLfloat fHeight, const GLfloat fAngle)
      { BlitQuad(GetSubName(stSubTexId), clTiles[stSubTexId][stTileId],
          FboItemSetAndGetVertex(fLeft, fTop, fLeft + fWidth, fTop + fHeight,
            fAngle),
          FboItemGetCData()); }
  /* -- Blit all quads as full image --------------------------------------- */
  void BlitMulti(const GLuint uiColumns, const GLfloat fLeft,
    const GLfloat fTop, const GLfloat fRight, const GLfloat fBottom)
  { // Calculate perspective width
    const GLfloat
      fNewWidth = (fRight - fLeft) / uiColumns,
      fNewHeight = (fBottom - fTop) / (GetTileCount() / uiColumns);
    // For each image
    for(size_t stTile = 0; stTile < GetTileCount(); ++stTile)
    { // Calculate X, Y co-ordinate
      const GLfloat
        fNewLeft = fLeft + (stTile % uiColumns) * fNewWidth,
        fNewTop = fTop + (stTile / uiColumns) * fNewHeight;
      // Blit the texture
      BlitLTRB(0, stTile, fNewLeft, fNewTop,
        fNewLeft + fNewWidth, fNewTop + fNewHeight);
    }
  }
  /* -- Replace partial texture in VRAM from raw data ---------------------- */
  void UpdateEx(const GLuint uiTexId, const GLint iLeft, const GLint iTop,
    const GLsizei siWidth, const GLsizei siHeight, const TextureType ttPixType,
    const GLvoid*const vpData)
  { // Bind the specified texture
    GL(cOgl->BindTexture(uiTexId), "Failed to bind texture to update!",
      "Identifier", IdentGet(), "TexId", uiTexId);
    // Upload the texture area
    GL(cOgl->UploadTextureSubTT(iLeft, iTop,
         siWidth, siHeight, ttPixType, vpData),
      "Failed to update VRAM with image!",
      "Identifier", IdentGet(), "OffsetX", iLeft,
      "OffsetY",    iTop,       "Width",   siWidth,
      "Height",     siHeight,   "SrcType", ImageGetPixelFormat(ttPixType));
    // Whats the minification value? We might need to regenerate mipmaps! If
    // we already had mipmaps, they will be overwritten.
    ReGenerateMipmaps();
    // Write that we updated the VRAM
    cLog->LogDebugExSafe("Texture '$'[$<$x$>] updated at $x$ with type $!",
      IdentGet(), uiTexId, siWidth, siHeight, iLeft, iTop,
      ImageGetPixelFormat(ttPixType));
  }
  /* -- Replace partial texture in VRAM from partial raw data -------------- */
  void UpdateEx(const GLuint uiTexId, const GLint iLeft, const GLint iTop,
    const GLsizei siWidth, const GLsizei siHeight, const TextureType ttPixType,
    const GLvoid*const vpData, const GLsizei siStride)
  { // Set stride and alignment
    GL(cOgl->SetUnpackRowLength(siStride), "Failed to set unpack row length!",
      "Texture", IdentGet(), "Stride", siStride);
    // Update the texture
    UpdateEx(uiTexId, iLeft, iTop, siWidth, siHeight, ttPixType, vpData);
    // Reset stride and alignment
    GL(cOgl->SetUnpackRowLength(cOgl->UnpackRowLength()),
      "Failed to restore unpack row length!",
      "Texture", IdentGet(), "Stride", cOgl->UnpackRowLength());
  }
  /* -- Replace partial texture in VRAM from array ------------------------- */
  void UpdateEx(const size_t stSubTexId, Image &imImage,
    const GLint iLeft, const GLint iTop)
  { // Do the update
    UpdateEx(GetSubName(stSubTexId), iLeft, iTop,
      imImage.DimGetWidth<GLsizei>(), imImage.DimGetHeight<GLsizei>(),
      imImage.GetPixelType(), imImage.GetSlotsConst().front().MemPtr());
  }
  /* -- Replace texture in VRAM from array --------------------------------- */
  void Update(Image &imOther)
  { // Deinit existing texture
    DeInit();
    // Replace image
    SwapImage(imOther);
    // Reload texture
    ReloadTexture();
  }
  /* -- Download texture to array ------------------------------------------ */
  Image Download(const size_t stSubTexId, const BitDepth bdDDepth=BD_RGBA,
    const ByteDepth byDDepth=BY_RGBA) const
  { // Get texture id and bind it
    GL(cOgl->BindTexture(GetSubName(stSubTexId)),
      "Failed to bind texture to download!",
      "Identifier", IdentGet(), "Index", stSubTexId);
    // Create memory block for texture data and read RGBA texture into it
    Memory mOut{ DimGetWidth() * DimGetHeight() * byDDepth };
    const TextureType ttPixType = ImageBYtoTexType(byDDepth);
    GL(cOgl->ReadTextureTT(ttPixType, mOut.MemPtr<GLvoid>()),
      "Download texture failed!",
      "Identifier", IdentGet(), "Index", stSubTexId, "Buffer", mOut.MemSize(),
      "Format",     ImageGetPixelFormat(ttPixType));
    // Return a newly created image class containing this data
    return Image{ IdentGet(), StdMove(mOut), DimGetWidth(), DimGetHeight(),
      bdDDepth };
  }
  /* -- Download texture and dump it to disk ------------------------------- */
  void Dump(const size_t stSubTexId, const string &strFile) const
    { Download(stSubTexId).SaveFile(strFile, stSubTexId, IFMT_PNG); }
  /* -- Reload texture array as normal texture ----------------------------- */
  void ReloadTexture(void)
  { // If image was not loaded from disk? Just (re)load the image data
    // that already should be there and should NEVER be released.
    if(IsDynamic()) LoadFromImage();
    // Texture is static?
    else
    { // It can just be reloaded from disk
      ReloadData();
      // Load the image into memory
      LoadFromImage();
      // Remove all image data because we can just load it from file again
      // and theres no point taking up precious memory for it.
      ClearSlots();
    } // Error if no textures loaded
    if(IsNotInitialised()) XC("No textures loaded!", "Identifier", IdentGet());
    // Show what was loaded
    cLog->LogInfoExSafe("Texture loaded $ textures from '$'!",
      GetSubCount(), IdentGet());
  }
  /* -- Init from a image class -------------------------------------------- */
  void InitTextureImage(Image &imgSrc, const GLuint uiTileWidth,
    const GLuint uiTileHeight, const GLuint uiPadX, const GLuint uiPadY,
    const OglFilterEnum ofeFilter, const bool bGenerateTileset = true)
  { // Show filename
    cLog->LogDebugExSafe("Texture loading from image '$'.", imgSrc.IdentGet());
    // If source and destination image class are not the same?
    if(this != &imgSrc)
    { // Move image data over. The old image will be unusable so guest should
      // discard it.
      SwapImage(imgSrc);
      // The image passed in the arguments is usually still allocated by LUA
      // and will still be registered, so lets put a note in the image to show
      // that this function has nicked by this image class.
      imgSrc.IdentSetEx("!TEX!$!", IdentGet());
    } // We'll set float versions for faster calculations later on
    dfPad.DimSet(static_cast<GLfloat>(uiPadX), static_cast<GLfloat>(uiPadY));
    // Set filter
    ofeTexFilter = ofeFilter;
    cOgl->SetMipMapFilterById(ofeFilter, iTexMinFilter, iTexMagFilter);
    // Initialise
    LoadFromImage();
    // Set specified tile dimensions and generate default tileset if needed
    duiTile.DimSet(uiTileWidth, uiTileHeight);
    if(bGenerateTileset)
    { // Resize co-ords list to the number of usable slots we loaded
      clTiles.resize(GetSubCount());
      // Set override tile size from Image loader if specified
      if(duTileOR.DimIsSet()) duiTile.DimSet(duTileOR);
      // Else clamp bounds to image size if unspecified or invalid size
      else if(duiTile.DimIsNotSet() ||
              GetTileWidth() > DimGetWidth() ||
              GetTileHeight() > DimGetHeight())
        duiTile.DimSet(*this);
      // Set tile dimensions as opengl float
      dfTile.DimSet(GetTileWidth<GLfloat>(), GetTileHeight<GLfloat>());
      // Create clamped image size
      const GLfloat
        fTPSizeX = dfTile.DimGetWidth() + GetPaddingWidth(),
        fTPSizeY = dfTile.DimGetHeight() + GetPaddingHeight(),
        fBTSizeX = floorf(dfImage.DimGetWidth() / fTPSizeX) * fTPSizeX,
        fBTSizeY = floorf(dfImage.DimGetHeight() / fTPSizeY) * fTPSizeY;
      // Calculate number of tile needed
      const size_t stTilesMax = static_cast<size_t>(
        floorf(fBTSizeX / fTPSizeX) * floorf(fBTSizeY / fTPSizeY));
      // If there are a limited number of tiles? Enumerate slots...
      if(stTiles) for(size_t stSubTexId = 0;
                             stSubTexId < GetSubCount();
                           ++stSubTexId)
      { // Make sure theres enough memory allocated for each coord data
        clTiles[stSubTexId].reserve(stTiles);
        // Get tile count
        const size_t stTileCount = GetTileCount(stSubTexId);
        // Image is reversed?
        if(IsReversed())
          for(GLfloat fTop = 0.0f;
                      fTop < fBTSizeY && stTileCount < stTiles;
                      fTop += fTPSizeY)
            for(GLfloat fLeft = 0.0f;
                        fLeft < fBTSizeX && stTileCount < stTiles;
                        fLeft += fTPSizeX)
              AddTileR(stSubTexId, fLeft, fTop,
                fLeft + dfTile.DimGetWidth(), fTop + dfTile.DimGetHeight());
        // Not reversed
        else for(GLfloat fTop = 0.0f;
                         fTop < fBTSizeY && stTileCount < stTiles;
                         fTop += fTPSizeY)
          for(GLfloat fLeft = 0.0f;
                      fLeft < fBTSizeX && stTileCount < stTiles;
                      fLeft += fTPSizeX)
            AddTile(stSubTexId, fLeft, fTop,
              fLeft + dfTile.DimGetWidth(), fTop + dfTile.DimGetHeight());
        // Recover memory
        clTiles[stSubTexId].shrink_to_fit();
      } // No tiles overwrite
      else for(size_t stSubTexId = 0;
                      stSubTexId < GetSubCount();
                    ++stSubTexId)
      { // Make sure theres enough memory allocated for each coord data
        clTiles[stSubTexId].reserve(stTilesMax);
        // Image is reversed?
        if(IsReversed())
          for(GLfloat fTop = 0.0f; fTop < fBTSizeY; fTop += fTPSizeY)
            for(GLfloat fLeft = 0.0f; fLeft < fBTSizeX; fLeft += fTPSizeX)
              AddTileR(stSubTexId, fLeft, fTop,
                fLeft + dfTile.DimGetWidth(), fTop + dfTile.DimGetHeight());
        // Not reversed
        else for(GLfloat fTop = 0.0f; fTop < fBTSizeY; fTop += fTPSizeY)
          for(GLfloat fLeft = 0.0f; fLeft < fBTSizeX; fLeft += fTPSizeX)
            AddTile(stSubTexId, fLeft, fTop,
              fLeft + dfTile.DimGetWidth(), fTop + dfTile.DimGetHeight());
        // Recover memory
        clTiles[stSubTexId].shrink_to_fit();
      } // Log tiling data
      cLog->LogDebugExSafe("- Tiles: $ (SC=$;B=$x$;T=$x$;P=$$x$).",
        GetTileCount(), GetSubCount(), DimGetWidth(), DimGetHeight(),
        GetTileWidth(), GetTileHeight(), fixed, GetPaddingWidth(),
        GetPaddingHeight());
    }
    // Remove all image data because we can just load it from file again
    // and theres no point taking up precious memory for it.
    if(IsNotDynamic()) ClearSlots();
  }
  /* -- Init from a image class -------------------------------------------- */
  void InitTextureImageTiles(Image &imgSrc, const OglFilterEnum ofeFilter,
    const GLUIntVector gluvTiles)
  { // Generate the texture as normal but we'll be generating the tileset
    InitTextureImage(imgSrc, 0, 0, 0, 0, ofeFilter, false);
    // Check that the tile count is divisble by 4 (X,Y,W,H)
    const size_t stExcess = gluvTiles.size() % 4;
    if(stExcess)
      XC("Invalid count of tiles specified!",
         "Identifier", IdentGet(), "Count", stExcess);
    // Reserve memory for actual count
    clTiles.resize(1);
    CoordList &clFirst = clTiles.front();
    clFirst.reserve(gluvTiles.size() / 4);
    // Shortcut to GLUIntVector const iterator
    typedef GLUIntVector::const_iterator GLUIntVectorItConst;
    // Are image pixels reversed?
    if(IsReversed())
      // Until there are no more values to parse
      for(GLUIntVectorItConst gluvicIt{ gluvTiles.cbegin() };
                              gluvicIt != gluvTiles.cend();
                            ++gluvicIt)
      { // Add the user specified tile
        const GLfloat fX      = static_cast<GLfloat>(*gluvicIt),
                      fY      = static_cast<GLfloat>(*(++gluvicIt)),
                      fWidth  = static_cast<GLfloat>(*(++gluvicIt)),
                      fHeight = static_cast<GLfloat>(*(++gluvicIt));
        AddTileRWH(0, fX, fY, fWidth, fHeight);
      }
    // Image pixels are not reversed?
    else
      // Until there are no more values to parse
      for(GLUIntVectorItConst gluvicIt{ gluvTiles.cbegin() };
                              gluvicIt != gluvTiles.cend();
                            ++gluvicIt)
      { // Add the user specified tile
        const GLfloat fX      = static_cast<GLfloat>(*gluvicIt),
                      fY      = static_cast<GLfloat>(*(++gluvicIt)),
                      fWidth  = static_cast<GLfloat>(*(++gluvicIt)),
                      fHeight = static_cast<GLfloat>(*(++gluvicIt));
        AddTileWH(0, fX, fY, fWidth, fHeight);
      }
  }
  /* -- Init from a manifest ----------------------------------------------- */
  void InitTextureImageManifest(Image &imSrc, const Json &jsDoc)
  { // Show filename and set it
    cLog->LogDebugExSafe("Texture loading '$' from manifest '$'.",
      imSrc.IdentGet(), jsDoc.IdentGet());
    // Make sure correct version
    const unsigned int uiVersionRequired = 1;
    const unsigned int uiVersion = jsDoc.GetInteger("Version");
    if(uiVersion != uiVersionRequired)
      XC("Invalid version in manifest!",
         "Identifier", imSrc.IdentGet(),  "Manfiest", jsDoc.IdentGet(),
         "Required",   uiVersionRequired, "Actual",   uiVersion);
    // Read filter
    const OglFilterEnum ofeFilter =
      static_cast<OglFilterEnum>(jsDoc.GetInteger("Filter"));
    // Read automatic tile generation option and if it is set?
    if(jsDoc.GetBoolean("GenerateTileset"))
    { // Read required tile width and padding
      const GLuint uiTileWidth = jsDoc.GetInteger("TileWidth"),
                   uiTileHeight = jsDoc.GetInteger("TileHeight"),
                   uiPadX = jsDoc.GetInteger("PaddingWidth"),
                   uiPadY = jsDoc.GetInteger("PaddingHeight");
      // Load the texture
      InitTextureImage(imSrc, uiTileWidth, uiTileHeight, uiPadX, uiPadY,
        ofeFilter, true);
      // We're done
      return;
    } // Get the array for user specified tile values
    using Lib::RapidJson::Value;
    const Value &rjvTiles = jsDoc.GetValue("Tiles");
    if(!rjvTiles.IsArray())
      XC("Tiles array not valid in manifest!",
         "Identifier", imSrc.IdentGet(), "Manfiest", jsDoc.IdentGet());
    // If we have items in the array
    if(rjvTiles.Empty())
      XC("Tiles array is empty in manifest!",
         "Identifier", imSrc.IdentGet(), "Manfiest", jsDoc.IdentGet());
    // Allocate memory for tiles on first image
    clTiles.resize(1);
    CoordList &clFirst = clTiles.front();
    clFirst.reserve(rjvTiles.Size());
    // Load the texture now
    InitTextureImage(imSrc, 0, 0, 0, 0, ofeFilter, false);
    // Walk the array which is of an unknown size
    StdForEach(seq, rjvTiles.Begin(), rjvTiles.End(),
      [this, &imSrc, &jsDoc, &clFirst](const Value &rjvValue)
      { // Must be a valid array
        if(!rjvValue.IsArray())
          XC("Tile array values not valid in manifest!",
             "Identifier", imSrc.IdentGet(), "Manfiest", jsDoc.IdentGet(),
             "Index",      clFirst.size());
        // Must have at least 4 values
        if(rjvValue.Size() < 4)
          XC("Tile array must have at least four values in manifest!",
             "Identifier", imSrc.IdentGet(), "Manfiest", jsDoc.IdentGet(),
             "Index",      clFirst.size(),   "Count",    rjvValue.Size());
        // Enumerate the values and set them in our array
        array<GLfloat, 4> aValues;
        for(unsigned int uiIndex = 0; uiIndex < aValues.size(); ++uiIndex)
        { // Get and check that the value is unsigned integer
          const Value &rjvCoord = rjvValue[uiIndex];
          if(!rjvCoord.IsUint())
            XC("Tile array value is not valid in manifest!",
               "Identifier", imSrc.IdentGet(), "Manfiest", jsDoc.IdentGet(),
               "Index",      clFirst.size(),   "Count",    rjvValue.Size(),
               "SubIndex",   uiIndex);
          // Convert and assign the value
          aValues[uiIndex] = static_cast<GLfloat>(rjvCoord.GetUint());
        } // Add the tile and then process the next tile
        AddTileDORWH(0, aValues[0], aValues[1], aValues[2], aValues[3]);
      });
  }
  /* -- Deinitialise ------------------------------------------------------- */
  void DeInit(void)
  { // Texture not loaded? return
    if(IsNotInitialised() || FlagIsSet(TF_DELETE)) return;
    // Mark texture for deletion (explanation in fbos class)
    cOgl->SetDeleteTextures(uivTexture);
    // Texture has been marked for deleting
    FlagSet(TF_DELETE);
    // Report de-initialisation and texture memory
    cLog->LogInfoExSafe("Texture '$' with $ textures marked for deletion.",
      IdentGet(), GetSubCount());
  }
  /* -- Constructor (Initialisation then registration) --------------------- */
  Texture(void) :                      // No parameters
    /* -- Initialisers ----------------------------------------------------- */
    ICHelperTexture{ cTextures, this },// Automatic (de)registration
    TextureBase{ IP_TEXTURE }          // Purpose is to be a texture
    /* -- Code ------------------------------------------------------------- */
    { }                                // Do nothing else
  /* -- Constructor (No registration, base class of Font class) ------------ */
  explicit Texture(const ImageFlagsConst ifcPurpose) :
    /* -- Initialisers ----------------------------------------------------- */
    ICHelperTexture{ cTextures },      // Initially unregistered
    TextureBase{ ifcPurpose }          // Purpose is specified by caller
    /* -- Code ------------------------------------------------------------- */
    { }                                // Do nothing else
  /* -- Destructor (Unregistration then deinitialisation) ------------------ */
  ~Texture(void) { DeInit(); }
};/* -- Finish the collector ----------------------------------------------- */
CTOR_END_NOINITS(Textures, Texture, TEXTURE)
/* -- DeInit Textures ------------------------------------------------------ */
static void TextureDeInitTextures(void)
{ // Ignore if no textures
  if(cTextures->empty()) return;
  // De-init all the textures and log the pre/post init of them
  cLog->LogDebugExSafe("Textures de-initialising $ objects...",
    cTextures->size());
  for(Texture*const tCptr : *cTextures) tCptr->DeInit();
  cLog->LogDebugExSafe("Textures de-initialised $ objects.",
    cTextures->size());
}
/* -- Reload Textures ------------------------------------------------------ */
static void TextureReInitTextures(void)
{ // Ignore if no textures
  if(cTextures->empty()) return;
  // Reload all the textures and log the pre/post init of them
  cLog->LogDebugExSafe("Textures re-initialising $ objects...",
    cTextures->size());
  for(Texture*const tCptr : *cTextures) tCptr->ReloadTexture();
  cLog->LogDebugExSafe("Textures re-initialised $ objects.",
    cTextures->size());
}
/* ------------------------------------------------------------------------- */
static GLuint TextureGetMaxSizeFromBounds(const GLuint uiWidth,
  const GLuint uiHeight, const GLuint uiXWidth, const GLuint uiXHeight,
  const GLuint uiMultiplier)
{ // Calculate the best possible texture size, by rounding up the
  // requested tile size plus the current canvas size up to the
  // nearest power of two, or double the current image size, whichever
  // value is largest.
  return UtilMaximum(
           UtilMaximum(
             UtilNearestPow2<GLuint>(uiWidth + uiXWidth),
             uiWidth * uiMultiplier),
           UtilMaximum(
             UtilNearestPow2<GLuint>(uiHeight + uiXHeight),
             uiHeight * uiMultiplier));
}
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
