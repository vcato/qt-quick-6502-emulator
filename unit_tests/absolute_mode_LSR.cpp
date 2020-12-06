#include "addressing_mode_helpers.hpp"



struct LSR_Absolute_Expectations
{
    NZCFlags flags;
    uint8_t  operand;
};

using LSRAbsolute     = LSR<Absolute, LSR_Absolute_Expectations, 6>;
using LSRAbsoluteMode = ParameterizedInstructionExecutorTestFixture<LSRAbsolute>;


static void StoreTestValueAtEffectiveAddress(InstructionExecutorTestFixture &fixture, const LSRAbsolute &instruction_param)
{
    fixture.fakeMemory[instruction_param.address.absolute_address       ] = instruction_param.requirements.initial.operand;
}

static void SetupAffectedOrUsedRegisters(InstructionExecutorTestFixture &fixture, const LSRAbsolute &instruction_param)
{
    fixture.r.SetFlag(FLAGS6502::N, instruction_param.requirements.initial.flags.n_value.expected_value);
    fixture.r.SetFlag(FLAGS6502::Z, instruction_param.requirements.initial.flags.z_value.expected_value);
    fixture.r.SetFlag(FLAGS6502::C, instruction_param.requirements.initial.flags.c_value.expected_value);
}

template<>
void LoadInstructionIntoMemoryAndSetRegistersToInitialState(      InstructionExecutorTestFixture &fixture,
                                                            const LSRAbsolute                    &instruction_param)
{
    SetupRAMForInstructionsThatHaveAnEffectiveAddress(fixture, instruction_param);
    SetupAffectedOrUsedRegisters(fixture, instruction_param);
}

template<>
void RegistersAreInExpectedState(const Registers &registers,
                                 const LSR_Absolute_Expectations &expectations)
{
    EXPECT_THAT(registers.GetFlag(FLAGS6502::N), Eq(expectations.flags.n_value.expected_value));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::Z), Eq(expectations.flags.z_value.expected_value));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::C), Eq(expectations.flags.c_value.expected_value));
}

template<>
void MemoryContainsInstruction(const InstructionExecutorTestFixture &fixture,
                               const Instruction<AbstractInstruction_e::LSR, Absolute> &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter ), Eq( OpcodeFor(AbstractInstruction_e::LSR, AddressMode_e::Absolute) ));
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 1), Eq( fixture.loByteOf(instruction.address.absolute_address) ));
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 2), Eq( fixture.hiByteOf(instruction.address.absolute_address) ));
}

template<>
void MemoryContainsExpectedComputation(const InstructionExecutorTestFixture &fixture,
                                       const LSRAbsolute                    &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( instruction.address.absolute_address ), Eq( instruction.requirements.initial.operand ));
}

template<>
void MemoryContainsExpectedResult(const InstructionExecutorTestFixture &fixture,
                                  const LSRAbsolute                    &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( instruction.address.absolute_address ), Eq(instruction.requirements.final.operand));
}

static const std::vector<LSRAbsolute> LSRAbsoluteModeTestValues {
LSRAbsolute{
    // Beginning of a page
    Absolute().address(0x4000).value(0xA000),
    LSRAbsolute::Requirements{
        .initial = {
            .flags = { },
            .operand = 0},
        .final = {
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true },
                .c_value = { .expected_value = false } },
            .operand = 0
        }}
},
LSRAbsolute{
    // Middle of a page
    Absolute().address(0x4088).value(0xA000),
    LSRAbsolute::Requirements{
        .initial = {
            .flags = { },
            .operand = 0},
        .final = {
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true },
                .c_value = { .expected_value = false } },
            .operand = 0
        }}
},
LSRAbsolute{
    // End of a page
    Absolute().address(0x40FD).value(0xA000),
    LSRAbsolute::Requirements{
        .initial = {
            .flags = { },
            .operand = 0},
        .final = {
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true },
                .c_value = { .expected_value = false } },
            .operand = 0
        }}
},
LSRAbsolute{
    // Crossing a page (partial absolute address)
    Absolute().address(0x40FE).value(0xA000),
    LSRAbsolute::Requirements{
        .initial = {
            .flags = { },
            .operand = 0},
        .final = {
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true },
                .c_value = { .expected_value = false } },
            .operand = 0
        }}
},
LSRAbsolute{
    // Crossing a page (entire absolute address)
    Absolute().address(0x40FF).value(0xA000),
    LSRAbsolute::Requirements{
        .initial = {
            .flags = { },
            .operand = 0},
        .final = {
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true },
                .c_value = { .expected_value = false } },
            .operand = 0
        }}
},
LSRAbsolute{
    // Check for Low bit going into carry
    Absolute().address(0x8000).value(0xA000),
    LSRAbsolute::Requirements{
        .initial = {
            .flags = { },
            .operand = 0b01010101 },
        .final = {
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true } },
            .operand = 0b00101010
        }}
},
LSRAbsolute{
    // Zero is set in highest bit
    Absolute().address(0x8000).value(0xA000),
    LSRAbsolute::Requirements{
        .initial = {
            .flags = { },
            .operand = 0b11111111 },
        .final = {
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true } },
            .operand = 0b01111111
        }}
}
};

TEST_P(LSRAbsoluteMode, TypicalInstructionExecution)
{
    TypicalInstructionExecution(*this, GetParam());
}

INSTANTIATE_TEST_SUITE_P(LogicalShiftRightAbsoluteAtVariousAddresses,
                         LSRAbsoluteMode,
                         testing::ValuesIn(LSRAbsoluteModeTestValues) );
