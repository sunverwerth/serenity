/*
 * Copyright (c) 2022, Stephan Unverwerth <s.unverwerth@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/Array.h>
#include <AK/SIMD.h>
#include <LibSoftGPU/Config.h>

namespace SoftGPU {

class Shader;

class ShaderProcessor final {
public:
    void execute(Shader const&);

private:
    size_t m_stack_pointer { 0 };
    size_t m_instruction_pointer { 0 };
    AK::SIMD::i32x4 m_write_mask;
    Array<AK::SIMD::i32x4, SHADER_STACK_DEPTH> m_write_mask_stack;
    Array<AK::SIMD::f32x4, NUM_SHADER_REGISTERS> m_registers;
};

}
