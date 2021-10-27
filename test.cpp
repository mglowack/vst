#include "vst.hpp"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

// #include <boost/multi_index_container.hpp>
// #include <boost/multi_index/indexed_by.hpp>
// #include <boost/multi_index/hashed_index.hpp>
// #include <boost/multi_index/member.hpp>

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

// #########
// # utils #
// #########

template<typename T, typename ENABLER = void>
constexpr bool is_equality_comparable = false;

template<typename T>
constexpr bool is_equality_comparable<
    T, 
    std::void_t<
        decltype(std::declval<const T&>() == std::declval<const T&>()),
        decltype(std::declval<const T&>() != std::declval<const T&>())>>
 = true;

template<typename T, typename ENABLER = void>
constexpr bool is_comparable = false;

template<typename T>
constexpr bool is_comparable<
    T, 
    std::void_t<
        decltype(std::declval<const T&>() < std::declval<const T&>()),
        decltype(std::declval<const T&>() > std::declval<const T&>()),
        decltype(std::declval<const T&>() <= std::declval<const T&>()),
        decltype(std::declval<const T&>() >= std::declval<const T&>())>>
 = is_equality_comparable<T>;

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

} // close anon namespace

template <typename T>
class test_vst_equality : public ::testing::Test {};

using test_vst_equality_types = ::testing::Types<
    simple_self_described<>,
    custom_from_func<>,
    custom_from_var<>,
    simple<>
>;

TYPED_TEST_SUITE(test_vst_equality, test_vst_equality_types);

TYPED_TEST(test_vst_equality, test)
{
    static_assert(is_streamable<TypeParam>);
    static_assert(is_equality_comparable<TypeParam>);
    static_assert(!is_comparable<TypeParam>);
    // static_assert(!is_hashable<TypeParam>);

    static_assert(TypeParam{1, 2.f} == TypeParam{1, 2.f});
    static_assert(TypeParam{2, 2.f} == TypeParam{2, 2.f});
    static_assert(TypeParam{2, 2.f} != TypeParam{3, 2.f});
    static_assert(TypeParam{2, 2.f} != TypeParam{2, 1.f});

    EXPECT_TRUE((TypeParam{1, 2.f} == TypeParam{1, 2.f}));
    EXPECT_TRUE((TypeParam{2, 2.f} == TypeParam{2, 2.f}));
    EXPECT_TRUE((TypeParam{2, 2.f} != TypeParam{3, 2.f}));
    EXPECT_TRUE((TypeParam{2, 2.f} != TypeParam{2, 1.f}));
}

template <typename T>
class test_vst_comparable : public ::testing::Test {};

using test_vst_comparable_types = ::testing::Types<
    simple<vst::op::ordered>,
    custom_from_func<vst::op::ordered>,
    custom_from_var<vst::op::ordered>,
    simple<vst::op::ordered>
>;

TYPED_TEST_SUITE(test_vst_comparable, test_vst_comparable_types);

TYPED_TEST(test_vst_comparable, test)
{
    static_assert(is_streamable<TypeParam>);
    static_assert(is_equality_comparable<TypeParam>);
    static_assert(is_comparable<TypeParam>);
    // static_assert(!is_hashable<TypeParam>);

    static_assert(TypeParam{1, 2.f} <= TypeParam{1, 2.f});
    static_assert(TypeParam{1, 2.f} >= TypeParam{1, 2.f});
    static_assert(TypeParam{2, 2.f} < TypeParam{3, 2.f});
    static_assert(TypeParam{2, 2.f} < TypeParam{2, 3.f});
    static_assert(TypeParam{2, 2.f} < TypeParam{3, 3.f});
    static_assert(TypeParam{2, 2.f} > TypeParam{2, 1.f});
    static_assert(TypeParam{2, 2.f} > TypeParam{1, 2.f});
    static_assert(TypeParam{2, 2.f} > TypeParam{1, 1.f});
    
    EXPECT_TRUE((TypeParam{1, 2.f} <= TypeParam{1, 2.f}));
    EXPECT_TRUE((TypeParam{1, 2.f} >= TypeParam{1, 2.f}));
    EXPECT_TRUE((TypeParam{2, 2.f} < TypeParam{3, 2.f}));
    EXPECT_TRUE((TypeParam{2, 2.f} < TypeParam{2, 3.f}));
    EXPECT_TRUE((TypeParam{2, 2.f} < TypeParam{3, 3.f}));
    EXPECT_TRUE((TypeParam{2, 2.f} > TypeParam{2, 1.f}));
    EXPECT_TRUE((TypeParam{2, 2.f} > TypeParam{1, 2.f}));
    EXPECT_TRUE((TypeParam{2, 2.f} > TypeParam{1, 1.f}));
}

template <typename T>
class test_vst_hashable : public ::testing::Test {};

using test_vst_hashable_types = ::testing::Types<
    simple<vst::op::hashable>,
    custom_from_func<vst::op::hashable>,
    custom_from_var<vst::op::hashable>,
    simple<vst::op::hashable>
>;

TYPED_TEST_SUITE(test_vst_hashable, test_vst_hashable_types);

TYPED_TEST(test_vst_hashable, test)
{
    static_assert(is_streamable<TypeParam>);
    static_assert(is_equality_comparable<TypeParam>);
    static_assert(!is_comparable<TypeParam>);
    // static_assert(is_hashable<TypeParam>);

    auto h = [](const TypeParam& o) { return vst::hash<TypeParam>{}(o); };
    auto sh = [](const TypeParam& o) { return std::hash<TypeParam>{}(o); };
    auto bh = [](const TypeParam& o) { return hash_value(o); };

    EXPECT_TRUE((h(TypeParam{1, 2.f}) == sh(TypeParam{1, 2.f})));
    EXPECT_TRUE((h(TypeParam{1, 2.f}) == bh(TypeParam{1, 2.f})));

    EXPECT_TRUE((h(TypeParam{1, 2.f}) == h(TypeParam{1, 2.f})));
    EXPECT_TRUE((h(TypeParam{1, 2.f}) != h(TypeParam{2, 2.f})));
    EXPECT_TRUE((h(TypeParam{1, 2.f}) != h(TypeParam{1, 1.f})));
}

TYPED_TEST(test_vst_hashable, unordered_set)
{
    // GIVEN
    std::unordered_set<TypeParam> c;

    // WHEN
    c.insert(TypeParam{1, 1.f});
    c.insert(TypeParam{1, 1.f});
    c.insert(TypeParam{2, 2.f});
    c.insert(TypeParam{2, 1.f});
    c.insert(TypeParam{2, 1.f});
    c.insert(TypeParam{1, 3.f});
    
    // THEN
    EXPECT_THAT(c, UnorderedElementsAre(
        TypeParam{1, 1.f}, 
        TypeParam{2, 2.f}, 
        TypeParam{2, 1.f}, 
        TypeParam{1, 3.f}));
}

TYPED_TEST(test_vst_hashable, unordered_map)
{
    // GIVEN
    std::unordered_map<TypeParam, int> c;

    // WHEN
    c.insert(std::make_pair(TypeParam{1, 1.f}, 1));
    c.insert(std::make_pair(TypeParam{1, 1.f}, 2));
    c.insert(std::make_pair(TypeParam{2, 2.f}, 3));
    c.insert(std::make_pair(TypeParam{2, 1.f}, 4));
    c.insert(std::make_pair(TypeParam{2, 1.f}, 5));
    c.insert(std::make_pair(TypeParam{1, 3.f}, 6));
    
    // THEN
    EXPECT_THAT(c, UnorderedElementsAre(
        Pair(TypeParam{1, 1.f}, 1), 
        Pair(TypeParam{2, 2.f}, 3),
        Pair(TypeParam{2, 1.f}, 4), 
        Pair(TypeParam{1, 3.f}, 6)));
}

TYPED_TEST(test_vst_hashable, containers)
{
    // namespace bmi = boost::multi_index;

    // using index_t = boost::multi_index_container<
    //     TypeParam,
    //     bmi::indexed_by<bmi::hashed_unique<bmi::identity<TypeParam>>>>;

    // std::unordered_map<TypeParam, int> map;
    // std::unordered_set<TypeParam> set;
    // index_t bmap;

    
}