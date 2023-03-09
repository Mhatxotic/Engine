/* == LLCLIP.HPP =========================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## Interfaces Lua to the glfw clipboard functions.                     ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
#pragma once                           // Only one incursion allowed
/* ========================================================================= **
** ######################################################################### **
** ========================================================================= */
// % Clip
/* ------------------------------------------------------------------------- */
// ! Contains functions for retrieving and setting text in the Clip.
/* ========================================================================= */
namespace LLClip {                     // Clipboard namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IClipboard::P;         using namespace Common;
/* ========================================================================= **
** ######################################################################### **
** ## Clip common helper classes                                          ## **
** ######################################################################### **
** -- Read Clip class argument --------------------------------------------- */
struct AgClip : public ArClass<Clip> {
  explicit AgClip(lua_State*const lS, const int iArg) :
    ArClass{*LuaUtilGetPtr<Clip>(lS, iArg, *cClips)}{} };
/* -- Create Clip class argument ------------------------------------------- */
struct AcClip : public ArClass<Clip> {
  explicit AcClip(lua_State*const lS) :
    ArClass{*LuaUtilClassCreate<Clip>(lS, *cClips)}{} };
/* ========================================================================= **
** ######################################################################### **
** ## Clip:* member functions                                             ## **
** ######################################################################### **
** ========================================================================= */
// $ Clip:Destroy
// ? Destroys the specified Clip object.
/* ------------------------------------------------------------------------- */
LLFUNC(Destroy, 0, LuaUtilClassDestroy<Clip>(lS, 1, *cClips))
/* ========================================================================= */
// $ Clip:Id
// < Id:integer=The id number of the Clip object.
// ? Returns the unique id of the Clip object.
/* ------------------------------------------------------------------------- */
LLFUNC(Id, 1, LuaUtilPushVar(lS, AgClip{lS, 1}().CtrGet()))
/* ========================================================================= */
// $ Clip:Name
// < Identifier:string = The identifier given at creation.
// ? Returns the identifier of this request given at creation.
/* ------------------------------------------------------------------------- */
LLFUNC(Name, 1, AgClip{lS, 1}().IdentGet())
/* ========================================================================= */
// $ Clip:Value
// < Value:string = The value retrieved from or sent to the Clip.
// ? Returns the value sent with Clip.Set or the value retrieved with
// ? Clip.Get.
/* ------------------------------------------------------------------------- */
LLFUNC(Value, 1, AgClip{lS, 1}().ClipGet())
/* ========================================================================= **
** ######################################################################### **
** ## Clip:* member functions structure                                   ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
LLRSMFBEGIN                            // Clip:* member functions begin
  LLRSFUNC(Destroy), LLRSFUNC(Id), LLRSFUNC(Name), LLRSFUNC(Value),
LLRSEND                                // Clip:* member functions end
/* ========================================================================= **
** ######################################################################### **
** ## Clip.* namespace functions                                          ## **
** ######################################################################### **
** ========================================================================= */
// $ Clip.Get
// > Identifier:string=The identifier for the request.
// > Completion:function=The function to call when completed.
// ? Retrieves text from the operating systems clipboard. Executes the
// ? specified function when completed as the window thread has to request
// ? the operation and then forward the request back to the engine thread.
/* ------------------------------------------------------------------------- */
LLFUNC(Get, 0,
  LuaUtilCheckParams(lS, 2);
  const AgNeString aIdentifier{lS, 1};
  LuaUtilCheckFunc(lS, 2);
  AcClip{lS}().ClipGetAsync(lS, aIdentifier))
/* ========================================================================= */
// $ Clip.Set
// > Identifier:string=The identifier for the request.
// > Text:string=The text to copy.
// > Completion:function=The function to call when completed.
// ? Stores the specified text into the operating system's clipboard and
// ? executes the specified completion function when done as the window thread
// ? has to request the operation and then forward the request back to the
// ? engine thread.
/* ------------------------------------------------------------------------- */
LLFUNC(Set, 0,
  LuaUtilCheckParams(lS, 3);
  const AgNeString aIdentifier{lS, 1};
  const AgString aText{lS, 2};
  LuaUtilCheckFunc(lS, 3);
  AcClip{lS}().ClipSetAsync(lS, aIdentifier, aText))
/* ========================================================================= **
** ######################################################################### **
** ## Clip.* namespace functions structure                                ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
LLRSBEGIN                              // Clip.* namespace functions begin
  LLRSFUNC(Get), LLRSFUNC(Set),
LLRSEND                                // Clip.* namespace functions end
/* ========================================================================= */
}                                      // End of Util namespace
/* == EoF =========================================================== EoF == */
