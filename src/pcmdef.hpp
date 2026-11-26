/* == PCMDEF.HPP =========================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## Definitions for PCM metadata.                                       ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IPcmDef {                    // Start of private module namespace
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IFlags::P;
/* -- Public typedefs ------------------------------------------------------ */
enum PcmFormat : size_t                // Available PCM codecs
{ /* ----------------------------------------------------------------------- */
  PFMT_WAV,                            // [0] WAV (IPcmFormat::CodecWAV)
  PFMT_CAF,                            // [1] CAF (IPcmFormat::CodecCAF)
  PFMT_OGG,                            // [2] OGG (IPcmFormat::CodecOGG)
  /* ----------------------------------------------------------------------- */
  PFMT_MAX                             // [3] Maximum supported PCM codecs
};/* ----------------------------------------------------------------------- */
enum PcmBitType : unsigned             // PCM bit-depth type
{ /* ----------------------------------------------------------------------- */
  PBI_NONE                       =  0, // PCM audio is uninitialised
  PBI_BYTE                       =  8, // PCM audio is 8-bits per channel
  PBI_SHORT                      = 16, // PCM audio is 16-bits per channel
  PBI_LONG                       = 32  // PCM audio is 32-bits per channel
};/* ----------------------------------------------------------------------- */
enum PcmByteType : unsigned            // PCM byte-depth type
{ /* ----------------------------------------------------------------------- */
  PBY_NONE                       =  0, // PCM audio is uninitialised
  PBY_BYTE                       =  1, // PCM audio is 1 byte per channel
  PBY_SHORT                      =  2, // PCM audio is 2 bytes per channel
  PBY_LONG                       =  4  // PCM audio is 4 bytes per channel
};/* ----------------------------------------------------------------------- */
enum PcmChannelType : unsigned         // PCM channels type
{ /* ----------------------------------------------------------------------- */
  PCT_NONE                       =  0, // PCM audio is uninitialised
  PCT_MONO                       =  1, // PCM audio is mono (1ch)
  PCT_STEREO                     =  2, // PCM audio is stereo (2ch)
};/* ----------------------------------------------------------------------- */
BUILD_FLAGS(unsigned, Pcm,             // PCM loading flags
  /* ----------------------------------------------------------------------- */
  PL_NONE                {PcmFlag[0]}, // No loading flags?
  /* -- Force types -------------------------------------------------------- */
  PL_FCE_WAV             {PcmFlag[1]}, // Force load as WAV format?
  PL_FCE_CAF             {PcmFlag[2]}, // Force load as CAF format?
  PL_FCE_OGG             {PcmFlag[3]}, // Force load as OGG format?
  /* -- Post processing requests ------------------------------------------- */
  PL_TOSPU               {PcmFlag[4]}, // Convert to nearest OpenAL compatible
  PL_TOBE                {PcmFlag[5]}, // Convert to big-endian format
  PL_TOLE                {PcmFlag[6]}, // Convert to little-endian format
  PL_TOSIGNED            {PcmFlag[7]}, // Convert to signed format
  PL_TOUNSIGNED          {PcmFlag[8]}, // Convert to unsigned format
  /* -- Mask bits ---------------------------------------------------------- */
  PL_MASK{ PL_FCE_WAV|PL_FCE_CAF|PL_FCE_OGG|PL_TOSPU|PL_TOBE|PL_TOLE|
           PL_TOSIGNED|PL_TOUNSIGNED },
  /* -- Private flags (Only used in 'Pcm' class) --------------------------- */
  PL_BE                  {PcmFlag[9]}, // Is big-endian encoded (or LE)?
  PL_DYNAMIC            {PcmFlag[10]}, // Waveform is dynamically created?
  PL_SIGNED             {PcmFlag[11]}, // Is signed/unsigned waveform?
  /* -- Active post-processing flags --------------------------------------- */
  PA_TOSPU              {PcmFlag[12]}, // Converted to nearest OpenAL compat.
  PA_TOBE               {PcmFlag[13]}, // Converted to big-endian format
  PA_TOLE               {PcmFlag[14]}, // Converted to little-endian format
  PA_TOSIGNED           {PcmFlag[15]}, // Converted to signed format
  PA_TOUNSIGNED         {PcmFlag[16]}, // Converted to unsigned format
  /* -- These flags are removed when the PCM is reloaded ------------------- */
  PL_ACTIVE{ PA_TOSPU|PA_TOBE|PA_TOLE|PA_TOSIGNED|PA_TOUNSIGNED },
  /* -- PCM purpose (help with debugging) ---------------------------------- */
  PP_SAMPLE             {PcmFlag[32]}  // Purpose is to be a Sample
);/* ----------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
