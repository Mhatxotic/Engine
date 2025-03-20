/* == PCMFMCAF.HPP ========================================================= **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## Handles loading and saving of .CAF files with the PcmLib system.    ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace ICodecCAF {                  // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IError::P;             using namespace IFileMap::P;
using namespace IFlags;                using namespace ILog::P;
using namespace IMemory::P;            using namespace IPcmDef::P;
using namespace IPcmLib::P;            using namespace IStd::P;
using namespace IUtil::P;              using namespace Lib::OpenAL::Types;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* ------------------------------------------------------------------------- */
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
    BUILD_FLAGS(Header,
      HF_NONE{ Flag(0) }, HF_ISFLOAT{ Flag(1) }, HF_ISPCMLE{ Flag(2) });
    static_cast<void>(HF_ISFLOAT); // Unused
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
  /* -- End ---------------------------------------------------------------- */
} *cCodecCAF = nullptr;                // Codec pointer
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
