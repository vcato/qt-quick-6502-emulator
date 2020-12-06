#include "addressing_mode_helpers.hpp"



struct CMP_Immediate_Expectations
{
    uint8_t  a;
    NZCFlags flags;
};

using CMPImmediate     = CMP<Immediate, CMP_Immediate_Expectations, 2>;
using CMPImmediateMode = ParameterizedInstructionExecutorTestFixture<CMPImmediate>;


static void StoreTestValueAtImmediateAddress(InstructionExecutorTestFixture &fixture, const CMPImmediate &instruction_param)
{
    fixture.fakeMemory[instruction_param.address.instruction_address + 1] = instruction_param.address.immediate_value;
}

static void SetupAffectedOrUsedRegisters(InstructionExecutorTestFixture &fixture, const CMPImmediate &instruction_param)
{
    fixture.r.a = instruction_param.requirements.initial.a;
    fixture.r.SetFlag(FLAGS6502::N, instruction_param.requirements.initial.flags.n_value.expected_value);
    fixture.r.SetFlag(FLAGS6502::Z, instruction_param.requirements.initial.flags.z_value.expected_value);
    fixture.r.SetFlag(FLAGS6502::C, instruction_param.requirements.initial.flags.c_value.expected_value);
}

template<>
void LoadInstructionIntoMemoryAndSetRegistersToInitialState(      InstructionExecutorTestFixture &fixture,
                                                            const CMPImmediate                   &instruction_param)
{
    SetupRAMForInstructionsThatHaveImmediateValue(fixture, instruction_param);
    SetupAffectedOrUsedRegisters(fixture, instruction_param);
}

template<>
void RegistersAreInExpectedState(const Registers &registers,
                                 const CMP_Immediate_Expectations &expectations)
{
    EXPECT_THAT(registers.a, Eq(expectations.a));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::N), Eq(expectations.flags.n_value.expected_value));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::Z), Eq(expectations.flags.z_value.expected_value));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::C), Eq(expectations.flags.c_value.expected_value));
}

template<>
void MemoryContainsInstruction(const InstructionExecutorTestFixture &fixture,
                               const Instruction<AbstractInstruction_e::CMP, Immediate> &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter ), Eq( OpcodeFor(AbstractInstruction_e::CMP, AddressMode_e::Immediate) ));
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 1), Eq(instruction.address.immediate_value));
}

template<>
void MemoryContainsExpectedComputation(const InstructionExecutorTestFixture &/* fixture */,
                                       const CMPImmediate                   &/* instruction */)
{
    // No memory is affected!
}


static const std::vector<CMPImmediate> CMPImmediateModeTestValues {
CMPImmediate{
    // Beginning of a page
    // 0 - 0
    Immediate().address(0x0000).value(0x00),
    CMPImmediate::Requirements{
        .initial = {
            .a = 0,
            .flags = { }},
        .final = {
            .a = 0,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true },
                .c_value = { .expected_value = true } }
        }}
},
CMPImmediate{
    // Middle of a page
    // 0 - 0
    Immediate().address(0x0080).value(0x00),
    CMPImmediate::Requirements{
        .initial = {
            .a = 0,
            .flags = { }},
        .final = {
            .a = 0,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true },
                .c_value = { .expected_value = true } }
        }}
},
CMPImmediate{
    // End of a page
    // 0xFF - 0
    Immediate().address(0x00FD).value(0),
    CMPImmediate::Requirements{
        .initial = {
            .a = 0xFF,
            .flags = { }},
        .final = {
            .a = 0xFF,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true } },
        }}
},
CMPImmediate{
    // Crossing a page (partial absolute address)
    // 0 - 0xFF
    Immediate().address(0x00FE).value(0xFF),
    CMPImmediate::Requirements{
        .initial = {
            .a = 0,
            .flags = { }},
        .final = {
            .a = 0,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false } },
        }}
},
CMPImmediate{
    // Crossing a page (entire absolute address)
    // 0 - 0
    Immediate().address(0x00FF).value(0),
    CMPImmediate::Requirements{
        .initial = {
            .a = 0,
            .flags = { }},
        .final = {
            .a = 0,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true },
                .c_value = { .expected_value = true } },
        }}
},
CMPImmediate{
    // 2 - 3
    // Negative Result
    Immediate().address(0x8000).value(3),
    CMPImmediate::Requirements{
        .initial = {
            .a = 2,
            .flags = { }},
        .final = {
            .a = 2,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = false } },
        }}
},
CMPImmediate{
    // 3 - 2
    // Positive Result
    Immediate().address(0x8000).value(2),
    CMPImmediate::Requirements{
        .initial = {
            .a = 3,
            .flags = { }},
        .final = {
            .a = 3,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = false },
                .c_value = { .expected_value = true } },
        }}
}
};


TEST_P(CMPImmediateMode, TypicalInstructionExecution)
{
    TypicalInstructionExecution(*this, GetParam());
}

INSTANTIATE_TEST_SUITE_P(CompareAccumulatorImmediateAtVariousAddresses,
                         CMPImmediateMode,
                         testing::ValuesIn(CMPImmediateModeTestValues) );

