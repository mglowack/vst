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

template<typename op_category_t, typename T, typename ENABLER = void>
constexpr bool is_transparent_with = false;

template<typename T>
constexpr bool is_transparent_with<transparent_ops_with<T>, T> = true;

static_assert( is_transparent_with<transparent_ops_with<int>, int>);
static_assert(!is_transparent_with<transparent_ops_with<int>, float>);
static_assert(!is_transparent_with<transparent_ops, int>);
static_assert(!is_transparent_with<default_ops, int>);
static_assert(!is_transparent_with<strict_ops, int>);

template<typename T, typename ENABLER = void>
constexpr bool is_ops_category = type_list_contains_v<type_list<default_ops, strict_ops, transparent_ops>, T>;

template<typename T>
constexpr bool is_ops_category<transparent_ops_with<T>> = true;

static_assert( is_ops_category<default_ops>);
static_assert( is_ops_category<strict_ops>);
static_assert( is_ops_category<transparent_ops>);
static_assert( is_ops_category<transparent_ops_with<int>>);
static_assert(!is_ops_category<int>);
static_assert(!is_ops_category<struct foo>);

template<typename underlying_t, typename tag_t, typename ops_category>
struct named_type_pod
{
    using self = named_type_pod<underlying_t, tag_t, ops_category>;
    using underlying_type = underlying_t;

    template<typename T>
    static constexpr bool is_transparent_withX = is_transparent_with<ops_category, T>;

    static constexpr bool is_transparent = is_transparent_withX<underlying_type>;

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
        is_ops_category<category> && !std::is_same_v<category, default_ops>, 
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
struct without_ops_category
{
    using type = type_list<ops...>; // covers empty
};

template<typename category, typename... ops>
struct without_ops_category<category, ops...>
{
    using type = std::conditional_t<is_ops_category<category>, type_list<ops...>, type_list<category, ops...>>;
};

template<typename... ops>
using without_ops_category_t = typename without_ops_category<ops...>::type;

static_assert(std::is_same_v<without_ops_category_t<>, type_list<>>);
static_assert(std::is_same_v<without_ops_category_t<strict_ops>, type_list<>>);
static_assert(std::is_same_v<without_ops_category_t<transparent_ops>, type_list<>>);
static_assert(std::is_same_v<without_ops_category_t<strict_ops, vst::op::ordered, vst::op::addable>, type_list<vst::op::ordered, vst::op::addable>>);
static_assert(std::is_same_v<without_ops_category_t<transparent_ops, vst::op::ordered, vst::op::addable>, type_list<vst::op::ordered, vst::op::addable>>);

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

template<typename T, std::enable_if_t<is_named_type<T> && T::is_transparent, int> = 0>
constexpr bool operator==(const T& lhs, const typename T::underlying_type& rhs)
{
    return lhs.get() == rhs;
}

template<typename T, std::enable_if_t<is_named_type<T> && T::is_transparent, int> = 0>
constexpr bool operator==(const typename T::underlying_type& lhs, const T& rhs)
{
    return lhs == rhs.get();
}

template<typename T, std::enable_if_t<is_named_type<T> && T::is_transparent, int> = 0>
constexpr bool operator<(const  T& lhs, const typename T::underlying_type& rhs)
{
    return lhs.get() < rhs;
}

template<typename T, std::enable_if_t<is_named_type<T> && T::is_transparent, int> = 0>
constexpr bool operator<(const typename T::underlying_type& lhs, const T& rhs)
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
struct transparent_equal_to<T, std::enable_if_t<T::is_transparent>>
{
    using is_transparent = void;
    
    constexpr bool operator()(const T& lhs, const T& rhs) const
    {
        return lhs == rhs;
    }
    
    constexpr bool operator()(const typename T::underlying_type& lhs, const T& rhs) const
    {
        return lhs == rhs;
    }
    
    constexpr bool operator()(const T& lhs, const typename T::underlying_type& rhs) const
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
struct transparent_less<T, std::enable_if_t<T::is_transparent>>
{
    using is_transparent = void;
    
    constexpr bool operator()(const T& lhs, const T& rhs) const
    {
        return lhs < rhs;
    }
    
    constexpr bool operator()(const typename T::underlying_type& lhs, const T& rhs) const
    {
        return lhs < rhs;
    }
    
    constexpr bool operator()(const T& lhs, const typename T::underlying_type& rhs) const
    {
        return lhs < rhs;
    }
};

namespace std {
    template<typename underlying_t, typename tag_t, typename ops_category, typename... ops>
    struct equal_to<vst::type<named_type_pod<underlying_t, tag_t, ops_category>, ops...>>
    : transparent_equal_to<vst::type<named_type_pod<underlying_t, tag_t, ops_category>, ops...>> {};

    template<typename underlying_t, typename tag_t, typename ops_category, typename... ops>
    struct less<vst::type<named_type_pod<underlying_t, tag_t, ops_category>, ops...>>
    : transparent_less<vst::type<named_type_pod<underlying_t, tag_t, ops_category>, ops...>> {};
}

namespace vst {
    
    template<typename T>
    struct hash<T, std::enable_if_t<is_named_type<T> && T::is_transparent>>
    {
        size_t operator()(const T& o) const noexcept {
            return (*this)(o.get());
        }
        size_t operator()(const typename T::underlying_type& o) const noexcept {
            return std::hash<typename T::underlying_type>{}(o);
        }
    };
}

// complementary operators
template<typename T, std::enable_if_t<is_named_type<T> && T::is_transparent, int> = 0>
constexpr bool operator!=(const T& lhs, const typename T::underlying_type& rhs)
{
    return !(lhs == rhs);
}

template<typename T, std::enable_if_t<is_named_type<T> && T::is_transparent, int> = 0>
constexpr bool operator!=(const typename T::underlying_type& lhs, const T& rhs)
{
    return !(lhs == rhs);
}

template<typename T, std::enable_if_t<is_named_type<T> && T::is_transparent, int> = 0>
constexpr bool operator>(const T& lhs, const typename T::underlying_type& rhs)
{
    return !(lhs < rhs || lhs == rhs);
}

template<typename T, std::enable_if_t<is_named_type<T> && T::is_transparent, int> = 0>
constexpr bool operator>(const typename T::underlying_type& lhs, const T& rhs)
{
    return !(lhs < rhs || lhs == rhs);
}

template<typename T, std::enable_if_t<is_named_type<T> && T::is_transparent, int> = 0>
constexpr bool operator>=(const T& lhs, const typename T::underlying_type& rhs)
{
    return !(lhs < rhs);
}

template<typename T, std::enable_if_t<is_named_type<T> && T::is_transparent, int> = 0>
constexpr bool operator>=(const typename T::underlying_type& lhs, const T& rhs)
{
    return !(lhs < rhs);
}

template<typename T, std::enable_if_t<is_named_type<T> && T::is_transparent, int> = 0>
constexpr bool operator<=(const T& lhs, const typename T::underlying_type& rhs)
{
    return !(lhs > rhs);
}

template<typename T, std::enable_if_t<is_named_type<T> && T::is_transparent, int> = 0>
constexpr bool operator<=(const typename T::underlying_type& lhs, const T& rhs)
{
    return !(lhs > rhs);
}

#endif