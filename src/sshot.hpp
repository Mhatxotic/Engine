/* == SSHOT.HPP ============================================================ **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## Contains the class to help with creating screenshots                ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace ISShot {                     // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IClock::P;             using namespace ICollector::P;
using namespace ICVarDef::P;           using namespace IFbo::P;
using namespace IFboCore::P;           using namespace IIdent::P;
using namespace IImage::P;             using namespace IImageDef::P;
using namespace IImageFormat::P;       using namespace IImageLib::P;
using namespace ILog::P;               using namespace ILuaLib::P;
using namespace IMemory::P;            using namespace IOgl::P;
using namespace IStd::P;               using namespace IString::P;
using namespace ISystem::P;            using namespace ISysUtil::P;
using namespace ITexDef::P;            using namespace IThread::P;
using namespace Lib::OS::GlFW;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* -- Begin collector class ------------------------------------------------ */
CTOR_BEGIN(SShots, SShot, CLHelperUnsafe,
  /* -- Collector variables ------------------------------------------------ */
  ImageFormat          ifFormatId;     // Screenshot id to use
) /* -- Begin member class ------------------------------------------------- */
CTOR_MEM_BEGIN(SShots, SShot, ICHelperUnsafe, /* n/a */),
  /* -- Initialisers ------------------------------------------------------- */
  public Image                         // Image class to store screen raw data
{ /* -- Screenshot thread -------------------------------------------------- */
  ImageFormat          ifFormatId;     // Screenshot type to save as
  Thread               tThread;        // Thread processing the request
  /* -- Fbo dumper thread callback ----------------------------------------- */
  int DumpThread(const Thread &)
  { // Code to return
    int iReturn;
    // Capture exceptions
    try
    { // Reverse pixels or they will be upside down
      ReversePixels();
      // Save the image to disk
      SaveFile(IdentGet(), 0, ifFormatId);
      // Success
      iReturn = 1;
    } // exception occured?
    catch(const exception &E)
    { // Report error
      cLog->LogErrorExSafe("(SCREENSHOT WRITER THREAD EXCEPTION) $", E.what());
      // Errored return code
      iReturn = -1;
    } // Free the memory created with the bitmap
    ResetAllData();
    // Return the code specified
    return iReturn;
  }
  /* -- Capture screenshot from FBO -------------------------------- */ public:
  bool DumpFBO(const Fbo &fboRef, const string &strFile=cCommon->Blank())
  { // Cancel if thread is still running
    if(tThread.ThreadIsRunning()) return false;
    // DeInit old thread, we need to reuse it
    tThread.ThreadDeInit();
    // Log procedure
    cLog->LogDebugExSafe(
      "SShot '$' grabbing back buffer to write to screenshot...",
      fboRef.IdentGet());
    // Allocate storage (Writing as RGB 24-bit).
    const BitDepth bdBPP = BD_RGB;
    const TextureType ttMode = TT_RGB;
    const size_t stBytesPerPixel = bdBPP / 8;
    Memory mBuffer{ fboRef.DimGetWidth<size_t>() *
      fboRef.DimGetHeight<size_t>() * stBytesPerPixel };
    // Bind the fbo
    GL(cOgl->BindFBO(fboRef.uiFBO), "Failed to bind FBO to dump!",
      "Identifier", fboRef.IdentGet(), "Id", fboRef.uiFBO);
    // Bind the texture in the fbo
    GL(cOgl->BindTexture(fboRef.uiFBOtex),
      "Failed to bind FBO texture to dump!",
      "Identifier", fboRef.IdentGet(), "Id", fboRef.uiFBOtex);
    // Read into buffer
    GL(cOgl->ReadTextureTT(ttMode, mBuffer.MemPtr<GLvoid>()),
      "Failed to read FBO pixel data!",
      "Identifier", fboRef.IdentGet(), "Mode", ImageGetPixelFormat(ttMode));
    // Get new filename or original filename
    IdentSet(strFile.empty() ? StrAppend(cSystem->GetGuestShortTitle(),
      cmSys.FormatTime("-%Y%m%d-%H%M%S")) : strFile);
    // Log status
    cLog->LogDebugExSafe("SShot '$' screen capture to '$' ($x$x$;$)...",
      fboRef.IdentGet(), IdentGet(), fboRef.DimGetWidth(),
      fboRef.DimGetHeight(), bdBPP, ImageGetPixelFormat(ttMode));
    // Setup raw image
    InitRaw(IdentGet(), mBuffer, fboRef.DimGetWidth<unsigned int>(),
      fboRef.DimGetHeight<unsigned int>(), bdBPP);
    // Launch thread to write the screenshot to disk in the background
    tThread.ThreadStart();
    // Success
    return true;
  }
  /* -- Dump main fbo ------------------------------------------------------ */
  void DumpMain(void) { DumpFBO(cFboCore->fboMain); }
  /* -- Default constructor ------------------------------------------------ */
  SShot(void) :                        // No parameters
    /* -- Initialisers ----------------------------------------------------- */
    ICHelperSShot{ cSShots },          // Initialise collector helper
    ifFormatId(cSShots->ifFormatId),   // Not truly initialised yet
    tThread{ "sshot", STP_LOW,         // Prepare low perf screenshot thread
      bind(&SShot::DumpThread,         // Dump thread entry function
        this, _1) }                    // Send this class pointer
    /* -- No code ---------------------------------------------------------- */
    { }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(SShot)               // Suppress default functions for safety
};/* ----------------------------------------------------------------------- */
CTOR_END(SShots, SShot, SSHOT,,,, ifFormatId(IFMT_MAX)) // Initialised by cvars
/* -- Set screenshot format flags ------------------------------------------ */
static CVarReturn SShotsSetType(const ImageFormat ifNFormatId)
  { return CVarSimpleSetIntNGE(cSShots->ifFormatId, ifNFormatId, IFMT_MAX); }
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
