/* == LLSQL.HPP ============================================================ **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## Defines the 'Sql' namespace and methods for the guest to use in     ## **
** ## Lua. This file is invoked by 'lualib.hpp'.                          ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
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
** ## Stat.* namespace functions                                          ## **
** ######################################################################### **
** ========================================================================= */
// $ Sql.Error
// < Code:integer=Last error code.
// ? Returns the error code of the last SQLlite operation.
/* ------------------------------------------------------------------------- */
LLFUNC(Error, 1, LuaUtilPushVar(lS, cSql->GetError()))
/* ========================================================================= */
// $ Sql.ErrorStr
// < Error:string=Last error reason string.
// ? Returns the string version of the error code.
/* ------------------------------------------------------------------------- */
LLFUNC(ErrorStr, 1, LuaUtilPushVar(lS, cSql->GetErrorAsIdString()))
/* ========================================================================= */
// $ Sql.Reason
// < Error:string=Last error reason string.
// ? Returns the error string of the last SQLlite operation.
/* ------------------------------------------------------------------------- */
LLFUNC(Reason, 1, LuaUtilPushVar(lS, cSql->GetErrorStr()))
/* ========================================================================= */
// $ Sql.Records
// < Records:table=Resulting records.
// ? Returns the resulting records of the last SQLlite operation. The format
// ? is an indexed table of string key/value tables. e.g.
// ? table = {
// ?   { ["Index"]=1, ["Name"]="John Doe", ["Age"]=37 },
// ?   { ["Index"]=2, ["Name"]="Bill Gates", ["Age"]=33 },
// ?   etc...
// ? }
/* ------------------------------------------------------------------------- */
LLFUNC(Records, 1, cSql->RecordsToLuaTable(lS))
/* ========================================================================= */
// $ Sql.Exec
// > Code:string=The SQLlite code to execute.
// > Data:Any=The arguments used in place of '?'
// < Result:boolean=If the statement was added or not
// ? Execute the specified SQLlite statement. See
// ? https://www.sqlite.org/docs.html for more information on the SQLlite API.
/* ------------------------------------------------------------------------- */
LLFUNC(Exec, 1, LuaUtilPushVar(lS, cSql->ExecuteFromLua(lS, AgString{lS,1})))
/* ========================================================================= */
// $ Sql.Reset
// ? Cleans up the last result, error and response.
/* ------------------------------------------------------------------------- */
LLFUNC(Reset, 0, cSql->Reset())
/* ========================================================================= */
// $ Sql.Affected
// ? Returns number of affected rows in the last result
/* ------------------------------------------------------------------------- */
LLFUNC(Affected, 1, LuaUtilPushVar(lS, cSql->Affected()))
/* ========================================================================= */
// $ Sql.Time
// ? Returns time taken with the last executed SQL statement
/* ------------------------------------------------------------------------- */
LLFUNC(Time, 1, LuaUtilPushVar(lS, cSql->Time()))
/* ========================================================================= */
// $ Sql.Active
// ? Returns wether a transaction is in progress or not.
/* ------------------------------------------------------------------------- */
LLFUNC(Active, 1, LuaUtilPushVar(lS, cSql->Active()))
/* ========================================================================= */
// $ Sql.Size
// ? Returns size of database.
/* ------------------------------------------------------------------------- */
LLFUNC(Size, 1, LuaUtilPushVar(lS, cSql->Size()))
/* ========================================================================= */
// $ Sql.Begin
// < Result:integer=The result of the operation
// ? Begins a new transaction. Returns non-zero if the call fails.
/* ------------------------------------------------------------------------- */
LLFUNC(Begin, 1, LuaUtilPushVar(lS, cSql->Begin()))
/* ========================================================================= */
// $ Sql.Commit
// < Result:integer=The result of the operation
// ? Ends an existing transaction and commits it. Returns non-zero if the call
// ? fails.
/* ------------------------------------------------------------------------- */
LLFUNC(End, 1, LuaUtilPushVar(lS, cSql->End()))
/* ========================================================================= **
** ######################################################################### **
** ## Sql.* namespace functions structure                                 ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
LLRSBEGIN                              // Sql.* namespace functions begin
  LLRSFUNC(Active),  LLRSFUNC(Affected),  LLRSFUNC(Begin),
  LLRSFUNC(End),     LLRSFUNC(Error),     LLRSFUNC(ErrorStr),
  LLRSFUNC(Exec),    LLRSFUNC(Reason),    LLRSFUNC(Records),
  LLRSFUNC(Reset),   LLRSFUNC(Size),      LLRSFUNC(Time),
LLRSEND                                // Sql.* namespace functions end
/* ========================================================================= **
** ######################################################################### **
** ## Sql.* namespace constants                                           ## **
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
** ## Sql.* namespace constants structure                                 ## **
** ######################################################################### **
** ========================================================================= */
LLRSCONSTBEGIN                         // Sql.* namespace consts begin
  LLRSCONST(Codes),
LLRSCONSTEND                           // Sql.* namespace consts end
/* ========================================================================= */
}                                      // End of Sql namespace
/* == EoF =========================================================== EoF == */
