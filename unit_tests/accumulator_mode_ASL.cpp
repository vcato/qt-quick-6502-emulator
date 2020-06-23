#include <gmock/gmock.h>
#include "instruction_checks.hpp"



struct ASL_Accumulator_Expectations
{
    constexpr ASL_Accumulator_Expectations &accumulator(const uint8_t v) { a = v; return *this; }

    uint8_t  a;
    NZCFlags flags;
};

using ASLAccumulator     = ASL<Accumulator, ASL_Accumulator_Expectations, 2>;
using ASLAccumulatorMode = ParameterizedInstructionExecutorTestFixture<ASLAccumulator>;


template<>
void LoadInstructionIntoMemoryAndSetRegistersToInitialState(      InstructionExecutorTestFixture &fixture,
                                                            const ASLAccumulator                    &instruction_param)
{
    fixture.loadOpcodeIntoMemory(instruction_param.operation,
                                 AddressMode_e::Accumulator,
                                 instruction_param.address.instruction_address);

    // Load appropriate registers
    fixture.r.a = instruction_param.requirements.initial.a;
    fixture.r.SetFlag(FLAGS6502::N, instruction_param.requirements.initial.flags.n_value.expected_value);
    fixture.r.SetFlag(FLAGS6502::Z, instruction_param.requirements.initial.flags.z_value.expected_value);
    fixture.r.SetFlag(FLAGS6502::C, instruction_param.requirements.initial.flags.c_value.expected_value);
}

template<>
void RegistersAreInExpectedState(const Registers &registers,
                                 const ASL_Accumulator_Expectations &expectations)
{
    EXPECT_THAT(registers.a, Eq(expectations.a));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::N), Eq(expectations.flags.n_value.expected_value));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::Z), Eq(expectations.flags.z_value.expected_value));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::C), Eq(expectations.flags.c_value.expected_value));
}

template<>
void MemoryContainsInstruction(const InstructionExecutorTestFixture &fixture,
                               const Instruction<AbstractInstruction_e::ASL, Accumulator> &)
{
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter ), Eq( OpcodeFor(AbstractInstruction_e::ASL, AddressMode_e::Accumulator) ));
}

template<>
void MemoryContainsExpectedComputation(const InstructionExecutorTestFixture &,
                                       const ASLAccumulator                 &)
{
    // No memory is affected
}


static const std::vector<ASLAccumulator> ASLAccumulatorModeTestValues {
ASLAccumulator{
    // Beginning of a page
    Accumulator().address(0x8000),
    ASLAccumulator::Requirements{
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
                    .expected_value = true },
                .c_value = {
                    .status_flag = FLAGS6502::C,
                    .expected_value = false } },
        }}
},
ASLAccumulator{
    // Middle of a page
    Accumulator().address(0x8080),
    ASLAccumulator::Requirements{
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
                    .expected_value = true },
                .c_value = {
                    .status_flag = FLAGS6502::C,
                    .expected_value = false } },
        }}
},
ASLAccumulator{
    // End of a page
    Accumulator().address(0x80FF),
    ASLAccumulator::Requirements{
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
                    .expected_value = true },
                .c_value = {
                    .status_flag = FLAGS6502::C,
                    .expected_value = false } },
        }}
},
ASLAccumulator{
    // Check for High bit going into carry
    Accumulator().address(0x8000),
    ASLAccumulator::Requirements{
        .initial = {
            .a = 0b10101010,
            .flags = { } },
        .final = {
            .a = 0b01010100,
            .flags = {
                .n_value = {
                    .status_flag = FLAGS6502::N,
                    .expected_value = false },
                .z_value = {
                    .status_flag = FLAGS6502::Z,
                    .expected_value = false },
                .c_value = {
                    .status_flag = FLAGS6502::C,
                    .expected_value = true } },
        }}
},
ASLAccumulator{
    // Check for N flag
    Accumulator().address(0x8000),
    ASLAccumulator::Requirements{
        .initial = {
            .a = 0b11101010,
            .flags = { } },
        .final = {
            .a = 0b11010100,
            .flags = {
                .n_value = {
                    .status_flag = FLAGS6502::N,
                    .expected_value = true },
                .z_value = {
                    .status_flag = FLAGS6502::Z,
                    .expected_value = false },
                .c_value = {
                    .status_flag = FLAGS6502::C,
                    .expected_value = true } },
        }}
},
ASLAccumulator{
    // Zero is set in lowest bit
    Accumulator().address(0x8000),
    ASLAccumulator::Requirements{
        .initial = {
            .a = 0b00000001,
            .flags = { } },
        .final = {
            .a = 0b00000010,
            .flags = {
                .n_value = {
                    .status_flag = FLAGS6502::N,
                    .expected_value = false },
                .z_value = {
                    .status_flag = FLAGS6502::Z,
                    .expected_value = false },
                .c_value = {
                    .status_flag = FLAGS6502::C,
                    .expected_value = false } },
        }}
}
};

TEST_P(ASLAccumulatorMode, TypicalInstructionExecution)
{
    TypicalInstructionExecution(*this, GetParam());
}

INSTANTIATE_TEST_SUITE_P(AslAbsoluteAtVariousAddresses,
                         ASLAccumulatorMode,
                         testing::ValuesIn(ASLAccumulatorModeTestValues) );
