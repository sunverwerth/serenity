/*
 * Copyright (c) 2022, cflip <cflip@cflip.net>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "Buffer.h"

namespace SoftGPU {

void Buffer::upload_data(size_t offset, size_t size, void const* data)
{
    VERIFY(offset + size < m_data.size());
    memcpy(&m_data.at(0), data, size);
}

}
