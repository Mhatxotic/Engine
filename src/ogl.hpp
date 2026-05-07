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
using namespace IClock::P;             using namespace ICommon::P;
using namespace ICVar::P;              using namespace ICVarDef::P;
using namespace ICVarLib::P;           using namespace IError::P;
using namespace IEvtWin::P;            using namespace IFboBlend::P;
using namespace IFboCmd::P;            using namespace IColour::P;
using namespace IFlags::P;             using namespace IGlFW::P;
using namespace IGlFWUtil::P;          using namespace IHelper::P;
using namespace ILog::P;               using namespace ILookupMap::P;
using namespace IShaderDef::P;         using namespace IStd::P;
using namespace IString::P;            using namespace ISystem::P;
using namespace ISysUtil::P;           using namespace ITexDef::P;
using namespace IUtf::P;               using namespace IUtil::P;
using namespace Lib::OS::GlFW;
/* ------------------------------------------------------------------------- */
constexpr static const GLuint gluMax = StdLimits<GLuint>::max();
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* -- GL error checking wrapper macros ------------------------------------- */
#define GLEX(EF,F,M,...)  do{ F; EF(M, ## __VA_ARGS__); }while(false)
#define GLL(F,M,...)      GLEX(cOgl->CheckLogError, F, M, ## __VA_ARGS__)
#define GL(F,M,...)       GLEX(cOgl->CheckExceptError, F, M, ## __VA_ARGS__)
#define GLC(M,...)        GLEX(cOgl->CheckExceptError, , M, ## __VA_ARGS__)
/* -- Public typedefs ------------------------------------------------------ */
BUILD_FLAGS(Ogl,                       // OpenGL flags
  /* ----------------------------------------------------------------------- */
  GFL_NONE                  {Flag(0)}, // No flags
  GFL_INITIALISED           {Flag(1)}, // OpenGL context initialised?
  GFL_HAVEMEM               {Flag(2)}, // Either of the below commands?
  GFL_HAVENVMEM             {Flag(3)}, // Have nVidia memory information?
  GFL_HAVEATIMEM            {Flag(4)}, // Have ATI memory avail info?
  GFL_SHARERAM              {Flag(5)}, // Devices shares memory with system?
  GFL_HAVEASYNC             {Flag(6)}, // Have adaptive sync? (negative vsync)
  GFL_HAVEWASYNC            {Flag(7)}, // Have Windows adaptive sync?
  GFL_HAVELASYNC            {Flag(8)}  // Have Linux adaptive sync?
);/* ----------------------------------------------------------------------- */
enum OglFilterEnum : size_t            // Available filter combinations
{ /* -- Non-mipmapped ------------------------------------------------------ */
  OF_N_N,                              // [00] GL_NEAREST GL_LINEAR
  OF_N_L,                              // [01] GL_NEAREST GL_LINEAR
  OF_L_N,                              // [02] GL_LINEAR  GL_NEAREST
  OF_L_L,                              // [03] GL_LINEAR  GL_LINEAR
  /* ----------------------------------------------------------------------- */
  OF_NM_MAX,                           // [04] Non-mipmap filters count
  /* -- Mipmapped ---------------------------------------------------------- */
  OF_N_N_MM_N             = OF_NM_MAX, // [04] GL_NEAREST GL_NEAREST GL_NEAREST
  OF_L_N_MM_N,                         // [05] GL_LINEAR  GL_NEAREST GL_NEAREST
  OF_N_N_MM_L,                         // [06] GL_NEAREST GL_NEAREST GL_LINEAR
  OF_L_N_MM_L,                         // [07] GL_LINEAR  GL_NEAREST GL_LINEAR
  OF_N_L_MM_N,                         // [08] GL_NEAREST GL_LINEAR  GL_NEAREST
  OF_L_L_MM_N,                         // [09] GL_LINEAR  GL_LINEAR  GL_NEAREST
  OF_N_L_MM_L,                         // [10] GL_NEAREST GL_LINEAR  GL_LINEAR
  OF_L_L_MM_L,                         // [11] GL_LINEAR  GL_LINEAR  GL_LINEAR
  /* ----------------------------------------------------------------------- */
  OF_MAX,                              // [12] GL_NEAREST/GL_NEAREST/GL_LINEAR
};/* ----------------------------------------------------------------------- */
enum OglBlendEnum : size_t             // Available blend combinations
{ /* ----------------------------------------------------------------------- */
  OB_Z,                                // [00] GL_ZERO
  OB_O,                                // [01] GL_ONE
  OB_S_C,                              // [02] GL_SRC_COLOR
  OB_O_M_S_C,                          // [03] GL_ONE_MINUS_SRC_COLOR
  OB_D_C,                              // [04] GL_DST_COLOR
  OB_O_M_D_C,                          // [05] GL_ONE_MINUS_DST_COLOR
  OB_S_A,                              // [06] GL_SRC_ALPHA
  OB_O_M_S_A,                          // [07] GL_ONE_MINUS_SRC_ALPHA
  OB_D_A,                              // [08] GL_DST_ALPHA
  OB_O_M_D_A,                          // [09] GL_ONE_MINUS_DST_ALPHA
  OB_C_C,                              // [10] GL_CONSTANT_COLOR
  OB_O_M_C_C,                          // [11] GL_ONE_MINUS_CONSTANT_COLOR
  OB_C_A,                              // [12] GL_CONSTANT_ALPHA
  OB_O_M_C_A,                          // [13] GL_ONE_MINUS_CONSTANT_ALPHA
  OB_S_A_S,                            // [14] GL_SRC_ALPHA_SATURATE
  /* ----------------------------------------------------------------------- */
  OB_MAX                               // [15] Blend functions supported
};/* ----------------------------------------------------------------------- */
enum OglUndefinedEnums : GLenum        // Some undefined OpenGL consts
{ /* ----------------------------------------------------------------------- */
  GL_RGBA_DXT1               = 0x83F1, // GL_COMPRESSED_RGBA_S3TC_DXT1_EXT
  GL_RGBA_DXT3               = 0x83F2, // GL_COMPRESSED_RGBA_S3TC_DXT3_EXT
  GL_RGBA_DXT5               = 0x83F3, // GL_COMPRESSED_RGBA_S3TC_DXT5_EXT
};/* ----------------------------------------------------------------------- */
using GLUIntVector  = StdVector<GLuint>;  // Vector of GLuints
using GLFloatVector = StdVector<GLfloat>; // Vector of GLfloats
/* ------------------------------------------------------------------------- */
enum VSyncMode : int {                 // VSync settings
  VSYNC_MIN      = -1,                 // [-1] Minimum Vertical Sync value
  VSYNC_ON_ADAPT = VSYNC_MIN,          // [-1] Adaptive Vertical Sync enabled
  VSYNC_OFF,                           // [ 0] Vertical Sync disable
  VSYNC_ON,                            // [ 1] Vertical Sync enabled
  VSYNC_ON_HALFRATE,                   // [ 2] Verfical sync enabled (half)
  VSYNC_MAX                            // [ 3] Maximum Vertical Sync value
};/* ----------------------------------------------------------------------- */
/* -- OpenGL manager class ------------------------------------------------- */
class Ogl;                             // Class prototype
static Ogl *cOgl = nullptr;            // Pointer to global class
class Ogl :                            // OGL class for OpenGL use simplicity
  /* -- Base classes ------------------------------------------------------- */
  private InitHelper,                  // Initialisation helper
  public Colour,                       // OGL global clear colour cache
  public FboBlend,                     // OGL global blend cache
  public OglFlags                      // OGL init flags
{ /* -- Defines ------------------------------------------------------------ */
#define IGLL(F,M,...) GLEX(CheckLogError, F, M, ## __VA_ARGS__)
#define IGL(F,M,...)  GLEX(CheckExceptError, F, M, ## __VA_ARGS__)
#define IGLC(M,...)   GLEX(CheckExceptError, , M, ## __VA_ARGS__)
  /* -- Private typedefs --------------------------------------------------- */
  using OglVHandles = StdArray<GLuint, 2>; // Vertex Array/Buffer handles type
  /* -- String defines ----------------------------------------------------- */
  using GLenumMap = LookupMap<GLenum>; // A map of ALenum integers
  const GLenumMap  glemExtensions,     // OpenGL extension names (log detail)
                   glemHintTargets,    // Hint target names (log detail)
                   glemHintModes,      // Hint mode values (log detail)
                   glemFormatModes,    // Pixel format modes (log detail)
                   glemOGLCodes;       // OpenGL codes
  /* -- Engine blending id to OpenGL blending id list ---------------------- */
  using BlendFunctions = StdArray<const GLenum, OB_MAX> ;
  const BlendFunctions aBlends;        // Convert engine to opengl blend type
  /* -- Texture filter lists ----------------------------------------------- */
  using TwoGLints = StdArray<const GLint, 2>;
  using TexFilterNMList = StdArray<const TwoGLints, OF_NM_MAX>;
  const TexFilterNMList tfnmlMM;       // Texture filter (no-mipmap) list
  using TexFilterList = StdArray<const TwoGLints, OF_MAX>;
  const TexFilterList tflMM;           // Texture filter list
  /* -- Variables ---------------------------------------------------------- */
  OglVHandles      ovhVao, ovhVbo;     // Vertex Array/Buffer Object handles
  GLuint           gluActiveFbo,       // Currently selected Fbo name cache
                   gluActiveProgram,   // Currently active shader program
                   gluActiveTexture,   // Currently bound texture
                   gluActiveTUnit,     // Currently active texture unit
                   gluActiveVao,       // Currently active vertex array object
                   gluActiveVbo,       // Currently active vertex buffer object
                   gluTexSize,         // Maximum reported texture size
                   gluPackAlign,       // Default pack alignment
                   gluUnpackAlign,     // Default Unpack alignment
                   gluMaxVertexAttr,   // Maximum vertex attributes per shader
                   gluTexUnits,        // Texture units count
                  &gluVaoGlobal,       // ovhVao[0]: Vao for global use
                  &gluVboGlobal,       // ovhVbo[0]: Fbo for global use
                  &gluVaoMain,         // ovhVao[1]: Vao to draw back buffer
                  &gluVboMain;         // ovhVbo[1]: Fbo to draw back buffer
  GLenum           glePolyMode;        // Current polygon mode
  GLint            gliUnpackRowLength; // Default unpack row length
  GLuint64         glullMinVRAM,       // Minimum VRAM required
                   glullTotalVRAM,     // Maximum VRAM supported
                   glullFreeVRAM;      // Current VRAM available
  ClkDuration      cdLimit;            // Frame limit based on refresh rate
  StdStringView    strvRenderer,       // GL renderer string
                   strvVersion,        // GL version string
                   strvVendor;         // GL vendor string
  VSyncMode        vsmSetting;         // VSync setting
  /* -- Delayed destruction ------------------------------------------------ */
  /* Because LUA garbage collection could zap a texture or Fbo class at any  */
  /* time, we need to delay deletion of textures and Fbo handles in OpenGL   */
  /* so that a framebuffer can select/draw without binding non-existant      */
  /* handles. Contents will be destroyed after all drawing is completed!     */
  GLUIntVector     gluivTextures,      // Textures to destroy
                   gluivFbos;          // Fbos to destroy
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
    PFNGLFINISHPROC                    glFinish;
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
  template<typename StrType, typename ...VarArgs>
    void CheckLogError(StrType &&strFormat, VarArgs &&...vaArgs) const
  { // While there are OpenGL errors
    for(GLenum gleCode = sAPI.glGetError();
               gleCode != GL_NO_ERROR;
               gleCode = sAPI.glGetError())
    cLog->LogWarningExSafe("GL call failed: $ ($/$$).",
      StrFormat(StdForward<StrType>(strFormat),
                StdForward<VarArgs>(vaArgs)...),
      GetGLErr(gleCode), StdIOSHex, gleCode);
  }
  /* -- GL error handler --------------------------------------------------- */
  template<typename ...VarArgs>
    void CheckExceptError(const char*const cpFormat, VarArgs &&...vaArgs) const
  { // If there is no error then return
    const GLenum gleCode = sAPI.glGetError();
    if(gleCode == GL_NO_ERROR) return;
    // Raise exception with error details
    XC(cpFormat, "Code", gleCode, "Reason",
      GetGLErr(gleCode), StdForward<VarArgs>(vaArgs)...);
  }
  /* -- Flag setter ----------------------------------------------- */ private:
  void SetFlagExt(const char*const cpName, const OglFlagsConst &ofcFlags)
    { FlagSetOrClear(ofcFlags, HaveExtension(cpName)); }
  /* -- Load GL capabilities ----------------------------------------------- */
  void DetectCapabilities()
  { // Current current OpenGL strings
    strvRenderer = GetString<char>(GL_RENDERER);
    strvVersion = GetString<char>(GL_VERSION);
    strvVendor = GetString<char>(GL_VENDOR);
    // Get vendor specific memory info extensions
    SetFlagExt("GL_NVX_gpu_memory_info", GFL_HAVENVMEM);
    SetFlagExt("GL_ATI_meminfo", GFL_HAVEATIMEM);
    // Set flag if have either
    FlagSet(FlagIsAnyOfSet(GFL_HAVENVMEM|GFL_HAVEATIMEM) ?
      GFL_HAVEMEM : GFL_SHARERAM|GFL_HAVEMEM);
    // Get if adaptive sync is available
    SetFlagExt("WGL_EXT_swap_control_tear", GFL_HAVEWASYNC);
    SetFlagExt("GLX_EXT_swap_control_tear", GFL_HAVELASYNC);
    // Set flag if have either
    FlagSetOrClear(GFL_HAVEASYNC,
      FlagIsAnyOfSet(GFL_HAVEWASYNC|GFL_HAVELASYNC));
    // Cache maximum texture size (Minimum hardware support for 3.2 is 1024^2)
    gluTexSize = GetInteger<GLuint>(GL_MAX_TEXTURE_SIZE);
    gluMaxVertexAttr = GetInteger<GLuint>(GL_MAX_VERTEX_ATTRIBS);
    // Cache texture unit count
    gluTexUnits = GetInteger<GLuint>(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS);
    // Cache current default pack alignment size
    gluPackAlign = GetInteger<GLuint>(GL_PACK_ALIGNMENT);
    gluUnpackAlign = GetInteger<GLuint>(GL_UNPACK_ALIGNMENT);
    gliUnpackRowLength = GetInteger<GLint>(GL_UNPACK_ROW_LENGTH);
    // Load current clear colour
    GetFloatArray<4>(GL_COLOR_CLEAR_VALUE, ColourGetMemory());
    // Load current blending settings
    FboBlendSetSrcRGB(GetInteger<GLenum>(GL_BLEND_SRC_RGB));
    FboBlendSetDstRGB(GetInteger<GLenum>(GL_BLEND_DST_RGB));
    FboBlendSetSrcAlpha(GetInteger<GLenum>(GL_BLEND_SRC_ALPHA));
    FboBlendSetDstAlpha(GetInteger<GLenum>(GL_BLEND_DST_ALPHA));
  }
  /* -- Zero index to hint helper for cvars -------------------------------- */
  static GLenum SHIndexToEnum(const size_t stIndex)
  { // Parameters available to translate
    using Values = StdArray<const GLenum, 4>;
    static const Values vaCmds{ GL_TRUE, GL_DONT_CARE, GL_FASTEST, GL_NICEST };
    // Return position or invalid
    return stIndex < vaCmds.size() ? vaCmds[stIndex] : GL_NONE;
  }
  /* -- Custom loader to avoid function typecast warnings ------------------ */
  template<typename FuncType>
    static void LoadFunc(FuncType &ftPtr, const char*const cpName)
  { // Get the function and copy it over to prevent warning and return it
    if(StdCopyFunction(&ftPtr, GlFWGetProcAddress(cpName)) != nullptr) return;
    // No function so return error
    XC("The specified OpenGL ICD function could not be found from your video "
      "card manufacturers display driver. Try upgrading your graphics "
      "drivers to the latest version from your manufacturers or OEM "
      "website and make sure your operating system is up to date. If all "
      "else fails, you will need to upgrade your graphics hardware!",
      "Function", cpName);
  }
  /* -- Load GL extensions ------------------------------------------------- */
  void LoadFunctions()
  { // Helper macro
#define GETPTR(v,t) LoadFunc(sAPI.v, #v)
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
    GETPTR(glFinish, PFNGLFINISHPROC);
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
    // Vertex Buffer Object (Vbo) functions
    GETPTR(glBindBuffer, PFNGLBINDBUFFERPROC);
    GETPTR(glBufferData, PFNGLBUFFERDATAPROC);
    GETPTR(glDeleteBuffers, PFNGLDELETEBUFFERSPROC);
    GETPTR(glGenBuffers, PFNGLGENBUFFERSPROC);
    // Vertex Memory Object (Vao) functions
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
  }
  /* == OpenGL features ============================================ */ public:
  static bool HaveExtension(const char*const cpName)
    { return !!glfwExtensionSupported(cpName); }
  /* ----------------------------------------------------------------------- */
  template<typename IntType = decltype(gluTexSize)>
    requires StdIsIntegral<IntType>
  IntType MaxTexSize() const { return static_cast<IntType>(gluTexSize); }
  /* ----------------------------------------------------------------------- */
  GLuint MaxVertexAttribs() const { return gluMaxVertexAttr; }
  /* ----------------------------------------------------------------------- */
  GLuint PackAlign() const { return gluPackAlign; }
  /* ----------------------------------------------------------------------- */
  GLuint UnpackAlign() const { return gluUnpackAlign; }
  /* ----------------------------------------------------------------------- */
  GLint UnpackRowLength() const { return gliUnpackRowLength; }
  /* ----------------------------------------------------------------------- */
  void VertexAttribPointer(const GLuint gluAttrib, const GLint gliSize,
    const GLsizei glsiStride, const GLvoid*const glvpBuffer) const
  { sAPI.glVertexAttribPointer(gluAttrib, gliSize, GL_FLOAT, GL_FALSE,
      glsiStride, glvpBuffer); }
  /* ----------------------------------------------------------------------- */
  void SetPixelStore(const GLenum gleId, const GLint gliValue) const
    { sAPI.glPixelStorei(gleId, gliValue); }
  /* ----------------------------------------------------------------------- */
  void SetPackAlignment(const GLint gliValue) const
    { SetPixelStore(GL_PACK_ALIGNMENT, gliValue); }
  /* ----------------------------------------------------------------------- */
  void SetUnpackAlignment(const GLint gliValue) const
    { SetPixelStore(GL_UNPACK_ALIGNMENT, gliValue); }
  /* ----------------------------------------------------------------------- */
  void SetUnpackRowLength(const GLint gliValue) const
    { SetPixelStore(GL_UNPACK_ROW_LENGTH, gliValue); }
  /* ----------------------------------------------------------------------- */
  void ReadBuffer(const GLenum gleBuffer) const
    { sAPI.glReadBuffer(gleBuffer); }
  /* ----------------------------------------------------------------------- */
  template<typename IntType = GLenum>
    requires StdIsIntegral<IntType>
  IntType TexTypeToNative(const TextureType ttType) const
  { // Setup translation list. Make sure it's the same order as shown in
    // ITexDef::P::TextureType in 'texdef.hpp'.
    using TexTypeToGLenum = StdArray<const GLenum, TT_MAX>;
    static const TexTypeToGLenum tttglLookup{
      GL_NONE,      /* TT_NONE */ GL_BGR,       /* TT_BGR */
      GL_BGRA,      /* TT_BGRA */ GL_RGBA_DXT1, /* TT_DXT1 */
      GL_RGBA_DXT3, /* TT_DXT3 */ GL_RGBA_DXT5, /* TT_DXT5 */
      GL_RED,       /* TT_GRAY */ GL_RG,        /* TT_GRAYALPHA */
      GL_RGB,       /* TT_RGB  */ GL_RGBA,      /* TT_RGBA */
    }; // Return casted value
    return static_cast<IntType>(tttglLookup[ttType]);
  }
  /* -- Texture functions -------------------------------------------------- */
  void ReadTexture(const GLenum gleFormat, GLvoid*const glvpBuffer) const
    { sAPI.glGetTexImage(GL_TEXTURE_2D, 0, gleFormat, GL_UNSIGNED_BYTE,
        glvpBuffer); }
  /* ----------------------------------------------------------------------- */
  void ReadTextureTT(const TextureType ttFormat, GLvoid*const glvpBuffer) const
    { ReadTexture(TexTypeToNative<GLenum>(ttFormat), glvpBuffer); }
  /* ----------------------------------------------------------------------- */
  void UploadTexture(const GLint gliLevel, const GLsizei glsiWidth,
    const GLsizei glsiHeight, const GLint gliFormat, const GLenum gleType,
    const GLvoid*const glvpBuffer) const
  { sAPI.glTexImage2D(GL_TEXTURE_2D, gliLevel, gliFormat, glsiWidth,
      glsiHeight, 0, gleType, GL_UNSIGNED_BYTE, glvpBuffer); }
  /* ----------------------------------------------------------------------- */
  void UploadTextureTT(const GLint gliLevel, const GLsizei glsiWidth,
    const GLsizei glsiHeight, const TextureType ttFormat,
    const TextureType ttType, const GLvoid*const glvpBuffer) const
  { UploadTexture(gliLevel, glsiWidth, glsiHeight,
      TexTypeToNative<GLint>(ttFormat), TexTypeToNative<GLenum>(ttType),
      glvpBuffer); }
  /* ----------------------------------------------------------------------- */
  void UploadCompressedTexture(const GLint gliLevel, const GLenum gleFormat,
    const GLsizei glsiWidth, const GLsizei glsiHeight, const GLsizei glsiSize,
    const GLvoid*const glvpBuffer) const
  { sAPI.glCompressedTexImage2D(GL_TEXTURE_2D, gliLevel, gleFormat, glsiWidth,
      glsiHeight, 0, glsiSize, glvpBuffer); }
  /* ----------------------------------------------------------------------- */
  void UploadCompressedTextureTT(const GLint gliLevel,
    const TextureType ttFormat, const GLsizei glsiWidth,
    const GLsizei glsiHeight, const GLsizei glsiSize,
    const GLvoid*const glvpBuffer) const
  { UploadCompressedTexture(gliLevel, TexTypeToNative<GLenum>(ttFormat),
      glsiWidth, glsiHeight, glsiSize, glvpBuffer); }
  /* ----------------------------------------------------------------------- */
  void UploadTextureSub(const GLint gliLeft, const GLint gliTop,
    const GLsizei glsiWidth, const GLsizei glsiHeight,
    const GLenum glePixFormat, const GLvoid*const glvpBuffer) const
  { sAPI.glTexSubImage2D(GL_TEXTURE_2D, 0, gliLeft, gliTop, glsiWidth,
      glsiHeight, glePixFormat, GL_UNSIGNED_BYTE, glvpBuffer); }
  /* ----------------------------------------------------------------------- */
  void UploadTextureSubTT(const GLint gliLeft, const GLint gliTop,
    const GLsizei glsiWidth, const GLsizei glsiHeight,
    const TextureType ttPixFormat, const GLvoid*const glvpBuffer) const
  { UploadTextureSub(gliLeft, gliTop, glsiWidth, glsiHeight,
      TexTypeToNative<GLenum>(ttPixFormat), glvpBuffer); }
  /* ----------------------------------------------------------------------- */
  void UploadTextureSub(const GLsizei glsiWidth, const GLsizei glsiHeight,
    const GLenum glePixFormat, const GLvoid*const glvpBuffer) const
  { UploadTextureSub(0, 0, glsiWidth, glsiHeight, glePixFormat, glvpBuffer); }
  /* ----------------------------------------------------------------------- */
  void GenerateMipmaps() const { sAPI.glGenerateMipmap(GL_TEXTURE_2D); }
  /* ----------------------------------------------------------------------- */
  void CreateTextures(const GLsizei glsiCount, GLuint*const glupTextures) const
  { sAPI.glGenTextures(glsiCount, glupTextures); }
  /* ----------------------------------------------------------------------- */
  template<class ListType>
    requires StdHasDataSize<ListType>
  void CreateTextures(ListType &ltTexList) const
    { CreateTextures(static_cast<GLsizei>(ltTexList.size()),
        ltTexList.data()); }
  /* ----------------------------------------------------------------------- */
  void CreateTexture(GLuint*const glupTextures) const
    { CreateTextures(1, glupTextures); }
  /* ----------------------------------------------------------------------- */
  GLuint GetCurrentTexture() const
    { return GetInteger<GLuint>(GL_TEXTURE_BINDING_2D); }
  /* ----------------------------------------------------------------------- */
  void ActiveTexture(const GLuint gluTexUnit=0)
  { // If we're already the active texture then don't set it
    if(gluActiveTUnit == gluTexUnit) return;
    // Texture unit id lookup table
    using TexUnits = StdArray<const GLenum, 3>;
    static const TexUnits tuTexUnit{ GL_TEXTURE0, GL_TEXTURE1, GL_TEXTURE2 };
    // Activate texture
    sAPI.glActiveTexture(tuTexUnit[gluTexUnit]);
    // Update the selected texture unit
    gluActiveTUnit = gluTexUnit;
  }
  /* ----------------------------------------------------------------------- */
  void DeleteTexture(const GLsizei glsiCount,
    const GLuint*const glupTextures)
  { // Check each texture that is about to be deleted
    for(GLsizei glsiIndex = 0; glsiIndex < glsiCount; ++glsiIndex)
    { // Keep scanning until we find a bound texture
      if(gluActiveTexture != glupTextures[glsiIndex]) continue;
      // Reset currently bound texture
      gluActiveTexture = gluMax;
      // No need to check any others
      break;
    } // Delete the textures
    sAPI.glDeleteTextures(glsiCount, glupTextures);
  }
  /* ----------------------------------------------------------------------- */
  void BindTexture(const GLuint gluTexture=0)
  { // Ignore if already bound
    if(gluTexture == gluActiveTexture) return;
    // Bind the texture
    sAPI.glBindTexture(GL_TEXTURE_2D, gluTexture);
    // Set active texture
    gluActiveTexture = gluTexture;
  }
  /* -- Create multiple framebuffer objects -------------------------------- */
  void CreateFbos(const GLsizei glsiCount, GLuint*const glupFbos) const
    { sAPI.glGenFramebuffers(glsiCount, glupFbos); }
  /* -- Create one framebuffer object -------------------------------------- */
  void CreateFbo(GLuint*const glupFbos) const { CreateFbos(1, glupFbos); }
  /* ----------------------------------------------------------------------- */
  void BindFbo(const GLuint gluFbo=0)
  { // If we're already the active Fbo, ignore
    if(gluActiveFbo == gluFbo) return;
    // Bind the Fbo
    sAPI.glBindFramebuffer(GL_FRAMEBUFFER, gluFbo);
    // Cache the active Fbo
    gluActiveFbo = gluFbo;
  }
  /* ----------------------------------------------------------------------- */
  void DeleteFbo(const GLsizei glsiCount, const GLuint*const glupFbos)
  { // Check each Fbo that is about to be deleted
    for(GLsizei glsiIndex = 0; glsiIndex < glsiCount; ++glsiIndex)
    { // Keep scanning until we find a cached Fbo
      if(glupFbos[glsiIndex] != gluActiveFbo) continue;
      // Reset currently selected Fbo
      gluActiveFbo = gluMax;
      // No need to check any others
      break;
    } // Delete the Fbo
    sAPI.glDeleteFramebuffers(glsiCount, glupFbos);
  }
  /* ----------------------------------------------------------------------- */
  void AttachTexFbo(const GLuint gluTexture) const
    { sAPI.glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_2D, gluTexture, 0); }
  /* ----------------------------------------------------------------------- */
  GLuint VerifyFbo() const
    { return sAPI.glCheckFramebufferStatus(GL_FRAMEBUFFER); }
  /* -- GLSL functions ----------------------------------------------------- */
  GLuint CreateShader(const GLenum gleType) const
    { return sAPI.glCreateShader(gleType); }
  /* ----------------------------------------------------------------------- */
  void DeleteShader(const GLuint gluShader) const
    { sAPI.glDeleteShader(gluShader); }
  /* ----------------------------------------------------------------------- */
  void ShaderSource(const GLuint gluShader, const GLchar*const glcpCode) const
    { sAPI.glShaderSource(gluShader, 1, &glcpCode, nullptr); }
  /* ----------------------------------------------------------------------- */
  void CompileShader(const GLuint gluShader) const
    { sAPI.glCompileShader(gluShader); }
  /* ----------------------------------------------------------------------- */
  void AttachShader(const GLuint gluProgram, const GLuint gluShader) const
    { sAPI.glAttachShader(gluProgram, gluShader); }
  /* ----------------------------------------------------------------------- */
  void DetachShader(const GLuint gluProgram, const GLuint gluShader) const
    { sAPI.glDetachShader(gluProgram, gluShader); }
  /* -- Get compilation status --------------------------------------------- */
  GLenum GetCompileStatus(const GLuint gluProgram) const
    { return GetShaderInt<GLenum>(gluProgram, GL_COMPILE_STATUS); }
  /* ----------------------------------------------------------------------- */
  GLsizei GetShaderInfoLog(const GLuint gluShader, const GLsizei glsiMaxLength,
    GLchar*const glcpDest) const
  { // Get shader log entry and return length
    GLsizei glsiLength;
    sAPI.glGetShaderInfoLog(gluShader, glsiMaxLength, &glsiLength, glcpDest);
    return glsiLength;
  }
  /* -- Get compilation failure reason ------------------------------------- */
  StdString GetCompileFailureReason(const GLuint gluProgram) const
  { // Make string to store the message and return generic string if empty
    StdString strErrMsg;
    strErrMsg.resize(GetShaderInt<size_t>(gluProgram, GL_INFO_LOG_LENGTH));
    if(strErrMsg.empty()) return "No reason";
    // Get the error message
    strErrMsg.resize(static_cast<size_t>(
      GetShaderInfoLog(gluProgram, static_cast<GLsizei>(strErrMsg.size()),
        StdToNonConstCast<GLchar*>(strErrMsg.data()))));
    // Get error and if an error occured put it as a failure reason
    const GLenum gleCode = GetError();
    if(gleCode != GL_NO_ERROR || strErrMsg.empty())
      return StrFormat("Problem getting reason (0x$$)", StdIOSHex, gleCode);
    // Return the string while chopping off return characters
    return StrChop(strErrMsg);
  }
  /* -- Get linker status -------------------------------------------------- */
  GLenum GetLinkStatus(const GLuint gluProgram) const
    { return GetProgramInt<GLenum>(gluProgram, GL_LINK_STATUS); }
  /* ----------------------------------------------------------------------- */
  GLsizei GetProgramInfoLog(const GLuint gluProgram,
    const GLsizei glsiMaxLength, GLchar*const glcpDest) const
  { // Get shader program log entry and return length
    GLsizei glsiLength;
    sAPI.glGetProgramInfoLog(gluProgram, glsiMaxLength, &glsiLength, glcpDest);
    return glsiLength;
  }
  /* -- Get linker failure reason ------------------------------------------ */
  StdString GetLinkFailureReason(const GLuint gluProgram) const
  { // Make string to store the message and return generic string if empty
    StdString strErrMsg;
    strErrMsg.resize(GetProgramInt<size_t>(gluProgram, GL_INFO_LOG_LENGTH));
    if(strErrMsg.empty()) return "No reason";
    // Get the error message and resize string as a result
    strErrMsg.resize(static_cast<size_t>(GetProgramInfoLog(gluProgram,
      static_cast<GLsizei>(strErrMsg.size()),
      StdToNonConstCast<GLchar*>(strErrMsg.data()))));
    // Get error and if an error occured put it as a failure reason
    const GLenum gleCode = GetError();
    if(gleCode != GL_NO_ERROR || strErrMsg.empty())
      return StrFormat("Problem getting reason ($$)", StdIOSHex, gleCode);
    // Return the string while chopping off return characters
    return StrChop(strErrMsg);
  }
  /* ----------------------------------------------------------------------- */
  template<typename IntType>
    requires StdIsIntegral<IntType>
  IntType GetShaderInt(const GLuint gluShader, const GLenum gleCmd) const
  { // Create storage for result, do the checked query and return it
    IntType itResult;
    IGL(sAPI.glGetShaderiv(gluShader, gleCmd,
      reinterpret_cast<GLint*>(&itResult)),
      "Get shader integer failed!",
      "Shader", gluShader, "Enum", gleCmd);
    return itResult;
  }
  /* ----------------------------------------------------------------------- */
  GLuint CreateProgram() const { return sAPI.glCreateProgram(); }
  /* ----------------------------------------------------------------------- */
  template<typename IntType>
    requires StdIsIntegral<IntType>
  IntType GetProgramInt(const GLuint gluProgram, const GLenum gleCmd) const
  { // Do the checked query and return the result
    IntType itResult;
    IGL(sAPI.glGetProgramiv(gluProgram, gleCmd,
          reinterpret_cast<GLint*>(&itResult)),
      "Get program integer failed!",
      "Program", gluProgram, "Enum", gleCmd);
    // return the result
    return itResult;
  }
  /* ----------------------------------------------------------------------- */
  void DeleteProgram(const GLuint gluProgram)
  { // Delete the program and reset cache if it was the active program
    sAPI.glDeleteProgram(gluProgram);
    if(gluActiveProgram == gluProgram) gluActiveProgram = gluMax;
  }
  /* ----------------------------------------------------------------------- */
  void LinkProgram(const GLuint gluProgram) const
    { sAPI.glLinkProgram(gluProgram); }
  /* ----------------------------------------------------------------------- */
  GLuint GetProgram() const { return GetInteger<GLuint>(GL_CURRENT_PROGRAM); }
  /* ----------------------------------------------------------------------- */
  void UseProgram(const GLuint gluProgram=0)
  { // Ignore if we already have the program selected
    if(gluProgram == gluActiveProgram) return;
    // Activate the shader program
    sAPI.glUseProgram(gluProgram);
    // Update the active program
    gluActiveProgram = gluProgram;
  }
  /* ----------------------------------------------------------------------- */
  GLint GetUniformLocation(const GLuint gluProgram,
    const GLchar*const cpValue) const
  { return sAPI.glGetUniformLocation(gluProgram, cpValue); }
  /* ----------------------------------------------------------------------- */
  void BindAttribLocation(const GLuint gluProgram, const GLuint gluAttrib,
    const GLchar*const cpValue) const
  { sAPI.glBindAttribLocation(gluProgram, gluAttrib, cpValue); }
  /* -- Assign a new vec4 array -------------------------------------------- */
  void Uniform(const GLint gliUniform, const GLsizei gliCount,
    const GLfloat*const glfpValues) const
  { sAPI.glUniform4fv(gliUniform, gliCount, glfpValues); }
  /* -- Assign a integer value to a uniform -------------------------------- */
  void Uniform(const GLint gliUniform, const GLint gliValue) const
    { sAPI.glUniform1i(gliUniform, gliValue); }
  /* -- Assign four floats to a uniform ------------------------------------ */
  void Uniform(const GLint gliUniform, const GLfloat glfV1,
    const GLfloat glfV2, const GLfloat glfV3, const GLfloat glfV4) const
  { sAPI.glUniform4f(gliUniform, glfV1, glfV2, glfV3, glfV4); }
  /* -- Create multiple vertex buffer objects ------------------------------ */
  void GenVertexBuffers(const GLsizei glsiCount, GLuint*const gluipVbo) const
    { sAPI.glGenBuffers(glsiCount, gluipVbo); }
  /* -- Bind vertex buffer object ------------------------------------------ */
  void BindVertexBuffer(const GLenum gleTarget, const GLuint gluVbo)
  { // Ignore if this vbo is already selected
    if(gluVbo == gluActiveVbo) return;
    // Bind the vbo
    sAPI.glBindBuffer(gleTarget, gluVbo);
    // Is active vbo
    gluActiveVbo = gluVbo;
  }
  /* -- Bind vertex buffer object ------------------------------------------ */
  void BindStaticVertexBuffer(const GLuint gluVbo)
    { BindVertexBuffer(GL_ARRAY_BUFFER, gluVbo); }
  /* -- Delete multiple vertex buffer objects ------------------------------ */
  void DeleteVertexBuffers(const GLsizei glsiCount,
    const GLuint*const gluipVbo)
  { // Check each vbo deleted
    for(GLsizei glsiIndex = 0; glsiIndex < glsiCount; ++glsiIndex)
    { // Keep scanning until we find a bound vbo
      if(gluipVbo[glsiIndex] != gluActiveVbo) continue;
      // Reset currently bound vbo
      gluActiveVbo = gluMax;
      // No need to check any others
      break;
    } // Delete the vbo's
    sAPI.glDeleteBuffers(glsiCount, gluipVbo);
  }
  /* -- Generate vertex array objects -------------------------------------- */
  void GenVertexArrays(const GLsizei glsiCount, GLuint*const gluipVao) const
    { sAPI.glGenVertexArrays(glsiCount, gluipVao); }
  /* -- Bind vertex array object ------------------------------------------- */
  void BindVertexArray(const GLuint gluVao)
  { // Ignore if this vao is already selected
    if(gluVao == gluActiveVao) return;
    // Bind the vertex array
    sAPI.glBindVertexArray(gluVao);
    // Is active vao
    gluActiveVao = gluVao;
  }
  /* -- Delete multiple vertex array objects ------------------------------- */
  void DeleteVertexArrays(const GLsizei glsiCount, const GLuint*const gluipVao)
  { // Check each vao deleted
    for(GLsizei glsiIndex = 0; glsiIndex < glsiCount; ++glsiIndex)
    { // Keep scanning until we find a bound vao
      if(gluipVao[glsiIndex] != gluActiveVao) continue;
      // Reset currently bound vao
      gluActiveVao = gluMax;
      // No need to check any others
      break;
    } // Delete the vao's
    sAPI.glDeleteVertexArrays(glsiCount, gluipVao);
  }
  /* -- Data buffering functions ------------------------------------------- */
  void BufferData(const GLenum gleTarget, const GLsizei glsiSize,
    const GLvoid*const glvpBuffer, const GLenum gleUsage) const
  { sAPI.glBufferData(gleTarget, glsiSize, glvpBuffer, gleUsage); }
  /* ----------------------------------------------------------------------- */
  void BufferStaticData(const GLsizei glsiSize, const GLvoid*const glvpBuffer)
    { BufferData(GL_ARRAY_BUFFER, glsiSize, glvpBuffer, GL_STREAM_DRAW); }
  /* -- Clear bound Fbo or back buffer ------------------------------------- */
  void ClearBuffer() const { sAPI.glClear(GL_COLOR_BUFFER_BIT); }
  /* -- Set specified Vbo and Vao ------------------------------------------ */
  void BindVaoandVbo(const GLuint gluNVao, const GLuint gluNVbo)
    { BindVertexArray(gluNVao); BindStaticVertexBuffer(gluNVbo); }
  /* -- Set main Vbo/Vao data for main Fbo --------------------------------- */
  void BindMainVaoandVbo(void) { BindVaoandVbo(gluVaoMain, gluVboMain); }
  /* -- Set default Vbo/Vao data for main Fbo ------------------------------ */
  void BindGlobalVaoandVbo(void) { BindVaoandVbo(gluVaoGlobal, gluVboGlobal); }
  /* -- Initialise main Vbo/Vao data for main Fbo -------------------------- */
  void MainFboInitDrawData(const GLsizei glsiSize,
    const GLvoid*const glvpBuffer, const GLvoid*const glvpCoordBuffer,
    const GLvoid*const glvpVertexBuffer, const GLvoid*const glvpColourBuffer)
  { // Bind main Fbo vertex buffer and array
    IGL(BindMainVaoandVbo(), "Failed to bind main frame buffer Vbo and Vao!",
      "Main Vao", gluVaoMain, "Main Vbo", gluVboMain);
    // Buffer the interlaced triangle data
    IGL(BufferStaticData(glsiSize, glvpBuffer),
      "Failed to buffer main Fbo data!",
      "Buffer", glvpBuffer, "Size", glsiSize);
    // Specify format of the interlaced triangle texture coord data
    IGL(VertexAttribPointer(A_COORD, stCompsPerCoord, 0, glvpCoordBuffer),
      "Failed to set main Fbo/Vao coord data!",
      "Buffer", glvpBuffer, "Size", glsiSize,
      "Attrib", A_COORD,    "Data", glvpCoordBuffer);
    IGL(EnableVertexAttribArray(A_COORD),
      "Failed to enable texture co-ordinates array for this Vao!",
      "Attrib", A_COORD);
    // Specify format of the interlaced triangle vertex coord data
    IGL(VertexAttribPointer(A_VERTEX, stCompsPerPos, 0, glvpVertexBuffer),
      "Failed to set main Fbo/Vao vertex data!",
      "Buffer", glvpBuffer, "Size", glsiSize,
      "Attrib", A_VERTEX,   "Data", glvpVertexBuffer);
    // Specify format of the interlaced triable texture intensity data
    IGL(EnableVertexAttribArray(A_VERTEX),
      "Failed to enable vertex array for this Vao!", "Attrib", A_VERTEX);
    IGL(VertexAttribPointer(A_COLOUR, stCompsPerColour, 0, glvpColourBuffer),
      "Failed to set main Fbo/Vao colour data!",
      "Buffer", glvpBuffer, "Size", glsiSize,
      "Attrib", A_COLOUR,   "Data", glvpColourBuffer);
    IGL(EnableVertexAttribArray(A_COLOUR),
      "Failed to enable colour intensity array for this Vao!",
      "Attrib", A_COLOUR);
    // Select default vertex buffer and array
    IGL(BindGlobalVaoandVbo(), "Failed to bind global Vbo and Vao!",
      "Global Vao", gluVaoGlobal, "Global Vbo", gluVboGlobal);
  }
  /* -- Set clear colour --------------------------------------------------- */
  void CommitClearColour() const
    { sAPI.glClearColor(ColourGetRed(),  ColourGetGreen(),
                        ColourGetBlue(), ColourGetAlpha()); }
  /* ----------------------------------------------------------------------- */
  void SetClearColourInt(const unsigned uColour)
    { ColourSetInt(uColour); CommitClearColour(); }
  /* -- Set clear colour (applies to all Fbo's) ---------------------------- */
  void SetClearColourIfChanged(const Colour &coCol)
    { if(ColourSet(coCol)) CommitClearColour(); }
  /* ----------------------------------------------------------------------- */
  void SetAndClear(const Colour &coCol)
    { SetClearColourIfChanged(coCol); ClearBuffer(); }
  /* -- Update polygon rendering mode -------------------------------------- */
  void SetPolygonMode(const GLenum gleMode)
  { // Return if we already set this mode else set it and update cached mode
    if(glePolyMode == gleMode) return;
    sAPI.glPolygonMode(GL_FRONT_AND_BACK, gleMode);
    glePolyMode = gleMode;
  }
  /* ----------------------------------------------------------------------- */
  GLenum GetError() const{ return sAPI.glGetError(); }
  /* -- Restore VSync setting ---------------------------------------------- */
  void RestoreVSync() const { GlFWSetVSync(vsmSetting); }
  /* ----------------------------------------------------------------------- */
  VSyncMode GetVSync() const { return vsmSetting; }
  /* ----------------------------------------------------------------------- */
  void DrawArrays(const GLenum gleMode, const GLint gliFirst,
    const GLsizei glsiCount) const
  { sAPI.glDrawArrays(gleMode, gliFirst, glsiCount); }
  /* ----------------------------------------------------------------------- */
  void DrawArraysTriangles(const GLsizei glsiCount) const
    { DrawArrays(GL_TRIANGLES, 0, glsiCount); }
  /* -- Delete lingering textures and finish ------------------------------- */
  void OglPostRender()
  { // Delete texture and Fbo handles
    DeleteTexturesAndFboHandles();
    // Flush and wait for GPU to complete
    Finish();
    // Update memory available
    UpdateVRAMAvailable();
    // Clear any existing errors
    GetError();
  }
  /* -- Render arrays ------------------------------------------------------ */
  void OglDrawArrays(void)
  { // Set normal fill poly mode
    SetPolygonMode(GL_FILL);
    // Bind main Vao and Vbo containing our pre-defined data (MainFboInit())
    BindMainVaoandVbo();
    // Using MacOS?
#if defined(MACOS)
    // This locking code is required to fix a major crash bug in Ventura
    // 13.3+. See https://github.com/glfw/glfw/issues/1997 for more
    // information.
    using namespace Lib::OS::GlFW::NSGL;
    // Get the current NSGL context and lock it. Note there is nothing to
    // throw in this routine so it is safe to use this as-is.
    CGLContextObj cglcoLock = CGLGetCurrentContext();
    CGLLockContext(cglcoLock);
    // Blit the two triangles
    DrawArraysTriangles(stTwoTriangles);
    // Context is unlocked when exiting this scope
    CGLUnlockContext(cglcoLock);
#else
    // Blit the two triangles
    DrawArraysTriangles(stTwoTriangles);
#endif
    // Reset to global Vao and Vbo
    BindGlobalVaoandVbo();
  }
  /* ----------------------------------------------------------------------- */
  void SetViewport(const GLsizei glsiWidth, const GLsizei glsiHeight) const
    { sAPI.glViewport(0, 0, glsiWidth, glsiHeight); }
  /* -- Get openGL float array --------------------------------------------- */
  template<size_t stCount>
    void GetFloatArray(const GLenum gleId, GLfloat *const glfpDest) const
  { IGL(sAPI.glGetFloatv(gleId, glfpDest),
      "Get float array failed!", "Index", gleId, "Count", stCount); }
  /* -- Get openGL float array --------------------------------------------- */
  template<size_t stCount, class IntType = StdArray<GLfloat, stCount>>
    requires StdHasDataSize<IntType>
  IntType GetFloatArray(const GLenum gleId) const
  { // Create array to return data into, populate and return it
    IntType tData;
    GetFloatArray<stCount>(gleId, tData.data());
    return tData;
  }
  /* -- Get openGL int array ----------------------------------------------- */
  template<size_t stCount, class IntType = StdArray<GLint, stCount>>
    requires StdHasDataSize<IntType>
  IntType GetIntegerArray(const GLenum gleId) const
  { // Create array to return data into, populate and return it
    IntType aData;
    IGL(sAPI.glGetIntegerv(gleId, aData.data()),
      "Get integer array failed!", "Index", gleId, "Count", stCount);
    return aData;
  }
  /* -- Get openGL int ----------------------------------------------------- */
  template<typename IntType = GLint>
    requires StdIsIntegral<IntType>
  IntType GetInteger(const GLenum gleId) const
  { return static_cast<IntType>(
      GetIntegerArray<sizeof(IntType) / sizeof(GLint)>(gleId)[0]); }
  /* -- Get openGL string -------------------------------------------------- */
  template<typename IntType = GLubyte>
    requires StdIsIntegral<IntType>
  const IntType* GetString(const GLenum gleId) const
  { // Get the variable and throw error if occured
    const GLubyte*const ucpStr = sAPI.glGetString(gleId);
    IGLC("Get string failed!", "Index", gleId);
    // Sanity check actual string
    if(!UtfIsCStringValid(ucpStr))
      XC("Invalid string returned!", "Index", gleId, "String", ucpStr);
    // Return result
    return reinterpret_cast<const IntType*>(ucpStr);
  }
  /* -- Do delete all the marked Fbo handles ------------------------------- */
  void DeleteMarkedFboHandles()
  { // Delete the Fbo's
    IGLL(DeleteFbo(static_cast<GLsizei>(gluivFbos.size()), gluivFbos.data()),
      "OGL failed to destroy $ Fbo handles!", gluivFbos.size());
    // Clear the list
    gluivFbos.clear();
  }
  /* -- Do delete all the marked texture handles --------------------------- */
  void DeleteMarkedTextureHandles()
  { // Delete the marked textures
    IGLL(DeleteTexture(static_cast<GLsizei>(gluivTextures.size()),
      gluivTextures.data()), "OGL failed to destroy $ texture handles!",
      gluivTextures.size());
    // Clear the list
    gluivTextures.clear();
  }
  /* -- Enable vertex attribute array -------------------------------------- */
  void EnableVertexAttribArray(const GLuint gluAttrib) const
    { sAPI.glEnableVertexAttribArray(gluAttrib); }
  /* -- Disable vertex attribute array ------------------------------------- */
  void DisableVertexAttribArray(const GLuint gluAttrib) const
    { sAPI.glDisableVertexAttribArray(gluAttrib); }
  /* -- Enable an extension ------------------------------------------------ */
  void EnableExtension(const GLenum gleExtension) const
  { // Ignore if already disabled
    if(sAPI.glIsEnabled(gleExtension)) return;
    // Enable the extension and log result
    IGL(sAPI.glEnable(gleExtension), "Enable extension failed!",
      "Extension",   gleExtension,
      "ExtensionId", glemExtensions.Get(gleExtension));
    cLog->LogDebugExSafe("OGL enabled extension $<0x$$>.",
      glemExtensions.Get(gleExtension), StdIOSHex, gleExtension);
  }
  /* -- Disable an extension ----------------------------------------------- */
  void DisableExtension(const GLenum gleExtension) const
  { // Ignore if already disabled
    if(!sAPI.glIsEnabled(gleExtension)) return;
    // Disable the extension and log result
    IGL(sAPI.glDisable(gleExtension), "Disable extension failed!",
      "Key", gleExtension, "KeyId", glemExtensions.Get(gleExtension));
    cLog->LogDebugExSafe("OGL disabled extension $<0x$$>.",
      glemExtensions.Get(gleExtension), StdIOSHex, gleExtension);
  }
  /* -- Commit blending algorithms ----------------------------------------- */
  void CommitBlend() const
    { sAPI.glBlendFuncSeparate(FboBlendGetSrcRGB(), FboBlendGetDstRGB(),
                               FboBlendGetSrcAlpha(), FboBlendGetDstAlpha()); }
  /* -- Set blending algorithms -------------------------------------------- */
  void SetBlendIfChanged(const FboBlend &fbOther)
    { if(FboBlendSet(fbOther)) CommitBlend(); }
  /* -- Set texture parameter (No error checking needed) ------------------- */
  void SetTexParam(const GLenum gleVar, const GLint gliValue) const
    { sAPI.glTexParameteri(GL_TEXTURE_2D, gleVar, gliValue); }
  /* -- Convert pixel mode to string --------------------------------------- */
  template<typename IntType> // Forcing any type to GLenum
    requires StdIsIntegral<IntType>
  const StdStringView &GetPixelFormat(const IntType glitMode) const
    { return glemFormatModes.Get(static_cast<GLenum>(glitMode)); }
  /* -- Update hint -------------------------------------------------------- */
  void SetHint(const GLenum gleTarget, const GLenum gleMode) const
  { // Get opengl hint and throw if not failed else set hint
    IGL(sAPI.glHint(gleTarget, gleMode), "Failed to set hint!",
      "Target", glemHintTargets.Get(gleTarget), "TargetId", gleTarget,
      "Mode",   glemHintModes.Get(gleMode),     "ModeId",   gleMode);
    cLog->LogDebugExSafe("OGL set hint $<0x$$> to $<0x$>.",
      glemHintTargets.Get(gleTarget), StdIOSHex, gleTarget,
      glemHintModes.Get(gleMode), gleMode);
  }
  /* -- GL is initialised? ------------------------------------------------- */
  bool IsGLInitialised() const { return FlagIsSet(GFL_INITIALISED); }
  bool IsGLNotInitialised() const { return !IsGLInitialised(); }
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
      StdIOSHex, FlagGet(), GetRenderer(), GetVersion(), GetVendor());
    // If debug log level?
    if(cLog->LogHasLevel(LH_DEBUG))
    { // Get number of extensions and return if we're not interested in them?
      const GLuint gluExts = GetExtensionCount();
      // Report device selected and basic capabilities
      cLog->LogNLCDebugExSafe(
        "- Clear colour: $,$,$,$; "      "Blend: $,$,$,$.\n"
        "- Maximum texture size: $$^2; " "Pack alignment: $.\n"
        "- Unpack alignment: $; "        "Unpack row length: $.\n"
        "- Vertex attributes: $; "       "Texture units: $.\n"
        "- Extensions count: $.",
        ColourGetRed(),        ColourGetGreen(),      ColourGetBlue(),
        ColourGetAlpha(),      FboBlendGetSrcRGB(),   FboBlendGetDstRGB(),
        FboBlendGetSrcAlpha(), FboBlendGetDstAlpha(), StdIOSDec,
        MaxTexSize(),          PackAlign(),           UnpackAlign(),
        UnpackRowLength(),     MaxVertexAttribs(),    gluTexUnits,
        gluExts);
      // Build sorted list of extensions and log them all
      StrUIntMap suimExts;
      for(GLuint gluIndex = 0; gluIndex < gluExts; ++gluIndex)
        suimExts.insert({ GetExtension(gluIndex), gluIndex });
      for(const StrUIntMapPair &suimpPair : suimExts)
        cLog->LogNLCDebugExSafe("- Have extension '$' (#$).",
          suimpPair.first, suimpPair.second);
    } // Show warning if not enough texture units
    if(gluTexUnits < 3)
      cLog->LogWarningSafe("Ogl detected only $ of the three texture units "
        "that are required for video playback!");
    // Set the initialised flag
    FlagSet(GFL_INITIALISED);
  }
  /* -- Set texture mode by filter id -------------------------------------- */
  void SetFilterById(const OglFilterEnum ofeId, GLint &gliMin, GLint &gliMag)
    const
  { // Get filter lookup id and set values
    const TwoGLints &tglItem = tfnmlMM[ofeId];
    gliMag = tglItem.front();
    gliMin = tglItem.back();
  }
  /* -- Convert engine blend id to blend OpenGL enum ----------------------- */
  GLenum EngineBlendToOglBlend(const OglBlendEnum obeEnum) const
    { return aBlends[obeEnum]; }
  /* -- Set texture mode by filter id -------------------------------------- */
  void SetMipMapFilterById(const OglFilterEnum ofeId, GLint &gliMin,
    GLint &gliMag) const
  { // Get filter lookup id and set values
    const TwoGLints &tglItem = tflMM[ofeId];
    gliMag = tglItem.front();
    gliMin = tglItem.back();
  }
  /* -- Extensions --------------------------------------------------------- */
  GLuint GetExtensionCount() const
    { return GetInteger<GLuint>(GL_NUM_EXTENSIONS); }
  /* ----------------------------------------------------------------------- */
  const char *GetExtension(const GLuint gluIndex) const
    { return reinterpret_cast<const char*>
        (sAPI.glGetStringi(GL_EXTENSIONS, gluIndex)); }
  /* -- Read OpenGL renderer data ------------------------------------------ */
  const StdStringView &GetVendor() const { return strvVendor; }
  const StdStringView &GetRenderer() const { return strvRenderer; }
  const StdStringView &GetVersion() const { return strvVersion; }
  /* -- Reset all binds ---------------------------------------------------- */
  void OglResetBinds()
  { // Unbind active texture, shader program, texture, fbo and select default
    // vertex buffer and array objects.
    ActiveTexture();
    UseProgram();
    BindTexture();
    BindFbo();
    BindStaticVertexBuffer(gluVboGlobal);
    BindVertexArray(gluVaoGlobal);
  }
  /* -- Mark an array of textures for deletion ----------------------------- */
  template<class ListType>
    requires StdHasDataSize<ListType>
  void SetDeleteTextures(const ListType &ltTexList)
    { gluivTextures.insert(gluivTextures.cend(),
        ltTexList.cbegin(), ltTexList.cend()); }
  /* -- Mark a single texture for deletion --------------------------------- */
  void SetDeleteTexture(const GLuint gluTexture)
    { gluivTextures.emplace_back(gluTexture); }
  /* -- Mark an Fbo for deletion ------------------------------------------- */
  void SetDeleteFbo(const GLuint gluFbo)
    { gluivFbos.emplace_back(gluFbo); }
  /* -- Do delete all the marked textures and Fbo's ------------------------ */
  void DeleteTexturesAndFboHandles()
  { // Have textures to delete?
    if(!gluivTextures.empty()) DeleteMarkedTextureHandles();
    // Have Fbo's to delete?
    if(!gluivFbos.empty()) DeleteMarkedFboHandles();
  }
  /* -- Get available and total VRAM --------------------------------------- */
  GLuint64 GetVRAMFree() const { return glullFreeVRAM; }
  GLuint64 GetVRAMTotal() const { return glullTotalVRAM; }
  GLuint64 GetVRAMUsed() const
    { return GetVRAMTotal() - GetVRAMFree(); }
  double GetVRAMFreePC() const
    { return 100.0 - UtilMakePercentage(GetVRAMFree(), GetVRAMTotal()); }
  /* -- Get free memory on nvidia cards ------------------------------------ */
  void UpdateVRAMAvailableNV()
  { // - https://www.khronos.org/registry/OpenGL/extensions/
    //     NVX/NVX_gpu_memory_info.txt
    glullFreeVRAM = GetInteger<GLuint64>(0x9049) * 1024;
  }
  /* -- Get free memory on ATI cards --------------------------------------- */
  void UpdateVRAMAvailableATI()
  { // - OG macro name: TEXTURE_FREE_MEMORY_ATI
    // - https://www.khronos.org/registry/OpenGL/extensions/
    //     ATI/ATI_meminfo.txt
    glullFreeVRAM = GetInteger<GLuint64>(0x87FC) * 1024;
    // Update total if higher
    if(glullFreeVRAM > glullTotalVRAM) glullTotalVRAM = glullFreeVRAM;
  }
  /* -- Get free memory on shared system memory renderer ------------------- */
  void UpdateVRAMAvailableShared()
  { // Update system memory usage data
    cSystem->UpdateMemoryUsageData();
    // Get system values
    glullFreeVRAM = cSystem->RAMFree();
  }
  /* -- Get memory information --------------------------------------------- */
  void UpdateVRAMAvailable()
  { // Have NVIDIA free memory?
    if(FlagIsSet(GFL_HAVENVMEM)) UpdateVRAMAvailableNV();
    // Have ATI free memory
    else if(FlagIsSet(GFL_HAVEATIMEM)) UpdateVRAMAvailableATI();
    // Have shared memory
    else if(FlagIsSet(GFL_SHARERAM)) UpdateVRAMAvailableShared();
  }
  /* -- Verify RAM constraints --------------------------------------------- */
  void VerifyVRAMConstraints()
  { // Have nVIDIA memory info?
    if(FlagIsSet(GFL_HAVENVMEM))
    { // Get total video memory
      // - OG macro name: GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX
      glullTotalVRAM =
        static_cast<GLuint64>(GetInteger<GLuint>(0x9048)) * 1024;
      // Update available VRAM
      UpdateVRAMAvailableNV();
      // Report VRAM information to log
      cLog->LogDebugSafe("- Using NVIDIA memory functions.");
    } // Have ATI memory info?
    else if(FlagIsSet(GFL_HAVEATIMEM))
    { // No total video memory on ATI cards so let UpdateVRAMAvailable set it.
      glullTotalVRAM = 0;
      // Update available VRAM
      UpdateVRAMAvailableATI();
      // Report VRAM information to log
      cLog->LogDebugSafe("- Using ATI memory functions (no total available).");
    } // Device shares memory with system?
    else if(FlagIsSet(GFL_SHARERAM))
    { // Update total memory information
      glullTotalVRAM = cSystem->RAMTotal();
      // Update memory information
      UpdateVRAMAvailableShared();
      // Report VRAM information to log
      cLog->LogDebugSafe("- Renderer shares memory with system.");
    } // Have no memory information command?
    else
    { // Set maximum total VRAM
      glullTotalVRAM = glullFreeVRAM = StdLimits<GLuint64>::max();
      // Report VRAM information to log
      cLog->LogWarningSafe("- Video memory data functions not available.");
      // No need to check anything
      return;
    } // Load free VRAM and if we don't have enough free VRAM? Throw error
    if(glullMinVRAM && glullFreeVRAM < glullMinVRAM)
      XC("There is not enough video memory available. Close any "
        "running applications consuming it and try running again!",
        "Available", glullFreeVRAM, "Total",  glullTotalVRAM,
        "Percent",   UtilMakePercentage(glullFreeVRAM, glullTotalVRAM),
        "Required",  glullMinVRAM,  "Needed", glullMinVRAM - glullFreeVRAM);
    // Report VRAM information to log
    cLog->LogInfoExSafe(
      "- Total VRAM: $ bytes ($).\n"
      "- Available VRAM: $ bytes ($).",
      glullTotalVRAM, StrToBytes(glullTotalVRAM), glullFreeVRAM,
      StrToBytes(glullFreeVRAM));
  }
  /* --------------------------------------------------------------- */ public:
  template<typename IntType>
    requires StdIsIntegral<IntType>
  const StdStringView &GetGLErr(const IntType itCode) const
    { return glemOGLCodes.Get(static_cast<GLenum>(itCode)); }
  /* -- Return potential fps limit ----------------------------------------- */
  double GetLimit() const { return ClockDurationToDouble(cdLimit); }
  /* -- Flush pipeline and wait for GPU to finish -------------------------- */
  void Finish(void) const { sAPI.glFinish(); }
  /* -- Update window size limits ------------------------------------------ */
  void UpdateWindowSizeLimits()
  { // Get app specified minimums and maximums
    const unsigned
      uWMin = cCVars->CVarsGetInternal<unsigned>(WIN_WIDTHMIN),
      uWMax = cCVars->CVarsGetInternal<unsigned>(WIN_HEIGHTMIN),
      uHMin = cCVars->CVarsGetInternal<unsigned>(WIN_WIDTHMAX),
      uHMax = cCVars->CVarsGetInternal<unsigned>(WIN_HEIGHTMAX);
    // Set the window size limits. The specified maximum must not exceed the
    // video cards maximum texture size or perhaps BOOM! (not tested though).
    cEvtWin->Add(EWC_WIN_LIMITS, uWMin, uWMax,
      uHMin ? UtilMinimum(uHMin, MaxTexSize()) : MaxTexSize(),
      uHMax ? UtilMinimum(uHMax, MaxTexSize()) : MaxTexSize());
  }
  /* -- Initialise --------------------------------------------------------- */
  void OglInit(const int iRefresh, const bool bForce=false)
  { // Class initialised
    if(!bForce) IHInitialise();
    // Log class initialising
    cLog->LogDebugSafe("OGL subsystem initialising...");
    // Set opengl context to engine thread
    cGlFW->WinSetContext();
    // Load GL functions and throw exception with reason if not all loaded
    LoadFunctions();
    // Clear error that Wine or GLFW might have caused
    if(const GLenum eError = sAPI.glGetError())
      cLog->LogDebugExSafe("Ogl cleared host caused error code! ($/$$).",
        GetGLErr(eError), StdIOSHex, eError);
    // Set frame limit
    cdLimit = duration_cast<ClkDuration>(duration<double>(
      UtilPerSec(static_cast<double>(iRefresh))));
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
    SetQShaderHint(cCVars->CVarsGetInternal<size_t>(VID_QSHADER));
    // Indicates the sampling quality of antialiased lines. If a larger filter
    // function is applied, hinting GL_NICEST can result in more pixel
    // fragments being generated during rasterization.
    SetQLineHint(cCVars->CVarsGetInternal<size_t>(VID_QLINE));
    // Indicates the sampling quality of antialiased polygons. Hinting
    // GL_NICEST can result in more pixel fragments being generated during
    // rasterization, if a larger filter function is applied.
    SetQPolygonHint(cCVars->CVarsGetInternal<size_t>(VID_QPOLYGON));
    // Indicates the quality and performance of the compressing texture images.
    // Hinting GL_FASTEST indicates that texture images should be compressed as
    // quickly as possible, while GL_NICEST indicates that texture images
    // should be compressed with as little image quality loss as possible.
    // GL_NICEST should be selected if the texture is to be retrieved by
    // glGetCompressedTexImage for reuse.
    SetQCompressHint(cCVars->CVarsGetInternal<size_t>(VID_QCOMPRESS));
    // Setup window size limits
    UpdateWindowSizeLimits();
    // Set pack alignment for grabbing screenshots and unpack alignment for
    // uploading odd sized textures.
    IGL(SetPackAlignment(1), "Set byte pack alignment failed!");
    IGL(SetUnpackAlignment(1), "Set byte unpack alignment failed!");
    // Create and bind Vao
    IGL(GenVertexArrays(static_cast<GLsizei>(ovhVao.size()), ovhVao.data()),
      "Generate default Vaos failed!", "Count", ovhVao.size());
    IGL(BindVertexArray(gluVaoGlobal),
      "Bind global Vao failed!", "Index", gluVaoGlobal);
    // Create and bind Vbo
    IGL(GenVertexBuffers(static_cast<GLsizei>(ovhVbo.size()), ovhVbo.data()),
      "Generate default Vbos failed!", "Count", ovhVbo.size());
    IGL(BindStaticVertexBuffer(gluVboGlobal),
      "Bind global Vbo failed!", "Index", gluVboGlobal);
    // Log class initialising
    cLog->LogInfoSafe("OGL subsystem initialised.");
  }
  /* -- DeInitialise ------------------------------------------------------- */
  void OglDeInit(const bool bForce=false)
  { // Ignore if class not initialised
    if(IHNotDeInitialise()) return;
    // OpenGL is now de-initialised
    SetInitialised(false, bForce);
    // Log class initialising
    cLog->LogDebugSafe("OGL subsystem de-initialising...");
    // Reset all binds
    OglResetBinds();
    // Have textures to delete?
    if(!gluivTextures.empty())
    { // Delete the texture handles
      cLog->LogDebugExSafe("OGL deleting $ marked texture handles...",
        gluivTextures.size());
      DeleteMarkedTextureHandles();
      cLog->LogInfoSafe("OGL deleted marked texture handles.");
    } // Have Fbo's to delete?
    if(!gluivFbos.empty())
    { // Delete the Fbo handles
      cLog->LogDebugExSafe("OGL deleting $ marked Fbo handles...",
        gluivFbos.size());
      DeleteMarkedFboHandles();
      cLog->LogInfoSafe("OGL deleted marked Fbo handles.");
    } // Vertex buffer object created?
    if(gluVboGlobal && gluVboMain)
    { // Delete vertex buffer object
      cLog->LogDebugExSafe("OGL deleting $ vertex buffer objects $ and $...",
        ovhVbo.size(), gluVboGlobal, gluVboMain);
      IGLL(DeleteVertexBuffers(
        static_cast<GLsizei>(ovhVbo.size()), ovhVbo.data()),
          "Failed to delete vertex buffer objects!",
          "Global", gluVboGlobal, "Main", gluVboMain);
      StdFill(seq, ovhVbo.begin(), ovhVbo.end(), 0);
      cLog->LogInfoExSafe("OGL deleted $ vertex buffer objects.",
        ovhVbo.size());
    } // Vertex array object created?
    if(gluVaoGlobal && gluVaoMain)
    { // Delete vertex array object
      cLog->LogDebugExSafe("OGL deleting $ vertex array objects $ and $...",
        ovhVao.size(), gluVaoGlobal, gluVaoMain);
      IGLL(DeleteVertexArrays(
        static_cast<GLsizei>(ovhVao.size()), ovhVao.data()),
          "Failed to delete vertex array object!", "Index", gluVaoGlobal);
      StdFill(seq, ovhVao.begin(), ovhVao.end(), 0);
      cLog->LogInfoExSafe("OGL deleted $ vertex array objects.",
        ovhVao.size());
    } // Release opengl context from engine thread
    GlFWSetContext();
    // Init flags
    FlagReset(GFL_NONE);
    // Pack alignment and texture size and caches
    gluActiveFbo = gluActiveProgram = gluActiveTexture = gluActiveTUnit =
      gluActiveVao = gluActiveVbo = gluMax;
    gluPackAlign = gluTexUnits = gluMaxVertexAttr = 0;
    glePolyMode = GL_NONE;
    // Set blank generic text for strings
    strvVendor = cCommon->CommonNull();
    strvVersion = cCommon->CommonNull();
    strvRenderer = cCommon->CommonNull();
    // Log class initialising
    cLog->LogInfoSafe("OGL subsystem de-initialised.");
  }
  /* -- Constructor --------------------------------------------- */ protected:
  Ogl() :
    /* -- Initialisers ----------------------------------------------------- */
    InitHelper{ __FUNCTION__ },        // Send name to InitHelper
    OglFlags{ GFL_NONE },              // Set no flags
    /* -- Const members ---------------------------------------------------- */
    glemExtensions{{                   // Init GL extension names
      IDMAPSTR(GL_BLEND),              IDMAPSTR(GL_DITHER),
      IDMAPSTR(GL_POLYGON_SMOOTH),     IDMAPSTR(GL_LINE_SMOOTH),
    }, "GL_EXT_UNKNOWN" },             // Unknown extension name
    glemHintTargets{{                  // Init hint target strings
      IDMAPSTR(GL_FRAGMENT_SHADER_DERIVATIVE_HINT),
      IDMAPSTR(GL_LINE_SMOOTH_HINT),   IDMAPSTR(GL_POLYGON_SMOOTH_HINT),
      IDMAPSTR(GL_TEXTURE_COMPRESSION_HINT),
    }, "GL_HINT_TARGET_UNKNOWN" },     // Unknown hint target name
    glemHintModes{{                    // INit hint mode strings
      IDMAPSTR(GL_FASTEST),            IDMAPSTR(GL_NICEST),
      IDMAPSTR(GL_DONT_CARE)
    }, "GL_HINT_MODE_UNKNOWN"},        // Unknown hint mode name
    glemFormatModes{{                  // Pixel format modes
      IDMAPSTR(GL_RGBA8),              IDMAPSTR(GL_RGB8),
      IDMAPSTR(GL_RED),                IDMAPSTR(GL_RG),
      IDMAPSTR(GL_RGB),                IDMAPSTR(GL_BGR),
      IDMAPSTR(GL_RGBA),               IDMAPSTR(GL_BGRA),
      IDMAPSTR(GL_RGBA_DXT1),          IDMAPSTR(GL_RGBA_DXT3),
      IDMAPSTR(GL_RGBA_DXT5)
    }, "GL_FORMAT_UNKNOWN"},           // Unknown pixel format mode
    glemOGLCodes{{                     // Init error codes
      IDMAPSTR(GL_NO_ERROR),           IDMAPSTR(GL_INVALID_ENUM),
      IDMAPSTR(GL_INVALID_VALUE),      IDMAPSTR(GL_INVALID_OPERATION),
#if !defined(MACOS)                    // These only work on non-Apple targets
      IDMAPSTR(GL_STACK_OVERFLOW),     IDMAPSTR(GL_STACK_UNDERFLOW),
#endif                                 // End of Apple target check
      IDMAPSTR(GL_OUT_OF_MEMORY),
    }, "GL_ERROR_UNKNOWN" },           // Unknown error value
    /* -- Engine blend ID to OpenGL blend id initialiser ------------------- */
    aBlends{                           // ## RGB & Alpha Blend Factors StringId
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
      GL_SRC_ALPHA_SATURATE },         // 14 (i,i,i)            1      S_A_S
    /* -- Engine filter ID to OpenGL filter ID initialiser (no mipmap) ----- */
    tfnmlMM{{ { GL_NEAREST, GL_NEAREST }, { GL_NEAREST, GL_LINEAR },    // 0-1
              { GL_LINEAR,  GL_NEAREST }, { GL_LINEAR,  GL_LINEAR } }}, // 2-3
    /* -- Engine filter ID to OpenGL filter ID initialiser ----------------- */
    tflMM{{ { GL_NEAREST, GL_NEAREST }, { GL_NEAREST, GL_LINEAR }, // 00-01
            { GL_LINEAR,  GL_NEAREST }, { GL_LINEAR,  GL_LINEAR }, // 02-03
            { GL_NEAREST, GL_NEAREST_MIPMAP_NEAREST },    // 04
            { GL_LINEAR,  GL_NEAREST_MIPMAP_NEAREST },    // 05
            { GL_NEAREST, GL_NEAREST_MIPMAP_LINEAR  },    // 06
            { GL_LINEAR,  GL_NEAREST_MIPMAP_LINEAR  },    // 07
            { GL_NEAREST, GL_LINEAR_MIPMAP_NEAREST  },    // 08
            { GL_LINEAR,  GL_LINEAR_MIPMAP_NEAREST  },    // 09
            { GL_NEAREST, GL_LINEAR_MIPMAP_LINEAR   },    // 10
            { GL_LINEAR,  GL_LINEAR_MIPMAP_LINEAR   } }}, // 11
    /* -- Other initialisers ----------------------------------------------- */
    ovhVao{ 0, 0 },                    // Initialise vertex array objects
    ovhVbo{ 0, 0 },                    // Initialise vertex buffer objects
    gluActiveFbo(gluMax),              // Select back buffer
    gluActiveProgram(gluMax),          // No active shader programme
    gluActiveTexture(gluMax),          // No active texture
    gluActiveTUnit(gluMax),            // No active texture unit
    gluActiveVao(gluMax),              // No active vertex array object
    gluActiveVbo(gluMax),              // No active vertex buffer object
    gluTexSize(0),                     // No maximum texture size
    gluPackAlign(0),                   // No pack align
    gluUnpackAlign(0),                 // No unpack align
    gluMaxVertexAttr(0),               // No maximum vertex attributes
    gluTexUnits(0),                    // No maximum texture units
    gluVaoGlobal(ovhVao[0]),           // Set reference to global Vao
    gluVboGlobal(ovhVbo[0]),           // Set reference to global Vbo
    gluVaoMain(ovhVao[1]),             // Set reference to bb draw Vao
    gluVboMain(ovhVbo[1]),             // Set reference to bb draw Vbo
    glePolyMode(GL_NONE),              // No set polygon mode yet
    gliUnpackRowLength(0),             // No unpack row length
    glullMinVRAM(0),                   // No minimum vram
    glullTotalVRAM(0),                 // No total vram
    glullFreeVRAM(0),                  // No free vram
    cdLimit{ cd0 },                    // Init frame duration
    strvRenderer{                      // Blank renderer
      cCommon->CommonNull() },         // Initialise with "<null>" text
    strvVersion{                       // Blank version
      cCommon->CommonNull() },         // Initialise with "<null>" text
    strvVendor{                        // Blank vendor
      cCommon->CommonNull() },         // Initialise with "<null>" text
    vsmSetting{ VSYNC_OFF }            // Set no VSync by default
    /* -- Set global pointer to static class ------------------------------- */
    { cOgl = this; }
  /* -- Destructor --------------------------------------------------------- */
  DTORHELPER(~Ogl, OglDeInit(true))
  /* -- Setup VSync ------------------------------------------------ */ public:
  CVarReturn SetVSyncMode(const int iValue)
  { // Deny if the value is not valid
    if(CVarSimpleSetIntNLGE(vsmSetting, static_cast<VSyncMode>(iValue),
      VSYNC_MIN, VSYNC_MAX) == DENY) return DENY;
    // If opengl is already initalised then update the new value immediately
    if(IsGLInitialised()) RestoreVSync();
    // Done
    return ACCEPT;
  }
  /* -- Update texture destroy list size ----------------------------------- */
  CVarReturn SetTexDListReserve(const size_t stCount)
    { return BoolToCVarReturn(UtilReserveList(gluivTextures, stCount)); }
  /* -- Update Fbo destroy list size --------------------------------------- */
  CVarReturn SetFboDListReserve(const size_t stCount)
    { return BoolToCVarReturn(UtilReserveList(gluivFbos, stCount)); }
  /* -- Update minimum VRAM ------------------------------------------------ */
  CVarReturn SetMinVRAM(const GLuint64 ullValue)
    { return CVarSimpleSetInt(glullMinVRAM, ullValue); }
  /* -- Update hints ------------------------------------------------------- */
  CVarReturn SetQCompressHint(const size_t stMode)
  { // Ignore if no context, but still succeeded
    if(IsGLNotInitialised()) return ACCEPT;
    // Get new value to set and return false if invalid
    switch(const GLenum gleNew = SHIndexToEnum(stMode))
    { // Invalid parameter or zero
      case GL_NONE: case GL_TRUE: return DENY;
      // Anything else, set the hint
      default: SetHint(GL_TEXTURE_COMPRESSION_HINT, gleNew); break;
    } // Succeeded
    return ACCEPT;
  }
  /* -- Update hints ------------------------------------------------------- */
  CVarReturn SetQPolygonHint(const size_t stMode)
  { // Ignore if no context, but still succeeded
    if(IsGLNotInitialised()) return ACCEPT;
    // Get new value to set and return false if invalid
    switch(const GLenum gleNew = SHIndexToEnum(stMode))
    { // Invalid parameter
      case GL_NONE: return DENY;
      // If feature is to be disabled? Disable it
      case GL_TRUE: DisableExtension(GL_POLYGON_SMOOTH); break;
      // Anything else, set the hint and enable it
      default: SetHint(GL_POLYGON_SMOOTH_HINT, gleNew);
               EnableExtension(GL_POLYGON_SMOOTH); break;
    } // Succeeded
    return ACCEPT;
  }
  /* -- Update hints ------------------------------------------------------- */
  CVarReturn SetQLineHint(const size_t stMode)
  { // Ignore if no context, but still succeeded
    if(IsGLNotInitialised()) return ACCEPT;
    // Get new value to set and return false if invalid
    switch(const GLenum gleNew = SHIndexToEnum(stMode))
    { // Invalid parameter
      case GL_NONE: return DENY;
      // If feature is to be disabled? Disable it
      case GL_TRUE: DisableExtension(GL_LINE_SMOOTH); break;
      // Anything else, set the hint and enable it
      default: SetHint(GL_LINE_SMOOTH_HINT, gleNew);
               EnableExtension(GL_LINE_SMOOTH); break;
    } // Succeeded
    return ACCEPT;
  }
  /* -- Update hints ------------------------------------------------------- */
  CVarReturn SetQShaderHint(const size_t stMode)
  { // Ignore if no context, but still succeeded
    if(IsGLNotInitialised()) return ACCEPT;
    // Get new value to set and return false if invalid
    switch(const GLenum gleNew = SHIndexToEnum(stMode))
    { // Invalid parameter or zero
      case GL_NONE: case GL_TRUE: return DENY;
      // Anything else, set the hint
      default: SetHint(GL_FRAGMENT_SHADER_DERIVATIVE_HINT, gleNew); break;
    } // Succeeded
    return ACCEPT;
  }
  /* -- Undefines ---------------------------------------------------------- */
#undef IGLC                            // This macro was only for this class
#undef IGL                             // This macro was only for this class
#undef IGLL                            // This macro was only for this class
};/* ----------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
