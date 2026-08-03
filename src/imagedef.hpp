/* == IMAGEDEF.HPP ========================================================= **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## Sets up the image data storage memory and metadata.                 ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IImageDef {                  // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IDim::P;               using namespace IFlags::P;
using namespace IMemory::P;            using namespace IStd::P;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* -- Public typedefs ------------------------------------------------------ */
enum ImageFormat : size_t              // Available image codecs
{ /* ----------------------------------------------------------------------- */
  IFMT_PNG,                            // [0] PNG  (IImageFormat::CodecPNG)
  IFMT_JPG,                            // [1] JPG  (IImageFormat::CodecJPG)
  IFMT_GIF,                            // [2] GIF  (IImageFormat::CodecGIF)
  IFMT_DDS,                            // [3] DDS  (IImageFormat::CodecDDS)
  IFMT_WEBP,                           // [4] WEBP (IImageFormat::CodecWEBP)
  /* ----------------------------------------------------------------------- */
  IFMT_MAX                             // [5] Maximum supported image codecs
};/* ----------------------------------------------------------------------- */
BUILD_FLAGS(Image,                     // Shared image flags
  /* -- Note --------------------------------------------------------------- **
  ** The 'ImageData' class contains a 'Flags' class which is shared between  **
  ** five different classes, 'Atlas', 'Image', 'ImageData', 'Font' and       **
  ** 'Texture' so it's important we don't duplicate values here.             **
  ** -- Font loading flags (Only used in 'Font' class) --------------------- */
  IL_NONE                   {Flag(0)}, // No flags?
  FF_STROKETYPE2            {Flag(1)}, // True stroke but more buggy?
  FF_USEGLYPHSIZE           {Flag(2)}, // Use image glyph size for advance?
  FF_ROUNDADVANCE           {Flag(3)}, // Do round() on advance width?
  FF_FLOORADVANCE           {Flag(4)}, // Do floor() on advance width?
  FF_CEILADVANCE            {Flag(5)}, // Do ceil() on advance width?
  /* -- Font loader public mask bits --------------------------------------- */
  FF_MASK{ FF_USEGLYPHSIZE|FF_STROKETYPE2|FF_FLOORADVANCE|FF_CEILADVANCE|
           FF_ROUNDADVANCE },
  /* -- Font types --------------------------------------------------------- */
  FT_FREETYPE               {Flag(6)}, // Font is a freetype font?
  FT_BITMAP                 {Flag(7)}, // Font is a static bitmap font?
  /* -- Post processing (Only used in 'Image' class) ----------------------- */
  IL_ATLAS                  {Flag(8)}, // Convert to atlas?
  IL_TOGPU                  {Flag(9)}, // Image will be loadable in OpenGL?
  IL_TO24BPP               {Flag(10)}, // Convert loaded image to 24bpp?
  IL_TO32BPP               {Flag(11)}, // Convert loaded image to 32bpp?
  IL_TOBGR                 {Flag(12)}, // Convert loaded image to BGR(A)?
  IL_TORGB                 {Flag(13)}, // Convert loaded image to RGB(A)?
  IL_TOBINARY              {Flag(14)}, // Convert loaded image to BINARY?
  IL_REVERSE               {Flag(15)}, // Force reverse the image?
  /* -- Force load formats (Only used in 'Image' class) -------------------- */
  IL_FCE_PNG               {Flag(24)}, // Force load as PNG?
  IL_FCE_JPG               {Flag(25)}, // Force load as JPEG?
  IL_FCE_GIF               {Flag(26)}, // Force load as GIF?
  IL_FCE_DDS               {Flag(27)}, // Force load as DDS?
  /* -- Image loader public mask bits -------------------------------------- */
  IL_MASK{ IL_TOGPU|IL_TO24BPP|IL_TO32BPP|IL_TOBGR|IL_TORGB|IL_TOBINARY|
    IL_REVERSE|IL_ATLAS|IL_FCE_JPG|IL_FCE_PNG|IL_FCE_GIF|IL_FCE_DDS },
  /* -- Active flags (Only used in 'Image' class) ----------------------- */
  IA_TOGPU                 {Flag(32)}, // Image converted to load in GL?
  IA_TO24BPP               {Flag(33)}, // Converted image to 24bpp?
  IA_TO32BPP               {Flag(34)}, // Converted image to 32bpp?
  IA_TOBGR                 {Flag(35)}, // Converted image to BGR(A)?
  IA_TORGB                 {Flag(36)}, // Converted image to RGB(A)?
  IA_TOBINARY              {Flag(37)}, // Converted image to BINARY?
  IA_REVERSE               {Flag(38)}, // Force reversed the image?
  IA_ATLAS                 {Flag(39)}, // Converted to atlas?
  /* -- Image loaded flags (Only used in 'ImageData' class) ---------------- */
  IF_MIPMAPS               {Flag(48)}, // Bitmap has mipmaps?
  IF_REVERSED              {Flag(49)}, // Bitmap FILE has reversed pixels?
  IF_COMPRESSED            {Flag(50)}, // Bitmap is compressed?
  IF_DYNAMIC               {Flag(51)}, // Bitmap is dynamically created?
  IF_PALETTE               {Flag(52)}, // A palette is included?
  /* -- Texture loaded flags (Only used in 'Image' class) ------------------ */
  TF_DELETE                {Flag(55)}, // Marked for deletion?
  /* -- Image purpose (help with debugging) -------------------------------- */
  IP_IMAGE                 {Flag(61)}, // Image is stand-alone?
  IP_TEXTURE               {Flag(62)}, // Image is part of a Texture class?
  IP_FONT                  {Flag(63)}, // Image is part of a Font class?
  IP_ATLAS                 {Flag(64)}  // Image is part of an Atlas class?
);/* ----------------------------------------------------------------------- */
enum BitDepth : unsigned               // Human readable bit-depths
{ /* ----------------------------------------------------------------------- */
  BD_NONE                        =  0, // Not initialised yet
  BD_BINARY                      =  1, // Binary format (8 pixels per byte)
  BD_GRAY                        =  8, // Gray channel format
  BD_GRAYALPHA                   = 16, // Gray + alpha channel format
  BD_RGB                         = 24, // 1 pixel per 3 bytes (R+G+B)
  BD_RGBA                        = 32  // 1 pixel per 4 bytes (R+G+B+Alpha)
};/* ----------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
