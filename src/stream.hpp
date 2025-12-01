/* == STREAM.HPP =========================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## This file handles streaming from .OGG files and playing to OpenAL.  ## **
** ## Note on OggVorbis thread safety: Documentation states that all ov_* ## **
** ## operations on the _SAME_ 'OggVorbis_File' struct must be serialised ## **
** ## and protected with mutexes if they are to be used in multiple       ## **
** ## threads and in our case, we do as the audio thread must manage the  ## **
** ## stream and the engine thread must be able to control it!            ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IStream {                    // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IAsset::P;             using namespace IASync::P;
using namespace ICodecOGG::P;          using namespace ICollector::P;
using namespace ICVarDef::P;           using namespace IError::P;
using namespace IEvtMain::P;           using namespace IFileMap::P;
using namespace IIdent::P;             using namespace ILog::P;
using namespace ILockable::P;          using namespace ILuaEvt::P;
using namespace ILuaIdent::P;          using namespace ILuaLib::P;
using namespace ILuaUtil::P;           using namespace IMemory::P;
using namespace IMutex::P;             using namespace IOal::P;
using namespace ISource::P;            using namespace IStd::P;
using namespace IString::P;            using namespace ISysUtil::P;
using namespace IUtil::P;              using namespace Lib::Ogg;
using namespace Lib::OpenAL::Types;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* ------------------------------------------------------------------------- */
enum StreamEvents : unsigned int { SE_PLAY, SE_STOP }; // Playback events
/* ------------------------------------------------------------------------- */
enum StreamPlayState : unsigned int    // Current playback state
{ /* ----------------------------------------------------------------------- */
  PS_STANDBY,                          // [0] Is not playing?
  PS_PLAYING,                          // [1] Is playing?
  PS_FINISHING,                        // [2] Was stopping? (no more data)
  PS_WASPLAYING,                       // [3] Was playing? (audio reinit)
  /* ----------------------------------------------------------------------- */
  PS_MAX                               // [4] Maximum number of states
};/* ----------------------------------------------------------------------- */
typedef IdList<PS_MAX> PSList;         // Play state strings
/* ------------------------------------------------------------------------- */
enum StreamStopReason : unsigned int   // Reason playback stopped
{ /* ----------------------------------------------------------------------- */
  SR_STOPNOUNQ,                        // [0] Successful stop with no unqueue
  SR_STOPUNQ,                          // [1] Successful stop with unqueue
  SR_REBUFFAIL,                        // [2] Rebuffer failed
  SR_RWREBUFFAIL,                      // [3] Rewind/Rebuffer failed
  SR_GENBUFFAIL,                       // [4] Generate source and buffer failed
  SR_STOPALL,                          // [5] Stopping all buffers (reset/quit)
  SR_LUA,                              // [6] Requested by Lua (guest).
  /* ----------------------------------------------------------------------- */
  SR_MAX                               // [7] Maximum number of stop reasons
};/* ----------------------------------------------------------------------- */
typedef IdList<SR_MAX> SRList;         // Stop reason strings
/* -- Stream collector class for collector data and custom variables ------- */
CTOR_BEGIN_ASYNC(Streams, Stream, CLHelperSafe,
/* -- Public variables ----------------------------------------------------- */
const SRList       srStrings;          // Stop reason strings
const PSList       psStrings;          // Play state strings
size_t             stBufCount;         // Buffer count
size_t             stBufSize;,,        // Size of each buffer
/* -- Derived classes ------------------------------------------------------ */
private LuaEvtMaster<Stream,LuaEvtTypeParam<Stream>>); // Lua event handler
/* ========================================================================= */
CTOR_MEM_BEGIN_ASYNC_CSLAVE(Streams, Stream, ICHelperUnsafe),
  /* -- Base classes ------------------------------------------------------- */
  public Ident,                        // Stream file name
  public AsyncLoaderStream,            // Asynchronous loading of Streams
  public LuaEvtSlave<Stream>,          // Lua event system for Stream
  public Lockable,                     // Lua garbage collector instruction
  protected MutexLock                  // Mutex helper object
{ /* -- Variables ---------------------------------------------------------- */
  FileMap          fmFile;             // FileMap class
  OggVorbis_File   ovfContext;         // Ogg vorbis file context
  vorbis_info      viData;             // Vorbis information structure
  ALUIntVector     vBuffers,           // Stream buffers space
                   vUnQBuffers;        // Unqueued buffers space
  Source          *sCptr;              // A free source to stream to
  ALenum           eFormat;            // Internal format
  ogg_int64_t      qLivePos,           // Live playback position (external)
                   qDecPos,            // Decoder playback position (internal)
                   qLoopBegin,         // Loop start position
                   qLoopEnd,           // Loop end position
                   qLoop;              // Loop counter
  StreamPlayState  psState;            // Play state
  ALfloat          fVolume;            // Saved volume
  StrNCStrMap      ssMetaData;         // Metadata strings
  /* -- Updates the PCM position ------------------------------------------- */
  void UpdatePosition() { qDecPos = qLivePos = ov_pcm_tell(&ovfContext); }
  /* -- Get time elapsed --------------------------------------------------- */
  ALdouble GetElapsed() { return ov_time_tell(&ovfContext); }
  /* -- Set time elapsed --------------------------------------------------- */
  void SetElapsed(const ALdouble dElapsed)
    { ov_time_seek(&ovfContext, dElapsed); UpdatePosition(); }
  /* -- Set time elapsed faster -------------------------------------------- */
  void SetElapsedFast(const ALdouble dElapsed)
    { ov_time_seek_page(&ovfContext, dElapsed); UpdatePosition(); }
  /* -- Get total time ----------------------------------------------------- */
  ALdouble GetDuration() { return ov_time_total(&ovfContext, -1); }
  /* -- Get PCM bytes duration --------------------------------------------- */
  ogg_int64_t GetPosition() const { return qLivePos; }
  /* -- Set PCM byte position ---------------------------------------------- */
  void SetPosition(const ogg_int64_t qNewPos)
    { ov_pcm_seek(&ovfContext, qNewPos); UpdatePosition(); }
  /* -- Set PCM byte position fast ----------------------------------------- */
  void SetPositionFast(const ogg_int64_t qPosition)
    { ov_pcm_seek_page(&ovfContext, qPosition); UpdatePosition(); }
  /* -- Get total PCM samples ---------------------------------------------- */
  ogg_int64_t GetSamples() { return ov_pcm_total(&ovfContext, -1); }
  /* -- Convert stop reason to string -------------------------------------- */
  const string_view &StopReasonToString(const StreamStopReason srReason) const
    { return cParent->srStrings.Get(srReason); }
  /* -- Convert stop reason to string -------------------------------------- */
  const string_view &StateReasonToString(const StreamPlayState psReason) const
    { return cParent->psStrings.Get(psReason); }
  /* -- Stop (without locks) ----------------------------------------------- */
  void Stop(const StreamStopReason srReason)
  { // Don't have source class? There is nothing else to do!
    if(!sCptr) return;
    // Stop source from playing
    sCptr->Stop();
    // Go back to unplayed position
    SetPosition(qLivePos);
    // Unlock the source so it can be used by other samples and streams
    UnloadSource();
    // Write debug reason for stoppage
    cLog->LogDebugExSafe("Stream stopped '$'! (R:$<$>;L:$<$>).", IdentGet(),
      StopReasonToString(srReason), srReason, StateReasonToString(psState),
      psState);
    // What was the state before?
    switch(psState)
    { // Anything else? Send playback event and set internal state to stopped
      default: LuaEvtDispatch(SE_STOP, psState, srReason);
               psState = PS_STANDBY;
      // Return if forced to stop or already in standby
      case PS_WASPLAYING: case PS_STANDBY: break;
    }
  }
  /* -- Play (without locks) ----------------------------------------------- */
  void Play()
  { // If we already have a source? Stop and unqueue all buffers
    if(sCptr) sCptr->StopAndUnQueueAllBuffers();
    // Grab and lock a new free source if need be return if we can't
    else if(!LockSource()) return;
    // Update volume
    UpdateVolume();
    // If we didn't rebuffer anything then no point playing
    if(!FullRebuffer()) return;
    // Play the buffers
    sCptr->Play();
    // Send playback event if was not already playing
    LuaEvtDispatch(SE_PLAY, psState);
    // Set internal state to playing
    psState = PS_PLAYING;
  }
  /* -- Decompression routine (VORBIS->PCM) -------------------------------- */
  bool Rebuffer(const ALuint uiBufferId)
  { // Reseek and rebuffer if looping
    if(qDecPos >= qLoopEnd) return false;
    // Bytes written to buffer and bytes per channel
    size_t stBSize = 0, stBpc;
    // Number of channels as size_t
    const size_t stChannels = static_cast<size_t>(GetChannels());
    // Decode the buffer and grab the size. If nothing was decoded then reloop.
    if(cOal->Have32FPPB())
    { // Four bytes per channel (float)
      stBpc = sizeof(ALfloat);
      // Loop...
      do
      { // Integer data buffer
        ALfloat **fpPCM;
        // Read buffer
        if(const long lResult = ov_read_float(&ovfContext, &fpPCM,
          static_cast<int>((MemSize() - stBSize) / stChannels /
            sizeof(ALfloat)), nullptr))
        { // Error?
          if(lResult < 0)
            XC("Failed to decode ogg stream to float pcm!",
               "Identifier", IdentGet(), "Result", lResult,
               "Reason",     cCodecOGG->GetOggErr(lResult));
          // Get size as size_t
          const size_t stBytes = static_cast<size_t>(lResult);
          // Converted to float buffer
          ALfloat*const fpPCMout = MemRead<ALfloat>(stBSize);
          // Process frames to buffer (iFI=FrameIndex / iCI=ChanIndex)
          cCodecOGG->F32FromVorbisFrames(fpPCM, stBytes, stChannels, fpPCMout);
          // Increase buffer
          stBSize += sizeof(ALfloat) * stBytes * stChannels;
        } // Break loop when no bytes read
        else break;
      } // ...until buffer is filled
      while(stBSize < MemSize());
    }
    else
    { // Two bytes per channel (short)
      stBpc = sizeof(ALshort);
      // Loop...
      do
      { // Read buffer
        if(const long lResult = ov_read(&ovfContext, MemRead(stBSize),
          static_cast<int>(MemSize() - stBSize), 0, sizeof(ALshort), 1,
            nullptr))
        { // Check result
          if(lResult < 0)
            XC("Failed to decode ogg stream to integer pcm!",
               "Identifier", IdentGet(), "Result", lResult,
               "Reason",     cCodecOGG->GetOggErr(lResult));
          // Add bytes read
          stBSize += static_cast<size_t>(lResult);
        } // Break loop when no bytes read
        else break;
      } // ...until buffer is filled
      while(stBSize < MemSize());
    } // Calculate pcm samples read in 32-bit floats
    const ogg_int64_t qS = static_cast<ogg_int64_t>(stBSize) /
                           static_cast<ogg_int64_t>(stBpc) /
                           static_cast<ogg_int64_t>(stChannels),
      // Get new position we want to ideally move to next
      qN = qDecPos + qS;
    // We can play the next part of the audio so set the new position
    if(qN <= qLoopEnd) qDecPos = qN;
    // We cannot play the next part of the audio due to loop end position
    else
    { // Restrict number of samples to play, but don't go over the buffer size
      stBSize = UtilMinimum(MemSize(),
        static_cast<size_t>(qLoopEnd - qDecPos) * stBpc * stChannels);
      // Push forward
      qDecPos += stBSize;
    } // Return failure if no bytes were buffered
    if(!stBSize) return false;
    // Buffer the PCM data if we have some
    AL(cOal->BufferData(uiBufferId, GetFormat(), MemPtr<ALvoid>(),
      static_cast<ALsizei>(stBSize), static_cast<ALsizei>(GetRate())),
      "Failed to buffer ogg stream data!",
      "Identifier", IdentGet(),  "BufferId",   uiBufferId,
      "Format",     GetFormat(), "BufferData", MemPtr<void>(),
      "BufferSize", stBSize,     "Rate",       GetRate());
    // Return status
    return true;
  }
  /* -- Unload buffers ----------------------------------------------------- */
  void UnloadBuffers()
  { // If buffers allocated
    if(vBuffers.empty()) return;
    // Unload the source, delete and free the buffers
    UnloadSource();
    // Check for error and log it
    ALL(cOal->DeleteBuffers(vBuffers),
      "Stream '$' failed to delete $ buffers!", IdentGet(), vBuffers.size());
  }
  /* -- Unload source ------------------------------------------------------ */
  void UnloadSource()
  { // If source is not available, bail
    if(!sCptr) return;
    // Save current state. Use our internal state to decide if we should replay
    // the source as if the context was lost due to hardware changes. IsPlaying
    // will report as AL_STOPPED and any music that was playing, will not
    // resume when the audio is reinitialised.
    if(psState == PS_PLAYING && IsPlaying()) psState = PS_WASPLAYING;
    // Get reference to source, stop it, unqueue it
    sCptr->StopAndUnQueueAllBuffers();
    // Allow the source manager to recycle this source
    sCptr->Unlock();
    sCptr = nullptr;
  }
  /* -- Load and lock source ----------------------------------------------- */
  void GenerateBuffers()
  { // Ignore if we already have buffers
    if(vBuffers.empty())
      XC("Empty sources list!",
         "Identifier", IdentGet(), "BufferCount", vBuffers.size());
    // Generate OpenAL buffers
    AL(cOal->CreateBuffers(vBuffers),
      "Failed to generate buffers for stream!",
      "Identifier", IdentGet(), "Count", vBuffers.size());
    // Generate space for queued buffers
    vUnQBuffers.resize(vBuffers.size());
  }
  /* -- Lock source buffer ------------------------------------------------- */
  bool LockSource()
  { // We already have a source locked or we can get a new source? Ignore
    if(sCptr) return true;
    sCptr = GetSource();
    if(sCptr) return true;
    // Tell log
    cLog->LogWarningExSafe("Stream out of sources locking '$'!", IdentGet());
    // Failed
    return false;
  }
  /* -- Get/Set loop ------------------------------------------------------- */
  void SetLoopBegin(const ogg_int64_t qNewPos) { qLoopBegin = qNewPos; }
  void SetLoopEnd(const ogg_int64_t qNewPos) { qLoopEnd = qNewPos; }
  void SetLoopRange(const ogg_int64_t qNBPos, const ogg_int64_t qNEPos)
    { SetLoopBegin(qNBPos); SetLoopEnd(qNEPos); }
  void SetLoop(const ogg_int64_t qLoopCount)
  { // Set the loop
    qLoop = qLoopCount;
    // Set loop to the end if we're to finish
    if(!qLoop) SetLoopEnd(GetSamples());
  }
  /* -- Get functions with safe versions---------------------------- */ public:
  bool IsPlaying() const
    { return sCptr && sCptr->GetState() == AL_PLAYING; }
  ALfloat GetVolume() const { return fVolume; }
  /* -- Get info (safe functions) ------------------------------------------ */
  long GetRate() const { return viData.rate; }
  int GetChannels() const { return viData.channels; }
  int GetVersion() const { return viData.version; }
  long GetBitRateUpper() const { return viData.bitrate_upper; }
  long GetBitRateNominal() const { return viData.bitrate_nominal; }
  long GetBitRateLower() const { return viData.bitrate_lower; }
  long GetBitRateWindow() const { return viData.bitrate_window; }
  /* -- Get loop (unsafe functions) ---------------------------------------- */
  ogg_int64_t GetLoop() const { return qLoop; }
  ogg_int64_t GetLoopBegin() const { return qLoopBegin; }
  ogg_int64_t GetLoopEnd() const { return qLoopEnd; }
  /* -- Seek and tell functions (with locks) ------------------------------- */
  void SetLoopSafe(const ogg_int64_t qLoopCount)
    { MutexCall([this, qLoopCount](){ SetLoop(qLoopCount); }); }
  ogg_int64_t GetLoopSafe()
    { return MutexCall([this](){ return GetLoop(); }); }
  ogg_int64_t GetLoopBeginSafe()
    { return MutexCall([this](){ return GetLoopBegin(); }); }
  ogg_int64_t GetLoopEndSafe()
    { return MutexCall([this](){ return GetLoopEnd(); }); }
  void SetLoopBeginSafe(const ogg_int64_t qNewPos)
    { MutexCall([this, qNewPos](){ SetLoopBegin(qNewPos); }); }
  void SetLoopEndSafe(const ogg_int64_t qNewPos)
    { MutexCall([this, qNewPos](){ SetLoopEnd(qNewPos); }); }
  void SetLoopRangeSafe(const ogg_int64_t qNBPos, const ogg_int64_t qNEPos)
    { MutexCall([this, qNBPos, qNEPos](){ SetLoopRange(qNBPos, qNEPos); }); }
  ALdouble GetElapsedSafe()
    { return MutexCall([this](){ return GetElapsed(); }); }
  void SetElapsedSafe(const ALdouble dElapsed)
    { MutexCall([this, dElapsed](){ SetElapsed(dElapsed); }); }
  void SetElapsedFastSafe(const ALdouble dElapsed)
    { MutexCall([this, dElapsed](){ SetElapsedFast(dElapsed); }); }
  ALdouble GetDurationSafe()
    { return MutexCall([this](){ return GetDuration(); }); }
  ogg_int64_t GetPositionSafe()
    { return MutexCall([this](){ return GetPosition(); }); }
  void SetPositionSafe(const ogg_int64_t qNewPos)
    { MutexCall([this, qNewPos](){ SetPosition(qNewPos); }); }
  void SetPositionFastSafe(const ogg_int64_t qPosition)
    { MutexCall([this, qPosition](){ SetPositionFast(qPosition); }); }
  ogg_int64_t GetSamplesSafe()
    { return MutexCall([this](){ return GetSamples(); }); }
  /* ----------------------------------------------------------------------- */
  ogg_int64_t GetOggBytes() const { return fmFile.MemSize<ogg_int64_t>(); }
  /* -- GetFormat ---------------------------------------------------------- */
  ALenum GetFormat() const { return eFormat; }
  const string_view GetFormatName() const
    { return cOal->GetALFormat(eFormat); }
  /* -- Main (from audio thread) ------------------------------------------- */
  void Main()
  { // Wait for audio thread and lock access to stream buffers
    MutexCall([this](){
      // Compare state
      switch(psState)
      { // Don't care if on stand by
        case PS_STANDBY: break;
        // Is finishing playing?
        case PS_FINISHING:
        { // Ignore if there is no source
          if(!sCptr) { psState = PS_STANDBY; return; }
          // Return and stop if no buffers are queued
          if(!sCptr->GetBuffersQueued()) return Stop(SR_STOPNOUNQ);
          // Unqueue all the buffers
          sCptr->UnQueueAllBuffers();
          // Return if theres still buffers queued
          if(sCptr->GetBuffersQueued()) return;
          // Full stop
          return Stop(SR_STOPUNQ);
        } // Is playing?
        case PS_PLAYING:
        { // Ignore if there is no source
          if(!sCptr) { psState = PS_STANDBY; return; }
          // Stopped playing and should be playing? Start playing again.
          if(!IsPlaying())
          { // Unqueue all buffers
            sCptr->UnQueueAllBuffers();
            // Do a full rebuffer of those buffers and if we can't rebuffer
            if(!FullRebuffer())
            { // Full stop!
              Stop(SR_REBUFFAIL);
              // Log problem
              cLog->LogWarningExSafe(
                "Stream '$' was stopped and was unable to be rebuffered!",
                fmFile.IdentGet());
              // Done
              return;
            } // Log problem
            cLog->LogWarningExSafe(
              "Stream '$' stopped unexpectedly and is being replayed!",
              fmFile.IdentGet());
            // Replay the buffers
            sCptr->Play();
            // Done
            return;
          } // Until all the buffers have finished processing
          if(const ALsizei stBuffersProcessed = sCptr->GetBuffersProcessed())
          { // Unqueue the buffers and enumerate through each unqueued buffer
            sCptr->UnQueueBuffers(vUnQBuffers.data(), stBuffersProcessed);
            for(ALsizei stIndex = 0; stIndex < stBuffersProcessed; ++stIndex)
            { // Get buffer index
              const ALuint uiBuffer =
                vUnQBuffers[static_cast<size_t>(stIndex)];
              // Progress live position. This is so if the guest is saving the
              // position to replay at a later time, this position will be on
              // or slightly before the decoder position instead of at the
              // decoder position which will be way after the live playback
              // position.
              qLivePos += cOal->GetBufferInt<ogg_int64_t>(uiBuffer, AL_SIZE) /
                // This could be 16 (if no AL_EXT_FLOAT32) or 32.
                (cOal->GetBufferInt<ogg_int64_t>(uiBuffer, AL_BITS)/8) /
                // This should always be 1 or 2.
                (cOal->GetBufferInt<ogg_int64_t>(uiBuffer, AL_CHANNELS));
              // Try to rebuffer data to it and if failed?
              if(Rebuffer(uiBuffer)) continue;
              // Run out of loops? Finish playing and stop
              if(!qLoop)
              { // Finish playing, reset position and break
                psState = PS_FINISHING;
                SetPosition(0);
                break;
              } // Seek to start and try rebuffering again and if failed still?
              SetPosition(qLoopBegin);
              if(!Rebuffer(uiBuffer))
              { // Stop playing, reset position and break
                Stop(SR_RWREBUFFAIL);
                SetPosition(0);
                break;
              } // If not looping forever? Reduce count and if zero play to end
              if(qLoop != -1 && !--qLoop) SetLoopEnd(GetSamples());
            } // Requeue the buffers
            sCptr->QueueBuffers(vUnQBuffers.data(), stBuffersProcessed);
            // Done
            break;
          }
        } // Other state (ignore)
        default: break;
      }
    });
  }
  /* -- Load source and buffers during a reinit ---------------------------- */
  void GenerateSourceAndBuffers()
  { // Protect modifications from audio main thread
    MutexCall([this](){
      // Generate buffer id's
      GenerateBuffers();
      // Ignore and set to standby if wasn't playing before
      if(psState != PS_WASPLAYING) { psState = PS_STANDBY; return; }
      // Lock the source and return with message if we can't
      if(!LockSource())
      { // Log problem
        cLog->LogWarningExSafe(
          "Stream '$' could not be allocated a source after reset!",
          fmFile.IdentGet());
        // Set internal state to standby
        psState = PS_STANDBY;
        // Done
        return;
      } // If we didn't rebuffer anything then no point playing
      if(!FullRebuffer())
      { // Log problem
        cLog->LogWarningExSafe(
          "Stream '$' could not be rebuffered after reset!",
            fmFile.IdentGet());
        // Set fully stopped and return
        return Stop(SR_GENBUFFAIL);
      } // Update volume
      UpdateVolume();
      // Play the buffers
      sCptr->Play();
      // Set internal state to playing
      psState = PS_PLAYING;
    });
  }
  /* -- Unload source and buffers ------------------------------------------ */
  void UnloadSourceAndBuffers() { UnloadSource(); UnloadBuffers(); }
  /* -- Update volume ------------------------------------------------------ */
  void UpdateVolume()
  {  // Ignore if no source
    if(!sCptr) return;
    // Set volume
    sCptr->SetGain(fVolume * cSources->fMVolume * cSources->fGVolume);
  }
  /* -- Update and apply volume -------------------------------------------- */
  void SetVolume(const ALfloat fNewVolume)
    { fVolume = fNewVolume; UpdateVolume(); }
  /* -- Fully rebuffer stream data ----------------------------------------- */
  bool FullRebuffer()
  { // Start from buffer at index 0
    size_t stIndex = 0;
    // Until we run out of buffers allocated
    while(stIndex < vBuffers.size())
    { // If we could not rebuffer then we maybe at the end of file
      if(!Rebuffer(vBuffers[stIndex]))
      { // Try to buffer from the loop start
        SetPosition(qLoopBegin);
        // Try to rebuffer again and if failed then throw an error
        if(!Rebuffer(vBuffers[stIndex]))
          XC("Full rebuffer from loop start failed!",
            "Identifier", fmFile.IdentGet(),
            "LoopBegin", static_cast<uint64_t>(qLoopBegin));
      } // Increment buffer index
      ++stIndex;
    } // We can't play anything if we couldn't decode to one buffer
    if(!stIndex) return false;
    // Queue the buffers and play them
    sCptr->QueueBuffers(vBuffers.data(), static_cast<ALsizei>(stIndex));
    // We buffered something
    return true;
  }
  /* -- Play with lock ----------------------------------------------------- */
  void PlaySafe() { MutexCall([this](){ Play(); }); }
  /* -- Stop with lock ----------------------------------------------------- */
  void StopSafe(const StreamStopReason srReason)
    { MutexCall([this, srReason](){ Stop(srReason); }); }
  /* -- Load from memory --------------------------------------------------- */
  void AsyncReady(FileMap &fmData)
  { // Set file class
    fmFile.FileMapSwap(fmData);
    // Initialise context and test for error
    if(const int iResult = ov_open_callbacks(&fmFile, &ovfContext, nullptr, 0,
         cCodecOGG->GetCallbacks()))
      XC("Init OGG decoder context failed!",
        "Identifier", IdentGet(), "Code", iResult,
        "Reason",     cCodecOGG->GetOggErr(iResult));
    // We don't need to create more buffers than we need. If we don't do this
    // then Rebuffer() will not fill all the buffers and subsequent OpenAL
    // calls will fail. We'll add a minimum value of 1 too just incase we get
    // a stream with no data.
    vBuffers.resize(UtilClamp(static_cast<size_t>(ceil(static_cast<ALdouble>
      (GetSamples()) / cParent->stBufSize)), 1, cParent->stBufCount));
    // Get info about ogg and copy it into our static buffer if succeeded,
    // else show an exception if failed. This removes dereferencing of the
    // vorbis info struct.
    if(const vorbis_info*const viPtr = ov_info(&ovfContext, -1))
      memcpy(&viData, viPtr, sizeof(viData));
    else XC("Failed to get vorbis info!", "Identifier", IdentGet());
    // Only 1-2 channels supported
    if(GetChannels() < 1 || GetChannels() > 2)
      XC("Unsupported channel count!",
        "Identifier", IdentGet(), "Channels", GetChannels());
    // Compare number of channels in file to set appropriate format
    eFormat = GetChannels() == 1 ?
      (cOal->Have32FPPB() ? AL_FORMAT_MONO_FLOAT32 : AL_FORMAT_MONO16) :
      (cOal->Have32FPPB() ? AL_FORMAT_STEREO_FLOAT32 : AL_FORMAT_STEREO16);
    // Allocate the buffer with size from global setting. We can re-use the
    // 'Memory' class from the 'AsyncLoader' class.
    MemInitBlank(cParent->stBufSize);
    // Set default loop position to the end
    SetLoopRange(0, GetSamples());
    // Parse vorbis comments and if we got them?
    if(vorbis_comment*const vcStrings = ov_comment(&ovfContext, -1))
    { // Parse the comments and then free the strings
      ssMetaData = StdMove(cCodecOGG->
        VorbisParseComments(vcStrings->user_comments, vcStrings->comments));
      vorbis_comment_clear(vcStrings);
      // Write vorbis comments to log if debug mode set
      if(cLog->HasLevel(LH_DEBUG))
        for(const StrNCStrMapPair &sncsmpPair : ssMetaData)
          cLog->LogNLCDebugExSafe("- Vorbis comment: $ -> $.",
            sncsmpPair.first, sncsmpPair.second);
    } // Generate buffers, recommending this amount
    GenerateBuffers();
    // Log ogg loaded
    cLog->LogInfoExSafe(
      "Stream loaded '$' (C=$;R=$;BR=$:$:$:$;D$=$;B=$;BS=$;V=$$).",
      IdentGet(), GetChannels(), GetRate(), viData.bitrate_upper,
      viData.bitrate_nominal, viData.bitrate_lower, viData.bitrate_window,
      fixed, GetDuration(), vBuffers.size(), MemSize(), hex, GetVersion());
  }
  /* -- Return metadata as table ------------------------------------------- */
  const StrNCStrMap &GetMetaData() const { return ssMetaData; }
  /* -- Constructor -------------------------------------------------------- */
  Stream() :
    /* -- Initialisers ----------------------------------------------------- */
    ICHelperStream{ cStreams },        // Initialise collector unregistered
    IdentCSlave{ cParent->CtrNext() }, // Initialise identification number
    AsyncLoaderStream{ *this, this,    // Initialise async loader
      EMC_MP_STREAM },                 //   with our streaming event
    LuaEvtSlave{ this,                 // Initialise stream event manager
      EMC_STR_EVENT },                 //   with our stremaing event
    ovfContext{},                      // No file opened yet
    viData{},                          // No vorbis information yet
    sCptr(nullptr),                    // No associated OAL source yet
    eFormat(AL_NONE),                  // No OAL format id yet
    qLivePos(0), qDecPos(0),           // No ext|internal position
    qLoopBegin(0), qLoopEnd(0),        // No loop start/end position
    qLoop(0),                          // Do not loop
    psState(PS_STANDBY),               // Current state to standby
    fVolume(1.0f)                      // No volume yet
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- Destructor --------------------------------------------------------- */
  ~Stream()
  { // Stop any pending async operations
    AsyncCancel();
    // Remove the registration now so it is no longer polled
    ICHelperStream::CollectorUnregister();
    // Ignore if file data not initialised
    if(fmFile.FileMapClosed()) return;
    // Synchronise from audio thread and sources management
    MutexScopedCall([this](){
      // Unload source and buffers
      UnloadSourceAndBuffers();
      // If stream opened? Clear ogg state
      if(ovfContext.datasource) ov_clear(&ovfContext);
    }, cParent->MutexGet());
    // Log that the stream was unloaded
    cLog->LogDebugExSafe("Stream unloaded '$'!", IdentGet());
  }
};/* -- End ---------------------------------------------------------------- */
CTOR_END_ASYNC_NOFUNCS(Streams, Stream, STREAM, STREAM,
  /* -- Initialisers ------------------------------------------------------- */
  LuaEvtMaster{ EMC_STR_EVENT },       // Initialise streaming event master
  srStrings{{                          // Initialise stop reason strings
    "SR_STOPNOUNQ",                    // [0] Successful stop with no unqueue
    "SR_STOPUNQ",                      // [1] Successful stop with unqueue
    "SR_REBUFFAIL",                    // [2] Rebuffer failed
    "SR_RWBUFFAIL",                    // [3] Rewind/Rebuffer failed
    "SR_GENBUFFAIL",                   // [4] Generate source and buffer failed
    "SR_STOPALL",                      // [5] Stopping all buffers (reset/quit)
    "SR_LUA",                          // [6] Requested by Lua (guest).
  }},                                  // Stop reason strings initialised
  psStrings{{                          // Initialise play state strings
    "PS_STANDBY",                      // [0] Is not playing
    "PS_PLAYING",                      // [1] Is playing
    "PS_FINISHING",                    // [2] Was stopping (no more data)
    "PS_WASPLAYING",                   // [3] Was playing (audio reset)
  }},                                  // Play state strings initialised
  stBufCount(0),                       // No buffers count yet
  stBufSize(0)                         // No buffer size yet
) /* == Manage streams for audio thread ==================================== */
static void StreamManage()
{ // Lock access to streams collector list and process logic for each stream
  cStreams->MutexCall([](){
    for(Stream*const sPtr : *cStreams) sPtr->Main();
  });
}
/* == Unload all source and buffers ======================================== */
static void StreamDeInit()
{ // Lock access to streams collector list and uninitialise all buffers
  cStreams->MutexCall([](){
    for(Stream*const sPtr : *cStreams) sPtr->UnloadSourceAndBuffers();
  });
}
/* == Clear event callbacks on all streams ================================= */
static void StreamClearEvents()
{ // Lock access to streams collector list and uninitialise all stream events
  cStreams->MutexCall([](){
    for(Stream*const sPtr : *cStreams) sPtr->LuaEvtDeInit();
  });
}
/* == Generate all source and buffers ====================================== */
static void StreamReInit()
{ // Lock access to bitmap collector list and uninitialise source/buffers
  cStreams->MutexCall([](){
    for(Stream*const sPtr : *cStreams) sPtr->GenerateSourceAndBuffers();
  });
}
/* == Stop all streams ===================================================== */
static void StreamStop()
{ // Lock access to bitmap collector list and stop all streams
  cStreams->MutexCall([](){
    for(Stream*const sPtr : *cStreams) sPtr->StopSafe(SR_STOPALL);
  });
}
/* == Update all streams base volume======================================== */
static void StreamCommitVolume()
{ // Lock access to bitmap collector list and update all stream volumes
  cStreams->MutexCall([](){
    for(Stream*const sPtr : *cStreams) sPtr->UpdateVolume();
  });
}
/* == Set number of buffers to allocate per stream ========================= */
static CVarReturn StreamSetBufferCount(const size_t stNewCount)
  { return CVarSimpleSetIntNLG(cStreams->stBufCount, stNewCount, 2UL, 16UL); }
/* == Set all streams base volume ========================================== */
static CVarReturn StreamSetVolume(const ALfloat fNewVolume)
{ // Ignore if invalid value
  if(fNewVolume < 0.0f || fNewVolume > 1.0f) return DENY;
  // Store volume (SOURCES class keeps it)
  cSources->fMVolume = fNewVolume;
  // Update volumes on all streams
  StreamCommitVolume();
  // Success
  return ACCEPT;
}
/* -- Set memory allocated per buffer -------------------------------------- */
static CVarReturn StreamSetBufferSize(const size_t stNewSize)
  { return CVarSimpleSetIntNLG(cStreams->stBufSize, stNewSize,
      4096UL, 65536UL); }
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
