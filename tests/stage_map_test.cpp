#include "other_functions.hpp"

#include <gtest/gtest.h>

TEST(StageMap, ReturnsMapForValidStage)
{
    const unsigned int* map = get_stage_map(0);

    ASSERT_NE(map, nullptr);
}

TEST(StageMap, FirstStageHasBricks)
{
    const unsigned int* map = get_stage_map(0);
    ASSERT_NE(map, nullptr);

    unsigned int bricks = 0;

    for (unsigned int i = 0; i < game::game_bricks::BRICKS_PER_LINE * game::game_bricks::LINES; ++i)
    {
        if (map[i] != game::game_bricks::NONE)
        {
            bricks++;
        }
    }

    EXPECT_GT(bricks, 0u);
}

TEST(StageMap, ReturnsNullForOutOfRangeStage)
{
    EXPECT_EQ(get_stage_map(game::game_bricks::TOTAL_STAGES), nullptr);
    EXPECT_EQ(get_stage_map(9999), nullptr);
}

TEST(StageMap, ReportsWhetherAStageHasBricks)
{
    // All ten stages are designed now; only out-of-range is empty.
    for (unsigned int stage = 0; stage < game::game_bricks::TOTAL_STAGES; ++stage)
    {
        EXPECT_TRUE(stage_has_bricks(stage)) << "stage " << stage;
    }

    EXPECT_FALSE(stage_has_bricks(game::game_bricks::TOTAL_STAGES));
}

TEST(StageMap, AdvancesToNextStageWithBricks)
{
    for (unsigned int stage = 0; stage + 1 < game::game_bricks::TOTAL_STAGES; ++stage)
    {
        EXPECT_EQ(next_stage_with_bricks(stage), stage + 1) << "stage " << stage;
    }
}

TEST(StageMap, WrapsToFirstStageAfterTheLastDesignedOne)
{
    EXPECT_EQ(next_stage_with_bricks(game::game_bricks::TOTAL_STAGES - 1), 0u);
}
