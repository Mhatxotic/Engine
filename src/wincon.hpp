/* == WINCON.HPP =========================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## This is a Windows specific module that handles text only mode       ## **
** ## output which is needed by the engines bot-mode.                     ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
// -- Palette entry to WIN32 colour lookup --------------------------------- */
typedef array<const WORD, COLOUR_MAX> ColourList;
static const ColourList wNDXtoW32C{
  0,                             FOREGROUND_BLUE,
  FOREGROUND_GREEN,              FOREGROUND_GREEN|FOREGROUND_BLUE,
  FOREGROUND_RED,                FOREGROUND_RED|FOREGROUND_BLUE,
  FOREGROUND_RED|FOREGROUND_GREEN,
  FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE,
  FOREGROUND_INTENSITY,          FOREGROUND_INTENSITY|FOREGROUND_BLUE,
  FOREGROUND_INTENSITY|FOREGROUND_GREEN,
  FOREGROUND_INTENSITY|FOREGROUND_GREEN|FOREGROUND_BLUE,
  FOREGROUND_INTENSITY|FOREGROUND_RED,
  FOREGROUND_INTENSITY|FOREGROUND_RED|FOREGROUND_BLUE,
  FOREGROUND_INTENSITY|FOREGROUND_RED|FOREGROUND_GREEN,
  FOREGROUND_INTENSITY|FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE,
};
// -- Palette entry to WIN32 colour lookup --------------------------------- */
static const ColourList wNDXtoW32BC{
  0,                             BACKGROUND_BLUE,
  BACKGROUND_GREEN,              BACKGROUND_GREEN|BACKGROUND_BLUE,
  BACKGROUND_RED,                BACKGROUND_RED|BACKGROUND_BLUE,
  BACKGROUND_RED|BACKGROUND_GREEN,
  BACKGROUND_RED|BACKGROUND_GREEN|BACKGROUND_BLUE,
  BACKGROUND_INTENSITY,          BACKGROUND_INTENSITY|BACKGROUND_BLUE,
  BACKGROUND_INTENSITY|BACKGROUND_GREEN,
  BACKGROUND_INTENSITY|BACKGROUND_GREEN|BACKGROUND_BLUE,
  BACKGROUND_INTENSITY|BACKGROUND_RED,
  BACKGROUND_INTENSITY|BACKGROUND_RED|BACKGROUND_BLUE,
  BACKGROUND_INTENSITY|BACKGROUND_RED|BACKGROUND_GREEN,
  BACKGROUND_INTENSITY|BACKGROUND_RED|BACKGROUND_GREEN|BACKGROUND_BLUE,
};
/* -- Windows Console Class ------------------------------------------------ */
class SysCon :                         // Members initially private
  /* -- Base classes ------------------------------------------------------- */
  public SysBase,                      // Defined in 'winbase.hpp'
  public SysConBase                    // Defined in 'syscore.hpp'
{ /* -- Private typedefs --------------------------------------------------- */
  typedef vector<CHAR_INFO> CharInfoVec;
  /* -- Private variables -------------------------------------------------- */
  const string    &strWine;            // Wine version
  /* -- Console data ----------------------------------------------- */ public:
  HANDLE           hIn, hOut;          // Handle to stdin and stdout
  WORD             wColour,            // Current colour
                   wColourSaved;       // Saved colour
  CharInfoVec      civBuf;             // Display buffers
  /* -- Co-ordinates and limits -------------------------------------------- */
  DWORD            dwCurSize;          // Current Win32Cursor size
  SHORT            sCurX, sCurY;       // Current Win32 cursor position
  size_t           stEndPos,           // End position
                   stX, stY,           // X/Y cursor position
                   stW, stH,           // Console width and height
                   stWm1, stWm2,       // Console width minus 1 and 2
                   stHm1, stHm2,       // Console height minus 1 and 2
  /* -- Bounds ------------------------------------------------------------- */
                   stX1, stY1,         // These are the bounds that have
                   stX2, stY2;         // been updated in the last frame
  /* -- Reset drawing bounds ----------------------------------------------- */
  void ResetDrawingBounds(void)
    { stX1 = stY1 = StdMaxSizeT; stX2 = stY2 = 0; }
  /* -- Control and break handler (thiscall) ------------------------------- */
  static BOOL WINAPI CtrlHandlerStatic(DWORD);
  BOOL WINAPI CtrlHandler(DWORD dwCtrlType)
  { // Get event information
    const char *cpEvent;
    switch(dwCtrlType)
    { // Note that we store a boolean in the first character of the string to
      // say if this event has a timer set by Windows that will forcefully
      // terminate the app when expired. '.' means no, '!' is yes.
      case CTRL_C_EVENT        : cpEvent = ".ctrl+c";     break;
      case CTRL_CLOSE_EVENT    : cpEvent = "!close";      break;
      case CTRL_BREAK_EVENT    : cpEvent = ".ctrl+break"; break;
      case CTRL_LOGOFF_EVENT   : cpEvent = "!logoff";     break;
      case CTRL_SHUTDOWN_EVENT : cpEvent = "!shutdown";   break;
      default                  : cpEvent = ".unknown";    break;
    } // Log event
    cLog->LogWarningExSafe(
      "SysCon got operating system event $<$>, shutting down...",
        cpEvent+1, dwCtrlType);
    // Because windows will terminate my process after this function returns
    // We'll need to wait for the exit
    if(*cpEvent == '!')
    { // Lock mutex
      UniqueLock ulExit{ mExit };
      // Ignore if we've already exited which can happen if shutdown and break
      // events occur at the same time.
      if(FlagIsClear(SCO_EXIT))
      { // Set exit flag so when main loop informs us that it cleaned up
        // properly, it can exit immediately cleaning everything up.
        FlagSet(SCO_EXIT);
        // Show warning
        cLog->LogWarningSafe("WARNING: Windows wants this application to "
          "shut down NOW! Consider using the 'quit' command or 'ctrl+c' "
          "instead of using the X window button or shutting down/logging off. "
          "There could be data loss as Windows may force terminate this "
          "application after five seconds of this message. Trying to save as "
          "much as possible to disk...");
        // Send logout event
        cEvtMain->RequestQuit();
        // Wait for main thread to clean up so we can exit
        cvExit.wait(ulExit);
      }
    } // Send normal exit event
    else cEvtMain->RequestQuit();
    // Block default event
    return TRUE;
  }
  /* -- Check keys --------------------------------------------------------- */
  KeyType GetKey(int &iKey, int &iMods)
  { // Return's key data
    INPUT_RECORD iData;
    // Number of items read
    DWORD dwRead;
    // See if key is there before potentially blocking execution. If failed?
    if(!PeekConsoleInput(hIn, &iData, 1, &dwRead))
    { // Report warning in log and return nothing read
      cLog->LogWarningExSafe("SysCon failed to peek console input: $!",
        SysError());
      return KT_NONE;
    } // Return if nothing read
    if(!dwRead) return KT_NONE;
    // Read key that is waiting for us. If failed?
    if(!ReadConsoleInput(hIn, &iData, 1, &dwRead))
    { // Report warning in log and return nothing read
      cLog->LogWarningExSafe("SysCon failed to read console input: $!",
        SysError());
      return KT_NONE;
    } // Return if nothing read
    if(!dwRead) return KT_NONE;
    // Clear key commands
    iKey = static_cast<unsigned int>(GLFW_KEY_UNKNOWN);
    iMods = 0;
    // Is a keypress by default
    KeyType ktType = KT_KEY;
    // If we have a key event and a key is down?
    if(iData.EventType & KEY_EVENT)
    { // Key is down?
      if(iData.Event.KeyEvent.bKeyDown == TRUE)
      { // What virtual key was pressed?
        switch(iData.Event.KeyEvent.wVirtualKeyCode)
        { // Valid key
          case VK_PRIOR  : iKey = GLFW_KEY_PAGE_UP; break;
          case VK_NEXT   : iKey = GLFW_KEY_PAGE_DOWN; break;
          case VK_HOME   : iKey = GLFW_KEY_HOME; break;
          case VK_END    : iKey = GLFW_KEY_END; break;
          case VK_BACK   : iKey = GLFW_KEY_BACKSPACE; break;
          case VK_INSERT : iKey = GLFW_KEY_INSERT; break;
          case VK_DELETE : iKey = GLFW_KEY_DELETE; break;
          case VK_UP     : iKey = GLFW_KEY_UP; break;
          case VK_DOWN   : iKey = GLFW_KEY_DOWN; break;
          case VK_ESCAPE : iKey = GLFW_KEY_ESCAPE; break;
          case VK_LEFT   : iKey = GLFW_KEY_LEFT; break;
          case VK_RIGHT  : iKey = GLFW_KEY_RIGHT; break;
          case VK_RETURN : iKey = GLFW_KEY_ENTER; break;
          case VK_TAB    : iKey = GLFW_KEY_TAB; break;
          // Key not supported?
          default: if(iData.Event.KeyEvent.uChar.UnicodeChar)
          { // Set the character code and return character type
            iKey = iData.Event.KeyEvent.uChar.UnicodeChar;
            ktType = KT_CHAR;
          } // Done
          break;
        }
      } // Key was pasted by Windows using right click or paste menu?
      else if(iData.Event.KeyEvent.wVirtualKeyCode == VK_MENU)
      { // Set key and flag if valid
        if(iData.Event.KeyEvent.uChar.UnicodeChar)
        {// Set the character code and return character type
          iKey = iData.Event.KeyEvent.uChar.UnicodeChar;
          ktType = KT_CHAR;
        }
      } // Reference to control keys state
      const DWORD &dwCFlag = iData.Event.KeyEvent.dwControlKeyState;
      // Add control modifier if pressed
      if(dwCFlag & LEFT_CTRL_PRESSED || dwCFlag & RIGHT_CTRL_PRESSED)
        iMods |= GLFW_MOD_CONTROL;
      // Add alt modifier if pressed
      if(dwCFlag & LEFT_ALT_PRESSED || dwCFlag & RIGHT_ALT_PRESSED)
        iMods |= GLFW_MOD_ALT;
      // Add shift modifier if pressed
      if(dwCFlag & SHIFT_PRESSED) iMods |= GLFW_MOD_SHIFT;
    } // Mouse event and it's a mouse wheel operation?
    if(iData.EventType & MOUSE_EVENT &&
      iData.Event.MouseEvent.dwEventFlags & MOUSE_WHEELED)
    { // Get scroll amount
      const short wAmount = UtilHighWord(iData.Event.MouseEvent.dwButtonState);
      // If wheel went backgrounds. Scroll downwards else upwards
      if(wAmount < 0) iKey = GLFW_KEY_PAGE_DOWN;
      else if(wAmount > 0) iKey = GLFW_KEY_PAGE_UP;
    } // The window was resized? Make sure we resize
    if(iData.EventType & WINDOW_BUFFER_SIZE_EVENT)
      UpdateSize(iData.Event.WindowBufferSizeEvent.dwSize.X,
                 iData.Event.WindowBufferSizeEvent.dwSize.Y);
    // We read an event
    return ktType;
  }
  /* -- Drawing attributes ------------------------------------------------- */
  size_t GetPos(void) const { return (stY * stW) + stX; }
  size_t GetPosEOL(void) const { return (stY * stW) + stW; }
  size_t EndPos(void) const { return stEndPos; }
  size_t SafeEnd(const size_t _stP) const
    { return UtilMinimum(_stP, EndPos()); }
  void SetColour(const WORD wNewCol) { wColour = wNewCol; }
  /* -- Set cursor size ---------------------------------------------------- */
  void SetCursorMode(const bool bInsert)
  { // Return if we already have this cursor siz
    const DWORD dwSize = bInsert ? 10 : 100;
    if(dwSize == dwCurSize) return;
    // Set new size and update size if failed?
    const CONSOLE_CURSOR_INFO
      cciInfo{ dwSize, FlagIsSetTwo(SCO_CURVISIBLE, TRUE, FALSE) };
    if(!SetConsoleCursorInfo(hOut, &cciInfo))
    { // Show error in log for failure
      cLog->LogErrorExSafe("SysCon failed to set cursor size from $ to $: $!",
        dwCurSize, dwSize, SysError());
    } // Succeeded so update cached value
    else dwCurSize = dwSize;
  }
  /* -- Set cursor visibility ---------------------------------------------- */
  void SetCursorVisibility(const bool bVisible)
  { // Return if cursor visibility already set
    if(bVisible == FlagIsSet(SCO_CURVISIBLE)) return;
    // Set new size and update size if succeeded?
    const CONSOLE_CURSOR_INFO
      cciInfo{ dwCurSize, bVisible ? TRUE : FALSE };
    if(SetConsoleCursorInfo(hOut, &cciInfo))
      return FlagSetOrClear(SCO_CURVISIBLE, bVisible);
    // Show error in log for failure
    cLog->LogErrorExSafe(
      "SysCon failed to set cursor visibility from $ to $: $!",
      StrFromBoolTF(FlagIsSet(SCO_CURVISIBLE)), StrFromBoolTF(bVisible),
      SysError());
  }
  /* -- Set cursor position ------------------------------------------------ */
  void SetCursor(const COORD &cData)
  { // Return if cursor position the same
    if(cData.X == sCurX && cData.Y == sCurY) return;
    // Set new cursor co-ordinates and if failed?
    if(!SetConsoleCursorPosition(hOut, cData))
    { // Show error in log for failure
      cLog->LogErrorExSafe(
        "SysCon failed to set cursor position from $x$ to $x$: $!",
        sCurX, sCurY, cData.X, cData.Y, SysError());
    } // Update cache'd co-ordinates
    else sCurX = cData.X, sCurY = cData.Y;
  }
  /* -- Commit buffer ------------------------------------------------------ */
  void CommitBuffer(void)
  { // Done if no bounds changed
    if(stX1 == StdMaxSizeT || stY1 == StdMaxSizeT) return;
    // Set drawing region
    SMALL_RECT srBounds = {static_cast<SHORT>(stX1),static_cast<SHORT>(stY1),
                           static_cast<SHORT>(stX2),static_cast<SHORT>(stY2)};
    // Do the clear
    if(!WriteConsoleOutputW(hOut, civBuf.data(),
      { static_cast<SHORT>(stW), static_cast<SHORT>(stH) },
      { static_cast<SHORT>(stX1), static_cast<SHORT>(stY1) }, &srBounds))
    { // Show error in log and leave drawing bounds as is
      cLog->LogWarningExSafe("SysCon failed to write console output: $!",
        SysError());
    } // Reset drawing bounds
    else ResetDrawingBounds();
  }
  /* -- Set a character at the specified screen buffer position ------------ */
  void SetCharPos(const size_t stPos, const unsigned int uiC=' ')
  { // Get pointer to character ata
    CHAR_INFO &ciCell = civBuf[stPos];
    // Convert parameters to native format
    WCHAR wcChar = static_cast<WCHAR>(uiC);
    // Character is the same?
    if(wcChar == ciCell.Char.UnicodeChar)
    { // Return if colour is the same
      if(wColour == ciCell.Attributes) return;
      // Set new colour code
      ciCell.Attributes = wColour;
    } // Character is not the same
    else
    { // Set new character code
      ciCell.Char.UnicodeChar = wcChar;
      // Set new colour code
      if(wColour != ciCell.Attributes) ciCell.Attributes = wColour;
    } // Update drawing bounds if neccesary
    const size_t stCX = stPos % stW;
    if(stCX < stX1) stX1 = stCX;
    if(stCX > stX2) stX2 = stCX;
    const size_t stCY = stPos / stW;
    if(stCY < stY1) stY1 = stCY;
    if(stCY > stY2) stY2 = stCY;
  }
  /* -- Set a character at the current position ---------------------------- */
  void SetChar(const unsigned int uiC=' ') { SetCharPos(GetPos(), uiC); }
  /* -- Clear to end of line ----------------------------------------------- */
  void ClearLine(void)
  { // Get number of chars to write
    const size_t stEnd = SafeEnd(GetPosEOL());
    // For each character index in the buffer
    for(size_t stPos = GetPos(); stPos < stEnd; ++stPos) SetCharPos(stPos);
  }
  /* -- Save and restore colour -------------------------------------------- */
  void PushColour(void) { wColourSaved = wColour; }
  void PopColour(void) { wColour = wColourSaved; }
  /* -- Distance ----------------------------------------------------------- */
  unsigned int Distance(const unsigned int uiD1, const unsigned int uiD2)
  { // Calculate deltas of components
    const unsigned int
      uiDeltaR = ((uiD1&0x00FF0000) >> 16) - ((uiD2&0x00FF0000) >> 16),
      uiDeltaG = ((uiD1&0x0000FF00) >>  8) - ((uiD2&0x0000FF00) >>  8),
      uiDeltaB =  (uiD1&0x000000FF)        -  (uiD2&0x000000FF);
    // Return distance
    return uiDeltaR * uiDeltaR + uiDeltaG * uiDeltaG + uiDeltaB * uiDeltaB;
  }
  /* -- Set colour as integer helper function ------------------------------ */
  void SetColourInteger(const unsigned int uiColour, const WORD wClearFlags,
    const ColourList &wLookup)
  { // Minimum distance between colour and closest colour
    unsigned int uiMinDist = Distance(uiColour, uiNDXtoRGB[0]);
    // Prepare first colour
    size_t stChosen = 0;
    // Walk through available colours
    for(size_t stIndex = 1; stIndex < COLOUR_MAX; ++stIndex)
    { // Get current distance between specified colour and colour in pallette
      unsigned int uiCurDist = Distance(uiColour, uiNDXtoRGB[stIndex]);
      // Ignore the entry if is not near the minimum
      if(uiCurDist >= uiMinDist) continue;
      // Update new selected colour and chosen index
      uiMinDist = uiCurDist;
      stChosen = stIndex;
    } // Set chosen colour
    wColour = (wColour & ~wClearFlags) | wLookup[stChosen];
  }
  /* -- Set foreground colour as int --------------------------------------- */
  void SetForegroundInteger(const unsigned int uiColour)
    { SetColourInteger(uiColour, FOREGROUND_RED|FOREGROUND_GREEN|
        FOREGROUND_BLUE|FOREGROUND_INTENSITY, wNDXtoW32C); }
  /* -- Set background colour as int --------------------------------------- */
  void SetBackgroundInteger(const unsigned int uiColour)
    { SetColourInteger(uiColour, BACKGROUND_RED|BACKGROUND_GREEN|
        BACKGROUND_BLUE|BACKGROUND_INTENSITY, wNDXtoW32BC); }
  /* -- Handle print control character ------------------------------------- */
  void HandlePrintControl(UtfDecoder &utfStr, const bool bSimulation)
  { // Get next character
    switch(utfStr.Next())
    { // Colour selection
      case 'c':
      { // Scan for the hexadecimal value and if we found it? Set Tint if not
        // simulation and we read 8 bytes
        unsigned int uiCol;
        if(utfStr.ScanValue(uiCol) == 8 && !bSimulation)
          SetForegroundInteger(uiCol);
        // Done
        break;
      } // Outline colour selection
      case 'o':
      { // Scan for the hexadecimal value and if we found it? Set Tint if not
        // simulation and we read 8 bytes
        unsigned int uiCol;
        if(utfStr.ScanValue(uiCol) == 8 && !bSimulation)
          SetBackgroundInteger(uiCol);
        // Done
        break;
      } // Reset colour
      case 'r': if(!bSimulation) PopColour(); break;
      // Print glyph? Ignore texture, skip the value and break
      case 't': utfStr.SkipValue(); break;
      // Invalid control character.
      default: break;
    }
  }
  /* -- Locate a supported character while checking if word can be printed - */
  bool PrintGetWord(UtfDecoder &utfStr, size_t stXp, const size_t stWi)
  { // Save position because we're not drawing anything
    const unsigned char *ucpPtr = utfStr.GetCPtr();
    // Until null character. Which control token?
    while(const unsigned int uiChar = utfStr.Next()) switch(uiChar)
    { // Carriage return or space char? Restore position and return no wrap
      case '\n': case ' ': utfStr.SetCPtr(ucpPtr); return false;
      // Other control character? Handle print control characters
      case '\r': HandlePrintControl(utfStr, true); break;
      // Normal character
      default:
        // Printing next character would exceed wrap width?
        if(1 + stXp >= stWi)
        { // Restore position
          utfStr.SetCPtr(ucpPtr);
          // Wrap to next position
          return true;
        } // Move X along
        ++stXp;
        // Done
        break;
    } // Restore position
    utfStr.SetCPtr(ucpPtr);
    // No wrap occured and caller should not Y adjust
    return false;
  }
  /* -- Handle return on print --------------------------------------------- */
  void HandleReturnSimulated(UtfDecoder &utfStr, size_t &stXp, size_t &stYp,
    const size_t stIn)
  { // Go down own line and set indentation
    stXp = stIn;
    ++stYp;
    // Discard further spaces and return string minus one space
    utfStr.Ignore(' ');
  }
  /* -- If co-ordinates are in valid range to draw ------------------------- */
  bool ValidY(const size_t stYp) { return stYp >= 1 && stYp <= stHm1; }
  /* -- Handle return on print --------------------------------------------- */
  void HandleReturn(UtfDecoder &utfStr, const size_t stIn)
  { // If we can draw on this Y? Clear up till the end
    if(ValidY(stY)) ClearLine();
    // If we can draw on the next line too? Clear up to indentation
    if(ValidY(++stY)) for(stX = 0; stX < stIn; ++stX) SetChar();
    // We cann't draw on the next line? Just set new indentation position
    else stX = stIn;
    // Discard further spaces and return string minus one space
    utfStr.Ignore(' ');
  }
  /* -- Write data upwards and wrapping (same as what Char::* does) -------- */
  size_t WriteLineWU(UtfDecoder &&utfStr)
  { // Check the string is valid
    if(!utfStr.Valid()) return 1;
    // Save current colour
    PushColour();
    // Indent
    const size_t stIndent = 1;
    // Simulated cursor position
    size_t stXp = 0, stYp = 1;
    // Until null character, which character?
    while(const unsigned int uiChar = utfStr.Next()) switch(uiChar)
    { // Carriage return?
      case '\n': HandleReturnSimulated(utfStr, stXp, stYp, stIndent); break;
      // Other control character? Handle print control characters
      case '\r': HandlePrintControl(utfStr, true); break;
      // Whitespace character?
      case ' ':
      { // Move X position forward
        ++stXp;
        // Ignore if the space character processed went over the limit OR
        // Check if the draw length of the next word would go off the limit
        // and if either did? Handle the return!
        if(1 + stXp > stW || PrintGetWord(utfStr, stXp, stW))
          HandleReturnSimulated(utfStr, stXp, stYp, stIndent);
        // Done
        break;
      } // Normal character
      default:
      { // Printing next character would exceed wrap width? Wrap and indent
        if(1 + stXp > stW) HandleReturnSimulated(utfStr, stXp, stYp, stIndent);
        // No exceed, move X position forward
        stXp += 1;
        // Done
        break;
      }
    } // Restore previous colour incase it was changed
    PopColour();
    // Reset the iterator on the utf string.
    utfStr.Reset();
    // Set actual cursor position
    stX = 0, stY -= stYp;
    // Record original X and Y position
    const size_t stXO = stX + stIndent;
    // Until null character, which character?
    while(const unsigned int uiChar = utfStr.Next()) switch(uiChar)
    { // Carriage return?
      case '\n': HandleReturn(utfStr, stXO); break;
      // Other control character? Handle print control characters
      case '\r': HandlePrintControl(utfStr, false); break;
      // Whitespace character?
      case ' ':
      { // Do the print if we can
        if(ValidY(stY)) SetChar(uiChar);
        // Move the next X position
        stX++;
        // Ignore if the space character processed went over the limit OR
        // Check if the draw length of the next word would go off the limit
        // and if either did? Handle the wrapping
        if(1 + stX > stW || PrintGetWord(utfStr, stX, stW))
          HandleReturn(utfStr, stXO);
        // Done
        break;
      } // Normal character
      default:
      { // Printing next character would exceed wrap width?
        if(stX + 1 > stW)
        { // Handle the wrapping
          HandleReturn(utfStr, stXO);
          // If we can print the character? Print the character
          if(ValidY(stY)) SetChar(uiChar);
        } // Wouldn't wrap so if we can print the character? Print the char
        else if(ValidY(stY)) SetChar(uiChar);
        // Move along the X position
        stX++;
        // Done
        break;
      }
    } // Clear the rest of the line
    if(ValidY(stY)) ClearLine();
    // Restore colour
    PopColour();
    // Return height of printed text
    return stYp;
  }
  /* -- Write data --------------------------------------------------------- */
  void WriteLine(UtfDecoder &&utfStr, const size_t stMax, const bool bClrEOL)
  { // Get current absolute character position in screen buffer
    size_t stPos = GetPos();
    // Get end of line position
    const size_t stPosEOL = SafeEnd(GetPosEOL());
    // Get absolute end position in screen buffer to protect from overrun
    const size_t stEnd = UtilMinimum(stPosEOL, SafeEnd(stPos + stMax));
    // For each character index in the buffer
    while(const unsigned int uiChar = utfStr.Next())
    { // Compare character
      switch(uiChar)
      { // Carriage return? (impossible).
        case '\n': break;
        // Is a control character?
        case '\r': HandlePrintControl(utfStr, false); break;
        // Get pointer to character ata
        default : SetCharPos(stPos++, uiChar); break;
      } // Increase position in screen buffer and break if at end of string/buf
      if(stPos >= stEnd) break;
    } // Now we need to clear old characters up to the end of line
    if(bClrEOL) while(stPos < stPosEOL) SetCharPos(stPos++);
  }
  /* -- Redraw status input text ------------------------------------------- */
  void RedrawInputBar(const string &strIL, const string &strIR)
  { // Set cursor position
    stX = 0, stY = stHm1;
    // Set input bar colour
    SetColour(FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE|
      BACKGROUND_INTENSITY|FOREGROUND_INTENSITY|BACKGROUND_RED);
    // Clear the line with the above colour
    ClearLine();
    // Length of left part of text
    stX = 1;
    // Left string length
    size_t stLen;
    // Have left side text?
    if(!strIL.empty())
    { // Put left text in a UTF container
      UtfDecoder utfString{ strIL };
      // Set length
      stLen = utfString.Length();
      // Reset at scrolled position
      utfString.Reset(strIL.c_str() +
        abs(UtilMaximum(0, (int)stLen-(int)stWm2)));
      // Draw start of input text
      WriteLine(StdMove(utfString), stWm2, false);
    } // Left size of text is zero long
    else stLen = 0;
    // Have right side of text and have characters left on screen to spare?
    if(!strIR.empty() && stLen < stWm2)
    { // Set cursor position
      stX = 1 + stLen, stY = stHm1;
      // Reset again and write the string
      WriteLine(UtfDecoder{ strIR }, stWm2 - stLen, false);
    } // Set cursor position
    SetCursor({ static_cast<SHORT>(UtilMinimum(stWm1, 1 + stLen)),
                static_cast<SHORT>(stHm1) });
    // Set cursor visiiblity
    SetCursorVisibility(true);
  }
  /* -- Set a space at the specified X position ---------------------------- */
  void SetWhitespace(const size_t stPos=0)
  { // Set start position
    stX = stPos;
    // Set the whitespace
    SetChar();
    // Move on
    ++stX;
  }
  /* -- Redraw a status bar ------------------------------------------------ */
  void RedrawStatus(const size_t stSY, const string &strL, const string &strR)
  { // Reset drawing row
    stY = stSY;
    // Set colour
    SetColour(FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE|
      FOREGROUND_INTENSITY|BACKGROUND_BLUE);
    // Get length of left and right part of string
    UtfDecoder utfL{ strL }, utfR{ strR };
    const size_t stL = utfL.Length(), stLC = UtilMinimum(stL, stWm2),
                 stR = utfR.Length(), stRC = UtilMinimum(stR, stWm2);
    // If we have left status text length?
    if(stLC)
    { // If the right text would not completely obscure the left text?
      if(stRC < stWm2)
      { // Set start position, clear the character and move forward
        SetWhitespace();
        // Reset left text position
        utfL.Reset();
        // Put string in a container and draw the string with left align
        WriteLine(StdMove(utfL), stLC, false);
        // Update X position
        stX += stLC;
        // If there is no right text we can just clear to the end of line
        if(!stRC) return ClearLine();
      } // No length of left text? Set initial position.
      else stX = 1;
    } // If there is no right next
    else if(!stRC)
    { // Select start of line
      stX = 0;
      // Clear to end of line
      return ClearLine();
    } // No length of left text and have right text? Set starting whitespace.
    else SetWhitespace();
    // Both texts can only share 'stWm2' characters so they need to share
    // this limited space. If there is enough space for both?
    const size_t stTotal = UtilMinimum(stWm2, stLC + stRC);
    if(stTotal < stWm2)
    { // Get position to write at
      size_t stPos = GetPos();
      const size_t stSpaces = stWm2 - stTotal;
      const size_t stEnd = SafeEnd(stPos + stSpaces);
      // Repeat writing spaces until we've reached the end position
      do SetCharPos(stPos); while(++stPos < stEnd);
      // Move X along the number of spaces we wrote
      stX += stSpaces;
    } // We didn't need to draw any spaces? Set final whitespace.
    else SetWhitespace(stWm2 - stRC);
    // Reset right string position
    utfR.Reset();
    // Write the line and clear the rest
    WriteLine(StdMove(utfR), stRC, true);
  }
  /* -- Redraw bottom status bar ------------------------------------------- */
  void RedrawStatusBar(const string &strSL, const string &strSR)
    { RedrawStatus(stHm1, strSL, strSR); SetCursorVisibility(false); }
  /* -- Redraw title status bar -------------------------------------------- */
  void RedrawTitleBar(const string &strTL, const string &strTR)
    { RedrawStatus(0, strTL, strTR); }
  /* -- Redraw console buffer ---------------------------------------------- */
  void RedrawBuffer(const ConLines &clLines,
    const ConLinesConstRevIt &clcriStart)
  { // Reset cursor position to top left of drawing area
    stX = 0;
    stY = stHm1;
    // Set default text colour
    SetColour(FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE);
    // Draw until we go off the top of the screen
    for(ConLinesConstRevIt clcriIt{ clcriStart };
                           clcriIt != clLines.crend() && ValidY(stY);
                         ++clcriIt)
    { // Get structure
      const ConLine &clLine = *clcriIt;
      // Convert RGB colour to Win32 console id colour
      SetColour(wNDXtoW32C[clLine.cColour]);
      // Put text in a utf container and write the data. Note: Although
      // stY could go effectively negative and we're not using a signed value
      // we can make sure we don't access any OOB memory by just checking that
      // the co-ordinates while drawing, we don't have to worry about the
      // integer wrapping at all we are not drawing.
      stY -= WriteLineWU(UtfDecoder{ clLine.strLine }) - 1;
    } // Done if there is no lines to clear up to the top
    if(!ValidY(stY)) return;
    // Goto beginning so we can clear lines
    stX = 0;
    // Clear extra lines we didn't draw too
    while(stY-- > 1) ClearLine();
  }
  /* -- Update console window info ----------------------------------------- */
  void UpdateWindowInfo(const SHORT sRight, const SHORT sBottom)
  { // Set the console window information
    const SMALL_RECT rBounds{ 0, 0, sRight, sBottom };
    if(!SetConsoleWindowInfo(hOut, TRUE, &rBounds))
      cLog->LogErrorExSafe(
        "SysCon failed to set window info bounds to $x$x$x$: $!",
        rBounds.Left, rBounds.Top, rBounds.Right,
        rBounds.Bottom, SysError());
  }
  /* -- Update console screen buffer size ---------------------------------- */
  void UpdateScreenBufferSize(const SHORT sX, const SHORT sY)
  { // Set the console screen buffer size
    const COORD cSize{ static_cast<SHORT>(sX), static_cast<SHORT>(sY) };
    if(!SetConsoleScreenBufferSize(hOut, cSize))
      cLog->LogErrorExSafe(
        "SysCon failed to set screen buffer size to $x$: $!",
        cSize.X, cSize.Y, SysError());
  }
  /* -- Size updated event (unused on win32) ------------------------------- */
  void OnResize(void) { }
  /* -- Update size -------------------------------------------------------- */
  void UpdateSize(const size_t _stW, const size_t _stH)
  { // Set new bounds minus one and two.
    stWm1 = _stW-1;
    stWm2 = _stW-2;
    stHm1 = _stH-1;
    stHm2 = _stH-2;
    // Buffer size is smaller than the current?
    if(_stW < stW || _stH < stH)
    { // Update window size before buffer size
      UpdateWindowInfo(static_cast<SHORT>(stWm1),
                       static_cast<SHORT>(stHm1));
      UpdateScreenBufferSize(static_cast<SHORT>(_stW),
                             static_cast<SHORT>(_stH));
    } // Buffer size is equal or greater than the current?
    else
    { // Update buffer size before the window size
      UpdateScreenBufferSize(static_cast<SHORT>(_stW),
                             static_cast<SHORT>(_stH));
      UpdateWindowInfo(static_cast<SHORT>(stWm1),
                       static_cast<SHORT>(stHm1));
    } // Calculate total number of characters
    const size_t stWtH = _stW * _stH;
    // Resize character buffer used for writing to the buffer
    civBuf.resize(stWtH);
    // Free any excess memory when resized down
    civBuf.shrink_to_fit();
    // This is the char and attribute to fill with
    const CHAR_INFO ciItem{ { 32 },
      FOREGROUND_BLUE|FOREGROUND_GREEN|FOREGROUND_RED };
    // Fill the entire cached screen buffer with the default character. This
    // make sure every character is rewritten to when the entire screen buffer
    // has changed.
    fill(civBuf.begin(), civBuf.end(), ciItem);
    // Initialise drawing bounds
    ResetDrawingBounds();
    // Set maximum buffer character count
    stEndPos = stWtH + _stW;
    // Update new size
    stW = _stW;
    stH = _stH;
    // Set default position
    stX = stY = 0;
    // Force a console buffer update
    cEvtMain->Add(EMC_CON_UPDATE, stW, stH);
  }
  /* -- Initialise --------------------------------------------------------- */
  void LoadDefaults(void)
  { // Get console screen buffer window info
    CONSOLE_SCREEN_BUFFER_INFO csbInfo;
    if(!GetConsoleScreenBufferInfo(hOut, &csbInfo))
      XCS("Failed to get cursor position!");
    // Update system cursor position
    sCurX = csbInfo.dwCursorPosition.X;
    sCurY = csbInfo.dwCursorPosition.Y;
    wColour = csbInfo.wAttributes;
    // Get console cursor information so we don't have to send api commands
    // when the current cursor info is the same.
    CONSOLE_CURSOR_INFO cciData;
    if(!GetConsoleCursorInfo(hOut, &cciData))
      XCS("Failed to get cursor info!");
    dwCurSize = cciData.dwSize;
    FlagSetOrClear(SCO_CURVISIBLE, cciData.bVisible);
  }
  /* -- DeInitialise ------------------------------------------------------- */
  void SysConDeInit(void)
  { // Done if console not opened
    if(IsNotWindowHandleSet()) return;
    // System console is de-initialising
    cLog->LogDebugSafe("SysCon de-initialising...");
    // Unset control and break handler
    if(!SetConsoleCtrlHandler(CtrlHandlerStatic, FALSE))
      cLog->LogErrorExSafe(
        "SysCon failed to clear console control handler: $!", SysError());
    // Destroy console window
    if(!FreeConsole())
      cLog->LogErrorExSafe("SysCon failed to close console window: $!",
        SysError());
    // No longer opened
    SetWindowDestroyed();
    // Clear screen buffer
    civBuf.clear();
    // System console is de-initialised
    cLog->LogDebugSafe("SysCon de-initialised.");
  }
  /* -- Initialise --------------------------------------------------------- */
  void SysConInit(const char*const cpTitle, const size_t _stW,
    const size_t _stH, const bool bNoClose)
  { // Console class initialised
    if(IsWindowHandleSet()) XC("System console already allocated!");
    // System console is initialising
    cLog->LogDebugSafe("SysCon initialising...");
    // Create a console window and if failed and the error is it not because
    // 'access denied' or no wine running? Then throw error. Wine cannot
    // AllocConsole() but can still run on the existing terminal screen if we
    // ignore the error.
    if(!AllocConsole() &&
        (SysIsNotErrorCode(ERROR_ACCESS_DENIED) || strWine.empty()))
      XCS("Failed to allocate console window!");
    // Get console window handle and if failed?
    SetWindowHandle(GetConsoleWindow());
    if(IsNotWindowHandleSet())
    { // Close console because it is not handled without hwndConsole
      FreeConsole();
      // Throw the error
      XCS("Failed to retrieve console window handle!");
    } // Set console window title (doesn't matter if failed)
    if(!SetConsoleTitleW(UTFtoS16(cpTitle).c_str()))
      XCS("Failed to set window title!", "Title", cpTitle);
    // Get output handle
    hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if(hOut == INVALID_HANDLE_VALUE)
      XCS("Failed to get standard output handle!");
    // Get input handle
    hIn = GetStdHandle(STD_INPUT_HANDLE);
    if(hIn == INVALID_HANDLE_VALUE)
      XCS("Failed to get standard input handle!");
    // Load settings from current console window
    LoadDefaults();
    // Update window size
    UpdateSize(_stW, _stH);
    // Set control and break handler
    if(!SetConsoleCtrlHandler(CtrlHandlerStatic, TRUE))
      XCS("Failed to set console control handler!");
    // If we're disabling the close button? Get handle to menu and gray the
    // close button and delete the close menu option. Throw if failed.
    if(bNoClose)
      if(const HMENU hMenu = GetSystemMenu(GetWindowHandle(), FALSE))
        if(!DeleteMenu(hMenu, SC_CLOSE, MF_BYCOMMAND))
          XCS("Failed to delete close menu option!");
    // System console is initialised
    cLog->LogDebugSafe("SysCon initialised.");
  }
  /* -- Destructor ---------------------------------------------- */ protected:
  ~SysCon(void) noexcept(false) { SysConDeInit(); }
  /* -- Constructor -------------------------------------------------------- */
  explicit SysCon(const string &strW): // Wine version if applicable
    /* -- Initialisers ----------------------------------------------------- */
    strWine{ strW },                   // Set reference to wine version
    hIn(nullptr), hOut(nullptr),       // Handles to input and output streams
    wColour(0),                        // Default colour
    wColourSaved(0),                   // Saved colour
    dwCurSize(0),                      // Current cursor size
    sCurX(0), sCurY(0),                // Cursor to top-left of screen
    stEndPos(0),                       // Ending position
    stX(0), stY(0),                    // Drawing position to top-left
    stW(0), stH(0),                    // Width and height of window not set
    stWm1(0), stWm2(0),                // Width and height of window-1 not set
    stHm1(0), stHm2(0),                // Width and height of window-2 not set
    stX1(0), stY1(0),                  // Minimum drawing extent not set
    stX2(0), stY2(0)                   // Maximum drawing extent not set
    /* -- Set console output codepage to UTF-8 ----------------------------- */
    { SetConsoleOutputCP(CP_UTF8); }
  /* -- Set maximum console line length ---------------------------- */ public:
  CVarReturn RowsModified(const size_t stRows)
  { // Deny if out of range. The maximum value is a SHORT from Win32 API.
    if(stRows < 25 || UtilIntWillOverflow<SHORT>(stRows)) return DENY;
    // Update the buffer size if console is opened
    if(IsWindowHandleSet()) UpdateSize(stW, stRows);
    // Value allowed
    return ACCEPT;
  }
  /* -- Set maximum console line length ------------------------------------ */
  CVarReturn ColsModified(const size_t stCols)
  { // Deny if out of range. The maximum value is a SHORT from Win32 API.
    if(stCols < 80 || UtilIntWillOverflow<SHORT>(stCols)) return DENY;
    // Update the buffer size
    if(IsWindowHandleSet()) UpdateSize(stCols, stH);
    // Value allowed
    return ACCEPT;
  }
};/* ----------------------------------------------------------------------- */
#define ENGINE_SYSCON_CALLBACKS() \
  BOOL WINAPI SysCon::CtrlHandlerStatic(DWORD dwCtrlType) \
    { return cSystem->CtrlHandler(dwCtrlType); }
/* == EoF =========================================================== EoF == */
