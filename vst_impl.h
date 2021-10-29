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

// #############
// # field_ptr #
// #############

template<typename field_ptr_t>
struct field_ptr
{
    field_ptr_t p;

    constexpr explicit field_ptr(field_ptr_t field_ptr) 
    : p(field_ptr) {}
};

// #define MEMBER(obj, x) field_ptr{&obj::x}

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

    constexpr explicit indexed_var(const T& value) 
    : value(value) {}
};

template<typename T, std::size_t I>
std::ostream& operator<<(std::ostream& os, const indexed_var<T, I>& rhs)
{
    return os << "field" << rhs.index << "=" << rhs.value;
}

// #################
// # wrapped_value #
// #################

template<typename T, typename ENABLER = void>
struct wrapped_value
{
    const T& value;
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
        return wrapped_tie(obj);
    }

    template<typename T>
    static constexpr auto wrapped_tie(T& obj)
    {
        return wrap(raw_tie(obj));
    }

    template<typename T>
    static constexpr auto raw_tie(T& obj)
    {
        using trait_t = trait<std::decay_t<T>>;
        if constexpr (has_get_fields<trait_t>) 
        {
            return from_fields_tie(obj, trait_t::get_fields());
        }
        else
        {
            return boost::pfr::structure_tie(as_aggregate(obj));
        }
    }

    template<typename T>
    static constexpr auto named_tie(T& obj)
    {
        using trait_t = trait<std::decay_t<T>>;
        if constexpr (has_get_fields<trait_t>) 
        {
            return from_fields_named_tie(obj, trait_t::get_fields());
        }
        else
        {
            return from_values_named_tie(boost::pfr::structure_tie(as_aggregate(obj)));
        }
    }

private:
    template<typename T, typename... field_ptrs_t>
    static constexpr auto from_fields_tie(
        T& obj, const std::tuple<field_ptrs_t...>& fields)
    {
        return std::apply(
            [&obj](const auto&... f) { 
                return std::tie(as_ref_to_value(obj, f)...); 
            }, 
            fields);
    }

    template<typename T, typename field_ptr_t>
    static constexpr decltype(auto) as_ref_to_value(T& obj, field_ptr_t f)
    {
        return obj.*f;
    }

    template<typename T, typename field_ptr_t>
    static constexpr decltype(auto) as_ref_to_value(T& obj, const field_ptr<field_ptr_t>& f)
    {
        return obj.*f.p;
    }

    template<typename T, typename field_ptr_t>
    static constexpr decltype(auto) as_ref_to_value(T& obj, const named_field_ptr<field_ptr_t>& f)
    {
        return obj.*f.field_ptr;
    }

    template<typename T, typename... field_ptrs_t>
    static constexpr auto from_fields_named_tie(
        T& obj, const std::tuple<named_field_ptr<field_ptrs_t>...>& fields)
    {
        return std::apply(
            [&obj](const auto&... f) { 
                return std::tuple(named_var{f.name, obj.*f.field_ptr}...); 
            }, 
            fields);
    }

    template<typename... Ts>
    static constexpr auto from_values_named_tie(std::tuple<Ts&...> fields)
    {
        return apply_with_index(
            [](const auto&... elem) { 
                return std::tuple(indexed_var<std::remove_const_t<Ts>, elem.index + 1>{elem.value}...); 
            }, 
            fields);
    }

    template<typename... Ts>
    static constexpr auto wrap(std::tuple<Ts&...> fields)
    {
        return std::apply(
            [](auto&... f) { 
                return std::tuple(wrapped_value<std::remove_const_t<Ts>>{f}...); 
            }, 
            fields);
    }
    
    template<typename T>
    static constexpr decltype(auto) as_aggregate(T& obj)
    {
        return static_cast<propagate_const_t<T, aggregate_t<std::remove_const_t<T>>>&>(obj);
    }
};

} // namespace vst

// #############
// # operators #
// #############

namespace vst::impl {

// comparable
template<typename T, std::enable_if_t<vst::trait<T>::exists && vst::is_vst_type<T>, int> = 0>
constexpr bool operator==(const T& lhs, const T& rhs)
{
    return vst::helper::tie(lhs) == vst::helper::tie(rhs);
}

template<typename T, std::enable_if_t<vst::trait<T>::exists && vst::is_vst_type<T>, int> = 0>
constexpr bool operator!=(const T& lhs, const T& rhs)
{
    return !(lhs == rhs);
}

// ordered
template<
    typename T, 
    std::enable_if_t<vst::trait<T>::exists && vst::is_vst_type<T> && vst::helper::has_op<T, vst::op::ordered>(), int> = 0>
constexpr bool operator<(const T& lhs, const T& rhs)
{
    return vst::helper::tie(lhs) < vst::helper::tie(rhs);
}

template<
    typename T, 
    std::enable_if_t<vst::trait<T>::exists && vst::is_vst_type<T> && vst::helper::has_op<T, vst::op::ordered>(), int> = 0>
constexpr bool operator<=(const T& lhs, const T& rhs)
{
    return lhs < rhs || lhs == rhs;
}

template<
    typename T, 
    std::enable_if_t<vst::trait<T>::exists && vst::is_vst_type<T> && vst::helper::has_op<T, vst::op::ordered>(), int> = 0>
constexpr bool operator>(const T& lhs, const T& rhs)
{
    return !(lhs <= rhs);
}

template<
    typename T, 
    std::enable_if_t<vst::trait<T>::exists && vst::is_vst_type<T> && vst::helper::has_op<T, vst::op::ordered>(), int> = 0>
constexpr bool operator>=(const T& lhs, const T& rhs)
{
    return !(lhs < rhs);
}

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
    apply_with_index([rhs_tie = vst::helper::raw_tie(rhs)](auto&&... a) {
        (op_t{}(
            a.value,
            std::get<a.index>(rhs_tie)), ...);
    }, vst::helper::raw_tie(lhs));
    return lhs;
}

template <typename op_t, typename vst_t>
constexpr vst_t binary_op(const vst_t& lhs, const vst_t& rhs)
{
    return apply_with_index(
        [lhs_tie = vst::helper::raw_tie(lhs), rhs_tie = vst::helper::raw_tie(rhs)]
        (auto&&... a) {
        return vst_t{op_t{}(
            std::get<a.index>(lhs_tie),
            std::get<a.index>(rhs_tie))...};
    }, vst::helper::raw_tie(lhs));
}

template<
    typename T, 
    std::enable_if_t<vst::trait<T>::exists && vst::is_vst_type<T> && vst::helper::has_op<T, vst::op::addable>(), int> = 0>
constexpr T operator+(const T& lhs, const T& rhs)
{
    return binary_op<std::plus<>>(lhs, rhs);
}
template<
    typename T, 
    std::enable_if_t<vst::trait<T>::exists && vst::is_vst_type<T> && vst::helper::has_op<T, vst::op::addable>(), int> = 0>
constexpr T operator-(const T& lhs, const T& rhs)
{
    return binary_op<std::minus<>>(lhs, rhs);
}
template<
    typename T, 
    std::enable_if_t<vst::trait<T>::exists && vst::is_vst_type<T> && vst::helper::has_op<T, vst::op::addable>(), int> = 0>
constexpr T& operator+=(T& lhs, const T& rhs)
{
    return binary_assign_op<plus_assign<>>(lhs, rhs);
}
template<
    typename T, 
    std::enable_if_t<vst::trait<T>::exists && vst::is_vst_type<T> && vst::helper::has_op<T, vst::op::addable>(), int> = 0>
constexpr T& operator-=(T& lhs, const T& rhs)
{
    return binary_assign_op<minus_assign<>>(lhs, rhs);
}

// stream
template<typename T, std::enable_if_t<vst::trait<T>::exists && vst::is_vst_type<T>, int> = 0>
std::ostream& operator<<(std::ostream& os, const T& rhs)
{
    os << "[";
    std::apply(
        [&os, &rhs](const auto&... field){
            ((os << " " << field), ...);
        }, vst::helper::named_tie(rhs));
    os << " ]";
    return os;
}

} // namespace vst::impl

// hashable
namespace vst
{

template<typename T, typename ENABLER = void>
struct hash;

template<typename T>
struct hash<T, std::enable_if_t<trait<T>::exists && helper::has_op<T, op::hashable>()>>
{
    size_t operator()(const T& o) const noexcept
    {
        // use boost helper for hash of tuples
        return boost::hash_value(helper::tie(o));
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
    using checkIfHashable = std::enable_if_t<vst::helper::has_op<vst::impl::type<args_t...>, vst::op::hashable>()>;
};

} // namespace std

#endif