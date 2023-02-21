#include <vst.hpp>
#include "vst_test_utils.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/indexed_by.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/member.hpp>

#include <sstream>

using namespace ::testing;

namespace {

// ##############
// # simple_pod #
// ##############

struct simple_pod {
    int x;
    float y;
};

constexpr auto get_simple_pod_fields() {
    return std::tuple{
        MEMBER(simple_pod, x), 
        MEMBER(simple_pod, y)};
}

constexpr auto k_simple_pod_fields = get_simple_pod_fields();

// ########################
// # simple_just_ptrs_pod #
// ########################

struct simple_just_ptrs_pod {
    int x;
    float y;

    static constexpr auto get_fields() {
        return std::tuple{
            &simple_just_ptrs_pod::x,
            &simple_just_ptrs_pod::y};
    }
};
        
// #############################
// # simple_self_described_pod #
// #############################

struct simple_self_described_pod {
    int x;
    float y;

    static constexpr auto get_fields() {
        return std::tuple{
            MEMBER(simple_self_described_pod, x), 
            MEMBER(simple_self_described_pod, y)};
    }
};
        
// #################
// # composite_pod #
// #################

// leveraging aggregate initialization to support all typed tests
struct float_pod {
    float f;
};

struct composite_pod {
    int x;
    vst::type<float_pod, vst::op::ordered, vst::op::hashable, vst::op::addable> y; // to support all typed tests
};

// ########
// # vsts #
// ########

template<typename... ops>
using simple                = vst::type<simple_pod, ops...>;

template<typename... ops>
using simple_just_ptrs      = vst::type<simple_just_ptrs_pod, ops...>;

template<typename... ops>
using simple_self_described = vst::type<simple_self_described_pod, ops...>;

template<typename... ops>
using simple_explicit_default = vst::type<simple_self_described_pod, vst::with_fields::use_default, ops...>;

template<typename... ops>
using custom_from_func      = vst::type<simple_pod, 
                                        vst::with_fields::from_func<get_simple_pod_fields>,
                                        ops...>;

template<typename... ops>
using custom_from_var       = vst::type<simple_pod,
                                        vst::with_fields::from_var<&k_simple_pod_fields>,
                                        ops...>;

template<typename... ops>                            
using composite = vst::type<composite_pod, ops...>;

// #########
// # utils #
// #########

template<typename T, typename... extra_args_t>
struct append_template_args;

template<typename T, typename... args_t, typename... extra_args_t>
struct append_template_args<vst::impl::type<T, type_list<args_t...>>, extra_args_t...>
{
    using type = vst::impl::type<T, type_list<args_t..., extra_args_t...>>;
};

template<typename T>
std::string stringify(const T& o)
{
    std::ostringstream oss;
    oss << o;
    return oss.str();
}

} // close anon namespace

TEST(test_vst, apply)
{
    std::tuple<int, int> a{1, 2};
    std::tuple<int, int> b{3, 4};

    const auto res = vst::apply_with_index(
            [b](auto... elem) { 
                // (std::cout << elem.index << ...);
                ((std::cout << elem.index << " = " << elem.value << '\n'), ...);
                ((std::cout << elem.index << " = " << std::get<elem.index>(b) << '\n'), ...);
                return std::tuple((elem.value + std::get<elem.index>(b))...);
                // return std::tuple(std::get<elem.index>(b)...);
            }, 
            a);
    EXPECT_THAT(res, (std::tuple<int, int>{4, 6}));

    // int x = 7;
    // const auto& e = vst::value_with_index<0, int>{x};
    // std::cout << e.index << " = " << e.value << '\n';
    // std::cout << e.index << " = " << std::get<e.index>(b) << '\n';
}

TEST(test_vst, empty)
{
    struct empty_pod {};
    using empty = vst::type<empty_pod, vst::with_fields::empty>;

    static_assert(empty{} == empty{});

    EXPECT_TRUE((empty{} == empty{}));
}

template <typename T>
class test_vst : public ::testing::Test {};

using all_types = ::testing::Types<
    simple<>,
    simple_just_ptrs<>,
    simple_self_described<>,
    simple_explicit_default<>,
    custom_from_func<>,
    custom_from_var<>,
    composite<>
>;

TYPED_TEST_SUITE(test_vst, all_types);

TYPED_TEST(test_vst, comparable)
{
    using VST = TypeParam;

    static_assert(is_streamable<VST>);
    static_assert(is_comparable<VST>);
    static_assert(!is_ordered<VST>);
    static_assert(!is_hashable<VST>);
    static_assert(!is_addable<VST>);

    static_assert(VST{1, 2.f} == VST{1, 2.f});
    static_assert(VST{2, 2.f} == VST{2, 2.f});
    static_assert(VST{2, 2.f} != VST{3, 2.f});
    static_assert(VST{2, 2.f} != VST{2, 1.f});

    EXPECT_TRUE((VST{1, 2.f} == VST{1, 2.f}));
    EXPECT_TRUE((VST{2, 2.f} == VST{2, 2.f}));
    EXPECT_TRUE((VST{2, 2.f} != VST{3, 2.f}));
    EXPECT_TRUE((VST{2, 2.f} != VST{2, 1.f}));
}

TYPED_TEST(test_vst, ordered)
{
    using VST = typename append_template_args<TypeParam, vst::op::ordered>::type;

    static_assert(is_streamable<VST>);
    static_assert(is_comparable<VST>);
    static_assert(is_ordered<VST>);
    static_assert(!is_hashable<VST>);
    static_assert(!is_addable<VST>);

    static_assert(VST{1, 2.f} <= VST{1, 2.f});
    static_assert(VST{1, 2.f} >= VST{1, 2.f});
    static_assert(VST{2, 2.f} < VST{3, 2.f});
    static_assert(VST{2, 2.f} < VST{2, 3.f});
    static_assert(VST{2, 2.f} < VST{3, 3.f});
    static_assert(VST{2, 2.f} > VST{2, 1.f});
    static_assert(VST{2, 2.f} > VST{1, 2.f});
    static_assert(VST{2, 2.f} > VST{1, 1.f});
    
    EXPECT_TRUE((VST{1, 2.f} <= VST{1, 2.f}));
    EXPECT_TRUE((VST{1, 2.f} >= VST{1, 2.f}));
    EXPECT_TRUE((VST{2, 2.f} < VST{3, 2.f}));
    EXPECT_TRUE((VST{2, 2.f} < VST{2, 3.f}));
    EXPECT_TRUE((VST{2, 2.f} < VST{3, 3.f}));
    EXPECT_TRUE((VST{2, 2.f} > VST{2, 1.f}));
    EXPECT_TRUE((VST{2, 2.f} > VST{1, 2.f}));
    EXPECT_TRUE((VST{2, 2.f} > VST{1, 1.f}));
}

TYPED_TEST(test_vst, set)
{
    using VST = typename append_template_args<TypeParam, vst::op::ordered>::type;
    
    // GIVEN
    std::set<VST> c;

    // WHEN
    c.insert(VST{1, 1.f});
    c.insert(VST{1, 1.f});
    c.insert(VST{2, 2.f});
    c.insert(VST{2, 1.f});
    c.insert(VST{2, 1.f});
    c.insert(VST{1, 3.f});
    
    // THEN
    EXPECT_THAT(c, ElementsAre(
        VST{1, 1.f}, 
        VST{1, 3.f}, 
        VST{2, 1.f}, 
        VST{2, 2.f}));
}

TYPED_TEST(test_vst, map)
{
    using VST = typename append_template_args<TypeParam, vst::op::ordered>::type;

    // GIVEN
    std::map<VST, int> c;

    // WHEN
    c.insert(std::make_pair(VST{1, 1.f}, 1));
    c.insert(std::make_pair(VST{1, 1.f}, 2));
    c.insert(std::make_pair(VST{2, 2.f}, 3));
    c.insert(std::make_pair(VST{2, 1.f}, 4));
    c.insert(std::make_pair(VST{2, 1.f}, 5));
    c.insert(std::make_pair(VST{1, 3.f}, 6));
    
    // THEN
    EXPECT_THAT(c, UnorderedElementsAre(
        Pair(VST{1, 1.f}, 1), 
        Pair(VST{1, 3.f}, 6),
        Pair(VST{2, 1.f}, 4), 
        Pair(VST{2, 2.f}, 3)));
}

TYPED_TEST(test_vst, boost_ordered)
{
    using VST = typename append_template_args<TypeParam, vst::op::ordered>::type;

    // GIVEN
    namespace bmi = boost::multi_index;

    using index_t = boost::multi_index_container<
        VST,
        bmi::indexed_by<bmi::ordered_unique<bmi::identity<VST>>>>;

    index_t c;

    // WHEN
    c.insert(VST{1, 1.f});
    c.insert(VST{1, 1.f});
    c.insert(VST{2, 2.f});
    c.insert(VST{2, 1.f});
    c.insert(VST{2, 1.f});
    c.insert(VST{1, 3.f});
    
    // THEN
    EXPECT_THAT(c, ElementsAre(
        VST{1, 1.f}, 
        VST{1, 3.f}, 
        VST{2, 1.f}, 
        VST{2, 2.f}));
}

TYPED_TEST(test_vst, hashable)
{
    using VST = typename append_template_args<TypeParam, vst::op::hashable>::type;

    static_assert(is_streamable<VST>);
    static_assert(is_comparable<VST>);
    static_assert(!is_ordered<VST>);
    static_assert(is_hashable<VST>);
    static_assert(!is_addable<VST>);

    auto h = [](const VST& o) { return vst::hash<VST>{}(o); };
    auto sh = [](const VST& o) { return std::hash<VST>{}(o); };
    auto bh = [](const VST& o) { return boost::hash<VST>{}(o); };

    EXPECT_TRUE((h(VST{1, 2.f}) == sh(VST{1, 2.f})));
    EXPECT_TRUE((h(VST{1, 2.f}) == bh(VST{1, 2.f})));

    EXPECT_TRUE((h(VST{1, 2.f}) == h(VST{1, 2.f})));
    EXPECT_TRUE((h(VST{1, 2.f}) != h(VST{2, 2.f})));
    EXPECT_TRUE((h(VST{1, 2.f}) != h(VST{1, 1.f})));
}

TYPED_TEST(test_vst, unordered_set)
{
    using VST = typename append_template_args<TypeParam, vst::op::hashable>::type;

    // GIVEN
    std::unordered_set<VST> c;

    // WHEN
    c.insert(VST{1, 1.f});
    c.insert(VST{1, 1.f});
    c.insert(VST{2, 2.f});
    c.insert(VST{2, 1.f});
    c.insert(VST{2, 1.f});
    c.insert(VST{1, 3.f});
    
    // THEN
    EXPECT_THAT(c, UnorderedElementsAre(
        VST{1, 1.f}, 
        VST{2, 2.f}, 
        VST{2, 1.f}, 
        VST{1, 3.f}));
}

TYPED_TEST(test_vst, unordered_map)
{
    using VST = typename append_template_args<TypeParam, vst::op::hashable>::type;
    
    // GIVEN
    std::unordered_map<VST, int> c;

    // WHEN
    c.insert(std::make_pair(VST{1, 1.f}, 1));
    c.insert(std::make_pair(VST{1, 1.f}, 2));
    c.insert(std::make_pair(VST{2, 2.f}, 3));
    c.insert(std::make_pair(VST{2, 1.f}, 4));
    c.insert(std::make_pair(VST{2, 1.f}, 5));
    c.insert(std::make_pair(VST{1, 3.f}, 6));
    
    // THEN
    EXPECT_THAT(c, UnorderedElementsAre(
        Pair(VST{1, 1.f}, 1), 
        Pair(VST{2, 2.f}, 3),
        Pair(VST{2, 1.f}, 4), 
        Pair(VST{1, 3.f}, 6)));
}

TYPED_TEST(test_vst, boost_hashed)
{
    using VST = typename append_template_args<TypeParam, vst::op::hashable>::type;
    
    // GIVEN
    namespace bmi = boost::multi_index;

    using index_t = boost::multi_index_container<
        VST,
        bmi::indexed_by<bmi::hashed_unique<bmi::identity<VST>>>>;

    index_t c;

    // WHEN
    c.insert(VST{1, 1.f});
    c.insert(VST{1, 1.f});
    c.insert(VST{2, 2.f});
    c.insert(VST{2, 1.f});
    c.insert(VST{2, 1.f});
    c.insert(VST{1, 3.f});
    
    // THEN
    EXPECT_THAT(c, UnorderedElementsAre(
        VST{1, 1.f}, 
        VST{2, 2.f}, 
        VST{2, 1.f}, 
        VST{1, 3.f}));
}

TYPED_TEST(test_vst, addable)
{
    using VST = typename append_template_args<TypeParam, vst::op::addable>::type;

    static_assert(is_streamable<VST>);
    static_assert(is_comparable<VST>);
    static_assert(!is_ordered<VST>);
    static_assert(!is_hashable<VST>);
    static_assert(is_addable<VST>);

    static_assert(VST{1, 2.f} + VST{2, 2.f} == VST{3, 4.f});
    static_assert(VST{1, 2.f} - VST{2, 2.f} == VST{-1, 0.f});
    
    EXPECT_TRUE((VST{1, 2.f} + VST{2, 2.f} == VST{3, 4.f}));
    EXPECT_TRUE((VST{1, 2.f} - VST{2, 2.f} == VST{-1, 0.f}));

    VST obj{0, 0.f};
    obj += VST{1, 1.f};
    EXPECT_TRUE((obj == VST{1, 1.f}));
    obj += VST{1, 1.f};
    EXPECT_TRUE((obj == VST{2, 2.f}));
    obj -= VST{0, 1.f};
    EXPECT_TRUE((obj == VST{2, 1.f}));
}

// #############
// # streaming #
// #############

TEST(test_vst, streaming)
{
    EXPECT_THAT(stringify(simple<>{1, 1.f}), Eq("[ field1=1 field2=1 ]"));
    EXPECT_THAT(stringify(simple_just_ptrs<>{1, 1.f}), Eq("[ field1=1 field2=1 ]"));
    EXPECT_THAT(stringify(simple_self_described<>{1, 1.f}), Eq("[ x=1 y=1 ]"));
    EXPECT_THAT(stringify(custom_from_func<>{1, 1.f}), Eq("[ x=1 y=1 ]"));
    EXPECT_THAT(stringify(custom_from_var<>{1, 1.f}), Eq("[ x=1 y=1 ]"));
    EXPECT_THAT(stringify(composite<>{1, 1.f}), Eq("[ field1=1 field2=[ field1=1 ] ]"));
}

// ###################################
// # manual overloading of operators #
// ###################################
namespace {

struct manual_override_pod {
    int x, y;
};
using manual_override = vst::type<manual_override_pod, vst::op::hashable>;

constexpr bool operator==(const manual_override&, const manual_override&) {
    return true;
}

} // close anon namespace

namespace vst 
{

template <>
struct hash<manual_override>
{
    size_t operator()(const manual_override&) const noexcept
    {
        return 42;
    }
};

}

TEST(test_vst, manual_override)
{
    EXPECT_TRUE((manual_override{1, 2} == manual_override{1, 1}));
    EXPECT_FALSE((manual_override{1, 2} != manual_override{1, 1}));
    EXPECT_TRUE((vst::hash<manual_override>{}(manual_override{1, 2}) == 42));
    EXPECT_TRUE((std::hash<manual_override>{}(manual_override{1, 2}) == 42));
    EXPECT_TRUE((boost::hash<manual_override>{}(manual_override{1, 2}) == 42));
}

// // #######################
// // # custom constructors #
// // #######################

// TEST(test_vst, custom_ctor)
// {
//     struct custom_ctor_pod {
//         int x, y;

//         // NOTE: default ctor *not required*
//         explicit custom_ctor_pod(int z) : x(z), y(2*z) {}
//     };
//     using custom_ctor = vst::type<custom_ctor_pod>;

//     custom_ctor obj(5);

//     EXPECT_THAT(obj.x, Eq(5));
//     EXPECT_THAT(obj.y, Eq(10));
// }

// // ########################################
// // # custom operators on a per type basis #
// // ########################################

// namespace 
// {
//     struct string_int {
//         std::string number;
//     };
    
//     bool operator==(const string_int& lhs, const string_int& rhs)
//     {
//         return lhs.number == rhs.number;
//     }
    
//     bool operator<(const string_int& lhs, const string_int& rhs)
//     {
//         return lhs.number < rhs.number;
//     }

//     std::ostream& operator<<(std::ostream& os, const string_int& rhs)
//     {
//         return os << rhs.number;
//     }

//     // NOTE: 'string_int' already has operators defined, 
//     // but we want to have different semantics in our code base 
//     // for that specific type e.g. we want to use the string 
//     // as if it was an integer like so:
//     bool operator<(const wrapped_value<string_int>& lhs, const wrapped_value<string_int>& rhs)
//     {
//         return std::atoi(lhs.value.number.c_str()) < std::atoi(rhs.value.number.c_str());
//     }

//     // NOTE: this overrides the stream operator of 'string_int'
//     //       but ONLY WHEN printed as part of ANY vst
//     std::ostream& operator<<(std::ostream& os, const wrapped_value<string_int>& rhs)
//     {
//         return os << "int:\"" << std::atoi(rhs.value.number.c_str()) << "\"";
//     }
    
//     struct specific_data_pod {
//         string_int s;
//     };
//     static constexpr auto specific_data_pod_fields = std::tuple{MEMBER(specific_data_pod, s)};
//     using specific_data = vst::type<specific_data_pod, vst::op::ordered>;
//     using specific_data_named = vst::type<
//         specific_data_pod, 
//         vst::with_fields::from_var<&specific_data_pod_fields>,
//         vst::op::ordered>;

//     // NOTE: this overrides the stream operator of 'string_int' 
//     //       but ONLY WHEN printed as part 'specific_data' or 'specific_data_named' vst
//     std::ostream& operator<<(std::ostream& os,
//                              const wrapped_value_of<specific_data, string_int>& rhs)
//     {
//         return os << "specific:\"" << std::atoi(rhs.value.number.c_str()) << "\"";
//     }

//     bool operator<(const wrapped_value_of<specific_data, string_int>& lhs, 
//                    const wrapped_value_of<specific_data, string_int>& rhs)
//     {
//         // fall back to the original operator
//         return lhs.value < rhs.value;
//     }

//     std::ostream& operator<<(std::ostream& os,
//                              const wrapped_value_of<specific_data_named, string_int>& rhs)
//     {
//         return os << "specific_named:\"" << std::atoi(rhs.value.number.c_str()) << "\"";
//     }

//     bool operator<(const wrapped_value_of<specific_data_named, string_int>& lhs, 
//                    const wrapped_value_of<specific_data_named, string_int>& rhs)
//     {
//         // fall back to the original operator
//         return lhs.value < rhs.value;
//     }
// }

// TEST(test_vst, custom_operators_for_string_int)
// {
//     struct pod {
//         string_int s;
//     };
//     static constexpr auto pod_fields = std::tuple{MEMBER(pod, s)};
//     using data_named = vst::type<pod, vst::with_fields::from_var<&pod_fields>, vst::op::ordered>;
//     using data = vst::type<pod, vst::op::ordered>;

//     ASSERT_THAT(stringify(string_int{"10"}), Eq("10"));
//     EXPECT_THAT(stringify(data{"10"}), Eq("[ field1=int:\"10\" ]"));
//     EXPECT_THAT(stringify(data_named{"10"}), Eq("[ s=int:\"10\" ]"));
//     EXPECT_THAT(stringify(specific_data{"10"}), Eq("[ field1=specific:\"10\" ]"));
//     EXPECT_THAT(stringify(specific_data_named{"10"}), Eq("[ s=specific_named:\"10\" ]"));

//     ASSERT_THAT((string_int{"10"}), Lt(string_int{"4"}));
//     EXPECT_THAT((data{"10"}), Gt(data{"4"}));
//     EXPECT_THAT((data_named{"10"}), Gt(data_named{"4"}));
//     EXPECT_THAT((specific_data{"10"}), Lt(specific_data{"4"}));
//     EXPECT_THAT((specific_data_named{"10"}), Lt(specific_data_named{"4"}));
// }

// TEST(test_vst, built_in_comparison_for_const_char)
// {
//     struct pod {
//         int x;
//         const char* s;
//     };
//     using data = vst::type<pod, vst::op::ordered>;

//     std::string s1 = "aaa";
//     std::string s2 = "bbb";

//     // make sure the one with lower address has the lexicographically higher value
//     if (s1.c_str() < s2.c_str())
//     {
//         // needs swapping
//         s1 = "bbb";
//         s2 = "aaa";
        
//         // make sure pointer arithmetic would give the wrong answer lexicographically
//         ASSERT_TRUE((s1.c_str() < s2.c_str() && s1 > s2));
//         EXPECT_THAT((data{4, s1.c_str()}), Gt(data{4, s2.c_str()}));
//     }
//     else
//     {
//         // make sure pointer arithmetic would give the wrong answer lexicographically
//         ASSERT_TRUE((s1.c_str() > s2.c_str() && s1 < s2));
//         EXPECT_THAT((data{4, s1.c_str()}), Lt(data{4, s2.c_str()}));
//     }
// }

// // TODO MG:
// //  * tests for composition with std containers, optionals, variants etc
// //  * add static asserts to help with debugging compiler errors
// //  * test ADL?
// //  * tags to allow re-using PODs?
// //
// // TODO MG cleanup:
// //  * move operators (plus_assign, minus_assign) to utils
// //  * move var classes to right namespace
// //  * use wrapped version on modyfying operators to allow customizing (maybe not possible)