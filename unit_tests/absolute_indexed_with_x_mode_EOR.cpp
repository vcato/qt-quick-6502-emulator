#include <gmock/gmock.h>
#include "instruction_checks.hpp"



struct EOR_AbsoluteXIndexed_Expectations
{
    constexpr EOR_AbsoluteXIndexed_Expectations &accumulator(const uint8_t v) { a = v; return *this; }

    uint8_t a;
    uint8_t x;
    NZFlags flags;

    uint8_t operand;
};

using EORAbsoluteXIndexed     = EOR<AbsoluteXIndexed, EOR_AbsoluteXIndexed_Expectations, 4>;
using EORAbsoluteXIndexedMode = ParameterizedInstructionExecutorTestFixture<EORAbsoluteXIndexed>;


template<>
void LoadInstructionIntoMemoryAndSetRegistersToInitialState(      InstructionExecutorTestFixture &fixture,
                                                            const EORAbsoluteXIndexed            &instruction_param)
{
    fixture.loadOpcodeIntoMemory(instruction_param.operation,
                                 AddressMode_e::AbsoluteXIndexed,
                                 instruction_param.address.instruction_address);
    fixture.fakeMemory[instruction_param.address.instruction_address + 1] = fixture.loByteOf(instruction_param.address.absolute_address);
    fixture.fakeMemory[instruction_param.address.instruction_address + 2] = fixture.hiByteOf(instruction_param.address.absolute_address);
    fixture.fakeMemory[instruction_param.address.absolute_address + instruction_param.requirements.initial.x ] = instruction_param.requirements.initial.operand;

    // Load appropriate registers
    fixture.r.a = instruction_param.requirements.initial.a;
    fixture.r.x = instruction_param.requirements.initial.x;
    fixture.r.SetFlag(FLAGS6502::N, instruction_param.requirements.initial.flags.n_value.expected_value);
    fixture.r.SetFlag(FLAGS6502::Z, instruction_param.requirements.initial.flags.z_value.expected_value);
}

template<>
void RegistersAreInExpectedState(const Registers &registers,
                                 const EOR_AbsoluteXIndexed_Expectations &expectations)
{
    EXPECT_THAT(registers.a, Eq(expectations.a));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::N), Eq(expectations.flags.n_value.expected_value));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::Z), Eq(expectations.flags.z_value.expected_value));
}

template<>
void MemoryContainsInstruction(const InstructionExecutorTestFixture &fixture,
                               const Instruction<AbstractInstruction_e::EOR, AbsoluteXIndexed> &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter ), Eq( OpcodeFor(AbstractInstruction_e::EOR, AddressMode_e::AbsoluteXIndexed) ));

    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 1), Eq( fixture.loByteOf(instruction.address.absolute_address) ));
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 2), Eq( fixture.hiByteOf(instruction.address.absolute_address) ));
}

template<>
void MemoryContainsExpectedComputation(const InstructionExecutorTestFixture &fixture,
                                       const EORAbsoluteXIndexed            &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( instruction.address.absolute_address + instruction.requirements.initial.x ), Eq( instruction.requirements.initial.operand ));
}

template<>
void InstructionExecutedInExpectedClockTicks(const InstructionExecutorTestFixture &fixture,
                                             const EORAbsoluteXIndexed            &instruction)
{
    // Account for a clock tick one greater if a page is crossed
    uint32_t original_address  = fixture.loByteOf(instruction.address.absolute_address);
    uint32_t effective_address = original_address + instruction.requirements.initial.x;
    bool     page_boundary_is_crossed = effective_address > 0xFF;
    uint32_t extra_cycle_count = (page_boundary_is_crossed) ? 1 : 0; // If the page is crossed

    EXPECT_THAT(fixture.executor.clock_ticks, Eq(instruction.requirements.cycle_count + extra_cycle_count));
}


static const std::vector<EORAbsoluteXIndexed> EORAbsoluteXIndexedModeTestValues {
EORAbsoluteXIndexed{
    // Beginning of a page
    AbsoluteXIndexed().address(0x0000).value(0xA000),
    EORAbsoluteXIndexed::Requirements{
        .initial = {
            .a = 0,
            .x = 0,
            .flags = { },
            .operand = 0 },
        .final = {
            .a = 0,
            .x = 0,
            .flags = {
                .n_value = {
                    .status_flag = FLAGS6502::N,
                    .expected_value = false },
                .z_value = {
                    .status_flag = FLAGS6502::Z,
                    .expected_value = true } },
            .operand = 0
        }}
},
EORAbsoluteXIndexed{
    // Middle of a page
    AbsoluteXIndexed().address(0x0080).value(0xA000),
    EORAbsoluteXIndexed::Requirements{
        .initial = {
            .a = 0,
            .x = 0,
            .flags = { },
            .operand = 0 },
        .final = {
            .a = 0,
            .x = 0,
            .flags = {
                .n_value = {
                    .status_flag = FLAGS6502::N,
                    .expected_value = false },
                .z_value = {
                    .status_flag = FLAGS6502::Z,
                    .expected_value = true } },
            .operand = 0
        }}
},
EORAbsoluteXIndexed{
    // End of a page
    AbsoluteXIndexed().address(0x00FD).value(0xA000),
    EORAbsoluteXIndexed::Requirements{
        .initial = {
            .a = 0xFF,
            .x = 0x0F,
            .flags = { },
            .operand = 0x00 },
        .final = {
            .a = 0xFF,
            .x = 0x0F,
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
EORAbsoluteXIndexed{
    // Crossing a page (partial absolute address)
    AbsoluteXIndexed().address(0x00FE).value(0xA000),
    EORAbsoluteXIndexed::Requirements{
        .initial = {
            .a = 0xFF,
            .x = 0x0F,
            .flags = { },
            .operand = 0x00 },
        .final = {
            .a = 0xFF,
            .x = 0x0F,
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
EORAbsoluteXIndexed{
    // Crossing a page (entire absolute address)
    AbsoluteXIndexed().address(0x00FF).value(0xA000),
    EORAbsoluteXIndexed::Requirements{
        .initial = {
            .a = 0x00,
            .x = 0x0F,
            .flags = { },
            .operand = 0xFF },
        .final = {
            .a = 0xFF,
            .x = 0x0F,
            .flags = {
                .n_value = {
                    .status_flag = FLAGS6502::N,
                    .expected_value = true },
                .z_value = {
                    .status_flag = FLAGS6502::Z,
                    .expected_value = false } },
            .operand = 0xFF
        }}
},
EORAbsoluteXIndexed{
    // Test individual bits are EOR'd.
    AbsoluteXIndexed().address(0x8000).value(0xA000),
    EORAbsoluteXIndexed::Requirements{
        .initial = {
            .a = 0b10101010,
            .x = 0x02,
            .flags = { },
            .operand = 0b10101010 },
        .final = {
            .a = 0b00000000,
            .x = 0x02,
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
EORAbsoluteXIndexed{
    // Test individual bits are EOR'd.
    AbsoluteXIndexed().address(0x8000).value(0xA0FF),
    EORAbsoluteXIndexed::Requirements{
        .initial = {
            .a = 0b01010101,
            .x = 0xFF,
            .flags = { },
            .operand = 0b01010101 },
        .final = {
            .a = 0b00000000,
            .x = 0xFF,
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
EORAbsoluteXIndexed{
    // Test individual bits are EOR'd.
    AbsoluteXIndexed().address(0x8000).value(0xA080),
    EORAbsoluteXIndexed::Requirements{
        .initial = {
            .a = 0b10101010,
            .x = 0x80,
            .flags = { },
            .operand = 0b01010101 },
        .final = {
            .a = 0b11111111,
            .x = 0x80,
            .flags = {
                .n_value = {
                    .status_flag = FLAGS6502::N,
                    .expected_value = true },
                .z_value = {
                    .status_flag = FLAGS6502::Z,
                    .expected_value = false } },
            .operand = 0b01010101
        }}
}
};

TEST_P(EORAbsoluteXIndexedMode, TypicalInstructionExecution)
{
    TypicalInstructionExecution(*this, GetParam());
}

INSTANTIATE_TEST_SUITE_P(ExclusiveOrAbsoluteXIndexedAtVariousAddresses,
                         EORAbsoluteXIndexedMode,
                         testing::ValuesIn(EORAbsoluteXIndexedModeTestValues) );
