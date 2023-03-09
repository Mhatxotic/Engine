/* == LLVIDEO.HPP ========================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## Defines the 'Video' namespace and methods for the guest to use in   ## **
** ## Lua. This file is invoked by 'lualib.hpp'.                          ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
#pragma once                           // Only one incursion allowed
/* ========================================================================= **
** ######################################################################### **
** ========================================================================= */
// % Video
/* ------------------------------------------------------------------------- */
// ! The video class allows loading of video streams using the Theora video
// ! codec, vorbis audio codec and ogg format container (.OGV) which will
// ! playback into a frame buffer object.
/* ========================================================================= */
namespace LLVideo {                    // Video namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IAsset::P;             using namespace IFbo::P;
using namespace IVideo::P;             using namespace Common;
/* ========================================================================= **
** ######################################################################### **
** ## Video common helper classes                                         ## **
** ######################################################################### **
** -- Read Video class argument -------------------------------------------- */
struct AgVideo : public ArClass<Video> {
  explicit AgVideo(lua_State*const lS, const int iArg) :
    ArClass{*LuaUtilGetPtr<Video>(lS, iArg, *cVideos)}{} };
/* -- Create Video class argument ------------------------------------------ */
struct AcVideo : public ArClass<Video> {
  explicit AcVideo(lua_State*const lS) :
    ArClass{*LuaUtilClassCreate<Video>(lS, *cVideos)}{} };
/* ========================================================================= **
** ######################################################################### **
** ## Video:* member functions                                            ## **
** ######################################################################### **
** ========================================================================= */
// $ Video:Advance
// ? Advances one frame. The video must be paused or stopped and it only
// ? happens in the engine thread and not its own thread.
/* ------------------------------------------------------------------------- */
LLFUNC(Advance, 0, AgVideo{lS, 1}().Advance())
/* ========================================================================= */
// $ Video:Awaken
// ? Initialises the audio and video structures after loading or after being
// ? manually stopped.
/* ------------------------------------------------------------------------- */
LLFUNC(Awaken, 0, AgVideo{lS, 1}().Awaken())
/** ======================================================================== */
// $ Video:Blit
// ? Blits the specified video texture frame to the currently ACTIVE fbo. This
// ? just adds to the active FBO drawing arrays and doesn't actually render
// ? until the active fbo is finished and rendered. The currently stored
// ? vertex, texcoord, colour and matrix values are used.
/* ------------------------------------------------------------------------- */
LLFUNC(Blit, 0, AgVideo{lS, 1}().Blit())
/* ========================================================================= */
// $ Video:BlitT
// ? TriIndex:integer=The specified triangle id.
// ? Blits the specified video texture frame to the currently ACTIVE fbo. This
// ? just adds to the active FBO drawing arrays and doesn't actually render
// ? until the active fbo is finished and rendered. The currently stored
// ? vertex, texcoord, colour and matrix values are used.
/* ------------------------------------------------------------------------- */
LLFUNC(BlitT, 0,
  const AgVideo aVideo{lS, 1};
  const AgTriangleId aTriangleId{lS, 2};
  aVideo().BlitTri(aTriangleId))
/* ========================================================================= */
// $ Video:Destroy
// ? Stops and destroys the stream object and frees all the memory associated
// ? with it. The OpenGL handles and VRAM associated with the object will be
// ? freed after the main FBO has been rendered. The object will no longer be
// ? useable after this call and an error will be generated if accessed.
/* ------------------------------------------------------------------------- */
LLFUNC(Destroy, 0, LuaUtilClassDestroy<Video>(lS, 1, *cVideos))
/* ========================================================================= */
// $ Video:GetATime
// < Seconds:Number=Returns the elapsed audio time in seconds
// ? Returns the elapsed video time in seconds
/* ------------------------------------------------------------------------- */
LLFUNC(GetATime, 1, LuaUtilPushVar(lS, AgVideo{lS, 1}().GetAudioTime()))
/* ========================================================================= */
// $ Video:GetDrift
// < Seconds:Number=Drift time in seconds.
// ? Returns the drift between audio and video sync.
/* ------------------------------------------------------------------------- */
LLFUNC(GetDrift, 1, LuaUtilPushVar(lS, AgVideo{lS, 1}().GetDrift()))
/* ========================================================================= */
// $ Video:GetFrame
// < Frame:Integer=The current frame
// ? Returns the current frame number
/* ------------------------------------------------------------------------- */
LLFUNC(GetFrame, 1, LuaUtilPushVar(lS, AgVideo{lS, 1}().GetFrame()))
/* ========================================================================= */
// $ Video:GetFrames
// < Frames:Integer=Returns the frames decoded
// ? Returns the number of frames decoded
/* ------------------------------------------------------------------------- */
LLFUNC(GetFrames, 1, LuaUtilPushVar(lS, AgVideo{lS, 1}().GetFrames()))
/* ========================================================================= */
// $ Video:GetFramesLost
// < Frames:Integer=Returns the frames that were skipped
// ? Returns the number of skipped frames decoded
/* ------------------------------------------------------------------------- */
LLFUNC(GetFramesLost, 1,
  LuaUtilPushVar(lS, AgVideo{lS, 1}().GetFramesSkipped()))
/* ========================================================================= */
// $ Video:GetFPS
// < FPS:Number=The FPS rate of the video
// ? Returns the FPS rate of the video
/* ------------------------------------------------------------------------- */
LLFUNC(GetFPS, 1, LuaUtilPushVar(lS, AgVideo{lS, 1}().GetFPS()))
/* ========================================================================= */
// $ Video:GetHeight
// < Width:integer=The height of the video
// ? Returns the height of the video
/* ------------------------------------------------------------------------- */
LLFUNC(GetHeight, 1, LuaUtilPushVar(lS, AgVideo{lS, 1}().GetHeight()))
/* ========================================================================= */
// $ Video:GetId
// < Id:integer=The id number of the Video object.
// ? Returns the unique id of the Video object.
/* ------------------------------------------------------------------------- */
LLFUNC(GetId, 1, LuaUtilPushVar(lS, AgVideo{lS, 1}().CtrGet()))
/* ========================================================================= */
// $ Video:GetLoop
// < Count:integer=Number of loops left.
// ? Returns the number of loops remaining of playback.
/* ------------------------------------------------------------------------- */
LLFUNC(GetLoop, 1, LuaUtilPushVar(lS, AgVideo{lS, 1}().GetLoop()))
/* ========================================================================= */
// $ Video:GetName
// < Id:string=The video identifier
// ? Returns the identifier of the video, usually the filename.
/* ------------------------------------------------------------------------- */
LLFUNC(GetName, 1, LuaUtilPushVar(lS, AgVideo{lS, 1}().IdentGet()))
/* ========================================================================= */
// $ Video:GetPlaying
// < Status:boolean=Playback status
// ? Returns the playback status
/* ------------------------------------------------------------------------- */
LLFUNC(GetPlaying, 1, LuaUtilPushVar(lS, AgVideo{lS, 1}().IsPlaying()))
/* ========================================================================= */
// $ Video:GetTime
// < Seconds:Number=Returns the elapsed video time in seconds
// ? Returns the elapsed audio time in seconds
/* ------------------------------------------------------------------------- */
LLFUNC(GetTime, 1, LuaUtilPushVar(lS, AgVideo{lS, 1}().GetVideoTime()))
/* ========================================================================= */
// $ Video:GetWidth
// < Width:integer=The width of the video
// ? Returns the width of the video
/* ------------------------------------------------------------------------- */
LLFUNC(GetWidth, 1, LuaUtilPushVar(lS, AgVideo{lS, 1}().GetWidth()))
/* ========================================================================= */
// $ Video:OnEvent
// > Func:function=The callback function to call when an event occurds. This
// ? function is called when the video stops, starts or loops. The callback
// ? function syntax is 'function(EventType:Integer)'. The valid values that
// ? the callback can send are in the 'Video.Events' table.
/* ------------------------------------------------------------------------- */
LLFUNC(OnEvent, 0, AgVideo{lS, 1}().LuaEvtInit(lS))
/* ========================================================================= */
// $ Video:Pause
// ? Pauses the specified video.
/* ------------------------------------------------------------------------- */
LLFUNC(Pause, 0, AgVideo{lS, 1}().Pause())
/* ========================================================================= */
// $ Video:Play
// ? Plays the specified video.
/* ------------------------------------------------------------------------- */
LLFUNC(Play, 0, AgVideo{lS, 1}().Play())
/* ========================================================================= */
// $ Video:Rewind
// ? Rewinds the specified video back to the beginning.
/* ------------------------------------------------------------------------- */
LLFUNC(Rewind, 0, AgVideo{lS, 1}().Rewind())
/* ========================================================================= */
// $ Video:SetKeyed
// < Keyed:boolean=Video should be keyed with the set colour
// ? Enabling this allowed the video to be chroma keyed. This also disables
// ? changing the colour intensity because the specified colour using SetCRGBA
// ? will set the colour to be keyed. If the pixel is greater or equal then the
// ? specified colour then the pixel is set to the specified alpha.
/* ------------------------------------------------------------------------- */
LLFUNC(SetKeyed, 0, AgVideo{lS, 1}().SetKeyed(AgBoolean{lS, 2}))
/* ========================================================================= */
// $ Video:SetCRGBA
// > Red:number=The entire fbo texture red colour intensity (0 to 1).
// > Green:number=The entire fbo texture green colour intensity (0 to 1).
// > Blue:number=The entire fbo texture blue colour intensity (0 to 1).
// > Alpha:number=The entire fbo texture alpha colour intensity (0 to 1).
// ? Sets the colour intensity of all the vertexes for the entire video frame
// ? texture.
/* ------------------------------------------------------------------------- */
LLFUNC(SetCRGBA, 0,
  const AgVideo aVideo{lS, 1};
  const AgGLfloat aRed{lS, 2},
                  aGreen{lS, 3},
                  aBlue{lS, 4},
                  aAlpha{lS, 5};
  aVideo().FboItemSetQuadRGBA(aRed, aGreen, aBlue, aAlpha))
/* ========================================================================= */
// $ Video:SetCX
// > TriIndex:integer=Triangle index. Triangle #1 (zero) or triangle #2 (one).
// > V1Red:number=Red component of vertex #1 of the specified triangle.
// > V1Green:number=Green component of vertex #1 of the specified triangle.
// > V1Blue:number=Blue component of vertex #1 of the specified triangle.
// > V1Alpha:number=Alpha component of vertex #1 of the specified triangle.
// > V2Red:number=Red component of vertex #2 of the specified triangle.
// > V2Green:number=Green component of vertex #2 of the specified triangle.
// > V2Blue:number=Blue component of vertex #2 of the specified triangle.
// > V2Alpha:number=Alpha component of vertex #2 of the specified triangle.
// > V3Red:number=Red component of vertex #3 of the specified triangle.
// > V3Green:number=Green component of vertex #3 of the specified triangle.
// > V3Blue:number=Blue component of vertex #3 of the specified triangle.
// > V3Alpha:number=Alpha component of vertex #3 of the specified triangle.
// ? Stores the specified colour intensities on each vertex for the
// ? Video:Blit() function.
/* ------------------------------------------------------------------------- */
LLFUNC(SetCX, 0,
  const AgVideo aVideo{lS, 1};
  const AgTriangleId aTriangleId{lS, 2};
  const FboBase::TriColData tpdData{
    AgGLfloat{lS,  3}, AgGLfloat{lS,  4}, AgGLfloat{lS,  5}, AgGLfloat{lS,  6},
    AgGLfloat{lS,  7}, AgGLfloat{lS,  8}, AgGLfloat{lS,  9}, AgGLfloat{lS, 10},
    AgGLfloat{lS, 11}, AgGLfloat{lS, 12}, AgGLfloat{lS, 13}, AgGLfloat{lS, 14}
  };
  aVideo().FboItemSetColourEx(aTriangleId, tpdData))
/* ========================================================================= */
// $ Video:SetFilter
// < Filter:boolean=Video should be filtered by OpenGL
// ? Sets GL_LINEAR filtering on the video if true, GL_NEAREST if not.
/* ------------------------------------------------------------------------- */
LLFUNC(SetFilter, 0, AgVideo{lS, 1}().SetFilter(AgBoolean{lS, 2}))
/* ========================================================================= */
// $ Video:SetKeyColour
// > Red:number=The R component to phase out
// > Green:number=The G component to phase out
// > Blue:number=The B component to phase out
// > Intensity:number=The intensity of the phasing out
// ? Allows a certain colour to be phased out on the video. You must enable
// ? keying colour with the SetKeyed() function before using this
/* ------------------------------------------------------------------------- */
LLFUNC(SetKeyColour, 0,
  const AgVideo aVideo{lS, 1};
  const AgGLfloat aRed{lS, 2},
                  aGreen{lS, 3},
                  aBlue{lS, 4},
                  aIntensity{lS, 5};
  aVideo().fboYCbCr.FboItemSetQuadRGBA(aRed, aGreen, aBlue, aIntensity))
/* ========================================================================= */
// $ Video:SetLoop
// > Count:integer=New playback loop count (-1 = infinity).
// ? Sets the new playback loop count.
/* ------------------------------------------------------------------------- */
LLFUNC(SetLoop, 0, AgVideo{lS, 1}().SetLoop(AgSizeT{lS, 2}))
/* ========================================================================= */
// $ Video:SetTCLTRB
// > Left:number=The left co-ordinate.
// > Top:number=The top co-ordinate.
// > Right:number=The right co-ordinate.
// > Bottom:number=The bottom co-ordinate.
// ? Allows you to set basic texture co-ordinates when blitting the video frame
// ? texture.
/* ------------------------------------------------------------------------- */
LLFUNC(SetTCLTRB, 0,
  const AgVideo aVideo{lS, 1};
  const AgGLfloat aLeft{lS, 2},
                  aTop{lS, 3},
                  aRight{lS, 4},
                  aBottom{lS, 5};
  aVideo().FboItemSetTexCoord(aLeft, aTop, aRight, aBottom))
/* ========================================================================= */
// $ Video:SetTCLTWH
// > Left:number=The left co-ordinate.
// > Top:number=The top co-ordinate.
// > Width:number=The width of the tile.
// > Height:number=The height of the tile.
// ? Allows you to set basic texture co-ordinates when blitting the video frame
// ? texture.
/* ------------------------------------------------------------------------- */
LLFUNC(SetTCLTWH, 0,
  const AgVideo aVideo{lS, 1};
  const AgGLfloat aLeft{lS, 2},
                  aTop{lS, 3},
                  aWidth{lS, 4},
                  aHeight{lS, 5};
  aVideo().FboItemSetTexCoordWH(aLeft, aTop, aWidth, aHeight))
/* ========================================================================= */
// $ Video:SetTCX
// > TriIndex:integer=Triangle index. Triangle #1 (zero) or triangle #2 (one).
// > TC1Left:number=Coord #1 of vertex #1 of the specified triangle.
// > TC1Top:number=Coord #2 of vertex #2 of the specified triangle.
// > TC2Left:number=Coord #1 of vertex #3 of the specified triangle.
// > TC2Top:number=Coord #2 of vertex #1 of the specified triangle.
// > TC3Left:number=Coord #1 of vertex #2 of the specified triangle.
// > TC3Top:number=Coord #2 of vertex #3 of the specified triangle.
// ? Stores the specified texture co-ordinates for when the frame buffer object
// ? is drawn.
/* ------------------------------------------------------------------------- */
LLFUNC(SetTCX, 0,
  const AgVideo aVideo{lS, 1};
  const AgTriangleId aTriangleId{lS, 2};
  const FboBase::TriCoordData tcdNew{
    AgGLfloat{lS, 3}, AgGLfloat{lS, 4}, AgGLfloat{lS, 5},
    AgGLfloat{lS, 6}, AgGLfloat{lS, 7}, AgGLfloat{lS, 8}
  };
  aVideo().FboItemSetTexCoordEx(aTriangleId, tcdNew))
/* ========================================================================= */
// $ Video:SetVolume
// > Volume:number=New video volume (0 to 1).
// ? Sets the volume of the video class even if it is playing.
/* ------------------------------------------------------------------------- */
LLFUNC(SetVolume, 0, AgVideo{lS, 1}().SetVolume(AgVolume{lS, 2}))
/* ========================================================================= */
// $ Video:SetVLTRB
// > Left:number=The destination left co-ordinate.
// > Top:number=The destination top co-ordinate.
// > Right:number=The destination right co-ordinate.
// > Bottom:number=The destination bottom co-ordinate.
// ? Allows you to set basic vertex bounds when blitting the frame buffer
// ? object.
/* ------------------------------------------------------------------------- */
LLFUNC(SetVLTRB, 0,
  const AgVideo aVideo{lS, 1};
  const AgGLfloat aLeft{lS, 2},
                  aTop{lS, 3},
                  aRight{lS, 4},
                  aBottom{lS, 5};
  aVideo().FboItemSetVertex(aLeft, aTop, aRight, aBottom))
/* ========================================================================= */
// $ Video:SetVLTWH
// > Left:number=The destination left co-ordinate.
// > Top:number=The destination top co-ordinate.
// > Width:number=The destination width.
// > Height:number=The destination height.
// ? Allows you to set basic vertex co-ordinates and dimensions when blitting
// ? the videos frame buffer object.
/* ------------------------------------------------------------------------- */
LLFUNC(SetVLTWH, 0,
  const AgVideo aVideo{lS, 1};
  const AgGLfloat aLeft{lS, 2},
                  aTop{lS, 3},
                  aWidth{lS, 4},
                  aHeight{lS, 5};
  aVideo().FboItemSetVertexWH(aLeft, aTop, aWidth, aHeight))
/* ========================================================================= */
// $ Video:SetVLTWHA
// > Left:number=The destination left co-ordinate.
// > Top:number=The destination top co-ordinate.
// > Width:number=The destination width.
// > Height:number=The destination height.
// > Angle:number=The angle of the vertex
// ? Allows you to set basic vertex co-ordinates, dimensions and angle when
// ? blitting the video's frame buffer object.
/* ------------------------------------------------------------------------- */
LLFUNC(SetVLTWHA, 0,
  const AgVideo aVideo{lS, 1};
  const AgGLfloat aLeft{lS, 2},
                  aTop{lS, 3},
                  aWidth{lS, 4},
                  aHeight{lS, 5};
  const AgAngle aAngle{lS, 6};
  aVideo().FboItemSetVertexWH(aLeft, aTop, aWidth, aHeight, aAngle))
/* ========================================================================= */
// $ Video:SetVX
// > TriIndex:integer=Triangle index. Triangle #1 (zero) or triangle #2 (one).
// > V1Left:number=Coord #1 of vertex #1 of the specified triangle.
// > V1Top:number=Coord #2 of vertex #2 of the specified triangle.
// > V2Left:number=Coord #1 of vertex #3 of the specified triangle.
// > V2Top:number=Coord #2 of vertex #1 of the specified triangle.
// > V3Left:number=Coord #1 of vertex #2 of the specified triangle.
// > V3Top:number=Coord #2 of vertex #3 of the specified triangle.
// ? Stores the specified vertex co-ordinates when using the 'Video:Blit'
// ? function.
/* ------------------------------------------------------------------------- */
LLFUNC(SetVX, 0,
  const AgVideo aVideo{lS, 1};
  const AgTriangleId aTriangleId{lS, 2};
  const FboBase::TriPosData tpdNew{
    AgGLfloat{lS, 3}, AgGLfloat{lS, 4}, AgGLfloat{lS, 5},
    AgGLfloat{lS, 6}, AgGLfloat{lS, 7}, AgGLfloat{lS, 8}
  };
  aVideo().FboItemSetVertexEx(aTriangleId, tpdNew))
/* ========================================================================= */
// $ Video:Stop
// ? Stops and unloads the specified video, this also unloads the audio and
// ? video output structures.
/* ------------------------------------------------------------------------- */
LLFUNC(Stop, 0, AgVideo{lS, 1}().Stop())
/* ========================================================================= **
** ######################################################################### **
** ## Video:* member functions structure                                  ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
LLRSMFBEGIN                            // Video:* member functions begin
  LLRSFUNC(Advance),
  LLRSFUNC(Awaken),        LLRSFUNC(Blit),      LLRSFUNC(BlitT),
  LLRSFUNC(Destroy),       LLRSFUNC(GetATime),  LLRSFUNC(GetDrift),
  LLRSFUNC(GetFPS),        LLRSFUNC(GetFrame),  LLRSFUNC(GetFrames),
  LLRSFUNC(GetFramesLost), LLRSFUNC(GetHeight), LLRSFUNC(GetId),
  LLRSFUNC(GetLoop),       LLRSFUNC(GetName),   LLRSFUNC(GetPlaying),
  LLRSFUNC(GetTime),       LLRSFUNC(GetWidth),  LLRSFUNC(OnEvent),
  LLRSFUNC(Pause),         LLRSFUNC(Play),      LLRSFUNC(Rewind),
  LLRSFUNC(SetCRGBA),      LLRSFUNC(SetCX),     LLRSFUNC(SetFilter),
  LLRSFUNC(SetKeyColour),  LLRSFUNC(SetKeyed),  LLRSFUNC(SetLoop),
  LLRSFUNC(SetTCLTRB),     LLRSFUNC(SetTCLTWH), LLRSFUNC(SetTCX),
  LLRSFUNC(SetVLTRB),      LLRSFUNC(SetVLTWH),  LLRSFUNC(SetVLTWHA),
  LLRSFUNC(SetVX),         LLRSFUNC(SetVolume), LLRSFUNC(Stop),
LLRSEND                                // Video:* member functions end
/* ========================================================================= */
// $ Video.Asset
// > Id:String=The identifier of the string
// > Data:Asset=The file data of the ogg file to load
// < Handle:Ogg=The ogg object
// ? Loads an ogv video file to stream on the main thread from the specified
// ? array object.
/* ------------------------------------------------------------------------- */
LLFUNC(Asset, 1,
  const AgNeString aIdentifier{lS, 1};
  const AgAsset aAsset{lS, 2};
  AcVideo{lS}().SyncInitArray(aIdentifier, aAsset))
/* ========================================================================= */
// $ Video.AssetAsync
// > Id:String=The identifier of the string
// > Data:array=The data of the ogg file to load
// > ErrorFunc:function=The function to call when there is an error
// > ProgressFunc:function=The function to call during initial IO loading.
// > SuccessFunc:function=The function to call when the audio file is laoded
// ? Loads an ogv file off the main thread from the specified array object.
// ? The callback functions send an argument to the video stream object that
// ? was created.
/* ------------------------------------------------------------------------- */
LLFUNC(AssetAsync, 0,
  LuaUtilCheckParams(lS, 5);
  const AgNeString aIdentifier{lS, 1};
  const AgAsset aAsset{lS, 2};
  LuaUtilCheckFunc(lS, 3, 4, 5);
  AcVideo{lS}().AsyncInitArray(lS, aIdentifier, "videoarray", aAsset))
/* ========================================================================= */
// $ Video.File
// > Filename:string=The filename of the ogg file to load
// < Handle:Video=The video stream object
// ? Loads a video stream sample on the main thread from the specified file
// ? on disk. Returns the video stream object.
/* ------------------------------------------------------------------------- */
LLFUNC(File, 1, const AgFilename aFilename{lS, 1};
  AcVideo{lS}().SyncInitFileSafe(aFilename))
/* ========================================================================= */
// $ Video.FileAsync
// > Filename:string=The filename of the ogv to load
// > ErrorFunc:function=The function to call when there is an error
// > ProgressFunc:function=The function to call during initial IO loading.
// > SuccessFunc:function=The function to call when the file is laoded
// ? Loads a streamable ogv file off the main thread. The callback functions
// ? send an argument to the video stream object that was created.
/* ------------------------------------------------------------------------- */
LLFUNC(FileAsync, 0,
  LuaUtilCheckParams(lS, 4);
  const AgFilename aFilename{lS, 1};
  LuaUtilCheckFunc(lS, 2, 3, 4);
  AcVideo{lS}().AsyncInitFile(lS, aFilename, "videofile"))
/* ========================================================================= */
// $ Video.WaitAsync
// ? Halts main-thread execution until all async video stream events have
// ? completed.
/* ------------------------------------------------------------------------- */
LLFUNC(WaitAsync, 0, cVideos->WaitAsync())
/* ========================================================================= */
// $ Video.ClearEvents
// ? Clear the OnEvent callback on all Video objects. This might be useful for
// ? when you use the OnEvent() function and the Video object is still
// ? referenced and no longer accessable by your code in which the result is
// ? that the Video object does not get garbage collected.
/* ------------------------------------------------------------------------- */
LLFUNC(ClearEvents, 0, VideoClearEvents())
/* ========================================================================= **
** ######################################################################### **
** ## Video.* namespace functions structure                               ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
LLRSBEGIN                              // Video.* namespace functions begin
  LLRSFUNC(Asset), LLRSFUNC(AssetAsync), LLRSFUNC(ClearEvents),
  LLRSFUNC(File),  LLRSFUNC(FileAsync),  LLRSFUNC(WaitAsync),
LLRSEND                                // Video.* namespace functions end
/* ========================================================================= **
** ######################################################################### **
** ## Video.* namespace constants                                         ## **
** ######################################################################### **
** ========================================================================= */
// @ Video.Events
// < Codes:table=The table of key/value pairs of available flags
// ? Returns possible values for Video:OnEvent() event command.
/* ------------------------------------------------------------------------- */
LLRSKTBEGIN(Events)                    // Beginning of Stream event flags
  LLRSKTITEM(Video::VE_,FINISH),       LLRSKTITEM(Video::VE_,LOOP),
  LLRSKTITEM(Video::VE_,PAUSE),        LLRSKTITEM(Video::VE_,PLAY),
  LLRSKTITEM(Video::VE_,STOP),
LLRSKTEND                              // End of Stream event flags
/* ========================================================================= **
** ######################################################################### **
** ## Video.* namespace constants structure                               ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
LLRSCONSTBEGIN                         // Video.* namespace consts begin
  LLRSCONST(Events),
LLRSCONSTEND                           // Video.* namespace consts end
/* ========================================================================= */
}                                      // End of Video namespace
/* == EoF =========================================================== EoF == */
