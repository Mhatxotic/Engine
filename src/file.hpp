/* == FILE.HPP ============================================================= **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## This module is a simple C++ wrapper for C file stream functions.    ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IFile {                      // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IAsset::P;             using namespace ICollector::P;
using namespace IFStream::P;           using namespace IIdent::P;
using namespace ILuaLib::P;            using namespace ILuaUtil::P;
using namespace IMemory::P;            using namespace IStd::P;
using namespace IString::P;            using namespace ISysUtil::P;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* -- File collector and member class -------------------------------------- */
CTOR_BEGIN_DUO(Files, File, CLHelperUnsafe, ICHelperUnsafe),
  /* -- Base classes ------------------------------------------------------- */
  public FStream,                      // File stream class
  public Lockable                      // Lua garbage collector instruction
{ /* -- Basic constructor with no init ----------------------------- */ public:
  File(void) :
    /* -- Initialisers ----------------------------------------------------- */
    ICHelperFile{ cFiles, this },      // Register the object in collector
    IdentCSlave{ cParent->CtrNext() }  // Initialise identification number
    /* --------------------------------------------------------------------- */
    { }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(File)                // Suppress default functions for safety
};/* ----------------------------------------------------------------------- */
CTOR_END_NOINITS(Files, File, FILE)    // Finish global Files collector
/* -- Read string to file in one go ---------------------------------------- */
static void FileReadString(lua_State*const lS)
{ // Open file in read text mode
  if(FStream fFile{ LuaUtilGetCppFile(lS, 1), FM_R_T })
  { // Read file and store in string
    const string strData{ fFile.FStreamReadStringSafe() };
    // If no error occured then return the data read
    if(!fFile.FStreamFError())
    { // Add the string that was read and return a nil for no error
      LuaUtilPushStr(lS, strData);
      return LuaUtilPushNil(lS);
    } // Error occured so return error message
    LuaUtilPushBool(lS, false);
    LuaUtilPushStr(lS, fFile.FStreamGetErrStr());
  } // Open failed?
  else
  { // Return error message
    LuaUtilPushBool(lS, false);
    LuaUtilPushStr(lS, fFile.FStreamGetErrStr());
  }
}
/* -- Write data a file in one go ------------------------------------------ */
static void FileWriteData(lua_State*const lS, const string &strFile,
  const FStreamMode fmMode, const void*const vpPtr, const size_t stBytes)
{ // Open file with the specified mode and if successful?
  if(FStream fFile{ strFile, fmMode })
  { // Success if zero size
    if(!stBytes) return LuaUtilPushBool(lS, true);
    // Write the data and check if we wrote enough data?
    const size_t stWritten = fFile.FStreamWrite(vpPtr, stBytes);
    if(stWritten == stBytes) return LuaUtilPushInt(lS, stWritten);
    // Error occured so return failure with error message
    LuaUtilPushBool(lS, false);
    LuaUtilPushStr(lS, fFile.FStreamFError() ? fFile.FStreamGetErrStr() :
      StrFormat("Only $ of $ bytes written", stWritten, stBytes));
  } // Failed so return reason
  else
  { // Error occured so return failure with error message
    LuaUtilPushBool(lS, false);
    LuaUtilPushStr(lS, fFile.FStreamGetErrStr());
  }
}
/* -- Append string to file in one go -------------------------------------- */
static void FileAppendString(lua_State*const lS)
{ // Get filename, string to write from Lua and send it to writer function
  const string strFile{ LuaUtilGetCppFile(lS, 1) },
               strWhat{ LuaUtilGetCppStr(lS, 2) };
  FileWriteData(lS, strFile, FM_A_B, strWhat.data(), strWhat.length());
}
/* -- Write string to file in one go --------------------------------------- */
static void FileWriteString(lua_State*const lS)
{ // Get filename, string to write from Lua and send it to writer function
  const string strFile{ LuaUtilGetCppFile(lS, 1) },
               strWhat{ LuaUtilGetCppStr(lS, 2) };
  FileWriteData(lS, strFile, FM_W_B, strWhat.data(), strWhat.length());
}
/* -- Append data to file in one go ---------------------------------------- */
static void FileAppendBlock(lua_State*const lS)
{ // Get filename, string to write from Lua and send it to writer function
  const string strFile{ LuaUtilGetCppFile(lS, 1) };
  const MemConst &mcSrc = *LuaUtilGetPtr<Asset>(lS, 2, *cAssets);
  FileWriteData(lS, strFile, FM_A_B, mcSrc.MemPtr(), mcSrc.MemSize());
}
/* -- Write data to file in one go ----------------------------------------- */
static void FileWriteBlock(lua_State*const lS)
{ // Get filename, string to write from Lua and send it to writer function
  const string strFile{ LuaUtilGetCppFile(lS, 1) };
  const MemConst &mcSrc = *LuaUtilGetPtr<Asset>(lS, 2, *cAssets);
  FileWriteData(lS, strFile, FM_W_B, mcSrc.MemPtr(), mcSrc.MemSize());
}
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
