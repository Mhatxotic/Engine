/* == SOCKHTTP.HPP ========================================================= **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## A basic HTTP(S)/1.0 client that supports custom headers and upgrade ## **
** ## to websocket (sockweb.hpp). It is included inline by socket.hpp.    ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* -- HTTP Socket main thread function ---------------------------- */ private:
ThreadFlagsType HTTPMain()
{ // Connect and send http request and break loop if failed.
  if(InitialConnect() == TS_ERROR) return TS_ERROR;
  // Check if websocket
  enum { HTTP, HTTP_HEAD, WEBSOCKET } eMode;
  { // Check for websocket key and if we have it
    ParserStringVolIt psvciIt{ GetRegistryIterator("Sec-WebSocket-Key") };
    if(psvciIt != psvRegistry.cend())
    { // Create base 64 key
      psvciIt->second.assign(CryptMBtoB64(CryptRandomBlock(16)));
      // Set mode
      eMode = WEBSOCKET;
      // Remove request method from registry so it doesn't accidentally get
      // sent as a header.
      psvciIt = GetRegistryIterator(ctParent.strRegVarMETHOD);
      if(psvciIt != psvRegistry.cend()) psvRegistry.erase(psvciIt);
    } // Set normal HTTP request or HEAD request
    else eMode = GetRegistry(ctParent.strRegVarMETHOD) == "HEAD" ?
                   HTTP_HEAD : HTTP;
  } // Set sending request status event
  AddStatus(SS_SENDREQUEST);
  { // Get resource requested
    const StdString strReq{ StdMove(GetRegistry(ctParent.strRegVarREQ)) };
    // Get body requested
    const StdString strBody{ StdMove(GetRegistry(ctParent.strRegVarBODY)) };
    // Get the request headers to be sent
    const StdString strHdrs{
      StdMove(psvRegistry.ParserImplodeEx(": ", cCommon->CommonCrLf())) };
    // Build entire packet to send
    const StdString strPk{
      StrAppend(strReq, strHdrs, cCommon->CommonCrLf(), strBody) };
    // Write the full request to the server and return if failed
    if(SockWrite(strPk) == static_cast<size_t>(TS_ERROR)) return TS_ERROR;
    // Set sent request status event and log the event
    AddStatus(SS_REPLYWAIT);
    SocketLogSafe(LH_DEBUG, "request of $ bytes sent...",
      strPk.size());
  } // Content read and content-length
  size_t stContentRead = 0, stContentLength = 0;
  // Initialise memory for response headers
  StdReserved<StdString> strHeaders{ 1024 };
  // Allocate memory for read buffer
  Memory mDest{ ctParent.astBufferSize };
  // Expecting reponse headers? and connection closed status
  bool bHeaders = true;
  // Begin monitoring for reply and break if thread should exit
  while(tReader.ThreadShouldNotExit())
  { // Wait for data from connected server
    const size_t stRead = SockRead(mDest.MemPtr<char>(), mDest.MemSize());
    // Connection error or server closed connection?
    if(stRead == static_cast<size_t>(TS_ERROR))
    { // If we were waiting for headers still?
      if(bHeaders) return SetErrorSafe("Response failed!");
      // We were downloading so if there was a content length?
      if(stContentLength)
      { // Read the correct number of bytes? Or we're just doing a HEAD req?
        // Log that the download was successful.
        if(stContentRead == stContentLength || eMode == HTTP_HEAD)
          SocketLogSafe(LH_DEBUG, "download successful.");
        // We did not get the correct number of bytes? Set error code.
        else return
          SetErrorSafe(StrFormat("Download failed at $ bytes!",
            stContentRead));
      } // There was no content length? Just log the bytes downloaded
      else SocketLogSafe(LH_DEBUG, "downloaded $ bytes.", stContentRead);
      // We're done with the connection
      return TS_OK;
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
        SocketLogSafe(LH_DEBUG, "download complete.");
        // So below scopes can jump here
        return TS_OK;
      } // Wait for next packet, thread abort or server disconnect.
      continue;
    } // Make string view of response which could contain binary chars
    StdStringView ssvResp{ mDest.MemPtr<char>(), stRead };
    // Find end of headers marker and if we do not have it yet?
    const size_t stEnd = ssvResp.find(cCommon->CommonCrLf2());
    if(stEnd == StdNPos)
    { // Check for binary data and if we found binary data? Bail out!
      if(!ValidHeaderPacket(ssvResp))
        return SetErrorStaticSafe("Binary code in headers");
      // Add to full headers string
      strHeaders += ssvResp;
      // Wait for next packet
      continue;
    } // Ok we got the headers. Collect data.
    bHeaders = false;
    // Get cut off point between headers to data and if we got it?
    const size_t stInitial = ssvResp.size() - (stEnd + 4);
    if(stInitial > 0)
    { // Push data into RX list
      PushDataSafe(plRX, stRX, mDest.MemRead(stEnd+4), stInitial);
      // Increment content read
      stContentRead += stInitial;
      // Truncate extra bytes
      ssvResp = { mDest.MemPtr<char>(), stEnd };
    } // Check for binary code in the last packet returned? Bail out!
    if(!ValidHeaderPacket(ssvResp))
      return SetErrorStaticSafe("Binary code in headers!");
    // Add rest of response to headers
    strHeaders += ssvResp;
    // Build output headers list by exploding header string
    psvRegistry.ParserReInit(strHeaders, cCommon->CommonCrLf(), ':');
    if(psvRegistry.empty()) return SetErrorStaticSafe("No response!");
    // Done with the headers string
    strHeaders.clear();
    strHeaders.shrink_to_fit();
    // Find initial reponse (should be #0 set by VARS class)
    const StrNCStrMapConstIt
      vlR{ GetRegistryIterator(ctParent.strRegVarRESPONSE) };
    if(vlR == psvRegistry.cend()) return SetErrorStaticSafe("Bad response!");
    // Split into words. We should have got at least three words
    const TokenStrView tsvWords{ vlR->second, cCommon->CommonSpace() };
    if(tsvWords.size() < 3) return SetErrorStaticSafe("Unknown response!");
    // Get protocol and if it is not valid?
    const StdStringView &ssvProtoRecv = tsvWords.front();
    if(ssvProtoRecv != "HTTP/1.0" && ssvProtoRecv != "HTTP/1.1")
      return SetErrorStaticSafe(
        StrFormat("Bad protocol '$'!", ssvProtoRecv));
    // Get http status code string and if not a valid number?
    const StdStringView &ssvStatus = tsvWords[1];
    if(!StrIsInt(ssvStatus))
      return SetErrorStaticSafe(StrFormat("Bad status '$'!", ssvStatus));
    // Convert to integer and if valid?
    const size_t stStatus = StrToNum<size_t>(ssvStatus);
    if(stStatus < 100 || stStatus > 999)
      return SetErrorStaticSafe(
        StrFormat("Bad status code '$'!", ssvStatus));
    // If the status code is anything but an error? Log successful code
    if(stStatus < 400) SocketLogSafe(LH_DEBUG, "status code $.", ssvStatus);
    // Error status code? Log error status code
    else SocketLogSafe(LH_WARNING, "status error $!", ssvStatus);
    // If connection upgrade required? Handle websocket if requested else
    // throw an error because the socket might be left in a waiting state
    // which our client expects the server to close the connection.
    if(stStatus == 101)
      return eMode == WEBSOCKET ? WebSocketMain(mDest):
        SetErrorStaticSafe("Not upgrading");
    // Add protocol and status code to registry so guest can read them
    // without having to perform any special string operations
    psvRegistry.ParserPushOrUpdatePair(ctParent.strRegVarPROTO,
      StdString{ ssvProtoRecv });
    psvRegistry.ParserPushOrUpdatePair(ctParent.strRegVarCODE,
      StdString{ ssvStatus });
    // We have to lock the TX list since a LUA function can read this
    MutexCall([this](){
      // Enumerate registry entries
      for(const StrNCStrMapPair &sncsmpPair : psvRegistry)
      { // Get items and push into TX since we're not using it anymore. Make
        // sure to include the null-terminator so we can use StdStringView
        // for when LUA grabs the list.
        PushData(plTX, stTX,
          StrToLowCaseRef(UtilToNonConst(sncsmpPair.first)).data(),
          sncsmpPair.first.size() + 1);
        PushData(plTX, stTX, sncsmpPair.second.data(),
          sncsmpPair.second.size() + 1);
      }
    });
    // If we got a content type?
    const StrNCStrMapConstIt sncsmciType{
      GetRegistryIterator("content-type") };
    if(sncsmciType != psvRegistry.cend())
      SocketLogSafe(LH_DEBUG, "content type is '$'.", sncsmciType->second);
    // Should get content length
    const StrNCStrMapConstIt sncsmciLen{
      GetRegistryIterator("content-length") };
    if(sncsmciLen != psvRegistry.cend())
    { // Get reference to string and if it's not valid?
      const StdString &strVal = sncsmciLen->second;
      if(!StrIsInt(strVal))
      { // Assume zero, safe to continue and log the warning
        stContentLength = 0;
        SocketLogSafe(LH_WARNING, "invalid content length!");
      } // Valid content-length
      else
      { // Convert length string to integer and log the length
        stContentLength = StrToNum<size_t>(strVal);
        SocketLogSafe(LH_DEBUG, "content length is $ bytes.",
          stContentLength);
      } // Set downloading status
      AddStatus(SS_DOWNLOADING);
      // If we already got enough bytes from the header packet?
      if(stInitial == stContentLength)
        SocketLogSafe(LH_DEBUG, "downloaded resource in one packet.");
      // If we got too many bytes? treat it as completed anyway
      else if(stInitial > stContentLength)
        SocketLogSafe(LH_WARNING, "downloaded resource ($ bytes excess)!",
          stInitial - stContentLength);
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
/* -- Socket http connection ----------------------------------------------- */
ThreadFlagsType SocketHTTPThreadMain(Thread &)
{ // Return code
  ThreadFlagsType tftReturn;
  // Capture exceptions and switch to thiscall
  try { tftReturn = HTTPMain(); }
  // Exception occurred?
  catch(const StdException &eReason)
  { // Report error
    cLog->LogErrorExSafe("(SOCKET HTTP THREAD EXCEPTION) $", eReason);
    // Set error message
    tftReturn = SetErrorStaticSafe(eReason.what());
  } // If we're a websocket then stop the writer thread
  SocketTerminateWriteThread();
  // Clear connection and clean-up
  SendDisconnect();
  FinishDisconnect();
  // Required to stop memory leak
  OPENSSL_thread_stop();
  // Return status
  return tftReturn;
}
/* -- Init --------------------------------------------------------- */ public:
void HTTPRequest(lua_State*const lS, const StdStringView &ssvName,
  const StdStringView &ssvCipher,  const StdStringView &ssvAddress,
  const unsigned uNPort, const StdStringView &ssvRequest,
  const StdStringView &ssvMethod, const StdStringView &ssvHeaders,
  const StdStringView &ssvBody)
{ // Initialise name
  NameSet(ssvName);
  // Request must begin with a forward slash
  if(ssvRequest.front() != '/')
    XC("Resource is invalid!", "Resource", ssvRequest);
  // Set address and TLS cipher
  SetAddressAndCipher(ssvAddress, uNPort, ssvCipher);
  // Initialise registry with headers
  psvRegistry.ParserReInit(StdString{ ssvHeaders },
    cCommon->CommonLf(), ':');
  // Push default user agent if not specified already
  psvRegistry.ParserPushIfNotExist("User-Agent",
    cCVars->CVarsGetStrInternal(NET_USERAGENT));
  // Chosen method
  StdStringView ssvChosenMethod;
  // Check for websocket method
  if(ssvMethod == "WS")
  { // Set GET mathod back
    ssvChosenMethod = "GET";
    // Set required websocket headers
    psvRegistry.ParserPushOrUpdatePairs({
      // Websocket required headers
      { "Connection", "Upgrade, close" },
      { "Sec-WebSocket-Key", "" },
      { "Sec-WebSocket-Version", "13" },
      { "Upgrade", "websocket" }
    });
  } // Not a wegbsocket
  else
  { // Set chosen method
    ssvChosenMethod = ssvMethod;
    // Disable keep-alive, we don't support it (yet?).
    psvRegistry.ParserPushPair("Connection", "close");
  } // Find if the request contains a bookmark fragment
  const size_t stFrag = ssvRequest.find('#');
  // Format the request
  StdResized<StdString> strRequest
    { ssvChosenMethod.size() + ssvRequest.size() + 12 };
  if(stFrag == StdNPos) strRequest.assign(ssvRequest);
  else strRequest.assign(ssvRequest.substr(0, stFrag));
  // Start building registry for connector thread
  psvRegistry.ParserPushOrUpdatePairs({
    // Push the source address
    { "Host", strAddrPort },
    // Push the formulated request line. Remove the right hand fragment from
    // the URL if neccesary.
    { ctParent.strRegVarREQ, StrFormat("$ $ HTTP/1.0\r\n",
        ssvChosenMethod, StrUrlEncodeSpaces(strRequest())) },
    // Push method because we need to check if this is a HEAD request and
    // thus to know when to expect no output.
    { ctParent.strRegVarMETHOD, StdString{ ssvChosenMethod } },
  });
  // Body specified?
  if(!ssvBody.empty()) psvRegistry.ParserPushOrUpdatePairs({
    // Add length of body text
    { "Content-Length", StrFromNum(ssvBody.size()) },
    // Add body text
    { ctParent.strRegVarBODY, StdString{ ssvBody } }
  });
  // Init LUA references
  LuaEvtInitEx(lS);
  // Initialse thread and start the connection process
  tReader.ThreadInit(StrAppend("sockethttp:", Serial()),
    bind(&Socket::SocketHTTPThreadMain, this, _1), this);
}
/* == EoF =========================================================== EoF == */
