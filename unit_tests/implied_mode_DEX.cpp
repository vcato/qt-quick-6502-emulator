#include <gmock/gmock.h>
#include "instruction_checks.hpp"



struct DEX_Implied_Expectations
{
    uint8_t x;
    NZFlags flags;
};

using DEXImplied     = DEX<Implied, DEX_Implied_Expectations, 2>;
using DEXImpliedMode = ParameterizedInstructionExecutorTestFixture<DEXImplied>;


template<>
void LoadInstructionIntoMemoryAndSetRegistersToInitialState(      InstructionExecutorTestFixture &fixture,
                                                            const DEXImplied                    &instruction_param)
{
    fixture.loadOpcodeIntoMemory(instruction_param.operation,
                                 AddressMode_e::Implied,
                                 instruction_param.address.instruction_address);

    // Load appropriate register
    fixture.r.x = instruction_param.requirements.initial.x;
    fixture.r.SetFlag(FLAGS6502::N, instruction_param.requirements.initial.flags.n_value.expected_value);
    fixture.r.SetFlag(FLAGS6502::Z, instruction_param.requirements.initial.flags.z_value.expected_value);
}

template<>
void RegistersAreInExpectedState(const Registers &registers,
                                 const DEX_Implied_Expectations &expectations)
{
    EXPECT_THAT(registers.x, Eq(expectations.x));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::N), Eq(expectations.flags.n_value.expected_value));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::Z), Eq(expectations.flags.z_value.expected_value));
}

template<>
void MemoryContainsInstruction(const InstructionExecutorTestFixture &fixture,
                               const Instruction<AbstractInstruction_e::DEX, Implied> &)
{
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter ), Eq( OpcodeFor(AbstractInstruction_e::DEX, AddressMode_e::Implied) ));
}

template<>
void MemoryContainsExpectedComputation(const InstructionExecutorTestFixture &,
                                       const DEXImplied                     &)
{
    // No memory affected
}

template<>
void MemoryContainsExpectedResult(const InstructionExecutorTestFixture &,
                                  const DEXImplied                     &)
{
    // No memory affected
}

static const std::vector<DEXImplied> DEXImpliedModeTestValues {
DEXImplied{
    Implied().address(0x1000),
    DEXImplied::Requirements{
        .initial = {
            .x = 1,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false } },
        },
        .final = {
            .x = 0,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true } },
        }}
},
DEXImplied{
    Implied().address(0x10FF),
    DEXImplied::Requirements{
        .initial = {
            .x = 0,
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
},
DEXImplied{
    Implied().address(0x1080),
    DEXImplied::Requirements{
        .initial = {
            .x = 0x80,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = true } },
        },
        .final = {
            .x = 0x7F,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false } },
        }}
},
DEXImplied{
    Implied().address(0x1070),
    DEXImplied::Requirements{
        .initial = {
            .x = 0xFF,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true } },
        },
        .final = {
            .x = 0xFE,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false } },
        }}
}
};

TEST_P(DEXImpliedMode, TypicalInstructionExecution)
{
    TypicalInstructionExecution(*this, GetParam());
}

INSTANTIATE_TEST_SUITE_P(DecrementXImpliedAtVariousAddresses,
                         DEXImpliedMode,
                         testing::ValuesIn(DEXImpliedModeTestValues) );
