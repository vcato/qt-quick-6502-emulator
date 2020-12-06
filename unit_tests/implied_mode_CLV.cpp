#include "addressing_mode_helpers.hpp"



struct CLV_Implied_Expectations
{
    bool overflow_flag;
};

using CLVImplied     = CLV<Implied, CLV_Implied_Expectations, 2>;
using CLVImpliedMode = ParameterizedInstructionExecutorTestFixture<CLVImplied>;


static void SetupAffectedOrUsedRegisters(InstructionExecutorTestFixture &fixture, const CLVImplied &instruction_param)
{
    fixture.r.SetFlag(FLAGS6502::V, instruction_param.requirements.initial.overflow_flag);
}

template<>
void LoadInstructionIntoMemoryAndSetRegistersToInitialState(      InstructionExecutorTestFixture &fixture,
                                                            const CLVImplied                    &instruction_param)
{
    SetupRAMForInstructionsThatHaveImpliedValue(fixture, instruction_param);
    SetupAffectedOrUsedRegisters(fixture, instruction_param);
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
