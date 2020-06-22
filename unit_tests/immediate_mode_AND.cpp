#include <gmock/gmock.h>
#include "instruction_checks.hpp"



struct AND_Immediate_Expectations
{
    constexpr AND_Immediate_Expectations &accumulator(const uint8_t v) { a = v; return *this; }

    uint8_t a;
    NZFlags flags;
};

using ANDImmediate     = AND<Immediate, AND_Immediate_Expectations, 2>;
using ANDImmediateMode = ParameterizedInstructionExecutorTestFixture<ANDImmediate>;


template<>
void LoadInstructionIntoMemoryAndSetRegistersToInitialState(      InstructionExecutorTestFixture &fixture,
                                                            const ANDImmediate                   &instruction_param)
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
                                 const AND_Immediate_Expectations &expectations)
{
    EXPECT_THAT(registers.a, Eq(expectations.a));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::N), Eq(expectations.flags.n_value.expected_value));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::Z), Eq(expectations.flags.z_value.expected_value));
}

template<>
void MemoryContainsInstruction(const InstructionExecutorTestFixture &fixture,
                               const Instruction<AbstractInstruction_e::AND, Immediate> &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter ), Eq( OpcodeFor(AbstractInstruction_e::AND, AddressMode_e::Immediate) ));
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 1), Eq(instruction.address.immediate_value));
}

template<>
void MemoryContainsExpectedComputation(const InstructionExecutorTestFixture &/* fixture */,
                                       const ANDImmediate                   &/* instruction */)
{
}

// We will basically be testing the AND truth table here with specially chosen values.

static const std::vector<ANDImmediate> ANDImmediateModeTestValues {
ANDImmediate{
    // Beginning of a page
    Immediate().address(0x0000).value(0x00),
    ANDImmediate::Requirements{
        .initial = {
            .a = 0,
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
ANDImmediate{
    // One before the end of a page
    Immediate().address(0x00FE).value(0xFF),
    ANDImmediate::Requirements{
        .initial = {
            .a = 0,
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
ANDImmediate{
    // Crossing a page boundary
    Immediate().address(0x00FF).value(0),
    ANDImmediate::Requirements{
        .initial = {
            .a = 0xFF,
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
ANDImmediate{
    Immediate().address(0x8000).value(0xFF),
    ANDImmediate::Requirements{
        .initial = {
            .a = 0xFF,
            .flags = { }},
        .final = {
            .a = 0xFF,
            .flags = {
                .n_value = {
                    .status_flag = FLAGS6502::N,
                    .expected_value = true },
                .z_value = {
                    .status_flag = FLAGS6502::Z,
                    .expected_value = false } }
        }}
},
ANDImmediate{
    // Check for masking out the high bit
    Immediate().address(0x8000).value(0x80),
    ANDImmediate::Requirements{
        .initial = {
            .a = 0xFF,
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
},
ANDImmediate{
    // Use alternating bits for a zero result
    Immediate().address(0x8000).value(0b10101010),
    ANDImmediate::Requirements{
        .initial = {
            .a = 0b01010101,
            .flags = { }},
        .final = {
            .a = 0x00,
            .flags = {
                .n_value = {
                    .status_flag = FLAGS6502::N,
                    .expected_value = false },
                .z_value = {
                    .status_flag = FLAGS6502::Z,
                    .expected_value = true } }
        }}
},
ANDImmediate{
    // Use the same bits for the same result
    Immediate().address(0x8000).value(0b01010101),
    ANDImmediate::Requirements{
        .initial = {
            .a = 0b01010101,
            .flags = { }},
        .final = {
            .a = 0b01010101,
            .flags = {
                .n_value = {
                    .status_flag = FLAGS6502::N,
                    .expected_value = false },
                .z_value = {
                    .status_flag = FLAGS6502::Z,
                    .expected_value = false } }
        }}
},
ANDImmediate{
    // Use the same bits for the same result (not the same pattern as before)
    Immediate().address(0x8000).value(0b10101010),
    ANDImmediate::Requirements{
        .initial = {
            .a = 0b10101010,
            .flags = { }},
        .final = {
            .a = 0b10101010,
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


TEST_P(ANDImmediateMode, TypicalInstructionExecution)
{
    TypicalInstructionExecution(*this, GetParam());
}

INSTANTIATE_TEST_SUITE_P(AndImmediateAtVariousAddresses,
                         ANDImmediateMode,
                         testing::ValuesIn(ANDImmediateModeTestValues) );

