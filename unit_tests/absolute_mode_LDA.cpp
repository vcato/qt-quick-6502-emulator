#include "addressing_mode_helpers.hpp"



struct LDA_Absolute_Expectations
{
    constexpr LDA_Absolute_Expectations &accumulator(const uint8_t v) { a = v; return *this; }

    uint8_t a;
    NZFlags flags;
};

using LDAAbsolute     = LDA<Absolute, LDA_Absolute_Expectations, 4>;
using LDAAbsoluteMode = ParameterizedInstructionExecutorTestFixture<LDAAbsolute>;


static void StoreTestValueAtEffectiveAddress(InstructionExecutorTestFixture &fixture, const LDAAbsolute &instruction_param)
{
    fixture.fakeMemory[instruction_param.address.absolute_address       ] = instruction_param.requirements.final.a;
}

static void SetupAffectedOrUsedRegisters(InstructionExecutorTestFixture &fixture, const LDAAbsolute &instruction_param)
{
    fixture.r.a = instruction_param.requirements.initial.a;
    fixture.r.SetFlag(FLAGS6502::N, instruction_param.requirements.initial.flags.n_value.expected_value);
    fixture.r.SetFlag(FLAGS6502::Z, instruction_param.requirements.initial.flags.z_value.expected_value);
}

template<>
void LoadInstructionIntoMemoryAndSetRegistersToInitialState(      InstructionExecutorTestFixture &fixture,
                                                            const LDAAbsolute                    &instruction_param)
{
    SetupRAMForInstructionsThatHaveAnEffectiveAddress(fixture, instruction_param);
    SetupAffectedOrUsedRegisters(fixture, instruction_param);
}

template<>
void RegistersAreInExpectedState(const Registers &registers,
                                 const LDA_Absolute_Expectations &expectations)
{
    EXPECT_THAT(registers.a, Eq(expectations.a));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::N), Eq(expectations.flags.n_value.expected_value));
    EXPECT_THAT(registers.GetFlag(FLAGS6502::Z), Eq(expectations.flags.z_value.expected_value));
}

template<>
void MemoryContainsInstruction(const InstructionExecutorTestFixture &fixture,
                               const Instruction<AbstractInstruction_e::LDA, Absolute> &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter ), Eq( OpcodeFor(AbstractInstruction_e::LDA, AddressMode_e::Absolute) ));
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 1), Eq( fixture.loByteOf(instruction.address.absolute_address) ));
    EXPECT_THAT(fixture.fakeMemory.at( fixture.executor.registers().program_counter + 2), Eq( fixture.hiByteOf(instruction.address.absolute_address) ));
}

template<>
void MemoryContainsExpectedComputation(const InstructionExecutorTestFixture &fixture,
                                       const LDAAbsolute                    &instruction)
{
    EXPECT_THAT(fixture.fakeMemory.at( instruction.address.absolute_address ), Eq( instruction.requirements.final.a ));
}


static const std::vector<LDAAbsolute> LDAAbsoluteModeTestValues {
LDAAbsolute{
    // Beginning of a page
    Absolute().address(0x0000).value(0xA000),
    LDAAbsolute::Requirements{
        .initial = {
            .a = 0,
            .flags = { }},
        .final = {
            .a = 6,
            .flags = { }
        }}
},
LDAAbsolute{
    // Middle of a page
    Absolute().address(0x0088).value(0xA000),
    LDAAbsolute::Requirements{
        .initial = {
            .a = 0,
            .flags = { }},
        .final = {
            .a = 6,
            .flags = { }
        }}
},
LDAAbsolute{
    // End of a page
    Absolute().address(0x00FD).value(0xA000),
    LDAAbsolute::Requirements{
        .initial = {
            .a = 0,
            .flags = { }},
        .final = {
            .a = 6,
            .flags = { }
        }}
},
LDAAbsolute{
    // Crossing a page (partial absolute address)
    Absolute().address(0x00FE).value(0xA000),
    LDAAbsolute::Requirements{
        .initial = {
            .a = 0,
            .flags = { }},
        .final = {
            .a = 6,
            .flags = { }
        }}
},
LDAAbsolute{
    // Crossing a page (entire absolute address)
    Absolute().address(0x00FF).value(0xA000),
    LDAAbsolute::Requirements{
        .initial = {
            .a = 0,
            .flags = { }},
        .final = {
            .a = 6,
            .flags = { }
        }}
},
LDAAbsolute{
    // Loading a zero affects the Z flag
    Absolute().address(0x8000).value(0xA000),
    LDAAbsolute::Requirements{
        .initial = {
            .a = 0xA0,
            .flags = { }},
        .final = {
            .a = 0,
            .flags = {
                .n_value = { .expected_value = false },
                .z_value = { .expected_value = true } }
        }}
},
LDAAbsolute{
    // Loading a negative affects the N flag
    Absolute().address(0x8000).value(0xA000),
    LDAAbsolute::Requirements{
        .initial = {
            .a = 0x10,
            .flags = { }},
        .final = {
            .a = 0xFF,
            .flags = {
                .n_value = { .expected_value = true },
                .z_value = { .expected_value = false } }
        }}
}
};

TEST_P(LDAAbsoluteMode, TypicalInstructionExecution)
{
    TypicalInstructionExecution(*this, GetParam());
}

INSTANTIATE_TEST_SUITE_P(LoadAbsoluteAtVariousAddresses,
                         LDAAbsoluteMode,
                         testing::ValuesIn(LDAAbsoluteModeTestValues) );
