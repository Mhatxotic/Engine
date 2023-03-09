/* == CLIP.HPP ============================================================= **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## This module handles interfacing Lua and the engine with GLFW's      ## **
** ## clipboard functions. Since GLFW requires that the clipboard funcs   ## **
** ## run only in the window thread, we need to send an async event to    ## **
** ## the window thread, and then back to the engine thread an Lua.       ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IClipboard {                 // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace ICollector::P;         using namespace IError::P;
using namespace IEvtMain::P;           using namespace IEvtWin::P;
using namespace IGlFW::P;              using namespace IIdent::P;
using namespace ILuaEvt::P;            using namespace ILuaLib::P;
using namespace ILuaUtil::P;           using namespace IStd::P;
using namespace ISysUtil::P;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* -- Clipboard collector and lua interface class -------------------------- */
CTOR_BEGIN(Clips, Clip, CLHelperUnsafe,
  /* ----------------------------------------------------------------------- */
  const EvtWin::RegVec rvEvents;,,     // Events list to register
  private LuaEvtMaster<Clip, LuaEvtTypeAsync<Clip>>
);/* ----------------------------------------------------------------------- */
CTOR_MEM_BEGIN_CSLAVE(Clips, Clip, ICHelperUnsafe),
  /* -- Base classes ------------------------------------------------------- */
  public LuaEvtSlave<Clip, 2>,         // Need to store callback and class
  public Lockable,                     // Lua garbage collector instruction
  public Ident                         // Name of this clipboard event
{ /* -- Private variables -------------------------------------------------- */
  string           strClipboard;       // Clipboard string
  /* -- Window set clipboard request --------------------------------------- */
  void ClipOnSetNRCbT(void) { cGlFW->WinSetClipboardString(strClipboard); }
  /* -- Window set clipboard request --------------------------------------- */
  void ClipOnSetCbT(void) { ClipOnSetNRCbT(); LuaEvtDispatch(); }
  /* -- Window get clipboard request in window thread ---------------------- */
  void ClipOnGetCbT(void)
  { // Grab the string to clipboard
    strClipboard = cGlFW->WinGetClipboardString();
    // Pass the string back to the engine thread and Lua
    LuaEvtDispatch();
  }
  /* -- Get string sent or retrieved ------------------------------- */ public:
  const string &ClipGet(void) const { return strClipboard; }
  /* -- Get string sent or retrieved --------------------------------------- */
  void ClipSet(const string &strText)
  { // Set clipboard string to set
    strClipboard = strText;
    // Dispatch the event
    cEvtWin->Add(EWC_CB_SETNR, this);
  }
  /* -- Window set clipboard request --------------------------------------- */
  static void ClipOnSetNRCb(const EvtWinEvent &eweEvent)
    { reinterpret_cast<Clip*>(eweEvent.aArgs.front().vp)->ClipOnSetNRCbT(); }
  /* -- Window get clipboard request in window thread ---------------------- */
  static void ClipOnGetCb(const EvtWinEvent &eweEvent)
    { reinterpret_cast<Clip*>(eweEvent.aArgs.front().vp)->ClipOnGetCbT(); }
  /* -- Window set clipboard request --------------------------------------- */
  static void ClipOnSetCb(const EvtWinEvent &eweEvent)
    { reinterpret_cast<Clip*>(eweEvent.aArgs.front().vp)->ClipOnSetCbT(); }
  /* -- Async thread event callback (called by LuaEvtMaster) --------------- */
  void LuaEvtCallbackAsync(const EvtMainEvent &emeEvent) try
  { // Get reference to string vector and we need three parameters
    // [0]=Pointer to clipboard class
    const EvtMainArgs &emaArgs = emeEvent.aArgs;
    // Must have 2 parameters
    if(!LuaEvtsCheckParams<2>(emaArgs))
      XC("Clipboard did not receive two parameters!",
         "Identifier", IdentGet(), "Count", emaArgs.size());
    // If lua is not paused?
    if(!uiLuaPaused)
    { // Get and push function
      if(!LuaRefGetFunc(1))
        XC("Clipboard first argument not a function!",
           "Identifier", IdentGet());
      // Get and push class
      if(!LuaRefGetUData())
        XC("Clipboard second argument not a class!",
           "Identifier", IdentGet());
      // Call callback with class
      LuaUtilCallFuncEx(lsState, 1);
    } // Done with references. We won't be using them anymore.
    return LuaEvtDeInit();
  } // Exception occured? Cleanup and rethrow exception
  catch(const exception&) { LuaEvtDeInit(); throw; }
  /* -- Initialise and set string ------------------------------------------ */
  void ClipSetAsync(lua_State*const lS, const string &strIdent,
    const string &strContent)
  { // Get and check parameters
    IdentSet(StdMove(strIdent));
    strClipboard = StdMove(strContent);
    // Init LUA references
    LuaEvtInitEx(lS);
    // We're ready, so dispatch to the window thread with this class
    cEvtWin->Add(EWC_CB_SET, this);
  }
  /* -- Initialise and get string ------------------------------------------ */
  void ClipGetAsync(lua_State*const lS, const string &strIdent)
  { // Get and check parameters
    IdentSet(StdMove(strIdent));
    // Init LUA references
    LuaEvtInitEx(lS);
    // We're ready, so dispatch to the window thread with this class
    cEvtWin->Add(EWC_CB_GET, this);
  }
  /* -- Default constructor ------------------------------------------------ */
  Clip(void) :
    /* -- Initialisers ----------------------------------------------------- */
    ICHelperClip{ cClips },            // Initially unregistered
    IdentCSlave{ cParent->CtrNext() }, // Initialise identification number
    LuaEvtSlave{ this, EMC_CB_EVENT }  // Register Clip async event
    /* -- No code ---------------------------------------------------------- */
    { }
};/* ----------------------------------------------------------------------- */
CTOR_END(Clips, Clip, CLIP,            // Finish 'Clips' class body
  /* -- Collector initialisers --------------------------------------------- */
  cEvtWin->RegisterEx(rvEvents),       // Register all events in 'rvEvents'
  cEvtWin->UnregisterEx(rvEvents),,    // Unregister all events in 'rvEvents'
  LuaEvtMaster{ EMC_CB_EVENT },        // Setup Lua event master
  rvEvents{                            // Define handled Window thread events
    { EWC_CB_SET,   &Clip::ClipOnSetCb   },
    { EWC_CB_GET,   &Clip::ClipOnGetCb   },
    { EWC_CB_SETNR, &Clip::ClipOnSetNRCb },
  }
);/* ----------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
