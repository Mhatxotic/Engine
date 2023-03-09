/* == PIXMOD.HPP =========================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## This is a POSIX specific module that parses executable files to     ## **
** ## reveal information about it. (ToDo: This is not implemented yet!).  ## **
** ## Since we support MacOS and Linux, we can support both systems very  ## **
** ## simply with POSIX compatible calls.                                 ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* == Version information subclass ========================================= */
struct SysModule :
  /* -- Base classes ------------------------------------------------------- */
  public SysModuleData                 // System module data
{ /* -- Manual data (i.e. for executable) ---------------------------------- */
  explicit SysModule(const string &strModule) :
    /* -- Initialisers ----------------------------------------------------- */
    SysModuleData{ strModule }
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Manual data (i.e. for executable) ---------------------------------- */
  explicit SysModule(string &&strModule) :
    /* -- Initialisers ----------------------------------------------------- */
    SysModuleData{ StdMove(strModule) }
    /* -- No code ---------------------------------------------------------- */
    { }
   /* -- Return data (move filename) --------------------------------------- */
  explicit SysModule(string &&strModule, const unsigned int uiMa,
    const unsigned int uiMi, const unsigned int uiBu, const unsigned int uiRe,
    string &&strVen, string &&strDe, string &&strCo, string &&strVer) :
    /* -- Initialisers ----------------------------------------------------- */
    SysModuleData{ StdMove(strModule), uiMa, uiMi, uiBu, uiRe,
      StdMove(strVen), StdMove(strDe), StdMove(strCo),
      StdMove(strVer) }
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Return data (copy filename) ---------------------------------------- */
  explicit SysModule(const string &strModule, const unsigned int uiMa,
    const unsigned int uiMi, const unsigned int uiBu, const unsigned int uiRe,
    string &&strVen, string &&strDe, string &&strCo, string &&strVer) :
    /* -- Initialisers ----------------------------------------------------- */
    SysModuleData{ strModule, uiMa, uiMi, uiBu, uiRe, StdMove(strVen),
      StdMove(strDe), StdMove(strCo), StdMove(strVer) }
    /* -- No code ---------------------------------------------------------- */
    { }
};/* -- End ---------------------------------------------------------------- */
/* == EoF =========================================================== EoF == */
