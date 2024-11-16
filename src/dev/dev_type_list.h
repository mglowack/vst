#pragma once
#include <tn/type_list>

namespace dev {

template<typename... Ts>
using type_list = tn::type_list<Ts...>;

using tn::any_type_list;

}