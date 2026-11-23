/* == FONTBLIT.HPP ========================================================= **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## This file is included as part of the Font class from font.hpp and   ## **
** ## cotains functions related to drawing strings and glyphs.            ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* -- Handle return on print ----------------------------------------------- */
void FontHandleReturn(UtfDecoder &udStr, GLfloat &glfX, GLfloat &glfY,
  const GLfloat glfI)
{ // Go down own line and set indentation
  glfX = glfI;
  glfY += FontGetLineSpacingHeight();
  // Discard further spaces and return string minus one space
  udStr.UtfIgnore(' ');
}
/* -- Handle print control character --------------------------------------- */
void FontHandlePrintControl(GLfloat &glfX, GLfloat &glfY, UtfDecoder &udStr,
  const bool bSimulation, const bool bReverse = false,
  const bool bClip = false, const GLfloat glfXO = 0.0f,
  const GLfloat glfL = 0.0f, const GLfloat glfR = 0.0f)
{ // Set logic to handler
  FormatPrintControl(udStr, bSimulation,
    [this](const unsigned uCol) { FboItemSetQuadRGBInt(uCol); },
    [this]() { FboItemPushQuadColour(); },
    [this]() { FboItemPopQuadColour(); },
    [this](const unsigned uCol) { fiOutline.FboItemSetQuadRGBInt(uCol); },
    [this]() { fiOutline.FboItemPushQuadColour(); },
    [this]() { fiOutline.FboItemPopQuadColour(); },
    [this, &glfX, &glfY, &udStr, bSimulation, bReverse, bClip, glfXO, glfL,
      glfR](const unsigned uGlyph)
  { // Return if no glyph handle or glyph number invalid
    if(!tGlyphs || uGlyph >= tGlyphs->TextureTileGetCount()) return;
    // to wrap and scan the string again
    if(!bClip && glfL != 0.0f && glfX + glfGlyphSize >= glfL)
    { // Go down own line and set indentation
      glfX = glfXO + glfR;
      glfY += FontGetLineSpacingHeight();
      // Print the tile
      if(!bSimulation)
        tGlyphs->TextureBlitLTRB(0, uGlyph, glfX, glfY+glfGPadScaled,
          glfX + glfGlyphSize, glfY+glfGlyphSize+glfGPadScaled);
      // Discard further spaces
      udStr.UtfIgnore(' ');
      // Go forward (or backword). Ignore the width because we want to
      // make it a perfect square.
      glfX = bReverse ? glfX - glfGlyphSize - FontGetCharSpacingScale() :
                        glfX + glfGlyphSize + FontGetCharSpacingScale();
      // Done
      return;
    } // If not simluation?
    if(!bSimulation)
    { // Need clipping?
      if(bClip)
      { // Clipping left margin
        if(glfX - glfXO < glfL)
        { // Really clipping left margin. Blit character.
          if(glfX - glfXO >= glfL - glfGlyphSize)
            tGlyphs->TextureBlitLTRBS(0, uGlyph, glfX, glfY + glfGPadScaled,
              glfX + glfGlyphSize, glfY + glfGlyphSize + glfGPadScaled,
              ((glfX + glfGlyphSize) - (glfL + glfXO)) / glfGlyphSize, 1);
        } // Clipping right margin?
        else if(glfX + glfGlyphSize >= glfXO + glfR)
        { // Really clipping right margin. Blit character.
          if(glfX + glfGlyphSize < glfXO + glfR + glfGlyphSize)
            tGlyphs->TextureBlitLTRBS(0, uGlyph, glfX, glfY + glfGPadScaled,
              glfX + glfGlyphSize, glfY + glfGlyphSize + glfGPadScaled,
              1, 1 - (((glfX + glfGlyphSize) - (glfR + glfXO)) /
                glfGlyphSize));
        } // Draw character normally
        else tGlyphs->TextureBlitLTRB(0, uGlyph, glfX, glfY + glfGPadScaled,
          glfX + glfGlyphSize, glfY+glfGlyphSize + glfGPadScaled);
      } // Draw glyph normally
      else tGlyphs->TextureBlitLTRB(0, uGlyph, glfX, glfY + glfGPadScaled,
        glfX + glfGlyphSize, glfY + glfGlyphSize + glfGPadScaled);
    }
    // Go forward (or backword). Ignore the width because we want to
    // make it a perfect square.
    glfX = bReverse ? glfX - glfGlyphSize - FontGetCharSpacingScale() :
                      glfX + glfGlyphSize + FontGetCharSpacingScale();
  });
}
/* -- Locate a supported character while checking if word can be printed --- */
bool FontPrintGetWord(UtfDecoder &udStr, size_t &stPos, GLfloat glfX,
  GLfloat &glfY, const GLfloat glfW)
{ // Save position because we're not drawing anything
  const StringViewConstIt svciPtr{ udStr.UtfGetPos() };
  // Get and compare character
  NextCharacter: switch(const UtfCodepoint utcChar = udStr.UtfNext())
  { // End, carriage return or space char? Restore position and return no wrap
    case '\0': case '\n': case ' ':
      udStr.UtfSetPos(svciPtr); return false;
    // Other control character? Handle print control characters
    case '\r': FontHandlePrintControl(glfX, glfY, udStr, true); break;
    // Normal character
    default:
      // Get character
      stPos = FontCheckGlyph(utcChar);
      // Get character width
      const GLfloat glfWidth = FontGetCharWidth(stPos);
      // Printing next character would exceed wrap width?
      if(glfWidth + glfX >= glfW)
      { // Restore position
        udStr.UtfSetPos(svciPtr);
        // Wrap to next position
        return true;
      } // Move X position forward
      glfX += glfWidth;
      // Done
      break;
  } // Enumerate to next character again if we can
  goto NextCharacter;
}
/* -- Handle actual printing of characters --------------------------------- */
void FontPrintDraw(GLfloat &glfX, const GLfloat glfY, const size_t stPos)
{ // Blit outline character?
  if(FtfIsStrokerLoaded())
  { // Get outline character info and print the outline glyph
    const Glyph &gOData = gvData[stPos + 1];
    TextureBlitLTRBC(0, stPos + 1,
      glfX + gOData.CoordsGetX1() * glfScale,
      glfY + gOData.CoordsGetY1() * glfScale,
      glfX + gOData.CoordsGetX2() * glfScale,
      glfY + gOData.CoordsGetY2() * glfScale,
      fiOutline.FboItemGetCData());
  } // Get character info and print the opaque glyph
  const Glyph &gData = gvData[stPos];
    TextureBlitLTRB(0, stPos,
      glfX + gData.CoordsGetX1() * glfScale,
      glfY + gData.CoordsGetY1() * glfScale,
      glfX + gData.CoordsGetX2() * glfScale,
      glfY + gData.CoordsGetY2() * glfScale);
  // Increase position
  glfX += (gData.GlyphGetAdvance() + FontGetCharSpacing()) * glfScale;
}
/* -- Clean-up drawing flags and colour ------------------------------------ */
void FontCleanupProcessing()
{ // Reset background and foreground colour if set
  FormatHandleFinish([this](){ FboItemPopQuadColour(); },
                     [this](){ fiOutline.FboItemPopQuadColour(); });
}
/* -- Pop colour and reset glyphs ------------------------------------------ */
void FontPopQuadColourAndGlyphs()
{ // Restore glyph colours and remove glyph texture
  tGlyphs->FboItemPopQuadColour();
  tGlyphs = nullptr;
  // Restore font colours
  FontCleanupProcessing();
}
/* -- Push colour and glyphs ----------------------------------------------- */
void FontPushQuadColourAndGlyphs(Texture &tNGlyphs)
{ // Set glyphs texture and set alpha to our current alpha (dont set colour)
  tGlyphs = &tNGlyphs;
  tNGlyphs.FboItemPushQuadColour();
  tNGlyphs.FboItemSetQuadAlpha(FboItemGetCData(0)[3]);
}
/* -- Handle space character ----------------------------------------------- */
void FontHandleSpace(const UtfCodepoint utcChar, GLfloat &glfX, GLfloat &glfY,
  const GLfloat glfW, const GLfloat glfI, UtfDecoder &udStr)
{ // Get first character as normal
  size_t stPos = FontCheckGlyph(utcChar);
  // Get character width
  const GLfloat glfWidth = FontGetCharWidth(stPos);
  // Just move the X position forward as we don't need to draw it.
  glfX += glfWidth;
  // Check if the draw length of the next word would go off the limit
  // and if either did?
  if(glfX + glfWidth >= glfW ||
     FontPrintGetWord(udStr, stPos, glfX, glfY, glfW))
    FontHandleReturn(udStr, glfX, glfY, glfI);
}
/* -- Print a utfstring of textures, wrap at width, return height ---------- */
GLfloat FontDoPrintW(GLfloat glfX, GLfloat glfY, const GLfloat glfW,
  const GLfloat glfI, UtfDecoder &udStr)
{ // Record original X and Y position and maximum X position
  const GLfloat glfXO = glfX + glfI, fYO = glfY;
  // Until null character, which character?
  NextCharacter: switch(const UtfCodepoint utcChar = udStr.UtfNext())
  { // End of string? Return height
    case '\0': return (glfY - fYO) + FontGetLineSpacingHeight();
    // Carriage return?
    case '\n': FontHandleReturn(udStr, glfX, glfY, glfXO); break;
    // Other control character? Handle print control characters
    case '\r': FontHandlePrintControl(glfX, glfY, udStr, false, false, false,
                 glfXO, glfW, glfI);
               break;
    // Whitespace character?
    case ' ': FontHandleSpace(utcChar, glfX, glfY, glfW, glfXO, udStr); break;
    // Normal character?
    default:
    { // Character storage
      const size_t stPos = FontCheckGlyph(utcChar);
      // Printing next character would exceed wrap width? Handle return
      if(glfX + FontGetCharWidth(stPos) >= glfW)
        FontHandleReturn(udStr, glfX, glfY, glfXO);
      // Print the character regardless of wrap
      FontPrintDraw(glfX, glfY, stPos);
      // Done
      break;
    }
  } // Enumerate to next character again if we can
  goto NextCharacter;
}
/* -- Simluate printing a wrapped utfstring and return height -------------- */
GLfloat FontDoPrintWS(const GLfloat glfW, const GLfloat glfI,
  UtfDecoder &udStr)
{ // Record original X and Y position and maximum X position
  GLfloat glfX = 0.0f, glfY = FontGetLineSpacingHeight();
  // Until null character, which character?
  NextCharacter: switch(const UtfCodepoint utcChar = udStr.UtfNext())
  { // End of string? Return height
    case '\0': return glfY;
    // Carriage return?
    case '\n': FontHandleReturn(udStr, glfX, glfY, glfI); break;
    // Other control character?
    case '\r':
    { // Handle print control characters
      FontHandlePrintControl(glfX, glfY, udStr, true, false, false, 0, glfW,
        glfI);
      // Done
      break;
    } // Whitespace character?
    case ' ': FontHandleSpace(utcChar, glfX, glfY, glfW, glfI, udStr); break;
    // Normal character?
    default:
    { // Get width
      const GLfloat glfWidth = FontGetCharWidth(FontCheckGlyph(utcChar));
      // Printing next character would exceed wrap width? Wrap and indent
      if(glfWidth + glfX >= glfW) FontHandleReturn(udStr, glfX, glfY, glfI);
      // No exceed, move X position forward
      glfX += glfWidth;
      // Done
      break;
    }
  } // Enumerate to next character again if we can
  goto NextCharacter;
}
/* -- Print a utf string --------------------------------------------------- */
void FontDoPrint(GLfloat glfX, GLfloat glfY, const GLfloat glfXO,
  UtfDecoder &udStr)
{ // Until null character. Do printing of characters
  NextCharacter: switch(const UtfCodepoint utcChar = udStr.UtfNext())
  { // End of string? Done
    case '\0': return;
    // Carriage return?
    case '\n': FontHandleReturn(udStr, glfX, glfY, glfXO); break;
    // Other control character? Handle print control characters
    case '\r': FontHandlePrintControl(glfX, glfY, udStr, false); break;
    // Normal character? Print character
    default: FontPrintDraw(glfX, glfY, FontCheckGlyph(utcChar)); break;
  } // Enumerate to next character again if we can
  goto NextCharacter;
}
/* -- Print a utfstring of textures with right align ----------------------- */
void FontDoPrintR(GLfloat glfX, GLfloat glfY, UtfDecoder &udStr)
{ // Record original X position
  const GLfloat glfXO = glfX;
  // Save position
  StringViewConstIt svciSavedIt{ udStr.UtfGetPos() };
  // The first character is already assumed as valid
  NextCharacter: switch(UtfCodepoint utcChar = udStr.UtfNext())
  { // Carriage return?
    case '\n':
    { // Slice the string we want to draw and draw it
      UtfDecoder udSliced{ udStr.UtfSlice(svciSavedIt) };
      FontDoPrint(glfX, glfY, glfXO, udSliced);
      // Set new string and save position on next line
      FontHandleReturn(udStr, glfX, glfY, glfXO);
      svciSavedIt = udStr.UtfGetPos();
      // Done
      break;
    } // Other control character?
    case '\r':
    { // Handle print control characters
      FontHandlePrintControl(glfX, glfY, udStr, true, true);
      // Done
      break;
    } // End of string or invalid unicode character
    case '\0':
    { // Slice the string we want to draw and draw it
      UtfDecoder udSliced{ udStr.UtfSlice(svciSavedIt) };
      FontDoPrint(glfX, glfY, glfXO, udSliced);
      // Handle the return character
      return FontHandleReturn(udStr, glfX, glfY, glfXO);
    } // Normal character? Get width and go backwards
    default: glfX -= FontGetCharWidth(FontCheckGlyph(utcChar)); break;
  } // Enumerate to next character again if we can
  goto NextCharacter;
}
/* -- Print a utfstring with centre alignment ------------------------------ */
void FontDoPrintC(GLfloat glfX, GLfloat glfY, UtfDecoder &udStr)
{ // Adjust X position with spacing so it doesnt affect the centre position
  if(FontGetCharSpacing() != 0.0f) glfX += FontGetCharSpacingScale() / 2.0f;
  // Record original X position.
  const GLfloat glfXO = glfX;
  // Save position
  StringViewConstIt svciSavedIt = udStr.UtfGetPos();
  // Until null character move the X axis
  NextCharacter: switch(UtfCodepoint utcChar = udStr.UtfNext())
  { // Carriage return?
    case '\n':
    { // Slice the string we want to draw and draw it
      UtfDecoder udSliced{ udStr.UtfSlice(svciSavedIt) };
      FontDoPrint(glfX, glfY, glfXO, udSliced);
      // Set new string and save position on next line
      FontHandleReturn(udStr, glfX, glfY, glfXO);
      svciSavedIt = udStr.UtfGetPos();
      // Done
      break;
    } // Other control character?
    case '\r':
    { // To store with of drawing if needed
      GLfloat fXW = 0.0f;
      // Handle print control characters
      FontHandlePrintControl(fXW, glfY, udStr, true);
      // Go back if needed
      if(fXW != 0.0f) glfX -= fXW / 2.0f;
      // Done
      break;
    } // End of string or invalid unicode character
    case '\0':
    { // Slice the string we want to draw and draw it
      UtfDecoder udSliced{ udStr.UtfSlice(svciSavedIt) };
      FontDoPrint(glfX, glfY, glfXO, udSliced);
      // Set new string
      return FontHandleReturn(udStr, glfX, glfY, glfXO);
    } // Normal character? Get width and go backwards
    default: glfX -= FontGetCharWidth(FontCheckGlyph(utcChar)) / 2.0f; break;
  } // Enumerate to next character again if we can
  goto NextCharacter;
}
/* -- Simulate printing a utfstring ---------------------------------------- */
GLfloat FontDoPrintS(UtfDecoder &udStr)
{ // Width and highest width
  GLfloat glfW = 0.0f, fWH = 0.0f;
  // Increase width until end of string
  NextCharacter: switch(const UtfCodepoint utcChar = udStr.UtfNext())
  { // End of string? Return width
    case '\0': return glfW > fWH ? glfW : fWH;
    // Carriage return? Set highest width and reset width
    case '\n': if(glfW > fWH) fWH = glfW; glfW = 0.0f; break;
    // Other control character? Handle the character
    case '\r': FontHandlePrintControl(glfW, glfW, udStr, true); break;
    // Normal character? Go forth!
    default: glfW += FontGetCharWidth(FontCheckGlyph(utcChar)); break;
  } // Enumerate to next character again if we can
  goto NextCharacter;
}
/* -- Simulate printing a utfstring and returning the height --------------- */
GLfloat FontDoPrintSU(UtfDecoder &udStr)
{ // Width and highest width
  GLfloat glfX = 0.0f, glfY;
  // We're going to print something
  glfY = FontGetLineSpacingHeight();
  // Increase width until end of string
  NextCharacter: switch(const UtfCodepoint utcChar = udStr.UtfNext())
  { // End of string? Return height
    case '\0': return glfY;
    // Carriage return? Handle it
    case '\n': FontHandleReturn(udStr, glfX, glfY, 0.0f); break;
    // Other control character? Handle it
    case '\r': FontHandlePrintControl(glfX, glfY, udStr, true); break;
    // Normal character? Go forward
    default: glfX += FontGetCharWidth(FontCheckGlyph(utcChar)); break;
  } // Enumerate to next character again if we can
  goto NextCharacter;
}
/* -- Draw a partial glyph ------------------------------------------------- */
bool FontDrawPartialGlyph(const bool bMove, const size_t stPos,
  const GLfloat glfXO, GLfloat &glfX, const GLfloat glfY, const GLfloat glfL,
  const GLfloat glfR, const QuadIntData &qidClrRef)
{ // Get glyph data
  const Glyph &gData = gvData[stPos];
  // Make adjusted co-ordinates based on glyph data
  const GLfloat
    fXMin = glfX + gData.CoordsGetX1() * glfScale,
    fYMin = glfY + gData.CoordsGetY1() * glfScale,
    fXMax = glfX + gData.CoordsGetX2() * glfScale,
    fYMax = glfY + gData.CoordsGetY2() * glfScale,
    glfW = gData.DimGetWidth() * glfScale;
  // Glyph would clip left margin?
  if(fXMin - glfXO < glfL)
  { // Really clipping left margin? Draw partial glyph with left clipping
    if(fXMin - glfXO >= glfL - glfW)
      TextureBlitLTRBSC(0, stPos, fXMin, fYMin, fXMax, fYMax,
        (fXMax - (glfL + glfXO)) / glfW, 1, qidClrRef);
  } // Clipping right margin?
  else if(fXMax > glfXO + glfR)
  { // Really clipping left margin? Draw partial glyph with left clipping
    if(fXMax <= glfXO + glfR + glfW)
      TextureBlitLTRBSC(0, stPos, fXMin, fYMin, fXMax, fYMax,
        1, 1 - ((fXMax - (glfR + glfXO)) / glfW), qidClrRef);
    // Do not process anymore characters
    return true;
  } // Draw character normally
  else TextureBlitLTRBC(0, stPos, fXMin, fYMin, fXMax, fYMax, qidClrRef);
  // Increase position if we're to move the pen
  if(bMove)
    glfX += (gData.GlyphGetAdvance() + FontGetCharSpacing()) * glfScale;
  // Not finished printing characters
  return false;
}
/* -- Print a utfstring of textures ---------------------------------------- */
void FontDoPrintM(GLfloat glfX, GLfloat glfY, const GLfloat glfL,
  const GLfloat glfR, UtfDecoder &udStr)
{ // Parameters:-
  // > glfX = The X position of where to draw the string.
  // > glfY = The Y position of where to draw the string.
  // > glfL = The amount to scroll leftwards by in pixels.
  // > glfR = The width of the string to print.
  // Record original X position
  const GLfloat glfXO = glfX;
  // Until null character. Do printing of characters
  NextCharacter: switch(const UtfCodepoint utcChar = udStr.UtfNext())
  { // End of string? We're done
    case '\0': break;
    // Carriage return? Set new co-ordinates.
    case '\n': FontHandleReturn(udStr, glfX, glfY, glfXO); break;
    // Other control character?
    case '\r':
    { // Handle print control characters
      FontHandlePrintControl(glfX, glfY, udStr, false, false, true, glfXO,
        glfL, glfR);
      // If we went past the end (glyph drawing might have done this)
      if(glfX >= glfXO + glfR) return;
      // Done
      break;
    } // Normal character
    default:
    { // Get character
      const size_t stPos = FontCheckGlyph(utcChar);
      // Draw outline glyph first, don't care about return status
      if(FtfIsStrokerLoaded())
        FontDrawPartialGlyph(false, stPos+1, glfXO, glfX, glfY, glfL, glfR,
          fiOutline.FboItemGetCData());
      // Draw main glyph, and if we don't need to draw anymore? Do not
      // process any more characters
      if(FontDrawPartialGlyph(true, stPos, glfXO, glfX, glfY, glfL, glfR,
           FboItemGetCData()))
        return;
      // Done
      break;
    }
  } // Enumerate to next character again if we can
  goto NextCharacter;
}
/* -- Check to make sure texture was loaded as a font ---------------------- */
static bool FontPrintSanityCheck(const UtfDecoder &udStr)
  { return !udStr.UtfValid(); }
/* -- Check to make sure texture was loaded as a font + glyphs are valid --- */
bool FontPrintSanityCheck(const UtfDecoder &udStr, const Texture*const tGlyph)
{ // Can't print anything if no characters stored
  if(FontPrintSanityCheck(udStr)) return false;
  // No glyph texture? Bail out
  if(!tGlyph) XC("Glyph handle to use is null!", "Name", NameGet());
  // Return if there are characters to process
  return true;
}
/* -- Print string of textures, wrap at width, return height ------- */ public:
template<typename StrType>
  requires StdIsPointer<StrType> || StdIsString<StrType>
GLfloat FontPrintW(const GLfloat glfX, const GLfloat glfY, const GLfloat glfW,
  const GLfloat glfI, const StrType &strStr)
{ // Build a new utfstring class with the string
  UtfDecoder udStr{ strStr };
  // Check that texture is a font and the string is valid
  if(FontPrintSanityCheck(udStr)) return FontGetLineSpacingHeight();
  // Print with word wrapping and save height
  const GLfloat fHeight = FontDoPrintW(glfX, glfY, glfW, glfI, udStr);
  // Cleanup enabled flags
  FontCleanupProcessing();
  // Check if texture needs reloading
  AtlasCheckReloadTexture();
  // Return height of printed text
  return fHeight;
}
/* -- Simluate printing a wrapped string and return height ----------------- */
template<typename StrType>
  requires StdIsPointer<StrType> || StdIsString<StrType>
GLfloat FontPrintWS(const GLfloat glfW, const GLfloat glfI,
  const StrType &strStr)
{ // Build a new utfstring class with the string
  UtfDecoder udStr{ strStr };
  // Check that texture is a font and the string is valid
  if(FontPrintSanityCheck(udStr)) return FontGetLineSpacingHeight();
  // Print simulated wrapping and return the height
  const GLfloat fHeight = FontDoPrintWS(glfW, glfI, udStr);
  // Cleanup enabled flags
  FontCleanupProcessing();
  // Return height of printed text
  return fHeight;
}
/* -- Print text, wrap at specified width, return height ------------------- */
template<typename StrType>
  requires StdIsPointer<StrType> || StdIsString<StrType>
GLfloat FontPrintWTS(const GLfloat glfW, const GLfloat glfI,
  const StrType &strStr, Texture &tNGlyphs)
{ // Build a new utfstring class with the string
  UtfDecoder udStr{ strStr };
  // Check that texture is a font and the string is valid
  if(FontPrintSanityCheck(udStr)) return FontGetLineSpacingHeight();
  // Save current colour and set glyph texture
  FontPushQuadColourAndGlyphs(tNGlyphs);
  // Print the string
  const GLfloat fHeight = FontDoPrintWS(glfW, glfI, udStr);
  // Restore colour and reset glyph texture
  FontPopQuadColourAndGlyphs();
  // Return height of printed text
  return fHeight;
}
/* -- Print text, wrap at specified width, return height ------------------- */
template<typename StrType>
  requires StdIsPointer<StrType> || StdIsString<StrType>
GLfloat FontPrintWU(const GLfloat glfX, const GLfloat glfY, const GLfloat glfW,
  const GLfloat glfI, const StrType &strStr)
{ // Build a new utfstring class with the string
  UtfDecoder udStr{ strStr };
  // Check that texture is a font and the string is valid
  if(FontPrintSanityCheck(udStr)) return FontGetLineSpacingHeight();
  // Simulate the height of the print
  const GLfloat fHeight = FontDoPrintWS(UtilDistance(glfX, glfW), glfI, udStr);
  // Reset the iterator on the utf string.
  udStr.UtfReset();
  // Print the string
  FontDoPrintW(glfX, glfY-fHeight, glfW, glfI, udStr);
  // Cleanup enabled flags
  FontCleanupProcessing();
  // Check if texture needs reloading
  AtlasCheckReloadTexture();
  // Return height of printed text
  return fHeight;
}
/* -- Print text upwards --------------------------------------------------- */
template<typename StrType>
  requires StdIsPointer<StrType> || StdIsString<StrType>
void FontPrintU(const GLfloat glfX, const GLfloat glfY, const StrType &strStr)
{ // Build a new utfstring class with the string
  UtfDecoder udStr{ strStr };
  // Check that texture is a font and the string is valid
  if(FontPrintSanityCheck(udStr)) return;
  // Simulate the height of the print
  const GLfloat fHeight = FontDoPrintSU(udStr);
  // Reset the iterator on the utf string
  udStr.UtfReset();
  // Simulate the height of the print
  FontDoPrint(glfX, glfY-fHeight, glfX, udStr);
  // Check if texture needs reloading
  AtlasCheckReloadTexture();
  // Cleanup enabled flags
  FontCleanupProcessing();
}
/* -- Print text upwards align centred ------------------------------------- */
template<typename StrType>
  requires StdIsPointer<StrType> || StdIsString<StrType>
void FontPrintUC(const GLfloat glfX, const GLfloat glfY, const StrType &strStr)
{ // Build a new utfstring class with the string
  UtfDecoder udStr{ strStr };
  // Check that texture is a font and the string is valid
  if(FontPrintSanityCheck(udStr)) return;
  // Simulate the height of the print
  const GLfloat fHeight = FontDoPrintSU(udStr);
  // Reset the iterator on the utf string
  udStr.UtfReset();
  // Simulate the height of the print
  FontDoPrintC(glfX, glfY-fHeight, udStr);
  // Check if texture needs reloading
  AtlasCheckReloadTexture();
  // Cleanup enabled flags
  FontCleanupProcessing();
}
/* -- Print text upwards align centred with custom texture ----------------- */
template<typename StrType>
  requires StdIsPointer<StrType> || StdIsString<StrType>
void FontPrintUCT(const GLfloat glfX, const GLfloat glfY,
  const StrType &strStr, Texture &tNGlyphs)
{ // Build a new utfstring class with the string
  UtfDecoder udStr{ strStr };
  // Check that texture is a font and the string is valid
  if(FontPrintSanityCheck(udStr)) return;
  // Save current colour and set glyph texture
  FontPushQuadColourAndGlyphs(tNGlyphs);
  // Simulate the height of the print
  const GLfloat fHeight = FontDoPrintSU(udStr);
  // Reset the iterator on the utf string
  udStr.UtfReset();
  // Print the string
  FontDoPrintC(glfX, glfY-fHeight, udStr);
  // Restore colour and reset glyph texture
  FontPopQuadColourAndGlyphs();
  // Check if texture needs reloading
  AtlasCheckReloadTexture();
}
/* -- Print text upwards align right --------------------------------------- */
template<typename StrType>
  requires StdIsPointer<StrType> || StdIsString<StrType>
void FontPrintUR(const GLfloat glfX, const GLfloat glfY, const StrType &strStr)
{ // Build a new utfstring class with the string
  UtfDecoder udStr{ strStr };
  // Check that texture is a font and the string is valid
  if(FontPrintSanityCheck(udStr)) return;
  // Simulate the height of the print
  const GLfloat fHeight = FontDoPrintSU(udStr);
  // Reset the iterator on the utf string
  udStr.UtfReset();
  // Simulate the height of the print
  FontDoPrintR(glfX, glfY-fHeight, udStr);
  // Check if texture needs reloading
  AtlasCheckReloadTexture();
  // Cleanup enabled flags
  FontCleanupProcessing();
}
/* -- Print text upwards align right with custom texture ------------------- */
template<typename StrType>
  requires StdIsPointer<StrType> || StdIsString<StrType>
void FontPrintURT(const GLfloat glfX, const GLfloat glfY,
  const StrType &strStr, Texture &tNGlyphs)
{ // Build a new utfstring class with the string
  UtfDecoder udStr{ strStr };
  // Check that texture is a font and the string is valid
  if(FontPrintSanityCheck(udStr)) return;
  // Save current colour and set glyph texture
  FontPushQuadColourAndGlyphs(tNGlyphs);
  // Simulate the height of the print
  const GLfloat fHeight = FontDoPrintSU(udStr);
  // Reset the iterator on the utf string
  udStr.UtfReset();
  // Print the string
  FontDoPrintR(glfX, glfY-fHeight, udStr);
  // Restore colour and reset glyph texture
  FontPopQuadColourAndGlyphs();
  // Check if texture needs reloading
  AtlasCheckReloadTexture();
}
/* -- Print text, wrap at specified width, return height ------------------- */
template<typename StrType>
  requires StdIsPointer<StrType> || StdIsString<StrType>
GLfloat FontPrintWUT(const GLfloat glfX, const GLfloat glfY,
  const GLfloat glfW, const GLfloat glfI, const StrType &strStr,
  Texture &tNGlyphs)
{ // Build a new utfstring class with the string
  UtfDecoder udStr{ strStr };
  // Check that texture is a font and the string is valid
  if(FontPrintSanityCheck(udStr)) return FontGetLineSpacingHeight();
  // Save current colour and set glyph texture
  FontPushQuadColourAndGlyphs(tNGlyphs);
  // Simulate the height of the print
  const GLfloat fHeight = FontDoPrintWS(UtilDistance(glfX, glfW), glfI, udStr);
  // Reset the iterator on the utf string
  udStr.UtfReset();
  // Now actually print
  FontDoPrintW(glfX, glfY - fHeight, glfW, glfI, udStr);
  // Restore colour and reset glyph texture
  FontPopQuadColourAndGlyphs();
  // Check if texture needs reloading
  AtlasCheckReloadTexture();
  // Return height
  return fHeight;
}
/* -- Print text, wrap at specified width, return height ------------------- */
template<typename StrType>
  requires StdIsPointer<StrType> || StdIsString<StrType>
GLfloat FontPrintWT(const GLfloat glfX, const GLfloat glfY, const GLfloat glfW,
  const GLfloat glfI, const StrType &strStr, Texture &tNGlyphs)
{ // Build a new utfstring class with the string
  UtfDecoder udStr{ strStr };
  // Check that texture is a font and the string is valid
  if(FontPrintSanityCheck(udStr)) return FontGetLineSpacingHeight();
  // Save current colour and set glyph texture
  FontPushQuadColourAndGlyphs(tNGlyphs);
  // Print the string
  const GLfloat fHeight = FontDoPrintW(glfX, glfY, glfW, glfI, udStr);
  // Restore colour and reset glyph texture
  FontPopQuadColourAndGlyphs();
  // Check if texture needs reloading
  AtlasCheckReloadTexture();
  // Return height
  return fHeight;
}
/* -- Print text ----------------------------------------------------------- */
template<typename StrType>
  requires StdIsPointer<StrType> || StdIsString<StrType>
void FontPrintT(const GLfloat glfX, const GLfloat glfY, const StrType &strStr,
  Texture &tNGlyphs)
{ // Build a new utfstring class with the string
  UtfDecoder udStr{ strStr };
  // Check that texture is a font and the string is valid
  if(FontPrintSanityCheck(udStr)) return;
  // Save current colour and set glyph texture
  FontPushQuadColourAndGlyphs(tNGlyphs);
  // Print the string
  FontDoPrint(glfX, glfY, glfX, udStr);
  // Restore colour and reset glyph texture
  FontPopQuadColourAndGlyphs();
  // Check if texture needs reloading
  AtlasCheckReloadTexture();
}
/* -- Print text with right align ------------------------------------------ */
template<typename StrType>
  requires StdIsPointer<StrType> || StdIsString<StrType>
void FontPrintRT(const GLfloat glfX, const GLfloat glfY, const StrType &strStr,
  Texture &tNGlyphs)
{ // Build a new utfstring class with the string
  UtfDecoder udStr{ strStr };
  // Check that texture is a font and the string is valid
  if(FontPrintSanityCheck(udStr)) return;
  // Save current colour and set glyph texture
  FontPushQuadColourAndGlyphs(tNGlyphs);
  // Print the string
  FontDoPrintR(glfX, glfY, udStr);
  // Restore colour and reset glyph texture
  FontPopQuadColourAndGlyphs();
  // Check if texture needs reloading
  AtlasCheckReloadTexture();
}
/* -- Print a string ------------------------------------------------------- */
template<typename StrType>
  requires StdIsPointer<StrType> || StdIsString<StrType>
void FontPrint(const GLfloat glfX, const GLfloat glfY, const StrType &strStr)
{ // Build a new utfstring class with the string
  UtfDecoder udStr{ strStr };
  // Check that texture is a font and the string is valid
  if(FontPrintSanityCheck(udStr)) return;
  // Print the utf string
  FontDoPrint(glfX, glfY, glfX, udStr);
  // Cleanup enabled flags
  FontCleanupProcessing();
  // Check if texture needs reloading
  AtlasCheckReloadTexture();
}
/* -- Print text with right align ------------------------------------------ */
template<typename StrType>
  requires StdIsPointer<StrType> || StdIsString<StrType>
void FontPrintR(const GLfloat glfX, const GLfloat glfY, const StrType &strStr)
{ // Build a new utfstring class with the string
  UtfDecoder udStr{ strStr };
  // Check that texture is a font and the string is valid
  if(FontPrintSanityCheck(udStr)) return;
  // Print the string
  FontDoPrintR(glfX, glfY, udStr);
  // Cleanup enabled flags
  FontCleanupProcessing();
  // Check if texture needs reloading
  AtlasCheckReloadTexture();
}
/* -- Print a string with centre alignment --------------------------------- */
template<typename StrType>
  requires StdIsPointer<StrType> || StdIsString<StrType>
void FontPrintC(const GLfloat glfX, const GLfloat glfY, const StrType &strStr)
{ // Build a new utfstring class with the string
  UtfDecoder udStr{ strStr };
  // Check that texture is a font and the string is valid
  if(FontPrintSanityCheck(udStr)) return;
  // Print the utfstring
  FontDoPrintC(glfX, glfY, udStr);
  // Restore colour
  FontCleanupProcessing();
  // Check if texture needs reloading
  AtlasCheckReloadTexture();
}
/* -- Print text ----------------------------------------------------------- */
template<typename StrType>
  requires StdIsPointer<StrType> || StdIsString<StrType>
void FontPrintCT(const GLfloat glfX, const GLfloat glfY, const StrType &strStr,
  Texture &tNGlyphs)
{ // Build a new utfstring class with the string
  UtfDecoder udStr{ strStr };
  // Check that texture is a font and the string is valid
  if(FontPrintSanityCheck(udStr)) return;
  // Save current colour and set glyph texture
  FontPushQuadColourAndGlyphs(tNGlyphs);
  // Print the string
  FontDoPrintC(glfX, glfY, udStr);
  // Restore colour and reset glyph texture
  FontPopQuadColourAndGlyphs();
  // Check if texture needs reloading
  AtlasCheckReloadTexture();
}
/* -- Print text with glyphs ----------------------------------------------- */
template<typename StrType>
  requires StdIsPointer<StrType> || StdIsString<StrType>
void FontPrintMT(const GLfloat glfX, const GLfloat glfY, const GLfloat glfL,
  const GLfloat glfR, const StrType &strStr, Texture &tNGlyphs)
{ // Build a new utfstring class with the string
  UtfDecoder udStr{ strStr };
  // Check that texture is a font and the string is valid
  if(FontPrintSanityCheck(udStr)) return;
  // Save current colour and set glyph texture
  FontPushQuadColourAndGlyphs(tNGlyphs);
  // Print the string
  FontDoPrintM(glfX, glfY, glfL, glfR, udStr);
  // Restore colour and reset glyph texture
  FontPopQuadColourAndGlyphs();
  // Check if texture needs reloading
  AtlasCheckReloadTexture();
}
/* -- Simulate printing a string and returning the height ------------------ */
template<typename StrType>
  requires StdIsPointer<StrType> || StdIsString<StrType>
GLfloat FontPrintSU(const StrType &strStr)
{ // Build a new utfstring class with the string
  UtfDecoder udStr{ strStr };
  // Check that texture is a font and the string is valid
  if(FontPrintSanityCheck(udStr)) return FontGetLineSpacingHeight();
  // Simulate the height
  const GLfloat fHeight = FontDoPrintSU(udStr);
  // Cleanup enabled flags
  FontCleanupProcessing();
  // Return highest height
  return fHeight;
}
/* -- Simulate printing a string ------------------------------------------- */
template<typename StrType>
  requires StdIsPointer<StrType> || StdIsString<StrType>
GLfloat FontPrintS(const StrType &strStr)
{ // Build a new utfstring class with the string
  UtfDecoder udStr{ strStr };
  // Check that texture is a font and the string is valid
  if(FontPrintSanityCheck(udStr)) return 0.0f;
  // Print simulate of the utf string
  const GLfloat glfWidth = FontDoPrintS(udStr);
  // Cleanup enabled flags
  FontCleanupProcessing();
  // Return highest width or width
  return glfWidth;
}
/* -- Simulate a string of textures with glyphs ---------------------------- */
template<typename StrType>
  requires StdIsPointer<StrType> || StdIsString<StrType>
GLfloat FontPrintTS(const StrType &strStr, Texture &tNGlyphs)
{ // Build a new utfstring class with the string
  UtfDecoder udStr{ strStr };
  // Check that texture is a font and the string is valid
  if(FontPrintSanityCheck(udStr)) return 0;
  // Save current colour and set glyph texture
  FontPushQuadColourAndGlyphs(tNGlyphs);
  // Print the string
  const GLfloat glfWidth = FontDoPrintS(udStr);
  // Restore colour and reset glyph texture
  FontPopQuadColourAndGlyphs();
  // Return highest width or width
  return glfWidth;
}
/* -- Print text ----------------------------------------------------------- */
template<typename StrType>
  requires StdIsPointer<StrType> || StdIsString<StrType>
void FontPrintM(const GLfloat glfX, const GLfloat glfY, const GLfloat glfL,
  const GLfloat glfR, const StrType &strStr)
{ // Build a new utfstring class with the string
  UtfDecoder udStr{ strStr };
  // Check that texture is a font and the string is valid
  if(FontPrintSanityCheck(udStr)) return;
  // Do the print
  FontDoPrintM(glfX, glfY, glfL, glfR, udStr);
  // Cleanup enabled flags
  FontCleanupProcessing();
  // Check if texture needs reloading
  AtlasCheckReloadTexture();
}
/* == EoF =========================================================== EoF == */
