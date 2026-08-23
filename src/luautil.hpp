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
using namespace ILuaBase::P;           using namespace ILuaIdent::P;
using namespace IMemory::P;            using namespace IRefCtr::P;
using namespace IStd::P;               using namespace IStdLib::P;
using namespace IString::P;            using namespace IToken::P;
using namespace IUtf::P;               using namespace IUtil::P;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* -- Variables ------------------------------------------------------------ */
static unsigned uLuaPaused   = 0;      // Times Lua paused before handling it
static bool     bDebugLocals = true;   // Specifies to show locals on stack
/* -- Utility type defs ---------------------------------------------------- */
struct LuaUtilClass { void *vpPtr; };  // Holds a pointer to a class
/* -- Get the light user data pointer -------------------------------------- */
template<typename ParentType>
  requires StdIsPointer<ParentType*>
static ParentType *LuaUtilGetSimplePtr(lua_State*const lS, const int iParam)
{ // Return userdata as specified type or throw exception if not userdata
  if(LuaBaseIsUData(lS, iParam))
    return LuaBaseToUData<ParentType>(lS, iParam);
  XC("Not userdata!", "Param", iParam);
}
/* -- Get and return a C++ string without checking it ---------------------- */
template<typename StrType = StdStringView>
  requires StdIsString<StrType>
static StrType LuaUtilToCppString(lua_State*const lS, const int iParam = -1)
{ // Storage for string length. Do not optimise this because I am not sure
  // what the standard direction is for evaluating expression. Left-to-right
  // or right-to-left, so I will just store the string point first to be safe.
  size_t stLength;
  const char*const cpStr = LuaBaseToLStr(lS, iParam, stLength);
  return { cpStr, stLength };
}
/* -- Simple class to save and restore stack ------------------------------- */
class LuaStackSaver                    // Lua stack saver class
{ /* -- Private variables -------------------------------------------------- */
  const int        iTop;               // Current stack position
  lua_State*const  lState;             // State to use
  /* -- Return stack position -------------------------------------- */ public:
  int Value() const { return iTop; }
  /* -- Restore stack position --------------------------------------------- */
  void Restore() const { LuaBaseSetTop(lState, Value()); }
  /* -- Constructor -------------------------------------------------------- */
  explicit LuaStackSaver(lua_State*const lS) :
    iTop(LuaBaseGetTop(lS)), lState(lS) {}
  /* -- Destructor --------------------------------------------------------- */
  DTORHELPER(~LuaStackSaver, Restore())
};/* ----------------------------------------------------------------------- */
/* -- Push a string we manage onto the stack ------------------------------- */
static void LuaUtilPushExtStr(lua_State*const lS, const auto &strStr,
  lua_Alloc laFunc = nullptr, void*const vpUserData = nullptr)
{ LuaBasePushExtStr(lS, strStr.data(), strStr.size(), laFunc, vpUserData); }
/* -- Push a C++ string onto the stack ------------------------------------- */
static void LuaUtilPushStr(lua_State*const lS, const auto &strStr)
  { LuaBasePushLStr(lS, strStr.data(), strStr.size()); }
/* -- Return type of item in stack ----------------------------------------- */
static StdString LuaUtilGetStackType(lua_State*const lS, const int iIndex)
{ // What type of variable?
  switch(LuaBaseType(lS, iIndex))
  { // Nil?
    case LUA_TNIL: return StdString{ cCommon->CommonNil() }; break;
    // A number?
    case LUA_TNUMBER:
    { // If not actually an integer? Write as normal floating-point number
      if(!LuaBaseIsInt(lS, iIndex))
        return StrFromNum(LuaBaseToNum(lS, iIndex));
      // Get actual integer value and return it and it's hex value
      const lua_Integer liValue = LuaBaseToInt(lS, iIndex);
      return StrFormat("$ [0x$$]", liValue, StdIOSHex, liValue);
    } // A boolean?
    case LUA_TBOOLEAN:
      return StdString{ StrFromBoolTF(LuaBaseToBool(lS, iIndex)) };
    // A string?
    case LUA_TSTRING:
    { // Get value of string and return value with size
      const StdStringView ssvVal{ LuaUtilToCppString(lS, iIndex) };
      return StrFormat("[$] \"$\"", ssvVal.size(), ssvVal);
    } // A table?
    case LUA_TTABLE: return StrFormat("<table:$>[$]",
      LuaBaseToPtr(lS, iIndex), LuaBaseRawLen(lS, iIndex));
    // Userdata?
    case LUA_TUSERDATA:
    { // Save stack count and restore it when leaving scope because this scope
      // adds items to the stack.
      const LuaStackSaver lssUserData{ lS };
      // Get pointer to actual data, we'll print this after the type
      const void*const vpPtr = LuaBaseToPtr(lS, iIndex);
      // Get metatable, it's index and return if not metadata
      LuaBaseGetMetaTable(lS, iIndex);
      const int iMTIndex = LuaBaseGetTop(lS);
      if(!LuaBaseIsTable(lS, iMTIndex))
        return StrFormat("<userdata:$>", vpPtr);
      // Query the engine names key's value and get it's value
      LuaUtilPushExtStr(lS, cCommon->CommonLuaName());
      LuaBaseRawGet(lS, iMTIndex);
      const int iMTTypeIndex = iMTIndex + 1;
      // Show the value as string if we found it, else it's an unknown type
      return StrFormat("<$:$>",
        LuaBaseIsStr(lS, iMTTypeIndex) ?
          LuaUtilToCppString(lS, iMTTypeIndex) :
          cCommon->CommonUnknown(),
        vpPtr);
    } // Who knows? Function? Userdata?
    default: return StrFormat("<$:$>",
      LuaBaseTypeName(lS, iIndex), LuaBaseToPtr(lS, iIndex));
  }
}
/* -- Return status of item in stack --------------------------------------- */
static StdString LuaUtilGetStackTokens(lua_State*const lS, const int iIndex)
{ // Fill token buffer depending on status
  return StrFromEvalTokens({
    { LuaBaseIsNone(lS, iIndex),          '0' },
    { LuaBaseIsBool(lS, iIndex),       'B' },
    { LuaBaseIsCFunc(lS, iIndex),     'C' },
    { LuaBaseIsFunc(lS, iIndex),      'F' },
    { LuaBaseIsInt(lS, iIndex),       'I' },
    { LuaBaseIsLightUData(lS, iIndex), 'L' },
    { LuaBaseIsNum(lS, iIndex),        'N' },
    { LuaBaseIsThread(lS, iIndex),        'R' },
    { LuaBaseIsStr(lS, iIndex),        'S' },
    { LuaBaseIsTable(lS, iIndex),         'T' },
    { LuaBaseIsUData(lS, iIndex),      'U' },
    { LuaBaseIsNil(lS, iIndex),           'X' },
  });
}
/* -- Log the stack -------------------------------------------------------- */
static StdString LuaUtilGetVarStack(lua_State*const lS)
{ // If there are variables in the stack?
  if(const int iCount = LuaBaseGetTop(lS))
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
/* -- Push a memory block onto the stack as a string ----------------------- */
static void LuaUtilPushMem(lua_State*const lS, const MemConst &mcSrc)
  { LuaBasePushLStr(lS, mcSrc.MemPtr<char>(), mcSrc.MemSize()); }
/* -- Push multiple values of different types (use in ll*.hpp sources) ----- */
static void LuaUtilPushVar(lua_State*const) {}
template<typename ...VarArgs, typename AnyType>
  static void LuaUtilPushVar(lua_State*const lS, const AnyType &atVal,
    VarArgs &&...vaArgs)
{ // Type is STL string?
  if constexpr(StdIsSame<AnyType, StdString> ||
               StdIsSame<AnyType, StdStringView>) LuaUtilPushStr(lS, atVal);
  // Type is boolean?
  else if constexpr(StdIsSame<AnyType, bool>) LuaBasePushBool(lS, atVal);
  // Type is any pointer type (assuming char*, don't send anything else)
  else if constexpr(StdIsPointer<AnyType>) LuaBasePushCStr(lS, atVal);
  // Type is enum, int, long, short or int64?
  else if constexpr(StdIsIntegral<AnyType> || StdIsEnum<AnyType>)
    LuaBasePushInt(lS, atVal);
  // Type is float or double?
  else if constexpr(StdIsFloat<AnyType>) LuaBasePushNum(lS, atVal);
  // Strange bug in MSVC which shows no compile time stack trace
#if defined(MSVC_VANILLA)
  // Just push nil
  else LuaBasePushNil(lS);
  // A real compiler?
#else
  // Just push nil otherwise
  else static_assert(false, "Unknown type sent in function call!");
  // Compiler check
#endif
  // Shift to next variable
  LuaUtilPushVar(lS, StdForward<VarArgs>(vaArgs)...);
}
/* ------------------------------------------------------------------------- */
static int LuaUtilProcException(lua_State*const lS,
  const StdException &eReason)
{ // Push a string onto the stack that describes the current execution context
  luaL_where(lS, 1);
  // Push the exception reason
  LuaBasePushCStr(lS, eReason.what());
  // Concatenate both strings
  LuaBaseConcat(lS, 2);
  // Throw the error
  LuaBaseError(lS);
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
/* -- Get and pop string in the specified position ------------------------- */
static StdString LuaUtilGetAndPopStr(lua_State*const lS, const int iIndex)
{ // If there is nothing on the stack then return a generic error
  if(LuaBaseIsNone(lS, iIndex)) return "Error signalled with no reason";
  // Not have a string on stack? Set embedded error!
  if(!LuaBaseIsStr(lS, iIndex))
    return StrFormat("Error signalled with invalid $ reason",
      LuaBaseTypeName(lS, iIndex));
  // Get error string
  const StdString strError{ LuaUtilToCppString(lS) };
  // Remove the error string
  LuaBaseRemove(lS, iIndex);
  // return the error
  return strError;
}
/* -- Get and pop string on top -------------------------------------------- */
static StdString LuaUtilGetAndPopStr(lua_State*const lS)
  { return LuaUtilGetAndPopStr(lS, LuaBaseGetTop(lS)); }
/* -- Return if reference is valid ----------------------------------------- */
static bool LuaUtilIsRefValid(const int iReference)
  { return iReference != LUA_REFNIL; }
/* -- Return if reference is not valid ------------------------------------- */
static bool LuaUtilIsNotRefValid(const int iReference)
  { return !LuaUtilIsRefValid(iReference); }
/* -- Return reference ----------------------------------------------------- */
static void LuaUtilGetRef(lua_State*const lS, const int iReference)
  { LuaBaseRawGetI(lS, LUA_REGISTRYINDEX,
      static_cast<lua_Integer>(iReference)); }
/* -- Return referenced function ------------------------------------------- */
static bool LuaUtilGetRefFunc(lua_State*const lS, const int iReference)
{ // If context and reference are valid?
  if(lS && LuaUtilIsRefValid(iReference))
  { // Push the userdata onto the stack and return success if successful
    LuaUtilGetRef(lS, iReference);
    const int iIndex = LuaBaseGetTop(lS);
    if(LuaBaseIsFunc(lS, iIndex)) return true;
    // Failed so remove whatever it was
    LuaBaseRemove(lS, iIndex);
  } // Failure
  return false;
}
/* ------------------------------------------------------------------------- */
static StdString LuaUtilStack(lua_State*const lST)
{ // We need the root state so we can iterate through all the threads and will
  // eventually arrive at *lS as the last stack. Most of the time GetState()
  // equals to *lS anyway, just depends if it triggered in a co-routine or not.
  LuaUtilGetRef(lST, LUA_RIDX_MAINTHREAD);
  const int iTIndex = LuaBaseGetTop(lST);
  lua_State *lS = LuaBaseToThread(lST, iTIndex);
  LuaBaseRemove(lST, iTIndex);
  // Return if state is invalid. Impossible really but just incase.
  if(!lS) return "\n- Could not find main thread!";
  // list of stack traces for coroutines. They are ordered from most recent
  // call to the root call so we need to use this list to reverse them after.
  // Also we (or even Lua) does know how many total calls there has been, we
  // can only enumerate them.
  struct Debug {
    lua_State*const lS;
    lua_Debug       ldD;
    Debug(lua_State*const lNS, lua_Debug &&ldND) :
      lS(lNS), ldD{StdMove(ldND)} {}
  };
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
    if(!LuaBaseIsThread(lS, 1)) break;
    // Set parent thread
    lS = LuaBaseToThread(lS, 1);
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
          << cpVar << " = " << LuaUtilGetStackType(lSt, -1);
      // Pop the last value added by lua_getlocal
      LuaBaseRemove(lS, -1);
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
  LuaBasePushCStr(lS, eReason.what());
  return 1;
} // Don't catch all as it will catch LUA's longjmp() throw.
/* -- Push a templated function onto the stack ----------------------------- */
template<lua_CFunction cFunc>
  static void LuaUtilPushCFunc(lua_State*const lS, const int iNVals = 0)
{ LuaBasePushCFunc(lS, LuaUtilCallback<cFunc>, iNVals); }
/* -- Push the above generic error function and return its id -------------- */
static int LuaUtilPushAndGetGenericErrId(lua_State*const lS)
  { LuaUtilPushCFunc<LuaUtilErrGeneric>(lS); return LuaBaseGetTop(lS); }
/* == Generate an exception if the specified condition is false ============ */
static void LuaUtilAssert(lua_State*const lS, const bool bCond,
  const int iIndex, const char*const cpType)
{ // Return if condition is true else break execution
  if(bCond) return;
  XC("Invalid parameter!",
    "Parameter", iIndex, "Required", cpType,
    "Supplied",  LuaBaseTypeName(lS, iIndex));
}
/* -- Check that parameter is a table -------------------------------------- */
static void LuaUtilCheckTable(lua_State*const lS, const int iParam)
  { LuaUtilAssert(lS, LuaBaseIsTable(lS, iParam), iParam, "table"); }
/* -- Check that parameter is a string ------------------------------------- */
static void LuaUtilCheckStr(lua_State*const lS, const int iParam)
  { LuaUtilAssert(lS, LuaBaseIsStr(lS, iParam), iParam, "string"); }
/* -- Check that parameter is a string and is not empty -------------------- */
static void LuaUtilCheckStrNE(lua_State*const lS, const int iParam)
{ // Return if parameter is a string and not empty else break execution
  LuaUtilCheckStr(lS, iParam);
  if(LuaBaseRawLen(lS, iParam) > 0) return;
  XC("Non-empty string required!", "Parameter", iParam);
}
/* -- Get the specified string from the stack ------------------------------ */
template<typename StringType, typename StringTypePtr = const StringType*>
  static StringTypePtr LuaUtilGetStr(lua_State*const lS, const int iParam)
{ // Throw if specified parameter isn't a string else return cast
  LuaUtilCheckStr(lS, iParam);
  return LuaBaseToStr<StringType, StringTypePtr>(lS, iParam);
}
/* -- Get the specified string from the stack ------------------------------ */
template<typename StringType, typename StringTypePtr = const StringType*>
  static StringTypePtr LuaUtilGetStrNE[[maybe_unused]](lua_State*const lS,
    const int iParam)
{ // Throw if specified parameter isn't a string or empty else return cast
  LuaUtilCheckStrNE(lS, iParam);
  return LuaBaseToStr<StringType, StringTypePtr>(lS, iParam);
}
/* -- Get and return a string and throw exception if not a string ---------- */
template<typename StringType, typename StringTypePtr = const StringType*>
requires (!StdIsPointer<StringType>) && (sizeof(StringType) == sizeof(uint8_t))
  static StringTypePtr LuaUtilGetLStr(lua_State*const lS, const int iParam,
    size_t &stLen)
{ // Throw if specified parameter isn't a string else return a cast of it
  LuaUtilCheckStr(lS, iParam);
  return reinterpret_cast<StringTypePtr>(LuaBaseToLStr(lS, iParam, stLen));
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
      "Param",  iParam,                          "File",     strFile,
      "Reason", cDirBase->DirBaseVNRtoStr(vrId), "ReasonId", vrId);
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
  const int iTop = LuaBaseGetTop(lS);
  if(iCount == iTop) return;
  XC((iCount < iTop) ? "Too many arguments!" : "Not enough arguments!",
    "Supplied", iTop, "Required", iCount);
}
/* -- Check multiple functions are valid ----------------------------------- */
static void LuaUtilCheckFunc(lua_State*const) {}
template<typename ...VarArgs>
  static void LuaUtilCheckFunc(lua_State*const lS, const int iIndex,
    VarArgs &&...vaArgs)
{ LuaUtilAssert(lS, LuaBaseIsFunc(lS, iIndex), iIndex, "function");
  LuaUtilCheckFunc(lS, StdForward<VarArgs>(vaArgs)...); }
/* -- Get and return a boolean and throw exception if not a boolean -------- */
static bool LuaUtilGetBool(lua_State*const lS, const int iIndex)
{ // Throw if requested parameter isn't a boolean else return it
  LuaUtilAssert(lS, LuaBaseIsBool(lS, iIndex), iIndex, "boolean");
  return LuaBaseToBool(lS, iIndex);
}
/* -- Try to get and check a valid number not < or >= ---------------------- */
template<typename IntType>
  requires StdIsFloat<IntType>
static IntType LuaUtilGetNum(lua_State*const lS, const int iIndex)
{ // Throw if requested parameter isn't a number else return a cast of it
  LuaUtilAssert(lS, LuaBaseIsNum(lS, iIndex), iIndex, "number");
  return LuaBaseToNum<IntType>(lS, iIndex);
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
  LuaUtilAssert(lS, LuaBaseIsInt(lS, iIndex), iIndex, "integer");
  return LuaBaseToInt<IntType>(lS, iIndex);
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
/* -- Creates a new item for object ---------------------------------------- */
static LuaUtilClass *LuaUtilClassPrepNew(lua_State*const lS,
  const LuaIdent &liParent)
{ // Create userdata
  LuaUtilClass*const lucPtr =
    LuaBaseNewUData<LuaUtilClass>(lS, sizeof(LuaUtilClass));
  // Get table data from collector reference and set it as class metatable
  LuaUtilGetRef(lS, liParent.LuaIdentGetRef());
  LuaBaseSetMetaTable(lS, -2);
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
    LuaBaseNewUData<LuaUtilClass>(lS, sizeof(LuaUtilClass));
  // Get table data from collector reference and set it as class metatable
  LuaUtilGetRef(lS, ptParent->LuaIdentGetRef());
  LuaBaseSetMetaTable(lS, -2);
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
static int LuaUtilGCParam(lua_State*const lS, const int iParam, const int iVal)
  { return LuaBaseGC(lS, LUA_GCPARAM, iParam, iVal); }
/* -- Garbage collection enable generational mode -------------------------- */
static bool LuaUtilGCSetGenerational(lua_State*const lS, const bool bEnabled)
  { return LuaBaseGC(lS, LUA_GCGEN, bEnabled ? 1 : 0) != 0 ? true : false; }
/* -- Garbage collection enable incremental mode --------------------------- */
static bool LuaUtilGCSetIncremental(lua_State*const lS, const bool bEnabled)
  { return LuaBaseGC(lS, LUA_GCINC, bEnabled ? 1 : 0) != 0 ? true : false; }
/* -- Stop garbage collection ---------------------------------------------- */
static int LuaUtilGCStop(lua_State*const lS)
  { return LuaBaseGC(lS, LUA_GCSTOP); }
/* -- Start garbage collection --------------------------------------------- */
static int LuaUtilGCStart(lua_State*const lS)
  { return LuaBaseGC(lS, LUA_GCRESTART); }
/* -- Execute garbage collection ------------------------------------------- */
static int LuaUtilGCRun(lua_State*const lS)
  { return LuaBaseGC(lS, LUA_GCCOLLECT); }
/* -- Returns if garbage collection is running ----------------------------- */
static bool LuaUtilGCRunning(lua_State*const lS)
  { return LuaBaseGC(lS, LUA_GCISRUNNING) != 0; }
/* -- Get memory usage ----------------------------------------------------- */
static size_t LuaUtilGetUsage(lua_State*const lS)
{  // Calculate LUA script usage in kilobytes
  constexpr const int iKB = 1024;
  return static_cast<size_t>(
    LuaBaseGC(lS, LUA_GCCOUNT) + LuaBaseGC(lS, LUA_GCCOUNTB) / iKB) * iKB;
}
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
/* -- Standard in-sandbox call function with toggler ref ctr (unmanaged) --- */
static void LuaUtilCallFuncRefCtrEx(lua_State*const lS,
  RefCtrMaster<>*const rcmMaster, const int iParams = 0,
  const int iReturns = 0)
{ // Set a 'protect' flag and then unset it when leaving this scope
  const RefCtrSlave<> rcsProtect{ rcmMaster };
  // Do the call
  LuaBaseCall(lS, iParams, iReturns);
}
/* -- Standard in-sandbox call function (unmanaged, no params) ------------- */
static void LuaUtilCallFunc(lua_State*const lS, const int iReturns = 0)
  { LuaBaseCall(lS, 0, iReturns); }
/* -- Sandboxed call function (removes error handler) ---------------------- */
static int LuaUtilPCallExSafe(lua_State*const lS, const int iParams = 0,
  const int iReturns = 0, const int iHandler = 0)
{ // Do protected call and get result
  const int iResult = LuaBasePCall(lS, iParams, iReturns, iHandler);
  // Remove error handler from stack if handler specified
  if(iHandler) LuaBaseRemove(lS, iHandler);
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
  LuaBasePCall(lS, iParams, iReturns, iHandler)); }
/* -- If string is blank then return other string -------------------------- */
static void LuaUtilIfBlank(lua_State*const lS, const int iIndex)
{ // Check that the alternate parameter is a string
  const int iBIndex = iIndex + 1;
  LuaUtilCheckStr(lS, iBIndex);
  // Return alternate string if string to test parameter is nil
  if(!LuaBaseIsNil(lS, iIndex))
  { // Make sure var to test is a string and return alternate string if empty
    LuaUtilCheckStr(lS, iIndex);
    if(LuaBaseRawLen(lS, iIndex))
      return LuaBasePushValue(lS, iIndex);
  } // No length or nil so return alternate string
  LuaBasePushValue(lS, iBIndex);
}
/* -- Push an table with specified array size ------------------------------ */
template<typename IntType>
  static void LuaUtilPushArray(lua_State*const lS, const IntType itType)
{ LuaBasePushTableAlloc(lS, itType, 0); }
/* -- Push an table with specified object size ----------------------------- */
template<typename IntType>
  static void LuaUtilPushObject(lua_State*const lS, const IntType itType)
{ LuaBasePushTableAlloc(lS, 0, itType); }
/* -- Convert string string map to lua table and put it on stack ----------- */
static void LuaUtilToTableEx(lua_State*const lS, const auto &mctData)
{ // Return an empty table if supplied array is empty
  if(mctData.empty()) return LuaBasePushTable(lS);
  // Create the table, we're creating non-indexed key/value pairs
  LuaUtilPushObject(lS, mctData.size());
  const int iTIndex = LuaBaseGetTop(lS);
  // For each table item
  for(auto &mctPair : mctData)
  { // Push value and key name
    LuaUtilPushStr(lS, mctPair.second);
    LuaBaseSetField(lS, iTIndex, mctPair.first.data());
  }
}
/* -- Push the specified string at the specified index --------------------- */
template<typename StrType>
  requires StdIsString<StrType>
static void LuaUtilSetTableIdxStr(lua_State*const lS,
  const int iTableId, const lua_Integer liIndex, const StrType &strValue)
{ // Push at the specified index, the specified string and set it to the table
  LuaBasePushInt(lS, liIndex);
  LuaUtilPushVar(lS, strValue);
  LuaBaseRawSet(lS, iTableId);
}
/* -- Push the specified integer at the specified index -------------------- */
static void LuaUtilSetTableIdxInt(lua_State*const lS,
  const int iTableId, const lua_Integer liIndex, const auto itValue)
{ // Push at the specified index, the specified value and set it to the table
  LuaBasePushInt(lS, liIndex);
  LuaBasePushInt(lS, static_cast<lua_Integer>(itValue));
  LuaBaseRawSet(lS, iTableId);
}
/* -- Convert a directory info object and put it on stack ------------------ */
static void LuaUtilToTable(lua_State*const lS, const DirEntMap &demList)
{ // Return an empty table if supplied array is empty
  if(demList.empty()) return LuaBasePushTable(lS);
  // Create the table, we're creating a indexed/value array
  LuaUtilPushArray(lS, demList.size());
  const int iTIndex = LuaBaseGetTop(lS);
  // Entry id
  lua_Integer liIndex = 0;
  // For each table item
  for(const DirEntMapPair &dempRef : demList)
  { // Push table index
    LuaBasePushInt(lS, ++liIndex);
    // Create the sub for file info, we're creating a indexed/value array
    LuaUtilPushArray(lS, 7);
    const int iSTIndex = LuaBaseGetTop(lS);
    // Push file parts
    LuaUtilSetTableIdxStr(lS, iSTIndex, 1, // File name
      dempRef.first);
    LuaUtilSetTableIdxInt(lS, iSTIndex, 2, // Order id
      dempRef.second.Id());
    LuaUtilSetTableIdxInt(lS, iSTIndex, 3, // Size
      dempRef.second.Size());
    LuaUtilSetTableIdxInt(lS, iSTIndex, 4, // Created
      dempRef.second.Created());
    LuaUtilSetTableIdxInt(lS, iSTIndex, 5, // Updated
      dempRef.second.Written());
    LuaUtilSetTableIdxInt(lS, iSTIndex, 6, // Accessed
      dempRef.second.Accessed());
    LuaUtilSetTableIdxInt(lS, iSTIndex, 7, // Attrs
       dempRef.second.Attributes());
    // Push file data table
    LuaBaseRawSet(lS, iTIndex);
  }
}
/* -- Convert string vector to lua table and put it on stack --------------- */
static void LuaUtilToTable(lua_State*const lS, const auto &ltData)
{ // Return an empty table if supplied array is empty
  if(ltData.empty()) return LuaBasePushTable(lS);
  // Create the table, we're creating a indexed/value array
  LuaUtilPushArray(lS, ltData.size());
  const int iTIndex = LuaBaseGetTop(lS);
  // Id number for array index
  lua_Integer iIndex = 0;
  // For each table item
  for(const auto &aItem : ltData)
    LuaUtilSetTableIdxStr(lS, iTIndex, ++iIndex, aItem);
}
/* -- Explode LUA string into table ---------------------------------------- */
static void LuaUtilExplode(lua_State*const lS)
{ // Check parameters
  const StdStringView ssvStr{ LuaUtilGetCppStr(lS, 1) },
                      ssvSep{ LuaUtilGetCppStr(lS, 2) };
  // Create empty table if string invalid
  if(ssvStr.empty() || ssvSep.empty()) return LuaBasePushTable(lS);
  // Else convert whats in the string
  LuaUtilToTable(lS, TokenStrView{ ssvStr, ssvSep });
}
/* -- Explode LUA string into table ---------------------------------------- */
static void LuaUtilExplodeEx(lua_State*const lS)
{ // Check parameters
  const StdStringView ssvStr{ LuaUtilGetCppStr(lS, 1) },
                      ssvSep{ LuaUtilGetCppStr(lS, 2) };
  const size_t stMax = LuaUtilGetInt<size_t>(lS, 3);
  // Create empty table if string invalid
  if(ssvStr.empty() || ssvSep.empty() || !stMax) return LuaBasePushTable(lS);
  // Else convert whats in the string
  LuaUtilToTable(lS, TokenStrView{ ssvStr, ssvSep, stMax });
}
/* -- Convert any value to human readable string and delete it ------------- */
static void LuaUtilConvertToString(lua_State*const lS, const int iIndex)
  { LuaBaseToLStrTS(lS, iIndex); LuaBaseRemove(lS, iIndex); }
/* -- Process initial implosion a table ------------------------------------ */
static lua_Integer LuaUtilImplodePrepare(lua_State*const lS,
  const int iIndex, const int iMaxParams)
{ // Get size of table clamped since lua_rawlen returns unsigned and the
  // lua_rawgeti parameter is signed. Compare the result...
  switch(const lua_Integer liLen =
    UtilIntOrMax<lua_Integer>(LuaBaseRawLen(lS, 1)))
  { // No entries?
    case 0:
    { // Just check the separator and push a blank string
      LuaUtilCheckStr(lS, iIndex + 1);
      LuaUtilPushStr(lS, cCommon->CommonBlankStr());
      // We handled it
      break;
    } // One entry?
    case 1:
    { // Just check the separator and push the first item in the table
      LuaUtilCheckStr(lS, iIndex + 1);
      LuaBaseRawGetI(lS, iIndex, 1);
      // Get the location of the popped first item and convert it to string
      LuaUtilConvertToString(lS, iMaxParams + 1);
      // We handled it
      break;
    } // More than one entry? Caller must process this;
    default: return liLen;
  } // We handled it
  return 0;
}
/* -- Pushes an item from the specified table onto the stack --------------- */
static void LuaUtilImplodeItem(lua_State*const lS, const int iTIndex,
  const int iVIndex, const lua_Integer liIndex, StdString &strOutput,
  const char *cpStr, size_t stStr)
{ // Add separator to string
  strOutput.append(cpStr, stStr);
  // Get item from table
  LuaBaseRawGetI(lS, iTIndex, liIndex);
  // Get the string from Lua stack and save the length
  cpStr = LuaBaseToLStr(lS, iVIndex, stStr);
  // Remove item extracted from table
  LuaBaseRemove(lS, iVIndex);
  // Append to supplied string
  strOutput.append(cpStr, stStr);
}
/* -- Implode LUA table into string ---------------------------------------- */
static void LuaUtilImplode(lua_State*const lS, const int iIndex)
{ // Prepare table for implosion and return if more than 1 entry in table?
  if(const lua_Integer liLen = LuaUtilImplodePrepare(lS, iIndex, 2))
  { // Calculate indexes for separator and table value
    const int iSIndex = iIndex + 1, iVIndex = iIndex + 2;
    // Get separator
    size_t stSep;
    const char*const cpSep = LuaUtilGetLStr<char>(lS, iSIndex, stSep);
    // Write first item
    LuaBaseRawGetI(lS, iIndex, 1);
    StdString strOutput{ LuaUtilToCppString(lS, iVIndex) };
    LuaBaseRemove(lS, iVIndex);
    // Iterate through rest of table and implode the items
    for(lua_Integer liI = 2; liI <= liLen; ++liI)
      LuaUtilImplodeItem(lS, iIndex, iVIndex, liI, strOutput, cpSep, stSep);
    // Return string
    LuaUtilPushStr(lS, strOutput);
  }
}
/* -- Implode LUA table into string with table check ----------------------- */
static void LuaUtilImplodeSafe(lua_State*const lS, const int iIndex)
  { LuaUtilCheckTable(lS, iIndex); LuaUtilImplode(lS, iIndex); }
/* -- Implode LUA table into human readable string ------------------------- */
static void LuaUtilImplodeEx(lua_State*const lS, const int iIndex)
{ // Prepare table for implosion and return if more than 1 entry in table?
  if(const lua_Integer liLen = LuaUtilImplodePrepare(lS, iIndex, 3))
  { // Calculate indexes for separator and table value
    const int iS1Index = iIndex + 1,
              iS2Index = iIndex + 2,
              iVIndex = iIndex + 3;
    // Get and check separators
    size_t stSep, stSep2;
    const char
      *const cpSep = LuaUtilGetLStr<char>(lS, iS1Index, stSep),
      *const cpSep2 = LuaUtilGetLStr<char>(lS, iS2Index, stSep2);
    // Write first item
    LuaBaseRawGetI(lS, iIndex, 1);
    StdString strOutput{ LuaUtilToCppString(lS, iVIndex) };
    LuaBaseRemove(lS, iVIndex);
    // Iterator through rest of table except for last entry
    for(lua_Integer liI = 2; liI < liLen; ++liI)
      LuaUtilImplodeItem(lS, iIndex, iVIndex, liI, strOutput, cpSep, stSep);
    // If there was more than one item? StrImplode the last item
    if(liLen > 1)
      LuaUtilImplodeItem(lS, iIndex, iVIndex, liLen, strOutput, cpSep2,
        stSep2);
    // Return string
    LuaUtilPushStr(lS, strOutput);
  }
}
/* -- Implode LUA table into human readable string ------------------------- */
static void LuaUtilImplodeExSafe(lua_State*const lS, const int iIndex)
  { LuaUtilCheckTable(lS, iIndex); LuaUtilImplodeEx(lS, iIndex); }
/* -- Enumerate number of items in a table (non-indexed) ------------------- */
static lua_Unsigned LuaUtilGetKeyValTableSize(lua_State*const lS,
  const int iIndex)
{ // Number of indexed items in table
  const lua_Unsigned luIndexedCount = LuaBaseRawLen(lS, 1 + iIndex);
  // Value to remove when popped by enumeration function
  const int iVIndex = iIndex + 2;
  // Number of items in table
  lua_Unsigned luCount = 0;
  // Until there are no more items
  for(LuaBasePushNil(lS);
      LuaBaseNext(lS, iIndex);
      LuaBaseRemove(lS, iVIndex)) ++luCount;
  // Return count of key/value pairs in table
  return luCount - luIndexedCount;
}
/* -- Enumerate number of items in a table (non-indexed) ------------------- */
static lua_Unsigned LuaUtilGetKeyValTableSizeSafe(lua_State*const lS,
  const int iIndex)
{ LuaUtilCheckTable(lS, 1); return LuaUtilGetKeyValTableSize(lS, iIndex); }
/* -- Creates a shallow copy of the table at the specified stack index. ---- */
static void LuaUtilCopyShallowTable(lua_State*const lS, const int iIndex)
{ // Ensure the provided index actually points to a table.
  LuaUtilCheckTable(lS, iIndex);
  // Create the new table and place it on top of the stack.
  LuaUtilPushArray(lS, LuaBaseRawLen<int>(lS, iIndex));
  // Index of supplied table and index of table key
  const int iTIndex = iIndex + 1, iKIndex = iIndex + 2;
  // Push nil onto the stack to initialise the lua_next iteration and iterate
  // through the original table. lua_next pops the key and pushes the next
  // key-value pair.
  for(LuaBasePushNil(lS); LuaBaseNext(lS, iIndex);)
  { // Current stack state: table, new_table, key, value. lua_settable will
    // consume the key and the value. However, lua_next requires the current
    // key to remain on the stack for the next iteration. Therefore, the key
    // must be duplicated.
    LuaBasePushValue(lS, iKIndex);
    // Current stack state: table, new_table, key, value, key_copy
    // Move the duplicated key immediately below the value.
    LuaBaseInsert(lS, iKIndex);
    // Current stack state: table, new_table, key, key_copy, value
    // Assign the key-value pair to the new table.
    // This operation consumes key_copy and value, leaving the original key at
    // the top.
    LuaBaseSetTable(lS, iTIndex);
    // Current stack state: table, new_table, key
  }
}
/* -- Creates a shallow copy of the table with table check ----------------- */
static void LuaUtilCopyShallowTableSafe(lua_State*const lS, const int iIndex)
  { LuaUtilCheckTable(lS, 1); LuaUtilCopyShallowTable(lS, iIndex); }
template<typename IntType = int>
  requires StdIsIntegral<IntType>
/* -- Clear a table of key pairs ------------------------------------------- */
static void LuaUtilClearObject(lua_State*const lS, const int iIndex)
{ // Create a new table which will hold keys to delete
  LuaBasePushTable(lS);
  // Calculate index to newly added table, key name to add and value to remove
  const int iTIndex = LuaBaseGetTop(lS),
            iKIndex = iTIndex + 1,
            iVIndex = iTIndex + 2;
  // Number of keys added to the table
  int iKCount = 0;
  // Enumerate each key pair in the table
  for(LuaBasePushNil(lS); LuaBaseNext(lS, iIndex);)
  { // Copy the key name into the array
    LuaBasePushValue(lS, iKIndex);
    LuaBaseRawSetI(lS, iTIndex, ++iKCount);
    LuaBaseRemove(lS, iVIndex);
  } // For each key in the table
  for(;iKCount > 0; --iKCount)
  { // Nil out each collected key using rawset (avoids metamethods)
    LuaBaseRawGetI(lS, iTIndex, iKCount);
    LuaBasePushNil(lS);
    LuaBaseRawSet(lS, iIndex);
  } // Remove keys table we created
  LuaBaseRemove(lS, iTIndex);
} // Test: lexec a={a=1,b=2,c=3};Util.FlushObject(a);return Util.TableSize(a)
/* -- Clear a table of key pairs with check -------------------------------- */
static void LuaUtilClearObjectSafe(lua_State*const lS, const int iIndex)
  { LuaUtilCheckTable(lS, iIndex); LuaUtilClearObject(lS, iIndex); }
/* -- Clear multiple tables of key pairs with check ------------------------ */
static void LuaUtilClearObjects(lua_State*const lS, int iIndex)
  { for(const int iEnd = LuaBaseGetTop(lS); iIndex <= iEnd; ++iIndex)
      LuaUtilClearObjectSafe(lS, iIndex); }
/* -- Clear a table of indicies -------------------------------------------- */
static void LuaUtilClearArray(lua_State*const lS, const int iIndex)
{ // If table array has size (clamp to lua_Integer to be future proof).
  if(lua_Integer liSize =
    UtilIntOrMax<lua_Integer>(LuaBaseRawLen(lS, iIndex))) do
  { // Push a nil and set it to the table index
    LuaBasePushNil(lS);
    LuaBaseRawSetI(lS, iIndex, liSize);
  } // Until all indicies removed
  while(--liSize > 0);
}
/* -- Clear a table of indices with check ---------------------------------- */
static void LuaUtilClearArraySafe(lua_State*const lS, const int iIndex)
  { LuaUtilCheckTable(lS, iIndex); LuaUtilClearArray(lS, iIndex); }
/* -- Clear multiple tables of indicies with check ------------------------- */
static void LuaUtilClearArrays(lua_State*const lS, int iStart)
  { for(const int iEnd = LuaBaseGetTop(lS); iStart <= iEnd; ++iStart)
      LuaUtilClearArraySafe(lS, iStart); }
/* -- Clear a table of both key pairs and indicies ------------------------- */
static void LuaUtilClearTable(lua_State*const lS, const int iIndex)
  { LuaUtilClearArray(lS, iIndex); LuaUtilClearObject(lS, iIndex); }
/* -- Clear a table of both key pairs and indicies with check -------------- */
static void LuaUtilClearTableSafe(lua_State*const lS, const int iIndex)
  { LuaUtilCheckTable(lS, iIndex); LuaUtilClearTable(lS, iIndex); }
/* -- Clear multiple tables of both key pairs and indicies with check ------ */
static void LuaUtilClearTables(lua_State*const lS, int iStart)
  { for(const int iEnd = LuaBaseGetTop(lS); iStart <= iEnd; ++iStart)
      LuaUtilClearTableSafe(lS, iStart); }
/* -- Replace text with values from specified LUA table -------------------- */
static StdString LuaUtilReplaceMulti(lua_State*const lS,
  const StdStringView &ssvWhat, const int iIndex)
{ // Return if source string is empty?
  if(ssvWhat.empty()) return {};
  // Table for replacements
  using StrViewPair = StdPair<const StdStringView, const StdStringView>;
  using StrViewPairList = StdList<StrViewPair>;
  StrViewPairList lList;
  // Prepare table for implosion and return if more than 1 entry in table?
  if(const lua_Unsigned luiLen = LuaBaseRawLen(lS, iIndex))
  { // Must have even number of parameters
    if(luiLen % 2) XC("Array size invalid!", "Size", luiLen);
    // Iterate through the indicies in the table
    const int iVIndex = iIndex + 1;
    for(lua_Integer liIndex = 1,
                    liMax = static_cast<lua_Integer>(
                      UtilIntWillOverflow<lua_Integer>(luiLen) ?
                        StdLimits<lua_Integer>::max() - 1 : luiLen);
                    liIndex <= liMax;
                    liIndex += 2)
    { // Get key from table and convert it to string if it isn't
      LuaBaseRawGetI(lS, iIndex, liIndex);
      if(!LuaBaseIsStr(lS, iVIndex)) LuaUtilConvertToString(lS, iVIndex);
      // Record the key name
      const StdStringView ssvKey{ LuaUtilToCppString(lS, iVIndex) };
      // Done with the key (value) name
      LuaBaseRemove(lS, iVIndex);
      // Get value from table and convert it to string if it isn't
      const lua_Integer liVIndex = liIndex + 1;
      LuaBaseRawGetI(lS, iIndex, liVIndex);
      if(!LuaBaseIsStr(lS, iVIndex)) LuaUtilConvertToString(lS, iVIndex);
      // Add the recorded keyname and value into the list
      lList.push_back({ ssvKey, LuaUtilToCppString(lS, iVIndex) });
      // Done with the value (value) name
      LuaBaseRemove(lS, iVIndex);
    } // Test: lexec return Util.ReplaceEx("test",{"t",1});
  } // Until there are no more items, add value if key is a string
  else
  { // Enumerate key/value pairs
    const int iKIndex = iIndex + 1, iVIndex = iIndex + 2;
    for(LuaBasePushNil(lS); LuaBaseNext(lS, iIndex);)
    { // Convert to string if needed
      if(!LuaBaseIsStr(lS, iVIndex)) LuaUtilConvertToString(lS, iVIndex);
      // Push key and value into replacement list
      lList.push_back({ LuaUtilToCppString(lS, iKIndex),
                        LuaUtilToCppString(lS, iVIndex) });
      // Remove the value
      LuaBaseRemove(lS, iVIndex);
    } // Test: lexec return Util.ReplaceEx("test",{key="val"});
  } // Do the replacement and return the string
  return StrReplaceEx(ssvWhat, lList);
}
/* -- Replace text with values from specified LUA table with table check --- */
static StdString LuaUtilReplaceMultiSafe(lua_State*const lS,
  const StdStringView &ssvWhat, const int iIndex)
{ LuaUtilCheckTable(lS, iIndex);
  return LuaUtilReplaceMulti(lS, ssvWhat, iIndex); }
/* -- Convert map tp table ------------------------------------------------- */
template<class MapType>
  static void LuaUtilToTable(lua_State*const lS, const MapType &mtRef,
    auto &&fcbFunc)
{ // Create the table, we're creating non-indexed key/value pairs
  LuaUtilPushObject(lS, mtRef.size());
  // Get top of stack so we don't have to use pseudo index
  const int iKIndex = LuaBaseGetTop(lS) + 1;
  // Enumerate each table item
  using ValueType = typename MapType::value_type;
  for(const ValueType &vtRef : mtRef)
  { // Push value and key name
    fcbFunc(vtRef.second);
    LuaBaseSetField(lS, iKIndex, vtRef.first.data());
  }
}
/* -- Convert string/uint map to table ------------------------------------- */
static void LuaUtilToTable(lua_State*const lS, const StrUIntMap &suimRef)
  { LuaUtilToTable(lS, suimRef,
      [lS](const unsigned uValue){ LuaBasePushInt(lS, uValue); }); }
/* -- Convert string/string map to table ----------------------------------- */
static void LuaUtilToTable(lua_State*const lS, const StrNCStrMap &sncsmMap)
  { LuaUtilToTable(lS, sncsmMap,
      [lS](const StdString &strValue){ LuaUtilPushStr(lS, strValue); }); }
/* -- Convert varlist to lua table and put it on stack --------------------- */
template<class ListType>
  static ListType LuaUtilToVector(lua_State*const lS, const int iIndex,
    auto &&fcbFunc)
{ // Create the table, we're creating non-indexed key/value pairs
  LuaUtilCheckTable(lS, iIndex);
  // Get maximums
  const size_t stMax =
    UtilIntOrMax<size_t>(LuaBaseRawLen(lS, iIndex));
  const lua_Integer liMax = static_cast<lua_Integer>(stMax) + 1;
  // Get top of stack so we don't have to use pseudo index
  const int iVIndex = iIndex + 1;
  // Preallocate the table
  StdReserved<ListType> vtArray{ stMax };
  // Walk the array
  for(lua_Integer liI = 1; liI < liMax; ++liI)
  { // Get item from table
    LuaBasePushInt(lS, liI);
    LuaBaseGetTable(lS, iIndex);
    // Get the string from Lua stack and save the length
    vtArray.push_back(fcbFunc(iVIndex));
    // Remove value item from stack
    LuaBaseRemove(lS, iVIndex);
  } // Recover memory
  vtArray.shrink_to_fit();
  // Return the container
  return vtArray;
}
/* -- Convert a table of numbers to vector --------------------------------- */
template<class ListType>
  static ListType LuaUtilToNumVector(lua_State*const lS, const int iIndex)
{ using VecValType = typename ListType::value_type;
  return LuaUtilToVector<ListType>(lS, iIndex,
    [lS](const int iVIndex){
      return LuaUtilGetNum<VecValType>(lS, iVIndex); }); }
/* -- Convert a table of integers to vector -------------------------------- */
template<class ListType>
  static ListType LuaUtilToIntVector(lua_State*const lS, const int iIndex)
{ using VecValType = typename ListType::value_type;
  return LuaUtilToVector<ListType>(lS, iIndex,
    [lS](const int iVIndex){
      return LuaUtilGetInt<VecValType>(lS, iVIndex); }); }
/* -- Initialise lua and clib random number generators --------------------- */
static void LuaUtilInitRNGSeed(lua_State*const lS, const lua_Integer liSeed)
{ // Make C-Lib use the specified seed
  StdSRand(static_cast<unsigned>(liSeed));
  // Get 'math' table and get its index
  LuaBaseGetGlobal(lS, "math");
  const int iTIndex = LuaBaseGetTop(lS);
  // Get pointer to function
  LuaBaseGetField(lS, iTIndex, "randomseed");
  // Push a random seed
  LuaBasePushInt(lS, liSeed);
  // Calls randomseed(ullSeed)
  LuaBaseCall(lS, 1);
  // Removes the table 'math'
  LuaBaseRemove(lS, iTIndex);
}
/* -- Return true if lua stack can take specified more items (diff type) --- */
static bool LuaUtilIsStackAvail(lua_State*const lS, const auto itCount)
  { return UtilIntWillOverflow<int>(itCount) ? false :
      LuaBaseCheckStack(lS, static_cast<int>(itCount)); }
/* -- Push a true or false ------------------------------------------------- */
static void LuaUtilPushTrue(lua_State*const lS)
  { LuaBasePushBool(lS, true); }
static void LuaUtilPushFalse(lua_State*const lS)
  { LuaBasePushBool(lS, false); }
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
