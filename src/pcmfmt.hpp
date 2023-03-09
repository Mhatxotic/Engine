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
using namespace IFlags;                using namespace ILog::P;
using namespace IMemory::P;            using namespace IOal::P;
using namespace IPcmLib::P;            using namespace IStd::P;
using namespace IUtil::P;              using namespace Lib::MiniMP3;
using namespace Lib::Ogg;              using namespace Lib::OpenAL;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* ========================================================================= **
** ######################################################################### **
** ## Windows WAVE format                                             WAV ## **
** ######################################################################### **
** -- WAV Codec Object ----------------------------------------------------- */
class CodecWAV final :
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
  static bool Load(FileMap &fmData, PcmData &pdData)
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
    PcmLib{ PFMT_WAV, "Windows Wave Audio", "WAV", Load }
    /* -- No code ---------------------------------------------------------- */
    { }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(CodecWAV)            // Suppress default functions for safety
};/* -- End ---------------------------------------------------------------- */
/* ========================================================================= **
** ######################################################################### **
** ## Core Audio Format                                               CAF ## **
** ######################################################################### **
** -- CAF Codec Object ----------------------------------------------------- */
class CodecCAF final :
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
  static bool Load(FileMap &fmData, PcmData &pdData)
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
          if(dV < 1 || dV > 5644800)
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
                  "Bits", pdData.GetBits());
    } // Done
    return true;
  }
  /* -- Constructor ------------------------------------------------ */ public:
  CodecCAF(void) :
    /* -- Initialisers ----------------------------------------------------- */
    PcmLib{ PFMT_CAF, "CoreAudio Format", "CAF", Load }
    /* -- No code ---------------------------------------------------------- */
    { }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(CodecCAF)            // Suppress default functions for safety
};/* -- End ---------------------------------------------------------------- */
/* ========================================================================= **
** ######################################################################### **
** ## Ogg Vorbis                                                      OGG ## **
** ######################################################################### **
** -- OGG Codec Object ----------------------------------------------------- */
class CodecOGG final :
  /* -- Base classes ------------------------------------------------------- */
  private PcmLib                       // Pcm format helper class
{ /* -- Loader for WAV files --------------------------------------- */ public:
  static bool Load(FileMap &fmData, PcmData &pdData)
  { // Check magic and that the file has the OggS string header
    if(fmData.MemSize() < 4 || fmData.FileMapReadVar32LE() != 0x5367674FUL)
      return false;
    // Reset position for library to read it as well
    fmData.FileMapRewind();
    // Ogg handle
    OggVorbis_File vorbisFile;
    // Open ogg file and pass callbacks and if failed? Throw error
    if(const int iR = ov_open_callbacks(&fmData, &vorbisFile, nullptr, 0,
      PcmGetOggCallbacks()))
        XC("OGG init context failed!",
           "Code", iR, "Reason", cOal->GetOggErr(iR));
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
             "Error", lBytesRead, "Reason", cOal->GetOggErr(lBytesRead));
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
    PcmLib{ PFMT_OGG, "Xiph.Org OGG Audio", "OGG", Load }
    /* -- No code ---------------------------------------------------------- */
    { }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(CodecOGG)            // Suppress default functions for safety
};/* -- End ---------------------------------------------------------------- */
/* ========================================================================= **
** ######################################################################### **
** ## MPEG Layer-3                                                    MP3 ## **
** ######################################################################### **
** -- MP3 codec object ----------------------------------------------------- */
class CodecMP3 final :
  /* -- Base classes ------------------------------------------------------- */
  private PcmLib                       // Pcm format helper class
{ /* -- Loader for MP3 files ----------------------------------------------- */
  static bool Load(FileMap &fmData, PcmData &pdData)
  { // Check size of file
    const size_t stTotal = fmData.MemSize();
    if(UtilIntWillOverflow<int>(stTotal))
      XC("Pcm data size is not valid to fit in an integer!",
         "Maximum", numeric_limits<int>::max());
    // Create mp3 decoder context and if it succeeded?
    typedef unique_ptr<void, function<decltype(mp3_done)>> Mp3Ptr;
    if(const Mp3Ptr mpData{ mp3_create(), mp3_done })
    { // Calculate memory size maximum for a frame
      const size_t stFrame = MP3_MAX_SAMPLES_PER_FRAME * 8;
      // Number of bytes to read from input
      const size_t stLen = 65536;
      // Prepare PCM output buffer. We will increment this in 1MB chunks.
      const size_t stBufferIncrement = 1048576;
      pdData.aPcmL.MemInitBlank(stBufferIncrement);
      // Current position and bytes read
      size_t stPos = 0, stRead = 0;
      // Frame informations struct
      mp3_info_t mpInfo{};
      // How much data do we have left to read? Break if not
      while(const size_t stRemain = UtilMinimum(stLen, stTotal - stRead))
      { // Try to decode more data and break if we could not
        const int iBytes =
          mp3_decode(mpData.get(),                    // Context
            fmData.FileMapReadPtrFrom<void>(stRead, stRemain), // Input data
            static_cast<int>(stRemain),               // Size of input
            pdData.aPcmL.MemRead<short>(stPos, stFrame), // Output pcm data
            &mpInfo);                                 // Frame data
        if(iBytes <= 0) break;
        // PCM bytes are available?
        if(mpInfo.audio_bytes != -1)
        { // Append to PCM buffer and increment PCM buffer position
          stPos += static_cast<size_t>(mpInfo.audio_bytes);
          // Increase memory if we're expected to overrun again
          if(stPos + stFrame > pdData.aPcmL.MemSize())
            pdData.aPcmL.MemResizeUp(pdData.aPcmL.MemSize() +
              stBufferIncrement);
        } // If we didn't move, it's probably not a mp3 file
        else if(!stPos) return false;
        // Add to bytes read
        stRead += static_cast<size_t>(iBytes);
      } // Check if valid MP3 and return as not mp3 file if not.
      if(!pdData.SetChannelsSafe(static_cast<PcmChannelType>(mpInfo.channels)))
        return false;
      // Shrink memory block to fit
      pdData.aPcmL.MemResize(stPos);
      // Set sample rate and bitrate (always 16-bit).
      pdData.SetRate(static_cast<unsigned int>(mpInfo.sample_rate));
      pdData.SetBits(PBI_SHORT);
      // Check that format is supported in OpenAL
      if(!pdData.ParseOALFormat())
        XC("MP3 pcm data not supported by AL!",
           "Channels", pdData.GetChannels(), "Bits", pdData.GetBits());
      // Successfully decoded
      return true;
    } // Failed to setup MP3 decoder so throw an error
    XC("Failed to initialise MP3 decoder!");
  }
  /* -- Constructor ------------------------------------------------ */ public:
  CodecMP3(void) :
    /* -- Initialisers ----------------------------------------------------- */
    PcmLib{ PFMT_MP3, "Motion Picture Experts Group layer-III", "MP3", Load }
    /* -- No code ---------------------------------------------------------- */
    { }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(CodecMP3)            // Suppress default functions for safety
};/* ----------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
