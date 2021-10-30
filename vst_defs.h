#ifndef VST_DEFS_H
#define VST_DEFS_H

#include "type_list.h"

#include <tuple>

#include <boost/functional/hash.hpp>

namespace vst {

namespace op {
    struct ordered;
    struct hashable;
    struct addable;
}

namespace impl {

template <typename T, typename properties_t, typename ENABLER = void>
struct type
: public T
{
    using T::T;
};

template <typename T, typename properties_t>
struct type<T, properties_t, std::enable_if_t<std::is_aggregate_v<T>>>
: public T
{
};

} // close impl namespace

template <typename T, typename... ops>
using type = impl::type<T, type_list<ops...>>;

template<typename T, typename ENABLER = void>
struct trait;

// template<typename T, typename ENABLER = void>
// struct hash;

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

struct from_aggregate {};

struct inferred {};

} // namespace fields

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