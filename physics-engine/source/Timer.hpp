#ifndef TIMER_HPP
#define TIMER_HPP

#include "Configuration.hpp"

#include <chrono>

class Timer final
{

    std::chrono::time_point<std::chrono::system_clock> m_start;
    std::chrono::time_point<std::chrono::system_clock> m_current;

public:

    Timer()
    {
        reset();
    }

    f64 elapsed() const
    {
        auto now = std::chrono::system_clock::now();
        return std::chrono::duration_cast<std::chrono::nanoseconds>(now - m_start).count() / 1E9;
    }
    f64 interval()
    {
        auto now = std::chrono::system_clock::now();
        f64 dt = std::chrono::duration_cast<std::chrono::nanoseconds>(now - m_current).count() / 1E9;
        m_current = now;
        return dt;
    }

    void reset()
    {
        m_start = m_current = std::chrono::system_clock::now();
    }

};

#endif // TIMER_HPP
