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
/* == Number is divisible by specified number ============================== */
static bool UtilIsDivisible(const double dNumber)
  { double dDummy; return modf(dNumber, &dDummy) == 0; }
/* -- Return -1 or 1 depending wether the value is positive or negative ---- */
template<typename T> requires is_signed_v<T> static T UtilSign(const T tValue)
  { return static_cast<T>(tValue > 0) - static_cast<T>(tValue < 0); }
/* ------------------------------------------------------------------------- */
template<typename IT=double, typename FT=double>
  requires is_arithmetic_v<IT> && is_floating_point_v<FT>
static IT UtilRound(const FT ftValue, const int iPrecision)
{ // Is 32-bit floating point?
  if constexpr(is_same_v<FT, float>)
  { // Non-zero?
    if(ftValue != 0.0f)
    { // Round as float and return result
      const FT ftAmount = powf(10.0f, static_cast<float>(iPrecision));
      return static_cast<IT>(floorf(ftValue * ftAmount + 0.5f) / ftAmount);
    }
  } // Is 64-bit floating point?
  else if constexpr(is_same_v<FT, double>)
  { // Non-zero?
    if(ftValue != 0.0)
    { // Round as double and return result
      const FT ftAmount = pow(10.0, static_cast<double>(iPrecision));
      return static_cast<IT>(floor(ftValue * ftAmount + 0.5) / ftAmount);
    }
  } // Zero or invalid type
  return static_cast<IT>(0);
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
                 dCentringFactor = (dOW - dNewWidth) / 2.0;
    // Set new bounds. We'll use the original vars to set the values
    dIW = dNewWidth + dCentringFactor;
    dIH = dOH;
    dOW = dCentringFactor;
    dOH = 0;
  } // Otherwise
  else if(dInnerAspect < dOuterAspect)
  { // Calculate new width and centring factor
    const double dNewHeight = dOW / dIW * dIH,
                 dCentringFactor = (dOH - dNewHeight) / 2.0;
    // Set new bounds. We'll use the original vars to set the values
    dIH = dNewHeight + dCentringFactor;
    dIW = dOW;
    dOW = 0.0;
    dOH = dCentringFactor;
  } // No change?
  else
  { // Keep original values
    dIW = dOW;
    dIH = dOH;
    dOW = dOH = 0.0;
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
                 dCentringFactor = (dOH - dNewHeight) / 2.0;
    // Set new bounds. We'll use the original vars to set the values
    dIH = dNewHeight + dCentringFactor;
    dIW = dOW;
    dOW = 0.0;
    dOH = dCentringFactor;
  } // Otherwise
  else if(dInnerAspect < dOuterAspect)
  { // Calculate new width and centring factor
    const double dNewWidth = dOH / dIH * dIW,
                 dCentringFactor = (dOW - dNewWidth) / 2.0;
    // Set new bounds. We'll use the original vars to set the values
    dIW = dNewWidth + dCentringFactor;
    dIH = 0.0;
    dOW = dCentringFactor;
  } // No change?
  else
  { // Keep original values
    dIW = dOW;
    dIH = dOH;
    dOW = dOH = 0.0;
  }
}
/* -- Try to reserve items in a list --------------------------------------- */
template<class ListType> requires is_class_v<ListType>
  static bool UtilReserveList(ListType &ltList, const size_t stCount)
{ // Return if specified value is outrageous!
  if(stCount > ltList.max_size()) return false;
  // Reserve room for this many flaots
  ltList.reserve(stCount);
  // Success
  return true;
}
/* -- Helper functions to force integer byte ordering ---------------------- */
template<typename IntType>
  requires (sizeof(IntType) == sizeof(uint16_t)) && is_integral_v<IntType>
static IntType UtilToI16LE(const IntType itV)
  { return static_cast<IntType>(STRICT_U16LE(itV)); }
/* ------------------------------------------------------------------------- */
template<typename IntType>
  requires (sizeof(IntType) == sizeof(uint16_t)) && is_integral_v<IntType>
static IntType UtilToI16BE(const IntType itV)
  { return static_cast<IntType>(STRICT_U16BE(itV)); }
/* ------------------------------------------------------------------------- */
template<typename IntType>
  requires (sizeof(IntType) == sizeof(uint32_t)) && is_integral_v<IntType>
static IntType UtilToI32LE(const IntType itV)
  { return static_cast<IntType>(STRICT_U32LE(itV)); }
/* ------------------------------------------------------------------------- */
template<typename IntType>
  requires (sizeof(IntType) == sizeof(uint32_t)) && is_integral_v<IntType>
static IntType UtilToI32BE(const IntType itV)
  { return static_cast<IntType>(STRICT_U32BE(itV)); }
/* ------------------------------------------------------------------------- */
template<typename IntType>
  requires (sizeof(IntType) == sizeof(uint64_t)) && is_integral_v<IntType>
static IntType UtilToI64LE(const IntType itV)
  { return static_cast<IntType>(STRICT_U64LE(itV)); }
/* ------------------------------------------------------------------------- */
template<typename IntType>
  requires (sizeof(IntType) == sizeof(uint64_t)) && is_integral_v<IntType>
static IntType UtilToI64BE(const IntType itV)
  { return static_cast<IntType>(STRICT_U64BE(itV)); }
/* -- Swap class functors -------------------------------------------------- */
struct UtilSwap32LEFunctor             // Swap 32-bit little <-> big integer
  { uint32_t v;                        // Output value (32-bit)
    explicit UtilSwap32LEFunctor(uint32_t dwV) :
      v(UtilToI32LE(dwV)) {} };
struct UtilSwap32BEFunctor             // Swap 32-bit big <-> little integer
  { uint32_t v;                        // Output value (32-bit)
    explicit UtilSwap32BEFunctor(uint32_t dwV) :
      v(UtilToI32BE(dwV)) {} };
struct UtilSwap64LEFunctor             // Swap 64-bit little <-> big integer
  { uint64_t v;                        // Output value (64-bit)
    explicit UtilSwap64LEFunctor(uint64_t ullV) :
      v(UtilToI64LE(ullV)) {} };
struct UtilSwap64BEFunctor             // Swap 64-bit big <-> little integer
  { uint64_t v;                        // Output value (64-bit)
    explicit UtilSwap64BEFunctor(uint64_t ullV) :
      v(UtilToI64BE(ullV)) {} };
/* -- Convert const object to non-const ------------------------------------ */
template<typename Type>static Type &UtilToNonConst(const Type &tSrc)
  { return const_cast<Type&>(tSrc); }
/* -- Brute cast one type to another --------------------------------------- */
template<typename TypeDst, typename TypeSrc>
  requires (sizeof(TypeDst) == sizeof(TypeSrc)) &&
           is_trivially_copyable_v<TypeSrc> &&
           is_trivially_copyable_v<TypeDst>
static TypeDst UtilBruteCast(const TypeSrc tsV)
  { return bit_cast<TypeDst>(tsV); }
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
/* ------------------------------------------------------------------------- */
static float UtilToF32BE(const float fV)
  { return UtilCastInt32ToFloat(UtilToI32BE(UtilCastFloatToInt32(fV))); }
/* ------------------------------------------------------------------------- */
static double UtilToF64LE(const double dV)
  { return UtilCastInt64ToDouble(UtilToI64LE(UtilCastDoubleToInt64(dV))); }
/* ------------------------------------------------------------------------- */
static double UtilToF64BE(const double dV)
  { return UtilCastInt64ToDouble(UtilToI64BE(UtilCastDoubleToInt64(dV))); }
/* -- Convert to little endian integer ------------------------------------- */
template<typename IntType>
  requires is_integral_v<IntType>
static IntType UtilToLittleEndian(const IntType itValue)
{ // Convert 16-bit big-endian integer to little-endian
  if constexpr(sizeof(IntType) == sizeof(uint16_t))
    return UtilToI16LE<IntType>(itValue);
  // Convert 32-bit big-endian integer to little-endian
  else if constexpr(sizeof(IntType) == sizeof(uint32_t))
    return UtilToI32LE<IntType>(itValue);
  // Convert 64-bit big-endian integer to little-endian
  else if constexpr(sizeof(IntType) == sizeof(uint64_t))
    return UtilToI64LE<IntType>(itValue);
  // Don't convert anything else
  else return itValue;
}
/* -- Convert to big endian integer ---------------------------------------- */
template<typename IntType> requires is_integral_v<IntType>
  static IntType UtilToBigEndian(const IntType itValue)
{ // Convert 16-bit big-endian integer to big-endian
  if constexpr(sizeof(IntType) == sizeof(uint16_t))
    return UtilToI16BE<IntType>(itValue);
  // Convert 32-bit big-endian integer to big-endian
  else if constexpr(sizeof(IntType) == sizeof(uint32_t))
    return UtilToI32BE<IntType>(itValue);
  // Convert 64-bit big-endian integer to big-endian
  else if constexpr(sizeof(IntType) == sizeof(uint64_t))
    return UtilToI64BE<IntType>(itValue);
  // Don't convert anything else
  else return itValue;
}
/* -- Convert float normal back to integer --------------------------------- */
template<typename IntTypeRet,
         typename IntTypeInternal=uint8_t,
         typename IntTypeParam>
requires is_integral_v<IntTypeRet> &&
         is_integral_v<IntTypeInternal> &&
         is_floating_point_v<IntTypeParam>
static IntTypeRet UtilDenormalise(const IntTypeParam itpVal)
{ // Do the conversion and return it
  return static_cast<IntTypeRet>(itpVal *
    numeric_limits<IntTypeInternal>::max());
}
/* -- Convert integer to float between 0 and 1 ----------------------------- */
template<typename IntTypeRet, typename IntTypeParam>
  requires is_floating_point_v<IntTypeRet> || is_integral_v<IntTypeParam>
static IntTypeRet UtilNormalise(const IntTypeParam itpVal)
  { return static_cast<IntTypeRet>(itpVal) /
      numeric_limits<IntTypeParam>::max(); }
/* -- Extract a part of an integer ----------------------------------------- */
template<typename IntTypeRet,
         size_t stShift=0,
         typename IntTypeInternal=uint8_t,
         typename IntTypeExternal=unsigned int>
requires (stShift <= sizeof(IntTypeExternal) * 8) &&
          is_integral_v<IntTypeInternal>
static IntTypeRet UtilExtract(const IntTypeExternal iteV)
  { return static_cast<IntTypeInternal>(iteV >> stShift); }
/* -- Grab part of an integer and normalise it between 0 and 1 ------------- */
template<typename IntTypeRet=float,
         size_t stShift=0,
         typename IntTypeInternal=uint8_t,
         typename IntTypeExternal=unsigned int>
requires is_floating_point_v<IntTypeRet> &&
         (stShift <= sizeof(IntTypeExternal) * 8) &&
         is_integral_v<IntTypeInternal>
static IntTypeRet UtilNormaliseEx(const IntTypeExternal iteV)
  { return UtilNormalise<IntTypeRet>(UtilExtract<IntTypeInternal,
      stShift, IntTypeInternal>(iteV)); }
/* -- Scale a value from (0-max) to a different min-max -------------------- */
template<typename DestIntType, typename PhysIntType, typename VirtIntType>
  requires is_arithmetic_v<DestIntType> && is_arithmetic_v<PhysIntType> &&
           is_arithmetic_v<VirtIntType>
static DestIntType UtilScaleValue(const DestIntType itV,
  const PhysIntType itPMax, const VirtIntType itVMin, const VirtIntType itVMax)
{ return static_cast<DestIntType>(itVMin) + itV /
    itPMax * static_cast<DestIntType>(itVMax); }
/* -- Returns if specified integer would overflow specified type ----------- */
template<typename IntTypeTarget, typename IntTypeSource>
  requires (is_integral_v<IntTypeSource> || is_enum_v<IntTypeSource>) &&
           (is_integral_v<IntTypeTarget> || is_enum_v<IntTypeTarget>)
constexpr static bool UtilIntWillOverflow(const IntTypeSource itsValue)
{ // If both types are signed?
  if constexpr(is_signed_v<IntTypeSource> == is_signed_v<IntTypeTarget>)
  { // Both are signed so no overflow possible if source size less
    if constexpr(sizeof(IntTypeSource) <= sizeof(IntTypeTarget))
      return false;
    // Return true if overflowed, false if not
    else return itsValue < static_cast<IntTypeSource>
        (numeric_limits<IntTypeTarget>::min()) ||
      itsValue > static_cast<IntTypeSource>
        (numeric_limits<IntTypeTarget>::max());
  } // Both types aren't signed so if source is signed?
  else if constexpr(is_signed_v<IntTypeSource>)
    // Source is signed, target is unsigned so return if negative or overflow
    return itsValue < 0 ||
           static_cast<make_unsigned_t<IntTypeSource>>(itsValue) >
             numeric_limits<IntTypeTarget>::max();
  // Source is unsigned, target is signed so return if overflow
  else return itsValue > static_cast<make_unsigned_t<IntTypeTarget>>
    (numeric_limits<IntTypeTarget>::max());
}
/* -- Join two integers to make a bigger integer --------------------------- */
template<typename IntTypeHigh,typename IntTypeLow>
requires is_integral_v<IntTypeHigh> && is_integral_v<IntTypeLow>
static uint16_t UtilMakeWord(const IntTypeHigh ithV,
  const IntTypeLow itlV)
{ return static_cast<uint16_t>((static_cast<uint16_t>(ithV) << 8) |
   (static_cast<uint16_t>(itlV) & 0xff)); }
/* ------------------------------------------------------------------------- */
template<typename IntTypeHigh,typename IntTypeLow>
requires is_integral_v<IntTypeHigh> && is_integral_v<IntTypeLow>
static uint32_t UtilMakeDWord(const IntTypeHigh ithV,
  const IntTypeLow itlV)
{ return static_cast<uint32_t>((static_cast<uint32_t>(ithV) << 16) |
        (static_cast<uint32_t>(itlV) & 0xffff)); }
/* ------------------------------------------------------------------------- */
template<typename IntTypeHigh,typename IntTypeLow>
  requires is_integral_v<IntTypeHigh> && is_integral_v<IntTypeLow>
static uint64_t UtilMakeQWord(const IntTypeHigh ithV,
  const IntTypeLow itlV)
{ return static_cast<uint64_t>((static_cast<uint64_t>(ithV) << 32) |
    (static_cast<uint64_t>(itlV) & 0xffffffff)); }
/* -- Return lowest and highest 8-bits of integer -------------------------- */
template<typename IntType>
  requires is_integral_v<IntType>
static uint8_t UtilLowByte(const IntType itVal)
  { return static_cast<uint8_t>(itVal & 0x00ff); }
/* ------------------------------------------------------------------------- */
template<typename IntType> requires is_integral_v<IntType>
  static uint8_t UtilHighByte(const IntType itVal)
{ return static_cast<uint8_t>((itVal & 0xff00) >> 8); }
/* -- Return lowest and highest 16-bits of integer ------------------------- */
template<typename IntType> requires is_integral_v<IntType>
  static uint16_t UtilLowWord(const IntType itVal)
{ return static_cast<uint16_t>(itVal & 0x0000ffff); }
/* ------------------------------------------------------------------------- */
template<typename IntType> requires is_integral_v<IntType>
  static uint16_t UtilHighWord(const IntType itVal)
{ return static_cast<uint16_t>((itVal & 0xffff0000) >> 16); }
/* -- Return lowest and highest 32-bits of integer ------------------------- */
template<typename IntType> requires is_integral_v<IntType>
  static uint32_t UtilLowDWord(const IntType itVal)
{ return static_cast<uint32_t>(itVal & 0x00000000ffffffff); }
/* ------------------------------------------------------------------------- */
template<typename IntType> requires is_integral_v<IntType>
  static uint32_t UtilHighDWord(const IntType itVal)
{ return static_cast<uint32_t>((itVal & 0xffffffff00000000) >> 32); }
/* -- Return lowest or highest number out of two --------------------------- */
template<typename IntType1,typename IntType2>
  requires is_arithmetic_v<IntType1> && is_arithmetic_v<IntType2> &&
           is_same_v<IntType1, IntType2>
static IntType1 UtilMinimum(const IntType1 itOne, const IntType2 itTwo)
  { return itOne < static_cast<IntType1>(itTwo) ?
      itOne : static_cast<IntType1>(itTwo); }
/* ------------------------------------------------------------------------- */
template<typename IntType1,typename IntType2>
  requires is_arithmetic_v<IntType1> && is_arithmetic_v<IntType2> &&
           is_same_v<IntType1, IntType2>
static IntType1 UtilMaximum(const IntType1 itOne, const IntType2 itTwo)
  { return itOne > static_cast<IntType1>(itTwo) ?
      itOne : static_cast<IntType1>(itTwo); }
/* -- Clamp a number between two values ------------------------------------ */
template<typename TVAL, typename TMIN, typename TMAX>
  requires is_arithmetic_v<TVAL> && is_arithmetic_v<TMIN> &&
           is_arithmetic_v<TMAX>
static TVAL UtilClamp(const TVAL tVal, const TMIN tMin,
  const TMAX tMax)
{ return UtilMaximum(static_cast<TVAL>(tMin),
         UtilMinimum(static_cast<TVAL>(tMax), tVal)); }
/* -- Make a percentage ---------------------------------------------------- */
template<typename T1, typename T2, typename R=double>
  requires is_arithmetic_v<T1> && is_arithmetic_v<T2> && is_floating_point_v<R>
static R UtilMakePercentage(const T1 tCurrent, const T2 tMaximum,
  const R rMulti=100)
{ return static_cast<R>(tCurrent) / static_cast<R>(tMaximum) * rMulti; }
/* -- Calculate distance between two values -------------------------------- */
template<typename AnyType>
  requires is_arithmetic_v<AnyType>
static AnyType UtilDistance(const AnyType atX, const AnyType atY)
  { return atX > atY ? atX - atY : atY - atX; }
/* -- Round to nearest value ----------------------------------------------- */
template<typename IntType>
  requires is_arithmetic_v<IntType>
static IntType UtilNearest(const IntType itValue,
  const IntType itMultiple)
{ return (itValue + itMultiple / 2) / itMultiple * itMultiple; }
/* -- Returns the nearest power of two to specified number ----------------- */
template<typename RetType, typename IntType>
  requires is_arithmetic_v<RetType> && is_arithmetic_v<IntType>
static RetType UtilNearestPow2(const IntType itValue)
  { return static_cast<RetType>(pow(2, ceil(log2(itValue)))); }
/* -- If variable would overflow another type then return its maximum ------ */
template<typename RetType, typename IntType>
  requires is_arithmetic_v<RetType> &&
          (is_arithmetic_v<IntType> || is_enum_v<IntType>)
static RetType UtilIntOrMax(const IntType itValue)
  { return UtilIntWillOverflow<RetType, IntType>(itValue) ?
      numeric_limits<RetType>::max() : static_cast<RetType>(itValue); }
/* -- Convert millimetres to inches ---------------------------------------- */
template<typename IntType> requires is_arithmetic_v<IntType>
  static double UtilMillimetresToInches(const IntType itValue)
{ return static_cast<double>(itValue) * 0.0393700787; }
/* -- Calculate times per second based on an interval ---------------------- */
constexpr static double UtilPerSec(const double dVal) { return 1.0 / dVal; }
/* -- Smooth out a value (i.e. CPU usage) ---------------------------------- */
template<typename T, T tAlpha=0.1> requires is_arithmetic_v<T>
  static T UtilSmooth(const T tValue, T &tSmoothed)
{ // Calculate the new value and reset if it when invalid
  tSmoothed = tAlpha * tValue + (static_cast<T>(1) - tAlpha) * tSmoothed;
  // Return actual value
  return tValue;
}
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
