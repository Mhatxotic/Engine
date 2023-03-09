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
  public Ident                         // File name to the map
{ /* -- Private typedefs --------------------------------------------------- */
  typedef array<StdTimeT,2> TwoTime;   // For holding two unix timestamps
  /* -- Private variables (don't change order!) ---------------------------- */
  HANDLE           hFile;              // Handle to the file
  uint64_t         qSize;              // Size of file
  HANDLE           hMap;               // Handle to the file map
  char            *cpMem;              // Handle to memory
  TwoTime          atTime;             // File times (0=creation,1=time)
  /* -- De-init the file map ----------------------------------------------- */
  void SysMapDeInitInternal(void)
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
  void SysMapClearVarsInternal(void)
  { // Clear mapped memory to file data
    cpMem = nullptr;
    // Clear handle to map
    hMap = nullptr;
    // Clear handle to file
    hFile = INVALID_HANDLE_VALUE;
  }
  /* -- Get file handle ---------------------------------------------------- */
  HANDLE SysMapSetupFile(void)
  { // Open file and return if opened
    HANDLE hF = CreateFile(UTFtoS16(IdentGetCStr()).c_str(), GENERIC_READ,
      FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr);
    if(hF != INVALID_HANDLE_VALUE) return hF;
    // Failed
    XCS("Open file for mapping failed!", "File", IdentGet());
  }
  /* -- Get size of file --------------------------------------------------- */
  uint64_t SysMapSetupSize(void)
  { // Get file size and throw exception if failed
    LARGE_INTEGER liSize;
    if(GetFileSizeEx(hFile, &liSize))
      return *reinterpret_cast<uint64_t*>(&liSize);
    XCS("Failed to query file size!", "File", IdentGet(), "Handle", hFile);
  }
  /* -- Get handle to map -------------------------------------------------- */
  HANDLE SysMapSetupMap(void)
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
  char *SMSetupMemory(void)
  { // Return a blank string if file is empty
    if(!qSize) return const_cast<char*>(cCommon->CBlank());
    // Get pointer to mapped memory and return it if successful
    if(char*const cpM =
      reinterpret_cast<char*>(MapViewOfFile(hMap, FILE_MAP_READ, 0, 0, 0)))
        return cpM;
    XCS("Map view of file failed!", "File", IdentGet(), "Handle", hMap);
  }
  /* -- Get file creation time --------------------------------------------- */
  TwoTime SysMapSetupTimes(void)
  { // Get file times and return filetime if successful
    FILETIME ftC, ftM;
    if(GetFileTime(hFile, &ftC, nullptr, &ftM))
      return { UtilBruteCast<StdTimeT>(ftC) / 100000000,
               UtilBruteCast<StdTimeT>(ftM) / 100000000 };
    XCS("Failed to query file creation time!",
      "File", IdentGet(), "Handle", hFile);
  }
  /* -- Get members ------------------------------------------------ */ public:
  template<typename RT=char>RT *SysMapGetMemory(void) const
    { return reinterpret_cast<RT*>(cpMem); }
  bool SysMapIsEmpty(void) const { return cpMem == cCommon->CBlank(); }
  bool SysMapIsNotEmpty(void) const { return !SysMapIsEmpty(); }
  bool SysMapIsAvailable(void) const { return !!SysMapGetMemory(); }
  bool SysMapIsNotAvailable(void) const { return !SysMapIsAvailable(); }
  uint64_t SysMapGetSize(void) const { return qSize; }
  StdTimeT SysMapGetCreation(void) const { return atTime.front(); }
  StdTimeT SysMapGetModified(void) const { return atTime.back(); }
  /* -- Init object from class --------------------------------------------- */
  void SysMapSwap(SysMap &smOther)
  { // Swap members
    IdentSwap(smOther);
    swap(hFile, smOther.hFile);
    swap(hMap, smOther.hMap);
    swap(cpMem, smOther.cpMem);
    swap(qSize, smOther.qSize);
    atTime.swap(smOther.atTime);
  }
  /* -- Assign constructor ------------------------------------------------- */
  void SysMapDeInit(void)
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
    qSize(0),                          // No file size
    hMap(nullptr),                     // No map handle
    cpMem(nullptr),                    // No memory pointer
    atTime{ tC, tM }                   // Set file times
    /* --------------------------------------------------------------------- */
    { }                                // Do nothing else
  /* -- Constructor on standby --------------------------------------------- */
  SysMap(void) :
    /* -- Initialisers ----------------------------------------------------- */
    hFile(INVALID_HANDLE_VALUE),       // No file handle
    qSize(0),                          // No size
    hMap(nullptr),                     // No map handle
    cpMem(nullptr),                    // No pointer to memory
    atTime{ 0, 0 }                     // No file times
    /* --------------------------------------------------------------------- */
    { }                                // Do nothing else
  /* -- Move constructor --------------------------------------------------- */
  SysMap(SysMap &&smOther) :
    /* -- Initialisers ----------------------------------------------------- */
    Ident{ StdMove(smOther) },         // Move other identifier
    hFile(smOther.hFile),              // Move other file handle
    qSize(smOther.qSize),              // Move other size
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
    qSize(SysMapSetupSize()),          // Get file size on disk
    hMap(SysMapSetupMap()),            // Get map handle
    cpMem(SMSetupMemory()),            // Get pointer to file in memory
    atTime{ SysMapSetupTimes() }       // Get times of file
    /* --------------------------------------------------------------------- */
    { }                                // Do nothing else
  /* -- Destructor --------------------------------------------------------- */
  ~SysMap(void) { SysMapDeInitInternal(); }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(SysMap)              // Suppress default functions for safety
};/* -- End ---------------------------------------------------------------- */
/* == EoF =========================================================== EoF == */
