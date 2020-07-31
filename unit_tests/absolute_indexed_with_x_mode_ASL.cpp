#include <gmock/gmock.h>
#include "instruction_checks.hpp"



struct ASL_AbsoluteXIndexed_Expectations
{
    constexpr ASL_AbsoluteXIndexed_Expectations &accumulator(const uint8_t v) { a = v; return *this; }

    uint8_t a;
    uint8_t x;
    NZCFlags flags;

    uint8_t   operand;
};

using ASLAbsoluteXIndexed     = ASL<AbsoluteXIndexed, ASL_AbsoluteXIndexed_Expectations, 7>;
using ASLAbsoluteXIndexedMode = ParameterizedInstructionExecutorTestFixture<ASLAbsoluteXIndexed>;


template<>
void LoadInstructionIntoMemoryAndSetRegistersToInitialState(      InstructionExecutorTestFixture &fixture,
                                                            const ASLAbsoluteXIndexed            &instruction_param)
{
    fixture.loadOpcodeIntoMemory(instruction_param.operation,
                                 AddressMode_e::AbsoluteXIndexed,
                                 instruction_param.address.instruction_address);
    fixture.fakeMemory[instruction_param.address.instruction_address + 1] = fixture.loByteOf(instruction_param.address.absolute_address);
    fixture.fakeMemory[instruction_param.address.instruction_address + 2] = fixture.hiByteOf(instruction_param.address.absolute_address);
    fixture.fakeMemory[instruction_param.address.absolute_address + instruction_param.requirements.final.x ] = instruction_param.requirements.initial.operand;

    // Load appropriate registers
    fixture.r.a = instruction_param.requirements.initial.a;
    fixture.r.x = instruction_param.requirements.initial.x;
    fixture.r.SetFlag(FLAGS6502::N, instruction_param.requirements.initial.flags.n_value.expected_value);
    fixture.r.SetFlag(FLAGS6502::Z, instruction_param.requirements.initial.flags.z_value.expected_value);
    fixture.r.SetFlag(FLAGS6502::C, instruction_param.requirements.initial.flags.c_value.expected_value);
}

template<>
void RegistersAreInExpectedState(const Registers &registers,
                                 const ASL_AbsoluteXIndexed_Expectations &expectations)
{
    EXPECT_THAT(registers.a, Eq(expectations.a));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::N), Eq(expectations.flags.n_value.expected_value));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::Z), Eq(expectations.flags.z_value.expected_value));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::C), Eq(expectations.flags.c_value.expected_value));
}

template<>
void MemoryContainsInstruction(const InstructionExecutorTestFixture &fixture,
                               const Instruction<AbstractInstruction_e::ASL, AbsoluteXIndexed> &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter ), Eq( OpcodeFor(AbstractInstruction_e::ASL, AddressMode_e::AbsoluteXIndexed) ));

    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 1), Eq( fixture.loByteOf(instruction.address.absolute_address) ));
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 2), Eq( fixture.hiByteOf(instruction.address.absolute_address) ));
}

template<>
void MemoryContainsExpectedComputation(const InstructionExecutorTestFixture &fixture,
                                       const ASLAbsoluteXIndexed            &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( instruction.address.absolute_address + instruction.requirements.final.x ), Eq( instruction.requirements.initial.operand ));
}

template<>
void MemoryContainsExpectedResult(const InstructionExecutorTestFixture &fixture,
                                  const ASLAbsoluteXIndexed            &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( instruction.address.absolute_address + instruction.requirements.final.x ), Eq(instruction.requirements.final.operand));
}


static const std::vector<ASLAbsoluteXIndexed> ASLAbsoluteXIndexedModeTestValues {
ASLAbsoluteXIndexed{
    // Beginning of a page
    AbsoluteXIndexed().address(0xA000).value(0xC000),
    ASLAbsoluteXIndexed::Requirements{
        .initial = {
            .a = 0,
            .x = 0,
            .flags = { },
            .operand = 0},
        .final = {
            .a = 0,
            .x = 0,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true },
                .c_value = { .expected_value = false } },
            .operand = 0
        }}
},
ASLAbsoluteXIndexed{
    // Middle of a page
    AbsoluteXIndexed().address(0xA088).value(0xC000),
    ASLAbsoluteXIndexed::Requirements{
        .initial = {
            .a = 0,
            .x = 5,
            .flags = { },
            .operand = 0},
        .final = {
            .a = 0,
            .x = 5,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true },
                .c_value = { .expected_value = false } },
            .operand = 0
        }}
},
ASLAbsoluteXIndexed{
    // End of a page
    AbsoluteXIndexed().address(0xA0FD).value(0xC000),
    ASLAbsoluteXIndexed::Requirements{
        .initial = {
            .a = 0,
            .x = 5,
            .flags = { },
            .operand = 0},
        .final = {
            .a = 0,
            .x = 5,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true },
                .c_value = { .expected_value = false } },
            .operand = 0
        }}
},
ASLAbsoluteXIndexed{
    // Crossing a page (partial absolute address)
    AbsoluteXIndexed().address(0xA0FE).value(0xC000),
    ASLAbsoluteXIndexed::Requirements{
        .initial = {
            .a = 0,
            .x = 5,
            .flags = { },
            .operand = 0},
        .final = {
            .a = 0,
            .x = 5,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true },
                .c_value = { .expected_value = false } },
            .operand = 0
        }}
},
ASLAbsoluteXIndexed{
    // Crossing a page (entire absolute address)
    AbsoluteXIndexed().address(0xA0FF).value(0xC000),
    ASLAbsoluteXIndexed::Requirements{
        .initial = {
            .a = 0,
            .x = 5,
            .flags = { },
            .operand = 0},
        .final = {
            .a = 0,
            .x = 5,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true },
                .c_value = { .expected_value = false } },
            .operand = 0
        }}
},
ASLAbsoluteXIndexed{
    // Check for High bit going into carry
    AbsoluteXIndexed().address(0x8000).value(0xA000),
    ASLAbsoluteXIndexed::Requirements{
        .initial = {
            .a = 1,
            .x = 0,
            .flags = { },
            .operand = 0b10101010 },
        .final = {
            .a = 1,
            .x = 0,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true } },
            .operand = 0b01010100
        }}
},
ASLAbsoluteXIndexed{
    // Check for N flag
    AbsoluteXIndexed().address(0x8000).value(0xA000),
    ASLAbsoluteXIndexed::Requirements{
        .initial = {
            .a = 0xFF,
            .x = 0,
            .flags = { },
            .operand = 0b11101010 },
        .final = {
            .a = 0xFF,
            .x = 0,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true } },
            .operand = 0b11010100
        }}
},
ASLAbsoluteXIndexed{
    // Zero is set in lowest bit
    AbsoluteXIndexed().address(0x8000).value(0xA000),
    ASLAbsoluteXIndexed::Requirements{
        .initial = {
            .a = 0x00,
            .x = 0x80,
            .flags = { },
            .operand = 0b00000001 },
        .final = {
            .a = 0x00,
            .x = 0x80,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false } },
            .operand = 0b00000010
        }}
},
ASLAbsoluteXIndexed{
    // Zero is set in lowest bit (max x index)
    AbsoluteXIndexed().address(0x80FF).value(0xA0FF),
    ASLAbsoluteXIndexed::Requirements{
        .initial = {
            .a = 0x00,
            .x = 0xFF,
            .flags = { },
            .operand = 0b00000001 },
        .final = {
            .a = 0x00,
            .x = 0xFF,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false } },
            .operand = 0b00000010
        }}
}
};

TEST_P(ASLAbsoluteXIndexedMode, TypicalInstructionExecution)
{
    TypicalInstructionExecution(*this, GetParam());
}

INSTANTIATE_TEST_SUITE_P(ArithmeticShiftLeftAbsoluteXIndexedAtVariousAddresses,
                         ASLAbsoluteXIndexedMode,
                         testing::ValuesIn(ASLAbsoluteXIndexedModeTestValues) );
