#ifndef INSTRUCTION_CHECKS_HPP
#define INSTRUCTION_CHECKS_HPP

#include "instructionexecutor.hpp"
#include "instruction_definitions.hpp"


template<AbstractInstruction_e TOperation,
         typename TAddressingMode>
bool ProgramCounterIsSetToInstructionAddress(const InstructionExecutor                      &executor,
                                             const Instruction<TOperation, TAddressingMode> &instruction)
{
    return executor.registers().program_counter == instruction.address.instruction_address;
}

template<AbstractInstruction_e TOperation,
         typename TAddressingMode>
bool ProgramCounterIsSetToOnePastTheEntireInstruction(const InstructionExecutor                      &executor,
                                                      const Instruction<TOperation, TAddressingMode> &instruction)
{
    return executor.registers().program_counter == (instruction.address.instruction_address +
                                                    instruction.address.operand_byte_count  + 1);
}

#endif // INSTRUCTION_CHECKS_HPP
