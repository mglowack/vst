#ifndef NAMED_TYPE_H
#define NAMED_TYPE_H

#include "vst.hpp"

// ##############
// # named_type #
// ##############

struct strict_ops;
struct transparent_ops;

template<typename underlying_t, typename tag_t, typename ops_category>
struct named_type_pod
{
    static constexpr bool is_transparent = std::is_same_v<ops_category, transparent_ops>;

    using self = named_type_pod<underlying_t, tag_t, ops_category>;
    underlying_t value;

    explicit constexpr named_type_pod(underlying_t value) : value(value) {}

    explicit constexpr operator const underlying_t&() const { return value; }
    explicit constexpr operator underlying_t&() { return value; }

    constexpr const underlying_t& get() const { return value; }
    constexpr underlying_t& get() { return value; }

    static constexpr auto get_fields()
    { 
        return std::tuple{&self::value};
    }
};

template<typename underlying_t, typename tag_t, typename... ops>
using named_type = vst::type<named_type_pod<underlying_t, tag_t, transparent_ops>, ops...>;

// template<typename underlying_t, typename tag_t, typename props_t>
// struct named_type_pod
// {
//     using props = props_t;
//     using self = named_type_pod<underlying_t, tag_t, props_t>;
//     underlying_t value;

//     explicit constexpr named_type_pod(underlying_t value) : value(value) {}

//     explicit constexpr operator const underlying_t&() const { return value; }
//     explicit constexpr operator underlying_t&() { return value; }

//     constexpr const underlying_t& get() const { return value; }
//     constexpr underlying_t& get() { return value; }

//     static constexpr auto get_fields()
//     { 
//         return std::tuple{&self::value};
//     }
// };

// template<typename underlying_t, typename tag_t, typename props_t typename... ops>
// using named_type = vst::type<named_type_pod<underlying_t, tag_t, props_t>, ops...>;

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

template<typename T, typename ENABLER = void>
struct transparent_equal_to : std::equal_to<T> {};

template<typename underlying_t, typename tag_t, typename... ops>
struct transparent_equal_to<
    named_type<underlying_t, tag_t, ops...>, 
    std::enable_if_t<named_type<underlying_t, tag_t, ops...>::is_transparent>>
{
    using is_transparent = void;
    
    constexpr bool operator()(
        const named_type<underlying_t, tag_t, ops...>& lhs, 
        const named_type<underlying_t, tag_t, ops...>& rhs) const
    {
        return lhs == rhs;
    }

    constexpr bool operator()(
        const named_type<underlying_t, tag_t, ops...>& lhs, 
        const underlying_t& rhs) const
    {
        return lhs == rhs;
    }

    constexpr bool operator()(
        const underlying_t& lhs, 
        const named_type<underlying_t, tag_t, ops...>& rhs) const
    {
        return lhs == rhs;
    }
};

template<typename T, typename ENABLER = void>
struct transparent_less : std::less<T> {};

template<typename underlying_t, typename tag_t, typename... ops>
struct transparent_less<
    named_type<underlying_t, tag_t, ops...>, 
    std::enable_if_t<named_type<underlying_t, tag_t, ops...>::is_transparent>>
{
    using is_transparent = void;

    constexpr bool operator()(
        const named_type<underlying_t, tag_t, ops...>& lhs, 
        const named_type<underlying_t, tag_t, ops...>& rhs) const
    {
        return lhs < rhs;
    }

    constexpr bool operator()(
        const named_type<underlying_t, tag_t, ops...>& lhs, 
        const underlying_t& rhs) const
    {
        return lhs < rhs;
    }
    
    constexpr bool operator()(
        const underlying_t& lhs, 
        const named_type<underlying_t, tag_t, ops...>& rhs) const
    {
        return lhs < rhs;
    }
};

namespace std {
    template<typename underlying_t, typename tag_t, typename... ops>
    struct equal_to<named_type<underlying_t, tag_t, ops...>>
    : transparent_equal_to<named_type<underlying_t, tag_t, ops...>> {};

    template<typename underlying_t, typename tag_t, typename... ops>
    struct less<named_type<underlying_t, tag_t, ops...>>
    : transparent_less<named_type<underlying_t, tag_t, ops...>> {};
}

namespace vst {
    template<typename underlying_t, typename tag_t, typename... ops>
    struct hash<
        named_type<underlying_t, tag_t, ops...>, 
        std::enable_if_t<named_type<underlying_t, tag_t, ops...>::is_transparent>>
    {
        size_t operator()(const named_type<underlying_t, tag_t, ops...>& o) const noexcept {
            return (*this)(o.get());
        }
        size_t operator()(const underlying_t& o) const noexcept {
            return std::hash<underlying_t>{}(o);
        }
    };
}

// complementary operators
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

template<typename T, typename tag_t, typename... ops>
constexpr bool operator>(const named_type<T, tag_t, ops...>& lhs, const T& rhs)
{
    return !(lhs < rhs || lhs == rhs);
}

template<typename T, typename tag_t, typename... ops>
constexpr bool operator>(const T& lhs, const named_type<T, tag_t, ops...>& rhs)
{
    return !(lhs < rhs || lhs == rhs);
}

template<typename T, typename tag_t, typename... ops>
constexpr bool operator>=(const named_type<T, tag_t, ops...>& lhs, const T& rhs)
{
    return !(lhs < rhs);
}

template<typename T, typename tag_t, typename... ops>
constexpr bool operator>=(const T& lhs, const named_type<T, tag_t, ops...>& rhs)
{
    return !(lhs < rhs);
}

template<typename T, typename tag_t, typename... ops>
constexpr bool operator<=(const named_type<T, tag_t, ops...>& lhs, const T& rhs)
{
    return !(lhs > rhs);
}

template<typename T, typename tag_t, typename... ops>
constexpr bool operator<=(const T& lhs, const named_type<T, tag_t, ops...>& rhs)
{
    return !(lhs > rhs);
}

#endif