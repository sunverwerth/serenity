/*
 * Copyright (c) 2021, Stephan Unverwerth <s.unverwerth@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <LibSoftGPU/Sampler.h>
#include <LibSoftGPU/math.h>

namespace SoftGPU {

static constexpr const Vector4<f32x4> black { f32x4 { 0 }, f32x4 { 0 }, f32x4 { 0 }, f32x4 { 1 } };
static constexpr const Vector4<f32x4> white { f32x4 { 1 }, f32x4 { 1 }, f32x4 { 1 }, f32x4 { 1 } };

Vector4<f32x4> Sampler::sample_2d(Vector2<f32x4> const& uv) const
{
    if (m_bound_texture_2d.is_null())
        return black;

    return sample_2d_grad(uv, ddx(uv), ddy(uv));
}

Vector4<f32x4> Sampler::sample_2d_lod(Vector2<f32x4> const& uv, f32x4 lod) const
{
    if (m_bound_texture_2d.is_null())
        return black;

    (void)uv;
    (void)lod;
    return white;
}

Vector4<f32x4> Sampler::sample_2d_grad(Vector2<f32x4> const& uv, Vector2<f32x4> const& duvdx, Vector2<f32x4> const& duvdy) const
{
    if (m_bound_texture_2d.is_null())
        return black;

    (void)uv;
    (void)duvdx;
    (void)duvdy;
    return white;
}

}