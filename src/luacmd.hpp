/* == LUACMD.HPP =========================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## Handles Lua Commands.                                               ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace ILuaCommand {                // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IArgs::P;              using namespace ICollector::P;
using namespace IConsole::P;           using namespace IError::P;
using namespace ILockable::P;          using namespace ILog::P;
using namespace ILuaFunc::P;           using namespace ILuaIdent::P;
using namespace ILuaLib::P;            using namespace ILuaUtil::P;
using namespace IName::P;              using namespace ISerial::P;
using namespace IStd::P;               using namespace IString::P;
using namespace ISysUtil::P;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* ------------------------------------------------------------------------- */
using LuaCmdPair = StdPair<LuaFunc, CmdMapIt>; // Lua id/cvar list
MAPPACK_BUILD(LuaCmd, const StdString, LuaCmdPair); // Map for lua vars
/* -- Variables ollector class for collector data and custom variables ----- */
CTOR_BEGIN(Commands, Command, CLHelperSafe,
  /* ----------------------------------------------------------------------- */
  LuaCmdMap        lcmMap;             // Lua console command list
);/* -- Lua command collector and member class ----------------------------- */
CTOR_MEM_BEGIN_CSLAVE(Commands, Command, ICHelperUnsafe),
  /* -- Base classes ------------------------------------------------------- */
  public Lockable                      // Lua garbage collector instruction
{ /* -- Private variables -------------------------------------------------- */
  LuaCmdMapIt      lcmiIt;             // Iterator to command Console gives us
  /* -- Returns the lua command list --------------------------------------- */
  LuaCmdMap &GetLuaCmdsList() { return cParent->lcmMap; }
  /* -- Returns the end of the lua command list ---------------------------- */
  LuaCmdMapIt GetLuaCmdsListEnd() { return GetLuaCmdsList().end(); }
  /* -- Push and get error callback function id ---------------------------- */
  static void LuaCallbackStatic(const Args &aArgs)
  { // Find command in console command list and log if not found (impossible)
    const LuaCmdMapIt lcmiIt{ cCommands->lcmMap.find(aArgs.front()) };
    if(lcmiIt == cCommands->lcmMap.cend())
      cLog->LogWarningExSafe("Command can't find virtual command '$'!",
        aArgs.front());
    // Call the function callback in Lua
    else lcmiIt->second.first.LuaFuncProtectedDispatch(0, aArgs);
  }
  /* -- Unregister the console command from lua -------------------- */ public:
  const StdString &Name() const { return lcmiIt->first; }
  /* -- Register user console command from lua ----------------------------- */
  void Init(lua_State*const lS, const StdStringView &strvName,
    const unsigned uMinimum, const unsigned uMaximum)
  { // Check that the console command is valid
    if(!cConsole->IsValidConsoleCommandName(strvName))
      XC("Console command name is invalid!",
        "Command", strvName, "Minimum", cConsole->stConCmdMinLength,
        "Maximum", cConsole->stConCmdMaxLength);
    // Check min/Max params and that they're valid
    if(uMinimum && uMaximum && uMaximum < uMinimum)
      XC("Minimum greater than maximum!",
        "Name", strvName, "Minimum",  uMinimum, "Maximum", uMaximum);
    // Find command and throw exception if already exists
    if(cConsole->CommandIsRegistered(strvName))
      XC("Console command already exists!", "Command", strvName);
    // Since the userdata for this class object is at arg 5, we need to make
    // sure the callback function is ahead of it in arg 6 or the LuaFunc()
    // class which calls luaL_ref will fail as it ONLY reads position -1.
    LuaUtilCopyValue(lS, 4);
    // Register the variable and get the iterator to the new cvar. Don't
    // forget the lua reference needs to be in place for when the callback
    // is called. Create a function and reference the function on the lua
    // stack and insert the reference into the list
    lcmiIt = GetLuaCmdsList().insert(GetLuaCmdsListEnd(),
      { StdString{ strvName },
        make_pair(LuaFunc{ StrAppend("CC:", strvName), true },
          cConsole->RegisterCommand(strvName,
            uMinimum, uMaximum, LuaCallbackStatic)) });
  }
  /* -- Basic constructor with no init ----------------------------- */ public:
  Command() :
    /* -- Initialisers ----------------------------------------------------- */
    ICHelperCommand{                   // Initialise and register the object
      cCommands, this },
    SerialSlave{ cParent->Serial() },  // Initialise identification number
    lcmiIt{ GetLuaCmdsListEnd() }      // Initialise iterator to the last
    /* --------------------------------------------------------------------- */
    {}
  /* -- Destructor that unregisters the cvar ------------------------------- */
  DTORHELPER(~Command,
    // Return if iterator is not registered
    if(lcmiIt == GetLuaCmdsListEnd()) return;
    // Unregister the command if set
    if(lcmiIt->second.second != cConsole->GetCmdsListEnd())
      cConsole->UnregisterCommand(lcmiIt->second.second);
    // Erase the item from the list
    GetLuaCmdsList().erase(lcmiIt);
  )
};/* ----------------------------------------------------------------------- */
CTOR_END_NOINITS(Commands, Command, COMMAND) // Finish global Files collector
/* -- Build a command list (for conlib) ------------------------------------ */
template<class ListType>
  static size_t CommandsBuildList(const ListType &ltList,
    const StdString &strFilter, StdString &strDest)
{ // Commands matched
  size_t stMatched = 0;
  // Set filter if specified and look for command and if we found one?
  // The 'auto' here could either be 'CmdMapIt' or 'LuaCmdMapIt'.
  auto ltIt{ ltList.lower_bound(strFilter) };
  if(ltIt != ltList.cend())
  { // Output string
    StdOStringStream osS;
    // Build output string
    do
    { // If no match found? return original string
      const StdString &strKey = ltIt->first;
      if(strKey.compare(0, strFilter.size(), strFilter)) continue;
      // Increment matched counter
      ++stMatched;
      // Add command to command list
      osS << ' ' << strKey;
    } // Until no more commands
    while(++ltIt != ltList.cend());
    // Move into destination
    strDest = osS.str();
  } // Return matches
  return stMatched;
}
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
