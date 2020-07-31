#include <gmock/gmock.h>
#include "instruction_checks.hpp"



struct CMP_ZeroPageXIndexed_Expectations
{
    uint8_t  a;
    uint8_t  x;
    NZCFlags flags;

    uint8_t  operand;
};

using CMPZeroPageXIndexed     = CMP<ZeroPageXIndexed, CMP_ZeroPageXIndexed_Expectations, 4>;
using CMPZeroPageXIndexedMode = ParameterizedInstructionExecutorTestFixture<CMPZeroPageXIndexed>;


template<>
void LoadInstructionIntoMemoryAndSetRegistersToInitialState(      InstructionExecutorTestFixture &fixture,
                                                            const CMPZeroPageXIndexed            &instruction_param)
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
    fixture.r.SetFlag(FLAGS6502::C, instruction_param.requirements.initial.flags.c_value.expected_value);
}

template<>
void RegistersAreInExpectedState(const Registers &registers,
                                 const CMP_ZeroPageXIndexed_Expectations &expectations)
{
    EXPECT_THAT(registers.a, Eq(expectations.a));
    EXPECT_THAT(registers.x, Eq(expectations.x));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::N), Eq(expectations.flags.n_value.expected_value));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::Z), Eq(expectations.flags.z_value.expected_value));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::C), Eq(expectations.flags.c_value.expected_value));
}

template<>
void MemoryContainsInstruction(const InstructionExecutorTestFixture &fixture,
                               const Instruction<AbstractInstruction_e::CMP, ZeroPageXIndexed> &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter ), Eq( OpcodeFor(AbstractInstruction_e::CMP, AddressMode_e::ZeroPageXIndexed) ));
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 1), Eq(instruction.address.zero_page_address));
}

template<>
void MemoryContainsExpectedComputation(const InstructionExecutorTestFixture &,
                                       const CMPZeroPageXIndexed            &)
{
    // No Memory Affected!
}


static const std::vector<CMPZeroPageXIndexed> CMPZeroPageXIndexedModeTestValues {
CMPZeroPageXIndexed{
    // Beginning of a page
    // 0 - 0
    ZeroPageXIndexed().address(0xA000).zp_address(6),
    CMPZeroPageXIndexed::Requirements{
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
            .operand = 0x00
        }}
},
CMPZeroPageXIndexed{
    // Middle of a page
    // 0 - 0
    ZeroPageXIndexed().address(0xA080).zp_address(0x80),
    CMPZeroPageXIndexed::Requirements{
        .initial = {
            .a = 0,
            .x = 2,
            .flags = { },
            .operand = 0 },
        .final = {
            .a = 0,
            .x = 2,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true },
                .c_value = { .expected_value = true } },
            .operand = 0
        }}
},
CMPZeroPageXIndexed{
    // End of a page
    // 0xFF - 0
    ZeroPageXIndexed().address(0xA0FE).zp_address(0xFF),
    CMPZeroPageXIndexed::Requirements{
        .initial = {
            .a = 0xFF,
            .x = 32,
            .flags = { },
            .operand = 0 },
        .final = {
            .a = 0xFF,
            .x = 32,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true } },
            .operand = 0
        }}
},
CMPZeroPageXIndexed{
    // Crossing a page
    // 0 - 0xFF
    ZeroPageXIndexed().address(0xA0FF).zp_address(0xCF),
    CMPZeroPageXIndexed::Requirements{
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
CMPZeroPageXIndexed{
    // 2 - 3
    // Negative Result
    ZeroPageXIndexed().address(0x8000).zp_address(0x10),
    CMPZeroPageXIndexed::Requirements{
        .initial = {
            .a = 0x02,
            .x = 0,
            .flags = { },
            .operand = 0x03 },
        .final = {
            .a = 0x02,
            .x = 0,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false } },
            .operand = 0x03
        }}
},
CMPZeroPageXIndexed{
    // 3 - 2
    // Positive Result
    ZeroPageXIndexed().address(0x8000).zp_address(0xA0),
    CMPZeroPageXIndexed::Requirements{
        .initial = {
            .a = 0x03,
            .x = 16,
            .flags = { },
            .operand = 0x02 },
        .final = {
            .a = 0x03,
            .x = 16,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true } },
            .operand = 0x02
        }}
}
};

TEST_P(CMPZeroPageXIndexedMode, TypicalInstructionExecution)
{
    TypicalInstructionExecution(*this, GetParam());
}

INSTANTIATE_TEST_SUITE_P(CompareAccumulatorZeroPageXIndexedAtVariousAddresses,
                         CMPZeroPageXIndexedMode,
                         testing::ValuesIn(CMPZeroPageXIndexedModeTestValues) );
