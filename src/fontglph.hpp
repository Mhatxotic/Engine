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
  struct NoOutline { NoOutline(Font*const foP, FT_GlyphSlot &ftgD,
    const UtfCodepoint utcStart, const UtfCodepoint utcEnd, const GLfloat glfA)
  { foP->DoInitFTChar<StrokerFunc::NoOutline>
      (ftgD, utcStart, utcEnd, glfA); } };
  /* -- Outline required --------------------------------------------------- */
  struct Outline2 { Outline2(Font*const foP, FT_GlyphSlot &ftgD,
    const UtfCodepoint utcStart, const UtfCodepoint utcEnd, const GLfloat glfA)
  { foP->DoInitFTCharOutline<StrokerFunc::OutlineInside, StrokerFunc::Outline>
      (ftgD, utcStart, utcEnd, glfA); } };
  /* -- Outside outline required ------------------------------------------- */
  struct Outline1 { Outline1(Font*const foP, FT_GlyphSlot &ftgD,
    const UtfCodepoint utcStart, const UtfCodepoint utcEnd, const GLfloat glfA)
  { foP->DoInitFTCharOutline<StrokerFunc::NoOutline, StrokerFunc::Outline>
      (ftgD, utcStart, utcEnd, glfA); } };
  /* ----------------------------------------------------------------------- */
};                                     // End of InitCharFunc
/* ------------------------------------------------------------------------- */
struct StrokerCheckFunc                // Members initially public
{ /* -- Functor to automatically check for outline method ------------------ */
  template<class InitCharFuncType>struct Auto
    { Auto(Font*const foP, FT_GlyphSlot &ftgD, const UtfCodepoint utcStart,
        const UtfCodepoint utcEnd, const GLfloat glfA)
      { foP->DoSelectOutlineType(ftgD, utcStart, utcEnd, glfA); } };
  /* -- Functor to set outline method manually ----------------------------- */
  template<class InitCharFuncType>struct Manual : private InitCharFuncType
    { Manual(Font*const foP, FT_GlyphSlot &ftgD, const UtfCodepoint utcStart,
        const UtfCodepoint utcEnd, const GLfloat glfA) :
          InitCharFuncType{ foP, ftgD, utcStart, utcEnd, glfA } {} };
  /* ----------------------------------------------------------------------- */
};                                     // End of StrokerCheckFunc
/* ------------------------------------------------------------------------- */
struct RoundFunc                       // Members initially public
{ /* -- Configurable rounding functor helper class ------------------------- */
  template<typename FloatType = double>
    requires StdIsFloat<FloatType>
  class Straight                       // Default no-outline class
  { /* --------------------------------------------------------------------- */
    const FloatType ftValue;           // Calculated advance value
    /* ------------------------------------------------------------- */ public:
    FloatType Result() const { return ftValue; }
    /* --------------------------------------------------------------------- */
    explicit Straight(const FloatType ftV) : ftValue{ ftV } {}
  };/* -- Round co-ordinates to lowest or highest unit --------------------- */
  template<typename FloatType = double>
    requires StdIsFloat<FloatType>
  struct Round : public Straight<FloatType>
    { explicit Round(const FloatType ftV) :
        Straight<FloatType>{ roundf(ftV) } {} };
  /* -- Round co-ordinates to lowest unit ---------------------------------- */
  template<typename FloatType = double>
    requires StdIsFloat<FloatType>
  struct Floor : public Straight<FloatType>
    { explicit Floor(const FloatType ftV) :
        Straight<FloatType>{ floorf(ftV) } {} };
  /* -- Round co-ordinates to upper unit ----------------------------------- */
  template<typename FloatType = double>
    requires StdIsFloat<FloatType>
  struct Ceil : public Straight<FloatType>
    { explicit Ceil(const FloatType ftV) :
        Straight<FloatType>{ ceilf(ftV) } {} };
  /* ----------------------------------------------------------------------- */
};                                     // End of RoundFunc
/* ------------------------------------------------------------------------- */
struct RoundCheckFunc                  // Members initially public
{ /* -- Function to to automatically check for rounding method ------------- */
  template<class RoundFuncType>class Auto : public RoundFuncType
  { /* -- Detect advance rounding method from flags ------------------------ */
    static GLfloat Detect(const ImageFlagsConst &ifcFlags,
      const GLfloat glfAdvance)
    { // Load the character with floor rounding?
      if(ifcFlags.FlagIsSet(FF_FLOORADVANCE))
        return RoundFunc::template Floor<GLfloat>{ glfAdvance }.Result();
      // Load the character with ceil rounding?
      else if(ifcFlags.FlagIsSet(FF_CEILADVANCE))
        return RoundFunc::template Ceil<GLfloat>{ glfAdvance }.Result();
      // Load the character with round rounding?
      else if(ifcFlags.FlagIsSet(FF_ROUNDADVANCE))
        return RoundFunc::template Round<GLfloat>{ glfAdvance }.Result();
      // No rounding (allows subpixel drawing)
      return glfAdvance;
    }
    /* -- Normal entry for auto-detection -------------------------- */ public:
    Auto(const ImageFlagsConst &ifcFlags, const GLfloat glfAdvance) :
      RoundFuncType{ Detect(ifcFlags, glfAdvance) } {}
  };/* -- Manually specified rounding functor check ------------------------ */
  template<class RoundFuncType>struct Manual : public RoundFuncType {
    Manual(const ImageFlagsConst&, const GLfloat glfAdvance) :
      RoundFuncType{ glfAdvance } {} };
  /* ----------------------------------------------------------------------- */
};                                     // End of RoundCheckFunc
/* ------------------------------------------------------------------------- */
struct HandleGlyphFunc                 // Members initially public
{ /* -- Auto detect font type functor -------------------------------------- */
  struct Auto                          // Members initially public
  { /* --------------------------------------------------------------------- */
    template<class StrokerCheckFuncType, class RoundCheckFuncType>
      size_t DoHandleGlyph(Font*const foP, const UtfCodepoint utcChar,
      const UtfCodepoint utcCharOutline)
    { return foP->DoSelectFontType<StrokerCheckFuncType,
        RoundCheckFuncType>(utcChar, utcCharOutline); }
  };/* -- Freetype font selector functor ----------------------------------- */
  struct FreeType                     // Members initially public
  { /* --------------------------------------------------------------------- */
    template<class StrokerCheckFuncType, class RoundCheckFuncType>
      size_t DoHandleGlyph(Font*const foP, const UtfCodepoint utcChar,
      const UtfCodepoint utcCharOutline)
    { return foP->DoHandleFTGlyph<StrokerCheckFuncType,
        RoundCheckFuncType>(utcChar, utcCharOutline); }
  };/* -- Standard glyph font selector functor ----------------------------- */
  struct Glyph                        // Members initially public
  { /* --------------------------------------------------------------------- */
    template<class StrokerCheckFuncType, class RoundCheckFuncType>
      static size_t DoHandleGlyph(Font*const foP, const UtfCodepoint utcChar,
      const UtfCodepoint utcCharOutline)
    { return foP->DoHandleStaticGlyph(utcChar, utcCharOutline); }
  };/* --------------------------------------------------------------------- */
};                                     // End of HandleGlyphFunc
/* -- Do load character function ------------------------------------------- */
template<class StrokerFuncType>
  void DoInitFTChar(FT_GlyphSlot &ftgsRef, const UtfCodepoint utcCharOutline,
    const UtfCodepoint utcChar, const GLfloat glfAdvance)
{ // Move The Face's Glyph Into A Glyph Object to get outline
  FT_Glyph gData;
  cFreeType->FreeTypeCheckError(FT_Get_Glyph(ftgsRef, &gData),
    "Failed to get glyph!", "Name", NameGet(), "Glyph", utcChar);
  // Put in autorelease ptr to autorelease
  using GlyphPtr = StdUniquePtr<FT_GlyphRec_,
    function<decltype(FT_Done_Glyph)>>;
  if(GlyphPtr gPtr{ gData, FT_Done_Glyph })
  { // Apply glyph border if requested
    cFreeType->FreeTypeCheckError(
      StrokerFuncType{ gData, FtfGetStroker() }.Result(),
      "Failed to apply outline to glyph!",
      "Name", NameGet(), "Glyph", utcChar);
    // Convert The Glyph To A Image.
    cFreeType->FreeTypeCheckError(FT_Glyph_To_Bitmap(&gData,
      FT_RENDER_MODE_NORMAL, nullptr, true),
      "Failed to render glyph to image!", "Name", NameGet(), "Glyph", utcChar);
    // The above function will have modified the glyph address and freed the
    // old one so let's release the old one (not freeing it) and update it so
    // unique_ptr dtor knows to destroy the new one.
    gPtr.release();
    gPtr.reset(gData);
    // Get glyph data class and set advanced and status to loaded
    Glyph &gRef = gvData[utcCharOutline];
    gRef.GlyphSetLoaded();
    gRef.GlyphSetAdvance(glfAdvance);
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
           gldfFont.DimGetHeight(),
         static_cast<GLfloat>(bbData.xMax),
        -static_cast<GLfloat>(bbData.yMin) + gldfFont.DimGetHeight());
      // Add the glyph to texture
      AtlasAddBitmap<ImageTypeGrayAlpha>
        (utcCharOutline, bData.width, bData.rows, bData.buffer);
    } // Glyph has no dimensions so push default font size
    else gRef.DimSet(gldfFont);
  } // Failed to grab pointer to glyph data
  else XC("Failed to get glyph pointer!", "Name", NameGet(), "Glyph", utcChar);
}
/* -- Initialise freetype char and set types ------------------------------- */
template<class StrokerFuncNormalType, class StrokerFuncOutlineType>
  void DoInitFTCharOutline(FT_GlyphSlot &ftgsRef,
    const UtfCodepoint utcCharOutline, const UtfCodepoint utcChar,
    const GLfloat glfAdvance)
{ // Initialise main character
  DoInitFTChar<StrokerFuncNormalType>
    (ftgsRef, utcCharOutline, utcChar, glfAdvance);
  // Initialise outline character
  DoInitFTChar<StrokerFuncOutlineType>
   (ftgsRef, utcCharOutline+1, utcChar, glfAdvance);
}
/* -- Function to select correct outline method ---------------------------- */
void DoSelectOutlineType(FT_GlyphSlot &ftgsRef,
  const UtfCodepoint utcCharOutline, const UtfCodepoint utcChar,
  const GLfloat glfAdvance)
{ // Stroker loaded?
  if(FtfIsStrokerLoaded())
  { // Stroke inside and outside border?
    if(FlagIsSet(FF_STROKETYPE2))
      InitCharFunc::Outline2{ this,
        ftgsRef, utcCharOutline, utcChar, glfAdvance };
    // Stroke just the outside? (default
    else InitCharFunc::Outline1{ this,
      ftgsRef, utcCharOutline, utcChar, glfAdvance };
  } // No outline, just load as normal
  else InitCharFunc::NoOutline{ this,
    ftgsRef, utcCharOutline, utcChar, glfAdvance };
}
/* ------------------------------------------------------------------------- */
template<class StrokerCheckFuncType, class RoundCheckFuncType>
  bool DoHandleFTGlyph(Ftf &ftfSrc, const UtfCodepoint utcChar,
    const UtfCodepoint utcCharOutline)
{ // Convert the character to glyph and if successful?
  if(const FT_UInt uGl = ftfSrc.FtfCharToGlyph(static_cast<FT_ULong>(utcChar)))
  { // Load glyph and return glyph on success else throw exception
    cFreeType->FreeTypeCheckError(ftfSrc.FtfLoadGlyph(uGl),
      "Failed to load glyph!", "Name", NameGet(), "Index", utcChar);
    // Get glyph slot handle and get advance width.
    FT_GlyphSlot ftgsRef = ftfSrc.FtfGetGlyphData();
    // Compare type of border required
    const GLfloat glfAdvance = RoundCheckFuncType(*this,
      static_cast<GLfloat>(ftgsRef->metrics.horiAdvance) / 64).Result();
    // Begin initialisation of char by checking stroker setting. This can
    // either be a pre-calculated or calculated right now.
    StrokerCheckFuncType{ this, ftgsRef, utcCharOutline, utcChar, glfAdvance };
    // Success
    return true;
  } // Failed
  return false;
}
/* ------------------------------------------------------------------------- */
template<class StrokerCheckFuncType, class RoundCheckFuncType>
  size_t DoHandleFTGlyph(const UtfCodepoint utcChar,
    const UtfCodepoint utcCharOutline)
{ // If position is not allocated?
  if(utcCharOutline >= gvData.size())
  { // Extend and initialise storage for glyph co-ordinates. Remember we
    // need double the space if we're using an outline. Also, a character of
    // ASCII value 0 is still a character so we got to allocate space for it.
    // For some reason, I need to add {} for .resize to invoke the default
    // 'Glyph' constructor for some reason.
    gvData.resize(stMultiplier + utcCharOutline * stMultiplier, {});
    // Extend and initialise storage for gl texture co-ordinates
    clTiles[0].resize(gvData.size());
  } // Return the position if already loaded
  else if(gvData[utcCharOutline].GlyphIsLoaded()) return utcCharOutline;
  // Translate character to glyph and if succeeded?
  if(DoHandleFTGlyph<StrokerCheckFuncType, RoundCheckFuncType>(*this,
     utcChar, utcCharOutline)) return utcCharOutline;
  // Try alternate fonts
  for(Ftf &ftfExtra : ftflExtras)
    if(DoHandleFTGlyph<StrokerCheckFuncType, RoundCheckFuncType>(ftfExtra,
       utcChar, utcCharOutline)) return utcCharOutline;
  // Show error if we couldn't load the default character
  if(utcChar == utcDefaultChar)
    XC("Default character not available!",
      "Name", NameGet(), "Index", utcChar, "Position", utcCharOutline);
  // Try to load the default character instead.
  return DoHandleFTGlyph<StrokerCheckFuncType, RoundCheckFuncType>(
    utcDefaultChar, utcDefaultChar * utcMultiplier);
}
/* -- Do handle a static glyph --------------------------------------------- */
size_t DoHandleStaticGlyph(const UtfCodepoint utcChar,
  const UtfCodepoint utcCharOutline)
{ // Static font. If character is in range and loaded
  if(utcCharOutline < gvData.size() && gvData[utcCharOutline].GlyphIsLoaded())
    return utcCharOutline;
  // Try to find the default character and return position if valiid
  const size_t stDefPos = static_cast<size_t>(utcDefaultChar) * stMultiplier;
  if(stDefPos < gvData.size()) return stDefPos;
  // Impossible situation. Caller should at least have a question mark in
  // their font.
  XC("Specified string contains unprintable characters with no "
    "valid fall-back character!",
    "Name",            NameGet(),      "Character",  utcChar,
    "BackupCharacter", utcDefaultChar, "Position",   utcCharOutline,
    "BackupPosition",  stDefPos,       "Mulitplier", stMultiplier,
    "Maximum",         gvData.size());
}
/* -- Function to select correct outline method ---------------------------- */
template<class StrokerCheckFuncType, class RoundCheckFuncType>
  size_t DoSelectFontType(const UtfCodepoint utcChar,
  const UtfCodepoint utcCharOutline)
{ return FtfIsLoaded() ?
    DoHandleFTGlyph<StrokerCheckFuncType,
      RoundCheckFuncType>(utcChar, utcCharOutline) :
    DoHandleStaticGlyph(utcChar, utcCharOutline); }
/* -- Check if a character needs initialising ------------------------------ */
template<class FontCheckFunc, class StrokerCheckFuncType,
  class RoundCheckFuncType>
size_t DoCheckGlyph(const UtfCodepoint utcChar)
{ return FontCheckFunc().template DoHandleGlyph<StrokerCheckFuncType,
    RoundCheckFuncType>(this, utcChar, utcChar * utcMultiplier); }
/* -- Do initialise all freetype characters in specified range ------------- */
template<class HandleGlyphFuncType,
  class InitCharFuncType,
  class RoundCheckFuncType>
void DoInitFTCharRange(const UtfCodepoint utcStart, const UtfCodepoint utcEnd)
  { for(UtfCodepoint utcChar = utcStart; utcChar < utcEnd; ++utcChar)
      DoCheckGlyph<HandleGlyphFuncType,
                   InitCharFuncType,
                   RoundCheckFuncType>(utcChar); }
/* -- Apply rounding functor before entering loop -------------------------- */
template<class HandleGlyphFuncType, class InitCharFuncType>
  void DoInitFTCharRangeApplyRound(const UtfCodepoint utcStart,
    const UtfCodepoint utcEnd)
{ // Load the character with floor rounding?
  if(FlagIsSet(FF_FLOORADVANCE))
    DoInitFTCharRange<HandleGlyphFuncType, InitCharFuncType,
      RoundCheckFunc::Manual<RoundFunc::Floor<GLfloat>>>(utcStart, utcEnd);
  // Load the character with ceil rounding?
  else if(FlagIsSet(FF_CEILADVANCE))
    DoInitFTCharRange<HandleGlyphFuncType, InitCharFuncType,
      RoundCheckFunc::Manual<RoundFunc::Ceil<GLfloat>>>(utcStart, utcEnd);
  // Load the character with round rounding?
  else if(FlagIsSet(FF_ROUNDADVANCE))
    DoInitFTCharRange<HandleGlyphFuncType, InitCharFuncType,
      RoundCheckFunc::Manual<RoundFunc::Round<GLfloat>>>(utcStart, utcEnd);
  // No rounding (allows subpixel drawing)
  DoInitFTCharRange<HandleGlyphFuncType, InitCharFuncType,
    RoundCheckFunc::Manual<RoundFunc::Straight<GLfloat>>>(utcStart, utcEnd);
}
/* -- Apply stroker functor before entering loop --------------------------- */
template<class HandleGlyphFuncType>
  void DoInitFTCharRangeApplyStroker(const UtfCodepoint utcStart,
    const UtfCodepoint utcEnd)
{ // Stroker requested
  if(FtfIsStrokerLoaded())
  { // Load entire stroker?
    if(FlagIsSet(FF_STROKETYPE2))
      DoInitFTCharRangeApplyRound<HandleGlyphFuncType,
        StrokerCheckFunc::Manual<InitCharFunc::Outline2>>
          (utcStart, utcEnd);
    // Load outside stroker (default)
    else DoInitFTCharRangeApplyRound<HandleGlyphFuncType,
      StrokerCheckFunc::Manual<InitCharFunc::Outline1>>
        (utcStart, utcEnd);
  }  // Load no stroker
  else DoInitFTCharRangeApplyRound<HandleGlyphFuncType,
    StrokerCheckFunc::Manual<InitCharFunc::NoOutline>>
      (utcStart, utcEnd);
}
/* -- Do initialise all freetype characters in specified string ------------ */
template<class HandleGlyphFuncType,
         class InitCharFuncType,
         class RoundCheckFuncType>
void DoInitFTCharString(const StdStringView &ssvStr)
{ // Build a new utfstring class with the string
  UtfDecoder udStr{ ssvStr };
  // Enumerate trough the entire string
  while(const UtfCodepoint utcChar = udStr.UtfNext())
    DoCheckGlyph<HandleGlyphFuncType,
                 InitCharFuncType,
                 RoundCheckFuncType>(utcChar);
}
/* -- Apply rounding functor before entering loop -------------------------- */
template<class HandleGlyphFuncType, class InitCharFuncType>
  void DoInitFTCharStringApplyRound(const StdStringView &ssvStr)
{ // Load the character with floor rounding?
  if(FlagIsSet(FF_FLOORADVANCE))
    DoInitFTCharString<HandleGlyphFuncType, InitCharFuncType,
      RoundCheckFunc::Manual<RoundFunc::Floor<GLfloat>>>(ssvStr);
  // Load the character with ceil rounding?
  else if(FlagIsSet(FF_CEILADVANCE))
    DoInitFTCharString<HandleGlyphFuncType, InitCharFuncType,
      RoundCheckFunc::Manual<RoundFunc::Ceil<GLfloat>>>(ssvStr);
  // Load the character with round rounding?
  else if(FlagIsSet(FF_ROUNDADVANCE))
    DoInitFTCharString<HandleGlyphFuncType, InitCharFuncType,
      RoundCheckFunc::Manual<RoundFunc::Round<GLfloat>>>(ssvStr);
  // No rounding (allows subpixel drawing)
  DoInitFTCharString<HandleGlyphFuncType, InitCharFuncType,
    RoundCheckFunc::Manual<RoundFunc::Straight<GLfloat>>>(ssvStr);
}
/* -- Apply rounding functor before entering loop -------------------------- */
template<class HandleGlyphFuncType>
  void DoInitFTCharStringApplyStroker(const StdStringView &ssvStr)
{ // If stroker is loaded?
  if(FtfIsStrokerLoaded())
  { // Load entire stroker outline?
    if(FlagIsSet(FF_STROKETYPE2))
      DoInitFTCharStringApplyRound<HandleGlyphFuncType,
        StrokerCheckFunc::Manual<InitCharFunc::Outline2>>(ssvStr);
    // Load outside of stroker (default)
    else DoInitFTCharStringApplyRound<HandleGlyphFuncType,
      StrokerCheckFunc::Manual<InitCharFunc::Outline1>>(ssvStr);
  } // Load no stroker
  else DoInitFTCharStringApplyRound<HandleGlyphFuncType,
    StrokerCheckFunc::Manual<InitCharFunc::NoOutline>>(ssvStr);
}
/* -- Check if a character needs initialising ------------------------------ */
size_t CheckGlyph(const UtfCodepoint utcChar)
{ return DoCheckGlyph<HandleGlyphFunc::Auto,
    StrokerCheckFunc::Auto<InitCharFunc::NoOutline>,
    RoundCheckFunc::Auto<RoundFunc::Straight<GLfloat>>>(utcChar); }
/* -- Get modified character width at specified position ------------------- */
GLfloat GetCharWidth(const size_t stPos)
  { return (gvData[stPos].GlyphGetAdvance() + GetCharSpacing()) * glfScale; }
/* == EoF =========================================================== EoF == */
