/* == PCMFMOGG.HPP ========================================================= **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## Handles loading and saving of .OGG files with the PcmLib system.    ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace ICodecOGG {                  // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace ICommon::P;            using namespace IError::P;
using namespace IFileMap::P;           using namespace IFlags;
using namespace IIdent::P;             using namespace ILog::P;
using namespace IMemory::P;            using namespace IPcmDef::P;
using namespace IPcmLib::P;            using namespace IStd::P;
using namespace IString::P;            using namespace IUtil::P;
using namespace Lib::OpenAL::Types;    using namespace Lib::Ogg;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* ------------------------------------------------------------------------- */
class CodecOGG;                        // Class prototype
static CodecOGG *cCodecOGG = nullptr;  // Pointer to global class
class CodecOGG :                       // OGG codec object
  /* -- Base classes ------------------------------------------------------- */
  private PcmLib,                      // Pcm format helper class
  private IdMap<>                      // Ogg error codes
{ /* -- Private variables -------------------------------------------------- */
  const ov_callbacks ovcCallbacks;     // Vorbis callbacks
  /* -- Vorbis read callback --------------------------------------- */ public:
  static size_t VorbisRead(void*const vpPtr,
    size_t stSize, size_t stCount, void*const vFmClassPtr)
      { return reinterpret_cast<FileMap*>(vFmClassPtr)->
          FileMapReadToAddr(vpPtr, stSize * stCount); }
  /* -- Vorbis seek callback ----------------------------------------------- */
  static int VorbisSeek(void*const vFmClassPtr, ogg_int64_t llOffset, int iLoc)
    { return static_cast<int>(reinterpret_cast<FileMap*>(vFmClassPtr)->
        FileMapSeek(static_cast<size_t>(llOffset), iLoc)); }
  /* -- Vorbis close callback ---------------------------------------------- */
  static int VorbisClose(void*const) { return 1; }
  /* -- Vorbis tell callback ----------------------------------------------- */
  static long VorbisTell(void*const vFmClassPtr)
    { return static_cast<long>(reinterpret_cast<FileMap*>(vFmClassPtr)->
        FileMapTell()); }
  /* -- Return generic ogg callback functions ------------------------------ */
  const ov_callbacks &GetCallbacks() { return ovcCallbacks; }
  /* -- Convert vorbis encoded frames to 32-bit floating point PCM audio --- */
  void F32FromVorbisFrames(const ALfloat*const*const fpFramesIn,
    const size_t stFrames, const size_t stChannels, ALfloat *fpPCMOut)
  { // Convert ogg frames data to native PCM float 32-bit audio
    for(size_t stFrameIndex = 0; stFrameIndex < stFrames; ++stFrameIndex)
      for(size_t stChanIndex = 0; stChanIndex < stChannels; ++stChanIndex)
        *(fpPCMOut++) = fpFramesIn[stChanIndex][stFrameIndex];
  }
  /* -- Convert vorbis encoded frames to 16-bit integer PCM audio ---------- */
  void I16FromVorbisFrames(const ALfloat*const*const fpFramesIn,
    const size_t stFrames, const size_t stChannels, ALshort *wPCMOut)
  { // Convert ogg frames data to native PCM integer 16-bit audio
    for(size_t stFrameIndex = 0; stFrameIndex < stFrames; ++stFrameIndex)
      for(size_t stChanIndex = 0; stChanIndex < stChannels; ++stChanIndex)
        *(wPCMOut++) = UtilClamp(static_cast<ALshort>
          (rint(fpFramesIn[stChanIndex][stFrameIndex]*32767.f)),
          -32767, 32767);
  }
  /* -- Parse vorbis comments block ---------------------------------------- */
  StrNCStrMap VorbisParseComments(char **const clpPtr, const int iCount)
  { // Metadata to return
    StrNCStrMap ssMetaData;
    // Enumerate all the strings...
    StdForEach(seq, clpPtr, clpPtr+iCount, [&ssMetaData](char*const cpStr)
    { // Find equals delimiter and if we find it?
      if(char*const cpPtr = strchr(cpStr, '='))
      { // Remove separator (safe), add key/value pair and readd separator
        *cpPtr = '\0';
        ssMetaData.insert(ssMetaData.cend(), { cpStr, cpPtr+1 });
      } // We at least have a string so add it as key with empty value
      else ssMetaData.insert(ssMetaData.cend(),
        { cpStr, cCommon->CommonCBlank() });
    }); // Return built metadata
    return ssMetaData;
  }
  template<typename IntType>
    const string_view &GetOggErr(const IntType itCode) const
      { return Get(static_cast<unsigned int>(itCode)); }
  /* -- Loader for WAV files ----------------------------------------------- */
  bool Decode(FileMap &fmData, PcmData &pdData)
  { // Check magic and that the file has the OggS string header
    if(fmData.MemSize() < 4 || fmData.FileMapReadVar32LE() != 0x5367674FUL)
      return false;
    // Reset position for library to read it as well
    fmData.FileMapRewind();
    // Ogg handle
    OggVorbis_File vorbisFile;
    // Open ogg file and pass callbacks and if failed? Throw error
    if(const int iR =
      ov_open_callbacks(&fmData, &vorbisFile, nullptr, 0, GetCallbacks()))
        XC("OGG init context failed!", "Code", iR, "Reason", GetOggErr(iR));
    // Put in a unique ptr
    typedef unique_ptr<OggVorbis_File, function<decltype(ov_clear)>>
      OggFilePtr;
    const OggFilePtr ofpPtr{ &vorbisFile, ov_clear };
    // Get info from ogg
    const vorbis_info*const vorbisInfo = ov_info(&vorbisFile, -1);
    // Assign members
    pdData.SetRate(static_cast<unsigned int>(vorbisInfo->rate));
    if(!pdData.SetChannelsSafe(
          static_cast<PcmChannelType>(vorbisInfo->channels)))
      XC("OGG channels not valid!", "Channels", pdData.GetChannels());
    pdData.SetBits(PBI_SHORT);
    // Check that format is supported in OpenAL
    if(!pdData.ParseOALFormat())
      XC("OGG pcm data not supported by AL!",
        "Channels", pdData.GetChannels(), "Bits", pdData.GetBits());
    // Create PCM buffer (Not sure if multiplication is correct :[)
    const ogg_int64_t llSize =
      ov_pcm_total(&vorbisFile, -1) * (vorbisInfo->channels * 2);
    if(llSize < 0) XC("OGG has invalid pcm size!", "Size", llSize);
    // Allocate memory
    pdData.aPcmL.MemResize(static_cast<size_t>(llSize));
    // Decompress until done
    for(ogg_int64_t llPos = 0; llPos < llSize; )
    { // Read ogg stream and if not end of file?
      const size_t stToRead = static_cast<size_t>(llSize - llPos);
      if(const long lBytesRead = ov_read(&vorbisFile,
           pdData.aPcmL.MemRead(static_cast<size_t>(llPos), stToRead),
        static_cast<int>(stToRead), 0, 2, 1, nullptr))
      { // Error occured? Bail out
        if(lBytesRead < 0)
          XC("OGG decode failed!",
            "Error", lBytesRead, "Reason", GetOggErr(lBytesRead));
        // Move position onwards
        llPos += lBytesRead;
      } // End of file so break;
      else break;
    } // Get ogg comments and enumerate through each comment
    if(const vorbis_comment*const vorbisComment = ov_comment(&vorbisFile, -1))
      for(char*const *clpPtr = vorbisComment->user_comments;
        const char*const cpComment = *clpPtr; ++clpPtr)
          cLog->LogDebugExSafe("- $.", cpComment);
    // Success
    return true;
  }
  /* -- Constructor --------------------------------------------- */ protected:
  CodecOGG() :
    /* -- Initialisers ----------------------------------------------------- */
    PcmLib{ PFMT_OGG, "Xiph.Org OGG Audio", "OGG",
      bind(&CodecOGG::Decode, this, _1, _2) },
    IdMap{{                            // Ogg error codes
      IDMAPSTR(OV_EOF),                IDMAPSTR(OV_HOLE),
      IDMAPSTR(OV_FALSE),              IDMAPSTR(OV_EREAD),
      IDMAPSTR(OV_EFAULT),             IDMAPSTR(OV_EIMPL),
      IDMAPSTR(OV_EINVAL),             IDMAPSTR(OV_ENOTVORBIS),
      IDMAPSTR(OV_EBADHEADER),         IDMAPSTR(OV_EVERSION),
      IDMAPSTR(OV_ENOTAUDIO),          IDMAPSTR(OV_EBADPACKET),
      IDMAPSTR(OV_EBADLINK),           IDMAPSTR(OV_ENOSEEK)
    }, "OV_UNKNOWN" },
    ovcCallbacks{ VorbisRead, VorbisSeek, VorbisClose, VorbisTell }
    /* -- Set global pointer to static class ------------------------------- */
    { cCodecOGG = this; }
};/* ----------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
