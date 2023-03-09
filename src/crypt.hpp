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
using namespace IClock::P;             using namespace ICollector::P;
using namespace IError::P;             using namespace ILog::P;
using namespace IMemory::P;            using namespace IStd::P;
using namespace IString::P;            using namespace ISystem::P;
using namespace ISysUtil::P;           using namespace IToken::P;
using namespace IUtf;
using namespace IUtil::P;              using namespace Lib::OS::OpenSSL;
using namespace Lib::OS::SevenZip;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* -- Convert the specified character to hexadecimal ----------------------- */
static char CryptHex2Char(const uint8_t ucChar)
{ // Hex lookup table
  static const array<const char,256> caLookup{
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //   0 - 15
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //  16 - 31
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //  32 - 47
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 0, 0, 0, 0, 0, //  48 - 63   (48 = '0')
    0,10,11,12,13,14,15, 0, 0, 0, 0, 0, 0, 0, 0, 0, //  64 - 79   (65 = 'A')
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //  80 - 95
    0,10,11,12,13,14,15, 0, 0, 0, 0, 0, 0, 0, 0, 0, //  96 - 111  (97 = 'a')
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 112 - 127
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 128 - 143
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 144 - 159
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 160 - 175
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 176 - 191
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 192 - 207
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 208 - 223
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 224 - 239
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0  // 240 - 255
  }; // Return character
  return caLookup[ucChar];
}
/* -- Convert the specified hexadecimanl string to 8-bit array ------------- */
static void CryptHexDecodePtr(const char*const cpSrc, const size_t stSrcLen,
  char*const cpDst)
{ // Build 8-bit value from two ASCII characters
  for(size_t stInPos = 0, stOutPos = 0;
             stInPos < stSrcLen;
             stInPos += 2, ++stOutPos)
    cpDst[stOutPos] = static_cast<char>
      ((CryptHex2Char(static_cast<uint8_t>(cpSrc[stInPos])) << 4) +
        CryptHex2Char(static_cast<uint8_t>(cpSrc[stInPos + 1])));
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
static const auto &CryptChar2HexU(const uint8_t ucP)
{ // Lookup table for uppercase conversion. Don't know how to do this in C++ :(
  static const array<const array<const char,2>,256>cHexTabUC{
  { {'0','0'},{'0','1'},{'0','2'},{'0','3'},{'0','4'},{'0','5'},{'0','6'},
    {'0','7'},{'0','8'},{'0','9'},{'0','A'},{'0','B'},{'0','C'},{'0','D'},
    {'0','E'},{'0','F'},{'1','0'},{'1','1'},{'1','2'},{'1','3'},{'1','4'},
    {'1','5'},{'1','6'},{'1','7'},{'1','8'},{'1','9'},{'1','A'},{'1','B'},
    {'1','C'},{'1','D'},{'1','E'},{'1','F'},{'2','0'},{'2','1'},{'2','2'},
    {'2','3'},{'2','4'},{'2','5'},{'2','6'},{'2','7'},{'2','8'},{'2','9'},
    {'2','A'},{'2','B'},{'2','C'},{'2','D'},{'2','E'},{'2','F'},{'3','0'},
    {'3','1'},{'3','2'},{'3','3'},{'3','4'},{'3','5'},{'3','6'},{'3','7'},
    {'3','8'},{'3','9'},{'3','A'},{'3','B'},{'3','C'},{'3','D'},{'3','E'},
    {'3','F'},{'4','0'},{'4','1'},{'4','2'},{'4','3'},{'4','4'},{'4','5'},
    {'4','6'},{'4','7'},{'4','8'},{'4','9'},{'4','A'},{'4','B'},{'4','C'},
    {'4','D'},{'4','E'},{'4','F'},{'5','0'},{'5','1'},{'5','2'},{'5','3'},
    {'5','4'},{'5','5'},{'5','6'},{'5','7'},{'5','8'},{'5','9'},{'5','A'},
    {'5','B'},{'5','C'},{'5','D'},{'5','E'},{'5','F'},{'6','0'},{'6','1'},
    {'6','2'},{'6','3'},{'6','4'},{'6','5'},{'6','6'},{'6','7'},{'6','8'},
    {'6','9'},{'6','A'},{'6','B'},{'6','C'},{'6','D'},{'6','E'},{'6','F'},
    {'7','0'},{'7','1'},{'7','2'},{'7','3'},{'7','4'},{'7','5'},{'7','6'},
    {'7','7'},{'7','8'},{'7','9'},{'7','A'},{'7','B'},{'7','C'},{'7','D'},
    {'7','E'},{'7','F'},{'8','0'},{'8','1'},{'8','2'},{'8','3'},{'8','4'},
    {'8','5'},{'8','6'},{'8','7'},{'8','8'},{'8','9'},{'8','A'},{'8','B'},
    {'8','C'},{'8','D'},{'8','E'},{'8','F'},{'9','0'},{'9','1'},{'9','2'},
    {'9','3'},{'9','4'},{'9','5'},{'9','6'},{'9','7'},{'9','8'},{'9','9'},
    {'9','A'},{'9','B'},{'9','C'},{'9','D'},{'9','E'},{'9','F'},{'A','0'},
    {'A','1'},{'A','2'},{'A','3'},{'A','4'},{'A','5'},{'A','6'},{'A','7'},
    {'A','8'},{'A','9'},{'A','A'},{'A','B'},{'A','C'},{'A','D'},{'A','E'},
    {'A','F'},{'B','0'},{'B','1'},{'B','2'},{'B','3'},{'B','4'},{'B','5'},
    {'B','6'},{'B','7'},{'B','8'},{'B','9'},{'B','A'},{'B','B'},{'B','C'},
    {'B','D'},{'B','E'},{'B','F'},{'C','0'},{'C','1'},{'C','2'},{'C','3'},
    {'C','4'},{'C','5'},{'C','6'},{'C','7'},{'C','8'},{'C','9'},{'C','A'},
    {'C','B'},{'C','C'},{'C','D'},{'C','E'},{'C','F'},{'D','0'},{'D','1'},
    {'D','2'},{'D','3'},{'D','4'},{'D','5'},{'D','6'},{'D','7'},{'D','8'},
    {'D','9'},{'D','A'},{'D','B'},{'D','C'},{'D','D'},{'D','E'},{'D','F'},
    {'E','0'},{'E','1'},{'E','2'},{'E','3'},{'E','4'},{'E','5'},{'E','6'},
    {'E','7'},{'E','8'},{'E','9'},{'E','A'},{'E','B'},{'E','C'},{'E','D'},
    {'E','E'},{'E','F'},{'F','0'},{'F','1'},{'F','2'},{'F','3'},{'F','4'},
    {'F','5'},{'F','6'},{'F','7'},{'F','8'},{'F','9'},{'F','A'},{'F','B'},
    {'F','C'},{'F','D'},{'F','E'},{'F','F'} }
  };
  // Return the string. Remember there is no null terminator
  return cHexTabUC[ucP];
}
/* -- Convert the specified 8-bit array to a hexadecmial string (upcase) --- */
static const string CryptBin2Hex(const uint8_t*const ucStr,
  const size_t stSize)
{ // The output string and we know what the size of the output will be so
  // we do not need to use an ostringstream object.
  string strOut; strOut.reserve(stSize * 2);
  // For each character in the string convert it to hex and push it onto
  // the string. The CHAR2HEX function returns only 2 bytes, so we must make
  // sure STL only processes 2 bytes!
  for(const uint8_t *ucPtr = const_cast<uint8_t*>(ucStr),
              *const ucPtrEnd = ucPtr + stSize;
                     ucPtr < ucPtrEnd;
                   ++ucPtr)
    strOut.append(CryptChar2HexU(*ucPtr).data(), 2);
  // We're done. return the string!
  return strOut;
}
static const string CryptBin2Hex(const MemConst &mcSrc)
  { return CryptBin2Hex(mcSrc.MemPtr<uint8_t>(), mcSrc.MemSize()); }
/* -- Convert the specified 8-bit char to a lowercase hex string ----------- */
static const auto &CryptChar2HexL(const uint8_t ucP)
{ // Lookup table for lowercase conversion
  static const array<const array<const char,2>,256>cHexTabLC{
  { {'0','0'},{'0','1'},{'0','2'},{'0','3'},{'0','4'},{'0','5'},{'0','6'},
    {'0','7'},{'0','8'},{'0','9'},{'0','a'},{'0','b'},{'0','c'},{'0','d'},
    {'0','e'},{'0','f'},{'1','0'},{'1','1'},{'1','2'},{'1','3'},{'1','4'},
    {'1','5'},{'1','6'},{'1','7'},{'1','8'},{'1','9'},{'1','a'},{'1','b'},
    {'1','c'},{'1','d'},{'1','e'},{'1','f'},{'2','0'},{'2','1'},{'2','2'},
    {'2','3'},{'2','4'},{'2','5'},{'2','6'},{'2','7'},{'2','8'},{'2','9'},
    {'2','a'},{'2','b'},{'2','c'},{'2','d'},{'2','e'},{'2','f'},{'3','0'},
    {'3','1'},{'3','2'},{'3','3'},{'3','4'},{'3','5'},{'3','6'},{'3','7'},
    {'3','8'},{'3','9'},{'3','a'},{'3','b'},{'3','c'},{'3','d'},{'3','e'},
    {'3','f'},{'4','0'},{'4','1'},{'4','2'},{'4','3'},{'4','4'},{'4','5'},
    {'4','6'},{'4','7'},{'4','8'},{'4','9'},{'4','a'},{'4','b'},{'4','c'},
    {'4','d'},{'4','e'},{'4','f'},{'5','0'},{'5','1'},{'5','2'},{'5','3'},
    {'5','4'},{'5','5'},{'5','6'},{'5','7'},{'5','8'},{'5','9'},{'5','a'},
    {'5','b'},{'5','c'},{'5','d'},{'5','e'},{'5','f'},{'6','0'},{'6','1'},
    {'6','2'},{'6','3'},{'6','4'},{'6','5'},{'6','6'},{'6','7'},{'6','8'},
    {'6','9'},{'6','a'},{'6','b'},{'6','c'},{'6','d'},{'6','e'},{'6','f'},
    {'7','0'},{'7','1'},{'7','2'},{'7','3'},{'7','4'},{'7','5'},{'7','6'},
    {'7','7'},{'7','8'},{'7','9'},{'7','a'},{'7','b'},{'7','c'},{'7','d'},
    {'7','e'},{'7','f'},{'8','0'},{'8','1'},{'8','2'},{'8','3'},{'8','4'},
    {'8','5'},{'8','6'},{'8','7'},{'8','8'},{'8','9'},{'8','a'},{'8','b'},
    {'8','c'},{'8','d'},{'8','e'},{'8','f'},{'9','0'},{'9','1'},{'9','2'},
    {'9','3'},{'9','4'},{'9','5'},{'9','6'},{'9','7'},{'9','8'},{'9','9'},
    {'9','a'},{'9','b'},{'9','c'},{'9','d'},{'9','e'},{'9','f'},{'a','0'},
    {'a','1'},{'a','2'},{'a','3'},{'a','4'},{'a','5'},{'a','6'},{'a','7'},
    {'a','8'},{'a','9'},{'a','a'},{'a','b'},{'a','c'},{'a','d'},{'a','e'},
    {'a','f'},{'b','0'},{'b','1'},{'b','2'},{'b','3'},{'b','4'},{'b','5'},
    {'b','6'},{'b','7'},{'b','8'},{'b','9'},{'b','a'},{'b','b'},{'b','c'},
    {'b','d'},{'b','e'},{'b','f'},{'c','0'},{'c','1'},{'c','2'},{'c','3'},
    {'c','4'},{'c','5'},{'c','6'},{'c','7'},{'c','8'},{'c','9'},{'c','a'},
    {'c','b'},{'c','c'},{'c','d'},{'c','e'},{'c','f'},{'d','0'},{'d','1'},
    {'d','2'},{'d','3'},{'d','4'},{'d','5'},{'d','6'},{'d','7'},{'d','8'},
    {'d','9'},{'d','a'},{'d','b'},{'d','c'},{'d','d'},{'d','e'},{'d','f'},
    {'e','0'},{'e','1'},{'e','2'},{'e','3'},{'e','4'},{'e','5'},{'e','6'},
    {'e','7'},{'e','8'},{'e','9'},{'e','a'},{'e','b'},{'e','c'},{'e','d'},
    {'e','e'},{'e','f'},{'f','0'},{'f','1'},{'f','2'},{'f','3'},{'f','4'},
    {'f','5'},{'f','6'},{'f','7'},{'f','8'},{'f','9'},{'f','a'},{'f','b'},
    {'f','c'},{'f','d'},{'f','e'},{'f','f'} }
  };
  // Return the string. Remember there is no null terminator
  return cHexTabLC[ucP];
}
/* -- Convert the specified 8-bit array to a hexadecmial string (lwcase) --- */
static const string CryptBin2HexL(const uint8_t*const ucStr,
  const size_t stSize)
{ // The output string and we know what the size of the output will be so we
  // don't need to use an ostringstream object.
  string strOut; strOut.reserve(stSize * 2);
  // For each character in the string convert it to hex and push it onto
  // the string. The CHAR2HEX function returns only 2 bytes, so we must make
  // sure STL only processes 2 bytes!
  for(const uint8_t *ucPtr = const_cast<uint8_t*>(ucStr),
                    *const ucPtrEnd = ucPtr + stSize;
                     ucPtr < ucPtrEnd;
                   ++ucPtr)
    strOut.append(CryptChar2HexL(*ucPtr).data(), 2);
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
static const string CryptURLEncode(const char*const cpURL)
{ // Bail if passed string is invalid
  if(UtfIsCStringNotValid(cpURL)) return {};
  // We will use a ostringstream to build this as we do not know what the
  // size of the output will be.
  ostringstream osS;
  // Movable pointer to input string
  const char *cpPtr = cpURL;
  // Perform these action for each character...
  do
  { // Get character
    const uint8_t ucC = static_cast<uint8_t>(*cpPtr);
    // Normal character? Append to string
    if(isalnum(ucC)) osS << static_cast<char>(ucC);
    // Test individual character
    else switch(ucC)
    { // A character we don't need to encode? Just add them 'as-is'.
      case '-': case '.':
      case '_': case '~': osS << static_cast<char>(ucC); break;
      // Any other character? Encode it!
      default: osS << '%' << CryptChar2HexU(ucC)[0]
                          << CryptChar2HexU(ucC)[1]; break;
    } // ...until null terminator
  } while(*(++cpPtr));
  // End of string so return it
  return osS.str();
}
/* -- URL encode the specified string -------------------------------------- */
static const string CryptURLEncode(const string &strS)
  { return CryptURLEncode(strS.c_str()); }
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
      if(stColon != string::npos)
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
    if(uiChar <= 31                  || // Control characters
      (uiChar >= 33 && uiChar <= 47) || // Symbols before numbers
      (uiChar >= 58 && uiChar <= 64) || // Symbols before capitals
       uiChar >= 123)                   // Extended characters
      osS << "&#x" << uiChar << ';';
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
    static Memory HashPtrRaw(const unsigned char*const ucpIn, \
      const size_t stLen) \
        { Memory mbOut{ s }; \
          x(ucpIn, stLen, mbOut.MemPtr<unsigned char>()); \
          return mbOut; } \
    static const string HashPtr(const unsigned char*const ucpIn, \
      const size_t stLen) \
        { return CryptBin2Hex(StdMove(HashPtrRaw(ucpIn, stLen))); } \
    static const string HashStr(const string &strIn) \
      { return HashPtr(reinterpret_cast<const unsigned char*>(strIn.data()), \
          strIn.length()); } \
    static const string HashMB(const MemConst &mcSrc) \
      { return HashPtr(mcSrc.MemPtr<unsigned char>(), mcSrc.MemSize()); } \
    static Memory HashPtrRaw(const void*const vpSalt, const size_t stSaltSize,\
      const unsigned char*const cpDest, const size_t stDestSize) \
        { return CryptHMACCall(f(), vpSalt, stSaltSize, cpDest, stDestSize); }\
    static Memory HashStrRaw(const string &strKey, const string &strIn) \
      { return HashPtrRaw( \
          reinterpret_cast<const void*>(strKey.data()), \
          strKey.size(), \
          reinterpret_cast<const unsigned char*>(strIn.data()),\
          strIn.size()); } \
    static const string HashStr(const string &strKey, const string &strIn) \
      { return CryptBin2Hex(StdMove(HashStrRaw(strKey, strIn))); } \
  };
DEFINE_HASH_FUNCS(SHA1,   SHA_DIGEST_LENGTH,    EVP_sha1);   // Insecure
DEFINE_HASH_FUNCS(SHA224, SHA224_DIGEST_LENGTH, EVP_sha224); // Secure
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
static const string CryptURLDecode(const char*const cpURL)
{ // Bail if passed string is invalid
  if(UtfIsCStringNotValid(cpURL)) return {};
  // We will use a ostringstream to build this as we do not know what the
  // size of the output will be.
  ostringstream osS;
  // Movable pointer to input string
  const char *cpPtr = cpURL;
  // Perform these action for each character...
  do
  { // Get character and compare it
    switch(unsigned char ucC = static_cast<unsigned char>(*cpPtr))
    { // % symbol denoting a custom value
      case '%':
        // Convert hex to number and if not at end of string?
        if(unsigned char uc1 = static_cast<unsigned char>(*(++cpPtr)))
        { // Goto next character if not a valid digit
          if(!isxdigit(uc1)) continue;
          // Get second hex character and if not at end of string?
          if(unsigned char uc2 = static_cast<unsigned char>(*(++cpPtr)))
          { // Goto next character if not a valid digit
            if(!isxdigit(uc2)) continue;
            // Is lowecase 'a'
            if(uc1 >= 'a') uc1 -= 'a'-'A';
            uc1 -= uc1 >= 'A' ? ('A' - 10) : '0';
            if(uc2 >= 'a') uc2 -= 'a'-'A';
            uc2 -= uc2 >= 'A' ? ('A' - 10) : '0';
            // Set new character and fall through to default
            ucC = 16 * uc1 + uc2;
          } // End of string so return it
          else return osS.str();
        } // End of string so return it
        else return osS.str();
        // Fall through
        [[fallthrough]];
      // Normal character? Push normal character
      default: osS << ucC; break;
    } // ...until null terminator
  } while(*(++cpPtr));
  // End of string so return it
  return osS.str();
}
/* ------------------------------------------------------------------------- */
static const Memory CryptRandomBlock(const size_t stSize)
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
  string strOutput;
  strOutput.reserve(strMessage.capacity());
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
      strOutput.push_back(cChar);
      // Next character
      continue;
    } // Increase repetition count and continue if over 2 repetitions
    if(++stCount > 1) continue;
    // Add character
    strOutput.push_back(cChar);
  } // Now split the string into words
  Token tWords{ strOutput, cCommon->Space() };
  // Return if empty?
  if(tWords.empty()) return {};
  // return single word?
  if(tWords.size() == 1) return tWords.front();
  // Clear output and recover memory now we've done with it
  strOutput.clear();
  strOutput.shrink_to_fit();
  // Repeated words
  string strWord;
  // For each word
  for(StrVectorIt svciIt{ tWords.begin() }; svciIt != tWords.end(); )
  { // Character is different?
    if(*svciIt != strWord)
    { // Set new character found and reset times found
      strWord = *svciIt;
      stCount = 0;
      // Next word
      continue;
    } // Increase repetition count and continue if repeated
    if(++stCount < 2) { ++svciIt; continue; }
    // Remove word
    svciIt = tWords.erase(svciIt);
  } // Implode words and return output
  return StrImplode(tWords);
}
/* -- Crypt manager class -------------------------------------------------- */
static class Crypt final :
  /* -- Base classes ------------------------------------------------------- */
  public IHelper                       // The crypto manager class
{ /* -------------------------------------------------------------- */ private:
  const StrVStrVMap svsvmEnt;          // Html entity decoding lookup table
  /* ----------------------------------------------------------------------- */
  static void *OSSLAlloc(size_t stSize, const char*const, const int)
    { return UtilMemAlloc<void>(stSize); }
  /* ----------------------------------------------------------------------- */
  static void *OSSLReAlloc(void*const vpPtr, size_t stSize,
    const char*const, const int)
      { return UtilMemReAlloc(vpPtr, stSize); }
  /* ----------------------------------------------------------------------- */
  static void OSSLFree(void*const vpPtr, const char*const, const int)
    { UtilMemFree(vpPtr); }
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
               stAPos != string::npos;
               stAPos = strS.find('&', stAPos))
    { // Get semi-colon position, break if not found
      const size_t stSPos = strS.find(';', stAPos+1);
      if(stSPos == string::npos) break;
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
            (strT.substr(2, string::npos));
        // Not hex but is a number? Normal number
        else if(isdigit(static_cast<unsigned char>(strT[1])))
          uiVal = StrToNum<unsigned int>
            (strT.substr(1, string::npos));
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
  /* -- Constructor -------------------------------------------------------- */
  Crypt(void) :
    /* -- Initialisers ----------------------------------------------------- */
    IHelper{ __FUNCTION__ },
    svsvmEnt{
      { "Agrave",  "\xC0"           }, { "Aacute",  "\xC1"           },
      { "Acirc",   "\xC2"           }, { "Atilde",  "\xC3"           },
      { "Auml",    "\xC4"           }, { "Aring",   "\xC5"           },
      { "AElig",   "\xC6"           }, { "Ccedil",  "\xC7"           },
      { "Egrave",  "\xC8"           }, { "Eacute",  "\xC9"           },
      { "Ecirc",   "\xCA"           }, { "Euml",    "\xCB"           },
      { "Igrave",  "\xCC"           }, { "Iacute",  "\xCD"           },
      { "Icirc",   "\xCE"           }, { "Iuml",    "\xCF"           },
      { "ETH",     "\xD0"           }, { "Ntilde",  "\xD1"           },
      { "Ograve",  "\xD2"           }, { "Oacute",  "\xD3"           },
      { "Ocirc",   "\xD4"           }, { "Otilde",  "\xD5"           },
      { "Ouml",    "\xD6"           }, { "Oslash",  "\xD8"           },
      { "Ugrave",  "\xD9"           }, { "Uacute",  "\xDA"           },
      { "Ucirc",   "\xDB"           }, { "Uuml",    "\xDC"           },
      { "Yacute",  "\xDD"           }, { "THORN",   "\xDE"           },
      { "aacute",  "\xE1"           }, { "acirc",   "\xE2"           },
      { "acute",   "\xB4"           }, { "aelig",   "\xE6"           },
      { "agrave",  "\xE0"           }, { "alefsym", cCommon->Blank() },
      { "alpha",   "a"              }, { "amp",     "&"              },
      { "and",     cCommon->Blank() }, { "ang",     cCommon->Blank() },
      { "apos",    "'"              }, { "aring",   "\xE5"           },
      { "asymp",   "\x98"           }, { "atilde",  "\xE3"           },
      { "auml",    "\xE4"           }, { "bdquo",   "\x84"           },
      { "beta",    "\xDF"           }, { "brvbar",  "\xA6"           },
      { "bull",    "\x95"           }, { "cap",     "n"              },
      { "ccedil",  "\xE7"           }, { "cedil",   "\xB8"           },
      { "cent",    "\xA2"           }, { "chi",     "X"              },
      { "circ",    "\x88"           }, { "clubs",   cCommon->Blank() },
      { "cong",    cCommon->Blank() }, { "copy",    "\xA9"           },
      { "crarr",   cCommon->Blank() }, { "cup",     cCommon->Blank() },
      { "curren",  "\xA4"           }, { "dArr",    cCommon->Blank() },
      { "dagger",  "\x86"           }, { "darr",    cCommon->Blank() },
      { "deg",     "\xB0"           }, { "delta",   "d"              },
      { "diams",   cCommon->Blank() }, { "divide",  "\xF7"           },
      { "eacute",  "\xE9"           }, { "ecirc",   "\xEA"           },
      { "egrave",  "\xE8"           }, { "empty",   "\xD8"           },
      { "emsp",    cCommon->Space() }, { "ensp",    cCommon->Space() },
      { "epsilon", "e"              }, { "equiv",   cCommon->Equals()},
      { "eta",     cCommon->Blank() }, { "eth",     "\xF0"           },
      { "euml",    "\xEB"           }, { "euro",    "\x80"           },
      { "exist",   cCommon->Blank() }, { "fnof",    "\x83"           },
      { "forall",  cCommon->Blank() }, { "frac12",  "\xBD"           },
      { "frac14",  "\xBC"           }, { "frac34",  "\xBE"           },
      { "frasl",   cCommon->FSlash()}, { "gamma",   cCommon->Blank() },
      { "ge",      cCommon->Equals()}, { "gt",      ">"              },
      { "hArr",    cCommon->Blank() }, { "harr",    cCommon->Blank() },
      { "hearts",  cCommon->Blank() }, { "hellip",  "\x85"           },
      { "iacute",  "\xED"           }, { "icirc",   "\xEE"           },
      { "iexcl",   "\xA1"           }, { "igrave",  "\xEC"           },
      { "image",   "I"              }, { "infin",   "8"              },
      { "int",     cCommon->Blank() }, { "iota",    cCommon->Blank() },
      { "iquest",  "\xBF"           }, { "isin",    cCommon->Blank() },
      { "iuml",    "\xEF"           }, { "kappa",   cCommon->Blank() },
      { "lArr",    cCommon->Blank() }, { "lambda",  cCommon->Blank() },
      { "lang",    "<"              }, { "laquo",   "\xAB"           },
      { "larr",    cCommon->Blank() }, { "lceil",   cCommon->Blank() },
      { "ldquo",   "\x93"           }, { "le",      cCommon->Equals()},
      { "lfloor",  cCommon->Blank() }, { "lowast",  "*"              },
      { "loz",     cCommon->Blank() }, { "lrm",     "\xE2\x80\x8E"   },
      { "lsaquo",  "\x8B"           }, { "lsquo",   "\x91"           },
      { "lt",      "<"              }, { "macr",    "\xAF"           },
      { "mdash",   "\x97"           }, { "micro",   "\xB5"           },
      { "middot",  "\xB7"           }, { "minus",   "-"              },
      { "mu",      "\xB5"           }, { "nabla",   cCommon->Blank() },
      { "nbsp",    cCommon->Space() }, { "ndash",   "\x96"           },
      { "ne",      cCommon->Blank() }, { "ni",      cCommon->Blank() },
      { "not",     "\xAC"           }, { "notin",   cCommon->Blank() },
      { "nsub",    cCommon->Blank() }, { "ntilde",  "\xF1"           },
      { "nu",      cCommon->Blank() }, { "oacute",  "\xF3"           },
      { "ocirc",   "\xF4"           }, { "oelig",   "\x9C"           },
      { "ograve",  "\xF2"           }, { "oline",   cCommon->Blank() },
      { "omega",   cCommon->Blank() }, { "omicron", cCommon->Blank() },
      { "oplus",   cCommon->Blank() }, { "or",      cCommon->Blank() },
      { "ordf",    "\xAA"           }, { "ordm",    "\xBA"           },
      { "oslash",  "\xF8"           }, { "otilde",  "\xF5"           },
      { "otimes",  cCommon->Blank() }, { "ouml",    "\xF6"           },
      { "para",    "\xB6"           }, { "part",    cCommon->Blank() },
      { "permil",  "\x89"           }, { "perp",    cCommon->Blank() },
      { "phi",     "f"              }, { "pi",      "p"              },
      { "piv",     cCommon->Blank() }, { "plusmn",  "\xB1"           },
      { "pound",   "\xA3"           }, { "prime",   "'"              },
      { "prod",    cCommon->Blank() }, { "prop",    cCommon->Blank() },
      { "psi",     cCommon->Blank() }, { "quot",    "\""             },
      { "rArr",    cCommon->Blank() }, { "radic",   "v"              },
      { "rang",    ">"              }, { "raquo",   "\xBB"           },
      { "rarr",    cCommon->Blank() }, { "rceil",   cCommon->Blank() },
      { "rdquo",   "\x94"           }, { "real",    "R"              },
      { "reg",     "\xAE"           }, { "rfloor",  cCommon->Blank() },
      { "rho",     cCommon->Blank() }, { "rlm",     "\xE2\x80\x8F"   },
      { "rsaquo",  "\x9B"           }, { "rsquo",   "\x92"           },
      { "sbquo",   "\x82"           }, { "scaron",  "\x9A"           },
      { "sdot",    "\xB7"           }, { "sect",    "\xA7"           },
      { "shy",     "\xC2\xAD"       }, { "sigma",   "s"              },
      { "sigmaf",  cCommon->Blank() }, { "sim",     "~"              },
      { "spades",  cCommon->Blank() }, { "sub",     cCommon->Blank() },
      { "sube",    cCommon->Blank() }, { "sum",     cCommon->Blank() },
      { "sup",     cCommon->Blank() }, { "sup1",    "\xB9"           },
      { "sup2",    "\xB2"           }, { "sup3",    "\xB3"           },
      { "supe",    cCommon->Blank() }, { "szlig",   "\xDF"           },
      { "tau",     "t"              }, { "there4",  cCommon->Blank() },
      { "theta",   cCommon->Blank() }, { "thetasym",cCommon->Blank() },
      { "thinsp",  cCommon->Blank() }, { "thorn",   "\xFE"           },
      { "tilde",   "\x98"           }, { "times",   "\xD7"           },
      { "trade",   "\x99"           }, { "uArr",    cCommon->Blank() },
      { "uacute",  "\xFA"           }, { "uarr",    cCommon->Blank() },
      { "ucirc",   "\xFB"           }, { "ugrave",  "\xF9"           },
      { "uml",     "\xA8"           }, { "upsih",   cCommon->Blank() },
      { "upsilon", "Y"              }, { "uuml",    "\xFC"           },
      { "weierp",  "P"              }, { "xi",      cCommon->Blank() },
      { "yacute",  "\xFD"           }, { "yen",     "\xA5"           },
      { "yuml",    "\xFF"           }, { "zeta",    "Z"              },
      { "zwj",     "\xE2\x80\x8D"   }, { "zwnj",    "\xE2\x80\x8C"   },
    },                                 // Default private
    pkDKey{ { { 0x9F7C1E39CA3935CA, 0x71F2A9630EF11A98,
                0xA2AB924A293A01FB, 0x2BA92A197F3AD1A9 },
              { 0x109CF37A284B8910, 0x89FE280958CFD102 } } },
    pkKey(pkDKey)                      // and IV key
    /* -- Code ------------------------------------------------------------- */
    { // Use sql to allocate memory?
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
  /* -- Destructor --------------------------------------------------------- */
  DTORHELPERBEGIN(~Crypt)
  // Ignore if not initialised
  if(IHNotDeInitialise()) return;
  // De-initialise openssl
  OPENSSL_cleanup();
  // Overwrite loaded private key so it doesn't linger in memory
  SetDefaultPrivateKey();
  // Done
  DTORHELPEREND(~Crypt)
  /* -- Macros ---------------------------------------------------- */ private:
  DELETECOPYCTORS(Crypt)               // Suppress default functions for safety
  /* ----------------------------------------------------------------------- */
} *cCrypt = nullptr;                   // Pointer to static class
/* -- URL decode the specified string -------------------------------------- */
static const string CryptURLDecode(const string &strS)
  { return CryptURLDecode(strS.c_str()); }
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
