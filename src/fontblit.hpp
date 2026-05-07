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
void HandleReturn(UtfDecoder &udRef, GLfloat &glfX, GLfloat &glfY,
  const GLfloat glfI)
{ // Go down own line and set indentation
  glfX = glfI;
  glfY += GetLineSpacingHeight();
  // Discard further spaces and return string minus one space
  udRef.UtfIgnore(' ');
}
/* -- Handle print control character --------------------------------------- */
void HandlePrintControl(GLfloat &glfX, GLfloat &glfY, UtfDecoder &udRef,
  const bool bSimulation, const bool bReverse = false,
  const bool bClip = false, const GLfloat glfXO = 0.0f,
  const GLfloat glfL = 0.0f, const GLfloat glfR = 0.0f)
{ // Set logic to handler
  FormatPrintControl(udRef, bSimulation,
    [this](const unsigned uCol) { FboItemSetQuadRGBInt(uCol); },
    [this]() { FboItemPushQuadColour(); },
    [this]() { FboItemPopQuadColour(); },
    [this](const unsigned uCol) { fiOutline.FboItemSetQuadRGBInt(uCol); },
    [this]() { fiOutline.FboItemPushQuadColour(); },
    [this]() { fiOutline.FboItemPopQuadColour(); },
    [this, &glfX, &glfY, &udRef, bSimulation, bReverse, bClip, glfXO, glfL,
      glfR](const unsigned uGlyph)
  { // Return if no glyph handle or glyph number invalid
    if(!tGlyphs || uGlyph >= tGlyphs->GetTileCount()) return;
    // to wrap and scan the string again
    if(!bClip && glfL != 0.0f && glfX + glfGlyphSize >= glfL)
    { // Go down own line and set indentation
      glfX = glfXO + glfR;
      glfY += GetLineSpacingHeight();
      // Print the tile
      if(!bSimulation)
        tGlyphs->BlitLTRB(0, uGlyph, glfX, glfY+glfGPadScaled,
          glfX + glfGlyphSize, glfY+glfGlyphSize+glfGPadScaled);
      // Discard further spaces
      udRef.UtfIgnore(' ');
      // Go forward (or backword). Ignore the width because we want to
      // make it a perfect square.
      glfX = bReverse ? glfX - glfGlyphSize - GetCharSpacingScale() :
                        glfX + glfGlyphSize + GetCharSpacingScale();
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
            tGlyphs->BlitLTRBS(0, uGlyph, glfX, glfY + glfGPadScaled,
              glfX + glfGlyphSize, glfY + glfGlyphSize + glfGPadScaled,
              ((glfX + glfGlyphSize) - (glfL + glfXO)) / glfGlyphSize, 1);
        } // Clipping right margin?
        else if(glfX + glfGlyphSize >= glfXO + glfR)
        { // Really clipping right margin. Blit character.
          if(glfX + glfGlyphSize < glfXO + glfR + glfGlyphSize)
            tGlyphs->BlitLTRBS(0, uGlyph, glfX, glfY + glfGPadScaled,
              glfX + glfGlyphSize, glfY + glfGlyphSize + glfGPadScaled,
              1, 1 - (((glfX + glfGlyphSize) - (glfR + glfXO)) /
                glfGlyphSize));
        } // Draw character normally
        else tGlyphs->BlitLTRB(0, uGlyph, glfX, glfY + glfGPadScaled,
          glfX + glfGlyphSize, glfY+glfGlyphSize + glfGPadScaled);
      } // Draw glyph normally
      else tGlyphs->BlitLTRB(0, uGlyph, glfX, glfY + glfGPadScaled,
        glfX + glfGlyphSize, glfY + glfGlyphSize + glfGPadScaled);
    }
    // Go forward (or backword). Ignore the width because we want to
    // make it a perfect square.
    glfX = bReverse ? glfX - glfGlyphSize - GetCharSpacingScale() :
                      glfX + glfGlyphSize + GetCharSpacingScale();
  });
}
/* -- Locate a supported character while checking if word can be printed --- */
bool PrintGetWord(UtfDecoder &udRef, size_t &stPos, GLfloat glfX,
  GLfloat &glfY, const GLfloat glfW)
{ // Save position because we're not drawing anything
  const StringViewConstIt svciPtr{ udRef.UtfGetPos() };
  // Get and compare character
  NextCharacter: switch(const Codepoint coChar = udRef.UtfNext())
  { // End, carriage return or space char? Restore position and return no wrap
    case '\0': case '\n': case ' ':
      udRef.UtfSetPos(svciPtr); return false;
    // Other control character? Handle print control characters
    case '\r': HandlePrintControl(glfX, glfY, udRef, true); break;
    // Normal character
    default:
      // Get character
      stPos = CheckGlyph(coChar);
      // Get character width
      const GLfloat glfWidth = GetCharWidth(stPos);
      // Printing next character would exceed wrap width?
      if(glfWidth + glfX >= glfW)
      { // Restore position
        udRef.UtfSetPos(svciPtr);
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
void PrintDraw(GLfloat &glfX, const GLfloat glfY, const size_t stPos)
{ // Blit outline character?
  if(ftfData.IsStrokerLoaded())
  { // Get outline character info and print the outline glyph
    const Glyph &gOData = gvData[stPos + 1];
    BlitLTRBC(0, stPos + 1,
      glfX + gOData.CoordsGetX1() * glfScale,
      glfY + gOData.CoordsGetY1() * glfScale,
      glfX + gOData.CoordsGetX2() * glfScale,
      glfY + gOData.CoordsGetY2() * glfScale,
      fiOutline.FboItemGetCData());
  } // Get character info and print the opaque glyph
  const Glyph &gData = gvData[stPos];
    BlitLTRB(0, stPos,
      glfX + gData.CoordsGetX1() * glfScale,
      glfY + gData.CoordsGetY1() * glfScale,
      glfX + gData.CoordsGetX2() * glfScale,
      glfY + gData.CoordsGetY2() * glfScale);
  // Increase position
  glfX += (gData.GlyphGetAdvance() + GetCharSpacing()) * glfScale;
}
/* -- Clean-up drawing flags and colour ------------------------------------ */
void CleanupProcessing()
{ // Reset background and foreground colour if set
  FormatHandleFinish([this](){ FboItemPopQuadColour(); },
                     [this](){ fiOutline.FboItemPopQuadColour(); });
}
/* -- Pop colour and reset glyphs ------------------------------------------ */
void PopQuadColourAndGlyphs()
{ // Restore glyph colours and remove glyph texture
  tGlyphs->FboItemPopQuadColour();
  tGlyphs = nullptr;
  // Restore font colours
  CleanupProcessing();
}
/* -- Push colour and glyphs ----------------------------------------------- */
void PushQuadColourAndGlyphs(Texture &tNGlyphs)
{ // Set glyphs texture and set alpha to our current alpha (dont set colour)
  tGlyphs = &tNGlyphs;
  tNGlyphs.FboItemPushQuadColour();
  tNGlyphs.FboItemSetQuadAlpha(FboItemGetCData(0)[3]);
}
/* -- Handle space character ----------------------------------------------- */
void HandleSpace(const Codepoint coChar, GLfloat &glfX, GLfloat &glfY,
  const GLfloat glfW, const GLfloat glfI, UtfDecoder &udRef)
{ // Get first character as normal
  size_t stPos = CheckGlyph(coChar);
  // Get character width
  const GLfloat glfWidth = GetCharWidth(stPos);
  // Just move the X position forward as we don't need to draw it.
  glfX += glfWidth;
  // Check if the draw length of the next word would go off the limit
  // and if either did?
  if(glfX + glfWidth >= glfW || PrintGetWord(udRef, stPos, glfX, glfY, glfW))
    HandleReturn(udRef, glfX, glfY, glfI);
}
/* -- Print a utfstring of textures, wrap at width, return height ---------- */
GLfloat DoPrintW(GLfloat glfX, GLfloat glfY, const GLfloat glfW,
  const GLfloat glfI, UtfDecoder &udRef)
{ // Record original X and Y position and maximum X position
  const GLfloat glfXO = glfX + glfI, fYO = glfY;
  // Until null character, which character?
  NextCharacter: switch(const Codepoint coChar = udRef.UtfNext())
  { // End of string? Return height
    case '\0': return (glfY - fYO) + GetLineSpacingHeight();
    // Carriage return?
    case '\n': HandleReturn(udRef, glfX, glfY, glfXO); break;
    // Other control character? Handle print control characters
    case '\r': HandlePrintControl(glfX, glfY, udRef, false, false, false,
                 glfXO, glfW, glfI);
               break;
    // Whitespace character?
    case ' ': HandleSpace(coChar, glfX, glfY, glfW, glfXO, udRef); break;
    // Normal character?
    default:
    { // Character storage
      const size_t stPos = CheckGlyph(coChar);
      // Printing next character would exceed wrap width? Handle return
      if(glfX + GetCharWidth(stPos) >= glfW)
        HandleReturn(udRef, glfX, glfY, glfXO);
      // Print the character regardless of wrap
      PrintDraw(glfX, glfY, stPos);
      // Done
      break;
    }
  } // Enumerate to next character again if we can
  goto NextCharacter;
}
/* -- Simluate printing a wrapped utfstring and return height -------------- */
GLfloat DoPrintWS(const GLfloat glfW, const GLfloat glfI, UtfDecoder &udRef)
{ // Record original X and Y position and maximum X position
  GLfloat glfX = 0.0f, glfY = GetLineSpacingHeight();
  // Until null character, which character?
  NextCharacter: switch(const Codepoint coChar = udRef.UtfNext())
  { // End of string? Return height
    case '\0': return glfY;
    // Carriage return?
    case '\n': HandleReturn(udRef, glfX, glfY, glfI); break;
    // Other control character?
    case '\r':
    { // Handle print control characters
      HandlePrintControl(glfX, glfY, udRef, true, false, false, 0, glfW, glfI);
      // Done
      break;
    } // Whitespace character?
    case ' ': HandleSpace(coChar, glfX, glfY, glfW, glfI, udRef); break;
    // Normal character?
    default:
    { // Get width
      const GLfloat glfWidth = GetCharWidth(CheckGlyph(coChar));
      // Printing next character would exceed wrap width? Wrap and indent
      if(glfWidth + glfX >= glfW) HandleReturn(udRef, glfX, glfY, glfI);
      // No exceed, move X position forward
      glfX += glfWidth;
      // Done
      break;
    }
  } // Enumerate to next character again if we can
  goto NextCharacter;
}
/* -- Print a utf string --------------------------------------------------- */
void DoPrint(GLfloat glfX, GLfloat glfY, const GLfloat glfXO,
  UtfDecoder &udRef)
{ // Until null character. Do printing of characters
  NextCharacter: switch(const Codepoint coChar = udRef.UtfNext())
  { // End of string? Done
    case '\0': return;
    // Carriage return?
    case '\n': HandleReturn(udRef, glfX, glfY, glfXO); break;
    // Other control character? Handle print control characters
    case '\r': HandlePrintControl(glfX, glfY, udRef, false); break;
    // Normal character? Print character
    default: PrintDraw(glfX, glfY, CheckGlyph(coChar)); break;
  } // Enumerate to next character again if we can
  goto NextCharacter;
}
/* -- Print a utfstring of textures with right align ----------------------- */
void DoPrintR(GLfloat glfX, GLfloat glfY, UtfDecoder &udRef)
{ // Record original X position
  const GLfloat glfXO = glfX;
  // Save position
  StringViewConstIt svciSavedIt{ udRef.UtfGetPos() };
  // The first character is already assumed as valid
  NextCharacter: switch(Codepoint coChar = udRef.UtfNext())
  { // Carriage return?
    case '\n':
    { // Slice the string we want to draw and draw it
      UtfDecoder udSliced{ udRef.UtfSlice(svciSavedIt) };
      DoPrint(glfX, glfY, glfXO, udSliced);
      // Set new string and save position on next line
      HandleReturn(udRef, glfX, glfY, glfXO);
      svciSavedIt = udRef.UtfGetPos();
      // Done
      break;
    } // Other control character?
    case '\r':
    { // Handle print control characters
      HandlePrintControl(glfX, glfY, udRef, true, true);
      // Done
      break;
    } // End of string or invalid unicode character
    case '\0':
    { // Slice the string we want to draw and draw it
      UtfDecoder udSliced{ udRef.UtfSlice(svciSavedIt) };
      DoPrint(glfX, glfY, glfXO, udSliced);
      // Handle the return character
      return HandleReturn(udRef, glfX, glfY, glfXO);
    } // Normal character? Get width and go backwards
    default: glfX -= GetCharWidth(CheckGlyph(coChar)); break;
  } // Enumerate to next character again if we can
  goto NextCharacter;
}
/* -- Print a utfstring with centre alignment ------------------------------ */
void DoPrintC(GLfloat glfX, GLfloat glfY, UtfDecoder &udRef)
{ // Adjust X position with spacing so it doesnt affect the centre position
  if(GetCharSpacing() != 0.0f) glfX += GetCharSpacingScale() / 2.0f;
  // Record original X position.
  const GLfloat glfXO = glfX;
  // Save position
  StringViewConstIt svciSavedIt = udRef.UtfGetPos();
  // Until null character move the X axis
  NextCharacter: switch(Codepoint coChar = udRef.UtfNext())
  { // Carriage return?
    case '\n':
    { // Slice the string we want to draw and draw it
      UtfDecoder udSliced{ udRef.UtfSlice(svciSavedIt) };
      DoPrint(glfX, glfY, glfXO, udSliced);
      // Set new string and save position on next line
      HandleReturn(udRef, glfX, glfY, glfXO);
      svciSavedIt = udRef.UtfGetPos();
      // Done
      break;
    } // Other control character?
    case '\r':
    { // To store with of drawing if needed
      GLfloat fXW = 0.0f;
      // Handle print control characters
      HandlePrintControl(fXW, glfY, udRef, true);
      // Go back if needed
      if(fXW != 0.0f) glfX -= fXW / 2.0f;
      // Done
      break;
    } // End of string or invalid unicode character
    case '\0':
    { // Slice the string we want to draw and draw it
      UtfDecoder udSliced{ udRef.UtfSlice(svciSavedIt) };
      DoPrint(glfX, glfY, glfXO, udSliced);
      // Set new string
      return HandleReturn(udRef, glfX, glfY, glfXO);
    } // Normal character? Get width and go backwards
    default: glfX -= GetCharWidth(CheckGlyph(coChar)) / 2.0f; break;
  } // Enumerate to next character again if we can
  goto NextCharacter;
}
/* -- Simulate printing a utfstring ---------------------------------------- */
GLfloat DoPrintS(UtfDecoder &udRef)
{ // Width and highest width
  GLfloat glfW = 0.0f, fWH = 0.0f;
  // Increase width until end of string
  NextCharacter: switch(const Codepoint coChar = udRef.UtfNext())
  { // End of string? Return width
    case '\0': return glfW > fWH ? glfW : fWH;
    // Carriage return? Set highest width and reset width
    case '\n': if(glfW > fWH) fWH = glfW; glfW = 0.0f; break;
    // Other control character? Handle the character
    case '\r': HandlePrintControl(glfW, glfW, udRef, true); break;
    // Normal character? Go forth!
    default: glfW += GetCharWidth(CheckGlyph(coChar)); break;
  } // Enumerate to next character again if we can
  goto NextCharacter;
}
/* -- Simulate printing a utfstring and returning the height --------------- */
GLfloat DoPrintSU(UtfDecoder &udRef)
{ // Width and highest width
  GLfloat glfX = 0.0f, glfY;
  // We're going to print something
  glfY = GetLineSpacingHeight();
  // Increase width until end of string
  NextCharacter: switch(const Codepoint coChar = udRef.UtfNext())
  { // End of string? Return height
    case '\0': return glfY;
    // Carriage return? Handle it
    case '\n': HandleReturn(udRef, glfX, glfY, 0.0f); break;
    // Other control character? Handle it
    case '\r': HandlePrintControl(glfX, glfY, udRef, true); break;
    // Normal character? Go forward
    default: glfX += GetCharWidth(CheckGlyph(coChar)); break;
  } // Enumerate to next character again if we can
  goto NextCharacter;
}
/* -- Draw a partial glyph ------------------------------------------------- */
bool DrawPartialGlyph(const bool bMove, const size_t stPos,
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
      BlitLTRBSC(0, stPos, fXMin, fYMin, fXMax, fYMax,
        (fXMax - (glfL + glfXO)) / glfW, 1, qidClrRef);
  } // Clipping right margin?
  else if(fXMax > glfXO + glfR)
  { // Really clipping left margin? Draw partial glyph with left clipping
    if(fXMax <= glfXO + glfR + glfW)
      BlitLTRBSC(0, stPos, fXMin, fYMin, fXMax, fYMax,
        1, 1 - ((fXMax - (glfR + glfXO)) / glfW), qidClrRef);
    // Do not process anymore characters
    return true;
  } // Draw character normally
  else BlitLTRBC(0, stPos, fXMin, fYMin, fXMax, fYMax, qidClrRef);
  // Increase position if we're to move the pen
  if(bMove) glfX += (gData.GlyphGetAdvance() + GetCharSpacing()) * glfScale;
  // Not finished printing characters
  return false;
}
/* -- Print a utfstring of textures ---------------------------------------- */
void DoPrintM(GLfloat glfX, GLfloat glfY, const GLfloat glfL,
  const GLfloat glfR, UtfDecoder &udRef)
{ // Parameters:-
  // > glfX = The X position of where to draw the string.
  // > glfY = The Y position of where to draw the string.
  // > glfL = The amount to scroll leftwards by in pixels.
  // > glfR = The width of the string to print.
  // Record original X position
  const GLfloat glfXO = glfX;
  // Until null character. Do printing of characters
  NextCharacter: switch(const Codepoint coChar = udRef.UtfNext())
  { // End of string? We're done
    case '\0': break;
    // Carriage return? Set new co-ordinates.
    case '\n': HandleReturn(udRef, glfX, glfY, glfXO); break;
    // Other control character?
    case '\r':
    { // Handle print control characters
      HandlePrintControl(glfX, glfY, udRef, false, false, true, glfXO, glfL,
        glfR);
      // If we went past the end (glyph drawing might have done this)
      if(glfX >= glfXO + glfR) return;
      // Done
      break;
    } // Normal character
    default:
    { // Get character
      const size_t stPos = CheckGlyph(coChar);
      // Draw outline glyph first, don't care about return status
      if(ftfData.IsStrokerLoaded())
        DrawPartialGlyph(false, stPos+1, glfXO, glfX, glfY, glfL, glfR,
          fiOutline.FboItemGetCData());
      // Draw main glyph, and if we don't need to draw anymore? Do not
      // process any more characters
      if(DrawPartialGlyph(true, stPos, glfXO, glfX, glfY, glfL, glfR,
           FboItemGetCData()))
        return;
      // Done
      break;
    }
  } // Enumerate to next character again if we can
  goto NextCharacter;
}
/* -- Check to make sure texture was loaded as a font ---------------------- */
static bool PrintSanityCheck(const UtfDecoder &udRef)
  { return !udRef.UtfValid(); }
/* -- Check to make sure texture was loaded as a font + glyphs are valid --- */
bool PrintSanityCheck(const UtfDecoder &udRef, const Texture*const tGlyph)
{ // Can't print anything if no characters stored
  if(PrintSanityCheck(udRef)) return false;
  // No glyph texture? Bail out
  if(!tGlyph) XC("Glyph handle to use is null!", "Name", NameGet());
  // Return if there are characters to process
  return true;
}
/* -- Print string of textures, wrap at width, return height ------- */ public:
template<typename StrType>
  requires StdIsPointer<StrType> || StdIsString<StrType>
GLfloat PrintW(const GLfloat glfX, const GLfloat glfY, const GLfloat glfW,
  const GLfloat glfI, const StrType &strStr)
{ // Build a new utfstring class with the string
  UtfDecoder udRef{ strStr };
  // Check that texture is a font and the string is valid
  if(PrintSanityCheck(udRef)) return 0.0f;
  // Print with word wrapping and save height
  const GLfloat fHeight = DoPrintW(glfX, glfY, glfW, glfI, udRef);
  // Cleanup enabled flags
  CleanupProcessing();
  // Check if texture needs reloading
  AtlasCheckReloadTexture();
  // Return height of printed text
  return fHeight;
}
/* -- Simluate printing a wrapped string and return height ----------------- */
template<typename StrType>
  requires StdIsPointer<StrType> || StdIsString<StrType>
GLfloat PrintWS(const GLfloat glfW, const GLfloat glfI, const StrType &strStr)
{ // Build a new utfstring class with the string
  UtfDecoder udRef{ strStr };
  // Check that texture is a font and the string is valid
  if(PrintSanityCheck(udRef)) return 0.0f;
  // Print simulated wrapping and return the height
  const GLfloat fHeight = DoPrintWS(glfW, glfI, udRef);
  // Cleanup enabled flags
  CleanupProcessing();
  // Return height of printed text
  return fHeight;
}
/* -- Print text, wrap at specified width, return height ------------------- */
template<typename StrType>
  requires StdIsPointer<StrType> || StdIsString<StrType>
GLfloat PrintWTS(const GLfloat glfW, const GLfloat glfI, const StrType &strStr,
  Texture &tNGlyphs)
{ // Build a new utfstring class with the string
  UtfDecoder udRef{ strStr };
  // Check that texture is a font and the string is valid
  if(PrintSanityCheck(udRef)) return 0.0f;
  // Save current colour and set glyph texture
  PushQuadColourAndGlyphs(tNGlyphs);
  // Print the string
  const GLfloat fHeight = DoPrintWS(glfW, glfI, udRef);
  // Restore colour and reset glyph texture
  PopQuadColourAndGlyphs();
  // Return height of printed text
  return fHeight;
}
/* -- Print text, wrap at specified width, return height ------------------- */
template<typename StrType>
  requires StdIsPointer<StrType> || StdIsString<StrType>
GLfloat PrintWU(const GLfloat glfX, const GLfloat glfY, const GLfloat glfW,
  const GLfloat glfI, const StrType &strStr)
{ // Build a new utfstring class with the string
  UtfDecoder udRef{ strStr };
  // Check that texture is a font and the string is valid
  if(PrintSanityCheck(udRef)) return 0.0f;
  // Simulate the height of the print
  const GLfloat fHeight = DoPrintWS(UtilDistance(glfX, glfW), glfI, udRef);
  // Reset the iterator on the utf string.
  udRef.UtfReset();
  // Print the string
  DoPrintW(glfX, glfY-fHeight, glfW, glfI, udRef);
  // Cleanup enabled flags
  CleanupProcessing();
  // Check if texture needs reloading
  AtlasCheckReloadTexture();
  // Return height of printed text
  return fHeight;
}
/* -- Print text upwards --------------------------------------------------- */
template<typename StrType>
  requires StdIsPointer<StrType> || StdIsString<StrType>
void PrintU(const GLfloat glfX, const GLfloat glfY, const StrType &strStr)
{ // Build a new utfstring class with the string
  UtfDecoder udRef{ strStr };
  // Check that texture is a font and the string is valid
  if(PrintSanityCheck(udRef)) return;
  // Simulate the height of the print
  const GLfloat fHeight = DoPrintSU(udRef);
  // Reset the iterator on the utf string
  udRef.UtfReset();
  // Simulate the height of the print
  DoPrint(glfX, glfY-fHeight, glfX, udRef);
  // Check if texture needs reloading
  AtlasCheckReloadTexture();
  // Cleanup enabled flags
  CleanupProcessing();
}
/* -- Print text upwards align centred ------------------------------------- */
template<typename StrType>
  requires StdIsPointer<StrType> || StdIsString<StrType>
void PrintUC(const GLfloat glfX, const GLfloat glfY, const StrType &strStr)
{ // Build a new utfstring class with the string
  UtfDecoder udRef{ strStr };
  // Check that texture is a font and the string is valid
  if(PrintSanityCheck(udRef)) return;
  // Simulate the height of the print
  const GLfloat fHeight = DoPrintSU(udRef);
  // Reset the iterator on the utf string
  udRef.UtfReset();
  // Simulate the height of the print
  DoPrintC(glfX, glfY-fHeight, udRef);
  // Check if texture needs reloading
  AtlasCheckReloadTexture();
  // Cleanup enabled flags
  CleanupProcessing();
}
/* -- Print text upwards align centred with custom texture ----------------- */
template<typename StrType>
  requires StdIsPointer<StrType> || StdIsString<StrType>
void PrintUCT(const GLfloat glfX, const GLfloat glfY, const StrType &strStr,
  Texture &tNGlyphs)
{ // Build a new utfstring class with the string
  UtfDecoder udRef{ strStr };
  // Check that texture is a font and the string is valid
  if(PrintSanityCheck(udRef)) return;
  // Save current colour and set glyph texture
  PushQuadColourAndGlyphs(tNGlyphs);
  // Simulate the height of the print
  const GLfloat fHeight = DoPrintSU(udRef);
  // Reset the iterator on the utf string
  udRef.UtfReset();
  // Print the string
  DoPrintC(glfX, glfY-fHeight, udRef);
  // Restore colour and reset glyph texture
  PopQuadColourAndGlyphs();
  // Check if texture needs reloading
  AtlasCheckReloadTexture();
}
/* -- Print text upwards align right --------------------------------------- */
template<typename StrType>
  requires StdIsPointer<StrType> || StdIsString<StrType>
void PrintUR(const GLfloat glfX, const GLfloat glfY, const StrType &strStr)
{ // Build a new utfstring class with the string
  UtfDecoder udRef{ strStr };
  // Check that texture is a font and the string is valid
  if(PrintSanityCheck(udRef)) return;
  // Simulate the height of the print
  const GLfloat fHeight = DoPrintSU(udRef);
  // Reset the iterator on the utf string
  udRef.UtfReset();
  // Simulate the height of the print
  DoPrintR(glfX, glfY-fHeight, udRef);
  // Check if texture needs reloading
  AtlasCheckReloadTexture();
  // Cleanup enabled flags
  CleanupProcessing();
}
/* -- Print text upwards align right with custom texture ------------------- */
template<typename StrType>
  requires StdIsPointer<StrType> || StdIsString<StrType>
void PrintURT(const GLfloat glfX, const GLfloat glfY, const StrType &strStr,
  Texture &tNGlyphs)
{ // Build a new utfstring class with the string
  UtfDecoder udRef{ strStr };
  // Check that texture is a font and the string is valid
  if(PrintSanityCheck(udRef)) return;
  // Save current colour and set glyph texture
  PushQuadColourAndGlyphs(tNGlyphs);
  // Simulate the height of the print
  const GLfloat fHeight = DoPrintSU(udRef);
  // Reset the iterator on the utf string
  udRef.UtfReset();
  // Print the string
  DoPrintR(glfX, glfY-fHeight, udRef);
  // Restore colour and reset glyph texture
  PopQuadColourAndGlyphs();
  // Check if texture needs reloading
  AtlasCheckReloadTexture();
}
/* -- Print text, wrap at specified width, return height ------------------- */
template<typename StrType>
  requires StdIsPointer<StrType> || StdIsString<StrType>
GLfloat PrintWUT(const GLfloat glfX, const GLfloat glfY, const GLfloat glfW,
  const GLfloat glfI, const StrType &strStr, Texture &tNGlyphs)
{ // Build a new utfstring class with the string
  UtfDecoder udRef{ strStr };
  // Check that texture is a font and the string is valid
  if(PrintSanityCheck(udRef)) return 0.0f;
  // Save current colour and set glyph texture
  PushQuadColourAndGlyphs(tNGlyphs);
  // Simulate the height of the print
  const GLfloat fHeight = DoPrintWS(UtilDistance(glfX, glfW), glfI, udRef);
  // Reset the iterator on the utf string
  udRef.UtfReset();
  // Now actually print
  DoPrintW(glfX, glfY - fHeight, glfW, glfI, udRef);
  // Restore colour and reset glyph texture
  PopQuadColourAndGlyphs();
  // Check if texture needs reloading
  AtlasCheckReloadTexture();
  // Return height
  return fHeight;
}
/* -- Print text, wrap at specified width, return height ------------------- */
template<typename StrType>
  requires StdIsPointer<StrType> || StdIsString<StrType>
GLfloat PrintWT(const GLfloat glfX, const GLfloat glfY, const GLfloat glfW,
  const GLfloat glfI, const StrType &strStr, Texture &tNGlyphs)
{ // Build a new utfstring class with the string
  UtfDecoder udRef{ strStr };
  // Check that texture is a font and the string is valid
  if(PrintSanityCheck(udRef)) return 0.0f;
  // Save current colour and set glyph texture
  PushQuadColourAndGlyphs(tNGlyphs);
  // Print the string
  const GLfloat fHeight = DoPrintW(glfX, glfY, glfW, glfI, udRef);
  // Restore colour and reset glyph texture
  PopQuadColourAndGlyphs();
  // Check if texture needs reloading
  AtlasCheckReloadTexture();
  // Return height
  return fHeight;
}
/* -- Print text ----------------------------------------------------------- */
template<typename StrType>
  requires StdIsPointer<StrType> || StdIsString<StrType>
void PrintT(const GLfloat glfX, const GLfloat glfY, const StrType &strStr,
  Texture &tNGlyphs)
{ // Build a new utfstring class with the string
  UtfDecoder udRef{ strStr };
  // Check that texture is a font and the string is valid
  if(PrintSanityCheck(udRef)) return;
  // Save current colour and set glyph texture
  PushQuadColourAndGlyphs(tNGlyphs);
  // Print the string
  DoPrint(glfX, glfY, glfX, udRef);
  // Restore colour and reset glyph texture
  PopQuadColourAndGlyphs();
  // Check if texture needs reloading
  AtlasCheckReloadTexture();
}
/* -- Print text with right align ------------------------------------------ */
template<typename StrType>
  requires StdIsPointer<StrType> || StdIsString<StrType>
void PrintRT(const GLfloat glfX, const GLfloat glfY, const StrType &strStr,
  Texture &tNGlyphs)
{ // Build a new utfstring class with the string
  UtfDecoder udRef{ strStr };
  // Check that texture is a font and the string is valid
  if(PrintSanityCheck(udRef)) return;
  // Save current colour and set glyph texture
  PushQuadColourAndGlyphs(tNGlyphs);
  // Print the string
  DoPrintR(glfX, glfY, udRef);
  // Restore colour and reset glyph texture
  PopQuadColourAndGlyphs();
  // Check if texture needs reloading
  AtlasCheckReloadTexture();
}
/* -- Print a string ------------------------------------------------------- */
template<typename StrType>
  requires StdIsPointer<StrType> || StdIsString<StrType>
void Print(const GLfloat glfX, const GLfloat glfY, const StrType &strStr)
{ // Build a new utfstring class with the string
  UtfDecoder udRef{ strStr };
  // Check that texture is a font and the string is valid
  if(PrintSanityCheck(udRef)) return;
  // Print the utf string
  DoPrint(glfX, glfY, glfX, udRef);
  // Cleanup enabled flags
  CleanupProcessing();
  // Check if texture needs reloading
  AtlasCheckReloadTexture();
}
/* -- Print text with right align ------------------------------------------ */
template<typename StrType>
  requires StdIsPointer<StrType> || StdIsString<StrType>
void PrintR(const GLfloat glfX, const GLfloat glfY, const StrType &strStr)
{ // Build a new utfstring class with the string
  UtfDecoder udRef{ strStr };
  // Check that texture is a font and the string is valid
  if(PrintSanityCheck(udRef)) return;
  // Print the string
  DoPrintR(glfX, glfY, udRef);
  // Cleanup enabled flags
  CleanupProcessing();
  // Check if texture needs reloading
  AtlasCheckReloadTexture();
}
/* -- Print a string with centre alignment --------------------------------- */
template<typename StrType>
  requires StdIsPointer<StrType> || StdIsString<StrType>
void PrintC(const GLfloat glfX, const GLfloat glfY, const StrType &strStr)
{ // Build a new utfstring class with the string
  UtfDecoder udRef{ strStr };
  // Check that texture is a font and the string is valid
  if(PrintSanityCheck(udRef)) return;
  // Print the utfstring
  DoPrintC(glfX, glfY, udRef);
  // Restore colour
  CleanupProcessing();
  // Check if texture needs reloading
  AtlasCheckReloadTexture();
}
/* -- Print text ----------------------------------------------------------- */
template<typename StrType>
  requires StdIsPointer<StrType> || StdIsString<StrType>
void PrintCT(const GLfloat glfX, const GLfloat glfY, const StrType &strStr,
  Texture &tNGlyphs)
{ // Build a new utfstring class with the string
  UtfDecoder udRef{ strStr };
  // Check that texture is a font and the string is valid
  if(PrintSanityCheck(udRef)) return;
  // Save current colour and set glyph texture
  PushQuadColourAndGlyphs(tNGlyphs);
  // Print the string
  DoPrintC(glfX, glfY, udRef);
  // Restore colour and reset glyph texture
  PopQuadColourAndGlyphs();
  // Check if texture needs reloading
  AtlasCheckReloadTexture();
}
/* -- Print text with glyphs ----------------------------------------------- */
template<typename StrType>
  requires StdIsPointer<StrType> || StdIsString<StrType>
void PrintMT(const GLfloat glfX, const GLfloat glfY, const GLfloat glfL,
  const GLfloat glfR, const StrType &strStr, Texture &tNGlyphs)
{ // Build a new utfstring class with the string
  UtfDecoder udRef{ strStr };
  // Check that texture is a font and the string is valid
  if(PrintSanityCheck(udRef)) return;
  // Save current colour and set glyph texture
  PushQuadColourAndGlyphs(tNGlyphs);
  // Print the string
  DoPrintM(glfX, glfY, glfL, glfR, udRef);
  // Restore colour and reset glyph texture
  PopQuadColourAndGlyphs();
  // Check if texture needs reloading
  AtlasCheckReloadTexture();
}
/* -- Simulate printing a string and returning the height ------------------ */
template<typename StrType>
  requires StdIsPointer<StrType> || StdIsString<StrType>
GLfloat PrintSU(const StrType &strStr)
{ // Build a new utfstring class with the string
  UtfDecoder udRef{ strStr };
  // Check that texture is a font and the string is valid
  if(PrintSanityCheck(udRef)) return 0.0f;
  // Simulate the height
  const GLfloat fHeight = DoPrintSU(udRef);
  // Cleanup enabled flags
  CleanupProcessing();
  // Return highest height
  return fHeight;
}
/* -- Simulate printing a string ------------------------------------------- */
template<typename StrType>
  requires StdIsPointer<StrType> || StdIsString<StrType>
GLfloat PrintS(const StrType &strStr)
{ // Build a new utfstring class with the string
  UtfDecoder udRef{ strStr };
  // Check that texture is a font and the string is valid
  if(PrintSanityCheck(udRef)) return 0.0f;
  // Print simulate of the utf string
  const GLfloat glfWidth = DoPrintS(udRef);
  // Cleanup enabled flags
  CleanupProcessing();
  // Return highest width or width
  return glfWidth;
}
/* -- Simulate a string of textures with glyphs ---------------------------- */
template<typename StrType>
  requires StdIsPointer<StrType> || StdIsString<StrType>
GLfloat PrintTS(const StrType &strStr, Texture &tNGlyphs)
{ // Build a new utfstring class with the string
  UtfDecoder udRef{ strStr };
  // Check that texture is a font and the string is valid
  if(PrintSanityCheck(udRef)) return 0;
  // Save current colour and set glyph texture
  PushQuadColourAndGlyphs(tNGlyphs);
  // Print the string
  const GLfloat glfWidth = DoPrintS(udRef);
  // Restore colour and reset glyph texture
  PopQuadColourAndGlyphs();
  // Return highest width or width
  return glfWidth;
}
/* -- Print text ----------------------------------------------------------- */
template<typename StrType>
  requires StdIsPointer<StrType> || StdIsString<StrType>
void PrintM(const GLfloat glfX, const GLfloat glfY, const GLfloat glfL,
  const GLfloat glfR, const StrType &strStr)
{ // Build a new utfstring class with the string
  UtfDecoder udRef{ strStr };
  // Check that texture is a font and the string is valid
  if(PrintSanityCheck(udRef)) return;
  // Do the print
  DoPrintM(glfX, glfY, glfL, glfR, udRef);
  // Cleanup enabled flags
  CleanupProcessing();
  // Check if texture needs reloading
  AtlasCheckReloadTexture();
}
/* == EoF =========================================================== EoF == */
