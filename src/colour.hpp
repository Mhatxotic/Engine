/* == COLOUR.HPP =========================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## Defines the 'Colour' class which will manage colours as floating    ## **
** ## point numbers.                                                      ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IColour {                    // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IStd::P;               using namespace IUtil::P;
using Lib::OS::GlFW::GLfloat;
typedef array<GLfloat,4> Components;   // Array of RGBA floats
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* == Colour class ========================================================= */
class Colour                           // Members initially private
{ /* -- Private typedefs --------------------------------------------------- */
  Components       cComponents;        // Red + Green + Blue + Alpha values
  /* -- Get colour array --------------------------------------------------- */
  const Components &ColourGetConst() const { return cComponents; }
  Components &ColourGet() { return cComponents; }
  /* -- Get individual colour floats ------------------------------- */ public:
  GLfloat ColourGetRed() const { return ColourGetConst()[0]; }
  GLfloat ColourGetGreen() const { return ColourGetConst()[1]; }
  GLfloat ColourGetBlue() const { return ColourGetConst()[2]; }
  GLfloat ColourGetAlpha() const { return ColourGetConst()[3]; }
  GLfloat *ColourGetMemory() { return ColourGet().data(); }
  /* -- Set individual colour floats --------------------------------------- */
  void ColourSetRed(const GLfloat fR) { ColourGet()[0] = fR; }
  void ColourSetGreen(const GLfloat fG) { ColourGet()[1] = fG; }
  void ColourSetBlue(const GLfloat fB) { ColourGet()[2] = fB; }
  void ColourSetAlpha(const GLfloat fA) { ColourGet()[3] = fA; }
  /* -- Set individual colours as integers (0-255) ------------------------- */
  void ColourSetRedInt(const unsigned int uiR)
    { ColourSetRed(UtilNormaliseEx<GLfloat>(uiR)); }
  void ColourSetGreenInt(const unsigned int uiG)
    { ColourSetGreen(UtilNormaliseEx<GLfloat>(uiG)); }
  void ColourSetBlueInt(const unsigned int uiB)
    { ColourSetBlue(UtilNormaliseEx<GLfloat>(uiB)); }
  void ColourSetAlphaInt(const unsigned int uiA)
    { ColourSetAlpha(UtilNormaliseEx<GLfloat>(uiA)); }
  /* -- Set colours by packed colours integer ------------------------------ */
  void ColourSetInt(const unsigned int uiValue)
  { // Strip bits and normalise proper clear colour from 0.0 to 1.0
    ColourSetRed(UtilNormaliseEx<GLfloat,16>(uiValue));   // 8-bits 16 to 24
    ColourSetGreen(UtilNormaliseEx<GLfloat,8>(uiValue));  // 8-bits 08 to 16
    ColourSetBlue(UtilNormaliseEx<GLfloat>(uiValue));     // 8-bits 00 to 08
    ColourSetAlpha(UtilNormaliseEx<GLfloat,24>(uiValue)); // 8-bits 24 to 32
  }
  /* -- Set colours from another colour structure -------------------------- */
  void ColourSetRed(const Colour &cRef) { ColourSetRed(cRef.ColourGetRed()); }
  void ColourSetGreen(const Colour &cRef)
    { ColourSetGreen(cRef.ColourGetGreen()); }
  void ColourSetBlue(const Colour &cRef)
    { ColourSetBlue(cRef.ColourGetBlue()); }
  void ColourSetAlpha(const Colour &cRef)
    { ColourSetAlpha(cRef.ColourGetAlpha()); }
  /* -- De-initialise colours ---------------------------------------------- */
  void ColourReset() { ColourGet().fill(-1.0f); }
  /* -- Test colour components --------------------------------------------- */
  bool ColourIsRedNotEqual(const Colour &cRef) const
    { return StdIsFloatNotEqual(ColourGetRed(), cRef.ColourGetRed()); }
  bool ColourIsGreenNotEqual(const Colour &cRef) const
    { return StdIsFloatNotEqual(ColourGetGreen(), cRef.ColourGetGreen()); }
  bool ColourIsBlueNotEqual(const Colour &cRef) const
    { return StdIsFloatNotEqual(ColourGetBlue(), cRef.ColourGetBlue()); }
  bool ColourIsAlphaNotEqual(const Colour &cRef) const
    { return StdIsFloatNotEqual(ColourGetAlpha(), cRef.ColourGetAlpha()); }
  /* -- Cast to colours of a different type -------------------------------- */
  template<typename IntType,
    class ArrayType=array<IntType, sizeof(Components) / sizeof(GLfloat)>>
      const ArrayType ColourCast() const
  { return { UtilDenormalise<IntType>(ColourGetRed()),
             UtilDenormalise<IntType>(ColourGetGreen()),
             UtilDenormalise<IntType>(ColourGetBlue()),
             UtilDenormalise<IntType>(ColourGetAlpha()) }; }
  /* -- Set colour from normalised values ---------------------------------- */
  void ColourSet(const GLfloat fR, const GLfloat fG, const GLfloat fB,
    const GLfloat fA) { ColourSetRed(fR); ColourSetGreen(fG);
                        ColourSetBlue(fB); ColourSetAlpha(fA); }
  /* -- Set colour from another colour struct ------------------------------ */
  bool ColourSet(const Colour &cRef)
  { // Red clear colour change?
    if(ColourIsRedNotEqual(cRef))
    { // Update saved red value and other values if changed
      ColourSetRed(cRef);
      if(ColourIsGreenNotEqual(cRef)) ColourSetGreen(cRef);
      if(ColourIsBlueNotEqual(cRef)) ColourSetBlue(cRef);
      if(ColourIsAlphaNotEqual(cRef)) ColourSetAlpha(cRef);
    } // Green clear colour changed?
    else if(ColourIsGreenNotEqual(cRef))
    { // Update saved green value and other values if changed
      ColourSetGreen(cRef);
      if(ColourIsBlueNotEqual(cRef)) ColourSetBlue(cRef);
      if(ColourIsAlphaNotEqual(cRef)) ColourSetAlpha(cRef);
    } // Blue clear colour changed?
    else if(ColourIsBlueNotEqual(cRef))
    { // Update saved blue value and other values if changed
      ColourSetBlue(cRef);
      if(ColourIsAlphaNotEqual(cRef)) ColourSetAlpha(cRef);
    } // Update alpha if cahnged
    else if(ColourIsAlphaNotEqual(cRef)) ColourSetAlpha(cRef);
    // No value was changed so return
    else return false;
    // Commit the new viewport
    return true;
  }
  /* -- Init constructor with RGBA ints ------------------------------------ */
  Colour(const unsigned int uiR, const unsigned int uiG,
         const unsigned int uiB, const unsigned int uiA) :
    /* -- Initialisers ----------------------------------------------------- */
    cComponents{UtilNormaliseEx<GLfloat>(uiR), // Init red component
                UtilNormaliseEx<GLfloat>(uiG), // Init green component
                UtilNormaliseEx<GLfloat>(uiB), // Init blue component
                UtilNormaliseEx<GLfloat>(uiA)} // Init alpha component
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- Init constructor with RGB ints ------------------------------------- */
  Colour(const unsigned int uiR, const unsigned int uiG,
         const unsigned int uiB) :
    /* -- Initialisers ----------------------------------------------------- */
    cComponents{UtilNormaliseEx<GLfloat>(uiR), // Init red component
                UtilNormaliseEx<GLfloat>(uiG), // Init green component
                UtilNormaliseEx<GLfloat>(uiB), // init blue component
                1.0f}                          // Opaque alpha
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- Init constructor with RGBA bytes ----------------------------------- */
  Colour(const uint8_t ucR, const uint8_t ucG,
         const uint8_t ucB, const uint8_t ucA) :
    /* -- Initialisers ----------------------------------------------------- */
    cComponents{UtilNormalise<GLfloat>(ucR), // Init red component
                UtilNormalise<GLfloat>(ucG), // Init green component
                UtilNormalise<GLfloat>(ucB), // Init blue component
                UtilNormalise<GLfloat>(ucA)} // Init alpha component
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- Init constructor --------------------------------------------------- */
  Colour(const GLfloat fR, const GLfloat fG, const GLfloat fB,
    const GLfloat fA) :
    /* -- Initialisers ----------------------------------------------------- */
    cComponents{ fR, fG, fB, fA }      // Copy intensity over
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- Default constructor ------------------------------------------------ */
  Colour() :
    /* -- Initialisers ----------------------------------------------------- */
    cComponents{ 1.0f, 1.0f, 1.0f, 1.0f } // Set full Re/Gr/Bl/Alpha intensity
    /* -- No code ---------------------------------------------------------- */
    {}
};/* ----------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
