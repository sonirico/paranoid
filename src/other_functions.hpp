#pragma once

#include "assets.h"

// Returns the brick layout for a stage, or nullptr when out of range.
const unsigned int* get_stage_map(unsigned int stage);

// True when the stage layout contains at least one brick.
bool stage_has_bricks(unsigned int stage);

// Returns the next stage after `from` that has bricks, wrapping to 0
// past the last designed one. Stage 0 always has bricks.
unsigned int next_stage_with_bricks(unsigned int from);
