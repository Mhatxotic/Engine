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
using namespace IArgs;                 using namespace ICollector::P;
using namespace IConsole::P;           using namespace IError::P;
using namespace IIdent::P;             using namespace ILog::P;
using namespace ILuaFunc::P;           using namespace ILuaLib::P;
using namespace ILuaUtil::P;           using namespace ILua::P;
using namespace IStd::P;               using namespace IString::P;
using namespace ISysUtil::P;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* ------------------------------------------------------------------------- */
typedef pair<LuaFunc, CmdMapIt> LuaCmdPair;      // Lua id/cvar list
MAPPACK_BUILD(LuaCmd, const string, LuaCmdPair); // Map for lua vars
/* -- Variables ollector class for collector data and custom variables ----- */
CTOR_BEGIN(Commands, Command, CLHelperSafe,
  /* ----------------------------------------------------------------------- */
  LuaCmdMap        lcmMap;             // Lua console command list
);/* -- Lua command collector and member class ----------------------------- */
CTOR_MEM_BEGIN_CSLAVE(Commands, Command, ICHelperUnsafe),
  /* -- Base classes ------------------------------------------------------- */
  public Lockable                      // Lua garbage collector instruction
{ /* -- Private variables -------------------------------------------------- */
  constexpr static const size_t
    stConCmdMinLength = 1,             // Minimum length of a console command
    stConCmdMaxLength = 255;           // Maximum length of a console command
  /* -- Private variables -------------------------------------------------- */
  LuaCmdMapIt      lcmiIt;             // Iterator to command Console gives us
  /* -- Returns the lua command list --------------------------------------- */
  LuaCmdMap &GetLuaCmdsList(void) { return cParent->lcmMap; }
  /* -- Returns the end of the lua command list ---------------------------- */
  LuaCmdMapIt GetLuaCmdsListEnd(void) { return GetLuaCmdsList().end(); }
  /* -- Push and get error callback function id ---------------------------- */
  static void LuaCallbackStatic(const Args &aArgs)
  { // Find command in console command list and log if not found (impossible)
    const LuaCmdMapIt lcmiIt{ cCommands->lcmMap.find(aArgs.front()) };
    if(lcmiIt == cCommands->lcmMap.end())
      cLog->LogWarningExSafe("Command can't find virtual command '$'!",
        aArgs.front());
    // Call the function callback in Lua
    lcmiIt->second.first.LuaFuncProtectedDispatch(0, aArgs);
  }
  /* -- Check that the console variable name is valid ---------------------- */
  bool IsValidConsoleCommandName(const string &strName)
  { // Check minimum name length
    if(strName.length() < stConCmdMinLength ||
       strName.length() > stConCmdMaxLength) return false;
    // Get address of string. The first character must be a letter
    const unsigned char *ucpPtr =
      reinterpret_cast<const unsigned char*>(strName.c_str());
    if(!isalpha(*ucpPtr)) return false;
    // For each character in cvar name until end of string...
    for(const unsigned char*const ucpPtrEnd = ucpPtr + strName.length();
                                   ++ucpPtr < ucpPtrEnd;)
      if(!isalnum(*ucpPtr) && *ucpPtr != '_') return false;
    // Success!
    return true;
  }
  /* -- Unregister the console command from lua -------------------- */ public:
  const string &Name(void) const { return lcmiIt->first; }
  /* -- Register user console command from lua ----------------------------- */
  void Init(lua_State*const lS, const string &strName,
    const unsigned int uiMinimum, const unsigned int uiMaximum)
  { // Must be running on the main thread
    cLua->StateAssert(lS);
    // Check that the console command is valid
    if(!IsValidConsoleCommandName(strName))
      XC("Console command name is invalid!",
         "Command", strName, "Minimum", stConCmdMinLength,
         "Maximum", stConCmdMaxLength);
    // Check min/Max params and that they're valid
    if(uiMinimum && uiMaximum && uiMaximum < uiMinimum)
      XC("Minimum greater than maximum!",
         "Identifier", strName, "Minimum",  uiMinimum, "Maximum", uiMaximum);
    // Find command and throw exception if already exists
    if(GetLuaCmdsList().contains(strName))
      XC("Virtual command already exists!", "Command", strName);
    // Since the userdata for this class object is at arg 5, we need to make
    // sure the callback function is ahead of it in arg 6 or the LuaFunc()
    // class which calls luaL_ref will fail as it ONLY reads position -1.
    LuaUtilCopyValue(lS, 4);
    // Register the variable and get the iterator to the new cvar. Don't
    // forget the lua reference needs to be in place for when the callback
    // is called. Create a function and reference the function on the lua
    // stack and insert the reference into the list
    lcmiIt = GetLuaCmdsList().insert(GetLuaCmdsListEnd(), { StdMove(strName),
      make_pair(LuaFunc{ StrAppend("CC:", strName), true },
        cConsole->RegisterCommand(strName,
          uiMinimum, uiMaximum, LuaCallbackStatic)) });
  }
  /* -- Destructor that unregisters the cvar ------------------------------- */
  ~Command(void)
  { // Return if iterator is not registered
    if(lcmiIt == GetLuaCmdsListEnd()) return;
    // Unregister the command if set
    if(lcmiIt->second.second != cConsole->GetCmdsListEnd())
      cConsole->UnregisterCommand(lcmiIt->second.second);
    // Erase the item from the list
    GetLuaCmdsList().erase(lcmiIt);
  }
  /* -- Basic constructor with no init ----------------------------- */ public:
  Command(void) :
    /* -- Initialisers ----------------------------------------------------- */
    ICHelperCommand{                   // Initialise and register the object
      cCommands, this },
    IdentCSlave{ cParent->CtrNext() }, // Initialise identification number
    lcmiIt{ GetLuaCmdsListEnd() }      // Initialise iterator to the last
    /* --------------------------------------------------------------------- */
    { }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(Command)             // Suppress default functions for safety
};/* ----------------------------------------------------------------------- */
CTOR_END_NOINITS(Commands, Command, COMMAND) // Finish global Files collector
/* -- Build a command list (for conlib) ------------------------------------ */
template<class ListType>
  static size_t CommandsBuildList(const ListType &ltList,
    const string &strFilter, string &strDest)
{ // Commands matched
  size_t stMatched = 0;
  // Set filter if specified and look for command and if we found one?
  // The 'auto' here could either be 'CmdMapIt' or 'LuaCmdMapIt'.
  auto ltIt{ ltList.lower_bound(strFilter) };
  if(ltIt != ltList.cend())
  { // Output string
    ostringstream osS;
    // Build output string
    do
    { // If no match found? return original string
      const string &strKey = ltIt->first;
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
