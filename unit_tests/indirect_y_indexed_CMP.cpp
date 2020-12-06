#include "addressing_mode_helpers.hpp"



struct CMP_IndirectYIndexed_Expectations
{
    uint16_t address_to_indirect_to;
    uint8_t  a;
    uint8_t  y;
    NZCFlags flags;

    uint8_t  operand;
};

using CMPIndirectYIndexed     = CMP<IndirectYIndexed, CMP_IndirectYIndexed_Expectations, 5>;
using CMPIndirectYIndexedMode = ParameterizedInstructionExecutorTestFixture<CMPIndirectYIndexed>;


static void StoreTestValueAtEffectiveAddress(InstructionExecutorTestFixture &fixture, const CMPIndirectYIndexed &instruction_param)
{
    fixture.fakeMemory[instruction_param.requirements.initial.address_to_indirect_to + instruction_param.requirements.initial.y ] = instruction_param.requirements.initial.operand;
}

static void SetupAffectedOrUsedRegisters(InstructionExecutorTestFixture &fixture, const CMPIndirectYIndexed &instruction_param)
{
    fixture.r.a = instruction_param.requirements.initial.a;
    fixture.r.y = instruction_param.requirements.initial.y;
    fixture.r.SetFlag(FLAGS6502::N, instruction_param.requirements.initial.flags.n_value.expected_value);
    fixture.r.SetFlag(FLAGS6502::Z, instruction_param.requirements.initial.flags.z_value.expected_value);
    fixture.r.SetFlag(FLAGS6502::C, instruction_param.requirements.initial.flags.c_value.expected_value);
}

template<>
void LoadInstructionIntoMemoryAndSetRegistersToInitialState(      InstructionExecutorTestFixture &fixture,
                                                            const CMPIndirectYIndexed            &instruction_param)
{
    SetupRAMForInstructionsThatHaveAnIndirectedEffectiveAddress(fixture, instruction_param);
    SetupAffectedOrUsedRegisters(fixture, instruction_param);
}

template<>
void RegistersAreInExpectedState(const Registers &registers,
                                 const CMP_IndirectYIndexed_Expectations &expectations)
{
    EXPECT_THAT(registers.a, Eq(expectations.a));
    EXPECT_THAT(registers.y, Eq(expectations.y));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::N), Eq(expectations.flags.n_value.expected_value));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::Z), Eq(expectations.flags.z_value.expected_value));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::C), Eq(expectations.flags.c_value.expected_value));
}

template<>
void MemoryContainsInstruction(const InstructionExecutorTestFixture &fixture,
                               const Instruction<AbstractInstruction_e::CMP, IndirectYIndexed> &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter ),     Eq( OpcodeFor(AbstractInstruction_e::CMP, AddressMode_e::IndirectYIndexed) ));
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 1 ), Eq(instruction.address.zero_page_address));
}

template<>
void MemoryContainsExpectedComputation(const InstructionExecutorTestFixture &fixture,
                                       const CMPIndirectYIndexed            &instruction)
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
                                             const CMPIndirectYIndexed            &instruction)
{
    // Account for a clock tick one greater if a page is crossed
    uint32_t original_address  = fixture.loByteOf(instruction.requirements.initial.address_to_indirect_to);
    uint32_t effective_address = original_address + instruction.requirements.initial.y;
    bool     page_boundary_is_crossed = (fixture.hiByteOf(effective_address) - fixture.hiByteOf(original_address)) > 0x00;
    uint32_t extra_cycle_count = (page_boundary_is_crossed) ? 1 : 0; // If the page is crossed

    EXPECT_THAT(fixture.executor.clock_ticks, Eq(instruction.requirements.cycle_count + extra_cycle_count));
}


static const std::vector<CMPIndirectYIndexed> CMPIndirectYIndexedModeTestValues {
CMPIndirectYIndexed{
    // Beginning of a page
    // 0 - 0
    IndirectYIndexed().address(0x1000).zp_address(0xA0),
    CMPIndirectYIndexed::Requirements{
        .initial = {
            .address_to_indirect_to = 0xA000,
            .a = 0,
            .y = 0,
            .flags = { },
            .operand = 0 },
        .final = {
            .address_to_indirect_to = 0xA000,
            .a = 0,
            .y = 0,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true },
                .c_value = { .expected_value = true } },
            .operand = 0
        }}
},
CMPIndirectYIndexed{
    // Middle of a page
    // 0 - 0
    IndirectYIndexed().address(0x1088).zp_address(0xA0),
    CMPIndirectYIndexed::Requirements{
        .initial = {
            .address_to_indirect_to = 0xA000,
            .a = 0,
            .y = 0,
            .flags = { },
            .operand = 0 },
        .final = {
            .address_to_indirect_to = 0xA000,
            .a = 0,
            .y = 0,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true },
                .c_value = { .expected_value = true } },
            .operand = 0
        }}
},
CMPIndirectYIndexed{
    // End of a page
    // 0 - 0
    IndirectYIndexed().address(0x10FE).zp_address(0xA0),
    CMPIndirectYIndexed::Requirements{
        .initial = {
            .address_to_indirect_to = 0xA000,
            .a = 0,
            .y = 0,
            .flags = { },
            .operand = 0 },
        .final = {
            .address_to_indirect_to = 0xA000,
            .a = 0,
            .y = 0,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true },
                .c_value = { .expected_value = true } },
            .operand = 0
        }}
},
CMPIndirectYIndexed{
    // Crossing a page boundary
    // 0 - 0xFF
    IndirectYIndexed().address(0x10FF).zp_address(0xA0),
    CMPIndirectYIndexed::Requirements{
        .initial = {
            .address_to_indirect_to = 0xA000,
            .a = 0,
            .y = 0,
            .flags = { },
            .operand = 0xFF },
        .final = {
            .address_to_indirect_to = 0xA000,
            .a = 0,
            .y = 0,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false } },
            .operand = 0xFF
        }}
},
CMPIndirectYIndexed{
    // 2 - 3
    // Negative Result
    IndirectYIndexed().address(0x1000).zp_address(0xA0),
    CMPIndirectYIndexed::Requirements{
        .initial = {
            .address_to_indirect_to = 0xA000,
            .a = 2,
            .y = 0x10,
            .flags = { },
            .operand = 3 },
        .final = {
            .address_to_indirect_to = 0xA000,
            .a = 2,
            .y = 0x10,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false } },
            .operand = 3
        }}
},
CMPIndirectYIndexed{
    // 3 - 2
    // Positive Result
    IndirectYIndexed().address(0x1000).zp_address(0xA0),
    CMPIndirectYIndexed::Requirements{
        .initial = {
            .address_to_indirect_to = 0xA0FF,
            .a = 3,
            .y = 0x10,
            .flags = { },
            .operand = 2 },
        .final = {
            .address_to_indirect_to = 0xA0FF,
            .a = 3,
            .y = 0x10,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true } },
            .operand = 2
        }}
}
};

TEST_P(CMPIndirectYIndexedMode, TypicalInstructionExecution)
{
    TypicalInstructionExecution(*this, GetParam());
}

INSTANTIATE_TEST_SUITE_P(CompareAccumulatorIndirectYIndexedAtVariousAddresses,
                         CMPIndirectYIndexedMode,
                         testing::ValuesIn(CMPIndirectYIndexedModeTestValues) );
