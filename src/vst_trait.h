#pragma once

#include <vst_defs.h>
#include <vst_impl_helpers.h>

#include <type_list.h>

namespace vst {

namespace impl {

// #################
// # is_fields_def #
// #################

template<typename T>  constexpr bool is_fields_def = false;
template<auto (*f)()> constexpr bool is_fields_def<with_fields::from_func<f>> = true;
template<auto v>      constexpr bool is_fields_def<with_fields::from_var<v>> = true;
template<typename T>  constexpr bool is_fields_def<with_fields::from<T>> = true;
// template<>            constexpr bool is_fields_def<with_fields::from_aggregate> = true;
template<>            inline constexpr bool is_fields_def<with_fields::empty> = true;


// #########
// # trait #
// #########

template<typename T, typename fields_def_helper_t, typename... ops>
struct trait
: fields_def_helper_t
{
    static constexpr bool exists = true;
    using properties = type_list<ops...>;
};

} // namespace impl

template<typename T>
struct trait<type<T>>
: trait<type<T, with_fields::use_default>>
{
};

template<typename T, typename... ops>
struct trait<
    type<T, with_fields::use_default, ops...>, 
    std::enable_if_t<!has_get_fields<T>>>
: trait<type<T, with_fields::from_aggregate, ops...>>
{
    static_assert(std::is_aggregate_v<T>, "T must be an aggregate or have 'get_fields' defined.");
};

template<typename T, typename... ops>
struct trait<
    type<T, with_fields::use_default, ops...>, 
    std::enable_if_t<has_get_fields<T>>>
: trait<type<T, with_fields::from<T>, ops...>>
{
};

template<typename T, typename... ops>
struct trait<type<T, with_fields::from_aggregate, ops...>>
: impl::trait<T, impl::aggregate_vst_helper, ops...>
{
    static_assert(std::is_aggregate_v<T>, "T must be an aggregate.");
};

template<typename T, typename first_op, typename... ops>
struct trait<
    type<T, first_op, ops...>, 
    std::enable_if_t<!impl::is_fields_def<first_op> && !std::is_same_v<first_op, with_fields::use_default>>>
: trait<type<T, with_fields::use_default, first_op, ops...>>
{
};

template<typename T, typename fields_def, typename... ops>
struct trait<
    type<T, fields_def, ops...>, 
    std::enable_if_t<impl::is_fields_def<fields_def>>>
: impl::trait<T, impl::described_vst_helper<fields_def>, ops...>
{
    static_assert(has_correct_get_fields<fields_def, T>, "'get_fields' must return a tuple of pointer to members or named_field_ptr");
};

} // namespace vst