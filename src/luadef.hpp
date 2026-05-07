/* == LUADEF.HPP =========================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## Some definitions specific to Lua.                                   ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* == LuaDef interface namespace =========================================== */
namespace ILuaDef {                    // Start of module namespace
/* == Typedefs ============================================================= */
struct LuaKeyInt                       // Lua key/value pairs C
{ /* ----------------------------------------------------------------------- */
  const StdStringView strvName;        // Name of const table
  const lua_Integer   liValue;         // Integer value for this const
};/* ----------------------------------------------------------------------- */
using LuaKeyIntSpan = StdSpan<const LuaKeyInt>; // Refs array of LuaKeyInts
/* ------------------------------------------------------------------------- */
struct LuaTable                        // Lua table as C
{ /* ----------------------------------------------------------------------- */
  const char*const      cpName;        // Name of const table
  const LuaKeyIntSpan   lkisList;      // Key value list
};/* ----------------------------------------------------------------------- */
using LuaTableSpan = StdSpan<const LuaTable>; // Refs array of LuaTables
/* ------------------------------------------------------------------------- */
};                                     // End of module namespace
/* == LibLua interface namespace =========================================== */
namespace ILuaLib {                    // Start of private module namespace
/* ------------------------------------------------------------------------- */
using namespace ICVarDef::P;           using namespace IFillCon::P;
using namespace ILuaDef;               using namespace IUtil::P;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* -- Lua API class namespace ids ------------------------------------------ */
enum LuaClassId : size_t {
  /* ----------------------------------------------------------------------- */
  LMT_ARCHIVE, LMT_ASSET,    LMT_ATLAS,   LMT_BIN,     LMT_CLIP,       // 00-04
  LMT_COMMAND, LMT_FBO,      LMT_FILE,    LMT_FONT,    LMT_FTF,        // 05-09
  LMT_IMAGE,   LMT_IMAGELIB, LMT_JSON,    LMT_LUAFUNC, LMT_MASK,       // 10-14
  LMT_PALETTE, LMT_PCM,      LMT_PCMLIB,  LMT_SAMPLE,  LMT_SHADER,     // 15-19
  LMT_SOCKET,  LMT_SOURCE,   LMT_SQL,     LMT_SSHOT,   LMT_STAT,       // 20-24
  LMT_STREAM,  LMT_THREAD,   LMT_TEXTURE, LMT_URL,     LMT_VARIABLE,   // 25-39
  LMT_VIDEO,                                                           // 40
  /* ----------------------------------------------------------------------- */
  LMT_TOTAL                            // Maximum number of classes
};/* -- LUA class reference ids (ref'd in luaident.hpp, lua.hpp) ----------- */
using LuaLibClassIdReferences = StdArray<int, LMT_TOTAL>;
static LuaLibClassIdReferences llcirAPI
  { FillConGeneric<LuaLibClassIdReferences>(LUA_REFNIL) };
/* -- Information about a LUA API namespace -------------------------------- */
struct LuaLibStatic
{ /* ----------------------------------------------------------------------- */
  const LuaClassId     lciId;          // Unique class id (see above)
  const StdStringView &strvName;       // Name of library
  const CoreFlagsConst cfcRequired;    // Required core flags to register
  const luaL_Reg*const libList;        // Library functions
  const size_t         stLLCount;      // Size of library functions
  const luaL_Reg*const libmfList;      // Member library functions
  const size_t         stLLMFCount;    // Size of member library functions
  const lua_CFunction  lcfpDestroy;    // Destruction function
  const LuaTableSpan   ltsList;        // Table of const key/values to define
  const size_t         stLLTotal;      // Total number of entries
};/* -- Lua API namespace descriptor list (ref'd in collect, lua, lualib) -- */
using LuaLibStaticArray = StdArray<const LuaLibStatic, LMT_TOTAL>;
extern const LuaLibStaticArray llsaAPI;
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
