#ifndef VST_TEST_UTILS_H
#define VST_TEST_UTILS_H

#include "vst.hpp"

#include <utility>
#include <iosfwd>

// #################
// # is_comparable #
// #################

template<typename T, typename U, typename ENABLER = void>
constexpr bool is_comparable_impl = false;

template<typename T, typename U>
constexpr bool is_comparable_impl<
    T, U,
    std::void_t<
        decltype(std::declval<const T&>() == std::declval<const U&>()),
        decltype(std::declval<const T&>() != std::declval<const U&>()),

        decltype(std::declval<const U&>() == std::declval<const T&>()),
        decltype(std::declval<const U&>() != std::declval<const T&>())
    >
>
= true;

template<typename T, typename U = T>
constexpr bool is_comparable = is_comparable_impl<T, U>;

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
= is_comparable<T>;

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

template<typename T, typename ENABLER = void>
constexpr bool is_hashable = false;

template<typename T>
constexpr bool is_hashable<
    T, 
    std::void_t<
        decltype(std::declval<vst::hash<T>>()(std::declval<T>()))
      , decltype(std::declval<std::hash<T>>()(std::declval<T>()))
      , decltype(std::declval<boost::hash<T>>()(std::declval<T>()))
    >
>
= true;

#endif