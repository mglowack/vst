#pragma once

#include <vst_wrapped_value.h>
#include <vst_utils.h>

#include <tuple>

namespace vst {

template<typename T, std::size_t I>
struct indexed_var
{
    static constexpr std::size_t index = I;
    const T& value;
};

template<typename T, std::size_t I>
struct indexed_var<wrapped_value<T>, I>
{
    static constexpr std::size_t index = I;
    wrapped_value<T> value;
};

template<typename P, typename T, std::size_t I>
struct indexed_var<wrapped_value_of<P, T>, I>
{
    static constexpr std::size_t index = I;
    wrapped_value_of<P, T> value;
};

template<typename T, std::size_t I>
std::ostream& operator<<(std::ostream& os, const indexed_var<T, I>& rhs)
{
    return os << "field" << rhs.index << "=" << rhs.value;
}

struct indexed_var_util
{
    template<typename vst_t, typename... Ts>
    static constexpr auto tie(std::tuple<Ts&...> fields)
    {
        return apply_with_index(
            [](const auto&... elem) {
                return std::tuple(as_indexed_var<vst_t>(elem)...);
            },
            fields);
    }

    template<typename vst_t, std::size_t I, typename T>
    static constexpr auto as_indexed_var(const value_with_index<I, T>& var)
    {
        using U = std::remove_const_t<T>;
        return indexed_var<wrapped_value_of<vst_t, U>, I+1>{ // convert to 1-based
            wrapped_value_of<vst_t, U>{var.value}};
    }
};

}