/*
 * Copyright (c) 2021, Stephan Unverwerth <s.unverwerth@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/NonnullRefPtr.h>
#include <AK/Vector.h>
#include <LibHAL/GPU/Buffer.h>

namespace HAL::GPU {

enum class VertexElementType {
    U8,
    I8,
    U16,
    I16,
    U32,
    I32,
    F32,
    F64,
};

struct VertexStream {
    NonnullRefPtr<Buffer> buffer;
    size_t first_element;
    size_t element_stride;
    VertexElementType element_type;
    unsigned element_count;
    unsigned instance_divisor;
};

struct InputAssemblyStageConfig {
    Vector<VertexStream> streams;
};

}