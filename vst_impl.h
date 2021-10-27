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
void show_type(non_matchable) {}

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

namespace vst {

// #########
// # trait #
// #########

template<typename T, typename... ops>
struct trait<type_impl<T, with_fields::inferred, ops...>, std::enable_if_t<has_get_fields_raw<T>>>
: make_trait<T, ops...>
{
};

template<typename T, typename... ops>
struct trait<type_impl<T, with_fields::inferred, ops...>, std::enable_if_t<!has_get_fields_raw<T>>>
: make_basic_trait<T, ops...>
{
};

template<typename T, auto (*get_fields_func)(), typename... ops>
struct trait<type_impl<T, with_fields::from_func<get_fields_func>, ops...>>
: make_basic_trait<T, ops...>
, with_fields::from_func<get_fields_func>
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
    static constexpr decltype(auto) tie(T& obj)
    {
        if constexpr (has_get_fields<std::decay_t<T>>) 
        {
            return std::apply(
                [&obj](const auto&... f) { 
                    return std::tie((obj.*f.p)...); 
                }, 
                trait<std::decay_t<T>>::get_fields());
        }
        else if constexpr(is_vst_type<std::decay_t<T>>)
        {
            using U = underlying_t<std::decay_t<T>>;
            using X = std::conditional_t<std::is_const_v<std::remove_reference_t<T>>, const U, U>;
            return boost::pfr::structure_tie(static_cast<X&>(obj));
            // return boost::pfr::structure_tie(obj);
        }
        else
        {
            return boost::pfr::structure_tie(obj);
        }
    }
};


// #############
// # operators #
// #############

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

} // namespace vst

// // comparable
// template<typename T, std::enable_if_t<vst::trait<T>::exists, int> = 0>
// constexpr bool operator==(const T& lhs, const T& rhs)
// {
//     return vst::helper::tie(lhs) == vst::helper::tie(rhs);
// }

// template<typename T, std::enable_if_t<vst::trait<T>::exists, int> = 0>
// constexpr bool operator!=(const T& lhs, const T& rhs)
// {
//     return !(lhs == rhs);
// }

// // ordered
// template<
//     typename T, 
//     std::enable_if_t<vst::trait<T>::exists && vst::helper::has_op<T, vst::op::ordered>(), int> = 0>
// constexpr bool operator<(const T& lhs, const T& rhs)
// {
//     return vst::helper::tie(lhs) < vst::helper::tie(rhs);
// }

// template<
//     typename T, 
//     std::enable_if_t<vst::trait<T>::exists && vst::helper::has_op<T, vst::op::ordered>(), int> = 0>
// constexpr bool operator<=(const T& lhs, const T& rhs)
// {
//     return lhs < rhs || lhs == rhs;
// }

// template<
//     typename T, 
//     std::enable_if_t<vst::trait<T>::exists && vst::helper::has_op<T, vst::op::ordered>(), int> = 0>
// constexpr bool operator>(const T& lhs, const T& rhs)
// {
//     return !(lhs <= rhs);
// }

// template<
//     typename T, 
//     std::enable_if_t<vst::trait<T>::exists && vst::helper::has_op<T, vst::op::ordered>(), int> = 0>
// constexpr bool operator>=(const T& lhs, const T& rhs)
// {
//     return !(lhs < rhs);
// }

// // comparable
// template<typename T, std::enable_if_t<vst::trait<T>::exists && !vst::is_vst_type<T>, int> = 0>
// constexpr bool operator==(const T& lhs, const T& rhs)
// {
//     return vst::helper::tie(lhs) == vst::helper::tie(rhs);
// }

// template<typename T, std::enable_if_t<vst::trait<T>::exists && !vst::is_vst_type<T>, int> = 0>
// constexpr bool operator!=(const T& lhs, const T& rhs)
// {
//     return !(lhs == rhs);
// }

// // ordered
// template<
//     typename T, 
//     std::enable_if_t<vst::trait<T>::exists && !vst::is_vst_type<T> && vst::helper::has_op<T, vst::op::ordered>(), int> = 0>
// constexpr bool operator<(const T& lhs, const T& rhs)
// {
//     return vst::helper::tie(lhs) < vst::helper::tie(rhs);
// }

// template<
//     typename T, 
//     std::enable_if_t<vst::trait<T>::exist && !vst::is_vst_type<T> && vst::helper::has_op<T, vst::op::ordered>(), int> = 0>
// constexpr bool operator<=(const T& lhs, const T& rhs)
// {
//     return lhs < rhs || lhs == rhs;
// }

// template<
//     typename T, 
//     std::enable_if_t<vst::trait<T>::exists && !vst::is_vst_type<T> && vst::helper::has_op<T, vst::op::ordered>(), int> = 0>
// constexpr bool operator>(const T& lhs, const T& rhs)
// {
//     return !(lhs <= rhs);
// }

// template<
//     typename T, 
//     std::enable_if_t<vst::trait<T>::exists && !vst::is_vst_type<T> && vst::helper::has_op<T, vst::op::ordered>(), int> = 0>
// constexpr bool operator>=(const T& lhs, const T& rhs)
// {
//     return !(lhs < rhs);
// }


// // comparable
// template<typename T, std::enable_if_t<vst::trait<T>::exists && vst::is_vst_type<T>, int> = 0>
// constexpr bool operator==(const T& lhs, const T& rhs)
// {
//     return vst::helper::tie(lhs) == vst::helper::tie(rhs);
// }

// template<typename T, std::enable_if_t<vst::trait<T>::exists && vst::is_vst_type<T>, int> = 0>
// constexpr bool operator!=(const T& lhs, const T& rhs)
// {
//     return !(lhs == rhs);
// }

// // ordered
// template<
//     typename T, 
//     std::enable_if_t<vst::trait<T>::exists && vst::is_vst_type<T> && vst::helper::has_op<T, vst::op::ordered>(), int> = 0>
// constexpr bool operator<(const T& lhs, const T& rhs)
// {
//     return vst::helper::tie(lhs) < vst::helper::tie(rhs);
// }

// template<
//     typename T, 
//     std::enable_if_t<vst::trait<T>::exists && vst::is_vst_type<T> && vst::helper::has_op<T, vst::op::ordered>(), int> = 0>
// constexpr bool operator<=(const T& lhs, const T& rhs)
// {
//     return lhs < rhs || lhs == rhs;
// }

// template<
//     typename T, 
//     std::enable_if_t<vst::trait<T>::exists && vst::is_vst_type<T> && vst::helper::has_op<T, vst::op::ordered>(), int> = 0>
// constexpr bool operator>(const T& lhs, const T& rhs)
// {
//     return !(lhs <= rhs);
// }

// template<
//     typename T, 
//     std::enable_if_t<vst::trait<T>::exists && vst::is_vst_type<T> && vst::helper::has_op<T, vst::op::ordered>(), int> = 0>
// constexpr bool operator>=(const T& lhs, const T& rhs)
// {
//     return !(lhs < rhs);
// }

namespace vst 
{

// hashable
template<typename T, typename ENABLER = void>
struct hash;

template<typename T>
struct hash<T, std::enable_if_t<trait<T>::exists && helper::has_op<T, op::hashable>()>>
{
    size_t operator()(const T& o) const noexcept
    {        
        size_t hash = 0;
        std::apply(
            [&hash](const auto&... field){
                (boost::hash_combine(hash, field), ...);
            }, helper::tie(o));
        return hash;
    }
};

} // namespace vst

namespace std
{

template<typename... args_t>
struct hash<vst::type_impl<args_t...>> : vst::hash<vst::type_impl<args_t...>>
{
    using checkIfHashable = std::enable_if_t<vst::helper::has_op<vst::type_impl<args_t...>, vst::op::hashable>()>;
};

} // namespace std

// namespace std {

// template<typename T>
// struct hash<T>
// {
//     using checkIfHashable = std::enable_if_t<trait<foo>::exists>;

//     size_t operator()(const foo&) const noexcept
//     {        
//         return 0;
//     }
// };

// }

#endif