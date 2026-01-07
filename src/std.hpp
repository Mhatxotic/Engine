/* == STD.HPP ============================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## This module describes proxy functions to target functions from the  ## **
** ## standard C or C++ library. Of course these functions differ between ## **
** ## different targets which is handled properly too.                    ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IStd {                       // Start of private module namespace
/* ------------------------------------------------------------------------- */
using namespace ICommon::P;            using namespace IUtf::P;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* -- Wrapper for std::forward<> as we can't do 'using std::forward' ------- */
template<typename AnyType>
  constexpr static auto &&StdForward(auto &&...aArgs)
{ return ::std::forward<AnyType>(aArgs...); }
/* ------------------------------------------------------------------------- */
#if defined(MACOS)                     // Using MacOS?
/* ------------------------------------------------------------------------- **
** ######################################################################### **
** ## Because MacOS doesn't support execution policy yet, we need to make ## **
** ## functions that ignore the first parameter while allowing the        ## **
** ## same parameter to pass through on other targets.                    ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
constexpr static bool par_unseq = false, // Parallel and vectorised disabled
                      par       = false, // Parallel only disabled
                      seq       = false; // Serialised disabled
/* ------------------------------------------------------------------------- */
constexpr static auto StdSort(auto&, auto &&...aArgs)
  { return ::std::sort(StdForward<decltype(aArgs)>(aArgs)...); }
/* ------------------------------------------------------------------------- */
constexpr static auto StdFill(auto&, auto &&...aArgs)
  { return ::std::fill(StdForward<decltype(aArgs)>(aArgs)...); }
/* ------------------------------------------------------------------------- */
constexpr static auto StdRotate(auto&, auto &&...aArgs)
  { return ::std::rotate(StdForward<decltype(aArgs)>(aArgs)...); }
/* ------------------------------------------------------------------------- */
constexpr static auto StdTransform(auto&, auto &&...aArgs)
  { return ::std::transform(StdForward<decltype(aArgs)>(aArgs)...); }
/* ------------------------------------------------------------------------- */
constexpr static auto StdForEach(auto&, auto &&...aArgs)
  { return ::std::for_each(StdForward<decltype(aArgs)>(aArgs)...); }
/* ------------------------------------------------------------------------- */
constexpr static auto StdFindIf(auto&, auto &&...aArgs)
  { return ::std::find_if(StdForward<decltype(aArgs)>(aArgs)...); }
/* ------------------------------------------------------------------------- */
constexpr static auto StdCopy(auto&, auto &&...aArgs)
  { return ::std::copy(StdForward<decltype(aArgs)>(aArgs)...); }
/* ------------------------------------------------------------------------- */
constexpr static auto StdAllOf(auto&, auto &&...aArgs)
  { return ::std::all_of(StdForward<decltype(aArgs)>(aArgs)...); }
/* ------------------------------------------------------------------------- */
#else                                  // Windows or Posix target?
/* ------------------------------------------------------------------------- */
using ::std::execution::par_unseq;     // Parallel and vectorised
using ::std::execution::par;           // Parallel only
using ::std::execution::seq;           // Serialised
/* ------------------------------------------------------------------------- */
constexpr static auto StdSort(auto &&...aArgs)
  { return ::std::sort(StdForward<decltype(aArgs)>(aArgs)...); }
/* ------------------------------------------------------------------------- */
constexpr static auto StdFill(auto &&...aArgs)
  { return ::std::fill(StdForward<decltype(aArgs)>(aArgs)...); }
/* ------------------------------------------------------------------------- */
constexpr static auto StdRotate(auto &&...aArgs)
  { return ::std::rotate(StdForward<decltype(aArgs)>(aArgs)...); }
/* ------------------------------------------------------------------------- */
constexpr static auto StdTransform(auto &&...aArgs)
  { return ::std::transform(StdForward<decltype(aArgs)>(aArgs)...); }
/* ------------------------------------------------------------------------- */
constexpr static auto StdForEach(auto &&...aArgs)
  { return ::std::for_each(StdForward<decltype(aArgs)>(aArgs)...); }
/* ------------------------------------------------------------------------- */
constexpr static auto StdFindIf(auto &&...aArgs)
  { return ::std::find_if(StdForward<decltype(aArgs)>(aArgs)...); }
/* ------------------------------------------------------------------------- */
constexpr static auto StdCopy(auto &&...aArgs)
  { return ::std::copy(StdForward<decltype(aArgs)>(aArgs)...); }
/* ------------------------------------------------------------------------- */
constexpr static auto StdAllOf(auto &&...aArgs)
  { return ::std::all_of(StdForward<decltype(aArgs)>(aArgs)...); }
/* ------------------------------------------------------------------------- */
#endif                                 // MacOS check
/* ------------------------------------------------------------------------- */
#if defined(WINDOWS)                   // Using Windows?
/* -- These don't exist on Windows so we'll make it easier ----------------- */
constexpr static const int R_OK = 4,   // File is readable (StdAccess)
                           W_OK = 2,   // File is writable (StdAccess)
                           X_OK = 1,   // File is executable (StdAccess)
                           F_OK = 0;   // File exists (StdAccess)
/* -- Typedefs for structure names that are different on Windows ----------- */
typedef struct __stat64 StdFStatStruct; // Different on Windows
typedef struct tm       StdTMStruct;   // Different on Windows
typedef __time64_t      StdTimeT;      // Different on Windows
/* -- Typedefs for types not in Windows ------------------------------------ */
typedef make_signed_t<size_t> ssize_t;  // Not in MSVC
/* -- Convert any widestring pointer type to utf8 class string ------------- */
const string S16toUTF(const wchar_t*const wcpStr)
  { return UtfFromWide(wcpStr); }
/* -- Convert STL widestring to utf8 --------------------------------------- */
const string WS16toUTF(const wstring &wstrStr)
  { return S16toUTF(wstrStr.data()); }
/* -- Convert UTF c-string to STL widestring ------------------------------- */
const wstring UTFtoS16(const char*const cpPtr)
  { return UtfDecoder{ cpPtr }.UtfWide(); };
/* -- Convert UTF string to STL widestring --------------------------------- */
const wstring UTFtoS16(const string &strStr)
  { return UtfDecoder{ strStr }.UtfWide(); };
/* -- Convert UTF string view to STL widestring ---------------------------- */
const wstring UTFtoS16(const string_view &strvStr)
  { return UtfDecoder{ strvStr }.UtfWide(); };
/* -- Unset an environment variable ---------------------------------------- */
static bool StdUnSetEnv(const char*const cpEnv)
  { return !_putenv_s(cpEnv, cCommon->CommonCBlank()); }
/* -- Wrapper for _waccess() ----------------------------------------------- */
static int StdAccess(const wchar_t*const wcpPath, const int iMode)
  { return _waccess(wcpPath, iMode); }
/* ------------------------------------------------------------------------- */
static int StdAccess(const wstring &wstrPath, const int iMode)
  { return StdAccess(wstrPath.data(), iMode); }
/* ------------------------------------------------------------------------- */
static int StdAccess(const string &strPath, const int iMode)
  { return StdAccess(UTFtoS16(strPath), iMode); }
/* -- Wrapper for mkdir() function ----------------------------------------- */
static int StdMkDir(const wchar_t*const wcpPath) { return _wmkdir(wcpPath); }
/* ------------------------------------------------------------------------- */
static int StdMkDir(const wstring &wstrPath)
  { return StdMkDir(wstrPath.data()); }
/* ------------------------------------------------------------------------- */
static int StdMkDir(const string &strPath)
  { return StdMkDir(UTFtoS16(strPath)); }
/* -- Wrapper for _wrename() function -------------------------------------- */
static int StdRename(const wchar_t*const wcpSrcPath,
  const wchar_t*const wcpDstPath)
{ return _wrename(wcpSrcPath, wcpDstPath); }
/* -- Wrapper for _wrename() function (wstring version) -------------------- */
static int StdRename(const wstring &wstrSrcPath, const wstring &wstrDstPath)
  { return StdRename(wstrSrcPath.data(), wstrDstPath.data()); }
/* -- Wrapper for _wrename() function (utf string version) ----------------- */
static int StdRename(const string &strSrcPath, const string &strDstPath)
  { return StdRename(UTFtoS16(strSrcPath), UTFtoS16(strDstPath)); }
/* -- Wrapper for _wrmdir() function --------------------------------------- */
static int StdRmDir(const wchar_t*const wcpPath) { return _wrmdir(wcpPath); }
/* ------------------------------------------------------------------------- */
static int StdRmDir(const wstring &wstrPath)
  { return StdRmDir(wstrPath.data()); }
/* ------------------------------------------------------------------------- */
static int StdRmDir(const string &strPath)
  { return StdRmDir(UTFtoS16(strPath)); }
/* -- Wrapper for _wchdir() function --------------------------------------- */
static int StdChDir(const wchar_t*const wcpPath) { return _wchdir(wcpPath); }
/* ------------------------------------------------------------------------- */
static int StdChDir(const wstring &wstrPath)
  { return StdChDir(wstrPath.data()); }
/* ------------------------------------------------------------------------- */
static int StdChDir(const string &strPath)
  { return StdChDir(UTFtoS16(strPath)); }
/* -- Wrapper for _wunlink() function -------------------------------------- */
static int StdUnlink(const wchar_t*const wcpPath) { return _wunlink(wcpPath); }
/* ------------------------------------------------------------------------- */
static int StdUnlink(const wstring &wstrPath)
  { return StdUnlink(wstrPath.data()); }
/* ------------------------------------------------------------------------- */
static int StdUnlink(const string &strPath)
  { return StdUnlink(UTFtoS16(strPath)); }
/* -- Wrapper for _wexecve() stdlib function ------------------------------- */
static int StdExecVE(const wchar_t*const wcpaArg[],
  const wchar_t*const wcpaEnv[])
{ return static_cast<int>(_wexecve(*wcpaArg, wcpaArg, wcpaEnv)); }
/* -- Wrapper for _wspawnve() stdlib function ------------------------------ */
static int StdSpawnVE(const wchar_t*const wcpaArg[],
  const wchar_t*const wcpaEnv[])
{ return static_cast<int>(_wspawnve(P_NOWAIT,
    *wcpaArg, wcpaArg, wcpaEnv) == -1); }
/* -- Wrapper for _fileno() function --------------------------------------- */
static int StdFileNo(FILE*const fStream) { return _fileno(fStream); }
/* -- Wrapper for _wstat64() function -------------------------------------- */
static int StdFStat(const wchar_t*const wcpPath,
  StdFStatStruct*const sDestBuffer)
{ return _wstat64(wcpPath, sDestBuffer);  }
/* ------------------------------------------------------------------------- */
static int StdFStat(const wstring &wstrPath,
  StdFStatStruct*const sDestBuffer)
{ return StdFStat(wstrPath.data(), sDestBuffer);  }
/* ------------------------------------------------------------------------- */
static int StdFStat(const string &strPath,
  StdFStatStruct*const sDestBuffer)
{ return StdFStat(UTFtoS16(strPath), sDestBuffer);  }
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
static FILE *StdPOpen(const wchar_t*const wcpCommand,
  const wchar_t*const wcpType=L"rt") { return _wpopen(wcpCommand, wcpType); }
/* ------------------------------------------------------------------------- */
static FILE *StdPOpen[[maybe_unused]](const wstring &wstrCommand,
  const wchar_t*const wcpType=L"rt")
{ return StdPOpen(wstrCommand.data(), wcpType); }
/* ------------------------------------------------------------------------- */
static FILE *StdPOpen[[maybe_unused]](const string &strCommand,
  const wchar_t*const wcpType=L"rt")
{ return StdPOpen(UTFtoS16(strCommand), wcpType); }
/* -- Wrapper for _pclose() function --------------------------------------- */
static int StdPClose[[maybe_unused]](FILE*const fStream)
  { return _pclose(fStream); }
/* -- Wrapper for srandom() function --------------------------------------- */
static void StdSRand(const unsigned int uiSeed) { srand(uiSeed); }
/* ------------------------------------------------------------------------- */
#else                                  // Posix compatible target?
/* -- Maximum path part lengths -------------------------------------------- */
constexpr static const size_t          // For compatibility with Windows
  _MAX_PATH  = PATH_MAX,               // Maximum size of all path parts
  _MAX_DIR   = PATH_MAX,               // Maximum size of directory part
  _MAX_FNAME = PATH_MAX,               // Maximum size of filename part
  _MAX_EXT   = PATH_MAX;               // Maximum size of extension part
/* -- Other cross compatibilities ------------------------------------------ */
constexpr static const unsigned int    // For compatibility with Windows
  _S_IFDIR           = S_IFDIR,        // Specifies a directory for stat()
  MB_SYSTEMMODAL     = 0,              // No window was available for message
  MB_ICONINFORMATION = 1,              // Informational message only
  MB_ICONEXCLAMATION = 2,              // A warning message occured
  MB_ICONSTOP        = 4;              // A critical error message occured
/* -- Typedefs for structure names that are different on Windows ----------- */
typedef struct stat StdFStatStruct;    // Different on Windows
typedef struct tm   StdTMStruct;       // Different on Windows
typedef time_t      StdTimeT;          // Different on Windows
/* -- Convert widestring to utf8 (n/a on posix systems) -------------------- */
constexpr const char *S16toUTF(const char*const cpPtr) { return cpPtr; }
/* -- Unset an environment variable ---------------------------------------- */
static bool StdUnSetEnv(const char*const cpEnv) { return !unsetenv(cpEnv); }
/* -- Wrapper for access() stdlib function --------------------------------- */
static int StdAccess(const char*const cpPath, const int iMode)
  { return access(cpPath, iMode); }
/* ------------------------------------------------------------------------- */
static int StdAccess(const string &strPath, const int iMode)
  { return StdAccess(strPath.data(), iMode); }
/* -- Wrapper for mkdir() function ----------------------------------------- */
static int StdMkDir(const char*const cpPath)
  { return mkdir(cpPath,
      S_IRUSR|S_IWUSR|S_IXUSR|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH); }
/* ------------------------------------------------------------------------- */
static int StdMkDir(const string &strPath) { return StdMkDir(strPath.data()); }
/* -- Wrapper for rename() function ---------------------------------------- */
static int StdRename(const char*const cpSrcPath, const char*const cpDstPath)
  { return rename(cpSrcPath, cpDstPath); }
/* ------------------------------------------------------------------------- */
static int StdRename(const string &strSrcPath, const string &strDstPath)
  { return StdRename(strSrcPath.data(), strDstPath.data()); }
/* -- Wrapper for rmdir() function ----------------------------------------- */
static int StdRmDir(const char*const cpPath) { return rmdir(cpPath); }
/* ------------------------------------------------------------------------- */
static int StdRmDir(const string &strPath) { return StdRmDir(strPath.data()); }
/* -- Wrapper for chdir() function ----------------------------------------- */
static int StdChDir(const char*const cpPath) { return chdir(cpPath); }
/* ------------------------------------------------------------------------- */
static int StdChDir(const string &strPath) { return StdChDir(strPath.data()); }
/* -- Wrapper for unlink() function ---------------------------------------- */
static int StdUnlink(const char*const cpPath) { return unlink(cpPath); }
/* ------------------------------------------------------------------------- */
static int StdUnlink(const string &strPath)
  { return StdUnlink(strPath.data()); }
/* -- Wrapper for execve() stdlib function --------------------------------- */
static int StdExecVE(const char*const cpaArg[], const char*const cpaEnv[])
  { return execve(*cpaArg, const_cast<char**>(cpaArg),
      const_cast<char**>(cpaEnv)); }
/* -- Wrapper for posix_spawn() stdlib function ---------------------------- */
static int StdSpawnVE(const char*const cpaArg[], const char*const cpaEnv[])
  { pid_t pId; return posix_spawn(&pId, *cpaArg, nullptr, nullptr,
      const_cast<char**>(cpaArg), const_cast<char**>(cpaEnv)); }
/* -- Wrapper for fileno() function ---------------------------------------- */
static int StdFileNo(FILE*const fStream) { return fileno(fStream); }
/* -- Wrapper for stat() function ------------------------------------------ */
static int StdFStat(const char*const cpPath, StdFStatStruct*const sDestBuffer)
  { return stat(cpPath, sDestBuffer);  }
static int StdFStat(const string &strPath, StdFStatStruct*const sDestBuffer)
  { return StdFStat(strPath.data(), sDestBuffer);  }
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
static FILE *StdPOpen(const char*const cpCommand, const char*const cpType="r")
  { return popen(cpCommand, cpType); }
static FILE *StdPOpen[[maybe_unused]](const string &strCommand,
  const char*const cpType="r")
{ return StdPOpen(strCommand.data(), cpType); }
/* -- Wrapper for pclose() function ---------------------------------------- */
static int StdPClose[[maybe_unused]](FILE*const fStream)
  { return pclose(fStream); }
/* -- Wrapper for srandom() function --------------------------------------- */
static void StdSRand(const unsigned int uiSeed) { srandom(uiSeed); }
/* -- Wrapper for mmap function -------------------------------------------- */
template<typename PtrType>PtrType *StdMMap(void*vpAddr, const size_t stLen,
  const int iProtection, const int iFlags, const int iDescriptor,
  const off_t otOffset)
{ return reinterpret_cast<PtrType*>(Lib::OS::mmap(vpAddr, stLen,
    iProtection, iFlags, iDescriptor, otOffset)); }
/* ------------------------------------------------------------------------- */
#endif                                 // Operating system check
/* -- Some frequently used maximums ---------------------------------------- */
constexpr const unsigned int StdMaxUInt = numeric_limits<unsigned int>::max();
constexpr const uint64_t StdMaxUInt64 = numeric_limits<uint64_t>::max();
constexpr const size_t StdMaxSizeT = numeric_limits<size_t>::max();
constexpr const size_t StdNPos = string::npos;
/* -- Set error number ----------------------------------------------------- */
static void StdSetError(const int iValue) { errno = iValue; }
/* -- Get error number ----------------------------------------------------- */
static int StdGetError() { return errno; }
/* -- Is error number equal to --------------------------------------------- */
static bool StdIsError(const int iValue) { return StdGetError() == iValue; }
/* -- Is error number not equal to ----------------------------------------- */
static bool StdIsNotError(const int iValue) { return !StdIsError(iValue); }
/* -- Uppercases the specified character ----------------------------------- */
template<typename IntType>static char StdToUpper(const IntType itC)
  { return static_cast<char>(toupper(static_cast<int>(itC))); }
/* -- Lowercases the specified character ----------------------------------- */
template<typename IntType>static char StdToLower(const IntType itC)
  { return static_cast<char>(tolower(static_cast<int>(itC))); }
/* -- Returns if character is a whitespace --------------------------------- */
template<typename IntType>
  constexpr static bool StdIsSpace(const IntType itChar)
{ return itChar == ' '; }
/* -- Returns if character is NOT a whitespace ----------------------------- */
template<typename IntType>
  constexpr static bool StdIsNotSpace(const IntType itChar)
{ return !StdIsSpace(itChar); }
/* -- Returns if character is a digit (0-9) -------------------------------- */
template<typename IntType>
  constexpr static bool StdIsDigit(const IntType itChar)
{ return itChar >= '0' && itChar <= '9'; }
/* -- Returns if character is NOT a digit (0-9) ---------------------------- */
template<typename IntType>
  constexpr static bool StdIsNotDigit(const IntType itChar)
{ return !StdIsDigit(itChar); }
/* -- Returns if character is alphanumeric (A-Za-z) ------------------------ */
template<typename IntType>
  constexpr static bool StdIsAlpha(const IntType itChar)
{ return (itChar >= 'A' && itChar <= 'Z') ||
         (itChar >= 'a' && itChar <= 'z'); }
/* -- Returns if character is NOT alphanumeric (A-Za-z) -------------------- */
template<typename IntType>
  constexpr static bool StdIsNotAlpha(const IntType itChar)
{ return !StdIsAlpha(itChar); }
/* -- Returns if character is alphanumeric or numeric (0-9A-Za-z) ---------- */
template<typename IntType>
  constexpr static bool StdIsAlnum(const IntType itChar)
{ return StdIsAlpha(itChar) || StdIsDigit(itChar); }
/* -- Returns if character is NOT alphanumeric or numeric (0-9A-Za-z) ------ */
template<typename IntType>
  constexpr static bool StdIsNotAlnum(const IntType itChar)
{ return !StdIsAlnum(itChar); }
/* -- Return absolute number ----------------------------------------------- */
template<typename IntType=int64_t>
  static IntType StdAbsolute(const IntType itVal)
{ // Check if supplied argument is signed and negate it if true
  if constexpr(is_signed_v<IntType>) return (itVal < 0) ? -itVal : itVal;
  // Else just return the value without any processing
  else return itVal;
}
/* -- Returns if the specified number is a power of two -------------------- */
template<typename IntType=int64_t>static bool StdIntIsPOW2(const IntType itVal)
  { return !((itVal & (itVal - 1)) && itVal); }
/* -- Get the distance between two opposing corners ------------------------ */
template<typename IntType>
  static double StdHypot(const IntType itWidth, const IntType itHeight)
{ return ::std::hypot(itWidth, itHeight); }
/* -- Allocate memory ------------------------------------------------------ */
template<typename AnyType,typename IntType>
  static AnyType *StdAlloc(const IntType itBytes)
{ return reinterpret_cast<AnyType*>
    (::std::malloc(static_cast<size_t>(itBytes))); }
/* -- Re-allocate memory --------------------------------------------------- */
template<typename AnyType,typename IntType>
  static AnyType *StdReAlloc(AnyType*const atPtr, const IntType itBytes)
{ return reinterpret_cast<AnyType*>
    (::std::realloc(reinterpret_cast<void*>(atPtr),
      static_cast<size_t>(itBytes))); }
/* -- Release allocated memory --------------------------------------------- */
template<typename AnyType>static void StdFree(AnyType*const atPtr)
  { ::std::free(reinterpret_cast<void*>(atPtr)); }
/* -- Compare memory ------------------------------------------------------- */
static int StdCompare(const void*const vpA, const void*const vpB,
  const size_t stSize)
{ return ::std::memcmp(vpA, vpB, stSize); }
/* -- Scan memory for character -------------------------------------------- */
template<typename PtrType=void*>
  static PtrType *StdFindChar(PtrType*const ptPtr, const int iC,
    const size_t stSize)
{ return reinterpret_cast<PtrType*>(::std::memchr(ptPtr, iC, stSize)); }
/* -- Returns number of threads supported by CPU --------------------------- */
static unsigned int StdThreadMax()
  { return ::std::thread::hardware_concurrency(); }
/* -- Returns current thread id -------------------------------------------- */
static auto StdThreadId() { return ::std::this_thread::get_id(); }
/* -- Returns current thread id -------------------------------------------- */
constexpr static void StdSuspend(const auto &aTime)
  { ::std::this_thread::sleep_for(aTime); }
/* ------------------------------------------------------------------------- */
constexpr static void StdSuspend()
  { StdSuspend(::std::chrono::milliseconds{ 1 }); }
/* -- Returns true if two numbers are equal (Omit != and == warnings) ------ */
template<typename FloatType> requires is_floating_point_v<FloatType>
  static bool StdIsFloatEqual(const FloatType ft1, const FloatType ft2,
    const FloatType ftEpsilon=static_cast<FloatType>(1e-6))
{ return ::std::fabs(ft1 - ft2) < ftEpsilon; }
/* ------------------------------------------------------------------------- */
template<typename FloatType> requires is_floating_point_v<FloatType>
  static bool StdIsFloatNotEqual(const FloatType ft1, const FloatType ft2,
    const FloatType ftEpsilon=static_cast<FloatType>(1e-6f))
{ return !StdIsFloatEqual<FloatType>(ft1, ft2, ftEpsilon); }
/* ------------------------------------------------------------------------- **
** ######################################################################### **
** ## Because some compilers may not allow me to alias ::std::move        ## **
** ## anymore with an error referring to being an 'unqualified call', the ## **
** ## only workaround seems to be to use this constexpr function which    ## **
** ## does not appear to cause an increase of machine code. We can also   ## **
** ## make it only allow classes since we don't need it for integrals   . ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
template<class AnyType, typename AnyTypeRR = remove_reference_t<AnyType>>
requires is_class_v<AnyTypeRR> &&
        (is_reference_v<AnyType> || is_same_v<AnyType, AnyTypeRR>)
  constexpr static AnyTypeRR &&StdMove(AnyType &&atVar) noexcept
{ return static_cast<AnyTypeRR&&>(atVar); }
/* == Static class try/catch helpers ======================================= **
** ######################################################################### **
** ## Don't put try/catch on func level. (C++ ISO/IEC JTC 1/SC 22 N 4411) ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
#define DTORHELPER(c,...) c() noexcept(false) { \
  try { __VA_ARGS__; } catch(const exception &eReason) \
    { cLog->LogWarningExSafe("(" STR(c) ") $", eReason); } }
/* == Z-Lib requirements =================================================== */
#if defined(ALPHA)                     // Z-Lib debug version requires this
extern "C" { int z_verbose = 0, z_error = 0; }
#endif                                 // Compiling DEBUG/ALPHA version
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
