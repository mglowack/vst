#pragma once

#include <named_type_pod.h>
#include <named_type_ops_category.h>
#include <named_type_trait.h>

#include "vst.hpp"
#include "type_list.h"

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
