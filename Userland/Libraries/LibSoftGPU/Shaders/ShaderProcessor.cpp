/*
 * Copyright (c) 2022, Stephan Unverwerth <s.unverwerth@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <LibSoftGPU/Shaders/Shader.h>
#include <LibSoftGPU/Shaders/ShaderProcessor.h>

namespace SoftGPU {

void ShaderProcessor::execute(Shader const& shader)
{
    m_instruction_pointer = shader.entry_point();
    while (m_instruction_pointer < shader.num_instructions()) {
        auto instruction = shader.instruction_at(m_instruction_pointer);
        switch (instruction.op) {
        case Opcode::Exit:
            break;
        }
        m_instruction_pointer++;
    }
}

}
