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
/* -- Dependencies --------------------------------------------------------- */
using namespace IStd::P;               using namespace IEndian::P;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* == Number is divisible by specified number ============================== */
static bool UtilIsDivisible(const double dNumber)
  { double dDummy; return modf(dNumber, &dDummy) == 0; }
/* -- Return -1 or 1 depending wether the value is positive or negative ---- */
template<typename IntType>
  requires StdIsSigned<IntType>
static IntType UtilSign(const IntType itValue)
  { return static_cast<IntType>(itValue > 0) -
      static_cast<IntType>(itValue < 0); }
/* ------------------------------------------------------------------------- */
template<typename IntType = double, typename FloatType = double>
  requires StdIsArithmatic<IntType> && StdIsFloat<FloatType>
static IntType UtilRound(const FloatType ftValue, const int iPrecision)
{ // Is 32-bit floating point?
  if constexpr(StdIsSame<FloatType, float>)
  { // Non-zero?
    if(ftValue != 0.0f)
    { // Round as float and return result
      const FloatType ftAmount = powf(10.0f, static_cast<float>(iPrecision));
      return static_cast<IntType>
        (floorf(ftValue * ftAmount + 0.5f) / ftAmount);
    }
  } // Is 64-bit floating point?
  else if constexpr(StdIsSame<FloatType, double>)
  { // Non-zero?
    if(ftValue != 0.0)
    { // Round as double and return result
      const FloatType ftAmount = pow(10.0, static_cast<double>(iPrecision));
      return static_cast<IntType>(floor(ftValue * ftAmount + 0.5) / ftAmount);
    }
  } // Zero or invalid type
  return static_cast<IntType>(0);
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
template<class ListType>
  requires StdIsClass<ListType>
static bool UtilReserveList(ListType &ltList, const size_t stCount)
{ // Return if specified value is outrageous!
  if(stCount > ltList.max_size()) return false;
  // Reserve room for this many floats
  ltList.reserve(stCount);
  // Success
  return true;
}
/* -- Convert const object to non-const ------------------------------------ */
template<typename Type>static Type &UtilToNonConst(const Type &tSrc)
  { return const_cast<Type&>(tSrc); }
/* -- Brute cast a 32-bit float to 32-bit integer -------------------------- */
static uint32_t UtilCastFloatToInt32(const float fV)
  { return StdBruteCast<uint32_t>(fV); }
/* -- Brute cast a 32-bit integer to a 32-bit float ------------------------ */
static float UtilCastInt32ToFloat(const uint32_t ulV)
  { return StdBruteCast<float>(ulV); }
/* -- Brute cast a 64-bit integer to a 64-bit double ----------------------- */
static double UtilCastInt64ToDouble(const uint64_t ullV)
  { return StdBruteCast<double>(ullV); }
/* -- Brute cast a 64-bit double to a 64-bit integer ----------------------- */
static uint64_t UtilCastDoubleToInt64(const double dV)
  { return StdBruteCast<uint64_t>(dV); }
/* -- Helper functions to force float byte ordering ------------------------ */
static float UtilToF32LE(const float fV)
  { return UtilCastInt32ToFloat(EndianTo32LE(UtilCastFloatToInt32(fV))); }
/* ------------------------------------------------------------------------- */
static float UtilToF32BE(const float fV)
  { return UtilCastInt32ToFloat(EndianTo32BE(UtilCastFloatToInt32(fV))); }
/* ------------------------------------------------------------------------- */
static double UtilToF64LE(const double dV)
  { return UtilCastInt64ToDouble(EndianTo64LE(UtilCastDoubleToInt64(dV))); }
/* ------------------------------------------------------------------------- */
static double UtilToF64BE(const double dV)
  { return UtilCastInt64ToDouble(EndianTo64BE(UtilCastDoubleToInt64(dV))); }
/* -- Convert to little endian integer ------------------------------------- */
template<typename IntType>
  requires StdIsIntegral<IntType>
static IntType UtilToLittleEndian(const IntType itValue)
{ // Convert 16-bit big-endian integer to little-endian
  if constexpr(sizeof(IntType) == sizeof(uint16_t))
    return EndianTo16LE<IntType>(itValue);
  // Convert 32-bit big-endian integer to little-endian
  else if constexpr(sizeof(IntType) == sizeof(uint32_t))
    return EndianTo32LE<IntType>(itValue);
  // Convert 64-bit big-endian integer to little-endian
  else if constexpr(sizeof(IntType) == sizeof(uint64_t))
    return EndianTo64LE<IntType>(itValue);
  // Don't convert anything else
  else return itValue;
}
/* -- Convert to big endian integer ---------------------------------------- */
template<typename IntType>
  requires StdIsIntegral<IntType>
static IntType UtilToBigEndian(const IntType itValue)
{ // Convert 16-bit big-endian integer to big-endian
  if constexpr(sizeof(IntType) == sizeof(uint16_t))
    return EndianTo16BE<IntType>(itValue);
  // Convert 32-bit big-endian integer to big-endian
  else if constexpr(sizeof(IntType) == sizeof(uint32_t))
    return EndianTo32BE<IntType>(itValue);
  // Convert 64-bit big-endian integer to big-endian
  else if constexpr(sizeof(IntType) == sizeof(uint64_t))
    return EndianTo64BE<IntType>(itValue);
  // Don't convert anything else
  else return itValue;
}
/* -- Convert float normal back to integer --------------------------------- */
template<typename IntTypeRet,
         typename IntTypeInternal=uint8_t,
         typename IntTypeParam>
requires StdIsIntegral<IntTypeRet> &&
         StdIsIntegral<IntTypeInternal> &&
         StdIsFloat<IntTypeParam>
static IntTypeRet UtilDenormalise(const IntTypeParam itpVal)
{ // Do the conversion and return it
  return static_cast<IntTypeRet>(itpVal * StdLimits<IntTypeInternal>::max());
}
/* -- Convert integer to float between 0 and 1 ----------------------------- */
template<typename IntTypeRet, typename IntTypeParam>
  requires StdIsFloat<IntTypeRet> || StdIsIntegral<IntTypeParam>
static IntTypeRet UtilNormalise(const IntTypeParam itpVal)
  { return static_cast<IntTypeRet>(itpVal) / StdLimits<IntTypeParam>::max(); }
/* -- Extract a part of an integer ----------------------------------------- */
template<typename IntTypeRet,
         size_t stShift = 0,
         typename IntTypeInternal = uint8_t,
         typename IntTypeExternal = unsigned>
requires (stShift <= sizeof(IntTypeExternal) * 8) &&
          StdIsIntegral<IntTypeInternal>
static IntTypeRet UtilExtract(const IntTypeExternal iteV)
  { return static_cast<IntTypeInternal>(iteV >> stShift); }
/* -- Grab part of an integer and normalise it between 0 and 1 ------------- */
template<typename IntTypeRet = float,
         size_t stShift = 0,
         typename IntTypeInternal = uint8_t,
         typename IntTypeExternal = unsigned>
requires StdIsFloat<IntTypeRet> &&
         (stShift <= sizeof(IntTypeExternal) * 8) &&
         StdIsIntegral<IntTypeInternal>
static IntTypeRet UtilNormaliseEx(const IntTypeExternal iteV)
  { return UtilNormalise<IntTypeRet>(UtilExtract<IntTypeInternal,
      stShift, IntTypeInternal>(iteV)); }
/* -- Scale a value from (0-max) to a different min-max -------------------- */
template<typename DestIntType, typename PhysIntType, typename VirtIntType>
  requires StdIsArithmatic<DestIntType> && StdIsArithmatic<PhysIntType> &&
           StdIsArithmatic<VirtIntType>
static DestIntType UtilScaleValue(const DestIntType itV,
  const PhysIntType itPMax, const VirtIntType itVMin, const VirtIntType itVMax)
{ return static_cast<DestIntType>(itVMin) + itV /
    itPMax * static_cast<DestIntType>(itVMax); }
/* -- Returns if specified integer would overflow specified type ----------- */
template<typename IntTypeTarget, typename IntTypeSource>
  requires (StdIsIntegral<IntTypeSource> || StdIsEnum<IntTypeSource>) &&
           (StdIsIntegral<IntTypeTarget> || StdIsEnum<IntTypeTarget>)
constexpr static bool UtilIntWillOverflow(const IntTypeSource itsValue)
{ // If both types are signed?
  if constexpr(StdIsSigned<IntTypeSource> == StdIsSigned<IntTypeTarget>)
  { // Both are signed so no overflow possible if source size less
    if constexpr(sizeof(IntTypeSource) <= sizeof(IntTypeTarget))
      return false;
    // Return true if overflowed, false if not
    else return itsValue <
      static_cast<IntTypeSource>(StdLimits<IntTypeTarget>::min()) ||
      itsValue > static_cast<IntTypeSource>(StdLimits<IntTypeTarget>::max());
  } // Both types aren't signed so if source is signed?
  else if constexpr(StdIsSigned<IntTypeSource>)
    // Source is signed, target is unsigned so return if negative or overflow
    return itsValue < 0 ||
      static_cast<StdMakeUnsigned<IntTypeSource>>(itsValue) >
        StdLimits<IntTypeTarget>::max();
  // Source is unsigned, target is signed so return if overflow
  else return itsValue > static_cast<StdMakeUnsigned<IntTypeTarget>>
    (StdLimits<IntTypeTarget>::max());
}
/* -- Join two integers to make a bigger integer --------------------------- */
template<typename RIntType = uint16_t, typename HighType, typename LowType>
  requires StdIsIntegral<RIntType> &&
    StdIsIntegral<HighType> && (sizeof(HighType) >= sizeof(uint8_t)) &&
    StdIsIntegral<LowType> && (sizeof(LowType) >= sizeof(uint8_t))
static RIntType UtilMakeWord(const HighType htV, const LowType ltV)
  { return static_cast<RIntType>
      ((static_cast<uint16_t>(htV) << 8) | (ltV & 0xff)); }
/* ------------------------------------------------------------------------- */
template<typename RIntType = uint32_t, typename HighType, typename LowType>
  requires StdIsIntegral<RIntType> &&
    StdIsIntegral<HighType> && (sizeof(HighType) >= sizeof(uint16_t)) &&
    StdIsIntegral<LowType> && (sizeof(LowType) >= sizeof(uint16_t))
static RIntType UtilMakeDWord(const HighType htV, const LowType ltV)
  { return static_cast<RIntType>
      ((static_cast<uint32_t>(htV) << 16) | (ltV & 0xffff)); }
/* ------------------------------------------------------------------------- */
template<typename RIntType = uint64_t, typename HighType, typename LowType>
  requires StdIsIntegral<RIntType> &&
    StdIsIntegral<HighType> && (sizeof(HighType) >= sizeof(uint32_t)) &&
    StdIsIntegral<LowType> && (sizeof(LowType) >= sizeof(uint32_t))
static RIntType UtilMakeQWord(const HighType htV, const LowType ltV)
  { return static_cast<RIntType>
      ((static_cast<uint64_t>(htV) << 32) | (ltV & 0xffffffff)); }
/* -- Return lowest and highest 8-bits of integer -------------------------- */
template<typename RIntType = uint8_t, typename IntType>
  requires StdIsIntegral<RIntType> && StdIsIntegral<IntType> &&
    (sizeof(IntType) >= sizeof(uint16_t))
static RIntType UtilLowByte(const IntType itVal)
  { return static_cast<RIntType>(itVal & 0x00ff); }
/* ------------------------------------------------------------------------- */
template<typename RIntType = uint8_t, typename IntType>
  requires StdIsIntegral<RIntType> && StdIsIntegral<IntType> &&
    (sizeof(IntType) >= sizeof(uint16_t))
static RIntType UtilHighByte(const IntType itVal)
  { return static_cast<RIntType>((itVal & 0xff00) >> 8); }
/* -- Return lowest and highest 16-bits of integer ------------------------- */
template<typename RIntType = uint16_t, typename IntType>
  requires StdIsIntegral<RIntType> && StdIsIntegral<IntType> &&
    (sizeof(IntType) >= sizeof(uint32_t))
static RIntType UtilLowWord(const IntType itVal)
  { return static_cast<RIntType>(itVal & 0x0000ffff); }
/* ------------------------------------------------------------------------- */
template<typename RIntType = uint16_t, typename IntType>
  requires StdIsIntegral<RIntType> && StdIsIntegral<IntType> &&
    (sizeof(IntType) >= sizeof(uint32_t))
static RIntType UtilHighWord(const IntType itVal)
  { return static_cast<RIntType>((itVal & 0xffff0000) >> 16); }
/* -- Return lowest and highest 32-bits of integer ------------------------- */
template<typename RIntType = uint32_t, typename IntType>
  requires StdIsIntegral<RIntType> && StdIsIntegral<IntType> &&
    (sizeof(IntType) >= sizeof(uint64_t))
static RIntType UtilLowDWord(const IntType itVal)
  { return static_cast<RIntType>(itVal & 0x00000000ffffffff); }
/* ------------------------------------------------------------------------- */
template<typename RIntType = uint32_t, typename IntType>
  requires StdIsIntegral<RIntType> && StdIsIntegral<IntType> &&
    (sizeof(IntType) >= sizeof(uint64_t))
static RIntType UtilHighDWord(const IntType itVal)
  { return static_cast<RIntType>((itVal & 0xffffffff00000000) >> 32); }
/* -- Return lowest or highest number out of two --------------------------- */
template<typename IntType1, typename IntType2>
  requires StdIsArithmatic<IntType1> && StdIsArithmatic<IntType2> &&
    StdIsSame<IntType1, IntType2>
static IntType1 UtilMinimum(const IntType1 itOne, const IntType2 itTwo)
  { return itOne < static_cast<IntType1>(itTwo) ?
      itOne : static_cast<IntType1>(itTwo); }
/* ------------------------------------------------------------------------- */
template<typename IntType1, typename IntType2>
  requires StdIsArithmatic<IntType1> && StdIsArithmatic<IntType2> &&
    StdIsSame<IntType1, IntType2>
static IntType1 UtilMaximum(const IntType1 itOne, const IntType2 itTwo)
  { return itOne > static_cast<IntType1>(itTwo) ?
      itOne : static_cast<IntType1>(itTwo); }
/* -- Clamp a number between two values ------------------------------------ */
template<typename IntType, typename IntTypeMin, typename IntTypeMax>
  requires StdIsArithmatic<IntType> && StdIsArithmatic<IntTypeMin> &&
    StdIsArithmatic<IntTypeMax>
static IntType UtilClamp(const IntType itVal, const IntTypeMin itMin,
  const IntTypeMax itMax)
{ return UtilMaximum(static_cast<IntType>(itMin),
    UtilMinimum(static_cast<IntType>(itMax), itVal)); }
/* -- Make a percentage ---------------------------------------------------- */
template<typename IntType1, typename IntType2, typename RIntType = double>
  requires StdIsArithmatic<IntType1> && StdIsArithmatic<IntType2> &&
    StdIsFloat<RIntType>
static RIntType UtilMakePercentage(const IntType1 itCurrent,
  const IntType2 itMaximum, const RIntType ritMulti = 100)
{ return static_cast<RIntType>(itCurrent) /
    static_cast<RIntType>(itMaximum) * ritMulti; }
/* -- Calculate distance between two values -------------------------------- */
template<typename IntType>
  requires StdIsArithmatic<IntType>
static IntType UtilDistance(const IntType itX, const IntType itY)
  { return itX > itY ? itX - itY : itY - itX; }
/* -- Round to nearest value ----------------------------------------------- */
template<typename IntType>
  requires StdIsArithmatic<IntType>
static IntType UtilNearest(const IntType itValue,
  const IntType itMultiple)
{ return (itValue + itMultiple / 2) / itMultiple * itMultiple; }
/* -- Returns the nearest power of two to specified number ----------------- */
template<typename RIntType, typename IntType>
  requires StdIsArithmatic<RIntType> && StdIsArithmatic<IntType>
static RIntType UtilNearestPow2(const IntType itValue)
  { return static_cast<RIntType>(pow(2, ceil(log2(itValue)))); }
/* -- If variable would overflow another type then return its maximum ------ */
template<typename RIntType, typename IntType>
  requires StdIsArithmatic<RIntType> && (StdIsArithmatic<IntType> ||
    StdIsEnum<IntType>)
static RIntType UtilIntOrMax(const IntType itValue)
  { return UtilIntWillOverflow<RIntType, IntType>(itValue) ?
      StdLimits<RIntType>::max() : static_cast<RIntType>(itValue); }
/* -- Convert millimetres to inches ---------------------------------------- */
template<typename IntType>
  requires StdIsArithmatic<IntType>
static double UtilMillimetresToInches(const IntType itValue)
  { return static_cast<double>(itValue) * 0.0393700787; }
/* -- Calculate times per second based on an interval ---------------------- */
constexpr static double UtilPerSec(const double dVal) { return 1.0 / dVal; }
/* -- Smooth out a value (i.e. CPU usage) ---------------------------------- */
template<typename IntType, IntType itAlpha = 0.1>
  requires StdIsArithmatic<IntType>
static IntType UtilSmooth(const IntType itValue, IntType &tSmoothed)
{ // Calculate the new value and reset if it when invalid
  tSmoothed = itAlpha * itValue +
    (static_cast<IntType>(1) - itAlpha) * tSmoothed;
  // Return actual value
  return itValue;
}
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
