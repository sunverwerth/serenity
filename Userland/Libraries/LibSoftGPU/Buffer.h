/*
 * Copyright (c) 2021, Stephan Unverwerth <s.unverwerth@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/ByteBuffer.h>
#include <AK/Format.h>
#include <LibHAL/GPU/Buffer.h>

namespace SoftGPU {

class Buffer final : public HAL::GPU::Buffer {
public:
    Buffer(ByteBuffer&& byte_buffer)
        : m_data(move(byte_buffer))
    {
        dbgln("Creating GPU buffer with {} bytes", m_data.size());
    }

    virtual ~Buffer() override
    {
        dbgln("Deleting GPU buffer with {} bytes", m_data.size());
    }

    virtual void update_range(size_t offset, void const* data, size_t size) override;

private:
    ByteBuffer m_data;
};

}