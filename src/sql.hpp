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
static const sqlite3_destructor_type fcbSqliteTransient =
  reinterpret_cast<sqlite3_destructor_type>(-1);
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* -- Public typedefs ------------------------------------------------------ */
BUILD_FLAGS(SqlCVarData,               // Sql cvar data types
  /* -- (Note: Don't ever change these around) ----------------------------- */
  SD_NONE                   {Flag(0)}, // No flags for this cvar in db?
  SD_ENCRYPTED              {Flag(1)}  // The data value was encrypted?
);/* ----------------------------------------------------------------------- */
BUILD_FLAGS(Sql,                       // Sql flags classes
  /* ----------------------------------------------------------------------- */
  SF_NONE                   {Flag(0)}, // No settings?
  SF_ISTEMPDB               {Flag(1)}, // Is temporary database?
  SF_DELETEEMPTYDB          {Flag(2)}  // Delete empty databases?
);/* ----------------------------------------------------------------------- */
struct SqlData :                       // Query response data item class
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
    {}
  /* -- Move constructor --------------------------------------------------- */
  SqlData(SqlData &&sdOther) :
    /* -- Initialisers ----------------------------------------------------- */
    SqlData{ StdMove(sdOther), sdOther.iType }
    /* -- No code ---------------------------------------------------------- */
    {}
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
    ADR_OK_NO_TABLES,                  // [0] No tables exist (delete ok)
    ADR_OK_NO_RECORDS,                 // [1] No records exist (delete ok)
    /* --------------------------------------------------------------------- */
    ADR_ERR,                           // [2] Min error value (delete denied)
    /* --------------------------------------------------------------------- */
    ADR_ERR_TEMP_DB = ADR_ERR,         // [2] Is temporary database?
    ADR_ERR_DENY_OPTION,               // [3] Delete not allowed by guest?
    ADR_ERR_LU_TABLE,                  // [4] Error looking up table?
    ADR_ERR_TABLES_EXIST,              // [5] Tables exist?
    ADR_ERR_LU_RECORD,                 // [6] Error looking up records?
    ADR_ERR_RECORDS_EXIST,             // [7] Records exist?
    /* --------------------------------------------------------------------- */
    ADR_MAX                            // [8] Maximum number of result types
  };/* --------------------------------------------------------------------- */
  enum PurgeResult                     // Result to a purge request
  { /* --------------------------------------------------------------------- */
    PR_FAIL,                           // [0] Sql call failed with error
    PR_OK,                             // [1] Sql call succeeded with changes
    PR_OK_NC,                          // [2] Sql call succeeded but no changes
  };/* --------------------------------------------------------------------- */
  enum CreateTableResult               // Create table result
  { /* --------------------------------------------------------------------- */
    CTR_FAIL,                          // [0] Sql create table failed
    CTR_OK,                            // [1] Sql call commited the variable
    CTR_OK_ALREADY,                    // [2] Sql table already exists
  };/* -- Private typedefs ---------------------------------------- */ private:
  typedef IdList<SQLITE_NOTICE> ErrorList; // Sqlite errors strings list
  typedef IdList<ADR_MAX> ADRList;         // AD result strings list
  /* -- Schema version ----------------------------------------------------- */
  static constexpr const sqlite3_int64 llVersion = 1; // Expected schema ver
  /* -- Variables ---------------------------------------------------------- */
  const ErrorList  elStrings;          // Sqlite error strings list
  const ADRList    adrlStrings;        // Can db be deleted strings list
  sqlite3         *sqlDB;              // Pointer to SQL context
  int              iError;             // Last error code
  SqlResult        srKeys;             // Last execute data result
  unsigned int     uiQueryRetries;     // Times to retry query before failing
  ClkDuration      cdRetry,            // Sleep for this time when retrying
                   cdQuery;            // Last query execution time
  /* -- Tables ----------------------------------------------------- */ public:
  const string     strMemoryDBName,    // Memory only database (no disk)
                   strCVKeyColumn,     // Name of cvar 'key' column
                   strCVFlagsColumn,   // Name of cvar 'flags' column
                   strCVValueColumn;   // Name of cvar 'value' column
  const string_view strvCVTable,       // Name of cvar table
                   strvLCTable,        // Name of lua cache table
                   strvLCCRCColumn,    // Name of lua cache 'crc' column
                   strvLCTimeColumn,   // Name of lua cache 'time' column
                   strvLCRefColumn,    // Name of lua cache 'ref' column
                   strvLCCodeColumn,   // Name of lua cache 'data' column
                   strvOn, strvOff,    // "ON" and "OFF" strings
                   strvPKeyTable,      // Name of pvt key table
                   strvPIndexColumn,   // Name of pvt key 'index' column
                   strvPValueColumn,   // Name of pvt key 'value' column
                   strvSKeyTable,      // Name of schema table
                   strvSIndexColumn,   // Name of schema 'index' column
                   strvSValueColumn,   // Name of schema 'value' column
                   strvSVersionKey;    // Name of schema version record name
  /* -- Convert sql error id to string ---------------------------- */ private:
  const string_view &SqlResultToString(const int iCode) const
    { return elStrings.Get(iCode); }
  /* -- Convert ADR id to string ------------------------------------------- */
  const string_view &SqlADResultToString(const ADResult adrResult) const
    { return adrlStrings.Get(adrResult); }
  /* -- Close the database ------------------------------------------------- */
  void SqlDoClose()
  { // Number of retries needed to close the database
    unsigned int uiRetries;
    // Wait until the database can be closed
    for(uiRetries = 0; sqlite3_close(sqlDB) == SQLITE_BUSY; ++uiRetries)
    { // Keep trying to clean up if we can (this shouldn't really happen).
      SqlFinalise();
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
  void SqlDoPair(SqlRecordsMap &srmMap, const int iType,
    const char*const cpKey, const void*const vpPtr, const size_t stSize)
  { // Generate the memory block with the specified data.
    Memory mData{ stSize, vpPtr };
    // Generate the pair value and move the memory into it.
    SqlData sdRef{ StdMove(mData), iType };
    // Insert a new pair moving key and value across.
    srmMap.emplace(make_pair(cpKey, StdMove(sdRef)));
  }
  /* -- Build a new node from an integral value ---------------------------- */
  template<typename T>void SqlDoPair(SqlRecordsMap &srmMap, const int iType,
    const char*const cpKey, const T tVal)
      { SqlDoPair(srmMap, iType, cpKey, &tVal, sizeof(tVal)); }
  /* -- Set error code ----------------------------------------------------- */
  void SqlSetError(const int iCode) { iError = iCode; }
  /* -- Compile the sql command and store output rows ---------------------- */
  void SqlDoStep(sqlite3_stmt*const stmtData)
  { // Retry count
    unsigned int uiRetries = 0;
    // Until we're done with all the data
    for(SqlSetError(sqlite3_step(stmtData));
        SqlIsErrorNotEqual(SQLITE_DONE);
        SqlSetError(sqlite3_step(stmtData)))
    { // Check status
      switch(SqlGetError())
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
            SqlDoPair(srmMap, iType, sqlite3_column_name(stmtData, iCol),
              sqlite3_column_int64(stmtData, iCol));
            break;
          } // 64-bit IEEE float?
          case SQLITE_FLOAT:
          { // Insert the double into the vector
            SqlDoPair(srmMap, iType, sqlite3_column_name(stmtData, iCol),
              sqlite3_column_double(stmtData, iCol));
            break;
          } // Raw data?
          case SQLITE_BLOB:
          { // Copy blob to a memory block if there's something to copy
            SqlDoPair(srmMap, iType, sqlite3_column_name(stmtData, iCol),
              sqlite3_column_blob(stmtData, iCol),
              static_cast<size_t>(sqlite3_column_bytes(stmtData, iCol)));
            break;
          } // Text?
          case SQLITE_TEXT:
          { // Copy to a memory block if there is something to copy
            SqlDoPair(srmMap, iType, sqlite3_column_name(stmtData, iCol),
              sqlite3_column_text(stmtData, iCol),
              static_cast<size_t>(sqlite3_column_bytes(stmtData, iCol)));
            break;
          } // NULL?
          case SQLITE_NULL:
          { // Copy to a memory block if there is something to copy
            SqlDoPair(srmMap, iType, sqlite3_column_name(stmtData, iCol),
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
    SqlSetError(SQLITE_OK);
  }
  /* -- Can database be deleted, no point keeping if it's empty! ----------- */
  ADResult SqlCanDatabaseBeDeleted()
  { // No if this is a temporary database as theres nothing to delete.
    if(FlagIsSet(SF_ISTEMPDB)) return ADR_ERR_TEMP_DB;
    // No if we are not allowed to delete the database
    if(FlagIsClear(SF_DELETEEMPTYDB)) return ADR_ERR_DENY_OPTION;
    // Compare how many tables there are in the database...
    switch(SqlGetRecordCount("sqlite_master",
      " AS tables WHERE `type`='table'"))
    { // Error occured? Don't delete the database!
      case StdMaxSizeT: return ADR_ERR_LU_TABLE;
      // No tables? Delete the database!
      case 0: return ADR_OK_NO_TABLES;
      // Only one or two tables (key and/or cvars table)?
      case 1: case 2:
        // Get number of records in the cvars table
        switch(SqlGetRecordCount(strvCVTable))
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
  bool SqlDoExecuteParamCheckCommit(sqlite3_stmt*const stmtData, int &iCol,
    const int iMax)
  { // Don't do anything if there is an error
    if(SqlIsError()) return false;
    // If we're not at the maximum parameters yet? Ok to continue
    if(iCol < iMax) { ++iCol; return true; }
    // Do step and tell caller to stop if failed
    SqlDoStep(stmtData);
    if(SqlIsError()) return false;
    // Reset bindings so we can insert another
    SqlSetError(sqlite3_reset(stmtData));
    if(SqlIsError()) return false;
    // Reset column id
    iCol = 0;
    // Success
    return true;
  }
  /* -- No more calls so just compile what we have left -------------------- */
  void SqlDoExecuteParam(const int iCol, const int iMax,
    sqlite3_stmt*const stmtData)
  { // Return if there are no columns currently queued
    if(!iCol) return;
    // I forgot to specify all the parameters properly
    XC("Internal error: Not enough Sql parameters sent!",
      "Statement", sqlite3_sql(stmtData), "Expected", iMax, "Actual", iCol);
  }
  /* -- Constructor that stores a 64-bit integer --------------------------- */
  template<typename ...VarArgs>
    void SqlDoExecuteParam(int &iCol, const int iMax,
      sqlite3_stmt*const stmtData, const sqlite_int64 llValue,
      VarArgs &&...vaArgs)
  { // Log the parameter
    cLog->LogDebugExSafe("- Arg #$<Int64/Integer> = $ <$0x$>.",
      iCol, llValue, hex, llValue);
    // Process as integer then pass next arguments
    SqlSetError(sqlite3_bind_int64(stmtData, iCol, llValue));
    if(SqlDoExecuteParamCheckCommit(stmtData, iCol, iMax))
      SqlDoExecuteParam(iCol, iMax, stmtData, StdForward<VarArgs>(vaArgs)...);
  }
  /* -- Initialise as double of type SQLITE_FLOAT -------------------------- */
  template<typename ...VarArgs>
    void SqlDoExecuteParam(int &iCol, const int iMax,
      sqlite3_stmt*const stmtData, const double dValue, VarArgs &&...vaArgs)
  { // Log the parameter
    cLog->LogDebugExSafe("- Arg #$<Double/Float> = $$.", iCol, fixed, dValue);
    // Process as integer then pass next arguments
    SqlSetError(sqlite3_bind_double(stmtData, iCol, dValue));
    if(SqlDoExecuteParamCheckCommit(stmtData, iCol, iMax))
      SqlDoExecuteParam(iCol, iMax, stmtData, StdForward<VarArgs>(vaArgs)...);
  }
  /* -- Constructor that stores similar types of integer ------------------- */
  template<typename ...VarArgs>
    void SqlDoExecuteParam(int &iCol, const int iMax,
      sqlite3_stmt*const stmtData, int iValue, VarArgs &&...vaArgs)
  { // Log the parameter
    cLog->LogDebugExSafe("- Arg #$<Int/Int> = $ <$0x$>.",
      iCol, iValue, hex, iValue);
    // Process as integer then pass next arguments
    SqlSetError(sqlite3_bind_int64(stmtData, iCol,
      static_cast<sqlite_int64>(iValue)));
    if(SqlDoExecuteParamCheckCommit(stmtData, iCol, iMax))
      SqlDoExecuteParam(iCol, iMax, stmtData, StdForward<VarArgs>(vaArgs)...);
  }
  /* -- Constructor that stores similar types of integer ------------------- */
  template<typename ...VarArgs>
    void SqlDoExecuteParam(int &iCol, const int iMax,
      sqlite3_stmt*const stmtData, const unsigned int uiValue,
      VarArgs &&...vaArgs)
  { // Log the parameter
    cLog->LogDebugExSafe("- Arg #$<UInt/Int> = $ <$0x$>.",
      iCol, uiValue, hex, uiValue);
    // Process as integer then pass next arguments
    SqlSetError(sqlite3_bind_int64(stmtData, iCol,
      static_cast<sqlite_int64>(uiValue)));
    if(SqlDoExecuteParamCheckCommit(stmtData, iCol, iMax))
      SqlDoExecuteParam(iCol, iMax, stmtData, StdForward<VarArgs>(vaArgs)...);
  }
  /* -- Constructor that stores similar types of integer ------------------- */
  template<typename ...VarArgs>
    void SqlDoExecuteParam(int &iCol, const int iMax,
      sqlite3_stmt*const stmtData, const long lValue, VarArgs &&...vaArgs)
  { // Log the parameter
    cLog->LogDebugExSafe("- Arg #$<Long/Int> = $ <$0x$>.",
      iCol, lValue, hex, lValue);
    // Process as integer then pass next arguments
    SqlSetError(sqlite3_bind_int64(stmtData, iCol,
      static_cast<sqlite_int64>(lValue)));
    if(SqlDoExecuteParamCheckCommit(stmtData, iCol, iMax))
      SqlDoExecuteParam(iCol, iMax, stmtData, StdForward<VarArgs>(vaArgs)...);
  }
  /* -- Constructor that stores similar types of integer ------------------- */
  template<typename ...VarArgs>
    void SqlDoExecuteParam(int &iCol, const int iMax,
      sqlite3_stmt*const stmtData, const short sValue, VarArgs &&...vaArgs)
  { // Log the parameter
    cLog->LogDebugExSafe("- Arg #$<Short/Int> = $ <$0x$>.",
      iCol, sValue, hex, sValue);
    // Process as integer then pass next arguments
    SqlSetError(sqlite3_bind_int64(stmtData, iCol,
      static_cast<sqlite_int64>(sValue)));
    if(SqlDoExecuteParamCheckCommit(stmtData, iCol, iMax))
      SqlDoExecuteParam(iCol, iMax, stmtData, StdForward<VarArgs>(vaArgs)...);
  }
  /* -- Initialise as a c-string with the specified size as text ----------- */
  template<typename ...VarArgs>
    void SqlDoExecuteParam(int &iCol, const int iMax,
      sqlite3_stmt*const stmtData, const size_t stSize, const char*const cpStr,
      VarArgs &&...vaArgs)
  { // Log the parameter
    cLog->LogDebugExSafe("- Arg #$<Size+CStr/Text> = \"$\" ($ bytes).",
      iCol, cpStr, stSize);
    // Process as text then pass next arguments
    SqlSetError(sqlite3_bind_text(stmtData, iCol, cpStr,
      UtilIntOrMax<int>(stSize), fcbSqliteTransient));
    if(SqlDoExecuteParamCheckCommit(stmtData, iCol, iMax))
      SqlDoExecuteParam(iCol, iMax, stmtData, StdForward<VarArgs>(vaArgs)...);
  }
  /* -- Initialise as a stand alone text string ---------------------------- */
  template<typename ...VarArgs>
    void SqlDoExecuteParam(int &iCol, const int iMax,
      sqlite3_stmt*const stmtData, const char*const cpStr, VarArgs &&...vaArgs)
  { // Get text length
    const size_t stLen = strlen(cpStr);
    // Log the parameter
    cLog->LogDebugExSafe("- Arg #$<CStr/Text> = \"$\" ($ bytes).",
      iCol, cpStr, stLen);
    // Process as text then pass next arguments
    SqlSetError(sqlite3_bind_text(stmtData, iCol, cpStr,
      UtilIntOrMax<int>(stLen), fcbSqliteTransient));
    if(SqlDoExecuteParamCheckCommit(stmtData, iCol, iMax))
      SqlDoExecuteParam(iCol, iMax, stmtData, StdForward<VarArgs>(vaArgs)...);
  }
  /* -- Initialise as a c++ string ----------------------------------------- */
  template<typename ...VarArgs>
    void SqlDoExecuteParam(int &iCol, const int iMax,
      sqlite3_stmt*const stmtData, const string &strStr, VarArgs &&...vaArgs)
  { // Log the parameter
    cLog->LogDebugExSafe("- Arg #$<Str/Text> = \"$\" ($ bytes).",
      iCol, strStr, strStr.length());
    // Process as text then pass next arguments
    SqlSetError(sqlite3_bind_text(stmtData, iCol, strStr.data(),
      UtilIntOrMax<int>(strStr.length()), fcbSqliteTransient));
    if(SqlDoExecuteParamCheckCommit(stmtData, iCol, iMax))
      SqlDoExecuteParam(iCol, iMax, stmtData, StdForward<VarArgs>(vaArgs)...);
  }
  /* -- Initialise as a c++ string_view ------------------------------------ */
  template<typename ...VarArgs>
    void SqlDoExecuteParam(int &iCol, const int iMax,
      sqlite3_stmt*const stmtData, const string_view &strvStr,
      VarArgs &&...vaArgs)
  { // Log the parameter
    cLog->LogDebugExSafe("- Arg #$<StrV/Text> = \"$\" ($ bytes).",
      iCol, strvStr, strvStr.length());
    // Process as text then pass next arguments
    SqlSetError(sqlite3_bind_text(stmtData, iCol, strvStr.data(),
      UtilIntOrMax<int>(strvStr.length()), fcbSqliteTransient));
    if(SqlDoExecuteParamCheckCommit(stmtData, iCol, iMax))
      SqlDoExecuteParam(iCol, iMax, stmtData, StdForward<VarArgs>(vaArgs)...);
  }
  /* -- Initialise as a c-string with the specified size as a blob --------- */
  template<typename ...VarArgs>
    void SqlDoExecuteParam(int &iCol, const int iMax,
      sqlite3_stmt*const stmtData, const char*const cpPtr, const size_t stSize,
      VarArgs &&...vaArgs)
  { // Log the parameter
    cLog->LogNLCDebugExSafe("- Arg #$<Ptr+Size/Blob> = $ bytes.",
      iCol, stSize);
    // Process as blob then pass next arguments
    SqlSetError(sqlite3_bind_blob(stmtData, iCol, cpPtr,
      UtilIntOrMax<int>(stSize), fcbSqliteTransient));
    if(SqlDoExecuteParamCheckCommit(stmtData, iCol, iMax))
      SqlDoExecuteParam(iCol, iMax, stmtData, StdForward<VarArgs>(vaArgs)...);
  }
  /* -- Initialise as custom type ------------------------------------------ */
  template<typename ...VarArgs>
    void SqlDoExecuteParam(int &iCol, const int iMax,
      sqlite3_stmt*const stmtData, const char*const cpPtr, const size_t stSize,
      const int iType, VarArgs &&...vaArgs)
  { // Log the parameter
    cLog->LogNLCDebugExSafe("- Arg #$<CStr/$> = \"$\" $ bytes.",
      iCol, iType, cpPtr, stSize);
    // Process as forced custom type then pass next arguments
    SqlSetError((iType == SQLITE_TEXT ?
      sqlite3_bind_text(stmtData, iCol, cpPtr,
        UtilIntOrMax<int>(stSize), fcbSqliteTransient)
           : (iType == SQLITE_BLOB ?
      sqlite3_bind_blob(stmtData, iCol, cpPtr,
        UtilIntOrMax<int>(stSize), fcbSqliteTransient)
           : (iType == SQLITE_INTEGER ?
      sqlite3_bind_int64(stmtData, iCol, StrToNum<sqlite_int64>(cpPtr))
           : (iType == SQLITE_FLOAT ?
      sqlite3_bind_double(stmtData, iCol, StrToNum<double>(cpPtr))
           : (iType == SQLITE_NULL ?
      sqlite3_bind_null(stmtData, iCol)
           : SQLITE_ERROR))))));
    if(SqlDoExecuteParamCheckCommit(stmtData, iCol, iMax))
      SqlDoExecuteParam(iCol, iMax, stmtData, StdForward<VarArgs>(vaArgs)...);
  }
  /* -- Initialise as a memory block --------------------------------------- */
  template<typename ...VarArgs>
    void SqlDoExecuteParam(int &iCol, const int iMax,
      sqlite3_stmt*const stmtData, const MemConst &mcRef, VarArgs &&...vaArgs)
  { // Log the parameter
    cLog->LogDebugExSafe("- Arg #$<Memory/Blob> = $ bytes.",
      iCol, mcRef.MemSize());
    // Process as blob then pass next arguments
    SqlSetError(sqlite3_bind_blob(stmtData, iCol, mcRef.MemPtr<char>(),
      UtilIntOrMax<int>(mcRef.MemSize()), fcbSqliteTransient));
    // Send the parameters if we've binded the maximum allowed
    if(SqlDoExecuteParamCheckCommit(stmtData, iCol, iMax))
      SqlDoExecuteParam(iCol, iMax, stmtData, StdForward<VarArgs>(vaArgs)...);
  }
  /* -- Send command to sql in raw format ---------------------------------- */
  template<typename ...VarArgs>
    void SqlDoExecute(const string &strQuery, VarArgs &&...vaArgs)
  { // Reset previous results
    SqlReset();
    // Set query start time
    const ClockInterval<> ciStart;
    // Statement preparation
    sqlite3_stmt *stmtData = nullptr;
    SqlSetError(sqlite3_prepare_v2(sqlDB, strQuery.data(),
      UtilIntOrMax<int>(strQuery.length()), &stmtData, nullptr));
    // If succeeded then start parsing the input and ouput
    if(SqlIsNoError())
    { // Free the statement context incase of exception
      typedef unique_ptr<sqlite3_stmt,
        function<decltype(sqlite3_finalize)>> SqliteStatementPtr;
      const SqliteStatementPtr sspPtr{ stmtData, sqlite3_finalize };
      // Get number of parameters required to bind and if there is any
      if(const int iMax = sqlite3_bind_parameter_count(stmtData))
      { // Column id
        int iCol = 1;
        // Get maximum parameters allowed before we have to send them
        SqlDoExecuteParam(iCol, iMax, stmtData,
          StdForward<VarArgs>(vaArgs)...);
      } // We don't have parameters so just execute the statement
      else SqlDoStep(stmtData);
    } // Get end query time to get total execution duration
    cdQuery = ciStart.CIDelta();
  }
  /* -- Set a pragma (used only with cvar callbacks) ----------------------- */
  void SqlPragma(const string_view &strvVar)
  { // Execute without value
    if(SqlExecute(StrAppend("PRAGMA ", strvVar)))
    { // If it was not because the database is read only or busy? Throw error
      if(SqlIsNotBusyOrReadOnlyError())
        XC("Database reconfiguration error!",
          "Setting", strvVar, "Error", SqlGetErrorStr(),
          "Code", SqlGetError());
      // Log error instead
      cLog->LogWarningExSafe(
        "Sql set pragma '$' failed because $ ($<$>)!",
        strvVar, SqlGetErrorStr(), SqlGetErrorAsIdString(), SqlGetError());
    } // Log and return success
    else cLog->LogDebugExSafe("Sql pragma '$' succeeded.", strvVar);
  }
  /* -- Set a pragma (used only with cvar callbacks) ----------------------- */
  void SqlPragma(const string_view &strvVar, const string_view &strvVal)
  { // Execute with value
    if(SqlExecute(StrFormat("PRAGMA $=$", strvVar, strvVal)))
    { // If it was not because the database is read only or busy? Throw error
      if(SqlIsNotBusyOrReadOnlyError())
        XC("Database reconfiguration error!",
          "Variable", strvVar,          "Value", strvVal,
          "Error",    SqlGetErrorStr(), "Code",  SqlGetError());
      // Log and return failure
      cLog->LogWarningExSafe(
        "Sql set pragma '$' to '$' failed because $ ($<$>)!",
        strvVar, strvVal, SqlGetErrorStr(), SqlGetErrorAsIdString(),
        SqlGetError());
    } // Log and return success
    else cLog->LogDebugExSafe("Sql set pragma '$' to '$' succeeded.",
      strvVar, strvVal);
  }
  /* -- Is sqlite database opened? --------------------------------- */ public:
  bool SqlIsOpened() { return !!sqlDB; }
  /* -- Heap used ---------------------------------------------------------- */
  size_t SqlHeapUsed() const
    { return static_cast<size_t>(sqlite3_memory_used()); }
  /* -- Execute a command from Lua ----------------------------------------- */
  int SqlExecuteFromLua(lua_State*const lS, const string &strQuery)
  { // Log progress
    cLog->LogDebugExSafe("Sql executing '$'<$> from LUA...",
      strQuery, strQuery.length());
    // Reset previous results
    SqlReset();
    // Set query start time
    const ClockInterval<> ciStart;
    // Statement preparation
    sqlite3_stmt *stmtData = nullptr;
    SqlSetError(sqlite3_prepare_v2(sqlDB, strQuery.data(),
      UtilIntOrMax<int>(strQuery.length()), &stmtData, nullptr));
    // Starting LUA parameter and current enumerated parameter
    const int iStartParam = 2;
    int iParam = iStartParam;
    // If succeeded then start parsing the input and ouput
    if(SqlIsNoError())
    { // Free the statement context incase of exception
      typedef unique_ptr<sqlite3_stmt,
        function<decltype(sqlite3_finalize)>> SqliteStatementPtr;
      const SqliteStatementPtr sspPtr{ stmtData, sqlite3_finalize };
      // Get maximum parameters allowed before we have to send them
      if(const int iMax = sqlite3_bind_parameter_count(stmtData))
      { // No parameters specified? Just execute the statement
        if(LuaUtilIsNone(lS, iParam)) SqlSetError(SQLITE_FORMAT);
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
                  SqlSetError(sqlite3_bind_int64(stmtData, iCol,
                    static_cast<sqlite_int64>(liInt)));
                } // Variable is actually a number
                else
                { // Get double, log it and add it as number
                  const lua_Number lnFloat = LuaUtilToNum(lS, iParam);
                  cLog->LogDebugExSafe("- Arg #$<Number/Float> = $$.",
                    iCol, fixed, lnFloat);
                  SqlSetError(sqlite3_bind_double(stmtData, iCol,
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
                SqlSetError(sqlite3_bind_text(stmtData,
                  iCol, cpStr, UtilIntOrMax<int>(stS), fcbSqliteTransient));
                break;
              } // Variable is a boolean
              case LUA_TBOOLEAN:
              { // Get boolean, log parameter, convert and add as integer
                const bool bBool = LuaUtilToBool(lS, iParam);
                cLog->LogDebugExSafe("- Arg #$<Bool/Int> = $.",
                  iCol, StrFromBoolTF(bBool));
                SqlSetError(sqlite3_bind_int64(stmtData, iCol,
                  static_cast<sqlite_int64>(bBool)));
                break;
              } // Variable is a 'nil'
              case LUA_TNIL:
              { // Log the nil and add it to the Sql query
                cLog->LogDebugExSafe("- Arg #$<Nil/Null>.", iCol);
                SqlSetError(sqlite3_bind_null(stmtData, iCol));
                break;
              } // Variable is userdata
              case LUA_TUSERDATA:
              { // Get reference to memory block, log it and push data to list
                const MemConst &mcRef =
                  *LuaUtilGetPtr<Asset>(lS, iParam, *cAssets);
                cLog->LogDebugExSafe("- Arg #$<Asset/Blob> = $ bytes.",
                  iCol, mcRef.MemSize());
                SqlSetError(sqlite3_bind_blob(stmtData, iCol,
                  mcRef.MemPtr<char>(), UtilIntOrMax<int>(mcRef.MemSize()),
                  fcbSqliteTransient));
                break;
              } // Other variable (ignore)
              default: XC("Unsupported parameter type!",
                "Param", iParam, "LuaType", iType,
                "Typename", LuaUtilGetType(lS, iType));
            } // Do the step if needed break if not needed or error
            if(!SqlDoExecuteParamCheckCommit(stmtData, iCol, iMax)) break;
          } // ...until no parameters left
          while(!LuaUtilIsNone(lS, ++iParam));
        }
      } // We can't add parameters so just execute the statement
      else SqlDoStep(stmtData);
    } // Get end query time to get total execution duration
    cdQuery = ciStart.CIDelta();
    // Log result
    cLog->LogDebugExSafe("- Total: $; Code: $<$>; RTT: $ sec.",
      iParam - iStartParam, SqlResultToString(SqlGetError()), SqlGetError(),
      SqlTimeStr());
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
        LuaUtilSetField(lS, -2, srmpRef.first.data());
      } // Push key pair as integer table
      LuaUtilSetRaw(lS, -3);
      // Next result number
      ++liId;
    }
  }
  /* -- Reset last sql result ---------------------------------------------- */
  void SqlReset()
  { // Clear error
    SqlSetError(SQLITE_OK);
    // Clear last result data
    srKeys.clear();
    // Reset query time
    cdQuery = cd0;
  }
  /* -- Dispatch stored transaction with logging --------------------------- */
  template<typename ...VarArgs>
    int SqlExecute(const string &strQuery, VarArgs &&...vaArgs)
  { // Ignore if nothing to dispatch
    if(strQuery.empty()) return SQLITE_ERROR;
    // Parameters count
    const size_t stCount = sizeof...(vaArgs);
    // Log query, do execution and return result
    cLog->LogDebugExSafe("Sql executing '$'<$>...",
      strQuery, strQuery.length());
    SqlDoExecute(strQuery, StdForward<VarArgs>(vaArgs)...);
    cLog->LogDebugExSafe("- Total: $; Code: $<$>; RTT: $ sec.",
      stCount, SqlResultToString(SqlGetError()), SqlGetError(), SqlTimeStr());
    // Return error status
    return iError;
  }
  /* -- Dispatch stored transaction with logging but return success bool -- */
  template<typename ...VarArgs>
    bool SqlExecuteAndSuccess(const string &strQuery, VarArgs &&...vaArgs)
      { return SqlExecute(strQuery,
          StdForward<VarArgs>(vaArgs)...) == SQLITE_OK; }
  /* -- Check integrity ---------------------------------------------------- */
  bool SqlCheckIntegrity()
  { // Do check (We need a result so dont use SqlPragma())
    if(SqlExecute("PRAGMA integrity_check(1)"))
    { // Log and return failure
      cLog->LogWarningExSafe(
        "Sql failed integrity check failed because $ ($<$>)!",
        SqlGetErrorStr(), SqlGetErrorAsIdString(), SqlGetError());
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
  uint64_t SqlSize()
  { // Get the database page size
    if(SqlExecute("pragma page_size") || srKeys.empty()) return StdMaxUInt64;
    // Get reference to keys list
    const SqlRecordsMap &srmSizePair = *srKeys.cbegin();
    if(srmSizePair.size() != 1) return StdMaxUInt64;
    // Get first item
    const SqlRecordsMapConstIt srmciSizeIt{ srmSizePair.cbegin() };
    if(srmciSizeIt->first != "page_size") return StdMaxUInt64;
    // Return number of tables
    const uint64_t ullPageSize = srmciSizeIt->second.MemReadInt<uint64_t>();
    // Get the database page count
    if(SqlExecute("pragma page_count") || srKeys.empty()) return StdMaxUInt64;
    // Get reference to keys list
    const SqlRecordsMap &srmCount = *srKeys.cbegin();
    if(srmCount.size() != 1) return StdMaxUInt64;
    // Get first item
    const SqlRecordsMapConstIt srmciCount{ srmCount.cbegin() };
    if(srmciCount->first != "page_count") return StdMaxUInt64;
    // Return number of tables
    const uint64_t ullPageCount = srmciCount->second.MemReadInt<uint64_t>();
    // Return result
    return ullPageSize * ullPageCount;
  }
  /* -- Return error string ------------------------------------------------ */
  const char *SqlGetErrorStr() const { return sqlite3_errmsg(sqlDB); }
  /* -- Return error code -------------------------------------------------- */
  int SqlGetError() const { return iError; }
  bool SqlIsErrorEqual(const int iWhat) const
    { return SqlGetError() == iWhat; }
  bool SqlIsErrorNotEqual(const int iWhat) const
    { return !SqlIsErrorEqual(iWhat); }
  bool SqlIsError() const { return SqlIsErrorNotEqual(SQLITE_OK); }
  bool SqlIsNoError() const { return !SqlIsError(); }
  bool SqlIsBusyError() const { return SqlIsErrorEqual(SQLITE_BUSY); }
  bool SqlIsReadOnlyError() const { return SqlIsErrorEqual(SQLITE_READONLY); }
  bool SqlIsBusyOrReadOnlyError() const
    { return SqlIsReadOnlyError() || SqlIsBusyError(); }
  bool SqlIsNotBusyOrReadOnlyError() const
    { return !SqlIsBusyOrReadOnlyError(); }
  const string_view &SqlGetErrorAsIdString() const
    { return SqlResultToString(iError); }
  /* -- Return duration of last query -------------------------------------- */
  double SqlTime() const { return ClockDurationToDouble(cdQuery); }
  /* -- Return formatted query time ---------------------------------------- */
  const string SqlTimeStr() const { return StrShortFromDuration(SqlTime()); }
  /* -- Returns if sql is in a transaction --------------------------------- */
  bool SqlNotActive() const { return !!sqlite3_get_autocommit(sqlDB); }
  bool SqlActive() const { return !SqlNotActive(); }
  /* -- Return string map of records --------------------------------------- */
  SqlResult &SqlGetRecords() { return srKeys; }
  /* -- Useful aliases ----------------------------------------------------- */
  int SqlBegin() { return SqlExecute("BEGIN TRANSACTION"); }
  int SqlEnd() { return SqlExecute("END TRANSACTION"); }
  int SqlDropTable(const string_view &strvTable)
    { return SqlExecute(StrFormat("DROP table `$`", strvTable)); }
  int SqlFlushTable(const string_view &strvTable)
    { return SqlExecute(StrFormat("DELETE from `$`", strvTable)); }
  int SqlOptimise() { return SqlExecute("VACUUM"); }
  int SqlAffected() { return sqlite3_changes(sqlDB); }
  /* -- Process a count(*) requested --------------------------------------- */
  size_t SqlGetRecordCount(const string_view &strvTable,
    const string_view &strvCondition=cCommon->CommonCBlank())
  { // Do a table count lookup. If succeeded and have records?
    if(SqlExecuteAndSuccess(StrFormat("SELECT count(*) FROM `$`$",
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
          SqlReset();
          // Return number of tables
          return stCount;
        } // Count(*) not found
      } // Result does not have one record
    } // Failed to execute query or no records, return failure
    return StdMaxSizeT;
  }
  /* -- Check if a table exists -------------------------------------------- */
  bool SqlIsTableExist(const string_view &strvTable)
    { return SqlExecuteAndSuccess(
        "SELECT `name` FROM `sqlite_master` WHERE `type`='table' AND `name`=?",
        strvTable) && !SqlGetRecords().empty(); }
  /* -- Flush all orphan statements ---------------------------------------- */
  size_t SqlFinalise() const
  { // Number of orphan transactions
    size_t stOrphan = 0;
    // Finalise all remaining statements until there are no more left
    while(sqlite3_stmt*const stmtData = sqlite3_next_stmt(sqlDB, nullptr))
      { sqlite3_finalize(stmtData); ++stOrphan; }
    // Return number of orphans
    return stOrphan;
  }
  /* ----------------------------------------------------------------------- */
  bool SqlCVarReadKeys()
  { // Read all variable names and if failed?
    if(SqlExecute(
      StrFormat("SELECT `$` from `$`", strCVKeyColumn, strvCVTable)))
    { // Put in log and return nothing loaded
      cLog->LogWarningExSafe(
        "CVars failed to fetch cvar key names because $ ($)!",
        SqlGetErrorStr(), SqlGetError());
      return false;
    } // Write number of records read and return success
    cLog->LogDebugExSafe("Sql read $ key names from CVars table.",
      SqlGetRecords().size());
    return true;
  }
  /* ----------------------------------------------------------------------- */
  bool SqlCVarReadAll()
  { // Read all variables and if failed?
    if(SqlExecute(StrFormat("SELECT `$`,`$`,`$` from `$`",
      strCVKeyColumn, strCVFlagsColumn, strCVValueColumn, strvCVTable)))
    { // Put in log and return nothing loaded
      cLog->LogWarningExSafe("Sql failed to read CVars table because $ ($)!",
        SqlGetErrorStr(), SqlGetError());
      return false;
    } // Write number of records read and return success
    cLog->LogDebugExSafe("Sql read $ records from CVars table.",
      SqlGetRecords().size());
    return true;
  }
  /* ----------------------------------------------------------------------- */
  CreateTableResult SqlCVarDropTable()
  { // If table exists return that it already exists
    if(SqlIsTableExist(strvCVTable)) return CTR_OK_ALREADY;
    // Drop the SQL table and if failed?
    if(SqlDropTable(strvCVTable))
    { // Write error in console and return failure
      cLog->LogWarningExSafe(
        "Sql failed to destroy CVars table because $ ($)!",
        SqlGetErrorStr(), SqlGetError());
      return CTR_FAIL;
    } // Write success in console and return success
    cLog->LogDebugSafe("Sql destroyed CVars table successfully.");
    return CTR_OK;
  }
  /* ----------------------------------------------------------------------- */
  CreateTableResult SqlCVarCreateTable()
  { // If table already exists then return success already
    if(SqlIsTableExist(strvCVTable)) return CTR_OK_ALREADY;
    // Create the SQL table for our settings if it does not exist
    if(SqlExecute(StrFormat(
           "CREATE table `$`("         // Table name
           "`$` TEXT UNIQUE NOT NULL," // Variable name
           "`$` INTEGER DEFAULT 0,"    // Value flags (crypt,comp,etc.)
           "`$` TEXT)",                // Value (any type allowed)
         strvCVTable, strCVKeyColumn, strCVFlagsColumn, strCVValueColumn)))
    { // Write error in console and return failure
      cLog->LogWarningExSafe("Sql failed to create CVars table because $ ($)!",
        SqlGetErrorStr(), SqlGetError());
      return CTR_FAIL;
    } // Write success in console and return success
    cLog->LogDebugSafe("Sql created CVars table successfully.");
    return CTR_OK;
  }
  /* ----------------------------------------------------------------------- */
  CreateTableResult SqlLuaCacheDropTable()
  { // If table exists return that it already exists
    if(SqlGetRecordCount(strvLCTable) == StdMaxSizeT) return CTR_OK_ALREADY;
    // Drop the SQL table and if failed?
    if(SqlDropTable(strvLCTable))
    { // Write error in console and return failure
      cLog->LogWarningExSafe(
        "Sql failed to destroy cache table because $ ($)!",
        SqlGetErrorStr(), SqlGetError());
      return CTR_FAIL;
    } // Write success in console and return success
    cLog->LogDebugSafe("Sql destroyed cache table successfully.");
    return CTR_OK;
  }
  /* ----------------------------------------------------------------------- */
  CreateTableResult SqlLuaCacheCreateTable()
  { // If table already exists then return success already
    if(SqlIsTableExist(strvLCTable)) return CTR_OK_ALREADY;
    // Create the SQL table for our settings if it does not exist
    if(SqlExecute(StrFormat(
         "CREATE table `$`("              // Table name
           "`$` INTEGER UNIQUE NOT NULL," // Code CRC value
           "`$` INTEGER NOT NULL,"        // Code update timestamp
           "`$` TEXT UNIQUE NOT NULL,"    // Code eference
           "`$` TEXT NOT NULL)",          // Code binary
         strvLCTable, strvLCCRCColumn, strvLCTimeColumn, strvLCRefColumn,
         strvLCCodeColumn)))
    { // Write error in console and return failure
      cLog->LogWarningExSafe("Sql failed to create cache table because $ ($)!",
        SqlGetErrorStr(), SqlGetError());
      return CTR_FAIL;
    } // Write success in console and return success
    cLog->LogDebugSafe("Sql created cache table successfully.");
    return CTR_OK;
  }
  /* ----------------------------------------------------------------------- */
  CreateTableResult SqlLuaCacheRebuildTable()
    { SqlLuaCacheDropTable(); return SqlLuaCacheCreateTable(); }
  /* ----------------------------------------------------------------------- */
  bool SqlCVarCommitData(const string &strVar,
    const SqlCVarDataFlagsConst &scvdfcFlags, const int iType,
    const char*const cpData, const size_t stLength)
  { // Try to write the specified cvar and if failed?
    if(SqlExecute(StrFormat(
           "INSERT or REPLACE into `$`(`$`,`$`,`$`) VALUES(?,?,?)",
           strvCVTable, strCVKeyColumn, strCVFlagsColumn, strCVValueColumn),
         strVar, scvdfcFlags.FlagGet<int>(), cpData, stLength, iType))
    { // Log the warning and return failure
      cLog->LogWarningExSafe(
        "Sql failed to commit CVar '$' (T:$;ST:$;B:$) because $ ($)!",
        strVar, scvdfcFlags.FlagGet(), iType, stLength, SqlGetErrorStr(),
        SqlGetError());
      return false;
    } // Report and return success
    cLog->LogDebugExSafe("Sql commited CVar '$' (T:$;ST:$;B:$).",
      strVar, scvdfcFlags.FlagGet(), iType, stLength);
    return true;
  }
  /* ----------------------------------------------------------------------- */
  bool SqlCVarCommitString(const string &strVar, const string &strVal)
    { return SqlCVarCommitData(strVar, SD_NONE,
        SQLITE_TEXT, strVal.data(), strVal.length()); }
  /* ----------------------------------------------------------------------- */
  bool SqlCVarCommitBlob(const string &strVar, const MemConst &mcSrc)
    { return SqlCVarCommitData(strVar, SD_ENCRYPTED,
        SQLITE_BLOB, mcSrc.MemPtr<char>(), mcSrc.MemSize()); }
  /* ----------------------------------------------------------------------- */
  PurgeResult SqlCVarPurgeData(const char*const cpKey, const size_t stKey)
  { // Try to purge the cvar from the database and if it failed?
    if(!SqlExecuteAndSuccess("DELETE from `C` WHERE `K`=?", stKey, cpKey))
    { // Log the warning and return failure
      cLog->LogWarningExSafe("Sql failed to purge CVar '$' because $ ($)!",
        cpKey, SqlGetErrorStr(), SqlGetError());
      return PR_FAIL;
    } // Just return if no records were affected
    if(!SqlAffected()) return PR_OK_NC;
    // Report and return success
    cLog->LogDebugExSafe("Sql purged CVar '$' from database.", cpKey);
    return PR_OK;
  }
  /* ----------------------------------------------------------------------- */
  PurgeResult SqlCVarPurge(const string &strVar)
    { return SqlCVarPurgeData(strVar.data(), strVar.length()); }
  /* -- DeInit ------------------------------------------------------------- */
  void SqlDeInit()
  { // Ignore if no handle to deinit
    if(!sqlDB) return;
    // Log deinitialisation
    cLog->LogDebugExSafe("Sql database '$' is closing...", IdentGet());
    // Finalise statements and if we found orphans
    if(const size_t stOrphans = SqlFinalise())
      cLog->LogWarningExSafe("Sql finalised $ orphan statements.", stOrphans);
    // Check if database should be deleted, if it should'nt?
    switch(const ADResult adrResult = SqlCanDatabaseBeDeleted())
    { // Deletion is ok?
      case ADR_OK_NO_TABLES:
      case ADR_OK_NO_RECORDS:
        // Print the result
        cLog->LogDebugExSafe("Sql will delete empty database because $ ($).",
          SqlADResultToString(adrResult), adrResult);
        // Close the database
        SqlDoClose();
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
          SqlADResultToString(adrResult), adrResult);
        // Sqlite docs recommend us to optimise before closing.
        SqlPragma("optimize");
        // Close the database
        SqlDoClose();
        // Done
        break;
    } // Reset results
    SqlReset();
    // Reset database name
    IdentClear();
    // Temporary database boolean
    FlagClear(SF_ISTEMPDB);
    // Clear private key
    cCrypt->CryptSetDefaultPrivateKey();
  }
  /* -- Load schema version ------------------------------------------------ */
  void SqlLoadSchemaVersion()
  { // If table exists create a new table
    if(!SqlIsTableExist(strvSKeyTable)) goto NewKeyNoDrop;
    // Check record count
    switch(SqlGetRecordCount(strvSKeyTable))
    { // Error reading table?
      case StdMaxSizeT:
      { // Log failure and create a new private key
        cLog->LogWarningExSafe(
          "Sql failed to read schema table because $ ($<$>)!",
          SqlGetErrorStr(), SqlGetErrorAsIdString(), SqlGetError());
        goto NewKey;
      } // Any other value is not allowed
      default:
      { // Read schema version key and if failed?
        if(SqlExecute(StrFormat("SELECT `$` from `$` WHERE `$`=?",
          strvSValueColumn, strvSKeyTable, strvSIndexColumn), strvSVersionKey))
        { // Log failure and create a new private key
          cLog->LogWarningExSafe(
            "Sql failed to read schema version because $ ($<$>)!",
            SqlGetErrorStr(), SqlGetErrorAsIdString(), SqlGetError());
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
        switch(const sqlite3_int64 llLVersion =
          sdRef.MemReadInt<sqlite3_int64>())
        { // Is our correct version?
          case llVersion:
            // Notify log of correct version
            cLog->LogDebugExSafe("Sql schema version $ verified!", llLVersion);
            break;
          // Is different version?
          default:
            // Notify log of version difference and create new version key
            cLog->LogWarningExSafe("Sql schema version of $ different from "
              "engine version of $!", llLVersion, llVersion);
            // Set updated to the new version
            goto NewKeyNoDropInsert;
        } // Done
        return;
      }
    } // Could not read new private key so setup new private key
    NewKey:
    // Try to drop the original private key table
    if(SqlDropTable(strvSKeyTable))
      cLog->LogWarningExSafe(
        "Sql failed to drop schema table because $ ($<$>)!",
        SqlGetErrorStr(), SqlGetErrorAsIdString(), SqlGetError());
    // Skipped dropping non-existant table
    NewKeyNoDrop:
    // Now try to create the table that holds the private key and if failed?
    if(!SqlExecuteAndSuccess(
         StrFormat("CREATE table `$`(`$` TEXT UNIQUE NOT NULL,"
                                    "`$` INTEGER NOT NULL)",
        strvSKeyTable, strvSIndexColumn, strvSValueColumn)))
      cLog->LogWarningExSafe(
        "Sql failed to create schema table because $ ($<$>)!",
        SqlGetErrorStr(), SqlGetErrorAsIdString(), SqlGetError());
    // Skipped drop and recreation of existing table
    NewKeyNoDropInsert:
    // Now try to create the table that holds the private key and if failed?
    if(!SqlExecuteAndSuccess(StrFormat(
        "INSERT or REPLACE into `$`(`$`,`$`) VALUES(?,?)",
        strvSKeyTable, strvSIndexColumn, strvSValueColumn), strvSVersionKey,
        llVersion))
      cLog->LogWarningExSafe(
        "Sql failed to insert schema version record because $ ($<$>)!",
        SqlGetErrorStr(), SqlGetErrorAsIdString(), SqlGetError());
    // Notify log of version update
    else cLog->LogInfoExSafe("Sql schema version updated to $.", llVersion);
  }
  /* -- Create new private key --------------------------------------------- */
  bool SqlCreatePrivateKeyNoDrop()
  { // Try to create the table that holds the private key and if failed?
    if(SqlExecute(StrFormat("CREATE table `$`(`$` INTEGER NOT NULL,"
                                             "`$` INTEGER NOT NULL)",
      strvPKeyTable, strvPIndexColumn, strvPValueColumn)))
    { // Log failure and return
      cLog->LogWarningExSafe(
        "Sql failed to create key table because $ ($<$>)!",
        SqlGetErrorStr(), SqlGetErrorAsIdString(), SqlGetError());
      return false;
    } // Prepare statement to insert values into sql
    const string strInsert{ StrFormat("INSERT into `$`(`$`,`$`) VALUES(?,?)",
      strvPKeyTable, strvPIndexColumn, strvPValueColumn) };
    // For each key part to write
    for(size_t stIndex = 0; stIndex < cCrypt->pkKey.qkData.size(); ++stIndex)
    { // Get private key value and if failed?
      const sqlite3_int64 llVal =
        static_cast<sqlite3_int64>(cCrypt->CryptReadPrivateKey(stIndex));
      if(!SqlExecuteAndSuccess(strInsert,
        static_cast<sqlite3_int64>(stIndex), llVal))
      { // Log failure and return failure
        cLog->LogWarningExSafe(
          "Sql failed to write key table at $ because $ ($<$>)!",
          stIndex, SqlGetErrorStr(), SqlGetErrorAsIdString(), SqlGetError());
        return false;
      }
    } // Log result and return success
    cLog->LogDebugSafe(
      "Sql wrote a new key table into database successfully!");
    return true;
  }
  /* -- Create new private key (drop first) -------------------------------- */
  bool SqlCreatePrivateKey()
  { // Generate a new private key
    cCrypt->CryptResetPrivateKey();
    // Try to drop the original private key table
    if(SqlDropTable(strvPKeyTable))
      cLog->LogWarningExSafe("Sql failed to drop key table because $ ($<$>)!",
        SqlGetErrorStr(), SqlGetErrorAsIdString(), SqlGetError());
    // Now create the private key table
    return SqlCreatePrivateKeyNoDrop();
  }
  /* -- Load private key --------------------------------------------------- */
  void SqlLoadPrivateKey()
  { // If table exists create a new table
    if(!SqlIsTableExist(strvPKeyTable)) SqlCreatePrivateKeyNoDrop();
    // Check record count
    switch(const size_t stCount = SqlGetRecordCount(strvPKeyTable))
    { // Error reading table?
      case StdMaxSizeT:
      { // Log failure and create a new private key
        cLog->LogWarningExSafe(
          "Sql failed to read key table because $ ($<$>)!",
          SqlGetErrorStr(), SqlGetErrorAsIdString(), SqlGetError());
        SqlCreatePrivateKey();
        return;
      } // Any other value is not allowed
      default:
      { // Log failure and create a new private key
        cLog->LogWarningExSafe("Sql key table corrupt ($ != $)!",
          stCount, cCrypt->pkKey.qkData.size());
        SqlCreatePrivateKey();
        return;
      } // Read enough entries?
      case Crypt::stPkTotalCount:
      { // Read keys and if failed?
        if(SqlExecute(StrFormat("SELECT `$` from `$` ORDER BY `$` ASC",
          strvPValueColumn, strvPKeyTable, strvPIndexColumn)))
        { // Log failure and create a new private key
          cLog->LogWarningExSafe(
            "Sql failed to read key table because $ ($<$>)!",
            SqlGetErrorStr(), SqlGetErrorAsIdString(), SqlGetError());
          SqlCreatePrivateKey();
          return;
        } // If not enough results?
        if(srKeys.size() != cCrypt->pkKey.qkData.size())
        { // Log failure and create a new private key
          cLog->LogWarningExSafe("Sql read only $ of $ key table records!",
            srKeys.size(), cCrypt->pkKey.qkData.size());
          SqlCreatePrivateKey();
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
            SqlCreatePrivateKey();
            return;
          } // Read record data. It must be 8 bytes. If it isnt?
          if(sdRef.MemSize() != sizeof(sqlite3_int64))
          { // Log failure and create new private key table
            cLog->LogWarningExSafe(
              "Sql key table at column $ expected $ not $ bytes!",
              stIndex, sizeof(sqlite3_int64), sdRef.MemSize());
            SqlCreatePrivateKey();
            return;
          } // Read in the value to the private key table
          cCrypt->CryptWritePrivateKey(stIndex++,
            static_cast<uint64_t>(sdRef.MemReadInt<sqlite3_int64>()));
        } // Log result and return
        cLog->LogDebugSafe("Sql loaded key table from database successfully!");
      }
    }
  }
  /* -- Init --------------------------------------------------------------- */
  bool SqlInit(const string &strPrefix)
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
    if(const int iCode = sqlite3_open_v2(strDb.data(), &sqlDBtemp,
         SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_FULLMUTEX |
         SQLITE_OPEN_SHAREDCACHE, reinterpret_cast<char*>(sqlDBtemp)))
    { // Log error result
      cLog->LogWarningExSafe("Sql could not open '$' because $ ($)!", strDb,
        SqlGetErrorStr(), iCode);
      // Sql open failed so 'sqlDBtemp' stays NULL.
      return false;
    } // Set to this database and set name
    sqlDB = sqlDBtemp;
    IdentSet(strDb);
    // Load schema version then private key
    SqlLoadSchemaVersion();
    SqlLoadPrivateKey();
    // Log successfull initialisation and return success
    cLog->LogInfoExSafe("Sql database '$' initialised.", IdentGet());
    return true;
  }
  /* -- Constructor --------------------------------------------- */ protected:
  Sql() :
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
    cdRetry{ cd1S },                   // Initially wait 1 second per retry
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
    if(SqlIsError())
      XC("Failed to initialise SQLite!",
        "Error", SqlGetError(), "Reason", SqlGetErrorAsIdString());
  }
  /* -- Destructor --------------------------------------------------------- */
  DTORHELPER(~Sql, SqlDeInit(); sqlite3_shutdown())
  /* -- Set a pragma on or off (used only with cvar callbacks) ----- */ public:
  CVarReturn SqlPragmaOnOff(const string &strVar, const bool bState)
    { SqlPragma(strVar, bState ? strvOn : strvOff); return ACCEPT; }
  /* -- Set retry count ---------------------------------------------------- */
  CVarReturn SqlRetryCountModified(const unsigned int uiCount)
    { return CVarSimpleSetInt(uiQueryRetries, uiCount); }
  /* -- Set retry suspend time --------------------------------------------- */
  CVarReturn SqlRetrySuspendModified(const uint64_t ullMilliseconds)
    { return CVarSimpleSetIntNLG(cdRetry,
        milliseconds{ ullMilliseconds }, cd0, cd1S); }
  /* -- Modify delete empty database permission ---------------------------- */
  CVarReturn SqlDeleteEmptyDBModified(const bool bState)
    { FlagSetOrClear(SF_DELETEEMPTYDB, bState); return ACCEPT; }
  /* -- sql_temp_store cvar was modified ----------------------------------- */
  CVarReturn SqlTempStoreModified(const string &strFile, string&)
  { // Prevent manipulating the query
    if(strFile.find(' ') != StdNPos || strFile.find(';') != StdNPos)
      return DENY;
    // Do the query
    SqlPragma("temp_store", strFile);
    // Success
    return ACCEPT;
  }
  /* -- sql_synchronous cvar was modified ---------------------------------- */
  CVarReturn SqlSynchronousModified(const bool bState)
    { return SqlPragmaOnOff("synchronous", bState); }
  /* -- sql_journal_mode cvar was modified --------------------------------- */
  CVarReturn SqlJournalModeModified(const bool bState)
    { return SqlPragmaOnOff("journal_mode", bState); }
  /* -- sql_auto_vacuum cvar was modified ---------------------------------- */
  CVarReturn SqlAutoVacuumModified(const bool bState)
    { return SqlPragmaOnOff("auto_vacuum", bState); }
  /* -- sql_auto_vacuum cvar was modified ---------------------------------- */
  CVarReturn SqlForeignKeysModified(const bool bState)
    { return SqlPragmaOnOff("foreign_keys", bState); }
  /* -- sql_inc_vacuum cvar was modified ----------------------------------- */
  CVarReturn SqlIncVacuumModified(const uint64_t ullVal)
    { SqlPragma(StrFormat("incremental_vacuum($)", ullVal)); return ACCEPT; }
  /* -- sql_db cvar was modified ------------------------------------------- */
  CVarReturn SqlUdbFileModified(const string &strFile, string &strVar)
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
    if(SqlInit(strVar))
    { // Set full path name of the database
      SqlInitOK: strVar = StdMove(PathSplit{ strVar, true }.strFull);
      // Success
      return ACCEPT_HANDLED;
    } // If we have a persistant directory?
    if(cCmdLine->CmdLineIsHome())
    { // Set a new filename in the users home directory
      strVar = cCmdLine->CmdLineGetHome(PathSplit{ strVar }.strFileExt);
      // Try opening that and if succeeded then return success
      if(SqlInit(strVar)) goto SqlInitOK;
    } // Use memory database instead
    strVar = strMemoryDBName;
    // Now open the memory database which should ALWAYS succeed.
    return SqlInit(strVar) ? ACCEPT_HANDLED : DENY;
  }
};/* ----------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
