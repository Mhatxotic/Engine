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
/* == Storage for flags ==================================================== **
** ######################################################################### **
** ## Simple unprotected integer based flags.                             ## **
** ######################################################################### */
template<typename IntType>class FlagsStorageUnsafe
{ /* -- Values storage ------------------------------------------ */ protected:
  IntType          itV;                // The simple value
  /* -- Reset with specified value ----------------------------------------- */
  template<typename AnyType>void FlagSetInt(const AnyType atValue)
    { itV = static_cast<IntType>(atValue); }
  /* -- Swap values -------------------------------------------------------- */
  void FlagSwapStorage(FlagsStorageUnsafe &fcValue) { swap(itV, fcValue.itV); }
  /* -- Implicit init constructor (todo, make explicit but many errors!) --- */
  explicit FlagsStorageUnsafe(const IntType itValue) : itV{ itValue } {}
  /* -- Get values ------------------------------------------------- */ public:
  template<typename AnyType=IntType>AnyType FlagGet(void) const
    { return static_cast<AnyType>(itV); }
};/* ----------------------------------------------------------------------- */
/* == Atomic storage for flags ============================================= **
** ######################################################################### **
** ## Thread safe integer based flags.                                    ## **
** ######################################################################### */
template<typename IntType,
         typename SafeType = atomic<IntType>>
class FlagsStorageSafe :
  /* -- Base classes ------------------------------------------------------- */
  private SafeType
{ /* -- Set values ---------------------------------------------- */ protected:
  template<typename AnyType>void FlagSetInt(const AnyType atValue)
    { this->store(static_cast<IntType>(atValue)); }
  /* -- Swap values -------------------------------------------------------- */
  void FlagSwapStorage(FlagsStorageSafe &fcValue) { swap(fcValue); }
  /* -- Implicit init constructor (todo, make explicit but many errors!) --- */
  explicit FlagsStorageSafe(const IntType itValue) : SafeType{ itValue } {}
  /* -- Get values ------------------------------------------------- */ public:
  template<typename AnyType=IntType>AnyType FlagGet(void) const
    { return static_cast<AnyType>(this->load()); }
};/* ----------------------------------------------------------------------- */
/* == Read-only flags helper class ========================================= **
** ######################################################################### **
** ## If only read-only access is desired.                                ## **
** ######################################################################### */
template<typename IntType,
         class StorageType = FlagsStorageUnsafe<IntType>>
class FlagsConst :
  /* -- Base classes ------------------------------------------------------- */
  public StorageType
{ /* -- Is bits lesser than specified value? ----------------------- */ public:
  bool FlagIsLesser(const FlagsConst &fcValue) const
    { return this->FlagGet() < fcValue.FlagGet(); }
  /* -- Is bits lesser or equal to specified value? ------------------------ */
  bool FlagIsLesserEqual(const FlagsConst &fcValue) const
    { return this->FlagGet() <= fcValue.FlagGet(); }
  /* -- Is bits greater to specified value? -------------------------------- */
  bool FlagIsGreater(const FlagsConst &fcValue) const
    { return this->FlagGet() > fcValue.FlagGet(); }
  /* -- Is bits greater or equal to specified value? ----------------------- */
  bool FlagIsGreaterEqual(const FlagsConst &fcValue) const
    { return this->FlagGet() >= fcValue.FlagGet(); }
  /* -- Are there not any flags set? --------------------------------------- */
  bool FlagIsZero(void) const
    { return this->FlagGet() == static_cast<IntType>(0); }
  /* -- Are any flags actually set? ---------------------------------------- */
  bool FlagIsNonZero(void) const { return !FlagIsZero(); }
  /* -- Is flag set with specified value? ---------------------------------- */
  bool FlagIsSet(const FlagsConst &fcValue) const
    { return (this->FlagGet() & fcValue.FlagGet()) == fcValue.FlagGet(); }
  /* -- Is any flag set with specified value? ------------------------------ */
  bool FlagIsAnyOfSet(const FlagsConst &fcValue) const
    { return (this->FlagGet() & fcValue.FlagGet()) != 0; }
  /* -- Is bit clear of specified value? ----------------------------------- */
  bool FlagIsClear(const FlagsConst &fcValue) const
    { return (~this->FlagGet() & fcValue.FlagGet()) == fcValue.FlagGet(); }
  /* -- Is bit clear of specified value? ----------------------------------- */
  bool FlagIsAnyOfClear(const FlagsConst &fcValue) const
    { return (~this->FlagGet() & fcValue.FlagGet()) != 0; }
  /* -- Is flag set with specified value and clear with another? ----------- */
  bool FlagIsSetAndClear(const FlagsConst &fcSet,
    const FlagsConst &fcClear) const
      { return FlagIsSet(fcSet) && FlagIsClear(fcClear); }
  /* -- Flags are not masked in specified other flags? --------------------- */
  bool FlagIsNotInMask(const FlagsConst &fcValue) const
    { return this->FlagGet() & ~fcValue.FlagGet(); }
  /* -- Flags are masked in specified other flags? ------------------------- */
  bool FlagIsInMask(const FlagsConst &fcValue) const
    { return !FlagIsNotInMask(fcValue); }
  /* -- Is any of these flags set and cleared? ----------------------------- */
  bool FlagIsAnyOfSetAndClear(void) const { return false; }
  template<typename ...VarArgs>
    bool FlagIsAnyOfSetAndClear(const FlagsConst &fcSet,
      const FlagsConst &fcClear, const VarArgs &...vaVars) const
  { return FlagIsSetAndClear(fcSet, fcClear) ?
      true : FlagIsAnyOfSetAndClear(vaVars...); }
  /* -- Is bits set? ------------------------------------------------------- */
  bool FlagIsEqualToBool(const FlagsConst &fcValue, const bool bState) const
    { return FlagIsSet(fcValue) == bState; }
  /* -- Is bits not set? --------------------------------------------------- */
  bool FlagIsNotEqualToBool(const FlagsConst &fcValue, const bool bState)
    const { return FlagIsSet(fcValue) != bState; }
  /* -- Return one variable or another if set ------------------------------ */
  template<typename AnyType>
    const AnyType FlagIsSetTwo(const FlagsConst &fcValue,
      const AnyType atSet, const AnyType atClear) const
  { return FlagIsSet(fcValue) ? atSet : atClear; }
  /* -- Init constructors -------------------------------------------------- */
  template<typename AnyType>explicit FlagsConst(const AnyType atValue) :
    StorageType{ static_cast<IntType>(atValue) } { }
  /* -- Operators ---------------------------------------------------------- */
  const FlagsConst operator~(void) const
    { return FlagsConst{ ~this->template FlagGet<IntType>() }; }
  const FlagsConst operator|(const FlagsConst &fcRHS) const
    { return FlagsConst{ this->template FlagGet<IntType>() |
                         fcRHS.template FlagGet<IntType>() }; }
  const FlagsConst operator&(const FlagsConst &fcRHS) const
    { return FlagsConst{ this->template FlagGet<IntType>() &
                         fcRHS.template FlagGet<IntType>() }; }
  const FlagsConst operator^(const FlagsConst &fcRHS) const
    { return FlagsConst{ this->template FlagGet<IntType>() ^
                         fcRHS.template FlagGet<IntType>() }; }
  /* -- Direct access using class variable name which returns value -------- */
  operator IntType(void) const
    { return this->template FlagGet<IntType>(); }
};/* ----------------------------------------------------------------------- */
/* == Flags helper class =================================================== **
** ######################################################################### **
** ## Read-write acesss for specified type.                               ## **
** ######################################################################### */
template<typename IntType,
         class StorageType = FlagsStorageUnsafe<IntType>,
         class ConstType = FlagsConst<IntType, StorageType>>
struct Flags :
  /* -- Base classes ------------------------------------------------------- */
  public ConstType
{ /* -- Swap function ------------------------------------------------------ */
  void FlagSwap(Flags &fValue) { this->FlagSwapStorage(fValue); }
  /* -- Set bits ----------------------------------------------------------- */
  void FlagSet(const IntType &itOther)
    { this->FlagSetInt(this->FlagGet() | itOther); }
  void FlagSet(const ConstType &ctValue)
    { this->FlagSetInt(this->FlagGet() | ctValue.FlagGet()); }
  /* -- Set all bits ------------------------------------------------------- */
  void FlagReset(const IntType itOther) { this->FlagSetInt(itOther); }
  void FlagReset(const ConstType &ctValue)
    { this->FlagSetInt(ctValue.FlagGet()); }
  void FlagReset(void) { this->FlagSetInt(0); }
  /* -- Not specified bits ------------------------------------------------- */
  void FlagNot(const ConstType &ctValue)
    { this->FlagSetInt(this->FlagGet() & ~ctValue.FlagGet()); }
  /* -- Mask bits ---------------------------------------------------------- */
  void FlagMask(const ConstType &ctValue)
    { this->FlagSetInt(this->FlagGet() & ctValue.FlagGet()); }
  /* -- Toggle specified bits ---------------------------------------------- */
  void FlagToggle(const ConstType &ctValue)
    { this->FlagSetInt(this->FlagGet() ^ ctValue.FlagGet()); }
  /* -- Clear specified bits ----------------------------------------------- */
  void FlagClear(const ConstType &ctValue)
    { FlagMask(ConstType{ ~ctValue.template FlagGet<IntType>() }); }
  /* -- Add or clear bits -------------------------------------------------- */
  void FlagSetOrClear(const ConstType &ctValue, const bool bCondition)
    { if(bCondition) FlagSet(ctValue); else FlagClear(ctValue); }
  /* -- Add and clear bits from specified enum ----------------------------- */
  void FlagSetAndClear(const ConstType &ctSet, const ConstType &ctClear)
    { FlagSet(ctSet); FlagClear(ctClear); }
  void FlagClearAndSet(const ConstType &ctClear, const ConstType &ctSet)
    { FlagClear(ctClear); FlagSet(ctSet); }
  /* -- Init constructors -------------------------------------------------- */
  explicit Flags(const IntType &itOther) : ConstType{ itOther } {}
  explicit Flags(const ConstType &ctOther) : ConstType{ ctOther } {}
  /* -- Default constructor ------------------------------------------------ */
  Flags(void) : ConstType{ 0 } {}
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
class SafeFlags :
  /* -- Base classes ------------------------------------------------------- */
  public FlagsType
{ /* -- Implicit init constructor (todo, make explicit!) ----------- */ public:
  explicit SafeFlags(const ConstType &fcValue) : FlagsType{ fcValue } {}
  explicit SafeFlags(const UConstType &fcValue) : FlagsType{ fcValue } {}
};/* ----------------------------------------------------------------------- */
/* == Flags helper macro =================================================== */
#define BUILD_FLAGS_EX(n, s, ...) \
  typedef uint64_t n ## FlagsType; \
  typedef s<n ## FlagsType> n ## Flags; \
  typedef FlagsConst<n ## FlagsType> n ## FlagsConst; \
  static const n ## FlagsConst __VA_ARGS__;
#define BUILD_FLAGS(n, ...) BUILD_FLAGS_EX(n, Flags, __VA_ARGS__)
#define BUILD_SECURE_FLAGS(n, ...) BUILD_FLAGS_EX(n, SafeFlags, __VA_ARGS__)
/* -- Pre-defined flags ---------------------------------------------------- */
constexpr const array<const uint64_t, 65> Flag
{ /* ----------------------------------------------------------------------- */
  0x0000000000000000,0x0000000000000001,0x0000000000000002,0x0000000000000004,
  0x0000000000000008,0x0000000000000010,0x0000000000000020,0x0000000000000040,
  0x0000000000000080,0x0000000000000100,0x0000000000000200,0x0000000000000400,
  0x0000000000000800,0x0000000000001000,0x0000000000002000,0x0000000000004000,
  0x0000000000008000,0x0000000000010000,0x0000000000020000,0x0000000000040000,
  0x0000000000080000,0x0000000000100000,0x0000000000200000,0x0000000000400000,
  0x0000000000800000,0x0000000001000000,0x0000000002000000,0x0000000004000000,
  0x0000000008000000,0x0000000010000000,0x0000000020000000,0x0000000040000000,
  0x0000000080000000,0x0000000100000000,0x0000000200000000,0x0000000400000000,
  0x0000000800000000,0x0000001000000000,0x0000002000000000,0x0000004000000000,
  0x0000008000000000,0x0000010000000000,0x0000020000000000,0x0000040000000000,
  0x0000080000000000,0x0000100000000000,0x0000200000000000,0x0000400000000000,
  0x0000800000000000,0x0001000000000000,0x0002000000000000,0x0004000000000000,
  0x0008000000000000,0x0010000000000000,0x0020000000000000,0x0040000000000000,
  0x0080000000000000,0x0100000000000000,0x0200000000000000,0x0400000000000000,
  0x0800000000000000,0x1000000000000000,0x2000000000000000,0x4000000000000000,
  0x8000000000000000
};/* ----------------------------------------------------------------------- */
}                                      // End of module namespace
/* == EoF =========================================================== EoF == */
