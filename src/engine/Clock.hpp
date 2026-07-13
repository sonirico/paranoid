#pragma once

#include <cstdint>

namespace engine
{

// Measures elapsed wall time in seconds between restarts.
class Clock
{
  public:
    Clock();

    float restart();

  private:
    std::uint64_t m_last;
};

} // namespace engine
