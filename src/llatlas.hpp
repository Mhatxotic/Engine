/* == LLATLAS.HPP ========================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## Defines the 'Atlas' namespace and methods for the guest to use in   ## **
** ## Lua. This file is invoked by 'lualib.hpp'.                          ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
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
** ## Atlas:* member functions structure                                  ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
LLRSMFBEGIN                            // Atlas:* member functions begin
LLRSEND                                // Atlas:* member functions end
/* ========================================================================= **
** ######################################################################### **
** ## Atlas.* namespace functions structure                               ## **
** ######################################################################### **
** ========================================================================= */
LLRSBEGIN                              // Atlas.* namespace consts begin
LLRSEND                                // Atlas.* namespace consts end
/* ========================================================================= */
}                                      // End of Atlas namespace
/* == EoF =========================================================== EoF == */
