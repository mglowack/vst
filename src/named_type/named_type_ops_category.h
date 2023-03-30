#pragma once

#include "type_list.h"

#include <type_traits>

// #######################
// # operator categories #
// #######################
//
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

// #################
// # op categories #
// #################

struct default_ops;
struct strict_ops;
struct transparent_ops;
template<typename T>
struct transparent_ops_with;

// ###################
// # is_ops_category #
// ###################

template<typename T>
struct is_ops_category : type_list_contains<type_list<default_ops, strict_ops, transparent_ops>, T> {};

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

template<typename from_t, typename into_t>
struct type_convert
{
    template<typename T>
    using func = std::conditional<std::is_same_v<T, from_t>, into_t, T>;
};

static_assert(std::is_same_v<void,  type_convert<int, float>::func<void>::type>);
static_assert(std::is_same_v<float, type_convert<int, float>::func<float>::type>);
static_assert(std::is_same_v<float, type_convert<int, float>::func<int>::type>);

// template<typename T, typename... funcs>
// struct apply_all;

// template<typename T, template<typename> typename func, typename... other_funcs>
// struct apply_all<T, func, other_funcs...>
// {
//     using type = apply_all<func<T>::type, other_funcs...>::type;
// };

// template<typename underlying_t, typename... conversions>
// struct transform_op_category
// {
//     template<typename op_category_t>
//     using func = apply_all_t<
//         op_category_t,
//         type_convert<default_ops, strict_ops>::func,
//         type_convert<transparent_ops, transparent_ops_with<underlying_t>>::func>;
// };

// template<typename underlying_t, typename... op_category_t>
// struct transform_op_category
// {
//     template<typename op_category_t>
//     using func = the_func<
//         type_convert<default_ops, strict_ops>,
//         type_convert<transparent_ops, transparent_ops_with<underlying_t>>
//     >::perform<op_category_t>::type;
// };

// template<typename op_category_t>
// struct transform_op_category
// {
//     using type = type_convert<default_ops, strict_ops>::func<op_category_t>::type;
// };

// template<typename op_category_t>
// using transform_op_category_t = typename transform_op_category<op_category_t>::type;

// template<typename T, typename op_category_t>
// struct transform_op_category_x : transform_op_category<op_category_t> {};

// template<typename T>
// struct transform_op_category_x<T, transparent_ops> {
//     using type = transparent_ops_with<T>;
// };

// template<typename T, typename op_category_t>
// using transform_op_category_x_t = typename transform_op_category_x<T, op_category_t>::type;

// static_assert(std::is_same_v<transform_op_category_t<default_ops>, strict_ops>);
// static_assert(std::is_same_v<transform_op_category_t<strict_ops>, strict_ops>);
// static_assert(std::is_same_v<transform_op_category_t<transparent_ops>, transparent_ops>);
// static_assert(std::is_same_v<transform_op_category_t<transparent_ops_with<int>>, transparent_ops_with<int>>);

// static_assert(std::is_same_v<transform_op_category_x_t<int, default_ops>, strict_ops>);
// static_assert(std::is_same_v<transform_op_category_x_t<int, strict_ops>, strict_ops>);
// static_assert(std::is_same_v<transform_op_category_x_t<int, transparent_ops>, transparent_ops_with<int>>);
// static_assert(std::is_same_v<transform_op_category_x_t<int, transparent_ops_with<int>>, transparent_ops_with<int>>);
