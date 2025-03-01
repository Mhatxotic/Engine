/* == LLCOMMON.HPP ========================================================= **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## Defines common helper classes used for querying user parameters in  ## **
** ## Lua. This file is invoked by 'lualib.hpp'.                          ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace Common {                     // Common namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IStd::P;               using namespace Lib::OS::GlFW::Types;
using namespace Lib::OpenAL::Types;
/* -- Read a LString ------------------------------------------------------- */
struct AgLCString { size_t stB; const char *cpD;
  explicit AgLCString(lua_State*const lS, const int iArg) :
    stB(0), cpD{LuaUtilGetLStr<char>(lS, iArg, stB)}{} };
/* -- Get string (empty allowed) ------------------------------------------- */
struct AgString { const string strString;
  const string &operator()(void) const { return strString; }
  operator const string&(void) const { return operator()(); }
  explicit AgString(lua_State*const lS, const int iArg) :
    strString{LuaUtilGetCppStr(lS, iArg)}{} };
/* -- Get non-empty string ------------------------------------------------- */
struct AgNeString { const string strNeString;
  const string &operator()(void) const { return strNeString; }
  operator const string&(void) const { return operator()(); }
  explicit AgNeString(lua_State*const lS, const int iArg) :
    strNeString{LuaUtilGetCppStrNE(lS, iArg)}{} };
/* -- Get Valid filename --------------------------------------------------- */
struct AgFilename { const string strFilename;
  const string &operator()(void) const { return strFilename; }
  operator const string&(void) const { return operator()(); }
  explicit AgFilename(lua_State*const lS, const int iArg) :
    strFilename{LuaUtilGetCppFile(lS, iArg)}{} };
/* -- Create class template ------------------------------------------------ */
template<class ClassType>struct ArClass { ClassType &ctClassRef;
  ClassType &operator()(void) const { return ctClassRef; }
  ClassType *operator&(void) const { return &operator()(); }
  operator ClassType&(void) const { return operator()(); }
  explicit ArClass(ClassType &ctNClassRef) : ctClassRef(ctNClassRef){} };
/* -- Read a generic pointer ----------------------------------------------- */
template<typename PtrType>struct AgPointer { const PtrType*const ptAddr;
  const PtrType* operator()(void) const { return ptAddr; }
  operator const PtrType*(void) const { return operator()(); }
  explicit AgPointer(const PtrType*const ptNAddr) : ptAddr(ptNAddr){} };
/* -- Read a C-String ------------------------------------------------------ */
template<typename PtrType>struct AgCString : public AgPointer<PtrType> {
  explicit AgCString(lua_State*const lS, const int iArg) :
    AgPointer<PtrType>{LuaUtilGetStr<PtrType>(lS, iArg)}{} };
/* -- Different types we can use ------------------------------------------- */
typedef AgCString<char> AgCStringChar;
/* -- Storage for a standard integral value -------------------------------- */
template<typename AnyType>struct AgIntegral { const AnyType atValue;
  AnyType operator()(void) const { return atValue; }
  operator AnyType(void) const { return operator()(); }
  explicit AgIntegral(const AnyType atNValue) : atValue(atNValue){} };
/* -- Read flags ----------------------------------------------------------- */
template<typename FlagsType>struct AgFlags : public AgIntegral<FlagsType> {
  explicit AgFlags(lua_State*const lS, const int iArg, const FlagsType ftMask):
    AgIntegral<FlagsType>{LuaUtilGetFlags<FlagsType>(lS, iArg, ftMask)}{} };
/* -- Read a generic boolean ----------------------------------------------- */
struct AgBoolean : public AgIntegral<bool> {
  explicit AgBoolean(lua_State*const lS, const int iArg) :
    AgIntegral{LuaUtilGetBool(lS, iArg)}{} };
/* -- Read a generic number ------------------------------------------------ */
template<typename NumType>struct AgNumber : public AgIntegral<NumType> {
  explicit AgNumber(lua_State*const lS, const int iArg) :
    AgIntegral<NumType>{LuaUtilGetNum<NumType>(lS, iArg)}{} };
/* -- Read a generic number in the specified range ------------------------- */
template<typename NumType>struct AgNumberL : public AgIntegral<NumType> {
  explicit AgNumberL(lua_State*const lS, const int iArg,
    const NumType ntMin) :
      AgIntegral<NumType>{LuaUtilGetNumL<NumType>(lS, iArg, ntMin)}{} };
/* -- Read a generic number in the specified range ------------------------- */
template<typename NumType>struct AgNumberLG : public AgIntegral<NumType> {
  explicit AgNumberLG(lua_State*const lS, const int iArg,
    const NumType ntMin, const NumType ntMax) :
      AgIntegral<NumType>{
        LuaUtilGetNumLG<NumType>(lS, iArg, ntMin, ntMax)}{} };
/* -- Read a generic number in the specified range ------------------------- */
template<typename NumType>struct AgNumberLGE : public AgIntegral<NumType> {
  explicit AgNumberLGE(lua_State*const lS, const int iArg,
    const NumType ntMin, const NumType ntMax) :
      AgIntegral<NumType>{
        LuaUtilGetNumLGE<NumType>(lS, iArg, ntMin, ntMax)}{} };
/* -- Read a normalised generic number ------------------------------------- */
template<typename NumType>struct AgNumberN : public AgIntegral<NumType> {
  explicit AgNumberN(lua_State*const lS, const int iArg) :
      AgIntegral<NumType>{LuaUtilGetNormal<NumType>(lS, iArg)}{} };
/* -- Different types we can use ------------------------------------------- */
typedef AgNumberN<GLfloat> AgAngle;
typedef AgNumberLG<GLfloat> AgGLfloatLG;
typedef AgNumber<lua_Number> AgLuaNumber;
typedef AgNumber<double> AgDouble;
typedef AgNumber<GLfloat> AgGLfloat;
typedef AgNumberLG<ALfloat> AgALfloatLG;
typedef AgNumber<ALfloat> AgALfloat;
/* -- Read a generic integer ----------------------------------------------- */
template<typename IntType>struct AgInteger : public AgIntegral<IntType> {
  explicit AgInteger(lua_State*const lS, const int iArg) :
    AgIntegral<IntType>{LuaUtilGetInt<IntType>(lS, iArg)}{} };
/* -- Read a generic integer in the specified range ------------------------ */
template<typename IntType>struct AgIntegerLG : public AgIntegral<IntType> {
  explicit AgIntegerLG(lua_State*const lS, const int iArg,
    const IntType itMin, const IntType itMax) :
      AgIntegral<IntType>{
        LuaUtilGetIntLG<IntType>(lS, iArg, itMin, itMax)}{} };
/* -- Read a generic integer as power of two ------------------------------- */
template<typename IntType>struct AgIntegerLGP2 : public AgIntegral<IntType> {
  explicit AgIntegerLGP2(lua_State*const lS, const int iArg,
    const IntType itMin, const IntType itMax) :
      AgIntegral<IntType>{
        LuaUtilGetIntLGP2<IntType>(lS, iArg, itMin, itMax)}{} };
/* -- Read a generic integer in the specified range ------------------------ */
template<typename IntType>struct AgIntegerLEG : public AgIntegral<IntType> {
  explicit AgIntegerLEG(lua_State*const lS, const int iArg,
    const IntType itMin, const IntType itMax) :
      AgIntegral<IntType>{
        LuaUtilGetIntLEG<IntType>(lS, iArg, itMin, itMax)}{} };
/* -- Read a generic integer in the specified range ------------------------ */
template<typename IntType>struct AgIntegerLGE : public AgIntegral<IntType> {
  explicit AgIntegerLGE(lua_State*const lS, const int iArg,
    const IntType itMin, const IntType itMax) :
      AgIntegral<IntType>{
        LuaUtilGetIntLGE<IntType>(lS, iArg, itMin, itMax)}{} };
/* -- Read a generic integer in the specified range ------------------------ */
template<typename IntType>struct AgIntegerL : public AgIntegral<IntType> {
  explicit AgIntegerL(lua_State*const lS, const int iArg, const IntType itMin):
    AgIntegral<IntType>{LuaUtilGetIntL<IntType>(lS, iArg, itMin)}{} };
/* -- Different types we can use ------------------------------------------- */
typedef AgInteger<int> AgInt;
typedef AgInteger<lua_Integer> AgLuaInteger;
typedef AgInteger<size_t> AgSizeT;
typedef AgInteger<uint32_t> AgUInt32;
typedef AgInteger<uint64_t> AgUInt64;
typedef AgInteger<uint8_t> AgUInt8;
typedef AgInteger<unsigned int> AgUInt;
typedef AgIntegerL<int> AgIntL;
typedef AgIntegerL<lua_Integer> AgLuaIntegerL;
typedef AgIntegerL<size_t> AgSizeTL;
typedef AgIntegerLG<size_t> AgSizeTLG;
typedef AgIntegerLG<ssize_t> AgSSizeTLG;
typedef AgIntegerLG<unsigned int> AgUIntLG;
typedef AgIntegerLGE<int> AgIntLGE;
typedef AgIntegerLGE<size_t> AgSizeTLGE;
typedef AgIntegerLGE<ssize_t> AgSSizeTLGE;
typedef AgIntegerLGE<unsigned int> AgUIntLGE;
typedef AgInteger<GLuint> AgGLuint;
/* -- Get/Create Asset object ---------------------------------------------- */
using IAsset::P::Asset;
using IAsset::P::cAssets;
struct AgAsset : public ArClass<Asset> {
  explicit AgAsset(lua_State*const lS, const int iArg) :
    ArClass{*LuaUtilGetPtr<Asset>(lS, iArg, *cAssets)}{} };
struct AcAsset : public ArClass<Asset> {
  explicit AcAsset(lua_State*const lS) :
    ArClass{*LuaUtilClassCreate<Asset>(lS, *cAssets)}{} };
/* -- Get Ftf object ------------------------------------------------------- */
using IFtf::P::Ftf;
using IFtf::P::cFtfs;
struct AgFtf : public ArClass<Ftf>
  { explicit AgFtf(lua_State*const lS, const int iArg) :
    ArClass{*LuaUtilGetPtr<Ftf>(lS, iArg, *cFtfs)}{} };
/* -- Get Texture object --------------------------------------------------- */
using ITexture::P::Texture;
using ITexture::P::cTextures;
struct AgTexture : public ArClass<Texture>
  { explicit AgTexture(lua_State*const lS, const int iArg) :
      ArClass{*LuaUtilGetPtr<Texture>(lS, iArg, *cTextures)}{} };
/* -- Get Fbo object ------------------------------------------------------- */
using IFbo::P::Fbo;
using IFbo::P::cFbos;
struct AgFbo : public ArClass<Fbo>
  { explicit AgFbo(lua_State*const lS, const int iArg) :
      ArClass{*LuaUtilGetPtr<Fbo>(lS, iArg, *cFbos)}{} };
/* -- Get Pcm object ------------------------------------------------------- */
using IPcm::P::Pcm;
using IPcm::P::cPcms;
struct AgPcm : public ArClass<Pcm>
  { explicit AgPcm(lua_State*const lS, const int iArg) :
      ArClass{*LuaUtilGetPtr<Pcm>(lS, iArg, *cPcms)}{} };
/* -- Get Image object ----------------------------------------------------- */
using IImage::P::Image;
using IImage::P::cImages;
struct AgImage : public ArClass<Image>
  { explicit AgImage(lua_State*const lS, const int iArg) :
      ArClass{*LuaUtilGetPtr<Image>(lS, iArg, *cImages)}{} };
/* -- Get Json object ------------------------------------------------------ */
using IJson::P::Json;
using IJson::P::cJsons;
struct AgJson : public ArClass<Json> {
  explicit AgJson(lua_State*const lS, const int iArg) :
    ArClass{*LuaUtilGetPtr<Json>(lS, iArg, *cJsons)}{} };
/* -- Read a valid triangle index ------------------------------------------ */
struct AgTriangleId : public AgSizeTLGE {
  explicit AgTriangleId(lua_State*const lS, const int iArg) :
    AgSizeTLGE{lS, iArg, 0, IFboDef::P::stTrisPerQuad}{} };
/* -- Read a texture id ---------------------------------------------------- */
struct AgTextureId : public AgSizeTLGE {
  explicit AgTextureId(lua_State*const lS, const int iArg,
    const Texture &tCref) :
      AgSizeTLGE{lS, iArg, 0, tCref.GetSubCount()}{} };
/* -- Read a volume value between 0 and 1 ---------------------------------- */
struct AgVolume : public AgALfloatLG {
  explicit AgVolume(lua_State*const lS, const int iArg) :
    AgALfloatLG{lS, iArg, 0.0f, 1.0f}{} };
/* -- Read a texture filter id --------------------------------------------- */
struct AgFilterId : public AgIntegerLGE<IOgl::P::OglFilterEnum> {
  explicit AgFilterId(lua_State*const lS, const int iArg) :
    AgIntegerLGE{lS, iArg, IOgl::P::OF_N_N, IOgl::P::OF_MAX}{} };
/* -- Other types ---------------------------------------------------------- */
struct AgTextureDimension : public AgIntegerLG<GLsizei>
  { explicit AgTextureDimension(lua_State*const lS, const int iArg) :
      AgIntegerLG{lS, iArg, 1, IOgl::P::cOgl->MaxTexSize<GLsizei>()}{} };
/* ------------------------------------------------------------------------- */
}                                      // End of common namespace
/* == EoF =========================================================== EoF == */
