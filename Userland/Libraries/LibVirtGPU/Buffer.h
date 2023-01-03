/*
 * Copyright (c) 2022, Stephan Unverwerth <s.unverwerth@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/Error.h>
#include <LibGPU/Buffer.h>

namespace VirtGPU {

class Buffer final : public GPU::Buffer {
public:
    Buffer(void const* ownership_token);

    virtual void upload_data(size_t, size_t, void const*) override {};
};

}
