#ifndef MATH_HPP
#define MATH_HPP

#include "../Common/Configuration.hpp"

#include <limits>

#undef min
#undef max

// Currently a wrapper around the standard math library
// To allow encapsulation and function overloading

namespace mm
{

/// Mathematical constants

template <typename T = precision_t>
constexpr T pi(T m = T(1))
{
    return m * T(3.1415926535897932384626433832795028841971693993751058209749445923078164062);
}

template <typename T = precision_t>
constexpr T e()
{
    return T(2.7182818284590452353602874713526624977572470936999595749669676277240766303);
}

template <typename T = precision_t>
constexpr T epsilon()
{
    return T(0.0001);
}

template <typename T = precision_t>
constexpr T infinity()
{
    return std::numeric_limits<T>::infinity();
}

/// Standard mathematical functions

f32 sqrt(f32 number);
f64 sqrt(f64 number);

f32 pow(f32 base, f32 exponent);
f64 pow(f64 base, f64 exponent);

f32 ln(f32 number);
f64 ln(f64 number);

f32 log(f32 number);
f64 log(f64 number);

f32 sin(f32 theta);
f64 sin(f64 theta);

f32 asin(f32 radians);
f64 asin(f64 radians);

f32 cos(f32 theta);
f64 cos(f64 theta);

f32 acos(f32 radians);
f64 acos(f64 radians);

f32 tan(f32 theta);
f64 tan(f64 theta);

f32 atan(f32 radians);
f64 atan(f64 radians);

f32 atan2(f32 y, f32 x);
f64 atan2(f64 y, f64 x);

/// Special utility functions

s32 random(s32 min, s32 max);
s64 random(s64 min, s64 max);
f32 random(f32 min, f32 max);
f64 random(f64 min, f64 max);

/// Extended mathematical functions

template <typename T = precision_t>
constexpr static inline T max(T a, T b)
{
    return (a > b) ? a : b;
}

template <typename T = precision_t, typename... Args>
constexpr static inline T max(T value, Args... args)
{
    return max( value, max(args...));
}

template <typename T = precision_t>
constexpr static inline T min(T a, T b)
{
    return (a > b) ? b : a;
}

template <typename T = precision_t, typename... Args>
constexpr static inline T min(T value, Args... args)
{
    return min(value, min(args...));
}

template <typename T = precision_t>
constexpr static inline T sq(T number)
{
    return (number * number);
}

template <typename T = precision_t>
constexpr static inline T sign(T number)
{
    return T(number > T(0)) - T(number < T(0));
}

template <typename T = precision_t>
constexpr static inline T clamp(T min, T max, T value)
{
    return (value < min) ? min : (value > max) ? max : value;
}

template <typename T = precision_t, typename K = T>
constexpr static inline T lerp(T min, T max, K t)
{
    return (K(1) - t) * min + t * max;
}

/// Custom implemented mathematical functions

template <typename T = precision_t>
constexpr static inline T abs(T number)
{
    return number * sign<T>(number);
}

}

#endif // MATH_HPP
