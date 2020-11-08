#include "addressing_mode_helpers.hpp"



struct SED_Implied_Expectations
{
    bool decimal_flag;
};

using SEDImplied     = SED<Implied, SED_Implied_Expectations, 2>;
using SEDImpliedMode = ParameterizedInstructionExecutorTestFixture<SEDImplied>;


static void SetupAffectedOrUsedRegisters(InstructionExecutorTestFixture &fixture, const SEDImplied &instruction_param)
{
    fixture.r.SetFlag(FLAGS6502::D, instruction_param.requirements.initial.decimal_flag);
}

template<>
void LoadInstructionIntoMemoryAndSetRegistersToInitialState(      InstructionExecutorTestFixture &fixture,
                                                            const SEDImplied                    &instruction_param)
{
    SetupRAMForInstructionsThatHaveImpliedValue(fixture, instruction_param);
    SetupAffectedOrUsedRegisters(fixture, instruction_param);
}

template<>
void RegistersAreInExpectedState(const Registers &registers,
                                 const SED_Implied_Expectations &expectations)
{
    EXPECT_THAT(registers.GetFlag(FLAGS6502::D), Eq(expectations.decimal_flag));
}

template<>
void MemoryContainsInstruction(const InstructionExecutorTestFixture &fixture,
                               const Instruction<AbstractInstruction_e::SED, Implied> &)
{
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter ), Eq( OpcodeFor(AbstractInstruction_e::SED, AddressMode_e::Implied) ));
}

template<>
void MemoryContainsExpectedComputation(const InstructionExecutorTestFixture &,
                                       const SEDImplied                     &)
{
    // No memory affected
}

template<>
void MemoryContainsExpectedResult(const InstructionExecutorTestFixture &,
                                  const SEDImplied                     &)
{
    // No memory affected
}

static const std::vector<SEDImplied> SEDImpliedModeTestValues {
SEDImplied{
    Implied().address(0x1000),
    SEDImplied::Requirements{
        .initial = {
            .decimal_flag = true
        },
        .final = {
            .decimal_flag = true
        }}
},
SEDImplied{
    Implied().address(0x10FF),
    SEDImplied::Requirements{
        .initial = {
            .decimal_flag = false
        },
        .final = {
            .decimal_flag = true
        }}
},
SEDImplied{
    Implied().address(0x1080),
    SEDImplied::Requirements{
        .initial = {
            .decimal_flag = true
        },
        .final = {
            .decimal_flag = true
        }}
},
SEDImplied{
    Implied().address(0x1070),
    SEDImplied::Requirements{
        .initial = {
            .decimal_flag = false
        },
        .final = {
            .decimal_flag = true
        }}
}
};

TEST_P(SEDImpliedMode, TypicalInstructionExecution)
{
    TypicalInstructionExecution(*this, GetParam());
}

INSTANTIATE_TEST_SUITE_P(SetDecimalImpliedAtVariousAddresses,
                         SEDImpliedMode,
                         testing::ValuesIn(SEDImpliedModeTestValues) );
