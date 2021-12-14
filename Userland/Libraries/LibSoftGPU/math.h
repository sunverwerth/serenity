/*
 * Copyright (c) 2021, Stephan Unverwerth <s.unverwerth@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/SIMD.h>
#include <LibGfx/Vector2.h>
#include <LibGfx/Vector3.h>
#include <LibGfx/Vector4.h>

namespace SoftGPU {

using AK::SIMD::f32x4;

// Calculates the partial derivative of the vector, representing a quad, with respect to x
// | 0 1 | -> | 1-0 1-0 |
// | 2 3 | -> | 3-2 3-2 |
inline static constexpr f32x4 ddx(f32x4 v)
{
    return f32x4 {
        v[1] - v[0], v[1] - v[0],
        v[3] - v[2], v[3] - v[2]
    };
}

// Calculates the partial derivative of the vector, representing a quad, with respect to y
// | 0 1 | -> | 2-0 3-1 |
// | 2 3 | -> | 2-0 3-1 |
inline static constexpr f32x4 ddy(f32x4 v)
{
    return f32x4 {
        v[2] - v[0], v[3] - v[1],
        v[2] - v[0], v[3] - v[1]
    };
}

inline static constexpr Vector2<f32x4> ddx(Vector2<f32x4> const& v)
{
    return {
        ddx(v.x()),
        ddx(v.y())
    };
}

inline static constexpr Vector2<f32x4> ddy(Vector2<f32x4> const& v)
{
    return {
        ddy(v.x()),
        ddy(v.y())
    };
}

inline static constexpr Vector3<f32x4> ddx(Vector3<f32x4> const& v)
{
    return {
        ddx(v.x()),
        ddx(v.y()),
        ddx(v.z())
    };
}

inline static constexpr Vector3<f32x4> ddy(Vector3<f32x4> const& v)
{
    return {
        ddy(v.x()),
        ddy(v.y()),
        ddy(v.z())
    };
}

inline static constexpr Vector4<f32x4> ddx(Vector4<f32x4> const& v)
{
    return {
        ddx(v.x()),
        ddx(v.y()),
        ddx(v.z()),
        ddx(v.w())
    };
}

inline static constexpr Vector4<f32x4> ddy(Vector4<f32x4> const& v)
{
    return {
        ddy(v.x()),
        ddy(v.y()),
        ddy(v.z()),
        ddy(v.w())
    };
}

template<typename T, typename U>
inline static constexpr T lerp(T const& a, T const& b, U t)
{
    U const one_minus_t = U { 1 } - t;
    return a * one_minus_t + b * t;
}

template<typename T, typename U>
inline static constexpr T bilerp(T const& a, T const& b, T const& c, T const& d, U x, U y)
{
    U const one_minus_x = U { 1 } - x;
    U const one_minus_y = U { 1 } - y;
    return lerp(lerp(a, b, x), lerp(c, d, x), y);
}

}