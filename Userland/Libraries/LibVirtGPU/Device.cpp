/*
 * Copyright (c) 2022, Stephan Unverwerth <s.unverwerth@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <AK/NonnullOwnPtr.h>
#include <AK/Vector.h>
#include <Kernel/API/VirGL.h>
#include <LibVirtGPU/CommandBufferBuilder.h>
#include <LibVirtGPU/Device.h>
#include <LibVirtGPU/Image.h>
#include <LibVirtGPU/VirGLProtocol.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/ioctl_numbers.h>
#include <unistd.h>

namespace VirtGPU {

static constexpr auto frag_shader = "FRAG\n"
                                    "PROPERTY FS_COLOR0_WRITES_ALL_CBUFS 1\n"
                                    "DCL IN[0], COLOR, COLOR\n"
                                    "DCL OUT[0], COLOR\n"
                                    "  0: MOV OUT[0], IN[0]\n"
                                    "  1: END\n"sv;

static constexpr auto vert_shader = "VERT\n"
                                    "DCL IN[0]\n"
                                    "DCL IN[1]\n"
                                    "DCL OUT[0], POSITION\n"
                                    "DCL OUT[1], COLOR\n"
                                    "DCL CONST[0..3]\n"
                                    "DCL TEMP[0..1]\n"
                                    "  0: MUL TEMP[0], IN[0].xxxx, CONST[0]\n"
                                    "  1: MAD TEMP[1], IN[0].yyyy, CONST[1], TEMP[0]\n"
                                    "  2: MAD TEMP[0], IN[0].zzzz, CONST[2], TEMP[1]\n"
                                    "  3: MAD OUT[0], IN[0].wwww, CONST[3], TEMP[0]\n"
                                    "  4: MOV_SAT OUT[1], IN[1]\n"
                                    "  5: END\n"sv;

struct VertexData {
    float r;
    float g;
    float b;
    float x;
    float y;
    float z;
};

int gpu_fd;
ResourceID vbo_resource_id;
ResourceID drawtarget;
ResourceID depthbuffer_surface;
ObjectHandle blend_handle;
ObjectHandle drawtarget_surface_handle;
ObjectHandle depthbuffer_surface_handle;
ObjectHandle ve_handle;
ObjectHandle frag_shader_handle;
ObjectHandle vert_shader_handle;
ObjectHandle rasterizer_handle;
ObjectHandle dsa_handle;
Vector<VertexData> g_vertices;

static ObjectHandle allocate_handle()
{
    static u32 last_allocated_handle = 0;
    return { ++last_allocated_handle };
}

static void upload_command_buffer(int fd, Vector<u32> const& command_buffer)
{
    VERIFY(command_buffer.size() <= NumericLimits<u32>::max());
    VirGLCommandBuffer command_buffer_descriptor {
        .data = command_buffer.data(),
        .num_elems = (u32)command_buffer.size(),
    };
    VERIFY(ioctl(fd, VIRGL_IOCTL_SUBMIT_CMD, &command_buffer_descriptor) >= 0);
}

static ResourceID create_virgl_resource(int fd, VirGL3DResourceSpec& spec)
{
    VERIFY(ioctl(fd, VIRGL_IOCTL_CREATE_RESOURCE, &spec) >= 0);
    return spec.created_resource_id;
}

static Protocol::PipePrimitiveTypes map_primitive_type(GPU::PrimitiveType primitive_type) {
    switch (primitive_type) {
        case GPU::PrimitiveType::Lines:
        return Protocol::PipePrimitiveTypes::LINES;
        case GPU::PrimitiveType::LineLoop:
        return Protocol::PipePrimitiveTypes::LINE_LOOP;
        case GPU::PrimitiveType::LineStrip:
        return Protocol::PipePrimitiveTypes::LINE_STRIP;
        case GPU::PrimitiveType::Points:
        return Protocol::PipePrimitiveTypes::POINTS;
        case GPU::PrimitiveType::TriangleFan:
        return Protocol::PipePrimitiveTypes::TRIANGLE_FAN;
        case GPU::PrimitiveType::Triangles:
        return Protocol::PipePrimitiveTypes::TRIANGLES;
        case GPU::PrimitiveType::TriangleStrip:
        return Protocol::PipePrimitiveTypes::TRIANGLE_STRIP;
        case GPU::PrimitiveType::Quads:
        return Protocol::PipePrimitiveTypes::QUADS;
        default:
        VERIFY_NOT_REACHED();
    }
}

Device::Device(Gfx::IntSize size)
{
    dbgln("LibVirtGPU: Creating device with {}x{} framebuffer", size.width(), size.height());
    m_gpu_fd = open("/dev/gpu/render0", O_RDWR);
    VERIFY(m_gpu_fd >= 0);
    // Create a virgl context for this file descriptor
    VERIFY(ioctl(m_gpu_fd, VIRGL_IOCTL_CREATE_CONTEXT) >= 0);

    // Create a VertexElements resource
    VirGL3DResourceSpec vbo_spec {
        .target = to_underlying(Gallium::PipeTextureTarget::BUFFER), // pipe_texture_target
        .format = 45,                                                // pipe_to_virgl_format
        .bind = VIRGL_BIND_VERTEX_BUFFER,
        .width = PAGE_SIZE * 1024,
        .height = 1,
        .depth = 1,
        .array_size = 1,
        .last_level = 0,
        .nr_samples = 0,
        .flags = 0,
        .created_resource_id = 0,
    };
    vbo_resource_id = create_virgl_resource(m_gpu_fd, vbo_spec);
    // Create a texture to draw to
    VirGL3DResourceSpec drawtarget_spec {
        .target = to_underlying(Gallium::PipeTextureTarget::TEXTURE_RECT),                  // pipe_texture_target
        .format = to_underlying(Protocol::TextureFormat::VIRTIO_GPU_FORMAT_B8G8R8A8_UNORM), // pipe_to_virgl_format
        .bind = VIRGL_BIND_RENDER_TARGET,
        .width = static_cast<u32>(size.width()),
        .height = static_cast<u32>(size.height()),
        .depth = 1,
        .array_size = 1,
        .last_level = 0,
        .nr_samples = 0,
        .flags = 0,
        .created_resource_id = 0,
    };
    drawtarget = create_virgl_resource(m_gpu_fd, drawtarget_spec);
    // Create a depthbuffer surface
    VirGL3DResourceSpec depthbuffer_surface_spec {
        .target = to_underlying(Gallium::PipeTextureTarget::TEXTURE_RECT),             // pipe_texture_target
        .format = to_underlying(Protocol::TextureFormat::VIRTIO_GPU_FORMAT_Z32_FLOAT), // pipe_to_virgl_format
        .bind = VIRGL_BIND_RENDER_TARGET | VIRGL_BIND_DEPTH_STENCIL,
        .width = static_cast<u32>(size.width()),
        .height = static_cast<u32>(size.height()),
        .depth = 1,
        .array_size = 1,
        .last_level = 0,
        .nr_samples = 0,
        .flags = 0,
        .created_resource_id = 0,
    };
    depthbuffer_surface = create_virgl_resource(m_gpu_fd, depthbuffer_surface_spec);

    // Initialize all required state
    CommandBufferBuilder builder;
    // Create and set the blend, to control the color mask
    blend_handle = allocate_handle();
    builder.append_create_blend(blend_handle);
    builder.append_bind_blend(blend_handle);
    // Create drawtarget surface
    drawtarget_surface_handle = allocate_handle();
    builder.append_create_surface(drawtarget, drawtarget_surface_handle, Protocol::TextureFormat::VIRTIO_GPU_FORMAT_B8G8R8A8_UNORM);
    // Create depthbuffer surface
    depthbuffer_surface_handle = allocate_handle();
    builder.append_create_surface(depthbuffer_surface, depthbuffer_surface_handle, Protocol::TextureFormat::VIRTIO_GPU_FORMAT_Z32_FLOAT);
    // Set some framebuffer state (attached handle, framebuffer size, etc)
    builder.append_set_framebuffer_state(drawtarget_surface_handle, depthbuffer_surface_handle);
    builder.append_set_framebuffer_state_no_attach(size.width(), size.height());
    // Set the vertex buffer
    builder.append_set_vertex_buffers(sizeof(VertexData), 0, vbo_resource_id);
    // Create and bind fragment shader
    frag_shader_handle = allocate_handle();
    builder.append_create_shader(frag_shader_handle, Gallium::ShaderType::SHADER_FRAGMENT, frag_shader);
    builder.append_bind_shader(frag_shader_handle, Gallium::ShaderType::SHADER_FRAGMENT);
    // Create and bind vertex shader
    vert_shader_handle = allocate_handle();
    builder.append_create_shader(vert_shader_handle, Gallium::ShaderType::SHADER_VERTEX, vert_shader);
    builder.append_bind_shader(vert_shader_handle, Gallium::ShaderType::SHADER_VERTEX);
    // Create a VertexElements object (used to specify layout of vertex data)
    ve_handle = allocate_handle();
    builder.append_create_vertex_elements(ve_handle);
    builder.append_bind_vertex_elements(ve_handle);
    // Create a DepthStencilAlpha (DSA) object
    dsa_handle = allocate_handle();
    builder.append_create_dsa(dsa_handle);
    builder.append_bind_dsa(dsa_handle);
    // Create a Rasterizer object
    rasterizer_handle = allocate_handle();
    builder.append_create_rasterizer(rasterizer_handle);
    builder.append_bind_rasterizer(rasterizer_handle);
    // Set the Viewport
    builder.append_gl_viewport(size.width(), size.height());
    // Upload buffer
    upload_command_buffer(m_gpu_fd, builder.build());
}

Device::~Device()
{
    close(m_gpu_fd);
}

GPU::DeviceInfo Device::info() const
{
    return {
        .vendor_name = "SerenityOS",
        .device_name = "VirtIO-GPU",
        .num_texture_units = GPU::NUM_TEXTURE_UNITS,
        .num_lights = 32,
        .max_clip_planes = 32,
        .max_texture_size = 4096,
        .max_texture_lod_bias = 4,
        .stencil_bits = 8,
        .supports_npot_textures = true,
        .supports_texture_clamp_to_edge = true,
        .supports_texture_env_add = true,
    };
}

static Vector<float> encode_constant_buffer(Gfx::FloatMatrix4x4 const& mat)
{
    // Flip the y axis. This is done because OpenGLs coordinate space has a Y-axis of
    // Opposite direction to that of LibGfx
    Gfx::FloatMatrix4x4 flip_y = Gfx::FloatMatrix4x4::identity();
    flip_y.elements()[1][1] = -1;
    auto real_mat = mat * flip_y;
    Vector<float> values;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            values.append(real_mat.elements()[i][j]);
        }
    }
    return values;
}

void Device::draw_primitives(GPU::PrimitiveType primitive_type, FloatMatrix4x4 const& modelview_matrix, FloatMatrix4x4 const& projection_matrix, Vector<GPU::Vertex>& vertices)
{
    if (vertices.size() == 0)
        return;

    g_vertices.clear_with_capacity();
    for(auto& vertex: vertices) {
        g_vertices.append({
            vertex.tex_coords[0].x(),
            vertex.tex_coords[0].y(),
            vertex.tex_coords[0].z(),
            vertex.position.x(),
            vertex.position.y(),
            vertex.position.z(),
        });
    }
    // Get model matrix
    auto model_matrix = (projection_matrix * modelview_matrix).transpose();
    VirGLTransferDescriptor descriptor {
        .data = (void*)g_vertices.data(),
        .offset_in_region = 0,
        .num_bytes = sizeof(VertexData) * g_vertices.size(),
        .direction = VIRGL_DATA_DIR_GUEST_TO_HOST,
    };
    // Transfer data from vertices array to kernel virgl transfer region
    VERIFY(ioctl(m_gpu_fd, VIRGL_IOCTL_TRANSFER_DATA, &descriptor) >= 0);
    // Create command buffer
    CommandBufferBuilder builder;
    // Transfer data from kernel virgl transfer region to host resource
    builder.append_transfer3d(vbo_resource_id, sizeof(VertexData) * g_vertices.size(), 1, 1, VIRGL_DATA_DIR_GUEST_TO_HOST);
    builder.append_end_transfers_3d();
    // Set the constant buffer to the identity matrix
    builder.append_set_constant_buffer(encode_constant_buffer(model_matrix));
    // Draw the vbo
    builder.append_draw_vbo(g_vertices.size(), map_primitive_type(primitive_type));
    // Upload the buffer
    upload_command_buffer(m_gpu_fd, builder.build());
}

void Device::resize(Gfx::IntSize)
{    
}

void Device::clear_color(FloatVector4 const& color)
{    
    // Create command buffer
    CommandBufferBuilder builder;
    // Clear the framebuffer
    builder.append_gl_clear(color.x(), color.y(), color.z(), color.w());
    // Upload the buffer
    upload_command_buffer(m_gpu_fd, builder.build());
}

void Device::clear_depth(GPU::DepthType depth)
{    
    // Create command buffer
    CommandBufferBuilder builder;
    // Clear the framebuffer
    builder.append_gl_clear(depth);
    // Upload the buffer
    upload_command_buffer(m_gpu_fd, builder.build());
}

void Device::clear_stencil(GPU::StencilType)
{    
}

void Device::blit_from_color_buffer(Gfx::Bitmap& front_buffer)
{    
    // Transfer data back from hypervisor to kernel transfer region
    CommandBufferBuilder builder;
    builder.append_transfer3d(drawtarget, front_buffer.size().width(), front_buffer.size().height(), 1, VIRGL_DATA_DIR_HOST_TO_GUEST);
    builder.append_end_transfers_3d();
    upload_command_buffer(m_gpu_fd, builder.build());
    // Copy from kernel transfer region to userspace
    VirGLTransferDescriptor descriptor {
        .data = (void*)front_buffer.scanline_u8(0),
        .offset_in_region = 0,
        .num_bytes = front_buffer.size().width() * front_buffer.size().height() * sizeof(u32),
        .direction = VIRGL_DATA_DIR_HOST_TO_GUEST,
    };
    VERIFY(ioctl(m_gpu_fd, VIRGL_IOCTL_TRANSFER_DATA, &descriptor) >= 0);
}

void Device::blit_from_color_buffer(NonnullRefPtr<GPU::Image>, u32, Vector2<u32>, Vector2<i32>, Vector3<i32>)
{    
}

void Device::blit_from_color_buffer(void*, Vector2<i32>, GPU::ImageDataLayout const&)
{    
}

void Device::blit_from_depth_buffer(void*, Vector2<i32>, GPU::ImageDataLayout const&)
{    
}

void Device::blit_from_depth_buffer(NonnullRefPtr<GPU::Image>, u32, Vector2<u32>, Vector2<i32>, Vector3<i32>)
{    
}

void Device::blit_to_color_buffer_at_raster_position(void const*, GPU::ImageDataLayout const&)
{    
}

void Device::blit_to_depth_buffer_at_raster_position(void const*, GPU::ImageDataLayout const&)
{    
}

void Device::set_options(GPU::RasterizerOptions const&)
{    
}

void Device::set_light_model_params(GPU::LightModelParameters const&)
{    
}

GPU::RasterizerOptions Device::options() const
{
    return {};
}

GPU::LightModelParameters Device::light_model() const
{    
    return {};
}


NonnullRefPtr<GPU::Image> Device::create_image(GPU::PixelFormat const& pixel_format, u32 width, u32 height, u32 depth, u32 max_levels)
{
    return adopt_ref(*new Image(this, pixel_format, width, height, depth, max_levels));
}


void Device::set_sampler_config(unsigned, GPU::SamplerConfig const&)
{    
}

void Device::set_light_state(unsigned, GPU::Light const&)
{    
}

void Device::set_material_state(GPU::Face, GPU::Material const&)
{    
}

void Device::set_stencil_configuration(GPU::Face, GPU::StencilConfiguration const&)
{    
}

void Device::set_texture_unit_configuration(GPU::TextureUnitIndex, GPU::TextureUnitConfiguration const&)
{    
}

void Device::set_clip_planes(Vector<FloatVector4> const&)
{    
}


GPU::RasterPosition Device::raster_position() const
{
    return {};
}

void Device::set_raster_position(GPU::RasterPosition const&)
{
}

void Device::set_raster_position(FloatVector4 const&, FloatMatrix4x4 const&, FloatMatrix4x4 const&)
{    
}

}

extern "C" {

GPU::Device* serenity_gpu_create_device(Gfx::IntSize size)
{
    return make<VirtGPU::Device>(size).leak_ptr();
}

}
