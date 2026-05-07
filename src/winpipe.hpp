/* == WINPIPE.HPP ========================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## This is a Windows specific module that will assist in executing     ## **
** ## a new process and capturing it's output into a buffer.              ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace ISysPipe {                   // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IArgs::P;              using namespace ICmdLine::P;
using namespace IDir::P;               using namespace IError::P;
using namespace ILog::P;               using namespace IMemory::P;
using namespace IStd::P;               using namespace IStdLib::P;
using namespace ISysUtil::P;           using namespace Lib::OS;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* ------------------------------------------------------------------------- */
class SysPipe :                        // Members initially private
  /* -- Base classes ------------------------------------------------------- */
  public SysPipeBase                   // System pipe base class
{ /* ----------------------------------------------------------------------- */
  HANDLE           hStdinRead,         // Input read handle
                   hStdinWrite,        // Input write handle
                   hStdoutRead,        // Output read handle
                   hStdoutWrite,       // Output write handle
                   hProcess,           // Handle to process
                   hThread;            // Handle to process main thread
  DWORD            dwPid;              // Process id
  /* -- DeInit pipe -------------------------------------------------------- */
  void DeInit()
  { // If we have the process handle
    if(hProcess != INVALID_HANDLE_VALUE)
    { // Close thread handle as we are done with it
      if(hThread != INVALID_HANDLE_VALUE) CloseHandle(hThread);
      // Get exit code of the process
      DWORD dwExitCode;
      if(GetExitCodeProcess(hProcess, &dwExitCode))
      { // If it is still active? Try to kill it and write error if failed
        if(dwExitCode == STILL_ACTIVE &&
          !TerminateProcess(hProcess, static_cast<UINT>(-1)))
            cLog->LogWarningExSafe(
              "System failed to terminate process for '$': $!",
                NameGet(), SysError());
        // Set exit code
        SysPipeBaseSetStatus(static_cast<int64_t>(dwExitCode));
      } // Failed to get exit code
      else cLog->LogWarningExSafe("System failed to get exit code for '$': $!",
        NameGet(), SysError());
      // Close the process handle
      if(!CloseHandle(hProcess))
        cLog->LogWarningExSafe(
          "System failed to close process handle for '$': $!",
            NameGet(), SysError());
    } // Close pipe handles if open
    if(hStdinRead != INVALID_HANDLE_VALUE && !CloseHandle(hStdinRead))
      cLog->LogWarningExSafe("System failed to close process handle for '$' "
        "stdin receive pipe: $!", NameGet(), SysError());
    if(hStdinWrite != INVALID_HANDLE_VALUE && !CloseHandle(hStdinWrite))
      cLog->LogWarningExSafe("System failed to close process handle for '$' "
        "stdin send pipe: $!", NameGet(), SysError());
    if(hStdoutRead != INVALID_HANDLE_VALUE && !CloseHandle(hStdoutRead))
      cLog->LogWarningExSafe("System failed to close process handle for '$' "
        "stdout read pipe: $!", NameGet(), SysError());
    if(hStdoutWrite != INVALID_HANDLE_VALUE && !CloseHandle(hStdoutWrite))
      cLog->LogWarningExSafe("System failed to close process handle for '$' "
        "stdout send pipe: $!", NameGet(), SysError());
  }
  /* -- Initialise arguments list ------------------------------------------ */
  void InitArgs(const StdString &strCmdLine, const Args &aList,
    const ValidType vtId)
  { // Get the program filename and check it
    const StdString &strApp = aList.front();
    switch(const ValidResult vrResult = DirValidName(strApp, vtId))
    { // Good result? Skip to next task
      case VR_OK: break;
      // Anything else and we need to throw an exception
      default: XC("Executable name is invalid!",
                 "Program", strApp, "Code", vrResult,
                 "Reason",  cDirBase->DirBaseVNRtoStr(vrResult),
                 "CmdLine", strCmdLine);
    } // De-init existing process
    Finish();
    // Show command and arguments
    cLog->LogDebugExSafe("System opening pipe to '$' with $ args '$'.",
      strApp, aList.size(), strCmdLine);
    // Security attributes
    SECURITY_ATTRIBUTES saAttr{ sizeof(SECURITY_ATTRIBUTES), nullptr, TRUE };
    // Create the pipe
    if(!CreatePipe(&hStdoutRead, &hStdoutWrite, &saAttr, 0))
      XCS("Create pipe for process stdout failed!", "Executable", strApp);
    // Ensure the read handle to the pipe for STDOUT is not inherited.
    if(!SetHandleInformation(hStdoutRead, HANDLE_FLAG_INHERIT, 0))
      XCS("Failed to remove inheritance from stdout!", "Executable", strApp);
    // Create pipe for the processes stdin
    if(!CreatePipe(&hStdinRead, &hStdinWrite, &saAttr, 0))
      XCS("Create pipe for process stdin failed!", "Executable", strApp);
    // Ensure the write handle to the pipe for STDIN is not inherited.
    if(!SetHandleInformation(hStdinWrite, HANDLE_FLAG_INHERIT, 0))
      XCS("Failed to remove inheritance from in!", "Executable", strApp);
    // Process information for execution
    PROCESS_INFORMATION piProcInfo;
    // Set up members of the STARTUPINFO structure.
    STARTUPINFO siStartInfo{           // Presumed STARTUPINFOW
      sizeof(STARTUPINFO),             // DWORD  cb (Mandatory)
      nullptr,                         // LPWSTR lpReserved (Not used)
      nullptr,                         // LPWSTR lpDesktop (Not used)
      nullptr,                         // LPWSTR lpTitle (Not used)
      0,                               // DWORD dwX (Not used)
      0,                               // DWORD dwY (Not used)
      0,                               // DWORD dwXSize (Not used)
      0,                               // DWORD dwYSize (Not used)
      0,                               // DWORD dwXCountChars (Not used)
      0,                               // DWORD dwYCountChars (Not used)
      0,                               // DWORD dwFillAttribute (Not used)
      STARTF_USESTDHANDLES |           // DWORD dwFlags (Using own handles)
        STARTF_USESHOWWINDOW |         // (We're setting initial window state)
        STARTF_PREVENTPINNING |        // (Cannot be pinned to taskbar)
        0x8000,                        // (STARTF_UNTRUSTEDSOURCE - Untrusted)
      SW_SHOWNA,                       // WORD wShowWindow (Hide windows)
      0,                               // WORD cbReserved2 (Not used)
      nullptr,                         // LPBYTE lpReserved2 (Not used)
      hStdinRead,                      // HANDLE hStdInput (Using ours)
      hStdoutWrite,                    // HANDLE hStdOutput (Using ours)
      hStdoutWrite                     // HANDLE hStdError (Using ours/stdout)
    };
    // Create the child process and if succeeded?
    if(CreateProcess(StdToNonConstCast<LPCWSTR>(UTFtoS16(strApp).data()),
      StdToNonConstCast<LPWSTR>(UTFtoS16(strCmdLine).data()), nullptr,
      nullptr, TRUE, CREATE_SUSPENDED|CREATE_NO_WINDOW, nullptr, nullptr,
      &siStartInfo, &piProcInfo)) try
    { // Store name of executable
      NameSet(StdMove(strApp));
      // Close handles to the stdin and stdout pipes no longer needed by the
      // child process. If they are not explicitly closed, there is no way to
      // recognize that the child process has ended.
      if(!CloseHandle(hStdoutWrite))
        XCS("Failed to close stdout write pipe!", "Executable", NameGet());
      hStdoutWrite = INVALID_HANDLE_VALUE;
      if(!CloseHandle(hStdinRead))
        XCS("Failed to close stdin write pipe!", "Executable", NameGet());
      hStdinRead = INVALID_HANDLE_VALUE;
      // Set process handle and check it. There will be no reason given if
      // this is true. This may happen when Wine tries to execute a symbolic
      // link's target that is invalid.
      if(!piProcInfo.hProcess)
        XC("No process handle was returned!", "Executable", NameGet());
      hProcess = piProcInfo.hProcess;
      // Set thread handle and check it
      if(!piProcInfo.hThread)
        XC("No thread handle was returned!", "Executable", NameGet());
      hThread = piProcInfo.hThread;
      // Set pid and check it
      if(!piProcInfo.dwProcessId)
        XC("No pid number was returned!", "Executable", NameGet());
      dwPid = piProcInfo.dwProcessId;
      // Report pid
      cLog->LogInfoExSafe("System spawned process '$' to pid $.",
        strApp, dwPid);
      // Resume main thread
      if(!ResumeThread(hThread))
        XCS("Failed to resume main thread!", "Executable", NameGet());
    } // Error occured so terminate process and close handles
    catch(const StdException &)
    { // Forcibly close handles
      Finish();
      // Throw exception
      throw;
    } // Create process failed
    else
    { // Store name of executable
      NameSet(StdMove(strApp));
      // Throw error
      XCS("Failed to execute process!", "Executable", NameGet());
    }
  }
  /* -- Set socket timeout ----------------------------------------- */ public:
  void Finish()
  { // De-init all the handles
    DeInit();
    // Clear variables
    hProcess = hThread = hStdinRead = hStdinWrite =
      hStdoutRead = hStdoutWrite = INVALID_HANDLE_VALUE;
  }
  /* -- Finished sending --------------------------------------------------- */
  void SendFinish()
  { // Return if handle not available
    if(hStdinWrite == INVALID_HANDLE_VALUE) return;
    // Close the pipe handle so the child process stops reading.
    if(!CloseHandle(hStdinWrite))
      XCS("Failed to close process stdin write handle!",
          "Executable", NameGet());
    // The handle is no longer valid
    hStdinWrite = INVALID_HANDLE_VALUE;
  }
  /* -- Read data ---------------------------------------------------------- */
  size_t Read(void*const vpDest, size_t stToRead)
  { // Send finish if stdin still open
    if(hStdinWrite != INVALID_HANDLE_VALUE) SendFinish();
    // Return if read not available
    if(hStdoutRead == INVALID_HANDLE_VALUE) return 0;
    // Read from the process return how many bytes we read
    DWORD dwRead;
    if(ReadFile(hStdoutRead, reinterpret_cast<LPSTR>(vpDest),
         static_cast<DWORD>(stToRead), &dwRead, nullptr))
      return static_cast<size_t>(dwRead);
    // If the pipe was ended then theres no need for an error
    if(SysIsNotErrorCode(ERROR_BROKEN_PIPE))
      XCS("Error reading from pipe!", "Executable", NameGet());
    // Nothing read
    return 0;
  }
  /* -- Read data into memory block ---------------------------------------- */
  Memory ReadBlock(const size_t stBuffer)
  { // Buffer for process output
    Memory mBuffer{ stBuffer };
    // Read some data and resize the block to the amount read
    mBuffer.MemResize(Read(mBuffer.MemPtr<char>(), mBuffer.MemSize()));
    // Return memory block
    return mBuffer;
  }
  /* -- Send to stdin ------------------------------------------------------ */
  size_t Send(void*const vpDest, size_t stToWrite)
  { // Write data to pipe if we have any
    if(hStdinWrite == INVALID_HANDLE_VALUE) return 0;
    // Write to the pipe and if failed throw exception
    DWORD dwWritten = 0;
    if(WriteFile(hStdinWrite, vpDest,
         static_cast<DWORD>(stToWrite), &dwWritten, nullptr))
      return static_cast<size_t>(dwWritten);
    // If the pipe was ended then theres no need for an error
    if(SysIsNotErrorCode(ERROR_BROKEN_PIPE))
      XCS("Error writing to process pipe!",
          "Executable", NameGet(), "Expect", stToWrite, "Written", dwWritten);
    // Nothing written
    return 0;
  }
  /* -- Constructor with init ---------------------------------------------- */
  void Init(const StdString &strCmdLine, const ValidType vtId)
    { if(const Args aList{ strCmdLine }) InitArgs(strCmdLine, aList, vtId); }
  /* -- Constructor with init with default safety mode --------------------- */
  void Init(const StdString &strCmdLine)
    { Init(strCmdLine, cDirBase->DirBaseGetSafetyMode()); }
  /* -- Return pid --------------------------------------------------------- */
  unsigned GetPid() { return static_cast<unsigned>(dwPid); }
  /* -- Constructor with init ---------------------------------------------- */
  explicit SysPipe(const StdString &strF) :
    /* -- Initialisers ----------------------------------------------------- */
    SysPipe()                          // Start uninitialised
    /* --------------------------------------------------------------------- */
    { Init(strF); }                    // Initialise with filename
  /* -- Constructor with init ---------------------------------------------- */
  SysPipe() :
    /* -- Initialisers ----------------------------------------------------- */
    hStdinRead(INVALID_HANDLE_VALUE),  // Stdin read handle uninitialised
    hStdinWrite(INVALID_HANDLE_VALUE), // Stdin write handle uninitialised
    hStdoutRead(INVALID_HANDLE_VALUE), // Stdout read handle uninitialised
    hStdoutWrite(INVALID_HANDLE_VALUE),// Stdout write handle uninitialised
    hProcess(INVALID_HANDLE_VALUE),    // Process handle uninitialised
    hThread(INVALID_HANDLE_VALUE),     // Main thread handle uninitialised
    dwPid(0)                           // No pid
    /* --------------------------------------------------------------------- */
    {}
  /* -- Destructor --------------------------------------------------------- */
  DTORHELPER(~SysPipe, DeInit())
};/* ----------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
