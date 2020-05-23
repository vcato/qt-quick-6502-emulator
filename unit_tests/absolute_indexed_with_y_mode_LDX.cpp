#include <gmock/gmock.h>
#include "instruction_checks.hpp"



struct LDX_AbsoluteYIndexed_Expectations
{
    uint8_t x;
    uint8_t y;
    NZFlags flags;
};

using LDXAbsoluteYIndexed     = LDX<AbsoluteYIndexed, LDX_AbsoluteYIndexed_Expectations, 4>;
using LDXAbsoluteYIndexedMode = ParameterizedInstructionExecutorTestFixture<LDXAbsoluteYIndexed>;


template<>
void LoadInstructionIntoMemoryAndSetRegistersToInitialState(      InstructionExecutorTestFixture &fixture,
                                                            const LDXAbsoluteYIndexed            &instruction_param)
{
    fixture.loadOpcodeIntoMemory(instruction_param.operation,
                                 AddressMode_e::AbsoluteYIndexed,
                                 instruction_param.address.instruction_address);
    fixture.fakeMemory[instruction_param.address.instruction_address + 1] = fixture.loByteOf(instruction_param.address.absolute_address);
    fixture.fakeMemory[instruction_param.address.instruction_address + 2] = fixture.hiByteOf(instruction_param.address.absolute_address);
    fixture.fakeMemory[instruction_param.address.absolute_address + instruction_param.requirements.final.y ] = instruction_param.requirements.final.x;

    // Load appropriate registers
    fixture.r.x = instruction_param.requirements.initial.x;
    fixture.r.y = instruction_param.requirements.initial.y;
    fixture.r.SetFlag(FLAGS6502::N, instruction_param.requirements.initial.flags.n_value.expected_value);
    fixture.r.SetFlag(FLAGS6502::Z, instruction_param.requirements.initial.flags.z_value.expected_value);
}

template<>
void RegistersAreInExpectedState(const Registers &registers,
                                 const LDX_AbsoluteYIndexed_Expectations &expectations)
{
    EXPECT_THAT(registers.x, Eq(expectations.x));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::N), Eq(expectations.flags.n_value.expected_value));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::Z), Eq(expectations.flags.z_value.expected_value));
}

template<>
void MemoryContainsInstruction(const InstructionExecutorTestFixture &fixture,
                               const Instruction<AbstractInstruction_e::LDX, AbsoluteYIndexed> &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter ), Eq( OpcodeFor(AbstractInstruction_e::LDX, AddressMode_e::AbsoluteYIndexed) ));
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 1), Eq( fixture.loByteOf(instruction.address.absolute_address) ));
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 2), Eq( fixture.hiByteOf(instruction.address.absolute_address) ));
}

template<>
void MemoryContainsExpectedComputation(const InstructionExecutorTestFixture &fixture,
                                       const LDXAbsoluteYIndexed            &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( instruction.address.absolute_address + instruction.requirements.final.y ), Eq( instruction.requirements.final.x ));
}


static const std::vector<LDXAbsoluteYIndexed> LDXAbsoluteYIndexedModeTestValues {
LDXAbsoluteYIndexed{
    // Beginning of a page
    AbsoluteYIndexed().address(0x0000).value(0xA000),
    LDXAbsoluteYIndexed::Requirements{
        .initial = {
            .x = 0,
            .y = 0,
            .flags = { }},
        .final = {
            .x = 6,
            .y = 0,
            .flags = { }
        }}
},
LDXAbsoluteYIndexed{
    // Middle of a page
    AbsoluteYIndexed().address(0x0088).value(0xA000),
    LDXAbsoluteYIndexed::Requirements{
        .initial = {
            .x = 0,
            .y = 5,
            .flags = { }},
        .final = {
            .x = 6,
            .y = 5,
            .flags = { }
        }}
},
LDXAbsoluteYIndexed{
    // End of a page
    AbsoluteYIndexed().address(0x00FD).value(0xA000),
    LDXAbsoluteYIndexed::Requirements{
        .initial = {
            .x = 0,
            .y = 0,
            .flags = { }},
        .final = {
            .x = 6,
            .y = 0,
            .flags = { }
        }}
},
LDXAbsoluteYIndexed{
    // Crossing a page (partial absolute address)
    AbsoluteYIndexed().address(0x00FE).value(0xA000),
    LDXAbsoluteYIndexed::Requirements{
        .initial = {
            .x = 0,
            .y = 0,
            .flags = { }},
        .final = {
            .x = 6,
            .y = 0,
            .flags = { }
        }}
},
LDXAbsoluteYIndexed{
    // Crossing a page (entire absolute address)
    AbsoluteYIndexed().address(0x00FF).value(0xA000),
    LDXAbsoluteYIndexed::Requirements{
        .initial = {
            .x = 0,
            .y = 0,
            .flags = { }},
        .final = {
            .x = 6,
            .y = 0,
            .flags = { }
        }}
},
LDXAbsoluteYIndexed{
    // Loading a zero affects the Z flag
    AbsoluteYIndexed().address(0x8000).value(0xA000),
    LDXAbsoluteYIndexed::Requirements{
        .initial = {
            .x = 0xA0,
            .y = 0,
            .flags = { }},
        .final = {
            .x = 0,
            .y = 0,
            .flags = {
                .n_value = {
                    .status_flag = FLAGS6502::N,
                    .expected_value = false },
                .z_value = {
                    .status_flag = FLAGS6502::Z,
                    .expected_value = true } }
        }}
},
LDXAbsoluteYIndexed{
    // Loading a negative affects the N flag
    AbsoluteYIndexed().address(0x8000).value(0xA000),
    LDXAbsoluteYIndexed::Requirements{
        .initial = {
            .x = 0x10,
            .y = 0,
            .flags = { }},
        .final = {
            .x = 0xFF,
            .y = 0,
            .flags = {
                .n_value = {
                    .status_flag = FLAGS6502::N,
                    .expected_value = true },
                .z_value = {
                    .status_flag = FLAGS6502::Z,
                    .expected_value = false } }
        }}
}
};

TEST_P(LDXAbsoluteYIndexedMode, TypicalInstructionExecution)
{
    TypicalInstructionExecution(*this, GetParam());
}

INSTANTIATE_TEST_SUITE_P(LoadAbsoluteYIndexedAtVariousAddresses,
                         LDXAbsoluteYIndexedMode,
                         testing::ValuesIn(LDXAbsoluteYIndexedModeTestValues) );
