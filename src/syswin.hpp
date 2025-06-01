/* == SYSWIN.HPP =========================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## This is a Windows specific module that allows the engine to talk    ## **
** ## to, and manipulate operating system procedures and funtions.        ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* == Win32 extras ========================================================= */
#include "winmod.hpp"                  // Module information class
#include "winreg.hpp"                  // Registry class
#include "winmap.hpp"                  // File mapping class
#include "winpip.hpp"                  // Process output piping class
#include "winbase.hpp"                 // Base system class
#include "wincon.hpp"                  // Console terminal window class
/* == System intialisation helper ========================================== **
** ######################################################################### **
** ## Because we want to try and statically init const data as much as    ## **
** ## possible, we need this class to derive by the System class so we    ## **
** ## can make sure these functions are initialised first. Also making    ## **
** ## this private prevents us from accessing these functions because     ## **
** ## again - they are only for initialisation.                           ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
class SysProcess :                     // Need this before of System init order
  /* -- Dependency classes ------------------------------------------------- */
  private Ident                        // Mutex identifier
{ /* ------------------------------------------------------------ */ protected:
  uint64_t         qwSKL,              // Kernel kernel time
                   qwSUL,              // Kernel user time
                   qwPKL,              // Process kernel time
                   qwPUL,              // Process user time
                   qwPTL;              // Current system time
  /* ----------------------------------------------------------------------- */
  const HMODULE    hKernel;            // Handle to kernel library
  const HANDLE     hProcess;           // Process handle
  const HINSTANCE  hInstance;          // Process instance
  HANDLE           hMutex;             // Global mutex handle
  /* -- Return process and thread id ------------------------------ */ private:
  const DWORD      ulProcessId;        // Process id
  const DWORD      ulThreadId;         // Thread id (WinMain())
  /* ----------------------------------------------------------------------- */
  static BOOL WINAPI EnumWindowsProc(HWND hH, LPARAM lP)
  { // Get title of window and cancel if empty
    wstring wstrT(GetWindowTextLength(hH), 0);
    if(!GetWindowText(hH, const_cast<LPWSTR>(wstrT.c_str()),
      static_cast<DWORD>(wstrT.capacity())))
        return TRUE;
    // If there is not enough characters in this windows title or the title
    // and requested window title's contents do not match? Ignore this window!
    const wstring &wstrN = *reinterpret_cast<wstring*>(lP);
    if(wstrT.length() < wstrN.length() ||
      StdCompare(wstrN.data(), wstrT.data(), wstrN.length()*sizeof(wchar_t)))
        return TRUE;
    // We found the window
    cLog->LogDebugExSafe("- Found window handle at $$.\n"
                 "- Window name is '$'.",
      hex, reinterpret_cast<void*>(hH), WS16toUTF(wstrT));
    // First try showing the window and if successful? Log the successful
    // command else if showing the window failed? Log the failure with reason
    if(ShowWindow(hH, SW_RESTORE|SW_SHOWNORMAL))
      cLog->LogDebugSafe("- Show window request was successful.");
    else
      cLog->LogWarningExSafe("- Show window request failed: $!", SysError());
    // Then try setting it as a foreground window and if succeeded? Log the
    // successful command else if setting the foreground window failed? Log the
    // failure with reason.
    if(SetForegroundWindow(hH))
      cLog->LogDebugSafe("- Set fg window request was successful.");
    else
      cLog->LogWarningExSafe("- Set fg window request failed: $!", SysError());
    // Then try setting the focus of the window and if succeeded? Log the
    // successful command else if the setting focus failed? Log the failure
    // with reason.
    if(SetFocus(hH) || SysIsErrorCode())
      cLog->LogDebugSafe("- Set focus request was successful.");
    else
      cLog->LogWarningExSafe("- Set focus request failed: $!", SysError());
    // Reset error so it's not seen as the result
    SetLastError(0);
    // Don't look for any more windows
    return FALSE;
  }
  /* -- Return process and thread id ---------------------------- */ protected:
  template<typename IntType=decltype(ulProcessId)>IntType GetPid(void) const
    { return static_cast<IntType>(ulProcessId); }
  template<typename IntType=decltype(ulThreadId)>IntType GetTid(void) const
    { return static_cast<IntType>(ulThreadId); }
  /* ----------------------------------------------------------------------- */
  void InitReportMemoryLeaks(void)
  { // Only needed if in debug mode
#if defined(ALPHA)
    // Create storage for the filename and clear it
    wstring wstrName; wstrName.resize(MAX_PATH);
    // Get path to executable. The base module filename info struct may not be
    // available so we'll keep it simple and use the full path name to
    // the executable.
    wstrName.resize(UtilMaximum(GetModuleFileName(nullptr,
      const_cast<LPWSTR>(wstrName.c_str()),
      static_cast<DWORD>(wstrName.capacity())) - 4, 0));
    wstrName.append(L".crt");
    // Create a file with the above name and just return if failed
    HANDLE hH = CreateFile(wstrName.c_str(), GENERIC_WRITE,
      FILE_SHARE_READ|FILE_SHARE_WRITE, 0, CREATE_ALWAYS,
      FILE_ATTRIBUTE_NORMAL, 0);
    if(hH == INVALID_HANDLE_VALUE) return;
    // Change reporting to file mode
    _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_WARN, hH);
    _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_ERROR, hH);
    _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_ASSERT, hH);
    // Unfortunately, we can't dump memory now or close the file because
    // all the static constructors won't have cleaned up yet so we have to let
    // the stdlib/kernel do everything for us on exit. Hurts my OCD, but meh!:(
#endif
  }
  /* -- Set heap information helper ---------------------------------------- */
  template<typename Type = ULONG>static void HeapSetInfo(const HANDLE hH,
    const HEAP_INFORMATION_CLASS hicData, const Type &tVal)
      { HeapSetInformation(hH, hicData, UtfToNonConstCast<PVOID>(&tVal),
          sizeof(tVal)); }
  /* ----------------------------------------------------------------------- */
  void ReconfigureMemoryModel(void) const
  { // Disable paging memory to disk. RAM is cheap now, cmon ffs!
    SetProcessWorkingSetSize(hProcess, static_cast<SIZE_T>(-1),
                                       static_cast<SIZE_T>(-1));
    // Get number of process heaps. Shouldn't be zero but if it is?
    const DWORD dwHeaps = GetProcessHeaps(0, nullptr);
    if(!dwHeaps)
    { // Log the error and return
      cLog->LogErrorExSafe("System failed to retrieve process heaps size: $!",
        SysError());
      return;
    } // Allocate memory for heaps handles, fill handles and
    typedef vector<HANDLE> HandleVec;
    HandleVec hvList{ dwHeaps, INVALID_HANDLE_VALUE };
    if(!GetProcessHeaps(dwHeaps, hvList.data()))
    { // Log the error and return
      cLog->LogErrorExSafe("System failed to retrieve process heaps: $!",
        SysError());
      return;
    } // For each heap, enable low fragmentation heap
    for(const HANDLE &hH : hvList)
      HeapSetInfo(hH, HeapCompatibilityInformation, 2);
    // Enable optimize resources in Win 8.1+ with HeapOptimizeResources
    // - This is a interesting little optimization because I don't invoke
    // - the Win8.1 API (and probably won't), the parameter required here is
    // - HEAP_OPTIMIZE_RESOURCES_INFORMATION(3) which we don't have, but it is
    // - quite simply only two ULONG's, ulV (Version) must be initialised to 1
    // - but the ulF (Flags) parameter doesn't seem to be used.
    const struct HORIDATA { const ULONG ulV, ulF; } horiData{ 1, 0 };
    HeapSetInfo<HORIDATA>(nullptr, static_cast<HEAP_INFORMATION_CLASS>(3),
      horiData);
  }
  /* -- Called when the C runtime runs into a problem ---------------------- */
  static int CRTException(const int, char*const cpMsg, int *iRet)
  { // Log exception. You can't actually throw an exception here and I don't
    // know why, so just log it for now.
    cLog->LogErrorExSafe("CRT exception: $", cpMsg);
    // Continue
    *iRet = 2;
    // Process other dialogs
    return FALSE;
  }
  /* -- Called when C std code runs into a problem ------------------------- */
  static int VisualCRTError(const int iType, const wchar_t*const wcpF,
    const int iLine, const wchar_t*const wcpM, const wchar_t*const wcpFmt, ...)
  { // Buffer for formatted data. The maximum size is 1024 bytes. That is
    // 512 wide characters. std::string adds the nullptr for us automatically.
    wstring wstrFmt; wstrFmt.reserve(511);
    // Use windows api function for this as we're not using the c-lib
    // formatting functions and theres no need to invoke extra exe space.
    va_list vlData;
    va_start(vlData, wcpFmt);
    wstrFmt.resize(wvsprintf(const_cast<wchar_t*>(wstrFmt.data()),
      wcpFmt, vlData));
    va_end(vlData);
    // Throw exception (parameters are wide strings :|)
    cLog->LogErrorExSafe("RTC error $ in $::$::$: $!", iType,
      S16toUTF(wcpF), S16toUTF(wcpM), iLine, WS16toUTF(wstrFmt));
    // Done
    return 0;
  }
  /* -- Called when C std functions need to abort -------------------------- */
  static void CException(
#if defined(ALPHA)
    const wchar_t*const wcpE, const wchar_t *const wcpFN,
    const wchar_t*const wcpF, const unsigned int uiL, uintptr_t)
      { XC("C exception!", "Expression", wcpE,  "File", wcpF,
                           "Function",   wcpFN, "Line", uiL); }
#else
    const wchar_t*const, const wchar_t*const,
    const wchar_t*const, const unsigned int, uintptr_t)
      { XC("C exception!"); }
#endif
  /* ----------------------------------------------------------------------- */
  void InitCRTParameters(void)
  { // Set runtime error callback
    _set_invalid_parameter_handler(CException);
    // Set runtime error callback (Ignored when _DEBUG not set)
    _RTC_SetErrorFuncW(VisualCRTError);
    // Set debug report hook (Ignored when _DEBUG not set)
    _CrtSetReportHook2(_CRT_RPTHOOK_INSTALL, CRTException);
    // Always show abort() dialog box and never use shitty WER
    _set_abort_behavior(1, _WRITE_ABORT_MSG);
    _set_abort_behavior(0, _CALL_REPORTFAULT);
    // Set debug flags
    // _CRTDBG_ALLOC_MEM_DF      = Track allocations.
    // _CRTDBG_LEAK_CHECK_DF     = Check for memory leaks.
    // _CRTDBG_DELAY_FREE_MEM_DF = Don't actually free memory.
    // _CRTDBG_CHECK_ALWAYS_DF   = Check memory all the time (SLOW!).
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    // Output all issues to debugger and window
    _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG | _CRTDBG_MODE_WNDW);
    _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_DEBUG | _CRTDBG_MODE_WNDW);
    _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_DEBUG | _CRTDBG_MODE_WNDW);
  }
  /* ----------------------------------------------------------------------- */
  void LoadCOM(void)
  { // Initialise COM and bail out if failed. We're not using COM in other
    // threads so we just initialise it normally.
    //   (COINIT_MULTITHREADED|COINIT_SPEED_OVER_MEMORY)
    if(const HRESULT hrResult = CoInitialize(nullptr))
      XC("Failed to initialise COM library!",
         "Result", static_cast<unsigned int>(hrResult));
    // Prepare common controls initialisation struct
    const INITCOMMONCONTROLSEX iccData{
      sizeof(iccData),                 // DWORD dwSize (Size of struct)
      0                                // DWORD dwICC (Flags)
    };
    // Init common controls. This is needed to stop Windows XP crashing
    // as I am using a manifest file for pretty controls.
    if(!InitCommonControlsEx(&iccData))
      XCS("Failed to initialise common controls library!");
  }
  /* ----------------------------------------------------------------------- */
  template<typename T>T Test(const T tParam, const char*const cpStr)
    { if(!tParam) XCS(cpStr); return tParam; }
  /* --------------------------------------------------------------- */ public:
  bool InitGlobalMutex(const string_view &strvTitle)
  { // Convert UTF title to wide string
    const wstring wstrTitle{ UTFtoS16(strvTitle) };
    // Create the global mutex handle with the specified name and check error
    hMutex = CreateMutex(nullptr, FALSE, wstrTitle.c_str());
    switch(const DWORD dwResult = SysErrorCode<DWORD>())
    { // No error, continue
      case 0: return true;
      // Global mutex name already exists?
      case ERROR_ALREADY_EXISTS:
        // Log that another instance already exists
        cLog->LogDebugSafe(
          "System found existing mutex, scanning for original window...");
        // Look for the specified window and if we activated it?
        if(!EnumWindows(EnumWindowsProc, reinterpret_cast<LPARAM>(&wstrTitle)))
        { // If an error occured?
          if(SysIsNotErrorCode())
          { // Log the error and reason why it failed
            cLog->LogErrorExSafe(
              "System failed to find the window to activate! $!", SysError());
          } // No window was found
          else cLog->LogWarningSafe("System window enumeration successful!");
        } // Could not find it?
        else cLog->LogDebugSafe("System window enumeration unsuccessful!");
        // Caller must terminate program
        return false;
      // Other error
      default: XCS("Failed to create global mutex object!",
        "Title",  strvTitle,
        "Result", static_cast<unsigned int>(dwResult),
        "mutex",  reinterpret_cast<void*>(hMutex));
    } // Getting here is impossible
  }
  /* -- Constructor --------------------------------------------- */ protected:
  SysProcess(void) :
    /* -- Initialisers ----------------------------------------------------- */
    qwSKL(0),
    qwSUL(0),
    qwPKL(0),
    qwPUL(0),
    qwPTL(0),
    hKernel(Test(GetModuleHandle(L"KERNEL32"), "No kernel library handle!")),
    hProcess(Test(GetCurrentProcess(), "No engine process handle!")),
    hInstance(Test(GetModuleHandle(nullptr), "No engine instance handle!")),
    hMutex(nullptr),
    ulProcessId(GetCurrentProcessId()),
    ulThreadId(GetCurrentThreadId())
    /* -- Code ------------------------------------------------------------- */
    { // Load common controls so dialog boxes are themed
      LoadCOM();
      // Init CRT stuff
      InitCRTParameters();
      // Reconfigure process heaps
      ReconfigureMemoryModel();
    }
  /* ----------------------------------------------------------------------- */
  ~SysProcess(void)
  { // Init file handle for storing CRT issues
    InitReportMemoryLeaks();
    // Remove debug report hook (because exceptions will crash)
    _CrtSetReportHook2(_CRT_RPTHOOK_REMOVE, CRTException);
    // Remove runtime error callback
    _RTC_SetErrorFunc(nullptr);
    // Deinitialise COM
    CoUninitialize();
    // If mutex initialised? Close the handle and log if failed
    if(hMutex && !CloseHandle(hMutex))
      cLog->LogWarningExSafe("System failed to close mutex handle '$'! $.",
        IdentGet(), SysError());
  }
};/* == Class ============================================================== */
class SysCore :
  /* -- Base classes ------------------------------------------------------- */
  public SysProcess,                   // Gets system process information
  public SysVersion,                   // Gets system version information
  public SysCommon,                    // Common system functions
  public SysCon                        // System console and crash handler
{ /* -- Public Variables ------------------------------------------ */ private:
  HICON            hIconLarge;         // Handle to large icon
  HICON            hIconSmall;         // Handle to small icon
  /* -- Helper to grab default locale information -------------------------- */
  size_t GetLocaleData(const LCTYPE lcType, const void*const vpData,
    const size_t stSize, const LCID lcidLocale)
  { return GetLocaleInfo(lcidLocale, lcType,
      UtfToNonConstCast<LPWSTR>(vpData), UtilIntOrMax<int>(stSize)); }
  /* ----------------------------------------------------------------------- */
  const wstring GetLocaleString(const LCTYPE lcType,
    const LCID lcidLocale=LOCALE_USER_DEFAULT)
  { // Allocate string for requested data and return error if faield
    wstring wstrData;
    wstrData.resize(GetLocaleData(lcType, nullptr, 0, lcidLocale));
    if(wstrData.empty())
      XCS("No storage for locale data!",
          "Type", lcType, "Id", lcidLocale);
    // Now fill in the string and show error if failed
    if(!GetLocaleData(lcType, wstrData.data(), wstrData.length(), lcidLocale))
      XCS("Failed to acquire locale data!",
          "Type", lcType, "Id", lcidLocale, "Buffer", wstrData.length());
    // Return data
    return wstrData;
  }
  /* -- Set socket timeout ----------------------------------------- */ public:
  static int SetSocketTimeout(const int iFd, const double dRTime,
    const double dWTime)
  { // Calculate timeout in milliseconds
    const DWORD dwR = static_cast<DWORD>(dRTime * 1000),
                dwW = static_cast<DWORD>(dWTime * 1000);
    // Unix:  struct timeval tData = { 30, 0 }; // Sec / USec
    // Set socket options and get result
    return (setsockopt(iFd,
      SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<const char*>(&dwR),
        sizeof(dwR)) < 0 ? 1 : 0) | (setsockopt(iFd,
      SOL_SOCKET, SO_SNDTIMEO, reinterpret_cast<const char*>(&dwW),
        sizeof(dwW)) < 0 ? 2 : 0);
  }
  /* -- Get uptime from clock class ---------------------------------------- */
  StdTimeT GetUptime(void) const { return cmHiRes.GetTimeS(); }
  /* -- Terminate a process ------------------------------------------------ */
  bool TerminatePid(const unsigned int uiPid) const
  { // Return result
    bool bResult;
    // Get parameter as DWORD (X-platform compatibility)
    const DWORD dwPid = static_cast<DWORD>(uiPid);
    // Open the process. Failed if no pid or no access
    if(const HANDLE hPid = OpenProcess(PROCESS_TERMINATE, FALSE, dwPid)) try
    { // We should find the parent process so create a process snapshot
      const HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
      if(hSnapshot != INVALID_HANDLE_VALUE) try
      { // Prepare process info structure
        PROCESSENTRY32 pedData{
          sizeof(pedData),             // DWORD dwSize
          0,                           // DWORD cntUsage
          0,                           // DWORD th32ProcessID
          0,                           // ULONG_PTR th32DefaultHeapID
          0,                           // DWORD th32ModuleID
          0,                           // DWORD cntThreads
          0,                           // DWORD th32ParentProcessID
          0,                           // LONG pcPriClassBase
          0,                           // DWORD dwFlags
          {},                          // CHAR szExeFile[MAX_PATH]
        }; // Interate through the process list
        if(Process32First(hSnapshot, &pedData))
        { // Loop...
          do
          { // Ignore if pid doesnt match requested argument
            if(dwPid != pedData.th32ProcessID) continue;
            // Pid matches but if we don't own this pid?
            if(pedData.th32ParentProcessID != GetPid())
            { // Failed result
              bResult = false;
              // Write that process isn't owned by me
              cLog->LogWarningExSafe("System process $ parent $ not $!",
                dwPid, pedData.th32ParentProcessID, GetPid());
            } // Terminate the process and if failed?
            else if(TerminateProcess(hPid, static_cast<UINT>(-1)))
            { // Success result
              bResult = true;
              // Write that we couldnt terminate processes
              cLog->LogInfoExSafe(
                "System forcefully terminated process $!", uiPid);
            } // Success so set success result
            else
            { // Failed result
              bResult = false;
              // Write that we couldnt terminate processes
              cLog->LogWarningExSafe(
                "System failed to terminate process $: $!", uiPid, SysError());
            } // We're finished
            goto Finished;
            // ...until no more processes.
          } while(Process32Next(hSnapshot, &pedData));
          // Write that we couldnt enumerate processes
          cLog->LogWarningExSafe(
            "System could not find process $ to terminate: $!",
            uiPid, SysError());
          // Failed
          bResult = false;
          // Finished label (used in above loop)
          Finished:;
        } // Enumerate processes function call failed?
        else
        { // Write that we couldnt enumerate processes
          cLog->LogWarningExSafe("System failed to read first process to "
            "terminate process $: $!", uiPid, SysError());
          // Failed
          bResult = false;
        } // Close the snapshot handle
        CloseHandle(hSnapshot);
      } // Exeception occured while process handle opened and snapshot grabbed?
      catch(...)
      { // Close the snapshot handle
        CloseHandle(hSnapshot);
        // Throw original error
        throw;
      } // Enumerate processes function call failed?
      else
      { // Write that we couldnt enumerate processes
        cLog->LogWarningExSafe(
          "System failed to snapshot to terminate process $: $!",
          uiPid, SysError());
        // Failed
        bResult = false;
      } // Close the opened process handle
      CloseHandle(hPid);
    } // Exeception occured while process handle opened?
    catch(...)
    { // Close the opened process handle
      CloseHandle(hPid);
      // Throw original error
      throw;
    } // Open process handle failed?
    else
    { // Write that we couldnt open process
      cLog->LogWarningExSafe(
        "System failed to open process $ to terminate: $!", uiPid, SysError());
      // Failed
      bResult = false;
    } // Return result
    return bResult;
  }
  /* -- Check if specified process id is running --------------------------- */
  bool IsPidRunning(const unsigned int uiPid) const
  { // Return result
    bool bResult;
    // Open the process. Failed if no pid or no access
    if(const HANDLE hPid = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE,
      static_cast<DWORD>(uiPid))) try
    { // Exit code storage
      DWORD dwXCode;
      // Get exit code and set result whether process is still active
      if(GetExitCodeProcess(hPid, &dwXCode)) bResult = dwXCode == STILL_ACTIVE;
      // Get process exit code failed
      else
      { // Set failed
        bResult = true;
        // Write error to log
        cLog->LogWarningExSafe("System failed get process $ exit code: $!",
          uiPid, SysError());
      } // Close the open handle
      CloseHandle(hPid);
    } // Exeception occured while process handle opened?
    catch(...)
    { // Close the opened process handle
      CloseHandle(hPid);
      // Throw original error
      throw;
    } // Failed to open process?
    else
    { // Set failed
      bResult = true;
      // Write to log ONLY if the process id was found
      if(SysIsNotErrorCode(ERROR_INVALID_PARAMETER))
        cLog->LogWarningExSafe("System failed to open executing process $: $!",
          uiPid, SysError());
    } // Return result
    return bResult;
  }
  /* ----------------------------------------------------------------------- */
  void UpdateIcon(const UINT uiMsg, const HICON hIcon) const
  { // Ignore if icon not available
    if(!hIcon) return;
    // Do the send message
    SendMessage(GetWindowHandle(), WM_SETICON, uiMsg,
      reinterpret_cast<LPARAM>(hIcon));
    // Log the result
    cLog->LogDebugExSafe("System updated the window icon with type $.", uiMsg);
  }
  /* ----------------------------------------------------------------------- */
  void UpdateIcons(void) const
  { // Return if the glfw or console window isn't available
    if(IsNotWindowHandleSet()) return;
    // Update the large and small icons
    UpdateIcon(ICON_BIG, hIconLarge);
    UpdateIcon(ICON_SMALL, hIconSmall);
  }
  /* ----------------------------------------------------------------------- */
  void SetIcon(const string &strId, const char *cpType, const UINT uiT,
    HICON &hI, const size_t stWidth, const size_t stHeight,
    const size_t stBits, const MemConst &mcSrc)
  { // Check parameters
    if(!stWidth || !stHeight)
      XC("Supplied icon dimensions invalid!",
         "Type",  cpType,  "Identifier", strId,
         "Width", stWidth, "Height",     stHeight);
    if(stBits != 24 && stBits != 32)
      XC("Must be 24/32 bpp icon!",
         "Type", cpType, "Identifier", strId, "Bits", stBits);
    if(mcSrc.MemIsEmpty())
      XC("Invalid icon data!", "Type", cpType, "Identifier", strId);
    // Create the icon. CreateIcon() seems to ignore the AND bits
    // on 24/32bpp icons but /analyse complains, so send original bits to it
    // The old icon will be preserved if the api call fails
    const HICON hNewIcon = CreateIcon(hInstance, static_cast<int>(stWidth),
      static_cast<int>(stHeight), 1, static_cast<BYTE>(stBits),
      mcSrc.MemPtr<BYTE>(), mcSrc.MemPtr<BYTE>());
    if(!hNewIcon)
      XCS("Failed to create new icon!",
          "Type",   cpType,  "Identifier", strId,
          "Width",  stWidth, "Height",     stHeight,
          "Bits",   stBits,  "Data",       mcSrc.MemIsNotEmpty(),
          "Window", reinterpret_cast<void*>(GetWindowHandle()));
    // Destroy old icon if created and then assign the new icon
    if(hI && !DestroyIcon(hI))
      cLog->LogWarningExSafe("System failed to delete old window icon: $!",
        SysError());
    hI = hNewIcon;
    // Update window icon if we have a window
    SendMessage(GetWindowHandle(),
      WM_SETICON, uiT, reinterpret_cast<LPARAM>(hI));
    // Remember: We have to destroy the icons when we're done with them.
    cLog->LogDebugExSafe("System set $ ($) $x$x$ window icon from '$'.",
      cpType, uiT, stWidth, stHeight, stBits, strId);
  }
  /* -- Set small or large icon -------------------------------------------- */
  void SetLargeIcon(const string &strId, const size_t stWidth,
    const size_t stHeight, const size_t stBits, const MemConst &mcSrc)
      { SetIcon(strId, "large", ICON_BIG, hIconLarge, stWidth, stHeight,
          stBits, mcSrc); }
  void SetSmallIcon(const string &strId, const size_t stWidth,
    const size_t stHeight, const size_t stBits, const MemConst &mcSrc)
      { SetIcon(strId, "small", ICON_SMALL, hIconSmall, stWidth, stHeight,
          stBits, mcSrc); }
  /* -- Free the library handle -------------------------------------------- */
  static bool LibFree(void*const vpModule)
    { return vpModule && !!FreeLibrary(reinterpret_cast<HMODULE>(vpModule)); }
  /* -- Get dll procedure address ------------------------------------------ */
  template<typename T>static const T
    GetSharedFunc(const HMODULE hModule, const char*const cpExport)
      { return reinterpret_cast<T>(GetProcAddress(hModule, cpExport)); }
  /* -- Get kernel procedure address --------------------------------------- */
  template<typename T>const T GetKernelFunc(const char*const cpExport)
    { return GetSharedFunc<T>(hKernel, cpExport); }
  /* -- Get the export address --------------------------------------------- */
  template<typename T>static T
    LibGetAddr(void*const vpModule, const char*const cpExport)
      { return GetSharedFunc<T>(reinterpret_cast<HMODULE>(vpModule),
          cpExport); }
  /* -- Load the specified .dll -------------------------------------------- */
  static void *LibLoad(const char*const cpFileName) { return
    reinterpret_cast<void*>(LoadLibraryEx(UTFtoS16(cpFileName).c_str(),
      nullptr, 0)); }
  /* -- Get full pathname to the library ----------------------------------- */
  const string LibGetName(void*const vpModule,
    const char*const cpAltName) const
  { // Return nothing if no module
    if(!vpModule) return {};
    // Storage for library name.
    Memory mStr{ _MAX_PATH * sizeof(ArgType) };
    // Get the library name and store it in the memory
    mStr.MemResize(GetModuleFileNameEx(hProcess,
      reinterpret_cast<HMODULE>(vpModule), mStr.MemPtr<ArgType>(),
      mStr.MemSize<DWORD>()) * sizeof(ArgType));
    // Use default name if empty or failed
    return mStr.MemIsEmpty() ? cpAltName : S16toUTF(mStr.MemPtr<ArgType>());
  }
  /* ----------------------------------------------------------------------- */
  void UpdateCPUUsageData(void)
  { // Storage for last system times
    uint64_t qwI, qwK, qwU, qwX;
    // Get system CPU times
    if(!GetSystemTimes(reinterpret_cast<LPFILETIME>(&qwI),
                       reinterpret_cast<LPFILETIME>(&qwK),
                       reinterpret_cast<LPFILETIME>(&qwU))) return;
    // Calculate system CPU times
    const uint64_t qcpuSUser   = qwU - qwSUL,
                   qcpuSKernel = qwK - qwSKL,
                   qcpuSysTot  = qcpuSKernel + qcpuSUser;
    // Set system cpu usage
    cpuUData.dSystem = UtilMakePercentage(qcpuSKernel, qcpuSysTot);
    // Update last system times
    qwSUL = qwU, qwSKL = qwK;
    // Get process CPU times
    if(!GetProcessTimes(hProcess, reinterpret_cast<LPFILETIME>(&qwI),
                                  reinterpret_cast<LPFILETIME>(&qwX),
                                  reinterpret_cast<LPFILETIME>(&qwK),
                                  reinterpret_cast<LPFILETIME>(&qwU))) return;
    // Get system time
    GetSystemTimeAsFileTime(reinterpret_cast<LPFILETIME>(&qwX));
    // Calculate process CPU time
    const uint64_t qcpuPUser   = qwU - qwPUL,
                   qcpuPKernel = qwK - qwPKL,
                   qcpuPTime   = qwX - qwPTL,
                   qcpuProcTot = qcpuPKernel + qcpuPUser;
    // Update last values
    qwPUL = qwU, qwPKL = qwK, qwPTL = qwX;
    // Set process cpu usage
    cpuUData.dProcess =
      UtilMakePercentage(static_cast<double>(qcpuProcTot) / qcpuPTime,
        StdThreadMax());
  }
  /* -- Seek to position in specified handle ------------------------------- */
  template<typename IntType>
    static IntType SeekFile(const HANDLE hH, const IntType itP)
  { // Bail if handle invalid
    if(hH == INVALID_HANDLE_VALUE) return numeric_limits<IntType>::max();
    // Convert uint64_t to UINT64. They're normally the same but we'll have
    // this here just to be correct. The compiler will optimise this out
    // anyway.
    const UINT64 qwP = static_cast<UINT64>(itP);
    // High-order 64-bit value will be sent and returned in this
    DWORD dwNH = UtilHighDWord(qwP);
    // Set file pointer
    const DWORD dwNL = SetFilePointer(hH, UtilLowDWord(qwP),
      reinterpret_cast<PLONG>(&dwNH), FILE_BEGIN);
    // Build new 64-bit position integer
    const UINT64 qwNP = UtilMakeQWord(dwNH, dwNL);
    // Return zero if failed or new position
    return qwNP == qwP ?
      static_cast<IntType>(qwNP) : numeric_limits<IntType>::max();
  }
  /* -- Get executable size from header ------------------------------------ */
  size_t GetExeSize(const string &strFile) const
  { // Open exe file and return on error
    if(FStream fExe{ strFile, FM_R_B })
    { // Create memblock for file header, must be at least 4K
      Memory mExe{ 4096 };
      // Get minimum amount of data we need to read
      const size_t stMinimum =
        sizeof(IMAGE_DOS_HEADER) + sizeof(IMAGE_NT_HEADERS32);
      // Read data into file and if failed? Report it
      const size_t stActual =
        fExe.FStreamReadSafe(mExe.MemPtr(), mExe.MemSize());
      if(stActual < stMinimum)
        XCL("Failed to read enough data in executable!",
            "File",    strFile,       "Maximum", mExe.MemSize(),
            "Minimum", stMinimum,     "Actual",  stActual,
            "Size",    fExe.FStreamSize());
      // Align a dos header structure to buffer
      const IMAGE_DOS_HEADER &pdhData =
        *mExe.MemRead<IMAGE_DOS_HEADER>(0, sizeof(IMAGE_DOS_HEADER));
      // Read PE header and throw error if it is not valid MZ signature
      const IMAGE_NT_HEADERS32 &pnthData =
        *mExe.MemRead<IMAGE_NT_HEADERS32>(pdhData.e_lfanew,
          sizeof(IMAGE_NT_HEADERS32));
      if(pdhData.e_magic != IMAGE_DOS_SIGNATURE)
        XC("Executable does not have a valid MZ signature!",
           "File",   strFile, "Requested", IMAGE_DOS_SIGNATURE,
           "Actual", pdhData.e_magic);
      // Throw error if it is not valid MZ signature
      if(pnthData.Signature != IMAGE_NT_SIGNATURE)
        XC("Executable does not have a valid PE signature!",
           "File",   strFile, "Requested", IMAGE_NT_SIGNATURE,
           "Actual", pnthData.Signature);
      // Detect machine type and set pointer to first section
      size_t stHdrSize;
      switch(pnthData.FileHeader.Machine)
      { case IMAGE_FILE_MACHINE_I386:                                // 32-bit?
          stHdrSize = sizeof(IMAGE_NT_HEADERS32); break;
        case IMAGE_FILE_MACHINE_IA64: case IMAGE_FILE_MACHINE_AMD64: // 64-bit?
          stHdrSize = sizeof(IMAGE_NT_HEADERS64); break;
        // Unknown. Bail out!
        default: XC("Could not detect executable header size!",
                    "File", strFile, "Actual", pnthData.FileHeader.Machine);
      } // Calculate beginning of headers offset
      const size_t stHdrsOffset = pdhData.e_lfanew + stHdrSize;
      // Maximum pointer and size of executable
      size_t stSize = 0;
      // Enumerate through each section recording header and highest position
      for(size_t stI = 0, stPtr = 0, stPos = 0;
                 stI < pnthData.FileHeader.NumberOfSections;
               ++stI, stPtr += sizeof(IMAGE_SECTION_HEADER))
      { // Get reference to section data
        const IMAGE_SECTION_HEADER &pshData =
          *mExe.MemRead<IMAGE_SECTION_HEADER>
            (stHdrsOffset + stPtr, sizeof(IMAGE_SECTION_HEADER));
        // Get pointer to raw data and ignore if we're not there yet
        if(pshData.PointerToRawData <= stPos) continue;
        // Update absolute executable offset and size
        stPos = pshData.PointerToRawData;
        stSize = pshData.PointerToRawData + pshData.SizeOfRawData;
      } // Return size of executable hopefully
      return stSize;
    } // Failed so throw error
    XCL("Failed to open executable!",
      "File", strFile, "Directory", DirGetCWD());
  }
  /* -- Enum modules ------------------------------------------------------- */
  SysModMap EnumModules(void)
  { // Module list
    SysModMap smmMap;
    // Number of modules
    DWORD dwNeeded = 0;
    // Get number of modules
    if(!EnumProcessModules(hProcess, nullptr, 0, &dwNeeded))
      XCS("Failed to enumerate size of process modules!",
          "Process", hProcess, "Required", dwNeeded);
    // Windoze should never set the size to 0 but just incase
    if(!dwNeeded)
      XC("Windows gave zero size module handle list!", "Process", hProcess);
    // Allocate memory
    typedef vector<HMODULE> ModuleHandleVec;
    ModuleHandleVec mhvList{ dwNeeded / sizeof(HMODULE) };
    // Get modules
    if(!EnumProcessModules(hProcess, mhvList.data(), dwNeeded, &dwNeeded))
      XCS("Failed to enumerate process modules!",
          "Process",    reinterpret_cast<void*>(hProcess),
          "Allocation", mhvList.size(),
          "Required",   static_cast<unsigned int>(dwNeeded));
    // For each module. Get filename then check the version info for it
    for(const HMODULE hH : mhvList)
    { // Prepare string to hold filename
      wstring wstrP; wstrP.resize(MAX_PATH);
      // Put filename of file in string and resize string to amount returned
      wstrP.resize(GetModuleFileNameEx(hProcess, hH,
        const_cast<LPWSTR>(wstrP.c_str()),
        static_cast<DWORD>(wstrP.capacity())));
      // ...and if empty, ignore (doubtful)
      if(wstrP.empty()) continue;
      // Insert into module list
      smmMap.emplace(make_pair(reinterpret_cast<size_t>(hH),
        SysModule(WS16toUTF(wstrP))));
    } // Return modules
    return smmMap;
  }
  /* ----------------------------------------------------------------------- */
  const wstring GetSystemFolder(const int iCSIDL) const
  { // To hold path name
    wstring wstrP; wstrP.resize(MAX_PATH);
    // Get folder path name
    SHGetSpecialFolderPath(nullptr, const_cast<wchar_t*>(wstrP.c_str()),
      iCSIDL, true);
    // Resize string to length. Shame the API doesn't return the length :-(
    wstrP.resize(wcslen(wstrP.c_str()));
    // Return pathname
    return wstrP;
  }
  /* ----------------------------------------------------------------------- */
  unsigned int DetectWindowsArchitechture(void)
  { // Grab appropriate kernel function. It only exists on 64-bit versions
    // of Windows XP, Vista, 7, 8, 8.1 and 10. If this succeeds?
    typedef void (WINAPI*const LPFN_GETNATIVESYSTEMINFO)(LPSYSTEM_INFO);
    if(LPFN_GETNATIVESYSTEMINFO fnGetNativeSystemInfo =
      GetKernelFunc<LPFN_GETNATIVESYSTEMINFO>("GetNativeSystemInfo"))
    { // Get operating system HAL information (returns nothing).
      // https://docs.microsoft.com/en-us/windows/win32
      //   /api/sysinfoapi/nf-sysinfoapi-getnativesysteminfo
      SYSTEM_INFO siD;
      fnGetNativeSystemInfo(&siD);
      // We now know if it's a 64-bit OS!
      return siD.wProcessorArchitecture ==
        PROCESSOR_ARCHITECTURE_AMD64 ? 64 : 32;
    } // Failed so if the function was not found? Then it's a 32-bit OS.
    if(SysIsErrorCode(ERROR_PROC_NOT_FOUND)) return 32;
    // Show other error
    XCS("Failed to get native system info function address!");
  }
  /* ----------------------------------------------------------------------- */
  const string GetLocale(const LCID lcidLocale)
  { // Build language and country code from system and return it
    return
      StrAppend(WS16toUTF(GetLocaleString(LOCALE_SISO639LANGNAME, lcidLocale)),
        '-', WS16toUTF(GetLocaleString(LOCALE_SISO3166CTRYNAME, lcidLocale)));
  }
  /* ----------------------------------------------------------------------- */
  OSData GetOperatingSystemData(void)
  { // Operating system data. Fuck you Microsoft. I'm still supporting XP.
    // > https://docs.microsoft.com/en-us/windows/win32/api/
    //     sysinfoapi/nf-sysinfoapi-getversionexw
    OSVERSIONINFOEX osviData;
    osviData.dwOSVersionInfoSize = sizeof(osviData);
    // Typedef for getversionex (Supported in Win2K+)
    typedef BOOL (WINAPI*const LPFN_GETOSVERSIONEXW)(LPOSVERSIONINFOW);
    const LPFN_GETOSVERSIONEXW fcbGVEW =
      GetKernelFunc<LPFN_GETOSVERSIONEXW>("GetVersionExW");
    // Now we can get version if we have the fuction
    if(!fcbGVEW || !fcbGVEW(reinterpret_cast<LPOSVERSIONINFOW>(&osviData)))
      XCS("Failed to query operating system version!");
    // Set operating system version string
    ostringstream osOS; osOS << "Windows ";
    // Version information table
    struct OSListItem
    { // Label to append if verified
      const char*const cpLabel;
      // Major, minor and service pack of OS which applies to this label
      const unsigned int uiHi, uiLo, uiBd, uiSp;
    };
    // List of recognised Windows versions
    static const array<const OSListItem,39>osList{ {
      { "11 24H2+", 10, 0, 26100, 0 }, { "11 23H2",  10, 0, 22631, 0 },
      { "11 22H2",  10, 0, 22621, 0 }, { "11 21H2",  10, 0, 22000, 0 },
      { "10 22H2",  10, 0, 19045, 0 }, { "10 21H2",  10, 0, 19044, 0 },
      { "10 21H1",  10, 0, 19043, 0 }, { "10 20H2",  10, 0, 19042, 0 },
      { "10 20H1",  10, 0, 19041, 0 }, { "10 19H2",  10, 0, 18363, 0 },
      { "10 19H1",  10, 0, 18362, 0 }, { "10 RS5",   10, 0, 17763, 0 },
      { "10 RS4",   10, 0, 17134, 0 }, { "10 RS3",   10, 0, 16299, 0 },
      { "10 RS2",   10, 0, 15063, 0 }, { "10 RS1",   10, 0, 14393, 0 },
      { "10 T2",    10, 0, 10586, 0 }, { "10 T1",    10, 0, 10240, 0 },
      { "10",       10, 0,     0, 0 }, { "8.1",       6, 3,     0, 0 },
      { "8",         6, 2,     0, 0 }, { "7 SP1",     6, 1,     0, 1 },
      { "7",         6, 1,     0, 0 }, { "Vista SP2", 6, 0,     0, 2 },
      { "Vista SP1", 6, 0,     0, 1 }, { "Vista",     6, 0,     0, 0 },
      { "2003 SP3",  5, 2,     0, 3 }, { "2003 SP2",  5, 2,     0, 2 },
      { "2003 SP1",  5, 2,     0, 1 }, { "2003",      5, 2,     0, 0 },
      { "XP SP3",    5, 1,     0, 3 }, { "XP SP2",    5, 1,     0, 2 },
      { "XP SP1",    5, 1,     0, 1 }, { "XP",        5, 1,     0, 0 },
      { "2K SP4",    5, 0,     0, 4 }, { "2K SP3",    5, 0,     0, 3 },
      { "2K SP2",    5, 0,     0, 2 }, { "2K SP1",    5, 0,     0, 1 },
      { "2K",        5, 0,     0, 0 },
    } };
    // Iterate through the versions and try to find a match for the
    // versions above. 'Unknown' is caught if none are found.
    for(const OSListItem &oslIt : osList)
    { // Ignore if this version item doesn't match
      if(osviData.dwMajorVersion < oslIt.uiHi ||
         osviData.dwMinorVersion < oslIt.uiLo ||
         osviData.dwBuildNumber < oslIt.uiBd ||
         osviData.wServicePackMajor < oslIt.uiSp) continue;
      // Set operating system version
      osOS << oslIt.cpLabel;
      // Skip adding version numbers
      goto SkipNumericalVersionNumber;
    } // Nothing was found so add version number detected
    osOS << osviData.dwMajorVersion << '.' << osviData.dwMinorVersion;
    // Label for when we found the a matching version
    SkipNumericalVersionNumber:
    // Add server label if is a server based operating system
    if(osviData.wProductType != VER_NT_WORKSTATION) osOS << " Server";
    // Look for Wine and set extra info if available
    string strExtra; bool bExtra;
    if(HMODULE hDLL = GetModuleHandle(L"ntdll"))
    { // Get wine version function and if succeeded?
      typedef const char *(WINAPI*const LPWINEGETVERSION)(void);
      if(LPWINEGETVERSION fcbWGV =
        GetSharedFunc<LPWINEGETVERSION>(hDLL, "wine_get_version"))
          strExtra = StrAppend("Wine ", fcbWGV()), bExtra = true;
      else bExtra = false;
    } // Store if we have extra info because strExtra is being StdMove()'d
    else bExtra = false;
    // Return data
    return {
      osOS.str(),                            // Version string
      StdMove(strExtra),                     // Extra version string
      osviData.dwMajorVersion,               // Major OS version
      osviData.dwMinorVersion,               // Minor OS version
      osviData.dwBuildNumber,                // OS build version
      DetectWindowsArchitechture(),          // 32 or 64 OS arch
      GetLocale(GetUserDefaultUILanguage()), // Get locale
      DetectElevation(),                     // Elevated?
      bExtra || osviData.dwMajorVersion < 6  // Wine or Old OS?
    };
  }
  /* ----------------------------------------------------------------------- */
  ExeData GetExecutableData(void)
  { // Get this executables checksum and show error if failed
    DWORD dwHeaderSum, dwCheckSum;
    if(const DWORD dwResult =
         MapFileAndCheckSum(UTFtoS16(ENGFull()).c_str(),
           &dwHeaderSum, &dwCheckSum))
      XCS("Error reading the executable checksum!",
          "Executable", ENGFull(), "Result", dwResult);
    // Return data
    return { dwHeaderSum, dwCheckSum, dwHeaderSum != dwCheckSum, false };
  }
  /* ----------------------------------------------------------------------- */
  CPUData GetProcessorData(void)
  { // Try to open the specified below registry key and if successful?
    const string strK{ "HARDWARE\\DESCRIPTION\\System\\CentralProcessor" };
    if(const SysReg srRoot{ HKEY_LOCAL_MACHINE, strK, KEY_ENUMERATE_SUB_KEYS })
    { // Enumerate subkeys
      const StrVector svKeys{ srRoot.QuerySubKeys() };
      // Open first subkey, usually "0" and if succeeded?
      const string &strSK = *svKeys.cbegin();
      if(const SysReg srSub{ srRoot.GetHandle(), strSK, KEY_QUERY_VALUE })
      { // Query required values
        string strVendor{ srSub.QueryString("VendorIdentifier") },
               strName{ srSub.QueryString("ProcessorNameString") },
               strIdent{ srSub.QueryString("Identifier") };
        // Remove unnecessary whitespaces from strings
        StrCompactRef(strVendor);
        StrCompactRef(strName);
        StrCompactRef(strIdent);
        // Fail-safe empty strings
        if(strVendor.empty()) strVendor = cCommon->CommonUnspec();
        if(strName.empty()) strName = strVendor;
        if(strIdent.empty()) strIdent = cCommon->CommonUnspec();
        // Detect family model and stepping from string (A F 0 M 0 S)
        unsigned int uiFamily, uiModel, uiStepping;
        Token tTokens{ strIdent, cCommon->CommonSpace() };
        if(tTokens.size() >= 7 && tTokens[1] == "Family" &&
          tTokens[3] == "Model" && tTokens[5] == "Stepping")
        { // Convert strings to numbers
          uiFamily = StrToNum<unsigned int>(tTokens[2]);
          uiModel = StrToNum<unsigned int>(tTokens[4]);
          uiStepping= StrToNum<unsigned int>(tTokens[6]);
        } // Invalid syntax
        else uiFamily = uiModel = uiStepping = 0;
        // Return data
        return { StdThreadMax(), srSub.Query<DWORD>("~MHz"),
                 uiFamily, uiModel, uiStepping, strName };
      } // Log that we couldn't open the subkey
      else cLog->LogWarningExSafe("System could not open registry key $ "
        "sub-key $! $", strK, strSK, SysError());
    } // Log that we couldn't open the root key
    else cLog->LogWarningExSafe("System could not open registry key $! $",
      strK, SysError());
    // Return default data we could not read
    return { StdThreadMax(), 0, 0, 0, 0, cCommon->CommonUnspec() };
  }
  /* ----------------------------------------------------------------------- */
  void UpdateMemoryUsageData(void)
  { // Get process memory info.
    PROCESS_MEMORY_COUNTERS pmcData;
    if(!GetProcessMemoryInfo(hProcess, &pmcData, sizeof(pmcData))) return;
    // Global memory info (64-bit)
    MEMORYSTATUSEX msD;
    msD.dwLength = sizeof(msD);
    if(!GlobalMemoryStatusEx(&msD)) return;
    // Set memory data
    memData.qMTotal = msD.ullTotalPhys;
    memData.qMFree = msD.ullAvailPhys;
    memData.qMUsed = msD.ullTotalPhys - msD.ullAvailPhys;
#if defined(X64)                       // 64-bit?
    memData.stMFree =
      static_cast<size_t>(UtilMinimum(msD.ullAvailPhys, 0xFFFFFFFF));
#elif defined(X86)                     // 32-bit?
    memData.stMFree = msD.ullAvailPhys <= 0xFFFFFFFF ?
      static_cast<size_t>(msD.ullAvailPhys) : 0 - pmcData.WorkingSetSize;
#endif                                 // Bits check
    memData.dMLoad = UtilMakePercentage(memData.qMUsed, msD.ullTotalPhys);
    memData.stMProcUse = pmcData.WorkingSetSize;
    memData.stMProcPeak = pmcData.PeakWorkingSetSize;
  }
  /* ----------------------------------------------------------------------- */
  bool DebuggerRunning(void) const
    { return !!IsDebuggerPresent(); }
  /* -- Get process affinity masks ----------------------------------------- */
  uint64_t GetAffinity(const bool bS)
  { // Get current affinity and return if successful
    DWORD_PTR dwAffinity, dwSysAffinity;
    if(GetProcessAffinityMask(hProcess, &dwAffinity, &dwSysAffinity))
      return static_cast<uint64_t>(bS ? dwSysAffinity : dwAffinity);
    // Failed so throw exception
    XCS("Failed to acquire process affinity!", "Handle", hProcess);
  }
  /* ----------------------------------------------------------------------- */
  DWORD GetPriority(void) const
  { // Get priority class and if successful? Return it
    if(const DWORD dwPriClass = GetPriorityClass(hProcess)) return dwPriClass;
    // Failed so throw exception
    XCS("Failed to acquire priority class", "Handle", hProcess);
  }
  /* ---------------------------------------------------------------------- */
  bool DetectElevation(void)
  { // Process token
    HANDLE hToken = INVALID_HANDLE_VALUE;
    // Open access token and ignore if failed
    if(!OpenProcessToken(hProcess, TOKEN_QUERY, &hToken)) return false;
    // Bytes return getting token
    DWORD dwBytes = 0;
    // Running as admin boolean
    bool bAdmin;
    // Get elevation status and if succeeded and has a linked token?
    TOKEN_ELEVATION_TYPE tokenElevationType;
    if(GetTokenInformation(hToken, TokenElevationType, &tokenElevationType,
        sizeof(tokenElevationType), &dwBytes) &&
      tokenElevationType != TokenElevationTypeDefault)
      // Running as admin if running as full elevation
      bAdmin = tokenElevationType == TokenElevationTypeFull;
    // Else use standard function (if XP or no linked token)
    else bAdmin = !!IsUserAnAdmin();
    // If handle not opened we're done
    if(hToken != INVALID_HANDLE_VALUE) CloseHandle(hToken);
    // Return result
   return bAdmin;
  }
  /* -- Entropy generator -------------------------------------------------- */
  Memory GetEntropy(void) const
  { // Entropy data structure to return to openssl. Should be enough I think!
    struct EntropyData
    { SYSTEMTIME            sSTime, sLTime;      // System times
      POINT                 pPos;                // Cursor position
      TIME_ZONE_INFORMATION tzData;              // Time zone information
      FILETIME              cpuD[7];             // Process and system times
      LARGE_INTEGER         liD[2];              // Current hires timers
    };
    // Allocate memory and assign a reference structure to this memory
    Memory mData{ sizeof(EntropyData) };
    EntropyData &edData = *mData.MemPtr<EntropyData>();
    // System time and local time entropy (Both return void).
    GetSystemTime(&edData.sSTime);
    GetLocalTime(&edData.sLTime);
    // Cursor position entropy
    if(!GetCursorPos(&edData.pPos))
      XCS("Failed to query cursor position!");
    // Time zone information
    if(!GetTimeZoneInformation(&edData.tzData))
      XCS("Failed to query timezone information!");
    // Cpu process times
    if(!GetProcessTimes(hProcess, &edData.cpuD[0], &edData.cpuD[1],
                                  &edData.cpuD[2], &edData.cpuD[3]))
      XCS("Failed to query process times!");
    // Cpu system times
    if(!GetSystemTimes(&edData.cpuD[4], &edData.cpuD[5], &edData.cpuD[6]))
      XCS("Failed to query system times!");
    // Cpu counters
    if(!QueryPerformanceFrequency(&edData.liD[0]))
      XCS("Failed to query performance frequency!");
    if(!QueryPerformanceCounter(&edData.liD[1]))
      XCS("Failed to query performance counter!");
    // Return data
    return mData;
  }
  /* ---------------------------------------------------------------------- */
  void WindowInitialised(GlFW::GLFWwindow*const gwWindow)
  { // If we don't have a GlFW window?
    if(!gwWindow) return;
    // Set handles to the GLFW window that was created, or the console. The
    // handle should be valid 100% of the time but check just incase
    SetWindowHandle(GlFW::glfwGetWin32Window(gwWindow));
    if(IsNotWindowHandleSet()) XC("Failed to get window handle from GlFW!");
    // Because GLFW has a horrible white background, let's make it a better
    // colour from the windows theme to not blind people.
    if(const HBRUSH hBrush = CreateSolidBrush(GetSysColor(COLOR_BTNFACE)))
    { // Set the new brush and delete  the old one
      if(const HBRUSH hOldBrush = reinterpret_cast<HBRUSH>
        (SetClassLongPtr(GetWindowHandle(), GCLP_HBRBACKGROUND,
          reinterpret_cast<LONG_PTR>(hBrush)))) DeleteObject(hOldBrush);
      // It's not a big deal if it fails but log it anyway. This can return
      // nullptr even when there is no error so check for an error first.
      else if(SysIsNotErrorCode())
        cLog->LogWarningExSafe(
          "System failed to apply new window background: $!", SysError());
      // Redraw the background
      if(!InvalidateRect(GetWindowHandle(), nullptr, 1))
        cLog->LogWarningExSafe(
          "System failed to invalidate window background: $!", SysError());
    } // It's not a big deal if this fails
    else cLog->LogWarningExSafe(
      "System failed to create new window brush: $!", SysError());
  }
  /* ----------------------------------------------------------------------- */
  int LastSocketOrSysError(void)
  { // Last last socket error
    const int iLastError = static_cast<int>(WSAGetLastError());
    // Use that or actual last error
    return static_cast<int>(iLastError ? iLastError : SysErrorCode<int>());
  }
  /* -- Build user roaming directory ---------------------------- */ protected:
  const string BuildRoamingDir(void) const
    { return cCmdLine->CmdLineMakeEnvPath("APPDATA", cCommon->CommonBlank()); }
  /* -- Constructor (only derivable) --------------------------------------- */
  SysCore(void) :
    /* -- Initialisers ----------------------------------------------------- */
    SysVersion{ EnumModules(),         // Enumerate modules
      reinterpret_cast<size_t>         // Stored as 'size_t' for cross-platform
        (hInstance) },                 // Send this processes instance handle
    SysCommon{ GetExecutableData(),    // Get and store executable data
             GetOperatingSystemData(), // Get and store operating system data
               GetProcessorData() },   // Get and store processor data
    hIconLarge(nullptr),               // Large icon not initialised yet
    hIconSmall(nullptr),               // Small icon not initialised yet
    SysCon { this->OSNameEx() }        // Send Wine version to console
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Destructor (only derivable) ---------------------------------------- */
  ~SysCore(void)
  { // Destroy large and small icon if created
    if(hIconLarge) DestroyIcon(hIconLarge);
    if(hIconSmall) DestroyIcon(hIconSmall);
  }
}; /* ---------------------------------------------------------------------- */
/* == EoF =========================================================== EoF == */
