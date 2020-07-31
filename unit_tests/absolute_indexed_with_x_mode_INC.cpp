#include <gmock/gmock.h>
#include "instruction_checks.hpp"



struct INC_AbsoluteXIndexed_Expectations
{
    uint8_t x;
    NZFlags flags;

    uint8_t operand;
};

using INCAbsoluteXIndexed     = INC<AbsoluteXIndexed, INC_AbsoluteXIndexed_Expectations, 7>;
using INCAbsoluteXIndexedMode = ParameterizedInstructionExecutorTestFixture<INCAbsoluteXIndexed>;


template<>
void LoadInstructionIntoMemoryAndSetRegistersToInitialState(      InstructionExecutorTestFixture &fixture,
                                                            const INCAbsoluteXIndexed            &instruction_param)
{
    fixture.loadOpcodeIntoMemory(instruction_param.operation,
                                 AddressMode_e::AbsoluteXIndexed,
                                 instruction_param.address.instruction_address);
    fixture.fakeMemory[instruction_param.address.instruction_address + 1] = fixture.loByteOf(instruction_param.address.absolute_address);
    fixture.fakeMemory[instruction_param.address.instruction_address + 2] = fixture.hiByteOf(instruction_param.address.absolute_address);
    fixture.fakeMemory[instruction_param.address.absolute_address + instruction_param.requirements.initial.x ] = instruction_param.requirements.initial.operand;

    // Load appropriate registers
    fixture.r.x = instruction_param.requirements.initial.x;
    fixture.r.SetFlag(FLAGS6502::N, instruction_param.requirements.initial.flags.n_value.expected_value);
    fixture.r.SetFlag(FLAGS6502::Z, instruction_param.requirements.initial.flags.z_value.expected_value);
}

template<>
void RegistersAreInExpectedState(const Registers &registers,
                                 const INC_AbsoluteXIndexed_Expectations &expectations)
{
    EXPECT_THAT(registers.x, Eq(expectations.x));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::N), Eq(expectations.flags.n_value.expected_value));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::Z), Eq(expectations.flags.z_value.expected_value));
}

template<>
void MemoryContainsInstruction(const InstructionExecutorTestFixture &fixture,
                               const Instruction<AbstractInstruction_e::INC, AbsoluteXIndexed> &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter ), Eq( OpcodeFor(AbstractInstruction_e::INC, AddressMode_e::AbsoluteXIndexed) ));

    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 1), Eq( fixture.loByteOf(instruction.address.absolute_address) ));
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 2), Eq( fixture.hiByteOf(instruction.address.absolute_address) ));
}

template<>
void MemoryContainsExpectedComputation(const InstructionExecutorTestFixture &fixture,
                                       const INCAbsoluteXIndexed            &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( instruction.address.absolute_address + instruction.requirements.initial.x ), Eq( instruction.requirements.initial.operand ));
}

template<>
void MemoryContainsExpectedResult(const InstructionExecutorTestFixture &fixture,
                                  const INCAbsoluteXIndexed            &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( instruction.address.absolute_address + instruction.requirements.final.x ), Eq(instruction.requirements.final.operand));
}


static const std::vector<INCAbsoluteXIndexed> INCAbsoluteXIndexedModeTestValues {
INCAbsoluteXIndexed{
    // Beginning of a page
    AbsoluteXIndexed().address(0xA000).value(0xB000),
    INCAbsoluteXIndexed::Requirements{
        .initial = {
            .x = 0,
            .flags = { },
            .operand = 0xFE },
        .final = {
            .x = 0,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false } },
            .operand = 0xFF
        }}
},
INCAbsoluteXIndexed{
    // Middle of a page
    AbsoluteXIndexed().address(0xA080).value(0xB080),
    INCAbsoluteXIndexed::Requirements{
        .initial = {
            .x = 0,
            .flags = { },
            .operand = 0xFE },
        .final = {
            .x = 0,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false } },
            .operand = 0xFF
        }}
},
INCAbsoluteXIndexed{
    // End of a page
    AbsoluteXIndexed().address(0xA0FD).value(0xB0FE),
    INCAbsoluteXIndexed::Requirements{
        .initial = {
            .x = 0,
            .flags = { },
            .operand = 0xFE },
        .final = {
            .x = 0,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false } },
            .operand = 0xFF
        }}
},
INCAbsoluteXIndexed{
    // Crossing a page (partial parameter)
    AbsoluteXIndexed().address(0xA0FE).value(0xB0FE),
    INCAbsoluteXIndexed::Requirements{
        .initial = {
            .x = 0,
            .flags = { },
            .operand = 0xFE },
        .final = {
            .x = 0,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false } },
            .operand = 0xFF
        }}
},
INCAbsoluteXIndexed{
    // Crossing a page (entire parameter)
    AbsoluteXIndexed().address(0xA0FF).value(0xB0FE),
    INCAbsoluteXIndexed::Requirements{
        .initial = {
            .x = 0,
            .flags = { },
            .operand = 0xFE },
        .final = {
            .x = 0,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false } },
            .operand = 0xFF
        }}
},
INCAbsoluteXIndexed{
    // Check for rolling back from 0 to 0xFF
    AbsoluteXIndexed().address(0xA000).value(0xB000),
    INCAbsoluteXIndexed::Requirements{
        .initial = {
            .x = 0x80,
            .flags = { },
            .operand = 0xFF },
        .final = {
            .x = 0x80,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true } },
            .operand = 0x00
        }}
},
INCAbsoluteXIndexed{
    // Rolling TO 0
    AbsoluteXIndexed().address(0xA000).value(0xB000),
    INCAbsoluteXIndexed::Requirements{
        .initial = {
            .x = 0xFF,
            .flags = { },
            .operand = 0 },
        .final = {
            .x = 0xFF,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false } },
            .operand = 1
        }}
},
INCAbsoluteXIndexed{
    // Crossing 0x80 (negative to positive)
    // Also, highest index for address of last location in page.
    AbsoluteXIndexed().address(0xA000).value(0xB0FF),
    INCAbsoluteXIndexed::Requirements{
        .initial = {
            .x = 0xFF,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true } },
            .operand = 0x7F },
        .final = {
            .x = 0xFF,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false } },
            .operand = 0x80
        }}
}
};

TEST_P(INCAbsoluteXIndexedMode, TypicalInstructionExecution)
{
    TypicalInstructionExecution(*this, GetParam());
}

INSTANTIATE_TEST_SUITE_P(IncrementMemoryAbsoluteXIndexedAtVariousAddresses,
                         INCAbsoluteXIndexedMode,
                         testing::ValuesIn(INCAbsoluteXIndexedModeTestValues) );
