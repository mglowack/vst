#pragma once

#include "vst_defs.h"

#include <type_list/type_list.hh>
#include <dev_type_traits.h>

namespace vst {

template <typename T, typename... params>
struct make_type : std::type_identity<type<T, params...>> {};

template <typename T, typename... params>
struct make_type<T, dev::type_list<params...>> : make_type<T, params...> {};

static_assert( std::same_as<type<int>, typename make_type<int>::type>);
static_assert( std::same_as<type<int>, typename make_type<int, dev::type_list<>>::type>);
static_assert( std::same_as<type<int, op::ordered>, typename make_type<int, op::ordered>::type>);
static_assert( std::same_as<type<int, op::ordered>, typename make_type<int, dev::type_list<op::ordered>>::type>);

}