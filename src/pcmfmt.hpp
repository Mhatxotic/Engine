/* == PCMFMT.HPP =========================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## These classes are plugins for the PcmLib manager to allow loading   ## **
** ## of certain formatted audio files.                                   ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IPcmFormat {                 // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IError::P;             using namespace IFileMap::P;
using namespace IIdent::P;             using namespace IFlags;
using namespace ILog::P;               using namespace IMemory::P;
using namespace IOal::P;               using namespace IPcmDef::P;
using namespace IPcmLib::P;            using namespace IStd::P;
using namespace IString::P;            using namespace IUtil::P;
using namespace Lib::OS::MiniMP3;      using namespace Lib::Ogg;
using namespace Lib::OpenAL::Types;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* ========================================================================= **
** ######################################################################### **
** ## Windows WAVE format                                             WAV ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
static class CodecWAV final :          // WAV codec object
  /* -- Base classes ------------------------------------------------------- */
  private PcmLib                       // Pcm format helper class
{ /* -- WAV header layout -------------------------------------------------- */
  enum HeaderLayout
  { // *** WAV FILE LIMITS ***
    HL_MINIMUM         =           20, // Minimum header size
    HL_U32LE_MINRATE   =            1, // Minimum sample rate allowed
    HL_U32LE_MAXRATE   =      5644800, // Maximum sample rate allowed
    // *** RIFF FILE HEADER (12 bytes) ***
    HL_U32LE_MAGIC     =            0, // Magic identifier
    HL_U32LE_SIZE      =            4, // Size of segment (without RIFF)
    HL_U32LE_TYPE      =            8, // Type of RIFF (usually 'WAVE')
    // *** WAVE CHUNK HEADER (8 bytes) ***
    HL_U32LE_CNK_MAGIC =            0, // Type of chunk (HL_U32LE_CNKT_*)
    HL_U32LE_CNK_SIZE  =            4, // Size of chunk data
    // *** WAVE FORMAT STRUCT (RIFF(12)+WCH(8)+?? bytes) ***
    HL_U16LE_FORMAT    =            0, // Wave format id (see below)
    HL_U16LE_CHANNELS  =            2, // Bits per channel
    HL_U32LE_RATE      =            4, // Sample rate
    HL_U32LE_AVGBPS    =            8, // Average bits per second (?)
    HL_U16LE_BLKALIGN  =           12, // Block align (?)
    HL_U16LE_BITPERSAM =           14, // Bits per sample
    // *** WAVE FORMAT TYPE (HL_U16LE_FORMAT) ***
    HL_U16LE_WF_PCM    =            1, // Integer PCM type
    HL_U16LE_WF_FLOAT  =            3, // Floating point IEEE format type
    // *** Header values ***
    HL_U32LE_V_RIFF    =   0x46464952, // RIFF magic ('RIFF')
    HL_U32LE_V_WAVE    =   0x45564157, // WAV format magic ('WAVE')
    HL_U32LE_CNKT_FMT  =   0x20746D66, // Wave format ('fmt ')
    HL_U32LE_CNKT_DATA =   0x61746164, // Pcm data chunk ('data')
  };
  /* -- Loader for WAV files ----------------------------------------------- */
  bool Decode(FileMap &fmData, PcmData &pdData)
  { // Must be at least 20 bytes and test RIFF header magic
    if(fmData.MemSize() < HL_MINIMUM ||
       fmData.FileMapReadVar32LE() != HL_U32LE_V_RIFF)
      return false;
    // Check size of chunk data matches available size
    const size_t stTwoDWords = sizeof(uint32_t) * 2,
                 stExpectedLength = fmData.MemSize() - stTwoDWords,
                 stActualLength = fmData.FileMapReadVar32LE();
    if(stActualLength != stExpectedLength)
      XC("WAVE file length mismatch with actual file length!",
         "Expected", stExpectedLength, "Actual", stActualLength);
    // Make sure its a MSWin WAV formatted file
    const unsigned int uiExpectedMagic = HL_U32LE_V_WAVE,
                       uiActualMagic = fmData.FileMapReadVar32LE();
    if(uiActualMagic != uiExpectedMagic)
      XC("RIFF must have WAV formatted data!",
         "Expected", uiExpectedMagic, "Actual", uiActualMagic);
    // Flag for if we got the data chunks we need
    BUILD_FLAGS(WaveLoad, WL_NONE{1}, WL_GOTFORMAT{2}, WL_GOTDATA{4});
    WaveLoadFlags chunkFlags{ WL_NONE };
    // The RIFF file contains dynamic 'chunks' of data. We need to iterate
    // through each one until we can no longer read any more chunks. We need
    // to make sure we can read at least two dwords every time.
    for(size_t stHeaderPos = fmData.FileMapTell();
               stHeaderPos + stTwoDWords < fmData.MemSize();
               stHeaderPos = fmData.FileMapTell())
    { // Get chunk header and size
      const unsigned int uiHeader = fmData.FileMapReadVar32LE(),
                         uiSize = fmData.FileMapReadVar32LE();
      // Which chunk is it?
      switch(uiHeader)
      { // Is it the wave format chunk?
        case HL_U32LE_CNKT_FMT:
        { // Check that the chunk will fit into WAVEHDR
          if(uiSize < 16)
            XC("Wave format chunk size invalid!", "HeaderSize", uiSize);
          // Compare wave data format type
          switch(const unsigned int uiFormatTag = fmData.FileMapReadVar16LE())
          { // Must be PCM or floating-point type.
            case HL_U16LE_WF_PCM: case HL_U16LE_WF_FLOAT: break;
            // Unsupported type?
            default:
              XC("Wave format must be either integer PCM or IEEE FLOAT!",
                 "Actual", uiFormatTag);
          } // Wave format data. Can't use #pragma pack nowadays :-(
          if(!pdData.SetChannelsSafe(
               static_cast<PcmChannelType>(fmData.FileMapReadVar16LE())))
             XC("Wave format has invalid channel count!",
               "Channels", pdData.GetChannels());
          // Check sample rate
          pdData.SetRate(fmData.FileMapReadVar32LE());
          if(pdData.GetRate() < HL_U32LE_MINRATE ||
             pdData.GetRate() > HL_U32LE_MAXRATE)
            XC("Wave format has invalid sample rate!",
               "Rate", pdData.GetRate(), "Minimum", HL_U32LE_MINRATE,
               "Maximum", HL_U32LE_MAXRATE);
          // Get bytes per second and block align
          const unsigned int uiAvgBytesPerSec = fmData.FileMapReadVar32LE();
          const unsigned int uiBlockAlign = fmData.FileMapReadVar16LE();
          // Get bits and calculate bytes per channel
          pdData.SetBits(static_cast<PcmBitType>(fmData.FileMapReadVar16LE()));
          // Get and check bytes per second
          const unsigned int uiCalcAvgBytes =
            pdData.GetRate() * pdData.GetChannels() * pdData.GetBytes();
          if(uiCalcAvgBytes != uiAvgBytesPerSec)
            XC("Average bytes per second mismatch!",
               "Rate",     pdData.GetRate(),
               "Channels", pdData.GetChannels(),
               "BitsPC",   pdData.GetBits(), "BytesPC",    pdData.GetBytes(),
               "Expected", uiAvgBytesPerSec, "Calculated", uiCalcAvgBytes);
          // Check block align
          const unsigned int uiCalcBlockAlign =
            static_cast<unsigned int>(pdData.GetChannels()) *
              pdData.GetBytes();
          if(uiCalcBlockAlign != uiBlockAlign)
            XC("Block align size mismatch!",
               "Channels",   pdData.GetChannels(),
               "BitsPC",     pdData.GetBits(),
               "BytesPC",    pdData.GetBytes(), "Expected", uiBlockAlign,
               "Calculated", uiCalcBlockAlign);
          // Determine openal format type from the WAV file structure
          if(!pdData.ParseOALFormat())
            XC("Wave format not supported by AL!",
               "Channels", pdData.GetChannels(), "Bits", pdData.GetBits());
          // We got the format chunk
          chunkFlags.FlagSet(WL_GOTFORMAT);
          // Done
          break;
        } // This is a data chunk?
        case HL_U32LE_CNKT_DATA:
        { // Store pcm data, mark that we got the data chunk and break
          pdData.aPcmL.MemInitData(uiSize, fmData.FileMapReadPtr(uiSize));
          chunkFlags.FlagSet(WL_GOTDATA);
          break;
        } // Unknown chunk?
        default:
        { // Report that we're ignoring it and goto next header
          cLog->LogDebugExSafe(
            "Pcm ignored unknown RIFF header 0x$$<$$> in '$'!",
            hex, uiHeader, dec, uiHeader, fmData.IdentGet());
          break;
        }
      }
      // Set position of next header. This skips any extra padding we did
      // not read and also protects from overruning.
      fmData.FileMapSeekSet(stHeaderPos + stTwoDWords + uiSize);
    } // Throw error if we did not get a 'fmt' chunk?
    if(chunkFlags.FlagIsClear(WL_GOTFORMAT)) XC("WAV has no 'fmt' chunk!");
    // Throw error if we did not get a 'data' chunk?
    if(chunkFlags.FlagIsClear(WL_GOTDATA)) XC("WAV has no 'data' chunk!");
    // Success
    return true;
  }
  /* -- Constructor ------------------------------------------------ */ public:
  CodecWAV(void) :
    /* -- Initialisers ----------------------------------------------------- */
    PcmLib{ PFMT_WAV, "Windows Wave Audio", "WAV",
      bind(&CodecWAV::Decode, this, _1, _2), }
    /* -- No code ---------------------------------------------------------- */
    { }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(CodecWAV)            // Suppress default functions for safety
  /* -- End ---------------------------------------------------------------- */
} *cCodecWAV = nullptr;                // Codec pointer
/* ========================================================================= **
** ######################################################################### **
** ## Core Audio Format                                               CAF ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
static class CodecCAF final :          // CAF codec object
  /* -- Base classes ------------------------------------------------------- */
  private PcmLib                       // Pcm format helper class
{ /* -- CAF header layout -------------------------------------------------- */
  enum HeaderLayout
  { // *** CAF FILE LIMITS ***
    HL_MINIMUM           =         44, // Minimum header size
    // *** CAF FILE HEADER (8 bytes). All values are big-endian!!! ***
    HL_U32LE_MAGIC       =          0, // Magic identifier (0x66666163)
    HL_U32LE_VERSION     =          4, // CAF format version (0x00010000)
    // *** CAF CHUNK 'desc' (32 bytes) ***
    HL_F64BE_RATE        =          8, // (00) Sample rate (1-5644800)
    HL_U32BE_TYPE        =         16, // (08) PcmData type ('lpcm'=0x6D63706C)
    HL_U32BE_FLAGS       =         20, // (12) Flags
    HL_U32BE_BYT_PER_PKT =         24, // (16) Bytes per packet
    HL_U32BE_FRM_PER_PKT =         28, // (20) Frames per packet
    HL_U32BE_CHANNELS    =         32, // (24) Audio channel count
    HL_U32BE_BITS_PER_CH =         36, // (28) Bits per channel
    // *** CAF CHUNK 'data' (8+?? bytes) ***
    HL_U64BE_DATA_SIZE   =          8, // (00) Size of data
    HL_U8ARR_DATA_BLOCK  =         16, // (08) Pcm data of size 'ullSize'
    // *** Header values ***
    HL_U32LE_V_MAGIC     = 0x66666163, // Primary header magic
    HL_U32BE_V_V1        = 0x00010000, // Allowed version
    HL_U32LE_V_DESC      = 0x63736564, // 'desc' chunk id
    HL_U32LE_V_LPCM      = 0x6D63706C, // 'desc'->'LPCM' sub-chunk id
    HL_U32LE_V_DATA      = 0x61746164, // 'data' chunk id
  };
  /* -- Loader for CAF files ----------------------------------------------- */
  bool Decode(FileMap &fmData, PcmData &pdData)
  { // CAF data endian types
    BUILD_FLAGS(Header, HF_NONE{0}, HF_ISFLOAT{1}, HF_ISPCMLE{2});
    // Check size at least 44 bytes for a file header, and 'desc' chunk and
    // a 'data' chunk of 0 bytes
    if(fmData.MemSize() < HL_MINIMUM ||
       fmData.FileMapReadVar32LE() != HL_U32LE_V_MAGIC)
      return false;
    // Check flags and bail if not version 1 CAF. Caf data is stored in reverse
    // byte order so we need to reverse it correctly. Although we should
    // reference the variable normally. We cannot because we have to modify it.
    const unsigned int ulVersion = fmData.FileMapReadVar32BE();
    if(ulVersion != HL_U32BE_V_V1)
      XC("CAF version not supported!",
        "Expected", HL_U32BE_V_V1, "Actual", ulVersion);
    // Detected file flags
    HeaderFlags hPcmFmtFlags{ HF_NONE };
    // The .caf file contains dynamic 'chunks' of data. We need to iterate
    // through each one until we hit the end-of-file.
    while(fmData.FileMapIsNotEOF())
    { // Get magic which we will test
      const unsigned int uiMagic = fmData.FileMapReadVar32LE();
      // Read size and if size is too big for machine to handle? Log warning.
      const uint64_t qSize = fmData.FileMapReadVar64BE();
      if(UtilIntWillOverflow<size_t>(qSize))
        cLog->LogWarningExSafe("Pcm CAF chunk too big $ > $!",
          qSize, StdMaxSizeT);
      // Accept maximum size the machine allows
      const size_t stSize = UtilIntOrMax<size_t>(qSize);
      // test the header chunk
      switch(uiMagic)
      { // Is it the 'desc' chunk?
        case HL_U32LE_V_DESC:
        { // Check that the chunk is at least 32 bytes.
          if(stSize < 32) XC("CAF 'desc' chunk needs >=32 bytes!");
          // Get sample rate as double convert from big-endian.
          const double dV =
            UtilCastInt64ToDouble(fmData.FileMapReadVar64BE());
          if(dV < 1.0 || dV > 5644800.0)
            XC("CAF sample rate invalid!", "Rate", dV);
          pdData.SetRate(static_cast<ALuint>(dV));
          // Check that FormatType(4) is 'lpcm'.
          const unsigned int ulHdr = fmData.FileMapReadVar32LE();
          if(ulHdr != HL_U32LE_V_LPCM)
            XC("CAF data chunk type not supported!",
              "Expected", HL_U32LE_V_LPCM, "Header", ulHdr);
          // Check that FormatFlags(4) is valid
          hPcmFmtFlags.FlagReset(
            static_cast<HeaderFlags>(fmData.FileMapReadVar32BE()));
          // Check that BytesPerPacket(4) is valid
          const unsigned int ulBPP = fmData.FileMapReadVar32BE();
          if(ulBPP != 4)
            XC("CAF bpp of 4 only supported!", "Bytes", ulBPP);
          // Check that FramesPerPacket(4) is 1
          const unsigned int ulFPP = fmData.FileMapReadVar32BE();
          if(ulFPP != 1)
            XC("CAF fpp of 1 only supported!", "Frames", ulFPP);
          // Update settings
          if(!pdData.SetChannelsSafe(
               static_cast<PcmChannelType>(fmData.FileMapReadVar32BE())))
            XC("CAF format has invalid channel count!",
               "Channels", pdData.GetChannels());
          // Read bits per sample and check that format is supported in OpenAL
          pdData.SetBits(static_cast<PcmBitType>(fmData.FileMapReadVar32BE()));
          if(!pdData.ParseOALFormat())
            XC("CAF pcm data un-supported by AL!",
               "Channels", pdData.GetChannels(), "Bits", pdData.GetBits());
          // Done
          break;
        } // Is it the 'data' chunk?
        case HL_U32LE_V_DATA:
        { // Store pcm data and break
          pdData.aPcmL.MemInitData(stSize, fmData.FileMapReadPtr(stSize));
          break;
        } // Unknown header so ignore unknown channel and break
        default: fmData.FileMapSeekCur(stSize); break;
      }
    } // Got \desc\ chunk?
    if(!pdData.GetRate()) XC("CAF has no 'desc' chunk!");
    // Got 'data' chunk?
    if(pdData.aPcmL.MemIsEmpty()) XC("CAF has no 'data' chunk!");
    // Type of endianness conversion required for log (if required)
    const char *cpConversion;
    // If data is in little-endian mode?
    if(hPcmFmtFlags.FlagIsSet(HF_ISPCMLE))
    { // ... and using a little-endian cpu?
#ifdef LITTLEENDIAN
      // No conversion needed
      return true;
#else
      // Set conversion label
      cpConversion = "little to big";
#endif
    } // If data is in big-endian mode?
    else
    { // ... and using big-endian cpu?
#ifdef BIGENDIAN
      // No conversion needed
      return true;
#else
      // Set conversion label
      cpConversion = "big to little";
#endif
    } // Compare bitrate
    switch(pdData.GetBits())
    { // No conversion required if 8-bits per channel
      case 8: break;
      // 16-bits per channel (2 bytes)
      case 16:
        // Log and perform byte swap
        cLog->LogDebugExSafe(
          "Pcm performing 16-bit $ byte-order conversion...", cpConversion);
        pdData.aPcmL.MemByteSwap16();
        break;
      // 32-bits per channel (4 bytes)
      case 32:
        // Log and perform byte swap
        cLog->LogDebugExSafe(
          "Pcm performing 32-bit $ byte-order conversion...", cpConversion);
        pdData.aPcmL.MemByteSwap32();
        break;
      // Not supported
      default: XC("Pcm bit count not supported for endian conversion!",
                  "Bits", pdData.GetBits(), "Type", cpConversion);
    } // Done
    return true;
  }
  /* -- Constructor ------------------------------------------------ */ public:
  CodecCAF(void) :
    /* -- Initialisers ----------------------------------------------------- */
    PcmLib{ PFMT_CAF, "CoreAudio Format", "CAF",
      bind(&CodecCAF::Decode, this, _1, _2) }
    /* -- No code ---------------------------------------------------------- */
    { }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(CodecCAF)            // Suppress default functions for safety
  /* -- End ---------------------------------------------------------------- */
} *cCodecCAF = nullptr;                // Codec pointer
/* ========================================================================= **
** ######################################################################### **
** ## Ogg Vorbis                                                      OGG ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
static class CodecOGG final :          // OGG codec object
  /* -- Base classes ------------------------------------------------------- */
  private PcmLib,                      // Pcm format helper class
  private IdMap<>                      // Ogg error codes
{ /* -- Private variables ------------------------------------------ */ public:
  const ov_callbacks ovcCallbacks;     // Vorbis callbacks
  /* -- Vorbis read callback --------------------------------------- */ public:
  static size_t VorbisRead(void*const vpPtr,
    size_t stSize, size_t stCount, void*const vFmClassPtr)
      { return reinterpret_cast<FileMap*>(vFmClassPtr)->
          FileMapReadToAddr(vpPtr, stSize * stCount); }
  /* -- Vorbis seek callback ----------------------------------------------- */
  static int VorbisSeek(void*const vFmClassPtr, ogg_int64_t qOffset, int iLoc)
    { return static_cast<int>(reinterpret_cast<FileMap*>(vFmClassPtr)->
        FileMapSeek(static_cast<size_t>(qOffset), iLoc)); }
  /* -- Vorbis close callback ---------------------------------------------- */
  static int VorbisClose(void*const) { return 1; }
  /* -- Vorbis tell callback ----------------------------------------------- */
  static long VorbisTell(void*const vFmClassPtr)
    { return static_cast<long>(reinterpret_cast<FileMap*>(vFmClassPtr)->
        FileMapTell()); }
  /* -- Return generic ogg callback functions ------------------------------ */
  const ov_callbacks &GetCallbacks(void) { return ovcCallbacks; }
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
      else ssMetaData.insert(ssMetaData.cend(), { cpStr, cCommon->CBlank() });
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
    if(const int iR = ov_open_callbacks(&fmData, &vorbisFile, nullptr, 0,
      GetCallbacks()))
        XC("OGG init context failed!",
           "Code", iR, "Reason", GetOggErr(iR));
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
    const ogg_int64_t qwSize =
      ov_pcm_total(&vorbisFile, -1) * (vorbisInfo->channels * 2);
    if(qwSize < 0) XC("OGG has invalid pcm size!", "Size", qwSize);
    // Allocate memory
    pdData.aPcmL.MemResize(static_cast<size_t>(qwSize));
    // Decompress until done
    for(ogg_int64_t qwPos = 0; qwPos < qwSize; )
    { // Read ogg stream and if not end of file?
      const size_t stToRead = static_cast<size_t>(qwSize - qwPos);
      if(const long lBytesRead = ov_read(&vorbisFile,
           pdData.aPcmL.MemRead(static_cast<size_t>(qwPos), stToRead),
        static_cast<int>(stToRead), 0, 2, 1, nullptr))
      { // Error occured? Bail out
        if(lBytesRead < 0)
          XC("OGG decode failed!",
             "Error", lBytesRead, "Reason", GetOggErr(lBytesRead));
        // Move position onwards
        qwPos += lBytesRead;
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
  /* -- Constructor -------------------------------------------------------- */
  CodecOGG(void) :
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
    /* -- No code ---------------------------------------------------------- */
    { }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(CodecOGG)            // Suppress default functions for safety
  /* -- End ---------------------------------------------------------------- */
} *cCodecOGG = nullptr;                // Codec pointer
/* ========================================================================= **
** ######################################################################### **
** ## MPEG Layer-3                                                    MP3 ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
static class CodecMP3 final :          // MP3 codec object
  /* -- Base classes ------------------------------------------------------- */
  private PcmLib,                      // Pcm format helper class
  private IdMap<int>                   // MiniMP3 error codes
{ /* -- Loader for MP3 files ----------------------------------------------- */
  bool Decode(FileMap &fmData, PcmData &pdData)
  { // Limit specified file size as 'int' and limit it to 2GB because the
    // original Minimp3 headers are littered with unsafe type conversions so
    // we changed all references of 'size_t', 'uint64_t', 'unsigned' to 'int'
    // to here to make the call safe and optimal.
    const int iSize = UtilIntOrMax<int>(fmData.MemSize());
    // See if it's an MP3 first and return if it is not
    if(mp3dec_detect_buf(fmData.MemPtr<uint8_t>(), iSize)) return false;
    // Initialise per-thread context and file data
    mp3dec_t mpdContext;
    mp3dec_init(&mpdContext);
    mp3dec_file_info_t mfiData;
    // Initialise context
    if(const int iResult = mp3dec_load_buf(&mpdContext,
      fmData.MemPtr<uint8_t>(), iSize, &mfiData, nullptr, nullptr))
        XC("Failure decoding MPEG data!",
            "Code", iResult, "Reason", Get(iResult));
    // This decoder doesn't track the buffer it allocated so we'll just take it
    pdData.aPcmL =
      { static_cast<size_t>(mfiData.samples)*sizeof(mp3d_sample_t),
        mfiData.buffer };
    // Error if invalid channels
    if(!pdData.SetChannelsSafe(static_cast<PcmChannelType>(mfiData.channels)))
      XC("Invalid MPEG channel count!", "Channels", mfiData.channels);
    // Set sample rate and bitrate (always 16-bit).
    pdData.SetRate(static_cast<unsigned int>(mfiData.hz));
    pdData.SetBits(PBI_SHORT);
    // Check that format is supported in OpenAL
    if(!pdData.ParseOALFormat())
      XC("MPEG decoded PCM data not supported by AL!",
         "Channels", pdData.GetChannels(), "Bits", pdData.GetBits());
    // Successfully decoded
    return true;
  }
  /* -- Constructor ------------------------------------------------ */ public:
  CodecMP3(void) :
    /* -- Initialisers ----------------------------------------------------- */
    PcmLib{ PFMT_MP3, "MPEG Layer 1-3 Audio", "MP3",
      bind(&CodecMP3::Decode, this, _1, _2) },
    IdMap{{                            // Mp3 error codes
      IDMAPSTR(MP3D_E_PARAM),          IDMAPSTR(MP3D_E_MEMORY),
      IDMAPSTR(MP3D_E_IOERROR),        IDMAPSTR(MP3D_E_USER),
      IDMAPSTR(MP3D_E_DECODE)
    }, "MP3D_E_UNKNOWN" }
    /* -- No code ---------------------------------------------------------- */
    {  }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(CodecMP3)            // Suppress default functions for safety
  /* -- End ---------------------------------------------------------------- */
} *cCodecMP3 = nullptr;                // Codec pointer
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
