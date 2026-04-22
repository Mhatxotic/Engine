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
template<typename T> requires StdIsSigned<T> static T UtilSign(const T tValue)
  { return static_cast<T>(tValue > 0) - static_cast<T>(tValue < 0); }
/* ------------------------------------------------------------------------- */
template<typename IT=double, typename FT=double>
  requires StdIsArithmatic<IT> && StdIsFloat<FT>
static IT UtilRound(const FT ftValue, const int iPrecision)
{ // Is 32-bit floating point?
  if constexpr(StdIsSame<FT, float>)
  { // Non-zero?
    if(ftValue != 0.0f)
    { // Round as float and return result
      const FT ftAmount = powf(10.0f, static_cast<float>(iPrecision));
      return static_cast<IT>(floorf(ftValue * ftAmount + 0.5f) / ftAmount);
    }
  } // Is 64-bit floating point?
  else if constexpr(StdIsSame<FT, double>)
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
template<class ListType> requires StdIsClass<ListType>
  static bool UtilReserveList(ListType &ltList, const size_t stCount)
{ // Return if specified value is outrageous!
  if(stCount > ltList.max_size()) return false;
  // Reserve room for this many flaots
  ltList.reserve(stCount);
  // Success
  return true;
}
/* -- Helper functions to force integer byte ordering ---------------------- */
template<typename IT>
  requires (sizeof(IT) == sizeof(uint16_t)) && StdIsInteger<IT>
static IT UtilToI16LE(const IT itV)
  { return static_cast<IT>(STRICT_U16LE(itV)); }
/* ------------------------------------------------------------------------- */
template<typename IT>
  requires (sizeof(IT) == sizeof(uint16_t)) && StdIsInteger<IT>
static IT UtilToI16BE(const IT itV)
  { return static_cast<IT>(STRICT_U16BE(itV)); }
/* ------------------------------------------------------------------------- */
template<typename IT>
  requires (sizeof(IT) == sizeof(uint32_t)) && StdIsInteger<IT>
static IT UtilToI32LE(const IT itV)
  { return static_cast<IT>(STRICT_U32LE(itV)); }
/* ------------------------------------------------------------------------- */
template<typename IT>
  requires (sizeof(IT) == sizeof(uint32_t)) && StdIsInteger<IT>
static IT UtilToI32BE(const IT itV)
  { return static_cast<IT>(STRICT_U32BE(itV)); }
/* ------------------------------------------------------------------------- */
template<typename IT>
  requires (sizeof(IT) == sizeof(uint64_t)) && StdIsInteger<IT>
static IT UtilToI64LE(const IT itV)
  { return static_cast<IT>(STRICT_U64LE(itV)); }
/* ------------------------------------------------------------------------- */
template<typename IT>
  requires (sizeof(IT) == sizeof(uint64_t)) && StdIsInteger<IT>
static IT UtilToI64BE(const IT itV)
  { return static_cast<IT>(STRICT_U64BE(itV)); }
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
template<typename IT>
  requires StdIsInteger<IT>
static IT UtilToLittleEndian(const IT itValue)
{ // Convert 16-bit big-endian integer to little-endian
  if constexpr(sizeof(IT) == sizeof(uint16_t))
    return UtilToI16LE<IT>(itValue);
  // Convert 32-bit big-endian integer to little-endian
  else if constexpr(sizeof(IT) == sizeof(uint32_t))
    return UtilToI32LE<IT>(itValue);
  // Convert 64-bit big-endian integer to little-endian
  else if constexpr(sizeof(IT) == sizeof(uint64_t))
    return UtilToI64LE<IT>(itValue);
  // Don't convert anything else
  else return itValue;
}
/* -- Convert to big endian integer ---------------------------------------- */
template<typename IT> requires StdIsInteger<IT>
  static IT UtilToBigEndian(const IT itValue)
{ // Convert 16-bit big-endian integer to big-endian
  if constexpr(sizeof(IT) == sizeof(uint16_t))
    return UtilToI16BE<IT>(itValue);
  // Convert 32-bit big-endian integer to big-endian
  else if constexpr(sizeof(IT) == sizeof(uint32_t))
    return UtilToI32BE<IT>(itValue);
  // Convert 64-bit big-endian integer to big-endian
  else if constexpr(sizeof(IT) == sizeof(uint64_t))
    return UtilToI64BE<IT>(itValue);
  // Don't convert anything else
  else return itValue;
}
/* -- Convert float normal back to integer --------------------------------- */
template<typename IntTypeRet,
         typename IntTypeInternal=uint8_t,
         typename IntTypeParam>
requires StdIsInteger<IntTypeRet> &&
         StdIsInteger<IntTypeInternal> &&
         StdIsFloat<IntTypeParam>
static IntTypeRet UtilDenormalise(const IntTypeParam itpVal)
{ // Do the conversion and return it
  return static_cast<IntTypeRet>(itpVal * StdLimits<IntTypeInternal>::max());
}
/* -- Convert integer to float between 0 and 1 ----------------------------- */
template<typename IntTypeRet, typename IntTypeParam>
  requires StdIsFloat<IntTypeRet> || StdIsInteger<IntTypeParam>
static IntTypeRet UtilNormalise(const IntTypeParam itpVal)
  { return static_cast<IntTypeRet>(itpVal) / StdLimits<IntTypeParam>::max(); }
/* -- Extract a part of an integer ----------------------------------------- */
template<typename IntTypeRet,
         size_t stShift=0,
         typename IntTypeInternal=uint8_t,
         typename IntTypeExternal=unsigned int>
requires (stShift <= sizeof(IntTypeExternal) * 8) &&
          StdIsInteger<IntTypeInternal>
static IntTypeRet UtilExtract(const IntTypeExternal iteV)
  { return static_cast<IntTypeInternal>(iteV >> stShift); }
/* -- Grab part of an integer and normalise it between 0 and 1 ------------- */
template<typename IntTypeRet=float,
         size_t stShift=0,
         typename IntTypeInternal=uint8_t,
         typename IntTypeExternal=unsigned int>
requires StdIsFloat<IntTypeRet> &&
         (stShift <= sizeof(IntTypeExternal) * 8) &&
         StdIsInteger<IntTypeInternal>
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
  requires (StdIsInteger<IntTypeSource> || StdIsEnum<IntTypeSource>) &&
           (StdIsInteger<IntTypeTarget> || StdIsEnum<IntTypeTarget>)
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
           static_cast<make_unsigned_t<IntTypeSource>>(itsValue) >
             StdLimits<IntTypeTarget>::max();
  // Source is unsigned, target is signed so return if overflow
  else return itsValue > static_cast<make_unsigned_t<IntTypeTarget>>
    (StdLimits<IntTypeTarget>::max());
}
/* -- Join two integers to make a bigger integer --------------------------- */
template<typename HT,typename LT> requires StdIsInteger<HT> && StdIsInteger<LT>
  static uint16_t UtilMakeWord(const HT htV, const LT ltV)
{ return static_cast<uint16_t>((static_cast<uint16_t>(htV) << 8) |
   (static_cast<uint16_t>(ltV) & 0xff)); }
/* ------------------------------------------------------------------------- */
template<typename HT,typename LT> requires StdIsInteger<HT> && StdIsInteger<LT>
  static uint32_t UtilMakeDWord(const HT htV, const LT ltV)
{ return static_cast<uint32_t>((static_cast<uint32_t>(htV) << 16) |
    (static_cast<uint32_t>(ltV) & 0xffff)); }
/* ------------------------------------------------------------------------- */
template<typename HT,typename LT> requires StdIsInteger<HT> && StdIsInteger<LT>
  static uint64_t UtilMakeQWord(const HT htV, const LT ltV)
{ return static_cast<uint64_t>((static_cast<uint64_t>(htV) << 32) |
    (static_cast<uint64_t>(ltV) & 0xffffffff)); }
/* -- Return lowest and highest 8-bits of integer -------------------------- */
template<typename T> requires StdIsInteger<T>
  static uint8_t UtilLowByte(const T tVal)
{ return static_cast<uint8_t>(tVal & 0x00ff); }
/* ------------------------------------------------------------------------- */
template<typename T> requires StdIsInteger<T>
  static uint8_t UtilHighByte(const T tVal)
{ return static_cast<uint8_t>((tVal & 0xff00) >> 8); }
/* -- Return lowest and highest 16-bits of integer ------------------------- */
template<typename T> requires StdIsInteger<T>
  static uint16_t UtilLowWord(const T tVal)
{ return static_cast<uint16_t>(tVal & 0x0000ffff); }
/* ------------------------------------------------------------------------- */
template<typename T> requires StdIsInteger<T>
  static uint16_t UtilHighWord(const T tVal)
{ return static_cast<uint16_t>((tVal & 0xffff0000) >> 16); }
/* -- Return lowest and highest 32-bits of integer ------------------------- */
template<typename T> requires StdIsInteger<T>
  static uint32_t UtilLowDWord(const T tVal)
{ return static_cast<uint32_t>(tVal & 0x00000000ffffffff); }
/* ------------------------------------------------------------------------- */
template<typename T> requires StdIsInteger<T>
  static uint32_t UtilHighDWord(const T tVal)
{ return static_cast<uint32_t>((tVal & 0xffffffff00000000) >> 32); }
/* -- Return lowest or highest number out of two --------------------------- */
template<typename T1,typename T2>
  requires StdIsArithmatic<T1> && StdIsArithmatic<T2> && StdIsSame<T1, T2>
static T1 UtilMinimum(const T1 tOne, const T2 tTwo)
  { return tOne < static_cast<T1>(tTwo) ? tOne : static_cast<T1>(tTwo); }
/* ------------------------------------------------------------------------- */
template<typename T1, typename T2>
  requires StdIsArithmatic<T1> && StdIsArithmatic<T2> && StdIsSame<T1, T2>
static T1 UtilMaximum(const T1 tOne, const T2 tTwo)
  { return tOne > static_cast<T1>(tTwo) ? tOne : static_cast<T1>(tTwo); }
/* -- Clamp a number between two values ------------------------------------ */
template<typename TVAL, typename TMIN, typename TMAX>
  requires StdIsArithmatic<TVAL> && StdIsArithmatic<TMIN> &&
    StdIsArithmatic<TMAX>
static TVAL UtilClamp(const TVAL tVal, const TMIN tMin, const TMAX tMax)
{ return UtilMaximum(static_cast<TVAL>(tMin),
    UtilMinimum(static_cast<TVAL>(tMax), tVal)); }
/* -- Make a percentage ---------------------------------------------------- */
template<typename T1, typename T2, typename RT=double>
  requires StdIsArithmatic<T1> && StdIsArithmatic<T2> && StdIsFloat<RT>
static RT UtilMakePercentage(const T1 tCurrent, const T2 tMaximum,
  const RT rMulti=100)
{ return static_cast<RT>(tCurrent) / static_cast<RT>(tMaximum) * rMulti; }
/* -- Calculate distance between two values -------------------------------- */
template<typename T>
  requires StdIsArithmatic<T>
static T UtilDistance(const T tX, const T tY)
  { return tX > tY ? tX - tY : tY - tX; }
/* -- Round to nearest value ----------------------------------------------- */
template<typename IT>
  requires StdIsArithmatic<IT>
static IT UtilNearest(const IT itValue,
  const IT itMultiple)
{ return (itValue + itMultiple / 2) / itMultiple * itMultiple; }
/* -- Returns the nearest power of two to specified number ----------------- */
template<typename RT, typename IT>
  requires StdIsArithmatic<RT> && StdIsArithmatic<IT>
static RT UtilNearestPow2(const IT itValue)
  { return static_cast<RT>(pow(2, ceil(log2(itValue)))); }
/* -- If variable would overflow another type then return its maximum ------ */
template<typename RT, typename IT>
  requires StdIsArithmatic<RT> && (StdIsArithmatic<IT> || StdIsEnum<IT>)
static RT UtilIntOrMax(const IT itValue)
  { return UtilIntWillOverflow<RT, IT>(itValue) ?
      StdLimits<RT>::max() : static_cast<RT>(itValue); }
/* -- Convert millimetres to inches ---------------------------------------- */
template<typename T> requires StdIsArithmatic<T>
  static double UtilMillimetresToInches(const T tValue)
{ return static_cast<double>(tValue) * 0.0393700787; }
/* -- Calculate times per second based on an interval ---------------------- */
constexpr static double UtilPerSec(const double dVal) { return 1.0 / dVal; }
/* -- Smooth out a value (i.e. CPU usage) ---------------------------------- */
template<typename T, T tAlpha=0.1> requires StdIsArithmatic<T>
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
