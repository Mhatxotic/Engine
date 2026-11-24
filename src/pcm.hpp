/* == PCM.HPP ============================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## This module defines a class that can load sound files and then      ## **
** ## can optionally be sent to an OpenAL 'Sample' object for playback.   ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IPcm {                       // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IAsset::P;             using namespace IASync::P;
using namespace ICollector::P;         using namespace ICommon::P;
using namespace IError::P;             using namespace IEvtMain::P;
using namespace IFileMap::P;           using namespace IGCHelper::P;
using namespace ILog::P;               using namespace ILuaIdent::P;
using namespace ILuaLib::P;            using namespace ILuaUtil::P;
using namespace IMemory::P;            using namespace IName::P;
using namespace IPcmData::P;           using namespace IPcmDef::P;
using namespace IPcmLib::P;            using namespace ISerial::P;
using namespace IStd::P;               using namespace IString::P;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* == Pcm collector and member class ======================================= */
CTOR_BEGIN_ASYNC_DUO(Pcms, Pcm, CLHelperUnsafe, ICHelperUnsafe),
  /* -- Base classes ------------------------------------------------------- */
  public AsyncLoaderPcm,               // For loading Pcm's off main-thread
  public PcmData                       // Pcm data
{ /* -- Split a stereo waveform into two seperate channels ----------------- */
  size_t PcmSplit()
  { // If pcm data only is single channel, we don't need to split channels.
    if(PcmDataGetChannels() == 1) return PcmDataGetLeftSize();
    // If the right channel was already filled then we don't need to do it
    if(PcmDataGetRight().MemIsEmpty())
    { // Move pcm file data to a temporary array
      const Memory mTemp{ StdMove(PcmDataGetLeft()) };
      // Initialise buffers, half the size since we're only splitting
      PcmDataGetLeft().MemInitBlank(mTemp.MemSize() / 2);
      PcmDataGetRight().MemInitBlank(PcmDataGetLeftSize());
      // Iterate through the samples
      for(size_t stIndex = 0,
                 stSubIndex = 0,
                 stBytes = PcmDataGetBytes(),
                 stStep = PcmDataGetChannels() * stBytes;
                 stIndex < mTemp.MemSize();
                 stIndex += stStep,
                 stSubIndex += stBytes)
      { // De-interleave into seperate channels
        PcmDataGetLeft().MemWrite(stSubIndex,
          mTemp.MemRead(stIndex, stBytes), stBytes);
        PcmDataGetRight().MemWrite(stSubIndex,
          mTemp.MemRead(stIndex + stBytes, stBytes), stBytes);
      }
    } // Return both sizes
    return PcmDataGetLeftSize() + PcmDataGetRightSize();
  }
  /* -- Split a stereo waveform and set allocation size -------------------- */
  void PcmSplitAndSetAlloc() { PcmDataSetAlloc(PcmSplit()); }
  /* -- Convert signed to unsigned ----------------------------------------- */
  template<typename IntType, typename UIntType = StdMakeUnsigned<IntType>>
    requires StdIsSigned<IntType>
  void PcmSignedToUnsigned()
  { // Get half of the specified integer type and do the transformation
    constexpr const UIntType utHalf =
      UIntType(1) << (StdLimits<UIntType>::digits - 1);
    StdTransform(par_unseq, PcmDataGetLeft().MemPtr<IntType>(),
      PcmDataGetLeft().MemPtrEnd<IntType>(),
      PcmDataGetLeft().MemPtr<UIntType>(),
      [](const auto aSample)->UIntType {
        return static_cast<UIntType>(aSample) + utHalf; });
  }
  /* -- Convert to little-endian ------------------------------------------- */
  bool PcmConvertLittleEndian()
  { // Return if already little endian
    if(PcmDataIsNotBigEndian()) return false;
    // Get byte depth of waveform
    switch(PcmDataGetBits())
    { // Only 8, 16 or 32-bits per channel supported
      case PBI_BYTE  : PcmDataGetLeft().MemByteSwap8(); break;
      case PBI_SHORT : PcmDataGetLeft().MemByteSwap16(); break;
      case PBI_LONG  : PcmDataGetLeft().MemByteSwap32(); break;
      case PBI_NONE  : default: return false;
    } // Big endian cleared and return success
    PcmDataClearBigEndian();
    return true;
  }
  /* -- Convert to unsigned ------------------------------------------------ */
  bool PcmConvertUnsigned()
  { // Ignore if already unsigned
    if(PcmDataIsNotSigned()) return false;
    // Get byte depth of waveform
    switch(PcmDataGetBits())
    { // Only 8, 16 or 32-bits per channel supported
      case PBI_BYTE  : PcmSignedToUnsigned<int8_t>(); break;
      case PBI_SHORT : PcmSignedToUnsigned<int16_t>(); break;
      case PBI_LONG  : PcmSignedToUnsigned<int32_t>(); break;
      case PBI_NONE  : default: return false;
    } // Now unsigned so clear it and return success
    PcmDataClearSigned();
    return true;
  }
  /* -- Apply filters ------------------------------------------------------ */
  void PcmApplyFilters()
  { // Record current parameters
    const size_t stOld = PcmDataGetAlloc();
    // Convert to SPU copmatible texture?
    if(PcmDataIsConvertSPUCompat())
    { // If it is not already converted by the file filter?
      if(PcmDataIsNotActiveSPUCompat())
      { // Log that we're running this function
        cLog->LogDebugExSafe("Pcm '$' safe data requested.", NameGet());
        // Both functions have to run and if one of them did something?
        if(static_cast<unsigned>(PcmConvertLittleEndian()) |
           static_cast<unsigned>(PcmConvertUnsigned()))
        { // Log the result and set that we made a change
          cLog->LogDebugExSafe("Pcm '$' safe data made changes.", NameGet());
          PcmDataSetActiveSPUCompat();
        } // No changes made
        else cLog->LogDebugExSafe("Pcm '$' safe data skipped.", NameGet());
      } // Conversion handled by codec
      else cLog->LogDebugExSafe("Pcm '$' safe data by codec!", NameGet());
    } // Convert to little-endian waveform?
    if(PcmDataIsConvertLittleEndian())
    { // Little-endian not set by codec?
      if(PcmDataIsNotActiveLittleEndian())
      { // Log the successful result
        cLog->LogDebugExSafe("Pcm '$' data to little endian request...",
          NameGet());
        // Do the conversion and if successful?
        if(PcmConvertLittleEndian())
        { // Write success to log and set requested op as active
          cLog->LogDebugExSafe("Pcm '$' data now little-endian.", NameGet());
          PcmDataSetActiveLittleEndian();
        } // Skipped conversion
        else cLog->LogDebugExSafe(
          "Pcm '$' skipped conversion to little-endian.", NameGet());
      } // Already set by codec
      else cLog->LogDebugExSafe("Pcm '$' already little-endian by codec.",
        NameGet());
    } // Convert to unsigned waveform?
    if(PcmDataIsConvertUnsigned())
    { // Unsigned not set by codec?
      if(PcmDataIsNotActiveUnsigned())
      { // Log the successful result
        cLog->LogDebugExSafe("Pcm '$' data to unsigned request...",
          NameGet());
        // Do the conversion and if successful?
        if(PcmConvertUnsigned())
        { // Write success to log and set requested op as active
          cLog->LogDebugExSafe("Pcm '$' data now unsigned.", NameGet());
          PcmDataSetActiveUnsigned();
        } // Skipped conversion
        else cLog->LogDebugExSafe(
          "Pcm '$' skipped conversion to unsigned.", NameGet());
      } // Already set by codec
      else cLog->LogDebugExSafe("Pcm '$' already unsigned.", NameGet());
    } // Report status if we acticated anything
    if(PcmDataIsActiveSPUCompat())
      cLog->LogDebugExSafe("Pcm '$' filtering completed...$$$$",
        NameGet(),
        stOld != PcmDataGetAlloc() ?
          StrFormat("\n- Memory usage: $ -> $ bytes.",
            stOld, PcmDataGetAlloc()) : cCommon->CommonBlankStr(),
        PcmDataIsActiveSPUCompat() ? "\n- Data made OpenAL compatible." :
          cCommon->CommonBlankStr(),
        PcmDataIsActiveLittleEndian() ? "\n- Data made little-endian." :
          cCommon->CommonBlankStr(),
        PcmDataIsActiveUnsigned() ? "\n- Data made unsigned." :
          cCommon->CommonBlankStr());
  }
  /* -- Load sample from memory ------------------------------------ */ public:
  bool AsyncReady(FileMap &fmData)
  { // Force load a certain type of audio (for speed?) but in Async mode,
    // force detection doesn't really matter as much, but overall, still
    // needed if speed is absolutely neccesary.
    if     (FlagIsSet(PL_FCE_WAV)) PcmLibLoadFile(PFMT_WAV, fmData, *this);
    else if(FlagIsSet(PL_FCE_CAF)) PcmLibLoadFile(PFMT_CAF, fmData, *this);
    else if(FlagIsSet(PL_FCE_OGG)) PcmLibLoadFile(PFMT_OGG, fmData, *this);
    // Auto detection of pcm audio
    else PcmLibLoadFile(fmData, *this);
    // Apply filters if image has no special circumstances
    PcmApplyFilters();
    // Split into two channels if audio in stereo
    PcmSplitAndSetAlloc();
    // The 'Memory' class data was moved so we can free the memory
    return true;
  }
  /* -- Reload data -------------------------------------------------------- */
  void PcmReloadData()
  { // Load the file from disk or archive
    FileMap fmData{ AssetExtract(NameGet()) };
    // Reset memory usage to zero
    PcmDataSetAlloc(0);
    // Run codecs and filters on it
    AsyncReady(fmData);
  }
  /* -- Init from array ---------------------------------------------------- */
  template<class StrType>
    requires StdIsStrOrCStr<StrType>
  void PcmInitArray(StrType &&strName, Memory &mSrc,
    const PcmFlagsConst &pfcFlags)
  { // Is dynamic because it was not loaded from disk
    PcmDataSetDynamic();
    // Set the loading flags
    FlagSet(pfcFlags);
    // Load the array normally
    SyncInitArray(StdForward<StrType>(strName), "pcmarray", mSrc);
  }
  /* -- Load pcm from memory asynchronously -------------------------------- */
  template<class StrType>
    requires StdIsStrOrCStr<StrType>
  void PcmInitAsyncArray(lua_State*const lS, StrType &&strFile,
    Asset &aCref, const PcmFlagsConst &pfcFlags)
  { // Is dynamic because it was not loaded from disk
    PcmDataSetDynamic();
    // Set user loading flags
    FlagSet(pfcFlags);
    // Load sample from memory asynchronously
    AsyncInitArray(lS, StdForward<StrType>(strFile), "pcmarray", aCref);
  }
  /* -- Load pcm from file asynchronously ---------------------------------- */
  template<class StrType>
    requires StdIsStrOrCStr<StrType>
  void PcmInitAsyncFile(lua_State*const lS, StrType &&strFile,
    const PcmFlagsConst &pfcFlags)
  { // Set user loading flags
    FlagSet(pfcFlags);
    // Load sample from file asynchronously
    AsyncInitFile(lS, StdForward<StrType>(strFile), "pcmfile");
  }
  /* -- Init from file ----------------------------------------------------- */
  template<class StrType>
    requires StdIsStrOrCStr<StrType>
  void PcmInitFile(StrType &&strFile, const PcmFlagsConst &pfcFlags)
  { // Set the loading flags
    FlagSet(pfcFlags);
    // Load the file normally
    SyncInitFileSafe(StdForward<StrType>(strFile), "pcmfile");
  }
  /* -- Load audio file from raw memory ------------------------------------ */
  template<class StrType>
    requires StdIsStrOrCStr<StrType>
  void PcmInitRaw(StrType &&strName, Memory &mSrc, const unsigned uNRate,
    const PcmChannelType pctNChannels, const PcmBitType pbtNBits)
  { // Calculate actual memory size required for raw data
    const size_t stExpected = uNRate * pctNChannels * pbtNBits;
    if(mSrc.MemSize() != stExpected)
      XC("Expected size versus actual size mismatch!",
        "Name",     strName,      "Rate",   uNRate,
        "Channels", pctNChannels, "Bits",   pbtNBits,
        "Expected", stExpected,   "Actual", mSrc.MemSize());
    // Set members
    NameSet(StdForward<StrType>(strName));
    PcmDataSetDynamic();
    PcmDataGetLeft().MemSwap(mSrc);
    PcmDataSetRate(uNRate);
    PcmDataSetChannels(pctNChannels);
    PcmDataSetBits(pbtNBits);
    // Split audio into two channels if audio in stereo
    PcmSplitAndSetAlloc();
    // Load succeeded so register the block
    CollectorRegister();
  }
  /* ----------------------------------------------------------------------- */
  void PcmSwap(Pcm &pcmRef)
  { // Swap datas
    LockSwap(pcmRef);
    CollectorSwapRegistration(pcmRef);
    PcmDataSwap(pcmRef);
  }
  /* -- Default constructor ------------------------------------------------ */
  Pcm() :
    /* -- Initialisers ----------------------------------------------------- */
    ICHelperPcm{ cPcms },              // Initially unregistered
    SerialSlave{ ctParent.Serial() },  // Initialise identification number
    AsyncLoaderPcm{ this, EMC_MP_PCM } // Setup async loader with this class
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- Constructor to MOVE from other object ------------------------------ */
  Pcm(
    /* -- Parameters ------------------------------------------------------- */
    Pcm &&pcmOther                     // Other Pcm reference to swap with
    ): /* -- Initialisation of members ------------------------------------- */
    Pcm()                              // Default initialisation of members
    /* -- Swap members with other class ------------------------------------ */
    { PcmSwap(pcmOther); }
  /* -- Destructor (override) ---------------------------------------------- */
  DTORHELPER(~Pcm, AsyncCancel())
};/* -- End-of-collector --------------------------------------------------- */
CTOR_END_ASYNC_NOFUNCS(Pcms, Pcm, PCM, PCM) // Finish collector class
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
