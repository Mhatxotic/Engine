/* == IMAGEDDS.HPP ========================================================= **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## Handles loading and saving of .DDS files with the ImageLib system.  ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace ICodecDDS {                  // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IError::P;             using namespace IFileMap::P;
using namespace IImageDef::P;          using namespace IImageLib::P;
using namespace IMemory::P;            using namespace ITexDef::P;
using namespace IUtil::P;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* ------------------------------------------------------------------------- */
class CodecDDS;                        // Class prototype
static CodecDDS *cCodecDDS = nullptr;  // Pointer to global class
class CodecDDS :                       // GIF codec object
  /* -- Base classes ------------------------------------------------------- */
  private ImageLib                     // Image format helper class
{ /* ----------------------------------------------------------------------- */
  // *** DDS_FILEHEADER (128 bytes) ***
  // 000:         uint32_t    ulMagic; // (= 0x20534444)
  // 004:     *** DDS_HEADER (124 bytes) ***
  // 004:000:     uint32_t    ulSize;
  // 008:004:     uint32_t    ulFlags;
  // 012:008:     uint32_t    ulHeight;
  // 016:012:     uint32_t    ulWidth;
  // 020:016:     uint32_t    ulPitchOrLinearSize;
  // 024:020:     uint32_t    ulDepth;
  // 028:024:     uint32_t    ulMipMapCount;
  // 032:028:     uint32_t    ulReserved1[11];
  // 076:072: *** DDS_PIXELFORMAT (32 bytes) *** [ddspf];
  // 076:072:000: uint32_t    ulSize;
  // 080:076:004: uint32_t    ulFlags;
  // 084:080:008: uint32_t    ulFourCC;
  // 088:084:012: uint32_t    ulRGBBitCount;
  // 092:088:016: uint32_t    ulRBitMask;
  // 096:092:020: uint32_t    ulGBitMask;
  // 100:096:024: uint32_t    ulBBitMask;
  // 104:100:028: uint32_t    ulABitMask;
  // 108:104:     uint32_t    ulCaps;
  // 112:108:     uint32_t    ulCaps2;
  // 116:112:     uint32_t    ulCaps3;
  // 120:116:     uint32_t    ulCaps4;
  // 124:120:     uint32_t    ulReserved2;
  /* -- Header flags ------------------------------------------------------- */
  enum DDSHeaderFlags : unsigned {
    DDSD_CAPS        = 0x00000001,   DDSD_HEIGHT      = 0x00000002,
    DDSD_WIDTH       = 0x00000004,   DDSD_PITCH       = 0x00000008,
    DDSD_PIXELFORMAT = 0x00001000,   DDSD_MIPMAPCOUNT = 0x00020000,
    DDSD_LINEARSIZE  = 0x00080000,   DDSD_DEPTH       = 0x00800000,
    DDSD_REQUIRED    = DDSD_CAPS|DDSD_HEIGHT|DDSD_WIDTH|DDSD_PIXELFORMAT|
                       DDSD_MIPMAPCOUNT|DDSD_PITCH,
    DDSD_MASK        = DDSD_CAPS|DDSD_HEIGHT|DDSD_WIDTH|DDSD_PITCH|
                       DDSD_PIXELFORMAT|DDSD_MIPMAPCOUNT|DDSD_LINEARSIZE|
                       DDSD_DEPTH
  };/* -- Pixel format flags ----------------------------------------------- */
  enum DDSPixelFormatFlags : unsigned {
    DDPF_ALPHAPIXELS = 0x00000001,   DDPF_ALPHA       = 0x00000002,
    DDPF_FOURCC      = 0x00000004,   DDPF_RGB         = 0x00000040,
    DDPF_YUV         = 0x00000200,   DDPF_LUMINANCE   = 0x00020000,
    DDPF_MASK        = DDPF_ALPHAPIXELS|DDPF_ALPHA|DDPF_FOURCC|DDPF_RGB|
                       DDPF_YUV|DDPF_LUMINANCE
  };/* -- FourCC formats --------------------------------------------------- */
  enum DDSFourCCFormat : unsigned {
    FOURCC_DXT1      = 0x31545844,   FOURCC_DXT3      = 0x33545844,
    FOURCC_DXT5      = 0x35545844
  };/* -- Capabilities primary flags --------------------------------------- */
  enum DDSCapsFlags : unsigned {
    DDSCAPS_COMPLEX  = 0x00000008,   DDSCAPS_UNKNOWN  = 0x00000002,
    DDSCAPS_TEXTURE  = 0x00001000,   DDSCAPS_MIPMAP   = 0x00400000,
    DDSCAPS_MASK     = DDSCAPS_COMPLEX|DDSCAPS_UNKNOWN|DDSCAPS_TEXTURE|
                       DDSCAPS_MIPMAP
  };/* -- Capabilities secondary flags ------------------------------------- */
  enum DDSCaps2Flags : unsigned {
    DDSCAPS2_CUBEMAP           = 0x00000200,
    DDSCAPS2_CUBEMAP_POSITIVEX = 0x00000400,
    DDSCAPS2_CUBEMAP_NEGATIVEX = 0x00000800,
    DDSCAPS2_CUBEMAP_POSITIVEY = 0x00001000,
    DDSCAPS2_CUBEMAP_NEGATIVEY = 0x00002000,
    DDSCAPS2_CUBEMAP_POSITIVEZ = 0x00004000,
    DDSCAPS2_CUBEMAP_NEGATIVEZ = 0x00008000,
    DDSCAPS2_VOLUME            = 0x00200000,
    DDSCAPS2_MASK              = DDSCAPS2_CUBEMAP|DDSCAPS2_CUBEMAP_POSITIVEX|
                                 DDSCAPS2_CUBEMAP_NEGATIVEX|
                                 DDSCAPS2_CUBEMAP_POSITIVEY|
                                 DDSCAPS2_CUBEMAP_NEGATIVEY|
                                 DDSCAPS2_CUBEMAP_POSITIVEZ|
                                 DDSCAPS2_CUBEMAP_NEGATIVEZ|DDSCAPS2_VOLUME
  };
  /* ----------------------------------------------------------------------- */
  bool Decode(FileMap &fmData, ImageData &idData)
  { // Get file size
    const size_t stSize = fmData.MemSize();
    // Quick check header which should be at least 128 bytes
    if(stSize < 128 || fmData.FileMapReadVar32LE() != 0x20534444) return false;
    // Read the image size and flags and if we got them
    if(const unsigned uSize = fmData.FileMapReadVar32LE())
    { // Must be 124 bytes long
      if(uSize != 124)
        XC("Header size must be 124 bytes!", "Required", stSize);
    } // No header bytes so throw error
    else XC("Header bytes not specified!");
    // Read header flags and if specified?
    if(const unsigned uFlags = fmData.FileMapReadVar32LE())
    { // Check that they're in the correct range
      if(uFlags & ~DDSD_MASK)
        XC("Header flags invalid!", "Flags", uFlags);
      // We should have caps, width and pixelformat
      if(uFlags & ~DDSD_REQUIRED)
        XC("Header flags required!", "Flags", uFlags, "Mask", DDSD_REQUIRED);
    } // Get and check dimensions
    if(!idData.SetDimSafe(fmData.FileMapReadVar32LE(),
          fmData.FileMapReadVar32LE()))
      XC("Dimensions invalid!",
         "Width",  idData.DimGetWidth(), "Height", idData.DimGetHeight());
    // Check that scanline length is equal to the width
    const unsigned uPitchOrLinearSize = fmData.FileMapReadVar32LEFrom(20);
    // Get bit-depth and we'll verify it later
    const unsigned uBPP = static_cast<ByteDepth>(fmData.FileMapReadVar32LE());
    // Get and check mipmap count. Theres still 1 image if this is zero.
    const unsigned uMipMapCount = fmData.FileMapReadVar32LE() + 1;
    if(uMipMapCount > 1) idData.SetMipmaps();
    // Ignore the next 44 bytes. Apps like GIMP can use this space to write
    // whatever they want here, such as 'GIMP-DDS\'.
    fmData.FileMapSeekCur(sizeof(uint32_t) * 11);
    // Get pixel format size
    const unsigned uPFSize = fmData.FileMapReadVar32LE();
    if(uPFSize != 32)
      XC("Unexpected pixel format size!", "PixelFormatSize", uPFSize);
    // Check flags and show error if incorrect flags
    const unsigned uPFFlags = fmData.FileMapReadVar32LE();
    if(uPFFlags & ~DDPF_MASK)
      XC("Unsupported pixel format flags!",
        "Flags", uPFFlags, "Mask", DDPF_MASK);
    // Carry on reading flags
    const unsigned uPFFourCC = fmData.FileMapReadVar32LE();
    // Read FOURCC data
    const unsigned uPFRGBBitCount = fmData.FileMapReadVar32LE(),
                   uPFRBitMask = fmData.FileMapReadVar32LE(),
                   uPFGBitMask = fmData.FileMapReadVar32LE(),
                   uPFBBitMask = fmData.FileMapReadVar32LE(),
                   uPFABitMask = fmData.FileMapReadVar32LE();
    // If there is compression?
    if(uPFFlags & DDPF_FOURCC)
    { // Compare pixel format...
      switch(uPFFourCC)
      { // Only compressed texture formats supported right now
        case FOURCC_DXT1: idData.SetPixelType(TT_DXT1); break;
        case FOURCC_DXT3: idData.SetPixelType(TT_DXT3); break;
        case FOURCC_DXT5: idData.SetPixelType(TT_DXT5); break;
        // Unknown compression
        default: XC("Only DXT1, DXT3 or DXT5 FourCC format supported!",
          "Type", uPFFourCC);
      } // All formats are 32-bpp and compressed
      idData.SetBitsAndBytesPerPixel(BD_RGBA);
      idData.SetCompressed();
    } // Is RGB format?
    else if(uPFFlags & DDPF_RGB)
    { // Has alpha? Then it is RGBA else then it is RGB
      if(uPFFlags & DDPF_ALPHAPIXELS)
      { // Compare bit count
        switch(uPFRGBBitCount)
        { // RGBA pixels?
          case BD_RGBA:
            // Set 32-bits per pixel
            idData.SetBitsAndBytesPerPixel(BD_RGBA);
            // Pixels are in RGBA order?
            if(uPFRBitMask == 0x000000ff && uPFGBitMask == 0x0000ff00 &&
               uPFBBitMask == 0x00ff0000 && uPFABitMask == 0xff000000)
              idData.SetPixelType(TT_RGBA);
            // Pixels are in BGRA order?
            else if(uPFRBitMask == 0x00ff0000 && uPFGBitMask == 0x0000ff00 &&
                    uPFBBitMask == 0x000000ff && uPFABitMask == 0xff000000)
              idData.SetPixelType(TT_BGRA);
            // Unsupported RGBA order
            else XC("Unsupported RGBA pixel assignments!",
              "RedBitMask",   uPFRBitMask, "GreenBitMask", uPFGBitMask,
              "BlueBitMask",  uPFBBitMask, "AlphaBitMask", uPFABitMask);
            // Done
            break;
          // GRAY+ALPHA Pixels?
          case BD_GRAYALPHA:
            // Make sure pixel bits match
            if(uPFRBitMask != 0xff || uPFGBitMask != 0xff ||
               uPFBBitMask != 0xff || uPFABitMask != 0xff00)
              XC("Unsupported LUMINANCE ALPHA pixel assignments!",
                "RedBitMask",   uPFRBitMask, "GreenBitMask", uPFGBitMask,
                "BlueBitMask",  uPFBBitMask, "AlphaBitMask", uPFABitMask);
            // Set 32-bits per pixel and LUMINANCE ALPHA pixel type
            idData.SetBitsAndBytesPerPixel(BD_GRAYALPHA);
            idData.SetPixelType(TT_GRAYALPHA);
            // DONE
            break;
          // Unsupported?
          default: XC("Detected RGBA but bit-depth not supported!",
            "PixelFormatBitCount", uPFRGBBitCount);
        }
      } // No alpha pixels?
      else
      { // Bit count must be 24 (BD_GRAY doesn't use this).
        if(uPFRGBBitCount != BD_RGB)
          XC("Detected RGB but bit-depth not supported!",
            "PixelFormatBitCount", uPFRGBBitCount);
        // Set RGBA bit-depth and byte depth
        idData.SetBitsAndBytesPerPixel(BD_RGB);
        // Pixels are in RGB order?
        if(uPFRBitMask == 0x000000ff && uPFGBitMask == 0x0000ff00 &&
           uPFBBitMask == 0x00ff0000 && uPFABitMask == 0x00000000)
          idData.SetPixelType(TT_RGB);
        // Pixels are in BGR order?
        else if(uPFRBitMask == 0x00ff0000 && uPFGBitMask == 0x0000ff00 &&
                uPFBBitMask == 0x000000ff && uPFABitMask == 0x00000000)
          idData.SetPixelType(TT_BGR);
        // Unsupported RGB order
        else XC("Unsupported RGB pixel assignments!",
          "RedBitMask",  uPFRBitMask, "GreenBitMask", uPFGBitMask,
          "BlueBitMask", uPFBBitMask, "AlphaBitMask", uPFABitMask);
      }
    } // Is RGB format?
    else if(uPFFlags & DDPF_LUMINANCE)
    { // Bit-depth should be 8
      if(uPFRGBBitCount != BD_GRAY)
        XC("Detected LUMINANCE but invalid bit-count!",
          "PixelFormatBitCount", uPFRGBBitCount);
      // Make sure pixel bits match
      if(uPFRBitMask != 0xff || uPFGBitMask != 0x00 ||
         uPFBBitMask != 0x00 || uPFABitMask != 0x00)
        XC("Unsupported LUMINANCE pixel assignments!",
          "RedBitMask",   uPFRBitMask, "GreenBitMask", uPFGBitMask,
          "BlueBitMask",  uPFBBitMask, "AlphaBitMask", uPFABitMask);
      // Set gray pixels
      idData.SetBitsAndBytesPerPixel(BD_GRAY);
      idData.SetPixelType(TT_GRAY);
    } // We do not recognise this pixel format
    else XC("Unsupported pixel format!", "PixelFormatFlags", uPFFlags);
    // Check that bit-depth matches
    if(uBPP && idData.GetBitsPerPixel() != uBPP)
      XC("Detected bit-depth doesn't match the bit-depth in the header!"
        "in the image.", "Detected", idData.GetBitsPerPixel(), "File", uBPP);
    // Get primary capabilities and if set?
    if(const unsigned uCaps1 = fmData.FileMapReadVar32LE())
    { // Throw if not in range
      if(uCaps1 & ~DDSCAPS_MASK)
        XC("Unsupported primary flags!",
          "Flags", uCaps1, "Mask", DDSCAPS_MASK);
    } // We should have some flags
    else XC("Primary flags not specified!");
    // Were secondary capabilities set?
    if(const unsigned uCaps2 = fmData.FileMapReadVar32LE())
    { // Show error if not in range
      if(uCaps2 & ~DDSCAPS2_MASK)
        XC("Invalid secondary flags!",
          "Flags", uCaps2, "Mask", DDSCAPS2_MASK);
      // None of them implemented regardless
      XC("Unimplemented secondary flags!", "Flags", uCaps2);
    } // Read rest of values
    if(const unsigned uCaps3 = fmData.FileMapReadVar32LE())
      XC("Unimplemented tertiary flags!", "Flags", uCaps3);
    if(const unsigned uCaps4 = fmData.FileMapReadVar32LE())
      XC("Unimplemented quaternary flags!", "Flags", uCaps4);
    if(const unsigned uReserved2 = fmData.FileMapReadVar32LE())
      XC("Reserved flags must not be set!", "Data", uReserved2);
    // Scan-line length is specified?
    if(uPitchOrLinearSize)
    { // Calculate actual memory for each scanline and check that it is same
      const unsigned uScanSize =
        idData.DimGetWidth()*idData.GetBytesPerPixel();
      if(uScanSize != uPitchOrLinearSize)
        XC("Scan line size mismatch versus calculated!",
          "Width",      idData.DimGetWidth(),
          "ByDepth",    idData.GetBytesPerPixel(),
          "Calculated", uScanSize, "Expected", uPitchOrLinearSize);
    } // Alocate slots as mipmaps
    idData.ReserveSlots(uMipMapCount);
    // DDS's are reversed
    idData.SetReversed();
    // DXT[n]? Compressed bitmaps need a special calculation
    if(idData.IsCompressed()) for(unsigned
      // Preinitialised variables
      uBitDiv    = (idData.GetPixelType() == TT_DXT1 ? 8 : 16),
      uMipIndex  = 0,
      uMipWidth  = idData.DimGetWidth(),
      uMipHeight = idData.DimGetHeight(),
      uMipBPP    = idData.GetBytesPerPixel(),
      uMipSize   = ((uMipWidth+3)/4)*((uMipHeight+3)/4)*uBitDiv;
      // Conditions
      uMipIndex < uMipMapCount &&
      (uMipWidth || uMipHeight);
      // Pocedures on each loop
    ++uMipIndex,
      uMipWidth  >>= 1,
      uMipHeight >>= 1,
      uMipBPP      = UtilMaximum(uMipBPP >> 1, 1U),
      uMipSize     = ((uMipWidth+3)/4)*((uMipHeight+3)/4)*uBitDiv)
    { // Read compressed data from file and show error if not enough data
      Memory mPixels{ fmData.FileMapReadBlock(uMipSize) };
      if(mPixels.MemSize() != uMipSize)
        XC("Read error!", "Expected", uMipSize, "Actual", mPixels.MemSize());
      // Push back a new slot for every new mipmap
      idData.AddSlot(mPixels, uMipWidth, uMipHeight);
    } // Uncompressed image?
    else for(unsigned
      // Preinitialised variables
      uMipIndex  = 0,                               // Mipmap index
      uMipWidth  = idData.DimGetWidth(),            // Mipmap width
      uMipHeight = idData.DimGetHeight(),           // Mipmap height
      uMipBPP    = idData.GetBytesPerPixel(),       // Mipmap bytes-per-pixel
      uMipSize   = uMipWidth*uMipHeight*uMipBPP; // Mipmap size in memory
      // Conditions
      uMipIndex < uMipMapCount &&    // Mipmap count not reached and...
      (uMipWidth || uMipHeight);     // ...width and height is not zero
      // Pocedures on each loop
    ++uMipIndex,                      // Increment mipmap index
      uMipWidth  >>= 1,               // Divide mipmap width by 2
      uMipHeight >>= 1,               // Divide mipmap height by 2
      uMipSize = uMipWidth*uMipHeight*uMipBPP) // New mipmap size
    { // Read uncompressed data from file and show error if not enough data
      Memory mData{ fmData.FileMapReadBlock(uMipSize) };
      if(mData.MemSize() != uMipSize)
        XC("Read error!", "Expected", uMipSize, "Actual", mData.MemSize());
      // Push back a new slot for every new mipmap
      idData.AddSlot(mData, uMipWidth, uMipHeight);
    } // Succeeded
    return true;
  }
  /* -- Default constructor ------------------------------------- */ protected:
  CodecDDS() :
    /* -- Initialisers ----------------------------------------------------- */
    ImageLib{ IFMT_DDS, "DirectDraw Surface", "DDS",
      bind(&CodecDDS::Decode, this, _1, _2) }
    /* -- Set global pointer to static class ------------------------------- */
    { cCodecDDS = this; }
};/* ----------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
