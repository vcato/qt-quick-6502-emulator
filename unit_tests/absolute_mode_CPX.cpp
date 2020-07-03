#include <gmock/gmock.h>
#include "instruction_checks.hpp"



struct CPX_Absolute_Expectations
{
    uint8_t  x;
    NZCFlags flags;

    uint8_t  operand;
};

using CPXAbsolute     = CPX<Absolute, CPX_Absolute_Expectations, 4>;
using CPXAbsoluteMode = ParameterizedInstructionExecutorTestFixture<CPXAbsolute>;


template<>
void LoadInstructionIntoMemoryAndSetRegistersToInitialState(      InstructionExecutorTestFixture &fixture,
                                                            const CPXAbsolute                    &instruction_param)
{
    fixture.loadOpcodeIntoMemory(instruction_param.operation,
                                 AddressMode_e::Absolute,
                                 instruction_param.address.instruction_address);
    fixture.fakeMemory[instruction_param.address.instruction_address + 1] = fixture.loByteOf(instruction_param.address.absolute_address);
    fixture.fakeMemory[instruction_param.address.instruction_address + 2] = fixture.hiByteOf(instruction_param.address.absolute_address);
    fixture.fakeMemory[instruction_param.address.absolute_address       ] = instruction_param.requirements.initial.operand;

    // Load appropriate registers
    fixture.r.x = instruction_param.requirements.initial.x;
    fixture.r.SetFlag(FLAGS6502::N, instruction_param.requirements.initial.flags.n_value.expected_value);
    fixture.r.SetFlag(FLAGS6502::Z, instruction_param.requirements.initial.flags.z_value.expected_value);
    fixture.r.SetFlag(FLAGS6502::C, instruction_param.requirements.initial.flags.c_value.expected_value);
}

template<>
void RegistersAreInExpectedState(const Registers &registers,
                                 const CPX_Absolute_Expectations &expectations)
{
    EXPECT_THAT(registers.x, Eq(expectations.x));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::N), Eq(expectations.flags.n_value.expected_value));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::Z), Eq(expectations.flags.z_value.expected_value));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::C), Eq(expectations.flags.c_value.expected_value));
}

template<>
void MemoryContainsInstruction(const InstructionExecutorTestFixture &fixture,
                               const Instruction<AbstractInstruction_e::CPX, Absolute> &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter ), Eq( OpcodeFor(AbstractInstruction_e::CPX, AddressMode_e::Absolute) ));
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 1), Eq( fixture.loByteOf(instruction.address.absolute_address) ));
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 2), Eq( fixture.hiByteOf(instruction.address.absolute_address) ));
}

template<>
void MemoryContainsExpectedComputation(const InstructionExecutorTestFixture &,
                                       const CPXAbsolute                    &)
{
    // Memory doesn't change!
    // Only the registers.
}


static const std::vector<CPXAbsolute> CPXAbsoluteModeTestValues {
CPXAbsolute{
    // Beginning of.x page
    // 0 - 0
    Absolute().address(0x0000).value(0xA000),
    CPXAbsolute::Requirements{
        .initial = {
            .x = 0,
            .flags = { },
            .operand = 0 },
        .final = {
            .x = 0,
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
CPXAbsolute{
    // Middle of.x page
    // 0 - 0
    Absolute().address(0x0088).value(0xA000),
    CPXAbsolute::Requirements{
        .initial = {
            .x = 0,
            .flags = { },
            .operand = 0 },
        .final = {
            .x = 0,
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
CPXAbsolute{
    // End of.x page
    // 0xFF - 0
    Absolute().address(0x00FD).value(0xA000),
    CPXAbsolute::Requirements{
        .initial = {
            .x = 0xFF,
            .flags = { },
            .operand = 0 },
        .final = {
            .x = 0xFF,
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
CPXAbsolute{
    // Crossing.x page (partial absolute address)
    // 0 - 0xFF
    Absolute().address(0x00FE).value(0xA000),
    CPXAbsolute::Requirements{
        .initial = {
            .x = 0,
            .flags = { },
            .operand = 0xFF },
        .final = {
            .x = 0,
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
CPXAbsolute{
    // Crossing.x page (entire absolute address)
    // 0 - 0
    Absolute().address(0x00FF).value(0xA000),
    CPXAbsolute::Requirements{
        .initial = {
            .x = 0,
            .flags = { },
            .operand = 0 },
        .final = {
            .x = 0,
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
CPXAbsolute{
    // 2 - 3
    // Negative Result
    Absolute().address(0x8000).value(0xA000),
    CPXAbsolute::Requirements{
        .initial = {
            .x = 0x02,
            .flags = { },
            .operand = 0x03 },
        .final = {
            .x = 0x02,
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
CPXAbsolute{
    // 3 - 2
    // Positive Result
    Absolute().address(0x8000).value(0xA000),
    CPXAbsolute::Requirements{
        .initial = {
            .x = 0x03,
            .flags = { },
            .operand = 0x02 },
        .final = {
            .x = 0x03,
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

TEST_P(CPXAbsoluteMode, TypicalInstructionExecution)
{
    TypicalInstructionExecution(*this, GetParam());
}

INSTANTIATE_TEST_SUITE_P(CompareXAbsoluteAtVariousAddresses,
                         CPXAbsoluteMode,
                         testing::ValuesIn(CPXAbsoluteModeTestValues) );
