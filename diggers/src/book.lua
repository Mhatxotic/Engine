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
local BlitSLT, BlitLT, Fade, GameProc, InitCon, InitContinueGame,
  LoadResources, PlayMusic, PlayStaticSound, RenderInterface, RenderShadow,
  RenderTip, RenderTipShadow, SetTip, SetCallbacks, SetHotSpot, SetKeys;
-- Consts ------------------------------------------------------------------ --
-- Pages each sized 510x200 stored inside texture sized 1024^2. OpenGL 3.2
-- guarantees us that 1024^2 textures are supported by every renderer.
local iPagesPerTexture<const> = 20;
local iTotalPages<const> = 88;                -- Maximum number of pages
local iTotalPagesM1<const> = iTotalPages - 1; -- Maximum " minus one
-- Locals ------------------------------------------------------------------ --
local aAssets,                         -- Assets required
      aLobbyMusic,                     -- Lobby music asset
      aLobbyClosedTexture,             -- Lobby closed texture asset
      aPageAsset;                      -- Book page asset
local bCoverPage = true;               -- Cover page was displayed?
local fcbFinish,                       -- Callback to call to exit
      fcbOnPageAssetsPost,             -- When page assets have loaded
      fcbProcLogic,                    -- Main page logic callback
      fcbProcRenderBack,               -- Rendering background callback
      fcbProcRenderCover;              -- Render callback for cover page
local iPage = 0;                       -- Book current page (persisted)
local iFilePage, iTilePage,            -- File id and tile id
      iHotSpotCoverId, iHotSpotPageId, -- Hot spot id for cover and pages part
      iHotSpotStartId,                 -- Hot spot to set when cover page load
      iKeyBankCoverId, iKeyBankPageId, -- Key bank id for cover and pages part
      iKeyBankStartId,                 -- Key bank to set when cover page load
      iLoadPage,                       -- Current physical page
      iSClick, iSSelect;               -- Sound effects used
local strExitTip, strPage,             -- Tip strings
      texBook, texLobby, texPage;      -- Book, lobby and page texture handles
-- Book render callback ---------------------------------------------------- --
local function ProcRenderPage()
  -- Render book background, spine and backdrop
  fcbProcRenderBack();
  BlitSLT(texBook, 1, 8, 8);
  BlitSLT(texPage, iTilePage, 57, 8);
end
-- Page texture texture handles loaded -------------------------------------- --
local function OnPageAssetsLoaded(aResource, fcbOnComplete)
  -- Set displayed page number and assign the tip to it
  strPage = "PAGE "..(1 + iPage).."/"..iTotalPages;
  -- Set new page
  iFilePage = iLoadPage;
  -- Set new page texture
  texPage = aResource[1];
  -- Set actual page on texture
  iTilePage = iPage % iPagesPerTexture;
  -- Call callback function if set on load completion
  if fcbOnComplete then return fcbOnComplete() end;
  -- Enable keybank
  SetKeys(true, iKeyBankPageId);
  SetHotSpot(iHotSpotPageId);
  -- Run page loaded function
  SetCallbacks(fcbProcLogic, ProcRenderPage);
end
-- Page loader function ---------------------------------------------------- --
local function LoadPage(fcbOnComplete)
  -- Calculate page plus one
  local iPageP1<const> = 1 + iPage;
  -- Which texture page do we need and if we need to load it?
  iLoadPage = iPage // iPagesPerTexture;
  if iLoadPage == iFilePage and not fcbOnComplete then
    -- Set new page and actual page on texture
    iFilePage, iTilePage = iLoadPage, iPage % iPagesPerTexture;
    -- Update page number
    strPage = "PAGE "..iPageP1.."/"..iTotalPages;
    SetTip(strPage);
    -- No need to do anything else
    return;
  end
  -- Set displayed page number and assign the tip to it
  strPage = "LOADING P"..iPageP1;
  SetTip(strPage);
  -- Load the specified texture with the page image
  aPageAsset[1].F = "e/"..iLoadPage;
  LoadResources("Book"..iLoadPage,
    aPageAsset, OnPageAssetsLoaded, fcbOnComplete);
end
-- Switch page with sound -------------------------------------------------- --
local function GoAdjustPage(iNewPage)
  -- Return if same page else set new page
  iNewPage = UtilClampInt(iNewPage, 0, iTotalPagesM1);
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
  BlitLT(texBook, 8, 8);
end
-- Change cover to inside the book ----------------------------------------- --
local function GoOpen()
  -- Cover page confirmed
  bCoverPage = false;
  -- Set renderer to book page
  fcbProcRenderCover = ProcRenderPage;
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
  RenderInterface();
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
  texPage, texBook = nil, nil;
  -- Start the loading waiting procedure
  SetCallbacks(GameProc, RenderInterface);
  -- Continue game
  InitContinueGame();
end
-- On faded event ---------------------------------------------------------- --
local function OnExitLobbyFadedOut()
  -- Dereference assets for garbage collector
  texPage, texBook, texLobby = nil, nil, nil;
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
-- When cover page has loaded ---------------------------------------------- --
local function OnPageAssetsPost()
  -- If we've shown the cover page?
  if not bCoverPage then
    -- Set page keybank and callbacks
    iKeyBankStartId, iHotSpotStartId = iKeyBankPageId, iHotSpotPageId;
    fcbProcRenderCover = ProcRenderPage;
  -- Not shown the cover page yet? Set render callback
  else
    -- Set cover page keybank and callbanks
    iKeyBankStartId, iHotSpotStartId = iKeyBankCoverId, iHotSpotCoverId;
    fcbProcRenderCover = ProcRenderCover;
  end
  -- Cover has loaded
  fcbOnPageAssetsPost();
end
-- When the resources have loaded ------------------------------------------ --
local function OnAssetsLoaded(aResources, fcbProcCustomHandle)
  -- Set texture and setup tiles
  texBook = aResources[1];
  -- Call supplimental load routine depending if we're in-game or not
  fcbProcCustomHandle(aResources[2]);
  -- Load current page
  LoadPage(OnPageAssetsPost);
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
    aAssets[2] = nil;
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
    aAssets[2] = aLobbyClosedTexture;
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
  BlitSLT, BlitLT, Fade, GameProc, InitCon, InitContinueGame, LoadResources,
    PlayMusic, PlayStaticSound, RegisterHotSpot, RegisterKeys, RenderInterface,
    RenderShadow, RenderTip, RenderTipShadow, SetCallbacks, SetHotSpot,
    SetKeys, SetTip, aAssetsData, aCursorIdData, aSfxData =
      GetAPI("BlitSLT", "BlitLT", "Fade", "GameProc", "InitCon",
        "InitContinueGame", "LoadResources", "PlayMusic", "PlayStaticSound",
        "RegisterHotSpot", "RegisterKeys", "RenderInterface", "RenderShadow",
        "RenderTip", "RenderTipShadow", "SetCallbacks", "SetHotSpot",
        "SetKeys", "SetTip", "aAssetsData", "aCursorIdData", "aSfxData");
  -- Prepare assets
  local aTexture<const> = aAssetsData.book;
  aLobbyMusic = aAssetsData.lobbym;
  aLobbyClosedTexture = aAssetsData.lobbyc;
  aAssets = { aTexture, false };
  aPageAsset = { aAssetsData.bookpage };
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
    {  17,  70,  37,  22, 0, iCSelect, "INDEX PAGE", OnScroll, GoIndex },
    {  17,  96,  37,  22, 0, iCSelect, "NEXT PAGE",  OnScroll, GoNext  },
    {  17, 122,  37,  22, 0, iCSelect, "LAST PAGE",  OnScroll, GoLast  },
    {   8,   8, 304, 200, 0, 0,        HoverIdle,    OnScroll, false   },
    {   0,   0,   0, 240, 3, iCExit,   HoverExit,    OnScroll, GoExit  }
  });
end
-- Exports and imports ----------------------------------------------------- --
return { A = { InitBook = InitBook }, F = OnScriptLoaded };
-- End-of-File ============================================================= --
