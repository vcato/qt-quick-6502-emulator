#include <gmock/gmock.h>
#include "instruction_checks.hpp"



struct INY_Implied_Expectations
{
    uint8_t y;
    NZFlags flags;
};

using INYImplied     = INY<Implied, INY_Implied_Expectations, 2>;
using INYImpliedMode = ParameterizedInstructionExecutorTestFixture<INYImplied>;


template<>
void LoadInstructionIntoMemoryAndSetRegistersToInitialState(      InstructionExecutorTestFixture &fixture,
                                                            const INYImplied                    &instruction_param)
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
                                 const INY_Implied_Expectations &expectations)
{
    EXPECT_THAT(registers.y, Eq(expectations.y));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::N), Eq(expectations.flags.n_value.expected_value));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::Z), Eq(expectations.flags.z_value.expected_value));
}

template<>
void MemoryContainsInstruction(const InstructionExecutorTestFixture &fixture,
                               const Instruction<AbstractInstruction_e::INY, Implied> &)
{
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter ), Eq( OpcodeFor(AbstractInstruction_e::INY, AddressMode_e::Implied) ));
}

template<>
void MemoryContainsExpectedComputation(const InstructionExecutorTestFixture &,
                                       const INYImplied                     &)
{
    // No memory affected
}

template<>
void MemoryContainsExpectedResult(const InstructionExecutorTestFixture &,
                                  const INYImplied                     &)
{
    // No memory affected
}

static const std::vector<INYImplied> INYImpliedModeTestValues {
INYImplied{
    Implied().address(0x1000),
    INYImplied::Requirements{
        .initial = {
            .y = 0,
            .flags = {
                .n_value = {
                    .status_flag = FLAGS6502::N,
                    .expected_value = true },
                .z_value = {
                    .status_flag = FLAGS6502::Z,
                    .expected_value = true } },
        },
        .final = {
            .y = 1,
            .flags = {
                .n_value = {
                    .status_flag = FLAGS6502::N,
                    .expected_value = false },
                .z_value = {
                    .status_flag = FLAGS6502::Z,
                    .expected_value = false } },
        }}
},
INYImplied{
    Implied().address(0x10FF),
    INYImplied::Requirements{
        .initial = {
            .y = 0xFF,
            .flags = {
                .n_value = {
                    .status_flag = FLAGS6502::N,
                    .expected_value = true },
                .z_value = {
                    .status_flag = FLAGS6502::Z,
                    .expected_value = false } },
        },
        .final = {
            .y = 0x00,
            .flags = {
                .n_value = {
                    .status_flag = FLAGS6502::N,
                    .expected_value = false },
                .z_value = {
                    .status_flag = FLAGS6502::Z,
                    .expected_value = true } },
        }}
},
INYImplied{
    Implied().address(0x1080),
    INYImplied::Requirements{
        .initial = {
            .y = 0x7F,
            .flags = {
                .n_value = {
                    .status_flag = FLAGS6502::N,
                    .expected_value = false },
                .z_value = {
                    .status_flag = FLAGS6502::Z,
                    .expected_value = true } },
        },
        .final = {
            .y = 0x80,
            .flags = {
                .n_value = {
                    .status_flag = FLAGS6502::N,
                    .expected_value = true },
                .z_value = {
                    .status_flag = FLAGS6502::Z,
                    .expected_value = false } },
        }}
},
INYImplied{
    Implied().address(0x1070),
    INYImplied::Requirements{
        .initial = {
            .y = 0xFE,
            .flags = {
                .n_value = {
                    .status_flag = FLAGS6502::N,
                    .expected_value = false },
                .z_value = {
                    .status_flag = FLAGS6502::Z,
                    .expected_value = true } },
        },
        .final = {
            .y = 0xFF,
            .flags = {
                .n_value = {
                    .status_flag = FLAGS6502::N,
                    .expected_value = true },
                .z_value = {
                    .status_flag = FLAGS6502::Z,
                    .expected_value = false } },
        }}
}
};

TEST_P(INYImpliedMode, TypicalInstructionExecution)
{
    TypicalInstructionExecution(*this, GetParam());
}

INSTANTIATE_TEST_SUITE_P(IncrementYImpliedAtVariousAddresses,
                         INYImpliedMode,
                         testing::ValuesIn(INYImpliedModeTestValues) );
