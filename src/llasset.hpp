/* == LLASSET.HPP ========================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## Defines the 'Asset' namespace and methods for the guest to use in   ## **
** ## Lua. This file is invoked by 'lualib.hpp'.                          ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ========================================================================= **
** ######################################################################### **
** ========================================================================= */
// % Asset
/* ------------------------------------------------------------------------- */
// ! The Asset class allows the programmer to load files from disk for personal
// ! use and manipulation of that data with codecs or manually by the
// ! programmer.
/* ========================================================================= */
namespace LLAsset {                    // Asset namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IAsset::P;             using namespace IASync::P;
using namespace IFStream::P;           using namespace ILuaCode::P;
using namespace IStd::P;               using namespace IUtil::P;
using namespace Common;
/* ========================================================================= **
** ######################################################################### **
** ## Archive common helper classes                                       ## **
** ######################################################################### **
** -- Read Asset and position ---------------------------------------------- */
struct AgAsPo : public AgAsset { const size_t stP;
  explicit AgAsPo(lua_State*const lS) : AgAsset{lS, 1},
     stP{LuaUtilGetInt<decltype(stP)>(lS, 2)}{} };
/* -- Read a number -------------------------------------------------------- */
template<typename AnyType>struct AgNumValue { const AnyType atV;
  explicit AgNumValue(lua_State*const lS, const int iArg) :
    atV{LuaUtilGetNum<AnyType>(lS, iArg)}{} };
/* -- Read a integer ------------------------------------------------------- */
template<typename AnyType>struct AgIntValue { const AnyType atV;
  explicit AgIntValue(lua_State*const lS, const int iArg) :
    atV{LuaUtilGetInt<AnyType>(lS, iArg)}{} };
/* -- Read a position or size ---------------------------------------------- */
struct AgBytes { const size_t stB;
  explicit AgBytes(lua_State*const lS) :
    stB{LuaUtilGetInt<decltype(stB)>(lS, 3)}{} };
/* -- Read Asset, position, bytes and integer value------------------------- */
template<typename AnyType>struct AgAsPoByVi :
  public AgAsPo, AgBytes, AgIntValue<AnyType> {
    explicit AgAsPoByVi(lua_State*const lS) :
      AgAsPo{lS}, AgBytes{lS}, AgIntValue<AnyType>{lS, 4}{} };
/* -- Read Asset, position, bytes and number value ------------------------- */
template<typename AnyType>struct AgAsPoByVn :
  public AgAsPo, AgBytes, AgNumValue<AnyType> {
    explicit AgAsPoByVn(lua_State*const lS) :
      AgAsPo{lS}, AgBytes{lS}, AgNumValue<AnyType>{lS, 4}{} };
/* -- Read Asset, position and integer value-------------------------------- */
template<typename AnyType>struct AgAsPoVi :
  public AgAsPo, AgIntValue<AnyType> {
    explicit AgAsPoVi(lua_State*const lS) :
      AgAsPo{lS}, AgIntValue<AnyType>{lS, 3}{} };
/* -- Read Asset, position and number value -------------------------------- */
template<typename AnyType>struct AgAsPoVn :
  public AgAsPo, AgNumValue<AnyType> {
    explicit AgAsPoVn(lua_State*const lS) :
      AgAsPo{lS}, AgNumValue<AnyType>{lS, 3}{} };
/* -- Read asset flags argument -------------------------------------------- */
struct AgAssetFlags : public AgFlags<AssetFlagsConst> {
  explicit AgAssetFlags(lua_State*const lS, const int iArg) :
    AgFlags{lS, iArg, CD_MASK}{} };
/* -- Get Valid directory allowing the dot --------------------------------- */
struct AgDirectory { const string strFilename;
  const string &operator()(void) const { return strFilename; }
  operator const string&(void) const { return operator()(); }
  explicit AgDirectory(lua_State*const lS, const int iArg) :
    strFilename{LuaUtilGetCppDir(lS, iArg)}{} };
/* ========================================================================= **
** ######################################################################### **
** ## Asset:* member functions                                            ## **
** ######################################################################### **
** ========================================================================= */
// $ Asset:Destroy
// ? Frees the internal memory allocated to the asset. The object will no
// ? longer be useable after this call and an error will be generated if
// ? accessed.
/* ------------------------------------------------------------------------- */
LLFUNC(Destroy, 0, LuaUtilClassDestroyChecked<Asset>(lS, cAssets))
/* ========================================================================= */
// $ Asset:Destroyed
// < Destroyed:boolean=If the Asset is destroyed
// ? Returns if the Asset is destroyed.
/* ------------------------------------------------------------------------- */
LLFUNC(Destroyed, 1, LuaUtilPushVar(lS, LuaUtilIsClassDestroyed(lS, cAssets)))
/* ========================================================================= */
// $ Asset:Fill
// > Position:integer=Starting position to start filling at.
// > Bytes:integer=Number of bytes to fill.
// > Value:integer|number=The value to fill with.
// ? Fills the buffer with the specified value. If the value cannot fit in the
// ? specified size at the end of the allocated buffer then the remainder is
// ? still filled with as much data from the specified value as it can.
// ?
// ? Actual functions (Replace 'Fill' with one of the following)...
// ? - FF32    = Fills each float with the specified value.
// ? - FF32BE  = Same as FF32 but forces the specified value to big-endian.
// ? - FF32LE  = Same as FF32 but forces the specified value to little-endian.
// ? - FF64    = Fills each double with the specified value.
// ? - FF64BE  = Same as FF64 but forces the specified value to big-endian.
// ? - FF64LE  = Same as FF64 but forces the specified value to little-endian.
// ? - FI16    = Fills each signed word with the specified value.
// ? - FI16BE  = Same as FI16 but forces the specified value to big-endian.
// ? - FI16LE  = Same as FI16 but forces the specified value to little-endian.
// ? - FI32    = Fills each signed dword with the specified value.
// ? - FI32BE  = Same as FI32 but forces the specified value to big-endian.
// ? - FI32LE  = Same as FI32 but forces the specified value to little-endian.
// ? - FI64    = Fills each signed qword with the specified value.
// ? - FI64BE  = Same as FI64 but forces the specified value to big-endian.
// ? - FI64LE  = Same as FI64 but forces the specified value to little-endian.
// ? - FI8     = Fills each signed character at the specified position.
// ? - FU16    = Fills each unsigned word with the specified value.
// ? - FU16BE  = Same as FU16 but forces the specified value to big-endian.
// ? - FU16LE  = Same as FU16 but forces the specified value to little-endian.
// ? - FU32    = Fills each unsigned dword with the specified value.
// ? - FU32BE  = Same as FU32 but forces the specified value to big-endian.
// ? - FU32LE  = Same as FU32 but forces the specified value to little-endian.
// ? - FU64    = Fills each unsigned qword with the specified value.
// ? - FU64BE  = Same as FU64 but forces the specified value to big-endian.
// ? - FU64LE  = Same as FU64 but forces the specified value to little-endian.
// ? - FU8     = Fills each unsigned character at the specified position.
/* ------------------------------------------------------------------------- */
LLFUNCTMPL(FillI, 0, const AgAsPoByVi<IntType> aAsset{lS};
  aAsset().template MemFillEx<IntType>(aAsset.stP, aAsset.atV, aAsset.stB))
LLFUNCTMPL(FI16BE, 0, const AgAsPoByVi<IntType> aAsset{lS};
  aAsset().template MemFillEx<IntType>
    (aAsset.stP, UtilToI16BE(aAsset.atV), aAsset.stB))
LLFUNCTMPL(FI16LE, 0, const AgAsPoByVi<IntType> aAsset{lS};
  aAsset().template MemFillEx<IntType>
    (aAsset.stP, UtilToI16LE(aAsset.atV), aAsset.stB))
LLFUNCTMPL(FI32BE, 0, const AgAsPoByVi<IntType> aAsset{lS};
  aAsset().template MemFillEx<IntType>
    (aAsset.stP, UtilToI32BE(aAsset.atV), aAsset.stB))
LLFUNCTMPL(FI32LE, 0, const AgAsPoByVi<IntType> aAsset{lS};
  aAsset().template MemFillEx<IntType>
    (aAsset.stP, UtilToI32LE(aAsset.atV), aAsset.stB))
LLFUNCTMPL(FI64BE, 0, const AgAsPoByVi<IntType> aAsset{lS};
  aAsset().template MemFillEx<IntType>
    (aAsset.stP, UtilToI64BE(aAsset.atV), aAsset.stB))
LLFUNCTMPL(FI64LE, 0, const AgAsPoByVi<IntType> aAsset{lS};
  aAsset().template MemFillEx<IntType>
    (aAsset.stP, UtilToI64LE(aAsset.atV), aAsset.stB))
/* ------------------------------------------------------------------------- */
LLFUNCTMPL(FillN, 0, const AgAsPoByVn<IntType> aAsset{lS};
  aAsset().template MemFillEx<IntType>(aAsset.stP, aAsset.atV, aAsset.stB))
LLFUNC(FF32BE, 0, const AgAsPoByVn<float> aAsset{lS};
  aAsset().MemFillEx<float>(aAsset.stP, UtilToF32BE(aAsset.atV), aAsset.stB))
LLFUNC(FF32LE, 0, const AgAsPoByVn<float> aAsset{lS};
  aAsset().MemFillEx<float>(aAsset.stP, UtilToF32LE(aAsset.atV), aAsset.stB))
LLFUNC(FF64BE, 0, const AgAsPoByVn<double> aAsset{lS};
  aAsset().MemFillEx<double>(aAsset.stP, UtilToF64BE(aAsset.atV), aAsset.stB))
LLFUNC(FF64LE, 0, const AgAsPoByVn<double> aAsset{lS};
  aAsset().MemFillEx<double>(aAsset.stP, UtilToF64LE(aAsset.atV), aAsset.stB))
/* ========================================================================= */
// $ Asset:Invert
// > Position:integer=Position to invert.
// > Bits:integer=Bits to invert (I*F functions only, all bits otherwise).
// ? Flips the specified bits at the specified position.
// ?
// ? Actual functions (Replace 'Invert' with one of the following)...
// ? - I8     = Inverts bits 0-7 at the specified position.
// ? - I8F    = Inverts specified bits between 0-7 at the specified position.
// ? - I16    = Inverts bits 0-15 at the specified position.
// ? - I16F   = Inverts specified bits between 0-15 at the specified position.
// ? - I16FBE = Same as I16F but forces the specified bits to be big-endian.
// ? - I16FLE = Same as I16F but forces the specified bits to be litten-endian.
// ? - I32    = Inverts bits 0-31 at the specified position.
// ? - I32F   = Inverts specified bits between 0-32 at the specified position.
// ? - I32FBE = Same as I32F but forces the specified bits to be big-endian.
// ? - I32FLE = Same as I32F but forces the specified bits to be litten-endian.
// ? - I64    = Inverts bits 0-63 at the specified position.
// ? - I64F   = Inverts specified bits between 0-63 at the specified position.
// ? - I64FBE = Same as I64F but forces the specified bits to be big-endian.
// ? - I64FLE = Same as I64F but forces the specified bits to be litten-endian.
/* ------------------------------------------------------------------------- */
LLFUNCTMPL(Invert, 0, const AgAsPo aAsset{lS};
  aAsset().MemInvert<IntType>(aAsset.stP))
LLFUNCTMPL(InvertEx, 0, const AgAsPoVi<IntType> aAsset{lS};
  aAsset().template MemInvert<IntType>(aAsset.stP, aAsset.atV))
LLFUNC(I16FBE, 0, const AgAsPoVi<uint16_t> aAsset{lS};
  aAsset().MemInvert<uint16_t>(aAsset.stP, UtilToI16BE(aAsset.atV)))
LLFUNC(I16FLE, 0, const AgAsPoVi<uint16_t> aAsset{lS};
  aAsset().MemInvert<uint16_t>(aAsset.stP, UtilToI16LE(aAsset.atV)))
LLFUNC(I32FBE, 0, const AgAsPoVi<uint32_t> aAsset{lS};
  aAsset().MemInvert<uint32_t>(aAsset.stP, UtilToI32BE(aAsset.atV)))
LLFUNC(I32FLE, 0, const AgAsPoVi<uint32_t> aAsset{lS};
  aAsset().MemInvert<uint32_t>(aAsset.stP, UtilToI32LE(aAsset.atV)))
LLFUNC(I64FBE, 0, const AgAsPoVi<uint64_t> aAsset{lS};
  aAsset().MemInvert<uint64_t>(aAsset.stP, UtilToI64BE(aAsset.atV)))
LLFUNC(I64FLE, 0, const AgAsPoVi<uint64_t> aAsset{lS};
  aAsset().MemInvert<uint64_t>(aAsset.stP, UtilToI64LE(aAsset.atV)))
/* ========================================================================= */
// $ Asset:Read
// > Position:integer=Position to write in the array.
// < Value:integer|number=Value at the specified position.
// ? Reads the specified integer at the specified position in the array.
// ?
// ? Actual functions (replace 'Read' with one of the following)...
// ? - RF32   = Reads a 32-bit IEEE754 number from the specified position.
// ? - RF32BE = Same as RF32 but forces the return value to big-endian.
// ? - RF32LE = Same as RF32 but forces the return value to little-endian.
// ? - RF64   = Reads a 64-bit IEEE754 number from the specified position.
// ? - RF64BE = Same as RF64 but forces the return value to big-endian.
// ? - RF64LE = Same as RF64 but forces the return value to little-endian.
// ? - RI16   = Reads a 16-bit signed integer from the specified position.
// ? - RI16BE = Same as RI16 but forces the return value to big-endian.
// ? - RI16LE = Same as RI16 but forces the return value to little-endian.
// ? - RI32   = Reads a 32-bit signed integer from the specified position.
// ? - RI32BE = Same as RI32 but forces the return value to big-endian.
// ? - RI32LE = Same as RI32 but forces the return value to little-endian.
// ? - RI64   = Reads a 64-bit signed integer from the specified position.
// ? - RI64BE = Same as RI64 but forces the return value to big-endian.
// ? - RI64LE = Same as RI64 but forces the return value to little-endian.
// ? - RI8    = Reads a 8-bit signed integer from the specified position.
// ? - RU16   = Reads a 16-bit unsigned integer from the specified position.
// ? - RU16BE = Same as RU16 but forces the return value to big-endian.
// ? - RU16LE = Same as RU16 but forces the return value to little-endian.
// ? - RU32   = Reads a 32-bit unsigned integer from the specified position.
// ? - RU32BE = Same as RU32 but forces the return value to big-endian.
// ? - RU32LE = Same as RU32 but forces the return value to little-endian.
// ? - RU64   = Reads a 64-bit unsigned integer from the specified position.
// ? - RU64BE = Same as RU64 but forces the return value to big-endian.
// ? - RU64LE = Same as RU64 but forces the return value to little-endian.
// ? - RU8    = Reads a 8-bit unsigned integer from the specified position.
/* ------------------------------------------------------------------------- */
LLFUNCTMPL(ReadI, 1, const AgAsPo aAsset{lS};
  LuaUtilPushVar(lS, aAsset().template MemReadInt<IntType>(aAsset.stP)))
LLFUNCTMPL(RI16LE, 1, const AgAsPo aAsset{lS};
  LuaUtilPushVar(lS, UtilToI16LE(aAsset().
    template MemReadInt<IntType>(aAsset.stP))))
LLFUNCTMPL(RI16BE, 1, const AgAsPo aAsset{lS};
  LuaUtilPushVar(lS, UtilToI16BE(aAsset().
    template MemReadInt<IntType>(aAsset.stP))))
LLFUNCTMPL(RI32LE, 1, const AgAsPo aAsset{lS};
  LuaUtilPushVar(lS, UtilToI32LE(aAsset().
    template MemReadInt<IntType>(aAsset.stP))))
LLFUNCTMPL(RI32BE, 1, const AgAsPo aAsset{lS};
  LuaUtilPushVar(lS, UtilToI32BE(aAsset().
    template MemReadInt<IntType>(aAsset.stP))))
LLFUNCTMPL(RI64LE, 1, const AgAsPo aAsset{lS};
  LuaUtilPushVar(lS, UtilToI64LE(aAsset().
    template MemReadInt<IntType>(aAsset.stP))))
LLFUNCTMPL(RI64BE, 1, const AgAsPo aAsset{lS};
  LuaUtilPushVar(lS, UtilToI64BE(aAsset().
    template MemReadInt<IntType>(aAsset.stP))))
/* ------------------------------------------------------------------------- */
LLFUNCTMPL(ReadN, 1, const AgAsPo aAsset{lS};
  LuaUtilPushVar(lS, aAsset().template MemReadInt<IntType>(aAsset.stP)))
LLFUNC(RF32LE, 1, const AgAsPo aAsset{lS};
  LuaUtilPushVar(lS, UtilCastInt32ToFloat(UtilToI32LE(
    aAsset().MemReadInt<uint32_t>(aAsset.stP)))))
LLFUNC(RF32BE, 1, const AgAsPo aAsset{lS};
  LuaUtilPushVar(lS, UtilCastInt32ToFloat(UtilToI32BE(
    aAsset().MemReadInt<uint32_t>(aAsset.stP)))))
LLFUNC(RF64LE, 1, const AgAsPo aAsset{lS};
  LuaUtilPushVar(lS, UtilCastInt64ToDouble(UtilToI64LE(
    aAsset().MemReadInt<uint64_t>(aAsset.stP)))))
LLFUNC(RF64BE, 1, const AgAsPo aAsset{lS};
  LuaUtilPushVar(lS, UtilCastInt64ToDouble(UtilToI64BE(
    aAsset().MemReadInt<uint64_t>(aAsset.stP)))))
/* ========================================================================= */
// $ Asset:Write
// > Position:integer=Position to write the integer to in the array.
// > Value:integer|number=Value to write to the array.
// ? Writes the specified integer at the specified position in the array.
// ?
// ? Actual functions (replace 'WriteInt' with one of the following)...
// ? - RF32   = Writes a 32-bit IEEE754 number to the specified position.
// ? - RF32BE = Same as RF32 but forces the specified value to big-endian.
// ? - RF32LE = Same as RF32 but forces the specified value to little-endian.
// ? - RF64   = Writes a 64-bit IEEE754 number to the specified position.
// ? - RF64BE = Same as RF64 but forces the specified value to big-endian.
// ? - RF64LE = Same as RF64 but forces the specified value to little-endian.
// ? - WI16   = Writes a 16-bit signed integer to the specified position.
// ? - WI16BE = Same as WI16 but forces the return value to big-endian.
// ? - WI16LE = Same as WI16 but forces the return value to little-endian.
// ? - WI32   = Writes a 32-bit signed integer to the specified position.
// ? - WI32BE = Same as WI32 but forces the return value to big-endian.
// ? - WI32LE = Same as WI32 but forces the return value to little-endian.
// ? - WI64   = Writes a 64-bit signed integer to the specified position.
// ? - WI64BE = Same as WI64 but forces the return value to big-endian.
// ? - WI64LE = Same as WI64 but forces the return value to little-endian.
// ? - WI8    = Writes an 8-bit signed integer to the specified position.
// ? - WU16   = Writes a 16-bit unsigned integer to the specified position.
// ? - WU16BE = Same as WU16 but forces the return value to big-endian.
// ? - WU16LE = Same as WU16 but forces the return value to little-endian.
// ? - WU32   = Writes a 32-bit unsigned integer to the specified position.
// ? - WU32BE = Same as WU32 but forces the return value to big-endian.
// ? - WU32LE = Same as WU32 but forces the return value to little-endian.
// ? - WU64   = Writes a 64-bit unsigned integer to the specified position.
// ? - WU64BE = Same as WU64 but forces the return value to big-endian.
// ? - WU64LE = Same as WU64 but forces the return value to little-endian.
// ? - WU8    = Writes an 8-bit unsigned integer to the specified position.
/* ------------------------------------------------------------------------- */
LLFUNCTMPL(WriteI, 0, const AgAsPoVi<IntType> aAsset{lS};
  aAsset().template MemWriteInt<IntType>(aAsset.stP, aAsset.atV))
LLFUNCTMPL(WI16LE, 0, const AgAsPoVi<IntType> aAsset{lS};
  aAsset().template MemWriteIntLE<IntType>(aAsset.stP, aAsset.atV))
LLFUNCTMPL(WI16BE, 0, const AgAsPoVi<IntType> aAsset{lS};
  aAsset().template MemWriteIntBE<IntType>(aAsset.stP, aAsset.atV))
LLFUNCTMPL(WI32LE, 0, const AgAsPoVi<IntType> aAsset{lS};
  aAsset().template MemWriteIntLE<IntType>(aAsset.stP, aAsset.atV))
LLFUNCTMPL(WI32BE, 0, const AgAsPoVi<IntType> aAsset{lS};
  aAsset().template MemWriteIntBE<IntType>(aAsset.stP, aAsset.atV))
LLFUNCTMPL(WI64LE, 0, const AgAsPoVi<IntType> aAsset{lS};
  aAsset().template MemWriteIntLE<IntType>(aAsset.stP, aAsset.atV))
LLFUNCTMPL(WI64BE, 0, const AgAsPoVi<IntType> aAsset{lS};
  aAsset().template MemWriteIntBE<IntType>(aAsset.stP, aAsset.atV))
/* ------------------------------------------------------------------------- */
LLFUNCTMPL(WriteN, 0, const AgAsPoVn<IntType> aAsset{lS};
  aAsset().template MemWriteInt<IntType>(aAsset.stP, aAsset.atV))
LLFUNC(WF32BE, 0, const AgAsPoVn<float> aAsset{lS};
  aAsset().MemWriteFloatBE(aAsset.stP, aAsset.atV))
LLFUNC(WF32LE, 0, const AgAsPoVn<float> aAsset{lS};
  aAsset().MemWriteFloatLE(aAsset.stP, aAsset.atV))
LLFUNC(WF64BE, 0, const AgAsPoVn<double> aAsset{lS};
  aAsset().MemWriteDoubleBE(aAsset.stP, aAsset.atV))
LLFUNC(WF64LE, 0, const AgAsPoVn<double> aAsset{lS};
  aAsset().MemWriteDoubleLE(aAsset.stP, aAsset.atV))
/* ========================================================================= */
// $ Asset:Swap
// > Position:integer=Position to swap the bytes at
// ? Swaps the high/low order bits at the specified position.
// ?
// ? Actual functions (replace 'Swap' with one of the following)...
// ? - S8     = Swaps high/low 4-bits.
// ? - S16    = Swaps high/low 8-bits.
// ? - S32    = Swaps high/low 16-bits.
// ? - S64    = Swaps high/low 32-bits.
/* ------------------------------------------------------------------------- */
LLFUNC(S8, 0, const AgAsPo aAsset{lS}; aAsset().MemSwap8(aAsset.stP))
LLFUNC(S16, 0, const AgAsPo aAsset{lS}; aAsset().MemSwap16(aAsset.stP))
LLFUNC(S32, 0, const AgAsPo aAsset{lS}; aAsset().MemSwap32(aAsset.stP))
LLFUNC(S64, 0, const AgAsPo aAsset{lS}; aAsset().MemSwap64(aAsset.stP))
/* ========================================================================= */
// $ Asset:Resize
// > Size:integer=The new size of the array in bytes.
// ? Resizes the specified array.
/* ------------------------------------------------------------------------- */
LLFUNC(Resize, 0,
  const AgAsset aAsset{lS, 1};
  const AgSizeT aSize{lS, 2};
  aAsset().MemResize(aSize))
/* ========================================================================= */
// $ Asset:ResizeUp
// > Size:integer=The new size of the array in bytes.
// ? Resizes the specified array upwards but never downwards.
/* ------------------------------------------------------------------------- */
LLFUNC(ResizeUp, 0,
  const AgAsset aAsset{lS, 1};
  const AgSizeT aSize{lS, 2};
  aAsset().MemResizeUp(aSize))
/* ========================================================================= */
// $ Asset:Clear
// ? Same as Asset:F64 but automatically assumes zero fill.
/* ------------------------------------------------------------------------- */
LLFUNC(Clear, 0, AgAsset{lS, 1}().MemFill())
/* ========================================================================= */
// $ Asset:Reverse
// ? Completely reverses all the bytes in the array.
/* ------------------------------------------------------------------------- */
LLFUNC(Reverse, 0, AgAsset{lS, 1}().MemReverse())
/* ========================================================================= */
// $ Asset:ToString
// < Text:string=The array converted to a string.
// ? Converts the array to a string. This is binary safe but will obviously
// ? terminate at the first null character or at the end of array, whichever
// ? comes first.
/* ------------------------------------------------------------------------- */
LLFUNC(ToString, 1, LuaUtilPushVar(lS, AgAsset{lS, 1}().MemToStringSafe()))
/* ========================================================================= */
// $ Asset:ToFile
// > Filename:string=Filename to write to.
// < Bytes:integer=Number of bytes written.
// ? Dumps the array to the specified file on disk. Useful for debugging.
/* ------------------------------------------------------------------------- */
LLFUNC(ToFile, 1,
  const AgAsset aAsset{lS, 1};
  const AgFilename aFilename{lS, 2};
  LuaUtilPushVar(lS, FStream{ aFilename, FM_W_B }.
    FStreamWriteBlockSafe(aAsset)))
/* ========================================================================= */
// $ Asset:Size
// < Size:integer=Size of the array in bytes.
// ? Returns the size of the array.
/* ------------------------------------------------------------------------- */
LLFUNC(Size, 1, LuaUtilPushVar(lS, AgAsset{lS, 1}().MemSize()))
/* ========================================================================= */
// $ Asset:Find
// > Text:string=String to find in array.
// < Position:integer=Position of occurrence.
// ? Finds the first occurence of 'Text' in array from the beginning.
/* ------------------------------------------------------------------------- */
LLFUNC(Find, 1,
  const AgAsset aAsset{lS, 1};
  const AgNeString aText{lS, 2};
  LuaUtilPushVar(lS, aAsset().MemFind(aText)))
/* ========================================================================= */
// $ Asset:FindEx
// > Text:string=String to find in array.
// > Position:integer=Position to start checking for occurences.
// < Position:integer=Position of occurrence.
// ? Finds the first occurrence of 'Text' in array from the specified position.
/* ------------------------------------------------------------------------- */
LLFUNC(FindEx, 1,
  const AgAsset aAsset{lS, 1};
  const AgNeString aText{lS, 2};
  const AgSizeT aPosition{lS, 3};
  LuaUtilPushVar(lS, aAsset().MemFind(aText, aPosition)))
/* ========================================================================= */
// $ Asset:Crop
// > Position:integer=Position to start crop at.
// > Bytes:integer=Number of bytes to keep.
// < Output:Asset=A new asset block containing the cropped memory.
// ? Crops the specified array from the specified position and the specified
// ? number of bytes.
/* ------------------------------------------------------------------------- */
LLFUNC(Crop, 0,
  const AgAsPo aAsset{lS};
  const AgSizeT aBytes{lS, 3};
  aAsset().MemCrop(aAsset.stP, aBytes))
/* ========================================================================= */
// $ Asset:BitClear
// > Position:integer=Position (in bits) to clear the bit from.
// ? Clears the bit at the specified position. Remember this is the bit
// ? position and not a byte position.
/* ------------------------------------------------------------------------- */
LLFUNC(BitClear, 0, const AgAsPo aAsset{lS}; aAsset().MemBitClear(aAsset.stP))
/* ========================================================================= */
// $ Asset:BitFlip
// > Position:integer=Position (in bits) to flip the bit from.
// ? Flips the bit at the specified position. Remember this is the bit
// ? position and not a byte position.
/* ------------------------------------------------------------------------- */
LLFUNC(BitFlip, 0, const AgAsPo aAsset{lS}; aAsset().MemBitFlip(aAsset.stP))
/* ========================================================================= */
// $ Asset:BitSet
// > Position:integer=Position (in bits) to set the bit from.
// ? Sets the bit at the specified position. Remember this is the bit
// ? position and not a byte position.
/* ------------------------------------------------------------------------- */
LLFUNC(BitSet, 0, const AgAsPo aAsset{lS}; aAsset().MemBitSet(aAsset.stP))
/* ========================================================================= */
// $ Asset:BitTest
// > Position:integer=Position (in bits) to set the bit from.
// < Set:boolean=Boolean to show if the bit is set or not.
// ? Sets the bit at the specified position. Remember this is the bit
// ? position and not a byte position.
/* ------------------------------------------------------------------------- */
LLFUNC(BitState, 1, const AgAsPo aAsset{lS};
  LuaUtilPushVar(lS, aAsset().MemBitTest(aAsset.stP)))
/* ========================================================================= */
// $ Asset:Id
// < Id:integer=The id number of the Asset object.
// ? Returns the unique id of the Asset object.
/* ------------------------------------------------------------------------- */
LLFUNC(Id, 1, LuaUtilPushVar(lS, AgAsset{lS, 1}().CtrGet()))
/* ========================================================================= */
// $ Asset:Name
// < Name:string=Name of the asset.
// ? If this asset was loaded by a filename or it was set with a custom id.
// ? This function returns that name which was assigned to it.
/* ------------------------------------------------------------------------- */
LLFUNC(Name, 1, LuaUtilPushVar(lS, AgAsset{lS, 1}().IdentGet()))
/* ========================================================================= **
** ######################################################################### **
** ## Asset:* member functions structure                                  ## **
** ######################################################################### **
** ========================================================================= */
LLRSMFBEGIN                            // Asset:* member functions begin
  LLRSFUNC(BitClear),                  LLRSFUNC(BitFlip),
  LLRSFUNC(BitSet),                    LLRSFUNC(BitState),
  LLRSFUNC(Clear),                     LLRSFUNC(Crop),
  LLRSFUNC(Destroy),                   LLRSFUNC(Destroyed),
  LLRSFUNCEX(FF32,FillN<float>),       LLRSFUNC(FF32BE),
  LLRSFUNC(FF32LE),                    LLRSFUNCEX(FF64,FillN<double>),
  LLRSFUNC(FF64BE),                    LLRSFUNC(FF64LE),
  LLRSFUNCEX(FI16,FillI<int16_t>),     LLRSFUNCEX(FI16BE,FI16BE<int16_t>),
  LLRSFUNCEX(FI16LE,FI16LE<int16_t>),  LLRSFUNCEX(FI32,FillI<int32_t>),
  LLRSFUNCEX(FI32BE,FI32BE<int32_t>),  LLRSFUNCEX(FI32LE,FI32LE<int32_t>),
  LLRSFUNCEX(FI64,FillI<int64_t>),     LLRSFUNCEX(FI64BE,FI64BE<int64_t>),
  LLRSFUNCEX(FI64LE,FI64LE<int64_t>),  LLRSFUNCEX(FI8,FillI<int8_t>),
  LLRSFUNCEX(FU16,FillI<uint16_t>),    LLRSFUNCEX(FU16BE,FI16BE<uint16_t>),
  LLRSFUNCEX(FU16LE,FI16LE<uint16_t>), LLRSFUNCEX(FU32,FillI<uint32_t>),
  LLRSFUNCEX(FU32BE,FI32BE<uint32_t>), LLRSFUNCEX(FU32LE,FI32LE<uint32_t>),
  LLRSFUNCEX(FU64,FillI<uint64_t>),    LLRSFUNCEX(FU64BE,FI64BE<uint64_t>),
  LLRSFUNCEX(FU64LE,FI64LE<uint64_t>), LLRSFUNCEX(FU8,FillI<uint8_t>),
  LLRSFUNC(Find),                      LLRSFUNC(FindEx),
  LLRSFUNCEX(I16,Invert<uint16_t>),    LLRSFUNCEX(I16F,InvertEx<uint16_t>),
  LLRSFUNC(I16FBE),                    LLRSFUNC(I16FLE),
  LLRSFUNCEX(I32,Invert<uint32_t>),    LLRSFUNCEX(I32F,InvertEx<uint32_t>),
  LLRSFUNC(I32FBE),                    LLRSFUNC(I32FLE),
  LLRSFUNCEX(I64,Invert<uint64_t>),    LLRSFUNCEX(I64F,InvertEx<uint64_t>),
  LLRSFUNC(I64FBE),                    LLRSFUNC(I64FLE),
  LLRSFUNCEX(I8,Invert<uint8_t>),      LLRSFUNCEX(I8F,InvertEx<uint8_t>),
  LLRSFUNC(Id),                        LLRSFUNC(Name),
  LLRSFUNC(Resize),                    LLRSFUNC(ResizeUp),
  LLRSFUNC(Reverse),                   LLRSFUNCEX(RF32,ReadN<float>),
  LLRSFUNC(RF32BE),                    LLRSFUNC(RF32LE),
  LLRSFUNCEX(RF64,ReadN<double>),      LLRSFUNC(RF64BE),
  LLRSFUNC(RF64LE),                    LLRSFUNCEX(RI16,ReadI<int16_t>),
  LLRSFUNCEX(RI16BE,RI16BE<int16_t>),  LLRSFUNCEX(RI16LE,RI16LE<int16_t>),
  LLRSFUNCEX(RI32,ReadI<int32_t>),     LLRSFUNCEX(RI32BE,RI32BE<int32_t>),
  LLRSFUNCEX(RI32LE,RI32LE<int32_t>),  LLRSFUNCEX(RI64,ReadI<int64_t>),
  LLRSFUNCEX(RI64BE,RI64BE<int64_t>),  LLRSFUNCEX(RI64LE,RI64LE<int64_t>),
  LLRSFUNCEX(RI8,ReadI<int8_t>),       LLRSFUNCEX(RU16,ReadI<uint16_t>),
  LLRSFUNCEX(RU16BE,RI16BE<uint16_t>), LLRSFUNCEX(RU16LE,RI16LE<uint16_t>),
  LLRSFUNCEX(RU32,ReadI<uint32_t>),    LLRSFUNCEX(RU32BE,RI32BE<uint32_t>),
  LLRSFUNCEX(RU32LE,RI32LE<uint32_t>), LLRSFUNCEX(RU64,ReadI<uint64_t>),
  LLRSFUNCEX(RU64BE,RI64BE<uint64_t>), LLRSFUNCEX(RU64LE,RI64LE<uint64_t>),
  LLRSFUNCEX(RU8,ReadI<uint8_t>),      LLRSFUNC(S16),
  LLRSFUNC(S32),                       LLRSFUNC(S64),
  LLRSFUNC(S8),                        LLRSFUNC(Size),
  LLRSFUNC(ToFile),                    LLRSFUNC(ToString),
  LLRSFUNCEX(WF32,WriteN<float>),      LLRSFUNC(WF32BE),
  LLRSFUNC(WF32LE),                    LLRSFUNCEX(WF64,WriteN<double>),
  LLRSFUNC(WF64BE),                    LLRSFUNC(WF64LE),
  LLRSFUNCEX(WI16,WriteI<int16_t>),    LLRSFUNCEX(WI16BE,WI16BE<int16_t>),
  LLRSFUNCEX(WI16LE,WI16LE<int16_t>),  LLRSFUNCEX(WI32,WriteI<int32_t>),
  LLRSFUNCEX(WI32BE,WI32BE<int32_t>),  LLRSFUNCEX(WI32LE,WI32LE<int32_t>),
  LLRSFUNCEX(WI64,WriteI<int64_t>),    LLRSFUNCEX(WI64BE,WI64BE<int64_t>),
  LLRSFUNCEX(WI64LE,WI64LE<int64_t>),  LLRSFUNCEX(WI8,WriteI<int8_t>),
  LLRSFUNCEX(WU16,WriteI<uint16_t>),   LLRSFUNCEX(WU16BE,WI16BE<uint16_t>),
  LLRSFUNCEX(WU16LE,WI16LE<uint16_t>), LLRSFUNCEX(WU32,WriteI<uint32_t>),
  LLRSFUNCEX(WU32BE,WI32BE<uint32_t>), LLRSFUNCEX(WU32LE,WI32LE<uint32_t>),
  LLRSFUNCEX(WU64,WriteI<uint64_t>),   LLRSFUNCEX(WU64BE,WI64BE<uint64_t>),
  LLRSFUNCEX(WU64LE,WI64LE<uint64_t>), LLRSFUNCEX(WU8,WriteI<uint8_t>),
LLRSEND                                // Asset:* member functions end
/* ========================================================================= **
** ######################################################################### **
** ## Asset.* namespace functions                                         ## **
** ######################################################################### **
** ========================================================================= */
// $ Asset.Asset
// > Id:string=The filename of the file load
// > Data:Asset=The array to duplicate
// > Flags:integer=Special operations to perform during load
// < Object:Asset=The new duplicated data array
// ? Copies, decompresses or decrypts the specified asset on the main thread.
/* ------------------------------------------------------------------------- */
LLFUNC(Asset, 1,
  const AgNeString aIdentifier{lS, 1};
  const AgAsset aAsset{lS, 2};
  const AgAssetFlags aFlags{lS, 3};
  AcAsset{lS}().AssetInitAsset(aIdentifier, aFlags, aAsset))
/* ========================================================================= */
// $ Asset.AssetAsync
// > Id:string=The user specified identifier of the asset.
// > Data:Asset=The asset class to process.
// > Flags:integer=Special operations to perform during load.
// > ErrorFunc:function(Reason:string)=Error callback.
// > ProgressFunc:function(Cmd:integer,...)=In-progress callback.
// > SuccessFunc:function(Data:Asset)=Succession callback.
// ? Consumes, copies, decompresses or decrypts a the specified asset off the
// ? main thread.
/* ------------------------------------------------------------------------- */
LLFUNC(AssetAsync, 0,
  LuaUtilCheckParams(lS, 6);
  const AgNeString aIdentifier{lS, 1};
  const AgAsset aAsset{lS, 2};
  const AgAssetFlags aFlags{lS, 3};
  LuaUtilCheckFunc(lS, 4, 5, 6);
  AcAsset{lS}().AssetInitAsyncAsset(lS, aIdentifier, aFlags, aAsset))
/* ========================================================================= */
// $ Asset.Compile
// > Filename:string=The name of the file to parse
// < Params...:*=The parameters the function returned
// ? Compiles the specified file and returns the function
/* ------------------------------------------------------------------------- */
LLFUNC(Compile, 1, LuaCodeCompileFile(lS, AgFilename{lS, 1}))
/* ========================================================================= */
// $ Asset.CompileBlock
// > Name:string=The name of the block to help identify the the source.
// > Data:Asset=The data to execute.
// < Function:function=The function.
// ? Compiles the specified asset and returns the function.
/* ------------------------------------------------------------------------- */
LLFUNC(CompileBlock, 1,
  const AgNeString aIdentifier{lS, 1};
  const AgAsset aAsset{lS, 2};
  LuaCodeCompileBlock(lS, aAsset, aIdentifier))
/* ========================================================================= */
// $ Asset.CompileFunction
// > Debug:boolean=Include debug data?
// > Function:function=The function to compile.
// < Binary:Asset=The asset containing the compiled data.
// ? Compiles the specified Lua function and returns the binary compiled data.
/* ------------------------------------------------------------------------- */
LLFUNC(CompileFunction, 1, LuaCodeCompileFunction(lS))
/* ========================================================================= */
// $ Asset.CompileString
// > Name:string=The name of the string to help identify the the source.
// > Code:string=The block of LUA code to compile.
// < Function:function=The function
// ? Compiles the specified string and returns the function
/* ------------------------------------------------------------------------- */
LLFUNC(CompileString, 1,
  const AgNeString aIdentifier{lS, 1};
  const AgNeString aCode{lS, 2};
  LuaCodeCompileString(lS, aCode, aIdentifier))
/* ========================================================================= */
// $ Asset.Create
// > Id:string=The user specified identifier of the asset.
// > Size:integer=Number of bytes of memory to allocate.
// < Handle:Asset=Handle to the array cless.
// ? Allocates the specified amount of system memory for you to read and write
// ? to. The memory is zero'd out after allocation for security reasons.
/* ------------------------------------------------------------------------- */
LLFUNC(Create, 1,
  const AgNeString aIdentifier{lS, 1};
  const AgSizeT aBytes{lS, 2};
  AcAsset{lS}().AssetInitBlank(aIdentifier, aBytes))
/* ========================================================================= */
// $ Asset.Count
// < Count:integer=Total number of assets created.
// ? Returns the total number of asset classes currently active.
/* ------------------------------------------------------------------------- */
LLFUNC(Count, 1, LuaUtilPushVar(lS, cAssets->CollectorCount()))
/* ========================================================================= */
// $ Asset:Duplicate
// < Handle:Asset=Handle to the duplicated asset.
// ? Duplicates the specified asset. By default arrays will move their data
// ? across. This allows you to create a duplicate of the specified asset to
// ? not destory the original.
/* ------------------------------------------------------------------------- */
LLFUNC(Duplicate, 1,
  const AgAsset aAsset{lS, 1};
  AcAsset{lS}().AssetInitDuplicate(aAsset))
/* ========================================================================= */
// $ Asset.Enumerate
// > Directory:string=The directory to scan (can only be exedir or below).
// > OnlyDir:string=Only enumerate directories.
// < Entries:table=The files or directories enumerated.
// ? Enumerates the list of files from the specified directory.
/* ------------------------------------------------------------------------- */
LLFUNC(Enumerate, 1,
  const AgDirectory aDirectory{lS, 1};
  const AgBoolean aOnlyDirs{lS, 2};
  LuaUtilToTable(lS, AssetList{ aDirectory, aOnlyDirs }.ToStrSet()))
/* ========================================================================= */
// $ Asset.EnumerateEx
// > Directory:string=The directory to scan (can only be exedir or below).
// > Extension:string=The extension of the files to filter.
// > OnlyDir:string=Only enumerate directories.
// < Entries:table=The files or directories enumerated.
// ? Enumerates the list of files from the specified directory.
/* ------------------------------------------------------------------------- */
LLFUNC(EnumerateEx, 1,
  const AgDirectory aDirectory{lS, 1};
  const AgFilename aExtension{lS, 2};
  const AgBoolean aOnlyDirs{lS, 3};
  LuaUtilToTable(lS,
    AssetList{ aDirectory, aExtension, aOnlyDirs }.ToStrSet()))
/* ========================================================================= */
// $ Asset.Exec
// > CmdLine:string=The command-line to execute
// > Flags:integer=Special operations to perform during load
// > ErrorFunc:function(Reason:string)=Error callback.
// > ProgressFunc:function(Cmd:integer,...)=In-progress callback.
// > SuccessFunc:function(Data:Asset)=Succession callback.
// ? Executes the specified command-line, stores all the output and sends it
// ? for optional processing with the specified flags.
/* ------------------------------------------------------------------------- */
LLFUNC(Exec, 0,
  LuaUtilCheckParams(lS, 5);
  const AgNeString aCmdLine{lS, 1};
  const AgAssetFlags aFlags{lS, 2};
  LuaUtilCheckFunc(lS, 3, 4, 5);
  AcAsset{lS}().AssetInitAsyncCmdLine(lS, aCmdLine, aFlags))
/* ========================================================================= */
// $ Asset.ExecEx
// > CmdLine:string=The command-line to execute
// > Flags:integer=Special operations to perform during load
// > Input:Asset=The data to send to the application before waiting for output
// > ErrorFunc:function(Reason:string)=Error callback.
// > ProgressFunc:function(Cmd:integer,...)=In-progress callback.
// > SuccessFunc:function(Data:Asset)=Succession callback.
// ? Executes the specified command-line, stores all the output and sends it
// ? for optional processing with the specified flags. The 'Input' is consumed
// ? by the call and sent to the applications 'stdin' on successful execution
// ? startup.
/* ------------------------------------------------------------------------- */
LLFUNC(ExecEx, 0,
  LuaUtilCheckParams(lS, 6);
  const AgNeString aCmdLine{lS, 1};
  const AgAssetFlags aFlags{lS, 2};
  const AgAsset aAsset{lS,3};
  LuaUtilCheckFunc(lS, 4, 5, 6);
  AcAsset{lS}().AssetInitAsyncCmdLineEx(lS, aCmdLine, aFlags, aAsset))
/* ========================================================================= */
// $ Asset.File
// > Filename:string=The filename of the file to load
// > Flags:integer=Special operations to perform during load
// < Handle:Asset=The asset object
// ? Loads a file from assets in the main thread.
/* ------------------------------------------------------------------------- */
LLFUNC(File, 1,
  const AgNeString aIdentifier{lS, 1};
  const AgAssetFlags aFlags{lS, 2};
  AcAsset{lS}().AssetInitFile(aIdentifier, aFlags))
/* ========================================================================= */
// $ Asset.FileAsync
// > Filename:string=The filename to load.
// > Flags:integer=Special operations to perform during load.
// > ErrorFunc:function(Reason:string)=Error callback.
// > ProgressFunc:function(Cmd:integer,...)=In-progress callback.
// > SuccessFunc:function(Data:Asset)=Succession callback.
// ? Loads the specified file on disk or from archives off the main thread.
/* ------------------------------------------------------------------------- */
LLFUNC(FileAsync, 0,
  LuaUtilCheckParams(lS, 5);
  const AgFilename aFilename{lS, 1};
  const AgAssetFlags aFlags{lS, 2};
  LuaUtilCheckFunc(lS, 3, 4, 5);
  AcAsset{lS}().AssetInitAsyncFile(lS, aFilename, aFlags))
/* ========================================================================= */
// $ Asset.FileExists
// > Filename:string=The name of the file to check
// < Status:boolean=Is the file availabe
// ? Checks if the specified file exists and is readable. The file is checked
// ? from the executable directory first and if not found or unable to open
// ? then the function traverses archives. Use this function to make sure
// ? exceptions aren't raised when loading files with any API function.
/* ------------------------------------------------------------------------- */
LLFUNC(FileExists, 1, LuaUtilPushVar(lS, AssetExists(AgFilename{lS, 1})))
/* ========================================================================= */
// $ Asset.Parse
// > Filename:string=The name of the file to parse.
// > Returns:integer=The number of returns expected from the call.
// < Params...:*=The parameters the function returned.
// < ...Last:integer=The result code of the compilation.
// ? Compiles and executes the specified string and returns a function and how
// ? the code was (re)compiled and cached.
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(Parse)
  LuaUtilCheckParams(lS, 2);
  const AgFilename aFilename{lS, 1};
  const AgIntL aReturns{lS, 2, 0};
  LuaUtilPushVar(lS, LuaCodeExecuteFile(lS, aFilename, aReturns));
LLFUNCENDEX(1 + aReturns)
/* ========================================================================= */
// $ Asset.ParseBlock
// > Name:string=The name of the block to help identify the the source.
// > Returns:integer=The number of returns expected from the call.
// > Data:Asset=The data to execute
// < Params...:*=The parameters the function returned
// < ...Last:integer=The result code of the compilation
// ? Compiles and executes the specified asset. Prefix the name with '!' to
// ? disable caching of this code and not pollute the code cache.
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(ParseBlock)
  LuaUtilCheckParams(lS, 3);
  const AgNeString aIdentifier{lS, 1};
  const AgIntL aReturns{lS, 2, 0};
  const AgAsset aAsset{lS, 3};
  LuaUtilPushVar(lS, LuaCodeExecuteBlock(lS, aAsset, aReturns, aIdentifier));
LLFUNCENDEX(1 + aReturns)
/* ========================================================================= */
// $ Asset.ParseString
// > Name:string=The name of the string to help identify the the source.
// > Returns:integer=The number of returns expected from the call.
// > Code:string=The block of LUA code to execute.
// < Params...:*=The parameters the function returned
// < ...Last:integer=The result code of the compilation
// ? Compiles and executes the specified string. Prefix the name with '!' to
// ? disable caching of this code and not pollute the code cache.
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(ParseString)
  LuaUtilCheckParams(lS, 3);
  const AgNeString aIdentifier{lS, 1};
  const AgIntL aReturns{lS, 2, 0};
  const AgNeString aCode{lS, 3};
  LuaUtilPushVar(lS, LuaCodeExecuteString(lS, aCode, aReturns, aIdentifier));
LLFUNCENDEX(1 + aReturns)
/* ========================================================================= */
// $ Asset.String
// > Id:string=The filename of the file to load
// > Text:string=The string of the data to process
// > Flags:integer=Special operations to perform during load
// < Handle:Asset=The asset object
// ? Copies, decompresses or decrypts a string into an asset class in the
// ? main thread.
/* ------------------------------------------------------------------------- */
LLFUNC(String, 1,
  const AgNeString aIdentifier{lS, 1};
  const AgLCString aText{lS, 2};
  const AgAssetFlags aFlags{lS, 3};
  AcAsset{lS}().AssetInitPtr(aIdentifier, aFlags, aText.stB, aText.cpD))
/* ========================================================================= */
// $ Asset.WaitAsync
// ? Halts main-thread execution until all async asset events have completed
/* ------------------------------------------------------------------------- */
LLFUNC(WaitAsync, 0, cAssets->WaitAsync())
/* ========================================================================= **
** ######################################################################### **
** ## Asset.* namespace functions structure                               ## **
** ######################################################################### **
** ========================================================================= */
LLRSBEGIN                              // Asset.* namespace functions begin
  LLRSFUNC(Asset),        LLRSFUNC(AssetAsync),      LLRSFUNC(Compile),
  LLRSFUNC(CompileBlock), LLRSFUNC(CompileFunction), LLRSFUNC(CompileString),
  LLRSFUNC(Count),        LLRSFUNC(Create),          LLRSFUNC(Duplicate),
  LLRSFUNC(Enumerate),    LLRSFUNC(EnumerateEx),     LLRSFUNC(Exec),
  LLRSFUNC(ExecEx),       LLRSFUNC(File),            LLRSFUNC(FileAsync),
  LLRSFUNC(FileExists),   LLRSFUNC(Parse),           LLRSFUNC(ParseBlock),
  LLRSFUNC(ParseString),  LLRSFUNC(String),          LLRSFUNC(WaitAsync),
LLRSEND                                // Asset.* namespace functions end
/* ========================================================================= **
** ######################################################################### **
** ## Asset.* namespace constants                                         ## **
** ######################################################################### **
** ========================================================================= */
// @ Asset.Flags
// < Codes:table=The table of key/value pairs of available flags
// ? A table of loading flags available. Returned as key/value pairs. The
// ? value is a unique identifier to the flag.
/* ------------------------------------------------------------------------- */
LLRSKTBEGIN(Flags)                     // Beginning of loading flags
  LLRSKTITEM(CD_,DECODE),              LLRSKTITEM(CD_,ENCODE_AES),
  LLRSKTITEM(CD_,ENCODE_LZMA),         LLRSKTITEM(CD_,ENCODE_LZMAAES),
  LLRSKTITEM(CD_,ENCODE_RAW),          LLRSKTITEM(CD_,ENCODE_ZLIB),
  LLRSKTITEM(CD_,ENCODE_ZLIBAES),      LLRSKTITEM(CD_,LEVEL_FAST),
  LLRSKTITEM(CD_,LEVEL_FASTEST),       LLRSKTITEM(CD_,LEVEL_MODERATE),
  LLRSKTITEM(CD_,LEVEL_SLOW),          LLRSKTITEM(CD_,LEVEL_SLOWEST),
  LLRSKTITEM(CD_,NONE),
LLRSKTEND                              // End of loading flags
/* ========================================================================= */
// @ Asset.Progress
// < Codes:table=The table of key/value pairs of available progress commands
// ? A table of loading commands for the progress callback. The first paramter
// ? in that callback is one of these.
/* ------------------------------------------------------------------------- */
LLRSKTBEGIN(Progress)                  // Beginning of progress flags
  LLRSKTITEM(APC_,EXECSTART), LLRSKTITEM(APC_,EXECDATAWRITE),
  LLRSKTITEM(APC_,FILESTART),
LLRSKTEND                              // End of progress flags
/* ========================================================================= */
// @ Asset.ParseResult
// < Codes:table=The table of key/value pairs of available parse results
// ? A table containing results for parse commands.
/* ------------------------------------------------------------------------- */
LLRSKTBEGIN(ParseResult)               // Beginning of parse results
  LLRSKTITEM(LCR_,CACHED), LLRSKTITEM(LCR_,RECOMPILE),
  LLRSKTITEM(LCR_,DBERR),  LLRSKTITEM(LCR_,NOCACHE),
LLRSKTEND                              // End of parse results
/* ========================================================================= **
** ######################################################################### **
** ## Asset.* namespace constants structure                               ## **
** ######################################################################### **
** ========================================================================= */
LLRSCONSTBEGIN                         // Asset.* namespace consts begin
  LLRSCONST(Flags), LLRSCONST(Progress), LLRSCONST(ParseResult),
LLRSCONSTEND                           // Asset.* namespace consts end
/* ========================================================================= */
}                                      // End of Asset namespace
/* == EoF =========================================================== EoF == */
