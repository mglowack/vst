#ifndef VST_IMPL_H
#define VST_IMPL_H

#include <vst_defs.h>
#include <vst_utils.h>
#include <vst_functional.h>
#include <vst_wrapped_value.h>
#include <vst_named_var.h>
#include <vst_indexed_var.h>
#include <vst_impl_helpers.h>

#include <vst_operators_hash.h>
#include <vst_operators_stream.h>

#include <type_list.h>

#include <boost/functional/hash.hpp>
#include <boost/pfr.hpp>

#include <string>
#include <memory>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <type_traits>
#include <cassert>

namespace vst {

// #########
// # trait #
// #########

namespace impl {

template<typename T, typename fields_def_helper_t, typename... ops>
struct trait
: fields_def_helper_t
{
    static constexpr bool exists = true;
    using properties = type_list<ops...>;
};

} // namespace impl

template<typename T>
struct trait<type<T>>
: trait<type<T, with_fields::use_default>>
{
};

template<typename T, typename... ops>
struct trait<
    type<T, with_fields::use_default, ops...>, 
    std::enable_if_t<!has_get_fields<T>>>
: trait<type<T, with_fields::from_aggregate, ops...>>
{
    static_assert(std::is_aggregate_v<T>, "T must be an aggregate or have 'get_fields' defined.");
};

template<typename T, typename... ops>
struct trait<
    type<T, with_fields::use_default, ops...>, 
    std::enable_if_t<has_get_fields<T>>>
: trait<type<T, with_fields::from<T>, ops...>>
{
};

template<typename T, typename... ops>
struct trait<type<T, with_fields::from_aggregate, ops...>>
: impl::trait<T, impl::aggregate_vst_helper, ops...>
{
    static_assert(std::is_aggregate_v<T>, "T must be an aggregate.");
};

template<typename T, typename first_op, typename... ops>
struct trait<
    type<T, first_op, ops...>, 
    std::enable_if_t<!is_fields_def<first_op> && !std::is_same_v<first_op, with_fields::use_default>>>
: trait<type<T, with_fields::use_default, first_op, ops...>>
{
};

template<typename T, typename fields_def, typename... ops>
struct trait<
    type<T, fields_def, ops...>, 
    std::enable_if_t<is_fields_def<fields_def>>>
: impl::trait<T, impl::described_vst_helper<fields_def>, ops...>
{
    static_assert(has_correct_get_fields<fields_def, T>, "'get_fields' must return a tuple of pointer to members or named_field_ptr");
};

} // namespace vst

// #############
// # operators #
// #############

namespace vst::impl {

// comparable
template<typename T, std::enable_if_t<vst::trait<T>::exists, int> = 0>
constexpr bool operator==(const T& lhs, const T& rhs)
{
    return helper::wrapped_tie(lhs) == helper::wrapped_tie(rhs);
}

template<typename T, std::enable_if_t<vst::trait<T>::exists, int> = 0>
constexpr bool operator!=(const T& lhs, const T& rhs)
{
    return !(lhs == rhs);
}

// ordered
template<
    typename T, 
    std::enable_if_t<vst::trait<T>::exists && helper::has_op<T, vst::op::ordered>(), int> = 0>
constexpr bool operator<(const T& lhs, const T& rhs)
{
    return helper::wrapped_tie(lhs) < helper::wrapped_tie(rhs);
}

template<
    typename T, 
    std::enable_if_t<vst::trait<T>::exists && helper::has_op<T, vst::op::ordered>(), int> = 0>
constexpr bool operator<=(const T& lhs, const T& rhs)
{
    return lhs < rhs || lhs == rhs;
}

template<
    typename T, 
    std::enable_if_t<vst::trait<T>::exists && helper::has_op<T, vst::op::ordered>(), int> = 0>
constexpr bool operator>(const T& lhs, const T& rhs)
{
    return !(lhs <= rhs);
}

template<
    typename T, 
    std::enable_if_t<vst::trait<T>::exists && helper::has_op<T, vst::op::ordered>(), int> = 0>
constexpr bool operator>=(const T& lhs, const T& rhs)
{
    return !(lhs < rhs);
}

template <typename op_t, typename vst_t>
constexpr vst_t& binary_assign_op(vst_t& lhs, const vst_t& rhs)
{
    apply_with_index([rhs_tie = helper::tie(rhs)](const auto... a) {
        (op_t{}(
            a.value,
            std::get<a.index>(rhs_tie)), ...);
    }, helper::tie(lhs));
    return lhs;
}

template <typename op_t, typename vst_t>
constexpr vst_t binary_op(const vst_t& lhs, const vst_t& rhs)
{
    return apply_with_index(
        [rhs_tie = helper::tie(rhs)](const auto... a) {
        return vst_t{op_t{}(
            a.value,
            std::get<a.index>(rhs_tie))...};
    }, helper::tie(lhs));
}

template<
    typename T, 
    std::enable_if_t<vst::trait<T>::exists && helper::has_op<T, vst::op::addable>(), int> = 0>
constexpr T operator+(const T& lhs, const T& rhs)
{
    return binary_op<std::plus<>>(lhs, rhs);
}
template<
    typename T, 
    std::enable_if_t<vst::trait<T>::exists && helper::has_op<T, vst::op::addable>(), int> = 0>
constexpr T operator-(const T& lhs, const T& rhs)
{
    return binary_op<std::minus<>>(lhs, rhs);
}
template<
    typename T, 
    std::enable_if_t<vst::trait<T>::exists && helper::has_op<T, vst::op::addable>(), int> = 0>
constexpr T& operator+=(T& lhs, const T& rhs)
{
    return binary_assign_op<plus_assign<>>(lhs, rhs);
}
template<
    typename T, 
    std::enable_if_t<vst::trait<T>::exists && helper::has_op<T, vst::op::addable>(), int> = 0>
constexpr T& operator-=(T& lhs, const T& rhs)
{
    return binary_assign_op<minus_assign<>>(lhs, rhs);
}

} // namespace vst::impl

#endif