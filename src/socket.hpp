/* == SOCKET.HPP =========================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## Allows multi-threaded network communications using OpenSSL.         ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace ISocket {                    // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace ICert::P;              using namespace IClock::P;
using namespace ICollector::P;         using namespace ICommon::P;
using namespace ICrypt::P;             using namespace ICVar::P;
using namespace ICVarDef::P;           using namespace ICVarLib::P;
using namespace IError::P;             using namespace IEvtMain::P;
using namespace IFlags;                using namespace IIdent::P;
using namespace ILockable::P;          using namespace ILog::P;
using namespace ILuaEvt::P;            using namespace ILuaIdent::P;
using namespace ILuaLib::P;            using namespace ILuaUtil::P;
using namespace IMemory::P;            using namespace IParser::P;
using namespace IStd::P;               using namespace IString::P;
using namespace ISystem::P;            using namespace ISysUtil::P;
using namespace IThread::P;            using namespace IToggler::P;
using namespace IToken::P;             using namespace IUtil::P;
using namespace IUtf::P;               using namespace Lib::OS::OpenSSL;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* -- Connection flags ----------------------------------------------------- */
BUILD_SECURE_FLAGS(Socket,
  /* ----------------------------------------------------------------------- */
  // No flags                          Socket is initialising?
  SS_NONE                   {Flag(0)}, SS_INITIALISING           {Flag(1)},
  // Socket is set to use encryption?  Socket is connecting
  SS_ENCRYPTION             {Flag(2)}, SS_CONNECTING             {Flag(3)},
  // Socket is connected               Socket is sending request (HTTP)
  SS_CONNECTED              {Flag(4)}, SS_SENDREQUEST            {Flag(5)},
  // Socket waiting for reply? (HTTP)? Socket is downloading (HTTP)
  SS_REPLYWAIT              {Flag(6)}, SS_DOWNLOADING            {Flag(7)},
  // Socket is upgraded (WEBSOCKET)    Socket was closed by server?
  SS_UPGRADED               {Flag(8)}, SS_CLOSEDBYSERVER         {Flag(9)},
  // Socket was closed by server?      Socket is disconnecting?
  SS_CLOSEDBYCLIENT        {Flag(10)}, SS_DISCONNECTING         {Flag(11)},
  // Socket on standby (disconnected)
  SS_STANDBY               {Flag(12)},
  /* ----------------------------------------------------------------------- */
  // Set if error with event callback? Socket read a packet (not ever set)
  SS_EVENTERROR            {Flag(13)}, SS_READPACKET            {Flag(14)},
  // Pong from WebSocket?
  SS_PONG                  {Flag(15)}
);/* == Socket collector class for collector data and custom variables ===== */
CTOR_BEGIN(Sockets, Socket, CLHelperUnsafe,
/* -- Internal registry values for http data ------------------------------- **
** We use these key names internally for passing http data around without    **
** creating unneccesary new variables. Let us keep these key names in        **
** binary text because it is impossible for the http server to return        **
** header key names in binary! C++ and LUA can still store keys in binary    **
** as well so this should be safe! ----------------------------------------- */
const string       strRegVarREQ;       // Registry key name for req data
const string       strRegVarBODY;      // " for http body data
const string       strRegVarPROTO;     // " for http protocol data
const string       strRegVarCODE;      // " for http status code data
const string       strRegVarMETHOD;    // " for http method string
const string       strRegVarRESPONSE;  // HTTP response string
const string       strCipherDefault;   // Default cipher to use
/* -- Variables ------------------------------------------------------------ */
string_view        strvCipher12;       // Ciphers for TLSv1.2 from CVar
string_view        strvCipher13;       // Ciphers for TLSv1.3+ from CVar
string_view        strvUserAgent;      // User agent string from CVar
SafeInt            iOCSP;              // Use OCSP (0=Off;1=On;2=Strict)
SafeSizeT          stBufferSize;       // Default recv/send buffer size
SafeDouble         dRecvTimeout;       // Receive packet timeout
SafeDouble         dSendTimeout;       // Send packet timeout
SafeUInt64         qRX;                // Total bytes received
SafeUInt64         qTX;                // Total bytes sent
SafeUInt64         qRXp;               // Total packets received
SafeUInt64         qTXp;               // Total packets sent
SafeSizeT          stConnected;,,      // Total connected sockets
/* -- Derived classes ------------------------------------------------------ */
public Certs,                          // Certificate store
private LuaEvtMaster<Socket,LuaEvtTypeAsync<Socket>>);
/* == Socket object class ================================================== */
CTOR_MEM_BEGIN_CSLAVE(Sockets, Socket, ICHelperUnsafe),
  /* -- Base classes ------------------------------------------------------- */
  public LuaEvtSlave<Socket,2>,        // Need to store two references
  public Lockable,                     // Lua garbage collector instruction
  public TogglerMaster<>,              // Boolean to protect LUA callbacks
  public SocketFlags,                  // Socket flags
  public Ident                         // Identifier
{ /* ----------------------------------------------------------------------- */
  struct Packet                        // Connection packet
  { /* --------------------------------------------------------------------- */
    ClkTimePoint   ctpStart;           // Packet timestamp
    Memory         mData;              // Memory block
  };/* --------------------------------------------------------------------- */
  typedef list<Packet> PacketList;     // list of blocks
  /* ----------------------------------------------------------------------- */
  enum OpCode : unsigned int           // Websocket packet op codes
  { /* --------------------------------------------------------------------- */
    OC_NONE      = static_cast<unsigned int>(-1),
    OC_FRAG      = 0x0,                // Fragmented packet
    OC_TEXT      = 0x1,                // UTF-8 Text packet
    OC_BINARY    = 0x2,                // Binary text packet
    OC_CLOSE     = 0x8,                // Close the connection request
    OC_PING      = 0x9,                // Ping the client test
    OC_PONG      = 0xA                 // Server ACK the ping client test.
  };/* -- OpenSSL core variables ------------------------------------------- */
  BIO             *bioPtr;             // OpenSSL socket, blank socket
  SSL_CTX         *sslctxPtr;          // OpenSSL context
  SSL             *sslPtr;             // OpenSSL descriptor
  /* -- Statistical variables ---------------------------------------------- */
  SafeUInt64       qRX, qTX,           // Total Transmit/Receive traffic
                   qRXp, qTXp;         // Total Transmit/Receive packets
  /* -- Threads and concurrency -------------------------------------------- */
  Thread           tReader,            // Thread for sockread/http operations
                   tWriter;            // Thread for sockwrite operations
  mutex            mMutex,             // mutex to prevent threading deadlocks
                   mWriter;            // For condition variable
  bool             bUnlock;            // Condition variable unblocker
  condition_variable cvWriter;         // Waiting for write/terminate event
  /* -- Other variables ---------------------------------------------------- */
  unsigned int     uiPort;             // The port number to connect to
  SafeInt          iError,             // Socket error
                   iFd;                // Socket descriptor
  string           strAddr,            // The address in string format
                   strAddrPort,        // The address and port in string format
                   strError,           // Last error string recorded
                   strCipherList,      // Requested cipher list (<=TLSv1.2)
                   strCipherSuite,     // Requested cipher suites (TLSv1.3)
                   strCipher,          // Cipher picked if SSL requested
                   strIP,              // IP address connected to
                   strHost,            // Virtual hostname connected to
                   strRealHost;        // Real hostname connected to
  PacketList       plRX, plTX;         // Transmit/Receive buffers
  size_t           stRX, stTX;         // Total bytes stored in buffers
  Parser<>         pRegistry;          // For storing keypairs
  /* -- Timestamps --------------------------------------------------------- */
  SafeClkDuration  cdConnect,          // Time socket was connecting
                   cdConnected,        // Time socket was connected
                   cdRead,             // Time socket was last read from
                   cdWrite,            // Time socket was last written to
                   cdDisconnect,       // Time socket was disconnecting
                   cdDisconnected;     // Time socket was disconnected
  /* -- Do internal log ---------------------------------------------------- */
  template<typename ...VarArgs>void SocketLog(const LHLevel lhlSeverity,
    const char*const cpFormat, const VarArgs &...vaArgs)
  { // If parameters are specified then cater to them
    if constexpr(sizeof...(VarArgs) > 0)
      cLog->LogExSafe(lhlSeverity, "Socket $:$$$:$ $", CtrGet(), hex,
        FlagGet(), dec, GetAddressAndPort(), StrFormat(cpFormat, vaArgs...));
    // No parameters specified so don't need to format them
    else cLog->LogExSafe(lhlSeverity, "Socket $:$$$:$ $", CtrGet(), hex,
      FlagGet(), dec, GetAddressAndPort(), cpFormat);
  }
  /* -- Internal log ------------------------------------------------------- */
  template<typename ...VarArgs>void SocketLogSafe(const LHLevel lhlSeverity,
    const char*const cpFormat, const VarArgs &...vaArgs)
  { // Return if we don't have this level
    if(cLog->NotHasLevel(lhlSeverity)) return;
    // Synchronise access to socket data while we log details
    const LockGuard lgSocketSync{ mMutex };
    // Write formatted string
    SocketLog(lhlSeverity, cpFormat, vaArgs...);
  }
  /* -- Internal log ------------------------------------------------------- */
  template<typename ...VarArgs>void SocketLogUnsafe(const LHLevel lhlSeverity,
    const char*const cpFormat, const VarArgs &...vaArgs)
  { // Return if we don't have this level
    if(cLog->NotHasLevel(lhlSeverity)) return;
    // Write formatted string
    SocketLog(lhlSeverity, cpFormat, vaArgs...);
  }
  /* -- Initialise static error (no openssl error) ------------------------- */
  ThreadStatus SetErrorStatic(const string &strReason, const bool bSet)
  { // Show reason in log
    SocketLogUnsafe(LH_WARNING, "$", strReason);
    // Forget any error if disconnecting or disconnected
    if(!bSet || IsDisconnectingOrDisconnected()) return TS_ERROR;
    // Set our own error code
    iError = -1;
    // Set the error as the reason
    strError = StdMove(strReason);
    // Done
    return TS_ERROR;
  }
  /* -- Set aborted connection --------------------------------------------- */
  ThreadStatus SetAborted(void)
  { // Clear errors
    ERR_clear_error();
    // Connection aborted message
    strError = "Connection aborted";
    // Set our own error code
    iError = -1;
    // Set the error as the reason
    SocketLogSafe(LH_WARNING, "$", strError);
    // Done
    return TS_ERROR;
  }
  /* -- Socket initial connect failed cleanup function --------------------- */
  ThreadStatus SetError(const string &strReason)
  { // If socket was not forcefully closed by us
    if(!IsDisconnectedByClient())
    { // If disconnecting or disconnected? Show reason in log
      if(IsDisconnectingOrDisconnected())
        SocketLogUnsafe(LH_WARNING, "$", strReason);
      // Not disconnecting or disconnected?
      else
      { // Process errors... Only show errors we can actually report on
        iError = CryptGetError(strError);
        SocketLogUnsafe(LH_WARNING, "$: $", strReason, strError);
        if(strError.empty()) strError = strReason;
      }
    } // Return thread error status
    return TS_ERROR;
  }
  /* -- Read socket -------------------------------------------------------- */
  size_t SockRead(char *cpData, const size_t stSize)
  { // If thread should exit
    if(tReader.ThreadShouldExit() || tWriter.ThreadShouldExit())
       return static_cast<size_t>(SetAborted());
    // Wait for new packet, storing bytes read and compare result
    switch(const size_t stRead = static_cast<size_t>
      (BIO_read(bioPtr, cpData, static_cast<int>(stSize))))
    { // Did the server close the conection?
      case 0:
      { // Server closed the connection
        FlagSet(SS_CLOSEDBYSERVER);
        // Return error status, but there is no actual socket error
        return static_cast<size_t>(TS_ERROR);
      } // Did the operation fail? Set error and clean up
      case StdMaxUInt:
        return static_cast<size_t>(IsDisconnectedByClient() ?
          SetAborted() : SetErrorSafe("Read error or timeout"));
      // Did openssl fail? Set error and clean up
      case static_cast<size_t>(-2):
        return static_cast<size_t>(SetErrorSafe("Not implemented"));
      // We read data. Incrememnt counter
      default:
        // Increment received bytes and packet counters
        qRX += stRead;
        ++qRXp;
        cParent->qRX += stRead;
        ++cParent->qRXp;
        // Set last received timestamp
        cdRead = cmHiRes.GetEpochTime();
        // Log status
        SocketLogSafe(LH_DEBUG, "$ received", stRead);
        // Return bytes read
        return stRead;
    }
  }
  /* -- Write socket ------------------------------------------------------- */
  size_t SockWrite(const char *cpData, const size_t stSize)
  { // If thread should exit
    if(tReader.ThreadShouldExit() || tWriter.ThreadShouldExit())
      return static_cast<size_t>(SetAborted());
    // Wait to write new packet, storing bytes written and compare result
    switch(const size_t stWritten = static_cast<size_t>
      (BIO_write(bioPtr, cpData, static_cast<int>(stSize))))
    { // Server closed connection. Set connection completed status
      case 0:
      { // Server closed the connection so return that the server closed
        FlagSet(SS_CLOSEDBYSERVER);
        return static_cast<size_t>(SetErrorStaticSafe("EOF from server"));
      } // Did the operation fail? Disconnect with error
      case StdMaxUInt:
        return static_cast<size_t>(SetErrorSafe("Send error or timeout"));
      // Other error? Set error and clean up
      case static_cast<size_t>(-2):
        return static_cast<size_t>(SetErrorSafe("Not implemented"));
      // We wrote data.
      default:
        // Increment sent bytes and packet counters
        qTX += stWritten;
        ++qTXp;
        cParent->qTX += stWritten;
        ++cParent->qTXp;
        // Set last sent timestamp
        cdWrite = cmHiRes.GetEpochTime();
        // Make sure we sent the same bytes as read. This should never
        // happen, but if we did?
        if(stWritten == stSize)
        { // Log the bytes sent and return bytes written
          SocketLogSafe(LH_DEBUG, "$ sent", stWritten);
          return stWritten;
        } // Log the error we did not send enough bytes
        return static_cast<size_t>
          (SetErrorSafe(StrFormat("Sent only $ of $", stWritten, stSize)));
    }
  }
  /* -- Write string to socket --------------------------------------------- */
  size_t SockWrite(const string &strStr)
    { return SockWrite(strStr.data(), strStr.length()); }
  /* -- Write memory block class to socket --------------------------------- */
  size_t SockWrite(const MemConst &mcSrc)
    { return SockWrite(mcSrc.MemPtr<char>(), mcSrc.MemSize()); }
  /* -- Convert packet to memblock for LUA API ----------------------------- */
  double GetPacket(Memory &mDest, PacketList &plData, size_t &stS)
  { // Get first top packet and move data to memblock supplied by caller
    Packet &pData = plData.front();
    mDest.MemSwap(pData.mData);
    // Copy record timestamp
    const ClkTimePoint ctpEnd{ StdMove(pData.ctpStart) };
    // Subtract total bytes counter
    stS -= mDest.MemSize();
    // Pop first RX packet
    plData.pop_front();
    // Return timestamp
    return ClockGetCount<duration<double>>(ctpEnd.time_since_epoch());
  }
  /* -- Packet management -------------------------------------------------- */
  void FlushPackets(PacketList &plList, size_t &stTotal)
    { plList.clear(); stTotal = 0; }
  /* -- Flush all stored packets ------------------------------------------- */
  void FlushPackets(void)
  { // Setup lists we want to flush
    struct PacketListRef { PacketList &plList; size_t &stTotal; };
    typedef array<const PacketListRef, 2> PacketListArray;
    const PacketListArray plaData{ { { plRX, stRX }, { plTX, stTX } } };
    // Flush each list and total value
    for(const PacketListRef &plrItem : plaData)
      FlushPackets(plrItem.plList, plrItem.stTotal);
  }
  /* -- Packet management -------------------------------------------------- */
  void PushData(PacketList &plList, size_t &stTotal, const char*const cpData,
    const size_t stSize)
  { // Insert a new entry into the specified packet list
    plList.push_back({ cmHiRes.GetTime(), { stSize, cpData } });
    // Increase counter for bytes processed
    stTotal += stSize;
  }
  /* -- Send raw data ------------------------------------------------------ */
  void Send(const char *cpData, const size_t stSize)
  { // Bail if not connected
    if(!IsConnected())
      XC("Send on unconnected socket!", "Address", strAddr, "Port", uiPort);
    // Add buffer to queue
    PushData(plTX, stTX, cpData, stSize);
    // Unblock writer thread
    WriteUnblock();
  }
  /* -- Send data as other types ------------------------------------------- */
  void Send(const MemConst &mcPacket)
    { Send(mcPacket.MemPtr<char>(), mcPacket.MemSize()); }
  void SendString(const string &strData)
    { Send(strData.data(), strData.length()); }
  /* ----------------------------------------------------------------------- */
  void SetAddressAndCipher(string &strNAddress, const unsigned int &uiNPort,
    const string &strNCipher)
  { // OK set address and port
    strAddr = StdMove(strNAddress);
    uiPort = uiNPort;
    strAddrPort = StrAppend(strAddr, ':', uiNPort);
    // Clear previous names if re-using class
    strRealHost.clear();
    strIP.clear();
    // Default specified? Use defaults from both cvars
    if(strNCipher == cParent->strCipherDefault)
    { // Setup <=TLSv1.2 ciphers
      strCipherList = cSockets->strvCipher12;
      // Setup TLSv1.3 ciphers
      strCipherSuite = cSockets->strvCipher13;
      // Done
      return;
    } // Split ciphers into two tokens
    const Token tData{ strNCipher, "|" };
    // If we only have one part
    switch(tData.size())
    { // No tokens (insecure connection)
      case 0: strCipherSuite.clear(); strCipherList.clear(); break;
      // Only one token specified?
      case 1:
      { // Set TLSv1.3 cipher suite
        const string &strSuite = tData.front();
        strCipherSuite = strSuite == cParent->strCipherDefault ?
          cSockets->strvCipher13 : strSuite;
        // Set <=TLSv1.2 cipher list
        strCipherList = cSockets->strvCipher12;
        // Done
        break;
      } // Two tokens specified?
      case 2:
      { // Set TLSv1.3 cipher suite
        const string &strSuite = tData.front();
        strCipherSuite = strSuite == cParent->strCipherDefault ?
          cSockets->strvCipher13 : strSuite;
        // Set <= TLSv1.2 cipher list
        const string &strList = tData[1];
        strCipherList = strList == cParent->strCipherDefault ?
          cSockets->strvCipher12 : strList;
        // Done
        break;
      } // Invalid
      default: XC("Only two cipher tokens allowed!",
                  "Address", strAddr,     "Port", uiPort,
                  "Count",   tData.size(),"Spec", strNCipher);
    }
  }
  /* -- Disconnect the socket ---------------------------------------------- */
  void FinishDisconnect(void)
  { // Lock mutex to prevent data race
    const LockGuard lgSocketSync{ mMutex };
    { // Have BIO socket pointer?
      if(bioPtr)
      { // This automatically frees the SSL context
        BIO_free_all(bioPtr);
        // Clear addresses
        bioPtr = nullptr;
        sslPtr = nullptr;
      } // Have SSL pointer? If for some reason we have it and not a bio
      else if(sslPtr)
      { // Shutdown and free the SSL context
        SSL_shutdown(sslPtr);
        SSL_free(sslPtr);
        sslPtr = nullptr;
      } // Clear context if created
      if(sslctxPtr) { SSL_CTX_free(sslctxPtr); sslctxPtr = nullptr; }
    } // Don't log if we're already disconnected
    if(IsDisconnected()) return;
    // Set standby status
    AddStatus(SS_STANDBY, cdDisconnected);
    // Return if socket was never connected
    if(FlagIsClear(SS_CONNECTED)) return;
    // Decrement connection count
    --cParent->stConnected;
    // Report disconnection and statistics to log
    SocketLogUnsafe(LH_DEBUG, "Disconnected (RX:$/$;TX:$/$).",
      GetRXpkt(), GetRX(), GetTXpkt(), GetTX());
  }
  /* -- Compact all packets into single packet ----------------------------- */
  void Compact(Memory &mDest, PacketList &plData, size_t &stX)
  { // Bail if no packets, but 0 bytes will still be allocated
    if(plData.empty()) { mDest.MemInitBlank(); return; }
    // If zero size just flush all the empty packets and return
    if(!stX) { mDest.MemInitBlank(); return FlushPackets(plData, stX); }
    // Resize memblock to hold all data
    mDest.MemInitBlank(stX);
    // Size of buffers is now zero
    stX = 0;
    // Byte offset counter
    size_t stOffset = 0;
    // Loop until...
    do
    { // Get packet memory block and copy it into our destination memory block
      const MemConst &mcPacket = plData.front().mData;
      mDest.MemWriteBlock(stOffset, mcPacket);
      // Increment counter
      stOffset += mcPacket.MemSize();
      // Pop packet
      plData.pop_front();
    } // ...list is fully emptied
    while(!plData.empty());
  }
  /* -- Create connection with select used to monitor for timeout ---------- */
  ThreadStatus DoConnect(void)
  { // Set hostname (always returns 1).
    if(CryptBIOSetConnHostname(bioPtr, GetAddressAndPort().c_str()) != 1)
      return SetErrorSafe("Resolve failed");
    // Log and do secure connection
    SocketLogSafe(LH_DEBUG, "$onnecting...", IsSecure() ? "Securely c" : "C");
    // Set connecting flag. Do send an event for this
    AddStatus(SS_CONNECTING, cdConnect);
    // Abort if requested
    if(tReader.ThreadShouldExit()) return SetAborted();
    // Try to connect and if failed?
    if(BIO_do_connect(bioPtr) != 1) return SetErrorSafe("Connect failed");
    // Abort if requested
    if(tReader.ThreadShouldExit()) return SetAborted();
    // Set descriptor and set error if failed
    if(!UpdateDescriptor()) return SetErrorSafe("Lost descriptor");
    // Get and check pointer to address data
    if(const BIO_ADDR*const baData = CryptBIOGetConnAddress(bioPtr))
    { // Setup query commands for host and IP data
      struct AddressData{ const int iId; string &strDest; };
      typedef array<const AddressData, 2> AddressDataArray;
      const AddressDataArray adaCmds{ { { 1, strIP }, { 0, strRealHost } } };
      // Enumerate and store the address data
      for(const AddressData &adCmd : adaCmds)
      { // Thanks to OpenSSL not giving us a unique_ptr compatible deallocator,
        // we need to do this ugly code to auto free the allocated the address.
        // I don't know how to make OPENSSL_free work with unique_ptr!
        struct AddrPtr{ const char*const cpPtr;
          AddrPtr(const BIO_ADDR*const baD, const int iId) :
            cpPtr(BIO_ADDR_hostname_string(baD, iId)) { }
          ~AddrPtr(void)
            { if(cpPtr) OPENSSL_free(UtfToNonConstCast<void*>(cpPtr)); }
          operator bool(void) const { return cpPtr != nullptr; }
        };
        // Get item of interest and if successful, move the result into the
        // specified destination
        if(const AddrPtr apAddr{ baData, adCmd.iId })
        { // Lock mutex
          const LockGuard lgSocketSync{ mMutex };
          // Load C-String into STL string
          adCmd.strDest = apAddr.cpPtr;
        }
      }
    } // No IP address detected for some reason
    else return SetErrorSafe("No address found");
    // Show connected ip address
    SocketLogSafe(LH_DEBUG, "Connected to $", GetIPAddress());
    // Set socket read and send timeout
    switch(cSystem->SetSocketTimeout(iFd,
      cParent->dRecvTimeout, cParent->dSendTimeout))
    { // Success
      case 0: break;
      // Failed so just log message
      case 1:
        SocketLogSafe(LH_WARNING, "Set recv timeout failed");
        break;
      case 2:
        SocketLogSafe(LH_WARNING, "Set send timeout failed");
        break;
      case 3:
        SocketLogSafe(LH_WARNING, "Set recv/send timeout failed");
        break;
      default:
        SocketLogSafe(LH_WARNING, "Unknown error setting socket timeouts");
        break;
    } // Until thread says to terminate
    if(tReader.ThreadShouldExit()) return SetAborted();
    // Return success if the handshake succeeded or process error
    return BIO_do_handshake(bioPtr) == 1 ?
      TS_OK : SetErrorSafe("Handshake failed");
  }
  /* -- OCSP verification result ------------------------------------------- */
  int OCSPVerificationResponse(SSL*const sslCbPtr)
  { // Allocate memory for response and get size of response. We actually need
    // the response to not be nullptr too or there is no response
    const unsigned char *cpResp = nullptr;
    const long lLength = SSL_get_tlsext_status_ocsp_resp(sslCbPtr, &cpResp);
    if(lLength != -1 && UtfIsCStringValid(cpResp))
    { // Got a response so make sure it is freed on leaving the scope
      typedef unique_ptr<ocsp_response_st,
        function<decltype(OCSP_RESPONSE_free)>> OcspResponsePtr;
      if(OcspResponsePtr orpResp{ d2i_OCSP_RESPONSE(nullptr, &cpResp,
        lLength), OCSP_RESPONSE_free })
          return 1;
      // Failed to parse response
      SocketLogSafe(LH_WARNING, "OCSP response parse failure");
      // Return result from response call
      return 0;
    } // No response but connection may continue
    SocketLogSafe(LH_DEBUG, "No OCSP response");
    // The callback when used on the client side should return a negative value
    // on error; 0 if the response is not acceptable (in which case the
    // handshake will fail) or a positive value if it is acceptable.
    return cParent->iOCSP >= 2 ? 0 : 1;
  }
  /* -- Socket initial connect function ------------------------------------ */
  ThreadStatus InitialConnect(void)
  { // Initialise the status flags
    FlagReset(SS_INITIALISING);
    // Reset counters and timers
    qRX = qTX = qRXp = qTXp = 0;
    cdConnect = cdConnected = cdRead = cdWrite = cdDisconnect =
      cdDisconnected = seconds{0};
    // Flush packets in all buffers
    FlushPackets();
    // If want TLS encryption?
    if(!strCipherSuite.empty() || !strCipherList.empty())
    { // Set encryption flag. Do not send a LUA event for this
      FlagSet(SS_ENCRYPTION);
      // Setup new TLS client context
      sslctxPtr = SSL_CTX_new(TLS_client_method());
      if(!sslctxPtr) return SetErrorSafe("Init TLS failed");
      // Set cipher options
      if(!strCipherSuite.empty())
        if(!SSL_CTX_set_ciphersuites(sslctxPtr, strCipherSuite.c_str()))
          return SetErrorStaticSafe("Invalid cipher suite");
      // Set ciphers supported, and if failed? Just show warning
      if(!strCipherList.empty())
        if(!SSL_CTX_set_cipher_list(sslctxPtr, strCipherList.c_str()))
          return SetErrorStaticSafe("Invalid cipher list");
      // Set context to release buffers as we don't reuse the contexts
      SSL_CTX_set_mode(sslctxPtr, SSL_MODE_RELEASE_BUFFERS);
      // Set our shared certificate store if we have one
      if(cParent->CertsIsStoreAvailable() &&
        !CryptSSLCtxSet1VerifyCertStore(sslctxPtr, cParent->CertsGetStore()))
          return SetErrorStaticSafe("Cert store failure");
      // Setup verification, make a new verification context and if succeded?
      typedef unique_ptr<X509_VERIFY_PARAM,
        function<decltype(X509_VERIFY_PARAM_free)>> X509VerifyParamPtr;
      if(X509VerifyParamPtr x509vp{ X509_VERIFY_PARAM_new(),
        X509_VERIFY_PARAM_free })
      { // Set flags
        // * Check all ceritificates against CRL.
        // * Strict X509 certificate formats.
        // * Check root CA (self-signed) certificates.
        if(X509_VERIFY_PARAM_set_flags(x509vp.get(), X509_V_FLAG_CRL_CHECK_ALL
          | X509_V_FLAG_X509_STRICT | X509_V_FLAG_CHECK_SS_SIGNATURE))
        { // Always check subject line in certificate
          X509_VERIFY_PARAM_set_hostflags(x509vp.get(),
            X509_CHECK_FLAG_ALWAYS_CHECK_SUBJECT);
          // Want good encryption
          X509_VERIFY_PARAM_set_auth_level(x509vp.get(), 0);
          // Our certificate chain is full of server CA certificates
          if(X509_VERIFY_PARAM_set_purpose(x509vp.get(),
            X509_PURPOSE_SSL_SERVER))
          { // Ceritificate must match this domain
            if(X509_VERIFY_PARAM_set1_host(x509vp.get(), strAddr.data(),
              strAddr.length()))
            { // Apply to context and return success
              if(!SSL_CTX_set1_param(sslctxPtr, x509vp.get()))
              { // Log the error and return failure
                SocketLogSafe(LH_WARNING,
                  "Failed to assign verification parameters to context");
                return TS_ERROR;
              } // Succeeded a this point
            } // Failed setting host?
            else
            { // Log the error and return failure
              SocketLogSafe(LH_WARNING, "Failed to set matching hostname");
              return TS_ERROR;
            }
          } // Failed setting purpose?
          else
          { // Log the error and return failure
            SocketLogSafe(LH_WARNING, "Failed to set purpose");
            return TS_ERROR;
          }
        } // Failed setting verification flags?
        else
        { // Log the error and return failure
          SocketLogSafe(LH_WARNING, "Failed to set verification flags");
          return TS_ERROR;
        }
      } // Failed creating context?
      else
      { // Log the error and return failure
        SocketLogSafe(LH_WARNING, "Failed to create verification context");
        return TS_ERROR;
      } // Done setting up verification. Now create socket
      bioPtr = BIO_new_ssl_connect(sslctxPtr);
      if(!bioPtr) return SetErrorSafe("Failed to create BIO socket");
      // Try to get ssl pointer from socket
      if(CryptBIOGetSSL(bioPtr, &sslPtr) < 1)
        return SetErrorSafe("No SSL ptr from BIO");
      if(!sslPtr) return SetErrorSafe("Get SSL ptr failed");
      // Client mode
      BIO_set_ssl_mode(bioPtr, 1);
      // OCSP verification option enabled?
      if(cParent->iOCSP >= 1)
      { // Setup OCSP verification
        if(!SSL_set_tlsext_status_type(sslPtr, TLSEXT_STATUSTYPE_ocsp))
          SocketLogSafe(LH_WARNING, "Failed to setup OCSP verification!");
        // Set callback and argument
        int(*fCB)(SSL*,void*) = [](SSL*const sO, void*const vpS)->int
          { return reinterpret_cast<Socket*>(vpS)->
              OCSPVerificationResponse(sO); };
        if(!CryptSSLCtxSetTlsExtStatusCb(sslctxPtr, fCB))
          SocketLogSafe(LH_WARNING,
            "Failed to setup OCSP verification callback!");
        if(!SSL_CTX_set_tlsext_status_arg(sslctxPtr,
          reinterpret_cast<void*>(this)))
            SocketLogSafe(LH_WARNING,
              "Failed to setup OCSP verification argument!");
      } // Set SNI hostname. Some sites break if this is not set
      if(!CryptSSLSetTlsExtHostName(sslPtr, strAddr.c_str()))
        return SetErrorStaticSafe("Init TLS SNI hostname failed");
      // Log and do secure connection
      if(DoConnect() == TS_ERROR) return TS_ERROR;
      // Get X509 chain verificiation result
      switch(const size_t stRes =
        static_cast<size_t>(SSL_get_verify_result(sslPtr)))
      { // No error? Log success and carry on
        case X509_V_OK: SocketLogSafe(LH_DEBUG, "X509 chain is good"); break;
        // Anything else?
        default:
        { // Find error code if the error code information is not found?
          const auto xErrInfoIt{ cParent->CertsGetError(stRes) };
          if(cParent->CertsIsNotErrorValid(xErrInfoIt))
          { // Return success if user wants to bypass it
            if(cParent->CertsIsNotX509BypassFlagSet(1, 0x8000000000000000ULL))
            { // Set error and return status
              SetErrorStaticSafe(StrAppend("X509_V_ERR_UNKNOWN_", stRes));
              return TS_ERROR;
            } // Log the warning and return success
            SocketLogSafe(LH_WARNING, "Unknown X509 error $ bypassed!", stRes);
          } // Found the error code
          else
          { // Get reference to structure
            const auto &xErrInfo = xErrInfoIt->second;
            // Build error code
            const string strErr{ StrAppend("X509_V_ERR_", xErrInfo.cpErr) };
            // Return success if user wants to bypass it
            if(cParent->CertsIsNotX509BypassFlagSet(
                 xErrInfo.stBank, xErrInfo.qFlag))
            { // Set error and return status
              SetErrorStaticSafe(strErr);
              return TS_ERROR;
            } // Log the warning and return success
            SocketLogSafe(LH_WARNING, "$ bypassed!", strErr);
            // Set socket error and error string
          } // Done
          break;
        }
      } // This is the size of the temporary string buffer (ssl needs int)
      const int iLen = 128;
      // Make a buffer of that size. Shouldn't really statically allocate it.
      const Memory mStr{ static_cast<size_t>(iLen) };
      // Get address of memory
      char*const cpStr = mStr.MemPtr<char>();
      // Is the cipher available?
      if(SSL_CIPHER_description(SSL_get_current_cipher(sslPtr), cpStr, iLen))
      { // Synchronise access to cipher string
        const LockGuard lgSetCipher{ mMutex };
        // Set cipher and remove spaces, carriage returns and linefeeds
        strCipher = cpStr;
        StrCompactRef(strCipher);
        StrChop(strCipher);
        // Print encryption info. Don't need to lock twice
        SocketLogUnsafe(LH_DEBUG, "Cipher is $", strCipher);
      } // Get cipher failed? Log failure
      else return SetErrorSafe("Server using no cipher!");
      // Get server certificate
      if(const X509*const xCert = SSL_get0_peer_certificate(sslPtr))
      { // Get certificate subject and if successful? Log subject line. OpenSSL
        // doesn't give us the length so feed into logger as c-string
        if(X509_NAME_oneline(X509_get_subject_name(xCert), cpStr, iLen))
          SocketLogSafe(LH_DEBUG, "Subject is $", cpStr);
        // Get certificate issuer and if successful? Log issuer line
        if(X509_NAME_oneline(X509_get_issuer_name(xCert), cpStr, iLen))
          SocketLogSafe(LH_DEBUG, "Issuer is $", cpStr);
        // Don't free certificate since we're using SSL_get0_*
      } // Error occured
      else return SetErrorSafe("Server returned no certificate");
    } // No security
    else
    { // Create socket and bail out if failed
      bioPtr = BIO_new(BIO_s_connect());
      if(!bioPtr) return SetErrorSafe("Failed to create BIO socket");
      // Log and do insecure connection
      if(DoConnect() == TS_ERROR) return TS_ERROR;
    } // Now connected
    AddStatus(SS_CONNECTED, cdConnected);
    // Increase connected count
    ++cParent->stConnected;
    // Successful connect
    return TS_OK;
  }
  /* -- Writer thread notification ----------------------------------------- */
  void WriteUnblock(void)
  { // Acquire unique lock
    const UniqueLock ulSocketGuard{ mWriter };
    // Unblock variable
    bUnlock = true;
    // Notify the condition variable
    cvWriter.notify_one();
  }
  /* -- Get registry iterator ---------------------------------------------- */
  StrNCStrMapIt GetRegistryIterator(const string &strItem)
    { return pRegistry.find(strItem); }
  /* -- Get and delete registry item --------------------------------------- */
  const string GetRegistry(const string &strItem)
  { // Find item and if we didn't find it? Return default string
    const StrNCStrMapIt sncsmiIt{ GetRegistryIterator(strItem) };
    if(sncsmiIt == pRegistry.cend()) return {};
    // Get the value and delete it. We will move instead of copying
    const string strReq{ StdMove(sncsmiIt->second) };
    pRegistry.erase(sncsmiIt);
    return strReq;
  }
  /* -- String is binary? Returns location of binary ----------------------- */
  bool ValidHeaderPacket(const string_view &strStr)
  { // For each character in response, if the character is valid printable
    // ASCII character then goto next
    return !any_of(strStr.cbegin(), strStr.cend(), [](const unsigned char &ucC)
      { return ucC < ' ' && ucC != '\r' && ucC != '\n'; });
  }
  /* -- Terminate writer thread -------------------------------------------- */
  void SocketTerminateWriteThread(void)
  { // Return if wrong thread or not running
    if(tWriter.ThreadIsCurrent() || tWriter.ThreadIsNotJoinable()) return;
    // Call for writer thread to terminate
    tWriter.ThreadSetExit();
    // Unblock writer thread so that it may terminate cleanly
    WriteUnblock();
    // Wait for thread to terminate and deinit
    tWriter.ThreadStop();
    tWriter.ThreadDeInit();
  }
  /* -- Web socket main thread function reusing temporary buffer ----------- */
  ThreadStatus WebSocketMain(Memory &mDest)
  { // Check for upgrade status and return error if not found
    const StrNCStrMapIt sncsmiIt{ GetRegistryIterator("Upgrade") };
    if(sncsmiIt == pRegistry.cend())
      return SetErrorStaticSafe("Missing upgrade header!");
    // Must be websocket
    if(sncsmiIt->second != "websocket")
      return SetErrorStaticSafe(
        StrAppend("Invalid upgrade protocol '$'!", sncsmiIt->second));
    // Create a thread to write data requests
    tWriter.ThreadInit(StrAppend("websocketwriter:", CtrGet()),
      bind(&Socket::SockWebWriteThreadMain, this, _1), this);
    // Set upgraded status and dispatch event
    AddStatus(SS_UPGRADED);
    // Websocket packet op codes
    enum OpCode uiOpCode = OC_NONE;
    // Websocket packet read modes
    enum ReadMode : unsigned int { RM_HEADER, RM_SMALL_PAYLOAD,
      RM_LARGE_PAYLOAD, RM_PAYLOAD } uiMode = RM_HEADER;
    // Current buffer position and required bytes
    size_t stRequired  = 2, // Bytes required in buffer
           stPLTotal   = 0, // Payload size
           stPLCurrent = 0; // Payload progress
    bool bFinal = false;    // Is final websocket packet?
    PacketList plTemp;      // Packets in current header
    ClkDuration cdPing;     // Ping time
    // Loop until thread should terminate
    while(tReader.ThreadShouldNotExit())
    { // Current buffer position and bytes read
      size_t stTRead = 0;
      // We need more bytes to required or fill the buffer?
      Read: for(size_t stRead = 0;
                stTRead < stRequired && stTRead < mDest.MemSize();
                stTRead += stRead)
      { // Wait for new payload and return if error or quit requested
        const size_t stAmount = mDest.MemSize() - stTRead;
        stRead = SockRead(mDest.MemRead(stTRead, stAmount), stAmount);
        if(stRead == static_cast<size_t>(TS_ERROR)) return TS_ERROR;
        if(tReader.ThreadShouldExit()) return TS_OK;
      } // Calculate any extra data we read
      size_t stExtra = stTRead > stRequired ? stTRead - stRequired : 0;
      // What section of the data are we on?
      switch(uiMode)
      { // Payload?
        case RM_PAYLOAD: Payload:
        { // Put payload straight into packets
          const size_t stPLPktSize = stTRead - stExtra;
          PushData(plTemp, stPLCurrent,
            mDest.MemRead(0, stPLPktSize), stPLPktSize);
          // Check opcode
          switch(uiOpCode)
          { // Normal data?
            case OC_TEXT: case OC_BINARY: case OC_FRAG:
            { // Keep reading data if we're not done yet
              stRequired -= stPLPktSize;
              if(stRequired) continue;
              // Ignore if not final packet
              if(!bFinal) break;
              { // Lock access to the packet list
                const LockGuard lgSocketSync{ mMutex };
                // Move packets to main packet list
                plRX.splice(plRX.end(), plTemp);
                // Set total size
                stRX += stPLTotal;
              } // Tell client that we have data ready
              DispatchEvent(SS_READPACKET);
              // Break to reset counters
              break;
            } // Ping? event?
            case OC_PING:
            { // Save ping time
              cdPing = cmHiRes.GetEpochTime();
              // Compact data segments and send it back
              Memory mTemp;
              Compact(mTemp, plTemp, stPLCurrent);
              // Send bytes not supported
              if(mTemp.MemIsEmpty()) Send(mTemp);
              // Done
              break;
            } // Pong! event? Ignore packet data.
            case OC_PONG:
            { // Send pong event
              DispatchEvent(SS_PONG);
              // Clear packet data
              plTemp.clear();
              // Done
              break;
            } // Close the connection? Do it.
            case OC_CLOSE: plTemp.clear(); return TS_OK;
            // Unsupported opcode?
            default: plTemp.clear();
              return SetErrorStaticSafe(
                StrAppend("Invalid opcode 0x", hex, uiOpCode));
          } // Reset to header mode
          uiMode = RM_HEADER;
          // Reset op code function
          uiOpCode = OC_NONE;
          // Reset payload values
          stPLCurrent = stPLTotal = 0;
          // Set required bytes for a header
          stRequired = 2;
          // If we don't have extra data? Wait for more data
          if(!stExtra) continue;
          // Shift data left
          mDest.MemMove(0, mDest.MemRead(stPLPktSize, stExtra), stExtra);
          // Set bytes unprocessed
          stTRead = stExtra;
          // We don't have enough for a header yet? Wait for more
          if(stTRead < stRequired) goto Read;
          // We have enough for a header so set extra data size
          stExtra = stTRead - stRequired;
          // Fall through to process header
          [[fallthrough]];
        } // Waiting for header?
        case RM_HEADER:
        { // Get bits 0 to 7 and 8 to 15
          const unsigned int uiFirst = mDest.MemReadInt<uint8_t>(),
                             uiSecond = mDest.MemReadInt<uint8_t>(1);
          // Throw error if reserved bits are set
          if(uiFirst & 0x40 || uiFirst & 0x20 || uiFirst & 0x10)
            return SetErrorStaticSafe("Reserved headers set!");
          // Server should not send a mask (bit 8)
          if(uiSecond & 0x80) return SetErrorStaticSafe("Mask bit set!");
          // Set if a final packet (bit 0)
          bFinal = !!(uiFirst & 0x80);
          // Set the opcode (bit 4-7)
          uiOpCode = static_cast<OpCode>(uiFirst & 0x0F);
          // Get initial payload value (bits 9 to 15)
          stPLTotal = uiSecond & 0x7F;
          // Compare payload value
          switch(stPLTotal)
          { // Payload is small? (126 bytes)
            case 126:
            { // Set small payload expected next
              uiMode = RM_SMALL_PAYLOAD;
              // Need two bytes next
              const size_t stRequiredNext = 2;
              // If we don't have extra data? Wait for more data
              if(!stExtra) { stRequired = stRequiredNext; continue; }
              // Shift data left
              mDest.MemMove(0, mDest.MemRead(stRequired, stExtra), stExtra);
              // Set next requirement
              stRequired = stRequiredNext;
              // Set bytes unprocessed
              stTRead = stExtra;
              // We don't have enough for a header yet? Wait for more
              if(stTRead < stRequired) goto Read;
              // We have enough for a header so set extra data size
              stExtra = stTRead - stRequired;
              // Process small payload immediately
              goto SmallPayload;
            } // Payload is large? (127 bytes)
            case 127:
            { // Set large payload expected next
              uiMode = RM_LARGE_PAYLOAD;
              // Need eight bytes next
              const size_t stRequiredNext = 8;
              // If we don't have extra data? Wait for more data
              if(!stExtra) { stRequired = stRequiredNext; continue; }
              // Shift data left
              mDest.MemMove(0, mDest.MemRead(stRequired, stExtra), stExtra);
              // Set next requirement
              stRequired = stRequiredNext;
              // Set bytes unprocessed
              stTRead = stExtra;
              // We don't have enough for a header yet? Wait for more
              if(stTRead < stRequired) goto Read;
              // We have enough for a header so set extra data size
              stExtra = stTRead - stRequired;
              // Process large payload immediately
              goto LargePayload;
            } // Payload is tiny? (<126 bytes) break to process payload
            default: break;
          } // Done
          break;
        } // Waiting for small payload size?
        case RM_SMALL_PAYLOAD: SmallPayload:
        { // Update actual payload size
          stPLTotal = static_cast<size_t>((mDest.MemReadInt<uint8_t>(0) << 8) |
                                           mDest.MemReadInt<uint8_t>(1));
          // Now process payload
          break;
        } // Waiting for large payload size?
        case RM_LARGE_PAYLOAD: LargePayload:
        { // Calculate 64-bit size of payload
          uint64_t uqPayload = 0;
          for(size_t stIndex = 0; stIndex < 8; ++stIndex)
            uqPayload = (uqPayload << 8) |
              mDest.MemReadInt<uint8_t>(stIndex);
          // Let's be sensible and deny above 4GB payloads for now
          if(uqPayload > 0xFFFFFFFF)
            return SetErrorStaticSafe(
              StrAppend("Payload $ too big", hex, uqPayload));
          // Now process payload
          break;
        } // Should never get here
        default: return SetErrorStaticSafe("Invalid mode!");
      } // Set payload mode
      uiMode = RM_PAYLOAD;
      // Expecting the payload size next
      const size_t stRequiredNext = stPLTotal;
      // If we don't have extra data? Wait for more data
      if(!stExtra) { stRequired = stRequiredNext; continue; }
      // Shift data left
      mDest.MemMove(0, mDest.MemRead(stRequired, stExtra), stExtra);
      // Set next requirement
      stRequired = stRequiredNext;
      // Set bytes unprocessed
      stTRead = stExtra;
      // We don't have enough for a header yet? Wait for more
      if(stTRead < stRequired) goto Read;
      // We have enough for a header so set extra data size
      stExtra = stTRead - stRequired;
      // Process actual payload data immediately
      goto Payload;
    } // Thread should terminate
    return TS_OK;
  }
  /* -- HTTP Socket main thread function ----------------------------------- */
  ThreadStatus HTTPMain(void)
  { // Connect and send http request and break loop if failed.
    if(InitialConnect() == TS_ERROR) return TS_ERROR;
    // Check if websocket
    enum { HTTP, HTTP_HEAD, WEBSOCKET } eMode;
    { // Check for websocket key and if we have it
      StrNCStrMapIt sncsmiIt{ GetRegistryIterator("Sec-WebSocket-Key") };
      if(sncsmiIt != pRegistry.cend())
      { // Create base 64 key
        sncsmiIt->second = CryptMBtoB64(CryptRandomBlock(16));
        // Set mode
        eMode = WEBSOCKET;
        // Remove request method from registry so it doesn't accidentally get
        // sent as a header.
        sncsmiIt = GetRegistryIterator(cParent->strRegVarMETHOD);
        if(sncsmiIt != pRegistry.cend()) pRegistry.erase(sncsmiIt);
      } // Set normal HTTP request or HEAD request
      else eMode = GetRegistry(cParent->strRegVarMETHOD) == "HEAD" ?
                     HTTP_HEAD : HTTP;
    }
    // Set sending request status event
    AddStatus(SS_SENDREQUEST);
    { // Get first line request and body which will also be deleted from the
      // map leaving only the list of headers that are to be sent. Careful when
      // trying to optimise/one-line this as MSVC compiler WILL evaluate
      // expressions in the opposite direction.
      const string
        strReq{
          StdMove(GetRegistry(cParent->strRegVarREQ)) },
        strBody{
          StdMove(GetRegistry(cParent->strRegVarBODY)) },
        strHdrs{
          StdMove(pRegistry.ParserImplodeEx(": ", cCommon->CommonCrLf())) },
        strPk{
          StdMove(StrAppend(strReq, strHdrs, cCommon->CommonCrLf(),
            strBody)) };
      // Write the full request to the server and return if failed
      if(SockWrite(strPk) == static_cast<size_t>(TS_ERROR)) return TS_ERROR;
    } // Set sent request status event
    AddStatus(SS_REPLYWAIT);
    // Content read and content-length
    size_t stContentRead = 0, stContentLength = 0;
    // Reserve memory for response headers
    string strHeaders; strHeaders.reserve(1024);
    // Allocate memory for read buffer
    Memory mDest{ cParent->stBufferSize };
    // Expecting reponse headers? and connection closed status
    bool bHeaders = true;
    // Begin monitoring for reply and break if thread should exit
    while(tReader.ThreadShouldNotExit())
    { // Wait for data from connected server
      const size_t stRead = SockRead(mDest.MemPtr<char>(), mDest.MemSize());
      // Connection error or server closed connection?
      if(stRead == static_cast<size_t>(TS_ERROR))
      { // If we were waiting for headers still?
        if(bHeaders) return SetErrorSafe("Response failed");
        // We were downloading so if there was a content length?
        if(stContentLength)
        { // Read the correct number of bytes? Or we're just doing a HEAD req?
          // Log that the download was successful.
          if(stContentRead == stContentLength || eMode == HTTP_HEAD)
            SocketLogSafe(LH_DEBUG, "Download successful");
          // We did not get the correct number of bytes? Set error code.
          else return SetErrorSafe(StrAppend("Failed at ", stContentRead));
        } // There was no content length? Just log the bytes downloaded
        else SocketLogSafe(LH_DEBUG, "$ downloaded", stContentRead);
        // We're done with the connection
        return TS_ERROR;
      } // Not processing headers?
      if(!bHeaders)
      { // Not processing headers? Processing content? Increment content read
        stContentRead += stRead;
        // Push data into RX list. Truncate bytes read if we have a content
        // length and the we read past the content length.
        PushDataSafe(plRX, stRX, mDest.MemPtr<char>(),
          stContentLength && stContentRead > stContentLength ?
            stRead - static_cast<size_t>(stContentLength - stContentRead) :
            stRead);
        // Have content length and at EOF? Log it
        if(stContentLength && stContentRead == stContentLength)
        { // All downloaded
          SocketLogSafe(LH_DEBUG, "Download complete");
          // So below scopes can jump here
          return TS_OK;
        } // Wait for next packet, thread abort or server disconnect.
        continue;
      } // Make string view of response which could contain binary chars
      string_view strvResp{ mDest.MemPtr<char>(), stRead };
      // Find end of headers marker and if we do not have it yet?
      const size_t stEnd = strvResp.find(cCommon->CommonCrLf2());
      if(stEnd == StdNPos)
      { // Check for binary data and if we found binary data? Bail out!
        if(!ValidHeaderPacket(strvResp))
          return SetErrorStaticSafe("Binary code in headers");
        // Add to full headers string
        strHeaders += strvResp;
        // Wait for next packet
        continue;
      } // Ok we got the headers. Collect data.
      bHeaders = false;
      // Get cut off point between headers to data and if we got it?
      const size_t stInitial = strvResp.length() - (stEnd + 4);
      if(stInitial > 0)
      { // Push data into RX list
        PushDataSafe(plRX, stRX, mDest.MemRead(stEnd+4), stInitial);
        // Increment content read
        stContentRead += stInitial;
        // Truncate extra bytes
        strvResp = { mDest.MemPtr<char>(), stEnd };
      } // Check for binary code in the last packet returned? Bail out!
      if(!ValidHeaderPacket(strvResp))
        return SetErrorStaticSafe("Binary code in headers");
      // Add rest of response to headers
      strHeaders += strvResp;
      // Build output headers list by exploding header string
      pRegistry.ParserReInit(strHeaders, cCommon->CommonCrLf(), ':');
      if(pRegistry.empty()) return SetErrorStaticSafe("No response");
      // Done with the headers string
      strHeaders.clear();
      strHeaders.shrink_to_fit();
      // Find initial reponse (should be #0 set by VARS class)
      const StrNCStrMapConstIt
        vlR{ GetRegistryIterator(cParent->strRegVarRESPONSE) };
      if(vlR == pRegistry.cend()) return SetErrorStaticSafe("Bad response");
      // Split into words. We should have got at least three words
      const Token tWords{ vlR->second, cCommon->CommonSpace() };
      if(tWords.size() < 3) return SetErrorStaticSafe("Unknown response");
      // Get protocol and if it is not valid?
      const string &strProtoRecv = tWords.front();
      if(strProtoRecv != "HTTP/1.0" && strProtoRecv != "HTTP/1.1")
        return SetErrorStaticSafe(StrFormat("Bad protocol '$'", strProtoRecv));
      // Get http status code string and if not a valid number?
      const string &strStatus = tWords[1];
      if(!StrIsInt(strStatus))
        return SetErrorStaticSafe(StrFormat("Bad status '$'", strStatus));
      // Convert to integer and if valid?
      const size_t stStatus = StrToNum<size_t>(strStatus);
      if(stStatus < 100 || stStatus > 999)
        return SetErrorStaticSafe(StrAppend("Bad status code ", strStatus));
      // If the status code is anything but an error? Log successful code
      if(stStatus < 400) SocketLogSafe(LH_DEBUG, "Status code $", strStatus);
      // Error status code? Log error status code
      else SocketLogSafe(LH_WARNING, "Status error $", strStatus);
      // If connection upgrade required? Handle websocket if requested else
      // throw an error because the socket might be left in a waiting state
      // which our client expects the server to close the connection.
      if(stStatus == 101)
        return eMode == WEBSOCKET ? WebSocketMain(mDest):
          SetErrorStaticSafe("Not upgrading");
      // Add protocol and status code to registry so guest can read them
      // without having to perform any special string operations
      pRegistry.ParserPushOrUpdatePair(cParent->strRegVarPROTO, strProtoRecv);
      pRegistry.ParserPushOrUpdatePair(cParent->strRegVarCODE, strStatus);
      { // We have to lock the TX list since a LUA function can read this
        const LockGuard lgSocketSync{ mMutex };
        for(const StrNCStrMapPair &sncsmpPair : pRegistry)
        { // Get items and push into TX since we're not using it anymore. Make
          // sure to include the null-terminator so we can use string_view for
          // when LUA grabs the list.
          PushData(plTX, stTX,
            StrToLowCaseRef(UtilToNonConst(sncsmpPair.first)).c_str(),
            sncsmpPair.first.size()+1);
          PushData(plTX, stTX, sncsmpPair.second.c_str(),
            sncsmpPair.second.size()+1);
        }
      } // If we got a content type?
      const StrNCStrMapConstIt sncsmciType{
        GetRegistryIterator("content-type") };
      if(sncsmciType != pRegistry.cend())
        SocketLogSafe(LH_DEBUG, "Type is $", sncsmciType->second);
      // Should get content length
      const StrNCStrMapConstIt sncsmciLen{
        GetRegistryIterator("content-length") };
      if(sncsmciLen != pRegistry.cend())
      { // Get reference to string and if it's not valid?
        const string &strVal = sncsmciLen->second;
        if(!StrIsInt(strVal))
        { // Assume zero, safe to continue and log the warning
          stContentLength = 0;
          SocketLogSafe(LH_WARNING, "Invalid content length");
        } // Valid content-length
        else
        { // Convert length string to integer and log the length
          stContentLength = StrToNum<size_t>(strVal);
          SocketLogSafe(LH_DEBUG, "Length is $", stContentLength);
        } // Set downloading status
        AddStatus(SS_DOWNLOADING);
        // If we already got enough bytes from the header packet?
        if(stInitial == stContentLength)
          SocketLogSafe(LH_DEBUG, "Downloaded in one go");
        // If we got too many bytes? treat it as completed anyway
        else if(stInitial > stContentLength)
          SocketLogSafe(LH_DEBUG, "Downloaded ($ excess)",
            stInitial-stContentLength);
        // Keep waiting for data
        else continue;
        // Success
        return TS_OK;
      } // No content length and connection not closed
      else
      { // Set zero content length and downloading flag
        stContentLength = 0;
        AddStatus(SS_DOWNLOADING);
      } // Wait for next packet, thread abort or server disconnect
    } // Got here because the thread was aborted
    return SetAborted();
  }
  /* -- Socket http connection --------------------------------------------- */
  ThreadStatus SocketHTTPThreadMain(Thread &)
  { // Return code
    ThreadStatus tsReturn;
    // Capture exceptions and switch to thiscall
    try { tsReturn = HTTPMain(); }
    // Exception occurred?
    catch(const exception &eReason)
    { // Report error
      cLog->LogErrorExSafe("(SOCKET HTTP THREAD EXCEPTION) $", eReason);
      // Set error message
      tsReturn = SetErrorStaticSafe(eReason.what());
    } // If we're a websocket then stop the writer thread
    SocketTerminateWriteThread();
    // Clear connection and clean-up
    SendDisconnect();
    FinishDisconnect();
    // Required to stop memory leak
    OPENSSL_thread_stop();
    // Return status
    return tsReturn;
  }
  /* -- Return if there are TX packets available --------------------------- */
  bool IsTXPacketAvailable(void)
    { const LockGuard lgSocketSync{ mMutex }; return !plTX.empty(); }
  /* -- Get memory to oldest TX packet ------------------------------------- */
  const MemConst &GetOldestTXPacketSafe(void)
    { const LockGuard lgSocketSync{ mMutex }; return plTX.front().mData; }
  /* -- Pop oldest TX packet ----------------------------------------------- */
  void PopOldestTXPacketSafe(void)
    { const LockGuard lgSocketSync{ mMutex }; return plTX.pop_front(); }
  /* -- Socket write manager ----------------------------------------------- */
  ThreadStatus SockWebWriteManager(void)
  { // Block until requested to exit
    while(tWriter.ThreadShouldNotExit())
    { // For each packet waiting to be written
      while(IsTXPacketAvailable())
      { // Get oldest available TX packet
        const MemConst &mcPacket = GetOldestTXPacketSafe();
        // Calculate payload size and if one byte?
        Memory mPayload;
        // What is the size of the memory
        switch(mcPacket.MemSize())
        { // Empty?
          case 0:
            // Send PONG packet
            mPayload.MemInitSafe(6);
            mPayload.MemWriteInt<uint8_t>(0, 0x80 | OC_PONG);
            mPayload.MemWriteInt<uint8_t>(1, 0x80);
            // Proceed to write the packet
            break;
          // One byte?
          case 1:
            // Treat as normal packet if is value zero because who needs to
            // send a single null byte to a websocket? We'll reuse it as PING.
            if(mcPacket.MemReadInt<uint8_t>(0)) goto SmallPacket;
            // Send PING packet
            mPayload.MemInitSafe(6);
            mPayload.MemWriteInt<uint8_t>(0, 0x80 | OC_PING);
            mPayload.MemWriteInt<uint8_t>(1, 0x80);
            // Proceed to write the packet
            break;
          // Anything else?
          default: SmallPacket:
            // Typedef for mask data
            typedef array<uint8_t,4> MaskData;
            // Small payload size?
            if(mcPacket.MemSize() < 125)
            { // Setup header
              mPayload.MemInitBlank(6 + mcPacket.MemSize());
              mPayload.MemWriteInt<uint8_t>(0, 0x80 | OC_TEXT);
              mPayload.MemWriteInt<uint8_t>(1, 0x80 |
                mcPacket.MemSize<uint8_t>());
              // Setup random masking key
              const MaskData ucaMask{ CryptRandom<MaskData>() };
              mPayload.MemWrite(2, ucaMask.data(), ucaMask.size());
              // Mask original packet data into the payload
              for(size_t stIndex = 0; stIndex < mcPacket.MemSize(); ++stIndex)
                mPayload.MemWriteInt<uint8_t>(6 + stIndex,
                  mcPacket.MemReadInt<uint8_t>(stIndex) ^
                    ucaMask[stIndex % 4]);
            } // Medium payload size?
            else if(mcPacket.MemSize() <= 65535)
            { // Setup header
              mPayload.MemInitBlank(8 + mcPacket.MemSize());
              mPayload.MemWriteInt<uint8_t>(0, 0x80 | OC_TEXT);
              mPayload.MemWriteInt<uint8_t>(1, 0x80 | 126);
              mPayload.MemWriteInt<uint8_t>(2,
                (mcPacket.MemSize() >> 8) & 0xFF);
              mPayload.MemWriteInt<uint8_t>(3, mcPacket.MemSize() & 0xFF);
              // Setup random masking key
              const MaskData ucaMask{ CryptRandom<MaskData>() };
              mPayload.MemWrite(4, ucaMask.data(), ucaMask.size());
              // Mask original packet data into the payload
              for(size_t stIndex = 0; stIndex < mcPacket.MemSize(); ++stIndex)
                mPayload.MemWriteInt<uint8_t>(8 + stIndex,
                  mcPacket.MemReadInt<uint8_t>(stIndex) ^
                    ucaMask[stIndex % 4]);
            } // Large payload size?
            else
            { // Setup header
              mPayload.MemInitBlank(13 + mcPacket.MemSize());
              mPayload.MemWriteInt<uint8_t>(0, 0x80 | OC_TEXT);
              mPayload.MemWriteInt<uint8_t>(1, 0x80 | 127);
              for(size_t stIndex = 7;
                         stIndex != static_cast<size_t>(-1);
                       --stIndex)
                mPayload.MemWriteInt<uint8_t>((mcPacket.MemSize() >>
                  (9 * stIndex)) & 0xFF);
              // Setup random masking key
              const MaskData ucaMask{ CryptRandom<MaskData>() };
              mPayload.MemWrite(9, ucaMask.data(), ucaMask.size());
              // Mask original packet data into the payload
              for(size_t stIndex = 0; stIndex < mcPacket.MemSize(); ++stIndex)
                mPayload.MemWriteInt<uint8_t>(13 + stIndex,
                  mcPacket.MemReadInt<uint8_t>(stIndex) ^
                    ucaMask[stIndex % 4]);
            } // Done
            break;
        } // Send it, and kill thread on error
        if(SockWrite(mPayload) == static_cast<size_t>(TS_ERROR))
          return TS_ERROR;
        // Subtract total bytes counter and pop the packet we just sent
        stTX -= mcPacket.MemSize();
        PopOldestTXPacketSafe();
      } // Setup lock for condition variable and wait for new data to write
      UniqueLock uLock{ mWriter };
      cvWriter.wait(uLock,
        [this]{ return bUnlock || tReader.ThreadShouldExit(); });
      // Next wait will block
      bUnlock = false;
    } // Sucesss
    return TS_OK;
  }
  /* -- WebSocket write thread --------------------------------------------- */
  ThreadStatus SockWebWriteThreadMain(Thread &)
  { // Return code
    ThreadStatus tsReturn;
    // Capture exceptions and execute the manager
    try { tsReturn = SockWebWriteManager(); }
    // exception occured?
    catch(const exception &eReason)
    { // Report error
      cLog->LogErrorExSafe("(WEBSOCKET WRITE THREAD EXCEPTION) $", eReason);
      // Set error message
      tsReturn = SetErrorStaticSafe(eReason.what());
    } // Force close the socket if the reader thread isn't already exiting
    SendDisconnect();
    // Required to stop memory leak
    OPENSSL_thread_stop();
    // Return exit code
    return tsReturn;
  }
  /* -- Socket write manager ----------------------------------------------- */
  ThreadStatus SockWriteManager(void)
  { // Block until requested to exit
    while(tWriter.ThreadShouldNotExit())
    { // For each packet waiting to be written
      while(IsTXPacketAvailable())
      { // Get oldest available TX packet, send it, and kill thread on error
        const MemConst &mcPacket = GetOldestTXPacketSafe();
        if(SockWrite(mcPacket) == static_cast<size_t>(TS_ERROR))
          return TS_ERROR;
        // Subtract total bytes counter and pop the packet we just sent
        stTX -= mcPacket.MemSize();
        PopOldestTXPacketSafe();
      } // Setup lock for condition variable and wait for new data to write
      UniqueLock uLock{ mWriter };
      cvWriter.wait(uLock,
        [this]{ return bUnlock || tReader.ThreadShouldExit(); });
      // Next wait will block
      bUnlock = false;
    } // Sucesss
    return TS_OK;
  }
  /* -- Socket write thread ------------------------------------------------ */
  ThreadStatus SockWriteThreadMain(Thread &)
  { // Return code
    ThreadStatus tsReturn;
    // Capture exceptions and execute the manager
    try { tsReturn = SockWriteManager(); }
    // exception occured?
    catch(const exception &eReason)
    { // Report error
      cLog->LogErrorExSafe("(SOCKET WRITE THREAD EXCEPTION) $", eReason);
      // Set error message
      tsReturn = SetErrorStaticSafe(eReason.what());
    } // Force close the socket if the reader thread isn't already exiting
    SendDisconnect();
    // Required to stop memory leak
    OPENSSL_thread_stop();
    // Return exit code
    return tsReturn;
  }
  /* -- Dispatch an event -------------------------------------------------- */
  template<typename ...VarArgs>
    void DispatchEvent(const SocketFlagsConst &evtId, const VarArgs &...vaArgs)
  { // Signal events handler to execute event callback on the next frame.
    // Add the event to the event store so we can remove the event when the
    // destructor is called before the event is called.
    LuaEvtDispatch(evtId.FlagGet(), vaArgs...);
  }
  /* -- Socket read manager ------------------------------------------------ */
  ThreadStatus SockReadManager(void)
  { // Create a thread to write data requests
    tWriter.ThreadInit(StrAppend("socketwriter:", CtrGet()),
      bind(&Socket::SockWriteThreadMain, this, _1), this);
    // Try to connect and if it didn't fail kill the thread
    if(InitialConnect() == TS_ERROR) return TS_ERROR;
    // Create read transfer buffer
    Memory mDest{ cParent->stBufferSize };
    // Loop until thread should terminate
    while(tReader.ThreadShouldNotExit())
    { // Wait for new data to be read and kill thread on error
      const size_t stRead = SockRead(mDest.MemPtr<char>(), mDest.MemSize());
      if(stRead == static_cast<size_t>(TS_ERROR)) return TS_ERROR;
      // Push data block into list ready for LUA to collect
      PushDataSafe(plRX, stRX, mDest.MemPtr<char>(), stRead);
      // Send read event
      DispatchEvent(SS_READPACKET);
    } // Thread should terminate
    return TS_OK;
  }
  /* -- Socket read thread ------------------------------------------------- */
  ThreadStatus SockReadThreadMain(Thread &)
  { // Return code
    ThreadStatus tsReturn;
    // Capture exceptions
    try { tsReturn = SockReadManager(); }
    // Exception occured?
    catch(const exception &eReason)
    { // Report error
      cLog->LogErrorExSafe("(SOCKET THREAD EXCEPTION) $", eReason);
      // Set error message
      tsReturn = SetErrorStaticSafe(eReason.what());
    } // Stop writer thread
    SocketTerminateWriteThread();
    // Clear connection and clean-up
    SendDisconnect();
    FinishDisconnect();
    // Required to stop memory leak
    OPENSSL_thread_stop();
    // Return status
    return tsReturn;
  }
  /* -- Error occured in event so start cleaning up ------------------------ */
  void EventError(void)
  { // Add error flag
    FlagSet(SS_EVENTERROR);
    // Disconnect the socket and clean up
    SendDisconnectAndWait();
    // Clear the references and state so no more exceptions/events occur
    LuaEvtDeInit();
  }
  /* -- Update file descriptor --------------------------------------------- */
  bool UpdateDescriptor(void)
  { // Update descriptor
    iFd = CryptBIOGetFd(bioPtr);
    // Return if succeeded
    return iFd != -1;
  }
  /* ----------------------------------------------------------------------- */
  const string &GetAddressAndPort(void) const { return strAddrPort; }
  const string GetIPAddressAndPort(void) const
    { return StrAppend(GetIPAddress(), ':', GetPort()); }
  const string &GetErrorStr(void) const { return strError; }
  /* ----------------------------------------------------------------------- */
  template<typename AnyType>const AnyType GetVarSafe(const AnyType &atVar)
    { const LockGuard lgSocketSync{ mMutex }; return atVar; }
  /* ----------------------------------------------------------------------- */
  double GetPacketXSafe(Memory &mbD, PacketList &plList, size_t &stX)
  { // Synchronise access to packet list
    const LockGuard lgSocketSync{ mMutex };
    // Not empty? Return top memory block else through error
    if(plList.empty())
      XC("No packets remaining in blocklist!",
         "Address", strAddr, "Port", uiPort);
    // Get last packet and return time
    return GetPacket(mbD, plList, stX);
  }
  /* ----------------------------------------------------------------------- */
  size_t GetXQCountSafe(const PacketList &plList)
    { const LockGuard lgSocketSync{ mMutex };
      return plList.size(); }
  void CompactXSafe(Memory &mbD, PacketList &plList, size_t &stX)
    { const LockGuard lgSocketSync{ mMutex };
      Compact(mbD, plList, stX); }
  /* -- Events status ------------------------------------------------------ */
  bool IsConnected(void) const { return FlagIsSet(SS_CONNECTED); }
  bool IsDisconnected(void) const { return FlagIsSet(SS_STANDBY); }
  bool IsDisconnecting(void) const { return FlagIsSet(SS_DISCONNECTING); }
  bool IsDisconnectingOrDisconnected(void) const
    { return IsDisconnecting() || IsDisconnected(); }
  bool IsDisconnectedByClient(void) { return FlagIsSet(SS_CLOSEDBYCLIENT); }
  /* --------------------------------------------------------------- */ public:
  bool IsSecure(void) const { return FlagIsSet(SS_ENCRYPTION); }
  int GetFD(void) const { return iFd; }
  int GetError(void) const { return iError; }
  /* -- Connection data ---------------------------------------------------- */
  const string &GetAddress(void) const { return strAddr; }
  const string GetAddressSafe(void) { return GetVarSafe(GetAddress()); }
  const unsigned int &GetPort(void) const { return uiPort; }
  unsigned int GetPortSafe(void) { return GetVarSafe(GetPort()); }
  const string &GetIPAddress(void) const { return strIP; }
  const string GetIPAddressSafe(void) { return GetVarSafe(GetIPAddress()); }
  const string GetAddressAndPortSafe(void)
    { return GetVarSafe(GetAddressAndPort()); }
  const string GetIPAddressAndPortSafe(void)
    { return GetVarSafe(GetIPAddressAndPort()); }
  const string &GetRealHost(void) const { return strRealHost; }
  const string GetRealHostSafe(void) { return GetVarSafe(GetRealHost()); }
  const string &GetCipher(void) const { return strCipher; }
  const string GetCipherSafe(void) { return GetVarSafe(GetCipher()); }
  const string GetErrorStrSafe(void) { return GetVarSafe(GetErrorStr()); }
  /* -- RX packets --------------------------------------------------------- */
  uint64_t GetRX(void) const { return qRX; }
  uint64_t GetRXpkt(void) const { return qRXp; }
  size_t GetRXQCount(void) const { return plRX.size(); }
  size_t GetRXQCountSafe(void) { return GetXQCountSafe(plRX); }
  double GetPacketRXSafe(Memory &mbD)
    { return GetPacketXSafe(mbD, plRX, stRX); }
  void CompactRXSafe(Memory &mbD) { CompactXSafe(mbD, plRX, stRX); }
  /* -- TX packets --------------------------------------------------------- */
  uint64_t GetTX(void) const { return qTX; }
  uint64_t GetTXpkt(void) const { return qTXp; }
  size_t GetTXQCount(void) const { return plTX.size(); }
  size_t GetTXQCountSafe(void) { return GetXQCountSafe(plTX); }
  double GetPacketTXSafe(Memory &mbD)
    { return GetPacketXSafe(mbD, plTX, stTX); }
  void CompactTXSafe(Memory &mbD) { CompactXSafe(mbD, plTX, stTX); }
  /* ----------------------------------------------------------------------- */
  ThreadStatus SetErrorSafe(const string &strS)
    { const LockGuard lgSocketSync{ mMutex };
      return SetError(strS); }
  ThreadStatus SetErrorStaticSafe(const string &strS, const bool bS=true)
    { const LockGuard lgSocketSync{ mMutex };
      return SetErrorStatic(strS, bS); }
  void PushDataSafe(PacketList &blD, size_t &stX, const char *cpData,
    const size_t stS)
      { const LockGuard lgSocketSync{ mMutex };
        PushData(blD, stX, cpData, stS); }
  void SendSafe(const MemConst &mcPacket)
    { const LockGuard lgSocketSync{ mMutex }; Send(mcPacket); }
  void SendStringSafe(const string &strData)
    { const LockGuard lgSocketSync{ mMutex }; SendString(strData); }
  /* -- Get timers --------------------------------------------------------- */
  const ClkTimePoint GetTConnect(void) const
    { return ClkTimePoint{ cdConnect }; }
  const ClkTimePoint GetTConnected(void) const
    { return ClkTimePoint{ cdConnected }; }
  const ClkTimePoint GetTRead(void) const
    { return ClkTimePoint{ cdRead }; }
  const ClkTimePoint GetTWrite(void) const
    { return ClkTimePoint{ cdWrite }; }
  const ClkTimePoint GetTDisconnect(void) const
    { return ClkTimePoint{ cdDisconnect }; }
  const ClkTimePoint GetTDisconnected(void) const
    { return ClkTimePoint{ cdDisconnected }; }
  /* -- Set a new callback ------------------------------------------------- */
  void SetNewCallback(lua_State*const lS) { LuaEvtInitEx(lS, 1); }
  /* -- Async thread event callback (called by LuaEvtMaster) ------------- */
  void LuaEvtCallbackAsync(const EvtMainEvent &emeEvent) try
  { // Get reference to string vector and we need three parameters
    // [0]=Pointer to socket class, [1]=Event list id, [2]=Status
    const EvtMainArgs &emaArgs = emeEvent.eaArgs;
    // Remove iterator from our events dispatched list if we can
    if(LuaEvtsCheckParams<3>(emaArgs))
    { // Get the status and warn if we have incorrect number of parameters
      const unsigned int uiStatus = emaArgs[2].UInt();
      // Lua is not paused?
      if(!uiLuaPaused)
      { // Push function callback onto stack
        if(LuaRefGetFunc(1))
        { // Push class reference onto stack
          if(LuaRefGetUData())
          { // Push the status code that was fired and if valid?
            LuaUtilPushInt(lsState, uiStatus);
            if(LuaUtilIsInteger(lsState, -1))
            { // Call callback
              LuaUtilCallFuncTogglerEx(lsState, this, 2);
              // Clear references and state if this is the last event
              if(uiStatus == SS_STANDBY.FlagGet()) LuaEvtDeInit();
              // Success
              return;
            } // Not a valid integer so debug it
            else SocketLogSafe(LH_ERROR,
              "Invalid integer\n$", LuaUtilGetVarStack(lsState));
          } // Unknown class?
          else SocketLogSafe(LH_ERROR,
            "Invalid class\n$", LuaUtilGetVarStack(lsState));
        } // Unknown function callback?
        else SocketLogSafe(LH_ERROR,
          "Invalid callback\n$", LuaUtilGetVarStack(lsState));
      } // Lua is paused? Log this just to know this event was ignored
      else SocketLogSafe(LH_WARNING,
        "Ignoring event $=$$!", emeEvent.cCmd, hex, uiStatus);
    } // Not enough parameters so log error
    else SocketLogSafe(LH_ERROR,
      "Not enough event params ($)", emaArgs.size());
    // Done
    return EventError();
  } // Exception occured? Cleanup and rethrow exception
  catch(const exception&) { EventError(); throw; }
  /* -- Send request to disconnect ----------------------------------------- */
  bool SendDisconnectAndWait(void)
  { // Send disconnect to socket
    SendDisconnect();
    // Have read thread running?
    if(tReader.ThreadIsJoinable())
    { // Tell the thread to stop and wait for it. The end of the thread should
      // call FinishDisconnect() already.
      tReader.ThreadDeInit();
      // Success
      return true;
    } // Cleanup the disconnect
    FinishDisconnect();
    // Nothing closed really
    return false;
  }
  /* -- Get connection status ---------------------------------------------- */
  void AddStatus(const SocketFlagsConst &ssNS)
  { // Ignore if this status is already set
    if(FlagIsSet(ssNS)) return;
    // Add status flag and send event to lua event callback
    FlagSet(ssNS);
    // Send as normal if not finishing up
    DispatchEvent(ssNS);
  }
  /* -- Get connection status and update a timestamp ----------------------- */
  void AddStatus(const SocketFlagsConst &ssNS, SafeClkDuration &duDest)
  { // Set the timestamp. We cannot have an atomic<ClkTimePoint> so we have to
    // store the time point as a duration instead. :-(
    duDest = cmHiRes.GetEpochTime();
    // Set the event
    AddStatus(ssNS);
  }
  /* -- Convert socket headers list to a table ----------------------------- */
  void HeadersToTable(lua_State*const lS)
  { // This function can only be called if we don't have a write thread
    if(tWriter.ThreadIsJoinable()) return;
    // Lock access to packet list
    const LockGuard lgSocketSync{ mMutex };
    // Return if there are packets and divisble by 2
    const size_t stCount = GetTXQCount();
    if(!stCount || stCount % 2) return LuaUtilPushTable(lS);
    // Create the table, we're creating non-indexed key/value pairs
    LuaUtilPushTable(lS, 0, stCount / 2);
    // Currently selected variable
    const char *cpVar = nullptr;
    // For each packet
    for(const Packet &pData : plTX)
    { // Get variable or value (both are already safely null-terminated)
      const Memory &mbPacket = pData.mData;
      // Set the variable and continue if we haven't set it yet
      if(!cpVar) { cpVar = mbPacket.MemPtr<char>(); continue; }
      // Push value and set it as the variable
      LuaUtilPushStrView(lS, mbPacket.MemToStringView());
      LuaUtilSetField(lS, -2, cpVar);
      // Done with variable
      cpVar = nullptr;
    } // Clear data and memory usage in queue
    plTX.clear();
    stTX = 0;
  }
  /* -- Send request to disconnect ----------------------------------------- */
  bool SendDisconnect(void)
  { // Ignore if already disconnecting
    if(IsDisconnectingOrDisconnected()) return false;
    // If the connection was closed by the server then it's a clean exit
    SocketLogSafe(LH_DEBUG, "Disconnecting...");
    // Disconnecting
    AddStatus(SS_DISCONNECTING, cdDisconnect);
    // Lock access to packet list
    const LockGuard lgSocketSync{ mMutex };
    // If we have a BIO and there is no fd? (i.e. stuck in BIO_do_connect)
    if(bioPtr && iFd == -1) UpdateDescriptor();
    // If socket is open?
    if(iFd != -1)
    { // Closed by us if not closed by server
      if(FlagIsClear(SS_CLOSEDBYSERVER)) FlagSet(SS_CLOSEDBYCLIENT);
      // Force close the socket to unblock recv()
      // This will probably cause a 'system lib' error as well
      BIO_closesocket(iFd);
      // Fd no longer valid
      iFd = -1;
      // We don't care if an error occured
      ERR_clear_error();
    } // Set thread to exit if we are not calling from it
    if(tReader.ThreadIsNotCurrent() && tReader.ThreadIsJoinable())
      tReader.ThreadSetExit();
    // Closing
    return true;
  }
  /* -- Init connection ---------------------------------------------------- */
  void Connect(lua_State*const lS, string &strNAddress,
    const unsigned int uiNPort, const string &strNCipher)
  { // Set address and port and TLS cipher
    SetAddressAndCipher(strNAddress, uiNPort, strNCipher);
    // Init LUA references
    LuaEvtInitEx(lS);
    // Initialise name, thread and start the connection process
    IdentSetA("S:", GetAddressAndPort());
    tReader.ThreadInit(StrAppend("socketreader:", CtrGet()),
      bind(&Socket::SockReadThreadMain, this, _1), this);
  }
  /* -- Init --------------------------------------------------------------- */
  void HTTPRequest(lua_State*const lS, const string &strNCipher,
    string &strNAddress, const unsigned int uiNPort,
    const string &strRequest, string &strMethod, const string &strHeaders,
    const string &strBody)
  { // Request must begin with a forward slash
    if(strRequest.front() != '/')
      XC("Resource is invalid!", "Resource", strRequest);
    // Set address and TLS cipher
    SetAddressAndCipher(strNAddress, uiNPort, strNCipher);
    // Initialise registry with headers
    pRegistry.ParserReInit(strHeaders, cCommon->CommonLf(), ':');
    // Push default user agent if not specified already
    pRegistry.ParserPushIfNotExist("User-Agent", cSockets->strvUserAgent);
    // Check for websocket method
    if(strMethod == "WS")
    { // Set GET mathod back
      strMethod = "GET";
      // Set required websocket headers
      pRegistry.ParserPushOrUpdatePairs({
        // Websocket required headers
        { "Connection", "Upgrade, close" },
        { "Sec-WebSocket-Key", "" },
        { "Sec-WebSocket-Version", "13" },
        { "Upgrade", "websocket" }
      });
    } // Disable keep-alive, we don't support it (yet?).
    else pRegistry.ParserPushPair("Connection", "close");
    // Find if the request contains a bookmark fragment
    const size_t stFrag = strRequest.find('#');
    // Start building registry for connector thread
    pRegistry.ParserPushOrUpdatePairs({
      // Push the source address
      { "Host", strAddrPort },
      // Push the formulated request line. Remove the right hand fragment from
      // the URL if neccesary.
      { cParent->strRegVarREQ, StrAppend(strMethod, ' ',
          (StrUrlEncodeSpaces(stFrag == StdNPos ?
            strRequest : strRequest.substr(0, stFrag))), " HTTP/1.0\r\n") },
      // Push method because we need to check if this is a HEAD request and
      // thus to know when to expect no output.
      { cParent->strRegVarMETHOD, StdMove(strMethod) },
    });
    // Body specified?
    if(!strBody.empty()) pRegistry.ParserPushOrUpdatePairs({
      // Add length of body text
      { "Content-Length", StrFromNum(strBody.length()) },
      // Add body text
      { cParent->strRegVarBODY, StdMove(strBody) }
    });
    // Init LUA references
    LuaEvtInitEx(lS);
    // Start the thread
    IdentSetA("HS:", GetAddressAndPort());
    tReader.ThreadInit(StrAppend("sockethttp:", CtrGet()),
      bind(&Socket::SocketHTTPThreadMain, this, _1), this);
  }
  /* -- Constructor -------------------------------------------------------- */
  Socket(void) :
    /* -- Initialisers ----------------------------------------------------- */
    ICHelperSocket{ cSockets, this },  // Register in collector
    IdentCSlave{ cParent->CtrNext() }, // Initialise identification number
    LuaEvtSlave{ this,EMC_MP_SOCKET }, // Socket async events init
    SocketFlags{ SS_STANDBY },         // Initially on standby
    bioPtr(nullptr),                   // Invalid bio (openssl)
    sslctxPtr(nullptr),                // Invalid ssl context (openssl)
    sslPtr(nullptr),                   // Invalid ssl (openssl)
    qRX(0), qTX(0),                    // No RX or TX bytes
    qRXp(0), qTXp(0),                  // No RX or TX packets
    tReader{ STP_LOW },                // Low priority reader thread
    tWriter{ STP_LOW },                // Low priority writer thread
    bUnlock(false),                    // Block sock writer thread
    uiPort(0),                         // No port
    iError(0),                         // No error
    iFd(-1),                           // Invalid file descriptor
    stRX(0), stTX(0)                   // No RX or TX packets in queue
    /* --------------------------------------------------------------------- */
    { }
  /* -- Destructor --------------------------------------------------------- */
  ~Socket(void)
  { // Send disconnect to socket
    SendDisconnect();
    // Have read thread running? Tell the thread to stop and wait for it. The
    // end of the thread should call FinishDisconnect() already.
    if(tReader.ThreadIsJoinable()) tReader.ThreadStopNoThrow();
    // Cleanup the disconnect
    FinishDisconnect();
  }
};/* ----------------------------------------------------------------------- */
static void DestroyAllSockets(void)
{ // No sockets? Ignore
  if(cSockets->empty()) return;
  // Close all connections and report status
  cLog->LogDebugExSafe("Sockets closing all $ connections...",
    cSockets->size());
  cSockets->CollectorDestroyUnsafe();
  cLog->LogInfoExSafe("Sockets closed all connections [$]!",
    cSockets->size());
}
/* ------------------------------------------------------------------------- */
static void DeInitSockets(void)
{ // Deregister event so callbacks cannot fire
  cEvtMain->Unregister(EMC_MP_SOCKET);
  // Close all socket
  DestroyAllSockets();
}
/* ------------------------------------------------------------------------- */
static void InitSockets(void)
{ // The operating systems sockets API needs to be initialised here because
  // we do not want it initialising during connection in other threads as other
  // connections and threads may not wait for initialisation (*cough* Windows)
  // and just fail. This fixes that so the sockets are ready to use on demand.
  // This could be an OpenSSL bug really where they really should be locking
  // other connections when sockets API intiialisation is needed.
  BIO_sock_init();
}
/* ------------------------------------------------------------------------- */
CTOR_END(Sockets, Socket, SOCKET, InitSockets(), DeInitSockets(),,
  /* -- Initialisers ------------------------------------------------------- */
  LuaEvtMaster{ EMC_MP_SOCKET },       // Setup async socket event
  strRegVarREQ{ "\001" },              // Init reg key name for request data
  strRegVarBODY{ "\002" },             // " for http body data
  strRegVarPROTO{ "\003" },            // " for http protocol data
  strRegVarCODE{ "\004" },             // " for http status code data
  strRegVarMETHOD{ "\005" },           // " for http method string
  strRegVarRESPONSE{ "\255" "0" },     // " for http response string
  strCipherDefault{ "-" },             // Default cipher
  qRX(0), qTX(0),                      // Init received and sent bytes
  qRXp(0), qTXp(0),                    // Init received and sent packets
  stConnected(0)                       // Init sockets connected
) /* ======================================================================= */
static size_t SocketWaitAsync(void)
{ // No sockets? Ignore
  if(cSockets->empty()) return 0;
  // Close all sockets. DON'T destroy them!
  cLog->LogDebugExSafe("Sockets waiting to close $ connections...",
    cSockets->size());
  const ssize_t stClosed = count_if(cSockets->begin(), cSockets->end(),
    [](Socket*const sCptr) { return sCptr->SendDisconnectAndWait(); });
  cLog->LogInfoExSafe("Sockets reset $ of $ connections!",
    stClosed, cSockets->size());
  // Return number of connections reset
  return static_cast<size_t>(stClosed);
}
/* ------------------------------------------------------------------------- */
static size_t SocketReset(void)
{ // No sockets? Ignore
  if(cSockets->empty()) return 0;
  // Close all sockets. DON'T destroy them!
  cLog->LogDebugExSafe("Sockets closing $ connections...",
    cSockets->size());
  const ssize_t stClosed = count_if(cSockets->begin(), cSockets->end(),
    [](Socket*const sCptr) { return sCptr->SendDisconnect(); });
  cLog->LogInfoExSafe("Sockets reset $ of $ connections!",
    stClosed, cSockets->size());
  // Return number of connections reset
  return static_cast<size_t>(stClosed);
}
/* -- OCSP options --------------------------------------------------------- */
static CVarReturn SocketOCSPModified(const int iState)
  { return CVarSimpleSetIntNG(cSockets->iOCSP, iState, 2); }
/* -- Return if cvar can be set (accept only 1024-16384 for now) ----------- */
static CVarReturn SocketSetBufferSize(const size_t stSize)
  { return CVarSimpleSetIntNLG(cSockets->stBufferSize,
      stSize, 4096UL, 1048576UL); }
/* ------------------------------------------------------------------------- */
static CVarReturn SocketSetRXTimeout(const double dNew)
  { return CVarSimpleSetIntNLG(cSockets->dRecvTimeout, dNew, 0, 3600); }
/* ------------------------------------------------------------------------- */
static CVarReturn SocketSetTXTimeout(const double dNew)
  { return CVarSimpleSetIntNLG(cSockets->dSendTimeout, dNew, 0, 3600); }
/* ------------------------------------------------------------------------- */
static CVarReturn SocketSetCipher12(const string&, const string &strV)
  { cSockets->strvCipher12 = strV; return ACCEPT; }
/* ------------------------------------------------------------------------- */
static CVarReturn SocketSetCipher13(const string&, const string &strV)
  { cSockets->strvCipher13 = strV; return ACCEPT; }
/* ------------------------------------------------------------------------- */
static CVarReturn SocketAgentModified(const string &strN, string &strV)
{ // Ignore if string too long
  if(strN.size() > 200) return DENY;
  // If empty? We'll set a default value
  if(strN.empty())
    strV = StrFormat("Mozilla/5.0 ($; $-bit; v$.$.$.$) $/$",
      cSystem->ENGName(), cSystem->ENGBits(), cSystem->ENGMajor(),
      cSystem->ENGMinor(), cSystem->ENGBuild(), cSystem->ENGRevision(),
      cSystem->GetGuestShortTitle(), cSystem->GetGuestVersion());
  // Not empty to use user value instead
  else strV = strN;
  // Set string view
  cSockets->strvUserAgent = strV;
  // We changed the value so return that
  return ACCEPT_HANDLED;
}
/* -- Find socket (Lock the mutex before using) ---------------------------- */
static const SocketsItConst SocketFind(const unsigned int uiId)
  { return StdFindIf(par_unseq, cSockets->cbegin(), cSockets->cend(),
      [uiId](const Socket*const sCptr)
        { return sCptr->CtrGet() == uiId; }); }
/* ------------------------------------------------------------------------- */
static void SocketResetCounters(void)
  { cSockets->qRX = cSockets->qTX = cSockets->qRXp = cSockets->qTXp = 0; }
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
