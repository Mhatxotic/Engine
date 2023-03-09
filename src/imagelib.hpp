/* == IMAGELIB.HPP ========================================================= **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## This file manages all the different image types we support in the   ## **
** ## engine. The actual image class will query this manager to test      ## **
** ## To use this, you build your class and derive your class with this   ## **
** ## class to enable the plugin to be used by the Image class            ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IImageLib {                  // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace ICollector::P;         using namespace IDataFormat::P;
using namespace IDir::P;               using namespace IError::P;
using namespace IFileMap::P;           using namespace IFStream::P;
using namespace IImageDef::P;          using namespace IIdent::P;
using namespace ILog::P;               using namespace ILuaIdent::P;
using namespace ILuaLib::P;            using namespace IStd::P;
using namespace IString::P;            using namespace ISysUtil::P;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* -- Image libraries collector class as a vector for direct access -------- */
CTOR_BEGIN_CUSTCTR(ImageLibs, ImageLib, vector, CLHelperUnsafe)
/* -- Image libraries format object class ---------------------------------- */
CTOR_MEM_BEGIN_CSLAVE(ImageLibs, ImageLib, ICHelperUnsafe),
  /* -- Base classes ------------------------------------------------------- */
  public DataFormat<ImageData, ImageFormat, ImageSlot>
{ /* -- Constructor with loader function only ---------------------- */ public:
  explicit ImageLib(
    /* -- Required arguments ----------------------------------------------- */
    const ImageFormat ifNId,           // The IFMT_* id
    const string_view &strvNName,      // The name of the codec
    const string_view &strvNExt,       // The default extension for the codec
    const CbFuncDecoder &cfdNFunc      // Function to call when loading
    ): /* -- Initialisers -------------------------------------------------- */
    ICHelperImageLib{ cImageLibs,      // Register filter in filter list
      this },                          // Initialise filter parent
    IdentCSlave{ cParent->CtrNext() }, // Initialise identification number
    DataFormat{ ifNId, strvNName, strvNExt, cfdNFunc, cParent->size() }
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Constructor with saver function only ------------------------------- */
  explicit ImageLib(
    /* -- Required arguments ----------------------------------------------- */
    const ImageFormat ifNId,           // The IFMT_* id
    const string_view &strvNName,      // The name of the codec
    const string_view &strvNExt,       // The default extension for the codec
    const CbFuncEncoder &cfeNFunc      // Function to call when saving
    ): /* -- Initialisers -------------------------------------------------- */
    ICHelperImageLib{ cImageLibs,      // Register filter in filter list
      this },                          // Initialise filter parent
    IdentCSlave{ cParent->CtrNext() }, // Initialise identification number
    DataFormat{ ifNId, strvNName, strvNExt, cfeNFunc, cParent->size() }
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Constructor with both loader and saver functions ------------------- */
  explicit ImageLib(
    /* -- Required arguments ----------------------------------------------- */
    const ImageFormat ifNId,           // The IFMT_* id
    const string_view &strvNName,      // The name of the codec
    const string_view &strvNExt,       // The default extension for the codec
    const CbFuncDecoder &cfdNFunc,     // Function to call when loading
    const CbFuncEncoder &cfeNFunc      // Function to call when saving
    ): /* -- Initialisers -------------------------------------------------- */
    ICHelperImageLib{ cImageLibs,      // Register filter in filter list
      this },                          // Initialise filter parent
    IdentCSlave{ cParent->CtrNext() }, // Initialise identification number
    DataFormat{ ifNId, strvNName, strvNExt, cfdNFunc, cfeNFunc,
      cParent->size() }
    /* -- No code ---------------------------------------------------------- */
    { }
};/* -- End of objects collector (reserve and set limit for formats) ------- */
CTOR_END(ImageLibs, ImageLib, IMAGELIB,
  reserve(IFMT_MAX); CollectorSetLimit(IFMT_MAX),)
/* -- Save a image using a specific type ----------------------------------- */
static void ImageSave(const ImageFormat ifId, const string &strFile,
  const ImageData &idData, const ImageSlot &isData)
{ // Get plugin class
  const ImageLib &ilRef = *cImageLibs->at(ifId);
  // Set filename with forced extension so we can delete it if it fails
  const string strFileNX{ StrAppend(strFile, '.', ilRef.GetExt()) };
  bool bCreated = false;
  // Capture exceptions
  try
  { // Create file on disk and report error if failed. Closed automatically
    // when leaving this scope. If there is an exception then the stream is
    // automatically closed
    if(FStream fsData{ strFileNX, FM_W_B })
    { // Created file
      bCreated = true;
      // Save the image and log the result if succeeded?
      if(ilRef.GetEncoder()(fsData, idData, isData))
      { // Log that we saved the image successfully and return
        cLog->LogInfoExSafe("Image saved '$' as $<$>! ($x$x$)",
          strFileNX, ilRef.GetExt(), ifId, isData.DimGetWidth(),
          isData.DimGetHeight(), idData.GetBitsPerPixel());
        return;
      } // Could not detect format so throw error
      throw runtime_error{ "Failed to save image!" };
    } // Failed to create file
    XCL("Failed to create file!", "File", strFileNX);
  } // Error occured. Error used as title
  catch(const exception &eReason)
  { // Remove file if created
    if(bCreated) DirFileUnlink(strFileNX);
    // Throw an error with the specified reason
    XC(eReason, "Identifier", strFileNX,
                "FormatId",   ifId,
                "Plugin",     ilRef.GetName());
  }
}
/* -- Load a image using a specific type ----------------------------------- */
static void ImageLoad(const ImageFormat ifId, FileMap &fmData,
  ImageData &idData)
{ // Get plugin class
  const ImageLib &ilRef = *cImageLibs->at(ifId);
  // Capture exceptions
  try
  { // Load the image, log and return and if succeeded
    if(ilRef.GetDecoder()(fmData, idData))
      return cLog->LogInfoExSafe("Image loaded '$' directly as $<$>! ($x$x$)",
        fmData.IdentGet(), ilRef.GetExt(), ifId, idData.DimGetWidth(),
        idData.DimGetHeight(), idData.GetBitsPerPixel());
    // Could not detect format so throw error
    throw runtime_error{ "Unable to load image!" };
  } // Error occured. Error used as title
  catch(const exception &eReason)
  { // Throw an error with the specified reason
    XC(eReason, "Identifier", fmData.IdentGet(),
                "Size",       fmData.MemSize(),
                "Position",   fmData.FileMapTell(),
                "FormatId",   ifId,
                "Plugin",     ilRef.GetName());
  }
}
/* -- Load a image and automatically detect type --------------------------- */
static void ImageLoad(FileMap &fmData, ImageData &idData)
{ // For each plugin registered
  for(const ImageLib*const ilPtr : *cImageLibs)
  { // Get reference to plugin
    const ImageLib &ilRef = *ilPtr;
    // Capture exceptions
    try
    { // Load the image, log and return if we loaded successfully
      if(ilRef.GetDecoder()(fmData, idData))
        return cLog->LogInfoExSafe("Image loaded '$' ($x$x$) as $!",
          fmData.IdentGet(), idData.DimGetWidth(), idData.DimGetHeight(),
          idData.GetBitsPerPixel(), ilRef.GetExt());
    } // Error occured. Error used as title
    catch(const exception &eReason)
    { // Throw an error with the specified reason
      XC(eReason, "Identifier", fmData.IdentGet(),
                  "Size",       fmData.MemSize(),
                  "Position",   fmData.FileMapTell(),
                  "Plugin",     ilRef.GetName());
    } // Rewind stream position
    fmData.FileMapRewind();
    // Reset other members to try next filter
    idData.ResetAllData();
  } // Could not detect so throw error
  XC("Unable to determine image format!", "Identifier", fmData.IdentGet());
}
/* ------------------------------------------------------------------------- */
static int ImageSwapPixels(char*const cpSrc, const size_t stSrc,
  const size_t stPixel, const size_t stSwapA, const size_t stSwapB)
{ // Check parameters
  if(!stSrc) return 0;
  if(!cpSrc) return -1;
  if(stSwapA >= stPixel) return -2;
  if(stSwapB >= stPixel) return -3;
  if(stPixel >= stSrc) return -4;
  if(stSwapA == stSwapB) return -5;
  // Do swap the pixels
  for(size_t stPos = 0; stPos < stSrc; stPos += stPixel)
  { // Calculate source position and copy character at source
    const size_t stPosA = stPos+stSwapA;
    const char cByte = cpSrc[stPosA];
    // Calculate the destination position and do the swap
    const size_t stPosB = stPos+stSwapB;
    cpSrc[stPosA] = cpSrc[stPosB];
    cpSrc[stPosB] = cByte;
  } // Success
  return 0;
}
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
