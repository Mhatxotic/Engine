/* == LLCLIP.HPP =========================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## Interfaces Lua to the glfw clipboard functions.                     ## **
** ######################################################################### **
** ========================================================================= */
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
    ArClass{LuaUtilGetClassRef<Clip>(lS, iArg, cClips)}{} };
/* -- Create Clip class argument ------------------------------------------- */
struct AcClip : public ArClass<Clip> {
  explicit AcClip(lua_State*const lS) :
    ArClass{LuaUtilClassCreateRef<Clip>(lS, cClips)}{} };
/* ========================================================================= **
** ######################################################################### **
** ## Clip:* methods                                                      ## **
** ######################################################################### **
** ========================================================================= */
// $ Clip:Destroy
// ? Destroys the specified Clip object.
/* ------------------------------------------------------------------------- */
LLFUNC(Destroy, 0, LuaUtilClassDestroyChecked<Clip>(lS, cClips))
/* ========================================================================= */
// $ Clip:GetId
// < Id:integer=The id number of the Clip object.
// ? Returns the unique id of the Clip object.
/* ------------------------------------------------------------------------- */
LLFUNC(GetId, 1, LuaUtilPushVar(lS, AgClip{lS, 1}().Serial()))
/* ========================================================================= */
// $ Clip:GetName
// < Name:string=The identifier given at creation.
// ? Returns the identifier of this request given at creation.
/* ------------------------------------------------------------------------- */
LLFUNC(GetName, 1, AgClip{lS, 1}().NameGet())
/* ========================================================================= */
// $ Clip:GetValue
// < Value:string=The value retrieved from or sent to the Clip.
// ? Returns the value sent with Clip.Set or the value retrieved with
// ? Clip.Get.
/* ------------------------------------------------------------------------- */
LLFUNC(GetValue, 1, AgClip{lS, 1}().ClipGet())
/* ========================================================================= */
// $ Clip:IsDestroyed
// < Destroyed:boolean=If the Clip class is destroyed
// ? Returns if the Clip class is destroyed.
/* ------------------------------------------------------------------------- */
LLFUNC(IsDestroyed, 1, LuaUtilPushVar(lS, LuaUtilIsClassDestroyed(lS, cClips)))
/* ========================================================================= **
** ######################################################################### **
** ## Clip:* methods structure                                            ## **
** ######################################################################### **
** ========================================================================= */
LLRSMFBEGIN                            // Clip:* methods begin
  LLRSFUNC(Destroy),  LLRSFUNC(GetId),       LLRSFUNC(GetName),
  LLRSFUNC(GetValue), LLRSFUNC(IsDestroyed),
LLRSEND                                // Clip:* methods end
/* ========================================================================= **
** ######################################################################### **
** ## Clip.* non-member functions                                         ## **
** ######################################################################### **
** ========================================================================= */
// $ Clip.Get
// > Name:string=The identifier for the request.
// > Completion:function=The function to call when completed.
// ? Retrieves text from the operating systems clipboard. Executes the
// ? specified function when completed as the window thread has to request
// ? the operation and then forward the request back to the engine thread.
/* ------------------------------------------------------------------------- */
LLFUNC(Get, 0,
  LuaUtilCheckParams(lS, 2);
  const AgNeString aName{lS, 1};
  LuaUtilCheckFunc(lS, 2);
  AcClip{lS}().ClipGetAsync(lS, aName()))
/* ========================================================================= */
// $ Clip.GetClasses
// < Count:integer=Total number of clips created.
// ? Returns the total number of clip classes currently active.
/* ------------------------------------------------------------------------- */
LLFUNC(GetClasses, 1, LuaUtilPushVar(lS, cClips->CollectorCount()))
/* ========================================================================= */
// $ Clip.Set
// > Name:string=The identifier for the request.
// > Text:string=The text to copy.
// > Completion:function=The function to call when completed.
// ? Stores the specified text into the operating system's clipboard and
// ? executes the specified completion function when done as the window thread
// ? has to request the operation and then forward the request back to the
// ? engine thread.
/* ------------------------------------------------------------------------- */
LLFUNC(Set, 0,
  LuaUtilCheckParams(lS, 3);
  const AgNeString aName{lS, 1};
  const AgString aText{lS, 2};
  LuaUtilCheckFunc(lS, 3);
  AcClip{lS}().ClipSetAsync(lS, aName(), aText))
/* ========================================================================= **
** ######################################################################### **
** ## Clip.* non-member functions structure                               ## **
** ######################################################################### **
** ========================================================================= */
LLRSBEGIN                              // Clip.* non-member functions begin
  LLRSFUNC(Get), LLRSFUNC(GetClasses), LLRSFUNC(Set),
LLRSEND                                // Clip.* non-member functions end
/* ========================================================================= */
}                                      // End of Util namespace
/* == EoF =========================================================== EoF == */
