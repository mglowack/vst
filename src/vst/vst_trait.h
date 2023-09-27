#pragma once

#include <vst_defs.h>
#include <vst_named_var.h>
#include <vst_indexed_var.h>

#include <dev_type_list.h>
#include <dev_type_traits.h>

#include <boost/pfr.hpp>

namespace vst {

namespace impl {

// #########
// # trait #
// #########

template<typename T, typename fields_def_t, Operator... ops>
struct trait
{
    using pod_t = T;
    using fields_def = fields_def_t;
    using properties = dev::type_list<ops...>;
};

} // namespace impl

// no 'with_fields::X' specified => injecting 'with_fields::use_default'
template<typename T, Operator... ops>
struct trait<type<T, ops...>>
: trait<type<T, with_fields::use_default, ops...>>
{
};

// 'with_fields::use_default':
// * translate to 'with_fields::from<T>' if 'get_fields()' is provided on the underlying type
// * translate to 'with_fields::from_aggregate' otherwise
#if 1
// NOTE: workaround for https://stackoverflow.com/questions/77187649/c20-partial-class-specialization-with-concepts-and-variadic-template-args
template <typename... Ts>
concept all_ops = (Operator<Ts> && ...);

template<SelfDescribed T, typename... ops> requires all_ops<ops...>
struct trait<type<T, with_fields::use_default, ops...>>
: trait<type<T, with_fields::from<T>, ops...>>
{
};

template<typename T, typename... ops> requires all_ops<ops...>
struct trait<type<T, with_fields::use_default, ops...>>
: trait<type<T, with_fields::from_aggregate, ops...>>
{
    static_assert(SelfDescribed<T> or std::is_aggregate_v<T>, "T must be an aggregate or have 'get_fields' defined.");
};
#else
template<SelfDescribed T, Operator... ops>
struct trait<type<T, with_fields::use_default, ops...>>
: trait<type<T, with_fields::from<T>, ops...>>
{
};

template<typename T, Operator... ops>
struct trait<type<T, with_fields::use_default, ops...>>
: trait<type<T, with_fields::from_aggregate, ops...>>
{
    static_assert(SelfDescribed<T> or std::is_aggregate_v<T>, "T must be an aggregate or have 'get_fields' defined.");
};
#endif

// 'with_fields::from_aggregate'
template<typename T, Operator... ops>
struct trait<type<T, with_fields::from_aggregate, ops...>>
: impl::trait<T, with_fields::from_aggregate, ops...>
{
    static_assert(std::is_aggregate_v<T>, "T must be an aggregate.");

    template<typename U>
    static constexpr auto tie(U& obj)
    {
        return boost::pfr::structure_tie(static_cast<dev::propagate_const_t<U, T>&>(obj));
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
template<typename T, DescribesThe<T> fields_def, Operator... ops>
struct trait<type<T, fields_def, ops...>>
: impl::trait<T, fields_def, ops...>
{
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