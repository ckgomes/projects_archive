#ifndef MATRIX2_HPP
#define MATRIX2_HPP

#include "Math.hpp"
#include "Vector2.hpp"
#include "Configuration.hpp"

namespace MATH_NAMESPACE
{

template <typename T = precision_t>
struct m2_t
{

    v2_t<T> r0;
    v2_t<T> r1;

    T determinant() const;
    m2_t transpose() const;

    v2_t<T>& operator[](u32 index);
    const v2_t<T>& operator[](u32 index) const;

};

/// Build matrices

template <typename T = precision_t>
constexpr static inline m2_t<T> identity()
{
    return {{T(1), T(0)}, {T(0), T(1)}};
}

template <typename T = precision_t>
constexpr static inline m2_t<T> scale(T factor)
{
    return {{T(factor), T(0)}, {T(0), T(factor)}};
}

template <typename T = precision_t>
static inline m2_t<T> rotation(T radians)
{
    T cos = math::cos(radians);
    T sin = math::sin(radians);
    return {{cos, -sin}, {sin, cos}};
}

/// Mathematical operators

template <typename T = precision_t>
static inline m2_t<T> operator+(const m2_t<T>& left, const m2_t<T>& right)
{
    return {left[0] + right[0], left[1] + right[1]};
}

template <typename T = precision_t>
static inline m2_t<T> operator+=(m2_t<T>& left, const m2_t<T>& right)
{
    left [0] += right [0];
    left [1] += right [1];
}

template <typename T = precision_t>
static inline m2_t<T> operator-(const m2_t<T>& left, const m2_t<T>& right)
{
    return {left[0] - right[0], left[1] - right[1]};
}

template <typename T = precision_t>
static inline m2_t<T> operator-=(m2_t<T>& left, const m2_t<T>& right)
{
    left[0] -= right[0];
    left[1] -= right[1];
}

template <typename T = precision_t>
static inline m2_t<T> operator*(const m2_t<T>& left, T right)
{
    return {left[0] * right, left[1] * right};
}

template <typename T = precision_t>
static inline m2_t<T> operator*(T left, const m2_t<T>& right)
{
    return {left * right[0], left * right[1]};
}

template <typename T = precision_t>
static inline v2_t<T> operator*(const m2_t<T>& left, v2_t<T> right)
{
    return
    {
        left[0][0] * right[0] + left[0][1] * right[1],
        left[1][0] * right[0] + left[1][1] * right[1]
    };
}

template <typename T = precision_t>
static inline m2_t<T> operator*(const m2_t<T>& left, const m2_t<T>& right)
{
    return
    {
        {
            left[0][0] * right[0][0] + left[0][1] * right[1][0],
            left[0][0] * right[0][1] + left[0][1] * right[1][1]
        },
        {
            left[1][0] * right[0][0] + left[1][1] * right[1][0],
            left[1][0] * right[0][1] + left[1][1] * right[1][1]
        }
    }
}

/// Indexing operators

template <typename T = precision_t>
inline v2_t<T>& m2_t<T>::operator[](u32 index)
{
    assert(index < 2);
    return (static_cast<v2_t<T>*>(this) + index);
}

template <typename T = precision_t>
inline const v2_t<T>& m2_t<T>::operator[](u32 index) const
{
    assert(index < 2);
    return (static_cast<v2_t<T>*>(this) + index);
}

/// Member methods

template <typename T = precision_t>
inline T m2_t<T>::determinant() const
{
    return (r0.x * r1.y) - (r0.y * r1.x);
}

template<typename T>
inline m2_t<T> m2_t<T>::transpose() const
{
    return {{r0[0], r1[0]}, {r0[1], r1[1]}};
}

}

// Export default precision type as [mat2]
using mat2 = MATH_NAMESPACE::m2_t<precision_t>;

#endif // MATRIX2_HPP
