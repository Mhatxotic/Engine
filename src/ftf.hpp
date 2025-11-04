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
using namespace IIdent::P;             using namespace ILockable::P;
using namespace ILog::P;               using namespace ILuaIdent::P;
using namespace ILuaLib::P;            using namespace ILuaUtil::P;
using namespace IMemory::P;            using namespace IStd::P;
using namespace ISysUtil::P;           using namespace IUtil::P;
using namespace Lib::OS::GlFW::Types;  using namespace Lib::FreeType;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* == Ftf collector class for collector data and custom variables ========== */
CTOR_BEGIN_ASYNC_DUO(Ftfs, Ftf, CLHelperUnsafe, ICHelperUnsafe),
  /* -- Base classes ------------------------------------------------------- */
  public AsyncLoaderFtf,               // Asyncronous loading of data
  public Lockable,                     // Lua garbage collector instruction
  public DimGLFloat,                   // Requested font width/height
  public FileMap                       // FT ttf file data (persistant)
{ /* -- Private variables -------------------------------------------------- */
  GLfloat          fOutline;           // FT outline size
  FT_Face          ftfFace;            // FT Char handle
  FT_Stroker       ftsStroker;         // FT Outline handle
  /* --------------------------------------------------------------- */ public:
  DimUInt          duDPI;              // FT DPI width and height
  /* -------------------------------------------------------------- */ private:
  void DoDeInit()
  { // Clear freetype handles if created
    if(IsStrokerLoaded()) FT_Stroker_Done(ftsStroker);
    if(IsLoaded()) cFreeType->DestroyFont(ftfFace);
  }
  /* -- Returns if face is loaded----------------------------------- */ public:
  bool IsLoaded() const { return !!ftfFace; }
  bool IsStrokerLoaded() const { return !!ftsStroker; }
  FT_Stroker GetStroker() const { return ftsStroker; }
  unsigned int GetDPIWidth() const { return duDPI.DimGetWidth(); }
  unsigned int GetDPIHeight() const { return duDPI.DimGetHeight(); }
  GLfloat GetOutline() const { return fOutline; }
  bool IsOutline() const { return GetOutline() > 0.0f; }
  FT_GlyphSlot GetGlyphData() const { return ftfFace->glyph; }
  const FT_String *GetFamily() const { return ftfFace->family_name; }
  const FT_String *GetStyle() const { return ftfFace->style_name; }
  FT_Long GetGlyphCount() const { return ftfFace->num_glyphs; }
  /* -- Set ftf size ------------------------------------------------------- */
  void UpdateSize()
  { // For some twisted reason, FreeType measures char size in terms f 1/64ths
    // of pixels. Thus, to make a char 'h' pixels high, we need to request a
    // size of 'h*64'.
    cFreeType->CheckError(FT_Set_Char_Size(ftfFace,
      static_cast<FT_F26Dot6>(DimGetWidth() * 64.0f),
      static_cast<FT_F26Dot6>(DimGetHeight() * 64.0f),
      duDPI.DimGetWidth<FT_UInt>(), duDPI.DimGetHeight<FT_UInt>()),
      "Failed to set character size!",
      "Identifier", IdentGet(),     "Width",    DimGetWidth(),
      "Height",     DimGetHeight(), "DPIWidth", duDPI.DimGetWidth(),
      "DPIHeight",  duDPI.DimGetHeight());
  }
  /* -- Convert character to glyph index --------------------------- */ public:
  FT_UInt CharToGlyph(const FT_ULong dwChar)
    { return FT_Get_Char_Index(ftfFace, dwChar); }
  /* -- Load a glyph ------------------------------------------------------- */
  FT_Error LoadGlyph(const FT_UInt uiIndex)
    { return FT_Load_Glyph(ftfFace, uiIndex,
        FT_LOAD_CROP_BITMAP|FT_LOAD_NO_AUTOHINT|FT_LOAD_NO_HINTING); }
  /* -- Load ftf from memory ----------------------------------------------- */
  void AsyncReady(FileMap &fmData)
  { // Take ownership of the file data
    FileMapSwap(fmData);
    // Load font
    cFreeType->CheckError(
      cFreeType->NewFont(static_cast<FileMap&>(*this), ftfFace),
        "Failed to create font!",
        "Identifier", IdentGet(),
        "Context",    cFreeType->IsLibraryAvailable(),
        "Buffer",     FileMap::MemIsPtrSet(),
        "Size",       FileMap::MemSize());
    // Update size
    UpdateSize();
    // Outline requested?
    if(IsOutline())
    { // Create stroker handle
      cFreeType->CheckError(cFreeType->NewStroker(ftsStroker),
        "Failed to create stroker!",
        "Identifier", IdentGet(), "Context", cFreeType->IsLibraryAvailable());
      // Set properties of stroker handle
      FT_Stroker_Set(ftsStroker, static_cast<FT_Fixed>(GetOutline() * 64.0f),
       FT_STROKER_LINECAP_ROUND, FT_STROKER_LINEJOIN_ROUND, 0);
    } // Report loaded font
    cLog->LogInfoExSafe("Ftf loaded '$' (FF:$;FS:$;S:$$$x$;D:$x$;B:$).",
      IdentGet(), GetFamily(), GetStyle(), setprecision(0), fixed,
      DimGetWidth(), DimGetHeight(), GetDPIWidth(), GetDPIHeight(),
      GetOutline());
  }
  /* -- Check and initialise supplied variables ---------------------------- */
  void InitVars(const GLfloat fWidth, const GLfloat fHeight,
    const unsigned int uiDpiWidth, const unsigned int uiDpiHeight,
    const GLfloat fNOutline)
  { // Set width and height
    DimSet(fWidth, fHeight);
    // Set DPI width and height
    duDPI.DimSet(uiDpiWidth, uiDpiHeight);
    // Set outline
    fOutline = fNOutline;
  }
  /* -- Load pcm from memory asynchronously -------------------------------- */
  void InitAsyncArray(lua_State*const lS, const string &strFile,
    Asset &aRef, const float fWidth, const float fHeight,
    const unsigned int uiDpiWidth, const unsigned int uiDpiHeight,
    const GLfloat fNOutline)
  { // Set other members
    InitVars(fWidth, fHeight, uiDpiWidth, uiDpiHeight, fNOutline);
    // Prepare asynchronous loading from array
    AsyncInitArray(lS, strFile, "ftfarray", aRef);
  }
  /* -- Load pcm from file asynchronously ---------------------------------- */
  void InitAsyncFile(lua_State*const lS, const string &strFile,
    const float fWidth, const float fHeight, const unsigned int uiDpiWidth,
    const unsigned int uiDpiHeight, const GLfloat fNOutline)
  { // Set other members
    InitVars(fWidth, fHeight, uiDpiWidth, uiDpiHeight, fNOutline);
    // Prepare asynchronous loading from array
    AsyncInitFile(lS, strFile, "ftffile");
  }
  /* -- Init from file ----------------------------------------------------- */
  void InitFile(const string &strFile, const GLfloat fWidth,
    const GLfloat fHeight, const unsigned int uiDpiWidth,
    const unsigned int uiDpiHeight, const GLfloat fNOutline)
  { // Set other members
    InitVars(fWidth, fHeight, uiDpiWidth, uiDpiHeight, fNOutline);
    // Load file normally
    SyncInitFileSafe(strFile);
  }
  /* -- Init from array ---------------------------------------------------- */
  void InitArray(const string &strName, Memory &mData, const GLfloat fWidth,
    const GLfloat fHeight, const unsigned int uiDpiWidth,
    const unsigned int uiDpiHeight, const GLfloat fNOutline)
  { // Set other members
    InitVars(fWidth, fHeight, uiDpiWidth, uiDpiHeight, fNOutline);
    // Load file as array
    SyncInitArray(strName, mData);
  }
  /* -- De-init ftf font --------------------------------------------------- */
  void DeInit() { DoDeInit(); ftsStroker = nullptr; ftfFace = nullptr; }
  /* ----------------------------------------------------------------------- */
  void SwapFtf(Ftf &ftfOther)
  { // Copy variables over from source class
    DimSwap(ftfOther);
    duDPI.DimSwap(ftfOther.duDPI);
    swap(fOutline, ftfOther.fOutline);
    swap(ftfFace, ftfOther.ftfFace);
    swap(ftsStroker, ftfOther.ftsStroker);
    // Swap file class
    FileMapSwap(ftfOther);
    // Swap async, lua lock data and registration
    LockSwap(ftfOther);
    CollectorSwapRegistration(ftfOther);
  }
  /* -- MOVE assignment (Ftf=Ftf) just do a swap --------------------------- */
  Ftf& operator=(Ftf &&ftfOther) { SwapFtf(ftfOther); return *this; }
  /* -- Default constructor ------------------------------------------------ */
  Ftf() :
    /* -- Initialisers ----------------------------------------------------- */
    ICHelperFtf{ cFtfs },              // Initially unregistered
    IdentCSlave{ cParent->CtrNext() }, // Initialise identification number
    AsyncLoaderFtf{ *this, this,       // Initialise async loader with class
      EMC_MP_FONT },                   // " and the event id
    fOutline(0.0f),                    // No outline size yet
    ftfFace(nullptr),                  // No FreeType handle yet
    ftsStroker(nullptr)                // No FreeType stroker handle yet
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- MOVE constructor --------------------------------------------------- */
  Ftf(Ftf &&ftfOther) :                // The other Ftf class to swap with
    /* -- Initialisers ----------------------------------------------------- */
    Ftf()                              // Use default initialisers
    /* -- Code that does the swap ------------------------------------------ */
    { SwapFtf(ftfOther); }
  /* -- Destructor --------------------------------------------------------- */
  ~Ftf() { AsyncCancel(); DoDeInit(); }
};/* -- End ---------------------------------------------------------------- */
CTOR_END_ASYNC_NOFUNCS(Ftfs, Ftf, FTF, FONT) // Finish collector class
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
