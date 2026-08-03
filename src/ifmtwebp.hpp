/* == IFMTWEBP.HPP ========================================================= **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## Handles loading of .WEBP files with the ImageLib system.            ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace ICodecWEBP {                 // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IError::P;             using namespace IFileMap::P;
using namespace IImageData::P;         using namespace IImageDef::P;
using namespace IImageLib::P;          using namespace IMemory::P;
using namespace IStd::P;               using namespace ITexDef::P;
using namespace Lib::WebP;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* ------------------------------------------------------------------------- */
class CodecWEBP;                       // Class prototype
static CodecWEBP *cCodecWEBP = nullptr; // Pointer to global class
class CodecWEBP :                      // WEBP codec object
  /* -- Base classes ------------------------------------------------------- */
  protected ImageLib                   // Image format helper class
{ /* ----------------------------------------------------------------------- */
  static void *WebPAlloc(void*const, size_t stSize)
    { return StdAlloc<void>(stSize); }
  static void WebPFree(void*const, void*const vpAddr)
    { StdFree<void>(vpAddr); }
  /* ----------------------------------------------------------------------- */
  bool Decode(FileMap &fmData, ImageData &idData)
  { // Check first 4 bytes of magic (RIFF)
    if(fmData.MemSize() < 4 || fmData.FileMapReadVar32LE() != 0x46464952)
      return false;
    // Initialise allocator
    simplewebp_allocator swaFuncs{ WebPAlloc, WebPFree, this };
    // Initialize input handle from the memory buffer
    simplewebp *swpData;
    if(const simplewebp_error switErr = simplewebp_load_from_memory(
         fmData.FileMapReadPtrFrom(0), fmData.MemSize(), &swaFuncs,
           &swpData))
      XC("Failed to load WebP from memory!", "Code", switErr);
    // So we can clean up
    try
    { // Get and set dimensions
      size_t stWidth = 0, stHeight = 0;
      simplewebp_get_dimensions(swpData, &stWidth, &stHeight);
      idData.DimSet(stWidth, stHeight);
      // Decode the stream and clear it after
      Memory mPixels{ stWidth * stHeight * BY_RGBA };
      if(const simplewebp_error switErr2 =
           simplewebp_decode(swpData, mPixels.MemPtr(), nullptr))
        XC("Failed to decode WebP image!", "Code", switErr2);
      simplewebp_unload(swpData);
      // Success, add the image data to list and set RGBA and reversed
      idData.AddSlot(mPixels);
      idData.SetBitsPerPixel(BD_RGBA);
      idData.SetBytesPerPixel(BY_RGBA);
      idData.SetPixelType(TT_RGBA);
      idData.SetReversed();
    } // Failure so clear input stream and rethrow
    catch(...) { simplewebp_unload(swpData); throw; }
    // We are done!
    return true;
  }
  /* -- Default constructor ------------------------------------- */ protected:
  CodecWEBP() :
    /* -- Initialisers ----------------------------------------------------- */
    ImageLib{ IFMT_WEBP, "Web Picture", "WEBP",
      bind(&CodecWEBP::Decode, this, _1, _2) }
    /* -- Set global pointer to static class ------------------------------- */
    { cCodecWEBP = this; }
};/* ----------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
