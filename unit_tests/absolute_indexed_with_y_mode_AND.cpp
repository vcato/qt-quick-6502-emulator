#include "addressing_mode_helpers.hpp"



struct AND_AbsoluteYIndexed_Expectations
{
    constexpr AND_AbsoluteYIndexed_Expectations &accumulator(const uint8_t v) { a = v; return *this; }

    uint8_t a;
    uint8_t y;
    NZFlags flags;

    uint8_t   operand;
};

using ANDAbsoluteYIndexed     = AND<AbsoluteYIndexed, AND_AbsoluteYIndexed_Expectations, 4>;
using ANDAbsoluteYIndexedMode = ParameterizedInstructionExecutorTestFixture<ANDAbsoluteYIndexed>;


static void StoreTestValueAtEffectiveAddress(InstructionExecutorTestFixture &fixture, const ANDAbsoluteYIndexed &instruction_param)
{
    fixture.fakeMemory[instruction_param.address.absolute_address + instruction_param.requirements.initial.y ] = instruction_param.requirements.initial.operand;
}

static void SetupAffectedOrUsedRegisters(InstructionExecutorTestFixture &fixture, const ANDAbsoluteYIndexed &instruction_param)
{
    fixture.r.a = instruction_param.requirements.initial.a;
    fixture.r.y = instruction_param.requirements.initial.y;
    fixture.r.SetFlag(FLAGS6502::N, instruction_param.requirements.initial.flags.n_value.expected_value);
    fixture.r.SetFlag(FLAGS6502::Z, instruction_param.requirements.initial.flags.z_value.expected_value);
}

template<>
void LoadInstructionIntoMemoryAndSetRegistersToInitialState(      InstructionExecutorTestFixture &fixture,
                                                            const ANDAbsoluteYIndexed            &instruction_param)
{
    SetupRAMForInstructionsThatHaveAnEffectiveAddress(fixture, instruction_param);
    SetupAffectedOrUsedRegisters(fixture, instruction_param);
}

template<>
void RegistersAreInExpectedState(const Registers &registers,
                                 const AND_AbsoluteYIndexed_Expectations &expectations)
{
    EXPECT_THAT(registers.a, Eq(expectations.a));
    EXPECT_THAT(registers.y, Eq(expectations.y));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::N), Eq(expectations.flags.n_value.expected_value));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::Z), Eq(expectations.flags.z_value.expected_value));
}

template<>
void MemoryContainsInstruction(const InstructionExecutorTestFixture &fixture,
                               const Instruction<AbstractInstruction_e::AND, AbsoluteYIndexed> &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter ), Eq( OpcodeFor(AbstractInstruction_e::AND, AddressMode_e::AbsoluteYIndexed) ));

    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 1), Eq( fixture.loByteOf(instruction.address.absolute_address) ));
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 2), Eq( fixture.hiByteOf(instruction.address.absolute_address) ));
}

template<>
void MemoryContainsExpectedComputation(const InstructionExecutorTestFixture &fixture,
                                       const ANDAbsoluteYIndexed            &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( instruction.address.absolute_address + instruction.requirements.initial.y ), Eq( instruction.requirements.initial.operand ));
}

template<>
void InstructionExecutedInExpectedClockTicks(const InstructionExecutorTestFixture &fixture,
                                             const ANDAbsoluteYIndexed            &instruction)
{
    // Account for a clock tick one greater if a page is crossed
    uint32_t original_address  = fixture.loByteOf(instruction.address.absolute_address);
    uint32_t effective_address = original_address + instruction.requirements.initial.y;
    bool     page_boundary_is_crossed = effective_address > 0xFF;
    uint32_t extra_cycle_count = (page_boundary_is_crossed) ? 1 : 0; // If the page is crossed

    EXPECT_THAT(fixture.executor.clock_ticks, Eq(instruction.requirements.cycle_count + extra_cycle_count));
}


static const std::vector<ANDAbsoluteYIndexed> ANDAbsoluteYIndexedModeTestValues {
ANDAbsoluteYIndexed{
    // Beginning of a page
    AbsoluteYIndexed().address(0x0000).value(0xA000),
    ANDAbsoluteYIndexed::Requirements{
        .initial = {
            .a = 0,
            .y = 0,
            .flags = { },
            .operand = 0x00 },
        .final = {
            .a = 0,
            .y = 0,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true } },
            .operand = 0x00
        }}
},
ANDAbsoluteYIndexed{
    // Middle of a page
    AbsoluteYIndexed().address(0x0088).value(0xA000),
    ANDAbsoluteYIndexed::Requirements{
        .initial = {
            .a = 0,
            .y = 0,
            .flags = { },
            .operand = 0x00 },
        .final = {
            .a = 0,
            .y = 0,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true } },
            .operand = 0x00
        }}
},
ANDAbsoluteYIndexed{
    // End of a page
    AbsoluteYIndexed().address(0x00FD).value(0xA000),
    ANDAbsoluteYIndexed::Requirements{
        .initial = {
            .a = 0,
            .y = 0,
            .flags = { },
            .operand = 0x00 },
        .final = {
            .a = 0,
            .y = 0,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true } },
            .operand = 0x00
        }}
},
ANDAbsoluteYIndexed{
    // Crossing a page (partial absolute address)
    AbsoluteYIndexed().address(0x00FE).value(0xA000),
    ANDAbsoluteYIndexed::Requirements{
        .initial = {
            .a = 0,
            .y = 0,
            .flags = { },
            .operand = 0x00 },
        .final = {
            .a = 0,
            .y = 0,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true } },
            .operand = 0x00
        }}
},
ANDAbsoluteYIndexed{
    // Crossing a page (entire absolute address)
    AbsoluteYIndexed().address(0x00FF).value(0xA000),
    ANDAbsoluteYIndexed::Requirements{
        .initial = {
            .a = 0,
            .y = 0,
            .flags = { },
            .operand = 0x00 },
        .final = {
            .a = 0,
            .y = 0,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true } },
            .operand = 0x00
        }}
},
ANDAbsoluteYIndexed{
    // Check for masking out the high bit
    AbsoluteYIndexed().address(0x8000).value(0xA000),
    ANDAbsoluteYIndexed::Requirements{
        .initial = {
            .a = 0xFF,
            .y = 4,
            .flags = { },
            .operand = 0x80 },
        .final = {
            .a = 0x80,
            .y = 4,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false } },
            .operand = 0x80
        }}
},
ANDAbsoluteYIndexed{
    // Use alternating bits for a zero result
    AbsoluteYIndexed().address(0x8000).value(0xA0FF),
    ANDAbsoluteYIndexed::Requirements{
        .initial = {
            .a = 0b10101010,
            .y = 0,
            .flags = { },
            .operand = 0b01010101 },
        .final = {
            .a = 0x00,
            .y = 0,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true } },
            .operand = 0b01010101
        }}
},
ANDAbsoluteYIndexed{
    // Use the same bits for the same result
    AbsoluteYIndexed().address(0x8000).value(0xA080),
    ANDAbsoluteYIndexed::Requirements{
        .initial = {
            .a = 0b10101010,
            .y = 16,
            .flags = { },
            .operand = 0b10101010 },
        .final = {
            .a = 0b10101010,
            .y = 16,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false } },
            .operand = 0b10101010
        }}
},
ANDAbsoluteYIndexed{
    // Use the same bits for the same result (not the same pattern as before)
    AbsoluteYIndexed().address(0x8000).value(0xA0FF),
    ANDAbsoluteYIndexed::Requirements{
        .initial = {
            .a = 0b01010101,
            .y = 0xFF,
            .flags = { },
            .operand = 0b01010101 },
        .final = {
            .a = 0b01010101,
            .y = 0xFF,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false } },
            .operand = 0b01010101
        }}
}
};

TEST_P(ANDAbsoluteYIndexedMode, TypicalInstructionExecution)
{
    TypicalInstructionExecution(*this, GetParam());
}

INSTANTIATE_TEST_SUITE_P(AndAbsoluteYIndexedAtVariousAddresses,
                         ANDAbsoluteYIndexedMode,
                         testing::ValuesIn(ANDAbsoluteYIndexedModeTestValues) );
