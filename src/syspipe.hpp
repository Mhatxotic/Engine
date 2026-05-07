/* == SYSPIPE.HPP ========================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## This is the header that interfaces the operating systems terminal   ## **
** ## output piping system interface with the engine.                     ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace ISysPipe {                   // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IName::P;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* == System pipe base class =============================================== */
class SysPipeBase :
  /* -- Base classes ------------------------------------------------------- */
  public NameStr                       // Name of the pipe
{ /* ----------------------------------------------------------------------- */
  int64_t          llExitCode;         // Process exit code
  /* -- Set status code ----------------------------------------- */ protected:
  void SysPipeBaseSetStatus(const int64_t llNewCode)
    { llExitCode = llNewCode; }
  /* -- Return status code ----------------------------------------- */ public:
  int64_t SysPipeBaseGetStatus() const { return llExitCode; }
  /* -- Constructor with init ---------------------------------------------- */
  SysPipeBase() :
    /* -- Initialisers ----------------------------------------------------- */
    llExitCode(127)                    // Standard exit code
    /* -- No code ---------------------------------------------------------- */
    {}
};/* -- End ---------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
