#include <gmock/gmock.h>
#include "instruction_checks.hpp"



struct DEC_ZeroPage_Expectations
{
    NZFlags flags;

    uint8_t operand;
};

using DECZeroPage     = DEC<ZeroPage, DEC_ZeroPage_Expectations, 5>;
using DECZeroPageMode = ParameterizedInstructionExecutorTestFixture<DECZeroPage>;


template<>
void LoadInstructionIntoMemoryAndSetRegistersToInitialState(      InstructionExecutorTestFixture &fixture,
                                                            const DECZeroPage                    &instruction_param)
{
    fixture.loadOpcodeIntoMemory(instruction_param.operation,
                                 AddressMode_e::ZeroPage,
                                 instruction_param.address.instruction_address);
    fixture.fakeMemory[instruction_param.address.instruction_address + 1] = instruction_param.address.zero_page_address;

    // Load expected data into memory
    fixture.fakeMemory[instruction_param.address.zero_page_address] = instruction_param.requirements.initial.operand;

    // Load appropriate registers
    fixture.r.SetFlag(FLAGS6502::N, instruction_param.requirements.initial.flags.n_value.expected_value);
    fixture.r.SetFlag(FLAGS6502::Z, instruction_param.requirements.initial.flags.z_value.expected_value);
}

template<>
void RegistersAreInExpectedState(const Registers &registers,
                                 const DEC_ZeroPage_Expectations &expectations)
{
    EXPECT_THAT(registers.GetFlag(FLAGS6502::N), Eq(expectations.flags.n_value.expected_value));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::Z), Eq(expectations.flags.z_value.expected_value));
}

template<>
void MemoryContainsInstruction(const InstructionExecutorTestFixture &fixture,
                               const Instruction<AbstractInstruction_e::DEC, ZeroPage> &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter ), Eq( OpcodeFor(AbstractInstruction_e::DEC, AddressMode_e::ZeroPage) ));
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 1), Eq(instruction.address.zero_page_address));
}

template<>
void MemoryContainsExpectedComputation(const InstructionExecutorTestFixture &fixture,
                                       const DECZeroPage                    &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( instruction.address.zero_page_address ), Eq(instruction.requirements.initial.operand));
}

template<>
void MemoryContainsExpectedResult(const InstructionExecutorTestFixture &fixture,
                                  const DECZeroPage                    &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( instruction.address.zero_page_address ), Eq(instruction.requirements.final.operand));
}

static const std::vector<DECZeroPage> DECZeroPageModeTestValues {
DECZeroPage{
    // Beginning of a page
    ZeroPage().address(0xA000).zp_address(6),
    DECZeroPage::Requirements{
        .initial = {
            .flags = { },
            .operand = 0xFF },
        .final = {
            .flags = {
                .n_value = {
                    .status_flag = FLAGS6502::N,
                    .expected_value = true },
                .z_value = {
                    .status_flag = FLAGS6502::Z,
                    .expected_value = false } },
            .operand = 0xFE
        }}
},
DECZeroPage{
    // Middle of a page
    ZeroPage().address(0xA080).zp_address(0x80),
    DECZeroPage::Requirements{
        .initial = {
            .flags = { },
            .operand = 0xFF },
        .final = {
            .flags = {
                .n_value = {
                    .status_flag = FLAGS6502::N,
                    .expected_value = true },
                .z_value = {
                    .status_flag = FLAGS6502::Z,
                    .expected_value = false } },
            .operand = 0xFE
        }}
},
DECZeroPage{
    // End of a page
    ZeroPage().address(0xA0FE).zp_address(0xFF),
    DECZeroPage::Requirements{
        .initial = {
            .flags = { },
            .operand = 0xFF },
        .final = {
            .flags = {
                .n_value = {
                    .status_flag = FLAGS6502::N,
                    .expected_value = true },
                .z_value = {
                    .status_flag = FLAGS6502::Z,
                    .expected_value = false } },
            .operand = 0xFE
        }}
},
DECZeroPage{
    // Crossing a page
    ZeroPage().address(0xA0FF).zp_address(0xCF),
    DECZeroPage::Requirements{
        .initial = {
            .flags = { },
            .operand = 0xFF },
        .final = {
            .flags = {
                .n_value = {
                    .status_flag = FLAGS6502::N,
                    .expected_value = true },
                .z_value = {
                    .status_flag = FLAGS6502::Z,
                    .expected_value = false } },
            .operand = 0xFE
        }}
},
DECZeroPage{
    // Check for rolling back from 0 to 0xFF
    ZeroPage().address(0x8000).zp_address(0xA0),
    DECZeroPage::Requirements{
        .initial = {
            .flags = { },
            .operand = 0 },
        .final = {
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
DECZeroPage{
    // Rolling TO 0
    ZeroPage().address(0x8000).zp_address(0xA0),
    DECZeroPage::Requirements{
        .initial = {
            .flags = { },
            .operand = 1 },
        .final = {
            .flags = {
                .n_value = {
                    .status_flag = FLAGS6502::N,
                    .expected_value = false },
                .z_value = {
                    .status_flag = FLAGS6502::Z,
                    .expected_value = true } },
            .operand = 0
        }}
},
DECZeroPage{
    // Crossing 0x80 (negative to positive)
    ZeroPage().address(0x8000).zp_address(0xA0),
    DECZeroPage::Requirements{
        .initial = {
            .flags = {
                .n_value = {
                    .status_flag = FLAGS6502::N,
                    .expected_value = true },
                .z_value = {
                    .status_flag = FLAGS6502::Z,
                    .expected_value = true } },
            .operand = 0x80 },
        .final = {
            .flags = {
                .n_value = {
                    .status_flag = FLAGS6502::N,
                    .expected_value = false },
                .z_value = {
                    .status_flag = FLAGS6502::Z,
                    .expected_value = false } },
            .operand = 0x7F
        }}
}
};

TEST_P(DECZeroPageMode, TypicalInstructionExecution)
{
    TypicalInstructionExecution(*this, GetParam());
}

INSTANTIATE_TEST_SUITE_P(DecrementMemoryZeroPageAtVariousAddresses,
                         DECZeroPageMode,
                         testing::ValuesIn(DECZeroPageModeTestValues) );
