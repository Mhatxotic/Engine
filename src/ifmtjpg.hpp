/* == IMAGEJPG.HPP ========================================================= **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## Handles loading and saving of .JPG files with the ImageLib system.  ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace ICodecJPG {                  // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IError::P;             using namespace IFileMap::P;
using namespace IFStream::P;           using namespace IImageDef::P;
using namespace IImageLib::P;          using namespace IMemory::P;
using namespace ITexDef::P;            using namespace Lib::OS::JpegTurbo;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* ------------------------------------------------------------------------- */
class CodecJPG;                        // Class prototype
static CodecJPG *cCodecJPG = nullptr;  // Pointer to global class
class CodecJPG :                       // JPEG codec object
  /* -- Base classes ------------------------------------------------------- */
  private ImageLib                     // Image format helper class
{ /* -- JPEG callbacks ----------------------------------------------------- */
  static void JPegInitSource(j_decompress_ptr) {}
  static void JPegTermSource(j_decompress_ptr) {}
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
    (*(ciData->err->format_message))(ciData,const_cast<char*>(strMsg.data()));
    // Throw back to intermediate handler so we can cleanup libjpeg
    throw runtime_error{ strMsg.data() };
  }
  /* --------------------------------------------------------------- */ public:
  bool Encode(const FStream &fmData, const ImageData &idData,
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
      ~JpegWriter() { jpeg_destroy_compress(&ciData); }
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
  bool Decode(FileMap &fmData, ImageData &idData)
  { // Make sure we have at least 12 bytes and the correct first 2 bytes
    if(fmData.MemSize() < 12 ||
       fmData.FileMapReadVar16LE() != 0xD8FF) return false;
    // We need access to this from the exception block
    struct JpegReader
    { // Private variables
      struct jpeg_decompress_struct ciData; // Decompress struct data
      struct jpeg_error_mgr        jemData; // Error manager data
      // Constructor
      JpegReader()
      { // Setup error manager
        ciData.err = jpeg_std_error(&jemData);
        jemData.error_exit = JPegErrorExit;
        // Setup decompressor
        jpeg_CreateDecompress(&ciData, JPEG_LIB_VERSION,
          sizeof(struct jpeg_decompress_struct));
      } // Destructor that cleans up libjpeg
      ~JpegReader() { jpeg_destroy_decompress(&ciData); }
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
    switch(const int iResult =
      jpeg_read_header(&ciData, static_cast<boolean>(true)))
    { // if SOS marker is reached?
      case JPEG_HEADER_OK: break;
      // For an abbreviated input image, if EOI is reached?
      case JPEG_HEADER_TABLES_ONLY: XC("Jpeg unexpected end of image!");
      // If data source module requests suspension of the decompressor?
      case JPEG_SUSPENDED: XC("Jpeg suspension requested!");
      // Unknown result
      default: XC("Internal error: Jpeg read header unknown result!",
        "Code", iResult);
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
  /* -- Default constructor ------------------------------------- */ protected:
  CodecJPG() :
    /* -- Initialisers ----------------------------------------------------- */
    ImageLib{ IFMT_JPG, "Joint Photographic Experts Group", "JPG",
      bind(&CodecJPG::Decode, this, _1, _2),
      bind(&CodecJPG::Encode, this, _1, _2, _3) }
    /* -- Set global pointer to static class ------------------------------- */
    { cCodecJPG = this; }
};/* ----------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
