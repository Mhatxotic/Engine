/* == SOCKETWEB.HPP ======================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## This file is included inline by 'socket.hpp' which defines the      ## **
** ## websocket routine.                                                  ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ---------------------------------------------------------------- */ private:
enum OpCode : unsigned                 // Websocket packet op codes
{ /* ----------------------------------------------------------------------- */
  OC_NONE        = StdUIntN1,          // Waiting for OpCode
  OC_FRAG        = 0x0,                // Fragmented packet
  OC_TEXT        = 0x1,                // UTF-8 Text packet
  OC_BINARY      = 0x2,                // Binary text packet
  OC_CLOSE       = 0x8,                // Close the connection request
  OC_PING        = 0x9,                // Ping the client test
  OC_PONG        = 0xA                 // Server ACK the ping client test.
}; /* -- Socket write manager ---------------------------------------------- */
ThreadFlagsType SockWebWriteManager()
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
          using MaskData = StdArray<uint8_t, 4>;
          // Small payload size?
          if(mcPacket.MemSize() < 125)
          { // Setup header
            mPayload.MemInitBlank(6 + mcPacket.MemSize());
            mPayload.MemWriteInt<uint8_t>(0, 0x80 | OC_TEXT);
            mPayload.MemWriteInt<uint8_t>(1, 0x80 |
              mcPacket.MemSize<uint8_t>());
            // Setup random masking key
            const MaskData mdMask{ CryptRandom<MaskData>() };
            mPayload.MemWrite(2, mdMask.data(), mdMask.size());
            // Mask original packet data into the payload
            for(size_t stIndex = 0; stIndex < mcPacket.MemSize(); ++stIndex)
              mPayload.MemWriteInt<uint8_t>(6 + stIndex,
                mcPacket.MemReadInt<uint8_t>(stIndex) ^
                  mdMask[stIndex % 4]);
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
            const MaskData mdMask{ CryptRandom<MaskData>() };
            mPayload.MemWrite(4, mdMask.data(), mdMask.size());
            // Mask original packet data into the payload
            for(size_t stIndex = 0; stIndex < mcPacket.MemSize(); ++stIndex)
              mPayload.MemWriteInt<uint8_t>(8 + stIndex,
                mcPacket.MemReadInt<uint8_t>(stIndex) ^
                  mdMask[stIndex % 4]);
          } // Large payload size?
          else
          { // Setup header
            mPayload.MemInitBlank(13 + mcPacket.MemSize());
            mPayload.MemWriteInt<uint8_t>(0, 0x80 | OC_TEXT);
            mPayload.MemWriteInt<uint8_t>(1, 0x80 | 127);
            for(size_t stIndex = 7; stIndex != StdSizeTN1; --stIndex)
              mPayload.MemWriteInt<uint8_t>
                ((mcPacket.MemSize() >> (9 * stIndex)) & 0xFF);
            // Setup random masking key
            const MaskData mdMask{ CryptRandom<MaskData>() };
            mPayload.MemWrite(9, mdMask.data(), mdMask.size());
            // Mask original packet data into the payload
            for(size_t stIndex = 0; stIndex < mcPacket.MemSize(); ++stIndex)
              mPayload.MemWriteInt<uint8_t>(13 + stIndex,
                mcPacket.MemReadInt<uint8_t>(stIndex) ^
                  mdMask[stIndex % 4]);
          } // Done
          break;
      } // Send it, and kill thread on error
      if(SockWrite(mPayload) == static_cast<size_t>(TS_ERROR))
        return TS_ERROR;
      // Subtract total bytes counter and pop the packet we just sent
      stTX -= mcPacket.MemSize();
      PopOldestTXPacketSafe();
    } // Wait for more data or exit request
    SockWriteWaitForMoreData();
  } // Sucesss
  return TS_OK;
}
/* -- Socket write manager ------------------------------------------------- */
ThreadFlagsType SockWriteManager()
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
    } // Wait for more data or exit request
    SockWriteWaitForMoreData();
  } // Sucesss
  return TS_OK;
}
/* -- WebSocket write thread ----------------------------------------------- */
ThreadFlagsType SockWebWriteThreadMain(Thread &)
{ // Return code
  ThreadFlagsType tftReturn;
  // Capture exceptions and execute the manager
  try { tftReturn = SockWebWriteManager(); }
  // exception occured?
  catch(const StdException &eReason)
  { // Report error
    cLog->LogErrorExSafe("(WEBSOCKET WRITE THREAD EXCEPTION) $", eReason);
    // Set error message
    tftReturn = SetErrorStaticSafe(eReason.what());
  } // Force close the socket if the reader thread isn't already exiting
  SendDisconnect();
  // Required to stop memory leak
  OPENSSL_thread_stop();
  // Return exit code
  return tftReturn;
}
/* -- Web socket main thread function reusing temporary buffer ------------- */
ThreadFlagsType WebSocketMain(Memory &mDest)
{ // Check for upgrade status and return error if not found
  const ParserStringVolConstIt psvciIt{ GetRegistryIterator("Upgrade") };
  if(psvciIt == psvRegistry.cend())
    return SetErrorStaticSafe("Missing upgrade header!");
  // Must be websocket
  if(psvciIt->second != "websocket")
    return SetErrorStaticSafe(
      StrAppend("Invalid upgrade protocol '$'!", psvciIt->second));
  // Create a thread to write data requests
  tWriter.ThreadInit(StrAppend("websocketwriter:", Serial()),
    bind(&Socket::SockWebWriteThreadMain, this, _1), this);
  // Set upgraded status and dispatch event
  AddStatus(SS_UPGRADED);
  // Websocket packet op codes
  enum OpCode uOpCode = OC_NONE;
  // Websocket packet read modes
  enum ReadMode : unsigned { RM_HEADER, RM_SMALL_PAYLOAD,
    RM_LARGE_PAYLOAD, RM_PAYLOAD } uMode = RM_HEADER;
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
    switch(uMode)
    { // Payload?
      case RM_PAYLOAD: Payload:
      { // Put payload straight into packets
        const size_t stPLPktSize = stTRead - stExtra;
        PushData(plTemp, stPLCurrent,
          mDest.MemRead(0, stPLPktSize), stPLPktSize);
        // Check opcode
        switch(uOpCode)
        { // Normal data?
          case OC_TEXT: case OC_BINARY: case OC_FRAG:
          { // Keep reading data if we're not done yet
            stRequired -= stPLPktSize;
            if(stRequired) continue;
            // Ignore if not final packet
            if(!bFinal) break;
            // Lock access to the packet list
            MutexCall([this, &plTemp, stPLTotal](){
              // Move packets to main packet list
              plRX.splice(plRX.end(), plTemp);
              // Set total size
              stRX += stPLTotal;
            });
            // Tell client that we have data ready
            DispatchEvent(SS_READPACKET);
            // Break to reset counters
            break;
          } // Ping? event?
          case OC_PING:
          { // Save ping time
            cdPing = cmHiRes.ClockGetEpochTime();
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
              StrAppend("Invalid opcode 0x", StdIOSHex, uOpCode));
        } // Reset to header mode
        uMode = RM_HEADER;
        // Reset op code function
        uOpCode = OC_NONE;
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
        const unsigned uFirst = mDest.MemReadInt<uint8_t>(),
                       uSecond = mDest.MemReadInt<uint8_t>(1);
        // Throw error if reserved bits are set
        if(uFirst & 0x40 || uFirst & 0x20 || uFirst & 0x10)
          return SetErrorStaticSafe("Reserved headers set!");
        // Server should not send a mask (bit 8)
        if(uSecond & 0x80) return SetErrorStaticSafe("Mask bit set!");
        // Set if a final packet (bit 0)
        bFinal = (uFirst & 0x80) != 0;
        // Set the opcode (bit 4-7)
        uOpCode = static_cast<OpCode>(uFirst & 0x0F);
        // Get initial payload value (bits 9 to 15)
        stPLTotal = uSecond & 0x7F;
        // Compare payload value
        switch(stPLTotal)
        { // Payload is small? (126 bytes)
          case 126:
          { // Set small payload expected next
            uMode = RM_SMALL_PAYLOAD;
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
            uMode = RM_LARGE_PAYLOAD;
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
        uint64_t ullPayload = 0;
        for(size_t stIndex = 0; stIndex < 8; ++stIndex)
          ullPayload = (ullPayload << 8) |
            mDest.MemReadInt<uint8_t>(stIndex);
        // Let's be sensible and deny above 4GB payloads for now
        if(ullPayload > 0xFFFFFFFF)
          return SetErrorStaticSafe(
            StrAppend("Payload $ too big", StdIOSHex, ullPayload));
        // Now process payload
        break;
      } // Should never get here
      default: return SetErrorStaticSafe("Invalid mode!");
    } // Set payload mode
    uMode = RM_PAYLOAD;
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
/* == EoF =========================================================== EoF == */
