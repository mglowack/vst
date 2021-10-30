#include "named_type.hpp"
#include "vst_test_utils.h"

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/indexed_by.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/member.hpp>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

using namespace ::testing;

namespace 
{
    using price = named_type<int, 
                             struct price_tag, 
                             vst::op::ordered, 
                             vst::op::hashable, 
                             vst::op::addable>;
}

TEST(test_named_type, basic)
{
    static_assert(is_comparable<price>);

    static_assert(price{4} == price{4});
    static_assert(price{4} != price{2});

    EXPECT_TRUE((price{4} == price{4}));
    EXPECT_TRUE((price{4} != price{2}));

    static_assert(is_ordered<price>);

    static_assert(price{4} < price{6});
    static_assert(price{4} <= price{6});
    static_assert(price{4} > price{2});
    static_assert(price{4} >= price{2});
    static_assert(price{4} <= price{4});
    static_assert(price{4} >= price{4});

    EXPECT_TRUE((price{4} < price{6}));
    EXPECT_TRUE((price{4} <= price{6}));
    EXPECT_TRUE((price{4} > price{2}));
    EXPECT_TRUE((price{4} >= price{2}));
    EXPECT_TRUE((price{4} <= price{4}));
    EXPECT_TRUE((price{4} >= price{4}));

    static_assert(is_addable<price>);

    static_assert(price{4} + price{1} == price{5});
    static_assert(price{4} - price{1} == price{3});

    EXPECT_TRUE((price{4} + price{1} == price{5}));
    EXPECT_TRUE((price{4} - price{1} == price{3}));

    price p{4};
    p += price{7};
    EXPECT_TRUE((p == price{11}));
    p -= price{3};
    EXPECT_TRUE((p == price{8}));
}

TEST(test_named_type, to_and_from_underlying)
{
    static_assert(is_comparable<price, int>);

    static_assert(price{4} == 4);
    static_assert(4 == price{4});
    static_assert(price{4} != 2);
    static_assert(2 != price{4});

    EXPECT_TRUE((price{4} == 4));
    EXPECT_TRUE((4 == price{4}));
    EXPECT_TRUE((price{4} != 2));
    EXPECT_TRUE((2 != price{4}));

    static_assert(is_ordered<price, int>);

    static_assert(price{4} < 6);
    static_assert(4 < price{6});
    static_assert(price{4} > 2);
    static_assert(4 > price{2});
    static_assert(price{4} <= 4);
    static_assert(4 <= price{4});
    static_assert(price{4} >= 4);
    static_assert(4 >= price{4});

    EXPECT_TRUE((price{4} < 6));
    EXPECT_TRUE((4 < price{6}));
    EXPECT_TRUE((price{4} > 2));
    EXPECT_TRUE((4 > price{2}));
    EXPECT_TRUE((price{4} <= 4));
    EXPECT_TRUE((4 <= price{4}));
    EXPECT_TRUE((price{4} >= 4));
    EXPECT_TRUE((4 >= price{4}));
    
    static_assert( is_addable<price>);      // has explcit addable, price to price
    static_assert(!is_addable<price, int>); // but no implcit addable to underlying
}

TEST(test_named_type, heterogeneous_lookup_std_set)
{
    // GIVEN
    std::set<price> s;

    // WHEN
    s.insert(price{4});
    s.insert(price{5});
    s.insert(price{1});
    
    // THEN
    EXPECT_THAT(s.find(5), Ne(std::end(s)));
}

TEST(test_named_type, heterogeneous_lookup_std_map)
{
    // GIVEN
    std::map<price, std::string> m;

    // WHEN
    m[price{5}] = "5";
    m[price{1}] = "1";
    m[price{4}] = "4";
    
    // THEN
    EXPECT_THAT(m.find(5), Ne(std::end(m)));
}

TEST(test_named_type, heterogeneous_lookup_boost_ordered_index)
{
    // GIVEN
    namespace bmi = boost::multi_index;

    using index_t = boost::multi_index_container<
        price,
        bmi::indexed_by<bmi::ordered_unique<bmi::identity<price>>>>;

    index_t c;

    // WHEN
    c.insert(price{5});
    c.insert(price{5});
    c.insert(price{1});
    
    // THEN
    EXPECT_THAT(c.find(5), Ne(std::end(c)));
}

TEST(test_named_type, heterogeneous_lookup_boost_hashed_index)
{
    // GIVEN
    namespace bmi = boost::multi_index;

    using index_t = boost::multi_index_container<
        price,
        bmi::indexed_by<bmi::hashed_unique<bmi::identity<price>>>>;

    EXPECT_TRUE((vst::hash<price>{}(4) == vst::hash<price>{}(price{4})));

    index_t c;

    // WHEN
    c.insert(price{5});
    c.insert(price{5});
    c.insert(price{1});
    
    // THEN
    EXPECT_THAT(c.find(5), Ne(std::end(c)));
}

// TODO MG:
//  * configurable comparisons to underlying?
//  * ref-types