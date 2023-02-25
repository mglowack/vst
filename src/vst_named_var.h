#pragma once

#include <vst_wrapped_value.h>

namespace vst {
    
template<typename T>
struct named_var
{
    const char* name;
    const T& value;

    constexpr explicit named_var(const char* name, const T& value) 
    : name(name), value(value) {}
};

template<typename T>
struct named_var<wrapped_value<T>>
{
    const char* name;
    wrapped_value<T> value;

    constexpr explicit named_var(const char* name, wrapped_value<T> value) 
    : name(name), value(value) {}
};

template<typename P, typename T>
struct named_var<wrapped_value_of<P, T>>
{
    const char* name;
    wrapped_value_of<P, T> value;

    constexpr explicit named_var(const char* name, wrapped_value_of<P, T> value) 
    : name(name), value(value) {}
};

template<typename T>
std::ostream& operator<<(std::ostream& os, const named_var<T>& rhs)
{
    return os << rhs.name << "=" << rhs.value;
}

}