#pragma once

#include <vst_defs.h>

#include <type_traits>
#include <ostream>

namespace vst::impl {

// stream
template<Type T>
std::ostream& operator<<(std::ostream& os, const T& rhs)
{
    os << "[";
    std::apply(
        [&os](const auto&... field){
            ((os << " " << field), ...);
        }, vst::trait<T>::named_tie(rhs));
    os << " ]";
    return os;
}

} // namespace vst::impl