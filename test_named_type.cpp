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
    using price = named_type<
        int, 
        struct price_tag, 
        vst::op::ordered, 
        vst::op::hashable, 
        vst::op::addable>;
        
    using price_transparent = named_type<
        int, 
        struct price_transparent_tag, 
        transparent_ops,
        vst::op::ordered, 
        vst::op::hashable, 
        vst::op::addable>;

    static_assert( is_named_type<price>);
    static_assert(!is_named_type<int>);

    // using price_relaxed = named_type<
    //     int, 
    //     struct price_tag, 
    //     type_list<implicitly_convertible_to<int>, transparently_>,
    //     vst::op::ordered, 
    //     vst::op::hashable, 
    //     vst::op::addable>;
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

TEST(test_named_type, to_and_from_underlying_no_operators_when_transparent_ops_not_enabled)
{
    static_assert(!is_comparable<price, int>);
    static_assert(!is_ordered<price, int>);
    static_assert(!is_hashable<price, int>);
}

TEST(test_named_type, to_and_from_underlying_transparent_equality)
{
    static_assert( is_comparable<price_transparent, int>);

    constexpr auto eq = std::equal_to<price_transparent>{};

    static_assert(price_transparent{4} == 4);
    static_assert(price_transparent{4} != 2);
    static_assert(4 == price_transparent{4});
    static_assert(2 != price_transparent{4});

    EXPECT_TRUE((price_transparent{4} == 4));
    EXPECT_TRUE((price_transparent{4} != 2));
    EXPECT_TRUE((4 == price_transparent{4}));
    EXPECT_TRUE((2 != price_transparent{4}));
    
    static_assert( eq(price_transparent{4}, 4));
    static_assert(!eq(price_transparent{4}, 2));
    static_assert( eq(4, price_transparent{4}));
    static_assert(!eq(2, price_transparent{4}));
}
    
TEST(test_named_type, to_and_from_underlying_transparent_ordered)
{
    static_assert( is_comparable<price_transparent, int>);
    static_assert( is_ordered<price_transparent, int>);

    constexpr auto eq = std::equal_to<price_transparent>{};
    constexpr auto lt = std::less<price_transparent>{};

    static_assert(price_transparent{4} < 6);
    static_assert(price_transparent{4} <= 6);
    static_assert(price_transparent{4} > 2);
    static_assert(price_transparent{4} >= 2);
    static_assert(price_transparent{4} <= 4);
    static_assert(price_transparent{4} >= 4);

    static_assert(4 < price_transparent{6});
    static_assert(4 <= price_transparent{6});
    static_assert(4 > price_transparent{2});
    static_assert(4 >= price_transparent{2});
    static_assert(4 <= price_transparent{4});
    static_assert(4 >= price_transparent{4});

    EXPECT_TRUE((price_transparent{4} < 6));
    EXPECT_TRUE((price_transparent{4} <= 6));
    EXPECT_TRUE((price_transparent{4} > 2));
    EXPECT_TRUE((price_transparent{4} >= 2));
    EXPECT_TRUE((price_transparent{4} <= 4));
    EXPECT_TRUE((price_transparent{4} >= 4));

    EXPECT_TRUE((4 < price_transparent{6}));
    EXPECT_TRUE((4 <= price_transparent{6}));
    EXPECT_TRUE((4 > price_transparent{2}));
    EXPECT_TRUE((4 >= price_transparent{2}));
    EXPECT_TRUE((4 <= price_transparent{4}));
    EXPECT_TRUE((4 >= price_transparent{4}));

    static_assert( lt(price_transparent{4}, 6));
    static_assert( lt(price_transparent{4}, 6) || eq(price_transparent{4}, 6));
    static_assert(!lt(price_transparent{4}, 2) && !eq(price_transparent{4}, 2));
    static_assert(!lt(price_transparent{4}, 2));
    static_assert( lt(price_transparent{4}, 4) || eq(price_transparent{4}, 4));
    static_assert(!lt(price_transparent{4}, 4));

    static_assert( lt(4, price_transparent{6}));
    static_assert( lt(4, price_transparent{6}) || eq(4, price_transparent{6}));
    static_assert(!lt(4, price_transparent{2}) && !eq(4, price_transparent{2}));
    static_assert(!lt(4, price_transparent{2}));
    static_assert( lt(4, price_transparent{4}) || eq(4, price_transparent{4}));
    static_assert(!lt(4, price_transparent{4}));
}
    
// TEST(test_named_type, to_and_from_underlying_transparent_hashable)
// {
//     // TODO MG: check for hashable, addable?
//     static_assert( is_addable<price_transparent>);      // has explcit addable, price to price
//     static_assert(!is_addable<price_transparent, int>); // but no implcit addable to underlying
// }
    
TEST(test_named_type, to_and_from_underlying_transparent_addable)
{
    // TODO MG: check for hashable, addable?
    static_assert( is_addable<price_transparent>);      // has explcit addable, price to price
    static_assert(!is_addable<price_transparent, int>); // but no implcit addable to underlying
}

// TEST(test_named_type, to_and_from_underlying)
// {
//     static_assert(is_comparable<price, int>);

//     static_assert(price{4} == 4);
//     static_assert(4 == price{4});
//     static_assert(price{4} != 2);
//     static_assert(2 != price{4});

//     EXPECT_TRUE((price{4} == 4));
//     EXPECT_TRUE((4 == price{4}));
//     EXPECT_TRUE((price{4} != 2));
//     EXPECT_TRUE((2 != price{4}));

//     static_assert(is_ordered<price, int>);

//     static_assert(price{4} < 6);
//     static_assert(4 < price{6});
//     static_assert(price{4} > 2);
//     static_assert(4 > price{2});
//     static_assert(price{4} <= 4);
//     static_assert(4 <= price{4});
//     static_assert(price{4} >= 4);
//     static_assert(4 >= price{4});

//     EXPECT_TRUE((price{4} < 6));
//     EXPECT_TRUE((4 < price{6}));
//     EXPECT_TRUE((price{4} > 2));
//     EXPECT_TRUE((4 > price{2}));
//     EXPECT_TRUE((price{4} <= 4));
//     EXPECT_TRUE((4 <= price{4}));
//     EXPECT_TRUE((price{4} >= 4));
//     EXPECT_TRUE((4 >= price{4}));
    
//     static_assert( is_addable<price>);      // has explcit addable, price to price
//     static_assert(!is_addable<price, int>); // but no implcit addable to underlying
// }

// TEST(test_named_type, heterogeneous_lookup_std_set)
// {
//     // GIVEN
//     std::set<price> c;

//     // WHEN
//     c.insert(price{4});
//     c.insert(price{5});
//     c.insert(price{1});
    
//     // THEN
//     EXPECT_THAT(c.find(5), Eq(c.find(price{5})));
// }

// TEST(test_named_type, heterogeneous_lookup_std_map)
// {
//     // GIVEN
//     std::map<price, std::string> c;

//     // WHEN
//     c[price{5}] = "5";
//     c[price{1}] = "1";
//     c[price{4}] = "4";
    
//     // THEN
//     EXPECT_THAT(c.find(5), Eq(c.find(price{5})));
// }

// TEST(test_named_type, heterogeneous_lookup_boost_ordered_index)
// {
//     // GIVEN
//     namespace bmi = boost::multi_index;

//     using index_t = boost::multi_index_container<
//         price,
//         bmi::indexed_by<bmi::ordered_unique<bmi::identity<price>>>>;

//     index_t c;

//     // WHEN
//     c.insert(price{5});
//     c.insert(price{5});
//     c.insert(price{1});
    
//     // THEN
//     EXPECT_THAT(c.find(5), Eq(c.find(price{5})));
// }

// TEST(test_named_type, heterogeneous_lookup_boost_hashed_index)
// {
//     // GIVEN
//     namespace bmi = boost::multi_index;

//     using index_t = boost::multi_index_container<
//         price,
//         bmi::indexed_by<bmi::hashed_unique<bmi::identity<price>>>>;

//     EXPECT_TRUE((vst::hash<price>{}(4) == vst::hash<price>{}(price{4})));

//     index_t c;

//     // WHEN
//     c.insert(price{5});
//     c.insert(price{5});
//     c.insert(price{1});
    
//     // THEN
//     EXPECT_THAT(c.find(5), Ne(std::end(c)));
// }

// TODO MG:
//  * configurable comparisons to underlying?
//  * ref-types