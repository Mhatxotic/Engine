/* == FBOBLEND.HPP ========================================================= **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## Definition of the 'FboBlend' class that handles blending values.    ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IFboBlend {                  // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using Lib::OS::GlFW::GLenum;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* == Fbo blend class ====================================================== */
class FboBlend
{ /* -- Private variables -------------------------------------------------- */
  typedef array<GLenum,4> FboBlendStruct; // Array of four GLenum's
  FboBlendStruct   fbsBlend;             // Blend union
  /* -- Get blend values ------------------------------------------- */ public:
  GLenum FboBlendGetSrcRGB() const { return fbsBlend[0]; }
  GLenum FboBlendGetDstRGB() const { return fbsBlend[1]; }
  GLenum FboBlendGetSrcAlpha() const { return fbsBlend[2]; }
  GLenum FboBlendGetDstAlpha() const { return fbsBlend[3]; }
  GLenum *FboBlendGetMemory() { return fbsBlend.data(); }
  /* -- Set blend values by enum ------------------------------------------- */
  void FboBlendSetSrcRGB(const GLenum eSrcRGB) { fbsBlend[0] = eSrcRGB; }
  void FboBlendSetDstRGB(const GLenum eDstRGB) { fbsBlend[1] = eDstRGB; }
  void FboBlendSetSrcAlpha(const GLenum eSrcAlpha) { fbsBlend[2] = eSrcAlpha; }
  void FboBlendSetDstAlpha(const GLenum eDstAlpha) { fbsBlend[3] = eDstAlpha; }
  /* -- Set blend values by another struct --------------------------------- */
  void FboBlendSetSrcRGB(const FboBlend &fbValue)
    { FboBlendSetSrcRGB(fbValue.FboBlendGetSrcRGB()); }
  void FboBlendSetDstRGB(const FboBlend &fbValue)
    { FboBlendSetDstRGB(fbValue.FboBlendGetDstRGB()); }
  void FboBlendSetSrcAlpha(const FboBlend &fbValue)
    { FboBlendSetSrcAlpha(fbValue.FboBlendGetSrcAlpha()); }
  void FboBlendSetDstAlpha(const FboBlend &fbValue)
    { FboBlendSetDstAlpha(fbValue.FboBlendGetDstAlpha()); }
  /* -- Test blend values -------------------------------------------------- */
  bool FboBlendIsSrcRGBNotEqual(const FboBlend &fbValue) const
    { return FboBlendGetSrcRGB() != fbValue.FboBlendGetSrcRGB(); }
  bool FboBlendIsDstRGBNotEqual(const FboBlend &fbValue) const
    { return FboBlendGetDstRGB() != fbValue.FboBlendGetDstRGB(); }
  bool FboBlendIsSrcAlphaNotEqual(const FboBlend &fbValue) const
    { return FboBlendGetSrcAlpha() != fbValue.FboBlendGetSrcAlpha(); }
  bool FboBlendIsDstAlphaNotEqual(const FboBlend &fbValue) const
    { return FboBlendGetDstAlpha() != fbValue.FboBlendGetDstAlpha(); }
  /* -- Set blending algorithms -------------------------------------------- */
  bool FboBlendSet(const FboBlend &fbValue)
  {  // Source RGB changed change?
    if(FboBlendIsSrcRGBNotEqual(fbValue))
    { // Update source RGB blend value and other values if changed
      FboBlendSetSrcRGB(fbValue);
      if(FboBlendIsDstRGBNotEqual(fbValue)) FboBlendSetDstRGB(fbValue);
      if(FboBlendIsSrcAlphaNotEqual(fbValue)) FboBlendSetSrcAlpha(fbValue);
      if(FboBlendIsDstAlphaNotEqual(fbValue)) FboBlendSetDstAlpha(fbValue);
    } // Destination RGB blend changed?
    else if(FboBlendIsDstRGBNotEqual(fbValue))
    { // Update destination RGB blend value and other values if changed
      FboBlendSetDstRGB(fbValue);
      if(FboBlendIsSrcAlphaNotEqual(fbValue)) FboBlendSetSrcAlpha(fbValue);
      if(FboBlendIsDstAlphaNotEqual(fbValue)) FboBlendSetDstAlpha(fbValue);
    } // Source alpha changed?
    else if(FboBlendIsSrcAlphaNotEqual(fbValue))
    { // Update source alpha blend value and other values if changed
      FboBlendSetSrcAlpha(fbValue);
      if(FboBlendIsDstAlphaNotEqual(fbValue)) FboBlendSetDstAlpha(fbValue);
    } // Destination alpha changed?
    else if(FboBlendIsDstAlphaNotEqual(fbValue)) FboBlendSetDstAlpha(fbValue);
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
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
