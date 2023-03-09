/* == LLPCM.HPP ============================================================ **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## Defines the 'Pcm' namespace and methods for the guest to use in     ## **
** ## Lua. This file is invoked by 'lualib.hpp'.                          ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
#pragma once                           // Only one incursion allowed
/* ========================================================================= **
** ######################################################################### **
** ========================================================================= */
// % Pcm
/* ------------------------------------------------------------------------- */
// ! This allows the programmer to load pcm wave forms or decompress encoded
// ! pcm waveforms and send the objects to the OpenAL if needed. You can use
// ! the command 'pcmfmts' to see what formats are usable which are WAV
// ! (WaveForm Audio Format) PCM/INT/FLOAT/LE, CAF (CoreAudio Format),
// ! PCM/INT/FLOAT/LE/BE, OGG (OGG Vorbis) and MP3 (MPEG-Layer III).
/* ========================================================================= */
namespace LLPcm {                      // Pcm namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IAsset::P;             using namespace IPcm::P;
using namespace IStd::P;               using namespace IPcmLib::P;
using namespace Common;
/* ========================================================================= **
** ######################################################################### **
** ## Pcm common helper classes                                           ## **
** ######################################################################### **
** -- Create Pcm class argument -------------------------------------------- */
struct AcPcm : public ArClass<Pcm> {
  explicit AcPcm(lua_State*const lS) :
    ArClass{*LuaUtilClassCreate<Pcm>(lS, *cPcms)}{} };
/* -- Read Pcm loading flags ----------------------------------------------- */
struct AgPcmFlags : public AgFlags<PcmFlagsConst> {
  explicit AgPcmFlags(lua_State*const lS, const int iArg) :
    AgFlags{ lS, iArg, PL_MASK }{} };
/* ========================================================================= **
** ######################################################################### **
** ## Pcm:* member functions                                              ## **
** ######################################################################### **
** ========================================================================= */
// $ Pcm:Destroy
// ? Destroys the pcm object and frees all the memory associated with it. The
// ? object will no longer be useable after this call and an error will be
// ? generated if accessed.
/* ------------------------------------------------------------------------- */
LLFUNC(Destroy, 0, LuaUtilClassDestroy<Pcm>(lS, 1, *cPcms))
/* ========================================================================= */
// $ Pcm:Id
// < Id:integer=The id number of the PCM object.
// ? Returns the unique id of the PCM object.
/* ------------------------------------------------------------------------- */
LLFUNC(Id, 1, LuaUtilPushVar(lS, AgPcm{lS, 1}().CtrGet()))
/* ========================================================================= */
// $ Pcm:Name
// < Name:string=The name of the object
// ? Returns the name of the specified object when it was created, or if used
// ? by another function, a small trace of who took ownership of it prefixed
// ? with an exclamation mark (!).
/* ------------------------------------------------------------------------- */
LLFUNC(Name, 1, LuaUtilPushVar(lS, AgPcm{lS, 1}().IdentGet()))
/* ========================================================================= **
** ######################################################################### **
** ## Pcm:* member functions structure                                    ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
LLRSMFBEGIN                            // Pcm:* member functions begin
  LLRSFUNC(Destroy), LLRSFUNC(Id), LLRSFUNC(Name),
LLRSEND                                // Pcm:* member functions end
/* ========================================================================= */
// $ Pcm.Asset
// > Id:String=The identifier of the string
// > Data:Asset=The file data of the audio file to load
// < Handle:Pcm=The pcm object
// ? Loads an audio file on the main thread from the specified array object.
/* ------------------------------------------------------------------------- */
LLFUNC(Asset, 1,
  const AgNeString aIdentifier{lS, 1};
  const AgAsset aAsset{lS, 2};
  const AgPcmFlags aFlags{lS, 3};
  AcPcm{lS}().InitArray(aIdentifier, aAsset, aFlags))
/* ========================================================================= */
// $ Pcm.AssetAsync
// > Id:String=The identifier of the string
// > Data:array=The data of the audio file to load
// > Flags:Integer=Load flags
// > ErrorFunc:function=The function to call when there is an error
// > ProgressFunc:function=The function to call when there is progress
// > SuccessFunc:function=The function to call when the audio file is laoded
// ? Loads an audio file off the main thread from the specified array object.
// ? The callback functions send an argument to the Pcm object that was
// ? created.
/* ------------------------------------------------------------------------- */
LLFUNC(AssetAsync, 0,
  LuaUtilCheckParams(lS, 6);
  const AgNeString aIdentifier{lS, 1};
  const AgAsset aAsset{lS, 2};
  const AgPcmFlags aFlags{lS, 3};
  LuaUtilCheckFunc(lS, 4, 5, 6);
  AcPcm{lS}().InitAsyncArray(lS, aIdentifier, aAsset, aFlags))
/* ========================================================================= */
// $ Pcm.File
// > Filename:string=The filename of the audio file to load
// > Flags:Integer=Load flags
// < Handle:Pcm=The pcm object
// ? Loads a audio sample on the main thread from the specified file on disk.
// ? Returns the pcm object.
/* ------------------------------------------------------------------------- */
LLFUNC(File, 1,
  const AgFilename aFilename{lS, 1};
  const AgPcmFlags aFlags{lS, 2};
  AcPcm{lS}().InitFile(aFilename, aFlags))
/* ========================================================================= */
// $ Pcm.FileAsync
// > Filename:string=The filename of the encoded waveform to load
// > Flags:Integer=Load flags
// > ErrorFunc:function=The function to call when there is an error
// > ProgressFunc:function=The function to call when there is progress
// > SuccessFunc:function=The function to call when the file is laoded
// ? Loads a audio file off the main thread. The callback functions send an
// ? argument to the pcm object that was created.
/* ------------------------------------------------------------------------- */
LLFUNC(FileAsync, 0,
  LuaUtilCheckParams(lS, 5);
  const AgFilename aFilename{lS, 1};
  const AgPcmFlags aFlags{lS, 2};
  LuaUtilCheckFunc(lS, 3, 4, 5);
  AcPcm{lS}().InitAsyncFile(lS, aFilename, aFlags))
/* ========================================================================= */
// $ Pcm.Raw
// > Identifier:string=Identifier of the sample.
// > Data:Asset=Sample PCM data.
// > Rate:integer=Sample rate.
// > Channels:integer=Sample channels.
// > Bits:integer=Sample ibts per channel.
// < Handle:Pcm=The pcm object
// ? Loads an audio file on the main thread from the specified array object.
/* ------------------------------------------------------------------------- */
LLFUNC(Raw, 1,
  const AgNeString aIdentifier{lS, 1};
  const AgAsset aAsset{lS, 2};
  const AgUIntLG aRate{lS, 3, 1, 5644800};
  const AgIntegerLG<PcmChannelType> aChannels{lS, 4, PCT_MONO, PCT_STEREO};
  const AgIntegerLGP2<PcmBitType> aBitsPerChannel{lS, 5, PBI_BYTE, PBI_LONG};
  AcPcm{lS}().InitRaw(aIdentifier, aAsset, aRate, aChannels, aBitsPerChannel))
/* ========================================================================= */
// $ Pcm.WaitAsync
// ? Halts main-thread execution until all async pcm events have completed
/* ------------------------------------------------------------------------- */
LLFUNC(WaitAsync, 0, cPcms->WaitAsync())
/* ========================================================================= **
** ######################################################################### **
** ## Pcm.* namespace functions structure                                 ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
LLRSBEGIN                              // Pcm.* namespace functions begin
  LLRSFUNC(AssetAsync), LLRSFUNC(Asset), LLRSFUNC(File),
  LLRSFUNC(FileAsync),  LLRSFUNC(Raw),   LLRSFUNC(WaitAsync),
LLRSEND                                // Pcm.* namespace functions end
/* ========================================================================= **
** ######################################################################### **
** ## Pcm.* namespace constants                                           ## **
** ######################################################################### **
** ========================================================================= */
// @ Pcm.Flags
// < Codes:table=The table of key/value pairs of available flags
// ? Returns the pcm flags available. Returned as key/value pairs. The
// ? value is a unique identifier to the flag.
/* ------------------------------------------------------------------------- */
LLRSKTBEGIN(Flags)                     // Beginning of pcm loading flags
  LLRSKTITEM(PL_,NONE),                LLRSKTITEM(PL_,FCE_CAF),
  LLRSKTITEM(PL_,FCE_MP3),             LLRSKTITEM(PL_,FCE_OGG),
  LLRSKTITEM(PL_,FCE_WAV),
LLRSKTEND                              // End of pcm loading flags
/* ========================================================================= **
** ######################################################################### **
** ## Pcm.* namespace constants structure                                 ## **
** ######################################################################### **
** ========================================================================= */
LLRSCONSTBEGIN                         // Pcm.* namespace consts begin
  LLRSCONST(Flags),
LLRSCONSTEND                           // Pcm.* namespace consts end
/* ========================================================================= */
}                                      // End of Pcm namespace
/* == EoF =========================================================== EoF == */
