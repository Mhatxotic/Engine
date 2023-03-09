-- SCENE.LUA =============================================================== --
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
-- M-Engine function aliases ----------------------------------------------- --
-- Diggers function and data aliases --------------------------------------- --
local Fade, LoadLevel, LoadResources, PlayMusic, SetCallbacks, aGlobalData,
  aLevelTypesData, aLevelsData, fontLarge;
-- Locals ------------------------------------------------------------------ --
local aAssets,                         -- Assets required
      aLevelInfo,                      -- Selected level information
      iLevelId,                        -- Level id to load
      iTileId,                         -- First tile number
      iWaitCounter,                    -- Ticks counted for next screen
      sTextToWin,                      -- Text to win label
      texScene;                        -- Scene texture
-- Render the scene callback since we're using it multiple times ----------- --
local function ProcRenderScene()
  -- Draw appropriate background
  texScene:BlitSLT(iTileId, 0, 0);
  -- Return if not ready
  if iTileId ~= 1 then return end;
  -- Draw the text to win
  fontLarge:SetCRGBA(1, 1, 1, 1);
  fontLarge:PrintC(160, 192, sTextToWin);
end
-- On required fade out? --------------------------------------------------- --
local function OnFadeOutToGame()
  -- Release assets to garbage collector
  texScene = nil;
  -- Load the requested level
  LoadLevel(iLevelId, "game", -1);
end
-- Required Zogs wait procedure -------------------------------------------- --
local function ProcLogicRequired()
  -- Increment timer and don't do anything until 4 seconds
  iWaitCounter = iWaitCounter + 1;
  if iWaitCounter < 300 then return end;
  -- Fade out and then load the level
  Fade(0, 1, 0.04, ProcRenderScene, OnFadeOutToGame, true);
end
-- Required Zogs fade in proc ---------------------------------------------- --
local function OnFadeInToRequired()
  -- Set required wait callbacks
  SetCallbacks(ProcLogicRequired, ProcRenderScene);
end
-- Scene fade out proc ----------------------------------------------------- --
local function OnSceneFadedOut()
  -- Set text to win label
  sTextToWin = "RAISE "..(aLevelInfo.w.r + aGlobalData.gCapitalCarried)..
    " ZOGS TO WIN";
  -- Set next tile number
  iTileId = 1;
  -- Fade in required scene
  Fade(1, 0, 0.04, ProcRenderScene, OnFadeInToRequired);
end
-- Waiting on scene graphic ------------------------------------------------ --
local function ProcLogicScene()
  -- Increment timer and don't do anything until 2 seconds
  iWaitCounter = iWaitCounter + 1;
  if iWaitCounter < 120 then return end;
  -- Set the gold scene
  Fade(0, 1, 0.04, ProcRenderScene, OnSceneFadedOut);
end
-- Fade in proc ------------------------------------------------------------ --
local function OnSceneFadedIn()
  -- Init wait timer
  iWaitCounter = 0;
  -- Set scene wait callbacks
  SetCallbacks(ProcLogicScene, ProcRenderScene);
end
-- On loaded function ------------------------------------------------------ --
local function OnAssetsLoaded(aResources)
  -- Play scene music
  PlayMusic(aResources[2]);
  -- Set scene texture
  texScene = aResources[1];
  -- Set first tile number
  iTileId = 0;
  -- Fade in
  Fade(1, 0, 0.04, ProcRenderScene, OnSceneFadedIn);
end
-- Init scene function ----------------------------------------------------- --
local function InitScene(iZoneId)
  -- Set level number and get data for it.
  iLevelId = 1 + ((iZoneId - 1) % #aLevelsData);
  aLevelInfo = aLevelsData[iLevelId];
  -- Get level terrain information and set scene setter texture to load
  local aTerrain<const> = aLevelInfo.t;
  aAssets[1].F = aTerrain.f.."ss";
  -- Load resources
  LoadResources("Scene "..aLevelInfo.n.."/"..aTerrain.n,
    aAssets, OnAssetsLoaded);
end
-- Scripts have been loaded ------------------------------------------------ --
local function OnScriptLoaded(GetAPI)
  -- Grab imports
  Fade, LoadLevel, LoadResources, PlayMusic, SetCallbacks, aGlobalData,
    aLevelTypesData, aLevelsData, fontLarge =
      GetAPI("Fade", "LoadLevel", "LoadResources", "PlayMusic", "SetCallbacks",
        "aGlobalData", "aLevelTypesData", "aLevelsData", "fontLarge");
  -- Setup assets
  local aAssetsData<const> = GetAPI("aAssetsData");
  aAssets = { aAssetsData.scene, aAssetsData.scenem };
end
-- Exports and imports ----------------------------------------------------- --
return { A = { InitScene = InitScene }, F = OnScriptLoaded };
-- End-of-File ============================================================= --
