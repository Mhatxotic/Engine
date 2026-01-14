/* == FBODEF.HPP =========================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## Definitions for the operatibility of framebuffer objects.           ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IFboDef {                    // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace ICoord::P;             using namespace IStd::P;
using namespace IUtil::P;              using namespace Lib::OS::GlFW::Types;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* -- Defines -------------------------------------------------------------- */
constexpr static const size_t
  /* -- Defines to describe a simple triangle  ----------------------------- */
  stVertexPerTriangle = 3,             // Vertices used in a triangle (3 ofc!)
  stTwoTriangles      = stVertexPerTriangle * 2,
  stTrisPerQuad       = 2,             // Triangles needed to make a quad
  /* -- Defines for triangle texture co-ordinates data --------------------- */
  // [0]X: The left 2D pixel co-ordinate. Shader converts it to 3D for us.
  // [1]Y: The top 2D pixel co-ordinate. Shader converts it to 3D for us.
  // [2]Z: Not being used so that co-ordinate is ignored and assumed 0.0f.
  // [3]W: Not being used so that co-ordinate is ignored and assumed 1.0f.
  stCompsPerCoord  = 2,                // Floats used to define texcoord (XY)
  stFloatsPerCoord = stVertexPerTriangle * stCompsPerCoord,
  // Note: If we need to increase this value in the future, it is important
  // that the arrays in 'FboItem.hpp' and 'Fbo.hpp' are updated to reflect the
  // change.
  /* -- Defines for triangle position co-ordinates data--------------------- */
  // [0]X: The left 2D pixel co-ordinate. Shader converts it to 3D for us.
  // [1]Y: The top 2D pixel co-ordinate. Shader converts it to 3D for us.
  // [2]Z: Not being used so that co-ordinate is ignored and assumed 0.0f.
  // [3]W: Not being used so that co-ordinate is ignored and assumed 0.0f.
  stCompsPerPos  = 2,                  // Floats used to define position (XY)
  stFloatsPerPos = stVertexPerTriangle * stCompsPerPos,
  // Note: If we need to increase this value in the future, it is important
  // that the arrays in 'FboItem.hpp', 'Font.hpp', 'Texture.hpp" and 'Fbo.hpp'
  // are updated to reflect the change.
  /* -- Defines for colour intensity data ---------------------------------- */
  // [0]R: The RED intensity of the vertice.
  // [1]G: The GREEN intensity of the vertice.
  // [2]B: The BLUE intensity of the vertice.
  // [3]A: The ALPHA intensity of the vertice.
  stCompsPerColour  = 4,               // Floats used to define colour (RGBA)
  stFloatsPerColour = stVertexPerTriangle * stCompsPerColour,
  /* -- Totals ------------------------------------------------------------- */
  stFloatsPerTri  = stFloatsPerCoord + stFloatsPerPos + stFloatsPerColour,
  stFloatsPerQuad = stFloatsPerTri * stTrisPerQuad,
  /* -- OpenGL buffer structure -------------------------------------------- */
  stSizeOfGLfloat   = sizeof(GLfloat),
  stFloatsPerVertex = stCompsPerCoord + stCompsPerPos + stCompsPerColour,
  stBytesPerVertex  = stFloatsPerVertex * stSizeOfGLfloat,
  stOffsetTxcData   = 0,
  stOffsetPosData   = stCompsPerCoord * stSizeOfGLfloat,
  stOffsetColData   = (stCompsPerCoord + stCompsPerPos) * stSizeOfGLfloat;
/* -- Render command item -------------------------------------------------- */
struct FboCmd                          // Render command structure
{ /* -- Public variables --------------------------------------------------- */
  const GLuint       uiTUId,           // - Texture unit id
                     uiTexId,          // - Texture id
                     uiPrgId;          // - Shader program id
  const GLvoid*const vpTCOffset,       // - Texcoord buffer offset
              *const vpVOffset,        // - vector buffer offset
              *const vpCOffset;        // - Colour buffer offset
  const GLsizei      uiVertices;       // Total vertices to draw
};/* -- Commands ----------------------------------------------------------- */
typedef vector<FboCmd>            FboCmdVec;         // Render command list
typedef FboCmdVec::const_iterator FboCmdVecConstInt; // " const iterator
/* -- Single point data ---------------------------------------------------- */
typedef array<GLfloat,stCompsPerCoord>  TriCoord;  // Coord data in triangle
typedef array<GLfloat,stCompsPerPos>    TriVertex; // Position in triangle
typedef array<GLfloat,stCompsPerColour> TriColour; // Colour in triangle
/* -- One triangle data ---------------------------------------------------- */
struct FboVert                         // Formatted data for OpenGL
{ /* -- Public variables --------------------------------------------------- */
  TriCoord       faCoord;              // TexCoord specific data send
  TriVertex      faVertex;             // Vertex specific data to send
  TriColour      faColour;             // Colour specific data to send
};/* ----------------------------------------------------------------------- */
/* FboVert[0].TriCoord  =  8 bytes @ GLfloat[ 0] - Point 1 / Texcoord 1      **
**     "     .TriVertex =  8 bytes @ GLfloat[ 8] -    "    / Vertex 1        **
**     "     .TriColour = 16 bytes @ GLfloat[16] -    "    / Colour 1        **
** FboVert[1].TriCoor   =  8 bytes @ GLfloat[32] - Point 2 / Texcoord 2      **
**     "     .TriVertex =  8 bytes @ GLfloat[40] -    "    / Vertex 2        **
**     "     .TriColour = 16 bytes @ GLfloat[48] -    "    / Colour 2        **
** FboVert[2].TriCoord  =  8 bytes @ GLfloat[64] - Point 3 / Texcoord 3      **
**     "     .TriVertex =  8 bytes @ GLfloat[72] -    "    / Vertex 3        **
**     "     .TriColour = 16 bytes @ GLfloat[80] -    "    / Colour 3        **
** +-- Single interlaced triangle --+- T(Vec2)=Texcoord(XY) -+-----+-----+-- **
** + TTVVCCCC | TTVVCCCC | TTVVCCCC |  V(Vec2)=Vertex(XY)    | ... | ... |   **
** +----------+----------+----------+- C(Vec4)=Colour(RGBA) -+-----+-----+-- */
typedef array<FboVert,stVertexPerTriangle> FboTri; // All triangles data
typedef vector<FboTri>                 FboTriVec;  // Render triangles list
/* == Fbo colour class ===================================================== */
class FboColour                        // Members initially private
{ /* -- Private typedefs --------------------------------------------------- */
  typedef array<GLfloat,4> FboRGBA;    // Array of RGBA floats
  FboRGBA           fboRGBA;           // Red + Green + Blue + Alpha values
  /* -- Get colour array --------------------------------------------------- */
  const FboRGBA &GetColourConst() const { return fboRGBA; }
  FboRGBA &GetColour() { return fboRGBA; }
  /* -- Get individual colour floats ------------------------------- */ public:
  GLfloat GetColourRed() const { return GetColourConst()[0]; }
  GLfloat GetColourGreen() const { return GetColourConst()[1]; }
  GLfloat GetColourBlue() const { return GetColourConst()[2]; }
  GLfloat GetColourAlpha() const { return GetColourConst()[3]; }
  GLfloat *GetColourMemory() { return GetColour().data(); }
  /* -- Set individual colour floats --------------------------------------- */
  void SetColourRed(const GLfloat fR) { GetColour()[0] = fR; }
  void SetColourGreen(const GLfloat fG) { GetColour()[1] = fG; }
  void SetColourBlue(const GLfloat fB) { GetColour()[2] = fB; }
  void SetColourAlpha(const GLfloat fA) { GetColour()[3] = fA; }
  /* -- Set individual colours as integers (0-255) ------------------------- */
  void SetColourRedInt(const unsigned int uiR)
    { SetColourRed(UtilNormaliseEx<GLfloat>(uiR)); }
  void SetColourGreenInt(const unsigned int uiG)
    { SetColourGreen(UtilNormaliseEx<GLfloat>(uiG)); }
  void SetColourBlueInt(const unsigned int uiB)
    { SetColourBlue(UtilNormaliseEx<GLfloat>(uiB)); }
  void SetColourAlphaInt(const unsigned int uiA)
    { SetColourAlpha(UtilNormaliseEx<GLfloat>(uiA)); }
  /* -- Set colours by packed colours integer ------------------------------ */
  void SetColourInt(const unsigned int uiValue)
  { // Strip bits and normalise proper clear colour from 0.0 to 1.0
    SetColourRed(UtilNormaliseEx<GLfloat,16>(uiValue));   // 8-bits 16 to 24
    SetColourGreen(UtilNormaliseEx<GLfloat,8>(uiValue));  // 8-bits 08 to 16
    SetColourBlue(UtilNormaliseEx<GLfloat>(uiValue));     // 8-bits 00 to 08
    SetColourAlpha(UtilNormaliseEx<GLfloat,24>(uiValue)); // 8-bits 24 to 32
  }
  /* -- Set colours from another colour structure -------------------------- */
  void SetColourRed(const FboColour &fcValue)
    { SetColourRed(fcValue.GetColourRed()); }
  void SetColourGreen(const FboColour &fcValue)
    { SetColourGreen(fcValue.GetColourGreen()); }
  void SetColourBlue(const FboColour &fcValue)
    { SetColourBlue(fcValue.GetColourBlue()); }
  void SetColourAlpha(const FboColour &fcValue)
    { SetColourAlpha(fcValue.GetColourAlpha()); }
  /* -- De-initialise colours ---------------------------------------------- */
  void ResetColour() { GetColour().fill(-1.0f); }
  /* -- Test colour components --------------------------------------------- */
  bool RedColourNotEqual(const FboColour &fcValue) const
    { return StdIsFloatNotEqual(GetColourRed(), fcValue.GetColourRed()); }
  bool GreenColourNotEqual(const FboColour &fcValue) const
    { return StdIsFloatNotEqual(GetColourGreen(), fcValue.GetColourGreen()); }
  bool BlueColourNotEqual(const FboColour &fcValue) const
    { return StdIsFloatNotEqual(GetColourBlue(), fcValue.GetColourBlue()); }
  bool AlphaColourNotEqual(const FboColour &fcValue) const
    { return StdIsFloatNotEqual(GetColourAlpha(), fcValue.GetColourAlpha()); }
  /* -- Cast to colours of a different type -------------------------------- */
  template<typename IntType,
    class ArrayType=array<IntType, sizeof(FboRGBA) / sizeof(GLfloat)>>
      const ArrayType Cast() const
  { return { UtilDenormalise<IntType>(GetColourRed()),
             UtilDenormalise<IntType>(GetColourGreen()),
             UtilDenormalise<IntType>(GetColourBlue()),
             UtilDenormalise<IntType>(GetColourAlpha()) }; }
  /* -- Set colour from normalised values ---------------------------------- */
  void SetColour(const GLfloat fR, const GLfloat fG, const GLfloat fB,
    const GLfloat fA) { SetColourRed(fR); SetColourGreen(fG);
                        SetColourBlue(fB); SetColourAlpha(fA); }
  /* -- Set colour from another colour struct ------------------------------ */
  bool SetColour(const FboColour &fcValue)
  { // Red clear colour change?
    if(RedColourNotEqual(fcValue))
    { // Update saved red value and other values if changed
      SetColourRed(fcValue);
      if(GreenColourNotEqual(fcValue)) SetColourGreen(fcValue);
      if(BlueColourNotEqual(fcValue)) SetColourBlue(fcValue);
      if(AlphaColourNotEqual(fcValue)) SetColourAlpha(fcValue);
    } // Green clear colour changed?
    else if(GreenColourNotEqual(fcValue))
    { // Update saved green value and other values if changed
      SetColourGreen(fcValue);
      if(BlueColourNotEqual(fcValue)) SetColourBlue(fcValue);
      if(AlphaColourNotEqual(fcValue)) SetColourAlpha(fcValue);
    } // Blue clear colour changed?
    else if(BlueColourNotEqual(fcValue))
    { // Update saved blue value and other values if changed
      SetColourBlue(fcValue);
      if(AlphaColourNotEqual(fcValue)) SetColourAlpha(fcValue);
    } // Update alpha if cahnged
    else if(AlphaColourNotEqual(fcValue)) SetColourAlpha(fcValue);
    // No value was changed so return
    else return false;
    // Commit the new viewport
    return true;
  }
  /* -- Init constructor with RGBA ints ------------------------------------ */
  FboColour(const unsigned int uiR, const unsigned int uiG,
            const unsigned int uiB, const unsigned int uiA) :
    /* -- Initialisers ----------------------------------------------------- */
    fboRGBA{UtilNormaliseEx<GLfloat>(uiR), // Copy/normalise red component
            UtilNormaliseEx<GLfloat>(uiG), // Copy/normalise green component
            UtilNormaliseEx<GLfloat>(uiB), // Copy/normalise blue component
            UtilNormaliseEx<GLfloat>(uiA)} // Copy/normalise alpha component
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- Init constructor with RGB ints ------------------------------------- */
  FboColour(const unsigned int uiR, const unsigned int uiG,
            const unsigned int uiB) :
    /* -- Initialisers ----------------------------------------------------- */
    fboRGBA{UtilNormaliseEx<GLfloat>(uiR), // Copy/normalise red component
            UtilNormaliseEx<GLfloat>(uiG), // Copy/normalise green component
            UtilNormaliseEx<GLfloat>(uiB), // Copy/normalise blue component
            1.0f }                     // Opaque alpha
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- Init constructor with RGBA bytes ----------------------------------- */
  FboColour(const uint8_t ucR, const uint8_t ucG,
            const uint8_t ucB, const uint8_t ucA) :
    /* -- Initialisers ----------------------------------------------------- */
    fboRGBA{UtilNormalise<GLfloat>(ucR),   // Copy/normalise red component
            UtilNormalise<GLfloat>(ucG),   // Copy/normalise green component
            UtilNormalise<GLfloat>(ucB),   // Copy/normalise blue component
            UtilNormalise<GLfloat>(ucA)}   // Copy/normalise alpha component
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- Init constructor --------------------------------------------------- */
  FboColour(const GLfloat fR, const GLfloat fG, const GLfloat fB,
    const GLfloat fA) :
    /* -- Initialisers ----------------------------------------------------- */
    fboRGBA{ fR, fG, fB, fA }          // Copy intensity over
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- Default constructor ------------------------------------------------ */
  FboColour() :
    /* -- Initialisers ----------------------------------------------------- */
    fboRGBA{ 1.0f, 1.0f, 1.0f, 1.0f }  // Set full Re/Gr/Bl/Alpha intensity
    /* -- No code ---------------------------------------------------------- */
    {}
};/* ----------------------------------------------------------------------- */
/* == Fbo blend class ====================================================== */
class FboBlend
{ /* -- Private variables -------------------------------------------------- */
  typedef array<GLenum,4> FboBlendStruct; // Array of four GLenum's
  FboBlendStruct   fbsBlend;             // Blend union
  /* -- Get blend values ------------------------------------------- */ public:
  GLenum GetSrcRGB() const { return fbsBlend[0]; }
  GLenum GetDstRGB() const { return fbsBlend[1]; }
  GLenum GetSrcAlpha() const { return fbsBlend[2]; }
  GLenum GetDstAlpha() const { return fbsBlend[3]; }
  GLenum *GetMemory() { return fbsBlend.data(); }
  /* -- Set blend values by enum ------------------------------------------- */
  void SetSrcRGB(const GLenum eSrcRGB) { fbsBlend[0] = eSrcRGB; }
  void SetDstRGB(const GLenum eDstRGB) { fbsBlend[1] = eDstRGB; }
  void SetSrcAlpha(const GLenum eSrcAlpha) { fbsBlend[2] = eSrcAlpha; }
  void SetDstAlpha(const GLenum eDstAlpha) { fbsBlend[3] = eDstAlpha; }
  /* -- Set blend values by another struct --------------------------------- */
  void SetSrcRGB(const FboBlend &fbValue) { SetSrcRGB(fbValue.GetSrcRGB()); }
  void SetDstRGB(const FboBlend &fbValue) { SetDstRGB(fbValue.GetDstRGB()); }
  void SetSrcAlpha(const FboBlend &fbValue)
    { SetSrcAlpha(fbValue.GetSrcAlpha()); }
  void SetDstAlpha(const FboBlend &fbValue)
    { SetDstAlpha(fbValue.GetDstAlpha()); }
  /* -- Test blend values -------------------------------------------------- */
  bool IsSrcRGBNotEqual(const FboBlend &fbValue) const
    { return GetSrcRGB() != fbValue.GetSrcRGB(); }
  bool IsDstRGBNotEqual(const FboBlend &fbValue) const
    { return GetDstRGB() != fbValue.GetDstRGB(); }
  bool IsSrcAlphaNotEqual(const FboBlend &fbValue) const
    { return GetSrcAlpha() != fbValue.GetSrcAlpha(); }
  bool IsDstAlphaNotEqual(const FboBlend &fbValue) const
    { return GetDstAlpha() != fbValue.GetDstAlpha(); }
  /* -- Set blending algorithms -------------------------------------------- */
  bool SetBlend(const FboBlend &fbValue)
  {  // Source RGB changed change?
    if(IsSrcRGBNotEqual(fbValue))
    { // Update source RGB blend value and other values if changed
      SetSrcRGB(fbValue);
      if(IsDstRGBNotEqual(fbValue)) SetDstRGB(fbValue);
      if(IsSrcAlphaNotEqual(fbValue)) SetSrcAlpha(fbValue);
      if(IsDstAlphaNotEqual(fbValue)) SetDstAlpha(fbValue);
    } // Destination RGB blend changed?
    else if(IsDstRGBNotEqual(fbValue))
    { // Update destination RGB blend value and other values if changed
      SetDstRGB(fbValue);
      if(IsSrcAlphaNotEqual(fbValue)) SetSrcAlpha(fbValue);
      if(IsDstAlphaNotEqual(fbValue)) SetDstAlpha(fbValue);
    } // Source alpha changed?
    else if(IsSrcAlphaNotEqual(fbValue))
    { // Update source alpha blend value and other values if changed
      SetSrcAlpha(fbValue);
      if(IsDstAlphaNotEqual(fbValue)) SetDstAlpha(fbValue);
    } // Destination alpha changed?
    else if(IsDstAlphaNotEqual(fbValue)) SetDstAlpha(fbValue);
    // No value was changed so return
    else return false;
    // Commit the new viewport
    return true;
  }
  /* -- Init constructor --------------------------------------------------- */
  FboBlend(const GLenum eSrcRGB, const GLenum eDstRGB, const GLenum eSrcAlpha,
    const GLenum eDstAlpha):
    /* -- Initialisers ----------------------------------------------------- */
    fbsBlend{ eSrcRGB, eDstRGB,        // Copy blend source and dest RGB
              eSrcAlpha, eDstAlpha }   // Copy blend source and dest Alpha
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- Default constructor ------------------------------------------------ */
  FboBlend() :
    /* -- Initialisers ----------------------------------------------------- */
    FboBlend{ GL_SRC_ALPHA,            // Init blend source RGB
              GL_ONE_MINUS_SRC_ALPHA,  // Init blend destination RGB
              GL_ONE,                  // Init blend source Alpha
              GL_ONE_MINUS_SRC_ALPHA } // Init blend destination Alpha
    /* -- No code ---------------------------------------------------------- */
    {}
};/* ----------------------------------------------------------------------- */
/* == Fbo coords class ===================================================== */
template<typename Type1 = GLfloat, typename Type2 = Type1>class FboCoords
{ /* -- Private typedefs --------------------------------------------------- */
  typedef Coordinates<Type1> CoordType1; // Co-ordinate type one
  typedef Coordinates<Type2> CoordType2; // Co-ordinate type two
  /* -- Private variables -------------------------------------------------- */
  CoordType1       ctXY1;              // Top left co-ordinates
  CoordType2       ctXY2;              // Width and height
  /* -- Get co-ordinate -------------------------------------------- */ public:
  Type1 GetCoLeft() const { return ctXY1.CoordGetX(); }
  Type1 GetCoTop() const { return ctXY1.CoordGetY(); }
  Type2 GetCoRight() const { return ctXY2.CoordGetX(); }
  Type2 GetCoBottom() const { return ctXY2.CoordGetY(); }
  /* -- Set co-ordinate by integral values --------------------------------- */
  void SetCoLeft(const Type1 tNLeft) { ctXY1.CoordSetX(tNLeft); }
  void SetCoTop(const Type1 tNTop) { ctXY1.CoordSetY(tNTop); }
  void SetCoRight(const Type2 tNRight) { ctXY2.CoordSetX(tNRight); }
  void SetCoBottom(const Type2 tNBottom) { ctXY2.CoordSetY(tNBottom);}
  /* -- Set co-ordinate by another struct ---------------------------------- */
  void SetCoLeft(const FboCoords &fcValue) { SetCoLeft(fcValue.GetCoLeft()); }
  void SetCoTop(const FboCoords &fcValue) { SetCoTop(fcValue.GetCoTop()); }
  void SetCoRight(const FboCoords &fcValue)
    { SetCoRight(fcValue.GetCoRight()); }
  void SetCoBottom(const FboCoords &fcValue)
    { SetCoBottom(fcValue.GetCoBottom()); }
  /* -- Reset co-ordinates ------------------------------------------------- */
  void ResetCoords() { ctXY1.CoordSet(); ctXY2.CoordSet(); }
  /* -- Set all co-ordinates by integral values ---------------------------- */
  void SetCoords(const Type1 tNLeft, const Type1 tNTop, const Type2 tNRight,
    const Type2 tNBottom)
  { SetCoLeft(tNLeft); SetCoTop(tNTop); SetCoRight(tNRight);
    SetCoBottom(tNBottom); }
  /* -- Set all co-ordinates by another struct ----------------------------- */
  void SetCoords(const FboCoords &fcValue)
    { SetCoords(fcValue.GetCoLeft(), fcValue.GetCoTop(),
                fcValue.GetCoRight(), fcValue.GetCoBottom()); }
  /* -- Init constructor --------------------------------------------------- */
  FboCoords(const Type1 tNLeft, const Type1 tNTop, const Type2 tNRight,
    const Type2 tNBottom) :            // X1, Y1, X2, Y2
    /* -- Initialisers ----------------------------------------------------- */
    ctXY1{ tNLeft, tNTop },            // X1 & Y1 co-ordinate
    ctXY2{ tNRight, tNBottom }         // X2 & Y2 co-ordinate
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- Default constructor ------------------------------------------------ */
  FboCoords() :
    /* -- Initialisers ----------------------------------------------------- */
    FboCoords{ 0, 0, 0, 0 }            // Initialise co-ordinates
    /* -- No code ---------------------------------------------------------- */
    {}
};/* ----------------------------------------------------------------------- */
typedef FboCoords<GLfloat> FboFloatCoords; // Coords made of floats
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
