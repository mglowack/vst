#pragma once

#include <dev_type_list.h>

namespace vst::op { struct ordered;  }
namespace vst::op { struct hashable; }
namespace vst::op { struct addable;  }
namespace vst {

// #########
// # is_op #
// #########

using all_ops_list = dev::type_list<op::ordered, op::hashable, op::addable>;

template<typename T>
struct is_op : dev::type_list_contains<all_ops_list, T> {};

template<typename T>
constexpr bool is_op_v = is_op<T>::value;

// ############
// # Operator #
// ############

template<typename T>
concept Operator = is_op_v<T>;

} // namespace vst