/*
 * Copyright (c) 2021, Stephan Unverwerth <s.unverwerth@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <AK/Types.h>
#include <AK/RefCounted.h>
#include <AK/ByteBuffer.h>

#pragma once

namespace SoftGPU
{
class SoftGPU;

class Buffer final : public RefCounted<Buffer> {
public:
    Buffer(ByteBuffer buffer)
        : m_data{std::move(buffer)}
    {
    }

    void upload_data(size_t offset, void const* data, size_t size);

private:
    ByteBuffer m_data;
};

}