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
using namespace IFlags::P;             using namespace IMemory::P;
using namespace IOal::P;               using namespace IStd::P;
using namespace Lib::OpenAL::Types;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* -- Public typedefs ------------------------------------------------------ */
enum PcmFormat : size_t                // Available PCM codecs
{ /* ----------------------------------------------------------------------- */
  PFMT_WAV,                            // [0] WAV (IPcmFormat::CodecWAV)
  PFMT_CAF,                            // [1] CAF (IPcmFormat::CodecCAF)
  PFMT_OGG,                            // [2] OGG (IPcmFormat::CodecOGG)
  /* ----------------------------------------------------------------------- */
  PFMT_MAX                             // [3] Maximum supported PCM codecs
};/* ----------------------------------------------------------------------- */
enum PcmBitType : unsigned             // PCM bit-depth type
{ /* ----------------------------------------------------------------------- */
  PBI_NONE                       =  0, // PCM audio is uninitialised
  PBI_BYTE                       =  8, // PCM audio is 8-bits per channel
  PBI_SHORT                      = 16, // PCM audio is 16-bits per channel
  PBI_LONG                       = 32  // PCM audio is 32-bits per channel
};/* ----------------------------------------------------------------------- */
enum PcmByteType : unsigned            // PCM byte-depth type
{ /* ----------------------------------------------------------------------- */
  PBY_NONE                       =  0, // PCM audio is uninitialised
  PBY_BYTE                       =  1, // PCM audio is 1 byte per channel
  PBY_SHORT                      =  2, // PCM audio is 2 bytes per channel
  PBY_LONG                       =  4  // PCM audio is 4 bytes per channel
};/* ----------------------------------------------------------------------- */
enum PcmChannelType : unsigned         // PCM channels type
{ /* ----------------------------------------------------------------------- */
  PCT_NONE                       =  0, // PCM audio is uninitialised
  PCT_MONO                       =  1, // PCM audio is mono (1ch)
  PCT_STEREO                     =  2, // PCM audio is stereo (2ch)
};/* ----------------------------------------------------------------------- */
BUILD_FLAGS(Pcm,                       // PCM loading flags
  /* ----------------------------------------------------------------------- */
  PL_NONE                   {Flag(0)}, // No loading flags?
  /* -- Force types -------------------------------------------------------- */
  PL_FCE_WAV                {Flag(1)}, // Force load as WAV format?
  PL_FCE_CAF                {Flag(2)}, // Force load as CAF format?
  PL_FCE_OGG                {Flag(3)}, // Force load as OGG format?
  /* -- Post processing requests ------------------------------------------- */
  PL_TOSPU                  {Flag(4)}, // Convert to nearest OpenAL compatible
  PL_TOBE                   {Flag(5)}, // Convert to big-endian format
  PL_TOLE                   {Flag(6)}, // Convert to little-endian format
  PL_TOSIGNED               {Flag(7)}, // Convert to signed format
  PL_TOUNSIGNED             {Flag(8)}, // Convert to unsigned format
  /* -- Private flags (Only used in 'Pcm' class) --------------------------- */
  PL_BE                     {Flag(9)}, // Is big-endian encoded (or LE)?
  PL_DYNAMIC               {Flag(10)}, // Waveform is dynamically created?
  PL_SIGNED                {Flag(11)}, // Is signed/unsigned waveform?
  /* -- Active post-processing flags --------------------------------------- */
  PA_TOSPU                 {Flag(12)}, // Converted to nearest OpenAL compat.
  PA_TOBE                  {Flag(13)}, // Converted to big-endian format
  PA_TOLE                  {Flag(14)}, // Converted to little-endian format
  PA_TOSIGNED              {Flag(15)}, // Converted to signed format
  PA_TOUNSIGNED            {Flag(16)}, // Converted to unsigned format
  /* -- Purposes ----------------------------------------------------------- */
  PP_SAMPLE                {Flag(64)}, // Purpose is to be a Sample
  /* -- Mask bits ---------------------------------------------------------- */
  PL_MASK{ PL_FCE_WAV|PL_FCE_CAF|PL_FCE_OGG|PL_TOSPU|PL_TOBE|PL_TOLE|
           PL_TOSIGNED|PL_TOUNSIGNED }
);/* -- Variables ---------------------------------------------------------- */
class PcmData :                        // Audio data structure
  /* ----------------------------------------------------------------------- */
  public PcmFlags                      // Shared with 'Pcm' class if needed
{ /* ----------------------------------------------------------------------- */
  unsigned         uRate;              // Samples per second (Frequency/Hz)
  PcmChannelType   pctChannels;        // Channels per sample
  PcmBitType       pbitBits;           // Bits per channel
  PcmByteType      pbytBytes;          // Bytes per channel
  StdArray<Memory,2> aPcm;             // Pcm data (aPcmR used if stereo)
  size_t           stAlloc;            // Bytes allocated
  /* -- Public variables ------------------------------------------- */ public:
  Memory           &aPcmL,             // First Pcm channel (Mono or left)
                   &aPcmR;             // Second Pcm channel (Right stereo)
  /* ----------------------------------------------------------------------- */
  size_t GetAlloc() const { return stAlloc; }
  /* -- De-init channel data memory ---------------------------------------- */
  void ClearData()
    { StdForEach(par_unseq, aPcm.begin(), aPcm.end(),
        [](Memory &mC){ mC.MemDeInit(); }); }
  /* ----------------------------------------------------------------------- */
  unsigned GetRate() const { return uRate; }
  /* ----------------------------------------------------------------------- */
  void SetRate(const unsigned uNRate) { uRate = uNRate; }
  /* ----------------------------------------------------------------------- */
  PcmChannelType GetChannels() const { return pctChannels; }
  /* ----------------------------------------------------------------------- */
  void SetChannels(const PcmChannelType pctNChannels)
    { pctChannels = pctNChannels; }
  /* ----------------------------------------------------------------------- */
  bool SetChannelsSafe(const PcmChannelType pctNChannels)
    { SetChannels(pctNChannels);
      return pctNChannels >= PCT_MONO && pctNChannels <= PCT_STEREO; }
  /* ----------------------------------------------------------------------- */
  PcmBitType GetBits() const { return pbitBits; }
  /* ----------------------------------------------------------------------- */
  void SetBits(const PcmBitType pbitNBits)
    { pbitBits = pbitNBits;
      pbytBytes = static_cast<PcmByteType>(pbitBits / CHAR_BIT); }
  /* ----------------------------------------------------------------------- */
  PcmByteType GetBytes() const { return pbytBytes; }
  /* ----------------------------------------------------------------------- */
  void SetBytes(const PcmByteType pbytNBytes)
    { pbytBytes = pbytNBytes;
      pbitBits = static_cast<PcmBitType>(pbytBytes * CHAR_BIT); }
  /* ----------------------------------------------------------------------- */
#define FH(n, f) \
  bool Is ## n() const { return FlagIsSet(f); } \
  bool IsNot ## n() const { return !Is ## n(); } \
  void Set ## n(bool bState=true) { FlagSetOrClear(f, bState); } \
  void Clear ## n() { Set ## n(false); }
  /* ----------------------------------------------------------------------- */
  FH(BigEndian,           PL_BE)       // Is big-endian encoded?
  FH(Dynamic,             PL_DYNAMIC)  // Is/Set/ClearDynamic
  FH(Signed,              PL_SIGNED)   // Is signed (or unsigned)?
  FH(ConvertBigEndian,    PL_TOBE)     // Convert to big-endian
  FH(ConvertLittleEndian, PL_TOLE)     // Convert to little-endian
  FH(ConvertSigned,       PL_TOSIGNED) // Convert to little-endian
  FH(ConvertUnsigned,     PL_TOUNSIGNED) // Convert to little-endian
  FH(ConvertSPUCompat,    PL_TOSPU)    // Convert to SPU compat requested?
  FH(ActiveSigned,        PA_TOSIGNED) // Converted to signed?
  FH(ActiveUnsigned,      PA_TOUNSIGNED) // Converted to unsigned?
  FH(ActiveBigEndian,     PA_TOBE)     // Converted to big-endian?
  FH(ActiveLittleEndian,  PA_TOLE)     // Converted to little-endian?
  FH(ActiveSPUCompat,     PA_TOSPU)    // SPU compat mode activated?
  FH(PurposeSample,       PP_SAMPLE)   // Pcm object owned by Sample?
  /* ----------------------------------------------------------------------- */
#undef FH                              // Done with this macro
  /* -- Set allocated data size -------------------------------------------- */
  void SetAlloc(const size_t stNAlloc) { stAlloc = stNAlloc; }
  /* ----------------------------------------------------------------------- */
  void ResetAllData()
  { // Reset all data
    SetRate(0);
    SetChannels(PCT_NONE);
    SetBits(PBI_NONE);
    // Clear data
    ClearSigned();
    ClearBigEndian();
    // De-init all channel memory
    ClearData();
  }
  /* ----------------------------------------------------------------------- */
  void PcmDataSwap(PcmData &pcmRef)
  { // Merge flags, don't swap
    FlagSet(pcmRef.FlagGet());
    // Swap data members
    StdSwap(uRate, pcmRef.uRate);
    StdSwap(pctChannels, pcmRef.pctChannels);
    StdSwap(pbitBits, pcmRef.pbitBits);
    StdSwap(pbytBytes, pcmRef.pbytBytes);
    StdSwap(stAlloc, pcmRef.stAlloc);
    aPcm.swap(pcmRef.aPcm);
  }
  /* -- Default constructor ------------------------------------------------ */
  explicit PcmData(const PcmFlagsConst pfcPurpose = PL_NONE) :
    /* -- Initialisers ----------------------------------------------------- */
    PcmFlags{ pfcPurpose },            // Flags not initialised
    uRate(0),                          // Rate not initialised
    pctChannels(PCT_NONE),             // Channels not initialised
    pbitBits(PBI_NONE),                // Bits per channel not initialised
    pbytBytes(PBY_NONE),               // Bytes per channel not initialised
    stAlloc(0),                        // No memory allocated
    aPcmL(aPcm.front()),               // Alias of first pcm channel
    aPcmR(aPcm.back())                 // Alias of second pcm channel
    /* -- No code ---------------------------------------------------------- */
    {}
};/* ----------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
