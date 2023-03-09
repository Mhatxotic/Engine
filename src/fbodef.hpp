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
using namespace IDim;                  using namespace IUtil::P;
using namespace Lib::OS::GlFW;
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
{ /* ----------------------------------------------------------------------- */
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
{ /* ----------------------------------------------------------------------- */
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
  /* ----------------------------------------------------------------------- */
  const FboRGBA &GetColourConst(void) const { return fboRGBA; }
  FboRGBA &GetColour(void) { return fboRGBA; }
  /* --------------------------------------------------------------- */ public:
  GLfloat GetColourRed(void) const { return GetColourConst()[0]; }
  GLfloat GetColourGreen(void) const { return GetColourConst()[1]; }
  GLfloat GetColourBlue(void) const { return GetColourConst()[2]; }
  GLfloat GetColourAlpha(void) const { return GetColourConst()[3]; }
  GLfloat *GetColourMemory(void) { return GetColour().data(); }
  /* ----------------------------------------------------------------------- */
  void SetColourRed(const GLfloat fR) { GetColour()[0] = fR; }
  void SetColourGreen(const GLfloat fG) { GetColour()[1] = fG; }
  void SetColourBlue(const GLfloat fB) { GetColour()[2] = fB; }
  void SetColourAlpha(const GLfloat fA) { GetColour()[3] = fA; }
  /* ----------------------------------------------------------------------- */
  void SetColourRedInt(const unsigned int uiR)
    { SetColourRed(UtilNormaliseEx<GLfloat>(uiR)); }
  void SetColourGreenInt(const unsigned int uiG)
    { SetColourGreen(UtilNormaliseEx<GLfloat>(uiG)); }
  void SetColourBlueInt(const unsigned int uiB)
    { SetColourBlue(UtilNormaliseEx<GLfloat>(uiB)); }
  void SetColourAlphaInt(const unsigned int uiA)
    { SetColourAlpha(UtilNormaliseEx<GLfloat>(uiA)); }
  /* ----------------------------------------------------------------------- */
  void SetColourInt(const unsigned int uiValue)
  { // Strip bits and send to proper clear colour
    SetColourRed(UtilNormaliseEx<GLfloat,16>(uiValue));
    SetColourGreen(UtilNormaliseEx<GLfloat,8>(uiValue));
    SetColourBlue(UtilNormaliseEx<GLfloat>(uiValue));
    SetColourAlpha(UtilNormaliseEx<GLfloat,24>(uiValue));
  }
  /* ----------------------------------------------------------------------- */
  void SetColourRed(const FboColour &fcValue)
    { SetColourRed(fcValue.GetColourRed()); }
  void SetColourGreen(const FboColour &fcValue)
    { SetColourGreen(fcValue.GetColourGreen()); }
  void SetColourBlue(const FboColour &fcValue)
    { SetColourBlue(fcValue.GetColourBlue()); }
  void SetColourAlpha(const FboColour &fcValue)
    { SetColourAlpha(fcValue.GetColourAlpha()); }
  /* ----------------------------------------------------------------------- */
  void ResetColour(void) { GetColour().fill(-1.0f); }
  /* ----------------------------------------------------------------------- */
  bool RedColourNotEqual(const FboColour &fcValue) const
    { return UtilIsFloatNotEqual(GetColourRed(), fcValue.GetColourRed()); }
  bool GreenColourNotEqual(const FboColour &fcValue) const
    { return UtilIsFloatNotEqual(GetColourGreen(), fcValue.GetColourGreen()); }
  bool BlueColourNotEqual(const FboColour &fcValue) const
    { return UtilIsFloatNotEqual(GetColourBlue(), fcValue.GetColourBlue()); }
  bool AlphaColourNotEqual(const FboColour &fcValue) const
    { return UtilIsFloatNotEqual(GetColourAlpha(), fcValue.GetColourAlpha()); }
  /* ----------------------------------------------------------------------- */
  template<typename IntType,
    class ArrayType=array<IntType, sizeof(FboRGBA)/sizeof(GLfloat)>>
      const ArrayType Cast(void) const
  { return { UtilDenormalise<IntType>(GetColourRed()),
             UtilDenormalise<IntType>(GetColourGreen()),
             UtilDenormalise<IntType>(GetColourBlue()),
             UtilDenormalise<IntType>(GetColourAlpha()) }; }
  /* ----------------------------------------------------------------------- */
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
    { }
  /* -- Init constructor with RGB ints ------------------------------------- */
  FboColour(const unsigned int uiR, const unsigned int uiG,
            const unsigned int uiB) :
    /* -- Initialisers ----------------------------------------------------- */
    fboRGBA{UtilNormaliseEx<GLfloat>(uiR), // Copy/normalise red component
            UtilNormaliseEx<GLfloat>(uiG), // Copy/normalise green component
            UtilNormaliseEx<GLfloat>(uiB), // Copy/normalise blue component
            1.0f }                     // Opaque alpha
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Init constructor with RGBA bytes ----------------------------------- */
  FboColour(const uint8_t ucR, const uint8_t ucG,
            const uint8_t ucB, const uint8_t ucA) :
    /* -- Initialisers ----------------------------------------------------- */
    fboRGBA{UtilNormalise<GLfloat>(ucR),   // Copy/normalise red component
            UtilNormalise<GLfloat>(ucG),   // Copy/normalise green component
            UtilNormalise<GLfloat>(ucB),   // Copy/normalise blue component
            UtilNormalise<GLfloat>(ucA)}   // Copy/normalise alpha component
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Init constructor --------------------------------------------------- */
  FboColour(const GLfloat fR, const GLfloat fG, const GLfloat fB,
    const GLfloat fA) :
    /* -- Initialisers ----------------------------------------------------- */
    fboRGBA{ fR, fG, fB, fA }          // Copy intensity over
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Default constructor ------------------------------------------------ */
  FboColour(void) :
    /* -- Initialisers ----------------------------------------------------- */
    fboRGBA{ 1.0f,1.0f,1.0f,1.0f }     // Set full Re/Gr/Bl/Alpha intensity
    /* -- No code ---------------------------------------------------------- */
    { }
};/* ----------------------------------------------------------------------- */
/* == Fbo blend class ====================================================== */
class FboBlend
{ /* -- Private variables ----------------------------------------- */ private:
  array<GLenum,4>  aBlend;             // Blend union
  /* --------------------------------------------------------------- */ public:
  GLenum GetSrcRGB(void) const { return aBlend[0]; }
  GLenum GetDstRGB(void) const { return aBlend[1]; }
  GLenum GetSrcAlpha(void) const { return aBlend[2]; }
  GLenum GetDstAlpha(void) const { return aBlend[3]; }
  GLenum *GetMemory(void) { return aBlend.data(); }
  /* ----------------------------------------------------------------------- */
  void SetSrcRGB(const GLenum eSrcRGB) { aBlend[0] = eSrcRGB; }
  void SetDstRGB(const GLenum eDstRGB) { aBlend[1] = eDstRGB; }
  void SetSrcAlpha(const GLenum eSrcAlpha) { aBlend[2] = eSrcAlpha; }
  void SetDstAlpha(const GLenum eDstAlpha) { aBlend[3] = eDstAlpha; }
  /* ----------------------------------------------------------------------- */
  void SetSrcRGB(const FboBlend &fcValue)
    { SetSrcRGB(fcValue.GetSrcRGB()); }
  void SetDstRGB(const FboBlend &fcValue)
    { SetDstRGB(fcValue.GetDstRGB()); }
  void SetSrcAlpha(const FboBlend &fcValue)
    { SetSrcAlpha(fcValue.GetSrcAlpha()); }
  void SetDstAlpha(const FboBlend &fcValue)
    { SetDstAlpha(fcValue.GetDstAlpha()); }
  /* ----------------------------------------------------------------------- */
  bool IsSrcRGBNotEqual(const FboBlend &fcValue) const
    { return GetSrcRGB() != fcValue.GetSrcRGB(); }
  bool IsDstRGBNotEqual(const FboBlend &fcValue) const
    { return GetDstRGB() != fcValue.GetDstRGB(); }
  bool IsSrcAlphaNotEqual(const FboBlend &fcValue) const
    { return GetSrcAlpha() != fcValue.GetSrcAlpha(); }
  bool IsDstAlphaNotEqual(const FboBlend &fcValue) const
    { return GetDstAlpha() != fcValue.GetDstAlpha(); }
  /* -- Set blending algorithms -------------------------------------------- */
  bool SetBlend(const FboBlend &fcValue)
  {  // Source RGB changed change?
    if(IsSrcRGBNotEqual(fcValue))
    { // Update source RGB blend value and other values if changed
      SetSrcRGB(fcValue);
      if(IsDstRGBNotEqual(fcValue)) SetDstRGB(fcValue);
      if(IsSrcAlphaNotEqual(fcValue)) SetSrcAlpha(fcValue);
      if(IsDstAlphaNotEqual(fcValue)) SetDstAlpha(fcValue);
    } // Destination RGB blend changed?
    else if(IsDstRGBNotEqual(fcValue))
    { // Update destination RGB blend value and other values if changed
      SetDstRGB(fcValue);
      if(IsSrcAlphaNotEqual(fcValue)) SetSrcAlpha(fcValue);
      if(IsDstAlphaNotEqual(fcValue)) SetDstAlpha(fcValue);
    } // Source alpha changed?
    else if(IsSrcAlphaNotEqual(fcValue))
    { // Update source alpha blend value and other values if changed
      SetSrcAlpha(fcValue);
      if(IsDstAlphaNotEqual(fcValue)) SetDstAlpha(fcValue);
    } // Destination alpha changed?
    else if(IsDstAlphaNotEqual(fcValue)) SetDstAlpha(fcValue);
    // No value was changed so return
    else return false;
    // Commit the new viewport
    return true;
  }
  /* -- Init constructor --------------------------------------------------- */
  FboBlend(const GLenum eSrcRGB, const GLenum eDstRGB, const GLenum eSrcAlpha,
    const GLenum eDstAlpha) :
    /* -- Initialisers ----------------------------------------------------- */
    aBlend{ eSrcRGB,                   // Copy blend source RGB
            eDstRGB,                   // Copy blend destination RGB
            eSrcAlpha,                 // Copy blend source Alpha
            eDstAlpha }                // Copy blend destination Alpha
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Default constructor ------------------------------------------------ */
  FboBlend(void) :
    /* -- Initialisers ----------------------------------------------------- */
    FboBlend{ GL_SRC_ALPHA,            // Init blend source RGB
              GL_ONE_MINUS_SRC_ALPHA,  // Init blend destination RGB
              GL_ONE,                  // Init blend source Alpha
              GL_ONE_MINUS_SRC_ALPHA } // Init blend destination Alpha
    /* -- No code ---------------------------------------------------------- */
    { }
};/* ----------------------------------------------------------------------- */
/* == Fbo coords class ===================================================== */
template<typename Type1 = GLfloat, typename Type2 = Type1>class FboCoords
{ /* -- Private variables ----------------------------------------- */ private:
  typedef Coordinates<Type1> CoordType1; // Co-ordinate type one
  typedef Coordinates<Type2> CoordType2; // Co-ordinate type two
  /* ----------------------------------------------------------------------- */
  CoordType1       ctXY1;              // Top left co-ordinates
  CoordType2       ctXY2;              // Width and height
  /* --------------------------------------------------------------- */ public:
  Type1 GetCoLeft(void) const { return ctXY1.CoordGetX(); }
  Type1 GetCoTop(void) const { return ctXY1.CoordGetY(); }
  Type2 GetCoRight(void) const { return ctXY2.CoordGetX(); }
  Type2 GetCoBottom(void) const { return ctXY2.CoordGetY(); }
  /* ----------------------------------------------------------------------- */
  void SetCoLeft(const Type1 tNLeft) { ctXY1.CoordSetX(tNLeft); }
  void SetCoTop(const Type1 tNTop) { ctXY1.CoordSetY(tNTop); }
  void SetCoRight(const Type2 tNRight) { ctXY2.CoordSetX(tNRight); }
  void SetCoBottom(const Type2 tNBottom) { ctXY2.CoordSetY(tNBottom); }
  /* ----------------------------------------------------------------------- */
  void SetCoLeft(const FboCoords &fcValue)
    { SetCoLeft(fcValue.GetCoLeft()); }
  void SetCoTop(const FboCoords &fcValue)
    { SetCoTop(fcValue.GetCoTop()); }
  void SetCoRight(const FboCoords &fcValue)
    { SetCoRight(fcValue.GetCoRight()); }
  void SetCoBottom(const FboCoords &fcValue)
    { SetCoBottom(fcValue.GetCoBottom()); }
  /* ----------------------------------------------------------------------- */
  void ResetCoords(void) { ctXY1.CoordSet(); ctXY2.CoordSet(); }
  /* ----------------------------------------------------------------------- */
  void SetCoords(const Type1 tNLeft, const Type1 tNTop,
                 const Type2 tNRight, const Type2 tNBottom)
  { // Viewport X origin different from cached?
    if(GetCoLeft() != tNLeft)
    { // Update viewport X value and other values if changed
      SetCoLeft(tNLeft);
      if(GetCoTop() != tNTop) SetCoTop(tNTop);
      if(GetCoRight() != tNRight) SetCoRight(tNRight);
      if(GetCoBottom() != tNBottom) SetCoBottom(tNBottom);
    } // Viewport Y origin different from cached?
    else if(GetCoTop() != tNTop)
    { // Update viewport Y value and other values if changed
      SetCoTop(tNTop);
      if(GetCoRight() != tNRight) SetCoRight(tNRight);
      if(GetCoBottom() != tNBottom) SetCoBottom(tNBottom);
    } // Viewport width different from cached?
    else if(GetCoRight() != tNRight)
    { // Update viewport width and other values if changed
      SetCoRight(tNRight);
      if(GetCoBottom() != tNBottom) SetCoBottom(tNBottom);
    } // Viewport height different from cached? Update it
    else if(GetCoBottom() != tNBottom) SetCoBottom(tNBottom);
  }
  /* ----------------------------------------------------------------------- */
  void SetCoords(const FboCoords &fcValue)
    { SetCoords(fcValue.GetCoLeft(), fcValue.GetCoTop(),
                fcValue.GetCoRight(), fcValue.GetCoBottom()); }
  /* -- Init constructor --------------------------------------------------- */
  FboCoords(const Type1 tNLeft,        // Left (X1) co-ordinate
            const Type1 tNTop,         // Top (Y1) co-ordinate
            const Type2 tNRight,       // Right (X2) co-ordinate
            const Type2 tNBottom) :    // Bottom (Y2) co-ordinate
    /* -- Initialisers ----------------------------------------------------- */
    ctXY1{ tNLeft, tNTop },            // X1 & Y1 co-ordinate
    ctXY2{ tNRight, tNBottom }         // X2 & Y2 co-ordinate
    /* -- Code ------------------------------------------------------------- */
    { }                                // Do nothing else
  /* -- Default constructor ------------------------------------------------ */
  FboCoords(void) :                    // No parameters
    /* -- Initialisers ----------------------------------------------------- */
    FboCoords{ 0, 0, 0, 0 }            // Initialise co-ordinates
    /* --------------------------------------------------------------------- */
    { }                                // Do nothing else
};/* ----------------------------------------------------------------------- */
typedef FboCoords<GLfloat>       FboFloatCoords; // Coords made of floats
typedef Dimensions<GLsizei>      DimGLInt;       // Dimension of GLints
/* == Data required to complete a render of an fbo ========================= */
struct FboRenderItem :                 // Rendering item data class
  /* -- Base classes ------------------------------------------------------- */
  public FboColour,                    // Clear colour of the fbo
  public FboBlend,                     // Blend mode of the fbo
  public FboFloatCoords,               // Matrix co-ordinates of the fbo
  public DimGLInt                      // Fbo dimensions
{ /* ----------------------------------------------------------------------- */
  GLuint           uiFBO;              // Fbo name
  bool             bClear;             // Clear the fbo?
  /* -- Default constructor ------------------------------------------------ */
  FboRenderItem(void) :                // No parameters
    /* -- Initialisers ----------------------------------------------------- */
    uiFBO(0),                          // No fbo id yet
    bClear(true)                       // Clear fbo set
    /* -- No code ---------------------------------------------------------- */
    { }
};/* ----------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
