/* == TEXTURE.HPP ========================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## This module handles the loading and uploading of textures into      ## **
** ## the renderer and the handling of tiles, texture co-ordinates,       ## **
** ## colour and drawing positions.                                       ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace ITexture {                   // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace ICollector::P;         using namespace IDim::P;
using namespace IError::P;             using namespace IFbo::P;
using namespace IFboCmd::P;            using namespace IFboItem::P;
using namespace IImage::P;             using namespace IImageDef::P;
using namespace IJson::P;              using namespace ILog::P;
using namespace ILuaIdent::P;          using namespace ILuaLib::P;
using namespace IMemory::P;            using namespace IOgl::P;
using namespace ISerial::P;            using namespace IShader::P;
using namespace IShaders::P;           using namespace IStd::P;
using namespace ISysUtil::P;           using namespace ITexDef::P;
using namespace IUtil::P;              using namespace Lib::OS::GlFW::Types;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* -- Define texture collector class --------------------------------------- */
CTOR_BEGIN_NOBB(Textures, Texture, CLHelperUnsafe)
/* -- Texture base class so we can reduce initialiser duplication ---------- */
class TextureBase :                    // All members initially private
  /* -- Base classes ------------------------------------------------------- */
  public FboItem,                      // Fbo item class with drawing co-ords
  public Image                         // Image class with raw image pixel data
{ /* -- Protected variables ------------------------------------- */ protected:
  GLint            gliTexMinFilter,    // GL texture minification setting
                   gliTexMagFilter,    // GL texture magnification setting
                   gliMipmaps;         // Sub-image's are mipmaps (count)
  OglFilterEnum    ofeTexFilter;       // Texture filter (for reference)
  /* -- Tile co-ordinates class ------------------------------------ */ public:
  struct CoordData :                   // All members are public
    /* -- Initialisers ----------------------------------------------------- */
    public DimGLFloat,                 // Dimensions of texture
    public QuadTexData                 // GL quad co-ords of two triangles
  { /* -- 2D tex-coord quad initialisation constructor --------------------- */
    CoordData(const GLfloat glfW,      // Pixel width of tile  |---Y1---| ^
              const GLfloat glfH,      // Pixel height of tile |........| |
              const GLfloat glfL,      // Left X1 coordinate   X1......X2 H
              const GLfloat glfT,      // Top Y1 coordinate    |........| |
              const GLfloat glfR,      // Right X2 coordinate  |---Y2---| v
              const GLfloat glfB) :    // Bottom Y2 coordinate <----W--->
      /* -- Initialisers --------------------------------------------------- */
      DimGLFloat{ glfW, glfH },        // Initialise tile pixel dimensions
      QuadTexData{ {                   // Initialise two GL triangle tex coords
        { glfL,glfT, glfR,             // [0](T1)[0-2](X1,Y1,X2, XY1--XY2 XY3
          glfT, glfL,glfB },           // [0](T1)[3-5] Y2,X3,Y3)   |T1/   /.|
        { glfR,glfB, glfL,             // [1](T2)[0-2](X1,Y1,X2,   |./   /T2|
          glfB, glfR,glfT } } }        // [1](T2)[3-5] Y2,X3,Y3)   XY3 XY2--XY1
      /* -- No code -------------------------------------------------------- */
      {}                               // Note that our shader handles Z co-ord
    /* -- Default constructor ---------------------------------------------- */
    CoordData() :
      /* -- Initialisers --------------------------------------------------- */
      QuadTexData{ {                   // Default init two GL tri tex coords
        { 0.0f,0.0f, 0.0f,             // [0](T1)[0-2](X1,Y1,X2, XY1--XY2 XY3
          0.0f, 0.0f,0.0f },           // [0](T1)[3-5] Y2,X3,Y3)   |T1/   /.|
        { 0.0f,0.0f, 0.0f,             // [1](T2)[0-2](X1,Y1,X2,   |./   /T2|
          0.0f, 0.0f,0.0f } } }        // [1](T2)[3-5] Y2,X3,Y3)   XY3 XY2--XY1
      /* -- No code -------------------------------------------------------- */
      {}                               // Note that our shader handles Z co-ord
  };/* --------------------------------------------------------------------- */
  using CoordList   = StdVector<CoordData>; // Tile coordinates data list
  using CoordListIt = CoordList::iterator;  // Iterator to a CoordList
  using CoordsList  = StdVector<CoordList>; // A list of tile coords per subtex
  /* ----------------------------------------------------------------------- */
  CoordsList       clTiles;            // Texture coordinates for tiles
  GLUIntVector     gluvTexture;        // OpenGL texture handle list
  Shader          *shpProgram;         // Default shader program to use
  DimGLUInt        dgluTile;           // Texture tile width and height
  DimGLFloat       dglfPad,            // Texture tile padding (GL)
                   dglfImage,          // Texture image width and height (GL)
                   dglfTile;           // Same as dgluTile but as a GLfloat
  /* -- Constructor -------------------------------------------------------- */
  explicit TextureBase(const ImageFlagsConst ifcPurpose) :
    /* -- Initialisers ----------------------------------------------------- */
    Image{ ifcPurpose },               // Iniitalise purpose of texture
    gliTexMinFilter(GL_NONE),          // No minification filter set yet
    gliTexMagFilter(GL_NONE),          // No magnification filter set et
    gliMipmaps(0),                     // No mipmaps yet
    ofeTexFilter(OF_N_N),              // No texture filter index set yet
    shpProgram(nullptr)                // No shader programme yet
    /* -- No code ---------------------------------------------------------- */
    {}
};/* ----------------------------------------------------------------------- */
CTOR_MEM_BEGIN(Textures, Texture, ICHelperUnsafe, /* No SerialSlave<> */),
  /* -- Base classes ------------------------------------------------------- */
  public TextureBase                   // Texture class variables
{ /* -- Functors for upload texture function ------------------------------- */
  struct TexCompFtor                   // Keep functors categorised
  { /* -- Load as raw uncompressed pixels ---------------------------------- */
    struct RAW                         // Uniform pixels (R,RG,RGB,RGBA, etc.)
    { /* -- Default constructor -------------------------------------------- */
      RAW(const Texture &tTexture, const size_t stSubTexId,
        const GLint gliMipLevel, const TextureType ttNICFormat,
        const TextureType ttNXCFormat, const ImageSlot &isSlot)
      { // Upload uncompressed texture to video ram
        GL(cOgl->UploadTextureTT(gliMipLevel, isSlot.DimGetWidth<GLsizei>(),
          isSlot.DimGetHeight<GLsizei>(), ttNICFormat, ttNXCFormat,
          isSlot.MemPtr()),
          "Could not upload uncompressed texture to video ram!",
          "Name",      tTexture.NameGet(), "Index", stSubTexId,
          "TexId",     tTexture.GetSubName(stSubTexId),
          "Level",     gliMipLevel,        "Width", isSlot.DimGetWidth(),
          "Height",    isSlot.DimGetHeight(),
          "XCFormat",  ImageGetPixelFormat(tTexture.GetPixelType()),
          "NICFormat", ImageGetPixelFormat(ttNICFormat),
          "NXCFormat", ImageGetPixelFormat(ttNXCFormat),
          "Size",      isSlot.MemSize(),   "Data",  isSlot.MemPtr<void>());
      }
    };
    /* -- Load as compressed dxt pixels ------------------------------------ */
    struct DXT                         // DXT1, DXT3 or DXT5
    { /* -- Default constructor -------------------------------------------- */
      DXT(const Texture &tTexture, const size_t stSubTexId,
        const GLint gliMipLevel, const TextureType, const TextureType,
        const ImageSlot &isSlot)
      { // Upload pre-compressed texture to video ram
        GL(cOgl->UploadCompressedTextureTT(gliMipLevel,
          tTexture.GetPixelType(), isSlot.DimGetWidth<GLsizei>(),
          isSlot.DimGetHeight<GLsizei>(), isSlot.MemSize<GLsizei>(),
          isSlot.MemPtr()),
          "Could not upload compressed texture to video ram!",
          "Name",     tTexture.NameGet(), "Index", stSubTexId,
          "TexId",    tTexture.GetSubName(stSubTexId),
          "Level",    gliMipLevel,        "Width", isSlot.DimGetWidth(),
          "Height",   isSlot.DimGetHeight(),
          "XCFormat", ImageGetPixelFormat(tTexture.GetPixelType()),
          "Size",     isSlot.MemSize(),   "Data",  isSlot.MemPtr<void>());
      }
    };
  };
  /* -- Do create textures ------------------------------------------------- */
  void CreateTextureHandles(const size_t stCount)
  { // Resize array to fit texture names
    gluvTexture.resize(stCount);
    // Make OpenGL to generate texture names and throw error if failed
    GL(cOgl->CreateTextures(gluvTexture), "Failed to generate textures!",
      "Name", NameGet(), "Count", stCount);
  }
  /* -- Generate mipmaps if needed ----------------------------------------- */
  void ReGenerateMipmaps()
  { // Only need to generate mipmaps if we're actually using mipmapping
    switch(gliTexMinFilter)
    { // Any of the mipmapping settings? Generate mipmaps
      case GL_LINEAR_MIPMAP_LINEAR:  case GL_LINEAR_MIPMAP_NEAREST:
      case GL_NEAREST_MIPMAP_LINEAR: case GL_NEAREST_MIPMAP_NEAREST:
        GL(cOgl->GenerateMipmaps(),
          "Failed to generate mipmaps!", "Name", NameGet());
        [[fallthrough]];
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
      gliMipmaps = 0;
      // Create texture handles
      CreateTextureHandles(stSlots);
      // For each image slot
      for(size_t stSubTexId = 0; stSubTexId < stSlots; ++stSubTexId)
      { // Get next slot and verify that dimensions are different from slot 0
        const ImageSlot &isRef = GetSlots()[stSubTexId];
        if(DimIsNotEqual(isRef))
          XC("Alternating image sizes are not supported!",
            "Name",       NameGet(),      "LastWidth", DimGetWidth(),
            "LastHeight", DimGetHeight(), "ThisWidth", isRef.DimGetWidth(),
            "ThisHeight", isRef.DimGetHeight());
        // Get texture id and bind it
        const unsigned gluTexId = GetSubName(stSubTexId);
        GL(cOgl->BindTexture(gluTexId), "Texture id failed to bind!",
          "Name",    NameGet(), "Index", stSubTexId,
          "Maximum", stSlots,   "TexId", gluTexId);
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
      gliMipmaps = UtilIntOrMax<GLint>(stSlots);
      // Create only one texture handle as other slots are for mipmaps
      CreateTextureHandles(1);
      // Get texture id
      const unsigned gluTexId = GetSubName();
      // Bind the texture id
      GL(cOgl->BindTexture(gluTexId), "Mipmapped texture failed to bind!",
        "Name", NameGet(), "TexId", gluTexId);
      // Initialise mipmap counter
      GL(cOgl->SetTexParam(GL_TEXTURE_BASE_LEVEL, 0),
        "Failed to set max mipmap base level on texture!",
        "Name", NameGet(), "TexId", gluTexId);
      GL(cOgl->SetTexParam(GL_TEXTURE_MAX_LEVEL, GetMipmaps()),
        "Failed to set max mipmap max level on texture!",
        "Name", NameGet(), "TexId", gluTexId, "Count", GetMipmaps());
      // Configure the texture
      ConfigureTexture(0);
      // Upload first mipmap
      TexCompFtor(*this, 0, 0, ttNICFormat, ttNXCFormat, isSlot);
      // Done if there more slots to load
      if(stSlots <= 1) return;
      // Last mipmap size
      unsigned uLWidth = DimGetWidth(), uLHeight = DimGetHeight();
      // Load all the other mipmaps
      for(size_t stSubTexId = 1; stSubTexId < stSlots; ++stSubTexId)
      { // Get next slot
        const ImageSlot &isRef = GetSlotsConst()[stSubTexId];
        // Make sure size is smaller than the last
        if(isRef.DimGetWidth() >= uLWidth ||
           isRef.DimGetHeight() >= uLHeight)
          XC("Specified mipmap is not smaller than the last!",
            "Name",       NameGet(), "LastWidth", uLWidth,
            "LastHeight", uLHeight,  "ThisWidth", isRef.DimGetWidth(),
            "ThisHeight", isRef.DimGetHeight());
        // Load the mipmap
        TexCompFtor(*this, 0, static_cast<GLint>(stSubTexId), ttNICFormat,
          ttNXCFormat, isRef);
        // Update last mipmap size
        uLWidth = DimGetWidth();
        uLHeight = DimGetHeight();
      }
    } // Log progress
    cLog->LogDebugExSafe("Texture '$'[S:$;F:$/$;M:$;D:$x$] uploaded.",
      NameGet(), stSlots, ImageGetPixelFormat(ttNXCFormat),
      ImageGetPixelFormat(ttNICFormat), GetMipmaps(), isSlot.DimGetWidth(),
      isSlot.DimGetHeight());
  }
  /* -- Return a new texcoord tile converting 2D texture coords to 3D ------ */
  const CoordData NewTile(const GLfloat glfX1, const GLfloat glfY1,
    const GLfloat glfX2, const GLfloat glfY2, const GLfloat glfWidth,
    const GLfloat glfHeight)
  { return { glfWidth, glfHeight, glfX1 / dglfImage.DimGetWidth(),
      1.0f - glfY1 / dglfImage.DimGetHeight(), glfX2 / dglfImage.DimGetWidth(),
      1.0f - glfY2 / dglfImage.DimGetHeight() }; }
  /* -- Set the texture co-ordinates of a tile ----------------------------- */
  void SetTile(const size_t stSubTexId, const size_t stTileId,
    const GLfloat glfX1, const GLfloat glfY1, const GLfloat glfX2,
    const GLfloat glfY2, const GLfloat glfWidth, const GLfloat glfHeight)
  { // Log the tile we will modify
#if defined(ALPHA)
    cLog->LogDebugExSafe("- Set tile $ in $: $x$ ($x$x$x$)",
      stTileId, stSubTexId, glfWidth, glfHeight, glfX1, glfY1, glfX2, glfY2);
#endif
    // Modify the tile
    clTiles[stSubTexId][stTileId] =
      NewTile(glfX1, glfY1, glfX2, glfY2, glfWidth, glfHeight);
  }
  /* -- Set the texture co-ordinates of a tile ----------------------------- */
  void SetTile(const size_t stSubTexId, const size_t stTileId,
    const GLfloat glfX1, const GLfloat glfY1, const GLfloat glfX2,
    const GLfloat glfY2)
  { SetTile(stSubTexId, stTileId, glfX1, glfY1, glfX2, glfY2,
      glfX2 - glfX1, glfY2 - glfY1); }
  /* -- Set the texture co-ordinates of a tile that has reversed scanlines - */
  void SetTileR(const size_t stSubTexId, const size_t stTileId,
    const GLfloat glfX1, const GLfloat glfY1, const GLfloat glfX2,
    const GLfloat glfY2)
  { const GLfloat glfNY1 = dglfImage.DimGetHeight() - glfY1,
                  glfNY2 = dglfImage.DimGetHeight() - glfY2;
    SetTile(stSubTexId, stTileId, glfX1, glfNY1,
      glfX2, glfNY2, glfX2 - glfX1, glfNY1 - glfNY2); }
  /* -- Add a tile with width and height ----------------------------------- */
  void SetTileWH(const size_t stSubTexId, const size_t stTileId,
    const GLfloat glfX1, const GLfloat glfY1, const GLfloat glfWidth,
    const GLfloat glfHeight)
  { const GLfloat glfNX2 = glfX1 + glfWidth,
                  glfNY2 = glfY1 + glfHeight;
    SetTile(stSubTexId, stTileId, glfX1, glfY1, glfNX2, glfNY2,
      glfNX2 - glfX1, glfNY2 - glfY1); }
  /* -- Add a reversed tile with width and height setting ------------------ */
  void SetTileRWH(const size_t stSubTexId, const size_t stTileId,
    const GLfloat glfX1, const GLfloat glfY1, const GLfloat glfWidth,
    const GLfloat glfHeight)
  { const GLfloat glfNY1 = dglfImage.DimGetHeight()-glfY1,
                  glfNY2 = glfNY1 + glfHeight,
                  glfNX2 = glfX1 + glfWidth;
    SetTile(stSubTexId, stTileId, glfX1, glfY1, glfNX2, glfNY2,
      glfNX2 - glfX1, glfNY1 - glfNY2); }
  /* -- Do add a tile with custom width and height setting ----------------- */
  void AddTile(const size_t stSubTexId, const GLfloat glfX1,
    const GLfloat glfY1, const GLfloat glfX2, const GLfloat glfY2,
    const GLfloat glfWidth, const GLfloat glfHeight)
  { // Get reference tile list
    CoordList &clData = clTiles[stSubTexId];
    // Log the tile we will add
#if defined(ALPHA)
    cLog->LogDebugExSafe("- Add tile $ in $: $x$ ($x$x$x$)",
      clData.size(), stSubTexId, glfWidth, glfHeight,
        glfX1, glfY1, glfX2, glfY2);
#endif
    // Add it to the list
    clData.emplace_back(
      NewTile(glfX1, glfY1, glfX2, glfY2, glfWidth, glfHeight));
  }
  /* -- Add a tile --------------------------------------------------------- */
  void AddTile(const size_t stSubTexId, const GLfloat glfX1,
    const GLfloat glfY1, const GLfloat glfX2, const GLfloat glfY2)
  { AddTile(stSubTexId, glfX1, glfY1, glfX2, glfY2, glfX2 - glfX1,
      glfY2 - glfY1); }
  /* -- Add a reversed tile ------------------------------------------------ */
  void AddTileR(const size_t stSubTexId, const GLfloat glfX1,
    const GLfloat glfY1, const GLfloat glfX2, const GLfloat glfY2)
  { const GLfloat glfNY1 = dglfImage.DimGetHeight() - glfY1,
                  glfNY2 = dglfImage.DimGetHeight() - glfY2;
    AddTile(stSubTexId, glfX1, glfNY1, glfX2, glfNY2,
      glfX2 - glfX1, glfNY1 - glfNY2); }
  /* -- Add a tile with width and height ----------------------------------- */
  void AddTileWH(const size_t stSubTexId, const GLfloat glfX1,
    const GLfloat glfY1, const GLfloat glfWidth, const GLfloat glfHeight)
  { const GLfloat glfNX2 = glfX1 + glfWidth,
                  glfNY2 = glfY1 + glfHeight;
    AddTile(stSubTexId, glfX1, glfY1, glfNX2, glfNY2, glfNX2 - glfX1,
      glfNY2 - glfY1); }
  /* -- Add a reversed tile with width and height -------------------------- */
  void AddTileRWH(const size_t stSubTexId, const GLfloat glfX1,
    const GLfloat glfY1, const GLfloat glfWidth, const GLfloat glfHeight)
  { const GLfloat glfNY1 = dglfImage.DimGetHeight() - glfY1,
                  glfNY2 = glfNY1 + glfHeight, glfNX2 = glfX1+glfWidth;
    AddTile(stSubTexId, glfX1, glfY1, glfNX2, glfNY2, glfNX2 - glfX1,
      glfNY1 - glfNY2); }
  /* -- Configure the specified texture id --------------------------------- */
  void ConfigureTexture(const size_t stSubTexId)
  { // Start configuring the texture.
    GL(cOgl->SetTexParam(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE),
      "Could not set texture wrapping S!",
      "Name", NameGet(), "Index", stSubTexId);
    GL(cOgl->SetTexParam(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE),
      "Could not set texture wrapping T!",
      "Name", NameGet(), "Index", stSubTexId);
    GL(cOgl->SetTexParam(GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE),
      "Could not set texture wrapping R!",
      "Name", NameGet(), "Index", stSubTexId);
    // Set filtering based on developers setting
    GL(cOgl->SetTexParam(GL_TEXTURE_MIN_FILTER, gliTexMinFilter),
      "Could not set texture minifaction filter!",
      "Name", NameGet(), "Index", stSubTexId,
      "MinFilter",  gliTexMinFilter);
    GL(cOgl->SetTexParam(GL_TEXTURE_MAG_FILTER, gliTexMagFilter),
      "Could not set texture magnification filter!",
      "Name", NameGet(), "Index", stSubTexId,
      "MagFilter",  gliTexMagFilter);
  }
  /* -- Load texture from image class -------------------------------------- */
  void LoadFromImage()
  { // Get number of slots in this image and return if zero
    if(IsNoSlots()) XC("No data in image object!", "Name", NameGet());
    // Get first slot
    const ImageSlot &isSlot = GetSlotsConst().front();
    // Copy image dimensions to texture dimensions
    DimSet(isSlot);
    // Set image dimensions as float for opengl
    dglfImage.DimSet(DimGetWidth<GLfloat>(), DimGetHeight<GLfloat>());
    // Check width and height's are valid for the graphics device
    const unsigned uMaxSize = cOgl->MaxTexSize();
    if(isSlot.DimIsNotSet() || isSlot.DimGetWidth() > uMaxSize ||
       isSlot.DimGetHeight() > uMaxSize)
      XC("Image dimensions not supported by graphics hardware!",
        "Name",   NameGet(),             "Width",   isSlot.DimGetWidth(),
        "Height", isSlot.DimGetHeight(), "Maximum", uMaxSize);
    // Lookup internal pixel type chosen and check it
    const TextureType ttICFormat = ImageBYtoTexType(GetBytesPerPixel());
    if(ttICFormat == TT_NONE)
      XC("Unsupported texture colour type!",
        "Name",         NameGet(), "BytesPerPixel", GetBytesPerPixel(),
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
          shpProgram = &cShaderCore->sh2D8Pal;
          // Force no filtering and no mipmapping or we get the wrong colours
          ofeTexFilter = OF_N_N;
          gliTexMinFilter = gliTexMagFilter = GL_NEAREST;
        } // No palette
        else shpProgram = &cShaderCore->sh2D8;
        // Set requested external format
        ttNXCFormat = GetPixelType();
        // Break to upload raw pixels
        break;
      // Texture is 16-bpp?
      case TT_GRAYALPHA:
        // Set GL_LUMINANCE_ALPHA decoding shader
        shpProgram = &cShaderCore->sh2D16;
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
            shpProgram = &cShaderCore->sh2D;
            // Upload as compressed texture
            UploadTexture<TexCompFtor::DXT>
              (stSlots, isSlot, ttICFormat, ttNXCFormat);
            // Return because we uploaded compressed pixels
            return;
          // BGRA colour order type?
          case TT_BGRA:
            // Use BGR shader and redefine to RGBA.
            shpProgram = &cShaderCore->sh2DBGR;
            ttNXCFormat = TT_RGBA;
            // Break to upload raw pixels
            break;
          // BGR colour order type?
          case TT_BGR:
            // Use BGR shader and redefine to RGB.
            shpProgram = &cShaderCore->sh2DBGR;
            ttNXCFormat = TT_RGB;
            // Break to upload raw pixels
            break;
          // RGBA or RGB (Normal image).
          case TT_RGB: case TT_RGBA:
            // Use RGB shader. No format change.
            shpProgram = &cShaderCore->sh2D;
            ttNXCFormat = GetPixelType();
            // Break to upload raw pixels
            break;
          // Unknown external format
          default: XC("External colour type not acceptable!",
            "Name", NameGet(), "XCFormat", GetPixelType());
        } // Done
        break;
      // Unknown internal format
      default: XC("Internal colour type not acceptable!",
        "Name",     NameGet(), "ICFormat", ttICFormat,
        "XCFormat", GetPixelType());
    } // The pixel type is raw uniform pixels so upload them
    UploadTexture<TexCompFtor::RAW>(stSlots, isSlot, ttICFormat, ttNXCFormat);
  }
  /* -- Return tile dimensions as float ------------------------- */ protected:
  GLfloat GetTileWidthFloat() const { return dglfTile.DimGetWidth(); }
  GLfloat GetTileHeightFloat() const { return dglfTile.DimGetHeight(); }
  /* -- Return padding dimensions ---------------------------------- */ public:
  GLfloat GetPaddingWidth() const { return dglfPad.DimGetWidth(); }
  GLfloat GetPaddingHeight() const { return dglfPad.DimGetHeight(); }
  /* -- Return tile dimensions --------------------------------------------- */
  template<typename IntType = GLuint>
    requires StdIsArithmatic<IntType>
  IntType GetTileWidth() const { return dgluTile.DimGetWidth<IntType>(); }
  template<typename IntType = GLuint>
    requires StdIsArithmatic<IntType>
  IntType GetTileHeight() const { return dgluTile.DimGetHeight<IntType>(); }
  /* -- Return number of tiles --------------------------------------------- */
  size_t GetTileCount(const size_t stSubTexId = 0) const
    { return clTiles[stSubTexId].size(); }
  /* -- Set tile count ----------------------------------------------------- */
  void SetTileCount(const size_t stCount, const size_t stSubTexId = 0)
    { clTiles[stSubTexId].resize(stCount); }
  /* -- Return the number of mipmaps in the texture ------------------------ */
  GLint GetMipmaps() const { return gliMipmaps; }
  /* -- Return the current texture filter index setting -------------------- */
  OglFilterEnum GetTexFilter() const { return ofeTexFilter; }
  /* -- Return the OpenGL texture name for the specified sub-textures ------ */
  GLuint GetSubName(const size_t stSubTexId=0) const
    { return gluvTexture[stSubTexId]; }
  /* -- Return number of sub-textures -------------------------------------- */
  size_t GetSubCount() const { return gluvTexture.size(); }
  /* -- Check if texture is initialised ------------------------------------ */
  bool IsNotInitialised() const { return gluvTexture.empty(); }
  bool IsInitialised() const { return !IsNotInitialised(); }
  /* -- Set a tile based on reversal --------------------------------------- */
  void SetTileDOR(const size_t stSubTexId, const size_t stTileId,
    const GLfloat glfX1, const GLfloat glfY1, const GLfloat glfX2,
    const GLfloat glfY2)
      { if(IsReversed())
          SetTileR(stSubTexId, stTileId, glfX1, glfY1, glfX2, glfY2);
        else SetTile(stSubTexId, stTileId, glfX1, glfY1, glfX2, glfY2); }
  /* -- Set a tile based on reversal with width and height setting --------- */
  void SetTileDORWH(const size_t stSubTexId, const size_t stTileId,
    const GLfloat glfX1, const GLfloat glfY1, const GLfloat glfWidth,
    const GLfloat glfHeight)
  { if(IsReversed())
      SetTileRWH(stSubTexId, stTileId, glfX1, glfY1, glfWidth, glfHeight);
    else SetTileWH(stSubTexId, stTileId, glfX1, glfY1, glfWidth, glfHeight); }
  /* -- Add a tile based on reversal setting ------------------------------- */
  void AddTileDOR(const size_t stSubTexId, const GLfloat glfX1,
    const GLfloat glfY1, const GLfloat glfX2, const GLfloat glfY2)
  { if(IsReversed()) AddTileR(stSubTexId, glfX1, glfY1, glfX2, glfY2);
    else AddTile(stSubTexId, glfX1, glfY1, glfX2, glfY2); }
  /* -- Add a tile with width and height based on reversal ----------------- */
  void AddTileDORWH(const size_t stSubTexId, const GLfloat glfX1,
    const GLfloat glfY1, const GLfloat glfWidth, const GLfloat glfHeight)
  { if(IsReversed()) AddTileRWH(stSubTexId, glfX1, glfY1, glfWidth, glfHeight);
    else AddTileWH(stSubTexId, glfX1, glfY1, glfWidth, glfHeight); }
  /* -- Blit a triangle ---------------------------------------------------- */
  void BlitTri(const GLuint gluTexId, const TriTexData &ttdTex,
    const TriPosData &tpdVert, const TriIntData &tidClr)
  { FboActive()->FboBlit(gluTexId, tpdVert, ttdTex, tidClr, 0, shpProgram); }
  /* -- Blit two triangles that form a square ------------------------------ */
  void BlitQuad(const GLuint gluTexId, const QuadTexData &qtdVert,
    const QuadPosData &qpdTex, const QuadIntData &qidClr)
  { for(size_t stTriId = 0; stTriId < stTrisPerQuad; ++stTriId)
      BlitTri(gluTexId, qtdVert[stTriId], qpdTex[stTriId], qidClr[stTriId]); }
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
    const GLfloat glfX1, const GLfloat glfY1)
  { const CoordData &cdTex = clTiles[stSubTexId][stTileId];
    BlitQuad(GetSubName(stSubTexId), cdTex,
      FboItemSetAndGetVertex(glfX1, glfY1, glfX1 + cdTex.DimGetWidth(),
        glfY1 + cdTex.DimGetHeight()), FboItemGetCData()); }
  /* -- Blit quad with custom colour (used by font) ------------------------ */
  void BlitLTRBC(const size_t stSubTexId, const size_t stTileId,
    const GLfloat glfX1, const GLfloat glfY1, const GLfloat glfX2,
    const GLfloat glfY2, const QuadIntData &qidClr)
  { BlitQuad(GetSubName(stSubTexId), clTiles[stSubTexId][stTileId],
      FboItemSetAndGetVertex(glfX1, glfY1, glfX2, glfY2), qidClr); }
  /* -- Blit quad with bounds ---------------------------------------------- */
  void BlitLTRB(const size_t stSubTexId, const size_t stTileId,
    const GLfloat glfX1, const GLfloat glfY1, const GLfloat glfX2,
    const GLfloat glfY2)
  { BlitLTRBC(stSubTexId, stTileId, glfX1, glfY1, glfX2, glfY2,
      FboItemGetCData()); }
  /* -- Blit quad with coords and dimensions ------------------------------- */
  void BlitLTWH(const size_t stSubTexId, const size_t stTileId,
    const GLfloat glfX1, const GLfloat glfY1, const GLfloat glfWidth,
    const GLfloat glfHeight)
  { BlitLTRB(stSubTexId, stTileId, glfX1, glfY1,
      glfX1 + glfWidth, glfY1 + glfHeight); }
  /* -- Blit quad with truncated texcoord width and colour ----------------- */
  void BlitLTRBSC(const size_t stSubTexId, const size_t stTileId,
    const GLfloat glfX1, const GLfloat glfY1, const GLfloat glfX2,
    const GLfloat glfY2, const GLfloat glfLEdge, const GLfloat glfREdge,
    const QuadIntData &qidClr)
  { BlitQuad(GetSubName(stSubTexId),
      FboItemSetAndGetCoord(clTiles[stSubTexId][stTileId],
        glfLEdge, glfREdge),
      FboItemSetAndGetVertex(glfX1, glfY1, glfX2, glfY2,
        glfLEdge, glfREdge),
      qidClr); }
  /* -- Blit quad with truncated texcoord width ---------------------------- */
  void BlitLTRBS(const size_t stSubTexId, const size_t stTileId,
    const GLfloat glfX1, const GLfloat glfY1, const GLfloat glfX2,
    const GLfloat glfY2, const GLfloat glfLEdge, const GLfloat glfREdge)
  { BlitLTRBSC(stSubTexId, stTileId, glfX1, glfY1, glfX2, glfY2,
      glfLEdge, glfREdge, FboItemGetCData()); }
  /* -- Blit quad with position, stored size and with angle ---------------- */
  void BlitLTA(const size_t stSubTexId, const size_t stTileId,
    const GLfloat glfX1, const GLfloat glfY1, const GLfloat glfAngle)
  { const CoordData &cdTex = clTiles[stSubTexId][stTileId];
    BlitQuad(GetSubName(stSubTexId), cdTex,
      FboItemSetAndGetVertex(glfX1, glfY1, glfX1 + cdTex.DimGetWidth(),
        glfY1 + cdTex.DimGetHeight(), glfAngle), FboItemGetCData()); }
  /* -- Blit quad with coords, dimensions and angle ------------------------ */
  void BlitLTWHA(const size_t stSubTexId, const size_t stTileId,
    const GLfloat glfX1, const GLfloat glfY1, const GLfloat glfWidth,
    const GLfloat glfHeight, const GLfloat glfAngle)
  { BlitQuad(GetSubName(stSubTexId), clTiles[stSubTexId][stTileId],
      FboItemSetAndGetVertex(glfX1, glfY1, glfX1 + glfWidth, glfY1 + glfHeight,
        glfAngle), FboItemGetCData()); }
  /* -- Blit all quads as full image --------------------------------------- */
  void BlitMulti(const GLuint gluColumns, const GLfloat glfX1,
    const GLfloat glfY1, const GLfloat glfX2, const GLfloat glfY2)
  { // Calculate perspective width
    const GLfloat
      glfNWidth = (glfX2 - glfX1) / static_cast<GLfloat>(gluColumns),
      glfNHeight = (glfY2 - glfY1) /
        static_cast<GLfloat>(GetTileCount() / gluColumns);
    // For each image
    for(size_t stTile = 0; stTile < GetTileCount(); ++stTile)
    { // Calculate X, Y co-ordinate
      const GLfloat
        glfNX1 = glfX1 +
          static_cast<GLfloat>(stTile % gluColumns) * glfNWidth,
        glfNY1 = glfY1 +
          static_cast<GLfloat>(stTile / gluColumns) * glfNHeight;
      // Blit the texture
      BlitLTRB(0, stTile, glfNX1, glfNY1,
        glfNX1 + glfNWidth, glfNY1 + glfNHeight);
    }
  }
  /* -- Replace partial texture in VRAM from raw data ---------------------- */
  void UpdateEx(const GLuint gluTexId, const GLint gliX1, const GLint gliY1,
    const GLsizei glsiWidth, const GLsizei glsiHeight,
    const TextureType ttPixType, const GLvoid*const glvpData)
  { // Bind the specified texture
    GL(cOgl->BindTexture(gluTexId), "Failed to bind texture to update!",
      "Name", NameGet(), "TexId", gluTexId);
    // Upload the texture area
    GL(cOgl->UploadTextureSubTT(gliX1, gliY1,
         glsiWidth, glsiHeight, ttPixType, glvpData),
      "Failed to update VRAM with image!",
      "Name",    NameGet(),  "OffsetX", gliX1,
      "OffsetY", gliY1,      "Width",   glsiWidth,
      "Height",  glsiHeight, "SrcType", ImageGetPixelFormat(ttPixType));
    // Whats the minification value? We might need to regenerate mipmaps! If
    // we already had mipmaps, they will be overwritten.
    ReGenerateMipmaps();
    // Write that we updated the VRAM
    cLog->LogDebugExSafe("Texture '$'[$<$x$>] updated at $x$ with type $!",
      NameGet(), gluTexId, glsiWidth, glsiHeight, gliX1, gliY1,
      ImageGetPixelFormat(ttPixType));
  }
  /* -- Replace partial texture in VRAM from partial raw data -------------- */
  void UpdateEx(const GLuint gluTexId, const GLint gliX1, const GLint gliY1,
    const GLsizei glsiWidth, const GLsizei glsiHeight,
    const TextureType ttPixType, const GLvoid*const glvpData,
    const GLsizei glsiStride)
  { // Set stride and alignment
    GL(cOgl->SetUnpackRowLength(glsiStride),
      "Failed to set unpack row length!",
      "Texture", NameGet(), "Stride", glsiStride);
    // Update the texture
    UpdateEx(gluTexId, gliX1, gliY1, glsiWidth, glsiHeight, ttPixType,
      glvpData);
    // Reset stride and alignment
    GL(cOgl->SetUnpackRowLength(cOgl->UnpackRowLength()),
      "Failed to restore unpack row length!",
      "Texture", NameGet(), "Stride", cOgl->UnpackRowLength());
  }
  /* -- Replace partial texture in VRAM from array ------------------------- */
  void UpdateEx(const size_t stSubTexId, Image &imImage, const GLint gliX1,
    const GLint gliY1)
  { // Do the update
    UpdateEx(GetSubName(stSubTexId), gliX1, gliY1,
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
    const ByteDepth byDDepth = BY_RGBA) const
  { // Get texture id and bind it
    GL(cOgl->BindTexture(GetSubName(stSubTexId)),
      "Failed to bind texture to download!",
      "Name", NameGet(), "Index", stSubTexId);
    // Create memory block for texture data and read RGBA texture into it
    Memory mOut{ DimGetWidth() * DimGetHeight() * byDDepth };
    const TextureType ttPixType = ImageBYtoTexType(byDDepth);
    GL(cOgl->ReadTextureTT(ttPixType, mOut.MemPtr<GLvoid>()),
      "Download texture failed!",
      "Name",   NameGet(), "Index", stSubTexId, "Buffer", mOut.MemSize(),
      "Format", ImageGetPixelFormat(ttPixType));
    // Return a newly created image class containing this data
    return Image{ NameGet(), StdMove(mOut), DimGetWidth(), DimGetHeight(),
      bdDDepth };
  }
  /* -- Download texture and dump it to disk ------------------------------- */
  void Dump(const size_t stSubTexId, const StdStringView &strvFile) const
    { Download(stSubTexId).SaveFile(strvFile, stSubTexId, IFMT_PNG); }
  /* -- Reload texture array as normal texture ----------------------------- */
  void ReloadTexture()
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
    if(IsNotInitialised()) XC("No textures loaded!", "Name", NameGet());
    // Show what was loaded
    cLog->LogInfoExSafe("Texture loaded $ textures from '$'!",
      GetSubCount(), NameGet());
  }
  /* -- Init from a image class -------------------------------------------- */
  void InitTextureImage(Image &imgSrc, const GLuint gluTileWidth,
    const GLuint gluTileHeight, const GLuint gluPadX, const GLuint gluPadY,
    const OglFilterEnum ofeFilter, const bool bGenerateTileset = true)
  { // Show filename
    cLog->LogDebugExSafe("Texture loading from image '$'.", imgSrc.NameGet());
    // Take control of the image if source and destination are not the same
    if(this != &imgSrc) { SwapImage(imgSrc); NameSwap(imgSrc); }
    // We'll set float versions for faster calculations later on
    dglfPad.DimSet(static_cast<GLfloat>(gluPadX),
      static_cast<GLfloat>(gluPadY));
    // Set filter
    ofeTexFilter = ofeFilter;
    cOgl->SetMipMapFilterById(ofeFilter, gliTexMinFilter, gliTexMagFilter);
    // Initialise
    LoadFromImage();
    // Set specified tile dimensions and generate default tileset if needed
    dgluTile.DimSet(gluTileWidth, gluTileHeight);
    if(bGenerateTileset)
    { // Resize co-ords list to the number of usable slots we loaded
      clTiles.resize(GetSubCount());
      // Set override tile size from Image loader if specified
      if(duTileOR.DimIsSet()) dgluTile.DimSet(duTileOR);
      // Else clamp bounds to image size if unspecified or invalid size
      else if(dgluTile.DimIsNotSet() || GetTileWidth() > DimGetWidth() ||
          GetTileHeight() > DimGetHeight())
        dgluTile.DimSet(*this);
      // Set tile dimensions as opengl float
      dglfTile.DimSet(GetTileWidth<GLfloat>(), GetTileHeight<GLfloat>());
      // Calculate tile sizes
      const GLfloat
        // Tile size plus padding size
        glfTPadX = dglfTile.DimGetWidth() + GetPaddingWidth(),
        glfTPadY = dglfTile.DimGetHeight() + GetPaddingHeight(),
        // Tile counts.
#if defined(MACOS) && defined(CISC)
        // There is a strange side effect on MacOS Rosetta when ONLY loading a
        // PNG paletted image, floorf() makes these results 0 for some reason.
        // It might be memory corruption but not sure yet. Rosetta is known to
        // have issues with X87 emulation.
        glfTCountX = static_cast<GLfloat>(static_cast<size_t>(
          dglfImage.DimGetWidth() / glfTPadX)),
        glfTCountY = static_cast<GLfloat>(static_cast<size_t>(
          dglfImage.DimGetHeight() / glfTPadY)),
#else   // Works fine on ARM64 MacOS, X86_64 Linux and Windows
        glfTCountX = floorf(dglfImage.DimGetWidth() / glfTPadX),
        glfTCountY = floorf(dglfImage.DimGetHeight() / glfTPadY),
#endif  // Bitmap size to perfectly fit all tiles
        glfTSizeX = glfTCountX * glfTPadX,
        glfTSizeY = glfTCountY * glfTPadY;
      // For each texture we have
      for(size_t stSubTexId = 0; stSubTexId < GetSubCount(); ++stSubTexId)
      { // Add bottom to top image tile function
        auto AddReverse = [stSubTexId, this]
          (const GLfloat glfX, const GLfloat glfY)->void
        { AddTileR(stSubTexId, glfX, glfY, glfX + dglfTile.DimGetWidth(),
                                           glfY + dglfTile.DimGetHeight()); };
        // Add top to bottom image tile function
        auto AddForward = [stSubTexId, this]
          (const GLfloat glfX, const GLfloat glfY)->void
        { AddTile(stSubTexId, glfX, glfY, glfX + dglfTile.DimGetWidth(),
                                          glfY + dglfTile.DimGetHeight()); };
        // Get co-ordinates list
        CoordList &clData = clTiles[stSubTexId];
        // Have a tile limit?
        if(stTiles)
        { // Make sure theres enough memory allocated for each coord data. We
          // add to the existing amount (from the Font class).
          clData.reserve(clData.size() + stTiles);
          // Add forward tile and return if we're at the limit
          auto AddForwardLimit = [AddForward, &clData, this]
            (const GLfloat glfX, const GLfloat glfY)->bool
          { AddForward(glfX, glfY); return clData.size() >= stTiles; };
          // Add reverse tile and return if we're at the limit
          auto AddReverseLimit = [AddReverse, &clData, this]
            (const GLfloat glfX, const GLfloat glfY)->bool
          { AddReverse(glfX, glfY); return clData.size() >= stTiles; };
          // Enumerate function with tile limit
          auto Enumerate = [glfTSizeX, glfTPadX, glfTSizeY, glfTPadY]
            (auto &&aFunc)
          { // Enumerate and add each pixel tile and return if we're at limit
            for(GLfloat glfY = 0.0f; glfY < glfTSizeY; glfY += glfTPadY)
              for(GLfloat glfX = 0.0f; glfX < glfTSizeX; glfX += glfTPadX)
                if(aFunc(glfX, glfY)) return;
          }; // Do the appropriate function and finish if no more tiles
          if(IsReversed()) Enumerate(AddReverseLimit);
          else Enumerate(AddForwardLimit);
        } // Have no tile limit?
        else
        { // Calculate number of tiles needed
          const size_t stTilesMax =
            static_cast<size_t>((glfTSizeX / glfTPadX) *
                                (glfTSizeY / glfTPadY));
          // Make sure theres enough memory allocated for each coord data. We
          // add to the existing amount (from the Font class).
          clData.reserve(clData.size() + stTilesMax);
          // Enumerate function with no tile limit
          auto Enumerate = [glfTSizeX, glfTPadX, glfTSizeY, glfTPadY]
            (auto &&aFunc)->void
          { // Enumerate and add each pixel tile
            for(GLfloat glfY = 0.0f; glfY < glfTSizeY; glfY += glfTPadY)
              for(GLfloat glfX = 0.0f; glfX < glfTSizeX; glfX += glfTPadX)
                aFunc(glfX, glfY);
          }; // Do the appropriate function and finish if no more tiles
          if(IsReversed()) Enumerate(AddReverse);
          else Enumerate(AddForward);
        }
      } // Log tiling data
      cLog->LogDebugExSafe("- Textures: $ ($x$).\n"
                           "- Tiles: $ ($ cols; $ rows; $ limit).\n"
                           "- Tile size: $x$ ($x$ excess).\n"
                           "- Padding size: $x$ ($x$ tile).",
        GetSubCount(), DimGetWidth(), DimGetHeight(),
        GetTileCount(), glfTCountX, glfTCountY, stTiles,
        GetTileWidth(), GetTileHeight(),
          dglfImage.DimGetWidth() - glfTPadX,
          dglfImage.DimGetHeight() - glfTPadY,
        GetPaddingWidth(), GetPaddingHeight(), glfTPadX, glfTPadY);
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
    constexpr static size_t stValues = 4, stValuesM1 = stValues - 1;
    if(const size_t stExcess = gluvTiles.size() % stValues)
      XC("Invalid count of tiles specified!",
        "Name", NameGet(), "Count", stExcess);
    // Reserve memory for actual count
    clTiles.resize(1);
    CoordList &clFirst = clTiles.front();
    clFirst.reserve(gluvTiles.size() / stValues);
    // Shortcut to GLUIntVector const iterator
    using GLUIntVectorItConst = GLUIntVector::const_iterator;
    // Inline function to enumerate the vector of coords and send to callback
    auto fFunc = [&gluvTiles](auto &&fCb){
      for(GLUIntVectorItConst gluvicIt{ gluvTiles.cbegin() };
                              gluvicIt + stValuesM1 < gluvTiles.cend();
                              gluvicIt += stValues)
        fCb(static_cast<GLfloat>(*gluvicIt),
            static_cast<GLfloat>(*(gluvicIt + 1)),
            static_cast<GLfloat>(*(gluvicIt + 2)),
            static_cast<GLfloat>(*(gluvicIt + 3)));
    }; // Are image pixels reversed?
    if(IsReversed())
      fFunc([this](const GLfloat glfX1, const GLfloat glfY1,
        const GLfloat glfX2, const GLfloat glfY2)
          { AddTileRWH(0, glfX1, glfY1, glfX2, glfY2); });
    // Image pixels not reversed
    else fFunc([this](const GLfloat glfX1, const GLfloat glfY1,
      const GLfloat glfX2, const GLfloat glfY2)
        { AddTileWH(0, glfX1, glfY1, glfX2, glfY2); });
  }
  /* -- Init from a manifest ----------------------------------------------- */
  void InitTextureImageManifest(Image &imSrc, const Json &jsDoc)
  { // Show filename and set it
    cLog->LogDebugExSafe("Texture loading '$' from manifest '$'.",
      imSrc.NameGet(), jsDoc.NameGet());
    // Make sure correct version
    const unsigned uVersionRequired = 1,
                   uVersion = jsDoc.GetInteger("Version");
    if(uVersion != uVersionRequired)
      XC("Invalid version in manifest!",
        "Name",     imSrc.NameGet(),  "Manfiest", jsDoc.NameGet(),
        "Required", uVersionRequired, "Actual",   uVersion);
    // Read filter
    const OglFilterEnum ofeFilter =
      static_cast<OglFilterEnum>(jsDoc.GetInteger("Filter"));
    // Read automatic tile generation option and if it is set?
    if(jsDoc.GetBoolean("GenerateTileset"))
    { // Read required tile width and padding
      const GLuint gluTileWidth = jsDoc.GetInteger("TileWidth"),
                   gluTileHeight = jsDoc.GetInteger("TileHeight"),
                   gluPadX = jsDoc.GetInteger("PaddingWidth"),
                   gluPadY = jsDoc.GetInteger("PaddingHeight");
      // Load the texture
      InitTextureImage(imSrc, gluTileWidth, gluTileHeight, gluPadX, gluPadY,
        ofeFilter, true);
      // We're done
      return;
    } // Get the array for user specified tile values
    using Lib::RapidJson::Value;
    const Value &rjvTiles = jsDoc.GetValue("Tiles");
    if(!rjvTiles.IsArray())
      XC("Tiles array not valid in manifest!",
        "Name", imSrc.NameGet(), "Manfiest", jsDoc.NameGet());
    // If we have items in the array
    if(rjvTiles.Empty())
      XC("Tiles array is empty in manifest!",
        "Name", imSrc.NameGet(), "Manfiest", jsDoc.NameGet());
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
            "Name",  imSrc.NameGet(), "Manfiest", jsDoc.NameGet(),
            "Index", clFirst.size());
        // Must have at least 4 values
        if(rjvValue.Size() < 4)
          XC("Tile array must have at least four values in manifest!",
            "Name",  imSrc.NameGet(), "Manfiest", jsDoc.NameGet(),
            "Index", clFirst.size(),  "Count",    rjvValue.Size());
        // Enumerate the values and set them in our array
        StdArray<GLfloat, 4> aValues;
        for(unsigned uIndex = 0; uIndex < aValues.size(); ++uIndex)
        { // Get and check that the value is unsigned integer
          const Value &rjvCoord = rjvValue[uIndex];
          if(!rjvCoord.IsUint())
            XC("Tile array value is not valid in manifest!",
              "Name",     imSrc.NameGet(), "Manfiest", jsDoc.NameGet(),
              "Index",    clFirst.size(),  "Count",    rjvValue.Size(),
              "SubIndex", uIndex);
          // Convert and assign the value
          aValues[uIndex] = static_cast<GLfloat>(rjvCoord.GetUint());
        } // Add the tile and then process the next tile
        AddTileDORWH(0, aValues[0], aValues[1], aValues[2], aValues[3]);
      });
  }
  /* -- Deinitialise ------------------------------------------------------- */
  void DeInit()
  { // Texture not loaded? return
    if(IsNotInitialised() || FlagIsSet(TF_DELETE)) return;
    // Mark texture for deletion
    cOgl->SetDeleteTextures(gluvTexture);
    // Texture has been marked for deleting
    FlagSet(TF_DELETE);
    // Report de-initialisation and texture memory
    cLog->LogInfoExSafe("Texture '$' with $ textures marked for deletion.",
      NameGet(), GetSubCount());
  }
  /* -- Constructor (Initialisation then registration) --------------------- */
  Texture() :
    /* -- Initialisers ----------------------------------------------------- */
    ICHelperTexture{ cTextures, this },// Automatic (de)registration
    TextureBase{ IP_TEXTURE }          // Purpose is to be a texture
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- Constructor (No registration, base class of Font class) ------------ */
  explicit Texture(const ImageFlagsConst ifcPurpose) :
    /* -- Initialisers ----------------------------------------------------- */
    ICHelperTexture{ cTextures },      // Initially unregistered
    TextureBase{ ifcPurpose }          // Purpose is specified by caller
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- Destructor --------------------------------------------------------- */
  DTORHELPER(~Texture, DeInit())
};/* -- Finish the collector ----------------------------------------------- */
CTOR_END_NOINITS(Textures, Texture, TEXTURE)
/* -- DeInit Textures ------------------------------------------------------ */
static void TextureDeInitTextures()
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
static void TextureReInitTextures()
{ // Ignore if no textures
  if(cTextures->empty()) return;
  // Reload all the textures and log the pre/post init of them
  cLog->LogDebugExSafe("Textures reinitialising $ objects...",
    cTextures->size());
  for(Texture*const tCptr : *cTextures) tCptr->ReloadTexture();
  cLog->LogDebugExSafe("Textures reinitialised $ objects.",
    cTextures->size());
}
/* ------------------------------------------------------------------------- */
static GLuint TextureGetMaxSizeFromBounds(const GLuint gluWidth,
  const GLuint gluHeight, const GLuint gluXWidth, const GLuint gluXHeight,
  const GLuint gluMultiplier)
{ // Calculate the best possible texture size, by rounding up the
  // requested tile size plus the current canvas size up to the
  // nearest power of two, or double the current image size, whichever
  // value is largest.
  return UtilMaximum(
           UtilMaximum(UtilNearestPow2<GLuint>(gluWidth + gluXWidth),
             gluWidth * gluMultiplier),
           UtilMaximum(UtilNearestPow2<GLuint>(gluHeight + gluXHeight),
             gluHeight * gluMultiplier));
}
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
