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
/* -- Dependencies --------------------------------------------------------- */
using namespace ICommon::P;            using namespace ICVarDef::P;
using namespace IError::P;             using namespace ILookupArray::P;
using namespace IPSplit::P;            using namespace ISerial::P;
using namespace IStd::P;               using namespace IStdLib::P;
using namespace IString::P;            using namespace IToken::P;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* ------------------------------------------------------------------------- */
enum ValidResult : unsigned            // Return values for ValidName()
{ /* ----------------------------------------------------------------------- */
  VR_OK,                               // [00] The filename is valid!
  VR_EMPTY,                            // [01] The filename is empty!
  VR_TOOLONG,                          // [02] The filename is too long!
  VR_NOROOT,                           // [03] Root directory not allowed
  VR_NODRIVE,                          // [04] Drive letter not allowed
  VR_INVDRIVE,                         // [05] Invalid drive letter
  VR_INVALID,                          // [06] Invalid trust parameter
  VR_RESERVED,                         // [07] No reserved names
  VR_INVCHAR,                          // [08] Invalid character in part
  VR_CSUBDIR,                          // [09] As above but as a a sub-dir
  VR_CURRENT,                          // [10] Current directory not allowed
  VR_EXPLODE,                          // [11] String failed to explode
  VR_NOTRAILWS,                        // [12] No trailing whitespace
  VR_NOLEADWS,                         // [13] No leading whitespace
  /* ----------------------------------------------------------------------- */
  VR_MAX                               // [14] Maximum number of errors
};/* ----------------------------------------------------------------------- */
enum ValidType : unsigned              // Types for ValidName()
{ /* ----------------------------------------------------------------------- */
  VT_NOTHING,                          // [0] Absolutely no safety whatsoever
  VT_TRUSTED,                          // [1] Trusted caller
  VT_UNTRUSTED,                        // [2] Untrusted caller (chroot-like)
  /* ----------------------------------------------------------------------- */
  VT_MAX                               // [3] Maximum trust modes
};/* ----------------------------------------------------------------------- */
class DirBase;                         // Prototype to class
static DirBase *cDirBase = nullptr;    // Pointer to global class
class DirBase                          // Members initially private
{ /* -- Private typedefs --------------------------------------------------- */
  using VRList = LookupArray<VR_MAX>;  // List of ValidName strings typedef
  /* -- Private variables -------------------------------------------------- */
  const VRList     vrlStrings;         // " container
  const StrVUSet   svusReserved;       // Reserved names
  ValidType        vtMode;             // Default safety mode
  /* -- Convert a valid result from ValidName to string ------------ */ public:
  const StdStringView &DirBaseVNRtoStr(const ValidResult vrId) const
    { return vrlStrings.Get(vrId); }
  /* -- Return if a name is reserved --------------------------------------- */
  bool DirBaseIsReservedName(const StdStringView &strvName) const
    { return svusReserved.contains(strvName); }
  /* -- Return safety mode ------------------------------------------------- */
  ValidType DirBaseGetSafetyMode() const { return vtMode; }
  /* -- Default constructor ------------------------------------- */ protected:
  DirBase() :
    /* -- Initialisers ----------------------------------------------------- */
    vrlStrings{{                       // Init ValidNameResult strings
      "Pathname is valid",         /*0001*/ "Empty pathname denied",
      "Pathname too long",         /*0203*/ "Root directory denied",
      "Drive letter denied",       /*0405*/ "Invalid drive letter",
      "Invalid trust parameter",   /*0607*/ "Reserved name denied",
      "Invalid character in part", /*0809*/ "Current sub-directory denied",
      "Current directory denied",  /*1011*/ "Explode pathname failed",
      "Trailing whitespace denied",/*1213*/ "Leading whitespace denied",
    }},                                 // Finished ValidNameResult strings
    svusReserved{{                      // Init reserved names
      "aux",  "auX",  "aUx",  "aUX",  "Aux",  "AuX",  "AUx",  "AUX",
      "com1", "coM1", "cOm1", "cOM1", "Com1", "CoM1", "COm1", "COM1",
      "com2", "coM2", "cOm2", "cOM2", "Com2", "CoM2", "COm2", "COM2",
      "com3", "coM3", "cOm3", "cOM3", "Com3", "CoM3", "COm3", "COM3",
      "com4", "coM4", "cOm4", "cOM4", "Com4", "CoM4", "COm4", "COM4",
      "com5", "coM5", "cOm5", "cOM5", "Com5", "CoM5", "COm5", "COM5",
      "com6", "coM6", "cOm6", "cOM6", "Com6", "CoM6", "COm6", "COM6",
      "com7", "coM7", "cOm7", "cOM7", "Com7", "CoM7", "COm7", "COM7",
      "com8", "coM8", "cOm8", "cOM8", "Com8", "CoM8", "COm8", "COM8",
      "com9", "coM9", "cOm9", "cOM9", "Com9", "CoM9", "COm9", "COM9",
      "con",  "coN",  "cOn",  "cON",  "Con",  "CoN",  "COn",  "CON",
      "lpt1", "lpT1", "lPt1", "lPT1", "Lpt1", "LpT1", "LPt1", "LPT1",
      "lpt2", "lpT2", "lPt2", "lPT2", "Lpt2", "LpT2", "LPt2", "LPT2",
      "lpt3", "lpT3", "lPt3", "lPT3", "Lpt3", "LpT3", "LPt3", "LPT3",
      "lpt4", "lpT4", "lPt4", "lPT4", "Lpt4", "LpT4", "LPt4", "LPT4",
      "lpt5", "lpT5", "lPt5", "lPT5", "Lpt5", "LpT5", "LPt5", "LPT5",
      "lpt6", "lpT6", "lPt6", "lPT6", "Lpt6", "LpT6", "LPt6", "LPT6",
      "lpt7", "lpT7", "lPt7", "lPT7", "Lpt7", "LpT7", "LPt7", "LPT7",
      "lpt8", "lpT8", "lPt8", "lPT8", "Lpt8", "LpT8", "LPt8", "LPT8",
      "lpt9", "lpT9", "lPt9", "lPT9", "Lpt9", "LpT9", "LPt9", "LPT9",
      "nul",  "nuL",  "nUl",  "nUL",  "Nul",  "NuL",  "NUl",  "NUL",
      "prn",  "prN",  "pRn",  "pRN",  "Prn",  "PrN",  "PRn",  "PRN",
      cCommon->CommonTwoPeriod()
    }},
    vtMode(VT_UNTRUSTED)               // Default mode is 'untrusted' access
    /* -- Set pointer to global class -------------------------------------- */
    { cDirBase = this; }
  /* -- Default safety mode ---------------------------------------- */ public:
  CVarReturn SetDefaultSafetyMode(const ValidType vtNMode)
  { // Deny if invalid mode
    if(vtNMode >= VT_MAX) return DENY;
    // Set new safety mode
    vtMode = vtNMode;
    // Accepted new setting
    return ACCEPT;
  }
};/* ----------------------------------------------------------------------- */
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
static bool DirIsValidPathPartCharacters(const StdStringView &strvPart,
  const size_t stPos)
    { return !StdAnyOf(StdNext(strvPart.cbegin(), static_cast<ssize_t>(stPos)),
        strvPart.cend(), DirIsValidPathPartCharactersCallback); }
static bool DirIsValidPathPartCharacters(const StdStringView &strvPart)
  { return !StdAnyOf(strvPart.cbegin(), strvPart.cend(),
      DirIsValidPathPartCharactersCallback); }
/* -- Valid Windows drive letter ------------------------------------------- */
static bool DirIsValidDrive(const char cFirst)
  { return (cFirst >= 'A' && cFirst <= 'Z') ||
           (cFirst >= 'a' && cFirst <= 'z'); }
/* -- Check that filename doesn't leave the exe directory ------------------ */
static ValidResult DirValidName(const StdStringView &strvName,
  const ValidType vtId)
{ // Failed if empty string
  if(strvName.empty()) return VR_EMPTY;
  // Failed if the length is longer than the maximum allowed path.
  if(strvName.size() > _MAX_PATH) return VR_TOOLONG;
  // If using windows? Replace backslashes with forward slashes.
  const StdString &strChosen{
#if defined(WINDOWS)
    PSplitBackToForwardSlashes(strvName)
#else
    StdString{ strvName }
#endif
  }; // Which type
  switch(vtId)
  { // Full sandbox. Do not leave .exe directory.
    case VT_UNTRUSTED:
    { // Failed if the first or last character is a space
      if(strvName.front() <= ' ') return VR_NOLEADWS;
      if(strvName.back() <= ' ') return VR_NOTRAILWS;
      // Root directory not allowed.
      if(strChosen.front() == '/') return VR_NOROOT;
      // Path length is one byte?
      if(strChosen.size() == 1)
      { // Disallow current directory if it is a dot.
        if(strChosen.front() == '.') return VR_CURRENT;
      } // Disallow Windows drive.
      else if(strChosen[1] == ':')
        return DirIsValidDrive(strChosen.front()) ? VR_NODRIVE : VR_INVDRIVE;
      // Get parts from pathname and compare size
      const TokenStrView tsvParts{ strChosen, cCommon->CommonFSlashV() };
      switch(tsvParts.size())
      { // Tokeniser failed (should be impossible)
        case 0: return VR_EXPLODE;
        // Two parts or more? (fall through to case 1 underneath)
        default:
        { // Skip trailing forward-slash if needed
          TokenStrViewConstIt tsvciPart
            { tsvParts.cend() - (tsvParts.rbegin()->empty() ? 2 : 1) };
          // Enumerate parts from chosen end to start plus one.
          while(tsvciPart != tsvParts.cbegin())
          { // Get part string
            const StdStringView &strvPart = *tsvciPart;
            // Test the length of the first path part
            switch(strvPart.size())
            { // No length? Return empty
              case 0: return VR_EMPTY;
              // One character. No dot allowed.
              case 1: if(strvPart.front() == '.') return VR_CSUBDIR; break;
              // Two or three characters or more? Checks are fine
              default: break;
            } // Failed first character is an invalid character.
            if(!DirIsValidPathPartCharacters(strvPart)) return VR_INVCHAR;
            // Check for reserved names.
            if(cDirBase->DirBaseIsReservedName(strvPart)) return VR_RESERVED;
            // Go backwards
            --tsvciPart;
          } // Fall through to check first string.
          [[fallthrough]];
        } // One part?
        case 1:
        { // Get first string
          const StdStringView &strvFirst = tsvParts.front();
          // Test all the characters in the first string
          if(!DirIsValidPathPartCharacters(strvFirst)) return VR_INVCHAR;
          // Check for reserved names. Only Windows has reserved names but
          // we'll prevent them on MacOS and Linux too to prevent problems
          // being passed over to Windows.
          if(cDirBase->DirBaseIsReservedName(strvFirst)) return VR_RESERVED;
          // Success if the first entry in the path isn't a dot
          return strvFirst.size() != 1 ||
                 strvFirst.front() != '.' ? VR_OK : VR_CURRENT;
        }
      }
    } // Trusted filename?
    case VT_TRUSTED:
    { // Get parts from pathname and compare size
      const TokenStrView tsvParts{ strChosen, cCommon->CommonFSlashV() };
      switch(tsvParts.size())
      { // Tokeniser failed (should be impossible)
        case 0: return VR_EXPLODE;
        // Two parts or more? (fall through to case 1 underneath)
        default:
        { // Skip trailing forward-slash if needed
          TokenStrViewConstIt tsvciPart{ tsvParts.cend() -
            (tsvParts.rbegin()->empty() ? 2 : 1) };
          // Enumerate parts from chosen end to start plus one.
          while(tsvciPart != tsvParts.cbegin())
          { // Get part string
            const StdStringView &strvPart = *tsvciPart;
            // Not allowed to be empty or parent directory
            if(strvPart.empty()) return VR_EMPTY;
            // Failed first character is an invalid character.
            if(!DirIsValidPathPartCharacters(strvPart)) return VR_INVCHAR;
            // Go backwards
            --tsvciPart;
          } // Fall through to check first string.
          [[fallthrough]];
        } // One part?
        case 1:
        { // Get first string
          const StdStringView &strvFirst = tsvParts.front();
          // Check drive letter is valid
          if(strvFirst.size() > 1 && strvFirst[1] == ':')
          { // Get first character and make sure the drive letter is valid
            if(!DirIsValidDrive(strvFirst.front())) return VR_INVDRIVE;
            // Test rest of characters from the second character
            if(!DirIsValidPathPartCharacters(strvFirst, 2)) return VR_INVCHAR;
          } // Test all of the characters
          else if(!DirIsValidPathPartCharacters(strvFirst)) return VR_INVCHAR;
        } // Success
        return VR_OK;
      }
    } // Disabled? No checks whatsoever
    case VT_NOTHING: return VR_OK;
    // Anything else invalid
    default: return VR_INVALID;
  }
}
/* -- Check that filename doesn't leave the exe directory ------------------ */
static ValidResult DirValidName(const StdStringView &strName)
  { return DirValidName(strName, cDirBase->DirBaseGetSafetyMode()); }
/* -- Public typedefs ------------------------------------------------------ */
class DirItem                          // File information structure
{ /* ----------------------------------------------------------------------- */
  StdTimeT         tCreate,            // File creation time
                   tAccess,            // File access time
                   tWrite;             // File modification time
  uint64_t         ullId,              // File order number
                   ullSize,            // File size
                   ullFlags;           // Attributes (OS specific)
  /* -- Set members --------------------------------------------- */ protected:
  void Set(const StdTimeT tNCreate, const StdTimeT tNAccess,
    const StdTimeT tNWrite, const uint64_t ullNId, const uint64_t ullNSize,
    const uint64_t ullNFlags)
  { tCreate = tNCreate; tAccess = tNAccess; tWrite = tNWrite;
    ullId = ullNId; ullSize = ullNSize; ullFlags = ullNFlags; }
  /* -- Clear members ------------------------------------------------------ */
  void Clear() { tCreate = tAccess = tWrite = 0; ullSize = ullFlags = 0; }
  /* -- Get members ------------------------------------------------ */ public:
  StdTimeT Created() const { return tCreate; }
  StdTimeT Accessed() const { return tAccess; }
  StdTimeT Written() const { return tWrite; }
  uint64_t Id() const { return ullId; }
  uint64_t Size() const { return ullSize; }
  uint64_t Attributes() const { return ullFlags; }
  /* -- Default constructor ------------------------------------------------ */
  DirItem() :
    /* -- Initialisers ----------------------------------------------------- */
    tCreate(0),                        // Clear file creation time
    tAccess(0),                        // Clear file access time
    tWrite(0),                         // Clear file modification time
    ullId(0),                          // Clear file id
    ullSize(0),                        // Clear file size
    ullFlags(0)                        // Clear file attributes
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- Copy constructor --------------------------------------------------- */
  DirItem(const DirItem &diOther) :
    /* -- Initialisers ----------------------------------------------------- */
    DirItem{ diOther.Created(), diOther.Accessed(), diOther.Written(),
             diOther.Id(), diOther.Size(), diOther.Attributes() }
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- Assignment operator ------------------------------------------------ */
  DirItem operator=                    // cppcheck-suppress operatorEqVarError
    (const DirItem &diRHS) const       // False positive as copy ctor used
      { return diRHS; }                // Thinks 'ullSize' is not initialised
  /* ----------------------------------------------------------------------- */
  DirItem(const StdTimeT tNCreate, const StdTimeT tNAccess,
    const StdTimeT tNWrite, const uint64_t ullNId, const uint64_t ullNSize,
    const uint64_t ullNFlags) :
    /* -- Initialisers ----------------------------------------------------- */
    tCreate(tNCreate),                 // Initialise file creation time
    tAccess(tNAccess),                 // Initialise file access time
    tWrite(tNWrite),                   // Initialise file modification time
    ullId(ullNId),                     // Initialise file size
    ullSize(ullNSize),                 // Initialise file size
    ullFlags(ullNFlags)                // Initialise file attributes
    /* -- No code ---------------------------------------------------------- */
    {}
};/* ----------------------------------------------------------------------- */
MAPPACK_BUILD(DirEnt, const StdString, const DirItem) // DirItem map types
/* -- DirFile class -------------------------------------------------------- */
class DirFile                          // Files container class
{ /* -- Public variables --------------------------------------------------- */
  DirEntMap        demDirs, demFiles;  // Directories and files list
  /* -- Export ------------------------------------------------------------- */
  static StrSet Export(const DirEntMap &dSrc)
  { // Write entries into a single set list and return it
    StrSet ssFiles;
    for(const DirEntMapPair &dempFile : dSrc)
      ssFiles.emplace(StdMove(dempFile.first));
    return ssFiles;
  }
  /* -- Convert to set --------------------------------------------- */ public:
  StrSet DirsToSet() const { return Export(demDirs); }
  StrSet FilesToSet() const { return Export(demFiles); }
  /* -- Get lists ---------------------------------------------------------- */
  const DirEntMap &GetDirs() const { return demDirs; }
  const DirEntMap &GetFiles() const { return demFiles; }
  /* -- Get lists iterators ------------------------------------------------ */
  DirEntMapConstIt GetDirsBegin() const { return GetDirs().cbegin(); }
  DirEntMapConstIt GetFilesBegin() const { return GetFiles().cbegin(); }
  DirEntMapConstIt GetDirsEnd() const { return GetDirs().cend(); }
  DirEntMapConstIt GetFilesEnd() const { return GetFiles().cend(); }
  /* -- Get elements in lists ---------------------------------------------- */
  size_t GetDirsSize() const { return GetDirs().size(); }
  size_t GetFilesSize() const { return GetFiles().size(); }
  /* -- Get if lists are empty or not -------------------------------------- */
  bool IsDirsEmpty() const { return GetDirs().empty(); }
  bool IsDirsNotEmpty() const { return !IsDirsEmpty(); }
  bool IsFilesEmpty() const { return GetFiles().empty(); }
  bool IsFilesNotEmpty() const { return !IsFilesEmpty(); }
  /* -- Default constructor ------------------------------------------------ */
  DirFile() = default;
  /* -- Move constructor --------------------------------------------------- */
  DirFile(DirEntMap &&demNDirs, DirEntMap &&demNFiles) :
    /* -- Initialisers ----------------------------------------------------- */
    demDirs{ StdMove(demNDirs) },
    demFiles{ StdMove(demNFiles) }
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- Move constructor --------------------------------------------------- */
  DirFile(DirFile &&dfOther) :
    /* -- Initialisers ----------------------------------------------------- */
    demDirs{ StdMove(dfOther.demDirs) },
    demFiles{ StdMove(dfOther.demFiles) }
    /* -- No code ---------------------------------------------------------- */
    {}
};/* -- DirCore class ------------------------------------------------------ */
class DirCore :                        // System specific implementation
  /* -- Base classes ------------------------------------------------------- */
  public SerialMaster<>,               // Unique serial number counter
  public DirItem                       // Current item information
{ /* -- Variables -------------------------------------------------- */ public:
  StdString        strFile;            // Name of next file
  bool             bIsDir;             // Current item is a directory
  /* -- Setup implementation for WIN32 ------------------------------------- */
#if defined(WINDOWS)                   // WIN32 implementation
  /* -- Variables for WIN32 system -------------------------------- */ private:
  _wfinddata64_t   wfData;             // Data returned
  const intptr_t   iHandle;            // Context handle
  bool             bMore;              // If we have more files
  /* -- Process current match ---------------------------------------------- */
  void ProcessItem()
  { // Set if this is a directory
    bIsDir = wfData.attrib & _A_SUBDIR;
    // Set filename
    strFile = S16toUTF(wfData.name);
    // Set file information
    Set(wfData.time_create, wfData.time_access, wfData.time_write,
      this->Serial(), static_cast<uint64_t>(wfData.size), wfData.attrib);
    // Get next item
    bMore = _wfindnext64(iHandle, &wfData) != -1;
  }
  /* -- Return if directory was opened on WIN32 system ------------- */ public:
  bool IsOpened() const { return iHandle != -1; }
  /* -- Prepare next file for WIN32 system --------------------------------- */
  bool GetNextFile()
  { // If there are no more files then we are done
    if(!bMore) return false;
    // Process the current item
    ProcessItem();
    // Success
    return true;
  }
  /* -- Constructor for WIN32 system --------------------------------------- */
  explicit DirCore(const StdStringView &strvDir) :
    /* -- Initialisers ----------------------------------------------------- */
    iHandle(_wfindfirst64(UTFtoS16(strvDir.empty() ?
      cCommon->CommonAsterisk() :
        StrAppend(StrTrimSuffix(strvDir, '/'), '/',
          cCommon->CommonAsterisk())).data(),
      &wfData)),
    bMore(iHandle != -1)
    /* -- Process file if there are more ----------------------------------- */
    { if(bMore) ProcessItem(); }
  /* -- Destructor for WIN32 system ---------------------------------------- */
  ~DirCore() { if(iHandle != -1) _findclose(iHandle); }
  /* ----------------------------------------------------------------------- */
#else                                  // Using anything but Windows?
  /* -- Private typedefs ------------------------------------------ */ private:
  // This handle will be cleaned up by closedir() when it goes out of scope! We
  // could just use decltype(closedir) but GCC fires a warning because the
  // closedir() function has attributes so this is the workaround.
  struct CloseDirFtor
    { void operator()(DIR*const dPtr) const { closedir(dPtr); } };
  using DirUPtr = StdUniquePtr<DIR, CloseDirFtor>;
  /* -- Private variables -------------------------------------------------- */
  const StdString strPrefix;           // Prefix for filenames with stat()
  DirUPtr         dupHandle;           // Context for opendir()
  /* -- Return if directory was opened on POSIX system ------------- */ public:
  bool IsOpened() const { return !!dupHandle; }
  /* -- Prepare next file for POSIX system --------------------------------- */
  bool GetNextFile()
  { // Read the filename and if failed
    if(const dirent*const dePtr = readdir(dupHandle.get()))
    { // Data for stat
      struct stat sfssData;
      // Set filename
      strFile = dePtr->d_name;
      // Get information about the filename
      if(stat(StrAppend(strPrefix, strFile).data(), &sfssData))
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
          this->Serial(), static_cast<uint64_t>(sfssData.st_size),
          sfssData.st_mode);
      } // Success
      return true;
    } // Failed
    return false;
  }
  /* -- Constructor for POSIX system --------------------------------------- */
  explicit DirCore(const StdStringView &strDir) :
    /* -- Initialisers ----------------------------------------------------- */
    strPrefix{ StrAppend(              // Initialise string prefix
      strDir.empty() ?                 // If requested directory is empty?
        cCommon->CommonPeriod() :      // Set to scan current directory
        StrTrimSuffix(strDir, '/'),    // Trim forward-slash trailing slashes
      cCommon->CommonFSlashV()) },      // Add our own slash at the end
    dupHandle{                         // Initialise directory handle
      opendir(strPrefix.data()) }     // Open the directory and store handle
    /* -- Unload and clear the dir handle if init and no first file -------- */
    { if(dupHandle && !GetNextFile()) dupHandle.reset(); }
  /* ----------------------------------------------------------------------- */
#endif                                 // End of system implementation check
};/* == Dir class ========================================================== */
class Dir :                            // Directory information class
  /* -- Base classes ------------------------------------------------------- */
  public DirFile                       // Files container class
{ /* -- Do scan --------------------------------------------------- */ private:
  static void RemoveEntry(DirEntMap &dfemMap, const StdString &strEntry)
  { // Remove specified entry
    const DirEntMapIt demiIt{ dfemMap.find(strEntry) };
    if(demiIt != dfemMap.cend()) dfemMap.erase(demiIt);
  }
  /* -- Remove current and parent directory entries ------------------------ */
  static void RemoveParentAndCurrentDirectory(DirEntMap &dfemMap)
  { // Remove "." and ".." current directory entries
    RemoveEntry(dfemMap, cCommon->CommonPeriod());
    RemoveEntry(dfemMap, cCommon->CommonTwoPeriod());
  }
  /* -- Scan with no match checking ---------------------------------------- */
  static DirFile ScanDir(const StdStringView &strvDir = cCommon->CommonBlank())
  { // Directory and file list
    DirEntMap demNDirs, demNFiles;
    // Load up the specification and return if failed
    DirCore dcInterface{ strvDir };
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
  static DirFile ScanDirExt(const StdStringView &strvDir,
    const StdStringView &strExt)
  { // Directory and file list
    DirEntMap demNDirs, demNFiles;
    // Load up the specification and return if failed
    DirCore dcInterface{ strvDir };
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
  explicit Dir(DirFile &&dfOther) : DirFile{ StdMove(dfOther) } {}
  /* -- Constructor of current directory --------------------------- */ public:
  Dir() : DirFile{ ScanDir() } {}
  /* -- Constructor of specified directory --------------------------------- */
  explicit Dir(const StdStringView &strvDir) : DirFile{ ScanDir(strvDir) } {}
  /* -- Scan specified directory for files with specified extension -------- */
  Dir(const StdStringView &strvDir, const StdStringView &strExt) :
    DirFile{ ScanDirExt(strvDir, strExt) } {}
};/* ----------------------------------------------------------------------- */
/* -- Get current directory ------------------------------------------------ */
static StdString DirGetCWD()
{ // On windows, we need to use unicode
#if defined(WINDOWS)
  // Storage of filename and initialise it to maximum path length
  StdResized<StdWideString> wstrDir{ _MAX_PATH };
  // Get current directory and store it in string, throw exception if error
  if(!_wgetcwd(const_cast<wchar_t*>(wstrDir.data()), _MAX_PATH))
    throw StdRunTimeError{ "getcwd() failed!" };
  wstrDir.resize(wcslen(wstrDir.data()));
  // Return directory replacing backslashes for forward slashes
  return PSplitBackToForwardSlashes(WS16toUTF(wstrDir));
#else
  // Storage of filename and initialise it to maximum path length
  StdResized<StdString> strDir{ _MAX_PATH };
  // Get current directory and store it in string, throw exception if error
  if(!getcwd(const_cast<char*>(strDir.data()), _MAX_PATH))
    throw StdRunTimeError{ "getcwd() failed!" };
  // Resize and recover memory
  strDir.resize(strlen(strDir.data()));
  strDir.shrink_to_fit();
  // Return directory
  return strDir;
#endif
}
/* == Set current directory ================================================ */
static bool DirSetCWD(const StdStringView &strvDirectory)
{ // Ignore if empty
  if(strvDirectory.empty()) return false;
  // Process is different on win32 with having drive letters
#if defined(WINDOWS)
  // Set drive first if specified
  if(strvDirectory.size() >= 3 && strvDirectory[1] == ':' &&
     (strvDirectory[2] == '\\' || strvDirectory[2] != '/') &&
     _chdrive((StdToUpper(strvDirectory.front()) - 'A') + 1) < 0)
    return false;
#endif
  // Set current directory and return false if there is a problem
  return !StdChDir(strvDirectory);
}
/* -- Make a directory ----------------------------------------------------- */
static bool DirMkDir(const StdStringView &strDir) { return !StdMkDir(strDir); }
/* -- Remove a directory --------------------------------------------------- */
static bool DirRmDir(const StdStringView &strDir) { return !StdRmDir(strDir); }
/* -- Make a directory and all it's interim components --------------------- */
static bool DirMkDirEx(const StdStringView &strDir)
{ // Break apart directory parts
  const PathSplit psParts{ strDir };
  // Break apart so we can check the directories. Will always be non-empty.
  if(const TokenStr tsParts{ StrAppend(psParts.strDir, psParts.strFileExt),
    cCommon->CommonFSlashV() })
  { // This will be the string that wile sent to mkdir multiple times
    // gradually.
    StdOStringStream osS; osS << psParts.strDrive;
    // Get the first item and if it is not empty?
    const StdString &strFirst = tsParts.front();
    if(!strFirst.empty())
    { // Make the directory if isn't the drive and return failure if the
      // directory doesn't already exist
      if(!DirMkDir(strFirst) && StdIsNotError(EEXIST)) return false;
      // Move first item. It will be empty if directory started with a slash
      osS << strFirst;
    } // If there are more directories?
    if(tsParts.size() >= 2)
    { // Create all the other directories
      for(StrVectorConstIt svciI{ StdNext(tsParts.cbegin()) };
                           svciI != tsParts.cend();
                         ++svciI)
      { // Append next directory
        osS << '/' << *svciI;
        // Make the directory and if failed and it doesn't exist return error
        if(!DirMkDir(osS.str()) && StdIsNotError(EEXIST)) return false;
      }
    } // Success
    return true;
  } // Tokeniser failed
  return false;
}
/* -- Remove a directory and all it's interim components ------------------- */
static bool DirRmDirEx(const StdStringView &strDir)
{ // Break apart directory parts
  const PathSplit psParts{ strDir };
  // Break apart so we can check the directories. Will always be non-empty.
  TokenStr tsParts{ StrAppend(psParts.strDir, psParts.strFileExt),
    cCommon->CommonFSlashV() };
  // Get the first item and if it is not empty?
  while(!tsParts.empty())
  { // This will be the string that wile sent to mkdir multiple times
    // gradually. Do not try to construct the oss with the drive string because
    // it won't work and thats not how the constructor works it seems!
    StdOStringStream osS; osS << psParts.strDrive;
    // Get the first item and if it is not empty?
    const StdString &strFirst = tsParts.front();
    if(!strFirst.empty()) osS << strFirst;
    // If there are more directories? Build directory structure
    if(tsParts.size() >= 2)
      StdForEach(seq, StdNext(tsParts.cbegin()), tsParts.cend(),
        [&osS](const StdString &strPart) { osS << '/' << strPart; });
    // Make the directory and if failed and it doesn't exist return error
    if(!DirRmDir(osS.str()) && StdIsNotError(EEXIST)) return false;
    // Remove the last item
    tsParts.pop_back();
  } // Success
  return true;
}
/* -- Delete a file -------------------------------------------------------- */
static bool DirFileUnlink(const StdStringView &strvFile)
  { return !StdUnlink(strvFile); }
/* -- Get file size - ------------------------------------------------------ */
static int DirFileSize(const StdStringView &strvFile, StdFStatStruct &sfssData)
  { return StdFStat(strvFile, &sfssData) ? StdGetError() : 0; }
/* -- True if specified file has the specified mode ------------------------ */
static bool DirFileHasMode(const StdStringView &strvFile, const int iMode,
  const int iNegate)
{ // Get file information and and if succeeded?
  StdFStatStruct sfssData;
  if(!DirFileSize(strvFile, sfssData))
  { // If file attributes have specified mode then success
    if((sfssData.st_mode ^ iNegate) & iMode) return true;
    // Set error number
    StdSetError(ENOTDIR);
  } // Failed
  return false;
}
/* -- True if specified file is actually a directory ----------------------- */
static bool DirLocalDirExists(const StdStringView &strvFile)
  { return DirFileHasMode(strvFile, _S_IFDIR, 0); }
/* -- True if specified file is actually a file ---------------------------- */
static bool DirLocalFileExists(const StdStringView &strvFile)
  { return DirFileHasMode(strvFile, _S_IFDIR, -1); }
/* -- Readable or writable? ------- Check if file is readable or writable -- */
static bool DirCheckFileAccess(const StdStringView &strvFile, const int iFlag)
  { return !StdAccess(strvFile, iFlag); }
/* -- True if specified file exists and is readable ------------------------ */
static bool DirIsFileReadable(const StdStringView &strvFile)
  { return DirCheckFileAccess(strvFile, R_OK); }
/* -- True if specified file exists and is readable and writable ----------- */
static bool DirIsFileReadWriteable(const StdStringView &strvFile)
  { return DirCheckFileAccess(strvFile, R_OK|W_OK); }
/* -- True if specified file exists and is writable ------------------------ */
static bool DirIsFileWritable(const StdStringView &strvFile)
  { return DirCheckFileAccess(strvFile, W_OK); }
/* -- True if specified file exists and is executable ---------------------- */
static bool DirIsFileExecutable(const StdStringView &strvFile)
  { return DirCheckFileAccess(strvFile, X_OK); }
/* -- True if specified file or directory exists --------------------------- */
static bool DirLocalResourceExists(const StdStringView &strvFile)
  { return DirCheckFileAccess(strvFile, F_OK); }
/* -- Rename file ---------------------------------------------------------- */
static bool DirFileRename(const StdStringView &strFrom,
  const StdStringView &strTo)
{ return !StdRename(strFrom, strTo); }
/* -- Check that filename is valid and throw on error ---------------------- */
static void DirVerifyFileNameIsValid(const StdStringView &strvFile)
{ // Throw error if invalid name
  if(const ValidResult vrId = DirValidName(strvFile))
    XC("Filename is invalid!",
      "File",     strvFile,
      "Reason",   cDirBase->DirBaseVNRtoStr(vrId),
      "ReasonId", vrId);
}
/* -- Directory saver/restorer class --------------------------------------- */
class DirSaver
{ /* -- Private variables -------------------------------------------------- */
  const StdString strCWD;              // Saved current directory
  /* -- Constructor to set directory ------------------------------- */ public:
  explicit DirSaver(const StdString &strNWD) :
    /* -- Initialisers ----------------------------------------------------- */
    strCWD{ DirGetCWD() }              // Save current working directory
    /* -- Set new directory ------------------------------------------------ */
    { DirSetCWD(strNWD); }
  /* -- Default constructor ------------------------------------------------ */
  DirSaver() :                         // Save current working directory
    /* -- Initialisers ----------------------------------------------------- */
    strCWD{ DirGetCWD() }              // Initialise current working directory
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- Destructor --------------------------------------------------------- */
  ~DirSaver()                          // Restore original working directory
    /* -- Restore current working directory -------------------------------- */
    { DirSetCWD(strCWD); }
};/* ----------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
