#ifndef CONFIGURATION_HPP
#define CONFIGURATION_HPP

// Namespace name configuration
#define MATH_NAMESPACE     math
#define PHYSICS_NAMESPACE  phy
#define GRAPHICS_NAMESPACE gfx

// Platform independent data types
using i8  = char;
using i16 = short;
using i32 = int;
using i64 = long long;
using u8  = unsigned char;
using u16 = unsigned short;
using u32 = unsigned int;
using u64 = unsigned long long;
using f32 = float;
using f64 = double;

// Default precision to use
using precision_t = f32;

// Include commonly used headers
#include <cassert>
#include <iosfwd>

#endif // CONFIGURATION_HPP
