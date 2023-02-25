#pragma once

#include <vst_defs.h>
#include <vst_impl_helpers.h>

#include <type_traits>
#include <ostream>

namespace vst::impl {

// stream
template<typename T, std::enable_if_t<vst::trait<T>::exists, int> = 0>
std::ostream& operator<<(std::ostream& os, const T& rhs)
{
    os << "[";
    std::apply(
        [&os](const auto&... field){
            ((os << " " << field), ...);
        }, helper::named_tie(rhs));
    os << " ]";
    return os;
}

} // namespace vst::impl