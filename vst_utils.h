#ifndef VST_UTILS_H
#define VST_UTILS_H

#include "vst_defs.h"

#include <type_traits>

namespace vst {

// ##################
// # has_get_fields #
// ##################

template<typename T, typename ENABLER = std::void_t<>>
constexpr bool has_get_fields = false;

template<typename T>
constexpr bool has_get_fields<
    T, 
    std::void_t<decltype(T::get_fields())>>
= true;

// ###############
// # is_vst_type #
// ###############

template<typename T>
constexpr bool is_vst_type = false;

template<typename T, typename... ops>
constexpr bool is_vst_type<type<T, ops...>> = true;

// ##############
// # underlying #
// ##############

template<typename T>
struct underlying;

template<typename T, typename... ops>
struct underlying<type<T, ops...>>
{
    using type = T;
};

template<typename T>
using underlying_t = typename underlying<T>::type;

// namespace std {
// template<typename T>
// struct type_identity
// {
//     using type = T;
// };
// }

// template<typename T>
// struct aggregate : std::type_identity<T> {};

// template<typename T, typename... ops>
// struct aggregate<type<T, ops...>> : std::type_identity<T> {};

// template<typename T>
// using aggregate_t = typename aggregate<T>::type;

// // ###################
// // # propagate_const #
// // ###################

// template<typename T, typename U>
// struct propagate_const : std::type_identity<U> {};

// template<typename T, typename U>
// struct propagate_const<const T, U> : std::type_identity<const U> {};

// template<typename T, typename U>
// using propagate_const_t = typename propagate_const<T, U>::type;

// #################
// # is_fields_def #
// #################

template<typename T>
struct is_fields_def : std::false_type {};

template<typename T>
constexpr bool is_fields_def_v = is_fields_def<T>::value;

template<auto (*f)()>
struct is_fields_def<with_fields::from_func<f>> : std::true_type {};

template<auto v>
struct is_fields_def<with_fields::from_var<v>> : std::true_type {};

template<typename T>
struct is_fields_def<with_fields::from<T>> : std::true_type {};

template<>
struct is_fields_def<with_fields::empty> : std::true_type {};

template<>
struct is_fields_def<with_fields::inferred> : std::true_type {};

}

#endif