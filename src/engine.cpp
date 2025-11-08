/* == ENGINE.CPP =========================================================== **
** ######################################################################### **
** ##*@@****@@*@@***@@**@@@@@**@@****@@***@@*@@@@@**@@*****@@@@@@**@@@@@**## **
** ##*@@@@@@@@*@@***@@*@@***@@*@@@@@@*@@*@@*@@***@@*@@@@@@***@@***@@***@@*## **
** ##*@@*@@*@@*@@@@@@@*@@@@@@@*@@******@@@**@@***@@*@@*******@@***@@******## **
** ##*@@****@@*@@***@@*@@***@@*@@**@@*@@*@@*@@***@@*@@*******@@***@@***@@*## **
** ##*@@****@@*@@***@@*@@***@@**@@@@*@@***@@**@@@@***@@@@@*@@@@@@**@@@@@**## **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## This the file that handles the inclusing of engine subsystems in a  ## **
** ## tidy namespace and handles the main entry point.                    ## **
** ######################################################################### **
** ========================================================================= */
#include "setup.hpp"                   // Setup the compilation environment
/* ------------------------------------------------------------------------- */
namespace E {                          // Start of engine namespace
/* ------------------------------------------------------------------------- */
#include "engine.hpp"                  // Engine version information header
#include "stdtypes.hpp"                // Engine STL type aliases header
#include "common.hpp"                  // Common constant variables header
#include "flags.hpp"                   // Flags helper utility header
#include "utf.hpp"                     // UTF strings utility header
#include "std.hpp"                     // StdLib function helpers header
#include "string.hpp"                  // String utilities header
#include "error.hpp"                   // Error handling utility header
#include "token.hpp"                   // String tokenisation utility header
#include "parser.hpp"                  // String parsing utility header
#include "psplit.hpp"                  // Path handling utilities header
#include "ident.hpp"                   // Identifier utility header
#include "cvardef.hpp"                 // CVar definitions header
#include "dir.hpp"                     // Directory handling utility header
#include "util.hpp"                    // Miscellenious utilities header
#include "sysutil.hpp"                 // System utilities header
#include "clock.hpp"                   // Clock utilities header
#include "timer.hpp"                   // Timing utilities header
#include "args.hpp"                    // Arguments handling header
#include "cmdline.hpp"                 // Command-line class header
#include "memory.hpp"                  // Memory management utilities header
#include "fstream.hpp"                 // File IO utility header
#include "mutex.hpp"                   // Mutex helper class
#include "log.hpp"                     // Logging helper class header
#include "luadef.hpp"                  // Lua definitions header
#include "luaident.hpp"                // Lua ident class header
#include "ihelper.hpp"                 // Init helper class header
#include "lockable.hpp"                // Lockable class header
#include "collect.hpp"                 // Class collector utility header
#include "stat.hpp"                    // Statistic utility class header
#include "thread.hpp"                  // Thread helper class header
#include "evtcore.hpp"                 // Thread-safe event system core header
#include "evtmain.hpp"                 // Main engine events system header
#include "glfwutil.hpp"                // GLFW utility class header
#include "evtwin.hpp"                  // Window thread events system header
#include "glfwcrsr.hpp"                // GLFW cursor class header
#include "intpair.hpp"                 // Integer pair class header
#include "coord.hpp"                   // Co-oridinates class header
#include "rectangl.hpp"                // Rectangle class header
#include "dim.hpp"                     // Dimensions class header
#include "dimcoord.hpp"                // Joined dim/coord class header
#include "glfwwin.hpp"                 // GLFW window class header
#include "glfw.hpp"                    // GLFW utilities header
#include "glfwmon.hpp"                 // GLFW monitor class header
#include "condef.hpp"                  // Console definitions header
#include "syscore.hpp"                 // Operating system interface header
#include "filemap.hpp"                 // Virtual file IO interface
#include "refctr.hpp"                  // Reference counter class header
#include "luautil.hpp"                 // Lua utility functions header
#include "luaref.hpp"                  // Lua reference helper class header
#include "luaevent.hpp"                // Lua event helper class header
#include "luafunc.hpp"                 // Lua callback helper class header
#include "async.hpp"                   // Async file loading class header
#include "crypt.hpp"                   // Cryptography utilities header
#include "url.hpp"                     // Url parsing library
#include "uuid.hpp"                    // UuId parsing header
#include "codec.hpp"                   // Codec classes header
#include "credits.hpp"                 // Credits handling class header
#include "archive.hpp"                 // Archive handling class header
#include "asset.hpp"                   // Asset handling class header
#include "sql.hpp"                     // SQL database management header
#include "cert.hpp"                    // X509 certificate store class header
#include "cvar.hpp"                    // CVar item header
#include "luacode.hpp"                 // Lua code subsystem header
#include "json.hpp"                    // Json handling class header
#include "cvars.hpp"                   // CVars management class header
#include "socket.hpp"                  // Socket handling class header
#include "console.hpp"                 // Console handling header
#include "oal.hpp"                     // OpenAL audio header
#include "pcmdef.hpp"                  // Pcm definitions header
#include "dformat.hpp"                 // Data format helper header
#include "pcmlib.hpp"                  // Pcm codecs handling header
#include "pcmwav.hpp"                  // PcmLib WAV file codec
#include "pcmcaf.hpp"                  // PcmLib CAF file codec
#include "pcmogg.hpp"                  // PcmLib OGG file codec
#include "pcmmp3.hpp"                  // PcmLib MP3 file codec
#include "pcm.hpp"                     // Pcm loader class header
#include "fbodef.hpp"                  // Frambuffer object definitions header
#include "texdef.hpp"                  // Texture data definitions header
#include "ogl.hpp"                     // OpenGL graphics management header
#include "imagedef.hpp"                // Image data definitions header
#include "imagelib.hpp"                // Image codecs handling header
#include "imagedds.hpp"                // ImageLib DDS file codec
#include "imagegif.hpp"                // ImageLib GIF file codec
#include "imagepng.hpp"                // ImageLib PNG file codec
#include "imagejpg.hpp"                // ImageLib JPG file codec
#include "bin.hpp"                     // Bin packing class header
#include "image.hpp"                   // Image load and save handling header
#include "shader.hpp"                  // OpenGL Shader handling header
#include "shaders.hpp"                 // Actual shaders core
#include "fboitem.hpp"                 // Frame buffer object item class header
#include "fbo.hpp"                     // Frame buffer object class header
#include "fbocore.hpp"                 // Core frame buffer object class header
#include "sshot.hpp"                   // Screenshot handling class header
#include "texture.hpp"                 // Texture handling class header
#include "atlas.hpp"                   // Atlas handling class header
#include "palette.hpp"                 // Palette handling class header
#include "ft.hpp"                      // Freetype base class
#include "ftf.hpp"                     // Freetype font handling class header
#include "font.hpp"                    // Font loading and printing header
#include "file.hpp"                    // FStream+FileMap class header
#include "clip.hpp"                    // Clipboard class header
#include "congraph.hpp"                // Console rendering class header
#include "joyaxis.hpp"                 // Joystick axis class header
#include "joybutton.hpp"               // Joystick button class header
#include "joyinfo.hpp"                 // Joystick info class header
#include "joystick.hpp"                // Joystick structure class header
#include "input.hpp"                   // Input handling class header
#include "display.hpp"                 // Window handling class header
#include "mask.hpp"                    // BitMask system header
#include "source.hpp"                  // Audio source class header
#include "stream.hpp"                  // Audio stream class header
#include "sample.hpp"                  // Audio sample class header
#include "video.hpp"                   // Theora video playback class header
#include "audio.hpp"                   // Audio base management class header
#include "luavar.hpp"                  // Lua variable class
#include "luacmd.hpp"                  // Lua console command class
#include "lua.hpp"                     // Lua core subsystem header
#include "core.hpp"                    // Core class header
#include "lualib.hpp"                  // Lua function API library
/* ------------------------------------------------------------------------- */
};                                     // End of engine namespace
/* == The main entry point ================================================= */
int ENTRYFUNC                          // Macro defined in 'setup.hpp'
{ // Includes required to build the engine.
  using namespace E;                   using namespace ISysUtil::P;
  using namespace IStd::P;
  // Initialise and label the main thread
  SysInitThread("main", STP_MAIN);
  // Capture exceptions.
  try
  { // Create the base systems, run main procedure and return result
    using namespace ICmdLine::P;       using namespace ICommon::P;
    using namespace ICore::P;          using namespace IDir::P;
    using namespace ILog::P;
    // Create engine class
    struct Engine final :
      // Base classes required to run the main part of the engine.
      private Common, private DirBase, private CmdLine, private Log
    { // Main procedure into running the engine
      int EngineMain() const try { return Core{}.CoreMain(); }
      // Safe loggable exception occured?
      catch(const exception &eReason)
      { // Send to log and show error message to user. Show message box and
        // return error status.
        cLog->LogErrorExSafe("(MAIN THREAD FATAL EXCEPTION) $", eReason);
        SysMessage("Main Thread Exception", eReason.what(), MB_ICONSTOP);
        return 2;
      }
      // Constructor
      Engine(const int iArgs,          // Arguments count
             ArgType**const lArgs,     // Arguments array
             ArgType**const lEnv) :    // Environment variables array
        // Initialisers
        CmdLine{ iArgs, lArgs, lEnv }  // Initialise command-line arg
        // No code
        {}
    };
    // Create the engine object, run the main function and return its result
    return Engine{ __argc, __wargv, _wenviron }.EngineMain();
  } // Unsafe exception occured?
  catch(const exception &eReason)
  { // Show message box and return error status.
    SysMessage("Main Init Exception", eReason.what(), MB_ICONSTOP);
    return 1;
  }
}
/* == End-of-File ========================================================== */
