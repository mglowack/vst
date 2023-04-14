#pragma once

#include <vst_defs_ops.h>

#include <dev_type_list.h>
#include <dev_type_traits.h>

namespace vst::impl {

template <typename T, typename properties_t>
struct type
: public T
{
    using T::T;
};

template <typename T, typename properties_t> requires std::is_aggregate_v<T>
struct type<T, properties_t>
: public T {};

} // close vst::impl namespace

namespace vst {

template <typename T, typename... ops>
using type = impl::type<T, dev::type_list<ops...>>;

template<typename T>
struct trait;

template <typename T>
concept Type = requires {
    typename trait<T>::pod_t;
    typename trait<T>::fields_def;
    typename trait<T>::properties;
};

template <typename T, typename OP>
concept OpEnabled = Type<T> && dev::type_list_contains_v<typename trait<T>::properties, OP>;

}