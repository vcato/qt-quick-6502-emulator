#include "addressing_mode_helpers.hpp"



struct TSX_Implied_Expectations
{
    uint8_t s;
    uint8_t x;
    NZFlags flags;
};

using TSXImplied     = TSX<Implied, TSX_Implied_Expectations, 2>;
using TSXImpliedMode = ParameterizedInstructionExecutorTestFixture<TSXImplied>;


static void SetupAffectedOrUsedRegisters(InstructionExecutorTestFixture &fixture, const TSXImplied &instruction_param)
{
    fixture.r.stack_pointer = instruction_param.requirements.initial.s;
    fixture.r.x = instruction_param.requirements.initial.x;
    fixture.r.SetFlag(FLAGS6502::N, instruction_param.requirements.initial.flags.n_value.expected_value);
    fixture.r.SetFlag(FLAGS6502::Z, instruction_param.requirements.initial.flags.z_value.expected_value);
}

template<>
void LoadInstructionIntoMemoryAndSetRegistersToInitialState(      InstructionExecutorTestFixture &fixture,
                                                            const TSXImplied                    &instruction_param)
{
    SetupRAMForInstructionsThatHaveImpliedValue(fixture, instruction_param);
    SetupAffectedOrUsedRegisters(fixture, instruction_param);
}

template<>
void RegistersAreInExpectedState(const Registers &registers,
                                 const TSX_Implied_Expectations &expectations)
{
    EXPECT_THAT(registers.stack_pointer, Eq(expectations.s));
    EXPECT_THAT(registers.x, Eq(expectations.x));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::N), Eq(expectations.flags.n_value.expected_value));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::Z), Eq(expectations.flags.z_value.expected_value));
}

template<>
void MemoryContainsInstruction(const InstructionExecutorTestFixture &fixture,
                               const Instruction<AbstractInstruction_e::TSX, Implied> &)
{
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter ), Eq( OpcodeFor(AbstractInstruction_e::TSX, AddressMode_e::Implied) ));
}

template<>
void MemoryContainsExpectedComputation(const InstructionExecutorTestFixture &,
                                       const TSXImplied                    &)
{
    // No memory affected
}


static const std::vector<TSXImplied> TSXImpliedModeTestValues {
TSXImplied{
    Implied().address(0x1000),
    TSXImplied::Requirements{
        .initial = {
            .s = 0,
            .x = 0xFF,
            .flags = {
                    .n_value = { .expected_value = false },
                    .z_value = { .expected_value = false } }
        },
        .final = {
            .s = 0,
            .x = 0,
            .flags = {
                    .n_value = { .expected_value = false },
                    .z_value = { .expected_value = true } }
        }}
},
TSXImplied{
    Implied().address(0x1000),
    TSXImplied::Requirements{
        .initial = {
            .s = 0xFF,
            .x = 0x00,
            .flags = {
                    .n_value = { .expected_value = false },
                    .z_value = { .expected_value = false } }
        },
        .final = {
            .s = 0xFF,
            .x = 0xFF,
            .flags = {
                    .n_value = { .expected_value = true },
                    .z_value = { .expected_value = false } }
        }}
}
};

TEST_P(TSXImpliedMode, TypicalInstructionExecution)
{
    TypicalInstructionExecution(*this, GetParam());
}

INSTANTIATE_TEST_SUITE_P(TSXImpliedAtVariousAddresses,
                         TSXImpliedMode,
                         testing::ValuesIn(TSXImpliedModeTestValues) );
