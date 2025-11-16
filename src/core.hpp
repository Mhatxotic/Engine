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
/* -- Dependencies (pretty much all of them) ------------------------------- */
using namespace IArchive::P;           using namespace IArgs::P;
using namespace IAsset::P;             using namespace IAtlas::P;
using namespace IAudio::P;             using namespace IBin::P;
using namespace ICVar::P;              using namespace ICVarDef::P;
using namespace ICVarLib::P;           using namespace ICert::P;
using namespace IClipboard::P;         using namespace IClock::P;
using namespace ICommon::P;            using namespace ICmdLine::P;
using namespace ICmdLine::P;           using namespace ICodecCAF::P;
using namespace ICodecDDS::P;          using namespace ICodecGIF::P;
using namespace ICodecJPG::P;          using namespace ICodecMP3::P;
using namespace ICodecOGG::P;          using namespace ICodecPNG::P;
using namespace ICodecWAV::P;          using namespace IConDef::P;
using namespace IConGraph::P;          using namespace IConLib::P;
using namespace IConsole::P;           using namespace ICredit::P;
using namespace ICrypt::P;             using namespace IDir::P;
using namespace IDisplay::P;           using namespace IError::P;
using namespace IEvtMain::P;           using namespace IEvtWin::P;
using namespace IFbo::P;               using namespace IFboCore::P;
using namespace IFile::P;              using namespace IFont::P;
using namespace IFreeType::P;          using namespace IFtf::P;
using namespace IGlFW::P;              using namespace IGlFWMonitor::P;
using namespace IGlFWUtil::P;          using namespace IImage::P;
using namespace IImageDef::P;          using namespace IImageLib::P;
using namespace IInput::P;             using namespace IJson::P;
using namespace ILog::P;               using namespace ILua::P;
using namespace ILuaCode::P;           using namespace ILuaCommand::P;
using namespace ILuaFunc::P;           using namespace ILuaUtil::P;
using namespace ILuaVariable::P;       using namespace IMask::P;
using namespace IMemory::P;            using namespace IMutex::P;
using namespace IOal::P;               using namespace IOgl::P;
using namespace IPSplit::P;            using namespace IPalette::P;
using namespace IPcm::P;               using namespace IPcmLib::P;
using namespace ISShot::P;             using namespace ISample::P;
using namespace IShader::P;            using namespace IShaders::P;
using namespace ISocket::P;            using namespace ISource::P;
using namespace ISql::P;               using namespace IStat::P;
using namespace IStd::P;               using namespace IStream::P;
using namespace IString::P;            using namespace IString::P;
using namespace ISysUtil::P;           using namespace ISystem::P;
using namespace ITexture::P;           using namespace IThread::P;
using namespace ITimer::P;             using namespace IToken::P;
using namespace IUrl::P;               using namespace IUtil::P;
using namespace IVideo::P;             using namespace Lib::OS::GlFW::Types;
using namespace Lib::OpenAL::Types;    using namespace Lib::Sqlite::Types;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* ------------------------------------------------------------------------- */
class Core;                            // Core class prototype
static Core *cCore = nullptr;          // Pointer to static class
class Core final :                     // Members initially private
  /* -- Base classes (order is critical!) ---------------------------------- */
  private Stats,          private Threads,        private EvtMain,
  private System,         private LuaFuncs,       private Archives,
  private Assets,         private Crypt,          private Urls,
  private Timer,          private Sql,            private Jsons,
  private CVarItemStaticList, private CVars,      private Sockets,
  private ConCmdStaticList,   private Console,    private GlFW,
  private Credits,        private FreeType,       private Ftfs,
  private Files,          private Masks,          private Bins,
  private Oal,            private PcmLibs,        private CodecWAV,
  private CodecCAF,       private CodecOGG,       private CodecMP3,
  private Pcms,           private Audio,          private Sources,
  private Samples,        private Streams,        private EvtWin,
  private Ogl,            private ImageLibs,      private CodecPNG,
  private CodecJPG,       private CodecGIF,       private CodecDDS,
  private Images,         private Shaders,        private Clips,
  private Display,        private Input,          private ShaderCore,
  private Fbos,           private FboCore,        private SShots,
  private Textures,       private Palettes,       private Atlases,
  private Fonts,          private Videos,         private ConGraphics,
  private Variables,      private Commands,       private Lua
{ /* -- Private Variables ------------------------------------------------- */
  enum CoreErrorReason                 // Lua error mode behaviour options
  { CER_IGNORE,                        // [0] Ignore errors and try to continue
    CER_RESET,                         // [1] Automatically reset on error
    CER_SHOW,                          // [2] Open console and show error
    CER_CRITICAL,                      // [3] Terminate engine with error
    CER_MAX,                           // [4] Maximum # of options supported
  } cerMode;                           // Lua error mode behaviour setting
  const CbThFunc   cbtMain;            // Bound main thread function
  unsigned int     uiErrorCount,       // Number of errors occured
                   uiErrorLimit;       // Number of errors allowed
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
    // Clear console status bars. Although this only applies if there is a
    // terminal window, we don't restrict use of the function in graphical only
    // mode so clear it regardless of what the gui mode is.
    cConsole->ClearStatus();
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
      // Reset the cursor
      cDisplay->RequestResetCursor();
      // If leaving main execution?
      if(bLeaving)
      { // Enable and show console, and set full-screen
        cConGraphics->LeaveResetEnvironment();
        // Force a 1ms suspend lock to not hog the cpu
        cTimer->TimerReset(true);
      } // If entering?
      else
      { // Disable and hide console, and restore size
        cConGraphics->EnterResetEnvironment();
        // Remove the 1ms FPS limit lock on the engine
        cTimer->TimerReset(false);
      } // Make sure main fbo is cleared
      cFboCore->SetDraw();
    } // Not graphical? Set or remove the 1ms FPS limit lock on the engine
    else cTimer->TimerReset(bLeaving);
    // Reset unique ids. Remember some classes aren't registered in the
    // collector, such as the console and main fbo.
#define RSCEX(x,v) x->CounterReset(x->CollectorCount() + v)
#define RSCX(x,v) RSCEX(c ## x, v)
#define RSC(x) RSCX(x, 0)
    RSC(Archives); RSC(Assets);    RSC(Atlases); RSC(Bins);      RSC(Clips);
    RSC(Commands); RSCX(Fbos,2);   RSC(Files);   RSC(Fonts);     RSC(Ftfs);
    RSC(Images);   RSC(ImageLibs); RSC(Jsons);   RSC(LuaFuncs);  RSC(Masks);
    RSC(Palettes); RSC(Pcms);      RSC(PcmLibs); RSC(Samples);   RSC(Shaders);
    RSC(Sockets);  RSC(Sources);   RSC(SShots);  RSC(Stats);     RSC(Streams);
    RSC(Textures); RSC(Threads);   RSC(Urls);    RSC(Variables); RSC(Videos);
#undef RSC
#undef RSCX
#undef RSCEX
    // Reset socket packet and traffic counters
    SocketResetCounters();
    // Clear any lingering events which is very important because events from
    // the last sandbox may contain invalidated pointers and as long as they
    // don't reach the 'cEvtMain->ManageSafe()' function we're fine.
    cEvtMain->Flush();
    // Reset tick count and catchup
    TimerCatchup();
    TimerResetTicks();
    // Log that we've reset the environment
    cLog->LogDebugExSafe("Core environment $.",
      bLeaving ? "reset" : "prepared");
  }
  /* -- Graphical core window thread tick without frame limiter ------------ */
  void CoreTickNoFrameLimiter()
  { // Update timer
    cTimer->TimerUpdateBot();
    // Render the console fbo (if update requested)
    cConGraphics->Render();
    // Render video textures (if any)
    VideoRender();
    // Set main fbo by default on each frame
    cFboCore->ActivateMain();
    // Poll joysticks
    cInput->JoyPoll();
    // Execute a tick for each frame missed
    cLua->ExecuteMain();
    // Add console fbo to render list
    cConGraphics->RenderToMain();
    // Render all fbos and copy the main fbo to screen
    cFboCore->Render();
  }
  /* -- Graphical core window thread tick with frame limiter --------------- */
  void CoreTickFrameLimiter()
  { // Return if it is not time to execute a game tick
    if(cTimer->TimerShouldNotTick()) return;
    // Render the console fbo (if update requested)
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
  }
  /* -- Fired when Lua enters the sandbox ---------------------------------- */
  int CoreThreadSandbox(lua_State*const lS)
  { // Capture exceptions...
    try
    { // Clear thread exit status
      cEvtMain->ThreadCancelExit();
      // Compare event code. Do we need to execute scripts?
      switch(cEvtMain->GetExitReason())
      { // Do not reinitialise anything if we processed a LUA error code
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
        // The thread and window was reinitialised? (e.g. vreset command)
        case EMC_QUIT_VREINIT: [[fallthrough]];
        case EMC_QUIT_THREAD:
          // Tell guest scripts to redraw their fbo's
          cEvtMain->Add(EMC_LUA_REDRAW);
          // Exceptions from here on are recoverable
          cEvtMain->SetExitReason(EMC_LUA_ERROR);
          // Done
          break;
        // Lua executing was reinitialised?
        case EMC_LUA_REINIT:
          // Commit modified cvars. We know if we get this far, all the
          // configurations are valid so let's make sure they're saved!
          cCVars->Save();
          // Fall through to default
          [[fallthrough]];
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
        { // Frame limiter enabled?
          if(cSystem->IsTimerMode())
          { // Loop until event manager emits break
            while(cEvtMain->HandleSafe())
            { // Execute unthreaded tick
              CoreTickFrameLimiter();
              // Process bot console
              cConsole->FlushToLog();
            }
          } // Frame limiter disabled so loop until event manager emits break
          else while(cEvtMain->HandleSafe())
          { // Execute unthreaded tick
            CoreTickNoFrameLimiter();
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
      { // Frame limiter enabled?
        if(cSystem->IsTimerMode())
        { // Loop until event manager says we should break
          while(cEvtMain->HandleSafe()) CoreTickFrameLimiter();
        } // Frame limiter disabled so loop without frame limiting
        else while(cEvtMain->HandleSafe()) CoreTickNoFrameLimiter();
      } // No mode set
      else while(cEvtMain->HandleSafe())
      { // Calculate time elapsed in this tick
        cTimer->TimerUpdateBot();
        // Execute the main tick
        cLua->ExecuteMain();
        // Process bot console
        cConsole->FlushToTerminal();
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
  void CoreLuaDeInitHelper()
  { // De-init lua and update systems that use Lua
    cLua->DeInit();
    // Reset environment (leaving)
    CoreResetEnvironment(true);
  }
  /* -- DeInitialise engine components ------------------------------------- */
  void CoreDeInitComponents() try
  { // Log reason for deinit
    cLog->LogDebugExSafe("Engine de-initialising interfaces with code $.",
      cEvtMain->GetExitReason());
    // Request to close window
    cDisplay->RequestClose();
    // Whats the exit reason code?
    switch(cEvtMain->GetExitReason())
    { // Quitting thread?
      case EMC_QUIT_VREINIT: [[fallthrough]];
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
        // If in graphical mode?
        if(cSystem->IsGraphicalMode())
        { // De-init console graphics and input
          cConGraphics->DeInit();
          cInput->DeInit();
        } // DeInitialise console class and freetype
        cConsole->DeInit();
        cFreeType->DeInit();
        // De-init audio
        if(cSystem->IsAudioMode()) cAudio->AudioDeInit();
        // Done
        break;
    } // Unload all fbos (NOT destroy);
    cFboCore->DeInitAllObjectsAndBuiltIns();
    // De-init core shaders
    cShaderCore->DeInitShaders();
    // OpenGL de-initialised (do not throw error if de-initialised)
    cOgl->DeInit(true);
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
  void CoreDeInitEverything()
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
  void CoreInitGraphicalSubsystems()
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
    } // Not initialising for the first time?
    else
    { // Reconfigure matrix
      cDisplay->CommitMatrix();
      // Reset window icon
      cDisplay->UpdateIcons();
      // Reload guest frame buffer objects
      FboReInit();
      // Reload graphical console textures and font
      cConGraphics->ReInitTextureAndFont();
      // Reload guest fonts
      FontReInitTextures();
      // Reload guest textures
      TextureReInitTextures();
      // Reload guest videos
      VideoReInitTextures();
      // Update cursor visibility as GLFW does not restore it
      cInput->CommitCursor();
    } // Show the window
    cDisplay->RequestOpen();
  }
  /* -- Engine thread (member function) ------------------------------------ */
  ThreadStatus CoreThreadMain(Thread&) try
  { // Log reason for init
    cLog->LogDebugExSafe("Core engine thread started (C:$;M:$<$>).",
      cEvtMain->GetExitReason(), cSystem->GetCoreFlagsString(),
      cSystem->GetCoreFlags());
    // Non-interactive mode?
    if(cSystem->IsTextMode())
    { // And interactive mode?
      if(cSystem->IsGraphicalMode()) CoreInitGraphicalSubsystems();
      // No interactive mode so if we're not initialising for the first time?
      else if(cEvtMain->IsExitReason(EMC_NONE))
      { // Initialise freetype and console
        cFreeType->Init();
        cConsole->Init();
      } // Initialising for first time? Just update window icons
      else cDisplay->UpdateIcons();
    } // Init interactive console?
    else if(cSystem->IsGraphicalMode()) CoreInitGraphicalSubsystems();
    // With audio mode enabled? Initialise audio class.
    if(cSystem->IsAudioMode() && cEvtMain->IsExitReason(EMC_NONE))
      cAudio->AudioInit();
    // Lua loop with initialisation. Compare event code
    SandBoxInit: switch(cEvtMain->GetExitReason())
    { // Ignore LUA initialisation if we're reinitialising other components
      case EMC_QUIT_VREINIT: [[fallthrough]];
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
            // Lua executing is reinitialising? (i.e. lreset).
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
        // Lua executing is reinitialising? Shouldn't happen.
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
        // Reinitialise lua and go back into the sandbox
        goto SandBoxInit;
      // Execution reinitialising? (e.g. 'lreset' command was used)
      case EMC_LUA_REINIT:
        // Add message to say the execution is restarting
        cConsole->AddLine("Execution restarting...");
        // De-initialis lua
        CoreLuaDeInitHelper();
        // Reinitialise lua and go back into the sandbox
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
      case EMC_QUIT_VREINIT: [[fallthrough]];
      case EMC_QUIT_THREAD: break;
    } // De-initilise everything
    CoreDeInitEverything();
    // Kill thread
    return TS_OK;
  } // exception occured out of loop. Fatal so we have to quit
  catch(const exception &eReason)
  { // We will quit since this is fatal
    cEvtMain->SetExitReason(EMC_QUIT);
    // Write exception to log
    cLog->LogErrorExSafe("(ENGINE THREAD FATAL EXCEPTION) $", eReason);
    // Show error
    cSystem->SysMsgEx("Engine Thread Fatal Exception", eReason.what(),
      MB_ICONSTOP);
    // De-init everything
    CoreDeInitEverything();
    // Kill thread
    return TS_ERROR;
  }
  /* -- Engine should continue? -------------------------------------------- */
  bool CoreShouldEngineContinue()
  { // Compare exit value
    switch(cEvtMain->GetExitReason())
    { // Engine was requested to quit or restart? NO!
      case EMC_QUIT: [[fallthrough]];
      case EMC_QUIT_RESTART: [[fallthrough]];
      case EMC_QUIT_RESTART_NP: return false;
      // Systems were just reinitialising? YES!
      default: return true;
    }
  }
  /* -- Initialise graphical mode ------------------------------------------ */
  void CoreEnterGraphicalMode()
  { // Initialise Glfw mode and de-init it when exiting
    INITHELPER(GlFWIH, cGlFW->Init(), cGlFW->DeInit());
    // Until engine should terminate.
    while(CoreShouldEngineContinue()) try
    { // Init window and de-init lua envifonment and window on scope exit
      INITHELPER(DIH, cDisplay->Init(),
        cEvtMain->ThreadDeInit();
        cDisplay->DeInit());
      // Setup main thread and start it
      Restart: cEvtMain->ThreadInit(cbtMain, nullptr);
      // Loop until window should close
      while(cGlFW->WinShouldNotClose())
      { // Process window event manager commands from other threads
        cEvtWin->Manage();
        // Wait for more window events
        GlFWWaitEvents();
      } // Restart to hard reinit the window if not doing a soft reinit
      if(cEvtMain->GetExitReason() != EMC_QUIT_VREINIT) continue;
      // De-initialise the thread
      cEvtMain->ThreadDeInit();
      // Soft reinitialise the window
      cDisplay->ReInit();
      // Go back to the thread restart point
      goto Restart;
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
  void CoreWaitAllAsync()
  { // Wait for all asynchronous operations to complete.
    cArchives->MutexScopedCall([](){
      // Log sychronisation result
      cLog->LogDebugExSafe("Core synchronised $ objects.", cArchives->size() +
        cAssets->size() + cFtfs->size() + cImages->size() + cJsons->size() +
        cPcms->size() + cSources->size() + cStreams->size() + cVideos->size());
    }, cAssets->MutexGet(), cFtfs->MutexGet(), cImages->MutexGet(),
    cJsons->MutexGet(), cPcms->MutexGet(), cSources->MutexGet(),
    cStreams->MutexGet(), cVideos->MutexGet());
  }
  /* -- Main function ------------------------------------------------------ */
  int CoreMain()
  { // Register default cvars and pass over the current gui mode by ref. All
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
        // Execute main function until EMC_QUIT or EMC_QUIT_RESTART is passed.
        // We are using the system's main thread so we just need to name this
        // thread properly. We won't actually be spawning a new thread with
        // this though, it's just used as simple exit condition flag to be
        // compatible with the GUI mode.
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
    // No front-end requested so we just use stdout
    else
    { // Init lightweight text mode console for monitoring.
      INITHELPER(NoConIH,
        cSystem->WindowInitialised(nullptr),
        cEvtMain->ThreadDeInit();
        cSystem->SetWindowDestroyed());
      // Execute main function until EMC_QUIT or EMC_QUIT_RESTART is passed.
      // We are using the system's main thread so we just need to name this
      // thread properly. We won't actually be spawning a new thread with
      // this though, it's just used as simple exit condition flag to be
      // compatible with the GUI mode.
      while(CoreShouldEngineContinue()) CoreThreadMain(*cEvtMain);
      // If system says we have to close as quickly as possible?
      if(cSystem->SysConIsClosing())
      { // Quickly save cvars, database and log, this is the priority since
        // Windows has a hardcoded termination time for console apps.
        cCVars->Save();
        cSql->DeInit();
        cLog->DeInitSafe();
        // Now Windows can exit anytime it wants
        cSystem->SysConCanCloseNow();
      }
    }
    // Compare engine exit code...
    switch(cEvtMain->GetExitReason())
    { // If we're to restart process with parameters? Set to do so
      case EMC_QUIT_RESTART:
        // Message to send
        cLog->LogWarningExSafe(
          "Core signalled to restart with $ parameters!",
          cCmdLine->CmdLineGetTotalCArgs());
        // Set exit procedure
        cCmdLine->CmdLineSetRestart(cSystem->IsTextMode() ?
          EO_TERM_REBOOT : EO_UI_REBOOT);
        // Have debugging enabled?
        if(cLog->HasLevel(LH_DEBUG))
        { // Log each argument that will be sent
          size_t stId = 0;
          for(const string &strArg : cCmdLine->CmdLineGetArgList())
            cLog->LogNLCDebugExSafe("- Arg $: $.", stId++, strArg);
        } // Clean-up and restart
        return 3;
      // If we're to restart process without parameters? Set to do so.
      case EMC_QUIT_RESTART_NP:
        // Put event in log
        cLog->LogWarningSafe(
          "Core signalled to restart without parameters!");
        // Set exit procedure
        cCmdLine->CmdLineSetRestart(cSystem->IsTextMode() ?
          EO_TERM_REBOOT_NOARG : EO_UI_REBOOT_NOARG);
        // Clean-up and restart
        return 4;
      // Normal exit (which is already set to EO_QUIT)
      default: break;
    } // Clean exit
    return 0;
  }
  /* -- Default constructor ------------------------------------------------ */
  Core() :
    /* -- Initialisers ----------------------------------------------------- */
#include "cvarlib.hpp"                 // Defines cvar list
    CVars{                             // Initialise cvars list
      static_cast<CVarItemStaticList&> // Grab the one we just made
        (*this) },
#include "conlib.hpp"                  // Defines console command list
    Console{                           // Initialise console commands list
      static_cast<ConCmdStaticList&>   // Grab the one we just made
        (*this) },
    cerMode{ CER_CRITICAL },           // Init lua error mode behaviour
    cbtMain{ bind(&Core::CoreThreadMain, this, _1) },
    uiErrorCount(0),                   // Init number of errors occured
    uiErrorLimit(0)                    // Init number of errors allowed
    /* -- Set global pointer to static class ------------------------------- */
    { cCore = this; }
  /* -- Process compatibility flags ---------------------------------------- */
  CVarReturn CoreProcessCompatibilityFlags(const uint64_t uqFlags)
  { // Nothing yet
    static_cast<void>(uqFlags);
    // Allow the cvar change
    return ACCEPT;
  }
  /* -- Set lua error mode behaviour --------------------------------------- */
  CVarReturn CoreErrorBehaviourModified(const CoreErrorReason cefNMode)
    { return CVarSimpleSetIntNGE(cerMode, cefNMode, CER_MAX); }
  /* -- Title modified ----------------------------------------------------- */
  CVarReturn CoreTitleModified(const string &strValue, string &strRValue)
  { // Do not allow user to set this variable, only empty is allowed
    if(!strValue.empty()) return DENY;
    // Set the title
    strRValue = StrAppend(cSystem->GetGuestTitle(), ' ',
      cSystem->GetGuestVersion(), " (", cSystem->ENGTarget(), ")");
    // We changed the value
    return ACCEPT_HANDLED;
  }
  /* -- Set once instance cvar changed ------------- Core::SetOneInstance -- */
  CVarReturn CoreSetOneInstance(const bool bEnabled)
  { // Ignore check if not needed or global mutex creation succeeded.
    if(!bEnabled || cSystem->InitGlobalMutex(cSystem->GetGuestTitle()))
      return ACCEPT;
    // De-initialise the log as we are about to exit.
    cLog->DeInitSafe();
    // Global mutex creation failed so exit program now.
    exit(5);
  }
  /* -- Set home directory where files are written if base dir dont work --- */
  CVarReturn CoreSetHomeDir(const string &strP, string &strV)
  { // Build user volatile directory name if user didn't specify one
    if(strP.empty())
      strV = StrFormat("$/$/$/", cSystem->GetRoamingDir(),
        cSystem->GetGuestAuthor().empty() ?
          cCommon->CommonUnspec() : cSystem->GetGuestAuthor(),
        cSystem->GetGuestShortTitle().empty() ?
          cCommon->CommonUnspec() : cSystem->GetGuestShortTitle());
    // Specified so use what the user specified
    else strV = StdMove(strP);
    // Try to make the users home directory as an alternative save place.
    if(!DirMkDirEx(PSplitBackToForwardSlashes(strV)))
      XCL("Failed to setup persistance directory!", "Directory", strV);
    // Set home directory and return that we set the value
    cCmdLine->CmdLineSetHome(strV);
    return ACCEPT_HANDLED;
  }
  /* -- Set error limit ---------------------------------------------------- */
  CVarReturn CoreSetResetLimit(const unsigned int uiLimit)
    { return CVarSimpleSetInt(uiErrorLimit, uiLimit); }
  /* -- Parses the command-line -------------------------------------------- */
  CVarReturn CoreParseCmdLine(const string&, string &strV)
  { // Get command line parameters and if we have parameters?
    const StrVector &svArgs = cCmdLine->CmdLineGetArgList();
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
        else if(const Token tKeyVal{ strArg, cCommon->CommonEquals(), 2 })
        { // Set the variable from command line with full permission because we
          // should allow any variable to be overridden from the command line.
          // Also show an error if the variable could not be set.
          if(cCVars->SetVarOrInitial(tKeyVal.front(), tKeyVal.size() > 1 ?
            tKeyVal.back() : cCommon->CommonBlank(),
            SCMDLINE|PCMDLINE, CCF_NOTHING))
          { // Append argument to accepted command line and add a space
            strV.append(strArg);
            strV.append(cCommon->CommonSpace());
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
};
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
