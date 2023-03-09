/* == LLVAR.HPP ============================================================ **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## Defines the 'Variable' namespace and methods for the guest to use   ## **
** ## in registering their own cvars. This file is invoked by             ## **
** ## 'lualib.hpp'.                                                       ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
#pragma once                           // Only one incursion allowed
/* ========================================================================= **
** ######################################################################### **
** ========================================================================= */
// % Variable
/* ------------------------------------------------------------------------- */
// ! The Variable class allows creation and manipulation of console commands
/* ========================================================================= */
namespace LLVariable {                 // Console namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace ICVar::P;              using namespace ICVarLib::P;
using namespace ILuaVariable::P;       using namespace Common;
/* ========================================================================= **
** ######################################################################### **
** ## Variable common helper classes                                      ## **
** ######################################################################### **
** -- Read Variable class argument ----------------------------------------- */
struct AgVariable : public ArClass<Variable> {
  explicit AgVariable(lua_State*const lS, const int iArg) :
    ArClass{*LuaUtilGetPtr<Variable>(lS, iArg, *cVariables)}{} };
/* -- Create Variable class argument --------------------------------------- */
struct AcVariable : public ArClass<Variable> {
  explicit AcVariable(lua_State*const lS) :
    ArClass{*LuaUtilClassCreate<Variable>(lS, *cVariables)}{} };
/* -- Get Variable flags --------------------------------------------------- */
struct AgCVarId : public AgIntegerLGE<CVarEnums>
  { explicit AgCVarId(lua_State*const lS, const int iArg) :
      AgIntegerLGE{ lS, iArg, CVAR_FIRST, CVAR_MAX } {} };
/* ========================================================================= */
// $ Variable:Destroy
// ? Unregisters the specified console command.
/* ------------------------------------------------------------------------- */
LLFUNC(Destroy, 0, LuaUtilClassDestroy<Variable>(lS, 1, *cVariables))
/* ========================================================================= */
// $ Variable:Empty
// < Empty:boolean=Is the value empty?
// ? Returns if the value is empty.
/* ------------------------------------------------------------------------- */
LLFUNC(Empty, 1, LuaUtilPushVar(lS, AgVariable{lS, 1}().Empty()))
/* ========================================================================= */
// $ Variable:Id
// < Id:integer=The id of the Variable object.
// ? Returns the unique id of the Variable object.
/* ------------------------------------------------------------------------- */
LLFUNC(Id, 1, LuaUtilPushVar(lS, AgVariable{lS, 1}().CtrGet()))
/* ========================================================================= */
// $ Variable:Name
// < Name:string=The name of the console command.
// ? Returns the name of the console command this object was registered with.
/* ------------------------------------------------------------------------- */
LLFUNC(Name, 1, LuaUtilPushVar(lS, AgVariable{lS, 1}().Name()))
/* ========================================================================= */
// $ Variable:Default
// < Value:string=The engine default cvar value.
// ? Retrieves the value of the specified cvar name. An exception is raised if
// ? the specified cvar does not exist.
/* ------------------------------------------------------------------------- */
LLFUNC(Default, 1, LuaUtilPushVar(lS, AgVariable{lS, 1}().Default()))
/* ========================================================================= */
// $ Variable:Get
// < Value:mixed=The engine cvar value.
// ? Retrieves the value of the specified cvar name. An exception is raised if
// ? the specified cvar does not exist.
/* ------------------------------------------------------------------------- */
LLFUNC(Get, 1, LuaUtilPushVar(lS, AgVariable{lS, 1}().Get()))
/* ========================================================================= */
// $ Variable:Reset
// ? Resets the cvar to the default value as registered.
/* ------------------------------------------------------------------------- */
LLFUNC(Reset, 0, AgVariable{lS, 1}().Reset())
/* ========================================================================= */
// $ Variable:Set
// > String:string=The new cvar value.
// < Success:integer=The return code.
// ? Sets the new value of the specified cvar name. An exception is raised if
// ? any error occurs. See Variable.Result to see the possible results.
/* ------------------------------------------------------------------------- */
LLFUNC(Set, 1, LuaUtilPushVar(lS, AgVariable{lS, 1}().Set(AgString{lS, 2})))
/* ========================================================================= **
** ######################################################################### **
** ## Variable:* member functions structure                               ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
LLRSMFBEGIN                            // Variable:* member functions begin
  LLRSFUNC(Default), LLRSFUNC(Destroy), LLRSFUNC(Empty), LLRSFUNC(Get),
  LLRSFUNC(Id),      LLRSFUNC(Name),    LLRSFUNC(Reset), LLRSFUNC(Set),
LLRSEND                                // Variable:* member functions end
/* ========================================================================= **
** ######################################################################### **
** ## Variable.* namespace functions                                      ## **
** ######################################################################### **
** ========================================================================= */
// $ Variable.Register
// > Name:string=The engine cvar name.
// > Default:string=The default cvar value.
// > Flags:integer=The cvar flags.
// > Callback:function=The callback global function name.
// < Class:Variable=The Variable object created.
// ? Registers the specified cvar name with the specified default value. The
// ? default value is ignored if there is already a saved value in the Sql
// ? cvars database. If the saved value is a corrupted encrypted value then
// ? the default is used. If the cvar already exists then an exception is
// ? thrown so make sure you check for it first if do not expect this call
// ? to garauntee success. Normally it is unless there is a memory allocation
// ? issue. You can register an unlimited amount of variables.
// ? See Variable.Flags to see available flags.
/* ------------------------------------------------------------------------- */
LLFUNC(Register, 1, AcVariable{lS}().Init(lS))
/* ========================================================================= */
// $ Variable.Exists
// > String:string=The console command name to lookup
// < Registered:boolean=True if the command is registered
// ? Returns if the specified console command is registered which includes
// ? the built-in engine console commands too.
/* ------------------------------------------------------------------------- */
LLFUNC(Exists, 1, LuaUtilPushVar(lS, cCVars->VarExists(AgNeString{lS,1})))
/* ========================================================================= */
// $ Variable.GetInt
// > Id:integer=The engine cvar index.
// < Value:mixed=The engine internal id value.
// ? Retrieves the value of the specified cvar given at the specified id. The
// ? id's are populated as key/value pairs in the 'Variable.Internal' table.
/* ------------------------------------------------------------------------- */
LLFUNC(GetInt, 1,
  LuaUtilPushVar(lS, cCVars->GetStrInternal(AgCVarId{lS, 1})))
/* ========================================================================= */
// $ Variable.SetInt
// > Id:integer=The engine cvar index.
// > Value:string=The new engine cvar value.
// ? Sets the new value of the specified engine cvar name. An exception is
// ? raised if any error occurds. See Variable.Result to see the possible
// ? results.
/* ------------------------------------------------------------------------- */
LLFUNC(SetInt, 1,
  const AgCVarId aCVarId{lS, 1};
  const AgString aValue{lS, 2};
  LuaUtilPushVar(lS, cCVars->SetInternal(aCVarId, aValue())))
/* ========================================================================= */
// $ Variable.ResetInt
// > Id:integer=The engine cvar index.
// ? Resets the specified cvar to its default variable.
/* ------------------------------------------------------------------------- */
LLFUNC(ResetInt, 1, LuaUtilPushVar(lS, cCVars->Reset(AgCVarId{lS, 1})))
/* ========================================================================= */
// $ Variable.Save
// < Count:number=Number of items saved
// ? Commits all modified variables to database. Returns number of items
// ? written.
/* ------------------------------------------------------------------------- */
LLFUNC(Save, 1, LuaUtilPushVar(lS, cCVars->Save()))
/* ========================================================================= **
** ######################################################################### **
** ## Variable.* namespace functions structure                            ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
LLRSBEGIN                              // Variable.* namespace functions begin
  LLRSFUNC(Exists),   LLRSFUNC(GetInt), LLRSFUNC(Register),
  LLRSFUNC(ResetInt), LLRSFUNC(Save),   LLRSFUNC(SetInt),
LLRSEND                                // Variable.* namespace functions end
/* ========================================================================= **
** ######################################################################### **
** ## Variable.* namespace constants                                      ## **
** ######################################################################### **
** ========================================================================= */
// @ Variable.Flags
// < Data:table=A table of const string/int key pairs
// ? Returns all the cvar flags supported by the engine in the form of a
// ? string/int key pairs which can be used with the Variable.Register()
// ? command.
/* ------------------------------------------------------------------------- */
LLRSKTBEGIN(Flags)                     // Beginning of cvar register flags
  LLRSKTITEM(T,STRING),                LLRSKTITEM(T,INTEGER),
  LLRSKTITEM(T,FLOAT),                 LLRSKTITEM(T,BOOLEAN),
  LLRSKTITEM(T,UINTEGER),              LLRSKTITEM(T,UFLOAT),
  LLRSKTITEM(T,INTEGERSAVE),           LLRSKTITEM(T,FLOATSAVE),
  LLRSKTITEM(T,UINTEGERSAVE),          LLRSKTITEM(T,UFLOATSAVE),
  LLRSKTITEM(T,STRINGSAVE),            LLRSKTITEM(T,BOOLEANSAVE),
  LLRSKTITEM(C,SAVEABLE),              LLRSKTITEM(C,PROTECTED),
  LLRSKTITEM(C,NOTEMPTY),              LLRSKTITEM(C,UNSIGNED),
  LLRSKTITEM(C,POW2),                  LLRSKTITEM(C,DEFLATE),
  LLRSKTITEM(C,ALPHA),                 LLRSKTITEM(C,FILENAME),
  LLRSKTITEM(C,NUMERIC),               LLRSKTITEM(M,TRIM),
  LLRSKTITEM(O,SAVEFORCE),
LLRSKTEND                              // End of cvar register status flags
/* ========================================================================= */
// @ Variable.Result
// < Data:table=A table of const string/int key pairs
// ? Returns the possible values that can be returned with the Variable.Set()
// ? function. Any other errors are thrown as an exception.
/* ------------------------------------------------------------------------- */
LLRSKTBEGIN(Result)                    // Beginning of cvar result flags
  LLRSKTITEM(CVS_,OK),                 LLRSKTITEM(CVS_,OKNOTCHANGED),
LLRSKTEND                              // End of cvar result flags
/* ========================================================================= **
** ######################################################################### **
** ## Variable.* namespace constants structure                            ## **
** ######################################################################### **
** ========================================================================= */
LLRSCONSTBEGIN                         // Variable.* namespace consts begin
  LLRSCONST(Flags),                    LLRSCONST(Result),
LLRSCONSTEND                           // Variable.* namespace consts end
/* ========================================================================= */
}                                      // End of Console namespace
/* == EoF =========================================================== EoF == */
