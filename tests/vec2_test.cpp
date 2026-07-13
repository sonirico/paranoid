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

TEST(Vec2, LengthIsEuclideanNorm)
{
    const engine::Vec2f v{3.f, 4.f};

    EXPECT_FLOAT_EQ(v.length(), 5.f);
}

TEST(Vec2, NormalizedHasUnitLength)
{
    const engine::Vec2f v{3.f, 4.f};

    const engine::Vec2f unit = v.normalized();

    EXPECT_FLOAT_EQ(unit.x, 0.6f);
    EXPECT_FLOAT_EQ(unit.y, 0.8f);
    EXPECT_FLOAT_EQ(unit.length(), 1.f);
}

TEST(Vec2, NormalizedZeroVectorStaysZero)
{
    const engine::Vec2f v;

    const engine::Vec2f unit = v.normalized();

    EXPECT_FLOAT_EQ(unit.x, 0.f);
    EXPECT_FLOAT_EQ(unit.y, 0.f);
}

TEST(Vec2, PerpendicularRotates90Degrees)
{
    const engine::Vec2f v{2.f, 3.f};

    const engine::Vec2f perp = v.perpendicular();

    EXPECT_FLOAT_EQ(perp.x, -3.f);
    EXPECT_FLOAT_EQ(perp.y, 2.f);
}

TEST(Vec2, ComparesForEquality)
{
    const engine::Vec2i a{1, 2};
    const engine::Vec2i b{1, 2};
    const engine::Vec2i c{2, 1};

    EXPECT_EQ(a, b);
    EXPECT_NE(a, c);
}
