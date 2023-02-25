#pragma once

#include <vst_defs.h>
#include <vst_impl_helpers.h>
#include <vst_functional.h>

#include <type_traits>

namespace vst::impl
{

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