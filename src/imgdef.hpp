/* == IMGDEF.HPP =========================================================== **
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
  IFMT_JPEG,                           // [1] JPEG (IImageFormat::CodecJPEG)
  IFMT_GIF,                            // [2] GIF  (IImageFormat::CodecGIF)
  IFMT_DDS,                            // [3] DDS  (IImageFormat::CodecDDS)
  IFMT_WEBP,                           // [4] WEBP (IImageFormat::CodecWEBP)
  /* ----------------------------------------------------------------------- */
  IFMT_MAX                             // [5] Maximum supported image codecs
};/* ----------------------------------------------------------------------- */
BUILD_FLAGS(uint64_t, Image,           // Shared image flags
  /* -- Note --------------------------------------------------------------- **
  ** The 'ImageData' class contains a 'Flags' class which is shared between  **
  ** five different classes, 'Atlas', 'Image', 'ImageData', 'Font' and       **
  ** 'Texture' so it's important we don't duplicate values here.             **
  ** -- Font loading flags (Only used in 'Font' class) --------------------- */
  IL_NONE              {ImageFlag[0]}, // No flags?
  FF_STROKETYPE2       {ImageFlag[1]}, // True stroke but more buggy?
  FF_USEGLYPHSIZE      {ImageFlag[2]}, // Use image glyph size for advance?
  FF_ROUNDADVANCE      {ImageFlag[3]}, // Do round() on advance width?
  FF_FLOORADVANCE      {ImageFlag[4]}, // Do floor() on advance width?
  FF_CEILADVANCE       {ImageFlag[5]}, // Do ceil() on advance width?
  /* -- Font loader public mask bits --------------------------------------- */
  FF_MASK{ FF_USEGLYPHSIZE|FF_STROKETYPE2|FF_FLOORADVANCE|FF_CEILADVANCE|
           FF_ROUNDADVANCE },
  /* -- Font types --------------------------------------------------------- */
  FT_FREETYPE          {ImageFlag[6]}, // Font is a freetype font?
  FT_BITMAP            {ImageFlag[7]}, // Font is a static bitmap font?
  /* -- Post processing (Only used in 'Image' class) ----------------------- */
  IL_ATLAS             {ImageFlag[8]}, // Convert to atlas?
  IL_TOGPU             {ImageFlag[9]}, // Image will be loadable in OpenGL?
  IL_TO24BPP          {ImageFlag[10]}, // Convert loaded image to 24bpp?
  IL_TO32BPP          {ImageFlag[11]}, // Convert loaded image to 32bpp?
  IL_TOBGR            {ImageFlag[12]}, // Convert loaded image to BGR(A)?
  IL_TORGB            {ImageFlag[13]}, // Convert loaded image to RGB(A)?
  IL_TOBINARY         {ImageFlag[14]}, // Convert loaded image to BINARY?
  IL_REVERSE          {ImageFlag[15]}, // Force reverse the image?
  /* -- Force load formats (Only used in 'Image' class) -------------------- */
  IL_FCE_PNG          {ImageFlag[24]}, // Force load as PNG?
  IL_FCE_JPEG         {ImageFlag[25]}, // Force load as JPEG?
  IL_FCE_GIF          {ImageFlag[26]}, // Force load as GIF?
  IL_FCE_DDS          {ImageFlag[27]}, // Force load as DDS?
  IL_FCE_WEBP         {ImageFlag[28]}, // Force load as WEBP?
  /* -- Image loader public mask bits -------------------------------------- */
  IL_MASK{ IL_TOGPU|IL_TO24BPP|IL_TO32BPP|IL_TOBGR|IL_TORGB|IL_TOBINARY|
    IL_REVERSE|IL_ATLAS|IL_FCE_JPEG|IL_FCE_PNG|IL_FCE_GIF|IL_FCE_DDS|
    IL_FCE_WEBP },
  /* -- Active flags (Only used in 'Image' class) ----------------------- */
  IA_TOGPU            {ImageFlag[32]}, // Image converted to load in GL?
  IA_TO24BPP          {ImageFlag[33]}, // Converted image to 24bpp?
  IA_TO32BPP          {ImageFlag[34]}, // Converted image to 32bpp?
  IA_TOBGR            {ImageFlag[35]}, // Converted image to BGR(A)?
  IA_TORGB            {ImageFlag[36]}, // Converted image to RGB(A)?
  IA_TOBINARY         {ImageFlag[37]}, // Converted image to BINARY?
  IA_REVERSE          {ImageFlag[38]}, // Force reversed the image?
  IA_ATLAS            {ImageFlag[39]}, // Converted to atlas?
  /* -- Image loaded flags (Only used in 'ImageData' class) ---------------- */
  IF_MIPMAPS          {ImageFlag[48]}, // Bitmap has mipmaps?
  IF_REVERSED         {ImageFlag[49]}, // Bitmap FILE has reversed pixels?
  IF_COMPRESSED       {ImageFlag[50]}, // Bitmap is compressed?
  IF_DYNAMIC          {ImageFlag[51]}, // Bitmap is dynamically created?
  IF_PALETTE          {ImageFlag[52]}, // A palette is included?
  /* -- Texture loaded flags (Only used in 'Image' class) ------------------ */
  TF_DELETE           {ImageFlag[55]}, // Marked for deletion?
  /* -- Image active flags to remove on reload ----------------------------- */
  IL_ACTIVE{ IA_TOGPU|IA_TO24BPP|IA_TO32BPP|IA_TOBGR|IA_TORGB|IA_TOBINARY|
    IA_REVERSE|IA_ATLAS|IF_MIPMAPS|IF_REVERSED|IF_COMPRESSED|IF_DYNAMIC|
    IF_PALETTE|TF_DELETE },
  /* -- Image purpose (help with debugging) -------------------------------- */
  IP_IMAGE            {ImageFlag[61]}, // Image is stand-alone?
  IP_TEXTURE          {ImageFlag[62]}, // Image is part of a Texture class?
  IP_FONT             {ImageFlag[63]}, // Image is part of a Font class?
  IP_ATLAS            {ImageFlag[64]}  // Image is part of an Atlas class?
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
