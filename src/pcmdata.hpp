/* == PCMDATA.HPP ========================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## A class to help store metadata for a loaded waveform.               ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IPcmData {                   // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IMemory::P;            using namespace IPcmDef::P;
using namespace IStd::P;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* -- Variables ------------------------------------------------------------ */
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
