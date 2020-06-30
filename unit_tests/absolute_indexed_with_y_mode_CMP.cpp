#include <gmock/gmock.h>
#include "instruction_checks.hpp"



struct CMP_AbsoluteYIndexed_Expectations
{
    uint8_t  a;
    uint8_t  y;
    NZCFlags flags;

    uint8_t  operand;
};

using CMPAbsoluteYIndexed     = CMP<AbsoluteYIndexed, CMP_AbsoluteYIndexed_Expectations, 4>;
using CMPAbsoluteYIndexedMode = ParameterizedInstructionExecutorTestFixture<CMPAbsoluteYIndexed>;


template<>
void LoadInstructionIntoMemoryAndSetRegistersToInitialState(      InstructionExecutorTestFixture &fixture,
                                                            const CMPAbsoluteYIndexed            &instruction_param)
{
    fixture.loadOpcodeIntoMemory(instruction_param.operation,
                                 AddressMode_e::AbsoluteYIndexed,
                                 instruction_param.address.instruction_address);
    fixture.fakeMemory[instruction_param.address.instruction_address + 1] = fixture.loByteOf(instruction_param.address.absolute_address);
    fixture.fakeMemory[instruction_param.address.instruction_address + 2] = fixture.hiByteOf(instruction_param.address.absolute_address);
    fixture.fakeMemory[instruction_param.address.absolute_address + instruction_param.requirements.initial.y ] = instruction_param.requirements.initial.operand;

    // Load appropriate registers
    fixture.r.a = instruction_param.requirements.initial.a;
    fixture.r.y = instruction_param.requirements.initial.y;
    fixture.r.SetFlag(FLAGS6502::N, instruction_param.requirements.initial.flags.n_value.expected_value);
    fixture.r.SetFlag(FLAGS6502::Z, instruction_param.requirements.initial.flags.z_value.expected_value);
    fixture.r.SetFlag(FLAGS6502::C, instruction_param.requirements.initial.flags.c_value.expected_value);
}

template<>
void RegistersAreInExpectedState(const Registers &registers,
                                 const CMP_AbsoluteYIndexed_Expectations &expectations)
{
    EXPECT_THAT(registers.a, Eq(expectations.a));
    EXPECT_THAT(registers.y, Eq(expectations.y));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::N), Eq(expectations.flags.n_value.expected_value));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::Z), Eq(expectations.flags.z_value.expected_value));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::C), Eq(expectations.flags.c_value.expected_value));
}

template<>
void MemoryContainsInstruction(const InstructionExecutorTestFixture &fixture,
                               const Instruction<AbstractInstruction_e::CMP, AbsoluteYIndexed> &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter ), Eq( OpcodeFor(AbstractInstruction_e::CMP, AddressMode_e::AbsoluteYIndexed) ));

    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 1), Eq( fixture.loByteOf(instruction.address.absolute_address) ));
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 2), Eq( fixture.hiByteOf(instruction.address.absolute_address) ));
}

template<>
void MemoryContainsExpectedComputation(const InstructionExecutorTestFixture &fixture,
                                       const CMPAbsoluteYIndexed            &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( instruction.address.absolute_address + instruction.requirements.initial.y ), Eq( instruction.requirements.initial.operand ));
}

template<>
void InstructionExecutedInExpectedClockTicks(const InstructionExecutorTestFixture &fixture,
                                             const CMPAbsoluteYIndexed            &instruction)
{
    // Account for a clock tick one greater if a page is crossed
    uint32_t original_address  = fixture.loByteOf(instruction.address.absolute_address);
    uint32_t effective_address = original_address + instruction.requirements.initial.y;
    bool     page_boundary_is_crossed = effective_address > 0xFF;
    uint32_t extra_cycle_count = (page_boundary_is_crossed) ? 1 : 0; // If the page is crossed

    EXPECT_THAT(fixture.executor.clock_ticks, Eq(instruction.requirements.cycle_count + extra_cycle_count));
}


static const std::vector<CMPAbsoluteYIndexed> CMPAbsoluteYIndexedModeTestValues {
CMPAbsoluteYIndexed{
    // Beginning of a page
    // 0 - 0
    AbsoluteYIndexed().address(0xA000).value(0xB000),
    CMPAbsoluteYIndexed::Requirements{
        .initial = {
            .a = 0,
            .y = 0,
            .flags = { },
            .operand = 0 },
        .final = {
            .a = 0,
            .y = 0,
            .flags = {
                .n_value = {
                    .status_flag = FLAGS6502::N,
                    .expected_value = false },
                .z_value = {
                    .status_flag = FLAGS6502::Z,
                    .expected_value = true },
                .c_value = {
                    .status_flag = FLAGS6502::C,
                    .expected_value = true } },
            .operand = 0
        }}
},
CMPAbsoluteYIndexed{
    // Middle of a page
    // 0 - 0
    AbsoluteYIndexed().address(0xA080).value(0xB080),
    CMPAbsoluteYIndexed::Requirements{
        .initial = {
            .a = 0,
            .y = 0,
            .flags = { },
            .operand = 0 },
        .final = {
            .a = 0,
            .y = 0,
            .flags = {
                .n_value = {
                    .status_flag = FLAGS6502::N,
                    .expected_value = false },
                .z_value = {
                    .status_flag = FLAGS6502::Z,
                    .expected_value = true },
                .c_value = {
                    .status_flag = FLAGS6502::C,
                    .expected_value = true } },
            .operand = 0
        }}
},
CMPAbsoluteYIndexed{
    // End of a page
    // 0 - 0
    AbsoluteYIndexed().address(0xA0FD).value(0xB0FE),
    CMPAbsoluteYIndexed::Requirements{
        .initial = {
            .a = 0,
            .y = 0,
            .flags = { },
            .operand = 0 },
        .final = {
            .a = 0,
            .y = 0,
            .flags = {
                .n_value = {
                    .status_flag = FLAGS6502::N,
                    .expected_value = false },
                .z_value = {
                    .status_flag = FLAGS6502::Z,
                    .expected_value = true },
                .c_value = {
                    .status_flag = FLAGS6502::C,
                    .expected_value = true } },
            .operand = 0
        }}
},
CMPAbsoluteYIndexed{
    // Crossing a page (partial parameter)
    // 0 - 0xFF
    AbsoluteYIndexed().address(0xA0FE).value(0xB0FE),
    CMPAbsoluteYIndexed::Requirements{
        .initial = {
            .a = 0,
            .y = 0,
            .flags = { },
            .operand = 0xFF },
        .final = {
            .a = 0,
            .y = 0,
            .flags = {
                .n_value = {
                    .status_flag = FLAGS6502::N,
                    .expected_value = false },
                .z_value = {
                    .status_flag = FLAGS6502::Z,
                    .expected_value = false },
                .c_value = {
                    .status_flag = FLAGS6502::C,
                    .expected_value = false } },
            .operand = 0xFF
        }}
},
CMPAbsoluteYIndexed{
    // Crossing a page (entire parameter)
    // 0 - 0xFF
    AbsoluteYIndexed().address(0xA0FF).value(0xB0FE),
    CMPAbsoluteYIndexed::Requirements{
        .initial = {
            .a = 0,
            .y = 0x80,
            .flags = { },
            .operand = 0xFF },
        .final = {
            .a = 0,
            .y = 0x80,
            .flags = {
                .n_value = {
                    .status_flag = FLAGS6502::N,
                    .expected_value = false },
                .z_value = {
                    .status_flag = FLAGS6502::Z,
                    .expected_value = false },
                .c_value = {
                    .status_flag = FLAGS6502::C,
                    .expected_value = false } },
            .operand = 0xFF
        }}
},
CMPAbsoluteYIndexed{
    // 2 - 3
    // Negative Result
    AbsoluteYIndexed().address(0xA000).value(0xB000),
    CMPAbsoluteYIndexed::Requirements{
        .initial = {
            .a = 2,
            .y = 0x10,
            .flags = { },
            .operand = 3 },
        .final = {
            .a = 2,
            .y = 0x10,
            .flags = {
                .n_value = {
                    .status_flag = FLAGS6502::N,
                    .expected_value = true },
                .z_value = {
                    .status_flag = FLAGS6502::Z,
                    .expected_value = false },
                .c_value = {
                    .status_flag = FLAGS6502::C,
                    .expected_value = false } },
            .operand = 3
        }}
},
CMPAbsoluteYIndexed{
    // 3 - 2
    // Positive Result
    AbsoluteYIndexed().address(0xA000).value(0xB000),
    CMPAbsoluteYIndexed::Requirements{
        .initial = {
            .a = 3,
            .y = 0x10,
            .flags = { },
            .operand = 2 },
        .final = {
            .a = 3,
            .y = 0x10,
            .flags = {
                .n_value = {
                    .status_flag = FLAGS6502::N,
                    .expected_value = false },
                .z_value = {
                    .status_flag = FLAGS6502::Z,
                    .expected_value = false },
                .c_value = {
                    .status_flag = FLAGS6502::C,
                    .expected_value = true } },
            .operand = 2
        }}
}
};

TEST_P(CMPAbsoluteYIndexedMode, TypicalInstructionExecution)
{
    TypicalInstructionExecution(*this, GetParam());
}

INSTANTIATE_TEST_SUITE_P(CompareAccumulatorAbsoluteYIndexedAtVariousAddresses,
                         CMPAbsoluteYIndexedMode,
                         testing::ValuesIn(CMPAbsoluteYIndexedModeTestValues) );
