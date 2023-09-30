#pragma once

#include <type_traits>
#include <concepts>

namespace dev {

// #############
// # make_func #
// #############

template<template<typename...> typename Template, typename... Args>
struct make_func
{
    template<typename... InputArgs>
    using func = Template<Args..., InputArgs...>;

    template<typename... InputArgs>
    using func_t = typename func<InputArgs...>::type;

    template<typename... InputArgs>
    static constexpr bool func_v = func<InputArgs...>::value;
};

// ####################
// # make_func_single #
// ####################

template<template<typename...> typename Template, typename... Args>
struct make_func_single
{
    template<typename InputArgs>
    using func = Template<Args..., InputArgs>;

    template<typename InputArgs>
    using func_t = typename func<InputArgs>::type;

    template<typename InputArgs>
    static constexpr bool func_v = func<InputArgs>::value;
};

// ################
// # type_convert #
// ################

template<typename from_t, typename into_t, typename T>
struct type_convert : std::conditional<std::is_same_v<T, from_t>, into_t, T> {};

template<typename from_t, typename into_t>
using type_convert_f = make_func_single<type_convert, from_t, into_t>;

// ###########
// # combine #
// ###########

template<template<typename> typename trait_t, template<typename> typename...>
struct combine
{
    template<typename Args>
    using result = trait_t<Args>;
};

template<typename T, template<typename> typename... Args>
using combine_r_t = typename combine<Args...>::template result<T>::type;

template<typename T, template<typename> typename... Args>
constexpr bool combine_r_v = combine<Args...>::template result<T>::value;

template<template<typename> typename trait_t, template<typename> typename func_t>
struct combine<trait_t, func_t>
{
    template<typename Args>
    using result = func_t<typename trait_t<Args>::type>;
};

template<template<typename> typename trait_t, template<typename> typename func_t, template<typename> typename... funcs_t>
struct combine<trait_t, func_t, funcs_t...>
{
    template<typename Args>
    using result = typename combine<
        combine<trait_t, func_t>::template result,
        funcs_t...>
    ::template result<Args>;
};

// #############
// # apply_all #
// #############

template<typename T, template<typename> typename... funcs>
struct apply_all : combine<funcs...>::template result<T> {};

template<typename T>
struct apply_all<T> : std::type_identity<T> {};

template<typename T, template<typename> typename... funcs>
using apply_all_t = typename apply_all<T, funcs...>::type;

// ###################
// # propagate_const #
// ###################

template<typename T, typename U>
struct propagate_const : std::type_identity<U> {};

template<typename T, typename U>
struct propagate_const<const T, U> : std::type_identity<const U> {};

template<typename T, typename U>
using propagate_const_t = typename propagate_const<T, U>::type;

// ###############
// # is_template #
// ###############

template<template<typename...> typename template_t, typename T>
struct is_template : std::false_type {};

template<template<typename...> typename template_t, typename T>
constexpr bool is_template_v =  is_template<template_t, T>::value;

template<template<typename...> typename template_t, typename... args_t>
struct is_template<template_t, template_t<args_t...>> : std::true_type {};

// #################
// # template_cast #
// #################

template<template<typename...> typename template_to_t, typename T>
struct template_cast;

template<
    template<typename...> typename template_to_t,
    template<typename...> typename template_from_t,
    typename... Ts>
struct template_cast<template_to_t, template_from_t<Ts...>>
: std::type_identity<template_to_t<Ts...>> {};

template<template<typename...> typename template_to_t, typename T>
using template_cast_t = typename template_cast<template_to_t, T>::type;

} // namespace dev