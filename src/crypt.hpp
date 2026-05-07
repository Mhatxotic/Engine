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
template<int iFailCode>
  static int CryptHex2Char(unsigned char ucChar)
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
/* -- Convert the specified hexadecimanl string to 8-bit array (thick) ----- */
static Memory CryptHexDecodeASafe(const char*const cpPtr, const size_t stSize)
{ // Must not be empty and a multiple of two to comply
  if(!stSize || stSize % 2) return {};
  // The memory to output. We know what size will be
  Memory mbDst{ stSize / 2 };
  // Build 8-bit value from two ASCII characters
  CryptHexDecodePtr(cpPtr, stSize, mbDst.MemPtr<char>());
  // Return memory
  return mbDst;
}
/* -- Convert the specified hex string to 8-bit array (thin) --------------- */
template<class StrType>
  requires StdIsString<StrType>
static Memory CryptHexDecodeA(StrType &&strStr)
  { return CryptHexDecodeASafe(strStr.data(), strStr.size()); }
/* -- Convert the specified hexadecimanl string to string (thick) ---------- */
static StdString CryptHexDecodeStrSafe(const char*const cpPtr,
  const size_t stSize)
{ // Must not be empty and a multiple of two to comply
  if(!stSize || stSize % 2) return {};
  // The memory to output. We know what size will be
  StdResized<StdString> strDst{ stSize / 2 };
  // Build 8-bit value from two ASCII characters
  CryptHexDecodePtr(cpPtr, stSize, StdToNonConstCast<char*>(strDst.data()));
  // Return string
  return strDst;
}
/* -- Convert the specified hex string to string (thin) -------------------- */
template<class StrType>
  requires StdIsString<StrType>
static StdString CryptHexDecodeStr(StrType &&strStr)
  { return CryptHexDecodeStrSafe(strStr.data(), strStr.size()); }
/* -- Convert the specified 8-bit char to a uppercase hex string ----------- */
static void CryptChar2HexU(const uint8_t ucChar, char*const cpPtr)
{ // Hex lookup table
  static const char caHex[17] = "0123456789ABCDEF";
  // Decode to buffer
  cpPtr[0] = caHex[(ucChar >> 4) & 0x0F]; // High nibble
  cpPtr[1] = caHex[ucChar & 0x0F];        // Low nibble
}
/* -- Convert the specified 8-bit array to a hexadecmial string (upcase) --- */
static StdString CryptBin2Hex(const uint8_t*const ucStr, const size_t stSize)
{ // The output string and we know what the size of the output will be so
  // we do not need to use an ostringstream object.
  StdResized<StdString> strOut{ stSize * 2 };
  // Cast to a char so theres no warnings and process the buffer
  char *cpBuffer = &strOut[0];
  for(size_t stPos = 0; stPos < stSize; ++stPos)
    CryptChar2HexU(ucStr[stPos], &cpBuffer[stPos * 2]);
  // We're done. return the string!
  return strOut;
}
static StdString CryptBin2Hex(const MemConst &mcSrc)
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
static StdString CryptBin2HexL(const uint8_t*const ucStr, const size_t stSize)
{ // The output string and we know what the size of the output will be so
  // we do not need to use an ostringstream object.
  StdResized<StdString> strOut{ stSize * 2 };
  // Cast to a char so theres no warnings and process the buffer
  char *cpBuffer = strOut.data();
  for(size_t stPos = 0; stPos < stSize; ++stPos)
    CryptChar2HexL(ucStr[stPos], &cpBuffer[stPos * 2]);
  // We're done. return the string!
  return strOut;
}
static StdString CryptBin2HexL(const MemConst &mcSrc)
  { return CryptBin2HexL(mcSrc.MemPtr<uint8_t>(), mcSrc.MemSize()); }
/* ------------------------------------------------------------------------- */
static void CryptAddEntropyPtr(const void*const vpPtr, const size_t stSize)
  { RAND_add(vpPtr, UtilIntOrMax<int>(stSize), static_cast<double>(stSize)); }
/* ------------------------------------------------------------------------- */
template<typename IntType>
  static void CryptAddEntropyInt(const IntType itValue)
    { CryptAddEntropyPtr(&itValue, sizeof(itValue)); }
/* ------------------------------------------------------------------------- */
template<class StrType>
  requires StdIsString<StrType>
static void CryptAddEntropyStr(StrType &&strStr)
  { CryptAddEntropyPtr(strStr.data(), strStr.capacity()); }
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
template<typename AnyType>
  static AnyType CryptRandom()
{ // Do the randomisation into the requested type and return it
  AnyType atData;
  CryptRandomPtr(&atData, sizeof(atData));
  return atData;
}
/* -- URL encode the specified c-string ------------------------------------ */
template<class StrType>
  static StdString CryptURLEncode(StrType &&strStr)
{ // Normalise to StringView if not a string
  using StdNormString = StdNormalisedString<StrType>;
  StdNormString snsStr{ StdForward<StrType>(strStr) };
  using StdNormStringConstIt =
    typename StdDecay<StdNormString>::const_iterator;
  // Bail if passed string is empty
  if(snsStr.empty()) return {};
  // Movable pointer to input string
  StdNormStringConstIt snsciIt{ snsStr.cbegin() };
  // Preallocate string to avoid multiple reallocations. Worst case: every char
  // needs encoding.
  StdReserved<StdString> strURL{ snsStr.size() * 3 };
  // Perform these actions for each character...
  do
  { // Get character
    const uint8_t ucC = static_cast<uint8_t>(*snsciIt);
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
  } while(++snsciIt != snsStr.cend());
  // Compact the URL
  strURL.shrink_to_fit();
  // End of string so return it
  return strURL;
}
/* -- Get error reason ----------------------------------------------------- */
static StdString CryptGetErrorReason(const unsigned long ulErr)
{ // Clear and resize error buffer to maximum
  StdResized<StdString> strError{ 128 };
  // Grab the error string from openssl and resize to correct size. Better to
  // use the non '_n' version so we can do all this in one line. Since
  // OpenSSL won't write more than 120 characters.
  strError.resize(static_cast<size_t>(
    strlen(ERR_error_string(ulErr, const_cast<char*>(strError.data())))));
  // Return error
  return strError;
}
/* -- Get error reason ----------------------------------------------------- */
static int CryptGetError(StdString &strError)
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
    constexpr static const unsigned
      // Replacement for ERR_SYSTEM_MASK which causes warnings
      uSystemMask = StdLimits<int>::max(),
      // Replacement for ERR_SYSTEM_FLAG which causes warnings
      uSystemFlag = uSystemMask + 1;
    // Is a system error?
    const bool bSysErr = ulErr & uSystemFlag;
    // Get library and reason...
    const unsigned
      // Replacement for ERR_GET_LIB in err.h which causes warnings
      uLib = bSysErr ?
        ERR_LIB_SYS : ((ulErr >> ERR_LIB_OFFSET) & ERR_LIB_MASK),
      // Replacement for ERR_GET_REASON in err.h which causes warning
      uReason = bSysErr ?
        (ulErr & uSystemFlag) : (ulErr & ERR_REASON_MASK);
    // If the operating system has the reason?
    if(uLib == ERR_LIB_SYS || uReason == ERR_R_SYS_LIB)
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
static StdString CryptGetError()
  { StdString strOut; CryptGetError(strOut); return strOut; }
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
      StdToNonConstCast<void*>(cpName))); }
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
  int(*const ftCb)(SSL*,void*))
{ return static_cast<int>(SSL_CTX_callback_ctrl(sslCtx,
    SSL_CTRL_SET_TLSEXT_STATUS_REQ_CB,
    reinterpret_cast<void(*)(void)>(reinterpret_cast<void*>(ftCb)))); }
/* -- Replacement for SSL_set_tlsext_host_name which causes warnings ------- */
static int CryptSSLSetTlsExtHostName(SSL*const sSSL, const char*const cpName)
  { return static_cast<int>(SSL_ctrl(sSSL, SSL_CTRL_SET_TLSEXT_HOSTNAME,
      TLSEXT_NAMETYPE_host_name, StdToNonConstCast<void*>(cpName))); }
/* -- Replacement for SSL_CTX_set1_verify_cert_store which causes warnings - */
static int CryptSSLCtxSet1VerifyCertStore(SSL_CTX*const sslCtx,
  X509_STORE*const x509dest)
{ return static_cast<int>(SSL_CTX_ctrl(sslCtx, SSL_CTRL_SET_VERIFY_CERT_STORE,
    1, reinterpret_cast<void*>(x509dest))); }
/* ------------------------------------------------------------------------- */
static StdString CryptPTRtoB64(const void*const vpIn, const size_t stIn)
{ // To clean up when leaving scope unexpectedliy
  using BioPtr = StdUniquePtr<BIO, function<decltype(BIO_free_all)>>;
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
  using BioPtr = StdUniquePtr<BIO, function<decltype(BIO_free_all)>>;
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
static StdString CryptMBtoB64(const MemConst &mcSrc)
  { return CryptPTRtoB64(mcSrc.MemPtr<void>(), mcSrc.MemSize()); }
/* ------------------------------------------------------------------------- */
template<class StrType>
  requires StdIsString<StrType>
static StdString CryptStoB64(StrType &&strStr)
  { return CryptPTRtoB64(StdToNonConstCast<void*>(strStr.data()),
      strStr.size()); }
/* ------------------------------------------------------------------------- */
template<class StrType>
  requires StdIsString<StrType>
static Memory CryptB64toMB(StrType &&strStr)
{ // Output buffer
  Memory mData{ strStr.size() };
  // Do conversion and resize string after
  mData.MemResize(CryptB64toPTR(StdToNonConstCast<void*>(strStr.data()),
    strStr.size(), mData.MemPtr(), mData.MemSize()));
  // Return data
  return mData;
}
/* ------------------------------------------------------------------------- */
template<class StrType>
  requires StdIsString<StrType>
static StdString CryptB64toS(StrType &&strStr)
{ // Create output buffer with enough size for output
  StdResized<StdString> strOut{ strStr.size() };
  // Do conversion and resize string after
  strOut.resize(CryptB64toPTR(StdToNonConstCast<void*>(strStr.data()),
    strStr.size(), StdToNonConstCast<void*>(strOut.data()),
    strOut.size()));
  // Return string
  return strOut;
}
/* ------------------------------------------------------------------------- */
template<class StrType>
  requires StdIsString<StrType>
static StdString CryptStoHEX(StrType &&strStr)
  { return CryptBin2Hex(reinterpret_cast<const uint8_t*>(strStr.data()),
      strStr.size()); }
/* ------------------------------------------------------------------------- */
template<class StrType>
  requires StdIsString<StrType>
static StdString CryptStoHEXL(StrType &&strStr)
  { return CryptBin2HexL(reinterpret_cast<const uint8_t*>(strStr.data()),
      strStr.size()); }
/* -- Encode XML/HTML entities into string (thick) ------------------------- */
static StdString CryptEntEncodePtr(const char*const cpPtr, const size_t stSize)
{ // Done if empty
  if(!stSize) return {};
  // Create sting to return and reserve memory. We will use a ostringstream
  // because we do not know what the size is going to be and we can make
  // use of hex which will work with our utf8 decoder.
  StdOStringStream osS; osS << StdIOSHex;
  // For each entity. Find it in the string
  // Until null character. Which control token?
  for(UtfDecoder udSrc{ cpPtr, stSize };
      const Codepoint cChar = udSrc.UtfNext();)
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
      osS << cCommon->CommonEntV() << cChar << ';';
    // Character is usable as is
    else osS << static_cast<char>(cChar);
  } // Return string
  return osS.str();
}
/* -- Encode XML/HTML entities into string (thin) -------------------------- */
template<class StrType>
  requires StdIsStrOrCStr<StrType>
static StdString CryptEntEncode(StrType &&strStr)
  { return StdNormaliseString<StrType>(StdForward<StrType>(strStr),
      [](auto &aStr){ return CryptEntEncodePtr(aStr.data(), aStr.size()); }); }
/* -- URL decode the specified normalised string (thick) ------------------- */
template<class StrType>
  static StdString CryptURLDecodeNormalisedString(StrType &&strStr)
{ // Bail if passed string is invalid
  if(strStr.empty()) return {};
  // Preallocate string to avoid multiple reallocations
  StdReserved<StdString> strURL{ strStr.size() };
  // Movable pointer to input string and perform actions for each character...
  using StrTypeConstIt = StdDecay<StrType>::const_iterator;
  for(StrTypeConstIt stciIt{ strStr.cbegin() };
                     stciIt != strStr.cend();)
  { // Get the character and if it denotes a encoded value?
    unsigned char ucC = static_cast<unsigned char>(*stciIt);
    if(ucC == '%')
    { // Convert hex to number and if not at end of string?
      if(const unsigned char uc1 = static_cast<unsigned char>(*(++stciIt)))
      { // Get the hexadecimal value and goto next character if not valid
        const int iVal1 = CryptHex2Char<-1>(uc1);
        if(iVal1 == -1) continue;
        // Get second hex character and if not at end of string?
        if(const unsigned char uc2 = static_cast<unsigned char>(*(++stciIt)))
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
    ++stciIt;
  } // Compact the URL
  strURL.shrink_to_fit();
  // Return the url
  return strURL;
}
/* -- URL decode the specified string (thin) ------------------------------- */
template<class StrType>
  static StdString CryptURLDecode(StrType &&strStr)
{ using StdNormString = StdNormalisedString<StrType>;
  StdNormString snsStr{ StdForward<StrType>(strStr) };
  return CryptURLDecodeNormalisedString<StdNormString>
           (StdForward<StdNormString>(snsStr)); }
/* -- Helper for mutliple hashing types from OpenSSL ----------------------- */
static Memory CryptHMACCall(const EVP_MD*const fFunc, const void*const vpSalt,
  const size_t stSaltSize, const unsigned char*const cpSrc,
  const size_t stSrcSize)
{ // Check sizes to make sure they can be converted to integer
  if(UtilIntWillOverflow<int>(stSaltSize))
    XC("Size of salt data too big!",
      "Requested", stSaltSize, "Maximum", StdLimits<int>::max());
  if(UtilIntWillOverflow<int>(stSrcSize))
    XC("Size of source data to hash too big!",
      "Requested", stSrcSize, "Maximum", StdLimits<int>::max());
  // Create output for HMAC-SHA hash
  Memory mData{ EVP_MAX_MD_SIZE };
  // For storage of size. We really need to know the output size.
  unsigned uLen = 0;
  // Get hash. Remember that HMAC returns as binary as we need to send the
  // whole buffer to Base64 and NOT a null-terminated string.
  if(!HMAC(fFunc, vpSalt, static_cast<int>(stSaltSize), cpSrc, stSrcSize,
    mData.MemPtr<unsigned char>(), &uLen))
      XC("Failed to perform salted-hash on source data!",
        "Function",    fFunc != nullptr,
        "SaltAddress", vpSalt != nullptr, "SaltSize", stSaltSize,
        "SrcAddress",  cpSrc != nullptr,  "SrcSize", stSrcSize,
        "OutLength",   uLen);
  // Resize string
  mData.MemResize(uLen);
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
    static StdString HSR(const unsigned char*const ucpIn, \
      const size_t stLen) \
        { return CryptBin2HexL(StdMove(HMR(ucpIn, stLen))); } \
    /* -- Hash string data and return string hash ------------------------- */\
    template<class StrType> \
      requires StdIsString<StrType> \
    static StdString HSS(StrType &&strStr) \
      { return HSR(reinterpret_cast<const unsigned char*>(strStr.data()), \
          strStr.size()); } \
    /* -- Hash string data and return raw hash ---------------------------- */\
    template<class StrType> \
      requires StdIsString<StrType> \
    static Memory HMS(StrType &&strStr) \
      { return HMR(reinterpret_cast<const unsigned char*>(strStr.data()), \
          strStr.size()); } \
    /* -- Hash raw data and return string hash ---------------------------- */\
    static StdString HSM(const MemConst &mcData) \
      { return HSR(mcData.MemPtr<unsigned char>(), mcData.MemSize()); } \
    /* -- Hash raw key ptr/size, raw data ptr/size and return raw hash ---- */\
    static Memory HMRR(const void*const vpSalt, const size_t stSaltSize,\
      const unsigned char*const cpDest, const size_t stDestSize) \
        { return CryptHMACCall(f(), vpSalt, stSaltSize, cpDest, stDestSize); }\
    /* -- Hash string key, string data and return raw hash ---------------- */\
    template<class StrSaltType, class StrValType> \
      requires StdIsString<StrSaltType> && StdIsString<StrValType> \
    static Memory HMSS(StrSaltType &&sstSalt, StrValType &&svtStr) \
    { return HMRR( \
        reinterpret_cast<const void*>(sstSalt.data()), sstSalt.size(), \
        reinterpret_cast<const unsigned char*>(svtStr.data()), \
          svtStr.size()); } \
    /* -- Hash string key, string data and return string hash ------------- */\
    template<class StrSaltType, class StrValType> \
      requires StdIsString<StrSaltType> && StdIsString<StrValType> \
    static StdString HSSS(StrSaltType &&sstSalt, StrValType &&svtStr) \
      { return CryptBin2HexL(StdMove(HMSS(sstSalt, svtStr))); } \
    /* -- Hash raw key, raw data and return raw hash ---------------------- */\
    static Memory HMMM(const MemConst &mcSalt, const MemConst &mcData) \
      { return HMRR(mcSalt.MemPtr<const void>(), mcSalt.MemSize(), \
                    mcData.MemPtr<unsigned char>(), mcData.MemSize()); } \
    /* -- Hash raw key, string data and return raw hash ------------------- */\
    template<class StrValType> \
      requires StdIsString<StrValType> \
    static Memory HMMS(const MemConst &mcSalt, StrValType &&svtStr) \
      { return HMRR(mcSalt.MemPtr<const void>(), \
                    mcSalt.MemSize(), \
                    reinterpret_cast<const unsigned char*>(svtStr.data()), \
                    svtStr.size()); } \
    /* -- Hash string key, raw data and return raw hash ------------------- */\
    template<class StrSaltType> \
      requires StdIsString<StrSaltType> \
    static Memory HMSM(StrSaltType &&sstSalt, const MemConst &mcData) \
      { return HMRR(reinterpret_cast<const void*>(sstSalt.data()), \
          sstSalt.size(), mcData.MemPtr<unsigned char>(), mcData.MemSize());}\
    /* -- Hash string key, string data and return string hash ------------- */\
    static StdString HSMM(const MemConst &mcSalt, const MemConst &mcData) \
      { return CryptBin2HexL(StdMove(HMMM(mcSalt, mcData))); } \
    /* -- Hash string key, raw data and return string hash ---------------- */\
    template<class StrValType> \
      requires StdIsString<StrValType> \
    static StdString HSMS(const MemConst &mcSalt, StrValType &&svtStr) \
      { return CryptBin2HexL(StdMove(HMMS(mcSalt, svtStr))); } \
    /* -- Hash raw key, string data and return string hash ---------------- */\
    template<class StrSaltType> \
      requires StdIsString<StrSaltType> \
    static StdString HSSM(StrSaltType &sstSalt, const MemConst &mcData) \
      { return CryptBin2HexL(StdMove(HMSM(sstSalt, mcData))); } \
  };/* --------------------------------------------------------------------- */
DEFINE_HASH_FUNCS(SHA1,   SHA_DIGEST_LENGTH,    EVP_sha1);   // Insecure
DEFINE_HASH_FUNCS(SHA224, SHA224_DIGEST_LENGTH, EVP_sha224); // Maybe secure
DEFINE_HASH_FUNCS(SHA256, SHA256_DIGEST_LENGTH, EVP_sha256); // Secure
DEFINE_HASH_FUNCS(SHA384, SHA384_DIGEST_LENGTH, EVP_sha384); // Very secure
DEFINE_HASH_FUNCS(SHA512, SHA512_DIGEST_LENGTH, EVP_sha512); // Really secure
#undef DEFINE_HASH_FUNCS
/* -- Create CRC32 hash of specified string using LZMA API ----------------- */
template<class StrType>
  requires StdIsString<StrType>
static unsigned CryptStrToCRC32(StrType &&strStr)
  { return CrcCalc(strStr.data(), strStr.size()); }
/* -- Create CRC32 hash of specified memory block using LZMA API ----------- */
static unsigned CryptMemToCRC32(const MemConst &mcSrc)
  { return CrcCalc(mcSrc.MemPtr<void>(), mcSrc.MemSize()); }
/* ------------------------------------------------------------------------- */
static Memory CryptRandomBlock(const size_t stSize)
{ // Allocate memory, fill it with random data and return it
  Memory mData{ stSize };
  CryptRandomPtr(mData.MemPtr(), mData.MemSize());
  return mData;
}
/* -- Crypt manager class -------------------------------------------------- */
class Crypt;                           // Class prototype
static Crypt *cCrypt = nullptr;        // Address of global class
class Crypt :                          // Actual class body
  /* -- Base classes ------------------------------------------------------- */
  public InitHelper                    // The crypto manager class
{ /* -- Private typedefs --------------------------------------------------- */
  MAPPACK_BUILD(CpCp, const Codepoint, const Codepoint)
  /* -- private variables -------------------------------------------------- */
  const StrVStrVMap svsvmEnt;          // Html entity decoding lookup table
  const CpCpMap    ccmLookalikeCodex;  // Homoglyph to ASCII char codex
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
  constexpr static const size_t
    stPkKeyCount   = 4,                // Number of quads in key (256bits)
    stPkIvCount    = 2,                // Number of quads in iv key (128bits)
    stPkTotalCount = (stPkKeyCount + stPkIvCount);
  /* ----------------------------------------------------------------------- */
  using QPKey  = StdArray<uint64_t, stPkKeyCount>;
  using QIVKey = StdArray<uint64_t, stPkIvCount>;
  using QKeys  = StdArray<uint64_t, stPkTotalCount>;
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
  /* -- Decode XML/HTML entities from a string (thick) --------------------- */
  StdString &CryptEntDecodeRef(StdString &strOut)
  { // Return original string if empty
    if(strOut.empty()) return strOut;
    // Loop until we don't find anymore entities to decode
    for(size_t stAPos = strOut.find('&');
               stAPos != StdNPos;
               stAPos = strOut.find('&', stAPos))
    { // Get semi-colon position, break if not found
      const size_t stSPos = strOut.find(';', stAPos + 1);
      if(stSPos == StdNPos) break;
      // Copy out the entity
      const StdString strEnt{ strOut.substr(stAPos + 1, stSPos - stAPos - 1) };
      // Cut out the entity
      strOut.erase(stAPos, stSPos - stAPos + 1);
      // Is a unicode number?
      if(strEnt.front() == '#')
      { // Don't have at least two characters? Ignore, goto next entity
        if(strEnt.size() <= 1) continue;
        // Value to convert
        Codepoint cVal;
        // Have more than 2 characters and hex character specified?
        if(strEnt.size() > 2 && (strEnt[1] == 'x' || strEnt[1] == 'X'))
          cVal = StrHexToInt<unsigned>(strEnt.substr(2, StdNPos));
        // Not hex but is a number? Normal number
        else if(StdIsDigit(strEnt[1]))
          cVal = StrToNum<unsigned>(strEnt.substr(1, StdNPos));
        // Shouldn't be anything else. Ignore insertations, goto next entity
        else continue;
        // Encoder character
        const UtfEncoderEx ueeCode{ UtfEncodeEx(cVal) };
        // Insert utf-8 string
        strOut.insert(stAPos, ueeCode.u.c);
        // Go forward the number of unicode bytes written
        stAPos += ueeCode.l;
        // Find another entity
        continue;
      } // Find string to decode, ignore further insertation if no match
      const StrVStrVMapConstIt svsvmciIt{ svsvmEnt.find(strEnt) };
      if(svsvmciIt == svsvmEnt.cend()) continue;
      // Insert result
      strOut.insert(stAPos, svsvmciIt->second);
      // Go forward
      stAPos += svsvmciIt->second.size();
    } // Return string
    return strOut;
  }
  /* -- Decode XML/HTML entities from a string (thin) ---------------------- */
  template<class StrType>
    requires StdIsStrOrCStr<StrType>
  StdString CryptEntDecode(StrType &&strStr)
    { StdString strOut{ strStr }; return CryptEntDecodeRef(strOut); }
  /* -- Sanitise a string removing excessive letters and words (thick) ----- */
  StdString CryptSanitiseStr(UtfDecoder &udStr) const
  { // Reserve memory for new output string
    StdReserved<StdString> strOut{ udStr.UtfSize() };
    // Last codepoint detected
    Codepoint coLast = 0;
    // Number of repeating codepoints detect
    unsigned uCount = 0;
    // Walk the string
    while(const Codepoint coChar = udStr.UtfNext())
    { // Find character in codex
      const CpCpMapConstIt ccmciIt{ ccmLookalikeCodex.find(coChar) };
      // If the character was found in the codex then use the codex value
      const Codepoint coNew =
        ccmciIt == ccmLookalikeCodex.cend() ? coChar : ccmciIt->second;
      // If this is not repeated character?
      if(coLast != coNew)
      { // Set new last character
        coLast = coNew;
        // Reset counter
        uCount = 0;
        // Append character
        UtfAppend(coNew, strOut);
      } // Increment repeat counter. Only two of the same character can exist
      else if(++uCount <= 2) UtfAppend(coNew, strOut);
    } // Trim string
    strOut.shrink_to_fit();
    // Implode words and return output
    return strOut;
  }
  /* -- Sanitise a string (thin) ------------------------------------------- */
  template<class StrType>
    requires StdIsStrOrCStr<StrType>
  StdString CryptSanitise(StrType &&strStr) const
    { UtfDecoder udStr{ strStr }; return CryptSanitiseStr(udStr); }
  /* -- Default constructor ------------------------------------- */ protected:
  Crypt() :                            // No arguments
    /* -- Initialisers ----------------------------------------------------- */
    InitHelper{ __FUNCTION__ },        // Initialise init helper
#include "cryptent.hpp"                // HTML entities codex
#include "cryptlac.hpp"                // UTF8 homoglyphs codex
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
