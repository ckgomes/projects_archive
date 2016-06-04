#include "Math.hpp"

#include <cmath>
#include <random>

namespace mm
{

static std::random_device rd;
static std::mt19937 mt(rd());

f32 sqrt(f32 number)
{
    return std::sqrtf(number);
}

f64 sqrt(f64 number)
{
    return std::sqrt(number);
}

f32 pow(f32 base, f32 exponent)
{
    return std::powf(base, exponent);
}

f64 pow(f64 base, f64 exponent)
{
    return std::pow(base, exponent);
}

f32 ln(f32 number)
{
    return std::logf(number);
}

f64 ln(f64 number)
{
    return std::log(number);
}

f32 log(f32 number)
{
    return std::log10f(number);
}

f64 log(f64 number)
{
    return std::log(number);
}

f32 sin(f32 theta)
{
    return std::sinf(theta);
}

f64 sin(f64 theta)
{
    return std::sin(theta);
}

f32 asin(f32 radians)
{
    return std::asinf(radians);
}

f64 asin(f64 radians)
{
    return std::asin(radians);
}

f32 cos(f32 theta)
{
    return std::cosf(theta);
}

f64 cos(f64 theta)
{
    return std::cos(theta);
}

f32 acos(f32 radians)
{
    return std::acosf(radians);
}

f64 acos(f64 radians)
{
    return std::acos(radians);
}

f32 tan(f32 theta)
{
    return std::tanf(theta);
}

f64 tan(f64 theta)
{
    return std::tan(theta);
}

f32 atan(f32 radians)
{
    return std::atanf(radians);
}

f64 atan(f64 radians)
{
    return std::atan(radians);
}

f32 atan2(f32 y, f32 x)
{
    return std::atan2f(y, x);
}

f64 atan2(f64 y, f64 x)
{
    return std::atan2(y, x);
}

s32 random(s32 min, s32 max)
{
    std::uniform_int_distribution<decltype(min)> distribution(min, max);
    return distribution(mt);
}

s64 random(s64 min, s64 max)
{
    std::uniform_int_distribution<decltype(min)> distribution(min, max);
    return distribution(mt);
}

f32 random(f32 min, f32 max)
{
    std::uniform_real_distribution<decltype(min)> distribution(min, max);
    return distribution(mt);
}

f64 random(f64 min, f64 max)
{
    std::uniform_real_distribution<decltype(min)> distribution(min, max);
    return distribution(mt);
}

}