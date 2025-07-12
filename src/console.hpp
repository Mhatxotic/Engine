/* == CONSOLE.HPP ========================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## This module handles a console for the engine so information about   ## **
** ## the engine can be seen and manipulated by the developer or user.    ## **
** ## Exclusively for use in the engine's main thread only.               ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IConsole {                   // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IArgs::P;              using namespace IClock::P;
using namespace ICommon::P;            using namespace IConDef::P;
using namespace IConLib::P;            using namespace ICVar::P;
using namespace ICVarDef::P;           using namespace ICVarLib::P;
using namespace IError::P;             using namespace IEvtMain::P;
using namespace IFlags;                using namespace IGlFW::P;
using namespace IHelper::P;            using namespace ILog::P;
using namespace IStd::P;               using namespace IString::P;
using namespace ISocket::P;            using namespace ISystem::P;
using namespace ISysUtil::P;           using namespace ITimer::P;
using namespace IToken::P;             using namespace IUtf::P;
using namespace IUtil::P;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public namespace
/* == Typedefs ============================================================= */
BUILD_FLAGS(Console,                   // Console flags classes
  /* --------------------------------------------------------------------- */
  // No settings?                      Can't disable console? (temporary)
  CF_NONE                   {Flag(0)}, CF_CANTDISABLE            {Flag(1)},
  // Ignore first key on show console? Autoscroll on message?
  CF_IGNOREKEY              {Flag(2)}, CF_AUTOSCROLL             {Flag(3)},
  // Automatically copy cvar on check? Character insert mode?
  CF_AUTOCOPYCVAR           {Flag(4)}, CF_INSERT                 {Flag(5)},
  // Console displayed?                Ignore escape key?
  CF_ENABLED                {Flag(6)}, CF_IGNOREESC              {Flag(7)},
  // Can't disable console?            Block output position update?
  CF_CANTDISABLEGLOBAL      {Flag(8)}, CF_BLOCKOUTPUTUPDATE      {Flag(9)}
);/* ======================================================================= */
BUILD_FLAGS(AutoComplete,              // Autocomplete flags classes
  /* ----------------------------------------------------------------------- */
  // No autocompletion?                Autocomplete command names?
  AC_NONE                   {Flag(0)}, AC_COMMANDS               {Flag(1)},
  // Autocomplete cvar names?
  AC_CVARS                  {Flag(2)},
  /* ----------------------------------------------------------------------- */
  AC_MASK{ AC_COMMANDS|AC_CVARS }      // All flags
);/* ======================================================================= */
BUILD_FLAGS(Redraw,                    // Redraw terminal or graphical console
  /* ----------------------------------------------------------------------- */
  // Redraw nothing                    Redraw text console
  RD_NONE                   {Flag(0)}, RD_TEXT                   {Flag(1)},
  // Redraw graphical console
  RD_GRAPHICS               {Flag(2)},
  /* ----------------------------------------------------------------------- */
  RD_BOTH{ RD_TEXT|RD_GRAPHICS }       // All flags
);/* ----------------------------------------------------------------------- */
MAPPACK_BUILD(Cmd, const string, const ConLib) // Map of commands type
/* ========================================================================= */
class Console;                         // Class prototype
static Console *cConsole = nullptr;    // Pointer to global class
class Console :                        // Members initially private
  /* -- Base classes ------------------------------------------------------- */
  public ConLines,                     // Console text lines list
  private ConLinesConstIt,             // Text lines forward iterator
  private ConLinesConstRevIt,          // Text lines reverse iterator
  private InitHelper,                  // Initialisation helper
  public ConsoleFlags                  // Console flags
{ /* -- Private typedefs --------------------------------------------------- */
  typedef queue<ConLine> ConLineQueue; // Pending console lines
  /* -- Input -------------------------------------------------------------- */
  ConLineQueue     clqOutput;          // Console lines pending
  ConLinesConstRevIt clriPosition;     // Console output position
  StrList          slHistory;          // Console history
  StrListConstRevIt slriInputPosition; // History position
  size_t           stInputMaximum,     // Maximum no. of input lines
                   stOutputMaximum,    // Maximum no. of output lines
                   stMaxInputLine,     // Maximum no. of chars in input
                   stMaxOutputLine,    // Maximum no. of chars per output line
                   stMaxOutputLineE;   // Maximum no. of chars with ellipsis
  string           strConsoleBegin,    // Console input line (before cursor)
                   strConsoleEnd;      // Console input line (after cursor)
  ssize_t          sstPageLines,       // Lines to move when paging up or down
                   sstPageLinesNeg;    // As above but negative version
  RedrawFlags      rfDefault,          // Default redraw type
                   rfFlags;            // Redraw flags
  Colour           cTextColour;        // Console text colour
  /* -- Text mode ---------------------------------------------------------- */
  ClockInterval<>  ciOutputRefresh,    // Next screen buffer update time
                   ciInputRefresh;     // Time to wait before next peek
  string           strStatusLeft,      // Text-mode console left status text
                   strStatusRight;     // Text-mode console right status text
  string_view      strvTimeFormat;     // Default time format
  /* -- Other -------------------------------------------------------------- */
  AutoCompleteFlags acFlags;           // Flags for autocomplete
  const ConCmdStaticList &ccslInt;     // Default console cmds list
  CmdMap           cmMap;              // Console commands list
  const EvtMainRegVec emrvEvents;      // Events list to register
  /* -- Do clear console, clear history and reset position ----------------- */
  void DoFlush(void)
  { // Do clear the console output lines
    clear();
    // Reset position
    clriPosition = rbegin();
    // Clear queued messages
    ConLineQueue clqOutputNew;
    clqOutput.swap(clqOutputNew);
  }
  /* -- Reserve history items ---------------------------------------------- */
  void ReserveHistoryLines(const size_t stLines)
  { // Calculate total lines when added
    const size_t stTotal = slHistory.size() + stLines;
    // Return if we don't need to remove lines
    if(stTotal <= GetInputMaximum()) return;
    // If too many lines would be written?
    if(stLines >= GetInputMaximum()) return ClearHistory();
    // Iterator to find
    const StrListConstIt slciIt{ slriInputPosition.base() };
    // Lines to prune to
    const size_t stRemove = slHistory.size() - stLines;
    // Repeat...
    do
    { // If this item is not selected in the history? Erase and next
      if(slHistory.cbegin() != slciIt) { slHistory.pop_front(); continue; }
      // Now we know that this iterator is selected we can erase the rest
      slriInputPosition = StrListConstRevIt{
        slHistory.erase(slHistory.cbegin(), next(slHistory.cbegin(),
          static_cast<ssize_t>((slHistory.size() - 1) - GetInputMaximum()))) };
      // Done
      break;
    } // ...Until we have cleared enough lines
    while(size() > stRemove);
  }
  /* -- Clear console line ------------------------------------------------- */
  void DoClearInput(void) { strConsoleBegin.clear(); strConsoleEnd.clear(); }
  /* -- Return commands list --------------------------------------- */ public:
  const CmdMap &GetCmdsList(void) const { return cmMap; }
  CmdMapIt GetCmdsListEnd(void) { return cmMap.end(); }
  /* -- Get redraw flags --------------------------------------------------- */
  RedrawFlags &GetRedrawFlags(void) { return rfFlags; }
  RedrawFlags &GetDefaultRedrawFlags(void) { return rfDefault; }
  /* -- Get input strings -------------------------------------------------- */
  string &GetConsoleBegin(void) { return strConsoleBegin; }
  string &GetConsoleEnd(void) { return strConsoleEnd; }
  /* -- Return maximum number of output history lines ---------------------- */
  size_t GetOutputMaximum(void) const { return stOutputMaximum; }
  /* -- Return maximum number of input history lines ----------------------- */
  size_t GetInputMaximum(void) const { return stInputMaximum; }
  /* -- Return information about a console command ------------------------- */
  const ConLibStatic &GetCommand(const ConCmdEnums cceId) const
    { return ccslInt[cceId]; }
  /* -- Clear console and redraw ------------------------------------------- */
  void Flush(void) { DoFlush(); SetRedraw(); }
  /* -- Add specified command line to history ------------------------------ */
  void AddHistory(const string &strCmdLine)
  { // Service room for one more input line
    ReserveHistoryLines(1);
    // Add input line to history
    slHistory.emplace_back(strCmdLine);
    // Update history to first item
    slriInputPosition = slHistory.crend();
  }
  /* -- TestAutoComplete ------------------- Processes a successful match -- */
  template<class MapType>
    bool TestAutoComplete(const MapType &mtList, const size_t stBPos,
      const size_t stEPos, const string &strWhat)
  { // Setup iterator to find item and return if empty
    typedef typename MapType::const_iterator MapIterator;
    const MapIterator ciItem{ mtList.lower_bound(strWhat) };
    if (ciItem == mtList.cend()) return false;
    // Get reference name of item and return failed if comparison failed
    const string &strKey = ciItem->first;
    if(strKey.compare(0, strWhat.size(), strWhat)) return false;
    // We found the word so now we need to replace it with the actual command.
    if(stBPos == StdNPos) strConsoleBegin = strKey;
    else strConsoleBegin =
      StrAppend(strConsoleBegin.substr(0, stBPos+1), strKey);
    if(stEPos == StdNPos) strConsoleEnd.clear();
    else strConsoleEnd = strConsoleEnd.substr(stEPos);
    // Redraw the console because we changed the input field
    SetRedraw();
    // Succeeded
    return true;
  }
  /* -- AutoComplete ------------ Auto complete the word under the cursor -- */
  bool AutoComplete(void)
  { // Do not attempt to autocomplete if disabled
    if(acFlags == AC_NONE) return false;
    // Find last whitespace or use the whole text on the beginning text
    // and also find first whitespace or use the whole text on the end text
    const size_t stBPos = strConsoleBegin.find_last_of(' '),
                 stEPos = strConsoleEnd.find(' ');
    // Grab word to autocomplete and return if it is empty
    const string strWhat{ (stBPos == StdNPos
      ? strConsoleBegin : strConsoleBegin.substr(stBPos + 1)) +
                          (stEPos == StdNPos
      ? strConsoleEnd : strConsoleEnd.substr(0, stEPos)) };
    // Return failure if word is empty... or
    if(strWhat.empty()) return false;
    // Walk through all the commands and check if the partial command matches
    // and then try the cvars if failed.
    return (acFlags.FlagIsSet(AC_COMMANDS) &&
            TestAutoComplete(cmMap, stBPos, stEPos, strWhat)) ||
           (acFlags.FlagIsSet(AC_CVARS) &&
            TestAutoComplete(cCVars->GetVarList(), stBPos, stEPos, strWhat));
  }
  /* -- Return text input -------------------------------------------------- */
  bool InputEmpty(void)
    { return strConsoleBegin.empty() && strConsoleEnd.empty(); }
  /* -- Return text input -------------------------------------------------- */
  const string InputText(void)
    { return StrAppend(strConsoleBegin, strConsoleEnd); }
  /* -- OnExecute event ------------------------ Execute inputted command -- */
  void Execute(void)
  { // Do not proceed further if no text is inputted
    if(InputEmpty()) return;
    // Build command by appending text before and after the cursor.
    const string strCmd{ InputText() };
    // Clear current text input now we have it here
    DoClearInput();
    // Split command-line into arguments and process them if not empty
    if(const Args aList{ strCmd }) ExecuteArguments(strCmd, aList);
  }
  /* -- OnCharPress --------------------------- Console character pressed -- */
  void OnCharPress(const unsigned int uiKey)
  { // Ignore first key? Ignore it
    if(FlagIsSet(CF_IGNOREKEY)) FlagClear(CF_IGNOREKEY);
    // Insert character if insert mode is on
    else if(FlagIsSet(CF_INSERT)) AddInputChar(uiKey);
    // Repalce character in input buffer
    else ReplaceInputChar(uiKey);
  }
  /* -- OnForceRedraw ------------------- Force a terminal display update -- */
  void OnForceRedraw(const EvtMainEvent&)
    { SetRedraw(); ciOutputRefresh.CISync(); }
  /* -- OnKeyPress ---------------------------------- Console key pressed -- */
  void OnKeyPress(const int iKey, const int iAction, const int iMods)
  { // Key released? Ignore
    if(iAction == GLFW_RELEASE) return;
    // Control key, which key?
    if(iMods & GLFW_MOD_CONTROL) switch(iKey)
    { // Test keys with control held
      case GLFW_KEY_PAGE_UP   : MoveLogPageUp(); break;
      case GLFW_KEY_PAGE_DOWN : MoveLogPageDown(); break;
      case GLFW_KEY_HOME      : MoveLogHome(); break;
      case GLFW_KEY_END       : MoveLogEnd(); break;
#if defined(MACOS) // Because MacOS keyboards don't have an 'insert' key.
      case GLFW_KEY_DELETE    : ToggleCursorMode(); break;
#endif
      // Unknown key (ignore)
      default: break;
    } // Normal key, which key?
    else switch(iKey)
    { // Test keys with no modifiers held
      case GLFW_KEY_PAGE_UP   : MoveLogUp(); break;
      case GLFW_KEY_PAGE_DOWN : MoveLogDown(); break;
      case GLFW_KEY_HOME      : CursorHome(); break;
      case GLFW_KEY_END       : CursorEnd(); break;
      case GLFW_KEY_BACKSPACE : PopInputBeforeCursor(); break;
      case GLFW_KEY_DELETE    : PopInputAfterCursor(); break;
      case GLFW_KEY_UP        : HistoryMoveBack(); break;
      case GLFW_KEY_DOWN      : HistoryMoveForward(); break;
      case GLFW_KEY_ESCAPE    : ClearInput(); break;
      case GLFW_KEY_LEFT      : CursorLeft(); break;
      case GLFW_KEY_RIGHT     : CursorRight(); break;
      case GLFW_KEY_ENTER     : Execute(); break;
      case GLFW_KEY_TAB       : AutoComplete(); break;
      case GLFW_KEY_INSERT    : ToggleCursorMode(); break;
      // Unknown key (ignore)
      default: break;
    }
  }
  /* -- Pop the back of the before cursor string --------------------------- */
  void PopInputBeforeCursor(void)
    { if(UtfPopBack(strConsoleBegin)) SetRedraw(); }
  /* -- Pop the front the after cursor string ------------------------------ */
  void PopInputAfterCursor(void)
    { if(UtfPopFront(strConsoleEnd)) SetRedraw(); }
  /* -- Move cursor left --------------------------------------------------- */
  void CursorLeft(void)
    { if(UtfMoveBackToFront(strConsoleBegin, strConsoleEnd)) SetRedraw(); }
  /* -- Move cursor right -------------------------------------------------- */
  void CursorRight(void)
    { if(UtfMoveFrontToBack(strConsoleEnd, strConsoleBegin)) SetRedraw(); }
  /* -- Move cursor to beginning of line ----------------------------------- */
  void CursorHome(void)
  { // Ignore if no end text
    if(strConsoleBegin.empty()) return;
    // Prepend before cursor text to after cursor text and clear before text
    strConsoleEnd.swap(strConsoleBegin);
    strConsoleEnd += strConsoleBegin;
    strConsoleBegin.clear();
    // Redraw the buffer, it changed
    SetRedraw();
  }
  /* -- Move cursor to end of line ----------------------------------------- */
  void CursorEnd(void)
  { // Ignore if no end text
    if(strConsoleEnd.empty()) return;
    // Set beginning of text to end of text and clear after cursor text
    strConsoleBegin += strConsoleEnd;
    strConsoleEnd.clear();
    // Redraw the buffer, it changed
    SetRedraw();
  }
  /* -- Scroll to the specified text in console backlog -------------------- */
  bool FindText(const string &strWhat)
  { // Return if we find the specified text where the viewer is viewing
    if(empty() || clriPosition == crend()) return false;
    // Find string in list and return if not found
    const ConLinesConstRevIt clcriIt{
      StdFindIf(seq, next(clriPosition, 1), crend(),
        [&strWhat](const ConLine &clLine)->bool
          { return clLine.strLine.find(strWhat) != StdNPos; }) };
    if(clcriIt == crend()) return false;
    // Set position where we found it
    clriPosition = clcriIt;
    // Redraw the console
    SetRedraw();
    // Block the execute function from updating the output position
    FlagSet(CF_BLOCKOUTPUTUPDATE);
    // We're done
    return true;
  }
  /* -- Move log by a certain amount --------------------------------------- */
  void MoveLogPage(const ConLinesConstRevIt &clriBegin,
    const ConLinesConstRevIt &clriEnd, const ConLinesConstRevIt &clriReset,
    const ssize_t sstAmount, const ssize_t sstMove)
  { // Ignore if already at reset position
    if(clriPosition == clriReset) return;
    // Redrawing
    SetRedraw();
    // If we can advance that far? distance should always return positive
    // value since we're moving forwards
    if(distance(clriBegin, clriEnd) < sstAmount) clriPosition = clriReset;
    // Can safely advance forwards
    else advance(clriPosition, sstMove);
  }
  /* -- Functions to move the active console line -------------------------- */
  void MoveLogHome(void)
    { if(clriPosition != crend()){ SetRedraw(); clriPosition = crend(); } }
  void MoveLogEnd(void)
    { if(clriPosition != crbegin()){ SetRedraw(); clriPosition = crbegin(); }}
  void MoveLogUp(void)
    { if(clriPosition != crend()){ SetRedraw(); ++clriPosition; }}
  void MoveLogDown(void)
    { if(clriPosition != crbegin()){ SetRedraw(); --clriPosition; }}
  void MoveLogPageUp(void)
    { MoveLogPage(clriPosition, crend(), crend(),
        sstPageLines, sstPageLines); }
  void MoveLogPageDown(void)
    { MoveLogPage(crbegin(), clriPosition, crbegin(),
        sstPageLines, sstPageLinesNeg); }
  /* -- OnLastItem event ---------------------- Selects last console item -- */
  void HistoryMoveBack(void)
  { // Ignore if no history lines or there is text after the cursor
    if(slHistory.empty() || !strConsoleEnd.empty()) return;
    // Set last typed item if not initialised
    if(slriInputPosition == slHistory.crend())
      slriInputPosition = slHistory.crbegin();
    // Move back towards beginning of list if we can
    else if(next(slriInputPosition, 1) != slHistory.crend())
      ++slriInputPosition;
    // Set line from history
    strConsoleBegin = *slriInputPosition;
    strConsoleEnd.clear();
    // Redraw the buffer, it changed
    SetRedraw();
  }
  /* -- OnNextItem event --------------------- Selects ntext console item -- */
  void HistoryMoveForward(void)
  { // Ignore if no history lines, history not initialised or there is text
    // after the cursor
    if(slHistory.empty() || !strConsoleEnd.empty() ||
       slriInputPosition == slHistory.crend()) return;
    // Move forward towards end if we can
    if(slriInputPosition != slHistory.crbegin()) --slriInputPosition;
    // Set line from history
    strConsoleBegin = *slriInputPosition;
    strConsoleEnd.clear();
    // Redraw the buffer, it changed
    SetRedraw();
  }
  /* -- Clear console input history ---------------------------------------- */
  void ClearHistory(void)
  { // Clear history list
    slHistory.clear();
    // No history selected
    slriInputPosition = slHistory.crend();
  }
  /* -- Clear console line ------------------------------------------------- */
  void ClearInput(void)
  { // Clear text before and after cursor
    DoClearInput();
    // Redraw the buffer, it changed
    SetRedraw();
  }
  /* -- Input manipulation ------------------------------------------------- */
  void AddInputChar(const unsigned int uiChar)
  { // Make sure line is under max characters
    if(strConsoleBegin.size() + strConsoleEnd.size() >= stMaxInputLine)
      return;
    // Encode character to utf-8
    UtfAppend(uiChar, strConsoleBegin);
    // Redraw the buffer, it changed
    SetRedraw();
  }
  /* -- Input manipulation ------------------------------------------------- */
  void ReplaceInputChar(const unsigned int uiChar)
  { // Add character if we are at the end of the line input
    if(strConsoleEnd.empty()) return AddInputChar(uiChar);
    // Encode character to utf-8
    UtfAppend(uiChar, strConsoleBegin);
    // Remove character from beginning of end of line input
    UtfPopFront(strConsoleEnd);
    // Redraw the buffer, it changed
    SetRedraw();
  }
  /* -- Toggle insert and overwrite mode ----------------------------------- */
  void ToggleCursorMode(void)
  { // Toggle cursor mode
    FlagToggle(CF_INSERT);
    // Tell SysCon that the cursor changed if needed and return
    cSystem->SetCursorMode(FlagIsSet(CF_INSERT));
  }
  /* -- Returns if the console should be visible --------------------------- */
  bool IsVisible(void) { return FlagIsSet(CF_ENABLED); }
  bool IsNotVisible(void) { return !IsVisible(); }
  /* -- Sets redraw flag so the console fbo or terminal buffer is redrawn -- */
  void SetRedraw(void)
    { GetRedrawFlags().FlagReset(GetDefaultRedrawFlags().FlagIsSet(RD_TEXT) ||
        IsVisible() ? GetDefaultRedrawFlags() : RD_NONE); }
  /* -- Do Set Console status ---------------------------------------------- */
  bool DoSetVisible(const bool bState)
  { // Enabling and not enabled?
    if(bState && IsNotVisible())
    { // Set console enabled and redraw the buffer
      FlagSet(CF_ENABLED);
      // Redraw console
      SetRedraw();
      // Log that the console has been enabled
      cLog->LogDebugSafe("Console has been enabled.");
    } // Disabled and not disabled?
    else if(!bState && IsVisible())
    { // Set console disabled and clear redraw flag
      FlagClear(CF_ENABLED);
      // Redraw console
      SetRedraw();
      // Log that the console has been disabled
      cLog->LogDebugSafe("Console has been disabled.");
    } // Say that nothing changed
    else
    { // Log the request
      cLog->LogDebugExSafe("Console has already been $.",
        bState ? "enabled" : "disabled");
      // Failed
      return false;
    } // Success
    return true;
  }
  /* -- Execute arguments list --------------------------------------------- */
  void ExecuteArguments(const string &strCmd, const Args &aList)
  { // Get var or command name
    const string &strVarOrCmd = aList.front();
    if(strVarOrCmd.empty()) return;
    // Get if we're already at the bottom of the log
    const bool bAtBottom = clriPosition == rbegin();
    // Make sure that all lines are processed immediately
    while(!clqOutput.empty())
    { // Move item into main console render buffer
      emplace_back(StdMove(clqOutput.front()));
      // remove the old item
      clqOutput.pop();
    } // Dump whole input to log
    push_back({ cLog->CCDeltaToDouble(),
      COLOUR_YELLOW, StrAppend('>', strCmd) });
    // Add command to input history
    AddHistory(strCmd);
    // Find console callback function and function not found?
    const CmdMapConstIt cmciIt{ cmMap.find(strVarOrCmd) };
    if(cmciIt == cmMap.cend())
    { // Output string
      ostringstream osS;
      // If the cvar exists?
      const CVarMapIt cvmMapIt{ cCVars->FindVariable(strVarOrCmd) };
      if(cvmMapIt != cCVars->GetVarListEnd())
      { // Resulting message
        osS << "Variable '" << strVarOrCmd << "' ";
        // No value specified?
        if(aList.size() <= 1)
        { // Show it
          osS << "is currently " << cCVars->GetValueSafe(cvmMapIt) << "!";
          // Copy it to clipboard if requested
          if(FlagIsSet(CF_AUTOCOPYCVAR))
            cGlFW->WinSetClipboardString(StrFormat("$ \"$\"",
              strVarOrCmd, cCVars->GetStr(cvmMapIt)));
        } // Else set item and get return value
        else switch(aList[1] == "~" ?
          cCVars->Reset(cvmMapIt, PCONSOLE, CCF_NOTHING) :
          cCVars->Set(cvmMapIt, aList[1], PCONSOLE, CCF_NOTHING))
        { // Success. Show result
          case CVS_OK:
            osS << "is now " << cCVars->GetValueSafe(cvmMapIt) << '!'; break;
          // Success. Show result
          case CVS_OKNOTCHANGED: osS << "not changed!"; break;
          // Cvar not found (THIS SHOULDNT HAPPEN!!!)
          case CVS_NOTFOUND: osS << "not found!"; break;
          // Not writable
          case CVS_NOTWRITABLE: osS << "is read-only!"; break;
          // Invalid filename
          case CVS_NOTFILENAME: osS << "is an invalid filename!"; break;
          // Invalid integer
          case CVS_NOTINTEGER: osS << "must be an integer!"; break;
          // Invalid double
          case CVS_NOTFLOAT: osS << "must be a float!"; break;
          // Invalid boolean
          case CVS_NOTBOOLEAN: osS << "must be a boolean!"; break;
          // Not greater or equal than zero
          case CVS_NOTUNSIGNED: osS << "must be equal or over zero!"; break;
          // Not power of two
          case CVS_NOTPOW2: osS << "must be power of two!"; break;
          // Must only contain letters
          case CVS_NOTALPHA: osS << "must contain only letters!"; break;
          // Must only contain digits
          case CVS_NOTNUMERIC: osS << "must contain only digits!"; break;
          // Must only contain letters or digits
          case CVS_NOTALPHANUMERIC:
            osS << "must contain only alphanumeric characters!"; break;
          // Trigger denied
          case CVS_TRIGGERDENIED: osS << "could not be updated!"; break;
          // exception in trigger
          case CVS_TRIGGEREXCEPTION:
            osS << "not set due to exception! " << cCVars->GetCBError(); break;
          // String empty
          case CVS_EMPTY: osS << "must not be empty!"; break;
          // Integer is zero
          case CVS_ZERO: osS << "must not be zero!"; break;
          // No type is set
          case CVS_NOTYPESET: osS << "not set due to unknown type!"; break;
          // Unknown error
          default: osS << "not set due to unknown error!"; break;
        }
      } // Try to set initial var if it exists
      else if(cCVars->InitialVarExists(strVarOrCmd))
      { // Resulting message
        osS << "Persistent variable '" << strVarOrCmd << "' ";
        // No value specified?
        if(aList.size() <= 1)
        { // Write that it is persistant
          osS << "exists!";
          // Copy to clipboard
          if(FlagIsSet(CF_AUTOCOPYCVAR))
            cGlFW->WinSetClipboardString(StrFormat("$ \"$\"",
              strVarOrCmd, cCVars->GetInitialVar(strVarOrCmd)));
        } // Set the value and say if it worked
        else if(cCVars->SetExistingInitialVar(strVarOrCmd, aList[1]))
          osS << "was set to '" << aList[1] << "'!";
        // Not changed
        else osS << "was not changed!";
      } // Cvar doesn't exist
      else osS << "Bad command or cvar name!";
      // Write result
      AddLine(osS.str());
    } // Command found?
    else
    { // Get data structure and check if enough parameters specified
      const ConLib &clData = cmciIt->second;
      if(clData.uiMinimum && aList.size() < clData.uiMinimum)
        AddLine("Required parameter missing!");
      else if(clData.uiMaximum && aList.size() > clData.uiMaximum)
        AddLine("Too many parameters!");
      // Enough parameters so capture exceptions so we can't halt execution
      else try { clData.ccbFunc(aList); }
      // exception did occur
      catch(const exception &eReason)
      { // Print the output in the console
        AddLineA("Console CB failed! > ", eReason);
        // Force the console to be shown because the callback might have
        // hidden the console
        DoSetVisible(true);
        // Force scroll to bottom
        clriPosition = rbegin();
      } // Carry on executing as normal
    } // Remove update block flag if set
    if(FlagIsSet(CF_BLOCKOUTPUTUPDATE)) FlagClear(CF_BLOCKOUTPUTUPDATE);
    // Were we already at the bottom or should we auto-scroll?
    else if(bAtBottom || FlagIsSet(CF_AUTOSCROLL))
    { // Autoscroll to bottom enabled? Scroll to bottom
      clriPosition = rbegin();
      // Set console to redraw for input text at least
      SetRedraw();
    }
  }
  /* -- Process queued console lines --------------------------------------- */
  void MoveQueuedLines(void)
  { // If there are console lines in the queue?
    if(clqOutput.empty()) return;
    // Stagger the queue to the output buffer but enough so it completes fast
    // and compensates for a growing queue. At minimum the number of elements
    // or the most of half of the number of elements or five.
    size_t stLines =
      UtilMinimum(clqOutput.size(), UtilMaximum(5, clqOutput.size()/4));
    // Calculate total lines
    const size_t stTotal = size() + stLines;
    // If writing this many lines wouldn't fit in the log, then the log needs
    // pruning.
    if(stTotal > GetOutputMaximum())
    { // If not too many lines would be written?
      if(stLines < GetOutputMaximum())
      { // Iterator to find
        const ConLinesConstIt clciIt{ clriPosition.base() };
        // Lines to prune to
        const size_t stRemove = size() - stLines;
        // Repeat...
        do
        { // If this item is not selected in the output history? Erase and next
          if(cbegin() != clciIt) { pop_front(); continue; }
          // Get items remaining to remove
          clriPosition = ConLinesConstRevIt{ erase(cbegin(),
            next(cbegin(), static_cast<ssize_t>(size() - stRemove))) };
          // Done
          break;
        } // ...Until we have cleared enough lines
        while(size() > stRemove);
      } // Too many lines would be written
      else
      { // Clear all lines
        clear();
        // Reset scroll position
        clriPosition = rbegin();
      }
    } // Get if we're at the bottom of the log
    const bool bAtBottom = clriPosition == rbegin();
    // Repeat...
    do
    { // Move item into main console render buffer
      emplace_back(StdMove(clqOutput.front()));
      // remove the old item
      clqOutput.pop();
    } // ...until we've moved enough lines
    while(--stLines);
    // Auto scroll enabled or were already at the bottom of log? Set the log
    // to the bottom.
    if(FlagIsSet(CF_AUTOSCROLL) || bAtBottom) clriPosition = rbegin();
    // Redraw the buffer, it changed
    SetRedraw();
  }
  /* -- SetRedraw -------------------------------------------------- */ public:
  void SetRedrawIfEnabled(void) { if(IsVisible()) SetRedraw(); }
  /* -- Get console lines -------------------------------------------------- */
  size_t GetOutputCount(void) { return size(); }
  size_t GetInputCount(void) { return slHistory.size(); }
  /* -- Get buffer position ------------------------------------------------ */
  const ConLinesConstRevIt GetConBufPos(void) const { return clriPosition; }
  const ConLinesConstRevIt GetConBufPosEnd(void) { return rend(); }
  /* -- Set console input status bar (left and right) ---------------------- */
  void SetStatusLeft(const string &strValue) { strStatusLeft = strValue; }
  void SetStatusRight(const string &strValue) { strStatusRight = strValue; }
  /* -- Clear both statuses ------------------------------------------------ */
  void ClearStatus(void)
  { // Clear left if empty
    if(!strStatusLeft.empty())
    { // Clear left and right side text
      strStatusLeft.clear();
      strStatusRight.clear();
    } // Left text already empty so clear right status if empty
    else if(!strStatusRight.empty()) strStatusRight.clear();
    // Nothing changed
    else return;
    // Redrawing screen again
    SetRedraw();
  }
  /* -- Tick for bot render ------------------------------------------------ */
  void FlushToLog(void)
  { // Process queued console log lines
    MoveQueuedLines();
    // If thread delay enabled and input polling interval ready to poll?
    if(cTimer->TimerGetDelay() != 0.0 || ciInputRefresh.CITriggerStrict())
    {  // Loop forever until no more keys are pressed
      for(int iKey, iMods;;) switch(cSystem->GetKey(iKey, iMods))
      { // No key is pressed (ignore)
        case SysCon::KT_NONE: goto Done;
        // A key was pressed?
        case SysCon::KT_KEY:
          // Send it to the key press handler
          OnKeyPress(iKey, GLFW_PRESS, iMods);
          // Try more keys
          continue;
        // A scan code was pressed?
        case SysCon::KT_CHAR:
          // Send it to the key character handler
          OnCharPress(static_cast<unsigned int>(iKey));
          // Try more keys
          continue;
        // No key is pressed but a reset was requested?
        case SysCon::KT_RESET:
          // Set to redraw entire screen
          ciOutputRefresh.CISync();
          rfFlags.FlagSet(RD_TEXT);
          // Break the for loop
          goto Done;
        // Unknown command (ignore)
        default: break;
      } // Only clean way to double-break without extra vars or functions
      Done:;
    } // Status update elapsed?
    if(ciOutputRefresh.CITriggerStrict())
    { // Update memory and cpu status
      cSystem->UpdateMemoryUsageData();
      cSystem->UpdateCPUUsage();
      // Redraw title
      cSystem->RedrawTitleBar(StrFormat("CPU:$$$%  FPS:$$  MEM:$  NET:$  UP:$",
        fixed, setprecision(1), cSystem->CPUUsage(), setprecision(0),
        cTimer->TimerGetFPS(), StrToBytes(cSystem->RAMProcUse(), 0),
        cSockets->stConnected.load(),
        StrShortFromDuration(cLog->CCDeltaToDouble(), 0)),
        cmSys.FormatTime(strvTimeFormat.data()));
      // Not redrawing?
      if(GetRedrawFlags().FlagIsClear(RD_TEXT))
      { // Redraw status if imput empty. Input redraw handled elseware.
        if(InputEmpty())
          cSystem->RedrawStatusBar(strStatusLeft, strStatusRight);
        // Commit and return buffer
        return cSystem->CommitBuffer();
      }
    } // Done if not redrawing
    else if(rfFlags.FlagIsClear(RD_TEXT)) return;
    // Reset redraw flag as we're about to draw
    rfFlags.FlagClear(RD_TEXT);
    // Redraw the main text buffer
    cSystem->RedrawBuffer(*this, clriPosition);
    // If no text is inputted? Redraw customised status bar
    if(InputEmpty()) cSystem->RedrawStatusBar(strStatusLeft, strStatusRight);
    // Text inputted so redraw input bar
    else cSystem->RedrawInputBar(strConsoleBegin, strConsoleEnd);
    // Commit buffer if we need to commit
    cSystem->CommitBuffer();
  }
  /* -- Copy all console lines to log -------------------------------------- */
  size_t ToLog(void)
  { // Write all console lines to log and return lines in buffer
    for(const ConLine &clItem : *this) cLog->LogNLCDebugSafe(clItem.strLine);
    return size();
  }
  /* -- Command exists? ---------------------------------------------------- */
  bool CommandIsRegistered(const string &strName) const
    { return cmMap.contains(strName); }
  /* -- Register console command ------------------------------------------- */
  const CmdMapIt RegisterCommand(const string &strName,
    const unsigned int uiMin, const unsigned int uiMax,
    const ConCbFunc ccbFunc)
  { // Insert trusted command into commands list
    return cmMap.insert({ strName,
      { strName, uiMin, uiMax, CFL_NONE, ccbFunc } }).first;
  }
  /* -- Unregister console command ----------------------------------------- */
  void UnregisterCommand(const CmdMapIt &cmiIt) { cmMap.erase(cmiIt); }
  /* -- Add line as string with specified text colour ---------------------- */
  void AddLine(const Colour cColour, const string &strText)
  { // Tokenise lines into a list limited by the maximum number of lines.
    if(const TokenList tlLines{
      strText, cCommon->CommonLf(), GetOutputMaximum() })
    { // Add all the lines to the output queue
      const double dTime = cLog->CCDeltaToDouble();
      for(const string &strLine : tlLines)
      { // Move the line across if it is long enough
        if(strLine.length() <= stMaxOutputLine)
          clqOutput.push({ dTime, cColour, StdMove(strLine) });
        // Push a truncated line
        else clqOutput.push({ dTime, cColour,
          strLine.substr(0, stMaxOutputLineE) + cCommon->CommonEllipsis() });
      }
    }
  }
  /* -- Add line as string with default text colour ------------------- */
  void AddLine(const string &strText) { AddLine(cTextColour, strText); }
  /* -- Formatted console output ------------------------------------------- */
  template<typename ...VarArgs>
    void AddLineF(const char*const cpFormat, const VarArgs &...vaArgs)
      { AddLine(StrFormat(cpFormat, vaArgs...)); }
  /* -- Formatted console output with colour ------------------------------- */
  template<typename ...VarArgs>
    void AddLineF(const Colour cColour, const char*const cpFormat,
      const VarArgs &...vaArgs)
        { AddLine(cColour, StrFormat(cpFormat, vaArgs...)); }
  /* -- Formatted console output using StrAppend() ------------------------- */
  template<typename ...VarArgs>
    void AddLineAC(const Colour cColour, const VarArgs &...vaArgs)
      { AddLine(cColour, StrAppend(vaArgs...)); }
  template<typename ...VarArgs>
    void AddLineA(const VarArgs &...vaArgs)
      { AddLineAC(cTextColour, vaArgs...); }
  /* -- Print version information ------------------------------------------ */
  void PrintVersion(void)
  { // Show engine version
    AddLineF(
      "$ ($) version $.$.$ build $ for $.\n"
      "Compiled $ using $ version $.\n"
      "Copyright \xC2\xA9 $, 2006-$. All Rights Reserved.",
      cSystem->ENGName(), cSystem->ENGBuildType(), cSystem->ENGMajor(),
      cSystem->ENGMinor(), cSystem->ENGBuild(), cSystem->ENGRevision(),
      cSystem->ENGTarget(), cSystem->ENGCompiled(), cSystem->ENGCompiler(),
      cSystem->ENGCompVer(), cSystem->ENGAuthor(), cmSys.FormatTime("%Y"));
    // Add disclaimer that the author of the engine disclaims all liability for
    // guest software actions and user usage.
    AddLine(COLOUR_RED, "Disclaimer: This scripting ENGINE is designed ONLY "
      "for legitimate and lawful multimedia solutions and is provided AS IS "
      "with ZERO warranty. It also contains very strong cryptographic "
      "technologies which might not be allowed to be imported in your "
      "country. By using this software, whether you are the GUEST author or "
      "the END user, you accept that the ENGINE author and ALL the authors of "
      "ALL the components listed in the 'credits' command output disclaim ALL "
      "liability for how the GUEST author or the END user chooses to use this "
      "software.");
    // Write guest info in a different colour
    AddLineF(COLOUR_GREEN, "Guest is $ ($) version $ by $.",
      cSystem->GetGuestTitle(), cSystem->GetGuestShortTitle(),
      cSystem->GetGuestVersion(), cSystem->GetGuestAuthor());
    // Get optional variables
    if(!cSystem->GetGuestCopyright().empty())
      AddLineAC(COLOUR_GREEN, cSystem->GetGuestCopyright(), '.');
    if(!cSystem->GetGuestDescription().empty())
      AddLineAC(COLOUR_GREEN, cSystem->GetGuestDescription(), '.');
    if(!cSystem->GetGuestWebsite().empty())
      AddLineF(COLOUR_GREEN, "Visit $ for more info, help and updates.",
        cSystem->GetGuestWebsite());
  }
  /* -- Print a string using textures -------------------------------------- */
  void Init(void)
  { // Class intiialised
    IHInitialise();
    // Log progress
    cLog->LogDebugExSafe("Console initialising with $ built-in commands...",
      ccslInt.size());
    // Reset cursor position
    clriPosition = rbegin();
    // Using text mode?
    if(cSystem->IsTextMode())
    { // Add flag for it
      GetDefaultRedrawFlags().FlagReset(RD_TEXT);
      // Register console events
      cEvtMain->RegisterEx(emrvEvents);
    } // Redraw the console
    SetRedraw();
    // Initially shown and not closable
    FlagSet(CF_CANTDISABLE|CF_ENABLED|CF_INSERT);
    // Show version information
    PrintVersion();
    // Iterate each item and register command if required core flags match
    for(const ConLibStatic &clCmd : ccslInt)
      if(cSystem->IsCoreFlagsHave(clCmd.cfcRequired))
        RegisterCommand(string(clCmd.strvName), clCmd.uiMinimum,
          clCmd.uiMaximum, clCmd.ccbFunc);
      // Write in log to say we skipped registration of this command
      else cLog->LogDebugExSafe(
        "Console ignoring registration of command '$'.", clCmd.strvName);
    // Say how many commands we registered
    cLog->LogInfoExSafe("Console initialised with $ of $ built-in commands.",
      cmMap.size(), ccslInt.size());
  }
  /* -- DeInit ------------------------------------------------------------- */
  void DeInit(void)
  { // Ignore if already deinitialised
    if(IHNotDeInitialise()) return;
    // Log progress
    cLog->LogDebugSafe("Console de-initialising...");
    // Initially shown and not closable. All other flags removed.
    FlagReset(CF_CANTDISABLE|CF_ENABLED|CF_INSERT);
    // Unregister console events if using text mode
    if(cSystem->IsTextMode()) cEvtMain->UnregisterEx(emrvEvents);
    // If commands registered?
    switch(const size_t stCount = cmMap.size())
    { // Impossible?
      case 0: break;
      // Anything else?
      default:
        // Remove all commands quickly and report it
        cLog->LogDebugExSafe("Console flushing $ commands...", stCount);
        cmMap.clear();
        cLog->LogDebugExSafe("Console flushed $ commands.", stCount);
        // Done
        break;
    } // Clear console input, output and input history
    DoClearInput();
    ClearHistory();
    DoFlush();
    // Log progress
    cLog->LogInfoSafe("Console de-initialised successfully.");
  }
  /* -- Constructor --------------------------------------------- */ protected:
  explicit Console(const ConCmdStaticList &ccslDef) :
    /* -- Initialisers ----------------------------------------------------- */
    InitHelper{ __FUNCTION__ },        // Init helper function name
    Flags{ CF_NONE },                  // No initial flags
    clriPosition{ rbegin() },          // Input position at beginning
    slriInputPosition{                 // Init log position...
      slHistory.crend() },             // ...at beginning
    stInputMaximum(0),                 // No maximum input characters
    stOutputMaximum(0),                // No maximum output lines
    stMaxInputLine(0),                 // No maximum characters per line
    stMaxOutputLine(0),                // No maximum output line
    stMaxOutputLineE(0),               // No maximum output line with ellipsis
    sstPageLines(0),                   // No page up/down lines setting
    sstPageLinesNeg(0),                // No neg page up/down lines setting
    rfDefault{ RD_NONE },              // Default redraw initially set by Init
    rfFlags{ RD_NONE },                // Redraw type
    cTextColour(COLOUR_WHITE),         // Default white text colour
    acFlags{ AC_NONE },                // No autocomplete flags
    ccslInt{ ccslDef },                // Set default commands list
    /* --------------------------------------------------------------------- */
    emrvEvents{                        // Default events
      { EMC_CON_UPDATE, bind(&Console::OnForceRedraw, this, _1) },
    }
    /* -- Set global pointer to static class ------------------------------- */
    { cConsole = this; }
  /* -- Set page move count ---------------------------------------- */ public:
  CVarReturn SetPageMoveCount(const ssize_t sstAmount)
  { // Deny if invalid value
    if(!CVarToBoolReturn(CVarSimpleSetIntNLG(sstPageLines, sstAmount, 1, 100)))
      return DENY;
    // Set negative too
    sstPageLinesNeg = -sstPageLines;
    // Success
    return ACCEPT;
  }
  /* -- Set time format ---------------------------------------------------- */
  CVarReturn SetTimeFormat(const string &strFmt, const string &strV)
  { // Verify the new time format and log it
    cLog->LogInfoExSafe("Console time from format '$' is '$'.",
      strFmt, cmSys.FormatTime(strFmt.c_str()));
    // Accept the new time format
    strvTimeFormat = strV;
    // Done
    return ACCEPT;
  }
  /* -- Set maximum console line length ------------------------------------ */
  CVarReturn SetMaxInputChars(const size_t stChars)
  { // Deny if out of range
    if(stChars < 256 ||
       stChars > UtilMinimum(UtilMinimum(strConsoleBegin.max_size(),
                                         strConsoleEnd.max_size()), 16384))
      return DENY;
    // Reserve buffer sizes for console input
    strConsoleBegin.reserve(stChars);
    strConsoleEnd.reserve(stChars);
    // Set new maximum length
    stMaxInputLine = stChars;
    // Redraw the buffer, it changed
    SetRedraw();
    // Value allowed
    return ACCEPT;
  }
  /* -- Set maximum console line length ------------------------------------ */
  CVarReturn SetMaxOutputChars(const size_t stChars)
  { // Check, set the new value and return if not acceptable
    if(!CVarToBoolReturn(CVarSimpleSetIntNLG(stMaxOutputLine, stChars,
      1024UL, 65536UL)))
        return DENY;
    // Set subtracted value from ellipsis size
    stMaxOutputLineE = stMaxOutputLine - cCommon->CommonEllipsis().length();
    // Value accepted
    return ACCEPT;
  }
  /* -- Set console buffers ------------------------------------------------ */
  CVarReturn SetConsoleOutputLines(const size_t stLines)
  { // Ignore if same value or invalid value or vector can't accept value
    if(stLines < 1 || stLines > 1000000 || stLines > max_size()) return DENY;
    // New value under previous amount? Remove excess lines
    if(stLines < size())
      clriPosition = ConLinesConstRevIt{ erase(cbegin(), next(cbegin(),
        static_cast<ssize_t>(size() - stLines))) };
    // Set new position
    stOutputMaximum = stLines;
    // Completed successfully
    return ACCEPT;
  }
  /* -- Set console buffers ------------------------------------------------ */
  CVarReturn SetConsoleInputLines(const size_t stLines)
  { // OK if same value
    if(GetInputMaximum() == stLines) return ACCEPT;
    // Ignore if same value or invalid value or vector can't accept value
    if(stLines < 1 || stLines > 1000000 || stLines > slHistory.max_size())
      return DENY;
    // New value under previous amount? Erase the rest
    if(stLines < slHistory.size())
      slriInputPosition =
        StrListConstRevIt{ slHistory.erase(slHistory.cbegin(),
          next(slHistory.cbegin(),
            static_cast<ssize_t>(slHistory.size() - stLines))) };
    // Set the new amount
    stInputMaximum = stLines;
    // Completed
    return ACCEPT;
  }
  /* -- Set console auto complete flags ------------------------------------ */
  CVarReturn SetAutoComplete(const AutoCompleteFlagsType acftFlags)
  { // Deny if flags are too much
    if(acftFlags != AC_NONE && (acftFlags & ~AC_MASK)) return DENY;
    // Set new flags
    acFlags.FlagReset(acftFlags);
    // Success
    return ACCEPT;
  }
  /* -- Set console auto copy cvar state ----------------------------------- */
  CVarReturn SetAutoCopyCVar(const bool bState)
    { FlagSetOrClear(CF_AUTOCOPYCVAR, bState); return ACCEPT; }
  /* -- Set console autoscroll state --------------------------------------- */
  CVarReturn SetAutoScroll(const bool bState)
    { FlagSetOrClear(CF_AUTOSCROLL, bState); return ACCEPT; }
  /* -- Set console input refresh rate ------------------------------------- */
  CVarReturn InputRefreshModified(const unsigned int uiMicroseconds)
    { if(uiMicroseconds < 1000 || uiMicroseconds > 1000000) return DENY;
      ciInputRefresh.CISetLimit(microseconds{ uiMicroseconds });
      return ACCEPT; }
  /* -- Set text-mode console refresh rate --------------------------------- */
  CVarReturn OutputRefreshModified(const unsigned int uiMilliseconds)
    { if(uiMilliseconds < 100 || uiMilliseconds > 1000) return DENY;
      ciOutputRefresh.CISetLimit(milliseconds{ uiMilliseconds });
      return ACCEPT; }
  /* -- Set console text colour -------------------------------------------- */
  CVarReturn TextForegroundColourModified(const unsigned int uiNewColour)
    { return CVarSimpleSetIntNG(cTextColour, static_cast<Colour>(uiNewColour),
        COLOUR_MAX); }
};/* ----------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
