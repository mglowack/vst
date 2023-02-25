#ifndef VST_IMPL_H
#define VST_IMPL_H

#include <vst_defs.h>
#include <vst_utils.h>
#include <vst_wrapped_value.h>
#include <vst_named_var.h>
#include <vst_indexed_var.h>
#include <type_list.h>

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

namespace vst::impl {

// ###########
// # helpers #
// ###########

struct indexed_tie_helper
{
    template<typename vst_t, typename... Ts>
    static constexpr auto tie(std::tuple<Ts&...> fields)
    {
        return apply_with_index(
            [](const auto... elem) { 
                return std::tuple(as_indexed_var<vst_t, elem.index + 1>(elem.value)...); // convert to 1-based
            }, 
            fields);
    }

    template<typename vst_t, std::size_t I, typename T>
    static constexpr auto as_indexed_var(const T& var)
    {
        return indexed_var<wrapped_value_of<vst_t, T>, I>{wrapped_value_of<vst_t, T>{var}};
    }
};

template<typename fields_def_t>
struct described_vst_helper
{
    template<typename T>
    static constexpr auto tie(T& obj)
    {
        return tie(obj, fields_def_t::get_fields());
    }

    template<typename T>
    static constexpr auto named_tie(T& obj)
    {
        return named_tie<std::decay_t<T>>(obj, fields_def_t::get_fields());
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

    template<typename vst_t, typename T, typename... field_ptrs_t>
    static constexpr auto named_tie(
        T& obj, const std::tuple<named_field_ptr<field_ptrs_t>...>& fields)
    {
        return std::apply(
            [&obj](const auto&... f) { 
                return std::tuple(as_named_var<vst_t>(f.name, as_ref_to_value(obj, f))...); 
            }, 
            fields);
    }

    // NOTE: this overload takes over if fields ARE NOT named_field_ptr<T> i.e. no names were specified
    template<typename vst_t, typename T, typename... field_ptrs_t>
    static constexpr auto named_tie(T& obj, const std::tuple<field_ptrs_t...>& fields)
    {
        return indexed_tie_helper::tie<vst_t>(tie(obj, fields)); // fallback to indexing members
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

    template<typename vst_t, typename T>
    static constexpr auto as_named_var(const char* name, const T& var)
    {
        return named_var<wrapped_value_of<vst_t, T>>{name, wrapped_value_of<vst_t, T>{var}};
    }
};

struct aggregate_vst_helper
{
    template<typename T>
    static constexpr auto tie(T& obj)
    {
        return boost::pfr::structure_tie(as_aggregate(obj));
    }

    template<typename T>
    static constexpr auto named_tie(T& obj)
    {
        return indexed_tie_helper::tie<std::decay_t<T>>(tie(obj));
    }

private:    
    template<typename T>
    static constexpr decltype(auto) as_aggregate(T& obj)
    {
        return static_cast<propagate_const_t<T, aggregate_t<std::remove_const_t<T>>>&>(obj);
    }
};

struct helper 
{
    template<typename T, typename op>
    static constexpr bool has_op() { return type_list_contains_v<typename trait<T>::properties, op>; }

    template<typename T>
    static constexpr auto tie(T& obj)
    {
        return trait<std::decay_t<T>>::tie(obj);
    }

    template<typename T>
    static constexpr auto named_tie(T& obj)
    {
        return trait<std::decay_t<T>>::named_tie(obj);
    }

    template<typename T>
    static constexpr auto wrapped_tie(T& obj)
    {
        using vst_t = std::decay_t<T>;
        if constexpr (std::is_same_v<std::tuple<>, decltype(tie(obj))>)
        {
            return std::tuple{};
        }
        else
        {
            return std::apply(
                [](auto&... f) { 
                    return std::tuple(wrapped_value_of<vst_t, std::decay_t<decltype(f)>>{f}...); 
                }, 
                tie(obj));
        }
    }
};

} // close vst::impl namespace

namespace vst {

// #########
// # trait #
// #########

namespace impl {

template<typename T, typename fields_def_helper_t, typename... ops>
struct trait
: fields_def_helper_t
{
    static constexpr bool exists = true;
    using properties = type_list<ops...>;
};

} // namespace impl

template<typename T>
struct trait<type<T>>
: trait<type<T, with_fields::use_default>>
{
};

template<typename T, typename... ops>
struct trait<
    type<T, with_fields::use_default, ops...>, 
    std::enable_if_t<!has_get_fields<T>>>
: trait<type<T, with_fields::from_aggregate, ops...>>
{
    static_assert(std::is_aggregate_v<T>, "T must be an aggregate or have 'get_fields' defined.");
};

template<typename T, typename... ops>
struct trait<
    type<T, with_fields::use_default, ops...>, 
    std::enable_if_t<has_get_fields<T>>>
: trait<type<T, with_fields::from<T>, ops...>>
{
};

template<typename T, typename... ops>
struct trait<type<T, with_fields::from_aggregate, ops...>>
: impl::trait<T, impl::aggregate_vst_helper, ops...>
{
    static_assert(std::is_aggregate_v<T>, "T must be an aggregate.");
};

template<typename T, typename first_op, typename... ops>
struct trait<
    type<T, first_op, ops...>, 
    std::enable_if_t<!is_fields_def<first_op> && !std::is_same_v<first_op, with_fields::use_default>>>
: trait<type<T, with_fields::use_default, first_op, ops...>>
{
};

template<typename T, typename fields_def, typename... ops>
struct trait<
    type<T, fields_def, ops...>, 
    std::enable_if_t<is_fields_def<fields_def>>>
: impl::trait<T, impl::described_vst_helper<fields_def>, ops...>
{
    static_assert(has_correct_get_fields<fields_def, T>, "'get_fields' must return a tuple of pointer to members or named_field_ptr");
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
    apply_with_index([rhs_tie = helper::tie(rhs)](const auto... a) {
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
        [rhs_tie = helper::tie(rhs)](const auto... a) {
        return vst_t{op_t{}(
            a.value,
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
        [&os](const auto&... field){
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
    
template<typename T, 
         typename ENABLER = std::enable_if_t<trait<T>::exists && impl::helper::has_op<T, op::hashable>()>>
struct hash
{
    size_t operator()(const T& o) const noexcept
    {
        // use boost helper for hash of tuples
        return boost::hash_value(impl::helper::tie(o));
    }
};

} // namespace vst

namespace boost
{

// boost::hash support
template<typename... args_t>
struct hash<vst::impl::type<args_t...>> : vst::hash<vst::impl::type<args_t...>>
{
};

} // namespace boost

namespace std
{

// std::hash support
template<typename... args_t>
struct hash<vst::impl::type<args_t...>> : vst::hash<vst::impl::type<args_t...>>
{
};

} // namespace std

#endif