/* == ARCHIVE.HPP ========================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## This the file handles the .adb (7-zip) archives and the extraction  ## **
** ## and decompression of files.                                         ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IArchive {                   // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IASync::P;             using namespace ICodec::P;
using namespace ICollector::P;         using namespace ICrypt::P;
using namespace ICVarDef::P;           using namespace IDir::P;
using namespace IError::P;             using namespace IEvtMain::P;
using namespace IFileMap::P;           using namespace IFlags;
using namespace IIdent::P;             using namespace ILockable::P;
using namespace ILog::P;               using namespace ILuaIdent::P;
using namespace ILuaLib::P;            using namespace IPSplit::P;
using namespace IMemory::P;            using namespace IStd::P;
using namespace IString::P;            using namespace ISystem::P;
using namespace ISysUtil::P;           using namespace IUtf::P;
using namespace IUtil::P;              using namespace Lib::OS::SevenZip;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* == Archive collector with extract buffer size =========================== */
CTOR_BEGIN_ASYNC(Archives, Archive, CLHelperSafe,
  /* ----------------------------------------------------------------------- */
  string           strArchiveExt;      // Archive extension
  size_t           stExtractBufSize;   // Extract buffer size
  ISzAlloc         isaData;            // Allocator functions
  /* -- Alloc function for lzma -------------------------------------------- */
  static void *Alloc(ISzAllocPtr, size_t stBytes)
    { return StdAlloc<void>(stBytes); }
  /* -- Free function for lzma --------------------------------------------- */
  static void Free(ISzAllocPtr, void*const vpAddress)
    { StdFree(vpAddress); }
);/* ----------------------------------------------------------------------- */
BUILD_FLAGS(Archive,
  /* ----------------------------------------------------------------------- */
  // Archive on standby?             Archive file handle opened?
  AE_STANDBY              {Flag(0)}, AE_FILEOPENED           {Flag(1)},
  // Allocated look2read structs?    Allocated archive structs?
  AE_SETUPL2R             {Flag(2)}, AE_ARCHIVEINIT          {Flag(3)}
);/* == Archive object class =============================================== */
CTOR_MEM_BEGIN_ASYNC_CSLAVE(Archives, Archive, ICHelperUnsafe),
  /* -- Base classes ------------------------------------------------------- */
  public Ident,                        // Archive file name
  public AsyncLoaderArchive,           // Async manager for off-thread loading
  public Lockable,                     // Lua garbage collect instruction
  public ArchiveFlags,                 // Archive initialisation flags
  private mutex,                       // Mutex for condition variable
  private condition_variable           // Waiting for async ops to complete
{ /* -- Private macros ----------------------------------------------------- */
#if defined(WINDOWS)                   // Not using Windows?
# define LZMAOpen(s,f)                 InFile_OpenW(s, UTFtoS16(f).c_str())
# define LZMAGetHandle(s)              (s).file.handle
#else                                  // Using Windows?
# define LZMAOpen(s,f)                 InFile_Open(s, f)
# define LZMAGetHandle(s)              (s).file.fd
#endif                                 // Operating system check
  /* -- Private Variables -------------------------------------------------- */
  SafeSizeT        stInUse;            // API in use reference count
  /* ----------------------------------------------------------------------- */
  StrUIntMap       suimFiles,          // Files and directories as a map means
                   suimDirs;           // quick access to assets via filenames
  /* ----------------------------------------------------------------------- */
  StrUIntMapConstItVector auimcivFiles,// 0-indexed files in the archive
                          auimcivDirs; // 0-indexed directories in the archive
  /* ----------------------------------------------------------------------- */
  uint64_t         uqArchPos;          // Position of archive in file
  CFileInStream    cfisData;           // LZMA file stream data
  CLookToRead2     cltrData;           // LZMA lookup data
  CSzArEx          csaeData;           // LZMA archive Data
  /* -- Process extracted data --------------------------------------------- */
  FileMap Extract(const string &strFile, const unsigned int uiSrcId,
    CLookToRead2 &cltrRef, CSzArEx &csaeRef)
  { // Storage for buffer
    unsigned char *ucpData = nullptr;
    // Capture exceptions so we can clean up
    try
    { // If the file came from a solid block then stUncompressed is the size
      // of the entire block which may include other data from other files.
      // stCompressed is the actual size of the file. Offset is the byte offset
      // of the file data requested insize the solid block, always zero with
      // files in non-solid blocks.
      size_t stCompressed = 0, stOffset = 0, stUncompressed = 0;
      // Block index returned in extractor function
      unsigned int uiBlockIndex = StdMaxUInt;
      // Decompress the buffer using our base handles and throw error if it
      // failed
      if(const int iCode = SzArEx_Extract(&csaeRef, &cltrRef.vt,
        uiSrcId, &uiBlockIndex, &ucpData, &stUncompressed, &stOffset,
        &stCompressed, &cParent->isaData, &cParent->isaData))
          XC("Failed to extract file",
             "Archive", IdentGet(), "File",  strFile,
             "Index",   uiSrcId, "Code",  iCode,
             "Reason",  CodecGetLzmaErrString(iCode));
      // No data returned meaning a zero-byte file?
      if(!ucpData)
      { // Allocate a zero-byte array to a new class. Remember we need to send
        // a valid allocated pointer to the file map.
        FileMap fmFile{ strFile, Memory{ 0 }, GetCreatedTime(uiSrcId),
          GetModifiedTime(uiSrcId) };
        // Log progress
        cLog->LogInfoExSafe("Archive extracted empty '$' from '$'.",
          strFile, IdentGet());
        // Return file
        return fmFile;
      } // If the file is NOT from a solid block?
      if(stUncompressed == stCompressed)
      { // Put this pointer in a memory block and inherit the pointer, which
        // means no allocation or copying of memory is needed. Do not manually
        // deallocate this pointer, then return newly added item
        FileMap fmFile{ strFile,
          { stUncompressed, reinterpret_cast<void*>(ucpData) },
          GetCreatedTime(uiSrcId), GetModifiedTime(uiSrcId) };
        // Log progress
        cLog->LogInfoExSafe("Archive extracted '$'[$]<$> from '$'.",
          strFile, uiBlockIndex, stUncompressed, IdentGet());
        // Return file
        return fmFile;
      }
      // In this case we need to allocate a new block and copy over the data. I
      // highly discourage use of solid blocks due to this performance issue,
      // but there is really no need to deny this kind of loading.
      Memory mData{ stCompressed,
        reinterpret_cast<void*>(ucpData + stOffset) };
      // Free the data that was allocated by LZMA as we had to copy it
      cParent->isaData.Free(nullptr, reinterpret_cast<void*>(ucpData));
      // Return newly added item
      FileMap fmFile{ strFile, StdMove(mData), GetCreatedTime(uiSrcId),
        GetModifiedTime(uiSrcId) };
      // Log progress
      cLog->LogInfoExSafe("Archive extracted '$'[$]{$>$} from '$'.",
        strFile, uiBlockIndex, stUncompressed, stCompressed, IdentGet());
      // Return class
      return fmFile;
    } // Exception occured
    catch(...)
    { // Free the block if it was allocated
      if(ucpData)
        cParent->isaData.Free(nullptr, reinterpret_cast<void*>(ucpData));
      // Rethrow error
      throw;
    }
  }
  /* -- DeInitialise Look2Read structs ------------------------------------- */
  void CleanupLookToRead(CLookToRead2 &cltrRef)
  { // Free the decopmression buffer if it was created
    if(cltrData.buf) ISzAlloc_Free(&cParent->isaData, cltrRef.buf);
  }
  /* -- Initialise Look2Read structs --------------------------------------- */
  void SetupLookToRead(CFileInStream &cfisRef, CLookToRead2 &cltrRef)
  { // Setup vtables and stream pointers
    FileInStream_CreateVTable(&cfisRef);
    LookToRead2_CreateVTable(&cltrRef, False);
    cltrRef.realStream = &cfisRef.vt;
    // Need to allocate transfer buffer in later LZMA.
    cltrRef.buf = reinterpret_cast<Byte*>
      (ISzAlloc_Alloc(&cParent->isaData, cParent->stExtractBufSize));
    if(!cltrData.buf)
      XC("Error allocating buffer for archive!",
         "Archive", IdentGet(), "Bytes", cParent->stExtractBufSize);
    cltrRef.bufSize = cParent->stExtractBufSize;
    // Initialise look2read structs. On p7zip, the buffer allocation is already
    // done for us.
    LookToRead2_INIT(&cltrRef);
  }
  /* -- Get archive file/dir as table ------------------------------ */ public:
  const StrUIntMap &GetFileList(void) const { return suimFiles; }
  const StrUIntMap &GetDirList(void) const { return suimDirs; }
  /* -- Returns modified time of specified file ---------------------------- */
  StdTimeT GetModifiedTime(const size_t stId) const
    { return static_cast<StdTimeT>(SzBitWithVals_Check(&csaeData.MTime, stId) ?
        UtilBruteCast<uint64_t>(csaeData.MTime.Vals[stId]) / 100000000 :
        numeric_limits<StdTimeT>::max()); }
  /* -- Returns creation time of specified file ---------------------------- */
  StdTimeT GetCreatedTime(const size_t stId) const
    { return static_cast<StdTimeT>(SzBitWithVals_Check(&csaeData.CTime, stId) ?
        UtilBruteCast<uint64_t>(csaeData.CTime.Vals[stId]) / 100000000 :
        numeric_limits<StdTimeT>::max()); }
  /* -- Returns uncompressed size of file by id ---------------------------- */
  uint64_t GetSize(const size_t stId) const
    { return static_cast<uint64_t>(SzArEx_GetFileSize(&csaeData, stId)); }
  /* -- Total files and directories in archive ----------------------------- */
  size_t GetTotal(void) const { return csaeData.NumFiles; }
  /* -- Get a file/dir and uid by zero-index ------------------------------- */
  const StrUIntMapConstIt &GetFile(const size_t stIndex) const
    { return auimcivFiles[stIndex]; }
  const StrUIntMapConstIt &GetDir(const size_t stIndex) const
    { return auimcivDirs[stIndex]; }
  /* -- Return number of extra archives opened ----------------------------- */
  size_t GetInUse(void) const { return stInUse; }
  /* -- Return if iterator is valid ---------------------------------------- */
  bool IsFileIteratorValid(const StrUIntMapConstIt &suimciIt) const
    { return suimciIt != suimFiles.cend(); }
  /* -- Gets the iterator of a filename ------------------------------------ */
  const StrUIntMapConstIt GetFileIterator(const string &strFile) const
    { return suimFiles.find(strFile); }
  /* -- Loads a file from archive by iterator ------------------------------ */
  FileMap Extract(const StrUIntMapConstIt &suimciIt)
  { // Lock mutex. We don't care if we can't lock it though because we will
    // open another archive if we cannot lock it.
    const UniqueLock ulDecoder{ *this, try_to_lock };
    // Create class to notify destructor when leaving this scope
    const class Notify { public: Archive &aRef;
      explicit Notify(Archive &aNRef) : aRef(aNRef) { }
      ~Notify(void) { aRef.notify_one(); } } cNotify(*this);
    // Get filename and filename id from iterator
    const string &strFile = suimciIt->first;
    const unsigned int &uiSrcId = suimciIt->second;
    // If the base archive file is in use?
    if(!ulDecoder.owns_lock())
    { // We need to adjust in-use counter when joining and leaving this scope
      // This tells the destructor to wait until it is zero.
      const class Counter { public: Archive &aRef;
        explicit Counter(Archive &aNRef) : aRef(aNRef) { ++aRef.stInUse; }
        ~Counter(void) { --aRef.stInUse; } } cCounter(*this);
      // Because the API doesn't support sharing file handles, we will need
      // to re-open the archive again with new data. We don't need to collect
      // any filename data again though thankfully so this should still be
      // quite a speedy process.
      CFileInStream cfisData2{};
      CSzArEx csaeData2{};
      CLookToRead2 cltrData2{};
      // Capture exceptions so we can clean up 7zip api
      try
      { // Open new archive and throw error if it failed
        if(const int iCode = LZMAOpen(&cfisData2.file, IdentGetCStr()))
          XC("Failed to open archive!",
            "Archive", IdentGet(),      "Index", suimciIt->second,
            "File",    suimciIt->first, "Code",  iCode,
            "Reason",  CodecGetLzmaErrString(iCode));
        // Custom start position specified
        if(uqArchPos > 0 &&
          cSystem->SeekFile(LZMAGetHandle(cfisData), uqArchPos) != uqArchPos)
            XC("Failed to seek in archive!",
              "Archive", IdentGet(),      "Index",    suimciIt->second,
              "File",    suimciIt->first, "Position", uqArchPos);
        // Load archive
        SetupLookToRead(cfisData2, cltrData2);
        // Init lzma data
        SzArEx_Init(&csaeData2);
        // Initialise archive database and if failed, just log it
        if(const int iCode = SzArEx_Open(&csaeData2,
          &cltrData2.vt, &cParent->isaData, &cParent->isaData))
            XC("Failed to load archive!",
               "Archive", IdentGet(),      "Index", suimciIt->second,
               "File",    suimciIt->first, "Code",  iCode,
               "Reason",  CodecGetLzmaErrString(iCode));
        // Decompress the buffer using our duplicated handles
        FileMap fmFile{ Extract(strFile, uiSrcId, cltrData2, csaeData2) };
        // Clean up look to read
        CleanupLookToRead(cltrData2);
        // Free memory
        SzArEx_Free(&csaeData2, &cParent->isaData);
        // Close archive
        if(File_Close(&cfisData2.file))
          cLog->LogWarningExSafe("Archive failed to close archive '$': $!",
            IdentGet(), SysError());
        // Done
        return fmFile;
      } // exception occured
      catch(const exception&)
      { // Clean up look to read
        CleanupLookToRead(cltrData2);
        // Free memory
        SzArEx_Free(&csaeData2, &cParent->isaData);
        // Close archive
        if(File_Close(&cfisData2.file))
          cLog->LogWarningExSafe("Archive failed to close archive '$': $!",
            IdentGet(), SysError());
        // Show new exception for plain error message
        throw;
      } // Extract block
    } // Extract and return decompressed file
    else return Extract(strFile, uiSrcId, cltrData, csaeData);
  }
  /* -- Loads a file from archive by filename ------------------------------ */
  FileMap Extract(const string &strFile)
  { // Get iterator from filename and return empty file if not found
    const StrUIntMapConstIt suimciIt{ GetFileIterator(strFile) };
    if(suimciIt == suimFiles.cend()) return {};
    // Extract the file
    return Extract(suimciIt);
  }
  /* -- Checks if file is in archive --------------------------------------- */
  bool FileExists(const string &strFile) const
    { return GetFileIterator(strFile) != suimFiles.cend(); }
  /* -- Loads the specified archive ---------------------------------------- */
  void AsyncReady(FileMap &)
  { // Open archive and throw and show errno if it failed
    if(const int iCode = LZMAOpen(&cfisData.file, IdentGetCStr()))
      XCS("Error opening archive!", "Archive", IdentGet(), "Code", iCode);
    FlagSet(AE_FILEOPENED);
    // Custom start position specified?
    if(uqArchPos > 0)
    { // Log position setting
      cLog->LogDebugExSafe("Archive loading '$' from position $...",
        IdentGet(), uqArchPos);
      // Seek to overlay in executable + 1 and if failed? Log the warning
      if(cSystem->SeekFile(LZMAGetHandle(cfisData), uqArchPos) != uqArchPos)
        cLog->LogWarningExSafe("Archive '$' seek error! [$].",
          IdentGet(), SysError());
    } // Load from beginning? Log that we're loading from beginning
    else cLog->LogDebugExSafe("Archive loading '$'...", IdentGet());
    // Setup look to read structs
    SetupLookToRead(cfisData, cltrData);
    FlagSet(AE_SETUPL2R);
    // Init lzma data
    SzArEx_Init(&csaeData);
    FlagSet(AE_ARCHIVEINIT);
    // Initialise archive database and if failed, just log it
    if(const int iCode = SzArEx_Open(&csaeData, &cltrData.vt,
      &cParent->isaData, &cParent->isaData))
    { // Log warning and return
      cLog->LogWarningExSafe("Archive '$' not opened with code $ ($)!",
        IdentGet(), iCode, CodecGetLzmaErrString(iCode));
      return;
    }
    // Alocate memory for quick access via index vector. 7-zip won't tell us
    // how many files and directories there are individually so we'll reserve
    // memory for the maximum amount of entries in the 7-zip file. We'll shrink
    // this after to reclaim the memory
    const size_t stFilesMaximum = auimcivFiles.max_size();
    if(csaeData.NumFiles > stFilesMaximum)
      XC("Insufficient storage for file list!",
         "Current", csaeData.NumFiles, "Maximum", stFilesMaximum);
    auimcivFiles.reserve(csaeData.NumFiles);
    // ...and same for the directories too.
    const size_t stDirsMaximum = auimcivDirs.max_size();
    if(auimcivFiles.size() > stDirsMaximum)
      XC("Insufficient storage for directory list!",
         "Current", auimcivFiles.size(), "Maximum", stDirsMaximum);
    auimcivDirs.reserve(auimcivFiles.size());
    // Enumerate through each file
    for(unsigned int uiIndex = 0; uiIndex < csaeData.NumFiles; ++uiIndex)
    { // Get length of file name string. This includes the null terminator.
      const size_t stLen =
        SzArEx_GetFileNameUtf16(&csaeData, uiIndex, nullptr);
      if(stLen < sizeof(UInt16)) continue;
      // Create buffer for file name.
      typedef vector<UInt16> UInt16Vec;
      UInt16Vec wvFilesWide(stLen, 0x0000);
      SzArEx_GetFileNameUtf16(&csaeData, uiIndex, wvFilesWide.data());
      // If is a directory?
      if(SzArEx_IsDir(&csaeData, uiIndex))
        // Convert wide-string to utf-8 and insert it in the dirs to integer
        // list and store the iterator in the vector
        auimcivDirs.push_back(suimDirs.insert({
          UtfFromWide(wvFilesWide.data()), uiIndex }).first);
      // Is a file?
      else
        // Convert wide-string to utf-8 and insert it in the files to integer
        // list and store the iterator in the vector
        auimcivFiles.push_back(suimFiles.insert({
          UtfFromWide(wvFilesWide.data()), uiIndex }).first);
    } // We did not know how many files and directories there were
    // specifically so lets free the extra memory allocated for the lists
    auimcivFiles.shrink_to_fit();
    auimcivDirs.shrink_to_fit();
    // Log progress
    cLog->LogInfoExSafe("Archive loaded '$' (F:$;D:$).",
      IdentGet(), suimFiles.size(), suimDirs.size());
  }
  /* -- Loads archive synchronously at specified position ------------------ */
  void InitFromFile(const string &strFile, const uint64_t uqPosition)
  { // Store position
    uqArchPos = uqPosition;
    // Set filename without filename checking
    SyncInitFileDisk(strFile);
  }
  /* -- For loading via lua ------------------------------------------------ */
  Archive(void) :
    /* -- Initialisers ----------------------------------------------------- */
    ICHelperArchive{ cArchives },      // Initialise collector with this obj
    IdentCSlave{ cParent->CtrNext() }, // Initialise identification number
    AsyncLoaderArchive{ *this, this,   // Initialise async collector
      EMC_MP_ARCHIVE },                // " our archive async event
    ArchiveFlags{ AE_STANDBY },        // Set default archive flags
    stInUse(0),                        // Set threads in use
    uqArchPos(0),                      // Set archive initial position
    cfisData{},                        // Init file stream data
    cltrData{},                        // Init lookup stream data
    csaeData{}                         // Init archive stream data
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Unloads the archive ------------------------------------------------ */
  ~Archive(void)
  { // Done if a filename is not set
    if(IdentIsNotSet()) return;
    // Wait for archive loading async operations to complete
    AsyncCancel();
    // Done if a archive file handle is not opened.
    if(FlagIsClear(AE_FILEOPENED)) return;
    // Unloading archive in log
    cLog->LogDebugExSafe("Archive unloading '$'...", IdentGet());
    // If decompression is being executed across threads?
    if(stInUse > 0)
    { // Then we need to wait until they finish.
      cLog->LogInfoExSafe("Archive '$' waiting for $ async ops to complete...",
        IdentGet(), static_cast<size_t>(stInUse));
      // Wait for base and spawned file operations to finish
      UniqueLock ulDecoder{ *this };
      wait(ulDecoder, [this]{ return !stInUse; });
    } // Free archive structs if allocated
    if(FlagIsSet(AE_ARCHIVEINIT)) SzArEx_Free(&csaeData, &cParent->isaData);
    // Memory allocated? Free memory allocated for buffer
    if(FlagIsSet(AE_SETUPL2R)) CleanupLookToRead(cltrData);
    // Close archive handle
    if(File_Close(&cfisData.file))
      cLog->LogWarningExSafe("Archive failed to close archive '$': $!",
        IdentGet(), SysError());
    // Log shutdown
    cLog->LogInfoExSafe("Archive unloaded '$' successfully.", IdentGet());
  }
  /* -- Done with these defines -------------------------------------------- */
#undef LZMAGetHandle                   // Done with this macro
#undef LZMAOpen                        // Done with this macro
#undef ISzAllocPtr                     // Done with this macro
};/* ----------------------------------------------------------------------- */
CTOR_END_ASYNC_NOFUNCS(Archives, Archive, ARCHIVE, ARCHIVE, // Finish collector
  /* -- Collector initialisers --------------------------------------------- */
  stExtractBufSize(0),                 // Init extract buffer size
  isaData{ Alloc, Free }               // Init custom allocators
);/* == Look if a file exists in archives ================================== */
static bool ArchiveFileExists(const string &strFile)
{ // Lock archive list so it cannot be modified and iterate through the list
  const LockGuard lgArchivesSync{ cArchives->CollectorGetMutex() };
  // For each archive. Return if the specified file exists in it.
  return any_of(cArchives->cbegin(), cArchives->cend(),
    [&strFile](const Archive*const aPtr)
      { return aPtr->FileExists(strFile); });
}
/* -- Create and check a dynamic archive ----------------------------------- */
static Archive *ArchiveInitNew(const string &strFile, const size_t stSize=0)
{ // Dynamically create the archive. The pointer is recorded in the parent
  // collector class.
  typedef unique_ptr<Archive> ArchivePtr;
  if(ArchivePtr oClass{ new Archive })
  { // Load the archive and return archive if there are entries inside it
    Archive &oRef = *oClass.get();
    oRef.InitFromFile(strFile, stSize);
    if(!oRef.GetFileList().empty() || !oRef.GetDirList().empty())
      return oClass.release();
  } // Return nothing
  return nullptr;
}
/* -- Set extraction buffer size ------------------------------------------- */
static CVarReturn ArchiveSetBufferSize(const size_t stSize)
  { return CVarSimpleSetIntNLG(cArchives->stExtractBufSize, stSize,
      262144UL, 16777216UL); }
/* -- Loads the archive from executable ------------------------------------ */
static CVarReturn ArchiveInitExe(const bool bCheck)
{ // If we're checking the executable for archive?
  if(bCheck)
  { // Temporarily switch to executable directory. It is mainly cosmetic, but
    // I guess it provides security that the executables directory can be read
    // and executed.
    const string &strDir = cSystem->ENGLoc();
    if(!DirSetCWD(strDir))
      XCL("Failed to switch to executable directory!", "Directory", strDir);
    // Try to read executable size and if succeeded? Try loading it from the
    // position that was detected from the executable.
    const string &strFile = cSystem->ENGFileExt();
    if(const size_t stSize = cSystem->GetExeSize(strFile))
      ArchiveInitNew(strFile, stSize);
  } // Status is acceptable regardless
  return ACCEPT;
}
/* -- Scan for the specified archives in the specified directory ----------- */
static CVarReturn ArchiveScan(const char*const cpType, const string &strDir,
  const string &strExt)
{ // Build archive listing and if none found?
  cLog->LogDebugExSafe("Archives scanning $ directory for '$' files...",
    cpType, strExt);
  // Do the scan and if there is no files?
  const Dir dArchives{ strDir, strExt };
  if(dArchives.IsFilesEmpty())
  { // Report it in log and return success regardless
    cLog->LogDebugSafe("Archives matched no potential archive filenames!");
    return ACCEPT;
  } // Start processing filenames
  cLog->LogDebugExSafe("Archives loading $ files in $ directory...",
    cpType, dArchives.GetFilesSize());
  // Counters
  size_t stFound = 0, stFiles = 0, stDirs = 0;
  // For each archive file
  for(const DirEntMapPair &dempPair : dArchives.GetFiles())
  { // Log archive info
    cLog->LogDebugExSafe("- #$: '$' (S:$;A:0x$$;C:0x$;M:0x$).",
      ++stFound, dempPair.first, StrToBytes(dempPair.second.Size()), hex,
      dempPair.second.Attributes(), dempPair.second.Created(),
      dempPair.second.Written());
    // Dynamically create the archive. The pointer is recorded in the parent
    // and is referenced from there when loading other files. If succeeded?
    if(const Archive*const aPtr = ArchiveInitNew({ strDir + dempPair.first }))
    { // Add counters to grand totals
      stFiles += aPtr->GetFileList().size();
      stDirs += aPtr->GetDirList().size();
    }
  } // Log init
  cLog->LogInfoExSafe("Archives loaded $ of $ archives (F:$;D:$).",
    cArchives->CollectorCount(), dArchives.GetFilesSize(), stFiles, stDirs);
  // Success
  return ACCEPT;
}
/* -- Loads the specified archive ------------------------------------------ */
static CVarReturn ArchiveInit(const string &strExt, string&)
{ // Ignore if file mask not specified
  if(strExt.empty()) return ACCEPT;
  // Set bundle extension
  cArchives->strArchiveExt = StdMove(strExt);
  // Scan executable directory
  return ArchiveScan("working", cCommon->Blank(), cArchives->strArchiveExt);
}
/* -- Loads the specified archive ------------------------------------------ */
static CVarReturn ArchiveInitPersist(const bool bState)
{ // Need functions from command-line module
  using namespace ICmdLine::P;
  // Ignore if requested to not do this or there is no home directory
  if(!bState || cCmdLine->IsNoHome()) return ACCEPT;
  // It is an error now if there is no extension
  if(cArchives->strArchiveExt.empty()) return DENY;
  // Now do the scan
  return ArchiveScan("home", cCmdLine->GetHome(), cArchives->strArchiveExt);
}
/* -- Parallel enumeration ------------------------------------------------- */
static void ArchiveEnumFiles(const string &strDir, const StrUIntMap &suimList,
  StrSet &ssFiles, mutex &mLock)
{ // For each directory in archive. Try to use multi-threading.
  StdForEach(par_unseq, suimList.cbegin(), suimList.cend(),
    [&strDir, &ssFiles, &mLock](const StrUIntMapPair &suimpRef)
  { // Ignore if folder name does not match or a forward-slash found after
    if(strDir != suimpRef.first.substr(0, strDir.length()) ||
      suimpRef.first.find(cCommon->CFSlash(),
        strDir.length() + 1) != StdNPos) return;
    // Split file path
    const PathSplit psParts{ suimpRef.first };
    // Lock access to archives list
    const LockGuard lgLock{ mLock };
    // Split path parts, lock mutex, and move into list
    ssFiles.emplace(StdMove(psParts.strFileExt));
  });
}
/* -- Return files in directories and archives with empty check ------------ */
static const StrSet &ArchiveEnumerate(const string &strDir,
  const string &strExt, const bool bOnlyDirs, StrSet &ssFiles)
{ // Lock archive list so it cannot be modified and if we have archives?
  const LockGuard lgArchivesSync{ cArchives->CollectorGetMutex() };
  if(!cArchives->empty())
  { // Lock for file list
    mutex mLock;
    // If only dirs requested? For each archive.
    if(bOnlyDirs)
      StdForEach(par, cArchives->cbegin(), cArchives->cend(),
        [&strDir, &ssFiles, &mLock](const Archive*const aPtr)
          { ArchiveEnumFiles(strDir, aPtr->GetDirList(), ssFiles, mLock); });
    // No extension specified? Show all files
    else if(strExt.empty())
      StdForEach(par, cArchives->cbegin(), cArchives->cend(),
        [&strDir, &ssFiles, &mLock](const Archive*const aPtr)
          { ArchiveEnumFiles(strDir, aPtr->GetFileList(), ssFiles, mLock); });
    // Files with extension requested. For each archive.
    else StdForEach(par, cArchives->cbegin(), cArchives->cend(),
      [&strDir, &ssFiles, &mLock, &strExt](const Archive*const aPtr)
    { // Get reference to file list
      const StrUIntMap &suimList = aPtr->GetFileList();
      // For each directory in archive...
      StdForEach(par_unseq, suimList.cbegin(), suimList.cend(),
        [&strDir, &ssFiles, &mLock, &strExt](const StrUIntMapPair &suimpRef)
      { // Ignore if folder name does not match or a forward-slash found after
        if(strDir != suimpRef.first.substr(0, strDir.length()) ||
          suimpRef.first.find(cCommon->CFSlash(),
            strDir.length() + 1) != StdNPos) return;
        // Split path parts, and ignore if extension does not match
        const PathSplit psParts{ suimpRef.first };
        if(psParts.strExt != strExt) return;
        // Lock the mutex and insert into list
        const LockGuard lgLock{ mLock };
        ssFiles.emplace(StdMove(psParts.strFileExt));
      });
    });
  } // Return file list
  return ssFiles;
}
/* -- Extract -------------------------------------------------------------- */
static FileMap ArchiveExtract(const string &strFile)
{ // Lock archive list so it cannot be modified and iterate through the list
  UniqueLock ulLock{ cArchives->CollectorGetMutex() };
  // Enumerate each archive from last to first because the latest-most loaded
  // archive should always have priority if multiple archives have the same
  // filename, just like game engines do.
  for(Archives::const_reverse_iterator cArchIt{ cArchives->crbegin() };
                                       cArchIt != cArchives->crend();
                                     ++cArchIt)
  { // Get reference to archive and find file, try next file if file not found
    Archive &aRef = **cArchIt;
    const StrUIntMapConstIt suimciIt{ aRef.GetFileIterator(strFile) };
    if(!aRef.IsFileIteratorValid(suimciIt)) continue;
    // Unlock the mutex because we don't need access to the list anymore
    ulLock.unlock();
    // Try to extract file and return it if succeeded!
    FileMap fmFile{ aRef.Extract(suimciIt) };
    if(fmFile.FileMapOpened()) return fmFile;
    // Something bad happened so relock the mutex and keep trying other
    // archives anyway.
    ulLock.lock();
  } // File not found
  return {};
}
/* -- ArchiveGetNames ------------------------------------------------------ */
static const string ArchiveGetNames(void)
{ // Set default archive name if no archives
  if(cArchives->empty()) return {};
  // Get first archive
  ArchivesItConst aicIt{ cArchives->cbegin() };
  // Lets build a list of archives
  ostringstream osS;
  // Add first archive name to list
  osS << (*aicIt)->IdentGet();
  // Until end of archives add their names prefixed with a space
  while(++aicIt != cArchives->cend()) osS << ' ' << (*aicIt)->IdentGet();
  // Return string
  return osS.str();
}
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
