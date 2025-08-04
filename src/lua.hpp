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
  LuaPtr           lsState;            // Lua state pointer
  bool             bExiting;           // Ending execution?
  int              iOperations;        // Default ops before timeout check
  int              iStack;             // Default stack size
  int              iGCPause;           // Default GC pause time
  int              iGCStep;            // Default GC step counter
  lua_Integer      liSeed;             // Default seed
  /* -- References ------------------------------------------------- */ public:
  LuaFunc          lrMainTick;         // Main tick function callback
  LuaFunc          lrMainEnd;          // End function callback
  LuaFunc          lrMainRedraw;       // Redraw function callback
  /* -- Generic end tick to quit the engine  ------------------------------- */
  static int OnGenericEndTick(lua_State*const)
    { cEvtMain->ConfirmExit(); return 0; }
  /* -- Return lua state --------------------------------------------------- */
  lua_State *GetState(void) const { return lsState.get(); }
  /* -- Set or clear a lua reference (LuaFunc can't have this check) ------- */
  bool SetLuaRef(lua_State*const lS, LuaFunc &lrEvent)
  { // Must be on the main thread
    StateAssert(lS);
    // Check we have the correct number of requested parameters
    LuaUtilCheckParams(lS, 1);
    // If is nil then clear it and return failure
    if(LuaUtilIsNil(lS, 1)) { lrEvent.LuaFuncClearRef(); return false; }
    // Set the function if valid
    lrEvent.LuaFuncSet();
    // Return success
    return true;
  }
  /* -- Ask LUA to tell guest to redraw ------------------------------------ */
  void SendRedraw(const EvtMainEvent&)
  { // Lua not initialised? This may be executed before Init() via an
    // exception. For example... The CONSOLE.Init() may have raised an
    // exception. Also do not fire if paused
    if(!lsState || uiLuaPaused) return;
    // Get ending function and ignore if not a function
    lrMainRedraw.LuaFuncDispatch();
    // Say that we've finished calling the function
    cLog->LogDebugSafe("Lua finished calling redraw execution callback.");
  }
  /* -- Fired when lua needs to be paused (EMC_LUA_PAUSE) ------------------ */
  void OnLuaPause(const EvtMainEvent &emeEvent)
  { // Pause execution and if paused for the first time?
    if(!PauseExecution())
    { // Performance is no longer a priority
      cTimer->TimerSetDelayIfZero();
      // Can't disable console while paused
      cConGraphics->SetCantDisable(true);
      // Write to console
      cConsole->AddLine("Execution paused. Type 'lresume' to continue.");
    } // Already paused? Remind console if it was manually requested
    else if(emeEvent.eaArgs.front().Bool())
      cConsole->AddLine(
        "Execution already paused. Type 'lresume' to continue.");
  }
  /* -- Fired when lua needs to be resumed (EMC_LUA_RESUME) ---------------- */
  void OnLuaResume(const EvtMainEvent&)
  { // Return if pause was not successful
    if(!ResumeExecution())
      return cConsole->AddLine("Execution already in progress.");
    // Refresh originally stored delay
    cTimer->TimerSetDelay(cCVars->GetInternal<unsigned int>(APP_DELAY));
    // Console can now be disabled
    cConGraphics->SetCantDisable(false);
    // Write to console
    cConsole->AddLine("Execution resumed.");
  }
  /* -- Check if we're already exiting ------------------------------------- */
  bool Exiting(void) { return bExiting; }
  /* -- Events asking LUA to quit ------------------------------------------ */
  void AskExit(const EvtMainEvent&)
  { // Ignore if already exiting
    if(bExiting) return;
    // Resume if paused
    ResumeExecution();
    // Swap end function with main function
    lrMainTick.LuaFuncDeInit();
    lrMainTick.LuaFuncSwap(lrMainEnd);
    // Now it's up to the guest to end execution with Core.Done();
    bExiting = true;
  }
  /* -- Request pause ----------------------------------------------------- */
  void RequestPause(const bool bFromException)
    { cEvtMain->Add(EMC_LUA_PAUSE, bFromException); }
  /* -- Pause execution ---------------------------------------------------- */
  unsigned int PauseExecution(void)
  { // If not paused and not exiting? Disable events
    if(!uiLuaPaused && !bExiting) LuaFuncDisableAllRefs();
    // Return current level and increment it after
    return uiLuaPaused++;
  }
  /* -- Resume execution --------------------------------------------------- */
  bool ResumeExecution(void)
  { // Bail if not initialised or already paused or exiting
    if(!uiLuaPaused || bExiting) return false;
    // Restore original functions
    LuaFuncEnableAllRefs();
    // Resumed
    uiLuaPaused = 0;
    // Done
    return true;
  }
  /* -- Stop gabage collection --------------------------------------------- */
  void StopGC(void) const
  { // Garbage collector is running?
    if(LuaUtilGCRunning(GetState()))
    { // Stop garbage collector
      LuaUtilGCStop(GetState());
      // Log success
      cLog->LogDebugSafe("Lua garbage collector stopped.");
    } // Garbage collector not running? Show warning in log.
    else cLog->LogWarningSafe("Lua garbage collector already stopped!");
  }
  /* -- Start garbage collection ------------------------------------------- */
  void StartGC(void) const
  { // Garbage collector is not running?
    if(!LuaUtilGCRunning(GetState()))
    { // Start garbage collector
      LuaUtilGCStart(GetState());
      // Log success
      cLog->LogDebugSafe("Lua garbage collector started.");
    } // Garbage collector running? Show warning in log.
    else cLog->LogWarningSafe("Lua garbage collector already started!");
  }
  /* -- Full garbage collection while logging memory usage ----------------- */
  size_t GarbageCollect(void) const { return LuaUtilGCCollect(GetState()); }
  /* -- Checks that the state matches with main state ---------------------- */
  void StateAssert(lua_State*const lS) const
  { // This function call is needed when some LUA API functions need to make
    // a reference to a variable (i.e. a callback function) on the internal
    // stack, and this cannot be done on a different (temporary) context. So
    // this makes sure that only the specified call can only be made in the
    // main LUA context.
    if(lS == GetState()) return;
    // Throw error to state passed
    XC("Call not allowed in temporary contexts!");
  }
  /* -- Compile a string and display it's result --------------------------- */
  const string CompileStringAndReturnResult(const string &strWhat)
  { // Save time so we can measure performance
    const ClockChrono ccExecute;
    // Save stack position. This restores the position whatever the result and
    // also cleans up the return values.
    const LuaStackSaver lssSaved{ GetState() };
    // Compile the specified script from the command line
    LuaCodeCompileString(GetState(), strWhat, {});
    // Move compiled function for LuaUtilPCall argument
    lua_insert(GetState(), 1);
    // Call the protected function. We don't know how many return values.
    LuaUtilPCall(GetState(), 0, LUA_MULTRET);
    // Scan for results
    StrList slResults;
    for(int iI = lssSaved.Value() + 1; !LuaUtilIsNone(GetState(), iI); ++iI)
      slResults.emplace_back(LuaUtilGetStackType(GetState(), iI));
    // Print result
    return slResults.empty() ?
      StrFormat("Request took $.",
        StrShortFromDuration(ccExecute.CCDeltaToDouble())) :
      StrFormat("Request took $ returning $: $.",
        StrShortFromDuration(ccExecute.CCDeltaToDouble()),
        StrCPluraliseNum(slResults.size(), "result", "results"),
        StrImplode(slResults, 0, ", "));
  }
  /* -- Execute main function ---------------------------------------------- */
  void ExecuteMain(void) const { lrMainTick.LuaFuncPushAndCall(); }
  /* -- When lua enters the specified function ----------------------------- */
  static void OnInstructionCount(lua_State*const lS, lua_Debug*const)
    { if(cTimer->TimerIsTimedOut()) LuaUtilPushErr(lS, "Frame timeout!"); }
  /* -- Return operations count -------------------------------------------- */
  int GetOpsInterval(void) { return iOperations; }
  /* -- Init lua library and configuration --------------------------------- */
  void SetupEnvironment(void)
  { // Stop the garbage collector
    StopGC();
    // Init references
    LuaFuncInitRef(GetState());
    // Set default end function to automatically exit the engine
    LuaUtilPushCFunc(GetState(), OnGenericEndTick);
    lrMainEnd.LuaFuncSet();
    // Set initial size of stack
    cLog->LogDebugExSafe("Lua $ stack size to $.",
      LuaUtilIsStackAvail(GetState(), iStack) ?
        "initialised" : "could not initialise", iStack);
    // Set incremental garbage collector settings. We're not using LUA_GCGEN
    // right now as it makes everything lag so investigate it sometime.
    LuaUtilGCSet(GetState(), LUA_GCINC, iGCPause, iGCStep);
    cLog->LogDebugExSafe("Lua initialised incremental gc to $:$.",
      iGCPause, iGCStep);
    // Log progress
    cLog->LogDebugSafe("Lua registering engine namespaces...");
    // Counters for logging stats
    int iCount        = 0,           // Number of global namespaces used
        iMembers      = 0,           // Number of static functions used
        iMethods      = 0,           // Number of class methods used
        iTables       = 0,           // Number of tables used
        iStatics      = 0,           // Number of static vars registered
        iTotal        = 0,           // Number of global namespaces in total
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
      ++iTotal;
      // If this namespace is not allowed in the current operation mode?
      if(cSystem->IsCoreFlagsNotHave(llRef.cfcRequired))
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
      ++iCount;
      // Load class creation functions
      LuaUtilPushTable(GetState(), 0, llRef.iLLTotal);
      luaL_setfuncs(GetState(), llRef.libList, 0);
      // Number of static vars registered in this namespace
      int iStaticsNS = 0;
      // If we have consts list?
      if(llRef.lkiList)
      { // Walk through the table
        for(const LuaTable *ltPtr = llRef.lkiList; ltPtr->cpName; ++ltPtr)
        { // Get reference to table
          const LuaTable &ltRef = *ltPtr;
          // Create a table
          LuaUtilPushTable(GetState(), 0, ltRef.iCount);
          // Walk through the key/value pairs
          for(const LuaKeyInt *kiPtr = ltRef.kiList; kiPtr->cpName; ++kiPtr)
          { // Get reference to key/value pair
            const LuaKeyInt &kiRef = *kiPtr;
            // Push the name of the item
            LuaUtilPushInt(GetState(), kiRef.liValue);
            LuaUtilSetField(GetState(), -2, kiRef.cpName);
          } // Set field name and finalise const table
          LuaUtilSetField(GetState(), -2, ltRef.cpName);
          // Add to total static variables registered for this namespace
          iStaticsNS += ltRef.iCount;
        } // Add to total static variables registered
        iStatics += iStaticsNS;
        iTotalStatics += iStaticsNS;
      } // If we have don't have member functions?
      if(!llRef.libmfList)
      { // Set this current list to global
        LuaUtilSetGlobal(GetState(), llRef.strvName.data());
        // Log progress
        cLog->LogDebugExSafe("- $ ($ members, $ tables, $ statics).",
          llRef.strvName, llRef.iLLCount, llRef.iLLKICount, iStaticsNS);
        // Continue
        continue;
      } // Load members into this namespace too for possible aliasing.
      luaL_setfuncs(GetState(), llRef.libmfList, 0);
      // Set to global variable
      LuaUtilSetGlobal(GetState(), llRef.strvName.data());
      // Pre-cache the metadata for the class and it's methods.
      LuaUtilPushTable(GetState(), 0, 4);
      // Copy a reference to the table and set an internal reference to it.
      LuaUtilCopyValue(GetState(), -1);
      const int iReference = LuaUtilRefInit(GetState());
      if(LuaUtilIsNotRefValid(iReference))
        XC("Could not create reference to metatable!",
           "Name", llRef.strvName);
      llcirAPI[llRef.lciId] = iReference;
      // Push the name of the object for 'tostring()' LUA function.
      LuaUtilPushStrView(GetState(), llRef.strvName);
      LuaUtilSetField(GetState(), -2, cCommon->CommonLuaName().c_str());
      // Set function methods so var:func() works.
      LuaUtilPushTable(GetState(), 0, llRef.iLLMFCount);
      luaL_setfuncs(GetState(), llRef.libmfList, 0);
      LuaUtilSetField(GetState(), -2, "__index");
      // Getmetatable(x) just returns the type name for now.
      LuaUtilPushStrView(GetState(), llRef.strvName);
      LuaUtilSetField(GetState(), -2, "__metatable");
      // Push garbage collector function.
      LuaUtilPushCFunc(GetState(), llRef.lcfpDestroy);
      LuaUtilSetField(GetState(), -2, "__gc");
      // Register the table in the global namespace.
      LuaUtilSetField(GetState(), LUA_REGISTRYINDEX, llRef.strvName.data());
      // Log progress
      cLog->LogDebugExSafe(
        "- $ ($ members, $ methods, $ tables and $ statics); Id:$; Ref:$.",
        llRef.strvName, llRef.iLLMFCount, llRef.iLLCount,
        llRef.iLLKICount, iStaticsNS, llRef.lciId, iReference);
    } // Report summary of API usage
    cLog->LogDebugExSafe(
      "Lua registered $ of $ global namespaces...\n"
      "- $ of $ static tables are registered.\n"
      "- $ of $ static variables are registered.\n"
      "- $ of $ member functions are registered.\n"
      "- $ of $ method functions are registered.\n"
      "- $ of $ functions are registered in total.\n"
      "- $ of $ variables are registered in total.",
      iCount,             iTotal,
      iTables,            iTotalTables,
      iStatics,           iTotalStatics,
      iMembers,           iTotalMembers,
      iMethods,           iTotalMethods,
      iMembers+iMethods,  iTotalMembers+iTotalMethods,
      iMembers+iMethods+iTables+iStatics,
      iTotalMembers+iTotalMethods+iTotalTables+iTotalStatics);
    // Load default libraries and log progress
    cLog->LogDebugSafe("Lua registering core namespaces...");
    luaL_openlibs(GetState());
    cLog->LogDebugSafe("Lua registered core namespaces.");
    // Initialise random number generator and if pre-defined?
    if(liSeed)
    { // Init pre-defined seed
      LuaUtilInitRNGSeed(GetState(), liSeed);
      // Warn developer/user that there is a pre-defined random seed
      cLog->LogWarningExSafe("Lua using pre-defined random seed $ (0x$$)!",
        liSeed, hex, liSeed);
    } // Use a random number instead
    else
    { // Get the random number seed
      const lua_Integer liRandSeed = CryptRandom<lua_Integer>();
      // Set the random number seed
      LuaUtilInitRNGSeed(GetState(), liRandSeed);
      // Log it
      cLog->LogDebugExSafe("Lua generated random seed $ (0x$$)!",
        liRandSeed, hex, liRandSeed);
    } // Get variables namespace
    LuaUtilGetGlobal(GetState(), "Variable");
    // Create a table of the specified number of variables
    LuaUtilPushTable(GetState(), 0, CVAR_MAX);
    // Enumerate cvars and if stored iterator is registered?
    for(const CVarMapIt &cvmiIt : cCVars->GetInternalList())
      if(cvmiIt != cCVars->GetVarListEnd())
      { // Push internal id value name
        LuaUtilClassCreate<Variable>(GetState(), *cVariables)->
          InitInternal(cvmiIt);
        // Assign the id to the cvar name
        LuaUtilSetField(GetState(), -2, cvmiIt->first.c_str());
      }
    // Push cvar id table into the core namespace
    LuaUtilSetField(GetState(), -2, "Internal");
    // Remove the table
    LuaUtilRmStack(GetState());
    // Log that we added the variables
    cLog->LogDebugExSafe("Lua published $ engine cvars.",  CVAR_MAX);
    // Use a timeout hook?
    if(iOperations > 0)
    { // Set the hook
      LuaUtilSetHookCallback(GetState(), OnInstructionCount, iOperations);
      // Log that it was enabled
      cLog->LogDebugExSafe("Lua timeout set to $ sec for every $ operations.",
        StrShortFromDuration(cTimer->TimerGetTimeOut(), 1), iOperations);
    } // Show a warning to say the timeout hook is disabled
    else cLog->LogWarningSafe("Lua timeout hook disabled so use at own risk!");
    // Resume garbage collector
    StartGC();
    // Report completion
    cLog->LogDebugSafe("Lua environment initialised.");
    // Set start of execution timer
    CCReset();
  }
  /* -- Enter sandbox mode ------------------------------------------------- */
  void EnterSandbox(lua_CFunction cbFunc, void*const vpPtr)
  { // Push and get error callback function id
    const int iParam = LuaUtilPushAndGetGenericErrId(GetState());
    // Push function and parameters
    LuaUtilPushCFunc(GetState(), cbFunc);
    // Push user parameter (core class)
    LuaUtilPushPtr(GetState(), vpPtr);
    // Call it! No parameters and no returns
    LuaUtilPCallSafe(GetState(), 1, 0, iParam);
  }
  /* -- De-initialise LUA context ------------------------------------------ */
  void DeInit(void)
  { // Return if class already initialised
    if(!lsState) return;
    // Report execution time
    cLog->LogInfoExSafe("Lua execution took $ seconds.",
      StrShortFromDuration(CCDeltaToDouble()));
    // Report progress
    cLog->LogDebugSafe("Lua sandbox de-initialising...");
    // De-init instruction count hook?
    LuaUtilSetHookCallback(GetState(), nullptr, 0);
    // Disable garbage collector
    StopGC();
    // DeInit references
    LuaFuncDeInitRef();
    // Close state and reset var
    lsState.reset();
    // Clear API class references
    llcirAPI.fill(LUA_REFNIL);
    // No longer paused or exited
    uiLuaPaused = 0;
    bExiting = false;
    // Report progress
    cLog->LogDebugSafe("Lua sandbox successfully deinitialised.");
  }
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
  /* -- Warning callback --------------------------------------------------- */
  static void WarningCallback(void*const, const char*const cpMsg, int)
    { cLog->LogWarningExSafe("(Lua) $", cpMsg); }
  /* -- Lua end execution helper ------------------------------------------- */
  bool TryEventOrForce(const EvtMainCmd emcCmd)
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
  bool ReInit(void)
  { // If exit event already processing? Ignore sending another event
    if(cEvtMain->ExitRequested()) return false;
    // Send the event
    cEvtMain->Add(EMC_LUA_REINIT);
    // Return success
    return true;
  }
  /* -- Initialise LUA context --------------------------------------------- */
  void Init(void)
  { // Class initialised
    if(lsState) XC("Lua sandbox already initialised!");
    // Report progress
    cLog->LogDebugSafe("Lua sandbox initialising...");
    // Create lua context and bail if failed. ONLY use malloc() because we
    // could sometimes interleave allocations with C++ STL and use of any other
    // allocator will cause issues.
    lsState = LuaPtr{ lua_newstate(LuaDefaultAllocator, this), lua_close };
    if(!lsState) XC("Failed to create Lua context!");
    // Set panic callback
    lua_atpanic(GetState(), LuaUtilException);
    // Set warning catcher
    lua_setwarnf(GetState(), WarningCallback, this);
    // Report initialisation with version and some important variables
    cLog->LogDebugExSafe("Lua sandbox initialised...\n"
      "- Stack size minimum: $; Stack size maximum: $.\n"
      "- Maximum CFunction calls: $; Maximum UpValues: $.",
      LUA_MINSTACK, LUAI_MAXSTACK, LUAI_MAXCCALLS, MAXUPVAL);
  }
  /* -- Destructor ---------------------------------------------- */ protected:
  DTORHELPER(~Lua, DeInit())
  /* -- Constructor -------------------------------------------------------- */
  Lua(void) :
    /* --------------------------------------------------------------------- */
    EvtMainRegAuto{ cEvtMain, {        // Regster LUA events
      { EMC_LUA_ASK_EXIT, bind(&Lua::AskExit,     this, _1) },
      { EMC_LUA_PAUSE,    bind(&Lua::OnLuaPause,  this, _1) },
      { EMC_LUA_REDRAW,   bind(&Lua::SendRedraw,  this, _1) },
      { EMC_LUA_RESUME,   bind(&Lua::OnLuaResume, this, _1) }
    } },                               // End of redraw event data
    bExiting(false),                   // Not exiting
    iOperations(0),                    // No operations
    iStack(0),                         // No stack
    iGCPause(0),                       // No GC pause
    iGCStep(0),                        // No GC step
    liSeed(0),                         // Random seed
    lrMainTick{ "MainTick" },          // Main tick event
    lrMainEnd{ "EndTick" },            // End tick event
    lrMainRedraw{ "OnRedraw" }         // Redraw event
    /* -- Set global pointer to static class ------------------------------- */
    { cLua = this; }
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
