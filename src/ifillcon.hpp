/* == IFILLCON.HPP ========================================================= **
** ######################################################################### **
** ## Mhatxotic Engine          (c) Mhatxotic Design, All Rights Reserved ## **
** ######################################################################### **
** ## Uses constant expression to initialise a container in a             ## **
** ## constructor initialiser.                                            ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IFillCon {                   // Start of private module namespace
/* -- Aliases -------------------------------------------------------------- */
template<size_t... Is>using StdIndexSequence = ::std::index_sequence<Is...>;
template<size_t N> using StdMakeIndexSequence = ::std::make_index_sequence<N>;
template<typename T>
  constexpr static size_t StdTupleSize = ::std::tuple_size_v<T>;
/* -- Helper to initialise an array of the specified value ----------------- */
namespace MakeFilledContainer
{ /* -- Fill with specified value ------------------------------------------ */
  template<typename IT>                // Container element type
    constexpr IT Value(const size_t, const IT &itValue) { return itValue; }
  /* -- Select indices ----------------------------------------------------- */
  template<class CT,                   // Container type (i.e. StdArray<>)
    typename IT,                       // Container element type
    size_t N,                          // Container maximum elements
    size_t... Is>                      // Parameter id
  constexpr CT Select(const IT &itValue, StdIndexSequence<Is...>)
  { return { Value<IT>(Is, itValue)... }; }
  /* -- Entry function ----------------------------------------------------- */
  template<class CT,                   // Container type
    typename IT = CT::value_type,      // Container element type
    size_t N = StdTupleSize<CT>>       // Number of items in container
  constexpr CT FillConGeneric(const IT &itValue)
    { return Select<CT, IT, N>(itValue, StdMakeIndexSequence<N>{}); }
};
/* -- Helper to initialise an array of the specified value ----------------- */
namespace MakeFilledClassContainer
{ /* -- Fill with specified value ------------------------------------------ */
  template<class CT,                   // Container type
    typename AT>                       // Argument type
  requires is_class_v<CT> &&           // Container type must be a class
    is_arithmetic_v<AT>                // Argument type must be computable
  constexpr static CT Value(const size_t, AT &atValue)
    { return CT{ atValue++ }; }
  /* -- Select indices ----------------------------------------------------- */
  template<class CT,                   // Container type
    typename AT,                       // Argument type
    typename VT = CT::value_type,      // Container item value type
    size_t... Is>                      // List of index values
  constexpr static CT Select(AT &atValue, StdIndexSequence<Is...>)
    { return { Value<VT,AT>(Is, atValue)... }; }
  /* -- Entry function ----------------------------------------------------- */
  template<class CT,                   // Container type
    typename AT,                       // Argument type
    size_t N = StdTupleSize<CT>>       // Number of items in container
  constexpr static CT FillConClass(AT atValue=0)
    { return Select<CT,AT>(atValue, StdMakeIndexSequence<N>{}); }
}; /* ---------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* -- Bring main functions into namespace ---------------------------------- */
using MakeFilledContainer::FillConGeneric;
using MakeFilledClassContainer::FillConClass;
/* ------------------------------------------------------------------------- */
};                                     // End of private module namespace
/* ------------------------------------------------------------------------- */
};                                     // End of public module namespace
/* == EoF =========================================================== EoF == */
