/* == SHADERDEF.HPP ======================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## This defines shader uniforms and attribute ids                      ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IShaderDef {                 // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace Lib::OS::GlFW::Types;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* -- Public typedefs ------------------------------------------------------ */
enum ShaderUniformId : size_t          // Mandatory uniforms
{ /* ----------------------------------------------------------------------- */
  U_MATRIX,                            // Matrix uniform vec4
  U_PALETTE,                           // Palette uniform vec4
  /* ----------------------------------------------------------------------- */
  U_MAX                                // Max no of mandatory uniforms
};/* ----------------------------------------------------------------------- */
enum ShaderAttributeId : GLuint        // Mandatory attributes
{ /* ----------------------------------------------------------------------- */
  A_COORD,                             // TexCoord attribute vec2 array
  A_VERTEX,                            // Vertex attribute vec2 array
  A_COLOUR,                            // Colour attribute vec4 array
  /* ----------------------------------------------------------------------- */
  A_MAX                                // Max no of mandatory attributes
};/* ----------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
