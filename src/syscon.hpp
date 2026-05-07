/* == SYSCON.HPP =========================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## This is the header as a base to handle terminal output from headers ## **
** ## 'pixcon.hpp' and 'wincon.hpp'.                                      ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace ISysCon {                    // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace ICVarDef::P;           using namespace IFlags::P;
using namespace IMutex::P;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* == SysConBase class ===================================================== **
** ######################################################################### **
** ## Base class for SysCon                                               ## **
** ######################################################################### **
** -- Typedefs ------------------------------------------------------------- */
BUILD_FLAGS(SysCon,                    // Console flags classes
  /* ----------------------------------------------------------------------- */
  SCO_NONE                  {Flag(0)}, // No settings?
  SCO_CURVISIBLE            {Flag(1)}, // Cursor is visible?
  SCO_CURINSERT             {Flag(2)}, // Cursor is in insert mode?
  SCO_EXIT                  {Flag(3)}  // Exit requested?
);/* ----------------------------------------------------------------------- */
struct SysConBase :
  /* -- Base classes ------------------------------------------------------- */
  protected SysConFlags                // Flags settings
{ /* -- Typedefs ----------------------------------------------------------- */
  enum KeyType { KT_NONE, KT_KEY, KT_CHAR, KT_RESET }; // GetKey return types
  /* -- Handle CTRL_CLOSE_EVENT -------------------------------------------- */
  bool SysConIsClosing() const { return FlagIsSet(SCO_EXIT); }
  bool SysConIsNotClosing() const { return !SysConIsClosing(); }
  void SysConCanCloseNow() { cvExit.notify_one(); }
   /* -- For handling CTRL_CLOSE_EVENT -------------------------- */ protected:
  bool            bBreakEnabled;       // Is control+c/break enabled?
  condition_variable cvExit;           // Exit condition variable
  /* -- Constructor -------------------------------------------------------- */
  SysConBase() :
    /* -- Initialisers ----------------------------------------------------- */
    SysConFlags{ SCO_NONE },           // Current flags
    bBreakEnabled(false)               // Initialised by cvar
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- Constructor ------------------------------------------------ */ public:
  CVarReturn SysConSetBreak(const bool bEnabled)
    { bBreakEnabled = bEnabled; return ACCEPT; }
};/* ----------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
