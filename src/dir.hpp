/* == DIR.HPP ============================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## Reads and stores all files in the specified directory.              ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IDir {                       // Start of private module namespace
/* ------------------------------------------------------------------------- */
using namespace IError::P;             using namespace IIdent::P;
using namespace IPSplit::P;            using namespace IStd::P;
using namespace IString::P;            using namespace IToken::P;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* ------------------------------------------------------------------------- */
enum ValidResult : unsigned int        // Return values for ValidName()
{ /* ----------------------------------------------------------------------- */
  VR_OK,                               // 00: The filename is valid!
  VR_EMPTY,                            // 01: The filename is empty!
  VR_TOOLONG,                          // 02: The filename is too long!
  VR_NOROOT,                           // 03: Root directory not allowed
  VR_NODRIVE,                          // 04: Drive letter not allowed
  VR_INVDRIVE,                         // 05: Invalid drive letter
  VR_INVALID,                          // 06: Invalid trust parameter
  VR_DPRS,                             // 07: Doubleslash or pre/suffix slash
  VR_INVCHAR,                          // 08: Invalid character in part
  VR_PARENT,                           // 09: Parent directory not allowed
  VR_NOTRAILWS,                        // 10: No trailing whitespace
  VR_NOLEADWS,                         // 11: No leading whitespace
  VR_RESERVED,                         // 12: No reserved names
  VR_EXPLODE,                          // 13: String failed to explode
  /* ----------------------------------------------------------------------- */
  VR_MAX,                              // 14: Maximum number of errors
};/* ----------------------------------------------------------------------- */
enum ValidType : unsigned int          // Types for ValidName()
{ /* ----------------------------------------------------------------------- */
  VT_TRUSTED,                          // Trusted caller
  VT_UNTRUSTED,                        // Untrusted caller (chroot-like)
};/* ----------------------------------------------------------------------- */
static const class DirBase final       // Members initially private
{ /* ----------------------------------------------------------------------- */
  typedef IdList<VR_MAX> VRList;       // List of ValidName strings typedef
  const VRList     vrlStrings;         // " container
  /* -- Convert a valid result from ValidName to string ------------ */ public:
  const string_view &VNRtoStr(const ValidResult vrId) const
    { return vrlStrings.Get(vrId); }
  /* -- Default constructor ------------------------------------------------ */
  DirBase(void) :                      // No parameters
    /* -- Initialisers ----------------------------------------------------- */
    vrlStrings{{                       // Init ValidNameResult strings
      "Pathname is valid",         /*0001*/ "Empty pathname",
      "Pathname too long",         /*0203*/ "Root directory not allowed",
      "Drive letter not allowed",  /*0405*/ "Invalid drive letter",
      "Invalid trust parameter",   /*0607*/ "Double-slash or pre/suffix slash",
      "Invalid character in part", /*0809*/ "Parent directory not allowed",
      "No trailing whitespace",    /*1011*/ "No leading whitespace",
      "No reserved names",         /*1213*/ "Explode pathname failed",
    }}                                 // Finished ValidNameResult strings
    /* -- No code ---------------------------------------------------------- */
    { }
} /* ----------------------------------------------------------------------- */
*cDirBase = nullptr;                   // Assigned by main function
/* -- Check that path part character is valid ------------------------------ */
static bool DirIsValidPathPartCharacter(const char cChar)
{ // Test character
  switch(cChar)
  { // Check for invalid characters. On unix systems these characters are
    // allowed except for ':' which is problematic on MacOS but let's ban
    // them on all systems to keep cross compatibility.
    case '<': case '>': case ':': case '\"': case '\\': case '|': case '?':
    case '*': return false;
    // No control characters allowed
    default: if(cChar < ' ') return false;
  } // Success
  return true;
}
/* -- Check that path part characters are valid ---------------------------- */
static bool DirIsValidPathPartCharactersCallback(const char cChar)
  { return !DirIsValidPathPartCharacter(cChar); }
static bool DirIsValidPathPartCharacters(const string &strPart,
  const size_t stPos)
    { return !any_of(next(strPart.cbegin(), static_cast<ssize_t>(stPos)),
        strPart.cend(), DirIsValidPathPartCharactersCallback); }
static bool DirIsValidPathPartCharacters(const string &strPart)
  { return !any_of(strPart.cbegin(), strPart.cend(),
      DirIsValidPathPartCharactersCallback); }
/* -- Check that filename doesn't leave the exe directory ------------------ */
static ValidResult DirValidName(const string &strName,
  const ValidType vtId=VT_UNTRUSTED)
{ // Failed if empty string
  if(strName.empty()) return VR_EMPTY;
  // Failed if the length is longer than the maximum allowed path.
  if(strName.length() > _MAX_PATH) return VR_TOOLONG;
  // If using windows
#if defined(WINDOWS)
  // Failed if the first or last character is a space
  if(strName.front() <= 32) return VR_NOLEADWS;
  if(strName.back() <= 32) return VR_NOTRAILWS;
  // Replace backslashes with forward slashes on Windows
  const string &strChosen = PSplitBackToForwardSlashes(strName);
#else
  const string &strChosen = strName;
#endif
  // Which type
  switch(vtId)
  { // Full sandbox. Do not leave .exe directory
    case VT_UNTRUSTED:
      // Root directory not allowed
      if(strChosen.front() == cCommon->CFSlash()) return VR_NOROOT;
      // Get parts from pathname and return if empty.
      if(const Token tParts{ strChosen, cCommon->FSlash() })
      { // Get first iterator and string.
        StrVectorConstIt svciPart{ tParts.cbegin() };
        const string &strFirst = tParts.front();
        // If we have a length of 2 or more?
        if(strFirst.length() > 1)
        { // No parent directory or drive letter allowed
          if(strFirst == cCommon->TwoPeriod()) return VR_PARENT;
          if(strFirst[1] == ':') return VR_NODRIVE;
        } // Test all the characters in the first string
        if(!DirIsValidPathPartCharacters(strFirst)) return VR_INVCHAR;
        // This check will allow trailing forwardslashes
        const StrVectorConstIt svciEnd{ tParts.cend() -
          (tParts.size() >= 2 && tParts.rbegin()->empty() ? 1 : 0) };
        // Check the rest of them
        while(++svciPart != svciEnd)
        { // Get part
          const string &strPart = *svciPart;
          // Not allowed to be empty or parent directory
          if(strPart.empty()) return VR_DPRS;
          if(strPart == cCommon->TwoPeriod()) return VR_PARENT;
          // Failed first character is an invalid character.
          if(!DirIsValidPathPartCharacters(strPart)) return VR_INVCHAR;
        } // Success
        return VR_OK;
      } // Tokeniser failed (should be impossible)
      return VR_EXPLODE;
    // Trusted filename?
    case VT_TRUSTED:
      // Get parts from pathname and if was just a root directory, it's fine
      if(const Token tParts{ strChosen, cCommon->FSlash() })
      { // Get first string item and iterator.
        StrVectorConstIt svciPart{ tParts.cbegin() };
        const string &strFirst = tParts.front();
        // Check drive letter is valid
        if(strFirst.length() > 1 && strFirst[1] == ':')
        { // Get first character and make sure the drive letter is valid
          const char cFirst = strFirst.front();
          if((cFirst < 'A' || cFirst > 'Z') &&
             (cFirst < 'a' || cFirst > 'z')) return VR_INVDRIVE;
          // Test rest of characters from the second character
          if(!DirIsValidPathPartCharacters(strFirst, 2)) return VR_INVCHAR;
        } // Test all of the characters
        else if(!DirIsValidPathPartCharacters(strFirst)) return VR_INVCHAR;
        // This check will allow trailing forwardslashes
        const StrVectorConstIt svciEnd{ tParts.cend() -
          (tParts.size() >= 2 && tParts.rbegin()->empty() ? 1 : 0) };
        // Check the rest of them
        while(++svciPart != svciEnd)
        { // Get part
          const string &strPart = *svciPart;
          // Not allowed to be empty or parent directory
          if(strPart.empty()) return VR_DPRS;
          // Failed first character is an invalid character.
          if(!DirIsValidPathPartCharacters(strPart)) return VR_INVCHAR;
        } // Success
        return VR_OK;
      } // Tokeniser failed (should be impossible)
      return VR_EXPLODE;
    // Anything else invalid
    default: return VR_INVALID;
  }
}
/* -- Public typedefs ------------------------------------------------------ */
class DirItem                          // File information structure
{ /* ----------------------------------------------------------------------- */
  StdTimeT         tCreate,            // File creation time
                   tAccess,            // File access time
                   tWrite;             // File modification time
  uint64_t         uqSize,             // File size
                   uqFlags;            // Attributes (OS specific)
  /* -- Set members --------------------------------------------- */ protected:
  void Set(const StdTimeT tNCreate, const StdTimeT tNAccess,
    const StdTimeT tNWrite, const uint64_t uqNSize, const uint64_t uqNFlags)
      { tCreate = tNCreate; tAccess = tNAccess; tWrite = tNWrite;
          uqSize = uqNSize; uqFlags = uqNFlags; }
  /* -- Clear members ------------------------------------------------------ */
  void Clear(void) { tCreate = tAccess = tWrite = 0; uqSize = uqFlags = 0; }
  /* -- Get members ------------------------------------------------ */ public:
  StdTimeT Created(void) const { return tCreate; }
  StdTimeT Accessed(void) const { return tAccess; }
  StdTimeT Written(void) const { return tWrite; }
  uint64_t Size(void) const { return uqSize; }
  uint64_t Attributes(void) const { return uqFlags; }
  /* -- Default constructor ------------------------------------------------ */
  DirItem(void) :
    /* -- Initialisers ----------------------------------------------------- */
    tCreate(0),                        // Clear file creation time
    tAccess(0),                        // Clear file access time
    tWrite(0),                         // Clear file modification time
    uqSize(0),                         // Clear file size
    uqFlags(0)                         // Clear file attributes
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Copy constructor --------------------------------------------------- */
  DirItem(const DirItem &diOther) :
    /* -- Initialisers ----------------------------------------------------- */
    DirItem{ diOther.Created(), diOther.Accessed(), diOther.Written(),
             diOther.Size(), diOther.Attributes() }
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Assignment operator ------------------------------------------------ */
  DirItem operator=                    // cppcheck-suppress operatorEqVarError
    (const DirItem &diRHS) const       // False positive as copy ctor used
      { return diRHS; }                // Thinks 'uqSize' is not initialised
  /* ----------------------------------------------------------------------- */
  DirItem(const StdTimeT tNCreate, const StdTimeT tNAccess,
    const StdTimeT tNWrite, const uint64_t uqNSize, const uint64_t uqNFlags) :
    /* -- Initialisers ----------------------------------------------------- */
    tCreate(tNCreate),                 // Initialise file creation time
    tAccess(tNAccess),                 // Initialise file access time
    tWrite(tNWrite),                   // Initialise file modification time
    uqSize(uqNSize),                   // Initialise file size
    uqFlags(uqNFlags)                  // Initialise file attributes
    /* -- No code ---------------------------------------------------------- */
    { }
};/* ----------------------------------------------------------------------- */
MAPPACK_BUILD(DirEnt, const string, const DirItem) // Build DirItem map types
/* -- DirFile class -------------------------------------------------------- */
class DirFile                          // Files container class
{ /* -- Public variables --------------------------------------------------- */
  DirEntMap        demDirs, demFiles;  // Directories and files list
  /* -- Export ------------------------------------------------------------- */
  const StrSet Export(const DirEntMap &dSrc) const
  { // Write entries into a single set list and return it
    StrSet ssFiles;
    for(const DirEntMapPair &dempFile : dSrc)
      ssFiles.emplace(StdMove(dempFile.first));
    return ssFiles;
  }
  /* -- Convert to set --------------------------------------------- */ public:
  const StrSet DirsToSet(void) const { return Export(demDirs); }
  const StrSet FilesToSet(void) const { return Export(demFiles); }
  /* -- Get lists ---------------------------------------------------------- */
  const DirEntMap &GetDirs(void) const { return demDirs; }
  const DirEntMap &GetFiles(void) const { return demFiles; }
  /* -- Get lists iterators ------------------------------------------------ */
  const DirEntMapConstIt GetDirsBegin(void) const
    { return GetDirs().cbegin(); }
  const DirEntMapConstIt GetFilesBegin(void) const
    { return GetFiles().cbegin(); }
  const DirEntMapConstIt GetDirsEnd(void) const
    { return GetDirs().cend(); }
  const DirEntMapConstIt GetFilesEnd(void) const
    { return GetFiles().cend(); }
  /* -- Get elements in lists ---------------------------------------------- */
  size_t GetDirsSize(void) const { return GetDirs().size(); }
  size_t GetFilesSize(void) const { return GetFiles().size(); }
  /* -- Get if lists are empty or not -------------------------------------- */
  bool IsDirsEmpty(void) const { return GetDirs().empty(); }
  bool IsDirsNotEmpty(void) const { return !IsDirsEmpty(); }
  bool IsFilesEmpty(void) const { return GetFiles().empty(); }
  bool IsFilesNotEmpty(void) const { return !IsFilesEmpty(); }
  /* -- Default constructor ------------------------------------------------ */
  DirFile(void) { }
  /* -- Move constructor --------------------------------------------------- */
  DirFile(DirEntMap &&demNDirs, DirEntMap &&demNFiles) :
    /* -- Initialisers ----------------------------------------------------- */
    demDirs{ StdMove(demNDirs) },
    demFiles{ StdMove(demNFiles) }
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Move constructor --------------------------------------------------- */
  DirFile(DirFile &&dfOther) :
    /* -- Initialisers ----------------------------------------------------- */
    demDirs{ StdMove(dfOther.demDirs) },
    demFiles{ StdMove(dfOther.demFiles) }
    /* -- No code ---------------------------------------------------------- */
    { }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(DirFile)             // Suppress default functions for safety
};/* -- DirCore class ------------------------------------------------------ */
class DirCore :                        // System specific implementation
  /* -- Base classes ------------------------------------------------------- */
  public DirItem                       // Current item information
{ /* -- Variables -------------------------------------------------- */ public:
  string           strFile;            // Name of next file
  bool             bIsDir;             // Current item is a directory
  /* -- Setup implementation for WIN32 ------------------------------------- */
#if defined(WINDOWS)                   // WIN32 implementation
  /* -- Variables for WIN32 system -------------------------------- */ private:
  _wfinddata64_t   wfData;             // Data returned
  const intptr_t   iHandle;            // Context handle
  bool             bMore;              // If we have more files
  /* -- Process current match ---------------------------------------------- */
  void ProcessItem(void)
  { // Set if this is a directory
    bIsDir = wfData.attrib & _A_SUBDIR;
    // Set filename
    strFile = S16toUTF(wfData.name);
    // Set file information
    Set(wfData.time_create, wfData.time_access, wfData.time_write,
      static_cast<uint64_t>(wfData.size), wfData.attrib);
    // Get next item
    bMore = _wfindnext64(iHandle, &wfData) != -1;
  }
  /* -- Return if directory was opened on WIN32 system ------------- */ public:
  bool IsOpened(void) const { return iHandle != -1; }
  /* -- Prepare next file for WIN32 system --------------------------------- */
  bool GetNextFile(void)
  { // If there are no more files then we are done
    if(!bMore) return false;
    // Process the current item
    ProcessItem();
    // Success
    return true;
  }
  /* -- Constructor for WIN32 system --------------------------------------- */
  explicit DirCore(const string &strDir) :
    /* -- Initialisers ----------------------------------------------------- */
    iHandle(_wfindfirst64(UTFtoS16(strDir.empty() ? "*" :
      StrAppend(StrTrimSuffix(strDir, cCommon->CFSlash()),
        cCommon->CFSlash(), "*")).c_str(), &wfData)),
    bMore(iHandle != -1)
    /* -- Process file if there are more ----------------------------------- */
    { if(bMore) ProcessItem(); }
  /* -- Destructor for WIN32 system ---------------------------------------- */
  ~DirCore(void) { if(iHandle != -1) _findclose(iHandle); }
  /* ----------------------------------------------------------------------- */
#else                                  // Using anything but Windows?
  /* -- Private typedefs ------------------------------------------ */ private:
  // This handle will be cleaned up by closedir() when it goes out of scope! We
  // could just use decltype(closedir) but GCC fires a warning because the
  // closedir() function has attributes so this is the workaround.
  struct CloseDirFtor
    { void operator()(DIR*const dPtr) const { closedir(dPtr); } };
  typedef unique_ptr<DIR, CloseDirFtor> DirUPtr;
  /* -- Private variables -------------------------------------------------- */
  const string    strPrefix;           // Prefix for filenames with stat()
  DirUPtr         dupHandle;           // Context for opendir()
  /* -- Return if directory was opened on POSIX system ------------- */ public:
  bool IsOpened(void) const { return !!dupHandle; }
  /* ----------------------------------------------------------------------- */
# if defined(MACOS)                    // Must use readdir_r on MacOS
  /* -- Private variables ----------------------------------------- */ private:
  struct dirent   dePtr, *dePtrNext;   // Directory entry struct + next ptr
  /* -- Prepare next file for POSIX system ------------------------- */ public:
  bool GetNextFile(void)
  { // Read the filename and if failed
    if(readdir_r(dupHandle.get(), &dePtr, &dePtrNext) || !dePtrNext)
      return false;
    // Set filename
    strFile = dePtr.d_name;
    // Set next handle
    dePtrNext = &dePtr;
    // Data for stat
    struct stat sfssData;
    // Get information about the filename
    if(stat(StrAppend(strPrefix, strFile).c_str(), &sfssData) == -1)
    { // Not a directory (unknown)
      bIsDir = false;
      // Set the file data as blank
      Clear();
    } // Stat was successful?
    else
    { // Set if is directory
      bIsDir = S_ISDIR(sfssData.st_mode);
      // Set data
      Set(sfssData.st_ctime, sfssData.st_atime, sfssData.st_mtime,
        static_cast<uint64_t>(sfssData.st_size), sfssData.st_mode);
    } // Success
    return true;
  }
  /* ----------------------------------------------------------------------- */
# else                                 // POSIX implementation?
  /* -- Prepare next file for POSIX system --------------------------------- */
  bool GetNextFile(void)
  { // Read the filename and if failed
    if(dirent*const dePtr = readdir(dupHandle.get()))
    { // Data for stat
      struct stat sfssData;
      // Set filename
      strFile = dePtr->d_name;
      // Get information about the filename
      if(stat(StrAppend(strPrefix, strFile).c_str(), &sfssData) == -1)
      { // Not a directory (unknown)
        bIsDir = false;
        // Set the file data as blank
        Clear();
      } // Stat was successful?
      else
      { // Set if is directory
        bIsDir = S_ISDIR(sfssData.st_mode);
        // Set data
        Set(sfssData.st_ctime, sfssData.st_atime, sfssData.st_mtime,
          static_cast<uint64_t>(sfssData.st_size), sfssData.st_mode);
      } // Success
      return true;
    } // Failed
    return false;
  }
  /* ----------------------------------------------------------------------- */
# endif                                // End of POSIX implementation check
  /* -- Constructor for POSIX system --------------------------------------- */
  explicit DirCore(const string &strDir) :
    /* -- Initialisers ----------------------------------------------------- */
    strPrefix{ StrAppend(              // Initialise string prefix
      strDir.empty() ?                 // If requested directory is empty?
        cCommon->Period() :            // Set to scan current directory
        StrTrimSuffix(strDir,          // Trim forward-slash trailing...
          cCommon->CFSlash()),         // ...characters from directory
      cCommon->FSlash()) },            // Add our own slash at the end
    dupHandle{                         // Initialise directory handle
      opendir(strPrefix.c_str()) }     // Open the directory and store handle
    /* -- MacOS initialisers ----------------------------------------------- */
# if defined(MACOS)                    // Initialise other vars on MacOS
    /* --------------------------------------------------------------------- */
    ,dePtr{},                          // Clear last directory entry
    dePtrNext{ &dePtr }                // Set last directory entry
    /* --------------------------------------------------------------------- */
# endif                                // Initialised dirent vars on MacOS
    /* -- Unload and clear the dir handle if init and no first file -------- */
    { if(dupHandle && !GetNextFile()) dupHandle.reset(); }
  /* ----------------------------------------------------------------------- */
#endif                                 // End of system implementation check
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(DirCore)             // Suppress default functions for safety
};/* == Dir class ========================================================== */
class Dir :                            // Directory information class
  /* -- Base classes ------------------------------------------------------- */
  public DirFile                       // Files container class
{ /* -- Do scan --------------------------------------------------- */ private:
  static void RemoveEntry(DirEntMap &dfemMap, const string &strEntry)
  { // Remove specified entry
    const DirEntMapIt demiIt{ dfemMap.find(strEntry) };
    if(demiIt != dfemMap.cend()) dfemMap.erase(demiIt);
  }
  /* -- Remove current and parent directory entries ------------------------ */
  static void RemoveParentAndCurrentDirectory(DirEntMap &dfemMap)
  { // Remove "." and ".." current directory entries
    RemoveEntry(dfemMap, cCommon->Period());
    RemoveEntry(dfemMap, cCommon->TwoPeriod());
  }
  /* -- Scan with no match checking ---------------------------------------- */
  static DirFile ScanDir(const string &strDir=cCommon->Blank())
  { // Directory and file list
    DirEntMap demNDirs, demNFiles;
    // Load up the specification and return if failed
    DirCore dcInterface{ strDir };
    if(dcInterface.IsOpened())
    { // Repeat...
      do
      { // Add directory if is a directory
        if(dcInterface.bIsDir)
          demNDirs.insert({ StdMove(dcInterface.strFile), dcInterface });
        // Insert into files list
        else demNFiles.insert({ StdMove(dcInterface.strFile), dcInterface });
        // ...until no more entries
      } while(dcInterface.GetNextFile());
      // Remove '.' and '..' entries
      RemoveParentAndCurrentDirectory(demNDirs);
    } // Return list of files and directories
    return { StdMove(demNDirs), StdMove(demNFiles) };
  }
  /* -- Scan with match checking ------------------------------------------- */
  static DirFile ScanDirExt(const string &strDir, const string &strExt)
  { // Directory and file list
    DirEntMap demNDirs, demNFiles;
    // Load up the specification and return if failed
    DirCore dcInterface{ strDir };
    if(dcInterface.IsOpened())
    { // Repeat...
      do
      { // Add directory if is a directory
        if(dcInterface.bIsDir)
          demNDirs.insert({ StdMove(dcInterface.strFile), dcInterface });
        // Is a file and extension doesn't match? Ignore it
        else if(PathSplit{ dcInterface.strFile }.strExt != strExt) continue;
        // Insert into files list
        else demNFiles.insert({ StdMove(dcInterface.strFile), dcInterface });
        // ...until no more entries
      } while(dcInterface.GetNextFile());
      // Remove '.' and '..' entries
      RemoveParentAndCurrentDirectory(demNDirs);
    } // Return list of files and directories
    return { StdMove(demNDirs), StdMove(demNFiles) };
  }
  /* -- Constructor of current directory ------------------------ */ protected:
  explicit Dir(DirFile &&dfOther) : DirFile{ StdMove(dfOther) } { }
  /* -- Constructor of current directory --------------------------- */ public:
  Dir(void) : DirFile{ ScanDir() } { }
  /* -- Constructor of specified directory --------------------------------- */
  explicit Dir(const string &strDir) : DirFile{ ScanDir(strDir) } { }
  /* -- Scan specified directory for files with specified extension -------- */
  Dir(const string &strDir, const string &strExt) :
    DirFile{ ScanDirExt(strDir, strExt) } { }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(Dir)                 // Suppress default functions for safety
};/* ----------------------------------------------------------------------- */
/* -- Get current directory ------------------------------------------------ */
static const string DirGetCWD(void)
{ // On windows, we need to use unicode
#if defined(WINDOWS)
  // Storage of filename and initialise it to maximum path length
  wstring wstrDir; wstrDir.resize(_MAX_PATH);
  // Get current directory and store it in string, throw exception if error
  if(!_wgetcwd(const_cast<wchar_t*>(wstrDir.data()),
    static_cast<int>(wstrDir.capacity())))
      throw runtime_error{ "getcwd() failed!" };
  // Resize and recover memory
  wstrDir.resize(wcslen(wstrDir.c_str()));
  // Return directory replacing backslashes for forward slashes
  return PSplitBackToForwardSlashes(WS16toUTF(wstrDir));
#else
  // Storage of filename and initialise it to maximum path length
  string strDir; strDir.resize(_MAX_PATH);
  // Get current directory and store it in string, throw exception if error
  if(!getcwd(const_cast<char*>(strDir.data()), strDir.capacity()))
    throw runtime_error{ "getcwd() failed!" };
  // Resize and recover memory
  strDir.resize(strlen(strDir.c_str()));
  strDir.shrink_to_fit();
  // Return directory
  return strDir;
#endif
}
/* == Set current directory ================================================ */
static bool DirSetCWD(const string &strDirectory)
{ // Ignore if empty
  if(strDirectory.empty()) return false;
  // Process is different on win32 with having drive letters
#if defined(WINDOWS)
  // Get first character because it needs casting
  const unsigned char &ucD = strDirectory.front();
  // Set drive first if specified
  if(strDirectory.length() >= 3 && strDirectory[1] == ':' &&
     (strDirectory[2] == '\\' || strDirectory[2] != cCommon->CFSlash()) &&
       _chdrive((toupper(ucD) - 'A') + 1) < 0) return false;
#endif
  // Set current directory and return false if there is a problem
  return !StdChDir(strDirectory);
}
/* -- Make a directory ----------------------------------------------------- */
static bool DirMkDir(const string &strDir) { return !StdMkDir(strDir); }
/* -- Remove a directory --------------------------------------------------- */
static bool DirRmDir(const string &strDir) { return !StdRmDir(strDir); }
/* -- Make a directory and all it's interim components --------------------- */
static bool DirMkDirEx(const string &strDir)
{ // Break apart directory parts
  const PathSplit psParts{ strDir };
  // Break apart so we can check the directories. Will always be non-empty.
  if(const Token tParts{ StrAppend(psParts.strDir, psParts.strFileExt),
    cCommon->FSlash() })
  { // This will be the string that wile sent to mkdir multiple times
    // gradually.
    ostringstream osS; osS << psParts.strDrive;
    // Get the first item and if it is not empty?
    const string &strFirst = tParts.front();
    if(!strFirst.empty())
    { // Make the directory if isn't the drive and return failure if the
      // directory doesn't already exist
      if(!DirMkDir(strFirst) && StdIsNotError(EEXIST)) return false;
      // Move first item. It will be empty if directory started with a slash
      osS << StdMove(strFirst);
    } // If there are more directories?
    if(tParts.size() >= 2)
    { // Create all the other directories
      for(StrVectorConstIt svI{ next(tParts.cbegin(), 1) };
                           svI != tParts.cend();
                         ++svI)
      { // Append next directory
        osS << cCommon->CFSlash() << StdMove(*svI);
        // Make the directory and if failed and it doesn't exist return error
        if(!DirMkDir(osS.str()) && StdIsNotError(EEXIST)) return false;
      }
    } // Success
    return true;
  } // Tokeniser failed
  return false;
}
/* -- Remove a directory and all it's interim components ------------------- */
static bool DirRmDirEx(const string &strDir)
{ // Break apart directory parts
  const PathSplit psParts{ strDir };
  // Break apart so we can check the directories. Will always be non-empty.
  Token tParts{ StrAppend(psParts.strDir, psParts.strFileExt),
    cCommon->FSlash() };
  // Get the first item and if it is not empty?
  while(!tParts.empty())
  { // This will be the string that wile sent to mkdir multiple times
    // gradually. Do not try to construct the oss with the drive string because
    // it won't work and thats not how the constructor works it seems!
    ostringstream osS; osS << psParts.strDrive;
    // Get the first item and if it is not empty?
    const string &strFirst = tParts.front();
    if(!strFirst.empty()) osS << strFirst;
    // If there are more directories? Build directory structure
    if(tParts.size() >= 2)
      for(StrVectorConstIt svI{ next(tParts.begin(), 1) };
                           svI != tParts.end();
                         ++svI)
        osS << cCommon->CFSlash() << *svI;
    // Make the directory and if failed and it doesn't exist return error
    if(!DirRmDir(osS.str()) && StdIsNotError(EEXIST)) return false;
    // Remove the last item
    tParts.pop_back();
  } // Success
  return true;
}
/* -- Delete a file -------------------------------------------------------- */
static bool DirFileUnlink(const string &strFile)
  { return !StdUnlink(strFile); }
/* -- Get file size - ------------------------------------------------------ */
static int DirFileSize(const string &strFile, StdFStatStruct &sfssData)
  { return StdFStat(strFile, &sfssData) ? StdGetError() : 0; }
/* -- True if specified file has the specified mode ------------------------ */
static bool DirFileHasMode(const string &strFile, const int iMode,
  const int iNegate)
{ // Get file information and and if succeeded?
  StdFStatStruct sfssData;
  if(!DirFileSize(strFile, sfssData))
  { // If file attributes have specified mode then success
    if((sfssData.st_mode ^ iNegate) & iMode) return true;
    // Set error number
    StdSetError(ENOTDIR);
  } // Failed
  return false;
}
/* -- True if specified file is actually a directory ----------------------- */
static bool DirLocalDirExists(const string &strFile)
  { return DirFileHasMode(strFile, _S_IFDIR, 0); }
/* -- True if specified file is actually a file ---------------------------- */
static bool DirLocalFileExists(const string &strFile)
  { return DirFileHasMode(strFile, _S_IFDIR, -1); }
/* -- Readable or writable? ------- Check if file is readable or writable -- */
static bool DirCheckFileAccess(const string &strFile, const int iFlag)
  { return !StdAccess(strFile, iFlag); }
/* -- True if specified file exists and is readable ------------------------ */
static bool DirIsFileReadable(const string &strFile)
  { return DirCheckFileAccess(strFile, R_OK); }
/* -- True if specified file exists and is readable and writable ----------- */
static bool DirIsFileReadWriteable(const string &strFile)
  { return DirCheckFileAccess(strFile, R_OK|W_OK); }
/* -- True if specified file exists and is writable ------------------------ */
static bool DirIsFileWritable(const string &strFile)
  { return DirCheckFileAccess(strFile, W_OK); }
/* -- True if specified file exists and is executable ---------------------- */
static bool DirIsFileExecutable(const string &strFile)
  { return DirCheckFileAccess(strFile, X_OK); }
/* -- True if specified file or directory exists --------------------------- */
static bool DirLocalResourceExists(const string &strFile)
   { return DirCheckFileAccess(strFile, F_OK); }
/* -- Rename file ---------------------------------------------------------- */
static bool DirFileRename(const string &strFrom, const string &strTo)
  { return !StdRename(strFrom, strTo); }
/* -- Check that filename is valid and throw on error ---------------------- */
static void DirVerifyFileNameIsValid(const string &strFile)
{ // Throw error if invalid name
  if(const ValidResult vrId = DirValidName(strFile))
    XC("Filename is invalid!",
       "File",     strFile,
       "Reason",   cDirBase->VNRtoStr(vrId),
       "ReasonId", vrId);
}
/* ------------------------------------------------------------------------- */
class DirSaver
{ /* ----------------------------------------------------------------------- */
  const string strCWD;                 // Saved current directory
  /* --------------------------------------------------------------- */ public:
  explicit DirSaver(const string &strNWD) :
    /* --------------------------------------------------------------------- */
    strCWD{ DirGetCWD() }              // Save current working directory
    /* --------------------------------------------------------------------- */
    { DirSetCWD(strNWD); }
  /* ----------------------------------------------------------------------- */
  DirSaver(void) :                     // Save current working directory
    /* --------------------------------------------------------------------- */
    strCWD{ DirGetCWD() }
    /* --------------------------------------------------------------------- */
    { }
  /* ----------------------------------------------------------------------- */
  ~DirSaver(void) noexcept(false)
    /* --------------------------------------------------------------------- */
    { DirSetCWD(strCWD); }
};/* ----------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
