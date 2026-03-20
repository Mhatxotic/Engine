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
using namespace IClock::P;             using namespace ICommon::P;
using namespace ICVarDef::P;           using namespace IFStream::P;
using namespace IIdent::P;             using namespace IMutex::P;
using namespace IStd::P;               using namespace IString::P;
using namespace ISysUtil::P;           using namespace IToken::P;
/* -- Typedefs ------------------------------------------------------------- */
typedef ClockChrono<CoreClock> CoreClockChrono; // Log timer class
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
class Log;                             // Class prototype
static Log *cLog = nullptr;            // Address of global log class
class Log :                            // The actual class body
  /* -- Base classes ------------------------------------------------------- */
  public LogLines,                     // Holds info about every log line
  public FStream,                      // Output log file if needed
  public CoreClockChrono,              // Holds the current log time
  public MutexLock                     // Because logger needs thread safe
{ /* -- Private typedefs --------------------------------------------------- */
  typedef IdList<LH_MAX> LogLevels;    // Log levels as human readable strings
  typedef atomic<LHLevel> SafeLHLevel; // Async safe version of 'LHLevel'.
  /* -- Private variables -------------------------------------------------- */
  const LogLevels  llLevels;           // Log level strings
  const string     strStdOut,          // Label for 'stdout'
                   strStdErr;          // Label for 'stderr'
  SafeLHLevel      slhlLevel;          // Log helper level for this instance
  size_t           stMaximum;          // Maximum log lines
  FStreamMode      fsmMode;            // Logging mode (append or truncate)
  /* -- Reserve a certain amount of log lines ------------------------------ */
  void LogReserveLines(const size_t stLines)
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
  /* -- Return log level --------------------------------------------------- */
  LHLevel LogGetLevel() const { return slhlLevel; }
  /* -- Write lines to log ------------------------------------------------- */
  void LogWriteLines(const LHLevel lhRequire, const TokenList &tLines)
  { // Ignore if no lines
    if(tLines.empty()) return;
    // Prune log to fit this many lines
    LogReserveLines(tLines.size());
    // For each line, print to log. Note that I tried using string
    // appending for this and it turned out to be almost twice as slow as
    // using formatstring due to the fact that less memory management
    // is required!
    for(const string &strLine : tLines)
      push_back({ CCDeltaToDouble(), lhRequire, StdMove(strLine) });
    // Ignore if file not opened
    if(FStreamClosed()) return;
    // Get start of log
    LogLinesConstIt llciItem{ cbegin() };
    // Repeat...
    do
    { // Get reference to line
      const LogLine &llLine = *llciItem;
      // Build string to write
      const string strLine{ StrFormat("[$$$]<$> $\n", fixed, setprecision(6),
        llLine.dTime, LogLevelToString(llLine.lhlLevel).front(),
        llLine.strLine) };
      // Write stored line and write number of bytes written, and if we did?
      if(const size_t stWritten = FStreamWriteString(strLine)) [[likely]]
      { // Correct number of bytes
        if(stWritten == strLine.length()) [[likely]]
        { // Flush success?
          if(FStreamFlush()) [[likely]]
          { // Remove the items we wrote and write the next line
            erase(cbegin(), llciItem);
            continue;
          } // If no disk space then return to try again later
          if(StdIsNoDiskSpace()) return;
          LogWarningExSafe("Log file flush $ bytes failure: $!",
            strLine.size(), StrFromErrNo());
        } // Incorrect number of bytes? Log the error
        else
        { // If no disk space then return to try again later
          if(StdIsNoDiskSpace()) return;
          LogWarningExSafe("Log file wrote only $ of $ bytes: $!",
            stWritten, strLine.size(), StrFromErrNo());
        } // Proceed to try and reopen the file
      } // Write failure?
      else
      { // If no disk space then return to try again later else reopen
        if(StdIsNoDiskSpace()) return;
        LogWarningExSafe("Log file write $ bytes error: $!",
          strLine.size(), StrFromErrNo());
      } // Reinitialise the log and if failed? Log complete and utter failure
      if(!LogInit(IdentGet())) [[unlikely]] return;
        LogErrorExSafe("Log file cannot be reopened: $!", StrFromErrNo());
      // Return failure and don't remove stored message. Next time another
      // log event occurs, fwrite will fail and retry again. Note that the
      // output buffer will just continue fill up until we can write again and
      // excess output lines will be purged as normal.
      return;
    } // ... until all lines written
    while(++llciItem != cend());
    // We wrote all the lines
    clear();
  }
  /* -- Write string to log. Line feed creates multiple lines -------------- */
  void LogWriteString(const LHLevel lhL, const string &strLine)
    { LogWriteLines(lhL, { strLine, cCommon->CommonLf(), stMaximum }); }
  /* -- Unformatted critical levle logging --------------------------------- */
  void LogWriteString(const string &strLine)
    { LogWriteString(LH_CRITICAL, strLine); }
  /* -- Formatted critical level logging ----------------------------------- */
  template<typename ...VarArgs>
    void LogWriteStringEx(const char*const cpFormat, VarArgs &&...vaArgs)
      { LogWriteString(StrFormat(cpFormat, StdForward<VarArgs>(vaArgs)...)); }
  /* ----------------------------------------------------------------------- */
  void LogDeInit()
  { // Bail if initialised
    if(FStreamClosed()) return;
    // Log file closure
    LogWriteString(StrFormat("Log file closed at $.", cmSys.FormatTime()));
    // Done so close the file
    FStreamClose();
  }
  /* -- Convert log level to a string -------------------------------------- */
  const string_view &LogLevelToString(const LHLevel lhId)
    { return llLevels.Get(lhId); }
  /* --------------------------------------------------------------- */ public:
  size_t Clear()
  { // Get num log lines for returning
    const size_t stSize = size();
    // Clear the log
    clear();
    // Return number of lines cleared
    return stSize;
  }
  /* ----------------------------------------------------------------------- */
  bool LogHasLevel(const LHLevel lhReq) const
    { return lhReq <= LogGetLevel(); }
  /* ----------------------------------------------------------------------- */
  bool LogNotHasLevel(const LHLevel lhReq) const
    { return lhReq > LogGetLevel(); }
  /* -- Safe async call to check if output file is a device ---------------- */
  bool LogIsRedirectedToDevice()
    { return MutexCall([this](){ return FStreamIsHandleStd(); }); }
  /* -- Safe async call to close file -------------------------------------- */
  void LogDeInitSafe() { MutexCall([this](){ LogDeInit(); }); }
  /* -- Safe async call to get log filename from Ident base class ---------- */
  const string LogGetNameSafe()
    { return MutexCall([this](){ return IdentGet(); }); }
  /* -- Unformatted logging without level check (specified level) ---------- */
  void LogNLCSafe(const LHLevel lhL, const string& strLine)
    { MutexCall([this, lhL, &strLine](){ LogWriteString(lhL, strLine); }); }
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
    const char*const cpFormat, VarArgs &&...vaArgs)
      { LogNLCSafe(lhLev,
          StrFormat(cpFormat, StdForward<VarArgs>(vaArgs)...)); }
  /* -- Formatted logging without level check (error level) ---------------- */
  template<typename ...VarArgs>
    void LogNLCErrorExSafe(const char*const cpFormat, VarArgs &&...vaArgs)
      { LogNLCExSafe(LH_ERROR, cpFormat, StdForward<VarArgs>(vaArgs)...); }
  /* -- Formatted logging without level check (warning level) -------------- */
  template<typename ...VarArgs>
    void LogNLCWarningExSafe(const char*const cpFormat, VarArgs &&...vaArgs)
      { LogNLCExSafe(LH_WARNING, cpFormat, StdForward<VarArgs>(vaArgs)...); }
  /* -- Formatted logging without level check (info level) ----------------- */
  template<typename ...VarArgs>
    void LogNLCInfoExSafe(const char*const cpFormat, VarArgs &&...vaArgs)
      { LogNLCExSafe(LH_INFO, cpFormat, StdForward<VarArgs>(vaArgs)...); }
  /* -- Formatted logging without level check (debug level) ---------------- */
  template<typename ...VarArgs>
    void LogNLCDebugExSafe(const char*const cpFormat, VarArgs &&...vaArgs)
      { LogNLCExSafe(LH_DEBUG, cpFormat, StdForward<VarArgs>(vaArgs)...); }
  /* -- Formatted logging with level check (specified level) --------------- */
  template<typename ...VarArgs>void LogExSafe(const LHLevel lhL,
    const char*const cpFormat, VarArgs &&...vaArgs)
  { // Return if we don't have this level
    if(LogNotHasLevel(lhL)) return;
    // Write formatted string
    LogNLCExSafe(lhL, cpFormat, StdForward<VarArgs>(vaArgs)...);
  }
  /* -- Formatted logging with level check (error level) ------------------- */
  template<typename ...VarArgs>
    void LogErrorExSafe(const char*const cpFormat, VarArgs &&...vaArgs)
      { LogExSafe(LH_ERROR, cpFormat, StdForward<VarArgs>(vaArgs)...); }
  /* -- Formatted logging with level check (warning level) ----------------- */
  template<typename ...VarArgs>
    void LogWarningExSafe(const char*const cpFormat, VarArgs &&...vaArgs)
      { LogExSafe(LH_WARNING, cpFormat, StdForward<VarArgs>(vaArgs)...); }
  /* -- Formatted logging with level check (info level) -------------------- */
  template<typename ...VarArgs>
    void LogInfoExSafe(const char*const cpFormat, VarArgs &&...vaArgs)
      { LogExSafe(LH_INFO, cpFormat, StdForward<VarArgs>(vaArgs)...); }
  /* -- Formatted logging with level check --------------------------------- */
  template<typename ...VarArgs>
    void LogDebugExSafe(const char*const cpFormat, VarArgs &&...vaArgs)
      { LogExSafe(LH_DEBUG, cpFormat, StdForward<VarArgs>(vaArgs)...); }
  /* -- Unformatted logging with level check (specified level) ------------- */
  void LogSafe(const LHLevel lhL, const string& strLine)
    { if(LogNotHasLevel(lhL)) return; LogNLCSafe(lhL, strLine); }
  /* -- Unformatted logging with level check (error level) ----------------- */
  void LogErrorSafe(const string& strLine) { LogSafe(LH_ERROR, strLine); }
  /* -- Unformatted logging with level check (warning level) --------------- */
  void LogWarningSafe(const string& strLine) { LogSafe(LH_WARNING, strLine); }
  /* -- Unformatted logging with level check (info level) ------------------ */
  void LogInfoSafe(const string& strLine) { LogSafe(LH_INFO, strLine); }
  /* -- Unformatted logging with level check (debug level) ----------------- */
  void LogDebugSafe(const string& strLine) { LogSafe(LH_DEBUG, strLine); }
  /* -- Return buffer lines for debugger ----------------------------------- */
  void LogGetBufferLines(ostringstream &osS)
  { // Gain exclusive access to log lines
    MutexCall([this, &osS](){
      // For each log entry, write the line to the buffer
      for(const LogLine &llLine : *this) osS
        << '[' << fixed << setprecision(6) << llLine.dTime << "] "
        << llLine.strLine << '\n';
    });
  }
  /* -- Initialise log to built-in standard output ------------------------- */
  void LogInit(FILE*const fpDevice, const string &strLabel)
  { // Set device, name and write confirmation of opening a device handle
    FStreamSetHandle(fpDevice);
    IdentSet(strLabel);
    LogWriteStringEx("Logging to standard output '$'.", IdentGet());
  }
  /* -- Initialise with specified file name -------------------------------- */
  bool LogInit(const string &strFN)
  { // Try to create the file, or open append it and return if failure
    if(FStreamOpen(strFN, fsmMode)) return false;
    LogWriteStringEx("Log file is '$' at $.", IdentGet(), cmSys.FormatTime());
    return true;
  }
  /* -- Constructor --------------------------------------------- */ protected:
  Log() :
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
    slhlLevel{ LH_DEBUG },             // Initialise default level
    stMaximum(1000),                   // Initialise maximum output lines
    fsmMode(FM_MAX)                    // File mode initialised by cvar
    /* -- Set global pointer to static class ------------------------------- */
    { cLog = this; }
  /* -- Destructor --------------------------------------------------------- */
  DTORHELPER(~Log, LogDeInitSafe())
  /* -- Set file mode ---------------------------------------------- */ public:
  CVarReturn LogSetFileAppend(const bool bAppend)
    { return CVarSimpleSetInt(fsmMode, bAppend ? FM_A_P_T : FM_W_T); }
  /* -- Safe access to members --------------------------------------------- */
  CVarReturn LogSetLevel(const LHLevel lhlNewLevel)
  { // Deny if invalid level
    if(lhlNewLevel >= LH_MAX) return DENY;
    // Not same level as current? (when cvar registered/init prevents echo)
    if(lhlNewLevel != LogGetLevel())
    { // Set new logging state and save old one
      const LHLevel lhOldLevel = LogGetLevel();
      slhlLevel = lhlNewLevel;
      // Report state, we could disable logging so we should force report it
      LogNLCDebugExSafe("Log change verbosity from $ ($) to $ ($).",
        LogLevelToString(lhOldLevel), lhOldLevel,
        LogLevelToString(lhlNewLevel), lhlNewLevel);
    } // Success
    return ACCEPT;
  }
  /* -- Conlib callback function for APP_LOG variable ---------------------- */
  CVarReturn LogFileModified(const string &strFN, string &strCV)
  { // Lock mutex
    return MutexCall([this, &strFN, &strCV](){
      // Close log if opened
      if(FStreamOpened()) LogDeInit();
      // Check for special character
      switch(strFN.length())
      { // Empty? Ignore
        case 0: return ACCEPT;
        // If not on Windows?
#if !defined(WINDOWS)
        // One character was specified?
        case 1:
          // Compare the character
          switch(strFN.front())
          { // Check for requested use of stderr or stdout
            case '!': LogInit(stderr, strStdErr); return ACCEPT;
            case '-': LogInit(stdout, strStdOut); return ACCEPT;
            // Anything else ignore and open the file normally
            default: break;
          } // Done
          break;
#endif
        // Anything else just break.
        default: break;
      } // Create new filename and set filename on success and return success
      if(!LogInit(StrAppend(strFN, "." LOG_EXTENSION))) return DENY;
      strCV = IdentGet();
      return ACCEPT_HANDLED;
    });
  }
  /* -- Conlib callback function for APP_LOGLINES variable ----------------- */
  CVarReturn LogLinesModified(const size_t stL)
  { // Must have at least one line and lets also set a safe maximum
    if(stL < 1 || stL > 1000000 || stL > max_size()) return DENY;
    // Prevent use of variables off the main thread
    return MutexCall([this, stL](){
      // Current size is over the new maximum? Trim the oldest entries out
      if(size() > stMaximum)
        erase(begin(),
          next(begin(), static_cast<ssize_t>(size() - stMaximum)));
      // Set new maximum and return success
      stMaximum = stL;
      return ACCEPT;
    });
  }
};
/* ------------------------------------------------------------------------- */
};                                     // End of public module namespace
/* ------------------------------------------------------------------------- */
};                                     // End of private module namespace
/* == EoF =========================================================== EoF == */
