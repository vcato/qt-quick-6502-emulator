#include "addressing_mode_helpers.hpp"



struct TAY_Implied_Expectations
{
    uint8_t a;
    uint8_t y;
    NZFlags flags;
};

using TAYImplied     = TAY<Implied, TAY_Implied_Expectations, 2>;
using TAYImpliedMode = ParameterizedInstructionExecutorTestFixture<TAYImplied>;


static void SetupAffectedOrUsedRegisters(InstructionExecutorTestFixture &fixture, const TAYImplied &instruction_param)
{
    fixture.r.a = instruction_param.requirements.initial.a;
    fixture.r.y = instruction_param.requirements.initial.y;
    fixture.r.SetFlag(FLAGS6502::N, instruction_param.requirements.initial.flags.n_value.expected_value);
    fixture.r.SetFlag(FLAGS6502::Z, instruction_param.requirements.initial.flags.z_value.expected_value);
}

template<>
void LoadInstructionIntoMemoryAndSetRegistersToInitialState(      InstructionExecutorTestFixture &fixture,
                                                            const TAYImplied                    &instruction_param)
{
    SetupRAMForInstructionsThatHaveImpliedValue(fixture, instruction_param);
    SetupAffectedOrUsedRegisters(fixture, instruction_param);
}

template<>
void RegistersAreInExpectedState(const Registers &registers,
                                 const TAY_Implied_Expectations &expectations)
{
    EXPECT_THAT(registers.a, Eq(expectations.a));
    EXPECT_THAT(registers.y, Eq(expectations.y));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::N), Eq(expectations.flags.n_value.expected_value));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::Z), Eq(expectations.flags.z_value.expected_value));
}

template<>
void MemoryContainsInstruction(const InstructionExecutorTestFixture &fixture,
                               const Instruction<AbstractInstruction_e::TAY, Implied> &)
{
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter ), Eq( OpcodeFor(AbstractInstruction_e::TAY, AddressMode_e::Implied) ));
}

template<>
void MemoryContainsExpectedComputation(const InstructionExecutorTestFixture &,
                                       const TAYImplied                    &)
{
    // No memory affected
}


static const std::vector<TAYImplied> TAYImpliedModeTestValues {
TAYImplied{
    Implied().address(0x1000),
    TAYImplied::Requirements{
        .initial = {
            .a = 0,
            .y = 0xFF,
            .flags = {
                    .n_value = { .expected_value = false },
                    .z_value = { .expected_value = false } }
        },
        .final = {
            .a = 0,
            .y = 0,
            .flags = {
                    .n_value = { .expected_value = false },
                    .z_value = { .expected_value = true } }
        }}
},
TAYImplied{
    Implied().address(0x1000),
    TAYImplied::Requirements{
        .initial = {
            .a = 0xFF,
            .y = 0x00,
            .flags = {
                    .n_value = { .expected_value = false },
                    .z_value = { .expected_value = false } }
        },
        .final = {
            .a = 0xFF,
            .y = 0xFF,
            .flags = {
                    .n_value = { .expected_value = true },
                    .z_value = { .expected_value = false } }
        }}
}
};

TEST_P(TAYImpliedMode, TypicalInstructionExecution)
{
    TypicalInstructionExecution(*this, GetParam());
}

INSTANTIATE_TEST_SUITE_P(TAYImpliedAtVariousAddresses,
                         TAYImpliedMode,
                         testing::ValuesIn(TAYImpliedModeTestValues) );
