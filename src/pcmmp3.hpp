/* == PCMFMMP3.HPP ========================================================= **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## Handles loading and saving of .MP3 files with the PcmLib system.    ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace ICodecMP3 {                  // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IError::P;             using namespace IFileMap::P;
using namespace IIdent::P;             using namespace IPcmDef::P;
using namespace IPcmLib::P;            using namespace IUtil::P;
using namespace Lib::OS::MiniMP3;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
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
  /* -- End ---------------------------------------------------------------- */
} *cCodecMP3 = nullptr;                // Codec pointer
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
