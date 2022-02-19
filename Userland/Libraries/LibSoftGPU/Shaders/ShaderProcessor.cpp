/*
 * Copyright (c) 2022, Stephan Unverwerth <s.unverwerth@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <AK/Format.h>
#include <AK/SIMDExtras.h>
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
    m_stack_pointer = 0;

    while (m_instruction_pointer < shader.num_instructions()) {
        auto instruction = shader.instruction_at(m_instruction_pointer);
        switch (instruction.op) {
        case Opcode::Exit:
            break;
        case Opcode::Discard:
            if (op_discard(instruction))
                return;
            break;
        case Opcode::Mov:
            op_mov(instruction);
            break;
        case Opcode::Texture2D:
            op_texture_2d(instruction);
            break;
        case Opcode::Add:
            op_add(instruction);
            break;
        case Opcode::Sub:
            op_sub(instruction);
            break;
        case Opcode::Mul:
            op_mul(instruction);
            break;
        case Opcode::Div:
            op_div(instruction);
            break;
        case Opcode::CmpLt:
            op_cmp_lt(instruction);
            break;
        case Opcode::CmpGt:
            op_cmp_gt(instruction);
            break;
        case Opcode::CmpLte:
            op_cmp_lte(instruction);
            break;
        case Opcode::CmpGte:
            op_cmp_gte(instruction);
            break;
        case Opcode::CmpEq:
            op_cmp_eq(instruction);
            break;
        case Opcode::CmpNeq:
            op_cmp_neq(instruction);
            break;
        case Opcode::If:
            op_if(instruction);
            break;
        case Opcode::Else:
            op_else(instruction);
            break;
        case Opcode::EndIf:
            op_end_if(instruction);
            break;
        }
        m_instruction_pointer++;
    }
}

bool ShaderProcessor::op_discard(Instruction)
{
    if (m_stack_pointer == 0) {
        // discarding at top level scope rejects all fragments and ends the program
        m_write_mask = AK::SIMD::i32x4 { 0, 0, 0, 0 };
        return true;
    } else {
        // mask out all fragments in the stack that are currently active
        for (size_t i = 0; i < m_write_mask_stack.size(); i++)
            m_write_mask_stack[i] &= ~m_write_mask;

        // if all fragments at top level are discarded we can exit immediately
        if (AK::SIMD::none(m_write_mask_stack[0])) {
            m_write_mask = m_write_mask_stack[0];
            return true;
        }

        m_write_mask = AK::SIMD::i32x4 { 0, 0, 0, 0 };
    }

    return false;
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

void ShaderProcessor::op_add(Instruction instruction)
{
    const size_t in1 = instruction.arg1 * 4;
    const size_t in2 = instruction.arg2 * 4;
    const size_t out = instruction.out_register * 4;

    set_register_with_current_mask(out, m_registers[in1] + m_registers[in2]);
    set_register_with_current_mask(out + 1, m_registers[in1 + 1] + m_registers[in2 + 1]);
    set_register_with_current_mask(out + 2, m_registers[in1 + 2] + m_registers[in2 + 2]);
    set_register_with_current_mask(out + 3, m_registers[in1 + 3] + m_registers[in2 + 3]);
}

void ShaderProcessor::op_sub(Instruction instruction)
{
    const size_t in1 = instruction.arg1 * 4;
    const size_t in2 = instruction.arg2 * 4;
    const size_t out = instruction.out_register * 4;

    set_register_with_current_mask(out, m_registers[in1] - m_registers[in2]);
    set_register_with_current_mask(out + 1, m_registers[in1 + 1] - m_registers[in2 + 1]);
    set_register_with_current_mask(out + 2, m_registers[in1 + 2] - m_registers[in2 + 2]);
    set_register_with_current_mask(out + 3, m_registers[in1 + 3] - m_registers[in2 + 3]);
}

void ShaderProcessor::op_mul(Instruction instruction)
{
    const size_t in1 = instruction.arg1 * 4;
    const size_t in2 = instruction.arg2 * 4;
    const size_t out = instruction.out_register * 4;

    set_register_with_current_mask(out, m_registers[in1] * m_registers[in2]);
    set_register_with_current_mask(out + 1, m_registers[in1 + 1] * m_registers[in2 + 1]);
    set_register_with_current_mask(out + 2, m_registers[in1 + 2] * m_registers[in2 + 2]);
    set_register_with_current_mask(out + 3, m_registers[in1 + 3] * m_registers[in2 + 3]);
}

void ShaderProcessor::op_div(Instruction instruction)
{
    const size_t in1 = instruction.arg1 * 4;
    const size_t in2 = instruction.arg2 * 4;
    const size_t out = instruction.out_register * 4;

    set_register_with_current_mask(out, m_registers[in1] / m_registers[in2]);
    set_register_with_current_mask(out + 1, m_registers[in1 + 1] / m_registers[in2 + 1]);
    set_register_with_current_mask(out + 2, m_registers[in1 + 2] / m_registers[in2 + 2]);
    set_register_with_current_mask(out + 3, m_registers[in1 + 3] / m_registers[in2 + 3]);
}

void ShaderProcessor::op_cmp_lt(Instruction instruction)
{
    const size_t in1 = instruction.arg1 * 4;
    const size_t in2 = instruction.arg2 * 4;
    const size_t out = instruction.out_register * 4;

    set_register_with_current_mask(out, m_registers[in1] < m_registers[in2]);
    set_register_with_current_mask(out + 1, m_registers[in1 + 1] < m_registers[in2 + 1]);
    set_register_with_current_mask(out + 2, m_registers[in1 + 2] < m_registers[in2 + 2]);
    set_register_with_current_mask(out + 3, m_registers[in1 + 3] < m_registers[in2 + 3]);
}

void ShaderProcessor::op_cmp_gt(Instruction instruction)
{
    const size_t in1 = instruction.arg1 * 4;
    const size_t in2 = instruction.arg2 * 4;
    const size_t out = instruction.out_register * 4;

    set_register_with_current_mask(out, m_registers[in1] > m_registers[in2]);
    set_register_with_current_mask(out + 1, m_registers[in1 + 1] > m_registers[in2 + 1]);
    set_register_with_current_mask(out + 2, m_registers[in1 + 2] > m_registers[in2 + 2]);
    set_register_with_current_mask(out + 3, m_registers[in1 + 3] > m_registers[in2 + 3]);
}

void ShaderProcessor::op_cmp_lte(Instruction instruction)
{
    const size_t in1 = instruction.arg1 * 4;
    const size_t in2 = instruction.arg2 * 4;
    const size_t out = instruction.out_register * 4;

    set_register_with_current_mask(out, m_registers[in1] <= m_registers[in2]);
    set_register_with_current_mask(out + 1, m_registers[in1 + 1] <= m_registers[in2 + 1]);
    set_register_with_current_mask(out + 2, m_registers[in1 + 2] <= m_registers[in2 + 2]);
    set_register_with_current_mask(out + 3, m_registers[in1 + 3] <= m_registers[in2 + 3]);
}

void ShaderProcessor::op_cmp_gte(Instruction instruction)
{
    const size_t in1 = instruction.arg1 * 4;
    const size_t in2 = instruction.arg2 * 4;
    const size_t out = instruction.out_register * 4;

    set_register_with_current_mask(out, m_registers[in1] >= m_registers[in2]);
    set_register_with_current_mask(out + 1, m_registers[in1 + 1] >= m_registers[in2 + 1]);
    set_register_with_current_mask(out + 2, m_registers[in1 + 2] >= m_registers[in2 + 2]);
    set_register_with_current_mask(out + 3, m_registers[in1 + 3] >= m_registers[in2 + 3]);
}

void ShaderProcessor::op_cmp_eq(Instruction instruction)
{
    const size_t in1 = instruction.arg1 * 4;
    const size_t in2 = instruction.arg2 * 4;
    const size_t out = instruction.out_register * 4;

    set_register_with_current_mask(out, m_registers[in1] == m_registers[in2]);
    set_register_with_current_mask(out + 1, m_registers[in1 + 1] == m_registers[in2 + 1]);
    set_register_with_current_mask(out + 2, m_registers[in1 + 2] == m_registers[in2 + 2]);
    set_register_with_current_mask(out + 3, m_registers[in1 + 3] == m_registers[in2 + 3]);
}

void ShaderProcessor::op_cmp_neq(Instruction instruction)
{
    const size_t in1 = instruction.arg1 * 4;
    const size_t in2 = instruction.arg2 * 4;
    const size_t out = instruction.out_register * 4;

    set_register_with_current_mask(out, m_registers[in1] != m_registers[in2]);
    set_register_with_current_mask(out + 1, m_registers[in1 + 1] != m_registers[in2 + 1]);
    set_register_with_current_mask(out + 2, m_registers[in1 + 2] != m_registers[in2 + 2]);
    set_register_with_current_mask(out + 3, m_registers[in1 + 3] != m_registers[in2 + 3]);
}

void ShaderProcessor::op_if(Instruction instruction)
{
    m_write_mask_stack[m_stack_pointer++] = m_write_mask;
    m_write_mask &= (AK::SIMD::i32x4)m_registers[instruction.arg1 * 4];
    const size_t skip = instruction.arg2;

    if (AK::SIMD::none(m_write_mask)) {
        m_instruction_pointer += skip;
        return;
    }
}

void ShaderProcessor::op_else(Instruction instruction)
{
    m_write_mask = ~m_write_mask;
    const size_t skip = instruction.arg2;
    if (AK::SIMD::none(m_write_mask)) {
        m_instruction_pointer += skip;
        return;
    }
}

void ShaderProcessor::op_end_if(Instruction)
{
    m_write_mask = m_write_mask_stack[--m_stack_pointer];
}

void ShaderProcessor::set_register_with_current_mask(size_t register_index, AK::SIMD::f32x4 value)
{
    m_registers[register_index] = m_write_mask ? value : m_registers[register_index];
}

void ShaderProcessor::set_register_with_current_mask(size_t register_index, AK::SIMD::i32x4 value)
{
    m_registers[register_index] = m_write_mask ? (AK::SIMD::f32x4)value : m_registers[register_index];
}

}
