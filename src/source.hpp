/* == SOURCE.HPP =========================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## This module defines a class that can be used to play and manage     ## **
** ## buffers in OpenAL. All sources are managed here too so we will      ## **
** ## manage the collector list with concurrency locks instead of the     ## **
** ## ICHelper class. The Lockable class is also used for a different     ## **
** ## reason to all the other interfaces to stop the source from being    ## **
** ## deleted while it is in use by the engine and not OpenAL.            ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace ISource {                    // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace ICollector::P;         using namespace ICVarDef::P;
using namespace IFlags;                using namespace IIdent::P;
using namespace ILockable::P;          using namespace ILog::P;
using namespace ILuaIdent::P;          using namespace ILuaLib::P;
using namespace ILuaUtil::P;           using namespace IOal::P;
using namespace IStd::P;               using namespace ISysUtil::P;
using namespace IUtil::P;              using namespace Lib::OpenAL::Types;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* -- Source flags --------------------------------------------------------- */
BUILD_FLAGS(Source,
  // No flags                          // Source is managed externally
  SF_NONE{Flag(0)},                    SF_EXTERNAL{Flag(1)},
  // Source is managed by LUA
  SF_CLASS{Flag(2)}
);/* -- Source collector class for collector data and custom variables ----- */
CTOR_BEGIN(Sources, Source, CLHelperSafe,
/* ------------------------------------------------------------------------- */
typedef atomic<ALfloat> SafeALFloat;   // Multi-threaded AL float
/* ------------------------------------------------------------------------- */
SafeALFloat        fGVolume;           // Global volume multiplier
SafeALFloat        fMVolume;           // Stream volume multiplier
SafeALFloat        fVVolume;           // Video volume multiplier
SafeALFloat        fSVolume;           // Sample volume multiplier
);/* ----------------------------------------------------------------------- */
CTOR_MEM_BEGIN_CSLAVE(Sources, Source, ICHelperSafe),
  /* -- Base classes ------------------------------------------------------- */
  public Lockable,                     // Lua garbage collector instruction
  public SourceFlags                   // Source flags
{ /* -- Private variables -------------------------------------------------- */
  ALuint           uiId;               // Source id
  /* -- Get/set source float ----------------------------------------------- */
  void SetSourceFloat(const ALenum eP, const ALfloat fV) const
    { AL(cOal->SetSourceFloat(uiId, eP, fV), "Set source float failed!",
        "Index", uiId, "Param", eP, "Value", fV); }
  ALfloat GetSourceFloat(const ALenum eP) const
  { // Make, store and return requested float parameter
    ALfloat fValue;
    AL(cOal->GetSourceFloat(uiId, eP, &fValue),
      "Get source float failed!", "Index", uiId, "Param", eP);
    return fValue;
  }
  /* -- set source integer ------------------------------------------------- */
  void SetSourceInt(const ALenum eParam, const ALint iValue) const
  { // Set the integer value and check for error
    AL(cOal->SetSourceInt(uiId, eParam, iValue),
      "Set source integer failed!",
      "Index", uiId, "Param", eParam, "Value", iValue);
  }
  /* -- Get source integer ------------------------------------------------- */
  template<typename IntType=ALint>
    const IntType GetSourceInt(const ALenum eParam) const
  { // Get the specified value and store the result and return a cast of it
    ALint iValue;
    AL(cOal->GetSourceInt(uiId, eParam, &iValue),
      "Get source integer failed!", "Index", uiId, "Param", eParam);
    return static_cast<IntType>(iValue);
  }
  /* -- Get/set source triple-float -------------------------------- */
  void GetSource3Float(const ALenum eP,
    ALfloat &fX, ALfloat &fY, ALfloat &fZ) const
      { AL(cOal->GetSourceVector(uiId, eP, &fX, &fY, &fZ),
          "Get source vector failed!",
          "Index", uiId, "Param", eP, "X", fX, "Y", fY, "Z", fZ); }
  void SetSource3Float(const ALenum eP,
    const ALfloat fX, const ALfloat fY, const ALfloat fZ) const
      { AL(cOal->SetSourceVector(uiId, eP, fX, fY, fZ),
          "Set source vector failed!",
          "Index", uiId, "Param", eP, "X", fX, "Y", fY, "Z", fZ); }
  /* -- Reset parameters ------------------------------------------- */ public:
  void Init(void)
  { // Reset each property of the source since there is no function to do it
    Rewind();                          // Rewind the source
    ClearBuffer();                     // Reset bound buffer id
    SetPosition(0.0f, 0.0f, 0.0f);     // Reset position
    SetVelocity(0.0f, 0.0f, 0.0f);     // Reset velocity
    SetDirection(0.0f, 0.0f, 0.0f);    // Reset direction
    SetRollOff(0.0f);                  // Reset roll off
    SetRefDist(0.5f);                  // Reset reference distance
    SetMaxDist(1.0f);                  // Reset maximum distance
    SetGain(1.0f);                     // Reset current gain
    SetMinGain(0.0f);                  // Reset minimum gain
    SetMaxGain(1.0f);                  // Reset maximum gain
    SetPitch(1.0f);                    // Reset pitch
    SetRelative(false);                // Reset relative flag
    SetLooping(false);                 // Reset looping flag
  }
  /* -- Reset parameters --------------------------------------------------- */
  void Reset(void) { Init(); SetExternal(true); }
  /* -- Re-initialise the source id ---------------------------------------- */
  void ReInit(void) { uiId = cOal->CreateSource(); Reset(); }
  /* -- Unlock a source so it can be recycled ------------------------------ */
  void Unlock(void)
  { // Clear the attached buffer
    ClearBuffer();
    // Unlock the source so it can be recycled by the audio thread
    FlagReset(SF_NONE);
  }
  /* -- Get/set externally managed source ---------------------------------- */
  bool GetExternal(void) const { return FlagIsSet(SF_EXTERNAL); }
  void SetExternal(const bool bState) { FlagSetOrClear(SF_EXTERNAL, bState); }
  /* -- Get/set externally managed by LUA source --------------------------- */
  bool GetClass(void) const { return FlagIsSet(SF_CLASS); }
  void SetClass(const bool bState) { FlagSetOrClear(SF_CLASS, bState); }
  /* -- Get/set elapsed time ----------------------------------------------- */
  ALfloat GetElapsed(void) const { return GetSourceFloat(AL_SEC_OFFSET); }
  void SetElapsed(const ALfloat fSeconds) const
    { SetSourceFloat(AL_SEC_OFFSET, fSeconds); }
  /* -- Get/set gain ------------------------------------------------------- */
  ALfloat GetGain(void) const { return GetSourceFloat(AL_GAIN); }
  void SetGain(const ALfloat fGain) const { SetSourceFloat(AL_GAIN, fGain); }
  /* -- Get/set minimum gain ----------------------------------------------- */
  ALfloat GetMinGain(void) const { return GetSourceFloat(AL_MIN_GAIN); }
  void SetMinGain(const ALfloat fMinGain) const
    { SetSourceFloat(AL_MIN_GAIN, fMinGain); }
  /* -- Get/set maximum gain ----------------------------------------------- */
  ALfloat GetMaxGain(void) const { return GetSourceFloat(AL_MAX_GAIN); }
  void SetMaxGain(const ALfloat fMaxGain) const
    { SetSourceFloat(AL_MAX_GAIN, fMaxGain); }
  /* -- Get/set pitch ------------------------------------------------------ */
  ALfloat GetPitch(void) const { return GetSourceFloat(AL_PITCH); }
  void SetPitch(const ALfloat fPitch) const
    { SetSourceFloat(AL_PITCH, fPitch); }
  /* -- Get/set reference distance ----------------------------------------- */
  ALfloat GetRefDist(void) const
    { return GetSourceFloat(AL_REFERENCE_DISTANCE); }
  void SetRefDist(const ALfloat fRefDist) const
    { SetSourceFloat(AL_REFERENCE_DISTANCE, fRefDist); }
  /* -- Get/set roll off --------------------------------------------------- */
  ALfloat GetRollOff(void) const { return GetSourceFloat(AL_ROLLOFF_FACTOR); }
  void SetRollOff(const ALfloat fRollOff) const
    { SetSourceFloat(AL_ROLLOFF_FACTOR, fRollOff); }
  /* -- Get/set maximum distance ------------------------------------------- */
  ALfloat GetMaxDist(void) const
    { return GetSourceFloat(AL_MAX_DISTANCE); }
  void SetMaxDist(const ALfloat fMaxDist) const
    { SetSourceFloat(AL_MAX_DISTANCE, fMaxDist); }
  /* -- Get/set looping ---------------------------------------------------- */
  bool GetLooping(void) const
    { return GetSourceInt<ALuint>(AL_LOOPING) == AL_TRUE; }
  void SetLooping(const bool bLooping) const
    { SetSourceInt(AL_LOOPING, bLooping ? AL_TRUE : AL_FALSE); }
  /* -- Get/set relative --------------------------------------------------- */
  bool GetRelative(void) const
    { return GetSourceInt<ALuint>(AL_SOURCE_RELATIVE) == AL_TRUE; }
  void SetRelative(const ALenum bRelative) const
    { SetSourceInt(AL_SOURCE_RELATIVE, bRelative ? AL_TRUE : AL_FALSE); }
  /* -- Get/set direction -------------------------------------------------- */
  void GetDirection(ALfloat &fX, ALfloat &fY, ALfloat &fZ) const
    { GetSource3Float(AL_DIRECTION, fX, fY, fZ); }
  void SetDirection(const ALfloat fX, const ALfloat fY, const ALfloat fZ) const
    { SetSource3Float(AL_DIRECTION, fX, fY, fZ); }
  /* -- Get/set position --------------------------------------------------- */
  void GetPosition(ALfloat &fX, ALfloat &fY, ALfloat &fZ) const
    { GetSource3Float(AL_POSITION, fX, fY, fZ); }
  void SetPosition(const ALfloat fX, const ALfloat fY, const ALfloat fZ) const
    { SetSource3Float(AL_POSITION, fX, fY, fZ); }
  /* -- Get/set velocity --------------------------------------------------- */
  void GetVelocity(ALfloat &fX, ALfloat &fY, ALfloat &fZ) const
    { GetSource3Float(AL_VELOCITY, fX, fY, fZ); }
  void SetVelocity(const ALfloat fX, const ALfloat fY,
    const ALfloat fZ) const { SetSource3Float(AL_VELOCITY, fX, fY, fZ); }
  /* -- Get source id ------------------------------------------------------ */
  ALuint GetSource(void) const { return uiId; }
  /* -- Get/set/clear buffer id -------------------------------------------- */
  ALuint GetBuffer(void) const { return GetSourceInt<ALuint>(AL_BUFFER); }
  void SetBuffer(const ALint iBufferId) const
    { SetSourceInt(AL_BUFFER, iBufferId); }
  void ClearBuffer(void) { SetBuffer(0); }
  /* -- Get* --------------------------------------------------------------- */
  ALenum GetState(void) const { return GetSourceInt<ALenum>(AL_SOURCE_STATE); }
  ALsizei GetBuffersProcessed(void) const
    { return GetSourceInt<ALsizei>(AL_BUFFERS_PROCESSED); }
  ALsizei GetBuffersQueued(void) const
    { return GetSourceInt<ALsizei>(AL_BUFFERS_QUEUED); }
  ALsizei GetBuffersTotal(void) const
    { return GetBuffersProcessed() + GetBuffersQueued(); }
  ALuint GetType(void) const { return GetSourceInt<ALuint>(AL_SOURCE_TYPE); }
  /* -- QueueBuffers ----------------------------------------------- */
  void QueueBuffers(ALuint *puiBuffers, const ALsizei stCount) const
  { // Queue buffers
    AL(cOal->QueueBuffers(uiId, stCount, puiBuffers),
      "Queue buffers failed on source!",
      "Index", uiId, "Buffers", puiBuffers, "Count", stCount);
  }
  /* -- Queue one buffer --------------------------------------------------- */
  void QueueBuffer(const ALuint uiBuffer) const
  { // Queue buffers
    AL(cOal->QueueBuffer(uiId, uiBuffer), "Queue one buffer failed on source!",
      "Index", uiId, "Buffer", uiBuffer);
  }
  /* -- UnQueueBuffers ----------------------------------------------------- */
  void UnQueueBuffers(ALuint *puiBuffers, const ALsizei stCount) const
  { // UnQueue buffers
    ALL(cOal->UnQueueBuffers(uiId, stCount, puiBuffers),
      "Source unqueue on $ failed with buffers $($)!",
      uiId, puiBuffers, stCount);
  }
  /* -- UnQueueBuffer ------------------------------------------------------ */
  void UnQueueBuffer(ALuint uiBuffer) const
  { // UnQueue buffers
    ALL(cOal->UnQueueBuffer(uiId, uiBuffer),
      "Source unqueue on $ failed with buffer $!", uiId, uiBuffer);
  }
  /* -- UnQueue one buffer ------------------------------------------------- */
  ALuint UnQueueBuffer(void) { return cOal->UnQueueBuffer(uiId); }
  /* -- UnQueueAlLBuffers -------------------------------------------------- */
  void UnQueueAllBuffers(void)
  { // Get number of buffers and if we have some to unqueue?
    if(const ALsizei stBuffersProcessed = GetBuffersProcessed())
    { // Create memory for these buffers and get the pointer to its memory.
      // This function shouldn't be repeatedly called so it should be ok.
      // Don't change this from () to {}.
      ALUIntVector vBuffers(static_cast<size_t>(stBuffersProcessed));
      // Unqueue the buffers
      UnQueueBuffers(vBuffers.data(), stBuffersProcessed);
    }
  }
  /* -- StopAndUnQueueAlLBuffers ------------------------------------------- */
  void StopAndUnQueueAllBuffers(void)
  { // Stop the playback
    Stop();
    // Unqueue all the buffers
    UnQueueAllBuffers();
  }
  /* -- UnQueueAndDeleteAllBuffers ----------------------------------------- */
  void UnQueueAndDeleteAllBuffers(void)
  { // Clear the buffer from the source. Apple AL needs this or
    // alDeleteBuffers() returns AL_INVALID_OPERATION.
    ClearBuffer();
    // Get number of buffers and if we have some to unqueue?
    if(const ALsizei stBuffersProcessed = GetBuffersProcessed())
    { // Create memory for these buffers and get the pointer to its memory.
      // This function shouldn't be repeatedly called so it should be ok.
      // Don't change this from () to {}.
      ALUIntVector vBuffers(static_cast<size_t>(stBuffersProcessed));
      // Unqueue the buffers
      UnQueueBuffers(vBuffers.data(), stBuffersProcessed);
      // Delete the buffers
      ALL(cOal->DeleteBuffers(stBuffersProcessed, vBuffers.data()),
        "Source delete $ buffers failed at $!",
          stBuffersProcessed, vBuffers.data());
    }
  }
  /* -- StopUnQueueAndDeleteAllBuffers ------------------------------------- */
  void StopUnQueueAndDeleteAllBuffers(void)
  { // Stop playback
    Stop();
    // Unqueue and delete all buffers
    UnQueueAndDeleteAllBuffers();
  }
  /* -- Stop --------------------------------------------------------------- */
  bool Stop(void)
  { // If source already stopped? return!
    if(IsStopped()) return false;
    // Stop it
    AL(cOal->StopSource(uiId), "Source failed to stop!", "Id", uiId);
    // Sucesss
    return true;
  }
  /* -- IsStopped ---------------------------------------------------------- */
  bool IsStopped(void) const { return GetState() == AL_STOPPED; }
  /* -- IsPlaying ---------------------------------------------------------- */
  bool IsPlaying(void) const { return GetState() == AL_PLAYING; }
  /* -- Rewind ------------------------------------------------------------- */
  void Rewind(void)
  { // Play the source
    AL(cOal->RewindSource(uiId), "Rewind failed on source!", "Index", uiId);
  }
  /* -- Play --------------------------------------------------------------- */
  void Play(void)
  { // If playing return
    if(IsPlaying()) return;
    // Play the source
    AL(cOal->PlaySource(uiId), "Play failed on source!", "Index", uiId);
  }
  /* -- Constructor -------------------------------------------------------- */
  explicit Source(
    /* -- Parameters ------------------------------------------------------- */
    const SourceFlagsConst sfFlags=SF_EXTERNAL) :  // Initial source flags
    /* -- Initialisers ----------------------------------------------------- */
    ICHelperSource{ cSources, this },  // Register in Sources list
    IdentCSlave{ cParent->CtrNext() }, // Initialise identification number
    SourceFlags{ sfFlags },            // Set source managed flags
    uiId(cOal->CreateSource())         // Initialise a new source from OpenAL
    /* -- Check for CreateSource error or initialise ----------------------- */
    { // Generate source
      ALC("Error generating al source id!");
      // Reset source parameters and set as
      Init();
    }
  /* -- Destructor --------------------------------------------------------- */
  ~Source(void)
  { // Delete the sourcess if id allocated
    if(uiId) ALL(cOal->DeleteSource(uiId), "Source failed to delete $!", uiId);
  }
};/* -- End ---------------------------------------------------------------- */
CTOR_END(Sources, Source, SOURCE,,,,
  fGVolume(0.0f), fMVolume(0.0f), fVVolume(0.0f), fSVolume(0.0f))
/* -- Stop (multiple buffers) ---------------------------------------------- */
static unsigned int SourceStop(const ALUIntVector &uiBuffers)
{ // Done if no buffers
  if(uiBuffers.empty()) return 0;
  // Buffers closed counter
  unsigned int uiStopped = 0;
  // Iterate through sources
  for(Source*const sCptr : *cSources)
  { // Ignore if locked stream or no sour
    if(sCptr->GetExternal()) continue;
    // Get sources buffer id and ignore if it is not set
    if(const ALuint uiSB = sCptr->GetBuffer())
    { // Find a matching buffer and skip if source doesn't have this buffer id
      if(StdFindIf(par_unseq, uiBuffers.cbegin(), uiBuffers.cend(),
        [uiSB](const ALuint &uiB) { return uiSB == uiB; }) == uiBuffers.cend())
          continue;
      // Stop buffer and add to stopped counter if succeeded
      if(sCptr->Stop()) ++uiStopped;
      // Clear the buffer from the source so the buffer can unload
      sCptr->ClearBuffer();
    } // Else buffer id not acquired
  } // Else return stopped buffers
  return uiStopped;
}
/* == Destroy all sources (except LUA ones) ================================ */
static void SourceDeInit(void)
{ // Done if empty
  if(cSources->empty()) return;
  // Walk through the sources, only deleting the ones that aren't LUA owned
  const size_t stSources = cSources->size();
  cLog->LogDebugExSafe("Sources de-initialising $ objects...", stSources);
  // Get first item and repeat until...
  SourcesIt siIt{ cSources->begin() };
  do
  { // Get reference to source class
    Source &soRef = **siIt;
    // If it is not LUA managed then delete it because the Sample, Video and
    // Stream classes will all re-initialise a new source again.
    if(!soRef.GetClass()) { siIt = cSources->erase(siIt); continue; }
    // Is a LUA class so just stop and clear it. LUA GC deletes this, not us.
    soRef.Stop();
    soRef.ClearBuffer();
    ++siIt;
  } // ...until we're at the end of the sources list
  while(siIt != cSources->end());
  // Log how many sources were deleted
  cLog->LogInfoExSafe("Sources de-initialised $ objects.",
    stSources - cSources->size());
}
/* ========================================================================= */
static void SourceReInit(void)
{ // Done if empty
  if(cSources->empty()) return;
  // Re-create buffers for all the samples and log pre/post re-init
  cLog->LogDebugExSafe("Sources reinitialising $ objects...",
    cSources->size());
  for(Source*const soPtr : *cSources) soPtr->ReInit();
  cLog->LogInfoExSafe("Sources reinitialised $ objects.", cSources->size());
}
/* == Manage sources (from audio thread) =================================== */
static Source *SourceGetFree(void)
{ // Iterate through available sources
  for(Source*const sCptr : *cSources)
  { // Is a locked stream? Then it's active and locked!
    if(sCptr->GetExternal() || sCptr->IsPlaying()) continue;
    // Reset source and return it
    sCptr->Reset();
    return sCptr;
  } // Couldn't find one
  return nullptr;
}
/* == Returns if we can make a new source ================================== */
static bool SourceCanMakeNew(void)
  { return cSources->size() < cOal->GetMaxMonoSources(); }
/* == Get a source using Lua to allocate it ================================ */
static Source *SourceGetFromLua(lua_State*const lS)
{ // Try to get a used source and if failed?
  if(Source*const soNew = SourceGetFree())
  { // Set that this is a LUA managed class
    soNew->SetClass(true);
    // Return reused class
    return LuaUtilClassReuse<Source>(lS, *cSources, soNew);
  } // If we can make a new source?
  if(SourceCanMakeNew())
    // Try to make a new one and if successful?
    if(Source*const soNew = LuaUtilClassCreate<Source>(lS, *cSources))
    { // Set that this is a LUA managed class
      soNew->SetClass(true);
      // Return the class
      return soNew;
    } // Failed so caller should handle this (for now).
  return nullptr;
}
/* == Return a free source ================================================= */
static Source *GetSource(void)
{ // Try to get an idle source and return it if possible
  if(Source*const soNew = SourceGetFree()) return soNew;
  // Else return a brand new source
  return SourceCanMakeNew() ? new Source : nullptr;
}
/* == SourceAlloc ========================================================== */
static void SourceAlloc(const size_t stCount)
{ // Set the value we can actually use
  const size_t stUsable = UtilClamp(stCount, 0, cOal->GetMaxMonoSources());
  // Get size and return if no new static sources needed to be created
  const size_t stSize = cSources->size();
  if(stSize >= stUsable) return;
  // Create new sources until we've reached the maximum and mark as usable
  for(size_t stI = stSize; stI < stUsable; ++stI) new Source{ SF_NONE };
  // Log count
  cLog->LogDebugExSafe("Audio added new sources [$:$$].",
    cSources->size(), hex, cOal->GetError());
}
/* == Set number of sources ================================================ */
static CVarReturn SourceSetCount(const size_t stCount)
{ // If AL is initialised reallocate the sources and return success
  if(cOal->IsInitialised()) SourceAlloc(stCount);
  return ACCEPT;
}
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
