#include "vst.hpp"

#include <tuple>

namespace my_ns {

// #######
// # foo #
// #######

struct foo_pod 
{
    int x;
    float y;

    // static constexpr auto get_fields() { return std::tuple{field_ptr{&foo_pod::x}, field_ptr{&foo_pod::y}}; }
};

constexpr static auto foo_fields = std::tuple{field_ptr{&foo_pod::x}, field_ptr{&foo_pod::y}};
static constexpr auto get_foo_fields() { return std::tuple{field_ptr{&foo_pod::x}, field_ptr{&foo_pod::y}}; }

using foo = vst::type_impl<
    foo_pod, 
    vst::with_fields::from_func<get_foo_fields>, 
    vst::op::ordered, vst::op::hashable>;

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

namespace my_ns {
static_assert(foo{1, 3.f} != foo{2, 3.f});
static_assert(foo{2, 3.f} == foo{2, 3.f});
static_assert(foo{1, 3.f} < foo{2, 3.f});
static_assert(foo{2, 2.f} < foo{2, 3.f});
}
static_assert(my_ns::foo{2, 3.f} == my_ns::foo{2, 3.f});

namespace some_other_ns {
struct wtf;
bool operator==(const wtf&, const wtf&) { return true; }
static_assert(my_ns::foo{2, 3.f} == my_ns::foo{2, 3.f});
}

static_assert(!vst::is_fields_def_v<vst::op::hashable>);
static_assert(!vst::is_fields_def_v<vst::op::ordered>);
// static_assert( vst::is_fields_def_v<vst::with_fields::from<my_ns::foo_pod>>);
static_assert( vst::is_fields_def_v<vst::with_fields::from_var<&my_ns::foo_fields>>);
static_assert( vst::is_fields_def_v<vst::with_fields::from_func<my_ns::get_foo_fields>>);
static_assert( vst::is_fields_def_v<vst::with_fields::empty>);
static_assert( vst::is_fields_def_v<vst::with_fields::inferred>);

// #######
// # bar #
// #######

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

// #######
// # baz #
// #######

namespace baz_ns {

struct baz_pod {
    int x;
    float y;

    static constexpr auto get_fields() { return std::tuple{field_ptr{&baz_pod::x}, field_ptr{&baz_pod::y}}; }
};
using baz = vst::type_impl<baz_pod, vst::op::ordered, vst::op::hashable>;
static_assert(vst::has_get_fields_raw<baz_pod>);

static_assert(baz{1, 3.f} != baz{2, 3.f});
static_assert(baz{2, 3.f} == baz{2, 3.f});
static_assert(baz{1, 3.f} < baz{2, 3.f});
static_assert(baz{2, 2.f} < baz{2, 3.f});

}

// ##########
// # simple #
// ##########

struct simple_pod {
    int x;
};
using simple = vst::type_impl<simple_pod>;

static_assert(!vst::has_get_fields_raw<simple_pod>);
static_assert(vst::is_vst_type<simple>);
static_assert(vst::trait<simple>::exists);

static_assert(simple{2} == simple{2});
static_assert(simple{2} != simple{3});

// ########
// # pure #
// ########

struct pure_pod {
    int x;
    float y;
};

static_assert(!vst::has_get_fields_raw<pure_pod>);

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

static_assert(std::is_same_v<
    std::tuple<int&, float&>, 
    decltype(boost::pfr::structure_tie(std::declval<pure_pod&>()))>);

static_assert(std::is_same_v<
    std::tuple<const int&, const float&>, 
    decltype(boost::pfr::structure_tie(std::declval<const pure_pod&>()))>);

using pure = vst::type_impl<pure_pod, vst::op::ordered>;

static_assert(pure{1, 3.f} != pure{2, 3.f});
static_assert(pure{2, 3.f} == pure{2, 3.f});
static_assert(pure{1, 3.f} < pure{2, 3.f});
static_assert(pure{2, 2.f} < pure{2, 3.f});
static_assert(pure{2, 4.f} > pure{2, 3.f});

struct empty {};

struct pod_with_empty : empty {
    int x;
    float y;
};

int main()
{
    // using namespace my_ns;
    assert((my_ns::foo{2, 2.f} == my_ns::foo{2, 2.f}));

    std::unordered_set<my_ns::foo, vst::hash<my_ns::foo>> s_foo;
    s_foo.insert(my_ns::foo{2, 2.f});
    s_foo.insert(my_ns::foo{2, 2.f});
    s_foo.insert(my_ns::foo{3, 2.f});
    assert(s_foo.size() == 2);
    
    std::unordered_set<baz_ns::baz> s_baz;
    s_baz.insert(baz_ns::baz{2, 2.f});
    s_baz.insert(baz_ns::baz{2, 2.f});
    s_baz.insert(baz_ns::baz{2, 1.f});
    s_baz.insert(baz_ns::baz{3, 2.f});
    assert(s_baz.size() == 3);

    // std::cout << foo{2, 4.f} << "\n";

    pure ppp{5, 6.f};
    const pure_pod& pp = ppp;
    std::cout << boost::pfr::get<0>(pp)  << "\n";
    std::cout << boost::pfr::get<1>(pp)  << "\n";
    std::cout << boost::pfr::tuple_size_v<baz_ns::baz_pod>  << "\n";
    std::cout << (boost::pfr::structure_tie(pp) == boost::pfr::structure_tie(pp))  << "\n";

    pod_with_empty ee{{}, 3, 7.f};
    // std::cout << (boost::pfr::structure_tie(ee) == boost::pfr::structure_tie(ee))  << "\n";
    return 0;
}