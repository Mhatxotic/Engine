/* == EVTCORE.HPP ========================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## This class allows threads to communicate with each other safely.    ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IEvtCore {                   // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IError::P;             using namespace ILog::P;
using namespace IStd::P;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public public namespace
/* ------------------------------------------------------------------------- */
enum EvtArgVarType: unsigned int       // EvtArgVar variable types
{ /* ----------------------------------------------------------------------- */
  EAVT_BOOL,     EAVT_CSTR,    EAVT_STR, EAVT_PTR,       EAVT_FLOAT,
  EAVT_DOUBLE,   EAVT_UINT,    EAVT_INT, EAVT_ULONGLONG, EAVT_LONGLONG,
  EAVT_LONGUINT, EAVT_LONGINT, EAVT_MAX
};/* ----------------------------------------------------------------------- */
struct EvtArgVar                       // Multi-type helps access event data
{ /* ----------------------------------------------------------------------- */
  const EvtArgVarType  t;              // Variable type
  /* ----------------------------------------------------------------------- */
  union                                // Variables share same memory space
  { /* -- All these use the same memory ------------------------------------ */
    size_t             z;              // Size Integer ............ (4-8 bytes)
    bool               b;              // Boolean .................... (1 byte)
    void              *vp;             // Pointer ................. (4-8 bytes)
    char              *cp;             // C-String pointer ........ (4-8 bytes)
    string            *str;            // STL-String pointer ...... (4-8 bytes)
    float              f;              // Float ..................... (4 bytes)
    double             d;              // Double .................... (8 bytes)
    unsigned int       ui;             // Unsigned Integer .......... (4 bytes)
    signed int         i;              // Signed Integer ............ (4 bytes)
    unsigned long long ull;            // Unsigned Long Long ........ (8 bytes)
    signed long long   ll;             // Signed Long Long .......... (8 bytes)
    long unsigned int  lui;            // Long Unsigned Integer ... (4-8 bytes)
    long signed int    li;             // Long Signed Integer ..... (4-8 bytes)
  }; /* -------------------------------------------------------------------- */
  explicit EvtArgVar(const void*const vpP) :
    t(EAVT_PTR), vp(const_cast<void*>(vpP)) {}
  explicit EvtArgVar(const char*const cpP) :
    t(EAVT_CSTR), cp(const_cast<char*>(cpP)) {}
  explicit EvtArgVar(const string &strV) :
    t(EAVT_STR), str(const_cast<string*>(&strV)) {}
  explicit EvtArgVar(const unsigned int uiV) :
    t(EAVT_UINT), ui(static_cast<unsigned int>(uiV)) {}
  explicit EvtArgVar(const signed int siV) :
    t(EAVT_INT), i(static_cast<signed int>(siV)){}
  explicit EvtArgVar(const double dV) :
    t(EAVT_DOUBLE), d(dV) {}
  explicit EvtArgVar(const float fV):
    t(EAVT_FLOAT), f(fV) {}
  explicit EvtArgVar(const unsigned long long ullV) :
    t(EAVT_ULONGLONG), ull(ullV) {}
  explicit EvtArgVar(const signed long long sllV) :
    t(EAVT_LONGLONG), ll(sllV) {}
  explicit EvtArgVar(const long unsigned int luiV) :
    t(EAVT_LONGUINT), lui(luiV) {}
  explicit EvtArgVar(const long signed int liV) :
    t(EAVT_LONGINT), li(liV) {}
  explicit EvtArgVar(const bool bV):
    t(EAVT_BOOL), b(bV) {}
};/* ----------------------------------------------------------------------- */
/* -- Common events system (since we need to use this twice) --------------- */
template<typename Cmd,                 // Variable type of command to use
         size_t   EvtMaxEvents,        // Maximum number of events
         Cmd      EvtNone,             // Id of 'none' event
         Cmd      EvtNoLog>            // Id of succeeding ids to not log for
class EvtCore                          // Start of common event system class
{ /* -- Typedefs --------------------------------------------------- */ public:
  struct Event;                         // (Prototype) Event packet info
  typedef void (CBFuncT)(const Event&); // Event callback type
  typedef function<CBFuncT> CBFunc;     // Actual event callback
  /* ----------------------------------------------------------------------- */
  typedef array<CBFunc, EvtMaxEvents>    Funcs;        // Reg'd events vector
  typedef list<Event>                    Queue;        // Current events queue
  typedef typename Queue::const_iterator QueueConstIt; // Current events queue
  /* ----------------------------------------------------------------------- */
  typedef pair<const Cmd, const CBFunc> RegPair; // Event command and callback
  typedef const vector<RegPair>         RegVec;  // Event list
  /* ----------------------------------------------------------------------- */
  typedef vector<EvtArgVar> Args;      // Vector of RegPairs
  /* ----------------------------------------------------------------------- */
  struct Event                         // Event packet information
  { /* --------------------------------------------------------------------- */
    Cmd            cCmd;               // Command send
    CBFunc         cbfFunc;            // Function to call
    Args           aArgs;              // User parameters
    /* -- Constructor with move parameters --------------------------------- */
    Event(const Cmd cNCmd, const CBFunc &cbfNFunc, Args &&aNArgs) :
      /* -- Initialisers --------------------------------------------------- */
      cCmd(cNCmd),                     // Set requested command
      cbfFunc{ cbfNFunc },             // Set callback function
      aArgs{ StdMove(aNArgs) }         // Move requested parameters
      /* -- No code -------------------------------------------------------- */
      { }
    /* -- Initialiser with copy parameters --------------------------------- */
    Event(const Cmd cNCmd, const CBFunc &cbfNFunc, const Args &aNArgs) :
      /* -- Initialisers --------------------------------------------------- */
      cCmd(cNCmd),                     // Set requested command
      cbfFunc{ cbfNFunc },             // Set callback function
      aArgs{ aNArgs }                  // Copy requested parameters
      /* -- No code -------------------------------------------------------- */
      { }
    /* -- Move constructor ------------------------------------------------- */
    Event(Event &&cOther) :
      /* -- Initialisers --------------------------------------------------- */
      cCmd(cOther.cCmd),               // Set other command
      cbfFunc{ StdMove(cOther.cbfFunc) }, // Set other cb function
      aArgs{ StdMove(cOther.aArgs) }   // Move other parameters
      /* -- No code -------------------------------------------------------- */
      { }
    /* --------------------------------------------------------------------- */
    DELETECOPYCTORS(Event)             // Suppress default functions for safety
  };/* -- Private variables --------------------------------------- */ private:
  Funcs            fFuncs;             // Event callback storage
  mutex            mMutex;             // Primary events list mutex
  Queue            qlEvents;           // Primary events list
  const string     strName;            // Name of event module
  /* -- Generic event ------------------------------------------------------ */
  void BlankFunction(const Event &eEvent)
  { // Log the error
    cLog->LogWarningExSafe("$ ignored unregistered event! ($>$).",
      strName, eEvent.cCmd, eEvent.aArgs.size());
  }
  /* -- Get a function ----------------------------------------------------- */
  const CBFunc GetFunction(const Cmd cCmd)
  { // Get event function and return if it is valid
    if(cCmd < fFuncs.size()) return fFuncs[cCmd];
    // Log the error
    cLog->LogWarningExSafe("$ accessed an invalid event! ($>$).",
      strName, cCmd, fFuncs.size());
    // Return a blank function
    return bind(&EvtCore::BlankFunction, this, _1);
  }
  /* -- Execute specified event NOW (finisher) ----------------------------- */
  void ExecuteParam(const Cmd cCmd, Args &aArgs)
  { // Get callback function
    const CBFunc &fCB = GetFunction(cCmd);
    // Execute callback function
    fCB({ cCmd, fCB, StdMove(aArgs) });
  }
  /* -- Execute specified event NOW (parameters) --------------------------- */
  template<typename ...VarArgs,typename AnyType>
    void ExecuteParam(const Cmd cCmd, Args &aArgs, AnyType atArg,
      const VarArgs &...vaArgs)
  { // Insert parameter into list
    aArgs.push_back(EvtArgVar{ atArg });
    // Add more parameters
    ExecuteParam(cCmd, aArgs, vaArgs...);
  }
  /* -- Add with copy parameter semantics (finisher) ----------------------- */
  void AddParam(const Cmd cCmd, Args &aArgs)
  { // Event data to add to events list
    Event eEvent{ cCmd, GetFunction(cCmd), StdMove(aArgs) };
    // Wait and lock main event list
    const LockGuard lgEventsSync{ mMutex };
    // Move cell into event list
    qlEvents.emplace_back(StdMove(eEvent));
  }
  /* -- Add with copy parameter semantics (parameters) --------------------- */
  template<typename ...VarArgs, typename AnyType>
    void AddParam(const Cmd cCmd, Args &aArgs, AnyType atArg,
      const VarArgs &...vaArgs)
  { // Place parameter in list
    aArgs.emplace_back(EvtArgVar{ atArg });
    // Add more parameters or finish
    AddParam(cCmd, aArgs, vaArgs...);
  }
  /* -- list is empty? --------------------------------------------- */ public:
  bool Empty(void)
  { // Lock access to events list
    const LockGuard lgEventsSync{ mMutex };
    // Return if queue is empty
    return qlEvents.empty();
  }
  /* -- Returns number of events in queue ---------------------------------- */
  size_t SizeSafe(void)
  { // Lock access to events list
    const LockGuard lgEventsSync{ mMutex };
    // Return number of elements in queue
    return qlEvents.size();
  }
  /* -- Returns the final iterator ----------------------------------------- */
  const QueueConstIt Last(void) { return qlEvents.cend(); }
  /* -- Manage without lock ------------------------------------------------ */
  Cmd ManageUnsafe(void)
  { // Until event list is empty
    while(!qlEvents.empty())
    { // Get event data. Move it and never reference it!
      const Event epData{ StdMove(qlEvents.front()) };
      // Erase element. We're done with it. This is needed here incase the
      // callback throws an exception and causes an infinite loop.
      qlEvents.pop_front();
      // Log event if loggable
      if(epData.cCmd < EvtNoLog)
        cLog->LogDebugExSafe("$ processing event $.",
          strName, epData.cCmd);
      // No callback? Return command to loop
      if(!epData.cbfFunc) return epData.cCmd;
      // Execute the event callback
      epData.cbfFunc(epData);
    } // Return no significant event
    return EvtNone;
  }
  /* -- Manage with lock --------------------------------------------------- */
  Cmd ManageSafe(void)
  { // Try to lock access to events list.
    UniqueLock uLock{ mMutex, try_to_lock };
    // Since we call this in our engine loop. We are in a time critical
    // situation so we need to continue executing instead of waiting for
    // other threads to post events. So just return no event if threads are
    // busy posting events.
    if(uLock.owns_lock())
    { // Until event list is empty
      while(!qlEvents.empty())
      { // Get event data. Move it and never reference it!
        const Event epData{ StdMove(qlEvents.front()) };
        // Erase element. We're done with it. This is needed here incase the
        // callback throws an exception and causes an infinite loop.
        qlEvents.pop_front();
        // Log event if loggable
        if(epData.cCmd < EvtNoLog)
          cLog->LogDebugExSafe("$ system processing event $.",
            strName, epData.cCmd);
        // No callback? Return command to loop
        if(!epData.cbfFunc) return epData.cCmd;
        // Unlock mutex so events can still be added
        uLock.unlock();
        // Execute the event callback
        epData.cbfFunc(epData);
        // Relock mutex if we can and break out of loop if failed
        if(!uLock.try_lock()) break;
      }
    } // Return no significant event
    return EvtNone;
  }
  /* -- Flush events list -------------------------------------------------- */
  void Flush(void)
  { // Lock access to the events list from other threads
    const LockGuard lgEventsSync{ mMutex };
    // Clear the events list
    qlEvents.clear();
  }
  /* -- Execute specified event NOW (starter) ------------------------------ */
  template<typename ...VarArgs>
    void Execute(const Cmd cCmd, const VarArgs &...vaArgs)
  { // Parameters list
    Args aArgs;
    // Reserve memory for parameters
    aArgs.reserve(sizeof...(VarArgs));
    // Prepare parameters list and execute
    ExecuteParam(cCmd, aArgs, vaArgs...);
  }
  /* -- Add with copy parameter semantics (starter) ------------------------ */
  template<typename ...VarArgs>
    void Add(const Cmd cCmd, const VarArgs &...vaArgs)
  { // Parameters list
    Args aArgs;
    // Reserve memory for parameters
    aArgs.reserve(sizeof...(VarArgs));
    // Prepare parameters list and add a new event
    AddParam(cCmd, aArgs, vaArgs...);
  }
  /* -- Add to events and return iterator (finisher) ----------------------- */
  void AddExParam(const Cmd cCmd, QueueConstIt &qciItem, Args &aArgs)
  { // Setup cell to insert
    Event eEvent{ cCmd, GetFunction(cCmd), StdMove(aArgs) };
    // Try to lock main event list
    const LockGuard lgEventsSync{ mMutex };
    // Push new event whilst move parameters into it
    qciItem = StdMove(qlEvents.emplace(qlEvents.cend(), StdMove(eEvent)));
  }
  /* -- Add to events and return iterator (parameters) --------------------- */
  template<typename ...VarArgs, typename AnyType>
    void AddExParam(const Cmd cCmd, QueueConstIt &qciItem,
      Args &aArgs, AnyType atArg, const VarArgs &...vaArgs)
  { // Place parameter into parameter list
    aArgs.emplace_back(EvtArgVar{ atArg });
    // Add more parameters or finish
    AddExParam(cCmd, qciItem, aArgs, vaArgs...);
  }
  /* -- Queue and event and return the id of the event copy params --------- */
  template<typename ...VarArgs>
    const QueueConstIt AddEx(const Cmd cCmd, const VarArgs &...vaArgs)
  { // Iterator to return
    QueueConstIt qciItem;
    // Parameters list
    Args aArgs;
    // Reserve parameters
    aArgs.reserve(sizeof...(VarArgs));
    // Prepare parameters list and execute
    AddExParam(cCmd, qciItem, aArgs, vaArgs...);
    // Return iterator
    return qciItem;
  }
  /* -- Remove event ------------------------------------------------------- */
  void Remove(const QueueConstIt &qciIt)
  { // Try to lock main event list
    const LockGuard lgEventsSync{ mMutex };
    // Remove the event
    qlEvents.erase(qciIt);
  }
  /* -- Register event ----------------------------------------------------- */
  void Register(const Cmd cCmd, const CBFunc &cbfFunc)
  { // Bail if invalid command
    if(cCmd >= fFuncs.size())
      XC("Invalid registration command!",
         "System",   strName, "Event", cCmd,
         "Function", reinterpret_cast<const void*>(&cbfFunc));
    // Assign callback function to event
    fFuncs[cCmd] = cbfFunc;
  }
  /* -- Register multiple events ------------------------------------------- */
  void RegisterEx(const RegVec &rvEvents)
    { for(const RegPair &rpItem : rvEvents)
        Register(rpItem.first, rpItem.second); }
  /* -- Unregister event --------------------------------------------------- */
  void Unregister(const Cmd cCmd)
  { // Bail if invalid command
    if(cCmd >= fFuncs.size())
      XC("Invalid de-registration command!", "System",
        strName, "Event", cCmd);
    // Unassign callback function
    fFuncs[cCmd] = bind(&EvtCore::BlankFunction, this, _1);
  }
  /* -- Unregister multiple events ----------------------------------------- */
  void UnregisterEx(const RegVec &rvEvents)
    { for(const RegPair &rpItem : rvEvents) Unregister(rpItem.first); }
    /* -- Event data, all empty functions ---------------------------------- */
  explicit EvtCore(const string &strN) : strName{StdMove(strN)}
    { fFuncs.fill(bind(&EvtCore::BlankFunction, this, _1)); }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(EvtCore)             // Suppress default functions for safety
};/* ----------------------------------------------------------------------- */
};                                     // End of public module namespace
/* ------------------------------------------------------------------------- */
};                                     // End of private module namespace
/* == EoF =========================================================== EoF == */
