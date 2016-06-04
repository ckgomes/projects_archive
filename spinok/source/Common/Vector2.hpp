#ifndef VECTOR2_HPP
#define VECTOR2_HPP

#include "Configuration.hpp"
#include "Math.hpp"

#include <iosfwd>

namespace mm
{

template <typename T = precision_t>
struct v2_t
{
    T x;
    T y;

    T lengthSq() const;
    T length() const;
    T angle() const;

    v2_t<T> normalize() const;

    v2_t<T> rotate(T radians) const;
    v2_t<T> rotateCW90() const;
    v2_t<T> rotateCCW90() const;

    T& operator[](u32 index);
    const T& operator[](u32 index) const;

    static_assert(std::is_same<T, f32>::value ||
                  std::is_same<T, f64>::value ||
                  std::is_same<T, s32>::value ||
                  std::is_same<T, s64>::value,
                  "Invalid template type for [v2_t]");

};

/// Operators implementation

template <typename T = precision_t>
static inline v2_t<T> operator+(v2_t<T> vec)
{
    return {+vec.x, +vec.y};
}

template <typename T = precision_t>
static inline v2_t<T> operator+(v2_t<T> left, v2_t<T> right)
{
    return {left.x + right.x, left.y + right.y};
}

template <typename T = precision_t>
static inline void operator+=(v2_t<T>& left, v2_t<T> right)
{
    left.x += right.x;
    left.y += right.y;
}

template <typename T = precision_t>
static inline v2_t<T> operator-(v2_t<T> vec)
{
    return {-vec.x, -vec.y};
}

template <typename T = precision_t>
static inline v2_t<T> operator-(v2_t<T> left, v2_t<T> right)
{
    return {left.x - right.x, left.y - right.y};
}

template <typename T = precision_t>
static inline void operator-=(v2_t<T>& left, v2_t<T> right)
{
    left.x -= right.x;
    left.y -= right.y;
}

template <typename T = precision_t>
static inline v2_t<T> operator*(v2_t<T> left, v2_t<T> right)
{
    return {left.x * right.x, left.y * right.y};
}

template <typename T = precision_t>
static inline void operator*=(v2_t<T>& left, v2_t<T> right)
{
    left.x *= right.x;
    left.y *= right.y;
}

template <typename T = precision_t>
static inline v2_t<T> operator*(v2_t<T> left, T right)
{
    return {left.x * right, left.y * right};
}

template <typename T = precision_t>
static inline v2_t<T> operator*(T left, v2_t<T> right)
{
    return {left * right.x, left * right.y};
}

template <typename T = precision_t>
static inline void operator*=(v2_t<T>& left, T right)
{
    left.x *= right;
    left.y *= right;
}

template <typename T = precision_t>
static inline v2_t<T> operator/(v2_t<T> left, v2_t<T> right)
{
    return {left.x / right.x, left.y / right.y};
}

template <typename T = precision_t>
static inline void operator/=(v2_t<T>& left, v2_t<T> right)
{
    left.x /= right.x;
    left.y /= right.y;
}

template <typename T = precision_t>
static inline v2_t<T> operator/(v2_t<T> left, T right)
{
    return {left.x / right, left.y / right};
}

template <typename T = precision_t>
static inline void operator/=(v2_t<T>& left, T right)
{
    left.x /= right;
    left.y /= right;
}

/// Indexing operators

template <typename T>
inline T& v2_t<T>::operator[](u32 index)
{
    assert(index < 2);
    return (static_cast<T*>(this) + index);
}

template <typename T>
inline const T& v2_t<T>::operator[](u32 index) const
{
    assert(index < 2);
    return (static_cast<T*>(this) + index);
}

/// Comparison operators

template <typename T = precision_t>
static inline bool operator==(v2_t<T> left, v2_t<T> right)
{
    return (left.x == right.x) && (left.y == right.y);
}

template <typename T = precision_t>
static inline bool operator!=(v2_t<T> left, v2_t<T> right)
{
    return (left.x != right.x) || (left.y != right.y);
}

/// Stream operators

template <typename T = precision_t>
static inline std::ostream& operator<<(std::ostream& stream, v2_t<T> vec)
{
    stream << '(' << vec.x << ',' << vec.y << ')';
    return stream;
}

/// Member functions implementation

template<typename T = precision_t>
inline T v2_t<T>::lengthSq() const
{
    return (x * x) + (y * y);
}

template <typename T = precision_t>
inline T v2_t<T>::length() const
{
    return mm::sqrt((x * x) + (y * y));
}

template <typename T = precision_t>
inline T v2_t<T>::angle() const
{
    return mm::atan2(y, x);
}

template<typename T = precision_t>
inline v2_t<T> v2_t<T>::normalize() const
{
    return (*this) / length();
}

template <typename T = precision_t>
inline v2_t<T> v2_t<T>::rotate(T radians) const
{
    auto sin = mm::sin(radians);
    auto cos = mm::cos(radians);
    return {x * cos - y * sin, x * sin + y * cos};
}

template <typename T = precision_t>
inline v2_t<T> v2_t<T>::rotateCW90() const
{
    return {y, -x};
}

template <typename T = precision_t>
inline v2_t<T> v2_t<T>::rotateCCW90() const
{
    return {-y, x};
}

/// Utility functions implementation

template <typename T = precision_t>
static inline T dot(v2_t<T> right, v2_t<T> left)
{
    return (right.x * left.x) + (right.y * left.y);
}

template <typename T = precision_t>
static inline T distance(v2_t<T> right, v2_t<T> left)
{
    return (right - left).length();
}

template <typename T = precision_t>
static inline v2_t<T> reflect(v2_t<T> vec, v2_t<T> normal)
{
    return T(-2) * dot(vec, normal) * normal + vec;
}

template <typename T = precision_t>
static inline v2_t<T> clamp(v2_t<T> min, v2_t<T> max, v2_t<T> value)
{
    return {clamp(min.x, max.x, value.x), clamp(min.y, max.y, value.y)};
}

}

// Export default precision type as [vec2]
using vec2 = mm::v2_t<precision_t>;

#endif // VECTOR2_HPP

