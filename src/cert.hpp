/* == CERT.HPP ============================================================= **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## Handles X509 certificate storage operations using OpenSSL.          ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace ICert {                      // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IAsset::P;             using namespace IClock::P;
using namespace ICVarDef::P;           using namespace IError::P;
using namespace IFileMap::P;           using namespace ILog::P;
using namespace IStd::P;               using namespace IString::P;
using namespace Lib::OS::OpenSSL;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* ------------------------------------------------------------------------- */
class Certs                            // Certificates store
{ /* -- Typedefs --------------------------------------------------- */ public:
  typedef pair<const string, X509*> X509Pair; // Do not &ref string
  typedef vector<X509Pair>          X509List; // A vector of pairs
  /* -- X509 error database --------------------------------------- */ private:
  struct X509ErrInfo                   // Information about the X509 error
  { /* --------------------------------------------------------------------- */
    const char*const cpErr;            // Error as a string (X509_V_ERR_*)
    const size_t     stBank;           // Override bank index (qCertBypass)
    const uint64_t   qFlag;            // Flag required to ignore this error
  };/* --------------------------------------------------------------------- */
  typedef map<const size_t, const X509ErrInfo> X509Err;
  typedef array<SafeUInt64, 2> SafeDoubleUInt64; // Two atomic double uint64's
  /* -- Class to serialise load of certificates ---------------------------- */
  struct LoadSerialised                // Serialised certificate installation
  { /* -- Lock/Unlock ------------------------------------------------------ */
    void LockFunction(void) { }        // Lock function not used
    void UnlockFunction(void) { }      // Unlock function not used
    /* -- Destructor/Constructor ------------------------------------------- */
    LoadSerialised(void) { }           // Constructor not used
    /* --------------------------------------------------------------------- */
    DELETECOPYCTORS(LoadSerialised)    // Suppress default functions for safety
  };/* -- Class to load certificates in parallel --------------------------- */
  class LoadParallel                   // Parallel certificate installation
  { /* --------------------------------------------------------------------- */
    bool           bLocked;            // Locked flag
    mutex          mMutex;             // Mutex
    /* -- Lock/Unlock ---------------------------------------------- */ public:
    void LockFunction(void) { mMutex.lock(); bLocked = true; }
    void UnlockFunction(void) { mMutex.unlock(); bLocked = false; }
    /* -- Destructor/Constructor ------------------------------------------- */
    ~LoadParallel(void) { if(bLocked) UnlockFunction(); }
    LoadParallel(void) : bLocked(false) { }
    /* --------------------------------------------------------------------- */
    DELETECOPYCTORS(LoadParallel)      // Suppress default functions for safety
  };/* -- Variables -------------------------------------------------------- */
  SSL_CTX           *scStore;          // Context used for cerificate store
  X509_STORE        *xsCerts;          // Certificate store inside OpenSSL
  X509List           lCAStore;         // Certificate store
  SafeDoubleUInt64   qCertBypass;      // Certificate bypass flags
  const string       strExtension;     // Default extension
  const X509Err      xErrDB;           // X509 error database
  /* -- Load certificate --------------------------------------------------- */
  template<class SyncMethod>void CertsLoad(SyncMethod &smClass,
    const string &strD, const string &strF) try
  { // Load the certificate
    const FileMap fmCert{
      AssetExtract(StrAppend(strD, cCommon->CFSlash(), strF)) };
    // Get pointer
    const unsigned char*ucpPtr = fmCert.MemPtr<unsigned char>();
    // Load the raw certificate and ig it succeeded?
    typedef unique_ptr<X509, function<decltype(X509_free)>> X509Ptr;
    if(X509Ptr caCert{
      d2i_X509(nullptr, &ucpPtr, fmCert.MemSize<long>()), X509_free })
    { // Get purpose struct of certificate
      if(X509_PURPOSE*const x509p = X509_PURPOSE_get0(X509_PURPOSE_SSL_SERVER))
      { // Get purpose id and reject if it is not a server CA certificate
        switch(X509_check_purpose(caCert.get(), X509_PURPOSE_get_id(x509p), 1))
        { // The certificate was created to perform the purpose represented
          case 1:
            // Valid server CA certificate so add to CA store and if succeeded?
            if(X509_STORE_add_cert(xsCerts, caCert.get()))
            { // Lock access to the list
              smClass.LockFunction();
              lCAStore.push_back({ fmCert.IdentGet(), caCert.get() });
              smClass.UnlockFunction();
            } // Failed to add certificate to CA store
            else cLog->LogWarningExSafe(
              "Certs failed to add '$' to SSL context!", fmCert.IdentGet());
            break;
          // The certificate was not created to perform the purpose represented
          case 0:
            cLog->LogWarningExSafe(
              "Certs rejected '$' as not a server CA certificate!",
              fmCert.IdentGet());
            break;
          // An error occured
          default:
            cLog->LogWarningExSafe(
              "Certs rejected '$' because an error occurred!",
              fmCert.IdentGet());
            break;
        }
      } // Failed to get purpose? Log the rejection
      else cLog->LogWarningExSafe(
        "Certs rejected '$' as unable to get purpose!", fmCert.IdentGet());
    } // Release the certificate (caCert)
  } // In the rare occurence that an exception occurs we should skip the cert
  catch(const exception &e)
  { // Show the exception and try the next certificate
    cLog->LogErrorExSafe(
      "Certs rejected certificate '$/$' due to exception: $",
      strD, strF, e.what());
  }
  /* -- Unload open ssl certificate store ---------------------------------- */
  void CertsUnload(void)
  { // Remove pointer to certificate store as it is above to become invalid
    if(xsCerts) xsCerts = nullptr;
    // Return if ssl store is not available
    if(!scStore) return;
    // Log that were unloading the certificate store
    cLog->LogDebugSafe("Certs unloading certificate store...");
    // Unload context that holds the certificate store
    SSL_CTX_free(scStore);
    scStore = nullptr;
    // Log that we unloaded the certificate store
    cLog->LogInfoSafe("Certs unloaded certificate store.");
  }
  /* -- Unload entire certificate store ------------------------------------ */
  void CertsEmpty(void)
  { // Unload the certificate store in OpenSSL
    CertsUnload();
    // Ignore if no ceritificates loaded
    if(lCAStore.empty()) return;
    // Now deleting certificate store
    cLog->LogDebugExSafe("Certs de-initialising $ certificates...",
      lCAStore.size());
    // Counters
    size_t stFreed = 0;
    // For each certificate
    while(!lCAStore.empty())
    { // Get last item
      const X509Pair &xpItem = lCAStore.back();
      // Have certificate? Free the cert and increment number
      if(xpItem.second) { X509_free(xpItem.second); ++stFreed; }
      // Remove the cached version
      lCAStore.pop_back();
    } // Free memory
    lCAStore.shrink_to_fit();
    // Finished
    cLog->LogInfoExSafe("Certs de-initialised $ certificates.", stFreed);
  }
  /* -- Load certificates from a list of files ----------------------------- */
  void CertsLoadList(const string &strD, const AssetList &aList)
  { // Unload existing certificates
    CertsEmpty();
    // Now initialising certificate store
    cLog->LogDebugExSafe(
      "Certs found $ files, initialising certificate store...", aList.size());
    // Create certificate store context
    scStore = SSL_CTX_new(TLS_client_method());
    if(!scStore) XC("Failed to create context for cert store cache!");
    // Get certificate store and failed if not retrieved
    xsCerts = SSL_CTX_get_cert_store(scStore);
    if(!xsCerts) XC("Failed to get cert store pointer!");
    // Allocate memory for list (assuming all certs are valid)
    lCAStore.reserve(aList.size());
    // Log that we're loading certificates in parallel
    cLog->LogDebugExSafe("Certs store initialised. Loading $ certificates...",
      aList.size());
    // Apple compiler does not support std::execution yet :(.
#if defined(MACOS)
    // Create sync method class
    LoadSerialised ccaSync;
    // Now initialising certificate store
    for(const string &strF : aList) CertsLoad(ccaSync, strD, strF);
#else
    // Create async method class
    LoadParallel ccaASync;
    // Now initialising certificate store
    StdForEach(par_unseq, aList.cbegin(), aList.cend(),
      [this, &ccaASync, &strD](const string &strF)
        { CertsLoad(ccaASync, strD, strF); });
#endif
    // If no certs were loaded then there is no need to keep the context
    if(lCAStore.empty()) CertsUnload();
    // We hav certs so shrink memory to fit actual contents
    else lCAStore.shrink_to_fit();
    // If we loaded all the certificates?
    if(lCAStore.size() == aList.size())
    { // Log that we loaded all the certificates
      cLog->LogInfoExSafe("Certs validated all $ certificates.",
        lCAStore.size(), aList.size());
    } // If we didn't load all the certificates?
    else
    { // Log a warning that we did not load all the certificates
      cLog->LogWarningExSafe("Certs could only validate $ of $ certificates!",
        lCAStore.size(), aList.size());
    }
  }
  /* --------------------------------------------------------------- */ public:
  X509_STORE *CertsGetStore(void) const { return xsCerts; }
  /* ----------------------------------------------------------------------- */
  const X509List &GetCertList(void) const { return lCAStore; }
  size_t GetCertListSize(void) const { return GetCertList().size(); }
  /* ----------------------------------------------------------------------- */
  bool CertsIsStoreAvailable(void) const { return !!CertsGetStore(); }
  /* -- Find a X509 error -------------------------------------------------- */
  auto CertsGetError(const size_t stId) const { return xErrDB.find(stId); }
  /* -- Is X509 error valid ------------------------------------------------ */
  bool CertsIsErrorValid(const auto &aItem) const
    { return aItem != xErrDB.cend(); }
  bool CertsIsNotErrorValid(const auto &aItem) const
    { return !CertsIsErrorValid(aItem); }
  /* -- Verify if a X509 bypass flag is set -------------------------------- */
  bool CertsIsX509BypassFlagSet(const size_t stBank, uint64_t qFlag)
    { return qCertBypass[stBank] & qFlag; }
  bool CertsIsNotX509BypassFlagSet(const size_t stBank, uint64_t qFlag)
    { return !CertsIsX509BypassFlagSet(stBank, qFlag); }
  /* -- Constructor --------------------------------------------- */ protected:
  Certs(void) :
    /* -- Initialisers ----------------------------------------------------- */
    scStore(nullptr),                  // No store initialised
    xsCerts(nullptr),                  // No certificate chain initialised
    qCertBypass{{0, 0}},               // No bypass flags setup yet
    strExtension{ "." CER_EXTENSION }, // Default extension
#define X509ERR(b,f,e) { X509_V_ERR_ ## e, { STR(e), b, 1ULL << f } }
    xErrDB{                            // Init X509 Error code definitions
      X509ERR(0, 0, UNSPECIFIED),
      X509ERR(0, 1, UNABLE_TO_GET_ISSUER_CERT),
      X509ERR(0, 2, UNABLE_TO_GET_CRL),
      X509ERR(0, 3, UNABLE_TO_DECRYPT_CERT_SIGNATURE),
      X509ERR(0, 4, UNABLE_TO_DECRYPT_CRL_SIGNATURE),
      X509ERR(0, 5, UNABLE_TO_DECODE_ISSUER_PUBLIC_KEY),
      X509ERR(0, 6, CERT_SIGNATURE_FAILURE),
      X509ERR(0, 7, CRL_SIGNATURE_FAILURE),
      X509ERR(0, 8, CERT_NOT_YET_VALID),
      X509ERR(0, 9, CERT_HAS_EXPIRED),
      X509ERR(0,10, CRL_NOT_YET_VALID),
      X509ERR(0,11, CRL_HAS_EXPIRED),
      X509ERR(0,12, ERROR_IN_CERT_NOT_BEFORE_FIELD),
      X509ERR(0,13, ERROR_IN_CERT_NOT_AFTER_FIELD),
      X509ERR(0,14, ERROR_IN_CRL_LAST_UPDATE_FIELD),
      X509ERR(0,15, ERROR_IN_CRL_NEXT_UPDATE_FIELD),
      X509ERR(0,16, OUT_OF_MEM),
      X509ERR(0,17, DEPTH_ZERO_SELF_SIGNED_CERT),
      X509ERR(0,18, SELF_SIGNED_CERT_IN_CHAIN),
      X509ERR(0,19, UNABLE_TO_GET_ISSUER_CERT_LOCALLY),
      X509ERR(0,20, UNABLE_TO_VERIFY_LEAF_SIGNATURE),
      X509ERR(0,21, CERT_CHAIN_TOO_LONG),
      X509ERR(0,22, CERT_REVOKED),
      X509ERR(0,23, INVALID_CA),
      X509ERR(0,24, PATH_LENGTH_EXCEEDED),
      X509ERR(0,25, INVALID_PURPOSE),
      X509ERR(0,26, CERT_UNTRUSTED),
      X509ERR(0,27, CERT_REJECTED),
      X509ERR(0,28, SUBJECT_ISSUER_MISMATCH),
      X509ERR(0,29, AKID_SKID_MISMATCH),
      X509ERR(0,30, AKID_ISSUER_SERIAL_MISMATCH),
      X509ERR(0,31, KEYUSAGE_NO_CERTSIGN),
      X509ERR(0,32, UNABLE_TO_GET_CRL_ISSUER),
      X509ERR(0,33, UNHANDLED_CRITICAL_EXTENSION),
      X509ERR(0,34, KEYUSAGE_NO_CRL_SIGN),
      X509ERR(0,35, UNHANDLED_CRITICAL_CRL_EXTENSION),
      X509ERR(0,36, INVALID_NON_CA),
      X509ERR(0,37, PROXY_PATH_LENGTH_EXCEEDED),
      X509ERR(0,38, KEYUSAGE_NO_DIGITAL_SIGNATURE),
      X509ERR(0,39, PROXY_CERTIFICATES_NOT_ALLOWED),
      X509ERR(0,40, INVALID_EXTENSION),
      X509ERR(0,41, INVALID_POLICY_EXTENSION),
      X509ERR(0,42, NO_EXPLICIT_POLICY),
      X509ERR(0,43, DIFFERENT_CRL_SCOPE),
      X509ERR(0,44, UNSUPPORTED_EXTENSION_FEATURE),
      X509ERR(0,45, UNNESTED_RESOURCE),
      X509ERR(0,46, PERMITTED_VIOLATION),
      X509ERR(0,47, EXCLUDED_VIOLATION),
      X509ERR(0,48, SUBTREE_MINMAX),
      X509ERR(0,49, APPLICATION_VERIFICATION),
      X509ERR(0,50, UNSUPPORTED_CONSTRAINT_TYPE),
      X509ERR(0,51, UNSUPPORTED_CONSTRAINT_SYNTAX),
      X509ERR(0,52, UNSUPPORTED_NAME_SYNTAX),
      X509ERR(0,53, CRL_PATH_VALIDATION_ERROR),
      X509ERR(0,54, PATH_LOOP),
      X509ERR(0,55, SUITE_B_INVALID_VERSION),
      X509ERR(0,56, SUITE_B_INVALID_ALGORITHM),
      X509ERR(0,57, SUITE_B_INVALID_CURVE),
      X509ERR(0,58, SUITE_B_INVALID_SIGNATURE_ALGORITHM),
      X509ERR(0,59, SUITE_B_LOS_NOT_ALLOWED),
      X509ERR(0,60, SUITE_B_CANNOT_SIGN_P_384_WITH_P_256),
      X509ERR(0,61, HOSTNAME_MISMATCH),
      X509ERR(0,62, EMAIL_MISMATCH),
      X509ERR(0,63, IP_ADDRESS_MISMATCH),
      X509ERR(1, 0, DANE_NO_MATCH),
      X509ERR(1, 1, EE_KEY_TOO_SMALL),
      X509ERR(1, 2, CA_KEY_TOO_SMALL),
      X509ERR(1, 3, CA_MD_TOO_WEAK),
      X509ERR(1, 4, INVALID_CALL),
      X509ERR(1, 5, STORE_LOOKUP),
      X509ERR(1, 6, NO_VALID_SCTS),
      X509ERR(1, 7, PROXY_SUBJECT_NAME_VIOLATION)
    }
#undef X509ERR
  /* -- No code ------------------------------------------------------------ */
  { }
  /* -- Destructor that unloads all x509 certificates ---------------------- */
  ~Certs(void) { CertsEmpty(); }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(Certs)               // Suppress default functions for safety
  /* --------------------------------------------------------------- */ public:
  CVarReturn CertsSetBypassFlags1(const uint64_t uiFlags)
    { return CVarSimpleSetInt(qCertBypass.front(), uiFlags); }
  /* ----------------------------------------------------------------------- */
  CVarReturn CertsSetBypassFlags2(const uint64_t uiFlags)
    { return CVarSimpleSetInt(qCertBypass.back(), uiFlags); }
  /* ----------------------------------------------------------------------- */
  CVarReturn CertsFileModified(const string &strD, string&)
  { // Empty string is ok, treat as no CA store
    if(strD.empty())
    { // Log that we are not using a store and return success
      cLog->LogWarningSafe("Certs using no root CA store!");
      return ACCEPT;
    } // Now initialising certificate store
    cLog->LogDebugExSafe("Certs searching '$' for certificates...", strD);
    // Get certificates in ca subdirectory and return if nothing found
    if(const AssetList aList{ strD, strExtension, false })
    { // Reload new certificates list
      CertsLoadList(strD, aList);
      // Clear error stack
      ERR_clear_error();
    } // No certificates to load so log it
    else cLog->LogWarningSafe("Certs found no matching files!");
    // Return success regardless
    return ACCEPT;
  }
}; /* ---------------------------------------------------------------------- */
/* ========================================================================= */
static StdTimeT CertGetTime(const ASN1_TIME &atD)
{ // Get expiry string
  const char*const cpStr = reinterpret_cast<const char*>(atD.data);
  // String position
  size_t stPos = 0;
  // Time structure
  StdTMStruct tD;
  // Get time type
  switch(atD.type)
  { // Two digit year?
    case V_ASN1_UTCTIME:
      tD.tm_year = (cpStr[stPos++] - '0') * 10;
      tD.tm_year += cpStr[++stPos] - '0';
      if(tD.tm_year < 70) tD.tm_year += 100;
      break;
    // Four digit year?
    case V_ASN1_GENERALIZEDTIME:
      tD.tm_year = (cpStr[stPos++] - '0') * 1000;
      tD.tm_year += (cpStr[++stPos] - '0') * 100;
      tD.tm_year += (cpStr[++stPos] - '0') * 10;
      tD.tm_year += (cpStr[++stPos] - '0');
      tD.tm_year -= 1900;
      break;
    // Unknown, just return string
    default: return 0;
  } // Continue other fields (-1 since January is 0 not 1.)
  tD.tm_mon   = (cpStr[stPos++] - '0') * 10;
  tD.tm_mon  += (cpStr[++stPos] - '0') - 1;
  tD.tm_mday  = (cpStr[stPos++] - '0') * 10;
  tD.tm_mday +=  cpStr[++stPos] - '0';
  tD.tm_hour  = (cpStr[stPos++] - '0') * 10;
  tD.tm_hour +=  cpStr[++stPos] - '0';
  tD.tm_min   = (cpStr[stPos++] - '0') * 10;
  tD.tm_min  +=  cpStr[++stPos] - '0';
  tD.tm_sec   = (cpStr[stPos++] - '0') * 10;
  tD.tm_sec  +=  cpStr[++stPos] - '0';
  // These are always zero
  tD.tm_isdst = tD.tm_yday = tD.tm_wday = 0;
  // Get local time and if the current time has expired
  return mktime(&tD);
}
/* ========================================================================= */
static bool CertIsExpired(const X509*const x509)
{ // Get current time
  const StdTimeT tTime = cmSys.GetTimeS();
  // Check expired begin and end date
  return tTime < CertGetTime(*X509_get0_notBefore(x509)) ||
         tTime > CertGetTime(*X509_get0_notAfter(x509));
}
/* ========================================================================= */
static const string CertGetSubject(const Certs::X509Pair &caPair)
{ // Storage for certificate field info
  string strD; strD.resize(1024);
  // Grab the subject line form ceritificate. We couldn't get the subject if
  // failed.
  if(!X509_NAME_oneline(X509_get_subject_name(caPair.second),
    const_cast<char*>(strD.data()), static_cast<int>(strD.length())))
      return {};
  // Resize and return string
  strD.resize(strlen(strD.c_str()));
  return strD;
}
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
