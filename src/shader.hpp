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
using namespace ICommon::P;            using namespace ICollector::P;
using namespace IError::P;             using namespace ICoords::P;
using namespace ILockable::P;          using namespace ILog::P;
using namespace ILuaIdent::P;          using namespace ILuaLib::P;
using namespace IName::P;              using namespace IOgl::P;
using namespace ISerial::P;            using namespace IShaderDef::P;
using namespace IStd::P;               using namespace IString::P;
using namespace ISysUtil::P;           using namespace Lib::OS::GlFW::Types;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* -- Shader list class ---------------------------------------------------- */
class ShaderCell :                     // Members initially private
  /* -- Initialisers ------------------------------------------------------- */
  public NameStr                        // Name of string
{ /* -- Private variables -------------------------------------------------- */
  const StdString  strCode;            // Shader name and code
  const GLenum     eType;              // Shader type
  const GLuint     uShader;            // Created shader id
  /* -- Return code of shader -------------------------------------- */ public:
  const StdString &GetCode() const { return strCode; }
  /* -- Return length of shader code --------------------------------------- */
  size_t GetCodeLength() const { return GetCode().size(); }
  /* -- Return name of shader as C-String ---------------------------------- */
  const char *GetCodeCStr() const { return GetCode().data(); }
  /* -- Return type of shader ---------------------------------------------- */
  GLenum GetType() const { return eType; }
  /* -- Return shader id --------------------------------------------------- */
  GLuint GetHandle() const { return uShader; }
  /* -- Default constructor ------------------------------------------------ */
  ShaderCell(const StdString &strNName, // Specified new identifier
             const StdString &strNCode, // Specified code to copmile
             const GLenum eNType,       // Specified GL type id of code
             const GLuint uNShader) :   // Specified GL shader id of code
    /* -- Initialisers ----------------------------------------------------- */
    Name{ strNName },                  // Set specified name
    strCode{ strNCode },               // Set code
    eType{ eNType },                   // Set type of shader
    uShader{ uNShader }                // Set shader handle
    /* -- No code ---------------------------------------------------------- */
    {}
};/* ----------------------------------------------------------------------- */
using ShaderList = StdList<ShaderCell>; // Shader cell list
/* ------------------------------------------------------------------------- */
CTOR_BEGIN_DUO(Shaders, Shader, CLHelperUnsafe, ICHelperUnsafe),
  /* -- Base classes ------------------------------------------------------- */
  public Lockable,                     // Lua garbage collector instruction
  public ShaderList                    // List of shader data in this program
{ /* -- Private typedefs --------------------------------------------------- */
  using UniList = StdArray<GLint, U_MAX>; // Uniform array list
  /* -- Private variables -------------------------------------------------- */
  GLuint           uProgram;           // Shader program id
  UniList          aUniforms;          // Ids of mandatory uniforms we need
  bool             bLinked;            // Shader program has been linked
  /* -- Get program id number -------------------------------------- */ public:
  GLuint GetProgramId() const { return uProgram; }
  /* -- SHader is linked? -------------------------------------------------- */
  bool IsLinked() const { return bLinked; }
  /* -- Verify the specified attribute is at the specified location -------- */
  void VerifyAttribLocation(const char *cpAttr, const ShaderAttributeId saiId)
  { // Verify that the id is valid (impossible but just incase)
    if(saiId >= cOgl->MaxVertexAttribs())
      XC("Invalid shader attribute location index!",
        "Attrib",  cpAttr, "Program", uProgram, "Index", saiId,
        "Maximum", cOgl->MaxVertexAttribs(),     "MaximumEngine", A_MAX);
    // Get attribute location
    GL(cOgl->BindAttribLocation(uProgram, saiId, cpAttr),
      "Failed to get attribute location from shader!",
      "Attrib", cpAttr, "Program", uProgram, "Index", saiId);
    // Enable the vertex attrib array. Keep an eye on this if you have problems
    // with glVertexAttribPointer. You'll have to restore Enable/Disable vertex
    // attrib pointers before both glDrawArrays calls if you add more shaders
    // and this call fails. Note that this is VAO depedent so whee this is
    // called, we should already be in the global VAO.
    GL(cOgl->EnableVertexAttribArray(saiId),
      "Failed to enable vertex attrib array!",
      "Attrib", cpAttr, "Program", uProgram, "Index", saiId);
    // Report location in log
    cLog->LogDebugExSafe("Shader bound attribute '$' at location $.",
      cpAttr, saiId);
  }
  /* -- Verify the specified uniform is at the specified location ---------- */
  void VerifyUniformLocation(const char *cpUni, const ShaderUniformId suiId)
  { // Get attribute location
    GL(aUniforms[suiId] = GetUniformLocation(cpUni),
      "Failed to get uniform location from shader!",
      "Uniform", cpUni, "Program", uProgram, "Assign", suiId);
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
  void UpdateMatrix(const CoordsGLFloat &cglfRef) const
  { // Activate shader (no error checking)
    cOgl->UseProgram(GetProgram());
    // Commit matrix bounds (no error checking)
    cOgl->Uniform(GetUID(U_MATRIX), cglfRef.CoordsGetX1(),
      cglfRef.CoordsGetY1(), cglfRef.CoordsGetX2(), cglfRef.CoordsGetY2());
  }
  /* -- Linkage ------------------------------------------------------------ */
  void Link()
  { // We want to make things simple and make sure every shader has a specific
    // attribute at a specific index so make sure these are in the correct
    // positions. This has to be done before the link.
    VerifyAttribLocation("texcoord", A_COORD);
    VerifyAttribLocation("vertex", A_VERTEX);
    VerifyAttribLocation("colour", A_COLOUR);
    // Do the link
    GL(cOgl->LinkProgram(uProgram), "Link shader program failed!",
      "Program", uProgram);
    // Get link result and show reason if failed
    if(cOgl->GetLinkStatus(uProgram) == GL_FALSE)
      XC("Shader program linkage failed!", "Program", uProgram,
        "Reason", cOgl->GetLinkFailureReason(uProgram));
    // Program linked
    bLinked = true;
    // Activate
    Activate();
    // Verify uniforms are in the correct position
    VerifyUniformLocation("matrix", U_MATRIX);
  }
  /* -- Activate program --------------------------------------------------- */
  void Activate()
  { // Do activate the program
    GL(cOgl->UseProgram(uProgram), "Failed to select shader program!",
      "Program", uProgram);
  }
  /* -- Deselect program --------------------------------------------------- */
  void Deactivate()
    { if(cOgl->GetProgram() == uProgram)
        GLL(cOgl->UseProgram(0), "Failed to deselect active program!",
          "Program", uProgram); }
  /* -- Get shader program name -------------------------------------------- */
  GLuint GetProgram() const { return uProgram; }
  /* -- Uniform value ------------------------------------------------------ */
  GLint GetUID(const ShaderUniformId suiId) const { return aUniforms[suiId]; }
  /* -- Variable location -------------------------------------------------- */
  GLint GetUniformLocation(const char*const cpVar) const
    { return cOgl->GetUniformLocation(uProgram, cpVar); }
  /* -- Shader initialiser ------------------------------------------------- */
  void AddShader(const StdString &strName, const GLenum eT,
    const StdString &strC)
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
        "Name", scItem.NameGet(), "Type", scItem.GetType());
    // Set shader source code
    GL(cOgl->ShaderSource(scItem.GetHandle(), scItem.GetCodeCStr()),
      "Failed to set shader source code!",
      "Name",   scItem.NameGet(),   "Type",   scItem.GetType(),
      "Shader", scItem.GetHandle(), "Source", scItem.GetCode());
    // Compile the shader source code
    GL(cOgl->CompileShader(scItem.GetHandle()),
      "Failed to compile shader source code!",
      "Type", eT, "Shader", scItem.GetHandle(), "Source", scItem.GetCode());
    // Get compiler result and show reason if failed
    if(cOgl->GetCompileStatus(scItem.GetHandle()) == GL_FALSE)
      XC("Shader compilation failed!",
        "Name",   scItem.NameGet(), "Program", uProgram,
        "Type",   scItem.GetType(), "Shader",  scItem.GetHandle(),
        "Reason", cOgl->GetCompileFailureReason(scItem.GetHandle()));
    // If the program hasn't been setup yet?
    if(!uProgram)
    { // Create the shader program, and if failed? We should bail out.
      uProgram = cOgl->CreateProgram();
      if(!uProgram)
        XC("Failed to create shader program!",
          "Name",   scItem.NameGet(), "Type", scItem.GetType(),
          "Shader", scItem.GetHandle());
    } // Attach the shader to the program
    GL(cOgl->AttachShader(uProgram, scItem.GetHandle()),
      "Failed to attach shader to program!",
      "Name", scItem.NameGet(), "Program", uProgram,
      "Type", scItem.GetType(), "Shader",  scItem.GetHandle());
    // Shader compiled
    cLog->LogDebugExSafe("Shader '$'($) compiled at index $ on program $.",
      scItem.NameGet(), scItem.GetHandle(), stIndex, uProgram);
  }
  /* -- Shader initialiser helper ------------------------------------------ */
  template<typename StrType, typename ...VarArgs>
    void AddShaderEx(const StdString &strName, const GLenum eT,
      StrType &&strFormat, VarArgs &&...vaArgs)
  { AddShader(strName, eT, StrFormat(StdForward<StrType>(strFormat),
      StdForward<VarArgs>(vaArgs)...)); }
  /* -- Detach and unlink shaders ------------------------------------------ */
  void DeInitShaders()
  { // Ignore if nothing in list
    if(empty()) return;
    // Until shader list is empty
    for(const ShaderCell &scItem : *this)
    { // Ignore if no shader
      if(!scItem.GetHandle()) continue;
      // If we have a program. Detach the shader if we have the program
      if(uProgram)
        GLL(cOgl->DetachShader(uProgram, scItem.GetHandle()),
          "Shader $ not detached from program $!",
          uProgram, scItem.GetHandle());
      // Delete the shader
      GLL(cOgl->DeleteShader(scItem.GetHandle()),
        "Shader $ could not be deleted!", scItem.GetHandle());
    } // Done if no program
    if(!uProgram) return;
    // Deselect program
    Deactivate();
    // Delete the program
    GLL(cOgl->DeleteProgram(uProgram),
      "Shader program $ could not be deleted!", uProgram);
  }
  /* -- Shader deinitialiser ----------------------------------------------- */
  void DeInit()
  { // Detatch, unlink and clear shaders list and program
    DeInitShaders();
    // Reset the identifier
    uProgram = 0;
    // No longer linked
    bLinked = false;
    // Clear shader list
    clear();
  }
  /* -- Constructor -------------------------------------------------------- */
  Shader() :
    /* -- Initialisers ----------------------------------------------------- */
    ICHelperShader{ cShaders, this },  // Register in Shaders list
    SerialSlave{ cParent->Serial() },  // Initialise identification number
    uProgram(0),                       // No program set
    bLinked(false)                     // Not linked
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- Destructor --------------------------------------------------------- */
  DTORHELPER(~Shader, DeInitShaders())
};/* ----------------------------------------------------------------------- */
CTOR_END_NOINITS(Shaders, Shader, SHADER) // Finish shaders collector
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
