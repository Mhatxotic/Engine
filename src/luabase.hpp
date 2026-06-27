/* == LUAIF.HPP ============================================================ **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## These are all wrapper functions we use to interface with LUA        ## **
** ## itself. Keep all other 'helper' functions in 'luautil.hpp'. We do   ## **
** ## this incase the LUA developers decide to change something and       ## **
** ## hopefully we just have to change this module. We also add fancy C++ ## **
** ## templates to make them easier to use. Note that with compiler       ## **
** ## optimisations enabled. This namespace and all the functions in it   ## **
** ## should be optimised out.                                            ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace ILuaBase {                   // Start of private module namespace
/* ------------------------------------------------------------------------- */
using namespace IUtil::P;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* -- Get and set top of stack --------------------------------------------- */
static int LuaBaseGetTop(lua_State*const lS) { return lua_gettop(lS); }
static void LuaBaseSetTop(lua_State*const lS, const int iIndex)
  { lua_settop(lS, iIndex); }
/* -- Get length of a table ------------------------------------------------ */
template<typename IntType = lua_Unsigned>
  requires StdIsIntegral<IntType>
static IntType LuaBaseRawLen(lua_State*const lS, const int iIndex)
  { return static_cast<IntType>(lua_rawlen(lS, iIndex)); }
/* -- Position on the stack doesn't exist? --------------------------------- */
static bool LuaBaseIsNone(lua_State*const lS, const int iIndex)
  { return lua_isnone(lS, iIndex) != 0; }
/* -- Type is a nil? ------------------------------------------------------- */
static bool LuaBaseIsNil(lua_State*const lS, const int iIndex)
  { return lua_isnil(lS, iIndex) != 0; }
/* -- Type is a thread? ---------------------------------------------------- */
static bool LuaBaseIsThread(lua_State*const lS, const int iIndex)
  { return lua_isthread(lS, iIndex) != 0; }
/* -- Type is a userdata? -------------------------------------------------- */
static bool LuaBaseIsBool(lua_State*const lS, const int iIndex)
  { return lua_isboolean(lS, iIndex) != 0; }
/* -- Type is a userdata? -------------------------------------------------- */
static bool LuaBaseIsUData(lua_State*const lS, const int iIndex)
  { return lua_isuserdata(lS, iIndex) != 0; }
/* -- Type is light userdata (pointer to class)? --------------------------- */
static bool LuaBaseIsLightUData(lua_State*const lS, const int iIndex)
  { return lua_islightuserdata(lS, iIndex) != 0; }
/* -- Type is a function? -------------------------------------------------- */
static bool LuaBaseIsFunc(lua_State*const lS, const int iIndex)
  { return lua_isfunction(lS, iIndex) != 0; }
/* -- Type is a C function? ------------------------------------------------ */
static bool LuaBaseIsCFunc(lua_State*const lS, const int iIndex)
  { return lua_iscfunction(lS, iIndex) != 0; }
/* -- Type is a integer? --------------------------------------------------- */
static bool LuaBaseIsInt(lua_State*const lS, const int iIndex)
  { return lua_isinteger(lS, iIndex) != 0; }
/* -- Type is a number? ---------------------------------------------------- */
static bool LuaBaseIsNum(lua_State*const lS, const int iIndex)
  { return lua_isnumber(lS, iIndex) != 0; }
/* -- Type is a string? ---------------------------------------------------- */
static bool LuaBaseIsStr(lua_State*const lS, const int iIndex)
  { return lua_isstring(lS, iIndex) != 0; }
/* -- Type is a table? ----------------------------------------------------- */
static bool LuaBaseIsTable(lua_State*const lS, const int iIndex)
  { return lua_istable(lS, iIndex) != 0; }
/* -- Push a pointer ------------------------------------------------------- */
static void LuaBasePushLightUData(lua_State*const lS, void*const vpPtr)
  { lua_pushlightuserdata(lS, vpPtr); }
/* -- Get string and size of it -------------------------------------------- */
static const char *LuaBaseToLStr(lua_State*const lS, const int iIndex,
  size_t &stSize) { return lua_tolstring(lS, iIndex, &stSize); }
/* -- Get string and size of it (type safe) -------------------------------- */
static const char *LuaBaseToLStrTS(lua_State*const lS, const int iIndex,
  size_t *stpSize = nullptr) { return luaL_tolstring(lS, iIndex, stpSize); }
/* -- Get a number from the stack ------------------------------------------ */
template<typename FloatType = lua_Number>
  requires StdIsFloat<FloatType>
static FloatType LuaBaseToNum(lua_State*const lS, const int iIndex)
  { return static_cast<FloatType>(lua_tonumber(lS, iIndex)); }
/* -- Get an integer from the stack ---------------------------------------- */
template<typename IntType = lua_Integer>
  requires StdIsIntegral<IntType> || StdIsEnum<IntType>
static IntType LuaBaseToInt(lua_State*const lS, const int iIndex)
  { return static_cast<IntType>(lua_tointeger(lS, iIndex)); }
/* -- Get an boolean from the stack ---------------------------------------- */
static bool LuaBaseToBool(lua_State*const lS, const int iIndex)
  { return lua_toboolean(lS, iIndex); }
/* -- Get an pointer from the stack ---------------------------------------- */
template<typename ParentType = void,
         typename ParentTypePtr = const ParentType*>
  requires (!StdIsPointer<ParentType>)
static ParentTypePtr LuaBaseToPtr(lua_State*const lS, const int iIndex)
  { return reinterpret_cast<ParentTypePtr>(lua_topointer(lS, iIndex)); }
/* -- Get type of item on stack -------------------------------------------- */
static int LuaBaseType(lua_State*const lS, const int iIndex)
  { return lua_type(lS, iIndex); }
/* -- Get human readable name of specified type id ------------------------- */
static const char *LuaBaseTypeName(lua_State*const lS, const int iIndex)
  { return lua_typename(lS, LuaBaseType(lS, iIndex)); }
/* -- Push a string onto the stack ----------------------------------------- */
static void LuaBasePushCStr(lua_State*const lS, const void*const vpString)
  { lua_pushstring(lS, reinterpret_cast<const char*>(vpString)); }
/* -- Push a nil onto the stack -------------------------------------------- */
static void LuaBasePushNil(lua_State*const lS) { lua_pushnil(lS); }
/* -- Get or set raw access without triggering meta methods ---------------- */
static void LuaBaseRawGet(lua_State*const lS, const int iIndex)
  { lua_rawget(lS, iIndex); }
static void LuaBaseRawSet(lua_State*const lS, const int iIndex)
  { lua_rawset(lS, iIndex); }
/* -- Remove item from stack ----------------------------------------------- */
static void LuaBaseRemove(lua_State*const lS, const int iIndex)
  { lua_remove(lS, iIndex); }
/* -- Push a part of memory as a string ------------------------------------ */
template<typename AnyType>
  requires StdIsPointer<AnyType*>
static void LuaBasePushExtStr(lua_State*const lS, const AnyType*const atPtr,
  const size_t stLen, lua_Alloc laFunc = nullptr,
  void*const vpUserData = nullptr)
{ lua_pushexternalstring(lS, atPtr, stLen, laFunc, vpUserData); }
/* -- Push a literal string onto the stack --------------------------------- */
template<typename AnyType, typename IntType>
  requires StdIsPointer<AnyType*> && StdIsIntegral<IntType>
static void LuaBasePushLStr(lua_State*const lS, const AnyType*const atPtr,
  const IntType itSize)
{ lua_pushlstring(lS, reinterpret_cast<const char*>(atPtr),
                      static_cast<size_t>(itSize)); }
/* -- Get and set metatable entry from userdata ---------------------------- */
static int LuaBaseSetMetaTable(lua_State*const lS, const int iIndex)
  { return lua_setmetatable(lS, iIndex); }
static int LuaBaseGetMetaTable(lua_State*const lS, const int iIndex)
  { return lua_getmetatable(lS, iIndex); }
/* -- Set hook ------------------------------------------------------------- */
static void LuaBaseSetHookCb(lua_State*const lS, lua_Hook fcbCb, const int iC)
  { lua_sethook(lS, fcbCb, LUA_MASKCOUNT, iC); }
/* -- Push a table onto the stack of the specified preallocated size ------- */
template<typename IntType1 = int, typename IntType2 = int>
  requires (StdIsIntegral<IntType1> || StdIsEnum<IntType1>) &&
           (StdIsIntegral<IntType2> || StdIsEnum<IntType2>)
static void LuaBasePushTableAlloc(lua_State*const lS, const IntType1 itIndexes,
  const IntType2 itKeys)
{ lua_createtable(lS, UtilIntOrMax<int>(itIndexes),
                      UtilIntOrMax<int>(itKeys)); }
/* -- Push a table onto the stack with no preallocation -------------------- */
static void LuaBasePushTable(lua_State*const lS) { lua_newtable(lS); }
/* -- Get next key/value from a table -------------------------------------- */
static bool LuaBaseNext(lua_State*const lS, const int iIndex)
{ // You must push a 'nil' onto the stack before calling this and it is
  // consumed when done so. When this function returns success (true), a key
  // and a value are pushed onto the stack which the caller is responsible for
  // removing the value before reiterating and when there are no more entries
  // (false), there will be nothing left on the stack but the table argument
  // that was sent.
  return lua_next(lS, iIndex) != 0;
}
/* -- Insert a value into the table ---------------------------------------- */
static void LuaBaseInsert(lua_State*const lS, const int iIndex)
  { lua_insert(lS, iIndex); }
/* -- Push specified integral as boolean on to the stack ------------------- */
template<typename IntType>
  requires StdIsIntegral<IntType>
static void LuaBasePushBool(lua_State*const lS, const IntType itValue)
  { lua_pushboolean(lS, static_cast<bool>(itValue)); }
/* -- Push a number onto the stack c --------------------------------------- */
template<typename IntType>
  requires StdIsArithmatic<IntType>
static void LuaBasePushNum(lua_State*const lS, const IntType itValue)
  { lua_pushnumber(lS, static_cast<lua_Number>(itValue)); }
/* -- Push an integer onto the stack --------------------------------------- */
template<typename IntType>
  requires StdIsArithmatic<IntType> || StdIsEnum<IntType>
static void LuaBasePushInt(lua_State*const lS, const IntType itValue)
  { lua_pushinteger(lS, static_cast<lua_Integer>(itValue)); }
/* -- Throw error ---------------------------------------------------------- */
static void LuaBaseError(lua_State*const lS) { lua_error(lS); }
/* -- Copy one value on the stack ------------------------------------------ */
static void LuaBasePushValue(lua_State*const lS, const int iIndex)
  { lua_pushvalue(lS, iIndex); }
/* -- Get t[k] or set t[k] = v --------------------------------------------- */
static void LuaBaseGetField(lua_State*const lS, const int iIndex,
  const char*const cpKey)
{ lua_getfield(lS, iIndex, cpKey); }
static void LuaBaseSetField(lua_State*const lS, const int iIndex,
  const char*const cpKey)
{ lua_setfield(lS, iIndex, cpKey); }
/* -- Get or set table reference ------------------------------------------- */
static void LuaBaseRawGetI(lua_State*const lS, const int iTable,
  const lua_Integer liIndex) { lua_rawgeti(lS, iTable, liIndex); }
static void LuaBaseRawSetI(lua_State*const lS, const int iTable,
  const lua_Integer liIndex) { lua_rawseti(lS, iTable, liIndex); }
/* -- Increase/decrease reference counter on stack item -------------------- */
static int LuaBaseRef(lua_State*const lS)
  { return luaL_ref(lS, LUA_REGISTRYINDEX); }
static void LuaBaseUnref(lua_State*const lS, const int iIndex)
  { luaL_unref(lS, LUA_REGISTRYINDEX, iIndex); }
/* -- Push a function onto the stack --------------------------------------- */
static void LuaBasePushCFunc(lua_State*const lS, lua_CFunction lcFunc,
  const int iNVals = 0) { lua_pushcclosure(lS, lcFunc, iNVals); }
/* -- Get the specified string --------------------------------------------- */
template<typename StringType, typename StringTypePtr = const StringType*>
  requires (!StdIsPointer<StringType>) &&
    (sizeof(StringType) == sizeof(uint8_t))
static StringTypePtr LuaBaseToStr(lua_State*const lS, const int iIndex)
  { return reinterpret_cast<StringTypePtr>(lua_tostring(lS, iIndex)); }
/* -- Dump lua function to memory ------------------------------------------ */
static int LuaBaseDump(lua_State*const lS, lua_Writer lwCallback,
  void*const vpUser, const int iStripDebug)
{ return lua_dump(lS, lwCallback, vpUser, iStripDebug); }
/* -- Return true if lua stack can take specified more items --------------- */
template<typename IntType>
  requires StdIsIntegral<IntType>
static bool LuaBaseCheckStack(lua_State*const lS, const IntType itCount)
  { return lua_checkstack(lS, UtilIntOrMax<int>(itCount)) != 0; }
/* -- Get and set a global variable ---------------------------------------- */
static void LuaBaseGetGlobal(lua_State*const lS, const char*const cpKey)
  { lua_getglobal(lS, cpKey); }
static void LuaBaseSetGlobal(lua_State*const lS, const char*const cpKey)
  { lua_setglobal(lS, cpKey); }
/* -- Get and set table t[k] ----------------------------------------------- */
static void LuaBaseGetTable(lua_State*const lS, const int iIndex)
  { lua_gettable(lS, iIndex); }
static void LuaBaseSetTable(lua_State*const lS, const int iIndex)
  { lua_settable(lS, iIndex); }
/* -- Garbage collection control ------------------------------------------- */
static int LuaBaseGC(lua_State*const lS, const int iCmd, const int iVal1,
  const int iVal2) { return lua_gc(lS, iCmd, iVal1, iVal2); }
static int LuaBaseGC(lua_State*const lS, const int iCmd, const int iVal)
  { return lua_gc(lS, iCmd, iVal); }
static int LuaBaseGC(lua_State*const lS, const int iCmd)
  { return lua_gc(lS, iCmd); }
/* -- Standard in-sandbox call function (unmanaged) ------------------------ */
static void LuaBaseCall(lua_State*const lS, const int iParams = 0,
  const int iReturns = 0)
{ lua_call(lS, iParams, iReturns); }
/* -- Sandboxed call function (doesn't remove error handler) --------------- */
static int LuaBasePCall(lua_State*const lS, const int iParams = 0,
  const int iReturns = 0, const int iHandler = 0)
{ return lua_pcall(lS, iParams, iReturns, iHandler); }
/* -- Create new user data object allocated with Lua's memory -------------- */
template<typename AnyType, typename IntType>
  requires StdIsPointer<AnyType*> && StdIsIntegral<IntType>
static AnyType *LuaBaseNewUData(lua_State*const lS, const IntType itSize)
 { return reinterpret_cast<AnyType*>(
     lua_newuserdata(lS, static_cast<size_t>(itSize))); }
/* -- Get user data as a specified type ------------------------------------ */
template<typename AnyType>
  requires StdIsPointer<AnyType*>
static AnyType *LuaBaseToUData(lua_State*const lS, const int iIndex)
  { return reinterpret_cast<AnyType*>(lua_touserdata(lS, iIndex)); }
/* -- Get a Lua thread ----------------------------------------------------- */
static lua_State *LuaBaseToThread(lua_State*const lS, const int iIndex)
  { return lua_tothread(lS, iIndex); }
/* -- Push global lua table (_G) ------------------------------------------- */
static void LuaBasePushGlobalsTable(lua_State*const lS)
  { lua_pushglobaltable(lS); }
/* -- Concatenates one string to another ----------------------------------- */
static void LuaBaseConcat(lua_State*const lS, const int iIndex)
  { lua_concat(lS, iIndex); }
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
