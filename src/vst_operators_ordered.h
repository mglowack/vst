#pragma once

#include <vst_defs.h>
#include <vst_impl_helpers.h>

#include <type_traits>

namespace vst::impl
{

// ordered
template<typename T> requires OpEnabled<T, vst::op::ordered>
constexpr bool operator<(const T& lhs, const T& rhs)
{
    return helper::wrapped_tie(lhs) < helper::wrapped_tie(rhs);
}

template<typename T> requires OpEnabled<T, vst::op::ordered>
constexpr bool operator<=(const T& lhs, const T& rhs)
{
    return lhs < rhs || lhs == rhs;
}

template<typename T> requires OpEnabled<T, vst::op::ordered>
constexpr bool operator>(const T& lhs, const T& rhs)
{
    return !(lhs <= rhs);
}

template<typename T> requires OpEnabled<T, vst::op::ordered>
constexpr bool operator>=(const T& lhs, const T& rhs)
{
    return !(lhs < rhs);
}

} // namespace vst::impl