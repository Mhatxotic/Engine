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
template<typename RT>static RT BitReverseByte(unsigned int uiByte)
{ // We shall use a lookup table for this as it is faster
  uiByte = ((uiByte & 0x55) << 1) | ((uiByte & 0xAA) >> 1);
  uiByte = ((uiByte & 0x33) << 2) | ((uiByte & 0xCC) >> 2);
  uiByte = ((uiByte & 0x0F) << 4) | ((uiByte & 0xF0) >> 4);
  return static_cast<RT>(uiByte & 0xFF);
}
/* -- Bits handling functions ---------------------------------------------- */
template<typename T> requires is_integral_v<T>
  static T BitSwap4(const T tValue)
{ return (((tValue & 0xff) >> 4) | ((tValue & 0xff) << 4)) & 0xff; }
/* -- Converts BIT position to BYTE position ------------------------------- */
template<typename T> requires is_integral_v<T>
  static T BitToByte(const T tPos) { return tPos / CHAR_BIT; }
/* -- Converts BYTE position to BIT position ------------------------------- */
template<typename T> requires is_integral_v<T>
  static T BitFromByte(const T tPos) { return tPos * CHAR_BIT; }
/* -- Returns specified BIT position modulo (0-7) -------------------------- */
template<typename RT,                  // Return type
         typename IT>                  // Integral type
  requires is_integral_v<RT> && is_integral_v<IT>
static RT BitMask(const IT itPos)
  { return static_cast<RT>(1 << itPos % CHAR_BIT); }
/* -- XOR at BIT position 'itPos' in 'ptDst' ------------------------------- */
template<typename PT,                  // Pointer type (without the pointer)
         typename IT>                  // Integral type
  requires is_integral_v<IT> && (!is_pointer_v<PT>)
static void BitSet(PT*const ptDst, const IT itPos)
  { ptDst[BitToByte(itPos)] |= BitMask<PT,IT>(itPos); }
/* -- Return if specified BIT at BIT position 'itPos' in 'ptDst' is set ---- */
template<typename PT,                  // Pointer type (without the pointer)
         typename IT>                  // Integral type
  requires is_integral_v<IT> && (!is_pointer_v<PT>)
static bool BitTest(PT*const ptDst, const IT itPos)
  { return (ptDst[BitToByte(itPos)] & BitMask<PT,IT>(itPos)) != 0; }
/* -- Clear the BIT at BIT position 'itPos' in 'ptDst' --------------------- */
template<typename PT,                  // Pointer type (without the pointer)
         typename IT>                  // Integral type
  requires is_integral_v<IT> && (!is_pointer_v<PT>)
static void BitClear(PT*const ptDst, const IT itPos)
  { ptDst[BitToByte(itPos)] &= ~BitMask<PT,IT>(itPos); }
/* -- Toggle the BIT at BIT position 'itPos' in 'ptDst' -------------------- */
template<typename PT,                  // Pointer type (without the pointer)
         typename IT>                  // Integral type
  requires is_integral_v<IT> && (!is_pointer_v<PT>)
static void BitFlip(PT*const ptDst, const IT itPos)
  { ptDst[BitToByte(itPos)] ^= BitMask<PT,IT>(itPos); }
/* -- Bits handling functions copying from another bit buffer -------------- */
template<typename PT,                  // Pointer type (without the pointer)
         typename IT>                  // Integral type
  requires is_integral_v<IT> && (!is_pointer_v<PT>)
static void BitSet2(PT*const ptDst, const IT itDstPos,
  const PT*const ptSrc, const IT itSrcPos)
{ ptDst[BitToByte(itDstPos)] |= ptSrc[BitToByte(itSrcPos)]; }
/* -- XOR bits from source to destination ---------------------------------- */
template<typename PT,                  // Pointer type (without the pointer)
         typename IT>                  // Integral type
  requires is_integral_v<IT> &&        // 'IT' must be integral
         (!is_pointer_v<PT>)           // 'PT' must not be a pointer
static void BitSet2R(                  // Returns nothing
  PT*const ptDst,                      // Destination buffer
  const IT itDstPos,                   // Source BIT (not BYTE) position
  const PT*const ptSrc,                // Source buffer
  const IT itSrcPos)                   // Destination BIT (not BYTE) position
{ ptDst[BitToByte(itDstPos)] |=
    BitReverseByte<PT>(ptSrc[BitToByte(itSrcPos)]); }
/* -- Unused --------------------------------------------------------------- */
// template<typename PtrType,typename IntType>
//   static bool BitTest2(PtrType*const ptDst, const IntType itDstPos,
//     const PtrType*const ptSrc, const IntType itSrcPos)
//       { return ptDst[BitToByte(itDstPos)] &
//                ptSrc[BitToByte(itSrcPos)]; }
// template<typename PtrType,typename IntType>
//   static void BitClear2(PtrType*const ptDst, const IntType itDstPos,
//     const PtrType*const ptSrc, const IntType itSrcPos)
//      { ptDst[BitToByte(itDstPos)] &= ~ptSrc[BitToByte(itSrcPos)]; }
// template<typename PtrType,typename IntType>
//   static void BitFlip2(PtrType*const ptDst, const IntType itDstPos,
//     const PtrType*const ptSrc, const IntType itSrcPos)
//       { ptDst[BitToByte(itDstPos)] ^= ptSrc[BitToByte(itSrcPos)]; }
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
