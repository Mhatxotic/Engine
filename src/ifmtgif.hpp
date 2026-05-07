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
class CodecGIF;                        // Class prototype
static CodecGIF *cCodecGIF = nullptr;  // Pointer to global class
class CodecGIF :                       // GIF codec object
  /* -- Base classes ------------------------------------------------------- */
  private ImageLib                     // Image format helper class
{ /* -- Allocate memory ---------------------------------------------------- */
  static void *GIFCreate(int iW, int iH)
    { return StdAlloc<void>(static_cast<unsigned>(iW * iH) * BY_RGBA); }
  /* -- Free memory -------------------------------------------------------- */
  static void GIFDestroy(void*const vpBuffer)
    { if(!vpBuffer) throw StdRunTimeError{ "Free buffer null pointer" };
      StdFree(vpBuffer); }
  /* -- Return ptr to pixel data in image ---------------------------------- */
  static unsigned char *GIFRead(void*const vpBuffer)
    { if(!vpBuffer) throw StdRunTimeError{ "Get buffer null pointer" };
      return reinterpret_cast<unsigned char*>(vpBuffer); }
  /* -- Sets whether a image should opaque --------------------------------- */
  static void GIFOpaque(void*const vpBuffer, bool)
    { if(!vpBuffer) throw StdRunTimeError{ "Set opaque null pointer" }; }
  /* -- If image has an opaque alpha channel ------------------------------- */
  static bool GIFIsOpaque(void*const vpBuffer)
    { if(!vpBuffer) throw StdRunTimeError{ "Test opaque null pointer" };
      return true; }
  /* -- Image changed, flush cache ----------------------------------------- */
  static void GIFFlush(void*const vpBuffer)
    { if(!vpBuffer) throw StdRunTimeError{ "Modified data null pointer" }; }
  /* -- Error checker with custom error details ---------------------------- */
  template<typename ...VarArgs>
    static void GIFError(const nsgif_error nsgErr, const char*const cpMessage,
      VarArgs &&...vaArgs)
  { if(nsgErr != NSGIF_OK)
      XC(cpMessage, "Reason", nsgif_strerror(nsgErr), "Code", nsgErr,
        StdForward<VarArgs>(vaArgs)...); }
  /* --------------------------------------------------------------- */ public:
  bool Decode(FileMap &fmData, ImageData &idData)
  { // Must have at least 10 bytes for DWORD header 'GIF8'.
    if(fmData.MemSize() < 10 || fmData.FileMapReadVar32LE() != 0x38464947)
      return false;
    // Next WORD must be '7a' or '9a' else it is not a gif file.
    switch(fmData.FileMapReadVar16LE())
      { case 0x6137: case 0x6139: break; default: return false; }
    // Read and check last 2 footer bytes of file
    if(fmData.FileMapReadVar16LEFrom(
         fmData.MemSize() - sizeof(uint16_t)) != 0x3b00)
      return false;
    // Set gif callbacks
    nsgif_bitmap_cb_vt sBMPCB{ GIFCreate, GIFDestroy, GIFRead, GIFOpaque,
      GIFIsOpaque, GIFFlush, nullptr };
    // create our gif animation
    nsgif_t *nsgCtx;
    GIFError(nsgif_create(&sBMPCB, NSGIF_BITMAP_FMT_R8G8B8A8, &nsgCtx),
      "Failed to prepare context!");
    // Trap exceptions so we can clean up
    try
    { // Decode the gif and report error if failed
      GIFError(nsgif_data_scan(nsgCtx, fmData.MemSize(),
        fmData.MemPtr<unsigned char>()), "Failed to parse file!");
      // The data is complete
      nsgif_data_complete(nsgCtx);
      // Get information about gif and if we got it?
     	if(const nsgif_info_t*const nsgInfo = nsgif_get_info(nsgCtx))
      { // Reserve memory for frames
        idData.ReserveSlots(nsgInfo->frame_count);
        // Set image properties
        idData.SetBitsAndBytesPerPixel(BD_RGBA);
        idData.SetPixelType(TT_RGBA);
        idData.SetReversed();
        idData.DimSet(nsgInfo->width, nsgInfo->height);
        // Decode the frames
        for(unsigned uIndex = 0; uIndex < nsgInfo->frame_count; ++uIndex)
        { // Area and frame data
          nsgif_rect_t nsgRect;
          uint32_t uFrame, uDelay;
          // Prepare frame
          GIFError(nsgif_frame_prepare(nsgCtx, &nsgRect, &uDelay, &uFrame),
            "Failed to prepare frame!");
          // Get frame info
          if(nsgif_get_frame_info(nsgCtx, uFrame))
          { // Decode the frame and throw exception if failed
            nsgif_bitmap_t *nsgBitmap;
            GIFError(nsgif_frame_decode(nsgCtx, uFrame, &nsgBitmap),
              "Failed to decode frame!", "Frame", uFrame);
            // Calculate image size
            const size_t stSize =
              nsgInfo->width * nsgInfo->height * idData.GetBytesPerPixel();
            // Now we can just copy the memory over
            Memory mData{ stSize, nsgBitmap };
            idData.AddSlot(mData, nsgInfo->width, nsgInfo->height);
          } // Failed to get frame data
          else XC("Failed to get frame data!");
        } // Frame decoding enumeration
      } // Failed to get gif information
      else XC("Failed to read metadata!");
      // Destroy gif and return success
      nsgif_destroy(nsgCtx);
      return true;
    } // Caught an exception (unlikely)
    catch(const StdException &)
    { // Clean up and throw back to proper exception handler if message sent
      nsgif_destroy(nsgCtx);
      throw;
    } // Never gets here
  }
  /* -- Default constructor ------------------------------------- */ protected:
  CodecGIF() :
    /* -- Initialisers ----------------------------------------------------- */
    ImageLib{ IFMT_GIF, "Graphics Interchange Format", "GIF",
      bind(&CodecGIF::Decode, this, _1, _2) }
    /* -- Set global pointer to static class ------------------------------- */
    { cCodecGIF = this; }
};/* ----------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
