#include "addressing_mode_helpers.hpp"



struct SEI_Implied_Expectations
{
    bool interrupt_flag;
};

using SEIImplied     = SEI<Implied, SEI_Implied_Expectations, 2>;
using SEIImpliedMode = ParameterizedInstructionExecutorTestFixture<SEIImplied>;


static void SetupAffectedOrUsedRegisters(InstructionExecutorTestFixture &fixture, const SEIImplied &instruction_param)
{
    fixture.r.SetFlag(FLAGS6502::I, instruction_param.requirements.initial.interrupt_flag);
}

template<>
void LoadInstructionIntoMemoryAndSetRegistersToInitialState(      InstructionExecutorTestFixture &fixture,
                                                            const SEIImplied                    &instruction_param)
{
    SetupRAMForInstructionsThatHaveImpliedValue(fixture, instruction_param);
    SetupAffectedOrUsedRegisters(fixture, instruction_param);
}

template<>
void RegistersAreInExpectedState(const Registers &registers,
                                 const SEI_Implied_Expectations &expectations)
{
    EXPECT_THAT(registers.GetFlag(FLAGS6502::I), Eq(expectations.interrupt_flag));
}

template<>
void MemoryContainsInstruction(const InstructionExecutorTestFixture &fixture,
                               const Instruction<AbstractInstruction_e::SEI, Implied> &)
{
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter ), Eq( OpcodeFor(AbstractInstruction_e::SEI, AddressMode_e::Implied) ));
}

template<>
void MemoryContainsExpectedComputation(const InstructionExecutorTestFixture &,
                                       const SEIImplied                     &)
{
    // No memory affected
}

template<>
void MemoryContainsExpectedResult(const InstructionExecutorTestFixture &,
                                  const SEIImplied                     &)
{
    // No memory affected
}

static const std::vector<SEIImplied> SEIImpliedModeTestValues {
SEIImplied{
    Implied().address(0x1000),
    SEIImplied::Requirements{
        .initial = {
            .interrupt_flag = true
        },
        .final = {
            .interrupt_flag = true
        }}
},
SEIImplied{
    Implied().address(0x10FF),
    SEIImplied::Requirements{
        .initial = {
            .interrupt_flag = false
        },
        .final = {
            .interrupt_flag = true
        }}
},
SEIImplied{
    Implied().address(0x1080),
    SEIImplied::Requirements{
        .initial = {
            .interrupt_flag = true
        },
        .final = {
            .interrupt_flag = true
        }}
},
SEIImplied{
    Implied().address(0x1070),
    SEIImplied::Requirements{
        .initial = {
            .interrupt_flag = false
        },
        .final = {
            .interrupt_flag = true
        }}
}
};

TEST_P(SEIImpliedMode, TypicalInstructionExecution)
{
    TypicalInstructionExecution(*this, GetParam());
}

INSTANTIATE_TEST_SUITE_P(SetInterruptImpliedAtVariousAddresses,
                         SEIImpliedMode,
                         testing::ValuesIn(SEIImpliedModeTestValues) );
