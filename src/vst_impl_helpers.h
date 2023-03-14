#pragma once

#include <vst_defs.h>
#include <vst_utils.h>
#include <vst_wrapped_value.h>
#include <vst_named_var.h>
#include <vst_indexed_var.h>

#include <boost/pfr.hpp>

#include <tuple>

namespace vst::impl {

// ###########
// # helpers #
// ###########

struct indexed_tie_helper
{
    template<typename vst_t, typename... Ts>
    static constexpr auto tie(std::tuple<Ts&...> fields)
    {
        return apply_with_index(
            [](const auto... elem) { 
                return std::tuple(as_indexed_var<vst_t, elem.index + 1>(elem.value)...); // convert to 1-based
            }, 
            fields);
    }

    template<typename vst_t, std::size_t I, typename T>
    static constexpr auto as_indexed_var(const T& var)
    {
        return indexed_var<wrapped_value_of<vst_t, T>, I>{wrapped_value_of<vst_t, T>{var}};
    }
};

template<typename fields_def_t>
struct described_vst_helper
{
    template<typename T>
    static constexpr auto tie(T& obj)
    {
        return tie(obj, fields_def_t::get_fields());
    }

    template<typename T>
    static constexpr auto named_tie(T& obj)
    {
        return named_tie<std::decay_t<T>>(obj, fields_def_t::get_fields());
    }

private:
    template<typename T, typename... field_ptrs_t>
    static constexpr auto tie(T& obj, const std::tuple<field_ptrs_t...>& fields)
    {
        return std::apply(
            [&obj](const auto&... f) { 
                return std::tie(as_ref_to_value(obj, f)...); 
            }, 
            fields);
    }

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
        return indexed_tie_helper::tie<vst_t>(tie(obj, fields)); // fallback to indexing members
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
        return named_var<wrapped_value_of<vst_t, T>>{name, wrapped_value_of<vst_t, T>{var}};
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
        return indexed_tie_helper::tie<std::decay_t<T>>(tie(obj));
    }

private:    
    template<typename T>
    static constexpr decltype(auto) as_aggregate(T& obj)
    {
        return static_cast<propagate_const_t<T, aggregate_t<std::remove_const_t<T>>>&>(obj);
    }
};

struct helper 
{
    template<typename T, typename op>
    static constexpr bool has_op() { return type_list_contains_v<typename trait<T>::properties, op>; }

    template<typename T>
    static constexpr auto tie(T& obj)
    {
        return trait<std::decay_t<T>>::tie(obj);
    }

    template<typename T>
    static constexpr auto named_tie(T& obj)
    {
        return trait<std::decay_t<T>>::named_tie(obj);
    }

    template<typename T>
    static constexpr auto wrapped_tie(T& obj)
    {
        using vst_t = std::decay_t<T>;
        // if constexpr (std::is_same_v<std::tuple<>, decltype(tie(obj))>)
        // {
        //     return std::tuple{};
        // }
        // else
        {
            return std::apply(
                [](auto&... f) { 
                    return std::tuple(wrapped_value_of<vst_t, std::decay_t<decltype(f)>>{f}...); 
                }, 
                tie(obj));
        }
    }
};

} // close vst::impl namespace