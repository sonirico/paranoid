#include "CStageStore.hpp"
#include "assets.h"

#include <gtest/gtest.h>

// The shipped stage files, loaded from the real media directory.
class StageStoreTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        ASSERT_TRUE(store.load(TEST_MEDIA_DIR "/stages"));
    }

    CStageStore store;
};

TEST_F(StageStoreTest, LoadsElevenStagesFromDisk)
{
    EXPECT_EQ(store.count(), 11u);
}

TEST_F(StageStoreTest, EveryStageHasDestructibleBricks)
{
    for (unsigned int stage = 0; stage < store.count(); ++stage)
    {
        EXPECT_TRUE(store.has_bricks(stage)) << "stage " << stage;
    }

    EXPECT_FALSE(store.has_bricks(store.count()));
}

TEST_F(StageStoreTest, ServesFullGridsAndRejectsOutOfRange)
{
    const std::vector<unsigned int>* cells = store.get(0);

    ASSERT_NE(cells, nullptr);
    EXPECT_EQ(cells->size(), game::game_bricks::BRICKS_PER_LINE * game::game_bricks::LINES);

    EXPECT_EQ(store.get(store.count()), nullptr);
}

TEST_F(StageStoreTest, AdvancesAndWrapsThroughStages)
{
    for (unsigned int stage = 0; stage + 1 < store.count(); ++stage)
    {
        EXPECT_EQ(store.next_stage_with_bricks(stage), stage + 1) << "stage " << stage;
    }

    EXPECT_EQ(store.next_stage_with_bricks(store.count() - 1), 0u);
}

TEST(StageStore, MapsEveryCellIdToACharacterAndBack)
{
    for (unsigned int id = 0; id < game::game_bricks::COUNT; ++id)
    {
        const char c = CStageStore::char_from_cell(id);

        EXPECT_EQ(CStageStore::cell_from_char(c), static_cast<int>(id)) << "id " << id;
    }

    EXPECT_EQ(CStageStore::cell_from_char('?'), -1);
}

TEST(StageStore, RefusesAMissingDirectory)
{
    CStageStore store;

    EXPECT_FALSE(store.load(TEST_MEDIA_DIR "/no-such-dir"));
}
