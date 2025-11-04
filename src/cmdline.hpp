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
using namespace IToken::P;             using namespace IUtil::P;
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
  ArgType        **lArgV;              // Arguments list
  ArgType        **lEnvP;              // Environment list
  const StrVector  svArg;              // Arguments list
  const StrStrMap  lEnv;               // Formatted environment variables
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
    const StrStrMapConstIt eiEnv{ lEnv.find(strEnv) };
    return eiEnv == lEnv.cend() ? strO : eiEnv->second;
  }
  /* -- Get environment variable and check that it is a valid pathname ----- */
  const string CmdLineMakeEnvPath(const string &strEnv,
    const string &strSuffix)
  { // Get home environment variable and throw error if not found
    const StrStrMapConstIt eiEnv{ lEnv.find(strEnv) };
    if(eiEnv == lEnv.cend())
      XC("The specified environment variable is required and missing!",
         "Variable", strEnv, "Suffix", strSuffix);
    // Check validity of the specified environmen variable
    const string &strEnvVal = eiEnv->second;
    const ValidResult vRes = DirValidName(strEnvVal, VT_TRUSTED);
    if(vRes == VR_OK) return StrAppend(strEnvVal, strSuffix);
    // Show error otherwise
    XC("The specified environment variable directory is invalid!",
       "Variable",  strEnv,
       "Suffix",    strSuffix,
       "Directory", strEnvVal,
       "Reason",    cDirBase->DirBaseVNRtoStr(vRes),
       "Result",    vRes);
  }
  /* -- Get parameter total ------------------------------------------------ */
  size_t CmdLineGetTotalCArgs() const
    { return static_cast<size_t>(iArgC); }
  ArgType*const*CmdLineGetCArgs() const { return lArgV; }
  ArgType*const*CmdLineGetCEnv() const { return lEnvP; }
  const StrStrMap &CmdLineGetEnvList() const { return lEnv; }
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
    if(!lArgV) XC("Arguments array corrupted!");
    if(!*lArgV) XC("Arguments array executable string corrupted!");
    if(!**lArgV) XC("Arguments array executable string is empty!");
    // Arguments list to return
    const size_t stArgCM1 = static_cast<size_t>(iArgC - 1);
    StrVector svRet; svRet.reserve(stArgCM1);
    // For each argument format the argument and add it to list
    StdForEach(seq, lArgV+1, lArgV+iArgC,
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
    if(!lEnvP) XC("Evironment array corrupted!");
    if(!*lEnvP) XC("First environment variable corrupted!");
    if(!**lEnvP) XC("First environment varable is empty!");
    // Arguments list to return
    StrStrMap ssmRet;
    // Compile on MacOS and in debug mode?
#if defined(MACOS) && defined(ALPHA)
    // Hacky method to avoid address sanitiser false-positive in XCode
    for(ArgType *atPtr = *lEnvP, *atStr = atPtr; *atStr; atStr = ++atPtr)
    { // Skip all non-null characters then we have the end of the c-string
      while(*atPtr) ++atPtr;
#else
    // Process environment variables
    for(ArgType **atPtr = lEnvP; ArgType*const atStr = *atPtr; ++atPtr)
    { // Ignore if string is empty
      if(!*atStr) continue;
#endif
      // Split argument into key/value pair. Ignore if no parameters
      if(Token tokParam{ S16toUTF(atStr), cCommon->CommonEquals(), 2 })
        ssmRet.insert({ StdMove(tokParam.front()), tokParam.size() >= 2 ?
          StdMove(tokParam.back()) : cCommon->CommonBlank() });
    }
    // Compile on MacOS?
#if defined(MACOS)
    // Unset variables for process children because these variables can cause
    // spawned terminal apps to spit garbage output and ruin the display for
    // scripts. If the guest needs these then they can just try the apps
    // standalone in the terminal without the need for the engine.
    SysUnSetEnv(
      "DYLD_INSERT_LIBRARIES",         // Disable dylib override
      "MallocCheckHeapAbort",          // Don't throw abort() on heap check
      "MallocCheckHeapEach",           // Don't check heap every 'n' mallocs
      "MallocCheckHeapStart",          // Don't check heap at 'n' mallocs
      "MallocGuardEdges",              // Don't guard edges
      "MallocHelp",                    // Help messages
      "MallocScribble",                // Don't scribble memory
      "NSDeallocateZombies",           // Deallocate zombies
      "NSZombieEnabled"                // Enable dealloc in foundation
    );
#endif
    // Return environment variables list
    return ssmRet;
  }
  /* -- Assign arguments ---------------------------------------- */ protected:
  CmdLine(const int iArgs, ArgType**const atArgs, ArgType**const atEnv) :
    /* -- Initialisers ----------------------------------------------------- */
    eoExit(EO_QUIT),                   // Initialise exit code
    iArgC(iArgs),                      // Initialise stdlib args count
    lArgV(atArgs),                     // Initialise stdlib args ptr
    lEnvP(atEnv),                      // Initialise stdlib environment ptr
    svArg{ StdMove(                    // Initialise command line arguments
      CmdLineParseArgArray()) },       // ...so we can keep them const
    lEnv{ StdMove(                     // Initialise environment variables
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
        lArgV[1] = nullptr; iArgC = 1; [[fallthrough]];
      // Restart while keeping parameters?
      case EO_TERM_REBOOT: CmdLineSetRestart(EO_QUIT);
        // Do the restart and replace the current process with the new one
        switch(const int iCode = StdExecVE(lArgV, lEnvP))
        { // Success? Shouldn't get here!
          case 0: break;
          // Error occured? Don't attempt execution again and show error
          default: XCL("Failed to restart process!",
            "Process", *lArgV, "Code", iCode, "Parameters", iArgC);
        } // Done
        break;
      // Remove first parameter and fallthrough to next label
      case EO_UI_REBOOT_NOARG: lArgV[1] = nullptr; iArgC = 1; [[fallthrough]];
      // Restart while keeping parameters in ui mode?
      case EO_UI_REBOOT: CmdLineSetRestart(EO_QUIT);
        // Do the restart using spawn as MacOS is weird with ui apps otherwise.
        switch(const int iCode = StdSpawnVE(lArgV, lEnvP))
        { // Success? Proceed to quit
          case 0: break;
          // Error occurred? Don't attempt execution again and show error
          default: XCL("Failed to spawn new process!",
            "Process", *lArgV, "Code", iCode, "Parameters", iArgC);
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
