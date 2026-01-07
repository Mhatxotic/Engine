/* == LUAREF.HPP =========================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## This class manages function callbacks for certain core engine       ## **
** ## events which should only be used witrh the main Lua thread only.    ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace ILuaFunc {                   // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace ICollector::P;         using namespace IError::P;
using namespace IIdent::P;             using namespace ILog::P;
using namespace ILuaIdent::P;          using namespace ILuaLib::P;
using namespace ILuaRef::P;            using namespace ILuaUtil::P;
using namespace IStd::P;               using namespace ISysUtil::P;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* -- LuaFunc ollector class for collector data and custom variables ------- */
CTOR_BEGIN(LuaFuncs, LuaFunc, CLHelperSafe,,,public LuaRef<1>)
/* -- LuaFunc base class --------------------------------------------------- */
class LuaFuncBase                      // Just for de-duplicating initialisers
{ /* -- Private variables -------------------------------------------------- */
  typedef array<int, 2> References;    // Type for our pair of LUA references
  /* -- Protected variables ------------------------------------- */ protected:
  References       aReferences;        // LUA variable references
  int             &iLiveReference,     // Reference to the live ref
                  &iPauseReference;    // Reference to pause ref (when paused)
  /* -- Move constructor --------------------------------------------------- */
  LuaFuncBase(LuaFuncBase &&lfbOther) :
    /* -- Initialisers ----------------------------------------------------- */
    aReferences{ StdMove(lfbOther.aReferences) }, // Move references over
    iLiveReference(aReferences[0]),    // Set reference to live reference
    iPauseReference(aReferences[1])    // Set reference to paused reference
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- Disabled constructor without registration -------------------------- */
  explicit LuaFuncBase() :
    /* -- Initialisers ----------------------------------------------------- */
    aReferences{LUA_REFNIL,LUA_REFNIL},// Reference to pause ref (when paused)
    iLiveReference(aReferences[0]),    // Set reference to live reference
    iPauseReference(aReferences[1])    // Set reference to paused reference
    /* -- No code ---------------------------------------------------------- */
    {}
};/* -- LuaFunc class ------------------------------------------------------ */
CTOR_MEM_BEGIN_CSLAVE(LuaFuncs, LuaFunc, ICHelperUnsafe),
  /* -- Base classes ------------------------------------------------------- */
  public LuaFuncBase                   // Using our LuaFunc base class
{ /* -- Get parent state --------------------------------------------------- */
  lua_State *LuaFuncGetState() const { return cParent->LuaRefGetState(); }
  /* -- Get empty function reference --------------------------------------- */
  int LuaFuncGetEmptyFunc() const { return cParent->LuaRefGetId(); }
  /* -- Returns if specified reference is NOT an empty function ------------ */
  bool LuaFuncIsNotRefEmptyFunc(const int iReference) const
    { return iReference != LuaFuncGetEmptyFunc(); }
  /* -- Safely delete a reference and reset it after ----------------------- */
  void LuaFuncRmSetRef(int &iReference, const int iNewReference=LUA_REFNIL)
  { // Return if reference not valid and unreference if not empty function
    if(LuaUtilIsNotRefValid(iReference)) return;
    if(LuaFuncIsNotRefEmptyFunc(iReference))
      LuaUtilRmRef(LuaFuncGetState(), iReference);
    // Reset the reference to the requested value
    iReference = iNewReference;
  }
  /* -- Public functions ------------------------------------------- */ public:
  void LuaFuncSwap(LuaFunc &oCref)
  { // Swap members, ident and collector iterators
    swap(iLiveReference, oCref.iLiveReference);
    swap(iPauseReference, oCref.iPauseReference);
    CollectorSwapRegistration(oCref);
  }
  /* -- Restore reference and reset saved reference if it is not set ------- */
  void LuaFuncEnable()
  { // Return if pause reference is not set
    if(LuaUtilIsNotRefValid(iPauseReference)) return;
    // Set live reference to pause reference and clear pause reference
    iLiveReference = iPauseReference;
    iPauseReference = LUA_REFNIL;
  }
  /* -- Save reference and set main reference to empty function if set ----- */
  void LuaFuncDisable()
  { // Return if pause reference is already set
    if(LuaUtilIsRefValid(iPauseReference)) return;
    // Set pause reference to live reference and live reference to empty func
    iPauseReference = iLiveReference;
    iLiveReference = LuaFuncGetEmptyFunc();
  }
  /* -- Returns the reference to this function ----------------------------- */
  int LuaFuncGet() const { return iLiveReference; }
  /* -- Returns the saved reference to this function ----------------------- */
  int LuaFuncGetSaved() const { return iPauseReference; }
  /* -- Check to see if we can add the specified number of parameters ------ */
  bool LuaFuncCheckAddParams(const size_t stParams,
    const char*const cpType) const
  { // Return if value is valid
    if(LuaUtilIsStackAvail(LuaFuncGetState(), stParams)) return true;
    // Write warning to log
    cLog->LogWarningExSafe("LuaFunc cannot add $ more $ parameters for "
      "calling '$' due to integer or potential stack overflow!",
      stParams, cpType, IdentGet());
    // Failed
    return false;
  }
  /* -- Send nothing ------------------------------------------------------- */
  static void LuaFuncParams(int&) {}
  /* -- Send string vector ------------------------------------------------- */
  template<typename ...VarArgs>
    void LuaFuncParams(int &iParams, const StrVector &svList,
      VarArgs &&...vaArgs) const
  { // If we have items
    if(!svList.empty())
    { // Make sure the number of parameters would not overflow
      if(!LuaFuncCheckAddParams(svList.size(), "vector strings")) return;
      // Convert to table on stack
      for(const string &strStr : svList)
        LuaUtilPushStr(LuaFuncGetState(), strStr);
      // Increase number of parameters
      iParams += static_cast<int>(svList.size());
    } // Next item
    LuaFuncParams(iParams, StdForward<VarArgs>(vaArgs)...);
  }
  /* ----------------------------------------------------------------------- */
  template<typename ...VarArgs>
    void LuaFuncParams(int &iParams, const string &strVal,
      VarArgs &&...vaArgs) const
  { // Make sure the number of parameters would not overflow
    if(!LuaFuncCheckAddParams(1, "string")) return;
    // Copy string to stack and process next argument
    LuaUtilPushStr(LuaFuncGetState(), strVal);
    LuaFuncParams(++iParams, StdForward<VarArgs>(vaArgs)...);
  }
  /* -- Helper function to make LUAREFDISPATCH parameters ------------------ */
#define MP(t,s,f) \
  template<typename ...VarArgs> \
    void LuaFuncParams(int &iParams, const t tValue, VarArgs &&...vaArgs) \
  { \
    if(!LuaFuncCheckAddParams(1, s)) return; \
    f(LuaFuncGetState(), tValue); \
    LuaFuncParams(++iParams, StdForward<VarArgs>(vaArgs)...); \
  }
  /* -- A function for each type ------------------------------------------- */
  MP(signed long long,   "int64",  LuaUtilPushInt)
  MP(unsigned long long, "uint64", LuaUtilPushInt)
  MP(signed int,         "int",    LuaUtilPushInt)
  MP(unsigned int,       "uint",   LuaUtilPushInt)
#if !defined(WINDOWS)
  MP(size_t,             "size_t", LuaUtilPushInt)
#endif
  MP(float,              "float",  LuaUtilPushNum)
  MP(double,             "double", LuaUtilPushNum)
  MP(bool,               "bool",   LuaUtilPushBool)
  /* -- Done with helper function ------------------------------------------ */
#undef MP
  /* -- Send a function ---------------------------------------------------- */
  void LuaFuncPushFunc() const
  { // Get referenced function and return if succeeded else break execution
    if(LuaUtilGetRefFunc(LuaFuncGetState(), iLiveReference)) return;
    XC("Pushed function is not a valid function!",
      "Name",  IdentGet(), "Value", iLiveReference,
      "Stack", LuaUtilGetVarStack(LuaFuncGetState()));
  }
  /* -- Send a function or blank ------------------------------------------- */
  void LuaFuncPushFuncOrBlank() const
  { // Get referenced function and return if succeeded
    if(LuaUtilGetRefFunc(LuaFuncGetState(), iLiveReference)) return;
    // Push empty function instead
    LuaUtilGetRefFunc(LuaFuncGetState(), cParent->LuaRefGetFunc());
  }
  /* -- Dispatch the requested variables ----------------------------------- */
  template<typename ...VarArgs>void LuaFuncDispatch(VarArgs &&...vArgs)
  { // Push the call back function
    LuaFuncPushFunc();
    // Number of parameters written. This cannot be optimised with sizeof...()
    // because we might want to add multiple entries from a list.
    int iParams = 0;
    // Push all the parameters and do the call
    LuaFuncParams(iParams, vArgs...);
    LuaUtilCallFuncEx(LuaFuncGetState(), iParams);
  }
  /* -- Dispatch the requested variables safely ---------------------------- */
  template<typename ...VarArgs>
    void LuaFuncProtectedDispatch(const int iReturns, VarArgs &&...vArgs)
      const
  { // Save stack position so we can restore it on error
    const int iStack = LuaUtilStackSize(LuaFuncGetState()),
    // Push generic error function. This needs to be cleaned up after
    // LuaUtilPCall use
    iErrorCallback = LuaUtilPushAndGetGenericErrId(LuaFuncGetState());
    // This exception block is so we can remove the error callback
    try
    { // Push the call back function
      LuaFuncPushFunc();
      // Push all the parameters sent by the caller and do the secure call
      int iParams = 0;
      LuaFuncParams(iParams, vArgs...);
      LuaUtilPCallSafe(LuaFuncGetState(), iParams, iReturns, iErrorCallback);
    } // Exception occured?
    catch(const exception&)
    { // Restore stack position because we don't know what might have added
      LuaUtilPruneStack(LuaFuncGetState(), iStack);
      // Rethrow the error
      throw;
    }
  }
  /* -- Push function and call it ------------------------------------------ */
  void LuaFuncPushAndCall() const
    { LuaFuncPushFunc(); LuaUtilCallFunc(LuaFuncGetState()); }
  /* -- De-initialise saved function --------------------------------------- */
  void LuaFuncDeInit()
    { for(int &iReference : aReferences) LuaFuncRmSetRef(iReference); }
  /* -- Set empty callbacks ------------------------------------------------ */
  void LuaFuncClearRef()
    { LuaFuncRmSetRef(iLiveReference, LuaFuncGetEmptyFunc()); }
  /* -- Set a new function ------------------------------------------------- */
  void LuaFuncSet()
  { // If last item on stack is a C function?
    if(LuaUtilIsCFunction(LuaFuncGetState(), -1))
    { // De-init old reference if it not empty function
      LuaFuncClearRef();
      // Set reference to C function
      iLiveReference = LuaUtilRefInit(LuaFuncGetState());
      if(LuaUtilIsNotRefValid(iLiveReference))
        XC("Failed to create refid to C function!",
          "Name",  IdentGet(),
          "Stack", LuaUtilGetVarStack(LuaFuncGetState()));
      // Succeeded so put in log
      cLog->LogDebugExSafe("LuaFunc allocated refid #$ for C function '$'.",
        iLiveReference, IdentGet());
    } // If last item on stack is a regular function?
    else if(LuaUtilIsFunction(LuaFuncGetState(), -1))
    { // Set reference to c function. Do NOT de-initialise empty function
      LuaFuncClearRef();
      // Set reference to regular function
      iLiveReference = LuaUtilRefInit(LuaFuncGetState());
      if(LuaUtilIsNotRefValid(iLiveReference))
        XC("Failed to create refid to function!",
          "Name",  IdentGet(),
          "Stack", LuaUtilGetVarStack(LuaFuncGetState()));
      // Succeeded so put in log
      cLog->LogDebugExSafe("LuaFunc allocated refid #$ for function '$'.",
        iLiveReference, IdentGet());
    } // Don't know what this was?
    else XC("Expected C or regular function type on stack!",
      "Name",  IdentGet(),
      "Type",  LuaUtilGetType(LuaFuncGetState(), -1),
      "Stack", LuaUtilGetVarStack(LuaFuncGetState()));
  }
  /* -- Set empty function ------------------------------------------------- */
  void LuaFuncSetEmptyFunc() { iLiveReference = LuaFuncGetEmptyFunc(); }
  /* -- Disabled constructor without registration -------------------------- */
  LuaFunc() :
    /* -- Initialisers ----------------------------------------------------- */
    ICHelperLuaFunc{ cLuaFuncs },      // Init collector class unregistered
    IdentCSlave{ cParent->CtrNext() }  // Initialise identification number
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- Move constructor --------------------------------------------------- */
  LuaFunc(LuaFunc &&lfOther) :
    /* -- Initialisers ----------------------------------------------------- */
    ICHelperLuaFunc{ cLuaFuncs, this },// Register in collector class
    IdentCSlave{ cParent->CtrNext() }, // Initialise identification number
    LuaFuncBase{ StdMove(lfOther) }    // Move base over
    /* -- Clear other references and unregister other from collector ------- */
    { lfOther.aReferences.fill(LUA_REFNIL); lfOther.CollectorUnregister(); }
  /* -- Name constructor --------------------------------------------------- */
  explicit LuaFunc(const string &strN, const bool bSet=false) :
    /* -- Initialisers ----------------------------------------------------- */
    Ident{ strN },                     // Move name of function over
    ICHelperLuaFunc{ cLuaFuncs, this },// Register in collector class
    IdentCSlave{ cParent->CtrNext() }  // Initialise identification number
    /* -- Set if requested ------------------------------------------------- */
    { if(bSet) LuaFuncSet(); }
  /* -- Name(move) constructor --------------------------------------------- */
  explicit LuaFunc(string &&strN, const bool bSet=false) :
    /* -- Initialisers ----------------------------------------------------- */
    Ident{ StdMove(strN) },            // Move name of function over
    ICHelperLuaFunc{ cLuaFuncs, this },// Register in collector
    IdentCSlave{ cParent->CtrNext() }  // Initialise identification number
    /* -- Set if requested ------------------------------------------------- */
    { if(bSet) LuaFuncSet(); }
  /* -- Destructor --------------------------------------------------------- */
  DTORHELPER(~LuaFunc,
    // If we have the parent state? Delete both refs if not empty func/set
    if(LuaFuncGetState())
      for(int iReference : aReferences)
        if(LuaUtilIsRefValid(iReference) &&
           LuaFuncIsNotRefEmptyFunc(iReference))
          LuaUtilRmRef(LuaFuncGetState(), iReference);
  )
};/* ----------------------------------------------------------------------- */
/* -- De-init state and all references ------------------------------------- */
static void LuaFuncDeInitRef()
{ // Ignore if no state or function
  if(!cLuaFuncs->LuaRefStateIsSet()) return;
  // Write to log that we're deinitialising
  cLog->LogDebugSafe("LuaFuncs de-initialising...");
  // If there are references?
  if(!cLuaFuncs->empty())
  { // DeInit all references
    cLog->LogDebugExSafe("LuaFuncs de-initialising $ engine event refids...",
      cLuaFuncs->size());
    for(LuaFunc*const lCptr : *cLuaFuncs) lCptr->LuaFuncDeInit();
    cLog->LogInfoExSafe("LuaFuncs de-initialised $ engine event refids.",
      cLuaFuncs->size());
  } // De-initialise the empty function and log the result
  cLog->LogDebugSafe(cLuaFuncs->LuaRefDeInit() ?
    "LuaFuncs de-initialised empty function refid." :
    "LuaFuncs failed to de-init empty function refid.");
  // Write to log that we're deinitialising
  cLog->LogDebugSafe("LuaFuncs de-initialised.");
}
/* -- Empty function ------------------------------------------------------- */
static int LuaFuncEmptyCFunction(lua_State*const) { return 0; }
/* -- Init with lua state -------------------------------------------------- */
static void LuaFuncInitRef(lua_State*const lS)
{ // DeInit current lua refs
  LuaFuncDeInitRef();
  // Write to log that we're deinitialising
  cLog->LogDebugSafe("LuaFuncs manager initialising...");
  // Push empty function and reference it into the lua stack
  LuaUtilPushCFunc(lS, LuaFuncEmptyCFunction);
  cLuaFuncs->LuaRefInit(lS);
  // Log result
  cLog->LogDebugExSafe("LuaFunc allocated refid #$ for empty function.",
    cLuaFuncs->LuaRefGetId());
   // If there are references?
  if(!cLuaFuncs->empty())
  { // Init all references to the empty function
    cLog->LogDebugExSafe("LuaFuncs initialising $ engine event refids...",
      cLuaFuncs->size());
    for(LuaFunc*const lCptr : *cLuaFuncs) lCptr->LuaFuncSetEmptyFunc();
    cLog->LogDebugExSafe("LuaFuncs initialised $ engine event refids.",
      cLuaFuncs->size());
  } // Write to log that we're deinitialising
  cLog->LogDebugSafe("LuaFuncs manager initialised!");
}
/* -- Disable all references and add to errors if it didn't work ----------- */
static void LuaFuncDisableAllRefs()
{ // Return if no refs to disable else disable all references
  if(cLuaFuncs->empty()) return;
  cLog->LogDebugExSafe("LuaFuncs disabling $ references...",
    cLuaFuncs->size());
  for(LuaFunc*const lCptr : *cLuaFuncs) lCptr->LuaFuncDisable();
  cLog->LogDebugExSafe("LuaFuncs disabled $ references...", cLuaFuncs->size());
}
/* -- Mass enable all references ------------------------------------------- */
static void LuaFuncEnableAllRefs()
{ // Return if no refs to enable else enable all references
  if(cLuaFuncs->empty()) return;
  cLog->LogDebugExSafe("LuaFuncs enabling $ references...", cLuaFuncs->size());
  for(LuaFunc*const lCptr : *cLuaFuncs) lCptr->LuaFuncEnable();
  cLog->LogDebugExSafe("LuaFuncs enabled $ references...", cLuaFuncs->size());
}
/* ------------------------------------------------------------------------- */
CTOR_END(LuaFuncs, LuaFunc, LUAFUNC,, LuaFuncDeInitRef(),) // Finish collector
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
