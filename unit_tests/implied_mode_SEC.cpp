#include "addressing_mode_helpers.hpp"



struct SEC_Implied_Expectations
{
    bool carry_flag;
};

using SECImplied     = SEC<Implied, SEC_Implied_Expectations, 2>;
using SECImpliedMode = ParameterizedInstructionExecutorTestFixture<SECImplied>;


static void SetupAffectedOrUsedRegisters(InstructionExecutorTestFixture &fixture, const SECImplied &instruction_param)
{
    fixture.r.SetFlag(FLAGS6502::C, instruction_param.requirements.initial.carry_flag);
}

template<>
void LoadInstructionIntoMemoryAndSetRegistersToInitialState(      InstructionExecutorTestFixture &fixture,
                                                            const SECImplied                    &instruction_param)
{
    SetupRAMForInstructionsThatHaveImpliedValue(fixture, instruction_param);
    SetupAffectedOrUsedRegisters(fixture, instruction_param);
}

template<>
void RegistersAreInExpectedState(const Registers &registers,
                                 const SEC_Implied_Expectations &expectations)
{
    EXPECT_THAT(registers.GetFlag(FLAGS6502::C), Eq(expectations.carry_flag));
}

template<>
void MemoryContainsInstruction(const InstructionExecutorTestFixture &fixture,
                               const Instruction<AbstractInstruction_e::SEC, Implied> &)
{
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter ), Eq( OpcodeFor(AbstractInstruction_e::SEC, AddressMode_e::Implied) ));
}

template<>
void MemoryContainsExpectedComputation(const InstructionExecutorTestFixture &,
                                       const SECImplied                     &)
{
    // No memory affected
}

template<>
void MemoryContainsExpectedResult(const InstructionExecutorTestFixture &,
                                  const SECImplied                     &)
{
    // No memory affected
}

static const std::vector<SECImplied> SECImpliedModeTestValues {
SECImplied{
    Implied().address(0x1000),
    SECImplied::Requirements{
        .initial = {
            .carry_flag = true
        },
        .final = {
            .carry_flag = true
        }}
},
SECImplied{
    Implied().address(0x10FF),
    SECImplied::Requirements{
        .initial = {
            .carry_flag = false
        },
        .final = {
            .carry_flag = true
        }}
},
SECImplied{
    Implied().address(0x1080),
    SECImplied::Requirements{
        .initial = {
            .carry_flag = true
        },
        .final = {
            .carry_flag = true
        }}
},
SECImplied{
    Implied().address(0x1070),
    SECImplied::Requirements{
        .initial = {
            .carry_flag = false
        },
        .final = {
            .carry_flag = true
        }}
}
};

TEST_P(SECImpliedMode, TypicalInstructionExecution)
{
    TypicalInstructionExecution(*this, GetParam());
}

INSTANTIATE_TEST_SUITE_P(SetCarryImpliedAtVariousAddresses,
                         SECImpliedMode,
                         testing::ValuesIn(SECImpliedModeTestValues) );
