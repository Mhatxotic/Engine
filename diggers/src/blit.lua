-- BLIT.LUA ================================================================ --
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
-- M-Engine function aliases ----------------------------------------------- --
local FontPrint<const>, FontPrintC<const>, FontPrintCT<const>,
  FontPrintM<const>, FontPrintR<const>, FontPrintS<const>, FontPrintU<const>,
  FontPrintUR<const>, FontPrintWS<const>, TextureBlitLTRB<const>,
  TextureBlitSLTWHA<const>, TextureBlitSLTWH<const>, TextureBlitLT<const>,
  TextureBlitSLT<const>, TextureBlitSLTRB<const>, TextureTileA<const>,
  VideoSetVLTRB =
    Font.Print, Font.PrintC, Font.PrintCT, Font.PrintM, Font.PrintR,
    Font.PrintS, Font.PrintU, Font.PrintUR, Font.PrintWS, Texture.BlitLTRB,
    Texture.BlitSLTWHA, Texture.BlitSLTWH, Texture.BlitLT, Texture.BlitSLT,
    Texture.BlitSLTRB, Texture.TileA, Video.SetVLTRB;
-- Locals ------------------------------------------------------------------ --
local iTexScale;                       -- Texture scale
-- Print text left with scale ---------------------------------------------- --
local function Print(fontHandle, iX, iY, sText)
  FontPrint(fontHandle, iX * iTexScale, iY * iTexScale, sText);
end
-- Print text centre with scale -------------------------------------------- --
local function PrintC(fontHandle, iX, iY, sText)
  FontPrintC(fontHandle, iX * iTexScale, iY * iTexScale, sText);
end
-- Print text centre with texture glyphs and scale ------------------------- --
local function PrintCT(fontHandle, iX, iY, sText, texHandle)
  FontPrintCT(fontHandle, iX * iTexScale, iY * iTexScale, sText, texHandle);
end
-- Print text right with scale --------------------------------------------- --
local function PrintR(fontHandle, iX, iY, sText)
  FontPrintR(fontHandle, iX * iTexScale, iY * iTexScale, sText);
end
-- Simulate a print with maximum width ------------------------------------- --
local function PrintWS(fontHandle, iWidth, strText)
  return FontPrintWS(fontHandle, iWidth * iTexScale, 0, strText) / iTexScale;
end
-- Simulate a print -------------------------------------------------------- --
local function PrintS(fontHandle, strText)
  return FontPrintS(fontHandle, strText) / iTexScale;
end
-- Print a scrolling string ------------------------------------------------ --
local function PrintM(fontHandle, iX, iY, iScroll, iWidth, sText)
  FontPrintM(fontHandle, iX * iTexScale, iY * iTexScale,
    iScroll * iTexScale, iWidth * iTexScale, sText);
end
-- Print text upwards (left align) ----------------------------------------- --
local function PrintU(fontHandle, iX, iY, sText)
  FontPrintU(fontHandle, iX * iTexScale, iY * iTexScale, sText);
end
-- Print text upwards (right align) ---------------------------------------- --
local function PrintUR(fontHandle, iX, iY, sText)
  FontPrintUR(fontHandle, iX * iTexScale, iY * iTexScale, sText);
end
-- Blit bounds with scale -------------------------------------------------- --
local function BlitLTRB(texHandle, iX1, iY1, iX2, iY2)
  TextureBlitLTRB(texHandle, iX1 * iTexScale, iY1 * iTexScale,
                             iX2 * iTexScale, iY2 * iTexScale);
end
-- Blit tile/coords/size/angle with scale ---------------------------------- --
local function BlitSLTWHA(texHandle, iTileId, iX, iY, iW, iH, nA)
  TextureBlitSLTWHA(texHandle, iTileId, iX * iTexScale, iY * iTexScale,
                                        iW * iTexScale, iH * iTexScale, nA);
end
-- Blit tile/coords/size with scale ---------------------------------------- --
local function BlitSLTWH(texHandle, iTileId, iX, iY, iW, iH)
  TextureBlitSLTWH(texHandle, iTileId, iX * iTexScale, iY * iTexScale,
                                       iW * iTexScale, iH * iTexScale);
end
-- Blit tile/bounds with scale --------------------------------------------- --
local function BlitSLTRB(texHandle, iTileId, iX1, iY1, iX2, iY2)
  TextureBlitSLTRB(texHandle, iTileId, iX1 * iTexScale, iY1 * iTexScale,
                                       iX2 * iTexScale, iY2 * iTexScale);
end
-- Blit tile/x/y with scale ------------------------------------------------ --
local function BlitSLT(texHandle, iTileId, iX, iY)
  TextureBlitSLT(texHandle, iTileId, iX * iTexScale, iY * iTexScale);
end
-- Blit x/y with scale ----------------------------------------------------- --
local function BlitLT(texHandle, iX, iY)
  TextureBlitLT(texHandle, iX * iTexScale, iY * iTexScale);
end
-- Set vertex for video handle --------------------------------------------- --
local function SetVLTRB(vidHandle, iX1, iY1, iX2, iY2)
  VideoSetVLTRB(vidHandle, iX1 * iTexScale, iY1 * iTexScale,
                           iX2 * iTexScale, iY2 * iTexScale);
end
-- Script ready function --------------------------------------------------- --
local function OnScriptLoaded(GetAPI)
  -- Get and store texture scale
  iTexScale = GetAPI("iTexScale");
  -- Return if texture scale is set to 1
  if iTexScale <= 1 then return end;
  -- Enumerate cursor id datas
  for iId, aCData in pairs(GetAPI("aCursorData")) do
    -- Save original cursor id position adjustments
    local iX<const>, iY<const> = aCData[3], aCData[4];
    -- Backup values
    aCData[5], aCData[6] = iX, iY;
    -- Scale current values
    aCData[3] = iX * iTexScale;
    aCData[4] = iY * iTexScale;
  end
end
-- Add a tile to a texture ------------------------------------------------- --
local function TileA(texHandle, iX1, iY1, iX2, iY2)
  return TextureTileA(texHandle, iX1 * iTexScale, iY1 * iTexScale,
                                 iX2 * iTexScale, iY2 * iTexScale);
end
-- Return imports and exports ---------------------------------------------- --
return { F = OnScriptLoaded, A = { Print = Print, PrintC = PrintC,
  PrintCT = PrintCT, PrintM = PrintM, PrintR = PrintR, PrintS = PrintS,
  PrintU = PrintU, PrintUR = PrintUR, PrintWS = PrintWS, BlitLTRB = BlitLTRB,
  BlitLT = BlitLT, BlitSLT = BlitSLT, BlitSLTRB = BlitSLTRB,
  BlitSLTWH = BlitSLTWH, BlitSLTWHA = BlitSLTWHA, SetVLTRB = SetVLTRB,
  TileA = TileA } };
-- End-of-File ============================================================= --
