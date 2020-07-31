#include <gmock/gmock.h>
#include "instruction_checks.hpp"



struct AND_ZeroPageXIndexed_Expectations
{
    uint8_t a;
    uint8_t x;
    NZFlags flags;

    uint8_t operand;
};

using ANDZeroPageXIndexed     = AND<ZeroPageXIndexed, AND_ZeroPageXIndexed_Expectations, 4>;
using ANDZeroPageXIndexedMode = ParameterizedInstructionExecutorTestFixture<ANDZeroPageXIndexed>;


template<>
void LoadInstructionIntoMemoryAndSetRegistersToInitialState(      InstructionExecutorTestFixture &fixture,
                                                            const ANDZeroPageXIndexed            &instruction_param)
{
    fixture.loadOpcodeIntoMemory(instruction_param.operation,
                                 AddressMode_e::ZeroPageXIndexed,
                                 instruction_param.address.instruction_address);
    fixture.fakeMemory[instruction_param.address.instruction_address + 1] = instruction_param.address.zero_page_address;

    // Load expected data into memory
    fixture.fakeMemory[ fixture.calculateZeroPageIndexedAddress(instruction_param.address.zero_page_address, instruction_param.requirements.initial.x) ] = instruction_param.requirements.initial.operand;

    // Load appropriate registers
    fixture.r.a = instruction_param.requirements.initial.a;
    fixture.r.x = instruction_param.requirements.initial.x;
    fixture.r.SetFlag(FLAGS6502::N, instruction_param.requirements.initial.flags.n_value.expected_value);
    fixture.r.SetFlag(FLAGS6502::Z, instruction_param.requirements.initial.flags.z_value.expected_value);
}

template<>
void RegistersAreInExpectedState(const Registers &registers,
                                 const AND_ZeroPageXIndexed_Expectations &expectations)
{
    EXPECT_THAT(registers.a, Eq(expectations.a));
    EXPECT_THAT(registers.x, Eq(expectations.x));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::N), Eq(expectations.flags.n_value.expected_value));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::Z), Eq(expectations.flags.z_value.expected_value));
}

template<>
void MemoryContainsInstruction(const InstructionExecutorTestFixture &fixture,
                               const Instruction<AbstractInstruction_e::AND, ZeroPageXIndexed> &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter ), Eq( OpcodeFor(AbstractInstruction_e::AND, AddressMode_e::ZeroPageXIndexed) ));
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 1), Eq(instruction.address.zero_page_address));
}

template<>
void MemoryContainsExpectedComputation(const InstructionExecutorTestFixture &fixture,
                                       const ANDZeroPageXIndexed            &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( fixture.calculateZeroPageIndexedAddress(instruction.address.zero_page_address, instruction.requirements.initial.x) ), Eq(instruction.requirements.initial.operand));
}


static const std::vector<ANDZeroPageXIndexed> ANDZeroPageXIndexedModeTestValues {
ANDZeroPageXIndexed{
    // Beginning of a page
    ZeroPageXIndexed().address(0x1000).zp_address(6),
    ANDZeroPageXIndexed::Requirements{
        .initial = {
            .a = 0,
            .x = 0,
            .flags = { },
            .operand = 0x00 },
        .final = {
            .a = 0,
            .x = 0,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true } },
            .operand = 0x00
        }}
},
ANDZeroPageXIndexed{
    // One before the end of a page
    ZeroPageXIndexed().address(0x10FE).zp_address(6),
    ANDZeroPageXIndexed::Requirements{
        .initial = {
            .a = 0,
            .x = 2,
            .flags = { },
            .operand = 0xFF },
        .final = {
            .a = 0,
            .x = 2,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true } },
            .operand = 0xFF
        }}
},
ANDZeroPageXIndexed{
    // Crossing a page boundary
    ZeroPageXIndexed().address(0x10FF).zp_address(6),
    ANDZeroPageXIndexed::Requirements{
        .initial = {
            .a = 0xFF,
            .x = 32,
            .flags = { },
            .operand = 0 },
        .final = {
            .a = 0,
            .x = 32,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true } },
            .operand = 0
        }}
},
ANDZeroPageXIndexed{
    ZeroPageXIndexed().address(0x8000).zp_address(16),
    ANDZeroPageXIndexed::Requirements{
        .initial = {
            .a = 0xFF,
            .x = 0,
            .flags = { },
            .operand = 0xFF },
        .final = {
            .a = 0xFF,
            .x = 0,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false } },
            .operand = 0xFF
        }}
},
ANDZeroPageXIndexed{
    // Check for masking out the high bit
    ZeroPageXIndexed().address(0x8000).zp_address(0xFF),
    ANDZeroPageXIndexed::Requirements{
        .initial = {
            .a = 0xFF,
            .x = 0,
            .flags = { },
            .operand = 0x80 },
        .final = {
            .a = 0x80,
            .x = 0,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false } },
            .operand = 0x80
        }}
},
ANDZeroPageXIndexed{
    // Use alternating bits for a zero result
    ZeroPageXIndexed().address(0x8000).zp_address(0xFF),
    ANDZeroPageXIndexed::Requirements{
        .initial = {
            .a = 0b10101010,
            .x = 16,
            .flags = { },
            .operand = 0b01010101 },
        .final = {
            .a = 0x00,
            .x = 16,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true } },
            .operand = 0b01010101
        }}
},
ANDZeroPageXIndexed{
    // Use the same bits for the same result
    ZeroPageXIndexed().address(0x8000).zp_address(0xFF),
    ANDZeroPageXIndexed::Requirements{
        .initial = {
            .a = 0b10101010,
            .x = 0xFF,
            .flags = { },
            .operand = 0b10101010 },
        .final = {
            .a = 0b10101010,
            .x = 0xFF,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false } },
            .operand = 0b10101010
        }}
},
ANDZeroPageXIndexed{
    // Use the same bits for the same result (not the same pattern as before)
    ZeroPageXIndexed().address(0x8000).zp_address(0xFF),
    ANDZeroPageXIndexed::Requirements{
        .initial = {
            .a = 0b01010101,
            .x = 0xFF,
            .flags = { },
            .operand = 0b01010101 },
        .final = {
            .a = 0b01010101,
            .x = 0xFF,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false } },
            .operand = 0b01010101
        }}
}
};

TEST_P(ANDZeroPageXIndexedMode, TypicalInstructionExecution)
{
    TypicalInstructionExecution(*this, GetParam());
}

INSTANTIATE_TEST_SUITE_P(AndZeroPageXIndexedAtVariousAddresses,
                         ANDZeroPageXIndexedMode,
                         testing::ValuesIn(ANDZeroPageXIndexedModeTestValues) );
