/* == SHADER.HPP =========================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## This allows programs and shaders to be easily compiled, linked and  ## **
** ## used.                                                               ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IShader {                    // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace ICollector::P;         using namespace IError::P;
using namespace IFboDef::P;            using namespace IIdent::P;
using namespace ILog::P;               using namespace ILuaLib::P;
using namespace IOgl::P;               using namespace IStd::P;
using namespace IString::P;            using namespace ISysUtil::P;
using namespace Lib::OS::GlFW;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* -- Public typedefs ------------------------------------------------------ */
enum ShaderUniformId : size_t          // Mandatory uniforms
{ /* ----------------------------------------------------------------------- */
  U_MATRIX,                            // Matrix uniform vec4
  U_PALETTE,                           // Palette uniform vec4
  /* ----------------------------------------------------------------------- */
  U_MAX                                // Max no of mandatory uniforms
};/* ----------------------------------------------------------------------- */
enum ShaderAttributeId : GLuint        // Mandatory attributes
{ /* ----------------------------------------------------------------------- */
  A_COORD,                             // TexCoord attribute vec2 array
  A_VERTEX,                            // Vertex attribute vec2 array
  A_COLOUR,                            // Colour attribute vec4 array
  /* ----------------------------------------------------------------------- */
  A_MAX                                // Max no of mandatory attributes
};/* -- Shader list class -------------------------------------------------- */
class ShaderCell :                     // Members initially private
  /* -- Initialisers ------------------------------------------------------- */
  public Ident                         // Name of string
{ /* -- Private variables -------------------------------------------------- */
  const string     strCode;            // Shader name and code
  const GLenum     eType;              // Shader type
  const GLuint     uiShader;           // Created shader id
  /* -- Return code of shader -------------------------------------- */ public:
  const string &GetCode(void) const { return strCode; }
  /* -- Return length of shader code --------------------------------------- */
  size_t GetCodeLength(void) const { return GetCode().length(); }
  /* -- Return name of shader as C-String ---------------------------------- */
  const char *GetCodeCStr(void) const { return GetCode().c_str(); }
  /* -- Return type of shader ---------------------------------------------- */
  GLenum GetType(void) const { return eType; }
  /* -- Return shader id --------------------------------------------------- */
  GLuint GetHandle(void) const { return uiShader; }
  /* -- Default constructor ------------------------------------------------ */
  ShaderCell(const string &strNName,   // Specified new identifier
             const string &strNCode,   // Specified code to copmile
             const GLenum eNType,      // Specified GL type id of code
             const GLuint uiNShader) : // Specified GL shader id of code
    /* -- Initialisers ----------------------------------------------------- */
    Ident{ strNName },                 // Set specified name
    strCode{ strNCode },               // Set code
    eType{ eNType },                   // Set type of shader
    uiShader{ uiNShader }              // Set shader handle
    /* -- No code ---------------------------------------------------------- */
    { }
};/* ----------------------------------------------------------------------- */
typedef list<ShaderCell> ShaderList;   // Shader cell list
/* ------------------------------------------------------------------------- */
CTOR_BEGIN_DUO(Shaders, Shader, CLHelperUnsafe, ICHelperUnsafe),
  /* -- Base classes ------------------------------------------------------- */
  public Lockable,                     // Lua garbage collector instruction
  public ShaderList                    // List of shader data in this program
{ /* -- Private typedefs --------------------------------------------------- */
  typedef array<GLint, U_MAX> UniList; // Uniform array list
  /* -- Private variables -------------------------------------------------- */
  GLuint           uiProgram;          // Shader program id
  UniList          aUniforms;          // Ids of mandatory uniforms we need
  bool             bLinked;            // Shader program has been linked
  /* -- Get program id number -------------------------------------- */ public:
  GLuint GetProgramId(void) const { return uiProgram; }
  /* -- SHader is linked? -------------------------------------------------- */
  bool IsLinked(void) const { return bLinked; }
  /* -- Verify the specified attribute is at the specified location -------- */
  void VerifyAttribLocation(const char *cpAttr, const ShaderAttributeId saiId)
  { // Get attribute location
    GL(cOgl->BindAttribLocation(uiProgram, saiId, cpAttr),
      "Failed to get attribute location from shader!",
      "Attrib", cpAttr, "Program", uiProgram, "Index", saiId);
    // Enable the vertex attrib array. Keep an eye on this if you have problems
    // with glVertexAttribPointer. You'll have to restore Enable/Disable vertex
    // attrib pointers before both glDrawArrays calls if you add more shaders
    // and this call fails.
    GL(cOgl->EnableVertexAttribArray(saiId),
      "Failed to enable vertex attrib array!",
      "Attrib", cpAttr, "Program", uiProgram, "Index", saiId);
    // Report location in log
    cLog->LogDebugExSafe("Shader bound attribute '$' at location $.",
      cpAttr, saiId);
  }
  /* -- Verify the specified uniform is at the specified location ---------- */
  void VerifyUniformLocation(const char *cpUni, const ShaderUniformId suiId)
  { // Get attribute location
    GL(aUniforms[suiId] = GetUniformLocation(cpUni),
      "Failed to get uniform location from shader!",
      "Uniform", cpUni, "Program", uiProgram, "Assign", suiId);
    // Report location in log
    cLog->LogDebugExSafe("Shader attribute for '$' at location $ and index $.",
      cpUni, GetUID(suiId), suiId);
  }
  /* -- Update palette ----------------------------------------------------- */
  void UpdatePalette(const size_t stSize, const GLfloat*const fpData) const
  { // Activate shader (no error checking)
    cOgl->UseProgram(GetProgram());
    // Commit palette
    cOgl->Uniform(GetUID(U_PALETTE), static_cast<GLsizei>(stSize), fpData);
  }
  /* -- Update shader matrix ----------------------------------------------- */
  void UpdateMatrix(const FboRenderItem &friRef) const
  { // Activate shader (no error checking)
    cOgl->UseProgram(GetProgram());
    // Commit matrix bounds (no error checking)
    cOgl->Uniform(GetUID(U_MATRIX), friRef.GetCoLeft(),
      friRef.GetCoTop(), friRef.GetCoRight(), friRef.GetCoBottom());
  }
  /* -- Linkage ------------------------------------------------------------ */
  void Link(void)
  { // We want to make things simple and make sure every shader has a specific
    // attribute at a specific index so make sure these are in the correct
    // positions. This has to be done before the link.
    VerifyAttribLocation("texcoord", A_COORD);
    VerifyAttribLocation("vertex", A_VERTEX);
    VerifyAttribLocation("colour", A_COLOUR);
    // Do the link
    GL(cOgl->LinkProgram(uiProgram), "Link shader program failed!",
      "Program", uiProgram);
    // Get link result and show reason if failed
    if(cOgl->GetLinkStatus(uiProgram) == GL_FALSE)
      XC("Shader program linkage failed!", "Program", uiProgram,
        "Reason", cOgl->GetLinkFailureReason(uiProgram));
    // Program linked
    bLinked = true;
    // Activate
    Activate();
    // Verify uniforms are in the correct position
    VerifyUniformLocation("matrix", U_MATRIX);
  }
  /* -- Activate program --------------------------------------------------- */
  void Activate(void)
  { // Do activate the program
    GL(cOgl->UseProgram(uiProgram), "Failed to select shader program!",
      "Program", uiProgram);
  }
  /* -- Deselect program --------------------------------------------------- */
  void Deactivate(void)
    { if(cOgl->GetProgram() == uiProgram)
        GLL(cOgl->UseProgram(0), "Failed to deselect active program!",
          "Program", uiProgram); }
  /* -- Get shader program name -------------------------------------------- */
  GLuint GetProgram(void) const { return uiProgram; }
  /* -- Uniform value ------------------------------------------------------ */
  GLint GetUID(const ShaderUniformId suiId) const { return aUniforms[suiId]; }
  /* -- Variable location -------------------------------------------------- */
  GLint GetUniformLocation(const char*const cpVar) const
    { return cOgl->GetUniformLocation(uiProgram, cpVar); }
  /* -- Shader initialiser ------------------------------------------------- */
  void AddShader(const string &strName, const GLenum eT, const string &strC)
  { // We need to deinit if we already linked
    if(bLinked) DeInit();
    // Create shader item and add it to list. We'll push it now so the
    // destructor can clean up any created data.
    const size_t stIndex = size();
    const ShaderCell &scItem = *insert(cend(), { strName,
      StrAppend("#version 150\n", strC), eT, cOgl->CreateShader(eT) });
    // Check the shader
    if(!scItem.GetHandle())
      XC("Failed to create GL shader!",
         "Identifier", scItem.IdentGet(), "Type", scItem.GetType());
    // Set shader source code
    GL(cOgl->ShaderSource(scItem.GetHandle(), scItem.GetCodeCStr()),
      "Failed to set shader source code!",
        "Identifier", scItem.IdentGet(),  "Type",   scItem.GetType(),
        "Shader",     scItem.GetHandle(), "Source", scItem.GetCode());
    // Compile the shader source code
    GL(cOgl->CompileShader(scItem.GetHandle()),
      "Failed to compile shader source code!",
        "Type", eT, "Shader", scItem.GetHandle(), "Source", scItem.GetCode());
    // Get compiler result and show reason if failed
    if(cOgl->GetCompileStatus(scItem.GetHandle()) == GL_FALSE)
      XC("Shader compilation failed!",
        "Identifier", scItem.IdentGet(), "Program", uiProgram,
        "Type",       scItem.GetType(),  "Shader",  scItem.GetHandle(),
        "Reason",     cOgl->GetCompileFailureReason(scItem.GetHandle()));
    // If the program hasn't been setup yet?
    if(!uiProgram)
    { // Create the shader program, and if failed? We should bail out.
      uiProgram = cOgl->CreateProgram();
      if(!uiProgram) XC("Failed to create shader program!",
        "Identifier", scItem.IdentGet(),
        "Type",       scItem.GetType(), "Shader", scItem.GetHandle());
    } // Attach the shader to the program
    GL(cOgl->AttachShader(uiProgram, scItem.GetHandle()),
      "Failed to attach shader to program!",
        "Identifier", scItem.IdentGet(), "Program", uiProgram,
        "Type",       scItem.GetType(),  "Shader",  scItem.GetHandle());
    // Shader compiled
    cLog->LogDebugExSafe("Shader '$'($) compiled at index $ on program $.",
      scItem.IdentGet(), scItem.GetHandle(), stIndex, uiProgram);
  }
  /* -- Shader initialiser helper ------------------------------------------ */
  template<typename ...VarArgs>
    void AddShaderEx(const string &strName, const GLenum eT,
      const char*const cpFormat, const VarArgs &...vaArgs)
  { AddShader(strName, eT, StrFormat(cpFormat, vaArgs...)); }
  /* -- Detach and unlink shaders ------------------------------------------ */
  void DeInitShaders(void)
  { // Ignore if nothing in list
    if(empty()) return;
    // Until shader list is empty
    for(const ShaderCell &scItem : *this)
    { // Ignore if no shader
      if(!scItem.GetHandle()) continue;
      // If we have a program. Detach the shader if we have the program
      if(uiProgram)
        GLL(cOgl->DetachShader(uiProgram, scItem.GetHandle()),
          "Shader $ not detached from program $!",
          uiProgram, scItem.GetHandle());
      // Delete the shader
      GLL(cOgl->DeleteShader(scItem.GetHandle()),
        "Shader $ could not be deleted!", scItem.GetHandle());
    } // Done if no program
    if(!uiProgram) return;
    // Deselect program
    Deactivate();
    // Delete the program
    GLL(cOgl->DeleteProgram(uiProgram),
      "Shader program $ could not be deleted!", uiProgram);
  }
  /* -- Shader deinitialiser ----------------------------------------------- */
  void DeInit(void)
  { // Detatch, unlink and clear shaders list and program
    DeInitShaders();
    // Reset the identifier
    uiProgram = 0;
    // No longer linked
    bLinked = false;
    // Clear shader list
    clear();
  }
  /* -- Constructor -------------------------------------------------------- */
  Shader(void) :                       // No parameters
    /* -- Initialisers ----------------------------------------------------- */
    ICHelperShader{ cShaders, this },  // Register in Shaders list
    IdentCSlave{ cParent->CtrNext() }, // Initialise identification number
    uiProgram(0),                      // No program set
    bLinked(false)                     // Not linked
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Destructor --------------------------------------------------------- */
  ~Shader(void) { DeInitShaders(); }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(Shader)              // Suppress default functions for safety
};/* ----------------------------------------------------------------------- */
CTOR_END_NOINITS(Shaders, Shader, SHADER) // Finish shaders collector
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
