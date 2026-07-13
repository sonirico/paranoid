#include "other_functions.hpp"

const unsigned int* get_stage_map(unsigned int stage)
{
    if (stage >= game::game_bricks::TOTAL_STAGES)
    {
        return nullptr;
    }

    return &(game::game_bricks::brick_map[stage][0]);
}
