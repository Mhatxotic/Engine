/* == CODEC.HPP ============================================================ **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## This file provides a class structure that can unify many codecs     ## **
** ## into a simple magic block for use with Lua.                         ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace ICodec {                     // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace ICrypt::P;             using namespace IError::P;
using namespace IMemory::P;            using namespace IStd::P;
using namespace Lib::OS::OpenSSL;      using namespace Lib::OS::SevenZip;
using namespace Lib::OS::ZLib;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* ------------------------------------------------------------------------- */
enum EncMode : unsigned int            // Encoding flags (DONT REORDER!!!)
{ /* ----------------------------------------------------------------------- */
  ENCMODE_RAW,                         // Raw format (Copy)
  ENCMODE_AES,                         // Encrypted with AES (openssl)
  ENCMODE_DEFLATE,                     // Deflated with ZLIB (zip)
  ENCMODE_LZMA,                        // Compressed with LZMA (7zip)
  ENCMODE_ZLIBAES,                     // Compressed and encrypted (zip+ossl)
  ENCMODE_LZMAAES,                     // Compressed and encrypted (lzma+ossl)
  ENCMODE_MAX                          // Maximum number of types supported
};/* ----------------------------------------------------------------------- */
class EncData                          // Encoded data returned by callback
{ /* ----------------------------------------------------------------------- */
  const EncMode    eMode;              // Encoded data type
  const size_t     stUncompressed;     // Bytes of uncompressed data
  const size_t     stCompressed;       // Bytes of compressed data
  const size_t     stExtra;            // Bytes of extra data stored
  /* --------------------------------------------------------------- */ public:
  EncMode GetMode(void) const { return eMode; }
  size_t GetUncompressed(void) const { return stUncompressed; }
  size_t GetCompressed(void) const { return stCompressed; }
  size_t GetExtra(void) const { return stExtra; }
  /* -- Init constructor --------------------------------------------------- */
  EncData(const EncMode eM, const size_t stU, const size_t stC,
    const size_t stX) :
    /* -- Initialisers ----------------------------------------------------- */
    eMode(eM),                         // Set requested mode
    stUncompressed(stU),               // Set requested uncompressed size
    stCompressed(stC),                 // Set requested compressed size
    stExtra(stX)                       // Set extra bytes size
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Copy constructor --------------------------------------------------- */
  explicit EncData(const EncData &edOther) :
    /* -- Initialisers ----------------------------------------------------- */
    eMode(edOther.GetMode()),                  // Copy requested mode
    stUncompressed(edOther.GetUncompressed()), // Copy uncompressed size
    stCompressed(edOther.GetCompressed()),     // Copy compressed size
    stExtra(edOther.GetExtra())                // Copy extra bytes size
    /* -- No code ---------------------------------------------------------- */
    { }
};/* -- Magic block header data -------------------------------------------- */
enum Header : unsigned long            // Buffer header integer location flags
{ /* ----------------------------------------------------------------------- */
  ENCHDR_MAGIC           = 0x1A43444D, // Magic (MDC[esc])
  ENCHDR_VERSION         =          1, // Structure version
  /* ----------------------------------------------------------------------- */
  ENCHDR_POS_MAGIC       =          0, // Offset of magic identifier
  ENCHDR_POS_VERSION     =          4, // Offset of version identifier
  ENCHDR_POS_FLAGS       =          8, // Offset of flags (ENCMODE_*)
  ENCHDR_POS_CBLEN       =         12, // Offset of compressed block length
  ENCHDR_POS_UBLEN       =         16, // Offset of uncompressed block length
  ENCHDR_POS_XLEN        =         20, // Offset of extra bytes length
  /* ----------------------------------------------------------------------- */
  ENCHDR_SIZE            =         24, // Size of compressed block header
};/* ----------------------------------------------------------------------- */
static const EncData CodecEncodeAES(const MemConst &mcSrc, Memory &mDest,
  const size_t, const Crypt::QPKey &qaKey, const Crypt::QIVKey &qaIV,
  const size_t stPos)
{ // Unique ptr type to free the cipher context
  typedef unique_ptr<EVP_CIPHER_CTX,
    function<decltype(EVP_CIPHER_CTX_free)>> EvpPtr;
  // Encryption context. Return failed if it coulnd't be created
  if(const EvpPtr ecCTX{ EVP_CIPHER_CTX_new(), EVP_CIPHER_CTX_free })
  { // IV keys typecast as unsigned char
    const unsigned char*const ucpKey =
      reinterpret_cast<const unsigned char*>(qaKey.data());
    const unsigned char*const ucpIV =
      reinterpret_cast<const unsigned char*>(qaIV.data());
    // Initialise the encryption operation. IMPORTANT- ensure you use a key
    // and IV size appropriate for your cipher. In this example we are
    // using 256 bit AES (i.e. a 256 bit key). The IV size for *most* modes
    // is the same as the block size. For AES this is 128 bits
    if(EVP_EncryptInit_ex(ecCTX.get(),
      EVP_aes_256_cbc(), nullptr, ucpKey, ucpIV) != 1)
        XC("Encrypt init failed!");
    // In and out length and buffer typecast to required types
    const int iInLen = static_cast<int>(mcSrc.MemSize());
    int iOutLen = static_cast<int>(mDest.MemSize());
    const unsigned char*const ucpIn = mcSrc.MemPtr<unsigned char>();
    unsigned char*const ucpOut = mDest.MemRead<unsigned char>(stPos);
    // Provide the message to be encrypted, and obtain the encrypted
    // output. EVP_EncryptUpdate can be called multiple times if necessary
    // Also, save the amount of bytes that were initially written
    if(EVP_EncryptUpdate(ecCTX.get(), ucpOut, &iOutLen, ucpIn, iInLen) != 1)
      XC("Encrypt data failed!");
    // Set a write counter and make sure we don't overflow
    size_t stWrote = static_cast<size_t>(iOutLen);
    // Finalise the encryption. Further ciphertext bytes may be written at
    // this stage.
    if(EVP_EncryptFinal_ex(ecCTX.get(), ucpOut + iOutLen, &iOutLen) != 1)
      XC("Encrypt final data failed!");
    // Add to written bytes
    stWrote += static_cast<size_t>(iOutLen);
    // Success
    return { ENCMODE_AES, mcSrc.MemSize(), stWrote, 0 };
  } // Context creation failed!
  else XC("Failed to create encode cipher context!");
}
/* -- Return size of AES256 cipher block ----------------------------------- */
static size_t CodecGetAES256CBCSize(void)
  { return static_cast<size_t>(EVP_CIPHER_block_size(EVP_aes_256_cbc())); }
/* ------------------------------------------------------------------------- */
static void CodecDecodeAES(const MemConst &mcSrc, Memory &mDest,
  const size_t stPos, const size_t stIn, const size_t stOut,
  const Crypt::QPKey &qaKey, const Crypt::QIVKey &qaIV)
{ // Create output
  mDest.MemResize(stOut + CodecGetAES256CBCSize());
  // Unique ptr type to free the cipher context
  typedef unique_ptr<EVP_CIPHER_CTX,
    function<decltype(EVP_CIPHER_CTX_free)>> EvpPtr;
  // Init encryption context and if succeeded
  if(const EvpPtr ecCTX{ EVP_CIPHER_CTX_new(), EVP_CIPHER_CTX_free })
  { // IV keys typecast as unsigned char
    const unsigned char*const ucpKey =
      reinterpret_cast<const unsigned char*>(qaKey.data());
    const unsigned char*const ucpIV =
      reinterpret_cast<const unsigned char*>(qaIV.data());
    // Initialise the encryption operation. IMPORTANT- ensure you use a
    // key and IV size appropriate for your cipher. In this example we
    // are using 256 bit AES (i.e. a 256 bit key). The IV size for *most*
    // modes is the same as the block size. For AES this is 128 bits.
    if(EVP_DecryptInit_ex(ecCTX.get(), EVP_aes_256_cbc(),
      nullptr, ucpKey, ucpIV) != 1)
        XC("Decrypt init failed!");
    // In and out length and buffer typecast to required types
    const int iInLen = static_cast<int>(stIn);
    int iOutLen = static_cast<int>(stOut);
    const unsigned char*const ucpIn =
      mcSrc.MemRead<unsigned char>(stPos, stIn);
    unsigned char*const ucpOut = mDest.MemPtr<unsigned char>();
    // Provide the message to be decrypted, and obtain the encrypted
    // output. EVP_EncryptUpdate can be called multiple times if
    // necessary
    if(EVP_DecryptUpdate(ecCTX.get(), ucpOut, &iOutLen, ucpIn, iInLen) != 1)
      XC("Decrypt update failed!");
    // Record amount of bytes written and check for overflow
    size_t stWrote = static_cast<size_t>(iOutLen);
    // Finalise the encryption. Further ciphertext bytes may be written
    // at this stage.
    if(EVP_DecryptFinal_ex(ecCTX.get(), ucpOut + iOutLen, &iOutLen) != 1)
      XC("Decrypt final failed!");
    // Add to written bytes
    stWrote += static_cast<size_t>(iOutLen);
    // Resize to actual uncompressed size
    mDest.MemResize(stWrote);
  } // Context creation failed!
  else XC("Failed to decode create cipher context!");
}
/* -- Stringify a 7zip error code ------------------------------------------ */
static const char *CodecGetLzmaErrString(const int iCode)
{ // Helpful temporary defines
#define SZERR "SZ_ERROR_"
#define CASE(n) case SZ_ERROR_ ## n: return SZERR STR(n)
  // Which code
  switch(iCode)
  { CASE(DATA);        CASE(MEM);        CASE(CRC);
    CASE(UNSUPPORTED); CASE(PARAM);      CASE(INPUT_EOF);
    CASE(OUTPUT_EOF);  CASE(READ);       CASE(WRITE);
    CASE(PROGRESS);    CASE(FAIL);       CASE(THREAD);
    CASE(ARCHIVE);     CASE(NO_ARCHIVE);
  } // Unknown error
  return SZERR "UNKNOWN";
  // Done with these defines
#undef CASE
#undef SZERR
}
/* -- Stringify zlib error code -------------------------------------------- */
static const char *CodecGetZLIBErrString(const int iCode)
{ // Helpful temporary defines
#define ZERR "Z_"
#define CASE(n) case Z_ ## n: return ZERR STR(n)
  // Check zlib code
  switch(iCode)
  { CASE(OK);        CASE(STREAM_END);   CASE(NEED_DICT);
    CASE(ERRNO);     CASE(STREAM_ERROR); CASE(DATA_ERROR);
    CASE(MEM_ERROR); CASE(BUF_ERROR);    CASE(VERSION_ERROR);
  } // Unknown error
  return ZERR "UNKNOWN";
  // Done with these defines
#undef CASE
#undef ZERR
}
/* ------------------------------------------------------------------------- */
static const EncData CodecEncodeAESCompressed(const EncMode eOverrideMode,
  const EncData &encCompressedData, Memory &mDest, const size_t stLevel,
  const Crypt::QPKey &qaKey, const Crypt::QIVKey &qaIV, const size_t stPos)
{ // Resize to fit the amount of compressed and extra data
  mDest.MemResize(sizeof(uint32_t) +
    encCompressedData.GetCompressed() +
    encCompressedData.GetExtra());
  // Store the size of the uncompressed block. The size of the compressed block
  // is already stored as the size of the uncompressed block in the actual
  // header.
  mDest.MemWriteIntLE<uint32_t>
    (static_cast<uint32_t>(encCompressedData.GetUncompressed()));
  // Discord compressed block and replace with encrypted block (dirty but meh)
  const Memory aCompressed{ StdMove(mDest) };
  // Resize the output block with enough memory for encrypting
  mDest.MemResize(ENCHDR_SIZE +
    aCompressed.MemSize() + CodecGetAES256CBCSize());
  // Encrypt data and get result
  const EncData encEncryptedData{
    CodecEncodeAES(aCompressed, mDest, stLevel, qaKey, qaIV, stPos) };
  // Return data with original extra bytes value
  return { eOverrideMode,
           encCompressedData.GetUncompressed(),
           encEncryptedData.GetCompressed(),
           encCompressedData.GetExtra() };
}
/* ------------------------------------------------------------------------- */
static const EncData CodecEncodeZLIB(const MemConst &mcSrc, Memory &mDest,
  const size_t stLevel, const size_t stPos)
{ // Input address and size, check the size
  const Bytef*const ucpSrc = mcSrc.MemPtr<Bytef>();
  const uLong ulSrc = mcSrc.MemSize<uLong>();
  if(static_cast<size_t>(ulSrc) != mcSrc.MemSize())
    XC("Input size not valid to deflate from memory!",
       "InputSizeExternal", mcSrc.MemSize(), "InputSizeInternal", ulSrc);
  // Output address and size, check the size
  const size_t stOut = mDest.MemSize() - stPos;
  Bytef*const ucpDest = mDest.MemRead<Bytef>(stPos);
  uLongf ulDest = static_cast<uLongf>(stOut);
  if(static_cast<size_t>(ulDest) != stOut)
    XC("Output size not valid to deflate from memory!",
       "OutputSizeExternal", stOut, "OutputSizeInternal", ulDest);
  // Do compress with default parameters and check for failure
  switch(const int iR = compress2(ucpDest, &ulDest, ucpSrc, ulSrc,
    static_cast<int>(stLevel)))
  { // Succeeded!
    case Z_OK: break;
    // Failed
    default: XC("Deflate failed!",
      "Code",         iR,    "Reason",        CodecGetZLIBErrString(iR),
      "Position",     stPos, "Input",         ucpSrc,
      "InputSize",    ulSrc, "Output",        ucpDest,
      "OutputSizeIn", stOut, "OutputSizeOut", ulDest,
      "Level",        stLevel);
  } // Return what we did to the encoder manager
  return { ENCMODE_DEFLATE, mcSrc.MemSize(), ulDest, 0 };
}
/* ------------------------------------------------------------------------- */
static const EncData CodecEncodeAESZLIB(const MemConst &mcSrc, Memory &mDest,
  const size_t stLevel, const Crypt::QPKey &qaKey, const Crypt::QIVKey &qaIV,
  const size_t stPos)
{ // Create memory for compressed data
  mDest.MemResize(mcSrc.MemSize() + sizeof(uint32_t) +
    static_cast<size_t>(ceil(mcSrc.MemSize() * 0.001)) + 12);
  // Compress then prepare and encode the compressed data
  return CodecEncodeAESCompressed(ENCMODE_ZLIBAES,
    CodecEncodeZLIB(mcSrc, mDest, stLevel, sizeof(uint32_t)),
      mDest, stLevel, qaKey, qaIV, stPos);
}
/* ------------------------------------------------------------------------- */
static const EncData CodecEncodeLZMA(const MemConst &mcSrc, Memory &mDest,
  const size_t stLevel, const size_t stPos)
{ // For the next function
  size_t stOutLen = mcSrc.MemSize(), stPropLen = 5;
  // Do compress with default parameters, return failure code if fails
  const int iResult = LzmaCompress(
    (mDest.MemRead<unsigned char>(stPos + stPropLen)),
    &stOutLen, mcSrc.MemPtr<unsigned char>(), mcSrc.MemSize(),
    (mDest.MemRead<unsigned char>(stPos)), &stPropLen,
    static_cast<int>(stLevel), 0, -1, -1, -1, -1, -1);
  if(iResult != SZ_OK)
    XC("Compress failed!",
       "Reason", CodecGetLzmaErrString(iResult), "Code", iResult);
  // Return what we did
  return { ENCMODE_LZMA, mcSrc.MemSize(), stOutLen, stPropLen };
}
/* ------------------------------------------------------------------------- */
static const EncData CodecEncodeAESLZMA(const MemConst &mcSrc, Memory &mDest,
  const size_t stLevel, const Crypt::QPKey &qaKey, const Crypt::QIVKey &qaIV,
  const size_t stPos)
{ // Create memory for compressed data
  mDest.MemResize(mcSrc.MemSize() + sizeof(uint32_t) + 5);
  // Compress then prepare and encode the compressed data
  return CodecEncodeAESCompressed(ENCMODE_LZMAAES,
    CodecEncodeLZMA(mcSrc, mDest, stLevel, sizeof(uint32_t)),
      mDest, stLevel, qaKey, qaIV, stPos);
}
/* ------------------------------------------------------------------------- */
static const EncData CodecEncodeRAW(const MemConst &mcSrc, Memory &mDest,
  const size_t, const size_t stPos)
{ // Copy the block over into the new one
  mDest.MemWriteBlock(stPos, mcSrc);
  // Return what we did
  return { ENCMODE_RAW, mcSrc.MemSize(), mcSrc.MemSize(), 0 };
}
/* ------------------------------------------------------------------------- */
static void CodecDecodeZLIB(const MemConst &mcSrc, Memory &mDest,
  const size_t stPos, const size_t stIn, const size_t stOut)
{ // Prepare and check source parameters and make sure source valid for ulong
  const Bytef*const ucpSrc = mcSrc.MemRead<Bytef>(stPos);
  const uLongf ulSrc = static_cast<uLongf>(stIn);
  if(static_cast<size_t>(ulSrc) != stIn)
    XC("Input size not valid to inflate from memory!",
       "InputSizeExternal", stIn, "InputSizeInternal", ulSrc);
  // Allocate buffer for output
  mDest.MemResize(stOut);
  // Prepare and check dest parameters and make sure dest valid for ulong
  Bytef*const ucpDest = mDest.MemPtr<Bytef>();
  uLongf ulDest = static_cast<uLongf>(stOut);
  if(static_cast<size_t>(ulDest) != stOut)
    XC("Output size not valid to inflate from memory!",
       "OutputSizeExternal", stOut, "OutputSizeInternal", ulDest);
  // Uncompress the block and compare result
  switch(const int iR = uncompress(ucpDest, &ulDest, ucpSrc, ulSrc))
  { // Succeeded!
    case Z_OK: break;
    // Failed
    default: XC("Inflate failed!",
      "Code",         iR,    "Reason",        CodecGetZLIBErrString(iR),
      "Position",     stPos, "Input",         ucpSrc,
      "InputSize",    ulSrc, "Output",        ucpDest,
      "OutputSizeIn", stOut, "OutputSizeOut", ulDest);
  } // Resize to actual uncompressed size
  mDest.MemResize(ulDest);
}
/* ------------------------------------------------------------------------- */
static void CodecDecodeAESZLIB(const MemConst &mcSrc, Memory &mDest,
  const size_t stPos, const size_t stIn, const size_t stOut,
  const Crypt::QPKey &qaKey, const Crypt::QIVKey &qaIV)
{ // First decode the
  CodecDecodeAES(mcSrc, mDest, stPos, stIn, stOut, qaKey, qaIV);
  // Grab size of compressed block
  const size_t stUncompressed =
    static_cast<size_t>(mDest.ReadIntLE<uint32_t>());
  // Move memory
  const Memory mIn{ StdMove(mDest) };
  // Decompress the block now from the start
  CodecDecodeZLIB(mIn, mDest, sizeof(uint32_t),
    mcSrc.MemSize()-sizeof(uint32_t), stUncompressed);
}
/* ------------------------------------------------------------------------- */
static void CodecDecodeLZMA(const MemConst &mcSrc, Memory &mDest,
  const size_t stPos, size_t stIn, size_t stOut, const size_t stExtra)
{ // Create output
  mDest.MemResize(stOut);
  // For decompress and return success or failure
  switch(const int iR = LzmaUncompress(
    mDest.MemPtr<unsigned char>(),                 &stOut,
    mcSrc.MemRead<unsigned char>(stPos + stExtra), &stIn,
    mcSrc.MemRead<unsigned char>(stPos),            stExtra))
  { // Succeeded!
    case SZ_OK: break;
    // Failed!
    default: XC("Decompress failed!",
      "Reason",      CodecGetLzmaErrString(iR),
      "Code",        iR,              "Position",    stPos,
      "SrcTotalIn",  mcSrc.MemSize(), "SrcDataIn",   stIn,
      "DestTotalIn", mDest.MemSize(), "DestDataOut", stOut,
      "ExSize",      stExtra);
  } // Resize to actual uncompressed size
  mDest.MemResize(stOut);
}
/* ------------------------------------------------------------------------- */
static void CodecDecodeAESLZMA(const MemConst &mcSrc, Memory &mDest,
  const size_t stPos, const size_t stIn, const size_t stOut,
  const size_t stExtra, const Crypt::QPKey &qaKey, const Crypt::QIVKey &qaIV)
{ // First decode the
  CodecDecodeAES(mcSrc, mDest, stPos, stIn, stOut, qaKey, qaIV);
  // Grab size of compressed block
  const size_t stUncompressed =
    static_cast<size_t>(mDest.ReadIntLE<uint32_t>());
  // Move memory
  const Memory mIn{ StdMove(mDest) };
  // Decompress the block now from the start
  CodecDecodeLZMA(mIn, mDest, sizeof(uint32_t),
    mcSrc.MemSize()-sizeof(uint32_t), stUncompressed, stExtra);
}
/* ------------------------------------------------------------------------- */
static void CodecDecodeRAW(const MemConst &mcSrc, Memory &mDest,
  const size_t stPos, const size_t stIn, const size_t stOut)
{ // Create output
  mDest.MemResize(stOut);
  // Copy the block
  mDest.MemWrite(0, mcSrc.MemRead(stPos, stIn), stOut);
}
/* ========================================================================= */
class CoDecoder :                      // Magic decoder derivative class
  /* -- Base classes ------------------------------------------------------- */
  public Memory                        // Allocated memory block object
{ /* ----------------------------------------------------------------------- */
  void DecodeV1(const MemConst &mcSrc)
  { // Read compressed data length
    const size_t stCBLen = mcSrc.ReadIntLE<uint32_t>(ENCHDR_POS_CBLEN);
    // Read extra data length
    const size_t stXLen = mcSrc.ReadIntLE<uint32_t>(ENCHDR_POS_XLEN);
    // Calculate and test expected length of input
    const size_t stExpect = ENCHDR_SIZE + stXLen + stCBLen;
    if(mcSrc.MemSize() != stExpect)
      XC("Invalid data length!",
         "Expect", stExpect, "Actual", mcSrc.MemSize());
    // Get uncompressed block length
    const size_t stUBLen = mcSrc.ReadIntLE<uint32_t>(ENCHDR_POS_UBLEN);
    // No compressed block length?
    if(!stCBLen)
    { // No point doing anything if empty
      if(!stUBLen) return;
      // Error because this will never happen
      XC("Compressed/Uncompressed data size mismtch!",
          "Compressed", stCBLen, "Uncompressed", stUBLen);
    } // Compare mode and decode as necessary
    switch(const uint32_t ulMode =
      mcSrc.ReadIntLE<uint32_t>(ENCHDR_POS_FLAGS))
    { // Raw (copy)?
      case ENCMODE_RAW:
        CodecDecodeRAW(mcSrc, *this, ENCHDR_SIZE, stCBLen, stUBLen);
        break;
      // Decompress (lzma)?
      case ENCMODE_LZMA:
        CodecDecodeLZMA(mcSrc, *this, ENCHDR_SIZE, stCBLen, stUBLen, stXLen);
        break;
      // Inflate (zlib)?
      case ENCMODE_DEFLATE:
        CodecDecodeZLIB(mcSrc, *this, ENCHDR_SIZE, stCBLen, stUBLen);
        break;
      // Decrypt (openssl)?
      case ENCMODE_AES:
        CodecDecodeAES(mcSrc, *this, ENCHDR_SIZE, stCBLen, stUBLen,
          cCrypt->pkKey.p.qKey, cCrypt->pkKey.p.qIV); break;
      // Decrypt/decompress? (compressed length stored in stXLen)
      case ENCMODE_ZLIBAES:
        CodecDecodeAESZLIB(mcSrc, *this, ENCHDR_SIZE, stCBLen, stUBLen,
          cCrypt->pkKey.p.qKey, cCrypt->pkKey.p.qIV); break;
      // Decrypt/decompress? (compressed length stored in stXLen)
      case ENCMODE_LZMAAES:
        CodecDecodeAESLZMA(mcSrc, *this, ENCHDR_SIZE, stCBLen, stUBLen, stXLen,
          cCrypt->pkKey.p.qKey, cCrypt->pkKey.p.qIV); break;
      // Unknown
      default: XC("Unknown decoding method!", "Mode", ulMode);
    }
  }
  /* --------------------------------------------------------------- */ public:
  explicit CoDecoder(const MemConst &mcSrc, const size_t)
  { // Should have at least enough bytes for header
    const size_t stSizeExpect = ENCHDR_SIZE, stSizeActual = mcSrc.MemSize();
    if(stSizeActual < stSizeExpect)
      XC("Invalid size!", "Expect", stSizeExpect, "Actual", stSizeActual);
    // Check magic, version and input size
    const unsigned int uiMagicExpect = ENCHDR_MAGIC,
      uiMagicActual = mcSrc.ReadIntLE<uint32_t>(ENCHDR_POS_MAGIC);
    if(uiMagicActual != uiMagicExpect)
      XC("Invalid header!", "Expect", uiMagicExpect, "Actual", uiMagicActual);
    // Compare version
    switch(const unsigned int uiVersionActual =
      mcSrc.ReadIntLE<uint32_t>(ENCHDR_POS_VERSION))
    { // Version 1
      case 1: DecodeV1(mcSrc); break;
      // Unknown version
      default: XC("Invalid version!", "Version", uiVersionActual);
    }
  } /* --------------------------------------------------------------------- */
  DELETECOPYCTORS(CoDecoder)           // Suppress default functions for safety
};/* == Encoder base class ================================================= */
template<class EncPlugin>class CoEncoder :
  /* -- Base classes ------------------------------------------------------- */
  public Memory,                       // Allocated memory block object
  public EncPlugin                     // Plugin object
{ /* -- Initialises the header of the block ----------------------- */ private:
  void InitHeader(void)
  { // Resize memory to fit actual output size
    MemResize(ENCHDR_SIZE + this->GetCompressed() + this->GetExtra());
    // Set properties
    MemWriteIntLE<uint32_t>(ENCHDR_POS_MAGIC,
      static_cast<uint32_t>(ENCHDR_MAGIC));
    MemWriteIntLE<uint32_t>(ENCHDR_POS_VERSION,
      static_cast<uint32_t>(ENCHDR_VERSION));
    MemWriteIntLE<uint32_t>(ENCHDR_POS_FLAGS,
      static_cast<uint32_t>(this->GetMode()));
    MemWriteIntLE<uint32_t>(ENCHDR_POS_CBLEN,
      static_cast<uint32_t>(this->GetCompressed()));
    MemWriteIntLE<uint32_t>(ENCHDR_POS_UBLEN,
      static_cast<uint32_t>(this->GetUncompressed()));
    MemWriteIntLE<uint32_t>(ENCHDR_POS_XLEN,
      static_cast<uint32_t>(this->GetExtra()));
  }
  /* -- Constructor that doesn't initialise memory block size ------ */ public:
  CoEncoder(const MemConst &mcSrc, const size_t stLevel) :
    /* -- Initialisers ----------------------------------------------------- */
    EncPlugin{ mcSrc, *this, stLevel }
    /* -- Code ------------------------------------------------------------- */
    { InitHeader(); }
  /* -- Constructor that initialises memory block size --------------------- */
  CoEncoder(const MemConst &mcSrc, const size_t stInit, const size_t stLevel) :
    /* -- Initialisers ----------------------------------------------------- */
    Memory{ ENCHDR_SIZE + mcSrc.MemSize() + stInit },
    EncPlugin{ mcSrc, *this, stLevel }
    /* -- Code ------------------------------------------------------------- */
    { InitHeader(); }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(CoEncoder)           // Suppress default functions for safety
};/* ======================================================================= **
** ######################################################################### **
** ## Encoder helpers                                                     ## **
** ######################################################################### **
** -- Command helper interface to a class with encoder data ---------------- */
#define CODEC_HELPER(n,...) namespace CodecHelper { \
  class n ## Encoder : public EncData { \
    DELETECOPYCTORS(n ## Encoder) \
    public: n ## Encoder(const MemConst &dS, Memory &mD, const size_t stU) : \
      EncData{ CodecEncode ## n(dS, mD, stU, ## __VA_ARGS__, ENCHDR_SIZE) } {}\
  }; \
};
/* -- Encoder with an extra memory required intialiser variable ------------ */
#define CODEC_PLUGINEX(n,is,...) \
  CODEC_HELPER(n, ## __VA_ARGS__); \
  class n ## Encoder : public CoEncoder<CodecHelper::n ## Encoder> { \
    public: explicit n ## Encoder(const MemConst &dS, const size_t stU) : \
      CoEncoder<CodecHelper::n ## Encoder>{ dS, is, stU } {} };
/* -- Encoder without an initialiser variable ------------------------------ */
#define CODEC_PLUGIN(n,...) \
  CODEC_HELPER(n, ## __VA_ARGS__); \
  class n ## Encoder : public CoEncoder<CodecHelper::n ## Encoder> { \
    public: explicit n ## Encoder(const MemConst &dS, const size_t stU) : \
      CoEncoder<CodecHelper::n ## Encoder>{ dS, stU } {} };
/* -- The AES + ZLIB combined encoder (already provides init sizes) -------- */
CODEC_PLUGIN(AESZLIB, cCrypt->pkKey.p.qKey, cCrypt->pkKey.p.qIV)
/* -- The AES + LZMA combined encoder (already provides init sizes) -------- */
CODEC_PLUGINEX(AESLZMA, 5, cCrypt->pkKey.p.qKey, cCrypt->pkKey.p.qIV)
/* -- The AES only encoder (needs init size and keys) ---------------------- */
CODEC_PLUGINEX(AES, CodecGetAES256CBCSize(), cCrypt->pkKey.p.qKey,
  cCrypt->pkKey.p.qIV)
/* -- The ZLIB encoder class (provides init size) -------------------------- */
CODEC_PLUGINEX(ZLIB, static_cast<size_t>(ceil(dS.MemSize() * 0.001)) + 12)
/* -- The LZMA encoder class (provides the extra data needed size) --------- */
CODEC_PLUGINEX(LZMA, 5)
/* -- The nullptr encoder (no extra size needed) --------------------------- */
CODEC_PLUGINEX(RAW, 0)
/* ------------------------------------------------------------------------- */
#undef CODEC_PLUGIN                    // Done need this anymore
#undef CODEC_HELPER                    // Done need this anymore
/* == Main interface class ================================================= */
template<class EncoderType>class Block final : public EncoderType
{ /* -- Initialise by data array ----------------------------------- */ public:
  explicit Block(const MemConst &mcSrc, const size_t stUser=StdMaxSizeT) :
    /* -- Initialisers ----------------------------------------------------- */
    EncoderType{ mcSrc, stUser }
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Initialise by string ----------------------------------------------- */
  explicit Block(const string &strIn, const size_t stUser=StdMaxSizeT) :
    /* -- Initialisers ----------------------------------------------------- */
    EncoderType{ StdMove(MemConst{ strIn }), stUser }
    /* -- No code ---------------------------------------------------------- */
    { }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(Block)               // Suppress default functions for safety
};/* ----------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
