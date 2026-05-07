/* == LUAUTIL.HPP ========================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## Lua utility functions. They normally need a state to work.          ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace ILuaUtil {                   // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace ICommon::P;            using namespace IDir::P;
using namespace IError::P;             using namespace ILog::P;
using namespace ILuaIdent::P;          using namespace IMemory::P;
using namespace IRefCtr::P;            using namespace IStd::P;
using namespace IStdLib::P;            using namespace IString::P;
using namespace IToken::P;             using namespace IUtf::P;
using namespace IUtil::P;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* -- Variables ------------------------------------------------------------ */
static unsigned uLuaPaused = 0;        // Times Lua paused before handling it
static bool bDebugLocals = true;       // Specifies to show locals on stack
/* -- Utility type defs ---------------------------------------------------- */
struct LuaUtilClass { void *vpPtr; };  // Holds a pointer to a class
/* -- Prune stack ---------------------------------------------------------- */
static void LuaUtilPruneStack(lua_State*const lS, const int iParam)
  { lua_settop(lS, iParam); }
/* -- Return items in stack ------------------------------------------------ */
static int LuaUtilStackSize(lua_State*const lS) { return lua_gettop(lS); }
/* -- Get length of a table ------------------------------------------------ */
template<typename IntType = lua_Unsigned>
  requires StdIsIntegral<IntType>
static IntType LuaUtilGetSize(lua_State*const lS, const int iParam)
  { return static_cast<IntType>(lua_rawlen(lS, iParam)); }
/* -- Position on the stack doesn't exist? --------------------------------- */
static bool LuaUtilIsNone(lua_State*const lS, const int iParam)
  { return lua_isnone(lS, iParam) != 0; }
/* -- Position on the stack doesn't exist or is a nil? --------------------- */
static bool LuaUtilIsNoneOrNil(lua_State*const lS, const int iParam)
  { return lua_isnoneornil(lS, iParam) != 0; }
/* -- Type is a nil? ------------------------------------------------------- */
static bool LuaUtilIsNil(lua_State*const lS, const int iParam)
  { return lua_isnil(lS, iParam) != 0; }
/* -- Type is a thread? ---------------------------------------------------- */
static bool LuaUtilIsThread(lua_State*const lS, const int iParam)
  { return lua_isthread(lS, iParam) != 0; }
/* -- Type is a userdata? -------------------------------------------------- */
static bool LuaUtilIsBoolean(lua_State*const lS, const int iParam)
  { return lua_isboolean(lS, iParam) != 0; }
/* -- Type is a userdata? -------------------------------------------------- */
static bool LuaUtilIsUserData(lua_State*const lS, const int iParam)
  { return lua_isuserdata(lS, iParam) != 0; }
/* -- Type is light userdata (pointer to class)? --------------------------- */
static bool LuaUtilIsLightUserData(lua_State*const lS, const int iParam)
  { return lua_islightuserdata(lS, iParam) != 0; }
/* -- Type is a function? -------------------------------------------------- */
static bool LuaUtilIsFunction(lua_State*const lS, const int iParam)
  { return lua_isfunction(lS, iParam) != 0; }
/* -- Type is a C function? ------------------------------------------------ */
static bool LuaUtilIsCFunction(lua_State*const lS, const int iParam)
  { return lua_iscfunction(lS, iParam) != 0; }
/* -- Type is a integer? --------------------------------------------------- */
static bool LuaUtilIsInteger(lua_State*const lS, const int iParam)
  { return lua_isinteger(lS, iParam) != 0; }
/* -- Type is a number? ---------------------------------------------------- */
static bool LuaUtilIsNumber(lua_State*const lS, const int iParam)
  { return lua_isnumber(lS, iParam) != 0; }
/* -- Type is a string? ---------------------------------------------------- */
static bool LuaUtilIsString(lua_State*const lS, const int iParam)
  { return lua_isstring(lS, iParam) != 0; }
/* -- Type is a table? ----------------------------------------------------- */
static bool LuaUtilIsTable(lua_State*const lS, const int iParam)
  { return lua_istable(lS, iParam) != 0; }
/* -- Get the light user data pointer -------------------------------------- */
template<typename ParentType, typename ParentTypePtr = ParentType*>
  requires (!StdIsPointer<ParentType>)
static ParentTypePtr LuaUtilGetSimplePtr(lua_State*const lS, const int iParam)
{ // Break execution if not userdata else return pointer as requested cast
  if(!LuaUtilIsUserData(lS, iParam)) XC("Not userdata!", "Param", iParam);
  void*const vpPtr = lua_touserdata(lS, iParam);
  return reinterpret_cast<ParentTypePtr>(vpPtr);
}
/* -- Get string and size of it -------------------------------------------- */
static const char *LuaUtilToLString(lua_State*const lS, const int iParam,
  size_t &stSize) { return lua_tolstring(lS, iParam, &stSize); }
/* -- Get and return a C++ string without checking it ---------------------- */
template<typename StrType = StdStringView>
  requires StdIsString<StrType>
static StrType LuaUtilToCppString(lua_State*const lS, const int iParam = -1)
{ // Storage for string length. Do not optimise this because I am not sure
  // what the standard direction is for evaluating expression. Left-to-right
  // or right-to-left, so I will just store the string point first to be safe.
  size_t stLength;
  const char*const cpStr = LuaUtilToLString(lS, iParam, stLength);
  return { cpStr, stLength };
}
/* -- Get a number from the stack ------------------------------------------ */
template<typename FloatType = lua_Number>
  requires StdIsFloat<FloatType>
static FloatType LuaUtilToNum(lua_State*const lS, const int iIndex)
  { return static_cast<FloatType>(lua_tonumber(lS, iIndex)); }
/* -- Get an integer from the stack ---------------------------------------- */
template<typename IntType = lua_Integer>
  requires StdIsIntegral<IntType> || StdIsEnum<IntType>
static IntType LuaUtilToInt(lua_State*const lS, const int iIndex)
  { return static_cast<IntType>(lua_tointeger(lS, iIndex)); }
/* -- Get an boolean from the stack ---------------------------------------- */
static bool LuaUtilToBool(lua_State*const lS, const int iIndex)
  { return lua_toboolean(lS, iIndex); }
/* -- Get an pointer from the stack ---------------------------------------- */
template<typename ParentType = void,
         typename ParentTypePtr = const ParentType*>
  requires (!StdIsPointer<ParentType>)
static ParentTypePtr LuaUtilToPtr(lua_State*const lS, const int iIndex)
  { return reinterpret_cast<ParentTypePtr>(lua_topointer(lS, iIndex)); }
/* -- Get human readable name of specified type id ------------------------- */
static const char *LuaUtilGetType(lua_State*const lS, const int iIndex)
  { return lua_typename(lS, lua_type(lS, iIndex)); }
/* -- Push a string onto the stack ----------------------------------------- */
static void LuaUtilPushCStr(lua_State*const lS, const auto*const aString)
  { lua_pushstring(lS, reinterpret_cast<const char*>(aString)); }
/* -- Return raw access without meta methods ------------------------------- */
static void LuaUtilGetRaw(lua_State*const lS, const int iIndex = 1)
  { lua_rawget(lS, iIndex); }
/* -- Simple class to save and restore stack ------------------------------- */
class LuaStackSaver                    // Lua stack saver class
{ /* -- Private variables -------------------------------------------------- */
  const int        iTop;               // Current stack position
  lua_State*const  lState;             // State to use
  /* -- Return stack position -------------------------------------- */ public:
  int Value() const { return iTop; }
  /* -- Restore stack position --------------------------------------------- */
  void Restore() const { LuaUtilPruneStack(lState, Value()); }
  /* -- Constructor -------------------------------------------------------- */
  explicit LuaStackSaver(lua_State*const lS) :
    iTop(LuaUtilStackSize(lS)), lState(lS) {}
  /* -- Destructor --------------------------------------------------------- */
  DTORHELPER(~LuaStackSaver, Restore())
};/* ----------------------------------------------------------------------- */
/* -- Remove item from stack ----------------------------------------------- */
static void LuaUtilRmStack(lua_State*const lS, const int iParam = -1)
  { lua_remove(lS, iParam); }
/* -- Push a part of memory as a string ------------------------------------ */
static void LuaUtilPushExtStr(lua_State*const lS, const auto*const aPtr,
  const size_t stLen, lua_Alloc laFunc = nullptr,
  void*const vpUserData = nullptr)
{ lua_pushexternalstring(lS, aPtr, stLen, laFunc, vpUserData); }
/* -- Push a string we manage onto the stack ------------------------------- */
static void LuaUtilPushExtStr(lua_State*const lS, const auto &strStr,
  lua_Alloc laFunc = nullptr, void*const vpUserData = nullptr)
{ LuaUtilPushExtStr(lS, strStr.data(), strStr.size(), laFunc, vpUserData); }
/* -- Push a literal string onto the stack --------------------------------- */
static void LuaUtilPushLStr(lua_State*const lS, const auto*const aPtr,
  const auto aSize)
{ lua_pushlstring(lS, reinterpret_cast<const char*>(aPtr),
                      static_cast<size_t>(aSize)); }
/* -- Push a C++ string onto the stack ------------------------------------- */
static void LuaUtilPushStr(lua_State*const lS, const auto &strStr)
  { LuaUtilPushLStr(lS, strStr.data(), strStr.size()); }
/* -- Get metatable entry from userdata ------------------------------------ */
static int LuaUtilGetMetaTable(lua_State*const lS, const int iIndex)
  { return lua_getmetatable(lS, iIndex); }
/* -- Return type of item in stack ----------------------------------------- */
static StdString LuaUtilGetStackType(lua_State*const lS, const int iIndex)
{ // What type of variable?
  switch(lua_type(lS, iIndex))
  { // Nil?
    case LUA_TNIL: return cCommon->CommonNil(); break;
    // A number?
    case LUA_TNUMBER:
    { // If not actually an integer? Write as normal floating-point number
      if(!LuaUtilIsInteger(lS, iIndex))
        return StrFromNum(LuaUtilToNum(lS, iIndex));
      // Get actual integer value and return it and it's hex value
      const lua_Integer liValue = LuaUtilToInt(lS, iIndex);
      return StrFormat("$ [0x$$]", liValue, StdIOSHex, liValue);
    } // A boolean?
    case LUA_TBOOLEAN: return StrFromBoolTF(LuaUtilToBool(lS, iIndex));
    // A string?
    case LUA_TSTRING:
    { // Get value of string and return value with size
      const StdString strVal{ LuaUtilToCppString(lS, iIndex) };
      return StrFormat("[$] \"$\"", strVal.size(), strVal);
    } // A table?
    case LUA_TTABLE: return StrFormat("<table:$>[$]",
      LuaUtilToPtr(lS, iIndex), LuaUtilGetSize(lS, iIndex));
    // Userdata?
    case LUA_TUSERDATA:
    { // Save stack count and restore it when leaving scope
      const LuaStackSaver lssUserData{ lS };
      // Get pointer to data
      const void*const vpPtr = LuaUtilToPtr(lS, iIndex);
      // Get metadata and return if not metadata (generic userdata)
      LuaUtilGetMetaTable(lS, -1);
      if(!LuaUtilIsTable(lS, -1)) return StrFormat("<userdata:$>", vpPtr);
      // Read internal engine name and return generic data if not a string
      LuaUtilPushExtStr(lS, cCommon->CommonLuaNameV());
      LuaUtilGetRaw(lS, -2);
      return StrFormat("<$:$>", LuaUtilIsString(lS, -1) ?
        LuaUtilToCppString(lS, -1) : "Unknown", vpPtr);
    } // Who knows? Function? Userdata?
    default: return StrFormat("<$:$>",
      LuaUtilGetType(lS, iIndex), LuaUtilToPtr(lS, iIndex));
  }
}
/* -- Return status of item in stack --------------------------------------- */
static StdString LuaUtilGetStackTokens(lua_State*const lS, const int iIndex)
{ // Fill token buffer depending on status
  return StrFromEvalTokens({
    { LuaUtilIsBoolean(lS, iIndex),       'B' },
    { LuaUtilIsCFunction(lS, iIndex),     'C' },
    { LuaUtilIsFunction(lS, iIndex),      'F' },
    { LuaUtilIsLightUserData(lS, iIndex), 'L' },
    { LuaUtilIsNil(lS, iIndex),           'X' },
    { LuaUtilIsNone(lS, iIndex),          '0' },
    { LuaUtilIsInteger(lS, iIndex),       'I' },
    { LuaUtilIsNumber(lS, iIndex),        'N' },
    { LuaUtilIsString(lS, iIndex),        'S' },
    { LuaUtilIsTable(lS, iIndex),         'T' },
    { LuaUtilIsThread(lS, iIndex),        'R' },
    { LuaUtilIsUserData(lS, iIndex),      'U' },
  });
}
/* -- Log the stack -------------------------------------------------------- */
static StdString LuaUtilGetVarStack(lua_State*const lS)
{ // If there are variables in the stack?
  if(const int iCount = LuaUtilStackSize(lS))
  { // String to return
    StdOStringStream osS;
     // For each element (1 is the first item)
    for(int iIndex = 1; iIndex <= iCount; ++iIndex)
      osS << iIndex
          << "["
          << iIndex - iCount - 1
          << "] (" << LuaUtilGetStackTokens(lS, iIndex) << ") "
          << LuaUtilGetStackType(lS, iIndex)
          << cCommon->CommonLf();
    // Return string
    return osS.str();
  } // No elements in variable stack
  return "<empty stack>";
}
/* -- Set hook ------------------------------------------------------------- */
static void LuaUtilSetHookCallback(lua_State*const lS,
  lua_Hook fcbCb, const int iC)
    { lua_sethook(lS, fcbCb, LUA_MASKCOUNT, iC); }
/* -- Push a table onto the stack ------------------------------------------ */
template<typename IntType1 = int, typename IntType2 = int>
  requires (StdIsIntegral<IntType1> || StdIsEnum<IntType1>) &&
           (StdIsIntegral<IntType2> || StdIsEnum<IntType2>)
static void LuaUtilPushTable(lua_State*const lS, const IntType1 itIndexes = 0,
  const IntType2 itKeys = 0)
{ lua_createtable(lS, UtilIntOrMax<int>(itIndexes),
                      UtilIntOrMax<int>(itKeys)); }
/* -- Push a nil onto the stack -------------------------------------------- */
static void LuaUtilPushNil(lua_State*const lS) { lua_pushnil(lS); }
/* -- Push specified integral as boolean on to the stack ------------------- */
static void LuaUtilPushBool(lua_State*const lS, const auto bValue)
  { lua_pushboolean(lS, static_cast<bool>(bValue)); }
/* -- Push a number onto the stack ----------------------------------------- */
static void LuaUtilPushNum(lua_State*const lS, const auto nValue)
  { lua_pushnumber(lS, static_cast<lua_Number>(nValue)); }
/* -- Push an integer onto the stack --------------------------------------- */
static void LuaUtilPushInt(lua_State*const lS, const auto iValue)
  { lua_pushinteger(lS, static_cast<lua_Integer>(iValue)); }
/* -- Push a memory block onto the stack as a string ----------------------- */
static void LuaUtilPushMem(lua_State*const lS, const MemConst &mcSrc)
  { LuaUtilPushLStr(lS, mcSrc.MemPtr<char>(), mcSrc.MemSize()); }
/* -- Push a pointer ------------------------------------------------------- */
static void LuaUtilPushPtr(lua_State*const lS, void*const vpPtr)
  { lua_pushlightuserdata(lS, vpPtr); }
/* -- Push multiple values of different types (use in ll*.hpp sources) ----- */
static void LuaUtilPushVar(lua_State*const) {}
template<typename ...VarArgs, typename AnyType>
  static void LuaUtilPushVar(lua_State*const lS, const AnyType &atVal,
    VarArgs &&...vaArgs)
{ // Type is STL string?
  if constexpr(StdIsSame<AnyType, StdString> ||
               StdIsSame<AnyType, StdStringView>) LuaUtilPushStr(lS, atVal);
  // Type is boolean?
  else if constexpr(StdIsSame<AnyType, bool>) LuaUtilPushBool(lS, atVal);
  // Type is any pointer type (assuming char*, don't send anything else)
  else if constexpr(StdIsPointer<AnyType>) LuaUtilPushCStr(lS, atVal);
  // Type is enum, int, long, short or int64?
  else if constexpr(StdIsIntegral<AnyType> || StdIsEnum<AnyType>)
    LuaUtilPushInt(lS, atVal);
  // Type is float or double?
  else if constexpr(StdIsFloat<AnyType>) LuaUtilPushNum(lS, atVal);
  // Strange bug in MSVC which shows no compile time stack trace
#if defined(MSVC_VANILLA)
  // Just push nil
  else LuaUtilPushNil(lS);
  // A real compiler?
#else
  // Just push nil otherwise
  else static_assert(false, "Unknown type sent in function call!");
  // Compiler check
#endif
  // Shift to next variable
  LuaUtilPushVar(lS, StdForward<VarArgs>(vaArgs)...);
}
/* -- Throw error ---------------------------------------------------------- */
static void LuaUtilErrThrow(lua_State*const lS) { lua_error(lS); }
/* ------------------------------------------------------------------------- */
static int LuaUtilProcException(lua_State*const lS,
  const StdException &eReason)
{ // Push a string onto the stack that describes the current execution context
  luaL_where(lS, 1);
  // Push the exception reason
  LuaUtilPushCStr(lS, eReason.what());
  // Concatenate both strings
  lua_concat(lS, 2);
  // Throw the error
  LuaUtilErrThrow(lS);
  // Shouldn't get here
  return 0;
}
/* -- Trampoline to wrap C closures ---------------------------------------- */
template<lua_CFunction cFunc>static int LuaUtilCallback(lua_State*const lS) try
{ // Execute the callback
  return cFunc(lS);
} // Unknown exception occured?
catch(const StdException &eReason)
{ // Throw error and return nothing (keep to a func to prevent duplicate code).
  return LuaUtilProcException(lS, eReason);
} // Don't catch all as it will catch LUA's longjmp() throw.
/* -- Get and pop string on top -------------------------------------------- */
static StdString LuaUtilGetAndPopStr(lua_State*const lS)
{ // If there is nothing on the stack then return a generic error
  if(LuaUtilIsNone(lS, -1)) return "Error signalled with no reason";
  // Not have a string on stack? Set embedded error!
  if(!LuaUtilIsString(lS, -1))
    return StrFormat("Error signalled with invalid '$' reason",
      LuaUtilGetType(lS, -1));
  // Get error string
  const StdString strError{ LuaUtilToCppString(lS) };
  // Remove the error string
  LuaUtilRmStack(lS);
  // return the error
  return strError;
}
/* -- Copy one value on the stack ------------------------------------------ */
static void LuaUtilCopyValue(lua_State*const lS, const int iIndex)
  { lua_pushvalue(lS, iIndex); }
/* -- Do the equivalent t[k] = v ------------------------------------------- */
static void LuaUtilSetField(lua_State*const lS, const int iIndex,
  const char*const cpKey)
{ lua_setfield(lS, iIndex, cpKey); }
/* -- Raw assignment without meta methods ---------------------------------- */
static void LuaUtilSetRaw(lua_State*const lS, const int iIndex = 1)
  { lua_rawset(lS, iIndex); }
/* -- Return if reference is valid ----------------------------------------- */
static bool LuaUtilIsRefValid(const int iReference)
  { return iReference != LUA_REFNIL; }
/* -- Return if reference is not valid ------------------------------------- */
static bool LuaUtilIsNotRefValid(const int iReference)
  { return !LuaUtilIsRefValid(iReference); }
/* -- Return reference ----------------------------------------------------- */
static void LuaUtilGetRefEx(lua_State*const lS, const int iTable = 1,
  const lua_Integer liIndex = 1) { lua_rawgeti(lS, iTable, liIndex); }
/* -- Return reference ----------------------------------------------------- */
static void LuaUtilGetRef(lua_State*const lS, const int iReference)
  { LuaUtilGetRefEx(lS, LUA_REGISTRYINDEX,
      static_cast<lua_Integer>(iReference)); }
/* -- Return referenced function ------------------------------------------- */
static bool LuaUtilGetRefFunc(lua_State*const lS, const int iReference)
{ // If context and reference are valid?
  if(lS && LuaUtilIsRefValid(iReference))
  { // Push the userdata onto the stack and return success if successful
    LuaUtilGetRef(lS, iReference);
    if(LuaUtilIsFunction(lS, -1)) return true;
    // Failed so remove whatever it was
    LuaUtilRmStack(lS);
  } // Failure
  return false;
}
/* -- Return referenced userdata ------------------------------------------- */
static bool LuaUtilGetRefUsrData(lua_State*const lS, const int iReference)
{ // If context and reference are valid?
  if(lS && LuaUtilIsRefValid(iReference))
  { // Push the userdata onto the stack and return success if successful
    LuaUtilGetRef(lS, iReference);
    if(LuaUtilIsUserData(lS, -1)) return true;
    // Failed so remove whatever it was
    LuaUtilRmStack(lS);
  } // Failure
  return false;
}
/* -- Remove reference to hidden variable without checking ----------------- */
static void LuaUtilRmRef(lua_State*const lS, const int iReference)
  { luaL_unref(lS, LUA_REGISTRYINDEX, iReference); }
/* -- Get a new reference without checking --------------------------------- */
static int LuaUtilRefInit(lua_State*const lS)
  { return luaL_ref(lS, LUA_REGISTRYINDEX); }
/* ------------------------------------------------------------------------- */
static StdString LuaUtilStack(lua_State*const lST)
{ // We need the root state so we can iterate through all the threads and will
  // eventually arrive at *lS as the last stack. Most of the time GetState()
  // equals to *lS anyway, just depends if it triggered in a co-routine or not.
  LuaUtilGetRef(lST, LUA_RIDX_MAINTHREAD);
  lua_State *lS = lua_tothread(lST, -1);
  LuaUtilRmStack(lST);
  // Return if state is invalid. Impossible really but just incase.
  if(!lS) return "\n- Could not find main thread!";
  // list of stack traces for coroutines. They are ordered from most recent
  // call to the root call so we need to use this list to reverse them after.
  // Also we (or even Lua) does know how many total calls there has been, we
  // can only enumerate them.
  struct Debug { lua_State*const lS; lua_Debug ldD; };
  using LuaStack = StdList<Debug>;
  using LuaStackRevIt = LuaStack::reverse_iterator;
  LuaStack lsStack;
  // Co-routine id so user knows which coroutine sub-level they were at.
  int iCoId = 0;
  // Loop until we've enumerated all the upstates
  do
  { // list of stack traces for this coroutine
    LuaStack lsThread;
    // For each stack
    for(int iParam = 0; ; ++iParam)
    { // Lua debug info container
      lua_Debug ldData;
      // Read stack data
      if(!lua_getstack(lS, iParam, &ldData)) break;
      // Set co-routine id. We're not using this 'event' var and neither does
      // LUA in lua_getinfo() according to ldebug.c.
      ldData.event = iCoId;
      // Insert into list
      lsThread.push_front({ lS, StdMove(ldData) });
    } // Move into lsStack in reverse order
    lsStack.splice(lsStack.cend(), lsThread);
    // If the top item is not a thread? We're done
    if(!LuaUtilIsThread(lS, 1)) break;
    // Set parent thread
    lS = lua_tothread(lS, 1);
    // Increment coroutine id
    iCoId++;
  } // Until theres no more upstates
  while(lS);
  // String to return
  StdOStringStream osS;
  // Stack id that will get decremented to 0 (the root call)
  size_t stId = lsStack.size();
  // For each stack trace
  for(LuaStackRevIt lsriIt{ lsStack.rbegin() };
                    lsriIt != lsStack.rend();
                  ++lsriIt)
  { // Get thread data
    Debug &dState = *lsriIt;
    lua_State*const lSt = dState.lS;
    lua_Debug &ldData = dState.ldD;
    // Query stack and ignore if failed or line is invalid and there is no name
    if(!lua_getinfo(lSt, "Slnu", &ldData) ||
      (ldData.currentline == -1 && !ldData.name)) continue;
    // Prepare start of stack trace
    osS << "\n- " << --stId << ':' << ldData.event << " = "
        << ldData.short_src;
    // We have line data? StrAppend data to string
    if(ldData.currentline != -1)
      osS << " @ " << ldData.currentline << '['
          << ldData.linedefined << '-' << ldData.lastlinedefined << ']';
    // Write rest of data
    osS << " : " << (ldData.name ? ldData.name : "?") << '('
        << (*ldData.namewhat ? ldData.namewhat : "?") << ';'
        << static_cast<unsigned>(ldData.nparams) << ';'
        << static_cast<unsigned>(ldData.nups) << ')';
    // Debug locals? Enumerate through them all
    if(bDebugLocals)
      for(int iIndex = 1;
        const char *cpVar = lua_getlocal(lSt, &ldData, iIndex);
        ++iIndex)
    { // Translate the value
      osS << "\n-- " << iIndex << ": "
          << cpVar << " = " << LuaUtilGetStackType( lSt, -1);
      // Pop the value added by lua_getlocal
      lua_pop(lSt, 1);
    }
  } // Return formatted stack string
  return osS.str();
}
/* -- Generic panic handler ------------------------------------------------ */
static int LuaUtilException(lua_State*const lS)
{ // Get error message and stack. Don't one line this because the order of
  // execution is important!
  const StdString strError{ LuaUtilGetAndPopStr(lS) };
  XC(StrAppend(strError, LuaUtilStack(lS)));
}
/* -- Generic error handler ------------------------------------------------ */
static int LuaUtilErrGeneric(lua_State*const lS) try
{ // Get error message and stack. Don't one line this because the order of
  // execution is important!
  const StdString strError{ LuaUtilGetAndPopStr(lS) };
  LuaUtilPushStr(lS, StrAppend(strError, LuaUtilStack(lS)));
  return 1;
} // Exception occured?
catch(const StdException &eReason)
{ // Push exception instead
  LuaUtilPushCStr(lS, eReason.what());
  return 1;
} // Don't catch all as it will catch LUA's longjmp() throw.
/* -- Push a function onto the stack --------------------------------------- */
static void LuaUtilPushCFunc(lua_State*const lS, lua_CFunction cFunc,
  const int iNVals = 0) { lua_pushcclosure(lS, cFunc, iNVals); }
/* -- Push a templated function onto the stack ----------------------------- */
template<lua_CFunction cFunc>
  static void LuaUtilPushCFunc(lua_State*const lS, const int iNVals = 0)
    { LuaUtilPushCFunc(lS, LuaUtilCallback<cFunc>, iNVals); }
/* -- Push the above generic error function and return its id -------------- */
static int LuaUtilPushAndGetGenericErrId(lua_State*const lS)
  { LuaUtilPushCFunc<LuaUtilErrGeneric>(lS); return LuaUtilStackSize(lS); }
/* == Generate an exception if the specified condition is false ============ */
static void LuaUtilAssert(lua_State*const lS, const bool bCond,
  const int iIndex, const char*const cpType)
{ // Return if condition is true else break execution
  if(bCond) return;
  XC("Invalid parameter!",
    "Parameter", iIndex, "Required", cpType,
    "Supplied",  LuaUtilGetType(lS, iIndex));
}
/* -- Check that parameter is a table -------------------------------------- */
static void LuaUtilCheckTable(lua_State*const lS, const int iParam)
  { LuaUtilAssert(lS, LuaUtilIsTable(lS, iParam), iParam, "table"); }
/* -- Check that parameter is a string ------------------------------------- */
static void LuaUtilCheckStr(lua_State*const lS, const int iParam)
  { LuaUtilAssert(lS, LuaUtilIsString(lS, iParam), iParam, "string"); }
/* -- Check that parameter is a string and is not empty -------------------- */
static void LuaUtilCheckStrNE(lua_State*const lS, const int iParam)
{ // Return if parameter is a string and not empty else break execution
  LuaUtilCheckStr(lS, iParam);
  if(LuaUtilGetSize(lS, iParam) > 0) return;
  XC("Non-empty string required!", "Parameter", iParam);
}
/* -- Get the specified string --------------------------------------------- */
template<typename StringType, typename StringTypePtr = const StringType*>
  requires (!StdIsPointer<StringType>) &&
    (sizeof(StringType) == sizeof(uint8_t))
static StringTypePtr LuaUtilToString(lua_State*const lS, const int iParam)
  { return reinterpret_cast<StringTypePtr>(lua_tostring(lS, iParam)); }
/* -- Get the specified string from the stack ------------------------------ */
template<typename StringType, typename StringTypePtr = const StringType*>
  static StringTypePtr LuaUtilGetStr(lua_State*const lS, const int iParam)
{ // Throw if specified parameter isn't a string else return cast
  LuaUtilCheckStr(lS, iParam);
  return LuaUtilToString<StringType, StringTypePtr>(lS, iParam);
}
/* -- Get the specified string from the stack ------------------------------ */
template<typename StringType, typename StringTypePtr = const StringType*>
  static StringTypePtr LuaUtilGetStrNE[[maybe_unused]](lua_State*const lS,
    const int iParam)
{ // Throw if specified parameter isn't a string or empty else return cast
  LuaUtilCheckStrNE(lS, iParam);
  return LuaUtilToString<StringType, StringTypePtr>(lS, iParam);
}
/* -- Get and return a string and throw exception if not a string ---------- */
template<typename StringType, typename StringTypePtr = const StringType*>
requires (!StdIsPointer<StringType>) && (sizeof(StringType) == sizeof(uint8_t))
  static StringTypePtr LuaUtilGetLStr(lua_State*const lS, const int iParam,
    size_t &stLen)
{ // Throw if specified parameter isn't a string else return a cast of it
  LuaUtilCheckStr(lS, iParam);
  return reinterpret_cast<StringTypePtr>(LuaUtilToLString(lS, iParam, stLen));
}
/* -- Helper for LuaUtilGetLStr that makes a memory block ------------------ */
static Memory LuaUtilGetMBfromLStr(lua_State*const lS, const int iParam)
{ // Get string, store size and return a conversion of it to memory class
  size_t stStrLen;
  const char*const cpStr = LuaUtilGetLStr<char>(lS, iParam, stStrLen);
  return { stStrLen, cpStr };
}
/* -- Get and return a C++ string and throw exception if not a string ------ */
template<class StrType = StdStringView>
  requires StdIsString<StrType>
static StrType LuaUtilGetCppStr(lua_State*const lS, const int iParam)
{ // Throw if requested parameter isn't a string else return it
  LuaUtilCheckStr(lS, iParam);
  return LuaUtilToCppString<StrType>(lS, iParam);
}
/* -- Get and return a C++ string and throw exception if not string/empty -- */
template<class StrType = StdStringView>
  requires StdIsString<StrType>
static StrType LuaUtilGetCppStrNE(lua_State*const lS, const int iParam)
{ // Throw if requested parameter isn't a string or empty else return it
  LuaUtilCheckStrNE(lS, iParam);
  return LuaUtilToCppString<StrType>(lS, iParam);
}
/* -- Get and return a C++ string and throw exception if not string/empty -- */
template<class StrType = StdStringView>
  requires StdIsString<StrType>
static StrType LuaUtilGetCppFile(lua_State*const lS, const int iParam)
{ // Test to make sure if supplied parameter is a valid string
  LuaUtilCheckStr(lS, iParam);
  // Get the filename and verify that the filename is valid
  const StrType strFile{ LuaUtilToCppString<StrType>(lS, iParam) };
  if(const ValidResult vrId = DirValidName(strFile))
    XC("Invalid parameter!",
      "Param",    iParam,                          "File",     strFile,
      "Reason",   cDirBase->DirBaseVNRtoStr(vrId), "ReasonId", vrId);
  // Return the constructed string
  return strFile;
}
/* -- Get and return a C++ string and throw exception if not string/empty -- */
template<class StrType = StdStringView>
  requires StdIsString<StrType>
static StrType LuaUtilGetCppDir(lua_State*const lS, const int iParam)
{ // Test to make sure if supplied parameter is a valid string.
  LuaUtilCheckStr(lS, iParam);
  // Get the filename and verify that the filename is valid.
  const StrType strFile{ LuaUtilToCppString<StrType>(lS, iParam) };
  switch(const ValidResult vrId = DirValidName(strFile))
  { // Ok or current directory? Allow the name.
    case VR_OK: case VR_CURRENT: return strFile;
    // Anything else?
    default: XC("Invalid parameter!",
      "Param",  iParam,                          "File",     strFile,
      "Reason", cDirBase->DirBaseVNRtoStr(vrId), "ReasonId", vrId);
  } // We don't get here.
}
/* -- Check if valid hostname helper --------------------------------------- */
template<typename StrType, class FuncsClass>
  requires StdIsString<StrType>
static auto LuaUtilGetHostname(lua_State*const lS, const int iParam)
{ // Return if parameter is a not a string or empty
  LuaUtilCheckStr(lS, iParam);
  StrType strType{ LuaUtilToCppString<StrType>(lS, iParam) };
  constexpr const size_t stMinimum = 1, stMaximum = 253;
  if(strType.size() < stMinimum || strType.size() > stMaximum)
    return FuncsClass{}.Length(iParam,
      stMinimum, stMaximum, strType.size());
  // Position where error occurds (domain parts
  constexpr size_t stDomainSize = 63;
  size_t stFirstDot = 0, stLastDot = stDomainSize;
  // Make sure the characters are valid
  using StrTypeIt = typename StrType::const_iterator;
  for(StrTypeIt stciIt{ strType.begin() }; stciIt != strType.end(); ++stciIt)
  { // Dereference character and check characters
    const char coChar = *stciIt;
    const size_t stPos = StdDistance<size_t>(strType.begin(), stciIt);
    if(StdIsNotAlpha(coChar) && StdIsNotDigit(coChar) &&
       coChar != '.' && coChar != '-' && coChar != '_')
      return FuncsClass{}.Char(iParam, stPos);
    // Domain separator?
    if(coChar == '.')
    { // Two periods cant be together
      const size_t stNextPos = stPos + 1;
      if(stFirstDot == stPos || stNextPos >= strType.size())
        return FuncsClass{}.Empty(iParam, stPos);
      // Prepare next domain
      stFirstDot = stNextPos;
      stLastDot = stPos + stDomainSize;
    } // Domain too long
    else if(stPos > stLastDot) return FuncsClass{}.Long(iParam, stPos);
  } // Valid hostname
  return FuncsClass{}.Complete(strType);
}
/* -- Check if valid hostname ---------------------------------------------- */
static bool LuaUtilValidHostname(lua_State*const lS, const int iParam)
{ // Functions to send to class.
  struct Functions
  { // Invalid length
    static bool Length(const int, const size_t, const size_t, const size_t)
      { return false; }
    // Invalid character
    static bool Char(const int, const size_t) { return false; }
    // Empty domain part
    static bool Empty(const int, const size_t) { return false; }
    // Domain part too long
    static bool Long(const int, const size_t) { return false; }
    // Successful verification
    static bool Complete(const StdStringView &) { return true; }
    // Constructor that does nothing
    Functions() {}
  };// Return if parameter is a string and not empty else break execution
  return LuaUtilGetHostname<StdStringView, Functions>(lS, iParam);
}
/* -- Get a valid hostname ------------------------------------------------- */
static StdString LuaUtilGetCppHostname(lua_State*const lS, const int iParam)
{ // Functions to send to class. We could just use lambdas but we can't
  // because we need to use [[noreturn]] which we can't until C++23.
  struct Functions
  { // Invalid length
    static StdString Length[[noreturn]](const int iParam,
      const size_t stMinimum, const size_t stMaximum, const size_t stSize)
    { XC("Invalid hostname length!",
        "Parameter",  iParam,    "NotLower", stMinimum,
        "NotGreater", stMaximum, "Supplied", stSize); }
    // Invalid character
    static StdString Char[[noreturn]](const int iParam, const size_t stPos)
    { XC("Invalid hostname characters!",
        "Parameter",  iParam, "At", stPos); }
    // Empty domain part
    static StdString Empty[[noreturn]](const int iParam, const size_t stPos)
    { XC("Empty domain!", "Parameter", iParam, "At", stPos); }
    // Domain part too long
    static StdString Long[[noreturn]](const int iParam, const size_t stPos)
    { XC("Invalid domain length!", "Parameter", iParam, "At", stPos); }
    // Successful verification
    static StdString Complete(const StdString &strHostname)
      { return strHostname; }
    // Constructor that does nothing
    Functions() {}
  };// Return if parameter is a string and not empty else break execution
  StdString strOut{ LuaUtilGetHostname<StdString, Functions>(lS, iParam) };
  // Lowercase and return result
  return StrToLowCaseRef(strOut);
}
/* -- Get and return a C++ string and throw exception if not a string ------ */
static StdString LuaUtilGetCppStrUpper(lua_State*const lS, const int iParam)
{ // Throw if requested parameter isn't a string else return it in uppercase
  StdString strStr{ LuaUtilGetCppStrNE(lS, iParam) };
  return StrToUpCaseRef(strStr);
}
/* -- Check the specified number of parameters are set --------------------- */
static void LuaUtilCheckParams(lua_State*const lS, const int iCount)
{ // Return if correct number of parameters else break execution
  const int iTop = LuaUtilStackSize(lS);
  if(iCount == iTop) return;
  XC((iCount < iTop) ? "Too many arguments!" : "Not enough arguments!",
    "Supplied", iTop, "Required", iCount);
}
/* -- Check multiple functions are valid ----------------------------------- */
static void LuaUtilCheckFunc(lua_State*const) {}
template<typename ...VarArgs>
  static void LuaUtilCheckFunc(lua_State*const lS, const int iIndex,
    VarArgs &&...vaArgs)
{ LuaUtilAssert(lS, LuaUtilIsFunction(lS, iIndex), iIndex, "function");
  LuaUtilCheckFunc(lS, StdForward<VarArgs>(vaArgs)...); }
/* -- Get and return a boolean and throw exception if not a boolean -------- */
static bool LuaUtilGetBool(lua_State*const lS, const int iIndex)
{ // Throw if requested parameter isn't a boolean else return it
  LuaUtilAssert(lS, LuaUtilIsBoolean(lS, iIndex), iIndex, "boolean");
  return LuaUtilToBool(lS, iIndex);
}
/* -- Try to get and check a valid number not < or >= ---------------------- */
template<typename IntType>
  requires StdIsFloat<IntType>
static IntType LuaUtilGetNum(lua_State*const lS, const int iIndex)
{ // Throw if requested parameter isn't a number else return a cast of it
  LuaUtilAssert(lS, LuaUtilIsNumber(lS, iIndex), iIndex, "number");
  return LuaUtilToNum<IntType>(lS, iIndex);
}
/* -- Try to get and check a valid number not < ---------------------------- */
template<typename FloatType>
  requires StdIsFloat<FloatType>
static FloatType LuaUtilGetNumL(lua_State*const lS, const int iIndex,
  const FloatType ftMin)
{ // Return number if valid and in range else break execution
  const FloatType ftVal = LuaUtilGetNum<FloatType>(lS, iIndex);
  if(ftVal >= ftMin) return ftVal;
  XC("Number out of range!",
    "Parameter", iIndex, "Supplied", ftVal, "NotLesser", ftMin);
}
/* -- Try to get and check a valid number not < or > ----------------------- */
template<typename FloatType>
  requires StdIsFloat<FloatType>
static FloatType LuaUtilGetNumLG(lua_State*const lS, const int iIndex,
    const FloatType ftMin, const FloatType ftMax)
{ // Return number if valid and in range else break execution
  const FloatType ftVal = LuaUtilGetNum<FloatType>(lS, iIndex);
  if(ftVal >= ftMin && ftVal <= ftMax) return ftVal;
  XC("Number out of range!",
    "Parameter", iIndex, "Supplied",   ftVal,
    "NotLesser", ftMin,  "NotGreater", ftMax);
}
/* -- Try to get and check a valid number not < or >= ---------------------- */
template<typename FloatType>
  requires StdIsFloat<FloatType>
static FloatType LuaUtilGetNumLGE(lua_State*const lS, const int iIndex,
    const FloatType ftMin, const FloatType ftMax)
{ // Return number if valid and in range else break execution
  const FloatType ftVal = LuaUtilGetNum<FloatType>(lS, iIndex);
  if(ftVal >= ftMin && ftVal < ftMax) return ftVal;
  XC("Number out of range!",
    "Parameter", iIndex, "Supplied",        ftVal,
    "NotLesser", ftMin,  "NotGreaterEqual", ftMax);
}
/* -- Try to get and force a number value between -1 and 1 ----------------- */
template<typename FloatType>
  requires StdIsFloat<FloatType>
static FloatType LuaUtilGetNormal(lua_State*const lS, const int iIndex)
{ // Throw error if value not a number else return it clamped between -1 and 1.
  const lua_Number lnVal = LuaUtilGetNum<lua_Number>(lS, iIndex);
  return static_cast<FloatType>(fmod(lnVal, 1.0));
}
/* -- Try to get and check a valid integer --------------------------------- */
template<typename IntType>
  requires StdIsIntegral<IntType> || StdIsEnum<IntType>
static IntType LuaUtilGetInt(lua_State*const lS, const int iIndex)
{ // Throw error if value isn't an integer else return a cast of it
  LuaUtilAssert(lS, LuaUtilIsInteger(lS, iIndex), iIndex, "integer");
  return LuaUtilToInt<IntType>(lS, iIndex);
}
/* -- Try to get and check a valid integer not < --------------------------- */
template<typename IntType>
  requires StdIsIntegral<IntType> || StdIsEnum<IntType>
static IntType LuaUtilGetIntL(lua_State*const lS, const int iIndex,
  const IntType itMin)
{ // Return integer if valid and in range else break execution
  const IntType tVal = LuaUtilGetInt<IntType>(lS, iIndex);
  if(tVal >= itMin) return tVal;
  XC("Integer out of range!",
    "Parameter", iIndex, "Supplied", tVal, "NotLesser", itMin);
}
/* -- Try to get and check a valid integer range not < or > ---------------- */
template<typename IntType>
  requires StdIsIntegral<IntType> || StdIsEnum<IntType>
static IntType LuaUtilGetIntLG(lua_State*const lS, const int iIndex,
  const IntType itMin, const IntType itMax)
{ // Return integer if valid and in range else break execution
  const IntType tVal = LuaUtilGetInt<IntType>(lS, iIndex);
  if(tVal >= itMin && tVal <= itMax) return tVal;
  XC("Integer out of range!",
    "Parameter", iIndex, "Supplied", tVal,
    "NotLesser", itMin,   "NotGreater", itMax);
}
/* -- Try to get and check a valid integer range not < or > and = ^2 ------- */
template<typename IntType>
  requires StdIsIntegral<IntType> || StdIsEnum<IntType>
static IntType LuaUtilGetIntLGP2(lua_State*const lS, const int iIndex,
  const IntType itMin, const IntType itMax)
{ // Return integer if valid, in range and is ^2 else break execution
  const IntType tVal = LuaUtilGetIntLG(lS, iIndex, itMin, itMax);
  if(StdIntIsPOW2<IntType>(tVal)) return tVal;
  XC("Integer is not a power of two!", "Parameter", iIndex, "Supplied", tVal);
}
/* -- Try to get and check a valid integer range not < or >= --------------- */
template<typename IntType>
  requires StdIsIntegral<IntType> || StdIsEnum<IntType>
static IntType LuaUtilGetIntLGE(lua_State*const lS, const int iIndex,
  const IntType itMin, const IntType itMax)
{ // Return integer if valid and in range else break execution
  const IntType tVal = LuaUtilGetInt<IntType>(lS, iIndex);
  if(tVal >= itMin && tVal < itMax) return tVal;
  XC("Integer out of range!",
    "Parameter", iIndex, "Supplied", tVal,
    "NotLesser", itMin,   "NotGreaterEqual", itMax);
}
/* -- Try to get and check a valid integer range not <= or > --------------- */
template<typename IntType>
  requires StdIsIntegral<IntType> || StdIsEnum<IntType>
static IntType LuaUtilGetIntLEG(lua_State*const lS, const int iIndex,
  const IntType itMin, const IntType itMax)
{ // Return integer if valid and in range else break execution
  const IntType tVal = LuaUtilGetInt<IntType>(lS, iIndex);
  if(tVal > itMin && tVal <= itMax) return tVal;
  XC("Integer out of range!",
    "Parameter",      iIndex, "Supplied", tVal,
    "NotLesserEqual", itMin,   "NotGreater", itMax);
}
/* -- Try to get and check a 'Flags' parameter ----------------------------- */
template<class FloatType>
  requires StdIsClass<FloatType>
static const FloatType LuaUtilGetFlags(lua_State*const lS, const int iIndex,
  const FloatType ftMask)
{ // Return flags if valid and in range else break execution
  using ValueType = typename FloatType::ValueType;
  const FloatType ftFlags{ LuaUtilGetInt<ValueType>(lS, iIndex) };
  if(ftFlags.FlagIsZero() || ftFlags.FlagIsInMask(ftMask)) return ftFlags;
  XC("Flags out of range!",
    "Parameter", iIndex, "Supplied",  ftFlags.FlagGet(),
    "Mask",      ftMask.FlagGet());
}
/* -- Get a LuaUtilClass pointer from userdata ----------------------------- */
static LuaUtilClass *LuaUtilGetBasePtr(lua_State*const lS, const int iParam,
  const LuaIdent &liParent)
{ return reinterpret_cast<LuaUtilClass*>
    (luaL_checkudata(lS, iParam, liParent.LuaIdentCStr())); }
/* -- Get a LuaUtilClass pointer from userdata and throw if null ----------- */
static LuaUtilClass *LuaUtilGetCheckedBasePtr(lua_State*const lS,
  const int iParam, const LuaIdent &liParent)
{ // Get lua data class and if it is valid else lua data class not valid
  if(LuaUtilClass*const lucPtr = LuaUtilGetBasePtr(lS, iParam, liParent))
    return lucPtr;
  XC("Null class parameter!",
    "Parameter", iParam, "Type", liParent.LuaIdentStr());
}
/* -- Boolean return for LuaUtilGetCheckedBasePtr -------------------------- */
static bool LuaUtilIsClassDestroyed(lua_State*const lS, const int iParam,
  const LuaIdent &liParent)
    { return LuaUtilGetCheckedBasePtr(lS, iParam, liParent)->vpPtr
        == nullptr; }
/* -- Boolean return for LuaUtilGetCheckedBasePtr without parameter -------- */
static bool LuaUtilIsClassDestroyed(lua_State*const lS,
  const LuaIdent*const liParent)
    { return LuaUtilIsClassDestroyed(lS, 1, *liParent); }
/* -- Gets a pointer to any class ------------------------------------------ */
template<class ClassType, class ParentType = decltype(ClassType::cParent)>
  requires StdIsClass<ClassType>
static ClassType *LuaUtilGetClassPtr(lua_State*const lS, const int iParam,
  const ParentType*const ptParent)
{ // Get reference to class and return pointer if valid
  const LuaUtilClass &lcR = *LuaUtilGetCheckedBasePtr(lS, iParam, *ptParent);
  if(lcR.vpPtr) return reinterpret_cast<ClassType*>(lcR.vpPtr);
  // Actual class pointer has already been freed so error occured
  XC("Unallocated class parameter!",
    "Parameter", iParam, "Type", ptParent->LuaIdentStr());
}
/* -- Gets a reference to any class ---------------------------------------- */
template<class ClassType, class ParentType = decltype(ClassType::cParent)>
  requires StdIsClass<ClassType>
static ClassType &LuaUtilGetClassRef(lua_State*const lS, const int iParam,
  const ParentType*const ptParent)
{ return *LuaUtilGetClassPtr<ClassType>(lS, iParam, ptParent); }
/* -- Do clear and free the object ----------------------------------------- */
static void LuaUtilDoClassDestroy(LuaUtilClass*const lucPtr, auto*const ctPtr)
{ // Clear the pointer to the class
  lucPtr->vpPtr = nullptr;
  // Free the class if not set to locked (engine managed class)
  if(ctPtr->LockIsNotSet()) delete ctPtr;
}
/* -- Destroy an object ---------------------------------------------------- */
template<class ClassType> requires StdIsClass<ClassType>
  static void LuaUtilClassDestroy(lua_State*const lS,
    const LuaIdent*const liParent)
{ // Get userdata pointer from Lua and if the address is valid?
  if(LuaUtilClass*const lucPtr = LuaUtilGetBasePtr(lS, 1, *liParent))
    // Get address to the C++ class and if that is valid?
    if(ClassType*const ctPtr = reinterpret_cast<ClassType*>(lucPtr->vpPtr))
      // Clear the pointer to the C++ class and destroy it if not locked
      LuaUtilDoClassDestroy(lucPtr, ctPtr);
  // Don't throw any errors even if the structs are invalid as much as I
  // want to. However, the garbage collector routine '__gc' calls this
  // function and we don't want any problems when this happens.
}
/* -- Destroy an object with async protected callback check ---------------- */
template<class ClassType>
  requires StdIsClass<ClassType>
static void LuaUtilClassDestroyChecked(lua_State*const lS,
  const LuaIdent*const liParent)
{ // Get reference to collector pointer
  const LuaIdent &liRef = *liParent;
  // Get userdata pointer from Lua and if the address is valid?
  if(LuaUtilClass*const lucPtr = LuaUtilGetBasePtr(lS, 1, liRef))
  { // Get address to the C++ class and if that is valid?
    if(ClassType*const ctPtr = reinterpret_cast<ClassType*>(lucPtr->vpPtr))
    { // Throw error if destruction attempted in protected callback
      if(ctPtr->RefCtrIsEnabled())
        XC("Call not allowed in protected callback!",
          "Type", liRef.LuaIdentStr());
      // Clear the pointer to the C++ class and destroy it if not locked
      LuaUtilDoClassDestroy(lucPtr, ctPtr);
    } // Don't throw any errors even if the structs are invalid as much as I
  } // want to. However, the garbage collector routine '__gc' calls this
  // function and we don't want any problems when this happens.
}
/* -- Set metatable entry in userdata -------------------------------------- */
static int LuaUtilSetMetaTable(lua_State*const lS, const int iIndex)
  { return lua_setmetatable(lS, iIndex); }
/* -- Creates a new item for object ---------------------------------------- */
static LuaUtilClass *LuaUtilClassPrepNew(lua_State*const lS,
  const LuaIdent &liParent)
{ // Create userdata
  LuaUtilClass*const lucPtr =
    reinterpret_cast<LuaUtilClass*>(lua_newuserdata(lS, sizeof(LuaUtilClass)));
  // Get metadata table reference from collector class
  LuaUtilGetRef(lS, liParent.LuaIdentGetRef());
  // Done setting metamethods, set the table
  LuaUtilSetMetaTable(lS, -2);
  // Return pointer to new class allocated by Lua
  return lucPtr;
}
/* -- Takes ownership of an object ----------------------------------------- */
template<class ClassType> requires StdIsClass<ClassType>
  static ClassType *LuaUtilClassReuse(lua_State*const lS,
    const LuaIdent &liParent, ClassType*const ctPtr)
{ // Prepare a new object
  LuaUtilClass*const lucPtr = LuaUtilClassPrepNew(lS, liParent);
  // Assign object to lua so lua will be incharge of deleting it
  lucPtr->vpPtr = ctPtr;
  // Return pointer to new class allocated elseware
  return ctPtr;
}
/* -- Creates and allocates a pointer to a new class ----------------------- */
template<typename ClassType, class ParentType = decltype(ClassType::cParent)>
  requires StdIsClass<ClassType>
static ClassType *LuaUtilClassCreate(lua_State*const lS,
  const ParentType*const ptParent)
{ // Prepare a new object
  LuaUtilClass*const lucPtr = LuaUtilClassPrepNew(lS, *ptParent);
  // Allocate class and return it if succeeded return it
  if(void*const vpPtr = lucPtr->vpPtr = new (StdNoThrow)ClassType)
    return reinterpret_cast<ClassType*>(vpPtr);
  // Error occured so just throw exception
  XC("Failed to allocate memory for class structure!",
    "Type", ptParent->LuaIdentStr(), "Size", sizeof(ClassType));
}
/* -- Creates and allocates a pointer to a new class and returns reference - */
template<typename ClassType, class ParentType = decltype(ClassType::cParent)>
  requires StdIsClass<ClassType>
static ClassType &LuaUtilClassCreateRef(lua_State*const lS,
  const ParentType*const ptParent)
{ return *LuaUtilClassCreate<ClassType,ParentType>(lS, ptParent); }
/* -- Creates a pointer to a class that LUA CAN'T deallocate --------------- */
template<typename ClassType, class ParentType = decltype(ClassType::cParent)>
  requires StdIsClass<ClassType>
static ClassType *LuaUtilClassCreateStaticPtr(lua_State*const lS,
  const ParentType*const ptParent, ClassType*const ctPtr)
{ // Create userdata
  LuaUtilClass*const lucPtr =
    reinterpret_cast<LuaUtilClass*>(lua_newuserdata(lS, sizeof(LuaUtilClass)));
  // Get table data from collector reference and set it as class metatable
  LuaUtilGetRef(lS, ptParent->LuaIdentGetRef());
  LuaUtilSetMetaTable(lS, -2);
  // Set pointer to class
  lucPtr->vpPtr = reinterpret_cast<void*>(ctPtr);
  // Return pointer to memory
  return ctPtr;
}
/* -- Check that a class isn't locked (i.e. a built-in class) -------------- */
template<class ClassType>
  requires StdIsClass<ClassType>
static ClassType *LuaUtilGetUnlockedPtr[[maybe_unused]]
  (lua_State*const lS, const int iParam)
{ // Get pointer to class and return if isn't locked (a built-in class)
  ClassType*const ctPtr = LuaUtilGetClassPtr<ClassType>(lS, iParam);
  if(ctPtr->LockIsNotSet()) return ctPtr;
  // Throw error
  XC("Call not allowed on this class!", "Name", ctPtr->NameGet());
}
/* -- Garbage collection control ------------------------------------------- */
static int LuaUtilGC(lua_State*const lS, const int iCmd, const int iVal1,
  const int iVal2) { return lua_gc(lS, iCmd, iVal1, iVal2); }
static int LuaUtilGC(lua_State*const lS, const int iCmd, const int iVal)
  { return lua_gc(lS, iCmd, iVal); }
static int LuaUtilGC(lua_State*const lS, const int iCmd)
  { return lua_gc(lS, iCmd); }
/* -- Garbage collection control ------------------------------------------- */
static int LuaUtilGCParam(lua_State*const lS, const int iParam, const int iVal)
  { return LuaUtilGC(lS, LUA_GCPARAM, iParam, iVal); }
/* -- Garbage collection enable generational mode -------------------------- */
static bool LuaUtilGCSetGenerational(lua_State*const lS, const bool bEnabled)
  { return LuaUtilGC(lS, LUA_GCGEN, bEnabled ? 1 : 0) != 0 ? true : false; }
/* -- Garbage collection enable incremental mode --------------------------- */
static bool LuaUtilGCSetIncremental(lua_State*const lS, const bool bEnabled)
  { return LuaUtilGC(lS, LUA_GCINC, bEnabled ? 1 : 0) != 0 ? true : false; }
/* -- Stop garbage collection ---------------------------------------------- */
static int LuaUtilGCStop(lua_State*const lS)
  { return LuaUtilGC(lS, LUA_GCSTOP); }
/* -- Start garbage collection --------------------------------------------- */
static int LuaUtilGCStart(lua_State*const lS)
  { return LuaUtilGC(lS, LUA_GCRESTART); }
/* -- Execute garbage collection ------------------------------------------- */
static int LuaUtilGCRun(lua_State*const lS)
  { return LuaUtilGC(lS, LUA_GCCOLLECT); }
/* -- Returns if garbage collection is running ----------------------------- */
static bool LuaUtilGCRunning(lua_State*const lS)
  { return LuaUtilGC(lS, LUA_GCISRUNNING) != 0; }
/* -- Get memory usage ----------------------------------------------------- */
static size_t LuaUtilGetUsage(lua_State*const lS)
  { return static_cast<size_t>(LuaUtilGC(lS, LUA_GCCOUNT) +
      LuaUtilGC(lS, LUA_GCCOUNTB) / 1024) * 1024; }
/* -- Get GC minor multiplier ---------------------------------------------- */
static int LuaUtilGCMinorMul(lua_State*const lS, const int iValue = -1)
  { return LuaUtilGCParam(lS, LUA_GCPMINORMUL, iValue); }
/* -- Get GC major/minor multiplier ---------------------------------------- */
static int LuaUtilGCMajorMinor(lua_State*const lS, const int iValue = -1)
  { return LuaUtilGCParam(lS, LUA_GCPMAJORMINOR, iValue); }
/* -- Get GC minor/major multiplier ---------------------------------------- */
static int LuaUtilGCMinorMajor(lua_State*const lS, const int iValue = -1)
  { return LuaUtilGCParam(lS, LUA_GCPMINORMAJOR, iValue); }
/* -- Get GC pause setting ------------------------------------------------- */
static int LuaUtilGCPauseFactor(lua_State*const lS, const int iValue = -1)
  { return LuaUtilGCParam(lS, LUA_GCPPAUSE, iValue); }
/* -- Get GC step multiplier ----------------------------------------------- */
static int LuaUtilGCStepMul(lua_State*const lS, const int iValue = -1)
  { return LuaUtilGCParam(lS, LUA_GCPSTEPMUL, iValue); }
/* -- Get GC step size ----------------------------------------------------- */
static int LuaUtilGCStepSize(lua_State*const lS, const int iValue = -1)
  { return LuaUtilGCParam(lS, LUA_GCPSTEPSIZE, iValue); }
/* -- Full garbage collection while logging memory usage ------------------- */
static size_t LuaUtilGCCollect(lua_State*const lS)
{ // Get current usage, do a full garbage collect and return delta
  const size_t stUsage = LuaUtilGetUsage(lS);
  LuaUtilGCRun(lS);
  return stUsage - LuaUtilGetUsage(lS);
}
/* -- Standard in-sandbox call function (unmanaged) ------------------------ */
static void LuaUtilCallFuncEx(lua_State*const lS, const int iParams = 0,
  const int iReturns = 0)
{ lua_call(lS, iParams, iReturns); }
/* -- Standard in-sandbox call function with toggler ref ctr (unmanaged) --- */
static void LuaUtilCallFuncRefCtrEx(lua_State*const lS,
  RefCtrMaster<>*const rcmMaster, const int iParams = 0,
  const int iReturns = 0)
{ // Set a 'protect' flag and then unset it when leaving this scope
  const RefCtrSlave<> rcsProtect{ rcmMaster };
  // Do the call
  LuaUtilCallFuncEx(lS, iParams, iReturns);
}
/* -- Standard in-sandbox call function (unmanaged, no params) ------------- */
static void LuaUtilCallFunc(lua_State*const lS, const int iReturns = 0)
  { LuaUtilCallFuncEx(lS, 0, iReturns); }
/* -- Sandboxed call function (doesn't remove error handler) --------------- */
static int LuaUtilPCallEx(lua_State*const lS, const int iParams = 0,
  const int iReturns = 0, const int iHandler = 0)
{ return lua_pcall(lS, iParams, iReturns, iHandler); }
/* -- Sandboxed call function (removes error handler) ---------------------- */
static int LuaUtilPCallExSafe(lua_State*const lS, const int iParams = 0,
  const int iReturns = 0, const int iHandler = 0)
{ // Do protected call and get result
  const int iResult = LuaUtilPCallEx(lS, iParams, iReturns, iHandler);
  // Remove error handler from stack if handler specified
  if(iHandler) LuaUtilRmStack(lS, iHandler);
  // Return result
  return iResult;
}
/* -- Handle LuaUtilPCall result ------------------------------------------- */
static void LuaUtilPCallResultHandle(lua_State*const lS, const int iResult)
{ // Compare error code
  switch(iResult)
  { // No error
    case LUA_OK: return;
    // Run-time error
    case LUA_ERRRUN:
      XC(StrAppend("Runtime error! > ", LuaUtilGetAndPopStr(lS)));
    // Memory allocation error
    case LUA_ERRMEM:
      XC("Memory allocation error!", "Usage", LuaUtilGetUsage(lS));
    // Error + error in error handler
    case LUA_ERRERR: XC("Error in error handler!");
    // Unknown error
    default: XC("Unknown error!");
  }
}
/* -- Sandboxed call function that pops the handler ------------------------ */
static void LuaUtilPCallSafe(lua_State*const lS, const int iParams = 0,
  const int iReturns = 0, const int iHandler = 0)
{ LuaUtilPCallResultHandle(lS,
  LuaUtilPCallExSafe(lS, iParams, iReturns, iHandler));}
/* -- Sandboxed call function that doesn't pop the handler ----------------- */
static void LuaUtilPCall(lua_State*const lS, const int iParams = 0,
  const int iReturns = 0, const int iHandler = 0)
{ LuaUtilPCallResultHandle(lS,
  LuaUtilPCallEx(lS, iParams, iReturns, iHandler)); }
/* -- If string is blank then return other string -------------------------- */
static void LuaUtilIfBlank(lua_State*const lS)
{ // Get replacement string first
  size_t stEmp;
  const char*const cpEmp = LuaUtilGetLStr<char>(lS, 1, stEmp);
  // If the second parameter doesn't exist then return the empty string
  if(LuaUtilIsNoneOrNil(lS, 2)) { LuaUtilPushLStr(lS, cpEmp, stEmp); return; }
  // Second parameter is valid, but return it if LUA says it is empty
  size_t stStr;
  const char*const cpStr = LuaUtilGetLStr<char>(lS, 2, stStr);
  if(!stStr) { LuaUtilPushLStr(lS, cpEmp, stEmp); return; }
  // It isn't empty so return original string
  LuaUtilPushLStr(lS, cpStr, stStr);
}
/* -- Convert string string map to lua table and put it on stack ----------- */
static void LuaUtilToTableEx(lua_State*const lS, const auto &mctData)
{ // Create the table, we're creating non-indexed key/value pairs
  LuaUtilPushTable(lS, 0, mctData.size());
  // For each table item
  for(auto &mctPair : mctData)
  { // Push value and key name
    LuaUtilPushStr(lS, mctPair.second);
    LuaUtilSetField(lS, -2, mctPair.first.data());
  }
}
/* -- Push the specified string at the specified index --------------------- */
template<typename StrType>
  requires StdIsString<StrType>
static void LuaUtilSetTableIdxStr(lua_State*const lS,
  const int iTableId, const lua_Integer liIndex, const StrType &strValue)
{ // Push at the specified index, the specified string and set it to the table
  LuaUtilPushInt(lS, liIndex);
  LuaUtilPushVar(lS, strValue);
  LuaUtilSetRaw(lS, iTableId);
}
/* -- Push the specified integer at the specified index -------------------- */
static void LuaUtilSetTableIdxInt(lua_State*const lS,
  const int iTableId, const lua_Integer liIndex, const auto itValue)
{ // Push at the specified index, the specified value and set it to the table
  LuaUtilPushInt(lS, liIndex);
  LuaUtilPushInt(lS, static_cast<lua_Integer>(itValue));
  LuaUtilSetRaw(lS, iTableId);
}
/* -- Convert a directory info object and put it on stack ------------------ */
static void LuaUtilToTable(lua_State*const lS, const DirEntMap &demList)
{ // Create the table, we're creating a indexed/value array
  LuaUtilPushTable(lS, demList.size());
  // Entry id
  lua_Integer liId = 0;
  // For each table item
  for(const DirEntMapPair &dempRef : demList)
  { // Push table index
    LuaUtilPushInt(lS, ++liId);
    // Create the sub for file info, we're creating a indexed/value array
    LuaUtilPushTable(lS, 7);
    // Push file parts
    LuaUtilSetTableIdxStr(lS, -3, 1, dempRef.first);               // File name
    LuaUtilSetTableIdxInt(lS, -3, 2, dempRef.second.Id());         // Order id
    LuaUtilSetTableIdxInt(lS, -3, 3, dempRef.second.Size());       // Size
    LuaUtilSetTableIdxInt(lS, -3, 4, dempRef.second.Created());    // Created
    LuaUtilSetTableIdxInt(lS, -3, 5, dempRef.second.Written());    // Updated
    LuaUtilSetTableIdxInt(lS, -3, 6, dempRef.second.Accessed());   // Accessed
    LuaUtilSetTableIdxInt(lS, -3, 7, dempRef.second.Attributes()); // Attrs
    // Push file data table
    LuaUtilSetRaw(lS, -3);
  }
}
/* -- Convert string vector to lua table and put it on stack --------------- */
static void LuaUtilToTable(lua_State*const lS, const auto &ltData)
{ // Create the table, we're creating a indexed/value array and return if empty
  LuaUtilPushTable(lS, ltData.size());
  if(ltData.empty()) return;
  // Id number for array index
  lua_Integer iIndex = 0;
  // For each table item
  for(const auto &aItem : ltData)
    LuaUtilSetTableIdxStr(lS, -3, ++iIndex, aItem);
}
/* -- Explode LUA string into table ---------------------------------------- */
static void LuaUtilExplode(lua_State*const lS)
{ // Check parameters
  const StdStringView strvStr{ LuaUtilGetCppStr(lS, 1) },
                      strvSep{ LuaUtilGetCppStr(lS, 2) };
  // Create empty table if string invalid
  if(strvStr.empty() || strvSep.empty()) { LuaUtilPushTable(lS); return; }
  // Else convert whats in the string
  LuaUtilToTable(lS, TokenStrView{ strvStr, strvSep });
}
/* -- Explode LUA string into table ---------------------------------------- */
static void LuaUtilExplodeEx(lua_State*const lS)
{ // Check parameters
  const StdStringView strvStr{ LuaUtilGetCppStr(lS, 1) },
                      strvSep{ LuaUtilGetCppStr(lS, 2) };
  const size_t stMax = LuaUtilGetInt<size_t>(lS, 3);
  // Create empty table if string invalid
  if(strvStr.empty() || strvSep.empty() || !stMax) return LuaUtilPushTable(lS);
  // Else convert whats in the string
  LuaUtilToTable(lS, TokenStrView{ strvStr, strvSep, stMax });
}
/* -- Process initial implosion a table ------------------------------------ */
static lua_Integer LuaUtilImplodePrepare(lua_State*const lS,
  const int iMaxParams)
{ // Must have this many parameters
  LuaUtilCheckParams(lS, iMaxParams);
  // Check table and get its size
  LuaUtilCheckTable(lS, 1);
  // Get size of table clamped since lua_rawlen returns unsigned and the
  // lua_rawgeti parameter is signed. Compare the result...
  switch(const lua_Integer liLen =
    UtilIntOrMax<lua_Integer>(LuaUtilGetSize(lS, 1)))
  { // No entries? Just check the separator for consistency and push blank
    case 0: LuaUtilCheckStr(lS, 2);
            LuaUtilPushStr(lS, cCommon->CommonBlank());
            break;
    // One entry? Just check the separator and push the first item
    case 1: LuaUtilCheckStr(lS, 2);
            LuaUtilGetRefEx(lS);
            break;
    // More than one entry? Caller must process this;
    default: return liLen;
  } // We handled it
  return 0;
}
/* -- Pushes an item from the specified table onto the stack --------------- */
static void LuaUtilImplodeItem(lua_State*const lS, const int iParam,
  const lua_Integer liIndex, StdString &strOutput, const char *cpStr,
  size_t stStr)
{ // Add separator to string
  strOutput.append(cpStr, stStr);
  // Get item from table
  LuaUtilGetRefEx(lS, 1, liIndex);
  // Get the string from Lua stack and save the length
  cpStr = LuaUtilToLString(lS, iParam, stStr);
  // Append to supplied string
  strOutput.append(cpStr, stStr);
  // Remove item from stack
  LuaUtilRmStack(lS);
}
/* -- Implode LUA table into string ---------------------------------------- */
static void LuaUtilImplode(lua_State*const lS)
{ // Prepare table for implosion and return if more than 1 entry in table?
  if(const lua_Integer liLen = LuaUtilImplodePrepare(lS, 2))
  { // Get separator
    size_t stSep;
    const char*const cpSep = LuaUtilGetLStr<char>(lS, 2, stSep);
    // Write first item
    LuaUtilGetRefEx(lS);
    StdString strOutput{ LuaUtilToCppString(lS) };
    LuaUtilRmStack(lS);
    // Iterate through rest of table and implode the items
    for(lua_Integer liI = 2; liI <= liLen; ++liI)
      LuaUtilImplodeItem(lS, 3, liI, strOutput, cpSep, stSep);
    // Return string
    LuaUtilPushStr(lS, strOutput);
  }
}
/* -- Implode LUA table into human readable string ------------------------- */
static void LuaUtilImplodeEx(lua_State*const lS)
{ // Prepare table for implosion and return if more than 1 entry in table?
  if(const lua_Integer liLen = LuaUtilImplodePrepare(lS, 3))
  { // Get and check separators
    size_t stSep, stSep2;
    const char
      *const cpSep = LuaUtilGetLStr<char>(lS, 2, stSep),
      *const cpSep2 = LuaUtilGetLStr<char>(lS, 3, stSep2);
    // Write first item
    LuaUtilGetRefEx(lS);
    StdString strOutput{ LuaUtilToCppString(lS) };
    LuaUtilRmStack(lS);
    // Iterator through rest of table except for last entry
    for(lua_Integer liI = 2; liI < liLen; ++liI)
      LuaUtilImplodeItem(lS, 4, liI, strOutput, cpSep, stSep);
    // If there was more than one item? StrImplode the last item
    if(liLen > 1) LuaUtilImplodeItem(lS, 4, liLen, strOutput, cpSep2, stSep2);
    // Return string
    LuaUtilPushStr(lS, strOutput);
  }
}
/* -- Enumerate number of items in a table (non-indexed) ------------------- */
static lua_Unsigned LuaUtilGetKeyValTableSize(lua_State*const lS)
{ // Check that we have a table of strings
  LuaUtilCheckTable(lS, 1);
  // Number of indexed items in table
  const lua_Unsigned uIndexedCount = LuaUtilGetSize(lS, 1);
  // Number of items in table
  lua_Unsigned uCount = 0;
  // Until there are no more items
  for(LuaUtilPushNil(lS); lua_next(lS, -2); LuaUtilRmStack(lS)) ++uCount;
  // Remove key
  LuaUtilRmStack(lS);
  // Return count of key/value pairs in table
  return uCount - uIndexedCount;
}
/* -- Clear a table of key pairs ------------------------------------------- */
static void LuaUtilClearObject(lua_State*const lS, const int iIndex)
{ // Create a new table which will hold keys to delete
  lua_newtable(lS);
  const int iKIndex = LuaUtilStackSize(lS);
  int iKCount = 0;
  // First key pair for lua_next()
  LuaUtilPushNil(lS);
  // For each key pair
  while(lua_next(lS, iIndex))
  { // Copy the key name into the array
    LuaUtilCopyValue(lS, -2);
    lua_rawseti(lS, iKIndex, ++iKCount);
    lua_pop(lS, 1);
  } // For each key in the table
  for(;iKCount > 0; --iKCount)
  { // Nil out each collected key using rawset (avoids metamethods)
    LuaUtilGetRefEx(lS, iKIndex, iKCount);
    LuaUtilPushNil(lS);
    LuaUtilSetRaw(lS, iIndex);
  } // Remove keys table we created
  lua_pop(lS, 1);
}
/* -- Clear a table of key pairs with check -------------------------------- */
static void LuaUtilClearObjectSafe(lua_State*const lS, const int iIndex)
  { LuaUtilCheckTable(lS, iIndex); LuaUtilClearObject(lS, iIndex); }
/* -- Clear multiple tables of key pairs with check ------------------------ */
static void LuaUtilClearObjects(lua_State*const lS, int iStart)
  { for(const int iEnd = LuaUtilStackSize(lS); iStart <= iEnd; ++iStart)
      LuaUtilClearObjectSafe(lS, iStart); }
/* -- Clear a table of indicies -------------------------------------------- */
static void LuaUtilClearArray(lua_State*const lS, const int iIndex)
{ // If table array has size (clamp to lua_Integer to be future proof).
  if(lua_Integer liSize =
    UtilIntOrMax<lua_Integer>(LuaUtilGetSize(lS, iIndex))) do
  { // Push a nil and set it to the table index
    LuaUtilPushNil(lS);
    lua_rawseti(lS, iIndex, liSize);
  } // Until all indicies removed
  while(--liSize > 0);
}
/* -- Clear a table of indices with check ---------------------------------- */
static void LuaUtilClearArraySafe(lua_State*const lS, const int iIndex)
  { LuaUtilCheckTable(lS, iIndex); LuaUtilClearArray(lS, iIndex); }
/* -- Clear multiple tables of indicies with check ------------------------- */
static void LuaUtilClearArrays(lua_State*const lS, int iStart)
  { for(const int iEnd = LuaUtilStackSize(lS); iStart <= iEnd; ++iStart)
      LuaUtilClearArraySafe(lS, iStart); }
/* -- Clear a table of both key pairs and indicies ------------------------- */
static void LuaUtilClearTable(lua_State*const lS, const int iIndex)
  { LuaUtilClearArray(lS, iIndex); LuaUtilClearObject(lS, iIndex); }
/* -- Clear a table of both key pairs and indicies with check -------------- */
static void LuaUtilClearTableSafe(lua_State*const lS, const int iIndex)
  { LuaUtilCheckTable(lS, iIndex); LuaUtilClearTable(lS, iIndex); }
/* -- Clear multiple tables of both key pairs and indicies with check ------ */
static void LuaUtilClearTables(lua_State*const lS, int iStart)
  { for(const int iEnd = LuaUtilStackSize(lS); iStart <= iEnd; ++iStart)
      LuaUtilClearTableSafe(lS, iStart); }
/* -- Replace text with values from specified LUA table -------------------- */
static StdString LuaUtilReplaceMulti(lua_State*const lS,
  const StdStringView &strvWhat)
{ // Return if source string is empty?
  if(strvWhat.empty()) return {};
  // Table for replacements
  using StrViewPair = StdPair<const StdStringView, const StdStringView>;
  using StrViewPairList = StdList<StrViewPair>;
  StrViewPairList lList;
  // Prepare table for implosion and return if more than 1 entry in table?
  if(const lua_Unsigned luiLen = LuaUtilGetSize(lS, 2))
  { // Must have even number of parameters
    if(luiLen % 2) XC("Array size invalid!", "Size", luiLen);
    // Iterate through rest of table and implode the items
    for(lua_Integer liIndex = 1,
                    liMax = static_cast<lua_Integer>(
                      UtilIntWillOverflow<lua_Integer>(luiLen) ?
                        StdLimits<lua_Integer>::max() - 1 : luiLen);
                    liIndex <= liMax;
                    liIndex += 2)
    { // Get key from table
      LuaUtilGetRefEx(lS, 2, liIndex);
      const StdStringView strvKey{ LuaUtilToCppString(lS) };
      LuaUtilRmStack(lS);
      // Get value from table
      LuaUtilGetRefEx(lS, 2, liIndex + 1);
      lList.push_back({ strvKey, LuaUtilToCppString(lS) });
      LuaUtilRmStack(lS);
    }
  } // Until there are no more items, add value if key is a string
  else
  { // Push key/values into replacement table
    for(LuaUtilPushNil(lS); lua_next(lS, -2); LuaUtilRmStack(lS))
      if(LuaUtilIsString(lS, -1))
        lList.push_back({ LuaUtilToCppString(lS, -2),
                          LuaUtilToCppString(lS) });
    // Remove string parameter
    LuaUtilRmStack(lS);
  } // Return nothing if empty
  if(lList.empty()) return {};
  // Do the replacement and return the string
  return StrReplaceEx(strvWhat, lList);
}
/* -- Convert map tp table ------------------------------------------------- */
template<class MapType>
  static void LuaUtilToTable(lua_State*const lS, const MapType &mtRef,
    auto &&fcbFunc)
{ // Create the table, we're creating non-indexed key/value pairs
  LuaUtilPushTable(lS, 0, mtRef.size());
  // Enumerate each table item
  using ValueType = typename MapType::value_type;
  for(const ValueType &vtRef : mtRef)
  { // Push value and key name
    fcbFunc(vtRef.second);
    LuaUtilSetField(lS, -2, vtRef.first.data());
  }
}
/* -- Convert string/uint map to table ------------------------------------- */
static void LuaUtilToTable(lua_State*const lS, const StrUIntMap &suimRef)
  { LuaUtilToTable(lS, suimRef, [lS](const unsigned uValue)
      { LuaUtilPushInt(lS, uValue); }); }
/* -- Convert string/string map to table ----------------------------------- */
static void LuaUtilToTable(lua_State*const lS, const StrNCStrMap &sncsmMap)
  { LuaUtilToTable(lS, sncsmMap, [lS](const StdString &strValue)
      { LuaUtilPushStr(lS, strValue); }); }
/* -- Convert varlist to lua table and put it on stack --------------------- */
template<class ListType>
  static ListType LuaUtilToVector(lua_State*const lS, const int iArg,
    auto &&fcbFunc)
{ // Create the table, we're creating non-indexed key/value pairs
  LuaUtilCheckTable(lS, iArg);
  // Get maximums
  const size_t stMax = UtilIntOrMax<size_t>(LuaUtilGetSize(lS, iArg));
  const lua_Integer liMax = static_cast<lua_Integer>(stMax) + 1;
  // Preallocate the table
  StdReserved<ListType> vtArray{ stMax };
  // Walk the array
  for(lua_Integer liI = 1; liI < liMax; ++liI)
  { // Get item from table
    LuaUtilPushInt(lS, liI);
    lua_gettable(lS, -2);
    // Get the string from Lua stack and save the length
    vtArray.push_back(fcbFunc());
    // Remove item from stack
    LuaUtilRmStack(lS);
  } // Recover memory
  vtArray.shrink_to_fit();
  // Return the container
  return vtArray;
}
/* -- Convert a table of numbers to vector --------------------------------- */
template<class ListType>
  static ListType LuaUtilToNumVector(lua_State*const lS, const int iArg)
{ using VecValType = typename ListType::value_type;
  return LuaUtilToVector<ListType>(lS, iArg,
    [lS](){ return LuaUtilGetNum<VecValType>(lS, -1); }); }
/* -- Convert a table of integers to vector -------------------------------- */
template<class ListType>
  static ListType LuaUtilToIntVector(lua_State*const lS, const int iArg)
{ using VecValType = typename ListType::value_type;
  return LuaUtilToVector<ListType>(lS, iArg,
    [lS](){ return LuaUtilGetInt<VecValType>(lS, -1); }); }
/* -- Get and set a global variable ---------------------------------------- */
static void LuaUtilGetGlobal(lua_State*const lS, const char*const cpKey)
  { lua_getglobal(lS, cpKey); }
static void LuaUtilSetGlobal(lua_State*const lS, const char*const cpKey)
  { lua_setglobal(lS, cpKey); }
/* -- Returns t[k] --------------------------------------------------------- */
static void LuaUtilGetField(lua_State*const lS, const int iIndex,
  const char*const cpKey)
{ lua_getfield(lS, iIndex, cpKey); }
/* -- Initialise lua and clib random number generators --------------------- */
static void LuaUtilInitRNGSeed(lua_State*const lS, const lua_Integer liSeed)
{ // Make C-Lib use the specified seed
  StdSRand(static_cast<unsigned>(liSeed));
  // Get 'math' table
  LuaUtilGetGlobal(lS, "math");
  // Get pointer to function
  LuaUtilGetField(lS, -1, "randomseed");
  // Push a random seed
  LuaUtilPushInt(lS, liSeed);
  // Calls randomseed(ullSeed)
  LuaUtilCallFuncEx(lS, 1);
  // Removes the table 'math'
  LuaUtilRmStack(lS);
}
/* -- Return true if lua stack can take specified more items --------------- */
static bool LuaUtilIsStackAvail(lua_State*const lS, const int iCount)
  { return lua_checkstack(lS, iCount); }
/* -- Return true if lua stack can take specified more items (diff type) --- */
static bool LuaUtilIsStackAvail(lua_State*const lS, const auto itCount)
  { return UtilIntWillOverflow<int>(itCount) ? false :
      LuaUtilIsStackAvail(lS, static_cast<int>(itCount)); }
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
