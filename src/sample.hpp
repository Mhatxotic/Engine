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
using namespace ILuaLib::P;            using namespace ILuaUtil::P;
using namespace IOal::P;               using namespace IPcm::P;
using namespace IStd::P;               using namespace ISource::P;
using namespace ISysUtil::P;           using namespace Lib::OpenAL;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* -- Sample collector and member class ------------------------------------ */
CTOR_BEGIN_NOBB(Samples, Sample, CLHelperUnsafe)
/* -- Sample member class -------------------------------------------------- */
CTOR_MEM_BEGIN(Samples, Sample, ICHelperUnsafe, /* n/a */),
  /* -- Base classes ------------------------------------------------------- */
  public Pcm                           // Loaded pcm data
{ /* -- Variables -------------------------------------------------- */ public:
  ALUIntVector      uivNames;          // OpenAL buffer handle ids
  /* -- Get AL buffer index from the specified physical buffer index ------- */
  template<typename IntType=ALint>IntType GetBufferInt(const ALenum aleId,
    const size_t stId=0) const
  { // Hold state
    ALint iValue;
    // Store state
    AL(cOal->GetBufferInt(uivNames[stId], aleId, &iValue),
      "Get buffer integer failed!",
      "Identifier", IdentGet(), "Param", aleId, "Index", stId);
    // Return state
    return static_cast<IntType>(iValue);
  }
  /* -- Get buffer information --------------------------------------------- */
  ALsizei GetALFrequency(void) const
    { return GetBufferInt<ALsizei>(AL_FREQUENCY); }
  ALsizei GetALBits(void) const
    { return GetBufferInt<ALsizei>(AL_BITS); }
  ALsizei GetALChannels(void) const
    { return GetBufferInt<ALsizei>(AL_CHANNELS); }
  ALsizei GetALSize(void) const
    { return GetBufferInt<ALsizei>(AL_SIZE); }
  ALdouble GetDuration(void) const
    { return (static_cast<ALdouble>(GetALSize()) * 8 /
        (GetALChannels() * GetALBits())) / GetALFrequency(); }
  /* -- Unload buffers ----------------------------------------------------- */
  void UnloadBuffer(void)
  { // Bail if buffers not allocated
    if(uivNames.empty()) return;
    // Stop this sample from playing in its entirety
    if(const unsigned int uiStopped = Stop())
      cLog->LogDebugExSafe("Sample '$' cleared $ sources using it!",
        IdentGet(), uiStopped);
    // Delete the buffers
    ALL(cOal->DeleteBuffers(uivNames), "Sample '$' failed to delete $ buffers",
      IdentGet(), uivNames.size());
    // Reset buffer
    uivNames.clear();
  }
  /* ----------------------------------------------------------------------- */
  ALuint PrepareSource(Source &scSource, const ALuint uiBufId,
    const ALfloat fGain, const ALfloat fPan, const ALfloat fPitch,
    const bool bLoop, const bool bLuaManaged)
  { // Set parameters
    scSource.SetBuffer(static_cast<ALint>(uiBufId));
    scSource.SetRelative(true);
    scSource.SetPosition(fPan, 0.0f, -sqrtf(1.0f - fPan * fPan));
    scSource.SetLooping(bLoop);
    scSource.SetPitch(fPitch);
    scSource.SetGain(fGain);
    scSource.SetExternal(bLuaManaged);
    // Return source id
    return scSource.GetSource();
  }
  /* ----------------------------------------------------------------------- */
  ALfloat GetAdjustedGain(const ALfloat fGain)
    { return fGain * cSources->fSVolume * cSources->fGVolume; }
  /* ----------------------------------------------------------------------- */
  void PlayStereoSource(ALfloat fGain, ALfloat fPan,  const ALfloat fPitch,
    const bool bLoop, Source &sCLref, Source &sCRref, const bool bLuaManaged)
  { // Adjust gain from global volumes
    fGain *= GetAdjustedGain(fGain);
    // Half the pan
    fPan *= 0.5f;
    // Prepare sources
    const array<const ALuint,2> aSourceIds{
      PrepareSource(sCLref,           // Left channel
        uivNames.front(), fGain, -0.5f + fPan, fPitch, bLoop, bLuaManaged),
      PrepareSource(sCRref,           // Right chanel
        uivNames[1], fGain, 0.5f + fPan, fPitch, bLoop, bLuaManaged)
    }; // Play all the sources together
    ALL(cOal->PlaySources(aSourceIds),
      "Sample '$' failed to play stereo sources!", IdentGet());
  }
  /* -- Play a mono source ------------------------------------------------- */
  void PlayMonoSource(const ALfloat fGain, ALfloat fPan,
    const ALfloat fPitch, const bool bLoop, Source &sCMref,
    const bool bLuaManaged)
      { ALL(cOal->PlaySource(PrepareSource(sCMref, uivNames.front(),
          GetAdjustedGain(fGain), fPan, fPitch, bLoop, bLuaManaged)),
          "Sample '$' failed to play mono source!", IdentGet()); }
  /* -- Spawn new sources in Lua ------------------------------------------- */
  void Spawn(lua_State*const lS)
  { // How many sources do we need?
    switch(uivNames.size())
    { // 1? (Mono source?) Create a new mono source and set buffer if succeeded
      case 1: if(const Source*const sCMptr = SourceGetFromLua(lS))
                sCMptr->SetBuffer(static_cast<ALint>(uivNames.front()));
              // Failed? Log the failure
              cLog->LogWarningExSafe("Sample cannot get a free source "
                "for spawning '$'!", IdentGet());
              break;
      // 2? (Stereo sources) Get the left channel and set buffer if succeeded
      case 2: if(const Source*const sCLptr = SourceGetFromLua(lS))
                sCLptr->SetBuffer(static_cast<ALint>(uivNames[0]));
              // Failed? Log the failure
              else cLog->LogWarningExSafe("Sample cannot get a free source "
                  "for spawning '$' left channel!", IdentGet());
              // Get a right channel source and set buffer if succeeded
              if(const Source*const sCRptr = SourceGetFromLua(lS))
                sCRptr->SetBuffer(static_cast<ALint>(uivNames[1]));
              // Failed? Log the failure
              else cLog->LogWarningExSafe("Sample cannot get a free source "
                "for spawning '$' right channel!", IdentGet());
              break;
      // Unsupported amount of channels
      default: XC("Unsupported amount of channels!",
                  "Identifier", IdentGet(),
                  "Channels",   uivNames.size());
    }
  }
  /* ----------------------------------------------------------------------- */
  void Play(const ALfloat fGain, const ALfloat fPan, const ALfloat fPitch,
    const bool bLoop)
  { // How many sources do we need?
    switch(uivNames.size())
    { // 1? (Mono source?) Create a new mono source and if we got it? Play it!
      case 1: if(Source*const sCMptr = GetSource())
                PlayMonoSource(fGain, fPan, fPitch, bLoop, *sCMptr, false);
              break;
      // 2? (Stereo sources) Get the left channel and if we got it?
      case 2: if(Source*const sCLptr = GetSource())
              { // Get a right channel source and if we got it? Play it
                if(Source*const sCRptr = GetSource())
                  PlayStereoSource(fGain, fPan, fPitch, bLoop,
                    *sCLptr, *sCRptr, false);
                // Could not grab a right channel source? Log failure
                else cLog->LogWarningExSafe("Sample cannot get a free source "
                  "for playing '$' left channel!", IdentGet());
              } // Could not grab a left channel source? Log failure
              else cLog->LogWarningExSafe("Sample cannot get a free source "
                "for playing '$' right channel!", IdentGet());
              break;
      // 0? (Internal error to log)
      case 0: cLog->LogWarningExSafe(
        "Internal error: Tried to play '$' which has no handles!", IdentGet());
        break;
      // Unsupported amount of channels
      default: XC("Unsupported amount of channels!",
                  "Identifier", IdentGet(),
                  "Channels",   uivNames.size());
    }
  }
  /* -- Play with a pre-allocated sources by Lua --------------------------- */
  void Play(lua_State*const lS, const ALfloat fGain, const ALfloat fPan,
    const ALfloat fPitch, const bool bLoop)
  { // How many sources do we need?
    switch(uivNames.size())
    { // 1? (Mono source?) Create a new mono source and if we got it? Play it
      case 1: if(Source*const sCMptr = SourceGetFromLua(lS))
                PlayMonoSource(fGain, fPan, fPitch, bLoop, *sCMptr, true);
              // Could not grab a mono channel source? Log failure
              else cLog->LogWarningExSafe("Sample cannot get a free source "
                "for playing '$'!", IdentGet());
              break;
      // 2? (Stereo sources) Get the left channel and if we got it?
      case 2: if(Source*const sCLptr = SourceGetFromLua(lS))
              { // Get a right channel source and if we got it? Play sources
                if(Source*const sCRptr = SourceGetFromLua(lS))
                  PlayStereoSource(fGain, fPan, fPitch, bLoop,
                    *sCLptr, *sCRptr, true);
                // Could not grab a right channel source? Log failure
                else cLog->LogWarningExSafe("Sample cannot get a free source "
                  "for playing '$' left channel!", IdentGet());
              } // Could not grab a left channel source? Log failure
              else cLog->LogWarningExSafe("Sample cannot get a free source "
                "for playing '$' right channel!", IdentGet());
              break;
      // 0? (Internal error to log)
      case 0: cLog->LogWarningExSafe(
        "Internal error: Tried to play '$' which has no handles!", IdentGet());
        break;
      // Unsupported amount of channels
      default: XC("Unsupported amount of channels!",
                  "Identifier", IdentGet(),
                  "Channels",   uivNames.size());
    } // Remember two 'Source' classes are left on the Lua stack on success.
  }
  /* == Stop the buffer ==================================================== */
  unsigned int Stop(void) const { return SourceStop(uivNames); }
  /* -- Load a single buffer from memory ----------------------------------- */
  void LoadSample(Pcm &pcmSrc)
  { // Allocate and generate openal buffers
    uivNames.resize(pcmSrc.GetChannels());
    AL(cOal->CreateBuffers(uivNames), "Error creating sample buffers!",
      "Identifier", pcmSrc.IdentGet(), "Count", uivNames.size());
    // Buffer the left or mono channel
    AL(cOal->BufferData(uivNames.front(), pcmSrc.GetSFormat(),
      pcmSrc.aPcmL, static_cast<ALsizei>(pcmSrc.GetRate())),
        "Error buffering left channel/mono PCM audio data!",
        "Identifier", pcmSrc.IdentGet(),  "Buffer",  uivNames.front(),
        "Format",     pcmSrc.GetFormat(), "MFormat", pcmSrc.GetSFormat(),
        "Rate",       pcmSrc.GetRate(),   "Size",    pcmSrc.aPcmL.MemSize());
    // Stereo sample?
    if(uivNames.size() > 1)
    { // Buffer the right stereo channel
      AL(cOal->BufferData(uivNames[1], pcmSrc.GetSFormat(),
        pcmSrc.aPcmR, static_cast<ALsizei>(pcmSrc.GetRate())),
          "Error buffering right/stereo channel PCM audio data!",
          "Identifier", pcmSrc.IdentGet(),  "Buffer",  uivNames[1],
          "Format",     pcmSrc.GetFormat(), "MFormat", pcmSrc.GetSFormat(),
          "Rate",       pcmSrc.GetRate(),   "Size",    pcmSrc.aPcmR.MemSize());
      // Log progress
      cLog->LogDebugExSafe(
        "Sample '$' uploaded as L:$[$] and R:$[$] at $Hz as format 0x$$.",
        pcmSrc.IdentGet(), uivNames.front(), pcmSrc.aPcmL.MemSize(),
        uivNames[1], pcmSrc.aPcmR.MemSize(), pcmSrc.GetRate(), hex,
        pcmSrc.GetFormat());
    } // Log progress
    else cLog->LogDebugExSafe(
      "Sample '$' uploaded as $[$] at $Hz as format 0x$$.",
      pcmSrc.IdentGet(), uivNames.front(), pcmSrc.aPcmL.MemSize(),
      pcmSrc.GetRate(), hex, pcmSrc.GetFormat());
  }
  /* -- Load a single buffer ----------------------------------------------- */
  void ReloadSample(void)
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
    cLog->LogInfoExSafe("Sample loaded '$'!", IdentGet());
  }
  /* -- Init from a bitmap class ------------------------------------------- */
  void InitSample(Pcm &pcmSrc)
  { // Show filename progress
    cLog->LogDebugExSafe("Sample loading from pcm '$'[$] ($;$;$)...",
      pcmSrc.IdentGet(), pcmSrc.GetAlloc(), pcmSrc.GetRate(),
      pcmSrc.GetChannels(), pcmSrc.GetBits());
    // If source and destination pcm class are not the same?
    if(this != &pcmSrc)
    { // Move pcm data over. The old pcm will be unusable so guest should
      // discard it.
      SwapPcm(pcmSrc);
      // The pcm passed in the arguments is usually still allocated by LUA and
      // will still be registered, so lets put a note in the pcm sample to show
      // that this sample class has nicked the pcm sample.
      pcmSrc.IdentSetEx("!SAM!$!", IdentGet());
    } // Initialise
    LoadSample(*this);
    // Remove all sample data because we can just load it from file again
    // and theres no point taking up precious memory for it.
    if(IsNotDynamic()) ClearData();
  }
  /* -- Constructor -------------------------------------------------------- */
  Sample(void) :
    /* -- Initialisers ----------------------------------------------------- */
    ICHelperSample{ cSamples, this }   // Initialise collector class
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Destructor --------------------------------------------------------- */
  ~Sample(void) { UnloadBuffer(); }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(Sample)              // Suppress default functions for safety
};/* -- End ---------------------------------------------------------------- */
CTOR_END_NOINITS(Samples, Sample, SAMPLE) // Finish collector class
/* ========================================================================= */
static void SampleStop(void)
{ // Stop all samples from playing
  if(cSamples->empty()) return;
  for(const Sample*const scPtr : *cSamples) scPtr->Stop();
}
/* ========================================================================= */
static void SampleDeInit(void)
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
static void SampleReInit(void)
{ // Done if empty
  if(cSamples->empty()) return;
  // Re-create buffers for all the samples and log pre/post re-init
  cLog->LogDebugExSafe("Samples reinitialising $ objects...",
    cSamples->size());
  for(Sample*const scPtr : *cSamples) scPtr->ReloadSample();
  cLog->LogInfoExSafe("Samples reinitialised $ objects.", cSamples->size());
}
/* == Update all streams base volume======================================== */
static void SampleUpdateVolume(void)
{ // Lock source list so it cannot be modified
  const LockGuard lgCollectorLock{ cSources->CollectorGetMutex() };
  // Walk through sources
  for(const Source*const scPtr : *cSources)
  { // Get class
    const Source &scRef = *scPtr;
    // If it is locked then its a sample so ignore it
    if(scRef.GetExternal()) continue;
    // Set new sample volume
    scRef.SetGain(cSources->fGVolume * cSources->fSVolume);
  }
}
/* == Set all streams base volume ========================================== */
static CVarReturn SampleSetVolume(const ALfloat fVolume)
{ // Ignore if invalid value
  if(fVolume < 0.0f || fVolume > 1.0f) return DENY;
  // Store volume (SOURCES class keeps it)
  cSources->fSVolume = fVolume;
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
