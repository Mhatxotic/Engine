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
  HKEY GetHandle() const { return hkKey; }
  /* -- Return if handle is opened or not ---------------------------------- */
  bool Opened() const { return GetHandle() != nullptr; }
  bool NotOpened() const { return !Opened(); }
  /* -- Query sub keys ----------------------------------------------------- */
  StrVector QuerySubKeys() const
  { // Key opened? Return nothing
    if(NotOpened()) return {};
    // Create key list
    StrVector klData;
    // Until there are no more items
    for(unsigned uIndex = 0;; ++uIndex)
    { // Create memory to hold string
      StdResized<StdWideString> wstrData{ MAX_PATH };
      // Set size
      DWORD dwSize = static_cast<DWORD>(wstrData.capacity() * sizeof(wchar_t));
      // Enumerate. Add to list if succeeded
      switch(RegEnumKeyEx(GetHandle(), uIndex,
        const_cast<wchar_t*>(wstrData.data()),
          &dwSize, nullptr, nullptr, nullptr, nullptr))
      { // Succeeded?
        case ERROR_SUCCESS:
          // Clean up string and add to list
          wstrData.resize(static_cast<size_t>(dwSize));
          wstrData.shrink_to_fit();
          klData.emplace_back(WS16toUTF(wstrData)); break;
        // No more items so return the list
        case ERROR_NO_MORE_ITEMS: return klData;
        // Other error, just ignore it.
        default: break;
      }
    } // Never gets here
  }
  /* -- Query value as string----------------------------------------------- */
  StdString QueryString(const StdString &strV) const
  { // Key opened? Return nothing
    if(NotOpened()) return {};
    // Initialise size and type
    DWORD dwSize = 0, dwType = 0;
    // Query value into string
    const StdWideString wstrV{ UTFtoS16(strV) };
    if(RegQueryValueEx(GetHandle(), wstrV.data(), nullptr, &dwType,
      reinterpret_cast<LPBYTE>(&dwType), &dwSize) != ERROR_MORE_DATA ||
        dwType != REG_SZ || !dwSize) return {};
    // Create a pre-allocated stringAllocate buffer and query value again
    StdResized<StdWideString> wstrBuffer{ dwSize / sizeof(wchar_t) };
    if(RegQueryValueEx(GetHandle(), wstrV.data(), nullptr, &dwType,
      reinterpret_cast<LPBYTE>(wstrBuffer.data()), &dwSize) != ERROR_SUCCESS)
        return {};
    // Return as UTF string.
    return WS16toUTF(wstrBuffer);
  }
  /* -- Query value -------------------------------------------------------- */
  LSTATUS Query(const StdString &strV, void **vpD, const DWORD dwS) const
  { // Ignore if key not opened else query registry value and return status
    if(NotOpened()) return ERROR_NO_TOKEN;
    DWORD dwSize = dwS, dwType = 0;
    return RegQueryValueEx(GetHandle(), UTFtoS16(strV).data(), nullptr,
      &dwType, reinterpret_cast<LPBYTE>(vpD), &dwSize);
  }
  /* -- Query integer ------------------------------------------------------ */
  template<typename AnyType>
    requires StdIsIntegral<AnyType>
  AnyType Query(const StdString &strV) const
  { // Query the key value and store it in the integer
    AnyType atValue{ 0 };
    Query(strV, reinterpret_cast<void**>(&atValue), sizeof(AnyType));
    return atValue;
  }
  /* -- Direct access to return if handle is opened ------------------------ */
  operator bool() const { return Opened(); }
  /* -- Constructor with init ---------------------------------------------- */
  SysReg(HKEY hkB, const StdString &strSK, const REGSAM rsA) :
    /* -- Initialisers ----------------------------------------------------- */
    hkKey(RegOpenKeyEx(hkB,            // Open registry key with specified root
      UTFtoS16(strSK).data(),          // Specified subkey to open
      0,                               // No options
      rsA,                             // Specified security
      &hkB) == ERROR_SUCCESS ?         // Destination handle and if succeeded?
        hkB :                          // Success so set the handle
        nullptr)                       // Failure so set a null handled
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- Destructor --------------------------------------------------------- */
  ~SysReg() { if(Opened()) RegCloseKey(GetHandle()); }
};/* ----------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
