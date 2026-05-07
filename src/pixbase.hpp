/* == PIXBASE.HPP ========================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## This is a POSIX specific module that handles unhandled exceptions   ## **
** ## and writes debugging information to disk to help resolve bugs.      ## **
** ## Since we support MacOS and Linux, we can support both systems very  ## **
** ## simply with POSIX compatible calls.                                 ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace ISysBase {                   // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IClock::P;             using namespace ICmdLine::P;
using namespace ICommon::P;            using namespace IError::P;
using namespace IEvtMain::P;           using namespace IFStream::P;
using namespace ILog::P;               using namespace IMemory::P;
using namespace IStd::P;               using namespace IStdLib::P;
using namespace IStat::P;              using namespace IString::P;
using namespace ISysMod::P;            using namespace ISysRedirect::P;
using namespace ISysUtil::P;           using namespace IThread::P;
using namespace IToken::P;             using namespace Lib::OS;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* ------------------------------------------------------------------------- */
class SysBase :                        // Safe exception handler namespace
  /* -- Base classes ------------------------------------------------------- */
  public SysVersion                    // Version information class
{ /* -- Private classes ---------------------------------------------------- */
#if !defined(BUILD) && !defined(ALPHA) // Not applicable on command-line tool
  SysRedirect srRedirect;              // Output redirector. Note that this
#endif                                 // is final can't be derived!!!
  /* -- Private typedefs --------------------------------------------------- */
  MAPPACK_BUILD(ResourceLimit, const int, struct rlimit)
  ResourceLimitMap rlmLimits;          // Resource limits database
  /* ----------------------------------------------------------------------- */
  enum ExitState { ES_SAFE, ES_UNSAFE, ES_CRITICAL }; // Signal exit types
  /* -- Signals to support ------------------------------------------------- */
#if !defined(ALPHA)
  using SignalPair = StdPair<const int, void(*)(int)>;
  using SignalList = StdArray<SignalPair, 14> ;
  SignalList       slSignals;          // Signal list
#endif
  /* ----------------------------------------------------------------------- */
  void DebugFunction(Statistic &staData, const char*const cpStack,
    const void*const vpStack) const
  { // Information about the object
    Dl_info diData;
    // Tokenise the stack after removing duplicate whitespaces. Note that objc
    // calls will have spaces in them.
    const TokenStr tsData{ StrCompact(cpStack), cCommon->CommonSpaceV() };
    // Need some extra work on Apple
#if defined(MACOS)
    // The last two tokens should always be a + and a number which we will
    // grab.
    switch(tsData.size())
    { // Not enough data?
      case  0: staData.Data(cCommon->CommonUnspec())
                      .Data(cCommon->CommonUnspec());
               break;
      case  1: staData.Data(tsData.front()).Data(cCommon->CommonUnspec());
               break;
      case  2: staData.Data(tsData[1]).Data(cCommon->CommonUnspec());
               break;
      case  3: staData.Data(tsData[1]).Data(tsData[2]);
               break;
      case  4: staData.Data(tsData[1]).Data(tsData[3]);
               break;
      case  5: staData.Data(tsData[1]).DataA(tsData[3], ' ', tsData[4]);
               break;
      // Expected amount for a C/C++ call. Just show the + amount
      default: staData.Data(tsData[1]).DataA(tsData[2], '+', tsData.back());
               break;
    } // Get information about the item and if failed?
    if(!dladdr(vpStack, &diData))
    { // Just add unknown and try the next function level
      staData.Data(cCommon->CommonUnspec());
      return;
    } // Running on Linux?
#else
    // Get information about the item and if failed?
    if(!dladdr(vpStack, &diData))
    { // Just add what the second value was and return
      staData.Data(tsData[1]);
      return;
    }
#endif
    // Buffer for allcated mrmory
    int iStatus = 0;
    // Demangle the name and check result and compare status. We need to
    // free the string after so we'll let unique_ptr do it for us. For some
    // reason, GCC on Linux doesn't like decltype(free) but void(void*) works.
    if(StdUniquePtr<char, function<void(void*)>>
      uPtr{ abi::__cxa_demangle(diData.dli_sname,
        nullptr, nullptr, &iStatus), free })
          staData.Data(uPtr.get());
    // What is the return code for this call?
    else switch(iStatus)
    { // Memory error?
      case -1: staData.DataF("<MAE:$>", tsData[1]); break;
      // Not a valid name?
      case -2: staData.Data(diData.dli_sname); break;
      // Invalid argument?
      case -3: staData.DataF("<IA:$>", tsData[1]); break;
      // Success (impossible) or unknown?
      default: staData.DataF("<$:$>", iStatus, tsData[1]); break;
    }
  }
  /* ----------------------------------------------------------------------- */
  void DumpStack(StdOStringStream &osS) const
  { // Create array to hold stack pointers
    using StackArray = StdArray<void*, 256> ;
    StackArray saArray;
    // Get the number of stack frames that can fit in the array and if can?
    void**const vplArPtr = saArray.data();
    constexpr size_t stArLen = saArray.size() / sizeof(StackArray::value_type);
    if(const int iSize = backtrace(vplArPtr, stArLen))
    { // Get stack trace. For some reason, GCC on Linux doesn't like
      // decltype(free) but void(void*) works.
      using StrStack = StdUniquePtr<char*, function<void(void*)>>;
      if(const StrStack ssStack{ backtrace_symbols(vplArPtr, iSize), free })
      { // Convert entries to size_t
        const size_t stSize = static_cast<size_t>(iSize);
        // Setup table formatter
        Statistic staData;
        staData.Header("#")
        // MacOS shows extra information with 'backtrace_symbols()'
        // 'STACKID MODULE ADDRESS FUNCTION' as opposite to 'STACKID FUNCTION'
        // on Linux systems.
#if defined(MACOS)
               .Header("Module").Header("Address")
#endif
               .Header("Function", false)
        // Reserve specified number of rows in output table
               .Reserve(stSize);
        // Write pointer address and name
        for(size_t stI = 0; stI < stSize; ++stI)
        { // Add ID
          staData.DataN(stI);
          // Add others
          DebugFunction(staData, ssStack.get()[stI], saArray[stI]);
        } // We got a stack trace
        osS << ", stack trace:-\n";
        // Build output into string stream
        staData.Finish(osS);
        // Footer
        osS << stSize << " calls.\n";
        // Done
        return;
      }
    } // Problem generating backtrace.
    osS << '.';
  }
  /* ----------------------------------------------------------------------- */
  void DumpMods(StdOStringStream &osS)
  { // Ignore if no mods
    if(empty()) return;
    // Add mods header
    osS << "\nShared objects:-\n"
           "================\n";
    // Prepare headers
    Statistic staData;
    staData.Header("DESCRIPTION").Header("VERSION", false)
           .Header("VENDOR", false).Header("MODULE", false)
           .Reserve(size());
    // Enumerate each detected module
    for(const SysModMapPair &smmpPair : *this)
    { // Get reference to mod data and add to table
      const SysModuleData &smdData = smmpPair.second;
      staData.Data(smdData.GetDesc()).Data(smdData.GetVersion())
             .Data(smdData.GetVendor()).Data(smdData.GetFull());
    } // Finished enumeration of modules
    osS << staData.Finish();
  }
  /* ----------------------------------------------------------------------- */
  ExitState DebugMessage(const char*const cpSignal, const char*const cpExtra)
  { // Build filename
    const StdString strFileName{ cCmdLine ?
      StrAppend(cCmdLine->CmdLineGetCArgs()[0], "." CRASH_EXTENSION) :
      "/tmp/engine-crash.txt" };
    // Begin message
    StdOStringStream osS;
    osS << "Received signal 'SIG" << cpSignal << "' at "
        << cmSys.FormatTime();
    // Dump the stack
    DumpStack(osS);
    // Add extra information if set
    if(*cpExtra) osS << cCommon->CommonLf() << cpExtra << cCommon->CommonLf();
 // Not building the command line too?
#if !defined(BUILD) && !defined(ALPHA)
    // Shut down the stderr monitoring thread
    srRedirect.ResetSafe();
#endif
    // Print it to stderr
    fputs(osS.str().data(), stderr);
    // Dump mods to log
    DumpMods(osS);
    // Add trace header
    osS << "\nLog trace:-\n"
           "===========\n";
    // Now add the buffer lines
    cLog->LogGetBufferLines(osS);
    // Write the output and close the log
    const StdString strMsg{ StrAppend(osS.str(), '\n') };
    // Message box string
    StdOStringStream osTS;
    osTS << "Received signal SIG" << cpSignal << " at " << cmSys.FormatTime()
         << ". This means that the engine must now terminate and we apologise "
            "for the inconvenience with the loss of any unsaved progress. ";
    // Create the debug log and exit if failed
    if(FStream fOut{ StdMove(strFileName), FM_W_T })
    { // Write to crash output file
      fOut.FStreamWriteString(strMsg);
      // We wrote the crash log
      osTS << "Please submit the crash dump file at '" << fOut.NameGet()
           << "' to us so that we may investigate further and resolve the "
              "problem so it does not reoccur again.";
    } // Exit write log failure? We wrote the crash log
    else osTS << "We unfortunately could not write an error log to '"
              << fOut.NameGet() << "' because " << fOut.FStreamGetErrStr()
              << '!';
    // Finish string
    osTS << " Please press the button to terminate.";
    // Show message box
    SysMessage("Critical error!", osTS.str(), MB_ICONSTOP);
    // Send requested exit code
    return ES_CRITICAL;
  }
  /* ----------------------------------------------------------------------- */
  ExitState DebugMessage(const char*const cpSignal)
    { return DebugMessage(cpSignal, cCommon->CommonCBlank()); }
  /* ----------------------------------------------------------------------- */
  ExitState ConditionalExit(const StdString &strName, unsigned &uAttempts)
  { // If events system is available?
    if(cEvtMain)
    { // Maximum number of attempts so repeated attempts eventually just
      // make the engine quit immediately.
      static const unsigned uMaxAttempts = 5;
      // Incrememnt attempts and if there are not too many attempts?
      if(++uAttempts < uMaxAttempts)
      { // Log that we're trying to shut down
        cLog->LogNLCErrorExSafe("System got $ signal $ of $, shutting down...",
          strName, uAttempts, uMaxAttempts);
        // Send clean shutdown event to engine
        return ES_SAFE;
      } // Log immediate shutdown because too many attempts
      cLog->LogNLCErrorExSafe("System got $ $ signals so terminating now...",
        uMaxAttempts, strName);
    } // No attempts required so log generic exit message
    else cLog->LogNLCErrorExSafe("System got $ signal so terminating now...",
      strName);
    // Unsafe shutdown
    return ES_UNSAFE;
  }
  /* ----------------------------------------------------------------------- */
  ExitState HandleSignalSafe(const int iSignal)
  { // Which signal
    switch(iSignal)
    { // The signal is usually initiated by the process itself when it calls
      // abort function of the C Standard Library, but it can be sent to the
      // process from outside like any other signal.
      case SIGABRT: return DebugMessage("ABRT (Aborted)");
      // Sent when we cause a bus error. The conditions that lead to the signal
      // being sent are, for example, incorrect memory access alignment or
      // non-existent physical address.
      case SIGBUS: return DebugMessage("BUS (Bus error)");
      // Sent when we execute an erroneous arithmetic operation, such as
      // division by zero (the name "FPE", standing for floating-point
      // exception, is a misnomer as the signal covers integer-arithmetic
      // errors as well).
      case SIGFPE: return DebugMessage("FPE (Floating-point exception)");
      // Sent by our controlling terminal when a user wishes to interrupt the
      // process. This is typically initiated by pressing Ctrl-C, but on some
      // systems, the "delete" character or "break" key can be used.
      case SIGINT:
      { // Number of sigint's processed
        static unsigned uSigIntCount = 0;
        // Perform conditional exit
        return ConditionalExit("interrupt", uSigIntCount);
      } // Incorrect machine code. Apparently, this instruction can't be
      // caught but we'll include it just incase some systems can.
      case SIGILL: return DebugMessage("ILL (Illegal instruction)",
        // Add extra information
        "This illegal instruction exception usually indicates that this "
        BUILD_TARGET " compiled binary is NOT compatible with your "
        "operating system and/or central processing unit. "
#if defined(X64)                       // Special message for 64-bit system?
# if defined(MACOS)                    // Apple target?
#  if defined(RISC)                    // Arm64 target?
        "Since you are running the 64-bit ARM version of this binary, "
        "please consider trying the 64-bit Intel version instead."
#  elif defined(CISC)                  // x86-64 target?
        "Since you are running the 64-bit Intel version of this binary, "
        "please consider trying the 64-bit ARM version instead."
#  endif                               // Arch target check
# elif defined(WINDOWS)                // Windows target?
        "Since you are running the 64-bit version of this binary, please "
        "consider trying the 32-bit version instead."
# endif                                // OS target check
#elif defined(X86)                     // X86 target?
        "Since you are running the 32-bit version of this binary, you may "
        "need to upgrade your operating system and/or central processing "
        "unit. If you originally tried the 64-bit version and you still got "
        "the the same error then you may need to report this issue as a bug."
#endif                                 // Bit target check
        );
      // Sent when we attempt to write to a pipe without a process connected
      // to the other end.
      case SIGPIPE: return DebugMessage("PIPE (Broken pipe)");
      // Sent by our controlling terminal when the user requests that the
      // process quit and perform a core dump.
      case SIGQUIT: return DebugMessage("QUIT (Quit from keyboard)");
      // Sent when we make an invalid virtual memory reference, or
      // segmentation fault, i.e. when it performs a segmentation violation.
      case SIGSEGV: return DebugMessage("SEGV (Segmentation fault)");
      // Sent when we pass a bad argument to a system call. In practice, this
      // kind of signal is rarely encountered since applications rely on
      // libraries (e.g. libc) to make the call for them.
      case SIGSYS: return DebugMessage("SYS (Illegal system call)");
      // Sent when our controlling terminal is closed. It was originally
      // designed to notify the process of a serial line drop (a hangup). In
      // modern systems, this signal usually means that the controlling
      // pseudo or virtual terminal has been closed. Many daemons will reload
      // their configuration files and reopen their logfiles instead of
      // exiting when receiving this signal. nohup is a command to make a
      // command ignore the signal.
      case SIGHUP:
      { // Number of sighup's processed
        static unsigned uSigHangUp = 0;
        // Perform conditional exit
        return ConditionalExit("hang up", uSigHangUp);
      } // Sent to request our termination. Unlike the SIGKILL signal, it can
      // be caught and interpreted or ignored by the process. This allows the
      // process to perform nice termination releasing resources and saving
      // state if appropriate. SIGINT is nearly identical to SIGTERM.
      case SIGTERM:
      { // Number of sighup's processed
        static unsigned uSigTerm = 0;
        // Perform conditional exit
        return ConditionalExit("termination", uSigTerm);
      } // Sent when an exception (or trap) occurs: a condition that a debugger
      // has requested to be informed of — for example, when a particular
      // function is executed, or when a particular variable changes value.
      case SIGTRAP: return DebugMessage("TRAP (Breakpoint)");
      // Sent when we have used up the CPU for a duration that exceeds a
      // certain predetermined user-settable value. The arrival of a SIGXCPU
      // signal provides the receiving process a chance to quickly save any
      // intermediate results and to exit gracefully, before it is terminated
      // by the operating system using the SIGKILL signal.
      case SIGXCPU: return DebugMessage("XCPU (Processor time threshold)");
      // Sent when we grow a file larger than the maximum allowed size.
      case SIGXFSZ: return DebugMessage("XFSZ (File size threshold)");
      // Unrecognised signal?
      default: return DebugMessage(StrFormat("UNKNOWN<$>", iSignal).data());
    }
  }
  /* ----------------------------------------------------------------------- */
  static void HandleSignalStatic(int);
  void HandleSignal(const int iSignal) try
  { // Handle signal and get result
    switch(HandleSignalSafe(iSignal))
    { // Exit safely? Send shutdown to engine and do nothing else
      case ES_SAFE: cEvtMain->RequestQuit(); return;
      // Exit unsafely? Send safe shutdown
      case ES_UNSAFE: exit(-1);
      // Exit unsafely and immediately? Shutdown now!
      case ES_CRITICAL: _exit(-2);
      // Invalid command? (To prevent compiler warning)
      default: _exit(-3);
    }
  } // Exception occured so just exit now
  catch(const StdException &) { _exit(-3); }
  /* -- Set socket timeout ----------------------------------------- */ public:
  int SetSocketTimeout(const int iFd, const double dRTime,
    const double dWTime)
  { // Calculate timeout in milliseconds
    struct timeval                     // Sec, USec
      tRT{ static_cast<int>(dRTime), static_cast<int>(dRTime*1000)%1000 },
      tWT{ static_cast<int>(dWTime), static_cast<int>(dWTime*1000)%1000 };
    // Set socket options and get result
    return (setsockopt(iFd, SOL_SOCKET, SO_RCVTIMEO,
              reinterpret_cast<void*>(&tRT), sizeof(tRT)) < 0 ? 1 : 0) |
           (setsockopt(iFd, SOL_SOCKET, SO_SNDTIMEO,
              reinterpret_cast<void*>(&tWT), sizeof(tWT)) < 0 ? 2 : 0);
  }
  /* -- Check if process is in background ---------------------------------- */
  static bool IsInBackground()
  { // Get terminal foreground process and return if in backround
    const pid_t pTerminal = tcgetpgrp(STDOUT_FILENO), pParent = getpgrp();
    return pTerminal != pParent;
  }
  /* ------------------------------------------------------------ */ protected:
  static void ProcessAndActivateLocale(StdString &strCode)
  { // Return generic id if empty
    if(strCode.empty()) { Default: strCode = "en-GB"; return; }
    // Show a warning if length doesn't make sense
    if(strCode.size() < 5 || strCode.size() > 32)
    { // Log a warning
      cLog->LogWarningExSafe(
        "System not processing malformed locale '$'!", strCode);
      // Set a default locale and return
      goto Default;
    } // Find a period (e.g. "en_GB.UTF-8") and remove suffix it if found
    const size_t stPeriod = strCode.find('.');
    if(stPeriod != StdNPos) strCode = strCode.substr(0, stPeriod);
    // Is there a hyphen?
    const size_t stHyphen = strCode.find('-');
    if(stHyphen != StdNPos)
    { // Is there no underscore?
      const size_t stUnderscore = strCode.find('_', stHyphen);
      if(stUnderscore == StdNPos)
      { // Log a warning
        cLog->LogWarningExSafe(
          "System could not decipher locale with hyphen '$'!", strCode);
        // Set a default locale and return
        goto Default;
      } // Rewrite the code without the script part
      strCode = StrAppend(strCode.substr(0, stHyphen),
                          strCode.substr(stUnderscore));
    } // If there is an at sign (variant) then remove it?
    const size_t stAt = strCode.find('@');
    if(stAt != StdNPos) strCode = strCode.substr(0, stAt);
    // Make a string with .UTF8 suffix
    const StdString strCodeUTF8{ strCode + ".UTF-8" };
    // Try to set C locale and warn on fail
    if(!setlocale(LC_ALL, strCodeUTF8.data()))
    { // Show warning to say the function call failed
      cLog->LogWarningExSafe("System could not set locale '$'! $",
        strCodeUTF8, SysError());
      // Set a default locale and return
      goto Default;
    } // Try to set the locale for the C++ object
    try { cCommon->CommonSetLocale(strCodeUTF8.data()); }
    // This very likely to fail on systems with odd locales
    catch(const StdException &eReason)
    { // Show warning to say there was an excaption
      cLog->LogWarningExSafe(
        "System could not build locale object '$'! $", strCodeUTF8, eReason);
      // Set a default locale and return
      goto Default;
    } // Replace underscore with dash to match Windows locale syntax (xx-XX).
    StrReplaceCharRef(strCode, '_', '-');
  }
  /* ----------------------------------------------------------------------- */
  SysBase(SysModMap &&smmMap, const size_t stI) :
    /* -- Initialisers ----------------------------------------------------- */
    SysVersion{                        // Initialise version info class
      StdMove(smmMap), stI },          // Move sent mod list into ours
#if !defined(BUILD) && !defined(ALPHA) // Not applicable on command-line tool
    srRedirect{ STDERR_FILENO, "stderr" }, // Initialise stderr redirect
#endif                                 // BUILD check complete
    /* --------------------------------------------------------------------- */
    rlmLimits{{                        // Limits data
#if !defined(MACOS)                    // Not all resources supported
      { RLIMIT_LOCKS,  { 0, 0 } },     { RLIMIT_MSGQUEUE,   { 0, 0 } },
      { RLIMIT_NICE,   { 0, 0 } },     { RLIMIT_RTPRIO,     { 0, 0 } },
      { RLIMIT_RTTIME, { 0, 0 } },     { RLIMIT_SIGPENDING, { 0, 0 } },
#endif                                 // Mac check
      { RLIMIT_AS,     { 0, 0 } },     { RLIMIT_CORE,       { 0, 0 } },
      { RLIMIT_CPU,    { 0, 0 } },     { RLIMIT_DATA,       { 0, 0 } },
      { RLIMIT_FSIZE,  { 0, 0 } },     { RLIMIT_MEMLOCK,    { 0, 0 } },
      { RLIMIT_NOFILE, { 0, 0 } },     { RLIMIT_NPROC,      { 0, 0 } },
      { RLIMIT_RSS,    { 0, 0 } },     { RLIMIT_STACK,      { 0, 0 } }
    }}
    /* --------------------------------------------------------------------- */
#if !defined(ALPHA)
    , slSignals{{                      // Init signals list
      { SIGABRT, HandleSignalStatic }, { SIGBUS,  HandleSignalStatic },
      { SIGFPE,  HandleSignalStatic }, { SIGHUP,  HandleSignalStatic },
      { SIGILL,  HandleSignalStatic }, { SIGINT,  HandleSignalStatic },
      { SIGPIPE, SIG_IGN            }, { SIGQUIT, HandleSignalStatic },
      { SIGSEGV, HandleSignalStatic }, { SIGSYS,  HandleSignalStatic },
      { SIGTERM, HandleSignalStatic }, { SIGTRAP, HandleSignalStatic },
      { SIGXCPU, HandleSignalStatic }, { SIGXFSZ, HandleSignalStatic },
    }}
#endif
    /* --------------------------------------------------------------------- */
  { // Install all those signal handlers (signal() is not thread safe.)
#if !defined(ALPHA)
    for(SignalPair &spPair : slSignals)
    { // Set the signal and check for error
      spPair.second = signal(spPair.first, spPair.second);
      if(spPair.second == SIG_ERR)
        XCL("Failed to install signal handler!", "Id", spPair.first);
    } // Increase resource limits we can change so the engine can do more
#endif
    StdForEach(par_unseq, rlmLimits.begin(), rlmLimits.end(),
      [](ResourceLimitMapPair &rlmpPair)
    { // Get the limit for this resource
      if(!getrlimit(rlmpPair.first, &rlmpPair.second))
      { // Ignore if value doesn't need to change
        if(rlmpPair.second.rlim_cur >= rlmpPair.second.rlim_max) return;
        // Set maximum allowed and if failed?
        const rlim_t rtOld = rlmpPair.second.rlim_cur;
        rlmpPair.second.rlim_cur = rlmpPair.second.rlim_max;
        if(setrlimit(rlmpPair.first, &rlmpPair.second))
        { // Restore original value
          rlmpPair.second.rlim_cur = rtOld;
          // Log a message
          cLog->LogWarningExSafe(
            "System failed to set resource limit $<0x$$$> from $<0x$$$> to "
            "$<0x$$$>: $!",
            rlmpPair.first, StdIOSHex, rlmpPair.first, StdIOSDec, rtOld,
            StdIOSHex, rtOld, StdIOSDec, rlmpPair.second.rlim_max, StdIOSHex,
            rlmpPair.second.rlim_max, StdIOSDec, SysError());
        }
      } // Failed to get limit so log the error and why
      else cLog->LogWarningExSafe(
        "System failed to get limit for resource $<0x$$$>: $!",
        rlmpPair.first, StdIOSHex, rlmpPair.first, StdIOSDec, SysError());
    });
  }
  /* -- Destructor to uninstall safe signals (signal() not thread safe) ---- */
#if !defined(ALPHA)
  DTORHELPER(~SysBase,
    for(SignalPair &spPair : slSignals)
      if(signal(spPair.first, spPair.second) == SIG_ERR && cLog)
        cLog->LogWarningExSafe("Failed to restore signal $ handler! $.",
          spPair.first, StrFromErrNo()))
#endif
};/* ----------------------------------------------------------------------- */
#define ENGINE_SYSBASE_CALLBACKS() \
  void ISysBase::P::SysBase::HandleSignalStatic(int iSignal) \
    { ISystem::P::cSystem->HandleSignal(iSignal); }
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
