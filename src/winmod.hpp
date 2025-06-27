/* == WINMOD.HPP =========================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## This is a Windows specific module that parses executable files to   ## **
** ## reveal information about it.                                        ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* -- System module class -------------------------------------------------- */
class SysModule :                      // Members initially private
  /* -- Base classes ------------------------------------------------------- */
  public SysModuleData                 // System module data
{ /* -- Get and store verison numbers -------------------------------------- */
  struct VersionNumbers                // Members initially public
  { /* -- Storage for version numbers -------------------------------------- */
    unsigned int uiMajor, uiMinor, uiBuild, uiRevision;
    /* -- Constructor ------------------------------------------------------ */
    explicit VersionNumbers(const wstring &wstrValue)
    { // Length of version info part
      UINT uiLength = 0;
      // Get version info. Return cleared information if failed.
      VS_FIXEDFILEINFO *lpFfi = nullptr;
      if(!VerQueryValueW(wstrValue.c_str(), L"\\",
        reinterpret_cast<LPVOID*>(&lpFfi), &uiLength))
      { // Clear version numbers
        uiMajor = SysErrorCode();
        uiMinor = uiRevision = uiBuild = StdMaxUInt;
        // Done
        return;
      } // Return if we did not read enough data
      if(uiLength < sizeof(VS_FIXEDFILEINFO)) return;
      // Create reference to version information
      const VS_FIXEDFILEINFO &rFfi = *lpFfi;
      uiMajor = UtilHighWord(rFfi.dwFileVersionMS);
      uiMinor = UtilLowWord(rFfi.dwFileVersionMS);
      uiBuild = UtilHighWord(rFfi.dwFileVersionLS);
      uiRevision = UtilLowWord(rFfi.dwFileVersionLS);
    }
    /* --------------------------------------------------------------------- */
  };                                   // End of VersionNumbers class
  /* -- Get and store string data ------------------------------------------ */
  struct VersionStrings                // Members initially public
  { /* -- Storage for version strings--------------------------------------- */
    string strDescription, strVendor, strCopyright;
    /* -- Get string value ---------------------------------------- */ private:
    const string GetStringValue(const wstring &wstrBlock,
      const wstring &wstrValue)
    { // Bail if no data
      if(wstrValue.empty() || wstrBlock.empty()) return {};
      // Get size of string. The actual string size is also returned in this.
      UINT uiStrSize = static_cast<UINT>(wstrBlock.length());
      // This is set to the resulting read string on success
      wchar_t *wcpStr = nullptr;
      // Return the result of the string resource lookup or return the system
      // error code string instead.
      return VerQueryValueW(wstrBlock.data(), wstrValue.c_str(),
           reinterpret_cast<LPVOID*>(&wcpStr), &uiStrSize) ?
        S16toUTF(wcpStr) : SysError();
    }
    /* -- Build path ------------------------------------------------------- */
    const string GetStringValue(const LONG lLng, const wstring &wstrBlock,
      const char*const cpValue)
    { // Build path name
      const string strValue{ StrFormat("\\StringFileInfo\\$$$$$\\$",
        right, hex, setw(8), setfill('0'), lLng, cpValue) };
      // Query the value from the resource
      return GetStringValue(wstrBlock, UTFtoS16(strValue));
    }
    /* -- Constructor ---------------------------------------------- */ public:
    explicit VersionStrings(const wstring &wstrBlock)
    { // Create language struct
      struct LANGANDCODEPAGE { WORD wLanguage, wCodePage; } *lcpData;
      // Length of version info part
      UINT uiLength = 0;
      // Read translation information strings and if failed or the length is
      // not big enough to fit the required structure?
      if(!VerQueryValueW(wstrBlock.data(), L"\\VarFileInfo\\Translation",
        reinterpret_cast<LPVOID*>(&lcpData), &uiLength) ||
        uiLength < sizeof(struct LANGANDCODEPAGE))
      { // Put error into data
        strDescription = StrAppend("!E#", SysErrorCode());
        strVendor = StrFromNum(uiLength);
        strCopyright = cCommon->CommonUnspec();
        // Done
        return;
      } // Clamp the limit to the lowest zero for safety.
      const size_t stLimit = static_cast<size_t>
        (floor(static_cast<double>(uiLength) /
          sizeof(struct LANGANDCODEPAGE)));
      // Read the file description for each language and code page.
      for(size_t stIndex = 0; stIndex < stLimit; ++stIndex)
      { // Get reference to version data struct and make language id from it
        const LANGANDCODEPAGE &lacpData = lcpData[stIndex];
        const LONG lLng =
          UtilMakeDWord(lacpData.wLanguage, lacpData.wCodePage);
        // Get version, vendor and comments strings from module
        strDescription = GetStringValue(lLng, wstrBlock, "FileDescription");
        strVendor = GetStringValue(lLng, wstrBlock, "CompanyName");
        strCopyright = GetStringValue(lLng, wstrBlock, "Comments");
      }
    }
    /* --------------------------------------------------------------------- */
  };                                   // End of VersionStrings class
  /* -- Get executable version information size ---------------------------- */
  DWORD ReadSize(const string &strModule)
  { // Get size of version info structure. Done if succeeded
    DWORD dwDummy = 0;
    if(const DWORD dwSize =
      GetFileVersionInfoSizeW(UTFtoS16(strModule).c_str(), &dwDummy))
        return dwSize;
    // Ignore if module has no resource section. This can be triggered when
    // using Wine as their DLL's don't have resource data sections.
    if(GetLastError() == ERROR_RESOURCE_DATA_NOT_FOUND) return 0;
    // All other errors must be thrown
    XCS("Unable to query the length of the version string inside "
        "the specified module!", "File", strModule);
  }
  /* -- Get version information--------------------------------------------- */
  const wstring ReadInfo(const string &strModule, const DWORD dwSize)
  { // Allocate memory for string and read data. Return string if succeeded!
    wstring wstrVI(dwSize, 0);
    if(GetFileVersionInfoW(UTFtoS16(strModule).c_str(), 0, dwSize,
      UtfToNonConstCast<LPVOID>(wstrVI.data())))
        return wstrVI;
    // Failed to throw error
    XCS("Unable to query version information from the specified module!",
        "File", strModule, "Size", static_cast<unsigned int>(dwSize));
  }
  /* -- Return version information ----------------------------------------- */
  SysModuleData Load(const string &strModule)
  { // Read the size of the executable version info return nothing if empty
    if(const DWORD dwSize = ReadSize(strModule))
    { // Query version numbers and strings data
      const wstring wstrVersionInfo{ ReadInfo(strModule, dwSize) };
      VersionNumbers vnData{ wstrVersionInfo };
      VersionStrings vsData{ wstrVersionInfo };
      // Version numbers together has string
      string strVersionNumbers{ StrFormat("$.$.$.$",
        vnData.uiMajor, vnData.uiMinor, vnData.uiBuild, vnData.uiRevision) };
      // Return data
      return SysModuleData{ strModule, vnData.uiMajor, vnData.uiMinor,
        vnData.uiBuild, vnData.uiRevision, StdMove(vsData.strVendor),
        StdMove(vsData.strDescription), StdMove(vsData.strCopyright),
        StdMove(strVersionNumbers) };
    } // Failed
    return SysModuleData{ strModule };
  }
  /* -- Return version information ----------------------------------------- */
  SysModuleData Load(string &&strModule)
  { // Read the size of the executable version info return nothing if empty
    if(const DWORD dwSize = ReadSize(strModule))
    { // Query version numbers and strings data
      const wstring wstrVersionInfo{ ReadInfo(strModule, dwSize) };
      VersionNumbers vnData{ wstrVersionInfo };
      VersionStrings vsData{ wstrVersionInfo };
      // Version numbers together has string
      string strVersionNumbers{ StrFormat("$.$.$.$",
        vnData.uiMajor, vnData.uiMinor, vnData.uiBuild, vnData.uiRevision) };
      // Return data
      return SysModuleData{ StdMove(strModule), vnData.uiMajor,
        vnData.uiMinor, vnData.uiBuild, vnData.uiRevision,
        StdMove(vsData.strVendor), StdMove(vsData.strDescription),
        StdMove(vsData.strCopyright), StdMove(strVersionNumbers) };
    } // Failed
    return SysModuleData{ StdMove(strModule) };
  }
  /* -- Return data (copy filename) -------------------------------- */ public:
  explicit SysModule(const string &strModule) :
    /* -- Initialisers ----------------------------------------------------- */
    SysModuleData{ Load(strModule) }
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Return data (move filename) ---------------------------------------- */
  explicit SysModule(string &&strModule) :
    /* -- Initialisers ----------------------------------------------------- */
    SysModuleData{ Load(StdMove(strModule)) }
    /* -- No code ---------------------------------------------------------- */
    { }
};/* ----------------------------------------------------------------------- */
/* == EoF =========================================================== EoF == */
