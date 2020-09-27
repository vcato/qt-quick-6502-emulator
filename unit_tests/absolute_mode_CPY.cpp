#include "addressing_mode_helpers.hpp"



struct CPY_Absolute_Expectations
{
    uint8_t  y;
    NZCFlags flags;

    uint8_t  operand;
};

using CPYAbsolute     = CPY<Absolute, CPY_Absolute_Expectations, 4>;
using CPYAbsoluteMode = ParameterizedInstructionExecutorTestFixture<CPYAbsolute>;


static void StoreTestValueAtEffectiveAddress(InstructionExecutorTestFixture &fixture, const CPYAbsolute &instruction_param)
{
    fixture.fakeMemory[instruction_param.address.absolute_address       ] = instruction_param.requirements.initial.operand;
}

static void SetupAffectedOrUsedRegisters(InstructionExecutorTestFixture &fixture, const CPYAbsolute &instruction_param)
{
    fixture.r.y = instruction_param.requirements.initial.y;
    fixture.r.SetFlag(FLAGS6502::N, instruction_param.requirements.initial.flags.n_value.expected_value);
    fixture.r.SetFlag(FLAGS6502::Z, instruction_param.requirements.initial.flags.z_value.expected_value);
    fixture.r.SetFlag(FLAGS6502::C, instruction_param.requirements.initial.flags.c_value.expected_value);
}

template<>
void LoadInstructionIntoMemoryAndSetRegistersToInitialState(      InstructionExecutorTestFixture &fixture,
                                                            const CPYAbsolute                    &instruction_param)
{
    SetupRAMForInstructionsThatHaveAnEffectiveAddress(fixture, instruction_param);
    SetupAffectedOrUsedRegisters(fixture, instruction_param);
}

template<>
void RegistersAreInExpectedState(const Registers &registers,
                                 const CPY_Absolute_Expectations &expectations)
{
    EXPECT_THAT(registers.y, Eq(expectations.y));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::N), Eq(expectations.flags.n_value.expected_value));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::Z), Eq(expectations.flags.z_value.expected_value));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::C), Eq(expectations.flags.c_value.expected_value));
}

template<>
void MemoryContainsInstruction(const InstructionExecutorTestFixture &fixture,
                               const Instruction<AbstractInstruction_e::CPY, Absolute> &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter ), Eq( OpcodeFor(AbstractInstruction_e::CPY, AddressMode_e::Absolute) ));
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 1), Eq( fixture.loByteOf(instruction.address.absolute_address) ));
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 2), Eq( fixture.hiByteOf(instruction.address.absolute_address) ));
}

template<>
void MemoryContainsExpectedComputation(const InstructionExecutorTestFixture &,
                                       const CPYAbsolute                    &)
{
    // Memory doesn't change!
    // Only the registers.
}


static const std::vector<CPYAbsolute> CPXAbsoluteModeTestValues {
CPYAbsolute{
    // Beginning of.y page
    // 0 - 0
    Absolute().address(0x0000).value(0xA000),
    CPYAbsolute::Requirements{
        .initial = {
            .y = 0,
            .flags = { },
            .operand = 0 },
        .final = {
            .y = 0,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true },
                .c_value = { .expected_value = true } },
            .operand = 0
        }}
},
CPYAbsolute{
    // Middle of.y page
    // 0 - 0
    Absolute().address(0x0088).value(0xA000),
    CPYAbsolute::Requirements{
        .initial = {
            .y = 0,
            .flags = { },
            .operand = 0 },
        .final = {
            .y = 0,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true },
                .c_value = { .expected_value = true } },
            .operand = 0
        }}
},
CPYAbsolute{
    // End of.y page
    // 0xFF - 0
    Absolute().address(0x00FD).value(0xA000),
    CPYAbsolute::Requirements{
        .initial = {
            .y = 0xFF,
            .flags = { },
            .operand = 0 },
        .final = {
            .y = 0xFF,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true } },
            .operand = 0
        }}
},
CPYAbsolute{
    // Crossing.y page (partial absolute address)
    // 0 - 0xFF
    Absolute().address(0x00FE).value(0xA000),
    CPYAbsolute::Requirements{
        .initial = {
            .y = 0,
            .flags = { },
            .operand = 0xFF },
        .final = {
            .y = 0,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false } },
            .operand = 0xFF
        }}
},
CPYAbsolute{
    // Crossing.y page (entire absolute address)
    // 0 - 0
    Absolute().address(0x00FF).value(0xA000),
    CPYAbsolute::Requirements{
        .initial = {
            .y = 0,
            .flags = { },
            .operand = 0 },
        .final = {
            .y = 0,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true },
                .c_value = { .expected_value = true } },
            .operand = 0
        }}
},
CPYAbsolute{
    // 2 - 3
    // Negative Result
    Absolute().address(0x8000).value(0xA000),
    CPYAbsolute::Requirements{
        .initial = {
            .y = 0x02,
            .flags = { },
            .operand = 0x03 },
        .final = {
            .y = 0x02,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false } },
            .operand = 0x03
        }}
},
CPYAbsolute{
    // 3 - 2
    // Positive Result
    Absolute().address(0x8000).value(0xA000),
    CPYAbsolute::Requirements{
        .initial = {
            .y = 0x03,
            .flags = { },
            .operand = 0x02 },
        .final = {
            .y = 0x03,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true } },
            .operand = 0x02
        }}
}
};

TEST_P(CPYAbsoluteMode, TypicalInstructionExecution)
{
    TypicalInstructionExecution(*this, GetParam());
}

INSTANTIATE_TEST_SUITE_P(CompareXAbsoluteAtVariousAddresses,
                         CPYAbsoluteMode,
                         testing::ValuesIn(CPXAbsoluteModeTestValues) );
