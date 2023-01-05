/*
 * Copyright (c) 2022, Stephan Unverwerth <s.unverwerth@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/Error.h>
#include <AK/NonnullRefPtr.h>
#include <LibGPU/Buffer.h>
#include <LibVirtGPU/VirGLProtocol.h>

namespace VirtGPU {

class Device;

class Buffer final : public GPU::Buffer {
public:
    Buffer(Device&, Protocol::ResourceID, size_t);

    virtual void upload_data(size_t offset, size_t size, void const* data) override;

private:
    Device& m_device;
    Protocol::ResourceID m_resource_id { 0 };
};

}
