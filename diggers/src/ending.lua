-- ENDING.LUA ============================================================== --
-- ooooooo.--ooooooo--.ooooo.-----.ooooo.--oooooooooo-oooooooo.----.ooooo..o --
-- 888'-`Y8b--`888'--d8P'-`Y8b---d8P'-`Y8b-`888'---`8-`888--`Y88.-d8P'---`Y8 --
-- 888----888--888--888---------888---------888--------888--.d88'-Y88bo.---- --
-- 888----888--888--888---------888---------888oo8-----888oo88P'---`"Y888o.- --
-- 888----888--888--888----oOOo-888----oOOo-888--"-----888`8b.--------`"Y88b --
-- 888---d88'--888--`88.---.88'-`88.---.88'-888-----o--888-`88b.--oo----.d8P --
-- 888bd8P'--oo888oo-`Y8bod8P'---`Y8bod8P'-o888ooood8-o888o-o888o-8""8888P'- --
-- ========================================================================= --
-- (c) Mhatxotic Design, 2025          (c) Millennium Interactive Ltd., 1994 --
-- ========================================================================= --
-- Core function aliases --------------------------------------------------- --
local error<const>, max<const> = error, math.max;
-- M-Engine function aliases ----------------------------------------------- --
local UtilIsInteger<const>, UtilIsTable<const> = Util.IsInteger, Util.IsTable;
-- Diggers function and data aliases --------------------------------------- --
local BlitLT, BlitSLT, Fade, InitCredits, LoadResources, PlayMusic, PrintC,
  SetCallbacks, aEndingData, fontLittle;
-- Locals ------------------------------------------------------------------ --
local aEndingItem;                     -- Race ending data
local iActionTimer;                    -- Action timer for delays
local iAlphaValue;                     -- Stranger alpha value
local iTileEnding;                     -- Race specific ending tile
local sText1, sText2;                  -- Race specific ending dialogue
local texEnding;                       -- Race specific ending texture handle
local texLobby;                        -- Lobby texture handle
local texStr;                          -- Stranger texture handle
-- Assets required --------------------------------------------------------- --
local aAssets1, aAssets2, aAssets3;
-- When ending screen 3 has faded out -------------------------------------- --
local function OnEnding3FadedOut()
  -- Dereference stranger asset to garbage collector
  texStr = nil;
  -- Initialise final credits
  InitCredits();
end
-- Credits render callback ------------------------------------------------- --
local function ProcRenderEnding3()
  -- Draw background
  BlitLT(texStr, -54, 0);
  -- Draw stranger
  texStr:SetCA(iAlphaValue);
  BlitSLT(texStr, 1, 0, 68);
  texStr:SetCA(1);
  -- Draw text
  fontLittle:SetCRGBA(1,1,1,1);
  PrintC(fontLittle, 160, 200,
    "...WHILST THE MYSTERIOUS FIGURE OF THE");
  PrintC(fontLittle, 160, 220,
    "MASTER MINER RETURNS FROM WHENCE HE CAME");
end
-- Ending 3 procedure ------------------------------------------------------ --
local function ProcLogicEnding3()
  -- Ignore if stranger isn't fully gone
  iAlphaValue = max(0, iAlphaValue - 0.002);
  if iAlphaValue > 0 then return end;
  -- Fade out ending screen 3
  Fade(0, 1, 0.01, ProcRenderEnding3, OnEnding3FadedOut, true);
end
-- When ending screen 3 has faded in --------------------------------------- --
local function OnEnding3FadedIn()
  -- Wait on ending screen 3
  SetCallbacks(ProcLogicEnding3, ProcRenderEnding3);
end
-- When ending 3 resources have loaded ------------------------------------- --
local function OnEnding3AssetsLoaded(aResources)
  -- Load stranger texture and tiles
  texStr = aResources[1];
  -- Fade in ending screen 3
  Fade(1, 0, 0.025, ProcRenderEnding3, OnEnding3FadedIn);
end
-- When ending screen 2 has faded out -------------------------------------- --
local function OnEnding2FadedOut()
  -- Dereference ending asset to garbage collector
  texEnding = nil;
  -- Reset action timer
  iActionTimer = 0;
  -- Reset alpha value
  iAlphaValue = 1;
  -- Load ending 3 resources
  LoadResources("Ending3", aAssets3, OnEnding3AssetsLoaded);
end
-- Render ending part 2 ---------------------------------------------------- --
local function ProcRenderEnding2()
  -- Draw background
  BlitSLT(texEnding, iTileEnding, -54, 0);
  -- Draw text
  fontLittle:SetCRGBA(1, 1, 1, 1);
  PrintC(fontLittle, 160, 200, sText1);
  PrintC(fontLittle, 160, 220, sText2);
end
-- Ending two procedure ---------------------------------------------------- --
local function ProcLogicEnding2()
  -- Increment action timer and done if action timer not reached yet
  iActionTimer = iActionTimer + 1;
  if iActionTimer < 240 then return end
  -- Fade out on ending screen 2
  Fade(0, 1, 0.025, ProcRenderEnding2, OnEnding2FadedOut);
end
-- When render part 2 has faded in ----------------------------------------- --
local function OnEnding2FadedIn()
  -- Wait on ending screen 2
  SetCallbacks(ProcLogicEnding2, ProcRenderEnding2);
end
-- Ending screen 2 resources loaded? --------------------------------------- --
local function OnEnding2AssetsLoaded(aResources)
  -- Load texture and tiles
  texEnding = aResources[1];
  -- Set tile ending
  iTileEnding = aEndingItem[2];
  -- Set custom race specific texts
  sText1, sText2 = aEndingItem[3], aEndingItem[4];
  -- Fade in ending screen 2
  Fade(1, 0, 0.025, ProcRenderEnding2, OnEnding2FadedIn);
end
-- When ending 1 has faded out --------------------------------------------- --
local function OnEnding1FadedOut()
  -- Dereference lobby asset to garbage collector
  texLobby = nil;
  -- Reset action timer
  iActionTimer = 0;
  -- Set race scene filename
  aAssets2[1].F = "ending"..aEndingItem[1];
  -- Load ending screen 2 resource
  LoadResources("Ending2", aAssets2, OnEnding2AssetsLoaded);
end
-- Render callback --------------------------------------------------------- --
local function ProcRenderEnding1()
  -- Draw background
  BlitLT(texLobby, -54, 0);
  -- Draw text
  fontLittle:SetCRGBA(1,1,1,1);
  PrintC(fontLittle, 160, 200, "MINING OPERATIONS COMPLETED!");
end
-- First ending scene waiting ---------------------------------------------- --
local function ProcLogicEnding1()
  -- Increment action timer and done if action timer not reached yet
  iActionTimer = iActionTimer + 1;
  if iActionTimer < 120 then return end;
  -- Fade out ending screen 1
  Fade(0, 1, 0.025, ProcRenderEnding1, OnEnding1FadedOut);
end
-- First ending screen procedure ------------------------------------------- --
local function OnEnding1FadedIn()
  -- Reset action timer
  iActionTimer = 0;
  -- Wait on ending screen 1
  SetCallbacks(ProcLogicEnding1, ProcRenderEnding1);
end
-- When assets have loaded? ------------------------------------------------ --
local function OnEnding1AssetsLoaded(aResources)
  -- Play win music
  PlayMusic(aResources[2], nil, nil, nil, 371767);
  -- Load lobby texture
  texLobby = aResources[1];
  -- Fade in to ending screen 1 (Mining operations complete!)
  Fade(1, 0, 0.025, ProcRenderEnding1, OnEnding1FadedIn);
end
-- Init ending screen functions -------------------------------------------- --
local function InitEnding(iRaceId)
  -- Check race id and check ending data
  if not UtilIsInteger(iRaceId) then error("No race id specified!") end;
  aEndingItem = aEndingData[iRaceId];
  if not UtilIsTable(aEndingData) then error("Invalid race id!") end;
  -- Load bank texture
  LoadResources("Ending1", aAssets1, OnEnding1AssetsLoaded);
end
-- Scripts have been loaded ------------------------------------------------ --
local function OnScriptLoaded(GetAPI)
  -- Grab imports
  BlitLT, BlitSLT, Fade, InitCredits, LoadResources, PlayMusic, PrintC,
    SetCallbacks, aEndingData, fontLittle =
      GetAPI("BlitLT", "BlitSLT", "Fade", "InitCredits", "LoadResources",
        "PlayMusic", "PrintC", "SetCallbacks", "aEndingData", "fontLittle");
  -- Setup required assets
  local aAssetsData<const> = GetAPI("aAssetsData");
  aAssets1 = { aAssetsData.lobbyc, aAssetsData.scenem };
  aAssets2 = { aAssetsData.ending };
  aAssets3 = { aAssetsData.ending3 };
end
-- Exports and imports ----------------------------------------------------- --
return { A = { InitEnding = InitEnding }, F = OnScriptLoaded };
-- End-of-File ============================================================= --
