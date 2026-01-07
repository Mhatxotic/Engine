/* == WINMAP.HPP =========================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## This is a Windows specific module that handles mapping files in     ## **
** ## memory. It will be derived by the FileMap class.                    ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* == Windows file mapping class =========================================== */
class SysMap :                         // Members initially private
  /* -- Base classes ------------------------------------------------------- */
  public virtual Ident                 // File name to the map
{ /* -- Private typedefs --------------------------------------------------- */
  typedef array<StdTimeT,2> TwoTime;   // For holding two unix timestamps
  /* -- Private variables (don't change order!) ---------------------------- */
  HANDLE           hFile;              // Handle to the file
  uint64_t         ullSize;            // Size of file
  HANDLE           hMap;               // Handle to the file map
  char            *cpMem;              // Handle to memory
  TwoTime          atTime;             // File times (0=creation,1=time)
  /* -- De-init the file map ----------------------------------------------- */
  void SysMapDeInitInternal()
  { // Have mapped file in memory and if it is not a zero sized map then unmap
    // the file
    if(SysMapIsAvailable() && SysMapIsNotEmpty() &&
      !UnmapViewOfFile(SysMapGetMemory<LPCVOID>()))
        cLog->LogWarningExSafe("System failed to unmap view of '$': $!",
          IdentGet(), SysError());
    // Have map handle? Unmap the file and clear the pointer
    if(hMap && !CloseHandle(hMap))
      cLog->LogWarningExSafe("System failed to close file mapping for '$': $!",
        IdentGet(), SysError());
    // Have file handle? Unmap the file and clear the pointer
    if(hFile != INVALID_HANDLE_VALUE && !CloseHandle(hFile))
      cLog->LogWarningExSafe("System failed to close file '$': $!",
        IdentGet(), SysError());
  }
  /* -- Clear variables ---------------------------------------------------- */
  void SysMapClearVarsInternal()
  { // Clear mapped memory to file data
    cpMem = nullptr;
    // Clear handle to map
    hMap = nullptr;
    // Clear handle to file
    hFile = INVALID_HANDLE_VALUE;
  }
  /* -- Get file handle ---------------------------------------------------- */
  HANDLE SysMapSetupFile()
  { // Open file and return if opened
    HANDLE hF = CreateFile(UTFtoS16(IdentGetData()).data(), GENERIC_READ,
      FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr);
    if(hF != INVALID_HANDLE_VALUE) return hF;
    // Failed
    XCS("Open file for mapping failed!", "File", IdentGet());
  }
  /* -- Get size of file --------------------------------------------------- */
  uint64_t SysMapSetupSize()
  { // Get file size and throw exception if failed
    LARGE_INTEGER liSize;
    if(GetFileSizeEx(hFile, &liSize))
      return *reinterpret_cast<uint64_t*>(&liSize);
    XCS("Failed to query file size!", "File", IdentGet(), "Handle", hFile);
  }
  /* -- Get handle to map -------------------------------------------------- */
  HANDLE SysMapSetupMap()
  {  // The file is not empty?
    if(SysMapGetSize())
    { // Create the file mapping and return it if successful
      if(HANDLE hM = CreateFileMapping(hFile,
        nullptr, PAGE_READONLY, 0, 0, nullptr))
          return hM;
      XCS("Create file mapping failed!", "File", IdentGet(), "Handle", hFile);
    } // Empty so close the file. Whats the point in keeping it open?
    if(!CloseHandle(hFile))
      XCS("Failed to close empty file!", "File", IdentGet(), "Handle", hFile);
    // Reset the handle
    hFile = INVALID_HANDLE_VALUE;
    // Return nothing
    return INVALID_HANDLE_VALUE;
  }
  /* -- Get pointer to memory ---------------------------------------------- */
  char *SysMapSetupMemory()
  { // Return a blank string if file is empty
    if(!ullSize) return const_cast<char*>(cCommon->CommonCBlank());
    // Get pointer to mapped memory and return it if successful
    if(char*const cpM =
      reinterpret_cast<char*>(MapViewOfFile(hMap, FILE_MAP_READ, 0, 0, 0)))
        return cpM;
    XCS("Map view of file failed!", "File", IdentGet(), "Handle", hMap);
  }
  /* -- Get file creation time --------------------------------------------- */
  TwoTime SysMapSetupTimes()
  { // Get file times and return filetime if successful
    FILETIME ftC, ftM;
    if(GetFileTime(hFile, &ftC, nullptr, &ftM))
      return { UtilBruteCast<StdTimeT>(ftC) / 100000000,
               UtilBruteCast<StdTimeT>(ftM) / 100000000 };
    XCS("Failed to query file creation time!",
      "File", IdentGet(), "Handle", hFile);
  }
  /* -- Get members ------------------------------------------------ */ public:
  template<typename RT=char>RT *SysMapGetMemory() const
    { return reinterpret_cast<RT*>(cpMem); }
  bool SysMapIsEmpty() const { return cpMem == cCommon->CommonCBlank(); }
  bool SysMapIsNotEmpty() const { return !SysMapIsEmpty(); }
  bool SysMapIsAvailable() const { return !!SysMapGetMemory(); }
  bool SysMapIsNotAvailable() const { return !SysMapIsAvailable(); }
  uint64_t SysMapGetSize() const { return ullSize; }
  StdTimeT SysMapGetCreation() const { return atTime.front(); }
  StdTimeT SysMapGetModified() const { return atTime.back(); }
  /* -- Init object from class --------------------------------------------- */
  void SysMapSwap(SysMap &smOther)
  { // Swap members
    swap(hFile, smOther.hFile);
    swap(hMap, smOther.hMap);
    swap(cpMem, smOther.cpMem);
    swap(ullSize, smOther.ullSize);
    atTime.swap(smOther.atTime);
  }
  /* -- Assign constructor ------------------------------------------------- */
  void SysMapDeInit()
  { // De-init the map
    SysMapDeInitInternal();
    // Clear the variables
    SysMapClearVarsInternal();
    // Clear the name
    IdentClear();
  }
  /* -- Constructor with just id initialisation ---------------------------- */
  SysMap(const string &strIn, const StdTimeT tC, const StdTimeT tM) :
    /* -- Initialisers ----------------------------------------------------- */
    Ident{ strIn },                    // Initialise file name
    hFile(INVALID_HANDLE_VALUE),       // No file handle
    ullSize(0),                        // No file size
    hMap(nullptr),                     // No map handle
    cpMem(nullptr),                    // No memory pointer
    atTime{ tC, tM }                   // Set file times
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- Constructor on standby --------------------------------------------- */
  SysMap() :
    /* -- Initialisers ----------------------------------------------------- */
    hFile(INVALID_HANDLE_VALUE),       // No file handle
    ullSize(0),                        // No size
    hMap(nullptr),                     // No map handle
    cpMem(nullptr),                    // No pointer to memory
    atTime{ 0, 0 }                     // No file times
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- Move constructor --------------------------------------------------- */
  SysMap(SysMap &&smOther) :
    /* -- Initialisers ----------------------------------------------------- */
    Ident{ StdMove(smOther) },         // Move other identifier
    hFile(smOther.hFile),              // Move other file handle
    ullSize(smOther.ullSize),          // Move other size
    hMap(smOther.hMap),                // Move other file map
    cpMem(smOther.cpMem),              // Move other memory pointer
    atTime{ StdMove(smOther.atTime) }  // Move other file times
    /* -- Clear other variables -------------------------------------------- */
    { smOther.SysMapClearVarsInternal(); }
  /* -- Constructor with actual initialisation ----------------------------- */
  explicit SysMap(const string &strIn) :
    /* -- Initialisers ----------------------------------------------------- */
    Ident{ strIn },                    // Set file name
    hFile(SysMapSetupFile()),          // Get file handle from file on disk
    ullSize(SysMapSetupSize()),        // Get file size on disk
    hMap(SysMapSetupMap()),            // Get map handle
    cpMem(SysMapSetupMemory()),        // Get pointer to file in memory
    atTime{ SysMapSetupTimes() }       // Get times of file
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- Destructor --------------------------------------------------------- */
  DTORHELPER(~SysMap, SysMapDeInitInternal())
};/* -- End ---------------------------------------------------------------- */
/* == EoF =========================================================== EoF == */
