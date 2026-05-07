/* == FBOITEM.HPP ========================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## Defines the 'FboItem' class which is used by the 'Fbo' class in     ## **
** ## 'fbo.hpp' to store texture co-ordinates, triangle positions and     ## **
** ## colour intensity modifiers. All this data can be manipulated and    ## **
** ## retrieved with the available methods.                               ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IFboItem {                   // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IFboCmd::P;            using IUtil::P::UtilNormaliseEx;
using namespace Lib::OS::GlFW::Types;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* == Fbo item class ======================================================= */
struct FboItem
{ /* -- Public typedefs ---------------------------------------------------- */
  using TriTexData  = StdArray<GLfloat, stFloatsPerCoord>;  // Tri TexCoords
  using QuadTexData = StdArray<TriTexData, stTrisPerQuad>;  // Quad TexCoords
  using TriPosData  = StdArray<GLfloat, stFloatsPerPos>;    // Tri Positions
  using QuadPosData = StdArray<TriPosData, stTrisPerQuad>;  // Quad Positions
  using TriIntData  = StdArray<GLfloat, stFloatsPerColour>; // Tri intensities
  using QuadIntData = StdArray<TriIntData, stTrisPerQuad>;  // Quad intensities
  /* -- Stored colour data ------------------------------------------------- */
  QuadIntData      qidSave;            // Saved colour data (Push/PopColour)
  /* -- Private typedefs ------------------------------------------ */ private:
  using AllData = StdArray<GLfloat, stFloatsPerQuad>; // All data elements
  /* -- Private variables -------------------------------------------------- */
  union Quad                           // Render to texture Vertex array data
  { /* --------------------------------------------------------------------- */
    AllData        adData;             // Vertices to upload to VBO
    /* --------------------------------------------------------------------- */
    struct Parts                       // Parts of 'adData'
    { /* ------------------------------------------------------------------- */
      QuadTexData  qtdCoord;           // Quad tex-coords data
      QuadPosData  qpdPos;             // Quad position data
      QuadIntData  qidColour;          // Quad colour data
      /* ------------------------------------------------------------------- */
    } glfCos;                          // Quad variable
    /* --------------------------------------------------------------------- */
  } sBuffer;                           // End of quad data union
  /* ----------------------------------------------------------------------- */
  constexpr static const size_t
    stUInt8Bits  = sizeof(uint8_t) * 8,
    stUInt16Bits = sizeof(uint16_t) * 8,
    stUInt24Bits = stUInt8Bits + stUInt16Bits;
  /* -- Get defaults as lookup table --------------------------------------- */
  static const Quad &FboItemGetDefaultLookup()
  { // This is equal to the following calls. It's just easier to memcpy
    // the whole table across then doing pointless calculation.
    // - SetTexCoord(0, 0, 1, 1);
    // - SetVertex(-1, 1, 1, -1);
    // - SetRGBA(1, 1, 1, 1);
    static const Quad qData{{
      // QuadTexData qtdCoord (render the entire texture on the triangles)
      0.0f, 0.0f,  1.0f, 0.0f, // (T1V1,T1V2)[X+Y]       V1 V2    V3
      0.0f, 1.0f,  1.0f, 1.0f, // (T1V3,T2V1)[X+Y] T1 -> |XX/   /XX| <- T2
      0.0f, 1.0f,  1.0f, 0.0f, // (T2V2,T2V3)[X+Y]       V3    V2 V1
      // QuadPosData qpdPos (render the two triangles full-screen)
     -1.0f, 1.0f,  1.0f, 1.0f, // (T1V1,T1V2)[X+Y]       V1 V2    V3
     -1.0f,-1.0f,  1.0f,-1.0f, // (T1V3,T2V1)[X+Y] T1 -> |XX/   /XX| <- T2
     -1.0f,-1.0f,  1.0f, 1.0f, // (T2V2,T2V3)[X+Y]       V3    V2 V1
      // QuadIntData qidColour (all solid white intensity fully opaque)
      1.0f, 1.0f, 1.0f, 1.0f,  1.0f, 1.0f, 1.0f, 1.0f, // (T1V1,T1V2)[R+G+B+A]
      1.0f, 1.0f, 1.0f, 1.0f,  1.0f, 1.0f, 1.0f, 1.0f, // (T1V3,T2V1)[R+G+B+A]
      1.0f, 1.0f, 1.0f, 1.0f,  1.0f, 1.0f, 1.0f, 1.0f  // (T2V2,T2V3)[R+G+B+A]
    }}; // Return the lookup table
    return qData;
  }
  /* -- Return static offset indexes for glVertexAttribPointer() --- */ public:
  constexpr static const size_t stTCPos = 0;
  static const GLvoid *FboItemGetTCPos()
    { return reinterpret_cast<GLvoid*>(stTCPos); }
  constexpr static const size_t stVPos =
    stTCPos + sizeof(sBuffer.glfCos.qtdCoord);
  static const GLvoid *FboItemGetVPos()
    { return reinterpret_cast<GLvoid*>(stVPos); }
  constexpr static const size_t stCPos =
    stVPos + sizeof(sBuffer.glfCos.qpdPos);
  static const GLvoid *FboItemGetCPos()
    { return reinterpret_cast<GLvoid*>(stCPos); }
  /* -- Positions of data for in aReturn static offset indexes ------------- */
  QuadPosData &FboItemGetVData() { return sBuffer.glfCos.qpdPos; }
  TriPosData &FboItemGetVData(const size_t stPos)
    { return FboItemGetVData()[stPos]; }
  TriPosData &FboItemGetVDataT1() { return FboItemGetVData().front(); }
  TriPosData &FboItemGetVDataT2() { return FboItemGetVData().back(); }
  QuadTexData &FboItemGetTCData() { return sBuffer.glfCos.qtdCoord; }
  TriTexData &FboItemGetTCData(const size_t stPos)
    { return FboItemGetTCData()[stPos]; }
  TriTexData &FboItemGetTCDataT1() { return FboItemGetTCData().front(); }
  TriTexData &FboItemGetTCDataT2() { return FboItemGetTCData().back(); }
  QuadIntData &FboItemGetCData() { return sBuffer.glfCos.qidColour; }
  TriIntData &FboItemGetCData(const size_t stPos)
    { return FboItemGetCData()[stPos]; }
  TriIntData &FboItemGetCDataT1() { return FboItemGetCData().front(); }
  TriIntData &FboItemGetCDataT2() { return FboItemGetCData().back(); }
  /* -- Get data ----------------------------------------------------------- */
  const GLvoid *FboItemGetData() const { return sBuffer.adData.data(); }
  GLsizei FboItemGetDataSize() const { return sizeof(sBuffer.adData); }
  /* -- Set vertex bounds directly on one triangle ------------------------- */
  void FboItemSetVertexEx(const size_t stId, const TriPosData &tpdNew)
    { FboItemGetVData(stId) = tpdNew; }
  void FboItemSetTexCoordEx(const size_t stId, const TriTexData &ttdNew)
    { FboItemGetTCData(stId) = ttdNew; }
  void FboItemSetColourEx(const size_t stId, const TriIntData &tidNew)
    { FboItemGetCData(stId) = tidNew; }
  /* -- Save and restore colour data --------------------------------------- */
  void FboItemPushQuadColour() { qidSave = FboItemGetCData(); }
  void FboItemPopQuadColour() { FboItemGetCData() = qidSave; }
  /* -- Set vertex bounds with pivoted angle ------------------------------- */
  void FboItemSetVertex(const GLfloat glfX1, const GLfloat glfY1,
    const GLfloat glfX2, const GLfloat glfY2, const GLfloat glfAlpha)
  { // Convert the angle to radians (M_PI)
    const GLfloat glfAR = glfAlpha * 2.0f * 3.14159265358979323846f,
    // Pre-calculate sine and cosine once
    glfSin = sinf(glfAR), glfCos = cosf(glfAR),
    // Get the half dimensions
    glfXP = (glfX2 - glfX1) * 0.5f, glfYP = (glfY2 - glfY1) * 0.5f,
    // Multiply components once to save CPU cycles
    glfCX = glfCos * glfXP, glfSX = glfSin * glfXP,
    glfCY = glfCos * glfYP, glfSY = glfSin * glfYP,
    // Reconstruct the exact original corner offsets using a rotation matrix
    glfCa = -glfCX + glfSY, glfCb = -glfSX - glfCY,
    glfCc =  glfCX + glfSY, glfCd =  glfSX - glfCY,
    glfCe = -glfCX - glfSY, glfCf = -glfSX + glfCY,
    glfCg =  glfCX - glfSY, glfCh =  glfSX + glfCY;
    // Update the first triangle of the quad
    TriPosData &tidT1 = FboItemGetVDataT1();
    tidT1[0] = glfX1 + glfCa; tidT1[1] = glfY1 + glfCb; // T1 V1 (XY)  V1 V2
    tidT1[2] = glfX1 + glfCc; tidT1[3] = glfY1 + glfCd; // T1 V2 (XY)  |XX/
    tidT1[4] = glfX1 + glfCe; tidT1[5] = glfY1 + glfCf; // T1 V3 (XY)  V3
    // Update the second triangle of the quad
    TriPosData &tidT2 = FboItemGetVDataT2();
    tidT2[0] = glfX1 + glfCg; tidT2[1] = glfY1 + glfCh; // T2 V1 (XY)     V3
    tidT2[2] = glfX1 + glfCe; tidT2[3] = glfY1 + glfCf; // T2 V2 (XY)   /XX|
    tidT2[4] = glfX1 + glfCc; tidT2[5] = glfY1 + glfCd; // T2 V3 (XY)  V2 V1
  }
  /* -- Set vertex bounds -------------------------------------------------- */
  void FboItemSetVertex(const GLfloat glfX1, const GLfloat glfY1,
    const GLfloat glfX2, const GLfloat glfY2)
  { // Update the first triangle of the quad
    TriPosData &tidT1 = FboItemGetVDataT1();
    tidT1[0] = glfX1; tidT1[1] = glfY1; // Triangle 1 / Vertice 1 (XY)  V1 V2
    tidT1[2] = glfX2; tidT1[3] = glfY1; //     "      /    "    2 (XY)  |XX/
    tidT1[4] = glfX1; tidT1[5] = glfY2; //     "      /    "    3 (XY)  V3
    // Update the second triangle of the quad
    TriPosData &tidT2 = FboItemGetVDataT2();
    tidT2[0] = glfX2; tidT2[1] = glfY2; // Triangle 2 / Vertice 1 (XY)     V3
    tidT2[2] = glfX1; tidT2[3] = glfY2; //     "      /    "    2 (XY)   /XX|
    tidT2[4] = glfX2; tidT2[5] = glfY1; //     "      /    "    3 (XY)  V2 V1
  }
  /* -- Set vertex bounds modified by normals horizontally ----------------- */
  void FboItemSetVertex(const GLfloat glfX1, const GLfloat glfY1,
    const GLfloat glfX2, const GLfloat glfY2, const GLfloat glfML,
    const GLfloat glfMR)
  { // Modify vertex based on horizotal scale normal (left edge)
    TriPosData &tpdT1 = FboItemGetVDataT1();
    tpdT1[0] = glfX2 - ((glfX2 - glfX1) * glfML);       // T1 / V1  V1 -- V2
    tpdT1[1] = glfY1;                                   //           |XXXX/
    tpdT1[2] = tpdT1[0] + ((glfX2 - tpdT1[0]) * glfMR); // T1 / V2   |XXX/
    tpdT1[3] = glfY1;                                   //           |XX/
    tpdT1[4] = tpdT1[0];                                // T1 / V3   |X/
    tpdT1[5] = glfY2;                                   //           V3
    // Modify vertex based on horizotal scale normal (right edge)
    TriPosData &tpdT2 = FboItemGetVDataT2();
    tpdT2[0] = glfX1 - ((glfX1 - glfX2) * glfMR);       // T2 / V1       V3
    tpdT2[1] = glfY2;                                   //              /X|
    tpdT2[2] = tpdT2[0] + ((glfX1 - tpdT2[0]) * glfML); // T2 / V2     /XX|
    tpdT2[3] = glfY2;                                   //            /XXX|
    tpdT2[4] = tpdT2[0];                                // T2 / V3   /XXXX|
    tpdT2[5] = glfY1;                                   //          V2 -- V1
  }
  /* -- Set vertex with coords, dimensions and angle ----------------------- */
  void FboItemSetVertexWH(const GLfloat glfX, const GLfloat glfY,
    const GLfloat glfW, const GLfloat glfH, const GLfloat glfAlpha)
  { FboItemSetVertex(glfX, glfY, glfX + glfW, glfY + glfH, glfAlpha); }
  /* -- Set vertex co-ordinates and dimensions ----------------------------- */
  void FboItemSetVertexWH(const GLfloat glfX, const GLfloat glfY,
    const GLfloat glfW, const GLfloat glfH)
  { FboItemSetVertex(glfX, glfY, glfX + glfW, glfY+glfH); }
  /* -- Set vertex bounds and return it ------------------------------------ */
  const QuadPosData &FboItemSetAndGetVertex(const GLfloat glfX1,
    const GLfloat glfY1, const GLfloat glfX2, const GLfloat glfY2,
    const GLfloat glfAlpha)
  { FboItemSetVertex(glfX1, glfY1, glfX2, glfY2, glfAlpha);
    return FboItemGetVData(); }
  /* -- Set vertex bounds and return it ------------------------------------ */
  const QuadPosData &FboItemSetAndGetVertex(const GLfloat glfX1,
    const GLfloat glfY1, const GLfloat glfX2, const GLfloat glfY2)
  { FboItemSetVertex(glfX1, glfY1, glfX2, glfY2); return FboItemGetVData(); }
  /* -- Set vertex bounds with modified left and right bounds and get ------ */
  const QuadPosData &FboItemSetAndGetVertex(const GLfloat glfX1,
    const GLfloat glfY1, const GLfloat glfX2, const GLfloat glfY2,
    const GLfloat glfML, const GLfloat glfMR)
  { FboItemSetVertex(glfX1, glfY1, glfX2, glfY2, glfML, glfMR);
    return FboItemGetVData(); }
  /* -- Set tex coords for FBO (Full and simple) --------------------------- */
  void FboItemSetTexCoord(const GLfloat glfX1, const GLfloat glfY1,
    const GLfloat glfX2, const GLfloat glfY2)
  { // Set the texture coordinates of the first triangle
    TriTexData &tidT1 = FboItemGetTCDataT1();
    tidT1[0] = glfX1; tidT1[1] = glfY1; // Vertex 1 of Triangle 1  V0 V1
    tidT1[2] = glfX2; tidT1[3] = glfY1; // Vertex 2 of Triangle 1  |  /
    tidT1[4] = glfX1; tidT1[5] = glfY2; // Vertex 3 of Triangle 1  V2
    // Set the texture coordinates of the second triangle
    TriTexData &tidT2 = FboItemGetTCDataT2();
    tidT2[0] = glfX2; tidT2[1] = glfY2; // Vertex 1 of Triangle 2     V2
    tidT2[2] = glfX1; tidT2[3] = glfY2; // Vertex 2 of Triangle 2   /  |
    tidT2[4] = glfX2; tidT2[5] = glfY1; // Vertex 3 of Triangle 2  V1 V0
  }
  /* -- Set tex coords for FBO based on horizontal scale normals ----------- */
  void FboItemSetTexCoord(const QuadTexData &qtdC, const GLfloat glfML,
    const GLfloat glfMR)
  { // Update tex coords for triangle 1
    const TriTexData &tcdSrc1 = qtdC.front();   // Source
    TriTexData &tcdDst1 = FboItemGetTCDataT1(); // Destination
    tcdDst1[0] = tcdSrc1[2] - ((tcdSrc1[2] - tcdSrc1[0]) * glfML); // T1 V1X
    tcdDst1[1] = tcdSrc1[1];                                       // T1 V1Y
    tcdDst1[2] = tcdDst1[0] + ((tcdSrc1[2] - tcdDst1[0]) * glfMR); // T1 V2X
    tcdDst1[3] = tcdSrc1[3];                                       // T1 V2Y
    tcdDst1[4] = tcdDst1[0];                                       // T1 V3X
    tcdDst1[5] = tcdSrc1[5];                                       // T1 V3Y
    // Update tex coords for triangle 2
    const TriTexData &tcdSrc2 = qtdC.back();    // Source
    TriTexData &tcdDst2 = FboItemGetTCDataT2(); // Destination
    tcdDst2[0] = tcdSrc2[2] - ((tcdSrc2[2] - tcdSrc2[0]) * glfMR); // T2 V1X
    tcdDst2[1] = tcdSrc2[1];                                       // T2 V1Y
    tcdDst2[2] = tcdDst2[0] + ((tcdSrc2[2] - tcdDst2[0]) * glfML); // T2 V2X
    tcdDst2[3] = tcdSrc2[3];                                       // T2 V2Y
    tcdDst2[4] = tcdDst2[0];                                       // T2 V3X
    tcdDst2[5] = tcdSrc2[5];                                       // T2 V3Y
  }
  /* -- Set colour for FBO ------------------------------------------------- */
  void FboItemSetQuadRGBA(const GLfloat glfRed, const GLfloat glfGreen,
    const GLfloat glfBlue, const GLfloat glfAlpha)
  { // Set the colour data of the first triangle
    TriIntData &tidT1 = FboItemGetCDataT1();
    tidT1[ 0] = glfRed;  tidT1[ 1] = glfGreen; // Vertex 1 of triangle 1
    tidT1[ 2] = glfBlue; tidT1[ 3] = glfAlpha;
    tidT1[ 4] = glfRed;  tidT1[ 5] = glfGreen; // Vertex 2 of triangle 1
    tidT1[ 6] = glfBlue; tidT1[ 7] = glfAlpha;
    tidT1[ 8] = glfRed;  tidT1[ 9] = glfGreen; // Vertex 3 of triangle 1
    tidT1[10] = glfBlue; tidT1[11] = glfAlpha;
    // Set the colour data of the second triangle
    TriIntData &tidT2 = FboItemGetCDataT2();
    tidT2[ 0] = glfRed;  tidT2[ 1] = glfGreen; // Vertex 1 of triangle 2
    tidT2[ 2] = glfBlue; tidT2[ 3] = glfAlpha;
    tidT2[ 4] = glfRed;  tidT2[ 5] = glfGreen; // Vertex 2 of triangle 2
    tidT2[ 6] = glfBlue; tidT2[ 7] = glfAlpha;
    tidT2[ 8] = glfRed;  tidT2[ 9] = glfGreen; // Vertex 3 of triangle 2
    tidT2[10] = glfBlue; tidT2[11] = glfAlpha;
  }
  /* -- Set texture coords and dimensions ---------------------------------- */
  void FboItemSetTexCoordWH(const GLfloat glfX, const GLfloat glfY,
    const GLfloat glfW, const GLfloat glfH)
      { FboItemSetTexCoord(glfX, glfY, glfX + glfW, glfY + glfH); }
  /* -- Set vertex bounds and return it ------------------------------------ */
  const QuadTexData &FboItemSetAndGetCoord(const QuadTexData &qtdC,
    const GLfloat glfML, const GLfloat glfMR)
      { FboItemSetTexCoord(qtdC, glfML, glfMR); return FboItemGetTCData(); }
  /* -- Set colour components (0xAARRGGBB) --------------------------------- */
  void FboItemSetQuadRGBAInt(const unsigned uColour)
    { FboItemSetQuadRGBA(UtilNormaliseEx<GLfloat, stUInt16Bits>(uColour),
        UtilNormaliseEx<GLfloat, stUInt8Bits>(uColour),
        UtilNormaliseEx<GLfloat>(uColour),
        UtilNormaliseEx<GLfloat, stUInt24Bits>(uColour)); }
  /* -- Set colour components (0xRRGGBB) ----------------------------------- */
  void FboItemSetQuadRGB(const GLfloat glfRed, const GLfloat glfGreen,
    const GLfloat glfBlue)
      { FboItemSetQuadRed(glfRed);
        FboItemSetQuadGreen(glfGreen);
        FboItemSetQuadBlue(glfBlue); }
  /* -- Set colour components by integer ----------------------------------- */
  void FboItemSetQuadRGBInt(const unsigned uColour)
    { FboItemSetQuadRGB(UtilNormaliseEx<GLfloat, stUInt16Bits>(uColour),
        UtilNormaliseEx<GLfloat, stUInt8Bits>(uColour),
        UtilNormaliseEx<GLfloat>(uColour)); }
  /* -- Update red component ----------------------------------------------- */
  void FboItemSetQuadRed(const GLfloat glfRed)
    { TriIntData &tidT1 = FboItemGetCDataT1(), &tidT2 = FboItemGetCDataT2();
      tidT1[0] = tidT1[4] = tidT1[8] =
      tidT2[0] = tidT2[4] = tidT2[8] = glfRed; }
  /* -- Update green component --------------------------------------------- */
  void FboItemSetQuadGreen(const GLfloat glfGreen)
    { TriIntData &tidT1 = FboItemGetCDataT1(), &tidT2 = FboItemGetCDataT2();
      tidT1[1] = tidT1[5] = tidT1[9] =
      tidT2[1] = tidT2[5] = tidT2[9] = glfGreen; }
  /* -- Update blue component ---------------------------------------------- */
  void FboItemSetQuadBlue(const GLfloat glfBlue)
    { TriIntData &tidT1 = FboItemGetCDataT1(), &tidT2 = FboItemGetCDataT2();
      tidT1[2] = tidT1[6] = tidT1[10] =
      tidT2[2] = tidT2[6] = tidT2[10] = glfBlue; }
  /* -- Update alpha component --------------------------------------------- */
  void FboItemSetQuadAlpha(const GLfloat glfAlpha)
    { TriIntData &tidT1 = FboItemGetCDataT1(), &tidT2 = FboItemGetCDataT2();
      tidT1[3] = tidT1[7] = tidT1[11] =
      tidT2[3] = tidT2[7] = tidT2[11] = glfAlpha; }
  /* -- Set defaults ------------------------------------------------------- */
  void FboItemSetDefaults() { sBuffer = FboItemGetDefaultLookup(); }
  /* -- Constructor -------------------------------------------------------- */
  FboItem() :
    /* -- Initialisers ----------------------------------------------------- */
    sBuffer{                           // Initialise storage...
      FboItemGetDefaultLookup() }      // ...with default values
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- Init with colour (from font) --------------------------------------- */
  explicit FboItem(const unsigned uColour) :
    /* -- Initialisers ----------------------------------------------------- */
    FboItem{}                          // Initialise default values
    /* -- Initialise colour ------------------------------------------------ */
    { FboItemSetQuadRGBAInt(uColour); }
};/* ----------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
