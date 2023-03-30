#pragma once

#include <named_type_ops_category.h>

#include "vst.hpp"
#include "type_list.h"

template<typename op_category_t, typename T>
constexpr bool is_transparent_with = false;

template<typename T>
constexpr bool is_transparent_with<transparent_ops_with<T>, T> = true;

template<typename op_category_t>
struct named_type_traits
{
    template<typename T>
    static constexpr bool is_transparent_with_v = is_transparent_with<op_category_t, T>;

    template<typename T>
    struct is_transparent_with_t : std::bool_constant<is_transparent_with_v<T>> {};
};

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

static_assert(std::is_same_v<transparent_type_traits<int>::transform_t<default_ops>, strict_ops>);
static_assert(std::is_same_v<transparent_type_traits<int>::transform_t<strict_ops>, strict_ops>);
static_assert(std::is_same_v<transparent_type_traits<int>::transform_t<transparent_ops>, transparent_ops_with<int>>);
static_assert(std::is_same_v<transparent_type_traits<int>::transform_t<transparent_ops_with<int>>, transparent_ops_with<int>>);

// clang-format off
static_assert( is_transparent_with<transparent_ops_with<int>, int>);
static_assert(!is_transparent_with<transparent_ops_with<int>, float>);
static_assert(!is_transparent_with<transparent_ops, int>);
static_assert(!is_transparent_with<default_ops, int>);
static_assert(!is_transparent_with<strict_ops, int>);

static_assert( named_type_traits<transparent_ops_with<int>>::is_transparent_with_v<int>);
static_assert(!named_type_traits<transparent_ops_with<int>>::is_transparent_with_v<float>);
static_assert(!named_type_traits<transparent_ops>::is_transparent_with_v<int>);
static_assert(!named_type_traits<default_ops>::is_transparent_with_v<int>);
static_assert(!named_type_traits<strict_ops>::is_transparent_with_v<int>);

static_assert( transparent_type_traits<int>  ::is_transparent_with_v<transparent_ops_with<int>>);
static_assert(!transparent_type_traits<float>::is_transparent_with_v<transparent_ops_with<int>>);
static_assert(!transparent_type_traits<int>  ::is_transparent_with_v<transparent_ops>);
static_assert(!transparent_type_traits<int>  ::is_transparent_with_v<default_ops>);
static_assert(!transparent_type_traits<int>  ::is_transparent_with_v<strict_ops>);

static_assert(!type_list_any_v<type_list<>,                                                       transparent_type_traits<int>::  is_transparent_with_t>);
static_assert( type_list_any_v<type_list<transparent_ops_with<int>>,                              transparent_type_traits<int>::  is_transparent_with_t>);
static_assert(!type_list_any_v<type_list<transparent_ops_with<int>>,                              transparent_type_traits<float>::is_transparent_with_t>);
static_assert( type_list_any_v<type_list<transparent_ops_with<int>, transparent_ops_with<float>>, transparent_type_traits<int>::  is_transparent_with_t>);
static_assert( type_list_any_v<type_list<transparent_ops_with<int>, transparent_ops_with<float>>, transparent_type_traits<float>::is_transparent_with_t>);
static_assert(!type_list_any_v<type_list<transparent_ops>,                                        transparent_type_traits<int>::  is_transparent_with_t>);
static_assert(!type_list_any_v<type_list<default_ops>,                                            transparent_type_traits<int>::  is_transparent_with_t>);
static_assert(!type_list_any_v<type_list<strict_ops>,                                             transparent_type_traits<int>::  is_transparent_with_t>);

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

// template<typename underlying_t, typename tag_t, typename ops_list_t, typename ENABLER = void>
// struct named_type_impl;

// template<typename underlying_t, typename tag_t>
// struct named_type_impl<
//     underlying_t, tag_t, type_list<>>
// {
//     // TODO MG: use default tag
//     using type = vst::type<named_type_pod<underlying_t, tag_t, strict_ops>>;
// };

// template<typename underlying_t, typename tag_t, typename ops_category, typename... ops>
// struct named_type_impl<
//     underlying_t, tag_t, type_list<ops_category, ops...>,
//     std::enable_if_t<type_list_contains_v<type_list<strict_ops, transparent_ops>, ops_category>>>
// {
//     using type = vst::type<named_type_pod<underlying_t, tag_t, ops_category>, ops...>;
// };

// template<typename underlying_t, typename tag_t, typename first_op, typename... ops>
// struct named_type_impl<
//     underlying_t, tag_t, type_list<first_op, ops...>,
//     std::enable_if_t<!type_list_contains_v<type_list<strict_ops, transparent_ops>, first_op>>>
// {
//     using type = vst::type<named_type_pod<underlying_t, tag_t, strict_ops>, first_op, ops...>;
// };

// template<typename underlying_t, typename tag_t, typename ops_list, typename ENABLER = void>
// using named_type_alias
// = vst::impl::type<named_type_pod<underlying_t, tag_t, strict_ops>, ops_list>;

// template<typename underlying_t, typename tag_t, typename ops_category, typename... ops>
// using named_type_alias<
//     underlying_t, tag_t, type_list<ops_category, ops...>,
//     std::enable_if_t<type_list_contains_v<type_list<strict_ops, transparent_ops>, ops_category>>>
// = vst::impl::type<named_type_pod<underlying_t, tag_t, ops_category>, type_list<ops...>>;

// template<typename underlying_t, typename tag_t, typename... ops>
// struct named_type_impl<
//     underlying_t, tag_t, type_list<ops...>>
// {
//     using type = vst::type<named_type_pod<underlying_t, tag_t, strict_ops>, ops...>;
// };

// #####################
// # get_op_categories #
// #####################

template<typename underlying_t, typename list_t, typename ENABLER = void>
struct get_op_categories;

template<typename underlying_t, typename list_t>
using get_op_categories_t = typename get_op_categories<underlying_t, list_t>::type;

// template<typename underlying_t, typename... args_t>
// struct get_op_categories<underlying, type_list<args_t>, std::enable_if_t<>>
// {
//     template<typename T>
//     using is_general = std::disjunction<std::is_same_v<T, strict_ops>, std::is_same_v<T, use_default>>;
//     static_assert(type_list_any_v<type_list<args_t...>, is_general> && type_list_len<type_list<args_t...>::value == 1, )
// }

// ################
// # ops_category #
// ################

template<typename underlying_t, typename list_t, typename ENABLER = void>
struct ops_category;

template<typename underlying_t, typename list_t>
using ops_category_t = typename ops_category<underlying_t, list_t>::type;

template<typename underlying_t, typename category_t, typename... ops>
struct ops_category<
    underlying_t,
    type_list<category_t, ops...>,
    std::enable_if_t<is_ops_category_v<category_t>>>
{
    using type = transform_op_category<underlying_t>::template func_t<category_t>;
    using filtered_list_type = type_list_filter_t<type_list<category_t, ops...>, is_ops_category>;
    using transformed_list_type = type_list_transform_t<filtered_list_type, transparent_type_traits<underlying_t>::template transform_t>;
};

template<typename underlying_t>
struct ops_category<underlying_t, type_list<>> : ops_category<underlying_t, type_list<default_ops>> {};
// not specified, no other ops => default_ops

template<typename underlying_t, typename first_op, typename... ops>
struct ops_category<
    underlying_t,
    type_list<first_op, ops...>,
    std::enable_if_t<!is_ops_category_v<first_op>> // not specified => default_ops
>
: ops_category<underlying_t, type_list<default_ops, first_op, ops...>> {};

static_assert(std::is_same_v<ops_category_t<int, type_list<>>,                                                    strict_ops>);
static_assert(std::is_same_v<ops_category_t<int, type_list<vst::op::ordered, vst::op::addable>>,                  strict_ops>);
static_assert(std::is_same_v<ops_category_t<int, type_list<default_ops>>,                                         strict_ops>);
static_assert(std::is_same_v<ops_category_t<int, type_list<default_ops, vst::op::ordered, vst::op::addable>>,     strict_ops>);
static_assert(std::is_same_v<ops_category_t<int, type_list<strict_ops>>,                                          strict_ops>);
static_assert(std::is_same_v<ops_category_t<int, type_list<strict_ops, vst::op::ordered, vst::op::addable>>,      strict_ops>);
static_assert(std::is_same_v<ops_category_t<int, type_list<transparent_ops>>,                                     transparent_ops_with<int>>);
static_assert(std::is_same_v<ops_category_t<int, type_list<transparent_ops, vst::op::ordered, vst::op::addable>>, transparent_ops_with<int>>);

static_assert(std::is_same_v<
    type_list_filter_t<
        type_list<strict_ops, transparent_ops, transparent_ops_with<float>, vst::op::ordered, vst::op::addable>,
        is_ops_category
    >,
    type_list<strict_ops, transparent_ops, transparent_ops_with<float>>
>);

static_assert(std::is_same_v<
    typename ops_category<
        int,
        type_list<strict_ops, transparent_ops, transparent_ops_with<float>, vst::op::ordered, vst::op::addable>
    >::filtered_list_type,
    type_list<strict_ops, transparent_ops, transparent_ops_with<float>>
>);

static_assert(std::is_same_v<
    type_list_transform_t<
        type_list<strict_ops, transparent_ops, transparent_ops_with<float>>,
        transparent_type_traits<int>::transform_t
    >,
    type_list<strict_ops, transparent_ops_with<int>, transparent_ops_with<float>>
>);

static_assert(std::is_same_v<
    typename ops_category<
        int,
        type_list<strict_ops, transparent_ops, transparent_ops_with<float>, vst::op::ordered, vst::op::addable>
    >::transformed_list_type,
    type_list<strict_ops, transparent_ops_with<int>, transparent_ops_with<float>>
>);

static_assert(std::is_same_v<
    typename ops_category<
        int,
        type_list<>
    >::transformed_list_type,
    type_list<strict_ops>
>);

static_assert(std::is_same_v<
    typename ops_category<
        int,
        type_list<vst::op::ordered, vst::op::addable>
    >::transformed_list_type,
    type_list<strict_ops>
>);

// ########################
// # without_ops_category #
// ########################

template<typename... ops>
struct without_ops_category : type_list_filter<type_list<ops...>, trait_op<is_ops_category>::negate> {};

template<typename... ops>
using without_ops_category_t = typename without_ops_category<ops...>::type;

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
        typename ops_category<
            underlying_t,
            type_list<ops...>
        >::transformed_list_type
    >,
    without_ops_category_t<ops...>>;

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

template<typename T, typename tag_t, typename... ops>
std::ostream& operator<<(std::ostream& os, const named_type<T, tag_t, ops...>& rhs)
{
    return os << rhs.get();
}

template<NamedType T>
struct transparent_equal_to
{
    using is_transparent = void;

    constexpr bool operator()(const T& lhs, const T& rhs) const
    {
        return lhs == rhs;
    }

    template<TransparentWith<T> U>
    constexpr bool operator()(const U& lhs, const T& rhs) const
    {
        return lhs == rhs;
    }

    template<TransparentWith<T> U>
    constexpr bool operator()(const T& lhs, const U& rhs) const
    {
        return lhs == rhs;
    }
};

template<NamedType T>
struct transparent_less
{
    using is_transparent = void;

    constexpr bool operator()(const T& lhs, const T& rhs) const
    {
        return lhs < rhs;
    }

    template<TransparentWith<T> U>
    constexpr bool operator()(const U& lhs, const T& rhs) const
    {
        return lhs < rhs;
    }

    template<TransparentWith<T> U>
    constexpr bool operator()(const T& lhs, const U& rhs) const
    {
        return lhs < rhs;
    }
};

namespace std
{
    template<typename underlying_t, typename tag_t, typename ops_category, typename... ops>
    struct equal_to<vst::type<named_type_pod<underlying_t, tag_t, ops_category>, ops...>>
    : transparent_equal_to<vst::type<named_type_pod<underlying_t, tag_t, ops_category>, ops...>> {};

    template<typename underlying_t, typename tag_t, typename ops_category, typename... ops>
    struct less<vst::type<named_type_pod<underlying_t, tag_t, ops_category>, ops...>>
    : transparent_less<vst::type<named_type_pod<underlying_t, tag_t, ops_category>, ops...>> {};
}

namespace vst
{
    template<typename T> requires is_named_type<T>
    struct hash<T>
    {
        constexpr size_t operator()(const T& o) const noexcept {
            return std::hash<typename T::underlying_type>{}(o.get());
        }

        template<typename U> requires T::template is_transparent_with<U>
        constexpr size_t operator()(const U& o) const noexcept {
            return std::hash<typename T::underlying_type>{}(o);
        }
    };
}

// complementary operators
template<typename T, typename U, std::enable_if_t<is_named_type<T> && T::template is_transparent_with<U>, int> = 0>
constexpr bool operator!=(const T& lhs, const U& rhs)
{
    return !(lhs == rhs);
}

template<typename T, typename U, std::enable_if_t<is_named_type<T> && T::template is_transparent_with<U>, int> = 0>
constexpr bool operator!=(const U& lhs, const T& rhs)
{
    return !(lhs == rhs);
}

template<typename T, typename U, std::enable_if_t<is_named_type<T> && T::template is_transparent_with<U>, int> = 0>
constexpr bool operator>(const T& lhs, const U& rhs)
{
    return !(lhs < rhs || lhs == rhs);
}

template<typename T, typename U, std::enable_if_t<is_named_type<T> && T::template is_transparent_with<U>, int> = 0>
constexpr bool operator>(const U& lhs, const T& rhs)
{
    return !(lhs < rhs || lhs == rhs);
}

template<typename T, typename U, std::enable_if_t<is_named_type<T> && T::template is_transparent_with<U>, int> = 0>
constexpr bool operator>=(const T& lhs, const U& rhs)
{
    return !(lhs < rhs);
}

template<typename T, typename U, std::enable_if_t<is_named_type<T> && T::template is_transparent_with<U>, int> = 0>
constexpr bool operator>=(const U& lhs, const T& rhs)
{
    return !(lhs < rhs);
}

template<typename T, typename U, std::enable_if_t<is_named_type<T> && T::template is_transparent_with<U>, int> = 0>
constexpr bool operator<=(const T& lhs, const U& rhs)
{
    return !(lhs > rhs);
}

template<typename T, typename U, std::enable_if_t<is_named_type<T> && T::template is_transparent_with<U>, int> = 0>
constexpr bool operator<=(const U& lhs, const T& rhs)
{
    return !(lhs > rhs);
}