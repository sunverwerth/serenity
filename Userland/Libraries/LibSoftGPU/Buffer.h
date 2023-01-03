/*
 * Copyright (c) 2022, cflip <cflip@cflip.net>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/FixedArray.h>
#include <LibGPU/Buffer.h>

namespace SoftGPU {

class Buffer final : public GPU::Buffer {
public:
    Buffer(void const* ownership_token, FixedArray<u8> data)
        : GPU::Buffer(ownership_token, data.size())
        , m_data { move(data) }
    {
    }

    virtual void upload_data(size_t offset, size_t size, void const*) override;

private:
    FixedArray<u8> m_data;
};

}
