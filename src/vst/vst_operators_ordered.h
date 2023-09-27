#pragma once

#include <vst_defs.h>
#include <vst_wrapped_value.h>

#include <type_traits>

namespace vst
{

// ordered
template<OpEnabled<op::ordered> T>
constexpr auto operator<=>(const T& lhs, const T& rhs)
{
    return vst::wrap<T>(vst::trait<T>::tie(lhs)) <=> vst::wrap<T>(vst::trait<T>::tie(rhs));
}

} // namespace vst