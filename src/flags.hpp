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
namespace P {                          // Start of public module namespace
/* -- Apply the concept directly to the template --------------------------- */
template<typename StorageType> class Flags
{ /* -- Private typdefs ---------------------------------------------------- */
  template <typename T>struct ExValueType { using type = T; };
  template <typename T>struct ExValueType<StdAtomic<T>> { using type = T; };
  /* -- Private variables -------------------------------------------------- */
  StorageType stoValue;
  /* -- Public typedefs -------------------------------------------- */ public:
  using AtomicType = StdAtomic<uint64_t>;
  using ValueType = typename ExValueType<StorageType>::type;
  /* -- Constructor -------------------------------------------------------- */
  constexpr explicit Flags(StorageType &stoV) : stoValue(stoV) {}
  constexpr explicit Flags(uint64_t ullValue = 0)
    requires (!std::same_as<StorageType, AtomicType>) :
      stoValue(ullValue) {}
  constexpr explicit Flags(uint64_t ullValue)
    requires (std::same_as<StorageType, AtomicType>)
  { stoValue.store(ullValue, std::memory_order_relaxed); }
  /* -- Handles ==, !=, <, >, <=, >= --------------------------------------- */
  constexpr auto operator<=>(const Flags &flOther) const
    { return static_cast<uint64_t>(*this) <=> static_cast<uint64_t>(flOther); }
  constexpr auto operator<=>(uint64_t ullOther) const
    { return static_cast<uint64_t>(*this) <=> ullOther; }
  /* -- Implicit conversion to plain uint64_t ------------------------------ */
  constexpr operator uint64_t() const
  {
    if constexpr(std::same_as<StorageType, AtomicType>)
      return stoValue.load(std::memory_order_relaxed);
    else return stoValue;
  }
  /* -- Assignment operator ------------------------------------------------ */
  constexpr Flags& operator=(uint64_t ullValue)
  {
    if constexpr(std::same_as<StorageType, AtomicType>)
      stoValue.store(ullValue, std::memory_order_relaxed);
    else stoValue = ullValue;
    return *this;
  }
  /* -- Compound Assignment Operators (+=, -=, &=, etc.) ------------------- */
  #define COMOPER(o, f) \
    constexpr Flags& operator o(uint64_t ullValue) \
    { if constexpr (std::same_as<StorageType, AtomicType>) \
        stoValue.f(ullValue, std::memory_order_relaxed); \
      else stoValue o ullValue; \
      return *this; }
  COMOPER(+=, fetch_add)
  COMOPER(-=, fetch_sub)
  COMOPER(&=, fetch_and)
  COMOPER(|=, fetch_or)
  COMOPER(^=, fetch_xor)
  #undef COMOPER
  /* -- Unary Operators ---------------------------------------------------- */
  constexpr uint64_t operator~() const { return ~static_cast<uint64_t>(*this); }
  constexpr uint64_t operator-() const { return -static_cast<uint64_t>(*this); }
  constexpr uint64_t operator+() const { return +static_cast<uint64_t>(*this); }
  /* -- Prefix Increment/Decrement ----------------------------------------- */
  constexpr Flags &operator++() { *this += 1; return *this; }
  constexpr Flags &operator--() { *this -= 1; return *this; }
  /* -- Get a variable and return it as the specified type ----------------- */
  template<typename AnyType = uint64_t>requires StdIsIntegral<AnyType>
    constexpr AnyType FlagGet() const { return static_cast<AnyType>(*this); }
  /* -- Is bits lesser than specified ullValue? ----------------------------- */
  constexpr bool FlagIsLesser(const uint64_t ullValue) const
    { return *this < ullValue; }
  /* -- Is bits lesser or equal to specified ullValue? ---------------------- */
  constexpr bool FlagIsLesserEqual(const uint64_t ullValue) const
    { return *this <= ullValue; }
  /* -- Is bits greater to specified ullValue? -------------------------------- */
  constexpr bool FlagIsGreater(const uint64_t ullValue) const
    { return *this > ullValue; }
  /* -- Is bits greater or equal to specified ullValue? ----------------------- */
  constexpr bool FlagIsGreaterEqual(const uint64_t ullValue) const
    { return *this >= ullValue; }
  /* -- Are there not any flags set? --------------------------------------- */
  constexpr bool FlagIsZero() const
    { return *this == static_cast<uint64_t>(0); }
  /* -- Are any flags actually set? ---------------------------------------- */
  constexpr bool FlagIsNonZero() const { return !FlagIsZero(); }
  /* -- Return result of an & operation ------------------------------------ */
  constexpr uint64_t FlagAnd(const uint64_t ullValue) const
    { return (*this & ullValue); }
  /* -- Return result of an & operation with inverted source flags --------- */
  constexpr uint64_t FlagAndInverted(const uint64_t ullValue) const
    { return (~*this & ullValue); }
  /* -- Is flag set with specified ullValue? ---------------------------------- */
  constexpr bool FlagIsSet(const uint64_t ullValue) const
    { return FlagAnd(ullValue) == ullValue; }
  /* -- Is any flag set with specified ullValue? ------------------------------ */
  constexpr bool FlagIsAnyOfSet(const uint64_t ullValue) const
    { return FlagAnd(ullValue) != 0; }
  /* -- Is bit clear of specified ullValue? ----------------------------------- */
  constexpr bool FlagIsClear(const uint64_t ullValue) const
    { return FlagAndInverted(ullValue) == ullValue; }
  /* -- Is bit clear of specified ullValue? ----------------------------------- */
  constexpr bool FlagIsAnyOfClear(const uint64_t ullValue) const
    { return FlagAndInverted(ullValue) != 0; }
  /* -- Is flag set with specified ullValue and clear with another? ----------- */
  constexpr bool FlagIsSetAndClear(const uint64_t ullSet,
    const uint64_t ullClear) const
  { return FlagIsSet(ullSet) && FlagIsClear(ullClear); }
  /* -- Flags are not masked in specified flOther flags? --------------------- */
  constexpr bool FlagIsNotInMask(const uint64_t ullValue) const
    { return *this & ~ullValue; }
  /* -- Flags are masked in specified flOther flags? ------------------------- */
  constexpr bool FlagIsInMask(const uint64_t ullValue) const
    { return !FlagIsNotInMask(ullValue); }
  /* -- Is bits set? ------------------------------------------------------- */
  constexpr bool FlagIsEqualToBool(const uint64_t ullValue,
    const bool bState) const
  { return FlagIsSet(ullValue) == bState; }
  /* -- Is bits not set? --------------------------------------------------- */
  constexpr bool FlagIsNotEqualToBool(const uint64_t ullValue,
    const bool bState) const
  { return FlagIsSet(ullValue) != bState; }
  /* -- Return one variable or another if set ------------------------------ */
  template<typename AnyType>
    constexpr const AnyType FlagIsSetTwo(const uint64_t ullValue,
      const AnyType atSet, const AnyType atClear) const
  { return FlagIsSet(ullValue) ? atSet : atClear; }
  /* -- Swap function ------------------------------------------------------ */
  constexpr void FlagSwap(Flags &flOther)
  { // Read both values first
    uint64_t ullTemp = static_cast<uint64_t>(flOther);
    flOther = *this;
    *this = ullTemp;
  }
  /* -- Set bits ----------------------------------------------------------- */
  constexpr void FlagSet(const uint64_t ullOther) { *this |= ullOther; }
  /* -- Set all bits ------------------------------------------------------- */
  constexpr void FlagReset(const uint64_t ullOther = 0) { *this = ullOther; }
  /* -- Not specified bits ------------------------------------------------- */
  constexpr void FlagNot(const uint64_t ullValue) { *this = ~ullValue; }
  /* -- Mask bits ---------------------------------------------------------- */
  constexpr void FlagMask(const uint64_t ullValue) { *this &= ullValue; }
  /* -- Toggle specified bits ---------------------------------------------- */
  constexpr void FlagToggle(const uint64_t ullValue) { *this ^= ullValue; }
  /* -- Clear specified bits ----------------------------------------------- */
  constexpr void FlagClear(const uint64_t ullValue) { *this &= ~ullValue; }
  /* -- Add or clear bits -------------------------------------------------- */
  constexpr void FlagSetOrClear(const uint64_t ullValue,
    const bool bCondition)
  { if(bCondition) FlagSet(ullValue); else FlagClear(ullValue); }
  /* -- Add and clear bits from specified enum ----------------------------- */
  constexpr void FlagSetAndClear(const uint64_t ullSet, const uint64_t ullClear)
    { FlagSet(ullSet); FlagClear(ullClear); }
  /* -- Clear bits and add from specified enum ----------------------------- */
  constexpr void FlagClearAndSet(const uint64_t ullClear, const uint64_t ullSet)
    { FlagClear(ullClear); FlagSet(ullSet); }
};/* ----------------------------------------------------------------------- */
/* -- Flags helper macro --------------------------------------------------- */
#define BUILD_FLAGS(n, ...) \
  using n##FlagsType  = uint64_t; \
  using n##FlagsConst = Flags<n##FlagsType>; \
  using n##Flags      = Flags<n##FlagsType>; \
  constexpr static const n##FlagsConst __VA_ARGS__
/* -- Secure (atomic) flags helper macro ----------------------------------- */
#define BUILD_SECURE_FLAGS(n, ...) \
  using n##FlagsType  = uint64_t; \
  using n##FlagsConst = Flags<n##FlagsType>; \
  using n##Flags      = Flags<StdAtomic<n##FlagsType>>; \
  constexpr static const n##FlagsConst __VA_ARGS__
/* -- Helper for defining flags -------------------------------------------- */
constexpr static uint64_t Flag(const size_t stIndex)
  { return stIndex ? 1ULL << (stIndex - 1) : 0; };
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */




