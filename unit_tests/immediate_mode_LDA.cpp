#include <gmock/gmock.h>
#include "instruction_checks.hpp"



struct LDA_Immediate_Expectations
{
    constexpr LDA_Immediate_Expectations &accumulator(const uint8_t v) { a = v; return *this; }

    uint8_t a;
    NZFlags flags;
};

using LDAImmediate     = LDA<Immediate, LDA_Immediate_Expectations, 2>;
using LDAImmediateMode = ParameterizedInstructionExecutorTestFixture<LDAImmediate>;


template<>
void LoadInstructionIntoMemoryAndSetRegistersToInitialState(      InstructionExecutorTestFixture &fixture,
                                                            const LDAImmediate                   &instruction_param)
{
    fixture.loadOpcodeIntoMemory(instruction_param.operation,
                                 AddressMode_e::Immediate,
                                 instruction_param.address.instruction_address);
    fixture.fakeMemory[instruction_param.address.instruction_address + 1] = instruction_param.address.immediate_value;

    // Load appropriate registers
    fixture.r.a = instruction_param.requirements.initial.a;
    fixture.r.SetFlag(FLAGS6502::N, instruction_param.requirements.initial.flags.n_value.expected_value);
    fixture.r.SetFlag(FLAGS6502::Z, instruction_param.requirements.initial.flags.z_value.expected_value);
}

template<>
void RegistersAreInExpectedState(const Registers &registers,
                                 const LDA_Immediate_Expectations &expectations)
{
    EXPECT_THAT(registers.a, Eq(expectations.a));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::N), Eq(expectations.flags.n_value.expected_value));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::Z), Eq(expectations.flags.z_value.expected_value));
}

template<>
void MemoryContainsInstruction(const InstructionExecutorTestFixture &fixture,
                               const Instruction<AbstractInstruction_e::LDA, Immediate> &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter ), Eq( OpcodeFor(AbstractInstruction_e::LDA, AddressMode_e::Immediate) ));
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 1), Eq(instruction.address.immediate_value));
}

template<>
void MemoryContainsExpectedComputation(const InstructionExecutorTestFixture &/* fixture */,
                                       const LDAImmediate                   &/* instruction */)
{
}


static const std::vector<LDAImmediate> LDAImmediateModeTestValues {
LDAImmediate{
    // Beginning of a page
    Immediate().address(0x0000).value(6),
    LDAImmediate::Requirements{
        .initial = {
            .a = 0,
            .flags = { }},
        .final = {
            .a = 6,
            .flags = { }
        }}
},
LDAImmediate{
    // One before the end of a page
    Immediate().address(0x00FE).value(6),
    LDAImmediate::Requirements{
        .initial = {
            .a = 0,
            .flags = { }},
        .final = {
            .a = 6,
            .flags = { }
        }}
},
LDAImmediate{
    // Crossing a page boundary
    Immediate().address(0x00FF).value(6),
    LDAImmediate::Requirements{
        .initial = {
            .a = 0,
            .flags = { }},
        .final = {
            .a = 6,
            .flags = { }
        }}
},
LDAImmediate{
    // Loading a zero affects the Z flag
    Immediate().address(0x8000).value(0),
    LDAImmediate::Requirements{
        .initial = {
            .a = 6,
            .flags = { }},
        .final = {
            .a = 0,
            .flags = {
                .n_value = {
                    .status_flag = FLAGS6502::N,
                    .expected_value = false },
                .z_value = {
                    .status_flag = FLAGS6502::Z,
                    .expected_value = true } }
        }}
},
LDAImmediate{
    // Loading a negative affects the N flag
    Immediate().address(0x8000).value(0x80),
    LDAImmediate::Requirements{
        .initial = {
            .a = 0,
            .flags = { }},
        .final = {
            .a = 0x80,
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


TEST_P(LDAImmediateMode, TypicalInstructionExecution)
{
    TypicalInstructionExecution(*this, GetParam());
}

INSTANTIATE_TEST_CASE_P(LoadImmediateAtVariousAddresses,
                         LDAImmediateMode,
                         testing::ValuesIn(LDAImmediateModeTestValues) );

