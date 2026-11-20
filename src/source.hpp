/* == SOURCE.HPP =========================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## This module defines a class that can be used to play and manage     ## **
** ## buffers in OpenAL. All sources are managed here too so we will      ## **
** ## manage the collector list with concurrency locks instead of the     ## **
** ## ICHelper class.                                                     ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace ISource {                    // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace ICollector::P;         using namespace ICVarDef::P;
using namespace IFlags::P;             using namespace ILog::P;
using namespace ILuaIdent::P;          using namespace ILuaLib::P;
using namespace ILuaUtil::P;           using namespace IName::P;
using namespace IOal::P;               using namespace ISerial::P;
using namespace IStd::P;               using namespace ISysUtil::P;
using namespace IUtil::P;              using namespace LAL::Types;
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
  public SourceFlags                   // Source flags
{ /* -- Private variables -------------------------------------------------- */
  ALuint           aluId;              // Source id
  /* -- Set source float --------------------------------------------------- */
  void SourceSetFloat(const ALenum aleP, const ALfloat alfV) const
    { AL(cOal->OalSetSourceFloat(SourceGet(), aleP, alfV),
        "Set source float failed!",
        "Index", SourceGet(), "Param", aleP, "Value", alfV); }
  /* -- Get source float --------------------------------------------------- */
  ALfloat SourceGetFloat(const ALenum aleP) const
  { // Make, store and return requested float parameter
    ALfloat fValue;
    AL(cOal->OalGetSourceFloat(SourceGet(), aleP, &fValue),
      "Get source float failed!", "Index", SourceGet(), "Param", aleP);
    return fValue;
  }
  /* -- set source integer ------------------------------------------------- */
  void SourceSetInt(const ALenum aleParam, const ALint aliValue) const
  { // Set the integer value and check for error
    AL(cOal->OalSetSourceInt(SourceGet(), aleParam, aliValue),
      "Set source integer failed!",
      "Index", SourceGet(), "Param", aleParam, "Value", aliValue);
  }
  /* -- Get source integer ------------------------------------------------- */
  template<typename IntType = ALint>
    requires StdIsIntegral<IntType>
  IntType SourceGetInt(const ALenum aleParam) const
  { // Get the specified value and store the result and return a cast of it
    ALint aliValue;
    AL(cOal->OalGetSourceInt(SourceGet(), aleParam, &aliValue),
      "Get source integer failed!",
      "Index", SourceGet(), "Param", aleParam);
    return static_cast<IntType>(aliValue);
  }
  /* -- Get source float vector (X, Y and Z) ------------------------------- */
  void SourceGetVector(const ALenum aleP,
    ALfloat &alfX, ALfloat &alfY, ALfloat &alfZ) const
  { AL(cOal->OalGetSourceVector(SourceGet(), aleP, &alfX, &alfY, &alfZ),
      "Get source vector failed!",
      "Index", SourceGet(), "Param", aleP,
      "X",     alfX,              "Y",     alfY,
      "Z",     alfZ); }
  /* -- Set source float vector (X, Y and Z) ------------------------------- */
  void SourceSetVector(const ALenum aleP,
    const ALfloat alfX, const ALfloat alfY, const ALfloat alfZ) const
  { AL(cOal->OalSetSourceVector(SourceGet(), aleP, alfX, alfY, alfZ),
      "Set source vector failed!",
      "Index", SourceGet(), "Param", aleP,
      "X",     alfX,              "Y",     alfY,
      "Z",     alfZ); }
  /* -- Reset parameters ------------------------------------------- */ public:
  void SourceInit()
  { // Reset each property of the source since there is no function to do it
    SourceRewind();                       // Rewind the source
    SourceClearBuffer();                  // Reset bound buffer id
    SourceSetPosition(0.0f, 0.0f, 0.0f);  // Reset position
    SourceSetVelocity(0.0f, 0.0f, 0.0f);  // Reset velocity
    SourceSetDirection(0.0f, 0.0f, 0.0f); // Reset direction
    SourceSetRollOff(0.0f);               // Reset roll off
    SourceSetRefDist(0.5f);               // Reset reference distance
    SourceSetMaxDist(1.0f);               // Reset maximum distance
    SourceSetGain(1.0f);                  // Reset current gain
    SourceSetMinGain(0.0f);               // Reset minimum gain
    SourceSetMaxGain(1.0f);               // Reset maximum gain
    SourceSetPitch(1.0f);                 // Reset pitch
    SourceSetRelative(false);             // Reset relative flag
    SourceSetLooping(false);              // Reset looping flag
  }
  /* -- Reset parameters --------------------------------------------------- */
  void SourceReset() { SourceInit(); SourceSetExternal(true); }
  /* -- Reinitialise the source id ----------------------------------------- */
  void SourceReInit() { aluId = cOal->OalCreateSource(); SourceReset(); }
  /* -- Unlock a source so it can be recycled ------------------------------ */
  void SourceUnlock()
  { // Clear the attached buffer
    SourceClearBuffer();
    // Unlock the source so it can be recycled by the audio thread
    FlagReset(SF_NONE);
  }
  /* -- Get/set externally managed source ---------------------------------- */
  bool SourceGetExternal() const { return FlagIsSet(SF_EXTERNAL); }
  void SourceSetExternal(const bool bState)
    { FlagSetOrClear(SF_EXTERNAL, bState); }
  /* -- Get/set externally managed by LUA source --------------------------- */
  bool SourceGetClass() const { return FlagIsSet(SF_CLASS); }
  void SourceSetClass(const bool bState) { FlagSetOrClear(SF_CLASS, bState); }
  /* -- Get/set elapsed time ----------------------------------------------- */
  ALfloat SourceGetElapsed() const { return SourceGetFloat(AL_SEC_OFFSET); }
  void SourceSetElapsed(const ALfloat alfSeconds) const
    { SourceSetFloat(AL_SEC_OFFSET, alfSeconds); }
  /* -- Get/set gain ------------------------------------------------------- */
  ALfloat SourceGetGain() const { return SourceGetFloat(AL_GAIN); }
  void SourceSetGain(const ALfloat alfGain) const
    { SourceSetFloat(AL_GAIN, alfGain); }
  /* -- Get/set minimum gain ----------------------------------------------- */
  ALfloat SourceGetMinGain() const { return SourceGetFloat(AL_MIN_GAIN); }
  void SourceSetMinGain(const ALfloat alfMinGain) const
    { SourceSetFloat(AL_MIN_GAIN, alfMinGain); }
  /* -- Get/set maximum gain ----------------------------------------------- */
  ALfloat SourceGetMaxGain() const { return SourceGetFloat(AL_MAX_GAIN); }
  void SourceSetMaxGain(const ALfloat alfMaxGain) const
    { SourceSetFloat(AL_MAX_GAIN, alfMaxGain); }
  /* -- Get/set pitch ------------------------------------------------------ */
  ALfloat SourceGetPitch() const { return SourceGetFloat(AL_PITCH); }
  void SourceSetPitch(const ALfloat alfPitch) const
    { SourceSetFloat(AL_PITCH, alfPitch); }
  /* -- Get/set reference distance ----------------------------------------- */
  ALfloat SourceGetRefDist() const
    { return SourceGetFloat(AL_REFERENCE_DISTANCE); }
  void SourceSetRefDist(const ALfloat alfRefDist) const
    { SourceSetFloat(AL_REFERENCE_DISTANCE, alfRefDist); }
  /* -- Get/set roll off --------------------------------------------------- */
  ALfloat SourceGetRollOff() const
   { return SourceGetFloat(AL_ROLLOFF_FACTOR); }
  void SourceSetRollOff(const ALfloat alfRollOff) const
    { SourceSetFloat(AL_ROLLOFF_FACTOR, alfRollOff); }
  /* -- Get/set maximum distance ------------------------------------------- */
  ALfloat SourceGetMaxDist() const
    { return SourceGetFloat(AL_MAX_DISTANCE); }
  void SourceSetMaxDist(const ALfloat alfMaxDist) const
    { SourceSetFloat(AL_MAX_DISTANCE, alfMaxDist); }
  /* -- Get/set looping ---------------------------------------------------- */
  bool SourceGetLooping() const
    { return SourceGetInt<ALuint>(AL_LOOPING) == AL_TRUE; }
  void SourceSetLooping(const bool bLooping) const
    { SourceSetInt(AL_LOOPING, bLooping ? AL_TRUE : AL_FALSE); }
  /* -- Get/set relative --------------------------------------------------- */
  bool SourceGetRelative() const
    { return SourceGetInt<ALuint>(AL_SOURCE_RELATIVE) == AL_TRUE; }
  void SourceSetRelative(const bool bRelative) const
    { SourceSetInt(AL_SOURCE_RELATIVE, bRelative ? AL_TRUE : AL_FALSE); }
  /* -- Get/set direction -------------------------------------------------- */
  void SourceGetDirection(ALfloat &alfX, ALfloat &alfY, ALfloat &alfZ) const
    { SourceGetVector(AL_DIRECTION, alfX, alfY, alfZ); }
  void SourceSetDirection(const ALfloat alfX, const ALfloat alfY,
    const ALfloat alfZ) const
  { SourceSetVector(AL_DIRECTION, alfX, alfY, alfZ); }
  /* -- Get/set position --------------------------------------------------- */
  void SourceGetPosition(ALfloat &alfX, ALfloat &alfY, ALfloat &alfZ) const
    { SourceGetVector(AL_POSITION, alfX, alfY, alfZ); }
  void SourceSetPosition(const ALfloat alfX, const ALfloat alfY,
    const ALfloat alfZ) const
  { SourceSetVector(AL_POSITION, alfX, alfY, alfZ); }
  /* -- Get/set velocity --------------------------------------------------- */
  void SourceGetVelocity(ALfloat &alfX, ALfloat &alfY, ALfloat &alfZ) const
    { SourceGetVector(AL_VELOCITY, alfX, alfY, alfZ); }
  void SourceSetVelocity(const ALfloat alfX, const ALfloat alfY,
    const ALfloat alfZ) const
  { SourceSetVector(AL_VELOCITY, alfX, alfY, alfZ); }
  /* -- Get source id ------------------------------------------------------ */
  ALuint SourceGet() const { return aluId; }
  /* -- Get/set/clear buffer id -------------------------------------------- */
  ALuint SourceGetBuffer() const { return SourceGetInt<ALuint>(AL_BUFFER); }
  void SourceSetBuffer(const ALint aliBufferId) const
    { SourceSetInt(AL_BUFFER, aliBufferId); }
  void SourceClearBuffer() { SourceSetBuffer(0); }
  /* -- Get* --------------------------------------------------------------- */
  ALenum SourceGetState() const
    { return SourceGetInt<ALenum>(AL_SOURCE_STATE); }
  ALsizei SourceGetBuffersProcessed() const
    { return SourceGetInt<ALsizei>(AL_BUFFERS_PROCESSED); }
  ALsizei SourceGetBuffersQueued() const
    { return SourceGetInt<ALsizei>(AL_BUFFERS_QUEUED); }
  ALsizei SourceGetBuffersTotal() const
    { return SourceGetBuffersProcessed() + SourceGetBuffersQueued(); }
  ALuint SourceGetType() const { return SourceGetInt<ALuint>(AL_SOURCE_TYPE); }
  /* -- QueueBuffers ----------------------------------------------- */
  void SourceQueueBuffers(ALuint*const alupBuffers,
    const ALsizei alsiCount) const
  { AL(cOal->OalQueueBuffers(SourceGet(), alsiCount, alupBuffers),
      "Queue buffers failed on source!",
      "Index", SourceGet(), "Buffers", alupBuffers,
      "Count", alsiCount); }
  /* -- Queue one buffer --------------------------------------------------- */
  void SourceQueueBuffer(const ALuint aluBuffer) const
    { AL(cOal->OalQueueBuffer(SourceGet(), aluBuffer),
        "Queue one buffer failed on source!",
        "Index", SourceGet(), "Buffer", aluBuffer); }
  /* -- SourceUnQueueBuffers ----------------------------------------------- */
  void SourceUnQueueBuffers(ALuint*const alupBuffers,
    const ALsizei alsiCount) const
  { ALL(cOal->OalUnQueueBuffers(SourceGet(), alsiCount, alupBuffers),
      "Source unqueue on $ failed with buffers $($)!",
      SourceGet(), alupBuffers, alsiCount); }
  /* -- SourceUnQueueBuffer ------------------------------------------------ */
  void SourceUnQueueBuffer(ALuint aluBuffer) const
    { ALL(cOal->OalUnQueueBuffer(SourceGet(), aluBuffer),
        "Source unqueue on $ failed with buffer $!",
        SourceGet(), aluBuffer); }
  /* -- UnQueue one buffer ------------------------------------------------- */
  ALuint SourceUnQueueBuffer()
    { return cOal->OalUnQueueBuffer(SourceGet()); }
  /* -- UnQueueAlLBuffers -------------------------------------------------- */
  void SourceUnQueueAllBuffers()
  { // Get number of buffers and if we have some to unqueue?
    if(const ALsizei alsiBuffersProcessed = SourceGetBuffersProcessed())
    { // Create memory for these buffers and get the pointer to its memory.
      // This function shouldn't be repeatedly called so it should be ok.
      // Don't change this from () to {}.
      ALUIntVector aluvBuffers(static_cast<size_t>(alsiBuffersProcessed));
      SourceUnQueueBuffers(aluvBuffers.data(), alsiBuffersProcessed);
    }
  }
  /* -- SourceStopAndUnQueueAlLBuffers ------------------------------------- */
  void SourceStopAndUnQueueAllBuffers()
    { SourceStop(); SourceUnQueueAllBuffers(); }
  /* -- UnQueueAndDeleteAllBuffers ----------------------------------------- */
  void SourceUnQueueAndDeleteAllBuffers()
  { // Clear the buffer from the source. Apple AL needs this or
    // alDeleteBuffers() returns AL_INVALID_OPERATION.
    SourceClearBuffer();
    // Get number of buffers and if we have some to unqueue?
    if(const ALsizei alsiBuffersProcessed = SourceGetBuffersProcessed())
    { // Create memory for these buffers and get the pointer to its memory.
      // This function shouldn't be repeatedly called so it should be ok.
      // Don't change this from () to {}.
      ALUIntVector aluvBuffers(static_cast<size_t>(alsiBuffersProcessed));
      // Unqueue the buffers
      SourceUnQueueBuffers(aluvBuffers.data(), alsiBuffersProcessed);
      // Delete the buffers
      ALL(cOal->OalDeleteBuffers(alsiBuffersProcessed, aluvBuffers.data()),
        "Source delete $ buffers failed at $!",
          alsiBuffersProcessed, aluvBuffers.data());
    }
  }
  /* -- SourceStopUnQueueAndDeleteAllBuffers ------------------------------- */
  void SourceStopUnQueueAndDeleteAllBuffers()
    { SourceStop(); SourceUnQueueAndDeleteAllBuffers(); }
  /* -- Stop --------------------------------------------------------------- */
  bool SourceStop()
  { // Ignore if stopped else stop it and return success
    if(SourceIsStopped()) return false;
    AL(cOal->OalStopSource(SourceGet()),
      "Source failed to stop!", "Id", SourceGet());
    return true;
  }
  /* -- IsStopped ---------------------------------------------------------- */
  bool SourceIsStopped() const { return SourceGetState() == AL_STOPPED; }
  /* -- IsPlaying ---------------------------------------------------------- */
  bool SourceIsPlaying() const { return SourceGetState() == AL_PLAYING; }
  /* -- Rewind ------------------------------------------------------------- */
  void SourceRewind()
    { AL(cOal->OalRewindSource(SourceGet()),
        "Rewind failed on source!", "Index", SourceGet()); }
  /* -- Return if buffer is valid ------------------------------------------ */
  bool SourceIsValid() const { return cOal->OalIsBuffer(SourceGet()); }
  bool SourceIsNotValid() const { return !SourceIsValid(); }
  /* -- Play --------------------------------------------------------------- */
  bool SourcePlay()
  { // If playing return else play the source
    if(SourceIsPlaying()) return false;
    AL(cOal->OalPlaySource(SourceGet()),
      "Play failed on source!", "Index", SourceGet());
    return true;
  }
  /* -- Play with another source ------------------------------------------- */
  bool SourcePlay(const Source &soOther)
  { // If playing return else prepare sources and play them together
    if(SourceIsPlaying() || soOther.SourceIsPlaying()) return false;
    const StdArray<const ALuint,2>
      aSourceIds{ SourceGet(), soOther.SourceGet() };
    AL(cOal->OalPlaySources(aSourceIds), "Play failed on sources!",
      "LeftIndex",  SourceGet(),       "LeftValid",  SourceIsValid(),
      "RightIndex", soOther.SourceGet(),
      "RightValid", soOther.SourceIsValid());
    return true;
  }
  /* -- Constructor -------------------------------------------------------- */
  explicit Source(
    /* -- Parameters ------------------------------------------------------- */
    const SourceFlagsConst sfFlags=SF_EXTERNAL) :  // Initial source flags
    /* -- Initialisers ----------------------------------------------------- */
    ICHelperSource{ cSources, this },  // Register in Sources list
    SerialSlave{ ctParent.Serial() },  // Initialise identification number
    SourceFlags{ sfFlags },            // Set source managed flags
    aluId(cOal->OalCreateSource())     // Initialise a new source from OpenAL
    /* -- Check for CreateSource error or initialise ----------------------- */
    { // Generate source
      ALNF("Error generating al source id!");
      // Reset source parameters and set as
      SourceInit();
    }
  /* -- Destructor --------------------------------------------------------- */
  DTORHELPER(~Source,
    // Delete the sourcess if id allocated
    if(SourceGet()) ALL(cOal->OalDeleteSource(SourceGet()),
      "Source failed to delete $!", SourceGet());
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
    if(sCptr->SourceGetExternal()) continue;
    // Get sources buffer id and ignore if it is not set
    if(const ALuint aluSB = sCptr->SourceGetBuffer())
    { // Find a matching buffer and skip if source doesn't have this buffer id
      if(StdFindIf(par_unseq, uBuffers.cbegin(), uBuffers.cend(),
           [aluSB](const ALuint &aluB){ return aluSB == aluB; })
         == uBuffers.cend())
        continue;
      // Stop buffer and add to stopped counter if succeeded
      if(sCptr->SourceStop()) ++uStopped;
      // Clear the buffer from the source so the buffer can unload
      sCptr->SourceClearBuffer();
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
  cLog->LogDebugExSafe("Sources deinitialising $ objects...", stSources);
  // Get first item and repeat until...
  SourcesIt siIt{ cSources->begin() };
  do
  { // Get reference to source class
    Source &soRef = **siIt;
    // If it is not LUA managed then delete it because the Sample, Video and
    // Stream classes will all reinitialise a new source again.
    if(!soRef.SourceGetClass()) { siIt = cSources->erase(siIt); continue; }
    // Is a LUA class so just stop and clear it. LUA GC deletes this, not us.
    soRef.SourceStop();
    soRef.SourceClearBuffer();
    ++siIt;
  } // ...until we're at the end of the sources list
  while(siIt != cSources->end());
  // Log how many sources were deleted
  cLog->LogInfoExSafe("Sources deinitialised $ objects.",
    stSources - cSources->size());
}
/* ========================================================================= */
static void SourceReInit()
{ // Done if empty
  if(cSources->empty()) return;
  // Re-create buffers for all the samples and log pre/post reinit
  cLog->LogDebugExSafe("Sources reinitialising $ objects...",
    cSources->size());
  for(Source*const soPtr : *cSources) soPtr->SourceReInit();
  cLog->LogInfoExSafe("Sources reinitialised $ objects.", cSources->size());
}
/* == Manage sources (from audio thread) =================================== */
static Source *SourceGetFree()
{ // Iterate through available sources
  for(Source*const sCptr : *cSources)
  { // Is a locked stream? Then it's active and locked!
    if(sCptr->SourceGetExternal() || sCptr->SourceIsPlaying()) continue;
    // Reset source and return it
    sCptr->SourceReset();
    return sCptr;
  } // Couldn't find one
  return nullptr;
}
/* == Returns if we can make a new source ================================== */
static bool SourceCanMakeNew()
  { return cSources->size() < cOal->OalGetMaxMonoSources(); }
/* == Get a source using Lua to allocate it ================================ */
static Source *SourceGetFromLua(lua_State*const lS)
{ // Try to get a used source and if failed?
  if(Source*const soNew = SourceGetFree())
  { // Set that this is a LUA managed class
    soNew->SourceSetClass(true);
    // Return reused class
    return LuaUtilClassReuse<Source>(lS, *cSources, soNew);
  } // Return if we can't make a new source
  if(!SourceCanMakeNew()) return nullptr;
  // Make a new source (exception is already thrown on error)
  Source*const soNew = LuaUtilClassCreate<Source>(lS, cSources);
  // Set that this is a LUA managed class
  soNew->SourceSetClass(true);
  // Return the class
  return soNew;
}
/* == Return a free source ================================================= */
static Source *SourceGet()
{ // Try to get an idle source and return it if possible
  if(Source*const soNew = SourceGetFree()) return soNew;
  // Else return a brand new source
  return SourceCanMakeNew() ? new Source : nullptr;
}
/* == SourceAlloc ========================================================== */
static bool SourceAlloc(const size_t stCount)
{ // Get the value we can actually use and the number of sources currently
  // allocated. Return if no new static sources are needed to be created.
  if(stCount >= cOal->OalGetMaxMonoSources()) return false;
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
  return BoolToCVarReturn(cOal->OalIsNotInitialised() || SourceAlloc(stCount));
}
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
