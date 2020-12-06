#include "addressing_mode_helpers.hpp"



struct AND_IndirectYIndexed_Expectations
{
    uint16_t address_to_indirect_to;
    uint8_t  a;
    uint8_t  y;
    NZFlags  flags;

    uint8_t  operand;
};

using ANDIndirectYIndexed     = AND<IndirectYIndexed, AND_IndirectYIndexed_Expectations, 5>;
using ANDIndirectYIndexedMode = ParameterizedInstructionExecutorTestFixture<ANDIndirectYIndexed>;


static void StoreTestValueAtEffectiveAddress(InstructionExecutorTestFixture &fixture, const ANDIndirectYIndexed &instruction_param)
{
    fixture.fakeMemory[instruction_param.requirements.initial.address_to_indirect_to + instruction_param.requirements.initial.y ] = instruction_param.requirements.initial.operand;
}

static void SetupAffectedOrUsedRegisters(InstructionExecutorTestFixture &fixture, const ANDIndirectYIndexed &instruction_param)
{
    fixture.r.a = instruction_param.requirements.initial.a;
    fixture.r.y = instruction_param.requirements.initial.y;
    fixture.r.SetFlag(FLAGS6502::N, instruction_param.requirements.initial.flags.n_value.expected_value);
    fixture.r.SetFlag(FLAGS6502::Z, instruction_param.requirements.initial.flags.z_value.expected_value);
}

template<>
void LoadInstructionIntoMemoryAndSetRegistersToInitialState(      InstructionExecutorTestFixture &fixture,
                                                            const ANDIndirectYIndexed            &instruction_param)
{
    SetupRAMForInstructionsThatHaveAnIndirectedEffectiveAddress(fixture, instruction_param);
    SetupAffectedOrUsedRegisters(fixture, instruction_param);
}

template<>
void RegistersAreInExpectedState(const Registers &registers,
                                 const AND_IndirectYIndexed_Expectations &expectations)
{
    EXPECT_THAT(registers.a, Eq(expectations.a));
    EXPECT_THAT(registers.y, Eq(expectations.y));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::N), Eq(expectations.flags.n_value.expected_value));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::Z), Eq(expectations.flags.z_value.expected_value));
}

template<>
void MemoryContainsInstruction(const InstructionExecutorTestFixture &fixture,
                               const Instruction<AbstractInstruction_e::AND, IndirectYIndexed> &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter ),     Eq( OpcodeFor(AbstractInstruction_e::AND, AddressMode_e::IndirectYIndexed) ));
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 1 ), Eq(instruction.address.zero_page_address));
}

template<>
void MemoryContainsExpectedComputation(const InstructionExecutorTestFixture &fixture,
                                       const ANDIndirectYIndexed            &instruction)
{
    const uint8_t zero_page_address_to_load_from = instruction.address.zero_page_address;
    const uint8_t value_to_and                   = instruction.requirements.initial.operand;
    const auto    address_stored_in_zero_page    = instruction.requirements.initial.address_to_indirect_to;
    const uint8_t y_register                     = instruction.requirements.initial.y;

    EXPECT_THAT(fixture.fakeMemory.at( zero_page_address_to_load_from ),    Eq( fixture.loByteOf(address_stored_in_zero_page) ));
    EXPECT_THAT(fixture.fakeMemory.at( zero_page_address_to_load_from + 1), Eq( fixture.hiByteOf(address_stored_in_zero_page) ));
    EXPECT_THAT(fixture.fakeMemory.at( address_stored_in_zero_page + y_register ), Eq(value_to_and));
}

template<>
void InstructionExecutedInExpectedClockTicks(const InstructionExecutorTestFixture &fixture,
                                             const ANDIndirectYIndexed            &instruction)
{
    // Account for a clock tick one greater if a page is crossed
    uint32_t original_address  = fixture.loByteOf(instruction.requirements.initial.address_to_indirect_to);
    uint32_t effective_address = original_address + instruction.requirements.initial.y;
    bool     page_boundary_is_crossed = (fixture.hiByteOf(effective_address) - fixture.hiByteOf(original_address)) > 0x00;
    uint32_t extra_cycle_count = (page_boundary_is_crossed) ? 1 : 0; // If the page is crossed

    EXPECT_THAT(fixture.executor.clock_ticks, Eq(instruction.requirements.cycle_count + extra_cycle_count));
}


static const std::vector<ANDIndirectYIndexed> ANDIndirectYIndexedModeTestValues {
ANDIndirectYIndexed{
    // Beginning of a page
    IndirectYIndexed().address(0x1000).zp_address(0xA0),
    ANDIndirectYIndexed::Requirements{
        .initial = {
            .address_to_indirect_to = 0xA000,
            .a = 0,
            .y = 0,
            .flags = { },
            .operand = 0},
        .final = {
            .address_to_indirect_to = 0xA000,
            .a = 0,
            .y = 0,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true } },
            .operand = 0x00
        }}
},
ANDIndirectYIndexed{
    // Middle of a page
    IndirectYIndexed().address(0x1088).zp_address(0xA0),
    ANDIndirectYIndexed::Requirements{
        .initial = {
            .address_to_indirect_to = 0xA000,
            .a = 0,
            .y = 0,
            .flags = { },
            .operand = 0x00},
        .final = {
            .address_to_indirect_to = 0xA000,
            .a = 0,
            .y = 0,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true } },
            .operand = 0x00
        }}
},
ANDIndirectYIndexed{
    // End of a page
    IndirectYIndexed().address(0x10FE).zp_address(0xA0),
    ANDIndirectYIndexed::Requirements{
        .initial = {
            .address_to_indirect_to = 0xA000,
            .a = 0,
            .y = 0,
            .flags = { },
            .operand = 0x00},
        .final = {
            .address_to_indirect_to = 0xA000,
            .a = 0,
            .y = 0,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true } },
            .operand = 0x00
        }}
},
ANDIndirectYIndexed{
    // Crossing a page boundary
    IndirectYIndexed().address(0x10FF).zp_address(0xA0),
    ANDIndirectYIndexed::Requirements{
        .initial = {
            .address_to_indirect_to = 0xA000,
            .a = 0,
            .y = 0,
            .flags = { },
            .operand = 0x00},
        .final = {
            .address_to_indirect_to = 0xA000,
            .a = 0,
            .y = 0,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true } },
            .operand = 0x00
        }}
},
ANDIndirectYIndexed{
    // Check for masking out the high bit
    IndirectYIndexed().address(0x1000).zp_address(0xA0),
    ANDIndirectYIndexed::Requirements{
        .initial = {
            .address_to_indirect_to = 0xA000,
            .a = 0xFF,
            .y = 4,
            .flags = { },
            .operand = 0x80},
        .final = {
            .address_to_indirect_to = 0xA000,
            .a = 0x80,
            .y = 4,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false } },
            .operand = 0x80
        }}
},
ANDIndirectYIndexed{
    // Use alternating bits for a zero result
    IndirectYIndexed().address(0x1000).zp_address(0xA0),
    ANDIndirectYIndexed::Requirements{
        .initial = {
            .address_to_indirect_to = 0xA0FF,
            .a = 0b10101010,
            .y = 0,
            .flags = { },
            .operand = 0b01010101 },
        .final = {
            .address_to_indirect_to = 0xA0FF,
            .a = 0x00,
            .y = 0,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true } },
            .operand = 0b01010101
        }}
},
ANDIndirectYIndexed{
    // Use the same bits for the same result
    IndirectYIndexed().address(0x1000).zp_address(0xA0),
    ANDIndirectYIndexed::Requirements{
        .initial = {
            .address_to_indirect_to = 0xA080,
            .a = 0b10101010,
            .y = 16,
            .flags = { },
            .operand = 0b10101010 },
        .final = {
            .address_to_indirect_to = 0xA080,
            .a = 0b10101010,
            .y = 16,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false } },
            .operand = 0b10101010
        }}
},
ANDIndirectYIndexed{
    // Use the same bits for the same result (not the same pattern as before)
    IndirectYIndexed().address(0x1000).zp_address(0xA0),
    ANDIndirectYIndexed::Requirements{
        .initial = {
            .address_to_indirect_to = 0xA0FF,
            .a = 0b01010101,
            .y = 0xFF,
            .flags = { },
            .operand = 0b01010101 },
        .final = {
            .address_to_indirect_to = 0xA0FF,
            .a = 0b01010101,
            .y = 0xFF,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false } },
            .operand = 0b01010101
        }}
}
};

TEST_P(ANDIndirectYIndexedMode, TypicalInstructionExecution)
{
    TypicalInstructionExecution(*this, GetParam());
}

INSTANTIATE_TEST_SUITE_P(AndIndirectYIndexedAtVariousAddresses,
                         ANDIndirectYIndexedMode,
                         testing::ValuesIn(ANDIndirectYIndexedModeTestValues) );
