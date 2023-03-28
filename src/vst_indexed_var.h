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

template<typename vst_t, typename T, std::size_t I>
constexpr auto wrap(const indexed_var<T, I>& var)
{
    return indexed_var<wrapped_value_of<vst_t, T>, I>{wrapped_value_of<vst_t, T>{var.value}};
}

struct indexed_var_util
{
    template<typename... Ts>
    static constexpr auto tie(std::tuple<Ts&...> fields)
    {
        return apply_with_index(
            [](const auto&... elem) {
                return std::tuple(make<elem.index + 1>(elem.value)...); // convert to 1-based
            },
            fields);
    }

    template<std::size_t I, typename T>
    static constexpr auto make(const T& value)
    {
        return indexed_var<T, I>{value};
    }
};

}