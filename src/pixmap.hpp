/* == PIXMAP.HPP =========================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## This is a POSIX specific module that handles mapping files in       ## **
** ## memory. It will be derived by the FileMap class. Since we support   ## **
** ## MacOS and Linux, we can support both systems very simply with POSIX ## **
** ## compatible calls.                                                   ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace ISysMap {                    // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace ICommon::P;            using namespace IError::P;
using namespace IFStream::P;           using namespace ILog::P;
using namespace IStd::P;               using namespace IStdLib::P;
using namespace ISysUtil::P;           using namespace Lib::OS;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* ------------------------------------------------------------------------- */
class SysMap :
  /* -- Base classes ------------------------------------------------------- */
  public FStreamBase                   // File stream base class
{ /* ----------------------------------------------------------------------- */
  StdFStatStruct   sData;              // File data
  char            *cpMem;              // Handle to memory
  /* -- De-init the file map ----------------------------------------------- */
  void SysMapDeInitInternal()
  { // Unmap the memory if it was mapped
    if(SysMapIsAvailable() && SysMapIsNotEmpty() && munmap(SysMapGetMemory(),
      static_cast<size_t>(SysMapGetSize())))
      cLog->LogWarningExSafe("Failed to unmap '$' from 0x$$[$$]: $!",
        NameGet(), SysMapGetMemory<void>(), SysMapGetSize(), SysError());
    // ~FStream() will close the file
  }
  /* -- Setup handle ------------------------------------------------------- */
  static FStreamBase SysMapSetupFile(const StdStringView &strvF)
  { // Open file and return it if opened else show error
    if(FStream fsFile{ strvF, FM_R_B }) return fsFile;
    XCS("Open file for file mapping failed!", "File", strvF);
  }
  /* -- Setup file information --------------------------------------------- */
  StdFStatStruct SysMapSetupInfo()
  { // Get informationa about file and return it else show error
    StdFStatStruct sNewData;
    if(FStreamStat(sNewData)) return sNewData;
    XCS("Failed to read file information!", "File", NameGet());
  }
  /* -- Setup memory pointer ----------------------------------------------- */
  char *SysMapSetupMemory()
  { // Memory to return
    char *cpNewMem;
    // Size cannot be bigger than 4GB on 32-bit system
#if defined(X86)
    if(SysMapGetSize() > 0xFFFFFFFF)
      XC("File too big to map into address space!",
        "File", NameGet(), "Size", SysMapGetSize());
#endif
    // If the file is not empty?
    if(SysMapGetSize())
    { // map the file in memory. This is automatically closed when stream is
      // closed
      cpNewMem = StdMMap<char>(nullptr, SysMapGetSize(),
        PROT_READ, MAP_PRIVATE, FStreamGetFdSafe(), 0);
      if(cpNewMem == MAP_FAILED)
        XCS("Map view of file failed!", "File", NameGet());
    } // File is empty
    else
    { // Set no data available
      cpNewMem = const_cast<char*>(cCommon->CommonCBlank());
      // Close the file. Whats the point in keeping it open?
      if(!FStreamClose())
        XCS("Failed to close empty file!", "File", NameGet());
    } // Return memory
    return cpNewMem;
  }
  /* -- Clear variables ---------------------------------------------------- */
  void SysMapClearVarsInternal() { cpMem = nullptr; sData = {}; }
  /* -- Get members ------------------------------------------------ */ public:
  template<typename PtrType = char>
    requires (!StdIsPointer<PtrType>)
  PtrType *SysMapGetMemory() const
    { return reinterpret_cast<PtrType*>(cpMem); }
  bool SysMapIsEmpty() const { return cpMem == cCommon->CommonCBlank(); }
  bool SysMapIsNotEmpty() const { return !SysMapIsEmpty(); }
  bool SysMapIsAvailable() const { return SysMapGetMemory() != nullptr; }
  bool SysMapIsNotAvailable() const { return !SysMapIsAvailable(); }
  uint64_t SysMapGetSize() const
    { return static_cast<uint64_t>(sData.st_size); }
  StdTimeT SysMapGetCreation() const { return sData.st_ctime; }
  StdTimeT SysMapGetModified() const { return sData.st_mtime; }
  /* -- Init object from class --------------------------------------------- */
  void SysMapSwap(SysMap &smOther)
  { // Swap members
    FStreamSwap(smOther);
    StdSwap(cpMem, smOther.cpMem);
    StdSwap(sData, smOther.sData);
  }
  /* -- Assign constructor ------------------------------------------------- */
  void SysMapDeInit()
  { // De-init the map
    SysMapDeInitInternal();
    // Close the file
    FStreamClose();
    // Clear the variables
    SysMapClearVarsInternal();
    // Clear the name
    NameClear();
  }
  /* -- Constructor with just id initialisation ---------------------------- */
  SysMap(const StdStringView &strvF, const StdTimeT tC, const StdTimeT tM) :
    /* -- Initialisers------------------------------------------------------ */
    FStreamBase{ strvF },              // Open specified file
#if defined(LINUX)                     // Using Linux?
    // Note that all these zeros cause an error for other systems because
    // the structure may contain padding values so this needs to be changed to
    // fit the system being compiled with.
    sData{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
           { 0,0}, // struct timespec st_atimespec = time of last access
           {tM,0}, // struct timespec st_mtimespec = time of last modification
           {tC,0}  // struct timespec st_ctimespec = time of last status change
         },
#elif defined(MACOS)                   // Using MacOS?
    sData{ 0,      // dev_t   st_dev   = ID of device containing file
           0,      // mode_t  st_mode  = Mode of file (see below)
           0,      // nlink_t st_nlink = Number of hard links
           0,      // ino64_t st_ino   = Serial number of the file
           0,      // uid_t   st_uid   = User ID of the file
           0,      // gid_t   st_gid   = Group ID of the file
           0,      // dev_t   st_rdev  = Device ID of the file
           { 0,0}, // struct timespec st_atimespec = time of last access
           {tM,0}, // struct timespec st_mtimespec = time of last modification
           {tC,0}, // struct timespec st_ctimespec = time of last status change
           {tC,0}, // struct timespec st_birthtimespec = time of file creation
           0,      // off_t     st_size      = file size, in bytes
           0,      // blkcnt_t  st_blocks    = blocks allocated for file
           0,      // blksize_t st_blksize   = optimal blocksize for I/O
           0,      // uint32_t  st_flags     = user defined flags for file
           0,      // uint32_t  st_gen       = file generation number
           0,      // int32_t   st_lspare    = RESERVED: DO NOT USE!
           { 0,0}  // int64_t   st_qspare[2] = RESERVED: DO NOT USE!
         },
#endif                                 // End of Linux check
    cpMem(nullptr)                     // No handle initialised yet
    /* -- No code ---------------------------------------------------------- */
    {}
  /* ----------------------------------------------------------------------- */
  SysMap(SysMap &&smOther) :
    /* -- Initialisers ----------------------------------------------------- */
    FStreamBase{ StdMove(smOther) },   // Move stream over
    sData{ StdMove(smOther.sData) },   // Move file data over
    cpMem(smOther.cpMem)               // Move memory pointer over
    /* -- So other class doesn't destruct ---------------------------------- */
    { smOther.SysMapClearVarsInternal(); }
  /* -- Constructor -------------------------------------------------------- */
  explicit SysMap(const StdStringView &strvF) :
    /* -- Initialisers ----------------------------------------------------- */
    FStreamBase{ SysMapSetupFile(strvF) }, // Iniitalise file handle
    sData{ SysMapSetupInfo() },        // Initialise file data
    cpMem(SysMapSetupMemory())         // Initialise file pointer
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- Constructor -------------------------------------------------------- */
  SysMap() :
    /* -- Initialisers ----------------------------------------------------- */
    sData{},                           // No file data
    cpMem(nullptr)                     // No memory pointer
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- Destructor --------------------------------------------------------- */
  DTORHELPER(~SysMap, SysMapDeInitInternal())
};/* -- End ---------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
