#pragma once

#include <vst_defs.h>

#include <dev_functional.h>
#include <dev_tuple.h>

#include <functional>
#include <type_traits>

namespace vst
{

template <typename op_t, typename vst_t>
constexpr vst_t& binary_assign_op(vst_t& lhs, const vst_t& rhs)
{
    dev::apply_with_index([rhs_tie = vst::trait<vst_t>::tie(rhs)](const auto... a) {
        (op_t{}(
            a.value,
            std::get<a.index>(rhs_tie)), ...);
    }, vst::trait<vst_t>::tie(lhs));
    return lhs;
}

template <typename op_t, typename vst_t>
constexpr vst_t binary_op(const vst_t& lhs, const vst_t& rhs)
{
    return dev::apply_with_index(
        [rhs_tie = vst::trait<vst_t>::tie(rhs)](const auto... a) {
        return vst_t{op_t{}(
            a.value,
            std::get<a.index>(rhs_tie))...};
    }, vst::trait<vst_t>::tie(lhs));
}

template<OpEnabled<op::addable> T>
constexpr T operator+(const T& lhs, const T& rhs)
{
    return binary_op<std::plus<>>(lhs, rhs);
}

template<OpEnabled<op::addable> T>
constexpr T operator-(const T& lhs, const T& rhs)
{
    return binary_op<std::minus<>>(lhs, rhs);
}

template<OpEnabled<op::addable> T>
constexpr T& operator+=(T& lhs, const T& rhs)
{
    return binary_assign_op<dev::plus_assign<>>(lhs, rhs);
}

template<OpEnabled<op::addable> T>
constexpr T& operator-=(T& lhs, const T& rhs)
{
    return binary_assign_op<dev::minus_assign<>>(lhs, rhs);
}

} // namespace vst