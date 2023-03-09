/* == OGL.HPP ============================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## Allows loading and execution of OpenGL functions.                   ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IOgl {                       // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IClock::P;             using namespace ICVar::P;
using namespace ICVarDef::P;           using namespace ICVarLib::P;
using namespace IError::P;             using namespace IEvtWin::P;
using namespace IFboDef::P;            using namespace IFlags;
using namespace IGlFW::P;              using namespace IGlFWUtil::P;
using namespace IHelper::P;            using namespace IIdent::P;
using namespace ILog::P;               using namespace IStd::P;
using namespace IString::P;            using namespace ISystem::P;
using namespace ISysUtil::P;           using namespace ITexDef::P;
using namespace IUtf::P;               using namespace IUtil::P;
using namespace Lib::OS::GlFW;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* -- GL error checking wrapper macros ------------------------------------- */
#define GLEX(EF,F,M,...)  { F; EF(M, ## __VA_ARGS__); }
#define GLL(F,M,...)      GLEX(cOgl->CheckLogError, F, M, ## __VA_ARGS__)
#define GL(F,M,...)       GLEX(cOgl->CheckExceptError, F, M, ## __VA_ARGS__)
#define GLC(M,...)        GLEX(cOgl->CheckExceptError, , M, ## __VA_ARGS__)
/* -- OpenGL flags --------------------------------------------------------- */
BUILD_FLAGS(Ogl,
  /* ----------------------------------------------------------------------- */
  // No flags                          OpenGL context initialised?
  GFL_NONE                  {Flag(0)}, GFL_INITIALISED           {Flag(1)},
  // Either of the below commands?     Have nVidia memory information?
  GFL_HAVEMEM               {Flag(2)}, GFL_HAVENVMEM             {Flag(3)},
  // Have ATI memory avail info?       Devices shares memory with system
  GFL_HAVEATIMEM            {Flag(4)}, GFL_SHARERAM              {Flag(5)}
);/* ----------------------------------------------------------------------- */
enum OglFilterEnum : size_t            // Available filter combinations
{ /* ----------------------------------------------------------------------- */
  OF_N_N,     OF_N_L,    OF_L_N,      OF_L_L,      OF_NM_MAX,
  OF_N_N_MM_N=OF_NM_MAX, OF_L_N_MM_N, OF_N_N_MM_L, OF_L_N_MM_L,
  OF_N_L_MM_N,           OF_L_L_MM_N, OF_N_L_MM_L, OF_L_L_MM_L,
  OF_MAX,
};/* ----------------------------------------------------------------------- */
enum OglBlendEnum : size_t             // Available blend combinations
{ /* ----------------------------------------------------------------------- */
  OB_Z,   OB_O,       OB_S_C,   OB_O_M_S_C, OB_D_C, OB_O_M_D_C,
  OB_S_A, OB_O_M_S_A, OB_D_A,   OB_O_M_D_A, OB_C_C, OB_O_M_C_C,
  OB_C_A, OB_O_M_C_A, OB_S_A_S, OB_MAX
};/* ---------------------------------------------------------------------- */
enum OglUndefinedEnums : GLenum        // Some undefined OpenGL consts
{ /* ----------------------------------------------------------------------- */
  GL_RGBA_DXT1               = 0x83F1, // GL_COMPRESSED_RGBA_S3TC_DXT1_EXT
  GL_RGBA_DXT3               = 0x83F2, // GL_COMPRESSED_RGBA_S3TC_DXT3_EXT
  GL_RGBA_DXT5               = 0x83F3, // GL_COMPRESSED_RGBA_S3TC_DXT5_EXT
};/* ----------------------------------------------------------------------- */
typedef vector<GLuint> GLUIntVector;   // Vector of GLuints
typedef vector<GLfloat> GLFloatVector; // Vector of GLfloats
/* -- OpenGL manager class ------------------------------------------------- */
static class Ogl final :               // OGL class for OpenGL use simplicity
  /* -- Sub-classes -------------------------------------------------------- */
  private InitHelper,                  // Initialisation helper
  public FboColour,                    // OpenGL colour
  public FboBlend,                     // OpenGL blend
  public OglFlags                      // OpenGL init flags
{ /* -- String defines ----------------------------------------------------- */
  const IdMap<GLenum> idExtensions,    // OpenGL extension names (log detail)
                      idHintTargets,   // Hint target names (log detail)
                      idHintModes,     // Hint mode values (log detail)
                      idFormatModes,   // Pixel format modes (log detail)
                      idOGLCodes;      // OpenGL codes
  /* -- Defines ------------------------------------------------------------ */
#define IGLL(F,M,...) GLEX(CheckLogError, F, M, ## __VA_ARGS__)
#define IGL(F,M,...)  GLEX(CheckExceptError, F, M, ## __VA_ARGS__)
#define IGLC(M,...)   GLEX(CheckExceptError, , M, ## __VA_ARGS__)
  /* ----------------------------------------------------------------------- */
  enum VSyncMode : int {               // VSync settings
    VSYNC_MIN      = -1,               // (-1) Minimum Vertical Sync value
    VSYNC_ON_ADAPT = VSYNC_MIN,        // (-1) Adaptive Vertical Sync enabled
    VSYNC_OFF,                         // ( 0) Vertical Sync disable
    VSYNC_ON,                          // ( 1) Vertical Sync enabled
    VSYNC_ON_HALFRATE,                 // ( 2) Verfical sync enabled (half)
    VSYNC_MAX                          // ( 3) Maximum Vertical Sync value
  } vsSetting;                         // Storage for setting
  /* -- Variables ---------------------------------------------------------- */
  GLuint           uiActiveFbo,        // Currently selected FBO name cache
                   uiActiveProgram,    // Currently active shader program
                   uiActiveTexture,    // Currently bound texture
                   uiActiveTUnit,      // Currently active texture unit
                   uiActiveVao,        // Currently active vertex array object
                   uiActiveVbo,        // Currently active vertex buffer object
                   uiTexSize,          // Maximum reported texture size
                   uiPackAlign,        // Default pack alignment
                   uiUnpackAlign,      // Default Unpack alignment
                   uiMaxVertexAttribs, // Maximum vertex attributes per shader
                   uiTexUnits,         // Texture units count
                   uiVAO,              // Vertex Array Object (only 1 needed)
                   uiVBO;              // Vertex Buffer Object (only 1 needed)
  GLenum           ePolyMode;          // Current polygon mode
  GLint            iUnpackRowLength;   // Default unpack row length
  GLuint64         qwMinVRAM,          // Minimum VRAM required
                   qwTotalVRAM,        // Maximum VRAM supported
                   qwFreeVRAM;         // Current VRAM available
  ClkDuration      cdLimit;            // Frame limit based on refresh rate
  string           strRenderer,        // GL renderer string
                   strVersion,         // GL version string
                   strVendor;          // GL vendor string
  /* -- Delayed destruction ------------------------------------------------ */
  /* Because LUA garbage collection could zap a texture or fbo class at any  */
  /* time, we need to delay deletion of textures and FBO handles in OpenGL   */
  /* so that a framebuffer can select/draw without binding non-existant      */
  /* handles. Contents will be destroyed after all drawing is completed!     */
  GLUIntVector     vTextures,          // Textures to destroy
                   vFbos;              // Fbos to destroy
  /* -- OpenGL functions (put in struct to zero easy) -------------- */ public:
  struct OpenGLAPI                     // OpenGL API functions
  { /* -- Callback type -------------- Function name ----------------------- */
    PFNGLACTIVETEXTUREPROC             glActiveTexture;
    PFNGLATTACHSHADERPROC              glAttachShader;
    PFNGLBINDATTRIBLOCATIONPROC        glBindAttribLocation;
    PFNGLBINDBUFFERPROC                glBindBuffer;
    PFNGLBINDFRAMEBUFFERPROC           glBindFramebuffer;
    PFNGLBINDTEXTUREPROC               glBindTexture;
    PFNGLBINDVERTEXARRAYPROC           glBindVertexArray;
    PFNGLBLENDFUNCSEPARATEPROC         glBlendFuncSeparate;
    PFNGLBUFFERDATAPROC                glBufferData;
    PFNGLCHECKFRAMEBUFFERSTATUSPROC    glCheckFramebufferStatus;
    PFNGLCLEARCOLORPROC                glClearColor;
    PFNGLCLEARPROC                     glClear;
    PFNGLCOMPILESHADERPROC             glCompileShader;
    PFNGLCOMPRESSEDTEXIMAGE2DPROC      glCompressedTexImage2D;
    PFNGLCREATEPROGRAMPROC             glCreateProgram;
    PFNGLCREATESHADERPROC              glCreateShader;
    PFNGLDELETEBUFFERSPROC             glDeleteBuffers;
    PFNGLDELETEFRAMEBUFFERSPROC        glDeleteFramebuffers;
    PFNGLDELETEPROGRAMPROC             glDeleteProgram;
    PFNGLDELETESHADERPROC              glDeleteShader;
    PFNGLDELETETEXTURESPROC            glDeleteTextures;
    PFNGLDELETEVERTEXARRAYSPROC        glDeleteVertexArrays;
    PFNGLDETACHSHADERPROC              glDetachShader;
    PFNGLDISABLEPROC                   glDisable;
    PFNGLDISABLEVERTEXATTRIBARRAYPROC  glDisableVertexAttribArray;
    PFNGLDRAWARRAYSPROC                glDrawArrays;
    PFNGLENABLEPROC                    glEnable;
    PFNGLENABLEVERTEXATTRIBARRAYPROC   glEnableVertexAttribArray;
    PFNGLFRAMEBUFFERTEXTURE2DPROC      glFramebufferTexture2D;
    PFNGLGENBUFFERSPROC                glGenBuffers;
    PFNGLGENERATEMIPMAPPROC            glGenerateMipmap;
    PFNGLGENFRAMEBUFFERSPROC           glGenFramebuffers;
    PFNGLGENTEXTURESPROC               glGenTextures;
    PFNGLGENVERTEXARRAYSPROC           glGenVertexArrays;
    PFNGLGETERRORPROC                  glGetError;
    PFNGLGETFLOATVPROC                 glGetFloatv;
    PFNGLGETINTEGERVPROC               glGetIntegerv;
    PFNGLGETPROGRAMINFOLOGPROC         glGetProgramInfoLog;
    PFNGLGETPROGRAMIVPROC              glGetProgramiv;
    PFNGLGETSHADERINFOLOGPROC          glGetShaderInfoLog;
    PFNGLGETSHADERIVPROC               glGetShaderiv;
    PFNGLGETSTRINGIPROC                glGetStringi;
    PFNGLGETSTRINGPROC                 glGetString;
    PFNGLGETTEXIMAGEPROC               glGetTexImage;
    PFNGLGETUNIFORMLOCATIONPROC        glGetUniformLocation;
    PFNGLHINTPROC                      glHint;
    PFNGLISENABLEDPROC                 glIsEnabled;
    PFNGLLINKPROGRAMPROC               glLinkProgram;
    PFNGLPIXELSTOREIPROC               glPixelStorei;
    PFNGLPOLYGONMODEPROC               glPolygonMode;
    PFNGLREADBUFFERPROC                glReadBuffer;
    PFNGLSHADERSOURCEPROC              glShaderSource;
    PFNGLTEXIMAGE2DPROC                glTexImage2D;
    PFNGLTEXPARAMETERIPROC             glTexParameteri;
    PFNGLTEXSUBIMAGE2DPROC             glTexSubImage2D;
    PFNGLUNIFORM1IPROC                 glUniform1i;
    PFNGLUNIFORM4FPROC                 glUniform4f;
    PFNGLUNIFORM4FVPROC                glUniform4fv;
    PFNGLUSEPROGRAMPROC                glUseProgram;
    PFNGLVERTEXATTRIBPOINTERPROC       glVertexAttribPointer;
    PFNGLVIEWPORTPROC                  glViewport;
    /* --------------------------------------------------------------------- */
  } sAPI;                              // API functions list
  /* -- GL error logger ---------------------------------------------------- */
  template<typename ...VarArgs>
    void CheckLogError(const char*const cpFormat,
      const VarArgs &...vaArgs) const
  { // While there are OpenGL errors
    for(GLenum eCode = sAPI.glGetError();
               eCode != GL_NO_ERROR;
               eCode = sAPI.glGetError())
    cLog->LogWarningExSafe("GL call failed: $ ($/$$).",
      StrFormat(cpFormat, vaArgs...), GetGLErr(eCode), hex, eCode);
  }
  /* -- GL error handler --------------------------------------------------- */
  template<typename ...VarArgs>
    void CheckExceptError(const char*const cpFormat,
      const VarArgs &...vaArgs) const
  { // If there is no error then return
    const GLenum eCode = sAPI.glGetError();
    if(eCode == GL_NO_ERROR) return;
    // Raise exception with error details
    XC(cpFormat, "Code", eCode, "Reason", GetGLErr(eCode), vaArgs...);
  }
  /* -- Flag setter ----------------------------------------------- */ private:
  void SetFlagExt(const char*const cpName, const OglFlagsConst &glFlag)
    { FlagSetOrClear(glFlag, HaveExtension(cpName)); }
  /* -- Load GL capabilities ----------------------------------------------- */
  void DetectCapabilities(void)
  { // Current current OpenGL strings
    strVendor = LuaUtilGetStr<char>(GL_VENDOR);
    strRenderer = LuaUtilGetStr<char>(GL_RENDERER);
    strVersion = LuaUtilGetStr<char>(GL_VERSION);
    // Get vendor specific memory info extensions
    SetFlagExt("GL_NVX_gpu_memory_info", GFL_HAVENVMEM);
    SetFlagExt("GL_ATI_meminfo", GFL_HAVEATIMEM);
    // Set flag if have either
    FlagSet(FlagIsAnyOfSet(GFL_HAVENVMEM|GFL_HAVEATIMEM) ?
      GFL_HAVEMEM : GFL_SHARERAM|GFL_HAVEMEM);
    // Cache maximum texture size (Minimum hardware support for 3.2 is 1024^2)
    uiTexSize = GetInteger<GLuint>(GL_MAX_TEXTURE_SIZE);
    uiMaxVertexAttribs = GetInteger<GLuint>(GL_MAX_VERTEX_ATTRIBS);
    // Cache texture unit count
    uiTexUnits = GetInteger<GLuint>(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS);
    // Cache current default pack alignment size
    uiPackAlign = GetInteger<GLuint>(GL_PACK_ALIGNMENT);
    uiUnpackAlign = GetInteger<GLuint>(GL_UNPACK_ALIGNMENT);
    iUnpackRowLength = GetInteger<GLint>(GL_UNPACK_ROW_LENGTH);
    // Load current clear colour
    GetFloatArray<4>(GL_COLOR_CLEAR_VALUE, GetColourMemory());
    // Load current blending settings
    SetSrcRGB(GetInteger<GLenum>(GL_BLEND_SRC_RGB));
    SetDstRGB(GetInteger<GLenum>(GL_BLEND_DST_RGB));
    SetSrcAlpha(GetInteger<GLenum>(GL_BLEND_SRC_ALPHA));
    SetDstAlpha(GetInteger<GLenum>(GL_BLEND_DST_ALPHA));
  }
  /* -- Zero index to hint helper for cvars -------------------------------- */
  static GLenum SHIndexToEnum(const size_t stIndex)
  { // Parameters available to translate
    typedef array<const GLenum, 4> Values;
    static const Values vaCmds{ GL_TRUE, GL_DONT_CARE, GL_FASTEST, GL_NICEST };
    // Return position or invalid
    return stIndex < vaCmds.size() ? vaCmds[stIndex] : GL_NONE;
  }
  /* -- Load GL extensions ------------------------------------------------- */
  const char *LoadFunctions(void)
  { // Helper macro
#define GETPTR(v,t) sAPI.v = reinterpret_cast<t>(GlFWGetProcAddress(#v)); \
    if(!sAPI.v) return #v;
    // Get basic ARB functions
    GETPTR(glActiveTexture, PFNGLACTIVETEXTUREPROC);
    GETPTR(glBindTexture, PFNGLBINDTEXTUREPROC);
    GETPTR(glBlendFuncSeparate, PFNGLBLENDFUNCSEPARATEPROC);
    GETPTR(glClear, PFNGLCLEARPROC);
    GETPTR(glClearColor, PFNGLCLEARCOLORPROC);
    GETPTR(glCompressedTexImage2D, PFNGLCOMPRESSEDTEXIMAGE2DPROC);
    GETPTR(glDeleteTextures, PFNGLDELETETEXTURESPROC);
    GETPTR(glDisable, PFNGLDISABLEPROC);
    GETPTR(glDrawArrays, PFNGLDRAWARRAYSPROC);
    GETPTR(glEnable, PFNGLENABLEPROC);
    GETPTR(glGenerateMipmap, PFNGLGENERATEMIPMAPPROC);
    GETPTR(glGenTextures, PFNGLGENTEXTURESPROC);
    GETPTR(glGetError, PFNGLGETERRORPROC);
    GETPTR(glGetFloatv, PFNGLGETFLOATVPROC);
    GETPTR(glGetIntegerv, PFNGLGETINTEGERVPROC);
    GETPTR(glGetString, PFNGLGETSTRINGPROC);
    GETPTR(glGetStringi, PFNGLGETSTRINGIPROC);
    GETPTR(glGetTexImage, PFNGLGETTEXIMAGEPROC);
    GETPTR(glHint, PFNGLHINTPROC);
    GETPTR(glIsEnabled, PFNGLISENABLEDPROC);
    GETPTR(glPixelStorei, PFNGLPIXELSTOREIPROC);
    GETPTR(glPolygonMode, PFNGLPOLYGONMODEPROC);
    GETPTR(glReadBuffer, PFNGLREADBUFFERPROC);
    GETPTR(glTexImage2D, PFNGLTEXIMAGE2DPROC);
    GETPTR(glTexParameteri, PFNGLTEXPARAMETERIPROC);
    GETPTR(glTexSubImage2D, PFNGLTEXSUBIMAGE2DPROC);
    GETPTR(glViewport, PFNGLVIEWPORTPROC);
    // Shader functions
    GETPTR(glAttachShader, PFNGLATTACHSHADERPROC);
    GETPTR(glBindAttribLocation, PFNGLBINDATTRIBLOCATIONPROC);
    GETPTR(glCompileShader, PFNGLCOMPILESHADERPROC);
    GETPTR(glCreateProgram, PFNGLCREATEPROGRAMPROC);
    GETPTR(glCreateShader, PFNGLCREATESHADERPROC);
    GETPTR(glDeleteProgram, PFNGLDELETEPROGRAMPROC);
    GETPTR(glDeleteShader, PFNGLDELETESHADERPROC);
    GETPTR(glDetachShader, PFNGLATTACHSHADERPROC);
    GETPTR(glGetProgramInfoLog, PFNGLGETPROGRAMINFOLOGPROC);
    GETPTR(glGetProgramiv, PFNGLGETPROGRAMIVPROC);
    GETPTR(glGetShaderInfoLog, PFNGLGETSHADERINFOLOGPROC);
    GETPTR(glGetShaderiv, PFNGLGETSHADERIVPROC);
    GETPTR(glGetUniformLocation, PFNGLGETUNIFORMLOCATIONPROC);
    GETPTR(glLinkProgram, PFNGLLINKPROGRAMPROC);
    GETPTR(glShaderSource, PFNGLSHADERSOURCEPROC);
    GETPTR(glUniform1i, PFNGLUNIFORM1IPROC);
    GETPTR(glUniform4f, PFNGLUNIFORM4FPROC);
    GETPTR(glUniform4fv, PFNGLUNIFORM4FVPROC);
    GETPTR(glUseProgram, PFNGLUSEPROGRAMPROC);
    // Vertex Buffer Object (VBO) functions
    GETPTR(glBindBuffer, PFNGLBINDBUFFERPROC);
    GETPTR(glBufferData, PFNGLBUFFERDATAPROC);
    GETPTR(glDeleteBuffers, PFNGLDELETEBUFFERSPROC);
    GETPTR(glGenBuffers, PFNGLGENBUFFERSPROC);
    // Vertex Memory Object (VAO) functions
    GETPTR(glBindVertexArray, PFNGLBINDVERTEXARRAYPROC);
    GETPTR(glDeleteVertexArrays, PFNGLDELETEVERTEXARRAYSPROC);
    GETPTR(glDisableVertexAttribArray, PFNGLDISABLEVERTEXATTRIBARRAYPROC);
    GETPTR(glEnableVertexAttribArray, PFNGLENABLEVERTEXATTRIBARRAYPROC);
    GETPTR(glGenVertexArrays, PFNGLGENVERTEXARRAYSPROC);
    GETPTR(glVertexAttribPointer, PFNGLVERTEXATTRIBPOINTERPROC);
    // Frame buffer functions
    GETPTR(glBindFramebuffer, PFNGLBINDFRAMEBUFFERPROC);
    GETPTR(glCheckFramebufferStatus, PFNGLCHECKFRAMEBUFFERSTATUSPROC);
    GETPTR(glDeleteFramebuffers, PFNGLDELETEFRAMEBUFFERSPROC);
    GETPTR(glFramebufferTexture2D, PFNGLFRAMEBUFFERTEXTURE2DPROC);
    GETPTR(glGenFramebuffers, PFNGLGENFRAMEBUFFERSPROC);
    // Done with this
#undef GETPTR
    // Log functions initialised
    cLog->LogDebugExSafe("OGL loaded $ function addresses.",
      sizeof(sAPI) / sizeof(void*));
    // Return success
    return nullptr;
  }
  /* == OpenGL features ============================================ */ public:
  bool HaveExtension(const char*const cpName) const
    { return !!glfwExtensionSupported(cpName); }
  /* ----------------------------------------------------------------------- */
  template<typename RetType=decltype(uiTexSize)>
    RetType MaxTexSize(void) const { return static_cast<RetType>(uiTexSize); }
  /* ----------------------------------------------------------------------- */
  GLuint MaxVertexAttribs(void) const { return uiMaxVertexAttribs; }
  /* ----------------------------------------------------------------------- */
  GLuint PackAlign(void) const { return uiPackAlign; }
  /* ----------------------------------------------------------------------- */
  GLuint UnpackAlign(void) const { return uiUnpackAlign; }
  /* ----------------------------------------------------------------------- */
  GLint UnpackRowLength(void) const { return iUnpackRowLength; }
  /* ----------------------------------------------------------------------- */
  void VertexAttribPointer(const GLuint uiAttrib, const GLint iSize,
    const GLsizei siStride, const GLvoid*const vpBuffer) const
      { sAPI.glVertexAttribPointer(uiAttrib, iSize, GL_FLOAT, GL_FALSE,
          siStride, vpBuffer); }
  /* ----------------------------------------------------------------------- */
  void SetPixelStore(const GLenum eId, const GLint iValue) const
    { sAPI.glPixelStorei(eId, iValue); }
  /* ----------------------------------------------------------------------- */
  void SetPackAlignment(const GLint iValue) const
    { SetPixelStore(GL_PACK_ALIGNMENT, iValue); }
  /* ----------------------------------------------------------------------- */
  void SetUnpackAlignment(const GLint iValue) const
    { SetPixelStore(GL_UNPACK_ALIGNMENT, iValue); }
  /* ----------------------------------------------------------------------- */
  void SetUnpackRowLength(const GLint iValue) const
    { SetPixelStore(GL_UNPACK_ROW_LENGTH, iValue); }
  /* ----------------------------------------------------------------------- */
  void ReadBuffer(const GLenum uiBuffer) const
    { sAPI.glReadBuffer(uiBuffer); }
  /* -- Texture functions -------------------------------------------------- */
  void ReadTexture(const GLenum eFormat, GLvoid*const vpBuffer) const
    { sAPI.glGetTexImage(GL_TEXTURE_2D, 0, eFormat, GL_UNSIGNED_BYTE,
        vpBuffer); }
  /* ----------------------------------------------------------------------- */
  void ReadTextureTT(const TextureType ttFormat, GLvoid*const vpBuffer) const
    { ReadTexture(TexTypeToNative<GLenum>(ttFormat), vpBuffer); }
  /* ----------------------------------------------------------------------- */
  template<typename IntType=GLenum>
    IntType TexTypeToNative(const TextureType ttType) const
  { // Setup translation list. Make sure it's the same order as shown in
    // ITexDef::P::TextureType in 'texdef.hpp'.
    typedef array<const GLenum, TT_MAX> TexTypeToGLenum;
    static const TexTypeToGLenum tttglLookup{
      GL_NONE,      /* TT_NONE */ GL_BGR,       /* TT_BGR */
      GL_BGRA,      /* TT_BGRA */ GL_RGBA_DXT1, /* TT_DXT1 */
      GL_RGBA_DXT3, /* TT_DXT3 */ GL_RGBA_DXT5, /* TT_DXT5 */
      GL_RED,       /* TT_GRAY */ GL_RG,        /* TT_GRAYALPHA */
      GL_RGB,       /* TT_RGB  */ GL_RGBA,      /* TT_RGBA */
    }; // Return casted value
    return static_cast<IntType>(tttglLookup[ttType]);
  }
  /* ----------------------------------------------------------------------- */
  void UploadTexture(const GLint iLevel, const GLsizei siWidth,
    const GLsizei siHeight, const GLint iFormat, const GLenum eType,
    const GLvoid*const vpBuffer) const
      { sAPI.glTexImage2D(GL_TEXTURE_2D, iLevel, iFormat, siWidth, siHeight, 0,
          eType, GL_UNSIGNED_BYTE, vpBuffer); }
  /* ----------------------------------------------------------------------- */
  void UploadTextureTT(const GLint iLevel, const GLsizei siWidth,
    const GLsizei siHeight, const TextureType ttFormat,
    const TextureType ttType, const GLvoid*const vpBuffer) const
      { UploadTexture(iLevel, siWidth, siHeight,
          TexTypeToNative<GLint>(ttFormat), TexTypeToNative<GLenum>(ttType),
          vpBuffer); }
  /* ----------------------------------------------------------------------- */
  void UploadCompressedTexture(const GLint iLevel, const GLenum eFormat,
    const GLsizei siWidth, const GLsizei siHeight, const GLsizei siSize,
    const GLvoid*const vpBuffer) const
      { sAPI.glCompressedTexImage2D(GL_TEXTURE_2D, iLevel, eFormat, siWidth,
          siHeight, 0, siSize, vpBuffer); }
  /* ----------------------------------------------------------------------- */
  void UploadCompressedTextureTT(const GLint iLevel,
    const TextureType ttFormat, const GLsizei siWidth, const GLsizei siHeight,
    const GLsizei siSize, const GLvoid*const vpBuffer) const
      { UploadCompressedTexture(iLevel, TexTypeToNative<GLenum>(ttFormat),
        siWidth, siHeight, siSize, vpBuffer); }
  /* ----------------------------------------------------------------------- */
  void UploadTextureSub(const GLint iLeft, const GLint iTop,
    const GLsizei siWidth, const GLsizei siHeight, const GLenum ePixFormat,
    const GLvoid*const vpBuffer) const
      { sAPI.glTexSubImage2D(GL_TEXTURE_2D, 0, iLeft, iTop, siWidth, siHeight,
          ePixFormat, GL_UNSIGNED_BYTE, vpBuffer); }
  /* ----------------------------------------------------------------------- */
  void UploadTextureSubTT(const GLint iLeft, const GLint iTop,
    const GLsizei siWidth, const GLsizei siHeight,
    const TextureType ttPixFormat, const GLvoid*const vpBuffer) const
      { UploadTextureSub(iLeft, iTop, siWidth, siHeight,
          TexTypeToNative<GLenum>(ttPixFormat), vpBuffer); }
  /* ----------------------------------------------------------------------- */
  void UploadTextureSub(const GLsizei siWidth, const GLsizei siHeight,
    const GLenum ePixFormat, const GLvoid*const vpBuffer) const
      { UploadTextureSub(0, 0, siWidth, siHeight, ePixFormat, vpBuffer); }
  /* ----------------------------------------------------------------------- */
  void GenerateMipmaps(void) const { sAPI.glGenerateMipmap(GL_TEXTURE_2D); }
  /* ----------------------------------------------------------------------- */
  void CreateTextures(const GLsizei siCount, GLuint*const uipTexture) const
    { sAPI.glGenTextures(siCount, uipTexture); }
  /* ----------------------------------------------------------------------- */
  template<class ListType>void CreateTextures(ListType &lIds) const
    { CreateTextures(static_cast<GLsizei>(lIds.size()), lIds.data()); }
  /* ----------------------------------------------------------------------- */
  void CreateTexture(GLuint*const uipTexture) const
    { CreateTextures(1, uipTexture); }
  /* ----------------------------------------------------------------------- */
  GLuint GetCurrentTexture(void) const
    { return GetInteger<GLuint>(GL_TEXTURE_BINDING_2D); }
  /* ----------------------------------------------------------------------- */
  void ActiveTexture(const GLuint uiTexUnit=0)
  { // If we're already the active texture then don't set it
    if(uiActiveTUnit == uiTexUnit) return;
    // Texture unit id lookup table
    typedef array<const GLenum, 3> TexUnits;
    static const TexUnits tuTexUnit{ GL_TEXTURE0, GL_TEXTURE1, GL_TEXTURE2 };
    // Activate texture
    sAPI.glActiveTexture(tuTexUnit[uiTexUnit]);
    // Update the selected texture unit
    uiActiveTUnit = uiTexUnit;
  }
  /* ----------------------------------------------------------------------- */
  void DeleteTexture(const GLsizei siCount, const GLuint*const uipTexture)
  { // Check each texture that is about to be deleted
    for(GLsizei siIndex = 0; siIndex < siCount; ++siIndex)
    { // Keep scanning until we find a bound texture
      if(uiActiveTexture != uipTexture[siIndex]) continue;
      // Reset currently bound texture
      uiActiveTexture = numeric_limits<GLuint>::max();
      // No need to check any others
      break;
    } // Delete the textures
    sAPI.glDeleteTextures(siCount, uipTexture);
  }
  /* ----------------------------------------------------------------------- */
  void BindTexture(const GLuint uiTexture=0)
  { // Ignore if already bound
    if(uiTexture == uiActiveTexture) return;
    // Bind the texture
    sAPI.glBindTexture(GL_TEXTURE_2D, uiTexture);
    // Set active texture
    uiActiveTexture = uiTexture;
  }
  /* -- Create multiple framebuffer objects -------------------------------- */
  void CreateFBOs(const GLsizei siCount, GLuint*const uipFbo) const
    { sAPI.glGenFramebuffers(siCount, uipFbo); }
  /* -- Create one framebuffer object -------------------------------------- */
  void CreateFBO(GLuint*const uipFbo) const { CreateFBOs(1, uipFbo); }
  /* ----------------------------------------------------------------------- */
  void BindFBO(const GLuint uiFbo=0)
  { // If we're already the active FBO, ignore
    if(uiActiveFbo == uiFbo) return;
    // Bind the FBO
    sAPI.glBindFramebuffer(GL_FRAMEBUFFER, uiFbo);
    // Cache the active fbo
    uiActiveFbo = uiFbo;
  }
  /* ----------------------------------------------------------------------- */
  void DeleteFBO(const GLsizei siCount, const GLuint*const uipFbo)
  { // Check each fbo that is about to be deleted
    for(GLsizei siIndex = 0; siIndex < siCount; ++siIndex)
    { // Keep scanning until we find a cached fbo
      if(uipFbo[siIndex] != uiActiveFbo) continue;
      // Reset currently selected fbo
      uiActiveFbo = numeric_limits<GLuint>::max();
      // No need to check any others
      break;
    } // Delete the fbo
    sAPI.glDeleteFramebuffers(siCount, uipFbo);
  }
  /* ----------------------------------------------------------------------- */
  void AttachTexFBO(const GLuint uiTId) const
    { sAPI.glFramebufferTexture2D(GL_FRAMEBUFFER,
      GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, uiTId, 0); }
  /* ----------------------------------------------------------------------- */
  GLuint VerifyFBO(void) const
    { return sAPI.glCheckFramebufferStatus(GL_FRAMEBUFFER); }
  /* -- GLSL functions ----------------------------------------------------- */
  GLuint CreateShader(const GLenum eType) const
    { return sAPI.glCreateShader(eType); }
  /* ----------------------------------------------------------------------- */
  void DeleteShader(const GLuint uiShader) const
    { sAPI.glDeleteShader(uiShader); }
  /* ----------------------------------------------------------------------- */
  void ShaderSource(const GLuint uiShader, const GLchar*const cpCode) const
    { sAPI.glShaderSource(uiShader, 1, &cpCode, nullptr); }
  /* ----------------------------------------------------------------------- */
  void CompileShader(const GLuint uiShader) const
    { sAPI.glCompileShader(uiShader); }
  /* ----------------------------------------------------------------------- */
  void AttachShader(const GLenum uiProgram, const GLuint uiShader) const
    { sAPI.glAttachShader(uiProgram, uiShader); }
  /* ----------------------------------------------------------------------- */
  void DetachShader(const GLenum uiProgram, const GLuint uiShader) const
    { sAPI.glDetachShader(uiProgram, uiShader); }
  /* -- Get compilation status --------------------------------------------- */
  GLenum GetCompileStatus(const GLuint uiProgram) const
    { return GetShaderInt<GLenum>(uiProgram, GL_COMPILE_STATUS); }
  /* ----------------------------------------------------------------------- */
  GLsizei GetShaderInfoLog(const GLuint uiPId, const GLsizei siMaxLength,
    GLchar*const cpDest) const
  { // Get shader log entry and return length
    GLsizei siLength;
    sAPI.glGetShaderInfoLog(uiPId, siMaxLength, &siLength, cpDest);
    return siLength;
  }
  /* -- Get compilation failure reason ------------------------------------- */
  const string GetCompileFailureReason(const GLuint uiId) const
  { // Make string to store the message and return generic string if empty
    string sErrMsg;
    sErrMsg.resize(GetShaderInt<size_t>(uiId, GL_INFO_LOG_LENGTH));
    if(sErrMsg.empty()) return "No reason";
    // Get the error message
    sErrMsg.resize(static_cast<size_t>(
      GetShaderInfoLog(uiId, static_cast<GLsizei>(sErrMsg.size()),
        UtfToNonConstCast<GLchar*>(sErrMsg.data()))));
    // Get error and if an error occured put it as a failure reason
    const GLenum eCode = GetError();
    if(eCode != GL_NO_ERROR || sErrMsg.empty())
      return StrFormat("Problem getting reason (0x$$)", hex, eCode);
    // Return the string while chopping off return characters
    return StrChop(sErrMsg);
  }
  /* -- Get linker status -------------------------------------------------- */
  GLenum GetLinkStatus(const GLuint uiProgram) const
    { return GetProgramInt<GLenum>(uiProgram, GL_LINK_STATUS); }
  /* ----------------------------------------------------------------------- */
  GLsizei GetProgramInfoLog(const GLuint uiProgram, const GLsizei siMaxLength,
    GLchar*const cpDest) const
  { // Get shader program log entry and return length
    GLsizei siLength;
    sAPI.glGetProgramInfoLog(uiProgram, siMaxLength, &siLength, cpDest);
    return siLength;
  }
  /* -- Get linker failure reason ------------------------------------------ */
  const string GetLinkFailureReason(const GLuint uiProgram) const
  { // Make string to store the message and return generic string if empty
    string sErrMsg;
    sErrMsg.resize(GetProgramInt<size_t>(uiProgram, GL_INFO_LOG_LENGTH));
    if(sErrMsg.empty()) return "No reason";
    // Get the error message and resize string as a result
    sErrMsg.resize(static_cast<size_t>(GetProgramInfoLog(uiProgram,
      static_cast<GLsizei>(sErrMsg.size()),
      UtfToNonConstCast<GLchar*>(sErrMsg.data()))));
    // Get error and if an error occured put it as a failure reason
    const GLenum eCode = GetError();
    if(eCode != GL_NO_ERROR || sErrMsg.empty())
      return StrFormat("Problem getting reason ($$)", hex, eCode);
    // Return the string while chopping off return characters
    return StrChop(sErrMsg);
  }
  /* ----------------------------------------------------------------------- */
  template<typename IntegerType>
    const IntegerType GetShaderInt(const GLuint uiShader,
      const GLenum eCmd) const
  { // Storage for result
    IntegerType tResult;
    // Do the query
    IGL(sAPI.glGetShaderiv(uiShader, eCmd, reinterpret_cast<GLint*>(&tResult)),
      "Get shader integer failed!", "Shader", uiShader, "Enum", eCmd);
    // return the result
    return tResult;
  }
  /* ----------------------------------------------------------------------- */
  GLuint CreateProgram(void) const { return sAPI.glCreateProgram(); }
  /* ----------------------------------------------------------------------- */
  template<typename IntegerType>
    const IntegerType GetProgramInt(const GLuint uiProgram,
      const GLenum eCmd) const
  { // Storage for result
    IntegerType tResult;
    // Do the query
    IGL(sAPI.glGetProgramiv(uiProgram, eCmd,
      reinterpret_cast<GLint*>(&tResult)),
        "Get program integer failed!", "Program", uiProgram, "Enum", eCmd);
    // return the result
    return tResult;
  }
  /* ----------------------------------------------------------------------- */
  void DeleteProgram(const GLuint uiProgram)
  { // Delete the program
    sAPI.glDeleteProgram(uiProgram);
    // If it was the active program then reset the cache
    if(uiActiveProgram == uiProgram)
      uiActiveProgram = numeric_limits<GLuint>::max();
  }
  /* ----------------------------------------------------------------------- */
  void LinkProgram(const GLuint uiProgram) const
    { sAPI.glLinkProgram(uiProgram); }
  /* ----------------------------------------------------------------------- */
  GLuint GetProgram(void) const
    { return GetInteger<GLuint>(GL_CURRENT_PROGRAM); }
  /* ----------------------------------------------------------------------- */
  void UseProgram(const GLuint uiProgram=0)
  { // Ignore if we already have the program selected
    if(uiProgram == uiActiveProgram) return;
    // Activate the shader program
    sAPI.glUseProgram(uiProgram);
    // Update the active program
    uiActiveProgram = uiProgram;
  }
  /* ----------------------------------------------------------------------- */
  GLint GetUniformLocation(const GLuint uiProgram,
    const GLchar*const cpValue) const
  { return sAPI.glGetUniformLocation(uiProgram, cpValue); }
  /* ----------------------------------------------------------------------- */
  void BindAttribLocation(const GLuint uiProgram, const GLuint uiAttrib,
    const GLchar*const cpValue) const
  { sAPI.glBindAttribLocation(uiProgram, uiAttrib, cpValue); }
  /* -- Assign a new vec4 array -------------------------------------------- */
  void Uniform(const GLint iUniform, const GLsizei iCount,
    const GLfloat*const fpValues) const
  { sAPI.glUniform4fv(iUniform, iCount, fpValues); }
  /* -- Assign a integer value to a uniform -------------------------------- */
  void Uniform(const GLint iUniform, const GLint iValue) const
    { sAPI.glUniform1i(iUniform, iValue); }
  /* -- Assign four floats to a uniform ------------------------------------ */
  void Uniform(const GLint iUniform, const GLfloat fV1, const GLfloat fV2,
    const GLfloat fV3, const GLfloat fV4) const
  { sAPI.glUniform4f(iUniform, fV1, fV2, fV3, fV4); }
  /* -- Create multiple vertex buffer objects ------------------------------ */
  void GenVertexBuffers(const GLsizei siCount, GLuint*const uipVbo) const
    { sAPI.glGenBuffers(siCount, uipVbo); }
  /* -- Create one vertex buffer object ------------------------------------ */
  void GenVertexBuffer(GLuint*const uipVbo) const
    { GenVertexBuffers(1, uipVbo); }
  /* -- Bind vertex buffer object ------------------------------------------ */
  void BindVertexBuffer(const GLenum eTarget, const GLuint uiVbo)
  { // Ignore if this vbo is already selected
    if(uiVbo == uiActiveVbo) return;
    // Bind the vbo
    sAPI.glBindBuffer(eTarget, uiVbo);
    // Is active vbo
    uiActiveVbo = uiVbo;
  }
  /* -- Bind vertex buffer object ------------------------------------------ */
  void BindStaticVertexBuffer(const GLuint uiVbo)
    { BindVertexBuffer(GL_ARRAY_BUFFER, uiVbo); }
  /* -- Delete multiple vertex buffer objects ------------------------------ */
  void DeleteVertexBuffers(const GLsizei siCount, const GLuint*const uipVbo)
  { // Check each vbo deleted
    for(GLsizei siIndex = 0; siIndex < siCount; ++siIndex)
    { // Keep scanning until we find a bound vbo
      if(uipVbo[siIndex] != uiActiveVbo) continue;
      // Reset currently bound vbo
      uiActiveVbo = numeric_limits<GLuint>::max();
      // No need to check any others
      break;
    } // Delete the vbo's
    sAPI.glDeleteBuffers(siCount, uipVbo);
  }
  /* -- Delete one vertex buffer object ------------------------------------ */
  void DeleteVertexBuffer(const GLuint uipVbo)
    { DeleteVertexBuffers(1, &uipVbo); }
  /* -- Generate vertex array objects -------------------------------------- */
  void GenVertexArrays(const GLsizei siCount, GLuint*const uipVao) const
    { sAPI.glGenVertexArrays(siCount, uipVao); }
  /* -- Generate a single vertex array object ------------------------------ */
  void GenVertexArray(GLuint*const uipVao) const
    { GenVertexArrays(1, uipVao); }
  /* -- Bind vertex array object ------------------------------------------- */
  void BindVertexArray(const GLuint uiVao)
  { // Ignore if this vao is already selected
    if(uiVao == uiActiveVao) return;
    // Bind the vertex array
    sAPI.glBindVertexArray(uiVao);
    // Is active vao
    uiActiveVao = uiVao;
  }
  /* -- Delete multiple vertex array objects ------------------------------- */
  void DeleteVertexArrays(const GLsizei siCount, const GLuint*const uipVao)
  { // Check each vao deleted
    for(GLsizei siIndex = 0; siIndex < siCount; ++siIndex)
    { // Keep scanning until we find a bound vao
      if(uipVao[siIndex] != uiActiveVao) continue;
      // Reset currently bound vao
      uiActiveVao = numeric_limits<GLuint>::max();
      // No need to check any others
      break;
    } // Delete the vao's
    sAPI.glDeleteVertexArrays(siCount, uipVao);
  }
  /* -- Delete single vertex array object ---------------------------------- */
  void DeleteVertexArray(const GLuint uipSrc)
    { DeleteVertexArrays(1, &uipSrc); }
  /* -- Data buffering functions ------------------------------------------- */
  void BufferData(const GLenum eTarget, const GLsizei siSize,
    const GLvoid*const vpBuffer, const GLenum eUsage) const
      { sAPI.glBufferData(eTarget, siSize, vpBuffer, eUsage); }
  /* ----------------------------------------------------------------------- */
  void BufferStaticData(const GLsizei siSize, const GLvoid*const vpBuffer)
    { BufferData(GL_ARRAY_BUFFER, siSize, vpBuffer, GL_STREAM_DRAW); }
  /* -- Clear bound fbo or back buffer ------------------------------------- */
  void ClearBuffer(void) const { sAPI.glClear(GL_COLOR_BUFFER_BIT); }
  /* -- Set clear colour --------------------------------------------------- */
  void CommitClearColour(void) const
   { sAPI.glClearColor(GetColourRed(),  GetColourGreen(),
                       GetColourBlue(), GetColourAlpha()); }
  /* ----------------------------------------------------------------------- */
  void SetClearColourInt(const unsigned int uiColour)
    { SetColourInt(uiColour); CommitClearColour(); }
  /* -- Set clear colour (applies to all fbos) ----------------------------- */
  void SetClearColourIfChanged(const FboColour &fcData)
    { if(SetColour(fcData)) CommitClearColour(); }
  /* ----------------------------------------------------------------------- */
  void SetAndClear(const FboColour &cCol)
    { SetClearColourIfChanged(cCol); ClearBuffer(); }
  /* -- Update polygon rendering mode -------------------------------------- */
  void SetPolygonMode(const GLenum eMode)
  { // Return if we already set this mode
    if(ePolyMode == eMode) return;
    // Set it and update cached mode
    sAPI.glPolygonMode(GL_FRONT_AND_BACK, eMode);
    ePolyMode = eMode;
  }
  /* ----------------------------------------------------------------------- */
  GLenum GetError(void) const { return sAPI.glGetError(); }
  /* -- Restore VSync setting ---------------------------------------------- */
  void RestoreVSync(void) const { GlFWSetVSync(vsSetting); }
  /* ----------------------------------------------------------------------- */
  void DrawArrays(const GLenum eMode, const GLint iFirst,
    const GLsizei siCount) const
      { sAPI.glDrawArrays(eMode, iFirst, siCount); }
  /* ----------------------------------------------------------------------- */
  void DrawArraysTriangles(const GLsizei siCount) const
    { DrawArrays(GL_TRIANGLES, 0, siCount); }
  /* ----------------------------------------------------------------------- */
  void SetViewport(const GLsizei siWidth, const GLsizei siHeight) const
    { sAPI.glViewport(0, 0, siWidth, siHeight); }
  /* -- Get openGL float array --------------------------------------------- */
  template<size_t stCount>
    void GetFloatArray(const GLenum eId, GLfloat *const fpDest) const
      { IGL(sAPI.glGetFloatv(eId, fpDest),
          "Get float array failed!", "Index", eId, "Count", stCount); }
  /* -- Get openGL float array --------------------------------------------- */
  template<size_t stCount, class ArrayType = array<GLfloat, stCount>>
    const ArrayType GetFloatArray(const GLenum eId) const
  { // Create array to return
    ArrayType aData;
    // Get specified value for enum and store it
    GetFloatArray<stCount>(eId, aData.data());
    // Return array
    return aData;
  }
  /* -- Get openGL int array ----------------------------------------------- */
  template<size_t stCount, class ArrayType = array<GLint, stCount>>
    const ArrayType GetIntegerArray(const GLenum eId) const
  { // Create array to return
    ArrayType aData;
    // Get specified value for enum and store it
    IGL(sAPI.glGetIntegerv(eId, aData.data()),
      "Get integer array failed!", "Index", eId, "Count", stCount);
    // Return array
    return aData;
  }
  /* -- Get openGL int ----------------------------------------------------- */
  template<typename IntegerType = GLint>
    IntegerType GetInteger(const GLenum eId) const
      { return static_cast<IntegerType>(GetIntegerArray<1>(eId)[0]); }
  /* -- Get openGL string -------------------------------------------------- */
  template<typename PtrType = GLubyte>
    const PtrType* LuaUtilGetStr(const GLenum eId) const
  { // Get the variable and throw error if occured
    const GLubyte*const ucpStr = sAPI.glGetString(eId);
    IGLC("Get string failed!", "Index", eId);
    // Sanity check actual string
    if(!ucpStr || !*ucpStr)
      XC("Invalid string returned!", "Index", eId, "String", ucpStr);
    // Return result
    return reinterpret_cast<const PtrType*>(ucpStr);
  }
  /* -- Do delete all the marked fbo handles ------------------------------- */
  void DeleteMarkedFboHandles(void)
  { // Delete the fbos
    IGLL(DeleteFBO(static_cast<GLsizei>(vFbos.size()), vFbos.data()),
      "OGL failed to destroy $ fbo handles!", vFbos.size());
    // Clear the list
    vFbos.clear();
  }
  /* -- Do delete all the marked texture handles --------------------------- */
  void DeleteMarkedTextureHandles(void)
  { // Delete the marked textures
    IGLL(DeleteTexture(static_cast<GLsizei>(vTextures.size()),
      vTextures.data()), "OGL failed to destroy $ texture handles!",
      vTextures.size());
    // Clear the list
    vTextures.clear();
  }
  /* -- Enable vertex attribute array ------------------------------ */ public:
  void EnableVertexAttribArray(const GLuint uiAId) const
    { sAPI.glEnableVertexAttribArray(uiAId); }
  /* -- Disable vertex attribute array ------------------------------------- */
  void DisableVertexAttribArray(const GLuint uiAId) const
    { sAPI.glDisableVertexAttribArray(uiAId); }
  /* -- Enable an extension ------------------------------------------------ */
  void EnableExtension(const GLenum eExtension) const
  { // Ignore if already disabled
    if(sAPI.glIsEnabled(eExtension)) return;
    // Enable the extension and log result
    IGL(sAPI.glEnable(eExtension), "Enable extension failed!",
      "Extension", eExtension, "ExtensionId", idExtensions.Get(eExtension));
    cLog->LogDebugExSafe("OGL enabled extension $<0x$$>.",
      idExtensions.Get(eExtension), hex, eExtension);
  }
  /* -- Disable an extension ----------------------------------------------- */
  void DisableExtension(const GLenum eExtension) const
  { // Ignore if already disabled
    if(!sAPI.glIsEnabled(eExtension)) return;
    // Disable the extension and log result
    IGL(sAPI.glDisable(eExtension), "Disable extension failed!",
      "Key", eExtension, "KeyId", idExtensions.Get(eExtension));
    cLog->LogDebugExSafe("OGL disabled extension $<0x$$>.",
      idExtensions.Get(eExtension), hex, eExtension);
  }
  /* -- Commit blending algorithms ----------------------------------------- */
  void CommitBlend(void) const
    { sAPI.glBlendFuncSeparate(GetSrcRGB(), GetDstRGB(),
                               GetSrcAlpha(), GetDstAlpha()); }
  /* -- Set blending algorithms -------------------------------------------- */
  void SetBlendIfChanged(const FboBlend &fbOther)
    { if(SetBlend(fbOther)) CommitBlend(); }
  /* -- Set texture parameter (No error checking needed) ------------------- */
  void SetTexParam(const GLenum eVar, const GLint iVal) const
    { sAPI.glTexParameteri(GL_TEXTURE_2D, eVar, iVal); }
  /* -- Convert pixel mode to string --------------------------------------- */
  template<typename IntType> // Forcing any type to GLenum
    const string_view &GetPixelFormat(const IntType itMode) const
       { return idFormatModes.Get(static_cast<GLenum>(itMode)); }
  /* -- Update hint -------------------------------------------------------- */
  void SetHint(const GLenum eTarget, const GLenum eMode) const
  { // Get opengl hint and throw if not failed else set hint
    IGL(sAPI.glHint(eTarget, eMode), "Failed to set hint!",
      "Target", idHintTargets.Get(eTarget), "TargetId", eTarget,
      "Mode",   idHintModes.Get(eMode),     "ModeId",   eMode);
    cLog->LogDebugExSafe("OGL set hint $<0x$$> to $<0x$>.",
      idHintTargets.Get(eTarget), hex, eTarget, idHintModes.Get(eMode), eMode);
  }
  /* -- GL is initialised? ------------------------------------------------- */
  bool IsGLInitialised(void) const { return FlagIsSet(GFL_INITIALISED); }
  bool IsGLNotInitialised(void) const { return !IsGLInitialised(); }
  void SetInitialised(const bool bState, const bool bForce=false)
  { // De-Initialising?
    if(!bState)
    { // If not initialised?
      if(IsGLNotInitialised())
      { // Only destructor allowed to continue
        if(bForce) return;
        // Show error
        XC("OGL is not initialised!");
      } // Log as de-initialised
      cLog->LogInfoSafe("OGL set to de-initialised.");
      // Return cleared flag
      return FlagClear(GFL_INITIALISED);
    } // Initialised and already initialised? Throw error
    if(!bForce && IsGLInitialised()) XC("OGL was already initialised!");
    // Detect GL capabilities
    DetectCapabilities();
    // Show change in state
    cLog->LogInfoExSafe(
      "OGL set initialised with capabilities 0x$$...\n"
      "- Renderer: $.\n"
      "- Version: $.\n"
      "- Vendor: $.",
      hex, FlagGet(), GetRenderer(), GetVersion(), GetVendor());
    // If debug log level?
    if(cLog->HasLevel(LH_DEBUG))
    { // Get number of extensions and return if we're not interested in them?
      const GLuint uiExts = GetExtensionCount();
      // Report device selected and basic capabilities
      cLog->LogNLCDebugExSafe(
        "- Clear colour: $,$,$,$; "      "Blend: $,$,$,$.\n"
        "- Maximum texture size: $$^2; " "Pack alignment: $.\n"
        "- Unpack alignment: $; "        "Unpack row length: $.\n"
        "- Vertex attributes: $; "       "Texture units: $.\n"
        "- Extensions count: $.",
        GetColourRed(),     GetColourGreen(),   GetColourBlue(),
        GetColourAlpha(),   GetSrcRGB(),        GetDstRGB(),
        GetSrcAlpha(),      GetDstAlpha(),
        dec,                MaxTexSize(),       PackAlign(),
        UnpackAlign(),      UnpackRowLength(),  MaxVertexAttribs(),
        uiTexUnits,         uiExts);
      // Build sorted list of extensions and log them all
      StrUIntMap mExts;
      for(GLuint uiI = 0; uiI < uiExts; ++uiI)
        mExts.insert({ GetExtension(uiI), uiI });
      for(const StrUIntMapPair &suimpPair : mExts)
        cLog->LogNLCDebugExSafe("- Have extension '$' (#$).",
          suimpPair.first, suimpPair.second);
    } // Show warning if not enough texture units
    if(uiTexUnits < 3)
      cLog->LogWarningSafe("Ogl detected only $ of the three texture units "
        "that are required for video playback!");
    // Set the initialised flag
    FlagSet(GFL_INITIALISED);
  }
  /* -- Set texture mode by filter id -------------------------------------- */
  void SetFilterById(const OglFilterEnum ofeId, GLint &iMin, GLint &iMag) const
  { // Filter table
    typedef array<const GLint, 2> TwoGLints;
    typedef array<const TwoGLints, OF_NM_MAX> TexFilterNMList;
    static const TexFilterNMList tfList{{
      // Point/Bilinear filtering options
      { GL_NEAREST, GL_NEAREST }, { GL_NEAREST, GL_LINEAR }, // 00-01
      { GL_LINEAR,  GL_NEAREST }, { GL_LINEAR,  GL_LINEAR }, // 02-03
    }};
    // Get filter lookup id and set values
    const TwoGLints &tfItem = tfList[ofeId];
    iMag = tfItem.front();
    iMin = tfItem.back();
  }
  /* -- Set texture mode by filter id -------------------------------------- */
  void SetMipMapFilterById(const OglFilterEnum ofeId, GLint &iMin,
    GLint &iMag) const
  { // Filter table
    typedef array<const GLint, 2> TwoGLints;
    typedef array<const TwoGLints, OF_MAX> TexFilterList;
    static const TexFilterList tfList{
    { // Point/Bilinear filtering options
      { GL_NEAREST, GL_NEAREST }, { GL_NEAREST, GL_LINEAR }, // 00-01
      { GL_LINEAR,  GL_NEAREST }, { GL_LINEAR,  GL_LINEAR }, // 02-03
      // Point/Bilinear/Mipmap/Trilinear filtering options
      { GL_NEAREST, GL_NEAREST_MIPMAP_NEAREST }, // 04
      { GL_LINEAR,  GL_NEAREST_MIPMAP_NEAREST }, // 05
      { GL_NEAREST, GL_NEAREST_MIPMAP_LINEAR },  // 06
      { GL_LINEAR,  GL_NEAREST_MIPMAP_LINEAR },  // 07
      { GL_NEAREST, GL_LINEAR_MIPMAP_NEAREST },  // 08
      { GL_LINEAR,  GL_LINEAR_MIPMAP_NEAREST },  // 09
      { GL_NEAREST, GL_LINEAR_MIPMAP_LINEAR },   // 10
      { GL_LINEAR,  GL_LINEAR_MIPMAP_LINEAR }    // 11
    } };
    // Get filter lookup id and set values
    const TwoGLints &iaPair = tfList[ofeId];
    iMag = iaPair.front();
    iMin = iaPair.back();
  }
  /* -- Extensions --------------------------------------------------------- */
  GLuint GetExtensionCount(void) const
    { return GetInteger<GLuint>(GL_NUM_EXTENSIONS); }
  /* ----------------------------------------------------------------------- */
  const char *GetExtension(const GLuint uiI) const
    { return reinterpret_cast<const char*>
        (sAPI.glGetStringi(GL_EXTENSIONS, uiI)); }
  /* -- Read OpenGL renderer data ------------------------------------------ */
  const string &GetVendor(void) const { return strVendor; }
  const string &GetRenderer(void) const { return strRenderer; }
  const string &GetVersion(void) const { return strVersion; }
  /* -- Reset all binds ---------------------------------------------------- */
  void ResetBinds(void)
  { // Unbind active texture
    ActiveTexture();
    // Unbind shader program
    UseProgram();
    // Unbind texture
    BindTexture();
    // Unbind fbo and select main back buffer
    BindFBO();
    // Select default vertex buffer
    BindStaticVertexBuffer(uiVBO);
    // Select default vertex array
    BindVertexArray(uiVAO);
  }
  /* -- Mark an array of textures for deletion ----------------------------- */
  template<class List>void SetDeleteTextures(const List &vT)
    { vTextures.insert(vTextures.cend(), vT.cbegin(), vT.cend()); }
  /* -- Mark a single texture for deletion --------------------------------- */
  void SetDeleteTexture(const GLuint uiI)
    { vTextures.emplace_back(uiI); }
  /* -- Mark an fbo for deletion ------------------------------------------- */
  void SetDeleteFbo(const GLuint uiI)
    { vFbos.emplace_back(uiI); }
  /* -- Do delete all the marked textures and fbos ------------------------- */
  void DeleteTexturesAndFboHandles(void)
  { // Have textures to delete?
    if(!vTextures.empty()) DeleteMarkedTextureHandles();
    // Have fbos to delete?
    if(!vFbos.empty()) DeleteMarkedFboHandles();
  }
  /* -- Get available and total VRAM --------------------------------------- */
  GLuint64 GetVRAMFree(void) const { return qwFreeVRAM; }
  GLuint64 GetVRAMTotal(void) const { return qwTotalVRAM; }
  GLuint64 GetVRAMUsed(void) const { return GetVRAMTotal() - GetVRAMFree(); }
  double GetVRAMFreePC(void) const {
    return 100.0 - ((static_cast<double>(GetVRAMFree()) /
      GetVRAMTotal()) * 100.0);
  }
  /* -- Get free memory on nvidia cards ------------------------------------ */
  void UpdateVRAMAvailableNV(void)
  { // - OG macro name: GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX
    // - https://www.khronos.org/registry/OpenGL/extensions/
    //     NVX/NVX_gpu_memory_info.txt
    qwFreeVRAM = GetInteger<GLuint64>(0x9049) * 1024;
  }
  /* -- Get free memory on ATI cards --------------------------------------- */
  void UpdateVRAMAvailableATI(void)
  { // - OG macro name: TEXTURE_FREE_MEMORY_ATI
    // - https://www.khronos.org/registry/OpenGL/extensions/
    //     ATI/ATI_meminfo.txt
    qwFreeVRAM = static_cast<GLuint64>(GetIntegerArray<4>(0x87FC)[0]) * 1024;
    // Update total if higher
    if(qwFreeVRAM > qwTotalVRAM) qwTotalVRAM = qwFreeVRAM;
  }
  /* -- Get free memory on shared system memory renderer ------------------- */
  void UpdateVRAMAvailableShared(void)
  { // Update system memory usage data
    cSystem->UpdateMemoryUsageData();
    // Get system values
    qwFreeVRAM = cSystem->RAMFree();
  }
  /* -- Get memory information --------------------------------------------- */
  void UpdateVRAMAvailable(void)
  { // Have NVIDIA free memory?
    if(FlagIsSet(GFL_HAVENVMEM)) UpdateVRAMAvailableNV();
    // Have ATI free memory
    else if(FlagIsSet(GFL_HAVEATIMEM)) UpdateVRAMAvailableATI();
    // Have shared memory
    else if(FlagIsSet(GFL_SHARERAM)) UpdateVRAMAvailableShared();
  }
  /* -- Verify RAM constraints --------------------------------------------- */
  void VerifyVRAMConstraints(void)
  { // Have nVIDIA memory info?
    if(FlagIsSet(GFL_HAVENVMEM))
    { // Get total video memory
      // - OG macro name: GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX
      qwTotalVRAM = static_cast<GLuint64>(GetInteger<GLuint>(0x9048)) * 1024;
      // Update available VRAM
      UpdateVRAMAvailableNV();
      // Report VRAM information to log
      cLog->LogDebugSafe("- Using NVIDIA memory functions.");
    } // Have ATI memory info?
    else if(FlagIsSet(GFL_HAVEATIMEM))
    { // No total video memory on ATI cards so let UpdateVRAMAvailable set it.
      qwTotalVRAM = 0;
      // Update available VRAM
      UpdateVRAMAvailableATI();
      // Report VRAM information to log
      cLog->LogDebugSafe("- Using ATI memory functions (no total available).");
    } // Device shares memory with system?
    else if(FlagIsSet(GFL_SHARERAM))
    { // Update total memory information
      qwTotalVRAM = cSystem->RAMTotal();
      // Update memory information
      UpdateVRAMAvailableShared();
      // Report VRAM information to log
      cLog->LogDebugSafe("- Renderer shares memory with system.");
    } // Have no memory information command?
    else
    { // Set maximum total VRAM
      qwTotalVRAM = qwFreeVRAM = numeric_limits<GLuint64>::max();
      // Report VRAM information to log
      cLog->LogWarningSafe("- Video memory data functions not available.");
      // No need to check anything
      return;
    } // Load free VRAM and if we don't have enough free VRAM? Throw error
    if(qwMinVRAM && qwFreeVRAM < qwMinVRAM)
      XC("There is not enough video memory available. Close any "
         "running applications consuming it and try running again!",
         "Available", qwFreeVRAM,      "Total",  qwTotalVRAM,
         "Percent",   UtilMakePercentage(qwFreeVRAM, qwTotalVRAM),
         "Required",  qwMinVRAM,       "Needed", qwMinVRAM - qwFreeVRAM);
    // Report VRAM information to log
    cLog->LogInfoExSafe("- Total VRAM: $ bytes ($).\n"
                "- Available VRAM: $ bytes ($).",
      qwTotalVRAM, StrToBytes(qwTotalVRAM), qwFreeVRAM,
      StrToBytes(qwFreeVRAM));
  }
  /* --------------------------------------------------------------- */ public:
  template<typename IntType>
    const string_view &GetGLErr(const IntType itCode) const
      { return idOGLCodes.Get(static_cast<GLenum>(itCode)); }
  /* -- Return limit ------------------------------------------------------- */
  double GetLimit(void) const { return ClockDurationToDouble(cdLimit); }
  /* -- Update window size limits ------------------------------------------ */
  void UpdateWindowSizeLimits(void)
  { // Get app specified minimums and maximums
    const unsigned int
      uiWMin = cCVars->GetInternal<unsigned int>(WIN_WIDTHMIN),
      uiWMax = cCVars->GetInternal<unsigned int>(WIN_HEIGHTMIN),
      uiHMin = cCVars->GetInternal<unsigned int>(WIN_WIDTHMAX),
      uiHMax = cCVars->GetInternal<unsigned int>(WIN_HEIGHTMAX);
    // Set the window size limits. The specified maximum must not exceed the
    // video cards maximum texture size or perhaps BOOM! (not tested though).
    cEvtWin->Add(EWC_WIN_LIMITS, uiWMin, uiWMax,
      uiHMin ? UtilMinimum(uiHMin, MaxTexSize()) : MaxTexSize(),
      uiHMax ? UtilMinimum(uiHMax, MaxTexSize()) : MaxTexSize());
  }
  /* -- Initialise --------------------------------------------------------- */
  void Init(const int iRefresh, const bool bForce=false)
  { // Class initialised
    if(!bForce) IHInitialise();
    // Log class initialising
    cLog->LogDebugSafe("OGL subsystem initialising...");
    // Set context
    cGlFW->WinSetContext();
    // Load GL functions and throw exception with reason if not all loaded
    if(const char*const cpErr = LoadFunctions())
      XCS("Failed to load OpenGL export!", "Function", cpErr);
    // Clear error that Wine or GLFW might have caused
    if(const GLenum eError = sAPI.glGetError())
      cLog->LogDebugExSafe("Ogl cleared host caused error code! ($/$$).",
        GetGLErr(eError), hex, eError);
    // Set frame limit
    cdLimit = duration_cast<ClkDuration>(
      duration<double>{ 1.0 / static_cast<double>(iRefresh) });
    // OpenGL is now initialised
    SetInitialised(true, bForce);
    // Check that there is enough VRAM available if requested
    VerifyVRAMConstraints();
    // Init vsync
    RestoreVSync();
    // Enable extensions
    EnableExtension(GL_BLEND);
    EnableExtension(GL_DITHER);
    // Set hints. Indicates the accuracy of the derivative calculation for the
    // GL shading language fragment processing built-in functions: dFdx, dFdy,
    // and fwidth.
    SetQShaderHint(cCVars->GetInternal<size_t>(VID_QSHADER));
    // Indicates the sampling quality of antialiased lines. If a larger filter
    // function is applied, hinting GL_NICEST can result in more pixel
    // fragments being generated during rasterization.
    SetQLineHint(cCVars->GetInternal<size_t>(VID_QLINE));
    // Indicates the sampling quality of antialiased polygons. Hinting
    // GL_NICEST can result in more pixel fragments being generated during
    // rasterization, if a larger filter function is applied.
    SetQPolygonHint(cCVars->GetInternal<size_t>(VID_QPOLYGON));
    // Indicates the quality and performance of the compressing texture images.
    // Hinting GL_FASTEST indicates that texture images should be compressed as
    // quickly as possible, while GL_NICEST indicates that texture images
    // should be compressed with as little image quality loss as possible.
    // GL_NICEST should be selected if the texture is to be retrieved by
    // glGetCompressedTexImage for reuse.
    SetQCompressHint(cCVars->GetInternal<size_t>(VID_QCOMPRESS));
    // Setup window size limits
    UpdateWindowSizeLimits();
    // Set pack alignment for grabbing screenshots and unpack alignment for
    // uploading odd sized textures.
    IGL(SetPackAlignment(1), "Set byte pack alignment failed!");
    IGL(SetUnpackAlignment(1), "Set byte unpack alignment failed!");
    // Create and bind VAO
    IGL(GenVertexArray(&uiVAO), "Generate default VAO failed!");
    IGL(BindVertexArray(uiVAO), "Bind default VAO failed!", "Index", uiVAO);
    // Create and bind VBO
    IGL(GenVertexBuffer(&uiVBO), "Generate default VBO failed!");
    IGL(BindStaticVertexBuffer(uiVBO), "Bind default VBO failed!",
      "Index", uiVBO);
    // Log class initialising
    cLog->LogInfoSafe("OGL subsystem initialised.");
  }
  /* -- DeInitialise ------------------------------------------------------- */
  void DeInit(const bool bForce=false)
  { // Ignore if class not initialised
    if(IHNotDeInitialise()) return;
    // OpenGL is now de-initialised
    SetInitialised(false, bForce);
    // Log class initialising
    cLog->LogDebugSafe("OGL subsystem de-initialising...");
    // Reset all binds
    ResetBinds();
    // Have textures to delete?
    if(!vTextures.empty())
    { // Delete the texture handles
      cLog->LogDebugExSafe("OGL deleting $ marked texture handles...",
        vTextures.size());
      DeleteMarkedTextureHandles();
      cLog->LogInfoSafe("OGL deleted marked texture handles.");
    } // Have fbos to delete?
    if(!vFbos.empty())
    { // Delete the fbo handles
      cLog->LogDebugExSafe("OGL deleting $ marked fbo handles...",
        vFbos.size());
      DeleteMarkedFboHandles();
      cLog->LogInfoSafe("OGL deleted marked fbo handles.");
    } // Vertex buffer object created?
    if(uiVBO)
    { // Delete vertex buffer object
      cLog->LogDebugExSafe("OGL deleting vertex buffer object $...", uiVBO);
      IGLL(DeleteVertexBuffer(uiVBO),
        "Failed to delete vertex buffer object!", "Index", uiVBO);
      cLog->LogInfoExSafe("OGL deleted vertex buffer object $.", uiVBO);
      // Clear value
      uiVBO = 0;
    } // Vertex array object created?
    if(uiVAO)
    { // Delete vertex array object
      cLog->LogDebugExSafe("OGL deleting vertex array object $...", uiVAO);
      IGLL(DeleteVertexArray(uiVAO),
        "Failed to delete vertex array object!", "Index", uiVAO);
      cLog->LogInfoExSafe("OGL deleted vertex array object $.", uiVAO);
      // Clear value
      uiVAO = 0;
    } // Release opengl context
    GlFWReleaseContext();
    // Init flags
    FlagReset(GFL_NONE);
    // Pack alignment and texture size and caches
    uiActiveFbo = uiActiveProgram = uiActiveTexture = uiActiveTUnit =
      uiActiveVao = uiActiveVbo = numeric_limits<GLuint>::max();
    uiPackAlign = uiTexUnits = uiMaxVertexAttribs = 0;
    ePolyMode = GL_NONE;
    // Set blank generic text for strings
    strVendor.clear();
    strVersion.clear();
    strRenderer.clear();
    // Log class initialising
    cLog->LogInfoSafe("OGL subsystem de-initialised.");
  }
  /* -- Constructor -------------------------------------------------------- */
  Ogl(void) :
    /* -- Initialisers ----------------------------------------------------- */
    InitHelper{ __FUNCTION__ },        // Send name to InitHelper
    OglFlags{ GFL_NONE },              // Set no flags
    /* -- Const members ---------------------------------------------------- */
    idExtensions{{                     // Init GL extension names
      IDMAPSTR(GL_BLEND),              IDMAPSTR(GL_DITHER),
      IDMAPSTR(GL_POLYGON_SMOOTH),     IDMAPSTR(GL_LINE_SMOOTH),
    }, "GL_EXT_UNKNOWN" },             // Unknown extension name
    idHintTargets{{                    // Init hint target strings
      IDMAPSTR(GL_FRAGMENT_SHADER_DERIVATIVE_HINT),
      IDMAPSTR(GL_LINE_SMOOTH_HINT),   IDMAPSTR(GL_POLYGON_SMOOTH_HINT),
      IDMAPSTR(GL_TEXTURE_COMPRESSION_HINT),
    }, "GL_HINT_TARGET_UNKNOWN" },     // Unknown hint target name
    idHintModes{{                      // INit hint mode strings
      IDMAPSTR(GL_FASTEST),            IDMAPSTR(GL_NICEST),
      IDMAPSTR(GL_DONT_CARE)
    }, "GL_HINT_MODE_UNKNOWN"},        // Unknown hint mode name
    idFormatModes{{                    // Pixel format modes
      IDMAPSTR(GL_RGBA8),              IDMAPSTR(GL_RGB8),
      IDMAPSTR(GL_RED),                IDMAPSTR(GL_RG),
      IDMAPSTR(GL_RGB),                IDMAPSTR(GL_BGR),
      IDMAPSTR(GL_RGBA),               IDMAPSTR(GL_BGRA),
      IDMAPSTR(GL_RGBA_DXT1),          IDMAPSTR(GL_RGBA_DXT3),
      IDMAPSTR(GL_RGBA_DXT5)
    }, "GL_FORMAT_UNKNOWN"},           // Unknown pixel format mode
    idOGLCodes{{                       // Init error codes
      IDMAPSTR(GL_NO_ERROR),           IDMAPSTR(GL_INVALID_ENUM),
      IDMAPSTR(GL_INVALID_VALUE),      IDMAPSTR(GL_INVALID_OPERATION),
#if !defined(MACOS)                    // These only work on non-Apple targets
      IDMAPSTR(GL_STACK_OVERFLOW),     IDMAPSTR(GL_STACK_UNDERFLOW),
#endif                                 // End of Apple target check
      IDMAPSTR(GL_OUT_OF_MEMORY),
    }, "GL_ERROR_UNKNOWN" },           // Unknown error value
    /* -- Initialisers ----------------------------------------------------- */
    vsSetting{ VSYNC_OFF },            // Set no VSync
    uiActiveFbo(                       // Select back buffer
      numeric_limits<GLuint>::max()),  // Maxed so values commit properly
    uiActiveProgram(uiActiveFbo),      // No active shader programme
    uiActiveTexture(uiActiveFbo),      // No active texture
    uiActiveTUnit(uiActiveFbo),        // No active texture unit
    uiActiveVao(uiActiveFbo),          // No active vertex array object
    uiActiveVbo(uiActiveFbo),          // No active vertex buffer object
    uiTexSize(0),                      // No maximum texture size
    uiPackAlign(0),                    // No pack align
    uiUnpackAlign(0),                  // No unpack align
    uiMaxVertexAttribs(0),             // No maximum vertex attributes
    uiTexUnits(0),                     // No maximum texture units
    uiVAO(0),                          // No default vertex array object
    uiVBO(0),                          // No default vertex buffer object
    ePolyMode(GL_NONE),                // No set polygon mode yet
    iUnpackRowLength(0),               // No unpack row length
    qwMinVRAM(0),                      // No minimum vram
    qwTotalVRAM(0),                    // No total vram
    qwFreeVRAM(0),                     // No free vram
    cdLimit{ milliseconds{0} }         // Init frame duration
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Destructor --------------------------------------------------------- */
  DTORHELPER(~Ogl, DeInit(true))
  /* -- Setup VSync -------------------------------------------------------- */
  CVarReturn SetVSyncMode(const int iValue)
  { // Deny if the value is not valid
    if(CVarSimpleSetIntNLGE(vsSetting, static_cast<VSyncMode>(iValue),
      VSYNC_MIN, VSYNC_MAX) == DENY) return DENY;
    // If opengl is already initalised then update the new value immediately
    if(IsGLInitialised()) RestoreVSync();
    // Done
    return ACCEPT;
  }
  /* -- Update texture destroy list size ----------------------------------- */
  CVarReturn SetTexDListReserve(const size_t stCount)
    { return BoolToCVarReturn(UtilReserveList(vTextures, stCount)); }
  /* -- Update fbo destroy list size --------------------------------------- */
  CVarReturn SetFboDListReserve(const size_t stCount)
    { return BoolToCVarReturn(UtilReserveList(vFbos, stCount)); }
  /* -- Update minimum VRAM ------------------------------------------------ */
  CVarReturn SetMinVRAM(const GLuint64 qwValue)
    { return CVarSimpleSetInt(qwMinVRAM, qwValue); }
  /* -- Update hints ------------------------------------------------------- */
  CVarReturn SetQCompressHint(const size_t stMode)
  { // Ignore if no context, but still succeeded
    if(IsGLNotInitialised()) return ACCEPT;
    // Get new value to set and return false if invalid
    switch(const GLenum eNew = SHIndexToEnum(stMode))
    { // Invalid parameter or zero
      case GL_NONE: case GL_TRUE: return DENY;
      // Anything else, set the hint
      default: SetHint(GL_TEXTURE_COMPRESSION_HINT, eNew); break;
    } // Succeeded
    return ACCEPT;
  }
  /* -- Update hints ------------------------------------------------------- */
  CVarReturn SetQPolygonHint(const size_t stMode)
  { // Ignore if no context, but still succeeded
    if(IsGLNotInitialised()) return ACCEPT;
    // Get new value to set and return false if invalid
    switch(const GLenum eNew = SHIndexToEnum(stMode))
    { // Invalid parameter
      case GL_NONE: return DENY;
      // If feature is to be disabled? Disable it
      case GL_TRUE: DisableExtension(GL_POLYGON_SMOOTH); break;
      // Anything else, set the hint and enable it
      default: SetHint(GL_POLYGON_SMOOTH_HINT, eNew);
               EnableExtension(GL_POLYGON_SMOOTH); break;
    } // Succeeded
    return ACCEPT;
  }
  /* -- Update hints ------------------------------------------------------- */
  CVarReturn SetQLineHint(const size_t stMode)
  { // Ignore if no context, but still succeeded
    if(IsGLNotInitialised()) return ACCEPT;
    // Get new value to set and return false if invalid
    switch(const GLenum eNew = SHIndexToEnum(stMode))
    { // Invalid parameter
      case GL_NONE: return DENY;
      // If feature is to be disabled? Disable it
      case GL_TRUE: DisableExtension(GL_LINE_SMOOTH); break;
      // Anything else, set the hint and enable it
      default: SetHint(GL_LINE_SMOOTH_HINT, eNew);
               EnableExtension(GL_LINE_SMOOTH); break;
    } // Succeeded
    return ACCEPT;
  }
  /* -- Update hints ------------------------------------------------------- */
  CVarReturn SetQShaderHint(const size_t stMode)
  { // Ignore if no context, but still succeeded
    if(IsGLNotInitialised()) return ACCEPT;
    // Get new value to set and return false if invalid
    switch(const GLenum eNew = SHIndexToEnum(stMode))
    { // Invalid parameter or zero
      case GL_NONE: case GL_TRUE: return DENY;
      // Anything else, set the hint
      default: SetHint(GL_FRAGMENT_SHADER_DERIVATIVE_HINT, eNew); break;
    } // Succeeded
    return ACCEPT;
  }
  /* -- Undefines ---------------------------------------------------------- */
#undef IGLC                            // This macro was only for this class
#undef IGL                             // This macro was only for this class
#undef IGLL                            // This macro was only for this class
  /* ----------------------------------------------------------------------- */
} *cOgl = nullptr;                     // Pointer to static class
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
