#pragma once

#include <tuple>

// ##################
// # named_type_pod #
// ##################

template<typename underlying_t, typename tag_t, typename ops_categories_t>
struct named_type_pod
{
    underlying_t value;

    explicit constexpr named_type_pod(underlying_t value) : value(value) {}

    explicit constexpr operator const underlying_t&() const { return value; }
    explicit constexpr operator underlying_t&()             { return value; }

    constexpr const underlying_t& get()               const { return value; }
    constexpr underlying_t& get()                           { return value; }

    static constexpr auto get_fields()
    {
        return std::tuple{&named_type_pod<underlying_t, tag_t, ops_categories_t>::value};
    }
};
