#include <gmock/gmock.h>
#include "instruction_checks.hpp"



struct LDA_AbsoluteXIndexed_Expectations
{
    constexpr LDA_AbsoluteXIndexed_Expectations &accumulator(const uint8_t v) { a = v; return *this; }

    uint8_t a;
    uint8_t x;
    NZFlags flags;
};

using LDAAbsoluteXIndexed     = LDA<AbsoluteXIndexed, LDA_AbsoluteXIndexed_Expectations, 4>;
using LDAAbsoluteXIndexedMode = ParameterizedInstructionExecutorTestFixture<LDAAbsoluteXIndexed>;


template<>
void LoadInstructionIntoMemoryAndSetRegistersToInitialState(      InstructionExecutorTestFixture &fixture,
                                                            const LDAAbsoluteXIndexed            &instruction_param)
{
    fixture.loadOpcodeIntoMemory(instruction_param.operation,
                                 AddressMode_e::AbsoluteXIndexed,
                                 instruction_param.address.instruction_address);
    fixture.fakeMemory[instruction_param.address.instruction_address + 1] = fixture.loByteOf(instruction_param.address.absolute_address);
    fixture.fakeMemory[instruction_param.address.instruction_address + 2] = fixture.hiByteOf(instruction_param.address.absolute_address);
    fixture.fakeMemory[instruction_param.address.absolute_address + instruction_param.requirements.final.x ] = instruction_param.requirements.final.a;

    // Load appropriate registers
    fixture.r.a = instruction_param.requirements.initial.a;
    fixture.r.x = instruction_param.requirements.initial.x;
    fixture.r.SetFlag(FLAGS6502::N, instruction_param.requirements.initial.flags.n_value.expected_value);
    fixture.r.SetFlag(FLAGS6502::Z, instruction_param.requirements.initial.flags.z_value.expected_value);
}

template<>
void RegistersAreInExpectedState(const Registers &registers,
                                 const LDA_AbsoluteXIndexed_Expectations &expectations)
{
    EXPECT_THAT(registers.a, Eq(expectations.a));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::N), Eq(expectations.flags.n_value.expected_value));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::Z), Eq(expectations.flags.z_value.expected_value));
}

template<>
void MemoryContainsInstruction(const InstructionExecutorTestFixture &fixture,
                               const Instruction<AbstractInstruction_e::LDA, AbsoluteXIndexed> &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter ), Eq( OpcodeFor(AbstractInstruction_e::LDA, AddressMode_e::AbsoluteXIndexed) ));

    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 1), Eq( fixture.loByteOf(instruction.address.absolute_address) ));
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 2), Eq( fixture.hiByteOf(instruction.address.absolute_address) ));
}

template<>
void MemoryContainsExpectedComputation(const InstructionExecutorTestFixture &fixture,
                                       const LDAAbsoluteXIndexed            &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( instruction.address.absolute_address + instruction.requirements.final.x ), Eq( instruction.requirements.final.a ));
}

template<>
void InstructionExecutedInExpectedClockTicks(const InstructionExecutorTestFixture &fixture,
                                             const LDAAbsoluteXIndexed            &instruction)
{
    // Account for a clock tick one greater if a page is crossed
    uint32_t original_address  = fixture.loByteOf(instruction.address.absolute_address);
    uint32_t effective_address = original_address + instruction.requirements.initial.x;
    bool     page_boundary_is_crossed = effective_address > 0xFF;
    uint32_t extra_cycle_count = (page_boundary_is_crossed) ? 1 : 0; // If the page is crossed

    EXPECT_THAT(fixture.executor.clock_ticks, Eq(instruction.requirements.cycle_count + extra_cycle_count));
}


static const std::vector<LDAAbsoluteXIndexed> LDAAbsoluteXIndexedModeTestValues {
LDAAbsoluteXIndexed{
    // Beginning of a page
    AbsoluteXIndexed().address(0x0000).value(0xA000),
    LDAAbsoluteXIndexed::Requirements{
        .initial = {
            .a = 0,
            .x = 0,
            .flags = { }},
        .final = {
            .a = 6,
            .x = 0,
            .flags = { }
        }}
},
LDAAbsoluteXIndexed{
    // Middle of a page
    AbsoluteXIndexed().address(0x0088).value(0xA000),
    LDAAbsoluteXIndexed::Requirements{
        .initial = {
            .a = 0,
            .x = 5,
            .flags = { }},
        .final = {
            .a = 6,
            .x = 5,
            .flags = { }
        }}
},
LDAAbsoluteXIndexed{
    // End of a page
    AbsoluteXIndexed().address(0x00FD).value(0xA000),
    LDAAbsoluteXIndexed::Requirements{
        .initial = {
            .a = 0,
            .x = 0,
            .flags = { }},
        .final = {
            .a = 6,
            .x = 0,
            .flags = { }
        }}
},
LDAAbsoluteXIndexed{
    // Crossing a page (partial absolute address)
    AbsoluteXIndexed().address(0x00FE).value(0xA000),
    LDAAbsoluteXIndexed::Requirements{
        .initial = {
            .a = 0,
            .x = 0,
            .flags = { }},
        .final = {
            .a = 6,
            .x = 0,
            .flags = { }
        }}
},
LDAAbsoluteXIndexed{
    // Crossing a page (entire absolute address)
    AbsoluteXIndexed().address(0x00FF).value(0xA000),
    LDAAbsoluteXIndexed::Requirements{
        .initial = {
            .a = 0,
            .x = 0,
            .flags = { }},
        .final = {
            .a = 6,
            .x = 0,
            .flags = { }
        }}
},
LDAAbsoluteXIndexed{
    // Loading a zero affects the Z flag
    AbsoluteXIndexed().address(0x8000).value(0xA000),
    LDAAbsoluteXIndexed::Requirements{
        .initial = {
            .a = 0xA0,
            .x = 0,
            .flags = { }},
        .final = {
            .a = 0,
            .x = 0,
            .flags = {
                .n_value = {
                    .status_flag = FLAGS6502::N,
                    .expected_value = false },
                .z_value = {
                    .status_flag = FLAGS6502::Z,
                    .expected_value = true } }
        }}
},
LDAAbsoluteXIndexed{
    // Loading a negative affects the N flag
    AbsoluteXIndexed().address(0x8000).value(0xA000),
    LDAAbsoluteXIndexed::Requirements{
        .initial = {
            .a = 0x10,
            .x = 0,
            .flags = { }},
        .final = {
            .a = 0xFF,
            .x = 0,
            .flags = {
                .n_value = {
                    .status_flag = FLAGS6502::N,
                    .expected_value = true },
                .z_value = {
                    .status_flag = FLAGS6502::Z,
                    .expected_value = false } }
        }}
}
};

TEST_P(LDAAbsoluteXIndexedMode, TypicalInstructionExecution)
{
    TypicalInstructionExecution(*this, GetParam());
}

INSTANTIATE_TEST_SUITE_P(LoadAbsoluteAtVariousAddresses,
                         LDAAbsoluteXIndexedMode,
                         testing::ValuesIn(LDAAbsoluteXIndexedModeTestValues) );
