/* == FLAGS.HPP ============================================================ **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## This class helps with managing flags (a collection of booleans) or  ## **
** ## many bits in a byte. The compiler should hopefully optimise all of  ## **
** ## this complecated functory to single CPU instructions.               ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IFlags {                     // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IStd::P;
/* ------------------------------------------------------------------------- */
using std::memory_order_relaxed;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* -- Apply the concept directly to the template --------------------------- */
template<typename StorageType> class FlagsConst
{ /* -- Private typdefs ---------------------------------------------------- */
  template<typename Int>struct Extract { using Type = Int; };
  template<typename Int>struct Extract<StdAtomic<Int>> { using Type = Int; };
  /* -- Protected variables ------------------------------------- */ protected:
  StorageType stoValue;
  /* -- Public typedefs -------------------------------------------- */ public:
  using ValueType  = typename Extract<StorageType>::Type;
  using AtomicType = StdAtomic<ValueType>;
  /* -- Handles ==, !=, <, >, <=, >= --------------------------------------- */
  constexpr auto operator<=>(const FlagsConst &flOther) const
    { return static_cast<ValueType>(*this) <=>
             static_cast<ValueType>(flOther); }
  constexpr auto operator<=>(ValueType vtOther) const
    { return static_cast<ValueType>(*this) <=> vtOther; }
  /* -- Implicit conversion to plain integer ------------------------------- */
  constexpr operator ValueType() const
  { // If is atomic type, return the value in the atomic or the integer
    if constexpr(StdIsSame<StorageType, AtomicType>)
      return stoValue.load(memory_order_relaxed);
    else return stoValue;
  }
  /* -- Unary Operators ---------------------------------------------------- */
  constexpr ValueType operator~() const
    { return ~static_cast<ValueType>(*this); }
  /* -- Get a variable and return it as the specified type ----------------- */
  template<typename AnyType = ValueType>
    requires StdIsIntegral<AnyType>
  constexpr AnyType FlagGet() const { return static_cast<AnyType>(*this); }
  /* -- Is bits lesser than specified value? ------------------------------- */
  constexpr bool FlagIsLesser(const ValueType vtValue) const
    { return *this < vtValue; }
  /* -- Is bits lesser or equal to specified value? ------------------------ */
  constexpr bool FlagIsLesserEqual(const ValueType vtValue) const
    { return *this <= vtValue; }
  /* -- Is bits greater to specified value? -------------------------------- */
  constexpr bool FlagIsGreater(const ValueType vtValue) const
    { return *this > vtValue; }
  /* -- Is bits greater or equal to specified value? ----------------------- */
  constexpr bool FlagIsGreaterEqual(const ValueType vtValue) const
    { return *this >= vtValue; }
  /* -- Are there not any flags set? --------------------------------------- */
  constexpr bool FlagIsZero() const
    { return *this == static_cast<ValueType>(0); }
  /* -- Are any flags actually set? ---------------------------------------- */
  constexpr bool FlagIsNonZero() const { return !FlagIsZero(); }
  /* -- Return result of an & operation ------------------------------------ */
  constexpr ValueType FlagAnd(const ValueType vtValue) const
    { return (*this & vtValue); }
  /* -- Return result of an & operation with inverted source flags --------- */
  constexpr ValueType FlagAndInverted(const ValueType vtValue) const
    { return (~*this & vtValue); }
  /* -- Is flag set with specified value? ---------------------------------- */
  constexpr bool FlagIsSet(const ValueType vtValue) const
    { return FlagAnd(vtValue) == vtValue; }
  /* -- Is any flag set with specified value? ------------------------------ */
  constexpr bool FlagIsAnyOfSet(const ValueType vtValue) const
    { return FlagAnd(vtValue) != 0; }
  /* -- Is bit clear of specified value? ----------------------------------- */
  constexpr bool FlagIsClear(const ValueType vtValue) const
    { return FlagAndInverted(vtValue) == vtValue; }
  /* -- Is bit clear of specified value? ----------------------------------- */
  constexpr bool FlagIsAnyOfClear(const ValueType vtValue) const
    { return FlagAndInverted(vtValue) != 0; }
  /* -- Is flag set with specified value and clear with another? ----------- */
  constexpr bool FlagIsSetAndClear(const ValueType vtSet,
    const ValueType vtClear) const
  { return FlagIsSet(vtSet) && FlagIsClear(vtClear); }
  /* -- Flags are not masked in specified other flags? --------------------- */
  constexpr bool FlagIsNotInMask(const ValueType vtValue) const
    { return *this & ~vtValue; }
  /* -- Flags are masked in specified other flags? ------------------------- */
  constexpr bool FlagIsInMask(const ValueType vtValue) const
    { return !FlagIsNotInMask(vtValue); }
  /* -- Is bits set? ------------------------------------------------------- */
  constexpr bool FlagIsEqualToBool(const ValueType vtValue,
    const bool bState) const
  { return FlagIsSet(vtValue) == bState; }
  /* -- Is bits not set? --------------------------------------------------- */
  constexpr bool FlagIsNotEqualToBool(const ValueType vtValue,
    const bool bState) const
  { return FlagIsSet(vtValue) != bState; }
  /* -- Return one variable or another if set ------------------------------ */
  template<typename AnyType>
    constexpr const AnyType FlagIsSetTwo(const ValueType vtValue,
      const AnyType atSet, const AnyType atClear) const
  { return FlagIsSet(vtValue) ? atSet : atClear; }
  /* -- Constructor to initialise another FlagsConst struct ---------------- */
  constexpr explicit FlagsConst(StorageType &stoV) : stoValue(stoV) {}
  /* -- Constructor to initialise from an integer (if storage not atomic) -- */
  constexpr explicit FlagsConst(ValueType vtValue = 0)
    requires (!StdIsSame<StorageType, AtomicType>) : stoValue(vtValue) {}
  /* -- Constructor to initialise from an integer (if storage is atomic) --- */
  constexpr explicit FlagsConst(ValueType vtValue)
    requires (StdIsSame<StorageType, AtomicType>)
  { stoValue.store(vtValue, memory_order_relaxed); }
};/* ----------------------------------------------------------------------- */
template<typename StorageType>
  class Flags : public FlagsConst<StorageType>
{ /* -- Public typedefs -------------------------------------------- */ public:
  using BaseType   = FlagsConst<StorageType>;
  using AtomicType = typename BaseType::AtomicType;
  using ValueType  = typename BaseType::ValueType;
  /* -- Inherit constructors ----------------------------------------------- */
  using BaseType::BaseType;
  /* -- Assignment operator ------------------------------------------------ */
  constexpr Flags& operator=(ValueType vtValue)
  { // If is atomic type, set and return the value in the atomic or the integer
    if constexpr(StdIsSame<StorageType, AtomicType>)
      this->stoValue.store(vtValue, memory_order_relaxed);
    else this->stoValue = vtValue;
    return *this;
  }
  /* -- Compound Assignment Operators (+=, -=, &=, etc.) ------------------- */
#define COMOPER(o, f) \
  constexpr Flags& operator o(ValueType vtValue) \
  { /* If is atomic type, set and return the value in the atomic or the int */\
    if constexpr(StdIsSame<StorageType, AtomicType>) \
      this->stoValue.f(vtValue, memory_order_relaxed); \
    else this->stoValue o vtValue; \
    return *this; \
  }
  COMOPER(&=, fetch_and)
  COMOPER(|=, fetch_or)
  COMOPER(^=, fetch_xor)
#undef COMOPER
  /* -- Swap function ------------------------------------------------------ */
  constexpr void FlagSwap(Flags &flOther)
  { // Read both values first
    const ValueType vtTemp = static_cast<ValueType>(flOther);
    flOther = *this;
    *this = vtTemp;
  }
  /* -- Set bits ----------------------------------------------------------- */
  constexpr void FlagSet(const ValueType vtOther) { *this |= vtOther; }
  /* -- Set all bits ------------------------------------------------------- */
  constexpr void FlagReset(const ValueType vtOther = 0) { *this = vtOther; }
  /* -- Not specified bits ------------------------------------------------- */
  constexpr void FlagNot(const ValueType vtValue) { *this = ~vtValue; }
  /* -- Mask bits ---------------------------------------------------------- */
  constexpr void FlagMask(const ValueType vtValue) { *this &= vtValue; }
  /* -- Toggle specified bits ---------------------------------------------- */
  constexpr void FlagToggle(const ValueType vtValue) { *this ^= vtValue; }
  /* -- Clear specified bits ----------------------------------------------- */
  constexpr void FlagClear(const ValueType vtValue) { *this &= ~vtValue; }
  /* -- Add or clear bits -------------------------------------------------- */
  constexpr void FlagSetOrClear(const ValueType vtValue, const bool bCondition)
    { if(bCondition) FlagSet(vtValue); else FlagClear(vtValue); }
  /* -- Add and clear bits from specified enum ----------------------------- */
  constexpr void FlagSetAndClear(const ValueType vtSet,
    const ValueType vtClear) { FlagSet(vtSet); FlagClear(vtClear); }
  /* -- Clear bits and add from specified enum ----------------------------- */
  constexpr void FlagClearAndSet(const ValueType vtClear,
    const ValueType vtSet) { FlagClear(vtClear); FlagSet(vtSet); }
};/* ----------------------------------------------------------------------- */
template<typename IntType>
  requires StdIsIntegral<IntType>
struct Flag
{ /* -- Public typedefs ---------------------------------------------------- */
  using ValueType    = IntType;
  using UnsignedType = StdMakeUnsigned<IntType>;
  /* -- Number of bits in the specified IntTYpe ---------------------------- */
  static constexpr size_t stWidth = StdLimits<UnsignedType>::digits;
  /* -- Contains the integral value of each bit ---------------------------- */
  StdArray<IntType, stWidth + 1> aFlags{};
  /* -- Constructor that bills 'aFlags' ------------------------------------ */
  consteval Flag() noexcept
  { // No flags (zero)
    aFlags[0] = IntType{0};
    // Fill rest of flags
    for(size_t stBit = 1; stBit <= stWidth; ++stBit)
      { aFlags[stBit] = static_cast<IntType>(UnsignedType{1} << (stBit - 1)); }
  }
  /* -- Access the bit integer value with this array bounds access func ---- */
  [[nodiscard]]constexpr IntType operator[](const size_t stIndex)
    const noexcept
  { return stIndex < aFlags.size() ? aFlags[stIndex] : IntType{0}; }
};
/* -- Flags helper macro --------------------------------------------------- */
#define BUILD_FLAGS_EX(t, s, n, ...) \
  using n##FlagsType  = t; \
  using n##FlagsConst = FlagsConst<n##FlagsType>; \
  using n##Flags      = Flags<s>; \
  constexpr static Flag<t> n##Flag; \
  constexpr static n##FlagsConst __VA_ARGS__
/* -- Insecure flags helper macro ------------------------------------------ */
#define BUILD_FLAGS(t, n, ...) \
  BUILD_FLAGS_EX(t, n##FlagsType, n, __VA_ARGS__)
/* -- Secure (atomic) flags helper macro ----------------------------------- */
#define BUILD_SECURE_FLAGS(t, n, ...) \
  BUILD_FLAGS_EX(t, StdAtomic<n##FlagsType>, n, __VA_ARGS__)
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
