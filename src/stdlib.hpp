/* == STDLIB.HPP =========================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## Core standard cross-platform wrappers for core C function.          ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IStdLib {                    // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace ICommon::P;            using namespace IStd::P;
using namespace IUtf::P;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* ------------------------------------------------------------------------- */
#if defined(WINDOWS)                   // Using Windows?
/* -- These don't exist on Windows so we'll make it easier ----------------- */
constexpr static const int R_OK = 4,   // File is readable (StdAccess)
                           W_OK = 2,   // File is writable (StdAccess)
                           X_OK = 1,   // File is executable (StdAccess)
                           F_OK = 0;   // File exists (StdAccess)
/* -- Typedefs for structure names that are different on Windows ----------- */
using StdFStatStruct = struct ::__stat64; // Different on Windows
using StdTMStruct    = struct ::tm;       // Different on Windows
using StdTimeT       = __time64_t;        // Different on Windows
/* -- Convert any widestring pointer type to utf8 class string ------------- */
template<typename AnyType>
  static StdString S16toUTF(const AnyType atVar)
{ // If sent as a wide c-string then make it a wide string
  using RawType = StdRemoveConstVolRef<AnyType>;
  if constexpr(StdIsPointer<RawType>)
    return UtfFromWide(StdWideStringView{ atVar });
  // If sent a literal wide c-string?
  else if constexpr(StdIsArray<RawType>)
    return UtfFromWide(StdWideStringView{ atVar, StdExtent<RawType>() - 1 });
  // Anything else treat as a string
  else { return UtfFromWide(atVar); }
}
/* -- Convert STL widestring to utf8 --------------------------------------- */
template<typename StrType>
  static StdString WS16toUTF(StrType &&strStr)
{ return UtfFromWide(StdForward<StrType>(strStr)); }
/* -- Convert UTF c-string to STL widestring ------------------------------- */
template<typename StrType>
  static StdWideString UTFtoS16(StrType &&strType)
{ return UtfDecoder{ StdForward<StrType>(strType) }.UtfWide(); }
/* -- Converts any string type to a pointer for wide C functions ----------- */
template<typename StrType>
  constexpr static auto StrDenormaliseWideString(StrType &&strStr,
    auto &&aFunc)
{ // Get type without reference and a decayed type
  using StrTypeDecayed = StdDecay<StrType>;
  // If type is an C-style text array?
  if constexpr(StdIsArray<StdRemoveReference<StrType>> ||
               StdIsPointer<StrTypeDecayed>)
    return aFunc(strStr);
  // Correct type is a StdString?
  else if constexpr(StdIsString<StrTypeDecayed>)
    return aFunc(UTFtoS16(strStr).data());
  // Anything else is invalid
  else static_assert(sizeof(StrType) == 0, "Invalid wide string type!");
}
/* -- Unset an environment variable ---------------------------------------- */
template<typename StrType>
  static bool StdUnSetEnv(StrType &&strStr)
{ return StrDenormaliseString(StdForward<StrType>(strStr),
    [](const char*const cpStr)->bool
      { return !_putenv_s(cpStr, cCommon->CommonCBlank()); }); }
/* -- Wrapper for _waccess() ----------------------------------------------- */
template<typename StrType>
  static bool StdAccess(StrType &&strStr, const int iMode)
{ return StrDenormaliseWideString(StdForward<StrType>(strStr),
    [iMode](const wchar_t*const wcpStr)->bool
      { return _waccess(wcpStr, iMode); }); }
/* -- Wrapper for mkdir() function ----------------------------------------- */
template<typename StrType>
  static int StdMkDir(StrType &&strStr)
{ return StrDenormaliseWideString(StdForward<StrType>(strStr),
    [](const wchar_t*const wcpStr)->int{ return _wmkdir(wcpStr); }); }
/* -- Wrapper for _wrename() function -------------------------------------- */
template<typename StrFromType, typename StrToType>
  static int StdRename(StrFromType &&strFromStr, StrToType &&strToStr)
{ return StrDenormaliseWideString(StdForward<StrFromType>(strFromStr),
    [&strToStr](const wchar_t*const wcpFromStr)->int{
      return StrDenormaliseWideString(StdForward<StrToType>(strToStr),
        [wcpFromStr](const wchar_t*const wcpToStr)->int{
          return _wrename(wcpFromStr, wcpToStr);
        });
    });
}
/* -- Wrapper for _wrmdir() function --------------------------------------- */
template<typename StrType>
  static int StdRmDir(StrType &&strStr)
{ return StrDenormaliseWideString(StdForward<StrType>(strStr),
    [](const wchar_t*const wcpStr)->int{ return _wrmdir(wcpStr); }); }
/* -- Wrapper for _wchdir() function --------------------------------------- */
template<typename StrType>
  static int StdChDir(StrType &&strStr)
{ return StrDenormaliseWideString(StdForward<StrType>(strStr),
    [](const wchar_t*const wcpStr)->int{ return _wchdir(wcpStr); }); }
/* -- Wrapper for _wunlink() function -------------------------------------- */
template<typename StrType>
  static int StdUnlink(StrType &&strStr)
{ return StrDenormaliseWideString(StdForward<StrType>(strStr),
    [](const wchar_t*const wcpStr)->int{ return _wunlink(wcpStr); }); }
/* -- Wrapper for _wexecve() stdlib function ------------------------------- */
static int StdExecVE(const wchar_t*const *wcpaArg,
  const wchar_t*const *wcpaEnv)
{ return static_cast<int>(_wexecve(*wcpaArg, wcpaArg, wcpaEnv)); }
/* -- Wrapper for _wspawnve() stdlib function ------------------------------ */
static int StdSpawnVE(const wchar_t*const wcpaArg[],
  const wchar_t*const *wcpaEnv)
{ return static_cast<int>(_wspawnve(P_NOWAIT,
    *wcpaArg, wcpaArg, wcpaEnv) == -1); }
/* -- Wrapper for _fileno() function --------------------------------------- */
static int StdFileNo(FILE*const fStream) { return _fileno(fStream); }
/* -- Wrapper for _wstat64() function -------------------------------------- */
template<typename StrType>
  static int StdFStat(StrType &&strStr, StdFStatStruct*const sDestBuffer)
{ return StrDenormaliseWideString(StdForward<StrType>(strStr),
    [sDestBuffer](const wchar_t*const wcpStr)->int
      { return _wstat64(wcpStr, sDestBuffer); }); }
/* -- Wrapper for _fseeki64() function ------------------------------------- */
static int StdFSeek(FILE*const fStream, const int64_t llOffset, int iWhence)
  { return _fseeki64(fStream, llOffset, iWhence); }
/* -- Wrapper for _ftelli64() function ------------------------------------- */
static int64_t StdFTell(FILE*const fStream)
  { return _ftelli64(fStream); }
/* -- Wrapper for _gmtime64_s() function ----------------------------------- */
static void StdGMTime(StdTMStruct*const tmpResult, const StdTimeT*const tpTime)
  { _gmtime64_s(tmpResult, tpTime); }
/* -- Wrapper for _fstat64() function -------------------------------------- */
static int StdHStat(const int iFd, StdFStatStruct*const sDestBuffer)
  { return _fstat64(iFd, sDestBuffer); }
/* -- Wrapper for _localtime64_s() function -------------------------------- */
static void StdLocalTime(StdTMStruct*const tmpResult,
  const StdTimeT*const tpTime)
{ _localtime64_s(tmpResult, tpTime); }
/* -- Wrapper for mktime() function ---------------------------------------- */
static StdTimeT StdMkTime(StdTMStruct*const tmpResult)
  { return _mktime64(tmpResult); }
/* -- Wrapper for _wpopen() function --------------------------------------- */
template<typename StrType>
  static FILE *StdPOpen[[maybe_unused]](StrType &&strStr,
    const wchar_t*const wcpType = L"rt")
{ return StrDenormaliseWideString(StdForward<StrType>(strStr),
    [wcpType](const wchar_t*const wcpStr)->FILE*
      { return _wpopen(wcpStr, wcpType); }); }
/* -- Wrapper for _pclose() function --------------------------------------- */
static int StdPClose[[maybe_unused]](FILE*const fStream)
  { return _pclose(fStream); }
/* -- Wrapper for srandom() function --------------------------------------- */
static void StdSRand(const unsigned uSeed) { srand(uSeed); }
/* ------------------------------------------------------------------------- */
#else                                  // Posix compatible target?
/* -- Maximum path part lengths -------------------------------------------- */
constexpr static const size_t          // For compatibility with Windows
  _MAX_PATH  = PATH_MAX,               // Maximum size of all path parts
  _MAX_DIR   = PATH_MAX,               // Maximum size of directory part
  _MAX_FNAME = PATH_MAX,               // Maximum size of filename part
  _MAX_EXT   = PATH_MAX;               // Maximum size of extension part
/* -- Other cross compatibilities ------------------------------------------ */
constexpr static const unsigned        // For compatibility with Windows
  _S_IFDIR           = S_IFDIR,        // Specifies a directory for stat()
  MB_SYSTEMMODAL     = 0,              // No window was available for message
  MB_ICONINFORMATION = 1,              // Informational message only
  MB_ICONEXCLAMATION = 2,              // A warning message occured
  MB_ICONSTOP        = 4;              // A critical error message occured
/* -- Typedefs for structure names that are different on Windows ----------- */
using StdFStatStruct = struct ::stat;  // Different on Windows
using StdTMStruct    = struct ::tm;    // Different on Windows
using StdTimeT       = time_t;         // Different on Windows
/* -- This is N/A on non-Windows so just return a string ------------------- */
static StdString S16toUTF(auto &&aAnything) { return aAnything; }
/* -- Unset an environment variable ---------------------------------------- */
template<typename StrType>
  static bool StdUnSetEnv(StrType &&strStr)
{ return StrDenormaliseString(StdForward<StrType>(strStr),
    [](const char*const cpStr)->bool{ return !unsetenv(cpStr); }); }
/* -- Wrapper for access() stdlib function --------------------------------- */
template<typename StrType>
  static bool StdAccess(StrType &&strStr, const int iMode)
{ return StrDenormaliseString(StdForward<StrType>(strStr),
    [iMode](const char*const cpStr)->bool{ return access(cpStr, iMode); }); }
/* -- Wrapper for rename() function ---------------------------------------- */
template<typename StrFromType, typename StrToType>
  static int StdRename(StrFromType &&strFromStr, StrToType &&strToStr)
{ return StrDenormaliseString(StdForward<StrFromType>(strFromStr),
    [&strToStr](const char*const cpFromStr)->int{
      return StrDenormaliseString(StdForward<StrToType>(strToStr),
        [cpFromStr](const char*const cpToStr)->int{
          return rename(cpFromStr, cpToStr);
        });
    });
}
/* -- Wrapper for mkdir() function ----------------------------------------- */
template<typename StrType>
  static int StdMkDir(StrType &&strStr)
{ return StrDenormaliseString(StdForward<StrType>(strStr),
    [](const char*const cpStr)->int{ return mkdir(cpStr,
      S_IRUSR|S_IWUSR|S_IXUSR|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH); }); }
/* -- Wrapper for rmdir() function ----------------------------------------- */
template<typename StrType>
  static int StdRmDir(StrType &&strStr)
{ return StrDenormaliseString(StdForward<StrType>(strStr),
    [](const char*const cpStr)->int{ return rmdir(cpStr); }); }
/* -- Wrapper for chdir() function ----------------------------------------- */
template<typename StrType>
  static int StdChDir(StrType &&strStr)
{ return StrDenormaliseString(StdForward<StrType>(strStr),
    [](const char*const cpStr)->int{ return chdir(cpStr); }); }
/* -- Wrapper for unlink() function ---------------------------------------- */
template<typename StrType>
  static int StdUnlink(StrType &&strStr)
{ return StrDenormaliseString(StdForward<StrType>(strStr),
    [](const char*const cpStr)->int{ return unlink(cpStr); }); }
/* -- Wrapper for execve() stdlib function --------------------------------- */
static int StdExecVE(const char*const *cpaArg, const char*const *cpaEnv)
  { return execve(*cpaArg, const_cast<char**>(cpaArg),
      const_cast<char**>(cpaEnv)); }
/* -- Wrapper for posix_spawn() stdlib function ---------------------------- */
static int StdSpawnVE(const char*const *cpaArg, const char*const *cpaEnv)
  { pid_t pId; return posix_spawn(&pId, *cpaArg, nullptr, nullptr,
      const_cast<char**>(cpaArg), const_cast<char**>(cpaEnv)); }
/* -- Wrapper for fileno() function ---------------------------------------- */
static int StdFileNo(FILE*const fStream) { return fileno(fStream); }
/* -- Wrapper for stat() function ------------------------------------------ */
template<typename StrType>
  static int StdFStat(StrType &&strStr, StdFStatStruct*const sDestBuffer)
{ return StrDenormaliseString(StdForward<StrType>(strStr),
    [sDestBuffer](const char*const cpStr)->int
      { return stat(cpStr, sDestBuffer); }); }
/* -- Wrapper for fseek() function ----------------------------------------- */
static int StdFSeek(FILE*const fStream, const off_t otOffset, int iWhence)
  { return fseeko(fStream, otOffset, iWhence); }
/* -- Wrapper for ftell() function ----------------------------------------- */
static off_t StdFTell(FILE*const fStream) { return ftello(fStream); }
/* -- Wrapper for gmtime_r() function -------------------------------------- */
static void StdGMTime(StdTMStruct*const tmpResult, const StdTimeT*const tpTime)
  { gmtime_r(tpTime, tmpResult); }
/* -- Wrapper for fstat() function ----------------------------------------- */
static int StdHStat(const int iFd, StdFStatStruct*const sDestBuffer)
  { return fstat(iFd, sDestBuffer); }
/* -- Wrapper for localtime_r() function ----------------------------------- */
static void StdLocalTime(StdTMStruct*const tmpResult,
  const StdTimeT*const tpTime)
{ localtime_r(tpTime, tmpResult); }
/* -- Wrapper for mktime() function ---------------------------------------- */
static StdTimeT StdMkTime(StdTMStruct*const tmpResult)
  { return mktime(tmpResult); }
/* -- Wrapper for popen() function ----------------------------------------- */
template<typename StrType>
  static FILE *StdPOpen[[maybe_unused]](StrType &&strStr,
    const char*const cpType = "r")
{ return StrDenormaliseString(StdForward<StrType>(strStr),
    [cpType](const char*const cpStr)->FILE*
      { return popen(cpStr, cpType); }); }
/* -- Wrapper for pclose() function ---------------------------------------- */
static int StdPClose[[maybe_unused]](FILE*const fStream)
  { return pclose(fStream); }
/* -- Wrapper for srandom() function --------------------------------------- */
static void StdSRand(const unsigned uSeed) { srandom(uSeed); }
/* -- Wrapper for mmap function -------------------------------------------- */
template<typename CharType>
  requires (!StdIsPointer<CharType>)
static CharType *StdMMap(void*vpAddr, const size_t stLen,
  const int iProtection, const int iFlags, const int iDescriptor,
  const off_t otOffset)
{ return reinterpret_cast<CharType*>(Lib::OS::mmap(vpAddr, stLen,
    iProtection, iFlags, iDescriptor, otOffset)); }
/* ------------------------------------------------------------------------- */
#endif                                 // Operating system check
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
