/* == LLSOCKET.HPP ========================================================= **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## Defines the 'Socket' namespace and methods for the guest to use in  ## **
** ## Lua. This file is invoked by 'lualib.hpp'.                          ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
#pragma once                           // Only one incursion allowed
/* ========================================================================= **
** ######################################################################### **
** ========================================================================= */
// % Socket
/* ------------------------------------------------------------------------- */
// ! The socket class allows the programmer to connect to a remote network
// ! using OpenSSL encryption or unencrypted connection. There is also basic
// ! support for HTTP connections, or you can write your own routines for that
// ! or any other protocol in Lua!
/* ========================================================================= */
namespace LLSocket {                   // Socket namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IAsset::P;             using namespace ILog::P;
using namespace ISocket::P;            using namespace Common;
/* ========================================================================= **
** ######################################################################### **
** ## Socket common helper classes                                        ## **
** ######################################################################### **
** -- Read Socket class argument ------------------------------------------- */
struct AgSocket : public ArClass<Socket> {
  explicit AgSocket(lua_State*const lS, const int iArg) :
    ArClass{*LuaUtilGetPtr<Socket>(lS, iArg, *cSockets)}{} };
/* -- Create Socket class argument ----------------------------------------- */
struct AcSocket : public ArClass<Socket> {
  explicit AcSocket(lua_State*const lS) :
    ArClass{*LuaUtilClassCreate<Socket>(lS, *cSockets)}{} };
/* ========================================================================= **
** ######################################################################### **
** ## Socket:* member functions                                           ## **
** ######################################################################### **
** ========================================================================= */
// $ Socket:Callback
// > Callback:function=The new callback function
// ? Sets a new callback function for the socket. This is useful - for example,
// ? if you have an addon system and you need to re-assign the newer callback
// ? so the older overwritten callback is no longer referenced.
/* ------------------------------------------------------------------------- */
LLFUNC(Callback, 0, AgSocket{lS, 1}().SetNewCallback(lS))
/* ========================================================================= */
// $ Socket:CompactRecvQ
// < Data:Asset=An array of data that was read from the queue.
// < Count:integer=The number of bytes removed from the read queue.
// ? This function collects all the data in the 'read' queue and moves it all
// ? into an array for you to manipulate as you desire.
/* ------------------------------------------------------------------------- */
LLFUNC(CompactRecvQ, 1, AgSocket{lS, 1}().CompactRXSafe(AcAsset{lS}()))
/* ========================================================================= */
// $ Socket:CompactSendQ
// < Data:Asset=An array of data that was waiting to be written to the queue.
// < Count:integer=The number of bytes removed from the queue.
// ? Removes all data from the pending write queue. If used as a HTTP socket
// ? then this may contain return header information (use PopSendQT() instead).
/* ------------------------------------------------------------------------- */
LLFUNC(CompactSendQ, 1, AgSocket{lS, 1}().CompactTXSafe(AcAsset{lS}()))
/* ========================================================================= */
// $ Socket:Destroy
// ? Aborts, disconnects and destroys the socket object and frees all the
// ? memory associated with it. The object will no longer be useable after
// ? this call and an error will be generated if accessed.
/* ------------------------------------------------------------------------- */
LLFUNC(Destroy, 0, LuaUtilClassDestroy<Socket>(lS, 1, *cSockets))
/* ========================================================================= */
// $ Socket:Disconnect
// > Socket:socket=The socket to disconnect.
// ? Disconnects the specified socket. All current asynchronous
// ? operations are cancelled so make sure they are finished before calling.
/* ------------------------------------------------------------------------- */
LLFUNC(Disconnect, 0, AgSocket{lS, 1}().SendDisconnect())
/* ========================================================================= */
// $ Socket:GetAddress
// < Address:string=The address of the socket.
// ? Returns the hostname of the socket.
/* ------------------------------------------------------------------------- */
LLFUNC(GetAddress, 1, LuaUtilPushVar(lS, AgSocket{lS, 1}().GetAddressSafe()))
/* ========================================================================= */
// $ Socket:GetAddressEx
// < Address:string=The address and port of the socket in string format.
// ? Returns the hostname and port of the socket as a string.
/* ------------------------------------------------------------------------- */
LLFUNC(GetAddressEx, 1,
  LuaUtilPushVar(lS, AgSocket{lS, 1}().GetAddressAndPortSafe()))
/* ========================================================================= */
// $ Socket:GetCipher
// < Cipher:string=Cipher tokens
// ? Returns the current cipher used for this connection
/* ------------------------------------------------------------------------- */
LLFUNC(GetCipher, 1, LuaUtilPushVar(lS, AgSocket{lS, 1}().GetCipherSafe()))
/* ========================================================================= */
// $ Socket:GetError
// < Code:integer=The OpenSSL error code.
// ? Returns the current OpenSSL error code associated with the socket.
/* ------------------------------------------------------------------------- */
LLFUNC(GetError, 1, LuaUtilPushVar(lS, AgSocket{lS, 1}().GetError()))
/* ========================================================================= */
// $ Socket:GetId
// < Id:integer=The id of the Socket object.
// ? Returns the unique id of the Socket object.
/* ------------------------------------------------------------------------- */
LLFUNC(GetId, 1, LuaUtilPushVar(lS, AgSocket{lS, 1}().CtrGet()))
/* ========================================================================= */
// $ Socket:GetIPAddress
// < Address:string=The IP address of the socket.
// ? Returns the IP address of the socket. Only valid when the hostname has
// ? been resolved.
/* ------------------------------------------------------------------------- */
LLFUNC(GetIPAddress, 1,
  LuaUtilPushVar(lS, AgSocket{lS, 1}().GetIPAddressSafe()))
/* ========================================================================= */
// $ Socket:GetIPAddressAndPortEx
// < Address:string=The IP address of the socket.
// ? Returns the IP address of the socket. Only valid when the hostname has
// ? been resolved.
/* ------------------------------------------------------------------------- */
LLFUNC(GetIPAddressEx, 1,
  LuaUtilPushVar(lS, AgSocket{lS, 1}().GetIPAddressAndPortSafe()))
/* ========================================================================= */
// $ Socket:GetReason
// < Reason:string=Reason for last error
// ? Returns the current OpenSSL error string associated with the socket.
/* ------------------------------------------------------------------------- */
LLFUNC(GetReason, 1, LuaUtilPushVar(lS, AgSocket{lS, 1}().GetErrorStrSafe()))
/* ========================================================================= */
// $ Socket:GetRXBytes
// < Total:integer=The number of bytes read from this socket.
// ? Returns the total number of bytes read from this socket.
/* ------------------------------------------------------------------------- */
LLFUNC(GetRXBytes, 1, LuaUtilPushVar(lS, AgSocket{lS, 1}().GetRX()))
/* ========================================================================= */
// $ Socket:GetRXPackets
// < Total:integer=The number of packets read from this socket.
// ? Returns the total number of packets read from this socket.
/* ------------------------------------------------------------------------- */
LLFUNC(GetRXPackets, 1, LuaUtilPushVar(lS, AgSocket{lS, 1}().GetRXpkt()))
/* ========================================================================= */
// $ Socket:GetSecure
// < State:boolean=Encryption state.
// ? Returns if this socket was initialised using SSL.
/* ------------------------------------------------------------------------- */
LLFUNC(GetSecure, 1, LuaUtilPushVar(lS, AgSocket{lS, 1}().IsSecure()))
/* ========================================================================= */
// $ Socket:GetStatus
// < Status:integer=The current socket status.
// ? Returns the current flags of the socket (See GetStatusCodes()).
/* ------------------------------------------------------------------------- */
LLFUNC(GetStatus, 1, LuaUtilPushVar(lS, AgSocket{lS, 1}().FlagGet()))
/* ========================================================================= */
// $ Socket:GetTXBytes
// < Total:integer=The number of bytes written to this socket.
// ? Returns the total number of bytes written to this socket.
/* ------------------------------------------------------------------------- */
LLFUNC(GetTXBytes, 1, LuaUtilPushVar(lS, AgSocket{lS, 1}().GetTX()))
/* ========================================================================= */
// $ Socket:GetTXPackets
// < Total:integer=The number of packets written to this socket.
// ? Returns the total number of packets written to this socket.
/* ------------------------------------------------------------------------- */
LLFUNC(GetTXPackets, 1, LuaUtilPushVar(lS, AgSocket{lS, 1}().GetTXpkt()))
/* ========================================================================= */
// $ Socket:PopRecvQ
// < Data:Asset=An array of data at the front of the read queue.
// < Count:integer=The number of bytes removed from the read queue.
// ? Removes the packet from the front of the read queue. If used as a HTTP
// ? socket then this may contain return header information (use PopSendQT()
// ? instead).
/* ------------------------------------------------------------------------- */
LLFUNC(PopRecvQ, 2,
  LuaUtilPushVar(lS, AgSocket{lS, 1}().GetPacketRXSafe(AcAsset{lS}())))
/* ========================================================================= */
// $ Socket:PopSendQ
// < Data:Asset=The data packet at the front of the send queue.
// < Count:integer=The number of bytes removed from the queue.
// ? Removes the packet from the front of the write queue. If used as a HTTP
// ? socket then this may contain return header information (use PopSendQT()
// ? instead).
/* ------------------------------------------------------------------------- */
LLFUNC(PopSendQ, 2,
  LuaUtilPushVar(lS, AgSocket{lS, 1}().GetPacketTXSafe(AcAsset{lS}())))
/* ========================================================================= */
// $ Socket:PopSendQT
// < Data:Asset=An array of data that was waiting to be written to the queue.
// < Count:integer=The number of bytes removed from the queue.
// ? Removes all data from the pending write queue. If used as a HTTP socket
// ? then this may contain return header information (use PopSendQT() instead).
/* ------------------------------------------------------------------------- */
LLFUNC(PopSendQT, 1, AgSocket{lS, 1}().ToLuaTable(lS))
/* ========================================================================= */
// $ Socket:RecvQCount
// < Count:integer=The current number of packets waiting to be processed.
// ? Because socket operations are asynchronous, the caller needs to process
// ? the packets manually. This function shows how many 'read' packets are
// ? waiting to be processed.
/* ------------------------------------------------------------------------- */
LLFUNC(RecvQCount, 1, LuaUtilPushVar(lS, AgSocket{lS, 1}().GetRXQCountSafe()))
/* ========================================================================= */
// $ Socket:SendQCount
// < Count:integer=The current number of packets waiting to be written.
// ? When a send operation is requested by the programmer, each packet is
// ? put in a queue and the worker thread dispatches these packets when it
// ? can. This functions shows how many packets are waiting to be 'written'.
/* ------------------------------------------------------------------------- */
LLFUNC(SendQCount, 1, LuaUtilPushVar(lS, AgSocket{lS, 1}().GetTXQCountSafe()))
/* ========================================================================= */
// $ Socket:TConnect
// < Time:number=The time the socket entered SS_CONNECTING state
// ? Returns the processor time that the socket entered the SS_CONNECTING
// ? state.
/* ------------------------------------------------------------------------- */
LLFUNC(TConnect, 1, LuaUtilPushVar(lS,
  cLog->CCDeltaToClampedDouble(AgSocket{lS, 1}().GetTConnect())))
/* ========================================================================= */
// $ Socket:TConnected
// < Time:number=The time the socket entered SS_CONNECTED state
// ? Returns the processor time that the socket entered the SS_CONNECTED
// ? state.
/* ------------------------------------------------------------------------- */
LLFUNC(TConnected, 1, LuaUtilPushVar(lS,
  cLog->CCDeltaToClampedDouble(AgSocket{lS, 1}().GetTConnected())))
/* ========================================================================= */
// $ Socket:TDisconnect
// < Time:number=The time the socket was closing.
// ? Returns the processor time that the socket initiated a close() operation.
/* ------------------------------------------------------------------------- */
LLFUNC(TDisconnect, 1, LuaUtilPushVar(lS,
  cLog->CCDeltaToClampedDouble(AgSocket{lS, 1}().GetTDisconnect())))
/* ========================================================================= */
// $ Socket:TDisconnected
// < Time:number=The time the socket was closed.
// ? Returns the processor time that the socket completed a close() operation.
/* ------------------------------------------------------------------------- */
LLFUNC(TDisconnected, 1, LuaUtilPushVar(lS, cLog->
  CCDeltaToClampedDouble(AgSocket{lS, 1}().GetTDisconnected())))
/* ========================================================================= */
// $ Socket:TRead
// < Time:number=The time the socket last completed a recv() operation.
// ? Returns the processor time that the socket last completed a recv()
// ? receive operation.
/* ------------------------------------------------------------------------- */
LLFUNC(TRead, 1, LuaUtilPushVar(lS,
  cLog->CCDeltaToClampedDouble(AgSocket{lS, 1}().GetTRead())))
/* ========================================================================= */
// $ Socket:TWrite
// < Time:number=The time the socket last completed a send() operation.
// ? Returns the processor time that the socket last completed a send()
// ? send operation.
/* ------------------------------------------------------------------------- */
LLFUNC(TWrite, 1, LuaUtilPushVar(lS,
  cLog->CCDeltaToClampedDouble(AgSocket{lS, 1}().GetTWrite())))
/* ========================================================================= */
// $ Socket:Write
// > Data:Asset=The data to write.
// ? Places the specified data packet into the sockets send queue. The worker
// ? thread will dispatch this data as soon as it can. The function returns
// ? immediately.
/* ------------------------------------------------------------------------- */
LLFUNC(Write, 0,
  const AgSocket aSocket{lS, 1};
  const AgAsset aAsset{lS, 2};
  aSocket().SendSafe(aAsset))
/* ========================================================================= */
// $ Socket:WriteString
// > Text:string=The data string to write.
// ? Places the specified string packet into the sockets send queue. The worker
// ? thread will dispatch this string as soon as it can. The function returns
// ? immediately.
/* ------------------------------------------------------------------------- */
LLFUNC(WriteString, 1,
  const AgSocket aSocket{lS, 1};
  const AgNeString aText{lS, 2};
  aSocket().SendStringSafe(aText))
/* ========================================================================= **
** ######################################################################### **
** ## Socket:* member functions structure                                 ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
LLRSMFBEGIN                            // Socket:* member functions begin
  LLRSFUNC(Callback),      LLRSFUNC(CompactRecvQ), LLRSFUNC(CompactSendQ),
  LLRSFUNC(Destroy),       LLRSFUNC(Disconnect),   LLRSFUNC(GetAddress),
  LLRSFUNC(GetAddressEx),  LLRSFUNC(GetCipher),    LLRSFUNC(GetError),
  LLRSFUNC(GetId),         LLRSFUNC(GetIPAddress), LLRSFUNC(GetIPAddressEx),
  LLRSFUNC(GetReason),     LLRSFUNC(GetRXBytes),   LLRSFUNC(GetRXPackets),
  LLRSFUNC(GetSecure),     LLRSFUNC(GetStatus),    LLRSFUNC(GetTXBytes),
  LLRSFUNC(GetTXPackets),  LLRSFUNC(PopRecvQ),     LLRSFUNC(PopSendQ),
  LLRSFUNC(PopSendQT),     LLRSFUNC(RecvQCount),   LLRSFUNC(SendQCount),
  LLRSFUNC(TConnect),      LLRSFUNC(TConnected),   LLRSFUNC(TDisconnect),
  LLRSFUNC(TDisconnected), LLRSFUNC(TRead),        LLRSFUNC(TWrite),
  LLRSFUNC(Write),         LLRSFUNC(WriteString),
LLRSEND                                // Socket:* member functions end
/* ========================================================================= */
// $ Socket.Connected
// < Count:integer=Total number of sockets connected
// ? Returns the total number of socket classes currently connected.
/* ------------------------------------------------------------------------- */
LLFUNC(Connected, 1, LuaUtilPushVar(lS, cSockets->stConnected.load()))
/* ========================================================================= */
// $ Socket.Count
// < Count:integer=Total number of sockets created.
// ? Returns the total number of socket classes currently active.
/* ------------------------------------------------------------------------- */
LLFUNC(Count, 1, LuaUtilPushVar(lS, cSockets->CollectorCount()))
/* ========================================================================= */
// $ Socket.Create
// > Address:string=The network destination address to connect to.
// > Port:integer=The port to connect to (1-65535).
// > Cipher:string=Make SSL connection and try this cipher.
// > Delay:string=The worker thread throttling rate.
// > Error:function=Function to call when a critical error occurs
// > Success:function=Function to call when any other event occurs
// ? Makes a standard point-to-point connection to the specified address.
// ? The function returns immediately as all socket operations are
// ? asynchronous. If you need SSL, make sure you specify the cipher suite
// ? you want to use or blank if you don't want to use SSL. Since a worker
// ? thread is used for each socket, you might want to specify a delay to
// ? throttle the loop as all operations are non-blocking. A value of 0 will
// ? make the thread consume 100% of a CPU core so be careful.
/* ------------------------------------------------------------------------- */
LLFUNC(Create, 0, AcSocket{lS}().Connect(lS))
/* ========================================================================= */
// $ Socket.CreateHTTP
// > Cipher:string=Make SSL connection and try this cipher.
// > Address:string=The network destination address to connect to.
// > Port:integer=The port to connect to (1-65535).
// > Request:string=The HTTP URI request resource.
// > Scheme:string=The HTTP scheme (GET, POST, PUT, DELETE, etc.).
// > Headers:string=A carriage return separated list of headers.
// > Body:string=A body of text to send with the request.
// > Error:function=Function to call when a critical error occurs
// > Success:function=Function to call when any other event occurs
// ? This is a minimalist implementation of a HTTP request and will support
// ? most (if not all) operations one would need. The function returns
// ? immediately. The worker thread uses blocking socket operations.
/* ------------------------------------------------------------------------- */
LLFUNC(CreateHTTP, 0, AcSocket{lS}().HTTPRequest(lS))
/* ========================================================================= */
// $ Socket.Flush
// < Count:integer=Total number of sockets disconnected.
// ? Disconnects all active Sockets. Returns immediately.
/* ------------------------------------------------------------------------- */
LLFUNC(Flush, 1, LuaUtilPushVar(lS, SocketReset()))
/* ========================================================================= */
// $ Socket.OAuth11
// > Method:string=The HTTP method (GET, PUT, etc.).
// > Scheme:string=The HTTP scheme (HTTP, HTTPS, etc.);
// > Port:integer=The HTTP port connected.
// > Resource:string=The HTTP resource requested.
// > Params:string=A HTTP URI list of variables that will be sent.
// > Body:string=Any HTTP body text sent.
// < Text:string=An OAuth11 compatible string that you can pass to web request.
// ? Calculates a OAuth v1.1 string to use with HTTP requests. Twitter is an
// ? example that uses this system.
/* ------------------------------------------------------------------------- */
LLFUNC(OAuth11, 1,
  const AgString aMethod{lS,1}, aScheme{lS, 2}, aPort{lS,3}, aResource{lS,4},
                 aParams{lS,5}, aBody{lS,6}, aText{lS,7};
   LuaUtilToTable(lS, SocketOAuth11(aMethod, aScheme, aPort, aResource,
     aParams, aBody, aText)))
/* ========================================================================= */
// $ Socket.TotalRXBytes
// < Total:integer=The number of bytes read from this socket.
// ? Returns the total number of bytes read from this socket.
/* ------------------------------------------------------------------------- */
LLFUNC(TotalRXBytes, 1, LuaUtilPushVar(lS, cSockets->qRX.load()))
/* ========================================================================= */
// $ Socket.TotalTXBytes
// < Total:integer=The number of bytes written to this socket.
// ? Returns the total number of bytes written to this socket.
/* ------------------------------------------------------------------------- */
LLFUNC(TotalTXBytes, 1, LuaUtilPushVar(lS, cSockets->qTX.load()))
/* ========================================================================= */
// $ Socket.TotalRXPackets
// < Total:integer=The total number of packets read from this socket.
// ? Returns the total number of packets read from this socket.
/* ------------------------------------------------------------------------- */
LLFUNC(TotalRXPackets, 1, LuaUtilPushVar(lS, cSockets->qRXp.load()))
/* ========================================================================= */
// $ Socket.TotalTXPackets
// < Total:integer=The total number of packets written to all sockets.
// ? Returns the total number of packets written to all sockets.
/* ------------------------------------------------------------------------- */
LLFUNC(TotalTXPackets, 1, LuaUtilPushVar(lS, cSockets->qTXp.load()))
/* ========================================================================= */
// $ Socket.ValidAddress
// > Address:string=The address to check.
// < State:boolean=The address is valid or not.
// ? Returns if the specified hostname or IP address is valid.
/* ------------------------------------------------------------------------- */
LLFUNC(ValidAddress, 1,
  LuaUtilPushVar(lS, Socket::ValidAddress(AgString{lS,1})))
/* ========================================================================= */
// $ Socket.SocketWaitAsync
// < Count:integer=Total number of sockets disconnected.
// ? Disconnects all active Sockets and waits until all sockets are closed.
/* ------------------------------------------------------------------------- */
LLFUNC(WaitAsync, 1, LuaUtilPushVar(lS, SocketWaitAsync()))
/* ========================================================================= **
** ######################################################################### **
** ## Socket.* namespace functions structure                              ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
LLRSBEGIN                              // Socket.* namespace functions begin
  LLRSFUNC(Create),         LLRSFUNC(CreateHTTP),   LLRSFUNC(Count),
  LLRSFUNC(Connected),      LLRSFUNC(Flush),        LLRSFUNC(OAuth11),
  LLRSFUNC(TotalRXBytes),   LLRSFUNC(TotalTXBytes), LLRSFUNC(TotalRXPackets),
  LLRSFUNC(TotalTXPackets), LLRSFUNC(WaitAsync),    LLRSFUNC(ValidAddress),
LLRSEND                                // Socket.* namespace functions end
/* ========================================================================= **
** ######################################################################### **
** ## Socket.* namespace constants                                        ## **
** ######################################################################### **
** ========================================================================= */
// @ Socket.Flags
// < Data:table=A list of socket status operations.
// ? Returns a key/value table of socket connction status codes supported.
/* ------------------------------------------------------------------------- */
LLRSKTBEGIN(Flags)                     // Beginning of socket status flags
  LLRSKTITEM(SS_,STANDBY),             LLRSKTITEM(SS_,INITIALISING),
  LLRSKTITEM(SS_,ENCRYPTION),          LLRSKTITEM(SS_,CONNECTING),
  LLRSKTITEM(SS_,CONNECTED),           LLRSKTITEM(SS_,DISCONNECTING),
  LLRSKTITEM(SS_,SENDREQUEST),         LLRSKTITEM(SS_,CLOSEDBYCLIENT),
  LLRSKTITEM(SS_,CLOSEDBYSERVER),      LLRSKTITEM(SS_,REPLYWAIT),
  LLRSKTITEM(SS_,DOWNLOADING),         LLRSKTITEM(SS_,EVENTERROR),
  LLRSKTITEM(SS_,READPACKET),
LLRSKTEND                              // End of socket status flags
/* ========================================================================= **
** ######################################################################### **
** ## Socket.* namespace constants structure                              ## **
** ######################################################################### **
** ========================================================================= */
LLRSCONSTBEGIN                         // Socket.* namespace consts begin
  LLRSCONST(Flags),                    // Socket status flags
LLRSCONSTEND                           // Socket.* namespace consts end
/* ========================================================================= */
}                                      // End of Socket namespace
/* == EoF =========================================================== EoF == */
