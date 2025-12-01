/* == FBOCORE.HPP ========================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## Contains the main drawing fbo's and commands. It would be nice to   ## **
** ## have this in the FBOs collector but we can't due to C++ limitations ## **
** ## of initialising static FBO classes when it hasn't been defined yet. ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IFboCore {                   // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IClock::P;             using namespace ICVar::P;
using namespace ICVarDef::P;           using namespace ICVarLib::P;
using namespace IDim::P;               using namespace IEvtMain::P;
using namespace IFboDef::P;            using namespace IFbo::P;
using namespace IGlFW::P;              using namespace ILog::P;
using namespace IOgl::P;               using namespace IShader::P;
using namespace IShaders::P;           using namespace IStd::P;
using namespace IString::P;            using namespace ISysUtil::P;
using namespace ITimer::P;             using namespace IUtil::P;
using namespace Lib::OS::GlFW::Types;
/* ------------------------------------------------------------------------- */
typedef array<Fbo, 2> FboDouble;       // Main and console fbo typedef
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* == Main fbo class ======================================================= */
class FboCore;                         // Class prototype
static FboCore *cFboCore = nullptr;    // Pointer to global class
class FboCore :                        // The main fbo operations manager
  /* -- Base classes ------------------------------------------------------- */
  public FboColour,                    // Backbuffer clear colour
  public FboBlend,                     // Default blending mode
  public DimGLSizei,                   // Main frame buffer object dimensions
  private FboDouble                    // Core frame buffer objects
{ /* -- Private variables -------------------------------------------------- */
  GLfloat          fAspectMin,         // Minimum orthangal matrix ratio
                   fAspectMax;         // Maximum orthangal matrix ratio
  DimGLFloat       dfMatrix;           // Requested matrix dimensions
  bool             bDraw,              // Should we draw the main fbo flag
                   bSimpleMatrix,      // Use simple not automatic matrix
                   bClearBuffer;       // Clear back buffer?
  /* -- Core fbos -------------------------------------------------- */ public:
  Fbo             &fboMain,            // Main fbo class (FboDouble[0])
                  &fboConsole;         // Console fbo class (FboDouble[1])
  /* -- Render timings ----------------------------------------------------- */
  ClockInterval<>  ciCurrent;          // One second interval to calculate fps
  unsigned int     uiFPS, uiFPSCur;    // Frames per second (last / current)
  /* -- Draw flags --------------------------------------------------------- */
  bool CanDraw() const { return bDraw; }
  bool CannotDraw() const { return !CanDraw(); }
  void SetDraw() { bDraw = true; }
  void ClearDraw() { bDraw = false; }
  /* -- Get members -------------------------------------------------------- */
  GLfloat GetMatrixWidth() const { return dfMatrix.DimGetWidth(); }
  GLfloat GetMatrixHeight() const { return dfMatrix.DimGetHeight(); }
  /* -- Reset backbuffer clear colour to colour stored in cvar ------------- */
  void ResetClearColour()
  { // Reset core framebuffer object colour intensities
    for(Fbo &fboRef : *this) fboRef.FboResetClearColour();
    // Commit the default back buffer clear colour
    cOgl->SetClearColourInt(
      cCVars->GetInternal<unsigned int>(VID_CLEARCOLOUR));
  }
  /* -- Render the main fbo from the engine thread ------------------------- */
  void Render()
  { // Finish and add the main fbo to the render list and render all fbos
    FinishMain();
    FboRender();
    // Flush the pipeline to prevent memory leak and wait if not drawing
    if(CannotDraw()) { cOgl->Flush(); return cTimer->TimerForceWait(); }
    // Clear redraw flag
    ClearDraw();
    // Unbind current FBO so we select the back buffer to draw to
    cOgl->BindFBO();
    // Set the first active texture unit
    cOgl->ActiveTexture();
    // Bind the texture attached to the fbo
    cOgl->BindTexture(fboMain.uiFBOtex);
    // Select our basic 3D transform shader
    cOgl->UseProgram(cShaderCore->sh3D.GetProgram());
    // Set the viewport of the FBO size
    cOgl->SetViewport(DimGetWidth(), DimGetHeight());
    // Set the default alpha blending mode
    cOgl->SetBlendIfChanged(*this);
    // Clear back buffer if main fbo has alpha
    if(fboMain.FboIsTransparencyEnabled()) cOgl->SetAndClear(*this);
    // Set normal fill poly mode
    cOgl->SetPolygonMode(GL_FILL);
    // Buffer the interlaced triangle data
    cOgl->BufferStaticData(fboMain.FboItemGetDataSize(),
      fboMain.FboItemGetData());
    // Specify format of the interlaced triangle data
    cOgl->VertexAttribPointer(A_COORD, stCompsPerCoord, 0,
      fboMain.FboItemGetTCPos());
    cOgl->VertexAttribPointer(A_VERTEX, stCompsPerPos, 0,
      fboMain.FboItemGetVPos());
    cOgl->VertexAttribPointer(A_COLOUR, stCompsPerColour, 0,
      fboMain.FboItemGetCPos());
 // Using MacOS?
#if defined(MACOS)
    // This locking code is required to fix a major crash bug in Ventura 13.3+.
    // See https://github.com/glfw/glfw/issues/1997 for more information.
    using namespace Lib::OS::GlFW::NSGL;
    // Get the current NSGL context and lock it. Note there is nothing to throw
    // in this routine so it is safe to use this as-is.
    CGLContextObj cglcoLock = CGLGetCurrentContext();
    CGLLockContext(cglcoLock);
    // Blit the two triangles
    cOgl->DrawArraysTriangles(stTwoTriangles);
    // Context is unlocked when exiting this scope
    CGLUnlockContext(cglcoLock);
#else
    // Blit the two triangles
    cOgl->DrawArraysTriangles(stTwoTriangles);
#endif
    // Swap buffers
    cGlFW->WinSwapGLBuffers();
    // Update memory
    cOgl->UpdateVRAMAvailable();
    // Clear any existing errors
    cOgl->GetError();
    // Frames rendered
    ++uiFPSCur;
    // Return if we've not reached one second yet
    if(!ciCurrent.CITriggerStrict()) return;
    // Set frames per second
    uiFPS = uiFPSCur;
    // Reset frames per second this frame
    uiFPSCur = 0;
  }
  /* -- Blits the console fbo to main fbo ---------------------------------- */
  void BlitConsoleToMain() { fboMain.FboBlit(fboConsole); }
  /* -- Finish main fbo and add it to render list -------------------------- */
  void FinishMain() { fboMain.FboFinishAndRender(); }
  /* -- Set main fbo as active fbo to draw too ----------------------------- */
  void ActivateMain() { fboMain.FboSetActive(); }
  /* -- Called from main tick incase we need to keep catching up frames ---- */
  void RenderFbosAndFlushMain() { FboRender(); fboMain.FboFlush(); }
  /* -- Sent when the window is resized/main fbo needs autosized --- */ public:
  bool AutoMatrix(const GLfloat fWidth, const GLfloat fHeight,
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
      StdIsFloatEqual(fLeft, fboMain.ffcStage.GetCoLeft()) &&
      StdIsFloatEqual(fTop, fboMain.ffcStage.GetCoTop()) &&
      StdIsFloatEqual(fRight, fboMain.ffcStage.GetCoRight()) &&
      StdIsFloatEqual(fBottom, fboMain.ffcStage.GetCoBottom());
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
    // Update the drawing position so the main fbo triangles are in the
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
    // Calculate new fbo width and height
    const GLsizei siFBOWidth = static_cast<GLsizei>(fRight - fLeft),
                  siFBOHeight = static_cast<GLsizei>(fBottom - fTop);
    // No point changing anything if the bounds are the same and if the fbo
    // needs updating? Also ignore if opengl isn't initialised as the GLfW FB
    // reset window event might be sent before we've initialised it!
    if((siFBOWidth != static_cast<GLsizei>(fboMain.GetCoRight()) ||
        siFBOHeight != static_cast<GLsizei>(fboMain.GetCoBottom()) ||
        bForce) && cOgl->IsGLInitialised())
    { // Re-initialise the main framebuffer
      fboMain.FboInit("main", siFBOWidth, siFBOHeight);
      // Log computations
      cLog->LogDebugExSafe(
        "FboCore matrix reinit to $x$[$] (D=$x$,A=$<$-$>,AW=$,S=$:$:$:$).",
        fboMain.GetCoRight(), fboMain.GetCoBottom(),
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
  bool AutoViewport(const GLsizei siWidth, const GLsizei siHeight,
    const bool bForce=false)
  { // Return if the viewport size did not change
    if(DimGetWidth() == siWidth && DimGetHeight() == siHeight) return false;
    // Set the new viewport and log the result
    DimSet(UtilMaximum(1, siWidth), UtilMaximum(1, siHeight));
    cLog->LogDebugExSafe("FboCore set new viewport of $x$.",
      DimGetWidth(), DimGetHeight());
    // Update matrix because the window's aspect ratio may have changed and
    // if the FBO changed then inform lua scripts that they should redraw any
    // FBO's they are managing and return the result.
    if(!AutoMatrix(GetMatrixWidth(), GetMatrixHeight(), bForce)) return false;
    cEvtMain->Add(EMC_LUA_REDRAW);
    return true;
  }
  /* -- Initialise the console fbo for the console object ------------------ */
  void InitConsoleFBO()
    { fboConsole.FboInit("console",
        fboMain.DimGetWidth(), fboMain.DimGetHeight()); }
  /* -- Temporary de-init all guest, console and main fbo objects ---------- */
  void DeInit()
  { // De-init guest fbo objects
    FboDeInit();
    // De-init core fbo objects
    StdForEach(seq, rbegin(), rend(), [](Fbo &fboRef){ fboRef.FboDeInit(); });
  }
  /* -- Initialise fbos using a different constructor ----------- */ protected:
  FboCore() :
    /* -- Initialisers ----------------------------------------------------- */
    FboDouble{{ { GL_RGB8,  true },    // Initialise main fbo (no register)
                { GL_RGBA8, true } }}, // Initialise console fbo (no register)
    fAspectMin(1.0f),                  // Minimum aspect ratio init by CVar
    fAspectMax(2.0f),                  // Minimum aspect ratio init by CVar
    bDraw(false),                      // Do not redraw the back buffer
    bSimpleMatrix(false),              // Simple matrix init by CVar
    bClearBuffer(false),               // Clear buffer init by CVar
    fboMain(front()),                  // Init reference to main fbo
    fboConsole(back()),                // Init reference to console fbo
    ciCurrent{ cd1S },                 // Init one second timer
    uiFPS(0),                          // Init zero last frames per second
    uiFPSCur(0)                        // Init zero current frames per second
    /* -- Set pointer to global class and main fbo ------------------------- */
    { cFboCore = this; cFbos->fboMain = &fboMain; }
  /* -- Set main fbo float reserve --------------------------------- */ public:
  CVarReturn SetFloatReserve(const size_t stCount)
    { return BoolToCVarReturn(fboMain.FboReserveTriangles(stCount)); }
  /* -- Set main fbo command reserve --------------------------------------- */
  CVarReturn SetCommandReserve(const size_t stCount)
    { return BoolToCVarReturn(fboMain.FboReserveCommands(stCount)); }
  /* -- Set main fbo filters (cvar event) ---------------------------------- */
  CVarReturn SetFilter(const OglFilterEnum ofeV)
  { // Check value
    if(ofeV >= OF_MAX) return DENY;
    // Set filtering of main and console framebuffer objects
    for(Fbo &fboRef : *this) fboRef.FboSetFilter(ofeV);
    // Accept the change anyway if opengl not initialised
    if(cOgl->IsGLNotInitialised()) return ACCEPT;
    // Commit the filters
    for(Fbo &fboRef : *this) fboRef.FboCommitFilter();
    // Redraw the main frame-buffer
    SetDraw();
    // Return success
    return ACCEPT;
  }
  /* -- Set back buffer clear ---------------------------------------------- */
  CVarReturn SetBackBufferClear(const bool bState)
    { return CVarSimpleSetInt(bClearBuffer, bState); }
  /* -- Set back buffer clear colour --------------------------------------- */
  CVarReturn SetBackBufferClearColour(const unsigned int uiColour)
    { SetColourInt(uiColour); return ACCEPT; }
  /* -- Set minimum orthagonal matrix ratio -------------------------------- */
  CVarReturn SetMinAspect(const GLfloat fMinimum)
    { return CVarSimpleSetIntNLG(fAspectMin, fMinimum, 1.0f, fAspectMax); }
  /* -- Set maximum orthagonal matrix ratio -------------------------------- */
  CVarReturn SetMaxAspect(const GLfloat fMaximum)
    { return CVarSimpleSetIntNLG(fAspectMax, fMaximum, fAspectMin, 2.0f); }
  /* -- Set simple matrix -------------------------------------------------- */
  CVarReturn SetSimpleMatrix(const bool bState)
    { return CVarSimpleSetInt(bSimpleMatrix, bState); }
};/* ----------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
