#include <gmock/gmock.h>
#include "instruction_checks.hpp"



struct CLD_Implied_Expectations
{
    bool decimal_flag;
};

using CLDImplied     = CLD<Implied, CLD_Implied_Expectations, 2>;
using CLDImpliedMode = ParameterizedInstructionExecutorTestFixture<CLDImplied>;


template<>
void LoadInstructionIntoMemoryAndSetRegistersToInitialState(      InstructionExecutorTestFixture &fixture,
                                                            const CLDImplied                    &instruction_param)
{
    fixture.loadOpcodeIntoMemory(instruction_param.operation,
                                 AddressMode_e::Implied,
                                 instruction_param.address.instruction_address);

    // Load appropriate registers
    fixture.r.SetFlag(FLAGS6502::D, instruction_param.requirements.initial.decimal_flag);
}

template<>
void RegistersAreInExpectedState(const Registers &registers,
                                 const CLD_Implied_Expectations &expectations)
{
    EXPECT_THAT(registers.GetFlag(FLAGS6502::D), Eq(expectations.decimal_flag));
}

template<>
void MemoryContainsInstruction(const InstructionExecutorTestFixture &fixture,
                               const Instruction<AbstractInstruction_e::CLD, Implied> &)
{
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter ), Eq( OpcodeFor(AbstractInstruction_e::CLD, AddressMode_e::Implied) ));
}

template<>
void MemoryContainsExpectedComputation(const InstructionExecutorTestFixture &,
                                       const CLDImplied                    &)
{
    // No memory affected
}


static const std::vector<CLDImplied> CLDImpliedModeTestValues {
CLDImplied{
    // Clear the flag
    Implied().address(0x1000),
    CLDImplied::Requirements{
        .initial = {
            .decimal_flag = true
        },
        .final = {
            .decimal_flag = false
        }}
}
};

TEST_P(CLDImpliedMode, TypicalInstructionExecution)
{
    TypicalInstructionExecution(*this, GetParam());
}

INSTANTIATE_TEST_SUITE_P(CLDImpliedAtVariousAddresses,
                         CLDImpliedMode,
                         testing::ValuesIn(CLDImpliedModeTestValues) );
