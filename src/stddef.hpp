/* == STDDEF.HPP =========================================================== **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## This module sets up all the common types we use from the STL. All   ## **
** ## these types sit in the Engine namespace so only the engine has      ## **
** ## access to them and not any other included external API's. We also   ## **
** ## alias them to avoid having to make multiple changes if and when the ## **
** ## STL ever changes which it actually does sometimes. Also we also     ## **
** ## like all our type names and function names with capitalised first   ## **
** ## letters of each syllable. An additional benefit to wrapping is that ## **
** ## it makes it easier to mass replace names if we need to because it   ## **
** ## really would be dangerous to mass replace the word 'string' so we   ## **
** ## wrapped it in 'StdString' instead.                                  ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* -- Common storage type aliases ------------------------------------------ */
template<typename AnyType, size_t stSize>
  using StdArray = ::std::array<AnyType, stSize>;
template<typename AnyType>
  using StdAtomic = ::std::atomic<AnyType>;
template<typename AnyType>
  using StdDeque = ::std::deque<AnyType>;
template<typename AnyType>
  using StdList = ::std::list<AnyType>;
template<typename KeyType, typename ValueType>
  using StdMap = ::std::map<KeyType, ValueType, ::std::less<>>;
template<typename AnyTypeA, typename AnyTypeB>
  using StdPair = ::std::pair<AnyTypeA, AnyTypeB>;
template<typename AnyType>
  using StdSet = ::std::set<AnyType>;
template<typename AnyType, typename FuncType = ::std::default_delete<AnyType>>
  using StdUniquePtr = ::std::unique_ptr<AnyType, FuncType>;
template<typename AnyType>
  using StdUnorderedSet = ::std::unordered_set<AnyType>;
template<typename AnyType>
  using StdVector = ::std::vector<AnyType>;
template<typename AnyType>
  using StdSpan = ::std::span<AnyType>;
template<typename AnyType>
  using StdQueue = ::std::queue<AnyType>;
template<size_t stIndex, class TupleType>
  using StdTupleElement = ::std::tuple_element_t<stIndex, TupleType>;
template<bool bCondition, class TrueType, class FalseType>
  using StdConditional = ::std::conditional_t<bCondition, TrueType, FalseType>;
/* -- Type manipulation aliases -------------------------------------------- */
template<typename AnyType>
  using StdRemoveConst = ::std::remove_const_t<AnyType>;
template<typename AnyType>
  using StdRemovePointer = ::std::remove_pointer_t<AnyType>;
template<typename AnyType>
  using StdRemoveReference = ::std::remove_reference_t<AnyType>;
template<typename AnyType>
  using StdMakeUnsigned = ::std::make_unsigned_t<AnyType>;
template<typename AnyType>
  using StdMakeSigned = ::std::make_signed_t<AnyType>;
template<typename AnyType>
  using StdUnderlyingType = ::std::underlying_type_t<AnyType>;
template<typename AnyType>
  using StdDecay = ::std::decay_t<AnyType>;
template<typename AnyType>
  using StdRefWrapper = ::std::reference_wrapper<AnyType>;
template<class AnyType>
  using StdRemoveConstVolRef = ::std::remove_cvref_t<AnyType>;
/* -- Template type checks ------------------------------------------------- */
template<typename AnyType>
  constexpr static bool StdIsArithmatic = ::std::is_arithmetic_v<AnyType>;
template<typename AnyType>
  constexpr static bool StdIsClass = ::std::is_class_v<AnyType>;
template<typename AnyType>
  constexpr static bool StdIsEnum = ::std::is_enum_v<AnyType>;
template<typename AnyType>
  constexpr static bool StdIsFloat = ::std::is_floating_point_v<AnyType>;
template<typename AnyType>
  constexpr static bool StdIsIntegral = ::std::is_integral_v<AnyType>;
template<typename AnyType>
  constexpr static bool StdIsNull = ::std::is_null_pointer_v<AnyType>;
template<typename AnyType>
  constexpr static bool StdIsPointer = ::std::is_pointer_v<AnyType>;
template<typename AnyType>
  constexpr static bool StdIsReference = ::std::is_reference_v<AnyType>;
template<typename AnyTypeA, typename AnyTypeB>
  constexpr static bool StdIsSame = ::std::is_same_v<AnyTypeA, AnyTypeB>;
template<typename AnyType>
  constexpr static bool StdIsSigned = ::std::is_signed_v<AnyType>;
template<typename AnyType>
  constexpr static bool StdIsUnsigned = ::std::is_unsigned_v<AnyType>;
template<typename AnyType>
  constexpr static bool StdIsArray = ::std::is_array_v<AnyType>;
template<typename AnyType>
  constexpr static size_t StdExtent = ::std::extent_v<AnyType>;
template<typename AnyType>
  constexpr static size_t StdTupleSize = ::std::tuple_size_v<AnyType>;
template<class AnyType>
  constexpr static bool StdIsTrCopyable =
    ::std::is_trivially_copyable_v<AnyType>;
/* ------------------------------------------------------------------------- */
template<typename AnyType>
  struct StdIsAtomic : ::std::false_type {};
template<typename AnyType>
  struct StdIsAtomic<StdAtomic<AnyType>> : ::std::true_type {};
template<typename AnyType>
  constexpr static bool StdIsAtomicV =
    StdIsAtomic<StdRemoveConst<AnyType>>::value;
/* ------------------------------------------------------------------------- */
template<typename AnyType>
  using StdLimits = ::std::numeric_limits<AnyType>;
/* ------------------------------------------------------------------------- */
constexpr static auto &StdNoThrow = ::std::nothrow;
/* -- General aliases ------------------------------------------------------ */
using StdIStream        = ::std::istream;
using StdIStringStream  = ::std::istringstream;
using StdLocale         = ::std::locale;
using StdOStream        = ::std::ostream;
using StdOStringStream  = ::std::ostringstream;
using StdStringBuf      = ::std::stringbuf;
using StdString         = ::std::string;
using StdStringView     = ::std::string_view;
using StdWideString     = ::std::wstring;
using StdWideStringView = ::std::wstring_view;
using StdWOStream       = ::std::wostream;
using StdNullPtr        = ::std::nullptr_t;
/* -- Exception aliases ---------------------------------------------------- */
using StdException    = ::std::exception;
using StdRunTimeError = ::std::runtime_error;
/* -- IOStream aliases ----------------------------------------------------- */
constexpr static auto &StdWcErr = ::std::wcerr;
constexpr static auto &StdWcOut = ::std::wcout;
/* ------------------------------------------------------------------------- */
using StdIOSBase = ::std::ios_base;
constexpr static auto &StdIOSNoSkipWS     = ::std::noskipws;
constexpr static auto &StdIOSDec          = ::std::dec;
constexpr static auto &StdIOSFixed        = ::std::fixed;
constexpr static auto &StdIOSHex          = ::std::hex;
constexpr static auto &StdIOSOct          = ::std::oct;
constexpr static auto &StdIOSLeft         = ::std::left;
constexpr static auto &StdIOSRight        = ::std::right;
constexpr static auto &StdIOSShowPos      = ::std::showpos;
constexpr static auto &StdIOSSetFill      = ::std::setfill<char>;
constexpr static auto &StdIOSSetFillWc    = ::std::setfill<wchar_t>;
constexpr static auto &StdIOSSetWidth     = ::std::setw;
constexpr static auto &StdIOSSetPrecision = ::std::setprecision;
constexpr static auto &StdIOSUpCase       = ::std::uppercase;
/* ------------------------------------------------------------------------- */
constexpr static StdWOStream &(*StdIOSWEndLine)(StdWOStream&) = ::std::endl;
constexpr static StdOStream &(*StdIOSEndLine)(StdOStream&)    = ::std::endl;
/* -- Other inclusions ----------------------------------------------------- */
using ::std::bind;                     using ::std::function;
using ::std::make_pair;                using ::std::make_unsigned_t;
using ::std::placeholders::_1;         using ::std::placeholders::_2;
using ::std::placeholders::_3;         using ::std::swap;
/* -- Synchronisation ------------------------------------------------------ */
using AtomicBool     = StdAtomic<bool>;     // Thread safe boolean
using AtomicDouble   = StdAtomic<double>;   // Thread safe double
using AtomicInt      = StdAtomic<int>;      // Thread safe integer
using AtomicSizeT    = StdAtomic<size_t>;   // Thread safe size_t
using AtomicUInt64   = StdAtomic<uint64_t>; // Thread safe 64-bit integer
using AtomicUnsigned = StdAtomic<unsigned>; // Thread safe unsigned integer
/* -- Indexed vector list types -------------------------------------------- */
using StringIt             = StdString::iterator;
using StringConstIt        = StdString::const_iterator;
using StringViewConstIt    = StdStringView::const_iterator;
using CStrVector           = StdVector<const char*>;
using CStrVectorConstIt    = CStrVector::const_iterator;
using FloatVector          = StdVector<float>;
using FloatVectorConstIt   = FloatVector::const_iterator;
using IntVector            = StdVector<int>;
using IntVectorConstIt     = IntVector::const_iterator;
using StrVector            = StdVector<StdString>;
using StrVectorIt          = StrVector::iterator;
using StrVectorConstIt     = StrVector::const_iterator;
using StrVectorConstRevIt  = StrVector::const_reverse_iterator;
using UIntVector           = StdVector<unsigned>;
using UIntVectorConstIt    = UIntVector::const_iterator;
using StrViewVector        = StdVector<StdStringView>;
using StrViewVectorConstIt = StrViewVector::const_iterator;
using BoolCharPair         = StdPair<const bool, const char>;
using BoolCharPairVector   = StdVector<BoolCharPair>;
/* -- Standard linked list types ------------------------------------------- */
using StrList           = StdList<StdString>;
using StrListConstIt    = StrList::const_iterator;
using StrListConstRevIt = StrList::const_reverse_iterator;
using StrListIt         = StrList::iterator;
/* -- List of StdString pairs ---------------------------------------------- */
using StrPair     = StdPair<const StdString, const StdString>;
using StrPairList = StdList<StrPair>;
/* -- Set of strings ------------------------------------------------------- */
using StrSet         = StdSet<StdString>;
using StrSetConstIt  = StrSet::const_iterator;
using StrVSet        = StdSet<StdStringView>;
using StrVSetConstIt = StrVSet::const_iterator;
using StrVUSet       = StdUnorderedSet<StdStringView> ;
/* -- Helper macro to build typedefs for a new map type -------------------- */
#define MAPPACK_BUILD(n,t1,t2) \
  using n ## MapPair = StdPair<t1, t2>; \
  using n ## Map = StdMap<n ## MapPair::first_type, \
                          n ## MapPair::second_type>; \
  using n ## MapIt = n ## Map::iterator; \
  using n ## MapConstIt = n ## Map::const_iterator;
/* -- Map of strings and unsigned integers --------------------------------- */
MAPPACK_BUILD(StrUInt, const StdString, const unsigned)
using StrUIntMapConstItVector = StdVector<StrUIntMapConstIt>;
/* -- Other map types ------------------------------------------------------ */
MAPPACK_BUILD(StrInt, const StdString, const int)
MAPPACK_BUILD(StrStr, const StdString, const StdString)
MAPPACK_BUILD(StrVStrV, const StdStringView, const StdStringView)
MAPPACK_BUILD(StrNCStr, const StdString, StdString)
/* -- Get array element type ----------------------------------------------- */
template<typename ArrayType>
  using ArrayElementType =
    StdRemoveConst<StdRemovePointer<StdRemoveReference<ArrayType>>>;
/* == EoF =========================================================== EoF == */
