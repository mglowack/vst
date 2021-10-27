#include "vst.hpp"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/indexed_by.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/member.hpp>

using namespace ::testing;

// TODO MG: test ADL?
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
using simple_self_described = vst::type<simple_self_described_pod, ops...>;
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

template<typename T, typename ENABLER = void>
constexpr bool is_comparable = false;

template<typename T>
constexpr bool is_comparable<
    T, 
    std::void_t<
        decltype(std::declval<const T&>() == std::declval<const T&>()),
        decltype(std::declval<const T&>() != std::declval<const T&>())>>
 = true;

template<typename T, typename ENABLER = void>
constexpr bool is_ordered = false;

template<typename T>
constexpr bool is_ordered<
    T, 
    std::void_t<
        decltype(std::declval<const T&>() < std::declval<const T&>()),
        decltype(std::declval<const T&>() > std::declval<const T&>()),
        decltype(std::declval<const T&>() <= std::declval<const T&>()),
        decltype(std::declval<const T&>() >= std::declval<const T&>())>>
 = is_comparable<T>;

template<typename T, typename ENABLER = void>
constexpr bool is_streamable = false;

template<typename T>
constexpr bool is_streamable<
    T, 
    std::void_t<
        decltype(std::declval<std::ostream&>() << std::declval<const T&>())>>
 = true;

template<typename T, typename ENABLER = void>
constexpr bool is_hashable = false;

// TODO MG: test for boost hash and boost::hash_value
template<typename T>
constexpr bool is_hashable<
    T, 
    std::void_t<
        // decltype(std::declval<const std::hash<T>&>())>>
        // decltype(std::declval<const std::hash<T>&>()(std::declval<const T&>()))>>
        decltype(hash_value(std::declval<const T&>()))>>
 = true;

//  static_assert(!is_hashable<simple<>>);
//  static_assert( is_hashable<simple<vst::op::hashable>>);

template<typename T, typename ENABLER = void>
constexpr bool is_addable = false;

template<typename T>
constexpr bool is_addable<
    T, 
    std::void_t<
        decltype(std::declval<const T&>() + std::declval<const T&>()),
        decltype(std::declval<const T&>() - std::declval<const T&>())>>
 = true;

template<typename T, typename... extra_args_t>
struct append_template_args;

template<typename T, typename... args_t, typename... extra_args_t>
struct append_template_args<vst::impl::type<T, type_list<args_t...>>, extra_args_t...>
{
    using type = vst::impl::type<T, type_list<args_t..., extra_args_t...>>;
};

} // close anon namespace

template <typename T>
class test_vst : public ::testing::Test {};

using all_types = ::testing::Types<
    simple<>,
    simple_self_described<>,
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
    // static_assert(!is_hashable<VST>);

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
    // static_assert(!is_hashable<VST>);

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
    // static_assert(is_hashable<VST>);

    auto h = [](const VST& o) { return vst::hash<VST>{}(o); };
    auto sh = [](const VST& o) { return std::hash<VST>{}(o); };
    auto bh = [](const VST& o) { return hash_value(o); };

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
    static_assert(is_addable<VST>);
    // static_assert(!is_hashable<VST>);

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

// ###################################
// # manual overloading of operators #
// ###################################
namespace {

struct manual_override_pod {
    int x, y;
};
using manual_override = vst::type<manual_override_pod>;

constexpr bool operator==(const manual_override&, const manual_override&) {
    return true;
}

} // close anon namespace

TEST(test_vst, manual_override)
{
    EXPECT_TRUE((manual_override{1, 2} == manual_override{1, 1}));
    EXPECT_FALSE((manual_override{1, 2} != manual_override{1, 1}));
}

// #######################
// # custom constructors #
// #######################

TEST(test_vst, custom_ctor)
{
    struct custom_ctor_pod {
        int x, y;

        // NOTE: default ctor *not required*
        explicit custom_ctor_pod(int z) : x(z), y(2*z) {}
    };
    using custom_ctor = vst::type<custom_ctor_pod>;

    custom_ctor obj(5);

    EXPECT_THAT(obj.x, Eq(5));
    EXPECT_THAT(obj.y, Eq(10));
}

// ########################################
// # custom operators on a per type basis #
// ########################################

namespace 
{
    struct string_int {
        std::string number;
    };
    
    bool operator==(const string_int& lhs, const string_int& rhs)
    {
        return lhs.number == rhs.number;
    }
    
    bool operator<(const string_int& lhs, const string_int& rhs)
    {
        return lhs.number < rhs.number;
    }

    std::ostream& operator<<(std::ostream& os, const string_int& rhs)
    {
        return os << rhs.number;
    }

    // NOTE: 'string_int' already has operators defined, 
    // but we want to have different semantics in our code base 
    // for that specific type e.g. we want to use the string 
    // as if it was an integer like so:
    bool operator<(const wrapped_value<string_int>& lhs, const wrapped_value<string_int>& rhs)
    {
        return std::atoi(lhs.value.number.c_str()) < std::atoi(rhs.value.number.c_str());
    }
}

TEST(test_vst, custom_comparison_for_string_int)
{
    struct pod {
        int x;
        string_int s;
    };
    using data = vst::type<pod, vst::op::ordered>;

    ASSERT_THAT((string_int{"10"}), Lt(string_int{"4"}));
    EXPECT_THAT((data{4, "10"}), Gt(data{4, "4"}));
}

TEST(test_vst, built_in_comparison_for_const_char)
{
    struct pod {
        int x;
        const char* s;
    };
    using data = vst::type<pod, vst::op::ordered>;

    std::string s1 = "aaa";
    std::string s2 = "bbb";

    // make sure the one with lower address has the lexicographically higher value
    if (s1.c_str() < s2.c_str())
    {
        // needs swapping
        s1 = "bbb";
        s2 = "aaa";
        
        // make sure pointer arithmetic would give the wrong answer lexicographically
        ASSERT_TRUE((s1.c_str() < s2.c_str() && s1 > s2));
        EXPECT_THAT((data{4, s1.c_str()}), Gt(data{4, s2.c_str()}));
    }
    else
    {
        // make sure pointer arithmetic would give the wrong answer lexicographically
        ASSERT_TRUE((s1.c_str() > s2.c_str() && s1 < s2));
        EXPECT_THAT((data{4, s1.c_str()}), Lt(data{4, s2.c_str()}));
    }
}

// TODO MG:
//  * named_type
//  * named_type automatic comparisons to underlying
//  * specifying names + tests for streaming
//  * tests for composition with std containers, optionals, variants etc
//  * add static asserts to help with debugging compiler errors
//  * solve hash detection and other TODOs