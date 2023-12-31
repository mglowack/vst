#pragma once

#include <boost/functional/hash.hpp>

#include <string>

namespace vst {

// #################
// # wrapped_value #
// #################

template<typename T>
struct wrapped_value
{
    const T& value;

    constexpr explicit wrapped_value(const T& value)
    : value(value) {}
};

template<typename P, typename T>
struct wrapped_value_of : wrapped_value<T>
{
    using wrapped_value<T>::wrapped_value;
};

template<typename vst_t, typename T>
constexpr auto wrap(const T& v)
{
    return wrapped_value_of<vst_t, T>{v};
}

template<typename vst_t, typename... Ts>
constexpr auto wrap(std::tuple<Ts...> values)
{
    return std::apply(
        [](const auto&... elem) { return std::tuple(wrap<vst_t>(elem)...); },
        values);
}

// defaults
template<typename T>
constexpr bool operator==(const wrapped_value<T>& lhs, const wrapped_value<T>& rhs)
{
    return lhs.value == rhs.value;
}

template<typename T>
constexpr bool operator!=(const wrapped_value<T>& lhs, const wrapped_value<T>& rhs)
{
    return !(lhs == rhs);
}

template<typename T>
constexpr bool operator<(const wrapped_value<T>& lhs, const wrapped_value<T>& rhs)
{
    return lhs.value < rhs.value;
}

template<typename T>
constexpr bool operator<=(const wrapped_value<T>& lhs, const wrapped_value<T>& rhs)
{
    return lhs < rhs || lhs == rhs;
}

template<typename T>
constexpr bool operator>(const wrapped_value<T>& lhs, const wrapped_value<T>& rhs)
{
    return !(lhs <= rhs);
}

template<typename T>
constexpr bool operator>=(const wrapped_value<T>& lhs, const wrapped_value<T>& rhs)
{
    return !(lhs < rhs);
}

template<typename T>
constexpr T operator+(const wrapped_value<T>& lhs, const wrapped_value<T>& rhs)
{
    return lhs.value + rhs.value;
}

template <typename T>
std::size_t hash_value(const wrapped_value<T>& v)
{
    using boost::hash_value;
    return hash_value(v.value);
}

template<typename T>
std::ostream& operator<<(std::ostream& os, const wrapped_value<T>& rhs)
{
    return os << rhs.value;
}

// overrides - const char*
inline
bool operator==(const wrapped_value<const char*>& lhs, const wrapped_value<const char*>& rhs)
{
    return strcmp(lhs.value, rhs.value) == 0;
}

inline
bool operator<(const wrapped_value<const char*>& lhs, const wrapped_value<const char*>& rhs)
{
    return strcmp(lhs.value, rhs.value) < 0;
}

}