#include <gmock/gmock.h>
#include "instruction_checks.hpp"



struct LDY_AbsoluteXIndexed_Expectations
{
    constexpr LDY_AbsoluteXIndexed_Expectations &accumulator(const uint8_t v) { y = v; return *this; }

    uint8_t y;
    uint8_t x;
    NZFlags flags;
};

using LDYAbsoluteXIndexed     = LDY<AbsoluteXIndexed, LDY_AbsoluteXIndexed_Expectations, 4>;
using LDYAbsoluteXIndexedMode = ParameterizedInstructionExecutorTestFixture<LDYAbsoluteXIndexed>;


template<>
void LoadInstructionIntoMemoryAndSetRegistersToInitialState(      InstructionExecutorTestFixture &fixture,
                                                            const LDYAbsoluteXIndexed            &instruction_param)
{
    fixture.loadOpcodeIntoMemory(instruction_param.operation,
                                 AddressMode_e::AbsoluteXIndexed,
                                 instruction_param.address.instruction_address);
    fixture.fakeMemory[instruction_param.address.instruction_address + 1] = fixture.loByteOf(instruction_param.address.absolute_address);
    fixture.fakeMemory[instruction_param.address.instruction_address + 2] = fixture.hiByteOf(instruction_param.address.absolute_address);
    fixture.fakeMemory[instruction_param.address.absolute_address + instruction_param.requirements.final.x ] = instruction_param.requirements.final.y;

    // Load appropriate registers
    fixture.r.y = instruction_param.requirements.initial.y;
    fixture.r.x = instruction_param.requirements.initial.x;
    fixture.r.SetFlag(FLAGS6502::N, instruction_param.requirements.initial.flags.n_value.expected_value);
    fixture.r.SetFlag(FLAGS6502::Z, instruction_param.requirements.initial.flags.z_value.expected_value);
}

template<>
void RegistersAreInExpectedState(const Registers &registers,
                                 const LDY_AbsoluteXIndexed_Expectations &expectations)
{
    EXPECT_THAT(registers.y, Eq(expectations.y));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::N), Eq(expectations.flags.n_value.expected_value));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::Z), Eq(expectations.flags.z_value.expected_value));
}

template<>
void MemoryContainsInstruction(const InstructionExecutorTestFixture &fixture,
                               const Instruction<AbstractInstruction_e::LDY, AbsoluteXIndexed> &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter ), Eq( OpcodeFor(AbstractInstruction_e::LDY, AddressMode_e::AbsoluteXIndexed) ));

    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 1), Eq( fixture.loByteOf(instruction.address.absolute_address) ));
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 2), Eq( fixture.hiByteOf(instruction.address.absolute_address) ));
}

template<>
void MemoryContainsExpectedComputation(const InstructionExecutorTestFixture &fixture,
                                       const LDYAbsoluteXIndexed            &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( instruction.address.absolute_address + instruction.requirements.final.x ), Eq( instruction.requirements.final.y ));
}


static const std::vector<LDYAbsoluteXIndexed> LDYAbsoluteXIndexedModeTestValues {
LDYAbsoluteXIndexed{
    // Beginning of a page
    AbsoluteXIndexed().address(0x0000).value(0xA000),
    LDYAbsoluteXIndexed::Requirements{
        .initial = {
            .y = 0,
            .x = 0,
            .flags = { }},
        .final = {
            .y = 6,
            .x = 0,
            .flags = { }
        }}
},
LDYAbsoluteXIndexed{
    // Middle of a page
    AbsoluteXIndexed().address(0x0088).value(0xA000),
    LDYAbsoluteXIndexed::Requirements{
        .initial = {
            .y = 0,
            .x = 5,
            .flags = { }},
        .final = {
            .y = 6,
            .x = 5,
            .flags = { }
        }}
},
LDYAbsoluteXIndexed{
    // End of a page
    AbsoluteXIndexed().address(0x00FD).value(0xA000),
    LDYAbsoluteXIndexed::Requirements{
        .initial = {
            .y = 0,
            .x = 0,
            .flags = { }},
        .final = {
            .y = 6,
            .x = 0,
            .flags = { }
        }}
},
LDYAbsoluteXIndexed{
    // Crossing a page (partial absolute address)
    AbsoluteXIndexed().address(0x00FE).value(0xA000),
    LDYAbsoluteXIndexed::Requirements{
        .initial = {
            .y = 0,
            .x = 0,
            .flags = { }},
        .final = {
            .y = 6,
            .x = 0,
            .flags = { }
        }}
},
LDYAbsoluteXIndexed{
    // Crossing a page (entire absolute address)
    AbsoluteXIndexed().address(0x00FF).value(0xA000),
    LDYAbsoluteXIndexed::Requirements{
        .initial = {
            .y = 0,
            .x = 0,
            .flags = { }},
        .final = {
            .y = 6,
            .x = 0,
            .flags = { }
        }}
},
LDYAbsoluteXIndexed{
    // Loading a zero affects the Z flag
    AbsoluteXIndexed().address(0x8000).value(0xA000),
    LDYAbsoluteXIndexed::Requirements{
        .initial = {
            .y = 0xA0,
            .x = 0,
            .flags = { }},
        .final = {
            .y = 0,
            .x = 0,
            .flags = {
                .n_value = {
                    .status_flag = FLAGS6502::N,
                    .expected_value = false },
                .z_value = {
                    .status_flag = FLAGS6502::Z,
                    .expected_value = true } }
        }}
},
LDYAbsoluteXIndexed{
    // Loading a negative affects the N flag
    AbsoluteXIndexed().address(0x8000).value(0xA000),
    LDYAbsoluteXIndexed::Requirements{
        .initial = {
            .y = 0x10,
            .x = 0,
            .flags = { }},
        .final = {
            .y = 0xFF,
            .x = 0,
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

TEST_P(LDYAbsoluteXIndexedMode, TypicalInstructionExecution)
{
    TypicalInstructionExecution(*this, GetParam());
}

INSTANTIATE_TEST_SUITE_P(LoadAbsoluteXIndexedAtVariousAddresses,
                         LDYAbsoluteXIndexedMode,
                         testing::ValuesIn(LDYAbsoluteXIndexedModeTestValues) );
