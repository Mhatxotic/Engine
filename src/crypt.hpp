/* == CRYPT.HPP ============================================================ **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## This file handles all the cryptographic and co/decompression        ## **
** ## routines and sets up the environment for OpenSSL library.           ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace ICrypt {                     // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IClock::P;             using namespace ICommon::P;
using namespace IError::P;             using namespace IHelper::P;
using namespace ILog::P;               using namespace IMemory::P;
using namespace IStd::P;               using namespace IString::P;
using namespace ISystem::P;            using namespace ISysUtil::P;
using namespace IToken::P;             using namespace IUtf::P;
using namespace IUtil::P;              using namespace Lib::OS::OpenSSL;
using namespace Lib::OS::SevenZip;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* -- Convert the specified character to hexadecimal ----------------------- */
template<int iFailCode>static int CryptHex2Char(unsigned char ucChar)
{ // Is a digit?
  if(ucChar >= '0' && ucChar <= '9') return ucChar - '0';
  // Is a upper-case letter?
  if(ucChar >= 'A' && ucChar <= 'F') return ucChar - 'A' + 10;
  // Is a lower-case letter?
  if(ucChar >= 'a' && ucChar <= 'f') return ucChar - 'a' + 10;
  // Is invalid? Just return zero
  return iFailCode;
}
/* -- Convert the specified hexadecimal string to 8-bit array -------------- */
static void CryptHexDecodePtr(const char* const cpSrc, const size_t stSrcLen,
  char* const cpDst)
{ // Walk the string
  for(size_t stInPos = 0, stOutPos = 0;
             stInPos < stSrcLen;
             stInPos += 2, ++stOutPos)
    // Decode the hexadecimal value
    cpDst[stOutPos] = static_cast<char>(
      (CryptHex2Char<0>(static_cast<unsigned char>(cpSrc[stInPos])) << 4) +
       CryptHex2Char<0>(static_cast<unsigned char>(cpSrc[stInPos + 1])));
}
/* -- Convert the specified hexadecimanl string to 8-bit array ------------- */
static Memory CryptHexDecodeA(const string &strSrc)
{ // Must not be empty and a multiple of two to comply
  if(strSrc.empty() || strSrc.size() % 2) return {};
  // The memory to output. We know what size will be
  Memory mbDst{ strSrc.size() / 2 };
  // Build 8-bit value from two ASCII characters
  CryptHexDecodePtr(strSrc.data(), strSrc.length(), mbDst.MemPtr<char>());
  // Return memory
  return mbDst;
}
/* -- Convert the specified hexadecimanl string to string ------------------ */
static string CryptHexDecodeStr(const string &strSrc)
{ // Must not be empty and a multiple of two to comply
  if(strSrc.empty() || strSrc.size() % 2) return {};
  // The memory to output. We know what size will be
  string strDst; strDst.resize(strSrc.size() / 2);
  // Build 8-bit value from two ASCII characters
  CryptHexDecodePtr(strSrc.data(), strSrc.length(),
    UtfToNonConstCast<char*>(strDst.data()));
  // Return memory
  return strDst;
}
/* -- Convert the specified 8-bit char to a uppercase hex string ----------- */
static void CryptChar2HexU(const uint8_t ucChar, char*const cpPtr)
{ // Hex lookup table
  static const char caHex[17] = "0123456789ABCDEF";
  // Decode to buffer
  cpPtr[0] = caHex[(ucChar >> 4) & 0x0F]; // High nibble
  cpPtr[1] = caHex[ucChar & 0x0F];        // Low nibble
}
/* -- Convert the specified 8-bit array to a hexadecmial string (upcase) --- */
static const string CryptBin2Hex(const uint8_t*const ucStr,
  const size_t stSize)
{ // The output string and we know what the size of the output will be so
  // we do not need to use an ostringstream object.
  string strOut; strOut.resize(stSize * 2, '\0');
  // Cast to a char so theres no warnings and process the buffer
  char *cpBuffer = &strOut[0];
  for(size_t stPos = 0; stPos < stSize; ++stPos)
    CryptChar2HexU(ucStr[stPos], &cpBuffer[stPos * 2]);
  // We're done. return the string!
  return strOut;
}
static const string CryptBin2Hex(const MemConst &mcSrc)
  { return CryptBin2Hex(mcSrc.MemPtr<uint8_t>(), mcSrc.MemSize()); }
/* -- Convert the specified 8-bit char to a lowercase hex string ----------- */
static void CryptChar2HexL(const uint8_t ucChar, char*const cpPtr)
{ // Hex lookup table
  static const char caHex[17] = "0123456789abcdef";
  // Decode to buffer
  cpPtr[0] = caHex[(ucChar >> 4) & 0x0F]; // High nibble
  cpPtr[1] = caHex[ucChar & 0x0F];        // Low nibble
}
/* -- Convert the specified 8-bit array to a hexadecmial string (lwcase) --- */
static const string CryptBin2HexL(const uint8_t*const ucStr,
  const size_t stSize)
{ // The output string and we know what the size of the output will be so
  // we do not need to use an ostringstream object.
  string strOut; strOut.resize(stSize * 2, '\0');
  // Cast to a char so theres no warnings and process the buffer
  char *cpBuffer = strOut.data();
  for(size_t stPos = 0; stPos < stSize; ++stPos)
    CryptChar2HexL(ucStr[stPos], &cpBuffer[stPos * 2]);
  // We're done. return the string!
  return strOut;
}
static const string CryptBin2HexL(const MemConst &mcSrc)
  { return CryptBin2HexL(mcSrc.MemPtr<uint8_t>(), mcSrc.MemSize()); }
/* ------------------------------------------------------------------------- */
static void CryptAddEntropyPtr(const void*const vpPtr, const size_t stSize)
  { RAND_add(vpPtr, UtilIntOrMax<int>(stSize), static_cast<double>(stSize)); }
/* ------------------------------------------------------------------------- */
template<typename IntType>void CryptAddEntropyInt(const IntType itValue)
  { CryptAddEntropyPtr(&itValue, sizeof(itValue)); }
/* ------------------------------------------------------------------------- */
template<typename StrType>void CryptAddEntropyStr(const StrType &strValue)
  { CryptAddEntropyPtr(strValue.data(), strValue.capacity()); }
/* ------------------------------------------------------------------------- */
static void CryptAddEntropy()
{ // Grab some data from the system
  CryptAddEntropyInt(cmSys.GetTimeUS());
  CryptAddEntropyInt(cmHiRes.GetTimeUS());
  CryptAddEntropyInt(cLog->CCDeltaUS());
  cSystem->SysUpdateCPUUsage();
  CryptAddEntropyInt(cSystem->CPUUsage());
  CryptAddEntropyInt(cSystem->CPUUsageSystem());
  cSystem->UpdateMemoryUsageData();
  CryptAddEntropyInt(cSystem->RAMFree());
  CryptAddEntropyStr(cSystem->ENGFull());
}
/* ------------------------------------------------------------------------- */
static void CryptRandomPtr(void*const vpDst, const size_t stSize)
{ // Add more entropy to the RNG
  CryptAddEntropy();
  // Fill data with random data
  RAND_bytes(reinterpret_cast<unsigned char*>(vpDst),
             static_cast<int>(stSize));
}
/* ------------------------------------------------------------------------- */
template<typename AnyType>static const AnyType CryptRandom()
{ // Do the randomisation into the requested type and return it
  AnyType atData;
  CryptRandomPtr(&atData, sizeof(atData));
  return atData;
}
/* -- URL encode the specified c-string ------------------------------------ */
static const string CryptURLEncode(const string &strS)
{ // Bail if passed string is empty
  if(strS.empty()) return {};
  // Movable pointer to input string
  const char *cpPtr = strS.data();
  // Preallocate string to avoid multiple reallocations. Worst case: every char
  // needs encoding.
  Reserved<string> strURL{ strS.size() * 3 };
  // Perform these actions for each character...
  do
  { // Get character
    const uint8_t ucC = static_cast<uint8_t>(*cpPtr);
    // Normal character? Append to string
    if(StdIsAlnum(ucC) || ucC == '-' || ucC == '.' || ucC == '_' || ucC == '~')
      strURL += static_cast<char>(ucC);
    else
    { // Create storage for buffer and put the hexadecimal inside it
      char cpBuf[2];
      CryptChar2HexU(ucC, cpBuf);
      // Put into string
      strURL += '%';
      strURL.append(cpBuf, sizeof(cpBuf));
    } // Repeat until end of string
  } while(*(++cpPtr));
  // Compact the URL
  strURL.shrink_to_fit();
  // End of string so return it
  return strURL;
}
/* ------------------------------------------------------------------------- */
template<class MapType>
  static const string CryptImplodeMapAndEncode[[maybe_unused]]
    (const MapType &mtRef, const string &strSep)
{ // The vector to return
  StrVector svRet;
  // Make pair type from passed map type
  typedef typename MapType::value_type PairType;
  // Iterate through each key pair and insert into vector whilst encoding
  transform(mtRef.cbegin(), mtRef.cend(), back_inserter(svRet),
    [](const PairType &ptRef)
      { return StdMove(StrAppend(CryptURLEncode(ptRef.first), '=',
          CryptURLEncode(ptRef.second))); });
  // Return vector
  return StrImplode(svRet, 0, strSep);
}
/* -- Get error reason ----------------------------------------------------- */
static string CryptGetErrorReason(const unsigned long ulErr)
{ // Clear and resize error buffer to maximum
  string strError; strError.resize(128);
  // Grab the error string from openssl and resize to correct size. Better to
  // use the non '_n' version so we can do all this in one line. Since
  // OpenSSL won't write more than 120 characters.
  strError.resize(static_cast<size_t>(
    strlen(ERR_error_string(ulErr, const_cast<char*>(strError.data())))));
  // Return error
  return strError;
}
/* -- Get error reason ----------------------------------------------------- */
static int CryptGetError(string &strError)
{ // Error to return
  strError.clear();
  // Error code
  int iError = -1;
  // Process errors... Only show errors we can actually report on
  // See ERR.H for actual error codes.
  while(const unsigned long ulErr = ERR_get_error())
  { // Set error number and string
    iError = static_cast<int>(ulErr);
    // Some statics
    static constexpr const unsigned int
      // Replacement for ERR_SYSTEM_MASK which causes warnings
      uiSystemMask = numeric_limits<int>::max(),
      // Replacement for ERR_SYSTEM_FLAG which causes warnings
      uiSystemFlag = uiSystemMask + 1;
    // Is a system error?
    const bool bSysErr = ulErr & uiSystemFlag;
    // Get library and reason...
    const unsigned int
      // Replacement for ERR_GET_LIB in err.h which causes warnings
      uiLib = bSysErr ?
        ERR_LIB_SYS : ((ulErr >> ERR_LIB_OFFSET) & ERR_LIB_MASK),
      // Replacement for ERR_GET_REASON in err.h which causes warning
      uiReason = bSysErr ?
        (ulErr & uiSystemFlag) : (ulErr & ERR_REASON_MASK);
    // If the operating system has the reason?
    if(uiLib == ERR_LIB_SYS || uiReason == ERR_R_SYS_LIB)
    { // Get system error and if no error set? Store what OpenSSL actually sent
      iError = cSystem->LastSocketOrSysError();
      strError = SysError();
      if(strError.empty()) strError = CryptGetErrorReason(ulErr);
      // Remove the rest of the errors because system errors are best
      else ERR_clear_error();
    } // An error we don't need to specially process
    else
    { // Clear the error and make room for the error message
      strError = CryptGetErrorReason(ulErr);
      // If theres a colon in it, delete everything up to that colon
      const size_t stColon = strError.find_last_of(':');
      if(stColon != StdNPos)
        strError = StrCapitalise(strError.substr(stColon + 1));
    }
  } // Free unused memory since the logevity of this value can be a while
  strError.shrink_to_fit();
  // Return the reason code
  return iError;
}
/* -- Get error reason without code ---------------------------------------- */
static string CryptGetError()
  { string strOut; CryptGetError(strOut); return strOut; }
/* -- Replacement for BIO_flush which causes warnings ---------------------- */
static int CryptBIOFlush(BIO*const bBio)
  { return static_cast<int>(BIO_ctrl(bBio, BIO_CTRL_FLUSH, 0, nullptr)); }
/* -- Replacement for BIO_get_mem_ptr which causes warnings ---------------- */
static void CryptBIOGetMemPtr(BIO*const bBio, BUF_MEM**const bmPtr)
  { BIO_ctrl(bBio, BIO_C_GET_BUF_MEM_PTR, 0, reinterpret_cast<void*>(bmPtr)); }
/* -- Replacement for BIO_get_fd which causes warnings --------------------- */
static int CryptBIOGetFd(BIO*const bBio)
  { return static_cast<int>(BIO_ctrl(bBio, BIO_C_GET_FD, 0, nullptr)); }
/* -- Replacement for BIO_set_conn_hostname which causes warnings ---------- */
static int CryptBIOSetConnHostname(BIO*const bBio, const char*const cpName)
  { return static_cast<int>(BIO_ctrl(bBio, BIO_C_SET_CONNECT, 0,
      UtfToNonConstCast<void*>(cpName))); }
/* -- Replacement for BIO_get_conn_addres which causes warnings ------------ */
static const BIO_ADDR *CryptBIOGetConnAddress(BIO*const bBio)
  { return reinterpret_cast<const BIO_ADDR*>(
      BIO_ptr_ctrl(bBio, BIO_C_GET_CONNECT, 2)); }
/* -- Replacement for BIO_get_ssl which causes warnings -------------------- */
static int CryptBIOGetSSL(BIO*const bBio, SSL**const sslDest)
  { return static_cast<int>(BIO_ctrl(bBio, BIO_C_GET_SSL, 0,
      reinterpret_cast<void*>(sslDest))); }
/* -- Replacement for SSL_CTX_set_tlsext_status_cb which causes warnings --- */
static int CryptSSLCtxSetTlsExtStatusCb(SSL_CTX*const sslCtx,
  int(*fCB)(SSL*,void*))
{ return static_cast<int>(SSL_CTX_callback_ctrl(sslCtx,
    SSL_CTRL_SET_TLSEXT_STATUS_REQ_CB,
      reinterpret_cast<void(*)()>(fCB))); }
/* -- Replacement for SSL_set_tlsext_host_name which causes warnings ------- */
static int CryptSSLSetTlsExtHostName(SSL*const sSSL, const char*const cpName)
  { return static_cast<int>(SSL_ctrl(sSSL, SSL_CTRL_SET_TLSEXT_HOSTNAME,
      TLSEXT_NAMETYPE_host_name, UtfToNonConstCast<void*>(cpName))); }
/* -- Replacement for SSL_CTX_set1_verify_cert_store which causes warnings - */
static int CryptSSLCtxSet1VerifyCertStore(SSL_CTX*const sslCtx,
  X509_STORE*const x509dest)
{ return static_cast<int>(SSL_CTX_ctrl(sslCtx, SSL_CTRL_SET_VERIFY_CERT_STORE,
    1, reinterpret_cast<void*>(x509dest))); }
/* ------------------------------------------------------------------------- */
static const string CryptPTRtoB64(const void*const vpIn, const size_t stIn)
{ // To clean up when leaving scope unexpectedliy
  typedef unique_ptr<BIO, function<decltype(BIO_free_all)>> BioPtr;
  // Create base 64 filter and if succeeded?
  if(BioPtr bB64{ BIO_new(BIO_f_base64()), BIO_free_all })
  { // Disable new line
    BIO_set_flags(bB64.get(), BIO_FLAGS_BASE64_NO_NL);
    // Create RAM based buffer filter and if succeeded?
    if(BIO*const bRAMraw = BIO_new(BIO_s_mem()))
    { // Assign RAM buffer to base 64 filter and if succeeded?
      if(BioPtr bRAM{ BIO_push(bB64.get(), bRAMraw), BIO_free_all })
      { // bB64 is now taken over by bRAM so make sure it doesn't destruct
        bB64.release();
        // Do conversion and if succeeded?
        size_t stBytesWritten;
        if(BIO_write_ex(bRAM.get(), vpIn, stIn, &stBytesWritten))
        { // Do flush and if succeeded?
          if(CryptBIOFlush(bRAM.get()))
          { // Get memory pointer (don't use *const, it will crash).
            BUF_MEM *bmPTR = nullptr;
            CryptBIOGetMemPtr(bRAM.get(), &bmPTR);
            // Done so return result
            return { bmPTR->data, bmPTR->length };
          } // Failed to flush stream
          XC("Failed to flush base64 encoder stream!",
            "InSize", stIn, "OutSize", stBytesWritten,
            "Reason", CryptGetError());
        } // Failed to decode base64
        XC("Failed to decode base64 stream!",
          "InSize", stIn, "Reason", CryptGetError());
      } // Failed to decode base64
      XC("Failed to assign RAM buffer to decode base64 stream!",
        "InSize", stIn, "Reason", CryptGetError());
    } // Failed to create RAM based buffer
    XC("Failed to create RAM based buffer to decode base64 stream!",
      "InSize", stIn, "Reason", CryptGetError());
  } // Failed to create base64 decoder context
  XC("Failed to create base64 decoder context!",
    "InSize", stIn, "Reason", CryptGetError());
}
/* ------------------------------------------------------------------------- */
static size_t CryptB64toPTR(void*const vpIn, const size_t stIn,
  void*const vpOut, const size_t stOut)
{ // To clean up when leaving scope unexpectedliy
  typedef unique_ptr<BIO, function<decltype(BIO_free_all)>> BioPtr;
  // Create base 64 filter, clean up if succeeded?
  if(BioPtr bB64{ BIO_new(BIO_f_base64()), BIO_free_all })
  { // Create RAM based buffer filter and clean up if succeeded?
    if(BIO*const bRAMraw = BIO_new_mem_buf(vpIn, static_cast<int>(stIn)))
    { // Assign RAM buffer to base 64 filter and if succeeded?
      if(BioPtr bRAM{ BIO_push(bB64.get(), bRAMraw), BIO_free_all })
      { // bB64 is now taken over by bRAM so make sure it is not released
        bB64.release();
        // Disable new line
        BIO_set_flags(bRAM.get(), BIO_FLAGS_BASE64_NO_NL);
        // Do conversion and return bytes read
        size_t stBytesRead;
        if(BIO_read_ex(bRAM.get(), vpOut, stOut, &stBytesRead))
          return stBytesRead;
        // Failure so raise exception
        XC("Failed to decode base64 data!",
          "InSize", stIn, "OutSize", stOut, "Reason", CryptGetError());
      } // Failure to assign RAM buffer
      XC("Failed to assign RAM buffer for base64 decoding!",
        "InSize", stIn, "OutSize", stOut, "Reason", CryptGetError());
    } // Failure to create RAM buffer filter
    XC("Failed to allocate memory for base64 decoding!",
      "InSize", stIn, "OutSize", stOut, "Reason", CryptGetError());
  }  // Failure to create base 64 filter
  XC("Failed to create base64 filter!",
    "InSize", stIn, "OutSize", stOut, "Reason", CryptGetError());
}
/* ------------------------------------------------------------------------- */
static const string CryptMBtoB64(const MemConst &mcSrc)
  { return CryptPTRtoB64(mcSrc.MemPtr<void>(), mcSrc.MemSize()); }
/* ------------------------------------------------------------------------- */
static const string CryptStoB64(const string &strIn)
  { return CryptPTRtoB64(UtfToNonConstCast<void*>(strIn.data()),
      strIn.length()); }
/* ------------------------------------------------------------------------- */
static Memory CryptB64toMB(const string &strIn)
{ // Output buffer
  Memory mData{ strIn.length() };
  // Do conversion and resize string after
  mData.MemResize(CryptB64toPTR(UtfToNonConstCast<void*>(strIn.data()),
    strIn.length(), mData.MemPtr(), mData.MemSize()));
  // Return data
  return mData;
}
/* ------------------------------------------------------------------------- */
static const string CryptB64toS(const string &strIn)
{ // Create output buffer with enough size for output
  string strOut; strOut.resize(strIn.length());
  // Do conversion and resize string after
  strOut.resize(CryptB64toPTR(UtfToNonConstCast<void*>(strIn.data()),
    strIn.length(), UtfToNonConstCast<void*>(strOut.data()),
    strOut.length()));
  // Return string
  return strOut;
}
/* -- Encode XML/HTML entities into string (crude but effective) ----------- */
static const string CryptEntEncode(const string &strS)
{ // Done if empty
  if(strS.empty()) return {};
  // Create sting to return and reserve memory. We will use a ostringstream
  // because we do not know what the size is going to be and we can make
  // use of hex which will work with our utf8 decoder.
  ostringstream osS; osS << hex;
  // For each entity. Find it in the string
  // Until null character. Which control token?
  for(UtfDecoder udSrc{ strS }; Codepoint cChar = udSrc.UtfNext();)
  { // If characters with special meaning in HTML that must be escaped in text
    // contexts? (ampersand, less-than, greater-than, quotation marks
    // (depending on context))?
    if(cChar == static_cast<Codepoint>('&') || // (U+0026)
       cChar == static_cast<Codepoint>('<') || // (U+003C)
       cChar == static_cast<Codepoint>('>') || // (U+003E)
       // Is control characters? (C0): U+0000..U+001F except allowed whitespace
       // (TAB U+0009, LF U+000A, CR U+000D)
       (cChar <= 0x001f && cChar != 0x0009 &&
        cChar != 0x000a && cChar != 0x000d) ||
       // Is DELETE control? (U+007F)
       cChar == 0x007f ||
       // Is C1 control characters? (U+0080..U+009F)
       (cChar > 0x0080 && cChar <= 0x009f) ||
       // Is surrogate code points character? (never valid Unicode scalar
       // values -> U+D800..U+DFFF)
       (cChar > 0xd800 && cChar <= 0xdfff) ||
       // Noncharacters? (U+FDD0..U+FDEF)
       (cChar > 0xfdd0 && cChar <= 0xfdef) ||
       // Any code point where low 16 bits are 0xFFFE or 0xFFFF?
       (cChar & 0xffff) >= 0xfffe)
      // Write the hexedecimal notation for the character instead
      osS << cCommon->CommonEnt() << cChar << ';';
    // Character is usable as is
    else osS << static_cast<char>(cChar);
  } // Return string
  return osS.str();
}
/* ------------------------------------------------------------------------- */
static const string CryptStoHEX(const string &strIn)
  { return CryptBin2Hex(reinterpret_cast<const uint8_t*>(strIn.data()),
      strIn.length()); }
/* ------------------------------------------------------------------------- */
static const string CryptStoHEXL(const string &strIn)
  { return CryptBin2HexL(reinterpret_cast<const uint8_t*>(strIn.data()),
      strIn.length()); }
/* -- Helper for mutliple hashing types from OpenSSL ----------------------- */
static Memory CryptHMACCall(const EVP_MD*const fFunc, const void*const vpSalt,
  const size_t stSaltSize, const unsigned char*const cpSrc,
  const size_t stSrcSize)
{ // Check sizes to make sure they can be converted to integer
  if(UtilIntWillOverflow<int>(stSaltSize))
    XC("Size of salt data too big!",
      "Requested", stSaltSize, "Maximum", numeric_limits<int>::max());
  if(UtilIntWillOverflow<int>(stSrcSize))
    XC("Size of source data to hash too big!",
      "Requested", stSrcSize, "Maximum", numeric_limits<int>::max());
  // Create output for HMAC-SHA hash
  Memory mData{ EVP_MAX_MD_SIZE };
  // For storage of size. We really need to know the output size.
  unsigned int uiLen = 0;
  // Get hash. Remember that HMAC returns as binary as we need to send the
  // whole buffer to Base64 and NOT a null-terminated string.
  if(!HMAC(fFunc, vpSalt, static_cast<int>(stSaltSize), cpSrc, stSrcSize,
    mData.MemPtr<unsigned char>(), &uiLen))
      XC("Failed to perform salted-hash on source data!",
        "Function",    fFunc != nullptr,
        "SaltAddress", vpSalt != nullptr, "SaltSize", stSaltSize,
        "SrcAddress",  cpSrc != nullptr,  "SrcSize", stSrcSize,
        "OutLength",   uiLen);
  // Resize string
  mData.MemResize(uiLen);
  // Return memory block
  return mData;
}
/* -- Hashing functions ---------------------------------------------------- */
#define DEFINE_HASH_FUNCS(x, s, f) \
  namespace x ## functions { \
    /* -- Hash raw ptr/size and return raw hash --------------------------- */\
    static Memory HMR(const unsigned char*const ucpIn, const size_t stLen) \
      { Memory mbOut{ s }; \
        x(ucpIn, stLen, mbOut.MemPtr<unsigned char>()); \
        return mbOut; } \
    /* -- Hash raw data and return raw hash ------------------------------- */\
    static Memory HMM(const MemConst &mcData) \
      { return HMR(mcData.MemPtr<unsigned char>(), mcData.MemSize()); } \
    /* -- Hash raw ptr/size and return string hash ------------------------ */\
    static const string HSR(const unsigned char*const ucpIn, \
      const size_t stLen) \
        { return CryptBin2HexL(StdMove(HMR(ucpIn, stLen))); } \
    /* -- Hash string data and return string hash ------------------------- */\
    static const string HSS(const string &strIn) \
      { return HSR(reinterpret_cast<const unsigned char*>(strIn.data()), \
          strIn.length()); } \
    /* -- Hash string data and return raw hash ---------------------------- */\
    static Memory HMS(const string &strIn) \
      { return HMR(reinterpret_cast<const unsigned char*>(strIn.data()), \
          strIn.length()); } \
    /* -- Hash raw data and return string hash ---------------------------- */\
    static const string HSM(const MemConst &mcData) \
      { return HSR(mcData.MemPtr<unsigned char>(), mcData.MemSize()); } \
    /* -- Hash raw key ptr/size, raw data ptr/size and return raw hash ---- */\
    static Memory HMRR(const void*const vpSalt, const size_t stSaltSize,\
      const unsigned char*const cpDest, const size_t stDestSize) \
        { return CryptHMACCall(f(), vpSalt, stSaltSize, cpDest, stDestSize); }\
    /* -- Hash string key, string data and return raw hash ---------------- */\
    static Memory HMSS(const string &strSalt, const string &strIn) \
      { return HMRR(reinterpret_cast<const void*>(strSalt.data()), \
                    strSalt.size(), \
                    reinterpret_cast<const unsigned char*>(strIn.data()), \
                    strIn.size()); } \
    /* -- Hash string key, string data and return string hash ------------- */\
    static const string HSSS(const string &strSalt, const string &strIn) \
      { return CryptBin2HexL(StdMove(HMSS(strSalt, strIn))); } \
    /* -- Hash raw key, raw data and return raw hash ---------------------- */\
    static Memory HMMM(const MemConst &mcSalt, const MemConst &mcData) \
      { return HMRR(mcSalt.MemPtr<const void>(), mcSalt.MemSize(), \
                    mcData.MemPtr<unsigned char>(), mcData.MemSize()); } \
    /* -- Hash raw key, string data and return raw hash ------------------- */\
    static Memory HMMS(const MemConst &mcSalt, const string &strData) \
      { return HMRR(mcSalt.MemPtr<const void*>(), \
                    mcSalt.MemSize(), \
                    reinterpret_cast<const unsigned char*>(strData.data()), \
                    strData.size()); } \
    /* -- Hash string key, raw data and return raw hash ------------------- */\
    static Memory HMSM(const string &strSalt, const MemConst &mcData) \
      { return HMRR(reinterpret_cast<const void*>(strSalt.data()), \
          strSalt.size(), mcData.MemPtr<unsigned char>(), mcData.MemSize()); }\
    /* -- Hash string key, string data and return string hash ------------- */\
    static const string HSMM(const MemConst &mcSalt, const MemConst &mcData) \
      { return CryptBin2HexL(StdMove(HMMM(mcSalt, mcData))); } \
    /* -- Hash string key, raw data and return string hash ---------------- */\
    static const string HSMS(const MemConst &mcSalt, const string &strData) \
      { return CryptBin2HexL(StdMove(HMMS(mcSalt, strData))); } \
    /* -- Hash raw key, string data and return string hash ---------------- */\
    static const string HSSM(const string &strSalt, const MemConst &mcData) \
      { return CryptBin2HexL(StdMove(HMSM(strSalt, mcData))); } \
  };/* --------------------------------------------------------------------- */
DEFINE_HASH_FUNCS(SHA1,   SHA_DIGEST_LENGTH,    EVP_sha1);   // Insecure
DEFINE_HASH_FUNCS(SHA224, SHA224_DIGEST_LENGTH, EVP_sha224); // Maybe secure
DEFINE_HASH_FUNCS(SHA256, SHA256_DIGEST_LENGTH, EVP_sha256); // Secure
DEFINE_HASH_FUNCS(SHA384, SHA384_DIGEST_LENGTH, EVP_sha384); // Very secure
DEFINE_HASH_FUNCS(SHA512, SHA512_DIGEST_LENGTH, EVP_sha512); // Really secure
#undef DEFINE_HASH_FUNCS
/* -- Create CRC32 hash of specified string using LZMA API ----------------- */
static unsigned int CryptToCRC32(const string &strIn)
  { return CrcCalc(strIn.data(), strIn.length()); }
/* -- Create CRC32 hash of specified memory block using LZMA API ----------- */
static unsigned int CryptToCRC32(const MemConst &mcSrc)
  { return CrcCalc(mcSrc.MemPtr<void>(), mcSrc.MemSize()); }
/* -- URL decode the specified c-string ------------------------------------ */
static string CryptURLDecode(const string &strS)
{ // Bail if passed string is invalid
  if(strS.empty()) return {};
  // Preallocate string to avoid multiple reallocations
  Reserved<string> strURL{ strS.size() };
  // Movable pointer to input string and perform actions for each character...
  for(const char *cpPtr = strS.data(); *cpPtr;)
  { // Get the character and if it denotes a encoded value?
    unsigned char ucC = static_cast<unsigned char>(*cpPtr);
    if(ucC == '%')
    { // Convert hex to number and if not at end of string?
      if(const unsigned char uc1 = static_cast<unsigned char>(*(++cpPtr)))
      { // Get the hexadecimal value and goto next character if not valid
        const int iVal1 = CryptHex2Char<-1>(uc1);
        if(iVal1 == -1) continue;
        // Get second hex character and if not at end of string?
        if(const unsigned char uc2 = static_cast<unsigned char>(*(++cpPtr)))
        { // Get the hexadecimal value and goto next character if not valid
          const int iVal2 = CryptHex2Char<-1>(uc2);
          if(iVal2 == -1) continue;
          // Set new character
          ucC = static_cast<unsigned char>(16 * iVal1 + iVal2);
        } // End of string
        else break;
      } // End of string
      else break;
    } // Normal character? Append to string
    strURL += static_cast<char>(ucC);
    cpPtr++;
  } // Compact the URL
  strURL.shrink_to_fit();
  // Return the url
  return strURL;
}
/* ------------------------------------------------------------------------- */
static Memory CryptRandomBlock(const size_t stSize)
{ // Memory to hold data
  Memory mData{ stSize };
  // Fill data with random data
  CryptRandomPtr(mData.MemPtr(), mData.MemSize());
  // Return array
  return mData;
}
/* -- Sanitise a string removing excessive letters and words --------------- */
static string CryptSanitise(const string &strMessage)
{ // Create output string and pre-allocate memory
  Reserved<string> strPruned{ strMessage.capacity() };
  // Last character processed and count
  char cLastChar = '\0';
  size_t stCount = 0;
  // Enumerate through the string
  for(char cChar : strMessage)
  { // Character is different
    if(cChar != cLastChar)
    { // Set new character found and reset times found
      cLastChar = cChar;
      stCount = 0;
      // Add character
      strPruned.push_back(cChar);
      // Next character
      continue;
    } // Increase repetition count and continue if over 2 repetitions
    if(++stCount > 1) continue;
    // Add character
    strPruned.push_back(cChar);
  } // Return if string is empty
  if(strPruned.empty()) return {};
  // Repeated words
  string strWord, strLastWord;
  // Output string
  Reserved<string> strOutput{ strPruned.size() };
  // Put pruned string into a string stream
  istringstream issS{ strPruned };
  // For each word
  while(issS >> strWord)
  { // Character is different?
    if(strWord != strLastWord)
    { // Set last word
      strLastWord = strWord;
      // Reset repetitions
      stCount = 0;
    } // Ignore repetitive uses of the word
    else if(++stCount >= 2) continue;
    // Set new character found and reset times found
    if(!strOutput.empty()) strOutput += ' ';
    // Add word
    strOutput += strWord;
  } // Trim string
  strOutput.shrink_to_fit();
  // Implode words and return output
  return strOutput;
}
/* -- Crypt manager class -------------------------------------------------- */
class Crypt;                           // Class prototype
static Crypt *cCrypt = nullptr;        // Address of global class
class Crypt :                          // Actual class body
  /* -- Base classes ------------------------------------------------------- */
  public InitHelper                    // The crypto manager class
{ /* ----------------------------------------------------------------------- */
  const StrVStrVMap svsvmEnt;          // Html entity decoding lookup table
  /* -- De-Initialise cryptographic systems -------------------------------- */
  void CryptDeInit()
  { // Ignore if not initialised
    if(IHNotDeInitialise()) return;
    // De-initialise openssl
    OPENSSL_cleanup();
    // Overwrite loaded private key so it doesn't linger in memory
    CryptSetDefaultPrivateKey();
  }
  /* -- Initialise cryptographic systems ----------------------------------- */
  void CryptInit()
  { // Set address of global class
    cCrypt = this;
    // Use sql to allocate memory?
    if(!CRYPTO_set_mem_functions(CryptAlloc, CryptReAlloc, CryptFree))
      XC("Failed to setup allocator for crypto interface!");
    // Generate CRC table (for lzma lib)
    CrcGenerateTable();
    // Init openSSL
    OPENSSL_init();
    // Class initialised
    IHInitialise();
    // Loop until...
    do
    { // Get some random entropy from the system hardware
      const Memory mData{ cSystem->GetEntropy() };
      // Set seed from system class to opensl
      RAND_seed(mData.MemPtr<void>(), mData.MemSize<int>());
      // Make a simple request to initialise more entropy
      CryptRandom<int>();
    } // ...PRNG is ready
    while(!RAND_status());
  }
  /* ----------------------------------------------------------------------- */
  static void *CryptAlloc(size_t stSize, const char*const, const int)
    { return StdAlloc<void>(stSize); }
  /* ----------------------------------------------------------------------- */
  static void *CryptReAlloc(void*const vpPtr, size_t stSize,
    const char*const, const int)
      { return StdReAlloc(vpPtr, stSize); }
  /* ----------------------------------------------------------------------- */
  static void CryptFree(void*const vpPtr, const char*const, const int)
    { StdFree(vpPtr); }
  /* -- Private keys ----------------------------------------------- */ public:
  static constexpr const size_t
    stPkKeyCount   = 4,                // Number of quads in key (256bits)
    stPkIvCount    = 2,                // Number of quads in iv key (128bits)
    stPkTotalCount = (stPkKeyCount + stPkIvCount);
  /* ----------------------------------------------------------------------- */
  typedef array<uint64_t, stPkKeyCount>   QPKey;
  typedef array<uint64_t, stPkIvCount>    QIVKey;
  typedef array<uint64_t, stPkTotalCount> QKeys;
  /* ----------------------------------------------------------------------- */
  union PrivateKey                     // Private key data
  { // --------------------------------------------------------------------- */
    struct Parts                       // Parts of the private key
    { // ------------------------------------------------------------------- */
      QPKey        qpkData;            // Private key (256bits)
      QIVKey       qivData;            // IV key (128bits)
      // ------------------------------------------------------------------ */
    } p;                               // Access to important parts
    // --------------------------------------------------------------------- */
    QKeys          qkData;             // Access to all parts of key
    // --------------------------------------------------------------------- */
  } pkDKey, pkKey;                     // Default and loaded private key data
  /* -- Set a new private key ---------------------------------------------- */
  void CryptResetPrivateKey()
    { CryptRandomPtr(&pkKey.qkData, sizeof(pkKey.qkData)); }
  /* -- Update private key mainly for use with protected cvars ------------- */
  void CryptWritePrivateKey(const size_t stId, const uint64_t ullVal)
    { pkKey.qkData[stId] = ullVal; }
  /* -- Set default private key -------------------------------------------- */
  void CryptSetDefaultPrivateKey() { pkKey = pkDKey; }
  /* -- Read part of the private key --------------------------------------- */
  uint64_t CryptReadPrivateKey(const size_t stId)
    { return pkKey.qkData[stId]; }
  /* -- Decode XML/HTML entities from a string ----------------------------- */
  const string CryptEntDecode(string strS)
  { // Done if empty
    if(strS.empty()) return strS;
    // Loop until we don't find anymore entities to decode
    for(size_t stAPos = strS.find('&');
               stAPos != StdNPos;
               stAPos = strS.find('&', stAPos))
    { // Get semi-colon position, break if not found
      const size_t stSPos = strS.find(';', stAPos + 1);
      if(stSPos == StdNPos) break;
      // Copy out the entity
      const string strT{ strS.substr(stAPos + 1, stSPos - stAPos - 1) };
      // Cut out the entity
      strS.erase(stAPos, stSPos - stAPos + 1);
      // Is a unicode number?
      if(strT.front() == '#')
      { // Don't have at least two characters? Ignore, goto next entity
        if(strT.length() <= 1) continue;
        // Value to convert
        Codepoint cVal;
        // Have more than 2 characters and hex character specified?
        if(strT.length() > 2 && (strT[1] == 'x' || strT[1] == 'X'))
          cVal = StrHexToInt<unsigned int>(strT.substr(2, StdNPos));
        // Not hex but is a number? Normal number
        else if(StdIsDigit(strT[1]))
          cVal = StrToNum<unsigned int>(strT.substr(1, StdNPos));
        // Shouldn't be anything else. Ignore insertations, goto next entity
        else continue;
        // Encoder character
        const UtfEncoderEx ueeCode{ UtfEncodeEx(cVal) };
        // Insert utf-8 string
        strS.insert(stAPos, ueeCode.u.c);
        // Go forward the number of unicode bytes written
        stAPos += ueeCode.l;
        // Find another entity
        continue;
      } // Find string to decode, ignore further insertation if no match
      const StrVStrVMapConstIt svsvmciIt{ svsvmEnt.find(strT) };
      if(svsvmciIt == svsvmEnt.cend()) continue;
      // Insert result
      strS.insert(stAPos, svsvmciIt->second);
      // Go forward
      stAPos += svsvmciIt->second.length();
    } // Return string
    return strS;
  }
  /* -- Default constructor ------------------------------------- */ protected:
  Crypt() :                            // No arguments
    /* -- Initialisers ----------------------------------------------------- */
    InitHelper{ __FUNCTION__ },        // Initialise init helper
    svsvmEnt{                          // Define HTML entities
      { "AElig", "\xC3\x86" }, { "Aacute", "\xC3\x81" },
      { "Acirc", "\xC3\x82" }, { "Agrave", "\xC3\x80" },
      { "Aring", "\xC3\x85" }, { "Atilde", "\xC3\x83" },
      { "Auml", "\xC3\x84" }, { "Ccedil", "\xC3\x87" },
      { "Dagger", "\xE2\x80\xA1" }, { "ETH", "\xC3\x90" },
      { "Eacute", "\xC3\x89" }, { "Ecirc", "\xC3\x8A" },
      { "Egrave", "\xC3\x88" }, { "Euml", "\xC3\x8B" },
      { "Iacute", "\xC3\x8D" }, { "Icirc", "\xC3\x8E" },
      { "Igrave", "\xC3\x8C" }, { "Iuml", "\xC3\x8F" },
      { "Ntilde", "\xC3\x91" }, { "OElig", "\xC5\x92" },
      { "Oacute", "\xC3\x93" }, { "Ocirc", "\xC3\x94" },
      { "Ograve", "\xC3\x92" }, { "Oslash", "\xC3\x98" },
      { "Otilde", "\xC3\x95" }, { "Ouml", "\xC3\x96" },
      { "Prime", "\xE2\x80\xB3" }, { "Scaron", "\xC5\xA0" },
      { "THORN", "\xC3\x9E" }, { "Uacute", "\xC3\x9A" },
      { "Ucirc", "\xC3\x9B" }, { "Ugrave", "\xC3\x99" },
      { "Uuml", "\xC3\x9C" }, { "Yacute", "\xC3\x9D" }, { "Yuml", "\xC5\xB8" },
      { "aacute", "\xC3\xA1" }, { "acirc", "\xC3\xA2" },
      { "acute", "\xC2\xB4" }, { "aelig", "\xC3\xA6" },
      { "agrave", "\xC3\xA0" }, { "alefsym", "\xE2\x84\xB5" },
      { "alpha", "\xCE\xB1" }, { "amp", "&" }, { "and", "\xE2\x88\xA7" },
      { "ang", "\xE2\x88\xA0" }, { "apos", "'" }, { "aring", "\xC3\xA5" },
      { "asymp", "\xE2\x89\x88" }, { "atilde", "\xC3\xA3" },
      { "auml", "\xC3\xA4" }, { "bdquo", "\xE2\x80\x9E" },
      { "beta", "\xCE\xB2" }, { "brvbar", "\xC2\xA6" },
      { "bull", "\xE2\x80\xA2" }, { "cap", "\xE2\x88\xA9" },
      { "ccedil", "\xC3\xA7" }, { "cedil", "\xC2\xB8" },
      { "cent", "\xC2\xA2" }, { "chi", "\xCF\x87" }, { "circ", "\xCB\x86" },
      { "clubs", "\xE2\x99\xA3" }, { "cong", "\xE2\x89\xA5" },
      { "copy", "\xC2\xA9" }, { "crarr", "\xE2\x86\xB5" },
      { "cup", "\xE2\x88\xAA" }, { "curren", "\xC2\xA4" },
      { "dArr", "\xE2\x87\x93" }, { "dagger", "\xE2\x80\xA0" },
      { "darr", "\xE2\x86\x93" }, { "deg", "\xC2\xB0" },
      { "delta", "\xCE\xB4" }, { "diams", "\xE2\x99\xA6" },
      { "divide", "\xC3\xB7" }, { "eacute", "\xC3\xA9" },
      { "ecirc", "\xC3\xAA" }, { "egrave", "\xC3\xA8" },
      { "empty", "\xE2\x88\x85" }, { "emsp", "\xE2\x80\x83" },
      { "ensp", "\xE2\x80\x82" }, { "epsilon", "\xCE\xB5" },
      { "equiv", "\xE2\x89\xA1" }, { "eta", "\xCE\xB7" },
      { "eth", "\xC3\xB0" }, { "euml", "\xC3\xAB" },
      { "euro", "\xE2\x82\xAC" }, { "exist", "\xE2\x88\x83" },
      { "fnof", "\xC6\x92" }, { "forall", "\xE2\x88\x80" },
      { "frac12", "\xC2\xBD" }, { "frac14", "\xC2\xBC" },
      { "frac34", "\xC2\xBE" }, { "frasl", "\xE2\x81\x84" },
      { "gamma", "\xCE\xB3" }, { "ge", "\xE2\x89\xA5" }, { "gt", ">" },
      { "hArr", "\xE2\x87\x94" }, { "hairsp", "\xE2\x80\x8A" },
      { "harr", "\xE2\x86\x94" }, { "hearts", "\xE2\x99\xA5" },
      { "hellip", "\xE2\x80\xA6" }, { "iacute", "\xC3\xAD" },
      { "icirc", "\xC3\xAE" }, { "iexcl", "\xC2\xA1" },
      { "igrave", "\xC3\xAC" }, { "image", "\xE2\x84\x91" },
      { "infin", "\xE2\x88\x9E" }, { "int", "\xE2\x88\xAB" },
      { "iota", "\xCE\xB9" }, { "iquest", "\xC2\xBF" },
      { "isin", "\xE2\x88\x88" }, { "iuml", "\xC3\xAF" },
      { "kappa", "\xCE\xBA" }, { "lArr", "\xE2\x87\x90" },
      { "lambda", "\xCE\xBB" }, { "lang", "\xE2\x8C\xA9" },
      { "laquo", "\xC2\xAB" }, { "larr", "\xE2\x86\x90" },
      { "lceil", "\xE2\x8C\x88" }, { "ldquo", "\xE2\x80\x9C" },
      { "le", "\xE2\x89\xA4" }, { "lfloor", "\xE2\x8C\x8A" },
      { "lowast", "\xE2\x88\x97" }, { "loz", "\xE2\x97\x8A" },
      { "lrm", "\xE2\x80\x8E" }, { "lsaquo", "\xE2\x80\xB9" },
      { "lsquo", "\xE2\x80\x98" }, { "lt", "<" }, { "macr", "\xC2\xAF" },
      { "mdash", "\xE2\x80\x94" }, { "micro", "\xC2\xB5" },
      { "middot", "\xC2\xB7" }, { "minus", "\xE2\x88\x92" },
      { "mu", "\xCE\xBC" }, { "nabla", "\xE2\x88\x87" },
      { "nbsp", "\xC2\xA0" }, { "nbspace", "\xC2\xA0" },
      { "ndash", "\xE2\x80\x93" }, { "ne", "\xE2\x89\xA0" },
      { "ni", "\xE2\x88\x8B" }, { "not", "\xC2\xAC" },
      { "notin", "\xE2\x88\x89" }, { "nsub", "\xE2\x8A\x84" },
      { "ntilde", "\xC3\xB1" }, { "nu", "\xCE\xBD" }, { "oacute", "\xC3\xB3" },
      { "ocirc", "\xC3\xB4" }, { "oelig", "\xC5\x93" },
      { "ograve", "\xC3\xB2" }, { "oline", "\xE2\x80\xBE" },
      { "omega", "\xCF\x89" }, { "omicron", "\xCE\xBF" },
      { "oplus", "\xE2\x8A\x95" }, { "or", "\xE2\x88\xA8" },
      { "ordf", "\xC2\xAA" }, { "ordm", "\xC2\xBA" }, { "oslash", "\xC3\xB8" },
      { "otilde", "\xC3\xB5" }, { "otimes", "\xE2\x8A\x97" },
      { "ouml", "\xC3\xB6" }, { "para", "\xC2\xB6" },
      { "part", "\xE2\x88\x82" }, { "permil", "\xE2\x80\xB0" },
      { "perp", "\xE2\x8A\xA5" }, { "phi", "\xCF\x86" }, { "pi", "\xCF\x80" },
      { "piv", "\xCF\xB6" }, { "plusmn", "\xC2\xB1" }, { "pound", "\xC2\xA3" },
      { "prime", "\xE2\x80\xB2" }, { "prod", "\xE2\x88\x8F" },
      { "prop", "\xE2\x88\x9D" }, { "psi", "\xCF\x88" }, { "quot", "\"" },
      { "rArr", "\xE2\x87\x92" }, { "radic", "\xE2\x88\x9A" },
      { "rang", "\xE2\x8C\xAA" }, { "raquo", "\xC2\xBB" },
      { "rarr", "\xE2\x86\x92" }, { "rceil", "\xE2\x8C\x89" },
      { "rdquo", "\xE2\x80\x9D" }, { "real", "\xE2\x84\x9C" },
      { "reg", "\xC2\xAE" }, { "rfloor", "\xE2\x8C\x8B" },
      { "rho", "\xCF\x81" }, { "rlm", "\xE2\x80\x8F" },
      { "rsaquo", "\xE2\x80\xBA" }, { "rsquo", "\xE2\x80\x99" },
      { "sbquo", "\xE2\x80\x9A" }, { "scaron", "\xC5\xA1" },
      { "sdot", "\xE2\x8B\x85" }, { "sect", "\xC2\xA7" },
      { "shy", "\xC2\xAD" }, { "sigma", "\xCF\x83" }, { "sigmaf", "\xCF\x82" },
      { "sim", "\xE2\x88\xBC" }, { "spades", "\xE2\x99\xA0" },
      { "sub", "\xE2\x8A\x82" }, { "sube", "\xE2\x8A\x86" },
      { "sum", "\xE2\x88\x91" }, { "sup", "\xE2\x8A\x83" },
      { "sup1", "\xC2\xB9" }, { "sup2", "\xC2\xB2" }, { "sup3", "\xC2\xB3" },
      { "supe", "\xE2\x8A\x87" }, { "szlig", "\xC3\x9F" },
      { "tau", "\xCF\x84" }, { "there4", "\xE2\x88\xB4" },
      { "theta", "\xCE\xB8" }, { "thetasym","\xCF\xB1" },
      { "thinsp", "\xE2\x80\x89" }, { "thorn", "\xC3\xBE" },
      { "tilde", "\xCB\x9C" }, { "times", "\xC3\x97" },
      { "trade", "\xE2\x84\xA2" }, { "uArr", "\xE2\x87\x91" },
      { "uacute", "\xC3\xBA" }, { "uarr", "\xE2\x86\x91" },
      { "ucirc", "\xC3\xBB" }, { "ugrave", "\xC3\xB9" },
      { "uml", "\xC2\xA8" }, { "upsih", "\xCF\xB2" },
      { "upsilon", "\xCF\x85" }, { "uuml", "\xC3\xBC" },
      { "weierp", "\xE2\x84\x98" }, { "xi", "\xCE\xBE" },
      { "yacute", "\xC3\xBD" }, { "yen", "\xC2\xA5" }, { "yuml", "\xC3\xBF" },
      { "zeta", "\xCE\xB6" }, { "zwj", "\xE2\x80\x8D" },
      { "zwnj", "\xE2\x80\x8C" }, { "zwsp", "\xE2\x80\x8B" },
    },
    pkDKey{{{ 0x9F7C1E39CA3935CA,      // Default private key (1/4) 256-bits
              0x71F2A9630EF11A98,      // Default private key (2/4)
              0xA2AB924A293A01FB,      // Default private key (3/4)
              0x2BA92A197F3AD1A9 },    // Default private key (4/4)
            { 0x109CF37A284B8910,      // Default IV key (1/2) 128-bits
              0x89FE280958CFD102 }}},  // Default IV key (2/2)
    pkKey(pkDKey)                      // Modified private key
    /* -- Initialise cryptography ------------------------------------------ */
    { CryptInit(); }
  /* -- Destructor --------------------------------------------------------- */
  DTORHELPER(~Crypt, CryptDeInit())
};/* ----------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
