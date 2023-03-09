/* == SYSUTIL.HPP ========================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## This is the header to define operating system level critical        ## **
** ## utility functions.                                                  ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace ISysUtil {                   // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IStd::P;               using namespace IString::P;
using namespace IUtf;                  using namespace IUtil::P;
using namespace Lib::OS;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* ------------------------------------------------------------------------- */
enum SysThread : size_t                // Thread priority types
{ /* ----------------------------------------------------------------------- */
  STP_MAIN,                            // Reserved for main thread
  STP_ENGINE,                          // Reserved for engine thread
  STP_AUDIO,                           // Reserved for audio thread
  STP_HIGH,                            // Aux thread high priority
  STP_LOW,                             // Aux thread low priority
  /* ----------------------------------------------------------------------- */
  STP_MAX                              // Maximum number of types
};/* -- Includes ----------------------------------------------------------- */
#if defined(WINDOWS)                   // Using windows?
/* -- System error formatter with specified error code --------------------- */
static const string SysError(const int iError)
{ // Convert int to DWORD as we use the same function type across platforms
  const DWORD dwError = static_cast<DWORD>(iError);
  // Pointer to string
  LPWSTR lpszError = nullptr;
  // Capture exceptions
  try
  { // Format the system error code and catch result
    switch(const DWORD dwLen = FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM|
      FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_IGNORE_INSERTS, nullptr,
      dwError, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        reinterpret_cast<LPWSTR>(&lpszError), 0, nullptr))
    { // Failed and no allocation was made so just store error code
      case 0: return StrAppend("SE#", dwError);
      // We can remove carriage return and free the string (fall through)
      default: lpszError[dwLen - 3] = '\0';
      // Just incase we don't have enough characters
      case 1: case 2: const string strOut(S16toUTF(lpszError));
                      LocalFree(lpszError);
                      return strOut;
    }
  } // exception occured
  catch(const exception &E)
  { // Free the string if allocated
    if(lpszError) LocalFree(lpszError);
    // Assign the exception message as the error
    return StrFormat("SE#$($)", dwError, E.what());
  }
}
/* -- System error code ---------------------------------------------------- */
template<typename IntType=int>static IntType SysErrorCode(void)
  { return static_cast<IntType>(GetLastError()); }
/* -- System error formatter with current error code ----------------------- */
static const string SysError(void) { return SysError(SysErrorCode()); }
/* -- Actual interface to MessageBoxExW ------------------------------------ */
static unsigned int SysMessage(void*const vpHandle, const string &strTitle,
  const string &strMessage, const unsigned int uiFlags)
    { return static_cast<unsigned int>(
        MessageBoxExW(reinterpret_cast<HWND>(vpHandle),
          UTFtoS16(strMessage).c_str(), UTFtoS16(strTitle).c_str(),
          static_cast<DWORD>(uiFlags), 0)); }
/* -- Set thread priority -------------------------------------------------- */
static bool SysSetThreadPriority(const SysThread stLevel)
{ // STP_* id to priority lookup table
  static const array<const int, STP_MAX> aValues{
    THREAD_PRIORITY_ABOVE_NORMAL,      // STP_MAIN
    THREAD_PRIORITY_HIGHEST,           // STP_ENGINE
    THREAD_PRIORITY_BELOW_NORMAL,      // STP_AUDIO
    THREAD_PRIORITY_NORMAL,            // STP_HIGH
    THREAD_PRIORITY_LOWEST,            // STP_LOW
  }; // Set thread priorty and return result
  return !!SetThreadPriority(GetCurrentThread(), aValues[stLevel]);
}
/* ------------------------------------------------------------------------- */
static void SysSetThreadName(const char*const cpName)
{ // Keep structure aligned
#pragma pack(push, 8)
  struct WinDBGThreadData {
    DWORD  dwType;                     // Must be 0x1000.
    LPCSTR szName;                     // Pointer to name (in user addr space).
    DWORD  dwThreadID;                 // Thread ID (-1=caller thread).
    DWORD  dwFlags;                    // Reserved for future use, keep zero.
  } // Initialiser
  tInfo { 0x1000, cpName, static_cast<DWORD>(-1), 0 }; // Was ~DWORD{0}
#pragma pack(pop)
  // Send message to debugger to name the thread
  __try { RaiseException(0x406D1388, 0,
            sizeof(tInfo)/sizeof(ULONG_PTR), (ULONG_PTR*)&tInfo); }
  __except(EXCEPTION_CONTINUE_EXECUTION) { }
}
/* ------------------------------------------------------------------------- */
#elif defined(MACOS)                   // Using mac?
/* -- Actual interface to show a message box ------------------------------- */
static unsigned int SysMessage(void*const, const string &strTitle,
  const string &strMessage, const unsigned int uiFlags)
{ // Make an autorelease ptr for Apple strings. Not sure if Apple provides a
  // non-pointer based CStringRef so we'll just remove it instead!
  typedef unique_ptr<const void, function<decltype(CFRelease)>> CFAutoRelPtr;
  // Setup dialogue title string with autorelease and if succeeded?
  if(const CFAutoRelPtr csrTitle{
    CFStringCreateWithCString(kCFAllocatorDefault, strTitle.c_str(),
      kCFStringEncodingUTF8), CFRelease })
    // Setup dialogue message string with autorelease and if succeeded?
    if(const CFAutoRelPtr csrMessage{
      CFStringCreateWithCString(kCFAllocatorDefault, strMessage.c_str(),
        kCFStringEncodingUTF8), CFRelease })
      // Setup button text string with autorelease and if succeeded?
      if(const CFAutoRelPtr csrButton{
        CFStringCreateWithCString(kCFAllocatorDefault, "Quit Application",
          kCFStringEncodingUTF8), CFRelease })
      { // Setup keys for dictionary
        array<const void*,3>
          vpKeys{ kCFUserNotificationAlertHeaderKey,
                  kCFUserNotificationAlertMessageKey,
                  kCFUserNotificationDefaultButtonTitleKey },
          // Setup values for dictionary
          vpVals{ csrTitle.get(),
                  csrMessage.get(),
                  csrButton.get() };
        // Create dictionary
        if(const CFAutoRelPtr cfdrDict{
          CFDictionaryCreate(nullptr, vpKeys.data(), vpVals.data(),
            vpKeys.size(), &kCFTypeDictionaryKeyCallBacks,
            &kCFTypeDictionaryValueCallBacks),
            CFRelease })
        { // Holds the result
          SInt32 nRes = 0;
          // Dispatch the message box and return result if successful
          if(const CFAutoRelPtr pDlg{
            CFUserNotificationCreate(kCFAllocatorDefault, 0,
              (uiFlags & MB_ICONSTOP) ?
                kCFUserNotificationStopAlertLevel
            :((uiFlags & MB_ICONEXCLAMATION) ?
                kCFUserNotificationCautionAlertLevel
            :((uiFlags & MB_ICONINFORMATION) ?
                kCFUserNotificationPlainAlertLevel
            : kCFUserNotificationNoteAlertLevel)), &nRes,
              reinterpret_cast<CFDictionaryRef>(cfdrDict.get())),
              CFRelease })
            return static_cast<unsigned int>(nRes);
        }
      }
  // Didn't work so put in stdout
  fwprintf(stderr, L"%ls: %ls\n", UtfDecoder{ strTitle }.Wide().c_str(),
                                  UtfDecoder{ strMessage }.Wide().c_str());
  // If exited successfully? Return success
  return 0;
}
/* -- Unset multiple environment variables --------------------------------- */
static void SysUnSetEnv(void) { }
template<typename ...VarArgs>
  static void SysUnSetEnv(const char*const cpEnv, const VarArgs &...vaVars)
    { unsetenv(cpEnv); SysUnSetEnv(vaVars...); }
/* -- Set thread priority -------------------------------------------------- */
static bool SysSetThreadPriority(const SysThread stLevel)
{ // Get this thread handle
  pthread_t ptHandle = pthread_self();
  // Set qos
  pthread_set_qos_class_self_np(stLevel <= STP_ENGINE ?
    QOS_CLASS_USER_INTERACTIVE : QOS_CLASS_BACKGROUND, 0);
  // Get requested thread policy level and priority fraction adjustment
  static const array<const float, STP_MAX>
    aValues{ 0.75 /* STP_MAIN */, 1.00 /* STP_ENGINE */, 0.50 /* STP_AUDIO */,
             0.25 /* STP_HIGH */, 0.00 /* STP_LOW */ };
  const float fFraction = aValues[stLevel];
  // Use round-robin policy
  const int iPolicy = SCHED_RR;
  // Get min and max priority for policy and the valid range. On MacOS this
  // range is 15-47 and on Linux (Ubuntu at least) it is 1-99.
  const float fMin = static_cast<float>(sched_get_priority_min(iPolicy)),
              fMax = static_cast<float>(sched_get_priority_max(iPolicy));
  // Calculate the priority by fractioning the range
  struct sched_param spParam{
    static_cast<int>(floorf(fMax - ((fMax - fMin) * fFraction))),
    { }                                // __opaque (MacOS only)
  };
  // Set the new parameters and return true if succeeded
  return !pthread_setschedparam(ptHandle, iPolicy, &spParam);
}
/* ------------------------------------------------------------------------- */
#else                                  // Using linux?
/* -- Compatibility with X11 ----------------------------------------------- */
# if defined(Bool)                     // Undefine 'Bool' set by X11
#  undef Bool                          // To prevent problems with other apis
# endif                                // Done checking for 'Bool'
/* -- Actual interface to show a message box ------------------------------- */
static unsigned int SysMessage(void*const, const string &strTitle,
  const string &strMessage, const unsigned int uiFlags)
{ // Print the error first
  fwprintf(stderr, L"%ls: %ls\n", UtfDecoder{ strTitle }.Wide().c_str(),
                                  UtfDecoder{ strMessage }.Wide().c_str());
  // Return status code
  return 0;
}
/* -- Set thread priority -------------------------------------------------- */
static bool SysSetThreadPriority(const SysThread stLevel)
{ // pthread_setschedparam() is pointless on Linux as root is required.
  return true;
}
/* ------------------------------------------------------------------------- */
#endif                                 // Done checking OS
/* ------------------------------------------------------------------------- */
#if defined(WINDOWS)                   // Windows is defined?
/* -- System message without a handle -------------------------------------- */
static unsigned int SysMessage(const string &strTitle,
  const string &strMessage, const unsigned int uiFlags)
    { return SysMessage(nullptr, strTitle, strMessage,
        MB_SYSTEMMODAL|uiFlags); }
/* ------------------------------------------------------------------------- */
#else                                  // Not using Windows target? (POSIX)
/* -- System error code ---------------------------------------------------- */
template<typename IntType=int>static IntType SysErrorCode(void)
  { return static_cast<IntType>(StdGetError()); }
/* -- System error formatter with specified error code --------------------- */
static const string SysError(const int iError) { return StrFromErrNo(iError); }
/* -- System error formatter with current error code ----------------------- */
static const string SysError(void) { return StrFromErrNo(SysErrorCode()); }
/* ------------------------------------------------------------------------- */
static void SysSetThreadName(const char*const cpName)
{ // Set thread name which helps a little with debugging
  pthread_setname_np(
#if defined(LINUX)                     // Linux?
    pthread_self(),                    // Requires handle
#endif
    cpName);
}
/* -- System message without a handle -------------------------------------- */
static unsigned int SysMessage(const string &strTitle,
  const string &strMessage, const unsigned int uiFlags)
    { return SysMessage(nullptr, strTitle, strMessage, uiFlags); }
/* ------------------------------------------------------------------------- */
#endif                                 // Not using Windows target
/* ------------------------------------------------------------------------- */
struct SysErrorPlugin final
{ /* -- Exception class helper macro for system errors --------------------- */
#define XCS(r,...) throw Error<SysErrorPlugin>(r, ## __VA_ARGS__)
  /* -- Constructor to add system error code ------------------------------- */
  explicit SysErrorPlugin(ostringstream &osS)
  { // Get system error code and add system formatted parameter
    const int iCode = SysErrorCode();
    osS << "\n+ Reason<" << iCode << "> = \"" << SysError(iCode) << "\".";
  }
};/* ----------------------------------------------------------------------- */
static bool SysIsErrorCode(const int iCode=0)
  { return SysErrorCode() == iCode; }
static bool SysIsNotErrorCode[[maybe_unused]](const int iCode=0)
  { return !SysIsErrorCode(iCode); }
/* ------------------------------------------------------------------------- */
static bool SysInitThread(const char*const cpName, const SysThread stLevel)
{ // Set the thread name
  SysSetThreadName(cpName);
  // Set the new parameters and return true if succeeded
  return SysSetThreadPriority(stLevel);
}
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
