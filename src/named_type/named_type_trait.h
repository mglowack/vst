#pragma once

#include <named_type_ops_category.h>
#include <named_type_conversions_category.h>

#include "vst.hpp"
#include "type_list.h"

// ###############################
// # allows_transparent_ops_with #
// ###############################

template<typename T>
struct allows_transparent_ops_with
{
    template<typename ops_category_t>
    struct pred : std::false_type {};

    template<typename U>
    struct pred<transparent_ops_with<U>> : std::bool_constant<std::is_same_v<T, U>> {};
};

static_assert( type_list_any_v<type_list<transparent_ops_with<int>>, allows_transparent_ops_with<int>::pred>);
static_assert( type_list_any_v<type_list<transparent_ops_with<int>, transparent_ops_with<float>>, allows_transparent_ops_with<float>::pred>);
static_assert(!type_list_any_v<type_list<>, allows_transparent_ops_with<int>::pred>);
static_assert(!type_list_any_v<type_list<strict_ops>, allows_transparent_ops_with<int>::pred>);

// ####################
// # named_type_trait #
// ####################

template<typename underlying_t, typename tag_t, typename op_categories_t, typename conversion_categories_t>
struct named_type_pod;

template<typename T>
struct named_type_trait;

template<typename underlying_t, typename tag_t, typename op_categories_t, typename conversion_categories_t>
struct named_type_trait<named_type_pod<underlying_t, tag_t, op_categories_t, conversion_categories_t>>
{
    static constexpr bool exists = true;

    using underlying_type = underlying_t;
    using op_categories = op_categories_t;
    using conversion_categories = conversion_categories_t;

    template<typename T>
    static constexpr bool is_transparent_with
        = type_list_any_v<op_categories, allows_transparent_ops_with<T>::template pred>;

    static constexpr bool is_transparent = is_transparent_with<underlying_type>;

    template<typename T>
    static constexpr bool is_implicitly_convertible_to
        = type_list_contains_v<conversion_categories, implicit_conversions_to<T>>;

    template<typename T>
    static constexpr bool is_implicitly_convertible_from
        = type_list_contains_v<conversion_categories, implicit_conversions_from<T>>;
};

template<typename underlying_t, typename tag_t, typename op_categories_t, typename conversion_categories_t, typename... ops>
struct named_type_trait<vst::type<named_type_pod<underlying_t, tag_t, op_categories_t, conversion_categories_t>, ops...>>
: named_type_trait<named_type_pod<underlying_t, tag_t, op_categories_t, conversion_categories_t>> {};

template<typename T>
concept NamedType = named_type_trait<T>::exists;

template<typename T>
concept Transparent = named_type_trait<T>::exists && named_type_trait<T>::is_transparent;

template<typename U, typename T>
concept TransparentWith = named_type_trait<T>::exists && named_type_trait<T>::template is_transparent_with<U>;

template<typename U, typename T>
concept ImplicitlyConvertibleTo = named_type_trait<T>::template is_implicitly_convertible_to<U>;

template<typename U, typename T>
concept ImplicitlyConvertibleFrom = named_type_trait<T>::template is_implicitly_convertible_from<U>;
