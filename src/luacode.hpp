/* == LUACODE.HPP ========================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## This file defines the execution and caching system of lua code      ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace ILuaCode {                   // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IAsset::P;             using namespace IClock::P;
using namespace ICredit::P;            using namespace ICVarDef::P;
using namespace IError::P;             using namespace IFileMap::P;
using namespace ILog::P;               using namespace ILuaUtil::P;
using namespace IMemory::P;            using namespace ISql::P;
using namespace IStd::P;               using namespace IString::P;
using namespace Lib::OS::SevenZip;     using namespace Lib::Sqlite;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* -- Consts --------------------------------------------------------------- */
static enum LuaCache : unsigned int    // User cache setting
{ /* ----------------------------------------------------------------------- */
  LCC_OFF,                             // No code caching
  LCC_FULL,                            // Cache with full debug information
  LCC_MINIMUM,                         // Cache with minimum debug information
  /* ----------------------------------------------------------------------- */
  LCC_MAX                              // Maximum number of settings
} /* ----------------------------------------------------------------------- */
lcSetting;                             // Initialised by CVar later
/* ------------------------------------------------------------------------- */
enum LuaCompResult : unsigned int      // Cache and compilation results
{ /* ----------------------------------------------------------------------- */
  LCR_CACHED,                          // [0] Using cached version
  LCR_RECOMPILE,                       // [1] Code compiled and stored
  LCR_DBERR,                           // [2] Code compiled but not stored
  LCR_NOCACHE,                         // [3] Code compiled cache disabled
  /* ----------------------------------------------------------------------- */
  LCR_MAX,                             // [6] Number of used result codes
};/* ----------------------------------------------------------------------- */
/* -- Set lua cache setting ------------------------------------------------ */
static CVarReturn LuaCodeSetCache(const LuaCache lcVal)
  { return CVarSimpleSetIntNGE(lcSetting, lcVal, LCC_MAX); }
/* -- Check lua version ---------------------------------------------------- */
static CVarReturn LuaCodeCheckVersion(const string &strVal, string &strNVal)
{ // Get current LUA version
  const string_view &svVersion = cCredits->CreditGetItem(CL_LUA).GetVersion();
  // Is version the same? Then we're good
  if(strVal == svVersion) return ACCEPT;
  // Log that the LUA version is different
  cLog->LogWarningExSafe("LuaCode detected LUA version mismatch ($ != $) so "
    "the code cache will be flushed.", strVal, svVersion);
  // Clear the cache and if succeeded?
  if(cSql->FlushTable(cSql->strvLCTable) == SQLITE_OK)
  { // Write success in the console
    cLog->LogWarningSafe("LuaCode flushed the LUA code cache successfully!");
    // Update cvar to the current version
    strNVal = svVersion;
  } // Failed? Write reason to console
  else cLog->LogWarningExSafe("LuaCode failed to flush the LUA code cache "
    "because $ ($)!", cSql->GetErrorStr(), cSql->GetError());
  // Accepted
  return ACCEPT;
}
/* -- Callback for lua_dump ------------------------------------------------ */
namespace LuaCodeDumpHelper
{ /* -- Memory blocks structure for dump function -------------------------- */
  struct MemData { MemoryList mlBlocks; size_t stTotal; };
  /* -- The callback function ---------------------------------------------- */
  static int PopulateMemoryListCallback(lua_State*const,
    const void*const vpAddr, const size_t stSize, void*const vpUser)
  { // Get memory data
    MemData &mdData = *reinterpret_cast<MemData*>(vpUser);
    // Make a new memory block
    mdData.mlBlocks.push_back({ stSize, vpAddr });
    // Add to size total
    mdData.stTotal += stSize;
    // Return success
    return 0;
  }
};/* -- Compile a function to binary --------------------------------------- */
static Memory LuaCodeCompileFunction(lua_State*const lS, const bool bDebug)
{ // Include utility namespace
  using namespace LuaCodeDumpHelper;
  MemData mdData{ {}, 0 };
  // Dump the code to binary and if error occured?
  if(lua_dump(lS, PopulateMemoryListCallback, &mdData, bDebug ? 0 : 1))
    XC("Failure dumping function!");
  // Error if no blocks
  if(mdData.mlBlocks.empty() || !mdData.stTotal) XC("Empty function dump!");
  // If there is only one block?
  if(mdData.mlBlocks.size() == 1)
  { // Get first memory block and if position not reached? Thats not right!
    Memory &mbData = *mdData.mlBlocks.begin();
    if(mdData.stTotal != mbData.MemSize())
      XC("Not enough bytes written to binary!",
        "Written", mbData.MemSize(), "Needed", mdData.stTotal);
    // Return memory block
    return StdMove(mbData);
  } // Make full memory block
  Memory mbData{ mdData.stTotal };
  // Position to write to
  size_t stPos = 0;
  // Until no blocks level or position reaches end
  while(!mdData.mlBlocks.empty() && stPos < mdData.stTotal)
  { // Get memory block and write it to our big final black
    const Memory &mbBlock = *mdData.mlBlocks.begin();
    mbData.MemWriteBlock(stPos, mbBlock);
    // Incrememnt position and erase the block
    stPos += mbBlock.MemSize();
    mdData.mlBlocks.erase(mdData.mlBlocks.cbegin());
  } // Error if position not reached
  if(stPos != mdData.stTotal)
    XC("Not enough bytes written to binary!",
      "Written", stPos, "Needed", mdData.stTotal,
      "Remain", mdData.mlBlocks.size());
  // Return compiled memory
  return mbData;
}
/* -- Copmile function to binary ------------------------------------------- */
static void LuaCodeCompileFunction(lua_State*const lS)
{ // Must have two parameters
  LuaUtilCheckParams(lS, 2);
  // Debug parameter
  const bool bDebug = LuaUtilGetBool(lS, 1);
  // Second parameter must be function
  LuaUtilCheckFunc(lS, 2);
  // Compile the function
  Memory mbData{ LuaCodeCompileFunction(lS, bDebug) };
  // Return a newly created asset
  LuaUtilClassCreate<Asset>(lS, *cAssets)->MemSwap(mbData);
}
/* -- Compile a buffer ----------------------------------------------------- */
static void LuaCodeDoCompileBuffer(lua_State*const lS, const char *cpBuf,
  size_t stSize, const string &strRef)
{ // Compile the specified script and capture result
  switch(const int iR = luaL_loadbuffer(lS, cpBuf, stSize, strRef.c_str()))
  { // No error? Execute functions and log success. We should always be in
    // the sandbox, so no pcall is needed.
    case LUA_OK: return;
    // Syntax error? Show error
    case LUA_ERRSYNTAX:
      XC(StrAppend("Compile error! > ", LuaUtilGetAndPopStr(lS)));
    // Not enough memory?
    case LUA_ERRMEM: XC("Not enough memory executing script!");
    // Unknown error (never get here, but only to stop compiler whining)
    default: XC("Unknown error executing script!", "Result", iR);
  }
}
/* -- Compile a buffer ----------------------------------------------------- */
static LuaCompResult LuaCodeCompileBuffer(lua_State*const lS,
  const char*const cpBuf, const size_t stSize, const string &strRef)
{ // If lua caching is disabled or the buffer is binary or no reference given?
  if(lcSetting == LCC_OFF || (stSize >= sizeof(uint32_t) &&
    *reinterpret_cast<const uint32_t*>(cpBuf) == 0x61754C1B) ||
    strRef.empty() || strRef.front() == '!')
  { // Do compile the buffer and return
    LuaCodeDoCompileBuffer(lS, cpBuf, stSize, strRef);
    // Return success but cache disabled
    return LCR_NOCACHE;
  } // Get checksum of module
  const unsigned int uiCRC = CrcCalc(cpBuf, stSize);
  // Check if we have cached this in the sql database and if we have?
  if(cSql->ExecuteAndSuccess(StrFormat("SELECT `$` from `$` WHERE R=? AND C=?",
    cSql->strvLCCodeColumn, cSql->strvLCTable), strRef, uiCRC))
  { // Get records and if we have results?
    const SqlResult &srData = cSql->GetRecords();
    if(!srData.empty())
    { // If we should show the rows affected. This is sloppy but sqllite
      // doesn't support resetting sqlite3_changes result yet :(
      const SqlRecordsMap &srmRef = *srData.cbegin();
      const SqlRecordsMapConstIt srmciIt{ srmRef.find("D") };
      if(srmciIt != srmRef.cend())
      { // Get value and if its a blob? Set new buffer to load
        const SqlData &sdRef = srmciIt->second;
        if(sdRef.iType == SQLITE_BLOB)
        { // Cache is valid
          cLog->LogDebugExSafe(
            "LuaCode will use cached version of '$'[$]($$)!",
              strRef, stSize, hex, uiCRC);
          // Do compile the buffer
          LuaCodeDoCompileBuffer(lS,
            sdRef.MemPtr<char>(), sdRef.MemSize(), strRef);
          // Return that we used the cached version
          return LCR_CACHED;
        } // Invalid type
        else cLog->LogWarningExSafe(
          "LuaCode will recompile '$'[$]($$$) as it has a bad type of $!",
            strRef, stSize, hex, uiCRC, dec, sdRef.iType);
      } // Invalid keyname?
      else cLog->LogWarningExSafe(
        "LuaCode will recompile '$'[$]($$) as it has a bad keyname!",
        strRef, stSize, hex, uiCRC);
    } // No results
    else cLog->LogDebugExSafe(
      "LuaCode will recompile '$'[$]($$) as the module was modified!",
        strRef, stSize, hex, uiCRC);
  } // Error reading database so try to rebuild table
  else cSql->LuaCacheRebuildTable();
  // Do compile the buffer
  LuaCodeDoCompileBuffer(lS, cpBuf, stSize, strRef);
  // Compile the function
  Memory mbData{ LuaCodeCompileFunction(lS, lcSetting == LCC_FULL) };
  // Send to sql database and return if succeeded
  if(cSql->ExecuteAndSuccess(StrFormat(
         "INSERT or REPLACE into `$`(`$`,`$`,`$`,`$`) VALUES(?,?,?,?)",
         cSql->strvLCTable, cSql->strvLCCRCColumn, cSql->strvLCTimeColumn,
         cSql->strvLCRefColumn, cSql->strvLCCodeColumn),
       uiCRC, cmSys.GetTimeNS<sqlite3_int64>(), strRef, mbData))
    return LCR_RECOMPILE;
  // Show error
  cLog->LogWarningExSafe(
    "LuaCode failed to store cache for '$' because $ ($)!",
    strRef, cSql->GetErrorStr(), cSql->GetError());
  // Try to rebuild table
  cSql->LuaCacheRebuildTable();
  // Return compiled but not stored
  return LCR_DBERR;
}
/* -- Compile a string ----------------------------------------------------- */
static LuaCompResult LuaCodeCompileString(lua_State*const lS,
  const string &strBuf, const string &strRef)
    { return LuaCodeCompileBuffer(lS,
        strBuf.data(), strBuf.length(), strRef); }
/* -- Executes the function and returns the compilation result ------------- */
static LuaCompResult LuaCodeExecCallRet(lua_State*const lS,
  const LuaCompResult lcrRes, const int iRet)
    { LuaUtilCallFunc(lS, iRet); return lcrRes; }
/* -- Compile a memory block ----------------------------------------------- */
static LuaCompResult LuaCodeCompileBlock(lua_State*const lS,
  const MemConst &mcSrc, const string &strRef)
    { return LuaCodeCompileBuffer(lS,
        mcSrc.MemPtr<char>(), mcSrc.MemSize(), strRef); }
/* -- Execute specified block ---------------------------------------------- */
static LuaCompResult LuaCodeExecuteBlock(lua_State*const lS,
  const MemConst &mcSrc, const int iRet, const string &strRef)
    { return LuaCodeExecCallRet(lS,
        LuaCodeCompileBlock(lS, mcSrc, strRef), iRet); }
/* -- Execute specified string in unprotected ------------------------------ */
static LuaCompResult LuaCodeExecuteString(lua_State*const lS,
  const string &strCode, const int iRet, const string &strRef)
    { return LuaCodeExecCallRet(lS,
        LuaCodeCompileString(lS, strCode, strRef), iRet); }
/* -- Compile contents of a file (returns function on lua stack) ----------- */
static LuaCompResult LuaCodeCompileFile(lua_State*const lS,
  const FileMap &fScript)
    { return LuaCodeCompileBuffer(lS, fScript.MemPtr<char>(),
        fScript.MemSize(), fScript.IdentGetCStr()); }
/* -- Copmile file and execute script that may be binary ------------------- */
static LuaCompResult LuaCodeCompileFile(lua_State*const lS,
  const string &strFilename)
    { return LuaCodeCompileFile(lS, AssetExtract(strFilename)); }
/* -- Load file and execute script that may be binary ---------------------- */
static LuaCompResult LuaCodeExecuteFile(lua_State*const lS,
  const string &strFilename, const int iRet=0)
    { return LuaCodeExecCallRet(lS, LuaCodeCompileFile(lS, strFilename),
        iRet); }
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
