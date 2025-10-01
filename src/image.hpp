/* == IMAGE.HPP ============================================================ **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## This module defines a class that can load and save image files and  ## **
** ## then can optionally be sent to opengl for viewing.                  ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IImage {                     // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IAsset::P;             using namespace IASync::P;
using namespace ICollector::P;         using namespace ICommon::P;
using namespace IDim::P;               using namespace IError::P;
using namespace IEvtMain::P;           using namespace IFileMap::P;
using namespace IIdent::P;             using namespace IImageDef::P;
using namespace IImageLib::P;          using namespace ILockable::P;
using namespace ILog::P;               using namespace ILuaIdent::P;
using namespace ILuaLib::P;            using namespace IMemory::P;
using namespace IOgl::P;               using namespace IStd::P;
using namespace IString::P;            using namespace ISysUtil::P;
using namespace ITexDef::P;            using namespace IUtil::P;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* == Image collector and member class ===================================== */
CTOR_BEGIN_ASYNC(Images, Image, CLHelperUnsafe,
  /* ----------------------------------------------------------------------- */
  IdMap<TextureType> idFormatModes;,   // Pixel format modes (log detail)
);/* ----------------------------------------------------------------------- */
template<typename IntType>             // Prototype
  static const string_view &ImageGetPixelFormat(const IntType);
/* ------------------------------------------------------------------------- */
CTOR_MEM_BEGIN_ASYNC_CSLAVE(Images, Image, ICHelperUnsafe),
  /* -- Base classes ------------------------------------------------------- */
  public Ident,                        // Image file name
  public AsyncLoaderImage,             // For loading images off main thread
  public Lockable,                     // Lua garbage collector instruction
  public ImageData                     // Raw image data
{ /* -- Swap image data  ------------------------------------------- */ public:
  void SwapImage(Image &imRef)
  { // Swap members with other class
    IdentSwap(imRef);                 // Image filename
    LockSwap(imRef);                  // Lua lock status
    CollectorSwapRegistration(imRef); // Collector registration
    ImageDataSwap(imRef);             // Image data and flags swap
  }
  /* -- Swap image data (so you can use temporary variables) --------------- */
  void SwapImage(Image &&imRef) { SwapImage(imRef); }
  /* -- Force the specified colour mode ------------------------------------ */
  bool ForcePixelOrder(const TextureType ttNType)
  { // Return failure if parameters are wrong
    if((ttNType == TT_BGR &&
       (GetPixelType() != TT_RGBA && GetPixelType() != TT_RGB)) ||
       (ttNType == TT_RGB &&
       (GetPixelType() != TT_BGRA && GetPixelType() != TT_BGR)))
      return false;
    // For each slot
    for(ImageSlot &isRef : GetSlots())
    { // Swap pixels
      const int iResult = ImageSwapPixels(isRef.MemPtr<char>(),
        isRef.MemSize(), GetBytesPerPixel(), 0, 2);
      if(iResult < 0)
        XC("Pixel reorder failed!",
           "Code",          iResult,
           "FromFormat",    ImageGetPixelFormat(GetPixelType()),
           "ToFormat",      ImageGetPixelFormat(ttNType),
           "BytesPerPixel", GetBytesPerPixel(),
           "Address",       isRef.MemPtr<void>(),
           "Length",        isRef.MemSize());
    } // Set new pixel type
    switch(GetPixelType())
    { case TT_RGBA : SetPixelType(TT_BGRA); break;
      case TT_RGB  : SetPixelType(TT_BGR);  break;
      case TT_BGRA : SetPixelType(TT_RGBA); break;
      case TT_BGR  : SetPixelType(TT_RGB);  break;
      default      : break;
    } // Success
    return true;
  }
  /* -- Force binary mode -------------------------------------------------- */
  bool ForceBinary(void)
  { // Ignore if already one bit or there are no slots
    if(GetBitsPerPixel() == BD_BINARY || IsNoSlots()) return false;
    // Throw error if compressed
    if(IsCompressed()) XC("Cannot binary downsample a compressed image!");
    // Must be 32bpp
    if(GetBitsPerPixel() != BD_RGBA)
      XC("Cannot binary downsample a non RGBA encoded image!",
         "BitsPerPixel", GetBitsPerPixel());
    // Calculate destination size
    const size_t stDst = TotalPixels() / CHAR_BIT;
    // Must be divisible by eight
    if(!UtilIsDivisible(static_cast<double>(stDst)))
      XC("Image size not divisible by eight!", "Size", stDst);
    // Bits lookup table
    static const array<const unsigned char, CHAR_BIT>
      ucaBits{0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};
    // For each image slot
    for(ImageSlot &isRef : GetSlots())
    { // Destination image for binary data
      Memory mDst{ stDst };
      // For each byte in the alpha channel array
      for(size_t stByte = 0, stByteOut = 0, stLimit = isRef.MemSize();
                 stByte < stLimit;
                 stByte += CHAR_BIT, ++stByteOut)
      { // Init bits
        unsigned char ucBits = 0;
        // Set bits depending on image bytes
        for(size_t stBit = 0; stBit < ucaBits.size(); ++stBit)
          if(isRef.MemReadInt<uint8_t>(stBit) > 0)
            ucBits |= ucaBits[stBit];
        // Write new bit value
        mDst.MemWriteInt<uint8_t>(stByteOut, ucBits);
      } // Update slot data
      isRef.MemSwap(mDst);
    } // Update image data
    SetBitsPerPixel(BD_BINARY);
    SetBytesPerPixel(BY_GRAY);
    SetPixelType(TT_GRAY);
    SetAlloc(stDst);
    // Success
    return true;
  }
  /* -- Reverse pixels ----------------------------------------------------- */
  bool ReversePixels(void)
  { // Compare bits per pixel
    switch(GetBitsPerPixel())
    { // Monochrome image?
      case BD_BINARY:
        // Enumerate through each slot and swap the 4-bits in each 8-bit byte.
        for(ImageSlot &isRef : GetSlots()) isRef.MemByteSwap8();
        // Done
        break;
      // 8-bits per pixel or greater?
      case BD_GRAY: case BD_GRAYALPHA: case BD_RGB: case BD_RGBA:
        // Enumerate through each slot
        for(ImageSlot &isRef : GetSlots())
        { // Create new mem block to write to
          Memory mbOut{ isRef.MemSize() };
          // Pixel size
          const size_t stStep = GetBytesPerPixel() * isRef.DimGetWidth();
          // Copy the pixels
          for(size_t stByte = 0; stByte < isRef.MemSize(); stByte += stStep)
            mbOut.MemWrite(stByte,
              isRef.MemRead(isRef.MemSize() - stStep - stByte), stStep);
          // Set new mem block for this class automatically unloading the old
          // one
          isRef.MemSwap(mbOut);
        } // Done
        break;
      // Nothing done so return and log a warning
      default: return false;
    } // Flip reversed state
    if(IsReversed()) ClearReversed();
    else SetReversed();
    // Success
    return true;
  }
  /* -- Pixel conversion process ------------------------------------------- */
  template<class PixelConversionFunction, size_t stSrcStep, size_t stDstStep,
    BitDepth bdNewBPP, TextureType ttType>
      void ConvertPixels(const char*const cpFilter)
  { // Some basic checks of parameters
    static_assert(stSrcStep > 0 && stSrcStep <= 2, "Invalid source step!");
    static_assert(stDstStep > 0 && stDstStep <= 32, "Invalid dest step!");
    // Set new bits and bytes
    SetBitsAndBytesPerPixel(bdNewBPP);
    SetPixelType(ttType);
    // New allocation size
    size_t stNewAlloc = 0;
    // For each slot
    for(ImageSlot &isRef : GetSlots())
    { // Calculate total pixels
      const size_t stTotal = isRef.DimGetWidth() * isRef.DimGetHeight();
      // Make a new memblock for the destinaiton pixels
      Memory mDst{ stTotal * GetBytesPerPixel() };
      // Quick sanity check to make sure the filters don't read/write OOB
      if(const size_t stUnpadded = isRef.MemSize() % stSrcStep)
        XC("Source image dimensions not acceptable for filter!",
           "File",   IdentGet(),            "Width",    isRef.DimGetWidth(),
           "Height", isRef.DimGetHeight(), "Total",    stTotal,
           "Depth",  GetBytesPerPixel(),    "Filter",   cpFilter,
           "Step",   stSrcStep,             "Unpadded", stUnpadded);
      if(const size_t stUnpadded = mDst.MemSize() % stDstStep)
        XC("Destination image dimensions not acceptable for filter!",
           "File",   IdentGet(),            "Width",    isRef.DimGetWidth(),
           "Height", isRef.DimGetHeight(), "Total",    stTotal,
           "Depth",  GetBytesPerPixel(),    "Filter",   cpFilter,
           "Step",   stDstStep,             "Unpadded", stUnpadded);
      // Enumerate and filter through each pixel
      for(uint8_t *ubpSrc = isRef.MemPtr<uint8_t>(),
         *const ubpSrcEnd = isRef.MemPtrEnd<uint8_t>(),
                  *ubpDst = mDst.MemPtr<uint8_t>();
                   ubpSrc < ubpSrcEnd;
                   ubpSrc += stSrcStep,
                   ubpDst += stDstStep)
        // Call pixel function conversion
        PixelConversionFunction(ubpSrc, ubpDst);
      // Move memory on top of old memory
      isRef.MemSwap(mDst);
      // Adjust alloc size
      stNewAlloc += isRef.MemSize();
    } // Update new size
    SetAlloc(stNewAlloc);
  }
  /* -- Force luminance alpha pixel to RGB pixel type ---------------------- */
  void ConvertLuminanceAlphaToRGB(void)
  { // Class to convert a luminance alpha pixel to a RGB pixel
    struct Filter{
      inline Filter(const uint8_t*const ubpSrc, uint8_t*const ubpDst)
      { // Unpack one luminance alpha pixel into one RGB pixel
        *reinterpret_cast<uint16_t*>(ubpDst) =
          (static_cast<uint16_t>(*ubpSrc) * 0x0101);
        *reinterpret_cast<uint8_t*>(ubpDst+1) = 0xFF;
      }
    }; // Do the conversion of luminance alpha to RGB
    ConvertPixels<Filter, 2, 3, BD_RGB, TT_RGB>("LUMA>RGB");
  }
  /* -- Force luminance pixel to RGB pixel type ---------------------------- */
  void ConvertLuminanceToRGB(void)
  { // Class to convert a luminance pixel to a RGB pixel
    struct Filter{
      inline Filter(const uint8_t*const ubpSrc, uint8_t*const ubpDst)
      { // Unpack one luminance pixel into one RGB pixel
        *reinterpret_cast<uint16_t*>(ubpDst) = *ubpSrc * 0x101;
        *reinterpret_cast<uint8_t*>(ubpDst+2) = *ubpSrc;
      }
    }; // Do the conversion of luminance to RGB
    ConvertPixels<Filter, 1, 3, BD_RGB, TT_RGB>("LUM>RGB");
  }
  /* -- Force luminance alpha pixel to RGBA pixel type --------------------- */
  void ConvertLuminanceAlphaToRGBA(void)
  { // Class to convert a luminance alpha pixel to a RGBA pixel
    struct Filter{
      inline Filter(const uint8_t*const ubpSrc, uint8_t*const ubpDst)
      { // Unpack one luminance alpha pixel into one RGBA pixel
        *reinterpret_cast<uint32_t*>(ubpDst) =
          (static_cast<uint32_t>(*ubpSrc) * 0x00010101) |
          (static_cast<uint32_t>(*(ubpSrc+1)) * 0x01000000);
      }
    }; // Do the conversion of luminance alpha to RGBA
    ConvertPixels<Filter, 2, 4, BD_RGBA, TT_RGBA>("LUMA>RGBA");
  }
  /* -- Force luminance pixel to RGBA pixel type --------------------------- */
  void ConvertLuminanceToRGBA(void)
  { // Class to convert a luminance pixel to a RGBA pixel
    struct Filter{
      inline Filter(const uint8_t*const ubpSrc, uint8_t*const ubpDst)
      { // Unpack one luminance pixel into one RGBA pixel ignoring alpha
        *reinterpret_cast<uint32_t*>(ubpDst) = (*ubpSrc * 0x01010100) | 0xFF;
      }
    }; // Do the conversion of luminance alpha to RGBA
    ConvertPixels<Filter, 1, 4, BD_RGBA, TT_RGBA>("LUM>RGBA");
  }
  /* -- Force binary pixel to luminance pixel type ------------------------- */
  void ConvertBinaryToLuminance(void)
  { // Class to convert a BINARY pixel to a LUMINANCE pixel
    struct Filter{
      inline Filter(const uint8_t*const ubpSrc, uint8_t*const ubpDst)
      { // Get the packed eight pixels
        const unsigned int uiPixels = *ubpSrc;
        // Unpack eight binary pixels into eight luminance (white) pixels
        *reinterpret_cast<uint64_t*>(ubpDst) =
          (uiPixels & 0x01 ? 0xff00000000000000 : 0) | // Bit 1/8 -> Byte 1
          (uiPixels & 0x02 ? 0x00ff000000000000 : 0) | // Bit 2/8 -> Byte 2
          (uiPixels & 0x04 ? 0x0000ff0000000000 : 0) | // Bit 3/8 -> Byte 3
          (uiPixels & 0x08 ? 0x000000ff00000000 : 0) | // Bit 4/8 -> Byte 4
          (uiPixels & 0x10 ? 0x00000000ff000000 : 0) | // Bit 5/8 -> Byte 5
          (uiPixels & 0x20 ? 0x0000000000ff0000 : 0) | // Bit 6/8 -> Byte 6
          (uiPixels & 0x40 ? 0x000000000000ff00 : 0) | // Bit 7/8 -> Byte 7
          (uiPixels & 0x80 ? 0x00000000000000ff : 0);  // Bit 8/8 -> Byte 8
      }
    }; // Do the conversion of binary to luminance
    ConvertPixels<Filter, 1, 8, BD_GRAY, TT_GRAY>("BIN>LUM");
  }
  /* -- Force binary pixel to RGB pixel type ------------------------------- */
  void ConvertBinaryToRGB(void)
  { // Class to convert a BINARY pixel to a RGB pixel
    struct Filter{
      inline Filter(const uint8_t*const ubpSrc, uint8_t*const ubpDst)
      { // Get the packed eight pixels
        const unsigned int uiPixels = *ubpSrc;
        // Unpack eight BINARY (1-bit) pixels into eight RGB (24-bit) pixels.
        *reinterpret_cast<uint64_t*>(ubpDst) =         // Pixels 1 to 3A
          // Pixel order       RRGGBBRRGGBBRRGG (R=RED, G=GREEN, B=BLUE)
          (uiPixels & 0x80 ? 0xffffff0000000000 : 0) | // * Bit 8/8 -> Byte 1-3
          (uiPixels & 0x40 ? 0x000000ffffff0000 : 0) | // * Bit 7/8 -> Byte 4-6
          (uiPixels & 0x20 ? 0x000000000000ffff : 0);  // * Bit 6/8 -> Byte 7-8
        *(reinterpret_cast<uint64_t*>(ubpDst)+1) =     // Pixels 3B to 6A
          // Pixel order       BBRRGGBBRRGGBBRR (R=RED, G=GREEN, B=BLUE)
          (uiPixels & 0x20 ? 0xff00000000000000 : 0) | // * Bit 6/8 -> Byte 9
          (uiPixels & 0x10 ? 0x00ffffff00000000 : 0) | // * Bit 5/8 -> By 10-12
          (uiPixels & 0x08 ? 0x00000000ffffff00 : 0) | // * Bit 4/8 -> By 13-15
          (uiPixels & 0x04 ? 0x00000000000000ff : 0);  // * Bit 3/8 -> Byte 16
        *(reinterpret_cast<uint64_t*>(ubpDst)+2) =     // Pixels 6B to 8
          // Pixel order       GGBBRRGGBBRRGGBB (R=RED, G=GREEN, B=BLUE)
          (uiPixels & 0x04 ? 0xffff000000000000 : 0) | // * Bit 3/8 -> By 17-18
          (uiPixels & 0x02 ? 0x0000ffffff000000 : 0) | // * Bit 2/8 -> By 19-21
          (uiPixels & 0x01 ? 0x0000000000ffffff : 0);  // * Bit 1/8 -> By 22-24
      }
    }; // Do the conversion of binary to RGBA
    ConvertPixels<Filter, 1, 24, BD_RGB, TT_RGB>("BIN>RGB");
  }
  /* -- Force binary pixel to RGBA pixel type ------------------------------ */
  void ConvertBinaryToRGBA(void)
  { // Class to convert a BINARY pixel to a RGBA pixel
    struct Filter{
      inline Filter(const uint8_t*const ubpSrc, uint8_t*const ubpDst)
      { // Get the packed eight pixels
        const unsigned int uiPixels = *ubpSrc;
        // Unpack eight BINARY pixels into eight RGBA (32-bit) pixels
        *reinterpret_cast<uint64_t*>(ubpDst) =         // Pixel 1-2
          // Pixel order       RRGGBBAARRGGBBAA (R=RED,G=GREEN,B=BLUE,A=ALPHA)
          (uiPixels & 0x80 ? 0xffffffff00000000 : 0) | // * Bit 8/8 -> DWord 1
          (uiPixels & 0x40 ? 0x00000000ffffffff : 0);  // * Bit 7/8 -> DWord 2
        *(reinterpret_cast<uint64_t*>(ubpDst)+1) =     // Pixel 3-4
          // Pixel order       RRGGBBAARRGGBBAA (R=RED,G=GREEN,B=BLUE,A=ALPHA)
          (uiPixels & 0x20 ? 0xffffffff00000000 : 0) | // * Bit 6/8 -> DWord 3
          (uiPixels & 0x10 ? 0x00000000ffffffff : 0);  // * Bit 5/8 -> DWord 4
        *(reinterpret_cast<uint64_t*>(ubpDst)+2) =     // Pixel 5-6
          // Pixel order       RRGGBBAARRGGBBAA (R=RED,G=GREEN,B=BLUE,A=ALPHA)
          (uiPixels & 0x08 ? 0xffffffff00000000 : 0) | // * Bit 4/8 -> DWord 5
          (uiPixels & 0x04 ? 0x00000000ffffffff : 0);  // * Bit 3/8 -> DWord 6
        *(reinterpret_cast<uint64_t*>(ubpDst)+3) =     // Pixel 7-8
          // Pixel order       RRGGBBAARRGGBBAA (R=RED,G=GREEN,B=BLUE,A=ALPHA)
          (uiPixels & 0x02 ? 0xffffffff00000000 : 0) | // * Bit 2/8 -> DWord 7
          (uiPixels & 0x01 ? 0x00000000ffffffff : 0);  // * Bit 1/8 -> DWord 8
      }
    }; // Do the conversion of binary to RGBA
    ConvertPixels<Filter, 1, 32, BD_RGBA, TT_RGBA>("BIN>RGBA");
  }
  /* -- Concatenate tiles into a single texture ---------------------------- */
  bool MakeAtlas(void)
  { // Save number of images compacted
    stTiles = GetSlotCount();
    // Return if 1 or less slides or 1 or less bit depth or has a palette
    if(stTiles <= 1 || GetBitsPerPixel() <= BD_BINARY || IsPalette())
      return false;
    // Remaining tiles
    size_t stRemain = stTiles;
    // Take ownership current slots list and make a blank new one.
    SlotList slSrc{ StdMove(slSlots) };
    // Reset allocation
    SetAlloc(0);
    // Get first image slot
    ImageSlot &isFirst = slSrc.front();
    // Set override tile size and count
    duTileOR.DimSet(isFirst);
    // Get maximum texture size allowed
    const size_t stMaxSize = cOgl->MaxTexSize();
    // Setup compatible types needed to do the compaction
    size_t stTexWidth  = 0,            // Current destination texture width
           stTexHeight = 0,            // Current destination texture height
           stWidth     = isFirst.DimGetWidth(),  // Current tile width
           stHeight    = isFirst.DimGetHeight(), // Current tile height
           stOptSize   = stMaxSize,    // Safe texture size to use
           stNOptSize  = stOptSize,    // New texture size to test with
           stCols      = stOptSize/stWidth,  // Safe columns count
           stRows      = stOptSize/stHeight, // Safe rows count
           stNCols     = stCols,       // New columns count to test
           stNRows     = stRows;       // New rows count to test
    // Memory to hold texture data
    Memory mTexture;
    // Function to (re)calculate required texture size
    const function SetupCanvas{ [this, &stRemain, stMaxSize, &stTexWidth,
      &stTexHeight, &stWidth, &stHeight, &stOptSize, &stNOptSize, &stCols,
      &stRows, &stNCols, &stNRows, &mTexture](void)->void
    { // Now keep dividing the texture size by two until we can no longer fit
      // the needed amount of tiles inside the texture.
      while(stNOptSize && stNCols * stNRows >= stRemain)
      { // Update new size
        stOptSize = stNOptSize;
        // Record new valid values
        stCols = stNCols;
        stRows = stNRows;
        // Divide texture size by half to keep power of two textures
        stNOptSize = stOptSize / 2;
        // Calculate new columns and rows
        stNCols = stNOptSize / stWidth;
        stNRows = stNOptSize / stHeight;
      } // If we are going to need more than 1 sub-texture then reset to max
      if(!stOptSize)
      { // Use maximum texture size
        stOptSize = stMaxSize;
        // Set exact new texture size
        stTexWidth = (stOptSize / stWidth) * stWidth;
        stTexHeight = (stOptSize / stHeight) * stHeight;
      } // Now we have the new width and height
      else { stTexWidth = stCols * stWidth; stTexHeight = stRows * stHeight; }
      // Make a new texture and clear its memory
      mTexture.MemInitBlank(stTexWidth * stTexHeight * GetBytesPerPixel());
    } };
    // Calculate the texture size
    SetupCanvas();
    // Texture position and tiles remaining
    size_t stTX = 0, stTY = 0;
    // Scanline size
    const size_t stScanLine = stWidth * GetBytesPerPixel();
    // Until we have no more slots. We'll keep deleting them as we process
    // them to keep the memory usage down
    for(const ImageSlot &isRef : slSrc)
    { // Now we copy this slide into the texuree
      for(size_t stY = 0; stY < stHeight; ++stY)
        mTexture.MemWrite((((stTY + stY) * stTexWidth) + stTX) *
          GetBytesPerPixel(), isRef.MemRead(stY * stScanLine, stScanLine),
          stScanLine);
      // Add to number of tiles added
      --stRemain;
      // Move texture position and continue if we can hold another column
      stTX += stWidth;
      if(stTX + stWidth <= stTexWidth) continue;
      // Move to next row and continue if we can hold another row
      stTX = 0;
      stTY += stHeight;
      if(stTY + stHeight <= stTexHeight) continue;
      // Now off the bottom of the texture so reset Y position
      stTY = 0;
      // Add this texture as it is finished
      AddSlot(mTexture, static_cast<unsigned int>(stTexWidth),
                        static_cast<unsigned int>(stTexHeight));
      // Copy new tile sizes
      stWidth  = isRef.DimGetWidth<size_t>();
      stHeight = isRef.DimGetHeight<size_t>();
      // Set new safe values and setup next values to test
      stOptSize  = stMaxSize;
      stNOptSize = stOptSize;
      stRows     = stOptSize/stHeight;
      stNRows    = stRows;
      stCols     = stOptSize/stWidth;
      stNCols    = stCols;
      // Calculate new texture size
      SetupCanvas();
    } // A new texture has been written? Add the final slot
    if(stTX || stTY)
    { // Check if we can crop unused scanlines
      if(stTY < stTexHeight)
      { // Yes we can. Crop to where the current Y drawing position is
        stTexHeight = stTX ? stTY + stHeight : stTY;
        mTexture.MemResize(stTexWidth * stTexHeight * GetBytesPerPixel());
      } // Add the slot
      AddSlot(mTexture, static_cast<unsigned int>(stTexWidth),
                        static_cast<unsigned int>(stTexHeight));
    } // Set size of first texture
    DimSet(GetSlotsConst().front());
    // Success
    return true;
  }
  /* -- Convert palette to RGB(A) ------------------------------------------ */
  template<ByteDepth byDepth, TextureType ttType>bool ExpandPalette(void)
  { // Ignore if not paletted
    if(IsNotPalette()) return false;
    // Must only have two slots
    if(GetSlotCount() != 2)
      XC("Cannot convert palette to RGB because we need two slots!",
         "Slots", GetSlotCount());
    // Take ownership current slots list and make a blank new one.
    SlotList slSrc{ StdMove(slSlots) };
    // Reset allocation
    SetAlloc(0);
    // Get datas
    const ImageSlot &isRef = slSrc.front(), &isPalette = slSrc.back();
    // Create output buffer and enumerate through the pixels
    Memory mOut{ DimGetWidth() * DimGetHeight() * byDepth };
    // If palette and output image are same depth?
    if(isPalette.DimGetHeight() == byDepth)
    { // We can copy directlry
      for(size_t stIn = 0, stOut = 0;
                 stIn < mOut.MemSize();
               ++stIn, stOut += byDepth)
        // Read palette index from pixel data, then write the RGB value
        // from the palette to the output image.
        mOut.MemWrite(stOut,
          isPalette.MemRead(isRef.MemReadInt<uint8_t>(stIn) * byDepth),
          byDepth);
    } // Not same so less trivial
    else if constexpr(byDepth == BY_RGBA)
      for(size_t stIn = 0, stOut = 0;
                 stIn < mOut.MemSize();
               ++stIn, stOut += byDepth)
    { // Get palette location
      const size_t stPalIndex = isRef.MemReadInt<uint8_t>(stIn) *
        isPalette.DimGetHeight();
      // Get palette value
      const uint32_t ulValue = static_cast<uint32_t>(
        (isPalette.MemReadInt<uint16_t>(stPalIndex) << 8) |
         isPalette.MemReadInt<uint8_t>(stPalIndex + sizeof(uint16_t)));
      // Write new value
      mOut.MemWriteInt<uint32_t>(stOut, ulValue);
    } // Unknown
    else XC("Image expanding circumstances not implemented!");
    // Update output, we will be converting to rgb
    SetBytesAndBitsPerPixel(byDepth);
    SetPixelType(ttType);
    // Add expanded image to list
    AddSlot(mOut);
    // Success
    return true;
  }
  /* -- Convert data to GPU compatible ------------------------------------- */
  bool ConvertGPUCompatible(void)
  { // We don't have functionality to load <8bpp images in GPU yet so if this
    // flag is specified, we will make sure <8bpp textures get converted
    // properly so they can be loaded by OpenGL. This is useful if the
    // bit-depth of the bitmap isn't known and the caller wants to make sure
    // it guarantees to be loaded into OpenGL.
    switch(GetBitsPerPixel())
    { // 1bpp (BINARY)
      case BD_BINARY: ConvertBinaryToLuminance(); break;
      // Ignore anything else
      default: return false;
    } // Success
    return true;
  }
  /* -- Convert image bit-depth to to 24-bits per pixel RGB format --------- */
  bool ConvertRGB(void)
  { // Compare current bit-depth
    switch(GetBitsPerPixel())
    { // 1bpp (BINARY)
      case BD_BINARY: ConvertBinaryToRGB(); break;
      // 8bpp (LUMINANCE)
      case BD_GRAY:
        if(IsPalette()) return ExpandPalette<BY_RGB,TT_RGB>();
        ConvertLuminanceToRGB();
        break;
      // 16bpp (LUMINANCE+ALPHA)
      case BD_GRAYALPHA: ConvertLuminanceAlphaToRGB(); break;
      // Ignore anything else
      default: return false;
    } // Success
    return true;
  }
  /* -- Convert image bit-depth to to 32-bits per pixel RGBA format -------- */
  bool ConvertRGBA(void)
  { // Compare current bit-depth
    switch(GetBitsPerPixel())
    { // 1bpp (BINARY)
      case BD_BINARY: ConvertBinaryToRGBA(); break;
      // 8bpp (LUMINANCE)
      case BD_GRAY:
        if(IsPalette()) return ExpandPalette<BY_RGBA,TT_RGBA>();
        ConvertLuminanceToRGBA();
        break;
      // 16bpp (LUMINANCE+ALPHA)
      case BD_GRAYALPHA: ConvertLuminanceAlphaToRGBA(); break;
      // Ignore anything else
      default: return false;
    } // Success
    return true;
  }
  /* -- Apply filters ------------------------------------------------------ */
  void ApplyFilters(void)
  { // Record current parameters
    const DimUInt duOld{ *this };
    const size_t stSlots = GetSlotCount();
    const BitDepth bdOld = GetBitsPerPixel();
    const ByteDepth byOld = GetBytesPerPixel();
    const TextureType ttOld = GetPixelType();
    const size_t stOld = GetAlloc();
    // Convert to GPU copmatible texture?
    if(IsConvertGPUCompat())
    { // If it is not already converted by the file filter?
      if(IsNotActiveGPUCompat())
      { // Log that we're running this function
        cLog->LogDebugExSafe("Image '$' safe pixel depth request...",
          IdentGet());
        // Run the function and log success if succeeded
        if(ConvertGPUCompatible())
        { // Log the successful result
          cLog->LogInfoExSafe("Image '$' pixel-depth now safe.", IdentGet());
          // Set activated flag
          SetActiveGPUCompat();
        } // Conversion did not happen so log that too
        else cLog->LogDebugExSafe("Image '$' skipped safe pixel depth!",
          IdentGet());
      } // Conversion handled by codec
      else cLog->LogDebugExSafe("Image '$' safe pixel depth by codec!",
        IdentGet());
    } // If a request to convert to 24-bits per pixel?
    else if(IsConvertRGB())
    { // If it is not already converted by the file filter?
      if(IsNotActiveRGB())
      { // Log that we're running this function
        cLog->LogDebugExSafe("Image '$' force 24-bit request...", IdentGet());
        // Run the function and log success if succeeded
        if(ConvertRGB())
        { // Log the successful result
          cLog->LogInfoExSafe("Image '$' pixel depth now 24-bit.", IdentGet());
          // Set activated flag
          SetActiveRGB();
        } // Conversion did not happen so log that too
        else cLog->LogDebugExSafe("Image '$' skipped converting to 24-bit!",
          IdentGet());
      } // Conversion handled by codec
      else cLog->LogDebugExSafe("Image '$' converted to 24-bit by codec!",
        IdentGet());
    } // If a request to convert to RGBA?
    else if(IsConvertRGBA())
    { // If it is not already converted by the file filter?
      if(IsNotActiveRGBA())
      { // Log that we're running this function
        cLog->LogDebugExSafe("Image '$' force 32-bit request...", IdentGet());
        // Run the function and log success if succeeded
        if(ConvertRGBA())
        { // Log the successful result
          cLog->LogInfoExSafe("Image '$' pixel depth now 32-bit.", IdentGet());
          // Set activated flag
          SetActiveRGBA();
        } // Conversion did not happen so log that too
        else cLog->LogDebugExSafe("Image '$' skipped converting to 32-bit!",
          IdentGet());
      } // Conversion handled by codec
      else cLog->LogDebugExSafe("Image '$' converted to 32-bit by codec!",
        IdentGet());
    } // To BGR colour mode?
    if(IsConvertBGROrder())
    { // If it is not already converted by the file filter?
      if(IsNotActiveBGROrder())
      { // Log that we're running this function
        cLog->LogDebugExSafe("Image '$' re-order to BGR request...",
          IdentGet());
        // Run the function and log success if succeeded
        if(ForcePixelOrder(TT_BGR))
        { // Log the successful result
          cLog->LogInfoExSafe("Image '$' re-ordered to BGR.", IdentGet());
          // Set activated flag
          SetActiveBGROrder();
        } // Conversion did not happen so log that too
        else cLog->LogDebugExSafe("Image '$' re-order to BGR skipped!",
          IdentGet());
      } // Conversion handled by codec
      else cLog->LogDebugExSafe("Image '$' re-ordered to BGR by codec!",
        IdentGet());
    } // To RGB colour mode
    else if(IsConvertRGBOrder())
    { // If it is not already converted by the file filter?
      if(IsNotActiveRGBOrder())
      { // Log that we're running this function
        cLog->LogDebugExSafe("Image '$' re-order to RGB request...",
          IdentGet());
        // Run the function and log success if succeeded
        if(ForcePixelOrder(TT_RGB))
        { // Log the successful result
          cLog->LogInfoExSafe("Image '$' re-ordered now RGB.", IdentGet());
          // Set activated flag
          SetActiveRGBOrder();
        } // Conversion did not happen so log that too
        else cLog->LogDebugExSafe("Image '$' re-order to RGB skipped!",
          IdentGet());
      } // Conversion handled by codec
      else cLog->LogDebugExSafe("Image '$' re-ordered to RGB by codec!",
        IdentGet());
    } // Reverse the image pixels
    if(IsConvertReverse())
    { // If it is not already converted by the file filter?
      if(IsNotActiveReverse())
      { // Log that we're running this function
        cLog->LogDebugExSafe("Image '$' pixel reversal request...",
          IdentGet());
        // Run the function and log success if succeeded
        if(ReversePixels())
        { // Log the successful result
          cLog->LogInfoExSafe("Image '$' pixels reversed!", IdentGet());
          // Set activated flag
          SetActiveReverse();
        } // Conversion did not happen so log that too
        else cLog->LogDebugExSafe("Image '$' skipped pixel reversal!",
          IdentGet());
      } // Conversion handled by codec
      else cLog->LogDebugExSafe("Image '$' reversal by codec!",
        IdentGet());
    } // To binary colour mode
    if(IsConvertBinary())
    { // If it is not already converted by the file filter?
      if(IsNotActiveBinary())
      { // Log that we're running this function
        cLog->LogDebugExSafe("Image '$' downsample to binary request...",
          IdentGet());
        // Run the function and log success if succeeded
        if(ForceBinary())
        { // Log the successful result
          cLog->LogInfoExSafe("Image '$' downsample to binary completed.",
            IdentGet());
          // Set activated flag
          SetActiveBinary();
        } // Conversion did not happen so log that too
        else cLog->LogDebugExSafe("Image '$' downsample to binary skipped!",
          IdentGet());
      } // Conversion handled by codec
      else cLog->LogDebugExSafe("Image '$' downsampled to binary by codec!",
        IdentGet());
    } // Compact all slides into a single texture if possible
    if(IsConvertAtlas())
    { // If it is not already converted by the file filter?
      if(IsNotActiveAtlas())
      { // Log that we're running this function
        cLog->LogDebugExSafe("Image '$' compact request...", IdentGet());
        // Run the function and log success if succeeded
        if(MakeAtlas())
        { // Log the successful result
          cLog->LogInfoExSafe("Image '$' compacted.", IdentGet());
          // Set activated flag
          SetActiveAtlas();
        } // Conversion did not happen so log that too
        else cLog->LogDebugExSafe("Image '$' compact skipped!", IdentGet());
      } // Conversion handled by codec
      else cLog->LogDebugExSafe("Image '$' compact handled by codec!",
        IdentGet());
    } // Report status if we acticated anything
    if(IsActiveAtlas() || IsActiveReverse() || IsActiveRGB() ||
       IsActiveRGBA() || IsActiveBGROrder() || IsActiveBinary() ||
       IsActiveGPUCompat() || IsActiveRGBOrder())
      cLog->LogDebugExSafe("Image '$' filtering completed...$$$$$$$$$$$$$$$",
        IdentGet(),
        duTileOR.DimGetWidth() && duTileOR.DimGetHeight() ?
          StrFormat("\n- Tile size override: $x$.",
            duTileOR.DimGetWidth(), duTileOR.DimGetHeight()) :
            cCommon->CommonBlank(),
        stTiles ? StrFormat("\n- Tile count override: $.", stTiles) :
          cCommon->CommonBlank(),
        duOld.DimGetWidth() != DimGetWidth() ||
        duOld.DimGetHeight() != DimGetHeight() ?
          StrFormat("\n- Bitmap dimensions: $x$ -> $x$.",
            duOld.DimGetWidth(), duOld.DimGetHeight(),
            DimGetWidth(), DimGetHeight()) : cCommon->CommonBlank(),
        stSlots != GetSlotCount() ?
          StrFormat("\n- Bitmap slots: $ -> $.",
            stSlots, GetSlotCount()) : cCommon->CommonBlank(),
        bdOld != GetBitsPerPixel() ?
          StrFormat("\n- Pixel depth: $<$> -> $<$>.", bdOld, byOld,
            GetBitsPerPixel(), GetBytesPerPixel()) : cCommon->CommonBlank(),
        ttOld != GetPixelType() ?
          StrFormat("\n- Pixel type: $<$$> -> $<$$>.",
            ImageGetPixelFormat(ttOld), hex, ttOld,
            ImageGetPixelFormat(GetPixelType()), GetPixelType(), dec) :
              cCommon->CommonBlank(),
        stOld != GetAlloc() ?
          StrFormat("\n- Memory usage: $ -> $ bytes.",
            stOld, GetAlloc()) : cCommon->CommonBlank(),
        IsActiveAtlas() ? "\n- Slots compacted to atlas." :
          cCommon->CommonBlank(),
        IsActiveReverse() ? "\n- Pixels reversed." :
          cCommon->CommonBlank(),
        IsActiveRGB() ? "\n- Pixels converted to 24-bit." :
          cCommon->CommonBlank(),
        IsActiveRGBA() ? "\n- Pixels converted to 32-bit." :
          cCommon->CommonBlank(),
        IsActiveBGROrder() ? "\n- Pixels converted to BGR order." :
          cCommon->CommonBlank(),
        IsActiveRGBOrder() ? "\n- Pixels converted to RGB order." :
          cCommon->CommonBlank(),
        IsActiveBinary() ? "\n- Pixels converted to monochrome." :
          cCommon->CommonBlank(),
        IsActiveGPUCompat() ? "\n- Pixels made OpenGL compatible." :
          cCommon->CommonBlank());
  }
  /* -- Load specified image ----------------------------------------------- */
  void AsyncReady(FileMap &fmData)
  { // Force load a certain type of image (for speed?) but in Async mode,
    // force detection doesn't really matter as much, but overall, still
    // needed if speed is absolutely neccesary.
    if(IsLoadAsPNG()) ImageLoad(IFMT_PNG, fmData, *this);
    else if(IsLoadAsJPG()) ImageLoad(IFMT_JPG, fmData, *this);
    else if(IsLoadAsGIF()) ImageLoad(IFMT_GIF, fmData, *this);
    else if(IsLoadAsDDS()) ImageLoad(IFMT_DDS, fmData, *this);
    // Auto detection of image
    else ImageLoad(fmData, *this);
    // Apply filters if image has no special circumstances
    if(IsNotCompressed() && IsNotMipmaps()) ApplyFilters();
    // Recover slots memory if they were modified
    CompactSlots();
  }
  /* -- Reload specified image --------------------------------------------- */
  void ReloadData(void)
  { // Load the file from disk or archive
    FileMap fmData{ AssetExtract(IdentGet()) };
    // Reset memory usage to zero
    SetAlloc(0);
    // Run codecs and filters on it
    AsyncReady(fmData);
  }
  /* -- Save image using a type id ----------------------------------------- */
  void SaveFile(const string &strFile, const size_t stSId,
    const ImageFormat ifPId)
      const { ImageSave(ifPId, strFile, *this, GetSlotsConst()[stSId]); }
  /* -- Load image from memory asynchronously ------------------------------ */
  void InitAsyncArray(lua_State*const lS, const string &strIdent, Asset &aData,
    const ImageFlagsConst ifcFlags)
  { // Set user flags
    FlagSet(ifcFlags);
    // The decoded image will be kept in memory
    SetDynamic();
    // Load image from memory asynchronously
    AsyncInitArray(lS, strIdent, "bmparray", aData);
  }
  /* -- Load image from file asynchronously -------------------------------- */
  void InitAsyncFile(lua_State*const lS, const string &strFile,
    const ImageFlagsConst ifcFlags)
  { // Set user flags
    FlagSet(ifcFlags);
    // Load image from file asynchronously
    AsyncInitFile(lS, strFile, "bmpfile");
  }
  /* -- Create a blank image for working on -------------------------------- */
  void InitBlank(const string &strIdent, const unsigned int uiBWidth,
    const unsigned int uiBHeight, const bool bAlpha, const bool bClear)
  { // Lookup table for alpha setting
    typedef pair<const BitDepth, const TextureType> BitDepthTexTypePair;
    typedef array<const BitDepthTexTypePair,2> BitDepthTexTypePairArray;
    static const BitDepthTexTypePairArray
      bdttpLookup{ { { BD_RGB, TT_RGB }, { BD_RGBA, TT_RGBA } } };
    const BitDepthTexTypePair &bdttpLookupRef =
      bdttpLookup[static_cast<size_t>(bAlpha)];
    // Set appropriate parameters
    SetBitsAndBytesPerPixel(bdttpLookupRef.first);
    SetPixelType(bdttpLookupRef.second);
    // Set other members
    IdentSet(StdMove(strIdent));
    SetDynamic();
    DimSet(uiBWidth, uiBHeight);
    // Add the raw data into a slot
    AddSlot({ TotalPixels() * GetBytesPerPixel(), bClear });
    // Load succeeded so register the block
    CollectorRegister();
  }
  /* -- Replace image from a raw image data -------------------------------- */
  void InitReplace(Memory &mSrc)
  { // Take ownership current slots list and make a blank new one.
    if(slSlots.empty()) XC("No slots in image!", "Identifier", IdentGet());
    // Get first image slot
    ImageSlot &isFirst = slSlots.front();
    // Simple check to make sure the sizes are same
    if(isFirst.MemSize() != mSrc.MemSize())
      XC("Pixel data buffer size mismatch!",
         "Identifier", IdentGet(),
         "Expected",   isFirst.MemSize(),
         "Actual",     mSrc.MemSize());
    // Now just move it over
    isFirst.MemSwap(mSrc);
  }
  /* -- Load image from a raw image data ----------------------------------- */
  void InitRaw(const string &strName, Memory &mSrc,
    const unsigned int uiBWidth, const unsigned int uiBHeight,
    const BitDepth bdBitsPP)
  { // Check that the range is valid
    const unsigned int uiMSize = 0xFFFF;
    if(!uiBWidth || !uiBHeight || uiBWidth > uiMSize || uiBHeight > uiMSize)
      XC("Image dimensions are not acceptable!",
        "File",   strName,   "Width",   uiBWidth,
        "Height", uiBHeight, "Maximum", uiMSize);
    // Set bits per pixel
    SetBitsAndBytesPerPixel(bdBitsPP);
    // Set the dimensions
    DimSet(uiBWidth, uiBHeight);
    // Expected image size
    size_t stExpect;
    // Set the pixel type and if if the type cannot be handled by the GPU?
    SetPixelType(ImageBYtoTexType(GetBytesPerPixel()));
    if(GetPixelType() == TT_NONE)
    { // Only fail if not binary
      if(GetBitsPerPixel() != BD_BINARY)
        XC("Image bits per pixel not valid!",
           "File", strName, "Depth", GetBitsPerPixel());
      // Total pixels must be divisible by 8
      if(const size_t stRemainder = TotalPixels() % CHAR_BIT)
        XC("Binary image pixel count must be divisible by eight!",
           "File", strName, "Pixels", TotalPixels(), "Remainder", stRemainder);
      // Set expected number of bits for binary image
      stExpect = TotalPixels() / CHAR_BIT;
    } // Compressed textures not supported yet
    else if(GetPixelType() >= TT_DXT1 && GetPixelType() <= TT_DXT3)
      XC("Compressed images not supported yet!",
         "File", strName, "Type", ImageGetPixelFormat(GetPixelType()));
    // Set expected number of bytes
    else stExpect = TotalPixels() * GetBytesPerPixel();
    // Check that the size matches
    if(stExpect != mSrc.MemSize())
      XC("Arguments are not valid for specified image data!",
        "File", strName, "Expect", stExpect, "Actual", mSrc.MemSize());
    // Everything looks OK, set rest of the members
    IdentSet(strName);
    SetDynamic();
    // Add the raw data into a slot
    AddSlot(mSrc);
    // Load succeeded so register the block
    CollectorRegister();
  }
  /* -- Load image from a single colour ------------------------------------ */
  void InitColour(const uint32_t ulColour)
  { // Set members
    IdentSetA("solid/0x", hex, ulColour);
    SetBitsAndBytesPerPixel(BD_RGBA);
    SetPixelType(TT_RGBA);
    SetDynamic();
    DimSet(1);
    // Make sure the specified colour is in the correct order that the GPU can
    // read. It will be interpreted as TT_RGBA. The guest will specify the
    // input as 0xAARRGGBB like FboItem::SetRGBAInt().
    const array<const uint8_t,4>ucaColour{  // Source ----- Bit - Dst Pixels
      static_cast<uint8_t>(ulColour >> 16), // 0x00[RR]0000 16-24 [0] [R]gba
      static_cast<uint8_t>(ulColour >>  8), // 0x0000[GG]00 08-16 [1] r[G]ba
      static_cast<uint8_t>(ulColour      ), // 0x000000[BB] 00-08 [2] rg[B]a
      static_cast<uint8_t>(ulColour >> 24)  // 0x[AA]000000 24-32 [3] rgb[A]
    };                                      // ------------ ----- --- ------
    // Add the image
    AddSlot({ GetBytesPerPixel(), ucaColour.data() });
    // Load succeeded so register the block
    CollectorRegister();
  }
  /* -- Init from file ----------------------------------------------------- */
  void InitFile(const string &strFileName, const ImageFlagsConst &ifcFlags)
  { // Set the loading flags
    FlagSet(ifcFlags);
    // Load the file normally
    SyncInitFileSafe(strFileName);
  }
  /* -- Init from array ---------------------------------------------------- */
  void InitArray(const string &strName, Memory &mRval,
    const ImageFlagsConst &ifcFlags)
  { // Is dynamic because it was not loaded from disk
    SetDynamic();
    // Set the loading flags
    FlagSet(ifcFlags);
    // Load the array normally
    SyncInitArray(strName, mRval);
  }
  /* -- Default constructor ------------------------------------------------ */
  Image(void) :                        // No parameters
    /* -- Initialisers ----------------------------------------------------- */
    ICHelperImage{ cImages },          // Initialise collector helper
    IdentCSlave{ cParent->CtrNext() }, // Initialise identification number
    AsyncLoaderImage{ *this, this,     // Initialise async loader
      EMC_MP_IMAGE }                   // Initialise async event id
    /* -- Code ------------------------------------------------------------- */
    { }                                // Do nothing else
  /* -- Constructor with purpose ------------------------------------------- */
  explicit Image(
    /* -- Parameters ------------------------------------------------------- */
    const ImageFlagsConst ifcPurpose) :
    /* -- Initialisers ----------------------------------------------------- */
    ICHelperImage{ cImages },          // Initialise collector helper
    IdentCSlave{ cParent->CtrNext() }, // Initialise identification number
    AsyncLoaderImage{ *this, this,     // Initialise async loader
      EMC_MP_IMAGE },                  // Initialise async event id
    ImageData{ ifcPurpose }            // Initialise purpose of image class
    /* -- Code ------------------------------------------------------------- */
    { }                                // Do nothing else
  /* -- Constructor -------------------------------------------------------- */
  explicit Image(                      // Initialise a 1x1 pixel texture
    /* -- Parameters ------------------------------------------------------- */
    const uint32_t uiColour            // 32-bit RGBA colour pixel value
    ): /* -- Initialisers -------------------------------------------------- */
    Image{}                            // Default initialisation
    /* -- Code  ------------------------------------------------------------ */
    { InitColour(uiColour); }          // Init 1x1 tex with specified colour
  /* -- Constructor -------------------------------------------------------- */
  explicit Image(                      // Initialise from RAW pixel data
    /* -- Parameters ------------------------------------------------------- */
    const string &strName,             // Name of the object
    Memory &&mRval,                    // Source pixel data
    const unsigned int uiWidth,        // Number of pixels in each scanline
    const unsigned int uiHeight,       // Number of scan lines
    const BitDepth bdBits              // Bit depth of the pixel data
    ): /* -- Initialisation of members ------------------------------------- */
    Image{}                            // Default initialisation
    /* -- Initialise raw image --------------------------------------------- */
    { InitRaw(strName, mRval, uiWidth, uiHeight, bdBits); }
  /* -- Constructor -------------------------------------------------------- */
  explicit Image(                      // Initialise from known file formats
    /* -- Parameters ------------------------------------------------------- */
    const string &strName,             // Name of object
    Memory &&mRval,                    // Source memory block to read from
    const ImageFlagsConst &ifFlags     // Loading flags
    ): /* -- Initialisation of members ------------------------------------- */
    Image{}                            // Default initialisation
    /* -- Initialise from array -------------------------------------------- */
    { InitArray(strName, mRval, ifFlags); }
  /* -- Constructor -------------------------------------------------------- */
  explicit Image(                      // Initialise image from file
    /* -- Parameters ------------------------------------------------------- */
    const string &strName,             // Name of image from assets to load
    const ImageFlagsConst &ifFlags     // Loading flags
    ): /* -- Initialisation of members ------------------------------------- */
    Image{}                            // Default initialisation
    /* -- Code ------------------------------------------------------------- */
    { InitFile(strName, ifFlags); }    // Initialisation from file
  /* -- Constructor -------------------------------------------------------- */
  Image(                               // MOVE constructor to SWAP with another
    /* -- Parameters ------------------------------------------------------- */
    Image &&imOtherRval                // Other image to swap with
    ): /* -- Initialisation of members ------------------------------------- */
    Image{}                            // Default initialisation
    /* -- Code ------------------------------------------------------------- */
    { SwapImage(imOtherRval); }        // Swap image over
  /* -- Destructor --------------------------------------------------------- */
  ~Image(void) { AsyncCancel(); }      // Wait for loading thread to cancel
};/* -- End ---------------------------------------------------------------- */
CTOR_END_ASYNC(Images, Image, IMAGE, IMAGE,,,, idFormatModes{{ // Pixel formats
  /* ----------------------------------------------------------------------- */
  IDMAPSTR(TT_NONE),                   IDMAPSTR(TT_BGR),
  IDMAPSTR(TT_BGRA),                   IDMAPSTR(TT_DXT1),
  IDMAPSTR(TT_DXT3),                   IDMAPSTR(TT_DXT5),
  IDMAPSTR(TT_GRAY),                   IDMAPSTR(TT_GRAYALPHA),
  IDMAPSTR(TT_RGB),                    IDMAPSTR(TT_RGBA),
  /* ----------------------------------------------------------------------- */
}, "TT_UNKNOWN"})                      // Unknown pixel format mode
/* ------------------------------------------------------------------------- */
template<typename IntType> // Forcing any type to GLenum
  static const string_view &ImageGetPixelFormat(const IntType itMode)
     { return cImages->idFormatModes.Get(static_cast<TextureType>(itMode)); }
/* ------------------------------------------------------------------------- */
typedef vector<Image> ImageVector;     // Vector of images
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
