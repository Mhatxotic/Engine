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
namespace IFlags {                     // Start of module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IStd::P;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* == Storage for flags ==================================================== **
** ######################################################################### **
** ## Simple unprotected integer based flags.                             ## **
** ######################################################################### */
template<typename IntType>
  requires StdIsIntegral<IntType> || StdIsAtomicV<IntType>
class FlagsStorageUnsafe
{ /* -- Values storage ------------------------------------------ */ protected:
  IntType          itV;                // The simple value
  /* -- Reset with specified value ----------------------------------------- */
  template<typename AnyType> requires StdIsIntegral<IntType>
    constexpr void FlagSetInt(const AnyType atValue)
  { itV = static_cast<IntType>(atValue); }
  /* -- Swap values -------------------------------------------------------- */
  constexpr void FlagSwapStorage(FlagsStorageUnsafe &fcuValue)
    { StdSwap(itV, fcuValue.itV); }
  /* -- Implicit init constructor (todo, make explicit but many errors!) --- */
  constexpr explicit FlagsStorageUnsafe(const IntType itValue) :
    /* -- Initialisers ----------------------------------------------------- */
    itV{ itValue }
    /* -- No code ---------------------------------------------------------- */
    {}
  /* -- Get values ------------------------------------------------- */ public:
  template<typename AnyType=IntType>requires StdIsIntegral<IntType>
    constexpr AnyType FlagGet() const { return static_cast<AnyType>(itV); }
  /* -- Type --------------------------------------------------------------- */
  using ValueType = IntType;
};/* ----------------------------------------------------------------------- */
/* == Atomic storage for flags ============================================= **
** ######################################################################### **
** ## Thread safe integer based flags.                                    ## **
** ######################################################################### */
template<typename IntType,
         typename SafeType = StdAtomic<IntType>>
requires StdIsIntegral<IntType> &&
         StdIsAtomicV<SafeType> &&
         StdIsIntegral<typename SafeType::value_type>
class FlagsStorageSafe :
  /* -- Base classes (note that StdAtomic will never throw) ---------------- */
  private SafeType
{ /* -- Set values ---------------------------------------------- */ protected:
  template<typename AnyType>constexpr void FlagSetInt(const AnyType atValue)
    { this->store(static_cast<IntType>(atValue)); }
  /* -- Swap values (This way round to stop CppCheck warning) -------------- */
  constexpr void FlagSwapStorage(FlagsStorageSafe &fssOther)
    { fssOther.StdSwap(*this); }
  /* -- Implicit init constructor (todo, make explicit but many errors!) --- */
  constexpr explicit FlagsStorageSafe(const IntType itValue) :
    SafeType{ itValue } {}
  /* -- Get values ------------------------------------------------- */ public:
  template<typename AnyType=IntType>constexpr AnyType FlagGet() const
    { return static_cast<AnyType>(this->load()); }
};/* ----------------------------------------------------------------------- */
/* == Read-only flags helper class ========================================= **
** ######################################################################### **
** ## If only read-only access is desired.                                ## **
** ######################################################################### */
template<typename IntType,
         class StorageType = FlagsStorageUnsafe<IntType>>
requires StdIsIntegral<IntType>
class FlagsConst :
  /* -- Base classes ------------------------------------------------------- */
  public StorageType
{ /* -- Is bits lesser than specified value? ----------------------- */ public:
  constexpr bool FlagIsLesser(const FlagsConst &fcValue) const
    { return this->FlagGet() < fcValue.FlagGet(); }
  /* -- Is bits lesser or equal to specified value? ------------------------ */
  constexpr bool FlagIsLesserEqual(const FlagsConst &fcValue) const
    { return this->FlagGet() <= fcValue.FlagGet(); }
  /* -- Is bits greater to specified value? -------------------------------- */
  constexpr bool FlagIsGreater(const FlagsConst &fcValue) const
    { return this->FlagGet() > fcValue.FlagGet(); }
  /* -- Is bits greater or equal to specified value? ----------------------- */
  constexpr bool FlagIsGreaterEqual(const FlagsConst &fcValue) const
    { return this->FlagGet() >= fcValue.FlagGet(); }
  /* -- Are there not any flags set? --------------------------------------- */
  constexpr bool FlagIsZero() const
    { return this->FlagGet() == static_cast<IntType>(0); }
  /* -- Are any flags actually set? ---------------------------------------- */
  constexpr bool FlagIsNonZero() const { return !FlagIsZero(); }
  /* -- Return result of an & operation ------------------------------------ */
  constexpr IntType FlagAnd(const FlagsConst &fcValue) const
    { return (this->FlagGet() & fcValue.FlagGet()); }
  /* -- Return result of an & operation with inverted source flags --------- */
  constexpr IntType FlagAndInverted(const FlagsConst &fcValue) const
    { return (~this->FlagGet() & fcValue.FlagGet()); }
  /* -- Is flag set with specified value? ---------------------------------- */
  constexpr bool FlagIsSet(const FlagsConst &fcValue) const
    { return FlagAnd(fcValue) == fcValue.FlagGet(); }
  /* -- Is any flag set with specified value? ------------------------------ */
  constexpr bool FlagIsAnyOfSet(const FlagsConst &fcValue) const
    { return FlagAnd(fcValue) != 0; }
  /* -- Is bit clear of specified value? ----------------------------------- */
  constexpr bool FlagIsClear(const FlagsConst &fcValue) const
    { return FlagAndInverted(fcValue) == fcValue.FlagGet(); }
  /* -- Is bit clear of specified value? ----------------------------------- */
  constexpr bool FlagIsAnyOfClear(const FlagsConst &fcValue) const
    { return FlagAndInverted(fcValue) != 0; }
  /* -- Is flag set with specified value and clear with another? ----------- */
  constexpr bool FlagIsSetAndClear(const FlagsConst &fcSet,
    const FlagsConst &fcClear) const
      { return FlagIsSet(fcSet) && FlagIsClear(fcClear); }
  /* -- Flags are not masked in specified other flags? --------------------- */
  constexpr bool FlagIsNotInMask(const FlagsConst &fcValue) const
    { return this->FlagGet() & ~fcValue.FlagGet(); }
  /* -- Flags are masked in specified other flags? ------------------------- */
  constexpr bool FlagIsInMask(const FlagsConst &fcValue) const
    { return !FlagIsNotInMask(fcValue); }
  /* -- Is bits set? ------------------------------------------------------- */
  constexpr bool FlagIsEqualToBool(const FlagsConst &fcValue,
    const bool bState) const
  { return FlagIsSet(fcValue) == bState; }
  /* -- Is bits not set? --------------------------------------------------- */
  constexpr bool FlagIsNotEqualToBool(const FlagsConst &fcValue,
    const bool bState) const
  { return FlagIsSet(fcValue) != bState; }
  /* -- Return one variable or another if set ------------------------------ */
  template<typename AnyType>
    constexpr const AnyType FlagIsSetTwo(const FlagsConst &fcValue,
      const AnyType atSet, const AnyType atClear) const
  { return FlagIsSet(fcValue) ? atSet : atClear; }
  /* -- Init constructors -------------------------------------------------- */
  template<typename AnyType>
    constexpr explicit FlagsConst(const AnyType atValue) :
      StorageType{ static_cast<IntType>(atValue) } {}
  /* -- Operators ---------------------------------------------------------- */
  constexpr const FlagsConst operator~() const
    { return FlagsConst{ ~this->template FlagGet<IntType>() }; }
  constexpr const FlagsConst operator|(const FlagsConst &fcRHS) const
    { return FlagsConst{ this->template FlagGet<IntType>() |
                         fcRHS.template FlagGet<IntType>() }; }
  constexpr const FlagsConst operator&(const FlagsConst &fcRHS) const
    { return FlagsConst{ this->template FlagGet<IntType>() &
                         fcRHS.template FlagGet<IntType>() }; }
  constexpr const FlagsConst operator^(const FlagsConst &fcRHS) const
    { return FlagsConst{ this->template FlagGet<IntType>() ^
                         fcRHS.template FlagGet<IntType>() }; }
  /* -- Direct access using class variable name which returns value -------- */
  constexpr operator IntType() const
    { return this->template FlagGet<IntType>(); }
};/* ----------------------------------------------------------------------- */
/* == Flags helper class =================================================== **
** ######################################################################### **
** ## Read-write acesss for specified type.                               ## **
** ######################################################################### */
template<typename IntType,
         class StorageType = FlagsStorageUnsafe<IntType>,
         class ConstType = FlagsConst<IntType, StorageType>>
requires StdIsIntegral<IntType>
struct Flags :
  /* -- Base classes ------------------------------------------------------- */
  public ConstType
{ /* -- Swap function ------------------------------------------------------ */
  constexpr void FlagSwap(Flags &fValue)
    { this->FlagSwapStorage(fValue); }
  /* -- Set bits ----------------------------------------------------------- */
  constexpr void FlagSet(const IntType &itOther)
    { this->FlagSetInt(this->FlagGet() | itOther); }
  constexpr void FlagSet(const ConstType &ctValue)
    { this->FlagSetInt(this->FlagGet() | ctValue.FlagGet()); }
  /* -- Set all bits ------------------------------------------------------- */
  constexpr void FlagReset(const IntType itOther)
    { this->FlagSetInt(itOther); }
  constexpr void FlagReset(const ConstType &ctValue)
    { this->FlagSetInt(ctValue.FlagGet()); }
  constexpr void FlagReset() { this->FlagSetInt(0); }
  /* -- Not specified bits ------------------------------------------------- */
  constexpr void FlagNot(const ConstType &ctValue)
    { this->FlagSetInt(this->FlagGet() & ~ctValue.FlagGet()); }
  /* -- Mask bits ---------------------------------------------------------- */
  constexpr void FlagMask(const ConstType &ctValue)
    { this->FlagSetInt(this->FlagGet() & ctValue.FlagGet()); }
  /* -- Toggle specified bits ---------------------------------------------- */
  constexpr void FlagToggle(const ConstType &ctValue)
    { this->FlagSetInt(this->FlagGet() ^ ctValue.FlagGet()); }
  /* -- Clear specified bits ----------------------------------------------- */
  constexpr void FlagClear(const ConstType &ctValue)
    { FlagMask(ConstType{ ~ctValue.template FlagGet<IntType>() }); }
  /* -- Add or clear bits -------------------------------------------------- */
  constexpr void FlagSetOrClear(const ConstType &ctValue,
    const bool bCondition)
      { if(bCondition) FlagSet(ctValue); else FlagClear(ctValue); }
  /* -- Add and clear bits from specified enum ----------------------------- */
  constexpr void FlagSetAndClear(const ConstType &ctSet,
    const ConstType &ctClear)
      { FlagSet(ctSet); FlagClear(ctClear); }
  /* -- Clear bits and add from specified enum ----------------------------- */
  constexpr void FlagClearAndSet(const ConstType &ctClear,
    const ConstType &ctSet)
      { FlagClear(ctClear); FlagSet(ctSet); }
  /* -- Init constructors -------------------------------------------------- */
  constexpr explicit Flags(const IntType &itOther) :
    ConstType{ itOther } {}
  constexpr explicit Flags(const ConstType &ctOther) :
   ConstType{ ctOther } {}
  /* -- Default constructor ------------------------------------------------ */
  constexpr Flags() : ConstType{ 0 } {}
};/* ----------------------------------------------------------------------- */
/* == Safe flags helper class ============================================== **
** ######################################################################### **
** ## Uses atomic storage for safe access.                                ## **
** ######################################################################### */
template<typename IntType,
         class StorageType = FlagsStorageSafe<IntType>,
         class UStorageType = FlagsStorageUnsafe<IntType>,
         class UConstType = FlagsConst<IntType, UStorageType>,
         class FlagsType = Flags<IntType, StorageType, UConstType>,
         class ConstType = FlagsConst<IntType, StorageType>>
requires StdIsIntegral<IntType>
class SafeFlags :
  /* -- Base classes ------------------------------------------------------- */
  public FlagsType
{ /* -- Implicit init constructor (todo, make explicit!) ----------- */ public:
  constexpr explicit SafeFlags(const ConstType &fcValue) :
    FlagsType{ fcValue } {}
  constexpr explicit SafeFlags(const UConstType &fcValue) :
    FlagsType{ fcValue } {}
};/* ----------------------------------------------------------------------- */
/* == Flags helper macro =================================================== */
#define BUILD_FLAGS_EX(n, s, ...) \
  using n ## FlagsType  = uint64_t; \
  using n ## Flags      = s<n ## FlagsType>; \
  using n ## FlagsConst = FlagsConst<n ## FlagsType>; \
  constexpr static const n ## FlagsConst __VA_ARGS__
#define BUILD_FLAGS(n, ...) BUILD_FLAGS_EX(n, Flags, __VA_ARGS__)
#define BUILD_SECURE_FLAGS(n, ...) BUILD_FLAGS_EX(n, SafeFlags, __VA_ARGS__)
/* -- Helper for defining flags -------------------------------------------- */
constexpr static uint64_t Flag(const size_t stIndex)
  { return stIndex ? 1ULL << (stIndex - 1) : 0; };
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of module namespace
/* == EoF =========================================================== EoF == */
