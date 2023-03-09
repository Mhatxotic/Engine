/* == IMAGEGIF.HPP ========================================================= **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## Handles loading and saving of .GIF files with the ImageLib system.  ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace ICodecGIF {                  // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IError::P;             using namespace IFileMap::P;
using namespace IImageDef::P;          using namespace IImageLib::P;
using namespace IMemory::P;            using namespace IStd::P;
using namespace ITexDef::P;            using namespace Lib::NSGif;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* ------------------------------------------------------------------------- */
static class CodecGIF final :          // GIF codec object
  /* -- Base classes ------------------------------------------------------- */
  private ImageLib                     // Image format helper class
{ /* -- Allocate memory ---------------------------------------------------- */
  static void *GIFCreate(int iW, int iH)
    { return StdAlloc<void>(static_cast<unsigned int>(iW * iH) * BY_RGBA); }
  /* -- Free memory -------------------------------------------------------- */
  static void GIFDestroy(void*const vpBuffer)
    { if(!vpBuffer) throw runtime_error{ "Free buffer null pointer" };
      StdFree(vpBuffer); }
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
  bool Decode(FileMap &fmData, ImageData &idData)
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
          if(nsgif_get_frame_info(nsgCtx, uiFrame))
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
    catch(const exception&)
    { // Clean up
      nsgif_destroy(nsgCtx);
      // Throw back to proper exception handler if message sent
      throw;
    } // Never gets here
  }
  /* -- Constructor -------------------------------------------------------- */
  CodecGIF(void) :
    /* -- Initialisers ----------------------------------------------------- */
    ImageLib{ IFMT_GIF, "Graphics Interchange Format", "GIF",
      bind(&CodecGIF::Decode, this, _1, _2) }
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- End ---------------------------------------------------------------- */
} *cCodecGIF = nullptr;                // Codec pointer
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
