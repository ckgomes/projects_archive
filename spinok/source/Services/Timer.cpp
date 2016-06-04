#include "Timer.hpp"

Timer::Timer()
{
    reset();
}

f64 Timer::elapsed() const
{
    auto now = std::chrono::system_clock::now();
    return std::chrono::duration_cast<std::chrono::nanoseconds>(now - m_start).count() / 1E9;
}

void Timer::reset()
{
    m_start = m_current = std::chrono::system_clock::now();
}
