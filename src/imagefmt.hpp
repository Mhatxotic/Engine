/* == IMAGEFMT.HPP ========================================================= **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## These classes are plugins for the ImgLib manager to allow loading   ## **
** ## of certain formatted images.                                        ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IImageFormat {               // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IClock::P;             using namespace ICVar::P;
using namespace ICVarLib::P;           using namespace IDim;
using namespace IError::P;             using namespace IFileMap::P;
using namespace IFStream::P;           using namespace IImageDef::P;
using namespace IImageLib::P;          using namespace ILog::P;
using namespace IMemory::P;            using namespace IStd::P;
using namespace IString::P;            using namespace ITexDef::P;
using namespace IUtf;                  using namespace IUtil::P;
using namespace Lib::NSGif;            using namespace Lib::OS::JpegTurbo;
using namespace Lib::Png;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* ========================================================================= **
** ######################################################################### **
** ## DirectDraw Surface                                              DDS ## **
** ######################################################################### **
** -- DDS Codec Object ----------------------------------------------------- */
struct CodecDDS final :                // Members initially public
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
  static bool Load(FileMap &fmData, ImageData &idData)
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
      // Pre-initialised variables
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
      // Pre-initialised variables
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
  /* -- Constructor -------------------------------------------------------- */
  CodecDDS(void) :
    /* -- Initialisers ----------------------------------------------------- */
    ImageLib{ IFMT_DDS, "DirectDraw Surface", "DDS", Load }
    /* -- No code ---------------------------------------------------------- */
    { }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(CodecDDS)            // Suppress default functions for safety
};/* -- End ---------------------------------------------------------------- */
/* ========================================================================= **
** ######################################################################### **
** ## Graphics Interchange Format                                     GIF ## **
** ######################################################################### **
** -- GIF Codec Object ----------------------------------------------------- */
class CodecGIF final :                 // Members initially private
  /* -- Base classes ------------------------------------------------------- */
  private ImageLib                     // Image format helper class
{ /* -- Allocate memory ---------------------------------------------------- */
  static void *GIFCreate(int iW, int iH)
    { return UtilMemAlloc<void>
        (static_cast<unsigned int>(iW * iH) * BY_RGBA); }
  /* -- Free memory -------------------------------------------------------- */
  static void GIFDestroy(void*const vpBuffer)
    { if(!vpBuffer) throw runtime_error{ "Free buffer null pointer" };
      UtilMemFree(vpBuffer); }
  /* -- Return ptr to pixel data in image ---------------------------------- */
  static unsigned char *GIFRead(void*const vpBuffer)
    { if(!vpBuffer) throw runtime_error{ "Get buffer null pointer" };
      return reinterpret_cast<unsigned char*>(vpBuffer); }
  /* -- Sets whether a image should opaque --------------------------------- */
  static void GIFOpaque(            // False positive / Upstream issue
    void*const vpBuffer, bool)      // cppcheck-suppress constParameterCallback
      { if(!vpBuffer) throw runtime_error{ "Set opaque null pointer" }; }
  /* -- If image has an opaque alpha channel ------------------------------- */
  static bool GIFIsOpaque(          // False positive / Upstream issue
    void*const vpBuffer)            // cppcheck-suppress constParameterCallback
      { if(!vpBuffer) throw runtime_error{ "Test opaque null pointer" };
        return true; }
  /* -- Image changed, flush cache ----------------------------------------- */
  static void GIFFlush(             // False positive / Upstream issue
    void*const vpBuffer)            // cppcheck-suppress constParameterCallback
      { if(!vpBuffer) throw runtime_error{ "Modified data null pointer" }; }
  /* --------------------------------------------------------------- */ public:
  static bool Load(FileMap &fmData, ImageData &idData)
  { // Must have at least 10 bytes for header 'GIF9' and ending footer.
    if(fmData.MemSize() < 10 || fmData.FileMapReadVar32LE() != 0x38464947)
      return false;
    // Next word must be 7a or 9a. Else not a gif file.
    const unsigned int uiMagic2 = fmData.FileMapReadVar16LE();
    if(uiMagic2 != 0x6137 && uiMagic2 != 0x6139) return false;
    // Read and check last 2 footer bytes of file
    if(fmData.FileMapReadVar16LEFrom(fmData.MemSize() -
         sizeof(uint16_t)) != 0x3b00)
      return false;
    // Set gif callbacks
    nsgif_bitmap_cb_vt sBMPCB{ GIFCreate, GIFDestroy, GIFRead, GIFOpaque,
      GIFIsOpaque, GIFFlush, nullptr };
    // Animations data and return code
    nsgif_t *nsgCtx;
    // create our gif animation
    switch(const nsgif_error nsgErr =
      nsgif_create(&sBMPCB, NSGIF_BITMAP_FMT_R8G8B8A8, &nsgCtx))
    { // Succeeded?
      case NSGIF_OK: break;
      // Anything else?
      default: XC("Failed to create nsgif context!",
        "Reason", nsgif_strerror(nsgErr), "Code", nsgErr);
    } // Trap exceptions so we can clean up
    try
    { // Decode the gif and report error if failed
      switch(const nsgif_error nsgErr =
        nsgif_data_scan(nsgCtx, fmData.MemSize(),
          fmData.MemPtr<unsigned char>()))
      { // Succeeded?
        case NSGIF_OK: break;
        // Anything else?
        default: XC("Failed to parse gif file!",
          "Reason", nsgif_strerror(nsgErr), "Code", nsgErr);
      } // The data is complete
      nsgif_data_complete(nsgCtx);
      // Get information about gif and if we got it?
     	if(const nsgif_info_t*const nsgInfo = nsgif_get_info(nsgCtx))
      { // Reserve memory for frames
        idData.ReserveSlots(nsgInfo->frame_count);
        // Set members
        idData.SetBitsAndBytesPerPixel(BD_RGBA);
        idData.SetPixelType(TT_RGBA);
        idData.SetReversed();
        idData.DimSet(nsgInfo->width, nsgInfo->height);
        // Decode the frames
        for(unsigned int uiIndex = 0;
                         uiIndex < nsgInfo->frame_count;
                       ++uiIndex)
        { // Area and frame data
          nsgif_rect_t nsgRect;
          uint32_t uiFrame, uiDelay;
          // Prepare frame
          switch(const nsgif_error nsgErr =
            nsgif_frame_prepare(nsgCtx, &nsgRect, &uiDelay, &uiFrame))
          { // Succeeded?
            case NSGIF_OK: break;
            // Anything else?
            default: XC("Failed to prepare frame!",
              "Reason", nsgif_strerror(nsgErr), "Code", nsgErr);
          } // Get frame info
          if(const nsgif_frame_info_t*const nsgFrame =
            nsgif_get_frame_info(nsgCtx, uiFrame))
          { // Decode the frame and throw exception if failed
            nsgif_bitmap_t *nsgBitmap;
            switch(const nsgif_error nsgErr =
              nsgif_frame_decode(nsgCtx, uiFrame, &nsgBitmap))
            { // Succeeded?
              case NSGIF_OK: break;
              // Anything else?
              default: XC("Failed to decode gif!",
                "Reason", nsgif_strerror(nsgErr), "Code", nsgErr);
            } // Calculate image size
            const size_t stSize =
              nsgInfo->width * nsgInfo->height * idData.GetBytesPerPixel();
            // Now we can just copy the memory over
            Memory mData{ stSize, nsgBitmap };
            idData.AddSlot(mData, nsgInfo->width, nsgInfo->height);
          } // Failed to get frame data
          else XC("Failed to get frame data!");
        }
      } // Failed to get gif information
      else XC("Failed to get gif information!");
      // Success! Clean up
      nsgif_destroy(nsgCtx);
      // Success parsing gif
      return true;
    } // Caught an exception (unlikely)
    catch(const exception &)
    { // Clean up
      nsgif_destroy(nsgCtx);
      // Throw back to proper exception handler if message sent
      throw;
    } // Never gets here
  }
  /* -- Constructor -------------------------------------------------------- */
  CodecGIF(void) :
    /* -- Initialisers ----------------------------------------------------- */
    ImageLib{ IFMT_GIF, "Graphics Interchange Format", "GIF", Load }
    /* -- No code ---------------------------------------------------------- */
    { }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(CodecGIF)            // Suppress default functions for safety
};/* -- End ---------------------------------------------------------------- */
/* ========================================================================= **
** ######################################################################### **
** ## Portable Network Graphics                                       PNG ## **
** ######################################################################### **
** -- PNG Codec Object ----------------------------------------------------- */
class CodecPNG final :                 // Members initially private
  /* -- Base classes ------------------------------------------------------- */
  private ImageLib                     // Image format helper class
{ /* -- Private typedefs --------------------------------------------------- */
  typedef vector<png_bytep> PngPtrVec; // Png pointer vector
  /* -- PNG callbacks ------------------------------------------------------ */
  static void PngError[[noreturn]](png_structp, png_const_charp pccString)
    { throw runtime_error{ pccString }; }
  static void PngWarning(png_structp psData, png_const_charp pccString)
    { cLog->LogWarningExSafe("Image '$' warning: $.",
        reinterpret_cast<char*>(png_get_error_ptr(psData)), pccString); }
  static void PngRead(png_structp psD, png_bytep ucP, png_size_t stC)
  { // Read file and return if we read the correct number of bytes
    const size_t stRead = reinterpret_cast<FileMap*>
      (png_get_io_ptr(psD))->FileMapReadToAddr(ucP, stC);
    if(stRead == stC) return;
    // Error occured so longjmp()
    png_error(psD, StrFormat("Read only $ of the $ requested bytes",
      stRead, stC).c_str());
  }
  /* -- Save png file ---------------------------------------------- */ public:
  static bool Save(const FStream &fmData, const ImageData &idData,
    const ImageSlot &isData)
  { // Check that dimensions are set
    if(isData.DimIsNotSet())
      XC("Dimensions are invalid!",
         "Width",  isData.DimGetWidth(),
         "Height", isData.DimGetHeight());
    // Check that there is data
    if(isData.MemIsEmpty()) XC("No image data!", "Size", isData.MemSize());
    // Png writer helper class
    class PngWriter
    { // Private variables
      png_structp  psData;             // PNG struct data
      png_infop    piData;             // PNG info struct data
      // Call when finished writing image
      public: void Finish(void*const vpData)
      { // Send image data to png writer
        png_write_image(psData, reinterpret_cast<png_bytepp>(vpData));
        // Finished writing data to png file
        png_write_end(psData, piData);
      } // Set header information
      void Header(const Dimensions<> &dDim, const int iBitDepth,
        const int iColourType, const int iInterlaceType,
        const int iCompressionType, const int iFilterType)
      { // Set format of our data and how we want the data stored
        png_set_IHDR(psData, piData, dDim.DimGetWidth(), dDim.DimGetHeight(),
          iBitDepth, iColourType, iInterlaceType, iCompressionType,
          iFilterType);
        // Finished writing metadata and header information
        png_write_info(psData, piData);
      } // Write a metadata item
      void Meta(const char*const cpK, const char*cpV, const size_t stS)
      { // Create struct
        png_text ptData{
          PNG_TEXT_COMPRESSION_NONE,   // Compression
          const_cast<png_charp>(cpK),  // Key
          const_cast<png_charp>(cpV),  // Text
          stS,                         // Text Length
          0,                           // ITxt Length (?)
          nullptr,                     // Lang
          nullptr                      // Lang Key
        }; // Set the header
        png_set_text(psData, piData, &ptData, 1);
      }
      void Meta(const char*const cpK, const char*cpV)
        { Meta(cpK, cpV, strlen(cpV)); }
      void Meta(const char*const cpK, const string &strV)
        { Meta(cpK, strV.c_str(), strV.length()); }
      void Meta(const char*const cpK, const string_view &strvV)
        { Meta(cpK, strvV.data(), strvV.length()); }
      // Constructor
      explicit PngWriter(const FStream &fsC) :
        // Initialisers
        psData(png_create_write_struct(  // Create a write struct
          PNG_LIBPNG_VER_STRING,         // Set version string
          UtfToNonConstCast<png_voidp>(  // Send user parameter
            fsC.IdentGetCStr()),         // Set filename as user parameter
          PngError,                      // Set error callback function
          PngWarning)),                  // Set warning callback function
        piData(                          // We'll handle the info struct here
          png_create_info_struct(psData))// Func checks if psData=NULL so safe
      { // Check to make sure write struct is valid
        if(!psData) XC("Create PNG write struct failed!");
        // Check to make sure info struct is valid
        if(!piData) XC("Create PNG info struct failed!");
        // Assign file stream handle
        png_init_io(psData, fsC.FStreamGetCtx());
      } // Destructor that cleans up the libpng context
      ~PngWriter(void) { png_destroy_write_struct(&psData, &piData); }
    } // Send file stream to constructor
    pwC{ fmData };
    // Set system data in metadata
    { using namespace ISystem::P;
      pwC.Meta("Title", cSystem->GetGuestTitle());
      pwC.Meta("Version", cSystem->GetGuestVersion());
      pwC.Meta("Author", cSystem->GetGuestAuthor());
      pwC.Meta("Copyright", cSystem->GetGuestCopyright());
      pwC.Meta("Creation Time", cmSys.FormatTime());
      pwC.Meta("Description", cSystem->ENGName() + " Exported Image");
      pwC.Meta("Software", StrFormat("$ ($) v$.$.$.$ ($-bit) by $",
        cSystem->ENGName(), cSystem->ENGBuildType(), cSystem->ENGMajor(),
        cSystem->ENGMinor(), cSystem->ENGBuild(), cSystem->ENGRevision(),
        cSystem->ENGBits(), cSystem->ENGAuthor()));
      pwC.Meta("Comment", cSystem->GetGuestWebsite());
    } // Set renderer in metadata
    { using namespace IOgl::P;
      pwC.Meta("Source", StrFormat("$ ($ by $)",
        cOgl->GetRenderer(), cOgl->GetVersion(), cOgl->GetVendor()));
    }
    // Create vector array to hold pointers to each scanline
    PngPtrVec ppvList{ isData.DimGetHeight<size_t>() };
    // Bit depth and colour type of data
    int iColourType;
    // Compare bitrate
    switch(idData.GetBitsPerPixel())
    { // Monochrome (1-bpp)?
      case BD_BINARY:
        // Set binary header type
        pwC.Header(isData, 1, PNG_COLOR_TYPE_GRAY, PNG_INTERLACE_ADAM7,
          PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
        // Set location of each scanline from the bottom
        for(size_t stScanIndex = ppvList.size() - 1,
                   stStride = isData.DimGetWidth() / 8;
                   stScanIndex != StdMaxSizeT;
                 --stScanIndex)
          ppvList[stScanIndex] =
            isData.MemRead<png_byte>(stScanIndex * stStride);
        // Done
        break;
      // Grayscale (8-bpp)?
      case BD_GRAY: iColourType = PNG_COLOR_TYPE_GRAY; goto Scan;
      // Grayscale with alpha (16-bpp)?
      case BD_GRAYALPHA: iColourType = PNG_COLOR_TYPE_GRAY_ALPHA; goto Scan;
      // True-colour (24-bpp)?
      case BD_RGB: iColourType = PNG_COLOR_TYPE_RGB; goto Scan;
      // True-colour with alpha (32-bpp)?
      case BD_RGBA: iColourType = PNG_COLOR_TYPE_RGB_ALPHA;
        // All the other types (except binary) converge to here
        Scan:; pwC.Header(isData, 8, iColourType, PNG_INTERLACE_ADAM7,
          PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
        // Set location of each scanline
        for(size_t stScanIndex = 0,
                   stScanLinesM1 = ppvList.size() - 1,
                   stStride = isData.DimGetWidth() * idData.GetBytesPerPixel();
                   stScanIndex < ppvList.size();
                 ++stScanIndex)
          ppvList[stScanIndex] =
            isData.MemRead<png_byte>((stScanLinesM1 - stScanIndex) * stStride);
        // Done
        break;
      // Un-supported format
      default: XC("Image is not binary or 24-bit!",
                  "BitsPerPixel", idData.GetBitsPerPixel());
    } // Finish write image
    pwC.Finish(ppvList.data());
    // Success
    return true;
  }
  /* ----------------------------------------------------------------------- */
  static bool Load(FileMap &fmData, ImageData &idData)
  { // Not a PNG file if not at least 8 bytes or header is incorrect
    if(fmData.MemSize() < 8 ||
       !png_check_sig(fmData.FileMapReadPtr<png_byte>(8), 8))
      return false;
    // Crete safe reader struct and info
    struct PngReader
    { // Private variables
      png_structp psData;
      png_infop   piData;
      // Constructor
      explicit PngReader(FileMap &fmC) :
        // Initialisers
        psData(png_create_read_struct(   // Create a read struct
          PNG_LIBPNG_VER_STRING,         // Set version string
          UtfToNonConstCast<png_voidp>(     // Send user parameter
            fmC.IdentGetCStr()),         // Set filename as user parameter
          PngError,                      // Set error callback function
          PngWarning)),                  // Set warning callback function
        piData(                          // We'll handle the info struct here
          png_create_info_struct(psData))// Func checks if psData=NULL so safe
      { // Check to make sure write struct is valid
        if(!psData) XC("Create PNG read struct failed!");
        // Check to make sure info struct is valid
        if(!piData) XC("Create PNG info struct failed!");
        // Set read callback, header bytes and then read header
        png_set_read_fn(psData, reinterpret_cast<void*>(&fmC), PngRead);
        png_set_sig_bytes(psData, 8);
        png_read_info(psData, piData);
      } // Destructor that cleans up the libpng context
      ~PngReader(void) { png_destroy_read_struct(&psData, &piData, nullptr); }
    } // Send file map class to constructor
    prC{ fmData };
    // Get pointers to created addresses
    png_structp psData = prC.psData;
    png_infop   piData = prC.piData;
    // The palette if needed and the number of colours it has
    png_colorp palData = nullptr;
    int iPalette = 0;
    // Do we have alpha?
    const bool bAlpha = !!png_get_valid(psData, piData, PNG_INFO_tRNS);
    // Get and check bits-per-CHANNEL (may change)
    switch(const unsigned int uiBPC = png_get_bit_depth(psData, piData))
    { // 1 bits-per-CHANNEL? (binary image)
      case 1:
        // Expand if requested or there is an alpha channel
        if(idData.IsNotConvertRGBA() && idData.IsNotConvertRGB() &&
           idData.IsNotConvertGPUCompat())
        { // Strip alpha if it is there
          if(bAlpha) png_set_strip_alpha(psData);
          // Transfomrations complete so update
          png_set_interlace_handling(psData);;
          png_read_update_info(psData, piData);
          // Update the pixel type to zero. This means that this format only
          // works with our BitMask system.
          idData.SetBitsPerPixel(BD_BINARY);
          idData.SetBytesPerPixel(BY_GRAY);
          idData.SetPixelType(TT_NONE);
          // Set image dimensions
          idData.DimSet(png_get_image_width(psData, piData),
                     png_get_image_height(psData, piData));
          // Initialise memory
          Memory mPixels{ UtilMaximum(1UL, idData.TotalPixels() / 8) };
          // Create vector array to hold scanline pointers and size it
          PngPtrVec ppvList{ idData.DimGetHeight<size_t>() };
          // For each scanline
          for(size_t stHeight = idData.DimGetHeight<size_t>(),
                     stHeightM1 = stHeight - 1,
                     stStride = UtilMaximum(1UL,
                       idData.DimGetWidth<size_t>() / 8),
                     stRow = 0;
                     stRow < stHeight;
                   ++stRow)
            ppvList[stRow] =
              mPixels.MemRead<png_byte>((stHeightM1 - stRow) *
                stStride, stStride);
          // Read image
          png_read_image(psData, ppvList.data());
          png_read_end(psData, piData);
          // Success, add the image data to list
          idData.AddSlot(mPixels);
          // OK!
          return true;
        } // Fall through to expand packed bits
        [[fallthrough]];
      // 2 and 4 bits-per-CHANNEL need expanding
      case 2: case 4: png_set_expand_gray_1_2_4_to_8(psData); [[fallthrough]];
      // Make sure bits are expanded
      case 8: png_set_packing(psData); break;
      // 16bpc needs compressing to 8bpc
      case 16: png_set_strip_16(psData); break;
      // Unsupported bpc
      default: XC("Unsupported bits-per-channel!", "Bits", uiBPC);
    } // Number of palette entries and the palette memory
    Memory mPalette;
    // Compare colour type
    switch(const unsigned int uiCT = png_get_color_type(psData, piData))
    { // 8-bits per channel (Palleted texture)
      case PNG_COLOR_TYPE_PALETTE:
        // If conversion to RGBA or RGB is required?
        if(idData.IsConvertRGBA() || idData.IsConvertRGB())
        { // Convert entire bitmap to 24-bits per pixel RGB
          png_set_palette_to_rgb(psData);
          // Jump to PNG_COLOR_TYPE_RGB case label
          goto RGB;
        } // If we're making sure this texture loads in OpenGL?
        else if(idData.IsConvertGPUCompat())
        { // Convert entire bitmap to 24-bits per pixel RGB
          png_set_palette_to_rgb(psData);
          // Image should be loadable in OpenGL now
          idData.SetActiveGPUCompat();
          // Jump to PNG_COLOR_TYPE_RGB case label
          goto RGB;
        } // We need this else so 'goto' works with constructing 'saHist' var.
        else
        { // Set that we're uploading a palette
          idData.SetPalette();
          // Read palette and show error if failed
          if(!png_get_PLTE(psData, piData, &palData, &iPalette))
            XC("Failed to read palette!");
          // Quantise to RGB palette if needed
          png_uint_16p saHist = nullptr;
          if(png_get_hIST(psData, piData, &saHist))
            png_set_quantize(psData, palData, iPalette, 256, saHist, 0);
          // Initialise palette data
          mPalette.MemInitData(static_cast<size_t>(iPalette)*BY_RGB,
                            static_cast<void*>(palData));
        } // Done
        break;
      // 8-bits per channel (Luminance / Gray)?
      case PNG_COLOR_TYPE_GRAY:
      // 16-bits per channel (Luminance / Gray + Alpha)?
      case PNG_COLOR_TYPE_GRAY_ALPHA:
        // If conversion to RGBA or RGB is required?
        if(idData.IsConvertRGBA() || idData.IsConvertRGB())
        { // Convert entire bitmap to 24-bits per pixel RGB
          png_set_palette_to_rgb(psData);
          // Jump to PNG_COLOR_TYPE_RGB case label
          goto RGB;
        } // Expand tRNS to alpha if set
        if(bAlpha) png_set_tRNS_to_alpha(psData);
        // Done
        break;
      // 24-bits per channel (RGB)?
      case PNG_COLOR_TYPE_RGB: RGB:;
        // If convert to RGBA requested and no alpha set?
        if(idData.IsConvertRGBA() && !bAlpha)
        { // Add an alpha channel if no alpha
          png_set_add_alpha(psData, 0, PNG_FILLER_AFTER);
          // We converted to RGBA here
          idData.SetActiveRGBA();
        } // If alpha is set in image? expand tRNS to alpha
        else if(bAlpha) png_set_tRNS_to_alpha(psData);
        // Done
        break;
      // 32-bits per channel (RGB + Alpha)?
      case PNG_COLOR_TYPE_RGBA:
        // If alpha set in image?
        if(bAlpha)
        { // If convert to RGB requested?
          if(idData.IsConvertRGB())
          { // Strip the alpha channel
            png_set_strip_alpha(psData);
            // We converted to RGBA here
            idData.SetActiveRGB();
          } // Expand tRNS to alpha
          else png_set_tRNS_to_alpha(psData);
        } // Done
        break;
      // Unsupported colour type
      default: XC("Unsupported colour type!", "Type", uiCT);
    } // Prevents warning (https://sourceforge.net/p/libpng/bugs/165/)
    png_set_interlace_handling(psData);
    // Transfomrations complete so update
    png_read_update_info(psData, piData);
    // All pixels should be 8bpc so set bytes and bits per pixel
    idData.SetBytesAndBitsPerPixelCast(png_get_channels(psData, piData));
    switch(idData.GetBitsPerPixel())
    { // 32-bpp
      case BD_RGBA: idData.SetPixelType(TT_RGBA); break;
      // 24-bpp
      case BD_RGB: idData.SetPixelType(TT_RGB); break;
      // 16-bpp (Gray + Alpha)
      case BD_GRAYALPHA: idData.SetPixelType(TT_GRAYALPHA); break;
      // 8-bits-per-pixel (Gray)
      case BD_GRAY: [[fallthrough]];
      // 1-bits-per-pixel (Monochrome
      case BD_BINARY: idData.SetPixelType(TT_GRAY); break;
      // Unsupported bit-depth
      default: XC("Unsupported bit-depth!",
                  "BitsPerPixel", idData.GetBitsPerPixel());
    } // Get image dimensions
    idData.DimSet(png_get_image_width(psData, piData),
                  png_get_image_height(psData, piData));
    // Initialise memory
    Memory mPixels{ idData.TotalPixels() * idData.GetBytesPerPixel() };
    // Create vector array to hold scanline pointers and size it
    PngPtrVec ppvList{ idData.DimGetHeight<size_t>() };
    // For each scanline
    // Set the pointer to the data pointer + i times the uiRow stride.
    // Notice that the uiRow order is reversed with q.
    // This is how at least OpenGL expects it,
    // and how many other image loaders present the data.
    for(size_t stHeight = idData.DimGetHeight<size_t>(),
               stHeightM1 = stHeight - 1,
               stStride = idData.DimGetWidth<size_t>() *
                          idData.GetBytesPerPixel(),
               stRow = 0;
               stRow < stHeight;
             ++stRow)
      ppvList[stRow] =
        mPixels.MemRead<png_byte>((stHeightM1 - stRow) * stStride, stStride);
    // Read image
    png_read_image(psData, ppvList.data());
    png_read_end(psData, piData);
    // Success, add the image data to list
    idData.AddSlot(mPixels);
    // Add palette data if it is there
    if(iPalette)
      idData.AddSlot(mPalette, static_cast<unsigned int>(iPalette), BY_RGB);
    // We are done!
    return true;
  }
  /* -- Constructor -------------------------------------------------------- */
  CodecPNG(void) :
    /* -- Initialisers ----------------------------------------------------- */
    ImageLib{ IFMT_PNG, "Portable Network Graphics", "PNG", Load, Save }
    /* -- No code ---------------------------------------------------------- */
    { }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(CodecPNG)            // Suppress default functions for safety
};/* -- End ---------------------------------------------------------------- */
/* ========================================================================= **
** ######################################################################### **
** ## JPEG format                                                    JPEG ## **
** ######################################################################### **
** -- JPEG Codec Object ---------------------------------------------------- */
class CodecJPG final :                 // Members initially private
  /* -- Base classes ------------------------------------------------------- */
  private ImageLib                     // Image format helper class
{ /* -- JPEG callbacks ----------------------------------------------------- */
  static void JPegInitSource(j_decompress_ptr) { }
  static void JPegTermSource(j_decompress_ptr) { }
  static boolean JPegFillInputBuffer(j_decompress_ptr)
    { return static_cast<boolean>(true); }
  static void JPegSkipInputData(j_decompress_ptr ciData, long lBytes)
  { // This function is called when jpeg needs to skip data, such as ICC
    // profiles. If we have no bytes then we're done
    if(lBytes <= 0) return;
    // Get jpeg data
    struct jpeg_source_mgr &jsmData = *ciData->src;
    // Since we're loading from memory, we can just move the pointers around.
    jsmData.next_input_byte += static_cast<size_t>(lBytes);
    jsmData.bytes_in_buffer -= static_cast<size_t>(lBytes);
  }
  static void JPegErrorExit[[noreturn]](j_common_ptr ciData)
  { // Buffer for error message
    string strMsg(JMSG_LENGTH_MAX, 0);
    // Popular error message
    (*(ciData->err->format_message))(ciData,const_cast<char*>(strMsg.c_str()));
    // Throw back to intermediate handler so we can cleanup libjpeg
    throw runtime_error{ strMsg.c_str() };
  }
  /* --------------------------------------------------------------- */ public:
  static bool Save(const FStream &fmData, const ImageData &idData,
                   const ImageSlot &isData)
  { // Only support 24-bit per pixel images
    if(idData.GetBitsPerPixel() != BD_RGB)
      XC("Only RGB supported!", "BitsPerPixel", idData.GetBitsPerPixel());
    // Check for valid dimensions
    if(isData.DimIsNotSet())
      XC("Dimensions are invalid!",
         "Width",  isData.DimGetWidth(), "Height", isData.DimGetHeight());
    // Have image data?
    if(isData.MemIsEmpty()) XC("No image data!", "Size", isData.MemSize());
    // Setup class to free pointer on exit
    struct JpegWriter
    { // Private variables
      struct jpeg_compress_struct ciData; // Compress struct data
      struct jpeg_error_mgr      jemData; // Error manager data
      // Constructor
      explicit JpegWriter(const FStream &fsC)
      {// Setup error manager
        ciData.err = jpeg_std_error(&jemData);
        jemData.error_exit = JPegErrorExit;
        // Setup decompressor
        jpeg_CreateCompress(&ciData, JPEG_LIB_VERSION,
          sizeof(struct jpeg_compress_struct));
        // first time for this JPEG object?
        jpeg_stdio_dest(&ciData, fsC.FStreamGetCtx());
      } // Destructor that cleans up libjpeg
      ~JpegWriter(void) { jpeg_destroy_compress(&ciData); }
    } // Send file map to class
    jwC{ fmData };
    // Get compress struct data
    struct jpeg_compress_struct &ciData = jwC.ciData;
    // Set image information
    ciData.image_width = isData.DimGetWidth();
    ciData.image_height = isData.DimGetHeight();
    ciData.input_components = idData.GetBitsPerPixel() / 8;
    ciData.in_color_space = JCS_RGB;
    // Call the setup defualts helper function to give us a starting point.
    // Note, don't call any of the helper functions before you call this, they
    // will have no effect if you do. Then call other helper functions, here I
    // call the set quality. Then start the compression.
    jpeg_set_defaults(&ciData);
    // Set the quality
    jpeg_set_quality(&ciData, 100, static_cast<boolean>(true));
    // We now start compressing
    jpeg_start_compress(&ciData, static_cast<boolean>(true));
    // Now we encode each can line
    while(ciData.next_scanline < ciData.image_height)
    { // Get scanline
      JSAMPROW rPtr = reinterpret_cast<JSAMPROW>
        (isData.MemRead((ciData.image_height-1-ciData.next_scanline)*
          static_cast<unsigned int>(ciData.input_components)*
          ciData.image_width));
      // Write scanline to disk
      jpeg_write_scanlines(&ciData, &rPtr, 1);
    } // Finished compressing
    jpeg_finish_compress(&ciData);
    // Success
    return true;
  }
  /* ----------------------------------------------------------------------- */
  static bool Load(FileMap &fmData, ImageData &idData)
  { // Make sure we have at least 12 bytes and the correct first 2 bytes
    if(fmData.MemSize() < 12 ||
       fmData.FileMapReadVar16LE() != 0xD8FF) return false;
    // We need access to this from the exception block
    struct JpegReader
    { // Private variables
      struct jpeg_decompress_struct ciData; // Decompress struct data
      struct jpeg_error_mgr        jemData; // Error manager data
      // Constructor
      JpegReader(void)
      { // Setup error manager
        ciData.err = jpeg_std_error(&jemData);
        jemData.error_exit = JPegErrorExit;
        // Setup decompressor
        jpeg_CreateDecompress(&ciData, JPEG_LIB_VERSION,
          sizeof(struct jpeg_decompress_struct));
      } // Destructor that cleans up libjpeg
      ~JpegReader(void) { jpeg_destroy_decompress(&ciData); }
    } // Send file map to class
    jrC;
    // Get compress struct data
    struct jpeg_decompress_struct &ciData = jrC.ciData;
    // Allocate memory manager object with a pointer to our file class
    ciData.src = reinterpret_cast<jpeg_source_mgr *>
      ((*ciData.mem->alloc_small) (reinterpret_cast<j_common_ptr>(&ciData),
        JPOOL_PERMANENT, sizeof(jpeg_source_mgr)));
    // Setup source manager
    jpeg_source_mgr &jsmData =
      *reinterpret_cast<struct jpeg_source_mgr *>(ciData.src);
    jsmData.init_source = JPegInitSource;
    jsmData.fill_input_buffer = JPegFillInputBuffer;
    jsmData.skip_input_data = JPegSkipInputData;
    jsmData.resync_to_restart = jpeg_resync_to_restart;
    jsmData.term_source = JPegTermSource;
    jsmData.next_input_byte = fmData.MemPtr<JOCTET>();
    jsmData.bytes_in_buffer = fmData.MemSize();
    // Read the header
    switch(jpeg_read_header(&ciData, static_cast<boolean>(true)))
    { // if SOS marker is reached?
      case JPEG_HEADER_OK: break;
      // For an abbreviated input image, if EOI is reached?
      case JPEG_HEADER_TABLES_ONLY: XC("Jpeg unexpected end of image!");
      // If data source module requests suspension of the decompressor?
      case JPEG_SUSPENDED: XC("Jpeg suspension requested!");
    } // Start decompressing before we can read main image information
    if(!jpeg_start_decompress(&ciData))
      XC("Jpeg start decompression failed!");
    // Make sure component count is valid
    idData.SetPixelType(ImageBYtoTexType(
      static_cast<ByteDepth>(ciData.output_components)));
    if(idData.GetPixelType() == TT_NONE)
      XC("Component count unsupported!",
         "OutputComponents", ciData.output_components);
    // Set dimensions and pixel bit depth
    idData.DimSet(ciData.output_width, ciData.output_height);
    idData.SetBytesAndBitsPerPixelCast(ciData.output_components);
    // Jpegs are reversed
    idData.SetReversed();
    // Create space for decompressed image
    Memory mPixels{ idData.TotalPixels() * idData.GetBytesPerPixel() };
    // Get row stride (or number of bytes in a scanline of image data)
    const size_t stRowStride =
      idData.DimGetWidth() * idData.GetBytesPerPixel();
    // Decompress scanlines
    while(ciData.output_scanline < ciData.output_height)
    { // For storing info
      array<unsigned char*,1> caPtr{ mPixels.MemRead<unsigned char>(
        static_cast<size_t>(ciData.output_scanline) * stRowStride, stRowStride)
      }; // Decompress the data
      jpeg_read_scanlines(&ciData, reinterpret_cast<JSAMPARRAY>(caPtr.data()),
        static_cast<JDIMENSION>(caPtr.size()));
    } // Add data to image list
    idData.AddSlot(mPixels);
    // Success
    return true;
  }
  /* -- Constructor -------------------------------------------------------- */
  CodecJPG(void) :
    /* -- Initialisers ----------------------------------------------------- */
    ImageLib{ IFMT_JPG, "Joint Photographic Experts Group", "JPG", Load, Save }
    /* -- No code ---------------------------------------------------------- */
    { }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(CodecJPG)            // Suppress default functions for safety
};/* ----------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
