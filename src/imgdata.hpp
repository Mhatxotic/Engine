/* == IMGDATA.HPP ========================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## Sets up the class to hold image data and metadata.                  ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IImageData {                 // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IDim::P;               using namespace IFlags::P;
using namespace IImageDef::P;          using namespace IImageSlot::P;
using namespace IMemory::P;            using namespace IStd::P;
using namespace ITexDef::P;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* ------------------------------------------------------------------------- */
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
    StdSwap(bdDepth, imdRef.bdDepth);
    StdSwap(byDepth, imdRef.byDepth);
    StdSwap(ttType, imdRef.ttType);
    StdSwap(stAlloc, imdRef.stAlloc);
    // Swap tiles and dimensions
    duTileOR.DimSwap(imdRef.duTileOR);
    StdSwap(stTiles, imdRef.stTiles);
  }
  /* ----------------------------------------------------------------------- */
  void ImageDataCopy(ImageData &imdRef)
  { // Copy flags
    FlagReset(imdRef.FlagGet());
    // Copy image lib data
    DimSet(imdRef);
    slSlots = imdRef.slSlots;
    // Copy values
    bdDepth = imdRef.bdDepth;
    byDepth = imdRef.byDepth;
    ttType = imdRef.ttType;
    stAlloc = imdRef.stAlloc;
    // Swap tiles and dimensions
    duTileOR.DimSet(imdRef);
    stTiles = imdRef.stTiles;
  }
  /* -- Set width and height and return if they are valid ------------------ */
  bool ImageDataSetDimSafe(const unsigned uNWidth, const unsigned uNHeight)
    { DimSet(uNWidth, uNHeight); return uNWidth && uNHeight; }
  /* -- Set bits per pixel ------------------------------------------------- */
  void ImageDataSetBitsPerPixel(const BitDepth bdNBPP) { bdDepth = bdNBPP; }
  void SetBitsPerPixelCast(const auto aNBPP)
    { ImageDataSetBitsPerPixel(static_cast<BitDepth>(aNBPP)); }
  /* -- Set bytes per pixel ------------------------------------------------ */
  void ImageDataSetBytesPerPixel(const ByteDepth byNBPP) { byDepth = byNBPP; }
  void SetBytesPerPixelCast(const auto aNBPP)
    { ImageDataSetBytesPerPixel(static_cast<ByteDepth>(aNBPP)); }
  /* -- Set bits per pixel and auto update bytes per pixel ----------------- */
  void ImageDataSetBitsAndBytesPerPixel(const BitDepth bdNBPP)
    { ImageDataSetBitsPerPixel(bdNBPP); SetBytesPerPixelCast(bdNBPP / 8); }
  void ImageDataSetBitsAndBytesPerPixelCast(const auto aNBPP)
    { ImageDataSetBitsAndBytesPerPixel(static_cast<BitDepth>(aNBPP)); }
  /* -- Set bytes per pixel and auto update bits per pixel ----------------- */
  void ImageDataSetBytesAndBitsPerPixel(const ByteDepth byNBPP)
    { ImageDataSetBytesPerPixel(byNBPP); SetBitsPerPixelCast(byNBPP * 8); }
  void ImageDataSetBytesAndBitsPerPixelCast(const auto aNBPP)
    { ImageDataSetBytesAndBitsPerPixel(static_cast<ByteDepth>(aNBPP)); }
  /* ----------------------------------------------------------------------- */
  size_t ImageDataGetAlloc() const { return stAlloc; }
  /* ----------------------------------------------------------------------- */
#define FH(n, f) \
  bool ImageDataIs##n() const { return FlagIsSet(f); } \
  bool ImageDataIsNot##n() const { return !ImageDataIs##n(); } \
  void ImageDataSet##n(bool bState = true) { FlagSetOrClear(f, bState); } \
  void ImageDataClear##n() { ImageDataSet##n(false); }
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
  FH(LoadAsJPEG,       IL_FCE_JPEG)    // Is/IsNot/Set/ClearLoadAsJPEG
  FH(LoadAsPNG,        IL_FCE_PNG)     // Is/IsNot/Set/ClearLoadAsPNG
  FH(LoadAsWEBP,       IL_FCE_WEBP)    // Is/IsNot/Set/ClearLoadAsWEBP
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
  void ImageDataSetPixelType(const TextureType ttNType) { ttType = ttNType; }
  /* ----------------------------------------------------------------------- */
  TextureType ImageDataGetPixelType() const { return ttType; }
  /* ----------------------------------------------------------------------- */
  size_t ImageDataGetTotalPixels() const
    { return DimGetWidth<size_t>() * DimGetHeight<size_t>(); }
  /* ----------------------------------------------------------------------- */
  decltype(bdDepth) ImageDataGetBitsPerPixel() const { return bdDepth; }
  /* ----------------------------------------------------------------------- */
  decltype(byDepth) ImageDataGetBytesPerPixel() const { return byDepth; }
  /* -- Get slots ---------------------------------------------------------- */
  SlotList &ImageDataGetSlots() { return slSlots; }
  const SlotList &ImageDataGetSlots() const { return slSlots; }
  /* -- Add a new slot ----------------------------------------------------- */
  void ImageDataAddSlot(Memory &mData, const unsigned uSWidth,
    const unsigned uSHeight)
  { // Add the slot moving the memory over
    ImageDataGetSlots().push_back({ StdMove(mData), uSWidth, uSHeight });
    // Add to memory bytes allocated counter
    ImageDataIncreaseAlloc(ImageDataGetSlots().back().MemSize());
  }
  /* -- Add a new slot using our image size -------------------------------- */
  void ImageDataAddSlot(Memory &mData)
    { ImageDataAddSlot(mData, DimGetWidth(), DimGetHeight()); }
  /* -- Helps with one-liners (temporary variables) ------------------------ */
  void ImageDataAddSlot(Memory &&mData) { ImageDataAddSlot(mData); }
  /* -- Is no slots? ------------------------------------------------------- */
  bool ImageDataIsNoSlots() const { return ImageDataGetSlots().empty(); }
  /* -- Clear slots -------------------------------------------------------- */
  void ImageDataClearSlots() { ImageDataGetSlots().clear(); }
  /* -- Recover slot memory ------------------------------------------------ */
  void ImageDataCompactSlots() { ImageDataGetSlots().shrink_to_fit(); }
  /* -- Get slots count ---------------------------------------------------- */
  size_t ImageDataGetSlotCount() const { return ImageDataGetSlots().size(); }
  /* -- Set allocated data size -------------------------------------------- */
  void ImageDataSetAlloc(const size_t stNAlloc) { stAlloc = stNAlloc; }
  /* -- Reserve allocated slots -------------------------------------------- */
  void ImageDataReserveSlots(const size_t stCount)
    { ImageDataGetSlots().reserve(stCount); }
  /* -- Increase allocated data size --------------------------------------- */
  void ImageDataIncreaseAlloc(const size_t stNAlloc) { stAlloc += stNAlloc; }
  /* -- Decrease allocated data size --------------------------------------- */
  void ImageDataDecreaseAlloc(const size_t stNAlloc) { stAlloc -= stNAlloc; }
  /* -- Adjust allocation from old and new value --------------------------- */
  void ImageDataAdjustAlloc(const size_t stOldAlloc, const size_t stNewAlloc)
  { // If new alloc is greater then the total value increased
    if(stNewAlloc > stOldAlloc)
      ImageDataIncreaseAlloc(stNewAlloc - stOldAlloc);
    // If the new alloc is lesser then the total value decreased
    else if(stNewAlloc < stOldAlloc)
      ImageDataDecreaseAlloc(stOldAlloc - stNewAlloc);
  }
  /* -- Clear slots and allocation size ------------------------------------ */
  void ImageDataClear() { ImageDataSetAlloc(0); ImageDataClearSlots(); }
  /* -- Reset all data ----------------------------------------------------- */
  void ImageDataResetAllData()
  { // Reset dimensions and pixel data
    DimSet();
    ImageDataSetBitsPerPixel(BD_NONE);
    ImageDataSetBytesPerPixel(BY_NONE);
    ImageDataSetPixelType(TT_NONE);
    // Remove image property flags
    ImageDataClearMipmaps();
    ImageDataClearReversed();
    ImageDataClearCompressed();
    ImageDataClearPalette();
    // Clear slots list and set allocation to zero
    ImageDataClear();
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
