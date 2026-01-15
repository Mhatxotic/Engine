/* == CONGRAPH.HPP ========================================================= **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## This module handles the OpenGL graphical rendering side for the     ## **
** ## console.                                                            ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IConGraph {                  // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace ICVar::P;              using namespace ICVarDef::P;
using namespace ICVarLib::P;           using namespace IConDef::P;
using namespace IConsole::P;           using namespace IEvtMain::P;
using namespace IFbo::P;               using namespace IFboCore::P;
using namespace IFont::P;              using namespace IFtf::P;
using namespace IGlFW::P;              using namespace IHelper::P;
using namespace IImage::P;             using namespace IImageDef::P;
using namespace ILog::P;               using namespace IOgl::P;
using namespace IStd::P;               using namespace IString::P;
using namespace ISystem::P;            using namespace ISysUtil::P;
using namespace ITexture::P;           using namespace Lib::OS::GlFW::Types;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public namespace
/* ========================================================================= */
class ConGraphics;                     // Class prototype
static ConGraphics *cConGraphics = nullptr; // Pointer to global class
class ConGraphics :                    // Members initially private
  /* -- Base classes ------------------------------------------------------- */
  private InitHelper                   // Initialisation helper
{ /* -- Private variables -------------------------------------------------- */
  const uint32_t   ulFgColour;         // Console input text colour
  uint32_t         ulBgColour;         // Console background texture colour
  GLfloat          fTextScale,         // Console font scale
                   fConsoleHeight,     // Console height
                   fTextLetterSpacing, // Console text letter spacing
                   fTextLineSpacing,   // Console text line spacing
                   fTextWidth,         // Console text width
                   fTextHeight;        // Console text height
  Texture          ctConsole;          // Console background texture
  Font             cfConsole;          // Console font
  char             cCursor;            // Cursor character to use
  /* -- Return console FBO from FboCore class ------------------------------ */
  Fbo &GetConsoleFBO() { return cFboCore->fboConsole; }
  Fbo &GetMainFBO() { return cFboCore->fboMain; }
  /* -- Toggle insert and overwrite mode ----------------------------------- */
  void ToggleCursorMode()
  { // Toggle cursor mode, set new cursor to display and redraw the console
    cConsole->ToggleCursorMode();
    cCursor = cConsole->FlagIsSet(CF_INSERT) ? '|' : '_';
    cConsole->SetRedraw();
  }
  /* -- Init console font -------------------------------------------------- */
  void InitConsoleFont()
  { // Load font. Cvars won't set the font size initially so we have to do
    // it manually. If we init cvars after, then destructor will crash because
    // the cvars havn't been initialised
    Ftf fFTFont;
    fFTFont.InitFile(cCVars->GetStrInternal(CON_FONT), fTextWidth,
      fTextHeight, 96, 96, 0.0f);
    GetFontRef().InitFontFtf(fFTFont,
      cCVars->GetInternal<GLuint>(CON_FONTTEXSIZE),
      cCVars->GetInternal<GLuint>(CON_FONTPADDING), OF_L_L_MM_L,
        GetFontRef());
    // Get minimum precache range and if valid?
    if(const unsigned int uiCharMin =
      cCVars->GetInternal<unsigned int>(CON_FONTPCMIN))
      // Get maximum precache range and return if valid?
      if(const unsigned int uiCharMax =
        cCVars->GetInternal<unsigned int>(CON_FONTPCMAX))
          // If maximum is above the minimum? Pre-cache the characters range
          if(uiCharMax >= uiCharMin)
            GetFontRef().InitFTCharRange(uiCharMin, uiCharMax);
    // Set default font parameters
    RestoreDefaultProperties();
    // LUA not allowed to deallocate this font!
    GetFontRef().LockSet();
  }
  /* -- Init console texture ----------------------------------------------- */
  void InitConsoleTexture()
  { // Get console texture filename, use a solid if not specified
    const string strCT{ cCVars->GetStrInternal(CON_BGTEXTURE) };
    if(strCT.empty())
    { // Create simple image for solid colour and load it into a texture
      Image imgData{ 0xFFFFFFFF };
      GetTextureRef().InitTextureImage(imgData, 0, 0, 0, 0, OF_N_N);
    } // Else filename specified so load it!
    else
    { // Load image from disk and load it into a texture
      Image imgData{ strCT, IL_NONE };
      GetTextureRef().InitTextureImage(imgData, 0, 0, 0, 0, OF_L_L_MM_L);
    } // LUA will not be allowed to garbage collect this texture class!
    GetTextureRef().LockSet();
  }
  /* -- Commit default text scale ------------------------------------------ */
  void CommitScale()
    { GetFontRef().SetSize(fTextScale); }
  /* -- Commit default letter spacing -------------------------------------- */
  void CommitLetterSpacing()
    { GetFontRef().SetCharSpacing(fTextLetterSpacing); }
  /* -- Commit default line spacing ---------------------------------------- */
  void CommitLineSpacing()
    { GetFontRef().SetLineSpacing(fTextLineSpacing); }
  /* -- Reset defaults (for lreset) ---------------------------------------- */
  void RestoreDefaultProperties()
  { // Restore default settings from cvar registry
    CommitLetterSpacing();
    CommitScale();
    CommitLineSpacing();
  }
  /* -- Get console textures --------------------------------------- */ public:
  Texture &GetTextureRef() { return ctConsole; }
  Font &GetFontRef() { return cfConsole; }
  Texture *GetTexture() { return &GetTextureRef(); }
  Font *GetFont() { return &GetFontRef(); }
  /* -- Do set visibility -------------------------------------------------- */
  bool SetVisible(const bool bState)
  { // Set the visibility state and redraw the main FBO if not visible
    const bool bResult = cConsole->SetVisible(bState);
    if(!bResult) cFboCore->SetDraw();
    return bResult;
  }
  /* -- Set console visibility lock status --------------------------------- */
  void SetLocked(const bool bState)
  { // Return if requested state is the same.
    if(cConsole->FlagIsEqualToBool(CF_LOCKED, bState)) return;
    // Keep locked/hidden if there's a terminal or user wants gfxcon disabled.
    if(cConsole->IsHidingGraphicalConsole())
      return cConsole->FlagSetAndClear(CF_LOCKED, CF_ENABLED);
    // Current value is disabled?
    if(cConsole->FlagIsClear(CF_LOCKED))
    { // Enable visibility control
      cConsole->FlagSet(CF_LOCKED);
      // Log that the console visibility control was disabled.
      return cLog->LogDebugSafe(
        "Console visibility control has been disabled.");
    } // Disability visibility control
    cConsole->FlagClear(CF_LOCKED);
    // Log that the console visibility control was enabled
    cLog->LogDebugSafe("Console visibility control has been enabled.");
  }
  /* -- Reset leaving defaults (for lreset) -------------------------------- */
  void LeaveResetEnvironment()
  { // Enable and show console, and set full-screen
    SetVisible(true);
    SetLocked(true);
    SetHeight(1.0f);
    // Restore console font properties
    RestoreDefaultProperties();
  }
  /* -- Reset entering defaults (for lreset) ------------------------------- */
  void EnterResetEnvironment()
  { // Disable and hide console, and restore size
    SetLocked(cConsole->FlagIsSet(CF_LOCKEDGLOBAL));
    SetVisible(false);
    SetHeight(cCVars->GetInternal<GLfloat>(CON_HEIGHT));
    // Restore console font properties
    RestoreDefaultProperties();
  }
  /* -- Mouse wheel moved event -------------------------------------------- */
  void OnMouseWheel(const double, const double dY)
  { // Move console if moving up
    if(dY > 0.0) cConsole->MoveLogUp();
    // Move console if moving down
    else if(dY < 0.0) cConsole->MoveLogDown();
  }
  /* -- Keyboard key pressed event ----------------------------------------- */
  void OnKeyPress(const int iKey, const int iAction, const int iMods)
  { // Key released? Ignore
    if(iAction == GLFW_RELEASE) return;
    // Control key, which key?
    if(iMods & GLFW_MOD_CONTROL) switch(iKey)
    { // Test keys with control held
#if defined(MACOS)
      // Because MacOS keyboards don't have an 'insert' key.
      case GLFW_KEY_DELETE: return ToggleCursorMode();
#endif
      // Copy text from clipboard?
      case GLFW_KEY_C:
      { // Build command by appending text before and after the cursor
        const string strCmd{ cConsole->InputText() };
        // Copy to clipboard if not empty
        if(!strCmd.empty()) cGlFW->WinSetClipboardString(strCmd);
        // Break to pass to console class
        break;
      } // Paste text from clipboard?
      case GLFW_KEY_V: cEvtMain->Add(EMC_INP_PASTE); break;
      // Nothing interesting break to pass to console class
      default: break;
    } // Normal key, which key?
    else switch(iKey)
    { // Test keys with no modifiers held
      case GLFW_KEY_ESCAPE:
      { // If there is no text in the input buffer?
        if(cConsole->InputEmpty())
        { // Hide the console and set to ignore an escape keypress if succeeded
          if(SetVisible(false)) cConsole->FlagSet(CF_IGNOREESC);
        } // Let console class clear the input
        else break;
        // Do not do anything else
        return;
      } // Nothing interesting break to pass to console class
      default: break;
    } // We didnt handle any keys so let the actual console handle them
    cConsole->OnKeyPress(iKey, iAction, iMods);
  }
  /* -- Check if console key pressed in console ---------------------------- */
  bool IsKeyHandled(const int iKey, const int iState, const int iMod)
  { // Return if console not visible
    if(cConsole->IsNotVisible()) return false;
    // Add normal key pressed. Since GLFW inconveniently gives us 3 int
    // parameters, we need to pack 2 ints together. Luckily, GLFW_RELEASE etc
    // is only 8-bit, we'll pack the modifiers with this value.
    OnKeyPress(iKey, iState, iMod);
    // We handled this key so do not dispatch it to scripts
    return true;
  }
  /* -- Redraw the console FBO if the console contents changed ------------- */
  void Render()
  { // Shift queued console lines
    cConsole->MoveQueuedLines();
    // Return if theres nothing to redraw else clear redraw flag
    if(cConsole->GetRedrawFlags().FlagIsClear(RD_GRAPHICS)) return;
    cConsole->GetRedrawFlags().FlagClear(RD_GRAPHICS);
    // Set main FBO to draw to
    GetConsoleFBO().FboSetActive();
    // Update matrix same as the main FBO
    GetConsoleFBO().FboSetMatrix(0.0f, 0.0f,
      GetMainFBO().CoordsGetRight(), GetMainFBO().CoordsGetBottom());
    // Set drawing position
    const GLfloat fYAdj =
      GetMainFBO().cfStage.CoordsGetBottom() * (1.0f - fConsoleHeight);
    GetConsoleFBO().FboItemSetVertex(GetMainFBO().cfStage.CoordsGetLeft(),
      GetMainFBO().cfStage.CoordsGetTop() - fYAdj,
      GetMainFBO().cfStage.CoordsGetRight(),
      GetMainFBO().cfStage.CoordsGetBottom() - fYAdj);
    // Set console texture colour and blit the console background
    GetTextureRef().FboItemSetQuadRGBAInt(ulBgColour);
    GetTextureRef().BlitLTRB(0, 0, GetConsoleFBO().cfStage.CoordsGetLeft(),
      GetConsoleFBO().cfStage.CoordsGetTop(),
      GetConsoleFBO().cfStage.CoordsGetRight(),
      GetConsoleFBO().cfStage.CoordsGetBottom());
    // Set console input text colour
    GetFontRef().FboItemSetQuadRGBAInt(ulFgColour);
    // Restore spacing and scale as well
    CommitLetterSpacing();
    CommitLineSpacing();
    CommitScale();
    // Get below baseline height
    const GLfloat fBL = (GetConsoleFBO().cfStage.CoordsGetBottom() -
      GetFontRef().GetBaselineBelow('g')) + GetFontRef().fLineSpacing;
    // Draw input text and subtract the height drawn from Y position
    GLfloat fY = fBL -
      GetFontRef().PrintWU(GetConsoleFBO().cfStage.CoordsGetLeft(), fBL,
        GetConsoleFBO().cfStage.CoordsGetRight(),
        GetFontRef().dfScale.DimGetWidth(),
        reinterpret_cast<const GLubyte*>(StrFormat(">$\rc000000ff$\rr$",
        cConsole->GetConsoleBegin(), cCursor,
        cConsole->GetConsoleEnd()).data()));
    // For each line or until we clip the top of the screen, print the text
    for(ConLinesConstRevIt clI{ cConsole->GetConBufPos() };
                           clI != cConsole->GetConBufPosEnd() && fY > 0.0f;
                         ++clI)
    { // Get reference to console line data structure
      const ConLine &clD = *clI;
      // Set text foreground colour with opaqueness already set above
      GetFontRef().FboItemSetQuadRGBInt(uiNDXtoRGB[clD.ccColour]);
      // Draw the text and move upwards of the height that was used
      fY -= GetFontRef().PrintWU(GetConsoleFBO().cfStage.CoordsGetLeft(), fY,
        GetConsoleFBO().cfStage.CoordsGetRight(),
          GetFontRef().dfScale.DimGetWidth(), reinterpret_cast<const GLubyte*>
            (clD.strLine.data()));
    } // Finish and render
    GetConsoleFBO().FboFinishAndRender();
    // Make sure the main FBO is updated
    cFboCore->SetDraw();
  }
  /* -- Render the console to main FBO if visible -------------------------- */
  void RenderToMain()
    { if(cConsole->IsVisible()) cFboCore->BlitConsoleToMain(); }
  /* -- Show the console and render it and render the FBO to main FBO ------ */
  void RenderNow()
  { // Show the console, render it to main frame buffer and blit it
    SetVisible(true);
    // Redraw console if not redrawn
    Render();
    // Bit console FBO to main
    cFboCore->BlitConsoleToMain();
  }
  /* -- DeInit console texture and font ------------------------------------ */
  void DeInitTextureAndFont()
    { GetTextureRef().DeInit(); GetFontRef().DeInit(); }
  /* -- Reload console texture and font ------------------------------------ */
  void ReInitTextureAndFont()
    { GetTextureRef().ReloadTexture(); GetFontRef().ReloadTexture(); }
  /* -- Estimate number of triangles needed to fill screen ----------------- */
  size_t GetTrianglesEstimate()
  { // Calculate characters per row
    const GLfloat fMainWidth     = GetMainFBO().DimGetWidth<GLfloat>(),
                  fFontWidth     = fTextWidth * fTextScale,
                  fSpacingWidth  = GetFontRef().fCharSpacingScale,
                  fFoSpWidth     = fFontWidth + fSpacingWidth,
                  fCharRow       = fMainWidth / fFoSpWidth * 2.0f,
                  fMainHeight    = GetMainFBO().DimGetHeight<GLfloat>(),
                  fFontHeight    = fTextHeight * fTextScale,
                  fSpacingHeight = GetFontRef().fLineSpacing,
                  fFoSpHeight    = fFontHeight + fSpacingHeight,
                  fCharColumn    = fMainHeight / fFoSpHeight * 2.0f;
    // Then return total number of triangles on screen
    return static_cast<size_t>(fCharColumn * fCharRow);
  }
  /* -- Init framebuffer object -------------------------------------------- */
  void InitFBO()
  { // Ignore if terminal mode
    if(GetFontRef().IsNotInitialised()) return;
    // Get reference to main FBO and initialise it
    cFboCore->InitConsoleFBO(GetTrianglesEstimate());
    // Redraw FBO
    cConsole->SetRedraw();
  }
  /* -- Init console font and texture -------------------------------------- */
  void InitConsoleFontAndTexture() { InitConsoleFont(); InitConsoleTexture(); }
  /* -- Print a string using textures -------------------------------------- */
  void Init()
  { // Class intiialised
    IHInitialise();
    // Log progress
    cLog->LogDebugSafe("ConGraph initialising...");
    // We have graphics now too
    cConsole->GetDefaultRedrawFlags().FlagSet(RD_GRAPHICS);
    // Load console texture
    InitConsoleFontAndTexture();
    // Redraw the console
    cConsole->SetRedraw();
    // Log progress
    cLog->LogDebugSafe("ConGraph initialised.");
  }
  /* -- DeInit ------------------------------------------------------------- */
  void DeInit()
  { // Ignore if already deinitialised
    if(IHNotDeInitialise()) return;
    // Log progress
    cLog->LogDebugSafe("ConGraph de-initialising...");
    // Remove graphics support flag
    cConsole->FlagClear(RD_GRAPHICS);
    // Unload console texture and font.
    DeInitTextureAndFont();
    // Remove font ftf
    GetFontRef().ftfData.DeInit();
    // Log progress
    cLog->LogInfoSafe("ConGraph de-initialised.");
  }
  /* -- Constructor --------------------------------------------- */ protected:
  explicit ConGraphics() :
    /* -- Initialisers ----------------------------------------------------- */
    InitHelper{ __FUNCTION__ },        // Init helper function name
    ulFgColour(                        // Set input text colour
      uiNDXtoRGB[COLOUR_YELLOW] |      // - Lookup RGB value for yellow
      0xFF000000),                     // - Force opaqueness
    ulBgColour(0x00000000),            // No background colour
    fTextScale(0.0f),                  // No font scale
    fConsoleHeight(0.0f),              // No console height
    fTextLetterSpacing(0.0f),          // No text letter spacing
    fTextLineSpacing(0.0f),            // No text line spacing
    fTextWidth(0.0f),                  // No text width
    fTextHeight(0.0f),                 // No text height
    ctConsole{ IP_TEXTURE },           // Console texture on stand-by
    cfConsole{ IP_FONT },              // Console font on stand-by
    cCursor('|')                       // Cursor shape
    /* -- Set global pointer to static class ------------------------------- */
    { cConGraphics = this; }
  /* -- Set console background colour ------------------------------ */ public:
  CVarReturn TextBackgroundColourModified(const uint32_t ulNewColour)
    { return CVarSimpleSetInt(ulBgColour, ulNewColour); }
  /* -- Set console text scale --------------------------------------------- */
  CVarReturn TextScaleModified(const GLfloat fNewScale)
  { // Failed if supplied scale is not in range
    if(!CVarToBoolReturn(CVarSimpleSetIntNLG(fTextScale,
      fNewScale, 0.01f, 16.0f))) return DENY;
    // If font initialised?
    if(GetFontRef().IsInitialised())
    { // Set console text size so the scaled size is properly calculated
      CommitScale();
      // Estimate amount of triangles that would fit in the console
      GetConsoleFBO().FboReserve(GetTrianglesEstimate(), 2);
    } // Succeeded reglardless of font availability
    return ACCEPT;
  }
  /* -- Set text letter spacing -------------------------------------------- */
  CVarReturn TextLetterSpacingModified(const GLfloat fNewSpacing)
  { // Failed if supplied spacing is not in range
    if(!CVarToBoolReturn(CVarSimpleSetIntNLG(fTextLetterSpacing,
      fNewSpacing, -256.0f, 256.0f))) return DENY;
    // Set console text line spacing if texture available
    CommitLetterSpacing();
    // Succeeded reglardless of font availability
    return ACCEPT;
  }
  /* -- Set text line spacing ---------------------------------------------- */
  CVarReturn TextLineSpacingModified(const GLfloat fNewSpacing)
  { // Failed if supplied spacing is not in range
    if(!CVarToBoolReturn(CVarSimpleSetIntNLG(fTextLineSpacing,
      fNewSpacing, -256.0f, 256.0f))) return DENY;
    // Set console text line spacing if texture available
    CommitLineSpacing();
    // Succeeded reglardless of font availability
    return ACCEPT;
  }
  /* -- Set text width ----------------------------------------------------- */
  CVarReturn TextWidthModified(const GLfloat fNewWidth)
    { return CVarSimpleSetIntNG(fTextWidth, fNewWidth, 4096.0f); }
  /* -- Set text height ---------------------------------------------------- */
  CVarReturn TextHeightModified(const GLfloat fNewHeight)
    { return CVarSimpleSetIntNG(fTextHeight, fNewHeight, 4096.0f); }
  /* -- Console font flags modfiied ---------------------------------------- */
  CVarReturn ConsoleFontFlagsModified(const ImageFlagsType iftFlags)
  { // Return failure if invalid flags else set flags and return success
    if(iftFlags != IL_NONE && (iftFlags & ~FF_MASK)) return DENY;
    GetFontRef().FlagClearAndSet(FF_MASK, ImageFlagsConst{ iftFlags });
    return ACCEPT;
  }
  /* -- Set Console status ------------------------------------------------- */
  CVarReturn CantDisableModified(const bool bState)
  { // Update flag and disabled status then return success
    cConsole->FlagSetOrClear(CF_LOCKEDGLOBAL, bState);
    SetLocked(bState);
    return ACCEPT;
  }
  /* -- Set console height ------------------------------------------------- */
  CVarReturn SetHeight(const GLfloat fHeight)
    { return CVarSimpleSetIntNLG(fConsoleHeight, fHeight, 0.1f, 1.0f); }
};/* ----------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
