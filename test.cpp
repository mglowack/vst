#include "vst.hpp"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

using namespace ::testing;

namespace {

struct simple_pod {
    int x;
    float y;
};
using simple = vst::type<simple_pod>;

std::ostream& operator<<(std::ostream& os, const simple&)
{
    return os;
}

static_assert(simple{2, 2.f} == simple{2, 2.f});
static_assert(simple{2, 2.f} != simple{3, 2.f});
static_assert(simple{2, 2.f} != simple{2, 1.f});

struct simple_inferred_pod {
    int x;
    float y;


};
using simple = vst::type<simple_pod>;

static_assert(simple{2, 2.f} == simple{2, 2.f});
static_assert(simple{2, 2.f} != simple{3, 2.f});
static_assert(simple{2, 2.f} != simple{2, 1.f});

}

template <typename T>
class test_vst : public ::testing::Test {
};

using MyTypes = ::testing::Types<simple>;
TYPED_TEST_SUITE(test_vst, MyTypes);

TYPED_TEST(test_vst, equality)
{
    EXPECT_EQ((TypeParam{2, 2.f}), (TypeParam{2, 3.f}));
}