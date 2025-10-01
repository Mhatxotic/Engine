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
  const char*const  cpName;            // Name of const table
  const lua_Integer liValue;           // Integer value for this const
};/* ----------------------------------------------------------------------- */
struct LuaTable                        // Lua table as C
{ /* ----------------------------------------------------------------------- */
  const char*const      cpName;        // Name of const table
  const LuaKeyInt*const kiList;        // Key value list
  const int             iCount;        // Number of items in this list
};/* ----------------------------------------------------------------------- */
};                                     // End of module namespace
/* == LibLua interface namespace =========================================== */
namespace ILuaLib {                    // Start of private module namespace
/* ------------------------------------------------------------------------- */
using namespace ICVarDef::P;           using namespace ILuaDef;
using namespace IUtil::P;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* -- Lua API class namespace ids ------------------------------------------ */
enum LuaClassId : size_t {
  /* ----------------------------------------------------------------------- */
  LMT_ARCHIVE,  LMT_ASSET,    LMT_ATLAS,  LMT_BIN,      LMT_CLIP,      // 00-04
  LMT_COMMAND,  LMT_FBO,      LMT_FILE,   LMT_FONT,     LMT_FTF,       // 05-09
  LMT_IMAGE,    LMT_IMAGELIB, LMT_JSON,   LMT_LUAFUNC,  LMT_MASK,      // 10-14
  LMT_PALETTE,  LMT_PCM,      LMT_PCMLIB, LMT_SAMPLE,   LMT_SHADER,    // 15-19
  LMT_SSHOT,    LMT_STAT,     LMT_SOCKET, LMT_SOURCE,   LMT_STREAM,    // 20-24
  LMT_THREAD,   LMT_TEXTURE,  LMT_URL,    LMT_VARIABLE, LMT_VIDEO,     // 25-39
  /* ----------------------------------------------------------------------- */
  LMT_CLASSES,                         // Maximum number of classes
  /* ----------------------------------------------------------------------- */
  LMT_TOTAL = LMT_CLASSES + 1,         // Absolute total namespaces [31]
};/* -- LUA class reference ids (ref'd in luaident.hpp, lua.hpp) ----------- */
typedef array<int, LMT_CLASSES> LuaLibClassIdReferences;
static LuaLibClassIdReferences llcirAPI
  { UtilMkFilledContainer<LuaLibClassIdReferences>(LUA_REFNIL) };
/* -- Information about a LUA API namespace -------------------------------- */
struct LuaLibStatic
{ /* ----------------------------------------------------------------------- */
  const LuaClassId     lciId;          // Unique class id (see above)
  const string_view   &strvName;       // Name of library
  const CoreFlagsConst cfcRequired;    // Required core flags to register
  const luaL_Reg*const libList;        // Library functions
  const int            iLLCount;       // Size of library functions
  const luaL_Reg*const libmfList;      // Member library functions
  const int            iLLMFCount;     // Size of member library functions
  const lua_CFunction  lcfpDestroy;    // Destruction function
  const LuaTable*const lkiList;        // Table of key/values to define
  const int            iLLKICount;     // Size of key/values library functions
  const int            iLLTotal;       // Total number of entries
};/* -- Lua API namespace descriptor list (ref'd in collect, lua, lualib) -- */
typedef array<const LuaLibStatic, LMT_TOTAL> LuaLibStaticArray;
extern const LuaLibStaticArray llsaAPI;
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
