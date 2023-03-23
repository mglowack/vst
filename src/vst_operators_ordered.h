#pragma once

#include <vst_defs.h>

#include <type_traits>

namespace vst::impl
{

// ordered
template<typename T> requires OpEnabled<T, vst::op::ordered>
constexpr bool operator<(const T& lhs, const T& rhs)
{
    return vst::trait<T>::wrapped_tie(lhs) < vst::trait<T>::wrapped_tie(rhs);
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