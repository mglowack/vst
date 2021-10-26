#pragma once

#include "vst_defs.h"

#include <type_traits>

namespace vst {

// ######################
// # has_get_fields_raw #
// ######################

template<typename T, typename ENABLER = std::void_t<>>
constexpr bool has_get_fields_raw = false;

template<typename T>
constexpr bool has_get_fields_raw<
    T, 
    std::void_t<decltype(T::get_fields())>>
= true;


// ##################
// # has_get_fields #
// ##################

template<typename T, typename ENABLER = std::void_t<>>
constexpr bool has_get_fields = false;

template<typename T>
constexpr bool has_get_fields<
    T, 
    std::void_t<decltype(trait<T>::get_fields())>>
= true;


// ###############
// # is_vst_type #
// ###############

template<typename T>
constexpr bool is_vst_type = false;

template<typename T, typename... ops>
constexpr bool is_vst_type<type<T, ops...>> = true;

}