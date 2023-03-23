#pragma once

#include <vst_defs.h>

#include <type_traits>

namespace vst::impl
{

// ordered
template<OpEnabled<op::ordered> T>
constexpr bool operator<(const T& lhs, const T& rhs)
{
    return vst::trait<T>::wrapped_tie(lhs) < vst::trait<T>::wrapped_tie(rhs);
}

template<OpEnabled<op::ordered> T>
constexpr bool operator<=(const T& lhs, const T& rhs)
{
    return lhs < rhs || lhs == rhs;
}

template<OpEnabled<op::ordered> T>
constexpr bool operator>(const T& lhs, const T& rhs)
{
    return !(lhs <= rhs);
}

template<OpEnabled<op::ordered> T>
constexpr bool operator>=(const T& lhs, const T& rhs)
{
    return !(lhs < rhs);
}

} // namespace vst::impl