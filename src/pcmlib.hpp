/* == PCMLIB.HPP =========================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## This module manages all the different audio codecs we support in    ## **
** ## the engine.                                                         ## **
** ######################################################################### **
** ## To add a new codec is straight forward. You need to modify the      ## **
** ## following files in order to achieve this...                         ## **
** ## PCMDEF.HPP    = Add a 'PFMT_ID' and 'PL_FCE_ID' enum to the         ## **
** ##                 'PcmFormat' and the 'PcmFlags' table respectively.  ## **
** ## PCM.HPP       = Add a manual override condition to 'AsyncReady()'.  ## **
** ## LLPCM.HPP     = Add your 'PL_FCE_ID' value at 'LLRSKTBEGIN(Flags)'  ## **
** ##                 to expose the flag to LUA at sandbox startup.       ## **
** ## CONLIB.HPP    = Add the 'PL_FCE_ID' token to the 'pcms' console     ## **
** ##                 command output.                                     ## **
** ## CREDIT.HPP    = (If using an external library) Add a new 'CL_ID'    ## **
** ##                 enum to the 'CreditEnums' table and add the         ## **
** ##                 appropriate information to credit the library.      ## **
** ## SETUP.HPP     = (If using an external library) Add appropriate      ## **
** ##                 includes into their own C++ namespace.              ## **
** ## PFMT<EXT>.HPP = Add your 'Codec<EXT>' class into a new file using   ## **
** ##                 using the same layout as the other 'PFMT<EXT>.HPP'  ## **
** ##                 files deriving your class with 'PcmLib' and sending ## **
** ##                 the proper information to the 'PcmLib' constructor. ## **
** ## ENGINE.CPP    = Add your 'PFMT<EXT>.HPP' include here with the      ## **
** ##                 others.                                             ## **
** ## CORE.CPP      = Derive your 'private Codec<EXT>' class into the     ## **
** ##                 'Core' class to make sure it is created at startup. ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IPcmLib {                    // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace ICollector::P;         using namespace IDataFormat::P;
using namespace IError::P;             using namespace IFileMap::P;
using namespace IIdent::P;             using namespace ILog::P;
using namespace ILuaIdent::P;          using namespace ILuaLib::P;
using namespace IPcmDef::P;            using namespace IStd::P;
using namespace IString::P;            using namespace ISysUtil::P;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* -- Pcm formats collector class as a vector for direct access ------------ */
CTOR_BEGIN_CUSTCTR(PcmLibs, PcmLib, StdVector, CLHelperUnsafe)
/* -- Pcm format object class ---------------------------------------------- */
CTOR_MEM_BEGIN_CSLAVE(PcmLibs, PcmLib, ICHelperUnsafe),
  /* -- Base classes ------------------------------------------------------- */
  public DataFormat<PcmData, PcmFormat>
{ /* -- Constructor with loader function only ---------------------- */ public:
  explicit PcmLib(
    /* -- Required arguments ----------------------------------------------- */
    const PcmFormat pfNId,             // The PFMT_* id
    const StdStringView &strvNName,    // The name of the codec
    const StdStringView &strvNExt,     // The default extension for the codec
    const CbFuncDecoder &cfdNFunc      // Function to call when loading
    ): /* -- Initialisers -------------------------------------------------- */
    ICHelperPcmLib{ cPcmLibs, this },  // Register filter in filter
    IdentCSlave{ cParent->CtrNext() }, // Initialise identification number
    DataFormat{ pfNId, strvNName, strvNExt, cfdNFunc, cParent->size() }
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- Constructor with saver function only ------------------------------- */
  explicit PcmLib(
    /* -- Required arguments ----------------------------------------------- */
    const PcmFormat pfNId,             // The PFMT_* id
    const StdStringView &strvNName,    // The name of the codec
    const StdStringView &strvNExt,     // The default extension for the codec
    const CbFuncEncoder &cfeNFunc      // Function to call when saving
    ): /* -- Initialisers -------------------------------------------------- */
    ICHelperPcmLib{ cPcmLibs, this },  // Register filter in filter
    IdentCSlave{ cParent->CtrNext() }, // Initialise identification number
    DataFormat{ pfNId, strvNName, strvNExt, cfeNFunc, cParent->size() }
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- Constructor with both loader and saver functions ------------------- */
  explicit PcmLib(
    /* -- Required arguments ----------------------------------------------- */
    const PcmFormat pfNId,             // The PFMT_* id
    const StdStringView &strvNName,    // The name of the codec
    const StdStringView &strvNExt,     // The default extension for the codec
    const CbFuncDecoder &cfdNFunc,     // Function to call when loading
    const CbFuncEncoder &cfeNFunc      // Function to call when saving
    ): /* -- Initialisers -------------------------------------------------- */
    ICHelperPcmLib{ cPcmLibs, this },  // Register filter in filter
    IdentCSlave{ cParent->CtrNext() }, // Initialise identification number
    DataFormat{ pfNId, strvNName, strvNExt, cfdNFunc, cfeNFunc,
      cParent->size() }
    /* -- No code ---------------------------------------------------------- */
    {}
};/* -- End of objects collector (reserve and set limit for formats) ------- */
CTOR_END(PcmLibs, PcmLib, PCMLIB,
  reserve(PFMT_MAX); CollectorSetLimit(PFMT_MAX),)
/* -- Load audio using a specific type ------------------------------------- */
static void PcmLoadFile(const PcmFormat pfId, FileMap &fmData, PcmData &pdData)
{ // Get plugin class. We already checked if the index was valid
  const PcmLib &plRef = *cPcmLibs->at(pfId);
  // Capture exceptions
  try
  { // Load the image, log and return if loaded successfully
    if(plRef.GetDecoder()(fmData, pdData))
      return cLog->LogInfoExSafe(
        "Pcm loaded '$' directly as $<$>! ($;$;$;$$;$;$;$$)",
        fmData.IdentGet(), plRef.GetExt(), pfId, pdData.GetRate(),
        pdData.GetChannels(), pdData.GetBits(), StdIOSHex, pdData.GetFormat(),
        pdData.GetSFormat(), StrFromBoolTF(pdData.IsDynamic()), StdIOSHex,
        pdData.GetAlloc());
    // Could not detect format so throw error
    throw StdRunTimeError{ "Unable to load sound!" };
  } // Error occured. Error used as title
  catch(const StdException &eReason)
  { // Throw an error with the specified reason
    XC(eReason,
      "Identifier", fmData.IdentGet(),    "Size",     fmData.MemSize(),
      "Position",   fmData.FileMapTell(), "FormatId", pfId,
      "Plugin",     plRef.GetName());
  }
}
/* -- Load a bitmap and automatically detect type -------------------------- */
static void PcmLoadFile(FileMap &fmData, PcmData &pdData)
{ // For each plugin registered
  for(const PcmLib*const plPtr : *cPcmLibs)
  { // Get reference to plugin
    const PcmLib &plRef = *plPtr;
    // Capture exceptions
    try
    { // Load the bitmap, log and return if we loaded successfully
      if(plRef.GetDecoder()(fmData, pdData))
        return cLog->LogInfoExSafe("Pcm loaded '$' as $! ($;$;$;$$;$;$;$$)",
          fmData.IdentGet(), plRef.GetExt(), pdData.GetRate(),
          pdData.GetChannels(), pdData.GetBits(), StdIOSHex,
          pdData.GetFormat(), pdData.GetSFormat(),
          StrFromBoolTF(pdData.IsDynamic()), StdIOSDec, pdData.GetAlloc());
    } // Error occured. Error used as title
    catch(const StdException &eReason)
    { // Throw an error with the specified reason
      XC(eReason,
        "Identifier", fmData.IdentGet(),    "Size",   fmData.MemSize(),
        "Position",   fmData.FileMapTell(), "Plugin", plRef.GetName());
    } // Rewind stream position and reset all pcm data read to load again
    fmData.FileMapRewind();
    pdData.ResetAllData();
  } // Could not detect so throw error
  XC("Unable to determine sound format!", "Identifier", fmData.IdentGet());
}
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
