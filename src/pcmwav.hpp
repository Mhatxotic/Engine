/* == PCMFMWAV.HPP ========================================================= **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## Handles loading and saving of .WAV files with the PcmLib system.    ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace ICodecWAV {                  // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IError::P;             using namespace IFileMap::P;
using namespace IIdent::P;             using namespace IFlags;
using namespace ILog::P;               using namespace IMemory::P;
using namespace IOal::P;               using namespace IPcmDef::P;
using namespace IPcmLib::P;            using namespace IStd::P;
using namespace IString::P;            using namespace IUtil::P;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* ========================================================================= **
** ######################################################################### **
** ## Windows WAVE format                                             WAV ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
class CodecWAV;                        // Class prototype
static CodecWAV *cCodecWAV = nullptr;  // Pointer to global class
class CodecWAV :                       // WAV codec object
  /* -- Base classes ------------------------------------------------------- */
  protected PcmLib                     // Pcm format helper class
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
  /* -- Constructor --------------------------------------------- */ protected:
  CodecWAV() :
    /* -- Initialisers ----------------------------------------------------- */
    PcmLib{ PFMT_WAV, "Windows Wave Audio", "WAV",
      bind(&CodecWAV::Decode, this, _1, _2), }
    /* -- Set global pointer to static class ------------------------------- */
    { cCodecWAV = this; }
};/* ----------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
