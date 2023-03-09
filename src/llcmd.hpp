/* == LLCMD.HPP ============================================================ **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## Defines the 'Command' namespace and methods for the guest to use in ## **
** ## registering their own console commands. This file is invoked by     ## **
** ## 'lualib.hpp'.                                                       ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
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
using namespace IConsole::P;           using namespace ILuaCommand::P;
using namespace Common;
/* ========================================================================= **
** ######################################################################### **
** ## Command common helper classes                                       ## **
** ######################################################################### **
** -- Read Command class argument ------------------------------------------ */
struct AgCommand : public ArClass<Command> {
  explicit AgCommand(lua_State*const lS, const int iArg) :
    ArClass{*LuaUtilGetPtr<Command>(lS, iArg, *cCommands)}{} };
/* -- Create Command class argument ---------------------------------------- */
struct AcCommand : public ArClass<Command> {
  explicit AcCommand(lua_State*const lS) :
    ArClass{*LuaUtilClassCreate<Command>(lS, *cCommands)}{} };
/* ========================================================================= **
** ######################################################################### **
** ## Command:* member functions                                          ## **
** ######################################################################### **
** ========================================================================= */
// $ Command:Destroy
// ? Unregisters the specified console command.
/* ------------------------------------------------------------------------- */
LLFUNC(Destroy, 0, LuaUtilClassDestroy<Command>(lS, 1, *cCommands))
/* ========================================================================= */
// $ Command:Id
// < Id:integer=The id of the Command object.
// ? Returns the unique id of the Command object.
/* ------------------------------------------------------------------------- */
LLFUNC(Id, 1, LuaUtilPushVar(lS, AgCommand{lS, 1}().CtrGet()))
/* ========================================================================= */
// $ Command:Name
// < Name:string=The name of the console command.
// ? Returns the name of the console command this object was registered with.
/* ------------------------------------------------------------------------- */
LLFUNC(Name, 1, AgCommand{lS, 1}().Name())
/* ========================================================================= **
** ######################################################################### **
** ## Command:* member functions structure                                ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
LLRSMFBEGIN                            // Command:* member functions begin
  LLRSFUNC(Destroy), LLRSFUNC(Id), LLRSFUNC(Name),
LLRSEND                                // Command:* member functions end
/* ========================================================================= **
** ######################################################################### **
** ## Command.* namespace functions                                       ## **
** ######################################################################### **
** ========================================================================= */
// $ Command.Exists
// > Identifier:string=The console command name to lookup
// < Registered:boolean=True if the command is registered
// ? Returns if the specified console command is registered which includes
// ? the built-in engine console commands too.
/* ------------------------------------------------------------------------- */
LLFUNC(Exists, 1,
  LuaUtilPushVar(lS, cConsole->CommandIsRegistered(AgNeString{lS, 1})))
/* ========================================================================= */
// $ Command.Register
// > Identifier:string=The console command name.
// > Minimum:integer=The minimum number of parameters allowed.
// > Maximum:integer=The maximum number of parameters allowed.
// > Callback:function=Pointer to the function to callback.
// < Object:Command=The class object created.
// ? Registers the specified console command and returns a reference to the
// ? newly created object which unregisters the console command when garbage
// ? collected.
/* ------------------------------------------------------------------------- */
LLFUNC(Register, 1,
  const AgNeString aIdentifier{lS, 1};
  const AgUInt aMinimum{lS, 2},
               aMaximum{lS, 3};
  LuaUtilCheckFunc(lS, 4);
  AcCommand{lS}().Init(lS, aIdentifier, aMinimum, aMaximum))
/* ========================================================================= **
** ######################################################################### **
** ## Command.* namespace functions structure                             ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
LLRSBEGIN                              // Command.* namespace functions begin
  LLRSFUNC(Exists), LLRSFUNC(Register),
LLRSEND                                // Command.* namespace functions end
/* ========================================================================= */
}                                      // End of Console namespace
/* == EoF =========================================================== EoF == */
