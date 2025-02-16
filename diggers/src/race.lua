-- CNTRL.LUA =============================================================== --
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
local BlitSLTRB, BlitSLT, BlitLT, Fade, InitCon, LoadResources,
  PlayStaticSound, RenderShadow, RenderTipShadow, SetCallbacks, SetHotSpot,
  SetKeys, TypeIdToId, aGlobalData, aRaceStatData, texSpr;
-- Locals ------------------------------------------------------------------ --
local aAssets,                         -- Assets Required
      aRaceDataSelected,               -- Race id data
      iHotSpotId,                      -- Hot spot id
      iKeyBankId,                      -- Key bank id
      iRaceId,                         -- Chosen race id
      iRaceIdSelected,                 -- Currently displayed race id
      iSClick, iSSelect,               -- Sound effect ids
      texLobby,                        -- Lobby texture
      texRace;                         -- Race texture
-- Tile data (See data.lua/aAssetsData.race.P) ----------------------------- --
local iTileBG<const>      =  9;        -- Race screen background
local iTileName<const>    =  5;        -- First race texture
local iTileSpecial<const> = 10;        -- Special items tile
-- Set clamped race id and race data --------------------------------------- --
local function SetRaceId(iId)
  iRaceId = iId % #aRaceStatData;
  aRaceDataSelected = aRaceStatData[iRaceId + 1];
end
-- Render race ------------------------------------------------------------- --
local function ProcRenderRace()
  -- Draw backdrop, race screen and it's shadow
  BlitLT(texLobby, -54, 0);
  BlitSLT(texRace, iTileBG, 8, 8);
  RenderShadow(8, 8, 312, 208);
  -- Draw race and title text
  BlitSLT(texRace, iRaceId, 172, 54);
  BlitSLT(texRace, iTileName+iRaceId, 80, 24);
  -- Draw stats
  texSpr:SetCRGBA(1, 0, 0, 0.5);
  BlitSLTRB(texSpr, 1022, 115,  62, 115+aRaceDataSelected[2],  65);
  BlitSLTRB(texSpr, 1022, 115,  82, 115+aRaceDataSelected[3],  85);
  BlitSLTRB(texSpr, 1022, 115, 102, 115+aRaceDataSelected[4], 105);
  BlitSLTRB(texSpr, 1022, 115, 122, 115+aRaceDataSelected[5], 125);
  BlitSLTRB(texSpr, 1022, 115, 142, 115+aRaceDataSelected[6], 145);
  BlitSLTRB(texSpr, 1022, 115, 162, 115+aRaceDataSelected[7], 165);
  texSpr:SetCRGBA(1, 1, 1, 1);
  -- Draw special
  if aRaceDataSelected[8] >= 0 then
    BlitSLT(texRace, iTileSpecial+aRaceDataSelected[8], 110, 175);
  end
  -- Draw selected symbol
  if iRaceId == iRaceIdSelected then
    BlitSLT(texRace, 4, 132, 80, 192, 208) end;
  -- Draw tip and shadow
  RenderTipShadow();
end
-- Proc race function while fading ----------------------------------------- --
local function ProcRaceInitial()
  -- Enable keybank and hotspots
  SetKeys(true, iKeyBankId);
  SetHotSpot(iHotSpotId);
  -- Set callbacks
  SetCallbacks(nil, ProcRenderRace);
end
-- Race selection function ------------------------------------------------- --
local function AdjustRace(iAmount)
  PlayStaticSound(iSClick);
  SetRaceId(iRaceId + iAmount);
end
-- Selext previous and next race functions --------------------------------- --
local function GoPrevious() AdjustRace(-1) end;
local function GoNext() AdjustRace(1) end;
-- When returning to controler --------------------------------------------- --
local function GoCntrl()
  -- Play select soud
  PlayStaticSound(iSSelect)
  -- When faded out?
  local function OnFadeOut()
    -- Dereference assets for garbage collector
    texRace, texLobby = nil, nil;
    -- Load controller screen
    InitCon();
  end
  -- Fade out to controller screen
  Fade(0, 1, 0.04, ProcRenderRace, OnFadeOut);
end
-- When accepting the race selection --------------------------------------- --
local function GoAccept()
  -- Apply new setting
  aGlobalData.gSelectedRace, iRaceIdSelected = aRaceDataSelected[1], iRaceId;
  -- Fade out to lobby
  GoCntrl();
end
-- When scroll wheel or trackpad is moved ---------------------------------- --
local function OnScroll(nX, nY)
  if nY > 0 then GoPrevious() elseif nY < 0 then GoNext() end;
end
-- Data loaded function ---------------------------------------------------- --
local function OnAssetsLoaded(aResources)
  -- Setup lobby texture
  texLobby = aResources[1];
  -- Get texture resource and trim texture coordinates list to 5
  texRace = aResources[2];
  -- Set currently selected race
  iRaceIdSelected = aGlobalData.gSelectedRace;
  -- Set race already selected
  SetRaceId(iRaceIdSelected or 0);
  -- Fade in
  Fade(1, 0, 0.04, ProcRenderRace, ProcRaceInitial);
end
-- Init race screen function ----------------------------------------------- --
local function InitRace() LoadResources("Race", aAssets, OnAssetsLoaded) end;
-- Scripts have been loaded ------------------------------------------------ --
local function OnScriptLoaded(GetAPI)
  -- Functions and variables used in this scope only
  local RegisterHotSpot, RegisterKeys, aAssetsData, aCursorIdData, aSfxData;
  -- Grab imports
  BlitSLTRB, BlitSLT, BlitLT, Fade, InitCon, LoadResources, PlayStaticSound,
    RegisterHotSpot, RegisterKeys, RenderShadow, RenderTipShadow, SetCallbacks,
    SetHotSpot, SetKeys, aAssetsData, aCursorIdData, aGlobalData,
    aRaceStatData, aSfxData, texSpr =
      GetAPI("BlitSLTRB", "BlitSLT", "BlitLT", "Fade", "InitCon",
        "LoadResources", "PlayStaticSound", "RegisterHotSpot", "RegisterKeys",
        "RenderShadow", "RenderTipShadow", "SetCallbacks", "SetHotSpot",
        "SetKeys", "aAssetsData", "aCursorIdData", "aGlobalData",
        "aRaceStatData", "aSfxData", "texSpr");
  -- Set assets data
  aAssets = { aAssetsData.lobbyc, aAssetsData.race };
  -- Set sound effect ids
  iSClick, iSSelect = aSfxData.CLICK, aSfxData.SELECT;
  -- Register keybinds
  local aKeys<const> = Input.KeyCodes;
  iKeyBankId = RegisterKeys("ZMTC RACE SELECT", {
    [Input.States.PRESS] = {
      { aKeys.ESCAPE, GoCntrl,    "zmtcrsc", "CANCEL"   },
      { aKeys.ENTER,  GoAccept,   "zmtcrsa", "ACCEPT"   },
      { aKeys.LEFT,   GoPrevious, "zmtcrsp", "PREVIOUS" },
      { aKeys.RIGHT,  GoNext,     "zmtcrsn", "NEXT"     },
    }
  });
  -- Register hotspots
  iHotSpotId = RegisterHotSpot({
    { 172,  54,  54, 128, 0, aCursorIdData.OK,
      "ACCEPT",      OnScroll, GoAccept },
    { 248, 192,  16,  16, 0, aCursorIdData.SELECT,
      "NEXT RACE",   OnScroll, GoNext   },
    {   8,   8, 304, 200, 0, 0,
      "SELECT RACE", OnScroll, false    },
    {   0,   0,   0, 240, 3, aCursorIdData.EXIT,
      "CANCEL",      OnScroll, GoCntrl  }
  });
end
-- Exports and imports ----------------------------------------------------- --
return { A = { InitRace = InitRace }, F = OnScriptLoaded };
-- End-of-File ============================================================= --
