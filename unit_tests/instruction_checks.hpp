#ifndef INSTRUCTION_CHECKS_HPP
#define INSTRUCTION_CHECKS_HPP

#include "instructionexecutor.hpp"
#include "instruction_definitions.hpp"
#include "InstructionExecutorTestFixture.hpp"
#include <gmock/gmock.h>

using namespace testing;

// Forward declarations
template<class TInstructionAndAddressingModeExpectations>
void RegistersAreInExpectedState(const Registers &registers,
                                 const TInstructionAndAddressingModeExpectations &expectations);

template<AbstractInstruction_e TOperation,
         typename TAddressingMode>
void MemoryContainsInstruction(const InstructionExecutorTestFixture &fixture,
                               const Instruction<TOperation, TAddressingMode> &instruction);

template<class TInstructionAndAddressingMode>
void MemoryContainsExpectedComputation(const InstructionExecutorTestFixture &fixture,
                                       const TInstructionAndAddressingMode  &instruction);

template<class TInstructionAndAddressingMode>
void SetupTypicalExecutionState(const InstructionExecutorTestFixture &fixture,
                                const TInstructionAndAddressingMode  &instruction);


// Function definitions
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

template<class TInstructionAndAddressingMode>
void InstructionExecutedInExpectedClockTicks(const InstructionExecutorTestFixture &fixture,
                                             const TInstructionAndAddressingMode  &instruction)
{
    EXPECT_THAT(fixture.executor.clock_ticks, Eq(instruction.requirements.cycle_count));
}

template<class TInstructionAndAddressingMode>
void SetupTypicalExecutionState(const InstructionExecutorTestFixture &fixture,
                                const TInstructionAndAddressingMode  &instruction)
{
    EXPECT_TRUE(ProgramCounterIsSetToInstructionAddress(fixture.executor, instruction));
    EXPECT_THAT(fixture.executor.complete(), Eq(true));
    EXPECT_THAT(fixture.executor.clock_ticks, Eq(0U));
    MemoryContainsInstruction(fixture, instruction);
    MemoryContainsExpectedComputation(fixture, instruction);
    RegistersAreInExpectedState(fixture.executor.registers(), instruction.requirements.initial);
}

template<class TInstructionAndAddressingMode>
void CheckTypicalExecutionResults(const InstructionExecutorTestFixture &fixture,
                                  const TInstructionAndAddressingMode  &instruction)
{
    EXPECT_TRUE(ProgramCounterIsSetToOnePastTheEntireInstruction(fixture.executor, instruction));
    EXPECT_THAT(fixture.executor.complete(), Eq(true));
    InstructionExecutedInExpectedClockTicks(fixture, instruction);
    RegistersAreInExpectedState(fixture.executor.registers(), instruction.requirements.final);
}

#endif // INSTRUCTION_CHECKS_HPP
