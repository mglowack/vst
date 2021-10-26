#include "vst.hpp"

#include <tuple>

namespace my_ns {

struct foo 
{
    int x;
    float y;

    // static constexpr auto get_fields() { return std::tuple{field_ptr{&foo::x}}; }
};

constexpr static auto foo_fields = std::tuple{field_ptr{&foo::x}, field_ptr{&foo::y}};
// static constexpr auto get_foo_fields() { return std::tuple{field_ptr{&foo::x}, field_ptr{&foo::y}}; }

} // my_ns

// namespace vst {

// template<>
// struct trait<my_ns::foo> 
// : make_basic_trait<op::ordered, op::hashable>
// // , with_fields::from<foo>
// // , with_fields::from_func<get_foo_fields>
// , with_fields::from_var<&my_ns::foo_fields>
// {
// };

// } // namespace vst

// namespace my_ns {
// static_assert(foo{1, 3.f} != foo{2, 3.f});
// static_assert(foo{2, 3.f} == foo{2, 3.f});
// static_assert(foo{1, 3.f} < foo{2, 3.f});
// static_assert(foo{2, 2.f} < foo{2, 3.f});
// }
// static_assert(my_ns::foo{2, 3.f} == my_ns::foo{2, 3.f});

// struct bar {};

// namespace vst {

// template<>
// struct trait<bar> 
// : make_basic_trait<>
// , with_fields::empty
// {
// };

// } // namespace vst

// static_assert(bar{} == bar{});
// // static_assert(bar{} < bar{});

// struct baz_pod {
//     int x;
//     float y;

//     static constexpr auto get_fields() { return std::tuple{field_ptr{&baz_pod::x}, field_ptr{&baz_pod::y}}; }
// };
// using baz = vst::type<baz_pod, vst::op::ordered, vst::op::hashable>;
// static_assert(vst::has_get_fields_raw<baz_pod>);

// static_assert(baz{1, 3.f} != baz{2, 3.f});
// static_assert(baz{2, 3.f} == baz{2, 3.f});
// static_assert(baz{1, 3.f} < baz{2, 3.f});
// static_assert(baz{2, 2.f} < baz{2, 3.f});

// struct pure_pod {
//     int x;
//     float y;
// };

// static_assert(!vst::has_get_fields_raw<pure_pod>);

// namespace vst {

// template<>
// struct trait<pure_pod> 
// : make_basic_trait<vst::op::ordered>
// // , with_fields::inferred
// {
// };

// } // namespace vst

// static_assert(pure_pod{1, 3.f} != pure_pod{2, 3.f});
// static_assert(pure_pod{2, 3.f} == pure_pod{2, 3.f});
// static_assert(pure_pod{1, 3.f} < pure_pod{2, 3.f});
// static_assert(pure_pod{2, 2.f} < pure_pod{2, 3.f});
// static_assert(pure_pod{2, 4.f} > pure_pod{2, 3.f});

// static_assert(std::is_same_v<
//     std::tuple<int&, float&>, 
//     decltype(boost::pfr::structure_tie(std::declval<pure_pod&>()))>);

// static_assert(std::is_same_v<
//     std::tuple<const int&, const float&>, 
//     decltype(boost::pfr::structure_tie(std::declval<const pure_pod&>()))>);

// using pure = vst::type<pure_pod, vst::op::ordered>;

// static_assert(pure{1, 3.f} != pure{2, 3.f});
// static_assert(pure{2, 3.f} == pure{2, 3.f});
// static_assert(pure{1, 3.f} < pure{2, 3.f});
// static_assert(pure{2, 2.f} < pure{2, 3.f});
// static_assert(pure{2, 4.f} > pure{2, 3.f});

// struct empty {};

// struct pod_with_empty : empty {
//     int x;
//     float y;
// };

int main()
{
    // // using namespace my_ns;
    // assert((my_ns::foo{2, 2.f} == my_ns::foo{2, 2.f}));

    // // std::unordered_set<my_ns::foo, vst::hash<my_ns::foo>> s_foo;
    // // s_foo.insert(my_ns::foo{2, 2.f});
    
    // std::unordered_set<baz> s_baz;
    // s_baz.insert(baz{2, 2.f});

    // // std::cout << foo{2, 4.f} << "\n";

    // pure ppp{5, 6.f};
    // const pure_pod& pp = ppp;
    // std::cout << boost::pfr::get<0>(pp)  << "\n";
    // std::cout << boost::pfr::get<1>(pp)  << "\n";
    // std::cout << boost::pfr::tuple_size_v<baz_pod>  << "\n";
    // std::cout << (boost::pfr::structure_tie(pp) == boost::pfr::structure_tie(pp))  << "\n";

    // pod_with_empty ee{{}, 3, 7.f};
    // // std::cout << (boost::pfr::structure_tie(ee) == boost::pfr::structure_tie(ee))  << "\n";
    return 0;
}