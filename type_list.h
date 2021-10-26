#pragma once

#include <type_traits>

template<typename... args_t>
struct type_list{};

template<typename list_t, typename item_t>
struct type_list_contains : std::false_type {};

template<
    typename item_t,
    typename first_t,
    typename... the_rest_t>
struct type_list_contains<type_list<first_t, the_rest_t...>, item_t>
    : type_list_contains<type_list<the_rest_t...>, item_t> {};

template<
    typename item_t,
    typename... the_rest_t>
struct type_list_contains<type_list<item_t, the_rest_t...>, item_t> : std::true_type {};

template<typename list_t, typename item_t>
constexpr bool type_list_contains_v = type_list_contains<list_t, item_t>::value;