#include <gmock/gmock.h>
#include "instruction_checks.hpp"



struct EOR_IndirectYIndexed_Expectations
{
    uint16_t address_to_indirect_to;
    uint8_t  a;
    uint8_t  y;
    NZFlags  flags;

    uint8_t  operand;
};

using EORIndirectYIndexed     = EOR<IndirectYIndexed, EOR_IndirectYIndexed_Expectations, 5>;
using EORIndirectYIndexedMode = ParameterizedInstructionExecutorTestFixture<EORIndirectYIndexed>;


template<>
void LoadInstructionIntoMemoryAndSetRegistersToInitialState(      InstructionExecutorTestFixture &fixture,
                                                            const EORIndirectYIndexed            &instruction_param)
{
    fixture.loadOpcodeIntoMemory(instruction_param.operation,
                                 AddressMode_e::IndirectYIndexed,
                                 instruction_param.address.instruction_address);
    fixture.fakeMemory[instruction_param.address.instruction_address + 1] = instruction_param.address.zero_page_address;
    fixture.fakeMemory[instruction_param.address.zero_page_address    ]   = fixture.loByteOf(instruction_param.requirements.initial.address_to_indirect_to);
    fixture.fakeMemory[instruction_param.address.zero_page_address + 1]   = fixture.hiByteOf(instruction_param.requirements.initial.address_to_indirect_to);
    fixture.fakeMemory[instruction_param.requirements.initial.address_to_indirect_to + instruction_param.requirements.initial.y ] = instruction_param.requirements.initial.operand;

    // Load appropriate registers
    fixture.r.a = instruction_param.requirements.initial.a;
    fixture.r.y = instruction_param.requirements.initial.y;
    fixture.r.SetFlag(FLAGS6502::N, instruction_param.requirements.initial.flags.n_value.expected_value);
    fixture.r.SetFlag(FLAGS6502::Z, instruction_param.requirements.initial.flags.z_value.expected_value);
}

template<>
void RegistersAreInExpectedState(const Registers &registers,
                                 const EOR_IndirectYIndexed_Expectations &expectations)
{
    EXPECT_THAT(registers.a, Eq(expectations.a));
    EXPECT_THAT(registers.y, Eq(expectations.y));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::N), Eq(expectations.flags.n_value.expected_value));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::Z), Eq(expectations.flags.z_value.expected_value));
}

template<>
void MemoryContainsInstruction(const InstructionExecutorTestFixture &fixture,
                               const Instruction<AbstractInstruction_e::EOR, IndirectYIndexed> &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter ),     Eq( OpcodeFor(AbstractInstruction_e::EOR, AddressMode_e::IndirectYIndexed) ));
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 1 ), Eq(instruction.address.zero_page_address));
}

template<>
void MemoryContainsExpectedComputation(const InstructionExecutorTestFixture &fixture,
                                       const EORIndirectYIndexed            &instruction)
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
                                             const EORIndirectYIndexed            &instruction)
{
    // Account for a clock tick one greater if a page is crossed
    uint32_t original_address  = fixture.loByteOf(instruction.requirements.initial.address_to_indirect_to);
    uint32_t effective_address = original_address + instruction.requirements.initial.y;
    bool     page_boundary_is_crossed = (fixture.hiByteOf(effective_address) - fixture.hiByteOf(original_address)) > 0x00;
    uint32_t extra_cycle_count = (page_boundary_is_crossed) ? 1 : 0; // If the page is crossed

    EXPECT_THAT(fixture.executor.clock_ticks, Eq(instruction.requirements.cycle_count + extra_cycle_count));
}


static const std::vector<EORIndirectYIndexed> EORIndirectYIndexedModeTestValues {
EORIndirectYIndexed{
    // Beginning of a page
    IndirectYIndexed().address(0x1000).zp_address(0xA0),
    EORIndirectYIndexed::Requirements{
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
                .n_value = {
                    .status_flag = FLAGS6502::N,
                    .expected_value = false },
                .z_value = {
                    .status_flag = FLAGS6502::Z,
                    .expected_value = true } },
            .operand = 0x00
        }}
},
EORIndirectYIndexed{
    // Middle of a page
    IndirectYIndexed().address(0x1088).zp_address(0xA0),
    EORIndirectYIndexed::Requirements{
        .initial = {
            .address_to_indirect_to = 0xA000,
            .a = 0,
            .y = 0,
            .flags = { },
            .operand = 0x00 },
        .final = {
            .address_to_indirect_to = 0xA000,
            .a = 0,
            .y = 0,
            .flags = {
                .n_value = {
                    .status_flag = FLAGS6502::N,
                    .expected_value = false },
                .z_value = {
                    .status_flag = FLAGS6502::Z,
                    .expected_value = true } },
            .operand = 0x00
        }}
},
EORIndirectYIndexed{
    // End of a page
    IndirectYIndexed().address(0x10FE).zp_address(0xA0),
    EORIndirectYIndexed::Requirements{
        .initial = {
            .address_to_indirect_to = 0xA000,
            .a = 0xFF,
            .y = 0xFF,
            .flags = { },
            .operand = 0x00 },
        .final = {
            .address_to_indirect_to = 0xA000,
            .a = 0xFF,
            .y = 0xFF,
            .flags = {
                .n_value = {
                    .status_flag = FLAGS6502::N,
                    .expected_value = true },
                .z_value = {
                    .status_flag = FLAGS6502::Z,
                    .expected_value = false } },
            .operand = 0x00
        }}
},
EORIndirectYIndexed{
    // Crossing a page boundary
    IndirectYIndexed().address(0x10FF).zp_address(0xA0),
    EORIndirectYIndexed::Requirements{
        .initial = {
            .address_to_indirect_to = 0xA000,
            .a = 0xFF,
            .y = 0xFF,
            .flags = { },
            .operand = 0x00 },
        .final = {
            .address_to_indirect_to = 0xA000,
            .a = 0xFF,
            .y = 0xFF,
            .flags = {
                .n_value = {
                    .status_flag = FLAGS6502::N,
                    .expected_value = true },
                .z_value = {
                    .status_flag = FLAGS6502::Z,
                    .expected_value = false } },
            .operand = 0x00
        }}
},
EORIndirectYIndexed{
    // Test individual bits are EOR'd.
    IndirectYIndexed().address(0x1000).zp_address(0xA0),
    EORIndirectYIndexed::Requirements{
        .initial = {
            .address_to_indirect_to = 0xA000,
            .a = 0b10101010,
            .y = 0x02,
            .flags = { },
            .operand = 0b10101010 },
        .final = {
            .address_to_indirect_to = 0xA000,
            .a = 0b00000000,
            .y = 0x02,
            .flags = {
                .n_value = {
                    .status_flag = FLAGS6502::N,
                    .expected_value = false },
                .z_value = {
                    .status_flag = FLAGS6502::Z,
                    .expected_value = true } },
            .operand = 0b10101010
        }}
},
EORIndirectYIndexed{
    // Test individual bits are EOR'd.
    IndirectYIndexed().address(0x1000).zp_address(0xA0),
    EORIndirectYIndexed::Requirements{
        .initial = {
            .address_to_indirect_to = 0xA000,
            .a = 0b01010101,
            .y = 0x80,
            .flags = { },
            .operand = 0b01010101 },
        .final = {
            .address_to_indirect_to = 0xA000,
            .a = 0b00000000,
            .y = 0x80,
            .flags = {
                .n_value = {
                    .status_flag = FLAGS6502::N,
                    .expected_value = false },
                .z_value = {
                    .status_flag = FLAGS6502::Z,
                    .expected_value = true } },
            .operand = 0b01010101
        }}
},
EORIndirectYIndexed{
    // Test individual bits are EOR'd.
    IndirectYIndexed().address(0x1000).zp_address(0xA0),
    EORIndirectYIndexed::Requirements{
        .initial = {
            .address_to_indirect_to = 0xA000,
            .a = 0b10101010,
            .y = 0x7F,
            .flags = { },
            .operand = 0b01010101 },
        .final = {
            .address_to_indirect_to = 0xA000,
            .a = 0b11111111,
            .y = 0x7F,
            .flags = {
                .n_value = {
                    .status_flag = FLAGS6502::N,
                    .expected_value = true },
                .z_value = {
                    .status_flag = FLAGS6502::Z,
                    .expected_value = false } },
            .operand = 0b01010101
        }}
},
};

TEST_P(EORIndirectYIndexedMode, TypicalInstructionExecution)
{
    TypicalInstructionExecution(*this, GetParam());
}

INSTANTIATE_TEST_SUITE_P(ExclusiveOrIndirectYIndexedAtVariousAddresses,
                         EORIndirectYIndexedMode,
                         testing::ValuesIn(EORIndirectYIndexedModeTestValues) );
