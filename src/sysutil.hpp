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
using namespace ICommon::P;            using namespace IDir::P;
using namespace IStd::P;               using namespace IStdLib::P;
using namespace IString::P;            using namespace IUtf::P;
using namespace IUtil::P;              using namespace Lib::OS;
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
};/* ----------------------------------------------------------------------- */
/* -- Includes ------------------------------------------------------------- */
#if defined(WINDOWS)                   // Using windows?
/* -- System error formatter with specified error code --------------------- */
static StdString SysError(const int iError)
{ // Convert int to DWORD as we use the same function type across platforms
  const DWORD dwError = static_cast<DWORD>(iError);
  // Flags we will be using
  constexpr const DWORD dwFlags =
    FORMAT_MESSAGE_ALLOCATE_BUFFER |   // Let Windows allocate the buffer
    FORMAT_MESSAGE_FROM_SYSTEM |       // Let Windows find the error message
    FORMAT_MESSAGE_IGNORE_INSERTS;     // Ignore inserts like %1, %2, etc.
  // Use default language id
  constexpr const DWORD dwLanguage = MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT);
  // Format the system error code and if we wrote something?
  LPWSTR lpszError = nullptr;
  if(const DWORD dwLen = FormatMessage(dwFlags, nullptr, dwError, dwLanguage,
       reinterpret_cast<LPWSTR>(&lpszError), 0, nullptr)) try
  { // Convert the output to UTF8 string, free the allocated buffer and
    // Remove any carriage returns and line feeds and return the string with
    // any carriage returns and line feeds chopped off.
    StdString strOut{ S16toUTF(StdWideStringView{
      lpszError, static_cast<size_t>(dwLen) }) };
    LocalFree(lpszError);
    return StrChopRef(strOut);
  } // Exception occurred whilst building error string?
  catch(const StdException &eReason)
  { // Free the string if allocated and assign code and exception as output
    if(lpszError) LocalFree(lpszError);
    return StrFormat("SE#$/XC($)", dwError, eReason);
  } // Error occurred so just stringify the error code and return it
  else return StrAppend("SE#", dwError);
}
/* -- System error code ---------------------------------------------------- */
template<typename IntType = int>
  requires StdIsIntegral<IntType>
static IntType SysErrorCode() { return static_cast<IntType>(GetLastError()); }
/* -- System error formatter with current error code ----------------------- */
static StdString SysError() { return SysError(SysErrorCode()); }
/* -- Actual interface to MessageBoxExW ------------------------------------ */
static unsigned SysMessage(void*const vpHandle, const StdString &strTitle,
  const StdString &strMessage, const unsigned uFlags)
    { return static_cast<unsigned>(
        MessageBoxExW(reinterpret_cast<HWND>(vpHandle),
          UTFtoS16(strMessage).data(), UTFtoS16(strTitle).data(),
          static_cast<DWORD>(uFlags), 0)); }
/* -- Set thread priority -------------------------------------------------- */
static bool SysSetThreadPriority(const SysThread stLevel)
{ // STP_* id to priority lookup table
  static const StdArray<const int, STP_MAX> aValues{
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
  // This next bit of code is required for debugger which causes a warning but
  // on CLang-CL but however is still parsed and machine code emitted.
#if defined(__clang__)
# pragma clang diagnostic ignored "-Wlanguage-extension-token"
# pragma clang diagnostic push
#endif
  // Send message to debugger to name the thread
  __try {
    RaiseException(0x406D1388, 0,
      sizeof(tInfo) / sizeof(ULONG_PTR),
      reinterpret_cast<ULONG_PTR*>(&tInfo)); }
  __except(EXCEPTION_CONTINUE_EXECUTION) {}
  // Restore original warning
#if defined(__clang__)
# pragma clang diagnostic pop
#endif
}
/* ------------------------------------------------------------------------- */
#elif defined(MACOS)                   // Using mac?
/* -- Actual interface to show a message box ------------------------------- */
static unsigned SysMessage(void*const, const StdString &strTitle,
  const StdString &strMessage, const unsigned uFlags)
{ // Make an autorelease ptr for Apple strings. Not sure if Apple provides a
  // non-pointer based CStringRef so we'll just remove it instead!
  using CFAutoRelPtr = StdUniquePtr<const void, function<decltype(CFRelease)>>;
  // Setup dialogue title string with autorelease and if succeeded?
  if(const CFAutoRelPtr csrTitle{
    CFStringCreateWithCString(kCFAllocatorDefault, strTitle.data(),
      kCFStringEncodingUTF8), CFRelease })
    // Setup dialogue message string with autorelease and if succeeded?
    if(const CFAutoRelPtr csrMessage{
      CFStringCreateWithCString(kCFAllocatorDefault, strMessage.data(),
        kCFStringEncodingUTF8), CFRelease })
      // Setup button text string with autorelease and if succeeded?
      if(const CFAutoRelPtr csrButton{
        CFStringCreateWithCString(kCFAllocatorDefault, "Quit Application",
          kCFStringEncodingUTF8), CFRelease })
      { // Setup keys for dictionary
        StdArray<const void*,3>
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
              (uFlags & MB_ICONSTOP) ?
                kCFUserNotificationStopAlertLevel
            :((uFlags & MB_ICONEXCLAMATION) ?
                kCFUserNotificationCautionAlertLevel
            :((uFlags & MB_ICONINFORMATION) ?
                kCFUserNotificationPlainAlertLevel
            : kCFUserNotificationNoteAlertLevel)), &nRes,
              reinterpret_cast<CFDictionaryRef>(cfdrDict.get())),
              CFRelease })
            return static_cast<unsigned>(nRes);
        }
      }
  // Didn't work so put in stdout
  StdWcErr << UtfDecoder{ strTitle }.UtfWide() << ": "
           << UtfDecoder{ strMessage }.UtfWide() << StdIOSEndLine;
  // If exited successfully? Return success
  return 0;
}
/* -- Set thread priority -------------------------------------------------- */
static bool SysSetThreadPriority(const SysThread stLevel)
{ // Get this thread handle
  pthread_t ptHandle = pthread_self();
  // Set qos
  pthread_set_qos_class_self_np(stLevel <= STP_ENGINE ?
    QOS_CLASS_USER_INTERACTIVE : QOS_CLASS_BACKGROUND, 0);
  // Get requested thread policy level and priority fraction adjustment
  static const StdArray<const float, STP_MAX>
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
    {}                                // __opaque (MacOS only)
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
static unsigned SysMessage(void*const, StdString strTitle,
  StdString strMessage, const unsigned uFlags)
{ // Print the error in console
  fprintf(stderr, "%s: %s\n", strTitle.data(), strMessage.data());
  // Eligable directories for dialog box elf binaries
  const StdArray<const StdStringView, 10> strvaDirPrefixes{
    "/bin/",             "/usr/bin/",        "/usr/sbin/",
    "/usr/local/bin/",   "/usr/local/sbin/", "/usr/games/",
    "/usr/local/games/", "/snap/bin/",       "/var/lib/flatpak/exports/bin/",
    "/run/current-system/sw/bin/" };
  // Dialog box applications we can use
  struct DlgBoxApplication {
    const StdStringView strvElf, strvCompulsoryParam,
                      strvTitleParam, strvMessageParam; };
  // The dialog box elf binary database
  const StdArray<const DlgBoxApplication, 5> dbaaApps{ {
    { "yad",                   cCommon->CommonBlank(),
      "--title=",              "--text=" },
    { "zenity",                "--info --no-markup",
      "--title=",              "--text=" },
    { "kdialog",               cCommon->CommonBlank(),
      "--title ",              "--msgbox " },
    { "gxmessage",             "-center -buttons OK:0",
      cCommon->CommonBlank(),  cCommon->CommonSpace() },
    { "xmessage",              "-center",
      cCommon->CommonBlank(),  cCommon->CommonSpace() }
  } }; // Command-line safety replacements
  const StrPairList splReplace
    { { "\"", "\\\"" }, { "'", "\\\'" }, { "`", "\\`" } };
  StrReplaceExRef(strTitle, splReplace);
  StrReplaceExRef(strMessage, splReplace);
  // Search for one of these apps now
  for(const DlgBoxApplication &dbaApp : dbaaApps)
  { // In one of these directories
    for(const StdStringView &strvDir : strvaDirPrefixes)
    { // Build filename and ignore if not exist, readable or executable
      const StdString strPath{ StrAppend(strvDir, dbaApp.strvElf) };
      if(!DirCheckFileAccess(strPath, F_OK|R_OK|X_OK)) continue;
      // Build command line
      const StdString strCmdLine{ StrFormat("$ $ $ $",
        strPath, dbaApp.strvCompulsoryParam,
       (dbaApp.strvTitleParam.empty() ?
          cCommon->CommonBlank() : StrFormat("$\"$\"",
            dbaApp.strvTitleParam, strTitle)),
       (dbaApp.strvMessageParam.empty() ?
          cCommon->CommonBlank() : StrFormat("$\"$\"",
            dbaApp.strvMessageParam, strMessage))) };
      // Now execute and break if successful
      if(!system(strCmdLine.data())) return 0;
    }
  } // Return status code
  return 0;
}
/* -- Set thread priority (pthread_setschedparam() requires root) ---------- */
static bool SysSetThreadPriority(const SysThread stPriority) { return true; }
/* ------------------------------------------------------------------------- */
#endif                                 // Done checking OS
/* ------------------------------------------------------------------------- */
#if defined(WINDOWS)                   // Windows is defined?
/* -- System message without a handle -------------------------------------- */
static unsigned SysMessage(const StdString &strTitle,
  const StdString &strMessage, const unsigned uFlags)
    { return SysMessage(nullptr, strTitle, strMessage,
        MB_SYSTEMMODAL|uFlags); }
/* ------------------------------------------------------------------------- */
#else                                  // Not using Windows target? (POSIX)
/* -- System error code ---------------------------------------------------- */
template<typename IntType = int>
  requires StdIsIntegral<IntType>
static IntType SysErrorCode() { return static_cast<IntType>(StdGetError()); }
/* -- System error formatter with specified error code --------------------- */
static StdString SysError(const int iError) { return StrFromErrNo(iError); }
/* -- System error formatter with current error code ----------------------- */
static StdString SysError() { return StrFromErrNo(SysErrorCode()); }
/* ------------------------------------------------------------------------- */
static void SysSetThreadName(const char*const cpName)
{ // Set thread name which helps a little with debugging
  pthread_setname_np(
#if defined(LINUX)                     // Linux?
    pthread_self(),                    // Requires handle
#endif                                 // Not on MacOS
    cpName);
}
/* -- System message without a handle -------------------------------------- */
static unsigned SysMessage(const StdString &strTitle,
  const StdString &strMessage, const unsigned uFlags)
{ return SysMessage(nullptr, strTitle, strMessage, uFlags); }
/* ------------------------------------------------------------------------- */
#endif                                 // Not using Windows target
/* ------------------------------------------------------------------------- */
struct SysErrorPlugin final
{ /* -- Exception class helper macro for system errors --------------------- */
#define XCS(r,...) throw Error<SysErrorPlugin>(r, ## __VA_ARGS__)
  /* -- Constructor to add system error code ------------------------------- */
  explicit SysErrorPlugin(StdOStringStream &osS)
  { // Get system error code and add system formatted parameter
    const int iCode = SysErrorCode();
    osS << "\n+ Reason<" << iCode << "> = \"" << SysError(iCode) << "\".";
  }
};
/* -- Unset multiple environment variables --------------------------------- */
static void SysUnSetEnv() {}
template<typename ...VarArgs>
  static void SysUnSetEnv(const char*const cpEnv, VarArgs &&...vaArgs)
{ StdUnSetEnv(cpEnv); SysUnSetEnv(StdForward<VarArgs>(vaArgs)...); }
/* ------------------------------------------------------------------------- */
static bool SysIsErrorCode(const int iCode=0)
  { return SysErrorCode() == iCode; }
/* ------------------------------------------------------------------------- */
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
