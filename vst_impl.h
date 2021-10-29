#ifndef VST_IMPL_H
#define VST_IMPL_H

#include "vst_defs.h"
#include "vst_utils.h"
#include "type_list.h"

#include <string>
#include <memory>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <iostream>
#include <type_traits>
#include <cassert>

#include <boost/functional/hash.hpp>
#include <boost/pfr.hpp>

struct non_matchable {};
inline void show_type(non_matchable) {}

// #################
// # wrapped_value #
// #################

template<typename T, typename ENABLER = void>
struct wrapped_value
{
    const T& value;

    constexpr explicit wrapped_value(const T& value) 
    : value(value) {}
};

template<typename P, typename T, typename ENABLER = void>
struct wrapped_value_of : wrapped_value<T>
{
    using wrapped_value<T>::wrapped_value;
};

// defaults
template<typename T>
constexpr bool operator==(const wrapped_value<T>& lhs, const wrapped_value<T>& rhs)
{
    return lhs.value == rhs.value;
}

template<typename T>
constexpr bool operator<(const wrapped_value<T>& lhs, const wrapped_value<T>& rhs)
{
    return lhs.value < rhs.value;
}

template<typename T>
constexpr T operator+(const wrapped_value<T>& lhs, const wrapped_value<T>& rhs)
{
    return lhs.value + rhs.value;
}

template <typename T>
std::size_t hash_value(const wrapped_value<T>& v)
{
    using boost::hash_value;
    return hash_value(v.value);
}

template<typename T>
std::ostream& operator<<(std::ostream& os, const wrapped_value<T>& rhs)
{
    return os << rhs.value;
}

// overrides - const char*
inline
bool operator==(const wrapped_value<const char*>& lhs, const wrapped_value<const char*>& rhs)
{
    return strcmp(lhs.value, rhs.value) == 0;
}

inline
bool operator<(const wrapped_value<const char*>& lhs, const wrapped_value<const char*>& rhs)
{
    return strcmp(lhs.value, rhs.value) < 0;
}

// ###################
// # named_field_ptr #
// ###################

template<typename field_ptr_t>
struct named_field_ptr
{
    const char* name;
    field_ptr_t field_ptr;

    constexpr explicit named_field_ptr(const char* name, field_ptr_t field_ptr) 
    : name(name), field_ptr(field_ptr) {}
};

#define MEMBER(obj, x) named_field_ptr{#x, &obj::x}

// #############
// # named_var #
// #############

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

template<typename P, typename T, typename ENABLER = void>
struct named_var_of : named_var<T> 
{
    using named_var<T>::named_var;
};

template<typename T>
std::ostream& operator<<(std::ostream& os, const named_var<T>& rhs)
{
    return os << rhs.name << "=" << rhs.value;
}

// ###############
// # indexed_var #
// ###############

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

namespace vst::impl {

// ##########
// # helper #
// ##########

struct helper 
{
    template<typename T, typename op>
    static constexpr bool has_op() { return type_list_contains_v<typename trait<T>::properties, op>; }

    template<typename T>
    static constexpr auto tie(T& obj)
    {
        using vst_t = std::decay_t<T>;
        if constexpr (has_get_fields<trait<vst_t>>) 
        {
            return tie(obj, trait<vst_t>::get_fields());
        }
        else
        {
            return boost::pfr::structure_tie(as_aggregate(obj));
        }
    }

    template<typename T>
    static constexpr auto wrapped_tie(T& obj)
    {
        using vst_t = std::decay_t<T>;
        return wrapped_tie<vst_t>(tie(obj));
    }

    template<typename T>
    static constexpr auto named_tie(T& obj)
    {
        using vst_t = std::decay_t<T>;
        if constexpr (has_get_fields<trait<vst_t>>) 
        {
            return named_tie<vst_t>(obj, trait<vst_t>::get_fields());
        }
        else
        {
            return named_tie<vst_t>(boost::pfr::structure_tie(as_aggregate(obj)));
        }
    }

private:
    template<typename T, typename... field_ptrs_t>
    static constexpr auto tie(T& obj, const std::tuple<field_ptrs_t...>& fields)
    {
        return std::apply(
            [&obj](const auto&... f) { 
                return std::tie(as_ref_to_value(obj, f)...); 
            }, 
            fields);
    }

    template<typename vst_t, typename... Ts>
    static constexpr auto wrapped_tie(std::tuple<Ts&...> fields)
    {
        return std::apply(
            [](auto&... f) { 
                return std::tuple(wrapped_value_of<vst_t, std::remove_const_t<Ts>>{f}...); 
            }, 
            fields);
    }

    template<typename vst_t, typename T, typename... field_ptrs_t>
    static constexpr auto named_tie(
        T& obj, const std::tuple<named_field_ptr<field_ptrs_t>...>& fields)
    {
        return std::apply(
            [&obj](const auto&... f) { 
                return std::tuple(as_named_var<vst_t>(f.name, obj.*f.field_ptr)...); 
            }, 
            fields);
    }

    // NOTE: this overload takes over if fields ARE NOT named_field_ptr<T> i.e. no names were specified
    template<typename vst_t, typename T, typename... field_ptrs_t>
    static constexpr auto named_tie(T& obj, const std::tuple<field_ptrs_t...>& fields)
    {
        return named_tie<vst_t>(tie(obj, fields));
    }

    template<typename vst_t, typename... Ts>
    static constexpr auto named_tie(std::tuple<Ts&...> fields)
    {
        return apply_with_index(
            [](const auto&... elem) { 
                return std::tuple(as_indexed_var<vst_t, elem.index + 1>(elem.value)...); // convert to 1-based
            }, 
            fields);
    }

    template<typename T, typename field_ptr_t>
    static constexpr decltype(auto) as_ref_to_value(T& obj, field_ptr_t f)
    {
        return obj.*f;
    }

    template<typename T, typename field_ptr_t>
    static constexpr decltype(auto) as_ref_to_value(T& obj, const named_field_ptr<field_ptr_t>& f)
    {
        return obj.*f.field_ptr;
    }

    template<typename vst_t, std::size_t I, typename T>
    static constexpr auto as_indexed_var(const T& var)
    {
        return indexed_var<wrapped_value_of<vst_t, T>, I>{wrapped_value_of<vst_t, T>{var}};
    }

    template<typename vst_t, typename T>
    static constexpr auto as_named_var(const char* name, const T& var)
    {
        return named_var_of<vst_t, wrapped_value_of<vst_t, T>>{name, wrapped_value_of<vst_t, T>{var}};
    }
    
    template<typename T>
    static constexpr decltype(auto) as_aggregate(T& obj)
    {
        return static_cast<propagate_const_t<T, aggregate_t<std::remove_const_t<T>>>&>(obj);
    }
};

} // close vst::impl namespace

namespace vst {

// #########
// # trait #
// #########

template<typename T>
struct trait<type<T>> : trait<type<T, with_fields::inferred>>
{
};

template<typename T, typename maybe_field_def, typename... ops>
struct trait<
    type<T, maybe_field_def, ops...>, 
    std::enable_if_t<!is_fields_def<maybe_field_def>>>
: trait<type<T, with_fields::inferred, maybe_field_def, ops...>>
{
};

template<typename T, typename... ops>
struct trait<type<T, with_fields::inferred, ops...>, std::enable_if_t<has_get_fields<T>>>
: trait<type<T, with_fields::from<T>, ops...>>
{
};

template<typename T, typename... ops>
struct trait<type<T, with_fields::inferred, ops...>, std::enable_if_t<!has_get_fields<T>>>
: make_basic_trait<T, ops...>
{
};

template<typename T, typename maybe_field_def, typename... ops>
struct trait<
    type<T, maybe_field_def, ops...>, 
    std::enable_if_t<
        is_fields_def<maybe_field_def> 
        && !std::is_same_v<maybe_field_def, with_fields::inferred>>>
: make_basic_trait<T, ops...>
, maybe_field_def
{
};

} // namespace vst

// #############
// # operators #
// #############

namespace vst::impl {

// comparable
template<typename T, std::enable_if_t<vst::trait<T>::exists, int> = 0>
constexpr bool operator==(const T& lhs, const T& rhs)
{
    return helper::wrapped_tie(lhs) == helper::wrapped_tie(rhs);
}

template<typename T, std::enable_if_t<vst::trait<T>::exists, int> = 0>
constexpr bool operator!=(const T& lhs, const T& rhs)
{
    return !(lhs == rhs);
}

// ordered
template<
    typename T, 
    std::enable_if_t<vst::trait<T>::exists && helper::has_op<T, vst::op::ordered>(), int> = 0>
constexpr bool operator<(const T& lhs, const T& rhs)
{
    return helper::wrapped_tie(lhs) < helper::wrapped_tie(rhs);
}

template<
    typename T, 
    std::enable_if_t<vst::trait<T>::exists && helper::has_op<T, vst::op::ordered>(), int> = 0>
constexpr bool operator<=(const T& lhs, const T& rhs)
{
    return lhs < rhs || lhs == rhs;
}

template<
    typename T, 
    std::enable_if_t<vst::trait<T>::exists && helper::has_op<T, vst::op::ordered>(), int> = 0>
constexpr bool operator>(const T& lhs, const T& rhs)
{
    return !(lhs <= rhs);
}

template<
    typename T, 
    std::enable_if_t<vst::trait<T>::exists && helper::has_op<T, vst::op::ordered>(), int> = 0>
constexpr bool operator>=(const T& lhs, const T& rhs)
{
    return !(lhs < rhs);
}

// TODO MG: move out
template<typename = void>
struct plus_assign
{
    template<typename T>
    [[maybe_unused]] T& operator()(T& lhs, const T& rhs)
    {
        return lhs += rhs;
    }
};

template<typename = void>
struct minus_assign
{
    template<typename T>
    [[maybe_unused]] T& operator()(T& lhs, const T& rhs)
    {
        return lhs -= rhs;
    }
};

template <typename op_t, typename vst_t>
constexpr vst_t& binary_assign_op(vst_t& lhs, const vst_t& rhs)
{
    apply_with_index([rhs_tie = helper::tie(rhs)](auto&&... a) {
        (op_t{}(
            a.value,
            std::get<a.index>(rhs_tie)), ...);
    }, helper::tie(lhs));
    return lhs;
}

template <typename op_t, typename vst_t>
constexpr vst_t binary_op(const vst_t& lhs, const vst_t& rhs)
{
    return apply_with_index(
        [lhs_tie = helper::tie(lhs), rhs_tie = helper::tie(rhs)]
        (auto&&... a) {
        return vst_t{op_t{}(
            std::get<a.index>(lhs_tie),
            std::get<a.index>(rhs_tie))...};
    }, helper::tie(lhs));
}

template<
    typename T, 
    std::enable_if_t<vst::trait<T>::exists && helper::has_op<T, vst::op::addable>(), int> = 0>
constexpr T operator+(const T& lhs, const T& rhs)
{
    return binary_op<std::plus<>>(lhs, rhs);
}
template<
    typename T, 
    std::enable_if_t<vst::trait<T>::exists && helper::has_op<T, vst::op::addable>(), int> = 0>
constexpr T operator-(const T& lhs, const T& rhs)
{
    return binary_op<std::minus<>>(lhs, rhs);
}
template<
    typename T, 
    std::enable_if_t<vst::trait<T>::exists && helper::has_op<T, vst::op::addable>(), int> = 0>
constexpr T& operator+=(T& lhs, const T& rhs)
{
    return binary_assign_op<plus_assign<>>(lhs, rhs);
}
template<
    typename T, 
    std::enable_if_t<vst::trait<T>::exists && helper::has_op<T, vst::op::addable>(), int> = 0>
constexpr T& operator-=(T& lhs, const T& rhs)
{
    return binary_assign_op<minus_assign<>>(lhs, rhs);
}

// stream
template<typename T, std::enable_if_t<vst::trait<T>::exists, int> = 0>
std::ostream& operator<<(std::ostream& os, const T& rhs)
{
    os << "[";
    std::apply(
        [&os, &rhs](const auto&... field){
            ((os << " " << field), ...);
        }, helper::named_tie(rhs));
    os << " ]";
    return os;
}

} // namespace vst::impl

// ############
// # hashable #
// ############

namespace vst
{

template<typename T, typename ENABLER = void>
struct hash;

template<typename T>
struct hash<T, std::enable_if_t<trait<T>::exists && impl::helper::has_op<T, op::hashable>()>>
{
    size_t operator()(const T& o) const noexcept
    {
        // use boost helper for hash of tuples
        return boost::hash_value(impl::helper::tie(o));
    }
};

} // namespace vst

namespace vst::impl {

template <typename T, std::enable_if_t<trait<T>::exists && helper::has_op<T, op::hashable>(), int> = 0>
std::size_t hash_value(const T& o)
{
    return hash<T>{}(o);
}

} // namespace vst::impl

namespace std
{

template<typename... args_t>
struct hash<vst::impl::type<args_t...>> : vst::hash<vst::impl::type<args_t...>>
{
    using checkIfHashable = std::enable_if_t<
        vst::impl::helper::has_op<vst::impl::type<args_t...>, vst::op::hashable>()>;
};

} // namespace std

#endif