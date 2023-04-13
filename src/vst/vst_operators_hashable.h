#pragma once

#include <vst_defs.h>

#include <boost/functional/hash.hpp>

#include <type_traits>

namespace vst {

template<OpEnabled<op::hashable> T>
struct hash
{
    constexpr size_t operator()(const T& o) const noexcept
    {
        // use boost helper for hash of tuples
        return boost::hash_value(vst::trait<T>::tie(o));
    }
};

template<OpEnabled<op::hashable> T>
constexpr auto hash_value(const T& o)
{
    return hash<T>{}(o);
}

} // namespace vst

namespace boost { template<vst::Type T> struct hash<T> : vst::hash<T> {}; }
namespace std   { template<vst::Type T> struct hash<T> : vst::hash<T> {}; }