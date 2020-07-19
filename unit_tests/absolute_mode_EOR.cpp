#include <gmock/gmock.h>
#include "instruction_checks.hpp"



struct EOR_Absolute_Expectations
{
    constexpr EOR_Absolute_Expectations &accumulator(const uint8_t v) { a = v; return *this; }

    uint8_t a;
    NZFlags flags;

    uint8_t operand;
};

using EORAbsolute     = EOR<Absolute, EOR_Absolute_Expectations, 4>;
using EORAbsoluteMode = ParameterizedInstructionExecutorTestFixture<EORAbsolute>;


template<>
void LoadInstructionIntoMemoryAndSetRegistersToInitialState(      InstructionExecutorTestFixture &fixture,
                                                            const EORAbsolute                    &instruction_param)
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
                                 const EOR_Absolute_Expectations &expectations)
{
    EXPECT_THAT(registers.a, Eq(expectations.a));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::N), Eq(expectations.flags.n_value.expected_value));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::Z), Eq(expectations.flags.z_value.expected_value));
}

template<>
void MemoryContainsInstruction(const InstructionExecutorTestFixture &fixture,
                               const Instruction<AbstractInstruction_e::EOR, Absolute> &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter ), Eq( OpcodeFor(AbstractInstruction_e::EOR, AddressMode_e::Absolute) ));
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 1), Eq( fixture.loByteOf(instruction.address.absolute_address) ));
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 2), Eq( fixture.hiByteOf(instruction.address.absolute_address) ));
}

template<>
void MemoryContainsExpectedComputation(const InstructionExecutorTestFixture &fixture,
                                       const EORAbsolute                    &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( instruction.address.absolute_address ), Eq( instruction.requirements.initial.operand ));
}


static const std::vector<EORAbsolute> EORAbsoluteModeTestValues {
EORAbsolute{
    // Beginning of a page
    Absolute().address(0x1000).value(0xA000),
    EORAbsolute::Requirements{
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
EORAbsolute{
    // Middle of a page
    Absolute().address(0x1080).value(0xA000),
    EORAbsolute::Requirements{
        .initial = {
            .a = 0xFF,
            .flags = { },
            .operand = 0x00 },
        .final = {
            .a = 0xFF,
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
EORAbsolute{
    // End of a page
    Absolute().address(0x10FD).value(0xA000),
    EORAbsolute::Requirements{
        .initial = {
            .a = 0xFF,
            .flags = { },
            .operand = 0x00 },
        .final = {
            .a = 0xFF,
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
EORAbsolute{
    // Crossing a page (partial absolute address)
    Absolute().address(0x10FE).value(0xA000),
    EORAbsolute::Requirements{
        .initial = {
            .a = 0x00,
            .flags = { },
            .operand = 0xFF },
        .final = {
            .a = 0xFF,
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
EORAbsolute{
    // Crossing a page (entire absolute address)
    Absolute().address(0x10FF).value(0xA000),
    EORAbsolute::Requirements{
        .initial = {
            .a = 0x00,
            .flags = { },
            .operand = 0xFF },
        .final = {
            .a = 0xFF,
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
EORAbsolute{
    // Test individual bits are EOR'd.
    Absolute().address(0x8000).value(0xA000),
    EORAbsolute::Requirements{
        .initial = {
            .a = 0b10101010,
            .flags = { },
            .operand = 0b10101010 },
        .final = {
            .a = 0b00000000,
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
EORAbsolute{
    // Test individual bits are EOR'd.
    Absolute().address(0x8000).value(0xA000),
    EORAbsolute::Requirements{
        .initial = {
            .a = 0b01010101,
            .flags = { },
            .operand = 0b01010101 },
        .final = {
            .a = 0b00000000,
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
EORAbsolute{
    // Test individual bits are EOR'd.
    Absolute().address(0x8000).value(0xA000),
    EORAbsolute::Requirements{
        .initial = {
            .a = 0b10101010,
            .flags = { },
            .operand = 0b01010101 },
        .final = {
            .a = 0b11111111,
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

TEST_P(EORAbsoluteMode, TypicalInstructionExecution)
{
    TypicalInstructionExecution(*this, GetParam());
}

INSTANTIATE_TEST_SUITE_P(ExclusiveOrAbsoluteAtVariousAddresses,
                         EORAbsoluteMode,
                         testing::ValuesIn(EORAbsoluteModeTestValues) );
