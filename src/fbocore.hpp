/* == FBOCORE.HPP ========================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## Contains the main drawing Fbo's and commands. It would be nice to   ## **
** ## have this in the Fbo's collector but we can't due to C++            ## **
** ## limitations of initialising static Fbo classes when it hasn't been  ## **
** ## defined yet.                                                        ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IFboCore {                   // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IClock::P;             using namespace IColour::P;
using namespace ICoords::P;            using namespace ICVar::P;
using namespace ICVarDef::P;           using namespace ICVarLib::P;
using namespace IDim::P;               using namespace IEvtMain::P;
using namespace IFboBlend::P;          using namespace IFbo::P;
using namespace IFrame::P;             using namespace IGlFW::P;
using namespace ILog::P;               using namespace IOgl::P;
using namespace IShader::P;            using namespace IShaders::P;
using namespace IStd::P;               using namespace IString::P;
using namespace ISysUtil::P;           using namespace IUtil::P;
using namespace Lib::OS::GlFW::Types;
/* ------------------------------------------------------------------------- */
using FboDouble = StdArray<Fbo, 2>;    // Main and console Fbo typedef
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* == Typedefs ============================================================= */
enum DrawState : unsigned              // Drawing states
{ /* ----------------------------------------------------------------------- */
  DS_NONE,                             // [0] No redrawing requested
  DS_PARTIAL,                          // [1] Draw main Fbo to back buffer only
  DS_FULL,                             // [2] Finish console/main, draw to back
};/* ----------------------------------------------------------------------- */
/* == Main Fbo class ======================================================= */
class FboCore;                         // Class prototype
static FboCore *cFboCore = nullptr;    // Pointer to global class
class FboCore :                        // The main Fbo operations manager
  /* -- Base classes ------------------------------------------------------- */
  public Colour,                       // Backbuffer clear colour
  public FboBlend,                     // Default blending mode
  public DimGLSizei,                   // Main Fbo dimensions
  private FboDouble                    // Main and console classes
{ /* -- Private variables -------------------------------------------------- */
  GLfloat          glfAspectMin,       // Minimum orthangal matrix ratio
                   glfAspectMax;       // Maximum orthangal matrix ratio
  DimGLFloat       dglfMatrix;         // Requested matrix dimensions
  DrawState        dsDraw;             // Draw mode
  bool             bSimpleMatrix,      // Use simple not automatic matrix
                   bClearBuffer;       // Clear back buffer?
  /* -- Core Fbo's --------------------------------------------------------- */
  Fbo             &fboMain,            // Main Fbo class (FboDouble[0])
                  &fboConsole;         // Console Fbo class (FboDouble[1])
  /* -- FPS ---------------------------------------------------------------- */
  ClkTimePoint     ctpStart;           // Fps checkpoint
  double           dFps, dFpsSmoothed; // Caluclated FPS value + smoothed
  /* -- Set main Fbo as active Fbo to draw too ------------------ */ protected:
  void FboCoreActivateMain() { fboMain.FboSetActive(); }
  /* -- Render the main Fbo from the engine thread ------------------------- */
  void FboCoreRender()
  { // Unbind current Fbo so we select the back buffer to draw to
    cOgl->BindFbo();
    // Set the first active texture unit
    cOgl->ActiveTexture();
    // Bind the texture attached to the Fbo
    cOgl->BindTexture(fboMain.gluFbotex);
    // Select our basic 3D transform shader
    cOgl->UseProgram(cShaderCore->sh3D.GetProgram());
    // Set the viewport of the Fbo size
    cOgl->SetViewport(DimGetWidth(), DimGetHeight());
    // Set the default alpha blending mode
    cOgl->SetBlendIfChanged(*this);
    // Clear back buffer if main Fbo has alpha
    if(fboMain.FboIsTransparencyEnabled()) cOgl->SetAndClear(*this);
    // Draw the main Fbo to the back buffer
    cOgl->OglDrawArrays();
    // Delete lingering texture and Fbo handles
    cOgl->OglPostRender();
    // Swap buffers
    cGlFW->WinSwapGLBuffers();
    // Calculate an average fps
    const ClkTimePoint ctpNow{ cFrame->FrameGetStartTime() };
    // Smooth the value with a 0.01 alpha
    dFps = UtilSmooth(UtilPerSec(ClockDurationToDouble(ctpNow - ctpStart)),
      dFpsSmoothed);
    // Update to the current timepoint
    ctpStart = ctpNow;
  }
  /* -- Reset core fbo settings from a reset ------------------------------- */
  void FboCoreResetEnvironment()
  { // Reset main fbo and console fbo timers
    FboCoreGetConsole().FboResetFinishAndFlush();
    FboCoreGetMain().FboResetFinishAndFlush();
    // Reset core framebuffer object colour intensities
    for(Fbo &fboRef : *this) fboRef.ColourReset();
    // Commit the default back buffer clear colour
    cOgl->SetClearColourInt(
      cCVars->CVarsGetInternal<unsigned>(VID_CLEARCOLOUR));
    // Reactivate main framebuffer for drawing
    FboCoreActivateMain();
    // Make sure main Fbo is cleared
    FboCoreSetDraw();
  }
  /* -- Get Fbo's -------------------------------------------------- */ public:
  Fbo &FboCoreGetMain() { return fboMain; }
  Fbo &FboCoreGetConsole() { return fboConsole; }
  /* -- Shortcut to the main Fbo stage bounds ------------------------------ */
  const CoordsGLFloat &FboCoreGetMainStage() const
    { return fboMain.FboGetStageConst(); }
  const CoordsGLFloat &FboCoreGetConsoleStage() const
    { return fboConsole.FboGetStageConst(); }
  /* -- Blits the console Fbo to main Fbo ---------------------------------- */
  void FboCoreBlitConsoleToMain() { fboMain.FboBlit(fboConsole); }
  /* -- Draw flags --------------------------------------------------------- */
  DrawState FboCoreGetDraw() const { return dsDraw; }
  bool FboCoreCanDraw() const { return FboCoreGetDraw() == DS_FULL; }
  bool FboCoreCannotDraw() const { return FboCoreGetDraw() == DS_NONE; }
  void FboCoreSetDraw(const DrawState dsNDraw = DS_FULL) { dsDraw = dsNDraw; }
  void FboCoreClearDrawPartial() { FboCoreSetDraw(DS_PARTIAL); }
  void FboCoreClearDraw() { FboCoreSetDraw(DS_NONE); }
  /* -- Get matrix dimensions ---------------------------------------------- */
  GLfloat FboCoreGetMatrixWidth() const { return dglfMatrix.DimGetWidth(); }
  GLfloat FboCoreGetMatrixHeight() const { return dglfMatrix.DimGetHeight(); }
  /* -- Get smoothed rendering frames per second --------------------------- */
  double FboCoreGetFPS() const { return dFpsSmoothed; }
  /* -- Sent when the window is resized/main Fbo needs autosized --- */ public:
  bool FboCoreAutoMatrix(const GLfloat glfWidth, const GLfloat glfHeight,
    const bool bForce)
  { // Some variables we'll need to do some calculations
    GLfloat glfAspect, glfAddWidth, glfLeft, glfTop, glfRight, glfBottom;
    // Use a simple matrix. The simple matrix means that the aspect ratio
    // will not be automatically calculated based on window size.
    if(bSimpleMatrix)
    { // Allow any aspect ratio
      glfAspect = glfWidth / glfHeight;
      glfAddWidth = glfLeft = glfTop = 0.0f;
      glfRight = glfWidth;
      glfBottom = glfHeight;
    } // Don't use simple matrix?
    else
    { // Calculate aspect ratio. We want to clamp the ratio the guests
      // specified minimum and maximum aspect ratio and to also keep the size
      // within the specified multiple value to prevent cracks appearing in
      // between tiles.
      glfAspect = UtilClamp(DimGetWidth<GLfloat>() / DimGetHeight<GLfloat>(),
        glfAspectMin, glfAspectMax) / 1.33333333333333333333333f;
      // For some unknown reason we could be sent invalid values so we need to
      // make sure we ignore this value to prevent error handlers triggering.
      if(glfAspect != glfAspect) glfAspect = 1.0f;
      // Calculate additional width over the 4:3 aspect ratio
      glfAddWidth =
        UtilMaximum(((glfWidth * glfAspect) - glfWidth) / 2.0f, 0.0f);
      // Calculate bounds for stage clamping to integral values
      glfLeft = floorf(-glfAddWidth);
      glfRight = floorf(glfWidth + glfAddWidth);
      // Set top and bottom stage bounds
      glfTop = 0.0f;
      glfBottom = glfHeight;
    } // Test if the viewport didn't change?
    const bool bUnchanged =
      StdIsFloatEqual(glfLeft, FboCoreGetMainStage().CoordsGetX1()) &&
      StdIsFloatEqual(glfTop, FboCoreGetMainStage().CoordsGetY1()) &&
      StdIsFloatEqual(glfRight, FboCoreGetMainStage().CoordsGetX2()) &&
      StdIsFloatEqual(glfBottom, FboCoreGetMainStage().CoordsGetY2());
    // Set stage bounds for drawing
    fboMain.FboSetMatrix(glfLeft, glfTop, glfRight, glfBottom);
    // Calculate matrix dimensions
    double dIW = static_cast<double>(glfRight) - static_cast<double>(glfLeft),
           dIH = static_cast<double>(glfBottom) - static_cast<double>(glfTop),
           // Viewport dimensions as double
           dOW = DimGetWidth<double>(),
           dOH = DimGetHeight<double>();
    // Stretch matrix into viewport to reveal discardable pixels
    UtilStretchToOuter(dOW, dOH, dIW, dIH);
    // Calculate effective scaled viewport width
    const double dW = dIW - dOW;
    // Update the drawing position so the main Fbo triangles are in the
    // centre of the screen to try and maintain 1:1 pixel ratio even though
    // on non-4:3 resolutions, you might not be able to scale the matrix
    // width into a scaled 16:9 aspect ratio (e.g. 426x240 isn't exactly
    // 16:9 and would cause one or two extra pixels on 2560x1440).
    fboMain.FboItemSetVertex(
      static_cast<GLfloat>(-1.0 - ( -dOW       / dW)),  1.0f,
      static_cast<GLfloat>( 1.0 + (-(dIW - dW) / dW)), -1.0f);
    // If the stage unchanged? Return failure if we're not forcing the change
    if(bUnchanged) { if(!bForce) return false; }
    // Stage changed so save matrix size incase viewport changes
    else dglfMatrix.DimSet(glfWidth, glfHeight);
    // Calculate new Fbo width and height
    const GLsizei glsiFboWidth = static_cast<GLsizei>(glfRight - glfLeft),
                  glsiFboHeight = static_cast<GLsizei>(glfBottom - glfTop);
    // No point changing anything if the bounds are the same and if the Fbo
    // needs updating? Also ignore if opengl isn't initialised as the GLfW FB
    // reset window event might be sent before we've initialised it!
    if((glsiFboWidth != static_cast<GLsizei>(fboMain.CoordsGetX2()) ||
        glsiFboHeight != static_cast<GLsizei>(fboMain.CoordsGetY2()) ||
        bForce) && cOgl->IsGLInitialised())
    { // Re-initialise the main framebuffer (the reservation is ignored)
      fboMain.FboInit("main", glsiFboWidth, glsiFboHeight, 0, 0);
      // Store new data for drawing main Fbo to the back buffer into VRAM
      cOgl->MainFboInitDrawData(fboMain.FboItemGetDataSize(),
        fboMain.FboItemGetData(), fboMain.FboItemGetTCPos(),
        fboMain.FboItemGetVPos(), fboMain.FboItemGetCPos());
      // Reset fps based on highest possible frames per second
      ctpStart = cFrame->FrameGetStartTime();
      dFps = dFpsSmoothed = UtilPerSec(
        UtilMaximum(cFrame->FrameGetLimit(), cOgl->GetLimit()));
      // Log computations
      cLog->LogDebugExSafe(
        "FboCore matrix reinit to $x$[$] (D=$x$,A=$<$-$>,AW=$,S=$:$:$:$).",
        fboMain.CoordsGetX2(), fboMain.CoordsGetY2(),
        StrFromRatio(glsiFboWidth, glsiFboHeight),
          glfWidth, glfHeight, glfAspect, glfAspectMin, glfAspectMax,
          glfAddWidth, glfLeft, glfTop, glfRight, glfBottom);
      // Everything changed
      return true;
    } // Re-initialisation required?
    cLog->LogDebugExSafe(
      "FboCore matrix recalculated! (D=$x$,A=$<$-$>,AW=$,S=$:$:$:$).",
      glfWidth, glfHeight, glfAspect, glfAspectMin, glfAspectMax, glfAddWidth,
      glfLeft, glfTop, glfRight, glfBottom);
    // Only bounds were changed
    return false;
  }
  /* -- Sent when the window is resized ------------------------------------ */
  bool FboCoreAutoViewport(const GLsizei glsiWidth, const GLsizei glsiHeight,
    const bool bForce=false)
  { // Return if the viewport size did not change
    if(DimIsEqual(glsiWidth, glsiHeight)) return false;
    // Set the new viewport and log the result
    DimSet(UtilMaximum(glsiWidth, 1), UtilMaximum(glsiHeight, 1));
    cLog->LogDebugExSafe("FboCore set new viewport of $x$.",
      DimGetWidth(), DimGetHeight());
    // Update matrix because the window's aspect ratio may have changed and
    // if the Fbo changed then inform lua scripts that they should redraw any
    // Fbo's they are managing and return the result.
    if(!FboCoreAutoMatrix(FboCoreGetMatrixWidth(),
      FboCoreGetMatrixHeight(), bForce))
        return false;
    cEvtMain->Add(EMC_LUA_REDRAW);
    return true;
  }
  /* -- Initialise the console Fbo for the console object ------------------ */
  void FboCoreInitConsole(const size_t stTriangles)
  { // Estimate amount of triangles that would fit in the console and if
    // we have a non-zero value?
    fboConsole.FboInit("console",
      fboMain.DimGetWidth(), fboMain.DimGetHeight(), stTriangles, 2);
   }
  /* -- Temporary de-init all guest, console and main Fbo objects ---------- */
  void FboCoreDeInit()
  { // De-init guest Fbo objects
    FboDeInit();
    // De-init core Fbo objects
    StdForEach(seq, rbegin(), rend(), [](Fbo &fboRef){ fboRef.FboDeInit(); });
  }
  /* -- Initialise Fbo's using a different constructor ---------- */ protected:
  FboCore() :
    /* -- Initialisers ----------------------------------------------------- */
    FboDouble{{{ GL_RGB8,  true },     // Initialise main Fbo (no register)
               { GL_RGBA8, true }}},   // Initialise console Fbo (no register)
    glfAspectMin(1.0f),                // Minimum aspect ratio init by CVar
    glfAspectMax(2.0f),                // Minimum aspect ratio init by CVar
    dsDraw(DS_NONE),                   // Redraw the back buffer
    bSimpleMatrix(false),              // Simple matrix init by CVar
    bClearBuffer(false),               // Clear buffer init by CVar
    fboMain(front()),                  // Init reference to main Fbo
    fboConsole(back()),                // Init reference to console Fbo
    ctpStart{ cd0 },                   // Initialise FPS timepoint
    dFps{ 0.0 }, dFpsSmoothed{ 0.0 }   // Initialise calculated FPS
    /* -- Set pointer to global class and main Fbo ------------------------- */
    { cFboCore = this; cFbos->fboMain = &fboMain; }
  /* -- Set main Fbo float reserve --------------------------------- */ public:
  CVarReturn FboCoreSetFloatReserve(const size_t stCount)
    { return BoolToCVarReturn(UtilReserveList(fboMain.ftvActive, stCount)); }
  /* -- Set main Fbo command reserve --------------------------------------- */
  CVarReturn FboCoreSetCommandReserve(const size_t stCount)
    { return BoolToCVarReturn(UtilReserveList(fboMain.fcvActive, stCount)); }
  /* -- Set main Fbo filters (cvar event) ---------------------------------- */
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
  CVarReturn FboCoreSetBackBufferClearColour(const unsigned uColour)
    { ColourSetInt(uColour); return ACCEPT; }
  /* -- Set minimum orthagonal matrix ratio -------------------------------- */
  CVarReturn FboCoreSetMinAspect(const GLfloat fMinimum)
    { return CVarSimpleSetIntNLG(glfAspectMin, fMinimum, 1.0f, glfAspectMax); }
  /* -- Set maximum orthagonal matrix ratio -------------------------------- */
  CVarReturn FboCoreSetMaxAspect(const GLfloat fMaximum)
    { return CVarSimpleSetIntNLG(glfAspectMax, fMaximum, glfAspectMin, 2.0f); }
  /* -- Set simple matrix -------------------------------------------------- */
  CVarReturn FboCoreSetSimpleMatrix(const bool bState)
    { return CVarSimpleSetInt(bSimpleMatrix, bState); }
};/* ----------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
