#include <gmock/gmock.h>
#include "instruction_checks.hpp"



struct BIT_Absolute_Expectations
{
    constexpr BIT_Absolute_Expectations &accumulator(const uint8_t v) { a = v; return *this; }

    uint8_t  a;
    NZVFlags flags;

    uint8_t  operand;
};

using BITAbsolute     = BIT<Absolute, BIT_Absolute_Expectations, 4>;
using BITAbsoluteMode = ParameterizedInstructionExecutorTestFixture<BITAbsolute>;


template<>
void LoadInstructionIntoMemoryAndSetRegistersToInitialState(      InstructionExecutorTestFixture &fixture,
                                                            const BITAbsolute                    &instruction_param)
{
    fixture.loadOpcodeIntoMemory(instruction_param.operation,
                                 AddressMode_e::Absolute,
                                 instruction_param.address.instruction_address);
    fixture.fakeMemory[instruction_param.address.instruction_address + 1] = fixture.loByteOf(instruction_param.address.absolute_address);
    fixture.fakeMemory[instruction_param.address.instruction_address + 2] = fixture.hiByteOf(instruction_param.address.absolute_address);
    fixture.fakeMemory[instruction_param.address.absolute_address       ] = instruction_param.requirements.initial.operand;

    // Load appropriate registers
    fixture.r.a = instruction_param.requirements.initial.a;
    fixture.r.SetFlag(FLAGS6502::N, instruction_param.requirements.initial.flags.n_value.expected_value);
    fixture.r.SetFlag(FLAGS6502::Z, instruction_param.requirements.initial.flags.z_value.expected_value);
    fixture.r.SetFlag(FLAGS6502::V, instruction_param.requirements.initial.flags.v_value.expected_value);
}

template<>
void RegistersAreInExpectedState(const Registers &registers,
                                 const BIT_Absolute_Expectations &expectations)
{
    EXPECT_THAT(registers.a, Eq(expectations.a));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::N), Eq(expectations.flags.n_value.expected_value));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::Z), Eq(expectations.flags.z_value.expected_value));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::V), Eq(expectations.flags.v_value.expected_value));
}

template<>
void MemoryContainsInstruction(const InstructionExecutorTestFixture &fixture,
                               const Instruction<AbstractInstruction_e::BIT, Absolute> &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter ), Eq( OpcodeFor(AbstractInstruction_e::BIT, AddressMode_e::Absolute) ));
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 1), Eq( fixture.loByteOf(instruction.address.absolute_address) ));
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 2), Eq( fixture.hiByteOf(instruction.address.absolute_address) ));
}

template<>
void MemoryContainsExpectedComputation(const InstructionExecutorTestFixture &,
                                       const BITAbsolute                    &)
{
    // No memory affected
}


static const std::vector<BITAbsolute> BITAbsoluteModeTestValues {
BITAbsolute{
    // Beginning of a page
    // 7th bit transfers to N flag
    Absolute().address(0x1000).value(0xA000),
    BITAbsolute::Requirements{
        .initial = {
            .a = 0b00000000,
            .flags = { },
            .operand = 0b10000000},
        .final = {
            .a = 0b00000000,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = true },
                .v_value = { .expected_value = false } },
            .operand = 0b10000000
        }}
},
BITAbsolute{
    // Beginning of a page
    // 6th bit transfers to N flag
    Absolute().address(0x1000).value(0xA000),
    BITAbsolute::Requirements{
        .initial = {
            .a = 0b00000000,
            .flags = { },
            .operand = 0b01000000},
        .final = {
            .a = 0b00000000,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true },
                .v_value = { .expected_value = true } },
            .operand = 0b01000000
        }}
},
BITAbsolute{
    // Beginning of a page
    // accumulator and operand -> Z
    Absolute().address(0x1000).value(0xA000),
    BITAbsolute::Requirements{
        .initial = {
            .a = 0b10000000,
            .flags = { },
            .operand = 0b00000000},
        .final = {
            .a = 0b10000000,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true },
                .v_value = { .expected_value = false } },
            .operand = 0b00000000
        }}
},
BITAbsolute{
    // accumulator and operand -> Z
    Absolute().address(0x1000).value(0xA000),
    BITAbsolute::Requirements{
        .initial = {
            .a = 0b00100000,
            .flags = { },
            .operand = 0b00000000},
        .final = {
            .a = 0b00100000,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true },
                .v_value = { .expected_value = false } },
            .operand = 0b00000000
        }}
},
BITAbsolute{
    // accumulator and operand -> Z
    Absolute().address(0x1000).value(0xA000),
    BITAbsolute::Requirements{
        .initial = {
            .a = 0b00100000,
            .flags = { },
            .operand = 0b00100000},
        .final = {
            .a = 0b00100000,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .v_value = { .expected_value = false } },
            .operand = 0b00100000
        }}
}
};

TEST_P(BITAbsoluteMode, TypicalInstructionExecution)
{
    TypicalInstructionExecution(*this, GetParam());
}

INSTANTIATE_TEST_SUITE_P(BitAbsoluteAtVariousAddresses,
                         BITAbsoluteMode,
                         testing::ValuesIn(BITAbsoluteModeTestValues) );
