#include "addressing_mode_helpers.hpp"



struct INX_Implied_Expectations
{
    uint8_t x;
    NZFlags flags;
};

using INXImplied     = INX<Implied, INX_Implied_Expectations, 2>;
using INXImpliedMode = ParameterizedInstructionExecutorTestFixture<INXImplied>;


static void SetupAffectedOrUsedRegisters(InstructionExecutorTestFixture &fixture, const INXImplied &instruction_param)
{
    fixture.r.x = instruction_param.requirements.initial.x;
    fixture.r.SetFlag(FLAGS6502::N, instruction_param.requirements.initial.flags.n_value.expected_value);
    fixture.r.SetFlag(FLAGS6502::Z, instruction_param.requirements.initial.flags.z_value.expected_value);
}

template<>
void LoadInstructionIntoMemoryAndSetRegistersToInitialState(      InstructionExecutorTestFixture &fixture,
                                                            const INXImplied                    &instruction_param)
{
    SetupRAMForInstructionsThatHaveImpliedValue(fixture, instruction_param);
    SetupAffectedOrUsedRegisters(fixture, instruction_param);
}

template<>
void RegistersAreInExpectedState(const Registers &registers,
                                 const INX_Implied_Expectations &expectations)
{
    EXPECT_THAT(registers.x, Eq(expectations.x));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::N), Eq(expectations.flags.n_value.expected_value));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::Z), Eq(expectations.flags.z_value.expected_value));
}

template<>
void MemoryContainsInstruction(const InstructionExecutorTestFixture &fixture,
                               const Instruction<AbstractInstruction_e::INX, Implied> &)
{
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter ), Eq( OpcodeFor(AbstractInstruction_e::INX, AddressMode_e::Implied) ));
}

template<>
void MemoryContainsExpectedComputation(const InstructionExecutorTestFixture &,
                                       const INXImplied                     &)
{
    // No memory affected
}

template<>
void MemoryContainsExpectedResult(const InstructionExecutorTestFixture &,
                                  const INXImplied                     &)
{
    // No memory affected
}

static const std::vector<INXImplied> INXImpliedModeTestValues {
INXImplied{
    Implied().address(0x1000),
    INXImplied::Requirements{
        .initial = {
            .x = 0,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = true } },
        },
        .final = {
            .x = 1,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false } },
        }}
},
INXImplied{
    Implied().address(0x10FF),
    INXImplied::Requirements{
        .initial = {
            .x = 0xFF,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false } },
        },
        .final = {
            .x = 0x00,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true } },
        }}
},
INXImplied{
    Implied().address(0x1080),
    INXImplied::Requirements{
        .initial = {
            .x = 0x7F,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true } },
        },
        .final = {
            .x = 0x80,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false } },
        }}
},
INXImplied{
    Implied().address(0x1070),
    INXImplied::Requirements{
        .initial = {
            .x = 0xFE,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true } },
        },
        .final = {
            .x = 0xFF,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false } },
        }}
}
};

TEST_P(INXImpliedMode, TypicalInstructionExecution)
{
    TypicalInstructionExecution(*this, GetParam());
}

INSTANTIATE_TEST_SUITE_P(IncrementXImpliedAtVariousAddresses,
                         INXImpliedMode,
                         testing::ValuesIn(INXImpliedModeTestValues) );
