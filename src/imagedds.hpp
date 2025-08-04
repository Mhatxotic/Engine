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
  enum DDSHeaderFlags : unsigned int {
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
  enum DDSPixelFormatFlags : unsigned int {
    DDPF_ALPHAPIXELS = 0x00000001,   DDPF_ALPHA       = 0x00000002,
    DDPF_FOURCC      = 0x00000004,   DDPF_RGB         = 0x00000040,
    DDPF_YUV         = 0x00000200,   DDPF_LUMINANCE   = 0x00020000,
    DDPF_MASK        = DDPF_ALPHAPIXELS|DDPF_ALPHA|DDPF_FOURCC|DDPF_RGB|
                       DDPF_YUV|DDPF_LUMINANCE
  };/* -- FourCC formats --------------------------------------------------- */
  enum DDSFourCCFormat : unsigned int {
    FOURCC_DXT1      = 0x31545844,   FOURCC_DXT3      = 0x33545844,
    FOURCC_DXT5      = 0x35545844
  };/* -- Capabilities primary flags --------------------------------------- */
  enum DDSCapsFlags : unsigned int {
    DDSCAPS_COMPLEX  = 0x00000008,   DDSCAPS_UNKNOWN  = 0x00000002,
    DDSCAPS_TEXTURE  = 0x00001000,   DDSCAPS_MIPMAP   = 0x00400000,
    DDSCAPS_MASK     = DDSCAPS_COMPLEX|DDSCAPS_UNKNOWN|DDSCAPS_TEXTURE|
                       DDSCAPS_MIPMAP
  };/* -- Capabilities secondary flags ------------------------------------- */
  enum DDSCaps2Flags : unsigned int {
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
    if(const unsigned int uiSize = fmData.FileMapReadVar32LE())
    { // Must be 124 bytes long
      if(uiSize != 124)
        XC("Header size must be 124 bytes!", "Required", stSize);
    } // No header bytes so throw error
    else XC("Header bytes not specified!");
    // Read header flags and if specified?
    if(const unsigned int uiFlags = fmData.FileMapReadVar32LE())
    { // Check that they're in the correct range
      if(uiFlags & ~DDSD_MASK)
        XC("Header flags invalid!", "Flags", uiFlags);
      // We should have caps, width and pixelformat
      if(uiFlags & ~DDSD_REQUIRED)
        XC("Header flags required!", "Flags", uiFlags, "Mask", DDSD_REQUIRED);
    } // Get and check dimensions
    if(!idData.SetDimSafe(fmData.FileMapReadVar32LE(),
          fmData.FileMapReadVar32LE()))
      XC("Dimensions invalid!",
         "Width",  idData.DimGetWidth(), "Height", idData.DimGetHeight());
    // Check that scanline length is equal to the width
    const unsigned int uiPitchOrLinearSize = fmData.FileMapReadVar32LEFrom(20);
    // Get bit-depth and we'll verify it later
    const unsigned int uiBPP =
      static_cast<ByteDepth>(fmData.FileMapReadVar32LE());
    // Get and check mipmap count. Theres still 1 image if this is zero.
    const unsigned int uiMipMapCount = fmData.FileMapReadVar32LE() + 1;
    if(uiMipMapCount > 1) idData.SetMipmaps();
    // Ignore the next 44 bytes. Apps like GIMP can use this space to write
    // whatever they want here, such as 'GIMP-DDS\'.
    fmData.FileMapSeekCur(sizeof(uint32_t) * 11);
    // Get pixel format size
    const unsigned int uiPFSize = fmData.FileMapReadVar32LE();
    if(uiPFSize != 32)
      XC("Unexpected pixel format size!", "PixelFormatSize", uiPFSize);
    // Check flags and show error if incorrect flags
    const unsigned int uiPFFlags = fmData.FileMapReadVar32LE();
    if(uiPFFlags & ~DDPF_MASK)
      XC("Unsupported pixel format flags!",
         "Flags", uiPFFlags, "Mask", DDPF_MASK);
    // Carry on reading flags
    const unsigned int uiPFFourCC = fmData.FileMapReadVar32LE();
    // Read FOURCC data
    const unsigned int uiPFRGBBitCount = fmData.FileMapReadVar32LE(),
                       uiPFRBitMask = fmData.FileMapReadVar32LE(),
                       uiPFGBitMask = fmData.FileMapReadVar32LE(),
                       uiPFBBitMask = fmData.FileMapReadVar32LE(),
                       uiPFABitMask = fmData.FileMapReadVar32LE();
    // If there is compression?
    if(uiPFFlags & DDPF_FOURCC)
    { // Compare pixel format...
      switch(uiPFFourCC)
      { // Only compressed texture formats supported right now
        case FOURCC_DXT1: idData.SetPixelType(TT_DXT1); break;
        case FOURCC_DXT3: idData.SetPixelType(TT_DXT3); break;
        case FOURCC_DXT5: idData.SetPixelType(TT_DXT5); break;
        // Unknown compression
        default: XC("Only DXT1, DXT3 or DXT5 FourCC format supported!",
                    "Type", uiPFFourCC);
      } // All formats are 32-bpp and compressed
      idData.SetBitsAndBytesPerPixel(BD_RGBA);
      idData.SetCompressed();
    } // Is RGB format?
    else if(uiPFFlags & DDPF_RGB)
    { // Has alpha? Then it is RGBA else then it is RGB
      if(uiPFFlags & DDPF_ALPHAPIXELS)
      { // Compare bit count
        switch(uiPFRGBBitCount)
        { // RGBA pixels?
          case BD_RGBA:
            // Set 32-bits per pixel
            idData.SetBitsAndBytesPerPixel(BD_RGBA);
            // Pixels are in RGBA order?
            if(uiPFRBitMask == 0x000000ff && uiPFGBitMask == 0x0000ff00 &&
               uiPFBBitMask == 0x00ff0000 && uiPFABitMask == 0xff000000)
              idData.SetPixelType(TT_RGBA);
            // Pixels are in BGRA order?
            else if(uiPFRBitMask == 0x00ff0000 && uiPFGBitMask == 0x0000ff00 &&
                    uiPFBBitMask == 0x000000ff && uiPFABitMask == 0xff000000)
              idData.SetPixelType(TT_BGRA);
            // Unsupported RGBA order
            else XC("Unsupported RGBA pixel assignments!",
                   "RedBitMask",   uiPFRBitMask, "GreenBitMask", uiPFGBitMask,
                   "BlueBitMask",  uiPFBBitMask, "AlphaBitMask", uiPFABitMask);
            // Done
            break;
          // GRAY+ALPHA Pixels?
          case BD_GRAYALPHA:
            // Make sure pixel bits match
            if(uiPFRBitMask != 0xff || uiPFGBitMask != 0xff ||
               uiPFBBitMask != 0xff || uiPFABitMask != 0xff00)
              XC("Unsupported LUMINANCE ALPHA pixel assignments!",
                 "RedBitMask",   uiPFRBitMask, "GreenBitMask", uiPFGBitMask,
                 "BlueBitMask",  uiPFBBitMask, "AlphaBitMask", uiPFABitMask);
            // Set 32-bits per pixel and LUMINANCE ALPHA pixel type
            idData.SetBitsAndBytesPerPixel(BD_GRAYALPHA);
            idData.SetPixelType(TT_GRAYALPHA);
            // DONE
            break;
          // Unsupported?
          default: XC("Detected RGBA but bit-depth not supported!",
                      "PixelFormatBitCount", uiPFRGBBitCount);
        }
      } // No alpha pixels?
      else
      { // Bit count must be 24 (BD_GRAY doesn't use this).
        if(uiPFRGBBitCount != BD_RGB)
          XC("Detected RGB but bit-depth not supported!",
             "PixelFormatBitCount", uiPFRGBBitCount);
        // Set RGBA bit-depth and byte depth
        idData.SetBitsAndBytesPerPixel(BD_RGB);
        // Pixels are in RGB order?
        if(uiPFRBitMask == 0x000000ff && uiPFGBitMask == 0x0000ff00 &&
           uiPFBBitMask == 0x00ff0000 && uiPFABitMask == 0x00000000)
          idData.SetPixelType(TT_RGB);
        // Pixels are in BGR order?
        else if(uiPFRBitMask == 0x00ff0000 && uiPFGBitMask == 0x0000ff00 &&
                uiPFBBitMask == 0x000000ff && uiPFABitMask == 0x00000000)
          idData.SetPixelType(TT_BGR);
        // Unsupported RGB order
        else XC("Unsupported RGB pixel assignments!",
                "RedBitMask",  uiPFRBitMask, "GreenBitMask", uiPFGBitMask,
                "BlueBitMask", uiPFBBitMask, "AlphaBitMask", uiPFABitMask);
      }
    } // Is RGB format?
    else if(uiPFFlags & DDPF_LUMINANCE)
    { // Bit-depth should be 8
      if(uiPFRGBBitCount != BD_GRAY)
        XC("Detected LUMINANCE but invalid bit-count!",
           "PixelFormatBitCount", uiPFRGBBitCount);
      // Make sure pixel bits match
      if(uiPFRBitMask != 0xff || uiPFGBitMask != 0x00 ||
         uiPFBBitMask != 0x00 || uiPFABitMask != 0x00)
        XC("Unsupported LUMINANCE pixel assignments!",
           "RedBitMask",   uiPFRBitMask, "GreenBitMask", uiPFGBitMask,
           "BlueBitMask",  uiPFBBitMask, "AlphaBitMask", uiPFABitMask);
      // Set gray pixels
      idData.SetBitsAndBytesPerPixel(BD_GRAY);
      idData.SetPixelType(TT_GRAY);
    } // We do not recognise this pixel format
    else XC("Unsupported pixel format!", "PixelFormatFlags", uiPFFlags);
    // Check that bit-depth matches
    if(uiBPP && idData.GetBitsPerPixel() != uiBPP)
      XC("Detected bit-depth doesn't match the bit-depth in the header!"
         "in the image.", "Detected", idData.GetBitsPerPixel(), "File", uiBPP);
    // Get primary capabilities and if set?
    if(const unsigned int uiCaps1 = fmData.FileMapReadVar32LE())
    { // Throw if not in range
      if(uiCaps1 & ~DDSCAPS_MASK)
        XC("Unsupported primary flags!",
           "Flags", uiCaps1, "Mask", DDSCAPS_MASK);
    } // We should have some flags
    else XC("Primary flags not specified!");
    // Were secondary capabilities set?
    if(const unsigned int uiCaps2 = fmData.FileMapReadVar32LE())
    { // Show error if not in range
      if(uiCaps2 & ~DDSCAPS2_MASK)
        XC("Invalid secondary flags!",
           "Flags", uiCaps2, "Mask", DDSCAPS2_MASK);
      // None of them implemented regardless
      XC("Unimplemented secondary flags!", "Flags", uiCaps2);
    } // Read rest of values
    if(const unsigned int uiCaps3 = fmData.FileMapReadVar32LE())
      XC("Unimplemented tertiary flags!", "Flags", uiCaps3);
    if(const unsigned int uiCaps4 = fmData.FileMapReadVar32LE())
      XC("Unimplemented quaternary flags!", "Flags", uiCaps4);
    if(const unsigned int uiReserved2 = fmData.FileMapReadVar32LE())
      XC("Reserved flags must not be set!", "Data", uiReserved2);
    // Scan-line length is specified?
    if(uiPitchOrLinearSize)
    { // Calculate actual memory for each scanline and check that it is same
      const unsigned int uiScanSize =
        idData.DimGetWidth()*idData.GetBytesPerPixel();
      if(uiScanSize != uiPitchOrLinearSize)
        XC("Scan line size mismatch versus calculated!",
           "Width",      idData.DimGetWidth(),
           "ByDepth",    idData.GetBytesPerPixel(),
           "Calculated", uiScanSize, "Expected", uiPitchOrLinearSize);
    } // Alocate slots as mipmaps
    idData.ReserveSlots(uiMipMapCount);
    // DDS's are reversed
    idData.SetReversed();
    // DXT[n]? Compressed bitmaps need a special calculation
    if(idData.IsCompressed()) for(unsigned int
      // Preinitialised variables
      uiBitDiv    = (idData.GetPixelType() == TT_DXT1 ? 8 : 16),
      uiMipIndex  = 0,
      uiMipWidth  = idData.DimGetWidth(),
      uiMipHeight = idData.DimGetHeight(),
      uiMipBPP    = idData.GetBytesPerPixel(),
      uiMipSize   = ((uiMipWidth+3)/4)*((uiMipHeight+3)/4)*uiBitDiv;
      // Conditions
      uiMipIndex < uiMipMapCount &&
      (uiMipWidth || uiMipHeight);
      // Pocedures on each loop
    ++uiMipIndex,
      uiMipWidth  >>= 1,
      uiMipHeight >>= 1,
      uiMipBPP      = UtilMaximum(uiMipBPP >> 1, 1),
      uiMipSize     = ((uiMipWidth+3)/4)*((uiMipHeight+3)/4)*uiBitDiv)
    { // Read compressed data from file and show error if not enough data
      Memory mPixels{ fmData.FileMapReadBlock(uiMipSize) };
      if(mPixels.MemSize() != uiMipSize)
        XC("Read error!", "Expected", uiMipSize, "Actual", mPixels.MemSize());
      // Push back a new slot for every new mipmap
      idData.AddSlot(mPixels, uiMipWidth, uiMipHeight);
    } // Uncompressed image?
    else for(unsigned int
      // Preinitialised variables
      uiMipIndex  = 0,                               // Mipmap index
      uiMipWidth  = idData.DimGetWidth(),            // Mipmap width
      uiMipHeight = idData.DimGetHeight(),           // Mipmap height
      uiMipBPP    = idData.GetBytesPerPixel(),       // Mipmap bytes-per-pixel
      uiMipSize   = uiMipWidth*uiMipHeight*uiMipBPP; // Mipmap size in memory
      // Conditions
      uiMipIndex < uiMipMapCount &&    // Mipmap count not reached and...
      (uiMipWidth || uiMipHeight);     // ...width and height is not zero
      // Pocedures on each loop
    ++uiMipIndex,                      // Increment mipmap index
      uiMipWidth  >>= 1,               // Divide mipmap width by 2
      uiMipHeight >>= 1,               // Divide mipmap height by 2
      uiMipSize = uiMipWidth*uiMipHeight*uiMipBPP) // New mipmap size
    { // Read uncompressed data from file and show error if not enough data
      Memory mData{ fmData.FileMapReadBlock(uiMipSize) };
      if(mData.MemSize() != uiMipSize)
        XC("Read error!", "Expected", uiMipSize, "Actual", mData.MemSize());
      // Push back a new slot for every new mipmap
      idData.AddSlot(mData, uiMipWidth, uiMipHeight);
    } // Succeeded
    return true;
  }
  /* -- Default constructor ------------------------------------- */ protected:
  CodecDDS(void) :
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
