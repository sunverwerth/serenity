/*
 * Copyright (c) 2021, Stephan Unverwerth <s.unverwerth@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/Error.h>
#include <AK/NonnullRefPtr.h>
#include <AK/RefCounted.h>
#include <AK/Vector.h>

#include <LibHAL/GPU/Buffer.h>
#include <LibHAL/GPU/Shader.h>
#include <LibHAL/GPU/Texture2D.h>
#include <LibHAL/GPU/VMInstruction.h>

namespace HAL::GPU {

enum class PrimitiveType {
    TriangleList,
    TriangleStrip,
    TriangleFan,
};

enum class IndexType {
    U8,
    U16,
    U32,
};

struct DeviceInfo;
struct InputAssemblyStageConfig;
struct VertexStageConfig;
struct RasterizerStageConfig;
struct FragmentStageConfig;
struct BlendStageConfig;

class Device : public RefCounted<Device> {
public:
    virtual ~Device() { }

    // Device info
    virtual DeviceInfo info() const = 0;

    // Resource creation
    virtual ErrorOr<NonnullRefPtr<Buffer>> try_create_buffer(BufferUsageHint, size_t) = 0;
    virtual ErrorOr<NonnullRefPtr<Texture2D>> try_create_texture_2d(PixelFormat, size_t width, size_t height, size_t num_levels) = 0;
    virtual ErrorOr<NonnullRefPtr<Shader>> try_create_shader(ShaderStage, Vector<Opcode> const&) = 0;

    // Render stage setup
    virtual void configure_input_assembly_stage(InputAssemblyStageConfig const&) = 0;
    virtual void configure_vertex_stage(VertexStageConfig const&) = 0;
    virtual void configure_rasterizer_stage(RasterizerStageConfig const&) = 0;
    virtual void configure_fragment_stage(FragmentStageConfig const&) = 0;
    virtual void configure_blend_stage(BlendStageConfig const&) = 0;

    // Draw commands
    virtual void draw(PrimitiveType, size_t first_vertex, size_t num_vertices, size_t first_instance, size_t num_instances) = 0;
    virtual void draw_indexed(PrimitiveType, IndexType, size_t first_index, size_t num_indices, size_t index_add, size_t first_instance, size_t num_instances) = 0;
};

}