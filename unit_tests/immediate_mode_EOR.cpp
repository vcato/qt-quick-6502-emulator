#include <gmock/gmock.h>
#include "instruction_checks.hpp"



struct EOR_Immediate_Expectations
{
    constexpr EOR_Immediate_Expectations &accumulator(const uint8_t v) { a = v; return *this; }

    uint8_t a;
    NZFlags flags;
};

using EORImmediate     = EOR<Immediate, EOR_Immediate_Expectations, 2>;
using EORImmediateMode = ParameterizedInstructionExecutorTestFixture<EORImmediate>;


template<>
void LoadInstructionIntoMemoryAndSetRegistersToInitialState(      InstructionExecutorTestFixture &fixture,
                                                            const EORImmediate                   &instruction_param)
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
                                 const EOR_Immediate_Expectations &expectations)
{
    EXPECT_THAT(registers.a, Eq(expectations.a));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::N), Eq(expectations.flags.n_value.expected_value));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::Z), Eq(expectations.flags.z_value.expected_value));
}

template<>
void MemoryContainsInstruction(const InstructionExecutorTestFixture &fixture,
                               const Instruction<AbstractInstruction_e::EOR, Immediate> &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter ), Eq( OpcodeFor(AbstractInstruction_e::EOR, AddressMode_e::Immediate) ));
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 1), Eq(instruction.address.immediate_value));
}

template<>
void MemoryContainsExpectedComputation(const InstructionExecutorTestFixture &/* fixture */,
                                       const EORImmediate                   &/* instruction */)
{
}


static const std::vector<EORImmediate> EORImmediateModeTestValues {
EORImmediate{
    // Beginning of a page
    Immediate().address(0x0000).value(0),
    EORImmediate::Requirements{
        .initial = {
            .a = 0,
                .flags = { } },
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
EORImmediate{
    // One before the end of a page
    Immediate().address(0x00FE).value(0),
    EORImmediate::Requirements{
        .initial = {
            .a = 0xFF,
            .flags = { } },
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
EORImmediate{
    // Crossing a page boundary
    Immediate().address(0x00FF).value(0xFF),
    EORImmediate::Requirements{
        .initial = {
            .a = 0,
            .flags = { } },
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
EORImmediate{
    // Test individual bits are EOR'd.
    Immediate().address(0x8000).value(0b10101010),
    EORImmediate::Requirements{
        .initial = {
            .a = 0b10101010,
            .flags = { } },
        .final = {
            .a = 0b00000000,
            .flags = {
                .n_value = {
                    .status_flag = FLAGS6502::N,
                    .expected_value = false },
                .z_value = {
                    .status_flag = FLAGS6502::Z,
                    .expected_value = true } }
        }}
},
EORImmediate{
    // Test individual bits are EOR'd.
    Immediate().address(0x8000).value(0b01010101),
    EORImmediate::Requirements{
        .initial = {
            .a = 0b01010101,
            .flags = { } },
        .final = {
            .a = 0b00000000,
            .flags = {
                .n_value = {
                    .status_flag = FLAGS6502::N,
                    .expected_value = false },
                .z_value = {
                    .status_flag = FLAGS6502::Z,
                    .expected_value = true } }
        }}
},
EORImmediate{
    // Test individual bits are EOR'd.
    Immediate().address(0x8000).value(0b01010101),
    EORImmediate::Requirements{
        .initial = {
            .a = 0b10101010,
            .flags = { } },
        .final = {
            .a = 0b11111111,
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


TEST_P(EORImmediateMode, TypicalInstructionExecution)
{
    TypicalInstructionExecution(*this, GetParam());
}

INSTANTIATE_TEST_SUITE_P(ExclusiveOrImmediateAtVariousAddresses,
                         EORImmediateMode,
                         testing::ValuesIn(EORImmediateModeTestValues) );
