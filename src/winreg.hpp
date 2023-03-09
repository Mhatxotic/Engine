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
/* == Windows Registry Class =============================================== */
class SysReg                           // Members initially private
{ /* ----------------------------------------------------------------------- */
  HKEY             hkKey;              // Key handle
  /* -- Return handle ---------------------------------------------- */ public:
  HKEY GetHandle(void) const { return hkKey; }
  /* -- Return if handle is opened or not ---------------------------------- */
  bool Opened(void) const { return GetHandle() != nullptr; }
  bool NotOpened(void) const { return !Opened(); }
  /* -- Query sub keys ----------------------------------------------------- */
  const StrVector QuerySubKeys(void) const
  { // Key opened? Return nothing
    if(NotOpened()) return {};
    // Create key list
    StrVector klData;
    // Create memory to hold string
    wstring wstrData; wstrData.reserve(MAX_PATH);
    // Until there are no more items
    for(unsigned int uiIndex = 0;; ++uiIndex)
    { // Set size
      DWORD dwSize = static_cast<DWORD>(wstrData.capacity() * sizeof(wchar_t));
      // Enumerate. Add to list if succeeded
      switch(RegEnumKeyEx(GetHandle(), uiIndex,
        const_cast<wchar_t*>(wstrData.data()),
          &dwSize, nullptr, nullptr, nullptr, nullptr))
      { // Succeeded so add to list
        case ERROR_SUCCESS: klData.emplace_back(WS16toUTF(wstrData)); break;
        // No more items so return the list
        case ERROR_NO_MORE_ITEMS: return klData;
        // Other error, just ignore it.
        default: break;
      }
    } // Never gets here
  }
  /* -- Query value as string----------------------------------------------- */
  const string QueryString(const string &strV) const
  { // Key opened? Return nothing
    if(NotOpened()) return {};
    // Initialise size and type
    DWORD dwSize = 0, dwType = 0;
    // Query value into string
    const wstring wstrV{ UTFtoS16(strV) };
    if(RegQueryValueEx(GetHandle(), wstrV.c_str(), nullptr, &dwType,
      reinterpret_cast<LPBYTE>(&dwType), &dwSize) != ERROR_MORE_DATA ||
        dwType != REG_SZ || !dwSize) return {};
    // Create a pre-allocated stringAllocate buffer and query value again
    wstring wstrBuffer; wstrBuffer.resize(dwSize / sizeof(wchar_t));
    if(RegQueryValueEx(GetHandle(), wstrV.c_str(), nullptr, &dwType,
      reinterpret_cast<LPBYTE>(wstrBuffer.data()), &dwSize) != ERROR_SUCCESS)
        return {};
    // Return as UTF string.
    return WS16toUTF(wstrBuffer);
  }
  /* -- Query value -------------------------------------------------------- */
  unsigned int Query(const string &strV, void **vpD, const DWORD dwS) const
  { // Bail if key not opened
    if(NotOpened()) return ERROR_NO_TOKEN;
    // Set size
    DWORD dwSize = dwS, dwType = 0;
    // Query value and return status
    return RegQueryValueEx(GetHandle(), UTFtoS16(strV).c_str(), nullptr,
      &dwType, reinterpret_cast<LPBYTE>(vpD), &dwSize);
  }
  /* -- Query integer ------------------------------------------------------ */
  template<typename StorageType>
    const StorageType Query(const string &strV) const
  { // Storage for value
    StorageType tValue{ 0 };
    // Query the key value and store it in the integer
    Query(strV, reinterpret_cast<void**>(&tValue), sizeof(StorageType));
    // We are done! Return the value
    return tValue;
  }
  /* -- Direct access to return if handle is opened ------------------------ */
  operator bool(void) const { return Opened(); }
  /* -- Constructor with init ---------------------------------------------- */
  SysReg(HKEY hkB, const string &strSK, const REGSAM rsA) :
    /* -- Initialisers ----------------------------------------------------- */
    hkKey(RegOpenKeyEx(hkB,            // Open registry key with specified root
      UTFtoS16(strSK).c_str(),         // Specified subkey to open
      0,                               // No options
      rsA,                             // Specified security
      &hkB) == ERROR_SUCCESS ?         // Destination handle and if succeeded?
        hkB :                          // Success so set the handle
        nullptr)                       // Failure so set a null handled
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Destructor --------------------------------------------------------- */
  ~SysReg(void) { if(Opened()) RegCloseKey(GetHandle()); }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(SysReg)              // Suppress default functions for safety
};/* -- End ---------------------------------------------------------------- */
/* == EoF =========================================================== EoF == */
