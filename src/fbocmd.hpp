/* == FBOCMD.HPP =========================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## Definition of the 'FboCmd' class which stores a list of OpenGL      ## **
** ## commands.                                                           ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IFboCmd {                    // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using Lib::OS::GlFW::GLfloat;          using Lib::OS::GlFW::GLsizei;
using Lib::OS::GlFW::GLuint;           using Lib::OS::GlFW::GLvoid;
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
  const GLsizei      siVertices;       // Total vertices to draw
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
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
