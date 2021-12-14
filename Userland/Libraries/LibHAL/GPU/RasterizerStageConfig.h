/*
 * Copyright (c) 2021, Stephan Unverwerth <s.unverwerth@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <LibGfx/Rect.h>

namespace HAL::GPU {

enum class CullMode {
    None,
    Clockwise,
    CounterClockwise,
};

struct RasterizerStageConfig {
    Gfx::IntRect viewport;
    Gfx::IntRect scissor_rect;
    CullMode cull_mode;
};

}