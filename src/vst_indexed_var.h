#pragma once

#include <vst_wrapped_value.h>

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

}