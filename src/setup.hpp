/* == SETUP.HPP ============================================================ **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## This module sets up the environment for the game engine. For        ## **
** ## example. Making the build process compatible with different         ## **
** ## compilers.                                                          ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* == Build type setup ===================================================== */
#if defined(RELEASE)                   // Final public release version?
# if defined(BETA)||defined(ALPHA)     // Make sure none of the others defined
#   error Do not mix BETA or ALPHA with RELEASE!
# endif                                // Incompatible symbol check
# define BUILD_TYPE_LABEL              "Release"
#elif defined(BETA)                    // Beta private release version?
# if defined(RELEASE)||defined(ALPHA)  // Make sure none of the others defined
#   error Do not mix RELEASE or ALPHA with BETA!
# endif                                // Incompatible symbol check
# define BUILD_TYPE_LABEL              "Beta"
#elif defined(ALPHA)                   // Alpha/Debug private release version?
# if defined(RELEASE)||defined(BETA)   // Make sure none of the others defined
#   error Do not mix RELEASE or BETA with ALPHA!
# endif                                // Incompatible symbol check
# define BUILD_TYPE_LABEL              "Alpha"
#else                                  // Not specified? (error!)
# error Release type of either RELEASE, BETA or ALPHA was not defined.
#endif                                 // End of release type check
/* == Compiled type setup ================================================== */
#if !defined(__cplusplus)              // Must be compiling in C++ mode
# error Please use a C++ compiler such as GCC, CLANG or MSVC!
#elif __cplusplus < 202002L            // Must be using a C++20 compiler
# error Must use a C++ 20 or better compiler!
#elif defined(_WIN32)                  // Only applies to MS compiler
# define WINDOWS                       // Using windows
# if defined(ALPHA)                    // Debugging?
#  if !defined(_DEBUG)                 // Might not be defined on LLVM
#   define _DEBUG                      // Not defined on CLang
#  endif                               // _DEBUG define check
#  define _CRTDBG_MAP_ALLOC            // >> Required by MSDN
# elif defined(BETA)||defined(RELEASE) // Beta mode defined?
#  ifndef NDEBUG                       // Clang-cl will define this
#   define NDEBUG                      // No assertions
#  endif                               // NDEBUG check
#  define _NDEBUG                      // No assertions
# endif                                // Debugging check
# if defined(__MINGW64__)              // Using MINGW64 compiler?
#  define COMPILER_NAME                "MingW"
#  define COMPILER_VERSION             STR(__MINGW64_MAJOR_VERSION) "." \
                                       STR(__MINGW64_MINOR_VERSION) "." \
                                       STR(__MINGW64_PATCHLEVEL)
# elif defined(__MINGW32__)            // Using MINGW32 compiler?
#  define COMPILER_NAME                "MingW"
#  define COMPILER_VERSION             STR(__MINGW32_MAJOR_VERSION) "." \
                                       STR(__MINGW32_MINOR_VERSION) "." \
                                       STR(__MINGW32_PATCHLEVEL)
# elif defined(__clang__)              // Actually using CLang?
#  define COMPILER_NAME                "CLang"
#  define COMPILER_VERSION             STR(__clang_major__) "." \
                                       STR(__clang_minor__) "." \
                                       STR(__clang_patchlevel__)
# elif defined(_MSC_VER)               // Actually using Microsoft compiler?
#  if _MSC_VER < 1911                  // Is correct version?
#   error Must use Microsoft C++ Compiler v19.11 or better!
#  endif                               // Microsoft version check
#  define COMPILER_NAME                "MSVisualC"
#  define COMPILER_VERSION             STR(_MSC_VER)
# else                                 // Unsupported compiler?
#  error This compiler is not recognised. Please use clang or msvc!
# endif                                // Check actual compiler
# if defined(_M_AMD64)||defined(__MINGW64__) // 64-bit compilation?
#  define WINVER                0x0502 // Target Windows XP 64-bit or higher
#  define CISC                         // Using INTEL or AMD instruction set
#  define X64                          // Using X64 architechture
#  if defined(__AVX2__)                // Target has AVX2 instructions?
#   define BUILD_TARGET                "Win-X64-AVX2"
#  elif defined(__AVX__)               // Target has AVX instructions?
#   define BUILD_TARGET                "Win-X64-AVX"
#  else                                // Generic AMD64?
#   define BUILD_TARGET                "Win-X64-SSE2"
#  endif                               // Floating point checks
# elif defined(_M_IX86)||defined(__MINGW32__) // 32-bit compilation?
#  define WINVER                0x0501 // Target Windows XP 32-bit or higher
#  define CISC                         // Using INTEL or AMD instruction set
#  define X86                          // Using X86 architechture
#  if _M_IX86_FP == 1                  // Target FP is SSE?
#   define BUILD_TARGET                "Win-X86-SSE"
#  elif _M_IX86_FP == 2                // Target FP is SSE2?
#   if defined(__AVX2__)               // Target has AVX2 instructions?
#    define BUILD_TARGET               "Win-X86-AVX2"
#   elif defined(__AVX__)              // Target has AVX instructions?
#    define BUILD_TARGET               "Win-X86-AVX"
#   else                               // Target has SSE2 instructions?
#    define BUILD_TARGET               "Win-X86-SSE2"
#   endif                              // SSE2 type checks
#  else                                // Target FP is generic IA32?
#   define BUILD_TARGET                "Win-X86"
#  endif                               // Floating point checks
# elif defined(_M_ARM64)               // Target is ARM64 architecture?
#  pragma message("This Windows target is not tested. Please give feedback!")
#  define WINVER                0x0A00 // Target Windows 11 or higher
#  define RISC                         // Using ARM instruction set
#  define X64                          // Using 64-bit architechture
#  define BUILD_TARGET                 "Win-ARM64"
# else                                 // Target is unknown?
#  error This Windows target being compiled with is not supported!
# endif                                // Target architechture detection
# define _WIN32_WINNT           WINVER // Target Windows XP or higher
# define WIN32_LEAN_AND_MEAN           // Faster compilation of headers
# define VC_EXTRALEAN                  // Extra lean Visual C headers
# define UNICODE                       // Using native Windows functions
# define _UNICODE                      //  prevents conversions & allocations
# define NOMINMAX                      // Do not define min/max please
#elif defined(__APPLE__)               // Apple target?
# define MACOS                         // Using MacOS
# include <TargetConditionals.h>       // Include target conditionals header
# if TARGET_OS_MAC == 1                // Targeting MacOS?
#  if defined(__x86_64__)              // 64-bit x86 architecture?
#   define CISC                        // Using INTEL or AMD instruction set
#   define X64                         // Using 64-bit architechture
#   define BUILD_TARGET                "MacOS-X64"
#  elif defined(__arm64__)             // 64-bit ARM architecture?
#   define RISC                        // Using ARM instruction set
#   define X64                         // Using 64-bit architechture
#   if __ARM_NEON__ == 1               // Neon instructions enabled?
#    define BUILD_TARGET               "MacOS-ARM64"
#   else                               // Neon extensions not enabled?
#    define BUILD_TARGET               "MacOS-ARM64-XNEON"
#   endif                              // Neon extensions check
#  else                                // Unknown target?
#   error This MacOS architechture being compiled with is not supported!
#  endif                               // Done checking architechture
#  include <AvailabilityMacros.h>      // Get minimum operating system version
#  if MAC_OS_X_VERSION_MIN_REQUIRED < MAC_OS_X_VERSION_10_15
#   error This project recommends at least MacOS 10.15 Catalina or later!
#  endif                               // MacOS version check
# else                                 // Not targeting MacOS?
#  error This Apple target being compiled with is not yet supported!
# endif                                // Target check
# define COMPILER_NAME                 "LLVM"
# define COMPILER_VERSION              STR(__GNUC__) "." \
                                       STR(__GNUC_MINOR__) "." \
                                       STR(__GNUC_PATCHLEVEL__)
# define _DARWIN_USE_64_BIT_INODE      // For 64-bit size values
# define _LIBCPP_ENABLE_CXX17_REMOVED_UNEXPECTED_FUNCTIONS // On in MSVC & GCC
#elif defined(__linux__)               // Linux detected?
# define LINUX                         // Using Linux
# define COMPILER_NAME                 "GCC"
# define COMPILER_VERSION              STR(__GNUC__) "." \
                                       STR(__GNUC_MINOR__) "." \
                                       STR(__GNUC_PATCHLEVEL__)
# if defined(__x86_64__)               // 64-bit compilation?
#  define CISC                         // Using INTEL or AMD instruction set
#  define X64                          // Using 64-bit architechture
#  if defined(__AVX2__)                // Using AVX2 instructions
#   define BUILD_TARGET                "Linux-X64-AVX2"
#  elif defined(__AVX__)               // Using AVX instructions
#   define BUILD_TARGET                "Linux-X64-AVX"
#  elif defined(__SSE3__)              // Using SSE3 instructions
#   define BUILD_TARGET                "Linux-X64-SSE3"
#  elif defined(__SSE2__)              // Using SSE2 instructions
#   define BUILD_TARGET                "Linux-X64-SSE2"
#  elif defined(__SSE__)               // Using SSE instructions
#   define BUILD_TARGET                "Linux-X64-SSE"
#  else                                // Using generic instructions
#   define BUILD_TARGET                "Linux-X64"
#  endif                               // FP extensions check
# elif defined(__i386__)               // 32-bit compilation?
#  define CISC                         // Using INTEL or AMD instruction set
#  define X86                          // Using 32-bit architechture
#  if defined(__AVX2__)                // Using AVX2 instructions
#   define BUILD_TARGET                "Linux-X86-AVX2"
#  elif defined(__AVX__)               // Using AVX instructions
#   define BUILD_TARGET                "Linux-X86-AVX"
#  elif defined(__SSE3__)              // Using SSE3 instructions
#   define BUILD_TARGET                "Linux-X86-SSE3"
#  elif defined(__SSE2__)              // Using SSE2 instructions
#   define BUILD_TARGET                "Linux-X86-SSE2"
#  elif defined(__SSE__)               // Using SSE instructions
#   define BUILD_TARGET                "Linux-X86-SSE"
#  else                                // Using generic instructions
#   define BUILD_TARGET                "Linux-X86"
#  endif                               // FP extensions check
# else                                 // Invalid architecture?
#  error This Linux target being compiled with is not supported!
# endif                                // Linux arch check
#else                                  // Unsupported target environment
# error This platform being compiled with is not supported!
#endif                                 // Using Microsoft compiler
/* -- Base defines --------------------------------------------------------- */
#define DEFAULT_CONFIGURATION    "app" // Default app configuration filename
#define ARCHIVE_EXTENSION        "adb" // Default archive extension
#define LOG_EXTENSION            "log" // Default log file extension
#define UDB_EXTENSION            "udb" // Default database file extension
#define JSON_EXTENSION          "json" // Default json file extension
#define CER_EXTENSION            "cer" // Default certificate file extension
#define MAX_PIPE_BUFFER           4096 // Maximum size of a pipe buffer
/* == Base STL includes ==================================================== */
#include <algorithm>                   // Searching, sorting, counting, etc.
#include <array>                       // Static arrays
#include <atomic>                      // Multithreaded integers
#include <cctype>                      // Character type functions
#include <clocale>                     // Regional specific functions
#include <cmath>                       // Perform mathematical functions
#include <condition_variable>          // Synchronisation conditions
#include <csetjmp>                     // Classic C jumps functions
#include <csignal>                     // Operating system signal functions
#include <cstdarg>                     // Classic variadic arguments functions
#include <cstdlib>                     // Standard library functions
#include <cstring>                     // Standard string functions
#include <deque>                       // Chunked vectors
#include <exception>                   // Exception handling
#include <functional>                  // Dynamic functions
#include <iomanip>                     // String stream manipulation
#include <iterator>                    // Container iterators
#include <limits>                      // Hardware bounds limits
#include <list>                        // Linked lists
#include <map>                         // Key value containers
#include <memory>                      // Memory allocation
#include <mutex>                       // Thread synchronisation
#include <numeric>                     // Accumulators
#include <queue>                       // First-in and first-out containers
#include <set>                         // Automatically sorted lists
#include <sstream>                     // String streams
#include <stdexcept>                   // Runtime errors
#include <string>                      // String containers
#include <thread>                      // Operating system threads
#include <unordered_set>               // Unordered set
#include <vector>                      // Dynamic arrays
/* -- More checks ---------------------------------------------------------- */
#if CHAR_BIT != 8                      // Sanity check bits-per-byte
# error Target architecture byte size must be eight bits!
#endif                                 // Sanity checks
/* -- Useful macros -------------------------------------------------------- */
#define STR_HELPER(...)  #__VA_ARGS__            // Convert macro integer
#define STR(...)         STR_HELPER(__VA_ARGS__) // to string
/* -- Target specific headers ---------------------------------------------- */
#if defined(WINDOWS)                   // Windows 32-bit or 64-bit
# if defined(_CRTDBG_MAP_ALLOC)        // Debug stuff required by MSDN
#  include <stdlib.h>                  // >> Required by MSDN
#  include <crtdbg.h>                  // >> Basic Leak detection SDK
// # define USEVLD                     // Set to use VLD (copy *.dll/manifest)
#  if defined(USEVLD)                  // Use VLD debugging library?
#   include <vld/vld.h>                // >> If basic detection not working
#   if defined(X64)                    // 64-bit windows?
#    pragma comment(lib, "res/lib/vld64d") // >> Use 64-bit VLD lib
#   elif defined(X86)                  // 32-bit windows?
#    pragma comment(lib, "res/lib/vld32d") // >> Use 32-bit VLD lib
#   endif                              // Bits check
#  endif                               // VLD inclusion requested
# endif                                // Not using debug mode
/* -- STL Headers ---------------------------------------------------------- */
# include <execution>                  // For std::execution::* classes
/* -- VC Headers ----------------------------------------------------------- */
# include <direct.h>                   // Directory header
# include <intrin.h>                   // Required by jpeg stuff
# include <stdio.h>                    // Standard IO stuff
# include <io.h>                       // Io header
# include <process.h>                  // Process header
# include <sys/timeb.h>                // Time block functions
/* -- Endianness ----------------------------------------------------------- */
# define SWAP_U16(v) _byteswap_ushort(static_cast<unsigned short>(v))
# define SWAP_U32(v) _byteswap_ulong(static_cast<unsigned long>(v))
# define SWAP_U64(v) _byteswap_uint64(static_cast<unsigned __int64>(v))
# if REG_DWORD == REG_DWORD_LITTLE_ENDIAN // Using Intel/AMD? (LE)
#  define STRICT_U16LE(v) (v)          // Use 16-bit integers as-is
#  define STRICT_U16BE(v) SWAP_U16(v)  // Swap 16-bits
#  define STRICT_U32LE(v) (v)          // Use 32-bit integers as-is
#  define STRICT_U32BE(v) SWAP_U32(v)  // Swap 32-bits
#  define STRICT_U64LE(v) (v)          // Use 64-bit integers as-is
#  define STRICT_U64BE(v) SWAP_U64(v)  // Swap 64-bits
#  define LITTLEENDIAN                 // Using little endian byte order
# elif REG_DWORD == REG_DWORD_BIG_ENDIAN // Using ARM? (BE)
#  define STRICT_U16LE(v) SWAP_U16(v)  // Swap 16-bits
#  define STRICT_U16BE(v) (v)          // Use 16-bit integers as-is
#  define STRICT_U32LE(v) SWAP_U32(v)  // Swap 32-bits
#  define STRICT_U32BE(v) (v)          // Use 32-bit integers as-is
#  define STRICT_U64LE(v) SWAP_U64(v)  // Swap 64-bits
#  define STRICT_U64BE(v) (v)          // Use 64-bit integers as-is
#  define BIGENDIAN                    // Using big endian byte order
# else                                 // Unknown endianness?
#  error Unknown endianness!
# endif                                // Endianess setup
/* -- Using anything but Windows? ------------------------------------------ */
#else                                  // Could be MacOS or Linux
/* ------------------------------------------------------------------------- */
# include <cxxabi.h>                   // Debug name demangling stuff
# include <dirent.h>                   // Directory functions (*dir)
# include <dlfcn.h>                    // Dylib functions
# include <execinfo.h>                 // Stack trace functions
# include <libgen.h>                   // Posix core (*name)
# include <spawn.h>                    // Spawn functions header
# include <sys/stat.h>                 // Stat struct
# include <sys/utsname.h>              // OS information
# include <unistd.h>                   // Unix standard stuff
/* -- Endianness ----------------------------------------------------------- */
# define SWAP_U16(v) __builtin_bswap16(static_cast<unsigned short>(v))
# define SWAP_U32(v) __builtin_bswap32(static_cast<unsigned int>(v))
# define SWAP_U64(v) __builtin_bswap64(static_cast<unsigned long long>(v))
# if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__ // Using Intel/AMD? (LE)
#  define STRICT_U16LE(v) (v)          // Use 16-bit integers as-is
#  define STRICT_U16BE(v) SWAP_U16(v)  // Swap 16-bits
#  define STRICT_U32LE(v) (v)          // Use 32-bit integers as-is
#  define STRICT_U32BE(v) SWAP_U32(v)  // Swap 32-bits
#  define STRICT_U64LE(v) (v)          // Use 64-bit integers as-is
#  define STRICT_U64BE(v) SWAP_U64(v)  // Swap 64-bits
#  define LITTLEENDIAN                 // Define it if not
# elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__ // Using ARM? (BE)
#  define STRICT_U16LE(v) SWAP_U16(v)  // Swap 16-bits
#  define STRICT_U16BE(v) (v)          // Use 16-bit integers as-is
#  define STRICT_U32LE(v) SWAP_U32(v)  // Swap 32-bits
#  define STRICT_U32BE(v) (v)          // Use 32-bit integers as-is
#  define STRICT_U64LE(v) SWAP_U64(v)  // Swap 64-bits
#  define STRICT_U64BE(v) (v)          // Use 64-bit integers as-is
#  define BIGENDIAN                    // Define it if not
# else                                 // Unknown endianness?
#  error Unknown endianness!
# endif                                // Endianness setup
/* ------------------------------------------------------------------------- */
# if defined(LINUX)                    // Uisng Linux?
#  include <execution>                 // For std::execution::* classes
# endif                                // Apple check
/* ------------------------------------------------------------------------- */
#endif                                 // Operating system
/* == Compiler warning configuration ======================================= */
#if !defined(WINDOWS)                  // Not using windows?
# pragma GCC diagnostic push           // Save warnings
# if defined(LINUX)                    // Using Linux?
#  pragma GCC diagnostic ignored "-Wall"        // Disable ALL warnings
# elif defined(MACOS)                  // Using MacOS?
#  pragma GCC diagnostic ignored "-Weverything" // Disable ALL warnings
# endif                                // Linux or MacOS check
#endif                                 // Not using windows
/* -- Lua includes --------------------------------------------------------- **
** Because Lua was compiled as C++ in the root namespace, Lua's includes     **
** also need to be defined in the root namespace.                            **
** ------------------------------------------------------------------------- */
#include <lua/lstate.h>                // Definition of Lua_State
#include <lua/lauxlib.h>               // Lua auxiliary library
#include <lua/lualib.h>                // Lua library
#include <lua/lfunc.h>                 // Lua functions
#undef getstr                          // Ambiguity with ncurses
/* == Libaries namespace to keep all the API's neat and tidy =============== */
namespace Lib                          // LIBRARY OF EXTERNAL API FUNCTIONS
{ /* ----------------------------------------------------------------------- */
  namespace OS                         // OPERATING SYSTEM API FUNCTIONS
  { /* --------------------------------------------------------------------- */
#if defined(WINDOWS)                   // Targeting Windows?
# include <Windows.H>                  // Need windows headers file
# include <TChar.H>                    // Unicode support
# include <ImageHlp.H>                 // Executable image headers file
# include <TlHelp32.H>                 // Toolhelp header file
# include <ShlObj.H>                   // Shell header file
# include <RTCapi.H>                   // RTC headers file
# include <WinSock.H>                  // Windows sockets headers file
# include <AccCtrl.H>                  // Access control headers file
# include <AclApi.H>                   // Access control list headers file
# define PSAPI_VERSION               1 // Want this to work on XP so use V1
# include <PsApi.H>                    // Process API header file
# undef PSAPI_VERSION                  // Don't need this anymore
# undef GetObject                      // RapidJSon compatibility
# define GLFW_EXPOSE_NATIVE_WIN32      // Expose Win32 specific funcs in GlfW
#else                                  // Anything but Windows?
# if defined(LINUX)                    // Targeting Linux?
#  include <X11/Xlib.h>                // Load X11 API
#  include <X11/extensions/Xrandr.h>   // Load X11 Xrandr API
#  include <wayland-client.h>          // Load WayLand API
#  include <elf.h>                     // Elf header file
#  include <link.h>                    // dlinfo() function
#  include <sys/ioctl.h>               // Gettingterminal size
#  include <sys/resource.h>            // Getting process memory info
#  include <sys/sysinfo.h>             // Getting system memory info
#  include <sys/time.h>                // Getting time info
#  include <sys/times.h>               // Getting cpu usage info
#  include <sys/wait.h>                // For waiting for pid
#  include <sys/fcntl.h>               // File control macros
#  include <sys/types.h>               // Socket types
#  undef Bool                          // Causes problem with FreeType
#  define GLFW_EXPOSE_NATIVE_X11       // Expose X11 specific funcs in GLFW
#  define GLFW_EXPOSE_NATIVE_WAYLAND   // Expose Wayland specific funcs in GLFW
# elif defined(MACOS)                  // Targeting MacOS?
#  include <ApplicationServices/ApplicationServices.h> // Load app services API
#  include <objc/objc.h>               // Load ObjectiveC in C++ API
#  include <sys/sysctl.h>              // Kernel info stuff
#  include <mach/mach_init.h>          // For getting cpu and memory usage
#  include <mach/mach_error.h>         // For getting cpu usage
#  include <mach/mach_host.h>          // For getting cpu and memory usage
#  include <mach/vm_map.h>             // For getting cpu usage
#  include <mach/vm_statistics.h>      // For getting memory usage
#  include <mach/mach_types.h>         // For getting memory usage
#  include <mach/mach.h>               // For getting process memory usage
#  include <mach/kern_return.h>        // For getting process memory usage
#  include <mach/task_info.h>          // For getting process memory usage
#  include <mach/mach_time.h>          // For getting system uptime
#  include <mach-o/dyld.h>             // For enumerating shared objects
#  include <mach-o/loader.h>           // For getting mach-o header format
#  include <mach-o/fat.h>              // For getting mach-o fat header format
#  include <fcntl.h>                   // File control macros
#  include <termios.h>                 // For changing terminal settings
#  include <libproc.h>                 // For getting program executable
#  define _XOPEN_SOURCE_EXTENDED       // Unlock extended ncurses functionality
     typedef void (*__sighandler_t)(int); // For signal() on MacOS
#  define GLFW_EXPOSE_NATIVE_COCOA     // Expose Cocoa specific funcs in GLFW
# endif                                // POSIX system check
# include <semaphore.h>                // Semaphores
# include <sys/socket.h>               // Socket header
# include <sys/mman.h>                 // Memory manager functions
#endif                                 // Operating system check
    /* --------------------------------------------------------------------- */
    namespace OpenSSL                  // OPENSSL API FUNCTIONS
    { /* ------------------------------------------------------------------- */
#define OPENSSL_SUPPRESS_DEPRECATED    // Want deprecated functions
#include <openssl/conf.h>              // Conf header
#include <openssl/evp.h>               // EVP header
#include <openssl/engine.h>            // Engine header
#include <openssl/comp.h>              // Comp header
#include <openssl/rand.h>              // RNG header
#include <openssl/ssl.h>               // main header
#include <openssl/md5.h>               // MD5 hash (compatibility only)
#include <openssl/sha.h>               // SHA-1 hash (compatibility only)
#include <openssl/bio.h>               // Bio header
#include <openssl/err.h>               // Errors header
#include <openssl/ocsp.h>              // OCSP header
#include <openssl/x509v3.h>            // Certs header
#include <openssl/httperr.h>           // HTTP client errors (ToDo)
#include <openssl/http.h>              // HTTP client (ToDo)
    } /* ------------------------------------------------------------------- */
    namespace SevenZip                 // 7-ZIP API FUNCTIONS
    { /* ------------------------------------------------------------------- */
#include <7z/CpuArch.h>                // CPU configuration
#include <7z/7z.h>                     // Library core (old p7zip)
#include <7z/7zCrc.h>                  // CRC header
#include <7z/7zFile.h>                 // 7-zip file format
#include <7z/7zVersion.h>              // 7-zip version information
#include <7z/LzmaLib.h>                // For compression stuff
    } /* ------------------------------------------------------------------- */
    namespace JpegTurbo                // LIBJPEGTURBO API FUNCTIONS
    { /* ------------------------------------------------------------------- */
#if defined(WINDOWS)                   // Using windows?
      typedef int boolean;             // Defined by system but not in our NS
#endif                                 // Windows check
#include <jpeg/jpeglib.h>              // Our main header
#include <jpeg/jerror.h>               // Our error handling
#include <jpeg/jversion.h>             // Our version information
    } /* ------------------------------------------------------------------- */
    namespace ZLib                     // ZLIB API FUNCTIONS
    { /* ------------------------------------------------------------------- */
#include <zlib/zlib.h>                 // Main header
    } /* ------------------------------------------------------------------- */
    namespace GlFW                     // GLFW API FUNCTIONS
    { /* ------------------------------------------------------------------- */
#define GLFW_INCLUDE_GLCOREARB         // Include arbitrary OpenGL API
#include <glfw/glfw3.h>                // Main header
#include <glfw/glfw3native.h>          // Operating system includes
      /* ------------------------------------------------------------------- */
      namespace Types                  // Common types
      { /* ----------------------------------------------------------------- */
        using GlFW::GLFWimage;         // GLFW specific image type
        using GlFW::GLFWmonitor;       // GLFW specific monitor type
        using GlFW::GLFWwindow;        // GLFW specific window type
        using GlFW::GLchar;            // GL specific char type
        using GlFW::GLenum;            // GL specific enum type
        using GlFW::GLfloat;           // GL specific float type
        using GlFW::GLint;             // GL specific int type
        using GlFW::GLsizei;           // GL specific sizei type
        using GlFW::GLubyte;           // GL specific unsigned char type
        using GlFW::GLuint;            // GL specific unsigned int type
        using GlFW::GLvoid;            // GL specific void type
      } /* ----------------------------------------------------------------- */
#if defined(MACOS)                     // MacOS defined?
      /* ------------------------------------------------------------------- */
      // This namespace is required to workaround a major crash bug in MacOS
      // See https://github.com/glfw/glfw/issues/1997 for more information
      /* ------------------------------------------------------------------- */
      namespace NSGL                   // Begin MacOS NSGL namespace
      { /* ----------------------------------------------------------------- */
#  include <OpenGL/OpenGL.h>           // Include MacOS NSOpenGL functions
      } /* ----------------------------------------------------------------- */
#endif                                 // End MacOS NSGL namespace
      /* ------------------------------------------------------------------- */
#undef GLFW_INCLUDE_GLCOREARB          // Done with this macro
    } /* ------------------------------------------------------------------- */
#undef GLFW_EXPOSE_NATIVE_WIN32        // Done with this macro
#undef GLFW_EXPOSE_NATIVE_X11          // Done with this macro
#undef GLFW_EXPOSE_NATIVE_WAYLAND      // Done with this macro
#undef GLFW_EXPOSE_NATIVE_COCOA        // Done with this macro
    /* --------------------------------------------------------------------- */
    namespace MiniMP3                  // MINIMP3 API FUNCTIONS
    { /* ------------------------------------------------------------------- */
#define MINIMP3_IMPLEMENTATION         // Using MINIMP3 implementation
#define MINIMP3_NO_STDIO               // No stdio routines
#if defined(WINDOWS)                   // Using Windows
#endif                                 // Using Windows
#include <mp3/minimp3.h>               // Main decoder
#include <mp3/minimp3_ex.h>            // Decoder utilities
#if defined(WINDOWS)                   // Using Windows
#endif                                 // Using Windows
    }
  } /* --------------------------------------------------------------------- */
  namespace NSGif                      // LIBNSGIF API FUNCTIONS
  { /* --------------------------------------------------------------------- */
#include <gif/gif.h>                   // Main header
  } /* --------------------------------------------------------------------- */
  namespace Png                        // LIBPNG API FUNCTIONS
  { /* --------------------------------------------------------------------- */
#include <png/png.h>                   // Main header
  } /* --------------------------------------------------------------------- */
  namespace RapidJson                  // RAPIDJSON API FUNCTIONS
  { /* --------------------------------------------------------------------- */
#define RAPIDJSON_NAMESPACE            Lib::RapidJson
#define RAPIDJSON_HAS_STDSTRING      1 // Have Std::String
#define RAPIDJSON_NOEXCEPT             // We have noexcept
#define RAPIDJSON_NAMESPACE_BEGIN      // Keep blank or errors
#define RAPIDJSON_NAMESPACE_END        // Keep blank or errors
#define RAPIDJSON_ASSERT(x)          if(!(x)) throw ::std::runtime_error{(#x)};
#define RAPIDJSON_HAS_CXX11_RVALUE_REFS 1 // So sorting works
#define RAPIDJSON_HAS_CXX11_NOEXCEPT 1 // Force to use noexcept
#include <rapidjson/document.h>        // Main header
#include <rapidjson/prettywriter.h>    // Pretty formatting
#include <rapidjson/error/en.h>        // Error handling
#include <rapidjson/cursorstreamwrapper.h> // To get better error information
#undef RAPIDJSON_HAS_CXX11_NOEXCEPT    // Done with this define
#undef RAPIDJSON_ASSERT                // Done with this define
#undef RAPIDJSON_NAMESPACE_END         // Done with this define
#undef RAPIDJSON_NAMESPACE_BEGIN       // Done with this define
#undef RAPIDJSON_HAS_STDSTRING         // Done with this define
#undef RAPIDJSON_NAMESPACE             // Done with this define
  } /* --------------------------------------------------------------------- */
  namespace OpenAL                     // OPENAL API FUNCTIONS
  { /* --------------------------------------------------------------------- */
#define AL_ALEXT_PROTOTYPES            // So can get alcResetDevice
#include <al/alext.h>                  // Main header
#undef AL_ALEXT_PROTOTYPES             // Extra functions check
    /* --------------------------------------------------------------------- */
    namespace Types                    // Common types
    { /* ------------------------------------------------------------------- */
      using OpenAL::ALCEVENTPROCTYPESOFT; // AL specific system event callback
      using OpenAL::ALCchar;           // AL specific context char type
      using OpenAL::ALCdevice;         // AL specific device context
      using OpenAL::ALCenum;           // AL specific context enum type
      using OpenAL::ALCsizei;          // AL specific context sizei type
      using OpenAL::ALdouble;          // AL specific double type
      using OpenAL::ALenum;            // AL specific enum type
      using OpenAL::ALfloat;           // AL specific float type
      using OpenAL::ALint;             // AL specific int type
      using OpenAL::ALshort;           // AL specific short type
      using OpenAL::ALsizei;           // AL specific sizei type
      using OpenAL::ALuint;            // AL specific unsigned int type
      using OpenAL::ALvoid;            // AL specific void type
    } /* ------------------------------------------------------------------- */
  } /* -- Ogg Includes ----------------------------------------------------- */
  namespace Ogg                        // OGGVORBIS API FUNCTIONS
  { /* --------------------------------------------------------------------- */
#define OV_EXCLUDE_STATIC_CALLBACKS    // We don't need these callbacks
#include <ogg/vorbisfile.h>            // Vorbis/ogg API headers
#undef OV_EXCLUDE_STATIC_CALLBACKS     // The define is no longer needed
    /* --------------------------------------------------------------------- */
    namespace Types                    // Common types
    { /* ------------------------------------------------------------------- */
      using Ogg::ogg_int64_t;          // Ogg specific int64 type
    } /* ------------------------------------------------------------------- */
    /* -- Theora decoder depends on ogg headers ---------------------------- */
    namespace Theora                   // THEORA API FUNCTIONS
    { /* ------------------------------------------------------------------- */
#include <theora/theora.h>             // StrFormat codec (+ ogg codec)
#include <theora/theoradec.h>          // Decoder
    } /* ------------------------------------------------------------------- */
#if !defined(WINDOWS)                  // Not using windows?
# pragma GCC diagnostic pop            // - Restore compiler warnings
#endif                                 // Not using windows
  } /* --------------------------------------------------------------------- */
  namespace Sqlite                     // SQLITE API FUNCTIONS
  {/* ---------------------------------------------------------------------- */
#include <sql/sqlite3.h>               // Main header
    /* --------------------------------------------------------------------- */
    namespace Types                    // Common types
    { /* ------------------------------------------------------------------- */
      using Sqlite::sqlite3_int64;     // Sqlite specific int64 type
    } /* ------------------------------------------------------------------- */
  } /* --------------------------------------------------------------------- */
  namespace FreeType                   // FREETYPE API FUNCTIONS
  { /* --------------------------------------------------------------------- */
#include <ft/ft2build.h>               // Build parameters header
#include <ft/freetype.h>               // Base header
#include <ft/ftstroke.h>               // Stroker header
#include <ft/ftglyph.h>                // Glyph header
#include <ft/ftmodapi.h>               // Modification header
  } /* --------------------------------------------------------------------- */
} /* ----------------------------------------------------------------------- */
/* == Main() configuration. So engine.cpp's main() declaration is tidy ===== */
#if defined(WINDOWS)                   // Targeting Windows?
typedef Lib::OS::TCHAR ArgType;        // Set main argument type
# define ENTRYFUNC WINAPI _tWinMain(Lib::OS::HINSTANCE, Lib::OS::HINSTANCE, \
    Lib::OS::LPTSTR, int)
# define CONENTRYFUNC _tmain           // For project management utility
#else                                  // Targeting POSIX?
typedef char ArgType;                  // Set main argument type
# define ENTRYFUNC main(int __argc, ArgType**__wargv, ArgType**_wenviron)
# define CONENTRYFUNC main             // For project management utility
#endif                                 // Target check
/* == EoF =========================================================== EoF == */
