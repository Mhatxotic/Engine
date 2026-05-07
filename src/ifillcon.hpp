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
template<size_t... stInitialisers>
  using StdIndexSequence = ::std::index_sequence<stInitialisers...>;
template<size_t stCount>
  using StdMakeIndexSequence = ::std::make_index_sequence<stCount>;
/* -- Helper to initialise an array of the specified value ----------------- */
namespace MakeFilledContainer
{ /* -- Fill with specified value ------------------------------------------ */
  template<typename IntType>           // Container element type
    constexpr IntType Value(const size_t, const IntType &itValue)
  { return itValue; }
  /* -- Select indices ----------------------------------------------------- */
  template<class ContainerType,        // Container type (i.e. StdArray<>)
    typename IntType,                  // Container element type
    size_t stCount,                    // Container maximum elements
    size_t... stInitialisers>          // Parameter id
  constexpr ContainerType Select(const IntType &itValue,
    StdIndexSequence<stInitialisers...>)
  { return { Value<IntType>(stInitialisers, itValue)... }; }
  /* -- Entry function ----------------------------------------------------- */
  template<class ContainerType,                     // Container type
    typename IntType = ContainerType::value_type,   // Container element type
      size_t stCount = StdTupleSize<ContainerType>> // Number of items in it
  constexpr ContainerType FillConGeneric(const IntType &itValue)
    { return Select<ContainerType, IntType, stCount>
        (itValue, StdMakeIndexSequence<stCount>{}); }
};
/* -- Helper to initialise an array of the specified value ----------------- */
namespace MakeFilledClassContainer
{ /* -- Fill with specified value ------------------------------------------ */
  template<class ContainerType,         // Container type
    typename AnyType>                   // Argument type
  requires StdIsClass<ContainerType> && // Container type must be a class
    StdIsArithmatic<AnyType>            // Argument type must be computable
  constexpr static ContainerType Value(const size_t, AnyType &atValue)
    { return ContainerType{ atValue++ }; }
  /* -- Select indices ----------------------------------------------------- */
  template<class ContainerType,         // Container type
    typename AnyType,                   // Argument type
    size_t... stInitialisers>           // List of index values
  constexpr static ContainerType Select(AnyType &atValue,
    StdIndexSequence<stInitialisers...>)
  { using ValueType = typename ContainerType::value_type; // Element type
    return { Value<ValueType,AnyType>(stInitialisers, atValue)... }; }
  /* -- Entry function ----------------------------------------------------- */
  template<class ContainerType,        // Container type
           typename AnyType>           // Number of items in container
  constexpr static ContainerType FillConClass(AnyType atValue=0)
    { return Select<ContainerType,AnyType>
        (atValue, StdMakeIndexSequence<StdTupleSize<ContainerType>>{}); }
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
