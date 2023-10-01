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
struct is_ops_category : std::bool_constant<dev::type_list<default_ops, strict_ops, transparent_ops>::contains<T>> {};

template<typename T>
struct is_ops_category<transparent_ops_with<T>> : std::true_type {};

template<typename T>
constexpr bool is_ops_category_v = is_ops_category<T>::value;

template<typename T>
concept OpCategory = is_ops_category_v<T>;

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

// #########################
// # transform_op_category #
// ########################

template<typename underlying_t, OpCategory op_category_t>
struct transform_op_category : dev::apply_all<
        op_category_t,
        dev::type_convert_f<default_ops, strict_ops>::func,
        dev::type_convert_f<transparent_ops, transparent_ops_with<underlying_t>>::template func>
{};

template<typename underlying_t>
using transform_op_category_f = dev::make_func_single<transform_op_category, underlying_t>;

static_assert(std::is_same_v<strict_ops,                  dev::make_func_single<transform_op_category, int>::func_t<default_ops>>);
static_assert(std::is_same_v<strict_ops,                  dev::make_func_single<transform_op_category, int>::func_t<strict_ops>>);
static_assert(std::is_same_v<transparent_ops_with<int>,   dev::make_func_single<transform_op_category, int>::func_t<transparent_ops>>);
static_assert(std::is_same_v<transparent_ops_with<int>,   dev::make_func_single<transform_op_category, int>::func_t<transparent_ops_with<int>>>);
static_assert(std::is_same_v<transparent_ops_with<float>, dev::make_func_single<transform_op_category, int>::func_t<transparent_ops_with<float>>>);

// #########################
// # extract_op_categories #
// #########################

template<dev::any_type_list params_list, typename underlying_t>
struct extract_op_categories
{
    using raw_op_categories_list = typename params_list::template erase_if<dev::combine<is_ops_category, std::negation>::result>;
    using op_categories_list = std::conditional_t<
        raw_op_categories_list::is_empty,
        dev::type_list<default_ops>, // insert 'default_ops' no other op categories are specified
        raw_op_categories_list>;
    using type = typename op_categories_list::template transform<transform_op_category_f<underlying_t>::template func_t>;
};

template<dev::any_type_list params_list, typename underlying_t>
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

template<dev::any_type_list params_list>
using filter_op_categories_t = typename params_list::template erase_if<is_ops_category>;

static_assert(std::is_same_v<
    dev::type_list<vst::op::ordered, vst::op::hashable>,
    filter_op_categories_t<dev::type_list<transparent_ops, transparent_ops_with<float>, vst::op::ordered, vst::op::hashable>>>);