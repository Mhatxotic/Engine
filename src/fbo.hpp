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
using namespace IFboItem::P;           using namespace IFrame::P;
using namespace ILockable::P;          using namespace ILog::P;
using namespace ILuaIdent::P;          using namespace ILuaLib::P;
using namespace IName::P;              using namespace IOgl::P;
using namespace ISerial::P;            using namespace IShader::P;
using namespace IShaderDef::P;         using namespace IShaders::P;
using namespace IStd::P;               using namespace IString::P;
using namespace ISysUtil::P;           using namespace IUtil::P;
using namespace Lib::OS::GlFW::Types;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* == Fbo collector class for collector data and custom variables ========== */
CTOR_BEGIN_NOBASE(Fbos, Fbo, CLHelperUnsafe,
 /* ------------------------------------------------------------------------ */
Fbo               *fboActive;          // Pointer to active Fbo object
Fbo               *fboMain;            // Pointer to main Fbo object
);/* ----------------------------------------------------------------------- */
/* == Fbo base class ======================================================= */
class FboBase :                        // Fbo base class
  /* ----------------------------------------------------------------------- */
  // Only put vars used in the Fbo class in here. This is an optimisation so
  // we do not have to initialise all these variables more than once as we have
  // more than one constructor in the main Fbo class.
  /* -- Base classes ------------------------------------------------------- */
  public FboItem,                      // Fbo rendering data class
  public Colour,                       // Clear colour of the Fbo
  public FboBlend,                     // Blend mode of the Fbo
  public CoordsGLFloat,                // Matrix co-ordinates of the Fbo
  public DimGLSizei,                   // Fbo dimensions
  public Lockable                      // Lua lockable class
{ /* --------------------------------------------------------------- */ public:
  bool             bClear;             // Clear the Fbo?
  GLenum           glePolyMode;        // Frame buffer polygon mode
  FboCmdVec        fcvActive;          // Commands list
  CoordsGLFloat    cglfStage;          // Stage co-ordinates
  FboTriVec        ftvActive;          // Triangles list
  GLint            gliMagFilter,       // Frame buffer magnification filter
                   gliMinFilter,       // Frame buffer minification filter
                   gliPixFormat,       // Frame buffer pixel format
                   gliWrapMode;        // Frame buffer wrapping mode
  OglFilterEnum    ofeFilterId;        // Chosen filter value
  size_t           stCommandsFrame,    // Commands this frame
                   stGLArrayOff,       // Current rendering offset
                   stTrianglesFrame,   // Triangles this frame
                   stTrianglesLast;    // Triangles before last cache change
  GLuint           gluFbo,             // Frame buffer object name
                   gluFbotex,          // Frame buffer object texture name
                   gluTextureCache,    // Last GL texture id used
                   gluTexUnitCache,    // Last GL multi-texture unit id used
                   gluShaderCache;     // Shader currently selected
  uint64_t         ullActive,          // Frame the Fbo was activated
                   ullFinish;          // Frame the Fbo was finished
  /* -- Constructor -------------------------------------------------------- */
  explicit FboBase(const GLint iPixFmt, const bool bLockable) :
    /* -- Initialisers ----------------------------------------------------- */
    Lockable{ bLockable },             bClear(true),
    glePolyMode(GL_FILL),              gliMagFilter(GL_NEAREST),
    gliMinFilter(GL_NEAREST),          gliPixFormat(iPixFmt),
    gliWrapMode(GL_CLAMP_TO_EDGE),     ofeFilterId(OF_N_N),
    stCommandsFrame(0),                stGLArrayOff(0),
    stTrianglesFrame(0),               stTrianglesLast(0),
    gluFbo(0),                         gluFbotex(0),
    gluTextureCache(0),                gluTexUnitCache(0),
    gluShaderCache(0),                 ullActive(StdMaxUInt64),
    ullFinish(ullActive)
    /* --------------------------------------------------------------------- */
    {}
};/* ----------------------------------------------------------------------- */
/* == Fbo object class ===================================================== */
CTOR_MEM_BEGIN_CSLAVE(Fbos, Fbo, ICHelperUnsafe),
  /* -- Base classes ------------------------------------------------------- */
  public FboBase                       // Fbo base class
{ /* -- Select Fbo ------------------------------------------------- */ public:
  void FboBindTexture() { cOgl->BindTexture(gluFbotex); }
  /* -- Bind Fbo ----------------------------------------------------------- */
  void FboBind() { cOgl->BindFbo(gluFbo); }
  /* -- Bind Fbo and texture ----------------------------------------------- */
  void FboBindAndTexture() { FboBind(); FboBindTexture(); }
  /* -- Set Fbo transparency (must be called before initialisation) -------- */
  void FboSetTransparency(const bool bState)
    { gliPixFormat = bState ? GL_RGBA8 : GL_RGB8; }
  bool FboIsTransparencyEnabled() const { return gliPixFormat == GL_RGBA8; }
  /* -- Set wireframe mode ------------------------------------------------- */
  void FboSetWireframe(const bool bWireframe)
    { glePolyMode = bWireframe ? GL_LINE : GL_FILL; }
  /* -- Flush all queues --------------------------------------------------- */
  void FboClearLists() { ftvActive.clear(); fcvActive.clear(); }
  /* -- Flush all queues and reset caches----------------------------------- */
  void FboFlush()
  { // Flush the triangle and command list
    FboClearLists();
    // Reset counters and caches
    gluTextureCache = gluTexUnitCache = gluShaderCache = 0;
    stTrianglesLast = stGLArrayOff = 0;
  }
  /* -- Reset finish time -------------------------------------------------- */
  void FboResetFinish() { ullActive = ullFinish = StdMaxUInt64; }
  /* -- Flush and reset finish times --------------------------------------- */
  void FboResetFinishAndFlush() { FboResetFinish(); FboFlush(); }
  /* -- Return if Fbo is already finished ---------------------------------- */
  bool FboIsFinished() const { return cFrame->FrameGetTicks() == ullFinish; }
  /* -- Throw exception if Finish() has already been called ---------------- */
  void FboCheckIfAlreadyFinished()
    { if(FboIsFinished()) [[unlikely]]
        XC("Fbo already finished in this frame!",
          "Name", NameGet(), "Frame", ullFinish); }
  /* -- Flush all queues and reset caches but not after Finish() ----------- */
  void FboFlushSafe() { FboCheckIfAlreadyFinished(); FboFlush(); }
  /* -- Reset Fbo properties and reset the last time this Fbo was used ----- */
  void FboFlushFull() { FboFlush(); FboResetFinish(); }
  /* -- Force a finish and reset ------------------------------------------- */
  void FboResetCache(const GLuint gluT, const GLuint gluTU, const GLuint gluSC)
  { // Update texture cache id, shader class, and texture unit id
    gluTextureCache = gluT;
    gluShaderCache = gluSC;
    gluTexUnitCache = gluTU;
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
  /* -- Return stage bounds ------------------------------------------------ */
  CoordsGLFloat &FboGetStage() { return cglfStage; }
  const CoordsGLFloat &FboGetStageConst() const { return cglfStage; }
  /* -- Activate this Fbo -------------------------------------------------- */
  void FboSetActive()
  { // Set active fbo to this
    cParent->fboActive = this;
    // Return if we've already activated this Fbo this frame
    if(ullActive == cFrame->FrameGetTicks()) [[unlikely]] return;
    // Set new Fbo activation tick to this frame
    ullActive = cFrame->FrameGetTicks();
    // Reset everything for this Fbo
    FboFlush();
  }
  /* -- Finished with drawing in the Fbo ----------------------------------- */
  void FboFinishQueue()
  { // Push the data we need to render the array
    fcvActive.push_back({
      // gluTUId, gluTexId, gluPrgId
      gluTexUnitCache, gluTextureCache, gluShaderCache,
      // vpTCOffset
      reinterpret_cast<GLvoid*>(stGLArrayOff + stOffsetTxcData),
      // vpVOffset
      reinterpret_cast<GLvoid*>(stGLArrayOff + stOffsetPosData),
      // vpCOffset
      reinterpret_cast<GLvoid*>(stGLArrayOff + stOffsetIntData),
      // glsiVertices
      static_cast<GLsizei>(FboGetTrisCmd() * stVertexPerTriangle),
    });
  }
  /* -- Finish and render the graphics without tick check ------------------ */
  void FboFinishAndRenderUnsafe()
  { // Finish writing to the arrays
    FboFinishQueue();
    // Set current triangle and frame count
    stTrianglesFrame = FboGetTrisNow();
    stCommandsFrame = FboGetCmdsNow();
    // Select our Fbo as render target
    FboBind();
    // Set the viewport of the framebuffer to the total visible pixels
    cOgl->SetViewport(DimGetWidth(), DimGetHeight());
    // Set blending mode
    cOgl->SetBlendIfChanged(*this);
    // Clear the Fbo if requested
    if(bClear) cOgl->SetAndClear(*this);
    // No point in continuing if there are no vertices
    const GLsizei glsiVertices =
      UtilIntOrMax<GLsizei>(stTrianglesFrame * sizeof(FboTri));
    if(!glsiVertices) [[unlikely]] return;
    // Set polygon fill mode
    cOgl->SetPolygonMode(glePolyMode);
    // Update matrix on each 2D shader...
    for(const Shader &shBuiltIn : cShaderCore->sh2DBuiltIns)
      shBuiltIn.UpdateMatrix(*this);
    // Buffer the new vertex data
    cOgl->BufferStaticData(glsiVertices, ftvActive.data());
    // Enumerate each command in this order...
    StdForEach(seq, fcvActive.cbegin(),
      StdNext(fcvActive.cbegin(), UtilIntOrMax<ssize_t>(stCommandsFrame)),
        [](const FboCmd &fcData)
    { // Set texture, texture unit and shader program
      cOgl->ActiveTexture(fcData.gluTUId);
      cOgl->BindTexture(fcData.gluTexId);
      cOgl->UseProgram(fcData.gluPrgId);
      // Prepare data arrays
      cOgl->VertexAttribPointer(A_COORD,
       stCompsPerCoord, stBytesPerVertex, fcData.vpTCOffset);
      cOgl->VertexAttribPointer(A_VERTEX,
        stCompsPerPos, stBytesPerVertex, fcData.vpVOffset);
      cOgl->VertexAttribPointer(A_COLOUR,
        stCompsPerColour, stBytesPerVertex, fcData.vpCOffset);
      // Draw data arrays
      cOgl->DrawArraysTriangles(fcData.glsiVertices);
    });
  }
  /* -- Finish and render the graphics ------------------------------------- */
  void FboFinishAndRender()
  { // If we've already finished this Fbo this frame?
    FboCheckIfAlreadyFinished();
    ullFinish = cFrame->FrameGetTicks();
    // Do the actual finish and render
    FboFinishAndRenderUnsafe();
  }
  /* -- Finish the queue without checking and reset cache ------------------ */
  void FboFinishAndReset(const GLuint gluT, const GLuint gluTU,
    const GLuint gluSC)
  { FboFinishQueue(); FboResetCache(gluT, gluTU, gluSC); }
  /* -- Blit the specified texture into the Fbo ---------------------------- */
  void FboBlit(const GLuint gluTex, const TriPosData &tpdV,
    const TriTexData &tcdC, const TriIntData &tidC, const GLuint gluTexU,
    const Shader*const shpProgram)
  { // If this is the first triangle in this command, we just init the cache
    if(stTrianglesLast == FboGetTrisNow())
      FboResetCache(gluTex, gluTexU, shpProgram->GetProgram());
    // Not the first triagnle so finish and reset cache if...
    else if(gluTex != gluTextureCache ||             // ...texture changed?
       shpProgram->GetProgram() != gluShaderCache || // ...*or* shader changed?
       gluTexU != gluTexUnitCache)                   // ...*or* txunit changed?
      FboFinishAndReset(gluTex, gluTexU, shpProgram->GetProgram());
    // Structure to insert into our contiguous array buffer. Note that this
    // method produces less assembler output instead of extending the vector
    // and copying the data into the last element and removing push_back.
    // Add completed triangle to list of arrays
    ftvActive.push_back({{
      // Vertex 1/3
      { { tcdC[0], tcdC[1] }, { tpdV[0], tpdV[1] },   // Texture, position
        { tidC[0], tidC[1], tidC[ 2], tidC[ 3]   } }, // Colour intensity
      // Vertex 2/3
      { { tcdC[2], tcdC[3] }, { tpdV[2], tpdV[3] },   // Texture, position
        { tidC[4], tidC[5], tidC[ 6], tidC[ 7]   } }, // Colour intensity
      // Vertex 3/3
      { { tcdC[4], tcdC[5] }, { tpdV[4], tpdV[5] },   // Texture, position
        { tidC[8], tidC[9], tidC[10], tidC[11]   } }  // Colour intensity
    }});
  }
  /* -- Blit the specified triangle of the specifed Fbo to this Fbo -------- */
  void FboBlitTri(Fbo &fboSrc, const size_t stId)
    { FboBlit(fboSrc.gluFbotex, fboSrc.FboItemGetVData(stId),
        fboSrc.FboItemGetTCData(stId), fboSrc.FboItemGetCData(stId), 0,
        &cShaderCore->sh2D); }
  /* -- Blit the specified Fbo texture into this Fbo ----------------------- */
  void FboBlit(Fbo &fboSrc)
    { for(size_t stId = 0; stId < stTrisPerQuad; ++stId)
        FboBlitTri(fboSrc, stId); }
  /* -- Blit the specified quad into this Fbo ------------------------------ */
  void FboBlit(FboItem &fiSrc, const GLuint gluTex, const GLuint gluTexU,
    const Shader*const shpProgram)
      { for(size_t stId = 0; stId < stTrisPerQuad; ++stId)
          FboBlit(gluTex, fiSrc.FboItemGetVData(stId),
            fiSrc.FboItemGetTCData(stId), fiSrc.FboItemGetCData(stId),
            gluTexU, shpProgram); }
  /* -- Set wrapping mode -------------------------------------------------- */
  void FboSetWrap(const GLint gliNWrapMode) { gliWrapMode = gliNWrapMode; }
  /* -- Commit wrapping mode ----------------------------------------------- */
  void FboCommitWrap()
  { // Bind Fbo and Fbo texture
    FboBindAndTexture();
    // Procedures to perform
    struct Procedure { const GLenum gleWrap; const char cWrap; };
    using Procedures = StdArray<const Procedure, 3>;
    static const Procedures sProcedures{{
      { GL_TEXTURE_WRAP_S, 'S' },
      { GL_TEXTURE_WRAP_T, 'T' },
      { GL_TEXTURE_WRAP_R, 'R' }
    }};
    // Set to repeat and disable filter
    for(const Procedure &pItem : sProcedures)
      cOgl->SetTexParam(pItem.gleWrap, gliWrapMode);
  }
  /* -- Set filtering by ID ------------------------------------------------ */
  OglFilterEnum FboGetFilter() const { return ofeFilterId; }
  void FboSetFilter(const OglFilterEnum ofeId)
  { // Translate our filter id to min and mag filter
    cOgl->SetFilterById(ofeId, gliMinFilter, gliMagFilter);
    // Record filter id
    ofeFilterId = ofeId;
  }
  /* -- Commit new filter modes -------------------------------------------- */
  void FboCommitFilter()
  { // Bind Fbo and Fbo texture
    FboBindAndTexture();
    // Set filtering
    cOgl->SetTexParam(GL_TEXTURE_MIN_FILTER, gliMinFilter);
    cOgl->SetTexParam(GL_TEXTURE_MAG_FILTER, gliMagFilter);
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
      XC("Failed to reserve memory for Fbo command list!",
        "Name",  NameGet(), "Commands", stCmd,
        "Bytes", stCmd * sizeof(FboCmdVec));
    // Reserve memory for triangles list and throw error on failure
    if(!UtilReserveList(ftvActive, stTri))
      XC("Failed to reserve memory for Fbo triangles list!",
        "Name",  NameGet(), "Commands", stTri,
        "Bytes", stTri * sizeof(FboTri));
  }
  /* -- Set matrix for drawing --------------------------------------------- */
  void FboSetMatrix(const GLfloat glfX1, const GLfloat glfY1,
    const GLfloat glfX2, const GLfloat glfY2)
  { // Set new stage bounds
    FboGetStage().CoordsSet(glfX1, glfY1, glfX2, glfY2);
    // Set matrix top-left co-ordinates
    CoordsSetX1(FboGetStageConst().CoordsGetX1() < 0 ?
      -FboGetStageConst().CoordsGetX1() : FboGetStageConst().CoordsGetX1());
    CoordsSetY1(FboGetStageConst().CoordsGetY1() < 0 ?
      -FboGetStageConst().CoordsGetY1() : FboGetStageConst().CoordsGetY1());
  }
  /* -- ReInitialise ------------------------------------------------------- */
  void FboReInit()
  { // Although we don't specify the buffer reservations. It's still save as
    // .reserve() below what has already been reserved is essentially a nullop.
    FboInit(NameGet(), DimGetWidth(), DimGetHeight(), 0, 0);
  }
  /* -- DeInitialise ------------------------------------------------------- */
  void FboDeInit()
  { // Remove as active Fbo if set
    if(cParent->fboActive == this) cParent->fboMain->FboSetActive();
    // Reset everything
    FboFlushFull();
    // Have Fbo texture?
    if(gluFbotex)
    { // Mark texture for deletion (explanation at top)
      cOgl->SetDeleteTexture(gluFbotex);
      // Log the de-initialised
      cLog->LogDebugExSafe("Fbo '$' at $ removed texture $.",
        NameGet(), gluFbo, gluFbotex);
      // Done with this texture handle
      gluFbotex = 0;
    } // Have Fbo?
    if(gluFbo)
    { // Mark Fbo for deletion (explanation at top)
      cOgl->SetDeleteFbo(gluFbo);
      // Log the de-initialised
      cLog->LogDebugExSafe("Fbo '$' removed from $.", NameGet(), gluFbo);
      // Done with this Fbo handle
      gluFbo = 0;
    }
  }
  /* -- Initialise --------------------------------------------------------- */
  void FboInit(const StdStringView &strvID, const GLsizei glsiW,
    const GLsizei glsiH, const size_t stTri, const size_t stCmd)
  { // Say we're initialising the frame buffer.
    cLog->LogDebugExSafe("Fbo initialising a $x$ object '$'...",
      glsiW, glsiH, strvID);
    // De-initialise old Fbo first.
    FboDeInit();
    // Set identifier.
    NameSet(strvID);
    // Reserve memory for triangle and command buffers
    FboReserve(stTri, stCmd);
    // Record dimensions and clamp texture size to maximum supported size.
    DimSet(UtilMinimum(cOgl->MaxTexSize<GLsizei>(), glsiW),
           UtilMinimum(cOgl->MaxTexSize<GLsizei>(), glsiH));
    // If dimensions are different we need to tell the user that
    if(DimIsNotEqual(glsiW, glsiH))
      cLog->LogWarningExSafe(
        "Fbo '$' dimensions exceed renderer limit ($x$ > $^2)!",
        NameGet(), glsiW, glsiH, cOgl->MaxTexSize());
    // Generate framebuffer and throw error if failed.
    GL(cOgl->CreateFbo(&gluFbo), "Failed to create framebuffer!",
      "Name", NameGet(), "Width", glsiW, "Height", glsiH);
    // Generate texture name for Fbo and bind it.
    GL(cOgl->CreateTexture(&gluFbotex),
     "Failed to create texture for framebuffer!",
     "Name", NameGet(), "Width",  glsiW,
     "Height",     glsiH,        "Buffer", &gluFbotex);
    // Bind Fbo texture
    GL(FboBindTexture(), "Failed to select Fbo texture as active!",
      "Name", NameGet(), "Texture", gluFbotex);
    // Bind Fbo
    GL(FboBind(), "Failed to select active Fbo!",
      "Name", NameGet(), "Fbo", gluFbo);
    // nullptr means reserve texture memory but to not copy any data to it
    GL(cOgl->UploadTexture(0, DimGetWidth(), DimGetHeight(), gliPixFormat,
      GL_BGR, nullptr),
        "Failed to allocate video memory for framebuffer texture!",
        "Name",  NameGet(),    "Texture", gluFbotex,
        "Width",       DimGetWidth(), "Height",  DimGetHeight(),
        "PixelFormat", gliPixFormat);
    // Attach 2D texture to this Fbo
    GL(cOgl->AttachTexFbo(gluFbotex),
      "Failed to attach texture to framebuffer!",
      "Name", NameGet(), "Texture", gluFbotex);
    // Make sure framebuffer generation is complete successfully
    const GLenum gleError = cOgl->VerifyFbo();
    if(gleError != GL_FRAMEBUFFER_COMPLETE)
      XC("Failed to complete framebuffer!",
        "Name", NameGet(), "Error", gleError);
    // Set total requested width and height
    CoordsSetX2(static_cast<GLfloat>(glsiW));
    CoordsSetY2(static_cast<GLfloat>(glsiH));
    // Commit the guest requested filter and wrapping texture properties
    GL(FboCommitFilter(), "Failed to set texture filter!",
      "Name", NameGet(), "MinFilter", gliMinFilter,
      "MagFilter",  gliMagFilter);
    // Commit texture wrapping settings
    GL(FboCommitWrap(), "Failed to set texture wrapping mode!",
      "Name", NameGet(), "Mode", gliWrapMode);
    // Say we've initialised the frame buffer
    cLog->LogDebugExSafe("Fbo initialised '$' at $ (S=$x$;A=$;T=$;F=$).",
      NameGet(), gluFbo, DimGetWidth(), DimGetHeight(),
      StrFromRatio(glsiW, glsiH), gluFbotex,
      cOgl->GetPixelFormat(gliPixFormat));
  }
  /* -- Constructor -------------------------------------------------------- */
  Fbo() :
    /* -- Initialisers ----------------------------------------------------- */
    ICHelperFbo{ cFbos, this },        // Initially registered
    SerialSlave{ cParent->Serial() },  // Initialise identification number
    FboBase{ GL_RGBA8, false }         // Has alpha channel and not protected
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- Constructor WITHOUT registration (used for core Fbos)--------------- */
  Fbo(const GLint iPixFmt,             // Pixel format requested
      const bool bLocked) :            // Locked from garbage collector
    /* -- Initialisers ----------------------------------------------------- */
    ICHelperFbo{ cFbos },              // Initially unregistered
    SerialSlave{ cParent->Serial() },  // Initialise identification number
    FboBase{ iPixFmt, bLocked }        // Init alpha channel and lua protect
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- Destructor --------------------------------------------------------- */
  DTORHELPER(~Fbo, FboDeInit())
};/* ----------------------------------------------------------------------- */
CTOR_END(Fbos, Fbo, FBO,,,, fboActive(nullptr), fboMain(nullptr))
/* == Re-initialise all Fbo's ============================================== */
static void FboReInit()
{ // Ignore if no Fbo's
  if(cFbos->empty()) return;
  // Reinit all Fbo's and log pre/post operation.
  cLog->LogDebugExSafe("Fbos reinitialising $ objects...", cFbos->size());
  for(Fbo*const fCptr : *cFbos) fCptr->FboReInit();
  cLog->LogInfoExSafe("Fbos reinitialised $ objects.", cFbos->size());
}
/* -- De-initialise all Fbo's ---------------------------------------------- */
static void FboDeInit()
{ // Ignore if no Fbo's
  if(cFbos->empty()) return;
  // De-init all Fbo's (NOT destroy them!) and log pre/post operation.
  cLog->LogDebugExSafe("Fbos de-initialising $ objects...", cFbos->size());
  for(Fbo*const fCptr : *cFbos) fCptr->FboDeInit();
  cLog->LogInfoExSafe("Fbos de-initialised $ objects.", cFbos->size());
}
/* -- Set Fbo render order reserve ----------------------------------------- */
static CVarReturn FboSetOrderReserve(const size_t) { return ACCEPT; }
/* -- Return active fbo ---------------------------------------------------- */
static Fbo *FboActive(void) { return cFbos->fboActive; }
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
