#include <named_type.hpp>
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

    static_assert(!Transparent<price>);
    static_assert( Transparent<price_transparent>);

    // using price_relaxed = named_type<
    //     int,
    //     struct price_tag,
    //     type_list<implicitly_convertible_to<int>, transparently_>,
    //     vst::op::ordered,
    //     vst::op::hashable,
    //     vst::op::addable>;
}

// clang-format off
static_assert(!Transparent<named_type<int, struct default_test                          >>);
static_assert(!Transparent<named_type<int, struct explicit_default_test, default_ops    >>);
static_assert(!Transparent<named_type<int, struct strict_test,           strict_ops     >>);
static_assert( Transparent<named_type<int, struct transparent_test,      transparent_ops>>);

static_assert( TransparentWith<int,   named_type<int, struct transparent_test,      transparent_ops>>);
static_assert(!TransparentWith<float, named_type<int, struct transparent_test,      transparent_ops>>);
static_assert( TransparentWith<int,   named_type<int, struct transparent_test, transparent_ops_with<int>>>);
static_assert(!TransparentWith<float, named_type<int, struct transparent_test, transparent_ops_with<int>>>);
static_assert( TransparentWith<float, named_type<int, struct transparent_test, transparent_ops_with<float>>>);

static_assert(std::is_same_v<type_list<strict_ops>, named_type_trait<named_type<int, struct default_test                          >>::op_categories>);
static_assert(std::is_same_v<type_list<strict_ops>, named_type_trait<named_type<int, struct explicit_default_test, default_ops    >>::op_categories>);
static_assert(std::is_same_v<type_list<strict_ops>, named_type_trait<named_type<int, struct strict_test,           strict_ops     >>::op_categories>);

static_assert(std::is_same_v<type_list<transparent_ops_with<int>>,   named_type_trait<named_type<int, struct transparent_test, transparent_ops>>::op_categories>);
static_assert(std::is_same_v<type_list<transparent_ops_with<int>>,   named_type_trait<named_type<int, struct transparent_test, transparent_ops_with<int>>>::op_categories>);
static_assert(std::is_same_v<type_list<transparent_ops_with<float>>, named_type_trait<named_type<int, struct transparent_test, transparent_ops_with<float>>>::op_categories>);
// clang-format on

template<typename T>
struct implicitly_convertible_to : std::false_type {};

template<>
struct implicitly_convertible_to<float> : std::true_type {};

template<typename T>
concept ImplicitlyConvertibleTo = implicitly_convertible_to<T>::value;

template<bool ctor, bool op>
struct a
{
    int u;

    constexpr explicit(ctor) a(int u) : u(u) {}
    constexpr explicit(op) operator int() const {
        return u;
    }

    template<ImplicitlyConvertibleTo T>
    constexpr operator T() const {
        return u;
    }
};

TEST(test_named_type, explicit_false_ctor)
{
    using price_relaxed_to = named_type<int, struct price_relaxed_to_tag, implicit_conversions_to<int>>;

    static_assert( std::is_convertible_v<price_relaxed_to, int>);
    static_assert(!std::is_convertible_v<int, price_relaxed_to>);

    {
        [[maybe_unused]] int p = price_relaxed_to{5};
    }

    using price_relaxed_from = named_type<int, struct price_relaxed_to_tag, implicit_conversions_from<int>>;

    static_assert(!std::is_convertible_v<price_relaxed_from, int>);
    static_assert( std::is_convertible_v<int, price_relaxed_from>);

    {
        [[maybe_unused]] price_relaxed_from p = 5;
    }

    static_assert(std::is_constructible_v<price, int>);
    static_assert(!std::is_convertible_v<price, int>);
    static_assert(!std::is_convertible_v<int, price>);

    // both ctor and conversion op explicit
    static_assert(!std::is_convertible_v<a<true, true>, int>);
    static_assert(!std::is_convertible_v<int, a<true, true>>);

    // ctor explicit
    static_assert(!std::is_convertible_v<int, a<true, false>>);
    static_assert( std::is_convertible_v<a<true, false>, int>);

    // op explicit
    static_assert(!std::is_convertible_v<a<false, true>, int>);
    static_assert( std::is_convertible_v<int, a<false, true>>);


    static_assert(!std::is_convertible_v<a<true, true>, int>);
    static_assert( std::is_convertible_v<a<true, true>, float>);
    static_assert(!std::is_convertible_v<a<true, true>, double>);
    // static_assert(!std::is_convertible_v<named_type<int, struct xxx, , int>);
    // price p = 4;
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
    static_assert(!is_addable<price, int>);
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

    EXPECT_TRUE(( eq(price_transparent{4}, 4)));
    EXPECT_TRUE((!eq(price_transparent{4}, 2)));
    EXPECT_TRUE(( eq(4, price_transparent{4})));
    EXPECT_TRUE((!eq(2, price_transparent{4})));
}

TEST(test_named_type, to_and_from_T_transparent_equality)
{
    using value = named_type<int, struct to_and_from_T_transparent_equality_test, transparent_ops_with<float>>;
    static_assert( is_comparable<value, float>);

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
    static_assert( is_comparable<value, int>);
    static_assert( is_comparable<value, float>);

    constexpr auto eq = std::equal_to<value>{};

    static_assert(value{4} == 4);
    static_assert(value{4} == 4.f);
    static_assert(eq(value{4}, 4));
    static_assert(eq(value{4}, 4.f));
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

TEST(test_named_type, to_and_from_T_transparent_ordered)
{
    using value = named_type<
        int,
        struct to_and_from_T_transparent_ordered_test,
        transparent_ops_with<float>,
        vst::op::ordered>;

    static_assert( is_comparable<value, float>);
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

TEST(test_named_type, to_and_from_underlying_transparent_addable)
{
    static_assert( is_addable<price_transparent>);      // has explcit addable, price to price
    static_assert(!is_addable<price_transparent, int>); // but no implcit addable to underlying
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