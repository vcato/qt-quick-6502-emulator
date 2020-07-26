#include <gmock/gmock.h>
#include "instruction_checks.hpp"



struct CLI_Implied_Expectations
{
    bool interrupt_flag;
};

using CLIImplied     = CLI<Implied, CLI_Implied_Expectations, 2>;
using CLIImpliedMode = ParameterizedInstructionExecutorTestFixture<CLIImplied>;


template<>
void LoadInstructionIntoMemoryAndSetRegistersToInitialState(      InstructionExecutorTestFixture &fixture,
                                                            const CLIImplied                    &instruction_param)
{
    fixture.loadOpcodeIntoMemory(instruction_param.operation,
                                 AddressMode_e::Implied,
                                 instruction_param.address.instruction_address);

    // Load appropriate registers
    fixture.r.SetFlag(FLAGS6502::I, instruction_param.requirements.initial.interrupt_flag);
}

template<>
void RegistersAreInExpectedState(const Registers &registers,
                                 const CLI_Implied_Expectations &expectations)
{
    EXPECT_THAT(registers.GetFlag(FLAGS6502::I), Eq(expectations.interrupt_flag));
}

template<>
void MemoryContainsInstruction(const InstructionExecutorTestFixture &fixture,
                               const Instruction<AbstractInstruction_e::CLI, Implied> &)
{
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter ), Eq( OpcodeFor(AbstractInstruction_e::CLI, AddressMode_e::Implied) ));
}

template<>
void MemoryContainsExpectedComputation(const InstructionExecutorTestFixture &,
                                       const CLIImplied                    &)
{
    // No memory affected
}


static const std::vector<CLIImplied> CLDImpliedModeTestValues {
CLIImplied{
    // Clear the flag
    Implied().address(0x1000),
    CLIImplied::Requirements{
        .initial = {
            .interrupt_flag = true
        },
        .final = {
            .interrupt_flag = false
        }}
}
};

TEST_P(CLIImpliedMode, TypicalInstructionExecution)
{
    TypicalInstructionExecution(*this, GetParam());
}

INSTANTIATE_TEST_SUITE_P(CLDImpliedAtVariousAddresses,
                         CLIImpliedMode,
                         testing::ValuesIn(CLDImpliedModeTestValues) );
