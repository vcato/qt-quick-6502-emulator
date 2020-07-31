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
void MemoryContainsExpectedResult(const InstructionExecutorTestFixture &,
                                  const TInstructionAndAddressingMode  &)
{
    // Default empty for everything.
    // Only specialize for memory-affecting operations!
}

template<typename TInstructionAndAddressingMode>
void LoadInstructionIntoMemoryAndSetRegistersToInitialState(      InstructionExecutorTestFixture &fixture,
                                                            const TInstructionAndAddressingMode  &instruction_param);

template<class TInstructionAndAddressingMode>
void SetupTypicalExecutionState(      InstructionExecutorTestFixture &fixture,
                                const TInstructionAndAddressingMode  &instruction);


// Function definitions
template<AbstractInstruction_e TOperation,
         typename TAddressingMode>
bool ProgramCounterIsSetToInstructionAddress(const InstructionExecutor                      &executor,
                                             const Instruction<TOperation, TAddressingMode> &instruction)
{
    return executor.registers().program_counter == instruction.address.instruction_address;
}

// NOTE: There are specializations in other files.
template<class TInstructionAndAddressingMode>
bool ProgramCounterIsSetToCorrectValue(const InstructionExecutor           &executor,
                                       const TInstructionAndAddressingMode &instruction)
{
    // One past the instruction, for typical instructions.
    return executor.registers().program_counter == (instruction.address.instruction_address +
                                                    instruction.address.operand_byte_count  + 1);
}

// This will be redefined for certain addressing modes
template<class TInstructionAndAddressingMode>
void InstructionExecutedInExpectedClockTicks(const InstructionExecutorTestFixture &fixture,
                                             const TInstructionAndAddressingMode  &instruction)
{
    EXPECT_THAT(fixture.executor.clock_ticks, Eq(instruction.requirements.cycle_count));
}

template<class TInstructionAndAddressingMode>
void SetupTypicalExecutionState(      InstructionExecutorTestFixture &fixture,
                                const TInstructionAndAddressingMode  &instruction)
{
    LoadInstructionIntoMemoryAndSetRegistersToInitialState(fixture, instruction);

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
    EXPECT_TRUE(ProgramCounterIsSetToCorrectValue(fixture.executor, instruction));
    EXPECT_THAT(fixture.executor.complete(), Eq(true));
    InstructionExecutedInExpectedClockTicks(fixture, instruction);
    RegistersAreInExpectedState(fixture.executor.registers(), instruction.requirements.final);
    MemoryContainsExpectedResult(fixture, instruction);
}

template <typename InstructionParam>
void StoreOpcode(InstructionExecutorTestFixture &fixture, const InstructionParam &instruction_param)
{
    fixture.loadOpcodeIntoMemory(
        instruction_param.operation,
        instruction_param.address.address_mode,
        instruction_param.address.instruction_address
        );
}


/** Tests the execution of an instruction.
 *
 *  @param fixture The test fixture to use for the test
 *  @param param   The instruction and state to test for
 */
template<typename TFixture, typename TParam>
void TypicalInstructionExecution(TFixture &fixture, const TParam &param)
{
    SetupTypicalExecutionState(fixture, param);

    fixture.executeInstruction();

    CheckTypicalExecutionResults(fixture, param);
}

#endif // INSTRUCTION_CHECKS_HPP
