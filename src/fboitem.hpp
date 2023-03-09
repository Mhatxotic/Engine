/* == FBOITEM.HPP ========================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## Allows storage and manipulation of a quad (two trangles).           ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IFboItem {                   // Start of private module namespace
/* -- Outside types used --------------------------------------------------- */
using Lib::OS::GlFW::GLfloat;          using Lib::OS::GlFW::GLsizei;
using Lib::OS::GlFW::GLvoid;
/* -- Outside functions used ----------------------------------------------- */
using IUtil::P::UtilNormaliseEx;
/* -- Outside variables used ----------------------------------------------- */
using IFboDef::P::stFloatsPerColour;   using IFboDef::P::stFloatsPerCoord;
using IFboDef::P::stFloatsPerPos;      using IFboDef::P::stFloatsPerQuad;
using IFboDef::P::stTrisPerQuad;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* == Fbo item class ======================================================= */
struct FboItem
{ /* -- Public typedefs ---------------------------------------------------- */
  typedef array<GLfloat, stFloatsPerCoord> TriCoordData; // Triangle TexCoords
  typedef array<TriCoordData, stTrisPerQuad> QuadCoordData; // Quad tex-coord
  typedef array<GLfloat, stFloatsPerPos> TriPosData; // Triangle positions
  typedef array<TriPosData, stTrisPerQuad> QuadPosData; // Quad position data
  typedef array<GLfloat, stFloatsPerColour> TriColData; // Triangle intensities
  typedef array<TriColData, stTrisPerQuad> QuadColData; // Quad colour data
  /* -- Stored colour data ------------------------------------------------- */
  QuadColData      faCSave;            // Saved colour data (Push/PopColour)
  /* -- Private typedefs ------------------------------------------ */ private:
  typedef array<GLfloat, stFloatsPerQuad> AllData; // All data elements
  /* -- Private variables -------------------------------------------------- */
  union Quad                           // Render to texture Vertex array data
  { /* --------------------------------------------------------------------- */
    AllData        faData;             // Vertices to upload to VBO
    /* --------------------------------------------------------------------- */
    struct Parts                       // Parts of 'faData'
    { /* ------------------------------------------------------------------- */
      QuadCoordData qdCoord;           // Quad tex-coords data
      QuadPosData   qdPos;             // Quad position data
      QuadColData   qdColour;          // Quad colour data
      /* ------------------------------------------------------------------- */
    } c;                               // Quad variable
    /* --------------------------------------------------------------------- */
  } sBuffer;                           // End of quad data union
  /* ----------------------------------------------------------------------- */
  constexpr static const size_t
    stUInt8Bits  = sizeof(uint8_t) * 8,
    stUInt16Bits = sizeof(uint16_t) * 8,
    stUInt24Bits = stUInt8Bits + stUInt16Bits;
  /* -- Get defaults as lookup table --------------------------------------- */
  const Quad &FboItemGetDefaultLookup(void) const
  { // This is equal to the following calls. It's just easier to memcpy the
    // whole table across then doing pointless calculation.
    // - SetTexCoord(0, 0, 1, 1);
    // - SetVertex(-1, 1, 1, -1);
    // - SetRGBA(1, 1, 1, 1);
    static const Quad qData{{
      // QuadCoordData qdCoord (render the entire texture on the triangles)
      0.0f, 0.0f,  1.0f, 0.0f, // (T1V1,T1V2)[X+Y]       V1 V2    V3
      0.0f, 1.0f,  1.0f, 1.0f, // (T1V3,T2V1)[X+Y] T1 -> |XX/   /XX| <- T2
      0.0f, 1.0f,  1.0f, 0.0f, // (T2V2,T2V3)[X+Y]       V3    V2 V1
      // QuadPosData qdPos (render the two triangles full-screen)
     -1.0f, 1.0f,  1.0f, 1.0f, // (T1V1,T1V2)[X+Y]       V1 V2    V3
     -1.0f,-1.0f,  1.0f,-1.0f, // (T1V3,T2V1)[X+Y] T1 -> |XX/   /XX| <- T2
     -1.0f,-1.0f,  1.0f, 1.0f, // (T2V2,T2V3)[X+Y]       V3    V2 V1
      // QuadColData qdColour (all solid white intensity fully opaque)
      1.0f, 1.0f, 1.0f, 1.0f,  1.0f, 1.0f, 1.0f, 1.0f, // (T1V1,T1V2)[R+G+B+A]
      1.0f, 1.0f, 1.0f, 1.0f,  1.0f, 1.0f, 1.0f, 1.0f, // (T1V3,T2V1)[R+G+B+A]
      1.0f, 1.0f, 1.0f, 1.0f,  1.0f, 1.0f, 1.0f, 1.0f  // (T2V2,T2V3)[R+G+B+A]
    }}; // Return the lookup table
    return qData;
  }
  /* -- Return static offset indexes for glVertexAttribPointer() --- */ public:
  constexpr static const size_t stTCPos = 0;
  static const GLvoid *FboItemGetTCPos(void)
    { return reinterpret_cast<GLvoid*>(stTCPos); }
  constexpr static const size_t stVPos = stTCPos + sizeof(sBuffer.c.qdCoord);
  static const GLvoid *FboItemGetVPos(void)
    { return reinterpret_cast<GLvoid*>(stVPos); }
  constexpr static const size_t stCPos = stVPos + sizeof(sBuffer.c.qdPos);
  static const GLvoid *FboItemGetCPos(void)
    { return reinterpret_cast<GLvoid*>(stCPos); }
  /* -- Positions of data for in aReturn static offset indexes ------------- */
  QuadPosData &FboItemGetVData(void) { return sBuffer.c.qdPos; }
  TriPosData &FboItemGetVData(const size_t stPos)
    { return FboItemGetVData()[stPos]; }
  TriPosData &FboItemGetVDataT1(void) { return FboItemGetVData().front(); }
  TriPosData &FboItemGetVDataT2(void) { return FboItemGetVData().back(); }
  QuadCoordData &FboItemGetTCData(void) { return sBuffer.c.qdCoord; }
  TriCoordData &FboItemGetTCData(const size_t stPos)
    { return FboItemGetTCData()[stPos]; }
  TriCoordData &FboItemGetTCDataT1(void) { return FboItemGetTCData().front(); }
  TriCoordData &FboItemGetTCDataT2(void) { return FboItemGetTCData().back(); }
  QuadColData &FboItemGetCData(void) { return sBuffer.c.qdColour; }
  TriColData &FboItemGetCData(const size_t stPos)
    { return FboItemGetCData()[stPos]; }
  TriColData &FboItemGetCDataT1(void) { return FboItemGetCData().front(); }
  TriColData &FboItemGetCDataT2(void) { return FboItemGetCData().back(); }
  /* -- Get data ----------------------------------------------------------- */
  const GLvoid *FboItemGetData(void) const { return sBuffer.faData.data(); }
  GLsizei FboItemGetDataSize(void) const { return sizeof(sBuffer.faData); }
  /* -- Set vertex bounds directly on one triangle ------------------------- */
  void FboItemSetVertexEx(const size_t stId, const TriPosData &tpdNew)
    { FboItemGetVData(stId) = tpdNew; }
  void FboItemSetTexCoordEx(const size_t stId, const TriCoordData &tcdNew)
    { FboItemGetTCData(stId) = tcdNew; }
  void FboItemSetColourEx(const size_t stId, const TriColData &tcdNew)
    { FboItemGetCData(stId) = tcdNew; }
  /* -- Save and restore colour data --------------------------------------- */
  void FboItemPushQuadColour(void) { faCSave = FboItemGetCData(); }
  void FboItemPopQuadColour(void) { FboItemGetCData() = faCSave; }
  /* -- Set vertex bounds with pivoted angle ------------------------------- */
  void FboItemSetVertex(const GLfloat fX1, const GLfloat fY1,
    const GLfloat fX2, const GLfloat fY2, const GLfloat fA)
  { // UtilDenormalise the angle to radians (M_PI)
    const GLfloat fAR = fA * 2.0f * 3.141592653589793238462643383279502884f,
    // Get the middle pixel of the quad.
    fXP = (fX2-fX1)/2,                  fYP = (fY2-fY1)/2,
    // Rotate vertices around the centre of the quad
    fC1 = atan2f(-fYP,fXP)+fAR,         fC2 = atan2f(-fYP,-fXP)+fAR,
    fC3 = atan2f( fYP,fXP)+fAR,         fC4 = atan2f( fYP,-fXP)+fAR,
    fXPs = fXP*fXP,                     fXPsN = -fXP*-fXP,
    fYPs = fYP*fYP,                     fYPsN = -fYP*-fYP,
    fC5 = sqrtf(fYPsN+fXPs),            fC6 = sqrtf(fYPsN+fXPsN),
    fC7 = sqrtf(fYPs+fXPs),             fC8 = sqrtf(fYPs+fXPsN),
    fCa = cosf(fC2)*fC5,                fCb = sinf(fC2)*fC5,
    fCc = cosf(fC1)*fC6,                fCd = sinf(fC1)*fC6,
    fCe = cosf(fC4)*fC7,                fCf = sinf(fC4)*fC7,
    fCg = cosf(fC3)*fC8,                fCh = sinf(fC3)*fC8;
    // Update the first triangle of the quad
    TriPosData &tdT1 = FboItemGetVDataT1();
    tdT1[0] = fX1+fCa; tdT1[1] = fY1+fCb; // Vertex 1 / Triangle 1 (XY)  V1 V2
    tdT1[2] = fX1+fCc; tdT1[3] = fY1+fCd; //   "    2 /     "    1 (XY)  |XX/
    tdT1[4] = fX1+fCe; tdT1[5] = fY1+fCf; //   "    3 /     "    1 (XY)  V3
    // Update the second triangle of the quad
    TriPosData &tdT2 = FboItemGetVDataT2();
    tdT2[0] = fX1+fCg; tdT2[1] = fY1+fCh; // Vertex 1 / Triangle 2 (XY)     V3
    tdT2[2] = fX1+fCe; tdT2[3] = fY1+fCf; //   "    2 /     "    2 (XY)   /XX|
    tdT2[4] = fX1+fCc; tdT2[5] = fY1+fCd; //   "    3 /     "    2 (XY)  V2 V1
  }
  /* -- Set vertex bounds -------------------------------------------------- */
  void FboItemSetVertex(const GLfloat fX1, const GLfloat fY1,
    const GLfloat fX2, const GLfloat fY2)
  { // Update the first triangle of the quad
    TriPosData &tdT1 = FboItemGetVDataT1();
    tdT1[0] = fX1; tdT1[1] = fY1; // Triangle 1 / Vertice 1 (XY)  V1 V2
    tdT1[2] = fX2; tdT1[3] = fY1; //     "      /    "    2 (XY)  |XX/
    tdT1[4] = fX1; tdT1[5] = fY2; //     "      /    "    3 (XY)  V3
    // Update the second triangle of the quad
    TriPosData &tdT2 = FboItemGetVDataT2();
    tdT2[0] = fX2; tdT2[1] = fY2; // Triangle 2 / Vertice 1 (XY)     V3
    tdT2[2] = fX1; tdT2[3] = fY2; //     "      /    "    2 (XY)   /XX|
    tdT2[4] = fX2; tdT2[5] = fY1; //     "      /    "    3 (XY)  V2 V1
  }
  /* -- Set vertex bounds modified by normals horizontally ----------------- */
  void FboItemSetVertex(const GLfloat fX1, const GLfloat fY1,
    const GLfloat fX2, const GLfloat fY2, const GLfloat fML, const GLfloat fMR)
  { // Modify vertex based on horizotal scale normal (left edge)
    TriPosData &tdT1 = FboItemGetVDataT1();
    tdT1[0] = fX2-((fX2-fX1)*fML);         tdT1[1] = fY1; // T1 / V1  V1 V2
    tdT1[2] = tdT1[0]+((fX2-tdT1[0])*fMR); tdT1[3] = fY1; // T1 / V2  |XX/
    tdT1[4] = tdT1[0];                     tdT1[5] = fY2; // T1 / V3  V3
    // Modify vertex based on horizotal scale normal (right edge)
    TriPosData &tdT2 = FboItemGetVDataT2();
    tdT2[0] = fX1-((fX1-fX2)*fMR);         tdT2[1] = fY2; // T2 / V1     V3
    tdT2[2] = tdT2[0]+((fX1-tdT2[0])*fML); tdT2[3] = fY2; // T2 / V2   /XX|
    tdT2[4] = tdT2[0];                     tdT2[5] = fY1; // T2 / V3  V2 V1
  }
  /* -- Set vertex with coords, dimensions and angle ----------------------- */
  void FboItemSetVertexWH(const GLfloat fX, const GLfloat fY, const GLfloat fW,
    const GLfloat fH, const GLfloat fA)
      { FboItemSetVertex(fX, fY, fX+fW, fY+fH, fA); }
  /* -- Set vertex co-ordinates and dimensions ----------------------------- */
  void FboItemSetVertexWH(const GLfloat fX, const GLfloat fY, const GLfloat fW,
    const GLfloat fH)
      { FboItemSetVertex(fX, fY, fX+fW, fY+fH); }
  /* -- Set vertex bounds and return it ------------------------------------ */
  const QuadPosData &FboItemSetAndGetVertex(const GLfloat fX1,
    const GLfloat fY1, const GLfloat fX2, const GLfloat fY2, const GLfloat fA)
      { FboItemSetVertex(fX1, fY1, fX2, fY2, fA); return FboItemGetVData(); }
  /* -- Set vertex bounds and return it ------------------------------------ */
  const QuadPosData &FboItemSetAndGetVertex(const GLfloat fX1,
    const GLfloat fY1, const GLfloat fX2, const GLfloat fY2)
      { FboItemSetVertex(fX1, fY1, fX2, fY2); return FboItemGetVData(); }
  /* -- Set vertex bounds with modified left and right bounds and get ------ */
  const QuadPosData &FboItemSetAndGetVertex(const GLfloat fX1,
    const GLfloat fY1, const GLfloat fX2, const GLfloat fY2, const GLfloat fML,
      const GLfloat fMR)
        { FboItemSetVertex(fX1, fY1, fX2, fY2, fML, fMR);
          return FboItemGetVData(); }
  /* -- Set tex coords for FBO (Full and simple) --------------------------- */
  void FboItemSetTexCoord(const GLfloat fX1, const GLfloat fY1,
    const GLfloat fX2, const GLfloat fY2)
  { // Set the texture coordinates of the first triangle
    TriCoordData &tdT1 = FboItemGetTCDataT1();
    tdT1[0] = fX1; tdT1[1] = fY1;      // Vertex 1 of Triangle 1  V0 V1  T1
    tdT1[2] = fX2; tdT1[3] = fY1;      // Vertex 2 of Triangle 1  V2 /
    tdT1[4] = fX1; tdT1[5] = fY2;      // Vertex 3 of Triangle 1
    // Set the texture coordinates of the second triangle
    TriCoordData &tdT2 = FboItemGetTCDataT2();
    tdT2[0] = fX2; tdT2[1] = fY2;      // Vertex 1 of Triangle 2
    tdT2[2] = fX1; tdT2[3] = fY2;      // Vertex 2 of Triangle 2   / V2
    tdT2[4] = fX2; tdT2[5] = fY1;      // Vertex 3 of Triangle 2  V1 V0  T2
  }
  /* -- Set tex coords for FBO based on horizontal scale normals ----------- */
  void FboItemSetTexCoord(const QuadCoordData &fTC, const GLfloat fML,
    const GLfloat fMR)
  { // Update tex coords for triangle 1
    const TriCoordData &tdTS1 = fTC[0]; // Source
    TriCoordData &tdTD1 = FboItemGetTCDataT1(); // Destination
    tdTD1[0] = tdTS1[2]-((tdTS1[2]-tdTS1[0])*fML); tdTD1[1] = tdTS1[1]; // V1V2
    tdTD1[2] = tdTD1[0]+((tdTS1[2]-tdTD1[0])*fMR); tdTD1[3] = tdTS1[3]; // V3V1
    tdTD1[4] = tdTD1[0];                           tdTD1[5] = tdTS1[5]; // V2V3
    // Update tex coords for triangle 2
    const TriCoordData &tdTS2 = fTC[1]; // Source
    TriCoordData &tdTD2 = FboItemGetTCDataT2(); // Destination
    tdTD2[0] = tdTS2[2]-((tdTS2[2]-tdTS2[0])*fMR); tdTD2[1] = tdTS2[1]; // V1V2
    tdTD2[2] = tdTD2[0]+((tdTS2[2]-tdTD2[0])*fML); tdTD2[3] = tdTS2[3]; // V3V1
    tdTD2[4] = tdTD2[0];                           tdTD2[5] = tdTS2[5]; // V2V3
  }
  /* -- Set colour for FBO ------------------------------------------------- */
  void FboItemSetQuadRGBA(const GLfloat fR, const GLfloat fG, const GLfloat fB,
    const GLfloat fA)
  { // Set the colour data of the first triangle
    TriColData &tdT1 = FboItemGetCDataT1();
    tdT1[ 0] = fR; tdT1[ 1] = fG; tdT1[ 2] = fB; tdT1[ 3] = fA; // V1 of T1
    tdT1[ 4] = fR; tdT1[ 5] = fG; tdT1[ 6] = fB; tdT1[ 7] = fA; // V2 of T1
    tdT1[ 8] = fR; tdT1[ 9] = fG; tdT1[10] = fB; tdT1[11] = fA; // V3 of T1
    // Set the colour data of the second triangle
    TriColData &tdT2 = FboItemGetCDataT2();
    tdT2[ 0] = fR; tdT2[ 1] = fG; tdT2[ 2] = fB; tdT2[ 3] = fA; // V1 of T2
    tdT2[ 4] = fR; tdT2[ 5] = fG; tdT2[ 6] = fB; tdT2[ 7] = fA; // V2 of T2
    tdT2[ 8] = fR; tdT2[ 9] = fG; tdT2[10] = fB; tdT2[11] = fA; // V3 of T2
  }
  /* -- Set texture coords and dimensions ---------------------------------- */
  void FboItemSetTexCoordWH(const GLfloat fX, const GLfloat fY,
    const GLfloat fW, const GLfloat fH)
      { FboItemSetTexCoord(fX, fY, fX+fW, fY+fH); }
  /* -- Set vertex bounds and return it ------------------------------------ */
  const QuadCoordData &FboItemSetAndGetCoord(const QuadCoordData &fTC,
    const GLfloat fML, const GLfloat fMR)
      { FboItemSetTexCoord(fTC, fML, fMR); return FboItemGetTCData(); }
  /* -- Set colour components (0xAARRGGBB) --------------------------------- */
  void FboItemSetQuadRGBAInt(const unsigned int uiColour)
    { FboItemSetQuadRGBA(UtilNormaliseEx<GLfloat, stUInt16Bits>(uiColour),
        UtilNormaliseEx<GLfloat, stUInt8Bits>(uiColour),
        UtilNormaliseEx<GLfloat>(uiColour),
        UtilNormaliseEx<GLfloat, stUInt24Bits>(uiColour)); }
  /* -- Set colour components (0xRRGGBB) ----------------------------------- */
  void FboItemSetQuadRGB(const GLfloat fRed, const GLfloat fGreen,
    const GLfloat fBlue)
      { FboItemSetQuadRed(fRed);
        FboItemSetQuadGreen(fGreen);
        FboItemSetQuadBlue(fBlue); }
  /* -- Set colour components by integer ----------------------------------- */
  void FboItemSetQuadRGBInt(const unsigned int uiColour)
    { FboItemSetQuadRGB(UtilNormaliseEx<GLfloat, stUInt16Bits>(uiColour),
        UtilNormaliseEx<GLfloat, stUInt8Bits>(uiColour),
        UtilNormaliseEx<GLfloat>(uiColour)); }
  /* -- Update red component ----------------------------------------------- */
  void FboItemSetQuadRed(const GLfloat fRed)
    { TriColData &tdT1 = FboItemGetCDataT1(), &tdT2 = FboItemGetCDataT2();
      tdT1[0] = tdT1[4] = tdT1[8] = tdT2[0] = tdT2[4] = tdT2[8] = fRed; }
  /* -- Update green component --------------------------------------------- */
  void FboItemSetQuadGreen(const GLfloat fGreen)
    { TriColData &tdT1 = FboItemGetCDataT1(), &tdT2 = FboItemGetCDataT2();
      tdT1[1] = tdT1[5] = tdT1[9] = tdT2[1] = tdT2[5] = tdT2[9] = fGreen; }
  /* -- Update blue component ---------------------------------------------- */
  void FboItemSetQuadBlue(const GLfloat fBlue)
    { TriColData &tdT1 = FboItemGetCDataT1(), &tdT2 = FboItemGetCDataT2();
      tdT1[2] = tdT1[6] = tdT1[10] = tdT2[2] = tdT2[6] = tdT2[10] = fBlue; }
  /* -- Update alpha component --------------------------------------------- */
  void FboItemSetQuadAlpha(const GLfloat fAlpha)
    { TriColData &tdT1 = FboItemGetCDataT1(), &tdT2 = FboItemGetCDataT2();
      tdT1[3] = tdT1[7] = tdT1[11] = tdT2[3] = tdT2[7] = tdT2[11] = fAlpha; }
  /* -- Set defaults ------------------------------------------------------- */
  void FboItemSetDefaults(void) { sBuffer = FboItemGetDefaultLookup(); }
  /* -- Constructor -------------------------------------------------------- */
  FboItem(void) :
    /* -- Initialisers ----------------------------------------------------- */
    sBuffer{                           // Initialise storage...
      FboItemGetDefaultLookup() }      // ...with default values
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Init with colour (from font) --------------------------------------- */
  explicit FboItem(const unsigned int uiColour) :
    /* -- Initialisers ----------------------------------------------------- */
    FboItem{}                          // Initialise default values
    /* -- Initialise colour ------------------------------------------------ */
    { FboItemSetQuadRGBAInt(uiColour); }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(FboItem)             // Suppress default functions for safety
};/* ----------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
