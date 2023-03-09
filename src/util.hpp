/* == UTIL.HPP ============================================================= **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## Miscellaneous utility classes and functions too small or            ## **
** ## insignificant (i.e. not a core engine sub-system) to be worth       ## **
** ## putting into it's own file.                                         ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IUtil {                      // Start of private module namespace
/* ------------------------------------------------------------------------- */
using namespace IStd::P;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* -- Check that uint32 and float are the same size ------------------------ */
static_assert(sizeof(float) == sizeof(uint32_t) &&
              sizeof(float) == sizeof(int32_t),
  "Size of 'float' and '(u)int32_t' are not equal!");
/* -- Check that uint64 and double are the same size ----------------------- */
static_assert(sizeof(double) == sizeof(uint64_t) &&
              sizeof(double) == sizeof(int64_t),
  "Size of 'double' and '(u)int64_t' are not equal!");
/* -- Manual memory allocation --------------------------------------------- */
template<typename AnyType,typename IntType>
  static AnyType *UtilMemAlloc(const IntType itBytes)
    { return reinterpret_cast<AnyType*>
        (malloc(static_cast<size_t>(itBytes))); }
template<typename AnyType,typename IntType>
  static AnyType *UtilMemReAlloc(AnyType*const atPtr, const IntType itBytes)
    { return reinterpret_cast<AnyType*>
        (realloc(reinterpret_cast<void*>(atPtr),
                 static_cast<size_t>(itBytes))); }
template<typename AnyType>
  static void UtilMemFree(AnyType*const atPtr)
    { free(reinterpret_cast<void*>(atPtr)); }
/* == Number is divisible by specified number ============================== */
static bool UtilIsDivisible(const double dNumber)
  { double dDummy; return modf(dNumber, &dDummy) == 0; }
// template<typename FloatType=double>
//  static bool IsNormal(const FloatType ftValue)
//    { return fpclassify(iitValue) == FP_NORMAL; }
/* ------------------------------------------------------------------------- */
template<typename IntType=double, typename FloatType=double>
  static IntType UtilRound(const FloatType ftValue, const int iPrecision)
{ // Classify input file
  if(ftValue == 0.0) return IntType(0);
  // Calculate adjustment
  const FloatType ftAmount = pow(10.0, iPrecision);
  // Do rounding
  return static_cast<IntType>(floor(ftValue * ftAmount + 0.5) / ftAmount);
}
/* -- Expand dimensions to specified outer bounds keeping aspect ----------- */
static void UtilStretchToOuter(double &dOW, double &dOH, double &dIW,
  double &dIH)
{ // Get aspect ratio of inner and outer dimensions
  const double dOuterAspect = dOW / dOH, dInnerAspect = dIW / dIH;
  // If the aspect ratios are the same then the screen rectangle will do,
  // otherwise we need to calculate the new rectangle
  if(dInnerAspect > dOuterAspect)
  { // Calculate new width and centring factor
    const double dNewWidth = dOH / dIH * dIW,
                 dCentringFactor = (dOW - dNewWidth) / 2;
    // Set new bounds. We'll use the original vars to set the values
    dIW = dNewWidth + dCentringFactor;
    dIH = dOH;
    dOW = dCentringFactor;
    dOH = 0;
  } // Otherwise
  else if(dInnerAspect < dOuterAspect)
  { // Calculate new width and centring factor
    const double dNewHeight = dOW / dIW * dIH,
                 dCentringFactor = (dOH - dNewHeight) / 2;
    // Set new bounds. We'll use the original vars to set the values
    dIH = dNewHeight + dCentringFactor;
    dIW = dOW;
    dOW = 0;
    dOH = dCentringFactor;
  } // No change?
  else
  { // Keep original values
    dIW = dOW;
    dIH = dOH;
    dOW = dOH = 0;
  }
}
/* -- Expand dimensions to specified inner bounds keeping aspect ----------- */
static void UtilStretchToInner(double &dOW, double &dOH, double &dIW,
  double &dIH)
{ // Get aspect ratio of inner and outer dimensions
  const double dOuterAspect = dOW / dOH, dInnerAspect = dIW / dIH;
  // If the aspect ratios are the same then the screen rectangle will do,
  // otherwise we need to calculate the new rectangle
  if(dInnerAspect > dOuterAspect)
  { // Calculate new width and centring factor
    const double dNewHeight = dOW / dIW * dIH,
                 dCentringFactor = (dOH - dNewHeight) / 2;
    // Set new bounds. We'll use the original vars to set the values
    dIH = dNewHeight + dCentringFactor;
    dIW = dOW;
    dOW = 0;
    dOH = dCentringFactor;
  } // Otherwise
  else if(dInnerAspect < dOuterAspect)
  { // Calculate new width and centring factor
    const double dNewWidth = dOH / dIH * dIW,
                 dCentringFactor = (dOW - dNewWidth) / 2;
    // Set new bounds. We'll use the original vars to set the values
    dIW = dNewWidth + dCentringFactor;
    dIH = 0;
    dOW = dCentringFactor;
  } // No change?
  else
  { // Keep original values
    dIW = dOW;
    dIH = dOH;
    dOW = dOH = 0;
  }
}
/* -- Try to reserve items in a list --------------------------------------- */
template<class ListType>
  static bool UtilReserveList(ListType &ltList, const size_t stCount)
{ // Return if specified value is outrageous!
  if(stCount > ltList.max_size()) return false;
  // Reserve room for this many flaots
  ltList.reserve(stCount);
  // Success
  return true;
}
/* -- Reverse a byte ------------------------------------------------------- */
static uint8_t UtilReverseByte(const uint8_t ucByte)
{ // We shall use a lookup table for this as it is faster
  static const array<const uint8_t,16> ucaLookup{
    0x0, 0x8, 0x4, 0xc, 0x2, 0xa, 0x6, 0xe,   // Index 1 == 0b0001 => 0b1000
    0x1, 0x9, 0x5, 0xd, 0x3, 0xb, 0x7, 0xf }; // Index 7 == 0b0111 => 0b1110
  // Reverse the top and bottom nibble then swap them.
  return static_cast<uint8_t>((ucaLookup[ucByte & 0b1111] << 4) |
    ucaLookup[ucByte >> 4]);
}
/* -- Helper functions to force integer byte ordering ---------------------- */
template<typename IntType>static IntType UtilToI16LE(const IntType itV)
  { static_assert(sizeof(IntType) == sizeof(uint16_t) &&
      is_integral_v<IntType>, "Not a 16-bit integer!");
    return static_cast<IntType>(STRICT_U16LE(itV)); }
template<typename IntType>static IntType UtilToI16BE(const IntType itV)
  { static_assert(sizeof(IntType) == sizeof(uint16_t) &&
      is_integral_v<IntType>, "Not a 16-bit integer!");
    return static_cast<IntType>(STRICT_U16BE(itV)); }
template<typename IntType>static IntType UtilToI32LE(const IntType itV)
  { static_assert(sizeof(IntType) == sizeof(uint32_t) &&
      is_integral_v<IntType>, "Not a 32-bit integer!");
    return static_cast<IntType>(STRICT_U32LE(itV)); }
template<typename IntType>static IntType UtilToI32BE(const IntType itV)
 { static_assert(sizeof(IntType) == sizeof(uint32_t) &&
     is_integral_v<IntType>, "Not a 32-bit integer!");
   return static_cast<IntType>(STRICT_U32BE(itV)); }
template<typename IntType>static IntType UtilToI64LE(const IntType itV)
  { static_assert(sizeof(IntType) == sizeof(uint64_t) &&
      is_integral_v<IntType>, "Not a 64-bit integer!");
    return static_cast<IntType>(STRICT_U64LE(itV)); }
template<typename IntType>static IntType UtilToI64BE(const IntType itV)
  { static_assert(sizeof(IntType) == sizeof(uint64_t) &&
      is_integral_v<IntType>, "Not a 64-bit integer!");
    return static_cast<IntType>(STRICT_U64BE(itV)); }
/* -- Convert bit count to bitmask ----------------------------------------- */
template<typename IntType>static IntType UtilBitsToMask(size_t stCount)
{ // Initial value
  IntType itValue = 0;
  // Build mask for bit count
  while(--stCount != StdMaxSizeT) itValue |= 1 << stCount;
  // Return generated value
  return itValue;
}
/* -- Swap class functors -------------------------------------------------- */
struct UtilSwap32LEFunctor             // Swap 32-bit little <-> big integer
  { uint32_t v;                        // Output value (32-bit)
    explicit UtilSwap32LEFunctor(uint32_t dwV) : v(UtilToI32LE(dwV)) { } };
struct UtilSwap32BEFunctor             // Swap 32-bit big <-> little integer
  { uint32_t v;                        // Output value (32-bit)
    explicit UtilSwap32BEFunctor(uint32_t dwV) : v(UtilToI32BE(dwV)) { } };
struct UtilSwap64LEFunctor             // Swap 64-bit little <-> big integer
  { uint64_t v;                        // Output value (64-bit)
    explicit UtilSwap64LEFunctor(uint64_t qwV) : v(UtilToI64LE(qwV)) { } };
struct UtilSwap64BEFunctor             // Swap 64-bit big <-> little integer
  { uint64_t v;                        // Output value (64-bit)
    explicit UtilSwap64BEFunctor(uint64_t qwV) : v(UtilToI64BE(qwV)) { } };
/* -- Convert const object to non-const ------------------------------------ */
template<typename Type>static Type &UtilToNonConst(const Type &tSrc)
  { return const_cast<Type&>(tSrc); }
/* -- Brute cast one type to another --------------------------------------- */
template<typename TypeDst, typename TypeSrc>
  static TypeDst UtilBruteCast(const TypeSrc tsV)
    { union U{ TypeSrc ts; TypeDst td; }; return U{ tsV }.td; }
/* -- Brute cast a 32-bit float to 32-bit integer -------------------------- */
static uint32_t UtilCastFloatToInt32(const float fV)
  { return UtilBruteCast<uint32_t>(fV); }
/* -- Brute cast a 32-bit integer to a 32-bit float ------------------------ */
static float UtilCastInt32ToFloat(const uint32_t ulV)
  { return UtilBruteCast<float>(ulV); }
/* -- Brute cast a 64-bit integer to a 64-bit double ----------------------- */
static double UtilCastInt64ToDouble(const uint64_t ullV)
  { return UtilBruteCast<double>(ullV); }
/* -- Brute cast a 64-bit double to a 64-bit integer ----------------------- */
static uint64_t UtilCastDoubleToInt64(const double dV)
  { return UtilBruteCast<uint64_t>(dV); }
/* -- Helper functions to force float byte ordering ------------------------ */
static float UtilToF32LE(const float fV)
  { return UtilCastInt32ToFloat(UtilToI32LE(UtilCastFloatToInt32(fV))); }
static float UtilToF32BE(const float fV)
  { return UtilCastInt32ToFloat(UtilToI32BE(UtilCastFloatToInt32(fV))); }
static double UtilToF64LE(const double dV)
  { return UtilCastInt64ToDouble(UtilToI64LE(UtilCastDoubleToInt64(dV))); }
static double UtilToF64BE(const double dV)
  { return UtilCastInt64ToDouble(UtilToI64BE(UtilCastDoubleToInt64(dV))); }
/* -- More helper functions for byte ordering ------------------------------ */
static int16_t UtilToLittleEndian(const int16_t wV)
  { return UtilToI16LE(wV); }
static int16_t UtilToBigEndian(const int16_t wV)
  { return UtilToI16BE(wV); }
static uint16_t UtilToLittleEndian(const uint16_t wV)
  { return UtilToI16LE(wV); }
static uint16_t UtilToBigEndian(const uint16_t wV)
  { return UtilToI16BE(wV); }
static int32_t UtilToLittleEndian(const int32_t dwV)
  { return UtilToI32LE(dwV); }
static int32_t UtilToBigEndian(const int32_t dwV)
  { return UtilToI32BE(dwV); }
static uint32_t UtilToLittleEndian(const uint32_t dwV)
  { return UtilToI32LE(dwV); }
static uint32_t UtilToBigEndian(const uint32_t dwV)
  { return UtilToI32BE(dwV); }
static int64_t UtilToLittleEndian(const int64_t qwV)
  { return UtilToI64LE(qwV); }
static int64_t UtilToBigEndian(const int64_t qwV)
  { return UtilToI64BE(qwV); }
static uint64_t UtilToLittleEndian(const uint64_t qwV)
  { return UtilToI64LE(qwV); }
static uint64_t UtilToBigEndian(const uint64_t qwV)
  { return UtilToI64BE(qwV); }
/* -- Convert float normal back to integer --------------------------------- */
template<typename IntTypeRet,
         typename IntTypeInternal=uint8_t,
         typename IntTypeParam>
static const IntTypeRet UtilDenormalise(const IntTypeParam itpVal)
{ // Validate typenames
  static_assert(is_integral_v<IntTypeRet>,
    "Return type must be integral!");
  static_assert(is_integral_v<IntTypeInternal>,
    "Internal type must be integral!");
  static_assert(is_floating_point_v<IntTypeParam>,
    "Param type must be number!");
  // Do the conversion and return it
  return static_cast<IntTypeRet>(itpVal *
    numeric_limits<IntTypeInternal>::max());
}
/* -- Convert integer to float between 0 and 1 ----------------------------- */
template<typename IntTypeRet,
         typename IntTypeParam>
static IntTypeRet UtilNormalise(const IntTypeParam itpVal)
{ // Check template parameters
  static_assert(is_floating_point_v<IntTypeRet>,
    "Return type must be number!");
  static_assert(is_integral_v<IntTypeParam>,
    "Param type must be integral!");
  // Calculate a value between 0.0 and 1.0
  return static_cast<IntTypeRet>(itpVal) / numeric_limits<IntTypeParam>::max();
}
/* -- Extract a part of an integer ----------------------------------------- */
template<typename IntTypeRet,
         size_t stShift=0,
         typename IntTypeInternal=uint8_t>
static IntTypeRet UtilExtract(const unsigned int uiV)
{ // Check template parameters
  static_assert(stShift <= sizeof(uiV)*8,
    "Shift value invalid!");
  static_assert(is_integral_v<IntTypeInternal>,
    "Internal type must be integral!");
  // Shift the value the specified amount of times and cast it to user type
  return static_cast<IntTypeInternal>(uiV >> stShift);
}
/* -- Grab part of an integer and normalise it between 0 and 1 ------------- */
template<typename IntTypeRet=float,
         size_t stShift=0,
         typename IntTypeInternal=uint8_t>
static IntTypeRet UtilNormaliseEx(const unsigned int uiV)
{ // Check template parameters
  static_assert(is_floating_point_v<IntTypeRet>,
    "Return type must be number!");
  static_assert(stShift <= sizeof(uiV)*8,
    "Shift value invalid!");
  static_assert(is_integral_v<IntTypeInternal>,
    "Internal type must be integral!");
  // Extract the specified value and then normalise it
  return UtilNormalise<IntTypeRet>(UtilExtract<IntTypeInternal,
    stShift, IntTypeInternal>(uiV));
}
/* -- Returns if specified integer would overflow specified type ----------- */
template<typename TestIntType, typename ParamIntType>
  static bool UtilIntWillOverflow(const ParamIntType pitVal)
{ // Automatically false if both types are the same
  if(is_same_v<TestIntType, ParamIntType>) return false;
  // Is signed (can be negative?). Return if out of bounds
  if(numeric_limits<TestIntType>::is_signed)
  { // If input type is unsigned then return true if it overflows.
    if(!numeric_limits<ParamIntType>::is_signed)
      if(static_cast<uintmax_t>(pitVal) > static_cast<uintmax_t>(INTMAX_MAX))
        return true;
    // Return if overflows
    const intmax_t iVal = static_cast<intmax_t>(pitVal);
    return iVal < static_cast<intmax_t>(numeric_limits<TestIntType>::min()) ||
           iVal > static_cast<intmax_t>(numeric_limits<TestIntType>::max());
  } // Unsigned so it would overflow is param value is signed and negative
  if constexpr(numeric_limits<ParamIntType>::is_signed)
    if(pitVal < 0) return true;
  // Unsigned so not as much checking needed. Return if out of bounds
  return static_cast<uintmax_t>(pitVal) >
         static_cast<uintmax_t>(numeric_limits<TestIntType>::max());
}
/* -- Join two integers to make a bigger integer --------------------------- */
template<typename IntTypeHigh,typename IntTypeLow>
  static uint16_t UtilMakeWord(const IntTypeHigh ithV, const IntTypeLow itlV)
    { return static_cast<uint16_t>((static_cast<uint16_t>(ithV) <<  8) |
        (static_cast<uint16_t>(itlV) & 0xff)); }
template<typename IntTypeHigh,typename IntTypeLow>
  static uint32_t UtilMakeDWord(const IntTypeHigh ithV, const IntTypeLow itlV)
    { return static_cast<uint32_t>((static_cast<uint32_t>(ithV) << 16) |
        (static_cast<uint32_t>(itlV) & 0xffff)); }
template<typename IntTypeHigh,typename IntTypeLow>
  static uint64_t UtilMakeQWord(const IntTypeHigh ithV, const IntTypeLow itlV)
    { return static_cast<uint64_t>((static_cast<uint64_t>(ithV) << 32) |
        (static_cast<uint64_t>(itlV) & 0xffffffff)); }
/* -- Return lowest and highest 8-bits of integer -------------------------- */
template<typename IntType>static uint8_t UtilLowByte(const IntType itVal)
  { return static_cast<uint8_t>(itVal & 0x00ff); }
template<typename IntType>static uint8_t UtilHighByte(const IntType itVal)
  { return static_cast<uint8_t>((itVal & 0xff00) >> 8); }
/* -- Return lowest and highest 16-bits of integer ------------------------- */
template<typename IntType>static uint16_t UtilLowWord(const IntType itVal)
  { return static_cast<uint16_t>(itVal & 0x0000ffff); }
template<typename IntType>static uint16_t UtilHighWord(const IntType itVal)
  { return static_cast<uint16_t>((itVal & 0xffff0000) >> 16); }
/* -- Return lowest and highest 32-bits of integer ------------------------- */
template<typename IntType>static uint32_t UtilLowDWord(const IntType itVal)
  { return static_cast<uint32_t>(itVal & 0x00000000ffffffff); }
template<typename IntType>static uint32_t UtilHighDWord(const IntType itVal)
  { return static_cast<uint32_t>((itVal & 0xffffffff00000000) >> 32); }
/* -- Return lowest or highest number out of two --------------------------- */
template<typename IntType1,typename IntType2>
  static IntType1 UtilMinimum(const IntType1 itOne, const IntType2 itTwo)
    { return (itOne < static_cast<IntType1>(itTwo)) ?
        itOne : static_cast<IntType1>(itTwo); }
template<typename IntType1,typename IntType2>
  static IntType1 UtilMaximum(const IntType1 itOne, const IntType2 itTwo)
    { return (itOne > static_cast<IntType1>(itTwo)) ?
        itOne : static_cast<IntType1>(itTwo); }
/* -- Clamp a number between two values ------------------------------------ */
template<typename TVAL, typename TMIN, typename TMAX>
  static TVAL UtilClamp(const TVAL tVal, const TMIN tMin, const TMAX tMax)
    { return UtilMaximum(static_cast<TVAL>(tMin),
             UtilMinimum(static_cast<TVAL>(tMax), tVal)); }
/* -- Make a percentage ---------------------------------------------------- */
template<typename T1, typename T2, typename R=double>
  static R UtilMakePercentage(const T1 tCurrent, const T2 tMaximum,
    const R rMulti=100)
      { return static_cast<R>(tCurrent) / tMaximum * rMulti; }
/* -- Calculate distance between two values -------------------------------- */
template<typename AnyType>
  static AnyType UtilDistance(const AnyType atX, const AnyType atY)
    { return atX > atY ? atX - atY : atY - atX; }
/* -- Round to nearest value ----------------------------------------------- */
template<typename IntType>
  static IntType UtilNearest(const IntType itValue, const IntType itMultiple)
    { return (itValue + (itMultiple / 2)) / itMultiple * itMultiple; }
/* -- Returns the nearest power of two to specified number ----------------- */
template<typename RetType, typename IntType>
  static const RetType UtilNearestPow2(const IntType itValue)
    { return static_cast<RetType>(pow(2, ceil(log(itValue) / log(2)))); }
/* -- If variable would overflow another type then return its maximum ------ */
template<typename RetType, typename IntType>
  static RetType UtilIntOrMax(const IntType itValue)
    { return UtilIntWillOverflow<RetType, IntType>(itValue) ?
        numeric_limits<RetType>::max() : static_cast<RetType>(itValue); }
/* -- Convert millimetres to inches ---------------------------------------- */
template<typename IntType>
  static double UtilMillimetresToInches(const IntType itValue)
    { return static_cast<double>(itValue) * 0.0393700787; }
/* -- Get the distance between two opposing corners ------------------------ */
template<typename IntType>
  static double UtilGetDiagLength(const IntType itWidth,
    const IntType itHeight)
      { return sqrt(pow(itWidth, 2) + pow(itHeight, 2)); }
/* -- Returns true if two numbers are equal (Omit != and == warnings) ------ */
template<typename FloatType>
  static bool UtilIsFloatEqual(const FloatType f1, const FloatType f2)
    { return ((f1 >= f2) && (f1 <= f2)); }
/* -- Returns true if two numbers are NOT equal (Omit != and == warnings) -- */
template<typename FloatType>
  static bool UtilIsFloatNotEqual(const FloatType f1, const FloatType f2)
    { return ((f1 < f2) || (f1 > f2)); }
/* -- Clear any static data of any size ------------------------------------ */
template<typename StaticType>void UtilClearStatic(StaticType &stData)
  { memset(&stData, '\0', sizeof(stData)); }
/* -- Bits handling functions ---------------------------------------------- */
template<typename IntType>
  static IntType UtilBitSwap4(const IntType itValue)
    { return (((itValue & 0xff) >> 4) | ((itValue & 0xff) << 4)) & 0xff; }
template<typename IntType>
  static IntType UtilBitToByte(const IntType itPos)
    { return itPos / CHAR_BIT; }
template<typename IntType>
  static IntType UtilBitFromByte(const IntType itPos)
    { return itPos * CHAR_BIT; }
template<typename RetType,typename IntType>
  static RetType UtilBitMask(const IntType itPos)
    { return static_cast<RetType>(1 << (itPos % CHAR_BIT)); }
template<typename PtrType,typename IntType>
  static void UtilBitSet(PtrType*const ptDst, const IntType itPos)
    { ptDst[UtilBitToByte(itPos)] |=
        UtilBitMask<typename remove_pointer<PtrType>::type,IntType>(itPos); }
template<typename PtrType,typename IntType>
  static bool UtilBitTest(PtrType*const ptDst, const IntType itPos)
    { return !!(ptDst[UtilBitToByte(itPos)] &
        UtilBitMask<typename remove_pointer<PtrType>::type,IntType>(itPos)); }
template<typename PtrType,typename IntType>
  static void UtilBitClear(PtrType*const ptDst, const IntType itPos)
    { ptDst[UtilBitToByte(itPos)] &=
        ~UtilBitMask<typename remove_pointer<PtrType>::type,IntType>(itPos); }
template<typename PtrType,typename IntType>
  static void UtilBitFlip(PtrType*const ptDst, const IntType itPos)
    { ptDst[UtilBitToByte(itPos)] ^=
        UtilBitMask<typename remove_pointer<PtrType>::type,IntType>(itPos); }
/* -- Bits handling functions copying from another bit buffer -------------- */
template<typename PtrType,typename IntType>
  static void UtilBitSet2(PtrType*const ptDst, const IntType itDstPos,
    const PtrType*const ptSrc, const IntType itSrcPos)
      { ptDst[UtilBitToByte(itDstPos)] |= ptSrc[UtilBitToByte(itSrcPos)]; }
template<typename PtrType,typename IntType>
  static void UtilBitSet2R(PtrType*const ptDst, const IntType itDstPos,
    const PtrType*const ptSrc, const IntType itSrcPos)
      { ptDst[UtilBitToByte(itDstPos)] |=
          UtilReverseByte(ptSrc[UtilBitToByte(itSrcPos)]); }
// template<typename PtrType,typename IntType>
//   static bool UtilBitTest2(PtrType*const ptDst, const IntType itDstPos,
//     const PtrType*const ptSrc, const IntType itSrcPos)
//       { return ptDst[UtilBitToByte(itDstPos)] &
//                ptSrc[UtilBitToByte(itSrcPos)]; }
// template<typename PtrType,typename IntType>
//   static void UtilBitClear2(PtrType*const ptDst, const IntType itDstPos,
//     const PtrType*const ptSrc, const IntType itSrcPos)
//      { ptDst[UtilBitToByte(itDstPos)] &= ~ptSrc[UtilBitToByte(itSrcPos)]; }
// template<typename PtrType,typename IntType>
//   static void UtilBitFlip2(PtrType*const ptDst, const IntType itDstPos,
//     const PtrType*const ptSrc, const IntType itSrcPos)
//       { ptDst[UtilBitToByte(itDstPos)] ^= ptSrc[UtilBitToByte(itSrcPos)]; }
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
