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
using namespace IFlags::P;             using namespace ILog::P;
using namespace IMemory::P;            using namespace IPcmDef::P;
using namespace IPcmLib::P;            using namespace IStd::P;
using namespace IUtil::P;              using namespace Lib::OpenAL::Types;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* ------------------------------------------------------------------------- */
class CodecCAF;                        // Class prototype
static CodecCAF *cCodecCAF = nullptr;  // Pointer to global class
class CodecCAF :                       // CAF codec object
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
    HL_U32LE_V_MAGIC     = 0x66666163, // 'caff' Primary header magic
    HL_U32BE_V_V1        = 0x00010000, // Allowed version
    HL_U32LE_V_DESC      = 0x63736564, // 'desc' chunk id
    HL_U32LE_V_LPCM      = 0x6D63706C, // 'lpcm' chunk id
    HL_U32LE_V_DATA      = 0x61746164, // 'data' chunk id
  };
  /* -- Loader for CAF files ----------------------------------------------- */
  bool Decode(FileMap &fmData, PcmData &pdData)
  { // CAF data endian types
    BUILD_FLAGS(Header,
      HF_NONE             { Flag(0) }, HF_ISFLOAT          { Flag(1) },
      HF_ISPCMLE          { Flag(2) }, HF_SIGNEDINT        { Flag(3) },
      HF_PACKED           { Flag(4) }, HF_HIGH             { Flag(5) },
      HF_NONINTERLEAVE    { Flag(6) },
      HF_MASK{ HF_ISFLOAT|HF_ISPCMLE|HF_SIGNEDINT|HF_PACKED|HF_HIGH|
               HF_NONINTERLEAVE });
    // Check size at least 44 bytes for a file header, and 'desc' chunk and
    // a 'data' chunk of 0 bytes
    if(fmData.MemSize() < HL_MINIMUM ||
       fmData.FileMapReadVar32LE() != HL_U32LE_V_MAGIC)
      return false;
    // Check flags and bail if not version 1 CAF. Caf data is stored in reverse
    // byte order so we need to reverse it correctly. Although we should
    // reference the variable normally. We cannot because we have to modify it.
    switch(const unsigned uVersion = fmData.FileMapReadVar32BE())
    { // Ok if it's a version 1 CAF?
      case HL_U32BE_V_V1: break;
      // Anything else is unsupported
      default: XC("CAF version not supported!",
        "Expected", HL_U32BE_V_V1, "Actual", uVersion);
    } // Detected file flags
    HeaderFlags hPcmFmtFlags{ HF_NONE };
    // The .caf file contains dynamic 'chunks' of data. We need to iterate
    // through each one until we hit the end-of-file.
    while(fmData.FileMapIsNotEOF())
    { // Get magic which we will test
      const unsigned uMagic = fmData.FileMapReadVar32LE();
      // Read size and if size is too big for machine to handle? Log warning.
      const uint64_t ullSize = fmData.FileMapReadVar64BE();
      if(UtilIntWillOverflow<size_t>(ullSize))
        cLog->LogWarningExSafe("Pcm CAF chunk too big $ > $!",
          ullSize, StdMaxSizeT);
      // Accept maximum size the machine allows
      const size_t stSize = UtilIntOrMax<size_t>(ullSize);
      // test the header chunk
      switch(uMagic)
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
          switch(const unsigned uHdr = fmData.FileMapReadVar32LE())
          { // Only the correct format time is allowed
            case HL_U32LE_V_LPCM: break;
            // Anything else is unsupported
            default: XC("CAF data chunk type not supported!",
              "Expected", HL_U32LE_V_LPCM, "Header", uHdr);
          } // Check that FormatFlags(4) is valid
          hPcmFmtFlags.FlagReset(
            static_cast<HeaderFlags>(fmData.FileMapReadVar32BE()));
          // Check flags
          if(hPcmFmtFlags.FlagIsNotInMask(HF_MASK))
            XC("Invalid flags specified!",
              "Flags", hPcmFmtFlags.FlagGet(), "Mask", HF_MASK.FlagGet());
          if(hPcmFmtFlags.FlagIsSet(HF_PACKED))
            XC("Packed bits not supported!",
              "Flags", hPcmFmtFlags.FlagGet());
          if(hPcmFmtFlags.FlagIsSet(HF_NONINTERLEAVE))
            XC("Non-interleave bits not supported!",
              "Flags", hPcmFmtFlags.FlagGet());
          if(hPcmFmtFlags.FlagIsSet(HF_HIGH))
            XC("High bits not supported!",
              "Flags", hPcmFmtFlags.FlagGet());
          // Get bytes per packet and make sure it is valid
          switch(const PcmByteType bptBPP =
            static_cast<PcmByteType>(fmData.FileMapReadVar32BE()))
          { // Only 1, 2 and 4 bytes supported right now
            case PBY_BYTE: case PBY_SHORT: case PBY_LONG: break;
            // Anything else is unsupported
            case PBY_NONE: default:
              XC("CAF bytes per packet not supported!", "Bytes", bptBPP);
          } // Check that FramesPerPacket(4) is 1
          switch(const unsigned uFPP = fmData.FileMapReadVar32BE())
          { // Only 1 allowed
            case 1: break;
            // Anything else is unsupported
            default: XC("CAF fpp of 1 only supported!", "Frames", uFPP);
          } // Update settings
          if(!pdData.SetChannelsSafe(
               static_cast<PcmChannelType>(fmData.FileMapReadVar32BE())))
            XC("CAF format has invalid channel count!",
              "Channels", pdData.GetChannels());
          // Read bits per sample and check that format is supported in OpenAL
          pdData.SetBits(static_cast<PcmBitType>(fmData.FileMapReadVar32BE()));
          switch(pdData.GetBits())
          { // Only 8-bit, 16-bit and 32-bit allowed
            case PBI_BYTE:
              if(hPcmFmtFlags.FlagIsZero()) pdData.SetSigned();
              break;
            case PBI_SHORT:
              if(hPcmFmtFlags.FlagIsClear(HF_ISPCMLE)) pdData.SetBigEndian();
              if(hPcmFmtFlags.FlagIsSet(HF_SIGNEDINT)) pdData.SetSigned();
              break;
            case PBI_LONG:
              if(hPcmFmtFlags.FlagIsSet(HF_ISFLOAT))
                XC("Pcm data must be integer 32bps!",
                  "Flags", hPcmFmtFlags.FlagGet());
              if(hPcmFmtFlags.FlagIsClear(HF_ISPCMLE)) pdData.SetBigEndian();
              if(hPcmFmtFlags.FlagIsSet(HF_SIGNEDINT)) pdData.SetSigned();
              break;
            case PBI_NONE: default:
              XC("Invalid sample bit-depth!", "Depth", pdData.GetBits());
          } // Done
          break;
        } // Is it the 'data' chunk?
        case HL_U32LE_V_DATA:
        { // Store pcm data and break
          pdData.aPcmL.MemInitData(stSize, fmData.FileMapReadPtr(stSize));
          break;
        } // Unknown header so ignore unknown channel and break
        default:
        { // Report that we're ignoring it and goto next header
          cLog->LogDebugExSafe(
            "Pcm ignored unknown CAF header 0x$$<$$> in '$'!",
            StdIOSHex, uMagic, StdIOSDec, uMagic, fmData.NameGet());
          fmData.FileMapSeekCur(stSize);
          break;
        }
      }
    } // Check that we got the chunks we care about?
    if(!pdData.GetRate()) XC("CAF has no 'desc' chunk!");
    if(pdData.aPcmL.MemIsEmpty()) XC("CAF has no 'data' chunk!");
    // Done
    return true;
  }
  /* -- Constructor --------------------------------------------- */ protected:
  CodecCAF() :
    /* -- Initialisers ----------------------------------------------------- */
    PcmLib{ PFMT_CAF, "CoreAudio Format", "CAF",
      bind(&CodecCAF::Decode, this, _1, _2) }
    /* -- Set global pointer to static class ------------------------------- */
    { cCodecCAF = this; }
};/* ----------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
