#pragma once

#include <named_type_trait.h>

#include <tuple>

// ##################
// # named_type_pod #
// ##################

template<
    typename underlying_t,
    typename tag_t,
    typename op_categories_t,
    typename conversion_categories_t>
struct named_type_pod
{
    using self = named_type_pod<underlying_t, tag_t, op_categories_t, conversion_categories_t>;

    underlying_t value;

    explicit(!ImplicitlyConvertibleFrom<underlying_t, self>)
    constexpr named_type_pod(underlying_t value) : value(value) {}

    template<ImplicitlyConvertibleFrom<self> T>
    // explicit(!ImplicitlyConvertibleFrom<T, self>)
    constexpr named_type_pod(T value) : value(value) {}

    template<ImplicitlyConvertibleTo<self> T>
    constexpr operator T() const { return value; }

    explicit(!ImplicitlyConvertibleTo<underlying_t, self>)
    constexpr operator const underlying_t&() const { return value; }

    explicit(!ImplicitlyConvertibleTo<underlying_t, self>)
    constexpr operator underlying_t&()             { return value; }

    constexpr const underlying_t& get()      const { return value; }
    constexpr underlying_t& get()                  { return value; }

    static constexpr auto get_fields()
    {
        return std::tuple{&self::value};
    }
};
