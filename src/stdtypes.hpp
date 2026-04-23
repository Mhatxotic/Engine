/* == STDTYPES.HPP ========================================================= **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## This module sets up all the common types we use from the STL. All   ## **
** ## these types sit in the Engine namespace so only the engine has      ## **
** ## access to them and not any other included external API's. We also   ## **
** ## alias them to avoid having to make multiple changes if and when the ## **
** ## STL ever changes which it actually does sometimes. Also we also     ## **
** ## like all our type names and function names with capitalised first   ## **
** ## letters of each syllable.                                           ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* -- Common storage type aliases ------------------------------------------ */
template<typename T, size_t S>using StdArray = ::std::array<T,S>;
template<typename T>using StdAtomic = ::std::atomic<T>;
template<typename T>using StdDeque = ::std::deque<T>;
template<typename T>using StdList = ::std::list<T>;
template<typename KT,typename VT>using StdMap = ::std::map<KT,VT>;
template<typename AT,typename BT>using StdPair = ::std::pair<AT,BT>;
template<typename T>using StdSet = ::std::set<T>;
template<typename T,typename F = ::std::default_delete<T>>
  using StdUniquePtr = ::std::unique_ptr<T,F>;
template<typename T>using StdUnorderedSet = ::std::unordered_set<T>;
template<typename T>using StdVector = ::std::vector<T>;
/* -- Type manipulation aliases -------------------------------------------- */
template<typename T>using StdRemoveConst = ::std::remove_const_t<T>;
template<typename T>using StdRemovePointer = ::std::remove_pointer_t<T>;
template<typename T>using StdRemoveReference = ::std::remove_reference_t<T>;
/* -- Template type checks ------------------------------------------------- */
template<class T>
  constexpr static bool StdIsArithmatic = ::std::is_arithmetic_v<T>;
template<class T>constexpr static bool StdIsClass = ::std::is_class_v<T>;
template<class T>constexpr static bool StdIsEnum = ::std::is_enum_v<T>;
template<class T>
  constexpr static bool StdIsFloat = ::std::is_floating_point_v<T>;
template<class T>constexpr static bool StdIsInteger = ::std::is_integral_v<T>;
template<class T>constexpr static bool StdIsNull = ::std::is_null_pointer_v<T>;
template<class T>constexpr static bool StdIsPointer = ::std::is_pointer_v<T>;
template<class T>
  constexpr static bool StdIsReference = ::std::is_reference_v<T>;
template<class AT,class BT>
  constexpr static bool StdIsSame = ::std::is_same_v<AT,BT>;
template<class T>constexpr static bool StdIsSigned = ::std::is_signed_v<T>;
/* ------------------------------------------------------------------------- */
template<typename T>using StdLimits = ::std::numeric_limits<T>;
/* -- String type aliases -------------------------------------------------- */
using StdIStringStream = ::std::istringstream;
using StdOStream       = ::std::ostream;
using StdOStringStream = ::std::ostringstream;
using StdString        = ::std::string;
using StdStringView    = ::std::string_view;
using StdWideString    = ::std::wstring;
/* -- Exception aliases ---------------------------------------------------- */
using StdException    = ::std::exception;
using StdRunTimeError = ::std::runtime_error;
/* -- IOStream manipulation ------------------------------------------------ */
constexpr static StdOStream &(*StdIOSEndLine)(StdOStream&) = std::endl;
constexpr static auto &StdIOSDec     = ::std::dec;
constexpr static auto &StdIOSFixed   = ::std::fixed;
constexpr static auto &StdIOSHex     = ::std::hex;
constexpr static auto &StdIOSLeft    = ::std::left;
constexpr static auto &StdIOSRight   = ::std::right;
constexpr static auto &StdIOSSetFill      = ::std::setfill<char>;
constexpr static auto &StdIOSSetWidth     = ::std::setw;
constexpr static auto &StdIOSSetPrecision = ::std::setprecision;
/* -- Other inclusions ----------------------------------------------------- */
using ::std::bind;                     using ::std::function;
using ::std::make_pair;                using ::std::make_signed_t;
using ::std::make_unsigned_t;          using ::std::placeholders::_1;
using ::std::placeholders::_2;         using ::std::placeholders::_3;
using ::std::swap;
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
template<typename ArrayType> using ArrayElementType =
  StdRemoveConst<StdRemovePointer<StdRemoveReference<ArrayType>>>;
/* == EoF =========================================================== EoF == */
