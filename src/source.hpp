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
using namespace IFlags::P;             using namespace ILockable::P;
using namespace ILog::P;               using namespace ILuaIdent::P;
using namespace ILuaLib::P;            using namespace ILuaUtil::P;
using namespace IName::P;              using namespace IOal::P;
using namespace ISerial::P;            using namespace IStd::P;
using namespace ISysUtil::P;           using namespace IUtil::P;
using namespace Lib::OpenAL::Types;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* -- Public typedefs ------------------------------------------------------ */
BUILD_FLAGS(Source,                    // Source flags
  /* ----------------------------------------------------------------------- */
  SF_NONE                   {Flag(0)}, // No flags
  SF_EXTERNAL               {Flag(1)}, // Source is managed externally
  SF_CLASS                  {Flag(2)}  // Source is managed by LUA
);/* -- Source collector class for collector data and custom variables ----- */
CTOR_BEGIN(Sources, Source, CLHelperSafe, size_t stSources;);
/* ------------------------------------------------------------------------- */
CTOR_MEM_BEGIN_CSLAVE(Sources, Source, ICHelperSafe),
  /* -- Base classes ------------------------------------------------------- */
  public Lockable,                     // Lua garbage collector instruction
  public SourceFlags                   // Source flags
{ /* -- Private variables -------------------------------------------------- */
  ALuint           aluId;              // Source id
  /* -- Get/set source float ----------------------------------------------- */
  void SetSourceFloat(const ALenum aleP, const ALfloat alfV) const
    { AL(cOal->SetSourceFloat(aluId, aleP, alfV), "Set source float failed!",
        "Index", aluId, "Param", aleP, "Value", alfV); }
  ALfloat GetSourceFloat(const ALenum aleP) const
  { // Make, store and return requested float parameter
    ALfloat fValue;
    AL(cOal->GetSourceFloat(aluId, aleP, &fValue),
      "Get source float failed!", "Index", aluId, "Param", aleP);
    return fValue;
  }
  /* -- set source integer ------------------------------------------------- */
  void SetSourceInt(const ALenum aleParam, const ALint aliValue) const
  { // Set the integer value and check for error
    AL(cOal->SetSourceInt(aluId, aleParam, aliValue),
      "Set source integer failed!",
      "Index", aluId, "Param", aleParam, "Value", aliValue);
  }
  /* -- Get source integer ------------------------------------------------- */
  template<typename IntType = ALint>
    requires StdIsIntegral<IntType>
  IntType GetSourceInt(const ALenum aleParam) const
  { // Get the specified value and store the result and return a cast of it
    ALint aliValue;
    AL(cOal->GetSourceInt(aluId, aleParam, &aliValue),
      "Get source integer failed!", "Index", aluId, "Param", aleParam);
    return static_cast<IntType>(aliValue);
  }
  /* -- Get/set source triple-float ---------------------------------------- */
  void GetSource3Float(const ALenum aleP,
    ALfloat &alfX, ALfloat &alfY, ALfloat &alfZ) const
  { AL(cOal->GetSourceVector(aluId, aleP, &alfX, &alfY, &alfZ),
      "Get source vector failed!",
      "Index", aluId, "Param", aleP, "X", alfX, "Y", alfY, "Z", alfZ); }
  void SetSource3Float(const ALenum aleP,
    const ALfloat alfX, const ALfloat alfY, const ALfloat alfZ) const
  { AL(cOal->SetSourceVector(aluId, aleP, alfX, alfY, alfZ),
      "Set source vector failed!",
      "Index", aluId, "Param", aleP, "X", alfX, "Y", alfY, "Z", alfZ); }
  /* -- Reset parameters ------------------------------------------- */ public:
  void Init()
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
  void Reset() { Init(); SetExternal(true); }
  /* -- Reinitialise the source id ----------------------------------------- */
  void ReInit() { aluId = cOal->CreateSource(); Reset(); }
  /* -- Unlock a source so it can be recycled ------------------------------ */
  void Unlock()
  { // Clear the attached buffer
    ClearBuffer();
    // Unlock the source so it can be recycled by the audio thread
    FlagReset(SF_NONE);
  }
  /* -- Get/set externally managed source ---------------------------------- */
  bool GetExternal() const { return FlagIsSet(SF_EXTERNAL); }
  void SetExternal(const bool bState) { FlagSetOrClear(SF_EXTERNAL, bState); }
  /* -- Get/set externally managed by LUA source --------------------------- */
  bool GetClass() const { return FlagIsSet(SF_CLASS); }
  void SetClass(const bool bState) { FlagSetOrClear(SF_CLASS, bState); }
  /* -- Get/set elapsed time ----------------------------------------------- */
  ALfloat GetElapsed() const { return GetSourceFloat(AL_SEC_OFFSET); }
  void SetElapsed(const ALfloat alfSeconds) const
    { SetSourceFloat(AL_SEC_OFFSET, alfSeconds); }
  /* -- Get/set gain ------------------------------------------------------- */
  ALfloat GetGain() const { return GetSourceFloat(AL_GAIN); }
  void SetGain(const ALfloat alfGain) const
    { SetSourceFloat(AL_GAIN, alfGain); }
  /* -- Get/set minimum gain ----------------------------------------------- */
  ALfloat GetMinGain() const { return GetSourceFloat(AL_MIN_GAIN); }
  void SetMinGain(const ALfloat alfMinGain) const
    { SetSourceFloat(AL_MIN_GAIN, alfMinGain); }
  /* -- Get/set maximum gain ----------------------------------------------- */
  ALfloat GetMaxGain() const { return GetSourceFloat(AL_MAX_GAIN); }
  void SetMaxGain(const ALfloat alfMaxGain) const
    { SetSourceFloat(AL_MAX_GAIN, alfMaxGain); }
  /* -- Get/set pitch ------------------------------------------------------ */
  ALfloat GetPitch() const { return GetSourceFloat(AL_PITCH); }
  void SetPitch(const ALfloat alfPitch) const
    { SetSourceFloat(AL_PITCH, alfPitch); }
  /* -- Get/set reference distance ----------------------------------------- */
  ALfloat GetRefDist() const
    { return GetSourceFloat(AL_REFERENCE_DISTANCE); }
  void SetRefDist(const ALfloat alfRefDist) const
    { SetSourceFloat(AL_REFERENCE_DISTANCE, alfRefDist); }
  /* -- Get/set roll off --------------------------------------------------- */
  ALfloat GetRollOff() const { return GetSourceFloat(AL_ROLLOFF_FACTOR); }
  void SetRollOff(const ALfloat alfRollOff) const
    { SetSourceFloat(AL_ROLLOFF_FACTOR, alfRollOff); }
  /* -- Get/set maximum distance ------------------------------------------- */
  ALfloat GetMaxDist() const
    { return GetSourceFloat(AL_MAX_DISTANCE); }
  void SetMaxDist(const ALfloat alfMaxDist) const
    { SetSourceFloat(AL_MAX_DISTANCE, alfMaxDist); }
  /* -- Get/set looping ---------------------------------------------------- */
  bool GetLooping() const
    { return GetSourceInt<ALuint>(AL_LOOPING) == AL_TRUE; }
  void SetLooping(const bool bLooping) const
    { SetSourceInt(AL_LOOPING, bLooping ? AL_TRUE : AL_FALSE); }
  /* -- Get/set relative --------------------------------------------------- */
  bool GetRelative() const
    { return GetSourceInt<ALuint>(AL_SOURCE_RELATIVE) == AL_TRUE; }
  void SetRelative(const bool bRelative) const
    { SetSourceInt(AL_SOURCE_RELATIVE, bRelative ? AL_TRUE : AL_FALSE); }
  /* -- Get/set direction -------------------------------------------------- */
  void GetDirection(ALfloat &alfX, ALfloat &alfY, ALfloat &alfZ) const
    { GetSource3Float(AL_DIRECTION, alfX, alfY, alfZ); }
  void SetDirection(const ALfloat alfX, const ALfloat alfY,
    const ALfloat alfZ) const
  { SetSource3Float(AL_DIRECTION, alfX, alfY, alfZ); }
  /* -- Get/set position --------------------------------------------------- */
  void GetPosition(ALfloat &alfX, ALfloat &alfY, ALfloat &alfZ) const
    { GetSource3Float(AL_POSITION, alfX, alfY, alfZ); }
  void SetPosition(const ALfloat alfX, const ALfloat alfY,
    const ALfloat alfZ) const
  { SetSource3Float(AL_POSITION, alfX, alfY, alfZ); }
  /* -- Get/set velocity --------------------------------------------------- */
  void GetVelocity(ALfloat &alfX, ALfloat &alfY, ALfloat &alfZ) const
    { GetSource3Float(AL_VELOCITY, alfX, alfY, alfZ); }
  void SetVelocity(const ALfloat alfX, const ALfloat alfY,
    const ALfloat alfZ) const
  { SetSource3Float(AL_VELOCITY, alfX, alfY, alfZ); }
  /* -- Get source id ------------------------------------------------------ */
  ALuint GetSource() const { return aluId; }
  /* -- Get/set/clear buffer id -------------------------------------------- */
  ALuint GetBuffer() const { return GetSourceInt<ALuint>(AL_BUFFER); }
  void SetBuffer(const ALint aliBufferId) const
    { SetSourceInt(AL_BUFFER, aliBufferId); }
  void ClearBuffer() { SetBuffer(0); }
  /* -- Get* --------------------------------------------------------------- */
  ALenum GetState() const { return GetSourceInt<ALenum>(AL_SOURCE_STATE); }
  ALsizei GetBuffersProcessed() const
    { return GetSourceInt<ALsizei>(AL_BUFFERS_PROCESSED); }
  ALsizei GetBuffersQueued() const
    { return GetSourceInt<ALsizei>(AL_BUFFERS_QUEUED); }
  ALsizei GetBuffersTotal() const
    { return GetBuffersProcessed() + GetBuffersQueued(); }
  ALuint GetType() const { return GetSourceInt<ALuint>(AL_SOURCE_TYPE); }
  /* -- QueueBuffers ----------------------------------------------- */
  void QueueBuffers(ALuint*const alupBuffers, const ALsizei alsiCount) const
    { AL(cOal->QueueBuffers(aluId, alsiCount, alupBuffers),
        "Queue buffers failed on source!",
        "Index", aluId, "Buffers", alupBuffers, "Count", alsiCount); }
  /* -- Queue one buffer --------------------------------------------------- */
  void QueueBuffer(const ALuint aluBuffer) const
    { AL(cOal->QueueBuffer(aluId, aluBuffer),
        "Queue one buffer failed on source!",
        "Index", aluId, "Buffer", aluBuffer); }
  /* -- UnQueueBuffers ----------------------------------------------------- */
  void UnQueueBuffers(ALuint*const alupBuffers, const ALsizei alsiCount) const
    { ALL(cOal->UnQueueBuffers(aluId, alsiCount, alupBuffers),
        "Source unqueue on $ failed with buffers $($)!",
        aluId, alupBuffers, alsiCount); }
  /* -- UnQueueBuffer ------------------------------------------------------ */
  void UnQueueBuffer(ALuint aluBuffer) const
    { ALL(cOal->UnQueueBuffer(aluId, aluBuffer),
        "Source unqueue on $ failed with buffer $!", aluId, aluBuffer); }
  /* -- UnQueue one buffer ------------------------------------------------- */
  ALuint UnQueueBuffer() { return cOal->UnQueueBuffer(aluId); }
  /* -- UnQueueAlLBuffers -------------------------------------------------- */
  void UnQueueAllBuffers()
  { // Get number of buffers and if we have some to unqueue?
    if(const ALsizei alsiBuffersProcessed = GetBuffersProcessed())
    { // Create memory for these buffers and get the pointer to its memory.
      // This function shouldn't be repeatedly called so it should be ok.
      // Don't change this from () to {}.
      ALUIntVector aluvBuffers(static_cast<size_t>(alsiBuffersProcessed));
      UnQueueBuffers(aluvBuffers.data(), alsiBuffersProcessed);
    }
  }
  /* -- StopAndUnQueueAlLBuffers ------------------------------------------- */
  void StopAndUnQueueAllBuffers() { Stop(); UnQueueAllBuffers(); }
  /* -- UnQueueAndDeleteAllBuffers ----------------------------------------- */
  void UnQueueAndDeleteAllBuffers()
  { // Clear the buffer from the source. Apple AL needs this or
    // alDeleteBuffers() returns AL_INVALID_OPERATION.
    ClearBuffer();
    // Get number of buffers and if we have some to unqueue?
    if(const ALsizei alsiBuffersProcessed = GetBuffersProcessed())
    { // Create memory for these buffers and get the pointer to its memory.
      // This function shouldn't be repeatedly called so it should be ok.
      // Don't change this from () to {}.
      ALUIntVector aluvBuffers(static_cast<size_t>(alsiBuffersProcessed));
      // Unqueue the buffers
      UnQueueBuffers(aluvBuffers.data(), alsiBuffersProcessed);
      // Delete the buffers
      ALL(cOal->DeleteBuffers(alsiBuffersProcessed, aluvBuffers.data()),
        "Source delete $ buffers failed at $!",
          alsiBuffersProcessed, aluvBuffers.data());
    }
  }
  /* -- StopUnQueueAndDeleteAllBuffers ------------------------------------- */
  void StopUnQueueAndDeleteAllBuffers()
    { Stop(); UnQueueAndDeleteAllBuffers(); }
  /* -- Stop --------------------------------------------------------------- */
  bool Stop()
  { // Ignore if stopped else stop it and return success
    if(IsStopped()) return false;
    AL(cOal->StopSource(aluId), "Source failed to stop!", "Id", aluId);
    return true;
  }
  /* -- IsStopped ---------------------------------------------------------- */
  bool IsStopped() const { return GetState() == AL_STOPPED; }
  /* -- IsPlaying ---------------------------------------------------------- */
  bool IsPlaying() const { return GetState() == AL_PLAYING; }
  /* -- Rewind ------------------------------------------------------------- */
  void Rewind()
    { AL(cOal->RewindSource(aluId),
        "Rewind failed on source!", "Index", aluId); }
  /* -- Return if buffer is valid ------------------------------------------ */
  bool IsValid() const { return cOal->IsBuffer(GetSource()); }
  bool IsNotValid() const { return !IsValid(); }
  /* -- Play --------------------------------------------------------------- */
  bool Play()
  { // If playing return else play the source
    if(IsPlaying()) return false;
    AL(cOal->PlaySource(aluId), "Play failed on source!", "Index", aluId);
    return true;
  }
  /* -- Play with another source ------------------------------------------- */
  bool Play(const Source &soOther)
  { // If playing return else prepare sources and play them together
    if(IsPlaying() || soOther.IsPlaying()) return false;
    const StdArray<const ALuint,2>
      aSourceIds{ GetSource(), soOther.GetSource() };
    AL(cOal->PlaySources(aSourceIds), "Play failed on sources!",
      "LeftIndex",  GetSource(),         "LeftValid",  IsValid(),
      "RightIndex", soOther.GetSource(), "RightValid", soOther.IsValid());
    return true;
  }
  /* -- Constructor -------------------------------------------------------- */
  explicit Source(
    /* -- Parameters ------------------------------------------------------- */
    const SourceFlagsConst sfFlags=SF_EXTERNAL) :  // Initial source flags
    /* -- Initialisers ----------------------------------------------------- */
    ICHelperSource{ cSources, this },  // Register in Sources list
    SerialSlave{ cParent->Serial() },  // Initialise identification number
    SourceFlags{ sfFlags },            // Set source managed flags
    aluId(cOal->CreateSource())        // Initialise a new source from OpenAL
    /* -- Check for CreateSource error or initialise ----------------------- */
    { // Generate source
      ALNF("Error generating al source id!");
      // Reset source parameters and set as
      Init();
    }
  /* -- Destructor --------------------------------------------------------- */
  DTORHELPER(~Source,
    // Delete the sourcess if id allocated
    if(aluId) ALL(cOal->DeleteSource(aluId),
      "Source failed to delete $!", aluId);
  )
};/* -- End ---------------------------------------------------------------- */
CTOR_END(Sources, Source, SOURCE,,,, stSources(0))
/* -- Stop (multiple buffers) ---------------------------------------------- */
static unsigned SourceStop(const ALUIntVector &uBuffers)
{ // Done if no buffers
  if(uBuffers.empty()) return 0;
  // Buffers closed counter
  unsigned uStopped = 0;
  // Iterate through sources
  for(Source*const sCptr : *cSources)
  { // Ignore if locked stream or no sour
    if(sCptr->GetExternal()) continue;
    // Get sources buffer id and ignore if it is not set
    if(const ALuint aluSB = sCptr->GetBuffer())
    { // Find a matching buffer and skip if source doesn't have this buffer id
      if(StdFindIf(par_unseq, uBuffers.cbegin(), uBuffers.cend(),
           [aluSB](const ALuint &aluB){ return aluSB == aluB; })
         == uBuffers.cend())
        continue;
      // Stop buffer and add to stopped counter if succeeded
      if(sCptr->Stop()) ++uStopped;
      // Clear the buffer from the source so the buffer can unload
      sCptr->ClearBuffer();
    } // Else buffer id not acquired
  } // Else return stopped buffers
  return uStopped;
}
/* == Destroy all sources (except LUA ones) ================================ */
static void SourceDeInit()
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
    // Stream classes will all reinitialise a new source again.
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
static void SourceReInit()
{ // Done if empty
  if(cSources->empty()) return;
  // Re-create buffers for all the samples and log pre/post reinit
  cLog->LogDebugExSafe("Sources reinitialising $ objects...",
    cSources->size());
  for(Source*const soPtr : *cSources) soPtr->ReInit();
  cLog->LogInfoExSafe("Sources reinitialised $ objects.", cSources->size());
}
/* == Manage sources (from audio thread) =================================== */
static Source *SourceGetFree()
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
static bool SourceCanMakeNew()
  { return cSources->size() < cOal->GetMaxMonoSources(); }
/* == Get a source using Lua to allocate it ================================ */
static Source *SourceGetFromLua(lua_State*const lS)
{ // Try to get a used source and if failed?
  if(Source*const soNew = SourceGetFree())
  { // Set that this is a LUA managed class
    soNew->SetClass(true);
    // Return reused class
    return LuaUtilClassReuse<Source>(lS, *cSources, soNew);
  } // Return if we can't make a new source
  if(!SourceCanMakeNew()) return nullptr;
  // Make a new source (exception is already thrown on error)
  Source*const soNew = LuaUtilClassCreate<Source>(lS, cSources);
  // Set that this is a LUA managed class
  soNew->SetClass(true);
  // Return the class
  return soNew;
}
/* == Return a free source ================================================= */
static Source *GetSource()
{ // Try to get an idle source and return it if possible
  if(Source*const soNew = SourceGetFree()) return soNew;
  // Else return a brand new source
  return SourceCanMakeNew() ? new Source : nullptr;
}
/* == SourceAlloc ========================================================== */
static bool SourceAlloc(const size_t stCount)
{ // Get the value we can actually use and the number of sources currently
  // allocated. Return if no new static sources are needed to be created.
  if(stCount >= cOal->GetMaxMonoSources()) return false;
  // Create new sources until we've reached the maximum and mark as usable.
  // The audio thread could already be polling these right now so all three of
  // these lines will lock access to the list.
  const size_t stBefore = cSources->CollectorCount();
  for(size_t stIndex = stBefore; stIndex < stCount; ++stIndex)
    new Source{ SF_NONE };
  const size_t stAfter = cSources->CollectorCount();
  // Log count
  cLog->LogDebugExSafe("Audio added $ new stand-by sources to total $.",
    stAfter - stBefore, stAfter);
  // Return success
  return true;
}
/* == Set number of sources ================================================ */
static CVarReturn SourceSetCount(const size_t stCount)
{ // Set preallocated source count for Audio class when initialising
  cSources->stSources = stCount;
  // Accepted if we can't check it yet, denied if not, or bad value
  return BoolToCVarReturn(cOal->IsNotInitialised() || SourceAlloc(stCount));
}
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
