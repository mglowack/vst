#if 0
#include <vst.hpp>

#include <dev_stringify.h>
#include <dev_concepts.h>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/indexed_by.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/member.hpp>

#include <sstream>
#include <unordered_set>
#include <unordered_map>
#include <set>
#include <map>

using namespace ::testing;

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

// ########################
// # simple_just_ptrs_pod #
// ########################

struct simple_just_ptrs_pod {
    int x;
    float y;

    static constexpr auto get_fields() {
        return std::tuple{
            &simple_just_ptrs_pod::x,
            &simple_just_ptrs_pod::y};
    }
};

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

// #################
// # composite_pod #
// #################

// leveraging aggregate initialization to support all typed tests
struct float_pod {
    float f;
};

struct composite_pod {
    int x;
    vst::type<float_pod, vst::op::ordered, vst::op::hashable, vst::op::addable> y; // to support all typed tests
};

// ########
// # vsts #
// ########

template<typename... ops>
using simple                = vst::type<simple_pod, ops...>;

template<typename... ops>
using simple_just_ptrs      = vst::type<simple_just_ptrs_pod, ops...>;

template<typename... ops>
using simple_self_described = vst::type<simple_self_described_pod, ops...>;

template<typename... ops>
using simple_explicit_default = vst::type<simple_self_described_pod, vst::with_fields::use_default, ops...>;

template<typename... ops>
using custom_from_func      = vst::type<simple_pod,
                                        vst::with_fields::from_func<get_simple_pod_fields>,
                                        ops...>;

template<typename... ops>
using custom_from_var       = vst::type<simple_pod,
                                        vst::with_fields::from_var<&k_simple_pod_fields>,
                                        ops...>;

template<typename... ops>
using composite = vst::type<composite_pod, ops...>;

// ########################
// # append_template_args #
// ########################

template<typename T, typename... extra_args_t>
struct append_template_args;

template<typename T, typename... args_t, typename... extra_args_t>
struct append_template_args<vst::type<T, args_t...>, extra_args_t...>
{
    using type = vst::type<T, args_t..., extra_args_t...>;
};

template<typename T>
concept Hashable = requires(const T& x) {
    { vst::hash<T>{}(x) } -> std::same_as<size_t>;
    requires dev::Hashable<T>;
    { boost::hash<T>{}(x) } -> std::same_as<size_t>;
};

} // close anon namespace

TEST(test_vst, empty)
{
    struct empty_pod {};
    using empty = vst::type<empty_pod, vst::with_fields::empty>;

    static_assert(empty{} == empty{});

    EXPECT_THAT(empty{}, Eq(empty{}));
}

template <typename T>
class test_vst : public ::testing::Test {};

using all_types = ::testing::Types<
    simple<>,
    simple_just_ptrs<>,
    simple_self_described<>,
    simple_explicit_default<>,
    custom_from_func<>,
    custom_from_var<>,
    composite<>
>;

TYPED_TEST_SUITE(test_vst, all_types);

TYPED_TEST(test_vst, regular)
{
    static_assert(std::regular<TypeParam>);
}

TYPED_TEST(test_vst, comparable)
{
    using VST = TypeParam;

    static_assert(dev::Streamable<VST>);
    static_assert(std::equality_comparable<VST>);
    static_assert(!std::totally_ordered<VST>);
    static_assert(!Hashable<VST>);
    static_assert(!dev::Addable<VST>);

    static_assert(VST{1, 2.f} == VST{1, 2.f});
    static_assert(VST{2, 2.f} == VST{2, 2.f});
    static_assert(VST{2, 2.f} != VST{3, 2.f});
    static_assert(VST{2, 2.f} != VST{2, 1.f});

    EXPECT_THAT((VST{1, 2.f}), Eq(VST{1, 2.f}));
    EXPECT_THAT((VST{2, 2.f}), Eq(VST{2, 2.f}));
    EXPECT_THAT((VST{2, 2.f}), Ne(VST{3, 2.f}));
    EXPECT_THAT((VST{2, 2.f}), Ne(VST{2, 1.f}));
}

TYPED_TEST(test_vst, ordered)
{
    using VST = typename append_template_args<TypeParam, vst::op::ordered>::type;

    static_assert(dev::Streamable<VST>);
    static_assert(std::equality_comparable<VST>);
    static_assert(std::totally_ordered<VST>);
    static_assert(!Hashable<VST>);
    static_assert(!dev::Addable<VST>);

    static_assert(VST{1, 2.f} <= VST{1, 2.f});
    static_assert(VST{1, 2.f} >= VST{1, 2.f});
    static_assert(VST{2, 2.f} < VST{3, 2.f});
    static_assert(VST{2, 2.f} < VST{2, 3.f});
    static_assert(VST{2, 2.f} < VST{3, 3.f});
    static_assert(VST{2, 2.f} > VST{2, 1.f});
    static_assert(VST{2, 2.f} > VST{1, 2.f});
    static_assert(VST{2, 2.f} > VST{1, 1.f});

    EXPECT_THAT((VST{1, 2.f}), Le(VST{1, 2.f}));
    EXPECT_THAT((VST{1, 2.f}), Ge(VST{1, 2.f}));
    EXPECT_THAT((VST{2, 2.f}), Lt(VST{3, 2.f}));
    EXPECT_THAT((VST{2, 2.f}), Lt(VST{2, 3.f}));
    EXPECT_THAT((VST{2, 2.f}), Lt(VST{3, 3.f}));
    EXPECT_THAT((VST{2, 2.f}), Gt(VST{2, 1.f}));
    EXPECT_THAT((VST{2, 2.f}), Gt(VST{1, 2.f}));
    EXPECT_THAT((VST{2, 2.f}), Gt(VST{1, 1.f}));
}

TYPED_TEST(test_vst, set)
{
    using VST = typename append_template_args<TypeParam, vst::op::ordered>::type;

    // GIVEN
    std::set<VST> c;

    // WHEN
    c.insert(VST{1, 1.f});
    c.insert(VST{1, 1.f});
    c.insert(VST{2, 2.f});
    c.insert(VST{2, 1.f});
    c.insert(VST{2, 1.f});
    c.insert(VST{1, 3.f});

    // THEN
    EXPECT_THAT(c, ElementsAre(
        VST{1, 1.f},
        VST{1, 3.f},
        VST{2, 1.f},
        VST{2, 2.f}));
}

TYPED_TEST(test_vst, map)
{
    using VST = typename append_template_args<TypeParam, vst::op::ordered>::type;

    // GIVEN
    std::map<VST, int> c;

    // WHEN
    c.insert(std::make_pair(VST{1, 1.f}, 1));
    c.insert(std::make_pair(VST{1, 1.f}, 2));
    c.insert(std::make_pair(VST{2, 2.f}, 3));
    c.insert(std::make_pair(VST{2, 1.f}, 4));
    c.insert(std::make_pair(VST{2, 1.f}, 5));
    c.insert(std::make_pair(VST{1, 3.f}, 6));

    // THEN
    EXPECT_THAT(c, UnorderedElementsAre(
        Pair(VST{1, 1.f}, 1),
        Pair(VST{1, 3.f}, 6),
        Pair(VST{2, 1.f}, 4),
        Pair(VST{2, 2.f}, 3)));
}

TYPED_TEST(test_vst, boost_ordered)
{
    using VST = typename append_template_args<TypeParam, vst::op::ordered>::type;

    // GIVEN
    namespace bmi = boost::multi_index;

    using index_t = boost::multi_index_container<
        VST,
        bmi::indexed_by<bmi::ordered_unique<bmi::identity<VST>>>>;

    index_t c;

    // WHEN
    c.insert(VST{1, 1.f});
    c.insert(VST{1, 1.f});
    c.insert(VST{2, 2.f});
    c.insert(VST{2, 1.f});
    c.insert(VST{2, 1.f});
    c.insert(VST{1, 3.f});

    // THEN
    EXPECT_THAT(c, ElementsAre(
        VST{1, 1.f},
        VST{1, 3.f},
        VST{2, 1.f},
        VST{2, 2.f}));
}

TYPED_TEST(test_vst, hashable)
{
    using VST = typename append_template_args<TypeParam, vst::op::hashable>::type;

    static_assert(dev::Streamable<VST>);
    static_assert(std::equality_comparable<VST>);
    static_assert(!std::totally_ordered<VST>);
    static_assert(Hashable<VST>);
    static_assert(!dev::Addable<VST>);

    auto vh = [](const VST& o) { return vst::hash<VST>{}(o); };
    auto sh = [](const VST& o) { return std::hash<VST>{}(o); };
    auto bh = [](const VST& o) { return boost::hash<VST>{}(o); };

    EXPECT_THAT((vh(VST{1, 2.f})), Eq(sh(VST{1, 2.f})));
    EXPECT_THAT((vh(VST{1, 2.f})), Eq(bh(VST{1, 2.f})));

    EXPECT_THAT((vh(VST{1, 2.f})), Eq(vh(VST{1, 2.f})));
    EXPECT_THAT((vh(VST{1, 2.f})), Ne(vh(VST{2, 2.f})));
    EXPECT_THAT((vh(VST{1, 2.f})), Ne(vh(VST{1, 1.f})));
}

TYPED_TEST(test_vst, unordered_set)
{
    using VST = typename append_template_args<TypeParam, vst::op::hashable>::type;

    // GIVEN
    std::unordered_set<VST> c;

    // WHEN
    c.insert(VST{1, 1.f});
    c.insert(VST{1, 1.f});
    c.insert(VST{2, 2.f});
    c.insert(VST{2, 1.f});
    c.insert(VST{2, 1.f});
    c.insert(VST{1, 3.f});

    // THEN
    EXPECT_THAT(c, UnorderedElementsAre(
        VST{1, 1.f},
        VST{2, 2.f},
        VST{2, 1.f},
        VST{1, 3.f}));
}

TYPED_TEST(test_vst, unordered_map)
{
    using VST = typename append_template_args<TypeParam, vst::op::hashable>::type;

    // GIVEN
    std::unordered_map<VST, int> c;

    // WHEN
    c.insert(std::make_pair(VST{1, 1.f}, 1));
    c.insert(std::make_pair(VST{1, 1.f}, 2));
    c.insert(std::make_pair(VST{2, 2.f}, 3));
    c.insert(std::make_pair(VST{2, 1.f}, 4));
    c.insert(std::make_pair(VST{2, 1.f}, 5));
    c.insert(std::make_pair(VST{1, 3.f}, 6));

    // THEN
    EXPECT_THAT(c, UnorderedElementsAre(
        Pair(VST{1, 1.f}, 1),
        Pair(VST{2, 2.f}, 3),
        Pair(VST{2, 1.f}, 4),
        Pair(VST{1, 3.f}, 6)));
}

TYPED_TEST(test_vst, boost_hashed)
{
    using VST = typename append_template_args<TypeParam, vst::op::hashable>::type;

    // GIVEN
    namespace bmi = boost::multi_index;

    using index_t = boost::multi_index_container<
        VST,
        bmi::indexed_by<bmi::hashed_unique<bmi::identity<VST>>>>;

    index_t c;

    // WHEN
    c.insert(VST{1, 1.f});
    c.insert(VST{1, 1.f});
    c.insert(VST{2, 2.f});
    c.insert(VST{2, 1.f});
    c.insert(VST{2, 1.f});
    c.insert(VST{1, 3.f});

    // THEN
    EXPECT_THAT(c, UnorderedElementsAre(
        VST{1, 1.f},
        VST{2, 2.f},
        VST{2, 1.f},
        VST{1, 3.f}));
}

TYPED_TEST(test_vst, addable)
{
    using VST = typename append_template_args<TypeParam, vst::op::addable>::type;

    static_assert(dev::Streamable<VST>);
    static_assert(std::equality_comparable<VST>);
    static_assert(!std::totally_ordered<VST>);
    static_assert(!Hashable<VST>);
    static_assert(dev::Addable<VST>);

    static_assert(VST{1, 2.f} + VST{2, 2.f} == VST{3, 4.f});
    static_assert(VST{1, 2.f} - VST{2, 2.f} == VST{-1, 0.f});

    EXPECT_THAT((VST{1, 2.f} + VST{2, 2.f}), Eq(VST{3, 4.f}));
    EXPECT_THAT((VST{1, 2.f} - VST{2, 2.f}), Eq(VST{-1, 0.f}));

    VST obj{0, 0.f};
    obj += VST{1, 1.f};
    EXPECT_THAT((obj), Eq(VST{1, 1.f}));
    obj += VST{1, 1.f};
    EXPECT_THAT((obj), Eq(VST{2, 2.f}));
    obj -= VST{0, 1.f};
    EXPECT_THAT((obj), Eq(VST{2, 1.f}));
}

// #############
// # streaming #
// #############

TEST(test_vst, streaming)
{
    EXPECT_THAT(dev::stringify(simple<>{1, 1.f}), Eq("[ field1=1 field2=1 ]"));
    EXPECT_THAT(dev::stringify(simple_just_ptrs<>{1, 1.f}), Eq("[ field1=1 field2=1 ]"));
    EXPECT_THAT(dev::stringify(simple_self_described<>{1, 1.f}), Eq("[ x=1 y=1 ]"));
    EXPECT_THAT(dev::stringify(custom_from_func<>{1, 1.f}), Eq("[ x=1 y=1 ]"));
    EXPECT_THAT(dev::stringify(custom_from_var<>{1, 1.f}), Eq("[ x=1 y=1 ]"));
    EXPECT_THAT(dev::stringify(composite<>{1, 1.f}), Eq("[ field1=1 field2=[ field1=1 ] ]"));
}

// TODO MG:
//  * tests for composition with std containers, optionals, variants etc
//  * add static asserts to help with debugging compiler errors
//  * test ADL?
//  * tags to allow re-using PODs?
//
// TODO MG cleanup:
//  * use wrapped version on modyfying operators to allow customizing (maybe not possible)
#endif