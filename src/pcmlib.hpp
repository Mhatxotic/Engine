/* == PCMLIB.HPP =========================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## This module manages all the different audio types we support in the ## **
** ## engine.                                                             ## **
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
CTOR_BEGIN_CUSTCTR(PcmLibs, PcmLib, vector, CLHelperUnsafe)
/* -- Pcm format object class ---------------------------------------------- */
CTOR_MEM_BEGIN_CSLAVE(PcmLibs, PcmLib, ICHelperUnsafe),
  /* -- Base classes ------------------------------------------------------- */
  public DataFormat<PcmData, PcmFormat>
{ /* -- Constructor with loader function only ---------------------- */ public:
  explicit PcmLib(
    /* -- Required arguments ----------------------------------------------- */
    const PcmFormat pfNId,             // The PFMT_* id
    const string_view &strvNName,      // The name of the codec
    const string_view &strvNExt,       // The default extension for the codec
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
    const string_view &strvNName,      // The name of the codec
    const string_view &strvNExt,       // The default extension for the codec
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
    const string_view &strvNName,      // The name of the codec
    const string_view &strvNExt,       // The default extension for the codec
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
        pdData.GetChannels(), pdData.GetBits(), hex, pdData.GetFormat(),
        pdData.GetSFormat(), StrFromBoolTF(pdData.IsDynamic()), hex,
        pdData.GetAlloc());
    // Could not detect format so throw error
    throw runtime_error{ "Unable to load sound!" };
  } // Error occured. Error used as title
  catch(const exception &eReason)
  { // Throw an error with the specified reason
    XC(eReason, "Identifier", fmData.IdentGet(),
                "Size",       fmData.MemSize(),
                "Position",   fmData.FileMapTell(),
                "FormatId",   pfId,
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
          pdData.GetChannels(), pdData.GetBits(), hex, pdData.GetFormat(),
          pdData.GetSFormat(), StrFromBoolTF(pdData.IsDynamic()), dec,
          pdData.GetAlloc());
    } // Error occured. Error used as title
    catch(const exception &eReason)
    { // Throw an error with the specified reason
      XC(eReason, "Identifier", fmData.IdentGet(),
                  "Size",       fmData.MemSize(),
                  "Position",   fmData.FileMapTell(),
                  "Plugin",     plRef.GetName());
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
