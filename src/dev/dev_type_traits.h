#pragma once

#include <type_traits>

namespace dev {

// ###################
// # propagate_const #
// ###################

template<typename T, typename U>
struct propagate_const : std::type_identity<U> {};

template<typename T, typename U>
struct propagate_const<const T, U> : std::type_identity<const U> {};

template<typename T, typename U>
using propagate_const_t = typename propagate_const<T, U>::type;

// ###############
// # is_template #
// ###############

template<template<typename...> typename template_t, typename T>
struct is_template : std::false_type {};

template<template<typename...> typename template_t, typename T>
constexpr bool is_template_v =  is_template<template_t, T>::value;

template<template<typename...> typename template_t, typename... args_t>
struct is_template<template_t, template_t<args_t...>> : std::true_type {};

// #################
// # template_cast #
// #################

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

// // ###############
// // # disjunction #
// // ###############

// template<template<typename> typename... preds_t>
// struct disjunction
// {
//     template<typename T>
//     using pred = std::disjunction<preds_t<T>...>;
// };

} // namespace dev


// ##################
// # type_list_cast #
// ##################

#include <dev_type_list.h>
#include <tuple>
#include <variant>

template<typename T>
using type_list_cast = dev::template_cast<dev::type_list, T>;

template<typename T>
using type_list_cast_t = typename type_list_cast<T>::type;

static_assert(std::is_same_v<type_list_cast_t<std::tuple<>>, dev::type_list<>>);
static_assert(std::is_same_v<type_list_cast_t<std::tuple<int, float>>, dev::type_list<int, float>>);
static_assert(std::is_same_v<type_list_cast_t<std::variant<>>, dev::type_list<>>);
static_assert(std::is_same_v<type_list_cast_t<std::variant<int, float>>, dev::type_list<int, float>>);