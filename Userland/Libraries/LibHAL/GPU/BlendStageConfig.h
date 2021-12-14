/*
 * Copyright (c) 2021, Stephan Unverwerth <s.unverwerth@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/Types.h>

namespace HAL::GPU {

enum class BlendFactor {
    One,
    Zero,
    SrcAlpha,
    OneMinusSrcAlpha,
    DstAlpha,
    OneMinusDstAlpha,
    Constant,
};

enum class BlendOp {
    Add,
    Subtract,
};

struct BlendStageConfig {
    bool blend_enabled;
    BlendFactor color_source_factor;
    BlendOp color_op;
    BlendFactor color_destination_factor;
    BlendFactor alpha_source_factor;
    BlendOp alpha_op;
    BlendFactor alpha_destination_factor;
    bool red_write_enabled;
    bool green_write_enabled;
    bool blue_write_enabled;
    bool alpha_write_enabled;
};

}