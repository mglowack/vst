#pragma once

#include <named_type_ops_category.h>
// #include <named_type_pod.h>
// #include <named_type_trait.h>
// #include <named_type_defs.h>

#include "vst.hpp"
#include "type_list.h"

template<typename op_category_t, typename T>
constexpr bool is_transparent_with = false;

template<typename T>
constexpr bool is_transparent_with<transparent_ops_with<T>, T> = true;

template<typename T>
struct transparent_type_traits
{
    template<typename op_category_t>
    static constexpr bool is_transparent_with_v = is_transparent_with<op_category_t, T>;

    template<typename op_category_t>
    struct is_transparent_with_t : std::bool_constant<is_transparent_with_v<op_category_t>> {};

    template<typename op_category_t>
    struct transform : transform_op_category<T>::func<op_category_t> {};

    template<typename op_category_t>
    using transform_t = typename transform<op_category_t>::type;
};

// clang-format on

template<typename underlying_t, typename tag_t, typename ops_categories_t>
struct named_type_pod
{
    using self = named_type_pod<underlying_t, tag_t, ops_categories_t>;
    using underlying_type = underlying_t;
    using ops_categories = ops_categories_t;

    template<typename T>
    static constexpr bool is_transparent_with = type_list_any_v<ops_categories, transparent_type_traits<T>::template is_transparent_with_t>;

    static constexpr bool is_transparent = self::is_transparent_with<underlying_type>;

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

// ##############
// # named_type #
// ##############

template<typename underlying_t, typename tag_t, typename... ops>
// using named_type = vst::type<named_type_pod<underlying_t, tag_t, strict_ops>, ops...>;
// using named_type = vst::type<named_type_pod<underlying_t, tag_t, transparent_ops>, ops...>;
// using named_type = typename named_type_impl<underlying_t, tag_t, type_list<ops...>>::type;
// using named_type = named_type_alias<underlying_t, tag_t, type_list<ops...>>;
using named_type = vst::impl::type<
    named_type_pod<
        underlying_t,
        tag_t,
        extract_op_categories_t<type_list<ops...>, underlying_t>
    >,
    filter_op_categories_t<type_list<ops...>>>;

template<typename T>
constexpr bool is_named_type = false;

template<typename underlying_t, typename tag_t, typename ops_category, typename... ops>
constexpr bool is_named_type<vst::type<named_type_pod<underlying_t, tag_t, ops_category>, ops...>> = true;
// struct is_named_type<typename named_type_impl<underlying_t, tag_t, type_list<ops...>>::type> : std::true_type {};
// struct is_named_type<vst::impl::type<named_type<underlying_t, tag_t, ops_category>, type_list<ops...>>> : std::true_type {};

template<typename T>
concept NamedType = is_named_type<T>;

template<typename U, typename T>
concept TransparentWith = is_named_type<T> && T::template is_transparent_with<U>;

// template<typename U, typename T>
// concept TransparentWith = is_named_type<T> && named_type_trait<T>::template is_transparent_with<U>;

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
constexpr bool operator<(const T& lhs, const U& rhs)
{
    return lhs.get() < rhs;
}

template<NamedType T, TransparentWith<T> U>
constexpr bool operator<(const U& lhs, const T& rhs)
{
    return lhs < rhs.get();
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
        constexpr size_t operator()(const T& o) const noexcept {
            return std::hash<typename T::underlying_type>{}(o.get());
        }

        template<TransparentWith<T> U>
        constexpr size_t operator()(const U& o) const noexcept {
            return std::hash<typename T::underlying_type>{}(o);
        }
    };
}

// complementary operators
template<NamedType T, TransparentWith<T> U>
constexpr bool operator!=(const T& lhs, const U& rhs)
{
    return !(lhs == rhs);
}

template<NamedType T, TransparentWith<T> U>
constexpr bool operator!=(const U& lhs, const T& rhs)
{
    return !(lhs == rhs);
}

template<NamedType T, TransparentWith<T> U>
constexpr bool operator>(const T& lhs, const U& rhs)
{
    return !(lhs < rhs || lhs == rhs);
}

template<NamedType T, TransparentWith<T> U>
constexpr bool operator>(const U& lhs, const T& rhs)
{
    return !(lhs < rhs || lhs == rhs);
}

template<NamedType T, TransparentWith<T> U>
constexpr bool operator>=(const T& lhs, const U& rhs)
{
    return !(lhs < rhs);
}

template<NamedType T, TransparentWith<T> U>
constexpr bool operator>=(const U& lhs, const T& rhs)
{
    return !(lhs < rhs);
}

template<NamedType T, TransparentWith<T> U>
constexpr bool operator<=(const T& lhs, const U& rhs)
{
    return !(lhs > rhs);
}

template<NamedType T, TransparentWith<T> U>
constexpr bool operator<=(const U& lhs, const T& rhs)
{
    return !(lhs > rhs);
}