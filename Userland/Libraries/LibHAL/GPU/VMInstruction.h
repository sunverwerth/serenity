/*
 * Copyright (c) 2021, Stephan Unverwerth <s.unverwerth@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/Types.h>

namespace HAL::GPU {

// FIXME: This is a very simple bytecode format to get the SoftGPU up and running.
// Once we implement actual hardware drivers we might want to switch
// to something more standardized like e.g SPIR-V.

inline static constexpr u8 swizzle_pattern(u8 a, u8 b, u8 c, u8 d)
{
    return (a & 3) | (b & 3) << 2 | (c & 3) << 4 | (d & 3) << 6;
}

inline static constexpr u8 const uniform_swizzle_pattern = swizzle_pattern(0, 1, 2, 3);

enum class Opcode : u8 {
    Discard,
    Mov,
    Add,
    Sub,
    Mul,
    Div,
    Dot,
    Sqrt,
    DDX,
    DDY,
    Texture2D,
    Texture2DLod,
    If,
    Else,
    EndIf,
};

struct VMInstruction {
    Opcode opcode;
    u8 destination;
    u8 param1;
    u8 param2;
};

}