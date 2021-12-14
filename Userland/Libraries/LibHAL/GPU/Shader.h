/*
 * Copyright (c) 2021, Stephan Unverwerth <s.unverwerth@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/RefCounted.h>

namespace HAL::GPU {

enum class ShaderStage {
    Vertex,
    Fragment,
};

class Shader : public RefCounted<Shader> {
public:
};

}