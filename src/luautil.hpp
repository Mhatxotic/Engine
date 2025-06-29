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
/* ------------------------------------------------------------------------- */
using namespace ICommon::P;            using namespace IDir::P;
using namespace IError::P;             using namespace ILuaIdent::P;
using namespace IMemory::P;            using namespace IStd::P;
using namespace IString::P;            using namespace IToken::P;
using namespace IUtil::P;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* -- Variables ------------------------------------------------------------ */
static unsigned int uiLuaPaused = 0;   // Times Lua paused before handling it
static bool bDebugLocals = true;       // Specifies to show locals on stack
/* -- Utility type defs ---------------------------------------------------- */
struct LuaUtilClass { void *vpPtr; };  // Holds a pointer to a class
/* -- Prune stack ---------------------------------------------------------- */
static void LuaUtilPruneStack(lua_State*const lS, const int iParam)
  { lua_settop(lS, iParam); }
/* -- Return items in stack ------------------------------------------------ */
static int LuaUtilStackSize(lua_State*const lS) { return lua_gettop(lS); }
/* -- Get length of a table ------------------------------------------------ */
static lua_Unsigned LuaUtilGetSize(lua_State*const lS, const int iParam)
  { return lua_rawlen(lS, iParam); }
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
template<typename PtrType, typename PtrTypePtr=PtrType*>
  static PtrTypePtr LuaUtilGetSimplePtr(lua_State*const lS, const int iParam)
{ // Break execution if not userdata else return pointer as requested cast
  static_assert(!is_pointer_v<PtrType>, "Don't use pointers!");
  if(!LuaUtilIsUserData(lS, iParam)) XC("Not userdata!", "Param", iParam);
  void*const vpPtr = lua_touserdata(lS, iParam);
  return reinterpret_cast<PtrTypePtr>(vpPtr);
}
/* -- Get string and size of it -------------------------------------------- */
static const char *LuaUtilToLString(lua_State*const lS, const int iParam,
  size_t &stSize) { return lua_tolstring(lS, iParam, &stSize); }
/* -- Get and return a C++ string without checking it ---------------------- */
static const string LuaUtilToCppString(lua_State*const lS, const int iParam=-1)
{ // Storage for string length. Do not optimise this because I am not sure
  // what the standard direction is for evaluating expression. Left-to-right
  // or right-to-left, so I will just store the string point first to be safe.
  size_t stLength;
  const char*const cpStr = LuaUtilToLString(lS, iParam, stLength);
  return { cpStr, stLength };
}
/* -- Get a number from the stack ------------------------------------------ */
template<typename NumType=lua_Number>
  static NumType LuaUtilToNum(lua_State*const lS, const int iIndex)
{ // Return requested number
  static_assert(is_floating_point_v<NumType>, "Not floating point!");
  return static_cast<NumType>(lua_tonumber(lS, iIndex));
}
/* -- Get an integer from the stack ---------------------------------------- */
template<typename IntType=lua_Integer>
  static IntType LuaUtilToInt(lua_State*const lS, const int iIndex)
{ // Return requested integer
  static_assert(is_integral_v<IntType> || is_enum_v<IntType>, "Not integral!");
  return static_cast<IntType>(lua_tointeger(lS, iIndex));
}
/* -- Get an boolean from the stack ---------------------------------------- */
template<typename IntType=bool>
  static IntType LuaUtilToBool(lua_State*const lS, const int iIndex)
{ // Return bool cast to requested type
  static_assert(is_integral_v<IntType> || is_enum_v<IntType>, "Not integral!");
  return static_cast<IntType>(lua_toboolean(lS, iIndex));
}
/* -- Get an pointer from the stack ---------------------------------------- */
template<typename PtrType=void, typename PtrTypeConst=const PtrType*>
  static PtrTypeConst LuaUtilToPtr(lua_State*const lS, const int iIndex)
{ // Return a pointer to requested type
  static_assert(!is_pointer_v<PtrType>, "Don't use pointers!");
  return reinterpret_cast<PtrTypeConst>(lua_topointer(lS, iIndex));
}
/* -- Get human readable name of specified type id ------------------------- */
static const char *LuaUtilGetType(lua_State*const lS, const int iIndex)
  { return lua_typename(lS, lua_type(lS, iIndex)); }
/* -- Push a string onto the stack ----------------------------------------- */
template<typename StrType>
  static void LuaUtilPushCStr(lua_State*const lS, const StrType stValue)
{ // Check that specified template argument is a pointer
  static_assert(is_pointer_v<StrType>, "Use a pointer!");
  static_assert(sizeof(stValue[0])==sizeof(uint8_t), "Must be 1 byte array!");
  lua_pushstring(lS, reinterpret_cast<const char*>(stValue));
}
/* -- Return raw access without meta methods ------------------------------- */
static void LuaUtilGetRaw(lua_State*const lS, const int iIndex=1)
  { lua_rawget(lS, iIndex); }
/* -- Simple class to save and restore stack ------------------------------- */
class LuaStackSaver                    // Lua stack saver class
{ /* -- Private variables -------------------------------------------------- */
  const int        iTop;               // Current stack position
  lua_State*const  lState;             // State to use
  /* -- Return stack position -------------------------------------- */ public:
  int Value(void) const { return iTop; }
  /* -- Restore stack position --------------------------------------------- */
  void Restore(void) const { LuaUtilPruneStack(lState, Value()); }
  /* -- Constructor -------------------------------------------------------- */
  explicit LuaStackSaver(lua_State*const lS) :
    iTop(LuaUtilStackSize(lS)), lState(lS) { }
  /* -- Destructor --------------------------------------------------------- */
  ~LuaStackSaver(void) { Restore(); }
};/* ----------------------------------------------------------------------- */
/* -- Remove item from stack ----------------------------------------------- */
static void LuaUtilRmStack(lua_State*const lS, const int iParam=-1)
  { lua_remove(lS, iParam); }
/* -- Push a literal string onto the stack --------------------------------- */
template<typename PtrType, typename IntType>
  static void LuaUtilPushLStr(lua_State*const lS, const PtrType ptValue,
    const IntType itSize)
{ // Check that specified template arguments are valid
  static_assert(is_pointer_v<PtrType>, "Use a pointer!");
  static_assert(is_integral_v<IntType> || is_enum_v<IntType>, "Not integral!");
  lua_pushlstring(lS, reinterpret_cast<const char*>(ptValue),
                      static_cast<size_t>(itSize));
}
/* -- Push a C++ string onto the stack ------------------------------------- */
static void LuaUtilPushStr(lua_State*const lS, const string &strStr)
  { LuaUtilPushLStr(lS, strStr.data(), strStr.length()); }
/* -- Get metatable entry from userdata ------------------------------------ */
static int LuaUtilGetMetaTable(lua_State*const lS, const int iIndex)
  { return lua_getmetatable(lS, iIndex); }
/* -- Return type of item in stack ----------------------------------------- */
static const string LuaUtilGetStackType(lua_State*const lS, const int iIndex)
{ // What type of variable?
  switch(lua_type(lS, iIndex))
  { // Nil?
    case LUA_TNIL: return "nil"; break;
    // A number?
    case LUA_TNUMBER:
    { // If not actually an integer? Write as normal floating-point number
      if(!LuaUtilIsInteger(lS, iIndex))
        return StrFromNum(LuaUtilToNum(lS, iIndex));
      // Get actual integer value and return it and it's hex value
      const lua_Integer liValue = LuaUtilToInt(lS, iIndex);
      return StrFormat("$ [0x$$]", liValue, hex, liValue);
    } // A boolean?
    case LUA_TBOOLEAN: return StrFromBoolTF(LuaUtilToBool(lS, iIndex));
    // A string?
    case LUA_TSTRING:
    { // Get value of string and return value with size
      const string strVal{ LuaUtilToCppString(lS, iIndex) };
      return StrFormat("[$] \"$\"", strVal.length(), strVal);
    } // A table?
    case LUA_TTABLE: return StrFormat("<table:$>[$]",
      LuaUtilToPtr(lS, iIndex), LuaUtilGetSize(lS, iIndex));
    // Userdata?
    case LUA_TUSERDATA:
    { // Save stack count and restore it when leaving scope
      const LuaStackSaver lssUserData{ lS };
      // Get metadata and return if not metadata (generic userdata)
      LuaUtilGetMetaTable(lS, -1);
      if(!LuaUtilIsTable(lS, -1))
        return StrFormat("<userdata:$>", LuaUtilToPtr(lS, iIndex));
      // Read internal engine name and return generic data if not a string
      LuaUtilPushStr(lS, cCommon->CommonLuaName());
      LuaUtilGetRaw(lS, -2);
      return StrFormat("<$:$>",
        LuaUtilIsString(lS, -1) ? LuaUtilToCppString(lS, -1) : "Unknown",
        LuaUtilToPtr(lS, iIndex));
    } // Who knows? Function? Userdata?
    default: return StrFormat("<$:$>",
      LuaUtilGetType(lS, iIndex), LuaUtilToPtr(lS, iIndex));
  }
}
/* -- Return status of item in stack --------------------------------------- */
static const string LuaUtilGetStackTokens(lua_State*const lS, const int iIndex)
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
static const string LuaUtilGetVarStack(lua_State*const lS)
{ // If there are variables in the stack?
  if(const int iCount = LuaUtilStackSize(lS))
  { // String to return
    ostringstream osS;
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
template<typename IdxIntType=int, typename KeyIntType=int>
  static void LuaUtilPushTable(lua_State*const lS,
    const IdxIntType iitIndexes=0, const KeyIntType kitKeys=0)
{ // Create a table of the specified size
  static_assert(is_integral_v<IdxIntType> || is_enum_v<IdxIntType>,
    "Indexes count Not integral!");
  static_assert(is_integral_v<KeyIntType> || is_enum_v<KeyIntType>,
    "Key pairs count Not integral!");
  lua_createtable(lS, UtilIntOrMax<int>(iitIndexes),
                      UtilIntOrMax<int>(kitKeys));
}
/* -- Push a nil onto the stack -------------------------------------------- */
static void LuaUtilPushNil(lua_State*const lS) { lua_pushnil(lS); }
/* -- Push specified integral as boolean on to the stack ------------------- */
template<typename IntType>
  static void LuaUtilPushBool(lua_State*const lS, const IntType itValue)
{ // Check that it's a valid integral number first
  static_assert(is_integral_v<IntType> || is_enum_v<IntType>, "Not integral!");
  lua_pushboolean(lS, static_cast<bool>(itValue));
}
/* -- Push a number onto the stack ----------------------------------------- */
template<typename NumType>
  static void LuaUtilPushNum(lua_State*const lS, const NumType ntValue)
{ // Check that it's a valid floating point number first
  static_assert(is_floating_point_v<NumType>, "Not floating point!");
  lua_pushnumber(lS, static_cast<lua_Number>(ntValue));
}
/* -- Push an integer onto the stack --------------------------------------- */
template<typename IntType>
  static void LuaUtilPushInt(lua_State*const lS, const IntType itValue)
{ // Check that it's a valid integral number first
  static_assert(is_integral_v<IntType> || is_enum_v<IntType>, "Not integral!");
  lua_pushinteger(lS, static_cast<lua_Integer>(itValue));
}
/* -- Push a memory block onto the stack as a string ----------------------- */
static void LuaUtilPushMem(lua_State*const lS, const MemConst &mcSrc)
  { LuaUtilPushLStr(lS, mcSrc.MemPtr<char>(), mcSrc.MemSize()); }
/* -- Push a C++ string view onto the stack -------------------------------- */
static void LuaUtilPushStrView(lua_State*const lS, const string_view &strvStr)
  { LuaUtilPushLStr(lS, strvStr.data(), strvStr.length()); }
/* -- Push a pointer ------------------------------------------------------- */
static void LuaUtilPushPtr(lua_State*const lS, void*const vpPtr)
  { lua_pushlightuserdata(lS, vpPtr); }
/* -- Push multiple values of different types (use in ll*.hpp sources) ----- */
static void LuaUtilPushVar(lua_State*const) { }
template<typename ...VarArgs, typename AnyType>
  static void LuaUtilPushVar(lua_State*const lS, const AnyType &atVal,
    const VarArgs &...vaVars)
{ // Type is STL string?
  if constexpr(is_same_v<AnyType, string>) LuaUtilPushStr(lS, atVal);
  // Type is STL string_view?
  else if constexpr(is_same_v<AnyType, string_view>)
    LuaUtilPushStrView(lS, atVal);
  // Type is boolean?
  else if constexpr(is_same_v<AnyType, bool>) LuaUtilPushBool(lS, atVal);
  // Type is any pointer type (assuming char*, don't send anything else)
  else if constexpr(is_pointer_v<AnyType>) LuaUtilPushCStr(lS, atVal);
  // Type is enum, int, long, short or int64?
  else if constexpr(is_integral_v<AnyType> || is_enum_v<AnyType>)
    LuaUtilPushInt(lS, atVal);
  // Type is float or double?
  else if constexpr(is_floating_point_v<AnyType>) LuaUtilPushNum(lS, atVal);
  // Just push nil otherwise
  else LuaUtilPushNil(lS);
  // Shift to next variable
  LuaUtilPushVar(lS, vaVars...);
}
/* -- Throw error ---------------------------------------------------------- */
static void LuaUtilErrThrow(lua_State*const lS) { lua_error(lS); }
/* -- Push C-String on stack and throw ------------------------------------- */
static void LuaUtilPushErr(lua_State*const lS,
  const char*const cpReason)
{ LuaUtilPushCStr(lS, cpReason); LuaUtilErrThrow(lS); }
/* -- Get and pop string on top -------------------------------------------- */
static const string LuaUtilGetAndPopStr(lua_State*const lS)
{ // If there is nothing on the stack then return a generic error
  if(LuaUtilIsNone(lS, -1)) return "Error signalled with no reason";
  // Not have a string on stack? Set embedded error!
  if(!LuaUtilIsString(lS, -1))
    return StrFormat("Error signalled with invalid '$' reason",
      LuaUtilGetType(lS, -1));
  // Get error string
  const string strError{ LuaUtilToCppString(lS) };
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
static void LuaUtilSetRaw(lua_State*const lS, const int iIndex=1)
  { lua_rawset(lS, iIndex); }
/* -- Return if reference is valid ----------------------------------------- */
static bool LuaUtilIsRefValid(const int iReference)
  { return iReference != LUA_REFNIL; }
/* -- Return if reference is not valid ------------------------------------- */
static bool LuaUtilIsNotRefValid(const int iReference)
  { return !LuaUtilIsRefValid(iReference); }
/* -- Return reference ----------------------------------------------------- */
static void LuaUtilGetRefEx(lua_State*const lS, const int iTable=1,
  const lua_Integer liIndex=1) { lua_rawgeti(lS, iTable, liIndex); }
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
static const string LuaUtilStack(lua_State*const lST)
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
  typedef list<Debug> LuaStack;
  typedef LuaStack::reverse_iterator LuaStackRevIt;
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
  ostringstream osS;
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
        << static_cast<unsigned int>(ldData.nparams) << ';'
        << static_cast<unsigned int>(ldData.nups) << ')';
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
  const string strError{ LuaUtilGetAndPopStr(lS) };
  XC(StrAppend(strError, LuaUtilStack(lS)));
}
/* -- Generic error handler ------------------------------------------------ */
static int LuaUtilErrGeneric(lua_State*const lS)
{ // Get error message and stack. Don't one line this because the order of
  // execution is important!
  const string strError{ LuaUtilGetAndPopStr(lS) };
  LuaUtilPushStr(lS, StrAppend(strError, LuaUtilStack(lS)));
  return 1;
}
/* -- Push a function onto the stack --------------------------------------- */
static void LuaUtilPushCFunc(lua_State*const lS, lua_CFunction cFunc,
  const int iNVals=0)
    { lua_pushcclosure(lS, cFunc, iNVals); }
/* -- Push the above generic error function and return its id -------------- */
static int LuaUtilPushAndGetGenericErrId(lua_State*const lS)
  { LuaUtilPushCFunc(lS, LuaUtilErrGeneric); return LuaUtilStackSize(lS); }
/* == Generate an exception if the specified condition is false ============ */
static void LuaUtilAssert(lua_State*const lS, const bool bCond,
  const int iIndex, const char*const cpType)
{ // Return if condition is true else break execution
  if(bCond) return;
  XC("Invalid parameter!", "Parameter", iIndex,
     "Required", cpType, "Supplied",  LuaUtilGetType(lS, iIndex));
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
  if(lua_rawlen(lS, iParam) > 0) return;
  XC("Non-empty string required!", "Parameter", iParam);
}
/* -- Get the specified string from the stack ------------------------------ */
template<typename StrType, typename StrTypeConstPtr=const StrType*>
  static StrTypeConstPtr LuaUtilGetStr(lua_State*const lS, const int iParam)
{ // Throw if specified parameter isn't a string else return cast
  static_assert(!is_pointer_v<StrType>, "Do not use pointers!");
  static_assert(sizeof(StrType)==sizeof(uint8_t), "Invalid size!");
  LuaUtilCheckStr(lS, iParam);
  return reinterpret_cast<StrTypeConstPtr>(lua_tostring(lS, iParam));
}
/* -- Get the specified string from the stack ------------------------------ */
template<typename StrType, typename StrTypeConstPtr=const StrType*>
  static StrTypeConstPtr LuaUtilGetStrNE[[maybe_unused]](lua_State*const lS,
    const int iParam)
{ // Throw if specified parameter isn't a string or empty else return cast
  static_assert(!is_pointer_v<StrType>, "Do not use pointers!");
  static_assert(sizeof(StrType)==sizeof(uint8_t), "Invalid size!");
  LuaUtilCheckStrNE(lS, iParam);
  return reinterpret_cast<StrTypeConstPtr>(lua_tostring(lS, iParam));
}
/* -- Get and return a string and throw exception if not a string ---------- */
template<typename StrType, typename StrTypeConstPtr=const StrType*>
  static StrTypeConstPtr LuaUtilGetLStr(lua_State*const lS,
    const int iParam, size_t &stLen)
{ // Throw if specified parameter isn't a string else return a cast of it
  static_assert(!is_pointer_v<StrType>, "Do not use pointers!");
  static_assert(sizeof(StrType)==sizeof(uint8_t), "Invalid size!");
  LuaUtilCheckStr(lS, iParam);
  return reinterpret_cast<StrTypeConstPtr>
    (LuaUtilToLString(lS, iParam, stLen));
}
/* -- Helper for LuaUtilGetLStr that makes a memory block ------------------ */
static Memory LuaUtilGetMBfromLStr(lua_State*const lS, const int iParam)
{ // Get string, store size and return a conversion of it to memory class
  size_t stStrLen;
  const char*const cpStr = LuaUtilGetLStr<char>(lS, iParam, stStrLen);
  return { stStrLen, cpStr };
}
/* -- Get and return a C++ string and throw exception if not a string ------ */
static const string LuaUtilGetCppStr(lua_State*const lS, const int iParam)
{ // Throw if requested parameter isn't a string else return it
  LuaUtilCheckStr(lS, iParam);
  return LuaUtilToCppString(lS, iParam);
}
/* -- Get and return a C++ string and throw exception if not string/empty -- */
static const string LuaUtilGetCppStrNE(lua_State*const lS, const int iParam)
{ // Throw if requested parameter isn't a string or empty else return it
  LuaUtilCheckStrNE(lS, iParam);
  return LuaUtilToCppString(lS, iParam);
}
/* -- Get and return a C++ string and throw exception if not string/empty -- */
static const string LuaUtilGetCppFile(lua_State*const lS, const int iParam)
{ // Test to make sure if supplied parameter is a valid string
  LuaUtilCheckStr(lS, iParam);
  // Get the filename and verify that the filename is valid
  const string strFile{ LuaUtilToCppString(lS, iParam) };
  if(const ValidResult vrId = DirValidName(strFile))
    XC("Invalid parameter!",
       "Param",    iParam,                          "File",    strFile,
       "Reason",   cDirBase->DirBaseVNRtoStr(vrId), "ReasonId", vrId);
  // Return the constructed string
  return strFile;
}
/* -- Get and return a C++ string and throw exception if not string/empty -- */
static const string LuaUtilGetCppDir(lua_State*const lS, const int iParam)
{ // Test to make sure if supplied parameter is a valid string.
  LuaUtilCheckStr(lS, iParam);
  // Get the filename and verify that the filename is valid.
  const string strFile{ LuaUtilToCppString(lS, iParam) };
  switch(const ValidResult vrId = DirValidName(strFile))
  { // Ok or current directory? Allow the name.
    case VR_OK: case VR_CURRENT: return strFile;
    // Anything else?
    default: XC("Invalid parameter!",
      "Param",  iParam,                          "File",     strFile,
      "Reason", cDirBase->DirBaseVNRtoStr(vrId), "ReasonId", vrId);
  } // We don't get here.
}
/* -- Get and return a C++ string and throw exception if not a string ------ */
static const string LuaUtilGetCppStrUpper(lua_State*const lS, const int iParam)
{ // Throw if requested parameter isn't a string else return it in uppercase
  string strStr{ LuaUtilGetCppStr(lS, iParam) };
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
static void LuaUtilCheckFunc(lua_State*const) { }
template<typename ...VarArgs>
  static void LuaUtilCheckFunc(lua_State*const lS, const int iIndex,
    const VarArgs &...vaVars)
      { LuaUtilAssert(lS, LuaUtilIsFunction(lS, iIndex), iIndex, "function");
        LuaUtilCheckFunc(lS, vaVars...); }
/* -- Get and return a boolean and throw exception if not a boolean -------- */
static bool LuaUtilGetBool(lua_State*const lS, const int iIndex)
{ // Throw if requested parameter isn't a boolean else return it
  LuaUtilAssert(lS, LuaUtilIsBoolean(lS, iIndex), iIndex, "boolean");
  return LuaUtilToBool(lS, iIndex);
}
/* -- Try to get and check a valid number not < or >= ---------------------- */
template<typename NumType>
  static NumType LuaUtilGetNum(lua_State*const lS, const int iIndex)
{ // Throw if requested parameter isn't a number else return a cast of it
  static_assert(is_floating_point_v<NumType>, "Not floating point!");
  LuaUtilAssert(lS, LuaUtilIsNumber(lS, iIndex), iIndex, "number");
  return LuaUtilToNum<NumType>(lS, iIndex);
}
/* -- Try to get and check a valid number not < ---------------------------- */
template<typename NumType>
  static NumType LuaUtilGetNumL(lua_State*const lS, const int iIndex,
    const NumType ntMin)
{ // Return number if valid and in range else break execution
  const NumType ntVal = LuaUtilGetNum<NumType>(lS, iIndex);
  if(ntVal >= ntMin) return ntVal;
  XC("Number out of range!",
     "Parameter", iIndex, "Supplied", ntVal, "NotLesser", ntMin);
}
/* -- Try to get and check a valid number not < or > ----------------------- */
template<typename NumType>
  static NumType LuaUtilGetNumLG(lua_State*const lS, const int iIndex,
    const NumType ntMin, const NumType ntMax)
{ // Return number if valid and in range else break execution
  const NumType ntVal = LuaUtilGetNum<NumType>(lS, iIndex);
  if(ntVal >= ntMin && ntVal <= ntMax) return ntVal;
  XC("Number out of range!",
     "Parameter", iIndex, "Supplied", ntVal,
     "NotLesser", ntMin,  "NotGreater", ntMax);
}
/* -- Try to get and check a valid number not < or >= ---------------------- */
template<typename NumType>
  static NumType LuaUtilGetNumLGE(lua_State*const lS, const int iIndex,
    const NumType ntMin, const NumType ntMax)
{ // Return number if valid and in range else break execution
  const NumType ntVal = LuaUtilGetNum<NumType>(lS, iIndex);
  if(ntVal >= ntMin && ntVal < ntMax) return ntVal;
  XC("Number out of range!",
     "Parameter", iIndex, "Supplied", ntVal,
     "NotLesser", ntMin,  "NotGreaterEqual", ntMax);
}
/* -- Try to get and force a number value between -1 and 1 ----------------- */
template<typename NumType>
  static NumType LuaUtilGetNormal(lua_State*const lS, const int iIndex)
{ // Throw error if value not a number else return it clamped between -1 and 1.
  const lua_Number lnVal = LuaUtilGetNum<lua_Number>(lS, iIndex);
  return static_cast<NumType>(fmod(lnVal, 1.0));
}
/* -- Try to get and check a valid integer --------------------------------- */
template<typename IntType>
  static IntType LuaUtilGetInt(lua_State*const lS, const int iIndex)
{ // Throw error if value isn't an integer else return a cast of it
  static_assert(is_integral_v<IntType> || is_enum_v<IntType>, "Not integral!");
  LuaUtilAssert(lS, LuaUtilIsInteger(lS, iIndex), iIndex, "integer");
  return LuaUtilToInt<IntType>(lS, iIndex);
}
/* -- Try to get and check a valid integer not < --------------------------- */
template<typename IntType>
  static IntType LuaUtilGetIntL(lua_State*const lS, const int iIndex,
    const IntType itMin)
{ // Return integer if valid and in range else break execution
  const IntType itVal = LuaUtilGetInt<IntType>(lS, iIndex);
  if(itVal >= itMin) return itVal;
  XC("Integer out of range!",
     "Parameter", iIndex, "Supplied", itVal, "NotLesser", itMin);
}
/* -- Try to get and check a valid integer range not < or > ---------------- */
template<typename IntType>
  static IntType LuaUtilGetIntLG(lua_State*const lS, const int iIndex,
    const IntType itMin, const IntType itMax)
{ // Return integer if valid and in range else break execution
  const IntType itVal = LuaUtilGetInt<IntType>(lS, iIndex);
  if(itVal >= itMin && itVal <= itMax) return itVal;
  XC("Integer out of range!",
     "Parameter", iIndex, "Supplied", itVal,
     "NotLesser", itMin,  "NotGreater", itMax);
}
/* -- Try to get and check a valid integer range not < or > and = ^2 ------- */
template<typename IntType>
  static IntType LuaUtilGetIntLGP2(lua_State*const lS, const int iIndex,
    const IntType itMin, const IntType itMax)
{ // Return integer if valid, in range and is ^2 else break execution
  const IntType itVal = LuaUtilGetIntLG(lS, iIndex, itMin, itMax);
  if(StdIntIsPOW2(itVal)) return itVal;
  XC("Integer is not a power of two!",
     "Parameter", iIndex, "Supplied", itVal);
}
/* -- Try to get and check a valid integer range not < or >= --------------- */
template<typename IntType>
  static IntType LuaUtilGetIntLGE(lua_State*const lS, const int iIndex,
    const IntType itMin, const IntType itMax)
{ // Return integer if valid and in range else break execution
  const IntType itVal = LuaUtilGetInt<IntType>(lS, iIndex);
  if(itVal >= itMin && itVal < itMax) return itVal;
  XC("Integer out of range!",
     "Parameter", iIndex, "Supplied", itVal,
     "NotLesser", itMin,  "NotGreaterEqual", itMax);
}
/* -- Try to get and check a valid integer range not <= or > --------------- */
template<typename IntType>
  static IntType LuaUtilGetIntLEG(lua_State*const lS, const int iIndex,
    const IntType itMin, const IntType itMax)
{ // Return integer if valid and in range else break execution
  const IntType itVal = LuaUtilGetInt<IntType>(lS, iIndex);
  if(itVal > itMin && itVal <= itMax) return itVal;
  // Throw error
  XC("Integer out of range!",
     "Parameter",      iIndex, "Supplied", itVal,
     "NotLesserEqual", itMin,  "NotGreater", itMax);
}
/* -- Try to get and check a 'Flags' parameter ----------------------------- */
template<class FlagsType>
  static const FlagsType LuaUtilGetFlags(lua_State*const lS, const int iIndex,
    const FlagsType &ftMask)
{ // Return flags if valid and in range else break execution
  static_assert(is_class_v<FlagsType>, "Not a class!");
  const FlagsType ftFlags{
    LuaUtilGetInt<decltype(ftFlags.FlagGet())>(lS, iIndex) };
  if(ftFlags.FlagIsZero() || ftFlags.FlagIsInMask(ftMask)) return ftFlags;
  XC("Flags out of range!",
     "Parameter", iIndex, "Supplied",  ftFlags.FlagGet(),
     "Mask", ftMask.FlagGet());
}
/* -- Destroy an object ---------------------------------------------------- */
template<class ClassType>
  static void LuaUtilClassDestroy(lua_State*const lS, const int iParam,
  const LuaIdent &liParent)
{ // Get userdata pointer from Lua and if the address is valid?
  static_assert(is_class_v<ClassType>, "Not a class!");
  if(LuaUtilClass*const lucPtr =
    reinterpret_cast<LuaUtilClass*>(
      luaL_checkudata(lS, iParam, liParent.LuaIdentCStr())))
  { // Get address to the C++ class and if that is valid?
    if(ClassType*const ctPtr = reinterpret_cast<ClassType*>(lucPtr->vpPtr))
    { // Clear the pointer to the C++ class and destroy it if not locked
      lucPtr->vpPtr = nullptr;
      if(ctPtr->LockIsNotSet()) delete ctPtr;
    }
  }
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
template<class ClassType>
  static ClassType *LuaUtilClassReuse(lua_State*const lS,
    const LuaIdent &liParent, ClassType*const ctPtr)
{ // Prepare a new object
  static_assert(is_class_v<ClassType>, "Not a class!");
  LuaUtilClass*const lucPtr = LuaUtilClassPrepNew(lS, liParent);
  // Assign object to lua so lua will be incharge of deleting it
  lucPtr->vpPtr = ctPtr;
  // Return pointer to new class allocated elseware
  return ctPtr;
}
/* -- Creates and allocates a pointer to a new class ----------------------- */
template<typename ClassType>
  static ClassType *LuaUtilClassCreate(lua_State*const lS,
    const LuaIdent &liParent)
{ // Prepare a new object
  static_assert(is_class_v<ClassType>, "Not a class!");
  LuaUtilClass*const lucPtr = LuaUtilClassPrepNew(lS, liParent);
  // Allocate class and return it if succeeded return it
  if(void*const vpPtr = lucPtr->vpPtr = new (nothrow)ClassType)
    return reinterpret_cast<ClassType*>(vpPtr);
  // Error occured so just throw exception
  XC("Failed to allocate memory for class structure!",
     "Type", liParent.LuaIdentStr(), "Size", sizeof(ClassType));
}
/* -- Creates a pointer to a class that LUA CAN'T deallocate --------------- */
template<typename ClassType>
  static ClassType *LuaUtilClassCreatePtr(lua_State*const lS,
    const LuaIdent &liParent, ClassType*const ctPtr)
{ // Create userdata
  static_assert(is_class_v<ClassType>, "Not a class!");
  LuaUtilClass*const lucPtr =
    reinterpret_cast<LuaUtilClass*>(lua_newuserdata(lS, sizeof(LuaUtilClass)));
  // Get table data from collector reference and set it as class metatable
  LuaUtilGetRef(lS, liParent.LuaIdentGetRef());
  LuaUtilSetMetaTable(lS, -2);
  // Set pointer to class
  lucPtr->vpPtr = reinterpret_cast<void*>(ctPtr);
  // Return pointer to memory
  return ctPtr;
}
/* -- Gets a pointer to any class ------------------------------------------ */
template<typename ClassType>
  ClassType *LuaUtilGetPtr(lua_State*const lS, const int iParam,
  const LuaIdent &liParent)
{ // Get lua data class and if it is valid
  static_assert(is_class_v<ClassType>, "Not a class!");
  if(const LuaUtilClass*const lucPtr =
    reinterpret_cast<LuaUtilClass*>(
      luaL_checkudata(lS, iParam, liParent.LuaIdentCStr())))
  { // Get reference to class and return pointer if valid
    const LuaUtilClass &lcR = *lucPtr;
    if(lcR.vpPtr) return reinterpret_cast<ClassType*>(lcR.vpPtr);
    // Actual class pointer has already been freed so error occured
    XC("Unallocated class parameter!",
       "Parameter", iParam, "Type", liParent.LuaIdentStr());
  } // lua data class not valid
  XC("Null class parameter!",
     "Parameter", iParam, "Type", liParent.LuaIdentStr());
}
/* -- Check that a class isn't locked (i.e. a built-in class) -------------- */
template<class ClassType>
  ClassType *LuaUtilGetUnlockedPtr(lua_State*const lS, const int iParam)
{ // Get pointer to class and return if isn't locked (a built-in class)
  static_assert(is_class_v<ClassType>, "Not a class!");
  ClassType*const ctPtr = LuaUtilGetPtr<ClassType>(lS, iParam);
  if(ctPtr->LockIsNotSet()) return ctPtr;
  // Throw error
  XC("Call not allowed on this class!", "Identifier", ctPtr->IdentGet());
}
/* -- Garbage collection control (two params) ------------------------------ */
static int LuaUtilGCSet(lua_State*const lS, const int iCmd, const int iVal1,
  const int iVal2)
    { return lua_gc(lS, iCmd, iVal1, iVal2); }
/* -- Garbage collection control (one param) ------------------------------- */
static int LuaUtilGCSet[[maybe_unused]](lua_State*const lS, const int iCmd,
  const int iVal)
    { return lua_gc(lS, iCmd, iVal); }
/* -- Garbage collection control (no param) -------------------------------- */
static int LuaUtilGCSet(lua_State*const lS, const int iCmd)
  { return lua_gc(lS, iCmd); }
/* -- Stop garbage collection ---------------------------------------------- */
static int LuaUtilGCStop(lua_State*const lS)
  { return LuaUtilGCSet(lS, LUA_GCSTOP); }
/* -- Start garbage collection --------------------------------------------- */
static int LuaUtilGCStart(lua_State*const lS)
  { return LuaUtilGCSet(lS, LUA_GCRESTART); }
/* -- Execute garbage collection ------------------------------------------- */
static int LuaUtilGCRun(lua_State*const lS)
  { return LuaUtilGCSet(lS, LUA_GCCOLLECT); }
/* -- Returns if garbage collection is running ----------------------------- */
static bool LuaUtilGCRunning(lua_State*const lS)
  { return !!LuaUtilGCSet(lS, LUA_GCISRUNNING); }
/* -- Get memory usage ----------------------------------------------------- */
static size_t LuaUtilGetUsage(lua_State*const lS)
  { return static_cast<size_t>(LuaUtilGCSet(lS, LUA_GCCOUNT) +
      LuaUtilGCSet(lS, LUA_GCCOUNTB) / 1024) * 1024; }
/* -- Full garbage collection while logging memory usage ------------------- */
static size_t LuaUtilGCCollect(lua_State*const lS)
{ // Get current usage, do a full garbage collect and return delta
  const size_t stUsage = LuaUtilGetUsage(lS);
  LuaUtilGCRun(lS);
  return stUsage - LuaUtilGetUsage(lS);
}
/* -- Standard in-sandbox call function (unmanaged) ------------------------ */
static void LuaUtilCallFuncEx(lua_State*const lS, const int iParams=0,
  const int iReturns=0)
    { lua_call(lS, iParams, iReturns); }
/* -- Standard in-sandbox call function (unmanaged, no params) ------------- */
static void LuaUtilCallFunc(lua_State*const lS, const int iReturns=0)
  { LuaUtilCallFuncEx(lS, 0, iReturns); }
/* -- Sandboxed call function (doesn't remove error handler) --------------- */
static int LuaUtilPCallEx(lua_State*const lS, const int iParams=0,
  const int iReturns=0, const int iHandler=0)
    { return lua_pcall(lS, iParams, iReturns, iHandler); }
/* -- Sandboxed call function (removes error handler) ---------------------- */
static int LuaUtilPCallExSafe(lua_State*const lS, const int iParams=0,
  const int iReturns=0, const int iHandler=0)
{ // Do protected call and get result
  const int iResult = LuaUtilPCallEx(lS, iParams, iReturns, iHandler);
  // Remove error handler from stack if handler specified
  if(iHandler) LuaUtilRmStack(lS, iHandler);
  // Return result
  return iResult;
}
/* -- Handle LuaUtilPCall result ------------------------------------------- */
static void LuaUtilPCallResultHandle(lua_State*const lS, const int iResult)
{ // Function to call lookup table
  // Compare error code
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
static void LuaUtilPCallSafe(lua_State*const lS, const int iParams=0,
  const int iReturns=0, const int iHandler=0)
    { LuaUtilPCallResultHandle(lS,
        LuaUtilPCallExSafe(lS, iParams, iReturns, iHandler));}
/* -- Sandboxed call function that doesn't pop the handler ----------------- */
static void LuaUtilPCall(lua_State*const lS, const int iParams=0,
  const int iReturns=0, const int iHandler=0)
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
static void LuaUtilToTable[[maybe_unused]](lua_State*const lS,
  const StrStrMap &ssmData)
{ // Create the table, we're creating non-indexed key/value pairs
  LuaUtilPushTable(lS, 0, ssmData.size());
  // For each table item
  for(const StrStrMapPair &ssmPair : ssmData)
  { // Push value and key name
    LuaUtilPushStr(lS, ssmPair.second);
    LuaUtilSetField(lS, -2, ssmPair.first.c_str());
  }
}
/* -- Push the specified string at the specified index --------------------- */
static void LuaUtilSetTableIdxStr(lua_State*const lS,
  const int iTableId, const lua_Integer liIndex, const string &strValue)
{ // Push at the specified index, the specified string and set it to the table
  LuaUtilPushInt(lS, liIndex);
  LuaUtilPushStr(lS, strValue);
  LuaUtilSetRaw(lS, iTableId);
}
/* -- Push the specified integer at the specified index -------------------- */
template<typename IntType>static void LuaUtilSetTableIdxInt(lua_State*const lS,
  const int iTableId, const lua_Integer liIndex, const IntType itValue)
{ // Push at the specified index, the specified value and set it to the table
  static_assert(is_integral_v<IntType> || is_enum_v<IntType>, "Not integral!");
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
    LuaUtilPushTable(lS, 6);
    // Push file parts
    LuaUtilSetTableIdxStr(lS, -3, 1, dempRef.first);               // File name
    LuaUtilSetTableIdxInt(lS, -3, 2, dempRef.second.Size());       // Size
    LuaUtilSetTableIdxInt(lS, -3, 3, dempRef.second.Created());    // Created
    LuaUtilSetTableIdxInt(lS, -3, 4, dempRef.second.Written());    // Updated
    LuaUtilSetTableIdxInt(lS, -3, 5, dempRef.second.Accessed());   // Accessed
    LuaUtilSetTableIdxInt(lS, -3, 6, dempRef.second.Attributes()); // Attrs
    // Push file data table
    LuaUtilSetRaw(lS, -3);
  }
}
/* -- Convert string vector to lua table and put it on stack --------------- */
template<typename ListType>
  static void LuaUtilToTable(lua_State*const lS, const ListType &ltData)
{ // Create the table, we're creating a indexed/value array and return if empty
  LuaUtilPushTable(lS, ltData.size());
  if(ltData.empty()) return;
  // Id number for array index
  lua_Integer iIndex = 0;
  // For each table item
  for(const string &strItem : ltData)
    LuaUtilSetTableIdxStr(lS, -3, ++iIndex, strItem);
}
/* -- Explode LUA string into table ---------------------------------------- */
static void LuaUtilExplode(lua_State*const lS)
{ // Check parameters
  size_t stStr, stSep;
  const char*const cpStr = LuaUtilGetLStr<char>(lS, 1, stStr),
            *const cpSep = LuaUtilGetLStr<char>(lS, 2, stSep);
  // Create empty table if string invalid
  if(!stStr || !stSep) { LuaUtilPushTable(lS); return; }
  // Else convert whats in the string
  LuaUtilToTable(lS, Token({cpStr, stStr}, {cpSep, stSep}));
}
/* -- Explode LUA string into table ---------------------------------------- */
static void LuaUtilExplodeEx(lua_State*const lS)
{ // Check parameters
  size_t stStr, stSep;
  const char*const cpStr = LuaUtilGetLStr<char>(lS, 1, stStr),
            *const cpSep = LuaUtilGetLStr<char>(lS, 2, stSep);
  const size_t stMax = LuaUtilGetInt<size_t>(lS, 3);
  // Create empty table if string invalid
  if(!stStr || !stSep || !stMax) { LuaUtilPushTable(lS); return; }
  // Else convert whats in the string
  LuaUtilToTable(lS, Token({cpStr, stStr}, {cpSep, stSep}, stMax));
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
  const lua_Integer liIndex, string &strOutput, const char *cpStr,
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
    string strOutput{ LuaUtilToCppString(lS) };
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
    string strOutput{ LuaUtilToCppString(lS) };
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
  const lua_Unsigned uiIndexedCount = LuaUtilGetSize(lS, 1);
  // Number of items in table
  lua_Unsigned uiCount = 0;
  // Until there are no more items
  for(LuaUtilPushNil(lS); lua_next(lS, -2); LuaUtilRmStack(lS)) ++uiCount;
  // Remove key
  LuaUtilRmStack(lS);
  // Return count of key/value pairs in table
  return uiCount - uiIndexedCount;
}
/* -- Replace text with values from specified LUA table -------------------- */
static void LuaUtilReplaceMulti(lua_State*const lS)
{ // Get string to replace
  string strDest{ LuaUtilGetCppStr(lS, 1) };
  // Check that we have a table of strings
  LuaUtilCheckTable(lS, 2);
  // Source string is empty or there are indexed items in the table? Remove
  // table and return original blank string
  if(strDest.empty() || LuaUtilGetSize(lS, 2)) return LuaUtilRmStack(lS);
  // Build table
  StrPairList lList;
  // Until there are no more items, add value if key is a string
  for(LuaUtilPushNil(lS); lua_next(lS, -2); LuaUtilRmStack(lS))
    if(LuaUtilIsString(lS, -1))
      lList.push_back({ LuaUtilToCppString(lS, -2), LuaUtilToCppString(lS) });
  // Return original string if nothing added
  if(lList.empty()) return LuaUtilRmStack(lS);
  // Remove table and string parameter
  lua_pop(lS, 2);
  // Execute replacements and return newly made string
  LuaUtilPushStr(lS, StrReplaceEx(strDest, lList));
}
/* -- Convert string/uint map to table ------------------------------------- */
static void LuaUtilToTable(lua_State*const lS, const StrUIntMap &suimRef)
{ // Create the table, we're creating non-indexed key/value pairs
  LuaUtilPushTable(lS, 0, suimRef.size());
  // For each table item
  for(const StrUIntMapPair &suimpRef : suimRef)
  { // Push value and key name
    LuaUtilPushInt(lS, suimpRef.second);
    LuaUtilSetField(lS, -2, suimpRef.first.c_str());
  }
}
/* -- Convert varlist to lua table and put it on stack --------------------- */
static void LuaUtilToTable(lua_State*const lS, const StrNCStrMap &sncsmMap)
{ // Create the table, we're creating non-indexed key/value pairs
  LuaUtilPushTable(lS, 0, sncsmMap.size());
  // For each table item
  for(const StrNCStrMapPair &sncsmpPair : sncsmMap)
  { // Push value and key name
    LuaUtilPushStr(lS, sncsmpPair.second);
    LuaUtilSetField(lS, -2, sncsmpPair.first.c_str());
  }
}
/* -- Convert varlist to lua table and put it on stack --------------------- */
template<class VectorType,
         typename VectorValueType = typename VectorType::value_type>
  static VectorType LuaUtilToNumVector(lua_State*const lS, const int iArg)
{ // Create the table, we're creating non-indexed key/value pairs
  LuaUtilCheckTable(lS, iArg);
  // Get maximums
  const size_t stMax = UtilIntOrMax<size_t>(LuaUtilGetSize(lS, iArg));
  const lua_Integer liMax = static_cast<lua_Integer>(stMax) + 1;
  // Preallocate the table
  VectorType vtArray;
  vtArray.reserve(stMax);
  // Walk the array
  for(lua_Integer liI = 1; liI < liMax; ++liI)
  { // Get item from table
    LuaUtilPushInt(lS, liI);
    lua_gettable(lS, -2);
    // Get the string from Lua stack and save the length
    vtArray.push_back(LuaUtilGetNum<VectorValueType>(lS, -1));
    // Remove item from stack
    LuaUtilRmStack(lS);
  } // Return the container
  return vtArray;
}
/* -- Convert varlist to lua table and put it on stack --------------------- */
template<class VectorType,
         typename VectorValueType = typename VectorType::value_type>
  static VectorType LuaUtilToIntVector(lua_State*const lS, const int iArg)
{ // Create the table, we're creating non-indexed key/value pairs
  LuaUtilCheckTable(lS, iArg);
  // Get maximums
  const size_t stMax = UtilIntOrMax<size_t>(LuaUtilGetSize(lS, iArg));
  const lua_Integer liMax = static_cast<lua_Integer>(stMax) + 1;
  // Preallocate the table
  VectorType vtArray;
  vtArray.reserve(stMax);
  // Walk the array
  for(lua_Integer liI = 1; liI < liMax; ++liI)
  { // Get item from table
    LuaUtilPushInt(lS, liI);
    lua_gettable(lS, -2);
    // Get the string from Lua stack and save the length
    vtArray.push_back(LuaUtilGetInt<VectorValueType>(lS, -1));
    // Remove item from stack
    LuaUtilRmStack(lS);
  } // Return the container
  return vtArray;
}
/* -- Set a global variable ------------------------------------------------ */
static void LuaUtilSetGlobal(lua_State*const lS, const char*const cpKey)
  { lua_setglobal(lS, cpKey); }
/* -- Get a global variable ------------------------------------------------ */
static void LuaUtilGetGlobal(lua_State*const lS, const char*const cpKey)
  { lua_getglobal(lS, cpKey); }
/* -- Returns t[k] --------------------------------------------------------- */
static void LuaUtilGetField(lua_State*const lS, const int iIndex,
  const char*const cpKey)
    { lua_getfield(lS, iIndex, cpKey); }
/* -- Initialise lua and clib random number generators --------------------- */
static void LuaUtilInitRNGSeed(lua_State*const lS, const lua_Integer liSeed)
{ // Make C-Lib use the specified seed
  StdSRand(static_cast<unsigned int>(liSeed));
  // Get 'math' table
  LuaUtilGetGlobal(lS, "math");
  // Get pointer to function
  LuaUtilGetField(lS, -1, "randomseed");
  // Push a random seed
  LuaUtilPushInt(lS, liSeed);
  // Calls randomseed(uqSeed)
  LuaUtilCallFuncEx(lS, 1);
  // Removes the table 'math'
  LuaUtilRmStack(lS);
}
/* -- Return true if lua stack can take specified more items --------------- */
static bool LuaUtilIsStackAvail(lua_State*const lS, const int iCount)
  { return lua_checkstack(lS, iCount); }
/* -- Return true if lua stack can take specified more items (diff type) --- */
template<typename IntType>
  static bool LuaUtilIsStackAvail(lua_State*const lS, const IntType itCount)
    { return UtilIntWillOverflow<int>(itCount) ? false :
        LuaUtilIsStackAvail(lS, static_cast<int>(itCount)); }
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
