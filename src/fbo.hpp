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
using namespace ICollector::P;         using namespace ICVarDef::P;
using namespace IError::P;             using namespace IFboDef::P;
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
  public FboItem,                      // Fbo item class
  public FboRenderItem,                // Render data
  public Ident,                        // Identifier class
  public Lockable                      // Lua lockable class
{ /* --------------------------------------------------------------- */ public:
  GLenum           ePolyMode;          // Frame buffer polygon mode
  FboCmdVec        fcvActive;          // Commands list
  FboFloatCoords   ffcStage;           // Stage co-ordinates
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
  GLuint           uiFBOtex,           // Frame buffer texture name
                   uiTextureCache,     // Last GL texture id used
                   uiTexUnitCache,     // Last GL multi-texture unit id used
                   uiShaderCache;      // Shader currently selected
  /* -- Constructor -------------------------------------------------------- */
  explicit FboBase(const GLint iPixFmt, const bool bLockable) :
    /* -- Initialisers ----------------------------------------------------- */
    Lockable{ bLockable },             ePolyMode(GL_FILL),
    iMagFilter(GL_NEAREST),            iMinFilter(GL_NEAREST),
    iPixFormat(iPixFmt),               iWrapMode(GL_CLAMP_TO_EDGE),
    ofeFilterId(OF_N_N),               stCommandsFrame(0),
    stGLArrayOff(0),                   stTrianglesFrame(0),
    stTrianglesLast(0),                uiFBOtex(0),
    uiTextureCache(0),                 uiTexUnitCache(0),
    uiShaderCache(0)
    /* --------------------------------------------------------------------- */
    {}
};/* ----------------------------------------------------------------------- */
/* == Fbo object class ===================================================== */
CTOR_MEM_BEGIN_CSLAVE(Fbos, Fbo, ICHelperUnsafe),
  /* -- Base classes ------------------------------------------------------- */
  public FboBase                       // Fbo base class
{ /* -- Select texture --------------------------------------------- */ public:
  void FboBindTexture()
    { GL(cOgl->BindTexture(uiFBOtex),
        "Failed to select FBO texture as active!",
        "Identifier", IdentGet(), "Texture", uiFBOtex); }
  /* -- (Un)bind framebuffer ----------------------------------------------- */
  void FboBind()
    { GL(cOgl->BindFBO(uiFBO), "Failed to select active FBO!",
        "Identifier", IdentGet(), "FBO", uiFBO); }
  /* -- Bind buffer and texture -------------------------------------------- */
  void FboBindAndTexture() { FboBind(); FboBindTexture(); }
  /* -- Set FBO transparency (must be called before initialisation) -------- */
  void FboSetTransparency(const bool bState)
    { iPixFormat = bState ? GL_RGBA8 : GL_RGB8; }
  bool FboIsTransparencyEnabled() const { return iPixFormat == GL_RGBA8; }
  /* -- Set wireframe mode ------------------------------------------------- */
  void FboSetWireframe(const bool bWireframe)
    { ePolyMode = bWireframe ? GL_LINE : GL_FILL; }
  /* -- Flush queue -------------------------------------------------------- */
  void FboFlush()
  { // Flush the vertex buffer and queue
    ftvActive.clear();
    fcvActive.clear();
    // Reset counters and caches
    uiTextureCache = uiTexUnitCache = uiShaderCache = 0;
    stTrianglesLast = stGLArrayOff = 0;
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
  void FboSetActive() { cParent->fboActive = this; }
  /* -- Finished with drawing in the FBO ----------------------------------- */
  void FboFinishQueue()
  { // Push the data we need to render the array
    fcvActive.push_back({
      uiTexUnitCache,                                             // uiTUId
      uiTextureCache,                                             // uiTexId
      uiShaderCache,                                              // uiPrgId
      reinterpret_cast<GLvoid*>(stGLArrayOff + stOffsetTxcData),  // vpTCOffset
      reinterpret_cast<GLvoid*>(stGLArrayOff + stOffsetPosData),  // vpVOffset
      reinterpret_cast<GLvoid*>(stGLArrayOff + stOffsetColData),  // vpCOffset
      static_cast<GLsizei>(FboGetTrisCmd() * stVertexPerTriangle),// uiVertices
    });
  }
  /* -- Finish and render the graphics ------------------------------------- */
  void FboFinishAndRender()
  { // Finish writing to the arrays
    FboFinishQueue();
    // Set current triangle and frame count
    stTrianglesFrame = FboGetTrisNow();
    stCommandsFrame = FboGetCmdsNow();
    // Select our FBO as render target
    cOgl->BindFBO(uiFBO);
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
    // For each command in this order
    for(FboCmdVecConstInt fclciIt{ fcvActive.cbegin() },
      fclItEnd{ next(fclciIt, UtilIntOrMax<ssize_t>(stCommandsFrame)) };
      fclciIt < fclItEnd; ++fclciIt)
    { // Get command data
      const FboCmd &fcData = *fclciIt;
      // Set texture, texture unit and shader program
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
      // Blit array
      cOgl->DrawArraysTriangles(fcData.uiVertices);
    } // Flush data so it can be used again
    FboFlush();
  }
  /* -- Finish the queue without checking and reset cache ------------------ */
  void FboFinishAndReset(const GLuint uiT, const GLuint uiTU,
    const GLuint uiSC)
  { // If we have triangles? check see if we finished using this texture name
    FboFinishQueue();
    // Reset the cache
    FboResetCache(uiT, uiTU, uiSC);
  }
  /* -- Check caches and start a new command if ids changed ---------------- */
  void FboCheckCache(const GLuint uiT, const GLuint uiTU, const GLuint uiSC)
  { // If...
    if(uiT != uiTextureCache ||        // ...texture id not changed -or-
       uiSC != uiShaderCache ||        // ...shader not changed -or-
       uiTU != uiTexUnitCache)         // ...texture unit not changed
      // Finish and reset cache
      FboFinishAndReset(uiT, uiTU, uiSC);
  }
  /* -- Blit the specified texture into the FBO ---------------------------- */
  void FboBlit(const GLuint uiTex, const TriPosData &fV,
    const TriCoordData &fTC, const TriColData &fC, const GLuint uiTexU,
    const Shader*const shProgram)
  { // If this is the first triangle in this command, we just init the cache
    if(stTrianglesLast == FboGetTrisNow())
      FboResetCache(uiTex, uiTexU, shProgram->GetProgram());
    // Check if texture id/unit or program changed and finish previous list
    else FboCheckCache(uiTex, uiTexU, shProgram->GetProgram());
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
  { // Select our FBO and texture
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
      GL(cOgl->SetTexParam(pItem.eWrap, iWrapMode),
        "Failed to set texture wrapping mode!",
        "Identifier", IdentGet(), "Mode", iWrapMode, "Type", pItem.cWrap);
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
  { // Select our FBO and texture
    FboBindAndTexture();
    // Set filtering
    GL(cOgl->SetTexParam(GL_TEXTURE_MIN_FILTER, iMinFilter),
      "Failed to set minification filter!",
      "Identifier", IdentGet(), "MinFilter", iMinFilter,
      "MagFilter",  iMagFilter);
    GL(cOgl->SetTexParam(GL_TEXTURE_MAG_FILTER, iMagFilter),
      "Failed to set magnification filter!",
      "Identifier", IdentGet(), "MinFilter", iMinFilter,
      "MagFilter",  iMagFilter);
  }
  /* -- Set filtering by ID and commit ------------------------------------- */
  void FboSetFilterCommit(const OglFilterEnum ofeId)
    { FboSetFilter(ofeId); FboCommitFilter(); }
  /* -- Set backbuffer blending mode --------------------------------------- */
  void FboSetBlend(const OglBlendEnum obeSFactorRGB,
    const OglBlendEnum obeDFactorRGB, const OglBlendEnum obeSFactorA,
    const OglBlendEnum obeDFactorA)
  { // OpenGL blending flags
    typedef array<const GLenum, OB_MAX> BlendFunctions;
    static const BlendFunctions aBlends
    { // ## RGB & Alpha Blend Factors StringId
      GL_ZERO,                         // 00 (0,0,0)            0      Z
      GL_ONE,                          // 01 (1,1,1)            1      O
      GL_SRC_COLOR,                    // 02 (Rs,Gs,Bs)         As     S_C
      GL_ONE_MINUS_SRC_COLOR,          // 03 (1,1,1)-(Rs,Gs,Bs) 1-As   O_M_S_C
      GL_DST_COLOR,                    // 04 (Rd,Gd,Bd)         Ad     D_C
      GL_ONE_MINUS_DST_COLOR,          // 05 (1,1,1)-(Rd,Gd,Bd) 1-Ad   O_M_D_C
      GL_SRC_ALPHA,                    // 06 (As,As,As)         As     S_A
      GL_ONE_MINUS_SRC_ALPHA,          // 07 (1,1,1)-(As,As,As) 1-As   O_M_S_A
      GL_DST_ALPHA,                    // 08 (Ad,Ad,Ad)         Ad     D_A
      GL_ONE_MINUS_DST_ALPHA,          // 09 (1,1,1)-(Ad,Ad,Ad) 1-Ad   O_M_D_A
      GL_CONSTANT_COLOR,               // 10 (Rc,Gc,Bc)         Ac     C_C
      GL_ONE_MINUS_CONSTANT_COLOR,     // 11 (1,1,1)-(Rc,Gc,Bc) 1-Ac   O_M_C_C
      GL_CONSTANT_ALPHA,               // 12 (Ac,Ac,Ac)         Ac     C_A
      GL_ONE_MINUS_CONSTANT_ALPHA,     // 13 (1,1,1)-(Ac,Ac,Ac) 1-Ac   O_M_C_A
      GL_SRC_ALPHA_SATURATE            // 14 (i,i,i)            1      S_A_S
    };
    // Lookup values and set
    SetSrcRGB(aBlends[obeSFactorRGB]);
    SetDstRGB(aBlends[obeDFactorRGB]);
    SetSrcAlpha(aBlends[obeSFactorA]);
    SetDstAlpha(aBlends[obeDFactorA]);
  }
  /* -- Set clear state ---------------------------------------------------- */
  void FboSetClear(const bool bState) { bClear = bState; }
  bool FboIsClearEnabled() const { return bClear; }
  /* -- Set clear colour for the FBO --------------------------------------- */
  void FboSetClearColour(const GLfloat fRed, const GLfloat fGreen,
    const GLfloat fBlue, const GLfloat fAlpha)
  { SetColourRed(fRed);   SetColourGreen(fGreen);
    SetColourBlue(fBlue); SetColourAlpha(fAlpha); }
  /* -- Set clear colour for the FBO as integer (console cvar) ------------- */
  void FboSetClearColourInt(const unsigned int uiC) { SetColourInt(uiC); }
  /* -- Reset clear colour ------------------------------------------------- */
  void FboResetClearColour() { ResetColour(); }
  /* -- Reserve memory for lists ------------------------------------------- */
  void FboReserve(const size_t stTri, const size_t stCmd)
    { fcvActive.reserve(stCmd); ftvActive.reserve(stTri); }
  /* -- Set matrix for drawing --------------------------------------------- */
  void FboSetMatrix(const GLfloat fLeft, const GLfloat fTop,
    const GLfloat fRight, const GLfloat fBottom)
  { // Set new stage bounds
    ffcStage.SetCoords(fLeft, fTop, fRight, fBottom);
    // Set matrix top-left co-ordinates
    SetCoLeft(ffcStage.GetCoLeft() < 0 ?
      -ffcStage.GetCoLeft() : ffcStage.GetCoLeft());
    SetCoTop(ffcStage.GetCoTop() < 0 ?
      -ffcStage.GetCoTop() : ffcStage.GetCoTop());
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
    if(cFbos->fboActive == this) cFbos->fboMain->FboSetActive();
    // Flush active triangle and command lists
    FboFlush();
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
  { // De-initialise old FBO first.
    FboDeInit();
    // Set identifier.
    IdentSet(strID);
    // Say we're initialising the frame buffer.
    cLog->LogDebugExSafe("Fbo initialising a $x$ object '$'...",
      siW, siH, IdentGet());
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
    // Generate the FBO, bind the FBO, generate the texture for the FBO,
    // commit the filter and wrapping setting for the FBO and verify that
    // the FBO was setup properly. Bind the newly created framebuffer.
    FboBind();
    // Generate texture name for FBO and bind it.
    GL(cOgl->CreateTexture(&uiFBOtex),
     "Failed to create texture for framebuffer!",
     "Identifier", IdentGet(), "Width",  siW,
     "Height",     siH,        "Buffer", &uiFBOtex);
    FboBindTexture();
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
    SetCoRight(static_cast<GLfloat>(siW));
    SetCoBottom(static_cast<GLfloat>(siH));
    // Commit the guest requested filter and wrapping texture properties
    FboCommitFilter();
    FboCommitWrap();
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
  ~Fbo() { FboDeInit(); }
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
/* ========================================================================= */
static void FboDeInit()
{ // Ignore if no FBO's
  if(cFbos->empty()) return;
  // De-init all FBO's (NOT destroy them!) and log pre/post operation.
  cLog->LogDebugExSafe("Fbos de-initialising $ objects...", cFbos->size());
  for(Fbo*const fCptr : *cFbos) fCptr->FboDeInit();
  cLog->LogInfoExSafe("Fbos de-initialised $ objects.", cFbos->size());
}
/* -- Set FBO render order reserve ----------------------------------------- */
static CVarReturn FboSetOrderReserve(const size_t)
  { return ACCEPT; }
/* -- Get active FBO ------------------------------------------------------- */
static Fbo *FboActive() { return cFbos->fboActive; }
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
