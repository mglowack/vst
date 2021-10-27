#include "vst.hpp"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

using namespace ::testing;

namespace {

// ##############
// # simple_pod #
// ##############

struct simple_pod {
    int x;
    float y;
};

static constexpr auto get_simple_pod_fields() {
    return std::tuple{
        MEMBER(simple_pod, x), 
        MEMBER(simple_pod, y)};
}

static constexpr auto k_simple_pod_fields = get_simple_pod_fields();

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

using simple                    = vst::type<simple_pod>;
// using simple_self_described = vst::type<simple_self_described_pod>;
// using custom_from_func          = vst::type<simple_pod, 
//                                             vst::with_fields::from_func<get_simple_pod_fields>>;
// using custom_from_var           = vst::type<simple_pod
//                                             vst::with_fields::from_func<&k_simple_pod_fields>>;

} // close anon namespace

template <typename T>
class test_vst_equality : public ::testing::Test {};

using equality_types = ::testing::Types<
    simple
    // simple_self_described,
    // simple_from_func,
    // simple_from_var,
>;

TYPED_TEST_SUITE(test_vst_equality, equality_types);

TYPED_TEST(test_vst_equality, test)
{
    static_assert(TypeParam{1, 2.f} == TypeParam{1, 2.f});
    static_assert(TypeParam{2, 2.f} == TypeParam{2, 2.f});
    static_assert(TypeParam{2, 2.f} != TypeParam{3, 2.f});
    static_assert(TypeParam{2, 2.f} != TypeParam{2, 1.f});

    EXPECT_EQ((TypeParam{1, 2.f}), (TypeParam{1, 2.f}));
    EXPECT_EQ((TypeParam{2, 2.f}), (TypeParam{2, 2.f}));
    EXPECT_NE((TypeParam{2, 2.f}), (TypeParam{3, 2.f}));
    EXPECT_NE((TypeParam{2, 2.f}), (TypeParam{2, 1.f}));
}