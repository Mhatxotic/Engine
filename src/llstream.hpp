/* == LLSTREAM.HPP ========================================================= **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## Defines the 'Stream' namespace and methods for the guest to use in  ## **
** ## Lua. This file is invoked by 'lualib.hpp'.                          ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
#pragma once                           // Only one incursion allowed
/* ========================================================================= **
** ######################################################################### **
** ========================================================================= */
// % Stream
/* ------------------------------------------------------------------------- */
// ! The stream class allows loading of large audio files such as ambience
// ! or music fuiles which are streamed into and played back from multiple
// ! buffers.
// !
// ! Only .OGG file format is supported at the moment. The entire file
// ! will be loaded into memory in it's compressed form. It will be
// ! decompressed into multiple buffers during playback.
// !
// ! The 'aud_strbufcount' cvar controls how many buffers are used to stream.
// ! The 'aud_strbufsiz' cvar controls the size of each buffers.
// !
// ! Since playback is done in separate threads, there is not much need to
// ! modify any of the above settings.
/* ========================================================================= */
namespace LLStream {                   // Stream namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IAsset::P;             using namespace IStd::P;
using namespace IStream::P;            using namespace Common;
using namespace Lib::Ogg;
/* ========================================================================= **
** ######################################################################### **
** ## Stream common helper classes                                        ## **
** ######################################################################### **
** -- Read Stream class argument ------------------------------------------- */
struct AgStream : public ArClass<Stream> {
  explicit AgStream(lua_State*const lS, const int iArg) :
    ArClass{*LuaUtilGetPtr<Stream>(lS, iArg, *cStreams)}{} };
/* -- Create Stream class argument ----------------------------------------- */
struct AcStream : public ArClass<Stream> {
  explicit AcStream(lua_State*const lS) :
    ArClass{*LuaUtilClassCreate<Stream>(lS, *cStreams)}{} };
/* -- Read duration value -------------------------------------------------- */
using Lib::OpenAL::ALdouble;
struct AgDuration : public AgNumberLG<ALdouble> {
  explicit AgDuration(lua_State*const lS, const int iArg, Stream &sCref):
    AgNumberLG{lS, iArg, 0, sCref.GetDurationSafe()}{} };
/* -- Read sample value ---------------------------------------------------- */
struct AgSample : public AgIntegerLG<ogg_int64_t> {
  explicit AgSample(lua_State*const lS, const int iArg, Stream &sCref) :
    AgIntegerLG{lS, iArg, 0, sCref.GetSamplesSafe()}{}};
/* ========================================================================= **
** ######################################################################### **
** ## Stream:* member functions                                           ## **
** ######################################################################### **
** ========================================================================= */
// $ Stream:Destroy
// ? Stops and destroys the stream object and frees all the memory associated
// ? with it. The object will no longer be useable after this call and an
// ? error will be generated if accessed.
/* ------------------------------------------------------------------------- */
LLFUNC(Destroy, 0, LuaUtilClassDestroy<Stream>(lS, 1, *cStreams))
/* ========================================================================= */
// $ Stream:OnEvent
// > Func:function=The callback function to call when an event occurs.
// ? This function is called when the stream stops, starts or loops. The
// ? callback function syntax is 'function(EventType:Integer)'. Please be
// ? aware that that reference counter on the supplied function is increased
// ? therefore the reference will linger until a new function is set or the
// ? Stream class is destroyed. This also means references to variables such as
// ? this class inside the callback will make the variable invulnerable to the
// ? garbage collector if the reference on the function is not properly
// ? unreferenced so be careful how you use this!
/* ------------------------------------------------------------------------- */
LLFUNC(OnEvent, 0, AgStream{lS, 1}().LuaEvtInit(lS))
/* ========================================================================= */
// $ Stream:Play
// > Loops:integer=Number of loops.
// > Volume:number=Stream volume (0 to 1).
// > Position:integer=Sample position.
// ? Plays the specified stream at the specified sample position.
/* ------------------------------------------------------------------------- */
LLFUNC(Play, 0, AgStream{lS, 1}().PlaySafe())
/* ========================================================================= */
// $ Stream:GetDuration
// < Duration:number=Duration in seconds
// ? Returns the duration of the specified stream in seconds.
/* ------------------------------------------------------------------------- */
LLFUNC(GetDuration, 1,
  LuaUtilPushVar(lS, AgStream{lS, 1}().GetDurationSafe()))
/* ========================================================================= */
// $ Stream:GetElapsed
// < Duration:number=Elapsed time in seconds
// ? Returns the time elapsed of the stream. This is the position of the codec
// ? and not the actual playback position.
/* ------------------------------------------------------------------------- */
LLFUNC(GetElapsed, 1, LuaUtilPushVar(lS, AgStream{lS, 1}().GetElapsedSafe()))
/* ========================================================================= */
// $ Stream:SetElapsed
// > Duration:number=Duration in seconds
// ? Sets the specified time index of the stream. This is not a precise time
// ? index and is optimised for speed. Use Stream.SetElapsedP() for precision.
/* ------------------------------------------------------------------------- */
LLFUNC(SetElapsed, 0,
  const AgStream aStream{lS, 1};
  const AgDuration aDuration{lS, 2, aStream};
  aStream().SetElapsedSafe(aDuration))
/* ========================================================================= */
// $ Stream:SetElapsedPage
// > Duration:number=Duration in seconds
// ? Sets the precise specified time index of the stream.
/* ------------------------------------------------------------------------- */
LLFUNC(SetElapsedPage, 0,
  const AgStream aStream{lS, 1};
  const AgDuration aDuration{lS, 2, aStream};
  aStream().SetElapsedFastSafe(aDuration))
/* ========================================================================= */
// $ Stream:GetPosition
// < Position:integer=Sample position
// ? Returns the sample position elapsed of the stream.
/* ------------------------------------------------------------------------- */
LLFUNC(GetPosition, 1,
  LuaUtilPushVar(lS, AgStream{lS, 1}().GetPositionSafe()))
/* ========================================================================= */
// $ Stream:SetPosition
// > Position:integer=Sample position.
// ? Sets the precise specified sample position index of the stream.
/* ------------------------------------------------------------------------- */
LLFUNC(SetPosition, 0,
  const AgStream aStream{lS, 1};
  const AgSample aSample{lS, 2, aStream};
  aStream().SetPositionSafe(aSample))
/* ========================================================================= */
// $ Stream:SetPositionPage
// > Position:integer=Sample position
// ? Sets the current sample position of playback. This is not a precise
// ? position is optimised for speed. Use Stream.SetPosition() for precision.
/* ------------------------------------------------------------------------- */
LLFUNC(SetPositionPage, 0,
  const AgStream aStream{lS, 1};
  const AgSample aSample{lS, 2, aStream};
  aStream().SetPositionFastSafe(aSample))
/* ========================================================================= */
// $ Stream:Stop
// ? Stops playback of the specified stream. This will not modify the position
// ? or any other stream property.
/* ------------------------------------------------------------------------- */
LLFUNC(Stop, 0, AgStream{lS, 1}().StopSafe(SR_LUA))
/* ========================================================================= */
// $ Stream:IsPlaying
// < State:boolean=Stream is playing?
// ? Returns true if the stream is playing or false if the stream is stopped.
/* ------------------------------------------------------------------------- */
LLFUNC(IsPlaying, 1, LuaUtilPushVar(lS, AgStream{lS, 1}().IsPlaying()))
/* ========================================================================= */
// $ Stream:GetLoop
// < Count:integer=Number of loops left.
// ? Returns the number of loops remaining of playback.
/* ------------------------------------------------------------------------- */
LLFUNC(GetLoop, 1, LuaUtilPushVar(lS, AgStream{lS, 1}().GetLoopSafe()))
/* ========================================================================= */
// $ Stream:GetId
// < Id:integer=The id number of the Stream object.
// ? Returns the unique id of the Stream object.
/* ------------------------------------------------------------------------- */
LLFUNC(GetId, 1, LuaUtilPushVar(lS, AgStream{lS, 1}().CtrGet()))
/* ========================================================================= */
// $ Stream:GetName
// < Name:string=Filename of stream
// ? Returns the stream indentifier or filename
/* ------------------------------------------------------------------------- */
LLFUNC(GetName, 1, LuaUtilPushVar(lS, AgStream{lS, 1}().IdentGet()))
/* ========================================================================= */
// $ Stream:GetLoopBegin
// < Count:integer=Smaple position begin point
// ? Gets the current loop begin sample.
/* ------------------------------------------------------------------------- */
LLFUNC(GetLoopBegin, 1,
  LuaUtilPushVar(lS, AgStream{lS, 1}().GetLoopBeginSafe()))
/* ========================================================================= */
// $ Stream:GetLoopEnd
// < Count:integer=Sample position end point
// ? Gets the current loop end sample.
/* ------------------------------------------------------------------------- */
LLFUNC(GetLoopEnd, 1, LuaUtilPushVar(lS, AgStream{lS, 1}().GetLoopEndSafe()))
/* ========================================================================= */
// $ Stream:SetLoop
// > Count:integer=New playback loop count.
// ? Sets the new playback loop count.
/* ------------------------------------------------------------------------- */
LLFUNC(SetLoop, 0, AgStream{lS, 1}().SetLoopSafe(
  AgIntegerLGE<ogg_int64_t>{lS, 2, -1, numeric_limits<ogg_int64_t>::max() }))
/* ========================================================================= */
// $ Stream:SetLoopRange
// > Begin:integer=New beginning sample playback position of loop.
// > End:integer=New ending sample playback position of loop.
// ? Sets the new playback beginning sample position after stream loops and
// ? the ending sample position.
/* ------------------------------------------------------------------------- */
LLFUNC(SetLoopRange, 0,
  const AgStream aStream{lS, 1};
  const AgSample aSample{lS, 2, aStream};
  const AgIntegerLGE<ogg_int64_t>
    aEnd{ lS, 3, aSample, aStream().GetSamplesSafe() };
  aStream().SetLoopRangeSafe(aSample, aEnd))
/* ========================================================================= */
// $ Stream:SetLoopBegin
// > Begin:integer=New beginning sample playback position of loop.
// ? Sets the new playback beginning sample position after stream loops.
/* ------------------------------------------------------------------------- */
LLFUNC(SetLoopBegin, 0, const AgStream aStream{lS, 1};
  aStream().SetLoopBeginSafe(AgSample{lS, 2, aStream}))
/* ========================================================================= */
// $ Stream:SetLoopEnd
// > End:integer=New ending sample playback position of loop.
// ? Sets the new playback ending sample position before stream loops.
/* ------------------------------------------------------------------------- */
LLFUNC(SetLoopEnd, 0, const AgStream aStream{lS, 1};
  aStream().SetLoopEndSafe(AgIntegerLGE<ogg_int64_t>
    { lS, 2, aStream().GetLoopBeginSafe(), aStream().GetSamplesSafe() }))
/* ========================================================================= */
// $ Stream:SetVolume
// > Volume:number=New stream volume (0 to 1).
// ? Sets the volume of the stream class even if it is playing.
/* ------------------------------------------------------------------------- */
LLFUNC(SetVolume, 0, AgStream{lS, 1}().SetVolume(AgVolume{lS, 2}))
/* ========================================================================= */
// $ Stream:GetVolume
// < Volume:number=Current stream volume (0 to 1).
// ? Returns the volume of the stream class.
/* ------------------------------------------------------------------------- */
LLFUNC(GetVolume, 1, LuaUtilPushVar(lS, AgStream{lS, 1}().GetVolume()))
/* ========================================================================= */
// $ Stream:GetBytes
// < Total:integer=Current stream total bytes.
// ? Returns the size of the file that is streaming.
/* ------------------------------------------------------------------------- */
LLFUNC(GetBytes, 1, LuaUtilPushVar(lS, AgStream{lS, 1}().GetOggBytes()))
/* ========================================================================= */
// $ Stream:GetSamples
// < Total:integer=Current stream total samples.
// ? Returns the duration of the stream in samples.
/* ------------------------------------------------------------------------- */
LLFUNC(GetSamples, 1, LuaUtilPushVar(lS, AgStream{lS, 1}().GetSamplesSafe()))
/* ========================================================================= */
// $ Stream:GetMetaData
// < Data:table=Stream metadata key/value table.
// ? Returns all the metadata that was stored inside the audio file.
/* ------------------------------------------------------------------------- */
LLFUNC(GetMetaData, 1, LuaUtilToTable(lS, AgStream{lS, 1}().GetMetaData()))
/* ========================================================================= */
// $ Stream:GetRate
// < Rate:integer=The sample rate
// ? Returns the sample rate per second in samples.
/* ------------------------------------------------------------------------- */
LLFUNC(GetRate, 1, LuaUtilPushVar(lS, AgStream{lS, 1}().GetRate()))
/* ========================================================================= */
// $ Stream:GetBitRate
// < Upper:integer=The upper bitrate
// < Nominal:integer=The nominal bitrate
// < Lower:integer=The lower bitrate
// < Window:integer=The window bitrate
// ? Returns bitrate information for the audio file.
/* ------------------------------------------------------------------------- */
LLFUNC(GetBitRate, 0, const AgStream aStream{lS, 1};
  LuaUtilPushVar(lS, aStream().GetBitRateUpper(),
    aStream().GetBitRateNominal(), aStream().GetBitRateLower(),
    aStream().GetBitRateWindow()))
/* ========================================================================= */
// $ Stream:GetVersion
// < Version:integer=The ogg version
// ? Returns the version of the ogg file
/* ------------------------------------------------------------------------- */
LLFUNC(GetVersion, 1, LuaUtilPushVar(lS, AgStream{lS, 1}().GetVersion()))
/* ========================================================================= */
// $ Stream:GetChannels
// < Rate:integer=The number of channels
// ? Returns the number of channels in the audio file.
/* ------------------------------------------------------------------------- */
LLFUNC(GetChannels, 1, LuaUtilPushVar(lS, AgStream{lS, 1}().GetChannels()))
/* ========================================================================= **
** ######################################################################### **
** ## Stream:* member functions structure                                 ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
LLRSMFBEGIN                            // Stream:* member functions begin
  LLRSFUNC(OnEvent),        LLRSFUNC(GetBitRate),   LLRSFUNC(GetBytes),
  LLRSFUNC(GetChannels),    LLRSFUNC(GetDuration),  LLRSFUNC(GetElapsed),
  LLRSFUNC(GetId),          LLRSFUNC(GetName),      LLRSFUNC(GetPosition),
  LLRSFUNC(GetLoop),        LLRSFUNC(GetLoopBegin), LLRSFUNC(GetLoopEnd),
  LLRSFUNC(GetRate),        LLRSFUNC(GetSamples),   LLRSFUNC(GetVersion),
  LLRSFUNC(GetVolume),      LLRSFUNC(GetMetaData),  LLRSFUNC(IsPlaying),
  LLRSFUNC(OnEvent),        LLRSFUNC(Play),         LLRSFUNC(SetElapsed),
  LLRSFUNC(SetElapsedPage), LLRSFUNC(SetPosition),  LLRSFUNC(SetPositionPage),
  LLRSFUNC(SetLoop),        LLRSFUNC(SetLoopBegin), LLRSFUNC(SetLoopEnd),
  LLRSFUNC(SetLoopRange),   LLRSFUNC(SetVolume),    LLRSFUNC(Stop),
LLRSEND                                // Stream:* member functions end
/* ========================================================================= */
// $ Stream.Asset
// > Id:String=The identifier of the string
// > Data:Asset=The file data of the ogg file to load
// < Handle:Ogg=The ogg object
// ? Loads an ogg file to stream on the main thread from the specified array
// ? object.
/* ------------------------------------------------------------------------- */
LLFUNC(Asset, 1,
  const AgNeString aIdentifier{lS, 1};
  const AgAsset aAsset{lS, 2};
  AcStream{lS}().SyncInitArray(aIdentifier, aAsset))
/* ========================================================================= */
// $ Stream.AssetAsync
// > Id:String=The identifier of the string
// > Data:array=The data of the ogg file to load
// > ErrorFunc:function=The function to call when there is an error
// > ProgressFunc:function=The function to call during initial IO loading.
// > SuccessFunc:function=The function to call when the audio file is laoded
// ? Loads an ogg file off the main thread from the specified array object.
// ? The callback functions send an argument to the Stream object that was
// ? created.
/* ------------------------------------------------------------------------- */
LLFUNC(AssetAsync, 0,
  LuaUtilCheckParams(lS, 5);
  const AgNeString aIdentifier{lS, 1};
  const AgAsset aAsset{lS, 2};
  LuaUtilCheckFunc(lS, 3, 4, 5);
  AcStream{lS}().AsyncInitArray(lS, aIdentifier, "streamarray", aAsset))
/* ========================================================================= */
// $ Stream.ClearEvents
// ? Clear the OnEvent callback on all Stream objects. This might be useful for
// ? when you use the OnEvent() function and the Stream object is still
// ? referenced and no longer accessable by your code in which the result is
// ? that the Stream object does not get garbage collected.
/* ------------------------------------------------------------------------- */
LLFUNC(ClearEvents, 0, StreamClearEvents())
/* ========================================================================= */
// $ Stream.File
// > Filename:string=The filename of the ogg file to load
// < Handle:Stream=The stream object
// ? Loads a stream sample on the main thread from the specified file on disk.
// ? Returns the stream object.
/* ------------------------------------------------------------------------- */
LLFUNC(File, 1, const AgFilename aFilename{lS, 1};
  AcStream{lS}().SyncInitFileSafe(aFilename))
/* ========================================================================= */
// $ Stream.FileAsync
// > Filename:string=The filename of the ogg to load
// > ErrorFunc:function=The function to call when there is an error
// > ProgressFunc:function=The function to call during initial IO loading.
// > SuccessFunc:function=The function to call when the file is laoded
// ? Loads a streamable ogg file off the main thread. The callback functions
// ? send an argument to the stream object that was created.
/* ------------------------------------------------------------------------- */
LLFUNC(FileAsync, 0,
  LuaUtilCheckParams(lS, 4);
  const AgFilename aFilename{lS, 1};
  LuaUtilCheckFunc(lS, 2, 3, 4);
  AcStream{lS}().AsyncInitFile(lS, aFilename, "streamfile"))
/* ========================================================================= */
// $ Stream.WaitAsync
// ? Halts main-thread execution until all async stream mevents have completed
/* ------------------------------------------------------------------------- */
LLFUNC(WaitAsync, 0, cStreams->WaitAsync())
/* ========================================================================= **
** ######################################################################### **
** ## Stream.* namespace functions structure                              ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
LLRSBEGIN                              // Stream.* namespace functions begin
  LLRSFUNC(Asset), LLRSFUNC(AssetAsync), LLRSFUNC(ClearEvents),
  LLRSFUNC(File),  LLRSFUNC(FileAsync),  LLRSFUNC(WaitAsync),
LLRSEND                                // Stream.* namespace functions end
/* ========================================================================= **
** ######################################################################### **
** ## Stream.* namespace constants                                        ## **
** ######################################################################### **
** ========================================================================= */
// @ Stream.Flags
// < Codes:table=The table of key/value pairs of available flags
// ? Returns possible values for Stream:OnEvent() event command.
/* ------------------------------------------------------------------------- */
LLRSKTBEGIN(Events)                    // Beginning of Video event flags
LLRSKTITEM(SE_,PLAY),                  LLRSKTITEM(SE_,STOP),
LLRSKTEND                              // End of Video event flags
/* ========================================================================= */
// @ Stream.States
// < Codes:table=The table of key/value pairs of available flags
// ? Returns possible values for Stream:OnEvent() last state command.
/* ------------------------------------------------------------------------- */
LLRSKTBEGIN(States)                    // Beginning of Stream last state flags
LLRSKTITEM(PS_,STANDBY),               LLRSKTITEM(PS_,PLAYING),
LLRSKTITEM(PS_,PLAYING),               LLRSKTITEM(PS_,FINISHING),
LLRSKTITEM(PS_,WASPLAYING),            LLRSKTITEM(PS_,MAX),
LLRSKTEND                              // End of Stream last state flags
/* ========================================================================= */
// @ Stream.Reasons
// < Codes:table=The table of key/value pairs of available flags
// ? Returns possible values for Stream:OnEvent() stop reason command.
/* ------------------------------------------------------------------------- */
LLRSKTBEGIN(Reasons)                   // Beginning of Stream stop reasons
LLRSKTITEM(SR_,STOPNOUNQ),             LLRSKTITEM(SR_,STOPUNQ),
LLRSKTITEM(SR_,REBUFFAIL),             LLRSKTITEM(SR_,RWREBUFFAIL),
LLRSKTITEM(SR_,GENBUFFAIL),            LLRSKTITEM(SR_,STOPALL),
LLRSKTITEM(SR_,LUA),                   LLRSKTITEM(SR_,MAX),
LLRSKTEND                              // End of Stream stop reasons
/* ========================================================================= **
** ######################################################################### **
** ## Stream.* namespace constants structure                              ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
LLRSCONSTBEGIN                         // Stream.* namespace consts begin
LLRSCONST(Events), LLRSCONST(States), LLRSCONST(Reasons),
LLRSCONSTEND                           // Stream.* namespace consts end
/* ========================================================================= */
}                                      // End of Stream namespace
/* == EoF =========================================================== EoF == */
