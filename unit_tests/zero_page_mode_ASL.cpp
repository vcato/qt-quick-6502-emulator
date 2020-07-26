#include <gmock/gmock.h>
#include "instruction_checks.hpp"



struct ASL_ZeroPage_Expectations
{
    uint8_t  a;
    NZCFlags flags;

    uint8_t  operand; // Data to be operated upon in Zero Page
};

using ASLZeroPage     = ASL<ZeroPage, ASL_ZeroPage_Expectations, 5>;
using ASLZeroPageMode = ParameterizedInstructionExecutorTestFixture<ASLZeroPage>;


template<>
void LoadInstructionIntoMemoryAndSetRegistersToInitialState(      InstructionExecutorTestFixture &fixture,
                                                            const ASLZeroPage                    &instruction_param)
{
    fixture.loadOpcodeIntoMemory(instruction_param.operation,
                                 AddressMode_e::ZeroPage,
                                 instruction_param.address.instruction_address);
    fixture.fakeMemory[instruction_param.address.instruction_address + 1] = instruction_param.address.zero_page_address;

    // Load expected data into memory
    fixture.fakeMemory[instruction_param.address.zero_page_address] = instruction_param.requirements.initial.operand;

    // Load appropriate registers
    fixture.r.a = instruction_param.requirements.initial.a;
    fixture.r.SetFlag(FLAGS6502::N, instruction_param.requirements.initial.flags.n_value.expected_value);
    fixture.r.SetFlag(FLAGS6502::Z, instruction_param.requirements.initial.flags.z_value.expected_value);
    fixture.r.SetFlag(FLAGS6502::C, instruction_param.requirements.initial.flags.c_value.expected_value);
}

template<>
void RegistersAreInExpectedState(const Registers &registers,
                                 const ASL_ZeroPage_Expectations &expectations)
{
    EXPECT_THAT(registers.a, Eq(expectations.a));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::N), Eq(expectations.flags.n_value.expected_value));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::Z), Eq(expectations.flags.z_value.expected_value));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::C), Eq(expectations.flags.c_value.expected_value));
}

template<>
void MemoryContainsInstruction(const InstructionExecutorTestFixture &fixture,
                               const Instruction<AbstractInstruction_e::ASL, ZeroPage> &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter ), Eq( OpcodeFor(AbstractInstruction_e::ASL, AddressMode_e::ZeroPage) ));
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 1), Eq(instruction.address.zero_page_address));
}

template<>
void MemoryContainsExpectedComputation(const InstructionExecutorTestFixture &fixture,
                                       const ASLZeroPage                    &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( instruction.address.zero_page_address ), Eq(instruction.requirements.initial.operand));
}

template<>
void MemoryContainsExpectedResult(const InstructionExecutorTestFixture &fixture,
                                  const ASLZeroPage                    &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( instruction.address.zero_page_address ), Eq(instruction.requirements.final.operand));
}


static const std::vector<ASLZeroPage> ASLZeroPageModeTestValues {
ASLZeroPage{
    // Beginning of a page
    ZeroPage().address(0x8000).zp_address(6),
    ASLZeroPage::Requirements{
        .initial = {
            .a = 0,
            .flags = { },
            .operand = 0 },
        .final = {
            .a = 0,
            .flags = {
                .n_value = {
                    .status_flag = FLAGS6502::N,
                    .expected_value = false },
                .z_value = {
                    .status_flag = FLAGS6502::Z,
                    .expected_value = true },
                .c_value = {
                    .status_flag = FLAGS6502::C,
                    .expected_value = false } },
            .operand = 0
        }}
},
ASLZeroPage{
    // Middle of a page
    ZeroPage().address(0x8080).zp_address(6),
    ASLZeroPage::Requirements{
        .initial = {
            .a = 0,
            .flags = { },
            .operand = 0 },
        .final = {
            .a = 0,
            .flags = {
                .n_value = {
                    .status_flag = FLAGS6502::N,
                    .expected_value = false },
                .z_value = {
                    .status_flag = FLAGS6502::Z,
                    .expected_value = true },
                .c_value = {
                    .status_flag = FLAGS6502::C,
                    .expected_value = false } },
            .operand = 0
        }}
},
ASLZeroPage{
    // End of a page
    ZeroPage().address(0x80FE).zp_address(6),
    ASLZeroPage::Requirements{
        .initial = {
            .a = 0,
            .flags = { },
            .operand = 0 },
        .final = {
            .a = 0,
            .flags = {
                .n_value = {
                    .status_flag = FLAGS6502::N,
                    .expected_value = false },
                .z_value = {
                    .status_flag = FLAGS6502::Z,
                    .expected_value = true },
                .c_value = {
                    .status_flag = FLAGS6502::C,
                    .expected_value = false } },
            .operand = 0
        }}
},
ASLZeroPage{
    // Crossing a page boundary
    ZeroPage().address(0x80FF).zp_address(6),
    ASLZeroPage::Requirements{
        .initial = {
            .a = 0,
            .flags = { },
            .operand = 0 },
        .final = {
            .a = 0,
            .flags = {
                .n_value = {
                    .status_flag = FLAGS6502::N,
                    .expected_value = false },
                .z_value = {
                    .status_flag = FLAGS6502::Z,
                    .expected_value = true },
                .c_value = {
                    .status_flag = FLAGS6502::C,
                    .expected_value = false } },
            .operand = 0
        }}
},
ASLZeroPage{
    // Check for High bit going into carry
    ZeroPage().address(0x8000).zp_address(6),
    ASLZeroPage::Requirements{
        .initial = {
            .a = 1,
            .flags = { },
            .operand = 0b10101010 },
        .final = {
            .a = 1,
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
            .operand = 0b01010100
        }}
},
ASLZeroPage{
    // Check for N flag
    ZeroPage().address(0x8000).zp_address(6),
    ASLZeroPage::Requirements{
        .initial = {
            .a = 0xFF,
            .flags = { },
            .operand = 0b11101010 },
        .final = {
            .a = 0xFF,
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
            .operand = 0b11010100
        }}
},
ASLZeroPage{
    // Zero is set in lowest bit
    ZeroPage().address(0x8000).zp_address(6),
    ASLZeroPage::Requirements{
        .initial = {
            .a = 0x00,
            .flags = { },
            .operand = 0b00000001 },
        .final = {
            .a = 0x00,
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
            .operand = 0b00000010
        }}
}
};

TEST_P(ASLZeroPageMode, TypicalInstructionExecution)
{
    TypicalInstructionExecution(*this, GetParam());
}

INSTANTIATE_TEST_SUITE_P(AddZeroPageAtVariousAddresses,
                         ASLZeroPageMode,
                         testing::ValuesIn(ASLZeroPageModeTestValues) );
