/* == WINMOD.HPP =========================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## This is a Windows specific module that parses executable files to   ## **
** ## reveal information about it.                                        ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace ISysMod {                    // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IStd::P;               using namespace IStdLib::P;
using namespace IString::P;            using namespace ISysUtil::P;
using namespace IUtil::P;              using namespace Lib::OS;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* ------------------------------------------------------------------------- */
class SysModule :                      // Members initially private
  /* -- Base classes ------------------------------------------------------- */
  public SysModuleData                 // System module data
{ /* -- Get and store verison numbers -------------------------------------- */
  struct VersionNumbers                // Members initially public
  { /* -- Storage for version numbers -------------------------------------- */
    unsigned uMajor, uMinor, uBuild, uRevision;
    /* -- Constructor ------------------------------------------------------ */
    explicit VersionNumbers(const StdWideString &wstrValue)
    { // Length of version info part
      UINT uLength = 0;
      // Get version info. Return cleared information if failed.
      VS_FIXEDFILEINFO *lpFfi = nullptr;
      if(!VerQueryValueW(wstrValue.data(), L"\\",
        reinterpret_cast<LPVOID*>(&lpFfi), &uLength))
      { // Clear version numbers
        uMajor = SysErrorCode<unsigned>();
        uMinor = uRevision = uBuild = StdMaxUInt;
        // Done
        return;
      } // Return if we did not read enough data
      if(uLength < sizeof(VS_FIXEDFILEINFO)) return;
      // Create reference to version information
      const VS_FIXEDFILEINFO &rFfi = *lpFfi;
      uMajor = UtilHighWord(rFfi.dwFileVersionMS);
      uMinor = UtilLowWord(rFfi.dwFileVersionMS);
      uBuild = UtilHighWord(rFfi.dwFileVersionLS);
      uRevision = UtilLowWord(rFfi.dwFileVersionLS);
    }
    /* --------------------------------------------------------------------- */
  };                                   // End of VersionNumbers class
  /* -- Get and store string data ------------------------------------------ */
  struct VersionStrings                // Members initially public
  { /* -- Storage for version strings--------------------------------------- */
    StdString strDescription, strVendor, strCopyright;
    /* -- Get string value ---------------------------------------- */ private:
    StdString GetStringValue(const StdWideString &wstrBlock,
      const StdWideString &wstrValue)
    { // Bail if no data
      if(wstrValue.empty() || wstrBlock.empty()) return {};
      // Get size of string. The actual string size is also returned in this.
      UINT uStrSize = static_cast<UINT>(wstrBlock.size());
      // This is set to the resulting read string on success
      wchar_t *wcpStr = nullptr;
      // Return the result of the string resource lookup or return the system
      // error code string instead.
      return VerQueryValueW(wstrBlock.data(), wstrValue.data(),
           reinterpret_cast<LPVOID*>(&wcpStr), &uStrSize) ?
        S16toUTF(wcpStr) : SysError();
    }
    /* -- Build path ------------------------------------------------------- */
    StdString GetStringValue(const LONG lLng, const StdWideString &wstrBlock,
      const char*const cpValue)
    { // Build path name
      const StdString strValue{ StrFormat("\\StringFileInfo\\$$$$$\\$",
        StdIOSRight, StdIOSHex, StdIOSSetWidth(8), StdIOSSetFill('0'), lLng,
        cpValue) };
      // Query the value from the resource
      return GetStringValue(wstrBlock, UTFtoS16(strValue));
    }
    /* -- Constructor ---------------------------------------------- */ public:
    explicit VersionStrings(const StdWideString &wstrBlock)
    { // Create language struct
      struct LANGANDCODEPAGE { WORD wLanguage, wCodePage; } *lcpData;
      // Length of version info part
      UINT uLength = 0;
      // Read translation information strings and if failed or the length is
      // not big enough to fit the required structure?
      if(!VerQueryValueW(wstrBlock.data(), L"\\VarFileInfo\\Translation",
        reinterpret_cast<LPVOID*>(&lcpData), &uLength) ||
        uLength < sizeof(struct LANGANDCODEPAGE))
      { // Put error into data
        strDescription = StrAppend("!E#", SysErrorCode());
        strVendor = StrFromNum(uLength);
        strCopyright = cCommon->CommonUnspec();
        // Done
        return;
      } // Get the number of items returned and if not empty?
      if(const size_t stSize = static_cast<size_t>
        (floor(static_cast<double>(uLength) /
          sizeof(struct LANGANDCODEPAGE))))
      { // Protect the returned data
        StdSpan<LANGANDCODEPAGE> sLcp{ lcpData, stSize };
        // Read the file description for each language and code page.
        StdFindIf(seq, sLcp.begin(), sLcp.end(),
          [this, &wstrBlock](const LANGANDCODEPAGE &lacpData)
        { // Make a language and codepage code
          const LONG lLng =
            UtilMakeDWord<LONG>(lacpData.wLanguage, lacpData.wCodePage);
          // Get version, vendor and comments strings from module
          strDescription = GetStringValue(lLng, wstrBlock, "FileDescription");
          strVendor = GetStringValue(lLng, wstrBlock, "CompanyName");
          strCopyright = GetStringValue(lLng, wstrBlock, "Comments");
          // Don't process anymore if the codepage and language are valid
          return lacpData.wCodePage != 0x04B0 &&
            UtilHighByte(lacpData.wLanguage) == LANG_ENGLISH;
        });
      }
    }
    /* --------------------------------------------------------------------- */
  };                                   // End of VersionStrings class
  /* -- Get executable version information size ---------------------------- */
  DWORD ReadSize(const StdString &strModule)
  { // Get size of version info structure. Done if succeeded
    DWORD dwDummy = 0;
    if(const DWORD dwSize =
      GetFileVersionInfoSizeW(UTFtoS16(strModule).data(), &dwDummy))
        return dwSize;
    // Ignore if module has no resource section. This can be triggered when
    // using Wine as their DLL's don't have resource data sections.
    if(SysIsErrorCode(ERROR_RESOURCE_DATA_NOT_FOUND)) return 0;
    // Write warning to log and return zero bytes size
    cLog->LogWarningExSafe(
      "System unable to query the length of the version string from '$': $",
      strModule, SysError());
    return 0;
  }
  /* -- Get version information--------------------------------------------- */
  StdWideString ReadInfo(const StdString &strModule, const DWORD dwSize)
  { // Allocate memory for string and read data. Return string if succeeded!
    StdWideString wstrVI(dwSize, 0);
    if(GetFileVersionInfoW(UTFtoS16(strModule).data(), 0, dwSize,
      StdToNonConstCast<LPVOID>(wstrVI.data())))
        return wstrVI;
    // Write log and return empty string
    cLog->LogWarningExSafe(
      "System unable to query $ bytes of version information from '$': $",
      dwSize, strModule, SysError());
    return {};
  }
  /* -- Return version information ----------------------------------------- */
  SysModuleData Load(const StdString &strModule)
  { // Read the size of the executable version info return nothing if empty
    if(const DWORD dwSize = ReadSize(strModule))
    { // Query version numbers and strings data
      const StdWideString wstrVersionInfo{ ReadInfo(strModule, dwSize) };
      VersionNumbers vnData{ wstrVersionInfo };
      VersionStrings vsData{ wstrVersionInfo };
      // Version numbers together has string
      StdString strVersionNumbers{ StrFormat("$.$.$.$",
        vnData.uMajor, vnData.uMinor, vnData.uBuild, vnData.uRevision) };
      // Return data
      return SysModuleData{ strModule, vnData.uMajor, vnData.uMinor,
        vnData.uBuild, vnData.uRevision, StdMove(vsData.strVendor),
        StdMove(vsData.strDescription), StdMove(vsData.strCopyright),
        StdMove(strVersionNumbers) };
    } // Failed
    return SysModuleData{ strModule };
  }
  /* -- Return version information ----------------------------------------- */
  SysModuleData Load(StdString &&strModule)
  { // Read the size of the executable version info return nothing if empty
    if(const DWORD dwSize = ReadSize(strModule))
    { // Query version numbers and strings data
      const StdWideString wstrVersionInfo{ ReadInfo(strModule, dwSize) };
      VersionNumbers vnData{ wstrVersionInfo };
      VersionStrings vsData{ wstrVersionInfo };
      // Version numbers together has string
      StdString strVersionNumbers{ StrFormat("$.$.$.$",
        vnData.uMajor, vnData.uMinor, vnData.uBuild, vnData.uRevision) };
      // Return data
      return SysModuleData{ StdMove(strModule), vnData.uMajor,
        vnData.uMinor, vnData.uBuild, vnData.uRevision,
        StdMove(vsData.strVendor), StdMove(vsData.strDescription),
        StdMove(vsData.strCopyright), StdMove(strVersionNumbers) };
    } // Failed
    return SysModuleData{ StdMove(strModule) };
  }
  /* -- Return data (copy filename) -------------------------------- */ public:
  explicit SysModule(const StdString &strModule) :
    /* -- Initialisers ----------------------------------------------------- */
    SysModuleData{ Load(strModule) }
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- Return data (move filename) ---------------------------------------- */
  explicit SysModule(StdString &&strModule) :
    /* -- Initialisers ----------------------------------------------------- */
    SysModuleData{ Load(StdMove(strModule)) }
    /* -- No code ---------------------------------------------------------- */
    {}
};/* ----------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
