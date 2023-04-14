#pragma once

#include <tuple>
#include <variant>
#include <type_traits>

// #############
// # type_list #
// #############

template<typename... args_t>
struct type_list{};

// ######################
// # type_list_contains #
// ######################

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

static_assert(!type_list_contains_v<type_list<>, int>);
static_assert(!type_list_contains_v<type_list<float, double>, int>);
static_assert( type_list_contains_v<type_list<int>, int>);
static_assert( type_list_contains_v<type_list<int, float>, int>);
static_assert( type_list_contains_v<type_list<float, int>, int>);

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
struct type_list_filter : std::type_identity<type_list<>> {};

template<typename list_t, template<typename> typename pred_t>
using type_list_filter_t = typename type_list_filter<list_t, pred_t>::type;

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
struct type_list_any;

template<template<typename> typename pred_t, typename... Ts>
struct type_list_any<type_list<Ts...>, pred_t> : std::disjunction<pred_t<Ts>...> {};

template<typename list_t, template<typename> typename pred_t>
constexpr bool type_list_any_v = type_list_any<list_t, pred_t>::value;

static_assert(!type_list_any_v<type_list<>, is_int>);
static_assert(!type_list_any_v<type_list<float>, is_int>);
static_assert(!type_list_any_v<type_list<float, double>, is_int>);
static_assert( type_list_any_v<type_list<float, double>, trait_op<is_int>::negate>);
static_assert(!type_list_any_v<type_list<int, int, int>, trait_op<is_int>::negate>);
static_assert( type_list_any_v<type_list<int>, is_int>);
static_assert( type_list_any_v<type_list<float, double, int>, is_int>);
static_assert( type_list_any_v<type_list<float, int, float, int>, is_int>);
static_assert( type_list_any_v<type_list<float, int, float, int>, trait_op<is_int>::negate>);

// #################
// # type_list_all #
// #################

template<typename list_t, template<typename> typename pred_t>
struct type_list_all;

template<template<typename> typename pred_t, typename... Ts>
struct type_list_all<type_list<Ts...>, pred_t> : std::conjunction<pred_t<Ts>...> {};

template<typename list_t, template<typename> typename pred_t>
constexpr bool type_list_all_v = type_list_all<list_t, pred_t>::value;

static_assert( type_list_all_v<type_list<>, is_int>);
static_assert(!type_list_all_v<type_list<float>, is_int>);
static_assert(!type_list_all_v<type_list<float, double>, is_int>);
static_assert( type_list_all_v<type_list<int>, is_int>);
static_assert( type_list_all_v<type_list<int, int, int>, is_int>);
static_assert(!type_list_all_v<type_list<float, double, int>, is_int>);
static_assert( type_list_all_v<type_list<int, int>, is_int>);
static_assert(!type_list_all_v<type_list<float, int, float, int>, trait_op<is_int>::negate>);
static_assert( type_list_all_v<type_list<float, float, double>, trait_op<is_int>::negate>);

// #######################
// # type_list_transform #
// #######################

template<typename list_t, template<typename> typename op_t>
struct type_list_transform;

template<template<typename> typename op_t, typename... Ts>
struct type_list_transform<type_list<Ts...>, op_t> : std::type_identity<type_list<op_t<Ts>...>> {};

template<typename list_t, template<typename> typename op_t>
using type_list_transform_t = typename type_list_transform<list_t, op_t>::type;

static_assert(std::is_same_v<type_list_transform_t<type_list<>, std::add_const_t>, type_list<>>);
static_assert(std::is_same_v<type_list_transform_t<type_list<int>, std::add_const_t>, type_list<const int>>);
static_assert(std::is_same_v<type_list_transform_t<type_list<int, double>, std::add_const_t>, type_list<const int, const double>>);

// ##################
// # type_list_cast #
// ##################

template<template<typename...> typename template_to_t, typename T>
struct template_cast;

template<
    template<typename...> typename template_to_t,
    template<typename...> typename template_from_t,
    typename... Ts>
struct template_cast<template_to_t, template_from_t<Ts...>>
: std::type_identity<template_to_t<Ts...>> {};

template<template<typename...> typename template_to_t, typename T>
using template_cast_t = typename template_cast<template_to_t, T>::type;



template<typename T>
using type_list_cast = template_cast<type_list, T>;

template<typename T>
using type_list_cast_t = typename type_list_cast<T>::type;

static_assert(std::is_same_v<type_list_cast_t<std::tuple<>>, type_list<>>);
static_assert(std::is_same_v<type_list_cast_t<std::tuple<int, float>>, type_list<int, float>>);
static_assert(std::is_same_v<type_list_cast_t<std::variant<>>, type_list<>>);
static_assert(std::is_same_v<type_list_cast_t<std::variant<int, float>>, type_list<int, float>>);