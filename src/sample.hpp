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
using namespace ISysUtil::P;           using namespace Lib::OpenAL::Types;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* -- Sample collector and member class ------------------------------------ */
CTOR_BEGIN_NOBB(Samples, Sample, CLHelperUnsafe)
/* -- Sample member class -------------------------------------------------- */
CTOR_MEM_BEGIN(Samples, Sample, ICHelperUnsafe, /* n/a */),
  /* -- Base classes ------------------------------------------------------- */
  public Pcm                           // Loaded pcm data
{ /* -- Variables -------------------------------------------------- */ public:
  ALUIntVector      aluvNames;         // OpenAL buffer handle ids
  ALenum            aleFormat,         // Format type for openal
                    aleSFormat;        // Single channel format for openal
  /* ----------------------------------------------------------------------- */
  ALenum GetFormat() const { return aleFormat; }
  /* ----------------------------------------------------------------------- */
  ALenum GetSFormat() const { return aleSFormat; }
  /* -- Get AL buffer index from the specified physical buffer index ------- */
  template<typename IntType = ALint>
    requires StdIsIntegral<IntType>
  IntType GetBufferInt(const ALenum aleId, const size_t stId=0) const
  { // Hold state
    ALint iValue;
    // Store state
    AL(cOal->GetBufferInt(aluvNames[stId], aleId, &iValue),
      "Get buffer integer failed!",
      "Name", NameGet(), "Param", aleId, "Index", stId);
    // Return state
    return static_cast<IntType>(iValue);
  }
  /* -- Get buffer information --------------------------------------------- */
  ALsizei GetALFrequency() const
    { return GetBufferInt<ALsizei>(AL_FREQUENCY); }
  ALsizei GetALBits() const
    { return GetBufferInt<ALsizei>(AL_BITS); }
  ALsizei GetALChannels() const
    { return GetBufferInt<ALsizei>(AL_CHANNELS); }
  ALsizei GetALSize() const
    { return GetBufferInt<ALsizei>(AL_SIZE); }
  ALdouble GetDuration() const
    { return static_cast<ALdouble>(GetALSize()) * 8 /
        GetALChannels() / GetALBits() / GetALFrequency(); }
  /* -- Unload buffers ----------------------------------------------------- */
  void UnloadBuffer()
  { // Bail if buffers not allocated
    if(aluvNames.empty()) return;
    // Stop this sample from playing in its entirety
    if(const unsigned uStopped = Stop())
      cLog->LogDebugExSafe("Sample '$' cleared $ sources using it!",
        NameGet(), uStopped);
    // Delete the buffers
    ALL(cOal->DeleteBuffers(aluvNames),
      "Sample '$' failed to delete $ buffers", NameGet(), aluvNames.size());
    // Reset buffer
    aluvNames.clear();
  }
  /* ----------------------------------------------------------------------- */
  ALuint PrepareSource(Source &scSource, const ALuint aluBufId,
    const ALfloat alfGain, const ALfloat alfPan, const ALfloat alfPitch,
    const bool bLoop, const bool bLuaManaged)
  { // Set parameters
    scSource.SetBuffer(static_cast<ALint>(aluBufId));
    scSource.SetRelative(true);
    scSource.SetPosition(alfPan, 0.0f, -sqrtf(1.0f - alfPan * alfPan));
    scSource.SetLooping(bLoop);
    scSource.SetPitch(alfPitch);
    scSource.SetGain(alfGain);
    scSource.SetExternal(bLuaManaged);
    // Return source id
    return scSource.GetSource();
  }
  /* ----------------------------------------------------------------------- */
  void PlayStereoSource(ALfloat alfGain, ALfloat alfPan,
    const ALfloat alfPitch, const bool bLoop, Source &sCLref, Source &sCRref,
    const bool bLuaManaged)
  { // Adjust gain from global volumes
    alfGain *= cMixer->MixerGetAdjSampleVolume();
    // Half the pan
    alfPan *= 0.5f;
    // Prepare sources
    const StdArray<const ALuint,2> aSourceIds{
      // [0] Left channel
      PrepareSource(sCLref, aluvNames.front(), alfGain, -0.5f + alfPan,
        alfPitch, bLoop, bLuaManaged),
      // [1] Right channel
      PrepareSource(sCRref, aluvNames.back(), alfGain, 0.5f + alfPan,
        alfPitch, bLoop, bLuaManaged)
    }; // Play all the sources together
    ALL(cOal->PlaySources(aSourceIds),
      "Sample '$' failed to play stereo sources!", NameGet());
  }
  /* -- Play a mono source ------------------------------------------------- */
  void PlayMonoSource(const ALfloat alfGain, ALfloat alfPan,
    const ALfloat alfPitch, const bool bLoop, Source &sCMref,
    const bool bLuaManaged)
  { ALL(cOal->PlaySource(PrepareSource(sCMref, aluvNames.front(),
      alfGain * cMixer->MixerGetAdjSampleVolume(), alfPan, alfPitch, bLoop,
      bLuaManaged)), "Sample '$' failed to play mono source!", NameGet()); }
  /* -- Spawn new sources in Lua ------------------------------------------- */
  size_t Spawn(lua_State*const lS)
  { // How many sources do we need?
    switch(aluvNames.size())
    { // 1? (Mono source?) Create a new mono source and set buffer if succeeded
      case 1: if(const Source*const sCMptr = SourceGetFromLua(lS))
                sCMptr->SetBuffer(static_cast<ALint>(aluvNames.front()));
              // Failed? Log the failure
              else cLog->LogWarningExSafe("Sample cannot get a free source "
                "for spawning '$'!", NameGet());
              break;
      // 2? (Stereo sources) Get the left channel and set buffer if succeeded
      case 2: if(const Source*const sCLptr = SourceGetFromLua(lS))
                sCLptr->SetBuffer(static_cast<ALint>(aluvNames.front()));
              // Failed? Log the failure
              else cLog->LogWarningExSafe("Sample cannot get a free source "
                  "for spawning '$' left channel!", NameGet());
              // Get a right channel source and set buffer if succeeded
              if(const Source*const sCRptr = SourceGetFromLua(lS))
                sCRptr->SetBuffer(static_cast<ALint>(aluvNames.back()));
              // Failed? Log the failure
              else cLog->LogWarningExSafe("Sample cannot get a free source "
                "for spawning '$' right channel!", NameGet());
              break;
      // Unsupported amount of channels
      default: XC("Unsupported amount of channels!",
        "Name", NameGet(), "Channels", aluvNames.size());
    } // Return number of channels
    return aluvNames.size();
  }
  /* ----------------------------------------------------------------------- */
  void Play(const ALfloat alfGain, const ALfloat alfPan,
    const ALfloat alfPitch, const bool bLoop)
  { // How many sources do we need?
    switch(aluvNames.size())
    { // 1? (Mono source?) Create a new mono source and if we got it? Play it!
      case 1: if(Source*const sCMptr = GetSource())
                PlayMonoSource(alfGain, alfPan, alfPitch, bLoop, *sCMptr,
                  false);
              break;
      // 2? (Stereo sources) Get the left channel and if we got it?
      case 2: if(Source*const sCLptr = GetSource())
              { // Get a right channel source and if we got it? Play it
                if(Source*const sCRptr = GetSource())
                  PlayStereoSource(alfGain, alfPan, alfPitch, bLoop,
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
        "Name", NameGet(), "Channels",   aluvNames.size());
    }
  }
  /* -- Play with a pre-allocated sources by Lua --------------------------- */
  size_t Play(lua_State*const lS, const ALfloat alfGain, const ALfloat alfPan,
    const ALfloat alfPitch, const bool bLoop)
  { // How many sources do we need?
    switch(aluvNames.size())
    { // 1 (Mono source?)?
      case 1:
        // Create a new mono source and if we got it? Play it
        if(Source*const sCMptr = SourceGetFromLua(lS))
          PlayMonoSource(alfGain, alfPan, alfPitch, bLoop, *sCMptr, true);
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
            PlayStereoSource(alfGain, alfPan, alfPitch, bLoop, *sCLptr,
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
        "Name", NameGet(), "Channels",   aluvNames.size());
    } // Return number of channels
    return aluvNames.size();
  }
  /* == Stop the buffer ==================================================== */
  unsigned Stop() const { return SourceStop(aluvNames); }
  /* -- Load a single buffer from memory ----------------------------------- */
  void LoadSample(Pcm &pcmSrc)
  { // Allocate and generate openal buffers
    aluvNames.resize(pcmSrc.GetChannels());
    AL(cOal->CreateBuffers(aluvNames), "Error creating sample buffers!",
      "Name", pcmSrc.NameGet(), "Count", aluvNames.size());
    // Buffer the left or mono channel
    AL(cOal->BufferData(aluvNames.front(), GetSFormat(),
      pcmSrc.aPcmL, static_cast<ALsizei>(pcmSrc.GetRate())),
        "Error buffering left channel/mono PCM audio data!",
        "Name", pcmSrc.NameGet(), "Buffer",  aluvNames.front(),
        "Format",     GetFormat(),       "MFormat", GetSFormat(),
        "Rate",       pcmSrc.GetRate(),  "Size",    pcmSrc.aPcmL.MemSize());
    // Log and return if mono sample
    if(pcmSrc.GetChannels() == PCT_MONO)
      return cLog->LogDebugExSafe(
        "Sample '$' uploaded as $[$] at $Hz as $.",
        pcmSrc.NameGet(), aluvNames.front(), pcmSrc.aPcmL.MemSize(),
        StrToGrouped(pcmSrc.GetRate(), 1), cOal->GetALFormat(GetFormat()));
    // Buffer the right stereo channel
    AL(cOal->BufferData(aluvNames.back(), GetSFormat(),
      pcmSrc.aPcmR, static_cast<ALsizei>(pcmSrc.GetRate())),
        "Error buffering right/stereo channel PCM audio data!",
        "Name", pcmSrc.NameGet(), "Buffer",  aluvNames.back(),
        "Format",     GetFormat(),       "MFormat", GetSFormat(),
        "Rate",       pcmSrc.GetRate(),  "Size",    pcmSrc.aPcmR.MemSize());
    // Log progress
    cLog->LogDebugExSafe(
      "Sample '$' uploaded as L:$[$] and R:$[$] at $Hz as $.",
      pcmSrc.NameGet(), aluvNames.front(), pcmSrc.aPcmL.MemSize(),
      aluvNames.back(), pcmSrc.aPcmR.MemSize(),
      StrToGrouped(pcmSrc.GetRate(), 1), cOal->GetALFormat(GetFormat()));
  }
  /* -- Load a single buffer ----------------------------------------------- */
  void ReloadSample()
  { // If pcm sample was not loaded from disk? Just (re)load the bitmap data
    // that already should be there and should NEVER be released.
    if(IsDynamic()) LoadSample(*this);
    // Pcm sample is static?
    else
    { // It can just be reloaded from disk
      ReloadData();
      // Load the bitmap into memory
      LoadSample(*this);
      // Clear memory because we can just reload from file
      ClearData();
    } // Show what was loaded
    cLog->LogInfoExSafe("Sample loaded '$'!", NameGet());
  }
  /* -- Init from a bitmap class ------------------------------------------- */
  void InitSample(Pcm &pcmSrc)
  { // Show filename progress
    cLog->LogDebugExSafe("Sample loading from pcm '$'[$] ($;$;$)...",
      pcmSrc.NameGet(), pcmSrc.GetAlloc(), pcmSrc.GetRate(),
      pcmSrc.GetChannels(), pcmSrc.GetBits());
    // Check that the format is valid
    if(pcmSrc.IsSigned() || pcmSrc.IsBigEndian() ||
       !Oal::GetOALType(pcmSrc.GetChannels(), pcmSrc.GetBits(),
         aleFormat, aleSFormat))
      XC("OpenAL does not support the specified format!",
        "BigEndian", pcmSrc.IsBigEndian(), "Signed", pcmSrc.IsSigned(),
        "Channels",  GetChannels(),        "Bits",   GetBits());
    // Take ownership of PCM object and identifier if src and dest not same
    if(this != &pcmSrc) { SwapPcm(pcmSrc); NameSwap(pcmSrc); }
    // Set purpose of the PCM object as a sample
    FlagSet(PP_SAMPLE);
    // Initialise
    LoadSample(*this);
    // Remove all sample data because we can just load it from file again
    // and theres no point taking up precious memory for it.
    if(IsNotDynamic()) ClearData();
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
  DTORHELPER(~Sample, UnloadBuffer())
};/* -- End ---------------------------------------------------------------- */
CTOR_END_NOINITS(Samples, Sample, SAMPLE) // Finish collector class
/* ========================================================================= */
static void SampleStop()
{ // Stop all samples from playing
  if(cSamples->empty()) return;
  for(const Sample*const scPtr : *cSamples) scPtr->Stop();
}
/* ========================================================================= */
static void SampleDeInit()
{ // Done if empty
  if(cSamples->empty()) return;
  // Re-create buffers for all the samples and log pre/post de-init
  cLog->LogDebugExSafe("Samples de-initialising $ objects...",
    cSamples->size());
  for(Sample*const scPtr : *cSamples) scPtr->UnloadBuffer();
  cLog->LogInfoExSafe("Samples de-initialised $ objects.",
    cSamples->size());
}
/* ========================================================================= */
static void SampleReInit()
{ // Done if empty
  if(cSamples->empty()) return;
  // Re-create buffers for all the samples and log pre/post reinit
  cLog->LogDebugExSafe("Samples reinitialising $ objects...",
    cSamples->size());
  for(Sample*const scPtr : *cSamples) scPtr->ReloadSample();
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
      if(scRef.GetExternal()) continue;
      // Set new sample volume
      scRef.SetGain(cMixer->MixerGetAdjSampleVolume());
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
