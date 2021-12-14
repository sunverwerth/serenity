/*
 * Copyright (c) 2021, Stephan Unverwerth <s.unverwerth@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/RefPtr.h>
#include <LibHAL/GPU/Device.h>

namespace SoftGPU {

class Device final : public HAL::GPU::Device {
public:
    virtual HAL::GPU::DeviceInfo info() const override;

    // Resource creation
    virtual ErrorOr<NonnullRefPtr<HAL::GPU::Buffer>> try_create_buffer(HAL::GPU::BufferUsageHint, size_t) override;
    virtual ErrorOr<NonnullRefPtr<HAL::GPU::Texture2D>> try_create_texture_2d(HAL::GPU::PixelFormat, size_t width, size_t height, size_t num_levels) override;
    virtual ErrorOr<NonnullRefPtr<HAL::GPU::Shader>> try_create_shader(HAL::GPU::ShaderStage, Vector<HAL::GPU::Opcode> const&) override;

    // Render stage setup
    virtual void configure_input_assembly_stage(HAL::GPU::InputAssemblyStageConfig const&) override;
    virtual void configure_vertex_stage(HAL::GPU::VertexStageConfig const&) override;
    virtual void configure_rasterizer_stage(HAL::GPU::RasterizerStageConfig const&) override;
    virtual void configure_fragment_stage(HAL::GPU::FragmentStageConfig const&) override;
    virtual void configure_blend_stage(HAL::GPU::BlendStageConfig const&) override;

    // Draw commands
    virtual void draw(HAL::GPU::PrimitiveType, size_t first_vertex, size_t num_vertices, size_t first_instance, size_t num_instances) override;
    virtual void draw_indexed(HAL::GPU::PrimitiveType, HAL::GPU::IndexType, size_t first_index, size_t num_indices, size_t index_add, size_t first_instance, size_t num_instances) override;
};

RefPtr<Device> create_device();

}