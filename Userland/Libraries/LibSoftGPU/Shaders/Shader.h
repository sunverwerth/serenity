/*
 * Copyright (c) 2022, Stephan Unverwerth <s.unverwerth@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/Types.h>
#include <AK/Vector.h>
#include <LibSoftGPU/Shaders/Instruction.h>

namespace SoftGPU {

class Shader final {
public:
    Shader(Vector<Instruction> const& instructions, size_t entry_point)
        : m_instructions { instructions }
        , m_entry_point { entry_point }
    {
    }

    size_t entry_point() const { return m_entry_point; }
    Instruction instruction_at(size_t position) const { return m_instructions[position]; }
    size_t num_instructions() const { return m_instructions.size(); }

private:
    Vector<Instruction> m_instructions;
    size_t m_entry_point { 0 };
};

}
