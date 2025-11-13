/* == IMAGEDEF.HPP ========================================================= **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## Sets up the image data storage memory and metadata.                 ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IImageDef {                  // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IDim::P;               using namespace IFlags;
using namespace IMemory::P;            using namespace IOgl::P;
using namespace IStd::P;               using namespace ITexDef::P;
using namespace Lib::OS::GlFW::Types;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* -- Public typedefs ------------------------------------------------------ */
enum ImageFormat : size_t              // Available image codecs
{ /* ----------------------------------------------------------------------- */
  IFMT_PNG,                            // [0] PNG (IImageFormat::CodecPNG)
  IFMT_JPG,                            // [1] JPG (IImageFormat::CodecJPG)
  IFMT_GIF,                            // [2] GIF (IImageFormat::CodecGIF)
  IFMT_DDS,                            // [3] DDS (IImageFormat::CodecDDS)
  /* ----------------------------------------------------------------------- */
  IFMT_MAX                             // [4] Maximum supported image codecs
};/* ----------------------------------------------------------------------- */
BUILD_FLAGS(Image,                     // Shared image flags
  /* -- Note --------------------------------------------------------------- **
  ** The 'ImageData' class contains a 'Flags' class which is shared between  **
  ** five different classes, 'Atlas', 'Image', 'ImageData', 'Font' and       **
  ** 'Texture' so it's important we don't duplicate values here.             **
  ** -- Font loading flags (Only used in 'Font' class) --------------------- */
  IL_NONE                   {Flag(0)}, // No flags?
  FF_STROKETYPE2            {Flag(1)}, // True stroke but more buggy?
  FF_USEGLYPHSIZE           {Flag(2)}, // Use image glyph size for advance?
  FF_ROUNDADVANCE           {Flag(3)}, // Do round() on advance width?
  FF_FLOORADVANCE           {Flag(4)}, // Do floor() on advance width?
  FF_CEILADVANCE            {Flag(5)}, // Do ceil() on advance width?
  /* -- Font loader public mask bits --------------------------------------- */
  FF_MASK{ FF_USEGLYPHSIZE|FF_STROKETYPE2|FF_FLOORADVANCE|FF_CEILADVANCE|
           FF_ROUNDADVANCE },
  /* -- Font types --------------------------------------------------------- */
  FT_FREETYPE               {Flag(6)}, // Font is a freetype font?
  FT_BITMAP                 {Flag(7)}, // Font is a static bitmap font?
  /* -- Post processing (Only used in 'Image' class) ----------------------- */
  IL_ATLAS                  {Flag(8)}, // Convert to atlas?
  IL_TOGPU                  {Flag(9)}, // Image will be loadable in OpenGL?
  IL_TO24BPP               {Flag(10)}, // Convert loaded image to 24bpp?
  IL_TO32BPP               {Flag(11)}, // Convert loaded image to 32bpp?
  IL_TOBGR                 {Flag(12)}, // Convert loaded image to BGR(A)?
  IL_TORGB                 {Flag(13)}, // Convert loaded image to RGB(A)?
  IL_TOBINARY              {Flag(14)}, // Convert loaded image to BINARY?
  IL_REVERSE               {Flag(15)}, // Force reverse the image?
  /* -- Force load formats (Only used in 'Image' class) -------------------- */
  IL_FCE_PNG               {Flag(24)}, // Force load as PNG?
  IL_FCE_JPG               {Flag(25)}, // Force load as JPEG?
  IL_FCE_GIF               {Flag(26)}, // Force load as GIF?
  IL_FCE_DDS               {Flag(27)}, // Force load as DDS?
  /* -- Image loader public mask bits -------------------------------------- */
  IL_MASK{ IL_TOGPU|IL_TO24BPP|IL_TO32BPP|IL_TOBGR|IL_TORGB|IL_TOBINARY|
    IL_REVERSE|IL_ATLAS|IL_FCE_JPG|IL_FCE_PNG|IL_FCE_GIF|IL_FCE_DDS },
  /* -- Active flags (Only used in 'Image' class) ----------------------- */
  IA_TOGPU                 {Flag(32)}, // Image converted to load in GL?
  IA_TO24BPP               {Flag(33)}, // Converted image to 24bpp?
  IA_TO32BPP               {Flag(34)}, // Converted image to 32bpp?
  IA_TOBGR                 {Flag(35)}, // Converted image to BGR(A)?
  IA_TORGB                 {Flag(36)}, // Converted image to RGB(A)?
  IA_TOBINARY              {Flag(37)}, // Converted image to BINARY?
  IA_REVERSE               {Flag(38)}, // Force reversed the image?
  IA_ATLAS                 {Flag(39)}, // Converted to atlas?
  /* -- Image loaded flags (Only used in 'ImageData' class) ---------------- */
  IF_MIPMAPS               {Flag(48)}, // Bitmap has mipmaps?
  IF_REVERSED              {Flag(49)}, // Bitmap FILE has reversed pixels?
  IF_COMPRESSED            {Flag(50)}, // Bitmap is compressed?
  IF_DYNAMIC               {Flag(51)}, // Bitmap is dynamically created?
  IF_PALETTE               {Flag(52)}, // A palette is included?
  /* -- Texture loaded flags (Only used in 'Image' class) ------------------ */
  TF_DELETE                {Flag(55)}, // Marked for deletion?
  /* -- Image purpose (help with debugging) -------------------------------- */
  IP_IMAGE                 {Flag(61)}, // Image is stand-alone?
  IP_TEXTURE               {Flag(62)}, // Image is part of a Texture class?
  IP_FONT                  {Flag(63)}, // Image is part of a Font class?
  IP_ATLAS                 {Flag(64)}  // Image is part of an Atlas class?
);/* ----------------------------------------------------------------------- */
struct ImageSlot :                     // Members initially public
  /* -- Initialisers ------------------------------------------------------- */
  public Memory,                       // Memory data
  public DimUInt                       // Bitmap dimensions
{ /* -- Init constructor --------------------------------------------------- */
  ImageSlot(Memory &&mData, const unsigned int uiW, const unsigned int uiH) :
    /* -- Initialisers ----------------------------------------------------- */
    Memory{ StdMove(mData) },          // Move memory in place
    DimUInt{ uiW, uiH }                // Set dimensions
    /* -- No code ---------------------------------------------------------- */
    {}
};/* ----------------------------------------------------------------------- */
typedef vector<ImageSlot> SlotList;    // list of bitmaps
/* ------------------------------------------------------------------------- */
enum BitDepth : unsigned int           // Human readable bit-depths
{ /* ----------------------------------------------------------------------- */
  BD_NONE                        =  0, // Not initialised yet
  BD_BINARY                      =  1, // Binary format (8 pixels per byte)
  BD_GRAY                        =  8, // Gray channel format
  BD_GRAYALPHA                   = 16, // Gray + alpha channel format
  BD_RGB                         = 24, // 1 pixel per 3 bytes (R+G+B)
  BD_RGBA                        = 32  // 1 pixel per 4 bytes (R+G+B+Alpha)
};/* ----------------------------------------------------------------------- */
class ImageData :                      // Members initially private
  /* ----------------------------------------------------------------------- */
  public ImageFlags,                   // Shared with 'Image' class if needed
  public DimUInt                       // Bitmap dimensions
{ /* ----------------------------------------------------------------------- */
  BitDepth         bdDepth;            // Image bits per pixel
  ByteDepth        byDepth;            // Image bytes per pixel
  TextureType      ttType;             // Image colour-byte bits
  size_t           stAlloc;            // Image data allocated in slots
  /* ------------------------------------------------------------ */ protected:
  SlotList         slSlots;            // Image data
  /* --------------------------------------------------------------- */ public:
  DimUInt          duTileOR;           // Override tile size if desired
  size_t           stTiles;            // Override number of tiles
  /* ----------------------------------------------------------------------- */
  void ImageDataSwap(ImageData &imdRef)
  { // Merge flags, don't swap
    FlagSet(imdRef.FlagGet());
    // Swap image lib data
    DimSwap(imdRef);
    slSlots.swap(imdRef.slSlots);
    // Swap values
    swap(bdDepth, imdRef.bdDepth);
    swap(byDepth, imdRef.byDepth);
    swap(ttType, imdRef.ttType);
    swap(stAlloc, imdRef.stAlloc);
    // Swap tiles and dimensions
    duTileOR.DimSwap(imdRef.duTileOR);
    swap(stTiles, imdRef.stTiles);
  }
  /* -- Set width and height and return if they are valid ------------------ */
  bool SetDimSafe(const unsigned int uiNWidth, const unsigned int uiNHeight)
    { DimSet(uiNWidth, uiNHeight); return uiNWidth && uiNHeight; }
  /* -- Set bits per pixel ------------------------------------------------- */
  void SetBitsPerPixel(const BitDepth bdNBPP) { bdDepth = bdNBPP; }
  template<typename IntType>void SetBitsPerPixelCast(const IntType uiNBPP)
    { SetBitsPerPixel(static_cast<BitDepth>(uiNBPP)); }
  /* -- Set bytes per pixel ------------------------------------------------ */
  void SetBytesPerPixel(const ByteDepth byNBPP) { byDepth = byNBPP; }
  template<typename IntType>void SetBytesPerPixelCast(const IntType uiNBPP)
    { SetBytesPerPixel(static_cast<ByteDepth>(uiNBPP)); }
  /* -- Set bits per pixel and auto update bytes per pixel ----------------- */
  void SetBitsAndBytesPerPixel(const BitDepth bdNBPP)
    { SetBitsPerPixel(bdNBPP); SetBytesPerPixelCast(bdNBPP / 8); }
  template<typename IntType>
    void SetBitsAndBytesPerPixelCast(const IntType uiNBPP)
      { SetBitsAndBytesPerPixel(static_cast<BitDepth>(uiNBPP)); }
  /* -- Set bytes per pixel and auto update bits per pixel ----------------- */
  void SetBytesAndBitsPerPixel(const ByteDepth byNBPP)
    { SetBytesPerPixel(byNBPP); SetBitsPerPixelCast(byNBPP * 8); }
  template<typename IntType>
    void SetBytesAndBitsPerPixelCast(const IntType uiNBPP)
      { SetBytesAndBitsPerPixel(static_cast<ByteDepth>(uiNBPP)); }
  /* ----------------------------------------------------------------------- */
  size_t GetAlloc() const { return stAlloc; }
  /* ----------------------------------------------------------------------- */
#define FH(n, f) \
  bool Is ## n() const { return FlagIsSet(f); } \
  bool IsNot ## n() const { return !Is ## n(); } \
  void Set ## n(bool bState=true) { FlagSetOrClear(f, bState); } \
  void Clear ## n() { Set ## n(false); }
  /* ----------------------------------------------------------------------- */
  FH(FontFreeType,     FT_FREETYPE)    // Is/IsNot/Set/ClearFontFreeType
  FH(FontBitmap,       FT_BITMAP)      // Is/IsNot/Set/ClearFontBitmap
  FH(Mipmaps,          IF_MIPMAPS)     // Is/IsNot/Set/ClearMipmaps
  FH(Reversed,         IF_REVERSED)    // Is/IsNot/Set/ClearReversed
  FH(Compressed,       IF_COMPRESSED)  // Is/IsNot/Set/ClearCompressed
  FH(Dynamic,          IF_DYNAMIC)     // Is/IsNot/Set/ClearDynamic
  FH(Palette,          IF_PALETTE)     // Is/IsNot/Set/ClearPalette
  FH(LoadAsDDS,        IL_FCE_DDS)     // Is/IsNot/Set/ClearLoadAsDDS
  FH(LoadAsGIF,        IL_FCE_GIF)     // Is/IsNot/Set/ClearLoadAsGIF
  FH(LoadAsJPG,        IL_FCE_JPG)     // Is/IsNot/Set/ClearLoadAsJPG
  FH(LoadAsPNG,        IL_FCE_PNG)     // Is/IsNot/Set/ClearLoadAsPNG
  FH(ConvertAtlas,     IL_ATLAS)       // Is/IsNot/Set/ClearConvertAtlas
  FH(ConvertReverse,   IL_REVERSE)     // Is/IsNot/Set/ClearConvertReverse
  FH(ConvertRGB,       IL_TO24BPP)     // Is/IsNot/Set/ClearConvertRGB
  FH(ConvertRGBA,      IL_TO32BPP)     // Is/IsNot/Set/ClearConvertRGBA
  FH(ConvertBGROrder,  IL_TOBGR)       // Is/IsNot/Set/ClearConvertBGROrder
  FH(ConvertBinary,    IL_TOBINARY)    // Is/IsNot/Set/ClearConvertBinary
  FH(ConvertGPUCompat, IL_TOGPU)       // Is/IsNot/Set/ClearConvertGPUCompat
  FH(ConvertRGBOrder,  IL_TORGB)       // Is/IsNot/Set/ClearConvertRGBOrder
  FH(ActiveAtlas,      IA_ATLAS)       // Is/IsNot/Set/ClearActiveAtlas
  FH(ActiveReverse,    IA_REVERSE)     // Is/IsNot/Set/ClearActiveReverse
  FH(ActiveRGB,        IA_TO24BPP)     // Is/IsNot/Set/ClearActiveRGB
  FH(ActiveRGBA,       IA_TO32BPP)     // Is/IsNot/Set/ClearActiveRGBA
  FH(ActiveBGROrder,   IA_TOBGR)       // Is/IsNot/Set/ClearActiveBGROrder
  FH(ActiveBinary,     IA_TOBINARY)    // Is/IsNot/Set/ClearActiveBinary
  FH(ActiveGPUCompat,  IA_TOGPU)       // Is/IsNot/Set/ClearActiveGPUCompat
  FH(ActiveRGBOrder,   IA_TORGB)       // Is/IsNot/Set/ClearActiveRGBOrder
  FH(PurposeFont,      IP_FONT)        // Is/IsNot/Set/ClearPurposeFont
  FH(PurposeImage,     IP_IMAGE)       // Is/IsNot/Set/ClearPurposeImage
  FH(PurposeTexture,   IP_TEXTURE)     // Is/IsNot/Set/ClearPurposeTexture
  /* ----------------------------------------------------------------------- */
#undef FH                              // Done with this macro
  /* ----------------------------------------------------------------------- */
  void SetPixelType(const TextureType ttNType) { ttType = ttNType; }
  /* ----------------------------------------------------------------------- */
  TextureType GetPixelType() const { return ttType; }
  /* ----------------------------------------------------------------------- */
  size_t TotalPixels() const
    { return DimGetWidth<size_t>() * DimGetHeight<size_t>(); }
  /* ----------------------------------------------------------------------- */
  template<typename IntType=decltype(bdDepth)>
    IntType GetBitsPerPixel() const
      { return static_cast<IntType>(bdDepth); }
  /* ----------------------------------------------------------------------- */
  template<typename IntType=decltype(byDepth)>
    IntType GetBytesPerPixel() const
      { return static_cast<IntType>(byDepth); }
  /* -- Get slots ---------------------------------------------------------- */
  SlotList &GetSlots() { return slSlots; }
  /* -- Add a new slot ----------------------------------------------------- */
  void AddSlot(Memory &mData, const unsigned int uiSWidth,
    const unsigned int uiSHeight)
  { // Add the slot moving the memory over
    GetSlots().push_back({ StdMove(mData), uiSWidth, uiSHeight });
    // Add to memory bytes allocated counter
    IncreaseAlloc(GetSlots().back().MemSize());
  }
  /* -- Add a new slot using our image size -------------------------------- */
  void AddSlot(Memory &mData)
    { AddSlot(mData, DimGetWidth(), DimGetHeight()); }
  /* -- Helps with one-liners (temporary variables) ------------------------ */
  void AddSlot(Memory &&mData) { AddSlot(mData); }
  /* -- Get read-only slots ------------------------------------------------ */
  const SlotList &GetSlotsConst() const { return slSlots; }
  /* -- Is no slots? ------------------------------------------------------- */
  bool IsNoSlots() const { return GetSlotsConst().empty(); }
  /* -- Clear slots -------------------------------------------------------- */
  void ClearSlots() { GetSlots().clear(); }
  /* -- Recover slot memory ------------------------------------------------ */
  void CompactSlots() { GetSlots().shrink_to_fit(); }
  /* -- Get slots count ---------------------------------------------------- */
  size_t GetSlotCount() const { return GetSlotsConst().size(); }
  /* -- Set allocated data size -------------------------------------------- */
  void SetAlloc(const size_t stNAlloc) { stAlloc = stNAlloc; }
  /* -- Reserve allocated slots -------------------------------------------- */
  void ReserveSlots(const size_t stCount) { GetSlots().reserve(stCount); }
  /* -- Increase allocated data size --------------------------------------- */
  void IncreaseAlloc(const size_t stNAlloc) { stAlloc += stNAlloc; }
  /* -- Decrease allocated data size --------------------------------------- */
  void DecreaseAlloc(const size_t stNAlloc) { stAlloc -= stNAlloc; }
  /* -- Adjust allocation from old and new value --------------------------- */
  void AdjustAlloc(const size_t stOldAlloc, const size_t stNewAlloc)
  { // If new alloc is greater then the total value increased
    if(stNewAlloc > stOldAlloc) IncreaseAlloc(stNewAlloc - stOldAlloc);
    // If the new alloc is lesser then the total value decreased
    else if(stNewAlloc < stOldAlloc) DecreaseAlloc(stOldAlloc - stNewAlloc);
  }
  /* -- Clear slots and allocation size ------------------------------------ */
  void Clear() { SetAlloc(0); ClearSlots(); }
  /* -- Reset all data ----------------------------------------------------- */
  void ResetAllData()
  { // Reset dimensions and pixel data
    DimSet();
    SetBitsPerPixel(BD_NONE);
    SetBytesPerPixel(BY_NONE);
    SetPixelType(TT_NONE);
    // Remove image property flags
    ClearMipmaps();
    ClearReversed();
    ClearCompressed();
    ClearPalette();
    // Clear slots list and set allocation to zero
    Clear();
  }
  /* -- Constructor with default purpose ----------------------------------- */
  explicit ImageData(const ImageFlagsConst ifcPurpose = IP_IMAGE) :
    /* -- Initialisers ----------------------------------------------------- */
    ImageFlags{ ifcPurpose },          // Set initial flags
    bdDepth(BD_NONE),                  // Bit depth not initialised yet
    byDepth(BY_NONE),                  // Bytes per pixel not initialised yet
    ttType(TT_NONE),                   // Pixel type not initialised yet
    stAlloc(0),                        // Allocated memory not initialised yet
    stTiles(0)                         // No number of tiles
    /* -- Code ------------------------------------------------------------- */
    {}                                 // Nothing else to do
};/* ----------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
