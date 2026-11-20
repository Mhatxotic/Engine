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
using namespace ILuaEvt::P;            using namespace ILuaIdent::P;
using namespace ILuaLib::P;            using namespace ILuaUtil::P;
using namespace IMemory::P;            using namespace IMixer::P;
using namespace IMutex::P;             using namespace IName::P;
using namespace IOal::P;               using namespace ISerial::P;
using namespace ISource::P;            using namespace IStd::P;
using namespace IString::P;            using namespace ISysUtil::P;
using namespace IUtil::P;              using namespace LOgg;
using namespace LAL::Types;
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
  void StreamUpdatePosition()
    { llDecPos = llLivePos = ov_pcm_tell(&ovfContext); }
  /* -- Get time elapsed --------------------------------------------------- */
  ALdouble StreamGetElapsed() { return ov_time_tell(&ovfContext); }
  /* -- Set time elapsed --------------------------------------------------- */
  void StreamSetElapsed(const ALdouble dElapsed)
    { ov_time_seek(&ovfContext, dElapsed); StreamUpdatePosition(); }
  /* -- Set time elapsed faster -------------------------------------------- */
  void StreamSetElapsedFast(const ALdouble dElapsed)
    { ov_time_seek_page(&ovfContext, dElapsed); StreamUpdatePosition(); }
  /* -- Get total time ----------------------------------------------------- */
  ALdouble StreamGetDuration() { return ov_time_total(&ovfContext, -1); }
  /* -- Get PCM bytes duration --------------------------------------------- */
  ogg_int64_t StreamGetPosition() const { return llLivePos; }
  /* -- Set PCM byte position ---------------------------------------------- */
  void StreamSetPosition(const ogg_int64_t llNewPos)
    { ov_pcm_seek(&ovfContext, llNewPos); StreamUpdatePosition(); }
  /* -- Set PCM byte position fast ----------------------------------------- */
  void StreamSetPositionFast(const ogg_int64_t llPosition)
    { ov_pcm_seek_page(&ovfContext, llPosition); StreamUpdatePosition(); }
  /* -- Get total PCM samples ---------------------------------------------- */
  ogg_int64_t StreamGetSamples() { return ov_pcm_total(&ovfContext, -1); }
  /* -- Convert stop reason to string -------------------------------------- */
  const StdStringView
    &StreamStopReasonToString(const StreamStopReason ssrReason) const
  { return ctParent.srStrings.Get(ssrReason); }
  /* -- Convert stop reason to string -------------------------------------- */
  const StdStringView
    &StreamStateReasonToString(const StreamPlayState psReason) const
  { return ctParent.psStrings.Get(psReason); }
  /* -- Stop (without locks) ----------------------------------------------- */
  void StreamStop(const StreamStopReason ssrReason)
  { // Don't have source class? There is nothing else to do!
    if(!sCptr) return;
    // Stop source from playing and unload it
    sCptr->SourceStop();
    StreamUnloadSource();
    // Go back to unplayed position
    StreamSetPosition(llLivePos);
    // Write debug reason for stoppage
    cLog->LogDebugExSafe("Stream stopped '$'! (R:$<$>;L:$<$>).", NameGet(),
      StreamStopReasonToString(ssrReason), ssrReason,
      StreamStateReasonToString(spsState), spsState);
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
  void StreamPlay()
  { // If we already have a source? Stop and unqueue all buffers
    if(sCptr) sCptr->SourceStopAndUnQueueAllBuffers();
    // Grab and lock a new free source if need be return if we can't
    else if(!StreamLockSource()) return;
    // Update volume
    StreamUpdateVolume();
    // If we didn't rebuffer anything then no point playing
    if(!StreamFullRebuffer()) return;
    // Play the buffers
    sCptr->SourcePlay();
    // Send playback event if was not already playing
    LuaEvtDispatch(SE_PLAY, spsState);
    // Set internal state to playing
    spsState = PS_PLAYING;
  }
  /* -- Decompression routine (VORBIS->PCM) -------------------------------- */
  bool StreamRebuffer(const ALuint uBufferId)
  { // Reseek and rebuffer if looping
    if(llDecPos >= llLoopEnd) return false;
    // Bytes written to buffer and bytes per channel
    size_t stPos = 0, stBpc;
    // Number of channels as size_t
    const size_t stChannels = static_cast<size_t>(StreamGetChannels());
    // Decode the buffer and grab the size. If nothing was decoded then reloop.
    if(cOal->OalHave32FPPB())
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
    AL(cOal->OalBufferData(uBufferId, StreamGetFormat(), MemPtr<ALvoid>(),
      static_cast<ALsizei>(stPos), static_cast<ALsizei>(StreamGetRate())),
      "Failed to buffer ogg stream data!",
      "Name",       NameGet(),         "BufferId",   uBufferId,
      "Format",     StreamGetFormat(), "BufferData", MemPtr<void>(),
      "BufferSize", stPos,             "Rate",       StreamGetRate());
    // Return status
    return true;
  }
  /* -- Unload buffers ----------------------------------------------------- */
  void StreamUnloadBuffers()
  { // Unload the buffers if we have them
    if(!aluvBuffers.empty())
      ALL(cOal->OalDeleteBuffers(aluvBuffers),
        "Stream '$' failed to delete $ buffers!",
        NameGet(), aluvBuffers.size());
  }
  /* -- Unload source ------------------------------------------------------ */
  void StreamUnloadSource()
  { // If source is not available, bail
    if(!sCptr) return;
    // Save current state. Use our internal state to decide if we should replay
    // the source as if the context was lost due to hardware changes. IsPlaying
    // will report as AL_STOPPED and any music that was playing, will not
    // resume when the audio is reinitialised.
    if(spsState == PS_PLAYING && StreamIsPlaying()) spsState = PS_WASPLAYING;
    // Get reference to source, stop it, unqueue it
    sCptr->SourceStopAndUnQueueAllBuffers();
    // Allow the source manager to recycle this source
    sCptr->SourceUnlock();
    sCptr = nullptr;
  }
  /* -- Load and lock source ----------------------------------------------- */
  void StreamGenerateBuffers()
  { // Ignore if we already have buffers
    if(aluvBuffers.empty())
      XC("Empty sources list!",
        "Name", NameGet(), "BufferCount", aluvBuffers.size());
    // Generate OpenAL buffers
    AL(cOal->OalCreateBuffers(aluvBuffers),
      "Failed to generate buffers for stream!",
      "Name", NameGet(), "Count", aluvBuffers.size());
    // Generate space for queued buffers
    aluvUnQBuffers.resize(aluvBuffers.size());
    aluvReQBuffers.reserve(aluvBuffers.size());
  }
  /* -- Lock source buffer ------------------------------------------------- */
  bool StreamLockSource()
  { // Need a source?
    if(!sCptr)
    { // Lock a new source and if failed?
      sCptr = SourceGet();
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
  void StreamSetLoopBegin(const ogg_int64_t llNewPos)
    { llLoopBegin = llNewPos; }
  void StreamSetLoopEnd(const ogg_int64_t llNewPos)
    { llLoopEnd = llNewPos; }
  void StreamSetLoopRange(const ogg_int64_t llNBPos, const ogg_int64_t llNEPos)
    { StreamSetLoopBegin(llNBPos); StreamSetLoopEnd(llNEPos); }
  void StreamSetLoop(const ogg_int64_t llLoopCount)
  { // Set the loop
    llLoop = llLoopCount;
    // Set loop to the end if we're to finish
    if(!llLoop) StreamSetLoopEnd(StreamGetSamples());
  }
  /* -- Get functions with safe versions---------------------------- */ public:
  bool StreamIsPlaying() const
    { return sCptr && sCptr->SourceGetState() == AL_PLAYING; }
  ALfloat StreamGetVolume() const { return alfVolume; }
  /* -- Get info (safe functions) ------------------------------------------ */
  long StreamGetRate() const { return viData.rate; }
  int StreamGetChannels() const { return viData.channels; }
  int StreamGetVersion() const { return viData.version; }
  long StreamGetBitRateUpper() const { return viData.bitrate_upper; }
  long StreamGetBitRateNominal() const { return viData.bitrate_nominal; }
  long StreamGetBitRateLower() const { return viData.bitrate_lower; }
  long StreamGetBitRateWindow() const { return viData.bitrate_window; }
  /* -- Get loop (unsafe functions) ---------------------------------------- */
  ogg_int64_t StreamGetLoop() const { return llLoop; }
  ogg_int64_t StreamGetLoopBegin() const { return llLoopBegin; }
  ogg_int64_t StreamGetLoopEnd() const { return llLoopEnd; }
  /* -- Seek and tell functions (with locks) ------------------------------- */
  void StreamSetLoopSafe(const ogg_int64_t llLoopCount)
    { MutexCall([this, llLoopCount](){ StreamSetLoop(llLoopCount); }); }
  ogg_int64_t StreamGetLoopSafe()
    { return MutexCall([this](){ return StreamGetLoop(); }); }
  ogg_int64_t StreamGetLoopBeginSafe()
    { return MutexCall([this](){ return StreamGetLoopBegin(); }); }
  ogg_int64_t StreamGetLoopEndSafe()
    { return MutexCall([this](){ return StreamGetLoopEnd(); }); }
  void StreamSetLoopBeginSafe(const ogg_int64_t llNewPos)
    { MutexCall([this, llNewPos](){ StreamSetLoopBegin(llNewPos); }); }
  void StreamSetLoopEndSafe(const ogg_int64_t llNewPos)
    { MutexCall([this, llNewPos](){ StreamSetLoopEnd(llNewPos); }); }
  void StreamSetLoopRangeSafe(const ogg_int64_t llNBPos,
    const ogg_int64_t llNEPos)
  { MutexCall([this, llNBPos, llNEPos]()
      { StreamSetLoopRange(llNBPos, llNEPos); }); }
  ALdouble StreamGetElapsedSafe()
    { return MutexCall([this](){ return StreamGetElapsed(); }); }
  void StreamSetElapsedSafe(const ALdouble dElapsed)
    { MutexCall([this, dElapsed](){ StreamSetElapsed(dElapsed); }); }
  void StreamSetElapsedFastSafe(const ALdouble dElapsed)
    { MutexCall([this, dElapsed](){ StreamSetElapsedFast(dElapsed); }); }
  ALdouble StreamGetDurationSafe()
    { return MutexCall([this](){ return StreamGetDuration(); }); }
  ogg_int64_t StreamGetPositionSafe()
    { return MutexCall([this](){ return StreamGetPosition(); }); }
  void StreamSetPositionSafe(const ogg_int64_t llNewPos)
    { MutexCall([this, llNewPos](){ StreamSetPosition(llNewPos); }); }
  void StreamSetPositionFastSafe(const ogg_int64_t llPosition)
    { MutexCall([this, llPosition](){ StreamSetPositionFast(llPosition); }); }
  ogg_int64_t StreamGetSamplesSafe()
    { return MutexCall([this](){ return StreamGetSamples(); }); }
  /* ----------------------------------------------------------------------- */
  ogg_int64_t StreamGetOggBytes() const
    { return fmFile.MemSize<ogg_int64_t>(); }
  /* -- GetFormat ---------------------------------------------------------- */
  ALenum StreamGetFormat() const { return aleFormat; }
  StdStringView StreamGetFormatName() const
    { return cOal->OalFormatToString(aleFormat); }
  /* -- Main (from audio thread) ------------------------------------------- */
  void StreamMain()
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
          if(!sCptr->SourceGetBuffersQueued()) return StreamStop(SR_STOPNOUNQ);
          // Unqueue all the buffers
          sCptr->SourceUnQueueAllBuffers();
          // Return if theres still buffers queued
          if(sCptr->SourceGetBuffersQueued()) return;
          // Full stop
          return StreamStop(SR_STOPUNQ);
        } // Is playing?
        case PS_PLAYING:
        { // Ignore if there is no source
          if(!sCptr) { spsState = PS_STANDBY; return; }
          // Stopped playing and should be playing? Start playing again.
          if(!StreamIsPlaying())
          { // Unqueue all buffers
            sCptr->SourceUnQueueAllBuffers();
            // Do a full rebuffer of those buffers and if we can't rebuffer
            if(!StreamFullRebuffer())
            { // Full stop!
              StreamStop(SR_REBUFFAIL);
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
            sCptr->SourcePlay();
            // Done
            return;
          } // Until all the buffers have finished processing
          if(const ALsizei alsiBuffersProcessed =
            sCptr->SourceGetBuffersProcessed())
          { // Unqueue the buffers and if we unqueued any?
            sCptr->SourceUnQueueBuffers(aluvUnQBuffers.data(),
              alsiBuffersProcessed);
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
                cOal->OalGetBufferInt<ogg_int64_t>(aluBuffer, AL_SIZE) /
                // This could be 16 (if no AL_EXT_FLOAT32) or 32.
                (cOal->OalGetBufferInt<ogg_int64_t>(aluBuffer, AL_BITS) / 8) /
                // This should always be 1 or 2.
                (cOal->OalGetBufferInt<ogg_int64_t>(aluBuffer, AL_CHANNELS));
              // Try to rebuffer data to it and if succeeded?
              if(StreamRebuffer(aluBuffer))
              { // Add to queue rebuffer list and goto next unqueued buffer
                aluvReQBuffers.push_back(aluBuffer);
                return;
              } // Run out of loops? Finish playing and stop
              if(!llLoop)
              { // Finish playing, reset position and break
                spsState = PS_FINISHING;
                StreamSetPosition(0);
                return;
              } // Seek to start and try rebuffer again and if failed still?
              StreamSetPosition(llLoopBegin);
              if(!StreamRebuffer(aluBuffer))
              { // Stop playing, reset position and break
                StreamStop(SR_RWREBUFFAIL);
                StreamSetPosition(0);
                return;
              } // We have a buffer to re-queue
              aluvReQBuffers.push_back(aluBuffer);
              // If not looping forever? Reduce count and if zero play to end
              if(llLoop != -1 && !--llLoop)
                StreamSetLoopEnd(StreamGetSamples());
            });
            // Have buffers to re-queue?
            if(aluvReQBuffers.size())
            { // Re-queue them
              sCptr->SourceQueueBuffers(aluvReQBuffers.data(),
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
  void StreamGenerateSourceAndBuffers()
  { // Protect modifications from audio main thread
    MutexCall([this](){
      // Generate buffer id's
      StreamGenerateBuffers();
      // Ignore and set to standby if wasn't playing before
      if(spsState != PS_WASPLAYING) { spsState = PS_STANDBY; return; }
      // Lock the source and return with message if we can't
      if(!StreamLockSource())
      { // Log problem
        cLog->LogWarningExSafe(
          "Stream '$' could not be allocated a source after reset!",
          fmFile.NameGet());
        // Set internal state to standby
        spsState = PS_STANDBY;
        // Done
        return;
      } // If we didn't rebuffer anything then no point playing
      if(!StreamFullRebuffer())
      { // Log problem
        cLog->LogWarningExSafe(
          "Stream '$' could not be rebuffered after reset!",
          fmFile.NameGet());
        // Set fully stopped and return
        return StreamStop(SR_GENBUFFAIL);
      } // Update volume
      StreamUpdateVolume();
      // Play the buffers
      sCptr->SourcePlay();
      // Set internal state to playing
      spsState = PS_PLAYING;
    });
  }
  /* -- Unload source and buffers ------------------------------------------ */
  void StreamUnloadSourceAndBuffers()
    { StreamUnloadSource(); StreamUnloadBuffers(); }
  /* -- Update volume ------------------------------------------------------ */
  void StreamUpdateVolume()
    { if(sCptr)
        sCptr->SourceSetGain(alfVolume * cMixer->MixerGetAdjStreamVolume()); }
  /* -- Update and apply volume -------------------------------------------- */
  void StreamSetVolume(const ALfloat fNewVolume)
    { alfVolume = fNewVolume; StreamUpdateVolume(); }
  /* -- Fully rebuffer stream data ----------------------------------------- */
  bool StreamFullRebuffer()
  { // Start from buffer at index 0
    size_t stIndex = 0;
    // Until we run out of buffers allocated
    while(stIndex < aluvBuffers.size())
    { // If we could not rebuffer then we maybe at the end of file
      if(!StreamRebuffer(aluvBuffers[stIndex]))
      { // Try to buffer from the loop start
        StreamSetPosition(llLoopBegin);
        // Try to rebuffer again and if failed then throw an error
        if(!StreamRebuffer(aluvBuffers[stIndex]))
          XC("Full rebuffer from loop start failed!",
            "Name", fmFile.NameGet(),
            "LoopBegin", static_cast<uint64_t>(llLoopBegin));
      } // Increment buffer index
      ++stIndex;
    } // We can't play anything if we couldn't decode to one buffer
    if(!stIndex) return false;
    // Queue the buffers and play them
    sCptr->SourceQueueBuffers(aluvBuffers.data(),
      static_cast<ALsizei>(stIndex));
    // We buffered something
    return true;
  }
  /* -- Play with lock ----------------------------------------------------- */
  void StreamPlaySafe() { MutexCall([this](){ StreamPlay(); }); }
  /* -- Stop with lock ----------------------------------------------------- */
  void StreamStopSafe(const StreamStopReason ssrReason)
    { MutexCall([this, ssrReason](){ StreamStop(ssrReason); }); }
  /* -- Load from memory --------------------------------------------------- */
  bool AsyncReady(FileMap &fmData)
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
      (StreamGetSamples()) / static_cast<ALdouble>(ctParent.stBufSize))), 1,
      ctParent.stBufCount));
    // Get info about ogg and copy it into our static buffer if succeeded,
    // else show an exception if failed. This removes dereferencing of the
    // vorbis info struct.
    if(const vorbis_info*const viPtr = ov_info(&ovfContext, -1))
      StdMemCopy(&viData, viPtr, sizeof(viData));
    else XC("Failed to get vorbis info!", "Name", NameGet());
    // Only 1-2 channels supported
    if(StreamGetChannels() < 1 || StreamGetChannels() > 2)
      XC("Unsupported channel count!",
        "Name", NameGet(), "Channels", StreamGetChannels());
    // Compare number of channels in file to set appropriate format
    aleFormat = StreamGetChannels() == 1 ?
      (cOal->OalHave32FPPB() ? AL_FORMAT_MONO_FLOAT32 : AL_FORMAT_MONO16) :
      (cOal->OalHave32FPPB() ? AL_FORMAT_STEREO_FLOAT32 : AL_FORMAT_STEREO16);
    // Allocate the buffer with size from global setting. We can re-use the
    // 'Memory' class from the 'AsyncLoader' class.
    MemInitBlank(ctParent.stBufSize);
    // Set default loop position to the end
    StreamSetLoopRange(0, StreamGetSamples());
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
    StreamGenerateBuffers();
    // Log ogg loaded
    cLog->LogInfoExSafe(
      "Stream loaded '$' (C=$;R=$;BR=$:$:$:$;D$=$;B=$;BS=$;V=$$).",
      NameGet(), StreamGetChannels(), StreamGetRate(), viData.bitrate_upper,
      viData.bitrate_nominal, viData.bitrate_lower, viData.bitrate_window,
      StdIOSFixed, StreamGetDuration(), aluvBuffers.size(), MemSize(),
      StdIOSHex, StreamGetVersion());
    // FileMap references the 'Memory' base class so don't free it
    return false;
  }
  /* -- Return metadata as table ------------------------------------------- */
  const StrNCStrMap &StreamGetMetaData() const { return ssMetaData; }
  /* -- Constructor -------------------------------------------------------- */
  Stream() :
    /* -- Initialisers ----------------------------------------------------- */
    ICHelperStream{ cStreams },        // Initialise collector unregistered
    SerialSlave{ ctParent.Serial() },  // Initialise identification number
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
      sCptr->SourceStopAndUnQueueAllBuffers();
      sCptr->SourceUnlock();
    } // Unload the buffers
    StreamUnloadBuffers();
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
    for(Stream*const sPtr : *cStreams) sPtr->StreamMain();
  });
}
/* == Unload all source and buffers ======================================== */
static void StreamDeInit()
{ // Lock access to streams collector list and uninitialise all buffers
  cStreams->MutexCall([](){
    for(Stream*const sPtr : *cStreams) sPtr->StreamUnloadSourceAndBuffers();
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
    for(Stream*const sPtr : *cStreams) sPtr->StreamGenerateSourceAndBuffers();
  });
}
/* == Stop all streams ===================================================== */
static void StreamStop()
{ // Lock access to bitmap collector list and stop all streams
  cStreams->MutexCall([](){
    for(Stream*const sPtr : *cStreams) sPtr->StreamStopSafe(SR_STOPALL);
  });
}
/* == Update all streams base volume======================================== */
static void StreamCommitVolume()
{ // Lock access to bitmap collector list and update all stream volumes
  cStreams->MutexCall([](){
    for(Stream*const sPtr : *cStreams) sPtr->StreamUpdateVolume();
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
