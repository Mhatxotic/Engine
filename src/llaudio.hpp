/* == LLAUDIO.HPP ========================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## Defines the 'Audio' namespace and methods for the guest to use in   ## **
** ## Lua. This file is invoked by 'lualib.hpp'.                          ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
#pragma once                           // Only one incursion allowed
/* ========================================================================= **
** ######################################################################### **
** ========================================================================= */
// % Audio
/* ------------------------------------------------------------------------- */
// ! The audio class allows control of audio subsystem.
/* ========================================================================= */
namespace LLAudio {                    // Audio namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IAudio::P;             using namespace ISample::P;
using namespace ISource::P;            using namespace IStream::P;
using namespace IVideo::P;             using namespace Common;
/* ========================================================================= **
** ######################################################################### **
** ## Audio.* namespace functions                                         ## **
** ######################################################################### **
** ========================================================================= */
// $ Audio.GetGlobalVolume
// < Volume:number=Current master volume (0 to 1).
// ? Returns global/master volume.
/* ------------------------------------------------------------------------- */
LLFUNC(GetGlobalVolume, 1, LuaUtilPushVar(lS, cSources->fGVolume.load()))
/* ========================================================================= */
// $ Audio.GetStreamVolume
// < Volume:number=Current streams volume (0 to 1).
// ? Returns master volume of all stream classes.
/* ------------------------------------------------------------------------- */
LLFUNC(GetStreamVolume, 1, LuaUtilPushVar(lS, cSources->fMVolume.load()))
/* ========================================================================= */
// $ Audio.GetSampleVolume
// < Volume:number=Current samples volume (0 to 1).
// ? Returns master volume of all sample classes.
/* ------------------------------------------------------------------------- */
LLFUNC(GetSampleVolume, 1, LuaUtilPushVar(lS, cSources->fSVolume.load()))
/* ========================================================================= */
// $ Audio.GetVideoVolume
// < Volume:number=Current samples volume (0 to 1).
// ? Returns master volume of all video classes.
/* ------------------------------------------------------------------------- */
LLFUNC(GetVideoVolume, 1, LuaUtilPushVar(lS, cSources->fVVolume.load()))
/* ========================================================================= */
// $ Audio.SetGlobalVolume
// > Volume:number=New master volume (0 to 1).
// ? Sets new master/global volume of all current and new streams/samples. This
// ? does not modify the cvar which controls the default sample volume.
/* ------------------------------------------------------------------------- */
LLFUNC(SetGlobalVolume, 0, cAudio->SetGlobalVolume(AgVolume{lS, 1}))
/* ========================================================================= */
// $ Audio.SetStreamVolume
// > Volume:number=New streams volume (0 to 1).
// ? Sets new volume of all current and new streams. This does not modify the
// ? cvar which controls the default sample volume.
/* ------------------------------------------------------------------------- */
LLFUNC(SetStreamVolume, 0, StreamSetVolume(AgVolume{lS, 1}))
/* ========================================================================= */
// $ Audio.SetSampleVolume
// > Volume:number=New samples volume (0 to 1).
// ? Sets new volume of all current and new samples. This does not modify the
// ? cvar which controls the default sample volume.
/* ------------------------------------------------------------------------- */
LLFUNC(SetSampleVolume, 0, SampleSetVolume(AgVolume{lS, 1}))
/* ========================================================================= */
// $ Audio.SetVideoVolume
// > Volume:number=New samples volume (0 to 1).
// ? Sets new volume of all current and new videos. This does not modify the
// ? cvar which controls the default video volume.
/* ------------------------------------------------------------------------- */
LLFUNC(SetVideoVolume, 0, VideoSetVolume(AgVolume{lS, 1}))
/* ========================================================================= */
// $ Audio.SetPosition
// > X:number=New X listener position.
// > Y:number=New Y listener position.
// > Z:number=New Z listener position.
// ? Sets the new audio listener position. This would normally be equal to the
// ? position of the game's protagonist character (if applicable).
/* ------------------------------------------------------------------------- */
LLFUNC(SetPosition, 0,
  const AgALfloat aX{lS, 1}, aY{lS, 2}, aZ{lS, 3};
  cAudio->SetPosition(aX, aY, aZ))
/* ========================================================================= */
// $ Audio.SetOrientation
// > XLook:number=New X listener look orientation.
// > YLook:number=New Y listener look orientation.
// > ZLook:number=New Z listener look orientation.
// > XUp:number=New X listener up orientation.
// > YUp:number=New Y listener up orientation.
// > ZUp:number=New Z listener up orientation.
// ? Sets the new audio listener orientation. This would normally be equal to
// ? orientation of the game's protagonist character (if applicable).
/* ------------------------------------------------------------------------- */
LLFUNC(SetOrientation, 0,
  const AgALfloat aXLook{lS, 1}, aYLook{lS, 2}, aZLook{lS, 3},
                  aXUp{lS, 4}, aYUp{lS, 5}, aZUp{lS, 6};
  cAudio->SetOrientation(aXLook, aYLook, aZLook, aXUp, aYUp, aZUp))
/* ========================================================================= */
// $ Audio.SetVelocity
// > X:number=New X listener velocity.
// > Y:number=New Y listener velocity.
// > Z:number=New Z listener velocity.
// ? Sets the new audio listener velocity. This would normally be equal to
// ? velocity of the game's protagonist character (if applicable).
/* ------------------------------------------------------------------------- */
LLFUNC(SetVelocity, 0,
  const AgALfloat aX{lS, 1}, aY{lS, 2}, aZ{lS, 3};
  cAudio->SetVelocity(aX, aY, aZ))
/* ========================================================================= */
// $ Audio.GetPBDeviceName
// > Id:integer=The playback audio device id.
// < Name:string=The name of the audio playback device.
// ? Returns the name of the playback audio device at id.
/* ------------------------------------------------------------------------- */
LLFUNC(GetPBDeviceName, 1, LuaUtilPushVar(lS, cAudio->GetPlaybackDeviceById(
  AgSizeTLG{lS, 1, 0, cAudio->GetNumPlaybackDevices()})))
/* ========================================================================= */
// $ Audio.GetNumPBDevices
// < Count:integer=The number of playback devices detected.
// ? Returns the number of audio playback devices detected by OpenAL.
/* ------------------------------------------------------------------------- */
LLFUNC(GetNumPBDevices, 1,
  LuaUtilPushVar(lS, cAudio->GetNumPlaybackDevices()))
/* ========================================================================= */
// $ Audio.Reset
// < Result:boolean = Was the event sent successfully?
// ? Resets the audio subsystem completely. Currently playing streams and
// ? samples should resume playing after the reset. The function will return
// ? 'false' if the request was already sent.
/* ------------------------------------------------------------------------- */
LLFUNC(Reset, 1, LuaUtilPushVar(lS, cAudio->ReInit()))
/* ========================================================================= **
** ######################################################################### **
** ## Audio.* namespace functions structure                               ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
LLRSBEGIN                              // Audio.* namespace functions begin
  LLRSFUNC(GetGlobalVolume),           LLRSFUNC(GetNumPBDevices),
  LLRSFUNC(GetPBDeviceName),           LLRSFUNC(GetSampleVolume),
  LLRSFUNC(GetStreamVolume),           LLRSFUNC(GetVideoVolume),
  LLRSFUNC(Reset),                     LLRSFUNC(SetGlobalVolume),
  LLRSFUNC(SetOrientation),            LLRSFUNC(SetPosition),
  LLRSFUNC(SetSampleVolume),           LLRSFUNC(SetVelocity),
  LLRSFUNC(SetStreamVolume),           LLRSFUNC(SetVideoVolume),
LLRSEND                                // Audio.* namespace functions end
/* ========================================================================= */
}                                      // End of Audio namespace
/* == EoF =========================================================== EoF == */
