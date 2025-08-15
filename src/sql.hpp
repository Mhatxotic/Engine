/* == SQL.HPP ============================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## This module handles writing and reading of persistant data to a     ## **
** ## database on disk.                                                   ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace ISql {                       // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IAsset::P;             using namespace IClock::P;
using namespace ICmdLine::P;           using namespace ICommon::P;
using namespace ICrypt::P;             using namespace ICVarDef::P;
using namespace IDir::P;               using namespace IError::P;
using namespace IFlags;                using namespace IIdent::P;
using namespace ILog::P;               using namespace ILuaUtil::P;
using namespace IPSplit::P;            using namespace IMemory::P;
using namespace IStd::P;               using namespace IString::P;
using namespace ISystem::P;            using namespace ISysUtil::P;
using namespace ITimer::P;             using namespace IUtil::P;
using namespace Lib::Sqlite;
/* -- Replacement for SQLITE_TRANSIENT which cases warnings ---------------- */
static const sqlite3_destructor_type fcbSqLiteTransient =
  reinterpret_cast<sqlite3_destructor_type>(-1);
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* -- Sql cvar data types -------------------------------------------------- */
BUILD_FLAGS(SqlCVarData,
  /* -- (Note: Don't ever change these around) ----------------------------- */
  // No flags for this cvar in db?     The data value was encrypted?
  SD_NONE                   {Flag(0)}, SD_ENCRYPTED              {Flag(1)}
);/* -- Sql flags ---------------------------------------------------------- */
BUILD_FLAGS(Sql,                       // Sql flags classes
  /* ----------------------------------------------------------------------- */
  // No settings?                      Is temporary database?
  SF_NONE                   {Flag(0)}, SF_ISTEMPDB               {Flag(1)},
  // Delete empty databases?           Debug sql executions?
  SF_DELETEEMPTYDB          {Flag(2)}
);/* ----------------------------------------------------------------------- */
struct SqlData :                        // Query response data item class
  /* -- Base classes ------------------------------------------------------- */
  public Memory                        // Memory block and type
{ /* -- Sql type variable -------------------------------------------------- */
  int              iType;              // Type of memory in block
  /* -- Move assignment operator ------------------------------------------- */
  SqlData &operator=(SqlData &&sdOther)
    { MemSwap(sdOther); iType = sdOther.iType; return *this; }
  /* -- Initialise with rvalue memory and type ----------------------------- */
  SqlData(Memory &&mRef,               // Memory to record data
          const int iNType) :          // Type of the contents in record data
    /* -- Initialisers ----------------------------------------------------- */
    Memory{ StdMove(mRef) },           // Move other memory block other
    iType(iNType)                      // Copy other type over
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Move constructor --------------------------------------------------- */
  SqlData(SqlData &&sdOther) :
    /* -- Initialisers ----------------------------------------------------- */
    SqlData{ StdMove(sdOther), sdOther.iType }
    /* -- No code ---------------------------------------------------------- */
    { }
};/* ----------------------------------------------------------------------- */
MAPPACK_BUILD(SqlRecords, const string, SqlData);
typedef list<SqlRecordsMap> SqlResult; // vector of key/raw data blocks
/* -- Sql manager class ---------------------------------------------------- */
struct Sql;                            // Class prototype
static Sql *cSql = nullptr;            // Pointer to global class
struct Sql :                           // Members initially public
  /* -- Base classes ------------------------------------------------------- */
  public Ident,                        // Sql database filename
  private SqlFlags                     // Sql flags
{ /* -- Typedefs ----------------------------------------------------------- */
  enum ADResult                        // Results for CanDeleteDatabase()
  { /* --------------------------------------------------------------------- */
    ADR_OK_NO_TABLES,                  // No tables exist (delete ok)
    ADR_OK_NO_RECORDS,                 // No records exist (delete ok)
    /* --------------------------------------------------------------------- */
    ADR_ERR,                           // Minimum error value (delete denied)
    /* --------------------------------------------------------------------- */
    ADR_ERR_TEMP_DB = ADR_ERR,         // Is temporary database?
    ADR_ERR_DENY_OPTION,               // Delete not allowed by guest?
    ADR_ERR_LU_TABLE,                  // Error looking up table?
    ADR_ERR_TABLES_EXIST,              // Tables exist?
    ADR_ERR_LU_RECORD,                 // Error looking up records?
    ADR_ERR_RECORDS_EXIST,             // Records exist?
    /* --------------------------------------------------------------------- */
    ADR_MAX                            // Maximum number of result types
  };/* --------------------------------------------------------------------- */
  enum PurgeResult                     // Result to a purge request
  { /* --------------------------------------------------------------------- */
    PR_FAIL,                           // Sql call failed with error
    PR_OK,                             // Sql call succeeded with changes
    PR_OK_NC,                          // Sql call succeeded but no changes
  };/* --------------------------------------------------------------------- */
  enum CreateTableResult               // Create table result
  { /* --------------------------------------------------------------------- */
    CTR_FAIL,                          // Sql create table failed
    CTR_OK,                            // Sql call commited the variable
    CTR_OK_ALREADY,                    // Sql table already exists
  };/* -- Private typedefs ---------------------------------------- */ private:
  typedef IdList<SQLITE_NOTICE> ErrorList; // Sqlite errors strings list
  typedef IdList<ADR_MAX> ADRList;         // AD result strings list
  /* -- Schema version ----------------------------------------------------- */
  static constexpr const sqlite3_int64 qVersion = 1; // Expected schema version
  /* -- Variables ---------------------------------------------------------- */
  const ErrorList  elStrings;          // Sqlite error strings list
  const ADRList    adrlStrings;        // Can db be deleted strings list
  sqlite3         *sqlDB;              // Pointer to SQL context
  int              iError;             // Last error code
  SqlResult        srKeys;             // Last Execute(Raw) result
  unsigned int     uiQueryRetries;     // Times to retry query before failing
  ClkDuration      cdRetry,            // Sleep for this time when retrying
                   cdQuery;            // Last query execution time
  /* -- Tables ----------------------------------------------------- */ public:
  const string     strMemoryDBName,    // Memory only database (no disk)
                   strCVKeyColumn,     // Name of cvar 'key' column
                   strCVFlagsColumn,   // Name of cvar 'flags' column
                   strCVValueColumn;   // Name of cvar 'value' column
  const string_view strvCVTable,       // Name of cvar table
                    strvLCTable,       // Name of lua cache table
                    strvLCCRCColumn,   // Name of lua cache 'crc' column
                    strvLCTimeColumn,  // Name of lua cache 'time' column
                    strvLCRefColumn,   // Name of lua cache 'ref' column
                    strvLCCodeColumn,  // Name of lua cache 'data' column
                    strvOn, strvOff,   // "ON" and "OFF" strings
                    strvPKeyTable,     // Name of pvt key table
                    strvPIndexColumn,  // Name of pvt key 'index' column
                    strvPValueColumn,  // Name of pvt key 'value' column
                    strvSKeyTable,     // Name of schema table
                    strvSIndexColumn,  // Name of schema 'index' column
                    strvSValueColumn,  // Name of schema 'value' column
                    strvSVersionKey;   // Name of schema version record name
  /* -- Convert sql error id to string ---------------------------- */ private:
  const string_view &ResultToString(const int iCode) const
    { return elStrings.Get(iCode); }
  /* -- Convert ADR id to string ------------------------------------------- */
  const string_view &ADResultToString(const ADResult adrResult) const
    { return adrlStrings.Get(adrResult); }
  /* -- Close the database ------------------------------------------------- */
  void DoClose(void)
  { // Number of retries needed to close the database
    unsigned int uiRetries;
    // Wait until the database can be closed
    for(uiRetries = 0; sqlite3_close(sqlDB) == SQLITE_BUSY; ++uiRetries)
    { // Keep trying to clean up if we can (this shouldn't really happen).
      Finalise();
      // Don't whore the CPU usage while waiting for async ops.
      StdSuspend();
    } // Database handle no longer valid
    sqlDB = nullptr;
    // Say we closed the database
    cLog->LogExSafe(uiRetries > 0 ? LH_WARNING : LH_INFO,
      "Sql database '$' closed successfully ($ retries).",
        IdentGet(), uiRetries);
  }
  /* -- Build a new node into the a records list --------------------------- */
  void DoPair(SqlRecordsMap &srmMap, const int iType, const char*const cpKey,
    const void*const vpPtr, const size_t stSize)
  { // Generate the memory block with the specified data.
    Memory mData{ stSize, vpPtr };
    // Generate the pair value and move the memory into it.
    SqlData sdRef{ StdMove(mData), iType };
    // Insert a new pair moving key and value across.
    srmMap.emplace(make_pair(cpKey, StdMove(sdRef)));
  }
  /* -- Build a new node from an integral value ---------------------------- */
  template<typename T>void DoPair(SqlRecordsMap &srmMap, const int iType,
    const char*const cpKey, const T tVal)
      { DoPair(srmMap, iType, cpKey, &tVal, sizeof(tVal)); }
  /* -- Set error code ----------------------------------------------------- */
  void SetError(const int iCode) { iError = iCode; }
  /* -- Compile the sql command and store output rows ---------------------- */
  void DoStep(sqlite3_stmt*const stmtData)
  { // Retry count
    unsigned int uiRetries = 0;
    // Until we're done with all the data
    for(SetError(sqlite3_step(stmtData));
        IsErrorNotEqual(SQLITE_DONE);
        SetError(sqlite3_step(stmtData)))
    { // Check status
      switch(GetError())
      { // Success so continue execution normal or error in the row
        case SQLITE_OK: case SQLITE_ROW: break;
        // The database is busy?
        case SQLITE_BUSY:
          // Wait a little and try again if busy until we've retried enough
          if(++uiRetries < uiQueryRetries || uiQueryRetries == StdMaxUInt)
            { StdSuspend(cdRetry); continue; }
          // Return failure
          [[fallthrough]];
        // Complete and utter failure
        default: return;
      } // Create key/memblock map and reserve entries
      SqlRecordsMap srmMap;
      // For each column, add to string/memblock map
      const int iColMax = sqlite3_column_count(stmtData);
      for(int iCol = 0; iCol < iColMax; ++iCol)
      { // What's the column type? Also save the value, we need it
        switch(const int iType = sqlite3_column_type(stmtData, iCol))
        { // 64-bit integer? Size the memory block and assign integer to it
          case SQLITE_INTEGER:
          { // Insert the int64 data into the vector
            DoPair(srmMap, iType, sqlite3_column_name(stmtData, iCol),
              sqlite3_column_int64(stmtData, iCol));
            break;
          } // 64-bit IEEE float?
          case SQLITE_FLOAT:
          { // Insert the double into the vector
            DoPair(srmMap, iType, sqlite3_column_name(stmtData, iCol),
              sqlite3_column_double(stmtData, iCol));
            break;
          } // Raw data?
          case SQLITE_BLOB:
          { // Copy blob to a memory block if there's something to copy
            DoPair(srmMap, iType, sqlite3_column_name(stmtData, iCol),
              sqlite3_column_blob(stmtData, iCol),
              static_cast<size_t>(sqlite3_column_bytes(stmtData, iCol)));
            break;
          } // Text?
          case SQLITE_TEXT:
          { // Copy to a memory block if there is something to copy
            DoPair(srmMap, iType, sqlite3_column_name(stmtData, iCol),
              sqlite3_column_text(stmtData, iCol),
              static_cast<size_t>(sqlite3_column_bytes(stmtData, iCol)));
            break;
          } // NULL?
          case SQLITE_NULL:
          { // Copy to a memory block if there is something to copy
            DoPair(srmMap, iType, sqlite3_column_name(stmtData, iCol),
              nullptr, 0);
            break;
          } // Unknown data type (shouldn't ever get here?)
          default:
          { // Warn on invalid type
            cLog->LogWarningExSafe("Sql ignored unknown result type $[0x$$$] "
              "for column '$' sized $ bytes.", iType, hex, iType, dec,
              sqlite3_column_name(stmtData, iCol),
              sqlite3_column_bytes(stmtData, iCol));
            break;
          }
        }
      } // Move key/values into records list if there were keys inserted
      if(!srmMap.empty()) srKeys.emplace_back(StdMove(srmMap));
    } // Set error code to OK because it's set to SQLITE_DONE
    SetError(SQLITE_OK);
  }
  /* -- Can database be deleted, no point keeping if it's empty! ----------- */
  ADResult CanDatabaseBeDeleted(void)
  { // No if this is a temporary database as theres nothing to delete.
    if(FlagIsSet(SF_ISTEMPDB)) return ADR_ERR_TEMP_DB;
    // No if we are not allowed to delete the database
    if(FlagIsClear(SF_DELETEEMPTYDB)) return ADR_ERR_DENY_OPTION;
    // Compare how many tables there are in the database...
    switch(GetRecordCount("sqlite_master", " AS tables WHERE `type`='table'"))
    { // Error occured? Don't delete the database!
      case StdMaxSizeT: return ADR_ERR_LU_TABLE;
      // No tables? Delete the database!
      case 0: return ADR_OK_NO_TABLES;
      // Only one or two tables (key and/or cvars table)?
      case 1: case 2:
        // Get number of records in the cvars table
        switch(GetRecordCount(strvCVTable))
        { // Error occured? Don't delete the database!
          case StdMaxSizeT: return ADR_ERR_LU_RECORD;
          // Zero records? Delete the database! There is always the
          // mandatory private key inside the cvars database so there is no
          // point keeping that if there is nothing else in the cvars database.
          case 0: return ADR_OK_NO_RECORDS;
          // More than one record? Do not delete the database
          default: return ADR_ERR_RECORDS_EXIST;
        } // Call should not continue after this switch statement
      // More than one table? Do not delete
      default: return ADR_ERR_TABLES_EXIST;
    } // Call should not continue after this switch statement
  }
  /* -- Sends arguments to sql if at maximum ------------------------------- */
  bool DoExecuteParamCheckCommit(sqlite3_stmt*const stmtData, int &iCol,
    const int iMax)
  { // Don't do anything if there is an error
    if(IsError()) return false;
    // If we're not at the maximum parameters yet? Ok to continue
    if(iCol < iMax) { ++iCol; return true; }
    // Do step and tell caller to stop if failed
    DoStep(stmtData);
    if(IsError()) return false;
    // Reset bindings so we can insert another
    SetError(sqlite3_reset(stmtData));
    if(IsError()) return false;
    // Reset column id
    iCol = 0;
    // Success
    return true;
  }
  /* -- No more calls so just compile what we have left -------------------- */
  void DoExecuteParam(const int iCol, const int iMax,
    sqlite3_stmt*const stmtData)
  { // Return if there are no columns currently queued
    if(!iCol) return;
    // I forgot to specify all the parameters properly
    XC("Internal error: Not enough Sql parameters sent!",
       "Statement", sqlite3_sql(stmtData), "Expected", iMax, "Actual", iCol);
  }
  /* -- Constructor that stores a 64-bit integer --------------------------- */
  template<typename ...VarArgs>
    void DoExecuteParam(int &iCol, const int iMax, sqlite3_stmt*const stmtData,
      const sqlite_int64 qValue, const VarArgs &...vaArgs)
  { // Log the parameter
    cLog->LogDebugExSafe("- Arg #$<Int64/Integer> = $ <$0x$>.",
      iCol, qValue, hex, qValue);
    // Process as integer then pass next arguments
    SetError(sqlite3_bind_int64(stmtData, iCol, qValue));
    if(DoExecuteParamCheckCommit(stmtData, iCol, iMax))
      DoExecuteParam(iCol, iMax, stmtData, vaArgs...);
  }
  /* -- Initialise as double of type SQLITE_FLOAT -------------------------- */
  template<typename ...VarArgs>
    void DoExecuteParam(int &iCol, const int iMax, sqlite3_stmt*const stmtData,
      const double dValue, const VarArgs &...vaArgs)
  { // Log the parameter
    cLog->LogDebugExSafe("- Arg #$<Double/Float> = $$.", iCol, fixed, dValue);
    // Process as integer then pass next arguments
    SetError(sqlite3_bind_double(stmtData, iCol, dValue));
    if(DoExecuteParamCheckCommit(stmtData, iCol, iMax))
      DoExecuteParam(iCol, iMax, stmtData, vaArgs...);
  }
  /* -- Constructor that stores similar types of integer ------------------- */
  template<typename ...VarArgs>
    void DoExecuteParam(int &iCol, const int iMax, sqlite3_stmt*const stmtData,
      int iValue, const VarArgs &...vaArgs)
  { // Log the parameter
    cLog->LogDebugExSafe("- Arg #$<Int/Int> = $ <$0x$>.",
      iCol, iValue, hex, iValue);
    // Process as integer then pass next arguments
    SetError(sqlite3_bind_int64(stmtData, iCol,
      static_cast<sqlite_int64>(iValue)));
    if(DoExecuteParamCheckCommit(stmtData, iCol, iMax))
      DoExecuteParam(iCol, iMax, stmtData, vaArgs...);
  }
  /* -- Constructor that stores similar types of integer ------------------- */
  template<typename ...VarArgs>
    void DoExecuteParam(int &iCol, const int iMax, sqlite3_stmt*const stmtData,
      const unsigned int uiValue, const VarArgs &...vaArgs)
  { // Log the parameter
    cLog->LogDebugExSafe("- Arg #$<UInt/Int> = $ <$0x$>.",
      iCol, uiValue, hex, uiValue);
    // Process as integer then pass next arguments
    SetError(sqlite3_bind_int64(stmtData, iCol,
      static_cast<sqlite_int64>(uiValue)));
    if(DoExecuteParamCheckCommit(stmtData, iCol, iMax))
      DoExecuteParam(iCol, iMax, stmtData, vaArgs...);
  }
  /* -- Constructor that stores similar types of integer ------------------- */
  template<typename ...VarArgs>
    void DoExecuteParam(int &iCol, const int iMax, sqlite3_stmt*const stmtData,
      const long lValue, const VarArgs &...vaArgs)
  { // Log the parameter
    cLog->LogDebugExSafe("- Arg #$<Long/Int> = $ <$0x$>.",
      iCol, lValue, hex, lValue);
    // Process as integer then pass next arguments
    SetError(sqlite3_bind_int64(stmtData, iCol,
      static_cast<sqlite_int64>(lValue)));
    if(DoExecuteParamCheckCommit(stmtData, iCol, iMax))
      DoExecuteParam(iCol, iMax, stmtData, vaArgs...);
  }
  /* -- Constructor that stores similar types of integer ------------------- */
  template<typename ...VarArgs>
    void DoExecuteParam(int &iCol, const int iMax, sqlite3_stmt*const stmtData,
      const short sValue, const VarArgs &...vaArgs)
  { // Log the parameter
    cLog->LogDebugExSafe("- Arg #$<Short/Int> = $ <$0x$>.",
      iCol, sValue, hex, sValue);
    // Process as integer then pass next arguments
    SetError(sqlite3_bind_int64(stmtData, iCol,
      static_cast<sqlite_int64>(sValue)));
    if(DoExecuteParamCheckCommit(stmtData, iCol, iMax))
      DoExecuteParam(iCol, iMax, stmtData, vaArgs...);
  }
  /* -- Initialise as a c-string with the specified size as text ----------- */
  template<typename ...VarArgs>
    void DoExecuteParam(int &iCol, const int iMax, sqlite3_stmt*const stmtData,
      const size_t stSize, const char*const cpStr, const VarArgs &...vaArgs)
  { // Log the parameter
    cLog->LogDebugExSafe("- Arg #$<Size+CStr/Text> = \"$\" ($ bytes).",
      iCol, cpStr, stSize);
    // Process as text then pass next arguments
    SetError(sqlite3_bind_text(stmtData, iCol, cpStr,
      UtilIntOrMax<int>(stSize), fcbSqLiteTransient));
    if(DoExecuteParamCheckCommit(stmtData, iCol, iMax))
      DoExecuteParam(iCol, iMax, stmtData, vaArgs...);
  }
  /* -- Initialise as a stand alone text string ---------------------------- */
  template<typename ...VarArgs>
    void DoExecuteParam(int &iCol, const int iMax, sqlite3_stmt*const stmtData,
      const char*const cpStr, const VarArgs &...vaArgs)
  { // Get text length
    const size_t stLen = strlen(cpStr);
    // Log the parameter
    cLog->LogDebugExSafe("- Arg #$<CStr/Text> = \"$\" ($ bytes).",
      iCol, cpStr, stLen);
    // Process as text then pass next arguments
    SetError(sqlite3_bind_text(stmtData, iCol, cpStr,
      UtilIntOrMax<int>(stLen), fcbSqLiteTransient));
    if(DoExecuteParamCheckCommit(stmtData, iCol, iMax))
      DoExecuteParam(iCol, iMax, stmtData, vaArgs...);
  }
  /* -- Initialise as a c++ string ----------------------------------------- */
  template<typename ...VarArgs>
    void DoExecuteParam(int &iCol, const int iMax, sqlite3_stmt*const stmtData,
    const string &strStr, const VarArgs &...vaArgs)
  { // Log the parameter
    cLog->LogDebugExSafe("- Arg #$<Str/Text> = \"$\" ($ bytes).",
      iCol, strStr, strStr.length());
    // Process as text then pass next arguments
    SetError(sqlite3_bind_text(stmtData, iCol, strStr.c_str(),
      UtilIntOrMax<int>(strStr.length()), fcbSqLiteTransient));
    if(DoExecuteParamCheckCommit(stmtData, iCol, iMax))
      DoExecuteParam(iCol, iMax, stmtData, vaArgs...);
  }
  /* -- Initialise as a c++ string_view ------------------------------------ */
  template<typename ...VarArgs>
    void DoExecuteParam(int &iCol, const int iMax, sqlite3_stmt*const stmtData,
    const string_view &strvStr, const VarArgs &...vaArgs)
  { // Log the parameter
    cLog->LogDebugExSafe("- Arg #$<StrV/Text> = \"$\" ($ bytes).",
      iCol, strvStr, strvStr.length());
    // Process as text then pass next arguments
    SetError(sqlite3_bind_text(stmtData, iCol, strvStr.data(),
      UtilIntOrMax<int>(strvStr.length()), fcbSqLiteTransient));
    if(DoExecuteParamCheckCommit(stmtData, iCol, iMax))
      DoExecuteParam(iCol, iMax, stmtData, vaArgs...);
  }
  /* -- Initialise as a c-string with the specified size as a blob --------- */
  template<typename ...VarArgs>
    void DoExecuteParam(int &iCol, const int iMax, sqlite3_stmt*const stmtData,
      const char*const cpPtr, const size_t stSize, const VarArgs &...vaArgs)
  { // Log the parameter
    cLog->LogNLCDebugExSafe("- Arg #$<Ptr+Size/Blob> = $ bytes.",
      iCol, stSize);
    // Process as blob then pass next arguments
    SetError(sqlite3_bind_blob(stmtData, iCol, cpPtr,
      UtilIntOrMax<int>(stSize), fcbSqLiteTransient));
    if(DoExecuteParamCheckCommit(stmtData, iCol, iMax))
      DoExecuteParam(iCol, iMax, stmtData, vaArgs...);
  }
  /* -- Initialise as custom type ------------------------------------------ */
  template<typename ...VarArgs>
    void DoExecuteParam(int &iCol, const int iMax, sqlite3_stmt*const stmtData,
      const char*const cpPtr, const size_t stSize, const int iType,
      const VarArgs &...vaArgs)
  { // Log the parameter
    cLog->LogNLCDebugExSafe("- Arg #$<CStr/$> = \"$\" $ bytes.",
      iCol, iType, cpPtr, stSize);
    // Process as forced custom type then pass next arguments
    SetError((iType == SQLITE_TEXT ?
      sqlite3_bind_text(stmtData, iCol, cpPtr,
        UtilIntOrMax<int>(stSize), fcbSqLiteTransient)
           : (iType == SQLITE_BLOB ?
      sqlite3_bind_blob(stmtData, iCol, cpPtr,
        UtilIntOrMax<int>(stSize), fcbSqLiteTransient)
           : (iType == SQLITE_INTEGER ?
      sqlite3_bind_int64(stmtData, iCol, StrToNum<sqlite_int64>(cpPtr))
           : (iType == SQLITE_FLOAT ?
      sqlite3_bind_double(stmtData, iCol, StrToNum<double>(cpPtr))
           : (iType == SQLITE_NULL ?
      sqlite3_bind_null(stmtData, iCol)
           : SQLITE_ERROR))))));
    if(DoExecuteParamCheckCommit(stmtData, iCol, iMax))
      DoExecuteParam(iCol, iMax, stmtData, vaArgs...);
  }
  /* -- Initialise as a memory block --------------------------------------- */
  template<typename ...VarArgs>
    void DoExecuteParam(int &iCol, const int iMax, sqlite3_stmt*const stmtData,
      const MemConst &mcRef, const VarArgs &...vaArgs)
  { // Log the parameter
    cLog->LogDebugExSafe("- Arg #$<Memory/Blob> = $ bytes.",
      iCol, mcRef.MemSize());
    // Process as blob then pass next arguments
    SetError(sqlite3_bind_blob(stmtData, iCol, mcRef.MemPtr<char>(),
      UtilIntOrMax<int>(mcRef.MemSize()), fcbSqLiteTransient));
    // Send the parameters if we've binded the maximum allowed
    if(DoExecuteParamCheckCommit(stmtData, iCol, iMax))
      DoExecuteParam(iCol, iMax, stmtData, vaArgs...);
  }
  /* -- Send command to sql in raw format ---------------------------------- */
  template<typename ...VarArgs>
    void DoExecute(const string &strQuery, const VarArgs &...vaArgs)
  { // Reset previous results
    Reset();
    // Set query start time
    const ClockInterval<> ciStart;
    // Statement preparation
    sqlite3_stmt *stmtData = nullptr;
    SetError(sqlite3_prepare_v2(sqlDB, strQuery.c_str(),
      UtilIntOrMax<int>(strQuery.length()), &stmtData, nullptr));
    // If succeeded then start parsing the input and ouput
    if(IsNoError())
    { // Free the statement context incase of exception
      typedef unique_ptr<sqlite3_stmt,
        function<decltype(sqlite3_finalize)>> SqliteStatementPtr;
      const SqliteStatementPtr sspPtr{ stmtData, sqlite3_finalize };
      // Get number of parameters required to bind and if there is any
      if(const int iMax = sqlite3_bind_parameter_count(stmtData))
      { // Column id
        int iCol = 1;
        // Get maximum parameters allowed before we have to send them
        DoExecuteParam(iCol, iMax, stmtData, vaArgs...);
      } // We don't have parameters so just execute the statement
      else DoStep(stmtData);
    } // Get end query time to get total execution duration
    cdQuery = ciStart.CIDelta();
  }
  /* -- Set a pragma (used only with cvar callbacks) ----------------------- */
  void Pragma(const string_view &strvVar)
  { // Execute without value
    if(Execute(StrAppend("PRAGMA ", strvVar)))
    { // If it was not because the database is read only or busy? Throw error
      if(IsNotBusyOrReadOnlyError())
        XC("Database reconfiguration error!",
           "Setting", strvVar, "Error", GetErrorStr(), "Code", GetError());
      // Log error instead
      cLog->LogWarningExSafe(
        "Sql set pragma '$' failed because $ ($<$>)!",
        strvVar, GetErrorStr(), GetErrorAsIdString(), GetError());
    } // Log and return success
    else cLog->LogDebugExSafe("Sql pragma '$' succeeded.", strvVar);
  }
  /* -- Set a pragma (used only with cvar callbacks) ----------------------- */
  void Pragma(const string_view &strvVar, const string_view &strvVal)
  { // Execute with value
    if(Execute(StrFormat("PRAGMA $=$", strvVar, strvVal)))
    { // If it was not because the database is read only or busy? Throw error
      if(IsNotBusyOrReadOnlyError())
        XC("Database reconfiguration error!",
           "Variable", strvVar,       "Value", strvVal,
           "Error",    GetErrorStr(), "Code",  GetError());
      // Log and return failure
      cLog->LogWarningExSafe(
        "Sql set pragma '$' to '$' failed because $ ($<$>)!",
        strvVar, strvVal, GetErrorStr(), GetErrorAsIdString(), GetError());
    } // Log and return success
    else cLog->LogDebugExSafe("Sql set pragma '$' to '$' succeeded.",
      strvVar, strvVal);
  }
  /* -- Is sqlite database opened? --------------------------------- */ public:
  bool IsOpened(void) { return !!sqlDB; }
  /* -- Heap used ---------------------------------------------------------- */
  size_t HeapUsed(void) const
    { return static_cast<size_t>(sqlite3_memory_used()); }
  /* -- Execute a command from Lua ----------------------------------------- */
  int ExecuteFromLua(lua_State*const lS, const string &strQuery)
  { // Log progress
    cLog->LogDebugExSafe("Sql executing '$'<$> from LUA...",
      strQuery, strQuery.length());
    // Reset previous results
    Reset();
    // Set query start time
    const ClockInterval<> ciStart;
    // Statement preparation
    sqlite3_stmt *stmtData = nullptr;
    SetError(sqlite3_prepare_v2(sqlDB, strQuery.c_str(),
      UtilIntOrMax<int>(strQuery.length()), &stmtData, nullptr));
    // Starting LUA parameter and current enumerated parameter
    const int iStartParam = 2;
    int iParam = iStartParam;
    // If succeeded then start parsing the input and ouput
    if(IsNoError())
    { // Free the statement context incase of exception
      typedef unique_ptr<sqlite3_stmt,
        function<decltype(sqlite3_finalize)>> SqliteStatementPtr;
      const SqliteStatementPtr sspPtr{ stmtData, sqlite3_finalize };
      // Get maximum parameters allowed before we have to send them
      if(const int iMax = sqlite3_bind_parameter_count(stmtData))
      { // No parameters specified? Just execute the statement
        if(LuaUtilIsNone(lS, iParam)) SetError(SQLITE_FORMAT);
        // Parameter is valid?
        else
        { // Column id
          int iCol = 1;
          // Repeat...
          do
          { // Get lua variable type and compare its type
            switch(const int iType = lua_type(lS, iParam))
            { // Variable is a number?
              case LUA_TNUMBER:
              { // Variable is actually an integer?
                if(LuaUtilIsInteger(lS, iParam))
                { // Get integer, log it and add it as integer
                  const lua_Integer liInt = LuaUtilToInt(lS, iParam);
                  cLog->LogDebugExSafe("- Arg #$<Integer/Int> = $ <$0x$>.",
                    iCol, liInt, hex, liInt);
                  SetError(sqlite3_bind_int64(stmtData, iCol,
                    static_cast<sqlite_int64>(liInt)));
                } // Variable is actually a number
                else
                { // Get double, log it and add it as number
                  const lua_Number lnFloat = LuaUtilToNum(lS, iParam);
                  cLog->LogDebugExSafe("- Arg #$<Number/Float> = $$.",
                    iCol, fixed, lnFloat);
                  SetError(sqlite3_bind_double(stmtData, iCol,
                    static_cast<double>(lnFloat)));
                } // Done
                break;
              } // Variable is a string
              case LUA_TSTRING:
              { // Get string, store size, log parameter, add as string
                size_t stS;
                const char*const cpStr = LuaUtilToLString(lS, iParam, stS);
                cLog->LogDebugExSafe(
                  "- Arg #$<String/Text> = \"$\" ($ bytes).",
                  iCol, cpStr, stS);
                SetError(sqlite3_bind_text(stmtData,
                  iCol, cpStr, UtilIntOrMax<int>(stS), fcbSqLiteTransient));
                break;
              } // Variable is a boolean
              case LUA_TBOOLEAN:
              { // Get boolean, log parameter, convert and add as integer
                const bool bBool = LuaUtilToBool(lS, iParam);
                cLog->LogDebugExSafe("- Arg #$<Bool/Int> = $.",
                  iCol, StrFromBoolTF(bBool));
                SetError(sqlite3_bind_int64(stmtData, iCol,
                  static_cast<sqlite_int64>(bBool)));
                break;
              } // Variable is a 'nil'
              case LUA_TNIL:
              { // Log the nil and add it to the Sql query
                cLog->LogDebugExSafe("- Arg #$<Nil/Null>.", iCol);
                SetError(sqlite3_bind_null(stmtData, iCol));
                break;
              } // Variable is userdata
              case LUA_TUSERDATA:
              { // Get reference to memory block, log it and push data to list
                const MemConst &mcRef =
                  *LuaUtilGetPtr<Asset>(lS, iParam, *cAssets);
                cLog->LogDebugExSafe("- Arg #$<Asset/Blob> = $ bytes.",
                  iCol, mcRef.MemSize());
                SetError(sqlite3_bind_blob(stmtData, iCol,
                  mcRef.MemPtr<char>(), UtilIntOrMax<int>(mcRef.MemSize()),
                  fcbSqLiteTransient));
                break;
              } // Other variable (ignore)
              default: XC("Unsupported parameter type!",
                          "Param", iParam, "LuaType", iType,
                          "Typename", LuaUtilGetType(lS, iType));
            } // Do the step if needed break if not needed or error
            if(!DoExecuteParamCheckCommit(stmtData, iCol, iMax)) break;
          } // ...until no parameters left
          while(!LuaUtilIsNone(lS, ++iParam));
        }
      } // We can't add parameters so just execute the statement
      else DoStep(stmtData);
    } // Get end query time to get total execution duration
    cdQuery = ciStart.CIDelta();
    // Log result
    cLog->LogDebugExSafe("- Total: $; Code: $<$>; RTT: $ sec.",
      iParam - iStartParam, ResultToString(GetError()), GetError(), TimeStr());
    // Return error status
    return iError;
  }
  /* -- Convert records to lua table --------------------------------------- */
  void RecordsToLuaTable(lua_State*const lS)
  { // Create the table, we're creating a indexed/value array
    LuaUtilPushTable(lS, srKeys.size());
    // Memory id
    lua_Integer liId = 1;
    // For each table item
    for(const SqlRecordsMap &srmRef : srKeys)
    { // Table index
      LuaUtilPushInt(lS, liId);
      // Create the table, we're creating non-indexed key/value pairs
      LuaUtilPushTable(lS, 0, srmRef.size());
      // For each column data
      for(const SqlRecordsMapPair &srmpRef : srmRef)
      { // Get data list item
        const SqlData &sdRef = srmpRef.second;
        // Get type
        switch(sdRef.iType)
        { // Text?
          case SQLITE_TEXT:
            LuaUtilPushMem(lS, sdRef);
            break;
          // 64-bit integer?
          case SQLITE_INTEGER:
            LuaUtilPushInt(lS, sdRef.MemReadInt<lua_Integer>());
            break;
          // 64-bit IEEE float?
          case SQLITE_FLOAT:
            LuaUtilPushNum(lS, sdRef.MemReadInt<lua_Number>());
            break;
          // Raw data? Save as array
          case SQLITE_BLOB:
          { // Create memory block array class
            Asset &aRef = *LuaUtilClassCreate<Asset>(lS, *cAssets);
            // Initialise the memory block depending on if we have data
            if(sdRef.MemIsNotEmpty())
              aRef.MemInitData(sdRef.MemSize(), sdRef.MemPtr<void*>());
            else aRef.MemInitBlank();
            // Done
            break;
          } // No data? Push a 'false' since we can't have 'nil' in keypairs.
          case SQLITE_NULL: LuaUtilPushBool(lS, false); break;
          // Since we don't store anything invalid in srKeys, this will NEVER
          // get here, but we'll hard fail just incase. GCC needs the typecast.
          default: XC("Invalid record type in results!",
                      "Record", static_cast<uint64_t>(liId),
                      "Column", srmpRef.first,
                      "Type",   sdRef.iType);
                   break;
        } // Push key name
        LuaUtilSetField(lS, -2, srmpRef.first.c_str());
      } // Push key pair as integer table
      LuaUtilSetRaw(lS, -3);
      // Next result number
      ++liId;
    }
  }
  /* -- Reset last sql result ---------------------------------------------- */
  void Reset(void)
  { // Clear error
    SetError(SQLITE_OK);
    // Clear last result data
    srKeys.clear();
    // Reset query time
    cdQuery = seconds{0};
  }
  /* -- Dispatch stored transaction with logging --------------------------- */
  template<typename ...VarArgs>
    int Execute(const string &strQuery, const VarArgs &...vaArgs)
  { // Ignore if nothing to dispatch
    if(strQuery.empty()) return SQLITE_ERROR;
    // Parameters count
    const size_t stCount = sizeof...(vaArgs);
    // Log query, do execution and return result
    cLog->LogDebugExSafe("Sql executing '$'<$>...",
      strQuery, strQuery.length());
    DoExecute(strQuery, vaArgs...);
    cLog->LogDebugExSafe("- Total: $; Code: $<$>; RTT: $ sec.",
      stCount, ResultToString(GetError()), GetError(), TimeStr());
    // Return error status
    return iError;
  }
  /* -- Dispatch stored transaction with logging but return success bool -- */
  template<typename ...VarArgs>
    bool ExecuteAndSuccess(const string &strQuery, const VarArgs &...vaArgs)
      { return Execute(strQuery, vaArgs...) == SQLITE_OK; }
  /* -- Check integrity ---------------------------------------------------- */
  bool CheckIntegrity(void)
  { // Do check (We need a result so dont use Pragma())
    if(Execute("PRAGMA integrity_check(1)"))
    { // Log and return failure
      cLog->LogWarningExSafe(
        "Sql failed integrity check failed because $ ($<$>)!",
        GetErrorStr(), GetErrorAsIdString(), GetError());
      return false;
    } // This should never be true but just incase
    if(srKeys.empty())
    { // Log and return failure
      cLog->LogWarningSafe("Sql integrity check failed to return result row.");
      return false;
    } // Get reference to the map of memory blocks. It shouldn't be empty
    const SqlRecordsMap &mbMap = *srKeys.cbegin();
    if(mbMap.empty())
    { // Log and return failure
      cLog->LogWarningSafe(
        "Sql integrity check failed to return result columns.");
      return false;
    } // Get result string. It should say 'ok' if everything went ok
    const string strResult{ mbMap.cbegin()->second.MemToStringSafe() };
    if(strResult != "ok")
    { // Log and return failure
      cLog->LogWarningExSafe("Sql database corrupted: $", strResult);
      return false;
    } // Passed so return success
    cLog->LogInfoSafe("Sql integrity check passed.");
    return true;
  }
  /* -- Get size of database ----------------------------------------------- */
  uint64_t Size(void)
  { // Get the database page size
    if(Execute("pragma page_size") || srKeys.empty()) return StdMaxUInt64;
    // Get reference to keys list
    const SqlRecordsMap &srmSizePair = *srKeys.cbegin();
    if(srmSizePair.size() != 1) return StdMaxUInt64;
    // Get first item
    const SqlRecordsMapConstIt srmciSizeIt{ srmSizePair.cbegin() };
    if(srmciSizeIt->first != "page_size") return StdMaxUInt64;
    // Return number of tables
    const uint64_t uqPageSize = srmciSizeIt->second.MemReadInt<uint64_t>();
    // Get the database page count
    if(Execute("pragma page_count") || srKeys.empty()) return StdMaxUInt64;
    // Get reference to keys list
    const SqlRecordsMap &srmCount = *srKeys.cbegin();
    if(srmCount.size() != 1) return StdMaxUInt64;
    // Get first item
    const SqlRecordsMapConstIt srmciCount{ srmCount.cbegin() };
    if(srmciCount->first != "page_count") return StdMaxUInt64;
    // Return number of tables
    const uint64_t uqPageCount = srmciCount->second.MemReadInt<uint64_t>();
    // Return result
    return uqPageSize * uqPageCount;
  }
  /* -- Return error string ------------------------------------------------ */
  const char *GetErrorStr(void) const { return sqlite3_errmsg(sqlDB); }
  /* -- Return error code -------------------------------------------------- */
  int GetError(void) const { return iError; }
  bool IsErrorEqual(const int iWhat) const { return GetError() == iWhat; }
  bool IsErrorNotEqual(const int iWhat) const { return !IsErrorEqual(iWhat); }
  bool IsError(void) const { return IsErrorNotEqual(SQLITE_OK); }
  bool IsNoError(void) const { return !IsError(); }
  bool IsBusyError(void) const { return IsErrorEqual(SQLITE_BUSY); }
  bool IsReadOnlyError(void) const { return IsErrorEqual(SQLITE_READONLY); }
  bool IsBusyOrReadOnlyError(void) const
    { return IsReadOnlyError() || IsBusyError(); }
  bool IsNotBusyOrReadOnlyError(void) const
    { return !IsBusyOrReadOnlyError(); }
  const string_view &GetErrorAsIdString(void) const
    { return ResultToString(iError); }
  /* -- Return duration of last query -------------------------------------- */
  double Time(void) const { return ClockDurationToDouble(cdQuery); }
  /* -- Return formatted query time ---------------------------------------- */
  const string TimeStr(void) const { return StrShortFromDuration(Time()); }
  /* -- Returns if sql is in a transaction --------------------------------- */
  bool Active(void) const { return !sqlite3_get_autocommit(sqlDB); }
  /* -- Return string map of records --------------------------------------- */
  SqlResult &GetRecords(void) { return srKeys; }
  /* -- Useful aliases ----------------------------------------------------- */
  int Begin(void)
    { return Execute("BEGIN TRANSACTION"); }
  int End(void)
    { return Execute("END TRANSACTION"); }
  int DropTable(const string_view &strvTable)
    { return Execute(StrFormat("DROP table `$`", strvTable)); }
  int FlushTable(const string_view &strvTable)
    { return Execute(StrFormat("DELETE from `$`", strvTable)); }
  int Optimise(void)
    { return Execute("VACUUM"); }
  int Affected(void)
    { return sqlite3_changes(sqlDB); }
  /* -- Process a count(*) requested --------------------------------------- */
  size_t GetRecordCount(const string_view &strvTable,
    const string_view &strvCondition=cCommon->CommonCBlank())
  { // Do a table count lookup. If succeeded and have records?
    if(ExecuteAndSuccess(StrFormat("SELECT count(*) FROM `$`$",
      strvTable, strvCondition)) && !srKeys.empty())
    { // Get reference to keys list and if we have one result?
      const SqlRecordsMap &srmRef = *srKeys.cbegin();
      if(srmRef.size() == 1)
      { // Get first item and if it's the count?
        const SqlRecordsMapConstIt srmciIt{ srmRef.cbegin() };
        if(srmciIt->first == "count(*)")
        { // Get result because we're going to clean up after
          const size_t stCount = srmciIt->second.MemReadInt<size_t>();
          // Clean up downloaded records
          Reset();
          // Return number of tables
          return stCount;
        } // Count(*) not found
      } // Result does not have one record
    } // Failed to execute query or no records, return failure
    return StdMaxSizeT;
  }
  /* -- Check if a table exists -------------------------------------------- */
  bool IsTableExist(const string_view &strvTable)
    { return ExecuteAndSuccess(
        "SELECT `name` FROM `sqlite_master` WHERE `type`='table' AND `name`=?",
        strvTable) && !GetRecords().empty(); }
  /* -- Flush all orphan statements ---------------------------------------- */
  size_t Finalise(void) const
  { // Number of orphan transactions
    size_t stOrphan = 0;
    // Finalise all remaining statements until there are no more left
    while(sqlite3_stmt*const stmtData = sqlite3_next_stmt(sqlDB, nullptr))
      { sqlite3_finalize(stmtData); ++stOrphan; }
    // Return number of orphans
    return stOrphan;
  }
  /* ----------------------------------------------------------------------- */
  bool CVarReadKeys(void)
  { // Read all variable names and if failed?
    if(Execute(StrFormat("SELECT `$` from `$`", strCVKeyColumn, strvCVTable)))
    { // Put in log and return nothing loaded
      cLog->LogWarningExSafe(
        "CVars failed to fetch cvar key names because $ ($)!",
        GetErrorStr(), GetError());
      return false;
    } // Write number of records read and return success
    cLog->LogDebugExSafe("Sql read $ key names from CVars table.",
      GetRecords().size());
    return true;
  }
  /* ----------------------------------------------------------------------- */
  bool CVarReadAll(void)
  { // Read all variables and if failed?
    if(Execute(StrFormat("SELECT `$`,`$`,`$` from `$`",
      strCVKeyColumn, strCVFlagsColumn, strCVValueColumn, strvCVTable)))
    { // Put in log and return nothing loaded
      cLog->LogWarningExSafe("Sql failed to read CVars table because $ ($)!",
        GetErrorStr(), GetError());
      return false;
    } // Write number of records read and return success
    cLog->LogDebugExSafe("Sql read $ records from CVars table.",
      GetRecords().size());
    return true;
  }
  /* ----------------------------------------------------------------------- */
  CreateTableResult CVarDropTable(void)
  { // If table exists return that it already exists
    if(IsTableExist(strvCVTable)) return CTR_OK_ALREADY;
    // Drop the SQL table and if failed?
    if(DropTable(strvCVTable))
    { // Write error in console and return failure
      cLog->LogWarningExSafe(
        "Sql failed to destroy CVars table because $ ($)!",
        GetErrorStr(), GetError());
      return CTR_FAIL;
    } // Write success in console and return success
    cLog->LogDebugSafe("Sql destroyed CVars table successfully.");
    return CTR_OK;
  }
  /* ----------------------------------------------------------------------- */
  CreateTableResult CVarCreateTable(void)
  { // If table already exists then return success already
    if(IsTableExist(strvCVTable)) return CTR_OK_ALREADY;
    // Create the SQL table for our settings if it does not exist
    if(Execute(StrFormat("CREATE table `$`" // Table name
         "(`$` TEXT UNIQUE NOT NULL,"    // Variable name
          "`$` INTEGER DEFAULT 0,"       // Value flags (crypt,comp,etc.)
          "`$` TEXT)",                   // Value (any type allowed)
         strvCVTable, strCVKeyColumn, strCVFlagsColumn, strCVValueColumn)))
    { // Write error in console and return failure
      cLog->LogWarningExSafe("Sql failed to create CVars table because $ ($)!",
        GetErrorStr(), GetError());
      return CTR_FAIL;
    } // Write success in console and return success
    cLog->LogDebugSafe("Sql created CVars table successfully.");
    return CTR_OK;
  }
  /* ----------------------------------------------------------------------- */
  CreateTableResult LuaCacheDropTable(void)
  { // If table exists return that it already exists
    if(GetRecordCount(strvLCTable) == StdMaxSizeT) return CTR_OK_ALREADY;
    // Drop the SQL table and if failed?
    if(DropTable(strvLCTable))
    { // Write error in console and return failure
      cLog->LogWarningExSafe(
        "Sql failed to destroy cache table because $ ($)!",
        GetErrorStr(), GetError());
      return CTR_FAIL;
    } // Write success in console and return success
    cLog->LogDebugSafe("Sql destroyed cache table successfully.");
    return CTR_OK;
  }
  /* ----------------------------------------------------------------------- */
  CreateTableResult LuaCacheCreateTable(void)
  { // If table already exists then return success already
    if(IsTableExist(strvLCTable)) return CTR_OK_ALREADY;
    // Create the SQL table for our settings if it does not exist
    if(Execute(StrFormat("CREATE table `$`" // Table name
         "(`$` INTEGER UNIQUE NOT NULL,"    // Code CRC value
          "`$` INTEGER NOT NULL,"           // Code update timestamp
          "`$` TEXT UNIQUE NOT NULL,"       // Code eference
          "`$` TEXT NOT NULL)",             // Code binary
         strvLCTable, strvLCCRCColumn, strvLCTimeColumn, strvLCRefColumn,
         strvLCCodeColumn
    )))
    { // Write error in console and return failure
      cLog->LogWarningExSafe("Sql failed to create cache table because $ ($)!",
        GetErrorStr(), GetError());
      return CTR_FAIL;
    } // Write success in console and return success
    cLog->LogDebugSafe("Sql created cache table successfully.");
    return CTR_OK;
  }
  /* ----------------------------------------------------------------------- */
  CreateTableResult LuaCacheRebuildTable(void)
    { LuaCacheDropTable(); return LuaCacheCreateTable(); }
  /* ----------------------------------------------------------------------- */
  bool CVarCommitData(const string &strVar,
    const SqlCVarDataFlagsConst &scvdfcFlags, const int iType,
    const char*const cpData, const size_t stLength)
  { // Try to write the specified cvar and if failed?
    if(Execute(StrFormat(
           "INSERT or REPLACE into `$`(`$`,`$`,`$`) VALUES(?,?,?)",
           strvCVTable, strCVKeyColumn, strCVFlagsColumn, strCVValueColumn),
         strVar, scvdfcFlags.FlagGet<int>(),
         cpData, stLength, iType))
    { // Log the warning and return failure
      cLog->LogWarningExSafe("Sql failed to commit CVar '$' (T:$;ST:$;B:$) "
                             "because $ ($)!",
        strVar, scvdfcFlags.FlagGet(), iType, stLength, GetErrorStr(),
        GetError());
      return false;
    } // Report and return success
    cLog->LogDebugExSafe("Sql commited CVar '$' (T:$;ST:$;B:$).",
      strVar, scvdfcFlags.FlagGet(), iType, stLength);
    return true;
  }
  /* ----------------------------------------------------------------------- */
  bool CVarCommitString(const string &strVar, const string &strVal)
    { return CVarCommitData(strVar, SD_NONE,
        SQLITE_TEXT, strVal.data(), strVal.length()); }
  /* ----------------------------------------------------------------------- */
  bool CVarCommitBlob(const string &strVar, const MemConst &mcSrc)
    { return CVarCommitData(strVar, SD_ENCRYPTED,
        SQLITE_BLOB, mcSrc.MemPtr<char>(), mcSrc.MemSize()); }
  /* ----------------------------------------------------------------------- */
  PurgeResult CVarPurgeData(const char*const cpKey, const size_t stKey)
  { // Try to purge the cvar from the database and if it failed?
    if(!ExecuteAndSuccess("DELETE from `C` WHERE `K`=?", stKey, cpKey))
    { // Log the warning and return failure
      cLog->LogWarningExSafe("Sql failed to purge CVar '$' because $ ($)!",
        cpKey, GetErrorStr(), GetError());
      return PR_FAIL;
    } // Just return if no records were affected
    if(!Affected()) return PR_OK_NC;
    // Report and return success
    cLog->LogDebugExSafe("Sql purged CVar '$' from database.", cpKey);
    return PR_OK;
  }
  /* ----------------------------------------------------------------------- */
  PurgeResult CVarPurge(const string &strVar)
    { return CVarPurgeData(strVar.c_str(), strVar.length()); }
  /* -- DeInit ------------------------------------------------------------- */
  void DeInit(void)
  { // Ignore if no handle to deinit
    if(!sqlDB) return;
    // Log deinitialisation
    cLog->LogDebugExSafe("Sql database '$' is closing...", IdentGet());
    // Finalise statements and if we found orphans
    if(const size_t stOrphans = Finalise())
      cLog->LogWarningExSafe("Sql finalised $ orphan statements.", stOrphans);
    // Check if database should be deleted, if it should'nt?
    switch(const ADResult adrResult = CanDatabaseBeDeleted())
    { // Deletion is ok?
      case ADR_OK_NO_TABLES:
      case ADR_OK_NO_RECORDS:
        // Print the result
        cLog->LogDebugExSafe("Sql will delete empty database because $ ($).",
          ADResultToString(adrResult), adrResult);
        // Close the database
        DoClose();
        // Try to delete it and log the result
        if(!DirFileUnlink(IdentGet()))
          cLog->LogWarningExSafe(
            "Sql empty database file '$' could not be deleted! $",
            IdentGet(), StrFromErrNo());
        // Delete failure
        else cLog->LogDebugExSafe("Sql empty database file '$' deleted.",
          IdentGet());
        // Done
        break;
      // Deletion is not ok?
      default:
        // Print the result
        cLog->LogDebugExSafe("Sql won't delete database because $ ($).",
          ADResultToString(adrResult), adrResult);
        // Sqlite docs recommend us to optimise before closing.
        Pragma("optimize");
        // Close the database
        DoClose();
        // Done
        break;
    } // Reset results
    Reset();
    // Reset database name
    IdentClear();
    // Temporary database boolean
    FlagClear(SF_ISTEMPDB);
    // Clear private key
    cCrypt->SetDefaultPrivateKey();
  }
  /* -- Load schema version ------------------------------------------------ */
  void LoadSchemaVersion(void)
  { // If table exists create a new table
    if(!IsTableExist(strvSKeyTable)) goto NewKeyNoDrop;
    // Check record count
    switch(GetRecordCount(strvSKeyTable))
    { // Error reading table?
      case StdMaxSizeT:
      { // Log failure and create a new private key
        cLog->LogWarningExSafe(
          "Sql failed to read schema table because $ ($<$>)!",
          GetErrorStr(), GetErrorAsIdString(), GetError());
        goto NewKey;
      } // Any other value is not allowed
      default:
      { // Read schema version key and if failed?
        if(Execute(StrFormat("SELECT `$` from `$` WHERE `$`=?",
          strvSValueColumn, strvSKeyTable, strvSIndexColumn), strvSVersionKey))
        { // Log failure and create a new private key
          cLog->LogWarningExSafe(
            "Sql failed to read schema version because $ ($<$>)!",
            GetErrorStr(), GetErrorAsIdString(), GetError());
          goto NewKey;
        } // If not enough results?
        if(srKeys.empty())
        { // Log failure and create a new private key
          cLog->LogWarningSafe("Sql did find any schema version record!");
          goto NewKey;
        } // Get reference to version and check its valid
        const SqlData &sdRef = srKeys.front().cbegin()->second;
        if(sdRef.iType != SQLITE_INTEGER)
        { // Log failure and create new private key table
          cLog->LogWarningExSafe(
            "Sql schema version record type of $ not SQLITE_INTEGER!",
            sdRef.iType);
          goto NewKey;
        } // Read record data. It must be 8 bytes. If it isnt?
        if(sdRef.MemSize() != sizeof(sqlite3_int64))
        { // Log failure and create new private key table
          cLog->LogWarningExSafe(
            "Sql schema version record size of $ not correct size of $!",
            sdRef.MemSize(), sizeof(sqlite3_int64));
          break;
        } // Convert to integer and compare
        switch(const sqlite3_int64 qLVersion =
          sdRef.MemReadInt<sqlite3_int64>())
        { // Is our correct version?
          case qVersion:
            // Notify log of correct version
            cLog->LogDebugExSafe("Sql schema version $ verified!", qLVersion);
            break;
          // Is different version?
          default:
            // Notify log of version difference and create new version key
            cLog->LogWarningExSafe("Sql schema version of $ different from "
              "engine version of $!", qLVersion, qVersion);
            // Set updated to the new version
            goto NewKeyNoDropInsert;
        } // Done
        return;
      }
    } // Could not read new private key so setup new private key
    NewKey:
    // Try to drop the original private key table
    if(DropTable(strvSKeyTable))
      cLog->LogWarningExSafe(
        "Sql failed to drop schema table because $ ($<$>)!",
        GetErrorStr(), GetErrorAsIdString(), GetError());
    // Skipped dropping non-existant table
    NewKeyNoDrop:
    // Now try to create the table that holds the private key and if failed?
    if(!ExecuteAndSuccess(
         StrFormat("CREATE table `$`(`$` TEXT UNIQUE NOT NULL,"
                                    "`$` INTEGER NOT NULL)",
        strvSKeyTable, strvSIndexColumn, strvSValueColumn)))
      cLog->LogWarningExSafe(
        "Sql failed to create schema table because $ ($<$>)!",
        GetErrorStr(), GetErrorAsIdString(), GetError());
    // Skipped drop and recreation of existing table
    NewKeyNoDropInsert:
    // Now try to create the table that holds the private key and if failed?
    if(!ExecuteAndSuccess(StrFormat(
        "INSERT or REPLACE into `$`(`$`,`$`) VALUES(?,?)",
        strvSKeyTable, strvSIndexColumn, strvSValueColumn), strvSVersionKey,
        qVersion))
      cLog->LogWarningExSafe(
        "Sql failed to insert schema version record because $ ($<$>)!",
        GetErrorStr(), GetErrorAsIdString(), GetError());
    // Notify log of version update
    else cLog->LogInfoExSafe("Sql schema version updated to $.", qVersion);
  }
  /* -- Create new private key --------------------------------------------- */
  bool CreatePrivateKeyNoDrop(void)
  { // Try to create the table that holds the private key and if failed?
    if(Execute(StrFormat("CREATE table `$`(`$` INTEGER NOT NULL,"
                                          "`$` INTEGER NOT NULL)",
      strvPKeyTable, strvPIndexColumn, strvPValueColumn)))
    { // Log failure and return
      cLog->LogWarningExSafe(
        "Sql failed to create key table because $ ($<$>)!",
        GetErrorStr(), GetErrorAsIdString(), GetError());
      return false;
    } // Prepare statement to insert values into sql
    const string strInsert{ StrFormat("INSERT into `$`(`$`,`$`) VALUES(?,?)",
      strvPKeyTable, strvPIndexColumn, strvPValueColumn) };
    // For each key part to write
    for(size_t stIndex = 0; stIndex < cCrypt->pkKey.qKeys.size(); ++stIndex)
    { // Get private key value and if failed?
      const sqlite3_int64 qVal =
        static_cast<sqlite3_int64>(cCrypt->ReadPrivateKey(stIndex));
      if(!ExecuteAndSuccess(strInsert,
        static_cast<sqlite3_int64>(stIndex), qVal))
      { // Log failure and return failure
        cLog->LogWarningExSafe(
          "Sql failed to write key table at $ because $ ($<$>)!",
          stIndex, GetErrorStr(), GetErrorAsIdString(), GetError());
        return false;
      }
    } // Log result and return success
    cLog->LogDebugSafe(
      "Sql wrote a new key table into database successfully!");
    return true;
  }
  /* -- Create new private key (drop first) -------------------------------- */
  bool CreatePrivateKey(void)
  { // Generate a new private key
    cCrypt->ResetPrivateKey();
    // Try to drop the original private key table
    if(DropTable(strvPKeyTable))
      cLog->LogWarningExSafe("Sql failed to drop key table because $ ($<$>)!",
        GetErrorStr(), GetErrorAsIdString(), GetError());
    // Now create the private key table
    return CreatePrivateKeyNoDrop();
  }
  /* -- Load private key --------------------------------------------------- */
  void LoadPrivateKey(void)
  { // If table exists create a new table
    if(!IsTableExist(strvPKeyTable)) CreatePrivateKeyNoDrop();
    // Check record count
    switch(const size_t stCount = GetRecordCount(strvPKeyTable))
    { // Error reading table?
      case StdMaxSizeT:
      { // Log failure and create a new private key
        cLog->LogWarningExSafe(
          "Sql failed to read key table because $ ($<$>)!",
          GetErrorStr(), GetErrorAsIdString(), GetError());
        CreatePrivateKey();
        return;
      } // Any other value is not allowed
      default:
      { // Log failure and create a new private key
        cLog->LogWarningExSafe("Sql key table corrupt ($ != $)!",
          stCount, cCrypt->pkKey.qKeys.size());
        CreatePrivateKey();
        return;
      } // Read enough entries?
      case Crypt::stPkTotalCount:
      { // Read keys and if failed?
        if(Execute(StrFormat("SELECT `$` from `$` ORDER BY `$` ASC",
          strvPValueColumn, strvPKeyTable, strvPIndexColumn)))
        { // Log failure and create a new private key
          cLog->LogWarningExSafe(
            "Sql failed to read key table because $ ($<$>)!",
            GetErrorStr(), GetErrorAsIdString(), GetError());
          CreatePrivateKey();
          return;
        } // If not enough results?
        if(srKeys.size() != cCrypt->pkKey.qKeys.size())
        { // Log failure and create a new private key
          cLog->LogWarningExSafe(
            "Sql read only $ of $ key table records!",
            srKeys.size(), cCrypt->pkKey.qKeys.size());
          CreatePrivateKey();
          return;
        } // Record index number
        size_t stIndex = 0;
        // Update the private key parts
        for(const SqlRecordsMap &rItem : srKeys)
        { // Get reference to data item and if it's not the right type?
          const SqlData &sdRef = rItem.cbegin()->second;
          if(sdRef.iType != SQLITE_INTEGER)
          { // Log failure and create new private key table
            cLog->LogWarningExSafe(
              "Sql key table at column $ not correct type ($)!",
              stIndex, sdRef.iType);
            CreatePrivateKey();
            return;
          } // Read record data. It must be 8 bytes. If it isnt?
          if(sdRef.MemSize() != sizeof(sqlite3_int64))
          { // Log failure and create new private key table
            cLog->LogWarningExSafe(
              "Sql key table at column $ expected $ not $ bytes!",
              stIndex, sizeof(sqlite3_int64), sdRef.MemSize());
            CreatePrivateKey();
            return;
          } // Read in the value to the private key table
          cCrypt->WritePrivateKey(stIndex++,
            static_cast<uint64_t>(sdRef.MemReadInt<sqlite3_int64>()));
        } // Log result and return
        cLog->LogDebugSafe(
          "Sql loaded key table from database successfully!");
      }
    }
  }
  /* -- Init --------------------------------------------------------------- */
  bool Init(const string &strPrefix)
  { // If named database is already opened
    if(sqlDB && strPrefix == IdentGet())
    { // Put in console and return failure
      cLog->LogWarningExSafe("Sql skipped reinit of '$'.", strPrefix);
      return false;
    } // Log initialisation. Set filename using memory db name if empty
    const string &strDb = strPrefix.empty() ? strMemoryDBName : strPrefix;
    cLog->LogDebugExSafe("Sql initialising database '$'...", strDb);
    // Open database with a temporary sqlite handle so if the open fails,
    // the old one stays intact. The cast of the sqlDBtemp for the last
    // parameter (VFS) that resolves to 'nullptr' is just to shut CppCheck up
    // with a false positive.
    sqlite3 *sqlDBtemp = nullptr;
    if(const int iCode = sqlite3_open_v2(strDb.c_str(), &sqlDBtemp,
         SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_FULLMUTEX |
         SQLITE_OPEN_SHAREDCACHE, reinterpret_cast<char*>(sqlDBtemp)))
    { // Log error result
      cLog->LogWarningExSafe("Sql could not open '$' because $ ($)!", strDb,
        GetErrorStr(), iCode);
      // Sql open failed so 'sqlDBtemp' stays NULL.
      return false;
    } // Set to this database and set name
    sqlDB = sqlDBtemp;
    IdentSet(strDb);
    // Load schema version then private key
    LoadSchemaVersion();
    LoadPrivateKey();
    // Log successfull initialisation and return success
    cLog->LogInfoExSafe("Sql database '$' initialised.", IdentGet());
    return true;
  }
  /* -- Destructor ---------------------------------------------- */ protected:
  DTORHELPER(~Sql, DeInit(); sqlite3_shutdown())
  /* -- Constructor -------------------------------------------------------- */
  Sql(void) :                          // No parameters
    /* -- Initialisers ----------------------------------------------------- */
    SqlFlags(SF_NONE),                 // No sql flags (loaded externally)
    elStrings{{                        // Init sqlite error strings list
      "OK",       /*00-01*/ "ERROR",     "INTERNAL", /*02-03*/ "PERM",
      "ABORT",    /*04-05*/ "BUSY",      "LOCKED",   /*06-07*/ "NOMEM",
      "READONLY", /*08-09*/ "INTERRUPT", "IOERR",    /*10-11*/ "CORRUPT",
      "NOTFOUND", /*12-13*/ "FULL",      "CANTOPEN", /*14-15*/ "PROTOCOL",
      "EMPTY",    /*16-17*/ "SCHEMA",    "TOOBIG",   /*18-19*/ "CONSTRAINT",
      "MISMATCH", /*20-21*/ "MISUSE",    "NOLFS",    /*22-23*/ "AUTH",
      "FORMAT",   /*24-25*/ "RANGE",     "NOTADB"    /*26---*/
    }, "UNKNOWN"},                     // Unknown sqlite error
    adrlStrings{{                      // Init 'can db be deleted' strings list
      "no tables",             /* 0-1 */ "no records",
      "temporary database",    /* 2-3 */ "option denied",
      "error reading tables",  /* 4-5 */ "tables exist",
      "error reading records", /* 6-7 */ "records exist"
    }},                                // Initialised 'can db be deleted' strs
    sqlDB(nullptr),                    // No sql database handle yet
    iError(sqlite3_initialize()),      // Initialise sqlite and store error
    uiQueryRetries(3),                 // Initially 3 retries
    cdRetry{milliseconds{1000}},       // Initially wait 1 second per retry
    strMemoryDBName{ ":memory:" },     // Create a memory database by default
    strCVKeyColumn{ "K" },             // Init name of cvars 'key' column
    strCVFlagsColumn{ "F" },           // Init name of cvars 'flags' column
    strCVValueColumn{ "V" },           // Init name of cvars 'value' column
    strvCVTable{ "C" },                // Init name of cvar table
    strvLCTable{ "L" },                // Init name of lua cache table
    strvLCCRCColumn{ "C" },            // Init name of lua cache 'crc' column
    strvLCTimeColumn{ "T" },           // Init name of lua cache 'time' column
    strvLCRefColumn{ "R" },            // Init name of lua cache 'ref' column
    strvLCCodeColumn{ "D" },           // Init name of lua cache 'data' column
    strvOn{ "ON" },                    // Init "ON" static string
    strvOff{ "OFF" },                  // Init "OFF" static string
    strvPKeyTable{ "K" },              // Init name of pvt key table
    strvPIndexColumn{ "I" },           // Init name of pvt key 'index' column
    strvPValueColumn{ "K" },           // Init name of pvt key 'value' column
    strvSKeyTable{ "S" },              // Init name of schema table
    strvSIndexColumn{ "K" },           // Init name of schema 'key' column
    strvSValueColumn{ "V" },           // Init name of schema 'value' column
    strvSVersionKey{ "V" }             // Init name of version # key in schema
  /* -- Code --------------------------------------------------------------- */
  { // Set global pointer to static class
    cSql = this;
    // Throw error if sqlite startup failed
    if(IsError()) XC("Failed to initialise SQLite!",
                     "Error", GetError(), "Reason", GetErrorAsIdString());
  }
  /* -- Set a pragma on or off (used only with cvar callbacks) ----- */ public:
  CVarReturn PragmaOnOff(const string &strVar, const bool bState)
    { Pragma(strVar, bState ? strvOn : strvOff); return ACCEPT; }
  /* -- Set retry count ---------------------------------------------------- */
  CVarReturn RetryCountModified(const unsigned int uiCount)
    { return CVarSimpleSetInt(uiQueryRetries, uiCount); }
  /* -- Set retry suspend time --------------------------------------------- */
  CVarReturn RetrySuspendModified(const uint64_t uqMilliseconds)
    { return CVarSimpleSetIntNLG(cdRetry, milliseconds{ uqMilliseconds },
        milliseconds{0}, milliseconds{1000}); }
  /* -- Modify delete empty database permission ---------------------------- */
  CVarReturn DeleteEmptyDBModified(const bool bState)
    { FlagSetOrClear(SF_DELETEEMPTYDB, bState); return ACCEPT; }
  /* -- sql_temp_store cvar was modified ----------------------------------- */
  CVarReturn TempStoreModified(const string &strFile, string&)
  { // Prevent manipulating the query
    if(strFile.find(' ') != StdNPos || strFile.find(';') != StdNPos)
      return DENY;
    // Do the query
    Pragma("temp_store", strFile);
    // Success
    return ACCEPT;
  }
  /* -- sql_synchronous cvar was modified ---------------------------------- */
  CVarReturn SynchronousModified(const bool bState)
    { return PragmaOnOff("synchronous", bState); }
  /* -- sql_journal_mode cvar was modified --------------------------------- */
  CVarReturn JournalModeModified(const bool bState)
    { return PragmaOnOff("journal_mode", bState); }
  /* -- sql_auto_vacuum cvar was modified ---------------------------------- */
  CVarReturn AutoVacuumModified(const bool bState)
    { return PragmaOnOff("auto_vacuum", bState); }
  /* -- sql_auto_vacuum cvar was modified ---------------------------------- */
  CVarReturn ForeignKeysModified(const bool bState)
    { return PragmaOnOff("foreign_keys", bState); }
  /* -- sql_inc_vacuum cvar was modified ----------------------------------- */
  CVarReturn IncVacuumModified(const uint64_t qwVal)
    { Pragma(StrFormat("incremental_vacuum($)", qwVal)); return ACCEPT; }
  /* -- sql_db cvar was modified ------------------------------------------- */
  CVarReturn UdbFileModified(const string &strFile, string &strVar)
  { // Save original working directory and restore it when leaving scope
    const DirSaver dsSaver;
    // If the user did not specify anything?
    if(strFile.empty())
    { // Switch to executable directory
      if(!DirSetCWD(cSystem->ENGLoc())) return DENY;
      // Set database file with executable
      strVar = StrAppend(cSystem->ENGFile(), "." UDB_EXTENSION);
    } // If a memory database was requested? We allow it!
    else if(strFile == strMemoryDBName) strVar = strFile;
    // If the user specified something?
    else
    { // Switch to original startup directory
      if(!DirSetCWD(cCmdLine->CmdLineGetStartupCWD())) return DENY;
      // Use theirs, but force UDB extension
      strVar = StrAppend(strFile, "." UDB_EXTENSION);
    } // Initialise the db and if succeeded?
    if(Init(strVar))
    { // Set full path name of the database
      SqlInitOK: strVar = StdMove(PathSplit{ strVar, true }.strFull);
      // Success
      return ACCEPT_HANDLED;
    } // If we have a persistant directory?
    if(cCmdLine->CmdLineIsHome())
    { // Set a new filename in the users home directory
      strVar = cCmdLine->CmdLineGetHome(PathSplit{ strVar }.strFileExt);
      // Try opening that and if succeeded then return success
      if(Init(strVar)) goto SqlInitOK;
    } // Use memory database instead
    strVar = strMemoryDBName;
    // Now open the memory database which should ALWAYS succeed.
    return Init(strVar) ? ACCEPT_HANDLED : DENY;
  }
};/* ----------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
