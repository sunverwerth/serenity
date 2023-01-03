/*
 * Copyright (c) 2022, Stephan Unverwerth <s.unverwerth@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <LibVirtGPU/Buffer.h>

namespace VirtGPU {

Buffer::Buffer(void const* ownership_token)
    : GPU::Buffer(ownership_token, 0)
{
}

}
