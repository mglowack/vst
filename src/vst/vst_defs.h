#ifndef VST_DEFS_H
#define VST_DEFS_H

#include "type_list.h"

#include <tuple>
#include <vector>
#include <utility>

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

// ######################
// # Described #
// ######################

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



template<typename spec_t, typename T>
concept FieldSpecOf =
    is_template_v<std::tuple, spec_t>
    && (type_list_all_v<template_cast_t<type_list, spec_t>, is_pointer_to_member_of<T>::template pred>
      || type_list_all_v<template_cast_t<type_list, spec_t>, is_named_field_ptr_of<T>::template pred>);

template<typename T, typename U>
concept DescribesThe = requires {
    { T::get_fields() } -> FieldSpecOf<U>;
};

template<typename T>
concept SelfDescribed = DescribesThe<T, T>;

namespace vst {

namespace op {
    struct ordered;
    struct hashable;
    struct addable;
}

template<typename T>
struct is_op : type_list_contains<type_list<op::ordered, op::hashable, op::addable>, T> {};

template<typename T>
constexpr bool is_op_v = is_op<T>::value;

template<typename T>
concept Operator = is_op_v<T>;

static_assert(!is_op_v<int>);
static_assert(!is_op_v<float>);
static_assert( is_op_v<op::ordered>);
static_assert( is_op_v<op::hashable>);
static_assert( is_op_v<op::addable>);

namespace impl {

template <typename T, typename properties_t>
struct type
: public T
{
    using T::T;
};

template <typename T, typename properties_t> requires std::is_aggregate_v<T>
struct type<T, properties_t>
: public T {};

} // close impl namespace

template <typename T, typename... ops>
using type = impl::type<T, type_list<ops...>>;

template<typename T>
struct trait;

template <typename T>
concept Type = requires {
    typename trait<T>::pod_t;
    typename trait<T>::fields_def;
    typename trait<T>::properties;
};

template <typename T, typename OP>
concept OpEnabled = Type<T> && type_list_contains_v<typename trait<T>::properties, OP>;

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
        static_assert(SelfDescribed<T>, "T must be an aggregate or have 'get_fields' defined.");
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

namespace Described_tests {

struct empty {};
static_assert(!SelfDescribed<empty>);

struct simple_empty_fields {
    static auto get_fields() {
        return std::tuple{};
    }
};
static_assert( SelfDescribed<simple_empty_fields>);
static_assert( DescribesThe<with_fields::empty, simple_empty_fields>);

struct simple_with_fields {
    int i;
    float f;
    static auto get_fields() {
        return std::tuple{&simple_with_fields::i, &simple_with_fields::f};
    }
};
static_assert( SelfDescribed<simple_with_fields>);
static_assert( DescribesThe<with_fields::from<simple_empty_fields>, simple_empty_fields>);

struct simple {
    int i;
    float f;
};
struct other {
    int i;
    float f;
};
static_assert(!FieldSpecOf<std::pair<int, float>,                                                                    simple>);
static_assert(!FieldSpecOf<std::vector<int>,                                                                         simple>);
static_assert(!FieldSpecOf<std::variant<int, float>,                                                                 simple>);
static_assert( FieldSpecOf<std::tuple<>,                                                                             simple>);
static_assert( FieldSpecOf<std::tuple<decltype(&simple::i)>,                                                         simple>);
static_assert(!FieldSpecOf<std::tuple<decltype(&other::i)>,                                                          simple>);
static_assert(!FieldSpecOf<std::tuple<decltype(&simple::i), decltype(&other::f)>,                                    simple>);
static_assert( FieldSpecOf<std::tuple<decltype(&simple::i), decltype(&simple::f)>,                                   simple>);
static_assert( FieldSpecOf<std::tuple<named_field_ptr<decltype(&simple::i)>, named_field_ptr<decltype(&simple::f)>>, simple>);
static_assert(!FieldSpecOf<std::tuple<named_field_ptr<decltype(&simple::i)>, decltype(&simple::f)>,                  simple>);
static_assert(!FieldSpecOf<std::tuple<decltype(&simple::i), named_field_ptr<decltype(&simple::f)>>,                  simple>);

}

} // close vst namespace

#endif