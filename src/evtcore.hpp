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
using namespace IError::P;             using namespace IIdent::P;
using namespace ILog::P;               using namespace IMutex::P;
using namespace IStd::P;               using namespace IUtil::P;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public public namespace
/* ------------------------------------------------------------------------- */
enum EvtArgVarType : unsigned int      // EvtArgVar variable types
{ /* ----------------------------------------------------------------------- */
  EAVT_BOOL,     EAVT_CSTR,    EAVT_STR, EAVT_PTR,       EAVT_FLOAT,
  EAVT_DOUBLE,   EAVT_UINT,    EAVT_INT, EAVT_ULONGLONG, EAVT_LONGLONG,
  EAVT_LONGUINT, EAVT_LONGINT, EAVT_MAX
};/* ----------------------------------------------------------------------- */
class EvtArgVar                        // Multi-type helps access event data
{ /* ----------------------------------------------------------------------- */
  const EvtArgVarType  t;              // Variable type
  /* ----------------------------------------------------------------------- */
  union                                // Variables share same memory space
  { /* -- All these use the same memory ------------------------------------ */
    bool               b;              // Boolean .................... (1 byte)
    char              *cp;             // C-String pointer ........ (4-8 bytes)
    double             d;              // Double .................... (8 bytes)
    float              f;              // Float ..................... (4 bytes)
    long signed int    lsi;            // Long Signed Integer ..... (4-8 bytes)
    long unsigned int  lui;            // Long Unsigned Integer ... (4-8 bytes)
    signed int         si;             // Signed Integer ............ (4 bytes)
    signed long long   sll;            // Signed Long Long .......... (8 bytes)
    size_t             st;             // Size Integer ............ (4-8 bytes)
    string            *str;            // STL-String pointer ...... (4-8 bytes)
    unsigned int       ui;             // Unsigned Integer .......... (4 bytes)
    unsigned long long ull;            // Unsigned Long Long ........ (8 bytes)
    void              *vp;             // Pointer ................. (4-8 bytes)
  }; /* ------------------------------------------------------------ */ public:
  EvtArgVarType Type() const { return t; }
  /* ----------------------------------------------------------------------- */
  template<typename AnyCast=void>AnyCast *Ptr() const
    { return reinterpret_cast<AnyCast*>(vp); }
  template<typename AnyCast>AnyCast &Ref() const
    { return *Ptr<AnyCast>(vp); }
  /* ----------------------------------------------------------------------- */
  char *CStr() const { return cp; }
  bool Bool() const { return b; }
  size_t SizeT() const { return st; }
  /* ----------------------------------------------------------------------- */
  signed int Int() const { return si; }
  unsigned int UInt() const { return ui; }
  /* ----------------------------------------------------------------------- */
  long signed int Long() const { return lsi; }
  long unsigned int ULong() const { return lui; }
  /* ----------------------------------------------------------------------- */
  signed long long LongLong() const { return sll; }
  unsigned long long ULongLong() const { return ull; }
  /* ----------------------------------------------------------------------- */
  double Double() const { return d; }
  float Float() const { return f; }
  /* ----------------------------------------------------------------------- */
  string *StrPtr() const { return str; }
  string &Str() const { return *StrPtr(); }
  /* ----------------------------------------------------------------------- */
  explicit EvtArgVar(const void*const vpP) :
    t(EAVT_PTR), vp(const_cast<void*>(vpP)) {}
  explicit EvtArgVar(const std::nullptr_t) :
    t(EAVT_PTR), vp(nullptr) {}
  explicit EvtArgVar(const char*const cpP) :
    t(EAVT_CSTR), cp(const_cast<char*>(cpP)) {}
  explicit EvtArgVar(const string &strV) :
    t(EAVT_STR), str(const_cast<string*>(&strV)) {}
  explicit EvtArgVar(const unsigned int uiV) :
    t(EAVT_UINT), ui(static_cast<unsigned int>(uiV)) {}
  explicit EvtArgVar(const signed int siV) :
    t(EAVT_INT), si(static_cast<signed int>(siV)){}
  explicit EvtArgVar(const double dV) :
    t(EAVT_DOUBLE), d(dV) {}
  explicit EvtArgVar(const float fV):
    t(EAVT_FLOAT), f(fV) {}
  explicit EvtArgVar(const unsigned long long ullV) :
    t(EAVT_ULONGLONG), ull(ullV) {}
  explicit EvtArgVar(const signed long long sllV) :
    t(EAVT_LONGLONG), sll(sllV) {}
  explicit EvtArgVar(const long unsigned int luiV) :
    t(EAVT_LONGUINT), lui(luiV) {}
  explicit EvtArgVar(const long signed int liV) :
    t(EAVT_LONGINT), lsi(liV) {}
  explicit EvtArgVar(const bool bV):
    t(EAVT_BOOL), b(bV) {}
};/* ----------------------------------------------------------------------- */
/* -- Common events system (since we need to use this twice) --------------- */
template<typename Cmd,                 // Variable type of command to use
         size_t   EvtMaxEvents,        // Maximum number of events
         Cmd      EvtNone,             // Id of 'none' event
         Cmd      EvtNoLog>            // Id of succeeding ids to not log for
class EvtCore :                        // Start of common event system class
  /* -- Base classes ------------------------------------------------------- */
  private Ident,                       // Identifier of event list
  public MutexLock                     // Primary events list mutex
{ /* -- Typedefs --------------------------------------------------- */ public:
  struct Event;                           // (Prototype) Event packet info
  typedef void (CbEcFuncT)(const Event&); // Event callback type
  typedef function<CbEcFuncT> CbEcFunc;   // Actual event callback
  /* ----------------------------------------------------------------------- */
  typedef array<CbEcFunc, EvtMaxEvents>  Funcs;        // Reg'd events vector
  typedef list<Event>                    Queue;        // Current events queue
  typedef typename Queue::const_iterator QueueConstIt; // Current events queue
  /* ----------------------------------------------------------------------- */
  typedef pair<const Cmd, const CbEcFunc> RegPair; // Event command and cb func
  typedef const vector<RegPair>           RegVec;  // Event list
  /* ----------------------------------------------------------------------- */
  typedef vector<EvtArgVar> EvtArgs;   // Vector of RegPairs
  /* ----------------------------------------------------------------------- */
  typedef IdList<EvtMaxEvents> ISList; // Events as strings
  const ISList &islEventStrings;       // Actual variable
  /* ----------------------------------------------------------------------- */
  class RegAuto                        // Automatic event registration
  { /* -- Private variables ------------------------------------------------ */
    EvtCore       &ecCore;             // Events core to unregister from
    const RegVec   rvEvents;           // Reference to events list to handle
    /* -- Constructor that registers the events -------------------- */ public:
    RegAuto(EvtCore*const ecpCore, const RegVec &&rvNEvents) :
      ecCore{ *ecpCore }, rvEvents{ StdMove(rvNEvents) }
    { // Register all events (order doesn't matter)
      StdForEach(par_unseq, rvEvents.cbegin(), rvEvents.cend(),
        [this](const RegPair &rpItem)
          { ecCore.Register(rpItem.first, rpItem.second); });
    }
    /* -- Destructor ------------------------------------------------------- */
    ~RegAuto()
    { // Unregister all events (order doesn't matter)
      StdForEach(par_unseq, rvEvents.cbegin(), rvEvents.cend(),
        [this](const RegPair &rpItem)
          { ecCore.Unregister(rpItem.first); });
    }
  }; /* -------------------------------------------------------------------- */
  struct Event                         // Event packet information
  { /* --------------------------------------------------------------------- */
    Cmd            cCmd;               // Command send
    const CbEcFunc &cbfFunc;           // Function to call
    EvtArgs        eaArgs;             // User parameters
    /* -- Constructor with move parameters --------------------------------- */
    Event(const Cmd cNCmd, const CbEcFunc &cbfNFunc, EvtArgs &&eaNArgs) :
      /* -- Initialisers --------------------------------------------------- */
      cCmd(cNCmd),                     // Set requested command
      cbfFunc{ cbfNFunc },             // Set callback function
      eaArgs{ StdMove(eaNArgs) }       // Move requested parameters
      /* -- No code -------------------------------------------------------- */
      {}
    /* -- Initialiser with copy parameters --------------------------------- */
    Event(const Cmd cNCmd, const CbEcFunc &cbfNFunc, const EvtArgs &eaNArgs) :
      /* -- Initialisers --------------------------------------------------- */
      cCmd(cNCmd),                     // Set requested command
      cbfFunc{ cbfNFunc },             // Set callback function
      eaArgs{ eaNArgs }                // Copy requested parameters
      /* -- No code -------------------------------------------------------- */
      {}
    /* -- Move constructor ------------------------------------------------- */
    Event(Event &&cOther) :
      /* -- Initialisers --------------------------------------------------- */
      cCmd(cOther.cCmd),               // Set other command
      cbfFunc{ cOther.cbfFunc },       // Set other cb function
      eaArgs{ StdMove(cOther.eaArgs) } // Move other parameters
      /* -- No code -------------------------------------------------------- */
      {}
  };/* -- Private variables --------------------------------------- */ private:
  const CbEcFunc   cefEmpty;           // Empty function
  Funcs            fFuncs;             // Event callback storage
  Queue            qlEvents;           // Primary events list
  /* -- Generic event that reports use as warning -------------------------- */
  void WarningFunction(const Event &eEvent)
  { // Log the error
    cLog->LogWarningExSafe("$ ignored unregistered event $<$>.",
      IdentGet(), IdToString(eEvent.cCmd), eEvent.cCmd);
  }
  /* -- Execute specified event NOW (finisher) ----------------------------- */
  void ExecuteParam(const Cmd cCmd, EvtArgs &eaArgs)
  { // Get callback function and execute callback function
    const CbEcFunc &fnCB = fFuncs[cCmd];
    fnCB({ cCmd, fnCB, StdMove(eaArgs) });
  }
  /* -- Execute specified event NOW (parameters) --------------------------- */
  template<typename ...VarArgs,typename AnyType>
    void ExecuteParam(const Cmd cCmd, EvtArgs &eaArgs, AnyType atArg,
      const VarArgs ...vaArgs)
  { // Check specified parameter is valid
    static_assert(is_enum_v<AnyType> || is_integral_v<AnyType> ||
      is_pointer_v<AnyType> || is_null_pointer_v<AnyType>,
        "Must be enum, integer, pointer or nullptr!");
    // Insert parameter into list and add more parameters. It only accepts
    // simple integers and pointers, hence why the direct copy on ...vaArgs.
    eaArgs.push_back(EvtArgVar{ atArg });
    ExecuteParam(cCmd, eaArgs, vaArgs...);
  }
  /* -- Add with copy parameter semantics (finisher) ----------------------- */
  void AddParam(const Cmd cCmd, EvtArgs &eaArgs)
  { // Event data to add to events list
    Event eEvent{ cCmd, fFuncs[cCmd], StdMove(eaArgs) };
    // Try to lock main event list and move cell into event list
    MutexCall([this, &eEvent](){ qlEvents.emplace_back(StdMove(eEvent)); });
  }
  /* -- Add with copy parameter semantics (parameters) --------------------- */
  template<typename ...VarArgs, typename AnyType>
    void AddParam(const Cmd cCmd, EvtArgs &eaArgs, AnyType atArg,
      const VarArgs ...vaArgs)
  { // Place parameter in list and add more parameters or finish
    eaArgs.emplace_back(EvtArgVar{ atArg });
    AddParam(cCmd, eaArgs, vaArgs...);
  }
  /* -- Lock access to events list and return if queue is empty ---- */ public:
  bool Empty() { return MutexCall([this](){ return qlEvents.empty(); }); }
  /* -- Convert event id to string ----------------------------------------- */
  const string_view &IdToString(const Cmd cCmd) const
    { return islEventStrings.Get(cCmd); }
  /* -- Lock access to events list and return number of elements in queue -- */
  size_t SizeSafe() { return MutexCall([this](){ return qlEvents.size(); }); }
  /* -- Returns the final iterator ----------------------------------------- */
  const QueueConstIt Last() { return qlEvents.cend(); }
  /* -- Manage with lock --------------------------------------------------- */
  Cmd Manage()
  { // Try to lock access to events list.
    return MutexUniqueCall([this](UniqueLock &ulLock){
      // Since we call this in our engine loop. We are in a time critical
      // situation so we need to continue executing instead of waiting for
      // other threads to post events. So just return no event if threads are
      // busy posting events.
      if(!ulLock.owns_lock()) return EvtNone;
      // Until event list is empty
      while(!qlEvents.empty())
      { // Get event data. Move it and never reference it!
        const Event epData{ StdMove(qlEvents.front()) };
        // Erase element. We're done with it. This is needed here incase the
        // callback throws an exception and causes an infinite loop.
        qlEvents.pop_front();
        // Log event if loggable
        if(epData.cCmd < EvtNoLog)
          cLog->LogDebugExSafe("$ system processing event $<$>.",
            IdentGet(), IdToString(epData.cCmd), epData.cCmd);
        // No callback? Return command to loop
        if(!epData.cbfFunc) return epData.cCmd;
        // Unlock the mutex while we execute the callback. This is so more
        // events can be appended while this one is being processed.
        const UniqueRelock urLock{ ulLock };
        // Execute the event callback
        epData.cbfFunc(epData);
      } // Return no significant event
      return EvtNone;
    }, try_to_lock);
  }
  /* -- Flush events list -------------------------------------------------- */
  void Flush()
  { // Lock access to the events list from other threads
    if(const size_t stCleared = MutexCall([this]()->size_t{
      // Return if no events to clear
      if(qlEvents.empty()) return 0;
      // Store number of events cleared
      const size_t stCleared = qlEvents.size();
      // Clear the events list
      qlEvents.clear();
      // Return events cleared
      return stCleared;
    })) // Write number of events cleared
      cLog->LogDebugExSafe("$ cleared $ lingering events.",
        IdentGet(), stCleared);
  }
  /* -- Execute specified event NOW (starter) ------------------------------ */
  template<typename ...VarArgs>
    void Execute(const Cmd cCmd, const VarArgs ...vaArgs)
  { // Parameters list
    EvtArgs eaArgs;
    // Reserve memory for parameters
    eaArgs.reserve(sizeof...(VarArgs));
    // Prepare parameters list and execute
    ExecuteParam(cCmd, eaArgs, vaArgs...);
  }
  /* -- Add with copy parameter semantics (starter) ------------------------ */
  template<typename ...VarArgs>
    void Add(const Cmd cCmd, const VarArgs ...vaArgs)
  { // Parameters list
    EvtArgs eaArgs;
    // Reserve memory for parameters
    eaArgs.reserve(sizeof...(VarArgs));
    // Prepare parameters list and add a new event
    AddParam(cCmd, eaArgs, vaArgs...);
  }
  /* -- Add to events and return iterator (finisher) ----------------------- */
  void AddExParam(const Cmd cCmd, QueueConstIt &qciItem, EvtArgs &eaArgs)
  { // Setup cell to insert
    Event eEvent{ cCmd, fFuncs[cCmd], StdMove(eaArgs) };
    // Try to lock event list and push new event whilst move parameters into it
    MutexCall([this, &qciItem, &eEvent](){
      qciItem = StdMove(qlEvents.emplace(qlEvents.cend(), StdMove(eEvent)));
    });
  }
  /* -- Add to events and return iterator (parameters) --------------------- */
  template<typename ...VarArgs, typename AnyType>
    void AddExParam(const Cmd cCmd, QueueConstIt &qciItem,
      EvtArgs &eaArgs, AnyType atArg, const VarArgs ...vaArgs)
  { // Place parameter into parameter list and add more parameters or finish
    eaArgs.emplace_back(EvtArgVar{ atArg });
    AddExParam(cCmd, qciItem, eaArgs, vaArgs...);
  }
  /* -- Queue and event and return the id of the event copy params --------- */
  template<typename ...VarArgs>
    const QueueConstIt AddEx(const Cmd cCmd, const VarArgs ...vaArgs)
  { // Iterator to return
    QueueConstIt qciItem;
    // Parameters list
    EvtArgs eaArgs;
    // Reserve parameters
    eaArgs.reserve(sizeof...(VarArgs));
    // Prepare parameters list and execute
    AddExParam(cCmd, qciItem, eaArgs, vaArgs...);
    // Return iterator
    return qciItem;
  }
  /* -- Remove event (lock is caller responsibility) ----------------------- */
  void RemoveUnsafe(const QueueConstIt &qciIt) { qlEvents.erase(qciIt); }
  /* -- Register single event ---------------------------------------------- */
  void Register(const Cmd cCmd, const CbEcFunc &cbfFunc)
  { // Bail if invalid command
    if(cCmd >= fFuncs.size())
      XC("Invalid registration command!",
         "System",   IdentGet(), "Event", IdToString(cCmd), "EventID", cCmd,
         "Function", reinterpret_cast<const void*>(&cbfFunc));
    // Assign callback function to event
    fFuncs[cCmd] = cbfFunc;
  }
  /* -- Unregister single  event ------------------------------------------- */
  void Unregister(const Cmd cCmd)
  { // Bail if invalid command
    if(cCmd >= fFuncs.size())
      XC("Invalid de-registration command!", "System",
        IdentGet(), "Event", IdToString(cCmd), "EventID", cCmd);
    // Unassign callback function
    fFuncs[cCmd] = cefEmpty;
  }
  /* -- Event data, all empty functions ------------------------- */ protected:
  EvtCore(string &&strCName, const ISList &islStrings) :
    /* -- Initialisers ----------------------------------------------------- */
    Ident{ StdMove(strCName) },        // Initialise event system name
    islEventStrings{ islStrings },     // Initialise event id names
    cefEmpty{ bind(&EvtCore::WarningFunction, this, _1) },
    fFuncs{ UtilMkFilledContainer<Funcs>(cefEmpty) }
    /* -- No code ---------------------------------------------------------- */
    {}
};/* ----------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
