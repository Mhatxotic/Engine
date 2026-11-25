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
using namespace IClock::P;             using namespace ICodecOGG::P;
using namespace ICollector::P;         using namespace ICVarDef::P;
using namespace IError::P;             using namespace IEvtMain::P;
using namespace IFbo::P;               using namespace IFileMap::P;
using namespace IFlags::P;             using namespace IFrame::P;
using namespace IGCHelper::P;          using namespace IInterval::P;
using namespace ILog::P;               using namespace ILookupArray::P;
using namespace ILuaEvt::P;            using namespace ILuaIdent::P;
using namespace ILuaLib::P;            using namespace ILuaUtil::P;
using namespace IMemory::P;            using namespace IMixer::P;
using namespace IMutex::P;             using namespace IName::P;
using namespace IOal::P;               using namespace IOgl::P;
using namespace IPcmLib::P;            using namespace IShader::P;
using namespace IShaders::P;           using namespace ISource::P;
using namespace IStd::P;               using namespace IStream::P;
using namespace IString::P;            using namespace ISysUtil::P;
using namespace IThread::P;            using namespace IUtil::P;
using namespace LOgg;                  using namespace LOgg::Theora;
using namespace LOal::Types;           using namespace LOgl::Types;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* -- Video collector class for collector data and custom variables -------- */
CTOR_BEGIN_ASYNC(Videos, Video, CLHelperSafe,
/* -- Public variables ----------------------------------------------------- */
using CSStrings = LookupArray<TH_CS_NSPACES>;
const CSStrings    csStrings;          // Colour space strings list
using PFStrings = LookupArray<TH_PF_NFORMATS>;
const PFStrings    pfStrings;          // Pixel format strings list
double             dAudioBufferSize;   // Default audio buffer size
size_t             stIOBufferSize;     // Default IO buffer size
double             dMaxDrift;,,        // Maximum drift before drop frames
/* -- Derived classes ------------------------------------------------------ */
private LuaEvtMaster<Video, LuaEvtTypeParam<Video>>); // Lua event
/* ------------------------------------------------------------------------- */
BUILD_FLAGS(unsigned, Video,           // Video flags
  /* ----------------------------------------------------------------------- */
  FL_NONE              {VideoFlag[0]}, // No flags set?
  FL_THEORA            {VideoFlag[1]}, // Have a theora stream?
  FL_VORBIS            {VideoFlag[2]}, // Have a vorbis stream?
  FL_GLINIT            {VideoFlag[3]}, // Video output initialised?
  FL_KEYED             {VideoFlag[4]}, // Video is keyed?
  FL_FDR               {VideoFlag[5]}, // Video is full dynamic range?
  FL_REC709            {VideoFlag[6]}, // Video is Rec.709 colour space?
  FL_FILTER            {VideoFlag[7]}, // Filtering is enabled?
  FL_STOP              {VideoFlag[8]}, // Hard stopped?
  FL_PLAY              {VideoFlag[9]}, // Video is playing?
  FL_RESUME           {VideoFlag[10]}, // Play after reinit?
  /* -- Combined flags ----------------------------------------------------- */
  FL_THEORAVORBIS { FL_THEORA|FL_VORBIS }
);/* ======================================================================= */
CTOR_MEM_BEGIN_ASYNC(Videos, Video, ICHelperSafe, /* No CLHelper */),
  /* -- Base classes ------------------------------------------------------- */
  public Fbo,                          // Video file name
  public AsyncLoaderVideo,             // Asynchronous laoding of videos
  public LuaEvtSlave<Video>,           // Lua asynchronous events
  public VideoFlags,                   // Video settings flags
  private Interval,                    // Frame playback timing helper
  private MutexLock                    // mutex for uploading data
{ /* -- Typedefs ----------------------------------------------------------- */
  struct YCbCr                         // Y/Cb/Cr plane data
  { /* -- Public variables ------------------------------------------------- */
    const size_t   stI;                // Unique index
    th_img_plane   tipP;               // Plane data
    /* -- Clear img plane struct (quicker than memset) --------------------- */
    void Reset()
      { tipP.width = tipP.height = tipP.stride = 0; tipP.data = nullptr; }
    /* -- Constructor that initialises id ---------------------------------- */
    explicit YCbCr(const size_t stNIndex) :
      /* -- Initialisers --------------------------------------------------- */
      stI(stNIndex),                   // Set unique id
      tipP{0, 0, 0, nullptr}           // Initialise frame data
      /* -- No code -------------------------------------------------------- */
      {}
  };/* --------------------------------------------------------------------- */
  struct Frame                         // Theora frame data
  { /* -- Public variables ------------------------------------------------- */
    bool           bDraw;              // Draw this frame?
    using YCCArray = StdArray<YCbCr, 3>; // Room for three frames
    YCCArray       yccaFrames;         // The planes (Y, Cb and Cr);
    /* -- Clear everything ------------------------------------------------- */
    void Reset()
    { // Do not draw to OpenGL
      bDraw = false;
      // Clear frame data
      for(YCbCr &yccFrame : yccaFrames) yccFrame.Reset();
    }
    /* -- Constructor that initialises frame data -------------------------- */
    Frame() :
      /* -- Initialisers --------------------------------------------------- */
      bDraw(false),                    // Set frame not ready for drawing
      yccaFrames{ YCbCr{0}, YCbCr{1},  // Initialise Y/Cb/Cr frame data
                  YCbCr{2} }
      /* -- No code -------------------------------------------------------- */
      {}
  };/* -- Theora frame data (double-buffered) ------------------------------ */
  using FrameArray = StdArray<Frame, 2>;
  /* -- Y, Cb and Cr texture ids ------------------------------------------- */
  using GLuintArray = StdArray<GLuint, 3>;
  /* -- Thread unblock reasons ------------------------------------- */ public:
  enum Unblock : unsigned
    { UB_STANDBY, UB_BLOCK, UB_DATA,  UB_REINIT,
      UB_PLAY,    UB_STOP,  UB_PAUSE, UB_FINISH };
  using AtomicUnblock = StdAtomic<Unblock>;
  /* -- Lua callback event ids --------------------------------------------- */
  enum Event : unsigned { VE_PLAY, VE_LOOP, VE_STOP, VE_PAUSE, VE_FINISH };
  /* -- Concurrency -------------------------------------------------------- */
  AtomicUnblock    auReason;           // Unlock condition variable
  AtomicSizeT      astLoop;            // Loops count
  double           dDrift;             // Drift between audio and video
  const double     dMaxDrift,          // Maximum allowed drift
                   dMaxDriftNeg;       // Maximum allowed drift (negative
  AtomicBool       abPause;            // Only pause the stream?
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
  th_ycbcr_buffer  tybPlanes;          // Theora colour buffer
  StdSpan<th_img_plane> sPlData;       // Protect 'tybPlanes' from raw access
  ogg_int64_t      llVideoGranulePos;  // Video granule position
  AtomicDouble     adVideoTime;        // Video time index
  double           dFPS;               // Video fps
  AtomicUnsigned   auVideoFrames,      // Frames rendered
                   auVideoFramesLost;  // Frames skipped
  FrameArray       faData;             // Frame data
  size_t           stFActive,          // Currently active frame
                   stFNext,            // Next frame to process
                   stFWaiting;         // Frames waiting to be processed
  AtomicSizeT      astFFree;           // Frames free to be processed
  StrNCStrMap      sncsmThMetaData;    // Theora comments block
  /* -- Vorbis ------------------------------------------------------------- */
  ogg_stream_state ostsVorbis;         // Ogg (Vorbis) stream states
  vorbis_info      viData;             // Vorbis decoder info
  vorbis_comment   vcData;             // Vorbis comment block
  vorbis_dsp_state vdsData;            // Vorbis DSP state
  vorbis_block     vbData;             // Vorbis decoder block
  AtomicDouble     adAudioTime;        // Audio time index
  StrNCStrMap      sncsmVoMetaData;    // Vorbis comments block
  /* -- OpenGL ------------------------------------------------------------- */
  FboItem          fiYCbCr;            // Blit data for actual YCbCr components
  GLuintArray      gluaYCbCr;          // Texture id's for YCbCr components
  Shader          *shpProgram;         // Shader program to use
  /* -- OpenAL ------------------------------------------------------------- */
  Source          *soSource;           // Source class
  ALenum           aleFormat;          // Internal format
  ALfloat          alfAudioVolume;     // Audio volume
  const ALdouble   aldAudBufMax;       // Audio buffer size
  ALdouble         aldAudioBuffer;     // Audio buffered
  /* == Buffer more data for OGG decoder ========================== */ private:
  bool VideoDoIOBuffer()
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
  void VideoDoRewind()
  { // Rewind video to start
    fmFile.FileMapRewind();
    // Tell theora we reset the video position
    if(FlagIsSet(FL_THEORA))
      VideoSetParameter<ogg_int64_t>(TH_DECCTL_SET_GRANPOS, 0);
    // Reset Vorbis dsp as required by documentation
    if(FlagIsSet(FL_VORBIS)) vorbis_synthesis_restart(&vdsData);
  }
  /* -- Rewind the theora stream and reset variables ----------------------- */
  void VideoDoRewindAndReset()
  { // Rewind video to start
    VideoDoRewind();
    // Reset granule position and frames rendered
    llVideoGranulePos = 0;
    auVideoFrames = auVideoFramesLost = 0;
    // Reset counters
    adVideoTime = adAudioTime = dDrift = aldAudioBuffer = 0.0;
  }
  /* -- Set theora decoder control ----------------------------------------- */
  template<typename IntType = int>
    requires StdIsIntegral<IntType>
  int VideoSetParameter(const int iVariable, IntType itValue)
    { return th_decode_ctl(tdcPtr, iVariable,
        reinterpret_cast<void*>(&itValue), sizeof(itValue)); }
  /* -- Get theora decoder value ------------------------------------------- */
  template<typename IntType = int>
    requires StdIsIntegral<IntType>
  IntType VideoGetParameter(const int iVariable) const
    { return static_cast<IntType>(
        th_decode_ctl(tdcPtr, iVariable, nullptr, 0)); }
  /* -- Process exhausted audio buffers ------------------------------------ */
  void VideoProcessExhaustedAudioBuffers()
  { // Get number of buffers queued
    for(ALsizei alsiP = soSource->SourceGetBuffersProcessed(); alsiP; --alsiP)
    { // Unqueue a buffer and break if failed
      const ALuint aluBuffer = soSource->SourceUnQueueBuffer();
      if(cOal->OalHaveError()) continue;
      // Remove buffer time
      aldAudioBuffer = UtilMaximum(aldAudioBuffer -
        (static_cast<ALdouble>
             (cOal->OalGetBufferInt<ALint>(aluBuffer, AL_SIZE)) /
           static_cast<ALdouble>(VideoGetSampleRate()) /
           static_cast<ALdouble>(VideoGetChannels())), 0.0);
      // Delete the buffer that was returned continue if successful
      ALL(cOal->OalDeleteBuffer(aluBuffer),
        "Video failed to delete unqueued buffer $ in '$'!",
        aluBuffer, NameGet());
    }
  }
  /* -- Try to parse and render more Vorbis data --------------------------- */
  bool VideoParseAndRenderVorbisData()
  { // Data was parsed?
    bool bParsed = false;
    // Repeat until break or thread should exit
    while(AsyncGetThread().ThreadShouldNotExit())
    { // If frames are available, but we're way behind the video?
      ALfloat **alfpaPCM;
      if(const int iFrames = vorbis_synthesis_pcmout(&vdsData, &alfpaPCM))
      { // Tell vorbis how much we read
        vorbis_synthesis_read(&vdsData, iFrames);
        // Set audio time
        adAudioTime = vorbis_granule_time(&vdsData, vdsData.granulepos);
        // Set that we got audio
        bParsed = true;
        // No need to do anymore if there is no source
        if(VideoIsSourceUnavailable()) break;
        // Convert vorbis frames to correct type dealing with memory
        const size_t stFrames = static_cast<size_t>(iFrames),
          // Get number of channels as size_t
          stChannels = static_cast<size_t>(VideoGetChannels());
        // Length of data (may need to be modified if 16-bit required)
        size_t stFrameSize = sizeof(float) * stFrames * stChannels;
        // If the hardware supports 32-bit (4b) floating point playback?
        if(cOal->OalHave32FPPB())
        { // Allocate required memory and do the conversion
          MemResizeUp(stFrameSize);
          cCodecOGG->F32FromVorbisFrames(alfpaPCM, stFrames, stChannels,
            MemPtr<ALfloat>());
        } // If the hardware only supports 16-bit (2b) integer playback?
        else
        { // Allocate half the required memory and do the conversion. Note
          // that we can't optimise this var and use MemSize() because this
          // memory buffer might be bigger than we need and would be a waste
          // of CPU time to keep reallocating it hence the MemResizeUp().
          stFrameSize >>= 1;
          MemResizeUp(stFrameSize);
          cCodecOGG->I16FromVorbisFrames(alfpaPCM, stFrames, stChannels,
            MemPtr<ALshort>());
        } // Generate a buffer for the pcm data and if succeeded?
        const ALuint aluBuffer = cOal->OalCreateBuffer();
        ALenum aleErr = cOal->OalGetError();
        if(aleErr == AL_NO_ERROR) try
        { // Buffer the data and throw exception if failed
          cOal->OalBufferData(aluBuffer, VideoGetAudioFormat(),
            MemPtr<ALvoid>(), static_cast<ALsizei>(stFrameSize),
            static_cast<ALsizei>(VideoGetSampleRate()));
          aleErr = cOal->OalGetError();
          if(aleErr != AL_NO_ERROR) throw "buffering";
          // Requeue the buffers and throw exception if failed
          soSource->SourceQueueBuffer(aluBuffer);
          aleErr = cOal->OalGetError();
          if(aleErr != AL_NO_ERROR) throw "queuing";
          // We ate everything so set audio time and add to buffer
          aldAudioBuffer += static_cast<ALdouble>(stFrameSize) /
            static_cast<ALdouble>(VideoGetSampleRate()) /
            static_cast<ALdouble>(VideoGetChannels());
          // Play the source if the audio timer has started
          soSource->SourcePlay();
          // Try to parse more data
          break;
        } // Exception occured?
        catch(const char*const cpReason)
        { // Delete the buffers because of error
          ALL(cOal->OalDeleteBuffer(aluBuffer),
            "Video failed to delete buffer $ in '$' "
            "after failed data upload attempt!", aluBuffer, NameGet());
          // Log the warning
          cLog->LogWarningExSafe("Video '$' $ audio failed "
            "(B:$;F:$<$$$>;A:$;S:$;R:$;AL:$<$$>)!",
            NameGet(), cpReason, aluBuffer, VideoGetFormatAsIdentifier(),
            StdIOSHex, VideoGetAudioFormat(), StdIOSDec, MemPtr(), stFrameSize,
            VideoGetSampleRate(), cOal->OalGetALErr(aleErr), StdIOSHex,
            aleErr);
        } // Create buffers failed?
        else cLog->LogWarningExSafe("Video create buffers failed on '$' "
          "(F:$<$$$>;A:$;S:$;R:$;AL:$<$$>)!",
          NameGet(), aluBuffer, VideoGetFormatAsIdentifier(), StdIOSHex,
          VideoGetAudioFormat(), StdIOSDec, MemPtr(), stFrameSize,
          VideoGetSampleRate(), cOal->OalGetALErr(aleErr), StdIOSHex, aleErr);
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
                  "Video '$' vorbis synthesis failed!", NameGet());
              // See if there is more audio
              break;
            // If the packet is not an audio packet?
            case OV_ENOTAUDIO: break;
            // if there was an error in the packet?
            case OV_EBADPACKET: break;
            // Unknown error
            default: XC("Unknown audio synthesis result!",
                       "Name", NameGet(), "Result", iR2);
          } // See if there is more vorbis packets
          continue;
        // If we are out of sync and there is a gap in the data?
        case -1: break;
        // If there is insufficient data available to complete a packet?
        case 0: break;
        // Unknown error
        default: XC("Unknown audio stream packet out result!",
                   "Name", NameGet(), "Result", iR1);
      } // Break loop
      break;
    } // Return parse result
    return bParsed;
  }
  /* -- Try to parse and render more Theora data --------------------------- */
  bool VideoParseAndRenderTheoraData()
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
            th_decode_packetin(tdcPtr, &opkData, &llVideoGranulePos))
          { // Success?
            case 0:
              // Prevent uploading while we modify the upload data
              MutexCall([this](){
                // Get next frame to draw
                Frame &frFrame = faData[stFNext];
                // If decoding the frame failed?
                if(th_decode_ycbcr_out(tdcPtr, tybPlanes))
                { // Skip the frame
                  frFrame.bDraw = false;
                  // We lost this frame
                  ++auVideoFramesLost;
                } // Decoding succeeded?
                else
                { // Set pointers to planes and we will be drawing this frame
                  for(YCbCr &yccFrame : frFrame.yccaFrames)
                    yccFrame.tipP = sPlData[yccFrame.stI];
                  frFrame.bDraw = true;
                  // We processed this frame
                  ++auVideoFrames;
                } // Buffer filled
                stFNext = (stFNext + 1) % faData.size();
                ++stFWaiting;
                --astFFree;
              });
              // We processed a video frame
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
              ++auVideoFrames;
              // Set next frame time and fall through to break
              CIAccumulate();
              // See if we can accumulate more packets
              continue;
            // Bad packet? (ignore)
            case TH_EBADPACKET: break;
            // Unknown error
            default: XC("Unknown video decode packet in result!",
                       "Name", NameGet(), "Result", iR2);
          } // Check for more packets
          break;
        // We are out of sync and there is a gap in the data, try again
        case -1: continue;
        // There is insufficient data available to complete a packet
        case 0: break;
        // Unknown error
        default: XC("Unknown stream packet out result!",
                   "Name", NameGet(), "Result", iR1);
      } // Break out of loop
      break;
    } // ...until the timer continues to trigger
    while(AsyncGetThread().ThreadShouldNotExit() && CIIsTriggered());
    // Return parse result
    return bParsed;
  }
  /* -- Try to parse more raw data ----------------------------------------- */
  bool VideoParseRawData()
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
                 "Name", NameGet(), "Result", iR);
    } // Break loop
    return false;
  }
  /* -- Try to load more raw data ------------------------------------------ */
  bool LoadRawData()
  { // Is end of file?
    if(fmFile.FileMapIsEOF())
    { // Rewind video
      VideoDoRewind();
      // We should loop?
      if(astLoop > 0)
      { // Reduce loops if not infinity then send looping event
        if(astLoop != StdMaxSizeT) --astLoop;
        LuaEvtDispatch(VE_LOOP);
      } // No more loops? so set finished reason and exit the thread
      else
      { // Set finished. We're not processing more data
        auReason = UB_FINISH;
        return true;
      }
    } // Load more data and throw exception if failed
    else if(VideoDoIOBuffer())
      XCL("Read video data failed!", "Name", NameGet());
    // There is more data
    return false;
  }
  /* -- Manage video decoding thread for ogg supporting only audio --------- */
  bool VideoHandleAudioOnly()
  { // Process exhausted audio buffers if there is a source
    if(VideoIsSourceAvailable()) VideoProcessExhaustedAudioBuffers();
    // If enough audio buffered and time is moving? Thread can breathe a little
    if(aldAudioBuffer >= aldAudBufMax && VideoGetAudioTime() >= 0.0)
      StdSuspend(cd10MS);
    // Parse and render more vorbis data and if we didn't?
    else if(!VideoParseAndRenderVorbisData())
    { // Try to load more raw data and return if we're at the end of file
      if(LoadRawData()) return false;
      // Repeat until there are more audio packets to parse
      while(AsyncGetThread().ThreadShouldNotExit() && VideoParseRawData())
        ogg_stream_pagein(&ostsVorbis, &opgData);
    } // Done
    return true;
  }
  /* -- Manage video decoding thread for ogg supporting only video --------- */
  bool VideoHandleVideoOnly()
  { // If it is not time to process a frame yet?
    if(CIIsNotTriggered())
    { // Wait a little bit if we can
      if(CIIsNotTriggeredIn(cd1MS)) StdSuspend();
    } // Decode and render new Theora data and if we did? Set new video time
    else if(VideoParseAndRenderTheoraData())
      adVideoTime = th_granule_time(tdcPtr, llVideoGranulePos);
    // No video rendered?
    else
    { // Try to load more raw data and return if at end of file
      if(LoadRawData()) return false;
      // Repeat until there are more video packets to parse
      while(AsyncGetThread().ThreadShouldNotExit() && VideoParseRawData())
        ogg_stream_pagein(&ostsTheora, &opgData);
    } // Keep thread loop alive
    return true;
  }
  /* -- Manage video decoding thread for ogg supporting audio and video ---- */
  bool VideoHandleAudioVideo()
  { // Stream status flags
    bool bVideoParsed = false, bAudioParsed = false;
    // If there is an audio source?
    if(VideoIsSourceAvailable())
    { // Process exhausted audio buffers
      VideoProcessExhaustedAudioBuffers();
      // Raise pitch if behind
      if(VideoGetDrift() > dMaxDrift) soSource->SourceSetPitch(1.1f);
      // Lower pitch if ahead
      else if(VideoGetDrift() < dMaxDriftNeg) soSource->SourceSetPitch(0.9f);
      // No pitch adjustment required
      else soSource->SourceSetPitch(1.0f);
    } // No source or buffer needs topping up? Repeat until we have audio
    if((VideoIsSourceUnavailable() || aldAudioBuffer < aldAudBufMax) &&
      VideoParseAndRenderVorbisData())
        bAudioParsed = true;
    // Have theora stream and we've got enough audio buffered?
    if(aldAudioBuffer >= aldAudBufMax)
    { // If it is not time to process a frame yet?
      if(CIIsNotTriggered())
      { // We got audio? Update drift
        if(bAudioParsed)
          dDrift = VideoGetAudioTime() > 0.0 ?
            VideoGetTime() - VideoGetAudioTime() : 0.0;
        // Wait a little bit if we can
        else if(CIIsNotTriggeredIn(cd1MS)) StdSuspend();
        // Done, keep thread alive
        return true;
      } // Time to check for new packets? Repeat...
      else if(VideoParseAndRenderTheoraData())
      { // Got a video packet and update video position and drift
        bVideoParsed = true;
        adVideoTime = th_granule_time(tdcPtr, llVideoGranulePos);
        dDrift = VideoGetAudioTime() > 0.0 ?
          VideoGetTime() - VideoGetAudioTime() : 0.0;
      }
    } // Didn't process anything this time?
    if(!bAudioParsed && !bVideoParsed)
    { // Try to load more raw data and return if we're at the end of file
      if(LoadRawData()) return false;
      // Repeat until there are more audio and video packets to parse
      while(AsyncGetThread().ThreadShouldNotExit() && VideoParseRawData())
      { // Parse more video and audio data. No point logging failures.
        ogg_stream_pagein(&ostsTheora, &opgData);
        ogg_stream_pagein(&ostsVorbis, &opgData);
      }
    } // Keep thread loop alive
    return true;
  }
  /* -- Thread main function ----------------------------------------------- */
  ThreadFlagsType VideoThreadMain(const Thread &tClass) try
  { // Send playing event if we're not temporarily deinitialising
    if(auReason != UB_REINIT) LuaEvtDispatch(VE_PLAY);
    // Loop until thread should exit
    if(FlagIsSet(FL_THEORAVORBIS))     // Ogg has both audio and video streams?
      while(tClass.ThreadShouldNotExit() && VideoHandleAudioVideo());
    else if(FlagIsSet(FL_VORBIS))      // Ogg has audio only stream?
      while(tClass.ThreadShouldNotExit() && VideoHandleAudioOnly());
    else if(FlagIsSet(FL_THEORA))      // Ogg has video only stream?
      while(tClass.ThreadShouldNotExit() && VideoHandleVideoOnly());
    // Log the reason why the thread should be terminated
    cLog->LogDebugExSafe("Video '$' main loop exit with reason $!",
      NameGet(), auReason.load());
    // Why did the video manager terminate?
    switch(const Unblock ubCode = auReason.load())
    { // The thread was in standby? Shouldn't happen! Restart the thread
      case UB_STANDBY: [[fallthrough]];
      // The thread was blocking? Shouldn't happen! Restart the thread
      case UB_BLOCK: [[fallthrough]];
      // The video was playing? Shouldn't happen! Restart the thread
      case UB_PLAY: [[fallthrough]];
      // The video was reinitialised? Shouldn't happen! Restart the thread
      case UB_REINIT: [[fallthrough]];
      // The thread was terminated? Just break
      case UB_DATA: break;
      // The video finished playing? Send finish event
      case UB_FINISH: LuaEvtDispatch(VE_FINISH); break;
      // The video was stopped? Send stop event to guest
      case UB_STOP: LuaEvtDispatch(VE_STOP); break;
      // The video was paused? Send pause event to guest
      case UB_PAUSE: LuaEvtDispatch(VE_PAUSE); break;
      // Unknown code
      default: XC("Internal error: Unknown unblock reason code!",
                 "Code", ubCode);
    } // Exit thread cleanly with specified reason
    return TS_OK;
  } // exception occured?
  catch(const StdException &eReason)
  { // Report it to log
    cLog->LogErrorExSafe("(VIDEO THREAD EXCEPTION) $", eReason);
    // Failure exit code
    return TS_ERROR;
  }
  /* -- Convert colour space to name --------------------------------------- */
  const StdStringView &VideoColourSpaceToString(const th_colorspace csId) const
    { return cVideos->csStrings.LukArrayGet(csId); }
  /* -- Convert pixel format to name --------------------------------------- */
  const StdStringView &VideoPixelFormatToString(const th_pixel_fmt pfId) const
    { return cVideos->pfStrings.LukArrayGet(pfId); }
  /* -- Update textures without lock --------------------------------------- */
  void VideoRenderUnsafe()
  { // Get frame
    Frame &frFrame = faData[stFActive];
    // On a slower system, this call could repeat in the same frame so if FBO
    // is already finished this frame? Ignore rendering to it.
    if(FboIsFinished()) frFrame.bDraw = false;
    // Skip ahead frames if we need to catch up with audio
    // If we should draw?
    else if(frFrame.bDraw)
    { // Upload texture data. This is quite safe because this data isnt
      // written to until the decoding routine thread has finished setting
      // these values.
      for(YCbCr &yccFrame : frFrame.yccaFrames)
      { // Bind the texture for this colour component
        cOgl->OglBindTexture(gluaYCbCr[yccFrame.stI]);
        // Get data
        th_img_plane &tipD = yccFrame.tipP;
        // Set unpack row length because of how the image data is formatted
        // by the vorbis api
        cOgl->OglSetUnpackRowLength(tipD.stride);
        // Now upload the image data to opengl, the memory is already
        // pre-allocated
        cOgl->OglUploadTextureSub(tipD.width, tipD.height, GL_RED, tipD.data);
      } // Reset unpack row length to default
      cOgl->OglSetUnpackRowLength(0);
      // Initialise Y texture id, active texture and shader program
      FboResetCache(gluaYCbCr[0], 0, shpProgram->GetProgram());
      // Commit the Y setup and configure the U and V setup
      FboFinishAndReset(gluaYCbCr[1], 1, shpProgram->GetProgram());
      FboFinishAndReset(gluaYCbCr[2], 2, shpProgram->GetProgram());
      // Blit the YCbCr multi-texture into the FBO
      FboBlit(fiYCbCr, gluaYCbCr[2], 2, shpProgram);
      // Commit the V texture and send everything to FBO list for rendering
      FboFinishAndRender();
      FboClearLists();
      // No need to update again until decoder thread rendered another frame
      frFrame.bDraw = false;
    } // One less buffer to wait
    --stFWaiting;
    ++astFFree;
    stFActive = (stFActive + 1) % faData.size();
  }
  /* -- Video properties ------------------------------------------- */ public:
  double VideoGetTime() const { return adVideoTime; }
  double VideoGetAudioTime() const { return adAudioTime; }
  double VideoGetDrift() const { return dDrift; }
  double VideoGetFPS() const { return dFPS; }
  unsigned VideoGetFrame() const
    { return static_cast<unsigned>(VideoGetTime() * VideoGetFPS()); }
  unsigned VideoGetFrames() const { return auVideoFrames; }
  unsigned VideoGetFramesSkipped() const { return auVideoFramesLost; }
  th_pixel_fmt VideoGetPixelFormat() const { return tiData.pixel_fmt; }
  th_colorspace VideoGetColourSpace() const { return tiData.colorspace; }
  ogg_uint32_t VideoGetFrameHeight() const { return tiData.frame_height; }
  ogg_uint32_t VideoGetFrameWidth() const { return tiData.frame_width; }
  ogg_uint32_t VideoGetHeight() const { return tiData.pic_height; }
  ogg_uint32_t VideoGetWidth() const { return tiData.pic_width; }
  ogg_uint32_t VideoGetOriginX() const { return tiData.pic_x; }
  ogg_uint32_t VideoGetOriginY() const { return tiData.pic_y; }
  long VideoGetSampleRate() const { return viData.rate; }
  int VideoGetChannels() const { return viData.channels; }
  uint64_t VideoGetLength() const { return fmFile.MemSize(); }
  bool VideoIsSourceAvailable() const { return soSource != nullptr; }
  bool VideoIsSourceUnavailable() const { return !VideoIsSourceAvailable(); }
  ALenum VideoGetAudioFormat() const { return aleFormat; }
  const StdStringView &VideoGetFormatAsIdentifier() const
    { return cOal->OalFormatToString(VideoGetAudioFormat()); }
  /* -- When data has asynchronously loaded -------------------------------- */
  bool AsyncReady(FileMap &fmData)
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
            XC("Not a valid ogg/theora stream!", "Name", NameGet());
          // Try to rebuffer more data and throw error if error reading
          if(VideoDoIOBuffer())
            XC("Read ogg/theora stream error!",
              "Name", NameGet(), "Reason", StrFromErrNo());
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
          // Fall through to break
          [[fallthrough]];
        } // Returned if stream has not yet captured sync (bytes were skipped).
        case -1: break;
        // Done
        default: XC("Unknown OGG pageout result!",
                   "Name", NameGet(), "Result", iPageOutResult);
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
            "Name", NameGet());
        // Got the page
        ++iGotTheoraPage;
      } // Look for further vorbis headers
      while(iGotVorbisPage && iGotVorbisPage < 3)
      { // Is not a valid theora packet? Ignore. Might be a vorbis packet
        if(ogg_stream_packetout(&ostsVorbis, &opkData) != 1) break;
        // decode the headers
        if(vorbis_synthesis_headerin(&viData, &vcData, &opkData))
          XC("Error parsing Vorbis stream headers!",
            "Name", NameGet());
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
      else if(VideoDoIOBuffer())
        XC("EOF while searching for codec headers!",
          "Name", NameGet(), "Position", fmFile.FileMapTell());
    } // Set flags of what headers we got
    if(iGotTheoraPage) FlagSet(FL_THEORA);
    if(iGotVorbisPage) FlagSet(FL_VORBIS);
    // And now we should have it all if not, well die
    if(FlagIsClear(FL_THEORAVORBIS))
      XC("Could not find a Theora and/or Vorbis stream!",
        "Name", NameGet());
    // If there is a video stream?
    if(FlagIsSet(FL_THEORA))
    { // Parse the comments and then free the strings
      sncsmThMetaData =
        StdMove(cCodecOGG->VorbisParseComments(tcData.user_comments,
          tcData.comments));
      th_comment_clear(&tcData);
      // Allocate a new one and throw error if not allocated
      tdcPtr = th_decode_alloc(&tiData, tsiPtr);
      if(!tdcPtr)
        XC("Error creating theora decoder context!",
          "Name", NameGet());
      // Calculate FPS of video
      dFPS = static_cast<double>(tiData.fps_numerator) /
             static_cast<double>(tiData.fps_denominator);
      // Sanity check FPS
      if(VideoGetFPS() < 1.0 || VideoGetFPS() > 200.0)
        XC("Ambiguous frame rate in video!",
          "Name", NameGet(), "FPS", VideoGetFPS());
      // Get maximum texture size to match stored types
      const ogg_uint32_t uMaxTexSize = cOgl->OglMaxTexSize<ogg_uint32_t>();
      // Make sure GPU can support texture size
      if(VideoGetFrameWidth() > uMaxTexSize ||
         VideoGetFrameHeight() > uMaxTexSize)
        XC("The currently active graphics device does not support a "
          "texture size that would fit the video dimensions!",
          "Name", NameGet(),       "Width", VideoGetFrameWidth(),
          "Height",     VideoGetFrameHeight(), "Limit", uMaxTexSize);
      // Verify colour space
      switch(VideoGetColourSpace())
      { // Valid colour spaces
        case TH_CS_UNSPECIFIED: [[fallthrough]];  // No colour content?
        case TH_CS_ITU_REC_470M: [[fallthrough]]; // Monochrome content?
        case TH_CS_ITU_REC_470BG: break;          // PAL/SECAM content?
        // Invalid colour space
        default: XC("The specified colour space is unsupported!",
                   "Name", NameGet(), "ColourSpace", VideoGetColourSpace());
      } // Verify chroma subsampling type
      switch(VideoGetPixelFormat())
      { // Valid chroma types
        case TH_PF_420: [[fallthrough]]; // YCbCr 4:2:0
        case TH_PF_422: [[fallthrough]]; // YCbCr 4:2:2
        case TH_PF_444: break;           // YCbCr 4:4:4
        // Invalid chroma type
        default: XC("Only 420, 422 or 444 pixel format is supported!",
                   "Name", NameGet(), "PixelFormat", VideoGetPixelFormat());
      } // Update frame immediately
      CISetLimit(UtilPerSec(VideoGetFPS()));
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
      sncsmVoMetaData =
        StdMove(cCodecOGG->VorbisParseComments(vcData.user_comments,
          vcData.comments));
      vorbis_comment_clear(&vcData);
      // Make sure rate is sane
      if(VideoGetSampleRate() < 1 || VideoGetSampleRate() > 192000)
        XC("Video playback rate not valid at this time!",
          "Name", NameGet(), "Rate", VideoGetSampleRate());
      // Make sure channels are correct
      if(VideoGetChannels() < 1 || VideoGetChannels() > 2)
        XC("Video playback channel count of not supported!",
          "Name", NameGet(), "Channels", VideoGetChannels());
      // Initialise vorbis synthesis
      if(vorbis_synthesis_init(&vdsData, &viData))
        XC("Failed to initialise vorbis synthesis!", "Name", NameGet());
      // Initialise vorbis block
      if(vorbis_block_init(&vdsData, &vbData))
        XC("Failed to initialise vorbis block!", "Name", NameGet());
      // Calculate memory required for buffering audio and verify the size
      // since we don't know if someone's sending us dodgy bitrate values
      const double dMem =
        UtilNearestPow2<double>(UtilMaximum(viData.bitrate_upper,
          viData.bitrate_nominal) / 8);
      if(dMem <= 0.0 || dMem > 1048576.0)
        XC("Calculated erroneous memory size for audio buffer!",
          "Name",  NameGet(),            "Amount",  dMem,
          "Upper", viData.bitrate_upper, "Nominal", viData.bitrate_nominal,
          "Lower", viData.bitrate_lower, "Window",  viData.bitrate_window);
      // Allocate enough memory for audio buffer. We may not actually use all
      // the memory, but it is better than putting the alloc in the decoder
      // tick. We can reuse the 'Memory' class from the 'AsyncLoader' class.
      MemResize(static_cast<size_t>(dMem));
      // Show what we allocated
      cLog->LogDebugExSafe("Video pre-allocated $ bytes for audio decoder.",
        MemSize());
      // Reset audio position and drift
      adAudioTime = dDrift = 0.0;
    } // Log success
    cLog->LogInfoExSafe("Video '$' loaded.", NameGet());
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
      StdIOSFixed, static_cast<int>(tiData.version_major),
        static_cast<int>(tiData.version_minor),
        static_cast<int>(tiData.version_subminor),
      ostsTheora.serialno, StdIOSHex, ostsTheora.serialno, StdIOSDec,
      VideoGetWidth(), VideoGetHeight(), StrFromRatio(VideoGetWidth(),
        VideoGetHeight()), VideoGetFrameWidth(), VideoGetFrameHeight(),
      StrFromRatio(VideoGetFrameWidth(), VideoGetFrameHeight()),
      tiData.aspect_numerator, tiData.aspect_denominator, VideoGetOriginX(),
      VideoGetOriginY(), VideoPixelFormatToString(VideoGetPixelFormat()),
      VideoGetPixelFormat(), VideoColourSpaceToString(VideoGetColourSpace()),
      VideoGetColourSpace(), VideoGetFPS(), viData.version, VideoGetChannels(),
      VideoGetSampleRate(), StrToGrouped(VideoGetSampleRate()),
      tiData.target_bitrate, StrToBits(tiData.target_bitrate),
      viData.bitrate_upper, StrToBits(viData.bitrate_upper),
      viData.bitrate_nominal, StrToBits(viData.bitrate_nominal),
      viData.bitrate_lower, StrToBits(viData.bitrate_lower),
      viData.bitrate_window, StrToBits(viData.bitrate_window));
    // Parse vorbis comments and if not empty? Enumerate and log each one
    if(cLog->LogHasLevel(LH_DEBUG))
    { // Prepare data for lists
      using ListPair = StdPair<const StdStringView&, StrNCStrMap&>;
      using Lists = StdArray<const ListPair, 2>;
      static const StdStringView svTheora{ "Theora" }, svVorbis{ "Vorbis" };
      const Lists lLists{ { { svTheora, sncsmThMetaData },
                            { svVorbis, sncsmVoMetaData } } };
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
    // Set new callback function for async thread since we're reusing it
    AsyncGetThread().CollectorUnregister();
    AsyncGetThread().ThreadInit(bind(&Video::VideoThreadMain, this, _1));
    AsyncGetThread().CollectorRegister();
    // FileMap references the 'Memory' base class so don't free it
    return false;
  }
  /* -- Reinitialise frame buffer object and texture ----------------------- */
  void VideoReInitDisplayOutput() { FboReInit(); VideoInitTexture(); }
  /* -- De-initialise texture and frame buffer object ---------------------- */
  void VideoDeInitDisplayOutput() { VideoDeInitTexture(); FboDeInit(); }
  /* -- Update volume ------------------------------------------------------ */
  void VideoCommitVolume()
  { // Ignore if no source
    if(VideoIsSourceUnavailable()) return;
    // Set volume
    soSource->SourceSetGain(alfAudioVolume * cMixer->MixerGetAdjVideoVolume());
  }
  /* -- Set and update volume ---------------------------------------------- */
  void VideoSetVolume(const ALfloat alfVolume)
    { alfAudioVolume = alfVolume; VideoCommitVolume(); }
  /* -- Blit specific triangle --------------------------------------------- */
  void VideoBlitTri(const size_t stTId)
   { FboActive()->FboBlitTri(*this, stTId); }
  /* -- Blit quad ---------------------------------------------------------- */
  void VideoBlit() { FboActive()->FboBlit(*this); }
  /* -- Upload the textures ------------------------------------------------ */
  void VideoRender()
  { // Try to lock access to uploading mutex and upload textures if lock was
    // successful and frames waiting to upload
    MutexUniqueCall(
      [this](UniqueLock &ulUpload)
        { if(ulUpload.owns_lock() && stFWaiting) VideoRenderUnsafe(); },
      try_to_lock);
  }
  /* -- Video is playing? -------------------------------------------------- */
  bool VideoIsPlaying() const { return AsyncGetThread().ThreadIsJoinable(); }
  /* -- DeInitialise audio ouput (because reinitialising) ------------------ */
  void VideoDeInitAudio()
  { // Return if there is no audio in this video
    if(FlagIsClear(FL_VORBIS)) return;
    // VideoPause the video and set to resume on reinit
    if(VideoPause(UB_REINIT)) FlagSet(FL_RESUME);
    // Audio buffers are empty
    aldAudioBuffer = 0.0;
    // De-initialise the OpenAL segment
    VideoStopAudioAndUnloadBuffers();
  }
  /* -- ReInitialise audio (because audio is restarting) ------------------- */
  void VideoReInitAudio()
  { // Just return if we're not to resume
    if(FlagIsClear(FL_RESUME)) return;
    // If there is an audio stream and we're reinitialising then resume play
    if(VideoPlay(UB_REINIT)) FlagClear(FL_RESUME);
  }
  /* -- Stop and unload audio buffers -------------------------------------- */
  void VideoStopAudioAndUnloadBuffers()
  { // Ignore if no source or no vorbis stream
    if(VideoIsSourceUnavailable()) return;
    // Stop from playing so all buffers are unqueued and wait for stop
    // then unqueue and delete the buffer
    soSource->SourceStopUnQueueAndDeleteAllBuffers();
    // Unlock the source so the source manager can recycle it
    soSource->SourceUnlock();
    soSource = nullptr;
  }
  /* -- Do pause video ----------------------------------------------------- */
  bool VideoPause(const Unblock ubNewReason = UB_PAUSE)
  { // Make sure playing flag is removed
    if(FlagIsClear(FL_PLAY)) return false;
    FlagClear(FL_PLAY);
    // Set exit reason
    auReason = ubNewReason;
    // DeInit the thread, unblock the worker thread and stop and unload buffers
    AsyncGetThread().ThreadDeInit();
    // Success
    return true;
  }
  /* -- Stop video and free everything ------------------------------------- */
  bool VideoStop(const Unblock ubNewReason = UB_STOP)
  { // Ignore if already stopped
    if(FlagIsSet(FL_STOP)) return false;
    FlagSet(FL_STOP);
    // VideoPause playback and synchronise
    VideoPause(ubNewReason);
    // Set the reason for stopping
    auReason = ubNewReason;
    // De-initialise the OpenGL segment
    if(FlagIsSet(FL_GLINIT)) { VideoDeInitTexture(); FboDeInit(); }
    // De-initialise the OpenAL segment
    VideoStopAudioAndUnloadBuffers();
    // Audio buffers are empty
    aldAudioBuffer = 0.0;
    // Reset buffer status
    stFActive = stFNext = stFWaiting = astFFree = astLoop = 0;
    // Rewind data stream
    VideoDoRewindAndReset();
    // Log that the video was stopped
    cLog->LogDebugExSafe("Video '$' stopped with reason $.",
      NameGet(), ubNewReason);
    // Success
    return true;
  }
  /* -- Advance a frame ---------------------------------------------------- */
  void VideoAdvance()
  { // Return if already playing
    if(AsyncGetThread().ThreadIsRunning()) return;
    // Loop until thread should exit
    if(FlagIsSet(FL_THEORAVORBIS)) // Ogg has both audio and video streams?
      while(!VideoHandleAudioVideo() && !stFWaiting);
    else if(FlagIsSet(FL_THEORA))      // Ogg has video only stream?
      while(!VideoHandleVideoOnly() && !stFWaiting);
  }
  /* -- Awaken the video --------------------------------------------------- */
  void VideoAwaken()
  { // If surfaces not initialised? Note that even if the video does not
    // contain a Theora stream, we still want to initialise it anyway.
    if(FlagIsClear(FL_GLINIT))
    { // OpenGL output initialised
      FlagSet(FL_GLINIT);
      // Init FBO with room for 2 triangles and 3 commands
      FboInit(NameGet(), static_cast<GLsizei>(VideoGetWidth()),
                         static_cast<GLsizei>(VideoGetHeight()), 2, 3);
      FboSetMatrix(0.0f, 0.0f, 0.0f, 0.0f);
      FboSetTransparency(true);
      FboItemSetTexCoord(0.0f, 0.0f, 1.0f, 1.0f);
      // Clear the FBO, initially transparent and blue
      ColourSet(0.0f, 0.0f, 1.0f, 0.0f);
      FboSetClear(false);
      // We must discard the extra garbage from the ogg video. We can do that
      // with the GPU very easily by altering texture coords!
      fiYCbCr.FboItemSetTexCoord(
        static_cast<GLfloat>(VideoGetOriginX()) /
          static_cast<GLfloat>(VideoGetFrameWidth()),
        static_cast<GLfloat>(VideoGetOriginY() + VideoGetHeight()) /
          static_cast<GLfloat>(VideoGetFrameHeight()),
        static_cast<GLfloat>(VideoGetOriginX() + VideoGetWidth()) /
          static_cast<GLfloat>(VideoGetFrameWidth()),
        static_cast<GLfloat>(VideoGetOriginY()) /
          static_cast<GLfloat>(VideoGetFrameHeight()));
      // Init texture
      VideoInitTexture();
    } // If theres a audio segment and AL portion is initialised?
    if(FlagIsSet(FL_VORBIS) && VideoIsSourceUnavailable())
    { // Compare number of channels in file to set appropriate format. This is
      // here and not at the files init stage as it handles reinits too and
      // the FP supported audio format flag have changed.
      switch(VideoGetChannels())
      { // 1 channel mono?
        case 1: aleFormat = cOal->OalHave32FPPB() ?
                  AL_FORMAT_MONO_FLOAT32 : AL_FORMAT_MONO16;
                break;
        // 2 channel stereo?
        case 2: aleFormat = cOal->OalHave32FPPB() ?
                  AL_FORMAT_STEREO_FLOAT32 : AL_FORMAT_STEREO16;
                break;
        // Unknown channel count. Problem should already be handled at init.
        default: cLog->LogWarningExSafe("Video '$' audio playback failed. "
          "Invalid channel count of $!", NameGet(), VideoGetChannels());
          return;
      } // Get a new sound source and if we got it update volume and return
      soSource = SourceGet();
      if(VideoIsSourceAvailable()) VideoCommitVolume();
      // Tell log
      else cLog->LogWarningExSafe(
        "Video '$' audio playback failed. Out of sources!", NameGet());
    }
  }
  /* -- Play video --------------------------------------------------------- */
  bool VideoPlay(const Unblock ubNewReason = UB_PLAY)
  { // If playing flag is already set?
    if(FlagIsSet(FL_PLAY)) return false;
    // Check that OpenGL and OpenAL is initialised
    VideoAwaken();
    // Set reason for playing
    auReason = ubNewReason;
    // Next frame can show immediately
    CISync();
    // Set playing flag
    FlagSet(FL_PLAY);
    // Start decoding (reusing the async thread)
    AsyncGetThread().ThreadStart(this);
    // Log that the video was started
    cLog->LogDebugExSafe("Video '$' playing with reason $!",
      NameGet(), ubNewReason);
    // Success
    return true;
  }
  /* -- Rewind video ------------------------------------------------------- */
  void VideoRewind()
  { // Ignore if already rewound
    if(llVideoGranulePos <= 0) return;
    // Rewind video to start
    VideoDoRewindAndReset();
    // Log that the video was rewound
    cLog->LogDebugExSafe("Video '$' rewound!", NameGet());
  }
  /* -- Deinitialise video ouput ------------------------------------------- */
  void VideoDeInitTexture()
  { // Delete the component textures
    GLL(cOgl->OglSetDeleteTextures(gluaYCbCr),
      "Failed to delete $ texture components", gluaYCbCr.size());
    // Clear texture names
    gluaYCbCr.fill(0);
  }
  /* -- Generate texture for specified video component --------------------- */
  void VideoConfigTexture(const GLuint gluTU, const GLsizei glsiW,
    const GLsizei glsiH)
  { // Get texture id
    GLuint &gluT = gluaYCbCr[gluTU];
    // Bind texture
    GL(cOgl->OglBindTexture(gluT), "Failed to bind video component texture!",
      "Name", NameGet(), "Index", gluTU, "Texture", gluT);
    // Allocate VRAM for texture
    GL(cOgl->OglUploadTexture(0, glsiW, glsiH, GL_R8, GL_RED, nullptr),
      "Failed to reserve texture memory for image component!",
        "Name", NameGet(), "Index",  gluTU, "Texture", gluT,
        "Width",      glsiW,      "Height", glsiH);
    // Set filtering. Only need to use GL_NEAREST because the FBO is the same
    // size as the video textures.
    GL(cOgl->OglSetTexParam(GL_TEXTURE_MIN_FILTER, GL_LINEAR),
      "Failed to set minification filter for video component!",
      "Name", NameGet(), "Index", gluTU, "Texture", gluT);
    GL(cOgl->OglSetTexParam(GL_TEXTURE_MAG_FILTER, GL_LINEAR),
      "Failed to set magnification filter for video component!",
        "Name", NameGet(), "Index", gluTU, "Texture", gluT);
    // Set to repeat and disable filter
    GL(cOgl->OglSetTexParam(GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT),
      "Failed to set texture wrapping S mode for video component!",
        "Name", NameGet(), "Index", gluTU, "Texture", gluT);
    GL(cOgl->OglSetTexParam(GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT),
      "Failed to set texture wrapping T mode for video component!",
        "Name", NameGet(), "Index", gluTU, "Texture", gluT);
    // Say what we did
    cLog->LogDebugExSafe("Video texture $x$ created for channel $.",
      glsiW, glsiH, gluTU);
  }
  /* -- Commit filter ------------------------------------------------------ */
  void VideoCommitFilter()
    { FboSetFilterCommit(FlagIsSet(FL_FILTER) ? OF_L_L : OF_N_N); }
  /* -- Set and commit filtering on video textures ------------------------- */
  void VideoSetFilter(const bool bState)
    { FlagSetOrClear(FL_FILTER, bState); VideoCommitFilter(); }
  /* -- Looping functions -------------------------------------------------- */
  size_t VideoGetLoop() const { return astLoop; }
  void VideoSetLoop(const size_t stCount) { astLoop = stCount; }
  /* -- Colour key functions ----------------------------------------------- */
  void VideoUpdateShader()
  { // Set program depending on the specified parameters
    shpProgram = VideoGetFDR() ? (VideoGet709() ?
      (VideoGetKeyed() ? &cShaderCore->sh3DYCbCrK709FR :
                    &cShaderCore->sh3DYCbCr709FR) :
      (VideoGetKeyed() ? &cShaderCore->sh3DYCbCrK601FR :
                    &cShaderCore->sh3DYCbCr601FR)
    ) : (VideoGet709() ?
      (VideoGetKeyed() ? &cShaderCore->sh3DYCbCrK709PR :
                    &cShaderCore->sh3DYCbCr709PR) :
      (VideoGetKeyed() ? &cShaderCore->sh3DYCbCrK601PR :
                    &cShaderCore->sh3DYCbCr601PR)
    );
  }
  void VideoUpdateShaderFlag(const VideoFlagsConst vfcFlag, const bool bState)
    { FlagSetOrClear(vfcFlag, bState); VideoUpdateShader(); }
  bool VideoGetFDR() const { return FlagIsSet(FL_FDR); }
  void VideoSetFDR(const bool bState)
    { VideoUpdateShaderFlag(FL_FDR, bState); }
  bool VideoGet709() const { return FlagIsSet(FL_REC709); }
  void VideoSet709(const bool bState)
    { VideoUpdateShaderFlag(FL_REC709, bState); }
  bool VideoGetKeyed() const { return FlagIsSet(FL_KEYED); }
  void VideoSetKeyed(const bool bState)
    { VideoUpdateShaderFlag(FL_KEYED, bState); }
  /* -- Generate component textures ---------------------------------------- */
  void VideoInitTexture()
  { // Ignore if we don't have a opengl
    if(FlagIsClear(FL_GLINIT)) return;
    // Chosen divisors
    GLsizei siWDIV, siHDIV;
    // Set divisor based on pixel format
    switch(VideoGetPixelFormat())
    { // YCbCr420p : Y = Full size; Cb/Cr = Half width and height
      case TH_PF_420: siWDIV = siHDIV = 2; break;
      // YCbCr422p : Y = Full size; Cb/Cr = Half width and full height
      case TH_PF_422: siWDIV = 2; siHDIV = 1; break;
      // YCbCr444p : Y = Full size; Cb/Cr = Full width and height
      case TH_PF_444: case TH_PF_RSVD: siWDIV = siHDIV = 1; break;
      // Unknown so throw error
      default: XC("Unknown theora pixel format!",
                  "Name", NameGet(), "PixelFormat", VideoGetPixelFormat());
    } // Create textures for Y/Cb/Cr multitexture
    GL(cOgl->OglCreateTextures(gluaYCbCr),
      "Failed to create texture components for YCbCr texture!",
      "Name", NameGet(), "Components", gluaYCbCr.size());
    // Cast frame dimensions as they're 64-bit and not compatible with GL.
    const GLsizei siWidth = static_cast<GLsizei>(VideoGetFrameWidth()),
                  siHeight = static_cast<GLsizei>(VideoGetFrameHeight());
    // Configure the Y component (always full size)
    VideoConfigTexture(0, siWidth, siHeight);
    // Configure the Cb/Cr components
    for(GLuint uIndex = 1; uIndex <= 2; ++uIndex)
      VideoConfigTexture(uIndex, siWidth / siWDIV, siHeight / siHDIV);
    // Commit the current filter setting
    VideoSetFilter(FlagIsSet(FL_FILTER));
    // Update choice of shader to use
    VideoUpdateShader();
  }
  /* -- Default Constructor ------------------------------------------------ */
  Video() :
    /* -- Initialisers ----------------------------------------------------- */
    ICHelperVideo{ cVideos, this },    // Initialise collector class
    Fbo{ GL_RGBA8, false },            // Create unregistered opaque surface
    AsyncLoaderVideo{ this,            // Initialise async loader
      EMC_MP_VIDEO },                  // ...video event code
    LuaEvtSlave{ this,                 // Initialise event handler
      EMC_VID_EVENT },                 // ...event handler code
    VideoFlags{ FL_FDR|FL_REC709 },    // Use full-dynamic range and Rec.709
    auReason(UB_STANDBY),              // Initially set to blocked
    astLoop(0),                        // Initialise no loops remaining
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
    tybPlanes{ /* Zeroed */ },         // Clear Theora pixel colour buffers
    sPlData{ tybPlanes,                // Protect 'tybPlanes' with this Span
      StdExtent<th_ycbcr_buffer> },    // Should be three pointers in there
    llVideoGranulePos(0),              // Initialise granule position
    adVideoTime(0.0),                  // Initialise position
    dFPS(0.0),                         // Initialise fps
    auVideoFrames(0),                  // Initialise frames processed
    auVideoFramesLost(0),              // Initialise frames lost
    stFActive(0),                      // initialise active frame id
    stFNext(0),                        // Initialise next frame id
    stFWaiting(0),                     // Initialise frames waiting
    astFFree{ faData.size() },         // Initialise free frames
    ostsVorbis{ /* Zeroed */ },        // Clear Vorbis stream status data
    viData{ /* Zeroed */ },            // Clear Vorbis decoder data
    vcData{ /* Zeroed */ },            // Clear Vorbis comment data
    vdsData{ /* Zeroed */ },           // Clear Vorbis DSP state data
    vbData{ /* Zeroed */ },            // Clear Vorbis decoder block data
    adAudioTime(0.0),                  // Initialise audio position
    shpProgram(nullptr),               // Initialise pointer to Shader used
    soSource(nullptr),                 // Initialise pointer to Source used
    aleFormat(AL_NONE),                // Initialise audio format type
    alfAudioVolume(1.0f),              // Initialise audio volume
    aldAudBufMax(                      // Initialise maximum audio buffer size
      cVideos->dAudioBufferSize),      // ...with value set by user
    aldAudioBuffer(0.0)                // Initialise audio buffer length
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- Destructor --------------------------------------------------------- */
  DTORHELPER(~Video,
    // Stop any pending async operations
    AsyncCancel();
    // Remove the registration now so it is no longer polled
    ICHelperVideo::CollectorUnregister();
    // Set exit reason
    auReason = UB_FINISH;
    // DeInit the thread, unblock the worker thread and stop and unload buffers
    AsyncGetThread().ThreadStopNoThrow();
    // Ignore if file data not initialised
    if(fmFile.FileMapClosed()) return;
    // Prevent more events being generated
    LuaEvtDeInit();
    // Stop and unload audio buffers
    VideoStopAudioAndUnloadBuffers();
    // Deinit texture and reset parameters
    if(FlagIsSet(FL_GLINIT)) { VideoDeInitTexture(); FboDeInit(); }
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
    // Log that the video was unloaded
    cLog->LogDebugExSafe("Video unloaded '$'!", NameGet());
  )
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
static void VideoReInitTextures()
{ // Ignore if no videos otherwise reinitialise ogl textures on all videos
  if(cVideos->empty()) return;
  cLog->LogDebugExSafe("Videos reinitialising $ video surfaces...",
    cVideos->CollectorCountUnsafe());
  for(Video*const vVideo : *cVideos) vVideo->VideoReInitDisplayOutput();
  cLog->LogDebugExSafe("Videos reinitialised $ video surfaces!",
    cVideos->CollectorCountUnsafe());
}
/* == De-init video textures (after thread shutdown) ======================= */
static void VideoDeInitTextures()
{ // Ignore if no videos otherwise deinitialise ogl textures on all videos
  if(cVideos->empty()) return;
  cLog->LogDebugExSafe("Videos deinitialising $ video surfaces...",
    cVideos->CollectorCountUnsafe());
  for(Video*const vVideo : *cVideos) vVideo->VideoDeInitDisplayOutput();
  cLog->LogDebugExSafe("Videos deinitialised $ video surfaces!",
    cVideos->CollectorCountUnsafe());
}
/* == Clear event callbacks on all videos (must be synchronised) =========== */
static void VideoClearEvents()
{ // Lock access to video collector list
  cVideos->MutexCall([](){
    // Ignore if no videos
    if(cVideos->empty()) return;
    // Clear all video events and log results
    cLog->LogDebugExSafe("Videos clearing events from $ video objects...",
      cVideos->CollectorCountUnsafe());
    for(Video*const vVideo : *cVideos) vVideo->LuaEvtDeInit();
    cLog->LogDebugExSafe("Videos cleared events from $ video objects!",
      cVideos->CollectorCountUnsafe());
  });
}
/* == Stop all videos (must be sychronised) ================================ */
static void VideoStop()
{ // Lock access to video collector list and stop all videos
  cVideos->MutexCall([](){
    for(Video*const vVideo : *cVideos) vVideo->VideoStop();
  });
}
/* == DeInit all videos (after engine thread shutdown) ===================== */
static void VideoDeInit()
{ // Ignore if no videos otherwise deinitialise oal buffers on all videos
  if(cVideos->empty()) return;
  cLog->LogDebugExSafe("Videos deinitialising $ videos...",
    cVideos->CollectorCountUnsafe());
  for(Video*const vVideo : *cVideos) vVideo->VideoDeInitAudio();
  cLog->LogDebugExSafe("Videos deinitialised $ videos!",
    cVideos->CollectorCountUnsafe());
}
/* == ReInit all videos (after engine thread shutdown) ===================== */
static void VideoReInit()
{ // Ignore if no videos otherwise reinitialise oal buffers on all videos
  if(cVideos->empty()) return;
  cLog->LogDebugExSafe("Videos reinitialising $ videos...",
    cVideos->CollectorCountUnsafe());
  for(Video*const vVideo : *cVideos) vVideo->VideoReInitAudio();
  cLog->LogDebugExSafe("Videos reinitialised $ videos!",
    cVideos->CollectorCountUnsafe());
}
/* == Render all videos ==================================================== */
static void VideoRender()
  { for(Video*const vVideo : *cVideos) vVideo->VideoRender(); }
/* == Update all streams base volume ======================================= */
static void VideoCommitVolume()
  { for(Video*const vVideo : *cVideos) vVideo->VideoCommitVolume(); }
/* == Set buffer size ====================================================== */
static CVarReturn VideoSetIOBufferSize(const size_t stSize)
  { return CVarSimpleSetIntNLG(cVideos->stIOBufferSize,
      stSize, 4096UL, 16777216UL); }
/* == Set drift length maximum ============================================= */
static CVarReturn VideoSetMaximumDrift(const double dMax)
  { return CVarSimpleSetIntNLG(cVideos->dMaxDrift, dMax, 0.01, 1.0); }
/* == Set audio buffer length maximum ====================================== */
static CVarReturn VideoSetAudioBufferSize(const double dMax)
  { return CVarSimpleSetIntNLG(cVideos->dAudioBufferSize, dMax, 0.01, 1.0); }
/* == Set all streams base volume ========================================== */
static CVarReturn VideoSetVolume(const ALfloat alfVolume)
{ // Ignore if invalid value
  if(alfVolume < 0.0f || alfVolume > 1.0f) return DENY;
  // Store volume (SOURCES class keeps it)
  cMixer->MixerSetVideoVolume(alfVolume);
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
