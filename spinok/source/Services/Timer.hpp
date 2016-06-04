#ifndef TIMER_HPP
#define TIMER_HPP

#include "../Common/Configuration.hpp"

#include <chrono>

class Timer final
{

    // As of VC2015 std::chrono is using QueryPerformanceCounter internally on Windows

    std::chrono::time_point<std::chrono::system_clock> m_start;
    std::chrono::time_point<std::chrono::system_clock> m_current;

public:

    Timer();

    // Returns the amount of time elapsed (in seconds) since the last time reset() was called
    f64 elapsed() const;

    // Resets the timer, this will affect the elapsed() method
    void reset();

};

#endif // TIMER_HPP
