#include <gmock/gmock.h>
#include "instruction_checks.hpp"



struct LDA_AbsoluteYIndexed_Expectations
{
    constexpr LDA_AbsoluteYIndexed_Expectations &accumulator(const uint8_t v) { a = v; return *this; }

    uint8_t a;
    uint8_t y;
    NZFlags flags;
};

using LDAAbsoluteYIndexed     = LDA<AbsoluteYIndexed, LDA_AbsoluteYIndexed_Expectations, 4>;
using LDAAbsoluteYIndexedMode = ParameterizedInstructionExecutorTestFixture<LDAAbsoluteYIndexed>;


template<>
void LoadInstructionIntoMemoryAndSetRegistersToInitialState(      InstructionExecutorTestFixture &fixture,
                                                            const LDAAbsoluteYIndexed            &instruction_param)
{
    fixture.loadOpcodeIntoMemory(instruction_param.operation,
                                 AddressMode_e::AbsoluteYIndexed,
                                 instruction_param.address.instruction_address);
    fixture.fakeMemory[instruction_param.address.instruction_address + 1] = fixture.loByteOf(instruction_param.address.absolute_address);
    fixture.fakeMemory[instruction_param.address.instruction_address + 2] = fixture.hiByteOf(instruction_param.address.absolute_address);
    fixture.fakeMemory[instruction_param.address.absolute_address + instruction_param.requirements.final.y ] = instruction_param.requirements.final.a;

    // Load appropriate registers
    fixture.r.a = instruction_param.requirements.initial.a;
    fixture.r.y = instruction_param.requirements.initial.y;
    fixture.r.SetFlag(FLAGS6502::N, instruction_param.requirements.initial.flags.n_value.expected_value);
    fixture.r.SetFlag(FLAGS6502::Z, instruction_param.requirements.initial.flags.z_value.expected_value);
}

template<>
void RegistersAreInExpectedState(const Registers &registers,
                                 const LDA_AbsoluteYIndexed_Expectations &expectations)
{
    EXPECT_THAT(registers.a, Eq(expectations.a));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::N), Eq(expectations.flags.n_value.expected_value));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::Z), Eq(expectations.flags.z_value.expected_value));
}

template<>
void MemoryContainsInstruction(const InstructionExecutorTestFixture &fixture,
                               const Instruction<AbstractInstruction_e::LDA, AbsoluteYIndexed> &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter ), Eq( OpcodeFor(AbstractInstruction_e::LDA, AddressMode_e::AbsoluteYIndexed) ));
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 1), Eq( fixture.loByteOf(instruction.address.absolute_address) ));
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 2), Eq( fixture.hiByteOf(instruction.address.absolute_address) ));
}

template<>
void MemoryContainsExpectedComputation(const InstructionExecutorTestFixture &fixture,
                                       const LDAAbsoluteYIndexed            &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( instruction.address.absolute_address + instruction.requirements.final.y ), Eq( instruction.requirements.final.a ));
}

template<>
void InstructionExecutedInExpectedClockTicks(const InstructionExecutorTestFixture &fixture,
                                             const LDAAbsoluteYIndexed            &instruction)
{
    // Account for a clock tick one greater if a page is crossed
    uint32_t original_address  = fixture.loByteOf(instruction.address.absolute_address);
    uint32_t effective_address = original_address + instruction.requirements.initial.y;
    bool     page_boundary_is_crossed = effective_address > 0xFF;
    uint32_t extra_cycle_count = (page_boundary_is_crossed) ? 1 : 0; // If the page is crossed

    EXPECT_THAT(fixture.executor.clock_ticks, Eq(instruction.requirements.cycle_count + extra_cycle_count));
}


static const std::vector<LDAAbsoluteYIndexed> LDAAbsoluteYIndexedModeTestValues {
LDAAbsoluteYIndexed{
    // Beginning of a page
    AbsoluteYIndexed().address(0x0000).value(0xA000),
    LDAAbsoluteYIndexed::Requirements{
        .initial = {
            .a = 0,
            .y = 0,
            .flags = { }},
        .final = {
            .a = 6,
            .y = 0,
            .flags = { }
        }}
},
LDAAbsoluteYIndexed{
    // Middle of a page
    AbsoluteYIndexed().address(0x0088).value(0xA000),
    LDAAbsoluteYIndexed::Requirements{
        .initial = {
            .a = 0,
            .y = 5,
            .flags = { }},
        .final = {
            .a = 6,
            .y = 5,
            .flags = { }
        }}
},
LDAAbsoluteYIndexed{
    // End of a page
    AbsoluteYIndexed().address(0x00FD).value(0xA000),
    LDAAbsoluteYIndexed::Requirements{
        .initial = {
            .a = 0,
            .y = 0,
            .flags = { }},
        .final = {
            .a = 6,
            .y = 0,
            .flags = { }
        }}
},
LDAAbsoluteYIndexed{
    // Crossing a page (partial absolute address)
    AbsoluteYIndexed().address(0x00FE).value(0xA000),
    LDAAbsoluteYIndexed::Requirements{
        .initial = {
            .a = 0,
            .y = 0,
            .flags = { }},
        .final = {
            .a = 6,
            .y = 0,
            .flags = { }
        }}
},
LDAAbsoluteYIndexed{
    // Crossing a page (entire absolute address)
    AbsoluteYIndexed().address(0x00FF).value(0xA000),
    LDAAbsoluteYIndexed::Requirements{
        .initial = {
            .a = 0,
            .y = 0,
            .flags = { }},
        .final = {
            .a = 6,
            .y = 0,
            .flags = { }
        }}
},
LDAAbsoluteYIndexed{
    // Loading a zero affects the Z flag
    AbsoluteYIndexed().address(0x8000).value(0xA000),
    LDAAbsoluteYIndexed::Requirements{
        .initial = {
            .a = 0xA0,
            .y = 0,
            .flags = { }},
        .final = {
            .a = 0,
            .y = 0,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true } }
        }}
},
LDAAbsoluteYIndexed{
    // Loading a negative affects the N flag
    AbsoluteYIndexed().address(0x8000).value(0xA000),
    LDAAbsoluteYIndexed::Requirements{
        .initial = {
            .a = 0x10,
            .y = 0,
            .flags = { }},
        .final = {
            .a = 0xFF,
            .y = 0,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false } }
        }}
}
};

TEST_P(LDAAbsoluteYIndexedMode, TypicalInstructionExecution)
{
    TypicalInstructionExecution(*this, GetParam());
}

INSTANTIATE_TEST_SUITE_P(LoadAbsoluteYIndexedAtVariousAddresses,
                         LDAAbsoluteYIndexedMode,
                         testing::ValuesIn(LDAAbsoluteYIndexedModeTestValues) );
