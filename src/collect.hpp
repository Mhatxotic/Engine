/* == COLLECT.HPP ========================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## This module contains routines in helping keeping a record of each   ## **
** ## allocated member pointers in a collector list and keeping a limit   ## **
** ## on the size of the list. It also features optional synchronisation. ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace ICollector {                 // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IClock::P;             using namespace ICVarDef::P;
using namespace IError::P;             using namespace IIdent::P;
using namespace ILog::P;               using namespace ILuaLib::P;
using namespace IStd::P;               using namespace IString::P;
using namespace ISysUtil::P;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* -- Class for collector class name in LUA -------------------------------- */
class LuaIdent {
  /* -- Private variables -------------------------------------------------- */
  const string_view svName;            // Name of class
  /* -- Public variables ------------------------------------------- */ public:
  int             &iRef;               // Reference to namespace in Lua
  /* -- Get name of class -------------------------------------------------- */
  const char* CStr(void) const { return svName.data(); }
  /* -- Get string view name of class -------------------------------------- */
  const string_view &Str(void) const { return svName; }
  /* -- Constructor -------------------------------------------------------- */
  explicit LuaIdent(const string_view svNName, const LuaClassId lciId) :
    /* --------------------------------------------------------------------- */
    svName{svNName},                   // Set name of class
    iRef(llcirAPI[lciId])              // Alias Lua class reference storage
    /* -- No code ---------------------------------------------------------- */
    { }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(LuaIdent)            // Suppress default functions for safety
};/* -- Collector header --------------------------------------------------- */
#define CTOR_HDR_BEGIN(p,              /* The parent (collector) class type */\
                       m,              /* The member class type             */\
                       l,              /* The parent container type typedef */\
                       ln,             /* Alias of complete container type  */\
                       in,             /* Alias of container iter type      */\
                       cn,             /* Alias of cont const iter type     */\
                       h,              /* CLHelper or CLHelperAsync         */\
                       hn,             /* Alias of complete helper type     */\
                       s,              /* CLHelperSafe or CLHelperUnsafe    */\
                       x,              /* Extra class body arguments (no ,) */\
                       ...)            /* Extra derivced classes            */\
  class m;                             /* Member class prototype            */\
  typedef l<m*> ln;                    /* Create container type alias       */\
  typedef ln::iterator in;             /* Create iterator type alias        */\
  typedef ln::const_iterator cn;       /* Create const iterator type alias  */\
  typedef h<m,s<m,ln,in>,ln,in> hn;    /* Create alias for collector type   */\
  static struct p final :              /* Begin collector object class      */\
    public LuaIdent,                   /* Name of object for Lua            */\
    public hn                          /* Derived by our collector class    */\
    __VA_ARGS__                        /* Any other custom class derives    */\
  { DELETECOPYCTORS(p)                 /* Suppress copy constructor         */\
    p(void);                           /* Constructor prototype             */\
    ~p(void) noexcept(false);          /* Destructor prototype              */\
    x                                  /* Any extra variables? (no comma!)  */
/* -- Collector header that lets you use a custom container ---------------- */
#define CTOR_HDR_CUSTCTR(p,m,l,h,s,x,...) \
  CTOR_HDR_BEGIN(p,m,l,p ## Ctr,p ## It,p ## ItConst,h,p ## Helper,\
    s,x,## __VA_ARGS__)
/* -- Collector header that assumes std::list for the container type ------- */
#define CTOR_HDR_DEFCTR(p,m,h,s,x,...) \
  CTOR_HDR_CUSTCTR(p,m,list,h,s,x,## __VA_ARGS__)
/* -- Collector footer that creates the global class pointer init in core -- */
#define CTOR_HDR_END(p) } *c ## p = nullptr;
/* -- Build a collector class with base classes and body ------------------- */
#define CTOR_BEGIN(p,m,s,x,...) \
  CTOR_HDR_DEFCTR(p,m,CLHelper,        /* Insert standard header            */\
    s,x,## __VA_ARGS__)                /*   with body and custom derives    */\
  CTOR_HDR_END(p)                      /* Insert standard footer             */
/* -- Build a collector class body with a fully custom body ---------------- */
#define CTOR_BEGIN_NOBASE(p,m,s,...) \
  CTOR_HDR_DEFCTR(p,m,CLHelper,s,)     /* Begin standard header             */\
  __VA_ARGS__                          /* Insert user collector body        */\
  CTOR_HDR_END(p)                      /* Insert standard footer             */
/* -- Build a collector class body with no special parameters -------------- */
#define CTOR_BEGIN_NOBB(p,m,s) CTOR_BEGIN(p,m,s,,)
/* -- Build a collector class body with no special parameters only type ---- */
#define CTOR_BEGIN_CUSTCTR(p,m,l,s,...) \
  CTOR_HDR_CUSTCTR(p,m,l,CLHelper,s,)  /* Begin std hdr with custom ctr     */\
  __VA_ARGS__                          /* Insert user collector body        */\
  CTOR_HDR_END(p)                      /* Insert standard footer             */
/* -- Thread safe collector with user-defined variables or classes --------- */
#define CTOR_BEGIN_ASYNC(p,m,h,x,...) \
  CTOR_HDR_DEFCTR(p,m,CLHelperAsync,h,x,## __VA_ARGS__) \
  CTOR_HDR_END(p)
/* -- Tailing collector class macro with init and deinit calls ------------- */
#define CTOR_END_EX(p,                 /* The parent (collector) class type */\
                    m,                 /* The child (member) class type     */\
                    c,                 /* Lua class id code (luadef.hpp)    */\
                    i,                 /* Constructor initialisation code   */\
                    d,                 /* Destructor de-initialisation code */\
                    ...)               /* Constructor initialisers code     */\
  p::p(void) : LuaIdent{#m, LMT_ ## c}, __VA_ARGS__ { IHInitialise(); i; } \
  DTORHELPER(p::~p, if(IHNotDeInitialise()) return; d)
/* -- Tailing collector class macro that inits a collector helper ---------- */
#define CTOR_END(p,m,c,i,d,...) \
  CTOR_END_EX(p,m,c,i,d,CLHelper{STR(p)} __VA_ARGS__)
/* -- Tailing collector class macro with no init and deinit calls ---------- */
#define CTOR_END_NOINITS(p,m,c) CTOR_END(p,m,c,,,)
/* -- Tailing async collector class macro with init and deinit calls ------- */
#define CTOR_END_ASYNC(p,m,c,e,i,d,...)  /* 'e' is the EvtMain event id     */\
  CTOR_END_EX(p,m,c,i,d,CLHelperAsync{STR(p), EMC_MP_ ## e} __VA_ARGS__);
/* -- Tailing async collector class macro with no init nor deinit calls ---- */
#define CTOR_END_ASYNC_NOFUNCS(p,m,c,e,...) \
  CTOR_END_ASYNC(p,m,c,e,,,,## __VA_ARGS__)
/* -- Start building a member class for a collector ------------------------ */
#define CTOR_MEM_BEGIN_EX(p,           /* The collector type                */\
                          m,           /* The member type                   */\
                          i,           /* The iterator type                 */\
                          s,           /* ICHelperSafe or ICHelperUnsafe    */\
                          n,           /* The completed helper type alias   */\
                          ...)         /* Extra arguments                   */\
  typedef ICHelper<p,m,s<p,m,i>,i> n;  /* Make an alias for the locktype    */\
  class m : public n                   /* Begin the member class            */\
    __VA_ARGS__                        /* Add extra arguments if needed     */
/* -- Start building a member class for a collector ------------------------ */
#define CTOR_MEM_BEGIN(p,m,s,...) \
  CTOR_MEM_BEGIN_EX(p,m,p ## It,s,ICHelper ## m,## __VA_ARGS__)
/* -- Start building a member class for a collector ------------------------ */
#define CTOR_MEM_BEGIN_CSLAVE(p,m,s) \
  CTOR_MEM_BEGIN(p,m,s,,public IdentCSlave<>)
/* -- All in one collector and member builder ------------------------------ */
#define CTOR_BEGIN_DUO(p,m,h,s) \
  CTOR_BEGIN_NOBB(p,m,h)               /* Start building collector class    */\
  CTOR_MEM_BEGIN_CSLAVE(p,m,s)         /* Start building member class        */
/* -- Start building a member class for a collector ------------------------ */
#define CTOR_MEM_BEGIN_ASYNC_EX(p,m,i,s,n,a,...) \
  typedef ICHelper<p,m,s<p,m,i>,i> n;  /* Create alias for locktype class   */\
  typedef AsyncLoader<m,n> a;          /* Create alias for loader class     */\
  class m : public n                   /* Begin the member class            */\
    __VA_ARGS__                        /* Add extra arguments if needed      */
/* -- Start building a member class for a collector ------------------------ */
#define CTOR_MEM_BEGIN_ASYNC(p,m,s,...) \
  CTOR_MEM_BEGIN_ASYNC_EX(p,m,p ## It,s,ICHelper ## m, \
    AsyncLoader ## m,## __VA_ARGS__)
/* -- Start building a member class for a collector ------------------------ */
#define CTOR_MEM_BEGIN_ASYNC_CSLAVE(p,m,s) \
  CTOR_MEM_BEGIN_ASYNC(p,m,s,,         /* Use expanded macro                */\
    public IdentCSlave<>)              /* Counter id slave class             */
/* -- All in one async collector and async member builder ------------------ */
#define CTOR_BEGIN_ASYNC_DUO(p,m,h,s) \
  CTOR_BEGIN_ASYNC(p,m,h,,)            /* Start building collector class    */\
  CTOR_MEM_BEGIN_ASYNC_CSLAVE(p,m,s)   /* Start building member class        */
/* == Init Helper Class ==================================================== **
** ######################################################################### **
** ## This class holds the name of a class and if it has been             ## **
** ## initialised. It stops classes being accidentally double             ## **
** ## initialised due to programming errors.                              ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
class IHelper :                        // The Init Helper class
  /* -- Base classes ------------------------------------------------------- */
  public IdentConst                    // Holds the identifier
{ /* -- Private variables -------------------------------------------------- */
  ClkTimePoint     ctInitialised,      // Time class was initialised
                   ctDeinitialised;    // Time class was deinitialised
  /* ----------------------------------------------------------------------- */
  void IHSetInitialised(void) { ctInitialised = cmHiRes.GetTime(); }
  void IHSetDeInitialised(void) { ctDeinitialised = cmHiRes.GetTime(); }
  /* --------------------------------------------------------------- */ public:
  bool IHIsInitialised(void) const { return ctInitialised > ctDeinitialised; }
  bool IHIsNotInitialised(void) const { return !IHIsInitialised(); }
  /* ----------------------------------------------------------------------- */
  void IHInitialise(void)
  { // Raise exception if object already initialised
    if(IHIsInitialised())
      XC("Object already initialised!",
         "Identifier", IdentGet(),
         "Age", StrShortFromDuration(cmHiRes.
                  TimePointToClampedDouble(ctInitialised)));
    // Class now initialised
    IHSetInitialised();
  }
  /* ----------------------------------------------------------------------- */
  bool IHDeInitialise(void)
  { // Return if class already initialised
    if(IHIsNotInitialised()) return false;
    // Class now de-initialised
    IHSetDeInitialised();
    // Success
    return true;
  }
  /* ----------------------------------------------------------------------- */
  bool IHNotDeInitialise(void) { return !IHDeInitialise(); }
  /* -- Constructors -------------------------------------------- */ protected:
  explicit IHelper(const string_view &strvName) :
    /* -- Initialisers ----------------------------------------------------- */
    IdentConst{strvName},              // Initialise name
    ctInitialised{seconds{0}},         // Clear initialised time
    ctDeinitialised{cmHiRes.GetTime()} // Set deinitialised time
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Default suppressions ----------------------------------------------- */
  DELETECOPYCTORS(IHelper)             // Suppress default functions for safety
};/* ----------------------------------------------------------------------- */
/* == Collector class helper =============================================== **
** ######################################################################### **
** ## Derive this class into the main class to allow collection of child  ## **
** ## classes.                                                            ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
template<class MemberType, class ListType, class IteratorType>
class CLHelperBase :
  /* -- Base classes ------------------------------------------------------- */
  public IdentCMaster<>,               // Counter master class
  public ListType,                     // The list of members
  public IHelper                       // Initialisation helper
{ /* -- Private variables -------------------------------------------------- */
  size_t           stMaximum;          // Maximum children allowed
  /* -- Return last item in the list ------------------------------- */ public:
  IteratorType CLBaseGetLastItemUnsafe(void) { return this->end(); }
  /* -- Return if collector is empty --------------------------------------- */
  bool CLBaseIsEmptyUnsafe(void) {  return this->empty(); }
  /* -- Destroy all objects (clear() doesn't do this!) --------------------- */
  void CLBaseDestroyUnsafe(void)
    { while(!CLBaseIsEmptyUnsafe()) delete this->back(); }
  /* -- Return number of registered items ---------------------------------- */
  size_t CLBaseCountUnsafe(void) { return this->size(); }
  /* -- Reference counter increment ---------------------------------------- */
  void CLBaseCheckUnsafe(void)
  { // Throw error if at limit. This check is only performed during
    // construction of an object. Construction should never occur off-thread or
    // we will need to serialise this access.
    if(CLBaseCountUnsafe() >= stMaximum)
      XC("Collector child object threshold exceeded!",
         "Type", IdentGet(), "Current", CLBaseCountUnsafe(),
         "Maximum", stMaximum);
  }
  /* -- Add/remove object to the list -------------------------------------- */
  IteratorType CLBaseAddUnsafe(MemberType*const mtObj)
    { return this->emplace(CLBaseGetLastItemUnsafe(), mtObj); }
  IteratorType CLBaseEraseUnsafe(IteratorType &itObj)
    { this->erase(itObj); return CLBaseGetLastItemUnsafe(); }
  /* -- Check and destroy -------------------------------------------------- */
  void CLBaseCheckAndDestroyUnsafe(void)
  { // Return if class was initialised and there are no children left
    if(CLBaseIsEmptyUnsafe()) return;
    // Show message box to say we have items remaining
    cLog->LogWarningExSafe("$ collector unloading $ remaining objects...",
      IdentGet(), CLBaseCountUnsafe());
    CLBaseDestroyUnsafe();
    cLog->LogWarningExSafe("$ collector unloaded the remaining objects!",
      IdentGet());
  }
  /* -- Constructor -------------------------------------------------------- */
  explicit CLHelperBase(const string_view &strvName) :
    /* -- Initialisers ----------------------------------------------------- */
    IHelper{ strvName },               // Set initialisation helper name
    stMaximum(StdMaxSizeT)             // Initialise maximum objects
    /* -- No code ---------------------------------------------------------- */
    { }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(CLHelperBase)        // Suppress default functions for safety
  /* -- Set limit ---------------------------------------------------------- */
  CVarReturn CLBaseSetLimitUnsafe(const size_t stLimit)
    { return CVarSimpleSetInt(stMaximum, stLimit); }
};/* ----------------------------------------------------- Collector::End -- */
template<class MemberType, class ListType, class IteratorType,
         class BaseType = CLHelperBase<MemberType, ListType, IteratorType>>
class CLHelperSafe :
  /* -- Base classes ------------------------------------------------------- */
  public BaseType,                     // The collector base type
  private mutex                        // Member list locking protection
{ /* -- Protected functions ------------------------------------- */ protected:
  void CLLock(void) { CollectorGetMutex().lock(); }
  void CLUnlock(void) { CollectorGetMutex().unlock(); }
  /* -- Lock the mutex and return if empty --------------------------------- */
  bool CLEmpty(void)
    { const LockGuard lgGuardEmpty{ CollectorGetMutex() };
      return this->CLBaseIsEmptyUnsafe(); }
  /* -- Lock the mutex and check size -------------------------------------- */
  void CLCheck(void)
    { const LockGuard lgGuardCheck{ CollectorGetMutex() };
      this->CLBaseCheckUnsafe(); }
  /* -- Lock the mutex and return size ------------------------------------- */
  size_t CLCount(void)
    { const LockGuard lgGuardCount{ CollectorGetMutex() };
      return this->CLBaseCountUnsafe(); }
  /* -- Lock the mutex and return newly added object ----------------------- */
  IteratorType CLAdd(MemberType*const mtObj)
    { const LockGuard lgGuardAdd{ CollectorGetMutex() };
      return this->CLBaseAddUnsafe(mtObj); }
  /* -- Lock the mutex and return the removed iterator --------------------- */
  IteratorType CLErase(IteratorType &itObj)
    { const LockGuard lgGuardErase{ CollectorGetMutex() };
      return this->CLBaseEraseUnsafe(itObj); }
  /* -- Constructor -------------------------------------------------------- */
  explicit CLHelperSafe(const char*const cpT) :
    /* -- Initialisers ----------------------------------------------------- */
    BaseType{ cpT }                    // Initialise base type with name
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Destructor --------------------------------------------------------- */
  ~CLHelperSafe(void) { }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(CLHelperSafe)        // Suppress default functions for safety
  /* -- Return the mutex ------------------------------------------- */ public:
  mutex &CollectorGetMutex(void) { return *this; }
  /* -- Lock the mutex and return the removed iterator --------------------- */
  CVarReturn CLSetLimit(const size_t stLimit)
    { const LockGuard lgGuardSetLimit{ CollectorGetMutex() };
      return this->CLBaseSetLimitUnsafe(stLimit); }
};/* ----------------------------------------------------------------------- */
template<class MemberType, class ListType, class IteratorType,
         class BaseType = CLHelperBase<MemberType, ListType, IteratorType>>
class CLHelperUnsafe :                 // Members initially private
  /* -- Base classes ------------------------------------------------------- */
  public BaseType                      // The collector base type
{ /* -- (Un)Lock the mutex (nope) ------------------------------- */ protected:
  void CLLock(void) { }
  void CLUnlock(void) { }
  /* -- Return if collector is empty --------------------------------------- */
  bool CLEmpty(void) { return this->CLBaseIsEmptyUnsafe(); }
  /* -- Check for overflow ------------------------------------------------- */
  void CLCheck(void) { this->CLBaseCheckUnsafe(); }
  /* -- Return number of registered items ---------------------------------- */
  size_t CLCount(void) { return this->CLBaseCountUnsafe(); }
  /* -- Add/remove object to the list -------------------------------------- */
  IteratorType CLAdd(MemberType*const mtObj)
    { return this->CLBaseAddUnsafe(mtObj); }
  IteratorType CLErase(IteratorType &itObj)
    { return this->CLBaseEraseUnsafe(itObj); }
  /* -- Constructor -------------------------------------------------------- */
  explicit CLHelperUnsafe(const char*const cpT) :
    /* -- Initialisers ----------------------------------------------------- */
    BaseType{ cpT }                    // Initialise base type with name
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Destructor --------------------------------------------------------- */
  ~CLHelperUnsafe(void) { }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(CLHelperUnsafe)      // Suppress default functions for safety
  /* -- Set maximum objects ------------------------------------------------ */
  CVarReturn CLSetLimit(const size_t stLimit)
    { return this->CLBaseSetLimitUnsafe(stLimit); }
};/* ----------------------------------------------------------------------- */
template<class MemberType, class LockType, class ListType, class IteratorType>
struct CLHelper :                      // Members initially public
  /* -- Base classes ------------------------------------------------------- */
  public LockType                      // CLHelperSafe or CLHelperUnsafe
{ /* -- (Un)Lock the mutex (nope) ------------------------------------------ */
  void CollectorLock(void) { this->CLLock(); }
  void CollectorUnlock(void) { this->CLUnlock(); }
  /* -- Return last item in list ------------------------------------------- */
  IteratorType CollectorGetLastItemUnsafe(void)
    { return this->CLBaseGetLastItemUnsafe(); }
  /* -- Destory all members ------------------------------------------------ */
  void CollectorDestroyUnsafe(void) { this->CLBaseDestroyUnsafe(); }
  /* -- Return if collector is empty --------------------------------------- */
  bool CollectorEmpty(void) { return this->CLEmpty(); }
  bool CollectorNotEmpty(void) { return !CollectorEmpty(); }
  /* -- Check for overflow ------------------------------------------------- */
  void CollectorCheck(void) { this->CLCheck(); }
  void CollectorCheckUnsafe(void) { this->CLBaseCheckUnsafe(); }
  /* -- Return number of registered items ---------------------------------- */
  size_t CollectorCount(void) { return this->CLCount(); }
  size_t CollectorCountUnsafe(void) { return this->CLBaseCountUnsafe(); }
  /* -- Add/remove object to the list (ICHelper needs these) --------------- */
  IteratorType CollectorAddUnsafe(MemberType*const mtObj)
    { return this->CLBaseAddUnsafe(mtObj); }
  IteratorType CollectorEraseUnsafe(IteratorType &itObj)
    { return this->CLBaseEraseUnsafe(itObj); }
  /* -- Add/remove object to the list (Use these everywhere else) ---------- */
  IteratorType CollectorAdd(MemberType*const mtObj)
    { return this->CLAdd(mtObj); }
  IteratorType CollectorErase(IteratorType &itObj)
    { return this->CLErase(itObj); }
  /* -- Constructor -------------------------------------------------------- */
  explicit CLHelper(const char*const cpT) :
    /* -- Initialisers ----------------------------------------------------- */
    LockType{ cpT }                    // Initialise lock type with name
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Destructor --------------------------------------------------------- */
  ~CLHelper(void) { this->CLBaseCheckAndDestroyUnsafe(); }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(CLHelper)            // Suppress default functions for safety
  /* -- Set maximum objects ------------------------------------------------ */
  CVarReturn CollectorSetLimit(const size_t stLimit)
    { return this->CLSetLimit(stLimit); }
}; /* ---------------------------------------------------------------------- */
/* == MEMBER HELPERS ======================================================= **
** ######################################################################### **
** ## This is the best class for ICHelper. It allows member classes to    ## **
** ## register and unregister themselves in the collector and other       ## **
** ## neat things.                                                        ## **
** ######################################################################### **
** == Parent class helper ================================================== */
template<class CollectorType, class MemberType, class IteratorType>
struct ICHelperBase                    // Members initially public
{ /* -- Swap registration with another class ------------------------------- */
  CollectorType*const cParent;         // Parent class of this object
  /* -- Protected variables ------------------------------------- */ protected:
  IteratorType     cIterator;          // Iterator to this object in parent
  /* -- Add from collector with set parent --------------------------------- */
  void ICHelperBaseRegister(void)
  { // Return if already registered else check count and add to collector
    if(cIterator != cParent->CollectorGetLastItemUnsafe()) return;
    cParent->CollectorCheckUnsafe();
    cIterator = cParent->CollectorAddUnsafe(static_cast<MemberType*>(this));
  }
  /* -- Erase from collector with set parent ------------------------------- */
  void ICHelperBaseUnregister(void)
  { // Return if not registered otherwise unregister from collector
    if(cIterator == cParent->CollectorGetLastItemUnsafe()) return;
    cIterator = cParent->CollectorEraseUnsafe(cIterator);
  }
  /* -- Initialisation helpers on construction ----------------------------- */
  static IteratorType
    ICHelperBaseInit(CollectorType*const ctPtr, MemberType*const mtPtr)
      { return ctPtr->CollectorAddUnsafe(mtPtr); }
  static IteratorType ICHelperBaseInit(CollectorType*const ctPtr)
    { return ctPtr->CollectorGetLastItemUnsafe(); }
   /* -- Swap registration ------------------------------------------------- */
  // We ideally want to just replace the pointers in the iterator without
  // actually disturbing any actual iterators because it will be much faster
  // than just erasing the iterator and readding it! We unfortunately cannot
  // use iter_swap() as it asserts() when it tries to swap an end iterator.
  void ICHelperBaseSwapRegistration(const MemberType &mtObj)
  { // Convert to non-const
    MemberType &mtObjRef = const_cast<MemberType&>(mtObj);
    // If this class is registered?
    if(cIterator != this->cParent->CollectorGetLastItemUnsafe())
    { // If other is registered? Nothing needs swapping because both iterators
      // are already in the collector list
      if(mtObjRef.cIterator != this->cParent->CollectorGetLastItemUnsafe())
      { // Just swap iterators and pointers to the members
        swap(cIterator, mtObjRef.cIterator);
        swap(*addressof(*cIterator), *addressof(*mtObjRef.cIterator));
        // Done
        return;
      } // Other not registered?
      // Copy this iterator over the other iterator because we are using the
      // same physical position in the collector list...
      mtObjRef.cIterator = cIterator;
      // ...and we just have to replace the pointer to the other class of which
      // is now registered, and not this.
      *addressof(*mtObjRef.cIterator) = &mtObjRef;
      // We are now unregistered
      cIterator = this->cParent->CollectorGetLastItemUnsafe();
      // Done
      return;
    } // This isn't registered and if other isn't too, just return
    else if(mtObjRef.cIterator ==
      this->cParent->CollectorGetLastItemUnsafe()) return;
    // Copy other iterator over this iterator because we are using the same
    // physical position in the collector list...
    cIterator = mtObjRef.cIterator;
    // ...and we just have to replace the pointer to the other class with the
    // pointer to this class.
    *addressof(*cIterator) = static_cast<MemberType*>(this);
    // Other member no longer registered. Any use of the iterator in the other
    // class is now undefined behaviour.
    mtObjRef.cIterator = this->cParent->CollectorGetLastItemUnsafe();
  }
  /* ----------------------------------------------------------------------- */
  explicit ICHelperBase(CollectorType*const ctPtr, IteratorType &&itObj) :
    /* -- Initialisers ----------------------------------------------------- */
    cParent{ ctPtr },                  // Initialise pointer to collector class
    cIterator{ StdMove(itObj) }        // Initialise iterator
    /* -- No code ---------------------------------------------------------- */
    { }
  /* ----------------------------------------------------------------------- */
  ~ICHelperBase(void) { }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(ICHelperBase)        // Suppress default functions for safety
};/* ----------------------------------------------------------------------- */
/* == Collector class helper WITH locks ==================================== **
** ######################################################################### **
** ## Derive this class into the IcHelper (main collector) class to make  ## **
** ## use the collector with concurrency locking. Only the 'Sources'      ## **
** ## objects need this because the list can be accessed and manipulated  ## **
** ## in the audio processing thread.                                     ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
template<class CollectorType, class MemberType, class IteratorType,
  class BaseType = ICHelperBase<CollectorType, MemberType, IteratorType>>
class ICHelperSafe :                   // Members initially private
  /* -- Base classes ------------------------------------------------------- */
  public BaseType                      // ICHelper base class
{ /* -- Initialise (un)registered entry with synchronisation --------------- */
  IteratorType ICHelperInit(CollectorType*const ctPtr,
    MemberType*const mtPtr) const
      { const LockGuard lgInitRegistered{ ctPtr->CollectorGetMutex() };
        return this->ICHelperBaseInit(ctPtr, mtPtr); }
  IteratorType ICHelperInit(CollectorType*const ctPtr) const
    { const LockGuard lgInitUnregistered{ ctPtr->CollectorGetMutex() };
      return this->ICHelperBaseInit(ctPtr); }
  /* -- Insert/Remove from collector list with synchronisation -- */ protected:
  void ICHelperPush(void)
    { const LockGuard lgPush{ this->cParent->CollectorGetMutex() };
      this->ICHelperBaseRegister(); }
  void ICHelperErase(void)
    { const LockGuard lgErase{ this->cParent->CollectorGetMutex() };
      this->ICHelperBaseUnregister(); }
  /* -- Swap to objects ---------------------------------------------------- */
  void ICHelperSwap(const MemberType &mtObj)
    { const LockGuard lgSwap{ this->cParent->CollectorGetMutex() };
      this->ICHelperBaseSwapRegistration(mtObj); }
  /* -- Constructors ------------------------------------------------------- */
  explicit ICHelperSafe(CollectorType*const ctPtr) :
    BaseType(ctPtr, StdMove(ICHelperInit(ctPtr))) { }
  explicit ICHelperSafe(CollectorType*const ctPtr, MemberType*const mtPtr) :
    BaseType(ctPtr, StdMove(ICHelperInit(ctPtr, mtPtr))) { }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(ICHelperSafe)        // Suppress default functions for safety
};/* ----------------------------------------------------------------------- */
/* == Collector class helper without locks ================================= **
** ######################################################################### **
** ## Derive this class into the IcHelper (main collector) class to make  ## **
** ## use the collector without concurrency locking. Use this only if you ## **
** ## know the objects are not going to modify or access the list in a    ## **
** ## thread other than the engine thread.                                ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
template<class CollectorType, class MemberType, class IteratorType,
  class BaseType = ICHelperBase<CollectorType, MemberType, IteratorType>>
class ICHelperUnsafe :                 // Members initially private
  /* -- Base classes ------------------------------------------------------- */
  public BaseType                      // ICHelper base class
{ /* -- Initialise (un)registered entry without synchronisation ------------ */
  IteratorType ICHelperInit(CollectorType*const ctPtr,
    MemberType*const mtPtr) const
      { return this->ICHelperBaseInit(ctPtr, mtPtr); }
  IteratorType ICHelperInit(CollectorType*const ctPtr) const
    { return this->ICHelperBaseInit(ctPtr); }
  /* -- Push/Remove object into collector list ------------------ */ protected:
  void ICHelperPush(void) { this->ICHelperBaseRegister(); }
  void ICHelperErase(void) { this->ICHelperBaseUnregister(); }
  /* -- Remove object from collector list ---------------------------------- */
  void ICHelperSwap(const MemberType &mtObj)
    { this->ICHelperBaseSwapRegistration(mtObj); }
  /* -- Constructors without registration ---------------------------------- */
  explicit ICHelperUnsafe(CollectorType*const ctPtr) :
    /* -- Initialisers ----------------------------------------------------- */
    BaseType{ ctPtr, StdMove(ICHelperInit(ctPtr)) }
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Constructor with registration -------------------------------------- */
  explicit ICHelperUnsafe(CollectorType*const ctPtr, MemberType*const mtPtr) :
    /* -- Initialisers ----------------------------------------------------- */
    BaseType{ ctPtr, StdMove(ICHelperInit(ctPtr, mtPtr)) }
    /* -- No code ---------------------------------------------------------- */
    { }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(ICHelperUnsafe)      // Suppress default functions for safety
};/* ----------------------------------------------------------------------- */
/* == Collector class ====================================================== **
** ######################################################################### **
** ## This collector helper class can manually or automate class          ## **
** ## registration of members in the collector class. It will store a     ## **
** ## list::iterator of itself so it can be removed from the collector    ## **
** ## as fast as possible. Also members can freely use 'cParent' to       ## **
** ## access the collector.                                               ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
template<class CollectorType, class MemberType, class LockType,
  class IteratorType>
struct ICHelper :                      // Members initially public
  /* -- Base classes ------------------------------------------------------- */
  public LockType                      // ICHelperSafe or ICHelperUnSafe
{ /* -- Swap registration with another class ------------------------------- */
  void CollectorSwapRegistration(const MemberType &mtObj)
    { this->ICHelperSwap(mtObj); }
  /* -- Register to list --------------------------------------------------- */
  void CollectorRegister(void) { this->ICHelperPush(); }
  void CollectorUnregister(void) { this->ICHelperErase(); }
  /* -- Destructor (unregister if registered) ------------------------------ */
  ~ICHelper(void) { CollectorUnregister(); }
  /* -- Constructor (move) ------------------------------------------------- */
  explicit ICHelper(ICHelper &&icOther) :
    /* -- Initialisers ----------------------------------------------------- */
    LockType{ StdMove(icOther) }
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Constructor (manual registration) ---------------------------------- */
  explicit ICHelper(CollectorType*const ctPtr) :
    /* -- Initialisers ----------------------------------------------------- */
    LockType{ ctPtr }
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Constructor with automatic registration ---------------------------- */
  explicit ICHelper(
    /* -- Parameters ------------------------------------------------------- */
    CollectorType*const ctPtr,         // Pointer to collector class
    MemberType*const mtPtr             // Pointer to member class
    ): /* -- Initialisers -------------------------------------------------- */
    LockType{ ctPtr, mtPtr }           // Initialise lock type
    /* -- No code ---------------------------------------------------------- */
    { }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(ICHelper)            // Suppress default functions for safety
};/* ----------------------------------------------------------------------- */
/* == Lua userdata helper ================================================== **
** ######################################################################### **
** ## This class stores pointers to classes which are used by LUA and can ## **
** ## locked by the engine so LUA cannot automatically delete them during ## **
** ## garbage collection.                                                 ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
class Lockable                         // Members initially private
{ /* -- Private variables -------------------------------------------------- */
  bool             bLocked;            // Class is locked from being dealloced
  /* -- Set locked status -------------------------------------------------- */
  void LockSet(const bool bState) { bLocked = bState; }
  /* -- Return true if lock is (not) set --------------------------- */ public:
  bool LockIsSet(void) const { return bLocked; }
  bool LockIsNotSet(void) const { return !LockIsSet(); }
  /* -- Set/clear locked status -------------------------------------------- */
  void LockSet(void) { LockSet(true); }
  void LockClear(void) { LockSet(false); }
  /* -- Swap lock status with another class -------------------------------- */
  void LockSwap(Lockable &lOther) { swap(bLocked, lOther.bLocked); }
  /* -- Constuctor --------------------------------------------------------- */
  explicit Lockable(                   // Initialise with lock (def: false)
    /* -- Parameters ------------------------------------------------------- */
    const bool bState=false            // The user requested lock state
    ): /* -- Initialisers -------------------------------------------------- */
    bLocked(bState)                    // Set the initial lock state
    /* -- No code ---------------------------------------------------------- */
    { }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(Lockable)            // Suppress default functions for safety
};/* -- End ---------------------------------------------------------------- */
};                                     // End of private module namespace
/* ------------------------------------------------------------------------- */
};                                     // End of public module namespace
/* == EoF =========================================================== EoF == */
