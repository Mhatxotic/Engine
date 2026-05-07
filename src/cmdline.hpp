/* == CMDLINE.HPP ========================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## This static class stores command line arguments and assists in      ## **
** ## restarting the engine when needed.                                  ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace ICmdLine {                   // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace ICommon::P;            using namespace IDir::P;
using namespace IError::P;             using namespace IStd::P;
using namespace IStdLib::P;            using namespace IString::P;
using namespace ISysUtil::P;           using namespace IToken::P;
using namespace IUtf::P;               using namespace IUtil::P;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* ------------------------------------------------------------------------- */
enum ExitOperation : unsigned          // Things to do at exit
{ /* ----------------------------------------------------------------------- */
  EO_QUIT,                             // Quit normally
  EO_TERM_REBOOT_NOARG,                // Restart without parameters
  EO_UI_REBOOT_NOARG,                  // Same as above but in UI mode
  EO_TERM_REBOOT,                      // Restart with parameters
  EO_UI_REBOOT,                        // Same as above but in UI mode
};/* ----------------------------------------------------------------------- */
/* -- Command line helper class (should be the first global to inti) ------- */
class CmdLine;                         // Class prototype
static CmdLine *cCmdLine = nullptr;    // Address of global class
class CmdLine                          // Members initially public
{ /* -- Private typedefs --------------------------------------------------- */
  using ArgList = StdSpan<ArgType*>;   // So we have the correct sizes
  /* -- Command-line and environment variables ----------------------------- */
  ExitOperation    eoExit;             // Actions to perform at exit
  ArgList          alArgs,             // Arguments list (safe size)
                   alEnv;              // Environment list (size size)
  const StrVector  svArg;              // Arguments list
  const StrStrMap  ssmEnv;             // Formatted environment variables
  const StdString  strCWD;             // Current startup working directory
  StdString        strHD;              // Persistant directory
  /* -- Check command line arguments validity ------------------------------ */
  static ArgType** CmdLineCheckArgs(ArgType** atNArgs)
  { // Check that args are valid
    if(!atNArgs) XC("Arguments array is corrupted!");
    if(!*atNArgs) XC("Arguments array executable string is corrupted!");
    if(!**atNArgs) XC("Arguments array executable string is empty!");
    // Variable validated
    return atNArgs;
  }
  /* -- Measure environment size ------------------------------------------- */
  static size_t CmdLineEnvSize(ArgType*const*const atNEnv)
  { // Check that args are valid
    if(!atNEnv) XC("Environment array is corrupted!");
    if(!*atNEnv) XC("Environment array first string is corrupted!");
    if(!**atNEnv) XC("Environment array first string is empty!");
    auto atNEnd = std::ranges::find_if(atNEnv, std::unreachable_sentinel,
      [](ArgType*const atPtr) { return atPtr == nullptr; });
    return static_cast<size_t>(StdDistance(atNEnv, atNEnd));
  }
  /* -- Parse command line arguments --------------------------------------- */
  StrVector CmdLineParseArgArray()
  { // Arguments list to return
    const size_t stArgCM1 = CmdLineGetTotalCArgs() - 1;
    StdReserved<StrVector> svRet{ stArgCM1 };
    // For each argument format the argument and add it to list
    StdForEach(seq, StdNext(alArgs.begin()), alArgs.end(),
      [&svRet](const ArgType*const atStr)
        { svRet.emplace_back(S16toUTF(atStr)); });
    // One final sanity check
    if(svRet.size() != stArgCM1)
      XC("Arguments array actual count mismatch!",
        "Expect", stArgCM1, "Actual", svRet.size());
    // Return arguments list
    return svRet;
  }
  /* -- Parse environment variables ---------------------------------------- */
  StrStrMap CmdLineParseEnvArray()
  { // Arguments list to return
    StrStrMap ssmRet;
    // Process environment variables
    StdForEach(seq, alEnv.begin(), alEnv.end(),
      [&ssmRet](const ArgType*const atStr)
#if defined(LINUX)
    { if(TokenStr tsParam{ atStr, cCommon->CommonEqualsV(), 2 })
#else
    { if(TokenStr tsParam{ S16toUTF(atStr), cCommon->CommonEqualsV(), 2 })
#endif
        ssmRet.insert({ StdMove(tsParam.front()), tsParam.size() >= 2 ?
          StdMove(tsParam.back()) : cCommon->CommonBlank() }); });
    // Unset unallowed variables
    SysUnSetEnv(
      // Operating system check
# if !defined(ALPHA)                   // Not using debug version?
#  if defined(MACOS)                   // Targeting MacOS?
      "DYLD_INSERT_LIBRARIES",         // Disable shared object overrides
      "MallocCheckHeapAbort",          // Don't throw abort() on heap check
      "MallocCheckHeapEach",           // Don't check heap every 'n' mallocs
      "MallocCheckHeapStart",          // Don't check heap at 'n' mallocs
      "MallocGuardEdges",              // Don't guard edges
      "MallocHelp",                    // Help messages
      "MallocScribble",                // Don't scribble memory
      "NSDeallocateZombies",           // Deallocate zombies
      "NSZombieEnabled",               // Enable dealloc in foundation
#  elif defined(LINUX)                 // Using Linux?
      "LD_PRELOAD",                    // Disable shared object overrides
#  endif                               // Using alpha version
# endif                                // Using MacOS or Linux
      "SSL_CERT_FILE",                 // Ignore OpenSSL CA store files
      "SSL_CERT_DIR"                   // Ignore OpenSSL CA store directories
    ); // Return environment variables list
    return ssmRet;
  }
  /* -- Set persistant directory ----------------------------------- */ public:
  void CmdLineSetHome(const StdString &strDir) { strHD = strDir; }
  /* -- Get persistant directory ------------------------------------------- */
  bool CmdLineIsNoHome() const { return strHD.empty(); }
  bool CmdLineIsHome() const { return !CmdLineIsNoHome(); }
  /* -- Return and move string into output string -------------------------- */
  const StdString &CmdLineGetHome() const { return strHD; }
  StdString CmdLineGetHome(const StdStringView &strvSuf) const
    { return StrAppend(CmdLineGetHome(), strvSuf); }
  /* -- Get environment variable ------------------------------------------- */
  template<class StrType>
    const StdString &CmdLineGetEnv(const StrType &strEnv) const
  { // Find item and return it else return the default item
    const StrStrMapConstIt smciEnv{ ssmEnv.find(strEnv) };
    return smciEnv != ssmEnv.cend() ? smciEnv->second : cCommon->CommonBlank();
  }
  /* -- Get environment variable and check that it is a valid pathname ----- */
  StdString CmdLineMakeEnvPath(const StdString &strEnv,
    const StdString &strSuffix)
  { // Get home environment variable and throw error if not found
    const StrStrMapConstIt eiEnv{ ssmEnv.find(strEnv) };
    if(eiEnv == ssmEnv.cend())
      XC("The specified environment variable is required and missing!",
        "Variable", strEnv, "Suffix", strSuffix);
    // Check validity of the specified environmen variable
    const StdString &strEnvVal = eiEnv->second;
    const ValidResult vRes = DirValidName(strEnvVal, VT_TRUSTED);
    if(vRes == VR_OK) return StrAppend(strEnvVal, strSuffix);
    // Show error otherwise
    XC("The specified environment variable directory is invalid!",
      "Variable", strEnv, "Suffix", strSuffix, "Directory", strEnvVal,
      "Result",   vRes,   "Reason", cDirBase->DirBaseVNRtoStr(vRes));
  }
  /* -- Get parameter total ------------------------------------------------ */
  size_t CmdLineGetTotalCArgs() const
    { return alArgs.size(); }
  ArgType*const *CmdLineGetCArgs() const
    { return const_cast<ArgType*const*>(alArgs.data()); }
  ArgType*const *CmdLineGetCEnv() const
    { return const_cast<ArgType*const*>(alEnv.data()); }
  const StrStrMap &CmdLineGetEnvList() const { return ssmEnv; }
  const StrVector &CmdLineGetArgList() const { return svArg; }
  /* -- Set restart flag (0 = no restart, 1 = no params, 2 = params) ------- */
  void CmdLineSetRestart(const ExitOperation ecCmd) { eoExit = ecCmd; }
  /* -- Get startup current directory -------------------------------------- */
  const StdString &CmdLineGetStartupCWD() const { return strCWD; }
  /* -- Return to startup directory ---------------------------------------- */
  void CmdLineSetStartupCWD()
  { // Try to set the startup working direcotry and throw if failed.
    if(!DirSetCWD(CmdLineGetStartupCWD()))
      XCL("Failed to set startup working directory!",
        "Directory", CmdLineGetStartupCWD());
  }
  /* -- Assign arguments ---------------------------------------- */ protected:
  CmdLine(const int iArgs, ArgType**const atNArgs, ArgType**const atNEnv) :
    /* -- Initialisers ----------------------------------------------------- */
    eoExit(EO_QUIT),                   // Initialise exit code
    alArgs{ CmdLineCheckArgs(atNArgs), // Initialise cmdline args ptr
      static_cast<size_t>(iArgs) },    // ...checking that it's valid first
    alEnv{ atNEnv,                     // Initialise cmdline environment ptr
      CmdLineEnvSize(atNEnv) },        // ...we have to find the end manually!
    svArg{ StdMove(                    // Initialise command line arguments
      CmdLineParseArgArray()) },       // ...so we can keep them const
    ssmEnv{ StdMove(                   // Initialise environment variables
      CmdLineParseEnvArray()) },       // ...so we can keep them const
    strCWD{ StdMove(DirGetCWD()) }     // Initialise current working directory
    /* -- Set global pointer to static class ------------------------------- */
    { cCmdLine = this; }
  /* -- Destructor --------------------------------------------------------- */
  ~CmdLine() noexcept(false) try
  { // Clear global class
    cCmdLine = nullptr;
    // Done if arguments were never initialised
    if(!CmdLineGetTotalCArgs()) return;
    // Restore startup working directory
    CmdLineSetStartupCWD();
    // Edited number of arguments we're sending (for show only)
    size_t stArgs = CmdLineGetTotalCArgs();
    // Do we have a restart mode set?
    switch(eoExit)
    { // Just return if no restart required
      case EO_QUIT: return;
      // Reboot with no arguments?
      case EO_TERM_REBOOT_NOARG:
        // Remove first parameter and fall through to reboot
        alArgs[1] = nullptr; stArgs = 1; [[fallthrough]];
      // Restart while keeping parameters?
      case EO_TERM_REBOOT: CmdLineSetRestart(EO_QUIT);
        // Do the restart and replace the current process with the new one
        switch(const int iCode =
          StdExecVE(CmdLineGetCArgs(), CmdLineGetCEnv()))
        { // Success? Shouldn't get here!
          case 0: break;
          // Error occured? Don't attempt execution again and show error
          default: XCL("Failed to restart process!",
            "Process", alArgs.front(), "Code", iCode, "Parameters", stArgs);
        } // Done
        break;
      // Remove first parameter and fallthrough to next label
      case EO_UI_REBOOT_NOARG: alArgs[1] = nullptr;
                               stArgs = 1;
                               [[fallthrough]];
      // Restart while keeping parameters in ui mode?
      case EO_UI_REBOOT: CmdLineSetRestart(EO_QUIT);
        // Do the restart using spawn as MacOS is weird with ui apps otherwise.
        switch(const int iCode =
          StdSpawnVE(CmdLineGetCArgs(), CmdLineGetCEnv()))
        { // Success? Proceed to quit
          case 0: break;
          // Error occurred? Don't attempt execution again and show error
          default: XCL("Failed to spawn new process!",
            "Process", alArgs.front(), "Code", iCode, "Parameters", stArgs);
        } // Done
        break;
      // Anything else? (Impossible but needed to prevent compiler warning)
      default: XC("Internal error: Invalid exit command!", "Command", eoExit);
    }
  } // Exception occured?
  catch(const StdException &eReason) \
  { // Show message box as the log is not available.
    SysMessage("CmdLine Shutdown Exception!", eReason.what(), MB_ICONSTOP);
  }
};/* ----------------------------------------------------------------------- */
};                                     // End of public module namespace
/* ------------------------------------------------------------------------- */
};                                     // End of private module namespace
/* == EoF =========================================================== EoF == */
