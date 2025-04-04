/* == CORE.HPP ============================================================= **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## This module is the core class which is created by main/WinMain on   ## **
** ## startup. It is pretty much the game-engine main routines.           ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace ICore {                      // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IArchive::P;           using namespace IAsset::P;
using namespace IAudio::P;             using namespace ICmdLine::P;
using namespace IConDef::P;            using namespace IConGraph::P;
using namespace IConsole::P;           using namespace ICVar::P;
using namespace ICVarDef::P;           using namespace ICVarLib::P;
using namespace IDir::P;               using namespace IDisplay::P;
using namespace IError::P;             using namespace IEvtMain::P;
using namespace IEvtWin::P;            using namespace IFbo::P;
using namespace IFboCore::P;           using namespace IJson::P;
using namespace IFont::P;              using namespace IFreeType::P;
using namespace IFtf::P;               using namespace IGlFW::P;
using namespace IGlFWUtil::P;          using namespace IImage::P;
using namespace IInput::P;             using namespace ILog::P;
using namespace ILua::P;               using namespace ILuaCode::P;
using namespace ILuaUtil::P;           using namespace ILuaVariable::P;
using namespace IOgl::P;               using namespace IPalette::P;
using namespace IPcm::P;               using namespace IPSplit::P;
using namespace IShaders::P;           using namespace ISql::P;
using namespace ISource::P;            using namespace IStd::P;
using namespace IStream::P;            using namespace IString::P;
using namespace ISystem::P;            using namespace ISysUtil::P;
using namespace ITexture::P;           using namespace IThread::P;
using namespace ITimer::P;             using namespace IToken::P;
using namespace IVideo::P;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* -- Prototype ------------------------------------------------------------ */
class Core;                            // Core class prototype
static Core *cCore = nullptr;          // Pointer to static class
/* ------------------------------------------------------------------------- */
enum CoreErrorReason                   // Lua error mode behaviour
{ /* ----------------------------------------------------------------------- */
  CER_IGNORE,                          // Ignore errors and try to continue
  CER_RESET,                           // Automatically reset on error
  CER_SHOW,                            // Open console and show error
  CER_CRITICAL,                        // Terminate engine with error
  CER_MAX,                             // Maximum number of options supported
};/* ----------------------------------------------------------------------- */
class Core final                       // Members initially private
{ /* -- Private Variables -------------------------------------------------- */
  const EvtMainRegVec emrvEvents;      // Core events list
  const CbThFunc   cbtMain;            // Bound main thread function
  CoreErrorReason  cerMode;            // Lua error mode behaviour
  unsigned int     uiErrorCount,       // Number of errors occured
                   uiErrorLimit;       // Number of errors allowed
  /* -- Fired when lua needs to be paused (EMC_LUA_PAUSE) ------------------ */
  void OnLuaPause(const EvtMainEvent &emeEvent)
  { // Pause execution and if paused for the first time?
    if(!cLua->PauseExecution())
    { // The mainmanual* functions will consume 100% of the thread load when
      // it doesn't request a request to redraw so make sure to throttle it.
      if(cSystem->IsNotGraphicalMode()) cTimer->TimerSetDelayIfZero();
      // Can't disable console while paused
      cConGraphics->SetCantDisable(true);
      // Write to console
      cConsole->AddLine("Execution paused. Type 'lresume' to continue.");
    } // Already paused? Remind console if it was manually requested
    else if(emeEvent.aArgs.front().b)
      cConsole->AddLine(
        "Execution already paused. Type 'lresume' to continue.");
  }
  /* -- Fired when lua needs to be resumed (EMC_LUA_RESUME) ---------------- */
  void OnLuaResume(const EvtMainEvent&)
  { // Return if pause was not successful
    if(!cLua->ResumeExecution())
      return cConsole->AddLine("Execution already in progress.");
    // Refresh stored delay because of manual render mode
    if(cSystem->IsNotGraphicalMode())
      cTimer->TimerSetDelay(cCVars->GetInternal<unsigned int>(APP_DELAY));
    // Console can now be disabled
    cConGraphics->SetCantDisable(false);
    // Write to console
    cConsole->AddLine("Execution resumed.");
  }
  /* -- Reset environment function ----------------------------------------- */
  void CoreResetEnvironment(const bool bLeaving)
  { // Log that we're resetting the environment
    cLog->LogDebugExSafe("Core $ environment...",
      bLeaving ? "resetting" : "preparing");
    // End current SQL transaction, we need to report it if it succeeded.
    if(bLeaving && cSql->End() != SQLITE_ERROR)
      cLog->LogWarningSafe("Core ended an in-progress SQL transaction!");
    // Reset all SQL error codes and stored results and records.
    cSql->Reset();
    // If using graphical inteactive mode?
    if(cSystem->IsGraphicalMode())
    { // Reset input environment
      cInput->ResetEnvironment();
      // Reset fbo clear colour, selected binds and 8-bit shader palette
      cFboCore->ResetClearColour();
      cOgl->ResetBinds();
      cPalettes->palDefault.Commit();
      // Set main framebuffer as default and reset to original settings
      cFboCore->ActivateMain();
      cDisplay->CommitDefaultMatrix();
      // Cant't disable console if leaving, can if entering
      cConGraphics->SetCantDisable(bLeaving);
      // Reset cursor if leaving else hide console if entering
      if(bLeaving) cDisplay->RequestResetCursor();
      else cConGraphics->SetVisible(false);
      // Restore console font properties
      cConGraphics->RestoreDefaultProperties();
    } // Bot mode? Clear bottom status texts
    if(cSystem->IsTextMode()) cConsole->ClearStatus();
    // Reset timer and clear any lingering engine events
    cTimer->TimerReset(bLeaving);
    cEvtMain->Flush();
    // Log that we've reset the environment
    cLog->LogDebugExSafe("Core environment $.",
      bLeaving ? "reset" : "prepared");
  }
  /* -- Graphical core window thread tick ---------------------------------- */
  void CoreTick(void)
  { // Is it time to execute a game tick?
    if(cTimer->TimerShouldTick())
    { // Render the console fbo (if update requested)
      cConGraphics->Render();
      // Render video textures (if any)
      VideoRender();
      // Loop point incase we need to catchup game ticks
      for(;;)
      { // Set main fbo by default on each frame
        cFboCore->ActivateMain();
        // Poll joysticks
        cInput->JoyPoll();
        // Execute a tick for each frame missed
        cLua->ExecuteMain();
        // Break if we've caught up
        if(cTimer->TimerShouldNotTick()) break;
        // Flush the main fbo as we're not drawing it yet
        cFboCore->RenderFbosAndFlushMain();
        // Render again until we've caught up
      } // Add console fbo to render list
      cConGraphics->RenderToMain();
      // Render all fbos and copy the main fbo to screen
      cFboCore->Render();
      // Update timer
      cTimer->TimerUpdateInteractive();
    } // Update interim timer without storing entire duration
    else cTimer->TimerUpdateInteractiveInterim();
  }
  /* -- Fired when Lua enters the sandbox ---------------------------------- */
  int CoreThreadSandbox(lua_State*const lS)
  { // Capture exceptions...
    try
    { // Clear thread exit status
      cEvtMain->ThreadCancelExit();
      // Compare event code. Do we need to execute scripts?
      switch(cEvtMain->GetExitReason())
      { // Do not re-initialise anything if we processed a LUA error code
        case EMC_LUA_ERROR: break;
        // Lua execution was ended (e.g. use of the 'lend' command)
        case EMC_LUA_END:
          // Setup lua default environment (libraries, config, etc.)
          cLua->SetupEnvironment();
          // Force timer delay to 1ms to prevent 100% thread use on Main*
          cTimer->TimerSetDelayIfZero();
          // Exceptions from here on are recoverable
          cEvtMain->SetExitReason(EMC_LUA_ERROR);
          // Done
          break;
        // The thread was re-initialised? (e.g. vreset command)
        case EMC_QUIT_THREAD:
          // Tell guest scripts to redraw their fbo's
          cEvtMain->Add(EMC_LUA_REDRAW);
          // Exceptions from here on are recoverable
          cEvtMain->SetExitReason(EMC_LUA_ERROR);
          // Done
          break;
        // For anything else
        default:
          // Reset environment (entering)
          CoreResetEnvironment(false);
          // Setup lua default environment (libraries, config, etc.)
          cLua->SetupEnvironment();
          // Default event code is error status. This is so if even c++
          // exceptions or C (LUA) exceptions occur, the underlying scope knows
          // to handle the error and try to recover. The actual loops will set
          // this to something different when they cleanly exit their loops.
          cEvtMain->SetExitReason(EMC_LUA_ERROR);
          // Execute startup script
          LuaCodeExecuteFile(lS, cCVars->GetStrInternal(LUA_SCRIPT));
          // Done
          break;
      } // Terminal mode requested?
      if(cSystem->IsTextMode())
      { // Graphical mode requested too?
        if(cSystem->IsGraphicalMode())
        { // Initialise accumulator for first time
          cTimer->TimerUpdateInteractive();
          // Loop until event manager says we should break
          while(cEvtMain->HandleSafe())
          { // Execute unthreaded tick
            CoreTick();
            // Process bot console
            cConsole->FlushToLog();
          }
        } // Terminal mode only so loop until thread says we should break loop.
        else while(cEvtMain->HandleSafe())
        { // Calculate time elapsed in this tick
          cTimer->TimerUpdateBot();
          // Execute the main tick
          cLua->ExecuteMain();
          // Process bot console
          cConsole->FlushToLog();
        }
      } // Graphical mode requested?
      else if(cSystem->IsGraphicalMode())
      { // On linux?
#if defined(LINUX)
        // We need to fix wayland?
        if(cDisplay->FlagIsSet(DF_WAYLANDFIX))
        { // Restart the thread if actually using wayland
          if(cSystem->IsWayland()) cEvtMain->RequestQuitThread();
          // Won't ever need to trigger this again
          cDisplay->FlagClear(DF_WAYLANDFIX);
        }
#endif
        // Initialise accumulator for first time
        cTimer->TimerUpdateInteractive();
        // Loop until event manager says we should break
        while(cEvtMain->HandleSafe()) CoreTick();
      }
    } // exception occured so throw LUA stackdump and leave the sandbox
    catch(const exception &eReason)
    { // Allow Lue to process error. WARNING!! This prevents destructors on all
      // statically initialised classes to NEVER call so make sure we do not
      // statically create something above!
      LuaUtilPushErr(lS, eReason.what());
    } // Returning nothing
    return 0;
  }
  /* -- Get core pointer and call the entry function ----------------------- */
  static int CoreThreadSandboxStatic(lua_State*const lS)
  { // Get pointer to class
    Core*const cPtr = LuaUtilGetSimplePtr<Core>(lS, 1);
    // Remove light user data pointer
    LuaUtilRmStack(lS);
    // Call sandbox function
    return cPtr->CoreThreadSandbox(lS);
  }
  /* -- Lua deinitialiser helper which updates all the classes that use it - */
  void CoreLuaDeInitHelper(void)
  { // De-init lua and update systems that use Lua
    cLua->DeInit();
    // Reset environment (leaving)
    CoreResetEnvironment(true);
  }
  /* -- DeInitialise engine components ------------------------------------- */
  void CoreDeInitComponents(void) try
  { // Log reason for deinit
    cLog->LogDebugExSafe("Engine de-initialising interfaces with code $.",
      cEvtMain->GetExitReason());
    // Unregister exit events
    cEvtMain->DeInit();
    // Whats the exit reason code?
    switch(cEvtMain->GetExitReason())
    { // Quitting thread?
      case EMC_QUIT_THREAD:
        // Not interactive mode? Nothing to de-initialise
        if(cSystem->IsNotGraphicalMode()) return;
        // Unload console background and font textures
        cConGraphics->DeInitTextureAndFont();
        // Unload font, texture, videos and curor textures
        VideoDeInitTextures();
        FontDeInitTextures();
        TextureDeInitTextures();
        // Done
        break;
      // Were exiting to de-initialise everything
      default:
        // De-initialise Lua
        CoreLuaDeInitHelper();
        // Deregister lua pause and resume callbacks
        cEvtMain->UnregisterEx(emrvEvents);
        // If in graphical mode?
        if(cSystem->IsGraphicalMode())
        { // De-init console graphics and input
          cConGraphics->DeInit();
          cInput->DeInit();
        } // DeInitialise console class and freetype
        cConsole->DeInit();
        cFreeType->DeInit();
        // De-init audio
        if(cSystem->IsAudioMode()) cAudio->DeInit();
        // Done
        break;
    } // Unload all fbos (NOT destroy);
    cFboCore->DeInitAllObjectsAndBuiltIns();
    // De-init core shaders
    cShaderCore->DeInitShaders();
    // OpenGL de-initialised (do not throw error if de-initialised)
    cOgl->DeInit(true);
    // Request to close window
    cDisplay->RequestClose();
  } // exception occured?
  catch(const exception &eReason)
  { // Make sure the exception is logged
    cLog->LogErrorExSafe("(ENGINE THREAD DE-INIT EXCEPTION) $", eReason);
  }
  /* -- Redraw the frame buffer when error occurs -------------------------- */
  void CoreForceRedrawFrameBuffer(const bool bAndConsole)
  { // Flush log if we have a text mode console
    if(cSystem->IsTextMode()) cConsole->FlushToLog();
    // Return if no graphical mode
    if(cSystem->IsNotGraphicalMode()) return;
    // Reset opengl binds to defaults just incase any were selected
    cOgl->ResetBinds();
    // Render the console and it has not already been drawn
    if(bAndConsole) cConGraphics->RenderNow();
    // Render all fbos and copy the main fbo to screen
    cFboCore->Render();
  }
  /* -- De-initialise everything ------------------------------------------- */
  void CoreDeInitEverything(void)
  { // De-initialise components
    CoreDeInitComponents();
    // If not in graphical mode, we're done
    if(cSystem->IsNotGraphicalMode()) return;
    // Window should close as well
    cGlFW->WinSetClose(GLFW_TRUE);
    // Unblock the window thread
    GlFWForceEventHack();
  }
  /* -- Initoialise graphics subsystems ------------------------------------ */
  void CoreInitGraphicalSubsystems(void)
  { // Set context current and pass selected refresh rate
    cOgl->Init(cDisplay->GetRefreshRate());
    // Initialise core shaders
    cShaderCore->InitShaders();
    // If we're initialising for the first time?
    if(cEvtMain->IsExitReason(EMC_NONE))
    { // Initialise freetype, console render, audio and input classes
      cFreeType->Init();
      cConsole->Init();
      cConGraphics->Init();
      cInput->Init();
      // Register lua pause and resume events
      cEvtMain->RegisterEx(emrvEvents);
      // Done
      return;
    } // Not initialising for the first time so reconfigure matrix
    cDisplay->CommitMatrix();
    // Reset window icon
    cDisplay->UpdateIcons();
    // Reload textures
    FboReInit();
    cConGraphics->ReInitTextureAndFont();
    FontReInitTextures();
    TextureReInitTextures();
    VideoReInitTextures();
  }
  /* -- Engine thread (member function) ------------------------------------ */
  int CoreThreadMain(Thread&) try
  { // Log reason for init
    cLog->LogDebugExSafe("Core engine thread started (C:$;M:$<$>).",
      cEvtMain->GetExitReason(), cSystem->GetCoreFlagsString(),
      cSystem->GetCoreFlags());
    // Register exit events
    cEvtMain->Init();
    // Non-interactive mode?
    if(cSystem->IsTextMode())
    { // And interactive mode?
      if(cSystem->IsGraphicalMode()) CoreInitGraphicalSubsystems();
      // No interactive mode so if we're not initialising for the first time?
      else if(cEvtMain->IsExitReason(EMC_NONE))
      { // Initialise freetype and console
        cFreeType->Init();
        cConsole->Init();
        cEvtMain->RegisterEx(emrvEvents);
      } // Initialising for first time? Just update window icons
      else cDisplay->UpdateIcons();
    } // Init interactive console?
    else if(cSystem->IsGraphicalMode()) CoreInitGraphicalSubsystems();
    // With audio mode enabled? Initialise audio class.
    if(cSystem->IsAudioMode() && cEvtMain->IsExitReason(EMC_NONE))
      cAudio->Init();
    // Lua loop with initialisation. Compare event code
    SandBoxInit: switch(cEvtMain->GetExitReason())
    { // Ignore LUA initialisation if we're re-initialising other components
      case EMC_QUIT_THREAD: break;
      // Any other code will initialise LUA
      default: cLua->Init();
    } // Lua loop without initialisation. Begin by capturing exceptions
    SandBox: try
    { // ...and enter sand box mode. Below function is when we're in sandbox
      cLua->EnterSandbox(CoreThreadSandboxStatic, this);
    } // ...and if exception occured?
    catch(const exception &eReason)
    { // Show error in console
      cConsole->AddLine(COLOUR_LRED, eReason.what());
      // Disable garbage collector so no shenangians while we reset
      // environment.
      cLua->StopGC();
      // Reset glfw errorlevel
      cGlFW->ResetErrorLevel();
      // Check event code that was set?
      switch(cEvtMain->GetExitReason())
      { // Run-time error?
        case EMC_LUA_ERROR:
          // If we are not in the exit script?
          if(!cLua->Exiting())
          { // Compare error mode behaviour
            switch(cerMode)
            { // Ignore errors and try to continue? Execute again
              case CER_IGNORE:
                // Write ignore exception to log and pause if at the limit.
                if(uiErrorCount >= uiErrorLimit) goto Pause;
                cLog->LogErrorExSafe(
                  "Core sandbox ignored #$/$ run-time exception: $",
                  ++uiErrorCount, uiErrorLimit, eReason);
                // Redraw the console but do not show it.
                CoreForceRedrawFrameBuffer(false);
                // Go back into the sandbox.
                goto SandBox;
              // Automatically reset on error?
              case CER_RESET:
                // Write ignore exception to log and pause if at the limit.
                if(uiErrorCount >= uiErrorLimit) goto Pause;
                cLog->LogErrorExSafe(
                  "Core sandbox reset #$/$ with run-time exception: $",
                  ++uiErrorCount, uiErrorLimit, eReason);
                // Flush events and restart the guest
                cLua->ReInit();
                // Redraw the console but do not show it
                CoreForceRedrawFrameBuffer(false);
                // Go back into the sandbox
                goto SandBox;
              // Open console and show error? Just break to other code.
              case CER_SHOW: Pause:      // May come here from above too
                // Write ignore exception to log.
                cLog->LogErrorExSafe("Core sandbox run-time exception: $",
                  eReason);
                // Add event to pause
                cLua->RequestPause(false);
                // Redraw the console and show it.
                CoreForceRedrawFrameBuffer(true);
                // Break to pause
                goto SandBox;
              // Unknown value?
              default:
                // Report it!
                cLog->LogErrorExSafe(
                  "Core exception with unknown behaviour $: $",
                  cerMode, eReason);
                // Fall through to CER_CRITICAL
                [[fallthrough]];
              // Terminate engine with error? Throw to critical error dialog.
              case CER_CRITICAL:
                // Redraw the console.
                CoreForceRedrawFrameBuffer(false);
                // Throw to critical error dialog
                throw;
            } // Should not get here
          }
          // Grab the exit code from events if the error because it wasn't
          // able to be caught in the events queue and fall through so the
          // original request can be process.
          cEvtMain->UpdateConfirmExit();
          // Check exit code. These are originally set in EvtMain::DoHandle()
          // so make sure all used exit values with ConfirmExit() are checked
          // for here.
          switch(cEvtMain->GetExitReason())
          { // Lua is ending execution? (i.e. via 'lend') fall through.
            case EMC_LUA_END: [[fallthrough]];
            // Lua executing is re-initialising? (i.e. lreset).
            case EMC_LUA_REINIT:
              // Write exception to log.
              cLog->LogErrorExSafe("Core sandbox de-init exception: $",
                eReason);
              // Break
              break;
            // Unknown exit reason?
            default:
              // Report unknown exit reason to log
              cLog->LogDebugExSafe("Core has unknown exit reason of $!",
                cEvtMain->GetExitReason());
              // Fall through to EMC_QUIT_RESTART
              [[fallthrough]];
            // Quitting and restarting?
            case EMC_QUIT_RESTART: [[fallthrough]];
            // Quitting and restarting with no parameters?
            case EMC_QUIT_RESTART_NP: [[fallthrough]];
            // Thread and main thread should quit so tell thread to break.
            case EMC_QUIT: throw;
          } // We get here if we're going to the choices
          break;
        // Unknown exit reason?
        default:
          // Report it
          cLog->LogWarningExSafe("Core has unknown exit reason of $!",
            cEvtMain->GetExitReason());
          // Fall through to EMC_LUA_END
          [[fallthrough]];
        // Lua is ending execution? Shouldn't happen.
        case EMC_LUA_END: [[fallthrough]];
        // Lua executing is re-initialising? Shouldn't happen.
        case EMC_LUA_REINIT: [[fallthrough]];
        // Quitting and restarting? Shouldn't happen.
        case EMC_QUIT_RESTART: [[fallthrough]];
        // Quitting and restarting without params? Shouldn't happen.
        case EMC_QUIT_RESTART_NP: [[fallthrough]];
        // Thread and main thread should quit. De-init components and rethrow.
        case EMC_QUIT: CoreDeInitComponents(); throw;
      } // We get here to process lua event as normal.
    } // Why did LUA break out of the sandbox?
    switch(cEvtMain->GetExitReason())
    { // Execution ended? (e.g. 'lend' command was used)
      case EMC_LUA_END:
        // Add message to say the execution ended
        cConsole->AddLine("Execution ended! Use 'lreset' to restart.");
        // De-initialis lua
        CoreLuaDeInitHelper();
        // Re-initialise lua and go back into the sandbox
        goto SandBoxInit;
      // Execution re-initialising? (e.g. 'lreset' command was used)
      case EMC_LUA_REINIT:
        // Add message to say the execution is restarting
        cConsole->AddLine("Execution restarting...");
        // De-initialis lua
        CoreLuaDeInitHelper();
        // Re-initialise lua and go back into the sandbox
        goto SandBoxInit;
      // Unknown value so report it and fall through.
      default: cLog->LogWarningExSafe("Core has unknown error behaviour of $!",
        cerMode); [[fallthrough]];
      // Execution ended because of error. Shouldn't get here. Fall through.
      case EMC_LUA_ERROR: [[fallthrough]];
      // Restarting engine completely? Fall through.
      case EMC_QUIT_RESTART: [[fallthrough]];
      case EMC_QUIT_RESTART_NP: [[fallthrough]];
      // Quitting the engine completely? De-initialise lua and fall through.
      case EMC_QUIT: [[fallthrough]];
      // Restarting engine subsystems. i.e. 'vreset'? Fall through.
      case EMC_QUIT_THREAD: break;
    } // De-initilise everything
    CoreDeInitEverything();
    // Kill thread
    return 1;
  } // exception occured out of loop. Fatal so we have to quit
  catch(const exception &eReason)
  { // We will quit since this is fatal
    cEvtMain->SetExitReason(EMC_QUIT);
    // Write exception to log
    cLog->LogErrorExSafe("(ENGINE THREAD FATAL EXCEPTION) $", eReason);
    // Show error
    cSystem->SysMsgEx("Engine Thread Fatal Exception", eReason.what(),
      MB_ICONSTOP);
    // De-inti everything
    CoreDeInitEverything();
    // Kill thread
    return 2;
  }
  /* -- Engine should continue? -------------------------------------------- */
  bool CoreShouldEngineContinue(void)
  { // Compare exit value
    switch(cEvtMain->GetExitReason())
    { // Engine was requested to quit or restart? NO!
      case EMC_QUIT: [[fallthrough]];
      case EMC_QUIT_RESTART: [[fallthrough]];
      case EMC_QUIT_RESTART_NP: return false;
      // Systems were just re-initialising? YES!
      default: return true;
    }
  }
  /* -- Initialise graphical mode ------------------------------------------ */
  void CoreEnterGraphicalMode(void)
  { // Initialise Glfw mode and de-init it when exiting
    INITHELPER(GlFWIH, cGlFW->Init(), cGlFW->DeInit());
    // Until engine should terminate.
    while(CoreShouldEngineContinue()) try
    { // Init window and de-init lua envifonment and window on scope exit
      INITHELPER(DIH, cDisplay->Init(),
        cEvtMain->ThreadDeInit();
        cDisplay->DeInit());
      // Full window events
      cEvtWin->Flush();
      // Setup main thread and start it
      cEvtMain->ThreadInit(cbtMain, nullptr);
      // Loop until window should close
      while(cGlFW->WinShouldNotClose())
      { // Process window event manager commands from other threads
        cEvtWin->ManageSafe();
        // Wait for more window events
        GlFWWaitEvents();
      }
    } // Error occured
    catch(const exception &eReason)
    { // Send to log and show error message to user
      cLog->LogErrorExSafe("(WINDOW LOOP EXCEPTION) $", eReason);
      // Exit loop so we don't infinite loop
      cEvtMain->SetExitReason(EMC_QUIT);
      // Show error and try to carry on and clean everything up
      cSystem->SysMsgEx("Window Loop Fatal Exception", eReason.what(),
        MB_ICONSTOP);
   } // Engine should terminate from here-on
  }
  /* -- Wait async on all systems ---------------------------------- */ public:
  void CoreWaitAllAsync(void)
  { // Wait for all asynchronous operations to complete.
    const scoped_lock slCollectorMutexes{
      cArchives->CollectorGetMutex(), cAssets->CollectorGetMutex(),
      cFtfs->CollectorGetMutex(),     cImages->CollectorGetMutex(),
      cJsons->CollectorGetMutex(),    cPcms->CollectorGetMutex(),
      cSources->CollectorGetMutex(),  cStreams->CollectorGetMutex(),
      cVideos->CollectorGetMutex()
    };
  }
  /* -- Main function ------------------------------------------------------ */
  int CoreMain(const int iArgs, ArgType**const lArgs, ArgType**const lEnv) try
  { // Set this thread's name for debugger and that it is high performance
    SysInitThread("main", STP_MAIN);
    // Create static classes to engine components and set the pointer to that
    // component (which should get optimised to static) so all the other
    // components can access each other. Then nullptr them on destruction so
    // any accidental access to them is easily identifiable by the debugger.
#define INITSS(x, ...) DEINITHELPER(dih ## x, c ## x = nullptr); \
      x eng ## x{ __VA_ARGS__ }; c ## x = &eng ## x
    // Initialise critical command-line and logging systems. We cannot really
    // do anything else special until we've enabled these subsystems.
    INITSS(Common);                    // cppcheck-suppress danglingLifetime
    INITSS(DirBase);                   // cppcheck-suppress danglingLifetime
    INITSS(CmdLine,iArgs,lArgs,lEnv);  // cppcheck-suppress danglingLifetime
    INITSS(Log);                       // cppcheck-suppress danglingLifetime
    // Capture exceptions so we can log any errors
    try
    { // Dependencies required only in this scope
      using namespace IArgs::P;        using namespace IAtlas::P;
      using namespace IBin::P;         using namespace ICert::P;
      using namespace IClipboard::P;   using namespace ICmdLine::P;
      using namespace IClock::P;       using namespace ICodecCAF::P;
      using namespace ICodecDDS::P;    using namespace ICodecGIF::P;
      using namespace ICodecJPG::P;    using namespace ICodecMP3::P;
      using namespace ICodecOGG::P;    using namespace ICodecPNG::P;
      using namespace ICodecWAV::P;    using namespace IConLib::P;
      using namespace ICredit::P;      using namespace ICrypt::P;
      using namespace IFile::P;        using namespace IGlFWMonitor::P;
      using namespace IImageDef::P;    using namespace IImageLib::P;
      using namespace ILuaCommand::P;  using namespace ILuaFunc::P;
      using namespace IMask::P;        using namespace IMemory::P;
      using namespace IOal::P;         using namespace IPcmLib::P;
      using namespace ISample::P;      using namespace IShader::P;
      using namespace ISocket::P;      using namespace ISShot::P;
      using namespace IStat::P;        using namespace IUtil::P;
      // Initialise other subsystems. The order is important!
      INITSS(Stats);                   // cppcheck-suppress danglingLifetime
      INITSS(Threads);                 // cppcheck-suppress danglingLifetime
      INITSS(EvtMain);                 // cppcheck-suppress danglingLifetime
      INITSS(System);                  // cppcheck-suppress danglingLifetime
      INITSS(LuaFuncs);                // cppcheck-suppress danglingLifetime
      INITSS(Archives);                // cppcheck-suppress danglingLifetime
      INITSS(Assets);                  // cppcheck-suppress danglingLifetime
      INITSS(Crypt);                   // cppcheck-suppress danglingLifetime
      INITSS(Timer);                   // cppcheck-suppress danglingLifetime
      INITSS(Sql);                     // cppcheck-suppress danglingLifetime
      INITSS(Jsons);                   // cppcheck-suppress danglingLifetime
#include "cvarlib.hpp"                 // Defines 'cvislList' cvar list
      INITSS(CVars, cvislList);        // cppcheck-suppress danglingLifetime
      INITSS(Sockets);                 // cppcheck-suppress danglingLifetime
#include "conlib.hpp"                  // Defines 'ccslList' command list
      INITSS(Console, ccslList);       // cppcheck-suppress danglingLifetime
      INITSS(GlFW);                    // cppcheck-suppress danglingLifetime
      INITSS(Credits);                 // cppcheck-suppress danglingLifetime
      INITSS(FreeType);                // cppcheck-suppress danglingLifetime
      INITSS(Ftfs);                    // cppcheck-suppress danglingLifetime
      INITSS(Files);                   // cppcheck-suppress danglingLifetime
      INITSS(Masks);                   // cppcheck-suppress danglingLifetime
      INITSS(Bins);                    // cppcheck-suppress danglingLifetime
      INITSS(Oal);                     // cppcheck-suppress danglingLifetime
      INITSS(PcmLibs);                 // cppcheck-suppress danglingLifetime
      INITSS(CodecWAV); /* PCMFID=0 */ // cppcheck-suppress danglingLifetime
      INITSS(CodecCAF); /* PCMFID=1 */ // cppcheck-suppress danglingLifetime
      INITSS(CodecOGG); /* PCMFID=2 */ // cppcheck-suppress danglingLifetime
      INITSS(CodecMP3); /* PCMFID=3 */ // cppcheck-suppress danglingLifetime
      INITSS(Pcms);                    // cppcheck-suppress danglingLifetime
      INITSS(Audio);                   // cppcheck-suppress danglingLifetime
      INITSS(Sources);                 // cppcheck-suppress danglingLifetime
      INITSS(Samples);                 // cppcheck-suppress danglingLifetime
      INITSS(Streams);                 // cppcheck-suppress danglingLifetime
      INITSS(EvtWin);                  // cppcheck-suppress danglingLifetime
      INITSS(Ogl);                     // cppcheck-suppress danglingLifetime
      INITSS(ImageLibs);               // cppcheck-suppress danglingLifetime
      INITSS(CodecPNG); /* IMGFID=0 */ // cppcheck-suppress danglingLifetime
      INITSS(CodecJPG); /* IMGFID=1 */ // cppcheck-suppress danglingLifetime
      INITSS(CodecGIF); /* IMGFID=2 */ // cppcheck-suppress danglingLifetime
      INITSS(CodecDDS); /* IMGFID=3 */ // cppcheck-suppress danglingLifetime
      INITSS(Images);                  // cppcheck-suppress danglingLifetime
      INITSS(Shaders);                 // cppcheck-suppress danglingLifetime
      INITSS(Clips);                   // cppcheck-suppress danglingLifetime
      INITSS(Display);                 // cppcheck-suppress danglingLifetime
      INITSS(Input);                   // cppcheck-suppress danglingLifetime
      INITSS(ShaderCore);              // cppcheck-suppress danglingLifetime
      INITSS(Fbos);                    // cppcheck-suppress danglingLifetime
      INITSS(FboCore);                 // cppcheck-suppress danglingLifetime
      INITSS(SShots);                  // cppcheck-suppress danglingLifetime
      INITSS(Textures);                // cppcheck-suppress danglingLifetime
      INITSS(Palettes);                // cppcheck-suppress danglingLifetime
      INITSS(Atlases);                 // cppcheck-suppress danglingLifetime
      INITSS(Fonts);                   // cppcheck-suppress danglingLifetime
      INITSS(Videos);                  // cppcheck-suppress danglingLifetime
      INITSS(ConGraphics);             // cppcheck-suppress danglingLifetime
      INITSS(Variables);               // cppcheck-suppress danglingLifetime
      INITSS(Commands);                // cppcheck-suppress danglingLifetime
      INITSS(Lua);                     // cppcheck-suppress danglingLifetime
      // Done with this macro
#undef INITSS
      // Register default cvars and pass over the current gui mode by ref. All
      // the core parts of the engine are initialised from cvar callbacks.
      cCVars->Init();
      // Text mode requested?
      if(cSystem->IsTextMode())
      { // Bail out if logging to standard output because this will prevent
        // the text mode from working properly
        if(cLog->IsRedirectedToDevice())
          XC("Text console cannot be used when logging to standard output!");
        // Init lightweight text mode console for monitoring.
        INITHELPER(SysConIH,
          cSystem->SysConInit(cSystem->GetGuestTitle().data(),
            cCVars->GetInternal<unsigned int>(CON_TMCCOLS),
            cCVars->GetInternal<unsigned int>(CON_TMCROWS),
            cCVars->GetInternal<bool>(CON_TMCNOCLOSE));
          cSystem->WindowInitialised(nullptr),
          cEvtMain->ThreadDeInit();
          cSystem->SetWindowDestroyed();
          cSystem->SysConDeInit());
        // Perform the initial draw of the console.
        cConsole->FlushToLog();
        // Graphical mode requested too?
        if(cSystem->IsGraphicalMode()) CoreEnterGraphicalMode();
        // Only text mode requested?
        else
        { // Reset window icon
          cDisplay->UpdateIcons();
          // Execute main function until EMC_QUIT or EMC_QUIT_RESTART is
          // passed. We are using the system's main thread so we just need to
          // name this thread properly. We won't actually be spawning a new
          // thread with this though, it's just used as simple exit condition
          // flag to be compatible with the GUI mode.
          while(CoreShouldEngineContinue()) CoreThreadMain(*cEvtMain);
        } // If system says we have to close as quickly as possible?
        if(cSystem->SysConIsClosing())
        { // Quickly save cvars, database and log, this is the priority since
          // Windows has a hardcoded termination time for console apps.
          cCVars->Save();
          cSql->DeInit();
          cLog->DeInitSafe();
          // Now Windows can exit anytime it wants
          cSystem->SysConCanCloseNow();
        }
      } // Else were in graphical interactive mode
      else if(cSystem->IsGraphicalMode()) CoreEnterGraphicalMode();
      // No front-end requested
      else XC("No front-end specified in core flags!",
        "Flags", cSystem->GetCoreFlags());
      // Compare engine exit code...
      switch(cEvtMain->GetExitReason())
      { // If we're to restart process with parameters? Set to do so
        case EMC_QUIT_RESTART:
          // Message to send
          cLog->LogWarningExSafe(
            "Core signalled to restart with $ parameters!",
            cCmdLine->GetTotalCArgs());
          // Set exit procedure
          cCmdLine->SetRestart(cSystem->IsGraphicalMode() ?
            EO_UI_REBOOT : EO_TERM_REBOOT);
          // Have debugging enabled?
          if(cLog->HasLevel(LH_DEBUG))
          { // Log each argument that will be sent
            size_t stId = 0;
            for(const string &strArg : cCmdLine->GetArgList())
              cLog->LogNLCDebugExSafe("- Arg $: $.", stId++, strArg);
          } // Clean-up and restart
          return 3;
        // If we're to restart process without parameters? Set to do so.
        case EMC_QUIT_RESTART_NP:
          // Put event in log
          cLog->LogWarningSafe(
            "Core signalled to restart without parameters!");
          // Set exit procedure
          cCmdLine->SetRestart(cSystem->IsGraphicalMode() ?
            EO_UI_REBOOT_NOARG : EO_TERM_REBOOT_NOARG);
          // Clean-up and restart
          return 4;
        // Normal exit (which is already set to EO_QUIT)
        default: break;
      }
    } // Safe loggable exception occured?
    catch(const exception &eReason)
    { // Send to log and show error message to user
      cLog->LogErrorExSafe("(MAIN THREAD FATAL EXCEPTION) $", eReason);
      // Show message box
      SysMessage("Main Thread Exception", eReason.what(), MB_ICONSTOP);
      // Done
      return 2;
    } // Clean exit
    return 0;
  } // Unsafe exception occured?
  catch(const exception &eReason)
  { // Show message box
    SysMessage("Main Init Exception", eReason.what(), MB_ICONSTOP);
    // Done
    return 1;
  }
  /* -- Default constructor ------------------------------------------------ */
  Core(void) :                         // No parameters
    /* --------------------------------------------------------------------- */
    emrvEvents{                     // Default events
      { EMC_LUA_PAUSE,  bind(&Core::OnLuaPause,  this, _1) },
      { EMC_LUA_RESUME, bind(&Core::OnLuaResume, this, _1) },
    },
    /* -- Initialisers ----------------------------------------------------- */
    cbtMain{ bind(&Core::CoreThreadMain, this, _1) },
    cerMode{ CER_CRITICAL },           // Init lua error mode behaviour
    uiErrorCount(0),                   // Init number of errors occured
    uiErrorLimit(0)                    // Init number of errors allowed
    /* -- Set pointer to this class ---------------------------------------- */
    { cCore = this; }
  /* -- Destructor that clears the core pointer ---------------------------- */
  DTORHELPER(~Core, cCore = nullptr)
  /* -- Set lua error mode behaviour --------------------------------------- */
  CVarReturn CoreErrorBehaviourModified(const CoreErrorReason cefNMode)
    { return CVarSimpleSetIntNGE(cerMode, cefNMode, CER_MAX); }
  /* -- Title modified ----------------------------------------------------- */
  CVarReturn CoreTitleModified(const string &strN, string &strV)
  { // Do not allow user to set this variable, only empty is allowed
    if(!strN.empty()) return DENY;
    // Set the title
    strV = StrAppend(cSystem->GetGuestTitle(), ' ', cSystem->GetGuestVersion(),
      " (", cSystem->ENGTarget(), ")");
    // We changed the value
    return ACCEPT_HANDLED;
  }
  /* -- Ask system to clear mutex ------------------ Core::SetOneInstance -- */
  CVarReturn CoreClearMutex(const bool bEnabled)
  { // Ignore check if not needed or global mutex creation succeeded
    if(bEnabled) cSystem->DeleteGlobalMutex(cSystem->GetGuestTitle());
    // Execution may continue
    return ACCEPT;
  }
  /* -- Set once instance cvar changed ------------- Core::SetOneInstance -- */
  CVarReturn CoreSetOneInstance(const bool bEnabled)
  { // Ignore check if not needed or global mutex creation succeeded
    if(!bEnabled || cSystem->InitGlobalMutex(cSystem->GetGuestTitle()))
      return ACCEPT;
    // Global mutex creation failed so exit program now, cleanly.
    exit(5);
  }
  /* -- Set home directory where files are written if base dir dont work --- */
  CVarReturn CoreSetHomeDir(const string &strP, string &strV)
  { // Build user volatile directory name if user didn't specify one
    if(strP.empty())
      strV = StrFormat("$/$/$/", cSystem->GetRoamingDir(),
        cSystem->GetGuestAuthor().empty() ?
          cCommon->Unspec() : cSystem->GetGuestAuthor(),
        cSystem->GetGuestShortTitle().empty() ?
          cCommon->Unspec() : cSystem->GetGuestShortTitle());
    // Specified so use what the user specified
    else strV = StdMove(strP);
    // Try to make the users home directory as an alternative save place.
    if(!DirMkDirEx(PSplitBackToForwardSlashes(strV)))
      XCL("Failed to setup persistance directory!", "Directory", strV);
    // Set home directory and return that we set the value
    cCmdLine->SetHome(strV);
    return ACCEPT_HANDLED;
  }
  /* -- Set error limit ---------------------------------------------------- */
  CVarReturn CoreSetResetLimit(const unsigned int uiLimit)
    { return CVarSimpleSetInt(uiErrorLimit, uiLimit); }
  /* -- Parses the command-line -------------------------------------------- */
  CVarReturn CoreParseCmdLine(const string&, string &strV)
  { // Get command line parameters and if we have parameters?
    const StrVector &svArgs = cCmdLine->GetArgList();
    if(!svArgs.empty())
    { // Reserve some memory for building command line
      strV.reserve(32767);
      // Valid commands parsed
      size_t stGood = 0, stArg = 1;
      // Parse command line arguments and iterate through them
      for(const string &strArg : svArgs)
      { // If empty argument? Log the failure and continue
        if(strArg.empty())
          cLog->LogWarningExSafe(
            "Core rejected empty command-line argument at $!", stArg);
        // Not empty argument? Tokenise the argument into a key/value pair. We
        // only want a maximum of two tokens, the seperator is allowed on the
        // second token and if succeeded?
        else if(const Token tKeyVal{ strArg, cCommon->Equals(), 2 })
        { // Set the variable from command line with full permission because we
          // should allow any variable to be overridden from the command line.
          // Also show an error if the variable could not be set.
          if(cCVars->SetVarOrInitial(tKeyVal.front(), tKeyVal.size() > 1 ?
            tKeyVal.back() : cCommon->Blank(), SCMDLINE|PBOOT, CCF_NOTHING))
          { // Append argument to accepted command line and add a space
            strV.append(strArg);
            strV.append(cCommon->Space());
            // Good variable
            ++stGood;
          } // Failure? Log the failure
          else cLog->LogWarningExSafe(
            "Core rejected command-line argument at $: '$'!", stArg, strArg);
        } // Log the failure and continue to next argument
        else cLog->LogWarningExSafe("Core rejected invalid command-line "
          "argument at $: '$'!", stArg, strArg);
        // Next argument
        ++stArg;
      } // Remove empty space if not empty
      if(!strV.empty()) strV.pop_back();
      // Free unused memory
      strV.shrink_to_fit();
      // Write command-line arguments parsed
      cLog->LogDebugExSafe("Core parsed $ of $ command-line arguments.",
        stGood, svArgs.size());
    } // No arguments processed
    else cLog->LogDebugSafe("Core parsed no command-line arguments!");
    // We handled setting the variable
    return ACCEPT_HANDLED;
  }
};/* ----------------------------------------------------------------------- */
};                                     // End of public module namespace
/* ------------------------------------------------------------------------- */
};                                     // End of private module namespace
/* == EoF =========================================================== EoF == */
