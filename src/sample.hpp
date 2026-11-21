/* == SAMPLE.HPP =========================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## This module defines a class that can load and play sound files      ## **
** ## using OpenAL's 3D positioning functions.                            ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace ISample {                    // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace ICollector::P;         using namespace ICVarDef::P;
using namespace IError::P;             using namespace ILog::P;
using namespace ILuaIdent::P;          using namespace ILuaLib::P;
using namespace ILuaUtil::P;           using namespace IMixer::P;
using namespace IOal::P;               using namespace IPcmDef::P;
using namespace IPcm::P;               using namespace IStd::P;
using namespace ISource::P;            using namespace IString::P;
using namespace ISysUtil::P;           using namespace LOal::Types;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* -- Sample collector and member class ------------------------------------ */
CTOR_BEGIN_NOBB(Samples, Sample, CLHelperUnsafe)
/* -- Sample member class -------------------------------------------------- */
CTOR_MEM_BEGIN(Samples, Sample, ICHelperUnsafe, /* n/a */),
  /* -- Base classes ------------------------------------------------------- */
  public Pcm                           // Loaded pcm data
{ /* -- Variables ---------------------------------------------------------- */
  ALUIntVector      aluvNames;         // OpenAL buffer handle ids
  ALenum            aleFormat,         // Format type for openal
                    aleSFormat;        // Single channel format for openal
  /* ----------------------------------------------------------------------- */
  ALuint SamplePrepareSource(Source &scSource, const ALuint aluBufId,
    const ALfloat alfGain, const ALfloat alfPan, const ALfloat alfPitch,
    const bool bLoop, const bool bLuaManaged)
  { // Set parameters
    scSource.SourceSetBuffer(static_cast<ALint>(aluBufId));
    scSource.SourceSetRelative(true);
    scSource.SourceSetPosition(alfPan, 0.0f, -sqrtf(1.0f - alfPan * alfPan));
    scSource.SourceSetLooping(bLoop);
    scSource.SourceSetPitch(alfPitch);
    scSource.SourceSetGain(alfGain);
    scSource.SourceSetExternal(bLuaManaged);
    // Return source id
    return scSource.SourceGet();
  }
  /* -- Load a single buffer from memory ----------------------------------- */
  void SampleLoad(Pcm &pcmSrc)
  { // Allocate and generate openal buffers
    aluvNames.resize(pcmSrc.PcmDataGetChannels());
    AL(cOal->OalCreateBuffers(aluvNames), "Error creating sample buffers!",
      "Name", pcmSrc.NameGet(), "Count", SampleNameCount());
    // Buffer the left or mono channel
    AL(cOal->OalBufferData(SampleLeftName(), SampleGetSFormat(),
      pcmSrc.PcmDataGetLeft(), static_cast<ALsizei>(pcmSrc.PcmDataGetRate())),
      "Error buffering left channel/mono PCM audio data!",
      "Name",   pcmSrc.NameGet(),        "Buffer",  SampleLeftName(),
      "Format", SampleGetFormat(),       "MFormat", SampleGetSFormat(),
      "Rate",   pcmSrc.PcmDataGetRate(),
      "Size",   pcmSrc.PcmDataGetLeftSize());
    // Log and return if mono sample
    if(pcmSrc.PcmDataGetChannels() == PCT_MONO)
      return cLog->LogDebugExSafe(
        "Sample '$' uploaded as $[$] at $Hz as $.",
        pcmSrc.NameGet(), SampleLeftName(), pcmSrc.PcmDataGetLeftSize(),
        StrToGrouped(pcmSrc.PcmDataGetRate(), 1),
        cOal->OalFormatToString(SampleGetFormat()));
    // Buffer the right stereo channel
    AL(cOal->OalBufferData(SampleRightName(), SampleGetSFormat(),
      pcmSrc.PcmDataGetRight(), static_cast<ALsizei>(pcmSrc.PcmDataGetRate())),
      "Error buffering right/stereo channel PCM audio data!",
      "Name",   pcmSrc.NameGet(),  "Buffer",  SampleRightName(),
      "Format", SampleGetFormat(), "MFormat", SampleGetSFormat(),
      "Rate",   pcmSrc.PcmDataGetRate(),
      "Size",   pcmSrc.PcmDataGetRightSize());
    // Log progress
    cLog->LogDebugExSafe(
      "Sample '$' uploaded as L:$[$] and R:$[$] at $Hz as $.",
      pcmSrc.NameGet(), SampleLeftName(), pcmSrc.PcmDataGetLeftSize(),
      SampleRightName(), pcmSrc.PcmDataGetRightSize(),
      StrToGrouped(pcmSrc.PcmDataGetRate(), 1),
      cOal->OalFormatToString(SampleGetFormat()));
  }
  /* -- Get AL buffer index from the specified physical buffer index ------- */
  template<typename IntType = ALint>
    requires StdIsIntegral<IntType>
  IntType SampleGetBufferInt(const ALenum aleId, const size_t stId = 0) const
  { // Hold state
    ALint iValue;
    // Store state
    AL(cOal->OalGetBufferInt(aluvNames[stId], aleId, &iValue),
      "Get buffer integer failed!",
      "Name", NameGet(), "Param", aleId, "Index", stId);
    // Return state
    return static_cast<IntType>(iValue);
  }
  /* ----------------------------------------------------------------------- */
  void SamplePlayStereoSource(ALfloat alfGain, ALfloat alfPan,
    const ALfloat alfPitch, const bool bLoop, Source &sCLref, Source &sCRref,
    const bool bLuaManaged)
  { // Adjust gain from global volumes
    alfGain *= cMixer->MixerGetAdjSampleVolume();
    // Half the pan
    alfPan *= 0.5f;
    // Prepare sources
    const StdArray<const ALuint,2> aSourceIds{
      // [0] Left channel
      SamplePrepareSource(sCLref, SampleLeftName(), alfGain, -0.5f + alfPan,
        alfPitch, bLoop, bLuaManaged),
      // [1] Right channel
      SamplePrepareSource(sCRref, SampleRightName(), alfGain, 0.5f + alfPan,
        alfPitch, bLoop, bLuaManaged)
    }; // Play all the sources together
    ALL(cOal->OalPlaySources(aSourceIds),
      "Sample '$' failed to play stereo sources!", NameGet());
  }
  /* -- Play a mono source ------------------------------------------------- */
  void SamplePlayMonoSource(const ALfloat alfGain, ALfloat alfPan,
    const ALfloat alfPitch, const bool bLoop, Source &sCMref,
    const bool bLuaManaged)
  { ALL(cOal->OalPlaySource(SamplePrepareSource(sCMref, SampleLeftName(),
      alfGain * cMixer->MixerGetAdjSampleVolume(), alfPan, alfPitch, bLoop,
      bLuaManaged)), "Sample '$' failed to play mono source!", NameGet()); }
  /* --------------------------------------------------------------- */ public:
  ALenum SampleGetFormat() const { return aleFormat; }
  /* ----------------------------------------------------------------------- */
  size_t SampleNameCount() const { return aluvNames.size(); }
  /* ----------------------------------------------------------------------- */
  template<typename RetType = ALuint>
    RetType SampleLeftName() const
  { return static_cast<RetType>(aluvNames.front()); }
  /* ----------------------------------------------------------------------- */
  template<typename RetType = ALuint>
    RetType SampleRightName() const
  { return static_cast<RetType>(aluvNames.back()); }
  /* ----------------------------------------------------------------------- */
  ALenum SampleGetSFormat() const { return aleSFormat; }
  /* -- Get buffer information --------------------------------------------- */
  ALsizei SampleGetALFrequency() const
    { return SampleGetBufferInt<ALsizei>(AL_FREQUENCY); }
  ALsizei SampleGetALBits() const
    { return SampleGetBufferInt<ALsizei>(AL_BITS); }
  ALsizei SampleGetALChannels() const
    { return SampleGetBufferInt<ALsizei>(AL_CHANNELS); }
  ALsizei SampleGetALSize() const
    { return SampleGetBufferInt<ALsizei>(AL_SIZE); }
  ALdouble SampleGetDuration() const
    { return static_cast<ALdouble>(SampleGetALSize()) * 8 /
        SampleGetALChannels() / SampleGetALBits() / SampleGetALFrequency(); }
  /* -- Spawn new sources in Lua ------------------------------------------- */
  size_t SampleSpawn(lua_State*const lS)
  { // How many sources do we need?
    switch(SampleNameCount())
    { // 1? (Mono source?) Create a new mono source and set buffer if succeeded
      case 1: if(const Source*const sCMptr = SourceGetFromLua(lS))
                sCMptr->SourceSetBuffer(SampleLeftName<ALint>());
              // Failed? Log the failure
              else cLog->LogWarningExSafe("Sample cannot get a free source "
                "for spawning '$'!", NameGet());
              break;
      // 2? (Stereo sources) Get the left channel and set buffer if succeeded
      case 2: if(const Source*const sCLptr = SourceGetFromLua(lS))
                sCLptr->SourceSetBuffer(SampleLeftName<ALint>());
              // Failed? Log the failure
              else cLog->LogWarningExSafe("Sample cannot get a free source "
                "for spawning '$' left channel!", NameGet());
              // Get a right channel source and set buffer if succeeded
              if(const Source*const sCRptr = SourceGetFromLua(lS))
                sCRptr->SourceSetBuffer(SampleRightName<ALint>());
              // Failed? Log the failure
              else cLog->LogWarningExSafe("Sample cannot get a free source "
                "for spawning '$' right channel!", NameGet());
              break;
      // Unsupported amount of channels
      default: XC("Unsupported amount of channels!",
        "Name", NameGet(), "Channels", SampleNameCount());
    } // Return number of channels
    return SampleNameCount();
  }
  /* -- Unload buffers ----------------------------------------------------- */
  void SampleUnloadBuffer()
  { // Bail if buffers not allocated
    if(aluvNames.empty()) return;
    // Stop this sample from playing in its entirety
    if(const unsigned uStopped = SampleStop())
      cLog->LogDebugExSafe("Sample '$' cleared $ sources using it!",
        NameGet(), uStopped);
    // Delete the buffers
    ALL(cOal->OalDeleteBuffers(aluvNames),
      "Sample '$' failed to delete $ buffers", NameGet(), SampleNameCount());
    // Reset buffer
    aluvNames.clear();
  }
  /* ----------------------------------------------------------------------- */
  void SamplePlay(const ALfloat alfGain, const ALfloat alfPan,
    const ALfloat alfPitch, const bool bLoop)
  { // How many sources do we need?
    switch(SampleNameCount())
    { // 1? (Mono source?) Create a new mono source and if we got it? Play it!
      case 1: if(Source*const sCMptr = SourceGet())
                SamplePlayMonoSource(alfGain, alfPan, alfPitch, bLoop, *sCMptr,
                  false);
              break;
      // 2? (Stereo sources) Get the left channel and if we got it?
      case 2: if(Source*const sCLptr = SourceGet())
              { // Get a right channel source and if we got it? Play it
                if(Source*const sCRptr = SourceGet())
                  SamplePlayStereoSource(alfGain, alfPan, alfPitch, bLoop,
                    *sCLptr, *sCRptr, false);
                // Could not grab a right channel source? Log failure
                else cLog->LogWarningExSafe("Sample cannot get a free source "
                  "for playing '$' left channel!", NameGet());
              } // Could not grab a left channel source? Log failure
              else cLog->LogWarningExSafe("Sample cannot get a free source "
                "for playing '$' right channel!", NameGet());
              break;
      // 0? (Internal error to log)
      case 0: cLog->LogWarningExSafe(
        "Internal error: Tried to play '$' which has no handles!", NameGet());
        break;
      // Unsupported amount of channels
      default: XC("Unsupported amount of channels!",
        "Name", NameGet(), "Channels", SampleNameCount());
    }
  }
  /* -- Play with a pre-allocated sources by Lua --------------------------- */
  size_t SamplePlay(lua_State*const lS, const ALfloat alfGain,
    const ALfloat alfPan, const ALfloat alfPitch, const bool bLoop)
  { // How many sources do we need?
    switch(SampleNameCount())
    { // 1 (Mono source?)?
      case 1:
        // Create a new mono source and if we got it? Play it
        if(Source*const sCMptr = SourceGetFromLua(lS))
          SamplePlayMonoSource(alfGain,
            alfPan, alfPitch, bLoop, *sCMptr, true);
        // Could not grab a mono channel source? Log failure
        else cLog->LogWarningExSafe(
          "Sample cannot get a free source for playing '$'!", NameGet());
        break;
      // 2 (Stereo sources)?
      case 2:
        // Get the left channel and if we got it?
        if(Source*const sCLptr = SourceGetFromLua(lS))
        { // Get a right channel source and if we got it? Play sources
          if(Source*const sCRptr = SourceGetFromLua(lS))
            SamplePlayStereoSource(alfGain, alfPan, alfPitch, bLoop, *sCLptr,
              *sCRptr, true);
          // Could not grab a right channel source? Log failure
          else cLog->LogWarningExSafe(
            "Sample cannot get a free source for playing '$' left channel!",
            NameGet());
        } // Could not grab a left channel source? Log failure
        else cLog->LogWarningExSafe(
          "Sample cannot get a free source for playing '$' right channel!",
          NameGet());
        break;
      // None?
      case 0:
        // Internal error
        cLog->LogWarningExSafe(
          "Internal error: Tried to play '$' which has no handles!",
          NameGet());
        break;
      // Unsupported amount of channels
      default: XC("Unsupported amount of channels!",
        "Name", NameGet(), "Channels", SampleNameCount());
    } // Return number of channels
    return SampleNameCount();
  }
  /* == Stop the buffer ==================================================== */
  unsigned SampleStop() const { return SourceStop(aluvNames); }
  /* -- Load a single buffer ----------------------------------------------- */
  void SampleReloadSample()
  { // If pcm sample was not loaded from disk? Just (re)load the bitmap data
    // that already should be there and should NEVER be released.
    if(PcmDataIsDynamic()) SampleLoad(*this);
    // Pcm sample is static?
    else
    { // It can just be reloaded from disk
      PcmReloadData();
      // Load the bitmap into memory
      SampleLoad(*this);
      // Clear memory because we can just reload from file
      PcmDataClearData();
    } // Show what was loaded
    cLog->LogInfoExSafe("Sample loaded '$'!", NameGet());
  }
  /* -- Init from a bitmap class ------------------------------------------- */
  void SampleInit(Pcm &pcmSrc)
  { // Show filename progress
    cLog->LogDebugExSafe("Sample loading from pcm '$'[$] ($;$;$)...",
      pcmSrc.NameGet(), pcmSrc.PcmDataGetAlloc(), pcmSrc.PcmDataGetRate(),
      pcmSrc.PcmDataGetChannels(), pcmSrc.PcmDataGetBits());
    // Check that the format is valid
    if(pcmSrc.PcmDataIsSigned() || pcmSrc.PcmDataIsBigEndian() ||
       !Oal::GetOALType(pcmSrc.PcmDataGetChannels(), pcmSrc.PcmDataGetBits(),
         aleFormat, aleSFormat))
      XC("OpenAL does not support the specified format!",
        "BigEndian", pcmSrc.PcmDataIsBigEndian(),
        "Signed",    pcmSrc.PcmDataIsSigned(),
        "Channels",  PcmDataGetChannels(), "Bits", PcmDataGetBits());
    // Take ownership of PCM object and identifier if src and dest not same
    if(this != &pcmSrc) { PcmSwap(pcmSrc); NameSwap(pcmSrc); }
    // Set purpose of the PCM object as a sample
    FlagSet(PP_SAMPLE);
    // Initialise
    SampleLoad(*this);
    // Remove all sample data because we can just load it from file again
    // and theres no point taking up precious memory for it.
    if(PcmDataIsNotDynamic()) PcmDataClearData();
  }
  /* -- Constructor -------------------------------------------------------- */
  Sample() :
    /* -- Initialisers ----------------------------------------------------- */
    ICHelperSample{ cSamples, this },  // Initialise collector class
    aleFormat(AL_NONE),                // Format not initialised
    aleSFormat(AL_NONE)                // Single channel format not initialised
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- Destructor --------------------------------------------------------- */
  DTORHELPER(~Sample, SampleUnloadBuffer())
};/* -- End ---------------------------------------------------------------- */
CTOR_END_NOINITS(Samples, Sample, SAMPLE) // Finish collector class
/* ========================================================================= */
static void SampleStop()
{ // Stop all samples from playing
  if(cSamples->empty()) return;
  for(const Sample*const scPtr : *cSamples) scPtr->SampleStop();
}
/* ========================================================================= */
static void SampleDeInit()
{ // Done if empty
  if(cSamples->empty()) return;
  // Re-create buffers for all the samples and log pre/post deinit
  cLog->LogDebugExSafe("Samples deinitialising $ objects...",
    cSamples->size());
  for(Sample*const scPtr : *cSamples) scPtr->SampleUnloadBuffer();
  cLog->LogInfoExSafe("Samples deinitialised $ objects.",
    cSamples->size());
}
/* ========================================================================= */
static void SampleReInit()
{ // Done if empty
  if(cSamples->empty()) return;
  // Re-create buffers for all the samples and log pre/post reinit
  cLog->LogDebugExSafe("Samples reinitialising $ objects...",
    cSamples->size());
  for(Sample*const scPtr : *cSamples) scPtr->SampleReloadSample();
  cLog->LogInfoExSafe("Samples reinitialised $ objects.", cSamples->size());
}
/* == Update all streams base volume======================================== */
static void SampleUpdateVolume()
{ // Lock source list so it cannot be modified
  cSources->MutexCall([](){
    // Walk through sources
    for(const Source*const scPtr : *cSources)
    { // Get class
      const Source &scRef = *scPtr;
      // If it is locked then its a sample so ignore it
      if(scRef.SourceGetExternal()) continue;
      // Set new sample volume
      scRef.SourceSetGain(cMixer->MixerGetAdjSampleVolume());
    }
  });
}
/* == Set all streams base volume ========================================== */
static CVarReturn SampleSetVolume(const ALfloat alfVolume)
{ // Ignore if invalid value
  if(alfVolume < 0.0f || alfVolume > 1.0f) return DENY;
  // Store volume (SOURCES class keeps it)
  cMixer->MixerSetSampleVolume(alfVolume);
  // Update volumes on all streams
  SampleUpdateVolume();
  // Success
  return ACCEPT;
}
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
