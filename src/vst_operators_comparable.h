#pragma once

#include <vst_defs.h>
#include <vst_impl_helpers.h>

#include <type_traits>

namespace vst::impl
{

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

} // namespace vst::impl