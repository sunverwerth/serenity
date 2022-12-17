/*
 * Copyright (c) 2022, Stephan Unverwerth <s.unverwerth@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <LibGPU/Device.h>

namespace VirtGPU {

class Device final : public GPU::Device {
public:
    Device(Gfx::IntSize);
    ~Device();

    virtual GPU::DeviceInfo info() const override;

    virtual void draw_primitives(GPU::PrimitiveType, FloatMatrix4x4 const& model_view_transform, FloatMatrix4x4 const& projection_transform, Vector<GPU::Vertex>& vertices) override;
    virtual void resize(Gfx::IntSize min_size) override;
    virtual void clear_color(FloatVector4 const&) override;
    virtual void clear_depth(GPU::DepthType) override;
    virtual void clear_stencil(GPU::StencilType) override;
    virtual void blit_from_color_buffer(Gfx::Bitmap& target) override;
    virtual void blit_from_color_buffer(NonnullRefPtr<GPU::Image>, u32 level, Vector2<u32> input_size, Vector2<i32> input_offset, Vector3<i32> output_offset) override;
    virtual void blit_from_color_buffer(void*, Vector2<i32> offset, GPU::ImageDataLayout const&) override;
    virtual void blit_from_depth_buffer(void*, Vector2<i32> offset, GPU::ImageDataLayout const&) override;
    virtual void blit_from_depth_buffer(NonnullRefPtr<GPU::Image>, u32 level, Vector2<u32> input_size, Vector2<i32> input_offset, Vector3<i32> output_offset) override;
    virtual void blit_to_color_buffer_at_raster_position(void const*, GPU::ImageDataLayout const&) override;
    virtual void blit_to_depth_buffer_at_raster_position(void const*, GPU::ImageDataLayout const&) override;
    virtual void set_options(GPU::RasterizerOptions const&) override;
    virtual void set_light_model_params(GPU::LightModelParameters const&) override;
    virtual GPU::RasterizerOptions options() const override;
    virtual GPU::LightModelParameters light_model() const override;

    virtual NonnullRefPtr<GPU::Image> create_image(GPU::PixelFormat const&, u32 width, u32 height, u32 depth, u32 max_levels) override;

    virtual void set_sampler_config(unsigned, GPU::SamplerConfig const&) override;
    virtual void set_light_state(unsigned, GPU::Light const&) override;
    virtual void set_material_state(GPU::Face, GPU::Material const&) override;
    virtual void set_stencil_configuration(GPU::Face, GPU::StencilConfiguration const&) override;
    virtual void set_texture_unit_configuration(GPU::TextureUnitIndex, GPU::TextureUnitConfiguration const&) override;
    virtual void set_clip_planes(Vector<FloatVector4> const&) override;

    virtual GPU::RasterPosition raster_position() const override;
    virtual void set_raster_position(GPU::RasterPosition const& raster_position) override;
    virtual void set_raster_position(FloatVector4 const& position, FloatMatrix4x4 const& model_view_transform, FloatMatrix4x4 const& projection_transform) override;

private:
    int m_gpu_fd{0};
};

}

typedef GPU::Device* (*serenity_gpu_create_device_t)(Gfx::IntSize size);

extern "C" GPU::Device* serenity_gpu_create_device(Gfx::IntSize size);
