/* == FONTGLPH.HPP ========================================================= **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## This file is included as part of the Font class from font.hpp and   ## **
** ## cotains functions related to glyph loading.                         ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
struct StrokerFunc                     // Using as a public namespace only
{ /* -- No outline required ------------------------------------------------ */
  class NoOutline                      // Members initially private
  { /* --------------------------------------------------------------------- */
    const FT_Error ftErr;              // Error code
    /* ------------------------------------------------------------- */ public:
    FT_Error Result() const { return ftErr; }
    /* --------------------------------------------------------------------- */
    NoOutline(FT_Glyph&, FT_Stroker) : ftErr(0) {}
    /* --------------------------------------------------------------------- */
    explicit NoOutline(const FT_Error ftE) : ftErr(ftE) {}
  };/* -- Full outline required -------------------------------------------- */
  struct Outline : public NoOutline {
    Outline(FT_Glyph &gD, FT_Stroker ftS) :
      NoOutline{ FreeType::FreeTypeApplyStrokerFull(gD, ftS) } {} };
  /* -- Outside outline required ------------------------------------------- */
  struct OutlineOutside : public NoOutline {
    OutlineOutside(FT_Glyph &gD, FT_Stroker ftS) :
      NoOutline{ FreeType::FreeTypeApplyStrokerOutside(gD, ftS) } {} };
  /* -- Inside outline required -------------------------------------------- */
  struct OutlineInside : public NoOutline {
    OutlineInside(FT_Glyph &gD, FT_Stroker ftS) :
      NoOutline{ FreeType::FreeTypeApplyStrokerInside(gD, ftS) } {} };
  /* ----------------------------------------------------------------------- */
};                                     // End of StrokerFunc
/* ------------------------------------------------------------------------- */
struct InitCharFunc                    // Members initially public
{ /* -- Load type class functors for InitFTChar() -------------------------- */
  struct NoOutline { NoOutline(Font*const fP, FT_GlyphSlot &ftgD,
    const Codepoint cStart, const Codepoint cEnd, const GLfloat fA)
  { fP->DoInitFTChar<StrokerFunc::NoOutline>(ftgD, cStart, cEnd, fA); } };
  /* -- Outline required --------------------------------------------------- */
  struct Outline2 { Outline2(Font*const fP, FT_GlyphSlot &ftgD,
    const Codepoint cStart, const Codepoint cEnd, const GLfloat fA)
  { fP->DoInitFTCharOutline<StrokerFunc::OutlineInside, StrokerFunc::Outline>
      (ftgD, cStart, cEnd, fA); } };
  /* -- Outside outline required ------------------------------------------- */
  struct Outline1 { Outline1(Font*const fP, FT_GlyphSlot &ftgD,
    const Codepoint cStart, const Codepoint cEnd, const GLfloat fA)
  { fP->DoInitFTCharOutline<StrokerFunc::NoOutline, StrokerFunc::Outline>
      (ftgD, cStart, cEnd, fA); } };
  /* ----------------------------------------------------------------------- */
};                                     // End of InitCharFunc
/* ------------------------------------------------------------------------- */
struct StrokerCheckFunc                // Members initially public
{ /* -- Functor to automatically check for outline method ------------------ */
  template<class InitCharFuncType> struct Auto {
    Auto(Font*const fP, FT_GlyphSlot &ftgD, const Codepoint cStart,
      const Codepoint cEnd, const GLfloat fA)
  { fP->DoSelectOutlineType(ftgD, cStart, cEnd, fA); } };
  /* -- Functor to set outline method manually ----------------------------- */
  template<class InitCharFuncType>struct Manual :
    private InitCharFuncType
  { Manual(Font*const fP, FT_GlyphSlot &ftgD, const Codepoint cStart,
      const Codepoint cEnd, const GLfloat fA) :
        InitCharFuncType{ fP, ftgD, cStart, cEnd, fA } {} };
  /* ----------------------------------------------------------------------- */
};                                     // End of StrokerCheckFunc
/* ------------------------------------------------------------------------- */
struct RoundFunc                       // Members initially public
{ /* -- Configurable rounding functor helper class ------------------------- */
  template<typename T=double>
    requires is_floating_point_v<T>
  class Straight                       // Default no-outline class
  { /* --------------------------------------------------------------------- */
    const T        tValue;             // Calculated advance value
    /* ------------------------------------------------------------- */ public:
    T Result() const { return tValue; }
    /* --------------------------------------------------------------------- */
    explicit Straight(const T tV) : tValue{ tV } {}
  };/* -- Round co-ordinates to lowest or highest unit --------------------- */
  template<typename T=double>struct Round : public Straight<T>
    { explicit Round(const T tV) : Straight<T>{ roundf(tV) } {} };
  /* -- Round co-ordinates to lowest unit ---------------------------------- */
  template<typename T=double>struct Floor : public Straight<T>
    { explicit Floor(const T tV) : Straight<T>{ floorf(tV) } {} };
  /* -- Round co-ordinates to upper unit ----------------------------------- */
  template<typename T=double>struct Ceil : public Straight<T>
    { explicit Ceil(const T tV) : Straight<T>{ ceilf(tV) } {} };
  /* ----------------------------------------------------------------------- */
};                                     // End of RoundFunc
/* ------------------------------------------------------------------------- */
struct RoundCheckFunc                  // Members initially public
{ /* -- Function to to automatically check for rounding method ------------- */
  template<class RoundFuncType>class Auto : public RoundFuncType
  { /* -- Detect advance rounding method from flags ------------------------ */
    static GLfloat Detect(const ImageFlagsConst &ffFlags,
      const GLfloat fAdvance)
    { // Load the character with floor rounding?
      if(ffFlags.FlagIsSet(FF_FLOORADVANCE))
        return RoundFunc::template Floor<GLfloat>{ fAdvance }.Result();
      // Load the character with ceil rounding?
      else if(ffFlags.FlagIsSet(FF_CEILADVANCE))
        return RoundFunc::template Ceil<GLfloat>{ fAdvance }.Result();
      // Load the character with round rounding?
      else if(ffFlags.FlagIsSet(FF_ROUNDADVANCE))
        return RoundFunc::template Round<GLfloat>{ fAdvance }.Result();
      // No rounding (allows subpixel drawing)
      return fAdvance;
    }
    /* -- Normal entry for auto-detection -------------------------- */ public:
    Auto(const ImageFlagsConst &ffFlags, const GLfloat fAdvance) :
      RoundFuncType{ Detect(ffFlags, fAdvance) } {}
  };/* -- Manually specified rounding functor check ------------------------ */
  template<class RoundFuncType>struct Manual : public RoundFuncType {
    Manual(const ImageFlagsConst&, const GLfloat fAdvance) :
      RoundFuncType{ fAdvance } {} };
  /* ----------------------------------------------------------------------- */
};                                     // End of RoundCheckFunc
/* ------------------------------------------------------------------------- */
struct HandleGlyphFunc                 // Members initially public
{ /* -- Auto detect font type functor -------------------------------------- */
  struct Auto                          // Members initially public
  { /* --------------------------------------------------------------------- */
    template<class StrokerCheckFuncType, class RoundCheckFuncType>
      size_t DoHandleGlyph(Font*const fP, const Codepoint cChar,
        const Codepoint cStartos)
          { return fP->DoSelectFontType<StrokerCheckFuncType,
              RoundCheckFuncType>(cChar, cStartos); }
  };/* -- Freetype font selector functor ----------------------------------- */
  struct FreeType                     // Members initially public
  { /* --------------------------------------------------------------------- */
    template<class StrokerCheckFuncType, class RoundCheckFuncType>
      size_t DoHandleGlyph(Font*const fP, const Codepoint cChar,
        const Codepoint cStartos)
          { return fP->DoHandleFTGlyph<StrokerCheckFuncType,
              RoundCheckFuncType>(cChar, cStartos); }
  };/* -- Standard glyph font selector functor ----------------------------- */
  struct Glyph                        // Members initially public
  { /* --------------------------------------------------------------------- */
    template<class StrokerCheckFuncType, class RoundCheckFuncType>
      static size_t DoHandleGlyph(Font*const fP, const Codepoint cChar,
        const Codepoint cStartos)
          { return fP->DoHandleStaticGlyph(cChar, cStartos); }
  };/* --------------------------------------------------------------------- */
};                                    // End of HandleGlyphFunc
/* -- Do load character function ------------------------------------------- */
template<class StrokerFuncType>
  void DoInitFTChar(FT_GlyphSlot &ftgsRef, const Codepoint cStartos,
    const Codepoint cChar, const GLfloat fAdvance)
{ // Move The Face's Glyph Into A Glyph Object to get outline
  FT_Glyph gData;
  cFreeType->FreeTypeCheckError(FT_Get_Glyph(ftgsRef, &gData),
    "Failed to get glyph!", "Identifier", IdentGet(), "Glyph", cChar);
  // Put in autorelease ptr to autorelease
  typedef unique_ptr<FT_GlyphRec_, function<decltype(FT_Done_Glyph)>> GlyphPtr;
  if(GlyphPtr gPtr{ gData, FT_Done_Glyph })
  { // Apply glyph border if requested
    cFreeType->FreeTypeCheckError(
      StrokerFuncType{ gData, ftfData.GetStroker() }.Result(),
      "Failed to apply outline to glyph!",
      "Identifier", IdentGet(), "Glyph", cChar);
    // Convert The Glyph To A Image.
    cFreeType->FreeTypeCheckError(FT_Glyph_To_Bitmap(&gData,
      FT_RENDER_MODE_NORMAL, nullptr, true),
      "Failed to render glyph to image!",
      "Identifier", IdentGet(), "Glyph", cChar);
    // The above function will have modified the glyph address and freed the
    // old one so let's release the old one (not freeing it) and update it so
    // unique_ptr dtor knows to destroy the new one.
    gPtr.release();
    gPtr.reset(gData);
    // Get glyph data class and set advanced and status to loaded
    Glyph &gRef = gvData[cStartos];
    gRef.GlyphSetLoaded();
    gRef.GlyphSetAdvance(fAdvance);
    // Access image information and if has dimensions?
    const FT_Bitmap &bData = reinterpret_cast<FT_BitmapGlyph>(gData)->bitmap;
    if(bData.width > 0 && bData.rows > 0)
    { // Get glyph outline origins
      FT_BBox bbData;
      FT_Glyph_Get_CBox(gData, FT_GLYPH_BBOX_PIXELS, &bbData);
      // Set glyph size
      gRef.DimSet(static_cast<GLfloat>(bData.width),
                  static_cast<GLfloat>(bData.rows));
      // Set glyph bounds
      gRef.CoordsSet(static_cast<GLfloat>(bbData.xMin),
        -static_cast<GLfloat>(static_cast<int>(bData.rows) + bbData.yMin) +
           dfFont.DimGetHeight(),
         static_cast<GLfloat>(bbData.xMax),
        -static_cast<GLfloat>(bbData.yMin) + dfFont.DimGetHeight());
      // Add the glyph to texture
      AtlasAddBitmap<ImageTypeGrayAlpha>
        (cStartos, bData.width, bData.rows, bData.buffer);
    } // Glyph has no dimensions so push default font size
    else gRef.DimSet(dfFont);
  } // Failed to grab pointer to glyph data
  else XC("Failed to get glyph pointer!",
    "Identifier", IdentGet(), "Glyph", cChar);
}
/* -- Initialise freetype char and set types ------------------------------- */
template<class StrokerFuncNormalType, class StrokerFuncOutlineType>
  void DoInitFTCharOutline(FT_GlyphSlot &ftgsRef, const Codepoint cStartos,
    const Codepoint cChar, const GLfloat fAdvance)
{ // Initialise main character
  DoInitFTChar<StrokerFuncNormalType>(ftgsRef, cStartos, cChar, fAdvance);
  // Initialise outline character
  DoInitFTChar<StrokerFuncOutlineType>(ftgsRef, cStartos+1, cChar, fAdvance);
}
/* -- Function to select correct outline method ---------------------------- */
void DoSelectOutlineType(FT_GlyphSlot &ftgsRef, const Codepoint cStartos,
    const Codepoint cChar, const GLfloat fAdvance)
{ // Stroker loaded?
  if(ftfData.IsStrokerLoaded())
  { // Stroke inside and outside border?
    if(FlagIsSet(FF_STROKETYPE2))
      InitCharFunc::Outline2{ this, ftgsRef, cStartos, cChar, fAdvance };
    // Stroke just the outside? (default
    else InitCharFunc::Outline1{ this, ftgsRef, cStartos, cChar, fAdvance };
  } // No outline, just load as normal
  else InitCharFunc::NoOutline{ this, ftgsRef, cStartos, cChar, fAdvance };
}
/* ------------------------------------------------------------------------- */
template<class StrokerCheckFuncType, class RoundCheckFuncType>
  size_t DoHandleFTGlyph(const Codepoint cChar, const Codepoint cStartos)
{ // If position is not allocated?
  if(cStartos >= gvData.size())
  { // Extend and initialise storage for glyph co-ordinates. Remember we
    // need double the space if we're using an outline. Also, a character of
    // ASCII value 0 is still a character so we got to allocate space for it.
    // For some reason, I need to add {} for .resize to invoke the default
    // 'Glyph' constructor for some reason.
    gvData.resize(stMultiplier + cStartos * stMultiplier, {});
    // Extend and initialise storage for gl texture co-ordinates
    clTiles[0].resize(gvData.size());
  } // Return the position if already loaded
  else if(gvData[cStartos].GlyphIsLoaded()) return cStartos;
  // Translate character to glyph and if succeeded?
  if(const FT_UInt uiGl = ftfData.CharToGlyph(static_cast<FT_ULong>(cChar)))
  { // Load glyph and return glyph on success else throw exception
    cFreeType->FreeTypeCheckError(ftfData.LoadGlyph(uiGl),
      "Failed to load glyph!",
      "Identifier", IdentGet(), "Index", cChar);
    // Get glyph slot handle and get advance width.
    FT_GlyphSlot ftgsRef = ftfData.GetGlyphData();
    // Compare type of border required
    const GLfloat fAdvance = RoundCheckFuncType(*this,
      static_cast<GLfloat>(ftgsRef->metrics.horiAdvance) / 64).Result();
    // Begin initialisation of char by checking stroker setting. This can
    // either be a pre-calculated or calculated right now.
    StrokerCheckFuncType{ this, ftgsRef, cStartos, cChar, fAdvance };
    // Return position
    return cStartos;
  } // Show error if we couldn't load the default character
  if(cChar == ulDefaultChar)
    XC("Default character not available!",
      "Identifier", IdentGet(), "Index", cChar, "Position", cStartos);
  // Try to load the default character instead.
  return DoHandleFTGlyph<StrokerCheckFuncType, RoundCheckFuncType>(
    static_cast<size_t>(ulDefaultChar),
    static_cast<size_t>(ulDefaultChar) * stMultiplier);
}
/* -- Do handle a static glyph --------------------------------------------- */
size_t DoHandleStaticGlyph(const Codepoint cChar, const Codepoint cStartos)
{ // Static font. If character is in range and loaded
  if(cStartos < gvData.size() && gvData[cStartos].GlyphIsLoaded())
    return cStartos;
  // Try to find the default character and return position if valiid
  const size_t stDefPos = static_cast<size_t>(ulDefaultChar) * stMultiplier;
  if(stDefPos < gvData.size()) return stDefPos;
  // Impossible situation. Caller should at least have a question mark in
  // their font.
  XC("Specified string contains unprintable characters with no "
    "valid fall-back character!",
    "Identifier",      IdentGet(),    "Character",  cChar,
    "BackupCharacter", ulDefaultChar, "Position",   cStartos,
    "BackupPosition",  stDefPos,      "Mulitplier", stMultiplier,
    "Maximum",         gvData.size());
}
/* -- Function to select correct outline method ---------------------------- */
template<class StrokerCheckFuncType, class RoundCheckFuncType>
  size_t DoSelectFontType(const Codepoint cChar, const Codepoint cStartos)
{ // Stroker loaded?
  return ftfData.IsLoaded() ?
    DoHandleFTGlyph<StrokerCheckFuncType, RoundCheckFuncType>(cChar,cStartos) :
    DoHandleStaticGlyph(cChar, cStartos);
}
/* -- Check if a character needs initialising ------------------------------ */
template<class FontCheckFunc, class StrokerCheckFuncType,
  class RoundCheckFuncType>
size_t DoCheckGlyph(const Codepoint cChar)
{ // Get character position and if freetype font is assigned?
  return FontCheckFunc().template DoHandleGlyph<StrokerCheckFuncType,
    RoundCheckFuncType>(this, cChar, cChar * stMultiplier);
}
/* -- Do initialise all freetype characters in specified range ------------- */
template<class HandleGlyphFuncType,
  class InitCharFuncType,
  class RoundCheckFuncType>
void DoInitFTCharRange(const Codepoint cStart, const Codepoint cEnd)
  { for(Codepoint cIndex = cStart; cIndex < cEnd; ++cIndex)
      DoCheckGlyph<HandleGlyphFuncType,
                   InitCharFuncType,
                   RoundCheckFuncType>(cIndex); }
/* -- Apply rounding functor before entering loop -------------------------- */
template<class HandleGlyphFuncType, class InitCharFuncType>
  void DoInitFTCharRangeApplyRound(const Codepoint cStart,
    const Codepoint cEnd)
{ // Load the character with floor rounding?
  if(FlagIsSet(FF_FLOORADVANCE))
    DoInitFTCharRange<HandleGlyphFuncType, InitCharFuncType,
      RoundCheckFunc::Manual<RoundFunc::Floor<GLfloat>>>(cStart, cEnd);
  // Load the character with ceil rounding?
  else if(FlagIsSet(FF_CEILADVANCE))
    DoInitFTCharRange<HandleGlyphFuncType, InitCharFuncType,
      RoundCheckFunc::Manual<RoundFunc::Ceil<GLfloat>>>(cStart, cEnd);
  // Load the character with round rounding?
  else if(FlagIsSet(FF_ROUNDADVANCE))
    DoInitFTCharRange<HandleGlyphFuncType, InitCharFuncType,
      RoundCheckFunc::Manual<RoundFunc::Round<GLfloat>>>(cStart, cEnd);
  // No rounding (allows subpixel drawing)
  DoInitFTCharRange<HandleGlyphFuncType, InitCharFuncType,
    RoundCheckFunc::Manual<RoundFunc::Straight<GLfloat>>>(cStart, cEnd);
}
/* -- Apply stroker functor before entering loop --------------------------- */
template<class HandleGlyphFuncType>
  void DoInitFTCharRangeApplyStroker(const Codepoint cStart,
    const Codepoint cEnd)
{ // Stroker requested
  if(ftfData.IsStrokerLoaded())
  { // Load entire stroker?
    if(FlagIsSet(FF_STROKETYPE2))
      DoInitFTCharRangeApplyRound<HandleGlyphFuncType,
        StrokerCheckFunc::Manual<InitCharFunc::Outline2>>
          (cStart, cEnd);
    // Load outside stroker (default)
    else DoInitFTCharRangeApplyRound<HandleGlyphFuncType,
      StrokerCheckFunc::Manual<InitCharFunc::Outline1>>
        (cStart, cEnd);
  }  // Load no stroker
  else DoInitFTCharRangeApplyRound<HandleGlyphFuncType,
    StrokerCheckFunc::Manual<InitCharFunc::NoOutline>>
      (cStart, cEnd);
}
/* -- Do initialise all freetype characters in specified string ------------ */
template<class HandleGlyphFuncType,
         class InitCharFuncType,
         class RoundCheckFuncType>
void DoInitFTCharString(const GLubyte*const ucpPtr)
{ // Build a new utfstring class with the string
  UtfDecoder udRef{ ucpPtr };
  // Enumerate trough the entire string
  while(const Codepoint cChar = udRef.UtfNext())
    DoCheckGlyph<HandleGlyphFuncType,
                 InitCharFuncType,
                 RoundCheckFuncType>(cChar);
}
/* -- Apply rounding functor before entering loop -------------------------- */
template<class HandleGlyphFuncType, class InitCharFuncType>
  void DoInitFTCharStringApplyRound(const GLubyte*const ucpPtr)
{ // Load the character with floor rounding?
  if(FlagIsSet(FF_FLOORADVANCE))
    DoInitFTCharString<HandleGlyphFuncType, InitCharFuncType,
      RoundCheckFunc::Manual<RoundFunc::Floor<GLfloat>>>(ucpPtr);
  // Load the character with ceil rounding?
  else if(FlagIsSet(FF_CEILADVANCE))
    DoInitFTCharString<HandleGlyphFuncType, InitCharFuncType,
      RoundCheckFunc::Manual<RoundFunc::Ceil<GLfloat>>>(ucpPtr);
  // Load the character with round rounding?
  else if(FlagIsSet(FF_ROUNDADVANCE))
    DoInitFTCharString<HandleGlyphFuncType, InitCharFuncType,
      RoundCheckFunc::Manual<RoundFunc::Round<GLfloat>>>(ucpPtr);
  // No rounding (allows subpixel drawing)
  DoInitFTCharString<HandleGlyphFuncType, InitCharFuncType,
    RoundCheckFunc::Manual<RoundFunc::Straight<GLfloat>>>(ucpPtr);
}
/* -- Apply rounding functor before entering loop -------------------------- */
template<class HandleGlyphFuncType>
  void DoInitFTCharStringApplyStroker(const GLubyte*const ucpPtr)
{ // If stroker is loaded?
  if(ftfData.IsStrokerLoaded())
  { // Load entire stroker outline?
    if(FlagIsSet(FF_STROKETYPE2))
      DoInitFTCharStringApplyRound<HandleGlyphFuncType,
        StrokerCheckFunc::Manual<InitCharFunc::Outline2>>
          (ucpPtr);
    // Load outside of stroker (default)
    else DoInitFTCharStringApplyRound<HandleGlyphFuncType,
      StrokerCheckFunc::Manual<InitCharFunc::Outline1>>
        (ucpPtr);
  } // Load no stroker
  else DoInitFTCharStringApplyRound<HandleGlyphFuncType,
    StrokerCheckFunc::Manual<InitCharFunc::NoOutline>>
      (ucpPtr);
}
/* == EoF =========================================================== EoF == */
