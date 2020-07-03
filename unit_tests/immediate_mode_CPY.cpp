#include <gmock/gmock.h>
#include "instruction_checks.hpp"



struct CPY_Immediate_Expectations
{
    uint8_t  y;
    NZCFlags flags;
};

using CPYImmediate     = CPY<Immediate, CPY_Immediate_Expectations, 2>;
using CPYImmediateMode = ParameterizedInstructionExecutorTestFixture<CPYImmediate>;


template<>
void LoadInstructionIntoMemoryAndSetRegistersToInitialState(      InstructionExecutorTestFixture &fixture,
                                                            const CPYImmediate                   &instruction_param)
{
    fixture.loadOpcodeIntoMemory(instruction_param.operation,
                                 AddressMode_e::Immediate,
                                 instruction_param.address.instruction_address);
    fixture.fakeMemory[instruction_param.address.instruction_address + 1] = instruction_param.address.immediate_value;

    // Load appropriate registers
    fixture.r.y = instruction_param.requirements.initial.y;
    fixture.r.SetFlag(FLAGS6502::N, instruction_param.requirements.initial.flags.n_value.expected_value);
    fixture.r.SetFlag(FLAGS6502::Z, instruction_param.requirements.initial.flags.z_value.expected_value);
    fixture.r.SetFlag(FLAGS6502::C, instruction_param.requirements.initial.flags.c_value.expected_value);
}

template<>
void RegistersAreInExpectedState(const Registers &registers,
                                 const CPY_Immediate_Expectations &expectations)
{
    EXPECT_THAT(registers.y, Eq(expectations.y));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::N), Eq(expectations.flags.n_value.expected_value));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::Z), Eq(expectations.flags.z_value.expected_value));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::C), Eq(expectations.flags.c_value.expected_value));
}

template<>
void MemoryContainsInstruction(const InstructionExecutorTestFixture &fixture,
                               const Instruction<AbstractInstruction_e::CPY, Immediate> &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter ), Eq( OpcodeFor(AbstractInstruction_e::CPY, AddressMode_e::Immediate) ));
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 1), Eq(instruction.address.immediate_value));
}

template<>
void MemoryContainsExpectedComputation(const InstructionExecutorTestFixture &/* fixture */,
                                       const CPYImmediate                   &/* instruction */)
{
    // No memory is affected!
}


static const std::vector<CPYImmediate> CPYImmediateModeTestValues {
CPYImmediate{
    // Beginning of a page
    // 0 - 0
    Immediate().address(0x0000).value(0x00),
    CPYImmediate::Requirements{
        .initial = {
            .y = 0,
            .flags = { }},
        .final = {
            .y = 0,
            .flags = {
                .n_value = {
                    .status_flag = FLAGS6502::N,
                    .expected_value = false },
                .z_value = {
                    .status_flag = FLAGS6502::Z,
                    .expected_value = true },
                .c_value = {
                    .status_flag = FLAGS6502::C,
                    .expected_value = true } }
        }}
},
CPYImmediate{
    // Middle of a page
    // 0 - 0
    Immediate().address(0x0080).value(0x00),
    CPYImmediate::Requirements{
        .initial = {
            .y = 0,
            .flags = { }},
        .final = {
            .y = 0,
            .flags = {
                .n_value = {
                    .status_flag = FLAGS6502::N,
                    .expected_value = false },
                .z_value = {
                    .status_flag = FLAGS6502::Z,
                    .expected_value = true },
                .c_value = {
                    .status_flag = FLAGS6502::C,
                    .expected_value = true } }
        }}
},
CPYImmediate{
    // End of a page
    // 0xFF - 0
    Immediate().address(0x00FD).value(0),
    CPYImmediate::Requirements{
        .initial = {
            .y = 0xFF,
            .flags = { }},
        .final = {
            .y = 0xFF,
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
CPYImmediate{
    // Crossing a page (partial absolute address)
    // 0 - 0xFF
    Immediate().address(0x00FE).value(0xFF),
    CPYImmediate::Requirements{
        .initial = {
            .y = 0,
            .flags = { }},
        .final = {
            .y = 0,
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
},
CPYImmediate{
    // Crossing a page (entire absolute address)
    // 0 - 0
    Immediate().address(0x00FF).value(0),
    CPYImmediate::Requirements{
        .initial = {
            .y = 0,
            .flags = { }},
        .final = {
            .y = 0,
            .flags = {
                .n_value = {
                    .status_flag = FLAGS6502::N,
                    .expected_value = false },
                .z_value = {
                    .status_flag = FLAGS6502::Z,
                    .expected_value = true },
                .c_value = {
                    .status_flag = FLAGS6502::C,
                    .expected_value = true } },
        }}
},
CPYImmediate{
    // 2 - 3
    // Negative Result
    Immediate().address(0x8000).value(3),
    CPYImmediate::Requirements{
        .initial = {
            .y = 2,
            .flags = { }},
        .final = {
            .y = 2,
            .flags = {
                .n_value = {
                    .status_flag = FLAGS6502::N,
                    .expected_value = true },
                .z_value = {
                    .status_flag = FLAGS6502::Z,
                    .expected_value = false },
                .c_value = {
                    .status_flag = FLAGS6502::C,
                    .expected_value = false } },
        }}
},
CPYImmediate{
    // 3 - 2
    // Positive Result
    Immediate().address(0x8000).value(2),
    CPYImmediate::Requirements{
        .initial = {
            .y = 3,
            .flags = { }},
        .final = {
            .y = 3,
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
}
};


TEST_P(CPYImmediateMode, TypicalInstructionExecution)
{
    TypicalInstructionExecution(*this, GetParam());
}

INSTANTIATE_TEST_SUITE_P(CompareYImmediateAtVariousAddresses,
                         CPYImmediateMode,
                         testing::ValuesIn(CPYImmediateModeTestValues) );

