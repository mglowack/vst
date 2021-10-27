#ifndef VST_DEFS_H
#define VST_DEFS_H

#include "type_list.h"

#include <tuple>

namespace vst {

namespace op {
    struct ordered;
    struct hashable;
}

namespace with_fields {

template<auto (*get_fields_func)()>
struct from_func
{
    static constexpr auto get_fields()
    {
        return get_fields_func();
    }
};

template<auto fields>
struct from_var
{
    static constexpr auto get_fields()
    {
        return *fields;
    }
};

template<typename T>
struct from
{
    static constexpr auto get_fields()
    {
        return T::get_fields();
    }
};

struct empty
{
    static constexpr auto get_fields()
    {
        return std::tuple{};
    }
};

struct inferred {};

} // namespace fields

template<typename T, typename fields_def, typename... ops>
struct type_impl : public T
{
};

template<typename T, typename... ops>
struct type : type_impl<T, with_fields::inferred, ops...>
{
};

template<typename T, typename ENABLER = void>
struct trait;

template<typename... ops>
struct make_basic_trait
{
    static constexpr bool exists = true;
    using properties = type_list<ops...>;
};

template <typename T, typename... ops>
struct make_trait 
: make_basic_trait<ops...>
, with_fields::from<T> 
{
};

} // close vst namespace

#endif