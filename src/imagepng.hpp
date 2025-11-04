/* == IMAGEPNG.HPP ========================================================= **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## Handles loading and saving of .PNG files with the ImageLib system.  ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace ICodecPNG {                  // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IClock::P;             using namespace IDim::P;
using namespace IError::P;             using namespace IFileMap::P;
using namespace IFStream::P;           using namespace IImageDef::P;
using namespace IImageLib::P;          using namespace ILog::P;
using namespace IMemory::P;            using namespace IStd::P;
using namespace IString::P;            using namespace ITexDef::P;
using namespace IUtf::P;               using namespace IUtil::P;
using namespace Lib::Png;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* ------------------------------------------------------------------------- */
class CodecPNG;                        // Class prototype
static CodecPNG *cCodecPNG = nullptr;  // Pointer to global class
class CodecPNG :                       // PNG codec object
  /* -- Base classes ------------------------------------------------------- */
  protected ImageLib                   // Image format helper class
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
      stRead, stC).data());
  }
  /* -- Save png file ---------------------------------------------- */ public:
  bool Encode(const FStream &fmData, const ImageData &idData,
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
      void Header(const DimUInt &duDim, const int iBitDepth,
        const int iColourType, const int iInterlaceType,
        const int iCompressionType, const int iFilterType)
      { // Set format of our data and how we want the data stored
        png_set_IHDR(psData, piData, duDim.DimGetWidth(), duDim.DimGetHeight(),
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
        { Meta(cpK, strV.data(), strV.length()); }
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
      ~PngWriter() { png_destroy_write_struct(&psData, &piData); }
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
      pwC.Meta("Software", StrFormat("$ ($) v$.$.$.$ ($-bit $) by $",
        cSystem->ENGName(), cSystem->ENGBuildType(), cSystem->ENGMajor(),
        cSystem->ENGMinor(), cSystem->ENGBuild(), cSystem->ENGRevision(),
        cSystem->ENGBits(), cSystem->ENGTarget(), cSystem->ENGAuthor()));
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
  bool Decode(FileMap &fmData, ImageData &idData)
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
      ~PngReader() { png_destroy_read_struct(&psData, &piData, nullptr); }
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
  /* -- Default constructor ------------------------------------- */ protected:
  CodecPNG() :
    /* -- Initialisers ----------------------------------------------------- */
    ImageLib{ IFMT_PNG, "Portable Network Graphics", "PNG",
      bind(&CodecPNG::Decode, this, _1, _2),
      bind(&CodecPNG::Encode, this, _1, _2, _3) }
    /* -- Set global pointer to static class ------------------------------- */
    { cCodecPNG = this; }
};/* ----------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
