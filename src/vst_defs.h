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
struct member_of
{
    template<typename ptr_t>
    struct is_pointer_to_member : std::false_type {};

    template<typename X>
    struct is_pointer_to_member<X (T::*)> : std::true_type {};

    template<typename ptr_t>
    static constexpr bool is_pointer_to_member_v = is_pointer_to_member<ptr_t>::value;
};

template<template<typename> typename checker_t, typename T, typename ENABLER = std::void_t<>>
constexpr bool is_tuple_of = false;

template<template<typename> typename checker_t, typename... ptrs_t>
constexpr bool is_tuple_of<checker_t, std::tuple<ptrs_t...>> = (checker_t<ptrs_t>::value && ...);

template<template<typename> typename checker_t, typename... ptrs_t>
constexpr bool is_tuple_of<checker_t, std::tuple<named_field_ptr<ptrs_t>...>> = (checker_t<ptrs_t>::value && ...);

template<typename T, typename U, typename ENABLER = std::void_t<>>
constexpr bool has_correct_get_fields = false;

template<typename T, typename U>
constexpr bool has_correct_get_fields<
    T, U,
    std::void_t<decltype(T::get_fields())>>
= is_tuple_of<member_of<U>::template is_pointer_to_member, decltype(T::get_fields())>;

namespace vst {

namespace op {
    struct ordered;
    struct hashable;
    struct addable;
}

namespace impl {

template <typename T, typename properties_t, typename ENABLER = void>
struct type
: public T
{
    using T::T;

    // friend bool operator==(const type&, const type&) = default;
};

template <typename T, typename properties_t>
struct type<T, properties_t, std::enable_if_t<std::is_aggregate_v<T>>>
: public T
{
    // friend bool operator==(const type&, const type&) = default;
};

} // close impl namespace

template <typename T, typename... ops>
using type = impl::type<T, type_list<ops...>>;

template<typename T, typename ENABLER = void>
struct trait;

template <typename T>
concept Type = trait<T>::exists;

template <typename T, typename OP>
concept OpEnabled = Type<T> && type_list_contains_v<typename trait<T>::properties, OP>;

// template<typename T, typename ENABLER = void>
// struct hash;

// template<typename T>
// struct hash {};

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