#pragma once

#include <tuple>
#include <type_traits>
#include <ostream>

// ####################
// # apply_with_index #
// ####################

template <std::size_t I, typename T>
struct value_with_index
{
    static constexpr std::size_t index = I;
    T& value;
};

template <std::size_t I, typename T>
constexpr bool operator==(const value_with_index<I, T>& lhs, const value_with_index<I, T>& rhs)
{
    return lhs.value == rhs.value;
}

template <std::size_t I, std::size_t J, typename T, typename U>
constexpr bool operator==(const value_with_index<I, T>& lhs, const value_with_index<J, U>& rhs)
{
    return false;
}

template <std::size_t I, std::size_t J, typename T, typename U>
constexpr bool operator!=(const value_with_index<I, T>& lhs, const value_with_index<J, U>& rhs)
{
    return !(lhs == rhs);
}


template <std::size_t I, typename T>
std::ostream& operator<<(std::ostream& os, const value_with_index<I, T>& rhs)
{
    return os << "[ index = " << rhs.index << ", value=" << rhs.value << " ]";
}


// template<typename F, typename Tuple, std::size_t... I>
// constexpr decltype(auto) apply_with_index_impl(F&& f, Tuple&& tuple, std::index_sequence<I...>)
// {
//     return std::forward<F>(f)(
//         value_with_index<I, std::remove_reference_t<decltype(std::get<I>(tuple))>>{std::get<I>(tuple)}...);
// }

// template<typename F, typename Tuple>
// constexpr decltype(auto) apply_with_index(F&& f, Tuple&& tuple)
// {
//     return apply_with_index_impl(
//         std::forward<F>(f),
//         std::forward<Tuple>(tuple),
//         std::make_index_sequence<std::tuple_size_v<std::remove_reference_t<Tuple>>>{});
// }

// TODO MG: figure out if better and why does not work in constexpr even though: https://godbolt.org/z/rfrs5bnjf
// EDIT: seems to be working on clang 14 arm-64bit, probably didn't work on some different compiler

template<typename F, std::size_t... I, typename... args_t>
constexpr decltype(auto) apply_with_index_impl(F&& f, std::index_sequence<I...>, args_t&&... a)
{
    return std::forward<F>(f)(value_with_index<I, std::remove_reference_t<args_t>>{a}...);

    // does it change anything? has trouble accepting temporaries
    // return std::forward<F>(f)(
    //     value_with_index<I, std::remove_reference_t<args_t>>{std::forward<args_t>(a)}...);
}

template<typename F, typename Tuple>
constexpr decltype(auto) apply_with_index(F&& f, Tuple&& tuple)
{
    return std::apply([&f](auto&&... a) {
        return apply_with_index_impl(
            std::forward<F>(f),
            std::make_index_sequence<sizeof...(a)>{},
            std::forward<decltype(a)>(a)...);
    }, std::forward<Tuple>(tuple));
}