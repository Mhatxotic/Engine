/* == LLSAMPLE.HPP ========================================================= **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## Defines the 'Sample' namespace and methods for the guest to use in  ## **
** ## Lua. This file is invoked by 'lualib.hpp'.                          ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
#pragma once                           // Only one incursion allowed
/* ========================================================================= **
** ######################################################################### **
** ========================================================================= */
// % Sample
/* ------------------------------------------------------------------------- */
// ! The sample class allows loading of audio files into a single buffer.
// ! Samples can also be played with 3D positional audio.
/* ========================================================================= */
namespace LLSample {                   // Sample namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IPcm::P;               using namespace ISample::P;
using namespace Common;
/* ========================================================================= **
** ######################################################################### **
** ## Sample common helper classes                                        ## **
** ######################################################################### **
** -- Read Sample class argument ------------------------------------------- */
struct AgSample : public ArClass<Sample> {
  explicit AgSample(lua_State*const lS, const int iArg) :
    ArClass{*LuaUtilGetPtr<Sample>(lS, iArg, *cSamples)}{} };
/* -- Create Sample class argument ----------------------------------------- */
struct AcSample : public ArClass<Sample> {
  explicit AcSample(lua_State*const lS) :
    ArClass{*LuaUtilClassCreate<Sample>(lS, *cSamples)}{} };
/* ========================================================================= **
** ######################################################################### **
** ## Sample:* member functions                                           ## **
** ######################################################################### **
** ========================================================================= */
// $ Sample:Destroy
// ? Destroys the sample object and frees all the memory associated with it.
// ? The object will no longer be useable after this call and an error will be
// ? generated if accessed.
/* ------------------------------------------------------------------------- */
LLFUNC(Destroy, 0, LuaUtilClassDestroy<Sample>(lS, 1, *cSamples))
/* ========================================================================= */
// $ Sample:Duration
// < Duration:number=Duration of buffer in seconds
// ? Returns the duration of the buffer in seconds
/* ------------------------------------------------------------------------- */
LLFUNC(Duration, 1, LuaUtilPushVar(lS, AgSample{lS, 1}().GetDuration()))
/* ========================================================================= */
// $ Sample:Id
// < Id:integer=The id number of the Sample object.
// ? Returns the unique id of the Sample object.
/* ------------------------------------------------------------------------- */
LLFUNC(Id, 1, LuaUtilPushVar(lS, AgSample{lS, 1}().CtrGet()))
/* ========================================================================= */
// $ Sample:Name
// < Name:string=Name of the sample.
// ? If this sample was loaded by a filename or it was set with a custom id.
// ? This function returns that name which was assigned to it.
/* ------------------------------------------------------------------------- */
LLFUNC(Name, 1, LuaUtilPushVar(lS, AgSample{lS, 1}().IdentGet()))
/* ========================================================================= */
// $ Sample:Play
// > Gain:number=Volume of sample to play at.
// > Pan:integer=Current 2D position.
// > Pitch:integer=Pitch of played sample.
// > Loop:boolean=Loop the sample?
// ? Plays the specified sample at the specified gain and pitch with the
// ? specified 2D panning position. The source that this sample spawns is
// ? managed internally. See PlayEx() if you need to manage the source.
/* ------------------------------------------------------------------------- */
LLFUNC(Play, 0,
  const AgSample aSample{lS, 1};
  const AgALfloatLG aGain{lS, 2, 0.0f, 1.0f},
                    aPan{lS, 3, -1.0f, 1.0f};
  const AgALfloat aPitch{lS, 4};
  const AgBoolean aLooping{lS, 5};
  aSample().Play(aGain, aPan, aPitch, aLooping))
/* ========================================================================= */
// $ Sample:PlayEx
// > Gain:number=Volume of sample to play at.
// > Pan:integer=Current 2D position.
// > Pitch:integer=Pitch of played sample.
// > Loop:boolean=Loop the sample?
// < Class:Source=The source created from the sample or left channel source.
// < Class:Source=(Optional) If in stereo, the right channel source.
// ? Plays the specified sample at the specified gain and pitch with the
// ? specified 2D panning position. The source that this sample spawns is
// ? managed internally. See PlayEx() if you need to manage the source.
/* ------------------------------------------------------------------------- */
LLFUNC(PlayEx, 2,
  const AgSample aSample{lS, 1};
  const AgALfloatLG aGain{lS, 2, 0.0f, 1.0f},
                    aPan{lS, 3, -1.0f, 1.0f};
  const AgALfloat aPitch{lS, 4};
  const AgBoolean aLooping{lS, 5};
  aSample().Play(lS, aGain, aPan, aPitch, aLooping))
/* ========================================================================= */
// $ Sample:Spawn
// < Class:Source=The source created from the sample or left channel source.
// < Class:Source=(Optional) If in stereo, the right channel source.
// ? Spawns a new source from the sample. It is initially stopped and
// ? uninitialised in a 3-D state.
/* ------------------------------------------------------------------------- */
LLFUNC(Spawn, 2, AgSample{lS, 1}().Spawn(lS))
/* ========================================================================= */
// $ Sample:Stop
// < Count:integer=Number of source buffers stopped;
// ? Stops playing the specified sample.
/* ------------------------------------------------------------------------- */
LLFUNC(Stop, 1, LuaUtilPushVar(lS, AgSample{lS, 1}().Stop()))
/* ========================================================================= **
** ######################################################################### **
** ## Sample:* member functions structure                                 ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
LLRSMFBEGIN                            // Sample:* member functions begin
  LLRSFUNC(Destroy), LLRSFUNC(Duration), LLRSFUNC(Id),    LLRSFUNC(Name),
  LLRSFUNC(Play),    LLRSFUNC(PlayEx),   LLRSFUNC(Spawn), LLRSFUNC(Stop),
LLRSEND                                // Sample:* member functions end
/* ========================================================================= */
// $ Sample.Create
// > Handle:Pcm=The pcm sample class to load
// < Handle:Sample=The handle to the specified sample
// ? Loads the specified number of samples and returns a handle.
/* ------------------------------------------------------------------------- */
LLFUNC(Create, 1,
  const AgPcm aPcm{lS,1};
  AcSample{lS}().InitSample(aPcm))
/* ========================================================================= **
** ######################################################################### **
** ## Sample.* namespace functions structure                              ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
LLRSBEGIN                              // Sample.* namespace functions begin
  LLRSFUNC(Create),
LLRSEND                                // Sample.* namespace functions end
/* ========================================================================= */
}                                      // End of Sample namespace
/* == EoF =========================================================== EoF == */
