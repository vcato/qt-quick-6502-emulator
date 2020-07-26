#include <gmock/gmock.h>
#include "instruction_checks.hpp"



struct CLV_Implied_Expectations
{
    bool overflow_flag;
};

using CLVImplied     = CLV<Implied, CLV_Implied_Expectations, 2>;
using CLVImpliedMode = ParameterizedInstructionExecutorTestFixture<CLVImplied>;


template<>
void LoadInstructionIntoMemoryAndSetRegistersToInitialState(      InstructionExecutorTestFixture &fixture,
                                                            const CLVImplied                    &instruction_param)
{
    fixture.loadOpcodeIntoMemory(instruction_param.operation,
                                 AddressMode_e::Implied,
                                 instruction_param.address.instruction_address);

    // Load appropriate registers
    fixture.r.SetFlag(FLAGS6502::V, instruction_param.requirements.initial.overflow_flag);
}

template<>
void RegistersAreInExpectedState(const Registers &registers,
                                 const CLV_Implied_Expectations &expectations)
{
    EXPECT_THAT(registers.GetFlag(FLAGS6502::V), Eq(expectations.overflow_flag));
}

template<>
void MemoryContainsInstruction(const InstructionExecutorTestFixture &fixture,
                               const Instruction<AbstractInstruction_e::CLV, Implied> &)
{
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter ), Eq( OpcodeFor(AbstractInstruction_e::CLV, AddressMode_e::Implied) ));
}

template<>
void MemoryContainsExpectedComputation(const InstructionExecutorTestFixture &,
                                       const CLVImplied                    &)
{
    // No memory affected
}


static const std::vector<CLVImplied> CLVImpliedModeTestValues {
CLVImplied{
    // Clear the flag
    Implied().address(0x1000),
    CLVImplied::Requirements{
        .initial = {
            .overflow_flag = true
        },
        .final = {
            .overflow_flag = false
        }}
}
};

TEST_P(CLVImpliedMode, TypicalInstructionExecution)
{
    TypicalInstructionExecution(*this, GetParam());
}

INSTANTIATE_TEST_SUITE_P(CLVImpliedAtVariousAddresses,
                         CLVImpliedMode,
                         testing::ValuesIn(CLVImpliedModeTestValues) );
