/* == LLSQL.HPP ============================================================ **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## Defines the 'Sql' namespace and methods for the guest to use in     ## **
** ## Lua. This file is invoked by 'lualib.hpp'.                          ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ========================================================================= **
** ######################################################################### **
** ========================================================================= */
// % Sql
/* ------------------------------------------------------------------------- */
// ! The sql class allows manipulation of the engine database using the sqlite
// ! library.
/* ========================================================================= */
namespace LLSql {                      // Sql namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace ISql::P;               using namespace Common;
/* ========================================================================= **
** ######################################################################### **
** ## Sql common helper classes                                           ## **
** ######################################################################### **
** -- Get Sql class argument ----------------------------------------------- */
struct AgSql : public ArClass<Sql>
  { explicit AgSql(lua_State*const lS, const int iArg) :
      ArClass{*LuaUtilGetPtr<Sql>(lS, iArg, *cSqls)}{} };
/* ========================================================================= **
** ######################################################################### **
** ## Sql.* member functions                                              ## **
** ######################################################################### **
** ========================================================================= */
// $ Sql:Destroy
// ? Destroys the database object, closing the database and freeing all the
// ? memory associated with it.
/* ------------------------------------------------------------------------- */
LLFUNC(Destroy, 0, LuaUtilClassDestroy<Sql>(lS, cSqls))
/* ========================================================================= */
// $ Sql:Error
// < Code:integer=Last error code.
// ? Returns the error code of the last SQLlite operation.
/* ------------------------------------------------------------------------- */
LLFUNC(Error, 1, LuaUtilPushVar(lS, AgSql{lS, 1}().SqlGetError()))
/* ========================================================================= */
// $ Sql:ErrorStr
// < Error:string=Last error reason string.
// ? Returns the string version of the error code.
/* ------------------------------------------------------------------------- */
LLFUNC(ErrorStr, 1, LuaUtilPushVar(lS, AgSql{lS, 1}().SqlGetErrorAsIdString()))
/* ========================================================================= */
// $ Sql:Reason
// < Error:string=Last error reason string.
// ? Returns the error string of the last SQLlite operation.
/* ------------------------------------------------------------------------- */
LLFUNC(Reason, 1, LuaUtilPushVar(lS, AgSql{lS, 1}().SqlGetErrorStr()))
/* ========================================================================= */
// $ Sql:Records
// < Records:table=Resulting records.
// ? Returns the resulting records of the last SQLlite operation. The format
// ? is an indexed table of string key/value tables. e.g.
// ? table = {
// ?   { ["Index"]=1, ["Name"]="John Doe", ["Age"]=37 },
// ?   { ["Index"]=2, ["Name"]="Bill Gates", ["Age"]=33 },
// ?   etc...
// ? }
/* ------------------------------------------------------------------------- */
LLFUNC(Records, 1, AgSql{lS, 1}().RecordsToLuaTable(lS))
/* ========================================================================= */
// $ Sql:Exec
// > Code:string=The SQLlite code to execute.
// > Data:Any=The arguments used in place of '?'
// < Result:boolean=If the statement was added or not
// ? Execute the specified SQLlite statement. See
// ? https://www.sqlite.org/docs.html for more information on the SQLlite API.
/* ------------------------------------------------------------------------- */
LLFUNC(Exec, 1,
  LuaUtilPushVar(lS, AgSql{lS, 1}().SqlExecuteFromLua(lS, AgString{lS, 2})))
/* ========================================================================= */
// $ Sql:Reset
// ? Cleans up the last result, error and response.
/* ------------------------------------------------------------------------- */
LLFUNC(Reset, 0, AgSql{lS, 1}().SqlReset())
/* ========================================================================= */
// $ Sql:Affected
// ? Returns number of affected rows in the last result
/* ------------------------------------------------------------------------- */
LLFUNC(Affected, 1, LuaUtilPushVar(lS, AgSql{lS, 1}().SqlAffected()))
/* ========================================================================= */
// $ Sql:Time
// ? Returns time taken with the last executed SQL statement
/* ------------------------------------------------------------------------- */
LLFUNC(Time, 1, LuaUtilPushVar(lS, AgSql{lS, 1}().SqlTime()))
/* ========================================================================= */
// $ Sql:Active
// ? Returns wether a transaction is in progress or not.
/* ------------------------------------------------------------------------- */
LLFUNC(Active, 1, LuaUtilPushVar(lS, AgSql{lS, 1}().SqlActive()))
/* ========================================================================= */
// $ Sql:Size
// ? Returns size of database.
/* ------------------------------------------------------------------------- */
LLFUNC(Size, 1, LuaUtilPushVar(lS, AgSql{lS, 1}().SqlSize()))
/* ========================================================================= */
// $ Sql:Begin
// < Result:integer=The result of the operation
// ? Begins a new transaction. Returns non-zero if the call fails.
/* ------------------------------------------------------------------------- */
LLFUNC(Begin, 1, LuaUtilPushVar(lS, AgSql{lS, 1}().SqlBegin()))
/* ========================================================================= */
// $ Sql:Commit
// < Result:integer=The result of the operation
// ? Ends an existing transaction and commits it. Returns non-zero if the call
// ? fails.
/* ------------------------------------------------------------------------- */
LLFUNC(End, 1, LuaUtilPushVar(lS, AgSql{lS, 1}().SqlEnd()))
/* ========================================================================= **
** ######################################################################### **
** ## Sql:* member functions structure                                    ## **
** ######################################################################### **
** ========================================================================= */
LLRSMFBEGIN                            // Sql:* member functions begin
  LLRSFUNC(Active),   LLRSFUNC(Affected),  LLRSFUNC(Begin),
  LLRSFUNC(Destroy),  LLRSFUNC(End),       LLRSFUNC(Error),
  LLRSFUNC(ErrorStr), LLRSFUNC(Exec),      LLRSFUNC(Reason),
  LLRSFUNC(Records),  LLRSFUNC(Reset),     LLRSFUNC(Size),
  LLRSFUNC(Time),
LLRSEND                                // Sql:* member functions end
/* ========================================================================= **
** ######################################################################### **
** ## Sql.* namespace functions                                           ## **
** ######################################################################### **
** ========================================================================= */
// $ Sql.Main
// < Result:Sql=The handle of the main engine database.
// ? Returns the handle of the main engine database.
/* ------------------------------------------------------------------------- */
LLFUNC(Main, 1, LuaUtilClassCreatePtr<Sql>(lS, *cSqls, cSql))
/* ========================================================================= */
// $ Sql.Open
// < Result:Sql=The handle of the database.
// ? Creates or opens the specified database and returns a handle to it.
/* ------------------------------------------------------------------------- */
LLFUNC(Open, 1,
  const AgFilename aFilename{lS, 1};
  LuaUtilClassCreate<Sql>(lS, *cSqls)->SqlInit(aFilename));
/* ========================================================================= **
** ######################################################################### **
** ## Sql:* namespace functions structure                                 ## **
** ######################################################################### **
** ========================================================================= */
LLRSBEGIN                              // Sql:* namespace functions begin
  LLRSFUNC(Main), LLRSFUNC(Open),
LLRSEND                                // Sql:* namespace functions end
/* ========================================================================= **
** ######################################################################### **
** ## Sql:* namespace constants                                           ## **
** ######################################################################### **
** ========================================================================= */
// @ Sql.Codes
// < Data:table=The entire list of errors SQL supports
// ? Returns a table of key/value pairs that identify a SQL error code.
/* ------------------------------------------------------------------------- */
LLRSKTBEGIN(Codes)                     // Beginning of error codes
  LLRSKTITEM(SQLITE_,ABORT),           LLRSKTITEM(SQLITE_,BUSY),
  LLRSKTITEM(SQLITE_,CANTOPEN),        LLRSKTITEM(SQLITE_,CONSTRAINT),
  LLRSKTITEM(SQLITE_,CORRUPT),         LLRSKTITEM(SQLITE_,DONE),
  LLRSKTITEM(SQLITE_,EMPTY),           LLRSKTITEM(SQLITE_,ERROR),
  LLRSKTITEM(SQLITE_,FORMAT),          LLRSKTITEM(SQLITE_,FULL),
  LLRSKTITEM(SQLITE_,INTERNAL),        LLRSKTITEM(SQLITE_,INTERRUPT),
  LLRSKTITEM(SQLITE_,IOERR),           LLRSKTITEM(SQLITE_,LOCKED),
  LLRSKTITEM(SQLITE_,MISMATCH),        LLRSKTITEM(SQLITE_,MISUSE),
  LLRSKTITEM(SQLITE_,NOLFS),           LLRSKTITEM(SQLITE_,NOMEM),
  LLRSKTITEM(SQLITE_,NOTADB),          LLRSKTITEM(SQLITE_,NOTFOUND),
  LLRSKTITEM(SQLITE_,NOTICE),          LLRSKTITEM(SQLITE_,OK),
  LLRSKTITEM(SQLITE_,PERM),            LLRSKTITEM(SQLITE_,PROTOCOL),
  LLRSKTITEM(SQLITE_,RANGE),           LLRSKTITEM(SQLITE_,READONLY),
  LLRSKTITEM(SQLITE_,ROW),             LLRSKTITEM(SQLITE_,SCHEMA),
  LLRSKTITEM(SQLITE_,TOOBIG),          LLRSKTITEM(SQLITE_,WARNING),
LLRSKTEND                              // End of error codes
/* ========================================================================= **
** ######################################################################### **
** ## Sql:* namespace constants structure                                 ## **
** ######################################################################### **
** ========================================================================= */
LLRSCONSTBEGIN                         // Sql:* namespace consts begin
  LLRSCONST(Codes),
LLRSCONSTEND                           // Sql:* namespace consts end
/* ========================================================================= */
}                                      // End of Sql namespace
/* == EoF =========================================================== EoF == */
