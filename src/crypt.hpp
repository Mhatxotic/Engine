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
  CryptHexDecodePtr(strSrc.c_str(), strSrc.length(), mbDst.MemPtr<char>());
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
  CryptHexDecodePtr(strSrc.c_str(), strSrc.length(),
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
static void CryptAddEntropy(void)
{ // Grab some data from the system
  CryptAddEntropyInt(cmSys.GetTimeNS());
  CryptAddEntropyInt(cmHiRes.GetTimeNS());
  CryptAddEntropyInt(cLog->CCDeltaUS());
  cSystem->UpdateCPUUsage();
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
template<typename AnyType>static const AnyType CryptRandom(void)
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
  const char *cpPtr = strS.c_str();
  // Preallocate string to avoid multiple reallocations. Worst case: every char
  // needs encoding.
  string strURL; strURL.reserve(strS.size() * 3);
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
  static const string CryptImplodeMapAndEncode(const MapType &mtRef,
    const string &strSep)
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
static string CryptGetError(void)
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
      reinterpret_cast<void(*)(void)>(fCB))); }
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
  for(UtfDecoder utfSrc{ strS }; unsigned int uiChar = utfSrc.Next();)
  { // Characters not these character ranges will be encoded
    if(uiChar <  ' '                   || // Control characters
      (uiChar >= '!' && uiChar <= '/') || // Symbols before numbers
      (uiChar >= ':' && uiChar <= '@') || // Symbols before capitals
       uiChar >= '{')                     // Extended characters
      osS << cCommon->CommonEnt() << uiChar << ';';
    // Character is usable as is
    else osS << static_cast<char>(uiChar);
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
static Memory CryptHMACCall(const EVP_MD*const fFunc,
  const void         *const vpSalt, const size_t stSaltSize,
  const unsigned char*const cpSrc,  const size_t stSrcSize)
{ // Check sizes to make sure they can be converted to integer
  if(UtilIntWillOverflow<int>(stSaltSize))
    XC("Size of salt data too big!",
       "Requested", stSaltSize, "Maximum", numeric_limits<int>::max());
  if(UtilIntWillOverflow<int>(stSrcSize))
    XC("Size of source data to hash too big!",
       "Requested", stSrcSize,  "Maximum", numeric_limits<int>::max());
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
  string strURL; strURL.reserve(strS.size());
  // Movable pointer to input string and perform actions for each character...
  for(const char *cpPtr = strS.c_str(); *cpPtr;)
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
  string strPruned;
  strPruned.reserve(strMessage.capacity());
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
  string strOutput; strOutput.reserve(strPruned.size());
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
  void CryptDeInit(void)
  { // Ignore if not initialised
    if(IHNotDeInitialise()) return;
    // De-initialise openssl
    OPENSSL_cleanup();
    // Overwrite loaded private key so it doesn't linger in memory
    SetDefaultPrivateKey();
  }
  /* -- Initialise cryptographic systems ----------------------------------- */
  void CryptInit(void)
  { // Set address of global class
    cCrypt = this;
    // Use sql to allocate memory?
    if(!CRYPTO_set_mem_functions(OSSLAlloc, OSSLReAlloc, OSSLFree))
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
  static void *OSSLAlloc(size_t stSize, const char*const, const int)
    { return StdAlloc<void>(stSize); }
  /* ----------------------------------------------------------------------- */
  static void *OSSLReAlloc(void*const vpPtr, size_t stSize,
    const char*const, const int)
      { return StdReAlloc(vpPtr, stSize); }
  /* ----------------------------------------------------------------------- */
  static void OSSLFree(void*const vpPtr, const char*const, const int)
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
      QPKey        qKey;               // Private key (256bits)
      QIVKey       qIV;                // IV key (128bits)
      // ------------------------------------------------------------------ */
    } p;                               // Access to important parts
    // --------------------------------------------------------------------- */
    QKeys          qKeys;              // Access to all parts of key
    // --------------------------------------------------------------------- */
  } pkDKey, pkKey;                     // Default and loaded private key data
  /* -- Set a new private key ---------------------------------------------- */
  void ResetPrivateKey(void)
    { CryptRandomPtr(&pkKey.qKeys, sizeof(pkKey.qKeys)); }
  /* -- Update private key mainly for use with protected cvars ------------- */
  void WritePrivateKey(const size_t stId, const uint64_t qVal)
    { pkKey.qKeys[stId] = qVal; }
  /* -- Set default private key -------------------------------------------- */
  void SetDefaultPrivateKey(void) { pkKey = pkDKey; }
  /* -- Read part of the private key --------------------------------------- */
  uint64_t ReadPrivateKey(const size_t stId) { return pkKey.qKeys[stId]; }
  /* -- Iterator is the last entitiy? -------------------------------------- */
  bool IsLastEntity(const StrVStrVMapConstIt &svsvmciIt)
    { return svsvmciIt == svsvmEnt.cend(); }
  /* -- Find entity in the entity list ------------------------------------- */
  const StrVStrVMapConstIt FindEntity(const string &strWhat)
    { return svsvmEnt.find(strWhat); }
  /* -- Encode XML/HTML entities into string ------------------------------- */
  const string EntDecode(string strS)
  { // Done if empty
    if(strS.empty()) return {};
    // Loop until we don't find anymore entities to decode
    for(size_t stAPos = strS.find('&');
               stAPos != StdNPos;
               stAPos = strS.find('&', stAPos))
    { // Get semi-colon position, break if not found
      const size_t stSPos = strS.find(';', stAPos+1);
      if(stSPos == StdNPos) break;
      // Copy out the entity
      const string strT{ strS.substr(stAPos+1, stSPos-stAPos-1) };
      // Cut out the entity
      strS.erase(stAPos, stSPos-stAPos+1);
      // Is a unicode number?
      if(strT.front() == '#')
      { // Don't have at least two characters? Ignore, goto next entity
        if(strT.length() <= 1) continue;
        // Value to convert
        unsigned int uiVal;
        // Have more than 2 characters and hex character specified?
        if(strT.length() > 2 && (strT[1] == 'x' || strT[1] == 'X'))
          uiVal = StrHexToInt<unsigned int>
            (strT.substr(2, StdNPos));
        // Not hex but is a number? Normal number
        else if(StdIsDigit(strT[1]))
          uiVal = StrToNum<unsigned int>(strT.substr(1, StdNPos));
        // Shouldn't be anything else. Ignore insertations, goto next entity
        else continue;
        // Encoder character
        const UtfEncoderEx utfCode{ UtfEncodeEx(uiVal) };
        // Insert utf-8 string
        strS.insert(stAPos, utfCode.u.c);
        // Go forward the number of unicode bytes written
        stAPos += utfCode.l;
        // Find another entity
        continue;
      } // Find string to decode, ignore further insertation if no match
      const StrVStrVMapConstIt svsvmciIt{ FindEntity(strT) };
      if(IsLastEntity(svsvmciIt)) continue;
      // Insert result
      strS.insert(stAPos, svsvmciIt->second);
      // Go forward
      stAPos += svsvmciIt->second.length();
    } // Return string
    return strS;
  }
  /* -- Destructor ---------------------------------------------- */ protected:
  DTORHELPER(~Crypt, CryptDeInit())
  /* -- Default constructor ------------------------------------------------ */
  Crypt(void) :                        // No arguments
    /* -- Initialisers ----------------------------------------------------- */
    InitHelper{ __FUNCTION__ },        // Initialise init helper
    svsvmEnt{                          // Define HTML entities
      { "Agrave", "\xC0" }, { "Aacute", "\xC1" }, { "Acirc", "\xC2" },
      { "Atilde", "\xC3" }, { "Auml", "\xC4" }, { "Aring", "\xC5" },
      { "AElig", "\xC6" }, { "Ccedil", "\xC7" }, { "Egrave", "\xC8" },
      { "Eacute", "\xC9" }, { "Ecirc", "\xCA" }, { "Euml", "\xCB" },
      { "Igrave", "\xCC" }, { "Iacute", "\xCD" }, { "Icirc", "\xCE" },
      { "Iuml", "\xCF" }, { "ETH", "\xD0" }, { "Ntilde", "\xD1" },
      { "Ograve", "\xD2" }, { "Oacute", "\xD3" }, { "Ocirc", "\xD4" },
      { "Otilde", "\xD5" }, { "Ouml", "\xD6" }, { "Oslash", "\xD8" },
      { "Ugrave", "\xD9" }, { "Uacute", "\xDA" }, { "Ucirc", "\xDB" },
      { "Uuml", "\xDC" }, { "Yacute", "\xDD" }, { "THORN", "\xDE" },
      { "aacute", "\xE1" }, { "acirc", "\xE2" }, { "acute", "\xB4" },
      { "aelig", "\xE6" }, { "agrave", "\xE0" },
      { "alefsym", cCommon->CommonBlank() }, { "alpha", "a" }, { "amp", "&" },
      { "and", cCommon->CommonBlank() }, { "ang", cCommon->CommonBlank() },
      { "apos", "'" }, { "aring", "\xE5" }, { "asymp", "\x98" },
      { "atilde", "\xE3" }, { "auml", "\xE4" }, { "bdquo", "\x84" },
      { "beta", "\xDF" }, { "brvbar", "\xA6" }, { "bull", "\x95" },
      { "cap", "n" }, { "ccedil", "\xE7" }, { "cedil", "\xB8" },
      { "cent", "\xA2" }, { "chi", "X" }, { "circ", "\x88" },
      { "clubs", cCommon->CommonBlank() }, { "cong", cCommon->CommonBlank() },
      { "copy", "\xA9" }, { "crarr", cCommon->CommonBlank() },
      { "cup", cCommon->CommonBlank() }, { "curren", "\xA4" },
      { "dArr", cCommon->CommonBlank() }, { "dagger", "\x86" },
      { "darr", cCommon->CommonBlank() }, { "deg", "\xB0" },
      { "delta", "d" }, { "diams", cCommon->CommonBlank() },
      { "divide", "\xF7" }, { "eacute", "\xE9" }, { "ecirc", "\xEA" },
      { "egrave", "\xE8" }, { "empty", "\xD8" },
      { "emsp", cCommon->CommonSpace() }, { "ensp", cCommon->CommonSpace() },
      { "epsilon", "e" }, { "equiv", cCommon->CommonEquals() },
      { "eta", cCommon->CommonBlank() }, { "eth", "\xF0" }, { "euml", "\xEB" },
      { "euro", "\x80" }, { "exist", cCommon->CommonBlank() },
      { "fnof", "\x83" }, { "forall", cCommon->CommonBlank() },
      { "frac12", "\xBD" }, { "frac14", "\xBC" }, { "frac34", "\xBE" },
      { "frasl", cCommon->CommonFSlash() },
      { "gamma", cCommon->CommonBlank() }, { "ge", cCommon->CommonEquals() },
      { "gt", ">" }, { "hArr", cCommon->CommonBlank() },
      { "harr", cCommon->CommonBlank() }, { "hearts", cCommon->CommonBlank() },
      { "hellip", "\x85" }, { "iacute", "\xED" }, { "icirc", "\xEE" },
      { "iexcl", "\xA1" }, { "igrave", "\xEC" }, { "image", "I" },
      { "infin", "8" }, { "int", cCommon->CommonBlank() },
      { "iota", cCommon->CommonBlank() }, { "iquest", "\xBF" },
      { "isin", cCommon->CommonBlank() }, { "iuml", "\xEF" },
      { "kappa", cCommon->CommonBlank() }, { "lArr", cCommon->CommonBlank() },
      { "lambda", cCommon->CommonBlank() }, { "lang", "<" },
      { "laquo", "\xAB" }, { "larr", cCommon->CommonBlank() },
      { "lceil", cCommon->CommonBlank() }, { "ldquo", "\x93" },
      { "le", cCommon->CommonEquals() }, { "lfloor", cCommon->CommonBlank() },
      { "lowast", "*" }, { "loz", cCommon->CommonBlank() },
      { "lrm", "\xE2\x80\x8E" }, { "lsaquo", "\x8B" }, { "lsquo", "\x91" },
      { "lt", "<" }, { "macr", "\xAF" }, { "mdash", "\x97" },
      { "micro", "\xB5" }, { "middot", "\xB7" }, { "minus", "-" },
      { "mu", "\xB5" }, { "nabla", cCommon->CommonBlank() },
      { "nbsp", cCommon->CommonSpace() }, { "ndash", "\x96" },
      { "ne", cCommon->CommonBlank() }, { "ni", cCommon->CommonBlank() },
      { "not", "\xAC" }, { "notin", cCommon->CommonBlank() },
      { "nsub", cCommon->CommonBlank() }, { "ntilde", "\xF1" },
      { "nu", cCommon->CommonBlank() }, { "oacute", "\xF3" },
      { "ocirc", "\xF4" }, { "oelig", "\x9C" }, { "ograve", "\xF2" },
      { "oline", cCommon->CommonBlank() }, { "omega", cCommon->CommonBlank() },
      { "omicron", cCommon->CommonBlank() },
      { "oplus", cCommon->CommonBlank() }, { "or", cCommon->CommonBlank() },
      { "ordf", "\xAA" }, { "ordm", "\xBA" }, { "oslash", "\xF8" },
      { "otilde", "\xF5" }, { "otimes", cCommon->CommonBlank() },
      { "ouml", "\xF6" }, { "para", "\xB6" },
      { "part", cCommon->CommonBlank() }, { "permil", "\x89" },
      { "perp", cCommon->CommonBlank() }, { "phi", "f" }, { "pi", "p" },
      { "piv", cCommon->CommonBlank() }, { "plusmn", "\xB1" },
      { "pound", "\xA3" }, { "prime", "'" },
      { "prod", cCommon->CommonBlank() }, { "prop", cCommon->CommonBlank() },
      { "psi", cCommon->CommonBlank() }, { "quot", "\"" },
      { "rArr", cCommon->CommonBlank() }, { "radic", "v" }, { "rang", ">" },
      { "raquo", "\xBB" }, { "rarr", cCommon->CommonBlank() },
      { "rceil", cCommon->CommonBlank() }, { "rdquo", "\x94" },
      { "real", "R" }, { "reg", "\xAE" }, { "rfloor", cCommon->CommonBlank() },
      { "rho", cCommon->CommonBlank() }, { "rlm", "\xE2\x80\x8F" },
      { "rsaquo", "\x9B" }, { "rsquo", "\x92" }, { "sbquo", "\x82" },
      { "scaron", "\x9A" }, { "sdot", "\xB7" }, { "sect", "\xA7" },
      { "shy", "\xC2\xAD" }, { "sigma", "s" },
      { "sigmaf", cCommon->CommonBlank() }, { "sim", "~" },
      { "spades", cCommon->CommonBlank() }, { "sub", cCommon->CommonBlank() },
      { "sube", cCommon->CommonBlank() }, { "sum", cCommon->CommonBlank() },
      { "sup", cCommon->CommonBlank() }, { "sup1", "\xB9" },
      { "sup2", "\xB2" }, { "sup3", "\xB3" },
      { "supe", cCommon->CommonBlank() }, { "szlig", "\xDF" }, { "tau", "t" },
      { "there4", cCommon->CommonBlank() },
      { "theta", cCommon->CommonBlank() },
      { "thetasym",cCommon->CommonBlank() },
      { "thinsp", cCommon->CommonBlank() }, { "thorn", "\xFE" },
      { "tilde", "\x98" }, { "times", "\xD7" }, { "trade", "\x99" },
      { "uArr", cCommon->CommonBlank() }, { "uacute", "\xFA" },
      { "uarr", cCommon->CommonBlank() }, { "ucirc", "\xFB" },
      { "ugrave", "\xF9" }, { "uml", "\xA8" },
      { "upsih", cCommon->CommonBlank() }, { "upsilon", "Y" },
      { "uuml", "\xFC" }, { "weierp", "P" }, { "xi", cCommon->CommonBlank() },
      { "yacute", "\xFD" }, { "yen", "\xA5" }, { "yuml", "\xFF" },
      { "zeta", "Z" }, { "zwj", "\xE2\x80\x8D" }, { "zwnj", "\xE2\x80\x8C" },
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
};/* ----------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
