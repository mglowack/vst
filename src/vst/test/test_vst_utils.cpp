#include <vst_utils.h>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

using namespace ::testing;

TEST(test_vst, apply_with_index_return)
{
    auto add_values = [](const auto&... elem) { return (elem.value + ...); };
    auto add_indices_and_values = [](const auto&... elem) { return (elem.index + ...) + (elem.value + ...); };

    const std::tuple t_const{3, 4, 5};
    std::tuple t_non_const{3, 4, 5};

    EXPECT_THAT(apply_with_index(add_indices_and_values, std::tuple{3, 4, 5}), Eq(0+3 + 1+4 + 2+5));
    EXPECT_THAT(apply_with_index(add_indices_and_values, t_const),             Eq(0+3 + 1+4 + 2+5));
    EXPECT_THAT(apply_with_index(add_indices_and_values, t_non_const),         Eq(0+3 + 1+4 + 2+5));

    EXPECT_THAT(apply_with_index(add_values, std::tuple{3, 4, 5}), Eq(3+4+5));
    EXPECT_THAT(apply_with_index(add_values, t_const),             Eq(3+4+5));
    EXPECT_THAT(apply_with_index(add_values, t_non_const),         Eq(3+4+5));
}

TEST(test_vst, apply_with_index)
{
    MockFunction<void(value_with_index<0, const int>, value_with_index<1, const float>)> mock;
    const std::tuple<int, float> a{1, 2.f};

    EXPECT_CALL(mock, Call(
        value_with_index<0, const int>{1},
        value_with_index<1, const float>{2.f}));

    apply_with_index(mock.AsStdFunction(), a);
}

TEST(test_vst, apply_with_index_non_const)
{
    MockFunction<void(value_with_index<0, int>, value_with_index<1, float>)> mock;
    std::tuple<int, float> a{1, 2.f};

    int i = 1;
    float f = 2.f;
    EXPECT_CALL(mock, Call(
        value_with_index<0, int>{i},
        value_with_index<1, float>{f})).Times(2);

    apply_with_index(mock.AsStdFunction(), a);
    apply_with_index(mock.AsStdFunction(), std::tuple<int, float>{1, 2.f});
}