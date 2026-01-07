/* == LLVAR.HPP ============================================================ **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## Defines the 'Variable' namespace and methods for the guest to use   ## **
** ## in registering their own cvars. This file is invoked by             ## **
** ## 'lualib.hpp'.                                                       ## **
** ######################################################################### **
** ========================================================================= */
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
using namespace ILua::P;               using namespace ILuaVariable::P;
using namespace Common;
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
// $ Variable:Boolean
// > State:boolean=The new cvar value.
// < Success:integer=The return code.
// ? Sets the new value based on the specified boolean. "0" if false or "1" if
// ? true. See Variable.Result to see the possible results.
/* ------------------------------------------------------------------------- */
LLFUNC(Boolean, 1, LuaUtilPushVar(lS,
  AgVariable{lS, 1}().SetBoolean(AgBoolean{lS, 2})))
/* ========================================================================= */
// $ Variable:Clear
// < Success:integer=The return code.
// ? Clears the variable (with an empty string). See Variable.Result to see
// ?the possible results.
/* ------------------------------------------------------------------------- */
LLFUNC(Clear, 1, LuaUtilPushVar(lS, AgVariable{lS, 1}().Clear()))
/* ========================================================================= */
// $ Variable:Destroy
// ? Unregisters the specified console command.
/* ------------------------------------------------------------------------- */
LLFUNC(Destroy, 0, LuaUtilClassDestroy<Variable>(lS, cVariables))
/* ========================================================================= */
// $ Variable:Destroyed
// < Destroyed:boolean=If the Variable class is destroyed
// ? Returns if the Variable class is destroyed.
/* ------------------------------------------------------------------------- */
LLFUNC(Destroyed, 1,
  LuaUtilPushVar(lS, LuaUtilIsClassDestroyed(lS, cVariables)))
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
// $ Variable:Integer
// > Value:integer=The new cvar value.
// < Success:integer=The return code.
// ? Sets the new value based on the specified integer. See Variable.Result to
// ? see the possible results.
/* ------------------------------------------------------------------------- */
LLFUNC(Integer, 1, LuaUtilPushVar(lS,
  AgVariable{lS, 1}().SetInteger(AgLuaInteger{lS, 2})))
/* ========================================================================= */
// $ Variable:NotEmpty
// < NotEmpty:boolean=Is the value not empty?
// ? Returns if the value is not empty.
/* ------------------------------------------------------------------------- */
LLFUNC(NotEmpty, 1, LuaUtilPushVar(lS, AgVariable{lS, 1}().NotEmpty()))
/* ========================================================================= */
// $ Variable:Number
// > Value:numberr=The new cvar value.
// < Success:integer=The return code.
// ? Sets the new value based on the specified number. See Variable.Result to
// ? see the possible results.
/* ------------------------------------------------------------------------- */
LLFUNC(Number, 1, LuaUtilPushVar(lS,
  AgVariable{lS, 1}().SetNumber(AgLuaNumber{lS, 2})))
/* ========================================================================= */
// $ Variable:Reset
// ? Resets the cvar to the default value as registered.
/* ------------------------------------------------------------------------- */
LLFUNC(Reset, 0, AgVariable{lS, 1}().Reset())
/* ========================================================================= */
// $ Variable:String
// > String:string=The new cvar value.
// < Success:integer=The return code.
// ? Sets the new value of the specified cvar name. An exception is raised if
// ? any error occurs. See Variable.Result to see the possible results.
/* ------------------------------------------------------------------------- */
LLFUNC(String, 1,
  LuaUtilPushVar(lS, AgVariable{lS, 1}().SetString(AgString{lS, 2})))
/* ========================================================================= **
** ######################################################################### **
** ## Variable:* member functions structure                               ## **
** ######################################################################### **
** ========================================================================= */
LLRSMFBEGIN                            // Variable:* member functions begin
  LLRSFUNC(Boolean),   LLRSFUNC(Clear), LLRSFUNC(Default), LLRSFUNC(Destroy),
  LLRSFUNC(Destroyed), LLRSFUNC(Empty), LLRSFUNC(Get),     LLRSFUNC(Id),
  LLRSFUNC(Integer),   LLRSFUNC(Name),  LLRSFUNC(NotEmpty),LLRSFUNC(Number),
  LLRSFUNC(Reset),     LLRSFUNC(String),
LLRSEND                                // Variable:* member functions end
/* ========================================================================= **
** ######################################################################### **
** ## Variable.* namespace functions                                      ## **
** ######################################################################### **
** ========================================================================= */
// $ Variable.Count
// < Count:integer=Total number of variables created.
// ? Returns the total number of variable classes currently active.
/* ------------------------------------------------------------------------- */
LLFUNC(Count, 1, LuaUtilPushVar(lS, cVariables->CollectorCount()))
/* ========================================================================= */
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
// ? issue. The amount of cvars you can register is limited by the integral
// ? value of the 'obj_cvarmax' cvar. The syntax of the callback function is
// ? function(V:string){return RR:boolean,RV:string} where 'V' is the value
// ? string that was requested to be set. If 'RV' is not set then 'RR' is
// ? whether to accept (true) or deny (false) the requested change to 'V',
// ? otherwise, if 'RV' is set to a string then the actual cvar value is
// ? replaced by the 'RV' string and 'RR' refers to if the variable should be
// ? force committed to disk (true) or not (false). Note that variable values
// ? that are equal to the default value are not committed to the database.
/* ------------------------------------------------------------------------- */
LLFUNC(Register, 1, cLua->LuaStateAssert(lS);AcVariable{lS}().Init(lS))
/* ========================================================================= */
// $ Variable.Exists
// > String:string=The console command name to lookup
// < Registered:boolean=True if the command is registered
// ? Returns if the specified console command is registered which includes
// ? the built-in engine console commands too.
/* ------------------------------------------------------------------------- */
LLFUNC(Exists, 1, LuaUtilPushVar(lS, cCVars->VarExists(AgNeString{lS, 1})))
/* ========================================================================= */
// $ Variable.Save
// < Count:number=Number of items saved
// ? Commits all modified variables to database. Returns number of items
// ? written.
/* ------------------------------------------------------------------------- */
LLFUNC(Save, 1, LuaUtilPushVar(lS, cCVars->CVarsSaveToDatabase()))
/* ========================================================================= */
// $ Variable.Valid
// > Var:string=Cvar name to test.
// < Valid:boolean=Cvar name is valid.
// ? Returns true if the given string has valid characters, false if not. CVars
// ? must begin with a letter but subsequent characters can be underscores and
// ? numbers. The name must also be between 5 and 255 characters in length.
/* ------------------------------------------------------------------------- */
LLFUNC(Valid, 1, LuaUtilPushVar(lS,
  cCVars->IsValidVariableName(AgString{lS, 1})))
/* ========================================================================= **
** ######################################################################### **
** ## Variable.* namespace functions structure                            ## **
** ######################################################################### **
** ========================================================================= */
LLRSBEGIN                              // Variable.* namespace functions begin
  LLRSFUNC(Count), LLRSFUNC(Exists), LLRSFUNC(Register), LLRSFUNC(Save),
  LLRSFUNC(Valid),
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
LLRSKTEND                              // End of cvar register status flags
/* ========================================================================= */
// @ Variable.Result
// < Data:table=A table of const string/int key pairs
// ? Returns the possible values that can be returned with the Variable.Set()
// ? function. Any other errors are thrown as an exception.
/* ------------------------------------------------------------------------- */
LLRSKTBEGIN(Result)                    // Beginning of cvar result flags
  LLRSKTITEM(CVS_,OK),                 LLRSKTITEM(CVS_,OKNOTCHANGED),
  LLRSKTITEM(CVS_,NOTFOUND),           LLRSKTITEM(CVS_,NOTWRITABLE),
  LLRSKTITEM(CVS_,NOTINTEGER),         LLRSKTITEM(CVS_,NOTFLOAT),
  LLRSKTITEM(CVS_,NOTBOOLEAN),         LLRSKTITEM(CVS_,NOTUNSIGNED),
  LLRSKTITEM(CVS_,NOTPOW2),            LLRSKTITEM(CVS_,NOTALPHA),
  LLRSKTITEM(CVS_,NOTNUMERIC),         LLRSKTITEM(CVS_,NOTALPHANUMERIC),
  LLRSKTITEM(CVS_,NOTFILENAME),        LLRSKTITEM(CVS_,TRIGGERDENIED),
  LLRSKTITEM(CVS_,TRIGGEREXCEPTION),   LLRSKTITEM(CVS_,EMPTY),
  LLRSKTITEM(CVS_,ZERO),               LLRSKTITEM(CVS_,NOTYPESET),
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
