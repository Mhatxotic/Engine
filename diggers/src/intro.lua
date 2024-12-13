-- INTRO.LUA =============================================================== --
-- ooooooo.--ooooooo--.ooooo.-----.ooooo.--oooooooooo-oooooooo.----.ooooo..o --
-- 888'-`Y8b--`888'--d8P'-`Y8b---d8P'-`Y8b-`888'---`8-`888--`Y88.-d8P'---`Y8 --
-- 888----888--888--888---------888---------888--------888--.d88'-Y88bo.---- --
-- 888----888--888--888---------888---------888oo8-----888oo88P'---`"Y888o.- --
-- 888----888--888--888----oOOo-888----oOOo-888--"-----888`8b.--------`"Y88b --
-- 888---d88'--888--`88.---.88'-`88.---.88'-888-----o--888-`88b.--oo----.d8P --
-- 888bd8P'--oo888oo-`Y8bod8P'---`Y8bod8P'-o888ooood8-o888o-o888o-8""8888P'- --
-- ========================================================================= --
-- (c) Mhatxotic Design, 2024          (c) Millennium Interactive Ltd., 1994 --
-- ========================================================================= --
-- Core function aliases --------------------------------------------------- --
local error<const> = error;
-- M-Engine function aliases ----------------------------------------------- --
local UtilBlank<const>, CoreTime<const>, UtilIsInteger<const> =
  Util.Blank, Core.Time, Util.IsInteger;
-- Diggers function and data aliases --------------------------------------- --
local Fade, GetCallbacks, InitSetup, InitTitle, IsButtonPressed, LoadResources,
  RegisterFBUCallback, RenderFade,  RenderShadow, SetCallbacks, VideoPlay,
  VideoStop, aIntroSubTitles, fontLittle, texSpr, SetKeys;
-- Resources --------------------------------------------------------------- --
local aAssets<const> = { { T = 1, F = "grass", P= { 16, 16, 0, 0, 0 } },
                         { T = 2, F = "title", P= { 0 } },
                         { T = 8, F = "intro", P= { } } };
-- Locals ------------------------------------------------------------------ --
local iKeyBankId;                      -- Keybank id
local iStageST = 0;                    -- Subtitle position
local iStageW, iStageH, iStageL;       -- Stage width, height and left
local iStageT, iStageR, iStageB;       -- Stage top, right and bottom
local texTiles, texTitle;              -- Texture handles
local vidVideo;                        -- Video handle
local iSubTitle, iFrameEnd;            -- Subtitle and frame end
local iFWidth, iFHeight, iPadding;     -- Font sizes and padding
local aSubTitle;                       -- Active subtitle to wait for
local iFadeX, iFadeY1, iFadeY2;        -- Fade values
local sSubTitle1, sSubTitle2;          -- First and second line of subtitles
local iSubTitle1, iSubTitle2;          -- First and second line subtitle id
local fcbRender;                       -- Rendering function
local fcbOnFadeIn;                     -- Callback to run on fade in
-- Playing procedure() ----------------------------------------------------- --
local function PlayingProc()
  -- If we're not in widescreen?
  if iStageL == 0 then
    -- Draw video normally
    vidVideo:SetTCLTRB(0, 0, 1, 1);
    vidVideo:SetVLTRB(0, 0, 320, 240);
    vidVideo:Blit();
  -- In widescreen?
  else
    -- Draw video effect for widescreen (left side)
    vidVideo:SetTCLTRB(0, 0, 0, 1);
    vidVideo:SetVLTRB(0, 0, iStageL, 240);
    vidVideo:Blit();
    -- Draw video effect for widescreen (right side)
    vidVideo:SetTCLTRB(1, 0, 1, 1);
    vidVideo:SetVLTRB(iStageR, 0, 320, 240);
    vidVideo:Blit();
    -- Draw the actual video in the centre (4:3)
    vidVideo:SetTCLTRB(0, 0, 1, 1);
    vidVideo:SetVLTRB(0, 0, 320, 240);
    vidVideo:Blit();
    -- Draw transparent tiles over the top of the widescreen border
    texTiles:SetCRGBA(1, 1, 1, 0.5);
    for iY = 0, 240, 16 do
      for iX = -16, iStageL-16, -16 do texTiles:BlitSLT(3, iX, iY) end;
      for iX = 320, iStageR, 16 do texTiles:BlitSLT(3, iX, iY) end;
    end
    -- Draw shadow
    texSpr:BlitSLTRB(1017, -16, iStageT,   0, iStageB);
    texSpr:BlitSLTRB(1018, 320, iStageT, 336, iStageB);
  end
  -- Get frame number
  local iFrame<const> = vidVideo:GetFrame();
  -- Have subtitle?
  if sSubTitle1 then
    -- Calculate iX position of rectangle
    local iFadeX1<const> = iStageST - iFadeX;
    local iFadeX2<const> = iStageST + iFadeX;
    -- Draw subtitle background and shadow
    RenderFade(0.75, iFadeX1, iFadeY1, iFadeX2, iFadeY2);
    RenderShadow(iFadeX1, iFadeY1, iFadeX2, iFadeY2);
    -- Draw subtitle
    fontLittle:PrintC(iStageST, iSubTitle1, sSubTitle1);
    -- If we have subtitle two
    if sSubTitle2 then
      fontLittle:PrintC(iStageST, iSubTitle2, sSubTitle2);
    end
    -- Hide subtitle if timed out
    if iFrame >= iFrameEnd then sSubTitle1, sSubTitle2 = nil, nil end;
  end
  -- Get next subtitle data (fontLittle:PrintC(iStageST, 170, iFrame);)
  if iFrame >= aSubTitle[1] then
    -- Grab new fade bounds
    iFadeX, iFadeY1, iFadeY2 = aSubTitle[6], aSubTitle[7], aSubTitle[8];
    -- Set subtitle position
    iSubTitle1, iSubTitle2 = aSubTitle[4], aSubTitle[5];
    -- Set new subtile, ending time and next id
    sSubTitle1, sSubTitle2, iFrameEnd =
      aSubTitle[3][1], aSubTitle[3][2], aSubTitle[2];
    -- Set next sub-title to monitor
    iSubTitle = iSubTitle + 1;
    aSubTitle = aIntroSubTitles[iSubTitle] or { math.maxinteger };
  end
end
-- Not playing procedure --------------------------------------------------- --
local function NotPlayingProc()
  -- Draw background
  texTitle:SetCRGBA(1, 1, 1, 1);
  texTitle:BlitLT(-96, 0);
  -- Don't draw anything if in 4:3 mode
  if iStageL >= 0 then return end;
  -- Draw sidebar scrolling logo's
  local iWidth<const> = -iStageL - 4;
  local nAspect<const> = 208 / 58;
  local nHeight<const> = iWidth * nAspect;
  local iLX = (CoreTime() * 100) % 240;
  local iLY = -iLX;
  local iX<const> = iStageL + 4;
  local iXpWidth<const> = iX + iWidth;
  local nHeightPiLX = nHeight + iLX;
  local nHeightPiLY = nHeight + iLY;
  local iXRight<const> = 320 + iWidth;
  texTitle:SetCA(0.25);
  texTitle:BlitSLTRB(1, iX, -240+iLX, iXpWidth, -240 + nHeightPiLX);
  texTitle:BlitSLTRB(1, iX,      iLX, iXpWidth,        nHeightPiLX);
  texTitle:BlitSLTRB(1, iX,  240+iLX, iXpWidth,  240 + nHeightPiLX);
  texTitle:BlitSLTRB(1, iXRight,       nHeightPiLY, 320,       iLY);
  texTitle:BlitSLTRB(1, iXRight, 240 + nHeightPiLY, 320, 240 + iLY);
  texTitle:BlitSLTRB(1, iXRight, 480 + nHeightPiLY, 320, 480 + iLY);
  iLX = -iLX;
  iLY = -iLY - 240;
  nHeightPiLX = nHeight + iLX;
  nHeightPiLY = nHeight + iLY;
  texTitle:BlitSLTRB(1, iX, -240 + iLX, iXpWidth, -240 + nHeightPiLX);
  texTitle:BlitSLTRB(1, iX,        iLX, iXpWidth,        nHeightPiLX);
  texTitle:BlitSLTRB(1, iX,  240 + iLX, iXpWidth,  240 + nHeightPiLX);
  texTitle:BlitSLTRB(1, iXRight,       nHeightPiLY, 320,       iLY);
  texTitle:BlitSLTRB(1, iXRight, 240 + nHeightPiLY, 320, 240 + iLY);
  texTitle:BlitSLTRB(1, iXRight, 480 + nHeightPiLY, 320, 480 + iLY);
end
-- Call render function ---------------------------------------------------- --
local function Render() fcbRender() end;
-- When video has faded out? ----------------------------------------------- --
local function OnFadeOut()
  -- Dereference loaded assets for garbage collector
  texTiles, texTitle, vidVideo = nil, nil, nil;
  -- Remove frame buffer update callback
  RegisterFBUCallback("intro");
  -- Destroy video and texture handles
  VideoStop();
  -- Load title screen
  InitTitle();
end
-- Finish function --------------------------------------------------------- --
local function Finish()
  -- Remove event function
  vidVideo:OnEvent(UtilBlank);
  -- Start fading out
  Fade(0, 1, 0.032, Render, OnFadeOut, true);
end
-- Play video and setup event ---------------------------------------------- --
local function PlayVideo()
  -- Get video events list and id's
  local aEvents<const> = Video.Events;
  local iStop<const>, iPause<const>, iPlay<const>, iFinish<const> =
    aEvents.STOP, aEvents.PAUSE, aEvents.PLAY, aEvents.FINISH;
  -- A video event occurred?
  local function OnEvent(iEvent)
    -- Playing?
    if iEvent == iPlay then
      -- If we were in our render loop? Transition to title screen
      if fcbRender == NotPlayingProc then fcbRender = PlayingProc end;
    -- Paused, stopped or finished?
    elseif iEvent == iPause or iEvent == iStop or iEvent == iFinish then
      -- We someone took our render callback (i.e. setup) then we need to
      -- to switch render proc
      local _, CBRender = GetCallbacks();
      if CBRender == Render then return Finish() end;
      -- If we were not in our render loop (i.e. setup?)
      fcbRender = NotPlayingProc;
    end
  end
  -- Set video event callback
  vidVideo:OnEvent(OnEvent);
  -- Play the video
  VideoPlay(vidVideo);
end
-- Register frame buffer update -------------------------------------------- --
local function OnFrameBufferUpdate(...)
  iStageW, iStageH, iStageL, iStageT, iStageR, iStageB = ...;
  iStageST = iStageL + (iStageW / 2);
end
-- Intro input procedure to just check if a button is pressed -------------- --
local function InputProc()
  if IsButtonPressed(0) then Finish() end;
end
-- When video has faded in? ------------------------------------------------ --
local function OnFadeIn()
  -- Enable keybank
  SetKeys(true, iKeyBankId);
  -- Set intro callbacks
  SetCallbacks(nil, Render, InputProc);
  -- Load setup now
  if fcbOnFadeIn then fcbOnFadeIn() end;
end
-- When intro resources have loaded? --------------------------------------- --
local function OnLoaded(aResources, bAndSetup)
  -- Subtitle position
  iStageST = 0;
  RegisterFBUCallback("intro", OnFrameBufferUpdate);
  -- Get the texture handle that was loaded
  texTiles, texTitle = aResources[1], aResources[2];
  texTitle:TileS(0, 0, 0, 512, 240);
  texTitle:TileA(227, 240, 285, 448);
  -- Load and play intro video
  vidVideo = aResources[3];
  vidVideo:SetFilter(false);
  -- Subtitles list
  iSubTitle, iFrameEnd = 1, math.maxinteger;
  -- For each credit
  for iI = 1, #aIntroSubTitles do
    -- Get subtitle item and we must have at least 3 leans
    local aSubTitle<const> = aIntroSubTitles[iI];
    -- Check parameter count
    if #aSubTitle < 3 then error("Not enough items on "..iI.."!") end;
    if #aSubTitle > 4 then error("Too many items on "..iI.."!") end;
    -- Verify frame start
    local iFrStart<const> = aSubTitle[1];
    if not UtilIsInteger(iFrStart) or iFrStart < 0 then
      error("Invalid start '"..tostring(iFrStart).."' on "..iI.."!") end;
    -- Verify frame end
    local iFrEnd<const> = aSubTitle[1];
    if not UtilIsInteger(iFrEnd) or iFrEnd < 0 then
      error("Invalid end '"..tostring(iFrEnd).."' on "..iI.."!") end;
    -- Get subtitles
    local aSubTitleTexts<const> = aSubTitle[3];
    local sSubTitle1<const> = aSubTitleTexts[1];
    local sSubTitle2<const> = aSubTitleTexts[2];
    -- Calculate length of first subtitle and if we have 2nd subtitle?
    local iWidth = #sSubTitle1 * iFWidth;
    if sSubTitle2 then
      -- Which subtitle is longest?
      iWidth = math.max(iWidth, #sSubTitle2 * iFWidth);
      -- Insert first and second subtitle Y position
      aSubTitle[1 + #aSubTitle] = 200; -- [4]
      aSubTitle[1 + #aSubTitle] = 210; -- [5]
    else
      -- Insert first and second (n/a) subtitle Y position
      aSubTitle[1 + #aSubTitle] = 206; -- [4]
      aSubTitle[1 + #aSubTitle] = 206; -- [5]
    end
    -- Insert half width of subtitle box plus padding
    aSubTitle[1 + #aSubTitle] = (iWidth / 2) + iPadding; -- [6]
    -- Insert top position of subtitle box minus padding
    aSubTitle[1 + #aSubTitle] = aSubTitle[4] - iPadding; -- [7]
    -- Insert bottom position of subtitle box plus font height plus padding
    aSubTitle[1 + #aSubTitle] = aSubTitle[5] + iFHeight + iPadding; -- [8]
  end
  -- Set first active subtitle to wait for
  aSubTitle = aIntroSubTitles[iSubTitle];
  -- Render proc
  fcbRender = NotPlayingProc;
  -- If not setup requested, we're done
  if bAndSetup then
    -- On faded in
    local function GoSetup()
      -- Play the intro
      PlayVideo()
      -- Show the setup screen
      InitSetup(1);
    end
    -- Show setup on faded in
    fcbOnFadeIn = GoSetup;
    -- Do the fade in
    Fade(1, 0, 0.025, Render, OnFadeIn);
  -- Setup not requested?
  else
    -- Just play video on fade in
    fcbOnFadeIn = PlayVideo;
    -- Prevent the graphic being shown for a couple of frames
    fcbRender = PlayingProc;
    -- No transition because the intro already has a transition
    OnFadeIn();
  end
end
-- Intro initialisation function ------------------------------------------- --
local function InitIntro(bAndSetup)
  -- Load resources asynchronously. Note: use 'ffmpeg2theora' and not 'ffmpeg'
  -- to encode proper quality videos.
  LoadResources("Intro", aAssets, OnLoaded, bAndSetup);
end
-- When script has loaded -------------------------------------------------- --
local function OnReady(GetAPI)
  -- Get imports
  Fade, GetCallbacks, InitSetup, InitTitle, IsButtonPressed, LoadResources,
    RegisterFBUCallback, RenderFade, RenderShadow, SetCallbacks, SetKeys,
    VideoPlay, VideoStop, aIntroSubTitles, fontLittle, texSpr =
      GetAPI("Fade", "GetCallbacks", "InitSetup", "InitTitle",
        "IsButtonPressed", "LoadResources", "RegisterFBUCallback",
        "RenderFade", "RenderShadow", "SetCallbacks", "SetKeys", "VideoPlay",
        "VideoStop", "aIntroSubTitles", "fontLittle", "texSpr");
  -- Get font size and padding
  iFWidth, iFHeight, iPadding =
    fontLittle:GetWidth(), fontLittle:GetHeight(), 5;
  -- Register keybinds
  iKeyBankId = GetAPI("RegisterKeys")("TITLE INTRO", {
    [Input.States.PRESS] =
      { { Input.KeyCodes.ESCAPE, Finish, "tic", "CANCEL" } }
  });
end
-- Exports and imports ----------------------------------------------------- --
return { A = { InitIntro = InitIntro }, F = OnReady };
-- End-of-File ============================================================= --
