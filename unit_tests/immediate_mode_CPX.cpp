#include "addressing_mode_helpers.hpp"



struct CPX_Immediate_Expectations
{
    uint8_t  x;
    NZCFlags flags;
};

using CPXImmediate     = CPX<Immediate, CPX_Immediate_Expectations, 2>;
using CPXImmediateMode = ParameterizedInstructionExecutorTestFixture<CPXImmediate>;


static void StoreTestValueAtImmediateAddress(InstructionExecutorTestFixture &fixture, const CPXImmediate &instruction_param)
{
    fixture.fakeMemory[instruction_param.address.instruction_address + 1] = instruction_param.address.immediate_value;
}

static void SetupAffectedOrUsedRegisters(InstructionExecutorTestFixture &fixture, const CPXImmediate &instruction_param)
{
    fixture.r.x = instruction_param.requirements.initial.x;
    fixture.r.SetFlag(FLAGS6502::N, instruction_param.requirements.initial.flags.n_value.expected_value);
    fixture.r.SetFlag(FLAGS6502::Z, instruction_param.requirements.initial.flags.z_value.expected_value);
    fixture.r.SetFlag(FLAGS6502::C, instruction_param.requirements.initial.flags.c_value.expected_value);
}

template<>
void LoadInstructionIntoMemoryAndSetRegistersToInitialState(      InstructionExecutorTestFixture &fixture,
                                                            const CPXImmediate                   &instruction_param)
{
    SetupRAMForInstructionsThatHaveImmediateValue(fixture, instruction_param);
    SetupAffectedOrUsedRegisters(fixture, instruction_param);
}

template<>
void RegistersAreInExpectedState(const Registers &registers,
                                 const CPX_Immediate_Expectations &expectations)
{
    EXPECT_THAT(registers.x, Eq(expectations.x));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::N), Eq(expectations.flags.n_value.expected_value));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::Z), Eq(expectations.flags.z_value.expected_value));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::C), Eq(expectations.flags.c_value.expected_value));
}

template<>
void MemoryContainsInstruction(const InstructionExecutorTestFixture &fixture,
                               const Instruction<AbstractInstruction_e::CPX, Immediate> &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter ), Eq( OpcodeFor(AbstractInstruction_e::CPX, AddressMode_e::Immediate) ));
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 1), Eq(instruction.address.immediate_value));
}

template<>
void MemoryContainsExpectedComputation(const InstructionExecutorTestFixture &/* fixture */,
                                       const CPXImmediate                   &/* instruction */)
{
    // No memory is affected!
}


static const std::vector<CPXImmediate> CPXImmediateModeTestValues {
CPXImmediate{
    // Beginning of a page
    // 0 - 0
    Immediate().address(0x0000).value(0x00),
    CPXImmediate::Requirements{
        .initial = {
            .x = 0,
            .flags = { }},
        .final = {
            .x = 0,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true },
                .c_value = { .expected_value = true } }
        }}
},
CPXImmediate{
    // Middle of a page
    // 0 - 0
    Immediate().address(0x0080).value(0x00),
    CPXImmediate::Requirements{
        .initial = {
            .x = 0,
            .flags = { }},
        .final = {
            .x = 0,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true },
                .c_value = { .expected_value = true } }
        }}
},
CPXImmediate{
    // End of a page
    // 0xFF - 0
    Immediate().address(0x00FD).value(0),
    CPXImmediate::Requirements{
        .initial = {
            .x = 0xFF,
            .flags = { }},
        .final = {
            .x = 0xFF,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true } },
        }}
},
CPXImmediate{
    // Crossing a page (partial absolute address)
    // 0 - 0xFF
    Immediate().address(0x00FE).value(0xFF),
    CPXImmediate::Requirements{
        .initial = {
            .x = 0,
            .flags = { }},
        .final = {
            .x = 0,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false } },
        }}
},
CPXImmediate{
    // Crossing a page (entire absolute address)
    // 0 - 0
    Immediate().address(0x00FF).value(0),
    CPXImmediate::Requirements{
        .initial = {
            .x = 0,
            .flags = { }},
        .final = {
            .x = 0,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true },
                .c_value = { .expected_value = true } },
        }}
},
CPXImmediate{
    // 2 - 3
    // Negative Result
    Immediate().address(0x8000).value(3),
    CPXImmediate::Requirements{
        .initial = {
            .x = 2,
            .flags = { }},
        .final = {
            .x = 2,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false } },
        }}
},
CPXImmediate{
    // 3 - 2
    // Positive Result
    Immediate().address(0x8000).value(2),
    CPXImmediate::Requirements{
        .initial = {
            .x = 3,
            .flags = { }},
        .final = {
            .x = 3,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true } },
        }}
}
};


TEST_P(CPXImmediateMode, TypicalInstructionExecution)
{
    TypicalInstructionExecution(*this, GetParam());
}

INSTANTIATE_TEST_SUITE_P(CompareXImmediateAtVariousAddresses,
                         CPXImmediateMode,
                         testing::ValuesIn(CPXImmediateModeTestValues) );

