/* == ASSET.HPP ============================================================ **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## This the file handles the .adb (7-zip) archives and the extraction  ## **
** ## and decompression of files.                                         ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IAsset {                     // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IArchive::P;           using namespace IASync::P;
using namespace ICodec::P;             using namespace ICommon::P;
using namespace ICollector::P;         using namespace ICVarDef::P;
using namespace IDir::P;               using namespace IError::P;
using namespace IEvtMain::P;           using namespace IIdent::P;
using namespace IFlags;                using namespace ILockable::P;
using namespace ILog::P;               using namespace ILuaIdent::P;
using namespace ILuaLib::P;            using namespace IMemory::P;
using namespace IStd::P;               using namespace IString::P;
using namespace ISysUtil::P;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* -- Override type -------------------------------------------------------- */
enum FSOverrideType
{ /* ----------------------------------------------------------------------- */
   FO_INTONLY,                         // Internal archives only
   FO_EXTINT,                          // External and then internal
   FO_INTEXT,                          // Internal and then external
   FO_EXTONLY                          // External files only
}; /* ---------------------------------------------------------------------- */
/* -- Typedefs ------------------------------------------------------------- */
BUILD_FLAGS(Asset,                     // Asset loading flags
  /* -- Commands ----------------------------------------------------------- */
  // Leave the file loaded as is?      Decode the specified block?
  CD_NONE                   {Flag(0)}, CD_DECODE                 {Flag(1)},
  // Copy the specified block?         Encrypt the specified block?
  CD_ENCODE_RAW             {Flag(2)}, CD_ENCODE_AES             {Flag(3)},
  // Deflate the specified block?      Compress the specified block?
  CD_ENCODE_ZLIB            {Flag(4)}, CD_ENCODE_LZMA            {Flag(5)},
  // Deflate+encrypt specified block?  Compress+encrypt specified block?
  CD_ENCODE_ZLIBAES         {Flag(6)}, CD_ENCODE_LZMAAES         {Flag(7)},
  /* -- Options ------------------------------------------------------------ */
  // Fastest compression (less mem)?   Fast compression?
  CD_LEVEL_FASTEST         {Flag(60)}, CD_LEVEL_FAST            {Flag(61)},
  // Medium compression?               Good compression?
  CD_LEVEL_MODERATE        {Flag(62)}, CD_LEVEL_SLOW            {Flag(63)},
  // Maximum compression (more mem)?
  CD_LEVEL_SLOWEST         {Flag(64)},
  /* -- All options -------------------------------------------------------- */
  CD_MASK{ CD_DECODE|CD_ENCODE_RAW|CD_ENCODE_AES|CD_ENCODE_ZLIB|CD_ENCODE_LZMA|
           CD_ENCODE_ZLIBAES|CD_ENCODE_LZMAAES|CD_LEVEL_FASTEST|CD_LEVEL_FAST|
           CD_LEVEL_MODERATE|CD_LEVEL_SLOW|CD_LEVEL_SLOWEST }
);/* == Asset collector class for collector and custom variables =========== */
CTOR_BEGIN_ASYNC(Assets, Asset, CLHelperUnsafe,
/* -- Asset collector variables -------------------------------------------- */
FSOverrideType     fsotOverride;       // Allow load of external files
SafeSizeT          stPipeBufSize;      // Pipe buffer size for execute
); /* ---------------------------------------------------------------------- */
/* -- Function to load a file locally -------------------------------------- */
static FileMap AssetLoadFromDisk(const string &strFile)
{ // Open it and sending full-load flag and if succeeded?
  if(FileMap fmFile{ strFile })
  { // Put in the log that we loaded the file successfully
    cLog->LogDebugExSafe("Assets mapped resource '$'[$]!",
      strFile, fmFile.MemSize());
    // Return file class to caller
    return fmFile;
  } // Failed so throw exception
  XCL("Failed to open local resource!", "File",  strFile, "Path", DirGetCWD());
}
/* -- Function to search local directory then archives for a file ---------- */
static FileMap AssetExtract(const string &strFile)
{ // Check the order in which we load
  switch(cAssets->fsotOverride)
  { // Internal files from archives only?
    case FO_INTONLY:
      // If no archives just error out
      if(cArchives->CollectorEmpty())
        XC("Failed to find resource in archives!", "File", strFile);
      // Load the file from archives
      if(FileMap fmFile{ ArchiveExtract(strFile) }) return fmFile;
      XCL("Failed to find resource in archives!",
        "File",     strFile,
        "Count",    cArchives->CollectorCount(),
        "Archives", ArchiveGetNames());
    // On disk files and then internal files from archives?
    case FO_EXTINT:
      // Load the file from disk if it exists try to load it
      if(DirLocalFileExists(strFile)) return AssetLoadFromDisk(strFile);
      // If no archives just error out
      if(cArchives->CollectorEmpty())
        XCL("Failed to find resource on disk or archives!", "File", strFile);
      // Load the file from archives
      if(FileMap fmFile{ ArchiveExtract(strFile) }) return fmFile;
      XCL("Failed to load resource on disk or archives!",
        "File",  strFile,                     "Directory", DirGetCWD(),
        "Count", cArchives->CollectorCount(), "Archives",  ArchiveGetNames());
    // Internal files from archives and then on disk files?
    case FO_INTEXT:
      // If we have archives?
      if(cArchives->CollectorNotEmpty())
      { // Load the file from archives
        if(FileMap fmFile{ ArchiveExtract(strFile) }) return fmFile;
        // Load the file from disk if it exists try to load it
        if(DirLocalFileExists(strFile)) return AssetLoadFromDisk(strFile);
        XC("Failed to find resource in archives or on disk!",
          "File", strFile, "Directory", DirGetCWD());
      } // Load the file from disk if it exists try to load it
      if(DirLocalFileExists(strFile)) return AssetLoadFromDisk(strFile);
      XCL("Failed to load resource in archives or on disk!",
        "File",  strFile,                     "Directory", DirGetCWD(),
        "Count", cArchives->CollectorCount(), "Archives",  ArchiveGetNames());
    // On disk files only?
    case FO_EXTONLY:
      // Load the file from disk if it exists else throw and exception
      if(DirLocalFileExists(strFile)) return AssetLoadFromDisk(strFile);
      XC("Failed to find resource on disk!",
        "File", strFile, "Directory", DirGetCWD());
    // Anything else is a failure
    default: XC("No route to resource!", "File", strFile);
  }
}
/* == Asset object class =================================================== */
CTOR_MEM_BEGIN_ASYNC_CSLAVE(Assets, Asset, ICHelperUnsafe),
  /* -- Base classes ------------------------------------------------------- */
  public Ident,                        // Asset file name
  public AsyncLoaderAsset,             // For loading assets off main thread
  public Lockable,                     // Lua garbage collector instruction
  public AssetFlags                    // Asset settings
{ /* -------------------------------------------------------------- */ private:
  void AssetSwap(Asset &aOther)
  { // Swap settings flags
    FlagSwap(aOther);
    MemSwap(aOther);
    LockSwap(aOther);
    IdentSwap(aOther);
    CollectorSwapRegistration(aOther);
  }
  /* -- Perform decoding --------------------------------------------------- */
  template<class Codec>void AssetCodecExec(FileMap &fmData, size_t stLevel=0)
    { MemSwap(Block<Codec>{ fmData, stLevel }); }
  /* -- Perform decoding converting flags to compression level ------------- */
  template<class Codec>void AssetCodecExecEx(FileMap &fmData)
    { AssetCodecExec<Codec>(fmData,
        FlagIsSet(CD_LEVEL_FASTEST)  ? 1 : (FlagIsSet(CD_LEVEL_FAST) ? 3 :
       (FlagIsSet(CD_LEVEL_MODERATE) ? 5 : (FlagIsSet(CD_LEVEL_SLOW) ? 7 :
       (FlagIsSet(CD_LEVEL_SLOWEST)  ? 9 : 1))))); }
  /* -- Load asset from memory ------------------------------------- */ public:
  void AsyncReady(FileMap &fmData)
  { // Guest wants data put into a raw magic block (no user flags)
    if(FlagIsSet(CD_ENCODE_RAW))
      AssetCodecExec<RAWEncoder>(fmData);
    // Guest wants data encrypted into a magic block (no user flags)
    else if(FlagIsSet(CD_ENCODE_AES))
      AssetCodecExec<AESEncoder>(fmData);
    // Guest wants data deflated into a magic block
    else if(FlagIsSet(CD_ENCODE_ZLIB))
      AssetCodecExecEx<ZLIBEncoder>(fmData);
    // Guest wants data encrypted and deflated into a magic block
    else if(FlagIsSet(CD_ENCODE_ZLIBAES))
      AssetCodecExecEx<AESZLIBEncoder>(fmData);
    // Guest wants data compressed into a magic block
    else if(FlagIsSet(CD_ENCODE_LZMA))
      AssetCodecExecEx<LZMAEncoder>(fmData);
    // Guest wants data encrypted and compressed into a magic block
    else if(FlagIsSet(CD_ENCODE_LZMAAES))
      AssetCodecExecEx<AESLZMAEncoder>(fmData);
    // Guest wants data decoded from a magic block (no user flags)
    else if(FlagIsSet(CD_DECODE))
      AssetCodecExec<CoDecoder>(fmData);
    // Guest wants data untouched? Load in all the data in the map
    else MemSwap(fmData.FileMapDecouple());
  }
  /* -- Load asset from asset asynchronously ------------------------------- */
  void AssetInitAsyncAsset(lua_State*const lS, const string &strName,
    const AssetFlagsConst &afcFlags, Asset &aCref)
  { // Prepare user flags
    FlagSet(afcFlags);
    // Dispatch the event
    AsyncInitArray(lS, strName, "assetdata", aCref);
  }
  /* -- Load asset from file asynchronously -------------------------------- */
  void AssetInitAsyncFile(lua_State*const lS, const string &strFile,
    const AssetFlagsConst &afcFlags)
  { // Prepare user flags
    FlagSet(afcFlags);
    // Load asset from file asynchronously
    AsyncInitFile(lS, strFile, "assetfile");
  }
  /* -- Load asset from command-line --------------------------------------- */
  void AssetInitAsyncCmdLine(lua_State*const lS, const string &strCmdLine,
    const AssetFlagsConst &afcFlags)
  { // Prepare user flags
    FlagSet(afcFlags);
    // Nothing to send to 'stdin'
    Memory mbBlank;
    // Dispatch the request asynchronously
    AsyncInitCmdLine(lS, strCmdLine, "assetexec", mbBlank);
  }
  /* -- Load asset from command-line --------------------------------------- */
  void AssetInitAsyncCmdLineEx(lua_State*const lS, const string &strCmdLine,
    const AssetFlagsConst &afcFlags, Asset &aStdIn)
  { // Prepare user flags
    FlagSet(afcFlags);
    // Load asset from file asynchronously
    AsyncInitCmdLine(lS, strCmdLine, "assetexecex", aStdIn);
  }
  /* -- Init from file ----------------------------------------------------- */
  void AssetInitFile(const string &strFile, const AssetFlagsConst &afcFlags)
  { // Set load flags
    FlagSet(afcFlags);
    // Load file normally
    SyncInitFileSafe(strFile);
  }
  /* -- Init from file ----------------------------------------------------- */
  void AssetInitPtr(const string &strName, const AssetFlagsConst &afcFlags,
    size_t stNSize, const char*const cpNPtr)
  { // Prepare flags
    FlagSet(afcFlags);
    // Copy the memory
    Memory mData{ stNSize, cpNPtr };
    // Do the initialisation
    SyncInitArray(strName, mData);
  }
  /* -- Init from asset ---------------------------------------------------- */
  void AssetInitAsset(const string &strName, const AssetFlagsConst &afcFlags,
    Asset &aData)
      { AssetInitPtr(strName,
          afcFlags, aData.MemSize(), aData.MemPtr<char>()); }
  /* -- Init duplicate asset ----------------------------------------------- */
  void AssetInitDuplicate(const Asset &aCref)
  { // Copy name of asset
    IdentSet(aCref.IdentGet());
    // Copy flags
    FlagSet(aCref.FlagGet());
    // Then copy the memory over
    MemInitCopy(aCref);
  }
  /* -- Init blank asset --------------------------------------------------- */
  void AssetInitBlank(const string &strName, const size_t stBytes)
  { // Set name of memory
    IdentSet(StdMove(strName));
    // Allocate the memory and fill it
    MemInitSafe(stBytes);
  }
  /* -- Init from string --------------------------------------------------- */
  void AssetInitArray(const string &strName, Asset &aRef,
    const AssetFlagsConst &afcFlags)
  { // Set load flags
    FlagSet(afcFlags);
    // Set filename and flags
    SyncInitArray(strName, aRef);
  }
  /* -- Move assignment ---------------------------------------------------- */
  Asset& operator=(Asset &&aOther) { AssetSwap(aOther); return *this; }
  /* -- Move constructor --------------------------------------------------- */
  Asset(Asset &&aOther) : Asset() { AssetSwap(aOther); }
  /* -- For loading via LUA ------------------------------------------------ */
  Asset(void) :
    /* -- Initialisers ----------------------------------------------------- */
    ICHelperAsset{ cAssets },          // Initially unregistered
    IdentCSlave{ cParent->CtrNext() }, // Initialise identification number
    AsyncLoaderAsset{ *this, this,     // Initialise async class with this
      EMC_MP_ASSET },                  // ...and the event id for it.
    AssetFlags{ CD_NONE }              // Np asset load flags initially
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Destructor --------------------------------------------------------- */
  ~Asset(void) { AsyncCancel(); }
};/* ======================================================================= */
CTOR_END_ASYNC_NOFUNCS(Assets, Asset, ASSET, ASSET, fsotOverride(FO_EXTONLY));
/* -- Class to help enumerate files ---------------------------------------- */
struct AssetList :
  /* -- Dependents --------------------------------------------------------- */
  public StrSet                        // Sorted storage for filenames
{ /* -- Direct access using class variable name which returns if !empty ---- */
  operator bool(void) const { return !empty(); }
  /* -- Cast down to StrSet ------------------------------------------------ */
  StrSet &ToStrSet(void) { return *this; }
  /* -- Return files in directories ---------------------------------------- */
  AssetList(const string &strDir, const bool bOnlyDirs) :
    /* -- Initialisers ----------------------------------------------------- */
    StrSet{ bOnlyDirs ? StdMove(Dir{ strDir }.DirsToSet()) :
                        StdMove(Dir{ strDir }.FilesToSet()) }
    /* -- Add archive files to list ---------------------------------------- */
    { ArchiveEnumerate(strDir, cCommon->CommonBlank(), bOnlyDirs, *this); }
  /* -- Return files in directories with extension matching ---------------- */
  AssetList(const string &strDir, const string &strExt, const bool bOnlyDirs) :
    /* -- Initialisers ----------------------------------------------------- */
    StrSet{ bOnlyDirs ? StdMove(Dir{ strDir, strExt }.DirsToSet()) :
                        StdMove(Dir{ strDir, strExt }.FilesToSet()) }
    /* -- Add archive files to list ---------------------------------------- */
    { ArchiveEnumerate(strDir, strExt, bOnlyDirs, *this); }
};/* ----------------------------------------------------------------------- */
/* -- Look if a file exists ------------------------------------------------ */
static bool AssetExists(const string &strFile)
  { return DirLocalFileExists(strFile) || ArchiveFileExists(strFile); }
/* -- Allow external file access ------------------------------------------- */
static CVarReturn AssetSetFSOverride(const FSOverrideType fsState)
  { return CVarSimpleSetInt(cAssets->fsotOverride, fsState); }
/* -- Set pipe buffer size ------------------------------------------------- */
static CVarReturn AssetSetPipeBufferSize(const size_t stSize)
  { return CVarSimpleSetIntNLG(cAssets->stPipeBufSize, stSize, 1UL,
      static_cast<unsigned long>(MAX_PIPE_BUFFER)); }
/* -- Set pipe buffer size ------------------------------------------------- */
static size_t AssetGetPipeBufferSize(void) { return cAssets->stPipeBufSize; }
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
