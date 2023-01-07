/*
 * Copyright (c) 2023, Stephan Unverwerth <s.unverwerth@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <AK/Vector.h>

#pragma once

namespace GPU {

enum class VertexAttributeType {
    None,
    Float,
    Float2,
    Float3,
    Float4,
};

struct VertexAttributeConfig {
    VertexAttributeType type { VertexAttributeType::None };
    size_t offset { 0 };
    size_t stride { 0 };
    unsigned buffer_binding_index { 0 };
    unsigned instance_divisor { 0 };
};

struct InputAssemblyConfig final {
    Vector<VertexAttributeConfig> attributes;
};

}
