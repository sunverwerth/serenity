/*
 * Copyright (c) 2021, Stephan Unverwerth <s.unverwerth@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/RefCounted.h>

namespace HAL::GPU {

enum class BufferUsageHint {
    Vertices,
    Indices,
};

class Buffer : public RefCounted<Buffer> {
public:
    virtual ~Buffer() { }

    virtual void update_range(size_t offset, void const* data, size_t size) = 0;
};

}