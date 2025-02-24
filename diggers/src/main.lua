-- MAIN.LUA ================================================================ --
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
-- Lua aliases (optimisation) ---------------------------------------------- --
local collectgarbage<const>, cos<const>, error<const>, floor<const>,
  format<const>, max<const>, min<const>, pairs<const>, random<const>,
  remove<const>, rep<const>, sin<const>, tonumber<const>, tostring<const>,
  type<const>, unpack<const> =
    collectgarbage, math.cos, error, math.floor, string.format, math.max,
    math.min, pairs, math.random, table.remove, string.rep, math.sin, tonumber,
    tostring, type, table.unpack;
-- M-Engine aliases (optimisation) ----------------------------------------- --
local AssetParseBlock<const>, ClipSet<const>, CoreCatchup<const>,
  CoreEnd<const>, CoreLog<const>, CoreLogEx<const>, CoreOnTick<const>,
  CoreQuit<const>, CoreReset<const>, CoreStack<const>, CoreWrite<const>,
  FboDraw<const>, FboMatrix<const>,  InputOnKey<const>, InputSetCursor<const>,
  CoreTime<const>, TextureCreate<const>, UtilBlank<const>, UtilClamp<const>,
  UtilDuration<const>, UtilExplode<const>, UtilImplode<const>,
  UtilIsFunction<const>, UtilIsInteger<const>, UtilIsNumber<const>,
  UtilIsString<const>, UtilIsTable<const>, VariableRegister<const> =
    Asset.ParseBlock, Clip.Set, Core.Catchup, Core.End, Core.Log, Core.LogEx,
    Core.OnTick, Core.Quit, Core.Reset, Core.Stack, Core.Write, Fbo.Draw,
    Fbo.Matrix, Input.OnKey, Input.SetCursor, Core.Time, Texture.Create,
    Util.Blank, Util.Clamp, Util.Duration, Util.Explode, Util.Implode,
    Util.IsFunction, Util.IsInteger, Util.IsNumber, Util.IsString,
    Util.IsTable, Variable.Register;
-- Locals ------------------------------------------------------------------ --
local CBProc, CBRender;                -- Generic tick callbacks
local aAPI<const> = { };               -- API to send to other functions
local aCache = { };                    -- File cache
local aKeys<const> = Input.KeyCodes;   -- Keyboard codes
local aModules<const> = { };           -- Modules data
local bTestMode = false;               -- Test mode enabled
local fFont<const> = Font.Console();   -- Main console class
local fboMain<const> = Fbo.Main();     -- Main frame buffer object class
local fcbFading = false;               -- Fading callback
local fcbFrameBufferCbs<const> = { };  -- Frame buffer updated function
local fontLarge;                       -- Large font (16px)
local fontLittle;                      -- Little font (8px)
local fontSpeech;                      -- Speech font (10px)
local fontTiny;                        -- Tiny font (5px)
local iTexScale;                       -- Texture scale
local sTip;                            -- Current tip and bounds
local texSpr;                          -- Sprites texture
-- Stage dimensions -------------------------------------------------------- --
local iStageWidth  = 320;              -- Width of stage (Monitor)
local iStageHeight = 240;              -- Height of stage (Monitor)
local iOrthoWidth  = iStageWidth;      -- Width of stage (4:3)
local iOrthoHeight = iStageHeight;     -- Height of stage (4:3)
local iStageLeft   = 0;                -- Left of stage
local iStageTop    = 0;                -- Top of stage
local iStageRight  = iStageWidth;      -- Right of stage
local iStageBottom = iStageHeight;     -- Bottom of stage
local iStageLeftO  = iStageLeft;       -- Left of stage
local iStageTopO   = iStageTop;        -- Top of stage
local iStageWidthO = iStageWidth;      -- Top of stage
-- Library functions loaded later ------------------------------------------ --
local BlitSLTRB, BlitSLT, ClearStates, InitCredits, InitTitleCredits, InitDebugPlay,
  InitEnding, InitFail, InitIntro, InitNewGame, InitScene, InitScore,
  InitTitle, IsMouseInBounds, JoystickProc, LoadLevel, MainProcFunc,
  MusicVolume, PrintC, aLevelsData, aObjectTypes, aRacesData;
-- These could be called even though they aren't initialised yet ----------- --
local DisableKeyHandlers, RestoreKeyHandlers, SetKeys, SetHotSpot =
  UtilBlank, UtilBlank, UtilBlank, UtilBlank;
-- Constants for loader ---------------------------------------------------- --
local aBFlags<const> = Image.Flags;        -- Get bitmap loading flags
local iPNG<const> = aBFlags.TOGPU|aBFlags.FCE_PNG;-- Get forced PNG format flag
local aPFlags<const> = Pcm.Flags;          -- Get waveform loading flags
local iOGG<const> = aPFlags.FCE_OGG;       -- Get forced wave format
local aPrFlags<const> = Asset.Progress;    -- Asset progress flags
local iFStart<const> = aPrFlags.FILESTART; -- File opened with information
-- Table debug function (global) ------------------------------------------- --
function Debug(aData)
  -- Printing function
  local function Print(iIndent, sWhat) CoreWrite(rep(" ", iIndent)..sWhat) end
  -- Debug a variable
  local function DoDump(sName, aData, iLv)
    -- Print variable name
    Print(iLv, sName.." ("..tostring(aData)..") = {");
    -- Index
    local iI = 0;
    -- Increase indent
    iLv = iLv + 2;
    -- Enumerate keys and values
    for sK, vV in pairs(aData) do
      -- Recurse if a table
      if type(vV) == "table" then DoDump(sK, vV, iLv);
      -- Print key and value
      else Print(iLv, sK.." : "..type(vV).." = "..tostring(vV)) end;
      -- Increment counter
      iI = iI + 1;
    end
    -- Decrease indent
    iLv = iLv - 2;
    -- Print total
    Print(iLv, "} = "..iI.." ["..#aData.."]");
  end
  -- Must be a table
  if type(aData) == "table" then return DoDump("ROOT", aData, 0) end;
  -- Just a variable
  Print(0, type(aData).." = "..tostring(aData));
end
-- Parse the return value of a script -------------------------------------- --
local function ParseScriptResult(sName, aModData)
  -- Check parameters
  if not UtilIsString(sName) then error("Bad name: "..tostring(sName)) end;
  if not UtilIsTable(aModData) then error(sName..": bad return!") end;
  local fcbModCb<const> = aModData.F;
  if not UtilIsFunction(fcbModCb) then error(sName..": bad callback!") end;
  local aModAPI<const> = aModData.A;
  if not UtilIsTable(aModAPI) then error(sName..": bad api!") end;
  -- Set name of module
  aModData.N = sName;
  -- Add functions to the api
  for sKey, vVar in pairs(aModAPI) do
    -- Check variable name
    if not UtilIsString(sKey) then
      error(sName.."["..tostring(sKey).."] bad key!") end;
    -- Check not already registered
    if aAPI[sKey] ~= nil then
      error(sName.."["..sKey.."] already registered!") end;
    -- Check that value is valid
    if nil == vVar then error(sName.."["..sKey.."] bad variable!") end;
    -- Assign variable in internal API
    aAPI[sKey] = vVar;
  end
  -- Put returned data in API for later when everything is loaded and we'll
  -- call the modules callback function with the fully loaded API.
  aModules[1 + #aModules] = aModData;
end
-- Function to parse a script ---------------------------------------------- --
local function ParseScript(aScript)
  -- Get name of module
  local sName<const> = aScript:Name();
  -- Compile the script and parse the return value
  ParseScriptResult(sName, AssetParseBlock(sName, 1, aScript));
  -- Return success
  return true;
end
-- Get callbacks ----------------------------------------------------------- --
local function GetCallbacks() return CBProc, CBRender end;
-- Set callbacks ----------------------------------------------------------- --
local function SetCallbacks(CBP, CBR)
  CBProc, CBRender = CBP or UtilBlank, CBR or UtilBlank end;
-- Error handler ----------------------------------------------------------- --
local function SetErrorMessage(sReason)
  -- Activate main frame buffer object just incase it isn't
  fboMain:Activate();
  -- Show cursor
  InputSetCursor(true);
  -- Convert to string if it isn't
  if not UtilIsString(sReason) then sReason = tostring(sReason) end;
  -- Make sure text doesn't go off the screen
  local sFullReason<const> = sReason;
  -- Parse lines in stack string
  local aLines<const> = UtilExplode(sReason, "\n");
  -- Prune locals (keeping them in the full reason)
  for iIndex = #aLines, 1, -1 do
    if aLines[iIndex]:sub(1, 2) == "--" then remove(aLines, iIndex) end;
  end
  -- Prune too many lines
  if #aLines > 15 then
    while #aLines > 15 do remove(aLines, #aLines) end;
    aLines[1 + #aLines] = "...more";
  end
  -- Build short reason
  sReason = UtilImplode(aLines, "\n");
  -- Log the message
  CoreLogEx(sFullReason, 1);
  -- Add generic info to the message
  local sMessage<const> =
    "ERROR!\n\n\z
     \rcffffff00The program has halted due to an unexpected problem.\rr\n\n\z
     Reason:-\n\n\z
     \rcffffff00"..sReason.."\rr\n\n\z
     C:Continue  R:Restart  A:Abort  P:Clipboard  F:Fail";
  -- Get key states
  local iRelease<const> = Input.States.RELEASE;
  -- Keys used in tick function
  local iKeyC<const>, iKeyR<const>, iKeyA<const>, iKeyP<const>, iKeyF<const> =
    aKeys.C, aKeys.R, aKeys.A, aKeys.P, aKeys.F;
  -- Disable key handlers
  DisableKeyHandlers();
  -- Input event callback
  local function OnKey(iKey, iState)
    -- Ignore if not releasing a key
    if iState ~= iRelease then return end;
    -- Continue key pressed?
    if iKey == iKeyC then
      -- Hide cursor
      InputSetCursor(false);
      -- Restore key handlers if available
      RestoreKeyHandlers();
      -- Restore tick function
      CoreOnTick(MainProcFunc);
    -- Restart key pressed?
    elseif iKey == iKeyR then CoreReset();
    -- Abort key pressed?
    elseif iKey == iKeyA then CoreQuit();
    -- Clipboard key pressed?
    elseif iKey == iKeyP then ClipSet("C", sFullReason, UtilBlank);
    -- Fail key pressed?
    elseif iKey == iKeyF then CoreEnd() end;
  end
  -- Override current input funciton
  InputOnKey(OnKey);
  -- Second change bool
  local nNext = 0;
  -- Text position and width
  local iTextLeft<const> = iStageLeftO + 8;
  local iTextTop<const> = iStageTopO + 8;
  local iTextWidth<const> = iStageWidthO - 8;
  -- Callback function
  local function OnTick()
    -- Set clear colour depending on time
    local nTime<const>, nRed = CoreTime();
    nRed = cos(nTime) * sin(nTime) + 0.5;
    -- Show error message
    fboMain:SetClearColour(nRed, 0, 0, 1);
    fFont:SetCRGBA(1, 1, 1, 1);
    fFont:SetSize(1);
    fFont:PrintW(iTextLeft, iTextTop, iTextWidth, 0, sMessage);
    -- Draw frame if we changed the background colour
    if nTime >= nNext then FboDraw() nNext = nTime + 0.032 end;
  end
  -- Set loop function
  CoreOnTick(OnTick);
end
-- Do render the tip ------------------------------------------------------- --
local function DoRenderTip(iX)
  -- Draw the background of the tip rect
  BlitSLT(texSpr, 847, iX,      216);
  BlitSLT(texSpr, 848, iX + 16, 216);
  BlitSLT(texSpr, 848, iX + 32, 216);
  BlitSLT(texSpr, 848, iX + 48, 216);
  BlitSLT(texSpr, 849, iX + 64, 216);
  -- Set tip colour and render the text
  fontLittle:SetCRGB(1, 1, 1);
  PrintC(fontLittle, iX + 40, 220, sTip);
end
-- Render the tip in the bottom right -------------------------------------- --
local function RenderTip()
  -- Return if no tip
  if not sTip then return end;
  -- Draw tip in different positions if mouse cursor is over the tip
  if IsMouseInBounds(232, 216, 312, 232) then DoRenderTip(144);
                                         else DoRenderTip(232) end;
end
-- Render the tip ---------------------------------------------------------- --
local function RenderTipShadow() if sTip then DoRenderTip(232) end end;
-- Set bottom right tip ---------------------------------------------------- --
local function SetTip(strTip) sTip = strTip end;
-- ------------------------------------------------------------------------- --
local function TimeIt(sName, fcbCallback, ...)
  -- Check parameters
  if not UtilIsString(sName) then
    error("Name string is invalid! "..tostring(sName)) end;
  if not UtilIsFunction(fcbCallback) then
    error("Function is invalid! "..tostring(fcbCallback)) end;
  -- Save time
  local nTime<const> = CoreTime();
  -- Execute function
  fcbCallback(...);
  -- Put result in console
  CoreLog("Procedure '"..sName.."' completed in "..
    UtilDuration(CoreTime()-nTime, 3).." sec!");
end
-- Generic function to return a handle ------------------------------------- --
local function NoSecondStage(hH) return hH end;
-- Asset types supported --------------------------------------------------- --
local aTypes<const> = {
  -- Async function   Params  Prefix  Suffix  Data loader function  Info?   Id
  { Image.FileAsync,  {iPNG}, "tex/", ".png", Texture.CreateTS, false,   --  1
    { 1, 2, 3, 4 } },
  { Image.FileAsync,  {iPNG}, "tex/", ".png", TextureCreate,    false }, --  2
  { Image.FileAsync,  {iPNG}, "tex/", ".png", Font.Image,       false,   --  3
    { 1, 2, 3, 4, 9 } },
  { Pcm.FileAsync,    {iOGG}, "sfx/", ".ogg", Sample.Create,    false }, --  4
  { Asset.FileAsync,  {0},    "",     "",     NoSecondStage,    false }, --  5
  { Image.FileAsync,  {0},    "tex/", ".png", Mask.Create,      false }, --  6
  { Stream.FileAsync, {},     "mus/", ".ogg", NoSecondStage,    false }, --  7
  { Video.FileAsync,  {},     "fmv/", ".ogv", NoSecondStage,    false }, --  8
  { Asset.FileAsync,  {0},    "src/", ".lua", ParseScript,      true  }, --  9
  { Image.FileAsync,  {iPNG}, "tex/", ".png", Texture.ImageUT,  false,   -- 10
    { 2 } },
  { Json.FileAsync,   {},     "",    ".json", Json.ToTable,     false }, -- 11
  -- Async function   Params  Prefix Suffix  Data loader function  Info?    Id
};
-- Loader ------------------------------------------------------------------ --
local function LoadResources(sProcedure, aResources, fComplete, ...)
  -- Check parameters
  if not UtilIsString(sProcedure) then
    error("Procedure name string is invalid! "..tostring(sProcedure)) end;
  if not UtilIsTable(aResources) then
    error("Resources table is invalid! "..tostring(aResources)) end;
  if #aResources == 0 then error("No resources specified to load!") end;
  if not UtilIsFunction(fComplete) then
    error("Finished callback is invalid! "..tostring(fComplete)) end;
  -- Initialise queue
  local sDst, aInfo, aNCache, iTotal, iLoaded = "", { }, { }, nil, nil;
  -- Progress update on asynchronous loading
  local function ProgressUpdate(iCmd, ...)
    if iCmd == iFStart then aInfo = { ... } end;
  end
  -- Output handles
  local aOutputHandles<const> = { };
  -- Grab extra parameters to send to callback
  local aParams<const> = { ... };
  -- Load item
  local function LoadItem(iI)
    -- Get resource data
    local aResource<const> = aResources[iI];
    if not UtilIsTable(aResource) then
      error("Supplied table at index "..iI.." is invalid!") end;
    -- Get type of resource and throw error if the type is invalid
    local aTypeData<const> = aTypes[aResource.T];
    if not UtilIsTable(aTypeData) then
      error("Supplied load type of '"..tostring(aResource.T)..
        "' is invalid at index "..iI.."!") end;
    -- Get destination file to load and check it
    sDst = aTypeData[3]..aResource.F..aTypeData[4];
    if #sDst == 0 then error("Filename at index "..iI.." is empty!") end;
    -- Build parameters table to send to function
    local aSrcParams<const> = aTypeData[2];
    local aDstParams<const> = { sDst,                 -- [1]
                                unpack(aSrcParams) }; -- [2]
    aDstParams[1 + #aDstParams] = SetErrorMessage;    -- [3]
    aDstParams[1 + #aDstParams] = ProgressUpdate;     -- [4]
    -- Get no-cache setting
    local bNoCache<const> = aResource.NC;
    -- When final handle has been acquired
    local function OnHandle(vHandle, bCached)
      -- Set handle for final callback
      aOutputHandles[iI] = vHandle;
      -- Cache the handle unless requested not to
      if not bNoCache then aNCache[sDst] = vHandle end;
      -- Set stage 2 duration and total duration
      aResource.ST2 = CoreTime() - aResource.ST2;
      aResource.ST3 = aResource.ST1 + aResource.ST2;
      -- Loaded counter increment
      iLoaded = iLoaded + 1;
      -- No need to show intermediate load times if cached
      if bCached then bCached = ".";
      -- Wasn't cached?
      else
        -- Calculate times for log
        bCached = " ("..UtilDuration(aResource.ST1, 3).."+"..
                        UtilDuration(aResource.ST2, 3);
        -- Add no cache flag and finish string
        if bNoCache then bCached = bCached.."/NC).";
                    else bCached = bCached..")." end;
      end
      -- Say in log that we loaded
      CoreLog("Loaded resource "..iLoaded.."/"..iTotal..": '"..
        sDst.."' in "..UtilDuration(aResource.ST3, 3).." sec"..bCached);
      -- Load the next item if not completed yet
      if iLoaded < iTotal then return LoadItem(iI + 1) end;
      -- Set new cache
      aCache = aNCache;
      -- Enable global keys
      SetKeys(true);
      -- Clear tip
      SetTip();
      -- Garbage collect to remove unloaded assets
      collectgarbage();
      -- Execute finished handler function with our resource list
      TimeIt(sProcedure, fComplete, aOutputHandles, unpack(aParams));
    end
    -- Setup handle
    local function SetupSecondStage()
      -- Get current time
      local nTime<const> = CoreTime();
      -- Set stage 1 duration and stage 2 start time
      aResource.ST1 = nTime - aResource.ST1;
      aResource.ST2 = nTime;
      -- Get final call parameters and if not specified?
      local aParams = aResource.P;
      if aParams == nil then
        -- Set empty parameters table
        aParams = { };
        aResource.P = aParams;
      -- Check that user specified parameters are valid
      elseif not UtilIsTable(aParams) then
        error("Invalid params "..tostring(aParams).." at index "..iI.."!") end;
      -- File information required? Add all the parameters
      if aTypeData[6] then
        for iI = 1, #aInfo do aParams[1 + #aParams] = aInfo[iI] end;
      end
    end
    -- When first handle has been loaded
    local function OnLoaded(vData)
      -- Setup second stage
      SetupSecondStage();
      -- Load the file and set the handle
      OnHandle(aTypeData[5](vData, unpack(aResource.P)));
    end
    aDstParams[1 + #aDstParams] = OnLoaded;
    -- Set stage 1 time
    aResource.ST1 = CoreTime();
    -- Reset info for progress update
    while #aInfo > 0 do remove(aInfo, #aInfo) end;
    -- Send cached handle if it exists
    local vCached<const> = aCache[sDst];
    if vCached then
      -- Setup second stage
      SetupSecondStage();
      -- Send straight to handle
      OnHandle(vCached, true);
    -- Dispatch the call
    else aTypeData[1](unpack(aDstParams)) end;
  end
  -- Disable global keys until everything has loaded
  SetKeys(false);
  -- Disable hotspots
  SetHotSpot();
  -- Set tip to loading
  SetTip("LOADING...");
  -- Initialise counters
  iTotal, iLoaded = #aResources, 0;
  -- Clear callbacks but keep the last render callback
  SetCallbacks(nil, CBRender);
  -- Load first item
  LoadItem(1);
  -- Progress function
  local function GetProgress() return iLoaded/iTotal, sDst end
  -- Return progress function
  return GetProgress;
end
-- Render fade ------------------------------------------------------------- --
local function RenderFade(nAmount, iL, iT, iR, iB, iS)
  texSpr:SetCA(nAmount);
  BlitSLTRB(texSpr, iS or 1023, iL or iStageLeft,  iT or iStageTop,
                                iR or iStageRight, iB or iStageBottom);
  texSpr:SetCA(1);
end
-- Render shadow ----------------------------------------------------------- --
local function RenderShadow(iL, iT, iR, iB)
  -- Draw a shadow using the solid sprite
  texSpr:SetCA(0.2);
  BlitSLTRB(texSpr, 1023, iL+3, iB, iR, iB+1); -- Horizontal row 1
  BlitSLTRB(texSpr, 1023, iR, iT+3, iR+1, iB); -- Vertical column 1
  texSpr:SetCA(0.1);
  BlitSLTRB(texSpr, 1023, iL+4, iB, iR, iB+2); -- Horizontal row 2
  BlitSLTRB(texSpr, 1023, iR, iT+4, iR+2, iB); -- Vertical column 2
  BlitSLTRB(texSpr, 1023, iR, iB, iR+2, iB+1); -- Horizontal corner 1
  BlitSLTRB(texSpr, 1023, iR, iB, iR+1, iB+2); -- Vertical corner 2
  texSpr:SetCA(0.05);
  BlitSLTRB(texSpr, 1023, iL+5, iB, iR, iB+3); -- Horizontal row 3
  BlitSLTRB(texSpr, 1023, iR, iT+5, iR+3, iB); -- Vertical column 3
  texSpr:SetCA(1);
end
-- Fade -------------------------------------------------------------------- --
local function Fade(S, E, C, D, A, M, L, T, R, B, Z)
  -- Check parameters
  if not UtilIsNumber(S) then
    error("Invalid starting value number! "..tostring(S)) end;
  if not UtilIsNumber(E) then
    error("Invalid ending value number! "..tostring(E)) end;
  if not UtilIsNumber(C) then
    error("Invalid fade inc/decremember value! "..tostring(C)) end
  if not UtilIsFunction(A) then
    error("Invalid after function! "..tostring(A)) end;
  -- If already fading, run the after function
  if UtilIsFunction(fcbFading) then fcbFading() end;
  -- Disable all keybanks and globals
  SetKeys(false);
  -- Disable hotspots
  SetHotSpot();
  -- During function
  local function During(nVal)
    -- Clear states
    ClearStates();
    -- Call users during function
    D();
    -- Clamp new fade value
    S = UtilClamp(nVal, 0, 1);
    -- Render blackout
    RenderFade(S, L, T, R, B, Z);
    -- Fade music too
    if M then MusicVolume(1 - S) end;
  end
  -- Finished function
  local function Finish()
    -- Reset fade vars
    S, fcbFading = E, nil;
    -- Enable global keys
    SetKeys(true);
    -- No callbacks incase caller forgets to set anything
    SetCallbacks(nil, nil);
    -- Call the after function
    A();
  end
  -- Cleanup function
  local function Clean()
    -- Garbage collect
    collectgarbage();
    -- Reset hi-res timer
    CoreCatchup();
  end
  -- Fade out?
  if S < E then
    -- Save old fade function
    fcbFading = A;
    -- Function during
    local function OnFadeOutFrame()
      -- Fade out
      During(S + C);
      -- Finished if we reached the ending point
      if S < E then return end;
      -- Cleanup
      Clean();
      -- Call finish function
      Finish()
    end
    -- Set fade out procedure
    SetCallbacks(nil, OnFadeOutFrame);
  -- Fade in?
  elseif S > E then
    -- Cleanup
    Clean();
    -- Save old fade function
    fcbFading = A;
    -- Function during
    local function OnFadeInFrame()
      -- Fade in
      During(S - C);
      -- Finished if we reached the ending point
      if S <= E then Finish() end;
    end
    -- Set fade in procedure
    SetCallbacks(nil, OnFadeInFrame);
  -- Ending already reached?
  else
    -- Cleanup
    Clean();
    -- Call finish function
    Finish();
  end
end
-- Refresh viewport info --------------------------------------------------- --
local function RefreshViewportInfo()
  -- Refresh matrix parameters
  iStageWidthO, iStageHeight,
    iStageLeftO, iStageTopO, iStageRight, iStageBottom = fboMain:GetMatrix();
  iOrthoWidth, iOrthoHeight = FboMatrix();
  -- Floor all the values as the main frame buffer object is always on the
  -- pixel boundary
  iStageWidth, iStageHeight, iStageLeft, iStageTop, iStageRight, iStageBottom,
    iOrthoWidth, iOrthoHeight =
      floor(iStageWidthO)//iTexScale, floor(iStageHeight)//iTexScale,
      floor(iStageLeftO)//iTexScale, floor(iStageTopO)//iTexScale,
      floor(iStageRight)//iTexScale, floor(iStageBottom)//iTexScale,
      floor(iOrthoWidth)//iTexScale, floor(iOrthoHeight)//iTexScale;
  -- Call frame buffer callbacks
  for _, fcbC in pairs(fcbFrameBufferCbs) do
    -- Protected call so we can handle errors
    local bResult<const>, sReason<const> = xpcall(fcbC, CoreStack,
      iStageWidth, iStageHeight, iStageLeft, iStageTop, iStageRight,
      iStageBottom, iOrthoWidth, iOrthoHeight);
    if not bResult then SetErrorMessage(sReason) end;
  end
end
-- Register a callback and automatically when window size changes ---------- --
local function RegisterFrameBufferUpdateCallback(sName, fCB)
  -- Check parameters
  if not UtilIsString(sName) then
    error("Invalid callback name string! "..tostring(sName)) end;
  if nil ~= fCB and not UtilIsFunction(fCB) then
    error("Invalid callback function! "..tostring(fCB)) end;
  -- Register callback when frame buffer is updated
  fcbFrameBufferCbs[sName] = fCB;
  -- If a callback was set then call it
  if nil ~= fCB then
    fCB(iStageWidth, iStageHeight, iStageLeft, iStageTop, iStageRight,
      iStageBottom, iOrthoWidth, iOrthoHeight) end;
end
-- Returns wether test mode is enabled ------------------------------------- --
local function GetTestMode() return bTestMode end;
-- The first tick function ------------------------------------------------- --
local function fcbTick()
  -- Load base assets data
  local aScriptTypeData<const> = aTypes[9];
  local aAssetsData, aBaseAssets, iBaseScripts, iBaseFonts, iBaseTextures,
    iBaseMasks, iBaseSounds, aBaseSounds;
  aAssetsData, aBaseAssets, iBaseScripts, iBaseFonts, iBaseTextures,
    iBaseMasks, iBaseSounds, aBaseSounds =
      Asset.Parse(aScriptTypeData[3].."asset"..aScriptTypeData[4], 9);
  -- Customised texture scale file exists?
  local sScaleFile<const> = "tex/scale.txt";
  if Asset.FileExists(sScaleFile) then
    -- Load the texture scale number from file and make sure it is valid
    local nTexScale<const> = tonumber(Asset.File(sScaleFile, 0):ToString());
    if not nTexScale then error("Erroneous texture scale '"..
      tostring(nTexScale).."' in '"..sScaleFile.."'!") end;
    -- Round it down and check it incase it's not a valid integer and check it
    iTexScale = math.floor(nTexScale);
    if iTexScale ~= nTexScale or iTexScale > 16 then
      error("Bad texture scale '"..nTexScale.."' in '"..sScaleFile.."'!") end;
    -- Get maximum texture size and make sure guest's GPU supports it. 1024^2
    -- is the maximum size texture we use at 1X scale.
    local iMaxUsedTexSize<const> = 1024;
    local iMaxSize<const> = Texture.MaxSize();
    if iTexScale * iMaxUsedTexSize > iMaxSize then
      local _<const>, _<const>, sDisplay<const> = Display.GPU();
      error("Fatal error! The installed "..iTexScale.."X scale texture pack \z
             is not supported on this rendering device ("..sDisplay..") as \z
             it will only support a maximum scale of "..
             (iMaxSize//iMaxUsedTexSize).."X ("..iMaxSize.."^2).");
    end
    -- Now we have to scale all relavant co-ordinates so for each asset
    for sIdentifier, aAssetData in pairs(aAssetsData) do
      -- Get type and if not valid show an error
      local iType<const> = aAssetData.T;
      if not UtilIsInteger(iType) then
        error("Invalid type '"..tostring(iType).."' in "..
          sIdentifier.."!") end;
      -- Get type data and if not valid then show an error
      local aTypeItem<const> = aTypes[iType];
      if not UtilIsTable(aTypeItem) then
        error("Invalid type data '"..tostring(aTypeItem).."' in "..
          sIdentifier.."!") end;
      -- Check that we have parameters to modify and if we do?
      local aParamsToModify<const> = aTypeItem[7];
      if UtilIsTable(aParamsToModify) then
        -- Get and check function parameters
        local aFuncParams = aAssetData.P;
        if not UtilIsTable(aFuncParams) then
          error("Invalid func params data '"..tostring(aFuncParams)..
            "' in data for type "..iType.." in "..sIdentifier.."!") end;
        -- Walk through the parameters to modify
        for iPMIndex = 1, #aParamsToModify do
          -- Get and check the function param
          local iFPIndex = aParamsToModify[iPMIndex];
          local vParam = aFuncParams[iFPIndex];
          if UtilIsTable(vParam) then
            -- Scale all its values
            for iPAIndex = 1, #vParam do
              -- Get current value and make sure it is valid
              local iValue<const> = vParam[iPAIndex];
              if UtilIsInteger(iValue) then
                vParam[iPAIndex] = iValue * iTexScale;
              -- Invalid value type
              else error("Invalid array param type '"..tostring(iValue)..
                "' at '"..iPAIndex..":"..iFPIndex"' for type "..iType..
                " in "..sIdentifier.."!") end;
            end
          -- Scale just the integer
          elseif UtilIsInteger(vParam) then
            aFuncParams[iFPIndex] = vParam * iTexScale;
          -- Unknown format. This is an error
          else error("Invalid param type '"..tostring(vParam).."' at '"..
            iFPIndex"' for type "..iType.." in "..sIdentifier.."!") end;
        end
      -- Invalid parameters to modify data if not nil
      elseif aParamsToModify ~= nil then
        error("Invalid type param data '"..tostring(aParamsToModify)..
          "' for type "..iType.." in "..sIdentifier.."!") end;
    end
    -- Resize frame buffer if texture scale different
    local VariableGetInt<const>, aVariables<const> =
      Variable.GetInt, Variable.Internal;
    Fbo.Resize(VariableGetInt(aVariables.vid_orwidth) * iTexScale,
               VariableGetInt(aVariables.vid_orheight) * iTexScale);
  -- No scale file found so no texture scale by default
  else iTexScale = 1 end;
  -- Refresh viewport info and automatically when window size changes
  Fbo.OnRedraw(RefreshViewportInfo);
  RefreshViewportInfo();
  -- Initialise base API functions
  ParseScriptResult("main", { F=UtilBlank, A={ Fade = Fade,
    GetCallbacks = GetCallbacks, GetTestMode = GetTestMode,
    LoadResources = LoadResources, RefreshViewportInfo = RefreshViewportInfo,
    RegisterFBUCallback = RegisterFrameBufferUpdateCallback,
    RenderFade = RenderFade, RenderShadow = RenderShadow,
    RenderTip = RenderTip, RenderTipShadow = RenderTipShadow,
    SetCallbacks = SetCallbacks, SetErrorMessage = SetErrorMessage,
    SetTip = SetTip, TimeIt = TimeIt } });
  -- Store texture scale and assets data
  aAPI.iTexScale = iTexScale;
  aAPI.aAssetsData = aAssetsData;
  -- When base assets have loaded
  local function OnLoaded(aResources)
    -- Set font handles
    fontLarge, fontLittle, fontTiny, fontSpeech =
      aResources[iBaseFonts], aResources[iBaseFonts + 1],
      aResources[iBaseFonts + 2], aResources[iBaseFonts + 3];
    aAPI.fontLarge, aAPI.fontLittle, aAPI.fontTiny, aAPI.fontSpeech =
      fontLarge, fontLittle, fontTiny, fontSpeech;
    -- Set sprites texture
    texSpr = aResources[iBaseTextures];
    aAPI.texSpr = texSpr;
    -- Set and check masks
    aAPI.maskLevel, aAPI.maskSprites =
      aResources[iBaseMasks], aResources[iBaseMasks + 1];
    -- Function to grab an API function. This function will be sent to all
    -- the above loaded modules.
    local function GetAPI(...)
      -- Get functions and if there is only one then return it
      local tFuncs<const> = { ... }
      if #tFuncs == 0 then error("No functions specified to check") end;
      -- Functions already added
      local aAdded<const> = { };
      -- Find each function specified and return all of them
      local tRets<const> = { };
      for iI = 1, #tFuncs do
        -- Check parameter
        local sMember<const> = tFuncs[iI];
        if not UtilIsString(sMember) then
          error("Function name at "..iI.." is invalid") end;
        -- Check if we already cached this member and if already have it?
        local iCached<const> = aAdded[sMember];
        if iCached ~= nil then
          -- Print an error so we can remove duplicates
          error("Member '"..sMember.."' at parameter "..iI..
            " already requested at parameter "..iCached.."!");
        end
        -- Get the function callback and if it's a function?
        local vMember<const> = aAPI[sMember];
        if vMember == nil then
          error("Invalid member '"..sMember.."'! "..tostring(vMember));
        end
        -- Cache function so we can track duplicated
        aAdded[sMember] = iI;
        -- Add it to returns
        tRets[1 + #tRets] = vMember;
      end
      -- Unpack returns table and return all the functions requested
      return unpack(tRets);
    end
    -- Empty callback function for CVar events
    local function fcbEmpty() return true end;
    -- Register file data CVar
    local aCVF<const> = Variable.Flags;
    -- Default CVar flags for string storage
    local iCFR<const> = aCVF.STRINGSAVE|aCVF.TRIM|aCVF.PROTECTED|aCVF.DEFLATE;
    -- Default CVar flags for boolean storage
    local iCFB<const> = aCVF.BOOLEANSAVE;
    -- 4 save slots so 4 save variables
    for iI = 1, 4 do
      aAPI["cvData"..iI] =
        VariableRegister("gam_data"..iI, "", iCFR, fcbEmpty);
    end
    -- ...and a CVar that lets us show setup for the first time
    aAPI.cvSetup = VariableRegister("gam_setup", 1, iCFB, fcbEmpty);
    -- ...and a CVar that lets us skip the intro
    aAPI.cvIntro = VariableRegister("gam_intro", 1, iCFB, fcbEmpty);
    -- ...and a CVar that lets us start straight into a level
    aAPI.cvTest = VariableRegister("gam_test", "", aCVF.STRING, fcbEmpty);
    -- Load dependecies we need on this module
    BlitSLTRB, BlitSLT, ClearStates, DisableKeyHandlers, InitCredits,
      InitDebugPlay, InitEnding, InitFail, InitIntro, InitNewGame, InitScene,
      InitScore, InitTitle, InitTitleCredits, IsMouseInBounds, JoystickProc,
      LoadLevel, MusicVolume, PrintC, RestoreKeyHandlers, SetHotSpot, SetKeys,
      aLevelsData, aObjectTypes, aRacesData =
        GetAPI("BlitSLTRB", "BlitSLT", "ClearStates", "DisableKeyHandlers",
          "InitCredits", "InitDebugPlay", "InitEnding", "InitFail",
          "InitIntro", "InitNewGame", "InitScene", "InitScore", "InitTitle",
          "InitTitleCredits", "IsMouseInBounds", "JoystickProc", "LoadLevel",
          "MusicVolume", "PrintC", "RestoreKeyHandlers", "SetHotSpot",
          "SetKeys", "aLevelsData", "aObjectTypes", "aRacesData");
    -- Assign loaded sound effects (audio.hpp)
    GetAPI("RegisterSounds")(aResources, iBaseSounds, #aBaseSounds);
    -- Get cursor render function (input.hpp)
    local CursorRender<const> = aAPI.CursorRender;
    -- Ask modules to grab needed functions from the API
    for iI = 1, #aModules do
      local aModData<const> = aModules[iI];
      aModData.F(GetAPI, aModData);
    end
    -- Main procedure callback
    local function MainCallback()
      -- Poll joysticks (input.hpp)
      JoystickProc();
      -- Execute input, tick and render callbacks
      CBProc();
      CBRender();
      -- Draw mouse cursor (input.hpp)
      CursorRender();
      -- Draw screen at end of LUA tick
      FboDraw();
    end
    -- Set main callback
    fcbTick = MainCallback;
    -- Init game counters so testing stuff quickly works properly
    InitNewGame();
    -- Hide the cursor
    InputSetCursor(false);
    -- Tests
    local sTestValue<const> = aAPI.cvTest:Get();
    if #sTestValue > 0 then
      -- Test mode enabled
      bTestMode = true;
      -- Get start level
      local iStartLevel<const> = tonumber(sTestValue) or 0;
      -- Test random level?
      if iStartLevel == 0 then
        return LoadLevel(random(#aLevelsData), "game", -1);
      -- Testing infinite play mode?
      elseif iStartLevel == -1 then return InitDebugPlay();
      -- Testing the fail screen
      elseif iStartLevel == -2 then return InitFail();
      -- Testing the game over
      elseif iStartLevel == -3 then return InitScore();
      -- Testing the final credits
      elseif iStartLevel == -4 then return InitCredits(false);
      -- Testing the final rolling credits
      elseif iStartLevel == -5 then return InitCredits(true);
      -- Testing the title screen rolling credits
      elseif iStartLevel == -6 then return InitTitleCredits();
      -- Testing a races ending
      elseif iStartLevel > -11 and iStartLevel <= -7 then
        return InitEnding(#aRacesData + (-11 - iStartLevel));
      -- Reserved for testing map post mortem maybe (todo)
      elseif iStartLevel <= -11 then
      -- Test a specific lvel
      elseif iStartLevel <= #aLevelsData then
        return LoadLevel(iStartLevel, "game", -1);
      -- Test a specific level with starting scene
      elseif iStartLevel > #aLevelsData and iStartLevel <= #aLevelsData*2 then
        return InitScene(iStartLevel-#aLevelsData, "game");
      end
    end
    -- If being run for first time
    if 0 == tonumber(aAPI.cvSetup:Get()) then
      -- Skip intro? Initialise title screen
      if 0 == tonumber(aAPI.cvIntro:Get()) then return InitTitle() end;
      -- Initialise intro with setup dialog
      return InitIntro(false);
    end
    -- Initialise setup screen by default
    InitIntro(true);
    -- No longer show setup screen
    aAPI.cvSetup:Set(0);
  end
  -- Start loading assets
  local fcbProgress<const> = LoadResources("Core", aBaseAssets, OnLoaded);
  -- Get console font and do positional calculations
  local fSolid<const> = TextureCreate(Image.Colour(0xFFFFFFFF), 0);
  local iWidth<const>, iHeight<const>, iBorder<const> =
    (300 * iTexScale), (2 * iTexScale), (1 * iTexScale);
  local iX<const> = (160 * iTexScale) - (iWidth / 2) - iBorder;
  local iY<const> = (120 * iTexScale) - (iHeight / 2) - iBorder;
  local iBorderX2<const> = iBorder * 2;
  local iXPlus1<const>, iYPlus1<const> = iX + iBorder, iY + iBorder;
  local iXBack<const> = iX + iWidth + iBorderX2
  local iYBack<const> = iY + iHeight + iBorderX2;
  local iXBack2<const> = iX + iWidth + iBorder;
  local iYBack2<const> = iY + iHeight + (iBorderX2 - iBorder);
  local iXText<const> = iX + iWidth + iBorderX2;
  local iYText<const> = iY - (3 * iTexScale);
  -- Last percentage
  local nLastPercentage = -1;
  -- Loader display function
  local function LoaderProc()
    -- Get current progress and return if progress hasn't changed
    local nPercent<const>, sFile<const> = fcbProgress();
    if nPercent == nLastPercentage then return end;
    nLastPercentage = nPercent;
    -- Draw progress bar
    fSolid:SetCRGBA(1, 0, 0, 1);        -- Border
    fSolid:BlitLTRB(iX, iY, iXBack, iYBack);
    fSolid:SetCRGBA(0.25, 0, 0, 1);     -- Backdrop
    fSolid:BlitLTRB(iXPlus1, iYPlus1, iXBack2, iYBack2);
    fSolid:SetCRGBA(1, 1, 1, 1);        -- Progress
    fSolid:BlitLTRB(iXPlus1, iYPlus1, iXPlus1+(nPercent*iWidth), iYBack2);
    fFont:SetSize(iTexScale);
    fFont:SetCRGBA(1, 1, 1, 1);         -- Filename & percentage
    fFont:PrintU(iX, iYText, sFile);
    fFont:PrintUR(iXText, iYText, format("%.f%% Completed", nPercent * 100));
    -- Catchup accumulator (we don't care about it);
    CoreCatchup();
    -- Draw screen at end of LUA tick
    FboDraw();
  end
  -- Set new tick function
  fcbTick = LoaderProc;
end
-- Main callback with smart error handling --------------------------------- --
local function MainProc()
  -- Protected call so we can handle errors
  local bResult<const>, sReason<const> = xpcall(fcbTick, CoreStack);
  if not bResult then SetErrorMessage(sReason) end;
end
-- Backup function for error handler --------------------------------------- --
MainProcFunc = MainProc;
-- This will be the main entry procedure ----------------------------------- --
CoreOnTick(MainProc);
-- End-of-File ============================================================= --
