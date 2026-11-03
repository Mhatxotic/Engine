/* == LLSSHOT.HPP ========================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## Allows taking of asynchronous screenshots                           ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ========================================================================= **
** ######################################################################### **
** ========================================================================= */
// % SShot
/* ------------------------------------------------------------------------- */
// ! Contains functions for making a screenshot
/* ========================================================================= */
namespace LLSShot {                    // Screenshot namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace ICommon::P;            using namespace IFbo::P;
using namespace ISShot::P;             using namespace IString::P;
using namespace Common;
/* ========================================================================= **
** ######################################################################### **
** ## SShot common helper classes                                         ## **
** ######################################################################### **
** -- Read SShot class argument -------------------------------------------- */
struct AgSShot : public ArClass<SShot> {
  explicit AgSShot(lua_State*const lS, const int iArg) :
    ArClass{LuaUtilGetClassRef<SShot>(lS, iArg, cSShots)}{} };
/* -- Create SShot class argument ------------------------------------------ */
struct AcSShot : public ArClass<SShot> {
  explicit AcSShot(lua_State*const lS) :
    ArClass{LuaUtilClassCreateRef<SShot>(lS, cSShots)}{} };
/* ========================================================================= **
** ######################################################################### **
** ## SShot:* methods                                                     ## **
** ######################################################################### **
** ========================================================================= */
// $ SShot:Destroy
// ? Destroys the specified screenshot object.
/* ------------------------------------------------------------------------- */
LLFUNC(Destroy, 0, LuaUtilClassDestroy<SShot>(lS, cSShots))
/* ========================================================================= */
// $ SShot:GetId
// < Id:integer=The id number of the SShot object.
// ? Returns the unique id of the SShot object.
/* ------------------------------------------------------------------------- */
LLFUNC(GetId, 1, LuaUtilPushVar(lS, AgSShot{lS, 1}().Serial()))
/* ========================================================================= */
// $ SShot:GetName
// < Name:string=The name of the SShot object.
// ? Returns the filename generated on this screenshot object.
/* ------------------------------------------------------------------------- */
LLFUNC(GetName, 1, LuaUtilPushVar(lS, AgSShot{lS, 1}().NameGet()))
/* ========================================================================= */
// $ SShot:IsDestroyed
// < Destroyed:boolean=If the SShot class is destroyed
// ? Returns if the SShot class is destroyed.
/* ------------------------------------------------------------------------- */
LLFUNC(IsDestroyed, 1,
  LuaUtilPushVar(lS, LuaUtilIsClassDestroyed(lS, cSShots)))
/* ========================================================================= **
** ######################################################################### **
** ## SShot:* methods structure                                           ## **
** ######################################################################### **
** ========================================================================= */
LLRSMFBEGIN                            // SShot:* methods begin
  LLRSFUNC(Destroy), LLRSFUNC(GetId), LLRSFUNC(GetName), LLRSFUNC(IsDestroyed),
LLRSEND                                // SShot:* methods end
/* ========================================================================= **
** ######################################################################### **
** ## SShot.* non-member functions                                        ## **
** ######################################################################### **
** ========================================================================= */
// $ SShot.GetClasses
// < Count:integer=Total number of sshots created.
// ? Returns the total number of sshot classes currently active.
/* ------------------------------------------------------------------------- */
LLFUNC(GetClasses, 1, LuaUtilPushVar(lS, cSShots->CollectorCount()))
/* ========================================================================= */
// $ SShot.Fbo
// > Dest:Fbo=The frame buffer to dump.
// > File:string=(Optional) File to save to.
// < Object:SShot=The screenshot object created.
// ? Takes a screenshot of the specified Fbo. You must completely omit the
// ? 'file' parameter to use an engine generated filename.
/* ------------------------------------------------------------------------- */
LLFUNC(Fbo, 1,
  const AgFbo aFbo{lS, 1};
  AcSShot{lS}().DumpFbo(aFbo,
    LuaBaseGetTop(lS) < 3 ? cCommon->CommonBlank() : AgFilename{lS,2}()))
/* ========================================================================= */
// $ SShot.Screen
// < Object:SShot=The screenshot object created.
// ? Takes a screenshot of the screen.
/* ------------------------------------------------------------------------- */
LLFUNC(Screen, 1, AcSShot{lS}().DumpMain())
/* ========================================================================= **
** ######################################################################### **
** ## SShot.* non-member functions structure                              ## **
** ######################################################################### **
** ========================================================================= */
LLRSBEGIN                              // SShot.* non-member functions begin
  LLRSFUNC(Fbo), LLRSFUNC(GetClasses), LLRSFUNC(Screen),
LLRSEND                                // SShot.* non-member functions end
/* ========================================================================= */
}                                      // End of Util namespace
/* == EoF =========================================================== EoF == */
