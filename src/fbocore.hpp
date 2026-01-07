/* == FBOCORE.HPP ========================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## Contains the main drawing FBO's and commands. It would be nice to   ## **
** ## have this in the FBO's collector but we can't due to C++            ## **
** ## limitations of initialising static FBO classes when it hasn't been  ## **
** ## defined yet.                                                        ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IFboCore {                   // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IClock::P;             using namespace ICoords::P;
using namespace ICVar::P;              using namespace ICVarDef::P;
using namespace ICVarLib::P;           using namespace IDim::P;
using namespace IEvtMain::P;           using namespace IFboBlend::P;
using namespace IColour::P;            using namespace IFbo::P;
using namespace IGlFW::P;              using namespace ILog::P;
using namespace IOgl::P;               using namespace IShader::P;
using namespace IShaders::P;           using namespace IStd::P;
using namespace IString::P;            using namespace ISysUtil::P;
using namespace ITimer::P;             using namespace IUtil::P;
using namespace Lib::OS::GlFW::Types;
/* ------------------------------------------------------------------------- */
typedef array<Fbo, 2> FboDouble;       // Main and console FBO typedef
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* == Typedefs ============================================================= */
enum DrawState : unsigned int          // Drawing states
{ /* ----------------------------------------------------------------------- */
  DS_NONE,                             // [0] No redrawing requested
  DS_PARTIAL,                          // [1] Draw main FBO to back buffer only
  DS_FULL,                             // [2] Finish console/main, draw to back
};/* ----------------------------------------------------------------------- */
/* == Main FBO class ======================================================= */
class FboCore;                         // Class prototype
static FboCore *cFboCore = nullptr;    // Pointer to global class
class FboCore :                        // The main FBO operations manager
  /* -- Base classes ------------------------------------------------------- */
  public Colour,                       // Backbuffer clear colour
  public FboBlend,                     // Default blending mode
  public DimGLSizei,                   // Main FBO dimensions
  private FboDouble                    // Main and console classes
{ /* -- Private variables -------------------------------------------------- */
  GLfloat          fAspectMin,         // Minimum orthangal matrix ratio
                   fAspectMax;         // Maximum orthangal matrix ratio
  DimGLFloat       dfMatrix;           // Requested matrix dimensions
  DrawState        dsDraw;             // Draw mode
  bool             bSimpleMatrix,      // Use simple not automatic matrix
                   bClearBuffer;       // Clear back buffer?
  /* -- Core FBO's --------------------------------------------------------- */
  Fbo             &fboMain,            // Main FBO class (FboDouble[0])
                  &fboConsole;         // Console FBO class (FboDouble[1])
  /* -- FPS ---------------------------------------------------------------- */
  ClkTimePoint     ctpStart;           // Fps checkpoint
  double           dFps, dFpsSmoothed; // Caluclated FPS value + smoothed
  /* -- Get FBO's -------------------------------------------------- */ public:
  Fbo &FboCoreGetMain() { return fboMain; }
  Fbo &FboCoreGetConsole() { return fboConsole; }
  /* -- Shortcut to the main FBO stage bounds ------------------------------ */
  const CoordsFloat &FboCoreGetMainStage() const { return fboMain.cfStage; }
  const CoordsFloat &FboCoreGetConsoleStage() const
    { return fboConsole.cfStage; }
  /* -- Blits the console FBO to main FBO ---------------------------------- */
  void FboCoreBlitConsoleToMain() { fboMain.FboBlit(fboConsole); }
  /* -- Set main FBO as active FBO to draw too ----------------------------- */
  void FboCoreActivateMain() { fboMain.FboSetActive(); }
  /* -- Draw flags --------------------------------------------------------- */
  DrawState FboCoreGetDraw() const { return dsDraw; }
  bool FboCoreCanDraw() const { return FboCoreGetDraw() == DS_FULL; }
  bool FboCoreCannotDraw() const { return FboCoreGetDraw() == DS_NONE; }
  void FboCoreSetDraw() { dsDraw = DS_FULL; }
  void FboCoreClearDrawPartial() { dsDraw = DS_PARTIAL; }
  void FboCoreClearDraw() { dsDraw = DS_NONE; }
  /* -- Get matrix dimensions ---------------------------------------------- */
  GLfloat FboCoreGetMatrixWidth() const { return dfMatrix.DimGetWidth(); }
  GLfloat FboCoreGetMatrixHeight() const { return dfMatrix.DimGetHeight(); }
  /* -- Get smoothed rendering frames per second --------------------------- */
  double FboCoreGetFPS() const { return dFpsSmoothed; }
  /* -- Reset backbuffer clear colour to colour stored in cvar ------------- */
  void FboCoreResetClearColour()
  { // Reset core framebuffer object colour intensities
    for(Fbo &fboRef : *this) fboRef.ColourReset();
    // Commit the default back buffer clear colour
    cOgl->SetClearColourInt(
      cCVars->CVarsGetInternal<unsigned int>(VID_CLEARCOLOUR));
  }
  /* -- Render the main FBO from the engine thread ------------------------- */
  void FboCoreRender()
  { // Unbind current FBO so we select the back buffer to draw to
    cOgl->BindFBO();
    // Set the first active texture unit
    cOgl->ActiveTexture();
    // Bind the texture attached to the FBO
    cOgl->BindTexture(fboMain.uiFBOtex);
    // Select our basic 3D transform shader
    cOgl->UseProgram(cShaderCore->sh3D.GetProgram());
    // Set the viewport of the FBO size
    cOgl->SetViewport(DimGetWidth(), DimGetHeight());
    // Set the default alpha blending mode
    cOgl->SetBlendIfChanged(*this);
    // Clear back buffer if main FBO has alpha
    if(fboMain.FboIsTransparencyEnabled()) cOgl->SetAndClear(*this);
    // Draw the main FBO to the back buffer
    cOgl->OglDrawArrays();
    // Delete lingering texture and FBO handles
    cOgl->OglPostRender();
    // Swap buffers
    cGlFW->WinSwapGLBuffers();
    // Calculate an average fps
    const ClkTimePoint ctpNow{ cTimer->TimerGetStartTime() };
    // Smooth the value with a 0.01 alpha
    dFps = UtilSmooth(UtilPerSec(ClockDurationToDouble(ctpNow - ctpStart)),
      dFpsSmoothed);
    // Update to the current timepoint
    ctpStart = ctpNow;
  }
  /* -- Sent when the window is resized/main FBO needs autosized --- */ public:
  bool FboCoreAutoMatrix(const GLfloat fWidth, const GLfloat fHeight,
    const bool bForce)
  { // Some variables we'll need to do some calculations
    GLfloat fAspect, fAddWidth, fLeft, fTop, fRight, fBottom;
    // Use a simple matrix. The simple matrix means that the aspect ratio
    // will not be automatically calculated based on window size.
    if(bSimpleMatrix)
    { // Allow any aspect ratio
      fAspect = fWidth / fHeight;
      fAddWidth = fLeft = fTop = 0.0f;
      fRight = fWidth;
      fBottom = fHeight;
    } // Don't use simple matrix?
    else
    { // Calculate aspect ratio. We want to clamp the ratio the guests
      // specified minimum and maximum aspect ratio and to also keep the size
      // within the specified multiple value to prevent cracks appearing in
      // between tiles.
      fAspect = UtilClamp(DimGetWidth<GLfloat>() / DimGetHeight<GLfloat>(),
        fAspectMin, fAspectMax) / 1.33333333333333333333333f;
      // For some unknown reason we could be sent invalid values so we need to
      // make sure we ignore this value to prevent error handlers triggering.
      if(fAspect != fAspect) fAspect = 1.0f;
      // Calculate additional width over the 4:3 aspect ratio
      fAddWidth = UtilMaximum(((fWidth * fAspect) - fWidth) / 2.0f, 0.0f);
      // Calculate bounds for stage clamping to integral values
      fLeft = floorf(-fAddWidth);
      fRight = floorf(fWidth + fAddWidth);
      // Set top and bottom stage bounds
      fTop = 0.0f;
      fBottom = fHeight;
    } // Test if the viewport didn't change?
    const bool bUnchanged =
      StdIsFloatEqual(fLeft, FboCoreGetMainStage().CoordsGetLeft()) &&
      StdIsFloatEqual(fTop, FboCoreGetMainStage().CoordsGetTop()) &&
      StdIsFloatEqual(fRight, FboCoreGetMainStage().CoordsGetRight()) &&
      StdIsFloatEqual(fBottom, FboCoreGetMainStage().CoordsGetBottom());
    // Set stage bounds for drawing
    fboMain.FboSetMatrix(fLeft, fTop, fRight, fBottom);
    // Calculate matrix dimensions
    double dIW = static_cast<double>(fRight) - static_cast<double>(fLeft),
           dIH = static_cast<double>(fBottom) - static_cast<double>(fTop),
           // Viewport dimensions as double
           dOW = DimGetWidth<double>(),
           dOH = DimGetHeight<double>();
    // Stretch matrix into viewport to reveal discardable pixels
    UtilStretchToOuter(dOW, dOH, dIW, dIH);
    // Calculate effective scaled viewport width
    const double dW = dIW - dOW;
    // Update the drawing position so the main FBO triangles are in the
    // centre of the screen to try and maintain 1:1 pixel ratio even though
    // on non-4:3 resolutions, you might not be able to scale the matrix
    // width into a scaled 16:9 aspect ratio (e.g. 426x240 isn't exactly
    // 16:9 and would cause one or two extra pixels on 2560x1440).
    fboMain.FboItemSetVertex(
      static_cast<GLfloat>(-1.0 - (-dOW        / dW)),  1.0f,
      static_cast<GLfloat>( 1.0 + (-(dIW - dW) / dW)), -1.0f);
    // If the stage unchanged? Return failure if we're not forcing the change
    if(bUnchanged) { if(!bForce) return false; }
    // Stage changed so save matrix size incase viewport changes
    else dfMatrix.DimSet(fWidth, fHeight);
    // Calculate new FBO width and height
    const GLsizei siFBOWidth = static_cast<GLsizei>(fRight - fLeft),
                  siFBOHeight = static_cast<GLsizei>(fBottom - fTop);
    // No point changing anything if the bounds are the same and if the FBO
    // needs updating? Also ignore if opengl isn't initialised as the GLfW FB
    // reset window event might be sent before we've initialised it!
    if((siFBOWidth != static_cast<GLsizei>(fboMain.CoordsGetRight()) ||
        siFBOHeight != static_cast<GLsizei>(fboMain.CoordsGetBottom()) ||
        bForce) && cOgl->IsGLInitialised())
    { // Re-initialise the main framebuffer (the reservation is ignored)
      fboMain.FboInit("main", siFBOWidth, siFBOHeight, 0, 0);
      // Store new data for drawing main FBO to the back buffer into VRAM
      cOgl->MainFBOInitDrawData(fboMain.FboItemGetDataSize(),
        fboMain.FboItemGetData(), fboMain.FboItemGetTCPos(),
        fboMain.FboItemGetVPos(), fboMain.FboItemGetCPos());
      // Reset fps based on highest possible frames per second
      ctpStart = cTimer->TimerGetStartTime();
      dFps = dFpsSmoothed = UtilPerSec(
        UtilMaximum(cTimer->TimerGetLimit(), cOgl->GetLimit()));
      // Log computations
      cLog->LogDebugExSafe(
        "FboCore matrix reinit to $x$[$] (D=$x$,A=$<$-$>,AW=$,S=$:$:$:$).",
        fboMain.CoordsGetRight(), fboMain.CoordsGetBottom(),
        StrFromRatio(siFBOWidth, siFBOHeight),
          fWidth, fHeight, fAspect, fAspectMin, fAspectMax, fAddWidth,
          fLeft, fTop, fRight, fBottom);
      // Everything changed
      return true;
    } // Re-initialisation required?
    cLog->LogDebugExSafe(
      "FboCore matrix recalculated! (D=$x$,A=$<$-$>,AW=$,S=$:$:$:$).",
      fWidth, fHeight, fAspect, fAspectMin, fAspectMax, fAddWidth,
      fLeft, fTop, fRight, fBottom);
    // Only bounds were changed
    return false;
  }
  /* -- Sent when the window is resized ------------------------------------ */
  bool FboCoreAutoViewport(const GLsizei siWidth, const GLsizei siHeight,
    const bool bForce=false)
  { // Return if the viewport size did not change
    if(DimGetWidth() == siWidth && DimGetHeight() == siHeight) return false;
    // Set the new viewport and log the result
    DimSet(UtilMaximum(siWidth, 1), UtilMaximum(siHeight, 1));
    cLog->LogDebugExSafe("FboCore set new viewport of $x$.",
      DimGetWidth(), DimGetHeight());
    // Update matrix because the window's aspect ratio may have changed and
    // if the FBO changed then inform lua scripts that they should redraw any
    // FBO's they are managing and return the result.
    if(!FboCoreAutoMatrix(FboCoreGetMatrixWidth(),
      FboCoreGetMatrixHeight(), bForce))
        return false;
    cEvtMain->Add(EMC_LUA_REDRAW);
    return true;
  }
  /* -- Initialise the console FBO for the console object ------------------ */
  void FboCoreInitConsole(const size_t stTriangles)
  { // Estimate amount of triangles that would fit in the console and if
    // we have a non-zero value?
    fboConsole.FboInit("console",
      fboMain.DimGetWidth(), fboMain.DimGetHeight(), stTriangles, 2);
   }
  /* -- Temporary de-init all guest, console and main FBO objects ---------- */
  void FboCoreDeInit()
  { // De-init guest FBO objects
    FboDeInit();
    // De-init core FBO objects
    StdForEach(seq, rbegin(), rend(), [](Fbo &fboRef){ fboRef.FboDeInit(); });
  }
  /* -- Initialise FBO's using a different constructor ---------- */ protected:
  FboCore() :
    /* -- Initialisers ----------------------------------------------------- */
    FboDouble{{ { GL_RGB8,  true },    // Initialise main FBO (no register)
                { GL_RGBA8, true } }}, // Initialise console FBO (no register)
    fAspectMin(1.0f),                  // Minimum aspect ratio init by CVar
    fAspectMax(2.0f),                  // Minimum aspect ratio init by CVar
    dsDraw(DS_FULL),                   // Redraw the back buffer
    bSimpleMatrix(false),              // Simple matrix init by CVar
    bClearBuffer(false),               // Clear buffer init by CVar
    fboMain(front()),                  // Init reference to main FBO
    fboConsole(back()),                // Init reference to console FBO
    ctpStart{ cd0 },                   // Initialise FPS timepoint
    dFps{ 0.0 }, dFpsSmoothed{ 0.0 }   // Initialise calculated FPS
    /* -- Set pointer to global class and main FBO ------------------------- */
    { cFboCore = this; cFbos->fboMain = &fboMain; }
  /* -- Set main FBO float reserve --------------------------------- */ public:
  CVarReturn FboCoreSetFloatReserve(const size_t stCount)
    { return BoolToCVarReturn(UtilReserveList(fboMain.ftvActive, stCount)); }
  /* -- Set main FBO command reserve --------------------------------------- */
  CVarReturn FboCoreSetCommandReserve(const size_t stCount)
    { return BoolToCVarReturn(UtilReserveList(fboMain.fcvActive, stCount)); }
  /* -- Set main FBO filters (cvar event) ---------------------------------- */
  CVarReturn FboCoreSetFilter(const OglFilterEnum ofeV)
  { // Check value
    if(ofeV >= OF_MAX) return DENY;
    // Set filtering of main and console framebuffer objects
    for(Fbo &fboRef : *this) fboRef.FboSetFilter(ofeV);
    // Accept the change anyway if opengl not initialised
    if(cOgl->IsGLNotInitialised()) return ACCEPT;
    // Commit the filters
    for(Fbo &fboRef : *this) fboRef.FboCommitFilter();
    // Redraw the main frame-buffer
    FboCoreSetDraw();
    // Return success
    return ACCEPT;
  }
  /* -- Set back buffer clear ---------------------------------------------- */
  CVarReturn FboCoreSetBackBufferClear(const bool bState)
    { return CVarSimpleSetInt(bClearBuffer, bState); }
  /* -- Set back buffer clear colour --------------------------------------- */
  CVarReturn FboCoreSetBackBufferClearColour(const unsigned int uiColour)
    { ColourSetInt(uiColour); return ACCEPT; }
  /* -- Set minimum orthagonal matrix ratio -------------------------------- */
  CVarReturn FboCoreSetMinAspect(const GLfloat fMinimum)
    { return CVarSimpleSetIntNLG(fAspectMin, fMinimum, 1.0f, fAspectMax); }
  /* -- Set maximum orthagonal matrix ratio -------------------------------- */
  CVarReturn FboCoreSetMaxAspect(const GLfloat fMaximum)
    { return CVarSimpleSetIntNLG(fAspectMax, fMaximum, fAspectMin, 2.0f); }
  /* -- Set simple matrix -------------------------------------------------- */
  CVarReturn FboCoreSetSimpleMatrix(const bool bState)
    { return CVarSimpleSetInt(bSimpleMatrix, bState); }
};/* ----------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
