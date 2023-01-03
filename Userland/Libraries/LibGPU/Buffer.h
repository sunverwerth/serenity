/*
 * Copyright (c) 2022, cflip <cflip@cflip.net>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/RefCounted.h>

namespace GPU {

class Buffer : public RefCounted<Buffer> {
public:
    Buffer(void const* ownership_token, size_t size)
        : m_ownership_token { ownership_token }
        , m_size { size }
    {
    }

    virtual ~Buffer() { }

    virtual void upload_data(size_t offset, size_t size, void const*) = 0;

    size_t size() const { return m_size; }

private:
    void const* m_ownership_token { nullptr };
    size_t m_size { 0 };
};

}
