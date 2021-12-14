#include <LibSoftGPU/ShaderVM.h>

namespace SoftGPU {

void ShaderVM::execute(Vector<HAL::GPU::VMInstruction> const& instructions)
{
    m_instruction_pointer = 0;
    auto num_instructions = instructions.size();
    while (m_instruction_pointer < num_instructions) {
        auto const& instruction = instructions[m_instruction_pointer];
        switch (instruction.opcode) {
        case HAL::GPU::Opcode::Discard:
            op_discard(instruction);
            break;
        case HAL::GPU::Opcode::Mov:
            op_mov(instruction);
            break;
        case HAL::GPU::Opcode::Add:
            op_add(instruction);
            break;
        case HAL::GPU::Opcode::Sub:
            op_sub(instruction);
            break;
        case HAL::GPU::Opcode::Mul:
            op_mul(instruction);
            break;
        case HAL::GPU::Opcode::Div:
            op_div(instruction);
            break;
        case HAL::GPU::Opcode::Dot:
            op_dot(instruction);
            break;
        case HAL::GPU::Opcode::Sqrt:
            op_sqrt(instruction);
            break;
        case HAL::GPU::Opcode::DDX:
            op_ddx(instruction);
            break;
        case HAL::GPU::Opcode::DDY:
            op_ddy(instruction);
            break;
        case HAL::GPU::Opcode::Texture2D:
            op_texture_2d(instruction);
            break;
        case HAL::GPU::Opcode::Texture2DLod:
            op_texture_2d_lod(instruction);
            break;
        case HAL::GPU::Opcode::If:
            op_if(instruction);
            break;
        case HAL::GPU::Opcode::Else:
            op_else(instruction);
            break;
        case HAL::GPU::Opcode::EndIf:
            op_end_if(instruction);
            break;
        }
        m_instruction_pointer++;
    }
}

void ShaderVM::op_discard(HAL::GPU::VMInstruction const& instruction)
{
    (void)instruction;
}

void ShaderVM::op_mov(HAL::GPU::VMInstruction const& instruction)
{
    (void)instruction;
}

void ShaderVM::op_add(HAL::GPU::VMInstruction const& instruction)
{
    (void)instruction;
}

void ShaderVM::op_sub(HAL::GPU::VMInstruction const& instruction)
{
    (void)instruction;
}

void ShaderVM::op_mul(HAL::GPU::VMInstruction const& instruction)
{
    (void)instruction;
}

void ShaderVM::op_div(HAL::GPU::VMInstruction const& instruction)
{
    (void)instruction;
}

void ShaderVM::op_dot(HAL::GPU::VMInstruction const& instruction)
{
    (void)instruction;
}

void ShaderVM::op_sqrt(HAL::GPU::VMInstruction const& instruction)
{
    (void)instruction;
}

void ShaderVM::op_ddx(HAL::GPU::VMInstruction const& instruction)
{
    (void)instruction;
}

void ShaderVM::op_ddy(HAL::GPU::VMInstruction const& instruction)
{
    (void)instruction;
}

void ShaderVM::op_texture_2d(HAL::GPU::VMInstruction const& instruction)
{
    (void)instruction;
}

void ShaderVM::op_texture_2d_lod(HAL::GPU::VMInstruction const& instruction)
{
    (void)instruction;
}

void ShaderVM::op_if(HAL::GPU::VMInstruction const& instruction)
{
    (void)instruction;
}

void ShaderVM::op_else(HAL::GPU::VMInstruction const& instruction)
{
    (void)instruction;
}

void ShaderVM::op_end_if(HAL::GPU::VMInstruction const& instruction)
{
    (void)instruction;
}

}