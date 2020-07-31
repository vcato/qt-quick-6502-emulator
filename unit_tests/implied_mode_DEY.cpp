#include <gmock/gmock.h>
#include "instruction_checks.hpp"



struct DEY_Implied_Expectations
{
    uint8_t y;
    NZFlags flags;
};

using DEYImplied     = DEY<Implied, DEY_Implied_Expectations, 2>;
using DEYImpliedMode = ParameterizedInstructionExecutorTestFixture<DEYImplied>;


template<>
void LoadInstructionIntoMemoryAndSetRegistersToInitialState(      InstructionExecutorTestFixture &fixture,
                                                            const DEYImplied                    &instruction_param)
{
    fixture.loadOpcodeIntoMemory(instruction_param.operation,
                                 AddressMode_e::Implied,
                                 instruction_param.address.instruction_address);

    // Load appropriate registers
    fixture.r.y = instruction_param.requirements.initial.y;
    fixture.r.SetFlag(FLAGS6502::N, instruction_param.requirements.initial.flags.n_value.expected_value);
    fixture.r.SetFlag(FLAGS6502::Z, instruction_param.requirements.initial.flags.z_value.expected_value);
}

template<>
void RegistersAreInExpectedState(const Registers &registers,
                                 const DEY_Implied_Expectations &expectations)
{
    EXPECT_THAT(registers.y, Eq(expectations.y));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::N), Eq(expectations.flags.n_value.expected_value));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::Z), Eq(expectations.flags.z_value.expected_value));
}

template<>
void MemoryContainsInstruction(const InstructionExecutorTestFixture &fixture,
                               const Instruction<AbstractInstruction_e::DEY, Implied> &)
{
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter ), Eq( OpcodeFor(AbstractInstruction_e::DEY, AddressMode_e::Implied) ));
}

template<>
void MemoryContainsExpectedComputation(const InstructionExecutorTestFixture &,
                                       const DEYImplied                     &)
{
    // No memory affected
}

template<>
void MemoryContainsExpectedResult(const InstructionExecutorTestFixture &,
                                  const DEYImplied                     &)
{
    // No memory affected
}

static const std::vector<DEYImplied> DEYImpliedModeTestValues {
DEYImplied{
    Implied().address(0x1000),
    DEYImplied::Requirements{
        .initial = {
            .y = 1,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false } },
        },
        .final = {
            .y = 0,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true } },
        }}
},
DEYImplied{
    Implied().address(0x10FF),
    DEYImplied::Requirements{
        .initial = {
            .y = 0,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true } },
        },
        .final = {
            .y = 0xFF,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false } },
        }}
},
DEYImplied{
    Implied().address(0x1080),
    DEYImplied::Requirements{
        .initial = {
            .y = 0x80,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = true } },
        },
        .final = {
            .y = 0x7F,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false } },
        }}
},
DEYImplied{
    Implied().address(0x1070),
    DEYImplied::Requirements{
        .initial = {
            .y = 0xFF,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true } },
        },
        .final = {
            .y = 0xFE,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false } },
        }}
}
};

TEST_P(DEYImpliedMode, TypicalInstructionExecution)
{
    TypicalInstructionExecution(*this, GetParam());
}

INSTANTIATE_TEST_SUITE_P(DecrementYImpliedAtVariousAddresses,
                         DEYImpliedMode,
                         testing::ValuesIn(DEYImpliedModeTestValues) );
