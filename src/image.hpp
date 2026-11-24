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
using namespace IGCHelper::P;          using namespace IImageData::P;
using namespace IImageDef::P;          using namespace IImageLib::P;
using namespace IImageSlot::P;         using namespace ILog::P;
using namespace ILuaIdent::P;          using namespace ILuaLib::P;
using namespace IMemory::P;            using namespace IName::P;
using namespace IOgl::P;               using namespace ISerial::P;
using namespace IStd::P;               using namespace IString::P;
using namespace ISysUtil::P;           using namespace ITexDef::P;
using namespace IUtil::P;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* == Image collector and member class ===================================== */
CTOR_BEGIN_ASYNC_DUO(Images, Image, CLHelperUnsafe, ICHelperUnsafe),
  /* -- Base classes ------------------------------------------------------- */
  public AsyncLoaderImage,             // For loading images off main thread
  public ImageData                     // Raw image data
{ /* -- Swap image data  ------------------------------------------- */ public:
  void ImageSwap(Image &imRef)
  { // Swap members with other class
    LockSwap(imRef);                   // Move Lua lock status
    CollectorSwapRegistration(imRef);  // Move collector registration status
    ImageDataSwap(imRef);              // Move image data and flags
  }
  /* -- Copy image data  --------------------------------------------------- */
  void ImageCopy(Image &imRef)
  { // Swap members with other class
    LockCopy(imRef);                   // Copy Lua lock status
    CollectorCopyRegistration(imRef);  // Copy collector registration status
    ImageDataCopy(imRef);              // Copy image data and flags swap
  }
  /* -- Swap image data (so you can use temporary variables) --------------- */
  void ImageSwap(Image &&imRef) { ImageSwap(imRef); }
  /* -- Force the specified colour mode ------------------------------------ */
  bool ImageForcePixelOrder(const TextureType ttNType)
  { // Return failure if parameters are wrong
    if((ttNType == TT_BGR &&
        (ImageDataGetPixelType() != TT_RGBA &&
         ImageDataGetPixelType() != TT_RGB)) ||
        (ttNType == TT_RGB &&
         (ImageDataGetPixelType() != TT_BGRA &&
          ImageDataGetPixelType() != TT_BGR)))
      return false;
    // For each slot
    for(ImageSlot &isRef : ImageDataGetSlots())
    { // Swap pixels
      const int iResult = ImageLibSwapPixels(isRef.MemPtr<char>(),
        isRef.MemSize(), ImageDataGetBytesPerPixel(), 0, 2);
      if(iResult < 0)
        XC("Pixel reorder failed!",
          "Code",          iResult,
          "FromFormat",    ImageLibGetPixelFormat(ImageDataGetPixelType()),
          "ToFormat",      ImageLibGetPixelFormat(ttNType),
          "BytesPerPixel", ImageDataGetBytesPerPixel(),
          "Address",       isRef.MemPtr<void>(),
          "Length",        isRef.MemSize());
    } // Set new pixel type
    switch(ImageDataGetPixelType())
    { case TT_RGBA : ImageDataSetPixelType(TT_BGRA); break;
      case TT_RGB  : ImageDataSetPixelType(TT_BGR);  break;
      case TT_BGRA : ImageDataSetPixelType(TT_RGBA); break;
      case TT_BGR  : ImageDataSetPixelType(TT_RGB);  break;
      default      : break;
    } // Success
    return true;
  }
  /* -- Force binary mode -------------------------------------------------- */
  bool ImageForceBinary()
  { // Ignore if already one bit or there are no slots
    if(ImageDataGetBitsPerPixel() == BD_BINARY ||
       ImageDataIsNoSlots()) return false;
    // Throw error if compressed
    if(ImageDataIsCompressed())
      XC("Cannot binary downsample a compressed image!");
    // Must be 32bpp
    if(ImageDataGetBitsPerPixel() != BD_RGBA)
      XC("Cannot binary downsample a non RGBA encoded image!",
        "BitsPerPixel", ImageDataGetBitsPerPixel());
    // Calculate destination size
    const size_t stDst = ImageDataGetTotalPixels() / CHAR_BIT;
    // Must be divisible by eight
    if(!UtilIsDivisible(static_cast<double>(stDst)))
      XC("Image size not divisible by eight!", "Size", stDst);
    // Bits lookup table
    static const StdArray<const unsigned char, CHAR_BIT>
      ucaBits{0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};
    // For each image slot
    for(ImageSlot &isRef : ImageDataGetSlots())
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
    ImageDataSetBitsPerPixel(BD_BINARY);
    ImageDataSetBytesPerPixel(BY_GRAY);
    ImageDataSetPixelType(TT_GRAY);
    ImageDataSetAlloc(stDst);
    // Success
    return true;
  }
  /* -- Reverse pixels ----------------------------------------------------- */
  bool ImageReversePixels()
  { // Compare bits per pixel
    switch(ImageDataGetBitsPerPixel())
    { // Monochrome image?
      case BD_BINARY:
        // Enumerate through each slot and swap the 4-bits in each 8-bit byte.
        for(ImageSlot &isRef : ImageDataGetSlots()) isRef.MemByteSwap8();
        // Done
        break;
      // 8-bits per pixel or greater?
      case BD_GRAY: case BD_GRAYALPHA: case BD_RGB: case BD_RGBA:
        // Enumerate through each slot
        for(ImageSlot &isRef : ImageDataGetSlots())
        { // Create new mem block to write to
          Memory mbOut{ isRef.MemSize() };
          // Pixel size
          const size_t stStep =
            ImageDataGetBytesPerPixel() * isRef.DimGetWidth();
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
    if(ImageDataIsReversed()) ImageDataClearReversed();
    else ImageDataSetReversed();
    // Success
    return true;
  }
  /* -- Pixel conversion process ------------------------------------------- */
  template<class PixelConversionFunction, size_t stSrcStep, size_t stDstStep,
    BitDepth bdNewBPP, TextureType ttType>
  requires (stSrcStep > 0) && (stSrcStep <= 2) &&
           (stDstStep > 0) && (stDstStep <= 32)
  void ImageConvertPixels(const char*const cpFilter)
  { // Set new bits and bytes
    ImageDataSetBitsAndBytesPerPixel(bdNewBPP);
    ImageDataSetPixelType(ttType);
    // New allocation size
    size_t stNewAlloc = 0;
    // For each slot
    for(ImageSlot &isRef : ImageDataGetSlots())
    { // Calculate total pixels
      const size_t stTotal = isRef.DimGetWidth() * isRef.DimGetHeight();
      // Make a new memblock for the destinaiton pixels
      Memory mDst{ stTotal * ImageDataGetBytesPerPixel() };
      // Quick sanity check to make sure the filters don't read/write OOB
      if(const size_t stUnpadded = isRef.MemSize() % stSrcStep)
        XC("Source image dimensions not acceptable for filter!",
          "File",   NameGet(),                   "Width",  isRef.DimGetWidth(),
          "Height", isRef.DimGetHeight(),        "Total",    stTotal,
          "Depth",  ImageDataGetBytesPerPixel(), "Filter",   cpFilter,
          "Step",   stSrcStep,                   "Unpadded", stUnpadded);
      if(const size_t stUnpadded = mDst.MemSize() % stDstStep)
        XC("Destination image dimensions not acceptable for filter!",
          "File",   NameGet(),                   "Width",  isRef.DimGetWidth(),
          "Height", isRef.DimGetHeight(),        "Total",    stTotal,
          "Depth",  ImageDataGetBytesPerPixel(), "Filter",   cpFilter,
          "Step",   stDstStep,                   "Unpadded", stUnpadded);
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
    ImageDataSetAlloc(stNewAlloc);
  }
  /* -- Force luminance alpha pixel to RGB pixel type ---------------------- */
  void ImageConvertLuminanceAlphaToRGB()
  { // Class to convert a luminance alpha pixel to a RGB pixel
    struct Filter{
      Filter(const uint8_t*const ubpSrc, uint8_t*const ubpDst)
      { // Unpack one luminance alpha pixel into one RGB pixel
        *reinterpret_cast<uint16_t*>(ubpDst) =
          (static_cast<uint16_t>(*ubpSrc) * 0x0101);
        *reinterpret_cast<uint8_t*>(ubpDst+1) = 0xFF;
      }
    }; // Do the conversion of luminance alpha to RGB
    ImageConvertPixels<Filter, 2, 3, BD_RGB, TT_RGB>("LUMA>RGB");
  }
  /* -- Force luminance pixel to RGB pixel type ---------------------------- */
  void ImageConvertLuminanceToRGB()
  { // Class to convert a luminance pixel to a RGB pixel
    struct Filter{
      Filter(const uint8_t*const ubpSrc, uint8_t*const ubpDst)
      { // Unpack one luminance pixel into one RGB pixel
        *reinterpret_cast<uint16_t*>(ubpDst) = *ubpSrc * 0x101;
        *reinterpret_cast<uint8_t*>(ubpDst+2) = *ubpSrc;
      }
    }; // Do the conversion of luminance to RGB
    ImageConvertPixels<Filter, 1, 3, BD_RGB, TT_RGB>("LUM>RGB");
  }
  /* -- Force luminance alpha pixel to RGBA pixel type --------------------- */
  void ImageConvertLuminanceAlphaToRGBA()
  { // Class to convert a luminance alpha pixel to a RGBA pixel
    struct Filter{
      Filter(const uint8_t*const ubpSrc, uint8_t*const ubpDst)
      { // Unpack one luminance alpha pixel into one RGBA pixel
        *reinterpret_cast<uint32_t*>(ubpDst) =
          (static_cast<uint32_t>(*ubpSrc) * 0x00010101) |
          (static_cast<uint32_t>(*(ubpSrc+1)) * 0x01000000);
      }
    }; // Do the conversion of luminance alpha to RGBA
    ImageConvertPixels<Filter, 2, 4, BD_RGBA, TT_RGBA>("LUMA>RGBA");
  }
  /* -- Force luminance pixel to RGBA pixel type --------------------------- */
  void ImageConvertLuminanceToRGBA()
  { // Class to convert a luminance pixel to a RGBA pixel
    struct Filter{
      Filter(const uint8_t*const ubpSrc, uint8_t*const ubpDst)
      { // Unpack one luminance pixel into one RGBA pixel ignoring alpha
        *reinterpret_cast<uint32_t*>(ubpDst) = (*ubpSrc * 0x01010100) | 0xFF;
      }
    }; // Do the conversion of luminance alpha to RGBA
    ImageConvertPixels<Filter, 1, 4, BD_RGBA, TT_RGBA>("LUM>RGBA");
  }
  /* -- Force binary pixel to luminance pixel type ------------------------- */
  void ImageConvertBinaryToLuminance()
  { // Class to convert a BINARY pixel to a LUMINANCE pixel
    struct Filter{
      Filter(const uint8_t*const ubpSrc, uint8_t*const ubpDst)
      { // Get the packed eight pixels
        const unsigned uPixels = *ubpSrc;
        // Unpack eight binary pixels into eight luminance (white) pixels
        *reinterpret_cast<uint64_t*>(ubpDst) =
          (uPixels & 0x01 ? 0xff00000000000000 : 0) | // Bit 1/8 -> Byte 1
          (uPixels & 0x02 ? 0x00ff000000000000 : 0) | // Bit 2/8 -> Byte 2
          (uPixels & 0x04 ? 0x0000ff0000000000 : 0) | // Bit 3/8 -> Byte 3
          (uPixels & 0x08 ? 0x000000ff00000000 : 0) | // Bit 4/8 -> Byte 4
          (uPixels & 0x10 ? 0x00000000ff000000 : 0) | // Bit 5/8 -> Byte 5
          (uPixels & 0x20 ? 0x0000000000ff0000 : 0) | // Bit 6/8 -> Byte 6
          (uPixels & 0x40 ? 0x000000000000ff00 : 0) | // Bit 7/8 -> Byte 7
          (uPixels & 0x80 ? 0x00000000000000ff : 0);  // Bit 8/8 -> Byte 8
      }
    }; // Do the conversion of binary to luminance
    ImageConvertPixels<Filter, 1, 8, BD_GRAY, TT_GRAY>("BIN>LUM");
  }
  /* -- Force binary pixel to RGB pixel type ------------------------------- */
  void ImageConvertBinaryToRGB()
  { // Class to convert a BINARY pixel to a RGB pixel
    struct Filter{
      Filter(const uint8_t*const ubpSrc, uint8_t*const ubpDst)
      { // Get the packed eight pixels
        const unsigned uPixels = *ubpSrc;
        // Unpack eight BINARY (1-bit) pixels into eight RGB (24-bit) pixels.
        *reinterpret_cast<uint64_t*>(ubpDst) =         // Pixels 1 to 3A
          // Pixel order       RRGGBBRRGGBBRRGG (R=RED, G=GREEN, B=BLUE)
          (uPixels & 0x80 ? 0xffffff0000000000 : 0) | // * Bit 8/8 -> Byte 1-3
          (uPixels & 0x40 ? 0x000000ffffff0000 : 0) | // * Bit 7/8 -> Byte 4-6
          (uPixels & 0x20 ? 0x000000000000ffff : 0);  // * Bit 6/8 -> Byte 7-8
        *(reinterpret_cast<uint64_t*>(ubpDst) + 1) =  // Pixels 3B to 6A
          // Pixel order       BBRRGGBBRRGGBBRR (R=RED, G=GREEN, B=BLUE)
          (uPixels & 0x20 ? 0xff00000000000000 : 0) | // * Bit 6/8 -> Byte 9
          (uPixels & 0x10 ? 0x00ffffff00000000 : 0) | // * Bit 5/8 -> By 10-12
          (uPixels & 0x08 ? 0x00000000ffffff00 : 0) | // * Bit 4/8 -> By 13-15
          (uPixels & 0x04 ? 0x00000000000000ff : 0);  // * Bit 3/8 -> Byte 16
        *(reinterpret_cast<uint64_t*>(ubpDst) + 2) =  // Pixels 6B to 8
          // Pixel order       GGBBRRGGBBRRGGBB (R=RED, G=GREEN, B=BLUE)
          (uPixels & 0x04 ? 0xffff000000000000 : 0) | // * Bit 3/8 -> By 17-18
          (uPixels & 0x02 ? 0x0000ffffff000000 : 0) | // * Bit 2/8 -> By 19-21
          (uPixels & 0x01 ? 0x0000000000ffffff : 0);  // * Bit 1/8 -> By 22-24
      }
    }; // Do the conversion of binary to RGBA
    ImageConvertPixels<Filter, 1, 24, BD_RGB, TT_RGB>("BIN>RGB");
  }
  /* -- Force binary pixel to RGBA pixel type ------------------------------ */
  void ImageConvertBinaryToRGBA()
  { // Class to convert a BINARY pixel to a RGBA pixel
    struct Filter{
      Filter(const uint8_t*const ubpSrc, uint8_t*const ubpDst)
      { // Get the packed eight pixels
        const unsigned uPixels = *ubpSrc;
        // Unpack eight BINARY pixels into eight RGBA (32-bit) pixels
        *reinterpret_cast<uint64_t*>(ubpDst) =        // Pixel 1-2
          // Pixel order       RRGGBBAARRGGBBAA (R=RED,G=GREEN,B=BLUE,A=ALPHA)
          (uPixels & 0x80 ? 0xffffffff00000000 : 0) | // * Bit 8/8 -> DWord 1
          (uPixels & 0x40 ? 0x00000000ffffffff : 0);  // * Bit 7/8 -> DWord 2
        *(reinterpret_cast<uint64_t*>(ubpDst) + 1) =  // Pixel 3-4
          // Pixel order       RRGGBBAARRGGBBAA (R=RED,G=GREEN,B=BLUE,A=ALPHA)
          (uPixels & 0x20 ? 0xffffffff00000000 : 0) | // * Bit 6/8 -> DWord 3
          (uPixels & 0x10 ? 0x00000000ffffffff : 0);  // * Bit 5/8 -> DWord 4
        *(reinterpret_cast<uint64_t*>(ubpDst) + 2) =  // Pixel 5-6
          // Pixel order       RRGGBBAARRGGBBAA (R=RED,G=GREEN,B=BLUE,A=ALPHA)
          (uPixels & 0x08 ? 0xffffffff00000000 : 0) | // * Bit 4/8 -> DWord 5
          (uPixels & 0x04 ? 0x00000000ffffffff : 0);  // * Bit 3/8 -> DWord 6
        *(reinterpret_cast<uint64_t*>(ubpDst) + 3) =  // Pixel 7-8
          // Pixel order       RRGGBBAARRGGBBAA (R=RED,G=GREEN,B=BLUE,A=ALPHA)
          (uPixels & 0x02 ? 0xffffffff00000000 : 0) | // * Bit 2/8 -> DWord 7
          (uPixels & 0x01 ? 0x00000000ffffffff : 0);  // * Bit 1/8 -> DWord 8
      }
    }; // Do the conversion of binary to RGBA
    ImageConvertPixels<Filter, 1, 32, BD_RGBA, TT_RGBA>("BIN>RGBA");
  }
  /* -- Concatenate tiles into a single texture ---------------------------- */
  bool ImageMakeAtlas()
  { // Save number of images compacted
    stTiles = ImageDataGetSlotCount();
    // Return if 1 or less slides or 1 or less bit depth or has a palette
    if(stTiles <= 1 || ImageDataGetBitsPerPixel() <= BD_BINARY ||
       ImageDataIsPalette())
      return false;
    // Remaining tiles
    size_t stRemain = stTiles;
    // Take ownership current slots list and make a blank new one.
    SlotList slSrc{ StdMove(slSlots) };
    // Reset allocation
    ImageDataSetAlloc(0);
    // Get first image slot
    ImageSlot &isFirst = slSrc.front();
    // Set override tile size and count
    duTileOR.DimSet(isFirst);
    // Get maximum texture size allowed
    const size_t stMaxSize = cOgl->OglMaxTexSize();
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
      &stRows, &stNCols, &stNRows, &mTexture]()
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
      mTexture.MemInitBlank(stTexWidth *
        stTexHeight * ImageDataGetBytesPerPixel());
    } };
    // Calculate the texture size
    SetupCanvas();
    // Texture position and tiles remaining
    size_t stTX = 0, stTY = 0;
    // Scanline size
    const size_t stScanLine = stWidth * ImageDataGetBytesPerPixel();
    // Until we have no more slots. We'll keep deleting them as we process
    // them to keep the memory usage down
    for(const ImageSlot &isRef : slSrc)
    { // Now we copy this slide into the texuree
      for(size_t stY = 0; stY < stHeight; ++stY)
        mTexture.MemWrite((((stTY + stY) * stTexWidth) + stTX) *
          ImageDataGetBytesPerPixel(),
          isRef.MemRead(stY * stScanLine, stScanLine), stScanLine);
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
      ImageDataAddSlot(mTexture, static_cast<unsigned>(stTexWidth),
                                 static_cast<unsigned>(stTexHeight));
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
        mTexture.MemResize(stTexWidth *
          stTexHeight * ImageDataGetBytesPerPixel());
      } // Add the slot
      ImageDataAddSlot(mTexture, static_cast<unsigned>(stTexWidth),
                                 static_cast<unsigned>(stTexHeight));
    } // Set size of first texture
    DimSet(ImageDataGetSlots().front());
    // Success
    return true;
  }
  /* -- Convert palette to RGB(A) ------------------------------------------ */
  template<ByteDepth byDepth, TextureType ttType>bool ImageExpandPalette()
  { // Ignore if not paletted
    if(ImageDataIsNotPalette()) return false;
    // Must only have two slots
    if(ImageDataGetSlotCount() != 2)
      XC("Cannot convert palette to RGB because we need two slots!",
        "Slots", ImageDataGetSlotCount());
    // Take ownership current slots list and make a blank new one.
    SlotList slSrc{ StdMove(slSlots) };
    // Reset allocation
    ImageDataSetAlloc(0);
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
    ImageDataSetBytesAndBitsPerPixel(byDepth);
    ImageDataSetPixelType(ttType);
    // Add expanded image to list
    ImageDataAddSlot(mOut);
    // Success
    return true;
  }
  /* -- Convert data to GPU compatible ------------------------------------- */
  bool ImageConvertGPUCompatible()
  { // We don't have functionality to load <8bpp images in GPU yet so if this
    // flag is specified, we will make sure <8bpp textures get converted
    // properly so they can be loaded by OpenGL. This is useful if the
    // bit-depth of the bitmap isn't known and the caller wants to make sure
    // it guarantees to be loaded into OpenGL.
    switch(ImageDataGetBitsPerPixel())
    { // 1bpp (BINARY)
      case BD_BINARY: ImageConvertBinaryToLuminance(); break;
      // Ignore anything else
      default: return false;
    } // Success
    return true;
  }
  /* -- Convert image bit-depth to to 24-bits per pixel RGB format --------- */
  bool ImageConvertRGB()
  { // Compare current bit-depth
    switch(ImageDataGetBitsPerPixel())
    { // 1bpp (BINARY)
      case BD_BINARY: ImageConvertBinaryToRGB(); break;
      // 8bpp (LUMINANCE)
      case BD_GRAY:
        if(ImageDataIsPalette()) return ImageExpandPalette<BY_RGB,TT_RGB>();
        ImageConvertLuminanceToRGB();
        break;
      // 16bpp (LUMINANCE+ALPHA)
      case BD_GRAYALPHA: ImageConvertLuminanceAlphaToRGB(); break;
      // Ignore anything else
      default: return false;
    } // Success
    return true;
  }
  /* -- Convert image bit-depth to to 32-bits per pixel RGBA format -------- */
  bool ImageConvertRGBA()
  { // Compare current bit-depth
    switch(ImageDataGetBitsPerPixel())
    { // 1bpp (BINARY)
      case BD_BINARY: ImageConvertBinaryToRGBA(); break;
      // 8bpp (LUMINANCE)
      case BD_GRAY:
        if(ImageDataIsPalette()) return ImageExpandPalette<BY_RGBA,TT_RGBA>();
        ImageConvertLuminanceToRGBA();
        break;
      // 16bpp (LUMINANCE+ALPHA)
      case BD_GRAYALPHA: ImageConvertLuminanceAlphaToRGBA(); break;
      // Ignore anything else
      default: return false;
    } // Success
    return true;
  }
  /* -- Apply filters ------------------------------------------------------ */
  void ImageApplyFilters()
  { // Record current parameters
    const DimUInt duOld{ *this };
    const size_t stSlots = ImageDataGetSlotCount();
    const BitDepth bdOld = ImageDataGetBitsPerPixel();
    const ByteDepth byOld = ImageDataGetBytesPerPixel();
    const TextureType ttOld = ImageDataGetPixelType();
    const size_t stOld = GetAlloc();
    // Convert to GPU copmatible texture?
    if(ImageDataIsConvertGPUCompat())
    { // If it is not already converted by the file filter?
      if(ImageDataIsNotActiveGPUCompat())
      { // Log that we're running this function
        cLog->LogDebugExSafe("Image '$' safe pixel depth request...",
          NameGet());
        // Run the function and log success if succeeded
        if(ImageConvertGPUCompatible())
        { // Log the successful result
          cLog->LogInfoExSafe("Image '$' pixel-depth now safe.", NameGet());
          // Set activated flag
          ImageDataSetActiveGPUCompat();
        } // Conversion did not happen so log that too
        else cLog->LogDebugExSafe("Image '$' skipped safe pixel depth!",
          NameGet());
      } // Conversion handled by codec
      else cLog->LogDebugExSafe("Image '$' safe pixel depth by codec!",
        NameGet());
    } // If a request to convert to 24-bits per pixel?
    else if(ImageDataIsConvertRGB())
    { // If it is not already converted by the file filter?
      if(ImageDataIsNotActiveRGB())
      { // Log that we're running this function
        cLog->LogDebugExSafe("Image '$' force 24-bit request...", NameGet());
        // Run the function and log success if succeeded
        if(ImageConvertRGB())
        { // Log the successful result
          cLog->LogInfoExSafe("Image '$' pixel depth now 24-bit.", NameGet());
          // Set activated flag
          ImageDataSetActiveRGB();
        } // Conversion did not happen so log that too
        else cLog->LogDebugExSafe("Image '$' skipped converting to 24-bit!",
          NameGet());
      } // Conversion handled by codec
      else cLog->LogDebugExSafe("Image '$' converted to 24-bit by codec!",
        NameGet());
    } // If a request to convert to RGBA?
    else if(ImageDataIsConvertRGBA())
    { // If it is not already converted by the file filter?
      if(ImageDataIsNotActiveRGBA())
      { // Log that we're running this function
        cLog->LogDebugExSafe("Image '$' force 32-bit request...", NameGet());
        // Run the function and log success if succeeded
        if(ImageConvertRGBA())
        { // Log the successful result
          cLog->LogInfoExSafe("Image '$' pixel depth now 32-bit.", NameGet());
          // Set activated flag
          ImageDataSetActiveRGBA();
        } // Conversion did not happen so log that too
        else cLog->LogDebugExSafe("Image '$' skipped converting to 32-bit!",
          NameGet());
      } // Conversion handled by codec
      else cLog->LogDebugExSafe("Image '$' converted to 32-bit by codec!",
        NameGet());
    } // To BGR colour mode?
    if(ImageDataIsConvertBGROrder())
    { // If it is not already converted by the file filter?
      if(ImageDataIsNotActiveBGROrder())
      { // Log that we're running this function
        cLog->LogDebugExSafe("Image '$' re-order to BGR request...",
          NameGet());
        // Run the function and log success if succeeded
        if(ImageForcePixelOrder(TT_BGR))
        { // Log the successful result
          cLog->LogInfoExSafe("Image '$' re-ordered to BGR.", NameGet());
          // Set activated flag
          ImageDataSetActiveBGROrder();
        } // Conversion did not happen so log that too
        else cLog->LogDebugExSafe("Image '$' re-order to BGR skipped!",
          NameGet());
      } // Conversion handled by codec
      else cLog->LogDebugExSafe("Image '$' re-ordered to BGR by codec!",
        NameGet());
    } // To RGB colour mode
    else if(ImageDataIsConvertRGBOrder())
    { // If it is not already converted by the file filter?
      if(ImageDataIsNotActiveRGBOrder())
      { // Log that we're running this function
        cLog->LogDebugExSafe("Image '$' re-order to RGB request...",
          NameGet());
        // Run the function and log success if succeeded
        if(ImageForcePixelOrder(TT_RGB))
        { // Log the successful result
          cLog->LogInfoExSafe("Image '$' re-ordered now RGB.", NameGet());
          // Set activated flag
          ImageDataSetActiveRGBOrder();
        } // Conversion did not happen so log that too
        else cLog->LogDebugExSafe("Image '$' re-order to RGB skipped!",
          NameGet());
      } // Conversion handled by codec
      else cLog->LogDebugExSafe("Image '$' re-ordered to RGB by codec!",
        NameGet());
    } // Reverse the image pixels
    if(ImageDataIsConvertReverse())
    { // If it is not already converted by the file filter?
      if(ImageDataIsNotActiveReverse())
      { // Log that we're running this function
        cLog->LogDebugExSafe("Image '$' pixel reversal request...",
          NameGet());
        // Run the function and log success if succeeded
        if(ImageReversePixels())
        { // Log the successful result
          cLog->LogInfoExSafe("Image '$' pixels reversed!", NameGet());
          // Set activated flag
          ImageDataSetActiveReverse();
        } // Conversion did not happen so log that too
        else cLog->LogDebugExSafe("Image '$' skipped pixel reversal!",
          NameGet());
      } // Conversion handled by codec
      else cLog->LogDebugExSafe("Image '$' reversal by codec!",
        NameGet());
    } // To binary colour mode
    if(ImageDataIsConvertBinary())
    { // If it is not already converted by the file filter?
      if(ImageDataIsNotActiveBinary())
      { // Log that we're running this function
        cLog->LogDebugExSafe("Image '$' downsample to binary request...",
          NameGet());
        // Run the function and log success if succeeded
        if(ImageForceBinary())
        { // Log the successful result
          cLog->LogInfoExSafe("Image '$' downsample to binary completed.",
            NameGet());
          // Set activated flag
          ImageDataSetActiveBinary();
        } // Conversion did not happen so log that too
        else cLog->LogDebugExSafe("Image '$' downsample to binary skipped!",
          NameGet());
      } // Conversion handled by codec
      else cLog->LogDebugExSafe("Image '$' downsampled to binary by codec!",
        NameGet());
    } // Compact all slides into a single texture if possible
    if(ImageDataIsConvertAtlas())
    { // If it is not already converted by the file filter?
      if(ImageDataIsNotActiveAtlas())
      { // Log that we're running this function
        cLog->LogDebugExSafe("Image '$' compact request...", NameGet());
        // Run the function and log success if succeeded
        if(ImageMakeAtlas())
        { // Log the successful result
          cLog->LogInfoExSafe("Image '$' compacted.", NameGet());
          // Set activated flag
          ImageDataSetActiveAtlas();
        } // Conversion did not happen so log that too
        else cLog->LogDebugExSafe("Image '$' compact skipped!", NameGet());
      } // Conversion handled by codec
      else cLog->LogDebugExSafe("Image '$' compact handled by codec!",
        NameGet());
    } // Report status if we acticated anything
    if(ImageDataIsActiveAtlas() || ImageDataIsActiveReverse() ||
       ImageDataIsActiveRGB() || ImageDataIsActiveRGBA() ||
       ImageDataIsActiveBGROrder() || ImageDataIsActiveBinary() ||
       ImageDataIsActiveGPUCompat() || ImageDataIsActiveRGBOrder())
      cLog->LogDebugExSafe("Image '$' filtering completed...$$$$$$$$$$$$$$$",
        NameGet(),
        duTileOR.DimGetWidth() && duTileOR.DimGetHeight() ?
          StrFormat("\n- Tile size override: $x$.",
            duTileOR.DimGetWidth(), duTileOR.DimGetHeight()) :
            cCommon->CommonBlankStr(),
        stTiles ? StrFormat("\n- Tile count override: $.", stTiles) :
          cCommon->CommonBlankStr(),
        duOld.DimGetWidth() != DimGetWidth() ||
        duOld.DimGetHeight() != DimGetHeight() ?
          StrFormat("\n- Bitmap dimensions: $x$ -> $x$.",
            duOld.DimGetWidth(), duOld.DimGetHeight(),
            DimGetWidth(), DimGetHeight()) : cCommon->CommonBlankStr(),
        stSlots != ImageDataGetSlotCount() ?
          StrFormat("\n- Bitmap slots: $ -> $.",
            stSlots, ImageDataGetSlotCount()) : cCommon->CommonBlankStr(),
        bdOld != ImageDataGetBitsPerPixel() ?
          StrFormat("\n- Pixel depth: $<$> -> $<$>.", bdOld, byOld,
            ImageDataGetBitsPerPixel(), ImageDataGetBytesPerPixel()) :
          cCommon->CommonBlankStr(),
        ttOld != ImageDataGetPixelType() ?
          StrFormat("\n- Pixel type: $<$$> -> $<$$>.",
            ImageLibGetPixelFormat(ttOld), StdIOSHex, ttOld,
            ImageLibGetPixelFormat(ImageDataGetPixelType()),
              ImageDataGetPixelType(), StdIOSDec) : cCommon->CommonBlankStr(),
        stOld != GetAlloc() ?
          StrFormat("\n- Memory usage: $ -> $ bytes.",
            stOld, GetAlloc()) : cCommon->CommonBlankStr(),
        ImageDataIsActiveAtlas() ? "\n- Slots compacted to atlas." :
          cCommon->CommonBlankStr(),
        ImageDataIsActiveReverse() ? "\n- Pixels reversed." :
          cCommon->CommonBlankStr(),
        ImageDataIsActiveRGB() ? "\n- Pixels converted to 24-bit." :
          cCommon->CommonBlankStr(),
        ImageDataIsActiveRGBA() ? "\n- Pixels converted to 32-bit." :
          cCommon->CommonBlankStr(),
        ImageDataIsActiveBGROrder() ? "\n- Pixels converted to BGR order." :
          cCommon->CommonBlankStr(),
        ImageDataIsActiveRGBOrder() ? "\n- Pixels converted to RGB order." :
          cCommon->CommonBlankStr(),
        ImageDataIsActiveBinary() ? "\n- Pixels converted to monochrome." :
          cCommon->CommonBlankStr(),
        ImageDataIsActiveGPUCompat() ? "\n- Pixels made OpenGL compatible." :
          cCommon->CommonBlankStr());
  }
  /* -- Load specified image ----------------------------------------------- */
  bool AsyncReady(FileMap &fmData)
  { // Force load a certain type of image (for speed?) but in Async mode,
    // force detection doesn't really matter as much, but overall, still
    // needed if speed is absolutely neccesary.
    if(ImageDataIsLoadAsPNG()) ImageLibLoad(IFMT_PNG, fmData, *this);
    else if(ImageDataIsLoadAsJPEG()) ImageLibLoad(IFMT_JPEG, fmData, *this);
    else if(ImageDataIsLoadAsGIF()) ImageLibLoad(IFMT_GIF, fmData, *this);
    else if(ImageDataIsLoadAsDDS()) ImageLibLoad(IFMT_DDS, fmData, *this);
    else if(ImageDataIsLoadAsWEBP()) ImageLibLoad(IFMT_WEBP, fmData, *this);
    // Auto detection of image
    else ImageLibLoad(fmData, *this);
    // Apply filters if image has no special circumstances
    if(ImageDataIsNotCompressed() && ImageDataIsNotMipmaps())
      ImageApplyFilters();
    // Recover slots memory if they were modified
    ImageDataCompactSlots();
    // Free any loaded memory
    return true;
  }
  /* -- Reload specified image --------------------------------------------- */
  void ImageReloadData()
  { // Load the file from disk or archive
    FileMap fmData{ AssetExtract(NameGet()) };
    // Reset memory usage to zero
    ImageDataSetAlloc(0);
    // Run codecs and filters on it
    AsyncReady(fmData);
  }
  /* -- Save image using a type id ----------------------------------------- */
  void ImageSaveFile(const StdStringView &ssvFile, const size_t stSId,
    const ImageFormat ifPId) const
  { ImageLibSave(ifPId, ssvFile, *this, ImageDataGetSlots()[stSId]); }
  /* -- Load image from memory asynchronously ------------------------------ */
  template<class StrType>
    requires StdIsStrOrCStr<StrType>
  void ImageInitAsyncArray(lua_State*const lS, StrType &&strIdent,
    Asset &aData, const ImageFlagsConst ifcFlags)
  { // Set user flags
    FlagSet(ifcFlags);
    // The decoded image will be kept in memory
    ImageDataSetDynamic();
    // Load image from memory asynchronously
    AsyncInitArray(lS, StdForward<StrType>(strIdent), "imagearray", aData);
  }
  /* -- Load image from file asynchronously -------------------------------- */
  template<class StrType>
    requires StdIsStrOrCStr<StrType>
  void ImageInitAsyncFile(lua_State*const lS, StrType &&strFile,
    const ImageFlagsConst ifcFlags)
  { // Set user flags
    FlagSet(ifcFlags);
    // Load image from file asynchronously
    AsyncInitFile(lS, StdForward<StrType>(strFile), "imagefile");
  }
  /* -- Create a blank image for working on -------------------------------- */
  void ImageInitBlank(const unsigned uBWidth, const unsigned uBHeight,
    const bool bAlpha, const bool bClear)
  { // Lookup table for alpha setting
    using BitDepthTexTypePair = StdPair<const BitDepth, const TextureType>;
    using BitDepthTexTypePairArray = StdArray<const BitDepthTexTypePair, 2>;
    static const BitDepthTexTypePairArray
      bdttpLookup{ { { BD_RGB, TT_RGB }, { BD_RGBA, TT_RGBA } } };
    const BitDepthTexTypePair &bdttpLookupRef =
      bdttpLookup[static_cast<size_t>(bAlpha)];
    // Set appropriate parameters
    ImageDataSetBitsAndBytesPerPixel(bdttpLookupRef.first);
    ImageDataSetPixelType(bdttpLookupRef.second);
    // Set other members
    ImageDataSetDynamic();
    DimSet(uBWidth, uBHeight);
    // Add the raw data into a slot
    ImageDataAddSlot({ ImageDataGetTotalPixels() *
      ImageDataGetBytesPerPixel(), bClear });
    // Load succeeded so register the block
    CollectorRegister();
  }
  /* -- Create a blank image for working on -------------------------------- */
  template<class StrType>
    requires StdIsStrOrCStr<StrType>
  void ImageInitBlank(StrType &&strIdent, const unsigned uBWidth,
    const unsigned uBHeight, const bool bAlpha, const bool bClear)
  { NameSet(StdForward<StrType>(strIdent));
    ImageInitBlank(uBWidth, uBHeight, bAlpha, bClear); }
  /* -- Replace image from a raw image data -------------------------------- */
  void ImageInitReplace(Memory &mSrc)
  { // Take ownership current slots list and make a blank new one.
    if(slSlots.empty()) XC("No slots in image!", "Name", NameGet());
    // Get first image slot
    ImageSlot &isFirst = slSlots.front();
    // Simple check to make sure the sizes are same
    if(isFirst.MemSize() != mSrc.MemSize())
      XC("Pixel data buffer size mismatch!",
        "Name", NameGet(),
        "Expected",   isFirst.MemSize(),
        "Actual",     mSrc.MemSize());
    // Now just move it over
    isFirst.MemSwap(mSrc);
  }
  /* -- Load image from a raw image data ----------------------------------- */
  void ImageInitRaw(Memory &mSrc, const unsigned uBWidth,
    const unsigned uBHeight, const BitDepth bdBitsPP)
  { // Check that the range is valid
    const unsigned uMSize = 0xFFFF;
    if(!uBWidth || !uBHeight || uBWidth > uMSize || uBHeight > uMSize)
      XC("Image dimensions are not acceptable!",
        "File",   NameGet(), "Width",   uBWidth,
        "Height", uBHeight,  "Maximum", uMSize);
    // Set bits per pixel
    ImageDataSetBitsAndBytesPerPixel(bdBitsPP);
    // Set the dimensions
    DimSet(uBWidth, uBHeight);
    // Expected image size
    size_t stExpect;
    // Set the pixel type and if if the type cannot be handled by the GPU?
    ImageDataSetPixelType(ImageBYtoTexType(ImageDataGetBytesPerPixel()));
    if(ImageDataGetPixelType() == TT_NONE)
    { // Only fail if not binary
      if(ImageDataGetBitsPerPixel() != BD_BINARY)
        XC("Image bits per pixel not valid!",
          "File", NameGet(), "Depth", ImageDataGetBitsPerPixel());
      // Total pixels must be divisible by 8
      if(const size_t stRemainder = ImageDataGetTotalPixels() % CHAR_BIT)
        XC("Binary image pixel count must be divisible by eight!",
          "File",      NameGet(), "Pixels", ImageDataGetTotalPixels(),
          "Remainder", stRemainder);
      // Set expected number of bits for binary image
      stExpect = ImageDataGetTotalPixels() / CHAR_BIT;
    } // Compressed textures not supported yet
    else if(ImageDataGetPixelType() >= TT_DXT1 &&
            ImageDataGetPixelType() <= TT_DXT3)
      XC("Compressed images not supported yet!",
        "File", NameGet(),
        "Type", ImageLibGetPixelFormat(ImageDataGetPixelType()));
    // Set expected number of bytes
    else stExpect = ImageDataGetTotalPixels() * ImageDataGetBytesPerPixel();
    // Check that the size matches
    if(stExpect != mSrc.MemSize())
      XC("Arguments are not valid for specified image data!",
        "File", NameGet(), "Expect", stExpect, "Actual", mSrc.MemSize());
    // Everything looks OK, set rest of the members
    ImageDataSetDynamic();
    // Add the raw data into a slot
    ImageDataAddSlot(mSrc);
    // Load succeeded so register the block
    CollectorRegister();
  }
  /* -- Load image from a raw image data ----------------------------------- */
  template<class StrType>
    requires StdIsStrOrCStr<StrType>
  void ImageInitRaw(StrType &&strName, Memory &mSrc, const unsigned uBWidth,
    const unsigned uBHeight, const BitDepth bdBitsPP)
  { NameSet(StdForward<StrType>(strName));
    ImageInitRaw(mSrc, uBWidth, uBHeight, bdBitsPP); }
  /* -- Load image from a single colour ------------------------------------ */
  void ImageInitColour(const uint32_t ulColour)
  { // Set members
    NameSetA("solid/0x", StdIOSHex, ulColour);
    ImageDataSetBitsAndBytesPerPixel(BD_RGBA);
    ImageDataSetPixelType(TT_RGBA);
    ImageDataSetDynamic();
    DimSet(1);
    // Make sure the specified colour is in the correct order that the GPU can
    // read. It will be interpreted as TT_RGBA. The guest will specify the
    // input as 0xAARRGGBB like FboItem::SetRGBAInt().
    const StdArray<const uint8_t,4>ucaColour{ // Source ----- Bit - Dst Pixels
      static_cast<uint8_t>(ulColour >> 16),   // 0x00[RR]0000 16-24 [0] [R]gba
      static_cast<uint8_t>(ulColour >>  8),   // 0x0000[GG]00 08-16 [1] r[G]ba
      static_cast<uint8_t>(ulColour      ),   // 0x000000[BB] 00-08 [2] rg[B]a
      static_cast<uint8_t>(ulColour >> 24)    // 0x[AA]000000 24-32 [3] rgb[A]
    };                                        // ------------ ----- --- ------
    // Add the image
    ImageDataAddSlot({ ImageDataGetBytesPerPixel(), ucaColour.data() });
    // Load succeeded so register the block
    CollectorRegister();
  }
  /* -- Init from file ----------------------------------------------------- */
  template<class StrType>
    requires StdIsStrOrCStr<StrType>
  void ImageInitFile(StrType &&strFile, const ImageFlagsConst &ifcFlags)
  { // Set the loading flags
    FlagSet(ifcFlags);
    // Load the file normally
    SyncInitFileSafe(StdForward<StrType>(strFile), "imagefile");
  }
  /* -- Init from array ---------------------------------------------------- */
  template<class StrType>
    requires StdIsStrOrCStr<StrType>
  void ImageInitArray(StrType &&strIdent, Memory &mRval,
    const ImageFlagsConst &ifcFlags)
  { // Is dynamic because it was not loaded from disk
    ImageDataSetDynamic();
    // Set the loading flags
    FlagSet(ifcFlags);
    // Load the array normally
    SyncInitArray(StdForward<StrType>(strIdent), "imagearray", mRval);
  }
  /* -- Default constructor ------------------------------------------------ */
  Image() :
    /* -- Initialisers ----------------------------------------------------- */
    ICHelperImage{ cImages },          // Initialise collector helper
    SerialSlave{ ctParent.Serial() },  // Initialise identification number
    AsyncLoaderImage{ this,            // Initialise async loader
      EMC_MP_IMAGE }                   // Initialise async event id
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- Constructor with purpose ------------------------------------------- */
  explicit Image(
    /* -- Parameters ------------------------------------------------------- */
    const ImageFlagsConst ifcPurpose) :
    /* -- Initialisers ----------------------------------------------------- */
    ICHelperImage{ cImages },          // Initialise collector helper
    SerialSlave{ ctParent.Serial() },  // Initialise identification number
    AsyncLoaderImage{ this,            // Initialise async loader
      EMC_MP_IMAGE },                  // Initialise async event id
    ImageData{ ifcPurpose }            // Initialise purpose of image class
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- Constructor to initialise a 1x1 pixel texture ---------------------- */
  explicit Image(
    /* -- Parameters ------------------------------------------------------- */
    const uint32_t ulColour            // 32-bit RGBA colour pixel value
    ): /* -- Initialisers -------------------------------------------------- */
    ICHelperImage{ cImages },          // Initialise collector helper
    SerialSlave{ ctParent.Serial() },  // Initialise identification number
    AsyncLoaderImage{ this,            // Initialise async loader
      EMC_MP_IMAGE }                   // Initialise async event id
    /* -- Init 1x1 tex with specified colour ------------------------------- */
    { ImageInitColour(ulColour); }
  /* -- Constructor to Initialise from RAW pixel data ---------------------- */
  Image(
    /* -- Parameters ------------------------------------------------------- */
    const StdStringView &ssvName,      // Name of the object
    Memory &&mRval,                    // Source pixel data
    const unsigned uWidth,             // Number of pixels in each scanline
    const unsigned uHeight,            // Number of scan lines
    const BitDepth bdBits              // Bit depth of the pixel data
    ): /* -- Initialisation of members ------------------------------------- */
    Name{ ssvName },                   // Initialise identifier
    ICHelperImage{ cImages },          // Initialise collector helper
    SerialSlave{ ctParent.Serial() } , // Initialise identification number
    AsyncLoaderImage{ this,            // Initialise async loader
      EMC_MP_IMAGE }                   // Initialise async event id
    /* -- Initialise raw image --------------------------------------------- */
    { ImageInitRaw(ssvName, mRval, uWidth, uHeight, bdBits); }
  /* -- Constructor to initialise from known file formats ------------------ */
  Image(
    /* -- Parameters ------------------------------------------------------- */
    const StdStringView &ssvName,      // Name of object
    Memory &&mRval,                    // Source memory block to read from
    const ImageFlagsConst &ifFlags     // Loading flags
    ): /* -- Initialisation of members ------------------------------------- */
    Name{ ssvName },                   // Initialise identifier
    ICHelperImage{ cImages },          // Initialise collector helper
    SerialSlave{ ctParent.Serial() } , // Initialise identification number
    AsyncLoaderImage{ this,            // Initialise async loader
      EMC_MP_IMAGE }                   // Initialise async event id
    /* -- Initialise from array -------------------------------------------- */
    { ImageInitArray(ssvName, mRval, ifFlags); }
  /* -- Constructor to initialise image from file -------------------------- */
  Image(
    /* -- Parameters ------------------------------------------------------- */
    const StdStringView &ssvName,      // Name of image from assets to load
    const ImageFlagsConst &ifFlags     // Loading flags
    ): /* -- Initialisation of members ------------------------------------- */
    Name{ ssvName },                   // Initialise identifier
    ICHelperImage{ cImages },          // Initialise collector helper
    SerialSlave{ ctParent.Serial() } , // Initialise identification number
    AsyncLoaderImage{ this,            // Initialise async loader
      EMC_MP_IMAGE }                   // Initialise async event id
    /* -- Code to initialise from file ------------------------------------- */
    { ImageInitFile(ssvName, ifFlags); }
  /* -- MOVE constructor to SWAP with another ------------------------------ */
  Image(
    /* -- Parameters ------------------------------------------------------- */
    Image &&imOtherRval                // Other image to swap with
    ): /* -- Initialisation of members ------------------------------------- */
    Name{ StdMove(imOtherRval) },      // Initialise identifier
    ICHelperImage{ cImages },          // Initialise collector helper
    SerialSlave{ ctParent.Serial() } , // Initialise identification number
    AsyncLoaderImage{ this,            // Initialise async loader
      EMC_MP_IMAGE }                   // Initialise async event id
    /* -- Swap image over -------------------------------------------------- */
    { ImageSwap(imOtherRval); }
  /* -- Destructor --------------------------------------------------------- */
  DTORHELPER(~Image, AsyncCancel())    // Wait for loading thread to cancel
};/* -- End ---------------------------------------------------------------- */
CTOR_END_ASYNC_NOFUNCS(Images, Image, IMAGE, IMAGE)
/* ------------------------------------------------------------------------- */
using ImageVector = StdVector<Image>;  // Vector of images
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
