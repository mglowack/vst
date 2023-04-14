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
concept Hashable = requires(const T& x) {
    { vst::hash<T>{}(x) } -> std::same_as<size_t>;
    { std::hash<T>{}(x) } -> std::same_as<size_t>;
    { boost::hash<T>{}(x) } -> std::same_as<size_t>;
};