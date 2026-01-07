/* == FBO.HPP ============================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## This the file handles rendering of two-dimensional frame buffer     ## **
** ## objects and the drawing of triangles.                               ## **
** ######################################################################### */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IFbo {                       // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace ICollector::P;         using namespace IDim::P;
using namespace ICVarDef::P;           using namespace IError::P;
using namespace IFboBlend::P;          using namespace IColour::P;
using namespace IFboCmd::P;            using namespace ICoords::P;
using namespace IFboItem::P;           using namespace IIdent::P;
using namespace ILockable::P;          using namespace ILog::P;
using namespace ILuaIdent::P;          using namespace ILuaLib::P;
using namespace IOgl::P;               using namespace IShader::P;
using namespace IShaderDef::P;         using namespace IShaders::P;
using namespace IStd::P;               using namespace IString::P;
using namespace ISysUtil::P;           using namespace ITimer::P;
using namespace IUtil::P;              using namespace Lib::OS::GlFW::Types;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* == Fbo collector class for collector data and custom variables ========== */
CTOR_BEGIN_NOBASE(Fbos, Fbo, CLHelperUnsafe,
 /* ------------------------------------------------------------------------ */
Fbo               *fboActive;          // Pointer to active FBO object
Fbo               *fboMain;            // Pointer to main FBO object
);/* ----------------------------------------------------------------------- */
/* == Fbo base class ======================================================= */
class FboBase :                        // Fbo base class
  /* ----------------------------------------------------------------------- */
  // Only put vars used in the Fbo class in here. This is an optimisation so
  // we do not have to initialise all these variables more than once as we have
  // more than one constructor in the main Fbo class.
  /* -- Base classes ------------------------------------------------------- */
  public FboItem,                      // FBO rendering data class
  public Colour,                       // Clear colour of the FBO
  public FboBlend,                     // Blend mode of the FBO
  public CoordsFloat,                  // Matrix co-ordinates of the FBO
  public DimGLSizei,                   // FBO dimensions
  public Lockable                      // Lua lockable class
{ /* --------------------------------------------------------------- */ public:
  bool             bClear;             // Clear the FBO?
  GLenum           ePolyMode;          // Frame buffer polygon mode
  FboCmdVec        fcvActive;          // Commands list
  CoordsFloat      cfStage;            // Stage co-ordinates
  FboTriVec        ftvActive;          // Triangles list
  GLint            iMagFilter,         // Frame buffer magnification filter
                   iMinFilter,         // Frame buffer minification filter
                   iPixFormat,         // Frame buffer pixel format
                   iWrapMode;          // Frame buffer wrapping mode
  OglFilterEnum    ofeFilterId;        // Chosen filter value
  size_t           stCommandsFrame,    // Commands this frame
                   stGLArrayOff,       // Current rendering offset
                   stTrianglesFrame,   // Triangles this frame
                   stTrianglesLast;    // Triangles before last cache change
  GLuint           uiFBO,              // Frame buffer object name
                   uiFBOtex,           // Frame buffer object texture name
                   uiTextureCache,     // Last GL texture id used
                   uiTexUnitCache,     // Last GL multi-texture unit id used
                   uiShaderCache;      // Shader currently selected
  uint64_t         ullActive,          // Frame the FBO was activated
                   ullFinish;          // Frame the FBO was finished
  /* -- Constructor -------------------------------------------------------- */
  explicit FboBase(const GLint iPixFmt, const bool bLockable) :
    /* -- Initialisers ----------------------------------------------------- */
    Lockable{ bLockable },             bClear(true),
    ePolyMode(GL_FILL),                iMagFilter(GL_NEAREST),
    iMinFilter(GL_NEAREST),            iPixFormat(iPixFmt),
    iWrapMode(GL_CLAMP_TO_EDGE),       ofeFilterId(OF_N_N),
    stCommandsFrame(0),                stGLArrayOff(0),
    stTrianglesFrame(0),               stTrianglesLast(0),
    uiFBO(0),                          uiFBOtex(0),
    uiTextureCache(0),                 uiTexUnitCache(0),
    uiShaderCache(0),                  ullActive(static_cast<uint64_t>(-1)),
    ullFinish(ullActive)
    /* --------------------------------------------------------------------- */
    {}
};/* ----------------------------------------------------------------------- */
/* == Fbo object class ===================================================== */
CTOR_MEM_BEGIN_CSLAVE(Fbos, Fbo, ICHelperUnsafe),
  /* -- Base classes ------------------------------------------------------- */
  public FboBase                       // Fbo base class
{ /* -- Select FBO ------------------------------------------------- */ public:
  void FboBindTexture() { cOgl->BindTexture(uiFBOtex); }
  /* -- Bind FBO ----------------------------------------------------------- */
  void FboBind() { cOgl->BindFBO(uiFBO); }
  /* -- Bind FBO and texture ----------------------------------------------- */
  void FboBindAndTexture() { FboBind(); FboBindTexture(); }
  /* -- Set FBO transparency (must be called before initialisation) -------- */
  void FboSetTransparency(const bool bState)
    { iPixFormat = bState ? GL_RGBA8 : GL_RGB8; }
  bool FboIsTransparencyEnabled() const { return iPixFormat == GL_RGBA8; }
  /* -- Set wireframe mode ------------------------------------------------- */
  void FboSetWireframe(const bool bWireframe)
    { ePolyMode = bWireframe ? GL_LINE : GL_FILL; }
  /* -- Flush all queues --------------------------------------------------- */
  void FboClearLists() { ftvActive.clear(); fcvActive.clear(); }
  /* -- Flush all queues and reset caches----------------------------------- */
  void FboFlush()
  { // Flush the triangle and command list
    FboClearLists();
    // Reset counters and caches
    uiTextureCache = uiTexUnitCache = uiShaderCache = 0;
    stTrianglesLast = stGLArrayOff = 0;
  }
  /* -- Return if FBO is already finished ---------------------------------- */
  bool FboIsFinished() const { return cTimer->TimerGetTicks() == ullFinish; }
  /* -- Throw exception if Finish() has already been called ---------------- */
  void FboCheckIfAlreadyFinished()
    { if(FboIsFinished()) [[unlikely]]
        XC("FBO already finished in this frame!",
          "Identifier", IdentGet(), "Frame", ullFinish); }
  /* -- Flush all queues and reset caches but not after Finish() ----------- */
  void FboFlushSafe() { FboCheckIfAlreadyFinished(); FboFlush(); }
  /* -- Flush all queues and frame times ----------------------------------- */
  void FboFlushFull()
  { // Reset FBO properties
    FboFlush();
    // Reset the last time this FBO was used
    ullActive = ullFinish = static_cast<uint64_t>(-1);
  }
  /* -- Force a finish and reset ------------------------------------------- */
  void FboResetCache(const GLuint uiT, const GLuint uiTU, const GLuint uiSC)
  { // Update texture cache id, shader class, and texture unit id
    uiTextureCache = uiT;
    uiShaderCache = uiSC;
    uiTexUnitCache = uiTU;
    // Reset triangles in current triangle count
    stTrianglesLast = FboGetTrisNow();
    // Update current offset of buffer for next finish command
    stGLArrayOff = stTrianglesLast * sizeof(FboTri);
  }
  /* -- Return number of commands parsed last frame ---------------- */ public:
  size_t FboGetCmdsNow() const { return fcvActive.size(); }
  size_t FboGetCmds() const { return stCommandsFrame; }
  size_t FboGetCmdsReserved() const { return fcvActive.capacity(); }
  /* -- Return number of triangles parsed last frame ----------------------- */
  size_t FboGetTrisCmd() const { return FboGetTrisNow() - stTrianglesLast; }
  size_t FboGetTrisNow() const { return ftvActive.size(); }
  size_t FboGetTris() const { return stTrianglesFrame; }
  size_t FboGetTrisReserved() const { return ftvActive.capacity(); }
  /* -- Activate this FBO -------------------------------------------------- */
  void FboSetActive()
  { // Set active fbo to this
    cParent->fboActive = this;
    // Return if we've already activated this FBO this frame
    if(ullActive == cTimer->TimerGetTicks()) [[unlikely]] return;
    // Set new FBO activation tick to this frame
    ullActive = cTimer->TimerGetTicks();
    // Reset everything for this FBO
    FboFlush();
  }
  /* -- Finished with drawing in the FBO ----------------------------------- */
  void FboFinishQueue()
  { // Push the data we need to render the array
    fcvActive.push_back({
      uiTexUnitCache, uiTextureCache, uiShaderCache, // uiTUId,uiTexId,uiPrgId
      reinterpret_cast<GLvoid*>(stGLArrayOff + stOffsetTxcData),  // vpTCOffset
      reinterpret_cast<GLvoid*>(stGLArrayOff + stOffsetPosData),  // vpVOffset
      reinterpret_cast<GLvoid*>(stGLArrayOff + stOffsetColData),  // vpCOffset
      static_cast<GLsizei>(FboGetTrisCmd() * stVertexPerTriangle),// siVertices
    });
  }
  /* -- Finish and render the graphics without tick check ------------------ */
  void FboFinishAndRenderUnsafe()
  { // Finish writing to the arrays
    FboFinishQueue();
    // Set current triangle and frame count
    stTrianglesFrame = FboGetTrisNow();
    stCommandsFrame = FboGetCmdsNow();
    // Select our FBO as render target
    FboBind();
    // Set the viewport of the framebuffer to the total visible pixels
    cOgl->SetViewport(DimGetWidth(), DimGetHeight());
    // Set blending mode
    cOgl->SetBlendIfChanged(*this);
    // Clear the FBO if requested
    if(bClear) cOgl->SetAndClear(*this);
    // No point in continuing if there are no vertices
    const GLsizei siVertices =
      UtilIntOrMax<GLsizei>(stTrianglesFrame * sizeof(FboTri));
    if(!siVertices) [[unlikely]] return;
    // Set polygon fill mode
    cOgl->SetPolygonMode(ePolyMode);
    // Update matrix on each 2D shader...
    for(const Shader &shBuiltIn : cShaderCore->sh2DBuiltIns)
      shBuiltIn.UpdateMatrix(*this);
    // Buffer the new vertex data
    cOgl->BufferStaticData(siVertices, ftvActive.data());
    // Enumerate each command in this order...
    StdForEach(seq, fcvActive.cbegin(),
      next(fcvActive.cbegin(), UtilIntOrMax<ssize_t>(stCommandsFrame)),
        [](const FboCmd &fcData)
    { // Set texture, texture unit and shader program
      cOgl->ActiveTexture(fcData.uiTUId);
      cOgl->BindTexture(fcData.uiTexId);
      cOgl->UseProgram(fcData.uiPrgId);
      // Prepare data arrays
      cOgl->VertexAttribPointer(A_COORD,
       stCompsPerCoord, stBytesPerVertex, fcData.vpTCOffset);
      cOgl->VertexAttribPointer(A_VERTEX,
        stCompsPerPos, stBytesPerVertex, fcData.vpVOffset);
      cOgl->VertexAttribPointer(A_COLOUR,
        stCompsPerColour, stBytesPerVertex, fcData.vpCOffset);
      // Draw data arrays
      cOgl->DrawArraysTriangles(fcData.siVertices);
    });
  }
  /* -- Finish and render the graphics ------------------------------------- */
  void FboFinishAndRender()
  { // If we've already finished this FBO this frame?
    FboCheckIfAlreadyFinished();
    ullFinish = cTimer->TimerGetTicks();
    // Do the actual finish and render
    FboFinishAndRenderUnsafe();
  }
  /* -- Finish the queue without checking and reset cache ------------------ */
  void FboFinishAndReset(const GLuint uiT, const GLuint uiTU,
    const GLuint uiSC) { FboFinishQueue(); FboResetCache(uiT, uiTU, uiSC); }
  /* -- Blit the specified texture into the FBO ---------------------------- */
  void FboBlit(const GLuint uiTex, const TriPosData &fV,
    const TriCoordData &fTC, const TriColData &fC, const GLuint uiTexU,
    const Shader*const shProgram)
  { // If this is the first triangle in this command, we just init the cache
    if(stTrianglesLast == FboGetTrisNow())
      FboResetCache(uiTex, uiTexU, shProgram->GetProgram());
    // Not the first triagnle so finish and reset cache if...
    else if(uiTex != uiTextureCache ||             // ...texture unchanged?
       shProgram->GetProgram() != uiShaderCache || // ...or shader unchanged?
       uiTexU != uiTexUnitCache)                   // ...or tex unit unchanged?
      FboFinishAndReset(uiTex, uiTexU, shProgram->GetProgram());
    // Structure to insert into our contiguous array buffer. Note that this
    // method produces less assembler output instead of extending the vector
    // and copying the data into the last element and removing push_back.
    // Add completed triangle to list of arrays
    ftvActive.push_back({{
      // <--TexCoord---> <-Vertex (2D)-> <-------Colour (RGBA)------->
      {{ fTC[0],fTC[1] },{ fV[0],fV[1] },{ fC[0],fC[1],fC[ 2],fC[ 3] }}, // P1
      {{ fTC[2],fTC[3] },{ fV[2],fV[3] },{ fC[4],fC[5],fC[ 6],fC[ 7] }}, // P2
      {{ fTC[4],fTC[5] },{ fV[4],fV[5] },{ fC[8],fC[9],fC[10],fC[11] }}  // P3
      // <--TexCoord---> <-Vertex (2D)-> <-------Colour (RGBA)------->
    }});
  }
  /* -- Blit the specified triangle of the specifed FBO to this FBO -------- */
  void FboBlitTri(Fbo &fboSrc, const size_t stId)
    { FboBlit(fboSrc.uiFBOtex, fboSrc.FboItemGetVData(stId),
        fboSrc.FboItemGetTCData(stId), fboSrc.FboItemGetCData(stId), 0,
        &cShaderCore->sh2D); }
  /* -- Blit the specified FBO texture into this FBO ----------------------- */
  void FboBlit(Fbo &fboSrc)
    { for(size_t stId = 0; stId < stTrisPerQuad; ++stId)
        FboBlitTri(fboSrc, stId); }
  /* -- Blit the specified quad into this FBO ------------------------------ */
  void FboBlit(FboItem &fboiSrc, const GLuint uiTex, const GLuint uiTexU,
    const Shader*const shProgram)
      { for(size_t stId = 0; stId < stTrisPerQuad; ++stId)
          FboBlit(uiTex, fboiSrc.FboItemGetVData(stId),
            fboiSrc.FboItemGetTCData(stId), fboiSrc.FboItemGetCData(stId),
            uiTexU, shProgram); }
  /* -- Set wrapping mode -------------------------------------------------- */
  void FboSetWrap(const GLint iM) { iWrapMode = iM; }
  /* -- Commit wrapping mode ----------------------------------------------- */
  void FboCommitWrap()
  { // Bind FBO and FBO texture
    FboBindAndTexture();
    // Procedures to perform
    struct Procedure { const GLenum eWrap; const char cWrap; };
    typedef array<const Procedure, 3> Procedures;
    static const Procedures sProcedures{{
      { GL_TEXTURE_WRAP_S, 'S' },
      { GL_TEXTURE_WRAP_T, 'T' },
      { GL_TEXTURE_WRAP_R, 'R' }
    }};
    // Set to repeat and disable filter
    for(const Procedure &pItem : sProcedures)
      cOgl->SetTexParam(pItem.eWrap, iWrapMode);
  }
  /* -- Set filtering by ID ------------------------------------------------ */
  OglFilterEnum FboGetFilter() const { return ofeFilterId; }
  void FboSetFilter(const OglFilterEnum ofeId)
  { // Translate our filter id to min and mag filter
    cOgl->SetFilterById(ofeId, iMinFilter, iMagFilter);
    // Record filter id
    ofeFilterId = ofeId;
  }
  /* -- Commit new filter modes -------------------------------------------- */
  void FboCommitFilter()
  { // Bind FBO and FBO texture
    FboBindAndTexture();
    // Set filtering
    cOgl->SetTexParam(GL_TEXTURE_MIN_FILTER, iMinFilter);
    cOgl->SetTexParam(GL_TEXTURE_MAG_FILTER, iMagFilter);
  }
  /* -- Set filtering by ID and commit ------------------------------------- */
  void FboSetFilterCommit(const OglFilterEnum ofeId)
    { FboSetFilter(ofeId); FboCommitFilter(); }
  /* -- Set backbuffer blending mode --------------------------------------- */
  void FboSetBlend(const OglBlendEnum obeSFactorRGB,
    const OglBlendEnum obeDFactorRGB, const OglBlendEnum obeSFactorA,
    const OglBlendEnum obeDFactorA)
  { // Lookup values and store requested blending value
    FboBlendSetSrcRGB(cOgl->EngineBlendToOglBlend(obeSFactorRGB));
    FboBlendSetDstRGB(cOgl->EngineBlendToOglBlend(obeDFactorRGB));
    FboBlendSetSrcAlpha(cOgl->EngineBlendToOglBlend(obeSFactorA));
    FboBlendSetDstAlpha(cOgl->EngineBlendToOglBlend(obeDFactorA));
  }
  /* -- Set clear state ---------------------------------------------------- */
  void FboSetClear(const bool bState) { bClear = bState; }
  bool FboIsClearEnabled() const { return bClear; }
  /* -- Reserve memory for lists ------------------------------------------- */
  void FboReserve(const size_t stTri, const size_t stCmd)
  { // Reserve memory for command list and throw error on failure
    if(!UtilReserveList(fcvActive, stCmd))
      XC("Failed to reserve memory for FBO command list!",
        "Identifier", IdentGet(), "Commands", stCmd,
        "Bytes", stCmd * sizeof(FboCmdVec));
    // Reserve memory for triangles list and throw error on failure
    if(!UtilReserveList(ftvActive, stTri))
      XC("Failed to reserve memory for FBO triangles list!",
        "Identifier", IdentGet(), "Commands", stTri,
        "Bytes", stTri * sizeof(FboTri));
  }
  /* -- Set matrix for drawing --------------------------------------------- */
  void FboSetMatrix(const GLfloat fLeft, const GLfloat fTop,
    const GLfloat fRight, const GLfloat fBottom)
  { // Set new stage bounds
    cfStage.CoordsSet(fLeft, fTop, fRight, fBottom);
    // Set matrix top-left co-ordinates
    CoordsSetLeft(cfStage.CoordsGetLeft() < 0 ?
      -cfStage.CoordsGetLeft() : cfStage.CoordsGetLeft());
    CoordsSetTop(cfStage.CoordsGetTop() < 0 ?
      -cfStage.CoordsGetTop() : cfStage.CoordsGetTop());
  }
  /* -- ReInitialise ------------------------------------------------------- */
  void FboReInit()
  { // Although we don't specify the buffer reservations. It's still save as
    // .reserve() below what has already been reserved is essentially a nullop.
    FboInit(IdentGet(), DimGetWidth(), DimGetHeight(), 0, 0);
  }
  /* -- DeInitialise ------------------------------------------------------- */
  void FboDeInit()
  { // Remove as active FBO if set
    if(cParent->fboActive == this) cParent->fboMain->FboSetActive();
    // Reset everything
    FboFlushFull();
    // Have FBO texture?
    if(uiFBOtex)
    { // Mark texture for deletion (explanation at top)
      cOgl->SetDeleteTexture(uiFBOtex);
      // Log the de-initialised
      cLog->LogDebugExSafe("Fbo '$' at $ removed texture $.",
        IdentGet(), uiFBO, uiFBOtex);
      // Done with this texture handle
      uiFBOtex = 0;
    } // Have FBO?
    if(uiFBO)
    { // Mark FBO for deletion (explanation at top)
      cOgl->SetDeleteFbo(uiFBO);
      // Log the de-initialised
      cLog->LogDebugExSafe("Fbo '$' removed from $.", IdentGet(), uiFBO);
      // Done with this FBO handle
      uiFBO = 0;
    }
  }
  /* -- Initialise --------------------------------------------------------- */
  void FboInit(const string &strID, const GLsizei siW, const GLsizei siH,
    const size_t stTri, const size_t stCmd)
  { // Say we're initialising the frame buffer.
    cLog->LogDebugExSafe("Fbo initialising a $x$ object '$'...",
      siW, siH, strID);
    // De-initialise old FBO first.
    FboDeInit();
    // Set identifier.
    IdentSet(strID);
    // Reserve memory for triangle and command buffers
    FboReserve(stTri, stCmd);
    // Record dimensions and clamp texture size to maximum supported size.
    DimSet(UtilMinimum(cOgl->MaxTexSize<GLsizei>(), siW),
           UtilMinimum(cOgl->MaxTexSize<GLsizei>(), siH));
    // If dimensions are different we need to tell the user that
    if(DimGetWidth() != siW || DimGetHeight() != siH)
      cLog->LogWarningExSafe(
        "Fbo '$' dimensions exceed renderer limit ($x$ > $^2)!",
        IdentGet(), siW, siH, cOgl->MaxTexSize());
    // Generate framebuffer and throw error if failed.
    GL(cOgl->CreateFBO(&uiFBO), "Failed to create framebuffer!",
      "Identifier", IdentGet(), "Width", siW, "Height", siH);
    // Generate texture name for FBO and bind it.
    GL(cOgl->CreateTexture(&uiFBOtex),
     "Failed to create texture for framebuffer!",
     "Identifier", IdentGet(), "Width",  siW,
     "Height",     siH,        "Buffer", &uiFBOtex);
    // Bind FBO texture
    GL(FboBindTexture(), "Failed to select FBO texture as active!",
      "Identifier", IdentGet(), "Texture", uiFBOtex);
    // Bind FBO
    GL(FboBind(), "Failed to select active FBO!",
      "Identifier", IdentGet(), "FBO", uiFBO);
    // nullptr means reserve texture memory but to not copy any data to it
    GL(cOgl->UploadTexture(0, DimGetWidth(), DimGetHeight(), iPixFormat,
      GL_BGR, nullptr),
        "Failed to allocate video memory for framebuffer texture!",
        "Identifier",  IdentGet(),    "Texture", uiFBOtex,
        "Width",       DimGetWidth(), "Height",  DimGetHeight(),
        "PixelFormat", iPixFormat);
    // Attach 2D texture to this FBO
    GL(cOgl->AttachTexFBO(uiFBOtex),
      "Failed to attach texture to framebuffer!",
      "Identifier", IdentGet(), "Texture", uiFBOtex);
    // Make sure framebuffer generation is complete successfully
    const GLenum uiError = cOgl->VerifyFBO();
    if(uiError != GL_FRAMEBUFFER_COMPLETE)
      XC("Failed to complete framebuffer!",
        "Identifier", IdentGet(), "Error", uiError);
    // Set total requested width and height
    CoordsSetRight(static_cast<GLfloat>(siW));
    CoordsSetBottom(static_cast<GLfloat>(siH));
    // Commit the guest requested filter and wrapping texture properties
    GL(FboCommitFilter(), "Failed to set texture filter!",
      "Identifier", IdentGet(), "MinFilter", iMinFilter,
      "MagFilter",  iMagFilter);
    // Commit texture wrapping settings
    GL(FboCommitWrap(), "Failed to set texture wrapping mode!",
      "Identifier", IdentGet(), "Mode", iWrapMode);
    // Say we've initialised the frame buffer
    cLog->LogDebugExSafe("Fbo initialised '$' at $ (S=$x$;A=$;T=$;F=$).",
      IdentGet(), uiFBO, DimGetWidth(), DimGetHeight(), StrFromRatio(siW, siH),
      uiFBOtex, cOgl->GetPixelFormat(iPixFormat));
  }
  /* -- Constructor -------------------------------------------------------- */
  Fbo() :
    /* -- Initialisers ----------------------------------------------------- */
    ICHelperFbo{ cFbos, this },        // Initially registered
    IdentCSlave{ cParent->CtrNext() }, // Initialise identification number
    FboBase{ GL_RGBA8, false }         // Has alpha channel and not protected
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- Constructor WITHOUT registration (used for core Fbos)--------------- */
  Fbo(const GLint iPixFmt,             // Pixel format requested
      const bool bLocked) :            // Locked from garbage collector
    /* -- Initialisers ----------------------------------------------------- */
    ICHelperFbo{ cFbos },              // Initially unregistered
    IdentCSlave{ cParent->CtrNext() }, // Initialise identification number
    FboBase{ iPixFmt, bLocked }        // Init alpha channel and lua protect
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- Destructor --------------------------------------------------------- */
  DTORHELPER(~Fbo, FboDeInit())
};/* ----------------------------------------------------------------------- */
CTOR_END(Fbos, Fbo, FBO,,,, fboActive(nullptr), fboMain(nullptr))
/* == Re-initialise all FBO's ============================================== */
static void FboReInit()
{ // Ignore if no FBO's
  if(cFbos->empty()) return;
  // Reinit all FBO's and log pre/post operation.
  cLog->LogDebugExSafe("Fbos reinitialising $ objects...", cFbos->size());
  for(Fbo*const fCptr : *cFbos) fCptr->FboReInit();
  cLog->LogInfoExSafe("Fbos reinitialised $ objects.", cFbos->size());
}
/* -- De-initialise all FBO's ---------------------------------------------- */
static void FboDeInit()
{ // Ignore if no FBO's
  if(cFbos->empty()) return;
  // De-init all FBO's (NOT destroy them!) and log pre/post operation.
  cLog->LogDebugExSafe("Fbos de-initialising $ objects...", cFbos->size());
  for(Fbo*const fCptr : *cFbos) fCptr->FboDeInit();
  cLog->LogInfoExSafe("Fbos de-initialised $ objects.", cFbos->size());
}
/* -- Set FBO render order reserve ----------------------------------------- */
static CVarReturn FboSetOrderReserve(const size_t) { return ACCEPT; }
/* -- Return active fbo ---------------------------------------------------- */
static Fbo *FboActive(void) { return cFbos->fboActive; }
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
