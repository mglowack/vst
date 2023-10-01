#pragma once

#include <vst_defs_ops.h>
#include <vst_defs_fields.h>
#include <vst_defs_with_fields.h>

#include <dev_type_list.h>

#include <type_traits>

namespace vst {

template <typename T, typename... params>
struct type
: public T
{
    using T::T;
};

template <typename T, typename... params> requires std::is_aggregate_v<T>
struct type<T, params...>
: public T {};

template<typename T>
struct trait;

template <typename T>
concept Type = requires {
    typename trait<T>::pod_t;
    typename trait<T>::fields_def;
    typename trait<T>::properties;
};

template <typename T, typename OP> // why not Operator OP?
concept OpEnabled = Type<T> && trait<T>::properties::template contains<OP>;

} // namespace vst