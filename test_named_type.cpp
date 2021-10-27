#include "named_type.hpp"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

using namespace ::testing;

TEST(test_named_type, basic)
{
    using price = named_type<int, struct price_tag, vst::op::ordered, vst::op::addable>;

    static_assert(price{4} == price{4});
    static_assert(price{4} != price{2});
    static_assert(price{4} < price{6});
    static_assert(price{4} > price{2});
    static_assert(price{4} + price{1} == price{5});
    static_assert(price{4} - price{1} == price{3});

    EXPECT_TRUE((price{4} == price{4}));
    EXPECT_TRUE((price{4} != price{2}));
    EXPECT_TRUE((price{4} < price{6}));
    EXPECT_TRUE((price{4} > price{2}));
    EXPECT_TRUE((price{4} + price{1} == price{5}));
    EXPECT_TRUE((price{4} - price{1} == price{3}));
}

// TODO MG:
//  * automatic comparisons to underlying