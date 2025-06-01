/* == STDTYPES.HPP ========================================================= **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## This module sets up all the types we use from the STL. All these    ## **
** ## types should sit in the Engine namespace so only the engine has     ## **
** ## access to them and not any other included API.                      ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* -- Storage spaces ------------------------------------------------------- */
using ::std::array;                    using ::std::deque;
using ::std::list;                     using ::std::map;
using ::std::queue;                    using ::std::pair;
using ::std::set;                      using ::std::string;
using ::std::string_view;              using ::std::unordered_set;
using ::std::vector;                   using ::std::wstring;
/* -- Exceptions ----------------------------------------------------------- */
using ::std::exception;                using ::std::runtime_error;
/* -- Other ---------------------------------------------------------------- */
using ::std::addressof;                using ::std::bind;
using ::std::bit_cast;                 using ::std::function;
using ::std::gcd;                      using ::std::index_sequence;
using ::std::locale;                   using ::std::make_index_sequence;
using ::std::make_pair;                using ::std::make_signed_t;
using ::std::make_unsigned_t;          using ::std::nothrow;
using ::std::numeric_limits;           using ::std::remove_const_t;
using ::std::remove_pointer_t;         using ::std::swap;
using ::std::tuple_size_v;
/* -- Iteratations --------------------------------------------------------- */
using ::std::accumulate;               using ::std::advance;
using ::std::any_of;                   using ::std::back_inserter;
using ::std::next;                     using ::std::prev;
/* -- String streams ------------------------------------------------------- */
using ::std::dec;                      using ::std::fixed;
using ::std::fpclassify;               using ::std::get_time;
using ::std::hex;                      using ::std::ios_base;
using ::std::istringstream;            using ::std::left;
using ::std::noskipws;                 using ::std::oct;
using ::std::ostream_iterator;         using ::std::ostringstream;
using ::std::put_time;                 using ::std::right;
using ::std::setfill;                  using ::std::setprecision;
using ::std::setw;                     using ::std::showpos;
using ::std::uppercase;
/* -- Constexpr functions -------------------------------------------------- */
using ::std::is_class_v;               using ::std::is_floating_point_v;
using ::std::is_integral_v;            using ::std::is_pointer_v;
using ::std::is_signed_v;              using ::std::is_enum_v;
using ::std::is_same_v;                using ::std::is_trivially_copyable_v;
using ::std::underlying_type_t;
/* -- Bind class function argument namespaces ------------------------------ */
using ::std::placeholders::_1;         using ::std::placeholders::_2;
using ::std::placeholders::_3;
/* -- Times ---------------------------------------------------------------- */
using ::std::chrono::duration_cast;    using ::std::chrono::duration;
using ::std::chrono::microseconds;     using ::std::chrono::milliseconds;
using ::std::chrono::nanoseconds;      using ::std::chrono::seconds;
using ::std::chrono::system_clock;
/* -- Operating system specific -------------------------------------------- */
using ::std::terminate_handler;        using ::std::set_terminate;
#if !defined(MACOS)                    // Not using apple compiler?
using ::std::execution::par_unseq;     using ::std::execution::par;
using ::std::execution::seq;
#endif                                 // Apple check
/* -- Asynchronisation ----------------------------------------------------- */
using ::std::atomic;                   using ::std::condition_variable;
using ::std::lock_guard;               using ::std::mutex;
using ::std::scoped_lock;              using ::std::thread;
using ::std::try_to_lock;              using ::std::unique_lock;
using ::std::unique_ptr;
typedef atomic<bool>       SafeBool;   // Thread safe boolean
typedef atomic<double>     SafeDouble; // Thread safe double
typedef atomic<int>        SafeInt;    // Thread safe integer
typedef atomic<long>       SafeLong;   // Thread safe long
typedef atomic<size_t>     SafeSizeT;  // Thread safe size_t
typedef atomic<uint64_t>   SafeUInt64; // Thread safe 64-bit integer
typedef atomic<unsigned int> SafeUInt; // Thread safe unsigned integer
typedef lock_guard<mutex>  LockGuard;  // Shortcut to a mutex lock guard
typedef unique_lock<mutex> UniqueLock; // SHortcut to a mutex unique lock
/* -- Clocks --------------------------------------------------------------- */
using CoreClock = ::std::chrono::high_resolution_clock; // Using HRC
typedef CoreClock::time_point ClkTimePoint;             // Holds a time
typedef CoreClock::duration   ClkDuration;              // Holds a duration
typedef atomic<ClkDuration>   SafeClkDuration;          // Thread safe duration
/* -- Indexed vector list types -------------------------------------------- */
typedef vector<const char*>               CStrVector;
typedef CStrVector::const_iterator        CStrVectorConstIt;
typedef vector<float>                     FloatVector;
typedef FloatVector::const_iterator       FloatVectorConstIt;
typedef vector<int>                       IntVector;
typedef IntVector::const_iterator         IntVectorConstIt;
typedef vector<string>                    StrVector;
typedef StrVector::iterator               StrVectorIt;
typedef StrVector::const_iterator         StrVectorConstIt;
typedef StrVector::const_reverse_iterator StrVectorConstRevIt;
typedef vector<unsigned int>              UIntVector;
typedef UIntVector::const_iterator        UIntVectorConstIt;
typedef vector<string_view>               StrViewVector;
typedef StrViewVector::const_iterator     StrViewVectorConstIt;
/* -- Standard linked list types ------------------------------------------- */
typedef list<string>                    StrList;
typedef StrList::const_iterator         StrListConstIt;
typedef StrList::const_reverse_iterator StrListConstRevIt;
typedef StrList::iterator               StrListIt;
/* -- List of string pairs ------------------------------------------------- */
typedef pair<const string, const string> StrPair;
typedef list<StrPair>                    StrPairList;
/* -- Set of strings ------------------------------------------------------- */
typedef set<string>            StrSet;
typedef StrSet::const_iterator StrSetConstIt;
typedef unordered_set<string_view> StrVUSet;
/* -- Helper macro to build typedefs for a new map type -------------------- */
#define MAPPACK_BUILD(n,t1,t2) \
  typedef pair<t1,t2> n ## MapPair; \
  typedef map<n ## MapPair::first_type, n ## MapPair::second_type> n ## Map; \
  typedef n ## Map::iterator n ## MapIt; \
  typedef n ## Map::const_iterator n ## MapConstIt;
/* -- Map of strings and unsigned ints ------------------------------------- */
MAPPACK_BUILD(StrUInt, const string, const unsigned int)
typedef vector<StrUIntMapConstIt> StrUIntMapConstItVector;
/* -- Other map types ------------------------------------------------------ */
MAPPACK_BUILD(StrInt, const string, const int)
MAPPACK_BUILD(StrStr, const string, const string)
MAPPACK_BUILD(StrVStrV, const string_view, const string_view)
MAPPACK_BUILD(StrNCStr, const string, string)
/* == EoF =========================================================== EoF == */
