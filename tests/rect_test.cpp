#include "engine/Rect.hpp"

#include <gtest/gtest.h>

TEST(Rect, DefaultConstructsToZero)
{
    const engine::FloatRect r;

    EXPECT_FLOAT_EQ(r.left, 0.f);
    EXPECT_FLOAT_EQ(r.top, 0.f);
    EXPECT_FLOAT_EQ(r.width, 0.f);
    EXPECT_FLOAT_EQ(r.height, 0.f);
}

TEST(Rect, ContainsInteriorPoint)
{
    const engine::FloatRect r{0.f, 0.f, 10.f, 10.f};

    EXPECT_TRUE(r.contains({5.f, 5.f}));
}

TEST(Rect, DoesNotContainOutsidePoint)
{
    const engine::FloatRect r{0.f, 0.f, 10.f, 10.f};

    EXPECT_FALSE(r.contains({15.f, 5.f}));
    EXPECT_FALSE(r.contains({5.f, -1.f}));
}

TEST(Rect, IntersectsOverlappingRect)
{
    const engine::FloatRect a{0.f, 0.f, 10.f, 10.f};
    const engine::FloatRect b{5.f, 5.f, 10.f, 10.f};

    EXPECT_TRUE(a.intersects(b));
    EXPECT_TRUE(b.intersects(a));
}

TEST(Rect, DoesNotIntersectDisjointRect)
{
    const engine::FloatRect a{0.f, 0.f, 10.f, 10.f};
    const engine::FloatRect b{20.f, 20.f, 5.f, 5.f};

    EXPECT_FALSE(a.intersects(b));
}

TEST(Rect, DoesNotIntersectWhenOnlyEdgesTouch)
{
    const engine::FloatRect a{0.f, 0.f, 10.f, 10.f};
    const engine::FloatRect b{10.f, 0.f, 10.f, 10.f};

    EXPECT_FALSE(a.intersects(b));
}
