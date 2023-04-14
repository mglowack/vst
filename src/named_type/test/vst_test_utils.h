#pragma once

#include <vst.hpp>

#include <utility>
#include <iosfwd>

// #################
// # is_comparable #
// #################

template<typename T, typename U, typename ENABLER = void>
constexpr bool is_comparable_with = false;

template<typename T, typename U>
constexpr bool is_comparable_with<
    T, U,
    std::void_t<
        decltype(std::declval<const T&>() == std::declval<const U&>()),
        decltype(std::declval<const T&>() != std::declval<const U&>()),

        decltype(std::declval<const U&>() == std::declval<const T&>()),
        decltype(std::declval<const U&>() != std::declval<const T&>())
    >
>
= true;

template<typename T>
constexpr bool is_comparable = is_comparable_with<T, T>;

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