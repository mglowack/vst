#pragma once

#include <vst_defs.h>
#include <vst_wrapped_value.h>

#include <type_traits>

namespace vst::impl
{

// comparable
template<Type T>
constexpr bool operator==(const T& lhs, const T& rhs)
{
    return vst::wrap<T>(vst::trait<T>::tie(lhs)) == vst::wrap<T>(vst::trait<T>::tie(rhs));
}

template<Type T>
constexpr bool operator!=(const T& lhs, const T& rhs)
{
    return !(lhs == rhs);
}

} // namespace vst::impl