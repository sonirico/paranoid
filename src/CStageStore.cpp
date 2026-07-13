#include "CStageStore.hpp"

#include "assets.h"

#include <SDL3/SDL.h>

#include <algorithm>
#include <filesystem>
#include <fstream>

namespace
{

// One character per brick type, indexed by its game_bricks id.
const char CELL_CHARS[] = ".ROYGABPWSDX";

// Reads one stage file into row-major cell ids; empty on any error.
std::vector<unsigned int> parse_stage_file(const std::filesystem::path& path)
{
    using namespace game::game_bricks;

    std::ifstream file(path);

    std::vector<unsigned int> cells;
    std::string line;

    while (cells.size() < BRICKS_PER_LINE * LINES && std::getline(file, line))
    {
        if (line.empty() || line[0] == '#')
        {
            continue;
        }

        if (line.size() != BRICKS_PER_LINE)
        {
            return {};
        }

        for (const char c : line)
        {
            const int cell = CStageStore::cell_from_char(c);

            if (cell < 0)
            {
                return {};
            }

            cells.push_back(static_cast<unsigned int>(cell));
        }
    }

    if (cells.size() != BRICKS_PER_LINE * LINES)
    {
        return {};
    }

    return cells;
}

} // namespace

bool CStageStore::load(const std::string& dir)
{
    this->stages.clear();

    std::error_code ec;
    std::vector<std::filesystem::path> paths;

    for (const auto& entry : std::filesystem::directory_iterator(dir, ec))
    {
        if (entry.path().extension() == ".txt")
        {
            paths.push_back(entry.path());
        }
    }

    if (ec)
    {
        SDL_Log("Stage directory %s: %s", dir.c_str(), ec.message().c_str());
        return false;
    }

    std::sort(paths.begin(), paths.end());

    for (const auto& path : paths)
    {
        std::vector<unsigned int> cells = parse_stage_file(path);

        if (cells.empty())
        {
            SDL_Log("Stage file %s is malformed, skipping it", path.c_str());
            continue;
        }

        this->stages.push_back(std::move(cells));
    }

    return !this->stages.empty();
}

unsigned int CStageStore::count() const
{
    return static_cast<unsigned int>(this->stages.size());
}

const std::vector<unsigned int>* CStageStore::get(unsigned int stage) const
{
    if (stage >= this->stages.size())
    {
        return nullptr;
    }

    return &this->stages[stage];
}

bool CStageStore::has_bricks(unsigned int stage) const
{
    using namespace game::game_bricks;

    const std::vector<unsigned int>* cells = this->get(stage);

    if (cells == nullptr)
    {
        return false;
    }

    for (const unsigned int cell : *cells)
    {
        if (cell != NONE && cell != UNDESTROYABLE)
        {
            return true;
        }
    }

    return false;
}

unsigned int CStageStore::next_stage_with_bricks(unsigned int from) const
{
    for (unsigned int stage = from + 1; stage < this->count(); ++stage)
    {
        if (this->has_bricks(stage))
        {
            return stage;
        }
    }

    return 0;
}

int CStageStore::cell_from_char(char c)
{
    for (unsigned int id = 0; id < game::game_bricks::COUNT; ++id)
    {
        if (CELL_CHARS[id] == c)
        {
            return static_cast<int>(id);
        }
    }

    return -1;
}

char CStageStore::char_from_cell(unsigned int id)
{
    return id < game::game_bricks::COUNT ? CELL_CHARS[id] : '.';
}
