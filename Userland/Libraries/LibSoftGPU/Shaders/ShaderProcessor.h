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
class Sampler;

class ShaderProcessor final {
public:
    ShaderProcessor(Array<Sampler, NUM_SAMPLERS> const&);

    void execute(Shader const&);

    AK::SIMD::f32x4 get_register(size_t index) const { return m_registers[index]; }
    void set_register(size_t index, AK::SIMD::f32x4 v) { m_registers[index] = v; }

    void set_mask(AK::SIMD::i32x4 mask) { m_write_mask = mask; };
    AK::SIMD::i32x4 mask() const { return m_write_mask; }

private:
    bool op_discard(Instruction);
    void op_mov(Instruction);
    void op_texture_2d(Instruction);
    void op_add(Instruction);
    void op_sub(Instruction);
    void op_mul(Instruction);
    void op_div(Instruction);
    void op_cmp_lt(Instruction);
    void op_cmp_gt(Instruction);
    void op_cmp_lte(Instruction);
    void op_cmp_gte(Instruction);
    void op_cmp_eq(Instruction);
    void op_cmp_neq(Instruction);
    void op_if(Instruction);
    void op_else(Instruction);
    void op_end_if(Instruction);

    void set_register_with_current_mask(size_t, AK::SIMD::f32x4);
    void set_register_with_current_mask(size_t, AK::SIMD::i32x4);

private:
    size_t m_stack_pointer { 0 };
    size_t m_instruction_pointer { 0 };
    AK::SIMD::i32x4 m_write_mask;
    Array<AK::SIMD::i32x4, SHADER_STACK_DEPTH> m_write_mask_stack;
    Array<AK::SIMD::f32x4, NUM_SHADER_REGISTERS> m_registers;
    Array<Sampler, NUM_SAMPLERS> const& m_samplers;
};

}
