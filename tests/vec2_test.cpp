#include "engine/Vec2.hpp"

#include <gtest/gtest.h>

TEST(Vec2, DefaultConstructsToZero)
{
    const engine::Vec2f v;

    EXPECT_FLOAT_EQ(v.x, 0.f);
    EXPECT_FLOAT_EQ(v.y, 0.f);
}

TEST(Vec2, AddsComponentWise)
{
    const engine::Vec2f a{1.f, 2.f};
    const engine::Vec2f b{3.f, 4.f};

    const engine::Vec2f sum = a + b;

    EXPECT_FLOAT_EQ(sum.x, 4.f);
    EXPECT_FLOAT_EQ(sum.y, 6.f);
}

TEST(Vec2, SubtractsComponentWise)
{
    const engine::Vec2f a{5.f, 7.f};
    const engine::Vec2f b{2.f, 3.f};

    const engine::Vec2f diff = a - b;

    EXPECT_FLOAT_EQ(diff.x, 3.f);
    EXPECT_FLOAT_EQ(diff.y, 4.f);
}

TEST(Vec2, ScalesByScalar)
{
    const engine::Vec2f v{2.f, -3.f};

    const engine::Vec2f scaled = v * 2.f;

    EXPECT_FLOAT_EQ(scaled.x, 4.f);
    EXPECT_FLOAT_EQ(scaled.y, -6.f);
}

TEST(Vec2, CompoundAssignmentAccumulates)
{
    engine::Vec2f v{1.f, 1.f};

    v += engine::Vec2f{2.f, 3.f};

    EXPECT_FLOAT_EQ(v.x, 3.f);
    EXPECT_FLOAT_EQ(v.y, 4.f);
}

TEST(Vec2, ComparesForEquality)
{
    const engine::Vec2i a{1, 2};
    const engine::Vec2i b{1, 2};
    const engine::Vec2i c{2, 1};

    EXPECT_EQ(a, b);
    EXPECT_NE(a, c);
}
