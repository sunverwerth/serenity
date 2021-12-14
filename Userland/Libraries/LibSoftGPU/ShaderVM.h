/*
 * Copyright (c) 2021, Stephan Unverwerth <s.unverwerth@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/SIMD.h>
#include <AK/Vector.h>
#include <LibHAL/GPU/VMInstruction.h>
#include <LibSoftGPU/Sampler.h>

namespace SoftGPU {

using AK::SIMD::f32x4;

static constexpr const size_t shader_register_count = 256;
static constexpr const size_t shader_sampler_count = 32;

class ShaderVM final {
public:
    void execute(Vector<HAL::GPU::VMInstruction> const&);

    void write_register(size_t index, f32x4 value)
    {
        VERIFY(index < shader_register_count);
        m_registers[index] = value;
    }

    f32x4 read_register(size_t index) const
    {
        VERIFY(index < shader_register_count);
        return m_registers[index];
    }

private:
    void op_discard(HAL::GPU::VMInstruction const&);
    void op_mov(HAL::GPU::VMInstruction const&);
    void op_add(HAL::GPU::VMInstruction const&);
    void op_sub(HAL::GPU::VMInstruction const&);
    void op_mul(HAL::GPU::VMInstruction const&);
    void op_div(HAL::GPU::VMInstruction const&);
    void op_dot(HAL::GPU::VMInstruction const&);
    void op_sqrt(HAL::GPU::VMInstruction const&);
    void op_ddx(HAL::GPU::VMInstruction const&);
    void op_ddy(HAL::GPU::VMInstruction const&);
    void op_texture_2d(HAL::GPU::VMInstruction const&);
    void op_texture_2d_lod(HAL::GPU::VMInstruction const&);
    void op_if(HAL::GPU::VMInstruction const&);
    void op_else(HAL::GPU::VMInstruction const&);
    void op_end_if(HAL::GPU::VMInstruction const&);

private:
    size_t m_instruction_pointer { 0 };
    f32x4 m_registers[shader_register_count];
    Sampler m_samplers[shader_sampler_count];
};

}