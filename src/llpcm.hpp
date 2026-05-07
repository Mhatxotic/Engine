/* == LLPCM.HPP ============================================================ **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## Defines the 'Pcm' namespace and methods for the guest to use in     ## **
** ## Lua. This file is invoked by 'lualib.hpp'.                          ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ========================================================================= **
** ######################################################################### **
** ========================================================================= */
// % Pcm
/* ------------------------------------------------------------------------- */
// ! This allows the programmer to load encoded waveforms which can be sent
// ! to OpenAL as a 'Sample' object if required. You can use the console
// ! command 'pcmfmts' to see what formats are usable.
/* ========================================================================= */
namespace LLPcm {                      // Pcm namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IPcmDef::P;            using namespace Common;
/* ========================================================================= **
** ######################################################################### **
** ## Pcm common helper classes                                           ## **
** ######################################################################### **
** -- Create Pcm class argument -------------------------------------------- */
struct AcPcm : public ArClass<Pcm> {
  explicit AcPcm(lua_State*const lS) :
    ArClass{LuaUtilClassCreateRef<Pcm>(lS, cPcms)}{} };
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
LLFUNC(Destroy, 0, LuaUtilClassDestroyChecked<Pcm>(lS, cPcms))
/* ========================================================================= */
// $ Pcm:Destroyed
// < Destroyed:boolean=If the Pcm class is destroyed
// ? Returns if the Pcm class is destroyed.
/* ------------------------------------------------------------------------- */
LLFUNC(Destroyed, 1, LuaUtilPushVar(lS, LuaUtilIsClassDestroyed(lS, cPcms)))
/* ========================================================================= */
// $ Pcm:Flags
// < Flags:integer=Pcm flags value
// ? Returns the current pcm flags.
/* ------------------------------------------------------------------------- */
LLFUNC(Flags, 1, LuaUtilPushVar(lS, AgPcm{lS, 1}().FlagGet()))
/* ========================================================================= */
// $ Pcm:Id
// < Id:integer=The id number of the PCM object.
// ? Returns the unique id of the PCM object.
/* ------------------------------------------------------------------------- */
LLFUNC(Id, 1, LuaUtilPushVar(lS, AgPcm{lS, 1}().Serial()))
/* ========================================================================= */
// $ Pcm:Name
// < Name:string=The name of the object
// ? Returns the name of the specified object when it was created, or if used
// ? by another function, a small trace of who took ownership of it prefixed
// ? with an exclamation mark (!).
/* ------------------------------------------------------------------------- */
LLFUNC(Name, 1, LuaUtilPushVar(lS, AgPcm{lS, 1}().NameGet()))
/* ========================================================================= **
** ######################################################################### **
** ## Pcm:* member functions structure                                    ## **
** ######################################################################### **
** ========================================================================= */
LLRSMFBEGIN                            // Pcm:* member functions begin
  LLRSFUNC(Destroy), LLRSFUNC(Destroyed), LLRSFUNC(Id), LLRSFUNC(Name),
LLRSEND                                // Pcm:* member functions end
/* ========================================================================= **
** ######################################################################### **
** ## Pcm.* namespace functions                                           ## **
** ######################################################################### **
** ========================================================================= */
// $ Pcm.Asset
// > Id:String=The identifier of the string
// > Data:Asset=The file data of the audio file to load
// < Handle:Pcm=The pcm object
// ? Loads an audio file on the main thread from the specified array object.
/* ------------------------------------------------------------------------- */
LLFUNC(Asset, 1,
  const AgNeString aName{lS, 1};
  const AgAsset aAsset{lS, 2};
  const AgPcmFlags aFlags{lS, 3};
  AcPcm{lS}().InitArray(aName, aAsset, aFlags))
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
  const AgNeString aName{lS, 1};
  const AgAsset aAsset{lS, 2};
  const AgPcmFlags aFlags{lS, 3};
  LuaUtilCheckFunc(lS, 4, 5, 6);
  AcPcm{lS}().InitAsyncArray(lS, aName, aAsset, aFlags))
/* ========================================================================= */
// $ Pcm.Count
// < Count:integer=Total number of pcms created.
// ? Returns the total number of pcm classes currently active.
/* ------------------------------------------------------------------------- */
LLFUNC(Count, 1, LuaUtilPushVar(lS, cPcms->CollectorCount()))
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
// > Name:string=Identifier of the sample.
// > Data:Asset=Sample PCM data.
// > Rate:integer=Sample rate.
// > Channels:integer=Sample channels.
// > Bits:integer=Sample ibts per channel.
// < Handle:Pcm=The pcm object
// ? Loads an audio file on the main thread from the specified array object.
/* ------------------------------------------------------------------------- */
LLFUNC(Raw, 1,
  const AgNeString aName{lS, 1};
  const AgAsset aAsset{lS, 2};
  const AgUIntLG aRate{lS, 3, 1, 5644800};
  const AgIntegerLG<PcmChannelType> aChannels{lS, 4, PCT_MONO, PCT_STEREO};
  const AgIntegerLGP2<PcmBitType> aBitsPerChannel{lS, 5, PBI_BYTE, PBI_LONG};
  AcPcm{lS}().InitRaw(aName, aAsset, aRate, aChannels, aBitsPerChannel))
/* ========================================================================= */
// $ Pcm.WaitAsync
// ? Halts main-thread execution until all async pcm events have completed
/* ------------------------------------------------------------------------- */
LLFUNC(WaitAsync, 0, cPcms->WaitAsync())
/* ========================================================================= **
** ######################################################################### **
** ## Pcm.* namespace functions structure                                 ## **
** ######################################################################### **
** ========================================================================= */
LLRSBEGIN                              // Pcm.* namespace functions begin
  LLRSFUNC(AssetAsync), LLRSFUNC(Asset),     LLRSFUNC(Count),
  LLRSFUNC(File),       LLRSFUNC(FileAsync), LLRSFUNC(Flags),
  LLRSFUNC(Raw),        LLRSFUNC(WaitAsync),
LLRSEND                                // Pcm.* namespace functions end
/* ========================================================================= **
** ######################################################################### **
** ## Pcm.* namespace constants                                           ## **
** ######################################################################### **
** ========================================================================= */
// @ Pcm.FlagsPcm
// < Codes:table=The table of key/value pairs of available flags.
// ? Returns the flags that define the original pcm before
// ? manipulation. These values are used with the 'Flags()' function.
/* ------------------------------------------------------------------------- */
LLRSKTBEGIN(FlagsPcm)                  // Beginning of pcm orig flags codes
  LLRSKTITEM(PL_,NONE),                LLRSKTITEM(PL_,BE),
  LLRSKTITEM(PL_,DYNAMIC),             LLRSKTITEM(PL_,SIGNED),
LLRSKTEND                              // End of pcm flags orig codes
/* ========================================================================= */
// @ Pcm.FlagsPost
// < Codes:table=The table of key/value pairs of available flags.
// ? Returns the flags that were activated after manipulation functions
// ? were completed. These values are used with the 'GetFlags()' function.
/* ------------------------------------------------------------------------- */
LLRSKTBEGIN(FlagsPost)                 // Beginning of pcm active flags codes
  LLRSKTITEM(PL_,NONE), LLRSKTITEM(PA_,TOSPU),    LLRSKTITEM(PA_,TOBE),
  LLRSKTITEM(PA_,TOLE), LLRSKTITEM(PA_,TOSIGNED), LLRSKTITEM(PA_,TOUNSIGNED),
LLRSKTEND                              // End of pcm flags active codes
/* ========================================================================= */
// @ Pcm.FlagsPre
// < Codes:table=The table of key/value pairs of available flags.
// ? Returns the flags that are sent to the loader functions to manipulate the
// ? the waveform. The flags that get activated are in the 'FlagsPost'
// ? array. These values are used with the 'GetFlags()' function.
/* ------------------------------------------------------------------------- */
LLRSKTBEGIN(FlagsPre)                  // Beginning of pcm loader flags codes
  LLRSKTITEM(PL_,NONE),                LLRSKTITEM(PL_,TOSPU),
  LLRSKTITEM(PL_,TOBE),                LLRSKTITEM(PL_,TOLE),
  LLRSKTITEM(PL_,TOSIGNED),            LLRSKTITEM(PL_,TOUNSIGNED),
  LLRSKTITEM(PL_,FCE_CAF),             LLRSKTITEM(PL_,FCE_OGG),
  LLRSKTITEM(PL_,FCE_WAV),
LLRSKTEND                              // End of pcm loader flags codes
/* ========================================================================= **
** ######################################################################### **
** ## Pcm.* namespace constants structure                                 ## **
** ######################################################################### **
** ========================================================================= */
LLRSCONSTBEGIN                         // Pcm.* namespace consts begin
  LLRSCONST(FlagsPcm), LLRSCONST(FlagsPost), LLRSCONST(FlagsPre),
LLRSCONSTEND                           // Pcm.* namespace consts end
/* ========================================================================= */
}                                      // End of Pcm namespace
/* == EoF =========================================================== EoF == */
