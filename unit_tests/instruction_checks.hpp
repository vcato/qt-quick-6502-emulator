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

#endif // INSTRUCTION_CHECKS_HPP
