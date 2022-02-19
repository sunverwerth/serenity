/*
 * Copyright (c) 2022, Stephan Unverwerth <s.unverwerth@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/Types.h>

namespace SoftGPU {

enum class Opcode : u8 {
    Exit,
    Discard,
    Mov,
    Texture2D,
    Add,
    Sub,
    Mul,
    Div,
    CmpLt,
    CmpGt,
    CmpLte,
    CmpGte,
    CmpEq,
    CmpNeq,
    If,
    Else,
    EndIf,
    Blend,
};

}
