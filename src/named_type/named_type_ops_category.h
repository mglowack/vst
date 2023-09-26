#pragma once

#include "vst.hpp"

#include <dev_type_list.h>

#include <type_traits>

// #######################
// # operator categories #
// #######################

struct default_ops;
struct strict_ops;
struct transparent_ops;
template<typename T>
struct transparent_ops_with;

// These types are used to specify what kind of operators the 'named_type' will support.
// Operator category tag should be placed after named type tag, i.e. before specifying 'vst::op's.
//
// Strict operators
// ----------------
// By default when using 'operator==', 'operator<' etc. both operands need to be 'named_type's.
// This behaviour is enabled when:
//  * no category is specified
//  * 'default_ops' is specified
//  * 'strict_ops' is specified
//..
//  using price = named_type<int, struct price, vst::op::ordered>;
//  // using price = named_type<int, struct price, default_ops, vst::op::ordered>; // same behaviour
//  // using price = named_type<int, struct price, strict_ops, vst::op::ordered>;  // same behaviour
//
//  price a{10}, a{20};
//  if (a == b) {}  // allowed
//  if (a < b) {}   // allowed
//  if (a == 10) {} // compile error
//  if (a < 10) {}  // compile error
//..
//
// Transparent operators
// ---------------------
// Trasparent operator means you can use an operator with two different types without casting,
// e.g. '2u = 2.f'
//
// 'named_type' can support transparent operators with the underlying type and/or a specified type,
// given it already can be used transparently in operators with the underlying type, see example below.
//
// Note, it's possible to specify transparent operators with multiple types.
//
// This behaviour is enabled when:
//  * 'transparent_ops' is specified, enables transparent ops with underlying type only
//  * 'transparent_ops_with<T>' is specified, enables transparent ops with specified type
//..
//  using price = named_type<int, struct price, transparent_ops, vst::op::ordered>;
//  // using price = named_type<int, struct price,
//                              transparent_ops_with<int>, vst::op::ordered>; // same behaviour
//
//  price a{10}, a{20};
//  if (a == 10) {} // allowed
//  if (a < 10)  {} // allowed
//
//  using price_float = named_type<int, struct price,
//                                 transparent_ops,
//                                 transparent_ops_with<float>, vst::op::ordered>;
//  // using price_float = named_type<int, struct price,
//                                    transparent_ops_with<int>,
//                                    transparent_ops_with<float>, vst::op::ordered>; // same behaviour
//
//  price_float a{10};
//  if (a == 10)   {} // allowed
//  if (a < 10)    {} // allowed
//  if (a == 10.f) {} // allowed
//  if (a < 10.f)  {} // allowed
//..

// ###################
// # is_ops_category #
// ###################

template<typename T>
struct is_ops_category : dev::type_list_contains<dev::type_list<default_ops, strict_ops, transparent_ops>, T> {};

template<typename T>
struct is_ops_category<transparent_ops_with<T>> : std::true_type {};

template<typename T>
constexpr bool is_ops_category_v = is_ops_category<T>::value;

static_assert(!is_ops_category_v<int>);
static_assert(!is_ops_category_v<float>);
static_assert( is_ops_category_v<default_ops>);
static_assert( is_ops_category_v<strict_ops>);
static_assert( is_ops_category_v<transparent_ops>);
static_assert( is_ops_category_v<transparent_ops_with<int>>);
static_assert( is_ops_category_v<transparent_ops_with<float>>);

// #########################
// # transform_op_category #
// #########################
//
// Utils to convert:
// * 'default_ops' into 'strict_ops'
// * 'transparent_ops' into 'transparent_ops_with<underlying_t>

// ################
// # type_convert #
// ################

template<typename from_t, typename into_t>
struct type_convert
{
    template<typename T>
    using func = std::conditional<std::is_same_v<T, from_t>, into_t, T>;

    template<typename T>
    using func_t = typename func<T>::type;
};

static_assert(std::is_same_v<void,  type_convert<int, float>::func<void>::type>);
static_assert(std::is_same_v<float, type_convert<int, float>::func<float>::type>);
static_assert(std::is_same_v<float, type_convert<int, float>::func<int>::type>);

// #############
// # apply_all #
// #############

template<typename T, template<typename> typename... funcs>
struct apply_all : std::type_identity<T> {};

template<typename T, template<typename> typename... funcs>
using apply_all_t = typename apply_all<T, funcs...>::type;

template<typename T, template<typename> typename func, template<typename> typename... other_funcs>
struct apply_all<T, func, other_funcs...>
{
    using type = typename apply_all<func<T>, other_funcs...>::type;
};

static_assert(std::is_same_v<void, apply_all_t<void>>);
static_assert(std::is_same_v<const int, apply_all_t<int, std::add_const_t>>);
static_assert(std::is_same_v<const int&, apply_all_t<int, std::add_const_t, std::add_lvalue_reference_t>>);
static_assert(std::is_same_v<void, apply_all_t<void, type_convert<int, void>::func_t>>);
static_assert(std::is_same_v<void, apply_all_t<int,  type_convert<int, void>::func_t>>);
static_assert(std::is_same_v<float, apply_all_t<int, type_convert<int, void>::func_t,
                                                     type_convert<void, float>::func_t>>);

// #########################
// # transform_op_category #
// ########################

template<typename underlying_t>
struct transform_op_category
{
    template<typename op_category_t>
    using func = apply_all<op_category_t,
                           type_convert<default_ops, strict_ops>::func_t,
                           type_convert<transparent_ops, transparent_ops_with<underlying_t>>::template func_t>;

    template<typename op_category_t>
    using func_t = typename func<op_category_t>::type;
};

static_assert(std::is_same_v<strict_ops, transform_op_category<int>::func_t<default_ops>>);
static_assert(std::is_same_v<strict_ops, transform_op_category<int>::func_t<strict_ops>>);
static_assert(std::is_same_v<transparent_ops_with<int>, transform_op_category<int>::func_t<transparent_ops>>);
static_assert(std::is_same_v<transparent_ops_with<int>, transform_op_category<int>::func_t<transparent_ops_with<int>>>);
static_assert(std::is_same_v<transparent_ops_with<float>, transform_op_category<int>::func_t<transparent_ops_with<float>>>);

// #########################
// # extract_op_categories #
// #########################

template<typename params_list, typename underlying_t>
struct extract_op_categories
{
    using raw_op_categories_list = dev::type_list_filter_t<params_list, is_ops_category>;
    using op_categories_list = std::conditional_t<
        std::is_same_v<dev::type_list<>, raw_op_categories_list>,
        dev::type_list<default_ops>, // insert 'default_ops' no other op categories are specified
        raw_op_categories_list>;
    using type = dev::type_list_transform_t<op_categories_list, transform_op_category<underlying_t>::template func_t>;
};

template<typename params_list, typename underlying_t>
using extract_op_categories_t = typename extract_op_categories<params_list, underlying_t>::type;

static_assert(std::is_same_v<dev::type_list<strict_ops>, extract_op_categories_t<dev::type_list<>, int>>);
static_assert(std::is_same_v<dev::type_list<strict_ops>, extract_op_categories_t<dev::type_list<vst::op::ordered, vst::op::hashable>, int>>);
static_assert(std::is_same_v<dev::type_list<strict_ops>, extract_op_categories_t<dev::type_list<strict_ops>, int>>);
static_assert(std::is_same_v<dev::type_list<strict_ops>, extract_op_categories_t<dev::type_list<default_ops>, int>>);
static_assert(std::is_same_v<
    dev::type_list<transparent_ops_with<int>>,
    extract_op_categories_t<dev::type_list<transparent_ops>, int>>);
static_assert(std::is_same_v<
    dev::type_list<transparent_ops_with<int>>,
    extract_op_categories_t<dev::type_list<transparent_ops, vst::op::ordered, vst::op::hashable>, int>>);
static_assert(std::is_same_v<
    dev::type_list<transparent_ops_with<int>, transparent_ops_with<float>>,
    extract_op_categories_t<dev::type_list<transparent_ops, transparent_ops_with<float>, vst::op::ordered, vst::op::hashable>, int>>);

// ########################
// # filter_op_categories #
// ########################

template<typename params_list>
struct filter_op_categories
: dev::type_list_filter<params_list, vst::is_op> {};

template<typename params_list>
using filter_op_categories_t = typename filter_op_categories<params_list>::type;

static_assert(std::is_same_v<
    dev::type_list<vst::op::ordered, vst::op::hashable>,
    filter_op_categories_t<dev::type_list<transparent_ops, transparent_ops_with<float>, vst::op::ordered, vst::op::hashable>>>);