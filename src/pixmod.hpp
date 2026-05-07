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
/* ------------------------------------------------------------------------- */
namespace ISysMod {                    // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace ISysBase::P;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* ------------------------------------------------------------------------- */
struct SysModule :
  /* -- Base classes ------------------------------------------------------- */
  public SysModuleData                 // System module data
{ /* -- Manual data (i.e. for executable) ---------------------------------- */
  explicit SysModule(const StdString &strModule) :
    /* -- Initialisers ----------------------------------------------------- */
    SysModuleData{ strModule }
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- Manual data (i.e. for executable) ---------------------------------- */
  explicit SysModule(StdString &&strModule) :
    /* -- Initialisers ----------------------------------------------------- */
    SysModuleData{ StdMove(strModule) }
    /* -- No code ---------------------------------------------------------- */
    {}
   /* -- Return data (move filename) --------------------------------------- */
  explicit SysModule(StdString &&strModule, const unsigned uMa,
    const unsigned uMi, const unsigned uBu, const unsigned uRe,
    StdString &&strVen, StdString &&strDe, StdString &&strCo,
    StdString &&strVer) :
    /* -- Initialisers ----------------------------------------------------- */
    SysModuleData{ StdMove(strModule), uMa, uMi, uBu, uRe,
      StdMove(strVen), StdMove(strDe), StdMove(strCo),
      StdMove(strVer) }
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- Return data (copy filename) ---------------------------------------- */
  explicit SysModule(const StdString &strModule, const unsigned uMa,
    const unsigned uMi, const unsigned uBu, const unsigned uRe,
    StdString &&strVen, StdString &&strDe, StdString &&strCo,
    StdString &&strVer) :
    /* -- Initialisers ----------------------------------------------------- */
    SysModuleData{ strModule, uMa, uMi, uBu, uRe, StdMove(strVen),
      StdMove(strDe), StdMove(strCo), StdMove(strVer) }
    /* -- No code ---------------------------------------------------------- */
    {}
};/* -- End ---------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
