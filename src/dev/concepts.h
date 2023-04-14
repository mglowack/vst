#pragma once

#include <concepts>
#include <functional>
#include <iosfwd>

// ###########
// # Addable #
// ###########

template<typename T>
concept ValueAddable = requires(const T& x) {
    { x + x } -> std::same_as<T>;
    { x - x } -> std::same_as<T>;
};

template<typename T>
concept AddAssignable = requires(T& lhs, const T& rhs) {
    { lhs += rhs } -> std::same_as<T&>;
    { lhs -= rhs } -> std::same_as<T&>;
};

template<typename T>
concept Addable = requires {
    requires ValueAddable<T>;
    requires AddAssignable<T>;
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