/* == PALETTE.HPP ========================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## This module handles palette handling and updating                   ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IPalette {                   // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace ICollector::P;         using namespace IError::P;
using namespace IFboDef::P;            using namespace IIdent::P;
using namespace IImage::P;             using namespace IImageDef::P;
using namespace ILuaLib::P;            using namespace IShaders::P;
using namespace IStd::P;               using namespace ISysUtil::P;
using namespace ITexDef::P;            using namespace IUtil::P;
using namespace Lib::OS::GlFW;
/* ------------------------------------------------------------------------- */
typedef array<FboColour, 256> PalData; // Palette data
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* ------------------------------------------------------------------------- */
class Pal :                            // Members initially public
  /* -- Base classes ------------------------------------------------------- */
  public PalData                       // Palette data class
{ /* -- Private typedefs --------------------------------------------------- */
  typedef PalData::reverse_iterator PalDataRevIt;   // Reverse iterator
  typedef PalData::iterator         PalDataIt;      // Forward iterator
  typedef PalData::const_iterator   PalDataConstIt; // Const forward iterator
  /* -- Get PalData ------------------------------------------------ */ public:
  const FboColour &GetSlotConst(const size_t stSlot) const
    { return (*this)[stSlot]; }
  FboColour &GetSlot(const size_t stSlot) { return (*this)[stSlot]; }
  /* -- Commit palette ----------------------------------------------------- */
  void Commit(void) const
    { cShaderCore->sh2D8Pal.UpdatePalette(size(),
        reinterpret_cast<const GLfloat*>(data())); }
  /* -- Set palette entry -------------------------------------------------- */
  void SetRGBA(const size_t stPos, const GLfloat fRed,
    const GLfloat fGreen, const GLfloat fBlue, const GLfloat fAlpha)
  { GetSlot(stPos) = { fRed, fGreen, fBlue, fAlpha }; }
  /* -- Set palette entry as integer --------------------------------------- */
  void SetRGBAInt(const size_t stPos, const unsigned int uiRed,
    const unsigned int uiGreen, const unsigned int uiBlue,
    const unsigned int uiAlpha)
  { GetSlot(stPos) = { uiRed, uiGreen, uiBlue, uiAlpha }; }
  /* -- Get red palette entry ---------------------------------------------- */
  GLfloat GetRed(const size_t stPos) const
    { return GetSlotConst(stPos).GetColourRed(); }
  unsigned int GetRedInt(const size_t stPos) const
    { return UtilDenormalise<unsigned int>(GetRed(stPos)); }
  /* -- Get green palette entry -------------------------------------------- */
  GLfloat GetGreen(const size_t stPos) const
    { return GetSlotConst(stPos).GetColourGreen(); }
  unsigned int GetGreenInt(const size_t stPos) const
    { return UtilDenormalise<unsigned int>(GetGreen(stPos)); }
  /* -- Get green palette entry -------------------------------------------- */
  GLfloat GetBlue(const size_t stPos) const
    { return GetSlotConst(stPos).GetColourBlue(); }
  unsigned int GetBlueInt(const size_t stPos) const
    { return UtilDenormalise<unsigned int>(GetBlue(stPos)); }
  /* -- Get alpha palette entry -------------------------------------------- */
  GLfloat GetAlpha(const size_t stPos) const
    { return GetSlotConst(stPos).GetColourAlpha(); }
  unsigned int GetAlphaInt(const size_t stPos) const
    { return UtilDenormalise<unsigned int>(GetAlpha(stPos)); }
  /* -- Set red palette entry ---------------------------------------------- */
  void SetRed(const size_t stPos, const GLfloat fRed)
    { GetSlot(stPos).SetColourRed(fRed); }
  void SetRedInt(const size_t stPos, const unsigned int uiRed)
    { GetSlot(stPos).SetColourRedInt(uiRed); }
  /* -- Set green palette entry -------------------------------------------- */
  void SetGreen(const size_t stPos, const GLfloat fGreen)
    { GetSlot(stPos).SetColourGreen(fGreen); }
  void SetGreenInt(const size_t stPos, const unsigned int uiGreen)
    { GetSlot(stPos).SetColourGreenInt(uiGreen); }
  /* -- Set blue palette entry --------------------------------------------- */
  void SetBlue(const size_t stPos, const GLfloat fBlue)
    { GetSlot(stPos).SetColourBlue(fBlue); }
  void SetBlueInt(const size_t stPos, const unsigned int uiBlue)
    { GetSlot(stPos).SetColourBlueInt(uiBlue); }
  /* -- Set alpha palette entry -------------------------------------------- */
  void SetAlpha(const size_t stPos, const GLfloat fAlpha)
    { GetSlot(stPos).SetColourAlpha(fAlpha); }
  void SetAlphaInt(const size_t stPos, const unsigned int uiAlpha)
    { GetSlot(stPos).SetColourAlphaInt(uiAlpha); }
  /* -- Size as signed size_t --------------------------------------------- */
  ssize_t Size(void) const { return static_cast<ssize_t>(size()); }
  ssize_t SizeM1(void) const { return Size()-1; }
  ssize_t SizeN(void) const { return -Size(); }
  /* -- Shift limited palette entries backwards ---------------------------- */
  void ShiftBck(const ssize_t stBegin, const ssize_t stEnd,
    const ssize_t stRot)
  { // Get starting position and rotate backwards
    const PalDataRevIt pdriStart{ rbegin() + (SizeM1() - stEnd) };
    StdRotate(seq, pdriStart, pdriStart + stRot,
      rbegin() + (Size() - stBegin));
  }
  /* -- Shift limited palette entries forwards ----------------------------- */
  void ShiftFwd(const ssize_t stBegin, const ssize_t stEnd,
    const ssize_t stRot)
  { // Get starting position and rotate forwards
    const PalDataIt pdiStart{ begin() + stBegin };
    StdRotate(seq, pdiStart, pdiStart + stRot, begin() + stEnd + 1);
  }
  /* -- Shift palette entries backwards or forwards ------------------------ */
  void Shift(const ssize_t stBegin, const ssize_t stLimit,
    const ssize_t stRot)
  { // Shift backwards?
    if(stRot < 0) ShiftBck(stBegin, stLimit, -stRot);
    // Shift forwards?
    else if(stRot > 0) ShiftFwd(stBegin, stLimit, stRot);
  }
  /* -- Copy a from other palette ------------------------------------------ */
  void Copy(const size_t stDstPos, const PalData &pdSrc, const size_t stSrcPos,
    const size_t stSrcCount)
  { // Get source data position and then copy it to output
    const PalDataConstIt pdciIt{ pdSrc.cbegin() + stSrcPos };
    StdCopy(par_unseq, pdciIt, pdciIt + stSrcCount, begin() + stDstPos);
  }
  /* -- Fill with specified value ------------------------------------------ */
  void Fill(const size_t stIndex, const size_t stCount,
    const GLfloat fRed=0.0f, const GLfloat fGreen=0.0f,
    const GLfloat fBlue=0.0f, const GLfloat fAlpha=0.0f)
  { // Get start and fill in the array
    const PalDataIt pdiStart{ begin() + stIndex };
    StdFill(par_unseq, pdiStart, pdiStart + stCount,
      FboColour{ fRed, fGreen, fBlue, fAlpha }); }
  /* ----------------------------------------------------------------------- */
  Pal(void) :                          // No parameters
    /* -- Initialisers ----------------------------------------------------- */
    PalData{}                          // Blank palette
    /* -- Code ------------------------------------------------------------- */
    { }                                // Do nothing else
  /* -- Copy constructor from other palette data --------------------------- */
  explicit Pal(const PalData &pdOther) :
    /* -- Initialisers ----------------------------------------------------- */
    PalData{ pdOther }                 // Copy palette data
    /* -- Code ------------------------------------------------------------- */
    { }                                // Do nothing else
};/* ----------------------------------------------------------------------- */
CTOR_BEGIN(Palettes, Palette, CLHelperUnsafe,
  const Pal palDefault;                // Default palette
) /* ----------------------------------------------------------------------- */
CTOR_MEM_BEGIN_CSLAVE(Palettes, Palette, ICHelperUnsafe),
  /* -- Base classes ------------------------------------------------------- */
  public Lockable,                     // Lua garbage collector instruction
  public Ident,                        // Identifier
  public Pal                           // Base Palette class
{ /* -- Init name only --------------------------------------------- */ public:
  void Init(const string &strName) { IdentSet(strName); }
  /* -- Init name and data from another palette ---------------------------- */
  void Init(const string &strName, const Pal &palOther)
    { Init(strName); Pal::operator=(palOther); }
  /* -- Load palette from image -------------------------------------------- */
  void Init(const string &strName, const Image &imOther)
  { // Set name from image
    IdentSet(strName);
    // Throw error if image doesn't have a palette
    if(imOther.IsNotPalette())
      XC("Image does not have a palette!",
         "Palette", strName, "Image", imOther.IdentGet());
    // Must have two images
    if(imOther.GetSlotCount() != 2)
      XC("Image must must have two slots!",
         "Palette", strName, "Image", imOther.IdentGet(),
         "Slots",   imOther.GetSlotCount());
    // Get last item
    const ImageSlot &isPalette = imOther.GetSlotsConst().back();
    // Dimensions must be valid
    if(isPalette.DimIsNotWidthSet() || isPalette.DimGetWidth() > size())
      XC("Image palette has invalid count!",
         "Palette", strName,                 "Image",   imOther.IdentGet(),
         "Actual",  isPalette.DimGetWidth(), "Maximum", size());
    // Make sure palette entries are the same depth
    if(isPalette.DimGetHeight() != BY_RGB)
      XC("Image palette has invalid byte count!",
         "Palette", strName,                  "Image",    imOther.IdentGet(),
         "Actual",  isPalette.DimGetHeight(), "Required", BY_RGB);
    // Step through our palette and set values to zero
    for(size_t stIndex = 0; stIndex < isPalette.DimGetWidth(); ++stIndex)
    { // Calculate position and set the new value
      const size_t stPos = stIndex * BY_RGB;
      GetSlot(stIndex) = {
        isPalette.MemReadInt<uint8_t>(stPos+sizeof(uint16_t)),
        isPalette.MemReadInt<uint8_t>(stPos+sizeof(uint8_t)),
        isPalette.MemReadInt<uint8_t>(stPos)
      };
    } // Fill the rest of the entries if we need to
    Fill(isPalette.DimGetWidth(), size() - isPalette.DimGetWidth());
  }
  /* -- Default constructor ------------------------------------------------ */
  Palette(void) :                      // No parameters
    /* -- Initialisers ----------------------------------------------------- */
    ICHelperPalette{ cPalettes, this },// Register the object in collector
    IdentCSlave{ cParent->CtrNext() }  // Initialise identification number
    /* -- Code  ------------------------------------------------------------ */
    { }                                // No code
};/* ----------------------------------------------------------------------- */
CTOR_END(Palettes, Palette, PALETTE,,,,
  palDefault{{{ // Init default palette to VGA
/* -- 0-15 ----------------------------------------------------------------- */
{.0f,.0f,.0f,.0f}, {   0,   2, 170 }, {  20, 170,   0 }, {   0, 170, 170 },
{ 170,   0,   3 }, { 170,   0, 170 }, { 170,  85,   0 }, { 170, 170, 170 },
{  85,  85,  85 }, {  85,  85, 255 }, {  85, 255,  85 }, {  85, 255, 255 },
{ 255,  85,  85 }, { 255,  85, 255 }, { 255, 255,  85 }, { 255, 255, 255 },
/* -- 16-31 ---------------------------------------------------------------- */
{   0,   0,   0 }, {  16,  16,  16 }, {  32,  32,  32 }, {  53,  53,  53 },
{  69,  69,  69 }, {  85,  85,  85 }, { 101, 101, 101 }, { 117, 117, 117 },
{ 138, 138, 138 }, { 154, 154, 154 }, { 170, 170, 170 }, { 186, 186, 186 },
{ 202, 202, 202 }, { 223, 223, 223 }, { 239, 239, 239 }, { 255, 255, 255 },
/* -- 32-47 ---------------------------------------------------------------- */
{   0,   4, 255 }, {  65,   4, 255 }, { 130,   3, 255 }, { 190,   2, 255 },
{ 253,   0, 255 }, { 254,   0, 190 }, { 255,   0, 130 }, { 255,   0,  65 },
{ 255,   0,   8 }, { 255,  65,   5 }, { 255, 130,   0 }, { 255, 190,   0 },
{ 255, 255,   0 }, { 190, 255,   0 }, { 130, 255,   0 }, {  65, 255,   1 },
/* -- 48-63 ---------------------------------------------------------------- */
{  36, 255,   0 }, {  34, 255,  66 }, {  29, 255, 130 }, {  18, 255, 190 },
{   0, 255, 255 }, {   0, 190, 255 }, {   1, 130, 255 }, {   0,  65, 255 },
{ 130, 130, 255 }, { 158, 130, 255 }, { 190, 130, 255 }, { 223, 130, 255 },
{ 253, 130, 255 }, { 254, 130, 223 }, { 255, 130, 190 }, { 255, 130, 158 },
/* -- 64-79 ---------------------------------------------------------------- */
{ 255, 130, 130 }, { 255, 158, 130 }, { 255, 190, 130 }, { 255, 223, 130 },
{ 255, 255, 130 }, { 223, 255, 130 }, { 190, 255, 130 }, { 158, 255, 130 },
{ 130, 255, 130 }, { 130, 255, 158 }, { 130, 255, 190 }, { 130, 255, 223 },
{ 130, 255, 255 }, { 130, 223, 255 }, { 130, 190, 255 }, { 130, 158, 255 },
/* -- 80-95 ---------------------------------------------------------------- */
{ 186, 186, 255 }, { 202, 186, 255 }, { 223, 186, 255 }, { 239, 186, 255 },
{ 254, 186, 255 }, { 254, 186, 239 }, { 255, 186, 223 }, { 255, 186, 202 },
{ 255, 186, 186 }, { 255, 202, 186 }, { 255, 223, 186 }, { 255, 239, 186 },
{ 255, 255, 186 }, { 239, 255, 186 }, { 223, 255, 186 }, { 202, 255, 187 },
/* -- 96-111 --------------------------------------------------------------- */
{ 186, 255, 186 }, { 186, 255, 202 }, { 186, 255, 223 }, { 186, 255, 239 },
{ 186, 255, 255 }, { 186, 239, 255 }, { 186, 223, 255 }, { 186, 202, 255 },
{   1,   1, 113 }, {  28,   1, 113 }, {  57,   1, 113 }, {  85,   0, 113 },
{ 113,   0, 113 }, { 113,   0,  85 }, { 113,   0,  57 }, { 113,   0,  28 },
/* -- 112-127 -------------------------------------------------------------- */
{ 113,   0,   1 }, { 113,  28,   1 }, { 113,  57,   0 }, { 113,  85,   0 },
{ 113, 113,   0 }, {  85, 113,   0 }, {  57, 113,   0 }, {  28, 113,   0 },
{   9, 113,   0 }, {   9, 113,  28 }, {   6, 113,  57 }, {   3, 113,  85 },
{   0, 113, 113 }, {   0,  85, 113 }, {   0,  57, 113 }, {   0,  28, 113 },
/* -- 128-143 -------------------------------------------------------------- */
{  57,  57, 113 }, {  69,  57, 113 }, {  85,  57, 113 }, {  97,  57, 113 },
{ 113,  57, 113 }, { 113,  57,  97 }, { 113,  57,  85 }, { 113,  57,  69 },
{ 113,  57,  57 }, { 113,  69,  57 }, { 113,  85,  57 }, { 113,  97,  57 },
{ 113, 113, 57 },  {  97, 113,  57 }, {  85, 113,  57 }, {  69, 113,  58 },
/* -- 144-159 -------------------------------------------------------------- */
{  57, 113,  57 }, {  57, 113,  69 }, {  57, 113,  85 }, {  57, 113,  97 },
{  57, 113, 113 }, {  57,  97, 113 }, {  57,  85, 113 }, {  57,  69, 114 },
{  81,  81, 113 }, {  89,  81, 113 }, {  97,  81, 113 }, { 105,  81, 113 },
{ 113,  81, 113 }, { 113,  81, 105 }, { 113,  81,  97 }, { 113,  81,  89 },
/* -- 160-175 -------------------------------------------------------------- */
{ 113,  81,  81 }, { 113,  89,  81 }, { 113,  97,  81 }, { 113, 105,  81 },
{ 113, 113,  81 }, { 105, 113,  81 }, {  97, 113,  81 }, {  89, 113,  81 },
{  81, 113,  81 }, {  81, 113,  90 }, {  81, 113,  97 }, {  81, 113, 105 },
{  81, 113, 113 }, {  81, 105, 113 }, {  81,  97, 113 }, {  81,  89, 113 },
/* -- 176-191 -------------------------------------------------------------- */
{   0,   0,  66 }, {  17,   0,  65 }, {  32,   0,  65 }, {  49,   0,  65 },
{  65,   0,  65 }, {  65,   0,  50 }, {  65,   0,  32 }, {  65,   0,  16 },
{  65,   0,   0 }, {  65,  16,   0 }, {  65,  32,   0 }, {  65,  49,   0 },
{  65,  65,   0 }, {  49,  65,   0 }, {  32,  65,   0 }, {  16,  65,   0 },
/* -- 192-207 -------------------------------------------------------------- */
{   3,  65,   0 }, {   3,  65,  16 }, {   2,  65,  32 }, {   1,  65,  49 },
{   0,  65,  65 }, {   0,  49,  65 }, {   0,  32,  65 }, {   0,  16,  65 },
{  32,  32,  65 }, {  40,  32,  65 }, {  49,  32,  65 }, {  57,  32,  65 },
{  65,  32,  65 }, {  65,  32,  57 }, {  65,  32,  49 }, {  65,  32,  40 },
/* -- 208-223 -------------------------------------------------------------- */
{  65,  32,  32 }, {  65,  40,  32 }, {  65,  49,  32 }, {  65,  57,  33 },
{  65,  65,  32 }, {  57,  65,  32 }, {  49,  65,  32 }, {  40,  65,  32 },
{  32,  65,  32 }, {  32,  65,  40 }, {  32,  65,  49 }, {  32,  65,  57 },
{  32,  65,  65 }, {  32,  57,  65 }, {  32,  49,  65 }, {  32,  40,  65 },
/* -- 224-239 -------------------------------------------------------------- */
{  45,  45,  65 }, {  49,  45,  65 }, {  53,  45,  65 }, {  61,  45,  65 },
{  65,  45,  65 }, {  65,  45,  61 }, {  65,  45,  53 }, {  65,  45,  49 },
{  65,  45,  45 }, {  65,  49,  45 }, {  65,  53,  45 }, {  65,  61,  45 },
{  65,  65,  45 }, {  61,  65,  45 }, {  53,  65,  45 }, {  49,  65,  45 },
/* -- 240-255 -------------------------------------------------------------- */
{  45,  65,  45 }, {  45,  65,  49 }, {  45,  65,  53 }, {  45,  65,  61 },
{  45,  65,  65 }, {  45,  61,  65 }, {  45,  53,  65 }, {  45,  49,  65 },
{   0,   0,   0 }, {   0,   0,   0 }, {   0,   0,   0 }, {   0,   0,   0 },
{   0,   0,   0 }, {   0,   0,   0 }, {   0,   0,   0 }, {   0,   0,   0 }
}}})/* --------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
