#include <gmock/gmock.h>
#include "instruction_checks.hpp"



struct EOR_ZeroPageXIndexed_Expectations
{
    uint8_t a;
    uint8_t x;
    NZFlags flags;

    uint8_t operand;
};

using EORZeroPageXIndexed     = EOR<ZeroPageXIndexed, EOR_ZeroPageXIndexed_Expectations, 4>;
using EORZeroPageXIndexedMode = ParameterizedInstructionExecutorTestFixture<EORZeroPageXIndexed>;


template<>
void LoadInstructionIntoMemoryAndSetRegistersToInitialState(      InstructionExecutorTestFixture &fixture,
                                                            const EORZeroPageXIndexed            &instruction_param)
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
                                 const EOR_ZeroPageXIndexed_Expectations &expectations)
{
    EXPECT_THAT(registers.a, Eq(expectations.a));
    EXPECT_THAT(registers.x, Eq(expectations.x));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::N), Eq(expectations.flags.n_value.expected_value));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::Z), Eq(expectations.flags.z_value.expected_value));
}

template<>
void MemoryContainsInstruction(const InstructionExecutorTestFixture &fixture,
                               const Instruction<AbstractInstruction_e::EOR, ZeroPageXIndexed> &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter ), Eq( OpcodeFor(AbstractInstruction_e::EOR, AddressMode_e::ZeroPageXIndexed) ));
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 1), Eq(instruction.address.zero_page_address));
}

template<>
void MemoryContainsExpectedComputation(const InstructionExecutorTestFixture &,
                                       const EORZeroPageXIndexed            &)
{
    // No memory affected
}


static const std::vector<EORZeroPageXIndexed> EORZeroPageXIndexedModeTestValues {
EORZeroPageXIndexed{
    // Beginning of a page
    ZeroPageXIndexed().address(0x1000).zp_address(0xA0),
    EORZeroPageXIndexed::Requirements{
        .initial = {
            .a = 0,
            .x = 0,
            .flags = { },
            .operand = 0 },
        .final = {
            .a = 0,
            .x = 0,
            .flags = {
                .n_value = {
                    .status_flag = FLAGS6502::N,
                    .expected_value = false },
                .z_value = {
                    .status_flag = FLAGS6502::Z,
                    .expected_value = true } },
            .operand = 0x00
        }}
},
EORZeroPageXIndexed{
    // One before the end of a page
    ZeroPageXIndexed().address(0x10FE).zp_address(0xA0),
    EORZeroPageXIndexed::Requirements{
        .initial = {
            .a = 0xFF,
            .x = 0x0F,
            .flags = { },
            .operand = 0x00 },
        .final = {
            .a = 0xFF,
            .x = 0x0F,
            .flags = {
                .n_value = {
                    .status_flag = FLAGS6502::N,
                    .expected_value = true },
                .z_value = {
                    .status_flag = FLAGS6502::Z,
                    .expected_value = false } },
            .operand = 0x00
        }}
},
EORZeroPageXIndexed{
    // Crossing a page boundary
    ZeroPageXIndexed().address(0x10FF).zp_address(0xA0),
    EORZeroPageXIndexed::Requirements{
        .initial = {
            .a = 0x00,
            .x = 0x0F,
            .flags = { },
            .operand = 0xFF },
        .final = {
            .a = 0xFF,
            .x = 0x0F,
            .flags = {
                .n_value = {
                    .status_flag = FLAGS6502::N,
                    .expected_value = true },
                .z_value = {
                    .status_flag = FLAGS6502::Z,
                    .expected_value = false } },
            .operand = 0xFF
        }}
},
EORZeroPageXIndexed{
    // Test individual bits are EOR'd.
    ZeroPageXIndexed().address(0x1080).zp_address(0xFF),
    EORZeroPageXIndexed::Requirements{
        .initial = {
            .a = 0b10101010,
            .x = 0x02,
            .flags = { },
            .operand = 0b10101010 },
        .final = {
            .a = 0b00000000,
            .x = 0x02,
            .flags = {
                .n_value = {
                    .status_flag = FLAGS6502::N,
                    .expected_value = false },
                .z_value = {
                    .status_flag = FLAGS6502::Z,
                    .expected_value = true } },
            .operand = 0b10101010
        }}
},
EORZeroPageXIndexed{
    // Test individual bits are EOR'd.
    ZeroPageXIndexed().address(0x1080).zp_address(0xFF),
    EORZeroPageXIndexed::Requirements{
        .initial = {
            .a = 0b01010101,
            .x = 0xFF,
            .flags = { },
            .operand = 0b01010101 },
        .final = {
            .a = 0b00000000,
            .x = 0xFF,
            .flags = {
                .n_value = {
                    .status_flag = FLAGS6502::N,
                    .expected_value = false },
                .z_value = {
                    .status_flag = FLAGS6502::Z,
                    .expected_value = true } },
            .operand = 0b01010101
        }}
},
EORZeroPageXIndexed{
    // Test individual bits are EOR'd.
    ZeroPageXIndexed().address(0x1080).zp_address(0xFF),
    EORZeroPageXIndexed::Requirements{
        .initial = {
            .a = 0b10101010,
            .x = 0x80,
            .flags = { },
            .operand = 0b01010101 },
        .final = {
            .a = 0b11111111,
            .x = 0x80,
            .flags = {
                .n_value = {
                    .status_flag = FLAGS6502::N,
                    .expected_value = true },
                .z_value = {
                    .status_flag = FLAGS6502::Z,
                    .expected_value = false } },
            .operand = 0b01010101
        }}
}
};

TEST_P(EORZeroPageXIndexedMode, TypicalInstructionExecution)
{
    TypicalInstructionExecution(*this, GetParam());
}

INSTANTIATE_TEST_SUITE_P(ExclusiveOrZeroPageXIndexedAtVariousAddresses,
                         EORZeroPageXIndexedMode,
                         testing::ValuesIn(EORZeroPageXIndexedModeTestValues) );
