/* == STDPRE.HPP =========================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## Core standard wrappers.                                             ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* -- Wrapper for STL character functions (can't put in std.hpp) ----------- */
namespace IStd {                       // Start of module namespace
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* -- Returns if character is hexadecimal (0-9A-Fa-f) ---------------------- */
template<typename IntType>
  requires StdIsIntegral<IntType>
constexpr static bool StdIsXDigit(const IntType itChar)
  { return ::std::isxdigit(static_cast<int>(itChar)); }
/* -- Template to construct and initialise part of another object ---------- */
template<class ClassType, void(*Function)(ClassType &, const size_t)>
  struct StdArrayInit : public ClassType
{ // To reference by name itself
  constexpr operator ClassType&() { return *this; }
  constexpr operator const ClassType&() const { return *this; }
  // To use the class assignment operator
  using ClassType::operator=;
  // Using operator () to return the actual type
  constexpr ClassType& operator()() { return *this; }
  constexpr const ClassType& operator()() const { return *this; }
  // Constructor
  constexpr explicit StdArrayInit(const size_t stSize)
    { Function(*this, stSize); }
};
/* -- Template to construct and reserve part of another object ------------- */
template<class ClassType>
  constexpr void StdReserve(ClassType &c, const size_t s) { c.reserve(s); }
template<class ClassType>
  concept StdHasReserve =
    requires(ClassType &c, const size_t s) { c.reserve(s); };
template<class ClassType>
  requires StdHasReserve<ClassType>
using StdReserved = StdArrayInit<ClassType, &StdReserve<ClassType>>;
/* -- Template to construct and resize part of another object -------------- */
template<class ClassType>
  constexpr void StdResize(ClassType &c, const size_t s) { c.resize(s); }
template<class ClassType>
  concept StdHasResize =
    requires(ClassType &c, const size_t s) { c.resize(s); };
template<class ClassType>
  requires StdHasResize<ClassType>
using StdResized = StdArrayInit<ClassType, &StdResize<ClassType>>;
/* -- Forward &&arguments to another function or variable ------------------ */
template<typename AnyType>
  constexpr static auto &&StdForward(auto &&...aArgs)
{ return ::std::forward<AnyType>(aArgs...); }
/* -- Measures the distance between two iterators/pointers ----------------- */
template<typename SizeType = ssize_t, class...VarArgs>
  constexpr static SizeType StdDistance(auto &&...aArgs)
{ return static_cast<SizeType>(
    ::std::ranges::distance(StdForward<decltype(aArgs)>(aArgs)...)); }
/* -- Caluclate iterator forwards ------------------------------------------ */
template<class ItType, class IntType = ssize_t>
  constexpr static ItType StdNext(const ItType itIt,
    const IntType itAmount = 1)
{ return ::std::next(itIt, static_cast<ssize_t>(itAmount)); }
/* -- Calculate iterator backwards ----------------------------------------- */
template<class ItType, class IntType = ssize_t>
  constexpr static ItType StdPrev(const ItType itIt,
    const IntType itAmount = 1)
{ return ::std::prev(itIt, static_cast<ssize_t>(itAmount)); }
/* -- Remove const from a pointer ------------------------------------------ */
template<typename TypeTo,
         typename TypeFrom,
         typename TypeNonConst = StdRemoveConst<StdRemovePointer<TypeFrom>>*>
  requires StdIsPointer<TypeFrom>
constexpr static TypeTo StdToNonConstCast(TypeFrom tfV)
  { return reinterpret_cast<TypeTo>(const_cast<TypeNonConst>(tfV)); }
/* -- Concept to test for string types ------------------------------------- */
template<typename AnyType, typename DecayedType = StdDecay<AnyType>>
  concept StdIsString =
    StdIsSame<DecayedType, StdString> ||
    StdIsSame<DecayedType, StdStringView>;
template<typename AnyType, typename DecayedType = StdDecay<AnyType>>
  concept StdIsWideString =
    StdIsSame<DecayedType, StdWideString> ||
    StdIsSame<DecayedType, StdWideStringView>;
/* -- Concept to test for string or char pointer types --------------------- */
template<typename AnyType>
  concept StdIsStrOrCStr =
    StdIsString<AnyType> ||                  // Is StdString or StdStringView?
    StdIsPointer<StdDecay<AnyType>> ||       // Is a pointer type?
    StdIsArray<StdRemoveReference<AnyType>>; // Is a C-string array?
/* -- Converts type to StdStringView if type not StdString or StdStringView  */
template<typename StrType>
  using StdNormalisedString = StdConditional<
    StdIsSame<StdDecay<StrType>, StdString>,
    StrType&&, // Keep the original string reference category
    StdStringView>;
/* -- Simple helper function thin wrapper for above normaliser ------------- */
template<typename StrType>
  constexpr static auto StdNormaliseString(StrType &&strStr, auto &&aFunc)
{ using StdNormString = StdNormalisedString<StrType>;
  StdNormString snsStr{ StdForward<StrType>(strStr) };
  return aFunc(snsStr); }
/* -- Converts any string type to a pointer for C functions ---------------- */
template<typename StrType>
  constexpr static auto StrDenormaliseString(StrType &&strStr, auto &&aFunc)
{ // Get type without reference and a decayed type
  using StrTypeDecayed = StdDecay<StrType>;
  // If type is an C-style text array?
  if constexpr(StdIsArray<StdRemoveReference<StrType>> ||
               StdIsPointer<StrTypeDecayed>)
    return aFunc(strStr);
  // Correct type is a StdString?
  else if constexpr(StdIsString<StrTypeDecayed>) return aFunc(strStr.data());
  // Anything else is invalid
  else static_assert(sizeof(StrType) == 0, "Invalid string type!");
}
/* -- Wrapper for STL swap function ---------------------------------------- */
template<typename...VarArgs>
  constexpr static void StdSwap(VarArgs &&...aArgs)
{ return ::std::swap(StdForward<VarArgs>(aArgs)...); }
/* -- Since compiler warns about use of the STL move() function ------------ */
template<class AnyType, typename AnyTypeRR = StdRemoveReference<AnyType>>
  requires (StdIsReference<AnyType> || StdIsSame<AnyType, AnyTypeRR>)
constexpr static AnyTypeRR &&StdMove(AnyType &&atVar) noexcept
  { return static_cast<AnyTypeRR&&>(atVar); }
/* -- Some frequently used maximums ---------------------------------------- */
constexpr const unsigned StdMaxUInt = StdLimits<unsigned>::max();
constexpr const uint64_t StdMaxUInt64 = StdLimits<uint64_t>::max();
constexpr const size_t StdMaxSizeT = StdLimits<size_t>::max();
constexpr const size_t StdNPos = StdString::npos;
/* -- Set error number ----------------------------------------------------- */
static void StdSetError(const int iValue) { errno = iValue; }
/* -- Get error number ----------------------------------------------------- */
static int StdGetError() { return errno; }
/* -- Is error number equal to --------------------------------------------- */
static bool StdIsError(const int iValue) { return StdGetError() == iValue; }
/* -- Is error number not equal to ----------------------------------------- */
static bool StdIsNotError(const int iValue) { return !StdIsError(iValue); }
/* -- Is error number non-zero? -------------------------------------------- */
static bool StdIsErrorSet[[maybe_unused]]() { return StdIsNotError(0); }
/* -- Is error number out of disk space ------------------------------------ */
static bool StdIsNoDiskSpace() { return StdIsError(ENOSPC); }
/* ------------------------------------------------------------------------- */
template<class...VarArgs>
  constexpr static decltype(auto) StdAccumulate(auto &&...aArgs)
{ return ::std::accumulate(StdForward<decltype(aArgs)>(aArgs)...); }
/* ------------------------------------------------------------------------- */
template<class...VarArgs>
  constexpr static decltype(auto) StdAdvance(auto &&...aArgs)
{ return ::std::advance(StdForward<decltype(aArgs)>(aArgs)...); }
/* ------------------------------------------------------------------------- */
template<class...VarArgs>
  constexpr static decltype(auto) StdAnyOf(auto &&...aArgs)
{ return ::std::any_of(StdForward<decltype(aArgs)>(aArgs)...); }
/* ------------------------------------------------------------------------- */
template<class...VarArgs>
  constexpr static decltype(auto) StdBackInserter(auto &&...aArgs)
{ return ::std::back_inserter(StdForward<decltype(aArgs)>(aArgs)...); }
/* -- Uppercases the specified character ----------------------------------- */
template<typename IntType>
  requires StdIsIntegral<IntType>
static char StdToUpper(const IntType itC)
  { return static_cast<char>(toupper(static_cast<int>(itC))); }
/* -- Lowercases the specified character ----------------------------------- */
template<typename IntType>
  requires StdIsIntegral<IntType>
static char StdToLower(const IntType itC)
  { return static_cast<char>(tolower(static_cast<int>(itC))); }
/* -- Returns if character is a whitespace --------------------------------- */
template<typename IntType>
  requires StdIsIntegral<IntType>
constexpr static bool StdIsSpace(const IntType itChar)
  { return itChar == ' '; }
/* -- Returns if character is NOT a whitespace ----------------------------- */
template<typename IntType>
  requires StdIsIntegral<IntType>
constexpr static bool StdIsNotSpace(const IntType itChar)
  { return !StdIsSpace(itChar); }
/* -- Returns if character is a digit (0-9) -------------------------------- */
template<typename IntType>
  requires StdIsIntegral<IntType>
constexpr static bool StdIsDigit(const IntType itChar)
  { return itChar >= '0' && itChar <= '9'; }
/* -- Returns if character is NOT a digit (0-9) ---------------------------- */
template<typename IntType>
  requires StdIsIntegral<IntType>
constexpr static bool StdIsNotDigit(const IntType itChar)
  { return !StdIsDigit(itChar); }
/* -- Returns if character is alphanumeric (A-Za-z) ------------------------ */
template<typename IntType>
  requires StdIsIntegral<IntType>
constexpr static bool StdIsAlpha(const IntType itChar)
  { return (itChar >= 'A' && itChar <= 'Z') ||
           (itChar >= 'a' && itChar <= 'z'); }
/* -- Returns if character is NOT alphanumeric (A-Za-z) -------------------- */
template<typename IntType>
  requires StdIsIntegral<IntType>
constexpr static bool StdIsNotAlpha(const IntType itChar)
  { return !StdIsAlpha(itChar); }
/* -- Returns if character is alphanumeric or numeric (0-9A-Za-z) ---------- */
template<typename IntType>
  requires StdIsIntegral<IntType>
constexpr static bool StdIsAlnum(const IntType itChar)
  { return StdIsAlpha(itChar) || StdIsDigit(itChar); }
/* -- Returns if character is NOT alphanumeric or numeric (0-9A-Za-z) ------ */
template<typename IntType>
  requires StdIsIntegral<IntType>
constexpr static bool StdIsNotAlnum(const IntType itChar)
  { return !StdIsAlnum(itChar); }
/* -- Return absolute number ----------------------------------------------- */
template<typename IntType = int64_t>
  requires StdIsIntegral<IntType>
constexpr static IntType StdAbsolute(const IntType itVal)
{ // Check if supplied argument is signed and negate it if true
  if constexpr(StdIsSigned<IntType>) return (itVal < 0) ? -itVal : itVal;
  // Else just return the value without any processing
  else return itVal;
}
/* -- Returns if the specified number is a power of two -------------------- */
template<typename IntType = int64_t>
  requires StdIsIntegral<IntType> || StdIsEnum<IntType>
constexpr static bool StdIntIsPOW2(const IntType itVal)
  { return !((itVal & (itVal - 1)) && itVal); }
/* -- Get the distance between two opposing corners ------------------------ */
template<typename IntType>
  requires StdIsIntegral<IntType>
constexpr static double StdHypot(const IntType itWidth, const IntType itHeight)
  { return ::std::hypot(itWidth, itHeight); }
/* -- Allocate memory ------------------------------------------------------ */
template<typename AnyType, typename IntType>
  requires StdIsPointer<AnyType*> && StdIsIntegral<IntType>
static AnyType *StdAlloc(const IntType itBytes)
  { return reinterpret_cast<AnyType*>
      (::std::malloc(static_cast<size_t>(itBytes))); }
/* -- Re-allocate memory ('inline' prevents -Wallocator-wrappers) ---------- */
template<typename AnyType, typename IntType>
  requires StdIsPointer<AnyType*> && StdIsIntegral<IntType>
static AnyType *StdReAlloc(AnyType*const atPtr, const IntType itBytes)
  { return reinterpret_cast<AnyType*>
      (::std::realloc(reinterpret_cast<void*>(atPtr),
         static_cast<size_t>(itBytes))); }
/* -- Release allocated memory --------------------------------------------- */
template<typename PtrType>
  requires (!StdIsPointer<PtrType>)
static void StdFree(PtrType*const ptPtr)
  { ::std::free(reinterpret_cast<void*>(ptPtr)); }
/* -- Compare memory ------------------------------------------------------- */
static int StdCompare(const void*const vpA, const void*const vpB,
  const size_t stSize)
{ return ::std::memcmp(vpA, vpB, stSize); }
/* -- Copy memory ---------------------------------------------------------- */
template<typename PtrTypeOut = void*,
         typename PtrTypeDest = void*const,
         typename PtrTypeSrc = void*const>
  requires StdIsPointer<PtrTypeOut> &&
           StdIsPointer<PtrTypeDest> &&
           StdIsPointer<PtrTypeSrc>
static PtrTypeOut StdMemCopy(PtrTypeDest ptdDest, const PtrTypeSrc ptsSrc,
  const size_t stSize)
{ return StdToNonConstCast<PtrTypeOut>(
    ::std::memcpy(ptdDest, ptsSrc, stSize)); }
/* -- Move memory (use when memory regions overlap) ------------------------ */
template<typename PtrTypeOut = void*,
         typename PtrTypeDest = void*const,
         typename PtrTypeSrc = void*const>
  requires StdIsPointer<PtrTypeOut> &&
           StdIsPointer<PtrTypeDest> &&
           StdIsPointer<PtrTypeSrc>
static PtrTypeOut StdMemMove(PtrTypeDest ptdDest, const PtrTypeSrc ptsSrc,
  const size_t stSize)
{ return StdToNonConstCast<PtrTypeOut>(
    ::std::memmove(ptdDest, ptsSrc, stSize)); }
/* -- Brute cast one type to another --------------------------------------- */
template<typename DestType, typename SrcType>
  requires (sizeof(DestType) == sizeof(SrcType)) &&
    StdIsTrCopyable<SrcType> && StdIsTrCopyable<DestType>
static DestType StdBruteCast(const SrcType stV)
  { return ::std::bit_cast<DestType>(stV); }
/* -- Brute cast one type to another if pointer is valid ------------------- */
template<typename FuncTypeDest, typename FuncTypeSrc>
  requires StdIsPointer<FuncTypeDest> && StdIsPointer<FuncTypeSrc> &&
    (sizeof(FuncTypeSrc) == sizeof(FuncTypeDest))
static FuncTypeDest StdBruteCastSafe[[maybe_unused]](FuncTypeSrc ftsSrc)
  { return ftsSrc ? StdBruteCast<FuncTypeDest,FuncTypeSrc>(ftsSrc) : nullptr; }
/* -- Copy a function (use instead of assigning to prevent warning) -------- */
template<typename FuncTypeDest, typename FuncTypeSrc>
  requires StdIsPointer<FuncTypeDest> && StdIsPointer<FuncTypeSrc> &&
    (sizeof(FuncTypeSrc) == sizeof(FuncTypeDest))
static FuncTypeDest StdCopyFunction(FuncTypeDest ftdDest, FuncTypeSrc ftsSrc)
  { if(!ftsSrc) return nullptr;
    StdMemCopy(ftdDest, &ftsSrc, sizeof(FuncTypeSrc));
    return ftdDest; }
/* -- Returns number of threads supported by CPU --------------------------- */
static unsigned StdThreadMax()
  { return ::std::thread::hardware_concurrency(); }
/* -- Returns current thread id -------------------------------------------- */
static auto StdThreadId() { return ::std::this_thread::get_id(); }
/* -- Returns current thread id -------------------------------------------- */
constexpr static void StdSuspend(const auto &aTime)
  { ::std::this_thread::sleep_for(aTime); }
/* ------------------------------------------------------------------------- */
constexpr static void StdSuspend()
  { StdSuspend(::std::chrono::milliseconds{ 1 }); }
/* -- Returns true if two numbers are equal (Omit != and == warnings) ------ */
template<typename FloatType>
  requires StdIsFloat<FloatType>
static bool StdIsFloatEqual(const FloatType ft1, const FloatType ft2,
  const FloatType ftEpsilon = static_cast<FloatType>(1e-6))
{ return ::std::fabs(ft1 - ft2) < ftEpsilon; }
/* -- Wrapper for non-execution policy version of StdTransform ------------- */
template<class InIt, class OutIt, class UnaryOp>
  static OutIt StdTransformNXP(InIt iiFirst, InIt iiLast, OutIt oiFirst,
    UnaryOp uoOp)
{ return ::std::transform(iiFirst, iiLast, oiFirst, StdMove(uoOp)); }
/* ------------------------------------------------------------------------- */
template<typename FloatType>
  requires StdIsFloat<FloatType>
static bool StdIsFloatNotEqual(const FloatType ft1, const FloatType ft2,
  const FloatType ftEpsilon = static_cast<FloatType>(1e-6f))
{ return !StdIsFloatEqual<FloatType>(ft1, ft2, ftEpsilon); }
/* -- Concept to test for .data and .resize methods ------------------------ */
template<class AnyType> concept StdHasDataSize =
  requires(AnyType &t)
{ t.data(); t.size(); };
/* -- Proxy to get address of a iterator ----------------------------------- */
constexpr static decltype(auto) StdAddressOf(auto &&aPtr)
  { return ::std::addressof(StdForward<decltype(aPtr)>(aPtr)); }
/* ------------------------------------------------------------------------- **
** ######################################################################### **
** ## Because MacOS doesn't support execution policy yet, we need to make ## **
** ## functions that ignore the first parameter while allowing the        ## **
** ## same parameter to pass through on other targets.                    ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
#if defined(MACOS)                     // Using MacOS?
/* ------------------------------------------------------------------------- */
constexpr static bool                  // MacOS doesn't have these policies
  par_unseq = false,                   // Parallel and vectorised disabled
  par       = false,                   // Parallel only disabled
  seq       = false;                   // Serialised disabled
/* ------------------------------------------------------------------------- */
constexpr static decltype(auto) StdSort(auto&, auto &&...aArgs)
  { return ::std::sort(StdForward<decltype(aArgs)>(aArgs)...); }
/* ------------------------------------------------------------------------- */
constexpr static decltype(auto) StdFill(auto&, auto &&...aArgs)
  { return ::std::fill(StdForward<decltype(aArgs)>(aArgs)...); }
/* ------------------------------------------------------------------------- */
constexpr static decltype(auto) StdRotate(auto&, auto &&...aArgs)
  { return ::std::rotate(StdForward<decltype(aArgs)>(aArgs)...); }
/* ------------------------------------------------------------------------- */
constexpr static decltype(auto) StdTransform(auto&, auto &&...aArgs)
  { return ::std::transform(StdForward<decltype(aArgs)>(aArgs)...); }
/* ------------------------------------------------------------------------- */
constexpr static decltype(auto) StdForEach(auto&, auto &&...aArgs)
  { return ::std::for_each(StdForward<decltype(aArgs)>(aArgs)...); }
/* ------------------------------------------------------------------------- */
constexpr static decltype(auto) StdFind(auto&, auto &&...aArgs)
  { return ::std::find(StdForward<decltype(aArgs)>(aArgs)...); }
/* ------------------------------------------------------------------------- */
constexpr static decltype(auto) StdSearch(auto&, auto &&...aArgs)
  { return ::std::search(StdForward<decltype(aArgs)>(aArgs)...); }
/* ------------------------------------------------------------------------- */
constexpr static decltype(auto) StdFindIf(auto&, auto &&...aArgs)
  { return ::std::find_if(StdForward<decltype(aArgs)>(aArgs)...); }
/* ------------------------------------------------------------------------- */
constexpr static decltype(auto) StdCopy(auto&, auto &&...aArgs)
  { return ::std::copy(StdForward<decltype(aArgs)>(aArgs)...); }
/* ------------------------------------------------------------------------- */
constexpr static decltype(auto) StdAllOf(auto&, auto &&...aArgs)
  { return ::std::all_of(StdForward<decltype(aArgs)>(aArgs)...); }
/* ------------------------------------------------------------------------- */
#else                                  // Windows or Posix target?
/* ------------------------------------------------------------------------- */
using ::std::execution::par_unseq;     // Parallel and vectorised
using ::std::execution::par;           // Parallel only
using ::std::execution::seq;           // Serialised
/* ------------------------------------------------------------------------- */
constexpr static decltype(auto) StdSort(auto &&...aArgs)
  { return ::std::sort(StdForward<decltype(aArgs)>(aArgs)...); }
/* ------------------------------------------------------------------------- */
constexpr static decltype(auto) StdFill(auto &&...aArgs)
  { return ::std::fill(StdForward<decltype(aArgs)>(aArgs)...); }
/* ------------------------------------------------------------------------- */
constexpr static decltype(auto) StdRotate(auto &&...aArgs)
  { return ::std::rotate(StdForward<decltype(aArgs)>(aArgs)...); }
/* ------------------------------------------------------------------------- */
constexpr static decltype(auto) StdTransform(auto &&...aArgs)
  { return ::std::transform(StdForward<decltype(aArgs)>(aArgs)...); }
/* ------------------------------------------------------------------------- */
constexpr static decltype(auto) StdForEach(auto &&...aArgs)
  { return ::std::for_each(StdForward<decltype(aArgs)>(aArgs)...); }
/* ------------------------------------------------------------------------- */
constexpr static decltype(auto) StdFind(auto &&...aArgs)
  { return ::std::find(StdForward<decltype(aArgs)>(aArgs)...); }
/* ------------------------------------------------------------------------- */
constexpr static decltype(auto) StdSearch(auto &&...aArgs)
  { return ::std::search(StdForward<decltype(aArgs)>(aArgs)...); }
/* ------------------------------------------------------------------------- */
constexpr static decltype(auto) StdFindIf(auto &&...aArgs)
  { return ::std::find_if(StdForward<decltype(aArgs)>(aArgs)...); }
/* ------------------------------------------------------------------------- */
constexpr static decltype(auto) StdCopy(auto &&...aArgs)
  { return ::std::copy(StdForward<decltype(aArgs)>(aArgs)...); }
/* ------------------------------------------------------------------------- */
constexpr static decltype(auto) StdAllOf(auto &&...aArgs)
  { return ::std::all_of(StdForward<decltype(aArgs)>(aArgs)...); }
/* ------------------------------------------------------------------------- */
#endif                                 // MacOS check
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of module namespace
/* == EoF =========================================================== EoF == */
