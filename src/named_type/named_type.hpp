#pragma once

#include <named_type_pod.h>
#include <named_type_trait.h>
#include <named_type_ops_category.h>
#include <named_type_conversions_category.h>

#include "vst.hpp"
#include "type_list.h"

// ##############
// # named_type #
// ##############

template<typename underlying_t, typename tag_t, typename... ops>
using named_type = vst::impl::type<
    named_type_pod<
        underlying_t,
        tag_t,
        extract_op_categories_t<type_list<ops...>, underlying_t>,
        extract_conversion_categories_t<type_list<ops...>, underlying_t>
    >,
    filter_op_categories_t<type_list<ops...>>>;

// #############
// # operators #
// #############

template<NamedType T, TransparentWith<T> U>
constexpr bool operator==(const T& lhs, const U& rhs)
{
    return lhs.get() == rhs;
}

template<NamedType T, TransparentWith<T> U>
constexpr bool operator==(const U& lhs, const T& rhs)
{
    return lhs == rhs.get();
}

template<NamedType T, TransparentWith<T> U>
constexpr auto operator<=>(const T& lhs, const U& rhs)
{
    return lhs.get() <=> rhs;
}

template<NamedType T, TransparentWith<T> U>
constexpr auto operator<=>(const U& lhs, const T& rhs)
{
    return lhs <=> rhs.get();
}

template<NamedType T>
std::ostream& operator<<(std::ostream& os, const T& rhs)
{
    return os << rhs.get();
}

template<NamedType T, typename OP>
struct transparent_op
{
    using is_transparent = void;

    constexpr bool operator()(const T& lhs, const T& rhs) const
    {
        return OP{}(lhs, rhs);
    }

    template<TransparentWith<T> U>
    constexpr bool operator()(const U& lhs, const T& rhs) const
    {
        return OP{}(lhs, rhs);
    }

    template<TransparentWith<T> U>
    constexpr bool operator()(const T& lhs, const U& rhs) const
    {
        return OP{}(lhs, rhs);
    }
};

namespace std
{
    template<NamedType T>
    struct equal_to<T> : transparent_op<T, equal_to<>> {};

    template<NamedType T>
    struct less<T> : transparent_op<T, less<>> {};
}

namespace vst
{
    template<NamedType T>
    struct hash<T>
    {
        using underlying_t = typename named_type_trait<T>::underlying_type;

        constexpr size_t operator()(const T& o) const noexcept {
            return std::hash<underlying_t>{}(o.get());
        }

        template<TransparentWith<T> U>
        constexpr size_t operator()(const U& o) const noexcept {
            return std::hash<underlying_t>{}(o);
        }
    };
}