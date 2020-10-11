#include "addressing_mode_helpers.hpp"



struct LSR_AbsoluteXIndexed_Expectations
{
    uint8_t x;
    NZCFlags flags;

    uint8_t   operand;
};

using LSRAbsoluteXIndexed     = LSR<AbsoluteXIndexed, LSR_AbsoluteXIndexed_Expectations, 7>;
using LSRAbsoluteXIndexedMode = ParameterizedInstructionExecutorTestFixture<LSRAbsoluteXIndexed>;


static void StoreTestValueAtEffectiveAddress(InstructionExecutorTestFixture &fixture, const LSRAbsoluteXIndexed &instruction_param)
{
    fixture.fakeMemory[instruction_param.address.absolute_address + instruction_param.requirements.final.x ] = instruction_param.requirements.initial.operand;
}

static void SetupAffectedOrUsedRegisters(InstructionExecutorTestFixture &fixture, const LSRAbsoluteXIndexed &instruction_param)
{
    fixture.r.x = instruction_param.requirements.initial.x;
    fixture.r.SetFlag(FLAGS6502::N, instruction_param.requirements.initial.flags.n_value.expected_value);
    fixture.r.SetFlag(FLAGS6502::Z, instruction_param.requirements.initial.flags.z_value.expected_value);
    fixture.r.SetFlag(FLAGS6502::C, instruction_param.requirements.initial.flags.c_value.expected_value);
}

template<>
void LoadInstructionIntoMemoryAndSetRegistersToInitialState(      InstructionExecutorTestFixture &fixture,
                                                            const LSRAbsoluteXIndexed            &instruction_param)
{
    SetupRAMForInstructionsThatHaveAnEffectiveAddress(fixture, instruction_param);
    SetupAffectedOrUsedRegisters(fixture, instruction_param);
}

template<>
void RegistersAreInExpectedState(const Registers &registers,
                                 const LSR_AbsoluteXIndexed_Expectations &expectations)
{
    EXPECT_THAT(registers.GetFlag(FLAGS6502::N), Eq(expectations.flags.n_value.expected_value));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::Z), Eq(expectations.flags.z_value.expected_value));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::C), Eq(expectations.flags.c_value.expected_value));
}

template<>
void MemoryContainsInstruction(const InstructionExecutorTestFixture &fixture,
                               const Instruction<AbstractInstruction_e::LSR, AbsoluteXIndexed> &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter ), Eq( OpcodeFor(AbstractInstruction_e::LSR, AddressMode_e::AbsoluteXIndexed) ));

    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 1), Eq( fixture.loByteOf(instruction.address.absolute_address) ));
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 2), Eq( fixture.hiByteOf(instruction.address.absolute_address) ));
}

template<>
void MemoryContainsExpectedComputation(const InstructionExecutorTestFixture &fixture,
                                       const LSRAbsoluteXIndexed            &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( instruction.address.absolute_address + instruction.requirements.final.x ), Eq( instruction.requirements.initial.operand ));
}

template<>
void MemoryContainsExpectedResult(const InstructionExecutorTestFixture &fixture,
                                  const LSRAbsoluteXIndexed            &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( instruction.address.absolute_address + instruction.requirements.final.x ), Eq(instruction.requirements.final.operand));
}


static const std::vector<LSRAbsoluteXIndexed> LSRAbsoluteXIndexedModeTestValues {
LSRAbsoluteXIndexed{
    // Beginning of a page
    AbsoluteXIndexed().address(0xA000).value(0xC000),
    LSRAbsoluteXIndexed::Requirements{
        .initial = {
            .x = 0,
            .flags = { },
            .operand = 0},
        .final = {
            .x = 0,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true },
                .c_value = { .expected_value = false } },
            .operand = 0
        }}
},
LSRAbsoluteXIndexed{
    // Middle of a page
    AbsoluteXIndexed().address(0xA088).value(0xC000),
    LSRAbsoluteXIndexed::Requirements{
        .initial = {
            .x = 5,
            .flags = { },
            .operand = 0},
        .final = {
            .x = 5,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true },
                .c_value = { .expected_value = false } },
            .operand = 0
        }}
},
LSRAbsoluteXIndexed{
    // End of a page
    AbsoluteXIndexed().address(0xA0FD).value(0xC000),
    LSRAbsoluteXIndexed::Requirements{
        .initial = {
            .x = 5,
            .flags = { },
            .operand = 0},
        .final = {
            .x = 5,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true },
                .c_value = { .expected_value = false } },
            .operand = 0
        }}
},
LSRAbsoluteXIndexed{
    // Crossing a page (partial absolute address)
    AbsoluteXIndexed().address(0xA0FE).value(0xC000),
    LSRAbsoluteXIndexed::Requirements{
        .initial = {
            .x = 5,
            .flags = { },
            .operand = 0},
        .final = {
            .x = 5,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true },
                .c_value = { .expected_value = false } },
            .operand = 0
        }}
},
LSRAbsoluteXIndexed{
    // Crossing a page (entire absolute address)
    AbsoluteXIndexed().address(0xA0FF).value(0xC000),
    LSRAbsoluteXIndexed::Requirements{
        .initial = {
            .x = 5,
            .flags = { },
            .operand = 0},
        .final = {
            .x = 5,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true },
                .c_value = { .expected_value = false } },
            .operand = 0
        }}
},
LSRAbsoluteXIndexed{
    // Check for Low bit going into carry
    AbsoluteXIndexed().address(0x8000).value(0xA000),
    LSRAbsoluteXIndexed::Requirements{
        .initial = {
            .x = 0,
            .flags = { },
            .operand = 0b01010101 },
        .final = {
            .x = 0,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true } },
            .operand = 0b00101010
        }}
},
LSRAbsoluteXIndexed{
    // Zero is set in highest bit
    AbsoluteXIndexed().address(0x8000).value(0xA000),
    LSRAbsoluteXIndexed::Requirements{
        .initial = {
            .x = 0x80,
            .flags = { },
            .operand = 0b11111111 },
        .final = {
            .x = 0x80,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true } },
            .operand = 0b01111111
        }}
},
LSRAbsoluteXIndexed{
    // Zero is set in highest bit (max x index)
    AbsoluteXIndexed().address(0x80FF).value(0xA0FF),
    LSRAbsoluteXIndexed::Requirements{
        .initial = {
            .x = 0xFF,
            .flags = { },
            .operand = 0b11111111 },
        .final = {
            .x = 0xFF,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true } },
            .operand = 0b01111111
        }}
}
};

TEST_P(LSRAbsoluteXIndexedMode, TypicalInstructionExecution)
{
    TypicalInstructionExecution(*this, GetParam());
}

INSTANTIATE_TEST_SUITE_P(ArithmeticShiftLeftAbsoluteXIndexedAtVariousAddresses,
                         LSRAbsoluteXIndexedMode,
                         testing::ValuesIn(LSRAbsoluteXIndexedModeTestValues) );
