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
using namespace IString::P;            using namespace ISysUtil::P;
using namespace IToken::P;             using namespace IUtf::P;
using namespace IUtil::P;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* ------------------------------------------------------------------------- */
enum ExitOperation : unsigned int      // Things to do at exit
{ /* ----------------------------------------------------------------------- */
  EO_QUIT,                             // Quit normally
  EO_TERM_REBOOT_NOARG,                // Restart without parameters
  EO_UI_REBOOT_NOARG,                  // Same as above but in UI mode
  EO_TERM_REBOOT,                      // Restart with parameters
  EO_UI_REBOOT,                        // Same as above but in UI mode
};/* ----------------------------------------------------------------------- */
/* -- Command line helper class (should be the first global to inti) ------- */
struct CmdLine;                        // Class prototype
static CmdLine *cCmdLine = nullptr;    // Address of global class
struct CmdLine                         // Members initially public
{ /* -- Command-line and environment variables ---------------------*/ private:
  ExitOperation    eoExit;             // Actions to perform at exit
  int              iArgC;              // Arguments count
  ArgType        **atArgs,             // Arguments list
                 **atEnv;              // Environment list
  const StrVector  svArg;              // Arguments list
  const StrStrMap  ssmEnv;             // Formatted environment variables
  const string     strCWD;             // Current startup working directory
  string           strHD;              // Persistant directory
  /* -- Set persistant directory ----------------------------------- */ public:
  void CmdLineSetHome(const string &strDir) { strHD = strDir; }
  /* -- Get persistant directory ------------------------------------------- */
  bool CmdLineIsNoHome() const { return strHD.empty(); }
  bool CmdLineIsHome() const { return !CmdLineIsNoHome(); }
  /* -- Return and move string into output string -------------------------- */
  const string &CmdLineGetHome() const { return strHD; }
  const string CmdLineGetHome(const string &strSuf) const
    { return StrAppend(CmdLineGetHome(), strSuf); }
  /* -- Get environment variable ------------------------------------------- */
  const string &CmdLineGetEnv(const string &strEnv,
    const string &strO=cCommon->CommonBlank()) const
  { // Find item and return it else return the default item
    const StrStrMapConstIt eiEnv{ ssmEnv.find(strEnv) };
    return eiEnv == ssmEnv.cend() ? strO : eiEnv->second;
  }
  /* -- Get environment variable and check that it is a valid pathname ----- */
  const string CmdLineMakeEnvPath(const string &strEnv,
    const string &strSuffix)
  { // Get home environment variable and throw error if not found
    const StrStrMapConstIt eiEnv{ ssmEnv.find(strEnv) };
    if(eiEnv == ssmEnv.cend())
      XC("The specified environment variable is required and missing!",
        "Variable", strEnv, "Suffix", strSuffix);
    // Check validity of the specified environmen variable
    const string &strEnvVal = eiEnv->second;
    const ValidResult vRes = DirValidName(strEnvVal, VT_TRUSTED);
    if(vRes == VR_OK) return StrAppend(strEnvVal, strSuffix);
    // Show error otherwise
    XC("The specified environment variable directory is invalid!",
      "Variable", strEnv, "Suffix", strSuffix, "Directory", strEnvVal,
      "Result",   vRes,   "Reason", cDirBase->DirBaseVNRtoStr(vRes));
  }
  /* -- Get parameter total ------------------------------------------------ */
  size_t CmdLineGetTotalCArgs() const
    { return static_cast<size_t>(iArgC); }
  ArgType*const*CmdLineGetCArgs() const { return atArgs; }
  ArgType*const*CmdLineGetCEnv() const { return atEnv; }
  const StrStrMap &CmdLineGetEnvList() const { return ssmEnv; }
  const StrVector &CmdLineGetArgList() const { return svArg; }
  /* -- Set restart flag (0 = no restart, 1 = no params, 2 = params) ------- */
  void CmdLineSetRestart(const ExitOperation ecCmd) { eoExit = ecCmd; }
  /* -- Get startup current directory -------------------------------------- */
  const string &CmdLineGetStartupCWD() const { return strCWD; }
  /* -- Return to startup directory ---------------------------------------- */
  void CmdLineSetStartupCWD()
  { // Try to set the startup working direcotry and throw if failed.
    if(!DirSetCWD(CmdLineGetStartupCWD()))
      XCL("Failed to set startup working directory!",
        "Directory", CmdLineGetStartupCWD());
  }
  /* -- Parse command line arguments --------------------------------------- */
  StrVector CmdLineParseArgArray()
  { // Check that args are valid
    if(iArgC < 1) XC("Arguments array count corrupted!", "Count", iArgC);
    // Check that args are valid
    if(!atArgs) XC("Arguments array corrupted!");
    if(!*atArgs) XC("Arguments array executable string corrupted!");
    if(!**atArgs) XC("Arguments array executable string is empty!");
    // Arguments list to return
    const size_t stArgCM1 = static_cast<size_t>(iArgC - 1);
    Reserved<StrVector> svRet{ stArgCM1 };
    // For each argument format the argument and add it to list
    StdForEach(seq, atArgs + 1, atArgs + iArgC,
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
  { // Check that environment are valid
    if(!atEnv) XC("Evironment array corrupted!");
    if(!*atEnv) XC("First environment variable corrupted!");
    if(!**atEnv) XC("First environment varable is empty!");
    // Arguments list to return
    StrStrMap ssmRet;
    // Compile on MacOS and in debug mode?
#if defined(MACOS) && defined(ALPHA)
    // Hacky method to avoid address sanitiser false-positive in XCode
    for(ArgType *atPtr = *atEnv, *atStr = atPtr; *atStr; atStr = ++atPtr)
    { // Skip all non-null characters then we have the end of the c-string
      while(*atPtr) ++atPtr;
#else
    // Process environment variables
    for(ArgType **atPtr = atEnv; ArgType*const atStr = *atPtr; ++atPtr)
    { // Ignore if string is empty
      if(!*atStr) continue;
#endif
      // Split argument into key/value pair. Ignore if no parameters
      if(Token tokParam{ S16toUTF(atStr), cCommon->CommonEquals(), 2 })
        ssmRet.insert({ StdMove(tokParam.front()), tokParam.size() >= 2 ?
          StdMove(tokParam.back()) : cCommon->CommonBlank() });
    } // Unset unallowed variables
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
  /* -- Assign arguments ---------------------------------------- */ protected:
  CmdLine(const int iArgs, ArgType**const atNArgs, ArgType**const atNEnv) :
    /* -- Initialisers ----------------------------------------------------- */
    eoExit(EO_QUIT),                   // Initialise exit code
    iArgC(iArgs),                      // Initialise stdlib args count
    atArgs(atNArgs),                   // Initialise stdlib args ptr
    atEnv(atNEnv),                     // Initialise stdlib environment ptr
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
    if(iArgC <= 0) return;
    // Restore startup working directory
    CmdLineSetStartupCWD();
    // Do we have a restart mode set?
    switch(eoExit)
    { // Just return if no restart required
      case EO_QUIT: return;
      // Reboot with no arguments?
      case EO_TERM_REBOOT_NOARG:
        // Remove first parameter and fall through to reboot
        atArgs[1] = nullptr; iArgC = 1; [[fallthrough]];
      // Restart while keeping parameters?
      case EO_TERM_REBOOT: CmdLineSetRestart(EO_QUIT);
        // Do the restart and replace the current process with the new one
        switch(const int iCode = StdExecVE(atArgs, atEnv))
        { // Success? Shouldn't get here!
          case 0: break;
          // Error occured? Don't attempt execution again and show error
          default: XCL("Failed to restart process!",
            "Process", *atArgs, "Code", iCode, "Parameters", iArgC);
        } // Done
        break;
      // Remove first parameter and fallthrough to next label
      case EO_UI_REBOOT_NOARG: atArgs[1] = nullptr; iArgC = 1; [[fallthrough]];
      // Restart while keeping parameters in ui mode?
      case EO_UI_REBOOT: CmdLineSetRestart(EO_QUIT);
        // Do the restart using spawn as MacOS is weird with ui apps otherwise.
        switch(const int iCode = StdSpawnVE(atArgs, atEnv))
        { // Success? Proceed to quit
          case 0: break;
          // Error occurred? Don't attempt execution again and show error
          default: XCL("Failed to spawn new process!",
            "Process", *atArgs, "Code", iCode, "Parameters", iArgC);
        } // Done
        break;
      // Anything else? (Impossible but needed to prevent compiler warning)
      default: XC("Internal error: Invalid exit command!", "Command", eoExit);
    }
  } // Exception occured?
  catch(const exception &eReason) \
  { // Show message box as the log is not available.
    SysMessage("CmdLine Shutdown Exception", eReason.what(), MB_ICONSTOP);
  }
};/* ----------------------------------------------------------------------- */
};                                     // End of public module namespace
/* ------------------------------------------------------------------------- */
};                                     // End of private module namespace
/* == EoF =========================================================== EoF == */
