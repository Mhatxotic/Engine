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
/* -- Public typedefs ------------------------------------------------------ */
BUILD_FLAGS(DataFormat,                // Filter properties
/* ------------------------------------------------------------------------- */
  DF_DECODE                 {Flag(1)}, // Filter has a decoder?
  DF_ENCODE                 {Flag(2)}  // Filter has an encoder?
);/* ----------------------------------------------------------------------- */
template<class DataType,               // Data storage type
         typename FormatType,          // Data type id
         typename SlotType=bool>       // Slot type (save only)
/* ------------------------------------------------------------------------- */
class DataFormat                       // Image libraries format object class
{ /* -- Protected typedefs -------------------------------------- */ protected:
  typedef bool (CbFuncDecoderT)(FileMap&, DataType&);
  typedef bool (CbFuncEncoderT)(const FStream&, const DataType&,
    const SlotType&);
  /* -- Public typedefs -------------------------------------------- */ public:
  typedef function<CbFuncDecoderT> CbFuncDecoder;
  typedef function<CbFuncEncoderT> CbFuncEncoder;
  /* -- Private variables ----------------------------------------- */ private:
  const string_view strvName,          // Name of plugin
                    strvExt;           // Default extension of plugin type
  const DataFormatFlagsConst dffcCaps; // Capabilities
  const CbFuncDecoder cfdFunc;         // Loader function
  const CbFuncEncoder cfeFunc;         // Saver function
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
  static bool NoDecoder(FileMap&, DataType&) { return false; }
  static bool NoEncoder(const FStream&, const DataType&, const SlotType&)
    { return false; }
  /* -- Get members ------------------------------------------------ */ public:
  const CbFuncDecoder &GetDecoder() const { return cfdFunc; }
  const CbFuncEncoder &GetEncoder() const { return cfeFunc; }
  const string_view &GetName() const { return strvName; }
  const string_view &GetExt() const { return strvExt; }
  bool HaveDecoder() const { return dffcCaps.FlagIsSet(DF_DECODE); }
  bool HaveEncoder() const { return dffcCaps.FlagIsSet(DF_ENCODE); }
  /* -- Constructor with loader function only ------------------- */ protected:
  explicit DataFormat(
    /* -- Required arguments ----------------------------------------------- */
    const FormatType ftNId,            // The IFMT_* id
    const string_view &strvNName,      // The name of the codec
    const string_view &strvNExt,       // The default extension for the codec
    const CbFuncDecoder &cfdNFunc,     // Function to call when loading
    const size_t stSize                // Size of collector to id
    ): /* -- Initialisers -------------------------------------------------- */
    strvName{ strvNName },             // Set name for filter
    strvExt{ strvNExt },               // Set extension for filter
    dffcCaps{ DF_DECODE },             // Set load only capability
    cfdFunc{ cfdNFunc },               // Set loader function
    cfeFunc{ NoEncoder },              // Set no saver function
    ftId(CheckId(ftNId, stSize))       // Set unique id for this filter
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- Constructor with saver function only ------------------------------- */
  explicit DataFormat(
    /* -- Required arguments ----------------------------------------------- */
    const FormatType ftNId,            // The IFMT_* id
    const string_view &strvNName,      // The name of the codec
    const string_view &strvNExt,       // The default extension for the codec
    const CbFuncEncoder &cfeNFunc,     // Function to call when saving
    const size_t stSize                // Size of collector to id
    ): /* -- Initialisers -------------------------------------------------- */
    strvName{ strvNName },             // Set name for filter
    strvExt{ strvNExt },               // Set extension for filter
    dffcCaps{ DF_ENCODE },             // Set save only capability
    cfdFunc{ NoDecoder },              // Set no loader function
    cfeFunc{ cfeNFunc },               // Set saver function
    ftId(CheckId(ftNId, stSize))       // Set unique id for this filter
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- Constructor with both loader and saver functions ------------------- */
  explicit DataFormat(
    /* -- Required arguments ----------------------------------------------- */
    const FormatType ftNId,            // The IFMT_* id
    const string_view &strvNName,      // The name of the codec
    const string_view &strvNExt,       // The default extension for the codec
    const CbFuncDecoder &cfdNFunc,     // Function to call when loading
    const CbFuncEncoder &cfeNFunc,     // Function to call when saving
    const size_t stSize                // Size of collector to id
    ): /* -- Initialisers -------------------------------------------------- */
    strvName{ strvNName },             // Set name for filter
    strvExt{ strvNExt },               // Set extension for filter
    dffcCaps{ DF_DECODE|DF_ENCODE },   // Set load and save capabilities
    cfdFunc{ cfdNFunc },               // Set loader function
    cfeFunc{ cfeNFunc },               // Set saver function
    ftId(CheckId(ftNId, stSize))       // Set unique id for this filter
    /* -- No code ---------------------------------------------------------- */
    {}
};/* ----------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
