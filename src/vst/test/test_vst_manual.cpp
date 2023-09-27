#include <vst.hpp>
#include <dev_stringify.h>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

using namespace ::testing;

// #######################
// # custom constructors #
// #######################

TEST(test_vst, custom_ctor)
{
    struct custom_ctor_pod {
        int x, y;

        // NOTE: default ctor *not required*
        explicit custom_ctor_pod(int z) : x(z), y(2*z) {}
    };
    using custom_ctor = vst::type<custom_ctor_pod>;

    custom_ctor obj(5);

    EXPECT_THAT(obj.x, Eq(5));
    EXPECT_THAT(obj.y, Eq(10));
}

// ###################################
// # manual overloading of operators #
// ###################################

namespace manual
{

struct override_pod {
    int x, y;
};
using override = vst::type<override_pod, vst::op::hashable>;

struct nested_override_pod {
    override f;
};
using nested_override = vst::type<nested_override_pod, vst::op::hashable>;

constexpr bool operator==(const override&, const override&) {
    return true;
}

} // close manual namespace

namespace vst
{

template <>
struct hash<manual::override>
{
    size_t operator()(const manual::override&) const noexcept
    {
        return 42;
    }
};

}

TEST(test_vst, manual_override)
{
    EXPECT_THAT((manual::override{1, 2}), Eq(manual::override{1, 1}));
    EXPECT_THAT((manual::override{1, 2}), Eq(manual::override{1, 1})); // because 'operator==' always returns true
    EXPECT_THAT((vst::hash_value(manual::override{1, 2})), Eq(42));

    EXPECT_THAT((manual::nested_override{manual::override{1, 2}}), Eq(manual::nested_override{manual::override{1, 1}}));
    EXPECT_THAT((manual::nested_override{manual::override{1, 2}}), Eq(manual::nested_override{manual::override{1, 1}})); // because 'operator==' always returns true
    // TODO: how do I test for this?
    // EXPECT_THAT((vst::hash_value(manual::nested_override{manual::override{1, 2}})), Eq(42));
}

// #######################################################
// # manual overloading of operators on a per type basis #
// #######################################################

namespace manual
{
    struct string_int {
        std::string number;
        friend constexpr auto operator<=>(const string_int&, const string_int&) = default;
    };

    std::ostream& operator<<(std::ostream& os, const string_int& rhs)
    {
        return os << rhs.number;
    }

    // VSTs
    struct specific_data_pod {
        string_int s;
    };
    static constexpr auto specific_data_pod_fields = std::tuple{MEMBER(specific_data_pod, s)};
    using specific_data = vst::type<specific_data_pod, vst::op::ordered>;
    using specific_data_named = vst::type<
        specific_data_pod,
        vst::with_fields::from_var<&specific_data_pod_fields>,
        vst::op::ordered>;


    // OPERATORS

    // NOTE: this overrides the stream operator of 'string_int'
    //       but ONLY WHEN printed as part of ANY vst
    std::ostream& operator<<(std::ostream& os, const vst::wrapped_value<string_int>& rhs)
    {
        return os << "int:\"" << std::atoi(rhs.value.number.c_str()) << "\"";
    }

    // NOTE: this overrides the stream operator of 'string_int'
    //       but ONLY WHEN printed as part 'specific_data' or 'specific_data_named' vst
    std::ostream& operator<<(std::ostream& os,
                             const vst::wrapped_value_of<specific_data, string_int>& rhs)
    {
        return os << "specific:\"" << std::atoi(rhs.value.number.c_str()) << "\"";
    }

    std::ostream& operator<<(std::ostream& os,
                             const vst::wrapped_value_of<specific_data_named, string_int>& rhs)
    {
        return os << "specific_named:\"" << std::atoi(rhs.value.number.c_str()) << "\"";
    }

    // NOTE: 'string_int' already has operators defined,
    // but we want to have different semantics in our code base
    // for that specific type e.g. we want to use the string
    // as if it was an integer like so:
    bool operator<(const vst::wrapped_value<string_int>& lhs, const vst::wrapped_value<string_int>& rhs)
    {
        return std::atoi(lhs.value.number.c_str()) < std::atoi(rhs.value.number.c_str());
    }

    // NOTE: 'string_int' as part of 'specific_data' or 'specific_data_named' falls back
    // to original string compare method
    bool operator<(const vst::wrapped_value_of<specific_data, string_int>& lhs,
                   const vst::wrapped_value_of<specific_data, string_int>& rhs)
    {
        // fall back to the original operator
        return lhs.value < rhs.value;
    }

    bool operator<(const vst::wrapped_value_of<specific_data_named, string_int>& lhs,
                   const vst::wrapped_value_of<specific_data_named, string_int>& rhs)
    {
        // fall back to the original operator
        return lhs.value < rhs.value;
    }
}

TEST(test_vst, wrapped_value)
{
    struct fake_vst {};
    using W = vst::wrapped_value<manual::string_int>;
    using WF = vst::wrapped_value_of<fake_vst, manual::string_int>;
    using WS = vst::wrapped_value_of<manual::specific_data, manual::string_int>;

    EXPECT_THAT(dev::stringify(   manual::string_int{"10"}),  Eq("10"));
    EXPECT_THAT(dev::stringify(W{ manual::string_int{"10"}}), Eq("int:\"10\""));
    EXPECT_THAT(dev::stringify(WF{manual::string_int{"10"}}), Eq("int:\"10\""));
    EXPECT_THAT(dev::stringify(WS{manual::string_int{"10"}}), Eq("specific:\"10\""));

    EXPECT_THAT(   manual::string_int{"10"},  Lt(   manual::string_int{"4"}));  // because of string compare, not actual int value
    EXPECT_THAT(W{ manual::string_int{"10"}}, Gt(W{ manual::string_int{"4"}})); // because now we compare the actual int value
    EXPECT_THAT(WF{manual::string_int{"10"}}, Gt(WF{manual::string_int{"4"}})); // because now we compare the actual int value
    EXPECT_THAT(WS{manual::string_int{"10"}}, Lt(WS{manual::string_int{"4"}})); // because now we compare the string again
}

TEST(test_vst, custom_operators_for_string_int)
{
    struct pod {
        manual::string_int s;
    };
    static constexpr auto pod_fields = std::tuple{MEMBER(pod, s)};
    using data_named = vst::type<pod, vst::with_fields::from_var<&pod_fields>, vst::op::ordered>;
    using data = vst::type<pod, vst::op::ordered>;

    // check standard operators work as if it's string
    ASSERT_THAT(dev::stringify(manual::string_int{"10"}), Eq("10"));
    ASSERT_THAT(manual::string_int{"10"}, Lt(manual::string_int{"4"}));

    // check wrapped values
    using WD = vst::wrapped_value_of<data, manual::string_int>;
    ASSERT_THAT(dev::stringify(WD{manual::string_int{"10"}}), Eq("int:\"10\""));
    ASSERT_THAT(WD{manual::string_int{"10"}}, Gt(WD{manual::string_int{"4"}}));

    // stream operator override for all VSTs
    EXPECT_THAT(dev::stringify(data{"10"}), Eq("[ field1=int:\"10\" ]"));
    EXPECT_THAT(dev::stringify(data_named{"10"}), Eq("[ s=int:\"10\" ]"));

    // stream operator override for specific VST
    EXPECT_THAT(dev::stringify(manual::specific_data{"10"}), Eq("[ field1=specific:\"10\" ]"));
    EXPECT_THAT(dev::stringify(manual::specific_data_named{"10"}), Eq("[ s=specific_named:\"10\" ]"));

    // compare operators override for all VSTs (defined to be int comparison)
    EXPECT_THAT((data{"10"}), Gt(data{"4"}));
    EXPECT_THAT((data_named{"10"}), Gt(data_named{"4"}));

    // compare operators override for specific VSTs (defined to be using string comparison)
    EXPECT_THAT((manual::specific_data{"10"}), Lt(manual::specific_data{"4"}));
    EXPECT_THAT((manual::specific_data_named{"10"}), Lt(manual::specific_data_named{"4"}));
}

TEST(test_vst, built_in_comparison_for_const_char)
{
    struct pod {
        int x;
        const char* s;
    };
    using data = vst::type<pod, vst::op::ordered>;

    std::string s1 = "aaa";
    std::string s2 = "bbb";

    // make sure the one with lower address has the lexicographically higher value
    if (s1.c_str() < s2.c_str())
    {
        // needs swapping
        s1 = "bbb";
        s2 = "aaa";

        // make sure pointer arithmetic would give the wrong answer lexicographically
        ASSERT_TRUE((s1.c_str() < s2.c_str() && s1 > s2));
        EXPECT_THAT((data{4, s1.c_str()}), Gt(data{4, s2.c_str()}));
    }
    else
    {
        // make sure pointer arithmetic would give the wrong answer lexicographically
        ASSERT_TRUE((s1.c_str() > s2.c_str() && s1 < s2));
        EXPECT_THAT((data{4, s1.c_str()}), Lt(data{4, s2.c_str()}));
    }
}