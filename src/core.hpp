/* == CORE.HPP ============================================================= **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## This module defines the 'Core' class and is created by the 'Engine' ## **
** ## class in the 'engine.cpp' module. It glues all the other engine     ## **
** ## subsystems together in a single class with also containing core     ## **
** ## logic to initialising and running the engine.                       ## **
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
using namespace IUrl::P;               using namespace IUtf::P;
using namespace IUtil::P;              using namespace IVideo::P;
using namespace Lib::OS::GlFW::Types;  using namespace Lib::OpenAL::Types;
using namespace Lib::Sqlite::Types;
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
  private Fonts,          private Videos,         private ConGfx,
  private Variables,      private Commands,       private Lua
{ /* -- Private typedefs to run a function when scope exits ---------------- */
  template<typename FuncType>struct ScopeGuard { FuncType ftFunc;
    explicit ScopeGuard(FuncType &&ftNFunc) : ftFunc(StdMove(ftNFunc)) {}
      DTORHELPER(~ScopeGuard, ftFunc()) }; // Use std::scoped_exit in C++23
  template<class FuncType>ScopeGuard<FuncType>
    MakeScopeGuard(FuncType &&ftFunc) {
      return ScopeGuard<FuncType>(StdForward<FuncType>(ftFunc)); }
  /* -- Private Variables -------------------------------------------------- */
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
    if(bLeaving && SqlEnd() != SQLITE_ERROR)
      cLog->LogWarningSafe("Core ended an in-progress SQL transaction!");
    // Reset all SQL error codes and stored results and records.
    SqlReset();
    // Clear console status bars. Although this only applies if there is a
    // terminal window, we don't restrict use of the function in graphical only
    // mode so clear it regardless of what the gui mode is.
    ConsoleClearStatus();
    // If using graphical inteactive mode?
    if(SysIsGraphicalMode())
    { // Reset input environment
      InputResetEnvironment();
      // Reset FBO clear colour, selected binds and 8-bit shader palette
      FboCoreResetClearColour();
      OglResetBinds();
      palDefault.Commit();
      // Set main framebuffer as default and reset to original settings
      FboCoreActivateMain();
      DisplayCommitDefaultMatrix();
      // Reset the cursor
      DisplayRequestResetCursor();
      // If leaving main execution?
      if(bLeaving)
      { // Enable and show console, and set full-screen
        ConGfxLeaveResetEnvironment();
        // Force a 1ms suspend lock to not hog the cpu
        TimerReset(true);
      } // If entering?
      else
      { // Disable and hide console, and restore size
        ConGfxEnterResetEnvironment();
        // Remove the 1ms FPS limit lock on the engine
        TimerReset(false);
      } // Make sure main FBO is cleared
      FboCoreSetDraw();
    } // Not graphical? Set or remove the 1ms FPS limit lock on the engine
    else TimerReset(bLeaving);
    // Reset unique ids. Remember some classes aren't registered in the
    // collector, such as the console and main FBO.
#define RSCEX(x,y,v) y::CLHelperBase::CtrReset(y::CollectorCount() + v)
#define RSCX(x,v) RSCEX(ICHelper ## x, x ## s, v)
#define RSC(x) RSCX(x, 0)
    RSC(Archive); RSC(Asset);    RSC(Atlase); RSC(Bin);      RSC(Clip);
    RSC(Command); RSCX(Fbo, 2);  RSC(File);   RSC(Font);     RSC(Ftf);
    RSC(Image);   RSC(ImageLib); RSC(Json);   RSC(LuaFunc);  RSC(Mask);
    RSC(Palette); RSC(Pcm);      RSC(PcmLib); RSC(Sample);   RSC(Shader);
    RSC(Socket);  RSC(Source);   RSC(SShot);  RSC(Stat);     RSC(Stream);
    RSC(Texture); RSC(Thread);   RSC(Url);    RSC(Variable); RSC(Video);
#undef RSC
#undef RSCX
#undef RSCEX
    // Reset socket packet and traffic counters
    SocketResetCounters();
    // Clear any lingering events which is very important because events from
    // the last sandbox may contain invalidated pointers and as long as they
    // don't reach the 'EvtMain::ManageSafe()' function we're fine.
    EvtMain::Flush();
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
    TimerUpdateBot();
    // Render the console FBO (if update requested)
    ConGfxRender();
    // Render video textures (if any)
    VideoRender();
    // Set main FBO by default on each frame
    FboCoreActivateMain();
    // Poll joysticks
    JoyPoll();
    // Execute a tick for each frame missed
    LuaExecuteMain();
    // Render the console FBO to the main FBO
    ConGfxRenderToMain();
    // Finish rendering the main FBO
    FboCoreGetMain().FboFinishAndRender();
    // Render all FBO's and copy the main FBO to screen
    FboCoreRender();
  }
  /* -- Graphical core window thread tick with frame limiter --------------- */
  void CoreTickFrameLimiter()
  { // Return if it is not time to execute a game tick
    if(TimerShouldNotTick()) return;
    // Render the console FBO (if update requested)
    ConGfxRender();
    // Render video textures (if any)
    VideoRender();
    // Set main FBO by default on each frame
    Catchup: FboCoreActivateMain();
    // Poll joysticks
    JoyPoll();
    // Execute a tick for each frame missed
    LuaExecuteMain();
    // What is the current draw condition?
    switch(FboCoreGetDraw())
    { // Must render everything?
      case DS_FULL:
        // We're behind? If true, we've already entered the next frame
        if(TimerShouldTick())
        { // Clear redraw flag but we still need to copy main to back buffer
          FboCoreClearDrawPartial();
          // Render the console FBO to the main FBO
          ConGfxRenderToMain();
          // Finish rendering the main FBO but don't update finish tickstamp
          FboCoreGetMain().FboFinishAndRenderUnsafe();
          // Try to catchup by ignoring video rendering and flipping buffers
          goto Catchup;
        } // Clear redraw flag
        FboCoreClearDraw();
        // Render the console FBO to the main FBO
        ConGfxRenderToMain();
        // Finish rendering the main FBO
        FboCoreGetMain().FboFinishAndRender();
        // Do the render
        FboCoreRender();
        // Done
        break;
      // No redrawing required this frame?
      case DS_NONE:
        // Try to catchup if we are behind
        if(TimerShouldTick()) goto Catchup;
        // Cannot draw so delete texture and FBO handles and finish
        OglPostRender();
        // Timer system can wait a little to not waste CPU cycles
        TimerForceWait();
        // Done
        break;
      // Only copy main to back buffer? Caused by being behind and DS_FULL
      case DS_PARTIAL:
        // Try to catchup if we are behind
        if(TimerShouldTick()) goto Catchup;
        // Clear redraw flag
        FboCoreClearDraw();
        // Do the render
        FboCoreRender();
        // Done
        break;
      // Anything else ignore
      default: break;
    }
  }
  /* -- Fired when Lua enters the sandbox ---------------------------------- */
  int CoreThreadSandbox(lua_State*const lS)
  { // Capture exceptions...
    try
    { // Clear thread exit status
      GetEngThread().ThreadCancelExit();
      // Compare event code. Do we need to execute scripts?
      switch(GetExitReason())
      { // Do not reinitialise anything if we processed a LUA error code
        case EMC_LUA_ERROR: break;
        // Lua execution was ended (e.g. use of the 'lend' command)
        case EMC_LUA_END:
          // Setup lua default environment (libraries, config, etc.)
          LuaSetupEnvironment();
          // Force timer delay to 1ms to prevent 100% thread use on Main*
          TimerSetDelayIfZero();
          // Exceptions from here on are recoverable
          SetExitReason(EMC_LUA_ERROR);
          // Done
          break;
        // The thread and window was reinitialised? (e.g. vreset command)
        case EMC_QUIT_VREINIT: [[fallthrough]];
        case EMC_QUIT_THREAD:
          // Tell guest scripts to redraw their FBO's
          EvtMain::Add(EMC_LUA_REDRAW);
          // Exceptions from here on are recoverable
          SetExitReason(EMC_LUA_ERROR);
          // Done
          break;
        // Lua executing was reinitialised?
        case EMC_LUA_REINIT:
          // Commit modified cvars. We know if we get this far, all the
          // configurations are valid so let's make sure they're saved!
          CVarsSaveToDatabase();
          // Fall through to default
          [[fallthrough]];
        // For anything else
        default:
          // Reset environment (entering)
          CoreResetEnvironment(false);
          // Setup lua default environment (libraries, config, etc.)
          LuaSetupEnvironment();
          // Default event code is error status. This is so if even c++
          // exceptions or C (LUA) exceptions occur, the underlying scope knows
          // to handle the error and try to recover. The actual loops will set
          // this to something different when they cleanly exit their loops.
          SetExitReason(EMC_LUA_ERROR);
          // Execute startup script
          LuaCodeExecuteFile(lS, GetStrInternal(LUA_SCRIPT));
          // Done
          break;
      } // Terminal mode requested?
      if(SysIsTextMode())
      { // Graphical mode requested too?
        if(SysIsGraphicalMode())
        { // Frame limiter enabled?
          if(SysIsTimerMode())
          { // Loop until event manager emits break
            while(EvtMain::HandleSafe())
            { // Execute unthreaded tick
              CoreTickFrameLimiter();
              // Process bot console
              ConsoleFlushToLog();
            }
          } // Frame limiter disabled so loop until event manager emits break
          else while(EvtMain::HandleSafe())
          { // Execute unthreaded tick
            CoreTickNoFrameLimiter();
            // Process bot console
            ConsoleFlushToLog();
          }
        } // Terminal mode only so loop until thread says we should break loop.
        else while(EvtMain::HandleSafe())
        { // Calculate time elapsed in this tick
          TimerUpdateBot();
          // Execute the main tick
          LuaExecuteMain();
          // Process bot console
          ConsoleFlushToLog();
        }
      } // Graphical mode requested?
      else if(SysIsGraphicalMode())
      { // Frame limiter enabled?
        if(SysIsTimerMode())
          // Loop until event manager says we should break
          while(EvtMain::HandleSafe()) CoreTickFrameLimiter();
          // Frame limiter disabled so loop without frame limiting
        else while(EvtMain::HandleSafe()) CoreTickNoFrameLimiter();
      } // No mode set
      else while(EvtMain::HandleSafe())
      { // Calculate time elapsed in this tick
        TimerUpdateBot();
        // Execute the main tick
        LuaExecuteMain();
        // Process bot console
        ConsoleFlushToTerminal();
      }
    } // exception occured so throw LUA stackdump and leave the sandbox
    catch(const exception &eReason)
    { // Allow Lue to process error. WARNING!! This prevents destructors on all
      // statically initialised classes to NEVER call so make sure we do not
      // statically create something above!
      LuaUtilPushCStr(lS, eReason.what());
      LuaUtilErrThrow(lS);
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
    LuaDeInit();
    // Reset environment (leaving)
    CoreResetEnvironment(true);
  }
  /* -- DeInitialise engine components ------------------------------------- */
  void CoreDeInitComponents() try
  { // Log reason for deinit
    cLog->LogDebugExSafe("Engine de-initialising interfaces with code $.",
      GetExitReason());
    // Request to close window
    DisplayRequestClose();
    // Whats the exit reason code?
    switch(GetExitReason())
    { // Quitting thread?
      case EMC_QUIT_VREINIT: [[fallthrough]];
      case EMC_QUIT_THREAD:
        // Not interactive mode? Nothing to de-initialise
        if(SysIsNotGraphicalMode()) return;
        // Unload console background and font textures
        ConGfxDeInitTextureAndFont();
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
        // If in graphical mode? De-init console graphics and input
        if(SysIsGraphicalMode()) { ConGfxDeInit(); InputDeInit(); }
        // DeInitialise console class and freetype
        ConsoleDeInit();
        FreeTypeDeInit();
        // De-init audio
        if(SysIsAudioMode()) AudioDeInit();
        // Done
        break;
    } // Unload all FBO's (NOT destroy);
    FboCoreDeInit();
    // De-init core shaders
    ShadersDeInit();
    // OpenGL de-initialised (do not throw error if de-initialised)
    OglDeInit(true);
  } // exception occured?
  catch(const exception &eReason)
  { // Make sure the exception is logged
    cLog->LogErrorExSafe("(ENGINE THREAD DE-INIT EXCEPTION) $", eReason);
  }
  /* -- Redraw the frame buffer when error occurs -------------------------- */
  void CoreForceRedrawFrameBuffer(const bool bAndConsole)
  { // Flush log if we have a text mode console
    if(SysIsTextMode()) ConsoleFlushToLog();
    // Return if no graphical mode
    if(SysIsNotGraphicalMode()) return;
    // Reset opengl binds to defaults just incase any were selected
    OglResetBinds();
    // Render the console and it has not already been drawn
    if(bAndConsole) ConGfxRenderNow();
    // Finish rendering the main FBO
    FboCoreGetMain().FboFinishAndRender();
    // Render all FBO's and copy the main FBO to screen
    FboCoreRender();
  }
  /* -- De-initialise everything ------------------------------------------- */
  void CoreDeInitEverything()
  { // De-initialise components
    CoreDeInitComponents();
    // If not in graphical mode, we're done
    if(SysIsNotGraphicalMode()) return;
    // Window should close as well
    WinSetClose(GLFW_TRUE);
    // Unblock the window thread
    GlFWForceEventHack();
  }
  /* -- Initoialise graphics subsystems ------------------------------------ */
  void CoreInitGraphicalSubsystems()
  { // Set context current and pass selected refresh rate
    OglInit(DisplayGetRefreshRate());
    // Initialise core shaders
    ShadersInit();
    // If we're initialising for the first time?
    if(IsExitReason(EMC_NONE))
    { // Initialise freetype, console render, audio and input classes
      FreeTypeInit();
      ConsoleInit();
      ConGfxInit();
      InputInit();
    } // Not initialising for the first time?
    else
    { // Reinitialise systems that rely on OpenGL and/or GLFW.
      DisplayCommitMatrix();
      DisplayUpdateIcons();
      FboReInit();
      ConGfxReInitTextureAndFont();
      FontReInitTextures();
      TextureReInitTextures();
      VideoReInitTextures();
      InputCommitCursor();
    } // Show the window
    DisplayRequestOpen();
  }
  /* -- Engine thread (member function) ------------------------------------ */
  ThreadStatus CoreThreadMain(Thread&) try
  { // Log reason for init
    cLog->LogDebugExSafe("Core engine thread started (C:$;M:$<$>).",
      GetExitReason(), SysGetCoreFlagsString(), SysGetCoreFlags());
    // Non-interactive mode?
    if(SysIsTextMode())
    { // Have interactive mode too? Init graphical systems
      if(SysIsGraphicalMode()) CoreInitGraphicalSubsystems();
      // Not interactive mode so init FT and console if not a first time init
      else if(IsExitReason(EMC_NONE)) { FreeTypeInit(); ConsoleInit(); }
      // Not initialising for first time so load icons
      else DisplayUpdateIcons();
    } // Init interactive console?
    else if(SysIsGraphicalMode()) CoreInitGraphicalSubsystems();
    // With audio mode enabled? Initialise audio class.
    if(SysIsAudioMode() && IsExitReason(EMC_NONE)) AudioInit();
    // Lua loop with initialisation. Compare event code
    SandBoxInit: switch(GetExitReason())
    { // Ignore LUA initialisation if we're reinitialising other components
      case EMC_QUIT_VREINIT: [[fallthrough]];
      case EMC_QUIT_THREAD: break;
      // Any other code will initialise LUA
      default: LuaInit();
    } // Lua loop without initialisation. Begin by capturing exceptions
    SandBox: try
    { // ...and enter sand box mode. Below function is when we're in sandbox
      LuaEnterSandbox<CoreThreadSandboxStatic>(this);
    } // ...and if exception occured?
    catch(const exception &eReason)
    { // Show error in console
      ConsoleAddLine(COLOUR_LRED, eReason.what());
      // Disable garbage collector so no shenangians while we reset.
      LuaStopGC();
      // Reset glfw errorlevel
      GlFWResetErrorLevel();
      // Check event code that was set?
      switch(GetExitReason())
      { // Run-time error?
        case EMC_LUA_ERROR:
          // If we are not in the exit script?
          if(!LuaExiting())
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
                LuaReInit();
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
                LuaRequestPause(false);
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
          UpdateConfirmExit();
          // Check exit code. These are originally set in EvtMain::DoHandle()
          // so make sure all used exit values with ConfirmExit() are checked
          // for here.
          switch(GetExitReason())
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
                GetExitReason());
              // Fall through to EMC_QUIT_RESTART
              [[fallthrough]];
            // Quitting and restarting?
            case EMC_QUIT_RESTART: [[fallthrough]];
            // Quitting and restarting with no parameters?
            case EMC_QUIT_RESTARTNP: [[fallthrough]];
            // Thread and main thread should quit so tell thread to break.
            case EMC_QUIT: throw;
          } // We get here if we're going to the choices
          break;
        // Unknown exit reason?
        default:
          // Report it
          cLog->LogWarningExSafe("Core has unknown exit reason of $!",
            GetExitReason());
          // Fall through to EMC_LUA_END
          [[fallthrough]];
        // Lua is ending execution? Shouldn't happen.
        case EMC_LUA_END: [[fallthrough]];
        // Lua executing is reinitialising? Shouldn't happen.
        case EMC_LUA_REINIT: [[fallthrough]];
        // Quitting and restarting? Shouldn't happen.
        case EMC_QUIT_RESTART: [[fallthrough]];
        // Quitting and restarting without params? Shouldn't happen.
        case EMC_QUIT_RESTARTNP: [[fallthrough]];
        // Thread and main thread should quit. De-init components and rethrow.
        case EMC_QUIT: CoreDeInitComponents(); throw;
      } // We get here to process lua event as normal.
    } // Why did LUA break out of the sandbox?
    switch(GetExitReason())
    { // Execution ended? (e.g. 'lend' command was used)
      case EMC_LUA_END:
        // Add message to say the execution ended
        ConsoleAddLine("Execution ended! Type 'lreset' to restart.");
        // De-initialis lua
        CoreLuaDeInitHelper();
        // Reinitialise lua and go back into the sandbox
        goto SandBoxInit;
      // Execution reinitialising? (e.g. 'lreset' command was used)
      case EMC_LUA_REINIT:
        // Add message to say the execution is restarting
        ConsoleAddLine("Execution restarting...");
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
      case EMC_QUIT_RESTARTNP: [[fallthrough]];
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
    SetExitReason(EMC_QUIT);
    // Write exception to log
    cLog->LogErrorExSafe("(ENGINE THREAD FATAL EXCEPTION) $", eReason);
    // Show error
    SysMsgEx("Engine Thread Fatal Exception", eReason.what(), MB_ICONSTOP);
    // De-init everything
    CoreDeInitEverything();
    // Kill thread
    return TS_ERROR;
  }
  /* -- Engine should continue? -------------------------------------------- */
  bool CoreShouldEngineContinue()
  { // Compare exit value
    switch(GetExitReason())
    { // Engine was requested to quit or restart? NO!
      case EMC_QUIT: [[fallthrough]];
      case EMC_QUIT_RESTART: [[fallthrough]];
      case EMC_QUIT_RESTARTNP: return false;
      // Systems were just reinitialising? YES!
      default: return true;
    }
  }
  /* -- Initialise graphical mode ------------------------------------------ */
  void CoreEnterGraphicalMode()
  { // De-initialise GLFW when this scope exists
    const auto sgGDeInit{ MakeScopeGuard([this]{ GlFWDeInit(); }) };
    // Initialise GLFW
    GlFWInit();
    // Until engine should terminate.
    while(CoreShouldEngineContinue()) try
    { // De-initialise display and thread when this scope exists
      const auto sgWDeInit{
        MakeScopeGuard([this]{
          GetEngThread().ThreadDeInit();
          DisplayDeInit();
        }) };
      // Initialise window
      DisplayInit();
      // Setup main thread and start it
      Restart: GetEngThread().ThreadInit(cbtMain, nullptr);
      // Loop until window should close
      while(WinShouldNotClose())
      { // Process window event manager commands from other threads
        EvtWin::Manage();
        // Wait for more window events
        GlFWWaitEvents();
      } // Restart to hard reinit the window if not doing a soft reinit
      if(GetExitReason() != EMC_QUIT_VREINIT) continue;
      // De-initialise the thread
      GetEngThread().ThreadDeInit();
      // Soft reinitialise the window
      DisplayReInit();
      // Go back to the thread restart point
      goto Restart;
    } // Error occured
    catch(const exception &eReason)
    { // Send to log and show error message to user
      cLog->LogErrorExSafe("(WINDOW LOOP EXCEPTION) $", eReason);
      // Exit loop so we don't infinite loop
      SetExitReason(EMC_QUIT);
      // Show error and try to carry on and clean everything up
      SysMsgEx("Window Loop Fatal Exception", eReason.what(), MB_ICONSTOP);
   } // Engine should terminate from here-on
  }
  /* -- Wait async on all systems ---------------------------------- */ public:
  void CoreWaitAllAsync()
  { // Log sychronisation result from all subsystems that use threads
    Archives::MutexScopedCall([this](){
      cLog->LogDebugExSafe("Core synchronised $ objects.",
        Archives::size() + AssetsCtr::size() + FtfsCtr::size() +
        ImagesCtr::size() + JsonsCtr::size() + PcmsCtr::size() +
        SourcesCtr::size() + StreamsCtr::size() + VideosCtr::size());
    }, Assets::MutexGet(), Ftfs::MutexGet(), Images::MutexGet(),
       Jsons::MutexGet(), Pcms::MutexGet(), Sources::MutexGet(),
       Streams::MutexGet(), Videos::MutexGet());
  }
  /* -- Main function ------------------------------------------------------ */
  int CoreMain()
  { // Register default cvars and pass over the current gui mode by ref. All
    // the core parts of the engine are initialised from cvar callbacks.
    CVarsInit();
    // Text mode requested?
    if(SysIsTextMode())
    { // Bail out if logging to standard output because this will prevent
      // the text mode from working properly
      if(cLog->LogIsRedirectedToDevice())
        XC("Text console cannot be used when logging to standard output!");
      // De-init text mode console when leaving scope
      const auto sgTDeInit{ MakeScopeGuard([this]{
        GetEngThread().ThreadDeInit();
        SetWindowDestroyed();
        SysConDeInit();
      }) };
      // Initialise terminal
      SysConInit(SysGetGuestTitle().data(),
        CVarsGetInternal<unsigned int>(CON_TMCCOLS),
        CVarsGetInternal<unsigned int>(CON_TMCROWS),
        CVarsGetInternal<bool>(CON_TMCNOCLOSE));
      // There is no GLFW window so maybe the system can get it instead?
      WindowInitialised(nullptr);
      // Perform the initial draw of the console.
      ConsoleFlushToLog();
      // Graphical mode requested too?
      if(SysIsGraphicalMode()) CoreEnterGraphicalMode();
      // Only text mode requested?
      else
      { // Reset window icon
        DisplayUpdateIcons();
        // Execute main function until EMC_QUIT or EMC_QUIT_RESTART is passed.
        // We are using the system's main thread so we just need to name this
        // thread properly. We won't actually be spawning a new thread with
        // this though, it's just used as simple exit condition flag to be
        // compatible with the GUI mode.
        while(CoreShouldEngineContinue()) CoreThreadMain(GetEngThread());
      } // If system says we have to close as quickly as possible?
      if(SysConIsClosing())
      { // Quickly save cvars, database and log, this is the priority since
        // Windows has a hardcoded termination time for console apps.
        CVarsSaveToDatabase();
        SqlDeInit();
        cLog->LogDeInitSafe();
        // Now Windows can exit anytime it wants
        SysConCanCloseNow();
      }
    } // Else were in graphical interactive mode
    else if(SysIsGraphicalMode()) CoreEnterGraphicalMode();
    // No front-end requested so we just use stdout
    else
    { // De-init window when leaving scope
      const auto sgWDeInit{ MakeScopeGuard([this]{
        GetEngThread().ThreadDeInit();
        SetWindowDestroyed();
      }) };
      // Init lightweight text mode console for monitoring.
      WindowInitialised(nullptr);
      // Execute main function until EMC_QUIT or EMC_QUIT_RESTART is passed.
      // We are using the system's main thread so we just need to name this
      // thread properly. We won't actually be spawning a new thread with
      // this though, it's just used as simple exit condition flag to be
      // compatible with the GUI mode.
      while(CoreShouldEngineContinue()) CoreThreadMain(GetEngThread());
      // If system says we have to close as quickly as possible?
      if(SysConIsClosing())
      { // Quickly save cvars, database and log, this is the priority since
        // Windows has a hardcoded termination time for console apps.
        CVarsSaveToDatabase();
        SqlDeInit();
        cLog->LogDeInitSafe();
        // Now Windows can exit anytime it wants
        SysConCanCloseNow();
      }
    }
    // Compare engine exit code...
    switch(GetExitReason())
    { // If we're to restart process with parameters? Set to do so
      case EMC_QUIT_RESTART:
        // Message to send
        cLog->LogWarningExSafe("Core signalled to restart with $ parameters!",
          cCmdLine->CmdLineGetTotalCArgs());
        // Set exit procedure
        cCmdLine->CmdLineSetRestart(SysIsTextMode() ?
          EO_TERM_REBOOT : EO_UI_REBOOT);
        // Have debugging enabled?
        if(cLog->LogHasLevel(LH_DEBUG))
        { // Log each argument that will be sent
          size_t stId = 0;
          for(const string &strArg : cCmdLine->CmdLineGetArgList())
            cLog->LogNLCDebugExSafe("- Arg $: $.", stId++, strArg);
        } // Clean-up and restart
        return 3;
      // If we're to restart process without parameters? Set to do so.
      case EMC_QUIT_RESTARTNP:
        // Put event in log
        cLog->LogWarningSafe("Core signalled to restart without parameters!");
        // Set exit procedure
        cCmdLine->CmdLineSetRestart(SysIsTextMode() ?
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
    CVars{ static_cast<CVarItemStaticList&>(*this) },
#include "conlib.hpp"                  // Defines console command list
    Console{ static_cast<ConCmdStaticList&>(*this) },
    cerMode{ CER_CRITICAL },           // Init lua error mode behaviour
    cbtMain{ bind(&Core::CoreThreadMain, this, _1) },
    uiErrorCount(0),                   // Init number of errors occured
    uiErrorLimit(0)                    // Init number of errors allowed
    /* -- Set global pointer to static class ------------------------------- */
    { cCore = this; }
  /* -- Process compatibility flags ---------------------------------------- */
  CVarReturn CoreProcessCompatibilityFlags(const uint64_t ullFlags)
  { // Nothing yet
    static_cast<void>(ullFlags);
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
    strRValue = StrFormat("$ ($)", SysGetGuestTitle(), ENGTarget());
    // We changed the value
    return ACCEPT_HANDLED;
  }
  /* -- Set once instance cvar changed ------------- Core::SetOneInstance -- */
  CVarReturn CoreSetOneInstance(const bool bEnabled)
  { // Ignore check if not needed or global mutex creation succeeded.
    if(!bEnabled || InitGlobalMutex(SysGetGuestTitle())) return ACCEPT;
    // De-initialise the log as we are about to exit.
    cLog->LogDeInitSafe();
    // Global mutex creation failed so exit program now.
    exit(5);
  }
  /* -- Set home directory where files are written if base dir dont work --- */
  CVarReturn CoreSetHomeDir(const string &strP, string &strV)
  { // Build user volatile directory name if user didn't specify one
    if(strP.empty())
      strV = StrFormat("$/$/$/", SysGetRoamingDir(),
        SysGetGuestAuthor().empty() ?
          cCommon->CommonUnspec() : SysGetGuestAuthor(),
        SysGetGuestShortTitle().empty() ?
          cCommon->CommonUnspec() : SysGetGuestShortTitle());
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
          if(CVarsSetVarOrInitial(tKeyVal.front(), tKeyVal.size() > 1 ?
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
