/* == WINREG.HPP =========================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## This is a Windows specific module that handles reading data from    ## **
** ## the Windows Registry. We don't ever want to write to this piece of  ## **
** ## shit database so let's just keep it read only!                      ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace ISysReg {                    // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IStd::P;               using namespace IStdLib::P;
using namespace IUtf::P;               using namespace Lib::OS;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* ------------------------------------------------------------------------- */
class SysReg                           // Members initially private
{ /* ----------------------------------------------------------------------- */
  HKEY             hkKey;              // Key handle
  /* -- Return handle ---------------------------------------------- */ public:
  HKEY SysRegGetHandle() const { return hkKey; }
  /* -- Return if handle is opened or not ---------------------------------- */
  bool SysRegOpened() const { return SysRegGetHandle() != nullptr; }
  bool SysRegNotOpened() const { return !SysRegOpened(); }
  /* -- Query sub keys ----------------------------------------------------- */
  StrVector SysRegQuerySubKeys() const
  { // Key opened? Return nothing
    if(SysRegNotOpened()) return {};
    // Create key list
    StrVector svKeys;
    // Until there are no more items
    for(unsigned uIndex = 0;; ++uIndex)
    { // Create memory to hold string
      StdResized<StdWideString> swsKey{ MAX_PATH };
      // Set size
      DWORD dwSize = static_cast<DWORD>(swsKey.capacity() * sizeof(wchar_t));
      // Enumerate. Add to list if succeeded
      switch(RegEnumKeyEx(SysRegGetHandle(), uIndex,
        const_cast<wchar_t*>(swsKey.data()),
          &dwSize, nullptr, nullptr, nullptr, nullptr))
      { // Succeeded?
        case ERROR_SUCCESS:
          // Clean up string and add to list
          swsKey.resize(static_cast<size_t>(dwSize));
          swsKey.shrink_to_fit();
          svKeys.emplace_back(WS16toUTF(swsKey)); break;
        // No more items so return the list
        case ERROR_NO_MORE_ITEMS: return svKeys;
        // Other error, just ignore it.
        default: break;
      }
    } // Never gets here
  }
  /* -- Query value as string----------------------------------------------- */
  StdString SysRegQueryString(const StdStringView &ssvV) const
  { // Key opened? Return nothing
    if(SysRegNotOpened()) return {};
    // Initialise size and type
    DWORD dwSize = 0, dwType = 0;
    // Query length of registry value
    const StdWideString wssvV{ UTFtoS16(ssvV) };
    if(RegQueryValueEx(SysRegGetHandle(), wssvV.data(), nullptr, &dwType,
      reinterpret_cast<LPBYTE>(&dwType), &dwSize) == ERROR_MORE_DATA)
    { // Must be a string!
      if(dwType == REG_SZ)
      { // Compare size (size includes NULL terminator)
        switch(dwSize)
        { // Function failure (no null terminator?, should be impossible)
          case 0: [[fallthrough]];
          // Just a null terminator
          case 1: break;
          // Anything else?
          default:
          { // Grab the string value and return a UTF8 string version
            StdResized<StdWideString> wstrBuffer{
              (dwSize / sizeof(wchar_t)) - 1 };
            if(RegQueryValueEx(SysRegGetHandle(), wssvV.data(), nullptr,
                 &dwType, reinterpret_cast<LPBYTE>(wstrBuffer.data()), &dwSize)
               == ERROR_SUCCESS) return WS16toUTF(wstrBuffer);
            // Failed
            break;
          } // Size comparison
        } // Invalid size
      } // Invalid type
    } // Query failed or something else so return empty string
    return {};
  }
  /* -- Query value -------------------------------------------------------- */
  LSTATUS SysRegQuery(const StdStringView &ssvV, void **vpD, const DWORD dwS)
    const
  { // Ignore if key not opened else query registry value and return status
    if(SysRegNotOpened()) return ERROR_NO_TOKEN;
    DWORD dwSize = dwS, dwType = 0;
    return RegQueryValueEx(SysRegGetHandle(), UTFtoS16(ssvV).data(), nullptr,
      &dwType, reinterpret_cast<LPBYTE>(vpD), &dwSize);
  }
  /* -- Query integer ------------------------------------------------------ */
  template<typename AnyType>
    requires StdIsIntegral<AnyType>
  AnyType SysRegQuery(const StdStringView &ssvV) const
  { // Query the key value and store it in the integer
    AnyType atValue{ 0 };
    SysRegQuery(ssvV, reinterpret_cast<void**>(&atValue), sizeof(AnyType));
    return atValue;
  }
  /* -- Direct access to return if handle is opened ------------------------ */
  operator bool() const { return SysRegOpened(); }
  /* -- Constructor with init ---------------------------------------------- */
  SysReg(HKEY hkB, const StdStringView &ssvSK, const REGSAM rsA) :
    /* -- Initialisers ----------------------------------------------------- */
    hkKey(RegOpenKeyEx(hkB,            // Open registry key with specified root
      UTFtoS16(ssvSK).data(),          // Specified subkey to open
      0,                               // No options
      rsA,                             // Specified security
      &hkB) == ERROR_SUCCESS ?         // Destination handle and if succeeded?
        hkB :                          // Success so set the handle
        nullptr)                       // Failure so set a null handled
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- Destructor --------------------------------------------------------- */
  ~SysReg() { if(SysRegOpened()) RegCloseKey(SysRegGetHandle()); }
};/* ----------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
