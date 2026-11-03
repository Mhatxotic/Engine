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
    ArClass{LuaUtilGetClassRef<Atlas>(lS, iArg, cAtlases)}{} };
/* ========================================================================= **
** ######################################################################### **
** ## Atlas:* methods                                                     ## **
** ######################################################################### **
** ========================================================================= */
/* ========================================================================= */
// $ Atlas:Destroy
// ? Destroys the atlas and frees all the memory associated with it.
/* ------------------------------------------------------------------------- */
LLFUNC(Destroy, 0, LuaUtilClassDestroy<Atlas>(lS, cAtlases))
/* ========================================================================= */
// $ Atlas:GetId
// < Id:integer=The id number of the Atlas object.
// ? Returns the unique id of the Atlas object.
/* ------------------------------------------------------------------------- */
LLFUNC(GetId, 1, LuaUtilPushVar(lS, AgAtlas{lS, 1}().Serial()))
/* ========================================================================= */
// $ Atlas:GetName
// < Name:string=The name of the Atlas object.
// ? Returns the name of the specified object when it was created.
/* ------------------------------------------------------------------------- */
LLFUNC(GetName, 1, LuaUtilPushVar(lS, AgAtlas{lS, 1}().NameGet()))
/* ========================================================================= */
// $ Atlas:IsDestroyed
// < Destroyed:boolean=If the Atlas class is destroyed
// ? Returns if the Atlas class is destroyed.
/* ------------------------------------------------------------------------- */
LLFUNC(IsDestroyed, 1,
  LuaUtilPushVar(lS, LuaUtilIsClassDestroyed(lS, cAtlases)))
/* ========================================================================= **
** ######################################################################### **
** ## Atlas:* methods structure                                           ## **
** ######################################################################### **
** ========================================================================= */
LLRSMFBEGIN                            // Atlas:* methods begin
  LLRSFUNC(Destroy), LLRSFUNC(GetId), LLRSFUNC(GetName), LLRSFUNC(IsDestroyed),
LLRSEND                                // Atlas:* methods end
/* ========================================================================= **
** ######################################################################### **
** ## Atlas.* non-member functions                                        ## **
** ######################################################################### **
** ========================================================================= */
// $ Atlas.GetClasses
// < GetClasses:integer=Total number of atlases created.
// ? Returns the total number of atlas classes currently active.
/* ------------------------------------------------------------------------- */
LLFUNC(GetClasses, 1, LuaUtilPushVar(lS, cAtlases->CollectorCount()))
/* ========================================================================= **
** ######################################################################### **
** ## Atlas.* non-member functions structure                              ## **
** ######################################################################### **
** ========================================================================= */
LLRSBEGIN                              // Atlas.* non-member functions begin
  LLRSFUNC(GetClasses),
LLRSEND                                // Atlas.* non-member functions end
/* ========================================================================= */
}                                      // End of Atlas namespace
/* == EoF =========================================================== EoF == */
