/* == LOG.HPP ============================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## This module handles logging of engine events which the user and     ## **
** ## developer can read to help resolve issues.                          ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace ILog {                       // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IClock::P;             using namespace ICVarDef::P;
using namespace IFStream::P;           using namespace IIdent::P;
using namespace IStd::P;               using namespace IString::P;
using namespace ISysUtil::P;           using namespace IToken::P;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* -- Log levels ----------------------------------------------------------- */
enum LHLevel : unsigned int            // Log helper level flags
{ /* ----------------------------------------------------------------------- */
  LH_CRITICAL,                         // Log message is critical
  LH_ERROR,                            // Log message is an error (exception)
  LH_WARNING,                          // Log message is a warning
  LH_INFO,                             // Log message is informational only
  LH_DEBUG,                            // Log message is for debugging
  LH_MAX,                              // Maximum log message level
};/* ----------------------------------------------------------------------- */
struct LogLine                         // Log line structure
{ /* ----------------------------------------------------------------------- */
  const double     dTime;              // The time it happend
  const LHLevel    lhlLevel;           // The type of log entry
  const string     strLine;            // The log entry string
};/* ----------------------------------------------------------------------- */
typedef list<LogLine>            LogLines;        // List of log lines
typedef LogLines::const_iterator LogLinesConstIt; // Log lines iterator
/* == Log class ============================================================ */
static class Log final :
  /* -- Base classes ------------------------------------------------------- */
  public LogLines,                     // Holds info about every log line
  private FStream,                     // Output log file if needed
  public ClockChrono<CoreClock>,       // Holds the current log time
  private mutex                        // Because logger needs thread safe
{ /* -- Private typedefs --------------------------------------------------- */
  typedef IdList<LH_MAX> LogLevels;    // Log levels as human readable strings
  /* -- Private variables -------------------------------------------------- */
  const LogLevels  llLevels;           // Log level strings
  const string     strStdOut,          // Label for 'stdout'
                   strStdErr;          // Label for 'stderr'
  atomic<LHLevel>  lhlLevel;           // Log helper level for this instance
  size_t           stMaximum;          // Maximum log lines
  /* ----------------------------------------------------------------------- */
  void ReserveLines(const size_t stLines)
  { // Calculate total liens
    const size_t stTotal = size() + stLines;
    // If writing this many lines would fit in the log, then the log does not
    // need pruning.
    if(stTotal <= stMaximum) return;
    // If too many lines would be written? Just clear the log
    if(stLines >= stMaximum) return clear();
    // Erase enough lines for the new ones to fit
    erase(begin(), next(begin(), static_cast<ssize_t>(stTotal - stMaximum)));
  }
  /* ----------------------------------------------------------------------- */
  void FlushLog(void)
  { // Ignore if file not opened or there is nothing to write
    if(empty() || FStreamClosed()) return;
    // Get start of log
    LogLinesConstIt llciItem{ cbegin() };
    // Repeat...
    do
    { // Get reference to line
      const LogLine &llLine = *llciItem;
      // Write stored line and if succeeded
      if(FStreamWriteStringEx("[$$$]<$> $\n", fixed, setprecision(6),
        llLine.dTime, LogLevelToString(llLine.lhlLevel).front(),
        llLine.strLine))
      { // Flush the line to file straight away and continue if succeeded
        if(FStreamFlush()) continue;
        // Flush failed so close file and write closure reason
        FStreamClose();
        LogErrorExSafe("Log file closed (flush error: $)!", StrFromErrNo());
      } // Write string failed?
      else
      { // Close file and write closure reason
        FStreamClose();
        LogErrorExSafe("Log file closed (write error: $)!", StrFromErrNo());
      } // Erase the lines we actually wrote. Never erase the first line.
      erase(begin(), llciItem);
      // Do not write anymore lines
      return;
    } // ... until all lines written
    while(++llciItem != end());
    // We wrote all the lines
    clear();
  }
  /* -- Write lines to log ------------------------------------------------- */
  void WriteLines(const LHLevel lhL, const TokenList &tLines) noexcept(true)
  { // Ignore if no lines
    if(tLines.empty()) return;
    // Prune log to fit this many lines
    ReserveLines(tLines.size());
    // For each line, print to log. Note that I tried using string
    // appending for this and it turned out to be almost twice as slow as
    // using formatstring due to the fact that less memory management
    // is required!
    for(const string &strLine : tLines)
      push_back({ CCDeltaToDouble(), lhL, StdMove(strLine) });
    // Write lines to log
    FlushLog();
  }
  /* -- Write string to log. Line feed creates multiple lines -------------- */
  void WriteString(const LHLevel lhL, const string &strL) noexcept(true)
    { WriteLines(lhL, { strL, cCommon->Lf(), stMaximum }); }
  /* ----------------------------------------------------------------------- */
  void WriteString(const string &strL) { WriteString(LH_CRITICAL, strL); }
  /* ----------------------------------------------------------------------- */
  void DeInit(void)
  { // Bail if initialised
    if(FStreamClosed()) return;
    // Log file closure
    WriteString("Log file closed.");
    // Flush all remaining strings
    FlushLog();
    // Done
    FStreamClose();
  }
  /* -- Convert log level to a string -------------------------------------- */
  const string_view &LogLevelToString(const LHLevel lhId)
    { return llLevels.Get(lhId); }
  /* -- Safe access to members ------------------------------------- */ public:
  CVarReturn SetLevel(const LHLevel lhlNewLevel)
  { // Deny if invalid level
    if(lhlNewLevel >= LH_MAX) return DENY;
    // Not same level as current? (when cvar registered/init prevents echo)
    if(lhlNewLevel != lhlLevel)
    { // Set new logging state
      const LHLevel lhOldLevel = lhlLevel;
      lhlLevel = lhlNewLevel;
      // Report state, we could disable logging so we should force report it
      LogNLCDebugExSafe("Log change verbosity from $ ($) to $ ($).",
        LogLevelToString(lhOldLevel), lhOldLevel,
        LogLevelToString(lhlNewLevel), lhlNewLevel);
    } // Success
    return ACCEPT;
  }
  /* ----------------------------------------------------------------------- */
  size_t Clear(void)
  { // Get num log lines for returning
    const size_t stSize = size();
    // Clear the log
    clear();
    // Return number of lines cleared
    return stSize;
  }
  /* ----------------------------------------------------------------------- */
  bool HasLevel(const LHLevel lhReq) const { return lhReq <= lhlLevel; }
  /* ----------------------------------------------------------------------- */
  bool NotHasLevel(const LHLevel lhReq) const { return !HasLevel(lhReq); }
  /* ----------------------------------------------------------------------- */
  LHLevel GetLevel(void) const { return lhlLevel; }
  /* ----------------------------------------------------------------------- */
  mutex &GetMutex(void) { return *this; }
  /* ----------------------------------------------------------------------- */
  bool IsRedirectedToDevice(void)
    { const LockGuard lgLogSync{ GetMutex() }; return FStreamIsHandleStd(); }
  /* ----------------------------------------------------------------------- */
  bool OpenedSafe(void)
    { const LockGuard lgLogSync{ GetMutex() }; return FStreamOpened(); }
  /* ----------------------------------------------------------------------- */
  void DeInitSafe(void)
    { const LockGuard lgLogSync{ GetMutex() }; DeInit(); }
  /* ----------------------------------------------------------------------- */
  const string GetNameSafe(void)
    { const LockGuard lgLogSync{ GetMutex() }; return IdentGet(); }
  /* -- Unformatted logging without level check (specified level) ---------- */
  void LogNLCSafe(const LHLevel lhL, const string& strLine)
    { const LockGuard lgLogSync{ GetMutex() }; WriteString(lhL, strLine); }
  /* -- Unformatted logging without level check (error level) -------------- */
  void LogNLCErrorSafe(const string& strLine)
    { LogNLCSafe(LH_ERROR, strLine); }
  /* -- Unformatted logging without level check (warning level) ------------ */
  void LogNLCWarningSafe(const string& strLine)
    { LogNLCSafe(LH_WARNING, strLine); }
  /* -- Unformatted logging without level check (info level) --------------- */
  void LogNLCInfoSafe(const string& strLine)
    { LogNLCSafe(LH_INFO, strLine); }
  /* -- Unformatted logging without level check (debug level) -------------- */
  void LogNLCDebugSafe(const string& strLine)
    { LogNLCSafe(LH_DEBUG, strLine); }
  /* -- Formatted logging without level check (specified level) ------------ */
  template<typename ...VarArgs>void LogNLCExSafe(const LHLevel lhLev,
    const char*const cpFormat, const VarArgs &...vaArgs)
      { LogNLCSafe(lhLev, StrFormat(cpFormat, vaArgs...)); }
  /* -- Formatted logging without level check (error level) ---------------- */
  template<typename ...VarArgs>
    void LogNLCErrorExSafe(const char*const cpFormat, const VarArgs &...vaArgs)
      { LogNLCExSafe(LH_ERROR, cpFormat, vaArgs...); }
  /* -- Formatted logging without level check (warning level) -------------- */
  template<typename ...VarArgs>
    void LogNLCWarningExSafe(const char*const cpFormat,
      const VarArgs &...vaArgs)
        { LogNLCExSafe(LH_WARNING, cpFormat, vaArgs...); }
  /* -- Formatted logging without level check (info level) ----------------- */
  template<typename ...VarArgs>
    void LogNLCInfoExSafe(const char*const cpFormat, const VarArgs &...vaArgs)
      { LogNLCExSafe(LH_INFO, cpFormat, vaArgs...); }
  /* -- Formatted logging without level check (debug level) ---------------- */
  template<typename ...VarArgs>
    void LogNLCDebugExSafe(const char*const cpFormat, const VarArgs &...vaArgs)
      { LogNLCExSafe(LH_DEBUG, cpFormat, vaArgs...); }
  /* -- Formatted logging with level check (specified level) --------------- */
  template<typename ...VarArgs>void LogExSafe(const LHLevel lhL,
    const char*const cpFormat, const VarArgs &...vaArgs)
  { // Return if we don't have this level
    if(NotHasLevel(lhL)) return;
    // Write formatted string
    LogNLCExSafe(lhL, cpFormat, vaArgs...);
  }
  /* -- Formatted logging with level check (error level) ------------------- */
  template<typename ...VarArgs>
    void LogErrorExSafe(const char*const cpFormat, const VarArgs &...vaArgs)
      { LogExSafe(LH_ERROR, cpFormat, vaArgs...); }
  /* -- Formatted logging with level check (warning level) ----------------- */
  template<typename ...VarArgs>
    void LogWarningExSafe(const char*const cpFormat, const VarArgs &...vaArgs)
      { LogExSafe(LH_WARNING, cpFormat, vaArgs...); }
  /* -- Formatted logging with level check (info level) -------------------- */
  template<typename ...VarArgs>
    void LogInfoExSafe(const char*const cpFormat, const VarArgs &...vaArgs)
      { LogExSafe(LH_INFO, cpFormat, vaArgs...); }
  /* -- Formatted logging with level check --------------------------------- */
  template<typename ...VarArgs>
    void LogDebugExSafe(const char*const cpFormat, const VarArgs &...vaArgs)
      { LogExSafe(LH_DEBUG, cpFormat, vaArgs...); }
  /* -- Unformatted logging with level check (specified level) ------------- */
  void LogSafe(const LHLevel lhL, const string& strLine)
    { if(NotHasLevel(lhL)) return; LogNLCSafe(lhL, strLine); }
  /* -- Unformatted logging with level check (error level) ----------------- */
  void LogErrorSafe(const string& strLine) { LogSafe(LH_ERROR, strLine); }
  /* -- Unformatted logging with level check (warning level) --------------- */
  void LogWarningSafe(const string& strLine) { LogSafe(LH_WARNING, strLine); }
  /* -- Unformatted logging with level check (info level) ------------------ */
  void LogInfoSafe(const string& strLine) { LogSafe(LH_INFO, strLine); }
  /* -- Unformatted logging with level check (debug level) ----------------- */
  void LogDebugSafe(const string& strLine) { LogSafe(LH_DEBUG, strLine); }
  /* -- Return buffer lines for debugger ----------------------------------- */
  void GetBufferLines(ostringstream &osS)
  { // Gain exclusive access to log lines
    const LockGuard lgLogSync{ GetMutex() };
    // For each log entry, write the line to the buffer
    for(const LogLine &llLine : *this) osS
      << '[' << fixed << setprecision(6) << llLine.dTime << "] "
      << llLine.strLine << '\n';
  }
  /* -- Initialise log to built-in standard output ------------------------- */
  void Init(FILE*const fpDevice, const string &strLabel)
  { // Set device, name and write confirmation of opening a device handle
    FStreamSetHandle(fpDevice);
    IdentSet(strLabel);
    WriteString(StrFormat("Logging to standard output '$'.", IdentGet()));
  }
  /* ----------------------------------------------------------------------- */
  bool Init(const string &strFN)
  { // Return failure if already opened else write log file and return success
    if(FStreamOpen(strFN, FM_W_T)) return false;
    WriteString(StrFormat("Log file is '$'.", IdentGet()));
    return true;
  }
  /* -- Constructor -------------------------------------------------------- */
  Log(void) :
    /* -- Initialisers ----------------------------------------------------- */
    llLevels{{                         // Initialise log level strings
      "Critical",                      // Log line is critical
      "Error",                         // Log line is an error
      "Warning",                       // Log line is a warning
      "Info",                          // Log line is information
      "Debug"                          // Log line is for developers
    }},                                // End of log level strings
    strStdOut{ "/dev/stdout" },        // Initialise display label for stdout
    strStdErr{ "/dev/stderr" },        // Initialise display label for stderr
    lhlLevel{ LH_DEBUG },              // Initialise default level
    stMaximum(1000)                    // Initialise maximum output lines
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Destructor --------------------------------------------------------- */
  DTORHELPER(~Log, DeInitSafe())
  /* -- Macros ------------------------------------------------------------- */
  DELETECOPYCTORS(Log)                 // Suppress default functions for safety
  /* -- Conlib callback function for APP_LOG variable ---------------------- */
  CVarReturn LogFileModified(const string &strFN, string &strCV)
  { // Lock mutex
    const LockGuard lgLogSync{ GetMutex() };
    // Close log if opened
    if(FStreamOpened()) DeInit();
    // Check for special character
    switch(strFN.length())
    { // Empty? Ignore
      case 0: return ACCEPT;
      // One character? Compare it...
      case 1: switch(strFN.front())
      { // Check for requested use of stderr or stdout
        case '!': Init(stderr, strStdErr); return ACCEPT;
        case '-': Init(stdout, strStdOut); return ACCEPT;
        // Anything else ignore and open the file normally
        default: break;
      } // Anything else just break;
      default: break;
    } // Create new filename and set filename on success and return success
    if(!Init(StrAppend(strFN, "." LOG_EXTENSION))) return DENY;
    strCV = IdentGet();
    return ACCEPT_HANDLED;
  }
  /* -- Conlib callback function for APP_LOGLINES variable ----------------- */
  CVarReturn LogLinesModified(const size_t stL)
  { // Must have at least one line and lets also set a safe maximum
    if(stL < 1 || stL > 1000000 || stL > max_size()) return DENY;
    // Prevent use of variables off the main thread
    const LockGuard lgLogSync{ GetMutex() };
    // Current size is over the new maximum? Trim the oldest entries out
    if(size() > stMaximum)
      erase(begin(), next(begin(), static_cast<ssize_t>(size() - stMaximum)));
    // Set new maximum and return success
    stMaximum = stL;
    return ACCEPT;
  }
  /* -- End ---------------------------------------------------------------- */
} *cLog = nullptr;                     // Pointer to static class
/* ------------------------------------------------------------------------- */
};                                     // End of public module namespace
/* ------------------------------------------------------------------------- */
};                                     // End of private module namespace
/* == EoF =========================================================== EoF == */
