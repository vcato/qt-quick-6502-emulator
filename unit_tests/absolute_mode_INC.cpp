#include <gmock/gmock.h>
#include "instruction_checks.hpp"



struct INC_Absolute_Expectations
{
    NZFlags flags;

    uint8_t operand;
};

using INCAbsolute     = INC<Absolute, INC_Absolute_Expectations, 6>;
using INCAbsoluteMode = ParameterizedInstructionExecutorTestFixture<INCAbsolute>;


template<>
void LoadInstructionIntoMemoryAndSetRegistersToInitialState(      InstructionExecutorTestFixture &fixture,
                                                            const INCAbsolute                    &instruction_param)
{
    fixture.loadOpcodeIntoMemory(instruction_param.operation,
                                 AddressMode_e::Absolute,
                                 instruction_param.address.instruction_address);
    fixture.fakeMemory[instruction_param.address.instruction_address + 1] = fixture.loByteOf(instruction_param.address.absolute_address);
    fixture.fakeMemory[instruction_param.address.instruction_address + 2] = fixture.hiByteOf(instruction_param.address.absolute_address);
    fixture.fakeMemory[instruction_param.address.absolute_address       ] = instruction_param.requirements.initial.operand;

    // Load appropriate registers
    fixture.r.SetFlag(FLAGS6502::N, instruction_param.requirements.initial.flags.n_value.expected_value);
    fixture.r.SetFlag(FLAGS6502::Z, instruction_param.requirements.initial.flags.z_value.expected_value);
}

template<>
void RegistersAreInExpectedState(const Registers &registers,
                                 const INC_Absolute_Expectations &expectations)
{
    EXPECT_THAT(registers.GetFlag(FLAGS6502::N), Eq(expectations.flags.n_value.expected_value));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::Z), Eq(expectations.flags.z_value.expected_value));
}

template<>
void MemoryContainsInstruction(const InstructionExecutorTestFixture &fixture,
                               const Instruction<AbstractInstruction_e::INC, Absolute> &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter ), Eq( OpcodeFor(AbstractInstruction_e::INC, AddressMode_e::Absolute) ));
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 1), Eq( fixture.loByteOf(instruction.address.absolute_address) ));
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 2), Eq( fixture.hiByteOf(instruction.address.absolute_address) ));
}

template<>
void MemoryContainsExpectedComputation(const InstructionExecutorTestFixture &fixture,
                                       const INCAbsolute                    &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( instruction.address.absolute_address ), Eq( instruction.requirements.initial.operand ));
}

template<>
void MemoryContainsExpectedResult(const InstructionExecutorTestFixture &fixture,
                                  const INCAbsolute                    &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( instruction.address.absolute_address ), Eq(instruction.requirements.final.operand));
}

static const std::vector<INCAbsolute> INCAbsoluteModeTestValues {
INCAbsolute{
    // Beginning of a page
    Absolute().address(0x4000).value(0xA000),
    INCAbsolute::Requirements{
        .initial = {
            .flags = { },
            .operand = 0xFE },
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
INCAbsolute{
    // Middle of a page
    Absolute().address(0x4080).value(0xA000),
    INCAbsolute::Requirements{
        .initial = {
            .flags = { },
            .operand = 0xFE },
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
INCAbsolute{
    // End of a page
    Absolute().address(0x40FD).value(0xA000),
    INCAbsolute::Requirements{
        .initial = {
            .flags = { },
            .operand = 0xFE },
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
INCAbsolute{
    // Crossing a page (partial absolute address)
    Absolute().address(0x40FE).value(0xA000),
    INCAbsolute::Requirements{
        .initial = {
            .flags = { },
            .operand = 0xFE },
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
INCAbsolute{
    // Crossing a page (entire absolute address)
    Absolute().address(0x40FF).value(0xA000),
    INCAbsolute::Requirements{
        .initial = {
            .flags = { },
            .operand = 0xFE },
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
INCAbsolute{
    // Check for rolling back from 0 to 0xFF
    Absolute().address(0x8000).value(0xA000),
    INCAbsolute::Requirements{
        .initial = {
            .flags = { },
            .operand = 0xFF },
        .final = {
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
INCAbsolute{
    // Rolling TO 0
    Absolute().address(0x8000).value(0xA000),
    INCAbsolute::Requirements{
        .initial = {
            .flags = { },
            .operand = 0 },
        .final = {
            .flags = {
                .n_value = {
                    .status_flag = FLAGS6502::N,
                    .expected_value = false },
                .z_value = {
                    .status_flag = FLAGS6502::Z,
                    .expected_value = false } },
            .operand = 1
        }}
},
INCAbsolute{
    // Crossing 0x80 (negative to positive)
    Absolute().address(0x8000).value(0xA000),
    INCAbsolute::Requirements{
        .initial = {
            .flags = {
                .n_value = {
                    .status_flag = FLAGS6502::N,
                    .expected_value = false },
                .z_value = {
                    .status_flag = FLAGS6502::Z,
                    .expected_value = true } },
            .operand = 0x7F },
        .final = {
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
INCAbsolute{
    // Rolling TO 0x80
    Absolute().address(0x8000).value(0xA000),
    INCAbsolute::Requirements{
        .initial = {
            .flags = {
                .n_value = {
                    .status_flag = FLAGS6502::N,
                    .expected_value = false },
                .z_value = {
                    .status_flag = FLAGS6502::Z,
                    .expected_value = true } },
            .operand = 0x80 },
        .final = {
            .flags = {
                .n_value = {
                    .status_flag = FLAGS6502::N,
                    .expected_value = true },
                .z_value = {
                    .status_flag = FLAGS6502::Z,
                    .expected_value = false } },
            .operand = 0x81
        }}
},
};

TEST_P(INCAbsoluteMode, TypicalInstructionExecution)
{
    TypicalInstructionExecution(*this, GetParam());
}

INSTANTIATE_TEST_SUITE_P(IncrementMemoryAbsoluteAtVariousAddresses,
                         INCAbsoluteMode,
                         testing::ValuesIn(INCAbsoluteModeTestValues) );
