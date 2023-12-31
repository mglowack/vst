#pragma once

#include <vst_defs.h>
#include <vst_wrapped_value.h>

#include <type_traits>
#include <ostream>

namespace vst {

// stream
template<Type T>
std::ostream& operator<<(std::ostream& os, const T& rhs)
{
    os << "[";
    std::apply(
        [&os](const auto&... field){
            ((os << " " << field), ...);
        }, vst::wrap<T>(vst::trait<T>::named_tie(rhs)));
    os << " ]";
    return os;
}

} // namespace vst