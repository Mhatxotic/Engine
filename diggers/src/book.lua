-- BOOK.LUA ================================================================ --
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
local UtilBlank<const>, UtilClampInt<const> = Util.Blank, Util.ClampInt;
-- Diggers function and data aliases --------------------------------------- --
local BlitLT, Fade, GameProc, InitCon, InitContinueGame,
  LoadResources, PlayMusic, PlayStaticSound, RenderAll, RenderShadow,
  RenderTip, RenderTipShadow, SetTip, SetCallbacks, SetHotSpot, SetKeys,
  fontSpeech;
-- Consts ------------------------------------------------------------------ --
-- Pages each sized 510x200 stored inside texture sized 1024^2. OpenGL 3.2
-- guarantees us that 1024^2 textures are supported by every renderer.
local iPagesPerTexture<const> = 20;
local iTotalPages<const> = 88;                -- Maximum number of pages
local iTotalPagesM1<const> = iTotalPages - 1; -- Maximum " minus one
-- Locals ------------------------------------------------------------------ --
local aAssets,                         -- Assets required
      aBookData,                       -- Book data
      aLobbyMusic,                     -- Lobby music asset
      aZmtcTexture,             -- Lobby closed texture asset
      aPageAsset;                      -- Book page asset
local fcbFinish,                       -- Callback to call to exit
      fcbOnPageAssetsPost,             -- When page assets have loaded
      fcbProcLogic,                    -- Main page logic callback
      fcbProcRenderBack,               -- Rendering background callback
      fcbProcRenderCover;              -- Render callback for cover page
local iPage = 0;                       -- Book current page (persisted)
local iHotSpotCoverId, iHotSpotPageId, -- Hot spot id for cover and pages part
      iHotSpotStartId,                 -- Hot spot to set when cover page load
      iKeyBankCoverId, iKeyBankPageId, -- Key bank id for cover and pages part
      iKeyBankStartId,                 -- Key bank to set when cover page load
      iSClick, iSSelect;               -- Sound effects used
local strExitTip, strPage, strText,    -- Tip strings
      texCover, texLobby,               -- Book, lobby and page texture handles
      texActive;
-- Book render callback ---------------------------------------------------- --
local function ProcRenderPage()
  -- Render book background, spine and backdrop
  fcbProcRenderBack();
  BlitLT(texActive, 8, 8);
  if strText then PrintW(fontSpeech, 76, 24, 298, strText) end;
end
-- Page loader function ---------------------------------------------------- --
local function LoadPage(fcbOnComplete)
  -- Set text
  strText = aBookData[iPage][1];
  -- Update page number
  strPage = "PAGE "..iPage.."/"..#aBookData;
  SetTip(strPage);
end
-- Switch page with sound -------------------------------------------------- --
local function GoAdjustPage(iNewPage)
  -- Return if same page else set new page
  iNewPage = UtilClampInt(iNewPage, 1, #aBookData);
  if iNewPage == iPage then return end;
  iPage = iNewPage;
  -- Play the sound
  PlayStaticSound(iSClick);
  -- Load the specified new page
  LoadPage();
end
-- Book button action callbacks -------------------------------------------- --
local function GoExit() fcbFinish() end
local function GoIndex() GoAdjustPage(1) end;
local function GoLast() GoAdjustPage(iPage - 1) end;
local function GoNext() GoAdjustPage(iPage + 1) end;
-- Hover functions (dynamic) ----------------------------------------------- --
local function HoverExit() SetTip(strExitTip) end;
local function HoverIdle() SetTip(strPage) end;
-- Scroll wheel callback --------------------------------------------------- --
local function OnScroll(nX, nY)
  if nY < 0 then GoLast() elseif nY > 0 then GoNext() end;
end
-- On render callback ------------------------------------------------------ --
local function ProcRenderCover()
  -- Render background
  fcbProcRenderBack();
  -- Draw backdrop
  BlitLT(texCover, 8, 8);
end
-- Change cover to inside the book ----------------------------------------- --
local function GoOpen()
  -- Set renderer to book page
  fcbProcRenderCover = ProcRenderPage;
  -- Set active page texture and clear cover texture
  texActive, texCover = texPage, nil;
  -- Load first page
  iPage = 1;
  LoadPage();
  -- Play click sound
  PlayStaticSound(iSSelect);
  -- Set page keys and hot spots
  SetKeys(true, iKeyBankPageId);
  SetHotSpot(iHotSpotPageId);
  -- Set main page game proc
  SetCallbacks(fcbProcLogic, ProcRenderPage);
end
-- Set render background function ------------------------------------------ --
local function ProcRenderBackInGame()
  -- Render game interface
  RenderAll();
  -- Draw tip
  RenderTip();
  -- Render shadow
  RenderShadow(8, 8, 312, 208);
end
-- Set render background function ------------------------------------------ --
local function ProcRenderBackLobby()
  -- Render static background
  BlitLT(texLobby, -54, 0);
  -- Draw tip and return
  RenderTipShadow();
end
-- Cover loaded in-game supplimental callback ------------------------------ --
local function OnPageAssetsPostInGame()
  -- Set intro page keys and hot spots
  SetKeys(true, iKeyBankStartId);
  SetHotSpot(iHotSpotStartId);
  -- No transition from in-game
  SetCallbacks(GameProc, fcbProcRenderCover);
end
-- Cover data loaded? ------------------------------------------------------ --
local function OnPageAssetsPostLobbyFadedIn()
  -- Set intro page and hot spots keys
  SetKeys(true, iKeyBankStartId);
  SetHotSpot(iHotSpotStartId);
  -- Return control to main loop
  SetCallbacks(nil, fcbProcRenderCover);
end
-- Cover loaded in-lobby supplimental callback ----------------------------- --
local function OnPageAssetsPostLobby()
  -- From controller screen? Fade in
  Fade(1, 0, 0.04, fcbProcRenderCover, OnPageAssetsPostLobbyFadedIn);
end
-- Finish in-game supplimental callback ------------------------------------ --
local function ExitInGame()
  -- Play sound
  PlayStaticSound(iSSelect);
  -- Dereference assets for garbage collector
  texPage, texCover, texActive = nil, nil, nil;
  -- Start the loading waiting procedure
  SetCallbacks(GameProc, RenderAll);
  -- Continue game
  InitContinueGame();
end
-- On faded event ---------------------------------------------------------- --
local function OnExitLobbyFadedOut()
  -- Dereference assets for garbage collector
  texPage, texCover, texLobby, texActive = nil, nil, nil, nil;
  -- Init controller screen
  InitCon();
end
-- Finish in-lobby supplimental callback ----------------------------------- --
local function ExitLobby()
  -- Play sound
  PlayStaticSound(iSSelect);
  -- Fade out to controller
  Fade(0, 1, 0.04, fcbProcRenderCover, OnExitLobbyFadedOut);
end
-- Lobby cover resources laoded -------------------------------------------- --
local function OnAssetsLoadedLobby(texHandle)
  -- Get lobby texture and setup background tile. This will be nil if loading
  -- from in-game so it doesn't matter. Already handled.
  texLobby = texHandle;
end
-- When the resources have loaded ------------------------------------------ --
local function OnAssetsLoaded(aResources, fcbProcCustomHandle)
  -- Set texture and setup tiles
  texCover, texPage = aResources[1], aResources[2];
  -- Call supplimental load routine depending if we're in-game or not
  fcbProcCustomHandle(aResources[3]);
  -- If we've shown the cover page?
  if strText then
    -- Set page keybank and callbacks
    iKeyBankStartId, iHotSpotStartId = iKeyBankPageId, iHotSpotPageId;
    fcbProcRenderCover = ProcRenderPage;
    texActive = texPage;
  -- Not shown the cover page yet? Set render callback
  else
    -- Set cover page keybank and callbanks
    iKeyBankStartId, iHotSpotStartId = iKeyBankCoverId, iHotSpotCoverId;
    fcbProcRenderCover = ProcRenderCover;
    texActive = texCover;
  end
  -- Cover has loaded
  fcbOnPageAssetsPost();
end
-- Init book screen function ----------------------------------------------- --
local function InitBook(bFromInGame)
  -- Post asset loaded function
  local fcbProcCustomHandle;
  -- Loading from in-game
  if bFromInGame then
    -- Set text for exit tip
    strExitTip = "BACK TO GAME";
    -- Nothing to load in slot two
    aAssets[3] = nil;
    -- Set specific behaviour from in-game
    fcbProcCustomHandle = UtilBlank;
    fcbOnPageAssetsPost = OnPageAssetsPostInGame;
    fcbProcRenderBack = ProcRenderBackInGame;
    fcbProcLogic = GameProc;
    fcbFinish = ExitInGame;
  -- Loading from lobby?
  else
    -- Set text for exit tip
    strExitTip = "CONTROLLER";
    -- Load backdrop from closed lobby
    aAssets[3] = aZmtcTexture;
    -- Set specific behaviour from the lobby
    fcbProcCustomHandle = OnAssetsLoadedLobby;
    fcbOnPageAssetsPost = OnPageAssetsPostLobby;
    fcbProcRenderBack = ProcRenderBackLobby;
    fcbProcLogic = UtilBlank;
    fcbFinish = ExitLobby;
  end
  -- Load the resources
  LoadResources("Book", aAssets, OnAssetsLoaded, fcbProcCustomHandle);
end
-- Scripts have been loaded ------------------------------------------------ --
local function OnScriptLoaded(GetAPI)
  -- Functions and variables used in this scope only
  local RegisterHotSpot, RegisterKeys, aAssetsData, aCursorIdData, aSfxData;
  -- Grab imports
  BlitLT, Fade, GameProc, InitCon, InitContinueGame, LoadResources,
    PlayMusic, PlayStaticSound, PrintW, RegisterHotSpot, RegisterKeys,
    RenderAll, RenderShadow, RenderTip, RenderTipShadow, SetCallbacks,
    SetHotSpot, SetKeys, SetTip, aAssetsData, aBookData, aCursorIdData,
    aSfxData, fontSpeech =
      GetAPI("BlitLT", "Fade", "GameProc", "InitCon", "InitContinueGame",
        "LoadResources", "PlayMusic", "PlayStaticSound", "PrintW",
        "RegisterHotSpot", "RegisterKeys", "RenderAll", "RenderShadow",
        "RenderTip", "RenderTipShadow", "SetCallbacks", "SetHotSpot",
        "SetKeys", "SetTip", "aAssetsData", "aBookData", "aCursorIdData",
        "aSfxData", "fontSpeech");
  -- Set book data language
  aBookData = aBookData.en;
  -- Prepare assets
  aZmtcTexture = aAssetsData.zmtc;
  aAssets = { aAssetsData.bookcover, aAssetsData.bookpage, false };
  -- Register key binds
  local aKeys<const> = Input.KeyCodes;
  local iPress<const> = Input.States.PRESS;
  local aClose<const> = { aKeys.ESCAPE, GoExit, "zmtctbcl", "CLOSE" };
  local sName<const> = "ZMTC BOOK";
  iKeyBankPageId = RegisterKeys(sName, { [iPress] = { aClose,
    { aKeys.LEFT,      GoLast,  "zmtctbpp", "PREVIOUS PAGE" },
    { aKeys.RIGHT,     GoNext,  "zmtctbnp", "NEXT PAGE"     },
    { aKeys.BACKSPACE, GoIndex, "zmtctbc",  "CONTENTS"      },
  } });
  iKeyBankCoverId = RegisterKeys(sName, { [iPress] = { aClose,
    { aKeys.ENTER, GoOpen, "zmtcbob", "OPEN BOOK" } } });
  -- Set sound effect ids
  iSClick, iSSelect = aSfxData.CLICK, aSfxData.SELECT;
  -- Set cursor ids
  local iCOK<const>, iCSelect<const>, iCExit<const> =
    aCursorIdData.OK, aCursorIdData.SELECT, aCursorIdData.EXIT;
  -- Set points of interest data for cover
  iHotSpotCoverId = RegisterHotSpot({
    {   8,   8, 304, 200, 0, iCOK,   "OPEN BOOK", false, GoOpen },
    {   0,   0,   0, 240, 3, iCExit, HoverExit,   false, GoExit }
  });
  -- Set points of interest data
  iHotSpotPageId = RegisterHotSpot({
    {  19,  68,  20,  22, 0, iCSelect, "INDEX PAGE", OnScroll, GoIndex },
    {  19,  96,  20,  22, 0, iCSelect, "NEXT PAGE",  OnScroll, GoNext  },
    {  19, 122,  20,  22, 0, iCSelect, "LAST PAGE",  OnScroll, GoLast  },
    {   8,   8, 304, 200, 0, 0,        HoverIdle,    OnScroll, false   },
    {   0,   0,   0, 240, 3, iCExit,   HoverExit,    OnScroll, GoExit  }
  });
end
-- Exports and imports ----------------------------------------------------- --
return { A = { InitBook = InitBook }, F = OnScriptLoaded };
-- End-of-File ============================================================= --
