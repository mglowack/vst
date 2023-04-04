#pragma once

#include <type_list.h>

#include <functional>

// struct default_conversions;
// struct explicit_conversions;
// struct implicit_conversions;
// template<typename T>
// struct implicit_conversions_with;

template<typename T>
struct implicit_conversions_from;
template<typename T>
struct implicit_conversions_to;

// ##########################
// # is_conversion_category #
// ##########################

template<typename T>
// struct is_conversion_category : type_list_contains<type_list<default_conversion, strict_conversion, transparent_conversion>, T> {};
struct is_conversion_category : std::false_type {};

template<typename T>
struct is_conversion_category<implicit_conversions_from<T>> : std::true_type {};

template<typename T>
struct is_conversion_category<implicit_conversions_to<T>> : std::true_type {};

template<typename T>
constexpr bool is_conversion_category_v = is_conversion_category<T>::value;

static_assert(!is_conversion_category_v<int>);
static_assert(!is_conversion_category_v<float>);
static_assert( is_conversion_category_v<implicit_conversions_from<int>>);
static_assert( is_conversion_category_v<implicit_conversions_from<float>>);
static_assert( is_conversion_category_v<implicit_conversions_to<int>>);
static_assert( is_conversion_category_v<implicit_conversions_to<float>>);

// #################################
// # extract_conversion_categories #
// #################################

template<typename params_list, typename underlying_t>
struct extract_conversion_categories
{
    using raw_categories_list = type_list_filter_t<params_list, is_conversion_category>;
    using categories_list = raw_categories_list;
    // using op_categories_list = std::conditional_t<
        // std::is_same_v<type_list<>, raw_op_categories_list>,
        // type_list<default_ops>, // insert 'default_ops' no other op categories are specified
        // raw_op_categories_list>;
    // using type = type_list_transform_t<categories_list, transform_op_category<underlying_t>::template func_t>;
    using type = categories_list;
};

template<typename params_list, typename underlying_t>
using extract_conversion_categories_t = typename extract_conversion_categories<params_list, underlying_t>::type;

static_assert(std::is_same_v<
    type_list<>,
    extract_conversion_categories_t<
        type_list<>,
        int
    >
>);

static_assert(std::is_same_v<
    type_list<>,
    extract_conversion_categories_t<
        type_list<double, int, float>,
        int
    >
>);

static_assert(std::is_same_v<
    type_list<implicit_conversions_from<int>, implicit_conversions_to<float>>,
    extract_conversion_categories_t<
        type_list<double, implicit_conversions_from<int>, implicit_conversions_to<float>, int, float>,
        int
    >
>);
