#ifndef TYPE_LIST_H
#define TYPE_LIST_H

#include <type_traits>

// #############
// # type_list #
// #############

template<typename... args_t>
struct type_list{};


// #####################
// # type_list_conains #
// #####################

template<typename list_t, typename item_t>
struct type_list_contains : std::false_type {};

template<
    typename item_t,
    typename first_t,
    typename... the_rest_t>
struct type_list_contains<type_list<first_t, the_rest_t...>, item_t>
    : type_list_contains<type_list<the_rest_t...>, item_t> {};

template<
    typename item_t,
    typename... the_rest_t>
struct type_list_contains<type_list<item_t, the_rest_t...>, item_t> : std::true_type {};

template<typename list_t, typename item_t>
constexpr bool type_list_contains_v = type_list_contains<list_t, item_t>::value;


// ####################
// # type_list_concat #
// ####################

template<typename list_a_t, typename list_b_t>
struct type_list_concat;

template<typename list_a_t, typename list_b_t>
using type_list_concat_t = typename type_list_concat<list_a_t, list_b_t>::type;

template<typename... list_a_args_t, typename... list_b_args_t>
struct type_list_concat<type_list<list_a_args_t...>, type_list<list_b_args_t...>>
{
    using type = type_list<list_a_args_t..., list_b_args_t...>;
};

static_assert(std::is_same_v<type_list_concat_t<type_list<>, type_list<>>, type_list<>>);
static_assert(std::is_same_v<type_list_concat_t<type_list<int>, type_list<>>, type_list<int>>);
static_assert(std::is_same_v<type_list_concat_t<type_list<>, type_list<int>>, type_list<int>>);
static_assert(std::is_same_v<type_list_concat_t<type_list<int>, type_list<int>>, type_list<int, int>>);
static_assert(std::is_same_v<type_list_concat_t<type_list<int>, type_list<float, double>>, type_list<int, float, double>>);
static_assert(std::is_same_v<type_list_concat_t<type_list<float, int>, type_list<float, double>>, type_list<float, int, float, double>>);

// ####################
// # type_list_filter #
// ####################

template<typename list_t, template<typename> typename pred_t>
struct type_list_filter;

template<typename list_t, template<typename> typename pred_t>
using type_list_filter_t = typename type_list_filter<list_t, pred_t>::type;

template<template<typename> typename pred_t>
struct type_list_filter<type_list<>, pred_t>
{
    using type = type_list<>;
};

template<template<typename> typename pred_t, typename first_t, typename... the_rest_t>
struct type_list_filter<type_list<first_t, the_rest_t...>, pred_t>
{
    using type = std::conditional_t<
        pred_t<first_t>::value,
        type_list_concat_t<
            type_list<first_t>, 
            type_list_filter_t<
                type_list<the_rest_t...>, 
                pred_t
            >
        >,
        type_list_filter_t<
            type_list<the_rest_t...>, 
            pred_t
        >
    >;
};

template<typename T>
struct is_int : std::is_same<T, int> {};

template<typename T>
constexpr bool is_int_v = is_int<T>::value;

static_assert( is_int_v<int>);
static_assert(!is_int_v<float>);
static_assert(!is_int_v<double>);

template<template<typename> typename trait_t>
struct trait_op
{
    template<typename T>
    using negate = std::negation<trait_t<T>>;

    template<typename T>
    static constexpr bool negate_v = negate<T>::value;
};

static_assert(!std::negation_v<is_int<int>>);
static_assert( std::negation_v<is_int<float>>);
static_assert( std::negation_v<is_int<double>>);

static_assert(!trait_op<is_int>::negate_v<int>);
static_assert( trait_op<is_int>::negate_v<float>);
static_assert( trait_op<is_int>::negate_v<double>);

static_assert(
    std::is_same_v<
        type_list_filter_t<
            type_list<>, 
            is_int
        >, 
        type_list<>
    >);

static_assert(
    std::is_same_v<
        type_list_filter_t<
            type_list<int>, 
            is_int
        >, 
        type_list<int>
    >);

static_assert(
    std::is_same_v<
        type_list_filter_t<
            type_list<int>, 
            trait_op<is_int>::negate
        >, 
        type_list<>
    >);

static_assert(
    std::is_same_v<
        type_list_filter_t<
            type_list<float>, 
            is_int
        >, 
        type_list<>
    >);

static_assert(
    std::is_same_v<
        type_list_filter_t<
            type_list<float>, 
            trait_op<is_int>::negate
        >, 
        type_list<float>
    >);

static_assert(
    std::is_same_v<
        type_list_filter_t<
            type_list<float, double>, 
            is_int
        >, 
        type_list<>
    >);

static_assert(
    std::is_same_v<
        type_list_filter_t<
            type_list<float, double>, 
            trait_op<is_int>::negate
        >, 
        type_list<float, double>
    >);

static_assert(
    std::is_same_v<
        type_list_filter_t<
            type_list<int, float, int, double>, 
            is_int
        >, 
        type_list<int, int>
    >);

static_assert(
    std::is_same_v<
        type_list_filter_t<
            type_list<int, float, int, double>, 
            trait_op<is_int>::negate
        >, 
        type_list<float, double>
    >);

// #################
// # type_list_any #
// #################

template<typename list_t, template<typename> typename pred_t>
struct type_list_any; // TODO MG: simplify

template<typename list_t, template<typename> typename pred_t>
constexpr bool type_list_any_v = type_list_any<list_t, pred_t>::value;

template<template<typename> typename pred_t>
struct type_list_any<type_list<>, pred_t> : std::false_type {};

template<template<typename> typename pred_t, typename first_t, typename... the_rest_t>
struct type_list_any<type_list<first_t, the_rest_t...>, pred_t>
: std::bool_constant<pred_t<first_t>::value || type_list_any_v<type_list<the_rest_t...>, pred_t>>
{};

static_assert(!type_list_any_v<type_list<>, is_int>);
static_assert(!type_list_any_v<type_list<float>, is_int>);
static_assert(!type_list_any_v<type_list<float, double>, is_int>);
static_assert( type_list_any_v<type_list<int>, is_int>);
static_assert( type_list_any_v<type_list<float, double, int>, is_int>);
static_assert( type_list_any_v<type_list<float, int, float, int>, is_int>);


#endif