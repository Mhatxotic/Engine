/* == LLSSHOT.HPP ========================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## Allows taking of asynchronous screenshots                           ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
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
using namespace IFbo::P;               using namespace ISShot::P;
using namespace IString::P;            using namespace Common;
/* ========================================================================= **
** ######################################################################### **
** ## SShot common helper classes                                         ## **
** ######################################################################### **
** -- Read SShot class argument -------------------------------------------- */
struct AgSShot : public ArClass<SShot> {
  explicit AgSShot(lua_State*const lS, const int iArg) :
    ArClass{*LuaUtilGetPtr<SShot>(lS, iArg, *cSShots)}{} };
/* -- Create SShot class argument ------------------------------------------ */
struct AcSShot : public ArClass<SShot> {
  explicit AcSShot(lua_State*const lS) :
    ArClass{*LuaUtilClassCreate<SShot>(lS, *cSShots)}{} };
/* ========================================================================= **
** ######################################################################### **
** ## SShot:* member functions                                            ## **
** ######################################################################### **
** ========================================================================= */
// $ SShot:Destroy
// ? Destroys the specified screenshot object.
/* ------------------------------------------------------------------------- */
LLFUNC(Destroy, 0, LuaUtilClassDestroy<SShot>(lS, 1, *cSShots))
/* ========================================================================= */
// $ SShot:Id
// < Id:integer=The id number of the SShot object.
// ? Returns the unique id of the SShot object.
/* ------------------------------------------------------------------------- */
LLFUNC(Id, 1, LuaUtilPushVar(lS, AgSShot{lS, 1}().CtrGet()))
/* ========================================================================= */
// $ SShot:Name
// < Name:string=The name of the SShot object.
// ? Returns the filename generated on this screenshot object.
/* ------------------------------------------------------------------------- */
LLFUNC(Name, 1, LuaUtilPushVar(lS, AgSShot{lS, 1}().IdentGet()))
/* ========================================================================= **
** ######################################################################### **
** ## SShot:* member functions structure                                  ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
LLRSMFBEGIN                            // SShot:* member functions begin
  LLRSFUNC(Destroy), LLRSFUNC(Id), LLRSFUNC(Name),
LLRSEND                                // SShot:* member functions end
/* ========================================================================= **
** ######################################################################### **
** ## SShot.* namespace functions                                         ## **
** ######################################################################### **
** ========================================================================= */
// $ SShot.Screen
// < Object:SShot=The screenshot object created.
// ? Takes a screenshot of the screen.
/* ------------------------------------------------------------------------- */
LLFUNC(Screen, 1, AcSShot{lS}().DumpMain())
/* ========================================================================= */
// $ SShot.Fbo
// > Dest:Fbo=The frame buffer to dump.
// > File:string=(Optional) File to save to.
// < Object:SShot=The screenshot object created.
// ? Takes a screenshot of the specified FBO. You must completely omit the
// ? 'file' parameter to use an engine generated file.
/* ------------------------------------------------------------------------- */
LLFUNC(Fbo, 1, const AgFbo aFbo{lS,1};
  AcSShot{lS}().DumpFBO(aFbo,
    LuaUtilStackSize(lS) < 3 ? cCommon->Blank() : AgFilename{lS,2}))
/* ========================================================================= **
** ######################################################################### **
** ## SShot.* namespace functions structure                               ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
LLRSBEGIN                              // SShot.* namespace functions begin
  LLRSFUNC(Fbo), LLRSFUNC(Screen),
LLRSEND                                // SShot.* namespace functions end
/* ========================================================================= */
}                                      // End of Util namespace
/* == EoF =========================================================== EoF == */
