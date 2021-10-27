#include "vst.hpp"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

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
        decltype(boost::hash_value(std::declval<const T&>()))>>
 = true;

} // close anon namespace

template <typename T>
class test_vst_equality : public ::testing::Test {};

using equality_types = ::testing::Types<
    simple_self_described<>,
    custom_from_func<>,
    custom_from_var<>,
    simple<>
>;

TYPED_TEST_SUITE(test_vst_equality, equality_types);

TYPED_TEST(test_vst_equality, test)
{
    static_assert(is_streamable<TypeParam>);
    static_assert(is_equality_comparable<TypeParam>);
    static_assert(!is_comparable<TypeParam>);
    static_assert(!is_hashable<TypeParam>);

    static_assert(TypeParam{1, 2.f} == TypeParam{1, 2.f});
    static_assert(TypeParam{2, 2.f} == TypeParam{2, 2.f});
    static_assert(TypeParam{2, 2.f} != TypeParam{3, 2.f});
    static_assert(TypeParam{2, 2.f} != TypeParam{2, 1.f});

    EXPECT_TRUE((TypeParam{1, 2.f} == TypeParam{1, 2.f}));
    EXPECT_TRUE((TypeParam{2, 2.f} == TypeParam{2, 2.f}));
    EXPECT_TRUE((TypeParam{2, 2.f} != TypeParam{3, 2.f}));
    EXPECT_TRUE((TypeParam{2, 2.f} != TypeParam{2, 1.f}));
}

// template <typename T>
// class test_vst_comparable : public ::testing::Test {};

// using ordered_types = ::testing::Types<
//     simple<vst::op::ordered>,
//     custom_from_func<vst::op::ordered>,
//     custom_from_var<vst::op::ordered>,
//     simple<vst::op::ordered>
// >;

// TYPED_TEST_SUITE(test_vst_comparable, equality_types);

// TYPED_TEST(test_vst_comparable, test)
// {
//     static_assert(is_streamable<TypeParam>);
//     static_assert(is_equality_comparable<TypeParam>);
//     static_assert(is_comparable<TypeParam>);
//     static_assert(!is_hashable<TypeParam>);

//     static_assert(TypeParam{1, 2.f} <= TypeParam{1, 2.f});
//     static_assert(TypeParam{1, 2.f} >= TypeParam{1, 2.f});
//     static_assert(TypeParam{2, 2.f} < TypeParam{3, 2.f});
//     static_assert(TypeParam{2, 2.f} < TypeParam{2, 3.f});
//     static_assert(TypeParam{2, 2.f} < TypeParam{3, 3.f});
//     static_assert(TypeParam{2, 2.f} > TypeParam{2, 1.f});
//     static_assert(TypeParam{2, 2.f} > TypeParam{1, 2.f});
//     static_assert(TypeParam{2, 2.f} > TypeParam{1, 1.f});
    
//     EXPECT_TRUE(TypeParam{1, 2.f} <= TypeParam{1, 2.f});
//     EXPECT_TRUE(TypeParam{1, 2.f} >= TypeParam{1, 2.f});
//     EXPECT_TRUE(TypeParam{2, 2.f} < TypeParam{3, 2.f});
//     EXPECT_TRUE(TypeParam{2, 2.f} < TypeParam{2, 3.f});
//     EXPECT_TRUE(TypeParam{2, 2.f} < TypeParam{3, 3.f});
//     EXPECT_TRUE(TypeParam{2, 2.f} > TypeParam{2, 1.f});
//     EXPECT_TRUE(TypeParam{2, 2.f} > TypeParam{1, 2.f});
//     EXPECT_TRUE(TypeParam{2, 2.f} > TypeParam{1, 1.f});
// }