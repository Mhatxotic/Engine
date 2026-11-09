/* == FTF.HPP ============================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## This module defines a class that can load true-type fonts using     ## **
** ## freetype. As of writing, Freetype supports the following fonts.     ## **
** ##   OTF, CFF, OTC  OpenType and variants                              ## **
** ##   TTF, TTC       TrueType and variants                              ## **
** ##   WOFF           Open web font format                               ## **
** ##   PFA, PFB       Type 1                                             ## **
** ##   CID            Keyed Type 1                                       ## **
** ##   SFNT           Bitmap fonts including colour emoji                ## **
** ##   PCF            X11 Unix                                           ## **
** ##   FNT            Windows bitmap font                                ## **
** ##   BDF            Including anti-aliased                             ## **
** ##   PFR            BitStream TrueDoc                                  ## **
** ##   N/A            Type 42 PostScript fonts (limited support)         ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IFtf {                       // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IAsset::P;             using namespace IASync::P;
using namespace ICollector::P;         using namespace IDim::P;
using namespace IError::P;             using namespace IEvtMain::P;
using namespace IFileMap::P;           using namespace IFreeType::P;
using namespace ILog::P;               using namespace ILuaIdent::P;
using namespace ILuaLib::P;            using namespace ILuaUtil::P;
using namespace IMemory::P;            using namespace IName::P;
using namespace ISerial::P;            using namespace IStd::P;
using namespace ISysUtil::P;           using namespace IUtil::P;
using namespace Lib::GlFW::Types;      using namespace Lib::FreeType;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* == Ftf collector class for collector data and custom variables ========== */
CTOR_BEGIN_ASYNC_DUO(Ftfs, Ftf, CLHelperUnsafe, ICHelperUnsafe),
  /* -- Base classes ------------------------------------------------------- */
  public AsyncLoaderFtf,               // Asyncronous loading of data
  public DimGLFloat,                   // Requested font width/height
  public FileMap                       // FT ttf file data (persistant)
{ /* -- Private variables -------------------------------------------------- */
  GLfloat          fOutline;           // FT outline size
  FT_Face          ftfFace;            // FT Char handle
  FT_Stroker       ftsStroker;         // FT Outline handle
  /* --------------------------------------------------------------- */ public:
  DimUInt          duDPI;              // FT DPI width and height
  /* -------------------------------------------------------------- */ private:
  void FtfDoDeInit()
  { // Clear freetype handles if created
    if(FtfIsStrokerLoaded()) cFreeType->FreeTypeDestroyStroker(ftsStroker);
    if(FtfIsLoaded()) cFreeType->FreeTypeDestroyFont(ftfFace);
  }
  /* -- Clear handles ------------------------------------------------------ */
  void FtfDoClearHandles() { ftsStroker = nullptr; ftfFace = nullptr; }
  /* -- Set ftf size ------------------------------------------------------- */
  void FtfUpdateSize()
  { // For some twisted reason, FreeType measures char size in terms f 1/64ths
    // of pixels. Thus, to make a char 'h' pixels high, we need to request a
    // size of 'h*64'.
    cFreeType->FreeTypeCheckError(FT_Set_Char_Size(ftfFace,
      static_cast<FT_F26Dot6>(DimGetWidth() * 64.0f),
      static_cast<FT_F26Dot6>(DimGetHeight() * 64.0f),
      duDPI.DimGetWidth<FT_UInt>(), duDPI.DimGetHeight<FT_UInt>()),
      "Failed to set character size!",
      "Name",      NameGet(),      "Width",    DimGetWidth(),
      "Height",    DimGetHeight(), "DPIWidth", duDPI.DimGetWidth(),
      "DPIHeight", duDPI.DimGetHeight());
  }
  /* -- Check and initialise supplied variables ---------------------------- */
  void FtfInitVars(const GLfloat fWidth, const GLfloat fHeight,
    const unsigned uDpiWidth, const unsigned uDpiHeight,
    const GLfloat fNOutline)
  { // Set width and height
    DimSet(fWidth, fHeight);
    // Set DPI width and height
    duDPI.DimSet(uDpiWidth, uDpiHeight);
    // Set outline
    fOutline = fNOutline;
  }
  /* -- Returns if face is loaded----------------------------------- */ public:
  bool FtfIsLoaded() const { return ftfFace != nullptr; }
  bool FtfIsNotLoaded() const { return !FtfIsLoaded(); }
  FT_Stroker FtfGetStroker() const { return ftsStroker; }
  bool FtfIsStrokerLoaded() const { return FtfGetStroker() != nullptr; }
  unsigned FtfGetDPIWidth() const { return duDPI.DimGetWidth(); }
  unsigned FtfGetDPIHeight() const { return duDPI.DimGetHeight(); }
  GLfloat FtfGetOutline() const { return fOutline; }
  bool FtfIsOutline() const { return FtfGetOutline() > 0.0f; }
  FT_GlyphSlot FtfGetGlyphData() const { return ftfFace->glyph; }
  const FT_String *FtfGetFamily() const { return ftfFace->family_name; }
  const FT_String *FtfGetStyle() const { return ftfFace->style_name; }
  FT_Long FtfGetGlyphCount() const { return ftfFace->num_glyphs; }
  /* -- Convert character to glyph index ----------------------------------- */
  FT_UInt FtfCharToGlyph(const FT_ULong ftulChar)
    { return FT_Get_Char_Index(ftfFace, ftulChar); }
  /* -- Load a glyph ------------------------------------------------------- */
  FT_Error FtfLoadGlyph(const FT_UInt ftuIndex)
    { return FT_Load_Glyph(ftfFace, ftuIndex,
        FT_LOAD_CROP_BITMAP|FT_LOAD_NO_AUTOHINT|FT_LOAD_NO_HINTING); }
  /* -- Load ftf from memory ----------------------------------------------- */
  void AsyncReady(FileMap &fmData)
  { // Take ownership of the file data
    FileMapSwap(fmData);
    // Load font
    cFreeType->FreeTypeCheckError(
      cFreeType->FreeTypeNewFont(static_cast<FileMap&>(*this), ftfFace),
        "Failed to create font!",
        "Name",    NameGet(),
        "Context", cFreeType->FreeTypeIsLibraryAvailable(),
        "Buffer",  FileMap::MemIsPtrSet(),
        "Size",    FileMap::MemSize());
    // Update size
    FtfUpdateSize();
    // Outline requested?
    if(FtfIsOutline())
    { // Create stroker handle
      cFreeType->FreeTypeCheckError(cFreeType->FreeTypeNewStroker(ftsStroker),
        "Failed to create stroker!",
        "Name",    NameGet(),
        "Context", cFreeType->FreeTypeIsLibraryAvailable());
      // Set properties of stroker handle
      FT_Stroker_Set(ftsStroker,
        static_cast<FT_Fixed>(FtfGetOutline() * 64.0f),
        FT_STROKER_LINECAP_ROUND, FT_STROKER_LINEJOIN_ROUND, 0);
    } // Report loaded font
    cLog->LogInfoExSafe("Ftf loaded '$' (FF:$;FS:$;S:$$$x$;D:$x$;B:$).",
      NameGet(), FtfGetFamily(), FtfGetStyle(), StdIOSSetPrecision(0),
      StdIOSFixed, DimGetWidth(), DimGetHeight(), FtfGetDPIWidth(),
      FtfGetDPIHeight(), FtfGetOutline());
  }
  /* -- Load pcm from memory asynchronously -------------------------------- */
  void FtfInitAsyncArray(lua_State*const lS, const StdStringView &ssvFile,
    Asset &aRef, const float fWidth, const float fHeight,
    const unsigned uDpiWidth, const unsigned uDpiHeight,
    const GLfloat fNOutline)
  { // Set other members
    FtfInitVars(fWidth, fHeight, uDpiWidth, uDpiHeight, fNOutline);
    // Prepare asynchronous loading from array
    AsyncInitArray(lS, ssvFile, "ftfarray", aRef);
  }
  /* -- Load pcm from file asynchronously ---------------------------------- */
  void FtfInitAsyncFile(lua_State*const lS, const StdStringView &ssvFile,
    const float fWidth, const float fHeight, const unsigned uDpiWidth,
    const unsigned uDpiHeight, const GLfloat fNOutline)
  { // Set other members
    FtfInitVars(fWidth, fHeight, uDpiWidth, uDpiHeight, fNOutline);
    // Prepare asynchronous loading from array
    AsyncInitFile(lS, ssvFile, "ftffile");
  }
  /* -- Init from file ----------------------------------------------------- */
  void FtfInitFile(const StdStringView &ssvFile, const GLfloat fWidth,
    const GLfloat fHeight, const unsigned uDpiWidth, const unsigned uDpiHeight,
    const GLfloat fNOutline)
  { // Set other members
    FtfInitVars(fWidth, fHeight, uDpiWidth, uDpiHeight, fNOutline);
    // Load file normally
    SyncInitFileSafe(ssvFile);
  }
  /* -- Init from array ---------------------------------------------------- */
  void FtfInitArray(const StdStringView &ssvName, Memory &mData,
    const GLfloat fWidth, const GLfloat fHeight, const unsigned uDpiWidth,
    const unsigned uDpiHeight, const GLfloat fNOutline)
  { // Set other members
    FtfInitVars(fWidth, fHeight, uDpiWidth, uDpiHeight, fNOutline);
    // Load file as array
    SyncInitArray(ssvName, mData);
  }
  /* -- De-init ftf font --------------------------------------------------- */
  void FtfDeInit() { FtfDoDeInit(); FtfDoClearHandles(); }
  /* ----------------------------------------------------------------------- */
  void FtfSwap(Ftf &ftfOther)
  { // Move variables over from source class
    DimSwap(ftfOther);
    duDPI.DimSwap(ftfOther.duDPI);
    StdSwap(fOutline, ftfOther.fOutline);
    StdSwap(ftfFace, ftfOther.ftfFace);
    StdSwap(ftsStroker, ftfOther.ftsStroker);
    // Swap file class
    FileMapSwap(ftfOther);
    // Swap async, lua lock data and registration
    LockSwap(ftfOther);
    CollectorSwapRegistration(ftfOther);
  }
  /* -- MOVE assignment (Ftf=Ftf) just do a swap --------------------------- */
  Ftf& operator=(Ftf &&ftfOther) { FtfSwap(ftfOther); return *this; }
  /* -- MOVE constructor --------------------------------------------------- */
  Ftf(Ftf &&ftfOther) :                // The other Ftf class to swap with
    /* -- Initialisers ----------------------------------------------------- */
    Name{ StdMove(ftfOther) },         // Move name over
    ICHelperFtf{ StdMove(ftfOther) },  // Move registration
    SerialSlave{ ftfOther },           // Move identification number
    AsyncLoaderFtf{ this,              // Initialise async loader with class
      EMC_MP_FONT },                   // " and the event id
    DimGLFloat{ StdMove(ftfOther) },   // Move other font width/height
    FileMap{ StdMove(ftfOther) },      // Move other FT ttf file data
    fOutline(ftfOther.fOutline),       // Copy outline size over
    ftfFace(ftfOther.ftfFace),         // Copy freetype font face handle
    ftsStroker(ftfOther.ftsStroker),   // Copy freetype stroker handle
    duDPI{ StdMove(ftfOther.duDPI) }   // Move DPI values over
    /* -- Clear the other handles ------------------------------------------ */
    { ftfOther.FtfDoClearHandles(); }
  /* -- Default constructor ------------------------------------------------ */
  Ftf() :
    /* -- Initialisers ----------------------------------------------------- */
    ICHelperFtf{ cFtfs },              // Initially unregistered
    SerialSlave{ ctParent.Serial() },  // Initialise identification number
    AsyncLoaderFtf{ this,              // Initialise async loader with class
      EMC_MP_FONT },                   // " and the event id
    fOutline(0.0f),                    // No outline size yet
    ftfFace(nullptr),                  // No FreeType handle yet
    ftsStroker(nullptr)                // No FreeType stroker handle yet
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- Destructor --------------------------------------------------------- */
  DTORHELPER(~Ftf, AsyncCancel(); FtfDoDeInit())
};/* -- End ---------------------------------------------------------------- */
CTOR_END_ASYNC_NOFUNCS(Ftfs, Ftf, FTF, FONT) // Finish collector class
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
