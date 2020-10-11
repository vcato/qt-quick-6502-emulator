#include "addressing_mode_helpers.hpp"



struct LSR_Accumulator_Expectations
{
    constexpr LSR_Accumulator_Expectations &accumulator(const uint8_t v) { a = v; return *this; }

    uint8_t  a;
    NZCFlags flags;
};

using LSRAccumulator     = LSR<Accumulator, LSR_Accumulator_Expectations, 2>;
using LSRAccumulatorMode = ParameterizedInstructionExecutorTestFixture<LSRAccumulator>;


static void SetupAffectedOrUsedRegisters(InstructionExecutorTestFixture &fixture, const LSRAccumulator &instruction_param)
{
    fixture.r.a = instruction_param.requirements.initial.a;
    fixture.r.SetFlag(FLAGS6502::N, instruction_param.requirements.initial.flags.n_value.expected_value);
    fixture.r.SetFlag(FLAGS6502::Z, instruction_param.requirements.initial.flags.z_value.expected_value);
    fixture.r.SetFlag(FLAGS6502::C, instruction_param.requirements.initial.flags.c_value.expected_value);
}

template<>
void LoadInstructionIntoMemoryAndSetRegistersToInitialState(      InstructionExecutorTestFixture &fixture,
                                                            const LSRAccumulator                 &instruction_param)
{
    SetupRAMForInstructionsThatHaveNoEffectiveAddress(fixture, instruction_param);
    SetupAffectedOrUsedRegisters(fixture, instruction_param);
}

template<>
void RegistersAreInExpectedState(const Registers &registers,
                                 const LSR_Accumulator_Expectations &expectations)
{
    EXPECT_THAT(registers.a, Eq(expectations.a));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::N), Eq(expectations.flags.n_value.expected_value));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::Z), Eq(expectations.flags.z_value.expected_value));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::C), Eq(expectations.flags.c_value.expected_value));
}

template<>
void MemoryContainsInstruction(const InstructionExecutorTestFixture &fixture,
                               const Instruction<AbstractInstruction_e::LSR, Accumulator> &)
{
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter ), Eq( OpcodeFor(AbstractInstruction_e::LSR, AddressMode_e::Accumulator) ));
}

template<>
void MemoryContainsExpectedComputation(const InstructionExecutorTestFixture &,
                                       const LSRAccumulator                 &)
{
    // No memory is affected
}


static const std::vector<LSRAccumulator> LSRAccumulatorModeTestValues {
LSRAccumulator{
    // Beginning of a page
    Accumulator().address(0x8000),
    LSRAccumulator::Requirements{
        .initial = {
            .a = 0,
            .flags = { } },
        .final = {
            .a = 0,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true },
                .c_value = { .expected_value = false } },
        }}
},
LSRAccumulator{
    // Middle of a page
    Accumulator().address(0x8080),
    LSRAccumulator::Requirements{
        .initial = {
            .a = 0,
            .flags = { } },
        .final = {
            .a = 0,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true },
                .c_value = { .expected_value = false } },
        }}
},
LSRAccumulator{
    // End of a page
    Accumulator().address(0x80FF),
    LSRAccumulator::Requirements{
        .initial = {
            .a = 0,
            .flags = { } },
        .final = {
            .a = 0,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true },
                .c_value = { .expected_value = false } },
        }}
},
LSRAccumulator{
    // Check for Low bit going into carry
    Accumulator().address(0x8000),
    LSRAccumulator::Requirements{
        .initial = {
            .a = 0b01010101,
            .flags = { } },
        .final = {
            .a = 0b00101010,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true } },
        }}
},
LSRAccumulator{
    // Zero is set in highest bit
    Accumulator().address(0x8000),
    LSRAccumulator::Requirements{
        .initial = {
            .a = 0b11111111,
            .flags = { } },
        .final = {
            .a = 0b01111111,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true } },
        }}
}
};

TEST_P(LSRAccumulatorMode, TypicalInstructionExecution)
{
    TypicalInstructionExecution(*this, GetParam());
}

INSTANTIATE_TEST_SUITE_P(LogicalShiftRightAccumulatorAtVariousAddresses,
                         LSRAccumulatorMode,
                         testing::ValuesIn(LSRAccumulatorModeTestValues) );
