#include "addressing_mode_helpers.hpp"



struct LDX_Immediate_Expectations
{
    constexpr LDX_Immediate_Expectations &accumulator(const uint8_t v) { x = v; return *this; }

    uint8_t x;
    NZFlags flags;
};

using LDXImmediate     = LDX<Immediate, LDX_Immediate_Expectations, 2>;
using LDXImmediateMode = ParameterizedInstructionExecutorTestFixture<LDXImmediate>;


static void StoreTestValueAtImmediateAddress(InstructionExecutorTestFixture &fixture, const LDXImmediate &instruction_param)
{
    fixture.fakeMemory[instruction_param.address.instruction_address + 1] = instruction_param.address.immediate_value;
}

static void SetupAffectedOrUsedRegisters(InstructionExecutorTestFixture &fixture, const LDXImmediate &instruction_param)
{
    fixture.r.x = instruction_param.requirements.initial.x;
    fixture.r.SetFlag(FLAGS6502::N, instruction_param.requirements.initial.flags.n_value.expected_value);
    fixture.r.SetFlag(FLAGS6502::Z, instruction_param.requirements.initial.flags.z_value.expected_value);
}

template<>
void LoadInstructionIntoMemoryAndSetRegistersToInitialState(      InstructionExecutorTestFixture &fixture,
                                                            const LDXImmediate                   &instruction_param)
{
    SetupRAMForInstructionsThatHaveImmediateValue(fixture, instruction_param);
    SetupAffectedOrUsedRegisters(fixture, instruction_param);
}

template<>
void RegistersAreInExpectedState(const Registers &registers,
                                 const LDX_Immediate_Expectations &expectations)
{
    EXPECT_THAT(registers.x, Eq(expectations.x));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::N), Eq(expectations.flags.n_value.expected_value));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::Z), Eq(expectations.flags.z_value.expected_value));
}

template<>
void MemoryContainsInstruction(const InstructionExecutorTestFixture &fixture,
                               const Instruction<AbstractInstruction_e::LDX, Immediate> &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter ), Eq( OpcodeFor(AbstractInstruction_e::LDX, AddressMode_e::Immediate) ));
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 1), Eq(instruction.address.immediate_value));
}

template<>
void MemoryContainsExpectedComputation(const InstructionExecutorTestFixture &/* fixture */,
                                       const LDXImmediate                   &/* instruction */)
{
}


static const std::vector<LDXImmediate> LDXImmediateModeTestValues {
LDXImmediate{
    // Beginning of a page
    Immediate().address(0x0000).value(6),
    LDXImmediate::Requirements{
        .initial = {
            .x = 0,
            .flags = { }},
        .final = {
            .x = 6,
            .flags = { }
        }}
},
LDXImmediate{
    // One before the end of a page
    Immediate().address(0x00FE).value(6),
    LDXImmediate::Requirements{
        .initial = {
            .x = 0,
            .flags = { }},
        .final = {
            .x = 6,
            .flags = { }
        }}
},
LDXImmediate{
    // Crossing a page boundary
    Immediate().address(0x00FF).value(6),
    LDXImmediate::Requirements{
        .initial = {
            .x = 0,
            .flags = { }},
        .final = {
            .x = 6,
            .flags = { }
        }}
},
LDXImmediate{
    // Loading a zero affects the Z flag
    Immediate().address(0x8000).value(0),
    LDXImmediate::Requirements{
        .initial = {
            .x = 6,
            .flags = { }},
        .final = {
            .x = 0,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true } }
        }}
},
LDXImmediate{
    // Loading a negative affects the N flag
    Immediate().address(0x8000).value(0x80),
    LDXImmediate::Requirements{
        .initial = {
            .x = 0,
            .flags = { }},
        .final = {
            .x = 0x80,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false } }
        }}
}
};


TEST_P(LDXImmediateMode, TypicalInstructionExecution)
{
    TypicalInstructionExecution(*this, GetParam());
}

INSTANTIATE_TEST_SUITE_P(LoadImmediateAtVariousAddresses,
                         LDXImmediateMode,
                         testing::ValuesIn(LDXImmediateModeTestValues) );
