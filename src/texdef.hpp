/* == TEXDEF.HPP =========================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## Sets up the texture related types and variables.                    ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace ITexDef {                    // Start of private module namespace
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* ------------------------------------------------------------------------- */
enum TextureType : size_t              // OpenGL texture type
{ /* ----------------------------------------------------------------------- */
  TT_NONE,                             // Uninitialised texture type
  TT_BGR,                              // 24-bit textures (bgra)
  TT_BGRA,                             // 32-bit textures (bgra)
  TT_DXT1,                             // aka. GL_COMPRESSED_RGB_S3TC_DXT1_EXT
  TT_DXT3,                             // aka. GL_COMPRESSED_RGB_S3TC_DXT3_EXT
  TT_DXT5,                             // aka. GL_COMPRESSED_RGB_S3TC_DXT5_EXT
  TT_GRAY,                             // 8-bit textures (gray)
  TT_GRAYALPHA,                        // 16-bit textures (gray+alpha)
  TT_RGB,                              // 24-bit textures (rgb)
  TT_RGBA,                             // 32-bit textures (rgba)
  /* ----------------------------------------------------------------------- */
  TT_MAX                               // Maximum number of texture types
};/* ----------------------------------------------------------------------- */
enum ByteDepth : unsigned int          // Human readable byte-depths
{ /* ----------------------------------------------------------------------- */
  BY_NONE,                             // [0] Not initialised yet
  BY_GRAY,                             // [1] Gray channel format
  BY_GRAYALPHA,                        // [2] Gray + alpha channel format
  BY_RGB,                              // [3] 1 pixel per 3 bytes (R+G+B)
  BY_RGBA,                             // [4] 1 pixel per 4 bytes (R+G+B+Alpha)
  /* ----------------------------------------------------------------------- */
  BY_MAX                               // [5] Maximum number of depth types
};/* ----------------------------------------------------------------------- */
static TextureType ImageBYtoTexType(const ByteDepth byDepth)
{ // Lookup table to convert bytedepth value to GL texture format
  typedef array<const TextureType, BY_MAX> ByteDepthToTexTypeLookup;
  static const ByteDepthToTexTypeLookup
    bdtttlLookup{ TT_NONE, TT_GRAY, TT_GRAYALPHA, TT_RGB, TT_RGBA };
  // Return clamped lookup value
  return bdtttlLookup[byDepth < bdtttlLookup.size() ? byDepth : BY_NONE];
}
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
