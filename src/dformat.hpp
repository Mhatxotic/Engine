/* == DFORMAT.HPP ========================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## This file defines a base class which is used with both PcmLib and   ## **
** ## ImageLib to define common routines for loading and saving different ## **
** ## kinds of data formats.                                              ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IDataFormat {                // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IError::P;             using namespace IFileMap::P;
using namespace IFlags;                using namespace IFStream::P;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* -- Functions available flags -------------------------------------------- */
BUILD_FLAGS(DataFormat, DF_LOAD{Flag[1]}, DF_SAVE{Flag[2]});
/* -- Image libraries format object class ---------------------------------- */
template<class DataType,               // Data storage type
         typename FormatType,          // Data type id
         typename SlotType=bool>       // Slot type (save only)
class DataFormat
{ /* -- Protected typedefs -------------------------------------- */ protected:
  typedef bool (CbFuncLoadT)(FileMap&, DataType&);
  typedef bool (CbFuncSaveT)(const FStream&, const DataType&, const SlotType&);
  /* -- Public typedefs -------------------------------------------- */ public:
  typedef function<CbFuncLoadT> CbFuncLoad;
  typedef function<CbFuncSaveT> CbFuncSave;
  /* -- Private variables ----------------------------------------- */ private:
  const string_view strvName,          // Name of plugin
                    strvExt;           // Default extension of plugin type
  const DataFormatFlagsConst dffcCaps; // Capabilities
  const CbFuncLoad  cflFunc;           // Loader function
  const CbFuncSave  cfsFunc;           // Saver function
  const FormatType  ftId;              // Data format id
  /* -- Check id number ---------------------------------------------------- */
  FormatType CheckId(const FormatType ftNId, const size_t stSize)
  { // The id should match the collector count so return value if it is
    const size_t stExpect = stSize - 1;
    if(ftNId == stExpect) return ftNId;
    XC("Internal error: Data format id mismatch!",
       "Id",     ftNId,    "Expect",    stExpect,
       "Filter", strvName, "Extension", strvExt);
  }
  /* -- Unsupported callbacks----------------------------------------------- */
  static bool NoLoader(FileMap&, DataType&) { return false; }
  static bool NoSaver(const FStream&, const DataType&, const SlotType&)
    { return false; }
  /* -- Get members ------------------------------------------------ */ public:
  const CbFuncLoad &GetLoader(void) const { return cflFunc; }
  const CbFuncSave &GetSaver(void) const { return cfsFunc; }
  const string_view &GetName(void) const { return strvName; }
  const string_view &GetExt(void) const { return strvExt; }
  bool HaveLoader(void) const { return dffcCaps.FlagIsSet(DF_LOAD); }
  bool HaveSaver(void) const { return dffcCaps.FlagIsSet(DF_SAVE); }
  /* -- Constructor with loader function only ------------------- */ protected:
  explicit DataFormat(
    /* -- Required arguments ----------------------------------------------- */
    const FormatType ftNId,            // The IFMT_* id
    const string_view &strvNName,      // The name of the codec
    const string_view &strvNExt,       // The default extension for the codec
    const CbFuncLoad &cflNFunc,        // Function to call when loading
    const size_t stSize                // Size of collector to id
    ): /* -- Initialisers -------------------------------------------------- */
    strvName{ strvNName },             // Set name for filter
    strvExt{ strvNExt },               // Set extension for filter
    dffcCaps{ DF_LOAD },               // Set load only capability
    cflFunc{ cflNFunc },               // Set loader function
    cfsFunc{ NoSaver },                // Set no saver function
    ftId(CheckId(ftNId, stSize))       // Set unique id for this filter
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Constructor with saver function only ------------------------------- */
  explicit DataFormat(
    /* -- Required arguments ----------------------------------------------- */
    const FormatType ftNId,            // The IFMT_* id
    const string_view &strvNName,      // The name of the codec
    const string_view &strvNExt,       // The default extension for the codec
    const CbFuncSave &cfsNFunc,        // Function to call when saving
    const size_t stSize                // Size of collector to id
    ): /* -- Initialisers -------------------------------------------------- */
    strvName{ strvNName },             // Set name for filter
    strvExt{ strvNExt },               // Set extension for filter
    dffcCaps{ DF_SAVE },               // Set save only capability
    cflFunc{ NoLoader },               // Set no loader function
    cfsFunc{ cfsNFunc },               // Set saver function
    ftId(CheckId(ftNId, stSize))       // Set unique id for this filter
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Constructor with both loader and saver functions ------------------- */
  explicit DataFormat(
    /* -- Required arguments ----------------------------------------------- */
    const FormatType ftNId,            // The IFMT_* id
    const string_view &strvNName,      // The name of the codec
    const string_view &strvNExt,       // The default extension for the codec
    const CbFuncLoad &cflNFunc,        // Function to call when loading
    const CbFuncSave &cfsNFunc,        // Function to call when saving
    const size_t stSize                // Size of collector to id
    ): /* -- Initialisers -------------------------------------------------- */
    strvName{ strvNName },             // Set name for filter
    strvExt{ strvNExt },               // Set extension for filter
    dffcCaps{ DF_LOAD|DF_SAVE },       // Set load and save capabilities
    cflFunc{ cflNFunc },               // Set loader function
    cfsFunc{ cfsNFunc },               // Set saver function
    ftId(CheckId(ftNId, stSize))       // Set unique id for this filter
    /* -- No code ---------------------------------------------------------- */
    { }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(DataFormat)          // Suppress default functions for safety
};/* ----------------------------------------------------------------------- */
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
