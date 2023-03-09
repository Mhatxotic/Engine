/* == PCMDEF.HPP =========================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## A class to help store and access PCM data and its metadata.         ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IPcmDef {                    // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IFlags;                using namespace IMemory::P;
using namespace IOal::P;               using namespace IStd::P;
using namespace Lib::OpenAL::Types;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* ------------------------------------------------------------------------- */
enum PcmFormat : size_t                // Available PCM codecs
{ /* ----------------------------------------------------------------------- */
  PFMT_WAV,                            // WAV format (IPcmFormat::CodecWAV)
  PFMT_CAF,                            // CAF format (IPcmFormat::CodecCAF)
  PFMT_OGG,                            // OGG format (IPcmFormat::CodecOGG)
  PFMT_MP3,                            // MP3 format (IPcmFormat::CodecMP3)
  /* ----------------------------------------------------------------------- */
  PFMT_MAX                             // Maximum supported PCM codecs
};/* ----------------------------------------------------------------------- */
enum PcmBitType : unsigned int         // PCM bit-depth type
{ /* ----------------------------------------------------------------------- */
  PBI_NONE                       =  0, // PCM audio is uninitialised
  PBI_BYTE                       =  8, // PCM audio is 8-bits per channel
  PBI_SHORT                      = 16, // PCM audio is 16-bits per channel
  PBI_LONG                       = 32  // PCM audio is 32-bits per channel
};/* ----------------------------------------------------------------------- */
enum PcmByteType : unsigned int        // PCM byte-depth type
{ /* ----------------------------------------------------------------------- */
  PBY_NONE                       =  0, // PCM audio is uninitialised
  PBY_BYTE                       =  1, // PCM audio is 1 byte per channel
  PBY_SHORT                      =  2, // PCM audio is 2 bytes per channel
  PBY_LONG                       =  4  // PCM audio is 4 bytes per channel
};/* ----------------------------------------------------------------------- */
enum PcmChannelType : unsigned int     // PCM channels type
{ /* ----------------------------------------------------------------------- */
  PCT_NONE                       =  0, // PCM audio is uninitialised
  PCT_MONO                       =  1, // PCM audio is mono (1ch)
  PCT_STEREO                     =  2, // PCM audio is stereo (2ch)
};/* -- Loading flags ------------------------------------------------------ */
BUILD_FLAGS(Pcm,
  /* -- Commands (Only used in 'Pcm' class) -------------------------------- */
  // No loading flags                  Force load as WAV
  PL_NONE                   {Flag(0)}, PL_FCE_WAV                {Flag(1)},
  // Force load as CAF                 Force load as OGG
  PL_FCE_CAF                {Flag(2)}, PL_FCE_OGG                {Flag(3)},
  // Force load as MP3
  PL_FCE_MP3                {Flag(4)},
  /* -- Private flags (Only used in 'PcmData' class) ----------------------- */
  // Bitmap is dynamically created
  PL_DYNAMIC                {Flag(5)},
  /* -- Mask bits ---------------------------------------------------------- */
  PL_MASK{ PL_FCE_WAV|PL_FCE_CAF|PL_FCE_OGG|PL_FCE_MP3 }
);/* -- Variables ---------------------------------------------------------- */
class PcmData :                        // Audio data structure
  /* ----------------------------------------------------------------------- */
  public PcmFlags                      // Shared with 'Pcm' class if needed
{ /* ----------------------------------------------------------------------- */
  unsigned int     uiRate;             // Samples per second (Frequency/Hz)
  PcmChannelType   pctChannels;        // Channels per sample
  PcmBitType       pbitBits;           // Bits per channel
  PcmByteType      pbytBytes;          // Bytes per channel
  ALenum           eFormat,            // Format type for openal
                   eSFormat;           // Single channel format for openal
  array<Memory,2>  aPcm;               // Pcm data (aPcmR used if stereo)
  size_t           stAlloc;            // Bytes allocated
  /* -- Public variables ------------------------------------------- */ public:
  Memory           &aPcmL,             // First Pcm channel (Mono or left)
                   &aPcmR;             // Second Pcm channel (Right stereo)
  /* ----------------------------------------------------------------------- */
  size_t GetAlloc(void) const { return stAlloc; }
  /* ----------------------------------------------------------------------- */
  void ClearData(void)
    { for(size_t stIndex = 0; stIndex < aPcm.size(); ++stIndex)
        aPcm[stIndex].MemDeInit(); }
  /* ----------------------------------------------------------------------- */
  unsigned int GetRate(void) const { return uiRate; }
  /* ----------------------------------------------------------------------- */
  void SetRate(const unsigned int uiNRate) { uiRate = uiNRate; }
  /* ----------------------------------------------------------------------- */
  PcmChannelType GetChannels(void) const { return pctChannels; }
  /* ----------------------------------------------------------------------- */
  void SetChannels(const PcmChannelType pctNChannels)
    { pctChannels = pctNChannels; }
  /* ----------------------------------------------------------------------- */
  bool SetChannelsSafe(const PcmChannelType pctNChannels)
    { SetChannels(pctNChannels);
      return pctNChannels >= PCT_MONO && pctNChannels <= PCT_STEREO; }
  /* ----------------------------------------------------------------------- */
  PcmBitType GetBits(void) const { return pbitBits; }
  /* ----------------------------------------------------------------------- */
  void SetBits(const PcmBitType pbitNBits)
    { pbitBits = pbitNBits;
      pbytBytes = static_cast<PcmByteType>(pbitBits / CHAR_BIT); }
  /* ----------------------------------------------------------------------- */
  PcmByteType GetBytes(void) const { return pbytBytes; }
  /* ----------------------------------------------------------------------- */
  void SetBytes(const PcmByteType pbytNBytes)
    { pbytBytes = pbytNBytes;
      pbitBits = static_cast<PcmBitType>(pbytBytes * CHAR_BIT); }
  /* ----------------------------------------------------------------------- */
  ALenum GetFormat(void) const { return eFormat; }
  /* ----------------------------------------------------------------------- */
  ALenum GetSFormat(void) const { return eSFormat; }
  /* ----------------------------------------------------------------------- */
  bool ParseOALFormat(void)
    { return Oal::GetOALType(GetChannels(), GetBits(), eFormat, eSFormat); }
  /* ----------------------------------------------------------------------- */
#define FH(n, f) \
  bool Is ## n(void) const { return FlagIsSet(f); } \
  bool IsNot ## n(void) const { return !Is ## n(); } \
  void Set ## n(bool bState=true) { FlagSetOrClear(f, bState); } \
  void Clear ## n(void) { Set ## n(false); }
  /* ----------------------------------------------------------------------- */
  FH(Dynamic, PL_DYNAMIC)              // Is/Set/ClearDynamic
  /* ----------------------------------------------------------------------- */
#undef FH                              // Done with this macro
  /* -- Set allocated data size -------------------------------------------- */
  void SetAlloc(const size_t stNAlloc) { stAlloc = stNAlloc; }
  /* ----------------------------------------------------------------------- */
  void ResetAllData(void)
  { // Reset all data
    SetRate(0);
    SetChannels(PCT_NONE);
    SetBits(PBI_NONE);
    eFormat = eSFormat = AL_NONE;
    // Deinit all channel memory
    ClearData();
  }
  /* ----------------------------------------------------------------------- */
  void PcmDataSwap(PcmData &pcmRef)
  { // Merge flags, don't swap
    FlagSet(pcmRef.FlagGet());
    // Swap data members
    swap(uiRate, pcmRef.uiRate);
    swap(pctChannels, pcmRef.pctChannels);
    swap(pbitBits, pcmRef.pbitBits);
    swap(pbytBytes, pcmRef.pbytBytes);
    swap(eFormat, pcmRef.eFormat);
    swap(eSFormat, pcmRef.eSFormat);
    swap(stAlloc, pcmRef.stAlloc);
    // Swap all channeld ata
    for(size_t stIndex = 0; stIndex < aPcm.size(); ++stIndex)
      aPcm[stIndex].MemSwap(pcmRef.aPcm[stIndex]);
  }
  /* -- Default constructor ------------------------------------------------ */
  explicit PcmData(const PcmFlagsConst pfcPurpose = PL_NONE) :
    /* -- Initialisers ----------------------------------------------------- */
    PcmFlags{ pfcPurpose },            // Flags not initialised
    uiRate(0),                         // Rate not initialised
    pctChannels(PCT_NONE),             // Channels not initialised
    pbitBits(PBI_NONE),                // Bits per channel not initialised
    pbytBytes(PBY_NONE),               // Bytes per channel not initialised
    eFormat(AL_NONE),                  // Format not initialised
    eSFormat(AL_NONE),                 // Single channel format not initialised
    stAlloc(0),                        // No memory allocated
    aPcmL(aPcm.front()),               // Alias of first pcm channel
    aPcmR(aPcm.back())                 // Alias of second pcm channel
    /* -- No code ---------------------------------------------------------- */
    { }
};/* ----------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
