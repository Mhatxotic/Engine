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
using namespace ICommon::P;            using namespace ICVarDef::P;
using namespace IFboDef::P;            using namespace ILog::P;
using namespace IOgl::P;               using namespace IShader::P;
using namespace IStd::P;               using namespace IString::P;
using namespace Lib::OS::GlFW::Types;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* == Shader core class ==================================================== */
struct ShaderCore;                     // Class prototype
static ShaderCore *cShaderCore = nullptr; // Pointer to global class
struct ShaderCore                      // Actual body
{ /* -- 3D shader references ----------------------------------------------- */
  array<Shader,9> sh3DBuiltIns;        // list of built-in 3D shaders
  Shader          &sh3D,               // Basic 3D transformation shader
                  &sh3DYCbCr601FR,     // 3D YCbCr Rec.601 full-range shader
                  &sh3DYCbCr601PR,     // 3D YCbCr Rec.601 partial-range shader
                  &sh3DYCbCr709FR,     // 3D YCbCr Rec.709 full-range shader
                  &sh3DYCbCr709PR,     // 3D YCbCr Rec.709 partial-range shader
                  &sh3DYCbCrK601FR,    // Keyed Rec.601 full-range shader
                  &sh3DYCbCrK601PR,    // Keyed Rec.601 partial-range shader
                  &sh3DYCbCrK709FR,    // Keyed Rec.709 full-range shader
                  &sh3DYCbCrK709PR;    // Keyed Rec.709 partial-range shader
  /* -- 2D shader references ----------------------------------------------- */
  array<Shader,5> sh2DBuiltIns;        // list of built-in 2D shaders
  Shader          &sh2D,               // 2D-3D transformation shader
                  &sh2DBGR,            // 2D BGR-3D transformation shader
                  &sh2D8,              // 2D LUM-3D transformation shader
                  &sh2D8Pal,           // 2D LUMPAL-3D transformation shader
                  &sh2D16;             // 2D LUMAL-3D transformation shader
  /* -------------------------------------------------------------- */ private:
  typedef array<const string,5> RoundList;
  const RoundList rList;               // Rounding method list
  string          strSPRMethod;        // Rounding method string
  /* -- Add vertex shader with template and extra code --------------------- */
  static void AddVertexShaderWith3DTemplate(Shader &shS, const string &strName,
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
      "void main(){"                   // Entry point
        "vec4 v=vec4(vertex.xy,0,1);"  // Store vertex
        "vec4 tc=vec4(texcoord.xy,0,0);" // Store texcoord
        "vec4 c=colour;"               // Store colour
        "$"                            // Custom code here
        "texcoordout=tc;"              // Set colour from glColorPointer
        "colourout=c;"                 // Set colour
        "gl_Position=v;"               // Set vertex position
      "}",                             // End of main function
      stCompsPerCoord, stCompsPerPos, stCompsPerColour, cpCode);
  }
  /* -- Add vertex shader with template ------------------------------------ */
  static void AddVertexShaderWith3DTemplate(Shader &shS,
    const string &strName)
  { AddVertexShaderWith3DTemplate(shS, strName, cCommon->CommonCBlank()); }
  /* -- Add fragment shader with template ---------------------------------- */
  static void AddFragmentShaderWithTemplate(Shader &shS, const string &strName,
    const char*const cpCode, const char*const cpHeader)
  { // Add fragmnet shader program
    shS.AddShaderEx(strName, GL_FRAGMENT_SHADER,
      // > The fragment shader is for modifying actual pixel data
      // Input params    (ONE TRIANGLE, ?=spare)    V1     V2     V3
      "in vec4 texcoordout;"           // [3][4]=({xy--},{xy--},{xy--})
      "in vec4 colourout;"             // [3][4]=({rgba},{rgba},{rgba})
      "out vec4 pixel;"                // Pixel (RGBA) to set
      "uniform sampler2D tex;"         // Input texture
      "$"                              // Any extra header code
      "void main(){"                   // Entry point
        "vec4 p=texture(tex,texcoordout.xy);" // Save current pixel
        "vec4 c=colourout;"            // Save custom colour
        "$"                            // Custom code goes here
        "pixel=p;"                     // Set actual pixel
      "}", cpHeader, cpCode);          // Done
  }
  /* -- Add fragment shader with template ---------------------------------- */
  static void AddFragmentShaderWithTemplate(Shader &shS, const string &strName)
    { AddFragmentShaderWithTemplate(shS, strName, cCommon->CommonCBlank(),
        cCommon->CommonCBlank()); }
  /* -- Add fragment shader with template ---------------------------------- */
  static void AddFragmentShaderWithTemplate(Shader &shS, const string &strName,
      const char*const cpCode)
    { AddFragmentShaderWithTemplate(shS,
        strName, cpCode, cCommon->CommonCBlank()); }
  /* -- Add vertex shader with template ------------------------------------ */
  void AddVertexShaderWith2DTemplate(Shader &shS, const string &strName,
    const char*const cpCode)
  { // Add vertex shader program
    AddVertexShaderWith3DTemplate(shS, strName, StrFormat("$"
      "v.x=-1.0+(((matrix.x+$(v.x))/matrix.z)*2.0);"  // X-coord
      "v.y=-1.0+(((matrix.y+$(v.y))/matrix.w)*2.0);", // Y-coord
        cpCode, strSPRMethod, strSPRMethod).data());
  }
  /* -- Add vertex shader with template ------------------------------------ */
  void AddVertexShaderWith2DTemplate(Shader &shS, const string &strName)
    { AddVertexShaderWith2DTemplate(shS, strName, cCommon->CommonCBlank()); }
  /* ----------------------------------------------------------------------- */
  void Init3DShader()
  { // Add our basic 3D shader
    sh3D.LockSet();
    AddVertexShaderWith3DTemplate(sh3D, "VERT-3D");
    AddFragmentShaderWithTemplate(sh3D, "FRAG-3D RGB", "p=p*c;");
    sh3D.Link();
  }
  /* ----------------------------------------------------------------------- */
  void Init2DShader()
  { // Add our 2D to 3D transformation shader
    sh2D.LockSet();
    AddVertexShaderWith2DTemplate(sh2D, "VERT-2D");
    AddFragmentShaderWithTemplate(sh2D, "FRAG-2D RGB", "p=p*c;");
    sh2D.Link();
  }
  /* ----------------------------------------------------------------------- */
  void Init2DBGRShader()
  { // Add our 2D to 3D BGR transformation shader
    sh2DBGR.LockSet();
    AddVertexShaderWith2DTemplate(sh2DBGR, "VERT-2D");
    AddFragmentShaderWithTemplate(sh2DBGR, "FRAG-2D BGR>RGB",
      "float r=p.r;p.r=p.b;p.b=r;p=p*c;");
    sh2DBGR.Link();
  }
  /* ----------------------------------------------------------------------- */
  void Init2D8Shader()
  { // Add our 2D to 3D transformation shader with GL_LUMINANCE decoding
    sh2D8.LockSet();
    AddVertexShaderWith2DTemplate(sh2D8, "VERT-2D");
    AddFragmentShaderWithTemplate(sh2D8, "FRAG-2D LU>RGB",
      "p.r=p.g=p.b=p.r;p.a=1.0;p=p*c;");
    sh2D8.Link();
  }
  /* ----------------------------------------------------------------------- */
  void Init2D8PalShader()
  { // Add our 2D to 3D transformation shader with GL_LUMINANCE decoding
    sh2D8Pal.LockSet();
    AddVertexShaderWith2DTemplate(sh2D8Pal, "VERT-2D");
    AddFragmentShaderWithTemplate(sh2D8Pal, "FRAG-2D PAL>RGB",
      "p=pal[int(p.r*255)]*c;",        // Set pixel and modulate
      "uniform vec4 pal[256];");       // Global colour palette
    sh2D8Pal.Link();
    // We need the location of the palette
    sh2D8Pal.VerifyUniformLocation("pal", U_PALETTE);
  }
  /* ----------------------------------------------------------------------- */
  void Init2D16Shader()
  { // Add our 2D to 3D transformation shader with GL_LUMINANCE_ALPHA decoding
    sh2D16.LockSet();
    AddVertexShaderWith2DTemplate(sh2D16, "VERT-2D");
    AddFragmentShaderWithTemplate(sh2D16, "FRAG-2D LUA>RGB",
      "p.a=p.g;p.g=p.b=p.r;p=p*c;");
    sh2D16.Link();
  }
  /* ----------------------------------------------------------------------- */
  static void Init3DYCbCrTemplate(Shader &shDest, const string &strName,
    const string_view &svRangeCode, const string_view &svMatrixCode,
    const string_view &svKeyCode)
  { // Add YCbCr to RGB shaders
    shDest.LockSet();
    AddVertexShaderWith3DTemplate(shDest, "VERT-3D");
    shDest.AddShaderEx(strName, GL_FRAGMENT_SHADER,
      "in vec4 texcoordout;"           // Texture info
      "in vec4 colourout;"             // Colour info
      "out vec4 pixel;"                // Pixel out
      "uniform sampler2D texY;"        // MultiTex unit 0 for Y component data
      "uniform sampler2D texCb;"       // MultiTex unit 1 for Cb component data
      "uniform sampler2D texCr;"       // MultiTex unit 2 for Cr component data
      "void main(){"                   // Entry point
        "vec3 ycbcr;"                  // Y, Cb and Cr components
        "$"                            // Dynamic range modification code
        "vec3 rgb=mat3($)*ycbcr;"      // Convert YCbCr to RGB matrix code
        "pixel=vec4(rgb,$);"           // Output the desired pixel color code
      "}", svRangeCode, svMatrixCode, svKeyCode);
    shDest.Link();
    shDest.Activate();
    // For each texture unit
    static const array<const GLchar*const,3>
      acpCmp{ "texY", "texCb", "texCr" };
    for(size_t stIndex = 0; stIndex < acpCmp.size(); ++stIndex)
    { // Get location of specified variable in gpu shader and set to the
      // required texture unit.
      const GLchar*const cpUniform = acpCmp[stIndex];
      const GLint &iUniformId = shDest.GetUniformLocation(cpUniform);
      GLC("Failed to get uniform location from YCbCr shader!",
        "Variable", cpUniform, "Index", stIndex);
      GL(cOgl->Uniform(iUniformId, static_cast<GLint>(stIndex)),
        "Failed to set YCbCr uniform texture unit!",
        "Variable", cpUniform, "Index", stIndex, "Uniform", iUniformId);
    }
  }
  /* ----------------------------------------------------------------------- */
  void Init3DYCbCrShaders()
  { // No colour keying (no transparency)
    const string_view svNoKey = "texture(texCr,vec2(texcoordout)).a",
    // Colour keying code
    svKey = "abs(colourout.r-rgb.r)<=colourout.a&&"
            "abs(colourout.g-rgb.g)<=colourout.a&&"
            "abs(colourout.b-rgb.b)<=colourout.a?0:1",
    // Rec.601 matrix code
    sv601 = "1,1,1,0,-0.344,1.77,1.403,-0.714,0",
    // Rec.709 matrix code
    sv709 = "1.0,1.0,1.0,0.0,-0.18732,1.8556,1.5748,-0.46812,0.0",
    // Full-dynamic range code
    svFull =
      // Sample the Y, Cb, and Cr components from textures
      "ycbcr.x=texture(texY,vec2(texcoordout)).r*255.0;"
      "ycbcr.y=texture(texCb,vec2(texcoordout)).r*255.0;"
      "ycbcr.z=texture(texCr,vec2(texcoordout)).r*255.0;"
      // Adjust for limited range YUV
      "ycbcr.x=(ycbcr.x-16.0)/219.0;"
      "ycbcr.y=(ycbcr.y-128.0)/224.0;"
      "ycbcr.z=(ycbcr.z-128.0)/224.0;",
    // Partial-dynamic range code
    svPartial =
      // Sample the Y, Cb, and Cr components from textures
      "ycbcr.x=texture(texY,vec2(texcoordout)).r;"
      "ycbcr.y=texture(texCb,vec2(texcoordout)).r-0.5;"
      "ycbcr.z=texture(texCr,vec2(texcoordout)).r-0.5;";
    // 3D YCbCr shaders to comple
    const struct ShaderList {
      Shader &shShader;                // Shader class destination
      const string      &strName;      // Name of shader
      const string_view &svRange,      // Shader dynamic range code
                        &svMatrix,     // Shader colour range matrix code
                        &svKey;        // RGB keying code
    } slShaders[] = {
      { sh3DYCbCr601FR, "FRAG-3D YCbCr>F601>RGB", svFull, sv601, svNoKey },
      { sh3DYCbCr601FR, "FRAG-3D YCbCr>F601>RGB", svFull, sv601, svNoKey },
      { sh3DYCbCr601PR, "FRAG-3D YCbCr>P601>RGB", svPartial, sv601, svNoKey },
      { sh3DYCbCr601PR, "FRAG-3D YCbCr>P601>RGB", svPartial, sv601, svNoKey },
      { sh3DYCbCr709FR, "FRAG-3D YCbCr>F709>RGB", svFull, sv709, svNoKey },
      { sh3DYCbCr709FR, "FRAG-3D YCbCr>F709>RGB", svFull, sv709, svNoKey },
      { sh3DYCbCr709PR, "FRAG-3D YCbCr>P709>RGB", svPartial, sv709, svNoKey },
      { sh3DYCbCr709PR, "FRAG-3D YCbCr>P709>RGB", svPartial, sv709, svNoKey },
      { sh3DYCbCrK601FR, "FRAG-3D YCbCr>F601>RGBK", svFull, sv601, svKey },
      { sh3DYCbCrK601FR, "FRAG-3D YCbCr>F601>RGBK", svFull, sv601, svKey },
      { sh3DYCbCrK601PR, "FRAG-3D YCbCr>P601>RGBK", svPartial, sv601, svKey },
      { sh3DYCbCrK601PR, "FRAG-3D YCbCr>P601>RGBK", svPartial, sv601, svKey },
      { sh3DYCbCrK709FR, "FRAG-3D YCbCr>F709>RGBK", svFull, sv709, svKey },
      { sh3DYCbCrK709FR, "FRAG-3D YCbCr>F709>RGBK", svFull, sv709, svKey },
      { sh3DYCbCrK709PR, "FRAG-3D YCbCr>P709>RGBK", svPartial, sv709, svKey },
      { sh3DYCbCrK709PR, "FRAG-3D YCbCr>P709>RGBK", svPartial, sv709, svKey }
    };
    // Compile each of the above shaders
    for(const ShaderList &slShader : slShaders)
      Init3DYCbCrTemplate(slShader.shShader, slShader.strName,
        slShader.svRange, slShader.svMatrix, slShader.svKey);
  }
  /* -- Initialise built-in shaders -------------------------------- */ public:
  void InitShaders()
  { // Log initialisation
    cLog->LogDebugExSafe(
      "ShaderCore initialising $ built-in 3D shader objects...",
      sh3DBuiltIns.size());
    // Setup 3D shaders
    Init3DShader();
    Init3DYCbCrShaders();
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
  void DeInitShaders()
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
  /* -- Default constructor ------------------------------------- */ protected:
  ShaderCore() :
    /* -- Initialisers ----------------------------------------------------- */
    sh3D{ sh3DBuiltIns[0] },           sh3DYCbCr601FR{ sh3DBuiltIns[1] },
    sh3DYCbCr601PR{ sh3DBuiltIns[2] }, sh3DYCbCr709FR{ sh3DBuiltIns[3] },
    sh3DYCbCr709PR{ sh3DBuiltIns[4] }, sh3DYCbCrK601FR{ sh3DBuiltIns[5] },
    sh3DYCbCrK601PR{ sh3DBuiltIns[6] },sh3DYCbCrK709FR{ sh3DBuiltIns[7] },
    sh3DYCbCrK709PR{ sh3DBuiltIns[8] },sh2D{ sh2DBuiltIns[0] },
    sh2DBGR{ sh2DBuiltIns[1] },        sh2D8{ sh2DBuiltIns[2] },
    sh2D8Pal{ sh2DBuiltIns[3] },       sh2D16{ sh2DBuiltIns[4] },
    /* -- Rounding list ---------------------------------------------------- */
    rList{{                            // Initialise rounding strings list
      cCommon->CommonBlank(),          // [0] No rounding
      "floor",                         // [1] Floor rounding
      "ceil",                          // [2] Ceil rounding
      "round",                         // [3] Nearest whole number
      "roundEven"                      // [4] Nearest even number
    }}                                 // End of rounding strings list
    /* -- Set global pointer to static class ------------------------------- */
    { cShaderCore = this; }
  /* -- Set rounding method for the shader ------------------------- */ public:
  CVarReturn SetSPRoundingMethod(const size_t stMethod)
  { // Return if specified value is outrageous!
    if(stMethod >= rList.size()) return DENY;
    strSPRMethod = rList[stMethod];
    // Success
    return ACCEPT;
  }
};/* ----------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
