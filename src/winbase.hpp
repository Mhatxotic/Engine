/* == WINBASE.HPP ========================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## This is a Windows specific module that handles unhandled exceptions ## **
** ## and writes debugging information to disk to help resolve bugs.      ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* == We'll put all these calls in a namespace ============================= */
class SysBase                          // Members initially private
{ /* -- Custom exceptions -------------------------------------------------- */
  constexpr static const DWORD         // Custom exceptions
    EXCEPTION_ABORT      = 0xF0000001, // Abornmal program termination
    EXCEPTION_ISA        = 0xF0000002, // Illegal storage access
    EXCEPTION_FPOINT     = 0xF0000003; // Floating point exception
  /* -- Private typedefs --------------------------------------------------- */
  typedef IdMap<const DWORD> ExCoList; // List of Win32 exception strings
  const ExCoList   eclStrings;         // Exception strings strings
  /* -- Private variables -------------------------------------------------- */
  HWND             hwndWindow;         // Main window handle being used
  /* ----------------------------------------------------------------------- */
  const UINT       uiOldErrorMode;            // Old error mode
  const _crt_signal_t fcbAbortCallback,       // Saved abort callback
                   fcbIllegalStorageAccess,   // " illegal storage access cb
                   fcbFloatingPointException; // " float point exception cb
  /* == Get environment ==================================================== */
  void SEHDumpEnvironment(StdOStringStream &osS) try
  { // Prepare formatted data
    Statistic tD;
    tD.Header("Variable").Header("Value", false).Reserve(10);
    // Get environment strings and if we have them?
    if(wchar_t*const wcpE = GetEnvironmentStrings())
    { // Iterate through the environment lines
      for(wchar_t *wcpEP = wcpE, *wcpN = wcpE + wcslen(wcpEP) + 1;
                  *wcpEP;
                   wcpEP = wcpN,  wcpN = wcpEP + wcslen(wcpEP) + 1)
      { // Find value and if we found it?
        if(wchar_t *wcpV = wcschr(wcpEP, '='))
        { // Remove equals
          *wcpV = 0;
          // Add variable and value
          tD.DataW(wcpEP).DataW(++wcpV);
          // Done
          continue;
        } // Not found. Add line as value
        tD.Data().DataW(wcpEP);
      } // Free the environment strings
      FreeEnvironmentStrings(wcpE);
    } // Return formatted data
    tD.Finish(osS);
  } // Shouldn't happen but just incase
  catch(const StdException &eReason) { osS << eReason.what(); }
  /* == Get memory information ============================================= */
  void SEHDumpMemoryStatus(StdOStringStream &osS) try
  { // Get process memory info
    PROCESS_MEMORY_COUNTERS pD;
    pD.cb = sizeof(pD);
    GetProcessMemoryInfo(GetCurrentProcess(), &pD, sizeof(pD));
    // Get memory info
    MEMORYSTATUSEX mD;
    mD.dwLength = sizeof(mD);
    GlobalMemoryStatusEx(&mD);
    // Process, format and return data
    Statistic()
      .Header("Type")
      .Header("Total")
      .Header("Free")
      .Header("Used")
      .Reserve(5)
      // Process memory
      .Data("Process")
      .DataB(pD.PeakWorkingSetSize, 2)
      .Data()
      .DataB(pD.WorkingSetSize, 2)
      // Physical memory
      .Data("Physical")
      .DataB(mD.ullTotalPhys, 2)
      .DataB(mD.ullAvailPhys, 2)
      .DataB(mD.ullTotalPhys - mD.ullAvailPhys, 2)
      // Paged memory
      .Data("Page")
      .DataB(mD.ullTotalPageFile, 2)
      .DataB(mD.ullAvailPageFile, 2)
      .DataB(mD.ullTotalPageFile - mD.ullAvailPageFile, 2)
      // Virtual memory
      .Data("Virtual")
      .DataB(mD.ullTotalVirtual, 2)
      .DataB(mD.ullAvailVirtual, 2)
      .DataB(mD.ullTotalVirtual - mD.ullAvailVirtual, 2)
      // Extended virtual memory
      .Data("XVirtual")
      .Data()
      .DataB(mD.ullAvailExtendedVirtual, 2)
      .DataN(int(mD.dwMemoryLoad) + '%').Finish(osS);
  } // Shouldn't happen but just incase
  catch(const StdException &eReason) { osS << eReason.what(); }
  /* == Get executable filename ============================================ */
  const StdWideString SEHGetExecutableFileNameWithoutExtension()
  { // Storage for executable and crash log file name
    StdResized<StdWideString> wstrExe{ MAX_PATH };
    // Get executable file name
    wstrExe.resize(GetModuleFileName(nullptr,
      const_cast<wchar_t*>(wstrExe.data()), MAX_PATH));
    // Remove extension if we can
    if(wstrExe.length() >= 4 && wstrExe[wstrExe.length() - 4] == '.')
      wstrExe.resize(wstrExe.length() - 4);
    // Return result
    return wstrExe;
  }
  /* == Dump exception memory addresses ==================================== */
  void SEHDumpExceptionMemoryAddresses(StdOStringStream &osS,
    const EXCEPTION_RECORD &erData) try
  { // Bail if exception code does not support this
    switch(erData.ExceptionCode)
    { // The first element of the array contains a read-write flag that
      // indicates the type of operation that caused the access violation. If
      // this value is zero, the thread attempted to read the inaccessible
      // data. If this value is 1, the thread attempted to write to an
      // inaccessible address. If this value is 8, the thread causes a
      // user-mode data execution prevention (DEP) violation.
      // The second array element specifies the virtual address of the
      // inaccessible data.
      // The third array element specifies the underlying NTSTATUS code that
      // resulted in the exception.
      case EXCEPTION_IN_PAGE_ERROR:
      // The first element of the array contains a read-write flag that
      // indicates the type of operation that caused the access violation. If
      // this value is zero, the thread attempted to read the inaccessible
      // data. If this value is 1, the thread attempted to write to an
      // inaccessible address. If this value is 8, the thread causes a
      // user-mode data execution prevention (DEP) violation.
      // The second array element specifies the virtual address of the
      // inaccessible data.
      case EXCEPTION_ACCESS_VIOLATION: break;
      // No other codes have exception information to show
      default: return;
    } // For each parameter. Clamping just incase
    for(DWORD dwParam = 0,
      dwMax = UtilMinimum(erData.NumberParameters,
                static_cast<DWORD>(EXCEPTION_MAXIMUM_PARAMETERS));
              dwParam < dwMax;
              dwParam += 2)
    { // Write where the access occured
      osS << cCommon->CommonCrLf() << "The memory at 0x"
          << reinterpret_cast<void*>(erData.ExceptionInformation[dwParam+1])
          << " could not be ";
      // Write the action attempted
      switch(const
#if defined(X64)
        DWORD64
#elif defined(X86)
        DWORD
#endif
        &dwAccess = erData.ExceptionInformation[dwParam])
      { // Read, written or executed?
        case 0: osS << "read."; break;
        case 1: osS << "written."; break;
        case 8: osS << "executed."; break;
        // Unknown? (Shouldn't get here)
        default: osS << "0x" << StdIOSHex << dwAccess << '.'; break;
      } // Was an in-page error? Move the parameter count onwards by one and
      // plus two to get the third element. The next iteration will push by
      // two.
      if(erData.ExceptionCode == EXCEPTION_IN_PAGE_ERROR)
        osS << cCommon->CommonCrLf() << "An in page error occured with code 0x"
            << erData.ExceptionInformation[(dwParam++)+2]
            << '!';
    }
  } // Shouldn't happen but just incase
  catch(const StdException &eReason) { osS << eReason.what(); }
  /* == Dump registers to string =========================================== */
  void SEHDumpRegisters(StdOStringStream &osS, const CONTEXT*const pcData) try
  { // Invalid context?
    if(!pcData)
      throw StdRunTimeError{ "No registers when context is null pointer!" };
    // Get context from exception record
    const CONTEXT &cData = *pcData;
    // Helper macros
#define PUSHINT(id,c,x,e) id "=" << StdIOSSetWidth(c) << cData.x << e
#define D64(id,x,e) PUSHINT(id,16,x,e)
#define D32(id,x,e) PUSHINT(id,8,x,e)
#define D16(id,x,e) PUSHINT(id,4,x,e)
#define D32X(id,x,e) id "=" \
      << StdIOSSetWidth(8) << *reinterpret_cast<const uint32_t*>(&cData.x) << e
#define D128X(id,x,e) id "=" \
      << StdIOSSetWidth(16) << *reinterpret_cast<const uint64_t*>(&cData.x) \
      << StdIOSSetWidth(16) \
      << *(reinterpret_cast<const uint64_t*>(&cData.x)+1) << e
    const StdString &strCrLf = cCommon->CommonCrLf(),
                 &strSpc = cCommon->CommonDblSpace();
    // Return registers
#if defined(X64)
    // Write basic registers
    osS << StdIOSHex << StdIOSSetFill('0')
      << D64("Rax", Rax, strSpc)  << D64("Rbx", Rbx, strSpc)
      << D64("Rcx", Rcx, strCrLf) << D64("Rdx", Rdx, strSpc)
      << D64("Rsp", Rsp, strSpc)  << D64("Rbp", Rbp, strCrLf)
      << D64("Rip", Rip, strSpc)  << D64("Rsi", Rsi, strSpc)
      << D64("Rdi", Rdi, strCrLf) << strCrLf
      << D16("SegCs", SegCs, strSpc) << D16("SegDs", SegDs, strSpc)
      << D16("SegEs", SegEs, strSpc) << D16("SegFs", SegFs, strSpc)
      << D16("SegGs", SegGs, strSpc) << D16("SegSs", SegSs, strCrLf) << strCrLf
      << D32("CFlags", ContextFlags, strSpc) << D32("MxCsr", MxCsr, strSpc)
      << D32("EFlags", EFlags, strCrLf) << strCrLf
      << D64("P1H", P1Home, strSpc)  << D64("P2H", P2Home, strSpc)
      << D64("P3H", P3Home, strCrLf) << D64("P4H", P4Home, strSpc)
      << D64("P5H", P5Home, strSpc)  << D64("P6H", P6Home, strCrLf) << strCrLf
      << D64("Dr0", Dr0, strSpc)  << D64("Dr1", Dr1, strSpc)
      << D64("Dr2", Dr2, strCrLf) << D64("Dr3", Dr3, strSpc)
      << D64("Dr6", Dr6, strSpc)  << D64("Dr7", Dr7, strCrLf) << strCrLf
      << D64("R08", R8,  strSpc)  << D64("R09", R9,  strSpc)
      << D64("R10", R10, strCrLf) << D64("R11", R11, strSpc)
      << D64("R12", R12, strSpc)  << D64("R13", R13, strCrLf)
      << D64("R14", R14, strSpc)  << D64("R15", R15, strCrLf) << strCrLf
      << D64("VCon", VectorControl, strSpc)
      << D64("DCon", DebugControl, strSpc)
      << D64("LBtR", LastBranchToRip, strCrLf)
      << D64("LBfR", LastBranchFromRip, strSpc)
      << D64("LEtR", LastExceptionToRip, strSpc)
      << D64("LEfR", LastExceptionFromRip, strCrLf) << strCrLf;
    // Write floating point header state
    for(size_t stQuad = 0; stQuad < 2; stQuad += 2)
      osS << D128X("XmmH" << StdIOSDec << stQuad << StdIOSHex <<,
        Header[stQuad], strSpc)
          << D128X("XmmH" << StdIOSDec << (stQuad+1) << StdIOSHex <<,
        Header[stQuad+1], strCrLf);
    // Write floating point legacy state
    for(size_t stQuad = 0; stQuad < 8; stQuad += 2)
      osS << D128X("XmmL" << StdIOSDec << stQuad << StdIOSHex <<,
        Legacy[stQuad], strSpc)
          << D128X("XmmL" << StdIOSDec << (stQuad+1) << StdIOSHex <<,
        Legacy[stQuad+1], strCrLf);
    // Write floating point state
# define XMM(x,e) << \
      D128X("Xmm" << StdIOSSetWidth(2) << StdIOSDec << x << StdIOSHex <<, \
        Xmm ## x, e)
    osS XMM( 0,strSpc)  XMM( 1,strCrLf) XMM( 2,strSpc)  XMM( 3,strCrLf)
        XMM( 4,strSpc)  XMM( 5,strCrLf) XMM( 6,strSpc)  XMM( 7,strCrLf)
        XMM( 8,strSpc)  XMM( 9,strCrLf) XMM(10,strSpc)  XMM(11,strCrLf)
        XMM(12,strSpc)  XMM(13,strCrLf) XMM(14,strSpc)  XMM(15,strCrLf)
     << strCrLf;
# undef XMM
    // Write vector state
    for(size_t stQuad = 0; stQuad < 26; stQuad += 2)
      osS << D128X("Vec" << StdIOSSetWidth(2) << StdIOSDec << stQuad
          << StdIOSHex <<, VectorRegister[stQuad], strSpc)
          << D128X("Vec" << StdIOSSetWidth(2) << StdIOSDec << (stQuad+1)
          << StdIOSHex <<, VectorRegister[stQuad+1], strCrLf);
    // Using 32-bit compiler?
#elif defined(X86)
    // Write basic registers
    osS << StdIOSHex << StdIOSSetFill('0')
      << D32("Eax", Eax, strSpc) << D32("Ebx", Ebx, strSpc)
      << D32("Ecx", Ecx, strSpc) << D32("Edx", Edx, strCrLf)
      << D32("Esp", Esp, strSpc) << D32("Ebp", Ebp, strSpc)
      << D32("Esi", Esi, strSpc) << D32("Edi", Edi, strSpc)
      << D32("Eip", Eip, strCrLf) << strCrLf
      << D32("SegCs", SegCs, strSpc) << D32("SegDs", SegDs, strSpc)
      << D32("SegEs", SegEs, strCrLf) << D32("SegFs", SegFs, strSpc)
      << D32("SegGs", SegGs, strSpc) << D32("SegSs", SegSs, strCrLf) << strCrLf
      << D32("CFlags", ContextFlags, strSpc)
      << D32("EFlags", EFlags, strCrLf) << strCrLf
      << D32("Dr0", Dr0, strSpc) << D32("Dr1", Dr1, strSpc)
      << D32("Dr2", Dr2, strCrLf) << D32("Dr3", Dr3, strSpc)
      << D32("Dr6", Dr6, strSpc) << D32("Dr7", Dr7, strCrLf) << strCrLf
      << D32("FCW", FloatSave.ControlWord, strSpc)
      << D32("FSW", FloatSave.StatusWord, strSpc)
      << D32("FTW", FloatSave.TagWord, strSpc)
      << D32("FES", FloatSave.ErrorSelector, strCrLf)
      << D32("FDO", FloatSave.DataOffset, strSpc)
      << D32("FDS", FloatSave.DataSelector, strSpc)
      << D32("FNS", FloatSave.Spare0, strCrLf) << strCrLf;
    // Write floating point state
    for(size_t stI = 0, stY = 0, stZ = 5 % WOW64_SIZE_OF_80387_REGISTERS;
               stY < WOW64_SIZE_OF_80387_REGISTERS;
               stY += sizeof(DWORD)*5)
      for(size_t stX = 0; stX < stZ; ++stX, ++stI)
        osS << D32X("FRA" << StdIOSSetWidth(2) << StdIOSDec << stI
            << StdIOSHex <<, FloatSave.RegisterArea[stY+(stX*sizeof(DWORD))],
              (stX == 4 ? strCrLf : strSpc));
    osS << strCrLf;
    // Write extended registers state
    for(size_t stI = 0, stY = 0, stZ = 5 % WOW64_MAXIMUM_SUPPORTED_EXTENSION;
               stY < WOW64_MAXIMUM_SUPPORTED_EXTENSION;
               stY += sizeof(DWORD)*5)
      for(size_t stX = 0; stX < stZ; ++stX, ++stI)
        osS << D32X("ER" << StdIOSSetWidth(3) << StdIOSDec << stI << StdIOSHex
            <<, ExtendedRegisters[stY+(stX*sizeof(DWORD))],
                  (stX == 4 ? strCrLf : strSpc));
#endif
    // Set fill back to space
    osS << StdIOSSetFill(' ');
    // Done with helper macros
#undef D128
#undef D16
#undef D32
#undef D64
#undef PUSHINT
  } // Shouldn't happen but just incase
  catch(const StdException &eReason) { osS << eReason.what(); }
  /* == Perform process dump =============================================== */
  void SEHProcessDump(StdOStringStream &osS) try
  { // Prepare formatted data
    Statistic tData;
    tData.Header("Name").Header("Pid").Header("PPid").Header("Thr")
         .Header("Pri").Header("Aff").Header("Version", false)
         .Header("Description", false).Header("Vendor", false)
         .Header("Path", false).Reserve(10);
    // Storage for filename
    StdResized<StdWideString>{ MAX_PATH };
    // Show modules
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    // Capture exceptions so we can clean up the snapshot handle
    try
    { // Process info
      PROCESSENTRY32 pedData;
      pedData.dwSize = sizeof(pedData);
      // Get first item and iterate through each process
      if(Process32First(hSnapshot, &pedData) == TRUE) do
      { // Add pid, parent pid, thread count and name
        tData.DataW(pedData.szExeFile)
             .DataN(pedData.th32ProcessID)
             .DataN(pedData.th32ParentProcessID)
             .DataN(pedData.cntThreads)
             .DataN(pedData.pcPriClassBase);
        // Open Process. This will fail on some processes, but not all
        HMODULE hProcess = reinterpret_cast<HMODULE>(OpenProcess(
          PROCESS_QUERY_INFORMATION|PROCESS_VM_READ, FALSE,
          pedData.th32ProcessID));
        // If failed?
        if(!hProcess)
        { // Push no affinity, blank version, error as description,
          // error number in vendor, reason for error as filename.
          tData.DataN(0)
               .Data("N/A")
               .Data("OpenProcess() failed!")
               .DataF("Error ", SysErrorCode())
               .Data(SysError());
          // Next process
          continue;
        } // Capture exceptions so we can clean up the snapshot handle
        try
        { // Get affinity mask and if failed
          DWORD_PTR dwMask, dwSys;
          tData.DataH(GetProcessAffinityMask(hProcess,
            &dwMask, &dwSys) ? dwMask : 0);
          // Get module file name and if succeeded?
          StdResized<StdWideString> wstrFN{ MAX_PATH };
          if(GetModuleFileNameEx(hProcess, nullptr,
            const_cast<LPWSTR>(wstrFN.data()), MAX_PATH))
          { // Get version information
            const SysModuleData vD{ StdMove(SysModule{ WS16toUTF(wstrFN) }) };
            // Push version, description vendor and filename (use .data())
            tData.DataF("$.$.$.$", vD.GetMajor(), vD.GetMinor(),
                    vD.GetRevision(), vD.GetBuild())
                 .Data(StdMove(vD.GetDesc()))
                 .Data(StdMove(vD.GetVendor()))
                 .DataW(wstrFN);
          } // Push blank field, error as description, number and reason
          else tData.Data()
                    .Data("GetModuleFileNameEx() failed!")
                    .DataA("Error ", SysErrorCode())
                    .Data(SysError());
          // Done with process handle
          CloseHandle(hProcess);
        } // exception occured?
        catch(const StdException &)
        { // Close the process handle
          CloseHandle(hProcess);
          // Rethrow the exception
          throw;
        }
      } // Continue if there is more
      while(Process32Next(hSnapshot, &pedData) == TRUE);
      // Done with snapshot
      CloseHandle(hSnapshot);
    } // exception occured?
    catch(const StdException &)
    { // Close the snapshot handle
      CloseHandle(hSnapshot);
      // Rethrow the exception
      throw;
    } // Build output into string stream
    tData.Finish(osS);
  } // Shouldn't happen but just incase
  catch(const StdException &eReason) { osS << eReason.what(); }
  /* == Perform module dump ================================================ */
  void SEHModuleDump(StdOStringStream &osS) try
  { // Prepare formatted data
    Statistic tD;
    tD.Header("Description").Header("Version", false)
      .Header("Vendor", false).Header("Module", false)
      .Reserve(10);
    // Show modules
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, 0);
    // Capture exceptions so we can close the handle
    try
    { // Module info
      MODULEENTRY32 medData;
      medData.dwSize = sizeof(medData);
      // Get first module and iterate each one
      if(Module32First(hSnapshot, &medData) == TRUE) do
      { // Get module information
        const SysModuleData vD{
          StdMove(SysModule{ S16toUTF(medData.szExePath) }) };
        // Print data
        tD.Data(vD.GetDesc()).Data(vD.GetVersion())
          .Data(vD.GetVendor()).DataW(medData.szExePath);
      } // Continue if there is more
      while(Module32Next(hSnapshot, &medData));
      // Done with handle
      CloseHandle(hSnapshot);
    } // If exception occured?
    catch(const StdException &)
    { // Close handle
      CloseHandle(hSnapshot);
      // Rethrow
      throw;
    } // Build output into string stream
    tD.Finish(osS);
  } // Shouldn't happen but just incase
  catch(const StdException &eReason) { osS << eReason.what(); }
  /* == Perform stack dump ================================================= */
  void SEHStackDump(StdOStringStream &osS, const HANDLE hProcess,
    const HANDLE hThread, const CONTEXT*const pcData) try
  { // Ignore if no context
    if(!pcData)
      throw StdRunTimeError{ "No stacktrace when context is a null pointer!" };
    // Initialise symbol reader and if succeeded?
    if(SymInitialize(hProcess, nullptr, TRUE))
    { // Auto clean-up symbol table
      const StdUniquePtr<void, function<decltype(SymCleanup)>>
        uProcess{ hProcess, SymCleanup };
      // Get context from exception record
      const CONTEXT &cData = *pcData;
      // Initialise stack frame structure
      STACKFRAME sfData{};
      sfData.AddrReturn = sfData.AddrStack = sfData.AddrBStore =
        { 0, 0, AddrModeFlat };
#if defined(X64)                       // Using 64-bit compiler?
# define IMAGE_FILE_MACHINE IMAGE_FILE_MACHINE_AMD64
# define ADDR_PC    cData.Rip
# define ADDR_FRAME cData.Rbp
#elif defined(X86)                     // Using 32-bit compiler?
# define IMAGE_FILE_MACHINE IMAGE_FILE_MACHINE_I386
# define ADDR_PC    cData.Eip
# define ADDR_FRAME cData.Ebp
#endif
      // Set register pointers
      sfData.AddrPC = { ADDR_PC, 0, AddrModeFlat };
      sfData.AddrFrame = { ADDR_FRAME, 0, AddrModeFlat };
      // Set sym options
      SymSetOptions((SymGetOptions() & ~SYMOPT_UNDNAME) | SYMOPT_LOAD_LINES);
      // Number of functions
      size_t stFunctions = 0;
      // Walk the stack
      while(StackWalk(IMAGE_FILE_MACHINE, hProcess, hThread, &sfData,
        StdToNonConstCast<PVOID>(&cData), nullptr, SymFunctionTableAccess,
        SymGetModuleBase, nullptr))
      { // Add function number
        osS << StdIOSDec << stFunctions++ << ": ";
        // Is a null address? Ignore it
        if(!sfData.AddrPC.Offset)
        { // Ignore it so goto next function
          osS << "<Null Address Pointer>" << cCommon->CommonCrLf();
          continue;
        } // Get module name and if succeeded?
        IMAGEHLP_MODULE ihmMod;
        ihmMod.SizeOfStruct = sizeof(ihmMod);
        if(SymGetModuleInfo(hProcess, sfData.AddrPC.Offset, &ihmMod))
          osS << ihmMod.ModuleName << '!';
        // Failed so write reason
        else osS << "<SGMI:" << SysError() << ">!";
        // A self note here that there were no wide functions until Windows 8.1
        // (or version 6.3 of the DbgHelp library) so we're stuck with ANSI
        // string functions.
        // Buffer for symbol name
        StdString strName; strName.resize(MAX_PATH);
        DWORD_PTR dwOffsetFromSym = 0;
        // Holds the memory for the symbol structure
        Memory aStruct{ sizeof(IMAGEHLP_SYMBOL)+255, true };
        PIMAGEHLP_SYMBOL ihsData = aStruct.MemPtr<IMAGEHLP_SYMBOL>();
        ihsData->SizeOfStruct = aStruct.MemSize<DWORD>();
        ihsData->MaxNameLength = 254;
        // Retreive function and if succeeded?
        if(SymGetSymFromAddr(hProcess, sfData.AddrPC.Offset, &dwOffsetFromSym,
          ihsData))
        { // Get function name
          strName.resize(UnDecorateSymbolName(ihsData->Name,
            StdToNonConstCast<PSTR>(strName.data()),
            static_cast<DWORD>(strName.length()), UNDNAME_COMPLETE));
          // Put in output string
          osS << strName << '@';
        } // Failed so record reason why that couldn't be obtained
        else switch(const DWORD dwCode = SysErrorCode<DWORD>())
        { // Invalid address
          case ERROR_INVALID_ADDRESS:
          // File not found? (This happens on Wine for some reason)
          case ERROR_FILE_NOT_FOUND:
            osS << "0x" << StdIOSHex << sfData.AddrPC.Offset << '@';
            break;
          // Any other code
          default: osS << "<SGSFA:" << SysError(dwCode) << ">@"; break;
        } // Get source file and line info and if succeded?
        IMAGEHLP_LINE ihlLine;
        ihlLine.SizeOfStruct = sizeof(ihlLine);
        if(SymGetLineFromAddr(hProcess, sfData.AddrPC.Offset,
          reinterpret_cast<PDWORD>(&dwOffsetFromSym), &ihlLine))
            osS << ihlLine.FileName << ':' << ihlLine.LineNumber <<
              cCommon->CommonCrLf();
        // Failed so record reason why that couldn't be obtained
        else switch(const DWORD dwCode = SysErrorCode<DWORD>())
        { // Invalid address
          case ERROR_INVALID_ADDRESS:
            osS << "<No Source Information>" << cCommon->CommonCrLf(); break;
          // Success? (This happens on Wine for some reason)
          case ERROR_SUCCESS:
            osS << "<Unavailable>" << cCommon->CommonCrLf(); break;
          // Any other code
          default:
            osS << "<SGLFA:" << SysError(dwCode) << ">!" <<
              cCommon->CommonCrLf(); break;
        }
      }
    } // Show error
    else osS << "SymInitialise failed: " << SysError() << '.' <<
      cCommon->CommonCrLf();
    // Done with these defines
#undef ADDR_PC
#undef ADDR_FRAME
#undef IMAGE_FILE_MACHINE
  } // Shouldn't happen but just incase
  catch(const StdException &eReason) { osS << eReason.what(); }
  /* == Dump summary to file =============================================== */
  DWORD SEHWrite(const HANDLE hH, const StdString &strS)
  { // Bytes out
    DWORD dwW;
    // Write data. MS says dwOut is optional, but the app crashes if you set
    // it to nullptr. So, something odd there.
    WriteFile(hH, strS.data(),
      static_cast<DWORD>(strS.length()), &dwW, nullptr);
    // Return bytes written
    return dwW;
  }
  /* == Create a subtitle for the output =================================== */
  void SEHSubTitle(StdOStringStream &osS, const StdString &strS)
  { // Build title and underline
    osS << strS << cCommon->CommonCrLf() << StdString(strS.length(), '=') <<
      cCommon->CommonCrLf();
  }
  /* == Dump file information ============================================== */
  void SEHDumpFileInfo(StdOStringStream &osS,
    const StdWideString &wstrFile) try
  { // Get system time
    SYSTEMTIME stData;
    GetLocalTime(&stData);
    // Write data
    osS << StdIOSDec << "Date/Time: "
        << StdIOSRight << StdIOSSetFill('0') << StdIOSSetWidth(2)
        << stData.wDay
        << '/' << StdIOSSetWidth(2) << stData.wMonth
        << '/' << StdIOSSetWidth(4) << stData.wYear
        << ' ' << StdIOSSetWidth(2) << stData.wHour
        << ':' << StdIOSSetWidth(2) << stData.wMinute
        << ':' << StdIOSSetWidth(2) << stData.wSecond
        << '.' << StdIOSSetWidth(3) << stData.wMilliseconds
        << '.' << cCommon->CommonCrLf()
        << "Filename: " << WS16toUTF(wstrFile) << '.' <<
          cCommon->CommonCrLf()
        << "Arguments: " << S16toUTF(GetCommandLine()) << '.' <<
          cCommon->CommonCrLf2();
  } // Shouldn't happen but just incase
  catch(const StdException &eReason) { osS << eReason.what(); }
  /* == Dump string to file ================================================ */
  void SEHDumpLog(StdOStringStream &osS, const CONTEXT*const cData,
    const StdString &strDialog) try
  { // Get filename
#define LIFY2(x) L ## x
#define LIFY(x) LIFY2(x)
    const StdWideString wstrFile{
      SEHGetExecutableFileNameWithoutExtension() +
        L"." LIFY(CRASH_EXTENSION) };
#undef LIFY
#undef LIFY2
    // Open dump file and return if failed
    const HANDLE hFile = CreateFile(wstrFile.data(), GENERIC_WRITE, 0, 0,
      CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
    if(hFile == INVALID_HANDLE_VALUE) return;
    // Capture exceptions so we can close the file
    try
    { // Write basic information (executable, time, arguments)
      SEHSubTitle(osS, "Basic");
        SEHDumpFileInfo(osS, wstrFile);
      // Write the crash summary
      SEHSubTitle(osS, "Summary");
        osS << strDialog << cCommon->CommonCrLf();
      // Write the crash registers
      SEHSubTitle(osS, "Registers");
        SEHDumpRegisters(osS, cData);
        osS << cCommon->CommonCrLf();
      // Write memory information
      SEHSubTitle(osS, "Memory");
        SEHDumpMemoryStatus(osS);
        osS << cCommon->CommonCrLf();
      // Write environment variables
      SEHSubTitle(osS, "Environment");
        SEHDumpEnvironment(osS);
        osS << cCommon->CommonCrLf();
      // Write modules
      SEHSubTitle(osS, "Modules");
        SEHModuleDump(osS);
        osS << cCommon->CommonCrLf();
      // Write processes
      SEHSubTitle(osS, "Processes");
        SEHProcessDump(osS);
        osS << cCommon->CommonCrLf();
      // Write engine log
      SEHSubTitle(osS, "Log");
        cLog->LogGetBufferLines(osS);
        osS << cCommon->CommonCrLf();
      // Write the final end-of-file label
      SEHSubTitle(osS, "End-of-File");
      // Commit the log to crash file
      SEHWrite(hFile, osS.str());
      // Close handle
      CloseHandle(hFile);
    } // If exception occured? Just close the file
    catch(const StdException &) { CloseHandle(hFile); }
  } // Shouldn't happen but just incase
  catch(const StdException &) {}
  /* == Build summary ====================================================== */
  const StdString SEHGetSummary(const EXCEPTION_POINTERS &epData) try
  { // String builder
    StdOStringStream osS;
    // Get exception record
    const EXCEPTION_RECORD &erData = *epData.ExceptionRecord;
    // Build message for dialog box
    osS << "The instruction at address 0x" << erData.ExceptionAddress
        << " of thread " << GetCurrentThreadId()
        << " in process " << GetCurrentProcessId()
        << " caused exception 0x" << StdIOSHex << erData.ExceptionCode
        << " (" << eclStrings.Get(erData.ExceptionCode)
        << ") with flags 0x" << erData.ExceptionFlags << StdIOSDec << '.';
    SEHDumpExceptionMemoryAddresses(osS, erData);
    osS << cCommon->CommonCrLf2();
    // Check for illegal instruction? Add extra information
    if(erData.ExceptionCode == EXCEPTION_ILLEGAL_INSTRUCTION)
      osS << "This illegal instruction exception usually indicates that this "
             BUILD_TARGET " compiled binary is NOT compatible with your "
             "central processing unit."
          << cCommon->CommonCrLf2();
    // Add stack dump[
    SEHStackDump(osS, GetCurrentProcess(), GetCurrentThread(),
      epData.ContextRecord);
    // Return string
    return osS.str();
  } // Shouldn't happen but just incase
  catch(const StdException &eReason) { return eReason.what(); }
  /* == Method for exception handler ======================================= */
  static LONG WINAPI HandleExceptionStatic(LPEXCEPTION_POINTERS);
  LONG HandleException(const EXCEPTION_POINTERS &epData) try
  { // Check exception record
    if(!epData.ExceptionRecord)
      throw StdRunTimeError{ "The engine crash handler was called but with a "
                             "null address exception record!" };
    // Prepare summary
    const StdString strDialog{ SEHGetSummary(epData) };
    // Make a shared string stream
    StdOStringStream osS;
    // Write the log file
    SEHDumpLog(osS, epData.ContextRecord, strDialog);
    // No need to show anything if we're in a debugger
    if(IsDebuggerPresent()) return EXCEPTION_CONTINUE_SEARCH;
    // Show message box
    MessageBox(hwndWindow, UTFtoS16(strDialog).data(),
      L"Unhandled exception", MB_ICONSTOP);
    // We handled the exception
    return EXCEPTION_EXECUTE_HANDLER;
  } // This shouldn't happen but just incase
  catch(const StdException &eReason)
  { // Show message box
    MessageBox(hwndWindow, UTFtoS16(eReason.what()).data(),
      L"exception in unhandled exception", MB_ICONSTOP);
    // We handled the exception
    return EXCEPTION_EXECUTE_HANDLER;
  }
  /* == Signal event handler =============================================== */
  /* Make sure to follow the rules of using the signal callback:-            */
  /* + Do not issue low-level or STDIO.H I/O routines (for example, printf   */
  /*   or fread).                                                            */
  /* + Do not call heap routines or any routine that uses the heap routines  */
  /*   (for example, malloc, _strdup, or _putenv). See malloc for more       */
  /*   information.                                                          */
  /* + Do not use any function that generates a system call (for example,    */
  /*   _getcwd or time).                                                     */
  /* + Do not use longjmp unless the interrupt is caused by a floating-point */
  /*   exception (that is, sig is SIGFPE). In this case, first reinitialize  */
  /*   the floating-point package by using a call to _fpreset.               */
  /* + Do not use any overlay routines.                                      */
  /*   docs.microsoft.com/en-us/previous-versions/xdkz3x12(v%3Dvs.140)       */
  /* ----------------------------------------------------------------------- */
  static void SignalEvent(int iSignal)
  { // Id code of exception (make sure to describe them in exception handler)
    DWORD dwId;
    // Compare signal
    switch(iSignal)
    { // Abort?
      case SIGABRT: dwId = EXCEPTION_ABORT; break;
      // Segmentation fault?
      case SIGSEGV: dwId = EXCEPTION_ISA; break;
      // Floating point error?
      case SIGFPE: dwId = EXCEPTION_FPOINT; break;
      // Something else? Breakpoint
      default: return DebugBreak();
    } // Raise exception
    RaiseException(dwId, 0, 0, nullptr);
  }
  /* --------------------------------------------------------------- */ public:
  HWND GetWindowHandle() const { return hwndWindow; }
  bool IsWindowHandleSet() const { return GetWindowHandle() != nullptr; }
  bool IsNotWindowHandleSet() const { return !IsWindowHandleSet(); }
  void SetWindowDestroyed() { SetWindowHandle(nullptr); }
  /* ------------------------------------------------------------ */ protected:
  void SetWindowHandle(HWND hwndNew) { hwndWindow = hwndNew; }
  /* -- Constructor (install exception filter) ----------------------------- */
  SysBase() :
    /* -- Initialisers ----------------------------------------------------- */
    hwndWindow(nullptr),
    // Set no dialogues for system errors and save code
    uiOldErrorMode(SetErrorMode(SEM_NOOPENFILEERRORBOX)),
    // Install signal handlers and save old ones
    fcbAbortCallback(signal(SIGABRT, SignalEvent)),
    fcbIllegalStorageAccess(signal(SIGSEGV, SignalEvent)),
    fcbFloatingPointException(signal(SIGFPE, SignalEvent)),
    // Exception strings
    eclStrings{{
      IDMAPSTR(EXCEPTION_ACCESS_VIOLATION),
      IDMAPSTR(EXCEPTION_ARRAY_BOUNDS_EXCEEDED),
      IDMAPSTR(EXCEPTION_BREAKPOINT),
      IDMAPSTR(EXCEPTION_DATATYPE_MISALIGNMENT),
      IDMAPSTR(EXCEPTION_FLT_DENORMAL_OPERAND),
      IDMAPSTR(EXCEPTION_FLT_DIVIDE_BY_ZERO),
      IDMAPSTR(EXCEPTION_FLT_INEXACT_RESULT),
      IDMAPSTR(EXCEPTION_FLT_INVALID_OPERATION),
      IDMAPSTR(EXCEPTION_FLT_OVERFLOW),
      IDMAPSTR(EXCEPTION_FLT_STACK_CHECK),
      IDMAPSTR(EXCEPTION_FLT_UNDERFLOW),
      IDMAPSTR(EXCEPTION_ILLEGAL_INSTRUCTION),
      IDMAPSTR(EXCEPTION_IN_PAGE_ERROR),
      IDMAPSTR(EXCEPTION_INT_DIVIDE_BY_ZERO),
      IDMAPSTR(EXCEPTION_INT_OVERFLOW),
      IDMAPSTR(EXCEPTION_INVALID_DISPOSITION),
      IDMAPSTR(EXCEPTION_NONCONTINUABLE_EXCEPTION),
      IDMAPSTR(EXCEPTION_PRIV_INSTRUCTION),
      IDMAPSTR(EXCEPTION_SINGLE_STEP),
      IDMAPSTR(EXCEPTION_STACK_OVERFLOW),
      IDMAPSTR(EXCEPTION_GUARD_PAGE),
      IDMAPSTR(EXCEPTION_INVALID_HANDLE),
      IDMAPSTR(EXCEPTION_ABORT),
      IDMAPSTR(EXCEPTION_ISA),
      IDMAPSTR(EXCEPTION_FPOINT)
    }}
  /* -- Install unhandled exception filter --------------------------------- */
  { SetUnhandledExceptionFilter(HandleExceptionStatic); }
  /* -- Destructor --------------------------------------------------------- */
  DTORHELPER(~SysBase,
    // Restore original signal handlers
    if(fcbFloatingPointException) signal(SIGFPE, fcbFloatingPointException);
    if(fcbIllegalStorageAccess) signal(SIGSEGV, fcbIllegalStorageAccess);
    if(fcbAbortCallback) signal(SIGABRT, fcbAbortCallback);
    // exception filter no longer valid
    SetUnhandledExceptionFilter(nullptr);
    // Restore old error mode
    SetErrorMode(uiOldErrorMode);
  )
};/* ----------------------------------------------------------------------- */
#define ENGINE_SYSBASE_CALLBACKS() \
  LONG WINAPI SysBase::HandleExceptionStatic(LPEXCEPTION_POINTERS \
    epData) { return cSystem->HandleException(*epData); }
/* == EoF =========================================================== EoF == */
