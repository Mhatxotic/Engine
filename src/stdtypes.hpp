/* == STDTYPES.HPP ========================================================= **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## This module sets up all the types we use from the STL. All these    ## **
** ## types sit in the Engine namespace so only the engine has access to  ## **
** ## them and not any other included external API's. We also alias them  ## **
** ## to avoid having to make multiple changes if and when the STL ever   ## **
** ## changes.                                                            ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* -- Common storage spaces ------------------------------------------------ */
template<typename AnyType, size_t stSize>
  using StdArray = ::std::array<AnyType, stSize>;
template<typename AnyType>using StdAtomic = ::std::atomic<AnyType>;
template<typename AnyType>using StdDeque = ::std::deque<AnyType>;
template<typename AnyType>using StdList = ::std::list<AnyType>;
template<typename KeyType, typename ValueType>
  using StdMap = ::std::map<KeyType, ValueType>;
template<typename AnyTypeA, typename AnyTypeB>
  using StdPair = ::std::pair<AnyTypeA, AnyTypeB>;
template<typename AnyType>using StdSet = ::std::set<AnyType>;
template<typename AnyType, typename DelFunc = ::std::default_delete<AnyType>>
  using StdUniquePtr = ::std::unique_ptr<AnyType, DelFunc>;
template<typename AnyType>
  using StdUnorderedSet = ::std::unordered_set<AnyType>;
template<typename AnyType>using StdVector = ::std::vector<AnyType>;
/* -- Strings -------------------------------------------------------------- */
using StdString = ::std::string;
using StdStringView = ::std::string_view;
using StdWideString = ::std::wstring;
/* -- Exceptions ----------------------------------------------------------- */
using StdException = ::std::exception;
using StdRunTimeError = ::std::runtime_error;
/* -- String streams ------------------------------------------------------- */
using ::std::dec;                      using ::std::endl;
using ::std::fixed;                    using ::std::fpclassify;
using ::std::get_time;                 using ::std::hex;
using ::std::ios_base;                 using ::std::istringstream;
using ::std::left;                     using ::std::noskipws;
using ::std::oct;                      using ::std::ostream_iterator;
using ::std::ostringstream;            using ::std::put_time;
using ::std::right;                    using ::std::setfill;
using ::std::setprecision;             using ::std::setw;
using ::std::showpos;                  using ::std::uppercase;
using ::std::wcerr;                    using ::std::wcout;
/* -- Constexpr functions -------------------------------------------------- */
using ::std::is_arithmetic_v;          using ::std::is_class_v;
using ::std::is_enum_v;                using ::std::is_floating_point_v;
using ::std::is_integral_v;            using ::std::is_null_pointer_v;
using ::std::is_pointer_v;             using ::std::is_reference_v;
using ::std::is_same_v;                using ::std::is_signed_v;
using ::std::is_trivially_copyable_v;  using ::std::underlying_type_t;
/* -- Bind class function argument namespaces ------------------------------ */
using ::std::placeholders::_1;         using ::std::placeholders::_2;
using ::std::placeholders::_3;
/* -- Other ---------------------------------------------------------------- */
using ::std::addressof;                using ::std::bind;
using ::std::bit_cast;                 using ::std::function;
using ::std::gcd;                      using ::std::index_sequence;
using ::std::locale;                   using ::std::make_index_sequence;
using ::std::make_pair;                using ::std::make_signed_t;
using ::std::make_unsigned_t;          using ::std::nothrow;
using ::std::numeric_limits;           using ::std::remove_const_t;
using ::std::remove_cv_t;              using ::std::remove_pointer_t;
using ::std::remove_reference_t;       using ::std::swap;
using ::std::tuple_size_v;
/* -- Synchronisation ------------------------------------------------------ */
typedef StdAtomic<bool>         AtomicBool;   // Thread safe boolean
typedef StdAtomic<double>       AtomicDouble; // Thread safe double
typedef StdAtomic<int>          AtomicInt;    // Thread safe integer
typedef StdAtomic<size_t>       AtomicSizeT;  // Thread safe size_t
typedef StdAtomic<uint64_t>     AtomicUInt64; // Thread safe 64-bit integer
typedef StdAtomic<unsigned int> AtomicUInt;   // Thread safe unsigned integer
/* -- Indexed vector list types -------------------------------------------- */
typedef StdString::const_iterator         StringConstIt;
typedef StdVector<const char*>            CStrVector;
typedef CStrVector::const_iterator        CStrVectorConstIt;
typedef StdVector<float>                  FloatVector;
typedef FloatVector::const_iterator       FloatVectorConstIt;
typedef StdVector<int>                    IntVector;
typedef IntVector::const_iterator         IntVectorConstIt;
typedef StdVector<StdString>              StrVector;
typedef StrVector::iterator               StrVectorIt;
typedef StrVector::const_iterator         StrVectorConstIt;
typedef StrVector::const_reverse_iterator StrVectorConstRevIt;
typedef StdVector<unsigned int>           UIntVector;
typedef UIntVector::const_iterator        UIntVectorConstIt;
typedef StdVector<StdStringView>          StrViewVector;
typedef StrViewVector::const_iterator     StrViewVectorConstIt;
/* -- Standard linked list types ------------------------------------------- */
typedef StdList<StdString>              StrList;
typedef StrList::const_iterator         StrListConstIt;
typedef StrList::const_reverse_iterator StrListConstRevIt;
typedef StrList::iterator               StrListIt;
/* -- List of StdString pairs ---------------------------------------------- */
typedef StdPair<const StdString, const StdString> StrPair;
typedef StdList<StrPair>               StrPairList;
/* -- Set of strings ------------------------------------------------------- */
typedef StdSet<StdString>              StrSet;
typedef StrSet::const_iterator         StrSetConstIt;
typedef StdUnorderedSet<StdStringView> StrVUSet;
/* -- Helper macro to build typedefs for a new map type -------------------- */
#define MAPPACK_BUILD(n,t1,t2) \
  typedef StdPair<t1,t2> n ## MapPair; \
  typedef StdMap<n ## MapPair::first_type, n ## MapPair::second_type> \
    n ## Map; \
  typedef n ## Map::iterator n ## MapIt; \
  typedef n ## Map::const_iterator n ## MapConstIt;
/* -- Map of strings and unsigned ints ------------------------------------- */
MAPPACK_BUILD(StrUInt, const StdString, const unsigned int)
typedef StdVector<StrUIntMapConstIt> StrUIntMapConstItVector;
/* -- Other map types ------------------------------------------------------ */
MAPPACK_BUILD(StrInt, const StdString, const int)
MAPPACK_BUILD(StrStr, const StdString, const StdString)
MAPPACK_BUILD(StrVStrV, const StdStringView, const StdStringView)
MAPPACK_BUILD(StrNCStr, const StdString, StdString)
/* -- Get array element type ----------------------------------------------- */
template<typename ArrayType>
  using ArrayElementType =
    remove_cv_t<remove_pointer_t<remove_reference_t<ArrayType>>>;
/* == EoF =========================================================== EoF == */
