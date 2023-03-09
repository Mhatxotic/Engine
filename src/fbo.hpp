/* == FBO.HPP ============================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## This the file handles rendering of 2d fbos and triangles in opengl  ## **
** ##                                                                     ## **
** ## TODO: Add custom shaders and expose the functions to LUA. Probably  ## **
** ##       only allow them to be attached to custom FBO's so the guest   ## **
** ##       can't override the built in shaders.                          ## **
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
using namespace IShaders::P;           using namespace IStd::P;
using namespace IString::P;            using namespace ISysUtil::P;
using namespace ITimer::P;             using namespace IUtil::P;
using namespace Lib::OS::GlFW::Types;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* == Fbo collector class for collector data and custom variables ========== */
CTOR_BEGIN_NOBASE(Fbos, Fbo, CLHelperUnsafe,
/* -- Fbo collector variables ---------------------------------------------- */
struct OrderItem :                     // Order item structure
  /* -- Base classes ------------------------------------------------------- */
  public FboRenderItem                 // Order structure
{ /* -- Variables ---------------------------------------------------------- */
  Fbo             *fboDest;            // Reference to fbo to draw to
  GLsizei          siVertices;         // No. of vertices in fbo gtlData
  ssize_t          stCommands;         // No. of commands in fbo gclData
  /* -- Init constructor --------------------------------------------------- */
  OrderItem(const FboRenderItem &friOther, Fbo*const fboNDest,
    const GLsizei siNVertices, const ssize_t stNCommands) :
    /* -- Initialisers ----------------------------------------------------- */
    FboRenderItem{ friOther },         fboDest(fboNDest),
    siVertices(siNVertices),           stCommands(stNCommands)
    /* -- No code ---------------------------------------------------------- */
    { }
}; /* ---------------------------------------------------------------------- */
Fbo               *fboActive;          // Pointer to active FBO object
Fbo               *fboMain;            // Pointer to main FBO object
/* -- Fbo order list ------------------------------------------------------- */
typedef vector<OrderItem> OrderVec;    // Vector of Fbo render details
OrderVec           ovActive;           // Active fbo order list to render
);/* ----------------------------------------------------------------------- */
typedef Fbos::OrderItem                FboOrderItem;  // Fbo order item
typedef Fbos::OrderVec                 FboOrderVec;   // " vector for items
typedef Fbos::OrderVec::const_iterator FboOrderVecIt; // "   "    iterator
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
{ /* -- Texture ---------------------------------------------------- */ public:
  OglFilterEnum    ofeFilterId;        // Chosen filter value
  GLint            iMinFilter,         // Frame buffer minification filter
                   iMagFilter,         // Frame buffer magnification filter
                   iWrapMode,          // Frame buffer wrapping mode
                   iPixFormat;         // Frame buffer pixel format
  GLenum           ePolyMode;          // Frame buffer polygon mode
  /* ----------------------------------------------------------------------- */
  FboTriVec        ftvActive;          // Triangles list
  FboCmdVec        fcvActive;          // Commands list
  /* -- Buffers ------------------------------------------------------------ */
  GLuint           uiTextureCache,     // Last GL texture id used
                   uiTexUnitCache,     // Last GL multi-texture unit id used
                   uiShaderCache;      // Shader currently selected
  size_t           stGLArrayOff,       // Current rendering offset
                   stTrianglesLast,    // Triangles before last cache change
                   stTrianglesFrame,   // Triangles this frame
                   stCommandsFrame,    // Commands this frame
                   stFinishCounter;    // Times fbo added to render queue
  /* -- Variables ---------------------------------------------------------- */
  GLuint           uiFBOtex;           // Frame buffer texture name
  FboFloatCoords   ffcStage;           // Stage co-ordinates
  /* -- Constructor -------------------------------------------------------- */
  explicit FboBase(const GLint iPixFmt, const bool bLockable) :
    /* -- Initialisers ----------------------------------------------------- */
    Lockable{ bLockable },             ofeFilterId(OF_N_N),
    iMinFilter(GL_NEAREST),            iMagFilter(GL_NEAREST),
    iWrapMode(GL_CLAMP_TO_EDGE),       iPixFormat(iPixFmt),
    ePolyMode(GL_FILL),                uiTextureCache(0),
    uiTexUnitCache(0),                 uiShaderCache(0),
    stGLArrayOff(0),                   stTrianglesLast(0),
    stTrianglesFrame(0),               stCommandsFrame(0),
    stFinishCounter(0),                uiFBOtex(0)
    /* --------------------------------------------------------------------- */
    { }
};/* ----------------------------------------------------------------------- */
/* == Fbo object class ===================================================== */
CTOR_MEM_BEGIN_CSLAVE(Fbos, Fbo, ICHelperUnsafe),
  /* -- Base classes ------------------------------------------------------- */
  public FboBase                       // Fbo base class
{ /* -- Select texture --------------------------------------------- */ public:
  void FboBindTexture(void)
    { GL(cOgl->BindTexture(uiFBOtex),
        "Failed to select FBO texture as active!",
        "Identifier", IdentGet(), "Texture", uiFBOtex); }
  /* -- (Un)bind framebuffer ----------------------------------------------- */
  void FboBind(void)
    { GL(cOgl->BindFBO(uiFBO), "Failed to select active FBO!",
        "Identifier", IdentGet(), "FBO", uiFBO); }
  /* -- Bind buffer and texture -------------------------------------------- */
  void FboBindAndTexture(void) { FboBind(); FboBindTexture(); }
  /* -- Set FBO transparency (must be called before initialisation) -------- */
  void FboSetTransparency(const bool bState)
    { iPixFormat = bState ? GL_RGBA8 : GL_RGB8; }
  bool FboIsTransparencyEnabled(void) const { return iPixFormat == GL_RGBA8; }
  /* -- Set wireframe mode ------------------------------------------------- */
  void FboSetWireframe(const bool bWireframe)
    { ePolyMode = bWireframe ? GL_LINE : GL_FILL; }
  /* -- Flush the vertex buffer and queue ---------------------------------- */
  void FboClearLists(void) { ftvActive.clear(); fcvActive.clear(); }
  /* -- Flush queue -------------------------------------------------------- */
  void FboFlush(void)
  { // Flush the vertex buffer and queue
    FboClearLists();
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
  /* -- Set main fbo command reserve --------------------------------------- */
  bool FboReserveCommands(const size_t stCount)
    { return UtilReserveList(fcvActive, stCount); }
  /* -- Set main fbo float reserve ----------------------------------------- */
  bool FboReserveTriangles(const size_t stCount)
    { return UtilReserveList(ftvActive, stCount); }
  /* -- Return number of commands parsed last frame ---------------- */ public:
  size_t FboGetCmdsNow(void) const { return fcvActive.size(); }
  size_t FboGetCmds(void) const { return stCommandsFrame; }
  size_t FboGetCmdsReserved(void) const { return fcvActive.capacity(); }
  /* -- Return number of triangles parsed last frame ----------------------- */
  size_t FboGetTrisCmd(void) const
     { return FboGetTrisNow() - stTrianglesLast; }
  size_t FboGetTrisNow(void) const { return ftvActive.size(); }
  size_t FboGetTris(void) const { return stTrianglesFrame; }
  size_t FboGetTrisReserved(void) const { return ftvActive.capacity(); }
  /* -- Return number of times fbo was added to the render list ------------ */
  size_t FboGetFinishCount(void) const { return stFinishCounter; }
  /* -- Activate this fbo -------------------------------------------------- */
  void FboSetActive(void) { cParent->fboActive = this; }
  /* -- Perform rendering inside the FBO to the texture -------------------- */
  void FboRender(const FboOrderItem &foiRef)
  { // Select our FBO as render target
    cOgl->BindFBO(foiRef.uiFBO);
    // Set the viewport of the framebuffer to the total visible pixels
    cOgl->SetViewport(foiRef.DimGetWidth(), foiRef.DimGetHeight());
    // Set blending mode
    cOgl->SetBlendIfChanged(foiRef);
    // Clear the fbo if requested
    if(foiRef.bClear) cOgl->SetAndClear(foiRef);
    // No point in continuing if there are no vertices
    if(!foiRef.siVertices) return;
    // Set polygon fill mode
    cOgl->SetPolygonMode(ePolyMode);
    // Update matrix on each 2D shader...
    for(const Shader &shBuiltIn : cShaderCore->sh2DBuiltIns)
      shBuiltIn.UpdateMatrix(foiRef);
    // Buffer the new vertex data
    cOgl->BufferStaticData(foiRef.siVertices, ftvActive.data());
    // For each command in this order
    for(FboCmdVecConstInt fclciIt{ fcvActive.cbegin() },
                          fclItEnd{ next(fclciIt, foiRef.stCommands) };
                          fclciIt < fclItEnd;
                        ++fclciIt)
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
    }
  }
  /* -- Render and flush if the last reference ----------------------------- */
  void FboRenderAndFlush(const FboOrderItem &foiRef)
  { // Render the fbo
    FboRender(foiRef);
    // Flush lists if reference is zero
    if(!--stFinishCounter) FboFlush();
  }
  /* -- Finished with drawing in the FBO ----------------------------------- */
  void FboFinishQueue(void)
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
  void FboFinishAndRender(void)
  { // Finish writing to the arrays
    FboFinishQueue();
    // Set current triangle and frame count
    stTrianglesFrame = FboGetTrisNow();
    stCommandsFrame = FboGetCmdsNow();
    // Add current count to fbo rendering queue
    cParent->ovActive.push_back({ *this, this,
      static_cast<GLsizei>(stTrianglesFrame * sizeof(FboTri)),
      UtilIntOrMax<ssize_t>(stCommandsFrame) });
    // Increment number of times this fbo is referenced in the active list,
    // this is so when the reference counter is reduced the zero, the triangles
    // and command lists are permitted to clear on FboRender(). Return if it
    // was added for the first time this frame.
    if(++stFinishCounter == 1) return;
    // Throw an error because it is not optimal to finish an fbo more than once
    // in the same frame. We still need to finish this fbo though as we've
    // probably already added data for it with Blit() functions so do not try
    // to raise the order of this check.
    XC("FBO already finished!",
       "Identifier", IdentGet(), "Count", stFinishCounter,
       "Frame",      cTimer->TimerGetTicks());
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
  /* -- Blit the specified triangle of the specifed fbo to this fbo -------- */
  void FboBlitTri(Fbo &fboSrc, const size_t stId)
    { FboBlit(fboSrc.uiFBOtex, fboSrc.FboItemGetVData(stId),
        fboSrc.FboItemGetTCData(stId), fboSrc.FboItemGetCData(stId), 0,
        &cShaderCore->sh2D); }
  /* -- Blit the specified fbo texture into this fbo ----------------------- */
  void FboBlit(Fbo &fboSrc)
    { for(size_t stId = 0; stId < stTrisPerQuad; ++stId)
        FboBlitTri(fboSrc, stId); }
  /* -- Blit the specified quad into this fbo ------------------------------ */
  void FboBlit(FboItem &fboiSrc, const GLuint uiTex, const GLuint uiTexU,
    const Shader*const shProgram)
      { for(size_t stId = 0; stId < stTrisPerQuad; ++stId)
          FboBlit(uiTex, fboiSrc.FboItemGetVData(stId),
            fboiSrc.FboItemGetTCData(stId), fboiSrc.FboItemGetCData(stId),
            uiTexU, shProgram); }
  /* -- Set wrapping mode -------------------------------------------------- */
  void FboSetWrap(const GLint iM) { iWrapMode = iM; }
  /* -- Commit wrapping mode ----------------------------------------------- */
  void FboCommitWrap(void)
  { // Select our fbo and texture
    FboBindAndTexture();
    // Structure of the wrapping types
    struct Procedure { const GLenum eWrap; const char cWrap; };
    typedef array<const Procedure, 3> Procedures;
    // Procedures to perform
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
  OglFilterEnum FboGetFilter(void) const { return ofeFilterId; }
  void FboSetFilter(const OglFilterEnum ofeId)
  { // Translate our filter id to min and mag filter
    cOgl->SetFilterById(ofeId, iMinFilter, iMagFilter);
    // Record filter id
    ofeFilterId = ofeId;
  }
  /* -- Commit new filter modes -------------------------------------------- */
  void FboCommitFilter(void)
  { // Select our fbo and texture
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
  { // New filters
    FboSetFilter(ofeId);
    // Apply the filters
    FboCommitFilter();
  }
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
  bool FboIsClearEnabled(void) const { return bClear; }
  /* -- Set clear colour for the FBO --------------------------------------- */
  void FboSetClearColour(const GLfloat fRed, const GLfloat fGreen,
    const GLfloat fBlue, const GLfloat fAlpha)
  { SetColourRed(fRed);   SetColourGreen(fGreen);
    SetColourBlue(fBlue); SetColourAlpha(fAlpha); }
  /* -- Set clear colour for the FBO as integer (console cvar) ------------- */
  void FboSetClearColourInt(const unsigned int uiC) { SetColourInt(uiC); }
  /* -- Reset clear colour ------------------------------------------------- */
  void FboResetClearColour(void) { ResetColour(); }
  /* -- Set matrix for drawing --------------------------------------------- */
  void FboSetMatrix(const GLfloat fLeft, const GLfloat fTop,
    const GLfloat fRight, const GLfloat fBottom)
  { // Set new stage bounds
    ffcStage.SetCoLeft(fLeft);
    ffcStage.SetCoTop(fTop);
    ffcStage.SetCoRight(fRight);
    ffcStage.SetCoBottom(fBottom);
    // Set matrix top-left co-ordinates
    SetCoLeft(ffcStage.GetCoLeft() < 0 ?
      -ffcStage.GetCoLeft() : ffcStage.GetCoLeft());
    SetCoTop(ffcStage.GetCoTop() < 0 ?
      -ffcStage.GetCoTop() : ffcStage.GetCoTop());
  }
  /* -- Reserve floats for vertex array ------------------------------------ */
  bool FboReserve(const size_t stTri, const size_t stCmd)
    { return FboReserveCommands(stCmd) && FboReserveTriangles(stTri); }
  /* -- ReInitialise ------------------------------------------------------- */
  void FboReInit(void) { FboInit(IdentGet(), DimGetWidth(), DimGetHeight()); }
  /* -- DeInitialise ------------------------------------------------------- */
  void FboDeInit(void)
  { // Remove as active fbo if set
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
    { // Mark fbo for deletion (explanation at top)
      cOgl->SetDeleteFbo(uiFBO);
      // Log the de-initialised
      cLog->LogDebugExSafe("Fbo '$' removed from $.", IdentGet(), uiFBO);
      // Done with this fbo handle
      uiFBO = 0;
    } // Return if fbo is not in the fbo order list
    if(!stFinishCounter) return;
    stFinishCounter = 0;
    // Get reference to order list and return if empty
    FboOrderVec &fpvList = cParent->ovActive;
    if(fpvList.empty()) return;
    // Find the fbo in the order list and erase it if we found it? Erase from
    // list if this is our fbo else try next fbo
    for(FboOrderVecIt fboIt{ fpvList.cbegin() }; fboIt != fpvList.cend();)
      if(fboIt->fboDest == this) fboIt = fpvList.erase(fboIt);
      else ++fboIt;
  }
  /* -- Initialise --------------------------------------------------------- */
  void FboInit(const string &strID, const GLsizei siW, const GLsizei siH)
  { // De-initialise old FBO first.
    FboDeInit();
    // Set identifier.
    IdentSet(strID);
    // Say we're initialising the frame buffer.
    cLog->LogDebugExSafe("Fbo initialising a $x$ object '$'...",
      siW, siH, IdentGet());
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
    // Generate texture name for FBO.
    GL(cOgl->CreateTexture(&uiFBOtex),
     "Failed to create texture for framebuffer!",
     "Identifier", IdentGet(), "Width",  siW,
     "Height",     siH,        "Buffer", &uiFBOtex);
    // Bind the texture so we can set it up
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
  Fbo(void) :                          // No parameters
    /* -- Initialisers ----------------------------------------------------- */
    ICHelperFbo{ cFbos, this },        // Initially registered
    IdentCSlave{ cParent->CtrNext() }, // Initialise identification number
    FboBase{ GL_RGBA8, false }         // Has alpha channel and not protected
    /* -- Code ------------------------------------------------------------- */
    { }                                // Do nothing else
  /* -- Constructor WITHOUT registration (used for core Fbos)--------------- */
  Fbo(const GLint iPixFmt,             // Pixel format requested
      const bool bLocked) :            // Locked from garbage collector
    /* -- Initialisers ----------------------------------------------------- */
    ICHelperFbo{ cFbos },              // Initially unregistered
    IdentCSlave{ cParent->CtrNext() }, // Initialise identification number
    FboBase{ iPixFmt, bLocked }        // Init alpha channel and lua protect
    /* -- Code ------------------------------------------------------------- */
    { }                                // Do nothing else
  /* -- Destructor --------------------------------------------------------- */
  ~Fbo(void) { FboDeInit(); }
};/* ----------------------------------------------------------------------- */
CTOR_END(Fbos, Fbo, FBO,,,, fboActive(nullptr), fboMain(nullptr))
/* ========================================================================= */
static void FboRender(void)
{ // If there are fbo's in the queue?
  if(!cFbos->ovActive.empty())
  { // For each FBO, reset the texture cache of the fbo if neccesary
    for(const FboOrderItem &foiRef : cFbos->ovActive)
      foiRef.fboDest->FboRenderAndFlush(foiRef);
    // Clear the fbo order list
    cFbos->ovActive.clear();
  } // Free textures and fbo's marked for deletion
  cOgl->DeleteTexturesAndFboHandles();
}
/* ========================================================================= */
static void FboReInit(void)
{ // Ignore if no fbos
  if(cFbos->empty()) return;
  // Re-init all fbos and log pre/post operation.
  cLog->LogDebugExSafe("Fbos re-initialising $ objects...", cFbos->size());
  for(Fbo*const fCptr : *cFbos) fCptr->FboReInit();
  cLog->LogInfoExSafe("Fbos re-initialised $ objects.", cFbos->size());
}
/* ========================================================================= */
static void FboDeInit(void)
{ // Ignore if no fbos
  if(cFbos->empty()) return;
  // De-init all fbos (NOT destroy them!) and log pre/post operation.
  cLog->LogDebugExSafe("Fbos de-initialising $ objects...", cFbos->size());
  for(Fbo*const fCptr : *cFbos) fCptr->FboDeInit();
  cLog->LogInfoExSafe("Fbos de-initialised $ objects.", cFbos->size());
}
/* -- Set fbo render order reserve ----------------------------------------- */
static CVarReturn FboSetOrderReserve(const size_t stCount)
  { return BoolToCVarReturn(UtilReserveList(cFbos->ovActive, stCount)); }
/* -- Get active FBO ------------------------------------------------------- */
static Fbo *FboActive(void) { return cFbos->fboActive; }
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
