/*
 * Copyright (c) 2021, Stephan Unverwerth <s.unverwerth@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <AK/Format.h>
#include <LibSoftGPU/Buffer.h>

namespace SoftGPU {
void Buffer::update_range(size_t offset, void const* data, size_t size)
{
    //dbgln("Update buffer range {}:{}", offset, size);
    m_data.overwrite(offset, data, size);
}
}