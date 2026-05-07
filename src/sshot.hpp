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
using namespace IClock::P;             using namespace ICommon::P;
using namespace ICollector::P;         using namespace ICVarDef::P;
using namespace IFbo::P;               using namespace IFboCore::P;
using namespace IImage::P;             using namespace IImageDef::P;
using namespace IImageLib::P;          using namespace ILog::P;
using namespace ILuaIdent::P;          using namespace ILuaLib::P;
using namespace IMemory::P;            using namespace IName::P;
using namespace IOgl::P;               using namespace IStd::P;
using namespace IString::P;            using namespace ISystem::P;
using namespace ISysUtil::P;           using namespace ITexDef::P;
using namespace IThread::P;            using namespace Lib::OS::GlFW::Types;
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
  ThreadStatus DumpThread(const Thread &)
  { // Code to return
    ThreadStatus tsReturn;
    // Capture exceptions
    try
    { // Reverse pixels or they will be upside down
      ReversePixels();
      // Save the image to disk
      SaveFile(NameGet(), 0, ifFormatId);
      // Success
      tsReturn = TS_OK;
    } // exception occured?
    catch(const StdException &eReason)
    { // Report error
      cLog->LogErrorExSafe("(SCREENSHOT WRITER THREAD EXCEPTION) $", eReason);
      // Errored return code
      tsReturn = TS_ERROR;
    } // Free the memory created with the bitmap
    ResetAllData();
    // Return the code specified
    return tsReturn;
  }
  /* -- Capture screenshot from Fbo -------------------------------- */ public:
  bool DumpFbo(const Fbo &fboRef,
    const StdStringView &strvFile = cCommon->CommonBlankV())
  { // Cancel if thread is still running
    if(tThread.ThreadIsJoinable()) return false;
    // DeInit old thread, we need to reuse it
    tThread.ThreadDeInit();
    // Log procedure
    cLog->LogDebugExSafe(
      "SShot '$' grabbing back buffer to write to screenshot...",
      fboRef.NameGet());
    // Allocate storage (Writing as RGB 24-bit).
    const BitDepth bdBPP = BD_RGB;
    const TextureType ttMode = TT_RGB;
    const size_t stBytesPerPixel = bdBPP / 8;
    Memory mBuffer{ fboRef.DimGetWidth<size_t>() *
      fboRef.DimGetHeight<size_t>() * stBytesPerPixel };
    // Bind the Fbo
    GL(cOgl->BindFbo(fboRef.gluFbo), "Failed to bind Fbo to dump!",
      "Name", fboRef.NameGet(), "Id", fboRef.gluFbo);
    // Bind the texture in the Fbo
    GL(cOgl->BindTexture(fboRef.gluFbotex),
      "Failed to bind Fbo texture to dump!",
      "Name", fboRef.NameGet(), "Id", fboRef.gluFbotex);
    // Read into buffer
    GL(cOgl->ReadTextureTT(ttMode, mBuffer.MemPtr<GLvoid>()),
      "Failed to read Fbo pixel data!",
      "Name", fboRef.NameGet(), "Mode", ImageGetPixelFormat(ttMode));
    // Get new filename or original filename
    NameSet(strvFile.empty() ? StrAppend(cSystem->SysGetGuestShortTitle(),
      cmSys.FormatTime("-%Y%m%d-%H%M%S")) : strvFile);
    // Log status
    cLog->LogDebugExSafe("SShot '$' screen capture to '$' ($x$x$;$)...",
      fboRef.NameGet(), NameGet(), fboRef.DimGetWidth(),
      fboRef.DimGetHeight(), bdBPP, ImageGetPixelFormat(ttMode));
    // Setup raw image
    InitRaw(NameGet(), mBuffer, fboRef.DimGetWidth<unsigned>(),
      fboRef.DimGetHeight<unsigned>(), bdBPP);
    // Launch thread to write the screenshot to disk in the background
    tThread.ThreadStart();
    // Success
    return true;
  }
  /* -- Dump main Fbo ------------------------------------------------------ */
  void DumpMain() { DumpFbo(cFboCore->FboCoreGetMain()); }
  /* -- Default constructor ------------------------------------------------ */
  SShot() :
    /* -- Initialisers ----------------------------------------------------- */
    ICHelperSShot{ cSShots },          // Initialise collector helper
    ifFormatId(cSShots->ifFormatId),   // Not truly initialised yet
    tThread{ "sshot", STP_LOW,         // Prepare low perf screenshot thread
      bind(&SShot::DumpThread,         // Dump thread entry function
        this, _1) }                    // Send this class pointer
    /* -- No code ---------------------------------------------------------- */
    {}
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
