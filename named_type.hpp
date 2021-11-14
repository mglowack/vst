#ifndef NAMED_TYPE_H
#define NAMED_TYPE_H

#include "vst.hpp"
#include "type_list.h"

// ##############
// # named_type #
// ##############

struct default_ops;
struct strict_ops;
struct transparent_ops;
template<typename T>
struct transparent_ops_with;

template<typename op_category_t, typename T>
constexpr bool is_transparent_with = false;

template<typename T>
constexpr bool is_transparent_with<transparent_ops_with<T>, T> = true;

template<typename op_category_t>
struct named_type_traits
{
    template<typename T>
    struct is_transparent_with_t : std::bool_constant<is_transparent_with<op_category_t, T>> {};
    
    template<typename T>
    static constexpr bool is_transparent_with_v = is_transparent_with_t<T>::value;
};

template<typename T>
struct transparent_type_traits
{
    template<typename op_category_t>
    struct is_transparent_with_t : std::bool_constant<is_transparent_with<op_category_t, T>> {};
    
    template<typename op_category_t>
    static constexpr bool is_transparent_with_v = is_transparent_with_t<op_category_t>::value;
};

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

static_assert( transparent_type_traits<int>::is_transparent_with_v<transparent_ops_with<int>>);
static_assert(!transparent_type_traits<float>::is_transparent_with_v<transparent_ops_with<int>>);
static_assert(!transparent_type_traits<int>::is_transparent_with_v<transparent_ops>);
static_assert(!transparent_type_traits<int>::is_transparent_with_v<default_ops>);
static_assert(!transparent_type_traits<int>::is_transparent_with_v<strict_ops>);

static_assert( type_list_any_v<type_list<transparent_ops_with<int>>, transparent_type_traits<int>::is_transparent_with_t>);
static_assert(!type_list_any_v<type_list<transparent_ops_with<int>>, transparent_type_traits<float>::is_transparent_with_t>);
static_assert( type_list_any_v<type_list<transparent_ops_with<int>, transparent_ops_with<float>>, transparent_type_traits<float>::is_transparent_with_t>);
static_assert(!type_list_any_v<type_list<transparent_ops>, transparent_type_traits<int>::is_transparent_with_t>);
static_assert(!type_list_any_v<type_list<default_ops>, transparent_type_traits<int>::is_transparent_with_t>);
static_assert(!type_list_any_v<type_list<strict_ops>, transparent_type_traits<int>::is_transparent_with_t>);

template<typename T>
struct is_ops_category : type_list_contains<type_list<default_ops, strict_ops, transparent_ops>, T> {};

template<typename T>
struct is_ops_category<transparent_ops_with<T>> : std::true_type {};

template<typename T>
constexpr bool is_ops_category_v = is_ops_category<T>::value;

static_assert( is_ops_category_v<default_ops>);
static_assert( is_ops_category_v<strict_ops>);
static_assert( is_ops_category_v<transparent_ops>);
static_assert( is_ops_category_v<transparent_ops_with<int>>);
static_assert(!is_ops_category_v<int>);
static_assert(!is_ops_category_v<struct foo>);

template<typename underlying_t, typename tag_t, typename ops_category_t>
struct named_type_pod
{
    using self = named_type_pod<underlying_t, tag_t, ops_category_t>;
    using underlying_type = underlying_t;
    using ops_category = ops_category_t;
    using ops_categories = type_list<ops_category_t>;

    template<typename T>
    // static constexpr bool is_transparent_with = type_list_any_v<ops_categories, transparent_type_traits<T>::is_transparent_with_t>;
    // static constexpr bool is_transparent_with = type_list_any_v<ops_categories, transparent_type_traits<T>::is_transparent_with_t>;
    static constexpr bool is_transparent_with = is_transparent_with<ops_category_t, T>;

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

template<typename underlying_t, typename... ops>
struct ops_category;

template<typename underlying_t, typename... ops>
using ops_category_t = typename ops_category<underlying_t, ops...>::type;

template<typename underlying_t, typename... ops>
struct ops_category
{
    using type = ops_category_t<underlying_t, default_ops, ops...>; // covers empty and not specified
};

template<typename underlying_t, typename category, typename... ops>
struct ops_category<underlying_t, category, ops...>
{
    using type = std::conditional_t<
        is_ops_category_v<category> && !std::is_same_v<category, default_ops>, 
        std::conditional_t<std::is_same_v<category, transparent_ops>,
            transparent_ops_with<underlying_t>,
            category>, 
        strict_ops>;
};

static_assert(std::is_same_v<ops_category_t<int>, strict_ops>);
static_assert(std::is_same_v<ops_category_t<int, strict_ops>, strict_ops>);
static_assert(std::is_same_v<ops_category_t<int, transparent_ops>, transparent_ops_with<int>>);
static_assert(std::is_same_v<ops_category_t<int, strict_ops, vst::op::ordered, vst::op::addable>, strict_ops>);
static_assert(std::is_same_v<ops_category_t<int, transparent_ops, vst::op::ordered, vst::op::addable>, transparent_ops_with<int>>);

template<typename... ops>
struct without_ops_category : type_list_filter<type_list<ops...>, trait_op<is_ops_category>::negate> {};

template<typename... ops>
using without_ops_category_t = typename without_ops_category<ops...>::type;

template<typename underlying_t, typename tag_t, typename... ops>
// using named_type = vst::type<named_type_pod<underlying_t, tag_t, strict_ops>, ops...>;
// using named_type = vst::type<named_type_pod<underlying_t, tag_t, transparent_ops>, ops...>;
// using named_type = typename named_type_impl<underlying_t, tag_t, type_list<ops...>>::type;
// using named_type = named_type_alias<underlying_t, tag_t, type_list<ops...>>;
using named_type = vst::impl::type<
    named_type_pod<underlying_t, tag_t, ops_category_t<underlying_t, ops...>>, 
    without_ops_category_t<ops...>>;

template<typename T>
constexpr bool is_named_type = false;

template<typename underlying_t, typename tag_t, typename ops_category, typename... ops>
constexpr bool is_named_type<vst::type<named_type_pod<underlying_t, tag_t, ops_category>, ops...>> = true;
// struct is_named_type<typename named_type_impl<underlying_t, tag_t, type_list<ops...>>::type> : std::true_type {};
// struct is_named_type<vst::impl::type<named_type<underlying_t, tag_t, ops_category>, type_list<ops...>>> : std::true_type {};

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

template<typename T, typename U, std::enable_if_t<is_named_type<T> && T::template is_transparent_with<U>, int> = 0>
constexpr bool operator==(const T& lhs, const U& rhs)
{
    return lhs.get() == rhs;
}

template<typename T, typename U, std::enable_if_t<is_named_type<T> && T::template is_transparent_with<U>, int> = 0>
constexpr bool operator==(const U& lhs, const T& rhs)
{
    return lhs == rhs.get();
}

template<typename T, typename U, std::enable_if_t<is_named_type<T> && T::template is_transparent_with<U>, int> = 0>
constexpr bool operator<(const T& lhs, const U& rhs)
{
    return lhs.get() < rhs;
}

template<typename T, typename U, std::enable_if_t<is_named_type<T> && T::template is_transparent_with<U>, int> = 0>
constexpr bool operator<(const U& lhs, const T& rhs)
{
    return lhs < rhs.get();
}

template<typename T, typename tag_t, typename... ops>
std::ostream& operator<<(std::ostream& os, const named_type<T, tag_t, ops...>& rhs)
{
    return os << rhs.get();
}

template<typename T, typename ENABLER = void>
struct transparent_equal_to 
{
    constexpr bool operator()( const T& lhs,  const T& rhs) const
    {
        return lhs == rhs;
    }
};

template<typename T>
struct transparent_equal_to<T, std::enable_if_t<is_named_type<T>>>
{
    using is_transparent = void;
    
    constexpr bool operator()(const T& lhs, const T& rhs) const
    {
        return lhs == rhs;
    }
    
    template<typename U, std::enable_if_t<T::template is_transparent_with<U>, int> = 0>
    constexpr bool operator()(const U& lhs, const T& rhs) const
    {
        return lhs == rhs;
    }
    
    template<typename U, std::enable_if_t<T::template is_transparent_with<U>, int> = 0>
    constexpr bool operator()(const T& lhs, const U& rhs) const
    {
        return lhs == rhs;
    }
};

template<typename T, typename ENABLER = void>
struct transparent_less
{
    constexpr bool operator()( const T& lhs,  const T& rhs) const
    {
        return lhs < rhs;
    }
};

template<typename T>
struct transparent_less<T, std::enable_if_t<is_named_type<T>>>
{
    using is_transparent = void;
    
    constexpr bool operator()(const T& lhs, const T& rhs) const
    {
        return lhs < rhs;
    }
    
    template<typename U, std::enable_if_t<T::template is_transparent_with<U>, int> = 0>
    constexpr bool operator()(const U& lhs, const T& rhs) const
    {
        return lhs < rhs;
    }
    
    template<typename U, std::enable_if_t<T::template is_transparent_with<U>, int> = 0>
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
    template<typename T>
    struct hash<T, std::enable_if_t<is_named_type<T>>>
    {
        size_t operator()(const T& o) const noexcept {
            return std::hash<typename T::underlying_type>{}(o.get());
        }

        template<typename U, std::enable_if_t<T::template is_transparent_with<U>, int> = 0>
        size_t operator()(const U& o) const noexcept {
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

#endif