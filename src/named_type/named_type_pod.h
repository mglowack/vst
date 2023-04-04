#pragma once

#include <named_type_trait.h>

#include <tuple>

// ##################
// # named_type_pod #
// ##################

template<typename underlying_t, typename tag_t, typename op_categories_t, typename conversion_categories_t>
struct named_type_pod
{
    using self = named_type_pod<underlying_t, tag_t, op_categories_t, conversion_categories_t>;

    underlying_t value;

    explicit(!named_type_trait<self>::template is_implicitly_convertible_from<underlying_t>)
    constexpr named_type_pod(underlying_t value) : value(value) {}

    explicit(!named_type_trait<self>::template is_implicitly_convertible_to<underlying_t>)
    constexpr operator const underlying_t&() const { return value; }

    explicit(!named_type_trait<self>::template is_implicitly_convertible_to<underlying_t>)
    constexpr operator underlying_t&()             { return value; }

    constexpr const underlying_t& get()               const { return value; }
    constexpr underlying_t& get()                           { return value; }

    static constexpr auto get_fields()
    {
        return std::tuple{&self::value};
    }
};
