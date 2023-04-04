#pragma once

#include <named_type_ops_category.h>

#include "vst.hpp"
#include "type_list.h"

// ##################
// # named_type_pod #
// ##################

template<typename underlying_t, typename tag_t, typename ops_categories_t>
struct named_type_pod
{
    underlying_t value;

    explicit constexpr named_type_pod(underlying_t value) : value(value) {}

    explicit constexpr operator const underlying_t&() const { return value; }
    explicit constexpr operator underlying_t&()             { return value; }

    constexpr const underlying_t& get()               const { return value; }
    constexpr underlying_t& get()                           { return value; }

    static constexpr auto get_fields()
    {
        return std::tuple{&named_type_pod<underlying_t, tag_t, ops_categories_t>::value};
    }
};

// ###############################
// # allows_transparent_ops_with #
// ###############################

template<typename T>
struct allows_transparent_ops_with
{
    template<typename ops_category_t>
    struct pred : std::false_type {};

    template<typename U>
    struct pred<transparent_ops_with<U>> : std::bool_constant<std::is_same_v<T, U>> {};
};

static_assert( type_list_any_v<type_list<transparent_ops_with<int>>, allows_transparent_ops_with<int>::pred>);
static_assert( type_list_any_v<type_list<transparent_ops_with<int>, transparent_ops_with<float>>, allows_transparent_ops_with<float>::pred>);
static_assert(!type_list_any_v<type_list<>, allows_transparent_ops_with<int>::pred>);
static_assert(!type_list_any_v<type_list<strict_ops>, allows_transparent_ops_with<int>::pred>);

// ####################
// # named_type_trait #
// ####################

template<typename T>
struct named_type_trait;

template<typename underlying_t, typename tag_t, typename op_categories_t, typename... ops>
struct named_type_trait<vst::type<named_type_pod<underlying_t, tag_t, op_categories_t>, ops...>>
{
    static constexpr bool exists = true;

    using underlying_type = underlying_t;
    using op_categories = op_categories_t;

    template<typename T>
    static constexpr bool is_transparent_with
        = type_list_any_v<op_categories, allows_transparent_ops_with<T>::pred>;

    static constexpr bool is_transparent = is_transparent_with<underlying_type>;
};

template<typename T>
concept NamedType = named_type_trait<T>::exists;

// ##############
// # named_type #
// ##############

template<typename params_list, typename underlying_t>
struct extract_op_categories
{
    using raw_op_categories_list = type_list_filter_t<params_list, is_ops_category>;
    using type = type_list_transform_t<raw_op_categories_list, transform_op_category<underlying_t>::template func_t>;
};

template<typename params_list, typename underlying_t>
using extract_op_categories_t = typename extract_op_categories<params_list, underlying_t>::type;

static_assert(std::is_same_v<type_list<>, extract_op_categories_t<type_list<>, int>>);
static_assert(std::is_same_v<type_list<strict_ops>, extract_op_categories_t<type_list<strict_ops>, int>>);
static_assert(std::is_same_v<type_list<strict_ops>, extract_op_categories_t<type_list<default_ops>, int>>);
static_assert(std::is_same_v<
    type_list<transparent_ops_with<int>>,
    extract_op_categories_t<type_list<transparent_ops>, int>>);
static_assert(std::is_same_v<
    type_list<transparent_ops_with<int>>,
    extract_op_categories_t<type_list<transparent_ops, vst::op::ordered, vst::op::hashable>, int>>);
static_assert(std::is_same_v<
    type_list<transparent_ops_with<int>, transparent_ops_with<float>>,
    extract_op_categories_t<type_list<transparent_ops, transparent_ops_with<float>, vst::op::ordered, vst::op::hashable>, int>>);

template<typename params_list>
struct filter_op_categories
: type_list_filter<params_list, trait_op<is_ops_category>::negate> {};

template<typename params_list>
using filter_op_categories_t = typename filter_op_categories<params_list>::type;

static_assert(std::is_same_v<
    type_list<vst::op::ordered, vst::op::hashable>,
    filter_op_categories_t<type_list<transparent_ops, transparent_ops_with<float>, vst::op::ordered, vst::op::hashable>>>);

// template<typename underlying_t, typename tag_t, typename... params>
// using named_type = vst::impl::type<
//     named_type_pod<
//         underlying_t,
//         tag_t,
//         extract_op_categories_t<type_list<params...>, underlying_t>
//     >,
//     filter_op_categories_t<params...>>>;
