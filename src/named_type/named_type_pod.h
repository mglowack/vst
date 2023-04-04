#pragma once

#include <tuple>

#include <named_type_conversions_category.h>
#include <type_list.h>
template<typename T, typename list>
constexpr bool is_implicitly_convertible_to = type_list_contains_v<list, implicit_conversions_to<T>>;
template<typename T, typename list>
constexpr bool is_implicitly_convertible_from = type_list_contains_v<list, implicit_conversions_from<T>>;

// ##################
// # named_type_pod #
// ##################

template<typename underlying_t, typename tag_t, typename op_categories_t, typename conversion_categories_t>
struct named_type_pod
{
    underlying_t value;

    explicit(!is_implicitly_convertible_from<underlying_t, conversion_categories_t>)
    constexpr named_type_pod(underlying_t value) : value(value) {}

    explicit(!is_implicitly_convertible_to<underlying_t, conversion_categories_t>)
    constexpr operator const underlying_t&() const { return value; }

    explicit(!is_implicitly_convertible_to<underlying_t, conversion_categories_t>)
    constexpr operator underlying_t&()             { return value; }

    constexpr const underlying_t& get()               const { return value; }
    constexpr underlying_t& get()                           { return value; }

    static constexpr auto get_fields()
    {
        return std::tuple{&named_type_pod<underlying_t, tag_t, op_categories_t, conversion_categories_t>::value};
    }
};
