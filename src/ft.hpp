/* == FT.HPP =============================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## This module interfaces engine with the freetype API                 ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IFreeType {                  // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IError::P;             using namespace ILog::P;
using namespace IMemory::P;            using namespace IMutex::P;
using namespace IStd::P;               using namespace ISysUtil::P;
using namespace IUtil::P;              using namespace Lib::FreeType;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* -- Freetype core class -------------------------------------------------- */
class FreeType;                        // Class prototype
static FreeType *cFreeType = nullptr;  // Pointer to global class
class FreeType :                       // Members initially private
  /* -- Base classes ------------------------------------------------------- */
  private MutexLock                    // Instance protection
{ /* -- Private variables -------------------------------------------------- */
  FT_Library    ftlContext;            // Freetype context
  FT_MemoryRec_ ftmrAlloc;             // Freetype custom allocator
  /* ----------------------------------------------------------------------- */
  bool DoDeInit()
  { // Return failed if library not available
    if(!IsLibraryAvailable()) return false;
    // Unload freetype library
    cLog->LogDebugSafe("FreeType subsystem deinitialising...");
    FT_Done_Library(ftlContext);
    cLog->LogDebugSafe("FreeType subsystem deinitialised.");
    // Success
    return true;
  }
  /* --------------------------------------------------------------- */ public:
  static FT_Error ApplyStrokerFull(FT_Glyph &ftgData, FT_Stroker ftsStroker)
    { return FT_Glyph_Stroke(&ftgData, ftsStroker, true); }
  /* ----------------------------------------------------------------------- */
  static FT_Error ApplyStrokerPartial(FT_Glyph &ftgData, FT_Stroker ftsStroker,
    const bool bInside)
      { return FT_Glyph_StrokeBorder(&ftgData, ftsStroker, bInside, true); }
  /* ----------------------------------------------------------------------- */
  static FT_Error ApplyStrokerOutside(FT_Glyph &ftgData, FT_Stroker ftsStroker)
    { return ApplyStrokerPartial(ftgData, ftsStroker, false); }
  /* ----------------------------------------------------------------------- */
  static FT_Error ApplyStrokerInside(FT_Glyph &ftgData, FT_Stroker ftsStroker)
    { return ApplyStrokerPartial(ftgData, ftsStroker, true); }
  /* ----------------------------------------------------------------------- */
  FT_Error NewStroker(FT_Stroker &ftsDst) const
    { return FT_Stroker_New(ftlContext, &ftsDst); }
  /* ----------------------------------------------------------------------- */
  bool IsLibraryAvailable() { return ftlContext != nullptr; }
  /* ----------------------------------------------------------------------- */
  FT_Error NewFont(const MemConst &mcSrc, FT_Face &ftfDst)
  { // Lock a mutex to protect FT_Library.
    // > freetype.org/freetype2/docs/reference/ft2-base_interface.html
    return MutexCall([this,&mcSrc,&ftfDst](){
      // Create the font, throw exception on error
      return FT_New_Memory_Face(ftlContext, mcSrc.MemPtr<FT_Byte>(),
        mcSrc.MemSize<FT_Long>(), 0, &ftfDst);
    });
  }
  /* ----------------------------------------------------------------------- */
  void DestroyFont(FT_Face ftfFace)
  { // Lock a mutex to protect FT_Library and destroy the font context.
    // > freetype.org/freetype2/docs/reference/ft2-base_interface.html
    MutexCall([ftfFace](){FT_Done_Face(ftfFace);});
  }
  /* -- Error checker with custom error details ---------------------------- */
  template<typename ...VarArgs>
    static void CheckError(const FT_Error ftErr,
      const char*cpMessage, VarArgs &&...vaArgs)
  { if(ftErr) XC(cpMessage, "Code", ftErr, "Reason", FT_Error_String(ftErr),
                StdForward<VarArgs>(vaArgs)...); }
  /* ----------------------------------------------------------------------- */
  void Init()
  { // Class initialised
    if(IsLibraryAvailable()) XC("Freetype already initialised!");
    // Log initialisation and do the init
    cLog->LogDebugSafe("FreeType subsystem initialising...");
    // Create the memory
    CheckError(FT_New_Library(&ftmrAlloc, &ftlContext),
      "Failed to initialise FreeType!");
    // Documentation says we must run this function (fails if not)
    // https://www.freetype.org/freetype2/docs/design/design-4.html
    FT_Add_Default_Modules(ftlContext);
    // Log successful initialisation
    cLog->LogDebugSafe("FreeType subsystem initialised.");
  }
  /* ----------------------------------------------------------------------- */
  void DeInit() { if(DoDeInit()) ftlContext = nullptr; }
  /* -- Destructor ---------------------------------------------- */ protected:
  DTORHELPER(~FreeType, DoDeInit())
  /* -- Default constructor ------------------------------------------------ */
  FreeType() : ftlContext(nullptr), ftmrAlloc{ this,
    [](FT_Memory, long lBytes)->void*
      { return StdAlloc<void>(lBytes); },
    [](FT_Memory, void*const vpAddress)
      { StdFree(vpAddress); },
    [](FT_Memory, long, long lBytes, void*const vpAddress)->void*
      { return StdReAlloc(vpAddress, lBytes); }
  }
  /* -- Set global pointer to static class --------------------------------- */
  { cFreeType = this; }
};/* ----------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
