/*
 * Copyright (c) 2022, Stephan Unverwerth <s.unverwerth@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/Types.h>
#include <LibSoftGPU/Shaders/Opcode.h>

namespace SoftGPU {

struct Instruction {
    Opcode op;
    u8 out_register;
    u8 arg1;
    u8 arg2;
};

}
