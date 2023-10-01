#pragma once
#include <type_list/type_list.hh>

namespace dev {

template<typename... Ts>
using type_list = tn::type_list<Ts...>;

using tn::any_type_list;

}