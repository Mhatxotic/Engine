/* == LLATLAS.HPP ========================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## Defines the 'Atlas' namespace and methods for the guest to use in   ## **
** ## Lua. This file is invoked by 'lualib.hpp'.                          ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ========================================================================= **
** ######################################################################### **
** ========================================================================= */
// % Atlas
/* ------------------------------------------------------------------------- */
// ! The atlas class allows dynamic building of an OpenGL texture by allowing
// ! the guest to combine multiple bitmaps into a single texture so there is
// ! less OpenGL texture id switching and fundamentally less CPU usage used.
/* ========================================================================= */
namespace LLAtlas {                    // Atlas namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IAtlas::P;             using namespace IImage::P;
using namespace IImageDef::P;          using namespace IOgl::P;
using namespace ITexture::P;           using namespace Common;
/* ========================================================================= **
** ######################################################################### **
** ## Atlas common helper classes                                         ## **
** ######################################################################### **
** -- Get Atlas class argument --------------------------------------------- */
struct AgAtlas : public ArClass<Atlas>
  { explicit AgAtlas(lua_State*const lS, const int iArg) :
    ArClass{*LuaUtilGetPtr<Atlas>(lS, iArg, *cAtlases)}{} };
/* ========================================================================= */
// $ Atlas:Destroy
// ? Destroys the atlas and frees all the memory associated with it.
/* ------------------------------------------------------------------------- */
LLFUNC(Destroy, 0, LuaUtilClassDestroy<Atlas>(lS, cAtlases))
/* ========================================================================= */
// $ Atlas:Destroyed
// < Destroyed:boolean=If the Atlas class is destroyed
// ? Returns if the Atlas class is destroyed.
/* ------------------------------------------------------------------------- */
LLFUNC(Destroyed, 1,
  LuaUtilPushVar(lS, LuaUtilIsClassDestroyed(lS, cAtlases)))
/* ========================================================================= */
// $ Atlas:Id
// < Id:integer=The id number of the Atlas object.
// ? Returns the unique id of the Atlas object.
/* ------------------------------------------------------------------------- */
LLFUNC(Id, 1, LuaUtilPushVar(lS, AgAtlas{lS, 1}().CtrGet()))
/* ========================================================================= */
// $ Atlas:Name
// < Name:string=The name of the Atlas object.
// ? Returns the name of the specified object when it was created.
/* ------------------------------------------------------------------------- */
LLFUNC(Name, 1, LuaUtilPushVar(lS, AgAtlas{lS, 1}().IdentGet()))
/* ========================================================================= **
** ######################################################################### **
** ## Atlas:* member functions structure                                  ## **
** ######################################################################### **
** ========================================================================= */
LLRSMFBEGIN                            // Atlas:* member functions begin
  LLRSFUNC(Destroy), LLRSFUNC(Destroyed), LLRSFUNC(Id), LLRSFUNC(Name),
LLRSEND                                // Atlas:* member functions end
/* ========================================================================= **
** ######################################################################### **
** ## Atlas.* namespace functions                                         ## **
** ######################################################################### **
** ========================================================================= */
// $ Atlas.Count
// < Count:integer=Total number of atlases created.
// ? Returns the total number of atlas classes currently active.
/* ------------------------------------------------------------------------- */
LLFUNC(Count, 1, LuaUtilPushVar(lS, cAtlases->CollectorCount()))
/* ========================================================================= **
** ######################################################################### **
** ## Atlas.* namespace functions structure                               ## **
** ######################################################################### **
** ========================================================================= */
LLRSBEGIN                              // Atlas.* namespace consts begin
  LLRSFUNC(Count),
LLRSEND                                // Atlas.* namespace consts end
/* ========================================================================= */
}                                      // End of Atlas namespace
/* == EoF =========================================================== EoF == */
