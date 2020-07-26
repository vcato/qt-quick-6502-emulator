#include <gmock/gmock.h>
#include "instruction_checks.hpp"



struct AND_Absolute_Expectations
{
    constexpr AND_Absolute_Expectations &accumulator(const uint8_t v) { a = v; return *this; }

    uint8_t a;
    NZFlags flags;

    uint8_t   operand;
};

using ANDAbsolute     = AND<Absolute, AND_Absolute_Expectations, 4>;
using ANDAbsoluteMode = ParameterizedInstructionExecutorTestFixture<ANDAbsolute>;


template<>
void LoadInstructionIntoMemoryAndSetRegistersToInitialState(      InstructionExecutorTestFixture &fixture,
                                                            const ANDAbsolute                    &instruction_param)
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
}

template<>
void RegistersAreInExpectedState(const Registers &registers,
                                 const AND_Absolute_Expectations &expectations)
{
    EXPECT_THAT(registers.a, Eq(expectations.a));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::N), Eq(expectations.flags.n_value.expected_value));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::Z), Eq(expectations.flags.z_value.expected_value));
}

template<>
void MemoryContainsInstruction(const InstructionExecutorTestFixture &fixture,
                               const Instruction<AbstractInstruction_e::AND, Absolute> &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter ), Eq( OpcodeFor(AbstractInstruction_e::AND, AddressMode_e::Absolute) ));
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 1), Eq( fixture.loByteOf(instruction.address.absolute_address) ));
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 2), Eq( fixture.hiByteOf(instruction.address.absolute_address) ));
}

template<>
void MemoryContainsExpectedComputation(const InstructionExecutorTestFixture &fixture,
                                       const ANDAbsolute                    &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( instruction.address.absolute_address ), Eq( instruction.requirements.initial.operand ));
}


static const std::vector<ANDAbsolute> ANDAbsoluteModeTestValues {
ANDAbsolute{
    // Beginning of a page
    Absolute().address(0x0000).value(0xA000),
    ANDAbsolute::Requirements{
        .initial = {
            .a = 0,
            .flags = { },
            .operand = 0x00 },
        .final = {
            .a = 0,
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
ANDAbsolute{
    // Middle of a page
    Absolute().address(0x0088).value(0xA000),
    ANDAbsolute::Requirements{
        .initial = {
            .a = 0,
            .flags = { },
            .operand = 0x00 },
        .final = {
            .a = 0,
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
ANDAbsolute{
    // End of a page
    Absolute().address(0x00FD).value(0xA000),
    ANDAbsolute::Requirements{
        .initial = {
            .a = 0,
            .flags = { },
            .operand = 0x00 },
        .final = {
            .a = 0,
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
ANDAbsolute{
    // Crossing a page (partial absolute address)
    Absolute().address(0x00FE).value(0xA000),
    ANDAbsolute::Requirements{
        .initial = {
            .a = 0,
            .flags = { },
            .operand = 0x00 },
        .final = {
            .a = 0,
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
ANDAbsolute{
    // Crossing a page (entire absolute address)
    Absolute().address(0x00FF).value(0xA000),
    ANDAbsolute::Requirements{
        .initial = {
            .a = 0,
            .flags = { },
            .operand = 0x00 },
        .final = {
            .a = 0,
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
ANDAbsolute{
    // Check for masking out the high bit
    Absolute().address(0x8000).value(0xA000),
    ANDAbsolute::Requirements{
        .initial = {
            .a = 0xFF,
            .flags = { },
            .operand = 0x80 },
        .final = {
            .a = 0x80,
            .flags = {
                .n_value = {
                    .status_flag = FLAGS6502::N,
                    .expected_value = true },
                .z_value = {
                    .status_flag = FLAGS6502::Z,
                    .expected_value = false } },
            .operand = 0x80
        }}
},
ANDAbsolute{
    // Use alternating bits for a zero result
    Absolute().address(0x8000).value(0xA000),
    ANDAbsolute::Requirements{
        .initial = {
            .a = 0b10101010,
            .flags = { },
            .operand = 0b01010101 },
        .final = {
            .a = 0x00,
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
ANDAbsolute{
    // Use the same bits for the same result
    Absolute().address(0x8000).value(0xA000),
    ANDAbsolute::Requirements{
        .initial = {
            .a = 0b10101010,
            .flags = { },
            .operand = 0b10101010 },
        .final = {
            .a = 0b10101010,
            .flags = {
                .n_value = {
                    .status_flag = FLAGS6502::N,
                    .expected_value = true },
                .z_value = {
                    .status_flag = FLAGS6502::Z,
                    .expected_value = false } },
            .operand = 0b10101010
        }}
},
ANDAbsolute{
    // Use the same bits for the same result (not the same pattern as before)
    Absolute().address(0x8000).value(0xA000),
    ANDAbsolute::Requirements{
        .initial = {
            .a = 0b01010101,
            .flags = { },
            .operand = 0b01010101 },
        .final = {
            .a = 0b01010101,
            .flags = {
                .n_value = {
                    .status_flag = FLAGS6502::N,
                    .expected_value = false },
                .z_value = {
                    .status_flag = FLAGS6502::Z,
                    .expected_value = false } },
            .operand = 0b01010101
        }}
}
};

TEST_P(ANDAbsoluteMode, TypicalInstructionExecution)
{
    TypicalInstructionExecution(*this, GetParam());
}

INSTANTIATE_TEST_SUITE_P(AndAbsoluteAtVariousAddresses,
                         ANDAbsoluteMode,
                         testing::ValuesIn(ANDAbsoluteModeTestValues) );
