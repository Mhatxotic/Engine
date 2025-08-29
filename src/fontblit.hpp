/* == FONTBLIT.HPP ========================================================= **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## This file is included as part of the Font class from font.hpp and   ## **
** ## cotains functions related to drawing strings and glyphs.            ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* -- Check if a character needs initialising ------------------------------ */
size_t CheckGlyph(const size_t stChar)
{ // Apply requested outline?
  return DoCheckGlyph<HandleGlyphFunc::Auto,
    StrokerCheckFunc::Auto<InitCharFunc::NoOutline>,
    RoundCheckFunc::Auto<RoundFunc::Straight<GLfloat>>>(stChar);
}
/* -- Handle return on print ----------------------------------------------- */
void HandleReturn(UtfDecoder &utfRef, GLfloat &fX, GLfloat &fY,
  const GLfloat fI)
{ // Go down own line and set indentation
  fX = fI;
  fY += fLineSpacingHeight;
  // Discard further spaces and return string minus one space
  utfRef.Ignore(' ');
}
/* -- Get modified character width at specified position ------------------- */
GLfloat GetCharWidth(const size_t stPos)
  { return (gvData[stPos].GlyphGetAdvance() + fCharSpacing) * fScale; }
/* -- Locate a supported character while checking if word can be printed --- */
bool PrintGetWord(UtfDecoder &utfRef, size_t &stPos, float fX, float &fY,
  const float fW)
{ // Save position because we're not drawing anything
  const unsigned char*const ucpPtr = utfRef.GetCPtr();
  // Until null character. Which control token?
  while(const unsigned int uiChar = utfRef.Next()) switch(uiChar)
  { // Carriage return or space char? Restore position and return no wrap
    case '\n': case ' ': utfRef.SetCPtr(ucpPtr); return false;
    // Other control character? Handle print control characters
    case '\r': HandlePrintControl(fX, fY, utfRef, true); break;
    // Normal character
    default:
      // Get character
      stPos = CheckGlyph(uiChar);
      // Get character width
      const GLfloat fWidth = GetCharWidth(stPos);
      // Printing next character would exceed wrap width?
      if(fWidth + fX >= fW)
      { // Restore position
        utfRef.SetCPtr(ucpPtr);
        // Wrap to next position
        return true;
      } // Move X position forward
      fX += fWidth;
      // Done
      break;
  } // Restore position
  utfRef.SetCPtr(ucpPtr);
  // No wrap occured and caller should not Y adjust
  return false;
}
/* -- Handle actual printing of characters --------------------------------- */
void PrintDraw(GLfloat &fX, const GLfloat fY, const size_t stPos)
{ // Blit outline character?
  if(ftfData.IsStrokerLoaded())
  { // Get outline character info and print the outline glyph
    const Glyph &gOData = gvData[stPos+1];
    BlitLTRBC(0, stPos+1,
      fX+gOData.RectGetX1() * fScale, fY+gOData.RectGetY1() * fScale,
      fX+gOData.RectGetX2() * fScale, fY+gOData.RectGetY2() * fScale,
      fiOutline.FboItemGetCData());
  } // Get character info and print the opaque glyph
  const Glyph &gData = gvData[stPos];
    BlitLTRB(0, stPos,
      fX+gData.RectGetX1() * fScale, fY+gData.RectGetY1() * fScale,
      fX+gData.RectGetX2() * fScale, fY+gData.RectGetY2() * fScale);
  // Increase position
  fX += (gData.GlyphGetAdvance() + fCharSpacing) * fScale;
}
/* -- Pop colour and reset glyphs ------------------------------------------ */
void PopQuadColourAndGlyphs(void)
{ // Restore glyph colours and remove glyph texture
  tGlyphs->FboItemPopQuadColour();
  tGlyphs = nullptr;
  // Restore font colours
  FboItemPopQuadColour();
}
/* -- Push colour and glyphs ----------------------------------------------- */
void PushQuadColourAndGlyphs(Texture &tNGlyphs)
{ // Push tint
  FboItemPushQuadColour();
  // Set glyphs texture and set alpha to our current alpha (dont set colour)
  tGlyphs = &tNGlyphs;
  tNGlyphs.FboItemPushQuadColour();
  tNGlyphs.FboItemSetQuadAlpha(FboItemGetCData(0)[3]);
}
/* -- Handle print control character --------------------------------------- */
void HandlePrintControl(GLfloat &fX, GLfloat &fY, UtfDecoder &utfRef,
  const bool bSimulation, const bool bReverse=false, const bool bClip=false,
  const GLfloat fXO=0.0f, const GLfloat fL=0.0f, const GLfloat fR=0.0f)
{ // Get next character
  switch(utfRef.Next())
  { // Colour selection
    case 'c':
    { // Scan for the hexadecimal value and if we found it? Set Tint if not
      // simulation and we read 8 bytes
      unsigned int uiCol;
      if(utfRef.ScanValue(uiCol) == 8 && !bSimulation)
        FboItemSetQuadRGBInt(uiCol);
      // Done
      break;
    } // Outline colour selection
    case 'o':
    { // Scan for the hexadecimal value and if we found it? Set Tint if not
      // simulation and we read 8 bytes
      unsigned int uiCol;
      if(utfRef.ScanValue(uiCol) == 8 && !bSimulation)
        fiOutline.FboItemSetQuadRGBInt(uiCol);
      // Done
      break;
    } // Reset colour
    case 'r': if(!bSimulation) FboItemPopQuadColour(); break;
    // Print glyph
    case 't':
    { // Scan for the hexadecimal value and if we found it and we have
      // a glyphs texture assigned and is a valid glyph value?
      unsigned int uiGlyph;
      if(utfRef.ScanValue(uiGlyph) == 8 && tGlyphs &&
        uiGlyph < tGlyphs->GetTileCount())
      { // to wrap and scan the string again
        if(!bClip && fL != 0.0f && fX + fGSize >= fL)
        { // Go down own line and set indentation
          fX = fXO + fR;
          fY += fLineSpacingHeight;
          // Print the tile
          if(!bSimulation)
            tGlyphs->BlitLTRB(0, uiGlyph, fX, fY+fGPadScaled,
              fX + fGSize, fY+fGSize+fGPadScaled);
          // Discard further spaces
          utfRef.Ignore(' ');
          // Go forward (or backword). Ignore the width because we want to
          // make it a perfect square.
          fX = bReverse ? fX - fGSize - fCharSpacingScale :
                          fX + fGSize + fCharSpacingScale;
          // Done
          break;
        } // If not simluation?
        if(!bSimulation)
        { // Need clipping?
          if(bClip)
          { // Clipping left margin
            if(fX - fXO < fL)
            { // Really clipping left margin. Blit character.
              if(fX - fXO >= fL - fGSize)
                tGlyphs->BlitLTRBS(0, uiGlyph, fX, fY+fGPadScaled,
                  fX+fGSize, fY+fGSize+fGPadScaled,
                  ((fX+fGSize)-(fL+fXO))/fGSize, 1);
            } // Clipping right margin?
            else if(fX + fGSize >= fXO + fR)
            { // Really clipping right margin. Blit character.
              if(fX + fGSize < fXO + fR + fGSize)
                tGlyphs->BlitLTRBS(0, uiGlyph, fX, fY+fGPadScaled,
                  fX+fGSize, fY+fGSize+fGPadScaled,
                  1, 1-(((fX+fGSize)-(fR+fXO))/fGSize));
            } // Draw character normally
            else tGlyphs->BlitLTRB(0, uiGlyph, fX, fY+fGPadScaled,
              fX+fGSize, fY+fGSize+fGPadScaled);
          } // Draw glyph normally
          else tGlyphs->BlitLTRB(0, uiGlyph, fX, fY+fGPadScaled,
            fX+fGSize, fY+fGSize+fGPadScaled);
        }
        // Go forward (or backword). Ignore the width because we want to
        // make it a perfect square.
        fX = bReverse ? fX - fGSize - fCharSpacingScale :
                        fX + fGSize + fCharSpacingScale;
      } // Done
      break;
    } // Invalid control character.
    default: break;
  }
}
/* -- Handle space character ----------------------------------------------- */
void HandleSpace(const unsigned int uiChar, GLfloat &fX, GLfloat &fY,
  const GLfloat fW, const GLfloat fI, UtfDecoder &utfRef)
{ // Get first character as normal
  size_t stPos = CheckGlyph(uiChar);
  // Get character width
  const GLfloat fWidth = GetCharWidth(stPos);
  // Just move the X position forward as we don't need to draw it.
  fX += fWidth;
  // Check if the draw length of the next word would go off the limit
  // and if either did?
  if(fX + fWidth >= fW || PrintGetWord(utfRef, stPos, fX, fY, fW))
    HandleReturn(utfRef, fX, fY, fI);
}
/* -- Print a utfstring of textures, wrap at width, return height ---------- */
GLfloat DoPrintW(GLfloat fX, GLfloat fY, const GLfloat fW, const GLfloat fI,
  UtfDecoder &utfRef)
{ // Record original X and Y position and maximum X position
  const GLfloat fXO = fX + fI, fYO = fY;
  // Until null character, which character?
  while(const unsigned int uiChar = utfRef.Next()) switch(uiChar)
  { // Carriage return?
    case '\n': HandleReturn(utfRef, fX, fY, fXO); break;
    // Other control character? Handle print control characters
    case '\r': HandlePrintControl(fX, fY, utfRef, false, false, false,
                 fXO, fW, fI);
               break;
    // Whitespace character?
    case ' ': HandleSpace(uiChar, fX, fY, fW, fXO, utfRef); break;
    // Normal character?
    default:
    { // Character storage
      const size_t stPos = CheckGlyph(uiChar);
      // Printing next character would exceed wrap width? Handle return
      if(fX + GetCharWidth(stPos) >= fW) HandleReturn(utfRef, fX, fY, fXO);
      // Print the character regardless of wrap
      PrintDraw(fX, fY, stPos);
      // Done
      break;
    }
  } // Return height
  return (fY - fYO) + fLineSpacingHeight;
}
/* -- Simluate printing a wrapped utfstring and return height -------------- */
GLfloat DoPrintWS(const GLfloat fW, const GLfloat fI, UtfDecoder &utfRef)
{ // Record original X and Y position and maximum X position
  GLfloat fX = 0.0f, fY = fLineSpacingHeight;
  // Until null character, which character?
  while(const unsigned int uiChar = utfRef.Next()) switch(uiChar)
  { // Carriage return?
    case '\n': HandleReturn(utfRef, fX, fY, fI); break;
    // Other control character?
    case '\r':
    { // Handle print control characters
      HandlePrintControl(fX, fY, utfRef, true, false, false, 0, fW, fI);
      // Done
      break;
    } // Whitespace character?
    case ' ': HandleSpace(uiChar, fX, fY, fW, fI, utfRef); break;
    // Normal character?
    default:
    { // Get width
      const GLfloat fWidth = GetCharWidth(CheckGlyph(uiChar));
      // Printing next character would exceed wrap width? Wrap and indent
      if(fWidth + fX >= fW) HandleReturn(utfRef, fX, fY, fI);
      // No exceed, move X position forward
      fX += fWidth;
      // Done
      break;
    }
  } // Return height
  return fY;
}
/* -- Print a utf string --------------------------------------------------- */
void DoPrint(GLfloat fX, GLfloat fY, const GLfloat fXO, UtfDecoder &utfRef)
{ // Until null character. Do printing of characters
  while(const unsigned int uiChar = utfRef.Next()) switch(uiChar)
  { // Carriage return?
    case '\n': HandleReturn(utfRef, fX, fY, fXO); break;
    // Other control character? Handle print control characters
    case '\r': HandlePrintControl(fX, fY, utfRef, false); break;
    // Normal character? Print character
    default: PrintDraw(fX, fY, CheckGlyph(uiChar)); break;
  }
}
/* -- Print a utfstring of textures with right align ----------------------- */
void DoPrintR(GLfloat fX, GLfloat fY, UtfDecoder &utfRef)
{ // Record original X position
  const GLfloat fXO = fX;
  // Save position
  const unsigned char *cpSaved = utfRef.GetCPtr();
  // The first character is already assumed as valid
  for(unsigned int uiChar = utfRef.Next();;
                   uiChar = utfRef.Next()) switch(uiChar)
  { // Carriage return?
    case '\n':
    { // Create and prepare spliced string. *DO NOT* merge these lines
      // as the string will be freed after utfSliced() returns.
      const string strSpliced{ utfRef.Slice(cpSaved) };
      UtfDecoder utfSliced{ strSpliced };
      // Now print the string. We need to duplicate the string here, because
      // we shouldn't modify the original string, even though it would be
      // more optimal to just nullify this character instead.
      DoPrint(fX, fY, fXO, utfSliced);
      // Set new string
      HandleReturn(utfRef, fX, fY, fXO);
      // Save current position
      cpSaved = utfRef.GetCPtr();
      // Done
      break;
    } // Other control character?
    case '\r':
    { // Handle print control characters
      HandlePrintControl(fX, fY, utfRef, true, true);
      // Done
      break;
    } // End of string or invalid unicode character
    case '\0':
    { // Create spliced string
      UtfDecoder utfSliced{ cpSaved };
      // Now print the string. We need to duplicate the string here, because
      // we shouldn't modify the original string, even though it would be
      // more optimal to just nullify this character instead.
      DoPrint(fX, fY, fXO, utfSliced);
      // Handle the return character
      return HandleReturn(utfRef, fX, fY, fXO);
    } // Normal character? Get width and go backwards
    default: fX -= GetCharWidth(CheckGlyph(uiChar)); break;
  }
}
/* -- Print a utfstring with centre alignment ------------------------------ */
void DoPrintC(GLfloat fX, GLfloat fY, UtfDecoder &utfRef)
{ // Adjust X position with spacing so it doesnt affect the centre position
  if(fCharSpacing != 0.0f) fX += fCharSpacingScale / 2.0f;
  // Record original X position.
  const GLfloat fXO = fX;
  // Save position
  const unsigned char *cpSaved = utfRef.GetCPtr();
  // Until null character move the X axis
  for(unsigned int uiChar = utfRef.Next();;uiChar = utfRef.Next())
    switch(uiChar)
  { // Carriage return?
    case '\n':
    { // Create and prepare spliced string. *DO NOT* merge these lines
      // as the string will be freed after utfSliced() returns.
      const string strSliced{ utfRef.Slice(cpSaved) };
      UtfDecoder utfSliced{ strSliced };
      // Now print the string. We need to duplicate the string here, because
      // we shouldn't modify the original string, even though it would be
      // more optimal to just nullify this character instead.
      DoPrint(fX, fY, fXO, utfSliced);
      // Set new string
      HandleReturn(utfRef, fX, fY, fXO);
      // Save current position
      cpSaved = utfRef.GetCPtr();
      // Done
      break;
    } // Other control character?
    case '\r':
    { // To store with of drawing if needed
      GLfloat fXW = 0.0f;
      // Handle print control characters
      HandlePrintControl(fXW, fY, utfRef, true);
      // Go back if needed
      if(fXW != 0.0f) fX -= fXW / 2.0f;
      // Done
      break;
    } // End of string or invalid unicode character
    case '\0':
    { // Slice the utf8 string
      UtfDecoder utfSliced{ cpSaved };
      // Now print the string. We need to duplicate the string here, because
      // we shouldn't modify the original string, even though it would be
      // more optimal to just nullify this character instead.
      DoPrint(fX, fY, fXO, utfSliced);
      // Set new string
      return HandleReturn(utfRef, fX, fY, fXO);
    } // Normal character? Get width and go backwards
    default: fX -= GetCharWidth(CheckGlyph(uiChar)) / 2.0f; break;
  }
}
/* -- Simulate printing a utfstring ---------------------------------------- */
GLfloat DoPrintS(UtfDecoder &utfRef)
{ // Width and highest width
  GLfloat fW = 0.0f, fWH = 0.0f;
  // Increase width until end of string
  while(const unsigned int uiChar = utfRef.Next()) switch(uiChar)
  { // Carriage return? Set highest width and reset width
    case '\n': if(fW > fWH) fWH = fW; fW = 0.0f; break;
    // Other control character? Handle the character
    case '\r': HandlePrintControl(fW, fW, utfRef, true); break;
    // Normal character? Go forth!
    default: fW += GetCharWidth(CheckGlyph(uiChar)); break;
  } // Return width
  return fW > fWH ? fW : fWH;
}
/* -- Simulate printing a utfstring and returning the height --------------- */
GLfloat DoPrintSU(UtfDecoder &utfRef)
{ // Width and highest width
  GLfloat fX = 0.0f, fY;
  // We're going to print something
  fY = fLineSpacingHeight;
  // Increase width until end of string
  while(const unsigned int uiChar = utfRef.Next()) switch(uiChar)
  { // Carriage return? Handle it
    case '\n': HandleReturn(utfRef, fX, fY, 0.0f); break;
    // Other control character? Handle it
    case '\r': HandlePrintControl(fX, fY, utfRef, true); break;
    // Normal character? Go forward
    default: fX += GetCharWidth(CheckGlyph(uiChar)); break;
  } // Return height
  return fY;
}
/* -- Draw a partial glyph ------------------------------------------------- */
bool DrawPartialGlyph(const bool bMove, const size_t stPos,
  const GLfloat fXO, GLfloat &fX, const GLfloat fY, const GLfloat fL,
  const GLfloat fR, const QuadColData &faColPtr)
{ // Get glyph data
  const Glyph &gData = gvData[stPos];
  // Make adjusted co-ordinates based on glyph data
  const GLfloat
    fXMin = fX + gData.RectGetX1() * fScale,
    fYMin = fY + gData.RectGetY1() * fScale,
    fXMax = fX + gData.RectGetX2() * fScale,
    fYMax = fY + gData.RectGetY2() * fScale,
    fW = gData.DimGetWidth() * fScale;
  // Glyph would clip left margin?
  if(fXMin - fXO < fL)
  { // Really clipping left margin? Draw partial glyph with left clipping
    if(fXMin - fXO >= fL - fW)
      BlitLTRBSC(0, stPos, fXMin, fYMin, fXMax, fYMax,
        (fXMax - (fL + fXO)) / fW, 1, faColPtr);
  } // Clipping right margin?
  else if(fXMax > fXO + fR)
  { // Really clipping left margin? Draw partial glyph with left clipping
    if(fXMax <= fXO + fR + fW)
      BlitLTRBSC(0, stPos, fXMin, fYMin, fXMax, fYMax,
        1, 1 - ((fXMax - (fR + fXO)) / fW), faColPtr);
    // Do not process anymore characters
    return true;
  } // Draw character normally
  else BlitLTRBC(0, stPos, fXMin, fYMin, fXMax, fYMax, faColPtr);
  // Increase position if we're to move the pen
  if(bMove) fX += (gData.GlyphGetAdvance() + fCharSpacing) * fScale;
  // Not finished printing characters
  return false;
}
/* -- Print a utfstring of textures ---------------------------------------- */
void DoPrintM(GLfloat fX, GLfloat fY, const GLfloat fL, const GLfloat fR,
  UtfDecoder &utfRef)
{ // Parameters:-
  // > fX = The X position of where to draw the string.
  // > fY = The Y position of where to draw the string.
  // > fL = The amount to scroll leftwards by in pixels.
  // > fR = The width of the string to print.
  // Record original X position
  const GLfloat fXO = fX;
  // Until null character. Do printing of characters
  while(const unsigned int uiChar = utfRef.Next()) switch(uiChar)
  { // Carriage return? Set new co-ordinates.
    case '\n': HandleReturn(utfRef, fX, fY, fXO); break;
    // Other control character?
    case '\r':
    { // Handle print control characters
      HandlePrintControl(fX, fY, utfRef, false, false, true, fXO, fL, fR);
      // If we went past the end (glyph drawing might have done this)
      if(fX >= fXO + fR) return;
      // Done
      break;
    } // Normal character
    default:
    { // Get character
      const size_t stPos = CheckGlyph(uiChar);
      // Draw outline glyph first, don't care about return status
      if(ftfData.IsStrokerLoaded())
        DrawPartialGlyph(false, stPos+1, fXO, fX, fY, fL, fR,
          fiOutline.FboItemGetCData());
      // Draw main glyph, and if we don't need to draw anymore? Do not
      // process any more characters
      if(DrawPartialGlyph(true, stPos, fXO, fX, fY, fL, fR, FboItemGetCData()))
        return;
      // Done
      break;
    }
  }
}
/* -- Return text height plus line spacing --------------------------------- */
void UpdateHeightPlusLineSpacing(void)
  { fLineSpacingHeight = fLineSpacing + dfScale.DimGetHeight(); }
void UpdateCharSpacingTimesScale(void)
  { fCharSpacingScale = fCharSpacing * fScale; }
void UpdateGlyphPaddingTimesScale(void)
  { fGPadScaled = fGPad * fScale; }
/* -- Check to make sure texture was loaded as a font ---------------------- */
bool PrintSanityCheck(const UtfDecoder &utfRef)
{ // No string? Bail out
  if(!utfRef.Valid())
    XC("Null pointer assignment!", "Identifier", IdentGet());
  // Return true if there are no characters to process
  return utfRef.Finished();
}
/* -- Check to make sure texture was loaded as a font + glyphs are valid --- */
bool PrintSanityCheck(const UtfDecoder &utfRef, const Texture*const tGlyph)
{ // Can't print anything if no characters stored
  if(PrintSanityCheck(utfRef)) return false;
  // No glyph texture? Bail out
  if(!tGlyph) XC("Glyph handle to use is null!", "Identifier", IdentGet());
  // Return if there are characters to process
  return true;
}
/* -- Print string of textures, wrap at width, return height ------- */ public:
GLfloat PrintW(const GLfloat fX, const GLfloat fY, const GLfloat fW,
  const GLfloat fI, const GLubyte*const ucpStr)
{ // Build a new utfstring class with the string
  UtfDecoder utfRef{ ucpStr };
  // Check that texture is a font and the string is valid
  if(PrintSanityCheck(utfRef)) return fLineSpacingHeight;
  // Push tint
  FboItemPushQuadColour();
  // Print with width
  const GLfloat fHeight = DoPrintW(fX, fY, fW, fI, utfRef);
  // Restore colour intensity
  FboItemPopQuadColour();
  // Check if texture needs reloading
  AtlasCheckReloadTexture();
  // Return height of printed text
  return fHeight;
}
/* -- Simluate printing a wrapped string and return height ----------------- */
GLfloat PrintWS(const GLfloat fW, const GLfloat fI, const GLubyte*const ucpStr)
{ // Build a new utfstring class with the string
  UtfDecoder utfRef{ ucpStr };
  // Check that texture is a font and the string is valid
  if(PrintSanityCheck(utfRef)) return fLineSpacingHeight;
  // Push tint
  FboItemPushQuadColour();
  // Print the utf string
  const GLfloat fHeight = DoPrintWS(fW, fI, utfRef);
  // Restore colour intensity
  FboItemPopQuadColour();
  // Return height of printed text
  return fHeight;
}
/* -- Print text, wrap at specified width, return height ------------------- */
GLfloat PrintWTS(const GLfloat fW, const GLfloat fI,
  const GLubyte*const ucpStr, Texture &tNGlyphs)
{ // Build a new utfstring class with the string
  UtfDecoder utfRef{ ucpStr };
  // Check that texture is a font and the string is valid
  if(PrintSanityCheck(utfRef)) return fLineSpacingHeight;
  // Save current colour and set glyph texture
  PushQuadColourAndGlyphs(tNGlyphs);
  // Print the string
  const GLfloat fHeight = DoPrintWS(fW, fI, utfRef);
  // Restore colour and reset glyph texture
  PopQuadColourAndGlyphs();
  // Return height of printed text
  return fHeight;
}
/* -- Print text, wrap at specified width, return height ------------------- */
GLfloat PrintWU(const GLfloat fX, const GLfloat fY, const GLfloat fW,
  const GLfloat fI, const GLubyte*const ucpStr)
{ // Build a new utfstring class with the string
  UtfDecoder utfRef{ ucpStr };
  // Check that texture is a font and the string is valid
  if(PrintSanityCheck(utfRef)) return fLineSpacingHeight;
  // Push tint
  FboItemPushQuadColour();
  // Simulate the height of the print
  const GLfloat fHeight = DoPrintWS(UtilDistance(fX, fW), fI, utfRef);
  // Reset the iterator on the utf string.
  utfRef.Reset();
  // Print the string
  DoPrintW(fX, fY-fHeight, fW, fI, utfRef);
  // Restore colour intensity
  FboItemPopQuadColour();
  // Check if texture needs reloading
  AtlasCheckReloadTexture();
  // Return height of printed text
  return fHeight;
}
/* -- Print text upwards --------------------------------------------------- */
void PrintU(const GLfloat fX, const GLfloat fY, const GLubyte*const ucpStr)
{ // Build a new utfstring class with the string
  UtfDecoder utfRef{ ucpStr };
  // Check that texture is a font and the string is valid
  if(PrintSanityCheck(utfRef)) return;
  // Push tint
  FboItemPushQuadColour();
  // Simulate the height of the print
  const GLfloat fHeight = DoPrintSU(utfRef);
  // Reset the iterator on the utf string
  utfRef.Reset();
  // Simulate the height of the print
  DoPrint(fX, fY-fHeight, fX, utfRef);
  // Check if texture needs reloading
  AtlasCheckReloadTexture();
  // Restore colour intensity
  FboItemPopQuadColour();
}
/* -- Print text upwards align centred ------------------------------------- */
void PrintUC(const GLfloat fX, const GLfloat fY, const GLubyte*const ucpStr)
{ // Build a new utfstring class with the string
  UtfDecoder utfRef{ ucpStr };
  // Check that texture is a font and the string is valid
  if(PrintSanityCheck(utfRef)) return;
  // Push tint
  FboItemPushQuadColour();
  // Simulate the height of the print
  const GLfloat fHeight = DoPrintSU(utfRef);
  // Reset the iterator on the utf string
  utfRef.Reset();
  // Simulate the height of the print
  DoPrintC(fX, fY-fHeight, utfRef);
  // Check if texture needs reloading
  AtlasCheckReloadTexture();
  // Restore colour intensity
  FboItemPopQuadColour();
}
/* -- Print text upwards align centred with custom texture ----------------- */
void PrintUCT(const GLfloat fX, const GLfloat fY, const GLubyte*const ucpStr,
  Texture &tNGlyphs)
{ // Build a new utfstring class with the string
  UtfDecoder utfRef{ ucpStr };
  // Check that texture is a font and the string is valid
  if(PrintSanityCheck(utfRef)) return;
  // Save current colour and set glyph texture
  PushQuadColourAndGlyphs(tNGlyphs);
  // Simulate the height of the print
  const GLfloat fHeight = DoPrintSU(utfRef);
  // Reset the iterator on the utf string
  utfRef.Reset();
  // Print the string
  DoPrintC(fX, fY-fHeight, utfRef);
  // Restore colour and reset glyph texture
  PopQuadColourAndGlyphs();
  // Check if texture needs reloading
  AtlasCheckReloadTexture();
}
/* -- Print text upwards align right --------------------------------------- */
void PrintUR(const GLfloat fX, const GLfloat fY, const GLubyte*const ucpStr)
{ // Build a new utfstring class with the string
  UtfDecoder utfRef{ ucpStr };
  // Check that texture is a font and the string is valid
  if(PrintSanityCheck(utfRef)) return;
  // Push tint
  FboItemPushQuadColour();
  // Simulate the height of the print
  const GLfloat fHeight = DoPrintSU(utfRef);
  // Reset the iterator on the utf string
  utfRef.Reset();
  // Simulate the height of the print
  DoPrintR(fX, fY-fHeight, utfRef);
  // Check if texture needs reloading
  AtlasCheckReloadTexture();
  // Restore colour intensity
  FboItemPopQuadColour();
}
/* -- Print text upwards align right with custom texture ------------------- */
void PrintURT(const GLfloat fX, const GLfloat fY, const GLubyte*const ucpStr,
  Texture &tNGlyphs)
{ // Build a new utfstring class with the string
  UtfDecoder utfRef{ ucpStr };
  // Check that texture is a font and the string is valid
  if(PrintSanityCheck(utfRef)) return;
  // Save current colour and set glyph texture
  PushQuadColourAndGlyphs(tNGlyphs);
  // Simulate the height of the print
  const GLfloat fHeight = DoPrintSU(utfRef);
  // Reset the iterator on the utf string
  utfRef.Reset();
  // Print the string
  DoPrintR(fX, fY-fHeight, utfRef);
  // Restore colour and reset glyph texture
  PopQuadColourAndGlyphs();
  // Check if texture needs reloading
  AtlasCheckReloadTexture();
}
/* -- Print text, wrap at specified width, return height ------------------- */
GLfloat PrintWUT(const GLfloat fX, const GLfloat fY, const GLfloat fW,
  const GLfloat fI, const GLubyte*const ucpStr, Texture &tNGlyphs)
{ // Build a new utfstring class with the string
  UtfDecoder utfRef{ ucpStr };
  // Check that texture is a font and the string is valid
  if(PrintSanityCheck(utfRef)) return fLineSpacingHeight;
  // Save current colour and set glyph texture
  PushQuadColourAndGlyphs(tNGlyphs);
  // Simulate the height of the print
  const GLfloat fHeight = DoPrintWS(UtilDistance(fX, fW), fI, utfRef);
  // Reset the iterator on the utf string
  utfRef.Reset();
  // Now actually print
  DoPrintW(fX, fY - fHeight, fW, fI, utfRef);
  // Restore colour and reset glyph texture
  PopQuadColourAndGlyphs();
  // Check if texture needs reloading
  AtlasCheckReloadTexture();
  // Return height
  return fHeight;
}
/* -- Print text, wrap at specified width, return height ------------------- */
GLfloat PrintWT(const GLfloat fX, const GLfloat fY, const GLfloat fW,
  const GLfloat fI, const GLubyte*const ucpStr, Texture &tNGlyphs)
{ // Build a new utfstring class with the string
  UtfDecoder utfRef{ ucpStr };
  // Check that texture is a font and the string is valid
  if(PrintSanityCheck(utfRef)) return fLineSpacingHeight;
  // Save current colour and set glyph texture
  PushQuadColourAndGlyphs(tNGlyphs);
  // Print the string
  const GLfloat fHeight = DoPrintW(fX, fY, fW, fI, utfRef);
  // Restore colour and reset glyph texture
  PopQuadColourAndGlyphs();
  // Check if texture needs reloading
  AtlasCheckReloadTexture();
  // Return height
  return fHeight;
}
/* -- Print text ----------------------------------------------------------- */
void PrintT(const GLfloat fX, const GLfloat fY, const GLubyte*const ucpStr,
  Texture &tNGlyphs)
{ // Build a new utfstring class with the string
  UtfDecoder utfRef{ ucpStr };
  // Check that texture is a font and the string is valid
  if(PrintSanityCheck(utfRef)) return;
  // Save current colour and set glyph texture
  PushQuadColourAndGlyphs(tNGlyphs);
  // Print the string
  DoPrint(fX, fY, fX, utfRef);
  // Restore colour and reset glyph texture
  PopQuadColourAndGlyphs();
  // Check if texture needs reloading
  AtlasCheckReloadTexture();
}
/* -- Print text with right align ------------------------------------------ */
void PrintRT(const GLfloat fX, const GLfloat fY, const GLubyte*const ucpStr,
  Texture &tNGlyphs)
{ // Build a new utfstring class with the string
  UtfDecoder utfRef{ ucpStr };
  // Check that texture is a font and the string is valid
  if(PrintSanityCheck(utfRef)) return;
  // Save current colour and set glyph texture
  PushQuadColourAndGlyphs(tNGlyphs);
  // Print the string
  DoPrintR(fX, fY, utfRef);
  // Restore colour and reset glyph texture
  PopQuadColourAndGlyphs();
  // Check if texture needs reloading
  AtlasCheckReloadTexture();
}
/* -- Print a string ------------------------------------------------------- */
void Print(const GLfloat fX, const GLfloat fY, const GLubyte*const ucpStr)
{ // Build a new utfstring class with the string
  UtfDecoder utfRef{ ucpStr };
  // Check that texture is a font and the string is valid
  if(PrintSanityCheck(utfRef)) return;
  // Save colour intensity
  FboItemPushQuadColour();
  // Print the utf string
  DoPrint(fX, fY, fX, utfRef);
  // Restore colour intensity
  FboItemPopQuadColour();
  // Check if texture needs reloading
  AtlasCheckReloadTexture();
}
/* -- Print text with right align ------------------------------------------ */
void PrintR(const GLfloat fX, const GLfloat fY, const GLubyte*const ucpStr)
{ // Build a new utfstring class with the string
  UtfDecoder utfRef{ ucpStr };
  // Check that texture is a font and the string is valid
  if(PrintSanityCheck(utfRef)) return;
  // Push tint
  FboItemPushQuadColour();
  // Print the string
  DoPrintR(fX, fY, utfRef);
  // Restore colour intensity
  FboItemPopQuadColour();
  // Check if texture needs reloading
  AtlasCheckReloadTexture();
}
/* -- Print a string with centre alignment --------------------------------- */
void PrintC(const GLfloat fX, const GLfloat fY, const GLubyte*const ucpStr)
{ // Build a new utfstring class with the string
  UtfDecoder utfRef{ ucpStr };
  // Check that texture is a font and the string is valid
  if(PrintSanityCheck(utfRef)) return;
  // Save colour
  FboItemPushQuadColour();
  // Print the utfstring
  DoPrintC(fX, fY, utfRef);
  // Restore colour
  FboItemPopQuadColour();
  // Check if texture needs reloading
  AtlasCheckReloadTexture();
}
/* -- Print text ----------------------------------------------------------- */
void PrintCT(const GLfloat fX, const GLfloat fY, const GLubyte*const ucpStr,
  Texture &tNGlyphs)
{ // Build a new utfstring class with the string
  UtfDecoder utfRef{ ucpStr };
  // Check that texture is a font and the string is valid
  if(PrintSanityCheck(utfRef)) return;
  // Save current colour and set glyph texture
  PushQuadColourAndGlyphs(tNGlyphs);
  // Print the string
  DoPrintC(fX, fY, utfRef);
  // Restore colour and reset glyph texture
  PopQuadColourAndGlyphs();
  // Check if texture needs reloading
  AtlasCheckReloadTexture();
}
/* -- Print text with glyphs ----------------------------------------------- */
void PrintMT(const GLfloat fX, const GLfloat fY, const GLfloat fL,
  const GLfloat fR, const GLubyte*const ucpStr, Texture &tNGlyphs)
{ // Build a new utfstring class with the string
  UtfDecoder utfRef{ ucpStr };
  // Check that texture is a font and the string is valid
  if(PrintSanityCheck(utfRef)) return;
  // Save current colour and set glyph texture
  PushQuadColourAndGlyphs(tNGlyphs);
  // Print the string
  DoPrintM(fX, fY, fL, fR, utfRef);
  // Restore colour and reset glyph texture
  PopQuadColourAndGlyphs();
  // Check if texture needs reloading
  AtlasCheckReloadTexture();
}
/* -- Simulate printing a string and returning the height ------------------ */
GLfloat PrintSU(const GLubyte*const ucpStr)
{ // Build a new utfstring class with the string
  UtfDecoder utfRef{ ucpStr };
  // Check that texture is a font and the string is valid
  if(PrintSanityCheck(utfRef)) return fLineSpacingHeight;
  // Push tint
  FboItemPushQuadColour();
  // Simulate the height
  const GLfloat fHeight = DoPrintSU(utfRef);
  // Restore colour intensity
  FboItemPopQuadColour();
  // Return highest height
  return fHeight;
}
/* -- Simulate printing a string ------------------------------------------- */
GLfloat PrintS(const GLubyte*const ucpStr)
{ // Build a new utfstring class with the string
  UtfDecoder utfRef{ ucpStr };
  // Check that texture is a font and the string is valid
  if(PrintSanityCheck(utfRef)) return 0;
  // Push tint
  FboItemPushQuadColour();
  // Print simulate of the utf string
  const GLfloat fWidth = DoPrintS(utfRef);
  // Restore colour intensity
  FboItemPopQuadColour();
  // Return highest width or width
  return fWidth;
}
/* -- Simulate a string of textures with glyphs ---------------------------- */
GLfloat PrintTS(const GLubyte*const ucpStr, Texture &tNGlyphs)
{ // Build a new utfstring class with the string
  UtfDecoder utfRef{ ucpStr };
  // Check that texture is a font and the string is valid
  if(PrintSanityCheck(utfRef)) return 0;
  // Save current colour and set glyph texture
  PushQuadColourAndGlyphs(tNGlyphs);
  // Print the string
  const GLfloat fWidth = DoPrintS(utfRef);
  // Restore colour and reset glyph texture
  PopQuadColourAndGlyphs();
  // Return highest width or width
  return fWidth;
}
/* -- Print text ----------------------------------------------------------- */
void PrintM(const GLfloat fX, const GLfloat fY, const GLfloat fL,
  const GLfloat fR, const GLubyte*const ucpStr)
{ // Build a new utfstring class with the string
  UtfDecoder utfRef{ ucpStr };
  // Check that texture is a font and the string is valid
  if(PrintSanityCheck(utfRef)) return;
  // Save colour intensity
  FboItemPushQuadColour();
  // Do the print
  DoPrintM(fX, fY, fL, fR, utfRef);
  // Restore colour intensity
  FboItemPopQuadColour();
  // Check if texture needs reloading
  AtlasCheckReloadTexture();
}
/* == EoF =========================================================== EoF == */
