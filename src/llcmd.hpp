/* == LLCMD.HPP ============================================================ **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## Defines the 'Command' namespace and methods for the guest to use in ## **
** ## registering their own console commands. This file is invoked by     ## **
** ## 'lualib.hpp'.                                                       ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ========================================================================= **
** ######################################################################### **
** ========================================================================= */
// % Command
/* ------------------------------------------------------------------------- */
// ! The Command class allows creation and manipulation of console commands
/* ========================================================================= */
namespace LLCommand {                  // Console namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IConsole::P;           using namespace ILua::P;
using namespace ILuaCommand::P;        using namespace Common;
/* ========================================================================= **
** ######################################################################### **
** ## Command common helper classes                                       ## **
** ######################################################################### **
** -- Read Command class argument ------------------------------------------ */
struct AgCommand : public ArClass<Command> {
  explicit AgCommand(lua_State*const lS, const int iArg) :
    ArClass{LuaUtilGetClassRef<Command>(lS, iArg, cCommands)}{} };
/* -- Create Command class argument ---------------------------------------- */
struct AcCommand : public ArClass<Command> {
  explicit AcCommand(lua_State*const lS) :
    ArClass{LuaUtilClassCreateRef<Command>(lS, cCommands)}{} };
/* ========================================================================= **
** ######################################################################### **
** ## Command:* methods                                                   ## **
** ######################################################################### **
** ========================================================================= */
// $ Command:Destroy
// ? Unregisters the specified console command.
/* ------------------------------------------------------------------------- */
LLFUNC(Destroy, 0, LuaUtilClassDestroy<Command>(lS, cCommands))
/* ========================================================================= */
// $ Command:GetId
// < Id:integer=The id of the Command object.
// ? Returns the unique id of the Command object.
/* ------------------------------------------------------------------------- */
LLFUNC(GetId, 1, LuaUtilPushVar(lS, AgCommand{lS, 1}().Serial()))
/* ========================================================================= */
// $ Command:GetName
// < Name:string=The name of the console command.
// ? Returns the name of the console command this object was registered with.
/* ------------------------------------------------------------------------- */
LLFUNC(GetName, 1, AgCommand{lS, 1}().Name())
/* ========================================================================= */
// $ Command:IsDestroyed
// < Destroyed:boolean=If the Command class is destroyed
// ? Returns if the Command class is destroyed.
/* ------------------------------------------------------------------------- */
LLFUNC(IsDestroyed, 1,
  LuaUtilPushVar(lS, LuaUtilIsClassDestroyed(lS, cCommands)))
/* ========================================================================= **
** ######################################################################### **
** ## Command:* methods structure                                         ## **
** ######################################################################### **
** ========================================================================= */
LLRSMFBEGIN                            // Command:* methods begin
  LLRSFUNC(Destroy), LLRSFUNC(GetId), LLRSFUNC(GetName), LLRSFUNC(IsDestroyed),
LLRSEND                                // Command:* methods end
/* ========================================================================= **
** ######################################################################### **
** ## Command.* non-member functions                                      ## **
** ######################################################################### **
** ========================================================================= */
// $ Command.GetClasses
// < Count:integer=Total number of commands created.
// ? Returns the total number of command classes currently active.
/* ------------------------------------------------------------------------- */
LLFUNC(GetClasses, 1, LuaUtilPushVar(lS, cCommands->CollectorCount()))
/* ========================================================================= */
// $ Command.IsExist
// > Name:string=The console command name to lookup
// < Registered:boolean=True if the command is registered
// ? Returns if the specified console command is registered which includes
// ? the built-in engine console commands too.
/* ------------------------------------------------------------------------- */
LLFUNC(IsExist, 1,
  LuaUtilPushVar(lS, cConsole->CommandIsRegistered(AgNeString{lS, 1})))
/* ========================================================================= */
// $ Command.IsValid
// > Cmd:string=Command name to test.
// < Valid:boolean=Command name is valid.
// ? Returns true if the given string has valid characters, false if not.
// ? Console commands must begin with a letter but subsequent characters can
// ? be underscores and numbers. The command must also be between 1 and 255
// ? characters in length.
/* ------------------------------------------------------------------------- */
LLFUNC(IsValid, 1, LuaUtilPushVar(lS,
  cConsole->IsValidConsoleCommandName(AgString{lS, 1}())))
/* ========================================================================= */
// $ Command.Register
// > Name:string=The console command name.
// > Minimum:integer=The minimum number of parameters allowed.
// > Maximum:integer=The maximum number of parameters allowed.
// > Callback:function=Pointer to the function to callback.
// < Object:Command=The class object created.
// ? Registers the specified console command and returns a reference to the
// ? newly created object which unregisters the console command when garbage
// ? collected.
/* ------------------------------------------------------------------------- */
LLFUNC(Register, 1,
  const AgNeString aName{lS, 1};
  const AgUInt aMinimum{lS, 2},
               aMaximum{lS, 3};
  LuaUtilCheckFunc(lS, 4);
  cLua->LuaStateAssert(lS);
  AcCommand{lS}().Init(lS, aName(), aMinimum, aMaximum))
/* ========================================================================= **
** ######################################################################### **
** ## Command.* non-member functions structure                            ## **
** ######################################################################### **
** ========================================================================= */
LLRSBEGIN                              // Command.* non-member functions begin
  LLRSFUNC(GetClasses), LLRSFUNC(IsExist), LLRSFUNC(IsValid),
  LLRSFUNC(Register),
LLRSEND                                // Command.* non-member functions end
/* ========================================================================= */
}                                      // End of Console namespace
/* == EoF =========================================================== EoF == */
