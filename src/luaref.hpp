/* == LUAREF.HPP =========================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## This class manages a user specified number of Lua references        ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace ILuaRef {                    // Start of private module namespace
/* ------------------------------------------------------------------------- */
using namespace ILuaUtil::P;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* ------------------------------------------------------------------------- */
template<size_t Refs=1>class LuaRef    // Lua easy reference class
{ /* -- Private typedefs ---------------------------------------- */ protected:
  typedef array<int, Refs> References; // Type for LUA references list
  typedef References::const_reverse_iterator ReferencesConstRevIt;
  typedef References::reverse_iterator       ReferencesRevIt;
  /* -- Protected variables ------------------------------------- */ protected:
  lua_State       *lsState;            // State that owns the reference.
  References       aReferences;        // Reference that the state refers to.
  /* -- Release a specific reference ------------------------------ */ private:
  bool LuaRefDoDeInit(const int iReference) const
  { // Return failure if not valid else remove it and return success
    if(LuaUtilIsNotRefValid(iReference)) return false;
    LuaUtilRmRef(LuaRefGetState(), iReference);
    return true;
  }
  /* -- Release and reset specific reference ------------------------------- */
  void LuaRefDoDeInitReset(int &iReference)
    { if(LuaRefDoDeInit(iReference)) iReference = LUA_REFNIL; }
  /* -- Initialise a specific reference ------------------------------------ */
  bool LuaRefDoInitEx(const size_t stIndex)
  { // Init the ref and return on failure else assign to the specifide ref ndx
    const int iReference = LuaUtilRefInit(LuaRefGetState());
    if(LuaUtilIsNotRefValid(iReference)) return false;
    aReferences[stIndex] = iReference;
    return true;
  }
  /* -- Set new state ------------------------------------------------------ */
  void LuaRefSetState(lua_State*const lS=nullptr) { lsState = lS; }
  /* -- Get the state ---------------------------------------------- */ public:
  lua_State *LuaRefGetState(void) const { return lsState; }
  /* -- Returns the reference at the specified index ----------------------- */
  int LuaRefGetId(const size_t stIndex=0) const
    { return aReferences[stIndex]; }
  /* -- Returns the function at the specified index ------------------------ */
  bool LuaRefGetFunc(const size_t stIndex=0) const
    { return LuaUtilGetRefFunc(LuaRefGetState(), LuaRefGetId(stIndex)); }
  /* -- Returns the userdata at the specified index ------------------------ */
  bool LuaRefGetUData(const size_t stIndex=0) const
    { return LuaUtilGetRefUsrData(LuaRefGetState(), LuaRefGetId(stIndex)); }
  /* -- Returns the reference at the specified index ----------------------- */
  void LuaRefGet(const size_t stIndex=0) const
    { return LuaUtilGetRef(LuaRefGetState(), LuaRefGetId(stIndex)); }
  /* -- Returns if the state is equal to the specified state --------------- */
  bool LuaRefStateIsEqual(const lua_State*const lS) const
    { return LuaRefGetState() == lS; }
  /* -- Returns if the state is NOT equal to the specified state ----------- */
  bool LuaRefStateIsNotEqual(const lua_State*const lS) const
    { return !LuaRefStateIsEqual(lS); }
  /* -- Returns if the state is set ---------------------------------------- */
  bool LuaRefStateIsSet(void) const { return LuaRefStateIsNotEqual(nullptr); }
  /* -- Returns if the state is NOT set ------------------------------------ */
  bool LuaRefStateIsNotSet(void) const { return !LuaRefStateIsSet(); }
  /* -- Returns if the specified reference is set -------------------------- */
  bool LuaRefIsSet(const size_t stIndex=0) const
    { return LuaRefStateIsSet() && LuaUtilIsRefValid(LuaRefGetId(stIndex)); }
  /* -- De-initialise the reference ---------------------------------------- */
  bool LuaRefDeInit(void)
  { // Return if theres a state?
    if(LuaRefStateIsNotSet()) return false;
    // Unload and clear references from back to front
    for(ReferencesRevIt rriIt{ aReferences.rbegin() };
                        rriIt != aReferences.rend();
                      ++rriIt)
      LuaRefDoDeInitReset(*rriIt);
    // Clear the state
    LuaRefSetState();
    // Success
    return true;
  }
  /* -- Initialise the reference ------------------------------------------- */
  bool LuaRefInitEx(const size_t stIndex)
  { // Failed if no state
    if(LuaRefStateIsNotSet()) return false;
    // Deinit existing reference
    LuaRefDoDeInitReset(aReferences[stIndex]);
    // Reference the specified state
    return LuaRefDoInitEx(stIndex);
  }
  /* -- Initialise the reference ------------------------------------------- */
  bool LuaRefInit(lua_State*const lS, const size_t stMax = Refs)
  { // De-init any existing reference first
    LuaRefDeInit();
    // Failed if no state
    if(!lS) return false;
    // Set the state
    LuaRefSetState(lS);
    // Initialise references
    for(size_t stIndex = 0; stIndex < stMax; ++stIndex)
      LuaRefDoInitEx(stIndex);
    // Return success
    return true;
  }
  /* -- Constructor that does nothing but pre-initialise references -------- */
  LuaRef(void) :
    /* -- Initialisers ----------------------------------------------------- */
    lsState(nullptr)                   // State not initialised yet
    /* -- Uninitialised lua reverences ------------------------------------- */
    { aReferences.fill(LUA_REFNIL); }
  /* -- Destructor, delete the reference if set----------------------------- */
  ~LuaRef(void)
  { // If theres a state? Delete references back to front
    if(LuaRefStateIsSet())
      for(ReferencesConstRevIt rcriIt{ aReferences.rbegin() };
                               rcriIt != aReferences.rend();
                             ++rcriIt)
        LuaRefDoDeInit(*rcriIt);
  }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(LuaRef)              // Suppress default functions for safety
};/* ----------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
