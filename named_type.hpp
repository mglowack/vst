#ifndef NAMED_TYPE_H
#define NAMED_TYPE_H

#include "vst.hpp"

// ##############
// # named_type #
// ##############

template<typename underlying_t, typename tag_t>
struct named_type_pod
{
    using self = named_type_pod<underlying_t, tag_t>;
    underlying_t value;

    explicit constexpr named_type_pod(underlying_t value) : value(value) {}

    explicit constexpr operator const underlying_t&() const { return value; }
    explicit constexpr operator underlying_t&() { return value; }

    constexpr const underlying_t& get() const { return value; }
    constexpr underlying_t& get() { return value; }

    static constexpr auto get_fields()
    { 
        return std::tuple{MEMBER(self, value)};
    }
};

template<typename underlying_t, typename tag_t, typename... ops>
using named_type = vst::type<named_type_pod<underlying_t, tag_t>, ops...>;

template<typename T, typename tag_t, typename... ops>
constexpr bool operator==(const named_type<T, tag_t, ops...>& lhs, const T& rhs)
{
    return lhs.get() == rhs;
}

template<typename T, typename tag_t, typename... ops>
constexpr bool operator==(const T& lhs, const named_type<T, tag_t, ops...>& rhs)
{
    return lhs == rhs.get();
}

template<typename T, typename tag_t, typename... ops>
constexpr bool operator<(const  named_type<T, tag_t, ops...>& lhs, const T& rhs)
{
    return lhs.get() < rhs;
}

template<typename T, typename tag_t, typename... ops>
constexpr bool operator<(const T& lhs, const named_type<T, tag_t, ops...>& rhs)
{
    return lhs < rhs.get();
}

template<typename T, typename tag_t, typename... ops>
std::ostream& operator<<(std::ostream& os, const named_type<T, tag_t, ops...>& rhs)
{
    return os << rhs.get();
}

// complementary ops, because ADL sucks
template<typename T, typename tag_t, typename... ops>
constexpr bool operator!=(const named_type<T, tag_t, ops...>& lhs, const T& rhs)
{
    return !(lhs == rhs);
}

template<typename T, typename tag_t, typename... ops>
constexpr bool operator!=(const T& lhs, const named_type<T, tag_t, ops...>& rhs)
{
    return !(lhs == rhs);
}

#endif