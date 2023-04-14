#pragma once

#include <concepts>
#include <functional>
#include <iosfwd>

// ###########
// # Addable #
// ###########

template<typename R, typename T, typename U>
concept ValueAddable = requires(const T& lhs, const U& rhs) {
    { lhs + rhs } -> std::same_as<R>;
    { lhs - rhs } -> std::same_as<R>;
};

template<typename R, typename T, typename U>
concept AddAssignable = requires(T& lhs, const U& rhs) {
    { lhs += rhs } -> std::same_as<R&>;
    { lhs -= rhs } -> std::same_as<R&>;
};

template<typename T>
concept Addable = requires {
    requires ValueAddable<T, T, T>;
    requires AddAssignable<T, T, T>;
};

template<typename T, typename U>
concept AddableWith = requires {
    requires ValueAddable<T, T, U>;
    requires ValueAddable<T, U, T>;
    requires AddAssignable<T, T, U>;
    requires AddAssignable<U, U, T>;
};

// ##############
// # Streamable #
// ##############

template<typename T>
concept Streamable = requires(std::ostream& os, const T& rhs) {
    { os << rhs } -> std::convertible_to<std::ostream&>;
};

// ############
// # Hashable #
// ############

template<typename T>
concept HashableX = requires(const T& x) {
    { std::hash<T>{}(x) } -> std::same_as<size_t>;
    // { boost::hash<T>{}(x) } -> std::same_as<size_t>;
};