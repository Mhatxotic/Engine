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
BUILD_FLAGS(DataFormat, FR_LOAD{Flag[1]}, FR_SAVE{Flag[2]});
/* -- Image libraries format object class ---------------------------------- */
template<class DataType,               // Data storage type
         typename FormatType,          // Data type id
         typename SlotType=bool>       // Slot type (save only)
class DataFormat
{ /* -- Protected typedefs -------------------------------------- */ protected:
  typedef bool (CbFuncLoadT)(FileMap&, DataType&);
  typedef bool (CbFuncSaveT)(const FStream&, const DataType&, const SlotType&);
  /* -- Private typedefs ------------------------------------------- */ public:
  typedef function<CbFuncLoadT> CbFuncLoad;
  typedef function<CbFuncSaveT> CbFuncSave;
  /* -- Variables ------------------------------------------------- */ private:
  const string_view strvName,          // Name of plugin
                    strvExt;           // Default extension of plugin type
  const DataFormatFlagsConst dffcCaps; // Capabilities
  const CbFuncLoad  cflFunc;           // Loader callback
  const CbFuncSave  cfsFunc;           // Saver callback
  const FormatType  ftId;              // Image format id
  /* -- Check id number ---------------------------------------------------- */
  FormatType CheckId(const FormatType ftNId, const size_t stSize)
  { // The id should match the collector count
    const size_t stExpect = stSize - 1;
    if(ftNId == stExpect) return ftNId;
    // Make sure the ImageFormats match the codec construction order!
    XC("Internal error: DataFormat id mismatch!",
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
  bool HaveLoader(void) const { return dffcCaps.FlagIsSet(FR_LOAD); }
  bool HaveSaver(void) const { return dffcCaps.FlagIsSet(FR_SAVE); }
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
    dffcCaps{ FR_LOAD },               // Set load only capability
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
    dffcCaps{ FR_SAVE },               // Set save only capability
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
    dffcCaps{ FR_LOAD|FR_SAVE },       // Set load and save capabilities
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
