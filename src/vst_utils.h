#ifndef VST_UTILS_H
#define VST_UTILS_H

#include "vst_defs.h"

#include <type_traits>

namespace std {
template<typename T>
struct type_identity
{
    using type = T;
};
}

namespace vst {

// #################
// # is_fields_def #
// #################

template<typename T>  constexpr bool is_fields_def = false;
template<auto (*f)()> constexpr bool is_fields_def<with_fields::from_func<f>> = true;
template<auto v>      constexpr bool is_fields_def<with_fields::from_var<v>> = true;
template<typename T>  constexpr bool is_fields_def<with_fields::from<T>> = true;
// template<>            constexpr bool is_fields_def<with_fields::from_aggregate> = true;
template<>            constexpr bool is_fields_def<with_fields::empty> = true;

// #############
// # aggregate #
// #############

template<typename T>
struct aggregate : std::type_identity<T> {};

template<typename T, typename... ops>
struct aggregate<type<T, ops...>> : std::type_identity<T> {};

template<typename T>
using aggregate_t = typename aggregate<T>::type;

// ###################
// # propagate_const #
// ###################

template<typename T, typename U>
struct propagate_const : std::type_identity<U> {};

template<typename T, typename U>
struct propagate_const<const T, U> : std::type_identity<const U> {};

template<typename T, typename U>
using propagate_const_t = typename propagate_const<T, U>::type;

// ####################
// # apply_with_index #
// ####################

template <std::size_t I, typename T>
struct value_with_index
{
    static constexpr std::size_t index = I;
    T& value;
};

template<typename F, typename Tuple, std::size_t... I>
constexpr decltype(auto) apply_with_index_impl(F&& f, Tuple&& tuple, std::index_sequence<I...>)
{
    return std::forward<F>(f)(
        value_with_index<I, decltype(std::get<I>(tuple))>{std::get<I>(tuple)}...);
}

template<typename F, typename Tuple>
constexpr decltype(auto) apply_with_index(F&& f, Tuple&& tuple)
{
    return apply_with_index_impl(
        std::forward<F>(f), 
        std::forward<Tuple>(tuple), 
        std::make_index_sequence<std::tuple_size_v<std::remove_reference_t<Tuple>>>{});
}

// TODO MG: figure out if better and why does not work in constexpr even though: https://godbolt.org/z/rfrs5bnjf

// template<typename F, std::size_t... I, typename... args_t>
// constexpr decltype(auto) apply_with_index_impl(F&& f, std::index_sequence<I...>, args_t&&... a)
// {
//     return std::forward<F>(f)(
//         value_with_index<I, decltype(a)>{std::forward<decltype(a)>(a)}...);
// }

// template<typename F, typename Tuple>
// constexpr decltype(auto) apply_with_index(F&& f, Tuple&& tuple)
// {
//     return std::apply([f = std::forward<F>(f)](auto&&... a) {
//         return apply_with_index_impl(
//             std::move(f), 
//             std::make_index_sequence<sizeof...(a)>{},
//             std::forward<decltype(a)>(a)...);
//     }, std::forward<Tuple>(tuple));
// }

} // namespace vst

#endif