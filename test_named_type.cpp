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
}