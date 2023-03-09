/* == SHADERS.HPP ========================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## Variables to help with fbos, rendering threading and context.       ## **
** ######################################################################### */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IShaders {                   // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace ICVarDef::P;           using namespace IFboDef::P;
using namespace ILog::P;               using namespace IOgl::P;
using namespace IShader::P;            using namespace IStd::P;
using namespace IString::P;            using namespace Lib::OS::GlFW;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* == Shader core class ==================================================== */
static struct ShaderCore final
{ /* -- 3D shader references ----------------------------------------------- */
  array<Shader,3> sh3DBuiltIns;        // list of built-in 3D shaders
  Shader          &sh3D;               // Basic 3D transformation shader
  Shader          &sh3DYCbCr;          // 3D YCbCr transformation shader
  Shader          &sh3DYCbCrK;         // 3D YCbCr ckey transformation shader
  /* -- 2D shader references ----------------------------------------------- */
  array<Shader,5> sh2DBuiltIns;        // list of built-in 2D shaders
  Shader          &sh2D;               // 2D-3D transformation shader
  Shader          &sh2DBGR;            // 2D BGR-3D transformation shader
  Shader          &sh2D8;              // 2D LUM-3D transformation shader
  Shader          &sh2D8Pal;           // 2D LUMPAL-3D transformation shader
  Shader          &sh2D16;             // 2D LUMAL-3D transformation shader
  /* -------------------------------------------------------------- */ private:
  typedef array<const string,5> RoundList;
  const RoundList rList;               // Rounding method list
  string          strSPRMethod;        // Rounding method string
  /* -- Add vertex shader with template and extra code --------------------- */
  void AddVertexShaderWith3DTemplate(Shader &shS, const string &strName,
    const char*const cpCode)
  { // Add vertex shader program
    shS.AddShaderEx(strName, GL_VERTEX_SHADER,
      // > The vertex shader is for modifying vertice coord data
      "in vec$ texcoord;"              // Texture coordinates for vertice
      "in vec$ vertex;"                // Position coordinates for vertice
      "in vec$ colour;"                // Colour intensity for vertice
      "out vec4 texcoordout;"          // Tex coords sent to fragment shader
      "out vec4 colourout;"            // Colour multiplier sent to frag shader
      "uniform vec4 matrix;"           // Current 2D matrix
      "void main(void){"               // Entry point
        "vec4 v = vec4(vertex.xy,0,1);"    // Store vertex
        "vec4 tc = vec4(texcoord.xy,0,0);" // Store texcoord
        "vec4 c = colour;"             // Store colour
        "$"                            // Custom code here
        "texcoordout = tc;"            // Set colour from glColorPointer
        "colourout = c;"               // Set colour
        "gl_Position = v;"             // Set vertex position
      "}",                             // End of main function
      stCompsPerCoord, stCompsPerPos, stCompsPerColour, cpCode);
  }
  /* -- Add vertex shader with template ------------------------------------ */
  void AddVertexShaderWith3DTemplate(Shader &shS, const string &strName)
    { AddVertexShaderWith3DTemplate(shS, strName, cCommon->CBlank()); }
  /* -- Add fragment shader with template ---------------------------------- */
  void AddFragmentShaderWithTemplate(Shader &shS, const string &strName,
    const char*const cpCode, const char*const cpHeader)
  { // Add fragmnet shader program
    shS.AddShaderEx(strName, GL_FRAGMENT_SHADER,
      // > The fragment shader is for modifying actual pixel data
      // Input params    (ONE TRIANGLE, ?=spare)    V1     V2     V3
      "in vec4 texcoordout;"           // [3][4]=({xy--},{xy--},{xy--})
      "in vec4 colourout;"             // [3][4]=({rgba},{rgba},{rgba})
      "out vec4 pixel;"                // Pixel (RGBA) to set
      "uniform sampler2D tex;"                  // Input texture
      "$"                                       // Any extra header code
      "void main(void){"                        // Entry point
        "vec4 p = texture(tex,texcoordout.xy);" // Save current pixel
        "vec4 c = colourout;"                   // Save custom colour
        "$"                                     // Custom code goes here
        "pixel = p;"                            // Set actual pixel
      "}", cpHeader, cpCode);                   // Done
  }
  /* -- Add fragment shader with template ---------------------------------- */
  void AddFragmentShaderWithTemplate(Shader &shS, const string &strName)
    { AddFragmentShaderWithTemplate(shS, strName, cCommon->CBlank(),
        cCommon->CBlank()); }
  /* -- Add fragment shader with template ---------------------------------- */
  void AddFragmentShaderWithTemplate(Shader &shS, const string &strName,
    const char*const cpCode)
  { AddFragmentShaderWithTemplate(shS, strName, cpCode, cCommon->CBlank()); }
  /* -- Add vertex shader with template ------------------------------------ */
  void AddVertexShaderWith2DTemplate(Shader &shS, const string &strName,
    const char*const cpCode)
  { // Add vertex shader program
    AddVertexShaderWith3DTemplate(shS, strName, StrFormat("$"
      "v.x = -1.0+(((matrix.x+$(v.x))/matrix.z)*2.0);"  // X-coord
      "v.y = -1.0+(((matrix.y+$(v.y))/matrix.w)*2.0);", // Y-coord
        cpCode, strSPRMethod, strSPRMethod).c_str());
  }
  /* -- Add vertex shader with template ------------------------------------ */
  void AddVertexShaderWith2DTemplate(Shader &shS, const string &strName)
    { AddVertexShaderWith2DTemplate(shS, strName, cCommon->CBlank()); }
  /* ----------------------------------------------------------------------- */
  void Init3DShader(void)
  { // Add our basic 3D shader
    sh3D.LockSet();
    AddVertexShaderWith3DTemplate(sh3D, "VERT-3D");
    AddFragmentShaderWithTemplate(sh3D, "FRAG-3D RGB", "p = p * c;");
    sh3D.Link();
  }
  /* ----------------------------------------------------------------------- */
  void Init2DShader(void)
  { // Add our 2D to 3D transformation shader
    sh2D.LockSet();
    AddVertexShaderWith2DTemplate(sh2D, "VERT-2D");
    AddFragmentShaderWithTemplate(sh2D, "FRAG-2D RGB", "p = p * c;");
    sh2D.Link();
  }
  /* ----------------------------------------------------------------------- */
  void Init2DBGRShader(void)
  { // Add our 2D to 3D BGR transformation shader
    sh2DBGR.LockSet();
    AddVertexShaderWith2DTemplate(sh2DBGR, "VERT-2D");
    AddFragmentShaderWithTemplate(sh2DBGR, "FRAG-2D BGR>RGB",
      "float r = p.r; p.r = p.b; p.b = r; p = p * c;");
    sh2DBGR.Link();
  }
  /* ----------------------------------------------------------------------- */
  void Init2D8Shader(void)
  { // Add our 2D to 3D transformation shader with GL_LUMINANCE decoding
    sh2D8.LockSet();
    AddVertexShaderWith2DTemplate(sh2D8, "VERT-2D");
    AddFragmentShaderWithTemplate(sh2D8, "FRAG-2D LU>RGB",
      "p.r = p.g = p.b = p.r; p.a = 1.0; p = p * c;");
    sh2D8.Link();
  }
  /* ----------------------------------------------------------------------- */
  void Init2D8PalShader(void)
  { // Add our 2D to 3D transformation shader with GL_LUMINANCE decoding
    sh2D8Pal.LockSet();
    AddVertexShaderWith2DTemplate(sh2D8Pal, "VERT-2D");
    AddFragmentShaderWithTemplate(sh2D8Pal, "FRAG-2D PAL>RGB",
      "p = pal[int(p.r * 255)] * c;",  // Set pixel and modulate
      "uniform vec4 pal[256];");       // Global colour palette
    sh2D8Pal.Link();
    // We need the location of the palette
    sh2D8Pal.VerifyUniformLocation("pal", U_PALETTE);
  }
  /* ----------------------------------------------------------------------- */
  void Init2D16Shader(void)
  { // Add our 2D to 3D transformation shader with GL_LUMINANCE_ALPHA decoding
    sh2D16.LockSet();
    AddVertexShaderWith2DTemplate(sh2D16, "VERT-2D");
    AddFragmentShaderWithTemplate(sh2D16, "FRAG-2D LUA>RGB",
      "p.a = p.g; p.g = p.b = p.r; p = p * c;");
    sh2D16.Link();
  }
  /* ----------------------------------------------------------------------- */
  void Init3DYCbCrTemplate(Shader &shDest, const char*const cpName,
    const char*const cpCode)
  { // Add YCbCr to RGB shaders
    shDest.LockSet();
    AddVertexShaderWith3DTemplate(shDest, "VERT-3D");
    shDest.AddShaderEx(cpName, GL_FRAGMENT_SHADER,
      "in vec4 texcoordout;"           // Texture info
      "in vec4 colourout;"             // Colour info
      "out vec4 pixel;"                // Pixel out
      "uniform sampler2D texY;"        // MultiTex unit 0 for Y component data
      "uniform sampler2D texCb;"       // MultiTex unit 1 for Cb component data
      "uniform sampler2D texCr;"       // MultiTex unit 2 for Cr component data
      "void main(void){"
        "mediump vec3 ycbcr;"
        "lowp vec3 rgb;"
        "ycbcr.x = texture(texY,vec2(texcoordout)).r;"
        "ycbcr.y = texture(texCb,vec2(texcoordout)).r-0.5;"
        "ycbcr.z = texture(texCr,vec2(texcoordout)).r-0.5;"
        "rgb = mat3(1,1,1,0,-0.344,1.77,1.403,-0.714,0)*ycbcr;"
        "pixel = vec4(rgb,$);"
      "}", cpCode);
    shDest.Link();
    shDest.Activate();
    // For each texture unit
    static const array<const GLchar*const,3>
      acpCmp{ "texY", "texCb", "texCr" };
    for(size_t stIndex = 0; stIndex < acpCmp.size(); ++stIndex)
    { // Get location of specified variable in gpu shader and set to the
      // required texture unit.
      const GLchar*const cpUniform = acpCmp[stIndex];
      const GLint &iUniformId = sh3DYCbCr.GetUniformLocation(cpUniform);
      GLC("Failed to get uniform location from YCbCr shader!",
        "Variable", cpUniform, "Index", stIndex);
      GL(cOgl->Uniform(iUniformId, static_cast<GLint>(stIndex)),
        "Failed to set YCbCr uniform texture unit!",
        "Variable", cpUniform, "Index", stIndex, "Uniform", iUniformId);
    }
  }
  /* ----------------------------------------------------------------------- */
  void Init3DYCbCrShader(void)
  { // Initialuse YCbCr 3D shader with basic conversion
    Init3DYCbCrTemplate(sh3DYCbCr, "FRAG-3D YCbCr>RGB",
      "texture(texCr,vec2(texcoordout)).a");
  }
  /* ----------------------------------------------------------------------- */
  void Init3DYCbCrKShader(void)
  { // Initialuse YCbCr 3D shader with colour key decoder
    Init3DYCbCrTemplate(sh3DYCbCrK, "FRAG-3D YCbCr>RGB>CK",
      "abs(colourout.r-rgb.r)<=colourout.a&&"
      "abs(colourout.g-rgb.g)<=colourout.a&&"
      "abs(colourout.b-rgb.b)<=colourout.a?0:1");
  }
  /* -- Initialise built-in shaders -------------------------------- */ public:
  void InitShaders(void)
  { // Log initialisation
    cLog->LogDebugExSafe(
      "ShaderCore initialising $ built-in 3D shader objects...",
      sh3DBuiltIns.size());
    // Setup 3D shaders
    Init3DShader();
    Init3DYCbCrShader();
    Init3DYCbCrKShader();
    cLog->LogDebugExSafe(
      "ShaderCore initialising $ built-in 2D shader objects...",
      sh2DBuiltIns.size());
    // Setup 2D shaders
    Init2DShader();
    Init2DBGRShader();
    Init2D8Shader();
    Init2D8PalShader();
    Init2D16Shader();
    // Log completion
    cLog->LogInfoExSafe("ShaderCore initialised $ built-in shader objects.",
      sh3DBuiltIns.size() + sh2DBuiltIns.size());
  }
  /* -- De-initialise built in shaders ------------------------------------- */
  void DeInitShaders(void)
  { // De-init built-in shaders
    cLog->LogDebugExSafe(
      "ShaderCore de-initialising $ built-in 3D shader objects...",
      sh3DBuiltIns.size());
    StdForEach(seq, sh3DBuiltIns.rbegin(), sh3DBuiltIns.rend(),
      [](Shader &shS) { shS.DeInit(); });
    cLog->LogDebugExSafe(
      "ShaderCore de-initialising $ built-in 2D shader objects...",
      sh2DBuiltIns.size());
    StdForEach(seq, sh2DBuiltIns.rbegin(), sh2DBuiltIns.rend(),
      [](Shader &shS) { shS.DeInit(); });
    cLog->LogInfoExSafe("ShaderCore de-initialised $ built-in shader objects.",
      sh3DBuiltIns.size() + sh2DBuiltIns.size());
  }
  /* ----------------------------------------------------------------------- */
  ShaderCore(void) :                   // No parameters
    /* -- Initialisers ----------------------------------------------------- */
    sh3D{ sh3DBuiltIns[0] },           sh3DYCbCr{ sh3DBuiltIns[1] },
    sh3DYCbCrK{ sh3DBuiltIns[2] },     sh2D{ sh2DBuiltIns[0] },
    sh2DBGR{ sh2DBuiltIns[1] },        sh2D8{ sh2DBuiltIns[2] },
    sh2D8Pal{ sh2DBuiltIns[3] },       sh2D16{ sh2DBuiltIns[4] },
    /* -- Rounding list ---------------------------------------------------- */
    rList{{                            // Initialise rounding strings list
      cCommon->Blank(),                // [0] No rounding
      "floor",                         // [1] Floor rounding
      "ceil",                          // [2] Ceil rounding
      "round",                         // [3] Nearest whole number
      "roundEven"                      // [4] Nearest even number
    }}                                 // End of rounding strings list
    /* -- Code ------------------------------------------------------------- */
    { }                                // No code
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(ShaderCore)          // Suppress default functions for safety
  /* -- Set rounding method for the shader --------------------------------- */
  CVarReturn SetSPRoundingMethod(const size_t stMethod)
  { // Return if specified value is outrageous!
    if(stMethod >= rList.size()) return DENY;
    strSPRMethod = rList[stMethod];
    // Success
    return ACCEPT;
  }
  /* ----------------------------------------------------------------------- */
} *cShaderCore = nullptr;              // Global access
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
