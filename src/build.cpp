/* == BUILD.CPP ============================================================ **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## This is the Mhatxotic Design Project Management utility which helps ## **
** ## build and maintain the components of the Mhatxotic engine.          ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
#define BUILD                          // Indicates command-line tool compile
/* ------------------------------------------------------------------------- */
#include "setup.hpp"                   // Environment configuration
/* -- Disable unreferenced function warning -------------------------------- */
#if defined(WINDOWS)                   // Compiling on Windows?
# pragma warning(disable: 4505)
#else                                  // Compiling on MacOS or Linux?
# pragma GCC diagnostic ignored "-Wunused-function"
#endif                                 // Operating system check
/* ------------------------------------------------------------------------- */
#include <iostream>                    // Using cout to print to con
/* ------------------------------------------------------------------------- */
using std::cout;                       // Using this to print text
using std::endl;                       // " End of line for std::cout
/* ------------------------------------------------------------------------- */
namespace E {                          // Put everything in engine namespace
/* ------------------------------------------------------------------------- */
#include "engine.hpp"                  // Engine version information header
#include "stdtypes.hpp"                // Engine STL type aliases header
#include "common.hpp"                  // Common constant variables header
#include "flags.hpp"                   // Flags helper utility header
#include "utf.hpp"                     // UTF strings utility header
#include "std.hpp"                     // StdLib function helpers header
#include "string.hpp"                  // String utilities header
#include "url.hpp"                     // Url parsing library
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
#include "ihelper.hpp"                 // Init helper utility header
#include "args.hpp"                    // Arguments handling header
#include "cmdline.hpp"                 // Command-line class header
#include "memory.hpp"                  // Memory management utilities header
#include "fstream.hpp"                 // File IO utility header
#include "log.hpp"                     // Logging helper class header
#include "luadef.hpp"                  // Lua definitions header
#include "collect.hpp"                 // Class collector utility header
#include "lockable.hpp"                // Lockable collector utility header
#include "luaident.hpp"                // Lua ident helper class header
#include "stat.hpp"                    // Statistic utility class header
#include "thread.hpp"                  // Thread helper class header
#include "evtcore.hpp"                 // Thread-safe event system core header
#include "evtmain.hpp"                 // Main engine events system header
#include "condef.hpp"                  // Console definitions header
#include "intpair.hpp"                 // IntPair class header
#include "coord.hpp"                   // Coord class header
#include "dim.hpp"                     // Dimension class header
#include "dimcoord.hpp"                // DimensionCoord class header
#include "syscore.hpp"                 // Operating system interface header
#include "filemap.hpp"                 // Virtual file IO interface
#include "luautil.hpp"                 // Lua utility functions header
#include "luaref.hpp"                  // Lua reference helper class header
#include "luaevent.hpp"                // Lua event helper class header
#include "luafunc.hpp"                 // Lua callback helper class header
#include "async.hpp"                   // Async file loading class header
#include "crypt.hpp"                   // Cryptography utilities header
#include "uuid.hpp"                    // UuId parsing header
#include "codec.hpp"                   // Codec classes header
#include "archive.hpp"                 // Archive handling class header
#include "asset.hpp"                   // Asset handling class header
#include "json.hpp"                    // Json handling class header
#include "file.hpp"                    // FStream+FileMap class header
/* -- Includes ------------------------------------------------------------- */
using namespace IClock::P;             using namespace ICmdLine::P;
using namespace ICommon::P;            using namespace ICrypt::P;
using namespace ICodec::P;             using namespace IDir::P;
using namespace IError::P;             using namespace IFStream::P;
using namespace IHelper::P;            using namespace IJson::P;
using namespace ILockable::P;          using namespace ILog::P;
using namespace ILuaIdent::P;          using namespace ILuaLib::P;
using namespace IMemory::P;            using namespace IParser::P;
using namespace IPSplit::P;            using namespace IStd::P;
using namespace IString::P;            using namespace ISystem::P;
using namespace ISysUtil::P;           using namespace IToken::P;
using namespace IUtf::P;               using namespace IUtil::P;
using namespace IUuId::P;
/* ========================================================================= */
#define STANDARD   "c++20"             // Current compilation standard used
#define ENGINENAME "engine"            // Name of engine 'engine'
#define SRCEXT     ".hpp"              // Extension of source files
#define ARCDIR     "archive"           // Archives directory
#define BINDIR     "bin"               // Binaries directory
#define CRTDIR     "certs"             // CA certificates directory
#define DBGDIR     "debug"             // Debugging symbols directory
#define DOCDIR     "docs"              // Documentation directory
#define DRDDIR     DBGDIR              // "-rd" Debugging symbols ramdisk dir
#define DISDIR     "distro"            // Distributable directory
#define CFGEXT     "cfg"               // Configuration file extension
#define INCDIR     "include"           // Include files directory
#define LIBDIR     "lib"               // Library files directory
#define LICDIR     "licenses"          // License files directory
#define SRCDIR     "src"               // Source files directory
#define UTLDIR     "util"              // Utilities directory
#define WINDIR     "win32"             // Windows resources directory
#define VERHEADER  ENGINENAME SRCEXT   // Include file for build info
#define LICHEADER  "license" SRCEXT    // Include file for licenses info
/* ------------------------------------------------------------------------- */
static struct Environment              // Preconfigured environment settings
{ /* ------------------------------------------------------------ */ const char
  *const cpPerl,     *const cpCMake,    *const cpCppCheck, *const cpCppChkM,
  *const cpCppChk32, *const cpCppChk64, *const cpDBG,      *const cp7z,
  *const cpAC4,      *const cpAC8,      *const cpACM,      *const cpACA,
  *const cpACB,      *const cpCCX,      *const cpCCM,      *const cpCCMX,
  *const cpCCLIB,    *const cpCCIncDBG, *const cpCCASM,    *const cpCCAnal,
  *const cpCCAA,     *const cpCCAB,     *const cpCCAR,     *const cpCCPP,
  *const cpCC4,      *const cpCC8,      *const cpCCOBJ,    *const cpCCRES,
  *const cpRCX,      *const cpRCM,      *const cpRCAA,     *const cpRCAB,
  *const cpRCAR,     *const cpRC4,      *const cpRC8,      *const cpLDX4,
  *const cpLDX8,     *const cpLDM,      *const cpLDAA,     *const cpLDAB,
  *const cpLDAR,     *const cpLDE4,     *const cpLDE8,     *const cpLDB4,
  *const cpLDB8,     *const cpLD4,      *const cpLD8,      *const cpLDL,
  *const cpLDMAP,    *const cpLIB,      *const cpOBJ,      *const cpASM,
  *const cpPDB,      *const cpLDEXE,    *const cpMAP,      *const cpEXE,
  *const cpDBGSUF;
} /* ----------------------------------------------------------------------- */
envWindowsMSVC =                       // Microsoft Visual C++ environment
{ /* ----------------------------------------------------------------------- */
  /* PERL       */ "D:\\Apps\\Perl\\bin\\PERL.EXE",
  /* CMAKE      */ "D:\\Apps\\CMake\\bin\\CMAKE.EXE",
  /* CPPCHECK   */ "D:\\Apps\\CppCheck\\CPPCHECK.EXE",
  /* CPPCHKP    */ "-D_WIN32 -D_MSC_VER=1922",
  /* CPPCHKP32  */ "-DX86 -D_M_IX86 -D_M_X86 -D_X86_ -D__i386__ "
                   "--platform=win32W",
  /* CPPCHKP64  */ "-DX64 -D__x86_64__ -D_WIN64 --platform=win64",
  /* DEBUGGER   */ "D:\\Apps\\Dbg\\X96DBG.EXE \"$/$$$\"",
  /* 7Z         */ UTLDIR "/7Z.EXE",
  /* AC4        */ "ML.EXE -coff",
  /* AC8        */ "ML64.EXE",
  /* ACM        */ "-nologo -safeseh -c ",
  /* ACA        */ "-Zi",
  /* ACB        */ "",
  /* CCX        */ "CL.EXE",
  /* CCM        */ "-nologo -c -Zc:__cplusplus -MP4 -GA -Gy -GF -EHsc -bigobj",
  /* CCMX       */ "-std:$ -utf-8 -W4 -I$ -I$/ft",
  /* CCLIB      */ "-DUNICODE -D_UNICODE",
  /* CCINCDBG   */ "-showIncludes",
  /* CCASM      */ "-Fa -WX",
  /* CCANAL     */ "-analyze -WX",
  /* CCAA       */ "-DALPHA -MTd -Z7 -Od -GS -Gs0 -RTCsu",
  /* CCAB       */ "-DBETA -MT -Z7 -O2 -GS- -GR- -Gw -Qpar",
  /* CCAR       */ "-DRELEASE -MT -O2 -GS- -GR- -GL -Gw -Qpar",
  /* CCPP       */ "-P",
  /* CC4        */ "",
  /* CC8        */ "",
  /* CCOBJ      */ "-Fo",
  /* CCRES      */ "-fo",
  /* RCX        */ "RC.EXE",
  /* RCM        */ "-nologo -I$ -n -l809",
  /* RCAA       */ "-DALPHA",
  /* RCAB       */ "-DBETA",
  /* RCAR       */ "-DRELEASE",
  /* RC4        */ "-DX86",
  /* RC8        */ "-DX64",
  /* LDX4       */ "LINK.EXE",
  /* LDX8       */ "LINK.EXE",
  /* LDM        */ "-nologo -wx -version:17485.6721",
  /* LDAA       */ "-debug -fixed -dynamicbase:no -incremental:no",
  /* LDAB       */ "-debug -fixed -opt:ref,icf",
  /* LDAR       */ "-release -opt:ref,icf -ltcg:status",
  /* LDE4       */ "-subsystem:windows,5.01",
  /* LDE8       */ "-subsystem:windows,5.02",
  /* LDB4       */ "-subsystem:console,5.01",
  /* LDB8       */ "-subsystem:console,5.02",
  /* LD4        */ "-machine:x86 -largeaddressaware",
  /* LD8        */ "-machine:x64",
  /* LDL        */ "advapi32.lib comctl32.lib crypt32.lib dbghelp.lib "
                   "gdi32.lib imagehlp.lib kernel32.lib ole32.lib psapi.lib "
                   "shell32.lib user32.lib version.lib winmm.lib ws2_32.lib ",
  /* LDMAP      */ "-map:$",
  /* LIB        */ ".lib",
  /* OBJ        */ ".obj",
  /* ASM        */ ".asm",
  /* PDB        */ ".pdb",
  /* LDEXE      */ "-out:$ -pdb:$",
  /* MAP        */ ".map",
  /* EXE        */ ".exe",
  /* DBGSUF     */ "",
},/* ----------------------------------------------------------------------- */
envWindowsLLVMcompat =                 // LLVM (MSVC compat) on Windows
{ /* ----------------------------------------------------------------------- */
  /* PERL       */ envWindowsMSVC.cpPerl,
  /* CMAKE      */ envWindowsMSVC.cpCMake,
  /* CPPCHECK   */ envWindowsMSVC.cpCppCheck,
  /* CPPCHKP    */ envWindowsMSVC.cpCppChkM,
  /* CPPCHKP32  */ envWindowsMSVC.cpCppChk32,
  /* CPPCHKP64  */ envWindowsMSVC.cpCppChk64,
  /* DEBUGGER   */ envWindowsMSVC.cpDBG,
  /* 7Z         */ envWindowsMSVC.cp7z,
  /* AC4        */ envWindowsMSVC.cpAC4,
  /* AC8        */ envWindowsMSVC.cpAC8,
  /* ACM        */ envWindowsMSVC.cpACM,
  /* ACA        */ envWindowsMSVC.cpACA,
  /* ACB        */ envWindowsMSVC.cpACB,
  /* CCX        */ "CLANG-CL.EXE",
  /* CCM        */ "-nologo -c -GA -Gy -GF -EHsc -bigobj",
  /* CCMX       */ "-std:$ -utf-8 -I$ -I$/ft",
  /* CCLIB      */ envWindowsMSVC.cpCCLIB,
  /* CCINCDBG   */ envWindowsMSVC.cpCCIncDBG,
  /* CCASM      */ envWindowsMSVC.cpCCASM,
  /* CCANAL     */ "-Weverything -Werror=all -Wno-c++98-compat "
                   "-Wno-c++98-compat-pedantic -Wno-c++20-compat "
                   "-Wno-documentation -Wno-documentation-unknown-command "
                   "-Wno-gnu-zero-variadic-macro-arguments "
                   "-Wno-covered-switch-default -Wno-switch-enum "
                   "-Wno-poison-system-directories -Wno-global-constructors "
                   "-Wno-padded -Wno-cast-function-type-strict",
  /* CCAA       */ envWindowsMSVC.cpCCAA,
  /* CCAB       */ "-DBETA -MT -Z7 -O2 -GS- -Gw",
  /* CCAR       */ "-DRELEASE -MT -O2 -GS- -Gw",
  /* CCPP       */ envWindowsMSVC.cpCCPP,
  /* CC4        */ "-m32",
  /* CC8        */ "-m64",
  /* CCOBJ      */ envWindowsMSVC.cpCCOBJ,
  /* CCRES      */ envWindowsMSVC.cpCCRES,
  /* RCX        */ envWindowsMSVC.cpRCX,
  /* RCM        */ envWindowsMSVC.cpRCM,
  /* RCAA       */ envWindowsMSVC.cpRCAA,
  /* RCAB       */ envWindowsMSVC.cpRCAB,
  /* RCAR       */ envWindowsMSVC.cpRCAR,
  /* RC4        */ envWindowsMSVC.cpRC4,
  /* RC8        */ envWindowsMSVC.cpRC8,
  /* LDX4       */ "LLD-LINK.EXE",
  /* LDX8       */ "LLD-LINK.EXE",
  /* LDM        */ "-nologo -version:17485.6721",
  /* LDAA       */ "-debug -fixed -dynamicbase:no -incremental:no",
  /* LDAB       */ "-debug -fixed -opt:ref,icf",
  /* LDAR       */ "-opt:ref,icf", // -release -ltcg:status unused on LLD-LINK
  /* LDE4       */ envWindowsMSVC.cpLDE4,
  /* LDE8       */ envWindowsMSVC.cpLDE8,
  /* LDB4       */ envWindowsMSVC.cpLDB4,
  /* LDB8       */ envWindowsMSVC.cpLDB8,
  /* LD4        */ envWindowsMSVC.cpLD4,
  /* LD8        */ envWindowsMSVC.cpLD8,
  /* LDL        */ envWindowsMSVC.cpLDL,
  /* LDMAP      */ envWindowsMSVC.cpLDMAP,
  /* LIB        */ envWindowsMSVC.cpLIB,
  /* OBJ        */ envWindowsMSVC.cpOBJ,
  /* ASM        */ envWindowsMSVC.cpASM,
  /* PDB        */ envWindowsMSVC.cpPDB,
  /* LDEXE      */ envWindowsMSVC.cpLDEXE,
  /* MAP        */ envWindowsMSVC.cpMAP,
  /* EXE        */ envWindowsMSVC.cpEXE,
  /* DBGSUF     */ "",
},/* ----------------------------------------------------------------------- */
envWindowsLLVM =                       // LLVM on Windows
{ /* ----------------------------------------------------------------------- */
  /* PERL       */ envWindowsMSVC.cpPerl,
  /* CMAKE      */ envWindowsMSVC.cpCMake,
  /* CPPCHECK   */ envWindowsMSVC.cpCppCheck,
  /* CPPCHKP    */ envWindowsMSVC.cpCppChkM,
  /* CPPCHKP32  */ envWindowsMSVC.cpCppChk32,
  /* CPPCHKP64  */ envWindowsMSVC.cpCppChk64,
  /* DEBUGGER   */ envWindowsMSVC.cpDBG,
  /* 7Z         */ envWindowsMSVC.cp7z,
  /* AC4        */ envWindowsMSVC.cpAC4,
  /* AC8        */ envWindowsMSVC.cpAC8,
  /* ACM        */ envWindowsMSVC.cpACM,
  /* ACA        */ envWindowsMSVC.cpACA,
  /* ACB        */ envWindowsMSVC.cpACB,
  /* CCX        */ "CLANG++.EXE",      // -ftime-report
  /* CCM        */ "-c -Wextra -static -Xclang -flto-visibility-public-std",
  /* CCMX       */ "-std=$ -I$ -I$/ft",
  /* CCLIB      */ envWindowsMSVC.cpCCLIB,
  /* CCINCDBG   */ "",
  /* CCASM      */ "-Fa",
  /* CCANAL     */ envWindowsLLVMcompat.cpCCAnal,
  /* CCAA       */ "-DALPHA -g -O0",
  /* CCAB       */ "-DBETA -O2",
  /* CCAR       */ "-DRELEASE -O3",
  /* CCPP       */ "-dD -E -P -ftabstop=2",
  /* CC4        */ "-m32",
  /* CC8        */ "-m64 -ffp-contract=fast",
  /* CCOBJ      */ "-o",
  /* CCRES      */ envWindowsMSVC.cpCCRES,
  /* RCX        */ envWindowsMSVC.cpRCX,
  /* RCM        */ envWindowsMSVC.cpRCM,
  /* RCAA       */ envWindowsMSVC.cpRCAA,
  /* RCAB       */ envWindowsMSVC.cpRCAB,
  /* RCAR       */ envWindowsMSVC.cpRCAR,
  /* RC4        */ envWindowsMSVC.cpRC4,
  /* RC8        */ envWindowsMSVC.cpRC8,
  /* LDX4       */ "LLD-LINK.EXE",
  /* LDX8       */ "LLD-LINK.EXE",
  /* LDM        */ "-nologo -version:17485.6721",
  /* LDAA       */ "-debug -fixed -dynamicbase:no -incremental:no libcmtd.lib "
                   "oldnames.lib",
  /* LDAB       */ "-debug -fixed -opt:ref,icf libcmt.lib "
                   "oldnames.lib",
  /* LDAR       */ "-release -opt:ref,icf -ltcg:status libcmt.lib "
                   "oldnames.lib",
  /* LDE4       */ envWindowsMSVC.cpLDE4,
  /* LDE8       */ envWindowsMSVC.cpLDE8,
  /* LDB4       */ envWindowsMSVC.cpLDB4,
  /* LDB8       */ envWindowsMSVC.cpLDB8,
  /* LD4        */ envWindowsMSVC.cpLD4,
  /* LD8        */ envWindowsMSVC.cpLD8,
  /* LDL        */ envWindowsMSVC.cpLDL,
  /* LDMAP      */ envWindowsMSVC.cpLDMAP,
  /* LIB        */ envWindowsMSVC.cpLIB,
  /* OBJ        */ envWindowsMSVC.cpOBJ,
  /* ASM        */ envWindowsMSVC.cpASM,
  /* PDB        */ envWindowsMSVC.cpPDB,
  /* LDEXE      */ envWindowsMSVC.cpLDEXE,
  /* MAP        */ envWindowsMSVC.cpMAP,
  /* EXE        */ envWindowsMSVC.cpEXE,
  /* DBGSUF     */ envWindowsLLVMcompat.cpDBGSUF,
},/* ----------------------------------------------------------------------- */
envMacOSLLVM =                         // XCode/LLVM on MacOS
{ /* ----------------------------------------------------------------------- */
#define MACOS_BASE "/Applications/Xcode.app/Contents/Developer/Platforms/" \
                     "MacOSX.platform/Developer/SDKs/MacOSX.sdk"
  /* ----------------------------------------------------------------------- */
  /* PERL       */ "/usr/bin/perl",
  /* CMAKE      */ "/usr/bin/cmake",
  /* CPPCHECK   */ "cppcheck",
  /* CPPCHKP    */ "-D__APPLE__ -D__clang__ "
                   "-D__clang_major__=" STR(__clang_major__) " "
                   "-D__clang_minor__=" STR(__clang_minor__) " "
                   "-D__clang_patchlevel__=" STR(__clang_patchlevel__) " "
                   "-D__LITTLE_ENDIAN__ "
                   "-DTARGET_OS_MAC --check-level=exhaustive",
  /* CPPCHKP32  */ "-DX86 -D__x86_64__ -DTARGET_CPU_X86_64 --platform=unix32",
  /* CPPCHKP64  */ "-DX64 -D__arm64__ -DTARGET_CPU_ARM64 --platform=unix64",
  /* DEBUGGER   */ "/usr/bin/lldb -- \"$/$$$\" app_basedir=\"$\" "
                   "sql_db=\"$/$\"",
  /* 7Z         */ "7zz",
  /* AC4        */ "nasm",
  /* AC8        */ "nasm",
  /* ACM        */ "",
  /* ACA        */ "-g",
  /* ACB        */ envWindowsMSVC.cpACB,
  /* CCX        */ "g++",
  /* CCM        */ "-c -stdlib=libc++ -ffast-math",
  /* CCMX       */ "-Wextra -Wall -std=$ -I$ -I" SRCDIR " -I$/curses -I$/ft",
  /* CCLIB      */ "",
  /* CCINCDBG   */ "",
  /* CCASM      */ "-S -D__ASMFILE__=",
  /* CCANAL     */ envWindowsLLVM.cpCCAnal,
  /* CCAA       */ "-DALPHA -g -O0",
  /* CCAB       */ "-DBETA -O2",
  /* CCAR       */ "-DRELEASE -O3",
  /* CCPP       */ envWindowsLLVM.cpCCPP,
  /* CC4        */ "-target x86_64-apple-macos10.15 -mtune=generic",
  /* CC8        */ "-target arm64-apple-macos11 -mtune=apple-m1",
  /* CCOBJ      */ "-o",
  /* CCRES      */ "",
  /* RCX        */ "",
  /* RCM        */ "",
  /* RCAA       */ "",
  /* RCAB       */ "",
  /* RCAR       */ "",
  /* RC4        */ "",
  /* RC8        */ "",
  /* LDX4       */ "ld",
  /* LDX8       */ "ld",
  /* LDM        */ "-demangle "
                   "-syslibroot /Applications/Xcode.app/Contents/"
                      "Developer/Platforms/MacOSX.platform/Developer/"
                      "SDKs/MacOSX.sdk",
  /* LDAA       */ "",
  /* LDAB       */ "",
  /* LDAR       */ "",
  /* LDE4       */ "",
  /* LDE8       */ "",
  /* LDB4       */ "",
  /* LDB8       */ "",
  /* LD4        */ "-arch x86_64 -platform_version macos 10.15 10.15 "
                   "-lcrt1.10.6.o",
  /* LD8        */ "-arch arm64 -platform_version macos 11.0 11.0",
  /* LDL        */ "-lc -lc++ -lSystem -framework AudioUnit "
                   "-framework AudioToolbox -framework Cocoa "
                   "-framework CoreAudio -framework CoreVideo "
                   "-framework IOKit -framework OpenGL",
  /* LDMAP      */ "-Wl",
  /* LIB        */ ".ma",
  /* OBJ        */ ".o",
  /* ASM        */ ".asm",
  /* PDB        */ ".s",
  /* LDEXE      */ "-o $",
  /* MAP        */ ".map",
  /* EXE        */ ".mac",
  /* DBGSUF     */ "",
  /* ----------------------------------------------------------------------- */
#undef MACOS_BASE
},/* ----------------------------------------------------------------------- */
envLinuxGCC =                          // GCC on Linux
{ /* ----------------------------------------------------------------------- */
#define LINUX_ARCH64 "x86_64-linux-gnu"
#define LINUX_ARCH32 "i386-linux-gnu"
#define LINUX_GCCDIR32 "/usr/lib/gcc/" LINUX_ARCH32 "/" STR(__GNUC__) "/"
#define LINUX_GCCDIR64 "/usr/lib/gcc/" LINUX_ARCH64 "/" STR(__GNUC__) "/"
#define LINUX_GCCLEDIR32 "/usr/libexec/gcc/" LINUX_ARCH32 "/" STR(__GNUC__) "/"
#define LINUX_GCCLEDIR64 "/usr/libexec/gcc/" LINUX_ARCH64 "/" STR(__GNUC__) "/"
  /* ----------------------------------------------------------------------- */
  /* PERL       */ "/usr/bin/perl",
  /* CMAKE      */ "/usr/bin/cmake",
  /* CPPCHECK   */ "cppcheck",
  /* CPPCHKP    */ "-D__linux__ "
                   "-D__GNUC__=" STR(__GNUC__) " "
                   "-D__GNUC_MINOR__=" STR(__GNUC_MINOR__) " "
                   "-D__GNUC_PATCHLEVEL__=" STR(__GNUC_PATCHLEVEL__),
  /* CPPCHKP32  */ envMacOSLLVM.cpCppChk32,
  /* CPPCHKP64  */ envMacOSLLVM.cpCppChk64,
  /* DEBUGGER   */ "/usr/bin/gdb --args \"$/$$$\" app_basedir=\"$\" "
                   "sql_db=\"$/$\"",
  /* 7Z         */ "7za",
  /* AC4        */ envMacOSLLVM.cpAC4,
  /* AC8        */ envMacOSLLVM.cpAC8,
  /* ACM        */ envMacOSLLVM.cpACM,
  /* ACA        */ envMacOSLLVM.cpACA,
  /* ACB        */ envMacOSLLVM.cpACB,
  /* CCX        */ "g++",
  /* CCM        */ "-c -shared-libgcc -mtune=generic -fmax-errors=1 "
                   "-funwind-tables",
  /* CCMX       */ "-Wextra -std=$ -I$ -I$/ft",
  /* CCLIB      */ "",
  /* CCINCDBG   */ "",
  /* CCASM      */ "-Fa",
  /* CCANAL     */ envWindowsLLVM.cpCCAnal,
  /* CCAA       */ "-DALPHA -g -rdynamic -O0",
  /* CCAB       */ "-DBETA -O2",
  /* CCAR       */ "-DRELEASE -O3",
  /* CCPP       */ envWindowsLLVM.cpCCPP,
  /* CC4        */ "-march=i386",
  /* CC8        */ "-march=x86-64",
  /* CCOBJ      */ "-o",
  /* CCRES      */ "",
  /* RCX        */ "",
  /* RCM        */ "",
  /* RCAA       */ "",
  /* RCAB       */ "",
  /* RCAR       */ "",
  /* RC4        */ "",
  /* RC8        */ "",
  /* LDX4       */ LINUX_GCCLEDIR32 "collect2 "
                   "-plugin " LINUX_GCCDIR32 "liblto_plugin.so "
                   "-plugin-opt=" LINUX_GCCDIR32 "lto-wrapper",
  /* LDX8       */ LINUX_GCCLEDIR64 "collect2 "
                   "-plugin " LINUX_GCCDIR64 "liblto_plugin.so "
                   "-plugin-opt=" LINUX_GCCDIR64 "lto-wrapper",
  /* LDM        */ "-plugin-opt=-pass-through=-lgcc_s "
                   "-plugin-opt=-pass-through=-lgcc "
                   "-plugin-opt=-pass-through=-lc "
                   "--build-id --eh-frame-hdr --hash-style=gnu --as-needed "
                   "-pie -z now -z relro",
  /* LDAA       */ "--export-dynamic",
  /* LDAB       */ "--export-dynamic",
  /* LDAR       */ "",
  /* LDE4       */ "",
  /* LDE8       */ "",
  /* LDB4       */ "",
  /* LDB8       */ "",
  /* LD4        */ LINUX_GCCDIR32 "../../../" LINUX_ARCH32 "/Scrt1.o "
                   LINUX_GCCDIR32 "../../../" LINUX_ARCH32 "/crti.o "
                   LINUX_GCCDIR32 "crtbeginS.o "
                   "-L" LINUX_GCCDIR32 " "
                   "-dynamic-linker /lib64/ld-linux-i386.so.2 "
                   "-m elf_i386"
                   LINUX_GCCDIR32 "crtendS.o "
                   LINUX_GCCDIR32 "../../../" LINUX_ARCH32 "/crtn.o",
  /* LD8        */ LINUX_GCCDIR64 "../../../" LINUX_ARCH64 "/Scrt1.o "
                   LINUX_GCCDIR64 "../../../" LINUX_ARCH64 "/crti.o "
                   LINUX_GCCDIR64 "crtbeginS.o "
                   "-L" LINUX_GCCDIR64 " "
                   "-dynamic-linker /lib64/ld-linux-x86-64.so.2 "
                   "-m elf_x86_64 "
                   LINUX_GCCDIR64 "crtendS.o "
                   LINUX_GCCDIR64 "../../../" LINUX_ARCH64 "/crtn.o",
  /* LDL        */ "-lsqlite3 -lz -lvorbis -lvorbisfile -logg -ltheora "
                   "-ltheoradec -lfreetype -lpng -lcrypt -lssl -lglfw "
                   "-lopenal -lrt -ldl -lX11 -lncursesw -lpthread -lstdc++ "
                   "-lm -lgcc_s -lgcc -lc -lgcc_s -lgcc",
  /* LDMAP      */ "-Wl",
  /* LIB        */ ".la",
  /* OBJ        */ ".o",
  /* ASM        */ ".asm",
  /* PDB        */ ".s",
  /* LDEXE      */ "-o $",
  /* MAP        */ ".map",
  /* EXE        */ ".elf",
  /* DBGSUF     */ "",
  /* ----------------------------------------------------------------------- */
#undef LINUX_GCCEXDIR64
#undef LINUX_GCCEXDIR32
#undef LINUX_GCCDIR64
#undef LINUX_GCCDIR32
#undef LINUX_ARCH64
#undef LINUX_ARCH32
};/* -- Configuration flags ------------------------------------------------ */
#define PF_NONE     0x0000000000000000 // No flags
#define PF_ALPHA    0x0000000000000001 // Alpha (Debug) compilation
#define PF_BETA     0x0000000000000002 // Beta compilation
#define PF_FINAL    0x0000000000000004 // Release (Final) compilation
#define PF_X86      0x0000000000000008 // 32-bit compilation
#define PF_X64      0x0000000000000010 // 64-bit compilation
#define PF_NOMERGE  0x0000000000000020 // Don't merge output lines
#define PF_EXEC     0x0000000000000040 // Perform execute after compilation
#define PF_EXTLIB   0x0000000000000080 // Compile external library
#define PF_USAGE    0x0000000000000100 // Show usage
#define PF_ASSETS   0x0000000000000200 // Update assets
#define PF_FASSETS  0x0000000000000400 // Complete rebuild of assets
#define PF_DISTRO   0x0000000000000800 // Build distributable package
#define PF_CHPROJ   0x0000000000001000 // Change project
#define PF_CERT     0x0000000000002000 // Rebuild certificate
#define PF_ANALYSE  0x0000000000004000 // Use -ANALYZE option (32-bit only)
#define PF_CAGEN    0x0000000000008000 // Generate root certificate store
#define PF_DOC      0x0000000000010000 // Generate documentation
#define PF_MORE     0x0000000000020000 // Show more compiler log lines
#define PF_ASM      0x0000000000040000 // Produce assembler output
#define PF_CCINCS   0x0000000000080000 // Show include files
#define PF_CPSELF   0x0000000000100000 // Rebuild self
#define PF_MAP      0x0000000000200000 // Build map of executable
#define PF_NPROJ    0x0000000000400000 // Setup a new project
#define PF_CPPCHK   0x0000000000800000 // Perform CppCheck
#define PF_RVER     0x0000000001000000 // Read version information
#define PF_RPROJ    0x0000000002000000 // Read project information
#define PF_SYSOUT   0x0000000004000000 // Show stdout output to system calls
#define PF_SYSNOERR 0x0000000008000000 // Hide syserr output
#define PF_CHECKSRC 0x0000000010000000 // Check source files
#define PF_ENVWMSVC 0x0000000020000000 // Use MSVC environment
#define PF_ENVWLLVM 0x0000000040000000 // Use LLVM environment
#define PF_ENVWCLCP 0x0000000080000000 // Use LLVM environment
#define PF_ENVXCODE 0x0000000100000000 // Use XCode environment
#define PF_ENVGCC   0x0000000200000000 // Use GCC environment
#define PF_VERSION  0x0000000400000000 // Show version information
#define PF_BLICS    0x0000000800000000 // Build licenses
#define PF_PREPROC  0x0000001000000000 // Preprocess
/* -- All compiling related flags ------------------------------------------ */
#define PF_COMPILE (PF_X86|PF_X64|PF_ANALYSE|PF_BETA|PF_ALPHA|PF_FINAL|\
                    PF_CCINCS|PF_MORE|PF_ASM|PF_ENVWMSVC|PF_ENVWLLVM|\
                    PF_ENVXCODE|PF_ENVGCC|PF_PREPROC)
/* -- All other command flags ---------------------------------------------- */
#define PF_OTHERS (PF_DOC|PF_DISTRO|\
                   PF_CAGEN|PF_ASSETS|PF_FASSETS|PF_NPROJ|PF_CHPROJ|\
                   PF_CPPCHK|PF_EXEC|PF_BLICS)
/* -- All the flags -------------------------------------------------------- */
#define PF_ALL (PF_COMPILE|PF_OTHERS)
/* ------------------------------------------------------------------------- */
static uint64_t uiFlags = PF_RVER|PF_RPROJ|PF_SYSNOERR|PF_X64|PF_BETA|
#if defined(WINDOWS)                   // Using windows?
                   PF_ENVWMSVC;        // Using MSVC environment by default.
static Environment envActive{envWindowsMSVC}; // Using MSVC environment
#elif defined(MACOS)                   // Using osx?
                   PF_ENVXCODE;        // Using XCode environment
static Environment envActive{envMacOSLLVM};   // Using LLVM environment
#elif defined(LINUX)                   // Using linux?
                   PF_ENVGCC;          // Using XCode environment
static Environment envActive{envLinuxGCC};    // Using GCC environment
#endif                                 // Setup environment
static array<unsigned int,4> uiVer{    // Version of engine (y.m.d.b)
  static_cast<unsigned int>(-1), static_cast<unsigned int>(-1),
  static_cast<unsigned int>(-1), static_cast<unsigned int>(-1) };
static string strName, strArch, strVer, strTitle, strAuthor;
static const string strEName{ VER_NAME }, strEAuthor{ VER_AUTHOR };
static const string strVerFile{ "build." JSON_EXTENSION };
/* -- These directory names are reserved ----------------------------------- */
static const StrVUSet svusIgnore{ ARCDIR, BINDIR, CRTDIR, DBGDIR, DOCDIR,
  DRDDIR, DISDIR, INCDIR, LIBDIR, LICDIR, SRCDIR, UTLDIR, WINDIR };
/* -- Get return character format of text string --------------------------- */
static const string StrGetReturnFormat(const string &strIn)
{ // String is not empty?
  if(!strIn.empty())
  { // Enumerate each character...
    for(string::const_iterator ciC{ strIn.cbegin() };
                               ciC != strIn.cend();
                             ++ciC)
    { // Test character
      switch(*ciC)
      { // Carriage-return found
        case '\r':
          return find(ciC, strIn.cend(), '\n') != strIn.cend() ?
             cCommon->CommonCrLf() : cCommon->CommonCr();
        // Line-feed found
        case '\n':
          return find(ciC, strIn.cend(), '\r') != strIn.cend() ?
             cCommon->CommonLfCr() : cCommon->CommonLf();
        // Anything else is ignored
        default: break;
      }
    } // Nothing found
  } // Return blank string
  return {};
}
/* ------------------------------------------------------------------------- */
static int CheckSources(void)
{ // Number of warnings
  size_t stWarnings = 0, stTotal = 0;
  // Read source files directory
  const Dir daFiles[2]={{ SRCDIR, ".hpp" },{ SRCDIR, ".cpp" }};
  for(const Dir &dFiles : daFiles)
    for(const DirEntMapPair &dempPair : dFiles.GetFiles())
  { // Total number of files processed
    ++stTotal;
    // Build actual filename
    const string strFile{ StrAppend(SRCDIR, '/', dempPair.first) };
    // Capture errors
    try
    { // Open file
      FStream fsFile{ strFile, FM_R_B };
      if(!fsFile.FStreamOpened()) throw runtime_error("Open failed!");
      // Read file and restart if empty
      const string strData{ fsFile.FStreamReadStringSafe() };
      if(strData.empty()) throw runtime_error("Empty file!");
      fsFile.FStreamClose();
      // Detect file format and split to lines
      const Token tLines{ strData, StrGetReturnFormat(strData) };
      // Got a comment block
      bool bCBlock = false;
      // Enumerate through all the parsed lines
      for(size_t stLine = 0; stLine < tLines.size(); ++stLine)
      { // Get line
        const string &strLine = tLines[stLine];
        // Capture errors
        try
        { // Must not be an empty line
          if(strLine.empty() && stLine+1 < tLines.size())
            throw runtime_error("Empty line!");
          // Must be at least 4 characters
          if(strLine.length() < 4) continue;
          // Beginning of a comment?
          if(strLine[0] == '/' && strLine[1] == '*')
          { // We already processed one on the last line
            if(bCBlock) throw runtime_error("Potential comment block!");
            // We haven't processed one
            bCBlock = true;
            // Check for unfinished separator lines
            if(strLine.length() > 4 &&
              (strLine[3] == '-' || strLine[3] == '=' ||
               strLine[3] == '#') &&
               strLine[strLine.length() - 1] == '/' &&
               strLine[strLine.length() - 2] == '*' &&
               strLine.length() < 79)
              throw runtime_error("Unfinished separator!");
          } // Not a comment
          else bCBlock = false;
          // Must be maximum of 79 characters
          if(strLine.length() >= 80) throw runtime_error("Line too long!");
          // Skip indentation
          size_t stPos = 0;
          const size_t stEnd = strLine.length();
          do { if(stPos >= stEnd || strLine[stPos] != ' ') break; }
          while(++stPos);
          // Check for macros
          if(stPos > 1 && stPos < stEnd && strLine[stPos] == '#' &&
            isalpha(strLine[stPos+1]))
              throw runtime_error("Remove pre-processor indentation!");
          // Scan rest of string
          bool bInQuote = false, bGotAlpha = false;
          for(; stPos < stEnd; ++stPos) try
          { // Is a quote?
            if(strLine[stPos] == '\"') bInQuote ^= true;
            // Not in a quote?
            else if(!bInQuote)
            { // Is a comment?
              if((stPos-1 >= strLine.length() || strLine[stPos-1] == ' ') &&
                 strLine[stPos] == '/' && strLine[stPos+1] == '/')
              { // Check if in centre
                if(bGotAlpha && stPos < 39 && (strLine.length() - stPos) < 39)
                  throw runtime_error("Comment not in centre!");
                // Next part must have a space
                if(strLine[stPos+2] != ' ')
                  throw runtime_error("Comment must begin with a whitespace!");
                // No need to check anything else
                break;
              }
            } // Is alpha?
            else if(StdIsAlnum(strLine[stPos])) bGotAlpha = true;
          } // exception occured
          catch(const exception &e)
          { // Print row number, column and error
            ++stWarnings;
            cout << "W: " << strFile << ':' << stLine+1 << '/'
                 << tLines.size() << ':' << stPos+1 << '/'
                 << strLine.length() << ": " << e.what() << endl;
          }
        } // exception occured
        catch(const exception &e)
        { // Print row number and error
          ++stWarnings;
          cout << "W: " << strFile << ':' << stLine+1 << '/'
               << tLines.size() << ':' << strLine.length() << '/'
               << strLine.length() << ": " << e.what() << endl;
        }
      }
    } // exception occured
    catch(const exception &e)
    { // Print file error
      ++stWarnings;
      cout << "W: " << strFile << ": " << e.what() << endl;
    }
  } // Add return if a warning occured
  if(stWarnings) cout << '\n';
  // Finished
  cout << "Finished with " << stWarnings << " warnings in "
       << stTotal << " modules." << endl;
  // Success
  return 0;
}
/* ------------------------------------------------------------------------- */
static int GenDoc(void)
{ // Typedefs
  struct StrStrStr { string strName, strType, strDesc; };
  typedef list<StrStrStr> StrStrStrList;
  struct ApiFunc { StrStrStrList lParameters, lReturns;
                   StrList lDescription; };
  typedef map<string, ApiFunc> FuncList;
  typedef pair<string, ApiFunc> FuncListPair;
  struct ApiNameSpace { StrList slDesc;
    FuncList flMembers, flMethods, flConsts; };
  typedef map<string, ApiNameSpace> ApiList;
  typedef pair<string, ApiNameSpace> ApiListPair;
  // The api library
  ApiList alApi;
  // Data for the current namespace
  ApiNameSpace ansCurrent;
  string strClass;
  // Data for the current function
  ApiFunc afCurrent;
  string strFunc;
  enum Type { TYP_NONE, TYP_METHOD, TYP_MEMBER, TYP_CONST } eType = TYP_NONE;
  // Classes members and methods
  size_t stTotalClasses = 0,
         stTotalMembers = 0,
         stTotalMethods = 0,
         stTotalConsts = 0;
  // Read source files directory
  const Dir dFiles{ SRCDIR, SRCEXT };
  for(const DirEntMapPair &dempPair : dFiles.GetFiles()) try
  { // Ignore if file doesnt begin with ll
    if(dempPair.first.substr(0, 2) != "ll") continue;
    // Build actual filename
    const string strFile{ StrAppend(SRCDIR, '/', dempPair.first) };
    // Loading this file
    cout << strFile << "... ";
    // Read file and memory block is empty?
    const string strData{
      FStream{ strFile, FM_R_B }.FStreamReadStringSafe() };
    if(strData.empty()) throw runtime_error{ "failed!\n" };
    // Report the number of that were read
    cout << strData.length() << "b... ";
    // Detect file format and split to lines
    const Token tLines{ strData, StrGetReturnFormat(strData) };
    // Report lines read.
    cout << tLines.size() << "#... ";
    // Variables needed to read file
    size_t stLine = 0, stClasses = 0, stMembers = 0,
           stMethods = 0, stConsts = 0;
    // Enumerate through all the parsed lines
    for(const string &strLine : tLines) try
    { // Increment line number
      ++stLine;
      // Must be at least 4 characters
      if(strLine.length() < 4) continue;
      // Must begin with '//' to be of some significance
      if(strLine.substr(0, 3) != "// ") continue;
      // Whats the id character?
      switch(strLine[3])
      { // New class?
        case '%':
        { // Working on class descriptor?
          if(!strClass.empty())
          { // Working on a function?
            if(!strFunc.empty())
            { // Add it to api func list
              switch(eType)
              { // No type? (error)
                case TYP_NONE: throw runtime_error{ "No type set" };
                // Method?
                case TYP_METHOD: ansCurrent.flMethods.
                  insert(FuncListPair(strFunc, afCurrent));
                  break;
                // Member?
                case TYP_MEMBER: ansCurrent.flMembers.
                  insert(FuncListPair(strFunc, afCurrent));
                  break;
                // Const?
                case TYP_CONST:  ansCurrent.flConsts.
                  insert(FuncListPair(strFunc, afCurrent));
                  break;
                // Unknown
                default: throw runtime_error{ "Unknown type" };
              } // Clear data on current function
              afCurrent.lParameters.clear();
              afCurrent.lReturns.clear();
              afCurrent.lDescription.clear();
              // clear function name
              strFunc.clear();
            } // Copy it to the api library
            alApi.insert(ApiListPair(strClass, ansCurrent));
            // Clear the current namespace
            ansCurrent.slDesc.clear();
            ansCurrent.flMembers.clear();
            ansCurrent.flMethods.clear();
            ansCurrent.flConsts.clear();
            eType = TYP_NONE;
          } // Set class name
          strClass = strLine.substr(5);
          // New class
          ++stClasses;
          // Done
          break;
        } // Class descriptor?
        case '!':
        { // Add to current class description
          ansCurrent.slDesc.push_back(strLine.length() > 5 ?
            strLine.substr(5) : cCommon->CommonBlank());
          // Done
          break;
        } // New CONST table?
        case '@':
        { // Working on existing API func struct?
          if(!strFunc.empty())
          { // Add it to api func list
            switch(eType)
            { // No type? (error)
              case TYP_NONE: throw runtime_error{ "No type set" };
              // Method?
              case TYP_METHOD: ansCurrent.flMethods.
                insert(FuncListPair(strFunc, afCurrent));
                break;
              // Member?
              case TYP_MEMBER: ansCurrent.flMembers.
                insert(FuncListPair(strFunc, afCurrent));
                break;
              // Const?
              case TYP_CONST:  ansCurrent.flConsts.
                insert(FuncListPair(strFunc, afCurrent));
                break;
              // Unknown
              default: throw runtime_error{ "Unknown type" };
            } // Clear data on current function
            afCurrent.lParameters.clear();
            afCurrent.lReturns.clear();
            afCurrent.lDescription.clear();
          } // Re-initialise struct members
          const string strNewFunc{ strLine.substr(5) };
          // Find colon and if we didn't find it?
          eType = TYP_CONST;
          size_t stPos = strNewFunc.find(':');
          if(stPos == StdNPos)
          { // Find period and if we didn't find it? Parse error
            stPos = strNewFunc.find('.');
            if(stPos == StdNPos)
              throw runtime_error{ "Period not found for new api func" };
          } // Is a member function
          else throw runtime_error{ "Method not allowed in const" };
          // Check the class didn't change
          if(strNewFunc.substr(0, stPos) != strClass)
            throw runtime_error{ "Unexpected class change" };
          // Set the new function name
          strFunc = strNewFunc.substr(stPos+1);
          for(char cChar : strFunc)
            if(StdIsNotAlnum(static_cast<int>(cChar)) &&
              cChar != ':' && cChar != '.')
                throw runtime_error{ "Invalid class name" };
          // New function
          ++stConsts;
          // Done
          break;
        } // New API function?
        case '$':
        { // Working on existing API func struct?
          if(!strFunc.empty())
          { // Add it to api func list
            switch(eType)
            { // No type? (error)
              case TYP_NONE: throw runtime_error{ "No type set" };
              // Method?
              case TYP_METHOD: ansCurrent.flMethods.
                insert(FuncListPair(strFunc, afCurrent));
                break;
              // Member?
              case TYP_MEMBER: ansCurrent.flMembers.
                insert(FuncListPair(strFunc, afCurrent));
                break;
              // Const?
              case TYP_CONST:  ansCurrent.flConsts.
                insert(FuncListPair(strFunc, afCurrent));
                break;
              // Unknown
              default: throw runtime_error{ "Unknown type" };
            } // Clear data on current function
            afCurrent.lParameters.clear();
            afCurrent.lReturns.clear();
            afCurrent.lDescription.clear();
          } // Re-initialise struct members
          const string strNewFunc{ strLine.substr(5) };
          // Find colon and if we didn't find it?
          size_t stPos = strNewFunc.find(':');
          if(stPos == StdNPos)
          { // Find period and if we didn't find it? Parse error
            stPos = strNewFunc.find('.');
            if(stPos == StdNPos)
              throw runtime_error{ "Period not found for new api func" };
            // Is not a member function
            else eType = TYP_MEMBER;
          } // Is a member function
          else eType = TYP_METHOD;
          // Check the class didn't change
          if(strNewFunc.substr(0, stPos) != strClass)
            throw runtime_error{ "Unexpected class change" };
          // Set the new function name
          strFunc = strNewFunc.substr(stPos+1);
          for(char cChar : strFunc)
            if(StdIsNotAlnum(static_cast<int>(cChar)) &&
              cChar != ':' && cChar != '.')
                throw runtime_error{ "Invalid function name" };
          // New function
          switch(eType)
          { // No type? (error)
            case TYP_NONE: throw runtime_error{ "No type set" };
            // Method?
            case TYP_METHOD: ++stMethods; break;
            // Member?
            case TYP_MEMBER: ++stMembers; break;
            // Const?
            case TYP_CONST: ++stConsts; break;
            // Unknown
            default: throw runtime_error{ "Unknown type" };
          } // Done
          break;
        } // Input or output pparameter description?
        case '>': case '<':
        { // Create input parameter
          StrStrStr sParams;
          // Get line
          sParams.strName = strLine.substr(5);
          // Find colon and if not found? Parse error
          size_t stPos = sParams.strName.find(':');
          if(stPos == StdNPos)
            throw runtime_error{ "Colon not found for in/out param" };
          // Set rest of string and name of parameter
          sParams.strType = sParams.strName.substr(stPos + 1);
          sParams.strName = sParams.strName.substr(0, stPos);
          // Find equals
          stPos = sParams.strType.find('=');
          // Not found? Parse error
          if(stPos == StdNPos)
            throw runtime_error{ "Equals not found for in/out param" };
          // Set rest of string and name of parameter
          sParams.strDesc = sParams.strType.substr(stPos + 1);
          sParams.strType = sParams.strType.substr(0, stPos);
          // Add to parameters list
          (strLine[3] == '>' ? afCurrent.lParameters : afCurrent.lReturns).
            push_back(sParams);
          // Done
          break;
        } // Description line?
        case '?':
        { // Have enough characters?
          if(strLine.length() > 5)
          { // Get line after the #, remove trailing spaces and return if empty
            string strPart{ CryptEntEncode(strLine.substr(5)) };
            while(!strPart.empty() && strPart.back() == ' ')
              strPart.pop_back();
            if(strPart.empty()) strPart = "<BR>";
            // Whats the last letter?
            else switch(strLine.back())
            { // Forward slash? (used to join long URL's)
              case '/' : break;
              // Full-stop for line breaks?
              case '.' : strPart += "<BR>"; break;
              // Join lines with a space for anything else
              default  : strPart += " "; break;
            } // Add line to description
            afCurrent.lDescription.push_back(strPart);
          } // Not enough characters so treat as line break
          else afCurrent.lDescription.push_back("<BR>");
        } // Unknown
        default: break;
      }
    } // exception occured
    catch(const exception &e)
    { // Print line number and error
      cout << "\nException! (" << dempPair.first << ":L#" << stLine
           << "): " << e.what() << "... ";
    } // Working on class descriptor? Add it to list
    if(!strClass.empty())
    { // Working on a function?
      if(!strFunc.empty())
      { // Add it to api func list
        switch(eType)
        { // No type? (error)
          case TYP_NONE: throw runtime_error{ "No type set" };
          // Method?
          case TYP_METHOD: ansCurrent.flMethods.
            insert(FuncListPair(strFunc, afCurrent));
            break;
          // Member?
          case TYP_MEMBER: ansCurrent.flMembers.
            insert(FuncListPair(strFunc, afCurrent));
            break;
          // Const?
          case TYP_CONST: ansCurrent.flConsts.
            insert(FuncListPair(strFunc, afCurrent));
            break;
          // Unknown
          default: throw runtime_error{ "Unknown type" };
        }
        // Clear data on current function
        afCurrent.lParameters.clear();
        afCurrent.lReturns.clear();
        afCurrent.lDescription.clear();
        // clear function name
        strFunc.clear();
      } // Copy it to the api library
      alApi.insert(ApiListPair(strClass, ansCurrent));
      // Clear the current namespace
      ansCurrent.slDesc.clear();
      ansCurrent.flMembers.clear();
      ansCurrent.flMethods.clear();
      ansCurrent.flConsts.clear();
    } // Set class name
    cout << stMembers << "MM, " << stMethods << "ME, " << stConsts << "C!\n";
    // Add to totals
    stTotalClasses += stClasses;
    stTotalMembers += stMembers;
    stTotalMethods += stMethods;
    stTotalConsts += stConsts;
  } // exception occured
  catch(const exception &e)
  { // Print line number and error
    cout << "\nException! (" << dempPair.first << ") " << e.what() << "... ";
  } // Console commands storage
  StrStrMap ssmConCmds;
  { // Build actual console commands list filename
    const string strFile{ StrAppend(SRCDIR, "/conlib.hpp") };
    // Loading this file
    cout << strFile << "... ";
    // Read file and memory block is empty?
    const string strData{
      FStream{ strFile, FM_R_B }.FStreamReadStringSafe() };
    if(strData.empty()) XCL("Can't open file!", "File", strFile);
    // Report the number of that were read
    cout << strData.length() << "b... ";
    // Detect file format and split to lines
    const Token tLines{ strData, StrGetReturnFormat(strData) };
    // Report lines read.
    cout << tLines.size() << "#... ";
    // Variables needed to read file
    size_t stLine = 0;
    // Current console command name
    string strCName, strCDesc;
    // Enumerate through all the parsed lines
    for(const string &strLine : tLines) try
    { // Increment line number
      ++stLine;
      // Must be at least 4 characters
      if(strLine.length() < 4) continue;
      // Must begin with '//' to be of some significance
      if(strLine.substr(0, 3) != "// ") continue;
      // Whats the id character?
      switch(strLine[3])
      { // New console command?
        case '!':
        { // Console command name not empty?
          if(!strCName.empty())
          { // Put into cvars list for writing
            ssmConCmds.insert({ StrToLowCase(strCName), strCDesc });
            // Clear cvar description
            strCDesc.clear();
          } // Set new cvar name
          strCName = strLine.substr(5);
          break;
        } // New description?
        case '?':
        { // Add line to description
          if(strLine.length() > 5)
          { // Get line remove trailing spaces and just add a linefeed if empty
            string strPart{ CryptEntEncode(strLine.substr(5)) };
            while(!strPart.empty() && strPart.back() == ' ')
              strPart.pop_back();
            if(strPart.empty()) strCDesc += "<BR>";
            // Last letter is now a full-stop? Don't add a new line
            else if(strLine.back() != '.') strCDesc += StrAppend(strPart, ' ');
            // Else add line with a new line
            else strCDesc += StrAppend(strPart, "<BR>");
          } // Not enough characters so treat as line break
          else strCDesc += "<BR>";
          // Done
          break;
        } // Unknown
        default: break;
      }
    } // exception occured
    catch(const exception &e)
    { // Print line number and error
      cout << "\nException! (" << strFile << ":L#" << stLine
           << "): " << e.what() << "... ";
    } // CVar name not empty? Put into cvars list for writing
    if(!strCName.empty())
      ssmConCmds.insert({ StrToLowCase(strCName), strCDesc });
    // Report cvars
    cout << ssmConCmds.size() << "CC!" << endl;
  } // CVars storage
  StrStrMap ssmCVars;
  { // Build actual cvars list filename
    const string strFile{ StrAppend(SRCDIR, "/cvarlib.hpp") };
    // Loading this file
    cout << strFile << "... ";
    // Read file and memory block is empty?
    const string strData{
      FStream{ strFile, FM_R_B }.FStreamReadStringSafe() };
    if(strData.empty()) XCL("Can't open file!", "File", strFile);
    // Report the number of that were read
    cout << strData.length() << "b... ";
    // Detect file format and split to lines
    const Token tLines{ strData, StrGetReturnFormat(strData) };
    // Report lines read.
    cout << tLines.size() << "#... ";
    // Variables needed to read file
    size_t stLine = 0;
    // Current cvar name
    string strCName, strCDesc;
    // Enumerate through all the parsed lines
    for(const string &strLine : tLines) try
    { // Increment line number
      ++stLine;
      // Must be at least 4 characters
      if(strLine.length() < 4) continue;
      // Must begin with '//' to be of some significance
      if(strLine.substr(0, 3) != "// ") continue;
      // Whats the id character?
      switch(strLine[3])
      { // New cvar?
        case '!':
        { // CVar name not empty?
          if(!strCName.empty())
          { // Put into cvars list for writing
            ssmCVars.insert({ StrToLowCase(strCName), strCDesc });
            // Clear cvar description
            strCDesc.clear();
          } // Set new cvar name
          strCName = strLine.substr(5);
          break;
        } // New description?
        case '?':
        { // Add line to description
          if(strLine.length() > 5)
          { // Get line remove trailing spaces and just add a linefeed if empty
            string strPart{ CryptEntEncode(strLine.substr(5)) };
            while(!strPart.empty() && strPart.back() == ' ')
              strPart.pop_back();
            if(strPart.empty()) strCDesc += "<BR>";
            // Last letter is now a full-stop? Don't add a new line
            else if(strLine.back() != '.') strCDesc += StrAppend(strPart, ' ');
            // Else add line with a new line
            else strCDesc += StrAppend(strPart, "<BR>");
          } // Not enough characters so treat as line break
          else strCDesc += "<BR>";
          // Done
          break;
        } // Unknown
        default: break;
      }
    } // exception occured
    catch(const exception &e)
    { // Print line number and error
      cout << "\nException! (" << strFile << ":L#" << stLine
           << "): " << e.what() << "... ";
    } // CVar name not empty? Put into cvars list for writing
    if(!strCName.empty())
      ssmCVars.insert({ StrToLowCase(strCName), strCDesc });
    // Report cvars
    cout << ssmCVars.size() << "CV!" << endl;
  } // Set output file
  const string strFile{ StrFormat("$/index.html", DOCDIR) };
  // Print result
  cout << "A total of "
       << StrCPluraliseNum(stTotalMembers, "member", "members")
       << ", "
       << StrCPluraliseNum(stTotalMethods, "methods", "methods")
       << " and "
       << StrCPluraliseNum(stTotalConsts, "consts", "consts")
       << " in "
       << StrCPluraliseNum(stTotalClasses, "namespace", "namespaces")
       << " with "
       << StrCPluraliseNum(ssmConCmds.size(), "cmd", "cmds")
       << " and "
       << StrCPluraliseNum(ssmCVars.size(), "var", "vars")
       << ".\n"
          "Creating documentation '" << strFile << "'... ";
  // Create file
  FStream fSrc{ strFile, FM_W_T };
  if(!fSrc.FStreamOpened()) throw runtime_error{ "failed!" };
  // Get date
  const string strDate{ cmSys.FormatTime() };
  // Write header
  fSrc.FStreamWriteStringEx(
    "<!DOCTYPE html>\n"
    "<!-- Reference for the $ API version $.$.$.$ -->\n"
    "<!-- Automatically built at $ -->\n"
    "<!-- With $ Project Management Utility version $.$.$.$ -->\n"
    "<!-- Copyright (c) 2006-$ $, All Rights Reserved Worldwide -->\n"
    "<HTML lang=\"en-GB\">\n"
    "<HEAD>\n"
    "\t<META charset=\"UTF-8\">\n"
    "\t<META name=\"apple-mobile-web-app-capable\" content=\"yes\">\n"
    "\t<META name=\"apple-mobile-web-app-status-bar-style\" "
            "content=\"black\">\n"
    "\t<META name=\"apple-mobile-web-app-title\" content=\"$ API\">\n"
    "\t<META name=\"author\" content=\"$\">\n"
    "\t<META name=\"copyright\" "
            "content=\"Copyright © 2006-$ $, All Rights Reserved\">\n"
    "\t<META name=\"description\" "
            "content=\"Reference for the $ API version $.$.$.$\">\n"
    "\t<META name=\"format-detection\" content=\"telephone=no\">\n"
    "\t<META name=\"generator\" "
            "content=\"$ Project Management Utility version $.$.$.$\">\n"
    "\t<META name=\"google-site-verification\" "
            "content=\"BvWxrqmgaIxqe_W8AW3OGK0fdCI_JfB_-gcOOxCF9_4\">\n"
    "\t<META name=\"googlebot\" content=\"index,follow\">\n"
    "\t<META name=\"og:description\" "
      "content=\"Reference for the $ API version $.$.$.$\">\n"
    "\t<META name=\"og:image\" "
      "content=\"https://repository-images.githubusercontent.com/"
                "611875607/ee7aa468-9797-4763-9a2d-ea3d782ef413\">\n"
    "\t<META name=\"og:title\" content=\"$ $.$.$.$ API reference\">\n"
    "\t<META name=\"og:url\" "
      "content=\"https://mhatxotic.github.io/Engine/\">\n"
    "\t<META name=\"robots\" content=\"index,follow\">\n"
    "\t<META name=\"viewport\" "
            "content=\"width=device-width, initial-scale=1\">\n"
    "\t<TITLE>$ $.$.$.$ API reference</TITLE>\n"
    "\t<STYLE media=\"screen\">\n"
    "\t* { margin:0; padding:0 }\n"
    "\tBODY { background-color:#111; border:0; font-family:sans-serif; "
        "overflow-x:hidden; text-shadow: 1px 1px #111 }\n"
    "\tA, A:visited { color:#fff; font-weight:700; text-decoration: none }\n"
    "\tA:hover { color:#ff0 }\n"
    "\tBODY>H1,BODY>DIV>H2,BODY>DIV>H3,BODY>DIV>H4,BODY>DIV>P,"
      "BODY>DIV>OL>LI>DIV,BODY>DIV>PRE,BODY>ADDRESS { "
        "border:1px solid #000; border-radius:5px; "
        "box-shadow:5px 5px #00000080 }\n"
    "\tBODY>H1,BODY>DIV>H2,BODY>DIV>H3,BODY>DIV>H4,BODY>DIV>P,BODY>ADDRESS { "
        "margin:10px; padding:10px; text-align:center }\n"
    "\tBODY>H1,BODY>ADDRESS { background-color:#666; color:#fff }\n"
    "\tBODY>DIV>H2 { background-color:#555; color:#eee }\n"
    "\tBODY>DIV>H3 { background-color:#444; color:#eee }\n"
    "\tBODY>DIV>H4 { background-color:#333; color:#eee }\n"
    "\tBODY>DIV>P { background-color:#444; color:#ddd; margin-bottom:0px }\n"
    "\tBODY>DIV>PRE { background-color:#222; color:#fff; color:#fff; "
        "margin:10px; overflow-x:auto; padding:10px; white-space:pre-wrap; "
        "word-wrap:break-word; margin-bottom:10px }\n"
    "\tBODY>DIV>OL { list-style-type:none; margin:5px 5px -5px 5px; "
        "font-size:0 }\n"
    "\tBODY>DIV>OL>LI { display:inline-block; width:10%; vertical-align:top; "
        "font-size:initial }\n"
    "\tBODY>DIV>OL>LI>DIV { background-color:#222; margin:5px }\n"
    "\tBODY>DIV>OL>LI>DIV>DIV { color:#eee; margin:10px; height:480px; "
      "overflow-y:scroll }\n"
    "\tBODY>DIV>OL>LI>DIV>DIV>I { float:right }\n"
    "\tBODY>DIV>OL>LI>DIV>DIV>P { margin-bottom:10px }\n"
    "\tBODY>DIV>OL>LI>DIV>DIV>OL { margin:10px 0 0 0; list-style-type:none }\n"
    "\tBODY>DIV>OL>LI>DIV>DIV>OL>LI { margin:10px 0 0 0 }\n"
    "\tBODY>DIV>OL>LI>DIV>DIV>OL>LI>B { padding:0 0 10px 0 }\n"
    "\tBODY>DIV>OL>LI>DIV>DIV>OL>LI>I { float:right }\n"
    "\tBODY>DIV>OL>LI>DIV>DIV>OL>LI>P { color:#aaa }\n"
    "\tBODY>DIV>OL>LI>DIV>DIV>OL>LI>OL { list-style-type:none }\n"
    "\tBODY>DIV>OL>LI>DIV>DIV>OL>LI>OL>LI { display:inline-block; "
         "margin:10px 2px 10px 0 }\n"
    "\tBODY>DIV>OL>LI>DIV>DIV>OL>LI>OL>LI>A { background-color:#444; "
         "padding:5px 10px 5px 10px; border:1px solid #000; "
         "border-radius:20px; box-shadow:5px 5px #00000080 }\n"
    "\tBODY>DIV>OL>LI>DIV>DIV>OL>LI>OL>LI>A:HOVER { background-color:#666; "
         "color:#eee }\n"
    "\tBODY>DIV>OL>LI>DIV>DIV>OL>LI:last-child { margin:10px 0 0 0; "
         "padding:0 }\n"
    "\t@media only screen and (min-width:1800px) and (max-width:2200px) { "
         "BODY>DIV>OL>LI { width:20% } "
      "}\n"
    "\t@media only screen and (min-width:1400px) and (max-width:1800px) { "
         "BODY>DIV>OL>LI { width:25% } "
      "}\n"
    "\t@media only screen and (min-width:1000px) and (max-width:1400px) { "
         "BODY>DIV>OL>LI { width:33.333333333333% } "
      "}\n"
    "\t@media only screen and (min-width:600px) and (max-width:1000px) { "
         "BODY>DIV>OL>LI { width:50% } "
      "}\n"
    "\t@media only screen and (max-width:600px) { "
         "BODY>DIV>OL>LI { width:100% } "
         "BODY>DIV>OL>LI>DIV>DIV { height:unset; overflow-y:unset } "
      "}\n"
    "\t</STYLE>\n"
    "</HEAD>\n"
    "<BODY id=\"idx.top\">\n"
    "\t<H1>$ $.$.$.$ API reference</H1>\n"
    "\t<NOSCRIPT>\n"
    "\t\t<H2>JavaScript is required to show and hide elements.</H2>\n"
    "\t</NOSCRIPT>\n"
    "\t<SCRIPT>\n"
    "\t\"use strict\";\n"
    "\tvar strLast = null;\n"
    "\tfunction go(strHash) {\n"
    "\t\tif(!strHash) strHash = \"idx.top\";\n"
    "\t\tvar eleStart = document.getElementById(strHash);\n"
    "\t\tif(!eleStart) {\n"
    "\t\t\tlocation.hash = \"\";\n"
    "\t\t\treturn;\n"
    "\t\t}\n"
    "\t\tif(strLast && strLast.substr(0,3) !== \"idx\") {\n"
    "\t\t\tvar eleLast = document.getElementById(strLast);\n"
    "\t\t\tif(eleLast) eleLast.style.display = \"none\";\n"
    "\t\t}\n"
    "\t\tvar eleIdx = document.getElementById(\"idx\");\n"
    "\t\tif(strHash.substr(0,3) === \"idx\") {\n"
    "\t\t\teleIdx.style.display = \"block\";\n"
    "\t\t} else {\n"
    "\t\t\teleIdx.style.display = \"none\";\n"
    "\t\t\teleStart.style.display = \"block\";\n"
    "\t\t}\n"
    "\t\teleStart.scrollIntoView(true);\n"
    "\t\tstrLast = strHash;\n"
    "\t}\n"
    "\twindow.onhashchange = function(eEvent) {\n"
    "\t\tif(location.hash.length <= 1) location.reload();\n"
    "\t\telse go(location.hash.substr(1));\n"
    "\t\teEvent.preventDefault();\n"
    "\t}\n"
    "\twindow.onload = function() {\n"
    "\t\tgo(location.hash.substr(1));\n"
    "\t}\n"
    "\t</SCRIPT>\n"
    "\t<DIV id=\"idx\" style=\"display:none\">\n"
    "\t\t<H2>Table of contents:-</H2>\n"
    "\t\t<P>This build of the $ API consists of "
      "$ ($, $) and $ in $ with $ and $.</P>\n",
      strEName, uiVer[0], uiVer[1], uiVer[2], uiVer[3],
      strDate,
      strEName, uiVer[0], uiVer[1], uiVer[2], uiVer[3],
      cmSys.FormatTime("%Y"), strEAuthor,
      strEName,
      strEAuthor,
      cmSys.FormatTime("%Y"), strEAuthor,
      strEName, uiVer[0], uiVer[1], uiVer[2], uiVer[3],
      strEName, uiVer[0], uiVer[1], uiVer[2], uiVer[3],
      strEName, uiVer[0], uiVer[1], uiVer[2], uiVer[3],
      strEName, uiVer[0], uiVer[1], uiVer[2], uiVer[3],
      strEName, uiVer[0], uiVer[1], uiVer[2], uiVer[3],
      strEName, uiVer[0], uiVer[1], uiVer[2], uiVer[3],
      strEName,
      StrCPluraliseNum(stTotalMembers + stTotalMethods,
        "function", "functions"),
      StrCPluraliseNum(stTotalMembers, "member", "members"),
      StrCPluraliseNum(stTotalMethods, "method", "methods"),
      StrCPluraliseNum(stTotalConsts, "consts", "consts"),
      StrCPluraliseNum(stTotalClasses, "namespace", "namespaces"),
      StrCPluraliseNum(ssmConCmds.size(), "command", "commands"),
      StrCPluraliseNum(ssmCVars.size(), "cvar", "cvars"));
  // Write table of contents index
  fSrc.FStreamWriteStringEx(
    "\t\t<OL id=\"toc\">\n"
    "\t\t\t<LI id=\"idx.ns\">\n"
    "\t\t\t\t<DIV>\n"
    "\t\t\t\t\t<DIV>\n"
    "\t\t\t\t\t\t<A href=\"#idx.ns\">Namespaces</A> <I>($)</I>\n"
    "\t\t\t\t\t\t<OL>\n"
    "\t\t\t\t\t\t\t<LI><P>Categorised functions that interact with the "
                         "engine.</P></LI>\n"
    "\t\t\t\t\t\t\t<LI>\n"
    "\t\t\t\t\t\t\t\t<OL>\n",
      StrCPluraliseNum(alApi.size(), "namespace", "namespaces"));
  for(const auto &lAPIp : alApi)
    fSrc.FStreamWriteStringEx(
      "\t\t\t\t\t\t\t\t\t<LI><A href=\"#idx:$\">$</A></LI>\n",
        lAPIp.first, lAPIp.first);
  fSrc.FStreamWriteString(
    "\t\t\t\t\t\t\t\t</OL>\n"
    "\t\t\t\t\t\t\t</LI>\n"
    "\t\t\t\t\t\t\t<LI><B>Miscellaneous</B></LI>\n"
    "\t\t\t\t\t\t\t<LI>\n"
    "\t\t\t\t\t\t\t\t<OL>\n"
    "\t\t\t\t\t\t\t\t\t<LI><A href=\"#idx.cc\">CVars</A></LI>\n"
    "\t\t\t\t\t\t\t\t\t<LI><A href=\"#idx.cv\">Commands</A></LI>\n"
    "\t\t\t\t\t\t\t\t</OL>\n"
    "\t\t\t\t\t\t\t</LI>\n"
    "\t\t\t\t\t\t\t<LI><A href=\"#idx.ns\">&#x25B2; Return to summary</A> / "
                      "<A href=\"#idx.top\">top</A></LI>\n"
    "\t\t\t\t\t\t</OL>\n"
    "\t\t\t\t\t</DIV>\n"
    "\t\t\t\t</DIV>\n"
    "\t\t\t</LI>\n");
  // Write index
  for(const auto &lAPIp : alApi)
  { // Get reference to api struct
    const ApiNameSpace &aData = lAPIp.second;
    // Draw header
    fSrc.FStreamWriteStringEx(
      "\t\t\t<LI id=\"idx:$\">\n"
      "\t\t\t\t<DIV>\n"
      "\t\t\t\t\t<DIV>\n"
      "\t\t\t\t\t\t<A href=\"#idx:$\">$ namespace</A>\n"
      "\t\t\t\t\t\t<OL>\n",
        lAPIp.first, lAPIp.first, lAPIp.first);
    // Find descriptor for class and if not found or empty? Use the default
    // description.
    if(aData.slDesc.empty())
      fSrc.FStreamWriteString(
        "\t\t\t\t\t\t\t<LI><P>"
          "No description was set for this class.</P></LI>\n");
    else
    { // Start of paragraph
      fSrc.FStreamWriteString("\t\t\t\t\t\t\t<LI><P>");
      // Have lines
      if(!aData.slDesc.empty())
      { // Write lines
        auto iIt{ aData.slDesc.cbegin() };
        fSrc.FStreamWriteString(*iIt);
        while(++iIt != aData.slDesc.cend())
          fSrc.FStreamWriteString(StrAppend(' ', *iIt));
      } // End of paragraph
      fSrc.FStreamWriteString("</P></LI>\n");
    } // If we have members?
    if(!aData.flMembers.empty())
    { // Write class and members
      fSrc.FStreamWriteString(
        StrFormat("\t\t\t\t\t\t\t<LI>\n"
                  "\t\t\t\t\t\t\t\t<B>Members</B> <I>($)</I>\n"
                  "\t\t\t\t\t\t\t\t<OL>\n",
          StrCPluraliseNum(aData.flMembers.size(), "member", "members")));
      // For each member, write the link to it
      for(const auto &flpItem : aData.flMembers)
        fSrc.FStreamWriteStringEx(
          "\t\t\t\t\t\t\t\t\t<LI><A href=\"#$.$\">$</A></LI>\n",
          lAPIp.first, flpItem.first, flpItem.first);
      // Write end of index
      fSrc.FStreamWriteString("\t\t\t\t\t\t\t\t</OL>\n"
                              "\t\t\t\t\t\t\t</LI>\n");
    } // If we have no methods? Finish the namespace part and continue
    if(!aData.flMethods.empty())
    { // Write class and members
      fSrc.FStreamWriteString(
        StrFormat("\t\t\t\t\t\t\t<LI>\n"
                  "\t\t\t\t\t\t\t\t<B>Methods</B> <I>($)</I>\n"
                  "\t\t\t\t\t\t\t\t<OL>\n",
          StrCPluraliseNum(aData.flMethods.size(), "method", "methods")));
      // For each method, write the link for it
      for(const auto &flpItem : aData.flMethods)
        fSrc.FStreamWriteStringEx(
          "\t\t\t\t\t\t\t\t\t<LI><A href=\"#$:$\">$</A></LI>\n",
          lAPIp.first, flpItem.first, flpItem.first);
      // Write end of index
      fSrc.FStreamWriteString("\t\t\t\t\t\t\t\t</OL>\n"
                              "\t\t\t\t\t\t\t</LI>\n");
    } // If we have consts?
    if(!aData.flConsts.empty())
    { // Write class and members
      fSrc.FStreamWriteStringEx(
        "\t\t\t\t\t\t\t<LI>\n"
        "\t\t\t\t\t\t\t\t<B>Consts</B> <I>($)</I>\n"
        "\t\t\t\t\t\t\t\t<OL>\n",
          StrCPluraliseNum(aData.flConsts.size(), "const", "consts"));
      // For each member, write the link to it
      for(const auto &flpItem : aData.flConsts)
        fSrc.FStreamWriteStringEx(
          "\t\t\t\t\t\t\t\t\t<LI><A href=\"#$.$\">$</A></LI>\n",
          lAPIp.first, flpItem.first, flpItem.first);
      // Write end of index
      fSrc.FStreamWriteString("\t\t\t\t\t\t\t\t</OL>\n"
                              "\t\t\t\t\t\t\t</LI>\n");
    } // End of namespace
    fSrc.FStreamWriteStringEx(
      "\t\t\t\t\t\t\t<LI><A href=\"#idx:$\">&#x25B2; Return to summary</A> / "
                    "<A href=\"#idx.top\">top</A></LI>\n"
      "\t\t\t\t\t\t</OL>\n"
      "\t\t\t\t\t</DIV>\n"
      "\t\t\t\t</DIV>\n"
      "\t\t\t</LI>\n", lAPIp.first);
  } // Write console commands
  fSrc.FStreamWriteStringEx(
    "\t\t\t<LI id=\"idx.cc\">\n"
    "\t\t\t\t<DIV>\n"
    "\t\t\t\t\t<DIV>\n"
    "\t\t\t\t\t\t<A href=\"#cmds\">Commands</A> <I>($)</I>\n"
    "\t\t\t\t\t\t<OL>\n"
    "\t\t\t\t\t\t\t<LI><P>Console commands that can see and manipulate all "
                         "the data in the engine.</P></LI>\n"
    "\t\t\t\t\t\t\t<LI>\n"
    "\t\t\t\t\t\t\t\t<OL>\n",
      StrCPluraliseNum(ssmConCmds.size(), "command", "commands"));
  for(const auto &ssmpItem : ssmConCmds)
    fSrc.FStreamWriteStringEx(
      "\t\t\t\t\t\t\t\t\t<LI><A href=\"#cc.$\">$</A></LI>\n",
      ssmpItem.first, ssmpItem.first);
  fSrc.FStreamWriteString("\t\t\t\t\t\t\t\t</OL>\n"
                          "\t\t\t\t\t\t\t</LI>\n"
    "\t\t\t\t\t\t\t<LI><A href=\"#idx.cc\">&#x25B2; Return to summary</A> / "
                      "<A href=\"#idx.top\">top</A></LI>\n"
    "\t\t\t\t\t\t</OL>\n"
    "\t\t\t\t\t</DIV>\n"
    "\t\t\t\t</DIV>\n"
    "\t\t\t</LI>\n");
  // Write cvars
  fSrc.FStreamWriteStringEx(
    "\t\t\t<LI id=\"idx.cv\">\n"
    "\t\t\t\t<DIV>\n"
    "\t\t\t\t\t<DIV>\n"
    "\t\t\t\t\t\t<A href=\"#cvars\">CVars</A> <I>($)</I>\n"
    "\t\t\t\t\t\t<OL>\n"
    "\t\t\t\t\t\t\t<LI><P>Variables that change the behaviour of the "
                         "engine.</P></LI>\n"
    "\t\t\t\t\t\t\t<LI>\n"
    "\t\t\t\t\t\t\t\t<OL>\n",
      StrCPluraliseNum(ssmCVars.size(), "cvar", "cvars"));
  for(const auto &ssmpItem : ssmCVars)
    fSrc.FStreamWriteStringEx(
      "\t\t\t\t\t\t\t\t\t<LI><A href=\"#cv.$\">$</A></LI>\n",
      ssmpItem.first, ssmpItem.first);
  fSrc.FStreamWriteString("\t\t\t\t\t\t\t\t</OL>\n"
                          "\t\t\t\t\t\t\t</LI>\n"
    "\t\t\t\t\t\t\t<LI><A href=\"#idx.cv\">&#x25B2; Return to summary</A> / "
                      "<A href=\"#idx.top\">top</A></LI>\n"
    "\t\t\t\t\t\t</OL>\n"
    "\t\t\t\t\t</DIV>\n"
    "\t\t\t\t</DIV>\n"
    "\t\t\t</LI>\n");
  // Write end of index
  fSrc.FStreamWriteString("\t\t</OL>\n"
                          "\t</DIV>\n");
  // Write body
  for(const auto &lAPIp : alApi)
  { // Get reference to api struct
    const ApiNameSpace &aData = lAPIp.second;
    // Process member and methods together
    struct METHODMEMBERDATA {
      const FuncList &flList;
      const unsigned char ucSep;
      const string strLeft, strRight;
    };
    const array<const METHODMEMBERDATA,3>mmData{{
      { aData.flMembers, '.', "(", ")" },
      { aData.flMethods, ':', "(", ")" },
      { aData.flConsts,  '.', "",  ""  }
    }};
    // Walk through the data
    for(const METHODMEMBERDATA &mmItem : mmData)
    { // Walk through the functiond
      for(const auto &flpItem : mmItem.flList)
      { // Get reference to function data
        const ApiFunc &afData = flpItem.second;
        // Formatted parameter list
        string strInParams;
        if(!afData.lParameters.empty())
        { for(const StrStrStr &lInParam : afData.lParameters)
            strInParams += StrFormat("<U>$</U>, ", lInParam.strName);
          if(!strInParams.empty()) strInParams.pop_back();
          if(!strInParams.empty()) strInParams.pop_back();
        } // For building return parameters list
        string strOutParams;
        if(!afData.lReturns.empty())
        { for(const StrStrStr &lOutParam : afData.lReturns)
            strOutParams += StrFormat("<U>$</U>, ", lOutParam.strName);
          if(!strOutParams.empty()) strOutParams.pop_back();
          if(!strOutParams.empty()) strOutParams.pop_back();
          strOutParams += " = ";
        } // Start of function declaration
        const string strCF{
          StrAppend(lAPIp.first, mmItem.ucSep, flpItem.first) };
        fSrc.FStreamWriteStringEx(
          "\t<DIV id=\"$\" style=\"display:none\">\n"
          "\t\t<H3>$</H3>\n"
          "\t\t<H4>Syntax:-</H4>\n"
          "\t\t<PRE>$$$$$</PRE>\n",
          strCF, strCF, strOutParams, strCF, mmItem.strLeft,
          strInParams.empty() ? cCommon->CommonBlank() :
            StrAppend("<I>", strInParams, "</I>"), mmItem.strRight);
        // Start writing parameters
        if(!afData.lParameters.empty())
        {
          fSrc.FStreamWriteString(
            "\t\t<H4>Function Parameters:-</H4>\n"
            "\t\t<PRE>");
          for(const StrStrStr &sssItem : afData.lParameters)
            fSrc.FStreamWriteStringEx("<U>$</U> <I>($)</I> = $<BR>",
              sssItem.strName, sssItem.strType, sssItem.strDesc);
          fSrc.FStreamWriteString("</PRE>\n");
        }
        if(!afData.lReturns.empty())
        {
          fSrc.FStreamWriteString("\t\t<H4>Return Parameters:-</H4>\n"
                                  "\t\t<PRE>");
          for(const StrStrStr &sssItem : afData.lReturns)
          fSrc.FStreamWriteStringEx("<U>$</U> <I>($)</I> = $<BR>",
            sssItem.strName, sssItem.strType, sssItem.strDesc);
          fSrc.FStreamWriteString("</PRE>\n");
        } // Write description
        if(!afData.lDescription.empty())
        {
          fSrc.FStreamWriteString("\t\t<H4>Synopsis:-</H4>\n"
                                  "\t\t<PRE>");
          for(const string &strDesc : afData.lDescription)
            fSrc.FStreamWriteString(strDesc);
          // Footer of descriptor
          fSrc.FStreamWriteString("</PRE>\n");
        }
        fSrc.FStreamWriteStringEx(
          "\t\t<H4><A href=\"#idx:$\">&#x25B2; Return to table of "
            "contents</A></H4>\n"
          "\t</DIV>\n", lAPIp.first);
      }
    }
  } // Write console command descriptions
  for(const auto &ssmpItem : ssmConCmds)
  { // Start of section
    fSrc.FStreamWriteStringEx(
      "\t<DIV id=\"cc.$\" style=\"display:none\">\n"
      "\t\t<H3>$</H3>\n",
      ssmpItem.first, ssmpItem.first);
    // Find descriptor for class and if not found or empty? Use the default
    // description.
    if(ssmpItem.second.empty())
      fSrc.FStreamWriteString(
        "\t\t<P>No description was set for this console command.</P>\n");
    // Start of paragraph
    else fSrc.FStreamWriteString(
      StrFormat("\t\t<PRE>$</PRE>\n", ssmpItem.second));
    // Write footer for function
    fSrc.FStreamWriteString(
      "\t\t<H4><A href=\"#idx.cc\">"
        "&#x25B2; Return to table of contents</A></H4>\n"
      "\t</DIV>\n");
  } // Write cvar descriptions
  for(const auto &ssmpItem : ssmCVars)
  { // Start of section
    fSrc.FStreamWriteStringEx(
      "\t<DIV id=\"cv.$\" style=\"display:none\">\n"
      "\t\t<H3>$</H3>\n",
      ssmpItem.first, ssmpItem.first);
    // Find descriptor for class and if not found or empty? Use the default
    // description.
    if(ssmpItem.second.empty())
      fSrc.FStreamWriteString(
        "\t\t<P>No description was set for this cvar.</P>\n");
    // Start of paragraph
    else fSrc.FStreamWriteString(
      StrFormat("\t\t<PRE>$</PRE>\n", ssmpItem.second));
    // Write footer for function
    fSrc.FStreamWriteString(
      "\t\t<H4><A href=\"#idx.cv\">"
               "&#x25B2; Return to table of contents</A></H4>\n"
      "\t</DIV>\n");
  } // Write footer
  fSrc.FStreamWriteString(
    StrFormat("\t<ADDRESS>Copyright &copy; $ $, All Rights Reserved.<BR>\n"
      "\tThis document was generated at $</ADDRESS>\n"
      "</BODY>\n"
      "</HTML>\n",
        cmSys.FormatTime("%Y"), strEAuthor, strDate));
  // Close file
  cout << "Wrote " << fSrc.FStreamSize() << " bytes.\n";
  fSrc.FStreamClose();
  // Done
  cout << "\nCompleted.\n";
  return 0;
}
/* ------------------------------------------------------------------------- */
static void ConWrite(const string &strText)
{ // Pretty word wrap the text and write each line
#if defined(WINDOWS)
  const StrVector svLines{ UtfWordWrap(strText, 79, 2) };
  for(const string &strLine : svLines) cout << strLine << endl;
#else
  cout << strText << endl;
#endif
}
/* ------------------------------------------------------------------------- */
#if defined(WINDOWS)
static void PatchIcon(const string &strIco, const string &strOut)
{ // Using Windows API
  using namespace Lib::OS;
  // Return if file does not exist
  if(!DirLocalFileExists(strIco))
    XC("Icon file not found!", "File", strIco, "Exe", strOut);
  // Only valid on WIN32
  // Define structs and pack to byte boundaries
#pragma pack(push, 1)
  // This is icon directory header
  struct ICONDIR { WORD usMagic, usType, usCount; };
  // This is the header for each image in the directory
  struct ICONDIRENT { BYTE bWidth, bHeight, bColorCount, bReserved;
    WORD wPlanes, wBitCount; DWORD dwBytesInRes, dwImageOffset; };
  // This is an altereted format when pushed into executable resource
  struct RESICONDIRENT { BYTE bWidth, bHeight, bColorCount, bReserved;
    WORD wPlanes, wBitCount; DWORD dwBytesInRes; WORD wId; };
#pragma pack(pop)
  // Read the icon file into memory
  const Memory mbData{ FStream{ strIco, FM_R_B }.FStreamReadBlockSafe() };
  if(mbData.MemIsEmpty())
    XCL("Failed to open icon file!", "File", strIco);
  // Get icon data as structure
  ICONDIR &idHdr = *mbData.MemPtr<ICONDIR>();
  // Check header
  if(idHdr.usMagic)
    XC("Icon file not valid!",
       "File", strIco, "Expect", 0, "Actual", idHdr.usMagic);
  if(idHdr.usType != 1)
    XC("Icon file type not actually ICO format!",
       "File", strIco, "Expect", 1, "Actual", idHdr.usType);
  if(!idHdr.usCount)
    XC("Icon file has no icon directory!",
       "File", strIco, "Count", idHdr.usCount);
  // Prepare icon directory
  size_t stTotal = 0, stId, stPos, stHdrSize;
  stHdrSize = sizeof(ICONDIR)+(sizeof(RESICONDIRENT)*idHdr.usCount);
  Memory mbHdr{ stHdrSize };
  char *cpHdr = mbHdr.MemPtr<char>();
  memcpy(cpHdr, &idHdr, sizeof(ICONDIR));
  // For each icon image header
  for(stId = 0, stPos = sizeof(ICONDIR);
      stId < idHdr.usCount && stPos < mbData.MemSize();
    ++stId)
  { // Get reference to icon directory entry
    ICONDIRENT &idHdrEnt = *mbData.MemRead<ICONDIRENT>(stPos);
    // Check members
    if(!idHdrEnt.bWidth)
      XC("Icon width invalid!",
         "File",      strIco,           "Position", stPos,
         "Size",      mbData.MemSize(), "IconId",   stId,
         "IconCount", static_cast<unsigned int>(idHdr.usCount),
         "Width",     static_cast<unsigned int>(idHdrEnt.bWidth));
    if(!idHdrEnt.bHeight)
      XC("Icon height invalid!",
         "File",      strIco,           "Position", stPos,
         "Size",      mbData.MemSize(), "IconId",   stId,
         "IconCount", static_cast<unsigned int>(idHdr.usCount),
         "Height",    static_cast<unsigned int>(idHdrEnt.bHeight));
    if(idHdrEnt.bReserved)
      XC("Icon magic invalid!",
         "File",      strIco,           "Position", stPos,
         "Size",      mbData.MemSize(), "IconId",   stId,
         "IconCount", static_cast<unsigned int>(idHdr.usCount),
         "Magic",     static_cast<unsigned int>(idHdrEnt.bReserved));
    if(!idHdrEnt.wPlanes || idHdrEnt.wPlanes > 1)
      XC("Icon planes invalid!",
         "File",      strIco,           "Position", stPos,
         "Size",      mbData.MemSize(), "IconId",   stId,
         "IconCount", static_cast<unsigned int>(idHdr.usCount),
         "Planes",    static_cast<unsigned int>(idHdrEnt.wPlanes));
    if(!idHdrEnt.wBitCount || idHdrEnt.wBitCount > 32)
      XC("Icon bit-count invalid!",
         "File",      strIco,           "Position", stPos,
         "Size",      mbData.MemSize(), "IconId",   stId,
         "IconCount", static_cast<unsigned int>(idHdr.usCount),
         "Depth",     static_cast<unsigned int>(idHdrEnt.wBitCount));
    if(!idHdrEnt.dwBytesInRes)
      XC("Icon bitmap bytes invalid!",
         "File",      strIco,           "Position", stPos,
         "Size",      mbData.MemSize(), "IconId",   stId,
         "IconCount", static_cast<unsigned int>(idHdr.usCount),
         "Bytes",     static_cast<unsigned int>(idHdrEnt.dwBytesInRes));
    if(!idHdrEnt.dwImageOffset)
      XC("Icon bitmap offset invalid!",
         "File",      strIco,           "Position", stPos,
         "Size",      mbData.MemSize(), "IconId",   stId,
         "IconCount", static_cast<unsigned int>(idHdr.usCount),
         "Offset",    static_cast<unsigned int>(idHdrEnt.dwImageOffset));
    // Get reference to icon header
    RESICONDIRENT &idHdrDst = (RESICONDIRENT&)*
      (cpHdr+(sizeof(ICONDIR)+(stId*sizeof(RESICONDIRENT))));
    // Copy memory over
    memcpy(&idHdrDst, &idHdrEnt, sizeof(RESICONDIRENT));
    // Set id
    idHdrDst.wId = 1;
    // Incrememnt total
    stTotal += idHdrEnt.dwBytesInRes;
    // Increment position
    stPos += sizeof(ICONDIRENT);
  }
  // Open executable for resource writing
  HANDLE hExeFile = BeginUpdateResourceW(UTFtoS16(strOut).c_str(), FALSE);
  if(hExeFile == INVALID_HANDLE_VALUE)
    XCS("Failed to open executable for updating resource!",
        "File", strOut);
  // Delete existing icon
  if(!UpdateResourceW(hExeFile, RT_ICON, MAKEINTRESOURCE(1),
    MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_UK), nullptr, 0))
      XCS("Delete icon error!", "File", strOut);
  // Write new icon
  if(!UpdateResourceW(hExeFile, RT_ICON, MAKEINTRESOURCE(1),
    MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_UK), mbData.MemRead<void>(stPos),
      static_cast<DWORD>(stTotal)))
        XCS("Insert icon resource error!", "File", strOut);
  // Delete existing icon group
  if(!UpdateResourceW(hExeFile, RT_GROUP_ICON, L"GLFW_ICON",
    MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_UK), nullptr, 0))
      XCS("Delete icon group resource error!", "File", strOut);
  // Write new icon group
  if(!UpdateResourceW(hExeFile, RT_GROUP_ICON, L"GLFW_ICON",
    MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_UK),
      UtfToNonConstCast<LPVOID>(cpHdr), static_cast<DWORD>(stHdrSize)))
        XCS("Update icon group resource error!", "File", strOut);
  // Close executable file
  if(!EndUpdateResource(hExeFile, FALSE))
    XCS("End update executable resource failed!", "File", strOut);
}
#endif
/* ------------------------------------------------------------------------- */
static const string GetTempDir(void)
{ // Get temporary directory and remove trailing slash
#if defined(WINDOWS)
  string strTmp{ cCmdLine->CmdLineGetEnv("TMP") };
  if(strTmp.length() <= 1)
    XCL("Could not read TMP environment variable!");
  // Replace backslashes with forwardslashes in directory
  switch(strTmp[strTmp.length()-1])
    { case '\\': case '/': strTmp.pop_back(); default: break; }
  // Replace backslashes with forward slashes
  for(char &cChar : strTmp) if(cChar == '\\') cChar = '/';
  // Return string
  return strTmp;
#else
  return "/tmp";
#endif
}
/* ------------------------------------------------------------------------- */
static void DoCleanCompilerTempFiles(void)
{ // Get files
#if defined(WINDOWS)
  const string strTmp{ StrAppend(GetTempDir(), '/') };
  const Dir dFiles{ strTmp };
  // For each file
  for(const DirEntMapPair &dempPair : dFiles.GetFiles())
  { // Compare length of filename
    switch(dempPair.first.length())
    { // 14 bytes and it begins with _CL_ (MS compiler junk)
      case 14: if(dempPair.first.substr(0, 4) == "_CL_") break;
               continue;
      // 38 bytes and it's a uid
      case 38: if(dempPair.first[0] == '{' &&
                  dempPair.first[37] && '}') break;
               continue;
      // Unknown
      default: continue;
    } // Delete the file and if failed?
    const string strFile{ StrAppend(strTmp, dempPair.first) };
    if(!DirFileUnlink(strFile))
      cout << "Failed to delete '" << strFile << "'!\n";
  }
#endif
}
/* ------------------------------------------------------------------------- */
static void DoClean(StrVector &svDeleted, StrVector &svNotDeleted,
  const Dir &dFiles)
{ // Write initial scan
  if(dFiles.IsFilesEmpty()) return;
  // Allocate memory for filenames
  svDeleted.reserve(svDeleted.size()+dFiles.GetFilesSize());
  svNotDeleted.reserve(svNotDeleted.size()+dFiles.GetFilesSize());
  // Enumerate files
  for(const DirEntMapPair &dempPair : dFiles.GetFiles())
  { // Delete the file and if failed?
    if(!DirFileUnlink(dempPair.first))
      svDeleted.push_back(StrFormat("$ [$:$]",
        dempPair.first, StdGetError(), StrFromErrNo()));
    // Success
    else svDeleted.push_back(dempPair.first);
  }
}
/* ------------------------------------------------------------------------- */
static void DoClean(const StrVector svExts)
{ // Files deleted
  StrVector svDeleted, svNotDeleted;
  // For each extension we don't want. Grab files and clean them
  for(const string &strExt : svExts)
    DoClean(svDeleted, svNotDeleted,
      strExt.empty() ? Dir{} : Dir{ cCommon->CommonBlank(), strExt });
  // Report result
  if(!svDeleted.empty())
    ConWrite(StrFormat("*** Deleted $ files from '$': $.",
      svDeleted.size(), DirGetCWD(), StrImplode(svDeleted, 0, ", ")));
  if(!svNotDeleted.empty())
    ConWrite(StrFormat("*** $ files not deleted from '$': $.",
      svNotDeleted.size(), DirGetCWD(), StrImplode(svNotDeleted, 0, ", ")));
}
/* ------------------------------------------------------------------------- */
static void MakeDirectory(const string &strDir)
  { if(!DirMkDirEx(strDir))
      XCL("Failed to make directory!", "Directory", strDir); }
/* ------------------------------------------------------------------------- */
static void SetDirectory(const string &strDir)
  { if(!DirSetCWD(strDir))
      XCL("Failed to set new directory!", "Directory", strDir); }
/* ------------------------------------------------------------------------- */
static void MakeAndSetDirectory(const string &strDir)
  { MakeDirectory(strDir); SetDirectory(strDir); }
/* ------------------------------------------------------------------------- */
static void DoClean(const StrVector svExts, const string &strDir)
{ // Save current directory
  const string strOldDir{ DirGetCWD() };
  // Chdir to the directory
  if(!strDir.empty()) SetDirectory(strDir);
  // Do the clean
  DoClean(svExts);
  // Restore old directory if needed
  if(!strDir.empty()) SetDirectory(strOldDir);
}
/* -- Open a process output stream ----------------------------------------- */
static FILE *POpen(const string &strCL) { return StdPOpen(strCL); }
/* -- Close a process output stream ---------------------------------------- */
static int PClose(FILE*const fHandle) { return StdPClose(fHandle); }
/* ------------------------------------------------------------------------- */
static int SpecialExecute(const string strCmd, const size_t stML,
  const bool bErrOverride = false)
{ // Clear error number
  StdSetError(0);
  // Write string tidily
  ConWrite(StrAppend("*** ", strCmd));
  // Record current time
  const ClkTimePoint tpBegin{ cmHiRes.GetTime() };
  // Execute process and capture output. throw runtime_error if failed
  FILE*const fpPipe = POpen(StrAppend(strCmd,
    (uiFlags & PF_SYSNOERR) || bErrOverride ?
      " 2>&1" : cCommon->CommonCBlank()));
  if(!fpPipe)
    XCL("Could not open pipe to execute process!",
        "CmdLine", strCmd, "Directory", DirGetCWD());
  // So we can clean up the handle
  try
  { // Prepare string vector
    StrVector svStrings;
    // Buffer for process output
    Memory aBuffer{ 4096 };
    // Until process finished
    while(fgets(aBuffer.MemPtr<char>(), aBuffer.MemSize<int>(), fpPipe))
    { // Merge duplicate strings?
      if(!(uiFlags & PF_NOMERGE))
      { // We got the line so find it in the list and if we found it then
        // ignore it!
        const StrVectorConstIt svciIt{ find(svStrings.begin(), svStrings.end(),
          aBuffer.MemPtr<char>()) };
        if(svciIt != svStrings.end()) continue;
        // If we've exceeded our allowable limit then ignore it as well
        if(svStrings.size() >= stML) continue;
        // Add it to list
        svStrings.push_back(aBuffer.MemPtr<char>());
      } // Print it out
      cout << aBuffer.MemPtr<char>();
    } // Read line and if not succeeded, keep retrying
    if(!feof(fpPipe))
      XCL("Could not read pipe to execute process!",
          "CmdLine", strCmd, "Directory", DirGetCWD());
  } // exception occured? Just ignore it and close handle
  catch(const exception&) { }
  // Close pipe and return result
  const int iR = PClose(fpPipe);
  // Print result if not successful
  cout << "*** Execution terminated ";
  if(iR) cout << "with status " << iR;
    else cout << "successfully";
  cout << " in " <<
    StrShortFromDuration(cmHiRes.TimePointToDouble(tpBegin), 6) << " sec!\n";
  // Return result
  return iR;
}
/* ------------------------------------------------------------------------- */
static int SpecialExecute2(const string &strCmd, const bool bOverride=true)
{ // Execute command line and throw error if it failed
  if(const int iR = SpecialExecute(strCmd,
    uiFlags & PF_SYSOUT ? StdNPos : 0, bOverride))
  { // Carriage return
    cout << '\n';
    // Throw error
    XCL("Execution returned error status!",
        "CmdLine", strCmd, "Directory", DirGetCWD(), "Exit", iR);
  } // Return status
  else return iR;
}
/* ------------------------------------------------------------------------- */
#define System(...) SpecialExecute2(__VA_ARGS__, true)
#define SystemF(...) SpecialExecute2(StrFormat(__VA_ARGS__), true)
#define SystemNF(...) SpecialExecute2(StrFormat(__VA_ARGS__), false)
/* ------------------------------------------------------------------------- */
static void RenameFileSafe(const string &strSrc, const string &strDst)
{ // Write progress
  cout << "*** Renaming '" << strSrc << "' to '" << strDst << "'...";
  // Rename the file and show error if failed
  if(DirFileRename(strSrc, strDst)) { cout << " OK!" << endl; return; }
  // File doesnt exist?
  if(errno != EEXIST)
    XCL("Rename failed!", "Source", strSrc, "Destination", strDst);
  // Try delete it and throw error if failed!
  if(!DirFileUnlink(strDst))
    XCL("Rename failed with unlink destination failure!",
        "Source", strSrc, "Destination", strDst);
  // Now try renaming and throw error if failed!
  if(!DirFileRename(strSrc, strDst))
    XCL("Rename failed after successful unlink!",
        "Source", strSrc, "Destination", strDst);
  // Done
  cout << " OW OK!" << endl;
}
/* ------------------------------------------------------------------------- */
static int CertFunc(const string strOut)
{ // Files that we care about
  const string
    strP12{ StrFormat("$/$.p12", WINDIR, ENGINENAME) },
    strCodePass{ StrAppend(strP12, ".txt") };
  // Destination file specified?
  if(!strOut.empty())
  { // Read password to string and return if failed
    const string strPass{ FStream{
      strCodePass, FM_R_B }.FStreamReadBlockSafe().MemToStringSafe() };
    if(strPass.empty()) XCL("Empty password!", "File", strCodePass);
    // Move to temp file because osslsigncode wont work
    const string strOutUnsigned{ StrAppend(strOut, ".unsigned") };
    SystemF("mv -fv \"$\" \"$\"", strOut, strOutUnsigned);
    // Do sign
    SystemF("osslsigncode sign "
            "-pkcs12 \"$\" "
            "-pass \"$\" "
            "-in \"$\" "
            "-out \"$\"", strP12, strPass, strOutUnsigned, strOut);
    // Done
    return 0;
  } // Create a strong random password
  const string
    strInPass{ CryptMBtoB64(CryptRandomBlock(256)) }, // Must be <= 1023 bytes
    strCrypt{ "-algorithm RSA "
              "-pkeyopt rsa_keygen_bits:4096" },
    strRootPrv{ StrFormat("$/$-r-pr.pem", DBGDIR, ENGINENAME) },
    strRootPub{ StrFormat("$/$-r-pu.pem", DBGDIR, ENGINENAME) },
    strRootPubConf{ StrFormat("$/$-r-pu.txt", DBGDIR, ENGINENAME) },
    strCodePrv{ StrFormat("$/$-c-pr.pem", DBGDIR, ENGINENAME) },
    strCodePub{ StrFormat("$/$-c-pu.pem", DBGDIR, ENGINENAME) },
    strCodePubConf{ StrFormat("$/$-c-pu.txt", DBGDIR, ENGINENAME) },
    strCodeCer{ StrFormat("$/$-c-x509.cer", DBGDIR, ENGINENAME) };
  // Generate CA private key
  SystemF("openssl genpkey "
          "$ "
          "-aes-256-cbc "
          "-pass \"pass:$\" "
          "-out \"$\"",
    strCrypt, strInPass, strRootPrv);
  // Generate CA public key configuration file
  FStream{ strRootPubConf, FM_W_B }.FStreamWriteStringEx(
    "[req]\n"
     "encrypt_key = yes\n"
     "prompt = no\n"
     "utf8 = yes\n"
     "string_mask = utf8only\n"
     "distinguished_name = dn\n"
     "x509_extensions = v3_ca\n"
     "\n"
    "[v3_ca]\n"
     "subjectKeyIdentifier = hash\n"
     "basicConstraints = critical, CA:TRUE, pathlen:0\n"
     "keyUsage = critical, keyCertSign, cRLSign\n"
     "\n"
    "[dn]\n"
     "CN = $ Root CA v1\n", cSystem->ENGAuthor());
  // Generate CA public key
  SystemF("openssl req "
          "-batch "
          "-verbose "
          "-new "
          "-sha512 "
          "-x509 "
          "-days 1826 "
          "-passin \"pass:$\" "
          "-key \"$\" "
          "-out \"$\" "
          "-config \"$\"",
    strInPass, strRootPrv, strRootPub, strRootPubConf);
  // Generate code sign private key
  SystemF("openssl genpkey "
          "$ "
          "-aes-256-cbc "
          "-pass \"pass:$\" "
          "-out \"$\"",
    strCrypt, strInPass, strCodePrv);
  // Generate CSR configuration file
  FStream{ strCodePubConf, FM_W_B }.FStreamWriteStringEx(
    "[req]\n"
     "encrypt_key = yes\n"
     "prompt = no\n"
     "utf8 = yes\n"
     "string_mask = utf8only\n"
     "distinguished_name = dn\n"
     "req_extensions = v3_req\n"
     "\n"
    "[v3_req]\n"
     "subjectKeyIdentifier = hash\n"
     "keyUsage = critical, digitalSignature\n"
     "# msCodeInd = Microsoft Individual Code Signing\n"
     "# msCodeCom = Microsoft Commercial Code Signing\n"
     "extendedKeyUsage = critical, codeSigning, msCodeInd\n"
     "\n"
    "[dn]\n"
     "CN = $ Code Signing Authority\n", cSystem->ENGAuthor());
  // Generate CSR private certificate
  SystemF("openssl req "
          "-batch "
          "-verbose "
          "-new "
          "-sha512 "
          "-passin \"pass:$\" "
          "-key \"$\" "
          "-out \"$\" "
          "-config \"$\"",
    strInPass, strCodePrv, strCodePub, strCodePubConf);
  // Generate CSR public certificate
  SystemF("openssl x509 "
          "-req "
          "-sha512 "
          "-days 1095 "
          "-extfile \"$\" "
          "-extensions v3_req "
          "-in \"$\" "
          "-passin \"pass:$\" "
          "-CA \"$\" "
          "-CAkey \"$\" "
          "-CAcreateserial "
          "-out \"$\"",
    strCodePubConf, strCodePub, strInPass, strRootPub, strRootPrv, strCodeCer);
  // Write output password
  const string strOutPass{ CryptMBtoB64(CryptRandomBlock(1024)) };
  FStream{ strCodePass, FM_W_B }.FStreamWriteString(strOutPass);
  // Generate P12 certificate
  SystemF("openssl pkcs12 "
          "-export "
          "-keypbe aes-256-cbc "
          "-certpbe aes-256-cbc "
          "-macalg sha512 "
          "-passout \"pass:$\" "
          "-passin \"pass:$\" "
          "-inkey \"$\" "
          "-in \"$\" "
          "-chain "
          "-CAfile \"$\" "
          "-out \"$\"",
    strOutPass, strInPass, strCodePrv, strCodeCer, strRootPub, strP12);
  // Done
  return 0;
}
/* ------------------------------------------------------------------------- */
#if defined(WINDOWS)
static void PatchChecksum(const string &strOut)
{ // Need Windows API
  using namespace Lib::OS;
  // Read executable file
  const Memory mB{ FStream{ strOut, FM_R_B }.FStreamReadBlockSafe() };
  if(mB.MemIsEmpty())
    XCL("Could not read whole executable file!", "File", strOut);
  // Align a dos header structure to buffer
  IMAGE_DOS_HEADER &DOSHDR = *mB.MemPtr<IMAGE_DOS_HEADER>();
  // Locate PE header
  IMAGE_NT_HEADERS32 &PEHDR =
    *mB.MemRead<IMAGE_NT_HEADERS32>(DOSHDR.e_lfanew);
  // Bail if not valid MZ signature
  if(DOSHDR.e_magic != IMAGE_DOS_SIGNATURE)
    XC("Executable DOS signature not valid!",
       "File",   strOut, "Expect", IMAGE_DOS_SIGNATURE,
       "Actual", static_cast<unsigned int>(DOSHDR.e_magic));
  // Bail if not valid PE signature
  if(PEHDR.Signature != IMAGE_NT_SIGNATURE)
    XC("Executable PE signature not valid!",
       "File",   strOut, "Expect", IMAGE_NT_SIGNATURE,
       "Actual", static_cast<unsigned int>(PEHDR.Signature));
  // Get actual CRC of executable. If call failed? Bail
  DWORD HS, CS;
  if(MapFileAndCheckSumW(UTFtoS16(strOut).c_str(), &HS, &CS) != S_OK)
    XCS("Failed to map executable file and checksum!",
        "File", strOut, "Headersum", static_cast<unsigned int>(HS),
                        "Checksum",  static_cast<unsigned int>(CS));
  // Patch Checksum
  PEHDR.OptionalHeader.CheckSum = CS;
  // Write executable back out
  if(FStream{ strOut, FM_W_B }.FStreamWriteBlock(mB) != mB.MemSize())
    XCL("Could not write executable file!",
        "File", strOut, "Size", mB.MemSize());
}
#endif
/* ------------------------------------------------------------------------- */
#if defined(WINDOWS) || defined(LINUX)
static void BuildExecutable(const string &strTmp, const string &strOS,
  const unsigned int uiBits, const unsigned int uiArch)
{ // Build filename for project assets
  const string strAdb{ StrFormat("$/$.adb", strName, strName) };
  // Executable filenames
  const string strIn{ StrFormat("$/$$$",
    BINDIR, ENGINENAME, uiBits, envActive.cpEXE) };
  const string strOut{ StrFormat("$/$$",
    strTmp, strName, envActive.cpEXE) };
  const string strOutTmp{ StrAppend(strOut, ".tmp") };
  // Copy binary, make it executable (upx needs this) and compress it
  SystemF("cp -fv \"$\" \"$\"", strIn, strOutTmp);
  // Using windows?
#if defined(WINDOWS)
  // Patch icon if on windows
  PatchIcon(StrFormat("$/$.ico", strName, strName), strOutTmp);
#endif
  // Upx resets the attribute, let us reset it or cat won't append properly
  SystemF("chmod -v 755 \"$\"", strOutTmp);
  // Append binary and archive to a new file and make it executable
  SystemF("cat \"$\" \"$\" > \"$\"", strOutTmp, strAdb, strOut);
  SystemF("chmod -v 755 \"$\"", strOut);
  // Windows?
#if defined(WINDOWS)
  // Patch CRC checksum in PE header
  PatchChecksum(strOut);
  // Sign executable
  CertFunc(strOut);
#endif
  // Build archives
  SystemF("$ a \"$/$-$-$-X$.7z\" \"$\"",
    envActive.cp7z, DISDIR, strArch, strVer, strOS, uiArch, strOut);
}
#endif
/* ------------------------------------------------------------------------- */
static const StrStrMap LoadVars(const string &strCfg)
{ // Open file and if succeeded?
  if(FStream fsVars{ strCfg, FM_R_B })
  { // Read filee
    const string strBuffer{ fsVars.FStreamReadStringSafe() };
    // Bail if size not acceptable
    if(strBuffer.length() <= 2)
      XCL("Config file has no data!", "File", strCfg);
    // Split characters and if nothing found?
    const string strSplit{ StrGetReturnFormat(strBuffer) };
    if(strSplit.empty())
      XCL("Failed to detect config file type!", "File", strCfg);
    // Initialise it and return if we have entries
    if(const ParserConst<> pVars{ strBuffer, strSplit, '=' })
      return reinterpret_cast<const StrStrMap&>(pVars);
    // No entries so log and return failed if there are no lines
    XCL("Failed to read any useful variables!", "File", strCfg);
  } // Failure to open file
  XCL("Failed to open vars file!", "File", strCfg);
}
/* ------------------------------------------------------------------------- */
static int BuildDistro(void)
{ // Get and make temporary directory and if it failed?
  const string strTmp{ StrAppend(GetTempDir(), '/', UuId().UuIdToString()) };
  MakeDirectory(strTmp);
  // exception occured?
  try
  { // Make sure distro directory is created
    MakeDirectory(DISDIR);
    // Windows specific instructions
#if defined(WINDOWS)
    // Process 32-bit and 64-bit executable.
    BuildExecutable(strTmp, "Windows", 32, 86);
    BuildExecutable(strTmp, "Windows", 64, 64);
    // Linux specific tasks? This is much easier because we have tools.
#elif defined(LINUX)
    // Process only 64-bit executable
    BuildExecutable(strTmp, "Ubuntu", 64, 64);
    // Apple specific tasks
#elif defined(MACOS)
    // Build filenames
    const string
      strDiskDir{ StrAppend(strTmp, "/Package") },
      strAppDir{ StrFormat("$/$.app", strDiskDir, strTitle) },
      strAppCDir{ StrAppend(strAppDir, "/Contents") },
      strAppRDir{ StrAppend(strAppCDir, "/Resources") },
      strAppMDir{ StrAppend(strAppCDir, "/MacOS") },
      strPList{ StrAppend(strAppCDir, "/Info.plist") },
      strExeDst{ StrAppend(strAppMDir, '/', strName) },
      strExeSrc32{ StrFormat("$/$32$", BINDIR, ENGINENAME, envActive.cpEXE) },
      strExeSrc64{ StrFormat("$/$64$", BINDIR, ENGINENAME, envActive.cpEXE) },
      strArchSrc{ StrFormat("$/$.$", strName, strName, ARCHIVE_EXTENSION) },
      strArchDst{ StrFormat("$/$.$", strAppRDir, strName, ARCHIVE_EXTENSION) },
      strIconDir{ StrFormat("$/$.iconset", strTmp, strTitle) },
      strIconSrc{ StrFormat("$/app/icon.png", strName) },
      strPkgInfo{ StrAppend(strAppCDir, "/PkgInfo") },
      strZipSrc{ StrFormat("$/Archive.dmg", strTmp) },
      strZipDst{
        StrFormat("$/$-$-MacOS-Universal.dmg", DISDIR, strArch, strVer)};
    // Create required mac app directories
    MakeDirectory(strDiskDir);
    MakeDirectory(strAppDir);
    MakeDirectory(strAppCDir);
    MakeDirectory(strAppRDir);
    MakeDirectory(strAppMDir);
    MakeDirectory(strIconDir);
    // Build universal binary
    SystemF("lipo \"$\" \"$\" -create -output \"$\"",
      strExeSrc32, strExeSrc64, strExeDst);
    // Copy data archive
    SystemF("cp -fv \"$\" \"$\"", strArchSrc, strArchDst);
    // Write package info
    SystemF("echo APPL>\"$\"", strPkgInfo);
    // Write plist
    if(!FStream{ strPList, FM_W_T }.FStreamWriteStringEx(
      "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
      "<!DOCTYPE plist PUBLIC \"-//Apple//DTD PLIST 1.0//EN\" "
        "\"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">\n"
      "<plist version=\"1.0\">\n"
      "\t<dict>\n"
      "\t\t<key>BuildMachineOSBuild</key>\n"
      "\t\t<string>8J135</string>\n"
      "\t\t<key>CFBundleDevelopmentRegion</key>\n"
      "\t\t<string>en</string>\n"
      "\t\t<key>CFBundleExecutable</key>\n"
      "\t\t<string>$</string>\n"
      "\t\t<key>CFBundleIconFile</key>\n"
      "\t\t<string>Icon</string>\n"
      "\t\t<key>CFBundleIdentifier</key>\n"
      "\t\t<string>msd.$.$</string>\n"
      "\t\t<key>CFBundleInfoDictionaryVersion</key>\n"
      "\t\t<string>6.0</string>\n"
      "\t\t<key>CFBundleName</key>\n"
      "\t\t<string>$</string>\n"
      "\t\t<key>CFBundlePackageType</key>\n"
      "\t\t<string>APPL</string>\n"
      "\t\t<key>CFBundleShortVersionString</key>\n"
      "\t\t<string>$</string>\n"
      "\t\t<key>CFBundleSignature</key>\n"
      "\t\t<string>MSDN</string>\n"
      "\t\t<key>CFBundleVersion</key>\n"
      "\t\t<string>$</string>\n"
      "\t\t<key>LSMinimumSystemVersion</key>\n"
      "\t\t<string>10.15</string>\n"
      "\t\t<key>LSApplicationCategoryType</key>\n"
      "\t\t<string>public.app-category.games</string>\n"
      "\t\t<key>NSHighResolutionCapable</key>\n"
      "\t\t<true/>\n"
      "\t\t<key>LSUIPresentationMode</key>\n"
      "\t\t<true/>\n"
      "\t\t<key>NSPrincipalClass</key>\n"
      "\t\t<string></string>\n"
      "\t\t<key>NSHumanReadableCopyright</key>\n"
      "\t\t<string>Copyright \xC2\xA9 $, $. All Rights Reserved.</string>\n"
      "\t</dict>\n"
      "</plist>\n",
      strName, ENGINENAME, strName, strTitle, strVer, uiVer[3],
      strAuthor, cmSys.FormatTime("%Y")))
        throw runtime_error{ strPList.c_str() };
    // Convert icons
    const struct _iconlist { const int i; const char *s; } iconlist[] = {
      {  16,"icon_16x16.png"   }, {   32,"icon_16x16@2x.png"   },
      {  32,"icon_32x32.png"   }, {   64,"icon_32x32@2x.png"   },
      { 128,"icon_128x128.png" }, {  256,"icon_128x128@2x.png" },
      { 256,"icon_256x256.png" }, {  512,"icon_256x256@2x.png" },
      { 512,"icon_512x512.png" }, { 1024,"icon_512x512@2x.png" }
    };
    for(const _iconlist &iptr : iconlist)
      SystemF("sips -z $ $ \"$\" --out \"$/$\"",
        iptr.i, iptr.i, strIconSrc, strIconDir, iptr.s);
    SystemF("iconutil -c icns \"$\" --output \"$/Icon.icns\"",
      strIconDir, strAppRDir);
    // Make link to applications
    SystemF("ln -s /Applications \"$/Drag $.app here to install\"",
      strDiskDir, strTitle);
    // Sign application
    SystemF("codesign -s \"$\" --deep -f \"$\"", strAuthor, strAppDir);
    // Build archives
    SystemF("hdiutil create \"$\" -ov -volname \"$ ($) by $\" "
            "-fs HFS+ -srcfolder \"$\"",
      strZipSrc, strTitle, strVer, strAuthor, strDiskDir);
    // Convert the writable, to a compressed (and such not writable) DMG
    SystemF("hdiutil convert \"$\" -format ULMO -ov -o \"$\"",
      strZipSrc, strZipDst);
    // Sign package
    SystemF("codesign -s \"$\" --deep -f \"$\"", strAuthor, strZipDst);
#endif
    // Clean-up temporary directory
    SystemF("rm -rfv \"$\"", strTmp);
  } // exception occured?
  catch(const exception&)
  { // Clean temporary directory
    SystemF("rm -rfv \"$\"", strTmp);
    // Rethrow error
    throw;
  } // Done
  return 0;
}
/* ------------------------------------------------------------------------- */
static int CertGen(void)
{ // Need openssl library
  using namespace Lib::OS::OpenSSL;
  // Switch to resources directory. Create it if not exists
  MakeAndSetDirectory(CRTDIR);
  // Ready to download
  ConWrite("*** Parsing offical Mozilla Trusted Root CA list...");
  // Build command string to send to operating system
  const string strCmd{
      "wget -q -t 3 --no-check-certificate -O - --retry-connrefused "
    "\"--referer=http://github.com\" -U \"Mozilla/5.0 (Windows NT 5.1; "
      "WOW64; rv:34.0) Gecko/20100101 Firefox/34.0\" "
    "\"https://raw.githubusercontent.com/"
      "bagder/ca-bundle/master/ca-bundle.crt\"" };
  // Run wget command and stream the output. exception if failed
  FILE *fp = POpen(strCmd);
  if(!fp) XCL("Could not execute 'WGET', make sure the program is in your\n"
              "%PATH% or the current directory!",
              "Command", strCmd);
  // Expecting name of certificate
  bool bGetTitle = true;
  // Expecting certificate data
  bool bParseData = false;
  // Base 64 data string and certificate name
  string strData, strFilename;
  // Files written
  size_t stWritten = 0;
  // List of files that were processed
  StrSet ssProcessed, ssObsolete;
  // Until all lines read?. 1KB - 1 size buffer should be enough.
  for(string strLine(1023, '\0');
    fgets(const_cast<char*>(strLine.data()),
          static_cast<int>(strLine.capacity()), fp);
    strLine.resize(strLine.capacity()))
  { // Update size of string because we bypassed C++ functions
    strLine.resize(strlen(strLine.c_str()));
    // Remove any extra suffixed control characters
    while(!strLine.empty() && strLine[strLine.length()-1] < 32)
      strLine.pop_back();
    // Line empty? Try next line
    if(strLine.empty()) continue;
    // First character is a hash? Ignore it
    if(strLine[0] == '#') continue;
    // Need title?
    if(bGetTitle && StdIsAlnum(strLine[0]))
    { // Set name
      strFilename = strLine;
      // Set certificate being processed
      ConWrite(StrFormat("Found '$'...", strFilename));
      // No longer expecting title
      bGetTitle = false;
      // Done
      continue;
    }
    // Start of certificate?
    if(strLine == "-----BEGIN CERTIFICATE-----" && !bParseData)
      bParseData = true;
    else if(strLine == "-----END CERTIFICATE-----" && bParseData)
    { // Decode base64 and generate checksum and bail out if failedj
      Memory mOut{ CryptB64toMB(strData) };
      // Get pointer to certificate
      const unsigned char *cpPtr = mOut.MemPtr<unsigned char>();
      // Load the raw certificate and ig it succeeded?
      if(X509 *caCert = d2i_X509(nullptr, &cpPtr, mOut.MemSize<long>())) try
      { // Get purpose struct of certificate
        if(X509_PURPOSE *x509p = X509_PURPOSE_get0(X509_PURPOSE_SSL_SERVER))
        { // Get purpose id and check it, it must be a CA certificate
          if(X509_check_purpose(caCert, X509_PURPOSE_get_id(x509p), 1) == 1)
          { // Get filename for certificate using checksum
            const string strFile{ StrAppend(hex, setw(8), setfill('0'), right,
              CryptToCRC32(mOut), ".cer") };
            // Add to processed list
            ssProcessed.emplace(strFile);
            // If file exists?
            if(!DirLocalFileExists(strFile))
            { // File does note exist? Write the certificate and if success?
              if(FStream fCert{ strFile, FM_W_B })
              { // Write the certificate and if failed? Throw error!
                const size_t stActual = fCert.FStreamWriteBlock(mOut);
                if(stActual != mOut.MemSize())
                  XCL("Failed to write to certificate!",
                      "File",   strFile,  "Expected", mOut.MemSize(),
                      "Actual", stActual);
                // Certificates written
                ++stWritten;
                // Done
                ConWrite(StrFormat("-> Wrote $ bytes to '$'.",
                  mOut.MemSize(), strFile));
              } // Failed to write certificate
              else XCL("Failed to open certificate for writing!",
                       "File", strFile);
            }
          } // Not a server CA cert
          else ConWrite("-> ERROR! This is not a server CA cert!");
        } // Not a server CA cert
        else ConWrite("-> ERROR! The purpose of this certificate is unknown!");
        // Free the certificate
        X509_free(caCert);
      } // In the rare occurence that an exception occurs we should free cert
      catch(const exception &)
      { // Free the certificate
        X509_free(caCert);
        // Throw to for loop exception catcher to log
        throw;
      } // Not a server CA cert
      else ConWrite("-> ERROR! This is not a valid X509 certificate!");
      // Try the next certificate
      strData.clear();
      // No longer parsing data
      bParseData = false;
      // Expect a title again
      bGetTitle = true;
    } // Add data if parsing
    else if(bParseData) strData += strLine;
  } // Get files in the directory and enumerate them
  const Dir dCerts{ cCommon->CommonPeriod(), ".cer" };
  for(const DirEntMapPair &dempPair : dCerts.GetFiles())
  { // Find this file in written list
    const auto &aItem = ssProcessed.find(dempPair.first);
    if(aItem == ssProcessed.cend())
    { // Try to delete it
      if(!DirFileUnlink(dempPair.first))
        XCL("Failed to delete obsolete certificate!",
            "File", dempPair.first);
      // Add to obsoleted file list
      ssObsolete.emplace(dempPair.first);
    }
  } // If we have obsoleted certificates? Display them
  if(!ssObsolete.empty())
    ConWrite(StrFormat("*** $ obsolete certificates were deleted: '$'.",
      ssObsolete.size(), StrImplode(ssObsolete, 0, "', '")));
  // Close output and grab result
  if(const int iR = PClose(fp))
    XCL("Failed to download Firefox certificate store!",
        "CmdLine", strCmd, "Code", iR);
  // Show number of certificates written
  ConWrite(StrFormat("*** $ of $ certificates were written.", stWritten,
    ssProcessed.size()));
  // Done
  return 0;
}
/* ------------------------------------------------------------------------- */
static int RebuildAssets(const bool bClear)
{ // Build filename of archive database and tempa rchive database
  const string strADB{ StrAppend(strName, '.', ARCHIVE_EXTENSION) },
               strADBTmp{ StrAppend(strADB, ".tmp") };
  // Goto assets directory
  SetDirectory(strName);
  // Delete original file if requested. Throw error if it wasn't missing
  if(bClear && !DirFileUnlink(strADB) && StdIsNotError(ENOENT))
    XCL("Failed to remove previous archive!", "File", strADB);
  // Delete temporary file created by 7zip if it exists
  if(!DirFileUnlink(strADBTmp) && StdIsNotError(ENOENT))
    XCL("Failed to remove temporary archive!", "File", strADBTmp);
  // Start archiving and return if succeeded
  SystemF("$ -t7z -ms=off -mx9 -myx9 -m0=LZMA2:d64m:fb273 -mhe=on -mmt=8 -mqs "
    "-slp -ssw -sse -r u \"$\" \"-x!$32.exe\" \"-x!$.exe\" \"-x!$32\" "
    "\"-x!$\" \"-x!$32" UDB_EXTENSION "\" \"-x!$64" UDB_EXTENSION "\" "
    "\"-x!docs\" \"-x!*.md\" \"-x!*.crt\" \"-x!*.ico\" \"-x!*.dbg\" "
    "\"-x!.*\" \"-x!prv\" \"-x!*.psd\" \"-x!*." LOG_EXTENSION "\" "
    "\"-x!*." ARCHIVE_EXTENSION "\" \"-x!*." UDB_EXTENSION "\" "
    ".",
      envActive.cp7z, strADB, strName, strName, strName, strName, strName,
      strName, strName);
  // Success
  return 0;
}
/* ------------------------------------------------------------------------- */
static void CheckFiles(const StrVector &svReadable,
  const StrVector &svWritable)
{ // Check files that need to be readable
  for(const string &strFile : svReadable)
    if(!DirCheckFileAccess(strFile, 4))
      XCL("File not found or readable!", "File", strFile);
  // Check files that need to be wrtiable
  for(const string &strFile : svWritable)
    if(!DirCheckFileAccess(strFile, 2) && StdIsNotError(ENOENT))
      XCL("File not writable!", "File", strFile);
}
/* ------------------------------------------------------------------------- */
static void ReplaceTextMulti(const string &strFile,
  const StrPairList &ssplPairs)
{ // Ignore if nothing in list
  if(ssplPairs.empty())
    XC("No entries specified to replace in file!", "File", strFile);
  // Load file
  FStream fsFile{ strFile, FM_R_B };
  if(!fsFile.FStreamOpened())
    XCL("Could not open file to replace text!",
        "File", strFile, "Directory", DirGetCWD());
  // Number of changes
  size_t stChanges = 0;
  // String to write
  string strNew{ fsFile.FStreamReadStringSafe() };
  if(strNew.empty())
    XCL("Could not read file to replace text!", "File", strFile);
  fsFile.FStreamClose();
  // For each pair in the list
  for(const StrPair &sspPair : ssplPairs)
  { // Old item
    string strOld;
    // Loop...
    do
    { // Update old string with new string
      strOld = strNew;
      // Change made
      ++stChanges;
      // Find a new change
      strNew = StrReplace(strOld, sspPair.first, sspPair.second);
    } // ...until nothing was changed.
    while(strOld != strNew);
  } // No changes? Soft fail
  if(!stChanges)
    XC("No occurences replaced in file!",
       "File", strFile, "Entries", ssplPairs.size());
  // Write file with new changes and close it
  fsFile.FStreamOpen(strFile, FM_W_B);
  if(!fsFile.FStreamOpened()) throw runtime_error{ strFile.c_str() };
  fsFile.FStreamWriteString(strNew);
  fsFile.FStreamClose();
  // Write what we found
  ConWrite(StrFormat("*** Replaced $ from $ criteria in $.",
    stChanges, ssplPairs.size(), strFile));
}
/* ------------------------------------------------------------------------- */
static void ReplaceText(const string &strFile, const string &strFrom,
  const string &strTo) { ReplaceTextMulti(strFile, {{ strFrom, strTo }}); }
/* ------------------------------------------------------------------------- */
static const string BuildHPPHeader(const string &strFileName,
  const string &strDesc)
{ // A double line
  const string strHashLine(73, '#');
  // Beginning of text
  string strLines{ StrFormat(
    "/* == $ $ **\n"
    "** $ **\n"
    "** ## Mhatxotic Engine"
    "          (c) $, All Rights Reserved ## **\n"
    "** $ **\n", StrToUpCase(strFileName),
    string(69 - strFileName.length(), '='),
    strHashLine, strEAuthor, strHashLine) };
  // Word wrap the text and write lines
  const StrVector svLines{ UtfWordWrap(strDesc, 67, 0) };
  for(const string &strLine : svLines)
    strLines += StrFormat("** ## $$$ ## **\n", left, setw(67), strLine);
  // Add rest of header
  strLines += StrFormat(
    "** $ **\n"
    "** $ */\n"
    "#pragma once                           // Only one incursion allowed",
    strHashLine, string(73, '='));
  // String to return
  return strLines;
}
/* ------------------------------------------------------------------------- */
static int BuildLicenses(void)
{ // Find license files
  const Dir dLicenses{ LICDIR, ".txt" };
  if(dLicenses.IsFilesEmpty())
    XC("No license files found ending in .txt!", "Directory", LICDIR);
  // Prepare predefined styles
  const string strDoubleLine{ StrFormat("/* $ */", string(73, '=')) };
  const string strLineTop{ StrFormat("/* $ */", string(73, '-')) };
  const string strLineBottom{ "ENDLICENSE" };
  // Start of file lines
  StrVector vFiles{
    BuildHPPHeader(LICHEADER, StrFormat(
      "This file was automatically generated by build at $ when the license "
      "files are rebuilt. Changes will be lost when this process repeats.",
        cmSys.FormatTime()))
  };
  // Show counted files
  cout << "Found " << dLicenses.GetFilesSize() << " license files.\n";
  // For each file
  for(const DirEntMapPair &dempPair : dLicenses.GetFiles())
  { // Show progress
    const string strFile{ StrAppend(LICDIR, '/', dempPair.first) };
    cout << "Reading '" << strFile << "'...";
    // Read the file
    const Memory mData{
      FStream{ strFile, FM_R_B }.FStreamReadBlockSafe() };
    if(mData.MemIsEmpty())
    { // File is empty
      cout << " empty file!\n";
      // Process next file
      continue;
    } // Show size
    cout << ' ' << mData.MemSize() << "...";
    // Compress the file and show compressed size
    const Block<LZMAEncoder> bCompressed{ mData, 9 };
    cout << ' ' << bCompressed.MemSize() << "...";
    // Tokenise filename and make sure we have 3 parts
    const Token tParts{ PathSplit(dempPair.first).strFile, "-", 4 };
    if(tParts.size() != 4)
    { // Show message
      cout << " invalid filename!" << endl;
      continue;
    } // Check for correct first token
    if(tParts[0] != "Lic")
    { // Show message
      cout << " invalid format!" << endl;
      continue;
    }
    // File and line data
    StrVector vLine, vFile{
      StrFormat("$\n$$$ // $",
        strLineTop,
        setw(38), left,
        StrFormat("BEGINLICENSE($, $)", StrToUpCase(tParts[2]),
          bCompressed.MemSize()),
        dempPair.first),
      strLineTop
    };
    // Line length
    size_t stLine = StdNPos;
    // Until we've processed all the compressed file
    for(size_t stIndex = 0; stIndex < bCompressed.MemSize(); ++stIndex)
    { // Read character as integer
      const int iVal =
        static_cast<int>(bCompressed.MemReadInt<uint8_t>(stIndex));
      // Get value of index as string literal
      const string strVal{ StrFromNum(iVal) };
      // If the length of this string would go over the limit?
      if(stLine + strVal.length() + 1 > 78)
      { // Insert a new line
        vFile.push_back(StrAppend(StrImplode(vLine, 0, ","), ','));
        // Reset line with new item
        vLine = { strVal };
        // Reset line count
        stLine = strVal.length();
      } // Under the maximum length?
      else
      { // Insert into line
        vLine.push_back(strVal);
        // Add to line length
        stLine += strVal.length() + 1;
      }
    } // If we have line data remaining? Insert a new line
    if(!vLine.empty()) vFile.push_back(StrImplode(vLine, 0, ","));
    // Write footer
    vFile.push_back(strLineTop);
    vFile.push_back(StrFormat("$$$ // $ > $b > $",
      setw(38), left, strLineBottom, tParts[3], mData.MemSize(), tParts[1]));
    string strSourceFile{ StrImplode(vFile, 0, "\n") };
    cout << ' ' << strSourceFile.length() << "b... done!" << endl;
    vFiles.push_back(StdMove(strSourceFile));
  } // Write footer
  vFiles.push_back(strDoubleLine);
  // Write rest of string
  const string strFile{ StrAppend(StrImplode(vFiles, 0, "\n"), '\n') };
  // Create the file
  const string strOutFile{ SRCDIR "/" LICHEADER };
  cout << "Writing " << strFile.length() << " bytes to '"
       << strOutFile << "'...";
  const size_t stWritten =
    FStream{ strOutFile, FM_W_B }.FStreamWriteString(strFile);
  if(strFile.length() != stWritten)
    XCL("Write error!", "Directory", DirGetCWD(),      "File",   strOutFile,
                        "Expected",  strFile.length(), "Actual", stWritten);
  // Write success to console
  cout << " done!\n\nBuilding of license header has completed successfully."
       << endl;
  // Return success
  return 0;
}
/* ------------------------------------------------------------------------- */
static int NewProject(const string &strProj)
{ // Ignore if directories we use
  if(strProj == SRCDIR)
    XC("Invalid project!", "Project", strProj);
  // Must be administrator to create links on win32
#if defined(WINDOWS)
  if(!cSystem->OSIsAdmin())
    XC("Need elevated privileges to create symlinks!", "Project", strProj);
#endif
  // Split executable name
  const PathSplit pExe{ strProj };
  // Make and switch to a directory
  MakeAndSetDirectory(pExe.strFile);
  // Create links which needs admin on windoze
#if defined(WINDOWS)
  SystemF("mklink $32.exe ..\\$\\$32.exe", pExe.strFile, BINDIR, ENGINENAME);
  SystemF("mklink $32.$ $.$", pExe.strFile, UDB_EXTENSION, pExe.strFile,
    UDB_EXTENSION);
  SystemF("mklink $.exe ..\\$\\$64.exe", pExe.strFile, BINDIR, ENGINENAME);
  SystemF("attrib /L +r $32.exe", pExe.strFile);
  SystemF("attrib /L +r $.exe", pExe.strFile);
  SystemF("attrib /L +r $.$", pExe.strFile, UDB_EXTENSION);
#else
  SystemF("ln -s ../$/$32.exe $32.exe", BINDIR, ENGINENAME, pExe.strFile);
  SystemF("ln -s $.$ $32.$", pExe.strFile, UDB_EXTENSION, pExe.strFile,
    UDB_EXTENSION);
  SystemF("ln -s ../$/$64.exe $.exe ", BINDIR, ENGINENAME, pExe.strFile);
#endif
  // Create a generic app file
  FStream fApp{ StrFormat("$." CFGEXT, DEFAULT_CONFIGURATION), FM_W_T };
  if(!fApp.FStreamOpened()) throw runtime_error{ "app config file" };
  fApp.FStreamWriteString("; Write application parameters here\n"
                "app_guimode = 0");
  fApp.FStreamClose();
  // Done
  return 0;
}
/* ------------------------------------------------------------------------- */
static void MakeIncludeFromBin(const string &strIn, const string &strPrefix,
  const string &strOut)
{ // Open binary file
  cout << "*** Binary in: " << strIn << "... ";
  Memory mA{ FStream{ strIn, FM_R_B } .FStreamReadBlockSafe() };
  if(mA.MemIsEmpty()) XCL("Failed to read data file", "File", strIn);
  // Create output file
  cout << mA.MemSize() << " bytes read!\n"
       << "    Header out: " << strOut << "... ";
  FStream fOut{ strOut, FM_W_B };
  if(!fOut.FStreamOpened())
    XCL("Failed to open output header file", "File", strOut);
  // Prepare output header
  fOut.FStreamWriteStringEx("// Automatically generated by build...\n"
                       "// * Generated..........: $.\n"
                       "// * Source file........: $.\n"
                       "// * Destination file...: $.\n"
                       "\n"
                       "$[$] = { ",
    cmSys.FormatTime(), strIn, strOut, strPrefix, mA.MemSize());
  for(uint8_t *ucPtr = mA.MemPtr<uint8_t>(),
        *const ucEnd = mA.MemPtrEnd<uint8_t>();
               ucPtr < ucEnd; ++ucPtr)
      fOut.FStreamWriteStringEx("$,", static_cast<int>(*ucPtr));
  fOut.FStreamWriteString(" };\n");
  // Finsihed
  cout << fOut.FStreamTell() << " bytes written!\n";
}
/* -- Build a file list ---------------------------------------------------- */
static string BuildFileList(const string &strBase, const string &strDir,
  const string &strExt)
{ // String storage
  string strOut;
  // Build base path
  const string strPath{ StrAppend(strBase, '/', strDir) };
  // Search for files
  const Dir dFiles{ strPath, strExt };
  // Throw error if failed
  if(dFiles.IsFilesEmpty())
    XCL("Failed to find required files!", "Path", strPath, "Ext", strExt);
  // Build file list
  for(const DirEntMapPair &dempPair : dFiles.GetFiles())
    strOut += StrAppend(' ', strDir, '/', dempPair.first);
  // Return string
  return strOut;
}
/* ------------------------------------------------------------------------- */
static void GenericExtLibBuild(const string &strCmdLine, const string &strLib,
  const string &strTempDir, const string &strPrefix, bool bSwap=false)
{ // Do swap?
  if(bSwap) System("swap");
  // Execution compilation
  System(strCmdLine);
  // Build the library
  SystemF("$ *$ -out:\"$/$$\"",
    strLib, envActive.cpOBJ, strTempDir, strPrefix, envActive.cpLIB);
  // Clean up the object files
  DoClean({ envActive.cpOBJ });
  // Do swap?
  if(bSwap) System("swap");
}
/* ------------------------------------------------------------------------- */
static void GenericExtLibBuildBits(const string &strCLRel,
  const string &strLib, const string &strTmp, const string &strPrefix,
  const unsigned int uiBits)
{ // Generate prefix with bits
  const string strBits{ StrAppend(strPrefix, uiBits) };
  // Compile release version
  GenericExtLibBuild(strCLRel, strLib, strTmp, strBits, uiBits==32);
}
/* ------------------------------------------------------------------------- */
static void GenericExtLibBuildAll(const string &strCLRel64,
  const string &strCLRel32,  const string &strL64,
  const string &strL32, const string &strTmp, const string &strPrefix)
{ // Compile 64-bit version
  GenericExtLibBuildBits(strCLRel64, strL64, strTmp, strPrefix, 64);
  // Compile 32-bit release
  GenericExtLibBuildBits(strCLRel32, strL32, strTmp, strPrefix, 32);
}
/* ------------------------------------------------------------------------- */
static const string GetFiles(const string &strExt, const string &strDir="")
{ // Get files and return if empty
  Dir dEntries{ strDir, strExt };
  if(dEntries.IsFilesEmpty()) return {};
  // String for output
  string strOut; strOut.reserve(4096);
  // Make output directory
  const string strDirNew{ strDir.empty() ? strDir : StrAppend(strDir, "/") };
  const bool bHasSpace = strDirNew.find(' ') != StdNPos;
  // Get first item
  DirEntMapConstIt demciIt{ dEntries.GetFilesBegin() };
  if(!bHasSpace && demciIt->first.find(' ') == StdNPos)
    strOut = StrAppend(strDirNew, demciIt->first);
  else strOut = StrAppend("\"", strDirNew, demciIt->first, "\"");
  // Build output
  for(++demciIt; demciIt != dEntries.GetFilesEnd(); ++demciIt)
    if(!bHasSpace && demciIt->first.find(' ') == StdNPos)
      strOut += StrAppend(' ', strDirNew, demciIt->first);
    else strOut = StrAppend(" \"", strDirNew, demciIt->first, "\"");
  // Reduce memory
  strOut.shrink_to_fit();
  // Return string
  return strOut;
}
/* ------------------------------------------------------------------------- */
static void SetupZipRepo(const string &strLibPath, const string &strTmp,
  const string &strPSFile, bool bNoDir=false)
{ // Set destination temp directory
  const string strDir{ StrAppend(strTmp, '/', strPSFile) },
               strDirNo{ StrAppend(strDir, bNoDir ? "" : "/..") };
  // Extract the .7z to the temporary directory
  SystemF("$ x -aoa \"$\" -o\"$\"", envActive.cp7z, strLibPath, strDirNo);
  // Set code directory
  SetDirectory(strDir);
}
/* ------------------------------------------------------------------------- */
static void SetupTarRepoNSD(const string &strLibPath, const string &strTmp,
  const string &strPSFile)
{ // Must have .tar. in filename
  if(strLibPath.find(".tar.") == StdNPos)
    throw runtime_error{
      StrFormat("Archive '$' must contain '.tar.'!", strLibPath).c_str() };
  // Make .tar file name
  const string strTar{ StrAppend(strTmp, '/', strPSFile) };
  // Extract the .gz to the temporary directory
  if(!DirLocalFileExists(strTar))
    SystemNF("$ x -so \"$\" > \"$\"", envActive.cp7z, strLibPath, strTar);
  // Extract the .tar to the temporary directory
  SystemF("$ x -aoa \"$\" -o\"$/\"", envActive.cp7z, strTar, strTmp);
  // Remove tar archive
  if(!DirFileUnlink(strTar))
    XCL("Failed to delete tar file!", "File", strTar);
}
/* ------------------------------------------------------------------------- */
static void SetupTarRepo(const string &strLibPath, const string &strTmp,
  const string &strPSFile, const string &strPSRFile)
{ // Do the operation
  SetupTarRepoNSD(strLibPath, strTmp, strPSFile);
  // Set destination directory
  SetDirectory({ StrAppend(strTmp, '/', strPSRFile) });
}
/* ------------------------------------------------------------------------- */
static int ExtLibScript(const string &strOpt, const string &strOpt2)
{ // Lib name and base package name
  const PathSplit PSLib{ strOpt };
  const string strLib{ PSLib.strFileExt };
  // Lib name without extension
  const PathSplit PSLibR{ PSLib.strFile };
  // Make lib name. Bail if it doesn't exist
  const string strLibPath{ StrAppend(ARCDIR, '/', strLib) };
  if(!DirLocalFileExists(strLibPath))
    XCL("Specified archive not found!", "File", strLibPath);
  // Get temporary directory and remove trailing slash
  const string strTmp{ GetTempDir() };
  // Extra base package name
  const PathSplit PSLibX{ strOpt2 };
  const string &strLibX = PSLibX.strFileExt;
  // Extra lib name without extension
  const PathSplit PSLibXR{ PSLibX.strFile };
  // Make extra lib name. Bail if it doesn't exist (and was specified)
  const string strLibPathX{ StrAppend(ARCDIR, '/', strLibX) };
  // Store extra lib if specified?
  if(!strLibX.empty() &&
     strLibX != cCommon->CommonPeriod() &&
     !DirLocalFileExists(strLibPathX))
    XCL("Specified dependency archive not found!", "File", strLibPathX);
  // Mandatory compiler flags
  const string
    strA64{ StrAppend(envActive.cpAC8, ' ', envActive.cpACM) },
    strA32{ StrAppend(envActive.cpAC4, ' ', envActive.cpACM) },
    strRelA64{ StrAppend(strA64, ' ', envActive.cpACB) },
    strRelA32{ StrAppend(strA32, ' ', envActive.cpACB) },
    strC{ StrFormat("$ $ $ ",
      envActive.cpCCX, envActive.cpCCM, envActive.cpCCLIB) },
    strL{ "LIB.EXE -nologo -ltcg" },
    strL32{ StrAppend(strL, " -machine:X86") },
    strL64{ StrAppend(strL, " -machine:X64") },
    strRelFlags{
      StrFormat("$ -DNDEBUG -D_NDEBUG $ -Zl", strC, envActive.cpCCAB) },
    strCMakeBase{
      StrAppend(envActive.cpCMake, " "
        "-DCMAKE_BUILD_TYPE=Release "
        "-DCMAKE_C_COMPILER_WORKS=1 "   // Fix for Wine+NMake issue
        "-DCMAKE_CXX_COMPILER_WORKS=1 " // Fix for Wine+NMake issue
        "-DCMAKE_VERBOSE_MAKEFILE:BOOL=ON "
        "-DENABLE_SHARED=FALSE "
        "-DENABLE_STATIC=TRUE "
        "-DFORCE_STATIC_VCRT=ON "
        "-DLIBTYPE=STATIC "
        "-Wno-dev") },
    strCMake{ StrAppend(strCMakeBase, " .") },
    strNMake{ "NMAKE.EXE /nologo" },
    strRel64Extra{ envActive.cpCC8 },
    strRel32Extra{ envActive.cpCC4 };
  string strRelFlags64{ StrFormat("$ $ ", strRelFlags, strRel64Extra) },
         strRelFlags32{ StrFormat("$ $ ", strRelFlags, strRel32Extra) };
  // = OPENSSL SCRIPT (TOO BIG TO DO MANUALLY!) ===============================
  if(strLib.length() >= 8 && strLib.substr(0, 8) == "openssl-")
  { // Mandatory directory replacements
#define STRMANDATORY \
      { "\\\\lib\\\\engines-3",    cCommon->CommonCBlank() },\
      { "///",                     cCommon->CommonCBlank() },\
      { "\\\\lib\\\\ossl-modules", cCommon->CommonCBlank() },\
      { "  -del /Q /F doc\\",      "# !doc\\"              },\
      { ".h\r\n\t-del /Q /F",      ".h"                    },\
      { ".c\r\n\t-del /Q /F",      ".c"                    },\
      { ".asm\r\n\t-del /Q /F",    ".asm"                  },\
      { ".pl\r\n\t-del /Q /F",     ".pl"                   },\
      { ".pm\r\n\t-del /Q /F",     ".pm"                   }
    // 64-bit directory replacements
#define STRBASE64 \
      { "C:\\\\Program Files\\\\Common Files\\\\SSL",\
        cCommon->CommonCBlank() },\
      { "C:\\\\Program Files\\\\OpenSSL",\
        cCommon->CommonCBlank() },\
      { "C:\\\\Program Files\\\\SSL",\
        cCommon->CommonCBlank() },\
      { "\\Program Files\\OpenSSL",\
        cCommon->CommonCBlank() },\
      { "\\Program Files\\Common Files\\SSL",\
        cCommon->CommonCBlank() },\
      { "\\Program Files\\OpenSSL\\lib\\engines-1_1",\
        cCommon->CommonCBlank() }
    // 32-bit directory replacements
#define STRBASE32 \
      { "C:\\\\Program Files (x86)\\\\Common Files\\\\SSL",\
        cCommon->CommonCBlank() },\
      { "C:\\\\Program Files (x86)\\\\OpenSSL",\
        cCommon->CommonCBlank() },\
      { "C:\\\\Program Files (x86)\\\\SSL",\
        cCommon->CommonCBlank() },\
      { "\\Program Files (x86)\\OpenSSL",\
        cCommon->CommonCBlank() },\
      { "\\Program Files (x86)\\Common Files\\SSL",\
        cCommon->CommonCBlank() },\
      { "\\Program Files (x86)\\OpenSSL\\lib\\engines-1_1",\
        cCommon->CommonCBlank() }
    // Release mode replacement flags
#define STRRELEASE \
      { "/Zs",                    cCommon->CommonCBlank() },\
      { "/Gs0",                   "/GS-"                  },\
      { "/Zi /Fdossl_static.pdb", cCommon->CommonCBlank() },\
      { "/debug",                 cCommon->CommonCBlank() },\
      { "/dll",                   cCommon->CommonCBlank() },\
      { "-D\"NDEBUG\"",           "-DNDEBUG"              } \
    // Actual merged flags
#define STRREPRELEASE64 { STRMANDATORY, STRBASE64, STRRELEASE,\
      { "/MD /O2", StrFormat("/MT /O2 $", strRel64Extra) } }
#define STRREPRELEASE32 { STRMANDATORY, STRBASE32, STRRELEASE,\
      { "/MD /O2", StrFormat("/MT /O2 $", strRel32Extra) } }
#define STRREPCLANG { "CC=\"cl\"",   "CC=CLANG-CL" }, \
                        { "LD=\"link\"", "LD=LLD-LINK" }
#define STRREPCLANG64 { STRREPCLANG, { "/MT", "-m64 /MT" } }
#define STRREPCLANG32 { STRREPCLANG, { "/MT", "-m32 /MT" } }
    const string strInstallDataPm{
      "package OpenSSL::safe::installdata;\n"
      "\n"
      "use strict;\n"
      "use warnings;\n"
      "use Exporter;\n"
      "our @ISA = qw(Exporter);\n"
      "our @EXPORT = qw(\n"
      "    @PREFIX\n"
      "    @libdir\n"
      "    @BINDIR @BINDIR_REL_PREFIX\n"
      "    @LIBDIR @LIBDIR_REL_PREFIX\n"
      "    @INCLUDEDIR @INCLUDEDIR_REL_PREFIX\n"
      "    @APPLINKDIR @APPLINKDIR_REL_PREFIX\n"
      "    @ENGINESDIR @ENGINESDIR_REL_LIBDIR\n"
      "    @MODULESDIR @MODULESDIR_REL_LIBDIR\n"
      "    @PKGCONFIGDIR @PKGCONFIGDIR_REL_LIBDIR\n"
      "    @CMAKECONFIGDIR @CMAKECONFIGDIR_REL_LIBDIR\n"
      "    $VERSION @LDLIBS\n"
      ");\n"
      "\n"
      "our @PREFIX                     = ( '' );\n"
      "our @libdir                     = ( 'lib' );\n"
      "our @BINDIR                     = ( 'bin' );\n"
      "our @BINDIR_REL_PREFIX          = ( 'bin' );\n"
      "our @LIBDIR                     = ( 'lib' );\n"
      "our @LIBDIR_REL_PREFIX          = ( 'lib' );\n"
      "our @INCLUDEDIR                 = ( 'include' );\n"
      "our @INCLUDEDIR_REL_PREFIX      = ( 'include' );\n"
      "our @APPLINKDIR                 = ( 'include\\openssl' );\n"
      "our @APPLINKDIR_REL_PREFIX      = ( 'include/openssl' );\n"
      "our @ENGINESDIR                 = ( 'lib\\engines-3' );\n"
      "our @ENGINESDIR_REL_LIBDIR      = ( 'engines-3' );\n"
      "our @MODULESDIR                 = ( 'lib\\ossl-modules' );\n"
      "our @MODULESDIR_REL_LIBDIR      = ( 'ossl-modules' );\n"
      "our @PKGCONFIGDIR               = ( 'lib' );\n"
      "our @PKGCONFIGDIR_REL_LIBDIR    = ( '' );\n"
      "our @CMAKECONFIGDIR             = ( 'lib\\cmake\\OpenSSL' );\n"
      "our @CMAKECONFIGDIR_REL_LIBDIR  = ( 'cmake\\OpenSSL' );\n"
      "our $VERSION                    = '3.4.0';\n"
      "our @LDLIBS                     =\n"
      "    # Unix and Windows use space separation, "
        "VMS uses comma separation\n"
      "    $^O eq 'VMS'\n"
      "    ? split(/ *, */, 'ws2_32.lib gdi32.lib advapi32.lib crypt32.lib "
        "user32.lib ')\n"
      "    : split(/ +/, 'ws2_32.lib gdi32.lib advapi32.lib crypt32.lib "
        "user32.lib ');\n"
      "\n"
      "1;\n" };
    // Setup the repo
    SetupTarRepo(strLibPath, strTmp, PSLib.strFile, PSLibR.strFile);
    // Perl configure parameters
    const string strPerlOpts{ "no-aria no-autoload-config no-bf no-camellia "
      "no-cast no-cms no-comp no-deprecated no-dso no-dtls1 no-gost no-idea "
      "no-makedepend no-md4 no-nextprotoneg no-ocb no-psk no-rc2 no-rc4 "
      "no-rc5 no-seed no-shared no-sm4 no-srp no-srp no-srtp no-srtp no-ssl "
      "no-ssl3 no-stdio no-tests no-tls1 no-tls1_1 no-ts no-ui-console "
      "no-unit-test no-weak-ssl-ciphers no-whirlpool no-zlib "
      "disable-capieng" },
    strPerl64{ StrAppend(envActive.cpPerl, // 64-bit XP minimum
      " configure VC-WIN64A -D_WIN32_WINNT=0x0502 ", strPerlOpts) },
    strPerl64Rel{ StrAppend(strPerl64, " no-filenames") },
    strPerl32{ StrAppend(envActive.cpPerl, // 32-bit XP minimum
      " configure VC-WIN32 -D_WIN32_WINNT=0x0501 ", strPerlOpts) },
    strPerl32Rel{ StrAppend(strPerl32, " no-filenames") };
    // Compile 64-bit release version
    if(uiFlags & PF_X64)
    { // Do compile 64-bit release version
      System(strPerl64Rel);
      FStream{ "installdata.pm", FM_W_B }.FStreamWriteString(strInstallDataPm);
      ReplaceTextMulti("makefile", STRREPRELEASE64);
      if(envActive.cpCCX == envWindowsLLVMcompat.cpCCX)
        ReplaceTextMulti("makefile", STRREPCLANG64);
      System(strNMake);
      SystemF("$ libcrypto.lib libssl.lib -out:\"$/ssl64.lib\"",
        strL64, strTmp);
      System("cp -rf include/openssl include/win64");
      System("mv -f include/win64/configuration.h "
                   "include/win64/configuration-w64.h");
      SystemF("$ clean", strNMake);
      System("rm -f makefile configdata.pm include/win64/*.in");
    } // Compile 32-bit release version
    if(uiFlags & PF_X86)
    { // Do compile 32-bit release version
      System(strPerl32Rel);
      FStream{ "installdata.pm", FM_W_B }.FStreamWriteString(strInstallDataPm);
      ReplaceTextMulti("makefile", STRREPRELEASE32);
      if(envActive.cpCCX == envWindowsLLVMcompat.cpCCX)
        ReplaceTextMulti("makefile", STRREPCLANG32);
      System(strNMake);
      SystemF("$ libcrypto.lib libssl.lib -out:\"$/ssl32.lib\"",
        strL32, strTmp);
      System("cp -rf include/openssl include/win32");
      System("mv -f include/win32/configuration.h "
                   "include/win32/configuration-w32.h");
      SystemF("$ clean", strNMake);
      System("rm -f makefile configdata.pm include/win32/*.in");
      System("swap");
    } // Done with helper macros
#undef STRREPRELEASE64
#undef STRREPRELEASE32
#undef STRRELEASE
#undef STRBASE64
#undef STRBASE32
#undef STRMANDATORY
  } // = LIBJPEGTURBO SCRIPT ==================================================
  else if(strLib.length() >= 14 && strLib.substr(0, 14) == "libjpeg-turbo-")
  { // Setup repo
    SetupTarRepo(strLibPath, strTmp, PSLib.strFile, PSLibR.strFile);
    // Directories
    const string strBase32{ "simd/i386" }, strBase64{ "simd/x86_64" },
    // NASM Assembler flags
    strNASMRel32{
      "nasm -fwin32 -Isimd/nasm/ -I$/ -Iwin/ -DWIN32" },
    strNASMRel64{
      "nasm -fwin64 -Isimd/nasm/ -I$/ -Iwin/ -DWIN64 -D__x86_64__" },
    // Compilations
    strCompRel32{
      StrAppend(StrFormat(strNASMRel32, strBase32), " simd/i386") },
    strCompRel64{
      StrAppend(StrFormat(strNASMRel64, strBase64), " simd/x86_64") },
    // Add jpegturbo specific flags
    strJPTSpecific{ "-I. "
                    "-DWIN32 "
                    "-D_WINDOWS "
                    "-D_CRT_SECURE_NO_DEPRECATE "
                    "-D_CRT_SECURE_NO_WARNINGS "
                    "-D_CRT_NONSTDC_NO_WARNINGS" },
    // 12-bit standard modules
    strJP12{ "-DBITS_IN_JSAMPLE=12 "
              "jcapistd.c " "jccoefct.c " "jccolor.c " "jcdctmgr.c "
              "jcdiffct.c " "jclossls.c " "jcmainct.c " "jcprepct.c "
              "jcsample.c " "jdapistd.c " "jdcoefct.c " "jdcolor.c "
              "jddctmgr.c " "jddiffct.c " "jdlossls.c " "jdmainct.c "
              "jdmerge.c "  "jdpostct.c " "jdsample.c " "jfdctfst.c "
              "jfdctint.c " "jidctflt.c " "jidctfst.c " "jidctint.c "
              "jidctred.c " "jquant1.c "  "jquant2.c "
              "jutils.c" },
    // Lossless standard modules
    strJP16{ "-DBITS_IN_JSAMPLE=16 "
              "jcapistd.c " "jccolor.c "  "jcdiffct.c " "jclossls.c "
              "jcmainct.c " "jcprepct.c " "jcsample.c " "jdapistd.c "
              "jdcolor.c "  "jddiffct.c " "jdlossls.c " "jdmainct.c "
              "jdpostct.c " "jdsample.c " "jutils.c " },
    // 12-bit turbo modules
    strJPT12{ "-DBITS_IN_JSAMPLE=12 -DPPM_SUPPORTED "
              "rdppm.c " "wrppm.c" },
    // Lossless turbo modules
    strJPT16{ "-DBITS_IN_JSAMPLE=16 -DPPM_SUPPORTED "
              "rdppm.c " "wrppm.c" },
    // Main modules
    strJPT{ "-DBMP_SUPPORTED -DPPM_SUPPORTED "
            "jcapistd.c "    "jccolor.c "   "jcdiffct.c " "jclossls.c "
            "jcmainct.c "    "jcprepct.c "  "jcsample.c " "jdapistd.c "
            "jdcolor.c "     "jddiffct.c "  "jdlossls.c " "jdmainct.c "
            "jdpostct.c "    "jdsample.c "  "jutils.c "   "jccoefct.c "
            "jcdctmgr.c "    "jdcoefct.c "  "jddctmgr.c " "jdmerge.c "
            "jfdctfst.c "    "jfdctint.c "  "jidctflt.c " "jidctfst.c "
            "jidctint.c "    "jidctred.c "  "jquant1.c "  "jquant2.c "
            "jcapimin.c "    "jchuff.c "    "jcicc.c "    "jcinit.c "
            "jclhuff.c "     "jcmarker.c "  "jcmaster.c " "jcomapi.c "
            "jcparam.c "     "jcphuff.c "   "jctrans.c "  "jdapimin.c "
            "jdatadst.c "    "jdatasrc.c "  "jdhuff.c "   "jdicc.c "
            "jdinput.c "     "jdlhuff.c "   "jdmarker.c " "jdmaster.c "
            "jdphuff.c "     "jdtrans.c "   "jerror.c "   "jfdctflt.c "
            "jmemmgr.c "     "jmemnobs.c "  "jaricom.c "  "jcarith.c "
            "jdarith.c "     "turbojpeg.c " "transupp.c " "jdatadst-tj.c "
            "jdatasrc-tj.c " "rdbmp.c "     "rdppm.c "    "wrbmp.c "
            "wrppm.c" },
    strJP12Name{ "12jp" },
    strJP16Name{ "16jp" },
    strJPT12Name{ "16jpt" },
    strJPT16Name{ "16jpt" },
    // 64-bit SIMD assembler modules. Note that some .asm files (not in this
    // list) are used for '%include'ing only.
    straSIMD64[]{
      { "jccolor-avx2"  }, { "jccolor-sse2"  }, { "jcgray-avx2"   },
      { "jcgray-sse2"   }, { "jchuff-sse2"   }, { "jcphuff-sse2"  },
      { "jcsample-avx2" }, { "jcsample-sse2" }, { "jdcolor-avx2"  },
      { "jdcolor-sse2"  }, { "jdmerge-avx2"  }, { "jdmerge-sse2"  },
      { "jdsample-avx2" }, { "jdsample-sse2" }, { "jfdctflt-sse"  },
      { "jfdctfst-sse2" }, { "jfdctint-avx2" }, { "jfdctint-sse2" },
      { "jidctflt-sse2" }, { "jidctfst-sse2" }, { "jidctint-avx2" },
      { "jidctint-sse2" }, { "jidctred-sse2" }, { "jquantf-sse2"  },
      { "jquanti-avx2"  }, { "jquanti-sse2"  }, { "jsimdcpu"      }
    },
    // 32-bit SIMD assembler modules
    straSIMD32[]{
      { "jccolor-avx2"  }, { "jccolor-mmx"   }, { "jccolor-sse2"  },
      { "jcgray-avx2"   }, { "jcgray-mmx"    }, { "jcgray-sse2"   },
      { "jchuff-sse2"   }, { "jcphuff-sse2"  }, { "jcsample-avx2" },
      { "jcsample-mmx"  }, { "jcsample-sse2" }, { "jdcolor-avx2"  },
      { "jdcolor-mmx"   }, { "jdcolor-sse2"  }, { "jdmerge-avx2"  },
      { "jdmerge-mmx"   }, { "jdmerge-sse2"  }, { "jdsample-avx2" },
      { "jdsample-mmx"  }, { "jdsample-sse2" }, { "jfdctflt-3dn"  },
      { "jfdctflt-sse"  }, { "jfdctfst-mmx"  }, { "jfdctfst-sse2" },
      { "jfdctint-avx2" }, { "jfdctint-mmx"  }, { "jfdctint-sse2" },
      { "jidctflt-3dn"  }, { "jidctflt-sse"  }, { "jidctflt-sse2" },
      { "jidctfst-mmx"  }, { "jidctfst-sse2" }, { "jidctint-avx2" },
      { "jidctint-mmx"  }, { "jidctint-sse2" }, { "jidctred-mmx"  },
      { "jidctred-sse2" }, { "jquant-3dn"    }, { "jquant-mmx"    },
      { "jquant-sse"    }, { "jquantf-sse2"  }, { "jquanti-avx2"  },
      { "jquanti-sse2"  }, { "jsimdcpu"      }
    };
    // We need to activate cmake once to init jpegturbo config and other things
    SystemF("$ "
            "-DINLINE_WORKS=1 "
            "-DHAVE_THREAD_LOCAL=1 "
            "-DCMAKE_SIZEOF_VOID_P=8 " // CMake can't detect bits on Wine LOL
            "-DSIZE_T=8", strCMake);   // CMake can't detect bits on Wine LOL
    // Directories to keep objs safe from being renamed
    MakeDirectory(strJP12Name);
    MakeDirectory(strJP16Name);
    MakeDirectory(strJPT12Name);
    // Clean up existing object files
    DoClean({ envActive.cpOBJ,
      StrAppend(strJP12Name, '/', envActive.cpOBJ),
      StrAppend(strJP16Name, '/', envActive.cpOBJ),
      StrAppend(strJPT12Name, '/', envActive.cpOBJ)
    });
    // Compile 64-bit release version
    {
      SystemF("$ $ $", strRelFlags64, strJPTSpecific, strJP12);
      const Dir dJP12{ cCommon->CommonPeriod(), envActive.cpOBJ };
      for(const DirEntMapPair &dempPair : dJP12.GetFiles())
        RenameFileSafe(dempPair.first,
          StrFormat("$/$-$", strJP12Name, strJP12Name, dempPair.first));
      SystemF("$ $ $", strRelFlags64, strJPTSpecific, strJP16);
      const Dir dJP16{ cCommon->CommonPeriod(), envActive.cpOBJ };
      for(const DirEntMapPair &dempPair : dJP16.GetFiles())
        RenameFileSafe(dempPair.first,
          StrFormat("$/$-$", strJP16Name, strJP16Name, dempPair.first));
      SystemF("$ $ $", strRelFlags64, strJPTSpecific, strJPT12);
      const Dir dJPT12{ cCommon->CommonPeriod(), envActive.cpOBJ };
      for(const DirEntMapPair &dempPair : dJPT12.GetFiles())
        RenameFileSafe(dempPair.first,
          StrFormat("$/$-$", strJPT12Name, strJPT12Name, dempPair.first));
      SystemF("$ $ $", strRelFlags64, strJPTSpecific, strJPT16);
      const Dir dJPT16{ cCommon->CommonPeriod(), envActive.cpOBJ };
      for(const DirEntMapPair &dempPair : dJPT12.GetFiles())
        RenameFileSafe(dempPair.first,
          StrFormat("$-$", strJPT16Name, dempPair.first));
      for(const DirEntMapPair &dempPair : dJP12.GetFiles())
        RenameFileSafe(StrFormat("$/$-$",
            strJP12Name, strJP12Name, dempPair.first),
          StrFormat("$-$", strJP12Name, dempPair.first));
      for(const DirEntMapPair &dempPair : dJP16.GetFiles())
        RenameFileSafe(
          StrFormat("$/$-$", strJP16Name, strJP16Name, dempPair.first),
          StrFormat("$-$", strJP16Name, dempPair.first));
      for(const DirEntMapPair &dempPair : dJPT12.GetFiles())
        RenameFileSafe(
          StrFormat("$/$-$", strJPT12Name, strJPT12Name, dempPair.first),
          StrFormat("$-$", strJPT12Name, dempPair.first));
      for(const string &strFile : straSIMD64)
        SystemF("$/$.asm -o $$",
          strCompRel64, strFile, strFile, envActive.cpOBJ);
      GenericExtLibBuild(StrFormat("$ $ $ $/jsimd.c", strRelFlags64,
        strJPTSpecific, strJPT, strBase64), strL64, strTmp, "jpeg64", true);
    }
    // We need to activate cmake once to init jpegturbo config and other things
    System("rm -rf CMakeFiles *.cmake CMakeCache.txt jconfig.h");
    SystemF("$ "
            "-DINLINE_WORKS=1 "
            "-DHAVE_THREAD_LOCAL=1 "
            "-DCMAKE_SIZEOF_VOID_P=4 " // CMake can't detect bits on Wine LOL
            "-DSIZE_T=4", strCMake);   // CMake can't detect bits on Wine LOL
    // Compile 32-bit release version
    {
      SystemF("$ $ $", strRelFlags32, strJPTSpecific, strJP12);
      const Dir dJP12{ cCommon->CommonPeriod(), envActive.cpOBJ };
      for(const DirEntMapPair &dempPair : dJP12.GetFiles())
        RenameFileSafe(dempPair.first,
          StrFormat("$/$-$", strJP12Name, strJP12Name, dempPair.first));
      SystemF("$ $ $", strRelFlags32, strJPTSpecific, strJP16);
      const Dir dJP16{ cCommon->CommonPeriod(), envActive.cpOBJ };
      for(const DirEntMapPair &dempPair : dJP16.GetFiles())
        RenameFileSafe(dempPair.first,
          StrFormat("$/$-$", strJP16Name, strJP16Name, dempPair.first));
      SystemF("$ $ $", strRelFlags32, strJPTSpecific, strJPT12);
      const Dir dJPT12{ cCommon->CommonPeriod(), envActive.cpOBJ };
      for(const DirEntMapPair &dempPair : dJPT12.GetFiles())
        RenameFileSafe(dempPair.first,
          StrFormat("$/$-$", strJPT12Name, strJPT12Name, dempPair.first));
      SystemF("$ $ $", strRelFlags32, strJPTSpecific, strJPT16);
      const Dir dJPT16{ cCommon->CommonPeriod(), envActive.cpOBJ };
      for(const DirEntMapPair &dempPair : dJPT16.GetFiles())
        RenameFileSafe(dempPair.first,
          StrFormat("$-$", strJPT16Name, dempPair.first));
      for(const DirEntMapPair &dempPair : dJP12.GetFiles())
        RenameFileSafe(
          StrFormat("$/$-$", strJP12Name, strJP12Name, dempPair.first),
          StrFormat("$-$", strJP12Name, dempPair.first));
      for(const DirEntMapPair &dempPair : dJP16.GetFiles())
        RenameFileSafe(
          StrFormat("$/$-$", strJP16Name, strJP16Name, dempPair.first),
          StrFormat("$-$", strJP16Name, dempPair.first));
      for(const DirEntMapPair &dempPair : dJPT12.GetFiles())
        RenameFileSafe(
          StrFormat("$/$-$", strJPT12Name, strJPT12Name, dempPair.first),
          StrFormat("$-$", strJPT12Name, dempPair.first));
      for(const string &strFile : straSIMD32)
        SystemF("$/$.asm -o $$",
          strCompRel32, strFile, strFile, envActive.cpOBJ);
      GenericExtLibBuild(StrFormat("$ $ $ $/jsimd.c", strRelFlags32,
        strJPTSpecific, strJPT, strBase32), strL32, strTmp, "jpeg32", true);
    }
  } // = LIBPNG SCRIPT ========================================================
  else if(strLib.length() >= 7 && strLib.substr(0, 7) == "libpng-")
  { // Ignore if no vorbis supplemental argument
    if(strOpt2.empty())
      throw runtime_error{ "Missing extra zlib package argument." };
    // We have it, make sure it's valid
    if(strLibX.length() < 5 || strLibX.substr(0, 5) != "zlib-")
      throw runtime_error{ "Extra argument must be a zlib package." };
    // Setup second archive first then the first archive
    SetupTarRepoNSD(strLibPathX, strTmp, PSLibX.strFile);
    SetupTarRepo(strLibPath, strTmp, PSLib.strFile, PSLibR.strFile);
    // Remove test files, we don't want them nin the lib
    System("rm -rf pngtest.*");
    // Copy config file over
    System("cp -f scripts/pnglibconf.h.prebuilt pnglibconf.h");
    // Add png specific flags
    const string strPNGSpecific(StrFormat(
      "-I\"$/$\" -D_CRT_SECURE_NO_DEPRECATE "
      "-D_CRT_SECURE_NO_WARNINGS *.c", strTmp, PSLibXR.strFile));
    strRelFlags64 += strPNGSpecific,
    strRelFlags32 += strPNGSpecific;
    // Compile sources
    GenericExtLibBuildAll(strRelFlags64, strRelFlags32,
      strL64, strL32, strTmp, "png");
  } // = OPENALSOFT SCRIPT ====================================================
  else if(strLib.length() >= 12 && strLib.substr(0, 12) == "openal-soft-")
  { // Setup the repository
    SetupZipRepo(strLibPath, strTmp, PSLib.strFile);
    // Get fmt directory version
    string strFmtDir;
    { const Dir dFiles;
      for(const DirEntMapPair &dempPair : dFiles.GetDirs())
        if(dempPair.first.substr(0, 4) == "fmt-")
          strFmtDir = StdMove(dempPair.first); }
    // We need to activate cmake once to build openal config and other things
    System("rm -rf CMakeFiles *.cmake CMakeCache.txt");
    // One time only build
    SystemF("$ -Wno-dev "
              "-DALSOFT_TESTS=OFF "
              "-DALSOFT_BACKEND_WAVE=FALSE "
              "-DALSOFT_BACKEND_WASAPI=FALSE "
              "-DALSOFT_DLOPEN=FALSE "
              "-DALSOFT_EMBED_HRTF_DATA=FALSE "
              "-DALSOFT_EXAMPLES=FALSE "
              "-DALSOFT_INSTALL_AMBDEC_PRESETS=FALSE "
              "-DALSOFT_INSTALL_CONFIG=FALSE "
              "-DALSOFT_INSTALL_EXAMPLES=FALSE "
              "-DALSOFT_INSTALL_HRTF_DATA=FALSE "
              "-DALSOFT_INSTALL_UTILS=FALSE "
              "-DALSOFT_INSTALL=FALSE "
              "-DALSOFT_NO_CONFIG_UTIL=TRUE "
              "-DALSOFT_REQUIRE_SDL2=FALSE "
              "-DALSOFT_REQUIRE_WASAPI=FALSE "
              "-DALSOFT_UPDATE_BUILD_VERSION=FALSE "
              "-DALSOFT_UTILS=FALSE "
              "-DSDL2_CORE_LIBRARY=FALSE "
              "-DSDL2_INCLUDE_DIR=FALSE .", strCMakeBase);
    // Apply header patches to conquer forcing of DLL exports
    ReplaceText(strFmtDir + "/include/fmt/base.h",
      "#    define FMT_API __declspec(dllimport)",
      "#    define FMT_API extern");
    ReplaceText("include/al/al.h", "#define AL_API __declspec(dllimport)",
                                   "#define AL_API extern");
    ReplaceText("include/al/alc.h", "#define ALC_API __declspec(dllimport)",
                                    "#define ALC_API extern");
    // Remove some sources we don't need
    System("rm -rf core/mixer/mixer_neon.cpp core/rtkit.* core/dbus_wrap.*");
    // Don't put messages in debugger
    ReplaceText("core/logging.cpp", "OutputDebugStringW(wstr.c_str());",
      "\n#ifdef _DEBUG\n"
      "  OutputDebugStringW(wstr.c_str());\n"
      "#endif");
    // Rename mixer_inc.cpp to h
    ReplaceText("core/mixer/mixer_c.cpp", "hrtf_inc.cpp", "hrtf_inc.h");
    ReplaceText("core/mixer/mixer_sse.cpp", "hrtf_inc.cpp", "hrtf_inc.h");
    ReplaceText("core/mixer/mixer_sse2.cpp", "hrtf_inc.cpp", "hrtf_inc.h");
    ReplaceText("alc/alconfig.cpp",
      "#if !defined(_GAMING_XBOX)", "#if 0");
    ReplaceText("core/helpers.cpp",
      "#if !ALSOFT_UWP && !defined(_GAMING_XBOX)", "#if 0");
    // ReplaceText("core/mixer/mixer_sse3.cpp", "hrtf_inc.cpp", "hrtf_inc.h");
    ReplaceText("core/mixer/mixer_sse41.cpp", "hrtf_inc.cpp", "hrtf_inc.h");
    // Remove existing files
    System("if exist alc/alc_* rm -rfv alc/alc_*");
    System("if exist al/effects/al_effect_* rm -rfv al/effects/al_effect_*");
    System("if exist alc/effects/alc_effect_* "
           "rm -rfv alc/effects/alc_effect_*");
    // We need to rename files to prevent .obj's overwriting each other
    { const Dir dALEffects("al/effects", ".cpp");
      for(const DirEntMapPair &dempPair : dALEffects.GetFiles())
        RenameFileSafe(StrAppend("al/effects/", dempPair.first),
                       StrAppend("al/effects/al_effect_", dempPair.first)); }
    { const Dir dALCEffects("alc/effects", ".cpp");
      for(const DirEntMapPair &dempPair : dALCEffects.GetFiles())
        RenameFileSafe(StrAppend("alc/effects/", dempPair.first),
                       StrAppend("alc/effects/alc_effect_", dempPair.first)); }
    // Rename some more filenames to prevent collision
    RenameFileSafe("alc/context.cpp", "alc/alc_context.cpp");
    RenameFileSafe("alc/device.cpp", "alc/alc_device.cpp");
    // Build hrtf table
    // MakeIncludeFromBin("hrtf/Default HRTF.mhr",
    //  "const uint8_t hrtf_default", "hrtf_default.h");
    // Add openal specific flags
    const string strALSpecific{
      "-std:c++20 -D_SILENCE_ALL_CXX20_DEPRECATION_WARNINGS "
      "-D_CRT_NONSTDC_NO_DEPRECATE -D_CRT_SECURE_NO_WARNINGS "
      "-D_LARGE_FILES -D_LARGEFILE_SOURCE -D_WIN32 -D_WINDOWS "
      "-DAL_ALEXT_PROTOTYPES -DAL_BUILD_LIBRARY -DAL_LIBTYPE_STATIC "
      "-DHAVE_STRUCT_TIMESPEC -DNOMINMAX -DRESTRICT=__restrict "
      "-Dstrcasecmp=_stricmp -Dstrncasecmp=_strnicmp -DWIN32 -EHsc -I. -Ialc "
      "-Icommon -Ihrtf -Iinclude -Iopenal32/include -I" +
      strFmtDir + "/include "
      // Files to compile
      "al/*.cpp "
      "al/eax/*.cpp "
      "al/effects/*.cpp "
      "alc/*.cpp "
      "alc/backends/base.cpp "
      "alc/backends/dsound.cpp "
      "alc/backends/loopback.cpp "
      "alc/backends/null.cpp "
      // "alc/backends/wasapi.cpp "
      "alc/backends/wave.cpp "
      "alc/backends/winmm.cpp "
      "alc/effects/*.cpp "
      "common/*.cpp "
      "core/*.cpp "
      "core/filters/*.cpp "
      "core/mixer/*.cpp " +
      strFmtDir + "/src/*.cc" };
    strRelFlags64 += "-D_WIN32_WINNT=0x0502 " + strALSpecific;
    strRelFlags32 += "-D_WIN32_WINNT=0x0501 " + strALSpecific;
    // Compile 64-bit version
    GenericExtLibBuildBits(strRelFlags64, strL64, strTmp, "al", 64);
    // Compile 32-bit release version
    ReplaceTextMulti("config.h", {
      { "#define HAVE_BITSCANFORWARD64_INTRINSIC",
        "#undef HAVE_BITSCANFORWARD64_INTRINSIC" },
      { "/* #undef HAVE_BITSCANFORWARD_INTRINSIC */",
        "#define HAVE_BITSCANFORWARD_INTRINSIC" }
    });
    // Compile 32-bit version
    GenericExtLibBuildBits(strRelFlags32, strL32, strTmp, "al", 32);
  } // = THEORA SCRIPT ========================================================
  else if(strLib.length() >= 10 && strLib.substr(0, 10) == "libtheora-")
  { // Ignore if no vorbis supplemental argument
    if(strOpt2.empty())
      throw runtime_error{ "Missing extra ogg package argument!" };
    // We have it, make sure it's valid
    if(strLibX.length() < 7 || strLibX.substr(0, 7) != "libogg-")
      throw runtime_error{ "Extra argument must be a libogg package!" };
    // Setup second archive first then the first archive
    SetupTarRepoNSD(strLibPathX, strTmp, PSLibX.strFile);
    SetupTarRepo(strLibPath, strTmp, PSLib.strFile, PSLibR.strFile);
    // Get destination directory
    const string strDir{ StrAppend(strTmp, '/', PSLibR.strFile) };
    // mv can fail if ogg directory already exists
    SystemF("rm -rf \"$/include/ogg\"", strDir);
    // Place ogg sources in lib vorbis
    SystemF("mv -f \"$/$/include/ogg\" \"$/include\"",
      strTmp, PSLibXR.strFile, strDir);
    // Remove ogg package
    SystemF("rm -rf \"$/$\"", strTmp, PSLibXR.strFile);
    // Set destination directory
    SetDirectory(strDir);
    // Remove unneeded sources
    System("rm -f lib/encapiwrapper.c lib/encode.c");
    // Add theora specific flags
    const string strTheoraSpecific(
      "-DWIN32 -D_MBCS -D_LIB -Iinclude -Iwin32 lib/*.c win32/*.c");
    strRelFlags64 += strTheoraSpecific, strRelFlags32 += strTheoraSpecific;
    // Compile sources
    GenericExtLibBuildAll(strRelFlags64, strRelFlags32,
      strL64, strL32, strTmp, "theora");
  } // = FREETYPE SCRIPT ======================================================
  else if(strLib.length() >= 9 && strLib.substr(0, 9) == "freetype-")
  { // Setup repository
    SetupTarRepo(strLibPath, strTmp, PSLib.strFile, PSLibR.strFile);
    // Make a build directory because FT cmake needs it
    System("if exist build rm -rf build");
    MakeAndSetDirectory("build");
    // We need to activate cmake once to build freetype config and other things
    SystemF("$ ..", strCMakeBase);
    // Go back to original base directory
    SetDirectory("..");
    // Add freetype specific flags
    const string strFTSpecific{
      "-Iinclude "                     "-Ibuild/include "
      "-Isrc/gzip "                    "-DFT2_BUILD_LIBRARY "
      "-DWIN32 "                       "-D_WINDOWS "
      "-D_CRT_SECURE_NO_WARNINGS "
      "src/autofit/autofit.c "         "src/base/ftbase.c "
      "src/base/ftbbox.c "             "src/base/ftbdf.c "
      "src/base/ftbitmap.c "           "src/base/ftcid.c "
                                       "src/base/ftfstype.c "
      "src/base/ftgasp.c "             "src/base/ftglyph.c "
      "src/base/ftgxval.c "            "src/base/ftinit.c "
                                       "src/base/ftmm.c "
      "src/base/ftotval.c "            "src/base/ftpatent.c "
      "src/base/ftpfr.c "              "src/base/ftstroke.c "
      "src/base/ftsynth.c "            "src/base/ftsystem.c "
      "src/base/fttype1.c "            "src/base/ftwinfnt.c "
      "src/bdf/bdf.c "                 "src/bzip2/ftbzip2.c "
      "src/cache/ftcache.c "           "src/cff/cff.c "
      "src/cid/type1cid.c "            "src/gzip/ftgzip.c "
      "src/lzw/ftlzw.c "               "src/pcf/pcf.c "
      "src/pfr/pfr.c "                 "src/psaux/psaux.c "
      "src/pshinter/pshinter.c "       "src/psnames/psnames.c "
      "src/raster/raster.c "           "src/sfnt/sfnt.c "
      "src/sdf/sdf.c "                 "src/svg/ftsvg.c "
      "src/smooth/smooth.c "           "src/truetype/truetype.c "
      "src/type1/type1.c "             "src/type42/type42.c "
      "src/winfonts/winfnt.c "         "src/base/ftdebug.c" };
    strRelFlags64 += strFTSpecific, strRelFlags32 += strFTSpecific;
    // Hack to force use our zlib since cmake doesn't listen anymore ----------
    ReplaceText("include/freetype/config/ftoption.h",
      "/* #define FT_CONFIG_OPTION_SYSTEM_ZLIB */",
      "#define FT_CONFIG_OPTION_SYSTEM_ZLIB");
    // Compile sources
    GenericExtLibBuildAll(strRelFlags64, strRelFlags32,
      strL64, strL32, strTmp, "ft");
    // Perform modification of headers
    System("mv -f build/include/freetype/config/*.h include/freetype/config");
    System("mv -f include/*.h include/freetype");
    // We don't want to use freetypes default directory. We handle it.
    const char*const cpDirs[2] = { "include/freetype",
                                   "include/freetype/config" };
    for(const char*const cpDir : cpDirs)
    { // Get all header files and replace all occurences
      const Dir dFiles(cpDir, ".h");
      for(const DirEntMapPair &dempPair : dFiles.GetFiles())
        ReplaceText(StrAppend(cpDir, '/', dempPair.first), "<freetype/", "<");
    }
  } // = GLFW SCRIPT ==========================================================
  else if(strLib.length() >= 5 && strLib.substr(0, 5) == "glfw-")
  { // Extract zip to the temporary directory
    SetupZipRepo(strLibPath, strTmp, PSLib.strFile);
    // We need to activate cmake once to build glfw_config.h and other things
    SystemF("$", strCMake);
    // Remove unneeded sources
    System("rm -rf src/cocoa* src/glx* src/linux* src/mir* "
                  "src/posix* src/x* src/wl*");
    // Build list of files to compile
    const Dir dFiles{ "src", ".c" };
    string strFiles;
    for(const auto &dFile : dFiles.GetFiles())
      strFiles += StrAppend(" src/", dFile.first);
    // Add glfw specific flags
    const string strGlfwSpecific{ StrAppend("-Iinclude "
      "-D_CRT_SECURE_NO_WARNINGS -D_GLFW_WIN32 -DWIN32 -D_WINDOWS$",
      strFiles) };
    strRelFlags64 += StrAppend("-DWINVER=0x0502 -D_WIN32_WINNT=0x0502 ",
      strGlfwSpecific),
    strRelFlags32 += StrAppend("-DWINVER=0x0501 -D_WIN32_WINNT=0x0501 ",
      strGlfwSpecific),
    // Using non-XP functions in 3.4.0 for some reason when XP still supported
    ReplaceTextMulti("src/win32_thread.c", {
      { "GLFWbool _glfwPlatformCreateCondVar(_GLFWcondvar* condvar)", "/*" },
      { "#endif // GLFW_BUILD_WIN32_THREAD", "*/\n#endif" },
    });
    // They are forcing use of ansi when asked not in 3.4.0
    ReplaceText("src/internal.h",
      "glfwPlatformLoadModule(const char* path)",
      "glfwPlatformLoadModule(const wchar_t* path)");
    ReplaceTextMulti("src/win32_module.c", {
      { "glfwPlatformLoadModule(const char* path)",
        "glfwPlatformLoadModule(const wchar_t* path)" },
      { "return LoadLibraryA(path);", "return LoadLibraryW(path);" }
    });
    ReplaceTextMulti("src/egl_context.c", {
      { "strncmp(sonames[i], \"lib\", 3)",
        "wcsncmp(sonames[i], L\"lib\", 3)" },
      { "_glfwPlatformLoadModule(\"", "_glfwPlatformLoadModule(L\"" },
      { "const char* sonames[] =", "const wchar_t* sonames[] =" },
      { "\"libEGL.dll", "L\"libEGL.dll" },
      { "\"EGL.dll", "L\"EGL.dll" },
      { "\"GLESv1_CM.dll", "L\"GLESv1_CM.dll" },
      { "\"libGLES_CM.dll", "L\"libGLES_CM.dll" },
      { "\"GLESv2.dll", "L\"GLESv2.dll" },
      { "\"libGLESv2.dll", "L\"libGLESv2.dll" },
      { "const char** sonames;", "const wchar_t** sonames;" },
      { "const char* es1sonames[] =", "const wchar_t* es1sonames[] =" },
      { "const char* es2sonames[] =", "const wchar_t* es2sonames[] =" },
      { "const char* glsonames[] =", "const wchar_t* glsonames[] =" },
    });
    ReplaceTextMulti("src/osmesa_context.c",{
      { "_glfwPlatformLoadModule(\"", "_glfwPlatformLoadModule(L\"" },
      { "const char* sonames[] =", "const wchar_t* sonames[] =" },
      { "\"libOSMesa.dll", "L\"libOSMesa.dll" },
      { "\"OSMesa.dll", "L\"OSMesa.dll" },
    });
    ReplaceText("src/vulkan.c",
      "_glfwPlatformLoadModule(\"", "_glfwPlatformLoadModule(L\"");
    ReplaceText("src/wgl_context.c",
      "_glfwPlatformLoadModule(\"", "_glfwPlatformLoadModule(L\"");
    ReplaceTextMulti("src/win32_init.c", {
      { "const char* names[]",  "const wchar_t* names[]" },
      { "            \"xinput", "            L\"xinput"  },
      { "_glfwPlatformLoadModule(\"", "_glfwPlatformLoadModule(L\"" },
    });
    // Compile sources
    GenericExtLibBuildAll(strRelFlags64, strRelFlags32,
      strL64, strL32, strTmp, "glfw");
  } // = LIBOGG/VORBIS SCRIPT =================================================
  else if(strLib.length() >= 7 && strLib.substr(0, 7) == "libogg-")
  { // Ignore if no vorbis supplemental argument
    if(strOpt2.empty())
      throw runtime_error{ "Missing extra vorbis package arg!" };
    // We have it, make sure it's valid
    if(strLibX.length() < 10 || strLibX.substr(0, 10) != "libvorbis-")
      throw runtime_error{ "Extra argument must be a libvorbis package." };
    // Extract second archive then first archive
    SetupTarRepoNSD(strLibPath, strTmp, PSLib.strFile);
    SetupTarRepo(strLibPathX, strTmp, PSLibX.strFile, PSLibXR.strFile);
    // Place ogg sources in lib vorbis
    SystemF("mv -f \"$/$/include/ogg\" \"$/$/include\"",
      strTmp, PSLibR.strFile, strTmp, PSLibXR.strFile);
    SystemF("mv -f \"$/$/src/*.c\" \"$/$/lib\"",
      strTmp, PSLibR.strFile, strTmp, PSLibXR.strFile);
    SystemF("mv -f \"$/$/src/*.h\" \"$/$/lib\"",
      strTmp, PSLibR.strFile, strTmp, PSLibXR.strFile);
    // Remove ogg package
    SystemF("rm -rf \"$/$\"", strTmp, PSLibR.strFile);
    // Remove unneeded sources
    System("rm -f lib/barkmel.c lib/tone.c lib/psytune.c");
    // Add ogg/vorbis specific flags
    const string strVorbisSpecific("-Iinclude lib/*.c");
    strRelFlags64 += strVorbisSpecific;
    strRelFlags32 += strVorbisSpecific;
    // Compile everything
    GenericExtLibBuildAll(strRelFlags64, strRelFlags32,
      strL64, strL32, strTmp, "ogg");
  } // = LIBNSGIF SCRIPT ======================================================
  else if(strLib.length() >= 9 && strLib.substr(0, 9) == "libnsgif-")
  { // Make sure it doesnt suffix in -src
    if(strLib.find("-src.") != StdNPos)
      throw runtime_error{ "Remove -src suffix from filename please!" };
    // Setup second archive first then the first archive
    SetupTarRepo(strLibPath, strTmp, PSLib.strFile, PSLibR.strFile);
    // Add nsgif specific flags
    const string strNSGSpecific("-Iinclude src/*.c");
    strRelFlags64 += strNSGSpecific;
    strRelFlags32 += strNSGSpecific;
    // Compile everything
    GenericExtLibBuildAll(strRelFlags64, strRelFlags32,
      strL64, strL32, strTmp, "nsgif");
  } // = SQLITE SCRIPT ========================================================
  else if(strLib.length() >= 20 &&
          strLib.substr(0, 20) == "sqlite-amalgamation-")
  { // Set destination temp directory
    SetupZipRepo(strLibPath, strTmp, PSLib.strFile);
    // Remove unneeded code
    System("if exist \"shell.c\" rm -rf \"shell.c\"");
    // Add sqlite specific flags
    const string strSQLiteSpecific(
      "-DSQLITE_DEFAULT_AUTOVACUUM=2 -DSQLITE_TEMP_STORE=2 "
      "-DSQLITE_ENABLE_NULL_TRIM -DSQLITE_OS_WINNT -DSQLITE_WIN32_NO_ANSI "
      "-DSQLITE_OMIT_LOAD_EXTENSION -DSQLITE_OMIT_DEPRECATED "
      "-DSQLITE_THREADSAFE -DSQLITE_ENABLE_MATH_FUNCTIONS *.c");
    strRelFlags64 += strSQLiteSpecific;
    strRelFlags32 += strSQLiteSpecific;
    // Compile 64-bit release version -----------------------------------------
    GenericExtLibBuildAll(strRelFlags64, strRelFlags32,
      strL64, strL32, strTmp, "sqlite");
  } // = XMP SCRIPT ===========================================================
  else if(strLib.length() >= 12 && strLib.substr(0, 12) == "libxmp-lite-")
  { // Setup the archive
    SetupTarRepo(strLibPath, strTmp, PSLib.strFile, PSLibR.strFile);
    // Compiler flags
    const string strXmpSpecific{ StrAppend(
      "-Iinclude/libxmp-lite "
      "-Isrc "
      "-DWIN32 "
      "-D_CRT_SECURE_NO_DEPRECATE "
      "-D_CRT_NONSTDC_NO_DEPRECATE "
      "-DHAVE_ALLOCA_H "
      "-DHAVE_FNMATCH "
      "-DHAVE_MKSTEMP "
      "-DHAVE_UMASK "
      "-DLIBXMP_CORE_PLAYER "
      "-DLIBXMP_NO_PROWIZARD "
      "-DLIBXMP_NO_DEPACKERS "
      "-DBUILDING_STATIC ",
      GetFiles(".c", "src"), ' ',
      GetFiles(".c", "src/loaders")) };
    strRelFlags64 += strXmpSpecific;
    strRelFlags32 += strXmpSpecific;
    // Compile everything
    GenericExtLibBuildAll(strRelFlags64, strRelFlags32,
      strL64, strL32, strTmp, "xmp");
  } // = ZLIB SCRIPT ==========================================================
  else if(strLib.length() >= 5 && strLib.substr(0, 5) == "zlib-")
  { // Setup the archive
    SetupTarRepo(strLibPath, strTmp, PSLib.strFile, PSLibR.strFile);
    // Compiler flags
    const string strZLibSpecific{ StrAppend(
      "-DWIN32 -D_CRT_SECURE_NO_DEPRECATE "
      "-D_CRT_NONSTDC_NO_DEPRECATE "
      "-I. ", GetFiles(".c")) };
    strRelFlags64 += strZLibSpecific;
    strRelFlags32 += strZLibSpecific;
    // Compile everything
    GenericExtLibBuildAll(strRelFlags64, strRelFlags32,
      strL64, strL32, strTmp, "zlib");
  } // = MINIMP3 SCRIPT =======================================================
  else if(strLib.length() >= 7 && strLib.substr(0, 7) == "minimp3")
  { // Setup second archive first then the first archive
    SetupTarRepo(strLibPath, strTmp, PSLib.strFile, PSLibR.strFile);
    // Remove un-needed source code
    System("rm -rf player*.c");
    // Perform a small compatibility fix to the code
    ReplaceTextMulti("minimp3.c", { { "#ifndef _MSC_VER", "#ifdef _WIN64" },
                                    { "mp3_decoder_t *", "mp3_decoder_t " } });
    ReplaceTextMulti("minimp3.h", { { "mp3_decoder_t *", "mp3_decoder_t " },
      { "#define __MINIMP3_H_INCLUDED__", "#define __MINIMP3_H_INCLUDED__\n"
                                          "#ifdef __cplusplus\n"
                                          "extern \"C\" { \n"
                                          "#endif" },
      { "#endif//__MINIMP3_H_INCLUDED__", "#endif//__MINIMP3_H_INCLUDED__\n"
                                          "#ifdef __cplusplus\n"
                                          "};\n"
                                          "#endif" } });
    // Set compiler flags
    const string strMMSpecific("*.c");
    strRelFlags64 += strMMSpecific;
    strRelFlags32 += strMMSpecific;
    // Compile everything
    GenericExtLibBuildAll(strRelFlags64, strRelFlags32,
      strL64, strL32, strTmp, "mpt");
  } // = LZMA SCRIPT ==========================================================
  else if(strLib.length() >= 4 && strLib.substr(0, 4) == "lzma")
  { // Set destination temp directory
    SetupZipRepo(strLibPath, strTmp, PSLib.strFile, true);
    // Remove read only flags on directory (sigh!)
    System("chmod -R a+w *");
    // Setup initial compiler flags and source files
    const string strLZMASpecificMandatory{
      "-D_7ZIP_ST "                    "-D_REENTRANT "
      "-D_FILE_OFFSET_BITS=64 "        "-D_LARGEFILE_SOURCE "
      "C/*.c" };
    // Rest flags
    strRelFlags64 += strLZMASpecificMandatory,
    strRelFlags32 += strLZMASpecificMandatory,
    // This disables use of AVX which isn't supported on wine
    ReplaceText("C/LzFind.c", "#define USE_SATUR_SUB_128", "");
    // Compile everything
    GenericExtLibBuildAll(strRelFlags64, strRelFlags32,
      strL64, strL32, strTmp, "lzma");
  } // = BZIP2 SCRIPT =========================================================
  else if(strLib.length() >= 6 && strLib.substr(0, 6) == "bzip2-")
  { // Setup second archive first then the first archive
    SetupTarRepo(strLibPath, strTmp, PSLib.strFile, PSLibR.strFile);
    // Delete some files we don't need to compile
    System("rm -rf dlltest.* bzip2.* spewg.* mk251.* unzcrash.*");
    // Add BZ2 specific flags to compiler command line
    const string strBZ2Sources("*.c");
    strRelFlags64 += strBZ2Sources;
    strRelFlags32 += strBZ2Sources;
    // Compile everything
    GenericExtLibBuildAll(strRelFlags64, strRelFlags32,
      strL64, strL32, strTmp, "bzip");
  } // = LUA SCRIPT ===========================================================
  else if(strLib.length() >= 4 && strLib.substr(0, 4) == "lua-")
  { // Extract the repository and switch to it --------------------------------
    SetupTarRepo(strLibPath, strTmp, PSLib.strFile, PSLibR.strFile);
    // Delete some files we don't need to compile -----------------------------
    System("rm -rf src/lua.c "    "src/luac.c "     "src/lbitlib.* "
                  "src/liolib.* " "src/lloadlib.* " "src/loslib.* "
                  "src/loadlib.*");
    // Patch makefile for linux/mac -------------------------------------------
    ReplaceTextMulti("src/makefile", {
      { "CC= gcc -std=gnu99", "CC=g++ -std=" STANDARD },
      { "-DLUA_COMPAT_5_3 ",  cCommon->CommonCBlank() },
      { "LUA_T=	lua",         "LUA_T=" },
      { "LUA_O=	lua.o",       "LUA_O=" },
      { "LUAC_T=	luac",      "LUAC_T=" },
      { "LUAC_O=	luac.o",    "LUAC_O=" },
      { " liolib.o",          cCommon->CommonCBlank() },
      { " loslib.o",          cCommon->CommonCBlank() },
      { " loadlib.o",         cCommon->CommonCBlank() },
    });
    // Performing removal of libs from LUA core lib ---------------------------
    ReplaceTextMulti("src/linit.c", {
      { "{LUA_IOLIBNAME, luaopen_io},",        cCommon->CommonCBlank() },
      { "{LUA_OSLIBNAME, luaopen_os},",        cCommon->CommonCBlank() },
      { "{LUA_LOADLIBNAME, luaopen_package},", cCommon->CommonCBlank() },
    });
    // Patch to replace the default throwing code with code that works better
    // to store the exception message so we don't have to have try blocks in
    // each C function
    ReplaceTextMulti("src/ldo.c", {
      { "throw(c)",
        "throw(c)\n#include <stdexcept>\n#include \"lauxlib.h\"" },
      { "L->errorJmp = &lj;",
        "L->errorJmp = &lj;\n"
        "#if defined(__cplusplus)\n"
        "\ttry{(*f)(L, ud);}\n"
        "\tcatch(struct lua_longjmp *lj){if(!lj->status) lj->status=-1;}\n"
        "\tcatch(const std::exception &e){\n"
        "\t\ttry{luaL_where(L,1);lua_pushstring(L,e.what());"
                "lua_concat(L,2);lua_error(L);}\n"
        "\t\tcatch(struct lua_longjmp *lj){if(!lj->status) lj->status=-1;}\n"
        "\t}\n#else\n" },
      { "L->errorJmp = lj.previous;",
        "#endif\n"
        "\tL->errorJmp = lj.previous;" }
    });
    // Performing removal of unneeded core function ---------------------------
    ReplaceTextMulti("src/lbaselib.c", {
      { "{\"dofile\", luaB_dofile},",     cCommon->CommonCBlank() },
      { "{\"loadfile\", luaB_loadfile},", cCommon->CommonCBlank() },
      { "{\"load\", luaB_load},",         cCommon->CommonCBlank() },
      { "{\"print\", luaB_print},",       cCommon->CommonCBlank() },
      { "{\"_VERSION\", nullptr},",       cCommon->CommonCBlank() },
      { "{\"_G\", nullptr},",             cCommon->CommonCBlank() },
    });
    // Perform increase of limits ---------------------------------------------
    ReplaceText("src/lparser.c", "MAXVARS\t\t200", "MAXVARS\t\t253");
    // Add lua specific flags to compiler command line ------------------------
    const string strLuaSpecific{ "-TP -EHsc" }, // DO NOT COMPILE AS C!!!
                 strLuaDebug{ "-DLUA_USE_APICHECK" },
                 strLuaSources{ "src/*.c" };
    strRelFlags64 += StrAppend(strLuaSpecific, ' ', strLuaSources),
    strRelFlags32 += StrAppend(strLuaSpecific, ' ', strLuaSources);
    // Compile everything
    GenericExtLibBuildAll(strRelFlags64, strRelFlags32,
      strL64, strL32, strTmp, "lua");
  } // Unrecognised archive filename
  else throw runtime_error{ "The archive is valid but unrecognised!" };
  // Done
  cout << "\nFinished without error!\n";
  // Done
  return 0;
}
/* ------------------------------------------------------------------------- */
static int CppCheck(void)
{ // Do exec, if succeeded no more code is executed
  SystemF("$ "                         "--inline-suppr "
    "--suppress=missingIncludeSystem " "--exception-handling "
    "--library=windows "               "--library=posix "
    "--library=std "                   "--library=zlib "
    "--library=sqlite3 "               "--library=openssl "
    "--library=lua "                   "--library=opengl "
    "--language=c++ "                  "--std=$ "
    "--enable=all $ $ "                "-D__cplusplus=202002 "
    "--disable=unusedFunction "
    "--report-progress "               "-DCPPCHECK "
    "-D$ "                             "\"$/$.cpp\"",
    envActive.cpCppCheck,
    STANDARD,
    envActive.cpCppChkM,
    (uiFlags & PF_X64) ? envActive.cpCppChk64 : envActive.cpCppChk32,
    (uiFlags & PF_ALPHA) ? "ALPHA -D_DEBUG" :
   ((uiFlags & PF_BETA) ? "BETA" :
   ((uiFlags & PF_FINAL) ? "RELEASE" : "UNKNOWN")),
       SRCDIR, ENGINENAME);
  // Success
  return 0;
}
/* ------------------------------------------------------------------------- */
#if defined(WINDOWS)
static void GotNewBuildExecutable(const string &strOldExe,
  const string &strNewExe)
{ // Wait for the compiled file to be readable and writable just incase
  while(!DirCheckFileAccess(strNewExe, 6)) StdSuspend(milliseconds{ 100 });
  // Open the new executable and if succeeded?
  if(FStream fsNewExe{ strNewExe, FM_R_B })
  { // Read the new executable and its checksum
    Memory mA{ fsNewExe.FStreamReadBlockSafe() };
    fsNewExe.FStreamClose();
    if(mA.MemIsEmpty())
      XCL("Failed to read new executable!", "File", strNewExe);
    const uint32_t uiNewExe = CryptToCRC32(mA);
    // Read the current executable and its checksum
    if(FStream fsExe{ strOldExe, FM_R_B })
    { // Read the executable and its checksum
      Memory mB{ fsExe.FStreamReadBlockSafe() };
      fsExe.FStreamClose();
      if(mA.MemIsEmpty())
        XCL("Failed to read old executable!", "File", strOldExe);
      const uint32_t uiExe = CryptToCRC32(mB);
      // If checksums match?
      if(uiExe == uiNewExe)
      { // Delete the 'new' executable (it isn't new!)
        if(!DirFileUnlink(strNewExe))
          XCL("Could not delete new executable!", "File", strOldExe);
        // Continue as normal
        return;
      } // If we are calling from an .exe extension? Run it to move it
      if(cSystem->ENGExt() == envActive.cpEXE)
      { // Convert new exe to widestring
        const wstring wstrNewExe{ UTFtoS16(strNewExe) };
        // Get environment and modify first parameter
        const ArgType *const lArgs[] = { wstrNewExe.c_str(), L"!", nullptr };
        const ArgType *const *lEnv = cCmdLine->CmdLineGetCEnv();
        // Execute the new program
        const int iCode = StdExecVE(lArgs, lEnv);
        XCL("Failed to execute program!",
            "Executable", strNewExe, "Code", iCode);
      } // Else if we're calling from the .x extension?
      if(cSystem->ENGExt() == ".x")
      { // Unlink the new executable
        while(!DirFileUnlink(strOldExe))
          StdSuspend(milliseconds{ 100 });
        // Move the executable over
        while(!DirFileRename(strNewExe, strOldExe))
          StdSuspend(milliseconds{ 100 });
        // Exit as normal if we were asked to or just return
        if(cCmdLine->CmdLineGetTotalCArgs() >= 2 &&
          !wcscmp(cCmdLine->CmdLineGetCArgs()[1], L"!")) exit(0);
        // Return as normal
        return;
      }
    } // Failed to open old executable
    else XCL("Failed to open old executable!", "File", strOldExe);
  } // Failed to open new executable
  else XCL("Failed to open new executable!", "File", strNewExe);
}
#endif
/* ------------------------------------------------------------------------- */
static void CheckForNewBuildExecutable(void)
{ // Build executable filename and temp filename
#if defined(WINDOWS)
  // Make filename to new executable in current directory and if we have it?
  // This will be apparent if using a 'bin' directory and a linked exe from
  // 'bin/..' executed 'bin/build.x'...
  const string strNewExe{ StrFormat("$$.x", cSystem->ENGLoc(),
    cSystem->ENGFile()) };
  if(DirLocalFileExists(strNewExe))
    return GotNewBuildExecutable(StrAppend(cSystem->ENGLoc(),
      cSystem->ENGFile(), envActive.cpEXE), strNewExe);
  // Make filename to new executable and return if it doesn't exist
  const string strNewExeAlt{ StrFormat("$$/$.x", cSystem->ENGLoc(), BINDIR,
    cSystem->ENGFile()) };
  if(DirLocalFileExists(strNewExeAlt))
    return GotNewBuildExecutable(StrFormat("$$/$$", cSystem->ENGLoc(),
      BINDIR, cSystem->ENGFile(), envActive.cpEXE), strNewExeAlt);
  // No new executable found
#endif
}
/* ------------------------------------------------------------------------- */
static void WriteVersion(void)
{ // Create the json object to write to
  using namespace IJson;
  Document dOut;
  Document::AllocatorType& dAlloc = dOut.GetAllocator();
  dOut.SetObject();
  // Add version number
  dOut.AddMember("Version", Value(1).Move(), dAlloc);
  // Add project name
  dOut.AddMember("Project", strName, dAlloc);
  // Build version array
  Value vVersion{ kArrayType };
  for(size_t stIndex = 0; stIndex < uiVer.size(); ++stIndex)
    vVersion.PushBack(Value{uiVer[stIndex]}.Move(), dAlloc);
  dOut.AddMember("Engine", vVersion, dAlloc);
  // Build stringified output buffer
  StringBuffer rsbOut;
  Writer<StringBuffer, UTF8<>, UTF8<>> pwWriter{ rsbOut };
  dOut.Accept(pwWriter);
  // Open version file and if opened?
  if(FStream{ strVerFile, FM_W_B }.
       FStreamWriteSafe(rsbOut.GetString(), rsbOut.GetSize()) !=
         rsbOut.GetSize())
    XCL("Cannot open version file for writing!", "File", strVerFile);
}
/* ------------------------------------------------------------------------- */
static void ReadVersion(void)
{ // Create the json object and parse it
  const Json jsManifest{ strVerFile };
  // Check version is correct
  const unsigned int uiVersionRequired = 1;
  const unsigned int uiVersion = jsManifest.GetInteger("Version");
  if(uiVersion != uiVersionRequired)
    XC("Invalid build manifest version!",
       "Manfiest", jsManifest.IdentGet(), "Required", uiVersionRequired,
       "Actual",   uiVersion);
  // Look for version information and throw error if there is none
  using Lib::RapidJson::Value;
  const Value &rjvConstants = jsManifest.GetValue("Engine");
  if(!rjvConstants.IsArray())
    XC("Constants array not valid!", "Manfiest", jsManifest.IdentGet());
  // Must have 4 values
  if(rjvConstants.Size() != uiVer.size())
    XC("Invalid array count!", "Manfiest", jsManifest.IdentGet(),
       "Expected", uiVer.size(), "Actual", rjvConstants.Size());
  for(unsigned int uiIndex = 0; uiIndex < uiVer.size(); ++uiIndex)
  { // Must be all integers
    const Value &rjvInteger = rjvConstants[uiIndex];
    if(!rjvInteger.IsInt())
      XC("Integer not valid!",
         "Manfiest", jsManifest.IdentGet(), "Index", uiIndex);
    // Set actual integer
    uiVer[uiIndex] = static_cast<unsigned int>(rjvInteger.GetInt());
  } // Set name of project
  strName = StdMove(jsManifest.GetString("Project"));
}
/* ------------------------------------------------------------------------- */
static int ChangeProject(const string &strProj)
{ // Make sure the directory specified is valid
  if(svusIgnore.contains(strProj) ||
     strProj.find('/') != StdNPos ||
     DirValidName(strProj) != VR_OK)
    XC("Invalid project name!", "Project", strProj);
  // Get directory name. Bail if project doesnt exist
  if(!DirLocalDirExists(strProj))
    XCL("No such project directory exists!", "Project", strProj);
  // Set the new project
  strName = strProj;
  // Write the new project
  WriteVersion();
  // Show success
  cout << "Switch project to '" << strProj << "' succeeded!" << endl;
  // Done
  return 0;
}
/* ------------------------------------------------------------------------- */
static int Compile(const bool bSelf)
{ // Storage for filenames and command lines
  string strCpp,
         strRc,
         strRes,
         strExe,
         strPdb,
         strObj,
         strAsm,
         strMap,
         strCmdCC,
         strCmdRC,
         strCmdLD,
         strDbgDir{ StrAppend(DBGDIR, envActive.cpDBGSUF) };
  // Have compiler?
  if(*envActive.cpCCX)
  { // Set compiler command line
    strCmdCC += StrAppend(envActive.cpCCX, ' ');
    // Add mandatory compiler command line parameters if set
    if(*envActive.cpCCM)
      strCmdCC += StrFormat("$ $ ", envActive.cpCCM,
        StrFormat(envActive.cpCCMX, STANDARD, INCDIR, INCDIR, INCDIR));
  }// Have linker?
  if(*envActive.cpRCX)
  { // Set resource compiler command line
    strCmdRC += StrAppend(envActive.cpRCX, ' ');
    // Add mandatory compiler command line parameters if set
    if(*envActive.cpRCM)
      strCmdRC += StrAppend(StrFormat(envActive.cpRCM, SRCDIR), ' ');
  } // Have 64-bit linker filename?
  if(uiFlags & PF_X64 && *envActive.cpLDX8)
  { // Add executable name
    strCmdLD += StrAppend(envActive.cpLDX8, ' ');
    // Add mandatory parameters
    if(*envActive.cpLDM) strCmdLD += StrAppend(envActive.cpLDM, ' ');
  } // Have 32-bit linker filename?
  else if(uiFlags & PF_X86 && *envActive.cpLDX4)
  { // Add executable name
    strCmdLD += StrAppend(envActive.cpLDX4, ' ');
    // Add mandatory parameters
    if(*envActive.cpLDM) strCmdLD += StrAppend(envActive.cpLDM, ' ');
  } // Compiling self?
  if(bSelf)
  { // Set filenames
    strCpp = StrFormat("$/$.cpp", SRCDIR, cSystem->ENGFile());
#if defined(WINDOWS)
    strExe = StrFormat("$/$.x", BINDIR, cSystem->ENGFile());
#else
    strExe = StrFormat("$/$$", BINDIR, cSystem->ENGFile(), envActive.cpEXE);
#endif
    strMap = StrAppend(strDbgDir, '/', cSystem->ENGFile(), envActive.cpMAP);
    strPdb = StrAppend(strDbgDir, '/', cSystem->ENGFile(), envActive.cpPDB);
    strObj = StrAppend(strDbgDir, '/', cSystem->ENGFile(),
      uiFlags & PF_PREPROC ? ".cpp" : envActive.cpOBJ),
    strAsm = StrAppend(strDbgDir, '/', cSystem->ENGFile(), envActive.cpASM);
    strRc = StrFormat("$/$.rc", WINDIR, ENGINENAME);
    strRes = StrFormat("$/$.res", strDbgDir, ENGINENAME);
    // 64 bit compilation?
    if(uiFlags & PF_X64)
    { // Append to command strings if available
      if(*envActive.cpCC8) strCmdCC += StrAppend(envActive.cpCC8, ' ');
      if(*envActive.cpRC8) strCmdRC += StrAppend(envActive.cpRC8, ' ');
      if(*envActive.cpLD8) strCmdLD += StrAppend(envActive.cpLD8, ' ');
      if(*envActive.cpLDB8) strCmdLD += StrAppend(envActive.cpLDB8, ' ');
    } // 32 bit compilation?
    else if(uiFlags & PF_X86)
    { // Append to command strings
      if(*envActive.cpCC4) strCmdCC += StrAppend(envActive.cpCC4, ' ');
      if(*envActive.cpRC4) strCmdRC += StrAppend(envActive.cpRC4, ' ');
      if(*envActive.cpLD4) strCmdLD += StrAppend(envActive.cpLD4, ' ');
      if(*envActive.cpLDB4) strCmdLD += StrAppend(envActive.cpLDB4, ' ');
    } // No architecture specified
    else XC("No architecture specified!", "Flags", uiFlags);
  } // Compiling engine?
  else
  { // Set filenames
    strCpp = StrFormat("$/$.cpp", SRCDIR, ENGINENAME);
    strRc = StrFormat("$/$.rc", WINDIR, ENGINENAME);
    strRes = StrFormat("$/$.res", strDbgDir, ENGINENAME);
    strExe = StrAppend(BINDIR, '/', ENGINENAME);
    strPdb = StrAppend(strDbgDir, '/', ENGINENAME);
    strObj = strPdb;
    strAsm = strPdb;
    strMap = strPdb;
    // 64 bit compilation?
    if(uiFlags & PF_X64)
    { // Append to command strings if available
      if(*envActive.cpCC8)  strCmdCC += StrAppend(envActive.cpCC8, ' ');
      if(*envActive.cpRC8)  strCmdRC += StrAppend(envActive.cpRC8, ' ');
      if(*envActive.cpLD8)  strCmdLD += StrAppend(envActive.cpLD8, ' ');
      if(*envActive.cpLDE8) strCmdLD += StrAppend(envActive.cpLDE8, ' ');
      // All these don't need checking because they still need 32/64 suffix
      strExe += StrAppend("64", envActive.cpEXE);
      strPdb += StrAppend("64", envActive.cpPDB);
      strObj += StrAppend("64",
        uiFlags & PF_PREPROC ? ".cpp" : envActive.cpOBJ);
      strAsm += StrAppend("64", envActive.cpASM);
      strMap += StrAppend("64", envActive.cpMAP);
    } // 32 bit compilation?
    else if(uiFlags & PF_X86)
    { // Append to command strings
      if(*envActive.cpCC4)  strCmdCC += StrAppend(envActive.cpCC4, ' ');
      if(*envActive.cpRC4)  strCmdRC += StrAppend(envActive.cpRC4, ' ');
      if(*envActive.cpLD4)  strCmdLD += StrAppend(envActive.cpLD4, ' ');
      if(*envActive.cpLDE4) strCmdLD += StrAppend(envActive.cpLDE4, ' ');
      // All these don't need checking because they still need 32/64 suffix
      strExe += StrAppend("32", envActive.cpEXE);
      strPdb += StrAppend("32", envActive.cpPDB);
      strObj += StrAppend("32", envActive.cpOBJ);
      strAsm += StrAppend("32", envActive.cpASM);
      strMap += StrAppend("32", envActive.cpMAP);
    } // No architecture specified
    else XC("No architecture specified!", "Flags", uiFlags);
    // Update version information
    { StdTMStruct tD;
      const StdTimeT tDuration = cmSys.GetTimeS();
      // Lets convert the duration as a time then it will be properly formated
      // in terms of leap years, proper days in a month etc.
      StdLocalTime(&tD, &tDuration);
      // Add time to years and months
      tD.tm_year = UtilMaximum(tD.tm_year-100, 0);
      tD.tm_mon += 1;
      // If dates are different then we reset the build number
      if(uiVer[0] != static_cast<unsigned int>(tD.tm_year) ||
         uiVer[1] != static_cast<unsigned int>(tD.tm_mon) ||
         uiVer[2] != static_cast<unsigned int>(tD.tm_mday))
        uiVer[0] = tD.tm_year, uiVer[1] = tD.tm_mon,
        uiVer[2] = tD.tm_mday, uiVer[3] = 1;
      // Update revision
      else uiVer[3]++;
    } // Update version information
    WriteVersion();
    // Write new version header
    const string
      strVersion{ StrFormat("$,$,$,$",
        uiVer[0], uiVer[1], uiVer[2], uiVer[3]) },
      strVersionStr{ StrFormat("\"$.$.$.$\"",
       uiVer[0], uiVer[1], uiVer[2], uiVer[3]) },
      strDate{ StrFormat("\"$\"", cmSys.FormatTime()) },
      strFile{ StrAppend(SRCDIR, '/', VERHEADER) },
      strText{ StrFormat(
        "$\n/* $ */\n"
        "#define VER_NAME    $$$ // Name of engine\n"
        "#define VER_AUTHOR  $$ // Author of engine\n"
        "#define VER_MAJOR   $$ // Version major (year)\n"
        "#define VER_MINOR   $$ // Version minor (month)\n"
        "#define VER_BUILD   $$ // Version build (day)\n"
        "#define VER_REV     $$ // Version rev (build#)\n"
        "#define VER_STR_NQ  $$ // Version as literal\n"
        "#define VER_STR     $$ // Version as string\n"
        "#define VER_DATE    $$ // Compilation date\n"
        "/* == EoF ==========================================================="
          " EoF == */\n",
        BuildHPPHeader(VERHEADER,
          "This file is automatically generated by build. Changes are lost "
          "when the engine is rebuilt."),
        string(73, '-'),
        left, setw(33), StrAppend('\"',strEName,'\"'),
              setw(33), StrAppend('\"',strEAuthor,'\"'),
              setw(33), uiVer[0],   setw(33), uiVer[1],
              setw(33), uiVer[2],   setw(33), uiVer[3],
              setw(33), strVersion, setw(33), strVersionStr,
              setw(33), strDate) };
    // Write new version header
    if(FStream{ strFile, FM_W_B }.FStreamWriteStringSafe(strText)
      != strText.length())
        XCL("Could not write version header!", "File", strFile);
  } // Check files are readable and writable
  CheckFiles({ strRc, strCpp },
             { strExe, strRes, strPdb, strObj, strAsm, strMap });
  // Analyse flag specified? Add to compiler flags.
  if((uiFlags & PF_ANALYSE) && *envActive.cpCCAnal)
    strCmdCC += StrAppend(envActive.cpCCAnal, ' ');
  // Analyse includes flag specified? Add to compiler flags.
  if((uiFlags & PF_CCINCS) && *envActive.cpCCIncDBG)
    strCmdCC += StrAppend(envActive.cpCCIncDBG, ' ');
  // Preprocess?
  if((uiFlags & PF_PREPROC) && *envActive.cpCCPP)
    strCmdCC += StrAppend(envActive.cpCCPP, ' ');
  // Add object and symbol file to linker if specified
  if(*envActive.cpLDEXE)
    strCmdLD += StrAppend(StrFormat(envActive.cpLDEXE, strExe, strPdb), ' ');
  // Release compilation specified?
  if(uiFlags & PF_FINAL)
  { // Append RELEASe parameters to command lines
    if(*envActive.cpCCAR) strCmdCC += StrAppend(envActive.cpCCAR, ' ');
    if(*envActive.cpRCAR) strCmdRC += StrAppend(envActive.cpRCAR, ' ');
    if(*envActive.cpLDAR) strCmdLD += StrAppend(envActive.cpLDAR, ' ');
  } // Alpha (debug) compilation specified?
  else if(uiFlags & PF_ALPHA)
  { // Append ALPHA parameters to command lines
    if(*envActive.cpCCAA) strCmdCC += StrAppend(envActive.cpCCAA, ' ');
    if(*envActive.cpRCAA) strCmdRC += StrAppend(envActive.cpRCAA, ' ');
    if(*envActive.cpLDAA) strCmdLD += StrAppend(envActive.cpLDAA, ' ');
  } // Beta (normal) compilation specified?
  else if(uiFlags & PF_BETA)
  { // Append BETA parameters to command lines
    if(*envActive.cpCCAB) strCmdCC += StrAppend(envActive.cpCCAB, ' ');
    if(*envActive.cpRCAB) strCmdRC += StrAppend(envActive.cpRCAB, ' ');
    if(*envActive.cpLDAB) strCmdLD += StrAppend(envActive.cpLDAB, ' ');
  } // Add map filename to linker flags if specified
  if((uiFlags & PF_MAP) && *envActive.cpLDMAP)
    strCmdLD += StrAppend(StrFormat(envActive.cpLDMAP, strMap), ' ');
  // Add assembler filename to compiler flags if specified
  if((uiFlags & PF_ASM) && *envActive.cpCCASM)
    strCmdCC += StrAppend(envActive.cpCCASM, strAsm, ' ');
  // Append object and source filename to compiler if specified
  if(*envActive.cpCCOBJ)
    strCmdCC += StrAppend(envActive.cpCCOBJ, strObj, ' ', strCpp);
  // Add object filename to linker
  strCmdLD += StrAppend(strObj, ' ');
  // Perform scan for libraries
  const Dir dLibs{ LIBDIR, envActive.cpLIB };
  for(DirEntMapConstIt demciIt{ dLibs.GetFilesBegin() };
                       demciIt != dLibs.GetFilesEnd();
                     ++demciIt)
  { // Only allow libs that contain this string
    string strX;
    // Build string
#if defined(MACOS)
    strX += "64.";
#else
    if(uiFlags & PF_X64) strX += "64.";
    else if(uiFlags & PF_X86) strX += "32.";
#endif
    // Add lib if it contains specified string
    if(demciIt->first.find(strX) != StdNPos)
      strCmdLD += StrFormat("$/$ ", LIBDIR, demciIt->first);
  } // Add default libs
  if(*envActive.cpLDL) strCmdLD += StrAppend(envActive.cpLDL, ' ');
  // Compile sources
  if(SpecialExecute(strCmdCC, (uiFlags & PF_MORE) ? 0xFFFFFFFF : 10)) return 1;
  // Return if only preprocessing or analysing
  if(uiFlags & PF_PREPROC || uiFlags & PF_ANALYSE || uiFlags & PF_ASM)
    return 0;
  // Resource manager specified?
  if(*envActive.cpCCRES)
  { // Add resource to linker command
    strCmdLD += StrAppend(strRes, ' ');
    // Compile resource for executable
    strCmdRC += StrAppend(envActive.cpCCRES, strRes, ' ', strRc);
    if(SpecialExecute(strCmdRC, 10)) return 1;
  } // Save executable size
  const uint64_t qOldSize = DirLocalFileExists(strExe) ?
    FStream{ strExe, FM_R_B }.FStreamSize() : 0;
  // Link sources and resources and if failed?
  if(SpecialExecute(strCmdLD, 10))
  { // Clean up temporary files that MS likes leaving behind
    DoCleanCompilerTempFiles();
    // Return status
    return 1;
  } // Clean up temporary files that MS likes leaving behind
  DoCleanCompilerTempFiles();
  // Show executable size
  const uint64_t qNewSize = FStream{ strExe, FM_R_B }.FStreamSize();
  cout << "*** Executable size is " << qNewSize << " bytes (" <<
    StrToBytes(qNewSize, 2) << ")." << endl;
  // If the old executable had size?
  if(qOldSize && qOldSize != qNewSize)
  { // Calculate and show changed size
    const int64_t qChange = qNewSize - qOldSize;
    // Decreased?
    if(qChange < 0)
    { // Negate to positive
      const uint64_t qAdjChange = -qChange;
      cout << "*** Executable size decreased " << qAdjChange << " bytes (" <<
        StrToBytes(qAdjChange, 2) << ")." << endl;
    } // Increqased?
    else cout << "*** Executable size increased " << qChange << " bytes (" <<
      StrToBytes(qChange, 2) << ")." << endl;
  } // Move new executable into position
  if(bSelf) CheckForNewBuildExecutable();
  // Done
  return 0;
}
/* ------------------------------------------------------------------------- */
static int DebugApp(void)
{ // Goto assets directory
  SetDirectory(strName);
  // Execute the debugger
  if(uiFlags & PF_X86)
    return SystemF(envActive.cpDBG, strName, strName, 32, envActive.cpEXE);
  else if(uiFlags & PF_X64)
    return SystemF(envActive.cpDBG, strName, strName, "", envActive.cpEXE);
  else throw runtime_error{ "unknown arch" };
}
/* ------------------------------------------------------------------------- */
static bool CheckCommandLine(string &strX1, string &strX2)
{ // Get first argument
  const string &strTokens = cCmdLine->CmdLineGetArgList().empty() ?
    cCommon->CommonBlank() : cCmdLine->CmdLineGetArgList()[0];
  // Function data structure
  struct FuncData {
    const unsigned int uiArg;           // Requires argument
    const uint64_t     uiAdd, uiRemove; // Flags to add and remove
    const char*const   cpDesc;          // Description of flag
  }; // Wrap into a list
  typedef pair<const string, const FuncData> FuncListMapPair;
  typedef map<FuncListMapPair::first_type,
              FuncListMapPair::second_type> FuncListMap;
  typedef FuncListMap::const_iterator FuncListMapConstIt;
  // Command-line argument to flag and description list
  const FuncListMap flData{
  /* ----------------------------------------------------------------------- */
  { "?", { 0, PF_USAGE,           PF_NONE,
    "Display this usage output."} },
  { "!", { 0, PF_VERSION,         PF_ALL|PF_RVER|PF_RPROJ,
    "Show version information."} },
#if defined(WINDOWS)
  { "1", { 0, PF_ENVWMSVC,        PF_ENVWLLVM|PF_ENVWCLCP|PF_ENVGCC|PF_OTHERS,
    "Use MSVC environment."} },
  { "2", { 0, PF_ENVWCLCP,        PF_ENVWMSVC|PF_ENVWLLVM|PF_ENVGCC|PF_OTHERS,
    "Use CLANG-MSVC environment."} },
  { "3", { 0, PF_ENVWLLVM,        PF_ENVWMSVC|PF_ENVWCLCP|PF_ENVGCC|PF_OTHERS,
    "Use pure CLANG environment."} },
#elif defined(MACOS)
  { "1", { 0, PF_ENVXCODE,        PF_ENVWMSVC|PF_ENVWLLVM|PF_ENVGCC|PF_OTHERS,
    "Use LLVM environment."} },
#elif defined(LINUX)
  { "1", { 0, PF_ENVGCC,         PF_ENVWMSVC|PF_ENVWLLVM|PF_ENVXCODE|PF_OTHERS,
    "Use GCC environment."} },
#endif
  { "4", { 0, PF_X86,             PF_X64|PF_OTHERS,
#if defined(MACOS)
    "Perform X86-64 compilation."} },
#else
    "Perform 32-bit compilation."} },
#endif
  { "8", { 0, PF_X64,             PF_X86|PF_OTHERS,
#if defined(MACOS)
    "Perform ARM64 compilation."} },
#else
    "Perform 64-bit compilation."} },
#endif
  { "a", { 0, PF_PREPROC,         PF_OTHERS,
    "Pre-process to amalgamation."} },
  { "A", { 0, PF_ANALYSE,         PF_OTHERS,
    "Analyse code for issues."} },
  { "b", { 0, PF_BETA,            PF_OTHERS|PF_ALPHA|PF_FINAL,
    "Compile beta version."} },
  { "C", { 0, PF_CERT,            PF_RVER|PF_RPROJ,
    "Rebuild executable certificate."} },
  { "D", { 0, PF_DOC,             PF_RPROJ|PF_ALL,
    "Build LUA API documentation."} },
  { "d", { 0, PF_ALPHA,           PF_OTHERS|PF_BETA|PF_FINAL,
    "Compile alpha (debug) version."} },
  { "e", { 2, PF_EXTLIB,          PF_RVER|PF_RPROJ|PF_ALL,
    "Compile archive library 'x'."} },
  { "f", { 0, PF_DISTRO,          PF_ALL,
    "Build distributable package."} },
  { "g", { 0, PF_CAGEN,           PF_RVER|PF_RPROJ|PF_ALL,
    "Generate trusted root CA store."} },
  { "h", { 0, PF_CHECKSRC,        PF_ALL,
    "Check source files style."} },
  { "i", { 0, PF_CCINCS,          PF_OTHERS,
    "Show included headers."} },
  { "I", { 0, PF_BLICS,           PF_ALL,
    "Build api license headers."} },
  { "l", { 0, PF_ASSETS,          PF_RPROJ|PF_ALL,
    "Update app assets library."} },
  { "L", { 0, PF_FASSETS,         PF_RPROJ|PF_ALL,
    "Rebuild app assets library."} },
  { "M", { 0, PF_MAP,             PF_OTHERS,
    "Generate map of executable."} },
  { "m", { 0, PF_MORE,            PF_OTHERS,
    "Show more compiler log lines."} },
  { "n", { 1, PF_NPROJ,           PF_ALL,
    "Create new project 'x'."} },
  { "N", { 0, PF_NOMERGE,         PF_NONE,
    "Do not drop message duplicates."} },
  { "p", { 1, PF_CHPROJ,          PF_ALL,
    "Switch project to 'x'."} },
  { "P", { 0, PF_CPPCHK|PF_MORE|PF_SYSOUT,PF_RVER|PF_RPROJ|PF_OTHERS,
    "Process source with CPPCheck."} },
  { "r", { 0, PF_FINAL,           PF_OTHERS|PF_ALPHA|PF_BETA,
    "Compile release version."} },
  { "s", { 0, PF_ASM,             PF_OTHERS,
    "Produce assembler output."} },
  { "S", { 0, PF_CPSELF,          PF_RVER|PF_RPROJ|PF_OTHERS,
    "Recompile build executable."} },
  { "V", { 0, PF_NONE,            PF_SYSNOERR,
    "Show stderr output on execute."} },
  { "v", { 0, PF_SYSOUT,          PF_NONE,
    "Verbose stdout on execute."} },
  { "x", { 0, PF_EXEC,            PF_OTHERS,
    "Run and debug the executable."} } };
  /* ----------------------------------------------------------------------- */
  // Arguments needed
  unsigned int uiArgsRequired = 0;
  unsigned char ucArgsNeededFor = '\0';
  // Iterate through the tokens we found in the string
  for(const char ucP : strTokens)
  { // Find the command and if we couldn't find it?
    const FuncListMapConstIt flItem{ flData.find(string{ &ucP, 1 }) };
    if(flItem == flData.end())
    { // Report invalid token and goto next token
      cout << "Invalid token '" << ucP << "' ignored.\n";
      continue;
    } // Get reference to flag data and if this token requires arguments?
    const FuncData &flD = flItem->second;
    if(flD.uiArg)
    { // If the argument data has already been recorded? Throw error
      if(uiArgsRequired)
        XC("Tokens incompatible!",
           "Token1", ucArgsNeededFor,
           "Token2", ucP,
           "Args", cCmdLine->CmdLineGetArgList().size());
      // Not enough arguments specified? Throw Error
      if(cCmdLine->CmdLineGetArgList().size() <= 1)
        XC("Token requires extra argument!",
           "Token", ucP, "Args", cCmdLine->CmdLineGetArgList().size());
      // Record the argument required
      strX1 = cCmdLine->CmdLineGetArgList()[1];
      // Record an optional argument if needed
      if(flD.uiArg >= 2 && cCmdLine->CmdLineGetArgList().size() >= 3)
        strX2 = cCmdLine->CmdLineGetArgList()[2];
      // Update token data
      ucArgsNeededFor = ucP, uiArgsRequired = flD.uiArg;
    } // We found it so set and remove flags as appropriate
    uiFlags = (uiFlags & ~flD.uiRemove) | flD.uiAdd;
  } // If we're not calling for the usage?
  if(!(uiFlags & PF_USAGE))
  { // Walk through available flags and write them out
    for(const FuncListMapPair &flItem : flData)
      if(uiFlags & flItem.second.uiAdd)
        cout << "* " << flItem.second.cpDesc << '\n';
    cout << '\n';
    // Done
    return false;
  } // Remove usage flag or the usage shows the usage as a default flag
  uiFlags &= ~PF_USAGE;
  // Show initial usage
  cout << "Usage: " << cSystem->ENGFile() << " <tokens> [x].\n\n"
       << "Tokens<" << flData.size() << ">:- (* = option assumed).\n"
       << left;
  // Buffer for left side option
  size_t stWidth = 32, stMax = stWidth;
  for(const FuncListMapPair &flI : flData)
  { // Write option to screen
    cout << ((uiFlags & flI.second.uiAdd) ? '*' : ' ')
         << flI.first
         << (flI.second.uiArg ? " x" : cCommon->CommonDblSpace())
         << " = "
         << setw(stWidth)
         << string{ flI.second.cpDesc, 0, stMax }
         << setw(0);
    // Switch width and add a carriage return if the right option
    if(stWidth) stWidth = 0; else stWidth = stMax, cout << '\n';
  } // Add a carriage return if on left side
  if(!stWidth) cout << '\n';
  // Terminate program normally
  return true;
}
/* ------------------------------------------------------------------------- */
static void ReadProject(void)
{ // If project does not exist then show the error
  if(!DirLocalDirExists(strName))
    XCL("Could not find project directory!",
        "Directory", strName, "Current", DirGetCWD());
  // Create filename
  const string strFile{ StrFormat("$/" DEFAULT_CONFIGURATION, strName) };
  // Create the json object and parse it
  const Json jsManifest{ StrAppend(strFile, "." JSON_EXTENSION) };
  // Check version is correct
  const unsigned int uiVersionRequired = 1;
  const unsigned int uiVersion = jsManifest.GetInteger("Version");
  if(uiVersion != uiVersionRequired)
    XC("Invalid application manifest version!",
       "Manfiest", jsManifest.IdentGet(), "Required", uiVersionRequired,
       "Actual",   uiVersion);
  // Look for constants and throw if there are none then report them in log
  using Lib::RapidJson::Value;
  const Value &rjvConstants = jsManifest.GetValue("Constants");
  if(!rjvConstants.IsObject())
    XC("Constants array not valid!", "Manfiest", jsManifest.IdentGet());
  // Get version and long name of app and update the version
  strArch = StdMove(rjvConstants["app_shortname"].GetString());
  strVer = StdMove(rjvConstants["app_version"].GetString());
  strTitle = StdMove(rjvConstants["app_longname"].GetString());
  strAuthor = StdMove(rjvConstants["app_author"].GetString());
}
/* ------------------------------------------------------------------------- */
static int ShowVersion(void)
{ // Write extra details to log
  cLog->LogDebugExSafe(
    "Executable drive: \"$\".\n"    "Executable directory: \"$\".\n"
    "Executable file: \"$\".\n"     "Executable extension: \"$\".\n"
    "Executable filename: \"$\".\n" "Executable location: \"$\".",
    cSystem->ENGDrive(), cSystem->ENGDir(), cSystem->ENGFile(),
    cSystem->ENGExt(), cSystem->ENGFileExt(), cSystem->ENGLoc());
  // For each log entry, write the line to the buffer
  for(const LogLine &llLine : *cLog)
    cout << '[' << fixed << setprecision(6) << llLine.dTime << "] "
         << llLine.strLine << endl;
  // Success
  return 0;
}
/* ------------------------------------------------------------------------- */
static void SelectEnvironment(const Environment &envNew)
{ // Copy new environment over. GCC will complain if you try to memcpy to
  // a structure. It thinks it's a class but it's not, it's a struct!
  memcpy(reinterpret_cast<void*>(&envActive), &envNew, sizeof(Environment));
}
/* ------------------------------------------------------------------------- */
static int Build(const int iArgC, ArgType**const saArgV,
  ArgType**const saEnv) try
{ // Set this thread's name for debugger and that it is high performance
  SysInitThread("main", STP_MAIN);
  // Dependencies required only in this scope
  using namespace IArchive::P;         using namespace IArgs::P;
  using namespace IAsset::P;           using namespace ICmdLine::P;
  using namespace IClock::P;           using namespace ICrypt::P;
  using namespace IEvtMain::P;         using namespace IFile::P;
  using namespace ILuaFunc::P;         using namespace IMemory::P;
  using namespace IParser::P;          using namespace IStat::P;
  using namespace IThread::P;          using namespace IUtil::P;
  using namespace IUtf::P;
  // Initialise required subsystems
  const struct Engine : private Common, private DirBase, private CmdLine,
    private Log, private Stats, private Threads, private EvtMain,
    private System, private LuaFuncs, private Archives, private Assets,
    private Crypt, private Jsons
  { Engine(const int iArgC, ArgType**const saArgV, ArgType**const saEnv) :
      CmdLine{ iArgC, saArgV, saEnv } { }
  } engEngine{ iArgC, saArgV, saEnv };
  // Force current working directory to the base directory
  SetDirectory(
#if defined(WINDOWS)
    cSystem->ENGLoc()                  // Exe dir will be where link is
#else
    StrAppend(cSystem->ENGLoc(), "..") // Links always run in bin dir
#endif
  );
  // Check for unfinished install of new build executable
  CheckForNewBuildExecutable();
  // Show header
  cout << strEName << "™, Project Management Utility, "
       << cSystem->ENGCompiled()
       << ".\nCopyright © " << strEAuthor << ", 2006-"
       << cmSys.FormatTime("%Y")
       << ". All Rights Reserved.\n\n";
  // Initialise default flags
  string strExtra1, strExtra2;
  if(CheckCommandLine(strExtra1, strExtra2)) return 0;
  // Read version and current project information if needed
  if(uiFlags & PF_RVER)  ReadVersion();
  if(uiFlags & PF_RPROJ) ReadProject();
  // Set environment
  if(uiFlags & PF_ENVXCODE) SelectEnvironment(envMacOSLLVM);
  if(uiFlags & PF_ENVGCC)   SelectEnvironment(envLinuxGCC);
  if(uiFlags & PF_ENVWMSVC) SelectEnvironment(envWindowsMSVC);
  if(uiFlags & PF_ENVWLLVM) SelectEnvironment(envWindowsLLVM);
  if(uiFlags & PF_ENVWCLCP) SelectEnvironment(envWindowsLLVMcompat);
  // Compare other flags
  if(uiFlags & PF_EXEC)     return DebugApp();
  if(uiFlags & PF_VERSION)  return ShowVersion();
  if(uiFlags & PF_ASSETS)   return RebuildAssets(false);
  if(uiFlags & PF_CAGEN)    return CertGen();
  if(uiFlags & PF_CERT)     return CertFunc({});
  if(uiFlags & PF_CHPROJ)   return ChangeProject(StrToLowCase(strExtra1));
  if(uiFlags & PF_CPPCHK)   return CppCheck();
  if(uiFlags & PF_CPSELF)   return Compile(true);
  if(uiFlags & PF_DISTRO)   return BuildDistro();
  if(uiFlags & PF_DOC)      return GenDoc();
  if(uiFlags & PF_CHECKSRC) return CheckSources();
  if(uiFlags & PF_EXTLIB)   return ExtLibScript(strExtra1, strExtra2);
  if(uiFlags & PF_FASSETS)  return RebuildAssets(true);
  if(uiFlags & PF_NPROJ)    return NewProject(StrToLowCase(strExtra1));
  if(uiFlags & PF_COMPILE)  return Compile(false);
  if(uiFlags & PF_BLICS)    return BuildLicenses();
  // Nthing to do
  XC("Nothing to do!", "Flags", uiFlags);
} // exception occured?
catch(const exception &e)
{ // Print exception message and return error status
  cout << "exception > " << e.what() << "\n";
  return -1;
}
/* ------------------------------------------------------------------------- */
};                                     // End of core interface namespace
/* ========================================================================= */
int CONENTRYFUNC(int iArgC, ArgType**saArgV, ArgType**saEnv)
  { return E::Build(iArgC, saArgV, saEnv); }
/* == End-of-File ========================================================== */
