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