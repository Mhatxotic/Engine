/* == MIXER.HPP ============================================================ **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## This module defines a class that manages volume values.             ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IMixer {                     // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using Lib::OpenAL::ALfloat;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* ------------------------------------------------------------------------- */
class Mixer;                           // Class prototype
static Mixer *cMixer = nullptr;        // Pointer to global class
class Mixer                            // The class
{ /* ----------------------------------------------------------------------- */
  using AtomicALFloat = StdAtomic<ALfloat>;
  /* ----------------------------------------------------------------------- */
  AtomicALFloat    aalfAdjSample,      // Adjusted 'Sample' class volume
                   aalfAdjStream,      // Adjusted 'Stream' class volume
                   aalfAdjVideo,       // Adjusted 'Video' class volume
                   aalfGlobal,         // Global volume
                   aalfSample,         // 'Sample' class default volume
                   aalfStream,         // 'Stream' class default volume
                   aalfVideo;          // 'Video' class default volume
  /* -- Get volume values ------------------------------------------ */ public:
  ALfloat MixerGetGlobalVolume() const { return aalfGlobal; }
  ALfloat MixerGetStreamVolume() const { return aalfStream; }
  ALfloat MixerGetVideoVolume() const { return aalfVideo; }
  ALfloat MixerGetSampleVolume() const { return aalfSample; }
  /* -- Get volume values adjusted by global volume ------------------------ */
  ALfloat MixerGetAdjStreamVolume() const { return aalfAdjStream; }
  ALfloat MixerGetAdjVideoVolume() const { return aalfAdjVideo; }
  ALfloat MixerGetAdjSampleVolume() const { return aalfAdjSample; }
  /* -- Set global volume level -------------------------------------------- */
  void MixerSetGlobalVolume(const ALfloat alfValue)
  { // Set global and adjusted volume
    aalfGlobal = alfValue;
    aalfAdjStream = MixerGetStreamVolume() * alfValue;
    aalfAdjVideo = MixerGetVideoVolume() * alfValue;
    aalfAdjSample = MixerGetSampleVolume() * alfValue;
  }
  /* -- Set 'Stream' class volume level ------------------------------------ */
  void MixerSetStreamVolume(const ALfloat alfValue)
    { aalfStream = alfValue;
      aalfAdjStream = alfValue * MixerGetGlobalVolume(); }
  /* -- Set 'Video' class volume level ------------------------------------- */
  void MixerSetVideoVolume(const ALfloat alfValue)
    { aalfVideo = alfValue;
      aalfAdjVideo = alfValue * MixerGetGlobalVolume(); }
  /* -- Set 'Sample' class volume level ------------------------------------ */
  void MixerSetSampleVolume(const ALfloat alfValue)
    { aalfSample = alfValue;
      aalfAdjSample = alfValue * MixerGetGlobalVolume(); }
  /* ----------------------------------------------------------------------- */
  Mixer(void) :
    /* -- Initialisers ----------------------------------------------------- */
    aalfAdjSample(0.0f),               // Set by 'MixerSetSampleVolume' func
    aalfAdjStream(0.0f),               // Set by 'MixerSetStreamVolume' func
    aalfAdjVideo(0.0f),                // Set by 'MixerSetVideoVolume' func
    aalfGlobal(0.0f),                  // Set by 'Audio' class
    aalfSample(0.0f),                  // Set by 'Sample' class
    aalfStream(0.0f),                  // Set by 'Stream' class
    aalfVideo(0.0f)                    // Set by 'Video' class
    /* --------------------------------------------------------------------- */
    { cMixer = this; }
};/* ----------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
