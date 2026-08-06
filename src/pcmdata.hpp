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
  /* -- Base classes ------------------------------------------------------- */
  public PcmFlags                      // Shared with 'Pcm' class if needed
{ /* -- Private typedefs --------------------------------------------------- */
  using StereoPCM = StdArray<Memory, 2>;
  /* -- Private variables -------------------------------------------------- */
  unsigned         uRate;              // Samples per second (Frequency/Hz)
  PcmChannelType   pctChannels;        // Channels per sample
  PcmBitType       pbitBits;           // Bits per channel
  PcmByteType      pbytBytes;          // Bytes per channel
  StereoPCM        spPcm;              // Pcm data (Only use [0] if mono)
  size_t           stAlloc;            // Bytes allocated
  /* -- Public variables --------------------------------------------------- */
  Memory           &mPcmL,             // 1st (Mono/Left) ref to spPcm[0]
                   &mPcmR;             // 2nd (Stereo/Right) ref to spPcm[1]
  /* --------------------------------------------------------------- */ public:
  size_t PcmDataGetAlloc() const { return stAlloc; }
  /* -- De-init channel data memory ---------------------------------------- */
  void PcmDataClearData()
    { StdForEach(par_unseq, spPcm.begin(), spPcm.end(),
        [](Memory &mRef){ mRef.MemDeInit(); }); }
  /* ----------------------------------------------------------------------- */
  unsigned PcmDataGetRate() const { return uRate; }
  /* ----------------------------------------------------------------------- */
  void PcmDataSetRate(const unsigned uNRate) { uRate = uNRate; }
  /* ----------------------------------------------------------------------- */
  PcmChannelType PcmDataGetChannels() const { return pctChannels; }
  /* ----------------------------------------------------------------------- */
  void PcmDataSetChannels(const PcmChannelType pctNChannels)
    { pctChannels = pctNChannels; }
  /* ----------------------------------------------------------------------- */
  bool PcmDataSetChannelsSafe(const PcmChannelType pctNChannels)
    { PcmDataSetChannels(pctNChannels);
      return pctNChannels >= PCT_MONO && pctNChannels <= PCT_STEREO; }
  /* ----------------------------------------------------------------------- */
  PcmBitType PcmDataGetBits() const { return pbitBits; }
  /* ----------------------------------------------------------------------- */
  void PcmDataSetBits(const PcmBitType pbitNBits)
    { pbitBits = pbitNBits;
      pbytBytes = static_cast<PcmByteType>(pbitBits / CHAR_BIT); }
  /* ----------------------------------------------------------------------- */
  PcmByteType PcmDataGetBytes() const { return pbytBytes; }
  /* ----------------------------------------------------------------------- */
  void PcmDataSetBytes(const PcmByteType pbytNBytes)
    { pbytBytes = pbytNBytes;
      pbitBits = static_cast<PcmBitType>(pbytBytes * CHAR_BIT); }
  /* ----------------------------------------------------------------------- */
  Memory &PcmDataGetLeft() { return mPcmL; }
  const Memory &PcmDataGetLeftConst() const { return mPcmL; }
  Memory &PcmDataGetRight() { return mPcmR; }
  const Memory &PcmDataGetRightConst() const { return mPcmR; }
  /* ----------------------------------------------------------------------- */
  size_t PcmDataGetLeftSize() const
    { return PcmDataGetLeftConst().MemSize(); }
  size_t PcmDataGetRightSize() const
    { return PcmDataGetRightConst().MemSize(); }
  /* ----------------------------------------------------------------------- */
  bool PcmDataNotPrepared() const
    { return PcmDataGetLeftConst().MemIsEmpty(); }
  /* ----------------------------------------------------------------------- */
  Memory &PcmDataPrepareData(const size_t stSize)
    { PcmDataGetLeft().MemResize(stSize); return PcmDataGetLeft(); }
  /* ----------------------------------------------------------------------- */
  void PcmDataPrepareData(const void*const vpData, const size_t stSize)
  { // The stereo data temporarily gets put into the left channel and if it is
    // really stereo then it will get deinterlaced into the right channel.
    PcmDataGetLeft().MemInitData(stSize, vpData);
  }
  /* ----------------------------------------------------------------------- */
#define FH(n, f) \
  bool PcmDataIs ## n() const { return FlagIsSet(f); } \
  bool PcmDataIsNot ## n() const { return !PcmDataIs ## n(); } \
  void PcmDataSet ## n(bool bState = true) { FlagSetOrClear(f, bState); } \
  void PcmDataClear ## n() { PcmDataSet ## n(false); }
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
  void PcmDataSetAlloc(const size_t stNAlloc) { stAlloc = stNAlloc; }
  /* ----------------------------------------------------------------------- */
  void PcmDataResetAllData()
  { // Reset all data
    PcmDataSetRate(0);
    PcmDataSetChannels(PCT_NONE);
    PcmDataSetBits(PBI_NONE);
    // Clear data
    PcmDataClearSigned();
    PcmDataClearBigEndian();
    // De-init all channel memory
    PcmDataClearData();
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
    spPcm.swap(pcmRef.spPcm);
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
    mPcmL(spPcm.front()),              // Alias of first pcm channel
    mPcmR(spPcm.back())                // Alias of second pcm channel
    /* -- No code ---------------------------------------------------------- */
    {}
};/* ----------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
