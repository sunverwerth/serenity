/*
 * Copyright (c) 2022, Stephan Unverwerth <s.unverwerth@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <AK/Format.h>
#include <LibSoftGPU/Sampler.h>
#include <LibSoftGPU/Shaders/Shader.h>
#include <LibSoftGPU/Shaders/ShaderProcessor.h>

namespace SoftGPU {

ShaderProcessor::ShaderProcessor(Array<Sampler, NUM_SAMPLERS> const& samplers)
    : m_samplers { samplers }
{
}

void ShaderProcessor::execute(Shader const& shader)
{
    m_instruction_pointer = shader.entry_point();
    while (m_instruction_pointer < shader.num_instructions()) {
        auto instruction = shader.instruction_at(m_instruction_pointer);
        switch (instruction.op) {
        case Opcode::Exit:
            break;
        case Opcode::Mov:
            op_mov(instruction);
            break;
        case Opcode::Texture2D:
            op_texture_2d(instruction);
            break;
        }
        m_instruction_pointer++;
    }
}

void ShaderProcessor::op_mov(Instruction instruction)
{
    const size_t in = instruction.arg1 * 4;
    const size_t out = instruction.out_register * 4;
    set_register_with_current_mask(out, m_registers[in]);
    set_register_with_current_mask(out + 1, m_registers[in + 1]);
    set_register_with_current_mask(out + 2, m_registers[in + 2]);
    set_register_with_current_mask(out + 3, m_registers[in + 3]);
}

void ShaderProcessor::op_texture_2d(Instruction instruction)
{
    const size_t sampler = instruction.arg1;
    const size_t uv_register = instruction.arg2 * 4;
    const size_t out = instruction.out_register * 4;

    auto sample = m_samplers[sampler].sample_2d({ m_registers[uv_register], m_registers[uv_register + 1] });

    set_register_with_current_mask(out, sample.x());
    set_register_with_current_mask(out + 1, sample.y());
    set_register_with_current_mask(out + 2, sample.z());
    set_register_with_current_mask(out + 3, sample.w());
}

void ShaderProcessor::set_register_with_current_mask(size_t register_index, AK::SIMD::f32x4 value)
{
    m_registers[register_index] = m_write_mask ? value : m_registers[register_index];
}

}
