/* == BIT.HPP ============================================================= **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## Helper functions to help dealing with bits inside bytes easier.     ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IBit {                       // Start of private module namespace
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* -- Reverse a byte but use 'int' math for speed -------------------------- */
template<typename IntType>
  requires StdIsIntegral<IntType>
static IntType BitReverseByte(unsigned uByte)
{ // We shall use a lookup table for this as it is faster
  uByte = ((uByte & 0x55) << 1) | ((uByte & 0xAA) >> 1);
  uByte = ((uByte & 0x33) << 2) | ((uByte & 0xCC) >> 2);
  uByte = ((uByte & 0x0F) << 4) | ((uByte & 0xF0) >> 4);
  return static_cast<IntType>(uByte & 0xFF);
}
/* -- Bits handling functions ---------------------------------------------- */
template<typename IntType>
  requires StdIsIntegral<IntType>
static IntType BitSwap4(const IntType tValue)
  { return (((tValue & 0xff) >> 4) | ((tValue & 0xff) << 4)) & 0xff; }
/* -- Converts BIT position to BYTE position ------------------------------- */
template<typename IntType>
  requires StdIsIntegral<IntType>
static IntType BitToByte(const IntType itPos) { return itPos / CHAR_BIT; }
/* -- Converts BYTE position to BIT position ------------------------------- */
template<typename IntType>
  requires StdIsIntegral<IntType>
static IntType BitFromByte(const IntType itPos) { return itPos * CHAR_BIT; }
/* -- Returns specified BIT position modulo (0-7) -------------------------- */
template<typename RIntType,            // Return integral type
         typename IntType>             // Integral type
  requires StdIsIntegral<RIntType> && StdIsIntegral<IntType>
static RIntType BitMask(const IntType itPos)
  { return static_cast<RIntType>(1 << itPos % CHAR_BIT); }
/* -- XOR at BIT position 'itPos' in 'ptDst' ------------------------------- */
template<typename PtrType,             // Pointer type (without the pointer)
         typename IntType>             // Integral type
  requires StdIsIntegral<IntType> && (!StdIsPointer<PtrType>)
static void BitSet(PtrType*const ptDst, const IntType itPos)
  { ptDst[BitToByte(itPos)] |= BitMask<PtrType,IntType>(itPos); }
/* -- Return if specified BIT at BIT position 'itPos' in 'ptDst' is set ---- */
template<typename PtrType,             // Pointer type (without the pointer)
         typename IntType>             // Integral type
  requires StdIsIntegral<IntType> && (!StdIsPointer<PtrType>)
static bool BitTest(PtrType*const ptDst, const IntType itPos)
  { return (ptDst[BitToByte(itPos)] & BitMask<PtrType,IntType>(itPos)) != 0; }
/* -- Clear the BIT at BIT position 'itPos' in 'ptDst' --------------------- */
template<typename PtrType,             // Pointer type (without the pointer)
         typename IntType>             // Integral type
  requires StdIsIntegral<IntType> && (!StdIsPointer<PtrType>)
static void BitClear(PtrType*const ptDst, const IntType itPos)
  { ptDst[BitToByte(itPos)] &= ~BitMask<PtrType,IntType>(itPos); }
/* -- Toggle the BIT at BIT position 'itPos' in 'ptDst' -------------------- */
template<typename PtrType,             // Pointer type (without the pointer)
         typename IntType>             // Integral type
  requires StdIsIntegral<IntType> && (!StdIsPointer<PtrType>)
static void BitFlip(PtrType*const ptDst, const IntType itPos)
  { ptDst[BitToByte(itPos)] ^= BitMask<PtrType,IntType>(itPos); }
/* -- Bits handling functions copying from another bit buffer -------------- */
template<typename PtrType,             // Pointer type (without the pointer)
         typename IntType>             // Integral type
  requires StdIsIntegral<IntType> && (!StdIsPointer<PtrType>)
static void BitSet2(PtrType*const ptDst, const IntType itDstPos,
  const PtrType*const ptSrc, const IntType itSrcPos)
{ ptDst[BitToByte(itDstPos)] |= ptSrc[BitToByte(itSrcPos)]; }
/* -- XOR bits from source to destination ---------------------------------- */
template<typename PtrType,             // Pointer type (without the pointer)
         typename IntType>             // Integral type
  requires StdIsIntegral<IntType> &&    // 'IntType' must be integral
         (!StdIsPointer<PtrType>)      // 'PtrType' must not be a pointer
static void BitSet2R(                  // Returns nothing
  PtrType*const ptDst,                 // Destination buffer
  const IntType itDstPos,              // Source BIT (not BYTE) position
  const PtrType*const ptSrc,           // Source buffer
  const IntType itSrcPos)              // Destination BIT (not BYTE) position
{ ptDst[BitToByte(itDstPos)] |=
    BitReverseByte<PtrType>(ptSrc[BitToByte(itSrcPos)]); }
/* -- Unused --------------------------------------------------------------- */
// template<typename PtrType, typename IntType>
//   static bool BitTest2(PtrType*const ptDst, const IntType itDstPos,
//     const PtrType*const ptSrc, const IntType itSrcPos)
//       { return ptDst[BitToByte(itDstPos)] &
//                ptSrc[BitToByte(itSrcPos)]; }
// template<typename PtrType, typename IntType>
//   static void BitClear2(PtrType*const ptDst, const IntType itDstPos,
//     const PtrType*const ptSrc, const IntType itSrcPos)
//      { ptDst[BitToByte(itDstPos)] &= ~ptSrc[BitToByte(itSrcPos)]; }
// template<typename PtrType, typename IntType>
//   static void BitFlip2(PtrType*const ptDst, const IntType itDstPos,
//     const PtrType*const ptSrc, const IntType itSrcPos)
//       { ptDst[BitToByte(itDstPos)] ^= ptSrc[BitToByte(itSrcPos)]; }
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
