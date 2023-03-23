#pragma once

#include <vst_defs.h>
#include <vst_impl_helpers.h>

#include <type_traits>

namespace vst
{

// template<typename T,
        //  typename ENABLER = std::enable_if_t<trait<T>::exists && impl::helper::has_op<T, op::hashable>()>>
// struct hash

template<typename T, typename ENABLER = void> requires (trait<T>::exists && impl::helper::has_op<T, op::hashable>())
struct hash
{
    constexpr size_t operator()(const T& o) const noexcept
    {
        // use boost helper for hash of tuples
        return boost::hash_value(impl::helper::tie(o));
    }
};

} // namespace vst

namespace boost
{

// boost::hash support
template<typename... args_t>
struct hash<vst::impl::type<args_t...>> : vst::hash<vst::impl::type<args_t...>>
{
};

} // namespace boost

namespace std
{

// std::hash support
template<typename... args_t>
struct hash<vst::impl::type<args_t...>> : vst::hash<vst::impl::type<args_t...>>
{
};

} // namespace std