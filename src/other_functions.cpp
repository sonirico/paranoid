#include "other_functions.hpp"

const unsigned int* get_stage_map(unsigned int stage)
{
    if (stage >= game::game_bricks::TOTAL_STAGES)
    {
        return nullptr;
    }

    return &(game::game_bricks::brick_map[stage][0]);
}

bool stage_has_bricks(unsigned int stage)
{
    const unsigned int* map = get_stage_map(stage);

    if (map == nullptr)
    {
        return false;
    }

    for (unsigned int i = 0; i < game::game_bricks::BRICKS_PER_LINE * game::game_bricks::LINES; ++i)
    {
        if (map[i] != game::game_bricks::NONE)
        {
            return true;
        }
    }

    return false;
}

unsigned int next_stage_with_bricks(unsigned int from)
{
    for (unsigned int stage = from + 1; stage < game::game_bricks::TOTAL_STAGES; ++stage)
    {
        if (stage_has_bricks(stage))
        {
            return stage;
        }
    }

    return 0;
}
