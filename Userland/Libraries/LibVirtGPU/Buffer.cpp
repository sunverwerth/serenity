/*
 * Copyright (c) 2022, Stephan Unverwerth <s.unverwerth@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <LibVirtGPU/Buffer.h>
#include <LibVirtGPU/CommandBufferBuilder.h>
#include <LibVirtGPU/Device.h>

namespace VirtGPU {

Buffer::Buffer(Device& device, Protocol::ResourceID resource_id, size_t size)
    : GPU::Buffer(&device, size)
    , m_device { device }
    , m_resource_id { resource_id }
{
}

void Buffer::upload_data(size_t offset, size_t size, void const* data)
{
    MUST(m_device.write_to_kernel_buffer(offset, size, data));

    CommandBufferBuilder builder;
    builder.append_transfer3d(m_resource_id, offset, 0, 0, size, 1, 1, VIRGL_DATA_DIR_GUEST_TO_HOST);
    MUST(m_device.upload_command_buffer(builder.build()));
}

}
