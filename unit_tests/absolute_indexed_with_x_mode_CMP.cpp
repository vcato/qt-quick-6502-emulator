#include <gmock/gmock.h>
#include "instruction_checks.hpp"



struct CMP_AbsoluteXIndexed_Expectations
{
    uint8_t  a;
    uint8_t  x;
    NZCFlags flags;

    uint8_t  operand;
};

using CMPAbsoluteXIndexed     = CMP<AbsoluteXIndexed, CMP_AbsoluteXIndexed_Expectations, 4>;
using CMPAbsoluteXIndexedMode = ParameterizedInstructionExecutorTestFixture<CMPAbsoluteXIndexed>;


template<>
void LoadInstructionIntoMemoryAndSetRegistersToInitialState(      InstructionExecutorTestFixture &fixture,
                                                            const CMPAbsoluteXIndexed            &instruction_param)
{
    fixture.loadOpcodeIntoMemory(instruction_param.operation,
                                 AddressMode_e::AbsoluteXIndexed,
                                 instruction_param.address.instruction_address);
    fixture.fakeMemory[instruction_param.address.instruction_address + 1] = fixture.loByteOf(instruction_param.address.absolute_address);
    fixture.fakeMemory[instruction_param.address.instruction_address + 2] = fixture.hiByteOf(instruction_param.address.absolute_address);
    fixture.fakeMemory[instruction_param.address.absolute_address + instruction_param.requirements.initial.x ] = instruction_param.requirements.initial.operand;

    // Load appropriate registers
    fixture.r.a = instruction_param.requirements.initial.a;
    fixture.r.x = instruction_param.requirements.initial.x;
    fixture.r.SetFlag(FLAGS6502::N, instruction_param.requirements.initial.flags.n_value.expected_value);
    fixture.r.SetFlag(FLAGS6502::Z, instruction_param.requirements.initial.flags.z_value.expected_value);
    fixture.r.SetFlag(FLAGS6502::C, instruction_param.requirements.initial.flags.c_value.expected_value);
}

template<>
void RegistersAreInExpectedState(const Registers &registers,
                                 const CMP_AbsoluteXIndexed_Expectations &expectations)
{
    EXPECT_THAT(registers.a, Eq(expectations.a));
    EXPECT_THAT(registers.x, Eq(expectations.x));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::N), Eq(expectations.flags.n_value.expected_value));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::Z), Eq(expectations.flags.z_value.expected_value));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::C), Eq(expectations.flags.c_value.expected_value));
}

template<>
void MemoryContainsInstruction(const InstructionExecutorTestFixture &fixture,
                               const Instruction<AbstractInstruction_e::CMP, AbsoluteXIndexed> &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter ), Eq( OpcodeFor(AbstractInstruction_e::CMP, AddressMode_e::AbsoluteXIndexed) ));

    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 1), Eq( fixture.loByteOf(instruction.address.absolute_address) ));
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 2), Eq( fixture.hiByteOf(instruction.address.absolute_address) ));
}

template<>
void MemoryContainsExpectedComputation(const InstructionExecutorTestFixture &fixture,
                                       const CMPAbsoluteXIndexed            &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( instruction.address.absolute_address + instruction.requirements.initial.x ), Eq( instruction.requirements.initial.operand ));
}

template<>
void InstructionExecutedInExpectedClockTicks(const InstructionExecutorTestFixture &fixture,
                                             const CMPAbsoluteXIndexed            &instruction)
{
    // Account for a clock tick one greater if a page is crossed
    uint32_t original_address  = fixture.loByteOf(instruction.address.absolute_address);
    uint32_t effective_address = original_address + instruction.requirements.initial.x;
    bool     page_boundary_is_crossed = effective_address > 0xFF;
    uint32_t extra_cycle_count = (page_boundary_is_crossed) ? 1 : 0; // If the page is crossed

    EXPECT_THAT(fixture.executor.clock_ticks, Eq(instruction.requirements.cycle_count + extra_cycle_count));
}


static const std::vector<CMPAbsoluteXIndexed> CMPAbsoluteXIndexedModeTestValues {
CMPAbsoluteXIndexed{
    // Beginning of a page
    // 0 - 0
    AbsoluteXIndexed().address(0xA000).value(0xB000),
    CMPAbsoluteXIndexed::Requirements{
        .initial = {
            .a = 0,
            .x = 0,
            .flags = { },
            .operand = 0 },
        .final = {
            .a = 0,
            .x = 0,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true },
                .c_value = { .expected_value = true } },
            .operand = 0
        }}
},
CMPAbsoluteXIndexed{
    // Middle of a page
    // 0 - 0
    AbsoluteXIndexed().address(0xA080).value(0xB080),
    CMPAbsoluteXIndexed::Requirements{
        .initial = {
            .a = 0,
            .x = 0,
            .flags = { },
            .operand = 0 },
        .final = {
            .a = 0,
            .x = 0,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true },
                .c_value = { .expected_value = true } },
            .operand = 0
        }}
},
CMPAbsoluteXIndexed{
    // End of a page
    // 0 - 0
    AbsoluteXIndexed().address(0xA0FD).value(0xB0FE),
    CMPAbsoluteXIndexed::Requirements{
        .initial = {
            .a = 0,
            .x = 0,
            .flags = { },
            .operand = 0 },
        .final = {
            .a = 0,
            .x = 0,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true },
                .c_value = { .expected_value = true } },
            .operand = 0
        }}
},
CMPAbsoluteXIndexed{
    // Crossing a page (partial parameter)
    // 0 - 0xFF
    AbsoluteXIndexed().address(0xA0FE).value(0xB0FE),
    CMPAbsoluteXIndexed::Requirements{
        .initial = {
            .a = 0,
            .x = 0,
            .flags = { },
            .operand = 0xFF },
        .final = {
            .a = 0,
            .x = 0,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false } },
            .operand = 0xFF
        }}
},
CMPAbsoluteXIndexed{
    // Crossing a page (entire parameter)
    // 0 - 0xFF
    AbsoluteXIndexed().address(0xA0FF).value(0xB0FE),
    CMPAbsoluteXIndexed::Requirements{
        .initial = {
            .a = 0,
            .x = 0x80,
            .flags = { },
            .operand = 0xFF },
        .final = {
            .a = 0,
            .x = 0x80,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false } },
            .operand = 0xFF
        }}
},
CMPAbsoluteXIndexed{
    // 2 - 3
    // Negative Result
    AbsoluteXIndexed().address(0xA000).value(0xB000),
    CMPAbsoluteXIndexed::Requirements{
        .initial = {
            .a = 2,
            .x = 0x10,
            .flags = { },
            .operand = 3 },
        .final = {
            .a = 2,
            .x = 0x10,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false } },
            .operand = 3
        }}
},
CMPAbsoluteXIndexed{
    // 3 - 2
    // Positive Result
    AbsoluteXIndexed().address(0xA000).value(0xB000),
    CMPAbsoluteXIndexed::Requirements{
        .initial = {
            .a = 3,
            .x = 0x10,
            .flags = { },
            .operand = 2 },
        .final = {
            .a = 3,
            .x = 0x10,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true } },
            .operand = 2
        }}
}
};

TEST_P(CMPAbsoluteXIndexedMode, TypicalInstructionExecution)
{
    TypicalInstructionExecution(*this, GetParam());
}

INSTANTIATE_TEST_SUITE_P(CompareAccumulatorAbsoluteXIndexedAtVariousAddresses,
                         CMPAbsoluteXIndexedMode,
                         testing::ValuesIn(CMPAbsoluteXIndexedModeTestValues) );
