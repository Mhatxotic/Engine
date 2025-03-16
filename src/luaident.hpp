/* == LUAIDENT.HPP ========================================================= **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## This module is a class for LUA member classes which contains a      ## **
** ## table reference from the 'llcirAPI' Lua class reference list        ## **
** ## (defined in luadef.hpp). This is so when creating new classes using ## **
** ## the LUA, we have a direct reference to the table of member          ## **
** ## functions and variables.                                            ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace ILuaIdent {                  // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace ILuaLib::P;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* -- Class for collector class name in LUA -------------------------------- */
class LuaIdent {
  /* -- Private variables -------------------------------------------------- */
  const string_view svName;            // Name of class
  /* -- Private variables ----------------------------------------- */ private:
  const int        &iRef;              // Reference to namespace in Lua
  /* -- Get reference ---------------------------------------------- */ public:
  int LuaIdentGetRef(void) const { return iRef; }
  /* -- Get name of class -------------------------------------------------- */
  const char* LuaIdentCStr(void) const { return svName.data(); }
  /* -- Get string view name of class -------------------------------------- */
  const string_view &LuaIdentStr(void) const { return svName; }
  /* -- Constructor --------------------------------------------- */ protected:
  explicit LuaIdent(const string_view svNName, const LuaClassId lciId) :
    /* --------------------------------------------------------------------- */
    svName{ svNName },                 // Set name of class
    iRef(llcirAPI[lciId])              // Alias Lua class reference storage
    /* -- No code ---------------------------------------------------------- */
    { }
};/* ----------------------------------------------------------------------- */
};                                     // End of private module namespace
/* ------------------------------------------------------------------------- */
};                                     // End of public module namespace
/* == EoF =========================================================== EoF == */
