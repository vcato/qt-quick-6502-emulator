#include <gmock/gmock.h>
#include "instruction_checks.hpp"



struct CPY_ZeroPage_Expectations
{
    uint8_t  y;
    NZCFlags flags;

    uint8_t  operand;
};

using CPYZeroPage     = CPY<ZeroPage, CPY_ZeroPage_Expectations, 3>;
using CPYZeroPageMode = ParameterizedInstructionExecutorTestFixture<CPYZeroPage>;


template<>
void LoadInstructionIntoMemoryAndSetRegistersToInitialState(      InstructionExecutorTestFixture &fixture,
                                                            const CPYZeroPage                    &instruction_param)
{
    fixture.loadOpcodeIntoMemory(instruction_param.operation,
                                 AddressMode_e::ZeroPage,
                                 instruction_param.address.instruction_address);
    fixture.fakeMemory[instruction_param.address.instruction_address + 1] = instruction_param.address.zero_page_address;

    // Load expected data into memory
    fixture.fakeMemory[instruction_param.address.zero_page_address] = instruction_param.requirements.initial.operand;

    // Load appropriate registers
    fixture.r.y = instruction_param.requirements.initial.y;
    fixture.r.SetFlag(FLAGS6502::N, instruction_param.requirements.initial.flags.n_value.expected_value);
    fixture.r.SetFlag(FLAGS6502::Z, instruction_param.requirements.initial.flags.z_value.expected_value);
    fixture.r.SetFlag(FLAGS6502::C, instruction_param.requirements.initial.flags.c_value.expected_value);
}

template<>
void RegistersAreInExpectedState(const Registers &registers,
                                 const CPY_ZeroPage_Expectations &expectations)
{
    EXPECT_THAT(registers.y, Eq(expectations.y));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::N), Eq(expectations.flags.n_value.expected_value));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::Z), Eq(expectations.flags.z_value.expected_value));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::C), Eq(expectations.flags.c_value.expected_value));
}

template<>
void MemoryContainsInstruction(const InstructionExecutorTestFixture &fixture,
                               const Instruction<AbstractInstruction_e::CPY, ZeroPage> &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter ), Eq( OpcodeFor(AbstractInstruction_e::CPY, AddressMode_e::ZeroPage) ));
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 1), Eq(instruction.address.zero_page_address));
}

template<>
void MemoryContainsExpectedComputation(const InstructionExecutorTestFixture &,
                                       const CPYZeroPage                    &)
{
    // No Memory Affected!
}


static const std::vector<CPYZeroPage> CPYZeroPageModeTestValues {
CPYZeroPage{
    // Beginning of a page
    // 0 - 0
    ZeroPage().address(0xA000).zp_address(6),
    CPYZeroPage::Requirements{
        .initial = {
            .y = 0,
            .flags = { },
            .operand = 0 },
        .final = {
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
CPYZeroPage{
    // Middle of a page
    // 0 - 0
    ZeroPage().address(0xA080).zp_address(0x80),
    CPYZeroPage::Requirements{
        .initial = {
            .y = 0,
            .flags = { },
            .operand = 0 },
        .final = {
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
CPYZeroPage{
    // End of a page
    // 0xFF - 0
    ZeroPage().address(0xA0FE).zp_address(0xFF),
    CPYZeroPage::Requirements{
        .initial = {
            .y = 0xFF,
            .flags = { },
            .operand = 0 },
        .final = {
            .y = 0xFF,
            .flags = {
                .n_value = {
                    .status_flag = FLAGS6502::N,
                    .expected_value = true },
                .z_value = {
                    .status_flag = FLAGS6502::Z,
                    .expected_value = false },
                .c_value = {
                    .status_flag = FLAGS6502::C,
                    .expected_value = true } },
            .operand = 0
        }}
},
CPYZeroPage{
    // Crossing a page
    // 0 - 0xFF
    ZeroPage().address(0xA0FF).zp_address(0xCF),
    CPYZeroPage::Requirements{
        .initial = {
            .y = 0,
            .flags = { },
            .operand = 0xFF },
        .final = {
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
CPYZeroPage{
    // 2 - 3
    // Negative Result
    ZeroPage().address(0x8000).zp_address(0xA0),
    CPYZeroPage::Requirements{
        .initial = {
            .y = 0x02,
            .flags = { },
            .operand = 0x03 },
        .final = {
            .y = 0x02,
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
            .operand = 0x03
        }}
},
CPYZeroPage{
    // 3 - 2
    // Positive Result
    ZeroPage().address(0x8000).zp_address(0xA0),
    CPYZeroPage::Requirements{
        .initial = {
            .y = 0x03,
            .flags = { },
            .operand = 0x02 },
        .final = {
            .y = 0x03,
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
            .operand = 0x02
        }}
}
};

TEST_P(CPYZeroPageMode, TypicalInstructionExecution)
{
    TypicalInstructionExecution(*this, GetParam());
}

INSTANTIATE_TEST_SUITE_P(CompareYZeroPageAtVariousAddresses,
                         CPYZeroPageMode,
                         testing::ValuesIn(CPYZeroPageModeTestValues) );
