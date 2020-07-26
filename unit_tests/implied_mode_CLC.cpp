#include <gmock/gmock.h>
#include "instruction_checks.hpp"



struct CLC_Implied_Expectations
{
    bool carry_flag;
};

using CLCImplied     = CLC<Implied, CLC_Implied_Expectations, 2>;
using CLCImpliedMode = ParameterizedInstructionExecutorTestFixture<CLCImplied>;


template<>
void LoadInstructionIntoMemoryAndSetRegistersToInitialState(      InstructionExecutorTestFixture &fixture,
                                                            const CLCImplied                    &instruction_param)
{
    fixture.loadOpcodeIntoMemory(instruction_param.operation,
                                 AddressMode_e::Implied,
                                 instruction_param.address.instruction_address);

    // Load appropriate registers
    fixture.r.SetFlag(FLAGS6502::C, instruction_param.requirements.initial.carry_flag);
}

template<>
void RegistersAreInExpectedState(const Registers &registers,
                                 const CLC_Implied_Expectations &expectations)
{
    EXPECT_THAT(registers.GetFlag(FLAGS6502::C), Eq(expectations.carry_flag));
}

template<>
void MemoryContainsInstruction(const InstructionExecutorTestFixture &fixture,
                               const Instruction<AbstractInstruction_e::CLC, Implied> &)
{
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter ), Eq( OpcodeFor(AbstractInstruction_e::CLC, AddressMode_e::Implied) ));
}

template<>
void MemoryContainsExpectedComputation(const InstructionExecutorTestFixture &,
                                       const CLCImplied                    &)
{
    // No memory affected
}


static const std::vector<CLCImplied> CLCImpliedModeTestValues {
CLCImplied{
    // Clear the flag
    Implied().address(0x1000),
    CLCImplied::Requirements{
        .initial = {
            .carry_flag = true
        },
        .final = {
            .carry_flag = false
        }}
}
};

TEST_P(CLCImpliedMode, TypicalInstructionExecution)
{
    TypicalInstructionExecution(*this, GetParam());
}

INSTANTIATE_TEST_SUITE_P(CLCImpliedAtVariousAddresses,
                         CLCImpliedMode,
                         testing::ValuesIn(CLCImpliedModeTestValues) );
