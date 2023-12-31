#include <named_type.hpp>
#include "vst_test_utils.h"

#include <dev_concepts.h>

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/indexed_by.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/member.hpp>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <unordered_map>
#include <map>

using namespace ::testing;

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

static_assert(!TransparentWith<int, price>);
static_assert( TransparentWith<int, price_transparent>);

// clang-format off
static_assert(!TransparentWith<int, named_type<int, struct default_test                          >>);
static_assert(!TransparentWith<int, named_type<int, struct explicit_default_test, default_ops    >>);
static_assert(!TransparentWith<int, named_type<int, struct strict_test,           strict_ops     >>);
static_assert( TransparentWith<int, named_type<int, struct transparent_test,      transparent_ops>>);

static_assert( TransparentWith<int,   named_type<int, struct transparent_test,      transparent_ops>>);
static_assert(!TransparentWith<float, named_type<int, struct transparent_test,      transparent_ops>>);
static_assert( TransparentWith<int,   named_type<int, struct transparent_test, transparent_ops_with<int>>>);
static_assert(!TransparentWith<float, named_type<int, struct transparent_test, transparent_ops_with<int>>>);
static_assert( TransparentWith<float, named_type<int, struct transparent_test, transparent_ops_with<float>>>);

static_assert(std::is_same_v<dev::type_list<strict_ops>, named_type_trait<named_type<int, struct default_test                          >>::op_categories>);
static_assert(std::is_same_v<dev::type_list<strict_ops>, named_type_trait<named_type<int, struct explicit_default_test, default_ops    >>::op_categories>);
static_assert(std::is_same_v<dev::type_list<strict_ops>, named_type_trait<named_type<int, struct strict_test,           strict_ops     >>::op_categories>);

static_assert(std::is_same_v<dev::type_list<transparent_ops_with<int>>,   named_type_trait<named_type<int, struct transparent_test, transparent_ops>>::op_categories>);
static_assert(std::is_same_v<dev::type_list<transparent_ops_with<int>>,   named_type_trait<named_type<int, struct transparent_test, transparent_ops_with<int>>>::op_categories>);
static_assert(std::is_same_v<dev::type_list<transparent_ops_with<float>>, named_type_trait<named_type<int, struct transparent_test, transparent_ops_with<float>>>::op_categories>);
// clang-format on

TEST(test_named_type, implicitly_convertible_to)
{
    using price_relaxed_to = named_type<int, struct price_relaxed_to_tag, implicit_conversions_to<int>>;

    static_assert( std::is_convertible_v<price_relaxed_to, int>);
    static_assert(!std::is_convertible_v<int, price_relaxed_to>);

    [[maybe_unused]] int p = price_relaxed_to{5};
}

TEST(test_named_type, implicitly_convertible_to_another_type)
{
    using price_relaxed_to = named_type<int, struct price_relaxed_to_tag, implicit_conversions_to<float>>;

    static_assert(!std::is_convertible_v<price_relaxed_to, int>);
    static_assert( std::is_convertible_v<price_relaxed_to, float>);
    static_assert(!std::is_convertible_v<float, price_relaxed_to>);

    [[maybe_unused]] float p = price_relaxed_to{5};
}

TEST(test_named_type, implicitly_convertible_from)
{
    using price_relaxed_from = named_type<int, struct price_relaxed_to_tag, implicit_conversions_from<int>>;

    static_assert(!std::is_convertible_v<price_relaxed_from, int>);
    static_assert( std::is_convertible_v<int, price_relaxed_from>);

    [[maybe_unused]] price_relaxed_from p = 5;
}

TEST(test_named_type, implicitly_convertible_from_another_type)
{
    using price_relaxed_from = named_type<int, struct price_relaxed_to_tag, implicit_conversions_from<float>>;

    static_assert(!std::is_convertible_v<int, price_relaxed_from>);
    static_assert( std::is_convertible_v<float, price_relaxed_from>);

    [[maybe_unused]] price_relaxed_from p = 5.f;
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

    static_assert(dev::Addable<price>);

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
    static_assert(!is_comparable_with<price, int>);
    static_assert(!is_ordered<price, int>);
    static_assert(!is_hashable<price, int>);
    static_assert(!dev::AddableWith<price, int>);
}

TEST(test_named_type, to_and_from_underlying_transparent_equality)
{
    static_assert( is_comparable_with<price_transparent, int>);
    static_assert( is_comparable_with<int, price_transparent>);

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

    EXPECT_TRUE(( eq(price_transparent{4}, 4)));
    EXPECT_TRUE((!eq(price_transparent{4}, 2)));
    EXPECT_TRUE(( eq(4, price_transparent{4})));
    EXPECT_TRUE((!eq(2, price_transparent{4})));
}

TEST(test_named_type, to_and_from_T_transparent_equality)
{
    using value = named_type<int, struct to_and_from_T_transparent_equality_test, transparent_ops_with<float>>;
    static_assert( is_comparable_with<value, float>);

    constexpr auto eq = std::equal_to<value>{};

    static_assert(value{4} == 4.f);
    static_assert(value{4} != 2.f);
    static_assert(4.f == value{4});
    static_assert(2.f != value{4});

    EXPECT_TRUE((value{4} == 4.f));
    EXPECT_TRUE((value{4} != 2.f));
    EXPECT_TRUE((4.f == value{4}));
    EXPECT_TRUE((2.f != value{4}));

    static_assert( eq(value{4}, 4.f));
    static_assert(!eq(value{4}, 2.f));
    static_assert( eq(4.f, value{4}));
    static_assert(!eq(2.f, value{4}));

    EXPECT_TRUE(( eq(value{4}, 4.f)));
    EXPECT_TRUE((!eq(value{4}, 2.f)));
    EXPECT_TRUE(( eq(4.f, value{4})));
    EXPECT_TRUE((!eq(2.f, value{4})));
}

TEST(test_named_type, to_and_from_multiple_T_transparent_equality)
{
    using value = named_type<
        int,
        struct to_and_from_multiple_T_transparent_equality_test,
        transparent_ops_with<float>,
        transparent_ops_with<int>>;
    static_assert( is_comparable_with<value, int>);
    static_assert( is_comparable_with<value, float>);

    constexpr auto eq = std::equal_to<value>{};

    static_assert(value{4} == 4);
    static_assert(value{4} == 4.f);
    static_assert(eq(value{4}, 4));
    static_assert(eq(value{4}, 4.f));
}

TEST(test_named_type, to_and_from_underlying_transparent_ordered)
{
    static_assert( is_comparable_with<price_transparent, int>);
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

    EXPECT_TRUE(( lt(price_transparent{4}, 6)));
    EXPECT_TRUE(( lt(price_transparent{4}, 6) || eq(price_transparent{4}, 6)));
    EXPECT_TRUE((!lt(price_transparent{4}, 2) && !eq(price_transparent{4}, 2)));
    EXPECT_TRUE((!lt(price_transparent{4}, 2)));
    EXPECT_TRUE(( lt(price_transparent{4}, 4) || eq(price_transparent{4}, 4)));
    EXPECT_TRUE((!lt(price_transparent{4}, 4)));

    EXPECT_TRUE(( lt(4, price_transparent{6})));
    EXPECT_TRUE(( lt(4, price_transparent{6}) || eq(4, price_transparent{6})));
    EXPECT_TRUE((!lt(4, price_transparent{2}) && !eq(4, price_transparent{2})));
    EXPECT_TRUE((!lt(4, price_transparent{2})));
    EXPECT_TRUE(( lt(4, price_transparent{4}) || eq(4, price_transparent{4})));
    EXPECT_TRUE((!lt(4, price_transparent{4})));
}

TEST(test_named_type, to_and_from_underlying_transparent_ordered_heterogeneous_lookup)
{
    using value = price_transparent;

    std::map<value, std::string> M;
    M[value{1}] = "one";

    ASSERT_THAT(M.contains(value{1}), Eq(true));
    ASSERT_THAT(M.contains(1), Eq(true));
    EXPECT_THAT(M.find(value{1})->second, Eq("one"));
    EXPECT_THAT(M.find(1)->second, Eq("one"));
}

TEST(test_named_type, to_and_from_T_transparent_ordered)
{
    using value = named_type<
        int,
        struct to_and_from_T_transparent_ordered_test,
        transparent_ops_with<float>,
        vst::op::ordered>;

    static_assert( is_comparable_with<value, float>);
    static_assert( is_ordered<value, float>);

    constexpr auto eq = std::equal_to<value>{};
    constexpr auto lt = std::less<value>{};

    static_assert(value{4} < 6.f);
    static_assert(value{4} <= 6.f);
    static_assert(value{4} > 2.f);
    static_assert(value{4} >= 2.f);
    static_assert(value{4} <= 4.f);
    static_assert(value{4} >= 4.f);

    static_assert(4.f < value{6});
    static_assert(4.f <= value{6});
    static_assert(4.f > value{2});
    static_assert(4.f >= value{2});
    static_assert(4.f <= value{4});
    static_assert(4.f >= value{4});

    EXPECT_TRUE((value{4} < 6.f));
    EXPECT_TRUE((value{4} <= 6.f));
    EXPECT_TRUE((value{4} > 2.f));
    EXPECT_TRUE((value{4} >= 2.f));
    EXPECT_TRUE((value{4} <= 4.f));
    EXPECT_TRUE((value{4} >= 4.f));

    EXPECT_TRUE((4.f < value{6}));
    EXPECT_TRUE((4.f <= value{6}));
    EXPECT_TRUE((4.f > value{2}));
    EXPECT_TRUE((4.f >= value{2}));
    EXPECT_TRUE((4.f <= value{4}));
    EXPECT_TRUE((4.f >= value{4}));

    static_assert( lt(value{4}, 6.f));
    static_assert( lt(value{4}, 6.f) || eq(value{4}, 6.f));
    static_assert(!lt(value{4}, 2.f) && !eq(value{4}, 2.f));
    static_assert(!lt(value{4}, 2.f));
    static_assert( lt(value{4}, 4.f) || eq(value{4}, 4.f));
    static_assert(!lt(value{4}, 4.f));

    static_assert( lt(4.f, value{6}));
    static_assert( lt(4.f, value{6}) || eq(4.f, value{6}));
    static_assert(!lt(4.f, value{2}) && !eq(4.f, value{2}));
    static_assert(!lt(4.f, value{2}));
    static_assert( lt(4.f, value{4}) || eq(4.f, value{4}));
    static_assert(!lt(4.f, value{4}));

    EXPECT_TRUE(( lt(value{4}, 6.f)));
    EXPECT_TRUE(( lt(value{4}, 6.f) || eq(value{4}, 6.f)));
    EXPECT_TRUE((!lt(value{4}, 2.f) && !eq(value{4}, 2.f)));
    EXPECT_TRUE((!lt(value{4}, 2.f)));
    EXPECT_TRUE(( lt(value{4}, 4.f) || eq(value{4}, 4.f)));
    EXPECT_TRUE((!lt(value{4}, 4.f)));

    EXPECT_TRUE(( lt(4.f, value{6})));
    EXPECT_TRUE(( lt(4.f, value{6}) || eq(4.f, value{6})));
    EXPECT_TRUE((!lt(4.f, value{2}) && !eq(4.f, value{2})));
    EXPECT_TRUE((!lt(4.f, value{2})));
    EXPECT_TRUE(( lt(4.f, value{4}) || eq(4.f, value{4})));
    EXPECT_TRUE((!lt(4.f, value{4})));
}

TEST(test_named_type, to_and_from_T_transparent_ordered_heterogeneous_lookup)
{
    using value = named_type<
        int,
        struct to_and_from_T_transparent_ordered_test,
        transparent_ops_with<float>,
        vst::op::ordered>;

    std::map<value, std::string> M;
    M[value{1}] = "one";

    ASSERT_THAT(M.contains(value{1}), Eq(true));
    ASSERT_THAT(M.contains(1.f), Eq(true));
    EXPECT_THAT(M.find(value{1})->second, Eq("one"));
    EXPECT_THAT(M.find(1.f)->second, Eq("one"));
}

TEST(test_named_type, to_and_from_underlying_transparent_hashable)
{
    static_assert( is_hashable<price_transparent, int>);
    auto vh = [](const auto& o) { return vst::hash<price_transparent>{}(o); };
    auto sh = [](const auto& o) { return std::hash<price_transparent>{}(o); };
    auto bh = [](const auto& o) { return boost::hash<price_transparent>{}(o); };

    auto test = [](const auto& h)
    {
        EXPECT_TRUE((h(price_transparent{1}) == h(1)));
        EXPECT_TRUE((h(price_transparent{1}) != h(2)));
        EXPECT_TRUE((h(price_transparent{2}) != h(1)));
    };
    std::apply([&test](const auto&&... f) {
        (test(f), ...);
    }, std::tuple{vh, sh, bh});
}

TEST(test_named_type, to_and_from_underlying_transparent_hashable_heterogeneous_lookup)
{
    using value = price_transparent;

    std::unordered_map<value, std::string> M;
    M[value{1}] = "one";

    ASSERT_THAT(M.contains(value{1}), Eq(true));
    ASSERT_THAT(M.contains(1), Eq(true));
    EXPECT_THAT(M.find(value{1})->second, Eq("one"));
    EXPECT_THAT(M.find(1)->second, Eq("one"));
}

TEST(test_named_type, to_and_from_T_transparent_hashable)
{
    using value = named_type<
        int,
        struct to_and_from_T_transparent_ordered_test,
        transparent_ops_with<float>,
        vst::op::hashable>;

    static_assert( is_hashable<value, float>);
    auto vh = [](const auto& o) { return vst::hash<value>{}(o); };
    auto sh = [](const auto& o) { return std::hash<value>{}(o); };
    auto bh = [](const auto& o) { return boost::hash<value>{}(o); };

    auto test = [](const auto& h)
    {
        EXPECT_THAT(h(value{1}), Eq(h(1.f)));
        EXPECT_THAT(h(value{1}), Ne(h(2.f)));
        EXPECT_THAT(h(value{2}), Ne(h(1.f)));
    };
    std::apply([&test](const auto&&... f) {
        (test(f), ...);
    }, std::tuple{vh, sh, bh});
}

TEST(test_named_type, to_and_T_transparent_hashable_heterogeneous_lookup)
{
    using value = named_type<
        int,
        struct to_and_from_T_transparent_ordered_test,
        transparent_ops_with<float>,
        vst::op::hashable>;

    std::unordered_map<value, std::string> M;
    M[value{1}] = "one";

    ASSERT_THAT(M.contains(value{1}), Eq(true));
    ASSERT_THAT(M.contains(1.f), Eq(true));
    EXPECT_THAT(M.find(value{1})->second, Eq("one"));
    EXPECT_THAT(M.find(1.f)->second, Eq("one"));
}

TEST(test_named_type, to_and_from_underlying_transparent_addable)
{
    static_assert( dev::Addable<price_transparent>);      // has explcit addable, price to price
    static_assert(!dev::AddableWith<price_transparent, int>); // but no implcit addable to underlying
}

TEST(test_named_type, heterogeneous_lookup_std_set)
{
    // GIVEN
    std::set<price_transparent> c;

    // WHEN
    c.insert(price_transparent{4});
    c.insert(price_transparent{5});
    c.insert(price_transparent{1});

    // THEN
    EXPECT_THAT(c.find(5), Eq(c.find(price_transparent{5})));
}

TEST(test_named_type, heterogeneous_lookup_std_map)
{
    // GIVEN
    std::map<price_transparent, std::string> c;

    // WHEN
    c[price_transparent{5}] = "5";
    c[price_transparent{1}] = "1";
    c[price_transparent{4}] = "4";

    // THEN
    EXPECT_THAT(c.find(5), Eq(c.find(price_transparent{5})));
}

TEST(test_named_type, heterogeneous_lookup_boost_ordered_index)
{
    // GIVEN
    namespace bmi = boost::multi_index;

    using index_t = boost::multi_index_container<
        price_transparent,
        bmi::indexed_by<bmi::ordered_unique<bmi::identity<price_transparent>>>>;

    index_t c;

    // WHEN
    c.insert(price_transparent{5});
    c.insert(price_transparent{5});
    c.insert(price_transparent{1});

    // THEN
    EXPECT_THAT(c.find(5), Eq(c.find(price_transparent{5})));
}

TEST(test_named_type, heterogeneous_lookup_boost_hashed_index)
{
    // GIVEN
    namespace bmi = boost::multi_index;

    using index_t = boost::multi_index_container<
        price_transparent,
        bmi::indexed_by<bmi::hashed_unique<bmi::identity<price_transparent>>>>;

    index_t c;

    // WHEN
    c.insert(price_transparent{5});
    c.insert(price_transparent{5});
    c.insert(price_transparent{1});

    // THEN
    EXPECT_THAT(c.find(5), Ne(std::end(c)));
}

// TODO MG:
//  * configurable comparisons to underlying?
//  * ref-types