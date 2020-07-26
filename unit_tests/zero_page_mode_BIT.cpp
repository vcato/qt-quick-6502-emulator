#include <gmock/gmock.h>
#include "instruction_checks.hpp"



struct BIT_ZeroPage_Expectations
{
    constexpr BIT_ZeroPage_Expectations &accumulator(const uint8_t v) { a = v; return *this; }

    uint8_t  a;
    NZVFlags flags;

    uint8_t  operand;
};

using BITZeroPage     = BIT<ZeroPage, BIT_ZeroPage_Expectations, 3>;
using BITZeroPageMode = ParameterizedInstructionExecutorTestFixture<BITZeroPage>;


template<>
void LoadInstructionIntoMemoryAndSetRegistersToInitialState(      InstructionExecutorTestFixture &fixture,
                                                            const BITZeroPage                    &instruction_param)
{
    fixture.loadOpcodeIntoMemory(instruction_param.operation,
                                 AddressMode_e::ZeroPage,
                                 instruction_param.address.instruction_address);
    fixture.fakeMemory[instruction_param.address.instruction_address + 1] = instruction_param.address.zero_page_address;
    fixture.fakeMemory[instruction_param.address.zero_page_address      ] = instruction_param.requirements.initial.operand;

    // Load appropriate registers
    fixture.r.a = instruction_param.requirements.initial.a;
    fixture.r.SetFlag(FLAGS6502::N, instruction_param.requirements.initial.flags.n_value.expected_value);
    fixture.r.SetFlag(FLAGS6502::Z, instruction_param.requirements.initial.flags.z_value.expected_value);
    fixture.r.SetFlag(FLAGS6502::V, instruction_param.requirements.initial.flags.v_value.expected_value);
}

template<>
void RegistersAreInExpectedState(const Registers &registers,
                                 const BIT_ZeroPage_Expectations &expectations)
{
    EXPECT_THAT(registers.a, Eq(expectations.a));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::N), Eq(expectations.flags.n_value.expected_value));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::Z), Eq(expectations.flags.z_value.expected_value));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::V), Eq(expectations.flags.v_value.expected_value));
}

template<>
void MemoryContainsInstruction(const InstructionExecutorTestFixture &fixture,
                               const Instruction<AbstractInstruction_e::BIT, ZeroPage> &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter ), Eq( OpcodeFor(AbstractInstruction_e::BIT, AddressMode_e::ZeroPage) ));
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 1), Eq( fixture.loByteOf(instruction.address.zero_page_address) ));
}

template<>
void MemoryContainsExpectedComputation(const InstructionExecutorTestFixture &,
                                       const BITZeroPage                    &)
{
    // No memory affected
}


static const std::vector<BITZeroPage> BITAbsoluteModeTestValues {
BITZeroPage{
    // Beginning of a page
    // 7th bit transfers to N flag
    ZeroPage().address(0x1000).zp_address(0xA0),
    BITZeroPage::Requirements{
        .initial = {
            .a = 0b00000000,
            .flags = { },
            .operand = 0b10000000},
        .final = {
            .a = 0b00000000,
            .flags = {
                .n_value = {
                    .status_flag = FLAGS6502::N,
                    .expected_value = true },
                .z_value = {
                    .status_flag = FLAGS6502::Z,
                    .expected_value = true },
                .v_value = {
                    .status_flag = FLAGS6502::V,
                    .expected_value = false } },
            .operand = 0b10000000
        }}
},
BITZeroPage{
    // Beginning of a page
    // 6th bit transfers to N flag
    ZeroPage().address(0x1000).zp_address(0xA0),
    BITZeroPage::Requirements{
        .initial = {
            .a = 0b00000000,
            .flags = { },
            .operand = 0b01000000},
        .final = {
            .a = 0b00000000,
            .flags = {
                .n_value = {
                    .status_flag = FLAGS6502::N,
                    .expected_value = false },
                .z_value = {
                    .status_flag = FLAGS6502::Z,
                    .expected_value = true },
                .v_value = {
                    .status_flag = FLAGS6502::V,
                    .expected_value = true } },
            .operand = 0b01000000
        }}
},
BITZeroPage{
    // Beginning of a page
    // accumulator and operand -> Z
    ZeroPage().address(0x1000).zp_address(0x02),
    BITZeroPage::Requirements{
        .initial = {
            .a = 0b10000000,
            .flags = { },
            .operand = 0b00000000},
        .final = {
            .a = 0b10000000,
            .flags = {
                .n_value = {
                    .status_flag = FLAGS6502::N,
                    .expected_value = false },
                .z_value = {
                    .status_flag = FLAGS6502::Z,
                    .expected_value = true },
                .v_value = {
                    .status_flag = FLAGS6502::V,
                    .expected_value = false } },
            .operand = 0b00000000
        }}
},
BITZeroPage{
    // accumulator and operand -> Z
    ZeroPage().address(0x1000).zp_address(0x80),
    BITZeroPage::Requirements{
        .initial = {
            .a = 0b00100000,
            .flags = { },
            .operand = 0b00000000},
        .final = {
            .a = 0b00100000,
            .flags = {
                .n_value = {
                    .status_flag = FLAGS6502::N,
                    .expected_value = false },
                .z_value = {
                    .status_flag = FLAGS6502::Z,
                    .expected_value = true },
                .v_value = {
                    .status_flag = FLAGS6502::V,
                    .expected_value = false } },
            .operand = 0b00000000
        }}
},
BITZeroPage{
    // accumulator and operand -> Z
    ZeroPage().address(0x1000).zp_address(0xFF),
    BITZeroPage::Requirements{
        .initial = {
            .a = 0b00100000,
            .flags = { },
            .operand = 0b00100000},
        .final = {
            .a = 0b00100000,
            .flags = {
                .n_value = {
                    .status_flag = FLAGS6502::N,
                    .expected_value = false },
                .z_value = {
                    .status_flag = FLAGS6502::Z,
                    .expected_value = false },
                .v_value = {
                    .status_flag = FLAGS6502::V,
                    .expected_value = false } },
            .operand = 0b00100000
        }}
}
};

TEST_P(BITZeroPageMode, TypicalInstructionExecution)
{
    TypicalInstructionExecution(*this, GetParam());
}

INSTANTIATE_TEST_SUITE_P(BitAbsoluteAtVariousAddresses,
                         BITZeroPageMode,
                         testing::ValuesIn(BITAbsoluteModeTestValues) );
