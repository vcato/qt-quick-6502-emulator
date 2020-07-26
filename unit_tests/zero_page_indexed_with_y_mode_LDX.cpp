#include <gmock/gmock.h>
#include "instruction_checks.hpp"



struct LDX_ZeroPageYIndexed_Expectations
{
    uint8_t x;
    uint8_t y;
    NZFlags flags;
};

using LDXZeroPageYIndexed     = LDX<ZeroPageYIndexed, LDX_ZeroPageYIndexed_Expectations, 4>;
using LDXZeroPageYIndexedMode = ParameterizedInstructionExecutorTestFixture<LDXZeroPageYIndexed>;


template<>
void LoadInstructionIntoMemoryAndSetRegistersToInitialState(      InstructionExecutorTestFixture &fixture,
                                                            const LDXZeroPageYIndexed            &instruction_param)
{
    fixture.loadOpcodeIntoMemory(instruction_param.operation,
                                 AddressMode_e::ZeroPageYIndexed,
                                 instruction_param.address.instruction_address);
    fixture.fakeMemory[instruction_param.address.instruction_address + 1] = instruction_param.address.zero_page_address;

    // Load expected data into memory
    fixture.fakeMemory[ fixture.calculateZeroPageIndexedAddress(instruction_param.address.zero_page_address, instruction_param.requirements.initial.y) ] = instruction_param.requirements.final.x;

    // Load appropriate registers
    fixture.r.x = instruction_param.requirements.initial.x;
    fixture.r.y = instruction_param.requirements.initial.y;
    fixture.r.SetFlag(FLAGS6502::N, instruction_param.requirements.initial.flags.n_value.expected_value);
    fixture.r.SetFlag(FLAGS6502::Z, instruction_param.requirements.initial.flags.z_value.expected_value);
}

template<>
void RegistersAreInExpectedState(const Registers &registers,
                                 const LDX_ZeroPageYIndexed_Expectations &expectations)
{
    EXPECT_THAT(registers.x, Eq(expectations.x));
    EXPECT_THAT(registers.y, Eq(expectations.y));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::N), Eq(expectations.flags.n_value.expected_value));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::Z), Eq(expectations.flags.z_value.expected_value));
}

template<>
void MemoryContainsInstruction(const InstructionExecutorTestFixture &fixture,
                               const Instruction<AbstractInstruction_e::LDX, ZeroPageYIndexed> &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter ), Eq( OpcodeFor(AbstractInstruction_e::LDX, AddressMode_e::ZeroPageYIndexed) ));
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 1), Eq(instruction.address.zero_page_address));
}

template<>
void MemoryContainsExpectedComputation(const InstructionExecutorTestFixture &fixture,
                                       const LDXZeroPageYIndexed            &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( instruction.address.zero_page_address + instruction.requirements.initial.y), Eq(instruction.requirements.final.x));
}


static const std::vector<LDXZeroPageYIndexed> LDXZeroPageYIndexedModeTestValues {
LDXZeroPageYIndexed{
    // Beginning of a page
    ZeroPageYIndexed().address(0x0000).zp_address(6),
    LDXZeroPageYIndexed::Requirements{
        .initial = {
            .x = 0,
            .y = 0,
            .flags = { }},
        .final = {
            .x = 10,
            .y = 0,
            .flags = { }
        }}
},
LDXZeroPageYIndexed{
    // One before the end of a page
    ZeroPageYIndexed().address(0x00FE).zp_address(6),
    LDXZeroPageYIndexed::Requirements{
        .initial = {
            .x = 0,
            .y = 0,
            .flags = { }},
        .final = {
            .x = 11,
            .y = 0,
            .flags = { }
        }}
},
LDXZeroPageYIndexed{
    // Crossing a page boundary
    ZeroPageYIndexed().address(0x00FF).zp_address(6),
    LDXZeroPageYIndexed::Requirements{
        .initial = {
            .x = 0,
            .y = 0,
            .flags = { }},
        .final = {
            .x = 32,
            .y = 0,
            .flags = { }
        }}
},
LDXZeroPageYIndexed{
    // Loading a zero affects the Z flag
    ZeroPageYIndexed().address(0x8000).zp_address(16),
    LDXZeroPageYIndexed::Requirements{
        .initial = {
            .x = 6,
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
LDXZeroPageYIndexed{
    // Loading a negative affects the N flag
    ZeroPageYIndexed().address(0x8000).zp_address(0xFF),
    LDXZeroPageYIndexed::Requirements{
        .initial = {
            .x = 0,
            .y = 0,
            .flags = { }},
        .final = {
            .x = 0x80,
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

TEST_P(LDXZeroPageYIndexedMode, TypicalInstructionExecution)
{
    TypicalInstructionExecution(*this, GetParam());
}

INSTANTIATE_TEST_SUITE_P(LoadZeroPageXIndexedAtVariousAddresses,
                         LDXZeroPageYIndexedMode,
                         testing::ValuesIn(LDXZeroPageYIndexedModeTestValues) );
