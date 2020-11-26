#include "addressing_mode_helpers.hpp"



struct TXA_Implied_Expectations
{
    uint8_t a;
    uint8_t x;
    NZFlags flags;
};

using TXAImplied     = TXA<Implied, TXA_Implied_Expectations, 2>;
using TXAImpliedMode = ParameterizedInstructionExecutorTestFixture<TXAImplied>;


static void SetupAffectedOrUsedRegisters(InstructionExecutorTestFixture &fixture, const TXAImplied &instruction_param)
{
    fixture.r.a = instruction_param.requirements.initial.a;
    fixture.r.x = instruction_param.requirements.initial.x;
    fixture.r.SetFlag(FLAGS6502::N, instruction_param.requirements.initial.flags.n_value.expected_value);
    fixture.r.SetFlag(FLAGS6502::Z, instruction_param.requirements.initial.flags.z_value.expected_value);
}

template<>
void LoadInstructionIntoMemoryAndSetRegistersToInitialState(      InstructionExecutorTestFixture &fixture,
                                                            const TXAImplied                    &instruction_param)
{
    SetupRAMForInstructionsThatHaveImpliedValue(fixture, instruction_param);
    SetupAffectedOrUsedRegisters(fixture, instruction_param);
}

template<>
void RegistersAreInExpectedState(const Registers &registers,
                                 const TXA_Implied_Expectations &expectations)
{
    EXPECT_THAT(registers.a, Eq(expectations.a));
    EXPECT_THAT(registers.x, Eq(expectations.x));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::N), Eq(expectations.flags.n_value.expected_value));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::Z), Eq(expectations.flags.z_value.expected_value));
}

template<>
void MemoryContainsInstruction(const InstructionExecutorTestFixture &fixture,
                               const Instruction<AbstractInstruction_e::TXA, Implied> &)
{
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter ), Eq( OpcodeFor(AbstractInstruction_e::TXA, AddressMode_e::Implied) ));
}

template<>
void MemoryContainsExpectedComputation(const InstructionExecutorTestFixture &,
                                       const TXAImplied                    &)
{
    // No memory affected
}


static const std::vector<TXAImplied> TXAImpliedModeTestValues {
TXAImplied{
    Implied().address(0x1000),
    TXAImplied::Requirements{
        .initial = {
            .a = 0xFF,
            .x = 0,
            .flags = {
                    .n_value = { .expected_value = false },
                    .z_value = { .expected_value = false } }
        },
        .final = {
            .a = 0,
            .x = 0,
            .flags = {
                    .n_value = { .expected_value = false },
                    .z_value = { .expected_value = true } }
        }}
},
TXAImplied{
    Implied().address(0x1000),
    TXAImplied::Requirements{
        .initial = {
            .a = 0x00,
            .x = 0xFF,
            .flags = {
                    .n_value = { .expected_value = false },
                    .z_value = { .expected_value = false } }
        },
        .final = {
            .a = 0xFF,
            .x = 0xFF,
            .flags = {
                    .n_value = { .expected_value = true },
                    .z_value = { .expected_value = false } }
        }}
}
};

TEST_P(TXAImpliedMode, TypicalInstructionExecution)
{
    TypicalInstructionExecution(*this, GetParam());
}

INSTANTIATE_TEST_SUITE_P(TXAImpliedAtVariousAddresses,
                         TXAImpliedMode,
                         testing::ValuesIn(TXAImpliedModeTestValues) );
