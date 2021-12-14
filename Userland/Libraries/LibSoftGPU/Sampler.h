/*
 * Copyright (c) 2021, Stephan Unverwerth <s.unverwerth@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/RefPtr.h>
#include <AK/SIMD.h>
#include <LibGfx/Vector2.h>
#include <LibGfx/Vector4.h>
#include <LibSoftGPU/Texture2D.h>

namespace SoftGPU {

class Texture2D;

using AK::SIMD::f32x4;

enum class TextureFilter {
    Nearest,
    Linear,
};

class Sampler final {
public:
    Vector4<f32x4> sample_2d(Vector2<f32x4> const& uv) const;
    Vector4<f32x4> sample_2d_lod(Vector2<f32x4> const& uv, f32x4 lod) const;
    Vector4<f32x4> sample_2d_grad(Vector2<f32x4> const& uv, Vector2<f32x4> const& duvdx, Vector2<f32x4> const& duvdy) const;

    RefPtr<Texture2D> bound_texture_2d() const { return m_bound_texture_2d; }
    void set_bound_texture_2d(RefPtr<Texture2D> texture) { m_bound_texture_2d = texture; }

private:
    RefPtr<Texture2D> m_bound_texture_2d;
    TextureFilter m_mipmap_filter { TextureFilter::Nearest };
    TextureFilter m_texture_filter { TextureFilter::Linear };
};

}