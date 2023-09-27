#include <dev_type_list.h>

#include <tuple>
#include <variant>

using namespace dev;

namespace {

    template<typename T>
    struct is_int : std::is_same<T, int> {};

    template<typename T>
    constexpr bool is_int_v = is_int<T>::value;

    static_assert( is_int_v<int>);
    static_assert(!is_int_v<float>);
    static_assert(!is_int_v<double>);

    template<template<typename> typename trait_t>
    struct trait_op
    {
        template<typename T>
        using negate = std::negation<trait_t<T>>;

        template<typename T>
        static constexpr bool negate_v = negate<T>::value;
    };

    static_assert(!std::negation_v<is_int<int>>);
    static_assert( std::negation_v<is_int<float>>);
    static_assert( std::negation_v<is_int<double>>);

    static_assert(!trait_op<is_int>::negate_v<int>);
    static_assert( trait_op<is_int>::negate_v<float>);
    static_assert( trait_op<is_int>::negate_v<double>);

}

namespace test::type_list_contains {

    static_assert(!type_list_contains_v<type_list<>, int>);
    static_assert(!type_list_contains_v<type_list<float, double>, int>);
    static_assert( type_list_contains_v<type_list<int>, int>);
    static_assert( type_list_contains_v<type_list<int, float>, int>);
    static_assert( type_list_contains_v<type_list<float, int>, int>);

}

namespace test::type_list_concat {

    static_assert(std::is_same_v<type_list_concat_t<type_list<>, type_list<>>, type_list<>>);
    static_assert(std::is_same_v<type_list_concat_t<type_list<int>, type_list<>>, type_list<int>>);
    static_assert(std::is_same_v<type_list_concat_t<type_list<>, type_list<int>>, type_list<int>>);
    static_assert(std::is_same_v<type_list_concat_t<type_list<int>, type_list<int>>, type_list<int, int>>);
    static_assert(std::is_same_v<type_list_concat_t<type_list<int>, type_list<float, double>>, type_list<int, float, double>>);
    static_assert(std::is_same_v<type_list_concat_t<type_list<float, int>, type_list<float, double>>, type_list<float, int, float, double>>);

}

namespace test::type_list_filter {

    static_assert(
        std::is_same_v<
            type_list_filter_t<
                type_list<>,
                is_int
            >,
            type_list<>
        >);

    static_assert(
        std::is_same_v<
            type_list_filter_t<
                type_list<int>,
                is_int
            >,
            type_list<int>
        >);

    static_assert(
        std::is_same_v<
            type_list_filter_t<
                type_list<int>,
                trait_op<is_int>::negate
            >,
            type_list<>
        >);

    static_assert(
        std::is_same_v<
            type_list_filter_t<
                type_list<float>,
                is_int
            >,
            type_list<>
        >);

    static_assert(
        std::is_same_v<
            type_list_filter_t<
                type_list<float>,
                trait_op<is_int>::negate
            >,
            type_list<float>
        >);

    static_assert(
        std::is_same_v<
            type_list_filter_t<
                type_list<float, double>,
                is_int
            >,
            type_list<>
        >);

    static_assert(
        std::is_same_v<
            type_list_filter_t<
                type_list<float, double>,
                trait_op<is_int>::negate
            >,
            type_list<float, double>
        >);

    static_assert(
        std::is_same_v<
            type_list_filter_t<
                type_list<int, float, int, double>,
                is_int
            >,
            type_list<int, int>
        >);

    static_assert(
        std::is_same_v<
            type_list_filter_t<
                type_list<int, float, int, double>,
                trait_op<is_int>::negate
            >,
            type_list<float, double>
        >);

}

namespace test::type_list_any {

    static_assert(!type_list_any_v<type_list<>, is_int>);
    static_assert(!type_list_any_v<type_list<float>, is_int>);
    static_assert(!type_list_any_v<type_list<float, double>, is_int>);
    static_assert( type_list_any_v<type_list<float, double>, trait_op<is_int>::negate>);
    static_assert(!type_list_any_v<type_list<int, int, int>, trait_op<is_int>::negate>);
    static_assert( type_list_any_v<type_list<int>, is_int>);
    static_assert( type_list_any_v<type_list<float, double, int>, is_int>);
    static_assert( type_list_any_v<type_list<float, int, float, int>, is_int>);
    static_assert( type_list_any_v<type_list<float, int, float, int>, trait_op<is_int>::negate>);

}

namespace test::type_list_all {

    static_assert( type_list_all_v<type_list<>, is_int>);
    static_assert(!type_list_all_v<type_list<float>, is_int>);
    static_assert(!type_list_all_v<type_list<float, double>, is_int>);
    static_assert( type_list_all_v<type_list<int>, is_int>);
    static_assert( type_list_all_v<type_list<int, int, int>, is_int>);
    static_assert(!type_list_all_v<type_list<float, double, int>, is_int>);
    static_assert( type_list_all_v<type_list<int, int>, is_int>);
    static_assert(!type_list_all_v<type_list<float, int, float, int>, trait_op<is_int>::negate>);
    static_assert( type_list_all_v<type_list<float, float, double>, trait_op<is_int>::negate>);

}

namespace test::type_list_transform {

    static_assert(std::is_same_v<type_list_transform_t<type_list<>, std::add_const_t>, type_list<>>);
    static_assert(std::is_same_v<type_list_transform_t<type_list<int>, std::add_const_t>, type_list<const int>>);
    static_assert(std::is_same_v<type_list_transform_t<type_list<int, double>, std::add_const_t>, type_list<const int, const double>>);

}

namespace test::type_list_cast {

    static_assert(std::is_same_v<type_list_cast_t<std::tuple<>>, type_list<>>);
    static_assert(std::is_same_v<type_list_cast_t<std::tuple<int, float>>, type_list<int, float>>);
    static_assert(std::is_same_v<type_list_cast_t<std::variant<>>, type_list<>>);
    static_assert(std::is_same_v<type_list_cast_t<std::variant<int, float>>, type_list<int, float>>);

}