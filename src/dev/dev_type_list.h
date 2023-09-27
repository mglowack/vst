#pragma once

#include "dev_type_traits.h"

#include <type_traits>

namespace dev {

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

// #################
// # type_list_any #
// #################

template<typename list_t, template<typename> typename pred_t>
struct type_list_any;

template<template<typename> typename pred_t, typename... Ts>
struct type_list_any<type_list<Ts...>, pred_t> : std::disjunction<pred_t<Ts>...> {};

template<typename list_t, template<typename> typename pred_t>
constexpr bool type_list_any_v = type_list_any<list_t, pred_t>::value;

// #################
// # type_list_all #
// #################

template<typename list_t, template<typename> typename pred_t>
struct type_list_all;

template<template<typename> typename pred_t, typename... Ts>
struct type_list_all<type_list<Ts...>, pred_t> : std::conjunction<pred_t<Ts>...> {};

template<typename list_t, template<typename> typename pred_t>
constexpr bool type_list_all_v = type_list_all<list_t, pred_t>::value;

// #######################
// # type_list_transform #
// #######################

template<typename list_t, template<typename> typename op_t>
struct type_list_transform;

template<template<typename> typename op_t, typename... Ts>
struct type_list_transform<type_list<Ts...>, op_t> : std::type_identity<type_list<op_t<Ts>...>> {};

template<typename list_t, template<typename> typename op_t>
using type_list_transform_t = typename type_list_transform<list_t, op_t>::type;

// ##################
// # type_list_cast #
// ##################

template<typename T>
using type_list_cast = dev::template_cast<dev::type_list, T>;

template<typename T>
using type_list_cast_t = typename type_list_cast<T>::type;


} // namespace dev