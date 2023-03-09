/* == VIDEO.HPP ============================================================ **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## This file handles streaming from .OGV files and playing to OpenAL.  ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IVideo {                     // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IAsset::P;             using namespace IASync::P;
using namespace IClock::P;             using namespace ICollector::P;
using namespace ICVarDef::P;           using namespace IError::P;
using namespace IEvtMain::P;           using namespace IFbo::P;
using namespace IFileMap::P;           using namespace IFlags;
using namespace IIdent::P;             using namespace ILog::P;
using namespace ILuaEvt::P;            using namespace ILuaLib::P;
using namespace ILuaUtil::P;           using namespace IMemory::P;
using namespace IOal::P;               using namespace IOgl::P;
using namespace IPcmLib::P;            using namespace IShader::P;
using namespace IShaders::P;           using namespace ISource::P;
using namespace IStd::P;               using namespace IStream::P;
using namespace IString::P;            using namespace ISysUtil::P;
using namespace IThread::P;            using namespace ITimer::P;
using namespace IUtil::P;              using namespace Lib::Ogg;
using namespace Lib::Ogg::Theora;      using namespace Lib::OpenAL;
using namespace Lib::OS::GlFW;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* -- Video collector class for collector data and custom variables -------- */
CTOR_BEGIN_ASYNC(Videos, Video, CLHelperSafe,
/* -- Public variables ----------------------------------------------------- */
typedef IdList<TH_CS_NSPACES> CSStrings;
const CSStrings    csStrings;          // Colour space strings list
typedef IdList<TH_PF_NFORMATS> PFStrings;
const PFStrings    pfStrings;          // Pixel format strings list
double             dAudioBufferSize;   // Default audio buffer size
size_t             stIOBufferSize;     // Default IO buffer size
double             dMaxDrift;,,        // Maximum drift before drop frames
/* -- Derived classes ------------------------------------------------------ */
private LuaEvtMaster<Video, LuaEvtTypeParam<Video>>); // Lua event
/* ------------------------------------------------------------------------- */
BUILD_FLAGS(Video,
  /* ----------------------------------------------------------------------- */
  // No flags set?                     Have a theora stream?
  FL_NONE                   {Flag[0]}, FL_THEORA                 {Flag[1]},
  // Have a vorbis stream?             Video output initialised?
  FL_VORBIS                 {Flag[2]}, FL_GLINIT                 {Flag[3]},
  // Video is keyed?                   Filtering is enabled?
  FL_KEYED                  {Flag[4]}, FL_FILTER                 {Flag[5]},
  // Hard stopped?                     Video is playing?
  FL_STOP                   {Flag[6]}, FL_PLAY                   {Flag[7]}
);/* ======================================================================= */
CTOR_MEM_BEGIN_ASYNC(Videos, Video, ICHelperSafe, /* No CLHelper */),
  /* -- Base classes ------------------------------------------------------- */
  public Fbo,                          // Video file name
  public AsyncLoaderVideo,             // Asynchronous laoding of videos
  public LuaEvtSlave<Video>,           // Lua asynchronous events
  public VideoFlags,                   // Video settings flags
  private ClockInterval<>              // Frame playback timing helper
{ /* -- Typedefs ----------------------------------------------------------- */
  struct YCbCr                         // Y/Cb/Cr plane data
  { /* --------------------------------------------------------------------- */
    const size_t   stI;                // Unique index
    th_img_plane   tipP;               // Plane data
    /* --------------------------------------------------------------------- */
    void Reset(void) {
      tipP.width = tipP.height = tipP.stride = 0;
      tipP.data = nullptr;
    }
    /* -- Constructor that initialises id ---------------------------------- */
    explicit YCbCr(const size_t stNIndex) :
      /* -- Initialisers --------------------------------------------------- */
      stI(stNIndex),                   // Set unique id
      tipP{0, 0, 0, nullptr}           // Initialise frame data
      /* -- No code -------------------------------------------------------- */
      { }
  };/* --------------------------------------------------------------------- */
  struct Frame                         // Frame data
  { /* --------------------------------------------------------------------- */
    bool           bDraw;              // Draw this frame?
    typedef array<YCbCr, 3> YCCArray;  // Room for three frames
    YCCArray       yccaFrames;         // The planes (Y, Cb and Cr);
    /* --------------------------------------------------------------------- */
    void Reset(void) { bDraw = false;
                       for(YCbCr &yccFrame : yccaFrames) yccFrame.Reset(); }
    /* -- Constructor that initialises frame data -------------------------- */
    Frame(void) :
      /* -- Initialisers --------------------------------------------------- */
      bDraw(false),                    // Set frame not ready for drawing
      yccaFrames{ YCbCr{0}, YCbCr{1},  // Initialise Y/Cb/Cr frame data
                  YCbCr{2} }
      /* -- No code -------------------------------------------------------- */
      { }
  };/* --------------------------------------------------------------------- */
  enum Unblock { UB_STANDBY, UB_BLOCK, UB_DATA, UB_REINIT, UB_PLAY, UB_STOP,
                 UB_PAUSE, UB_FINISH };
  /* --------------------------------------------------------------- */ public:
  enum Event { VE_PLAY, VE_LOOP, VE_STOP, VE_PAUSE, VE_FINISH };
  /* -- Concurrency -------------------------------------------------------- */
  Thread           tThread;            // Video Decoding Thread
  mutex            mUpload;            // mutex for uploading data
  atomic<Unblock>  ubReason;           // Unlock condition variable
  SafeSizeT        stLoop;             // Loops count
  double           dDrift;             // Drift between audio and video
  const double     dMaxDrift,          // Maximum allowed drift
                   dMaxDriftNeg;       // Maximum allowed drift (negative
  SafeBool         bPause;             // Only pause the stream?
  /* -- Ogg ---------------------------------------------------------------- */
  ogg_sync_state   osysData;           // Ogg sync state
  ogg_page         opgData;            // Ogg page
  ogg_packet       opkData;            // Ogg packet
  const size_t     stIOBuf;            // IO buffer size
  const long       lIOBuf;             // IO buffer size
  FileMap          fmFile;             // File map for reading file
  /* -- Theora ------------------------------------------------------------- */
  ogg_stream_state ostsTheora;         // Ogg (Theora) stream states
  th_info          tiData;             // Theora info struct
  th_comment       tcData;             // Theora comment struct
  th_setup_info   *tsiPtr;             // Theora setup info
  th_dec_ctx      *tdcPtr;             // Theora decoder context
  th_ycbcr_buffer  tybData;            // Theora colour buffer
  ogg_int64_t      iVideoGranulePos;   // Video granule position
  SafeDouble       dVideoTime;         // Video time index
  double           dFPS;               // Video fps
  SafeUInt         uiVideoFrames,      // Frames rendered
                   uiVideoFramesLost;  // Frames skipped
  array<Frame,2>   faData;             // Frame data
  size_t           stFActive,          // Currently active frame
                   stFNext,            // Next frame to process
                   stFWaiting;         // Frames waiting to be processed
  SafeSizeT        stFFree;            // Frames free to be processed
  StrNCStrMap      ssThMetaData;       // Theora comments block
  /* -- Vorbis ------------------------------------------------------------- */
  const double     dAudBufMax;         // Audio buffer size
  ogg_stream_state ostsVorbis;         // Ogg (Vorbis) stream states
  vorbis_info      viData;             // Vorbis decoder info
  vorbis_comment   vcData;             // Vorbis comment block
  vorbis_dsp_state vdsData;            // Vorbis DSP state
  vorbis_block     vbData;             // Vorbis decoder block
  SafeDouble       dAudioTime;         // Audio time index
  ALdouble         dAudioBuffer;       // Audio buffered
  ALfloat          fAudioVolume;       // Audio volume
  StrNCStrMap      ssVoMetaData;       // Vorbis comments block
  /* -- OpenGL ------------------------------------------------------------- */
  FboItem          fboYCbCr;           // Blit data for actual YCbCr components
  array<GLuint,3>  uiaYCbCr;           // Texture id's for YCbCr components
  Shader          *shProgram;          // Shader program to use
  /* -- OpenAL ------------------------------------------------------------- */
  Source          *sSource;            // Source class
  ALenum           eFormat;            // Internal format
  /* == Buffer more data for OGG decoder ========================== */ private:
  bool DoIOBuffer(void)
  { // Get some memory from ogg which we have to do every time we need to read
    // data into it and if succeeded? Read data info buffer and if we read
    // some bytes? Tell ogg how much we wrote and return.
    if(char*const cpBuffer = ogg_sync_buffer(&osysData, lIOBuf))
      if(const size_t stCount = fmFile.FileMapReadToAddr(cpBuffer, stIOBuf))
        return ogg_sync_wrote(&osysData, static_cast<long>(stCount)) == -1;
    // EOF or buffer invalid so return error
    return true;
  }
  /* -- Rewind the theora stream ------------------------------------------- */
  void DoRewind(void)
  { // Rewind video to start
    fmFile.FileMapRewind();
    // Tell theora we reset the video position
    if(FlagIsSet(FL_THEORA))
      SetParameter<ogg_int64_t>(TH_DECCTL_SET_GRANPOS, 0);
    // Reset Vorbis dsp as required by documentation
    if(FlagIsSet(FL_VORBIS)) vorbis_synthesis_restart(&vdsData);
  }
  /* -- Rewind the theora stream and reset variables ----------------------- */
  void DoRewindAndReset(void)
  { // Rewind video to start
    DoRewind();
    // Reset granule position and frames rendered
    iVideoGranulePos = 0;
    uiVideoFrames = uiVideoFramesLost = 0;
    // Reset counters
    dVideoTime = dAudioTime = dDrift = dAudioBuffer = 0.0;
  }
  /* -- Get/Set theora decoder control ------------------------------------- */
  template<typename AnyType>
    int SetParameter(const int iVariable, AnyType atValue)
      { return th_decode_ctl(tdcPtr, iVariable,
          reinterpret_cast<void*>(&atValue), sizeof(atValue)); }
  template<typename AnyType=int>AnyType GetParameter(const int iVariable) const
    { return static_cast<AnyType>
        (th_decode_ctl(tdcPtr, iVariable, nullptr, 0)); }
  /* -- Process exhausted audio buffers ------------------------------------ */
  void ProcessExhaustedAudioBuffers(void)
  { // Get number of buffers queued
    for(ALsizei stP = sSource->GetBuffersProcessed(); stP; --stP)
    { // Unqueue a buffer and break if failed
      const ALuint uiBuffer = sSource->UnQueueBuffer();
      if(cOal->HaveError()) continue;
      // Remove buffer time
      dAudioBuffer = UtilMaximum(dAudioBuffer -
        (cOal->GetBufferInt<ALdouble>(uiBuffer, AL_SIZE) /
          GetSampleRate() / GetChannels()), 0.0);
      // Delete the buffer that was returned continue if successful
      ALL(cOal->DeleteBuffer(uiBuffer),
        "Video failed to delete unqueued buffer $ in '$'!",
           uiBuffer, IdentGet());
    }
  }
  /* -- Try to parse and render more Vorbis data --------------------------- */
  bool ParseAndRenderVorbisData(void)
  { // Data was parsed?
    bool bParsed = false;
    // Repeat until break or thread should exit
    while(tThread.ThreadShouldNotExit())
    { // If frames are available, but we're way behind the video?
      ALfloat **fpPCM;
      if(const int iFrames = vorbis_synthesis_pcmout(&vdsData, &fpPCM))
      { // Tell vorbis how much we read
        vorbis_synthesis_read(&vdsData, iFrames);
        // Set audio time
        dAudioTime = vorbis_granule_time(&vdsData, vdsData.granulepos);
        // Set that we got audio
        bParsed = true;
        // No need to do anymore if there is no source
        if(IsSourceUnavailable()) break;
        // Convert vorbis frames to correct type dealing with memory
        const size_t stFrames = static_cast<size_t>(iFrames),
          // Get number of channels as size_t
          stChannels = static_cast<size_t>(GetChannels());
        // Length of data (may need to be modified if 16-bit required)
        size_t stFrameSize = sizeof(float) * stFrames * stChannels;
        // If the hardware supports 32-bit (4b) floating point playback?
        if(cOal->Have32FPPB())
        { // Allocate required memory and do the conversion
          MemResizeUp(stFrameSize);
          PcmF32FromVorbisFrames(fpPCM, stFrames, stChannels,
            MemPtr<ALfloat>());
        } // If the hardware only supports 16-bit (2b) integer playback?
        else
        { // Allocate half the required memory and do the conversion. Note
          // that we can't optimise this var and use MemSize() because this
          // memory buffer might be bigger than we need and would be a waste
          // of CPU time to keep reallocating it hence the MemResizeUp().
          stFrameSize >>= 1;
          MemResizeUp(stFrameSize);
          PcmI16FromVorbisFrames(fpPCM, stFrames, stChannels,
            MemPtr<ALshort>());
        } // Generate a buffer for the pcm data and if succeeded?
        const ALuint uiBuffer = cOal->CreateBuffer();
        ALenum alErr = cOal->GetError();
        if(alErr == AL_NO_ERROR) try
        { // Buffer the data and throw exception if failed
          cOal->BufferData(uiBuffer, GetAudioFormat(), MemPtr<ALvoid>(),
            static_cast<ALsizei>(stFrameSize),
            static_cast<ALsizei>(GetSampleRate()));
          alErr = cOal->GetError();
          if(alErr != AL_NO_ERROR) throw "buffering";
          // Requeue the buffers and throw exception if failed
          sSource->QueueBuffer(uiBuffer);
          alErr = cOal->GetError();
          if(alErr != AL_NO_ERROR) throw "queuing";
          // We ate everything so set audio time and add to buffer
          dAudioBuffer += static_cast<ALdouble>(stFrameSize) /
            GetSampleRate() / GetChannels();
          // Play the source if the audio timer has started
          sSource->Play();
          // Try to parse more data
          break;
        } // Exception occured?
        catch(const char*const cpReason)
        { // Delete the buffers because of error
          ALL(cOal->DeleteBuffer(uiBuffer),
            "Video failed to delete buffer $ in '$' "
            "after failed data upload attempt!", uiBuffer, IdentGet());
          // Log the warning
          cLog->LogWarningExSafe("Video '$' $ audio failed "
            "(B:$;F:$<$$$>;A:$;S:$;R:$;AL:$<$$>)!",
            IdentGet(), cpReason, uiBuffer, GetFormatAsIdentifier(), hex,
            GetAudioFormat(), dec, MemPtr(), stFrameSize, GetSampleRate(),
            cOal->GetALErr(alErr), hex, alErr);
        } // Create buffers failed?
        else cLog->LogWarningExSafe("Video create buffers failed on '$' "
          "(F:$<$$$>;A:$;S:$;R:$;AL:$<$$>)!",
          IdentGet(), uiBuffer, GetFormatAsIdentifier(), hex, GetAudioFormat(),
          dec, MemPtr(), stFrameSize, GetSampleRate(), cOal->GetALErr(alErr),
          hex, alErr);
      } // No audio left so try to feed another packet and break if failed
      else switch(const int iR1 = ogg_stream_packetout(&ostsVorbis, &opkData))
      { // If a packet was assembled normally?
        case 1:
          // Synthesise it and compare result
          switch(const int iR2 = vorbis_synthesis(&vbData, &opkData))
          { // Success?
            case 0:
              // Submits vorbis_block for assembly into the final PCM audio
              if(vorbis_synthesis_blockin(&vdsData, &vbData) == OV_EINVAL)
                cLog->LogWarningExSafe(
                  "Video '$' vorbis synthesis failed!", IdentGet());
              // See if there is more audio
              break;
            // If the packet is not an audio packet?
            case OV_ENOTAUDIO: break;
            // if there was an error in the packet?
            case OV_EBADPACKET: break;
            // Unknown error
            default: XC("Unknown audio synthesis result!",
                        "Identifier", IdentGet(), "Result", iR2);
          } // See if there is more vorbis packets
          continue;
        // If we are out of sync and there is a gap in the data?
        case -1: break;
        // If there is insufficient data available to complete a packet?
        case 0: break;
        // Unknown error
        default: XC("Unknown audio stream packet out result!",
                    "Identifier", IdentGet(), "Result", iR1);
      } // Break loop
      break;
    } // Return parse result
    return bParsed;
  }
  /* -- Try to parse and render more Theora data --------------------------- */
  bool ParseAndRenderTheoraData(void)
  { // Theora frames were parsed?
    bool bParsed = false;
    // Repeat...
    do
    { // See if theres a new packet and get result
      switch(const int iR1 = ogg_stream_packetout(&ostsTheora, &opkData))
      { // if a packet was assembled normally?
        case 1:
          // Decode the packet and if we get a positive result?
          switch(const int iR2 =
            th_decode_packetin(tdcPtr, &opkData, &iVideoGranulePos))
          { // Success?
            case 0:
              // Need a scope for destructing upcoming sychronisation
              { // Wait until uploading is done
                const LockGuard lgWaitForUpload{ mUpload };
                // Get next frame to draw
                Frame &frFrame = faData[stFNext];
                // If decoding the frame failed?
                if(th_decode_ycbcr_out(tdcPtr, tybData))
                { // Skip the frame
                  frFrame.bDraw = false;
                  // We lost this frame
                  ++uiVideoFramesLost;
                } // Decoding succeeded?
                else
                { // Set pointers to planes and we will be drawing this frame
                  for(YCbCr &yccFrame : frFrame.yccaFrames)
                    yccFrame.tipP = tybData[yccFrame.stI];
                  frFrame.bDraw = true;
                  // We processed this frame
                  ++uiVideoFrames;
                } // Buffer filled
                stFNext = (stFNext + 1) % faData.size();
                ++stFWaiting;
                --stFFree;
              } // We processed a video frame
              bParsed = true;
              // Set next frome time and fall through to break
              CIAccumulate();
              // See if we can accumulate more packets
              continue;
            // Duplicate frame? We don't need to decode anything
            case TH_DUPFRAME:
              // We processed a video frame
              bParsed = true;
              // Update video time and increment frames counter
              ++uiVideoFrames;
              // Set next frame time and fall through to break
              CIAccumulate();
              // See if we can accumulate more packets
              continue;
            // Bad packet? (ignore)
            case TH_EBADPACKET: break;
            // Unknown error
            default: XC("Unknown video decode packet in result!",
                        "Identifier", IdentGet(), "Result", iR2);
          } // Check for more packets
          break;
        // We are out of sync and there is a gap in the data, try again
        case -1: continue;
        // There is insufficient data available to complete a packet
        case 0: break;
        // Unknown error
        default: XC("Unknown stream packet out result!",
                    "Identifier", IdentGet(), "Result", iR1);
      } // Break out of loop
      break;
    } // ...until the timer continues to trigger
    while(tThread.ThreadShouldNotExit() && CIIsTriggered());
    // Return parse result
    return bParsed;
  }
  /* -- Try to parse more raw data ----------------------------------------- */
  bool ParseRawData(void)
  { // Break apart file data to useful packets
    switch(const int iR = ogg_sync_pageout(&osysData, &opgData))
    { // A page was synced and returned.
      case 1: return true;
      // Stream has not yet captured sync (bytes were skipped).
      case -1: break;
      // More data needed or an internal error occurred.
      case 0: break;
      // Unknown error
      default: XC("Unknown sync page out result!",
                  "Identifier", IdentGet(), "Result", iR);
    } // Break loop
    return false;
  }
  /* -- Try to load more raw data ------------------------------------------ */
  bool LoadRawData(void)
  { // Is end of file?
    if(fmFile.FileMapIsEOF())
    { // Rewind video
      DoRewind();
      // We should loop?
      if(stLoop > 0)
      { // Reduce loops if not infinity
        if(stLoop != StdMaxSizeT) --stLoop;
        // Send looping event
        LuaEvtDispatch(VE_LOOP);
      } // No more loops? so set finished reason and exit the thread
      else
      { // Set finished
        ubReason = UB_FINISH;
        // We're not processing more data
        return true;
      }
    } // Load more data
    else if(DoIOBuffer())
      XCL("Read video data failed!", "Identifier", IdentGet());
    // There is more data
    return false;
  }
  /* -- Manage video decoding thread for ogg supporting only audio --------- */
  int VideoHandleAudioOnly(void)
  { // Process exhausted audio buffers if there is a source
    if(IsSourceAvailable()) ProcessExhaustedAudioBuffers();
    // If enough audio buffered and time is moving? Thread can breathe a little
    if(dAudioBuffer >= dAudBufMax && GetAudioTime() >= 0.0)
      cTimer->TimerSuspend(10);
    // Parse and render more vorbis data and if we didn't?
    else if(!ParseAndRenderVorbisData())
    { // Try to load more raw data and return if we're at the end of file
      if(LoadRawData()) return 2;
      // Repeat until there are more audio packets to parse
      while(tThread.ThreadShouldNotExit() && ParseRawData())
        ogg_stream_pagein(&ostsVorbis, &opgData);
    } // Done
    return 0;
  }
  /* -- Manage video decoding thread for ogg supporting only video --------- */
  int VideoHandleVideoOnly(void)
  { // If it is not time to process a frame yet?
    if(CIIsNotTriggered())
    { // Wait a little bit if we can
      if(CIIsNotTriggered(milliseconds{1})) cTimer->TimerSuspend(1);
    } // Decode and render new Theora data and if we did? Set new video time
    else if(ParseAndRenderTheoraData())
      dVideoTime = th_granule_time(tdcPtr, iVideoGranulePos);
    // No video rendered?
    else
    { // Try to load more raw data and return if at end of file
      if(LoadRawData()) return 2;
      // Repeat until there are more video packets to parse
      while(tThread.ThreadShouldNotExit() && ParseRawData())
        ogg_stream_pagein(&ostsTheora, &opgData);
    } // Keep thread loop alive
    return 0;
  }
  /* -- Manage video decoding thread for ogg supporting audio and video ---- */
  int VideoHandleAudioVideo(void)
  { // Stream status flags
    bool bVideoParsed = false, bAudioParsed = false;
    // If there is an audio source?
    if(IsSourceAvailable())
    { // Process exhausted audio buffers
      ProcessExhaustedAudioBuffers();
      // Raise pitch if behind
      if(GetDrift() > dMaxDrift) sSource->SetPitch(1.1f);
      // Lower pitch if ahead
      else if(GetDrift() < dMaxDriftNeg) sSource->SetPitch(0.9f);
      // No pitch adjustment required
      else sSource->SetPitch(1.0f);
    } // No source or buffer needs topping up? Repeat until we have audio
    if((IsSourceUnavailable() || dAudioBuffer < dAudBufMax) &&
      ParseAndRenderVorbisData())
        bAudioParsed = true;
    // Have theora stream and we've got enough audio buffered?
    if(dAudioBuffer >= dAudBufMax)
    { // If it is not time to process a frame yet?
      if(CIIsNotTriggered())
      { // We got audio? Update drift
        if(bAudioParsed)
          dDrift = GetAudioTime() > 0.0 ?
            GetVideoTime() - GetAudioTime() : 0.0;
        // Wait a little bit if we can
        else if(CIIsNotTriggered(milliseconds{1})) cTimer->TimerSuspend(1);
        // Done
        return 0;
      } // Time to check for new packets? Repeat...
      else if(ParseAndRenderTheoraData())
      { // Got a video packet
        bVideoParsed = true;
        // Update video position and drift
        dVideoTime = th_granule_time(tdcPtr, iVideoGranulePos);
        dDrift = GetAudioTime() > 0.0 ? GetVideoTime() - GetAudioTime() : 0.0;
      }
    } // Didn't process anything this time?
    if(!bAudioParsed && !bVideoParsed)
    { // Try to load more raw data and return if we're at the end of file
      if(LoadRawData()) return 2;
      // Repeat until there are more audio and video packets to parse
      while(tThread.ThreadShouldNotExit() && ParseRawData())
      { // Parse more video and audio data. No point logging failures.
        ogg_stream_pagein(&ostsTheora, &opgData);
        ogg_stream_pagein(&ostsVorbis, &opgData);
      }
    } // Keep thread loop alive
    return 0;
  }
  /* -- Thread main function ----------------------------------------------- */
  int VideoThreadMain(const Thread &tClass) try
  { // Send playing event if we're not temporarily de-initialising
    if(ubReason != UB_REINIT) LuaEvtDispatch(VE_PLAY);
    // Loop until thread should exit
    if(FlagIsSet(FL_THEORA|FL_VORBIS)) // Ogg has both audio and video streams?
      while(tClass.ThreadShouldNotExit() && !VideoHandleAudioVideo());
    else if(FlagIsSet(FL_VORBIS))      // Ogg has audio only stream?
      while(tClass.ThreadShouldNotExit() && !VideoHandleAudioOnly());
    else if(FlagIsSet(FL_THEORA))      // Ogg has video only stream?
      while(tClass.ThreadShouldNotExit() && !VideoHandleVideoOnly());
    // Log the reason why the thread should be terminated
    cLog->LogDebugExSafe("Video '$' main loop exit with reason $!",
      IdentGet(), ubReason.load());
    // Why did the video manager terminate?
    switch(ubReason.load())
    { // The thread was in standby? Shouldn't happen! Restart the thread
      case UB_STANDBY: [[fallthrough]];
      // The thread was blocking? Shouldn't happen! Restart the thread
      case UB_BLOCK: [[fallthrough]];
      // The video was playing? Shouldn't happen! Restart the thread
      case UB_PLAY: [[fallthrough]];
      // The video was re-initialised? Shouldn't happen! Restart the thread
      case UB_REINIT: [[fallthrough]];
      // The thread was terminated? Just break
      case UB_DATA: break;
      // The video finished playing? Send finish event
      case UB_FINISH: LuaEvtDispatch(VE_FINISH); break;
      // The video was stopped? Send stop event to guest
      case UB_STOP: LuaEvtDispatch(VE_STOP); break;
      // The video was paused? Send pause event to guest
      case UB_PAUSE: LuaEvtDispatch(VE_PAUSE); break;
    } // Exit thread cleanly with specified reason
    return 1;
  } // exception occured?
  catch(const exception &E)
  { // Report it to log
    cLog->LogErrorExSafe("(VIDEO THREAD EXCEPTION) $", E.what());
    // Failure exit code
    return -1;
  }
  /* -- Convert colour space to name --------------------------------------- */
  const string_view &ColourSpaceToString(const th_colorspace csId) const
    { return cVideos->csStrings.Get(csId); }
  /* -- Convert pixel format to name --------------------------------------- */
  const string_view &PixelFormatToString(const th_pixel_fmt pfId) const
    { return cVideos->pfStrings.Get(pfId); }
  /* -- Video properties ------------------------------------------- */ public:
  double GetVideoTime(void) const { return dVideoTime; }
  double GetAudioTime(void) const { return dAudioTime; }
  double GetDrift(void) const { return dDrift; }
  double GetFPS(void) const { return dFPS; }
  unsigned int GetFrame(void) const
    { return static_cast<unsigned int>(GetVideoTime() * GetFPS()); }
  unsigned int GetFrames(void) const { return uiVideoFrames; }
  unsigned int GetFramesSkipped(void) const { return uiVideoFramesLost; }
  th_pixel_fmt GetPixelFormat(void) const { return tiData.pixel_fmt; }
  th_colorspace GetColourSpace(void) const { return tiData.colorspace; }
  ogg_uint32_t GetFrameHeight(void) const { return tiData.frame_height; }
  ogg_uint32_t GetFrameWidth(void) const { return tiData.frame_width; }
  ogg_uint32_t GetHeight(void) const { return tiData.pic_height; }
  ogg_uint32_t GetWidth(void) const { return tiData.pic_width; }
  ogg_uint32_t GetOriginX(void) const { return tiData.pic_x; }
  ogg_uint32_t GetOriginY(void) const { return tiData.pic_y; }
  long GetSampleRate(void) const { return viData.rate; }
  int GetChannels(void) const { return viData.channels; }
  uint64_t GetLength(void) const { return fmFile.MemSize(); }
  bool IsSourceAvailable(void) const { return !!sSource; }
  bool IsSourceUnavailable(void) const { return !IsSourceAvailable(); }
  ALenum GetAudioFormat(void) const { return eFormat; }
  const string_view &GetFormatAsIdentifier(void) const
    { return cOal->GetALFormat(GetAudioFormat()); }
  /* -- When data has asynchronously loaded -------------------------------- */
  void AsyncReady(FileMap &fmData)
  { // Move filemap into ours
    fmFile.FileMapSwap(fmData);
    // Ok, Ogg parsing. The idea here is we have a bitstream that is made up of
    // Ogg pages. The libogg sync layer will find them for us. There may be
    // pages from several logical streams interleaved; we find the first theora
    // stream and ignore any others. Then we pass the pages for our stream to
    // the libogg stream layer which assembles our original set of packets out
    // of them. It's the packets that libtheora actually knows how to handle.
    // Start up Ogg stream synchronization layer
    ogg_sync_init(&osysData);
    // Init supporting Theora and vorbis structures needed in header parsing
    th_comment_init(&tcData);
    th_info_init(&tiData);
    vorbis_comment_init(&vcData);
    vorbis_info_init(&viData);
    // Which headers did we get?
    int iGotTheoraPage = 0, iGotVorbisPage = 0;
    // Finished buffering?
    bool bDone = false;
    // Loop.
    do
    { // This function takes the data stored in the buffer of the
      // ogg_sync_state struct and inserts them into an ogg_page. In an actual
      // decoding loop, this function should be called first to ensure that the
      // buffer is cleared. Caution:This function should be called before
      // reading into the buffer to ensure that data does not remain in the
      // ogg_sync_state struct. Failing to do so may result in a memory leak.
      switch(const int iPageOutResult = ogg_sync_pageout(&osysData, &opgData))
      { // Returned if more data needed or an internal error occurred.
        case 0:
        { // If we're end of file then it wasn't a valid stream
          if(fmFile.FileMapIsEOF())
            XC("Not a valid ogg/theora stream!", "Identifier", IdentGet());
          // Try to rebuffer more data and throw error if error reading
          if(DoIOBuffer())
            XC("Read ogg/theora stream error!",
               "Identifier", IdentGet(), "Reason", StrFromErrNo());
          // Done
          break;
        } // Indicated a page was synced and returned.
        case 1:
        { // Is this a mandated initial header? If not, stop parsing
          if(!ogg_page_bos(&opgData))
          { // don't leak the page; get it into the appropriate stream
            if(iGotTheoraPage) ogg_stream_pagein(&ostsTheora, &opgData);
            if(iGotVorbisPage) ogg_stream_pagein(&ostsVorbis, &opgData);
            // We've initialised successfully (Break parent loop)
            bDone = true;
            // Break all loops
            break;
          } // Init stream
          ogg_stream_state ossTest;
          ogg_stream_init(&ossTest, ogg_page_serialno(&opgData));
          ogg_stream_pagein(&ossTest, &opgData);
          ogg_stream_packetout(&ossTest, &opkData);
          // Try Theora. If it is theora -- save this stream state
          if(!iGotTheoraPage &&
            th_decode_headerin(&tiData, &tcData, &tsiPtr, &opkData) >= 0)
              { ostsTheora = ossTest; iGotTheoraPage = 1; }
          // Not Theora, try Vorbis. If it is vorbis -- save this stream state
          else if(!iGotVorbisPage &&
            vorbis_synthesis_headerin(&viData, &vcData, &opkData) == 0)
              { ostsVorbis = ossTest; iGotVorbisPage = 1; }
          // Whatever it is, we don't care about it
          else ogg_stream_clear(&ossTest);
        } // Returned if stream has not yet captured sync (bytes were skipped).
        case -1: break;
        // Done
        default: XC("Unknown OGG pageout result!",
                    "Identifier", IdentGet(), "Result", iPageOutResult);
                 break;
      } // fall through ostsTheora non-bos page parsing
    } // ...until done
    while(!bDone);
    // We're expecting more header packets
    while((iGotTheoraPage && iGotTheoraPage < 3) ||
          (iGotVorbisPage && iGotVorbisPage < 3))
    { // look for further theora headers
      while(iGotTheoraPage && iGotTheoraPage < 3)
      { // Is not a valid theora packet? Ignore. Might be a vorbis packet
        if(ogg_stream_packetout(&ostsTheora, &opkData) != 1) break;
        // decode the headers
        if(!th_decode_headerin(&tiData, &tcData, &tsiPtr, &opkData))
          XC("Error parsing Theora stream headers!",
             "Identifier", IdentGet());
        // Got the page
        ++iGotTheoraPage;
      } // Look for further vorbis headers
      while(iGotVorbisPage && iGotVorbisPage < 3)
      { // Is not a valid theora packet? Ignore. Might be a vorbis packet
        if(ogg_stream_packetout(&ostsVorbis, &opkData) != 1) break;
        // decode the headers
        if(vorbis_synthesis_headerin(&viData, &vcData, &opkData))
          XC("Error parsing Vorbis stream headers!",
             "Identifier", IdentGet());
        // Got the page
        ++iGotVorbisPage;
      }
      // The header pages/packets will arrive before anything else we
      // care about, or the stream is not obeying spec.
      if(ogg_sync_pageout(&osysData, &opgData) > 0)
      { // demux into the appropriate stream
        if(iGotTheoraPage) ogg_stream_pagein(&ostsTheora, &opgData);
        if(iGotVorbisPage) ogg_stream_pagein(&ostsVorbis, &opgData);
      } // Theora needs more data
      else if(DoIOBuffer())
        XC("EOF while searching for codec headers!",
           "Identifier", IdentGet(), "Position", fmFile.FileMapTell());
    } // Set flags of what headers we got
    if(iGotTheoraPage) FlagSet(FL_THEORA);
    if(iGotVorbisPage) FlagSet(FL_VORBIS);
    // And now we should have it all if not, well die
    if(FlagIsClear(FL_THEORA|FL_VORBIS))
      XC("Could not find a Theora and/or Vorbis stream!",
         "Identifier", IdentGet());
    // If there is a video stream?
    if(FlagIsSet(FL_THEORA))
    { // Parse the comments and then free the strings
      ssThMetaData =
        StdMove(PcmVorbisParseComments(tcData.user_comments, tcData.comments));
      th_comment_clear(&tcData);
      // Allocate a new one and throw error if not allocated
      tdcPtr = th_decode_alloc(&tiData, tsiPtr);
      if(!tdcPtr)
        XC("Error creating theora decoder context!",
           "Identifier", IdentGet());
      // Calculate FPS of video
      dFPS = static_cast<double>(tiData.fps_numerator) /
             static_cast<double>(tiData.fps_denominator);
      // Sanity check FPS
      if(GetFPS()<1 || GetFPS()>200)
        XC("Ambiguous frame rate in video!",
          "Identifier", IdentGet(), "FPS", GetFPS());
      // Get maximum texture size to match stored types
      const ogg_uint32_t uiMaxTexSize = cOgl->MaxTexSize<ogg_uint32_t>();
      // Make sure GPU can support texture size
      if(GetFrameWidth() > uiMaxTexSize || GetFrameHeight() > uiMaxTexSize)
        XC("The currently active graphics device does not support a "
           "texture size that would fit the video dimensions!",
           "Identifier", IdentGet(),       "Width", GetFrameWidth(),
           "Height",     GetFrameHeight(), "Limit", uiMaxTexSize);
      // Verify colour space
      switch(GetColourSpace())
      { // Valid colour spaces
        case TH_CS_UNSPECIFIED: [[fallthrough]];  // No colour content?
        case TH_CS_ITU_REC_470M: [[fallthrough]]; // NTSC content?
        case TH_CS_ITU_REC_470BG: break;          // PAL/SECAM content?
        // Invalid colour space
        default: XC("The specified colour space is unsupported!",
                    "Identifier", IdentGet(), "ColourSpace", GetColourSpace());
      } // Verify chroma subsampling type
      switch(GetPixelFormat())
      { // Valid chroma types
        case TH_PF_420: [[fallthrough]]; // YCbCr 4:2:0
        case TH_PF_422: [[fallthrough]]; // YCbCr 4:2:2
        case TH_PF_444: break;           // YCbCr 4:4:4
        // Invalid chroma type
        default: XC("Only 420, 422 or 444 pixel format is supported!",
                    "Identifier", IdentGet(), "PixelFormat", GetPixelFormat());
      } // Update frame immediately
      CISetLimit(1.0 / GetFPS());
    } // No Theora stream?
    else
    { // Force dummy 1x1 surfaces
      tiData.frame_width = tiData.frame_height =
        tiData.pic_width = tiData.pic_height = 1;
      // Dummy pixel format
      tiData.pixel_fmt = TH_PF_RSVD;
    } // If there is an audio stream?
    if(FlagIsSet(FL_VORBIS))
    { // Parse the comments and then free the strings
      ssVoMetaData =
        StdMove(PcmVorbisParseComments(vcData.user_comments, vcData.comments));
      vorbis_comment_clear(&vcData);
      // Make sure rate is sane
      if(GetSampleRate() < 1 || GetSampleRate() > 192000)
        XC("Video playback rate not valid at this time!",
           "Identifier", IdentGet(), "Rate", GetSampleRate());
      // Make sure channels are correct
      if(GetChannels() < 1 || GetChannels() > 2)
        XC("Video playback channel count of not supported!",
           "Identifier", IdentGet(), "Channels", GetChannels());
      // Initialise vorbis synthesis
      if(vorbis_synthesis_init(&vdsData, &viData))
        XC("Failed to initialise vorbis synthesis!", "Identifier", IdentGet());
      // Initialise vorbis block
      if(vorbis_block_init(&vdsData, &vbData))
        XC("Failed to initialise vorbis block!", "Identifier", IdentGet());
      // Calculate memory required for buffering audio and verify the size
      // since we don't know if someone's sending us dodgy bitrate values
      const double dMem =
        UtilNearestPow2<double>(UtilMaximum(viData.bitrate_upper,
          viData.bitrate_nominal) / 8);
      if(dMem <= 0.0 || dMem > 1048576.0)
        XC("Calculated erroneous memory size for audio buffer!",
          "Identifier", IdentGet(),      "Amount", dMem,
          "Upper", viData.bitrate_upper, "Nominal", viData.bitrate_nominal,
          "Lower", viData.bitrate_lower, "Window", viData.bitrate_window);
      // Allocate enough memory for audio buffer. We may not actually use all
      // the memory, but it is better than putting the alloc in the decoder
      // tick. We can reuse the 'Memory' class from the 'AsyncLoader' class.
      MemResize(static_cast<size_t>(dMem));
      // Show what we allocated
      cLog->LogDebugExSafe("Video pre-allocated $ bytes for audio decoder.",
        MemSize());
      // Reset audio position and drift
      dAudioTime = dDrift = 0.0;
    } // Log success
    cLog->LogInfoExSafe("Video loaded '$' successfully.", IdentGet());
    // Log debug inforation
    cLog->LogDebugExSafe(
      "- Version: $$.$.$.\n"         "- Serial: $ <0x$$$>.\n"
      "- Viewable size: $x$ <$>.\n"  "- Actual size: $x$ <$>.\n"
      "- Aspect ratio: $:$.\n"       "- Origin: $x$.\n"
      "- Pixel format: $ <$>.\n"     "- Colour space: $ <$>.\n"
      "- Frame rate: $ fps.\n"       "- Version: $.\n"
      "- Audio channels: $.\n"       "- Frequency: $ ($Hz).\n"
      "- Target bit rate: $ ($).\n"  "- Upper bit rate: $ ($).\n"
      "- Nominal bit rate: $ ($).\n" "- Lower bit rate: $ ($).\n"
      "- Bit window: $ ($).",
      fixed, static_cast<int>(tiData.version_major),
        static_cast<int>(tiData.version_minor),
        static_cast<int>(tiData.version_subminor),
      ostsTheora.serialno, hex, ostsTheora.serialno, dec,
      GetWidth(), GetHeight(), StrFromRatio(GetWidth(), GetHeight()),
      GetFrameWidth(), GetFrameHeight(),
        StrFromRatio(GetFrameWidth(), GetFrameHeight()),
      tiData.aspect_numerator, tiData.aspect_denominator,
      GetOriginX(), GetOriginY(),
      PixelFormatToString(GetPixelFormat()), GetPixelFormat(),
      ColourSpaceToString(GetColourSpace()), GetColourSpace(), GetFPS(),
      viData.version,
      GetChannels(),
      GetSampleRate(), StrToGrouped(GetSampleRate()),
      tiData.target_bitrate, StrToBits(tiData.target_bitrate),
      viData.bitrate_upper, StrToBits(viData.bitrate_upper),
      viData.bitrate_nominal, StrToBits(viData.bitrate_nominal),
      viData.bitrate_lower, StrToBits(viData.bitrate_lower),
      viData.bitrate_window, StrToBits(viData.bitrate_window));
    // Parse vorbis comments and if not empty? Enumerate and log each one
    if(cLog->HasLevel(LH_DEBUG))
    { // Prepare data for lists
      typedef pair<const string_view&, StrNCStrMap&> ListPair;
      typedef array<const ListPair, 2> Lists;
      static const string_view svTheora{ "Theora" }, svVorbis{ "Vorbis" };
      const Lists lLists{ { { svTheora, ssThMetaData },
                            { svVorbis, ssVoMetaData } } };
      // Write Theora comments iv available
      for(const ListPair lpPair : lLists)
      { // Ignore if empty else log metadata for specified list
        if(lpPair.second.empty()) continue;
        cLog->LogNLCDebugExSafe("- $ comments: $...",
          lpPair.first, lpPair.second.size());
        for(const StrNCStrMapPair &sncsmpPair : lpPair.second)
          cLog->LogNLCDebugExSafe("-- $: $.",
            sncsmpPair.first, sncsmpPair.second);
      }
    } // Set stopped
    FlagSet(FL_STOP);
  }
  /* -- Reinitialise frame buffer object and texture ----------------------- */
  void ReInitDisplayOutput(void) { FboReInit(); InitTexture(); }
  /* -- De-initialise texture and frame buffer object ---------------------- */
  void DeInitDisplayOutput(void) { DeInitTexture(); FboDeInit(); }
  /* -- Update volume ------------------------------------------------------ */
  void CommitVolume(void)
  { // Ignore if no source
    if(IsSourceUnavailable()) return;
    // Set volume
    sSource->SetGain(fAudioVolume * cSources->fVVolume * cSources->fGVolume);
  }
  /* -- Set volume --------------------------------------------------------- */
  void SetVolume(const ALfloat fVolume)
  { // Set volume
    fAudioVolume = fVolume;
    // Update volume
    CommitVolume();
  }
  /* -- Blit specific triangle --------------------------------------------- */
  void BlitTri(const size_t stTId) { FboActive()->FboBlitTri(*this, stTId); }
  /* -- Blit quad ---------------------------------------------------------- */
  void Blit(void) { FboActive()->FboBlit(*this); }
  /* -- Upload the texture -------read ------------------------------------- */
  void Render(void)
  { // Try to lock and return if failed or no frames waiting
    const UniqueLock ulWaitForProcessing{ mUpload, try_to_lock };
    if(!ulWaitForProcessing.owns_lock() || !stFWaiting) return;
    // Get frame
    Frame &frFrame = faData[stFActive];
    // Skip ahead frames if we need to catch up with audio
    // If we should draw?
    if(frFrame.bDraw)
    { // Upload texture data. This is quite safe because this data isnt
      // written to until the decoding routine thread has finished setting
      // these values.
      for(YCbCr &yccFrame : frFrame.yccaFrames)
      { // Bind the texture for this colour component
        cOgl->BindTexture(uiaYCbCr[yccFrame.stI]);
        // Get data
        th_img_plane &tipD = yccFrame.tipP;
        // Set unpack row length because of how the image data is formatted by
        // the vorbis api
        cOgl->SetUnpackRowLength(tipD.stride);
        // Now upload the image data to opengl, the memory is already
        // pre-allocated
        cOgl->UploadTextureSub(tipD.width, tipD.height, GL_RED, tipD.data);
      } // Reset unpack row length to default
      cOgl->SetUnpackRowLength(0);
      // Initialise Y texture id, active texture and shader program
      FboResetCache(uiaYCbCr[0], 0, shProgram->GetProgram());
      // Commit the Y setup and configure the U and V setup
      FboFinishAndReset(uiaYCbCr[1], 1, shProgram->GetProgram());
      FboFinishAndReset(uiaYCbCr[2], 2, shProgram->GetProgram());
      // Blit the YCbCr multi-texture into the fbo
      FboBlit(fboYCbCr, uiaYCbCr[2], 2, shProgram);
      // Commit the V texture and send everything to fbo list for rendering
      FboFinishAndRender();
      // No need to update again until decoder thread rendered another frame
      frFrame.bDraw = false;
    } // One less buffer to wait
    --stFWaiting;
    ++stFFree;
    stFActive = (stFActive + 1) % faData.size();
  }
  /* -- Video is playing? -------------------------------------------------- */
  bool IsPlaying(void) const { return tThread.ThreadIsRunning(); }
  /* -- DeInitialise audio ouput (because re-initialising) ----------------- */
  void DeInitAudio(void)
  { // Return if there is no audio in this video
    if(FlagIsClear(FL_VORBIS)) return;
    // Pause the video from re-initialisation
    Pause(UB_REINIT);
    // Audio buffers are empty
    dAudioBuffer = 0.0;
    // De-initialise the OpenAL segment
    StopAudioAndUnloadBuffers();
  }
  /* -- ReInitialise audio (because audio is restarting) ------------------- */
  void ReInitAudio(void)
  { // Return if there is no audio in this video
    if(FlagIsClear(FL_VORBIS)) return;
    // If there is an audio stream and we're re-initialising then resume play
    Play(UB_REINIT);
  }
  /* -- Stop and unload audio buffers -------------------------------------- */
  void StopAudioAndUnloadBuffers(void)
  { // Ignore if no source or no vorbis stream
    if(IsSourceUnavailable()) return;
    // Stop from playing so all buffers are unqueued and wait for stop
    // then unqueue and delete the buffer
    sSource->StopUnQueueAndDeleteAllBuffers();
    // Unlock the source so the source manager can recycle it
    sSource->Unlock();
    sSource = nullptr;
  }
  /* -- Do pause video ----------------------------------------------------- */
  void Pause(const Unblock ubNewReason = UB_PAUSE)
  { // Make sure playing flag is removed
    FlagClear(FL_PLAY);
    // Set exit reason
    ubReason = ubNewReason;
    // DeInit the thread, unblock the worker thread and stop and unload buffers
    tThread.ThreadStop();
  }
  /* -- Stop video and free everything ------------------------------------- */
  void Stop(const Unblock ubNewReason = UB_STOP)
  { // Ignore if already stopped
    if(FlagIsSet(FL_STOP)) return;
    FlagSet(FL_STOP);
    // Pause playback and synchronise
    Pause(ubNewReason);
    // Set the reason for stopping
    ubReason = ubNewReason;
    // De-initialise the OpenGL segment
    if(FlagIsSet(FL_GLINIT)) { DeInitTexture(); FboDeInit(); }
    // De-initialise the OpenAL segment
    StopAudioAndUnloadBuffers();
    // Audio buffers are empty
    dAudioBuffer = 0.0;
    // Reset buffer status
    stFActive = stFNext = stFWaiting = stFFree = stLoop = 0;
    // Rewind data stream
    DoRewindAndReset();
    // Log that the video was stopped
    cLog->LogDebugExSafe("Video '$' stopped with reason $.",
      IdentGet(), ubNewReason);
  }
  /* -- Advance a frame ---------------------------------------------------- */
  void Advance(void)
  { // If not playing
    if(tThread.ThreadIsNotExited()) return;
    // Loop until thread should exit
    if(FlagIsSet(FL_THEORA|FL_VORBIS)) // Ogg has both audio and video streams?
      while(!VideoHandleAudioVideo() && !stFWaiting);
    else if(FlagIsSet(FL_THEORA))      // Ogg has video only stream?
      while(!VideoHandleVideoOnly() && !stFWaiting);
  }
  /* -- Awaken the video --------------------------------------------------- */
  void Awaken(void)
  { // If surfaces not initialised? Note that even if the video does not
    // contain a Theora stream, we still want to initialise it anyway.
    if(FlagIsClear(FL_GLINIT))
    { // OpenGL output initialised
      FlagSet(FL_GLINIT);
      // Init fbo
      FboInit(IdentGet(), static_cast<GLsizei>(GetWidth()),
                          static_cast<GLsizei>(GetHeight()));
      FboSetMatrix(0.0f, 0.0f, 0.0f, 0.0f);
      FboSetTransparency(true);
      FboItemSetTexCoord(0.0f, 0.0f, 1.0f, 1.0f);
      // Clear the fbo, initially transparent and blue
      FboSetClearColour(0.0f, 0.0f, 1.0f, 0.0f);
      FboSetClear(false);
      // Only 2 triangles and 3 commands are needed so reserve the memory
      if(!FboReserve(2, 3))
        cLog->LogWarningSafe("Video failed to reserve memory for fbo lists.");
      // We must discard the extra garbage from the ogg video. We can do that
      // with the GPU very easily by altering texture coords!
      fboYCbCr.FboItemSetTexCoord(
        static_cast<GLfloat>(GetOriginX()) / GetFrameWidth(),
        static_cast<GLfloat>(GetOriginY() + GetHeight()) / GetFrameHeight(),
        static_cast<GLfloat>(GetOriginX() + GetWidth()) / GetFrameWidth(),
        static_cast<GLfloat>(GetOriginY()) / GetFrameHeight());
      // Init texture
      InitTexture();
    } // If theres a audio segment and AL portion is initialised?
    if(FlagIsSet(FL_VORBIS) && IsSourceUnavailable())
    { // Compare number of channels in file to set appropriate format. This is
      // here and not at the files init stage as it handles re-inits too and
      // the FP supported audio format flag have changed.
      switch(GetChannels())
      { // 1 channel mono?
        case 1: eFormat = cOal->Have32FPPB() ?
                  AL_FORMAT_MONO_FLOAT32 : AL_FORMAT_MONO16;
                break;
        // 2 channel stereo?
        case 2: eFormat = cOal->Have32FPPB() ?
                  AL_FORMAT_STEREO_FLOAT32 : AL_FORMAT_STEREO16;
                break;
        // Unknown channel count. Problem should already be handled at init.
        default: cLog->LogWarningExSafe("Video '$' audio playback failed. "
          "Invalid channel count of $!", IdentGet(), GetChannels()); return;
      } // Get a new sound source and if we got it update volume and return
      sSource = GetSource();
      if(IsSourceAvailable()) CommitVolume();
      // Tell log
      else cLog->LogWarningExSafe(
        "Video '$' audio playback failed. Out of sources!", IdentGet());
    }
  }
  /* -- Play video --------------------------------------------------------- */
  void Play(const Unblock ubNewReason = UB_PLAY)
  { // If playing flag is already set?
    if(FlagIsSet(FL_PLAY))
    { // Thread is not started?
      if(tThread.ThreadIsExited())
      { // Set reason for playing
        ubReason = ubNewReason;
        // Next frame can show immediately
        CISync();
        // Thread is stopped? Just start it again
        tThread.ThreadStart(this);
        // Log that the video was restarted
        cLog->LogDebugExSafe("Video '$' restarted with reason $.",
          IdentGet(), ubNewReason);
      } // Log that the command was ignored
      else cLog->LogWarningExSafe(
        "Video '$' play request with reason $ ignored!",
        IdentGet(), ubNewReason);
      // Done
      return;
    } // Check that OpenGL and OpenAL is initialised
    Awaken();
    // Set reason for playing
    ubReason = ubNewReason;
    // Next frame can show immediately
    CISync();
    // Set playing flag
    FlagSet(FL_PLAY);
    // Start decoding if we're still playing
    tThread.ThreadStart(this);
    // Log that the video was started
    cLog->LogDebugExSafe("Video '$' playing with reason $!",
      IdentGet(), ubNewReason);
  }
  /* -- Rewind video ------------------------------------------------------- */
  void Rewind(void)
  { // Ignore if already rewound
    if(iVideoGranulePos <= 0) return;
    // Rewind video to start
    DoRewindAndReset();
    // Log that the video was rewound
    cLog->LogDebugExSafe("Video '$' rewound!", IdentGet());
  }
  /* -- (De)Initialise video ouput ----------------------------------------- */
  void DeInitTexture(void)
  { // Delete the component textures
    GLL(cOgl->SetDeleteTextures(uiaYCbCr),
      "Failed to delete $ texture components", uiaYCbCr.size());
    // Clear texture names
    uiaYCbCr.fill(0);
  }
  /* -- Generate texture for specified video component --------------------- */
  void ConfigTexture(const GLuint uiTU, const GLsizei siW, const GLsizei siH)
  { // Get texture id
    GLuint &uiT = uiaYCbCr[uiTU];
    // Bind texture
    GL(cOgl->BindTexture(uiT), "Failed to bind video component texture!",
      "Identifier", IdentGet(), "Index", uiTU, "Texture", uiT);
    // Allocate VRAM for texture
    GL(cOgl->UploadTexture(0, siW, siH, GL_R8, GL_RED, nullptr),
      "Failed to reserve texture memory for image component!",
        "Identifier", IdentGet(), "Index",  uiTU, "Texture", uiT,
        "Width",      siW,        "Height", siH);
    // Set filtering. Only need to use GL_NEAREST because the FBO is the same
    // size as the video textures.
    GL(cOgl->SetTexParam(GL_TEXTURE_MIN_FILTER, GL_LINEAR),
      "Failed to set minification filter for video component!",
      "Identifier", IdentGet(), "Index", uiTU, "Texture", uiT);
    GL(cOgl->SetTexParam(GL_TEXTURE_MAG_FILTER, GL_LINEAR),
      "Failed to set magnification filter for video component!",
        "Identifier", IdentGet(), "Index", uiTU, "Texture", uiT);
    // Set to repeat and disable filter
    GL(cOgl->SetTexParam(GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT),
      "Failed to set texture wrapping S mode for video component!",
        "Identifier", IdentGet(), "Index", uiTU, "Texture", uiT);
    GL(cOgl->SetTexParam(GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT),
      "Failed to set texture wrapping T mode for video component!",
        "Identifier", IdentGet(), "Index", uiTU, "Texture", uiT);
    // Say what we did
    cLog->LogDebugExSafe("Video texture $x$ created for channel $.",
      siW, siH, uiTU);
  }
  /* -- Commit filter ------------------------------------------------------ */
  void CommitFilter(void)
    { FboSetFilterCommit(FlagIsSet(FL_FILTER) ? OF_L_L : OF_N_N); }
  /* -- Set filtering on video textures ------------------------------------ */
  void SetFilter(const bool bState)
  { // Update and commit filter
    FlagSetOrClear(FL_FILTER, bState);
    CommitFilter();
  }
  /* -- Looping functions -------------------------------------------------- */
  size_t GetLoop(void) const { return stLoop; }
  void SetLoop(const size_t stCount) { stLoop = stCount; }
  /* -- Colour key functions ----------------------------------------------- */
  bool GetKeyed(void) const
    { return FlagIsSet(FL_KEYED); }
  void UpdateShader(void)
    { shProgram = GetKeyed() ?
        &cShaderCore->sh3DYCbCrK : &cShaderCore->sh3DYCbCr; }
  void SetKeyed(const bool bState)
    { FlagSetOrClear(FL_KEYED, bState); UpdateShader(); }
  /* -- Generate component textures ---------------------------------------- */
  void InitTexture(void)
  { // Ignore if we don't have a opengl
    if(FlagIsClear(FL_GLINIT)) return;
    // Chosen divisors
    GLsizei siWDIV, siHDIV;
    // Set divisor based on pixel format
    switch(GetPixelFormat())
    { // YCbCr420p : Y = Full size; Cb/Cr = Half width and height
      case TH_PF_420: siWDIV = siHDIV = 2; break;
      // YCbCr422p : Y = Full size; Cb/Cr = Half width and full height
      case TH_PF_422: siWDIV = 2; siHDIV = 1; break;
      // YCbCr444p : Y = Full size; Cb/Cr = Full width and height
      case TH_PF_444: case TH_PF_RSVD: siWDIV = siHDIV = 1; break;
      // Unknown so throw error
      default: XC("Unknown theora pixel format!",
                  "Identifier", IdentGet(), "PixelFormat", GetPixelFormat());
    } // Create textures for Y/Cb/Cr multitexture
    GL(cOgl->CreateTextures(uiaYCbCr),
      "Failed to create texture components for YCbCr texture!",
      "Identifier", IdentGet(), "Components", uiaYCbCr.size());
    // Cast frame dimensions as they're 64-bit and not compatible with GL.
    const GLsizei siWidth = static_cast<GLsizei>(GetFrameWidth()),
                  siHeight = static_cast<GLsizei>(GetFrameHeight());
    // Configure the Y component (always full size)
    ConfigTexture(0, siWidth, siHeight);
    // Configure the Cb/Cr components
    for(GLuint uiIndex = 1; uiIndex <= 2; ++uiIndex)
      ConfigTexture(uiIndex, siWidth/siWDIV, siHeight/siHDIV);
    // Commit the current filter setting
    SetFilter(FlagIsSet(FL_FILTER));
    // Update choice of shader to use
    UpdateShader();
  }
  /* -- Destructor --------------------------------------------------------- */
  ~Video(void)
  { // Stop any pending async operations
    AsyncCancel();
    // Remove the registration now so it is no longer polled
    ICHelperVideo::CollectorUnregister();
    // Prevent more events being generated
    LuaEvtDeInit();
    // Set exit reason
    ubReason = UB_FINISH;
    // DeInit the thread, unblock the worker thread and stop and unload buffers
    tThread.ThreadStopNoThrow();
    // Stop and unload audio buffers
    StopAudioAndUnloadBuffers();
    // Deinit texture and reset parameters
    if(FlagIsSet(FL_GLINIT)) { DeInitTexture(); FboDeInit(); }
    // Free Theora decode and setup pointers
    if(tdcPtr) th_decode_free(tdcPtr);
    if(tsiPtr) th_setup_free(tsiPtr);
    // Clear theora and vorbis data. These are safe to call on static structs
    vorbis_comment_clear(&vcData);
    th_comment_clear(&tcData);
    ogg_stream_clear(&ostsTheora);
    ogg_stream_clear(&ostsVorbis);
    vorbis_block_clear(&vbData);
    vorbis_dsp_clear(&vdsData);
    ogg_sync_clear(&osysData);
    vorbis_info_clear(&viData);
    th_info_clear(&tiData);
  }
  /* -- Constructor -------------------------------------------------------- */
  Video(void) :
    /* -- Initialisers ----------------------------------------------------- */
    ICHelperVideo{ cVideos, this },    // Initialise collector class
    Fbo{ GL_RGBA8, false },            // Create unregistered opaque surface
    AsyncLoaderVideo{ *this, this,     // Initialise async loader
      EMC_MP_VIDEO },                  // ...video event code
    LuaEvtSlave{ this,                 // Initialise event handler
      EMC_VID_EVENT },                 // ...event handler code
    VideoFlags{ FL_NONE },             // No video flags just yet
    tThread{ "video", STP_HIGH,        // Initialise thread with high priority
      bind(&Video::VideoThreadMain,    // ...preset callback
        this, _1) },                   // ...class as parameter
    ubReason(UB_STANDBY),              // Initially set to blocked
    stLoop(0),                         // Initialise no loops remaining
    dDrift(0.0),                       // Initialise drift time
    dMaxDrift(cVideos->dMaxDrift),     // Initialise maximum drift time
    dMaxDriftNeg(-dMaxDrift),          // Initialise negated max drift time
    osysData{ /* Zeroed */ },          // Clear ogg sync state
    opgData{ /* Zeroed */ },           // Clear ogg page data
    opkData{ /* Zeroed */ },           // Clear ogg packet data
    stIOBuf(cVideos->stIOBufferSize),  // Initialise buffer length
    lIOBuf(static_cast<long>(stIOBuf)),// Initialise buffer length as long
    ostsTheora{ /* Zeroed */ },        // Clear ogg stream state
    tiData{ /* Zeroed */ },            // Clear Theora info struct
    tcData{ /* Zeroed */ },            // Clear Theora comment struct
    tsiPtr(nullptr),                   // Initialise Theora setup information
    tdcPtr(nullptr),                   // Initialise Theora decoder context
    tybData{ /* Zeroed */ },           // Clear Theora pixel colour buffers
    iVideoGranulePos(0),               // Initialise granule position
    dVideoTime(0.0),                   // Initialise position
    dFPS(0.0),                         // Initialise fps
    uiVideoFrames(0),                  // Initialise frames processed
    uiVideoFramesLost(0),              // Initialise frames lost
    stFActive(0),                      // initialise active frame id
    stFNext(0),                        // Initialise next frame id
    stFWaiting(0),                     // Initialise frames waiting
    stFFree{ faData.size() },          // Initialise free frames
    dAudBufMax(                        // Initialise maximum audio buffer size
      cVideos->dAudioBufferSize),      // ...with value set by user
    ostsVorbis{ /* Zeroed */ },        // Clear Vorbis stream status data
    viData{ /* Zeroed */ },            // Clear Vorbis decoder data
    vcData{ /* Zeroed */ },            // Clear Vorbis comment data
    vdsData{ /* Zeroed */ },           // Clear Vorbis DSP state data
    vbData{ /* Zeroed */ },            // Clear Vorbis decoder block data
    dAudioTime(0.0),                   // Initialise audio position
    dAudioBuffer(0.0),                 // Initialise audio buffer length
    fAudioVolume(1.0f),                // Initialise audio volume
    shProgram(nullptr),                // Initialise pointer to Shader used
    sSource(nullptr),                  // Initialise pointer to Source used
    eFormat(AL_NONE)                   // Initialise audio format type
    /* -- No code ---------------------------------------------------------- */
    { }
};/* -- End ---------------------------------------------------------------- */
CTOR_END_ASYNC_NOFUNCS(Videos, Video, VIDEO, VIDEO, // Finish collector class
  /* -- Initialisers ------------------------------------------------------- */
  LuaEvtMaster{ EMC_VID_EVENT },       // Init lua event async helper
  csStrings{{                          // Init colour space strings list
    "TH_CS_UNSPECIFIED",               // [0] Unspecified colour format
    "TH_CS_ITU_REC_470M",              // [1] NTSC television colour format
    "TH_CS_ITU_REC_470BG",             // [2] PAL television colour format
  }, "TH_CS_UNSUPPORTED" },            // End of colour space strings list
  pfStrings{{                          // Init pixel format strings list
    "TH_PF_420",                       // [0] 4:2:0 chroma subsampling scheme
    "TH_PF_RSVD",                      // [1] Dummy chroma subsampling scheme
    "TH_PF_422",                       // [2] 4:2:2 chroma subsampling scheme
    "TH_PF_444",                       // [3] 4:4:4 chroma subsampling scheme
  }, "TH_PF_UNSUPPORTED" },            // End of pixel format strings list
  dAudioBufferSize(0),                 // Audio buffer size init by cvar
  stIOBufferSize(0),                   // Buffer size initialised by cvar
  dMaxDrift(0.0)                       // Max drift initialised by cvar
)/* == Reinit textures (after engine thread shutdown) ====================== */
static void VideoReInitTextures(void)
{ // Ignore if no videos otherwise re-initialise ogl textures on all videos
  if(cVideos->empty()) return;
  cLog->LogDebugExSafe("Videos re-initialising $ video surfaces...",
    cVideos->CollectorCountUnsafe());
  for(Video*const vVideo : *cVideos) vVideo->ReInitDisplayOutput();
  cLog->LogDebugExSafe("Videos re-initialised $ video surfaces!",
    cVideos->CollectorCountUnsafe());
}
/* == De-init video textures (after thread shutdown) ======================= */
static void VideoDeInitTextures(void)
{ // Ignore if no videos otherwise de-initialise ogl textures on all videos
  if(cVideos->empty()) return;
  cLog->LogDebugExSafe("Videos de-initialising $ video surfaces...",
    cVideos->CollectorCountUnsafe());
  for(Video*const vVideo : *cVideos) vVideo->DeInitDisplayOutput();
  cLog->LogDebugExSafe("Videos de-initialised $ video surfaces!",
    cVideos->CollectorCountUnsafe());
}
/* == Clear event callbacks on all videos (must be synchronised) =========== */
static void VideoClearEvents(void)
{ // Lock access to video collector list and clear all video events
  const LockGuard lgVideosSync{ cVideos->CollectorGetMutex() };
  if(cVideos->empty()) return;
  cLog->LogDebugExSafe("Videos clearing events from $ video objects...",
    cVideos->CollectorCountUnsafe());
  for(Video*const vVideo : *cVideos) vVideo->LuaEvtDeInit();
  cLog->LogDebugExSafe("Videos cleared events from $ video objects!",
    cVideos->CollectorCountUnsafe());
}
/* == Stop all videos (must be sychronised) ================================ */
static void VideoStop(void)
{ // Lock access to video collector list and stop all videos
  const LockGuard lgVideosSync{ cVideos->CollectorGetMutex() };
  for(Video*const vVideo : *cVideos) vVideo->Stop();
}
/* == DeInit all videos (after engine thread shutdown) ===================== */
static void VideoDeInit(void)
{ // Ignore if no videos otherwise de-initialise oal buffers on all videos
  if(cVideos->empty()) return;
  cLog->LogDebugExSafe("Videos de-initialising $ videos...",
    cVideos->CollectorCountUnsafe());
  for(Video*const vVideo : *cVideos) vVideo->DeInitAudio();
  cLog->LogDebugExSafe("Videos de-initialised $ videos!",
    cVideos->CollectorCountUnsafe());
}
/* == ReInit all videos (after engine thread shutdown) ===================== */
static void VideoReInit(void)
{ // Ignore if no videos otherwise re-initialise oal buffers on all videos
  if(cVideos->empty()) return;
  cLog->LogDebugExSafe("Videos re-initialising $ videos...",
    cVideos->CollectorCountUnsafe());
  for(Video*const vVideo : *cVideos) vVideo->ReInitAudio();
  cLog->LogDebugExSafe("Videos re-initialised $ videos!",
    cVideos->CollectorCountUnsafe());
}
/* == Render all videos ==================================================== */
static void VideoRender(void)
  { for(Video*const vVideo : *cVideos) vVideo->Render(); }
/* == Update all streams base volume ======================================= */
static void VideoCommitVolume(void)
  { for(Video*const vVideo : *cVideos) vVideo->CommitVolume(); }
/* == Set buffer size ====================================================== */
static CVarReturn VideoSetIOBufferSize(const size_t stSize)
  { return CVarSimpleSetIntNLG(cVideos->stIOBufferSize,
      stSize, 4096UL, 16777216UL); }
/* == Set drift length maximum ============================================= */
static CVarReturn VideoSetMaximumDrift(const double dMax)
  { return CVarSimpleSetIntNLG(cVideos->dMaxDrift, dMax, 0.01, 1.0); }
/* == Set audio buffer length maximum ====================================== */
static CVarReturn VideoSetAudioBufferSize(const double dMax)
  { return CVarSimpleSetIntNLG(cVideos->dAudioBufferSize,
      dMax, 0.01, 1.0); }
/* == Set all streams base volume ========================================== */
static CVarReturn VideoSetVolume(const ALfloat fVolume)
{ // Ignore if invalid value
  if(fVolume < 0.0f || fVolume > 1.0f) return DENY;
  // Store volume (SOURCES class keeps it)
  cSources->fVVolume = fVolume;
  // Update volumes
  VideoCommitVolume();
  // Success
  return ACCEPT;
}
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
