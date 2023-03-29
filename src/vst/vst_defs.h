#ifndef VST_DEFS_H
#define VST_DEFS_H

#include "type_list.h"

#include <tuple>

// ###################
// # named_field_ptr #
// ###################

template<typename field_ptr_t>
struct named_field_ptr
{
    const char* name;
    field_ptr_t field_ptr;

    constexpr explicit named_field_ptr(const char* name, field_ptr_t field_ptr)
    : name(name), field_ptr(field_ptr) {}
};

#define MEMBER(obj, x) named_field_ptr{#x, &obj::x}


// ##########################
// # has_correct_get_fields #
// ##########################

template<typename T>
struct is_pointer_to_member_of
{
    template<typename ptr_t>
    struct pred : std::false_type {};

    template<typename X>
    struct pred<X (T::*)> : std::true_type {};
};

template<typename T>
struct is_named_field_ptr_of
{
    template<typename ptr_t>
    struct pred : std::false_type {};

    template<typename X>
    struct pred<named_field_ptr<X (T::*)>> : std::true_type {};
};



template<template<typename> typename... preds_t>
struct disjunction
{
    template<typename T>
    using pred = std::disjunction<preds_t<T>...>;
};



template<template<typename...> typename template_t, typename T>
struct is_template : std::false_type {};

template<template<typename...> typename template_t, typename T>
constexpr bool is_template_v =  is_template<template_t, T>::value;

template<template<typename...> typename template_t, typename... args_t>
struct is_template<template_t, template_t<args_t...>> : std::true_type {};



template<typename T, typename U, typename ENABLER = std::void_t<>>
constexpr bool has_correct_get_fields = false;

template<typename T, typename U>
constexpr bool has_correct_get_fields<
    T, U,
    std::void_t<decltype(T::get_fields())>>
= is_template_v<std::tuple, decltype(T::get_fields())>
    && type_list_all_v<
        type_list_cast_t<decltype(T::get_fields())>,
        disjunction<
            is_pointer_to_member_of<U>::template pred,
            is_named_field_ptr_of<U>::template pred>::template pred>;
// =    type_list_all_v<list_from_tuple_t<decltype(T::get_fields())>, is_pointer_to_member_of<U>::template pred>
//   || type_list_all_v<list_from_tuple_t<decltype(T::get_fields())>, is_named_field_ptr_of<U>::template pred>;

namespace vst {

namespace op {
    struct ordered;
    struct hashable;
    struct addable;
}

namespace impl {

template<typename T>
concept Aggregate = std::is_aggregate_v<T>;

template <typename T, typename properties_t>
struct type
: public T
{
    using T::T;
};

template <Aggregate T, typename properties_t>
struct type<T, properties_t>
: public T {};

} // close impl namespace

template <typename T, typename... ops>
using type = impl::type<T, type_list<ops...>>;

template<typename T, typename ENABLER = void>
struct trait;

template <typename T>
concept Type = trait<T>::exists;

template <typename T, typename OP>
concept OpEnabled = Type<T> && type_list_contains_v<typename trait<T>::properties, OP>;

// ##################
// # has_get_fields #
// ##################

template<typename T, typename ENABLER = std::void_t<>>
constexpr bool has_get_fields = false;

template<typename T>
constexpr bool has_get_fields<
    T,
    std::void_t<decltype(T::get_fields())>>
= true;

namespace with_fields {

template<auto (*get_fields_func)()>
struct from_func
{
    static constexpr auto get_fields()
    {
        return get_fields_func();
    }
};

template<auto fields>
struct from_var
{
    static constexpr auto get_fields()
    {
        return *fields;
    }
};

template<typename T>
struct from
{
    static constexpr auto get_fields()
    {
        static_assert(has_get_fields<T>, "T must be an aggregate or have 'get_fields' defined.");
        return T::get_fields();
    }
};

struct empty
{
    static constexpr auto get_fields()
    {
        return std::tuple{};
    }
};

struct use_default {};

struct from_aggregate {};

} // namespace fields

} // close vst namespace

#endif