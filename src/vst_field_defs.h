#pragma once

#include <vst_defs.h>
#include <vst_utils.h>
#include <vst_wrapped_value.h>
#include <vst_named_var.h>
#include <vst_indexed_var.h>

#include <boost/pfr.hpp>

#include <tuple>

namespace vst::impl {

template<typename fields_def_t>
struct described_vst_helper
{
    template<typename T>
    static constexpr auto tie(T& obj)
    {
        return std::apply(
            [&obj](const auto&... f) {
                return std::tie(as_ref_to_value(obj, f)...);
            },
            fields_def_t::get_fields());
    }

    template<typename T>
    static constexpr auto named_tie(T& obj)
    {
        return named_tie<std::decay_t<T>>(obj, fields_def_t::get_fields());
    }

private:
    template<typename vst_t, typename T, typename... field_ptrs_t>
    static constexpr auto named_tie(
        T& obj, const std::tuple<named_field_ptr<field_ptrs_t>...>& fields)
    {
        return std::apply(
            [&obj](const auto&... f) {
                return std::tuple(as_named_var<vst_t>(f.name, as_ref_to_value(obj, f))...);
            },
            fields);
    }

    // NOTE: this overload takes over if fields ARE NOT named_field_ptr<T> i.e. no names were specified
    template<typename vst_t, typename T, typename... field_ptrs_t>
    static constexpr auto named_tie(T& obj, const std::tuple<field_ptrs_t...>& fields)
    {
        return vst::indexed_var_util::tie<vst_t>(tie(obj)); // fallback to indexing members
    }

    template<typename T, typename field_ptr_t>
    static constexpr decltype(auto) as_ref_to_value(T& obj, field_ptr_t f)
    {
        return obj.*f;
    }

    template<typename T, typename field_ptr_t>
    static constexpr decltype(auto) as_ref_to_value(T& obj, const named_field_ptr<field_ptr_t>& f)
    {
        return obj.*f.field_ptr;
    }

    template<typename vst_t, typename T>
    static constexpr auto as_named_var(const char* name, const T& var)
    {
        return named_var_util::wrap<vst_t>(named_var{name, var});
    }
};

struct aggregate_vst_helper
{
    template<typename T>
    static constexpr auto tie(T& obj)
    {
        return boost::pfr::structure_tie(as_aggregate(obj));
    }

    template<typename T>
    static constexpr auto named_tie(T& obj)
    {
        return vst::indexed_var_util::tie<std::decay_t<T>>(tie(obj));
    }

private:
    template<typename T>
    static constexpr decltype(auto) as_aggregate(T& obj)
    {
        return static_cast<propagate_const_t<T, aggregate_t<std::remove_const_t<T>>>&>(obj);
    }
};

} // close vst::impl namespace