#pragma once

#include <vst.hpp>

#include <concepts>
#include <utility>
#include <iosfwd>

// ########################
// # append_template_args #
// ########################

template<typename T, typename... extra_args_t>
struct append_template_args;

template<typename T, typename... args_t, typename... extra_args_t>
struct append_template_args<vst::type<T, args_t...>, extra_args_t...>
{
    using type = vst::type<T, args_t..., extra_args_t...>;
};

// #############
// # stringify #
// #############

template<typename T>
std::string stringify(const T& o)
{
    std::ostringstream oss;
    oss << o;
    return oss.str();
}

// ##############
// # is_ordered #
// ##############

template<typename T, typename U, typename ENABLER = void>
constexpr bool is_ordered_impl = false;

template<typename T, typename U>
constexpr bool is_ordered_impl<
    T, U,
    std::void_t<
        decltype(std::declval<const T&>() <  std::declval<const U&>()),
        decltype(std::declval<const T&>() >  std::declval<const U&>()),
        decltype(std::declval<const T&>() <= std::declval<const U&>()),
        decltype(std::declval<const T&>() >= std::declval<const U&>()),

        decltype(std::declval<const U&>() <  std::declval<const T&>()),
        decltype(std::declval<const U&>() >  std::declval<const T&>()),
        decltype(std::declval<const U&>() <= std::declval<const T&>()),
        decltype(std::declval<const U&>() >= std::declval<const T&>())>>
= true;

template<typename T, typename U = T>
constexpr bool is_ordered = is_ordered_impl<T, U>;

// ##############
// # is_addable #
// ##############

template<typename T, typename U, typename ENABLER = void>
constexpr bool is_addable_impl = false;

template<typename T, typename U>
constexpr bool is_addable_impl<
    T,
    U,
    std::void_t<
        decltype(std::declval<T&>()      += std::declval<const U&>()),
        decltype(std::declval<T&>()      -= std::declval<const U&>()),
        decltype(std::declval<const T&>() + std::declval<const U&>()),
        decltype(std::declval<const T&>() - std::declval<const U&>()),

        decltype(std::declval<U&>()      += std::declval<const T&>()),
        decltype(std::declval<U&>()      -= std::declval<const T&>()),
        decltype(std::declval<const U&>() + std::declval<const T&>()),
        decltype(std::declval<const U&>() - std::declval<const T&>())
    >
>
= true;

template<typename T, typename U = T>
constexpr bool is_addable = is_addable_impl<T, U>;

// #################
// # is_streamable #
// #################

template<typename T, typename ENABLER = void>
constexpr bool is_streamable = false;

template<typename T>
constexpr bool is_streamable<
    T,
    std::void_t<
        decltype(std::declval<std::ostream&>() << std::declval<const T&>())>>
= true;

// ###############
// # is_hashable #
// ###############

template<typename T, typename U, typename ENABLER = void>
constexpr bool is_hashable_impl = false;

template<typename T, typename U>
constexpr bool is_hashable_impl<
    T, U,
    std::void_t<
        decltype(std::declval<vst::hash<T>>()(std::declval<U>()))
      , decltype(std::declval<std::hash<T>>()(std::declval<U>()))
      , decltype(std::declval<boost::hash<T>>()(std::declval<U>()))
    >
>
= true;

template<typename T, typename U = T>
constexpr bool is_hashable = is_hashable_impl<T, U>;