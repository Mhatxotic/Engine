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
using namespace ICoords::P;            using namespace ICVar::P;
using namespace ICVarDef::P;           using namespace ICVarLib::P;
using namespace IConDef::P;            using namespace IConsole::P;
using namespace IEvtMain::P;           using namespace IFbo::P;
using namespace IFboCore::P;           using namespace IFont::P;
using namespace IFtf::P;               using namespace IGlFW::P;
using namespace IHelper::P;            using namespace IImage::P;
using namespace IImageDef::P;          using namespace ILog::P;
using namespace IOgl::P;               using namespace IStd::P;
using namespace IString::P;            using namespace ISystem::P;
using namespace ISysUtil::P;           using namespace ITexture::P;
using namespace IUtf::P;               using namespace Lib::OS::GlFW::Types;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public namespace
/* ========================================================================= */
class ConGfx;                          // Class prototype
static ConGfx *cConGfx = nullptr;      // Pointer to global class
class ConGfx :                         // Members initially private
  /* -- Base classes ------------------------------------------------------- */
  private InitHelper                   // Initialisation helper
{ /* -- Private variables -------------------------------------------------- */
  const uint32_t   ulFontColour;       // Console input text colour
  uint32_t         ulTexColour;        // Console background texture colour
  GLfloat          glfFontScale,       // Console font scale
                   glfConHeight,       // Console height
                   glfLetterSpacing,   // Console text letter spacing
                   glfLineSpacing,     // Console text line spacing
                   glfFontWidth,       // Console text width
                   glfFontHeight;      // Console text height
  Texture          texConsole;         // Console background texture
  Font             fonConsole;         // Console font
  char             cCursor;            // Cursor character to use
  /* -- Return console and main FBO from FboCore class --------------------- */
  Fbo &ConGfxGetFBO() { return cFboCore->FboCoreGetConsole(); }
  const CoordsGLFloat &ConGfxGetFBOStage() const
    { return cFboCore->FboCoreGetConsoleStage(); }
  Fbo &ConGfxGetMainFBO() { return cFboCore->FboCoreGetMain(); }
  const CoordsGLFloat &ConGfxGetMainFBOStage() const
    { return cFboCore->FboCoreGetMainStage(); }
  /* -- Init console font -------------------------------------------------- */
  void ConGfxInitFont()
  { // Load font. Cvars won't set the font size initially so we have to do
    // it manually. If we init cvars after, then destructor will crash because
    // the cvars havn't been initialised
    Ftf fFTFont;
    fFTFont.InitFile(cCVars->GetStrInternal(CON_FONT), glfFontWidth,
      glfFontHeight, 96, 96, 0.0f);
    ConGfxGetFontRef().InitFontFtf(fFTFont,
      cCVars->CVarsGetInternal<GLuint>(CON_FONTTEXSIZE),
      cCVars->CVarsGetInternal<GLuint>(CON_FONTPADDING), OF_L_L_MM_L,
      ConGfxGetFontRef());
    // Get minimum precache range and if valid?
    if(const Codepoint cCharMin =
         cCVars->CVarsGetInternal<unsigned>(CON_FONTPCMIN))
      // Get maximum precache range and return if valid?
      if(const Codepoint cCharMax =
           cCVars->CVarsGetInternal<unsigned>(CON_FONTPCMAX))
        // If maximum is above the minimum? Pre-cache the characters range
        if(cCharMax >= cCharMin)
          ConGfxGetFontRef().InitFTCharRange(cCharMin, cCharMax);
    // Set default font parameters
    ConGfxRestoreDefaultProperties();
    // LUA not allowed to deallocate this font!
    ConGfxGetFontRef().LockSet();
  }
  /* -- Init console texture ----------------------------------------------- */
  void ConGfxInitTexture()
  { // Get console texture filename, use a solid if not specified
    const StdString strCT{ cCVars->GetStrInternal(CON_BGTEXTURE) };
    if(strCT.empty())
    { // Create simple image for solid colour and load it into a texture
      Image imgData{ 0xFFFFFFFF };
      ConGfxGetTextureRef().InitTextureImage(imgData, 0, 0, 0, 0, OF_N_N);
    } // Else filename specified so load it!
    else
    { // Load image from disk and load it into a texture
      Image imgData{ strCT, IL_NONE };
      ConGfxGetTextureRef().InitTextureImage(imgData, 0, 0, 0, 0, OF_L_L_MM_L);
    } // LUA will not be allowed to garbage collect this texture class!
    ConGfxGetTextureRef().LockSet();
  }
  /* -- Commit default text scale ------------------------------------------ */
  void ConGfxCommitScale()
    { ConGfxGetFontRef().SetSize(glfFontScale); }
  /* -- Commit default letter spacing -------------------------------------- */
  void ConGfxCommitLetterSpacing()
    { ConGfxGetFontRef().SetCharSpacing(glfLetterSpacing); }
  /* -- Commit default line spacing ---------------------------------------- */
  void ConGfxCommitLineSpacing()
    { ConGfxGetFontRef().SetLineSpacing(glfLineSpacing); }
  /* -- Reset defaults (for lreset) ---------------------------------------- */
  void ConGfxRestoreDefaultProperties()
  { // Restore default settings from cvar registry
    ConGfxCommitLetterSpacing();
    ConGfxCommitScale();
    ConGfxCommitLineSpacing();
  }
  /* -- Get console textures --------------------------------------- */ public:
  Texture &ConGfxGetTextureRef() { return texConsole; }
  Font &ConGfxGetFontRef() { return fonConsole; }
  Texture *ConGfxGetTexture() { return &ConGfxGetTextureRef(); }
  Font *ConGfxGetFont() { return &ConGfxGetFontRef(); }
  /* -- Do set visibility -------------------------------------------------- */
  bool ConGfxSetVisible(const bool bState)
  { // Set the visibility state and redraw the main FBO if not visible
    const bool bResult = cConsole->SetVisible(bState);
    if(!bResult) cFboCore->FboCoreSetDraw();
    return bResult;
  }
  /* -- Set console visibility lock status --------------------------------- */
  void ConGfxSetLocked(const bool bState)
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
  void ConGfxLeaveResetEnvironment()
  { // Enable and show console, and set full-screen
    ConGfxSetVisible(true);
    ConGfxSetLocked(true);
    ConGfxSetHeight(1.0f);
    // Restore console font properties
    ConGfxRestoreDefaultProperties();
  }
  /* -- Reset entering defaults (for lreset) ------------------------------- */
  void ConGfxEnterResetEnvironment()
  { // Disable console (if set), hide the console (force), and restore size
    ConGfxSetLocked(cConsole->FlagIsSet(CF_LOCKEDGLOBAL));
    cConsole->HideConsole();
    ConGfxSetHeight(cCVars->CVarsGetInternal<GLfloat>(CON_HEIGHT));
    // Restore console font properties
    ConGfxRestoreDefaultProperties();
  }
  /* -- Mouse wheel moved event -------------------------------------------- */
  void ConGfxOnMouseWheel(const double, const double dY)
  { // Move console if moving up
    if(dY > 0.0) cConsole->MoveLogUp();
    // Move console if moving down
    else if(dY < 0.0) cConsole->MoveLogDown();
  }
  /* -- Check if console key pressed in console ---------------------------- */
  bool ConGfxIsKeyHandled(const int iKey, const int iState, const int iMod)
  { // Return unhandled if console not visible or a key was released
    if(cConsole->IsNotVisible() || iState == GLFW_RELEASE) return false;
    // Control key, which key?
    if(iMod & GLFW_MOD_CONTROL) switch(iKey)
    { // Test keys with control held
#if defined(MACOS)
      // Because MacOS keyboards don't have an 'insert' key.
      case GLFW_KEY_DELETE:
        // Toggle cursor mode, set new cursor to display and redraw the console
        cConsole->ToggleCursorMode();
        cCursor = cConsole->FlagIsSet(CF_INSERT) ? '|' : '_';
        cConsole->SetRedraw();
        // Key is handled
        return true;
#endif
      // Copy text from clipboard?
      case GLFW_KEY_C:
      { // Build command by appending text before and after the cursor
        const StdString strCmd{ cConsole->InputText() };
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
        // Break if there is input
        if(cConsole->InputNotEmpty()) break;
        // Hide the console and set to ignore an escape keypress if succeeded
        if(ConGfxSetVisible(false)) cConsole->FlagSet(CF_IGNOREESC);
        // Do not do anything else
        return true;
      // Nothing interesting break to pass to console class
      default: break;
    } // We didnt handle any keys so let the actual console handle them
    cConsole->OnKeyPress(iKey, iState, iMod);
    // We handled this key so do not dispatch it to scripts
    return true;
  }
  /* -- Redraw the console FBO if the console contents changed ------------- */
  void ConGfxRender()
  { // Shift queued console lines
    cConsole->MoveQueuedLines();
    // Return if console is not visible
    if(cConsole->IsNotVisible()) return;
    // Return if theres nothing to redraw else clear redraw flag
    if(cConsole->GetRedrawFlags().FlagIsClear(RD_GRAPHICS)) return;
    cConsole->GetRedrawFlags().FlagClear(RD_GRAPHICS);
    // Set main FBO to draw to
    ConGfxGetFBO().FboSetActive();
    // Update matrix same as the main FBO
    ConGfxGetFBO().FboSetMatrix(0.0f, 0.0f,
      ConGfxGetMainFBO().CoordsGetX2(),
      ConGfxGetMainFBO().CoordsGetY2());
    // Set drawing position
    const GLfloat fYAdj =
      ConGfxGetMainFBOStage().CoordsGetY2() * (1.0f - glfConHeight);
    ConGfxGetFBO().FboItemSetVertex(
      ConGfxGetMainFBOStage().CoordsGetX1(),
      ConGfxGetMainFBOStage().CoordsGetY1() - fYAdj,
      ConGfxGetMainFBOStage().CoordsGetX2(),
      ConGfxGetMainFBOStage().CoordsGetY2() - fYAdj);
    // Set console texture colour and blit the console background
    ConGfxGetTextureRef().FboItemSetQuadRGBAInt(ulTexColour);
    ConGfxGetTextureRef().BlitLTRB(0, 0,
      ConGfxGetFBOStage().CoordsGetX1(),
      ConGfxGetFBOStage().CoordsGetY1(),
      ConGfxGetFBOStage().CoordsGetX2(),
      ConGfxGetFBOStage().CoordsGetY2());
    // Set console input text colour
    ConGfxGetFontRef().FboItemSetQuadRGBAInt(ulFontColour);
    // Restore spacing and scale as well
    ConGfxRestoreDefaultProperties();
    // Get below baseline height
    const GLfloat glfBL = (ConGfxGetFBOStage().CoordsGetY2() -
      ConGfxGetFontRef().GetBaselineBelow('g')) +
      ConGfxGetFontRef().GetLineSpacing();
    // Draw input text and subtract the height drawn from Y position
    GLfloat glfY = glfBL -
      ConGfxGetFontRef().PrintWU(ConGfxGetFBOStage().CoordsGetX1(), glfBL,
        ConGfxGetFBOStage().CoordsGetX2(),
        ConGfxGetFontRef().gldfScale.DimGetWidth(),
        StrFormat(">$\rc000000ff$\rr$", cConsole->GetConsoleBegin(), cCursor,
          cConsole->GetConsoleEnd()));
    // For each line or until we clip the top of the screen, print the text
    for(ConLinesConstRevIt
          clcriIt{ cConsole->GetConBufPos() };
          clcriIt != cConsole->GetConBufPosEnd() && glfY > 0.0f;
        ++clcriIt)
    { // Get reference to console line data structure
      const ConLine &clD = *clcriIt;
      // Set text foreground colour with opaqueness already set above
      ConGfxGetFontRef().FboItemSetQuadRGBInt(ConColourToRGB(clD.ccColour));
      // Draw the text and move upwards of the height that was used
      glfY -= ConGfxGetFontRef().PrintWU(ConGfxGetFBOStage().CoordsGetX1(),
        glfY, ConGfxGetFBOStage().CoordsGetX2(),
        ConGfxGetFontRef().gldfScale.DimGetWidth(), clD.strLine);
    } // Finish and render
    ConGfxGetFBO().FboFinishAndRender();
    // Make sure the main FBO is updated if it is visible
    cFboCore->FboCoreSetDraw();
  }
  /* -- Render the console to main FBO if visible -------------------------- */
  void ConGfxRenderToMain()
    { if(cConsole->IsVisible()) cFboCore->FboCoreBlitConsoleToMain(); }
  /* -- DeInit console texture and font ------------------------------------ */
  void ConGfxDeInitTextureAndFont()
    { ConGfxGetTextureRef().DeInit(); ConGfxGetFontRef().DeInit(); }
  /* -- Reload console texture and font ------------------------------------ */
  void ConGfxReInitTextureAndFont()
    { ConGfxGetTextureRef().ReloadTexture();
      ConGfxGetFontRef().ReloadTexture(); }
  /* -- Estimate number of triangles needed to fill screen ----------------- */
  size_t ConGfxGetTrianglesEstimate()
  { // Calculate characters per row
    const GLfloat
      glfMainWidth     = ConGfxGetMainFBO().DimGetWidth<GLfloat>(),
      glfFoScWidth     = glfFontWidth * glfFontScale,
      glfSpacingWidth  = ConGfxGetFontRef().GetCharSpacingScale(),
      glfFoSpWidth     = glfFoScWidth + glfSpacingWidth,
      glfCharRow       = glfMainWidth / glfFoSpWidth * 2.0f,
      glfMainHeight    = ConGfxGetMainFBO().DimGetHeight<GLfloat>(),
      glfFoScHeight    = glfFontHeight * glfFontScale,
      glfSpacingHeight = ConGfxGetFontRef().GetLineSpacing(),
      glfFoSpHeight    = glfFoScHeight + glfSpacingHeight,
      glfCharColumn    = glfMainHeight / glfFoSpHeight * 2.0f;
    // Then return total number of triangles on screen
    return static_cast<size_t>(glfCharColumn * glfCharRow);
  }
  /* -- Init framebuffer object -------------------------------------------- */
  void ConGfxInitFBO()
  { // Ignore if terminal mode
    if(ConGfxGetFontRef().IsNotInitialised()) return;
    // Get reference to main FBO and initialise it
    cFboCore->FboCoreInitConsole(ConGfxGetTrianglesEstimate());
    // Redraw FBO
    cConsole->SetRedraw();
  }
  /* -- Init console font and texture -------------------------------------- */
  void ConGfxInitConsoleFontAndTexture()
    { ConGfxInitFont(); ConGfxInitTexture(); }
  /* -- Print a string using textures -------------------------------------- */
  void ConGfxInit()
  { // Class intiialised
    IHInitialise();
    // Log progress
    cLog->LogDebugSafe("ConGraph initialising...");
    // We have graphics now too
    cConsole->GetDefaultRedrawFlags().FlagSet(RD_GRAPHICS);
    // Load console texture
    ConGfxInitConsoleFontAndTexture();
    // Redraw the console
    cConsole->SetRedraw();
    // Log progress
    cLog->LogDebugSafe("ConGraph initialised.");
  }
  /* -- DeInit ------------------------------------------------------------- */
  void ConGfxDeInit()
  { // Ignore if already deinitialised
    if(IHNotDeInitialise()) return;
    // Log progress
    cLog->LogDebugSafe("ConGraph de-initialising...");
    // Remove graphics support flag
    cConsole->FlagClear(RD_GRAPHICS);
    // Unload console texture and font.
    ConGfxDeInitTextureAndFont();
    // Remove font ftf
    ConGfxGetFontRef().ftfData.DeInit();
    // Log progress
    cLog->LogInfoSafe("ConGraph de-initialised.");
  }
  /* -- Constructor --------------------------------------------- */ protected:
  explicit ConGfx() :
    /* -- Initialisers ----------------------------------------------------- */
    InitHelper{ __FUNCTION__ },        // Init helper function name
    ulFontColour(                      // Set input text colour
      ConColourToRGB(COLOUR_YELLOW) |  // - Lookup RGB value for yellow
      0xFF000000),                     // - Force opaqueness
    ulTexColour(0x00000000),           // No background colour
    glfFontScale(0.0f),                // No font scale
    glfConHeight(0.0f),                // No console height
    glfLetterSpacing(0.0f),            // No text letter spacing
    glfLineSpacing(0.0f),              // No text line spacing
    glfFontWidth(0.0f),                // No text width
    glfFontHeight(0.0f),               // No text height
    texConsole{ IP_TEXTURE },          // Console texture on stand-by
    fonConsole{ IP_FONT },             // Console font on stand-by
    cCursor('|')                       // Cursor shape
    /* -- Set global pointer to static class ------------------------------- */
    { cConGfx = this; }
  /* -- Set console background colour ------------------------------ */ public:
  CVarReturn ConGfxTextBackgroundColourModified(const uint32_t ulNewColour)
    { return CVarSimpleSetInt(ulTexColour, ulNewColour); }
  /* -- Set console text scale --------------------------------------------- */
  CVarReturn ConGfxTextScaleModified(const GLfloat glfNScale)
  { // Failed if supplied scale is not in range
    if(!CVarToBoolReturn(CVarSimpleSetIntNLG(glfFontScale,
      glfNScale, 0.01f, 16.0f))) return DENY;
    // If font initialised?
    if(ConGfxGetFontRef().IsInitialised())
    { // Set console text size so the scaled size is properly calculated
      ConGfxCommitScale();
      // Estimate amount of triangles that would fit in the console
      ConGfxGetFBO().FboReserve(ConGfxGetTrianglesEstimate(), 2);
    } // Succeeded reglardless of font availability
    return ACCEPT;
  }
  /* -- Set text letter spacing -------------------------------------------- */
  CVarReturn ConGfxTextLetterSpacingModified(const GLfloat glfNSpacing)
  { // Failed if supplied spacing is not in range
    if(!CVarToBoolReturn(CVarSimpleSetIntNLG(glfLetterSpacing,
      glfNSpacing, -256.0f, 256.0f))) return DENY;
    // Set console text line spacing if texture available
    ConGfxCommitLetterSpacing();
    // Succeeded reglardless of font availability
    return ACCEPT;
  }
  /* -- Set text line spacing ---------------------------------------------- */
  CVarReturn ConGfxTextLineSpacingModified(const GLfloat glfNSpacing)
  { // Failed if supplied spacing is not in range
    if(!CVarToBoolReturn(CVarSimpleSetIntNLG(glfLineSpacing,
      glfNSpacing, -256.0f, 256.0f))) return DENY;
    // Set console text line spacing if texture available
    ConGfxCommitLineSpacing();
    // Succeeded reglardless of font availability
    return ACCEPT;
  }
  /* -- Set text width ----------------------------------------------------- */
  CVarReturn ConGfxTextWidthModified(const GLfloat glfNWidth)
    { return CVarSimpleSetIntNG(glfFontWidth, glfNWidth, 4096.0f); }
  /* -- Set text height ---------------------------------------------------- */
  CVarReturn ConGfxTextHeightModified(const GLfloat glfNHeight)
    { return CVarSimpleSetIntNG(glfFontHeight, glfNHeight, 4096.0f); }
  /* -- Console font flags modfiied ---------------------------------------- */
  CVarReturn ConGfxFontFlagsModified(const ImageFlagsType iftFlags)
  { // Return failure if invalid flags else set flags and return success
    if(iftFlags != IL_NONE && (iftFlags & ~FF_MASK)) return DENY;
    ConGfxGetFontRef().FlagClearAndSet(FF_MASK, ImageFlagsConst{ iftFlags });
    return ACCEPT;
  }
  /* -- Set Console status ------------------------------------------------- */
  CVarReturn ConGfxCantDisableModified(const bool bState)
  { // Update flag and disabled status then return success
    cConsole->FlagSetOrClear(CF_LOCKEDGLOBAL, bState);
    ConGfxSetLocked(bState);
    return ACCEPT;
  }
  /* -- Set console height ------------------------------------------------- */
  CVarReturn ConGfxSetHeight(const GLfloat glfNHeight)
    { return CVarSimpleSetIntNLG(glfConHeight, glfNHeight, 0.1f, 1.0f); }
};/* ----------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
