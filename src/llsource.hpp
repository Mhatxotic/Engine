/* == LLSOURCE.HPP ========================================================= **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## Defines the 'Source' namespace and methods for the guest to use in  ## **
** ## Lua. This file is invoked by 'lualib.hpp'.                          ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ========================================================================= **
** ######################################################################### **
** ========================================================================= */
// % Source
/* ------------------------------------------------------------------------- */
// ! The source is a sound channel in OpenAL that can be manipulated such as */
// ! changing volume, pitch, position and other things. It can only be       */
// ! created by the Sample:Play() function.
/* ========================================================================= */
namespace LLSource {                   // Stream namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace ISource::P;            using namespace Common;
/* ========================================================================= **
** ######################################################################### **
** ## Source common helper classes                                        ## **
** ######################################################################### **
** -- Read Source class argument ------------------------------------------- */
struct AgSource : public ArClass<Source> {
  explicit AgSource(lua_State*const lS, const int iArg) :
    ArClass{LuaUtilGetClassRef<Source>(lS, iArg, cSources)}{} };
/* -- Read integer --------------------------------------------------------- */
using AgALint = AgInteger<ALint>;
/* ========================================================================= **
** ######################################################################### **
** ## Source:* methods                                                    ## **
** ######################################################################### **
** ========================================================================= */
// $ Source:Destroy
// ? Stops and destroys the source object and frees all the memory associated
// ? with it. The object will no longer be useable after this call and an
// ? error will be generated if accessed.
/* ------------------------------------------------------------------------- */
LLFUNC(Destroy, 0, LuaUtilClassDestroy<Source>(lS, cSources))
/* ========================================================================= */
// $ Source:GetByte
// < Byte:integer=The byte position of the current playback.
// ? Returns the current byte position of the sample being played.
/* ------------------------------------------------------------------------- */
LLFUNC(GetByte, 1, LuaUtilPushVar(lS, AgSource{lS, 1}().SourceGetByte()))
/* ========================================================================= */
// $ Source:GetDirection
// < X:number=Current X direction.
// < Y:number=Current Y direction.
// < Z:number=Current Z direction.
// ? Returns the current direction of the source.
/* ------------------------------------------------------------------------- */
LLFUNC(GetDirection, 3,
  ALfloat fX, fY, fZ;
  AgSource{lS, 1}().SourceGetDirection(fX, fY, fZ);
  LuaUtilPushVar(lS, fX, fY, fZ))
/* ========================================================================= */
// $ Source:GetElapsed
// < Seconds:number=Number of seconds elapsed.
// ? Returns the number of seconds elapsed in the source.
/* ------------------------------------------------------------------------- */
LLFUNC(GetElapsed, 1,
  LuaUtilPushVar(lS, AgSource{lS, 1}().SourceGetElapsed()))
/* ========================================================================= */
// $ Source:GetId
// < Id:integer=The id number of the Source object.
// ? Returns the unique id of the Source object.
/* ------------------------------------------------------------------------- */
LLFUNC(GetId, 1, LuaUtilPushVar(lS, AgSource{lS, 1}().Serial()))
/* ========================================================================= */
// $ Source:GetGain
// < Gain:number=The current gain value.
// ? Returns the current gain of the source.
/* ------------------------------------------------------------------------- */
LLFUNC(GetGain, 1, LuaUtilPushVar(lS, AgSource{lS, 1}().SourceGetGain()))
/* ========================================================================= */
// $ Source:GetLooping
// < State:boolean=Is the source looping?
// ? Returns if the source is looping.
/* ------------------------------------------------------------------------- */
LLFUNC(GetLooping, 1, LuaUtilPushVar(lS, AgSource{lS, 1}().SourceGetLooping()))
/* ========================================================================= */
// $ Source:GetMaxDist
// < Distance:number=The current maximum distance.
// ? Returns the current maximum distance of the source.
/* ------------------------------------------------------------------------- */
LLFUNC(GetMaxDist, 1, LuaUtilPushVar(lS, AgSource{lS, 1}().SourceGetMaxDist()))
/* ========================================================================= */
// $ Source:GetMaxGain
// < Gain:number=The current gain value.
// ? Returns the current maximum gain of the source.
/* ------------------------------------------------------------------------- */
LLFUNC(GetMaxGain, 1, LuaUtilPushVar(lS, AgSource{lS, 1}().SourceGetMaxGain()))
/* ========================================================================= */
// $ Source:GetMinGain
// < Gain:number=The current gain value.
// ? Returns the current minimum gain of the source.
/* ------------------------------------------------------------------------- */
LLFUNC(GetMinGain, 1, LuaUtilPushVar(lS, AgSource{lS, 1}().SourceGetMinGain()))
/* ========================================================================= */
// $ Source:GetPitch
// < Pitch:number=The current pitch value.
// ? Returns the current pitch of the source.
/* ------------------------------------------------------------------------- */
LLFUNC(GetPitch, 1, LuaUtilPushVar(lS, AgSource{lS, 1}().SourceGetPitch()))
/* ========================================================================= */
// $ Source:GetPosition
// < X:number=Current X position.
// < Y:number=Current Y position.
// < Z:number=Current Z position.
// ? Returns the current position of the source.
/* ------------------------------------------------------------------------- */
LLFUNC(GetPosition, 3,
  ALfloat fX, fY, fZ;
  AgSource{lS, 1}().SourceGetPosition(fX, fY, fZ);
  LuaUtilPushVar(lS, fX, fY, fZ))
/* ========================================================================= */
// $ Source:GetRefDist
// < Distance:number=The current reference distance.
// ? Returns the current reference distance of the source.
/* ------------------------------------------------------------------------- */
LLFUNC(GetRefDist, 1, LuaUtilPushVar(lS, AgSource{lS, 1}().SourceGetRefDist()))
/* ========================================================================= */
// $ Source:GetRelative
// < State:boolean=Is the source relative?
// ? Returns if the source is relative.
/* ------------------------------------------------------------------------- */
LLFUNC(GetRelative, 1,
  LuaUtilPushVar(lS, AgSource{lS, 1}().SourceGetRelative()))
/* ========================================================================= */
// $ Source:GetRollOff
// < RollOff:number=The current rolloff.
// ? Returns the current rolloff of the source.
/* ------------------------------------------------------------------------- */
LLFUNC(GetRollOff, 1, LuaUtilPushVar(lS, AgSource{lS, 1}().SourceGetRollOff()))
/* ========================================================================= */
// $ Source:GetSample
// < Sample:integer=The sample position of the current playback.
// ? Returns the current sample position of the sample being played.
/* ------------------------------------------------------------------------- */
LLFUNC(GetSample, 1, LuaUtilPushVar(lS, AgSource{lS, 1}().SourceGetSample()))
/* ========================================================================= */
// $ Source:GetState
// < State:integer=The current playback state.
// ? Returns if current playback state.
/* ------------------------------------------------------------------------- */
LLFUNC(GetState, 1, LuaUtilPushVar(lS, AgSource{lS, 1}().SourceGetState()))
/* ========================================================================= */
// $ Source:GetVelocity
// < X:number=Current X velocity.
// < Y:number=Current Y velocity.
// < Z:number=Current Z velocity.
// ? Returns the current velocity of the source.
/* ------------------------------------------------------------------------- */
LLFUNC(GetVelocity, 3,
  ALfloat fX, fY, fZ;
  AgSource{lS, 1}().SourceGetVelocity(fX, fY, fZ);
  LuaUtilPushVar(lS, fX, fY, fZ))
/* ========================================================================= */
// $ Source:IsDestroyed
// < Destroyed:boolean=If the Source class is destroyed
// ? Returns if the Source class is destroyed.
/* ------------------------------------------------------------------------- */
LLFUNC(IsDestroyed, 1,
  LuaUtilPushVar(lS, LuaUtilIsClassDestroyed(lS, cSources)))
/* ========================================================================= */
// $ Source:IsPlaying
// < Playing:boolean=Is the source playing?
// ? Return if source is playing.
/* ------------------------------------------------------------------------- */
LLFUNC(IsPlaying, 1, LuaUtilPushVar(lS, AgSource{lS, 1}().SourceIsPlaying()))
/* ========================================================================= */
// $ Source:IsStopped
// < Stopped:boolean=Is the source playing?
// ? Return if source is stopped.
/* ------------------------------------------------------------------------- */
LLFUNC(IsStopped, 1, LuaUtilPushVar(lS, AgSource{lS, 1}().SourceIsStopped()))
/* ========================================================================= */
// $ Source:IsValid
// < Valid:boolean=Is the buffer attached to the source valid?
// ? Return if source buffer is valid. If a Sample gets unloaded, then all of
// ? these methods are going to throw OpenAL exceptions. It is a good idea to
// ? keep a 'Sample' class created until the 'Source' class it spawned is
// ? destroyed.
/* ------------------------------------------------------------------------- */
LLFUNC(IsValid, 1, LuaUtilPushVar(lS, AgSource{lS, 1}().SourceIsValid()))
/* ========================================================================= */
// $ Source:Play
// < Success:boolean=Is the source now playing?
// ? Plays the source.
/* ------------------------------------------------------------------------- */
LLFUNC(Play, 1, LuaUtilPushVar(lS, AgSource{lS, 1}().SourcePlay()))
/* ========================================================================= */
// $ Source:PlayTwo
// < Success:boolean=Are the sources now playing?
// ? Plays two sources. Useful if you want to play a stereo source.
/* ------------------------------------------------------------------------- */
LLFUNC(PlayTwo, 0,
  AgSource aLeft{lS, 1},
           aRight{lS, 2};
  LuaUtilPushVar(lS, aLeft().SourcePlay(aRight())))
/* ========================================================================= */
// $ Source:SetByte
// > Byte:integer=Byte position.
// ? Sets the byte position to play at in the source.
/* ------------------------------------------------------------------------- */
LLFUNC(SetByte, 0,
  const AgSource aSource{lS, 1};
  const AgALint aByte{lS, 2};
  aSource().SourceSetByte(aByte))
/* ========================================================================= */
// $ Source:SetDirection
// > X:number=The new X direction.
// > Y:number=The new Y direction.
// > Z:number=The new Z direction.
// ? Sets the new direction of the source.
/* ------------------------------------------------------------------------- */
LLFUNC(SetDirection, 0,
  const AgSource aSource{lS, 1};
  const AgALfloat aX{lS, 2}, aY{lS, 3}, aZ{lS, 4};
  aSource().SourceSetDirection(aX, aY, aZ))
/* ========================================================================= */
// $ Source:SetElapsed
// > Seconds:number=Number of seconds elapsed.
// ? Sets the number of seconds elapsed in the source.
/* ------------------------------------------------------------------------- */
LLFUNC(SetElapsed, 0,
  const AgSource aSource{lS, 1};
  const AgALfloat aSeconds{lS, 2};
  aSource().SourceSetElapsed(aSeconds))
/* ========================================================================= */
// $ Source:SetGain
// > Gain:number=The new gain value.
// ? Sets the new gain of the source.
/* ------------------------------------------------------------------------- */
LLFUNC(SetGain, 0,
  const AgSource aSource{lS, 1};
  const AgALfloat aGain{lS, 2};
  aSource().SourceSetGain(aGain))
/* ========================================================================= */
// $ Source:SetLooping
// > Looping:boolean=The new looping state
// ? Sets if the source should be looping.
/* ------------------------------------------------------------------------- */
LLFUNC(SetLooping, 0,
  const AgSource aSource{lS, 1};
  const AgBoolean aLooping{lS, 2};
  aSource().SourceSetLooping(aLooping))
/* ========================================================================= */
// $ Source:SetMaxDist
// > Distance:number=The new maximum distance.
// ? Sets the maximum distance of the source.
/* ------------------------------------------------------------------------- */
LLFUNC(SetMaxDist, 0,
  const AgSource aSource{lS, 1};
  const AgALfloat aMaxDistance{lS, 2};
  aSource().SourceSetMaxDist(aMaxDistance))
/* ========================================================================= */
// $ Source:SetMaxGain
// > Gain:number=The new maximum  gain value.
// ? Sets a new maximum gain of the source.
/* ------------------------------------------------------------------------- */
LLFUNC(SetMaxGain, 0,
  const AgSource aSource{lS, 1};
  const AgALfloat aMaxGain{lS, 2};
  aSource().SourceSetMaxGain(aMaxGain))
/* ========================================================================= */
// $ Source:SetMinGain
// > Gain:number=The new minimum gain value.
// ? Sets the new minimum gain gain of the source.
/* ------------------------------------------------------------------------- */
LLFUNC(SetMinGain, 0,
  const AgSource aSource{lS, 1};
  const AgALfloat aMinGain{lS, 2};
  aSource().SourceSetMinGain(aMinGain))
/* ========================================================================= */
// $ Source:SetPitch
// > Pitch:number=The new pitch value.
// ? Sets a new pitch value of the source.
/* ------------------------------------------------------------------------- */
LLFUNC(SetPitch, 0,
  const AgSource aSource{lS, 1};
  const AgALfloat aPitch{lS, 2};
  aSource().SourceSetPitch(aPitch))
/* ========================================================================= */
// $ Source:SetPosition
// < X:number=The new X position.
// < Y:number=The new Y position.
// < Z:number=The new Z position.
// ? Sets the new position of the source.
/* ------------------------------------------------------------------------- */
LLFUNC(SetPosition, 0,
  const AgSource aSource{lS, 1};
  const AgALfloat aX{lS, 2}, aY{lS, 3}, aZ{lS, 4};
  aSource().SourceSetPosition(aX, aY, aZ))
/* ========================================================================= */
// $ Source:SetRefDist
// > Distance:number=The new reference distance.
// ? Sets the new reference distance of the source.
/* ------------------------------------------------------------------------- */
LLFUNC(SetRefDist, 0,
  const AgSource aSource{lS, 1};
  const AgALfloat aRefDistance{lS, 2};
  aSource().SourceSetRefDist(aRefDistance))
/* ========================================================================= */
// $ Source:SetRelative
// > Relative:boolean=Sets if the source is relative.
// ? Sets the new relative state of the source.
/* ------------------------------------------------------------------------- */
LLFUNC(SetRelative, 0,
  const AgSource aSource{lS, 1};
  const AgBoolean aRelative{lS, 2};
  aSource().SourceSetRelative(aRelative))
/* ========================================================================= */
// $ Source:SetRollOff
// > RollOff:number=The new rolloff.
// ? Sets the new rolloff of the source.
/* ------------------------------------------------------------------------- */
LLFUNC(SetRollOff, 0,
  const AgSource aSource{lS, 1};
  const AgALfloat aRollOff{lS, 2};
  aSource().SourceSetRollOff(aRollOff))
/* ========================================================================= */
// $ Source:SetSample
// > Sample:integer=Sample position.
// ? Sets the sample position to play at in the source.
/* ------------------------------------------------------------------------- */
LLFUNC(SetSample, 0,
  const AgSource aSource{lS, 1};
  const AgALint aSample{lS, 2};
  aSource().SourceSetSample(aSample))
/* ========================================================================= */
// $ Source:SetVelocity
// < X:number=The new X velocity.
// < Y:number=The new Y velocity.
// < Z:number=The new Z velocity.
// ? Sets the new velocity of the source.
/* ------------------------------------------------------------------------- */
LLFUNC(SetVelocity, 0,
  const AgSource aSource{lS, 1};
  const AgALfloat aX{lS, 2}, aY{lS, 3}, aZ{lS, 4};
  aSource().SourceSetVelocity(aX, aY, aZ))
/* ========================================================================= */
// $ Source:Stop
// ? Stops the source.
/* ------------------------------------------------------------------------- */
LLFUNC(Stop, 0, AgSource{lS, 1}().SourceStop())
/* ========================================================================= **
** ######################################################################### **
** ## Source:* methods structure                                          ## **
** ######################################################################### **
** ========================================================================= */
LLRSMFBEGIN                            // Source:* methods begin
  LLRSFUNC(Destroy), LLRSFUNC(GetByte), LLRSFUNC(GetDirection),
  LLRSFUNC(GetElapsed), LLRSFUNC(GetGain), LLRSFUNC(GetId),
  LLRSFUNC(GetLooping), LLRSFUNC(GetMaxDist), LLRSFUNC(GetMaxGain),
  LLRSFUNC(GetMinGain), LLRSFUNC(GetPitch), LLRSFUNC(GetPosition),
  LLRSFUNC(GetRefDist), LLRSFUNC(GetRelative), LLRSFUNC(GetRollOff),
  LLRSFUNC(GetSample), LLRSFUNC(GetState), LLRSFUNC(GetVelocity),
  LLRSFUNC(IsDestroyed), LLRSFUNC(IsPlaying), LLRSFUNC(IsStopped),
  LLRSFUNC(IsValid), LLRSFUNC(Play), LLRSFUNC(PlayTwo), LLRSFUNC(SetByte),
  LLRSFUNC(SetDirection), LLRSFUNC(SetElapsed), LLRSFUNC(SetGain),
  LLRSFUNC(SetLooping), LLRSFUNC(SetMaxDist), LLRSFUNC(SetMaxGain),
  LLRSFUNC(SetMinGain), LLRSFUNC(SetPitch), LLRSFUNC(SetPosition),
  LLRSFUNC(SetRefDist), LLRSFUNC(SetRelative), LLRSFUNC(SetSample),
  LLRSFUNC(SetRollOff), LLRSFUNC(SetVelocity), LLRSFUNC(Stop),
LLRSEND                                // Source:* methods end
/* ========================================================================= **
** ######################################################################### **
** ## Source.* non-member functions                                       ## **
** ######################################################################### **
** ========================================================================= */
// $ Source.GetClasses
// < Count:integer=Total number of sources created.
// ? Returns the total number of source classes currently active.
/* ------------------------------------------------------------------------- */
LLFUNC(GetClasses, 1, LuaUtilPushVar(lS, cSources->CollectorCount()))
/* ========================================================================= **
** ######################################################################### **
** ## Source.* non-member functions structure                             ## **
** ######################################################################### **
** ========================================================================= */
LLRSBEGIN                              // Source.* non-member functions begin
  LLRSFUNC(GetClasses),
LLRSEND                                // Source.* non-member functions end
/* ========================================================================= */
}                                      // End of Stream namespace
/* == EoF =========================================================== EoF == */
