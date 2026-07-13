#pragma once

#include <string>
#include <vector>

// Loads and serves the stage walls from a directory of .txt files:
// 15 lines of 15 characters, one per grid cell, top to bottom. Files
// are ordered by name; lines starting with '#' are comments.
class CStageStore
{
  public:
    // True when at least one valid stage file was loaded.
    bool load(const std::string& dir);

    unsigned int count() const;

    // Row-major cell ids (game_bricks) for the given stage, or nullptr
    // when out of range.
    const std::vector<unsigned int>* get(unsigned int stage) const;

    // True when the stage holds at least one destructible brick, so it
    // can actually be cleared.
    bool has_bricks(unsigned int stage) const;

    // The next stage after `from` with bricks, wrapping to 0 past the
    // last one.
    unsigned int next_stage_with_bricks(unsigned int from) const;

    // The character <-> cell id mapping shared with the map editor.
    static int cell_from_char(char c);
    static char char_from_cell(unsigned int id);

  private:
    std::vector<std::vector<unsigned int>> stages;
};
