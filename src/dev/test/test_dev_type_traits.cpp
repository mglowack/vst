#include <dev_type_traits.h>

namespace test::make_func {

    static_assert( std::is_const<const int>::value);
    static_assert(!std::is_const<int      >::value);

    static_assert( dev::make_func<std::is_const>::func<const int>::value);
    static_assert(!dev::make_func<std::is_const>::func<int      >::value);

    static_assert( dev::make_func<std::is_const>::func_v<const int>);
    static_assert(!dev::make_func<std::is_const>::func_v<int      >);

    static_assert( dev::make_func<std::is_same, int>::func_v<int> );
    static_assert(!dev::make_func<std::is_same, int>::func_v<float>);

    static_assert( std::same_as<const int, dev::make_func<std::add_const>::func_t<int>>);
    static_assert( std::same_as<int &    , dev::make_func<std::add_lvalue_reference>::func_t<int>>);

}


namespace test::type_convert {

    static_assert(std::same_as<void,  dev::type_convert<int, float, void>::type>);
    static_assert(std::same_as<float, dev::type_convert<int, float, float>::type>);
    static_assert(std::same_as<float, dev::type_convert<int, float, int>::type>);

}

namespace test::combine {

    static_assert( std::is_const_v<const int>);
    static_assert(!std::negation_v<std::is_const<const int>>);
    static_assert(!dev::combine_r_v<const int, std::is_const, std::negation>);
    static_assert( dev::combine_r_v<const int, std::is_reference, std::negation>);
    static_assert( dev::combine_r_v<const int, std::is_reference, std::negation>);

    static_assert(std::same_as<const int , typename std::add_const<int>::type>);
    static_assert(std::same_as<const int&, typename std::add_lvalue_reference<typename std::add_const<int>::type>::type>);
    static_assert(std::same_as<const int , dev::combine_r_t<int, std::add_const>>);
    static_assert(std::same_as<const int&, dev::combine_r_t<int, std::add_const, std::add_lvalue_reference>>);
    static_assert(std::same_as<const volatile int&, dev::combine_r_t<int, std::add_volatile, std::add_const, std::add_lvalue_reference>>);

}

namespace test::apply_all {

    static_assert(std::same_as<void      , dev::apply_all_t<void>>);
    static_assert(std::same_as<const int , dev::apply_all_t<int,  std::add_const>>);
    static_assert(std::same_as<const int&, dev::apply_all_t<int,  std::add_const, std::add_lvalue_reference>>);
    static_assert(std::same_as<void      , dev::apply_all_t<void, dev::type_convert_f<int, void>::func>>);
    static_assert(std::same_as<void      , dev::apply_all_t<int,  dev::type_convert_f<int, void>::func>>);
    static_assert(std::same_as<float     , dev::apply_all_t<int,  dev::type_convert_f<int, void>::func,
                                                                  dev::type_convert_f<void, float>::func>>);

}