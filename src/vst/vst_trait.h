#pragma once

#include <vst_defs.h>
#include <vst_utils.h>
#include <vst_named_var.h>
#include <vst_indexed_var.h>

#include <type_list.h>

#include <boost/pfr.hpp>

namespace vst {

namespace impl {

// #################
// # is_fields_def #
// #################

template<typename T>  constexpr bool is_fields_def = false;
template<auto (*f)()> constexpr bool is_fields_def<with_fields::from_func<f>>   = true;
template<auto v>      constexpr bool is_fields_def<with_fields::from_var<v>>    = true;
template<typename T>  constexpr bool is_fields_def<with_fields::from<T>>        = true;
template<>            constexpr bool is_fields_def<with_fields::empty>          = true;

template<>            constexpr bool is_fields_def<with_fields::from_aggregate> = false;
template<>            constexpr bool is_fields_def<with_fields::use_default>    = false;

template<typename T>
concept FieldsDef = is_fields_def<T>;

// #########
// # trait #
// #########

template<typename T, typename fields_def_t, typename... ops>
struct trait
{
    using pod_t = T;
    using fields_def = fields_def_t;
    using properties = type_list<ops...>;
};

} // namespace impl

// no 'with_fields::X' specified => injecting 'with_fields::use_default'
template<typename T, typename... ops>
struct trait<type<T, ops...>>
: trait<type<T, with_fields::use_default, ops...>>
{
};

// 'with_fields::use_default':
// * translate to 'with_fields::from<T>' if 'get_fields()' is provided on the underlying type
// * translate to 'with_fields::from_aggregate' otherwise
template<SelfDescribed T, typename... ops>
struct trait<type<T, with_fields::use_default, ops...>>
: trait<type<T, with_fields::from<T>, ops...>>
{
};

template<typename T, typename... ops>
struct trait<type<T, with_fields::use_default, ops...>>
: trait<type<T, with_fields::from_aggregate, ops...>>
{
    static_assert(SelfDescribed<T> or std::is_aggregate_v<T>, "T must be an aggregate or have 'get_fields' defined.");
};

// 'with_fields::from_aggregate'
template<typename T, typename... ops>
struct trait<type<T, with_fields::from_aggregate, ops...>>
: impl::trait<T, with_fields::from_aggregate, ops...>
{
    static_assert(std::is_aggregate_v<T>, "T must be an aggregate.");

    template<typename U>
    static constexpr auto tie(U& obj)
    {
        return boost::pfr::structure_tie(static_cast<propagate_const_t<U, T>&>(obj));
    }

    template<typename U>
    static constexpr auto named_tie(U& obj)
    {
        return vst::indexed_var_util::index(tie(obj));
    }
};

// 'with_fields::from<T>'
// 'with_fields::from_func<f>'
// 'with_fields::from_var<v>'
// 'with_fields::empty'
template<typename T, SelfDescribed fields_def, typename... ops>// requires impl::is_fields_def<fields_def>
struct trait<type<T, fields_def, ops...>>
: impl::trait<T, fields_def, ops...>
{
    // static_assert(has_correct_get_fields<fields_def, T>,
    static_assert(is_fields_spec<T, decltype(fields_def::get_fields())>,
    "'get_fields' must return a tuple of either pointer to members or named_field_ptr");

    template<typename U>
    static constexpr auto tie(U& obj)
    {
        return std::apply(
            [&obj](const auto&... f) { return std::tie(as_ref_to_value(obj, f)...); },
            fields_def::get_fields());
    }

    template<typename U>
    static constexpr auto named_tie(U& obj)
    {
        return named_tie(obj, fields_def::get_fields());
    }

private:
    template<typename U, typename... field_ptrs>
    static constexpr auto named_tie(U& obj, std::tuple<named_field_ptr<field_ptrs>...> fields)
    {
        return std::apply(
            [&obj](const auto&... f) {
                return std::tuple(named_var{f.name, as_ref_to_value(obj, f)}...);
            },
            fields);
    }

    template<typename U, typename... field_ptrs>
    static constexpr auto named_tie(U& obj, std::tuple<field_ptrs...> fields)
    {
        // fallback to indexing members when fields are not 'named_field_ptr's
        return vst::indexed_var_util::index(tie(obj));
    }

    template<typename U, typename field_ptr_t>
    static constexpr decltype(auto) as_ref_to_value(U& obj, field_ptr_t f)
    {
        return obj.*f;
    }

    template<typename U, typename field_ptr_t>
    static constexpr decltype(auto) as_ref_to_value(U& obj, const named_field_ptr<field_ptr_t>& f)
    {
        return obj.*f.field_ptr;
    }
};

} // namespace vst