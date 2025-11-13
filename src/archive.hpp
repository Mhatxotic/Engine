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
using namespace ICommon::P;            using namespace ICollector::P;
using namespace ICrypt::P;             using namespace ICVarDef::P;
using namespace IDir::P;               using namespace IError::P;
using namespace IEvtMain::P;           using namespace IFileMap::P;
using namespace IFlags;                using namespace IIdent::P;
using namespace ILockable::P;          using namespace ILog::P;
using namespace ILuaIdent::P;          using namespace ILuaLib::P;
using namespace IPSplit::P;            using namespace IMemory::P;
using namespace IMutex::P;             using namespace IStd::P;
using namespace IString::P;            using namespace ISystem::P;
using namespace ISysUtil::P;           using namespace IUtf::P;
using namespace IUtil::P;              using namespace Lib::OS::SevenZip;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* -- Public typedefs ------------------------------------------------------ */
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
BUILD_FLAGS(Archive,                   // Archive flags
  /* ----------------------------------------------------------------------- */
  AE_STANDBY                {Flag(0)}, // Archive on standby?
  AE_FILEOPENED             {Flag(1)}, // Archive file handle opened?
  AE_SETUPL2R               {Flag(2)}, // Allocated look2read structs?
  AE_ARCHIVEINIT            {Flag(3)}  // Allocated archive structs?
);/* ----------------------------------------------------------------------- */
CTOR_MEM_BEGIN_ASYNC_CSLAVE(Archives, Archive, ICHelperUnsafe),
  /* -- Base classes ------------------------------------------------------- */
  public Ident,                        // Archive file name
  public AsyncLoaderArchive,           // Async manager for off-thread loading
  public Lockable,                     // Lua garbage collect instruction
  public ArchiveFlags,                 // Archive initialisation flags
  private Mutex,                       // Mutex for condition variable
  private condition_variable           // Waiting for async ops to complete
{ /* -- Private Variables -------------------------------------------------- */
  SafeSizeT        stInUse;            // API in use reference count
  /* ----------------------------------------------------------------------- */
  StrUIntMap       suimFiles,          // Files and directories as a map means
                   suimDirs;           // quick access to assets via filenames
  /* ----------------------------------------------------------------------- */
  StrUIntMapConstItVector suimcivFiles,// 0-indexed files in the archive
                          suimcivDirs; // 0-indexed directories in the archive
  /* ----------------------------------------------------------------------- */
  uint64_t         uqArchPos;          // Position of archive in file
  CFileInStream    cfisData;           // LZMA file stream data
  CLookToRead2     cltrData;           // LZMA lookup data
  CSzArEx          csaeData;           // LZMA archive Data
  /* ----------------------------------------------------------------------- */
#if defined(WINDOWS)                   // Using Windows?
  /* -- Open a stream ------------------------------------------------------ */
  WRes ArchiveOpenFile(CSzFile*const csfStream) const
    { return InFile_OpenW(csfStream, UTFtoS16(IdentGet()).data()); }
  /* -- Get operating system specific handle ------------------------------- */
  Lib::OS::HANDLE ArchiveCFISToOSHandle(const CFileInStream &cfisStream) const
    { return cfisStream.file.handle; }
  /* ----------------------------------------------------------------------- */
#else                                  // Linux or MacOS?
  /* -- Open a stream ------------------------------------------------------ */
  WRes ArchiveOpenFile(CSzFile*const csfStream) const
    { return InFile_Open(csfStream, IdentGet().data()); }
  /* -- Get operating system specific handle ------------------------------- */
  int ArchiveCFISToOSHandle(const CFileInStream &cfisStream) const
    { return cfisStream.file.fd; }
  /* ----------------------------------------------------------------------- */
#endif                                 // Operating system check
  /* -- Process extracted data --------------------------------------------- */
  FileMap ArchiveExtract(const string &strFile, const unsigned int uiSrcId,
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
             "Archive", IdentGet(), "File", strFile,
             "Index",   uiSrcId,    "Code", iCode,
             "Reason",  CodecGetLzmaErrString(iCode));
      // No data returned meaning a zero-byte file?
      if(!ucpData)
      { // Allocate a zero-byte array to a new class. Remember we need to send
        // a valid allocated pointer to the file map.
        FileMap fmFile{ strFile, Memory{ 0 }, ArchiveGetCreatedTime(uiSrcId),
          ArchiveGetModifiedTime(uiSrcId) };
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
          ArchiveGetCreatedTime(uiSrcId), ArchiveGetModifiedTime(uiSrcId) };
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
      FileMap fmFile{ strFile, StdMove(mData), ArchiveGetCreatedTime(uiSrcId),
        ArchiveGetModifiedTime(uiSrcId) };
      // Log progress
      cLog->LogInfoExSafe("Archive extracted '$'[$]{$>$} from '$'.",
        strFile, uiBlockIndex, stUncompressed, stCompressed, IdentGet());
      // Return class
      return fmFile;
    } // Exception occured
    catch(...)
    { // Free the block if it was allocated and rethrow the error
      if(ucpData)
        cParent->isaData.Free(nullptr, reinterpret_cast<void*>(ucpData));
      throw;
    }
  }
  /* -- Free the decopmression buffer if it was created -------------------- */
  void ArchiveCleanupLookToRead(CLookToRead2 &cltrRef)
    { if(cltrData.buf) ISzAlloc_Free(&cParent->isaData, cltrRef.buf); }
  /* -- Initialise Look2Read structs --------------------------------------- */
  void ArchiveSetupLookToRead(CFileInStream &cfisRef, CLookToRead2 &cltrRef)
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
  /* -- Get creation/modification time helper ------------------------------ */
  StdTimeT ArchiveSzTimeToStdTime(const size_t stId,
    const CSzBitUi64s &csbuTime) const
      { return static_cast<StdTimeT>(SzBitWithVals_Check(&csbuTime, stId) ?
          UtilBruteCast<uint64_t>(csbuTime.Vals[stId]) / 100000000 :
          numeric_limits<StdTimeT>::max()); }
  /* -- Get archive file/dir count as human readable string ---------------- */
  const string ArchiveGetFilesString(const auto iFiles) const
    { return StrCPluraliseNum(iFiles, "file", "files"); }
  const string ArchiveGetNumFilesString() const
    { return ArchiveGetFilesString(ArchiveGetNumFiles()); }
  const string ArchiveGetDirsString(const auto iDirs) const
    { return StrCPluraliseNum(iDirs, "directory", "directories"); }
  const string ArchiveGetNumDirsString() const
    { return ArchiveGetDirsString(ArchiveGetNumDirs()); }
  /* -- Get archive file/dir as table ------------------------------ */ public:
  const StrUIntMap &ArchiveGetFileList() const { return suimFiles; }
  const StrUIntMap &ArchiveGetDirList() const { return suimDirs; }
  /* -- Get archive file/dir counts ---------------------------------------- */
  size_t ArchiveGetNumFiles() const { return suimFiles.size(); }
  size_t ArchiveGetNumDirs() const { return suimDirs.size(); }
  size_t ArchiveGetTotal() const { return csaeData.NumFiles; }
  /* -- Returns modified or creation time of specified file ---------------- */
  StdTimeT ArchiveGetModifiedTime(const size_t stId) const
    { return ArchiveSzTimeToStdTime(stId, csaeData.MTime); }
  StdTimeT ArchiveGetCreatedTime(const size_t stId) const
    { return ArchiveSzTimeToStdTime(stId, csaeData.CTime); }
  /* -- Returns uncompressed size of file by id ---------------------------- */
  uint64_t ArchiveGetSize(const size_t stId) const
    { return static_cast<uint64_t>(SzArEx_GetFileSize(&csaeData, stId)); }
  /* -- Get a file/dir and uid by zero-index ------------------------------- */
  const StrUIntMapConstIt &ArchiveGetFile(const size_t stIndex) const
    { return suimcivFiles[stIndex]; }
  const StrUIntMapConstIt &ArchiveGetDir(const size_t stIndex) const
    { return suimcivDirs[stIndex]; }
  /* -- Return number of extra archives opened ----------------------------- */
  size_t ArchiveGetInUse() const { return stInUse; }
  /* -- Return if iterator is valid ---------------------------------------- */
  bool ArchiveIsFileIteratorValid(const StrUIntMapConstIt &suimciIt) const
    { return suimciIt != suimFiles.cend(); }
  /* -- Gets the iterator of a filename ------------------------------------ */
  const StrUIntMapConstIt ArchiveGetFileIterator(const string &strFile) const
    { return suimFiles.find(strFile); }
  /* -- Loads a file from archive by iterator ------------------------------ */
  FileMap ArchiveExtract(const StrUIntMapConstIt &suimciIt)
  { // Lock mutex. We don't care if we can't lock it though because we will
    // open another archive if we cannot lock it.
    return MutexUniqueCall([this, &suimciIt](UniqueLock &ulLock){
      // Create class to notify destructor when leaving this scope
      const class Notify { public: Archive &aRef;
        explicit Notify(Archive &aNRef) : aRef(aNRef) {}
        ~Notify() { aRef.notify_one(); } } cNotify(*this);
      // Get filename and filename id from iterator
      const string &strFile = suimciIt->first;
      const unsigned int &uiSrcId = suimciIt->second;
      // If the base archive file is in use?
      if(!ulLock.owns_lock())
      { // We need to adjust in-use counter when joining and leaving this scope
        // This tells the destructor to wait until it is zero.
        const class Counter { public: Archive &aRef;
          explicit Counter(Archive &aNRef) : aRef(aNRef) { ++aRef.stInUse; }
          ~Counter() { --aRef.stInUse; } } cCounter(*this);
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
          if(const int iCode = ArchiveOpenFile(&cfisData2.file))
            XC("Failed to open archive!",
              "Archive", IdentGet(),      "Index", suimciIt->second,
              "File",    suimciIt->first, "Code",  iCode,
              "Reason",  CodecGetLzmaErrString(iCode));
          // Custom start position specified
          if(uqArchPos > 0 &&
             cSystem->SeekFile(ArchiveCFISToOSHandle(cfisData),
               uqArchPos) != uqArchPos)
            XC("Failed to seek in archive!",
              "Archive", IdentGet(),      "Index",    suimciIt->second,
              "File",    suimciIt->first, "Position", uqArchPos);
          // Load archive
          ArchiveSetupLookToRead(cfisData2, cltrData2);
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
          FileMap fmFile{
            ArchiveExtract(strFile, uiSrcId, cltrData2, csaeData2) };
          // Clean up look to read
          ArchiveCleanupLookToRead(cltrData2);
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
          ArchiveCleanupLookToRead(cltrData2);
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
      else return ArchiveExtract(strFile, uiSrcId, cltrData, csaeData);
    }, try_to_lock);
  }
  /* -- Loads a file from archive by filename ------------------------------ */
  FileMap ArchiveExtract(const string &strFile)
  { // Get iterator from filename and return empty file if not found
    const StrUIntMapConstIt suimciIt{ ArchiveGetFileIterator(strFile) };
    if(suimciIt == suimFiles.cend()) return {};
    // Extract the file
    return ArchiveExtract(suimciIt);
  }
  /* -- Checks if file is in archive --------------------------------------- */
  bool ArchiveFileExists(const string &strFile) const
    { return ArchiveGetFileIterator(strFile) != suimFiles.cend(); }
  /* -- Loads the specified archive ---------------------------------------- */
  void AsyncReady(FileMap &)
  { // Open archive and throw and show errno if it failed
    if(const int iCode = ArchiveOpenFile(&cfisData.file))
      XCS("Error opening archive!", "Archive", IdentGet(), "Code", iCode);
    FlagSet(AE_FILEOPENED);
    // Custom start position specified?
    if(uqArchPos > 0)
    { // Log position setting
      cLog->LogDebugExSafe("Archive loading '$' from position $...",
        IdentGet(), uqArchPos);
      // Seek to overlay in executable + 1 and if failed? Log the warning
      if(cSystem->SeekFile(ArchiveCFISToOSHandle(cfisData),
           uqArchPos) != uqArchPos)
        cLog->LogWarningExSafe("Archive '$' seek error! [$].",
          IdentGet(), SysError());
    } // Load from beginning? Log that we're loading from beginning
    else cLog->LogDebugExSafe("Archive loading '$'...", IdentGet());
    // Setup look to read structs
    ArchiveSetupLookToRead(cfisData, cltrData);
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
    if(csaeData.NumFiles > suimcivFiles.max_size())
      XC("Insufficient storage for file list!",
         "Current", csaeData.NumFiles, "Maximum", suimcivFiles.max_size());
    suimcivFiles.reserve(csaeData.NumFiles);
    // ...and same for the directories too.
    const size_t stDirsMaximum = suimcivDirs.max_size();
    if(suimcivFiles.size() > stDirsMaximum)
      XC("Insufficient storage for directory list!",
         "Current", suimcivFiles.size(), "Maximum", stDirsMaximum);
    suimcivDirs.reserve(suimcivFiles.size());
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
        suimcivDirs.push_back(suimDirs.insert({
          UtfFromWide(wvFilesWide.data()), uiIndex }).first);
      // Is a file?
      else
        // Convert wide-string to utf-8 and insert it in the files to integer
        // list and store the iterator in the vector
        suimcivFiles.push_back(suimFiles.insert({
          UtfFromWide(wvFilesWide.data()), uiIndex }).first);
    } // We did not know how many files and directories there were
    // specifically so lets free the extra memory allocated for the lists
    suimcivFiles.shrink_to_fit();
    suimcivDirs.shrink_to_fit();
    // Archive has files?
    if(!suimFiles.empty())
      // Archive has directories? Write log with number of files and dirs.
      if(!suimDirs.empty())
        cLog->LogInfoExSafe("Archive loaded '$' with $ and $.", IdentGet(),
          ArchiveGetNumFilesString(), ArchiveGetNumDirsString());
      // Archive has only files? Write log with number of dirs.
      else cLog->LogInfoExSafe("Archive loaded '$' with $.", IdentGet(),
        ArchiveGetNumFilesString());
    // Archive has only dirs? Write log with number of dirs.
    else if(!suimDirs.empty())
      cLog->LogInfoExSafe("Archive loaded '$' with $.", IdentGet(),
        ArchiveGetNumDirsString());
    // Archive is empty?
    else cLog->LogWarningExSafe("Archive loaded empty '$'!", IdentGet());
  }
  /* -- Loads archive synchronously at specified position ------------------ */
  void InitFromFile(const string &strFile, const uint64_t uqPosition)
  { // Store position
    uqArchPos = uqPosition;
    // Set filename without filename checking
    SyncInitFileDisk(strFile);
  }
  /* -- Default constructor ------------------------------------------------ */
  Archive() :
    /* -- Initialisers ----------------------------------------------------- */
    ICHelperArchive{ cArchives },      // Initialise collector with this obj
    IdentCSlave{ cParent->CtrNext() }, // Initialise identification number
    AsyncLoaderArchive{ *this, this,   // Initialise async collector
      EMC_MP_ARCHIVE },                // " our archive async event
    ArchiveFlags{ AE_STANDBY },        // Set default archive flags
    stInUse(0),                        // Set threads in use
    uqArchPos(0),                      // Set archive initial position
    cfisData{},                        // Clear file stream data
    cltrData{},                        // Clear lookup stream data
    csaeData{}                         // Clear archive stream data
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- Unloads the archive ------------------------------------------------ */
  ~Archive()
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
      MutexUniqueCall([this](UniqueLock &ulLock){
        wait(ulLock, [this]{ return !stInUse; }); });
    } // Free archive structs if allocated
    if(FlagIsSet(AE_ARCHIVEINIT)) SzArEx_Free(&csaeData, &cParent->isaData);
    // Memory allocated? Free memory allocated for buffer
    if(FlagIsSet(AE_SETUPL2R)) ArchiveCleanupLookToRead(cltrData);
    // Close archive handle
    if(File_Close(&cfisData.file))
      cLog->LogWarningExSafe("Archive failed to close archive '$': $!",
        IdentGet(), SysError());
    // Log shutdown
    cLog->LogInfoExSafe("Archive unloaded '$' successfully.", IdentGet());
  }
};/* ----------------------------------------------------------------------- */
CTOR_END_ASYNC_NOFUNCS(Archives, Archive, ARCHIVE, ARCHIVE, // Finish collector
  /* -- Collector initialisers --------------------------------------------- */
  stExtractBufSize(0),                 // Init extract buffer size
  isaData{ Alloc, Free }               // Init custom allocators
);/* == Look if a file exists in archives ================================== */
static bool ArchiveFileExists(const string &strFile)
{ // Lock archive list so it cannot be modified and iterate through the list
  return cArchives->MutexCall([&strFile](){
    // For each archive. Return if the specified file exists in it.
    return any_of(cArchives->cbegin(), cArchives->cend(),
      [&strFile](const Archive*const aPtr)
        { return aPtr->ArchiveFileExists(strFile); });
  });
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
    if(!oRef.ArchiveGetFileList().empty() || !oRef.ArchiveGetDirList().empty())
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
  cLog->LogDebugExSafe("Archives loading $ candidates...",
    dArchives.GetFilesSize());
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
      stFiles += aPtr->ArchiveGetFileList().size();
      stDirs += aPtr->ArchiveGetDirList().size();
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
  return ArchiveScan("working",
    cCommon->CommonBlank(), cArchives->strArchiveExt);
}
/* -- Loads the specified archive ------------------------------------------ */
static CVarReturn ArchiveInitPersist(const bool bState)
{ // Need functions from command-line module
  using namespace ICmdLine::P;
  // Ignore if requested to not do this or there is no home directory
  if(!bState || cCmdLine->CmdLineIsNoHome()) return ACCEPT;
  // It is an error now if there is no extension
  if(cArchives->strArchiveExt.empty()) return DENY;
  // Now do the scan
  return ArchiveScan("home", cCmdLine->CmdLineGetHome(),
    cArchives->strArchiveExt);
}
/* -- Parallel enumeration ------------------------------------------------- */
static void ArchiveEnumFiles(const string &strDir, const StrUIntMap &suimList,
  StrSet &ssFiles, Mutex &mLock)
{ // For each directory in archive. Try to use multi-threading.
  StdForEach(par_unseq, suimList.cbegin(), suimList.cend(),
    [&strDir, &ssFiles, &mLock](const StrUIntMapPair &suimpRef)
  { // Ignore if folder name does not match or a forward-slash found after
    if(strDir != suimpRef.first.substr(0, strDir.length()) ||
      suimpRef.first.find('/', strDir.length() + 1) != StdNPos) return;
    // Split file path
    const PathSplit psParts{ suimpRef.first };
    // Lock access to archives list and split path parts and move into list
    mLock.MutexCall([&ssFiles, &psParts](){
      ssFiles.emplace(StdMove(psParts.strFileExt)); });
  });
}
/* -- Return files in directories and archives with empty check ------------ */
static const StrSet &ArchiveEnumerate(const string &strDir,
  const string &strExt, const bool bOnlyDirs, StrSet &ssFiles)
{ // Lock archive list so it cannot be modified
  return cArchives->MutexCall(
    [&strDir, &strExt, bOnlyDirs, &ssFiles]()->StrSet&{
    // Return if no archives
    if(cArchives->empty()) return ssFiles;
    // Lock for file list
    Mutex mLock;
    // If only dirs requested? For each archive.
    if(bOnlyDirs)
      StdForEach(par, cArchives->cbegin(), cArchives->cend(),
        [&strDir, &ssFiles, &mLock](const Archive*const aPtr)
          { ArchiveEnumFiles(strDir,
              aPtr->ArchiveGetDirList(), ssFiles, mLock); });
    // No extension specified? Show all files
    else if(strExt.empty())
      StdForEach(par, cArchives->cbegin(), cArchives->cend(),
        [&strDir, &ssFiles, &mLock](const Archive*const aPtr)
          { ArchiveEnumFiles(strDir,
              aPtr->ArchiveGetFileList(), ssFiles, mLock); });
    // Files with extension requested. For each archive.
    else StdForEach(par, cArchives->cbegin(), cArchives->cend(),
      [&strDir, &ssFiles, &mLock, &strExt](const Archive*const aPtr)
    { // Get reference to file list
      const StrUIntMap &suimList = aPtr->ArchiveGetFileList();
      // For each directory in archive...
      StdForEach(par_unseq, suimList.cbegin(), suimList.cend(),
        [&strDir, &ssFiles, &mLock, &strExt](const StrUIntMapPair &suimpRef)
      { // Ignore if folder name does not match or a forward-slash found after
        if(strDir != suimpRef.first.substr(0, strDir.length()) ||
          suimpRef.first.find('/', strDir.length() + 1) != StdNPos) return;
        // Split path parts, and ignore if extension does not match
        const PathSplit psParts{ suimpRef.first };
        if(psParts.strExt != strExt) return;
        // Lock the mutex and insert into list
        mLock.MutexCall([&ssFiles, &psParts](){
          ssFiles.emplace(StdMove(psParts.strFileExt)); });
      });
    });
    // Return file list
    return ssFiles;
  });
}
/* -- Extract -------------------------------------------------------------- */
static FileMap ArchiveExtract(const string &strFile)
{ // Lock archive list so it cannot be modified
  return cArchives->MutexUniqueCall([&strFile](UniqueLock &ulLock)->FileMap{
    // Enumerate each archive from last to first because the latest-most loaded
    // archive should always have priority if multiple archives have the same
    // filename, just like game engines do.
    for(Archives::const_reverse_iterator cArchIt{ cArchives->crbegin() };
                                         cArchIt != cArchives->crend();
                                       ++cArchIt)
    { // Get reference to archive and find file, try next file if not found
      Archive &aRef = **cArchIt;
      const StrUIntMapConstIt suimciIt{ aRef.ArchiveGetFileIterator(strFile) };
      if(!aRef.ArchiveIsFileIteratorValid(suimciIt)) continue;
      // Unlock the mutex because we don't need access to the list anymore
      const UniqueRelock urLock{ ulLock };
      // Try to extract file and return it if succeeded!
      FileMap fmFile{ aRef.ArchiveExtract(suimciIt) };
      if(fmFile.FileMapOpened()) return fmFile;
    } // File not found
    return {};
  });
}
/* -- ArchiveGetNames ------------------------------------------------------ */
static const string ArchiveGetNames()
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
