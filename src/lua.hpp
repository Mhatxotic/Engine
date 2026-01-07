/* == LUA.HPP ============================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## The LUA instance is the core scripting module that glues all the    ## **
** ## engine components together. Note that this class does not handle    ## **
** ## and inits or deinits because this has to be done later on as        ## **
** ## lua allocated objects need to destruct their objects before the     ## **
** ## actual object destructors are called.                               ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace ILua {                       // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IClock::P;             using namespace ICollector::P;
using namespace ICommon::P;            using namespace IConGraph::P;
using namespace IConsole::P;           using namespace ICrypt::P;
using namespace ICVarDef::P;           using namespace ICVar::P;
using namespace ICVarLib::P;           using namespace IError::P;
using namespace IEvtMain::P;           using namespace IFlags;
using namespace ILog::P;               using namespace ILuaDef;
using namespace ILuaCode::P;           using namespace ILuaFunc::P;
using namespace ILuaLib::P;            using namespace ILuaUtil::P;
using namespace ILuaVariable::P;       using namespace IStd::P;
using namespace IString::P;            using namespace ISystem::P;
using namespace ISysUtil::P;           using namespace ITimer::P;
using namespace IUtil::P;              using namespace Lib::Sqlite::Types;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* == Lua class ============================================================ */
class Lua;                             // Class prototype
static Lua *cLua = nullptr;            // Pointer to global class
class Lua :                            // Actual class body
  /* -- Base classes ------------------------------------------------------- */
  public ClockChrono<CoreClock>,       // Runtime clock
  private EvtMainRegAuto               // Events list to register
{ /* -- Private typedefs --------------------------------------------------- */
  typedef unique_ptr<lua_State, function<decltype(lua_close)>> LuaPtr;
  /* -- Private variables -------------------------------------------------- */
  LuaPtr           lpState;            // Lua state pointer
  bool             bExiting;           // Ending execution?
  int              iOperations,        // Default ops before timeout check
                   iStack,             // Default stack size
                   iGCPause,           // Default GC pause time
                   iGCStep;            // Default GC step counter
  lua_Integer      liSeed;             // Default seed
  /* -- References --------------------------------------------------------- */
  LuaFunc          lrMainTick,         // Main tick function callback
                   lrMainEnd,          // End function callback
                   lrMainRedraw;       // Redraw function callback
  /* -- LUA state is set or not? ------------------------------------------- */
  bool LuaStateIsSet() const { return !!LuaGetState(); }
  bool LuaStateIsNotSet() const { return !LuaStateIsSet(); }
  /* -- Resume execution --------------------------------------------------- */
  bool LuaResumeExecution()
  { // Bail if not initialised or already paused or exiting
    if(!uiLuaPaused || bExiting) return false;
    // Restore original functions
    LuaFuncEnableAllRefs();
    // Resumed
    uiLuaPaused = 0;
    // Done
    return true;
  }
  /* -- Events asking LUA to quit ------------------------------------------ */
  void LuaOnAskExit(const EvtMainEvent&)
  { // Ignore if already exiting
    if(bExiting) return;
    // Resume if paused
    LuaResumeExecution();
    // Swap end function with main function
    lrMainTick.LuaFuncDeInit();
    lrMainTick.LuaFuncSwap(lrMainEnd);
    // Now it's up to the guest to end execution with Core.Done();
    bExiting = true;
  }
  /* -- Ask LUA to tell guest to redraw ------------------------------------ */
  void LuaOnSendRedraw(const EvtMainEvent&)
  { // Lua not initialised? This may be executed before Init() via an
    // exception. For example... The CONSOLE.Init() may have raised an
    // exception. Also do not fire if paused
    if(LuaStateIsNotSet() || uiLuaPaused) return;
    // Get ending function and ignore if not a function
    lrMainRedraw.LuaFuncDispatch();
    // Say that we've finished calling the function
    cLog->LogDebugSafe("Lua finished calling redraw execution callback.");
  }
  /* -- Generic end tick to quit the engine  ------------------------------- */
  static int LuaOnGenericEndTick(lua_State*const)
    { cEvtMain->ConfirmExit(); return 0; }
  /* -- Fired when lua needs to be paused (EMC_LUA_PAUSE) ------------------ */
  void LuaOnPause(const EvtMainEvent &emeEvent)
  { // If we are paused already?
    if(uiLuaPaused++)
    { // Remind console if it was manually requested then return regardless
      if(emeEvent.eaArgs.front().Bool())
        cConsole->ConsoleAddLine(
          "Execution already paused. Type 'lresume' to continue.");
      return;
    } // Disable events if not exiting
    if(!bExiting) LuaFuncDisableAllRefs();
    // Performance is no longer a priority
    cTimer->TimerSetDelayIfZero();
    // Enable and show the console
    cConGfx->ConGfxSetVisible(true);
    cConGfx->ConGfxSetLocked(true);
    // Write to console
    cConsole->ConsoleAddLine("Execution paused. Type 'lresume' to continue.");
  }
  /* -- Fired when lua needs to be resumed (EMC_LUA_RESUME) ---------------- */
  void LuaOnResume(const EvtMainEvent&)
  { // Return if pause was not successful
    if(!LuaResumeExecution())
      return cConsole->ConsoleAddLine("Execution already in progress.");
    // Refresh originally stored delay
    cTimer->TimerSetDelay(cCVars->CVarsGetInternal<int>(APP_DELAY));
    // Disable console
    cConGfx->ConGfxSetLocked(false);
    cConGfx->ConGfxSetVisible(false);
    // Write to console
    cConsole->ConsoleAddLine("Execution resumed.");
  }
  /* -- When lua enters the specified function ----------------------------- */
  static void LuaOnInstructionCount(lua_State*const lS, lua_Debug*const)
  { // Return if timer is not timed out
    if(cTimer->TimerIsNotTimedOut()) [[likely]] return;
    // Push error message and throw error
    LuaUtilPushExtStr(lS, cCommon->CommonTimeout());
    LuaUtilErrThrow(lS);
  }
  /* -- Warning callback --------------------------------------------------- */
  static void LuaOnWarning(void*const, const char*const cpMsg, int)
    { cLog->LogWarningExSafe("(Lua) $", cpMsg); }
  /* -- Default allocator that uses malloc() ------------------------------- */
  static void *LuaDefaultAllocator(void*const, void*const vpAddr,
    size_t, size_t stSize)
  { // (Re)allocate if memory needed and return
    if(stSize) return StdReAlloc<void>(vpAddr, stSize);
    // Zero for free memory
    StdFree(vpAddr);
    // Return nothing
    return nullptr;
  }
  /* -- Get LUA state ---------------------------------------------- */ public:
  lua_State *LuaGetState() const { return lpState.get(); }
  /* -- Check if we're already exiting ------------------------------------- */
  bool LuaExiting() const { return bExiting; }
  /* -- Execute main function ---------------------------------------------- */
  void LuaExecuteMain() const { lrMainTick.LuaFuncPushAndCall(); }
  /* -- Return operations count -------------------------------------------- */
  int LuaGetOpsInterval() { return iOperations; }
  /* -- Set or clear a LUA reference (LuaFunc can't have this check) ------- */
  bool LuaSetRef(lua_State*const lS, LuaFunc &lrEvent)
  { // Must be on the main thread
    LuaStateAssert(lS);
    // Check we have the correct number of requested parameters
    LuaUtilCheckParams(lS, 1);
    // If is nil then clear it and return failure
    if(LuaUtilIsNil(lS, 1)) { lrEvent.LuaFuncClearRef(); return false; }
    // Set the function if valid
    lrEvent.LuaFuncSet();
    // Return success
    return true;
  }
  /* -- Set core event callbacks ------------------------------------------- */
  void LuaSetRedrawFunc(lua_State*const lS) { LuaSetRef(lS, lrMainRedraw); }
  void LuaSetEndFunc(lua_State*const lS) { LuaSetRef(lS, lrMainEnd); }
  void LuaSetTickFunc(lua_State*const lS) { LuaSetRef(lS, lrMainTick); }
  /* -- Request pause ----------------------------------------------------- */
  void LuaRequestPause(const bool bFromException)
    { cEvtMain->Add(EMC_LUA_PAUSE, bFromException); }
  /* -- Stop gabage collection --------------------------------------------- */
  void LuaStopGC() const
  { // Garbage collector is running?
    if(LuaUtilGCRunning(LuaGetState()))
    { // Stop garbage collector
      LuaUtilGCStop(LuaGetState());
      // Log success
      cLog->LogDebugSafe("Lua garbage collector stopped.");
    } // Garbage collector not running? Show warning in log.
    else cLog->LogWarningSafe("Lua garbage collector already stopped!");
  }
  /* -- Start garbage collection ------------------------------------------- */
  void LuaStartGC() const
  { // Garbage collector is not running?
    if(!LuaUtilGCRunning(LuaGetState()))
    { // Start garbage collector
      LuaUtilGCStart(LuaGetState());
      // Log success
      cLog->LogDebugSafe("Lua garbage collector started.");
    } // Garbage collector running? Show warning in log.
    else cLog->LogWarningSafe("Lua garbage collector already started!");
  }
  /* -- Full garbage collection while logging memory usage ----------------- */
  size_t LuaGarbageCollect() const { return LuaUtilGCCollect(LuaGetState()); }
  /* -- Checks that the state matches with main state ---------------------- */
  void LuaStateAssert(lua_State*const lS) const
  { // This function call is needed when some LUA API functions need to make
    // a reference to a variable (i.e. a callback function) on the internal
    // stack, and this cannot be done on a different (temporary) context. So
    // this makes sure that only the specified call can only be made in the
    // main LUA context.
    if(lS == LuaGetState()) return;
    // Throw error to state passed
    XC("Call not allowed in temporary contexts!");
  }
  /* -- Compile a string and display it's result --------------------------- */
  const string LuaCompileStringAndReturnResult(const string &strWhat)
  { // Save time so we can measure performance
    const ClockChrono ccExecute;
    // Save stack position. This restores the position whatever the result and
    // also cleans up the return values.
    const LuaStackSaver lssSaved{ LuaGetState() };
    // Compile the specified script from the command line
    LuaCodeCompileString(LuaGetState(), strWhat, {});
    // Move compiled function for LuaUtilPCall argument
    lua_insert(LuaGetState(), 1);
    // Call the protected function. We don't know how many return values.
    LuaUtilPCall(LuaGetState(), 0, LUA_MULTRET);
    // Scan for results
    StrList slResults;
    for(int iI = lssSaved.Value() + 1; !LuaUtilIsNone(LuaGetState(), iI); ++iI)
      slResults.emplace_back(LuaUtilGetStackType(LuaGetState(), iI));
    // Print result
    return slResults.empty() ?
      StrFormat("Request took $.",
        StrShortFromDuration(ccExecute.CCDeltaToDouble())) :
      StrFormat("Request took $ returning $: $.",
        StrShortFromDuration(ccExecute.CCDeltaToDouble()),
        StrCPluraliseNum(slResults.size(), "result", "results"),
        StrImplode(slResults, 0, ", "));
  }
  /* -- Init lua library and configuration --------------------------------- */
  void LuaSetupEnvironment()
  { // Stop the garbage collector
    LuaStopGC();
    // Init references
    LuaFuncInitRef(LuaGetState());
    // Set default end function to automatically exit the engine
    LuaUtilPushCFunc<LuaOnGenericEndTick>(LuaGetState());
    lrMainEnd.LuaFuncSet();
    // Set initial size of stack
    cLog->LogDebugExSafe("Lua $ stack size to $.",
      LuaUtilIsStackAvail(LuaGetState(), iStack) ?
        "initialised" : "could not initialise", iStack);
    // Set incremental garbage collector settings. We're not using LUA_GCGEN
    // right now as it makes everything lag so investigate it sometime.
    LuaUtilGCSet(LuaGetState(), LUA_GCINC, iGCPause, iGCStep);
    cLog->LogDebugExSafe("Lua initialised incremental gc to $:$.",
      iGCPause, iGCStep);
    // Log progress
    cLog->LogDebugSafe("Lua registering engine namespaces...");
    // Counters for logging stats
    int iGlobals      = 0,           // Number of global namespaces used
        iMembers      = 0,           // Number of static functions used
        iMethods      = 0,           // Number of class methods used
        iTables       = 0,           // Number of tables used
        iStatics      = 0,           // Number of static vars registered
        iTotalGlobals = 0,           // Number of global namespaces in total
        iTotalMembers = 0,           // Number of static functions in total
        iTotalMethods = 0,           // Number of class methods in total
        iTotalTables  = 0,           // Number of tables in total
        iTotalStatics = 0;           // Number of static vars in total
    // Init core libraries
    for(const LuaLibStatic &llRef : llsaAPI)
    { // If there is nothing to add in this namespace? Go to the next one
      if(!llRef.iLLTotal) continue;
      // Increment total statistics
      iTotalMembers += llRef.iLLCount;
      iTotalMethods += llRef.iLLMFCount;
      iTotalTables += llRef.iLLKICount;
      ++iTotalGlobals;
      // If this namespace is not allowed in the current operation mode?
      if(cSystem->SysIsCoreFlagsNotHave(llRef.cfcRequired))
      { // If we have consts list?
        if(llRef.lkiList)
        { // Number of static vars registered in this namespace
          int iStaticsNS = 0;
          // Walk through the table
          for(const LuaTable *ltPtr = llRef.lkiList; ltPtr->cpName; ++ltPtr)
            // Add to total static variables registered for this namespace
            iStaticsNS += ltPtr->iCount;
          // Add to total static variables registered
          iTotalStatics += iStaticsNS;
        } // Next namespace
        continue;
      } // Increment used statistics
      iMembers += llRef.iLLCount;
      iMethods += llRef.iLLMFCount;
      iTables += llRef.iLLKICount;
      ++iGlobals;
      // Load class creation functions
      LuaUtilPushTable(LuaGetState(), 0, llRef.iLLTotal);
      luaL_setfuncs(LuaGetState(), llRef.libList, 0);
      // Number of static vars registered in this namespace
      int iStaticsNS = 0;
      // If we have consts list?
      if(llRef.lkiList)
      { // Walk through the table
        for(const LuaTable *ltPtr = llRef.lkiList; ltPtr->cpName; ++ltPtr)
        { // Get reference to table
          const LuaTable &ltRef = *ltPtr;
          // Create a table
          LuaUtilPushTable(LuaGetState(), 0, ltRef.iCount);
          // Walk through the key/value pairs
          for(const LuaKeyInt *kiPtr = ltRef.kiList; kiPtr->cpName; ++kiPtr)
          { // Get reference to key/value pair and it to LUA
            const LuaKeyInt &kiRef = *kiPtr;
            LuaUtilPushInt(LuaGetState(), kiRef.liValue);
            LuaUtilSetField(LuaGetState(), -2, kiRef.cpName);
          } // Set field name and finalise const table
          LuaUtilSetField(LuaGetState(), -2, ltRef.cpName);
          // Add to total static variables registered for this namespace
          iStaticsNS += ltRef.iCount;
        } // Add to total static variables registered
        iStatics += iStaticsNS;
        iTotalStatics += iStaticsNS;
      } // If we have don't have member functions?
      if(!llRef.libmfList)
      { // Set this current list to global
        LuaUtilSetGlobal(LuaGetState(), llRef.strvName.data());
        // Log progress
        cLog->LogDebugExSafe(
          "- $ with $ functions and $ tables with $ values.",
          llRef.strvName, llRef.iLLCount, llRef.iLLKICount, iStaticsNS);
        // Continue
        continue;
      } // Load members into this namespace too for possible aliasing.
      luaL_setfuncs(LuaGetState(), llRef.libmfList, 0);
      // Set to global variable
      LuaUtilSetGlobal(LuaGetState(), llRef.strvName.data());
      // Pre-cache the metadata for the class and it's methods.
      LuaUtilPushTable(LuaGetState(), 0, 4);
      // Copy a reference to the table and set an internal reference to it.
      LuaUtilCopyValue(LuaGetState(), -1);
      const int iReference = LuaUtilRefInit(LuaGetState());
      if(LuaUtilIsNotRefValid(iReference))
        XC("Could not create reference to metatable!", "Name", llRef.strvName);
      llcirAPI[llRef.lciId] = iReference;
      // Push the name of the object for 'tostring()' LUA function.
      LuaUtilPushExtStr(LuaGetState(), llRef.strvName);
      LuaUtilSetField(LuaGetState(), -2, cCommon->CommonLuaName().data());
      // Set function methods so var:func() works.
      LuaUtilPushTable(LuaGetState(), 0, llRef.iLLMFCount);
      luaL_setfuncs(LuaGetState(), llRef.libmfList, 0);
      LuaUtilSetField(LuaGetState(), -2, "__index");
      // Getmetatable(x) just returns the type name for now.
      LuaUtilPushExtStr(LuaGetState(), llRef.strvName);
      LuaUtilSetField(LuaGetState(), -2, "__metatable");
      // Push garbage collector function.
      LuaUtilPushCFunc(LuaGetState(), llRef.lcfpDestroy);
      LuaUtilSetField(LuaGetState(), -2, "__gc");
      // Register the table in the global namespace.
      LuaUtilSetField(LuaGetState(), LUA_REGISTRYINDEX, llRef.strvName.data());
      // Log progress
      cLog->LogDebugExSafe(
        "- $ ($:$) with $ methods, $ functions and $ tables with $ values.",
        llRef.strvName, llRef.lciId, iReference, llRef.iLLMFCount,
        llRef.iLLCount, llRef.iLLKICount, iStaticsNS);
    } // Report summary of API usage
    cLog->LogDebugExSafe(
      "Lua registered $ of $ global namespaces...\n"
      "- $ of $ method functions are registered.\n"
      "- $ of $ member functions are registered.\n"
      "- $ of $ static tables are registered.\n"
      "- $ of $ static values are registered.\n"
      "- $ of $ functions are registered in total.\n"
      "- $ of $ variables are registered in total.",
      iGlobals,           iTotalGlobals,
      iMethods,           iTotalMethods,
      iMembers,           iTotalMembers,
      iTables,            iTotalTables,
      iStatics,           iTotalStatics,
      iMembers+iMethods,  iTotalMembers+iTotalMethods,
      iMembers+iMethods+iTables+iStatics,
      iTotalMembers+iTotalMethods+iTotalTables+iTotalStatics);
    // Load default libraries and log progress
    cLog->LogDebugSafe("Lua registering core namespaces...");
    luaL_openlibs(LuaGetState());
    cLog->LogDebugSafe("Lua registered core namespaces.");
    // Initialise random number generator and if pre-defined?
    if(liSeed)
    { // Init pre-defined seed
      LuaUtilInitRNGSeed(LuaGetState(), liSeed);
      // Warn developer/user that there is a pre-defined random seed
      cLog->LogWarningExSafe("Lua using pre-defined random seed $ (0x$$)!",
        liSeed, hex, liSeed);
    } // Use a random number instead
    else
    { // Get the new random number seed
      const lua_Integer liRandSeed = CryptRandom<lua_Integer>();
      // Set the random number seed
      LuaUtilInitRNGSeed(LuaGetState(), liRandSeed);
      // Log it
      cLog->LogDebugExSafe("Lua generated random seed $ (0x$$)!",
        liRandSeed, hex, liRandSeed);
    } // Get variables namespace
    LuaUtilGetGlobal(LuaGetState(), "Variable");
    // Create a table of the specified number of variables
    LuaUtilPushTable(LuaGetState(), 0, CVAR_MAX);
    // Enumerate cvars and if stored iterator is registered?
    for(const CVarMapIt &cvmiIt : cCVars->GetInternalListConst())
      if(cvmiIt != cCVars->GetVarListEnd())
      { // Push internal id value name
        LuaUtilClassCreate<Variable>(LuaGetState(), *cVariables)->
          InitInternal(cvmiIt);
        // Assign the id to the cvar name
        LuaUtilSetField(LuaGetState(), -2, cvmiIt->first.data());
      }
    // Push cvar id table into the core namespace
    LuaUtilSetField(LuaGetState(), -2, "Internal");
    // Remove the table
    LuaUtilRmStack(LuaGetState());
    // Log that we added the variables
    cLog->LogDebugExSafe("Lua published $ engine cvars.",  CVAR_MAX);
    // Use a timeout hook?
    if(iOperations > 0)
    { // Set the hook
      LuaUtilSetHookCallback(LuaGetState(),
        LuaOnInstructionCount, iOperations);
      // Log that it was enabled
      cLog->LogDebugExSafe("Lua timeout set to $ sec for every $ operations.",
        StrShortFromDuration(cTimer->TimerGetTimeOut(), 1), iOperations);
    } // Show a warning to say the timeout hook is disabled
    else cLog->LogWarningSafe("Lua timeout hook disabled so use at own risk!");
    // Resume garbage collector
    LuaStartGC();
    // Report completion
    cLog->LogDebugSafe("Lua environment initialised.");
    // Set start of execution timer
    CCReset();
  }
  /* -- Enter sandbox mode ------------------------------------------------- */
  template<lua_CFunction cFunc>void LuaEnterSandbox(void*const vpPtr)
  { // Push and get error callback function id
    const int iParam = LuaUtilPushAndGetGenericErrId(LuaGetState());
    // Push function and parameters and user parameter from core class
    LuaUtilPushCFunc(LuaGetState(), cFunc);
    LuaUtilPushPtr(LuaGetState(), vpPtr);
    // Call it! One parameter and no returns
    LuaUtilPCallSafe(LuaGetState(), 1, 0, iParam);
  }
  /* -- De-initialise LUA context ------------------------------------------ */
  void LuaDeInit()
  { // Return if class already initialised
    if(LuaStateIsNotSet()) return;
    // Report execution time
    cLog->LogInfoExSafe("Lua execution took $ seconds.",
      StrShortFromDuration(CCDeltaToDouble()));
    // Report progress
    cLog->LogDebugSafe("Lua sandbox de-initialising...");
    // Disable garbage collector
    LuaStopGC();
    // De-init instruction count hook?
    LuaUtilSetHookCallback(LuaGetState(), nullptr, 0);
    // DeInit references
    LuaFuncDeInitRef();
    // Close state and reset var
    lpState.reset();
    // Clear API class references
    llcirAPI.fill(LUA_REFNIL);
    // No longer paused or exited
    uiLuaPaused = 0;
    bExiting = false;
    // Report progress
    cLog->LogDebugSafe("Lua sandbox successfully deinitialised.");
  }
  /* -- Lua end execution helper ------------------------------------------- */
  bool LuaTryEventOrForce(const EvtMainCmd emcCmd)
  { // If exit event already processing?
    if(cEvtMain->ExitRequested())
    { // Log event
      cLog->LogWarningExSafe("Lua sending event $ with forced confirm exit!",
        emcCmd);
      // Change or confirm exit reason
      cEvtMain->Add(emcCmd);
      // Quit by force instead
      cEvtMain->ConfirmExit();
      // Quit forced
      return true;
    } // End lua execution
    cEvtMain->Add(emcCmd);
    // Quit requested normally
    return false;
  }
  /* -- ReInitialise LUA context ------------------------------------------- */
  bool LuaReInit()
  { // If exit event already processing? Ignore sending another event
    if(cEvtMain->ExitRequested()) return false;
    // Send the event
    cEvtMain->Add(EMC_LUA_REINIT);
    // Return success
    return true;
  }
  /* -- Initialise LUA context --------------------------------------------- */
  void LuaInit()
  { // Class initialised
    if(LuaStateIsSet()) XC("Lua sandbox already initialised!");
    // Report progress
    cLog->LogDebugSafe("Lua sandbox initialising...");
    // Create lua context and throw exception if failed. ONLY use malloc()
    // because we could sometimes interleave allocations with C++ STL and use
    // of any other allocator will cause issues.
    if(LuaPtr lpNState{ lua_newstate(LuaDefaultAllocator, this,
      CryptRandom<unsigned int>()), lua_close })
        lpState = StdMove(lpNState);
    else XC("Failed to create Lua context!");
    // Set panic and warning callbacks
    lua_atpanic(LuaGetState(), LuaUtilException);
    lua_setwarnf(LuaGetState(), LuaOnWarning, this);
    // Report initialisation with version and some important variables
    cLog->LogDebugExSafe("Lua sandbox initialised ($;$;$).",
      LUA_MINSTACK, LUAI_MAXCCALLS, MAXUPVAL);
  }
  /* -- Constructor --------------------------------------------- */ protected:
  Lua() :
    /* -- Initialisers ----------------------------------------------------- */
    EvtMainRegAuto{ cEvtMain, {        // Regster LUA events
      { EMC_LUA_ASKEXIT, bind(&Lua::LuaOnAskExit,    this, _1) },
      { EMC_LUA_PAUSE,   bind(&Lua::LuaOnPause,      this, _1) },
      { EMC_LUA_REDRAW,  bind(&Lua::LuaOnSendRedraw, this, _1) },
      { EMC_LUA_RESUME,  bind(&Lua::LuaOnResume,     this, _1) }
    } },                               // Other initialisers
    bExiting(false),                   // Not exiting
    iOperations(0),                    // Operations executed
    iStack(0),                         // Initialised later
    iGCPause(0),                       // Initialised later
    iGCStep(0),                        // Initialised later
    liSeed(0),                         // Initialised later
    lrMainTick{ "OnTick" },            // Setup main tick LUA event
    lrMainEnd{ "OnEnd" },              // Setup ending tick LUA event
    lrMainRedraw{ "OnRedraw" }         // Setup graphical redraw LUA event
    /* -- Set global pointer to static class ------------------------------- */
    { cLua = this; }
  /* -- Destructor --------------------------------------------------------- */
  DTORHELPER(~Lua, LuaDeInit())
  /* -- When operations count have changed ------------------------- */ public:
  CVarReturn SetOpsInterval(const int iCount)
    { return CVarSimpleSetIntNL(iOperations, iCount, 1); }
  /* -- Set default size of stack ------------------------------------------ */
  CVarReturn SetStack(const int iValue)
    { return CVarSimpleSetInt(iStack, iValue); }
  /* -- Set GC pause time -------------------------------------------------- */
  CVarReturn SetGCPause(const int iValue)
    { return CVarSimpleSetInt(iGCPause, iValue); }
  /* -- Set GC step -------------------------------------------------------- */
  CVarReturn SetGCStep(const int iValue)
    { return CVarSimpleSetInt(iGCStep, iValue); }
  /* -- Set GC step -------------------------------------------------------- */
  CVarReturn SetSeed(const lua_Integer liV)
    { return CVarSimpleSetInt(liSeed, liV); }
  /* -- Set debug locals on stack ------------------------------------------ */
  CVarReturn SetDebugLocals(const bool bState)
    { return CVarSimpleSetInt(bDebugLocals, bState); }
};/* ----------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
