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
// template<typename FloatType=double>
//  static bool IsNormal(const FloatType ftValue)
//    { return fpclassify(iitValue) == FP_NORMAL; }
/* -- Return -1 or 1 depending wether the value is positive or negative ---- */
template<typename IntType>IntType UtilSign(const IntType itValue)
  { static_assert(is_signed_v<IntType>, "Type must be signed.");
    return static_cast<IntType>(itValue > 0) -
           static_cast<IntType>(itValue < 0); }
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
static uint8_t UtilReverseByte(int iByte)
{ // We shall use a lookup table for this as it is faster
  iByte = ((iByte & 0x55) << 1) | ((iByte & 0xAA) >> 1);
  iByte = ((iByte & 0x33) << 2) | ((iByte & 0xCC) >> 2);
  iByte = ((iByte & 0x0F) << 4) | ((iByte & 0xF0) >> 4);
  return static_cast<uint8_t>(iByte);
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
/* -- Swap class functors -------------------------------------------------- */
struct UtilSwap32LEFunctor             // Swap 32-bit little <-> big integer
  { uint32_t v;                        // Output value (32-bit)
    explicit UtilSwap32LEFunctor(uint32_t dwV) : v(UtilToI32LE(dwV)) {} };
struct UtilSwap32BEFunctor             // Swap 32-bit big <-> little integer
  { uint32_t v;                        // Output value (32-bit)
    explicit UtilSwap32BEFunctor(uint32_t dwV) : v(UtilToI32BE(dwV)) {} };
struct UtilSwap64LEFunctor             // Swap 64-bit little <-> big integer
  { uint64_t v;                        // Output value (64-bit)
    explicit UtilSwap64LEFunctor(uint64_t qwV) : v(UtilToI64LE(qwV)) {} };
struct UtilSwap64BEFunctor             // Swap 64-bit big <-> little integer
  { uint64_t v;                        // Output value (64-bit)
    explicit UtilSwap64BEFunctor(uint64_t qwV) : v(UtilToI64BE(qwV)) {} };
/* -- Convert const object to non-const ------------------------------------ */
template<typename Type>static Type &UtilToNonConst(const Type &tSrc)
  { return const_cast<Type&>(tSrc); }
/* -- Brute cast one type to another --------------------------------------- */
template<typename TypeDst, typename TypeSrc>
  static TypeDst UtilBruteCast(const TypeSrc tsV)
{ // Make sure sizes are the same
  static_assert(sizeof(TypeDst) == sizeof(TypeSrc),
    "Size of source and destination types must be equal");
  // Make sure we can copy both
  static_assert(is_trivially_copyable_v<TypeSrc>,
    "Source type must be trivially copyable");
  static_assert(is_trivially_copyable_v<TypeDst>,
    "Destination type must be trivially copyable");
  // Now cast it to the requested type
  return bit_cast<TypeDst>(tsV);
}
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
/* -- Convert to little endian integer ------------------------------------- */
template<typename IntType>IntType UtilToLittleEndian(const IntType itValue)
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
template<typename IntType>IntType UtilToBigEndian(const IntType itValue)
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
/* -- Scale a value from (0-max) to a different min-max -------------------- */
template<typename DestIntType, typename PhysIntType, typename VirtIntType>
DestIntType UtilScaleValue(const DestIntType itV, const PhysIntType itPMax,
                           const VirtIntType itVMin, const VirtIntType itVMax)
  { return static_cast<DestIntType>(itVMin) + itV /
      itPMax * static_cast<DestIntType>(itVMax); }
/* -- Returns if specified integer would overflow specified type ----------- */
template<typename IntTypeTarget,       // Target requested type
         typename IntTypeSource>       // Source type (function parameter)
constexpr bool UtilIntWillOverflow(const IntTypeSource itsValue) {
  // If both types are signed?
  if constexpr(is_signed_v<IntTypeSource> == is_signed_v<IntTypeTarget>)
  { // Both are signed so no overflow possible if source size less
    if constexpr(sizeof(IntTypeSource) <= sizeof(IntTypeTarget))
      return false;
    // Return true if overflowed, false if not
    else return itsValue < static_cast<IntTypeSource>
        (numeric_limits<IntTypeTarget>::lowest()) ||
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
  static uint16_t UtilMakeWord(const IntTypeHigh ithV, const IntTypeLow itlV)
    { return static_cast<uint16_t>((static_cast<uint16_t>(ithV) << 8) |
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
    { return itOne < static_cast<IntType1>(itTwo) ?
        itOne : static_cast<IntType1>(itTwo); }
template<typename IntType1,typename IntType2>
  static IntType1 UtilMaximum(const IntType1 itOne, const IntType2 itTwo)
    { return itOne > static_cast<IntType1>(itTwo) ?
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
    { return (itValue + itMultiple / 2) / itMultiple * itMultiple; }
/* -- Returns the nearest power of two to specified number ----------------- */
template<typename RetType, typename IntType>
  static const RetType UtilNearestPow2(const IntType itValue)
    { return static_cast<RetType>(pow(2, ceil(log2(itValue)))); }
/* -- If variable would overflow another type then return its maximum ------ */
template<typename RetType, typename IntType>
  static RetType UtilIntOrMax(const IntType itValue)
    { return UtilIntWillOverflow<RetType, IntType>(itValue) ?
        numeric_limits<RetType>::max() : static_cast<RetType>(itValue); }
/* -- Convert millimetres to inches ---------------------------------------- */
template<typename IntType>
  static double UtilMillimetresToInches(const IntType itValue)
    { return static_cast<double>(itValue) * 0.0393700787; }
/* -- Returns true if two numbers are equal (Omit != and == warnings) ------ */
template<typename FloatType>
  static bool UtilIsFloatEqual(const FloatType f1, const FloatType f2)
    { return f1 >= f2 && f1 <= f2; }
/* -- Returns true if two numbers are NOT equal (Omit != and == warnings) -- */
template<typename FloatType>
  static bool UtilIsFloatNotEqual(const FloatType f1, const FloatType f2)
    { return f1 < f2 || f1 > f2; }
/* -- Initialise an array of the specified value --------------------------- */
namespace MakeFilledContainer
{ /* -- Fill with specified value ------------------------------------------ */
  template<typename ItemType>          // Container element type
  constexpr ItemType Value(const size_t, const ItemType &itValue)
    { return itValue; }
  /* -- Select indices ----------------------------------------------------- */
  template<class ContainerType,        // Container type (i.e. array<>)
           typename ItemType,          // Container element type
           size_t stNumItems,          // Container maximum elements
           size_t... Is>               // Parameter id
  constexpr ContainerType Select(const ItemType &itValue,
    index_sequence<Is...>)
      { return { Value<ItemType>(Is, itValue)... }; }
  /* -- Entry function ----------------------------------------------------- */
  template<typename ContainerType,
           class ItemType = ContainerType::value_type,
           size_t stNumItems = tuple_size_v<ContainerType>>
  constexpr ContainerType UtilMkFilledContainer(const ItemType &itValue)
    { return Select<ContainerType, ItemType, stNumItems>
        (itValue, make_index_sequence<stNumItems>{}); }
};/* ----------------------------------------------------------------------- */
using MakeFilledContainer::UtilMkFilledContainer;
/* -- Initialise an array of the specified value --------------------------- */
namespace MakeFilledClassContainer
{ /* -- Fill with specified value ------------------------------------------ */
  template<class ClassType,
           typename ArgType>
  constexpr ClassType Value(const size_t, ArgType &atValue)
    { return ClassType{ atValue++ }; }
  /* -- Select indices ----------------------------------------------------- */
  template<class ContainerType,
           typename ArgType,
           class ClassType = ContainerType::value_type,
           size_t... Is>
  constexpr ContainerType Select(ArgType &atValue, index_sequence<Is...>)
    { return { Value<ClassType, ArgType>(Is, atValue)... }; }
  /* -- Entry function ----------------------------------------------------- */
  template<class ContainerType,
           typename ArgType,
           size_t stNumItems = tuple_size_v<ContainerType>>
  constexpr ContainerType UtilMkFilledClassContainer(ArgType atValue=0)
    { return Select<ContainerType, ArgType>
        (atValue, make_index_sequence<stNumItems>{}); }
};/* ----------------------------------------------------------------------- */
using MakeFilledClassContainer::UtilMkFilledClassContainer;
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
    { return static_cast<RetType>(1 << itPos % CHAR_BIT); }
template<typename PtrType,typename IntType>
  static void UtilBitSet(PtrType*const ptDst, const IntType itPos)
    { ptDst[UtilBitToByte(itPos)] |=
        UtilBitMask<remove_pointer_t<PtrType>,IntType>(itPos); }
template<typename PtrType,typename IntType>
  static bool UtilBitTest(PtrType*const ptDst, const IntType itPos)
    { return !!(ptDst[UtilBitToByte(itPos)] &
        UtilBitMask<remove_pointer_t<PtrType>,IntType>(itPos)); }
template<typename PtrType,typename IntType>
  static void UtilBitClear(PtrType*const ptDst, const IntType itPos)
    { ptDst[UtilBitToByte(itPos)] &=
        ~UtilBitMask<remove_pointer_t<PtrType>,IntType>(itPos); }
template<typename PtrType,typename IntType>
  static void UtilBitFlip(PtrType*const ptDst, const IntType itPos)
    { ptDst[UtilBitToByte(itPos)] ^=
        UtilBitMask<remove_pointer_t<PtrType>,IntType>(itPos); }
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
