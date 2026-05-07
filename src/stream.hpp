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
using namespace ILog::P;               using namespace ILookupArray::P;
using namespace ILockable::P;          using namespace ILuaEvt::P;
using namespace ILuaIdent::P;          using namespace ILuaLib::P;
using namespace ILuaUtil::P;           using namespace IMemory::P;
using namespace IMixer::P;             using namespace IMutex::P;
using namespace IName::P;              using namespace IOal::P;
using namespace ISerial::P;            using namespace ISource::P;
using namespace IStd::P;               using namespace IString::P;
using namespace ISysUtil::P;           using namespace IUtil::P;
using namespace Lib::Ogg;              using namespace Lib::OpenAL::Types;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* ------------------------------------------------------------------------- */
enum StreamEvents : unsigned { SE_PLAY, SE_STOP }; // Playback events
/* ------------------------------------------------------------------------- */
enum StreamPlayState : unsigned        // Current playback state
{ /* ----------------------------------------------------------------------- */
  PS_STANDBY,                          // [0] Is not playing?
  PS_PLAYING,                          // [1] Is playing?
  PS_FINISHING,                        // [2] Was stopping? (no more data)
  PS_WASPLAYING,                       // [3] Was playing? (audio reinit)
  /* ----------------------------------------------------------------------- */
  PS_MAX                               // [4] Maximum number of states
};/* ----------------------------------------------------------------------- */
using PSList = LookupArray<PS_MAX>;    // Play state strings
/* ------------------------------------------------------------------------- */
enum StreamStopReason : unsigned       // Reason playback stopped
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
using SRList = LookupArray<SR_MAX>;    // Stop reason strings
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
CTOR_MEM_BEGIN_ASYNC_CSLAVE(Streams, Stream, ICHelperSafe),
  /* -- Base classes ------------------------------------------------------- */
  public AsyncLoaderStream,            // Asynchronous loading of Streams
  public LuaEvtSlave<Stream>,          // Lua event system for Stream
  public Lockable,                     // Lua garbage collector instruction
  protected MutexLock                  // Mutex helper object
{ /* -- Variables ---------------------------------------------------------- */
  FileMap          fmFile;             // FileMap class
  OggVorbis_File   ovfContext;         // Ogg vorbis file context
  vorbis_info      viData;             // Vorbis information structure
  ALUIntVector     aluvBuffers,        // Stream buffers space
                   aluvUnQBuffers,     // Un-queued buffers space
                   aluvReQBuffers;     // Re-queued buffers space
  Source          *sCptr;              // A free source to stream to
  ALenum           aleFormat;          // Internal format
  ogg_int64_t      llLivePos,          // Live playback position (external)
                   llDecPos,           // Decoder playback position (internal)
                   llLoopBegin,        // Loop start position
                   llLoopEnd,          // Loop end position
                   llLoop;             // Loop counter
  StreamPlayState  spsState;           // Play state
  ALfloat          alfVolume;          // Saved volume
  StrNCStrMap      ssMetaData;         // Metadata strings
  /* -- Updates the PCM position ------------------------------------------- */
  void UpdatePosition() { llDecPos = llLivePos = ov_pcm_tell(&ovfContext); }
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
  ogg_int64_t GetPosition() const { return llLivePos; }
  /* -- Set PCM byte position ---------------------------------------------- */
  void SetPosition(const ogg_int64_t llNewPos)
    { ov_pcm_seek(&ovfContext, llNewPos); UpdatePosition(); }
  /* -- Set PCM byte position fast ----------------------------------------- */
  void SetPositionFast(const ogg_int64_t llPosition)
    { ov_pcm_seek_page(&ovfContext, llPosition); UpdatePosition(); }
  /* -- Get total PCM samples ---------------------------------------------- */
  ogg_int64_t GetSamples() { return ov_pcm_total(&ovfContext, -1); }
  /* -- Convert stop reason to string -------------------------------------- */
  const StdStringView
    &StopReasonToString(const StreamStopReason ssrReason) const
  { return cParent->srStrings.Get(ssrReason); }
  /* -- Convert stop reason to string -------------------------------------- */
  const StdStringView
    &StateReasonToString(const StreamPlayState psReason) const
  { return cParent->psStrings.Get(psReason); }
  /* -- Stop (without locks) ----------------------------------------------- */
  void Stop(const StreamStopReason ssrReason)
  { // Don't have source class? There is nothing else to do!
    if(!sCptr) return;
    // Stop source from playing and unload it
    sCptr->Stop();
    UnloadSource();
    // Go back to unplayed position
    SetPosition(llLivePos);
    // Write debug reason for stoppage
    cLog->LogDebugExSafe("Stream stopped '$'! (R:$<$>;L:$<$>).", NameGet(),
      StopReasonToString(ssrReason), ssrReason, StateReasonToString(spsState),
      spsState);
    // What was the state before?
    switch(spsState)
    { // Anything else? Send playback event and set internal state to stopped
      default: LuaEvtDispatch(SE_STOP, spsState, ssrReason);
               spsState = PS_STANDBY;
               [[fallthrough]];
      // Return if forced to stop or already in standby
      case PS_WASPLAYING: [[fallthrough]];
      case PS_STANDBY: break;
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
    LuaEvtDispatch(SE_PLAY, spsState);
    // Set internal state to playing
    spsState = PS_PLAYING;
  }
  /* -- Decompression routine (VORBIS->PCM) -------------------------------- */
  bool Rebuffer(const ALuint uBufferId)
  { // Reseek and rebuffer if looping
    if(llDecPos >= llLoopEnd) return false;
    // Bytes written to buffer and bytes per channel
    size_t stPos = 0, stBpc;
    // Number of channels as size_t
    const size_t stChannels = static_cast<size_t>(GetChannels());
    // Decode the buffer and grab the size. If nothing was decoded then reloop.
    if(cOal->Have32FPPB())
    { // Four bytes per channel (float)
      stBpc = sizeof(ALfloat);
      // Loop...
      do
      { // Read buffer
        ALfloat **alfpaPCM;
        if(const long lResult = ov_read_float(&ovfContext, &alfpaPCM,
          static_cast<int>((MemSize() - stPos) / stChannels /
            sizeof(ALfloat)), nullptr))
        { // Error?
          if(lResult < 0)
            XC("Failed to decode ogg stream to float pcm!",
              "Name",   NameGet(), "Result", lResult,
              "Reason", cCodecOGG->GetOggErr(lResult));
          // Process frames to buffer (iFI=FrameIndex / iCI=ChanIndex)
          const size_t stFrames = static_cast<size_t>(lResult);
          cCodecOGG->F32FromVorbisFrames(alfpaPCM, stFrames, stChannels,
            MemRead<ALfloat>(stPos));
          // Increase buffer
          stPos += sizeof(ALfloat) * stFrames * stChannels;
        } // Break loop when no bytes read
        else break;
      } // ...until buffer is filled
      while(stPos < MemSize());
    }
    else
    { // Two bytes per channel (short)
      stBpc = sizeof(ALshort);
      // Loop...
      do
      { // Read buffer
        if(const long lResult = ov_read(&ovfContext, MemRead(stPos),
          static_cast<int>(MemSize() - stPos), 0, sizeof(ALshort), 1, nullptr))
        { // Check result
          if(lResult < 0)
            XC("Failed to decode ogg stream to integer pcm!",
              "Name",   NameGet(), "Result", lResult,
              "Reason", cCodecOGG->GetOggErr(lResult));
          // Add bytes read
          stPos += static_cast<size_t>(lResult);
        } // Break loop when no bytes read
        else break;
      } // ...until buffer is filled
      while(stPos < MemSize());
    } // Calculate pcm samples read in 32-bit floats
    const ogg_int64_t llS = static_cast<ogg_int64_t>(stPos) /
                            static_cast<ogg_int64_t>(stBpc) /
                            static_cast<ogg_int64_t>(stChannels),
      // Get new position we want to ideally move to next
      llN = llDecPos + llS;
    // We can play the next part of the audio so set the new position
    if(llN < llLoopEnd) llDecPos = llN;
    // We cannot play the next part of the audio due to loop end position
    else
    { // Restrict number of samples to play, but don't go over the buffer size
      stPos = UtilMinimum(MemSize(),
        static_cast<size_t>(llLoopEnd - llDecPos) * stBpc * stChannels);
      // Push forward
      llDecPos += stPos;
    } // Return failure if no bytes were buffered
    if(!stPos) return false;
    // Buffer the PCM data if we have some
    AL(cOal->BufferData(uBufferId, GetFormat(), MemPtr<ALvoid>(),
      static_cast<ALsizei>(stPos), static_cast<ALsizei>(GetRate())),
      "Failed to buffer ogg stream data!",
      "Name",       NameGet(),   "BufferId",   uBufferId,
      "Format",     GetFormat(), "BufferData", MemPtr<void>(),
      "BufferSize", stPos,       "Rate",       GetRate());
    // Return status
    return true;
  }
  /* -- Unload buffers ----------------------------------------------------- */
  void UnloadBuffers()
  { // Unload the buffers if we have them
    if(!aluvBuffers.empty())
      ALL(cOal->DeleteBuffers(aluvBuffers),
        "Stream '$' failed to delete $ buffers!",
        NameGet(), aluvBuffers.size());
  }
  /* -- Unload source ------------------------------------------------------ */
  void UnloadSource()
  { // If source is not available, bail
    if(!sCptr) return;
    // Save current state. Use our internal state to decide if we should replay
    // the source as if the context was lost due to hardware changes. IsPlaying
    // will report as AL_STOPPED and any music that was playing, will not
    // resume when the audio is reinitialised.
    if(spsState == PS_PLAYING && IsPlaying()) spsState = PS_WASPLAYING;
    // Get reference to source, stop it, unqueue it
    sCptr->StopAndUnQueueAllBuffers();
    // Allow the source manager to recycle this source
    sCptr->Unlock();
    sCptr = nullptr;
  }
  /* -- Load and lock source ----------------------------------------------- */
  void GenerateBuffers()
  { // Ignore if we already have buffers
    if(aluvBuffers.empty())
      XC("Empty sources list!",
        "Name", NameGet(), "BufferCount", aluvBuffers.size());
    // Generate OpenAL buffers
    AL(cOal->CreateBuffers(aluvBuffers),
      "Failed to generate buffers for stream!",
      "Name", NameGet(), "Count", aluvBuffers.size());
    // Generate space for queued buffers
    aluvUnQBuffers.resize(aluvBuffers.size());
    aluvReQBuffers.reserve(aluvBuffers.size());
  }
  /* -- Lock source buffer ------------------------------------------------- */
  bool LockSource()
  { // Need a source?
    if(!sCptr)
    { // Lock a new source and if failed?
      sCptr = GetSource();
      if(!sCptr)
      { // Log and return failure
        cLog->LogWarningExSafe("Stream out of sources locking '$'!",
          NameGet());
        return false;
      } // Success
    } // Success
    return true;
  }
  /* -- Get/Set loop ------------------------------------------------------- */
  void SetLoopBegin(const ogg_int64_t llNewPos) { llLoopBegin = llNewPos; }
  void SetLoopEnd(const ogg_int64_t llNewPos) { llLoopEnd = llNewPos; }
  void SetLoopRange(const ogg_int64_t llNBPos, const ogg_int64_t llNEPos)
    { SetLoopBegin(llNBPos); SetLoopEnd(llNEPos); }
  void SetLoop(const ogg_int64_t llLoopCount)
  { // Set the loop
    llLoop = llLoopCount;
    // Set loop to the end if we're to finish
    if(!llLoop) SetLoopEnd(GetSamples());
  }
  /* -- Get functions with safe versions---------------------------- */ public:
  bool IsPlaying() const { return sCptr && sCptr->GetState() == AL_PLAYING; }
  ALfloat GetVolume() const { return alfVolume; }
  /* -- Get info (safe functions) ------------------------------------------ */
  long GetRate() const { return viData.rate; }
  int GetChannels() const { return viData.channels; }
  int GetVersion() const { return viData.version; }
  long GetBitRateUpper() const { return viData.bitrate_upper; }
  long GetBitRateNominal() const { return viData.bitrate_nominal; }
  long GetBitRateLower() const { return viData.bitrate_lower; }
  long GetBitRateWindow() const { return viData.bitrate_window; }
  /* -- Get loop (unsafe functions) ---------------------------------------- */
  ogg_int64_t GetLoop() const { return llLoop; }
  ogg_int64_t GetLoopBegin() const { return llLoopBegin; }
  ogg_int64_t GetLoopEnd() const { return llLoopEnd; }
  /* -- Seek and tell functions (with locks) ------------------------------- */
  void SetLoopSafe(const ogg_int64_t llLoopCount)
    { MutexCall([this, llLoopCount](){ SetLoop(llLoopCount); }); }
  ogg_int64_t GetLoopSafe()
    { return MutexCall([this](){ return GetLoop(); }); }
  ogg_int64_t GetLoopBeginSafe()
    { return MutexCall([this](){ return GetLoopBegin(); }); }
  ogg_int64_t GetLoopEndSafe()
    { return MutexCall([this](){ return GetLoopEnd(); }); }
  void SetLoopBeginSafe(const ogg_int64_t llNewPos)
    { MutexCall([this, llNewPos](){ SetLoopBegin(llNewPos); }); }
  void SetLoopEndSafe(const ogg_int64_t llNewPos)
    { MutexCall([this, llNewPos](){ SetLoopEnd(llNewPos); }); }
  void SetLoopRangeSafe(const ogg_int64_t llNBPos, const ogg_int64_t llNEPos)
    { MutexCall([this, llNBPos, llNEPos]()
        { SetLoopRange(llNBPos, llNEPos); }); }
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
  void SetPositionSafe(const ogg_int64_t llNewPos)
    { MutexCall([this, llNewPos](){ SetPosition(llNewPos); }); }
  void SetPositionFastSafe(const ogg_int64_t llPosition)
    { MutexCall([this, llPosition](){ SetPositionFast(llPosition); }); }
  ogg_int64_t GetSamplesSafe()
    { return MutexCall([this](){ return GetSamples(); }); }
  /* ----------------------------------------------------------------------- */
  ogg_int64_t GetOggBytes() const { return fmFile.MemSize<ogg_int64_t>(); }
  /* -- GetFormat ---------------------------------------------------------- */
  ALenum GetFormat() const { return aleFormat; }
  StdStringView GetFormatName() const { return cOal->GetALFormat(aleFormat); }
  /* -- Main (from audio thread) ------------------------------------------- */
  void Main()
  { // Wait for audio thread and lock access to stream buffers
    MutexCall([this](){
      // Compare state
      switch(spsState)
      { // Don't care if on stand by
        case PS_STANDBY: break;
        // Is finishing playing?
        case PS_FINISHING:
        { // Ignore if there is no source
          if(!sCptr) { spsState = PS_STANDBY; return; }
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
          if(!sCptr) { spsState = PS_STANDBY; return; }
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
                fmFile.NameGet());
              // Done
              return;
            } // Log problem
            cLog->LogWarningExSafe(
              "Stream '$' stopped unexpectedly and is being replayed!",
              fmFile.NameGet());
            // Replay the buffers
            sCptr->Play();
            // Done
            return;
          } // Until all the buffers have finished processing
          if(const ALsizei alsiBuffersProcessed = sCptr->GetBuffersProcessed())
          { // Unqueue the buffers and if we unqueued any?
            sCptr->UnQueueBuffers(aluvUnQBuffers.data(), alsiBuffersProcessed);
            // For each unqueued buffer
            StdForEach(seq, aluvUnQBuffers.cbegin(),
              next(aluvUnQBuffers.cbegin(), alsiBuffersProcessed),
            [this](const ALuint aluBuffer)
            { // Progress live position. This is so if the guest is saving
              // the position to replay at a later time, this position will
              // be on or slightly before the decoder position instead of at
              // the decoder position which will be way after the live
              // playback position.
              llLivePos +=
                cOal->GetBufferInt<ogg_int64_t>(aluBuffer, AL_SIZE) /
                // This could be 16 (if no AL_EXT_FLOAT32) or 32.
                (cOal->GetBufferInt<ogg_int64_t>(aluBuffer, AL_BITS) / 8) /
                // This should always be 1 or 2.
                (cOal->GetBufferInt<ogg_int64_t>(aluBuffer, AL_CHANNELS));
              // Try to rebuffer data to it and if succeeded?
              if(Rebuffer(aluBuffer))
              { // Add to queue rebuffer list and goto next unqueued buffer
                aluvReQBuffers.push_back(aluBuffer);
                return;
              } // Run out of loops? Finish playing and stop
              if(!llLoop)
              { // Finish playing, reset position and break
                spsState = PS_FINISHING;
                SetPosition(0);
                return;
              } // Seek to start and try rebuffer again and if failed still?
              SetPosition(llLoopBegin);
              if(!Rebuffer(aluBuffer))
              { // Stop playing, reset position and break
                Stop(SR_RWREBUFFAIL);
                SetPosition(0);
                return;
              } // We have a buffer to re-queue
              aluvReQBuffers.push_back(aluBuffer);
              // If not looping forever? Reduce count and if zero play to end
              if(llLoop != -1 && !--llLoop) SetLoopEnd(GetSamples());
            });
            // Have buffers to re-queue?
            if(aluvReQBuffers.size())
            { // Re-queue them
              sCptr->QueueBuffers(aluvReQBuffers.data(),
                static_cast<ALsizei>(aluvReQBuffers.size()));
              // Clear the re-queue
              aluvReQBuffers.clear();
            }
          } // Fall through to break
          [[fallthrough]];
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
      if(spsState != PS_WASPLAYING) { spsState = PS_STANDBY; return; }
      // Lock the source and return with message if we can't
      if(!LockSource())
      { // Log problem
        cLog->LogWarningExSafe(
          "Stream '$' could not be allocated a source after reset!",
          fmFile.NameGet());
        // Set internal state to standby
        spsState = PS_STANDBY;
        // Done
        return;
      } // If we didn't rebuffer anything then no point playing
      if(!FullRebuffer())
      { // Log problem
        cLog->LogWarningExSafe(
          "Stream '$' could not be rebuffered after reset!",
          fmFile.NameGet());
        // Set fully stopped and return
        return Stop(SR_GENBUFFAIL);
      } // Update volume
      UpdateVolume();
      // Play the buffers
      sCptr->Play();
      // Set internal state to playing
      spsState = PS_PLAYING;
    });
  }
  /* -- Unload source and buffers ------------------------------------------ */
  void UnloadSourceAndBuffers() { UnloadSource(); UnloadBuffers(); }
  /* -- Update volume ------------------------------------------------------ */
  void UpdateVolume()
    { if(sCptr)
        sCptr->SetGain(alfVolume * cMixer->MixerGetAdjStreamVolume()); }
  /* -- Update and apply volume -------------------------------------------- */
  void SetVolume(const ALfloat fNewVolume)
    { alfVolume = fNewVolume; UpdateVolume(); }
  /* -- Fully rebuffer stream data ----------------------------------------- */
  bool FullRebuffer()
  { // Start from buffer at index 0
    size_t stIndex = 0;
    // Until we run out of buffers allocated
    while(stIndex < aluvBuffers.size())
    { // If we could not rebuffer then we maybe at the end of file
      if(!Rebuffer(aluvBuffers[stIndex]))
      { // Try to buffer from the loop start
        SetPosition(llLoopBegin);
        // Try to rebuffer again and if failed then throw an error
        if(!Rebuffer(aluvBuffers[stIndex]))
          XC("Full rebuffer from loop start failed!",
            "Name", fmFile.NameGet(),
            "LoopBegin", static_cast<uint64_t>(llLoopBegin));
      } // Increment buffer index
      ++stIndex;
    } // We can't play anything if we couldn't decode to one buffer
    if(!stIndex) return false;
    // Queue the buffers and play them
    sCptr->QueueBuffers(aluvBuffers.data(), static_cast<ALsizei>(stIndex));
    // We buffered something
    return true;
  }
  /* -- Play with lock ----------------------------------------------------- */
  void PlaySafe() { MutexCall([this](){ Play(); }); }
  /* -- Stop with lock ----------------------------------------------------- */
  void StopSafe(const StreamStopReason ssrReason)
    { MutexCall([this, ssrReason](){ Stop(ssrReason); }); }
  /* -- Load from memory --------------------------------------------------- */
  void AsyncReady(FileMap &fmData)
  { // Set file class
    fmFile.FileMapSwap(fmData);
    // Initialise context and test for error
    if(const int iResult = ov_open_callbacks(&fmFile, &ovfContext, nullptr, 0,
         cCodecOGG->GetCallbacks()))
      XC("Init OGG decoder context failed!",
        "Name",   NameGet(), "Code", iResult,
        "Reason", cCodecOGG->GetOggErr(iResult));
    // We don't need to create more buffers than we need. If we don't do this
    // then Rebuffer() will not fill all the buffers and subsequent OpenAL
    // calls will fail. We'll add a minimum value of 1 too just incase we get
    // a stream with no data.
    aluvBuffers.resize(
      UtilClamp(static_cast<size_t>(ceil(static_cast<ALdouble>
      (GetSamples()) / static_cast<ALdouble>(cParent->stBufSize))), 1,
      cParent->stBufCount));
    // Get info about ogg and copy it into our static buffer if succeeded,
    // else show an exception if failed. This removes dereferencing of the
    // vorbis info struct.
    if(const vorbis_info*const viPtr = ov_info(&ovfContext, -1))
      StdMemCopy(&viData, viPtr, sizeof(viData));
    else XC("Failed to get vorbis info!", "Name", NameGet());
    // Only 1-2 channels supported
    if(GetChannels() < 1 || GetChannels() > 2)
      XC("Unsupported channel count!",
        "Name", NameGet(), "Channels", GetChannels());
    // Compare number of channels in file to set appropriate format
    aleFormat = GetChannels() == 1 ?
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
      if(cLog->LogHasLevel(LH_DEBUG))
        for(const StrNCStrMapPair &sncsmpPair : ssMetaData)
          cLog->LogNLCDebugExSafe("- Vorbis comment: $ -> $.",
            sncsmpPair.first, sncsmpPair.second);
    } // Generate buffers, recommending this amount
    GenerateBuffers();
    // Log ogg loaded
    cLog->LogInfoExSafe(
      "Stream loaded '$' (C=$;R=$;BR=$:$:$:$;D$=$;B=$;BS=$;V=$$).",
      NameGet(), GetChannels(), GetRate(), viData.bitrate_upper,
      viData.bitrate_nominal, viData.bitrate_lower, viData.bitrate_window,
      StdIOSFixed, GetDuration(), aluvBuffers.size(), MemSize(), StdIOSHex,
      GetVersion());
  }
  /* -- Return metadata as table ------------------------------------------- */
  const StrNCStrMap &GetMetaData() const { return ssMetaData; }
  /* -- Constructor -------------------------------------------------------- */
  Stream() :
    /* -- Initialisers ----------------------------------------------------- */
    ICHelperStream{ cStreams },        // Initialise collector unregistered
    SerialSlave{ cParent->Serial() },  // Initialise identification number
    AsyncLoaderStream{ this,           // Initialise async loader
      EMC_MP_STREAM },                 //   with our streaming event
    LuaEvtSlave{ this,                 // Initialise stream event manager
      EMC_STR_EVENT },                 //   with our stremaing event
    ovfContext{},                      // No file opened yet
    viData{},                          // No vorbis information yet
    sCptr(nullptr),                    // No associated OAL source yet
    aleFormat(AL_NONE),                // No OAL format id yet
    llLivePos(0), llDecPos(0),         // No ext|internal position
    llLoopBegin(0), llLoopEnd(0),      // No loop start/end position
    llLoop(0),                         // Do not loop
    spsState(PS_STANDBY),              // Current state to standby
    alfVolume(1.0f)                    // No volume yet
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- Destructor --------------------------------------------------------- */
  DTORHELPER(~Stream,
    // Stop any pending async operations
    AsyncCancel();
    // Remove the collector registration to stop audio thread polling
    ICHelperStream::CollectorUnregister();
    // Nothing else to do if file data not initialised
    if(fmFile.FileMapClosed()) return;
    // If there is a source?
    if(sCptr)
    { // Stop the source, unqueue all buffers, unlock and
      sCptr->StopAndUnQueueAllBuffers();
      sCptr->Unlock();
    } // Unload the buffers
    UnloadBuffers();
    // If stream opened? Clear ogg state
    if(ovfContext.datasource) ov_clear(&ovfContext);
    // Log that the stream was unloaded
    cLog->LogDebugExSafe("Stream unloaded '$'!", NameGet());
  )
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
    "SR_LUA"                           // [6] Requested by Lua (guest).
  }, "SR_UNKNOWN" },                   // Stop reason strings initialised
  psStrings{{                          // Initialise play state strings
    "PS_STANDBY",                      // [0] Is not playing
    "PS_PLAYING",                      // [1] Is playing
    "PS_FINISHING",                    // [2] Was stopping (no more data)
    "PS_WASPLAYING",                   // [3] Was playing (audio reset)
  }, "PS_UNKNOWN" },                   // Play state strings initialised
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
  cMixer->MixerSetStreamVolume(fNewVolume);
  // Update volumes on all streams
  StreamCommitVolume();
  // Success
  return ACCEPT;
}
/* -- Set memory allocated per buffer -------------------------------------- */
static CVarReturn StreamSetBufferSize(const size_t stNewSize)
  { return CVarSimpleSetIntNLG(cStreams->stBufSize, stNewSize,
      4096UL, 1048576UL); }
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
