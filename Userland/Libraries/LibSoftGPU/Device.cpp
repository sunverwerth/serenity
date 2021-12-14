/*
 * Copyright (c) 2021, Stephan Unverwerth <s.unverwerth@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <AK/StringView.h>
#include <LibHAL/GPU/DeviceInfo.h>
#include <LibHAL/GPU/Texture2D.h>
#include <LibSoftGPU/Buffer.h>
#include <LibSoftGPU/Device.h>
#include <LibSoftGPU/ShaderVM.h>

namespace SoftGPU {

HAL::GPU::DeviceInfo Device::info() const
{
    return {
        .vendor = "SerenityOS",
        .model = "SoftGPU",
        .is_accelerated = false,
        .num_texture_units = shader_sampler_count
    };
}

// Resource creation
ErrorOr<NonnullRefPtr<HAL::GPU::Buffer>> Device::try_create_buffer(HAL::GPU::BufferUsageHint, size_t size)
{
    auto maybe_byte_buffer = ByteBuffer::create_zeroed(size);
    if (!maybe_byte_buffer.has_value())
        return Error::from_string_literal("SoftGPU: Could not create buffer storage");

    return adopt_ref(*new Buffer(move(maybe_byte_buffer.value())));
}

ErrorOr<NonnullRefPtr<HAL::GPU::Texture2D>> Device::try_create_texture_2d(HAL::GPU::PixelFormat, size_t width, size_t height, size_t num_levels)
{
    (void)width;
    (void)height;
    (void)num_levels;
    return Error::from_string_literal("not implemented");
}

ErrorOr<NonnullRefPtr<HAL::GPU::Shader>> Device::try_create_shader(HAL::GPU::ShaderStage, Vector<HAL::GPU::Opcode> const&)
{
    return Error::from_string_literal("not implemented");
}

// Render stage setup
void Device::configure_input_assembly_stage(HAL::GPU::InputAssemblyStageConfig const&)
{
}

void Device::configure_vertex_stage(HAL::GPU::VertexStageConfig const&)
{
}

void Device::configure_rasterizer_stage(HAL::GPU::RasterizerStageConfig const&)
{
}

void Device::configure_fragment_stage(HAL::GPU::FragmentStageConfig const&)
{
}

void Device::configure_blend_stage(HAL::GPU::BlendStageConfig const&)
{
}

// Draw commands
void Device::draw(HAL::GPU::PrimitiveType, size_t first_vertex, size_t num_vertices, size_t first_instance, size_t num_instances)
{
    for (size_t instance = first_instance; instance < first_instance + num_instances; instance++) {

    }
    
    (void)first_vertex;
    (void)num_vertices;
    (void)first_instance;
    (void)num_instances;
}

void Device::draw_indexed(HAL::GPU::PrimitiveType, HAL::GPU::IndexType, size_t first_index, size_t num_indices, size_t index_add, size_t first_instance, size_t num_instances)
{
    (void)first_index;
    (void)num_indices;
    (void)index_add;
    (void)first_instance;
    (void)num_instances;
}

RefPtr<Device> create_device()
{
    return new Device();
}

}