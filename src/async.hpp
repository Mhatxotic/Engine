/* == ASYNC.HPP ============================================================ **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## Helps with loading data asynchronously and handle parallelisation.  ## **
** ## Do not use class directly. It must be derived with another class.   ## **
** ######################################################################### **
** ## This class requires the parent class to have these functions...     ## **
** ##   void AsyncReady(FileMap &fmData)                                  ## **
** ##     - Fired when file is loading from memory (data block)           ## **
** ######################################################################### **
** ## It also requires the parent class' collector class to also register ## **
** ## the supplied event command argument prior to using. The event must  ## **
** ## point to the EventCallback function in this class.                  ## **
** ######################################################################### **
** ## Also do swap members in this class in anyway, it causes corruption! ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* -- Prototypes ----------------------------------------------------------- */
namespace IAsset {                     // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IFileMap::P;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* -- Prototypes ----------------------------------------------------------- */
static FileMap AssetExtract(const string&);      // Extract files from archives
static FileMap AssetLoadFromDisk(const string&); // Load file from disk
static size_t AssetGetPipeBufferSize(void);      // Get pipe buffer size
/* ------------------------------------------------------------------------- */
}                                      // End of public namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private namespace
/* ------------------------------------------------------------------------- */
namespace IASync {                     // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IAsset::P;             using namespace IClock::P;
using namespace ICollector::P;         using namespace IError::P;
using namespace IEvtMain::P;           using namespace IFileMap::P;
using namespace IIdent::P;             using namespace ILog::P;
using namespace ILuaEvt::P;            using namespace ILuaUtil::P;
using namespace IMemory::P;            using namespace IStd::P;
using namespace IString::P;            using namespace ISystem::P;
using namespace ISysUtil::P;           using namespace IThread::P;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* ------------------------------------------------------------------------- */
enum ASyncProgressCommand : unsigned int // For lua callback events
{ /* ----------------------------------------------------------------------- */
  APC_EXECSTART,                       // Exec started (2=Pid)
  APC_EXECDATAWRITE,                   // Exec writing data (2=Pos,3=Total)
  APC_FILESTART,                       // File opened (2=Siz,3=MTime,4=CTime)
};/* ----------------------------------------------------------------------- */
template<class MemberType, class ColType>class AsyncLoader :
  /* -- Base classes ------------------------------------------------------- */
  public Memory                        // Loading from memory data (reusable)
{ /* -- Private typedefs ------------------------------------------ */ private:
  enum AsyncResult : unsigned int      // Async loading results
  { /* --------------------------------------------------------------------- */
    AR_STANDBY,                        // Loading has not started
    AR_LOADING,                        // Resource is loading
    AR_SUCCESS,                        // Loading succeeded
    AR_SUCCESS_PARAM,                  // Loading succeeded (param supplied)
    AR_ABORT,                          // Loading aborted
    AR_ERROR,                          // Loading failed
    AR_UNKNOWN                         // Unknown code
  }; /* -------------------------------------------------------------------- */
  enum ASyncCmdType                    // Loader content type
  { /* --------------------------------------------------------------------- */
    BA_NONE,                           // No event
    BA_FILE,                           // Load from file
    BA_MEMORY,                         // Load from memory
    BA_EXECUTE                         // Execute command-line
  }; /* -------------------------------------------------------------------- */
  enum ASyncCallback                   // Loader content type
  { /* --------------------------------------------------------------------- */
    LR_CLASS,                          // [0] Ref to the pointer to class
    LR_SUCCESS,                        // [1] Ref to the success callback
    LR_PROGRESS,                       // [2] Ref to the progress callback
    LR_ERROR,                          // [3] Ref to the error callback
    LR_MAX                             // [4] Maximum lua references
  };/* --------------------------------------------------------------------- */
  typedef LuaEvtSlave<MemberType,LR_MAX> LuaEvtCallback;
  /* -- Initialised in parent class ---------------------------------------- */
  MemberType      &mtAsyncOwner;       // Parent collector that owns this
  EvtMainCmd       emcAsyncCmd;        // Event command to fire on load
  /* -- Private variables -------------------------------------------------- */
  Ident           &idName;             // Reference to name of object
  LuaEvtCallback   lecAsync;           // Async state and references
  Thread           tAsyncThread;       // Asynchronous loading
  string           strAsyncError;      // The last error exception
  ASyncCmdType     asctAsyncType;      // Async load init type
  SafeUInt         uiAsyncPid;         // Pid of executing process
  /* -- Dispatch event without parameter ----------------------------------- */
  void AsyncCompletionDispatch(const AsyncResult arResult)
  { // Signal events handler to execute event callback on the next frame.
    // Add the event to the event store so we can remove the event when the
    // destructor is called before the event is called.
    lecAsync.LuaEvtsDispatch(emcAsyncCmd,
      reinterpret_cast<void*>(&mtAsyncOwner), arResult);
  }
  /* -- Send completion ---------------------------------------------------- */
  void AsyncCompletion(void)
  { // Dispatch the result of the asynchronous event. Send a cancel instead if
    // the thread wants to terminate.
    AsyncCompletionDispatch(tAsyncThread.ThreadShouldExit() ?
      AR_ABORT : AR_SUCCESS);
  }
  /* -- Send completion ---------------------------------------------------- */
  void AsyncCompletionWithParam(const int64_t qwParam)
  { // If thread was asked to abort? Dispatch the asynchronous event and return
    if(tAsyncThread.ThreadShouldExit())
      return AsyncCompletionDispatch(AR_ABORT);
    // Dispatch the asynchronous event with parameter. Unforunately we have to
    // cast any value we get into int64 because we read as int64 at the event
    // handling routine, unless we send a type as well which probably isn't
    // worth the effort!
    lecAsync.LuaEvtsDispatch(emcAsyncCmd,
      reinterpret_cast<void*>(&mtAsyncOwner), AR_SUCCESS_PARAM, qwParam);
  }
  /* -- Send progress event ------------------------------------------------ */
  template<typename ...VarArgs>
    void AsyncProgress(const ASyncProgressCommand apcCmd,
      const VarArgs &...vaVars)
  { lecAsync.LuaEvtsDispatch(emcAsyncCmd,
      reinterpret_cast<void*>(&mtAsyncOwner), AR_LOADING,
      static_cast<uint64_t>(apcCmd), vaVars...); }
  /* -- Write to pipe assistant -------------------------------------------- */
  void AsyncWriteToPipe(SysPipe &spProcess, size_t &stPosition,
    const size_t stBuffer)
  { // Write a block of data and get how much we wrote. We hard fail if we did
    // not write the correct number of bytes
    const size_t stWrote = spProcess.Send(MemRead(stPosition), stBuffer);
    if(stWrote != stBuffer)
      XCS("Not enough data written to process pipe!",
        "Ident",    spProcess.IdentGet(), "Pid",    spProcess.GetPid(),
        "Expected", stBuffer,             "Actual", stWrote,
        "Position", stPosition);
    // Move forward
    stPosition += stBuffer;
    // Send progress update
    AsyncProgress(APC_EXECDATAWRITE,
      static_cast<uint64_t>(stPosition), static_cast<uint64_t>(MemSize()));
  }
  /* ----------------------------------------------------------------------- */
  void AsyncTidyUpVars(void)
  { // Clear Lua stack string and recover memory as we are done with it. If
    // the member class wants to reuse this string now they can.
    strAsyncError.clear();
    strAsyncError.shrink_to_fit();
    // Clear other members
    asctAsyncType = BA_NONE;
    uiAsyncPid = 0;
    // The 'Memory' used from the loading would have already been transferred
    // to the 'FileMap' class on the callback so it will already be empty now.
    // Also the member controls the 'Ident' class so it's up to them to manage
    // it.
  }
  /* -- Get thread ---------------------------------------------- */ protected:
  bool AsyncThreadIsCurrent(void) const
    { return tAsyncThread.ThreadIsCurrent(); }
  /* -- Prepend message to the full stack message -------------------------- */
  void AsyncDoAddStackMessage(const char*const cpMsg)
    { strAsyncError.insert(0, cpMsg); }
  void AsyncDoAddStackMessage(const string &strMsg)
    { strAsyncError.insert(0, strMsg); }
  /* -- Execute function (returns exit code) ----------------------- */ public:
  int64_t AsyncExecute(void)
  { // Duration of execution
    const ClockChrono<> ccExecute;
    // Open pipe to application
    SysPipe spProcess{ idName.IdentGet() };
    // Record pid. This is so the destructor can kill it on exit
    uiAsyncPid = spProcess.GetPid();
    // Send progress event
    AsyncProgress(APC_EXECSTART, static_cast<uint64_t>(uiAsyncPid.load()));
    // If we have data to send?
    if(MemIsNotEmpty())
    { // Current position
      size_t stPosition = 0;
      // Buffer size to write (maybe configurable todo)
      const size_t stBuffer = AssetGetPipeBufferSize(),
        // Get amount of extra data to write
        stExtra = MemSize() % stBuffer,
        // Clamp end position to number of pages
        stEnd = MemSize() < stBuffer ? MemSize() : MemSize() - stExtra;
      // If we are writing more than one page? Copy full pages of buffers
      // until we are left with remainders.
      if(stExtra > stBuffer)
        while(stPosition < stEnd && tAsyncThread.ThreadShouldNotExit())
          AsyncWriteToPipe(spProcess, stPosition, stBuffer);
      // If theres remainder to write then write that too.
      if(stExtra) AsyncWriteToPipe(spProcess, stPosition, stExtra);
      // Close the writing portion of the pipe
      spProcess.SendFinish();
      // Report that we write data
      cLog->LogInfoExSafe("Async pipe wrote $ bytes to stdin of pid $ in $!",
        stPosition, spProcess.GetPid(),
        StrShortFromDuration(ccExecute.CCDeltaToDouble()));
      // Clear the memory to re-use for output
      MemDeInit();
    } // List of output
    MemoryList mlBlocks;
    size_t stTotalRead = 0;
    // Until the thread says we should exit
    while(tAsyncThread.ThreadShouldNotExit())
    { // Read process output and break out of loop if nothing read
      Memory mBuffer{ spProcess.ReadBlock() };
      if(mBuffer.MemIsEmpty()) break;
      // Increase total bytes read
      stTotalRead += mBuffer.MemSize();
      // Move it into the packet list
      mlBlocks.emplace_back(StdMove(mBuffer));
    } // Finished with pipe. Also gets the exit code too.
    spProcess.Finish();
    // Pid no longer valid
    uiAsyncPid = 0;
    // Log pipe result.
    cLog->LogInfoExSafe(
      "Async pipe returned $ bytes and exit code $ from pid $ in $!",
      stTotalRead, spProcess.SysPipeBaseGetStatus(), spProcess.GetPid(),
      StrShortFromDuration(ccExecute.CCDeltaToDouble()));
    // If we have no memory stored? Return exit code
    if(!stTotalRead) return spProcess.SysPipeBaseGetStatus();
    // Initialise memory for program output data block
    MemInitBlank(stTotalRead);
    // Byte offset counter
    size_t stOffset = 0;
    // Loop until...
    do
    { // Get packet memory block and copy it into our dest memory block
      const MemConst &dcPacket = mlBlocks.front();
      MemWriteBlock(stOffset, dcPacket);
      // Increment counter
      stOffset += dcPacket.MemSize();
      // Pop packet
      mlBlocks.pop_front();
    } // ...list is fully emptied
    while(!mlBlocks.empty());
    // Return exit code
    return spProcess.SysPipeBaseGetStatus();
  }
  /* -- Process a file map and dispatch events ----------------------------- */
  void AsyncParseFileMap(FileMap &fmData)
  { // Send progress event with the file size
    AsyncProgress(APC_FILESTART,
      static_cast<uint64_t>(fmData.MemSize()),
      static_cast<uint64_t>(fmData.FileMapModifiedTime()),
      static_cast<uint64_t>(fmData.FileMapCreationTime()));
    // Move our memory block into a file map and send it to loader
    mtAsyncOwner.AsyncReady(fmData);
  }
  /* -- Move the stored memory block into a new filemap and parse it ------- */
  void AsyncParseMemory(void)
    { FileMap fmData{ idName.IdentGet(), StdMove(*this), cmSys.GetTimeS() };
      AsyncParseFileMap(fmData); }
  /* -- Load the specified file and parse it ------------------------------- */
  void AsyncParseFile(void)
    { FileMap fmData{ AssetExtract(idName.IdentGet()) };
      AsyncParseFileMap(fmData); }
  /* -- Async off-main thread function ------------------------------------- */
  int AsyncThreadMain(Thread&)
  { // Capture exceptions. Remember that after these operations the memory at
    // mAsyncLoadData should be de-initialised. So don't use it again.
    try
    { // How are we loading?
      switch(asctAsyncType)
      { // Do nothing?
        case BA_NONE:
        { // Just send a blank file and signal completion
          FileMap fmData;
          mtAsyncOwner.AsyncReady(fmData);
          AsyncCompletion();
          break;
        } // Loading from a file?
        case BA_FILE:
        { // Open and load the file and signal completion
          AsyncParseFile();
          AsyncCompletion();
          break;
        } // Execute as command line?
        case BA_EXECUTE:
        { // Perform command line execution and send return code to handler
          const int64_t qwResult = AsyncExecute();
          AsyncParseMemory();
          AsyncCompletionWithParam(qwResult);
          break;
        } // Load from memory?
        case BA_MEMORY:
        { // Parse the memory block and signal completion
          AsyncParseMemory();
          AsyncCompletion();
          break;
        } // Unknown asset load type
        default: XC("Internal error: Unknown asset load type!",
                    "Type", asctAsyncType);
      } // Return success to thread manager
      return 1;
    } // exception occured?
    catch(const exception &eReason)
    { // Prepend the reason incase there are nested exceptions
      AsyncDoAddStackMessage(eReason.what());
      // Signal completion
      AsyncCompletionDispatch(AR_ERROR);
      // Report error in log
      cLog->LogErrorExSafe("(ASYNCLOADER THREAD EXCEPTION) $", strAsyncError);
      // Return failure to thread manager
      return -1;
    }
  }
  /* ----------------------------------------------------------------------- */
  void AsyncInit(lua_State*const lsS, const string &strIdentifier,
    const string &strL)
  { // Set the filename
    idName.IdentSet(strIdentifier);
    // Parse the class, error and success functions.
    lecAsync.LuaEvtInitEx(lsS);
    // Save the current stack because if an error occurs asynchronously, the
    // event subsystem executes the callback and will be empty.
    strAsyncError = StdMove(LuaUtilStack(lsS));
    // Begin async thread
    tAsyncThread.ThreadInit(StrAppend(strL, ':', strIdentifier),
      bind(&AsyncLoader<MemberType,ColType>::AsyncThreadMain,
        this, _1), this);
  }
  /* ----------------------------------------------------------------------- */
  void AsyncStop(void)
  { // If a pid is set then we need to kill it
    if(uiAsyncPid) { cSystem->TerminatePid(uiAsyncPid); uiAsyncPid = 0; }
    // Wait for thread to terminate
    tAsyncThread.ThreadDeInit();
  }
  /* -- Async do protected dispatch (assumes params already on lua stack) -- */
  void AsyncDoLuaThrowErrorHandler(const EvtMainEvent &emeEvent)
  { // Get error function callback
    if(lecAsync.LuaRefGetFunc(LR_ERROR))
    { // Push the error message
      LuaUtilPushStr(lecAsync.LuaRefGetState(), strAsyncError);
      // Wait for thread and register the class
      AsyncStopAndRegister();
      // Now do the callback. An exception could occur here.
      LuaUtilCallFuncEx(lecAsync.LuaRefGetState(), 1);
    } // Invalid userdata?
    else cLog->LogErrorExSafe("AsyncLoader got invalid params in failure "
      "event $ for '$' with luastate($) and fref($) from $ params!",
      emeEvent.cCmd, idName.IdentGet(), lecAsync.LuaRefStateIsSet(),
      lecAsync.LuaRefGetFunc(LR_ERROR), emeEvent.aArgs.size());
    // The memory for the error is no longer needed
    AsyncTidyUpVars();
  }
  /* -- Async do protected dispatch (assumes params already on lua stack) -- */
  void AsyncDoLuaProtectedDispatch(const EvtMainEvent &emeEvent,
    const int iParams, const int iHandler)
  { // Compare error code
    switch(LuaUtilPCallExSafe(lecAsync.LuaRefGetState(), iParams, 0, iHandler))
    { // No error so remove error handler value and return
      case LUA_OK: return;
      // Run-time error
      case LUA_ERRRUN:
        AsyncDoAddStackMessage(StrAppend("Async runtime error! > ",
          LuaUtilGetAndPopStr(lecAsync.LuaRefGetState()))); break;
      // Memory allocation error
      case LUA_ERRMEM:
        AsyncDoAddStackMessage("Async memory allocation error!"); break;
      // Error + error in error handler
      case LUA_ERRERR:
        AsyncDoAddStackMessage("Error in async error handler!"); break;
      // Unknown error
      default:
        AsyncDoAddStackMessage("Unknown error in async callback!"); break;
    } // Throw error back to user error handler
    AsyncDoLuaThrowErrorHandler(emeEvent);
  }
  /* -- Waits for the thread to finish and registers the class ------------- */
  void AsyncStopAndRegister(void)
  { // Wait for the thread to terminate if it is still running
    AsyncStop();
    // Register the class from the requested collector type
    static_cast<ColType&>(mtAsyncOwner).CollectorRegister();
  }
  /* -- Async do protected call dispatams already pushed onto lua stack) --- */
  void AsyncDoFinishLuaProtectedDispatch(const EvtMainEvent &emeEvent,
    const int iParam, const int iHandler)
  { // Wait for thread and register the class
    AsyncStopAndRegister();
    // Now do the callback. An exception could occur here as well and it
    // should be protected.
    AsyncDoLuaProtectedDispatch(emeEvent, iParam, iHandler);
  }
  /* --------------------------------------------------------------- */ public:
  void AsyncWait(void) { tAsyncThread.ThreadWait(); }
  /* ----------------------------------------------------------------------- */
  void AsyncCancel(void)
  { // Wait for the thread to stop
    AsyncStop();
    // Unreference the class
    lecAsync.LuaEvtDeInit();
  }
  /* -- The thread only calls LoadData() and thats it ---------------------- */
  void AsyncInitNone(lua_State*const lsS, const string &strIdentifier,
    const string &strLabel)
  { // Loading from memory
    asctAsyncType = BA_NONE;
    // Init rest of members
    AsyncInit(lsS, strIdentifier, strLabel);
  }
  /* -- The thread loads from a filemap and calls LoadData() --------------- */
  void AsyncInitFile(lua_State*const lsS, const string &strFilename,
    const string &strLabel)
  { // Loading from file
    asctAsyncType = BA_FILE;
    // Init rest of members
    AsyncInit(lsS, strFilename, strLabel);
  }
  /* -- The thread loads from the command-line ----------------------------- */
  void AsyncInitCmdLine(lua_State*const lsS, const string &strCmdLine,
    const string &strLabel, Memory &mbInput)
  { // Loading from command-line
    asctAsyncType = BA_EXECUTE;
    // Set memory sent to stdin
    MemSwap(mbInput);
    // Init rest of members
    AsyncInit(lsS, strCmdLine, strLabel);
  }
  /* -- The thread loads from existing memory and calls LoadData() --------- */
  void AsyncInitArray(lua_State*const lsS, const string &strIdentifier,
    const string &strLabel, Memory &mData)
  { // Set data to load from
    MemSwap(mData);
    // Loading from memory
    asctAsyncType = BA_MEMORY;
    // Init rest of members
    AsyncInit(lsS, strIdentifier, strLabel);
  }
  /* -- Send load data update and register in collector -------------------- */
  void SyncLoadDataAndRegister(FileMap &fmData) const
  { // Send file map to derived class
    mtAsyncOwner.AsyncReady(fmData);
    // Load succeeded so register the block.
    static_cast<ColType&>(mtAsyncOwner).CollectorRegister();
  }
  /* -- Init from file synchronously from disk only ------------------------ */
  void SyncInitFileDisk(const string &strFilename) const
  { // Set filename
    idName.IdentSet(strFilename);
    // Load from specified file
    FileMap fmData{ AssetLoadFromDisk(strFilename) };
    // Send to derived class and register
    SyncLoadDataAndRegister(fmData);
  }
  /* -- Init from file synchronously --------------------------------------- */
  void SyncInitFile(const string &strFilename) const
  { // Set filename
    idName.IdentSet(strFilename);
    // Load from specified file
    FileMap fmData{ AssetExtract(strFilename) };
    // Send to derived class and register
    SyncLoadDataAndRegister(fmData);
  }
  /* -- Init from array synchronously -------------------------------------- */
  void SyncInitArray(const string &strIdentifier, Memory &mData) const
  { // Set identifier
    idName.IdentSet(strIdentifier);
    // Put the memory block into a file map and load the block
    FileMap fmData{ strIdentifier, StdMove(mData), cmSys.GetTimeS() };
    // Send to derived class and register
    SyncLoadDataAndRegister(fmData);
  }
  /* -- Init from file synchronously with filename checking ---------------- */
  void SyncInitFileSafe(const string &strFilename) const
  { // Set filename
    idName.IdentSet(strFilename);
    // Load from specified file from archives or disk
    FileMap fmData{ AssetExtract(strFilename) };
    // Send to derived class and register
    SyncLoadDataAndRegister(fmData);
  }
  /* -- Async thread event callback (called by LuaEvtMaster) ------------- */
  void LuaEvtCallbackAsync(const EvtMainEvent &emeEvent) try
  { // Get reference to string vector and we need three parameters
    // [0]=Pointer to socket class, [1]=Event list id, [2]=Status
    const EvtMainArgs &emaArgs = emeEvent.aArgs;
    // Event result
    const AsyncResult uiAsyncResult =
      lecAsync.template LuaEvtsCheckParams<3>(emaArgs) ?
        static_cast<AsyncResult>(emaArgs[2].lui) : AR_UNKNOWN;
    // If lua is not paused?
    if(!uiLuaPaused)
    { // Whats the code sent in the event?
      switch(uiAsyncResult)
      { // Unknown operation?
        case AR_UNKNOWN:
        { // Log the invalid parameter count
          cLog->LogWarningExSafe("AsyncLoader parameter mismatch of $ in "
            "event $ for '$' with status $!", emaArgs.size(),
            emeEvent.cCmd, idName.IdentGet(), uiAsyncResult);
          // Done
          break;
        } // The operation is still loading?
        case AR_LOADING:
        { // Push and get error callback function id
          const int iErrorHandler =
            LuaUtilPushAndGetGenericErrId(lecAsync.LuaRefGetState());
          // Push the progress callback and if succeeded?
          if(lecAsync.LuaRefGetFunc(LR_PROGRESS))
          { // Push the sent parameters onto the stack
            const size_t stMax = emaArgs.size();
            for(size_t stIndex = 3; stIndex < stMax; ++stIndex)
              LuaUtilPushInt(lecAsync.LuaRefGetState(),
                emaArgs[stIndex].ll);
            // Execute the progress callback
            AsyncDoLuaProtectedDispatch(emeEvent, static_cast<int>(stMax - 3),
              iErrorHandler);
            // Do not do anything else
            return;
          } // Show error
          else cLog->LogErrorExSafe("AsyncLoader got invalid params in "
            "progress event $ for '$' with luastate($) and fref($) from $ "
            "params!",
            emeEvent.cCmd, idName.IdentGet(), lecAsync.LuaRefStateIsSet(),
            lecAsync.LuaRefGetFunc(LR_PROGRESS), emaArgs.size());
          // Done
          break;
        } // The operation succeeded with a parameter attached?
        case AR_SUCCESS_PARAM:
        { // We need that third (first) parameter and if we do?
          if(emaArgs.size() >= 4)
          { // Push and get error callback function id
            const int iErrorHandler =
              LuaUtilPushAndGetGenericErrId(lecAsync.LuaRefGetState());
            // Push the success callback and if succeeded?
            if(lecAsync.LuaRefGetFunc(LR_SUCCESS))
            { // Push the class ref and if both succeeded?
              if(lecAsync.LuaRefGetUData())
              { // Push the specified parameter
                LuaUtilPushInt(lecAsync.LuaRefGetState(), emaArgs[3].ll);
                // Dispatch the event with two parameters
                AsyncDoFinishLuaProtectedDispatch(emeEvent, 2, iErrorHandler);
              } // Failed? Write error to log
              else cLog->LogErrorExSafe("AsyncLoader got invalid userdata "
                "param in success param event $ for '$' from $ params!",
                emeEvent.cCmd, idName.IdentGet(), emaArgs.size());
            } // Show error
            else cLog->LogErrorExSafe("AsyncLoader got invalid params in "
              "success param event $ for '$' with luastate($) and fref($) "
              "from $ params!",
              emeEvent.cCmd, idName.IdentGet(),
              lecAsync.LuaRefStateIsSet(), lecAsync.LuaRefGetFunc(LR_SUCCESS),
              emaArgs.size());
          } // Log error
          else cLog->LogErrorExSafe("AsyncLoader got invalid value in "
            "success param event $ for '$' from $ params!", emeEvent.cCmd,
              idName.IdentGet(), emaArgs.size());
          // Done
          break;
        } // The operation succeeded?
        case AR_SUCCESS:
        { // Push and get error callback function id
          const int iErrorHandler =
            LuaUtilPushAndGetGenericErrId(lecAsync.LuaRefGetState());
          // Push the success callback and if succeeded?
          if(lecAsync.LuaRefGetFunc(LR_SUCCESS))
          { // Push the class ref and if both succeeded?
            if(lecAsync.LuaRefGetUData())
              AsyncDoFinishLuaProtectedDispatch(emeEvent, 1, iErrorHandler);
            // Failed? Write error to log
            else cLog->LogErrorExSafe("AsyncLoader got invalid userdata param "
              "in success event $ for '$' from $ params!",
              emeEvent.cCmd, idName.IdentGet(), emaArgs.size());
          } // Show error
          else cLog->LogErrorExSafe("AsyncLoader got invalid params in "
            "success event $ for '$' with luastate($) and fref($) from $ "
            "params!",
            emeEvent.cCmd, idName.IdentGet(), lecAsync.LuaRefStateIsSet(),
            lecAsync.LuaRefGetFunc(LR_SUCCESS), emaArgs.size());
          // Done
          break;
        } // If there was an error?
        case AR_ERROR:
        { // Push the error callback and if succeeded?
          AsyncDoLuaThrowErrorHandler(emeEvent);
          // Done
          break;
        } // Other error?
        default:
        { // Report it and fall through to clean-up
          cLog->LogErrorExSafe(
            "AsyncLoader got unknown result $ after loading '$'!",
            uiAsyncResult, idName.IdentGet());
        } // If the thread was told to abort? Just break to clear references
        case AR_ABORT: break;
      }
    } // Tidy up the variables
    AsyncTidyUpVars();
    // Clear references and state or Lua's GC won't ever delete the class
    lecAsync.LuaRefDeInit();
  } // Exception occured? Disable lua callback/refs and rethrow
  catch(const exception&) { lecAsync.LuaRefDeInit(); throw; }
  /* -- Main constructor --------------------------------------------------- */
  AsyncLoader(                         // Initialise with derived class
    Ident &idNName,                      // Reference to identified
    MemberType*const mtNAsyncOwner,    // The pointer to the class
    const EvtMainCmd emcNAsyncCmd) :   // The event id to use
    /* -- Initialisers ----------------------------------------------------- */
    mtAsyncOwner{ *mtNAsyncOwner },    // Initialise owner of this class
    emcAsyncCmd(emcNAsyncCmd),         // Initialise event id
    idName(idNName),                   // Initialise reference to identifier
    lecAsync{ mtNAsyncOwner,           // Initialise Lua event class...
              emcNAsyncCmd },          // ...with owner class and event cmd.
    tAsyncThread{ STP_LOW },           // Initialise low priority thread
    asctAsyncType(BA_NONE),            // Initialise load source type
    uiAsyncPid(0)                      // Initialise pid for BA_EXECUTE
    /* --------------------------------------------------------------------- */
    { }                                // Do nothing else
  /* ----------------------------------------------------------------------- */
  // NOTE: This is here for convenience only. ALWAYS call AsyncCancel() in your
  // derived classes because static objects created in the event callback while
  // waiting for it to be called will NOT be cleaned up properly and cause
  // memory leaks.
  ~AsyncLoader(void)
  { // Ignore if thread isn't running
    if(tAsyncThread.ThreadIsNotJoinable()) return;
    // Print a warning to say that we should not be allowing this destructor
    // the clean up the thread
    cLog->LogWarningExSafe("AsyncLoader waiting for '$' to unload in its own "
      "destructor. There could be memory leaks!", strAsyncError);
    // Wait for the thread to stop
    AsyncStop();
  }
};/* -- Function to wait for async of all members in a collector ----------- */
template<class MemberType,
         class LockType,
         class ListType,
         class IteratorType,
         class CLHelperType = CLHelperSafe<MemberType, ListType, IteratorType>,
         class BaseType =
           CLHelper<MemberType, CLHelperType, ListType, IteratorType>,
         class LuaEvtSlaveType = LuaEvtTypeAsync<MemberType>,
         class LuaEvtMasterType = LuaEvtMaster<MemberType, LuaEvtSlaveType>>
class CLHelperAsync :
  /* -- Derivced classes --------------------------------------------------- */
  public BaseType                      // CLHelper
{ /* -- Functions -------------------------------------------------- */ public:
  const LuaEvtMasterType lemAsync;     // Event name id
  /* -- Functions ---------------------------------------------------------- */
  void WaitAsync(void)
  { // Lock the collector's mutex
    const LockGuard lgSync{ this->CollectorGetMutex() };
    // Sync all members
    for(MemberType*const cChild : *this) cChild->AsyncWait();
  }
  /* -- Constructor -------------------------------------------------------- */
  CLHelperAsync(const char*const cpName, const EvtMainCmd emcCmd) :
    /* -- Initialisers ----------------------------------------------------- */
    BaseType{ cpName },                // Initialise name of base tpye
    lemAsync{ emcCmd }                 // Initialise event name
    /* -- No code ---------------------------------------------------------- */
    { }
};/* ----------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
