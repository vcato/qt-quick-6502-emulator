#include "addressing_mode_helpers.hpp"



struct LDY_Immediate_Expectations
{
    uint8_t y;
    NZFlags flags;
};

using LDYImmediate     = LDY<Immediate, LDY_Immediate_Expectations, 2>;
using LDYImmediateMode = ParameterizedInstructionExecutorTestFixture<LDYImmediate>;


static void StoreTestValueAtImmediateAddress(InstructionExecutorTestFixture &fixture, const LDYImmediate &instruction_param)
{
    fixture.fakeMemory[instruction_param.address.instruction_address + 1] = instruction_param.address.immediate_value;
}

static void SetupAffectedOrUsedRegisters(InstructionExecutorTestFixture &fixture, const LDYImmediate &instruction_param)
{
    fixture.r.y = instruction_param.requirements.initial.y;
    fixture.r.SetFlag(FLAGS6502::N, instruction_param.requirements.initial.flags.n_value.expected_value);
    fixture.r.SetFlag(FLAGS6502::Z, instruction_param.requirements.initial.flags.z_value.expected_value);
}

template<>
void LoadInstructionIntoMemoryAndSetRegistersToInitialState(      InstructionExecutorTestFixture &fixture,
                                                            const LDYImmediate                   &instruction_param)
{
    SetupRAMForInstructionsThatHaveImmediateValue(fixture, instruction_param);
    SetupAffectedOrUsedRegisters(fixture, instruction_param);
}

template<>
void RegistersAreInExpectedState(const Registers &registers,
                                 const LDY_Immediate_Expectations &expectations)
{
    EXPECT_THAT(registers.y, Eq(expectations.y));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::N), Eq(expectations.flags.n_value.expected_value));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::Z), Eq(expectations.flags.z_value.expected_value));
}

template<>
void MemoryContainsInstruction(const InstructionExecutorTestFixture &fixture,
                               const Instruction<AbstractInstruction_e::LDY, Immediate> &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter ), Eq( OpcodeFor(AbstractInstruction_e::LDY, AddressMode_e::Immediate) ));
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 1), Eq(instruction.address.immediate_value));
}

template<>
void MemoryContainsExpectedComputation(const InstructionExecutorTestFixture &/* fixture */,
                                       const LDYImmediate                   &/* instruction */)
{
}


static const std::vector<LDYImmediate> LDYImmediateModeTestValues {
LDYImmediate{
    // Beginning of a page
    Immediate().address(0x0000).value(6),
    LDYImmediate::Requirements{
        .initial = {
            .y = 0,
            .flags = { }},
        .final = {
            .y = 6,
            .flags = { }
        }}
},
LDYImmediate{
    // One before the end of a page
    Immediate().address(0x00FE).value(6),
    LDYImmediate::Requirements{
        .initial = {
            .y = 0,
            .flags = { }},
        .final = {
            .y = 6,
            .flags = { }
        }}
},
LDYImmediate{
    // Crossing a page boundary
    Immediate().address(0x00FF).value(6),
    LDYImmediate::Requirements{
        .initial = {
            .y = 0,
            .flags = { }},
        .final = {
            .y = 6,
            .flags = { }
        }}
},
LDYImmediate{
    // Loading a zero affects the Z flag
    Immediate().address(0x8000).value(0),
    LDYImmediate::Requirements{
        .initial = {
            .y = 6,
            .flags = { }},
        .final = {
            .y = 0,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true } }
        }}
},
LDYImmediate{
    // Loading a negative affects the N flag
    Immediate().address(0x8000).value(0x80),
    LDYImmediate::Requirements{
        .initial = {
            .y = 0,
            .flags = { }},
        .final = {
            .y = 0x80,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false } }
        }}
}
};


TEST_P(LDYImmediateMode, TypicalInstructionExecution)
{
    TypicalInstructionExecution(*this, GetParam());
}

INSTANTIATE_TEST_SUITE_P(LoadImmediateAtVariousAddresses,
                         LDYImmediateMode,
                         testing::ValuesIn(LDYImmediateModeTestValues) );
