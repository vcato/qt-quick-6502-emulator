#include <gmock/gmock.h>
#include "instruction_checks.hpp"



struct INC_ZeroPageXIndexed_Expectations
{
    uint8_t x;
    NZFlags flags;

    uint8_t operand;
};

using INCZeroPageXIndexed     = INC<ZeroPageXIndexed, INC_ZeroPageXIndexed_Expectations, 6>;
using INCZeroPageXIndexedMode = ParameterizedInstructionExecutorTestFixture<INCZeroPageXIndexed>;


template<>
void LoadInstructionIntoMemoryAndSetRegistersToInitialState(      InstructionExecutorTestFixture &fixture,
                                                            const INCZeroPageXIndexed            &instruction_param)
{
    fixture.loadOpcodeIntoMemory(instruction_param.operation,
                                 AddressMode_e::ZeroPageXIndexed,
                                 instruction_param.address.instruction_address);
    fixture.fakeMemory[instruction_param.address.instruction_address + 1] = instruction_param.address.zero_page_address;

    // Load expected data into memory
    fixture.fakeMemory[ fixture.calculateZeroPageIndexedAddress(instruction_param.address.zero_page_address, instruction_param.requirements.initial.x) ] = instruction_param.requirements.initial.operand;

    // Load appropriate registers
    fixture.r.x = instruction_param.requirements.initial.x;
    fixture.r.SetFlag(FLAGS6502::N, instruction_param.requirements.initial.flags.n_value.expected_value);
    fixture.r.SetFlag(FLAGS6502::Z, instruction_param.requirements.initial.flags.z_value.expected_value);
}

template<>
void RegistersAreInExpectedState(const Registers &registers,
                                 const INC_ZeroPageXIndexed_Expectations &expectations)
{
    EXPECT_THAT(registers.x, Eq(expectations.x));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::N), Eq(expectations.flags.n_value.expected_value));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::Z), Eq(expectations.flags.z_value.expected_value));
}

template<>
void MemoryContainsInstruction(const InstructionExecutorTestFixture &fixture,
                               const Instruction<AbstractInstruction_e::INC, ZeroPageXIndexed> &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter ), Eq( OpcodeFor(AbstractInstruction_e::INC, AddressMode_e::ZeroPageXIndexed) ));
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 1), Eq(instruction.address.zero_page_address));
}

template<>
void MemoryContainsExpectedComputation(const InstructionExecutorTestFixture &fixture,
                                       const INCZeroPageXIndexed            &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( fixture.calculateZeroPageIndexedAddress(instruction.address.zero_page_address, instruction.requirements.initial.x ) ), Eq( instruction.requirements.initial.operand ));
}

template<>
void MemoryContainsExpectedResult(const InstructionExecutorTestFixture &fixture,
                                  const INCZeroPageXIndexed            &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( fixture.calculateZeroPageIndexedAddress( instruction.address.zero_page_address, instruction.requirements.final.x ) ), Eq(instruction.requirements.final.operand));
}


static const std::vector<INCZeroPageXIndexed> INCZeroPageXIndexedModeTestValues {
INCZeroPageXIndexed{
    // Beginning of a page
    ZeroPageXIndexed().address(0xA000).zp_address(6),
    INCZeroPageXIndexed::Requirements{
        .initial = {
            .x = 0,
            .flags = { },
            .operand = 0xFE },
        .final = {
            .x = 0,
            .flags = {
                .n_value = {  .expected_value = true },
                .z_value = {  .expected_value = false } },
            .operand = 0xFF
        }}
},
INCZeroPageXIndexed{
    // Middle of a page
    ZeroPageXIndexed().address(0xA080).zp_address(0x80),
    INCZeroPageXIndexed::Requirements{
        .initial = {
            .x = 0,
            .flags = { },
            .operand = 0xFE },
        .final = {
            .x = 0,
            .flags = {
                .n_value = {  .expected_value = true },
                .z_value = {  .expected_value = false } },
            .operand = 0xFF
        }}
},
INCZeroPageXIndexed{
    // End of a page
    ZeroPageXIndexed().address(0xA0FE).zp_address(0xFF),
    INCZeroPageXIndexed::Requirements{
        .initial = {
            .x = 0,
            .flags = { },
            .operand = 0xFE },
        .final = {
            .x = 0,
            .flags = {
                .n_value = {  .expected_value = true },
                .z_value = {  .expected_value = false } },
            .operand = 0xFF
        }}
},
INCZeroPageXIndexed{
    // Crossing a page
    ZeroPageXIndexed().address(0xA0FF).zp_address(0xCF),
    INCZeroPageXIndexed::Requirements{
        .initial = {
            .x = 0,
            .flags = { },
            .operand = 0xFE },
        .final = {
            .x = 0,
            .flags = {
                .n_value = {  .expected_value = true },
                .z_value = {  .expected_value = false } },
            .operand = 0xFF
        }}
},
INCZeroPageXIndexed{
    // Check for rolling back from 0 to 0xFF
    ZeroPageXIndexed().address(0x8000).zp_address(0x10),
    INCZeroPageXIndexed::Requirements{
        .initial = {
            .x = 0x80,
            .flags = { },
            .operand = 0xFF },
        .final = {
            .x = 0x80,
            .flags = {
                .n_value = {  .expected_value = false },
                .z_value = {  .expected_value = true } },
            .operand = 0x00
        }}
},
INCZeroPageXIndexed{
    // Rolling TO 0
    ZeroPageXIndexed().address(0x8000).zp_address(0xA0),
    INCZeroPageXIndexed::Requirements{
        .initial = {
            .x = 0xFF,
            .flags = { },
            .operand = 0 },
        .final = {
            .x = 0xFF,
            .flags = {
                .n_value = {  .expected_value = false },
                .z_value = {  .expected_value = false } },
            .operand = 1
        }}
},
INCZeroPageXIndexed{
    // Crossing 0x80 (negative to positive)
    // Also, highest index for address of last location in page.
    ZeroPageXIndexed().address(0x8000).zp_address(0xA0),
    INCZeroPageXIndexed::Requirements{
        .initial = {
            .x = 0xFF,
            .flags = {
                .n_value = {  .expected_value = true },
                .z_value = {  .expected_value = true } },
            .operand = 0x7F },
        .final = {
            .x = 0xFF,
            .flags = {
                .n_value = {  .expected_value = true },
                .z_value = {  .expected_value = false } },
            .operand = 0x80
        }}
}
};

TEST_P(INCZeroPageXIndexedMode, TypicalInstructionExecution)
{
    TypicalInstructionExecution(*this, GetParam());
}

INSTANTIATE_TEST_SUITE_P(IncrementMemoryZeroPageXIndexedAtVariousAddresses,
                         INCZeroPageXIndexedMode,
                         testing::ValuesIn(INCZeroPageXIndexedModeTestValues) );
