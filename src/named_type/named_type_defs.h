#pragma once

#include <named_type_pod.h>
#include <named_type_ops_category.h>
#include <named_type_trait.h>

#include "vst.hpp"
#include "type_list.h"

// #########################
// # extract_op_categories #
// #########################

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

// ########################
// # filter_op_categories #
// ########################

template<typename params_list>
struct filter_op_categories
: type_list_filter<params_list, trait_op<is_ops_category>::negate> {};

template<typename params_list>
using filter_op_categories_t = typename filter_op_categories<params_list>::type;

static_assert(std::is_same_v<
    type_list<vst::op::ordered, vst::op::hashable>,
    filter_op_categories_t<type_list<transparent_ops, transparent_ops_with<float>, vst::op::ordered, vst::op::hashable>>>);

// ##############
// # named_type #
// ##############

// template<typename underlying_t, typename tag_t, typename... params>
// using named_type = vst::impl::type<
//     named_type_pod<
//         underlying_t,
//         tag_t,
//         extract_op_categories_t<type_list<params...>, underlying_t>
//     >,
//     filter_op_categories_t<type_list<params...>>>;
