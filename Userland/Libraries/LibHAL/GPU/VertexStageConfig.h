/*
 * Copyright (c) 2021, Stephan Unverwerth <s.unverwerth@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/NonnullRefPtr.h>
#include <LibHAL/GPU/Shader.h>

namespace HAL::GPU {

struct VertexStageConfig {
    NonnullRefPtr<Shader> vertex_shader;
};

}