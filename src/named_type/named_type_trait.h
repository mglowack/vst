#pragma once

#include <named_type_ops_category.h>
#include <named_type_conversions_category.h>

#include "vst.hpp"
#include "type_list.h"

// ####################
// # named_type_trait #
// ####################

template<
    typename underlying_t,
    typename tag_t,
    typename op_categories_t,
    typename conversion_categories_t>
struct named_type_pod;

template<typename T>
struct named_type_trait;

template<
    typename underlying_t,
    typename tag_t,
    typename op_categories_t,
    typename conversion_categories_t>
struct named_type_trait<named_type_pod<underlying_t, tag_t, op_categories_t, conversion_categories_t>>
{
    static constexpr bool exists = true;

    using underlying_type = underlying_t;
    using op_categories = op_categories_t;
    using conversion_categories = conversion_categories_t;

    template<typename T>
    static constexpr bool is_transparent_with
        = type_list_contains_v<op_categories, transparent_ops_with<T>>;

    template<typename T>
    static constexpr bool is_implicitly_convertible_to
        = type_list_contains_v<conversion_categories, implicit_conversions_to<T>>;

    template<typename T>
    static constexpr bool is_implicitly_convertible_from
        = type_list_contains_v<conversion_categories, implicit_conversions_from<T>>;
};

template<
    typename underlying_t,
    typename tag_t,
    typename op_categories_t,
    typename conversion_categories_t,
    typename... ops>
struct named_type_trait<vst::type<named_type_pod<underlying_t, tag_t, op_categories_t, conversion_categories_t>, ops...>>
: named_type_trait<named_type_pod<underlying_t, tag_t, op_categories_t, conversion_categories_t>> {};

template<typename T>
concept NamedType = named_type_trait<T>::exists;

template<typename U, typename T>
concept TransparentWith = NamedType<T> && named_type_trait<T>::template is_transparent_with<U>;

template<typename U, typename T>
concept ImplicitlyConvertibleTo = NamedType<T> && named_type_trait<T>::template is_implicitly_convertible_to<U>;

template<typename U, typename T>
concept ImplicitlyConvertibleFrom = NamedType<T> && named_type_trait<T>::template is_implicitly_convertible_from<U>;
